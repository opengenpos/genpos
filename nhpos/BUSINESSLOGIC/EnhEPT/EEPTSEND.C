/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9              **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Enhanced EPT Send Task
* Category    : Enhanced EPT module,NCR2170 GP R2.0
* Program Name: EEPTSEND.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               EEptMainSendTask();
*               EEptSendCom();
*               EEptSend();                              (R2.0)
*               EEptCheckRcv();                          (R2.0)
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Feb-19-97 : 02.00.00 : T.Yatuhasi   : Initial
* Dec-07-99 : 01.00.00 : hrkato       : Saratoga
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

/**
;============================================================================
;                  I N C L U D E     F I L E s                              +
;============================================================================
**/
/**------- MS - C ------**/
#include	<tchar.h>

#include <memory.h>
#include <string.h>
#include <stdlib.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "plu.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "eeptl.h"
#include "appllog.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

UCHAR uchEEPTSendStatus;            /* Send Thread sub Status (R2.0) */
SHORT sEEPTSendRetryCount;          /* retry counter for receive NAK (R2.0) */

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
/**
;=============================================================================
;+              E X T E R N A L  R A M s
;=============================================================================
**/
/*
*=============================================================================
*
** Format  : VOID THREADENTRY   EEptMainSendTask(VOID);
*
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing
*
** Return  :    nothing
*
** Synopsis:    This function start of Send task.
*                                                R2.0
*=============================================================================
*/
VOID THREADENTRY    EEptMainSendTask(VOID)
{
    SHORT   sRet;
    SHORT   sMsgType;

#ifdef DEBUG
    aEEptLog.usSendthreadId = PifGetThreadId();                  /* save self thread ID  */
#endif

    for (;;) {
        PifRequestSem(usEEptSemSend);                            /* wait data is written */
		memset(&EEptSpool, 0, sizeof(EEPTSPOOL));                /* clear spool area     */

		// If we are EEPT_PORT_USENET or EEPT_PORT_USEDLL then we don't do all of the
		// handshaking and such that the serial port communications functions do.
		// Instead we just send the waiting data either over the LAN or through the
		// appropriate DLL interface.
		if (chEEptPortID == EEPT_PORT_USENET || chEEptPortID == EEPT_PORT_USEDLL) {

            EEptSetSeqNo((EEPTREQDATA *)&EEptSaveSend.auchReqData.u.eeptReqData,EEptSaveSend.usLen);
                                            /* set Sequence# & LRC */
            if (EEptSend(EEPT_REQ) == EEPT_RET_SUCCESS) {
                                            /* execute REQ send      */
               sEEPTSendRetryCount = 0;         /* initialize            */
            }
			// set the state before we do the response handling.  We do this
			// in order to prevent a run condition that can result in a false
			// EEPT_RET_BUSY indication.  The function EEptExeResponse() is
			// using PifSleep(100) in several places.
            EEptWriteStatus(EEPT_STATE_OUTSIDE);
            sRet = EEptExeResponse();             /* send response to user*/
			continue;
		}

		// Following is the legacy source code which implements the serial communications
		// protocol with an external EPT peripherial.
		/* send ENQ */
		sRet = EEptSend(EEPT_ENQ_CHAR);                           /* execute ENQ send    */
		if (sRet == EEPT_RET_SUCCESS) {
			sEEPTSendRetryCount = 0;                                /* initialize */
			uchEEPTSendStatus = EEPT_SUBSTATE_ENQACK;                /* sent ENQ and wait ACK */
		} else {
            EEptWriteStatus(EEPT_STATE_OUTSIDE);
			PifLog (MODULE_EEPT, LOG_EVENT_EPT_SEND_FAIL_ENQ);
			continue;                                              /* wait for user req     */
		}

        for (;;) {
            PifRequestSem(usEEptSemHandle);                      /* for hsEEptSemHandle   */
            if (hsEEptPortHandle == EEPT_PORT_NONE) {             /* not open comm. port? */
                EEptRecoverPort();                               /* port recover process */
            }
            PifReleaseSem(usEEptSemHandle);                      /* for hsEEptSemHandle   */

            sMsgType = EEptCreateMessage();

            switch (uchEEPTSendStatus) {
                case EEPT_SUBSTATE_ENQACK:                       /* sent ENQ and wait ACK */
                    switch(sMsgType) {
                        case EEPT_TYPE_USER_RESP:                /* response data for user*/
/*                            Para.EPTTally.usPMSThrowResp++;     * count up tally        */
                            break;                              /* ignore                */

                        case EEPT_TYPE_ERR_RESP:                 /* other data            */
                        case EEPT_TYPE_UNSOLI:                   /* unsoli. data from PMS */
                            break;                              /* ignore                */

                        case EEPT_TYPE_TRACON:                   /* transmission control char */
                            if (EEptCheckRcv(EEPT_ACK_CHAR,EEPT_ENQ_CHAR) == EEPT_RET_SUCCESS) {
                                                                /* check ack             */
                                EEptSetSeqNo((EEPTREQDATA *)&EEptSaveSend.auchReqData.u.eeptReqData,EEptSaveSend.usLen);
                                                                /* set Sequence# & LRC */
                                if (EEptSend(EEPT_REQ) == EEPT_RET_SUCCESS) {
                                                                /* execute REQ send      */
                                   uchEEPTSendStatus = EEPT_SUBSTATE_REQACK; /* sent REQ and wait ACK */
                                   sEEPTSendRetryCount = 0;         /* initialize            */
                                }
                            }
                            break;

                        case EEPT_RET_TIMEOUT:                   /* time out              */
                        default:                                /* another error         */
                            EEptExeError(sMsgType);              /* send error to user & if  */
                            break;
                    }
                    break;

                case EEPT_SUBSTATE_REQACK:                       /* sent REQ and wait ACK */
                    switch(sMsgType) {
                        case EEPT_TYPE_USER_RESP:                /* response data for user*/
/*                            Para.EPTTally.usPMSThrowResp++;     * count up tally        */
                            break;                              /* ignore                */

                        case EEPT_TYPE_ERR_RESP:                 /* other data            */
                        case EEPT_TYPE_UNSOLI:                   /* unsoli. data from PMS */
                            break;                              /* ignore                */

                        case EEPT_TYPE_TRACON:                   /* transmission control char */

                            if (EEptCheckRcv(EEPT_ACK_CHAR,EEPT_REQ) == EEPT_RET_SUCCESS) {
                                                                /* check ack             */
                                EEptWriteStatus(EEPT_STATE_SEND_OK);
                                uchEEPTSendStatus = EEPT_SUBSTATE_RESP;
                                sEEPTSendRetryCount = 0;            /* initialize            */
                            }
                            break;

                        case EEPT_RET_TIMEOUT:                   /* time out              */
                        default:                                /* another error         */
                            EEptExeError(sMsgType);              /* send error to user & if  */
                            break;
                    }
                    break;

                case EEPT_SUBSTATE_RESP:             /* sent or received ACK and wait RESP */
                    switch(sMsgType) {
                        case EEPT_TYPE_USER_RESP:            /* response data for user    */
                            if (EEptCheckSpoolData() != EEPT_SAME) { /* check send add     */
/*                                 Para.EPTTally.usPMSThrowResp++; * count up tally       */
                                 break;                          /* if during this status*/
                            }                                    /* message is thrown    */
                            Para.EPTTally.usEPTRespOK++;        /* count up tally       */
                            sRet = EEptExeResponse();             /* send response to user*/

                            if(EEptSend(EEPT_ACK_CHAR) == EEPT_RET_SUCCESS) {
                                                                 /* execute ACK send    */
                                if(sRet == EEPT_END) {            /* last message */
                                    uchEEPTSendStatus = EEPT_SUBSTATE_ACKEOT;
                                }
                            }
                            break;

                        case EEPT_TYPE_UNSOLI:                    /* unsoli. data from PMS */
                        case EEPT_TYPE_TRACON:                    /* transmission control char */
                            break;                               /* ignore               */

                        case EEPT_TYPE_ERR_RESP:                  /* other data           */
                        case EEPT_RET_LENGTH:                     /* received data length error */
                                                                 /* (include LRC error (R2.0)) */
                            EEptSend(EEPT_NAK_CHAR);               /* send NAK */
                            break;


                        case EEPT_RET_TIMEOUT:                    /* time out */
                            Para.EPTTally.usEPTRespErr++;           /* count up tally       */
                        default:                                 /* another error        */

                            EEptExeError(sMsgType);               /* send error to user & if  */
                                                                 /* err tally count up   */
                            break;

                    }
                    break;

                case EEPT_SUBSTATE_ACKEOT:                       /* sent ACK and wait EOT */
                    switch(sMsgType) {
                        case EEPT_TYPE_USER_RESP:                /* response data for user*/
                        case EEPT_TYPE_ERR_RESP:                 /* other data            */
                        case EEPT_TYPE_UNSOLI:                   /* unsoli. data from PMS */
                            break;                              /* ignore                */

                        case EEPT_TYPE_TRACON:                   /* transmission control char */
                            if (EEptSpool.auchData[0] == EEPT_EOT_CHAR) {
                               EEptWriteStatus(EEPT_STATE_OUTSIDE);
                            }
                            break;

                        case EEPT_RET_TIMEOUT:                   /* time out              */
                        default:                                /* another error         */
                             EEptWriteStatus(EEPT_STATE_OUTSIDE);
                    }
                    break;

				default:
					PifLog (MODULE_EEPT, LOG_EVENT_EPT_BAD_STATE_02);
					break;
            } // switch (uchEEPTSendStatus)

            PifRequestSem(usEEptSemState);                   /* for uchEEptStatus     */
            if (uchEEptStatus == EEPT_STATE_OUTSIDE) {        /* finish process        */
                PifReleaseSem(usEEptSemState);               /* for uchEEptStatus     */
                break;
            }
            PifReleaseSem(usEEptSemState);                   /* for uchEEptStatus     */

        }
    }
}

