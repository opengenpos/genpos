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
* Title       : CHARGE POSTING MANAGER Send Task
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMSEND.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               CpmMainSendTask();
*               CpmSendCom();
*               CpmWaitPortRecovery();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-01-93 : 00.00.01 : Yoshiko.Jim  : Initial                                   
*
** NCR2171 **                                         
* Aug-26-99 : 01.00.00 : Mikio Teraki :initial (for 2171)
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
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
*=============================================================================
*
** Format  : VOID THREADENTRY   CpmMainSendTask(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function is the Main Send task of the Cpm Send Thread.
				The semaphore usCpmSemSend controls when to start execution of
				CpmMainSendTask(), to send User requests to the Cpm Host.

				There are four different states
					CPM_STATE_INSIDE - sending user request
					CPM_STATE_OUTSIDE - Waiting for next user request
					CPM_STATE_SEND_OK - Waiting on response to a request
					CPM_STATE_STOP - Not processing user requests

				In order to use CpmMainSendTask() the State must be set to CPM_STATE_INSIDE,
				the data must be copied to CpmSaveSend and the usCpmSemSend semaphore must be
				released:  see also CpmSendMessage()

				CpmSendCom() will modify the STATE to CPM_STATE_SEND_OK for a
				successful send otherwise the STATE is Unchanged by CpmSendCom()
				and the CpmMainSendTask() will retry the Send on the last retry
				the STATE will be changed to CPM_STATE_OUTSIDE
*
*=============================================================================
*/
VOID THREADENTRY    CpmMainSendTask(VOID)
{
    USHORT  i, usIntvlTimer;
    SHORT   sRet;

    for (;;) {
        PifRequestSem(usCpmSemSend);                /* wait data is written */
        usIntvlTimer = CpmGetMDCTime(MDC_CPM_INTVL_TIME);
                                                    /* get interval timer   */
/** send data is written **/
        if (uchCpmStatus != CPM_STATE_INSIDE) {
            continue;
        }
        for (i=0; i < CPM_RETRY_COUNT; i++) {       /* loop for retry       */
            sRet = CpmSendCom();                    /* execute data send    */
            if (sRet == CPM_RET_SUCCESS) {
                break;
            } else {
                if ( i == (CPM_RETRY_COUNT-1)) {    /* retry last time ?    */
                    if (CpmSaveSend.uchTerminal == CPM_FROM_SELF) { 
                                                    /* if request from self */
                        CpmSpool.sErrCode = sRet;
                        PifReleaseSem(usCpmSemRcv); /* release for SELF term*/
                        PifRequestSem(usCpmSemSpool); 
                                                    /* request for CpmSpool */
                    } else {
                        CpmReturnErrMessage(sRet);  /* return err message   */
                    }
                    CpmWriteStatus(CPM_STATE_OUTSIDE);
                                                    /* change idle status   */
                    break;
                }  else {
                    PifSleep((USHORT)(usIntvlTimer*1000));    /* wait interval time   */
                }
            }
        }
        if (sRet == CPM_RET_SUCCESS) {
            Para_CpmTally.usPMSSendOK++;                 /* incremnt tally count */
        } else {
            Para_CpmTally.usPMSSendErr++;                /* incremnt tally count */
        }                                           /* current process end  */
    }
}

/*
*=============================================================================
** Format  : SHORT   CpmSendCom(VOID);
*               
*    Input : none
*    InOut : none
* 
**   Return : CPM_RET_SUCCESS    -success to send data
*             CPM_RET_OFFLINE    -PMS offline err
*
** Synopsis: This function sends data to PMS (RS-232 Port).
*=============================================================================
*/
SHORT   CpmSendCom(VOID)
{
    SHORT  sRet;
    DATE_TIME   Timer;

    PifRequestSem(usCpmSemHandle);                  /* for usCpmStatus      */
    if (hsCpmPortHandle == CPM_PORT_NONE) {         /* if RS-232 port open? */
        sRet = CpmWaitPortRecovery();               /* wait port recovery   */
        if (sRet == CPM_RET_FAIL) {                 /* RS-232 port not open */
            return(CPM_RET_OFFLINE);                /* return port open err */
        }
    }

	// RJC CPM modified this section of code for sending to the CPM host
	// to either use RS-232 port or the LAN depending on system configuration.
	if (hsCpmPortHandle != CPM_PORT_USENET) {
		sRet = PifWriteCom(hsCpmPortHandle,
						   (VOID *)&CpmSaveSend.auchData[0],
						   CpmSaveSend.usLen);          /* send data to port    */

		PifGetDateTime(&Timer);                         /* get current time     */
		CpmSendTOD.uchHour = (UCHAR)Timer.usHour;
		CpmSendTOD.uchMin  = (UCHAR)Timer.usMin;
		CpmSendTOD.uchSec  = (UCHAR)Timer.usSec;

		sRet = CpmCheckComError(sRet, CpmSaveSend.usLen, CPM_SEND);
	}
	else {
		sRet = CpmHostSend((VOID *)&CpmSaveSend.auchData[0],
						   CpmSaveSend.usLen);          /* send data to port    */

		// If there was not error with the send then we indicate it was
		// successful by setting the sRet to CPM_RET_SUCCESS.
		// This in turn will trigger the state change to CPM_STATE_SEND_OK
		// which is needed for the further process of the request/response
		// cycle.
		if (sRet >= 0)
			sRet = CPM_RET_SUCCESS;

		PifGetDateTime(&Timer);                         /* get current time     */
		CpmSendTOD.uchHour = (UCHAR)Timer.usHour;
		CpmSendTOD.uchMin  = (UCHAR)Timer.usMin;
		CpmSendTOD.uchSec  = (UCHAR)Timer.usSec;
	}
                                                    /* check PifWriteCom()  */
    if (sRet == CPM_RET_SUCCESS) {
        uchCpmStatus = CPM_STATE_SEND_OK;           /* status change        */
    } else {
        sRet = CPM_RET_OFFLINE;
    }
    PifReleaseSem(usCpmSemHandle);                  /* for usCpmStatus      */
    return(sRet);
}

/*
*=============================================================================
** Format  : SHORT   CpmWaitPortRecovery(VOID);
*               
*    Input : none
*    InOut : none
* 
**   Return : CPM_RET_SUCCESS    -success to recover port
*             CPM_RET_FAIL       -fail to recover port      
*
** Synopsis: This function wait until port recovery.
*
*=============================================================================
*/
SHORT   CpmWaitPortRecovery(VOID)
{
    USHORT   i;


    if (hsCpmPortHandle != CPM_PORT_NONE) {         /* not open comm. port? */
        hsCpmSaveHandle = hsCpmPortHandle;          /* save previous handle */
        hsCpmPortHandle = CPM_PORT_NONE;            /* reset port handle    */
    }

    PifReleaseSem(usCpmSemHandle);
    for (i = 0; i < CPM_WAIT_PORT; i++) {           /* waiting loop         */
        if (hsCpmPortHandle != CPM_PORT_NONE) {
            PifRequestSem(usCpmSemHandle);
            return(CPM_RET_SUCCESS);                /* port recover success */
        } else {
            PifSleep(CPM_WAIT_PORT_TIME*1000);      /* wait                 */
        }
    }
    PifRequestSem(usCpmSemHandle);
    return(CPM_RET_FAIL);                           /* port recover fail    */
}



/* ---------- End of CPMSEND.C ---------- */


