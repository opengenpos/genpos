/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Get Cursor Position Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosGCur.C
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
** Synopsis:    VOID VosGetCurPos(USHORT usHandle, USHORT *pusRow,
*                                 USHORT *pusCol);
*     Input:    usHandle - user handle of window
*    Output:    pusRow   -
*               pusCol   -
*
** Return:      VOS_SUCCESS - success
*               VOS_ERROR   - error
*===========================================================================
*/
__declspec( dllexport ) VOID VosGetCurPos(USHORT usHandle, USHORT *pusRow, USHORT *pusCol)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_GETCURPOS, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_GETCURPOS, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* --- get current cursor position --- */

    *pusRow = pWin->Cursor.usRow;
    *pusCol = pWin->Cursor.usCol;

    PifReleaseSem(usVosApiSem);
}


/* ====================================== */
/* ========== End of VosGCur.C ========== */
/* ====================================== */
