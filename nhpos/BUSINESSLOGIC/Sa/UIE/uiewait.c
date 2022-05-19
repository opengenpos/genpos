/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Waiter Lock Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieWait.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Sep-09-92  00.01.02  O.Nakada     Initial
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include "ecr.h"
#include "pif.h"
#include "fsc.h"
#include "uie.h"
#include "uiel.h"
#include "uieio.h"
#include "uietrack.h"

                                                                                
/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

typedef struct _UIEWAITER {
    SHORT   sCurrent;                       /* current status     */
    SHORT   sAppl;                          /* application status */
} UIEWAITER;


STATIC  UIEWAITER   UieWaiter;


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetStatusWaiter(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieSetStatusWaiter(VOID)
{
    UieWaiter.sAppl = 0;                /* reset ID of waiter lock */
}


/*
*===========================================================================
** Synopsis:    VOID UieWaiterInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieWaiterInit(VOID)
{
    UieWaiter.sAppl = 0;
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadWaiterLock(FSCTBL *pFsc);
*    Output:    pFsc - address of FSC buffer
*
** Return:      UIE_SUCCESS - changed to waiter lock
*               UIE_ERROR   - not changed to waiter lock
*===========================================================================
*/
SHORT UieReadWaiterLock(FSCTBL *pFsc)
{
    /* --- check device status --- */
    
    if (! (fchUieDevice & UIE_ENA_WAITER)) {    /* disable waiter lock */
        return (UIE_ERROR);
    }
    
    if (UieWaiter.sAppl == UieWaiter.sCurrent) {   /* same waiter lock */
        return (UIE_ERROR);
    }

    /* --- changed waiter lock --- */

    UieWaiter.sAppl = UieWaiter.sCurrent;           /* set new ID       */

    /* --- set FSC --- */

    pFsc->uchMajor = FSC_WAITER_PEN;                /* major FSC        */
    pFsc->uchMinor = (UCHAR)UieWaiter.sAppl;        /* minor FSC        */

    UieTracking(UIE_TKEY_WAITER, pFsc->uchMinor, 0);
    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    LONG UiePutServerLockData(ULONG ulLength,
*                                         CONST MSR_BUFFER *pData,
*                                         VOID *pResult);
*
*     Input:    ulLength - length of data from pData
*               pData    - data from external module
*
*    Output:    pResult - reserved
*
** Return:      reserved
*
** Description: Accept ServerLock data and write to the ring buffer
*===========================================================================
*/
LONG UiePutServerLockData(ULONG ulLength, CONST SHORT *psData, VOID *pResult)
{
    UieWaiter.sCurrent = *psData;
    PifReleaseSem(usUieSyncNormal);                 /* wake-up user thread */

    return -1;
}


/* ====================================== */
/* ========== End of UieWait.C ========== */
/* ====================================== */
