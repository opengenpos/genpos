/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Scroll Down Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosSDown.C
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
#include <rfl.h>

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
** Synopsis:    VOID VosExecScrollDown(VOSWINDOW FAR *pWin, USHORT usNumRow);
*     Input:    pWin     - address of window information
*               usNumRow - number of scroll up
*
** Return:      nothing
*===========================================================================
*/
static VOID VosExecScrollDown(VOSWINDOW *pWin, USHORT usNumRow, USHORT usRowFirst)
{
    USHORT  usBytes, usSour, usDest;
	USHORT  usRowFirstSave = usRowFirst;

    if (usNumRow <= pWin->CharSize.usRow) {
        /* 
			We are going to perform a scroll down by moving data in the display
			buffer, pointed to by pWin->puchBuf.  We will need to compute a
			source offset, indicates where to get characters from, and a destination
			offset, indicates where to put characters to.

			Remember that this display buffer is composed of twice the number of bytes
			for the characters needed because the character data is stored in the buffer
			as two TCHARs, the first is the character and the second is the display
			attributes.

			usRowFirst indicates the first row that is involved in the scroll down.  If
			the scroll down is for the entire display area, usRowFirst will be equal to
			zero, the first line of the display.  Otherwise, it will be equal to some
			number from zero (0) up to but not including pWin->CharSize.usRow which contains
			the number of rows of characters in the display.

			To begin the scroll down, we will set the destination to the last row in the
			display area and we will set source to the row above it.  We will then do
			a row by row move of the display area until we have moved all rows up to
			and including the row specified in usRowFirst.
		**/

        usBytes = pWin->PhySize.usCol * 2;
		usDest  = (pWin->CharBase.usRow + pWin->CharSize.usRow - 1)  * usBytes;
		usSour  = usDest - usBytes;

		for (usRowFirst += 1; usRowFirst < pWin->CharSize.usRow; usRowFirst++) {
            memmove (pWin->puchBuf + usDest, pWin->puchBuf + usSour, usBytes * sizeof (TCHAR));
			usDest -= usBytes;
			usSour -= usBytes;
		}
    }
	else {
		usNumRow = pWin->CharSize.usRow;
		usRowFirstSave = 0;
	}

    /* --- fill space character, without top and bottom border --- */
    VosEditCls (pWin, usRowFirstSave, usNumRow);

    if (! (pWin->usState & VOS_STATE_WNORMAL &&  /* normal window */
           pWin->usState & VOS_STATE_OVERLAP)) { /* overlapped    */
        /* --- scroll down of video --- */

        EvsVioScrollDown ((USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow),
                         (USHORT)(pWin->PhyBase.usCol + pWin->CharBase.usCol),
                         (USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow + pWin->CharSize.usRow - 1),
                         (USHORT)(pWin->PhyBase.usCol + pWin->CharBase.usCol + pWin->CharSize.usCol - 1),
                         usNumRow,
                         pWin->uchAttr);   /* ### Mod (2171 for Win32) V1.0 */
    }

	// inform Framework that an update is needed to the displayed screen
	// failure to do so will cause the display to lag the internal data structs.
	__vEvsUpdateData_VOSWINDOW (pWin);  // send WU_EVS_UPDATE to Framework

    /* --- set new cursor position --- */
    pWin->Cursor.usRow = usRowFirstSave;
    pWin->Cursor.usCol = 0;
    VosExecCurPos (pWin);
}


/*
*===========================================================================
** Synopsis:    VOID VosScrollDown(USHORT usHandle, USHORT usNumber);
*     Input:    usHandle - user handle of window
*               usNumber - number of scroll down
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosScrollDown (USHORT usHandle, USHORT usNumber)
{
    USHORT      usReal;
    VOSWINDOW   *pWin;

    /* --- check number of scroll down --- */

    if (usNumber == 0) {                    /* invalid number */
        return;                             /* exit ...       */
    }

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW ||             /* range error    */
        ! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem (usVosApiSem);
        PifAbort (MODULE_VOS_SCROLLDOWN, FAULT_INVALID_HANDLE);
		return;
    }

    if (usNumber > pWin->CharSize.usRow) {
		PifReleaseSem (usVosApiSem);
        PifAbort (MODULE_VOS_SCROLLDOWN, FAULT_INVALID_ARGUMENTS);
		return;
    }

    VosExecWindowActive (usReal, FALSE);     /* active current window  */
    VosExecScrollDown (pWin, usNumber, 0);   /* execute scroll down    */

    PifReleaseSem (usVosApiSem);
}


__declspec( dllexport ) VOID VosScrollDownAtRow (USHORT usHandle, USHORT usNumber, USHORT usRowFirst)
{
    USHORT      usReal;
    VOSWINDOW   *pWin;

    /* --- check number of scroll down --- */
    if (usNumber == 0) {                    /* invalid number */
        return;                             /* exit ...       */
    }

    PifRequestSem (usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW ||             /* range error    */
        ! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem (usVosApiSem);
        PifAbort (MODULE_VOS_SCROLLDOWN, FAULT_INVALID_HANDLE);
		return;
    }

    if (usNumber > pWin->CharSize.usRow) {
		PifReleaseSem (usVosApiSem);
        PifAbort (MODULE_VOS_SCROLLDOWN, FAULT_INVALID_ARGUMENTS);
		return;
    }

    VosExecWindowActive (usReal, FALSE);        /* active current window  */
    VosExecScrollDown (pWin, usNumber, usRowFirst);  /* execute scroll down    */

    PifReleaseSem(usVosApiSem);
}



/* ======================================= */
/* ========== End of VosSDown.C ========== */
/* ======================================= */
