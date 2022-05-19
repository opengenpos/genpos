/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Clear Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosCls.C
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
** Synopsis:    VOID VosCls(USHORT usHandle);
*     Input:    usHandle - user handle of window
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosCls(USHORT usHandle)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_CLS, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_CLS, FAULT_INVALID_ENVIRON_2);
		return;
    }

    VosEditCls(pWin, 0, pWin->CharSize.usRow);  /* clear character window */
    pWin->Cursor.usRow = 0;                 /* new cursor position        */
    pWin->Cursor.usCol = 0;                 /* new cursor position        */
	if (! (pWin->usState & VOS_STATE_WBUTTONS)) {
		VosExecWindowActive(usReal, TRUE);
	}

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    VOID VosEditCls(VOSWINDOW FAR *pWin, USHORT usTopRow,
*                               USHORT usNumRow);
*     Input:    pWin     - address of window information
*               usTopRow - starting row of character window
*               usNumRow - number of row
*
** Return:      nothing
*===========================================================================
*/
VOID VosEditCls(VOSWINDOW FAR *pWin, USHORT usTopRow, USHORT usNumRow)
{
    USHORT  usRow, usRowC, usColC;
    TCHAR   FAR *puchBuf;

    usRow = pWin->CharBase.usRow + usTopRow;

    for (usRowC = 0; usRowC < usNumRow; usRowC++) {
        puchBuf = pWin->puchBuf +           /* get edit address  */
                  (usRow * pWin->PhySize.usCol + pWin->CharBase.usCol) * 2;

        for (usColC = 0; usColC < pWin->CharSize.usCol; usColC++) {
            *puchBuf++ = _T(' ');               /* space character   */
            *puchBuf++ = pWin->uchAttr;     /* default attribute */
        }

        usRow++;                            /* next row          */
    }
}


/* ===================================== */
/* ========== End of VosCls.C ========== */
/* ===================================== */
