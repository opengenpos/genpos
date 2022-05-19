/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9              **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Enhanced EPT Receive Task
* Category    : Enhanced EPT module,NCR2170 GP R2.0
* Program Name: EEPTRCV.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               EEptMainReceiveTask()
*                   EEptOpenSetPort();
*                   EEptCreateMessage();
*                       EEptSetReadTimer();
*                       EEptReceiveCom();
*                           EEptRecoverPort();
*                           EEpt1ByteTimeOut();
*                           EEpt1BytePowerOff();
*                       EEptCheckMessageType();
*                   EEptExeResponse();
*                   EEptExeUnsoli();
*                   EEptCreateUnsMessage();
*
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Feb-19-97 : 02.00.00 : T.Yatuhasi   : Initial
* Aug-24-98 : 02.00.13 : M.Suzuki     : Correct wait time (TAR 85352)
* Feb-22-99 : 02.00.14 : A.Mitsui     : Swicth order EptStatus
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

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "plu.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "pifmain.h"
#include "csstbpar.h"
#include "eeptl.h"
#include "prevept.h"
#include "rfl.h"
#include "appllog.h"
/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

EEPTSPOOL    EEptSpool;                               /* read data save area  */
USHORT       usEEptWaitTime;                          /* wait timer for Read  */
UCHAR        uchEEPTReceiveStatus;                    /* receive thread sub status (R2.0) */
UCHAR        uchEEPTNewState;                         /* new status save area (R2.0) */
SHORT        sEEPTUnsControl;                         /* unsoli rep func control(R2.0) */


