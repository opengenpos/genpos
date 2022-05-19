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
*                   EEptReturnMessage();
*                   EEptReturnErrMessage();
*                   EEptHostOpen();
*                   EEptHostClose();
*                   EEptHostSend ();
*                   EEptHostRecv ();
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
#include "eeptl.h"
#include "prevept.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/**     NET CONTROL     **/  
static SHORT   hsEEptHostHandle;                        /* save network handle  */

EEPTHOSTCONFIG  EEptHostConfig;

XGRAMEX   EEptHostSndBuf;      /* Send response buffer */
XGRAMEX   EEptHostRcvBuf;      /* Receive request buffer */

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
** Synopsis:    VOID    EEptHostOpen(VOID);
*
*     InPut:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return: none
*
** Description: This function opens a network connection to the specified
				Enhanced Electronic Payment (EEPT) host machine.  .
*===========================================================================
*/
VOID    EEptHostOpen(VOID)
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

    memcpy(IPdata.auchFaddr, EEptProtocol.uchNetIpAddr, sizeof(EEptProtocol.uchNetIpAddr));

    IPdata.usLport = CLI_PORT_EEPT_HOST;
    IPdata.usFport = CLI_PORT_EEPT_HOST;
    sHandle = PifNetOpen((XGHEADER FAR *)&IPdata);  /* Open network         */

    if (0 <= sHandle) {                              
        hsEEptHostHandle = sHandle;                   /* success, save handle */
        PifNetControl(hsEEptHostHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);
        return;
    } else {
        PifAbort(MODULE_EEPT, FAULT_BAD_ENVIRONMENT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    EEptHostClose(VOID);
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
VOID    EEptHostClose(VOID)
{
    PifNetClose(hsEEptHostHandle);                    /* for power down recov */
    hsEEptHostHandle = EEPT_PORT_NONE;                 /* IHC port handle      */
    return;
}

/*
*===========================================================================
** Synopsis:    SHORT   EEptNetSend(USHORT usLen);
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
SHORT   EEptHostSend(VOID *pEEptHostSndBuf, USHORT usLen)
{
    SHORT   sError=0, sRetry=5;
    ULONG   ulSendLen;

    memcpy(EEptHostSndBuf.auchFaddr, EEptProtocol.uchNetIpAddr, sizeof(EEptProtocol.uchNetIpAddr));
    EEptHostSndBuf.usFport = CLI_PORT_EEPT_HOST;
    EEptHostSndBuf.usLport = CLI_PORT_EEPT_HOST;
    ulSendLen = sizeof(EEptHostSndBuf.auchFaddr) +
			sizeof(EEptHostSndBuf.usFport) + sizeof(EEptHostSndBuf.usLport);

	if (usLen > sizeof(EEptHostSndBuf.auchData)) {
		NHPOS_ASSERT(0);  // when testing, ASSERT out if transfer data is too large for buffer
		usLen = sizeof(EEptHostSndBuf.auchData);
	}

	memcpy(EEptHostSndBuf.auchData, pEEptHostSndBuf, usLen);
    ulSendLen += usLen;

    do {
        if (5 > sRetry) {
            PifSleep(100);
        }
        if (sError == PIF_ERROR_NET_POWER_FAILURE) {    /* if power down ?  */
            EEptHostClose();                             /* close old port   */
            EEptHostOpen();                              /* open new port    */
        }
        sError = PifNetSend(hsEEptHostHandle,    
                            &EEptHostSndBuf,
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
** Synopsis:    SHORT   EEptNetRecv(USHORT usLen);
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
SHORT   EEptHostRecv(VOID *pEEptRcvBuf, USHORT usLen)
{
    SHORT   sError=0, sRetry=2;
	USHORT	usTimer = 500;      // 500 millisecond time out set

    PifNetControl(hsEEptHostHandle, PIF_NET_SET_TIME, usTimer);

    do {
        if (2 > sRetry) {
            PifSleep(100);
        }
        if (sError == PIF_ERROR_NET_POWER_FAILURE) {    /* if power down ?  */
            EEptHostClose();                             /* close old port   */
            EEptHostOpen();                              /* open new port    */
        }
        sError = PifNetReceive(hsEEptHostHandle,    
                            &EEptHostRcvBuf,
                            sizeof(EEptHostRcvBuf));
    } while (( (PIF_ERROR_NET_WRITE == sError)   && (--sRetry > 0) ) ||
             ( (PIF_ERROR_NET_OFFLINE == sError) && (--sRetry > 0) ) );

	// Need to convert PIF return into a CPM return code.
    if (sError >= PIF_OK) {                         /* E-067 corr. '94 4/26 */ 
		memcpy(pEEptRcvBuf, EEptHostRcvBuf.auchData, sError);
    }

	return(sError);
}

/* ---------- End of CPMHOSTIF.C ---------- */

