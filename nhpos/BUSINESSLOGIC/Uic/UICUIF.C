/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : User interface Module for Master Terminal                        
* Category    : UIC user interface, NCR 2170 US Hospitality Application         
* Program Name: UICUIF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UicInit()        : Initialize UIC Thread
*               UicCheckAndSet() : Check and set the Terminal Lock Flag
*               UicResetFlag()   : Reset the Terminal Lock Flag
*               UicStart()       : Starts UIC Task
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
*          :        :           :                                    
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#include <tchar.h>
#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <nb.h>
#include <uic.h>
#include <storage.h>
#include "uiccom.h"

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
SHORT   husUicHandle = 0;                              /* Save a handle */
USHORT  fsUicExecuteFG = UIC_IDLESTATE;                /* Execute Request parameter and Backup copy */
PifSemHandle  husUicFlagSem = PIF_SEM_INVALID_HANDLE;  /* Control set/reset terminal  */
PifSemHandle  husUicExecSem = PIF_SEM_INVALID_HANDLE;  /* Control to start task       */

static USHORT  fsUicTerLockFG = UIC_IDLESTATE;         /* Terminal lock flag */
                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UicInit(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Main Application Thread
*===========================================================================
*/
VOID UicInit(VOID)
{
	VOID (THREADENTRY *pFunc)(VOID) = UicMain;      /* UIC Thread entry point */
	CHAR CONST  *UICTHREDNAME = "UIC";              /* UIC Thread Name */

    husUicFlagSem = PifCreateSem(1);        /* Create Flag Semaphore */
    husUicExecSem = PifCreateSem(0);        /* Create Execute Semaphore */

    PifBeginThread(pFunc, 0, 0, PRIO_UIC, UICTHREDNAME);
}

/*
*===========================================================================
** Synopsis:    SHORT UicCheckAndSet(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Handle (If SUCCESS) or UIC_NOT_IDLE  (If error )
*
** Description: Check terminal lock flag, if idle then set "Executing ".
*               Check and set the keyboard and transaction lock.  this lock is used
*               to ensure that Cashier can not Sign-in if there is a remote application
*               that has locked the keyboard for actions that must not be done in
*               parallel to Cashier actions.
*
*               WARNING: if the lock works, you must use UicResetFlag() to unlock.
*                        we have seen mysterious "During Lock" (error 22) error
*                        dialogs displayed under some circumstance which were due
*                        to a successful lock that was not reset when an error
*                        occurred causing the action to be aborted or rejected.
*===========================================================================
*/
SHORT  UicCheckAndSet(VOID)
{
    SHORT sRet;
    DATE_TIME pDate;

    PifRequestSem(husUicFlagSem);
    
    if ( fsUicTerLockFG == UIC_IDLESTATE ) {
        fsUicTerLockFG = UIC_EXECSTATE;           /* set Executeing state */
        PifGetDateTime(&pDate);
		// We are generated a semi-unique handle by using the seconds of the
		// current time.  If seconds is zero then set to a fixed value as
		// zero means not in use.
        sRet = pDate.usSec;
        if (pDate.usSec == 0) {
            sRet = UIC_FIXEDHND;
        }
        husUicHandle = sRet;
    } else {
        sRet = UIC_NOT_IDLE;
    }
    
    PifReleaseSem(husUicFlagSem);
    return (sRet);
}
 
/*
*===========================================================================
** Synopsis:    SHORT UicResetFlag(husHandle)
*     Input:    husHandle
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS   or UIC_OTHER_USE
*
** Description: Reset terminal lock flag that was set by UicCheckAndSet().
*===========================================================================
*/
SHORT  UicResetFlag(SHORT husHandle)
{
    SHORT sRet;

    sRet = UIC_SUCCESS;

    PifRequestSem(husUicFlagSem);

    if ( husUicHandle == husHandle) {           /* If user is same, then OK */
        fsUicTerLockFG = UIC_IDLESTATE;         /* set Idle state */
		husUicHandle = 0;
    }  else {
        sRet = UIC_OTHER_USE;                   /* set error code */
    }

    PifReleaseSem(husUicFlagSem);
    return (sRet);
}


/*
*===========================================================================
** Synopsis:    SHORT UicStart(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS
*
** Description: Starts the UIC thread if the UIC thread at UicMain ()
*               is sitting at idle on semaphore husUicExecSem.
*               This function is invoked by function NbRcvMesHand()
*               when it is processing an NB_FCSTARTASMASTER function
*               code in the latest message received over the
*               Notice Board cluster communications link.
*===========================================================================
*/
SHORT  UicStart(VOID)
{
    PifRequestSem(husUicFlagSem);
    
    if ( fsUicExecuteFG == UIC_IDLESTATE ) {    /* If not start, then set flag */
        fsUicExecuteFG = UIC_EXECSTATE;         /* Sets Executeing state */
        PifReleaseSem(husUicExecSem);           /* Start UIC thred */
    }
    
    PifReleaseSem(husUicFlagSem);
    return (UIC_SUCCESS);

}



/*====== End of Source File ======*/
