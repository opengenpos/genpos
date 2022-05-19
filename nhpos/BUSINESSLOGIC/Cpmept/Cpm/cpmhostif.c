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
* Title       : CHARGE POSTING Host Interface Network Routines
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMHOSTIF.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*                   CpmReturnMessage();
*                   CpmReturnErrMessage();
*                   CpmHostOpen();
*                   CpmHostClose();
*                   CpmHostSend ();
*                   CpmHostRecv ();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-01-93 : 00.00.01 : Yoshiko.Jim  : Initial                                   
* Dec-09-93 : 00.00.02 : Yoshiko.Jim  : Change PifNetSend() return control                                   
* Apr-26-94 : 00.00.05 : Yoshiko.Jim  : E-067 corr. (mod. PifNetSend())                                   
*=============================================================================
*/

/**
;============================================================================
;                  I N C L U D E     F I L E s                              +
;============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
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
static SHORT   hsCpmHostHandle;                        /* save network handle  */

CPMHOSTCONFIG  CpmHostConfig;

XGRAMEX   CpmHostSndBuf;      /* Send response buffer */
XGRAMEX   CpmHostRcvBuf;      /* Receive request buffer */

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
*===========================================================================
** Synopsis:    VOID    CpmHostOpen(VOID);
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return: none
*
** Description: This function opens a network connection to the specified
				Charge Post host machine.  .
*===========================================================================
*/
VOID    CpmHostOpen(VOID)
{
    XGHEADER        IPdata;
    SHORT           sHandle;
    SYSCONFIG CONST FAR *SysCon;

    SysCon = PifSysConfig();
    if (((0 == SysCon->auchLaddr[0]) &&
         (0 == SysCon->auchLaddr[1]) &&
         (0 == SysCon->auchLaddr[2])) ||   /* In-House Communications board not provide*/
         (0 == SysCon->auchLaddr[3])) {    /* U.A is 0             */
        return;
    }
    memset(&IPdata, 0, sizeof(XGHEADER));

    memcpy(IPdata.auchFaddr, CpmProtocol.uchNetIpAddr, sizeof(CpmProtocol.uchNetIpAddr));

    IPdata.usLport = CLI_PORT_CPM_HOST;
    IPdata.usFport = CLI_PORT_CPM_HOST;
    sHandle = PifNetOpen((XGHEADER FAR *)&IPdata);  /* Open network         */

    if (0 <= sHandle) {                              
        hsCpmHostHandle = sHandle;                   /* success, save handle */
        PifNetControl(hsCpmHostHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);
        return;
    } else {
        PifAbort(MODULE_CPM, FAULT_BAD_ENVIRONMENT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    CpmHostClose(VOID);
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
VOID    CpmHostClose(VOID)
{
    PifNetClose(hsCpmHostHandle);                    /* for power down recov */
    hsCpmHostHandle = CPM_PORT_NONE;                 /* IHC port handle      */
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
SHORT   CpmHostSend(VOID *pCpmHostSndBuf, USHORT usLen)
{
    SHORT   sError=0, sRetry=5;
    ULONG   ulSendLen;

    memcpy(CpmHostSndBuf.auchFaddr, CpmProtocol.uchNetIpAddr, sizeof(CpmProtocol.uchNetIpAddr));
    CpmHostSndBuf.usFport = CLI_PORT_CPM_HOST;
    CpmHostSndBuf.usLport = CLI_PORT_CPM_HOST;
    ulSendLen = sizeof(CpmHostSndBuf.auchFaddr) +
			sizeof(CpmHostSndBuf.usFport) + sizeof(CpmHostSndBuf.usLport);

	if (usLen > sizeof(CpmHostSndBuf.auchData)) {
		NHPOS_ASSERT(0);  // when testing, ASSERT out if transfer data is too large for buffer
		usLen = sizeof(CpmHostSndBuf.auchData);
	}

	memcpy(CpmHostSndBuf.auchData, pCpmHostSndBuf, usLen);
    ulSendLen += usLen;

    do {
        if (5 > sRetry) {
            PifSleep(100);
        }
        if (sError == PIF_ERROR_NET_POWER_FAILURE) {    /* if power down ?  */
            CpmHostClose();                             /* close old port   */
            CpmHostOpen();                              /* open new port    */
        }
        sError = PifNetSend(hsCpmHostHandle,    
                            &CpmHostSndBuf,
                            ulSendLen);
		sRetry--;
    } while (((PIF_ERROR_NET_WRITE == sError) && (sRetry > 0)) || 
		((PIF_ERROR_NET_OFFLINE == sError) && (sRetry > 0)) ||
		((PIF_ERROR_NET_BUSY == sError) && (sRetry > 0)) );

	// Need to convert PIF return into a CPM return code.
    if (sError >= PIF_OK) {                         /* E-067 corr. '94 4/26 */ 
        return(CPM_RET_SUCCESS);
    } else {                                        /* error case           */ 
        return(sError);
    }
}


/*
*===========================================================================
** Synopsis:    SHORT   CpmNetRecv(USHORT usLen);
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
SHORT   CpmHostRecv(VOID *pCpmRcvBuf, USHORT usLen)
{
    SHORT   sError=0, sRetry=2;
	USHORT	usTimer = 500;      // 500 millisecond time out set

    PifNetControl(hsCpmHostHandle, PIF_NET_SET_TIME, usTimer);

    do {
        if (2 > sRetry) {
            PifSleep(100);
        }
        if (sError == PIF_ERROR_NET_POWER_FAILURE) {    /* if power down ?  */
            CpmHostClose();                             /* close old port   */
            CpmHostOpen();                              /* open new port    */
        }
        sError = PifNetReceive(hsCpmHostHandle,    
                            &CpmHostRcvBuf,
                            sizeof(CpmHostRcvBuf));
    } while (( (PIF_ERROR_NET_WRITE == sError)   && (--sRetry > 0) ) ||
             ( (PIF_ERROR_NET_OFFLINE == sError) && (--sRetry > 0) ) );

	// Need to convert PIF return into a CPM return code.
    if (sError >= PIF_OK) {                         /* E-067 corr. '94 4/26 */ 
		memcpy(pCpmRcvBuf, CpmHostRcvBuf.auchData, sError);
    }

	return(sError);
}

/* ---------- End of CPMHOSTIF.C ---------- */

