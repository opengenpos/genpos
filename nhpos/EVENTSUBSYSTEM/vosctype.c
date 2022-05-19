/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Cursor Control Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosCType.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
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
#include "log.h"
#define  __EventSubSystem
#include "vos.h"
#include "vosl.h"

#include "EvsL.h"

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    USHORT VosSetCurType(USHORT usHandle, USHORT usVisible);
*     Input:    usHandle  - user handle of window
*               usVisible - visible/invisible state
*
** Return:      previous cursor type
*===========================================================================
*/
__declspec( dllexport ) SHORT VosSetCurType(USHORT usHandle, USHORT usVisible)
{
    USHORT      usReal, usPrevious;
    VOSWINDOW  *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW ||             /* range error    */
        ! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
//		PifLog(MODULE_VOS_SETCURTYPE, FAULT_INVALID_HANDLE);
//		PifLog(MODULE_VOS_SETCURTYPE, usHandle);
		PifReleaseSem(usVosApiSem);  //Added for SR 808 JHHJ
		return VOS_ERRORVISIBLE;
    }

    /* --- get current cursor type --- */
    usPrevious = (pWin->usState & VOS_STATE_CTYPE) ? (VOS_VISIBLE) : (VOS_INVISIBLE);

    /* --- set new cursor type --- */
    if (usVisible == VOS_VISIBLE) {         /* visible state     */
        pWin->usState |= VOS_STATE_CTYPE;
    } else {                                /* invisible state   */
        pWin->usState &= ~VOS_STATE_CTYPE;
    }

    VosExecWindowActive(usReal, FALSE);
    VosExecCursor(pWin);                    /* set cursor status */

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/*
*===========================================================================
** Synopsis:    VOID VosExecCursor(VOSWINDOW FAR *pWin);
*     Input:    pWin - address of window information
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecCursor(VOSWINDOW FAR *pWin)
{
    /* --- check window type --- */
    if (pWin->usState & VOS_STATE_WNORMAL) {        /* normal window   */
        return;                                     /* exit ...        */
    }

    if (pWin->usState & VOS_STATE_CTYPE) {          /* visible state   */
        /* --- check cursor position --- */

        if (pWin->Cursor.usRow < pWin->CharSize.usRow &&    /* normal  */
            pWin->Cursor.usCol < pWin->CharSize.usCol) {
            VosExecCurPos(pWin);
        } else {                                            /* special */
            VosExecScrollUp(pWin, 1);
        }

        /* --- check physical status --- */
        if (! (VosVideo.usState & VOS_STATE_CTYPE)) {   /* invisible   */
            EvsVioCurOnOff(PIF_VIO_ON, pWin);
            VosVideo.usState |= VOS_STATE_CTYPE;
        }
    } else {                                        /* invisible state */
        /* --- check physical status --- */

        if (VosVideo.usState & VOS_STATE_CTYPE) {   /* visible         */
            EvsVioCurOnOff(PIF_VIO_OFF, pWin);
            VosVideo.usState &= ~VOS_STATE_CTYPE;
        }

        VosExecCurPos(pWin);
    }
}


/* ======================================= */
/* ========== End of VosCType.C ========== */
/* ======================================= */
