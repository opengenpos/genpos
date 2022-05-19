/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Set Cursor Position Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosSCur.C
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
** Synopsis:    VOID VosSetCurPos(USHORT usHandle, USHORT usRow,
*                                 USHORT usCol);
*     Input:    usHandle - user handle of window
*               usRow    - row position
*               usCol    - column position
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosSetCurPos(USHORT usHandle, USHORT usRow, USHORT usCol)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

	NHPOS_ASSERT(usHandle > 0);

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
		NHPOS_ASSERT(usReal < VOS_NUM_WINDOW);
        PifAbort(MODULE_VOS_SETCURPOS, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
		NHPOS_ASSERT(pWin->usState & VOS_STATE_USE);
        PifAbort(MODULE_VOS_SETCURPOS, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* --- check cursor position --- */

    if (pWin->usState & VOS_STATE_CTYPE) {      /* cursor visible         */
        if (usRow >= pWin->CharSize.usRow ||    /* range error of row     */
            usCol >= pWin->CharSize.usCol) {    /* range error of column  */
			PifReleaseSem(usVosApiSem);
			NHPOS_ASSERT(usRow < pWin->CharSize.usRow);
			NHPOS_ASSERT(usCol < pWin->CharSize.usCol);
            PifAbort(MODULE_VOS_SETCURPOS, FAULT_INVALID_ENVIRON_3);
			return;
        }
    } else {                                    /* cursor invisible       */
        if (usRow >= pWin->CharSize.usRow ||    /* range error of row     */
            usCol >  pWin->CharSize.usCol) {    /* range error of column  */
			PifReleaseSem(usVosApiSem);
			NHPOS_ASSERT(usRow < pWin->CharSize.usRow);
			NHPOS_ASSERT(usCol <= pWin->CharSize.usCol);
            PifAbort(MODULE_VOS_SETCURPOS, FAULT_INVALID_ENVIRON_4);
			return;
        }
    }

    /* --- set new cursor position --- */

    pWin->Cursor.usRow = usRow;
    pWin->Cursor.usCol = usCol;

    VosExecWindowActive(usReal, FALSE);
    VosExecCurPos(pWin);

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    VOID VosExecCurPos(VOSWINDOW FAR *pWin);
*     Input:    pWin - address of window information
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecCurPos(VOSWINDOW FAR *pWin)
{
    USHORT  usRow, usCol;

    /* --- check window type --- */

    if (pWin->usState & VOS_STATE_WNORMAL) {        /* normal window     */
        return;                                     /* exit ...          */
    }

    /* --- get physical position --- */

    usRow = pWin->PhyBase.usRow + pWin->CharBase.usRow + pWin->Cursor.usRow;
    usCol = pWin->PhyBase.usCol + pWin->CharBase.usCol + pWin->Cursor.usCol;

    /* --- check cursor position of video information --- */

    if (VosVideo.Cursor.usRow == usRow &&   /* same position */
        VosVideo.Cursor.usCol == usCol) {
        return;                             /* exit ...      */
    }

    /* --- set new cursor position --- */

    EvsVioSetCurPos(usRow, usCol, usVosPage, pWin);
    VosVideo.Cursor.usRow = usRow;
    VosVideo.Cursor.usCol = usCol;
}


/* ====================================== */
/* ========== End of VosSCur.C ========== */
/* ====================================== */
