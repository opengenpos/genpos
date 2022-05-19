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
* Title       : Enhanced EPT Common routines
* Category    : Enhanced EPT module,NCR2170 GP R2.0
Charge Posting Manager, NCR 2170 GP R2.0
* Program Name: EEPTSUB.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               EEptSetProtocol();
*               EEptGetMDCTime();
*               EEptCheckComError();
*               EEptWriteStatus();
*               EEptComparePrevious();
*               EEptCheckSpoolData();
*               EEptCheckLeftTime();
*               EEptSendComTC();
*               EEptLog();
*
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

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "rfl.h"
#include "plu.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "csstbpar.h"
#include "csstbfcc.h"
#include "eeptl.h"
#include "prevept.h"


/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

#ifdef DEBUG
EEPT_232C_LOG aEEptLog;                       /* RS-232/C data log    */
#endif

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
*==========================================================================
**    Synopsis:   VOID   EEptSetProtocol(VOID)
*
*     Inout  :    none
*
**  Return   :    none
*
**  Description:  This function sets data of protocol to static area.
*
*==========================================================================
*/
VOID    EEptSetProtocol(VOID)
{
    SYSCONFIG CONST FAR *pSys;

    pSys = PifSysConfig();                      /* get system configration  */

    memset(&EEptProtocol, 0, sizeof(EEptProtocol));
    EEptProtocol.fPip = PIF_COM_PROTOCOL_NON;
#ifdef DEBUG
/* baud rate=9600/even parity,1 stop bit,7 bits data */
/* control character is not available (no affect) */
    EEptProtocol.usComBaud = 9600;
    EEptProtocol.uchComByteFormat = 0x1a;
    EEptProtocol.uchComTextFormat = 0x20;      /** no affect **/
#else
    EEptProtocol.usComBaud = pSys->ausComBaud[chEEptPortID];
    EEptProtocol.uchComByteFormat = pSys->auchComByteFormat[chEEptPortID];
#endif

	if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_LAN) {
		EEptProtocol.fPip = PIF_COM_PROTOCOL_LAN;
		EEptProtocol.usNetPortNo = pSys->EPT_ENH_Info[0].usPortNo;
		memcpy (EEptProtocol.uchNetIpAddr, pSys->EPT_ENH_Info[0].ucIpAddress, sizeof(pSys->EPT_ENH_Info[0].ucIpAddress));
	}
	else if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL) {
		EEptProtocol.fPip = PIF_COM_PROTOCOL_DLL;
		EEptProtocol.usNetPortNo = pSys->EPT_ENH_Info[0].usPortNo;
		memcpy (EEptProtocol.uchNetIpAddr, pSys->EPT_ENH_Info[0].ucIpAddress, sizeof(pSys->EPT_ENH_Info[0].ucIpAddress));
	}

}

/*
*=============================================================================
*
** Format  : USHORT     EEptGetMDCTime(UCHAR uchAddress);
*
*    Input :    uchAddress      : read address
*
*   Output :    return          : timer value
*
** Return  :    timer value (sec.)
*
** Synopsis:    This function read parameter of EEPT from MDC.
*
*=============================================================================
*/
USHORT  EEptGetMDCTime(USHORT uchAddress)
{
    PARAMDC DataMDC;
    USHORT  usTime=0;

    memset(&DataMDC, 0, sizeof(DataMDC));           /* reset area           */

    DataMDC.uchMajorClass = CLASS_PARAMDC;          /* set major class      */
    DataMDC.usAddress     = uchAddress;             /* set request address  */

    CliParaRead(&DataMDC);                          /* executs reading      */
    if (uchAddress ==MDC_EPT_RCV_TIME) {
        usTime = DataMDC.uchMDCData & (EVEN_MDC_BIT0 | EVEN_MDC_BIT1 | EVEN_MDC_BIT2 | EVEN_MDC_BIT3);
        usTime >>= 4;
        usTime++;
        usTime *= 30;                           /* set ?? sec.          */

    } else {                                    /* case MDC_EPT_SND_TIME */
        usTime = DataMDC.uchMDCData & (ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3);
        usTime++;                               /* Send Value change    */
    }
    return(usTime);                                 /* return timer value   */
}

