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
* Title       : CHARGE POSTING MANAGER Network Routines
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMNET.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*                   CpmReturnMessage();
*                   CpmReturnErrMessage();
*                   CpmNetOpen();
*                   CpmNetClose();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-01-93 : 00.00.01 : Yoshiko.Jim  : Initial                                   
* Dec-09-93 : 00.00.02 : Yoshiko.Jim  : Change PifNetSend() return control                                   
* Apr-26-94 : 00.00.05 : Yoshiko.Jim  : E-067 corr. (mod. PifNetSend())                                   
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
#include "log.h"
#include "plu.h"
#include "appllog.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "csstbpar.h"
#include "csstbfcc.h"
#include "cpm.h"
#include "csstbcpm.h"
#include "cpmlocal.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/**     NET CONTROL     **/  
CPMSENDMES  CpmSndBuf;                              /* Send Buffer to Cli   */
SHORT   hsCpmNetHandle;                             /* save network handle  */

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
** Format  : VOID  CpmReturnMessage(UCHAR *pMessage, USHORT usLen);
*               
*    Input : pMessage   - pointer of return message
*            usLen      - length of return message
*   Output : none
*    InOut : none
*
** Return  : none;
*
** Synopsis: This function send return message.
*               return address  CpmSaveSend.pData.auchTerminalNo.
*
*=============================================================================
*/
VOID    CpmReturnMessage(UCHAR *pMessage, USHORT usLen)
{
    SHORT           sRet;

    if (uchCpmStatus != CPM_STATE_SEND_OK) {        /* during not executing?*/
        return;
    }
    if (hsCpmNetHandle == CPM_PORT_NONE) {          /* not stand alone?     */
        return;
    }
                                                    /* set IHC send buffer  */
    CpmSndBuf.CpmResDat.sReturn = CPM_RET_SUCCESS;  /* set success code     */
    CpmSndBuf.CpmResDat.usRetLen = usLen;           /* set response length  */
    memcpy(CpmSndBuf.CpmResDat.auchCpmData,         /* copy response message*/
           pMessage, usLen);
                                                    
    sRet = CpmNetSend(sizeof(CpmSndBuf));           /* send to IHC          */
    if (sRet != CPM_RET_SUCCESS) {
        ((CPMTALLY *)&Para_CpmTally)->usIHCSendErr++;                    /* tally count up       */ 
    }
    return;
}

/*
*=============================================================================
** Format  : VOID  CpmReturnErrMessage(SHORT sErrCode);
*               
*    Input : sErrCode   - error code
*   Output : none
*    InOut : none
*
** Return  : none;
*
** Synopsis: This function send error code if during executing process.
*               return address  CpmSaveSend.uchData.auchTerminalNo.
*
*=============================================================================
*/
VOID   CpmReturnErrMessage(SHORT sErrCode)
{
    SHORT   sRet;

    if (!( (uchCpmStatus == CPM_STATE_INSIDE) ||    /* during not executing?*/
           (uchCpmStatus == CPM_STATE_SEND_OK))) {
        return;
    }
    if (hsCpmNetHandle == CPM_PORT_NONE) {          /*  stand alone?        */
        return;
    }
                                                    /* set IHC send buffer  */
    CpmSndBuf.CpmResDat.sReturn = sErrCode;         /* set error code       */
    CpmSndBuf.CpmResDat.usRetLen = 0;               /* set error length     */

    sRet = CpmNetSend(sizeof(CpmSndBuf) - CLI_MAX_CPMDATA);
                                                    /* send to IHC          */
    if (sRet != CPM_RET_SUCCESS) {
        Para_CpmTally.usIHCSendErr++;    /* tally count up       */ 
    }
    return;
}

/*
*===========================================================================
** Synopsis:    VOID    CpmNetOpen(VOID);
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return: none
*
** Description: This function opens NETWORK.
*===========================================================================
*/
VOID    CpmNetOpen(VOID)
{
    XGHEADER        IPdata;
    SHORT           sHandle;
    SYSCONFIG CONST FAR *SysCon;

    SysCon = PifSysConfig();
    if (((0 == SysCon->auchLaddr[0]) &&
         (0 == SysCon->auchLaddr[1]) &&
         (0 == SysCon->auchLaddr[2])) ||            /* IHC board not provide*/
         (0 == SysCon->auchLaddr[3])) {             /* U.A is 0             */
        return;
    }
    memset(&IPdata, 0, sizeof(XGHEADER));

    IPdata.usLport = CLI_PORT_CPM;
    IPdata.usFport = ANYPORT;
    sHandle = PifNetOpen((XGHEADER FAR *)&IPdata);  /* Open network         */

    if (0 <= sHandle) {                              
        hsCpmNetHandle = sHandle;                   /* success, save handle */
        PifNetControl(hsCpmNetHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);
        return;
    } else {
        PifAbort(MODULE_CPM, FAULT_BAD_ENVIRONMENT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CpmNetClose(VOID);
*
*     InPut:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function closes NETWORK.
*===========================================================================
*/
VOID    CpmNetClose(VOID)
{
    PifNetClose(hsCpmNetHandle);                    /* for power down recov */
    hsCpmNetHandle = CPM_PORT_NONE;                 /* IHC port handle      */
    return;
}

/*
*===========================================================================
** Synopsis:    SHORT   CpmNetSend(USHORT usLen);
*
*     InPut:    usLen       : length of send data 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CPM_RET_SUCCESS     : success send
*               other               : send failure
*
** Description: This function send message by NETWORK.
*===========================================================================
*/
SHORT   CpmNetSend(USHORT usLen)
{
    SHORT   sError=0, sRetry=5;

    do {
        if (5 > sRetry) {
            PifSleep(100);
        }
        if (sError == PIF_ERROR_NET_POWER_FAILURE) {    /* if power down ?  */
            CpmNetClose();                          /* close old IHC port   */
            CpmNetOpen();                           /* open new IHC port    */
        }
        sError = PifNetSend(hsCpmNetHandle,    
                            (VOID FAR *)&CpmSndBuf,
                            usLen);
		sRetry--;
    } while (((PIF_ERROR_NET_WRITE == sError) && (sRetry > 0)) || 
		((PIF_ERROR_NET_OFFLINE == sError) && (sRetry > 0)) ||
		((PIF_ERROR_NET_BUSY == sError) && (sRetry > 0)) );

    if (sError >= PIF_OK) {                         /* E-067 corr. '94 4/26 */ 
        return(CPM_RET_SUCCESS);
    } else {                                        /* error case           */ 
        return(sError);
    }
}

/* ---------- End of CPMNET.C ---------- */