/*
*=============================================================================
*
** Format  : VOID THREADENTRY   EEptMainReceiveTask(VOID);
*
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing
*
** Return  :    nothing
*
** Synopsis:    This function is the main loop for the EEPT message
*				receiving thread.  This loop runs as a background process to accept
*				messages from the EEPT Host and providing those messages to
*				the NeighborhoodPOS EEPT system such as Tender.
*
*				There are two main kinds of messages from the EEPT Host:
*					EEPT_TYPE_USER_RESP - response to a user request
*						Normally used to respond to a user charge request, accept or decline.
*					EEPT_TYPE_UNSOLI - an unsolicited or autonomous message from the Host
*						Normally used to indicate status changes for the EEPT Host.
*
*=============================================================================
*/
VOID THREADENTRY    EEptMainReceiveTask(VOID)
{
    SHORT       sMsgType;
    UCHAR       uchSaveState = EEPT_STATE_OUTSIDE;
    SHORT       sRet;

#ifdef DEBUG
    aEEptLog.usReceivethreadId = PifGetThreadId();                    /* save self thread ID  */
#endif

    for (;;) {

        PifRequestSem(usEEptSemState);                                /* for uchEEptStatus     */

        switch(uchEEptStatus) {
            case EEPT_STATE_OUTSIDE:                                  /* ready  */

//				PifReleaseSem(usEEptSemState);
                /* ----- receive wait time bugfix 99/2/22 start ----- */
                sMsgType = EEptCreateUnsMessage();                    /* create message by RS-232 port (1sec) */

                uchSaveState = uchEEptStatus;                       /* save current status  */
                uchEEptStatus = EEPT_STATE_UNSOLI;                  /* change processing unsoli func. */
                /* ----- receive wait time bugfix 99/2/22 end ----- */

                switch (sMsgType) {
                    case EEPT_TYPE_USER_RESP:
                        uchEEptStatus = uchSaveState;                 /* RESTORE STATUS */
                        PifReleaseSem(usEEptSemState);                /* for uchEEptStatus     */
/*                        Para.EPTTally.usPMSThrowResp++;              * count up tally       */
                        PifSleep(EEPT_WAIT_RECEIVE*1000);
                        break;                                       /* if during this status*/

                    case EEPT_TYPE_TRACON:                            /* transmission control char */
                        if (EEptSpool.auchData[0] == EEPT_ENQ_CHAR) {
                            sRet = EEptSendComTC(EEPT_ACK_CHAR);       /* send Acknowledge char (ACK) */
                            if (sRet == EEPT_RET_SUCCESS) {
                                sEEPTUnsControl = EEPT_UNSOLI_1ST;        /* initialize */
                                uchEEPTReceiveStatus = EEPT_SUBSTATE_REQ; /* sent ACK and wait REQ */
                                PifReleaseSem(usEEptSemState);        /* for uchEEptStatus     */
                            } else {
                                uchEEptStatus = uchSaveState;         /* RESTORE STATUS */
                                PifReleaseSem(usEEptSemState);        /* for uchEEptStatus     */
                                PifSleep(EEPT_WAIT_RECEIVE*1000);
                            }
                        } else {
                            uchEEptStatus = uchSaveState;             /* RESTORE STATUS */
                            PifReleaseSem(usEEptSemState);            /* for uchEEptStatus     */
                            PifSleep(EEPT_WAIT_RECEIVE*1000);
                        }
                        break;

                    default:
                        uchEEptStatus = uchSaveState;                 /* RESTORE STATUS */
                        PifReleaseSem(usEEptSemState);                /* for uchEEptStatus     */
                        PifSleep(EEPT_WAIT_RECEIVE*1000);
                        break;
                }
                break;

            case EEPT_STATE_SEND_OK:                                 /* wait response data   */
            case EEPT_STATE_INSIDE:                                  /* sending request data */
                PifReleaseSem(usEEptSemState);                       /* for uchEEptStatus     */
                PifSleep(EEPT_WAIT_RECEIVE*1000);                    /* sleep   */
                break;

            case EEPT_STATE_UNSOLI:                                  /* processing unsoli func.*/
                PifReleaseSem(usEEptSemState);                       /* for uchEEptStatus     */
                PifRequestSem(usEEptSemHandle);                      /* for hsEEptSemHandle   */
                if (hsEEptPortHandle == EEPT_PORT_NONE) {             /* not open comm. port? */
                    EEptRecoverPort();                               /* port recover process */
                }
                PifReleaseSem(usEEptSemHandle);                      /* for hsEEptSemHandle   */
                sMsgType = EEptCreateMessage();
                switch (sMsgType) {
#if 0       /* Saratoga */
                    case EEPT_TYPE_UNSOLI:
                        if (uchEEPTReceiveStatus == EEPT_SUBSTATE_REQ) {
                            if (EEptExeUnsoli(uchSaveState) == EEPT_UNSOLI_OK) {
                                                                    /* complite unsoli request */
/*                                Para.EPTTally.usPMSRecUnsoli++;     * count up tally       */
                            } else {                                /* incomplite unsoli request */
                                sRet = EEptSendComTC(EEPT_NAK_CHAR);  /* send NAK */
                            }

                        }
                        break;
#endif
                    case EEPT_TYPE_USER_RESP:
/*                        Para.EPTTally.usPMSThrowResp++;              * count up tally       */
                        break;                                       /* if during this status*/

                    case EEPT_TYPE_TRACON:                            /* transmission control char */
                        if (EEptSpool.auchData[0] == EEPT_EOT_CHAR) {
                            if(uchEEPTReceiveStatus == EEPT_SUBSTATE_ACKEOT) {
                                EEptWriteStatus(uchEEPTNewState);         /* change saved new state */
                            } else {
                                EEptWriteStatus(uchSaveState);            /* change saved previous state */
                            }

                        } else {
                            if (EEptSpool.auchData[0] == EEPT_ENQ_CHAR) {
                                sRet = EEptSendComTC(EEPT_ACK_CHAR);   /* send Acknowledge char (ACK) */
                            }
                        }
                        break;

                    case EEPT_RET_TIMEOUT:
                        if (uchEEPTReceiveStatus == EEPT_SUBSTATE_ACKEOT) {
                            EEptWriteStatus(uchEEPTNewState);             /* change saved new state */
                        } else {
                            EEptWriteStatus(uchSaveState);            /* change saved previous state */
                        }
                        break;

                    case EEPT_RET_LENGTH:                             /* received data length error */
                                                                     /* (include LRC error) */
                        if (uchEEPTReceiveStatus == EEPT_SUBSTATE_REQ) {
                            EEptSendComTC(EEPT_NAK_CHAR);              /* send NAK */
                        }
                        break;

                    default:
						PifLog (MODULE_EEPT, LOG_EVENT_EPT_BAD_STATE_03);
                        EEptWriteStatus(uchSaveState);                /* change saved previous state */
                        break;
                }

            default:
                PifReleaseSem(usEEptSemState);                       /* for uchEEptStatus     */
				PifLog (MODULE_EEPT, LOG_EVENT_EPT_BAD_STATE_01);
                PifSleep(EEPT_WAIT_RECEIVE*1000);                    /* sleep   */
                break;
        } // switch(uchEEptStatus)
    } // for (;;)
}

