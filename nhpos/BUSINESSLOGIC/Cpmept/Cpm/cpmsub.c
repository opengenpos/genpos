/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993                **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : CHARGE POSTING MANAGER Common routines
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMSUB.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               CpmSetProtocol();
*               CpmGetMDCTime();
*               CpmCheckComError();
*               CpmWriteStatus();
*               CpmComparePrevious();
*               CpmCheckSpoolData();
*               CpmCheckLeftTime();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-01-93 : 00.00.01 : Yoshiko.Jim  : Initial                                   
* May-27-94 : 02.05.00 : K.You        : Send Timer Value changed
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
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <plu.h>
#include <csstbfcc.h>
#include <cpm.h>
#include "cpmlocal.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

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
**    Synopsis:   VOID   CpmSetProtocol(VOID)
*
*     Inout  :    none
*                       
**  Return   :    none
*
**  Description:  This function sets data of protocol to static area.  
*               
*==========================================================================
*/
VOID    CpmSetProtocol(VOID)
{
    SYSCONFIG CONST FAR *pSys;

    pSys = PifSysConfig();                      /* get system configration  */
      
    memset(&CpmProtocol, 0, sizeof(CpmProtocol));
    CpmProtocol.fPip = PIF_COM_PROTOCOL_NON;
    CpmProtocol.usComBaud = pSys->ausComBaud[chCpmPortID];
    CpmProtocol.uchComByteFormat = pSys->auchComByteFormat[chCpmPortID];

	if (pSys->usInterfaceDetect & SYSCONFIG_IF_CPM_LAN) {
		CpmProtocol.fPip = PIF_COM_PROTOCOL_LAN;
		CpmProtocol.usNetPortNo = pSys->usCPMport;
		memcpy (CpmProtocol.uchNetIpAddr, pSys->ucCPMaddr, sizeof(pSys->ucCPMaddr));
	}
}

/*
*=============================================================================
*
** Format  : USHORT     CpmGetMDCTime(UCHAR uchAddress);
*               
*    Input :    uchAddress      : read address
*
*   Output :    return          : timer value
*
** Return  :    timer value (sec.)
*
** Synopsis:    This function read parameter of CPM from MDC.
*
*=============================================================================
*/
USHORT  CpmGetMDCTime(UCHAR uchAddress)
{
    PARAMDC DataMDC;
    USHORT  usTime=0;

    memset(&DataMDC, 0, sizeof(DataMDC));           /* reset area           */
    
    DataMDC.uchMajorClass = CLASS_PARAMDC;          /* set major class      */
    DataMDC.usAddress     = (USHORT)uchAddress;     /* set request address  */
    
    CliParaRead(&DataMDC);                          /* executs reading      */
    switch (uchAddress) {
        case MDC_CPM_WAIT_TIME:
            usTime = DataMDC.uchMDCData & (EVEN_MDC_BIT0 | EVEN_MDC_BIT1 |
                                           EVEN_MDC_BIT2 | EVEN_MDC_BIT3);
            usTime >>= 4;
            if (!usTime) {
                usTime = CLI_DEFMDC_CPMWAIT;        /* set default time     */
            }
            usTime *= 30;                           /* set ?? sec.          */
            break;

        case MDC_CPM_INTVL_TIME:
            usTime = DataMDC.uchMDCData & (ODD_MDC_BIT0 | ODD_MDC_BIT1 |
                                           ODD_MDC_BIT2 | ODD_MDC_BIT3);
            usTime++;                               /* Send Value change    */
            break;
    }
    return(usTime);                                 /* return timer value   */
}

/*
*=============================================================================
** Format  : SHORT   CpmCheckComError(SHORT sRet, USHORT usReqLen,
*                                     UCHAR uchTask)
*               
*    Input : sRet            -return value of PifWriteComm(), PifReadComm() 
*            usReqLen        -User Requireed length ( if 0, not check length)
*            uchTask         -caller (CPM_SEND or CPM_RECEIVE)
*   Output : none
*    InOut : none
*
** Return  : CPM_RET_SUCCESS     -success
*            CPM_RET_FAIL        -fail of PifReadCom()/PifWriteCom()
*
** Synopsis: This function checks result of PifReadCom()/PifWriteCom().
*
*=============================================================================
*/
SHORT   CpmCheckComError(SHORT sRet, USHORT usReqLen, UCHAR uchTask)
{
    if (sRet == PIF_ERROR_COM_POWER_FAILURE) {      /* power down error ?   */
        if (uchTask == CPM_SEND) {                  /* call from send task  */
            CpmWaitPortRecovery();                  /* wait port recovery   */
        } else { 
            CpmRecoverPort();                       /* recovery port handle */
        }
        return (sRet);                              /* return power down err*/
    } else if (sRet < 0) {
        return (sRet);                              /* return PifXxx() err  */
    }

    if (usReqLen != 0) {
        if ((USHORT)sRet != usReqLen) {             /* Required/Result same */
            return (CPM_RET_FAIL);                  /* return PifXxxx() err */
        }
    }
    return (CPM_RET_SUCCESS);                       /* PifXxxCom() success! */
}

