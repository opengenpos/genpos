/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Scroll Up Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosSUp.C
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
* Data      : Ver.     : Name       : Description
* Feb-23-95 : G0       : O.Nakada   : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#define  __EventSubSystem
#include "vos.h"
#include "vosl.h"
#include <rfl.h> /* ### Add (2171 for Win32) V1.0 */

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
** Synopsis:    VOID VosExecScrollUp(VOSWINDOW FAR *pWin, USHORT usNumRow);
*     Input:    pWin     - address of window information
*               usNumRow - number of scroll up
*
** Return:      nothing
*===========================================================================
*/
static VOID VosExecScrollUpAtRow(VOSWINDOW *pWin, USHORT usNumRow, USHORT usRowFirst)
{
    USHORT  usBytes, usSour, usDest;
	USHORT  usRowFirstSave = usRowFirst;

    if (usNumRow <= pWin->CharSize.usRow) {
        /* --- set scroll up information,    --- */
        /* --- without top and buttom border --- */

        usBytes = pWin->PhySize.usCol * 2;
		usDest  = (pWin->CharBase.usRow + usRowFirst)  * usBytes;
		usSour  = usDest + usBytes;

		for (usRowFirst += 1; usRowFirst < pWin->CharSize.usRow; usRowFirst++) {
            memmove (pWin->puchBuf + usDest, pWin->puchBuf + usSour, usBytes * sizeof (TCHAR));
			usDest += usBytes;
			usSour += usBytes;
		}
    }
	else {
		usNumRow = pWin->CharSize.usRow;
		usRowFirstSave = 0;
	}

    /* --- fill space character, without top and bottom border --- */
    VosEditCls(pWin, (USHORT)(pWin->CharSize.usRow - usNumRow), usNumRow);   /* ### Mod (2171 for Win32) V1.0 */

    if (! (pWin->usState & VOS_STATE_WNORMAL &&  /* normal window */
           pWin->usState & VOS_STATE_OVERLAP)) { /* overlapped    */
        /* --- scroll up of video --- */
        EvsVioScrollUp((USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow),
                       (USHORT)(pWin->PhyBase.usCol + pWin->CharBase.usCol),
                       (USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow + pWin->CharSize.usRow - 1),
                       (USHORT)(pWin->PhyBase.usCol + pWin->CharBase.usCol + pWin->CharSize.usCol - 1),
                       usNumRow,
                       pWin->uchAttr);    /* ### Mod (2171 for Win32) V1.0 */
    }

	// inform Framework that an update is needed to the displayed screen
	// failure to do so will cause the display to lag the internal data structs.
	__vEvsUpdateData_VOSWINDOW (pWin);  // send WU_EVS_UPDATE to Framework

    /* --- set new cursor position --- */
    pWin->Cursor.usRow = usRowFirst;
    pWin->Cursor.usCol = 0;
    VosExecCurPos(pWin);
}

VOID VosExecScrollUp (VOSWINDOW  *pWin, USHORT usNumRow)
{
	VosExecScrollUpAtRow (pWin, usNumRow, 0);
}

/*
*===========================================================================
** Synopsis:    VOID VosScrollUp(USHORT usHandle, USHORT usNumber);
*     Input:    usHandle - user handle of window
*               usNumber - number of scroll up
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosScrollUp(USHORT usHandle, USHORT usNumber)
{
    USHORT      usReal;
    VOSWINDOW  *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLUP, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLUP, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* --- check number of scroll up --- */
    if (usNumber == 0) {                    /* invalid number */
		PifReleaseSem(usVosApiSem);
        return;                             /* exit ...       */
    }

    if (usNumber > pWin->CharSize.usRow) {
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLDOWN, FAULT_INVALID_ARGUMENTS);
		return;
    }

    VosExecWindowActive(usReal, FALSE);     /* active current window  */
    VosExecScrollUp(pWin, usNumber);        /* execute scroll up      */

    PifReleaseSem(usVosApiSem);
}



__declspec( dllexport ) VOID VosScrollUpAtRow(USHORT usHandle, USHORT usNumber, USHORT usRowFirst)
{
    USHORT      usReal;
    VOSWINDOW  *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLUP, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLUP, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* --- check number of scroll up --- */
    if (usNumber == 0) {                    /* invalid number */
		PifReleaseSem(usVosApiSem);
        return;                             /* exit ...       */
    }

    if (usNumber > pWin->CharSize.usRow) {
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SCROLLDOWN, FAULT_INVALID_ARGUMENTS);
		return;
    }

    VosExecWindowActive(usReal, FALSE);     /* active current window  */
    VosExecScrollUpAtRow(pWin, usNumber, usRowFirst);   /* execute scroll up      */

    PifReleaseSem(usVosApiSem);
}



/* ===================================== */
/* ========== End of VosSUp.C ========== */
/* ===================================== */
