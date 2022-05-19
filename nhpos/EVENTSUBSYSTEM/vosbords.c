/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display String (on the window border) Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosBordS.C
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
** Synopsis:    VOID VosBorderString(USHORT usHandle, UCHAR *puchString,
*                                    USHORT usLength, UCHAR uchAttr);
*     Input:    usHandle   - user handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               uchAttr    - character attribute
*               usBorder   - display position on window border
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosBorderString(USHORT usHandle, TCHAR *puchString, USHORT usLength,
                   UCHAR uchAttr, USHORT usBorder)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;                  /* real window handle */
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_BORDERSTRING, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_BORDERSTRING, FAULT_INVALID_ENVIRON_2);
		return;
    }

    if (VosEditBorderString(pWin,
                            puchString,
                            usLength,
                            uchAttr,
                            usBorder) != VOS_SUCCESS) {
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_BORDERSTRING, FAULT_INVALID_ARGUMENTS);
 		return;
   }
    VosExecWindowActive(usReal, TRUE);

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    SHORT VosEditBorderString(VOSWINDOW FAR *pWin,
*                                         UCHAR *puchStr, USHORT usStrLen,
*                                         UCHAR uchAttr, USHORT usBorder);
*     Input:    pWin     - address of window information
*               puchStr  - string buffer
*               usStrLen - string length
*               uchAttr  - character attribute
*               usBorder - display position on window border
*
** Return:      nothing
*===========================================================================
*/
SHORT VosEditBorderString(VOSWINDOW FAR *pWin, TCHAR *puchStr,
                          USHORT usStrLen, UCHAR uchAttr, USHORT usBorder)
{
    USHORT  usRow, usCol, usBytes, usPos;
    TCHAR   FAR *puchBuf;

    /* --- check arguments --- */

    if (usBorder       & VOS_B_TOP &&           /* top border    */
        pWin->usBorder & VOS_B_TOP) {
        usRow = 0;
    } else if (usBorder       & VOS_B_BOTTOM && /* bottom border */
               pWin->usBorder & VOS_B_BOTTOM) {
        usRow = pWin->PhySize.usRow - 1;
    } else {
        return (VOS_ERROR);
    }

    /* --- set display string length --- */

    usBytes = pWin->CharSize.usCol;

    if (usStrLen < usBytes) {
        usBytes = usStrLen;
    }

    if (usBorder & VOS_B_LEFT) {            /* left side                 */
        if (pWin->usBorder & VOS_B_LEFT) {  /* visible border of left    */
            usCol = 1;
        } else {                            /* invisible border of left  */
            usCol = 0;
        }
    } else if (usBorder & VOS_B_RIGHT) {    /* right side                */
        if (pWin->usBorder & VOS_B_RIGHT) { /* visible border of right   */
            usCol = pWin->PhySize.usCol - usBytes - 1;
        } else {                            /* invisible border of right */
            usCol = pWin->PhySize.usCol - usBytes;
        }
    } else if (usBorder & VOS_B_CENTER) {   /* center                    */
        usCol = pWin->CharSize.usCol / 2 - (usBytes / 2 + usBytes % 2);
        if (pWin->usBorder & VOS_B_LEFT) {  /* visible border of left    */
            usCol += 1;
        }
    } else {                                /* undefined argument        */
        return (VOS_ERROR);
    }

    /* --- edit string on the window border --- */

    puchBuf = pWin->puchBuf + (usRow * pWin->PhySize.usCol + usCol) * 2;

    for (usPos = 0; usPos < usBytes; usPos++) {
        *puchBuf++ = *puchStr++;
        *puchBuf++ = uchAttr;
    }

    return (VOS_SUCCESS);
}


/* ======================================= */
/* ========== End of VosBordS.C ========== */
/* ======================================= */

