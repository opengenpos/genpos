/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Popup Display
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDPOPUP.C
* --------------------------------------------------------------------------
* Abstract:
*        MldCreatePopUp(): create popup window
*        MldDeletePopUp(): delete popup window
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-27-95 : 03.00.00 :  M.Ozawa   : Initial
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <rfl.h>
#include <vos.h>
#include <bl.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
TCHAR FARCONST  aszMldOrderPop[] = _T("%02d %s\n"); /* 2172 */
TCHAR FARCONST  aszMldOrderPop1[] = _T("%02d %.10s\n"); /* 2172 */
TCHAR FARCONST  aszMldOrderPop2[] = _T("%02d %-4s %.18s\n"); /* 2172 */
TCHAR FARCONST  aszMldOrderPop3[] = _T("%02d %-2s %.7s\n");
TCHAR FARCONST  aszMldOrderPop4[] = _T(" \t%s");

/*
*===========================================================================
** Format  : SHORT MldCreatePopUp(VOID *pData)
*
*   Input  : VOID   *pData      -popup display data
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: Create popup window on the scroll#1 display
============================================================================
*/
static USHORT MldCalculateMaxButtons (USHORT usMldPopupHandle)
{
	UCHAR  ucMaxButtons = 0;
	USHORT usMaxButtons = 0, usWindowSize[3] = { 0, 0, 0}; /*{type, width, height}*/

	usMaxButtons = BlFwIfGetMaxOEPButtons (&ucMaxButtons);

#if 0
	VosGetWndTxtSize(usMldPopupHandle, usWindowSize); // # of Buttons: [0] is Across, [1] is Down

	if (usWindowSize[0]) {
		// We are on a touchscreen system so we figure out how many
		// buttons will fit into the window.  We take window size in
		// number of buttons across and number of buttons down but then
		// we need to ensure there is room for the back, next, and done buttons
		// on the bottom row.
		usMaxButtons = usWindowSize[1] * usWindowSize[2] - 2*usWindowSize[1];

		if (usMaxButtons < 1 || usMaxButtons > 127)
		{
			usMaxButtons = STD_OEP_MAX_NUM;
		}
		VosSetWndHasButtons (usMldPopupHandle, 1);
	}
	else {
		usMaxButtons = STD_OEP_MAX_NUM;
		VosSetWndHasButtons (usMldPopupHandle, 0);
	}
#else
	if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH)
	{
		VosSetWndHasButtons (usMldPopupHandle, 1);
	}
#endif
	return usMaxButtons;
}

SHORT MldCreatePopUpRectWindowOnly(VOSRECT *pRect)
{
    USHORT usWinHandle;
    USHORT usHandle = LCDWIN_ID_REG102;
	USHORT usMaxButtons = 0, usWindowSize[2] = { 0, 0}; /*{width, height}*/

    /* change overlapped scroll display type */    
    /* turn off cursor of current scroll */
    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        usWinHandle = MldScroll1Buff.usWinHandle;
        break;

    case MLD_SCROLL_2:
        usWinHandle = MldScroll2Buff.usWinHandle;
        break;

    case MLD_SCROLL_3:
        usWinHandle = MldScroll3Buff.usWinHandle;
        break;

    default:
        break;
    }

    VosSetCurType(usWinHandle, VOS_INVISIBLE);
    VosSetWinType(usWinHandle, VOS_WIN_NORMAL);

    /* --- create window for popup --- */
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
		// previous to TOUCHSCREEN this handle would be LCDWIN_ID_REG209
		usHandle = LCDWIN_ID_REG102;
        //usMldPopupHandle = MldCreateDrive3Popup(MLD_C_BLACK);
		VosCreateWindowRect(pRect,              /* rectangle for the window to popup */
                    VOS_FONT_NORMAL,        /* font attribute */
                    MLD_C_BLACK,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

		/* set normal status for cursor control */
		VosSetWinType(usHandle, VOS_WIN_NORMAL);
		usMldPopupHandle = usHandle;
    } else {

		usHandle = LCDWIN_ID_REG102;
        //usMldPopupHandle = MldCreatePrechkPopup(MLD_C_BLACK);
		VosCreateWindowRect(pRect,              /* rectangle for the window to popup */
                    VOS_FONT_NORMAL,        /* font attribute */
                    MLD_C_BLACK,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

		/* set normal status for cursor control */
		VosSetWinType(usHandle, VOS_WIN_NORMAL);
		usMldPopupHandle = usHandle;
    }

	VosShowWindow(usMldPopupHandle);

	usMaxButtons = MldCalculateMaxButtons(usMldPopupHandle);

	return usMaxButtons;
}

