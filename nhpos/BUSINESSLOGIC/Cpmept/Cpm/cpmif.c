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
* Title       : CHARGE POSTING MANAGER User Interface Routines
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMIF.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               CpmInit();
*                   CpmInitialArea();
*                   CpmGetComPort();
*               CpmSendMessage();
*               CpmReceiveMessage();
*               CpmChangeStatus();
*               CpmReadResetTally();
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
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include	<tchar.h>

#include <memory.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "plu.h"
#include "termcfg.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "cpm.h"
#include "rfl.h"
#include "cpmlocal.h"
#include "uie.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/**     protocol       **/  
PROTOCOL    CpmProtocol;                            /* protocol for CPM     */

/**     semaphore       **/  
PifSemHandle  usCpmSemMain = PIF_SEM_INVALID_HANDLE;                               /* for access to stub   */
PifSemHandle  usCpmSemSend = PIF_SEM_INVALID_HANDLE;                               /* for start send thred */
PifSemHandle  usCpmSemRcv = PIF_SEM_INVALID_HANDLE;                                /* for inform user I/F  */
PifSemHandle  usCpmSemSpool = PIF_SEM_INVALID_HANDLE;                              /* for CpmSpool         */
PifSemHandle  usCpmSemState = PIF_SEM_INVALID_HANDLE;                              /* for uchCpmStatus     */
PifSemHandle  usCpmSemHandle = PIF_SEM_INVALID_HANDLE;                             /* for hsCpmPortHandle  */

/**     RS-232 Port     **/  
CHAR    chCpmPortID;                                /* RS-232 Port connect# */
SHORT   hsCpmPortHandle;                            /* port handle saved    */
SHORT   hsCpmSaveHandle;                            /* port handle backup   */

/**     CPM STATUS      **/  
UCHAR  uchCpmStatus;                                /* current state of CPM */

/**     SEND DATA       **/  
CPMSAVESEND CpmSaveSend;                            /* send data save area  */

/**     send time       **/  
CPMTIMER    CpmSendTOD;                             /* message send time    */

/**
;=============================================================================
;+              E X T E R N A L  R A M s
;=============================================================================
**/

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/

/*
*=============================================================================
*
** Format  : VOID   CpmInit(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function initializes CPM.
*               This Creates semaphore, creates thread of send & receive,
*               and opens port for RS-232 and IHC.
*=============================================================================
*/
VOID    CpmInit(VOID)
{
	/**     thread control  **/   
	UCHAR  CONST aszCpmRcvThreadName[] = "CPM_RCV";      /* receive name     */
	UCHAR  CONST aszCpmSendThreadName[] = "CPM_SEND";    /* send name        */
	VOID (THREADENTRY *pFunc1)(VOID) = CpmMainReceiveTask;    /* Add R3.0 */
	VOID (THREADENTRY *pFunc2)(VOID) = CpmMainSendTask;    /* Add R3.0 */

    chCpmPortID = (UCHAR)CpmGetComPort();           /* get RS-232 port ID   */
    if (chCpmPortID == CPM_PORT_NONE) {         
        return;
    }

    CpmInitialArea();
													
    usCpmSemMain = PifCreateSem(1);                 // forces only one request for CpmSendMessage() in a multithreaded environment
    usCpmSemSend  = PifCreateSem(0);                // allows CpmMainSendTask() to start a send of a user request
    usCpmSemState = PifCreateSem(1);                /* controls access to uchCpmStatus so that only one thread can chage or read it at a time   */
    usCpmSemHandle = PifCreateSem(1);               /* controls access to the Com Port handle: hsCpmPortHandle  */
    usCpmSemRcv  = PifCreateSem(0);                 /* allows */
    usCpmSemSpool  = PifCreateSem(0);               /* for CpmSpool         */

    CpmSetProtocol();                               /* Set protocol data    */
    if (CpmSetReadTimer(0) == CPM_RET_COM_ERRORS) { /* open RS-232 port     */ //SR517
		UieErrorMsg(LDT_CPM_OFFLINE_ADR, UIE_WITHOUT);		
	}

    if (chCpmPortID == CPM_PORT_USENET) {         
		CpmHostOpen();
	}

    PifBeginThread(pFunc1, 0, 0, PRIO_CPM_RCV, aszCpmRcvThreadName);     /* start Send thread    */
    PifBeginThread(pFunc2, 0, 0, PRIO_CPM_SEND, aszCpmSendThreadName);

    CpmNetOpen();                                   /* open IHC port        */
}

/*
*=============================================================================
*
** Format  : VOID   CpmInitialArea(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function initializes static area of CPM.
*=============================================================================
*/
VOID    CpmInitialArea(VOID)
{
    uchCpmStatus = CPM_STATE_OUTSIDE;               /* current state of CPM */
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmInitialArea: CPM_STATE_OUTSIDE"));
#endif
    hsCpmPortHandle = CPM_PORT_NONE;                /* port handle saved    */
    hsCpmSaveHandle = CPM_PORT_NONE;                /* port handle backup   */
    hsCpmNetHandle = CPM_PORT_NONE;                 /* IHC port handle      */
    return;
}