/*
*=============================================================================
** Format  : SHORT  EEptOpenSetPort(USHORT usWaitTimer)
*
*    Input : ulWaitTimer -Wait timer value (NN sec.)
*   Output : none
*    InOut : none
*
** Return  : EEPT_RET_SUCCESS    - open port success
*            sRet               - return open failure
*
** Synopsis: This function is open port and set status.
*
*=============================================================================
*/
SHORT   EEptOpenSetPort(USHORT usWaitTimer)
{
    SHORT   i, sRet;

    if (usWaitTimer > EEPT_MAX_SET_TIMER) {
        usWaitTimer = EEPT_MAX_SET_TIMER * 1000;     /* set max msec.        */
    } else {
        usWaitTimer *= 1000;                        /* set msec.            */
    }

	// RJC added following if for LAN support of Charge Post
	if (chEEptPortID == EEPT_PORT_USENET) {
		hsEEptPortHandle = EEPT_PORT_USENET;  // indicate we are using LAN
		return PIF_OK;
	}
	else if (chEEptPortID == EEPT_PORT_USEDLL) {
		hsEEptPortHandle = EEPT_PORT_USEDLL;  // indicate we are using DLL
		return PIF_OK;
	}

    sRet = hsEEptPortHandle;
    for (i = 0; i < EEPT_WAIT_PORT; i++) {
        sRet = hsEEptPortHandle;
        if (hsEEptPortHandle < 0) {                  /* not open comm. port? */
            sRet = PifOpenCom(chEEptPortID, &EEptProtocol);
                                                    /* executes open port   */
        }
        if (sRet >= 0) {                            /* open port success ?  */

            hsEEptPortHandle = sRet;                 /* set handle value     */
            sRet = PifControlCom(hsEEptPortHandle,   /* set receive time     */
                                 PIF_COM_SET_TIME,
                                 (USHORT FAR *)&usWaitTimer);

            if (sRet == PIF_ERROR_COM_POWER_FAILURE) {
                                                    /* power down error ?   */
                PifCloseCom(hsEEptPortHandle);       /* port close           */
                hsEEptPortHandle = EEPT_PORT_NONE;    /* reset handle value   */
                continue;                           /* retry port open      */

            } else if (sRet > 0) {
                return(EEPT_RET_SUCCESS);            /* return open success  */
            }
        }
        PifSleep(EEPT_WAIT_PORT_TIME*1000);
    }

	PifLog (MODULE_EEPT, LOG_EVENT_EPT_PORT_OPN_ERR_02);
    hsEEptPortHandle = EEPT_PORT_NONE;                /* reset handle value   */
    return(sRet);                                   /* return open failure  */

}

/*
*=============================================================================
*
** Format  :    SHORT   EEptCreateMessage(VOID);
*
*    Input :    nothing
*
*   Output :    usMsgType;
*    InOut :    nothing
*
** Return  :    EEPT_TYPE_USER_RESP  - response message for user.
*               EEPT_TYPE_UNSOLI     - unsoli. message from PMS.
*               EEPT_TYPE_TRACON     - transmission control char(R2.0)
*               < 0                 - error message for user.
*
** Synopsis:    This function create message by RS-232 port.
*
*=============================================================================
*/
SHORT   EEptCreateMessage(VOID)
{
    UCHAR   i = 1;                                  /* for other task       */
    SHORT   sCurrent = EEPT_NOISE;

    for (;;) {
        EEptSetReadTimer(sCurrent);                  /* set for PifReadCom() */

        sCurrent = EEptReceiveCom(sCurrent);         /* read 1 byte data     */

        if (sCurrent < 0) {
            return(sCurrent);                       /* return err message   */
        }
        switch (sCurrent ) {
            case EEPT_NOISE:                         /* unnecessary data ?   */
                if (i > EEPT_SLEEP_COUNT) {          /* normal loop          */
                    PifSleep(EEPT_SLEEP_TIME*1000);  /* sleep for other task */
                    i = 1;                          /* reset counter        */
                    break;
                } else {
                    i++;                            /* count up             */
                    break;
                }
                                                    /* check message type   */
            case EEPT_END:                           /* created message !    */
                if(! CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT0)
                    &&(EEptSpool.usLen == sizeof(EPTRSPDATA))) {    /* EPT RESP MSG of US R1.1 type */
                    return(EEPT_TYPE_USER_RESP);
                }
                return(EEptCheckMessageType(EEptSpool.auchData));

            case EEPT_TRACON:                        /* transmission control char */
                return(EEPT_TYPE_TRACON);
/* R2.0 End   */

        }
    }
}

/*
*=============================================================================
** Format  : SHORT   EEptSetReadTimer(SHORT sCurrent);
*
*    Input : sCurrent   current status during reading
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to recover port
*             EEPT_RET_FAIL       -fail to recover port
*
** Synopsis: This function close RS-232 port and open again.
*
*=============================================================================
*/
SHORT   EEptSetReadTimer(SHORT sCurrent)
{
    USHORT  usTime;
    SHORT   sRet;

    if (sCurrent == EEPT_CONT) {
        usTime = EEPT_FIX_TIME;
    } else {
        usTime = EEptGetMDCTime(MDC_EPT_RCV_TIME);
    }

    PifRequestSem(usEEptSemHandle);
    sRet = EEptOpenSetPort(usTime);                  /* open port & set time */
    PifReleaseSem(usEEptSemHandle);
    return(sRet);
}