/*
*=============================================================================
** Format  : SHORT   EEptSendCom(VOID);
*
*    Input : none
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*
** Synopsis: This function sends data to PMS (RS-232 Port).
*=============================================================================
*/
SHORT   EEptSendCom(VOID)
{
    SHORT  sRet;

    sRet = EEptEEptSendCom();

#ifdef DEBUG
    EEptLog(EEPT_MODULE_SENDCOM,sRet,0);
#endif

    return(sRet);

}
/*
*=============================================================================
** Format  : SHORT   EEptEEptSendCom(VOID);
*
*    Input : none
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*
** Synopsis: This function sends data to PMS (RS-232 Port).
*=============================================================================
*/
SHORT   EEptEEptSendCom(VOID)
{
    SHORT  sRet;
    DATE_TIME   Timer;
	EEPTREQDATA_v14	EEptReqV14;

    PifRequestSem(usEEptSemHandle);                  /* for usEEptStatus      */
    if (hsEEptPortHandle == EEPT_PORT_NONE) {         /* if RS-232 port open? */
        sRet = EEptRecoverPort();                    /* port recovery (R2.0) */
        if (sRet == EEPT_RET_FAIL) {                 /* RS-232 port not open */
            PifReleaseSem(usEEptSemHandle);          /* for usEEptStatus(R2.0)*/
            return(EEPT_RET_OFFLINE);                /* return port open err */
        }
    }

	// RJC EEPT modified this section of code for sending to the EEPT host
	// to either use RS-232 port or the LAN depending on system configuration.
	if (hsEEptPortHandle == EEPT_PORT_USENET) {
		sRet = EEptHostSend((VOID *)&EEptSaveSend.auchReqData.u.eeptReqData, EEptSaveSend.usLen);          /* send data to port    */

		// If there was not error with the send then we indicate it was
		// successful by setting the sRet to CPM_RET_SUCCESS.
		// This in turn will trigger the state change to CPM_STATE_SEND_OK
		// which is needed for the further process of the request/response
		// cycle.
		if (sRet >= 0)
			sRet = EEPT_RET_SUCCESS;

		PifGetDateTime(&Timer);                         /* get current time     */
		EEptSendTOD.uchHour = (UCHAR)Timer.usHour;
		EEptSendTOD.uchMin  = (UCHAR)Timer.usMin;
		EEptSendTOD.uchSec  = (UCHAR)Timer.usSec;
		if (sRet != EEPT_RET_SUCCESS) {
			//SR368
			sRet = EEPT_RET_OFFLINE;
		} else {
			uchEEptStatus = EEPT_STATE_SEND_OK;           /* status change        */
		}
	}
	else if(hsEEptPortHandle == EEPT_PORT_USEDLL){
		//Use loaded Dll
		sRet = XEptDllSend((VOID*)&EEptSaveSend.auchReqData, EEptSaveSend.usLen);
		PifGetDateTime(&Timer);                         /* get current time     */
		EEptSendTOD.uchHour = (UCHAR)Timer.usHour;
		EEptSendTOD.uchMin  = (UCHAR)Timer.usMin;
		EEptSendTOD.uchSec  = (UCHAR)Timer.usSec;

		uchEEptStatus = EEPT_STATE_SEND_OK;           /* status change        */
		sRet = EEPT_RET_SUCCESS;
	}
	else {//Use RS-232 Port

		memset(&EEptReqV14,' ',sizeof(EEptReqV14));

		EEptSetSendRS232((EEPTREQDATA *)&EEptSaveSend.auchReqData.u.eeptReqData, &EEptReqV14);
		
		memset(&EEptSaveSend.auchReqData.u.eeptReqData, ' ', EEptSaveSend.usLen);
		memcpy(&EEptSaveSend.auchReqData.u.eeptReqData, &EEptReqV14, sizeof(EEptReqV14));

		EEptSaveSend.usLen = sizeof(EEptReqV14);

		sRet = PifWriteCom(hsEEptPortHandle, (VOID *)&EEptSaveSend.auchReqData.u.eeptReqData, EEptSaveSend.usLen);          /* send data to port    */

		PifGetDateTime(&Timer);                         /* get current time     */
		EEptSendTOD.uchHour = (UCHAR)Timer.usHour;
		EEptSendTOD.uchMin  = (UCHAR)Timer.usMin;
		EEptSendTOD.uchSec  = (UCHAR)Timer.usSec;

		sRet = EEptCheckComError(sRet, EEptSaveSend.usLen);
														/* check PifWriteCom()  */
		if (sRet != EEPT_RET_SUCCESS) {
			//SR368
			sRet = EEPT_STATE_ERROR;
		} else {
			uchEEptStatus = EEPT_STATE_SEND_OK;           /* status change        */
		}
	}
                                                    /* check PifWriteCom()  */
	PifReleaseSem(usEEptSemHandle);                  /* for usEEptStatus      */
	return(sRet);
}