/*                                  
*=============================================================================
** Format  : SHORT   CpmGetComPort(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 0 >=          ; Port ID
*            CPM_PORT_NONE ; nothing  
*
** Synopsis: This function search port id for PMS.
*=============================================================================
*/
SHORT   CpmGetComPort(VOID)
{
    SYSCONFIG CONST FAR *pSys;
    USHORT i;

    pSys = PifSysConfig();                      /* get system configration  */

	if (pSys->usInterfaceDetect & SYSCONFIG_IF_CPM_LAN) {
		return (CPM_PORT_USENET);
	}

    if (!((pSys->uchCom & COM_PROVIDED) == COM_PROVIDED)) {   /* exist RS-232 board ?     */
		return(CPM_PORT_NONE);
    }

    for (i=1; i < PIF_LEN_PORT; i++) {          /* get port ID              */
                                                /* auchComPort[0] = R/J     */
        if (pSys->auchComPort[i] == DEVICE_CHARGE_POSTING) {
            return((SHORT)i);
        }
    }
    return(CPM_PORT_NONE);
}

/*
*=============================================================================
** Format  : SHORT   CpmSendMessage(UCHAR *pSendData, USHORT usSendLen,
*                                   UCHAR uchTerminal);
*               
*    Input : pSendData      pointer of send data
*            usSendLen      length of send data
*            uchTerminal    self or not self terminal
*    InOut : none
* 
**   Return : CPM_RET_SUCCESS    -accept send process
*             CPM_RET_BUSY       -reject process
*             CPM_RET_STOP       -stopping PMS
*             CPM_RET_PORT_NONE  -PMS is not provide
*
** Synopsis: This function sends data to PMS.

	For an example of how this function, CpmSendMessage () is used in
	conjunction with CpmReceiveMessage () to communicate with a Charge
	Post Management System, see function ItemTendCPComm () in file itend5.c
	in the Tender project of the BusinessLogic subsystem.

	This function starts the sending of a user request to the Charge Post Host
	by copying the user request to the central send buffer CpmSaveSend and then
	setting the global variable uchCpmStatus to CPM_STATE_INSIDE indicating
	that a send is in progress.
*=============================================================================
*/
SHORT   CpmSendMessage(UCHAR *pSendData, USHORT usSendLen, UCHAR uchTerminal)
{
    USHORT  usCompare;

    if (chCpmPortID == CPM_PORT_NONE) {         
        return(CPM_RET_PORT_NONE);                  /* ret PMS not provide  */
    }
    PifRequestSem(usCpmSemMain);                    /* for access to stub   */
    PifRequestSem(usCpmSemState);                   /* for uchCpmStatus     */

    switch (uchCpmStatus) {                         /* current CPM status ? */
        case CPM_STATE_STOP:
            PifReleaseSem(usCpmSemState);
            PifReleaseSem(usCpmSemMain);
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmSendMessage: CPM_STATE_STOP"));
#endif
            return(CPM_RET_STOP);                   /* ret CPM is stopping  */

        case CPM_STATE_SEND_OK:
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmSendMessage: CPM_STATE_SEND_OK"));
#endif
        case CPM_STATE_INSIDE:
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmSendMessage: CPM_STATE_INSIDE"));
#endif
            usCompare = CpmComparePrevious(pSendData, usSendLen);
            if (usCompare == CPM_SAME) {
                PifReleaseSem(usCpmSemState);
                PifReleaseSem(usCpmSemMain);
                return(CPM_RET_SUCCESS);            /* ret process accepted */

            } else {
                PifReleaseSem(usCpmSemState);       /* for usCpmStatus      */
                PifReleaseSem(usCpmSemMain);
                return(CPM_RET_BUSY);               /* ret process rejected */
            }

        case CPM_STATE_OUTSIDE:
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmSendMessage: CPM_STATE_OUTSIDE"));
#endif
            break;                                  /* go to next process   */
    }
                                                    /* accept process start */
    uchCpmStatus = CPM_STATE_INSIDE;                /* status change        */
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmSendMessage: CPM_STATE_INSIDE"));
#endif

	NHPOS_ASSERT(sizeof(CpmSaveSend.auchData) <= usSendLen);

    memcpy(CpmSaveSend.auchData, pSendData, usSendLen);
                                                    /* copy send data       */
    CpmSaveSend.usLen = usSendLen;                  /* copy length of data  */
    CpmSaveSend.uchTerminal = uchTerminal;          /* copy called from xxx */

    PifReleaseSem(usCpmSemState);                   /* for chCpmStatus      */
    PifReleaseSem(usCpmSemSend);                    /* for start send thred */ 
    PifReleaseSem(usCpmSemMain);                    /* for access to stub   */
    return(CPM_RET_SUCCESS);
}