/*
*=============================================================================
** Format  : SHORT   EEptReceiveCom(SHORT sCurrent)
*
*    Input : sCurrent   current status
*    InOut : none
*
** Return  : EEPT_CONT        : data is continue on message
*            EEPT_END         : data is last on message
*            EEPT_NOISE       : unnecessary data
*            EEPT_TRACON      : transmission control char             (R2.0)
**           EEPT_RET_LENGTH  : received data length error(include LRC error (R2.0))
*            EEPT_RET_TIMEOUT : timeout err
*            EEPT_RET_OFFLINE : offline err
*            EEPT_RET_FAIL    : other error
*
** Synopsis: This function receives 1byte data from RS-232 Port, and spool
*            static area.
*=============================================================================
*/
SHORT   EEptReceiveCom(SHORT sCurrent)
{

    SHORT   sRet;

    sRet = EEptEEptReceiveCom(sCurrent);

#ifdef DEBUG
    EEptLog(EEPT_MODULE_RECEIVECOM,sRet,0);
#endif

    return(sRet);
}
/*
*=============================================================================
** Format  : SHORT   EEptEEptReceiveCom(SHORT sCurrent)
*
*    Input : sCurrent   current status
*    InOut : none
*
** Return  : EEPT_CONT        : data is continue on message
*            EEPT_END         : data is last on message
*            EEPT_NOISE       : unnecessary data
*            EEPT_TRACON      : transmission control char             (R2.0)
**           EEPT_RET_LENGTH  : received data length error(include LRC error (R2.0))
*            EEPT_RET_TIMEOUT : timeout err
*            EEPT_RET_OFFLINE : offline err
*            EEPT_RET_FAIL    : other error
*
** Synopsis: This function receives 1byte data from RS-232 Port, and spool
*            static area.
*=============================================================================
*/
SHORT   EEptEEptReceiveCom(SHORT sCurrent)
{
    SHORT   sRet, sRet1Byte;
    UCHAR   uchLRC;                                 /* R2.0 */
    USHORT  usLen = 0;                              /* R2.0 */
	UCHAR   uchEEptStatusSave;
	SHORT	sRetry = 5;
	EEPTRSPDATA		EEptRsp;

	// The following are some basic data integrity checks in case people change #defines
	// If any of these fail, PIF_ERROR_NET_TIMEOUT value conflicts with other return values
	// and the receive loop may not function correctly.
#if PIF_ERROR_NET_TIMEOUT == EEPT_RET_TIMEOUT
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == EEPT_RET_TIMEOUT")
#endif

#if PIF_ERROR_NET_TIMEOUT == EEPT_RET_FAIL
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == EEPT_RET_FAIL")
#endif

#if PIF_ERROR_NET_TIMEOUT == EEPT_END
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == EEPT_END")
#endif

	if (hsEEptPortHandle == EEPT_PORT_USENET) {
		UCHAR   *puchP;
		memset(&EEptSpool, 0, sizeof(EEPTSPOOL));         /* clear spool area     */
		puchP = &EEptSpool.auchData[0];                  /* R2.0 */

		sRet = PIF_ERROR_NET_TIMEOUT;
		while (sRet == PIF_ERROR_NET_TIMEOUT) {

			// We use this semaphore which gates access to the uchEEptStatus global
			// variable as a means to synchronize sends and receives of requests/responses
			// for users between the sending code in eeptsend.c and the receiving code
			// here.  This will ensure that a receive will be synchroized with a user
			// send or if an unsolicited message arrives, we will hande that.
			// The global variable uchEEptStatus tells us whether we are expecting a
			// response or not.
			PifRequestSem(usEEptSemHandle);                  /* for usCpmStatus      */
			sRet = EEptHostRecv(EEptSpool.auchData, sizeof(EEptSpool.auchData));
			uchEEptStatusSave = uchEEptStatus;
		    PifReleaseSem(usEEptSemHandle);                  /* for usCpmStatus      */
			if (sRet < 0) {
				EEptSpool.usLen = 0;
				if (uchEEptStatusSave == EEPT_STATE_SEND_OK) {
					sRetry--;
					if(sRetry < 0){
						switch (sRet) {
							case PIF_ERROR_NET_TIMEOUT:
								sRet = EEPT_RET_TIMEOUT;
								break;
							default:
								sRet = EEPT_RET_FAIL;
								break;
						}
					}
				}else{
					sRetry = 5;
				}
			}
			else {
				EEptSpool.usLen = sRet;
				sRet = EEPT_END;
			}
		}
		return sRet;
	}
	//USE
	else if(hsEEptPortHandle == EEPT_PORT_USEDLL){
		// this is not used if the EPT interface is using a DLL such as DataCap control
//			PifRequestSem(usEEptSemHandle);
//			uchEEptStatusSave = uchEEptStatus;
//			PifReleaseSem(usEEptSemHandle);
			return EEPT_RET_TIMEOUT;
	}
	else {
		UCHAR   *puchP;
		memset(&EEptSpool, 0, sizeof(EEPTSPOOL));         /* clear spool area     */
		puchP = &EEptSpool.auchData[0];                  /* R2.0 */

		for (;;) {
			PifRequestSem(usEEptSemHandle);              /* for hsEEptSemHandle  */
 			if (hsEEptPortHandle == EEPT_PORT_NONE) {    /* not open comm. port? */
				sRet = EEptRecoverPort();                /* port recover process */
				if ((sRet == EEPT_RET_FAIL) || (sRet == EEPT_RET_COM_ERRORS)) {     /* RS-232 port not open */
					PifReleaseSem(usEEptSemHandle);      /* for hsEEptSemHandle  */
					PifLog (MODULE_EEPT, LOG_EVENT_EPT_PORT_OPN_ERR_01);
					return(sRet);                        /* return port open err */
				}
			}
			PifReleaseSem(usEEptSemHandle);              /* for hsEEptSemHandle   */

			if (sCurrent == EEPT_NOISE) {
				PifRequestSem(usEEptSemHandle);          /* for hsEEptSemHandle   */
				sRet = PifReadCom(hsEEptPortHandle, (VOID *)&EEptSpool.auchData[0], 1);      /* read 1byte from PMS  */
				PifReleaseSem(usEEptSemHandle);          /* for hsEEptSemHandle   */
				sRet = EEptCheckComError(sRet, 1);       /* check PifReadCom err */
			} else {
				EEptSetReadTimer(sCurrent);
				PifRequestSem(usEEptSemHandle);          /* for hsEEptSemHandle   */
				sRet = PifReadCom(hsEEptPortHandle,      /* read next data       */
									(VOID *)&EEptSpool.auchData[1],
									(EEPT_SPOOL_SIZE-1));
				PifReleaseSem(usEEptSemHandle);          /* for hsEEptSemHandle   */
				sRet = EEptCheckComError(sRet, 0);       /* check PifReadCom err */
			}
			switch (sRet) {
				case EEPT_RET_SUCCESS:
					break;

				case PIF_ERROR_COM_TIMEOUT:             /* time out error ?     */
					sRet1Byte = EEpt1ByteTimeOut();
					if (sRet1Byte == EEPT_READ_AGAIN) {
						if (sCurrent == EEPT_CONT) {
							sCurrent = EEPT_NOISE;
						}
						continue;
					} else {
						return(sRet1Byte);
					}

				case PIF_ERROR_COM_POWER_FAILURE:       /* power down error ?   */
					sRet1Byte = EEpt1BytePowerOff();
					if (sRet1Byte == EEPT_READ_AGAIN) {
						continue;
					} else {
						return(sRet1Byte);
					}

				default:
					return(sRet);
			}

			if (sCurrent == EEPT_NOISE) {
	/* R2.0 START */
				switch(EEptSpool.auchData[0]) {     /* check received 1st char */
					case   EEPT_STX:                 /* STX Character            */
						sCurrent = EEPT_CONT;        /* start to receive response data */
						break;                      /* continue                 */
					case   EEPT_ACK_CHAR:            /* Acknowledge  char         (ACK) */
					case   EEPT_NAK_CHAR:            /* Negative Acknowledge char (NAK) */
					case   EEPT_ENQ_CHAR:            /* Enquiry char              (ENQ) */
					case   EEPT_EOT_CHAR:            /* End of Transmission char  (EOT) */
						EEptSpool.usLen = ((USHORT)(puchP - &EEptSpool.auchData[0]))+1;
						return(EEPT_TRACON);
					default:                        /* NOISE                    */
						break;                      /* continue                 */
				 }
			} else {
				USHORT  i;                                      /* R2.0 */

				puchP = memchr((VOID *)&EEptSpool.auchData[1], EEPT_ETX, (EEPT_SPOOL_SIZE-1)); /* search ETX char          */
				if (puchP == 0) {                   /* not found ?              */
					return(EEPT_RET_LENGTH);
				} else {
					usLen = ((USHORT)(puchP - &EEptSpool.auchData[0]))+1;
					if (usLen < EEPT_MIN_TEXT_LEN) {
						EEptSpool.usLen = usLen;     /* set received length */
						return(EEPT_RET_LENGTH);
					}
					for (i=0,uchLRC=0;i < usLen ;i++) {
						uchLRC ^= EEptSpool.auchData[i];  /* calculate LRC   */
					}
					if (uchLRC != EEptSpool.auchData[i]) { /* check LRC      */
						EEptSpool.usLen = usLen+1;   /* set received length */
						return(EEPT_RET_LENGTH);
					} else {
						NHPOS_ASSERT(sizeof(EEptSpool.auchData) >= sizeof(EEptRsp));
						memset(&EEptRsp,' ',sizeof(EEptRsp));

						EEptSetReceiveRS232((EEPTRSPDATA_v14*)EEptSpool.auchData,&EEptRsp);

						memset(EEptSpool.auchData,' ',EEptSpool.usLen);
						memcpy(EEptSpool.auchData,&EEptRsp,sizeof(EEptRsp));

						EEptSpool.usLen = sizeof(EEptRsp);
					}
					return(EEPT_END);
				}
			}
	/* R2.0 END */
		}
	}
}

