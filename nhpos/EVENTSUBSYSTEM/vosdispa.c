/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display String (with Attribute) Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosDispA.C
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
** Synopsis:    VOID VosDisplayAttr(USHORT usHandle, USHORT usRow,
*                                   USHORT usCol, UCHAR *puchString,
*                                   USHORT usLength);
*     Input:    usHandle   - user handle of window
*               usRow      - start row
*               usCol      - start column
*               puchString - address of string buffer
*               usLength   - length of string buffer
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosDisplayAttr(USHORT usHandle, USHORT usRow, USHORT usCol,
                    TCHAR *puchString, USHORT usLength)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

    /* --- check length of string --- */
    if (usLength == 0) {                    /* invalid length         */
        return;
    }

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;                  /* real window handle */
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_DISPLAYATTR, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_DISPLAYATTR, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* --- check cursor position --- */

    if ((usRow >= pWin->CharSize.usRow ||   /* range error of row     */
         usCol >= pWin->CharSize.usCol) &&  /* range error of column  */
        (usRow != pWin->CharSize.usRow &&   /* cursor for buttom side */
         usCol != pWin->CharSize.usCol)) {  /* cursor for right side  */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_DISPLAYATTR, FAULT_INVALID_ARGUMENTS);
		return;
    }

    /* --- set new cursor position --- */

    pWin->Cursor.usRow = usRow;
    pWin->Cursor.usCol = usCol;

    VosExecWindowActive(usReal, FALSE);
    VosExecCursor(pWin);                    /* set cursor status      */
    VosExecStringAttr(usReal, puchString, (USHORT)(usLength * 2), pWin->uchAttr, (USHORT)TRUE);  /* ### Mod (2171 for Win32) V1.0 */

    PifReleaseSem(usVosApiSem);
}


/* ======================================= */
/* ========== End of VosDispA.C ========== */
/* ======================================= */