/*
*=============================================================================
** Format  : SHORT   EEptCheckComError(SHORT sRet, USHORT usReqLen)
*
*
*    Input : sRet            -return value of PifWriteComm(), PifReadComm()
*            usReqLen        -User Requireed length ( if 0, not check length)
*   Output : none
*    InOut : none
*
** Return  : EEPT_RET_SUCCESS     -success
*            EEPT_RET_FAIL        -fail of PifReadCom()/PifWriteCom()
*
** Synopsis: This function checks result of PifReadCom()/PifWriteCom().
*
*=============================================================================
*/
SHORT   EEptCheckComError(SHORT sRet, USHORT usReqLen)
{
    if (sRet == PIF_ERROR_COM_POWER_FAILURE) {      /* power down error ?   */
        EEptRecoverPort();                           /* recovery port handle (R2.0)*/
        return (sRet);                              /* return power down err*/
    } else if (sRet < 0) {
        return (sRet);                              /* return PifXxx() err  */
    }

    if (usReqLen != 0) {
        if ((USHORT)sRet != usReqLen) {             /* Required/Result same */
            return (EEPT_RET_FAIL);                  /* return PifXxxx() err */
        }
    }
    return (EEPT_RET_SUCCESS);                       /* PifXxxCom() success! */
}

/*
*=============================================================================
** Format  : VOID   EEptWriteStatus(UCHAR uchState);
*
*    Input : uchState       change status
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function overide usEEptStatus.
*
*=============================================================================
*/
VOID    EEptWriteStatus(UCHAR uchState)
{
    PifRequestSem(usEEptSemState);                   /* for uchEEptStatus     */
    uchEEptStatus = uchState;                        /* status change        */
    PifReleaseSem(usEEptSemState);                   /* for uchEEptStatus     */
}

/*
*=============================================================================
** Format  : USHORT   EEptComparePrevious(UCHAR *pSendData, USHORT usSendLen);
*
*    Input : pSendData      pointer of send data
*            usSendLen      length of send data
*   Output : return
*    InOut : none
*
** Return  : EEPT_SAME       -data of EEptSaveSend and this is same data
*            EEPT_NOT_SAME   -data of EEptSaveSend and this isn't same data
*
** Synopsis: This function compare EEptSaveSend data and pSendData.
*
*=============================================================================
*/
USHORT   EEptComparePrevious(UCHAR *pSendData, USHORT usSendLen)
{

    if (usSendLen == EEptSaveSend.usLen) {                                   /* data length check    */
        if (memcmp(pSendData, &EEptSaveSend.auchReqData.u.eeptReqData, usSendLen) == 0) {   /* compare send data    */
            return(EEPT_SAME);                                               /* return same          */
        }
    }
    return(EEPT_NOT_SAME);                           /* return not same      */
}