VOID EEptSetReceiveRS232(EEPTRSPDATA_v14* pEEPTRespv14, EEPTRSPDATA *pEEPTResp)
{
	pEEPTResp->uchSTX = pEEPTRespv14->uchSTX;

	memcpy(pEEPTResp->auchSrcTerminal,pEEPTRespv14->auchSrcTerminal,sizeof(pEEPTRespv14->auchSrcTerminal));
	memcpy(pEEPTResp->auchDesTerminal,pEEPTRespv14->auchDesTerminal,sizeof(pEEPTRespv14->auchDesTerminal));
	memcpy(pEEPTResp->auchDataSeqNo, pEEPTRespv14->auchDataSeqNo,sizeof(pEEPTRespv14->auchDataSeqNo));

	pEEPTResp->auchEptStatus 		= pEEPTRespv14->auchStatus;
	pEEPTResp->auchTerminalNo 		= pEEPTRespv14->auchTerminalNo;
	pEEPTResp->auchLastMsgID 		= pEEPTRespv14->auchLastMsgID;
	pEEPTResp->auchMsgInstruction 	= pEEPTRespv14->auchMsgInstruction;


	memcpy(pEEPTResp->auchRspMsgNo,pEEPTRespv14->auchRspMsgNo,sizeof(pEEPTRespv14->auchRspMsgNo));
	memcpy(pEEPTResp->auchFolioNumber, pEEPTRespv14->auchFolioNumber,sizeof(pEEPTRespv14->auchFolioNumber));
	memcpy(pEEPTResp->auchPostTransNo,pEEPTRespv14->auchPostTransNo,sizeof(pEEPTRespv14->auchPostTransNo));

	pEEPTResp->auchMsgRecpt = pEEPTRespv14->auchMsgRecpt;
	pEEPTResp->auchMsgSlip 	= pEEPTRespv14->auchMsgSlip;

	memcpy(pEEPTResp->auchPrintLineNo,pEEPTRespv14->auchPrintLineNo,sizeof(pEEPTRespv14->auchPrintLineNo));

	pEEPTResp->auchPrintSelect = pEEPTRespv14->auchPrintSelect;

	memcpy(pEEPTResp->auchPartTender,pEEPTRespv14->auchPartTender,sizeof(pEEPTRespv14->auchPartTender));

	_RflMemcpyUchar2Tchar(pEEPTResp->auchMsgText,pEEPTRespv14->auchMsgText,sizeof(pEEPTRespv14->auchMsgText));

	pEEPTResp->uchETX = pEEPTRespv14->uchETX;
	pEEPTResp->uchLRC = pEEPTRespv14->uchLRC;
}