VOID EEptSetSendRS232(EEPTREQDATA *pEEPTReq, EEPTREQDATA_v14* pEEPTReqv14)
{	
	pEEPTReqv14->uchSTX = pEEPTReq->uchSTX;

	memcpy(pEEPTReqv14->auchSrcTerminal,pEEPTReq->auchSrcTerminal,sizeof(pEEPTReqv14->auchSrcTerminal));
	memcpy(pEEPTReqv14->auchDesTerminal,pEEPTReq->auchDesTerminal,sizeof(pEEPTReqv14->auchDesTerminal));
	memcpy(pEEPTReqv14->auchTenderID,pEEPTReq->auchTenderID,sizeof(pEEPTReqv14->auchTenderID));
	memcpy(pEEPTReqv14->auchDataSeqNo,pEEPTReq->auchDataSeqNo,sizeof(pEEPTReqv14->auchDataSeqNo));

	pEEPTReqv14->auchFuncCode = pEEPTReq->auchFuncCode;

	pEEPTReqv14->auchTerminalNo = pEEPTReq->auchTerminalNo;

	memcpy(pEEPTReqv14->auchSystemNo,pEEPTReq->auchSystemNo,sizeof(pEEPTReqv14->auchSystemNo));
	memcpy(pEEPTReqv14->auchStoreNo,pEEPTReq->auchStoreNo,sizeof(pEEPTReqv14->auchStoreNo));
	memcpy(pEEPTReqv14->auchHotelID,pEEPTReq->auchHotelID,sizeof(pEEPTReqv14->auchHotelID));
	memcpy(pEEPTReqv14->auchGuest,pEEPTReq->auchGuest,sizeof(pEEPTReqv14->auchGuest));
	memcpy(pEEPTReqv14->auchChargeTtl,pEEPTReq->auchChargeTtl,sizeof(pEEPTReqv14->auchChargeTtl));
	memcpy(pEEPTReqv14->auchTransNo,pEEPTReq->auchTransNo,sizeof(pEEPTReqv14->auchTransNo));

	pEEPTReqv14->auchCPFlag = pEEPTReq->auchCPFlag;

	memcpy(pEEPTReqv14->auchSLDNumber,pEEPTReq->auchSLDNumber,sizeof(pEEPTReqv14->auchSLDNumber));
	memcpy(pEEPTReqv14->auchMSRData,pEEPTReq->auchMSRData,sizeof(pEEPTReqv14->auchMSRData));
	memcpy(pEEPTReqv14->auchExpDate,pEEPTReq->auchExpDate,sizeof(pEEPTReqv14->auchExpDate));
	memcpy(pEEPTReqv14->auchGuestLineNo,pEEPTReq->auchGuestLineNo,sizeof(pEEPTReqv14->auchGuestLineNo));
	memcpy(pEEPTReqv14->auchCashierNo,pEEPTReq->auchCashierNo,sizeof(pEEPTReqv14->auchCashierNo));
	memcpy(pEEPTReqv14->auchChargeTip,pEEPTReq->auchChargeTip,sizeof(pEEPTReqv14->auchChargeTip));
	memcpy(pEEPTReqv14->auchTtlTax,pEEPTReq->auchTtlTax,sizeof(pEEPTReqv14->auchTtlTax));
	memcpy(pEEPTReqv14->auchAcctNo,pEEPTReq->auchAcctNo,sizeof(pEEPTReqv14->auchAcctNo));
	memcpy(pEEPTReqv14->auchActionCode,pEEPTReq->auchActionCode,sizeof(pEEPTReqv14->auchActionCode));
	memcpy(pEEPTReqv14->auchDiscItemTtl1,pEEPTReq->auchDiscItemTtl1,sizeof(pEEPTReqv14->auchDiscItemTtl1));
	memcpy(pEEPTReqv14->auchDiscItemTtl2,pEEPTReq->auchDiscItemTtl2,sizeof(pEEPTReqv14->auchDiscItemTtl2));
	memcpy(pEEPTReqv14->auchBonus1,pEEPTReq->auchBonus1,sizeof(pEEPTReqv14->auchBonus1));
	memcpy(pEEPTReqv14->auchBonus2,pEEPTReq->auchBonus2,sizeof(pEEPTReqv14->auchBonus2));
	memcpy(pEEPTReqv14->auchBonus3,pEEPTReq->auchBonus3,sizeof(pEEPTReqv14->auchBonus3));
	memcpy(pEEPTReqv14->auchBonus4,pEEPTReq->auchBonus4,sizeof(pEEPTReqv14->auchBonus4));
	memcpy(pEEPTReqv14->auchBonus5,pEEPTReq->auchBonus5,sizeof(pEEPTReqv14->auchBonus5));
	memcpy(pEEPTReqv14->auchBonus6,pEEPTReq->auchBonus6,sizeof(pEEPTReqv14->auchBonus6));
	memcpy(pEEPTReqv14->auchBonus7,pEEPTReq->auchBonus7,sizeof(pEEPTReqv14->auchBonus7));
	memcpy(pEEPTReqv14->auchBonus8,pEEPTReq->auchBonus8,sizeof(pEEPTReqv14->auchBonus8));
	memcpy(pEEPTReqv14->auchFreq,pEEPTReq->auchFreq,sizeof(pEEPTReqv14->auchFreq));
	memcpy(pEEPTReqv14->auchFoodStamp,pEEPTReq->auchFoodStamp,sizeof(pEEPTReqv14->auchFoodStamp));

	pEEPTReqv14->uchETX = pEEPTReq->uchETX;

	pEEPTReqv14->uchLRC = pEEPTReq->uchLRC;

}