/*
*=============================================================================
** Format  : USHORT  EEptCheckSpoolData(VOID);
*
*    Input : none
*   Output : none
*    InOut : none
*
** Return  : EEPT_SAME       -EEptSaveSame and EEptSpool is called from same terminal.
*            EEPT_NOT_SAME   -EEptSaveSame and EEptSpool isn't called from same terminal.
*
** Synopsis: This function check source terminal, distination terminal,
*            and terminal# .                                    R2.0
*=============================================================================
*/
USHORT  EEptCheckSpoolData(VOID)
{
    if(! CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT0) &&(EEptSpool.usLen == sizeof(EPTRSPDATA))) {    /* EPT RESP MSG of US R1.1 type */
        if (EEptSaveSend.usLen == sizeof(EPTREQDATA)) { /* if EPT REQ MSG of US R1.1 type */
            return(EEPT_SAME);                          /* same */
        }
        return(EEPT_NOT_SAME);                          /* not same */
    }

	if((hsEEptPortHandle == EEPT_PORT_USEDLL) || (hsEEptPortHandle== EEPT_PORT_USENET)) {
		EEPTREQDATA  *pSend = (EEPTREQDATA *)&EEptSaveSend.auchReqData.u.eeptReqData;
		EEPTRSPDATA  *pRcv = (EEPTRSPDATA *)&EEptSpool.auchData[0];

		if (memcmp(pSend->auchSrcTerminal, pRcv->auchSrcTerminal, sizeof(pRcv->auchSrcTerminal)) != 0 ) {    /* check EEPTREQDATA source term    */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
		if (memcmp(pSend->auchDesTerminal, pRcv->auchDesTerminal, sizeof(pRcv->auchDesTerminal)) != 0 ) {    /* check EEPTREQDATA destinat term    */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
		if (pSend->auchTerminalNo != pRcv->auchTerminalNo) {  /* check terminal #     */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
	} else {
		EEPTREQDATA_v14  *pSend = (EEPTREQDATA_v14 *)&EEptSaveSend.auchReqData.u.eeptReqData;
		EEPTRSPDATA  *pRcv = (EEPTRSPDATA *)&EEptSpool.auchData[0];

		if (memcmp(pSend->auchSrcTerminal, pRcv->auchSrcTerminal, sizeof(pRcv->auchSrcTerminal)) != 0 ) {   /* check EEPTREQDATA_v14 source term    */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
		if (memcmp(pSend->auchDesTerminal, pRcv->auchDesTerminal, sizeof(pRcv->auchDesTerminal)) != 0 ) {   /* check EEPTREQDATA_v14 destinat term    */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
		if (pSend->auchTerminalNo != pRcv->auchTerminalNo) { /* check terminal #     */
			return(EEPT_NOT_SAME);                       /* return not same      */
		}
	}
    return(EEPT_SAME);                               /* return same          */
}
/*
*=============================================================================
** Format  : USHORT  EEptCheckLeftTime(VOID);
*
*    Input : none
*   Output : none
*    InOut : none
*
** Return  : EEPT_PASS       -already passed MDC_EPT_SND_TIME
*            > 0            -left time (sec.)
*
** Synopsis: This function check TOD and get time for left.
*=============================================================================
*/
USHORT   EEptCheckLeftTime(VOID)
{
    DATE_TIME   Timer;
    EEPTTIMER    CurTime;
    ULONG       ulOld, ulNew;
    USHORT      usIntvlTimer;

    PifGetDateTime(&Timer);                           /* get current time     */

    CurTime.uchHour = (UCHAR)Timer.usHour;
    CurTime.uchMin  = (UCHAR)Timer.usMin;
    CurTime.uchSec  = (UCHAR)Timer.usSec;

    ulOld = (EEptSendTOD.uchHour * 3600L) + (EEptSendTOD.uchMin * 60L) + EEptSendTOD.uchSec;  /* HH:MM:SS -> XX sec.  */
    ulNew = (CurTime.uchHour * 3600L) + (CurTime.uchMin * 60L) + CurTime.uchSec;  /* HH:MM:SS -> XX sec.  */

    if (ulOld > ulNew) {                            /* next day ?           */
        ulNew += 24 * 3600L;
    }

    usIntvlTimer = EEptGetMDCTime(MDC_EPT_RCV_TIME);

    if ((ulNew - ulOld) >= (ULONG)usIntvlTimer) {
        return(EEPT_PASS);
    } else {
        return((usIntvlTimer - (USHORT)(ulNew -ulOld)));
    }
}

/*
*=============================================================================
** Format  : SHORT   EEptSendComTC(UCHAR ucCode);
*
*    Input : ucCode              Transmission Control Code
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*
** Synopsis: This function sends Transmission Control Code to PMS (RS-232 Port).
*=============================================================================
*/
SHORT   EEptSendComTC(UCHAR ucCode)
{
    SHORT  sRet;

    sRet = EEptEEptSendComTC(ucCode);

#ifdef DEBUG
    EEptLog(EEPT_MODULE_SENDCOMTC,sRet,ucCode);
#endif

    return(sRet);

}
/*
*=============================================================================
** Format  : SHORT   EEptEEptSendComTC(UCHAR ucCode);
*
*    Input : ucCode              Transmission Control Code
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -success to send data
*             EEPT_RET_OFFLINE    -PMS offline err
*
** Synopsis: This function sends Transmission Control Code to PMS (RS-232 Port).
*=============================================================================
*/
SHORT   EEptEEptSendComTC(UCHAR ucCode)
{
    SHORT  sRet;
    DATE_TIME   Timer;

    PifRequestSem(usEEptSemHandle);                  /* for usEEptStatus      */
    if (hsEEptPortHandle == EEPT_PORT_NONE) {         /* if RS-232 port open? */
        sRet = EEptRecoverPort();                    /* wait port recovery   */
        if (sRet == EEPT_RET_FAIL) {                 /* RS-232 port not open */
            PifReleaseSem(usEEptSemHandle);          /* for usEEptStatus      */
            return(EEPT_RET_OFFLINE);                /* return port open err */
        }
    }
    sRet = PifWriteCom(hsEEptPortHandle, (VOID *)&ucCode, 1);  /* send transmission code to port    */

    PifGetDateTime(&Timer);                         /* get current time     */
    EEptSendTOD.uchHour = (UCHAR)Timer.usHour;
    EEptSendTOD.uchMin  = (UCHAR)Timer.usMin;
    EEptSendTOD.uchSec  = (UCHAR)Timer.usSec;

    sRet = EEptCheckComError(sRet, 1);               /* check PifWriteCom()  */
    if (sRet != EEPT_RET_SUCCESS) {
        sRet = EEPT_RET_OFFLINE;
    }
    PifReleaseSem(usEEptSemHandle);                  /* for usEEptStatus      */
    return(sRet);
}

/*
*==========================================================================
**  Synopsis:   VOID EEPTReadResetTally( UCHAR uchType, EPTTALLY *pTally )
*
*   Input:      UCHAR       uchType
*                       
*   Output:     nothing
*   InOut:      EPTTALLY    *pTally
*
*   Return:     nothing
*
*   Description: read and reset EPT comm. tally  
*==========================================================================
*/
VOID    EEPTReadResetTally(UCHAR uchType, EPTTALLY *pTally)
{
	if (pTally) {
		*pTally = Para.EPTTally;
	}

    if (uchType == EPT_TALLY_RESET) {
        memset (&Para.EPTTally, 0, sizeof(EPTTALLY));
    }
}

#ifdef DEBUG
/*
*==========================================================================
**    Synopsis:   VOID   EEptLog(UCHAR uchModuleId,SHORT sRet,UCHAR uchTC)
*
*     Input  :    uchmoduleId   Module Id
*                 sRet         Module Return Code
*                 uchTC        send data(for EEptSendComTC() )
**    Output :    none
*
**  Return   :    none
*
**  Description:  This function logs RS-232/C send/receive data to static area.
*
*==========================================================================
*/
VOID    EEptLog(UCHAR uchModuleId,SHORT sRet,UCHAR uchTC)
{

    DATE_TIME   Timer;

    if (aEEptLog.uchExecuteFlag != 0) {

        PifRequestSem(usEEptSemHandle);

        PifGetDateTime(&Timer);                         /* get current time     */
        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchMin  = (UCHAR)Timer.usMin;
        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchSec  = (UCHAR)Timer.usSec;

        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].usThreadId = PifGetThreadId();

        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchModuleId = uchModuleId;
        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchStatus  = uchEEptStatus;

        if (aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].usThreadId == aEEptLog.usSendthreadId) {
            aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchSubStatus = uchEEPTSendStatus;
        } else {
            aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].uchSubStatus = uchEEPTReceiveStatus;
        }

        aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].sRetCode = sRet;

        memset(aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].auchData, 0, EEPT_LOG_DATA_SIZE);  /* clear log area     */

        switch(uchModuleId) {
            case EEPT_MODULE_RECEIVECOM:            /* EEptReceiveCom() Module Id */
                aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].usLen = EEptSpool.usLen;
                memcpy(aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].auchData, EEptSpool.auchData,EEPT_LOG_DATA_SIZE);
                break;

            case EEPT_MODULE_SENDCOM:               /* EEptSendCom()    Module Id */
                aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].usLen = EEptSaveSend.usLen;
                memcpy(aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].auchData, EEptSaveSend.auchData,EEPT_LOG_DATA_SIZE);
                break;

            case EEPT_MODULE_SENDCOMTC:             /* EEptSendComTC()  Module Id */
                aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].usLen = 1;
                aEEptLog.aEEptLogItem[aEEptLog.usCurrentLogId].auchData[0] = uchTC;
                break;
        }

        if (++aEEptLog.usCurrentLogId >= EEPT_MAX_LOG_NUMBER) {
            aEEptLog.usCurrentLogId = 0;
        }
        PifReleaseSem(usEEptSemHandle);
    }
    return;
}
#endif

/* ---------- End of EEPTSUB.C ---------- */