/*
*=============================================================================
** Format  : SHORT   EEptRecoverPort(VOID);
*
*    Input : none
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to recover port
*             EEPT_RET_FAIL       -fail to recover port
*
** Synopsis: This function close RS-232 port and open again.
*
*=============================================================================
*/
SHORT   EEptRecoverPort(VOID)
{
    USHORT      usWaitTimer;
    SHORT       sRet;

    if (hsEEptPortHandle != EEPT_PORT_NONE) {     /* port was recovered with another task ? */
        PifCloseCom(hsEEptPortHandle);           /* port close           */
        hsEEptPortHandle = EEPT_PORT_NONE;        /* reset handle value   */
    }

    usWaitTimer = EEptGetMDCTime(MDC_EPT_RCV_TIME);
    sRet=EEptOpenSetPort(usWaitTimer);               /* open port & set time */

    return(sRet);                                   /* port recover fail    */
}

/*
*=============================================================================
** Format  : SHORT   EEpt1ByteTimeOut(VOID)
*
*    Input : none
*    InOut : none
*
*            EEPT_READ_AGAIN  : wait left time
**           EEPT_RET_TIMEOUT : time out error
*
** Synopsis: This function executes timeout process.
*
*=============================================================================
*/
SHORT   EEpt1ByteTimeOut(VOID)
{
    USHORT  usTime = EEPT_MAX_SET_TIMER;
    SHORT   sRet;
    if ((uchEEptStatus != EEPT_STATE_SEND_OK)       /* after write message  */
       && (uchEEptStatus != EEPT_STATE_UNSOLI)      /* or after ACK (R2.0)  8-24-98 */
       && (uchEEptStatus != EEPT_STATE_INSIDE)){    /* or after ENQ (R2.0)  8-24-98 */
        return(EEPT_RET_TIMEOUT);
    }
    usTime = EEptCheckLeftTime();
    if (usTime != EEPT_PASS) {
        PifRequestSem(usEEptSemHandle);
        sRet = EEptOpenSetPort(usTime);              /* open port & set time */
        PifReleaseSem(usEEptSemHandle);
        return(EEPT_READ_AGAIN);                     /* ret wait left time   */
    } else {
        return(EEPT_RET_TIMEOUT);                    /* return time out      */
    }
}