/*
*=============================================================================
** Format  : SHORT   CpmReceiveMessage(UCHAR *pRecData, USHORT usRecSize);
*               
*    Input : pRecData       pointer of receive data
*            usRecSize      length of receive data
*    InOut : none
* 
**   Return : >= 0               -length of response message 
*             CPM_RET_PORT_NONE  -PMS is not provide
*             CPM_RET_LENGTH     -length error
*
** Synopsis: This function sends received data to SELF (called from only SELF terminal).
*=============================================================================
*/
SHORT   CpmReceiveMessage(UCHAR *pRecData, USHORT usRecSize)
{
    USHORT  usCompare;
    SHORT   sRet;
    CPMRCRSPDATA    *pRec;

    pRec = (CPMRCRSPDATA *)&CpmSpool.auchData[0];

    if (chCpmPortID == CPM_PORT_NONE) {         
        return(CPM_RET_PORT_NONE);                  /* ret PMS not provide  */
    }

    PifRequestSem(usCpmSemRcv);                     /* for inform rcv data  */
    PifRequestSem(usCpmSemState);                   /* for usCpmStatus      */

    if (CpmSpool.sErrCode < 0) {
        sRet = CpmSpool.sErrCode;                   /* if receive task err  */
    } else {
        if (CpmSpool.usLen <= usRecSize) {          /* save area size is OK */ 
            usCompare = CpmCheckSpoolData();        /* check send address   */
            if (usCompare == CPM_SAME) {
                memcpy(pRecData, CpmSpool.auchData, CpmSpool.usLen);
                sRet = (SHORT)CpmSpool.usLen;       /* return read length   */
            } else {
                sRet = CPM_RET_FAIL;                /* return length error  */
            }
        } else {
            sRet = CPM_RET_LENGTH;                  /* return length error  */
        }
    } 
    PifReleaseSem(usCpmSemSpool);                   /* for CpmSpool         */
    if ((pRec->auchLastMsgInd[0] == CPM_LAST_MSG)   /* last message ?       */
        || (sRet < 0) ) {                           /* or error status ?    */
        uchCpmStatus = CPM_STATE_OUTSIDE;           /* change idle status   */
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmReceiveMessage: CPM_STATE_OUTSIDE"));
#endif
    }
    PifReleaseSem(usCpmSemState);                   /* for uchCpmStatus     */
    return(sRet);
}

/*
*=============================================================================
*
** Format  : SHORT   CpmChangeStatus(UCHAR uchState);
*               
*    Input :    uchState    - status of want to change
*                   CPM_STATE_START     : start PMS
*                   CPM_STATE_STOP      : stop PMS
*
*   Output :    nothing
*    InOut :    nothing 
*
**   Return : CPM_RET_SUCCESS    -success change
*             CPM_RET_BUSY       -not change because busy 
*             CPM_RET_PORT_NONE  -PMS isn't connected
*
** Synopsis:    This function write uchCpmStatus.
*=============================================================================
*/
SHORT   CpmChangeStatus(UCHAR uchState)
{
    if (chCpmPortID == CPM_PORT_NONE) {         
        return(CPM_RET_PORT_NONE);                  /* ret PMS not provide  */
    }
    PifRequestSem(usCpmSemState);                   /* for uchCpmStatus     */

    if ((uchCpmStatus == CPM_STATE_SEND_OK) || (uchCpmStatus == CPM_STATE_INSIDE)) {       /* current CPM status ? */
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmChangeStatus: CPM_STATE_SEND_OK || CPM_STATE_INSIDE, CPM_RET_BUSY"));
#endif
        PifReleaseSem(usCpmSemState);
        return(CPM_RET_BUSY);                       /* ret error change     */
    }
    uchCpmStatus = uchState;                        /* status change        */
#ifdef _DEBUG
    OutputDebugString(TEXT("CpmChangeStatus: state Changed uchCpmStatus = uchState"));
#endif
    PifReleaseSem(usCpmSemState);
    return(CPM_RET_SUCCESS);                        /* ret success change   */
}

/*
*=============================================================================
*
** Format  : VOID   CpmReadResetTally(UCHAR uchType, CPMTALLY *pTally);
*               
*    Input :    uchType     - required type
*                   CPM_TALLY_READ     : read tally
*                   CPM_TALLY_RESET    : reset tally
*               pTally      - pointer of setting area
*
*   Output :    nothing
*    InOut :    nothing 
*
**   Return : none
*
** Synopsis:    This function read tally to pTally and reset tally if required.
*=============================================================================
*/
VOID   CpmReadResetTally(UCHAR uchType, CPMTALLY *pTally)
{
	if (pTally) {
		*pTally = Para.CpmTally;
	}

    if (uchType == CPM_TALLY_RESET) {
        memset(&Para.CpmTally, 0, sizeof(Para.CpmTally));
    }
    return;
}

/* ---------- End of CPMIF.C ---------- */