SHORT MldCreatePopUp(VOID *pData)
{
	VOSRECT VosRect;

#ifndef TOUCHSCREEN
    /* --- create popup window on the scroll #1, order #1 and total #1 --- */
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
		VosRect.usRow = MLD_DRV3_POP_ROW_POS;    /* start row */
		VosRect.usCol = MLD_DRV3_POP_CLM_POS;   /* start colum */
		VosRect.usNumRow = MLD_DRV3_POP_ROW_LEN;   /* row length */
		VosRect.usNumCol = MLD_DRV3_POP_CLM_LEN;   /* colum length */
	}
	else
	{
		VosRect.usRow = MLD_NOR_POP_ROW_POS;    /* start row */
		VosRect.usCol = MLD_NOR_POP_CLM_POS;   /* start colum */
		VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length */
		VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */
	}
#else
	VosRect.usRow = MLD_NOR_POP_ROW_POS;    /* start row */
	VosRect.usCol = MLD_NOR_POP_CLM_POS;   /* start colum */
	VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length */
	VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */
#endif

	return MldCreatePopUpRect (pData, &VosRect);
}

SHORT MldCreatePopUpRect(VOID *pData, VOSRECT *pRect)
{
	USHORT usMaxButtons = 0;

    if (((MLDITEMDATA *)pData)->uchMajorClass != CLASS_POPUP) {
        return(MLD_ERROR);
    }

	usMaxButtons = MldCreatePopUpRectWindowOnly (pRect);

    VosCls(usMldPopupHandle);

    switch (((MLDITEMDATA *)pData)->uchMinorClass) {
    case CLASS_POPORDERPMT:

        MldDispOrderEntryPrompt(pData); /* order entryp prompt */
        break;

    default:
        break;

    }

    return(usMaxButtons);
}