/*
*=============================================================================
** Format  : SHORT   EEpt1BytePowerOff(VOID)
*
*    Input : none
*    InOut : none
*
** Return  : EEPT_READ_AGAIN  : wait left time
**           EEPT_RET_OFFLINE : offline error
*
** Synopsis: This function executes power down process.
*
*=============================================================================
*/
SHORT   EEpt1BytePowerOff(VOID)
{
    USHORT  usTime;
    SHORT   sRet;

    usTime = EEptCheckLeftTime();
    if (usTime != EEPT_PASS) {
        PifRequestSem(usEEptSemHandle);
        sRet = EEptOpenSetPort(usTime);              /* open port & set time */
        PifReleaseSem(usEEptSemHandle);
        return(EEPT_READ_AGAIN);                     /* ret wait left time   */
    } else {
        return(EEPT_RET_OFFLINE);                    /* return offline       */
    }
}

/*
*=============================================================================
** Format  : USHORT   EEptCheckMessageType(UCHAR *pData);
*
*    Input : pData     address of message
*    InOut : none
*
**   Return : EEPT_TYPE_USER_RESP    -response to user
*             EEPT_TYPE_UNSOLI       -unsoli.data from PMS
*             EEPT_TYPE_ERR_RESP     -other data
*
** Synopsis: This function check spool data.                        R2.0
*=============================================================================
*/
SHORT   EEptCheckMessageType(UCHAR *pData)
{
	EEPTRSPDATA      *pRec;

    pRec = (EEPTRSPDATA *)pData;
    switch (pRec->auchEptStatus) {
        case EEPT_FC2_ACCEPTED:                      /* user response data   */
        case EEPT_FC2_STORED:                        /* transaction was stored, same as EEPT_FC2_ACCEPTED   */
        case EEPT_FC2_REJECTED:
        case EEPT_FC2_OVERRIDE:
        case EEPT_FC2_NEEDGLINE:
        case EEPT_FC2_PARTTEND:                      /* E-022 corr. '93 4/15 */
             return(EEPT_TYPE_USER_RESP);

        case EEPT_FC2_UNSOLI:                       /* (R2.0) */
             return(EEPT_TYPE_UNSOLI);

        default:                                    /* other data           */
             return(EEPT_TYPE_ERR_RESP);
    }
}