/*
*=============================================================================
** Format  : SHORT   EEptSend(UCHAR uchDataType);
*
*    Input : uchDataType    : send data type
*                              EEPT_ACK_CHAR   * Acknowledge  char         (ACK) *
*                              EEPT_NAK_CHAR   * Negative Acknowledge char (NAK) *
*                              EEPT_ENQ_CHAR   * Enquiry char              (ENQ) *
*                              EEPT_EOT_CHAR   * End of Transmission char  (EOT) *
*                              EEPT_REQ        * Request data                    *
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*
*
** Synopsis: This function sends data to PMS (RS-232 Port). [for Send Thread]
*                                                                R2.0
*=============================================================================
*/
SHORT   EEptSend(UCHAR uchDataType)
{
    USHORT  i, usIntvlTimer;
    SHORT   sRet = EEPT_RET_BUSY;

    usIntvlTimer = EEptGetMDCTime(MDC_EPT_SND_TIME);
    for (i=0; i < EEPT_RETRY_COUNT; i++) {       /* loop for retry       */
        switch(uchDataType) {
            case EEPT_ACK_CHAR:
                sRet = EEptSendComTC(EEPT_ACK_CHAR); /* send ACK          */
                break;
            case EEPT_NAK_CHAR:
                sRet = EEptSendComTC(EEPT_NAK_CHAR); /* send NAK          */
                break;
            case EEPT_ENQ_CHAR:
                sRet = EEptSendComTC(EEPT_ENQ_CHAR); /* send ENQ          */
                break;
            case EEPT_EOT_CHAR:
                sRet = EEptSendComTC(EEPT_EOT_CHAR); /* send EOT          */
                break;
            case EEPT_REQ:
                sRet = EEptSendCom();               /* send Request data */
                break;
			default:
				PifLog (MODULE_EEPT, LOG_EVENT_EPT_SEND_BAD_DATA_01);
				sRet = EEPT_RET_BUSY;
				break;
        }
        if (sRet == EEPT_RET_SUCCESS) {
            if (uchDataType == EEPT_REQ) {
                Para.EPTTally.usEPTSendOK++;   /* incremnt tally count */
            }
            break;
        } else { //SR368
			if ( (sRet == EEPT_RET_BUSY) ||
				 (sRet == EEPT_RET_LENGTH) ||
				 (sRet == EEPT_RET_TIMEOUT)) {

					if ( i == (EEPT_RETRY_COUNT-1)) {    /* retry last time ?    */
														/* if request from self */
						EEptSpool.sErrCode = sRet;
						PifReleaseSem(usEEptSemRcv);     /* release for SELF term*/
						PifRequestSem(usEEptSemSpool);
														/* request for EEptSpool */
						EEptWriteStatus(EEPT_STATE_OUTSIDE);
														/* change idle status   */
						if (uchDataType == EEPT_REQ) {
							Para.EPTTally.usEPTSendErr++;   /* incremnt tally count */
						}
						break;
					}  else {
						PifSleep((USHORT)(usIntvlTimer*1000));  /* wait interval time,  Saratoga */
					}
			} else {
				// Return messages from any server should not shut down charge
				// posting or the electronic payment interface. We only allow
				// certain messages to be re-sent when an error occurs. All errors
				// returned from the DLL interfaces are never re-sent automatically.
				// Instead we increment the error count and allow the user to operator
				// of the terminal to allow when to resend.
				/* change idle status   */
				if (uchDataType == EEPT_REQ) {
					Para.EPTTally.usEPTSendErr++;   /* incremnt tally count */
				}

				break;
			}
        }
    }
    return(sRet);
}