USHORT MldCreatePrechkPopup(UCHAR uchAttr)
{
    /* --- create popup window on the scroll #1 --- */

    USHORT usHandle = LCDWIN_ID_REG102;

    VosCreateWindow(MLD_NOR_POP_ROW_POS,     /* start row */
                    MLD_NOR_POP_CLM_POS,   /* start colum */
                    MLD_NOR_POP_ROW_LEN,   /* row length */
                    MLD_NOR_POP_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateDrive3Popup(UCHAR uchAttr)
{
	// previous to TOUCHSCREEN this handle would be LCDWIN_ID_REG209
    USHORT usHandle = LCDWIN_ID_REG102;
	/* --- create popup window using Normal sizes for Touchscreen --- */
    VosCreateWindow(MLD_NOR_POP_ROW_POS,     /* start row */
                    MLD_NOR_POP_CLM_POS,   /* start colum */
                    MLD_NOR_POP_ROW_LEN,   /* row length */
                    MLD_NOR_POP_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

// Adding support for a more general OEP window functionality that is
// used by Amtrak for displaying list of Preauths that need to be finalized.
// This extends the OEP window functionality from PLU and condiments to
// more general data to present the user with a selection decision.
// Supports MLDPOPORDER_VARY  pDisp->uchMinorClass == CLASS_POPORDER_LABELED
VOID MldDispOrderEntryPromptText( MLDPOPORDERUNION_VARY *pData)
{
	USHORT usMaxButtons = 0;
    USHORT i;
	CONST TCHAR *aszMldOrderTextPop = _T("%04d %-26s\n"); /* 2172 */

	/* Getting window size and button dimensions to determine maximum number of buttons to be shown. - CSMALL */
	usMaxButtons = MldCalculateMaxButtons(usMldPopupHandle);
    /* --- draw plu name and entry number --- */
    for (i=0; i < usMaxButtons; i++) {
        if (pData->u[i].OrderPmtText.usOrder == 0 || pData->u[i].OrderPmtText.aszLabel[0] == 0) {
            break;
        }

		if (pData->u[i].OrderPmtText.usOrder & 0x2000) {
			/* display special message as is */
			VosPrintf(usMldPopupHandle, pData->u[i].OrderPmtText.aszLabel);
		} else {
			VosPrintf(usMldPopupHandle, aszMldOrderTextPop, pData->u[i].OrderPmtText.usOrder, pData->u[i].OrderPmtText.aszLabel);
		}
    }

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_BACK) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_BACK;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_BACK;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_MORE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_MORE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_DONE) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_DONE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_DONE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	PifSleep (15); // RJC give the window time to display.
}

/*
*===========================================================================
** Format  : SHORT MldUpdatePopUp(VOID *pData)
*
*   Input  : VOID   *pData      -popup display data
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: Update popup window on the scroll#1 display
============================================================================
*/
SHORT MldUpdatePopUp(VOID *pData)
{

    if (((MLDITEMDATA *)pData)->uchMajorClass != CLASS_POPUP) {

        return(MLD_ERROR);
    }

    if (!usMldPopupHandle) return (MLD_ERROR);

    VosCls(usMldPopupHandle);

    switch (((MLDITEMDATA *)pData)->uchMinorClass) {
    case CLASS_POPORDERPMT:
    case CLASS_POPORDERPMT_UP:      /* V3.3 */
    case CLASS_POPORDERPMT_DOWN:    /* V3.3 */

        MldDispOrderEntryPrompt(pData); /* order entryp prompt */
        break;

	case CLASS_POPORDER_LABELED:
		MldDispOrderEntryPromptText(pData);
		break;

    default:
        break;

    }

    return(MLD_SUCCESS);
}

VOID MldDispOrderEntryPrompt( MLDPOPORDER *pData)
{
	USHORT usMaxButtons = 0;
#ifndef TOUCHSCREEN
    ORDERPMT OrderPmt;
#endif	//not TOUCHSCREEN
    USHORT i;

	/* Getting window size and button dimensions to determine maximum number of buttons to be shown. - CSMALL */
	usMaxButtons = MldCalculateMaxButtons(usMldPopupHandle);
    /* --- draw plu name and entry number --- */

    //RJC VosCls(usMldPopupHandle);

    for (i=0; i < usMaxButtons; i++) {

        if (pData->OrderPmt[i].uchOrder == 0) {
            break;
        }

        /* display with adjective mnemonic */
        if (pData->OrderPmt[i].aszAdjective[0]) {
            /* display adjective 4 char. and plu 12 char */
            VosPrintf(usMldPopupHandle, aszMldOrderPop2, pData->OrderPmt[i].uchOrder, pData->OrderPmt[i].aszAdjective, pData->OrderPmt[i].aszPluName);
        } else {
            /* display plu 12 char. */
            VosPrintf(usMldPopupHandle, aszMldOrderPop, pData->OrderPmt[i].uchOrder, pData->OrderPmt[i].aszPluName);
        }
    }

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_BACK) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_BACK;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_BACK;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_MORE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_MORE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	if (pData->ulDisplayFlags & MLD_DISPLAYFLAGS_DONE) {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_DTXT_DONE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	} else {
		TCHAR *tcsDisplayFlag = MLD_DISPLAYFLAG_RTXT_DONE;
        VosPrintf(usMldPopupHandle, tcsDisplayFlag);
	}

	PifSleep (15); // RJC give the window time to display.
}

/*
*===========================================================================
** Format  : SHORT MldDeletePopUp(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: delete popup window and recover scroll#1 display
*===========================================================================
*/
SHORT MldForceDeletePopUp(VOID)
{
    if (!usMldPopupHandle) return (MLD_ERROR);

    VosDeleteWindow(usMldPopupHandle);
	usMldPopupHandle = 0;

    return(MLD_SUCCESS);
}

SHORT MldDeletePopUp(VOID)
{
    USHORT usHandle;

    if (!usMldPopupHandle) return (MLD_ERROR);

    VosDeleteWindow(usMldPopupHandle);

    /* recover overlapped window type */
    /* change overlapped scroll display type */    
    /* turn off cursor of current scroll */
    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        usHandle = MldScroll1Buff.usWinHandle;
        break;

    case MLD_SCROLL_2:
        usHandle = MldScroll2Buff.usWinHandle;
        break;

    case MLD_SCROLL_3:
        usHandle = MldScroll3Buff.usWinHandle;
        break;

    default:
        break;
    }

    VosSetWinType(usHandle, VOS_WIN_OVERLAP);
    VosSetCurType(usHandle, VOS_VISIBLE);

	usMldPopupHandle = 0;

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldPopupDownCursor(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll down order entry prompt display
============================================================================
*/
SHORT MldPopUpDownCursor(VOID)
{
    return(MLD_CURSOR_BOTTOM);  /* V3.3 */
    /* return(MLD_SUCCESS); */
}

/*
*===========================================================================
** Format  : SHORT MldPopupUpCursor(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll down order entry prompt display
============================================================================
*/
SHORT MldPopUpUpCursor(VOID)
{
    return(MLD_CURSOR_TOP); /* V3.3 */
    /* return(MLD_SUCCESS); */
}

/*
*===========================================================================
** Format  : SHORT MldPopUpRedisplay(USHORT usType)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: redisplay order entry prompt display
============================================================================
*/
SHORT MldPopUpRedisplay(USHORT usType)
{
    return(MLD_SUCCESS);

    usType = usType;
}

/*
*===========================================================================
** Format  : SHORT MldPopUpGetCursorDisplay(VOID *pData)
*
*   Input  : none
*   Output : none
*   InOut  : VOID *pData
** Return  : MLD_SUCCESS
*
** Synopsis: Get current displayed data structer of OEP
============================================================================
*/
SHORT MldPopUpGetCursorDisplay(VOID *pData)
{
    return(MLD_ERROR);  /* called at lcd terminal */

    pData = pData;
}

/**** End of File ****/
