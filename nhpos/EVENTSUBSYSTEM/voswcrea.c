/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Window Create Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosWCrea.C
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
#include <rfl.h> 

#include "EvsL.h"      /* ### ADD (2172 for Win32) V1.0 */

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
USHORT      usVosNumWindow;             /* number of window   */
VOSWINDOW   aVosWindow[VOS_NUM_WINDOW]; /* window information */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID VosCreateWindow(USHORT usRow, USHORT usCol,
*                                    USHORT usNumRow, USHORT usNumCol,
*                                    USHORT usFont, UCHAR uchAttr,
*                                    USHORT usBorder, USHORT *pusHandle);
*     Input:    usRow     - row at which to start window
*               usCol     - column at which to start window
*               usNumRow  - number of rows of character size
*               usNumCol  - number of column of character size
*               usFont    - character font
*               uchAttr   - character color and attribute
*               usBorder  - border attribute
*               pusHandle - address of handle, points to identifier such as LCDWIN_ID_COMMON001
*
*    Output:    pusHandle - address of handle, returns window handle
*
** Return:      nothing
*
** Description: Creates a window rectangle of the specified size for the specified
*               window identifier.  The window identifier indicates the particular
*               window type and is one of the defines such as LCDWIN_ID_COMMON001
*               that are specified in include file termcfg.h.
*===========================================================================
*/
__declspec( dllexport ) VOID VosCreateWindowRect(VOSRECT *pVosRect, USHORT usFont, UCHAR uchAttr,
                     USHORT usBorder, USHORT *pusHandle)
{

	VosCreateWindow(pVosRect->usRow, pVosRect->usCol, pVosRect->usNumRow,
                     pVosRect->usNumCol, usFont, uchAttr,
                     usBorder, pusHandle);
}

__declspec( dllexport ) VOID VosCreateWindow(USHORT usRow, USHORT usCol, USHORT usNumRow,
                     USHORT usNumCol, USHORT usFont, UCHAR uchAttr,
                     USHORT usBorder, USHORT *pusHandle)
{
    USHORT      usReal, usBytes;
    VOSWINDOW   *pWin;

#if !defined(TEST991026)
	usBorder = 0;
#endif
	
	_wEvsDebugTrace("CREATE WINDOW");

    if (! (usVosConfig & VOS_CONFIG_INIT) ||    /* not initialization     */
        ! (usVosConfig & VOS_CONFIG_LCD)) {     /* not LCD                */
        PifAbort(MODULE_VOS_CREATEWINDOW, FAULT_BAD_ENVIRONMENT);
		return;
    }

    PifRequestSem(usVosApiSem);

    /* --- check arguments --- */
/* ### DEL (2172 Rel1.0) 
    if (usRow > VosVideo.Size.usRow - 1 ||
        usCol > VosVideo.Size.usCol - 1 ||
        usNumRow == 0 || usNumRow > VosVideo.Size.usRow ||
        usNumCol == 0 || usNumCol > VosVideo.Size.usCol ||
        usFont != VOS_FONT_NORMAL) {
        PifAbort(MODULE_VOS_CREATEWINDOW, FAULT_INVALID_ARGUMENTS);
    }
*/

    /* --- get free window buffer --- */

    for (usReal = 0; usReal < VOS_NUM_WINDOW; usReal++) {
        if (aVosWindow[usReal].usState == VOS_STATE_FREE) {
            break;
        }
    }
    if (usReal >= VOS_NUM_WINDOW) {
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_CREATEWINDOW, FAULT_SHORT_RESOURCE);
		return;
    }

    /* --- set window information --- */

    pWin = &aVosWindow[usReal];             /* set address                */
    memset(pWin, 0, sizeof(VOSWINDOW));     /* initialization             */ /* ### Mod (2171 for Win32 V1.0) */
    pWin->PhyBase.usRow  = usRow;       /* physical base row of window    */
    pWin->PhyBase.usCol  = usCol;       /* physical base column of window */
    pWin->PhySize.usRow  = usNumRow;        /* row size of window         */
    pWin->PhySize.usCol  = usNumCol;        /* column size of window      */
    pWin->CharSize.usRow = usNumRow;        /* row size of character      */
    pWin->CharSize.usCol = usNumCol;        /* column size of character   */
    pWin->usFont         = usFont;          /* font number                */
    pWin->usBorder       = usBorder;        /* border attribute           */
    pWin->uchAttr        = uchAttr;         /* character attribute        */
	pWin->usIdentifier   = *pusHandle;      // remember identifier if caller provides one.

    if (VosUpdateWindow(pWin) != VOS_SUCCESS) {
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_CREATEWINDOW, FAULT_INVALID_ARGUMENTS);
		return;
    }

    usBytes       = pWin->PhySize.usRow * pWin->PhySize.usCol * 2;
    pWin->puchBuf = VosMemoryAlloc(*pusHandle, usBytes, &pWin->usMemInfo);
    pWin->usState = VOS_STATE_USE;          /* state of window            */
    usVosNumWindow++;                       /* number of window           */
    *pusHandle    = usReal + 1;             /* set handle                 */

    VosUpdateOverlapFlag(usReal, VOS_ENABLE);  /* set overlap flag        */
    VosInsertQueue(usReal);                 /* insert handle in queue     */

    VosEditBorder(pWin);                    /* edit window border         */
    VosEditCls(pWin, 0, pWin->CharSize.usRow); /* clear character window  */
    VosExecWindowActive(usReal, TRUE);      /* show current window        */

    /* ### ADD (2172 for Win32) V1.0 */
    __vEvsCreateData_VOSWINDOW(pWin); /* register data */

    PifReleaseSem(usVosApiSem);
}