/*
*=============================================================================
** Format  : VOID   CpmWriteStatus(UCHAR uchState);
*               
*    Input : uchState       change status
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function overide usCpmStatus.
*
*=============================================================================
*/
VOID    CpmWriteStatus(UCHAR uchState)
{
    PifRequestSem(usCpmSemState);                   /* for usCpmStatus      */
    uchCpmStatus = uchState;                        /* status change        */
    PifReleaseSem(usCpmSemState);                   /* for uchCpmStatus     */
}

/*
*=============================================================================
** Format  : USHORT   CpmComparePrevious(UCHAR *pSendData, USHORT usSendLen);
*               
*    Input : pSendData      pointer of send data
*            usSendLen      length of send data
*   Output : return
*    InOut : none
*
** Return  : CPM_SAME       -data of CpmSaveSend and this is same data
*            CPM_NOT_SAME   -data of CpmSaveSend and this isn't same data
*
** Synopsis: This function compare CpmSaveSend data and pSendData.
*
*=============================================================================
*/
USHORT   CpmComparePrevious(UCHAR *pSendData, USHORT usSendLen)
{

    if (usSendLen == CpmSaveSend.usLen) {           /* data length check    */    
      if (memcmp(pSendData, CpmSaveSend.auchData, usSendLen) == 0) {
                                                    /* compare send data    */
        return(CPM_SAME);                           /* return same          */
      }
    }
    return(CPM_NOT_SAME);                           /* return not same      */    
}

/*
*=============================================================================
** Format  : USHORT  CpmCheckSpoolData(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
*
** Return  : CPM_SAME       -CpmSaveSame and CpmSpool is called from same terminal.
*            CPM_NOT_SAME   -CpmSaveSame and CpmSpool isn't called from same terminal. 
*
** Synopsis: This function check source terminal, distination terminal,
*            and terminal# .
*=============================================================================
*/
USHORT  CpmCheckSpoolData(VOID)
{
    CPMRCREQDATA   *pSend;
    CPMRCRSPDATA   *pRcv;

    pSend = (CPMRCREQDATA *)&CpmSaveSend.auchData[0];
    pRcv = (CPMRCRSPDATA *)&CpmSpool.auchData[0];

    if (memcmp(pSend->auchSrcTerminal,              /* check source term    */
               pRcv->auchSrcTerminal,
               sizeof(pRcv->auchSrcTerminal)) != 0 ) {
        return(CPM_NOT_SAME);                       /* return not same      */
    }
    if (memcmp(pSend->auchDesTerminal,              /* check destinat. term */
               pRcv->auchDesTerminal,
               sizeof(pRcv->auchDesTerminal)) != 0 ) {
        return(CPM_NOT_SAME);                       /* return not same      */
    }
    if (pSend->auchTerminalNo[0] != pRcv->auchTerminalNo[0]) {
                                                    /* check terminal #     */
        return(CPM_NOT_SAME);                       /* return not same      */
    }
    return(CPM_SAME);                               /* return same          */
}
/*
*=============================================================================
** Format  : USHORT  CpmCheckLeftTime(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
*
** Return  : CPM_PASS       -already passed MDC_CPM_INTVL_TIME
*            > 0            -left time (sec.) 
*
** Synopsis: This function check TOD and get time for left.
*=============================================================================
*/
USHORT   CpmCheckLeftTime(VOID)
{
    DATE_TIME   Timer;
    CPMTIMER    CurTime;
    ULONG       ulOld, ulNew;
    USHORT      usIntvlTimer;
    
    PifGetDateTime(&Timer);                         /* get current time     */
                    
    CurTime.uchHour = (UCHAR)Timer.usHour;
    CurTime.uchMin  = (UCHAR)Timer.usMin;
    CurTime.uchSec  = (UCHAR)Timer.usSec;

    ulOld = (CpmSendTOD.uchHour * 3600L)            /* HH:MM:SS -> XX sec.  */
          + (CpmSendTOD.uchMin * 60L) 
          + CpmSendTOD.uchSec;
    ulNew = (CurTime.uchHour * 3600L)               /* HH:MM:SS -> XX sec.  */
          + (CurTime.uchMin * 60L) 
          + CurTime.uchSec;
    
    if (ulOld > ulNew) {                            /* next day ?           */
        ulNew += 24 * 3600L;
    }
    
    usIntvlTimer = CpmGetMDCTime(MDC_CPM_WAIT_TIME);

    if ((ulNew - ulOld) >= (ULONG)usIntvlTimer) {
        return(CPM_PASS);
    } else {
        return((usIntvlTimer - (USHORT)(ulNew -ulOld)));
    }
}

/* ---------- End of CPMSUB.C ---------- */