/*
*=============================================================================
** Format  : SHORT   EEptCheckRcv(UCHAR uchExpDataType,UCHAR uchRetryDataType);
*
*    Input : uchExpDataType    : Expect data type
*    Input : uchRetryDataType  : Retry data type
*
*                              EEPT_ACK_CHAR   * Acknowledge  char         (ACK) *
*                              EEPT_NAK_CHAR   * Negative Acknowledge char (NAK) *
*                              EEPT_ENQ_CHAR   * Enquiry char              (ENQ) *
*                              EEPT_EOT_CHAR   * End of Transmission char  (EOT) *
*                              EEPT_REQ        * Request data                    *
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*             EEPT_RETRY_COUNT    NAK receive time out
*
*
** Synopsis: This function check receive data . [for Send Thread]
*                                                                R2.0
*=============================================================================
*/
SHORT   EEptCheckRcv(UCHAR uchExpDataType,UCHAR uchRetryDataType)
{
    SHORT   sRet=EEPT_RET_SUCCESS;

    if (EEptSpool.auchData[0] != uchExpDataType) {
        if (EEptSpool.auchData[0] == EEPT_NAK_CHAR) {
            if (sEEPTSendRetryCount++ < EEPT_RETRY_COUNT) {
                sRet = EEptSend(uchRetryDataType);  /* retry to send    */
            } else {
                EEptSend(EEPT_NAK_CHAR);             /* send NAK */
                sRet = EEPT_RET_TIMEOUT;
            }
        }
    }
    return(sRet);
}
/*
*=============================================================================
** Format  : VOID   EEptSetSeqNo(EEPTREQDATA *pSendData,USHORT usLen);
*
*    Input : pSendData  :pointer of send message
*            usLen      :send message data length
*    InOut : none
*
**   Return : none
*
** Synopsis: This function set Sequence# & LRC.
*=============================================================================
*/
VOID   EEptSetSeqNo(EEPTREQDATA *pSendData,USHORT usLen)
{
    UCHAR        aszWork[4];
    SHORT        i;
    UCHAR        uchLRC;
    UCHAR        *pMes;

    if (usLen == sizeof(EEPTREQDATA)) {
        memset(&(pSendData->auchDataSeqNo[0]),'0',sizeof(pSendData->auchDataSeqNo));
        _itoa(usEEptSeqNo, aszWork, 10);                /* Saratoga */
        for (i=1; i<=sizeof(aszWork); i++) {
            if (aszWork[i] == 0) {
                memcpy(&(pSendData->auchDataSeqNo[3-i]), aszWork, i);
                break;
            }
        }
        if (usEEptSeqNo++ >= 999 ) {                  /* increment EEpt SeqNo */
            usEEptSeqNo = 1;
        }

        pMes = (UCHAR *)pSendData;
        for (i=0,uchLRC=0;i < (SHORT)(usLen - 1);i++,pMes++) {
            uchLRC ^= *pMes;                       /* calculate LRC        */
        }
        pSendData->uchLRC = uchLRC;                /* set LRC              */
    }
}

/* ---------- End of EEPTSEND.C ---------- */