/* used to get button dimensions from Framework - CSMALL */
__declspec( dllexport ) VOID VosGetWndTxtSize(USHORT usHandle, USHORT *usParmResult)
{
	ULONG       ulParmCommand = 1;
	VOSWINDOW   *pWin;

	if (usHandle > 1)
	{
		pWin = &aVosWindow[usHandle - 1];             /* set address                */

		__vEvsGetParam_VOSWINDOW(pWin, ulParmCommand, usParmResult);
	}

}

__declspec( dllexport ) VOID VosSetWndHasButtons(USHORT usHandle, USHORT usHasButtons)
{
	ULONG       ulParmCommand = 1;
	VOSWINDOW   *pWin;

	if (usHandle > 1)
	{
		pWin = &aVosWindow[usHandle - 1];             /* set address                */

		if (usHasButtons) {
			pWin->usState |= VOS_STATE_WBUTTONS;
		}
		else {
			pWin->usState &= ~VOS_STATE_WBUTTONS;
		}
	}

}


/*
*===========================================================================
** Synopsis:    SHORT VosUpdateWindow(VOSWINDOW FAR *pWin);
*     Input:    pWin - address of window information
*
** Return:      VOS_SUCCESS - success
*               VOS_ERROR   - error
*===========================================================================
*/
SHORT VosUpdateWindow(VOSWINDOW FAR *pWin)
{
    /* --- update window information by border attribute --- */

    if (pWin->usBorder & VOS_B_TOP) {       /* border of top    */
        pWin->PhySize.usRow++;
        pWin->CharBase.usRow++;
    }
    if (pWin->usBorder & VOS_B_BOTTOM) {    /* border of bottom */
        pWin->PhySize.usRow++;
    }
    if (pWin->usBorder & VOS_B_LEFT) {      /* border of left   */
        pWin->PhySize.usCol++;
        pWin->CharBase.usCol++;
    }
    if (pWin->usBorder & VOS_B_RIGHT) {     /* border of right  */
        pWin->PhySize.usCol++;
    }

    /* --- check physical start position of window --- */

    if (pWin->PhyBase.usRow > VosVideo.Size.usRow - 1 ||
        pWin->PhyBase.usCol > VosVideo.Size.usCol - 1) {
        return (VOS_ERROR);
    }

    /* --- check physical size of window --- */

    if (pWin->PhySize.usRow == 0 ||
        pWin->PhySize.usRow > VosVideo.Size.usRow ||
        pWin->PhySize.usCol == 0 ||
        pWin->PhySize.usCol > VosVideo.Size.usCol) {
        return (VOS_ERROR);
    }

    /* --- check physical end position of window --- */

    if (pWin->PhyBase.usRow + pWin->PhySize.usRow > VosVideo.Size.usRow ||
        pWin->PhyBase.usCol + pWin->PhySize.usCol > VosVideo.Size.usCol) {
        return (VOS_ERROR);
    }

    /* --- check size of character area --- */

    if (pWin->CharBase.usRow > VosVideo.Size.usRow - 1 ||
        pWin->CharBase.usCol > VosVideo.Size.usCol - 1 ||
        pWin->CharSize.usRow > VosVideo.Size.usRow ||
        pWin->CharSize.usCol > VosVideo.Size.usCol) {
        return (VOS_ERROR);
    }

    return (VOS_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID VosEditBorder(VOSWINDOW FAR *pWin);
*     Input:    pWin - address of window information
*
** Return:      nothing
*===========================================================================
*/
VOID VosEditBorder(VOSWINDOW FAR *pWin)
{
    TCHAR   uchChar;
    USHORT  usPos;
    TCHAR   FAR *puchBuf;

    /* --- edit horizontal border --- */

    if (pWin->usBorder & VOS_B_TOP) {       /* visible border of top */
        uchChar = BORDER_H(pWin->usBorder);
        puchBuf = pWin->puchBuf;

        for (usPos = 0; usPos < pWin->PhySize.usCol; usPos++) {
            *puchBuf++ = uchChar;           /* border character    */
            *puchBuf++ = pWin->uchAttr;     /* character attribute */
        }
    }

    if (pWin->usBorder & VOS_B_BOTTOM) {    /* visible border of buttom */
        uchChar = BORDER_H(pWin->usBorder);
        puchBuf = pWin->puchBuf +
                  pWin->PhySize.usCol * (pWin->PhySize.usRow - 1) * 2;

        for (usPos = 0; usPos < pWin->PhySize.usCol; usPos++) {
            *puchBuf++ = uchChar;           /* border character    */
            *puchBuf++ = pWin->uchAttr;     /* character attribute */
        }
    }

    /* --- edit vertical border --- */

    if (pWin->usBorder & VOS_B_LEFT) {      /* visible border of left */
        uchChar = BORDER_V(pWin->usBorder);
        puchBuf = pWin->puchBuf;

        for (usPos = 0; usPos < pWin->PhySize.usRow; usPos++) {
            * puchBuf      = uchChar;       /* border character    */
            *(puchBuf + 1) = pWin->uchAttr; /* character attribute */

            puchBuf += pWin->PhySize.usCol * 2;
        }
    }

    if (pWin->usBorder & VOS_B_RIGHT) {     /* visible border of right */
        uchChar = BORDER_V(pWin->usBorder);
        puchBuf = pWin->puchBuf + (pWin->PhySize.usCol - 1) * 2;

        for (usPos = 0; usPos < pWin->PhySize.usRow; usPos++) {
            * puchBuf      = uchChar;       /* border character    */
            *(puchBuf + 1) = pWin->uchAttr; /* character attribute */

            puchBuf += pWin->PhySize.usCol * 2;
        }
    }

    /* --- edit corner border --- */

    if (pWin->usBorder & VOS_B_TOP &&
        pWin->usBorder & VOS_B_LEFT) {
          puchBuf      = pWin->puchBuf;
        * puchBuf      = BORDER_TL(pWin->usBorder);
        *(puchBuf + 1) = pWin->uchAttr;
    }

    if (pWin->usBorder & VOS_B_TOP &&
        pWin->usBorder & VOS_B_RIGHT) {
          puchBuf      = pWin->puchBuf +
                         (pWin->PhySize.usCol - 1) * 2;
        * puchBuf      = BORDER_TR(pWin->usBorder);
        *(puchBuf + 1) = pWin->uchAttr;
    }

    if (pWin->usBorder & VOS_B_BOTTOM &&
        pWin->usBorder & VOS_B_LEFT) {
          puchBuf      = pWin->puchBuf +
                         (pWin->PhySize.usRow - 1) * pWin->PhySize.usCol * 2;
        * puchBuf      = BORDER_BL(pWin->usBorder);
        *(puchBuf + 1) = pWin->uchAttr;
    }

    if (pWin->usBorder & VOS_B_BOTTOM &&
        pWin->usBorder & VOS_B_RIGHT) {
          puchBuf      = pWin->puchBuf +
                         (pWin->PhySize.usRow * pWin->PhySize.usCol - 1) * 2;
        * puchBuf      = BORDER_BR(pWin->usBorder);
        *(puchBuf + 1) = pWin->uchAttr;
    }
}


/* ======================================= */
/* ========== End of VosWCrea.C ========== */
/* ======================================= */