/*
*=============================================================================
** Format  : SHORT   EEptExeResponse(VOID);
*
*    Input : none
*    InOut : none
*
**   Return : EEPT_CONT                * message continued        *
*             EEPT_END                 * message end              *
*
** Synopsis: This function send response to user.                   R2.0
*=============================================================================
*/
SHORT    EEptExeResponse(VOID)
{
    EEPTRSPDATA  *pRcv;
    DATE_TIME   Timer;

    if(! CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT0)
        &&(EEptSpool.usLen == sizeof(EPTRSPDATA))) {    /* EPT RESP MSG of US R1.1 type */
        PifReleaseSem(usEEptSemRcv);                    /* release for SELF term */
        PifRequestSem(usEEptSemSpool);                  /* request for EEptSpool */
        PifSleep(100);
        return(EEPT_END);                               /* do not send by IHC   */
    }

    pRcv = (EEPTRSPDATA *)&EEptSpool.auchData[0];
    if (pRcv->auchLastMsgID == EEPT_CONT_MSG) {
        PifGetDateTime(&Timer);                 /* get new time         */
        EEptSendTOD.uchHour = (UCHAR)Timer.usHour;
        EEptSendTOD.uchMin  = (UCHAR)Timer.usMin;
        EEptSendTOD.uchSec  = (UCHAR)Timer.usSec;
        PifReleaseSem(usEEptSemRcv);             /* release for SELF term*/
        PifRequestSem(usEEptSemSpool);           /* request for EEptSpool */
        PifSleep(100);
        return(EEPT_CONT);                       /* do not send by IHC   */
    }
	PifSleep(100);
    PifReleaseSem(usEEptSemRcv);                 /* release for SELF term*/
    PifRequestSem(usEEptSemSpool);               /* request for EEptSpool */
    return(EEPT_END);                            /* do not send by IHC   */
}
#if 0       /* Saratoga */
/*
*=============================================================================
** Format  : SHORT    EEptExeUnsoli(UCHAR uchState);
*
*    Input : none
*    InOut : none
*
**   Return : EEPT_UNSOLI_OK   (0)     * complite unsoli request *
*             EEPT_UNSOLI_BUSY (1)     * incomplite unsoli request *
*
** Synopsis: This function control unsoli.data & status.            R2.0
*=============================================================================
*/
SHORT    EEptExeUnsoli(UCHAR uchState)
{
    EEPTRSPDATA      *pRcv;
    SHORT           sRet;

    pRcv = (EEPTRSPDATA *)&EEptSpool.auchData[0];

    if (UifCPCheckUns()== UIF_CP_EXIST_DATA) {
                                         /* check transaction status */
        return(EEPT_UNSOLI_BUSY);         /* if not enough user buffer ,return */
    }
    if (sEEPTUnsControl == EEPT_UNSOLI_1ST) { /* 1st REQ data of Unsoli. Rep. Func */
       UifCPSaveRspUns((VOID *)&EEptSpool.auchData[0],EEPT_UNSOLI_1ST);
       sEEPTUnsControl = EEPT_UNSOLI_NEXT;
    } else {
       UifCPSaveRspUns((VOID *)&EEptSpool.auchData[0],EEPT_UNSOLI_NEXT);
    }

    if (pRcv->auchLastMsgID == EEPT_LAST_MSG) { /* last message             */
        UifCPSetUns();                      /* set unsoli status */
        EEptSendComTC(EEPT_ACK_CHAR);       /* send Acknowledge char    (ACK) */
        EEptWriteStatus(uchState);         /* restore previous state */
    } else {
        sRet = EEptSendComTC(EEPT_ACK_CHAR);    /* send Acknowledge char    (ACK) */
        if (sRet != EEPT_RET_SUCCESS) {
           EEptWriteStatus(uchState);         /* restore previous state */
        }
    }
    return(EEPT_UNSOLI_OK);
}
#endif
/*
*=============================================================================
** Format  : VOID   EEptExeError(SHORT sErrCode);
*
*    Input : sErrCode   - error code
*    InOut : none
*
**   Return : none;
*
** Synopsis: This function send error response to user.
*=============================================================================
*/
VOID    EEptExeError(SHORT sErrCode)
{
    EEptSpool.sErrCode = sErrCode;
    PifReleaseSem(usEEptSemRcv);                 /* release for SELF term*/
    PifRequestSem(usEEptSemSpool);               /* request for EEptSpool */
    EEptWriteStatus(EEPT_STATE_OUTSIDE);          /* change idle status   */
    return;                                     /* do not send by IHC   */
}

/*
*=============================================================================
*
** Format  :    SHORT   EEptCreateUnsMessage(VOID);
*
*    Input :    nothing
*
*   Output :    usMsgType;
*    InOut :    nothing
*
** Return  :    EEPT_TYPE_USER_RESP  - response message for user.
*               EEPT_TYPE_UNSOLI     - unsoli. message from PMS.
*               EEPT_TYPE_TRACON     - transmission control char(R2.0)
*               < 0                 - error message for user.
*
** Synopsis:    This function create message by RS-232 port.[for Receive Thread]
*                                                                   R2.0
*=============================================================================
*/
SHORT   EEptCreateUnsMessage(VOID)
{
    SHORT   sCurrent;

    EEptSetReadTimer(EEPT_CONT);                  /* set for PifReadCom() */
                                                /* (1sec)               */
    sCurrent = EEptReceiveCom(EEPT_NOISE);        /* read 1 byte data     */

    if (sCurrent < 0) {
        return(sCurrent);                       /* return err message   */
    }
    switch (sCurrent ) {
    case EEPT_NOISE:                         /* unnecessary data ?   */
        return(sCurrent);                   /* return EEPT_NOISE     */

    case EEPT_END:                           /* created message !    */
        if( !CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT0)
            &&(EEptSpool.usLen == sizeof(EPTRSPDATA))) { /* EPT RESP MSG of US R1.1 type */
            return(EEPT_TYPE_USER_RESP);
        }
        return(EEptCheckMessageType(EEptSpool.auchData));
                                                /* check message type   */
    case EEPT_TRACON:                        /* transmission control char */
        return(EEPT_TYPE_TRACON);
    }

    return(EEPT_SEND);                      /* Saratoga */
}


/* ---------- End of EEPTRCV.C ---------- */