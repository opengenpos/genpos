/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Echo Back Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieEcho.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieEchoBack()       - set type of echo back
*                   UieEchoBackGround() - set string of echo back ground
*                   UieCtrlBackGround() - disable echo back ground
*                   UieEchoBackWide()   - update status of wide character
*                   UiePresetCash()     - update preset cash amount
*                   UieKeepMessage()    - set keep message flag
*                   UieSetMessage()     - update return type
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-02-92  00.01.00  O.NAkada     Changed to caller position of
*                                   UieDisplayEchoBackWide() in
*                                   UieDisplayEchoBack().
* Sep-03-92  00.01.00  O.Nakada     The caller position of
*                                   UieDisplayEchoBackWide() moved to
*                                   UieEchoBackWide() from UieInitEchoBack().
* Sep-07-92  00.01.01  O.Nakada     Moved Double Wide Flag.
* Sep-26-92  00.01.05  O.Nakada     Modify UieEchoBackPrintf().
* Nov-09-92  00.01.11  O.Nakada     Added reseting of double wide flag in
*           (00.00.02)              UieInitEchoBack().
* Dec-10-92  01.00.03  O.Nakada     Modify echo back function in
*                                   UieInitEchoBack().
* Jan-07-93  01.00.07  O.Nakada     Modify display visble judgement in
*                                   UieEchoBackWideWide()
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*     note: (XX.XX.XX) for T-POS Version.
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "vos.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uiedisp.h"
#include "uieecho.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
UIEECHO UieEcho;                            /* echo back status          */
UCHAR   uchUieEcho;                         /* echo back type of current */

static  UCHAR       uchUieRow;              /* echo back for start row   */
static  DCURRENCY   lUieAmount;             /* preset cash of current    */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieEchoBack(UCHAR uchType, UCHAR uchMax);
*     Input:    uchType - type of echo back
*               uchMax  - max digits
*
** Return:      nothing
*===========================================================================
*/
VOID  UieEchoBack(UCHAR uchType, UCHAR uchMax)
{
    UieEcho.uchType    = uchType;
    UieEcho.uchMax     = uchMax;
    UieEcho.fchStatus &= ~UIE_DOUBLE_WIDE;  /* reset double wide flag */
}


/*
*===========================================================================
** Synopsis:    VOID UieEchoBackGround(ULONG flDescriptor,
*                                               UCHAR uchAttr,
*                                               CONST UCHAR *pszFmt,
*                                               ...);
*     Input:    flDescriptor - descriptor attribute
*               uchAttr      - display attribute
*               pszFmt       - display formats
*               ...          - arguments
*
** Return:      nothing
*===========================================================================
*/
VOID  UieEchoBackGround(ULONG flDescriptor, UCHAR uchAttr, CONST TCHAR *pszFmt, ...)
{
    UieEcho.flDescriptor = flDescriptor;

    PifRequestSem(usUieSyncDisplay);
    UieChooseDisplay(UIE_OPER, 0, 0, uchAttr, pszFmt, (USHORT *)(&pszFmt + 1), UieEcho.auchBack, NULL);
    PifReleaseSem(usUieSyncDisplay);

    UieEcho.fchStatus |= UIE_CTRL_BACK_GROUND;  /* enable echo back ground */
}


/*
*===========================================================================
** Synopsis:    VOID UieCtrlBackGround(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieCtrlBackGround(VOID)
{
    UieEcho.fchStatus &= ~UIE_CTRL_BACK_GROUND; /* disable echo back ground */
}


/*
*===========================================================================
** Synopsis:    VOID UieEchoBackWide(USHORT usRow,
*                                             USHORT usColumn);
*     Input:    usRow    - position of row
*               usColumn - position of column
*
** Return:      nothing
*===========================================================================
*/
VOID  UieEchoBackWide(USHORT usRow, USHORT usColumn)
{
    UieEcho.WIDE.usRow    = usRow;
    UieEcho.WIDE.usColumn = usColumn;

    /* --- display character of double wide --- */
    UieDisplayEchoBackWide(&UieEcho.uchType);
}


/*
*===========================================================================
** Synopsis:    VOID UiePresetCash(DCURRENCY lAmount);
*     Input:    lAmount - preset cash amount
*
** Return:      nothing
*===========================================================================
*/
VOID  UiePresetCash(DCURRENCY lAmount)
{
    UieEcho.lAmount = lAmount;
}


/*
*===========================================================================
** Synopsis:    VOID UieKeepMessage(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieKeepMessage(VOID)
{
    UieEcho.fchStatus |= UIE_KEEP_MSG;      /* set keep message flag */
}


/*
*===========================================================================
** Synopsis:    USHORT UieSetMessage(USHORT usFigure);
*     Input:    usFigure - number figure
*
** Return:      previous figure
*===========================================================================
*/
USHORT  UieSetMessage(USHORT usFigure)
{
    USHORT  usPrevious = UieEcho.usFigure;
    UieEcho.usFigure = usFigure;
    return (usPrevious);
}


/*
*===========================================================================
** Synopsis:    VOID UieResetKeepMessage(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieResetKeepMessage(VOID)
{
    UieEcho.fchStatus &= ~UIE_KEEP_MSG;     /* reset keep message flag */
}


/*
*===========================================================================
** Synopsis:    UCHAR UieGetEchoStatus(VOID);
*
** Return:      echo back status
*===========================================================================
*/
UCHAR  UieGetEchoStatus(VOID)
{
    return (UieEcho.fchStatus);
}


/*
*===========================================================================
** Synopsis:    VOID UieRedisplay(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieRedisplay(VOID)
{
    PifRequestSem(usUieSyncDisplay);
    UieBuf2SwitchDispRecovery(UIE_OPER, auchUiePrevOD, NULL);
    PifReleaseSem(usUieSyncDisplay);

    /* --- display character of double wide --- */
    UieDisplayEchoBackWide(&UieEcho.uchType);
}


/*
*===========================================================================
** Synopsis:    VOID UieEchoBackInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieEchoBackInit(VOID)
{
    UieEcho.WIDE.usRow = 255;               /* disable echo back wide */
    UieEcho.uchType    = 1;                 /* clear echo back status */
}


/*
*===========================================================================
** Synopsis:    VOID UiePreEchoBack(KEYMSG *pMsg);
*     Input:    pMsg - address of message buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UiePreEchoBack(KEYMSG *pMsg)
{
    UieEcho.fchStatus |= UIE_INIT_ECHO_BACK;    /* background display     */
    UieEcho.fchStatus &= ~UIE_DOUBLE_WIDE;      /* reset double wide flag */

    UieDisplayEchoBackWide(&UieEcho.uchType);   /* display wide character */

    uchUieEcho = UieEcho.uchType;               /* set type of echo back  */
    if (uchUieEcho % 2) {
        uchUieRow = 1;                          /* display for 2nd Line   */
    } else {
        uchUieRow = 0;                          /* display for 1st Line   */
    }

    if (uchUieEcho == UIE_ECHO_ROW0_PRESET ||   /* set preset cash        */
        uchUieEcho == UIE_ECHO_ROW1_PRESET) {               
        uchUieEcho = (UCHAR)(2 + uchUieEcho % 2);
        lUieAmount = UieEcho.lAmount;
    } else {
        lUieAmount = 0L;
    }

    /* --- initialize user buffer --- */
    memset(pMsg, 0, sizeof(KEYMSG));
    pMsg->SEL.INPUT.uchDec = UIE_DEC_INIT;
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayEchoBack(KEYMSG *pMsg);
*     Input:    pMsg - address of message buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplayEchoBack(KEYMSG *pMsg)
{
    static  TCHAR   CONST UieEchoFmt1[] = _T("\t %l$");  /* currency      */
    static  TCHAR   CONST UieEchoFmt2[] = _T("\t %.*l$");    /* weight    */
    static  TCHAR   CONST UieEchoFmt3[] = _T("\t %s");   /* code/acount   */
    static  TCHAR   CONST UieEchoFmt4[] = _T("%-*s");    /* alpha/numeric */
    static  TCHAR   CONST UieEchoFmt5[] = _T("\t%s");    /* alpha/numeric */
	static  TCHAR   CONST UieEchoFmt6[] = _T("\t%s");    /* date:MM */
	static  TCHAR   CONST UieEchoFmt7[] = _T("\t%0.2s-%0.2s");    /* date: MM-DD */
	static  TCHAR   CONST UieEchoFmt8[] = _T("\t%0.2s-%0.2s-%0.4s");    /* date: MM-DD-YYYY */
    struct  _INPUT  *pKey = &pMsg->SEL.INPUT;
	int     i = 0;
	TCHAR	UieEchoNone[50] = {0};
	TCHAR	tchDateHolder[3] = {0}, tchMonthHolder[3] = {0}, tchYearHolder[5] = {0};
    USHORT  usWidth;

    if (UieEcho.fchStatus & UIE_INIT_ECHO_BACK) {   /* first echo back */
        UieEcho.fchStatus &= ~UIE_INIT_ECHO_BACK;   /* reset flag      */

        /* --- check display of echo back ground --- */
        if (UieEcho.fchStatus & UIE_CTRL_BACK_GROUND) { /* enable      */
            PifRequestSem(usUieSyncDisplay);
            UieBuf2SwitchDispRecovery(UIE_OPER, UieEcho.auchBack, NULL);
            UieDisplayEchoBackDesc(UieEcho.flDescriptor);
            PifReleaseSem(usUieSyncDisplay);
        }
    }

    UieDisplayEchoBackWide(&UieEcho.uchType);

    switch (uchUieEcho) {
    case UIE_ECHO_NO:                       /* no echo back            */
		for(i = 0; i < pKey->uchLen && i < TCHARSIZEOF(UieEchoNone); i++){
			UieEchoNone[i] = _T('*');
		}

        UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt3, UieEchoNone);
        return;
        
    case UIE_ECHO_ROW0_REG:                 /* currency mode           */
    case UIE_ECHO_ROW1_REG:
        UieEchoBackPrintf(1, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt1, pKey->lData + lUieAmount);
        return;

    case UIE_ECHO_WEIGHT:                   /* weight mode             */
        UieEchoBackPrintf(1, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt2, pKey->uchLen - pKey->uchDec - 1, pKey->lData + lUieAmount);
        return;

    case UIE_ECHO_ROW0_AN:                  /* alpha/numeric entry     */
    case UIE_ECHO_ROW1_AN:
        usWidth = UieGetOperWidth();

        if ((USHORT)pKey->uchLen < usWidth) {
            UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt4, usWidth, pKey->aszKey);
        } else {
            UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt5, pKey->aszKey);
        }
        return;

    case UIE_ECHO_ROW0_NUMBER:              /* number entry            */ 
    case UIE_ECHO_ROW1_NUMBER:
    case UIE_ECHO_CA:                       /* code, acount mode       */
        UieEchoBackPrintf(1, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt3, pKey->aszKey);
        return;

	case UIE_ECHO_DATE_ENTRY:
		if(pKey->uchLen < 3)//MM
		{
            UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt6, pKey->aszKey);
		}else if((pKey->uchLen >= 3) && (pKey->uchLen < 5))//MM-DD
		{
			_tcsncpy(tchMonthHolder,pKey->aszKey, sizeof(TCHAR));
			_tcsncpy(tchDateHolder,&pKey->aszKey[2], sizeof(TCHAR));
            UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt7, tchMonthHolder, tchDateHolder);
		}else								//MM-DD-YYYY
		{
			_tcsncpy(tchMonthHolder,pKey->aszKey, sizeof(TCHAR));
			_tcsncpy(tchDateHolder,&pKey->aszKey[2], sizeof(TCHAR));
			_tcsncpy(tchYearHolder,&pKey->aszKey[4], (sizeof(TCHAR)*2));
            UieEchoBackPrintf(0, UIE_OPER, uchUieRow, 0, UIE_ATTR_NORMAL, UieEchoFmt8, tchMonthHolder, tchDateHolder, tchYearHolder);
		}
    default:                                /* not use                 */
        break;
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayEchoBackDesc(ULONG flDescriptor);
*     Input:    flDescriptor - Descriptor Buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplayEchoBackDesc(ULONG flDescriptor)
{
    UCHAR   uchAttr;
    USHORT  usNo, usWidth;

    usWidth = UieGetOperWidth();
    for (usNo = 0; usNo < usWidth; usNo++) {
        if ((uchAttr = (UCHAR)(flDescriptor & UIE_DESC_MASK)) != UIE_WITHOUT) {
            UieLightDescr(UIE_OPER, usNo, (UCHAR)(uchAttr - 1));
        }
        flDescriptor >>= UIE_DESC_SHIFT;
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayEchoBackWide(CONST CHAR *pchType);
*     Input:    pchType - echo back type
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplayEchoBackWide(CONST CHAR *pchType)
{
    TCHAR   auchSingle[] = {_T('S'), DISP_ATTR_NORMAL};
    TCHAR   auchWide[]   = {_T('W'), DISP_ATTR_NORMAL};
    TCHAR   *puchString;
    USHORT  usWidth;

    /* --- check display of echo back wide --- */
    if (UieEcho.WIDE.usRow == 255) {        /* disable                */
        return;                             /* exit ...               */
    }

    /* --- check echo back type, not alpha/numeric mode  --- */
    if (*pchType != UIE_ECHO_ROW0_AN && *pchType != UIE_ECHO_ROW1_AN) {
        return;                             /* exit ...               */
    }

    PifRequestSem(usUieSyncDisplay);

    /* --- set string address of single/wide character --- */
    if (UieEcho.fchStatus & UIE_DOUBLE_WIDE) {
        puchString = auchWide;
    } else {
        puchString = auchSingle;
    }

	/* display enable  active for normal   */
    if ((fchUieActive & UIE_DISPLAY) && (fchUieActive & UIE_ACTIVE_GET)) {
        if (PifSysConfig()->uchOperType == DISP_LCD) {  /* LCD            */
            VosDisplayAttr(DISP_OPER, UieEcho.WIDE.usRow, UieEcho.WIDE.usColumn, puchString, 1);
        } else {                            /* 2X20 or 10N10D         */
            PifDisplayAttr(DISP_OPER, UieEcho.WIDE.usRow, UieEcho.WIDE.usColumn, puchString, 1);
        }
    }

    /* --- update recovery buffer --- */
    usWidth = UieGetOperWidth();

    _tcsncpy(UieRecovery.auchOperDisp + (UieEcho.WIDE.usRow * usWidth + UieEcho.WIDE.usColumn) * 2, puchString, 2);

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    VOID UieEchoBackMacro(USHORT usLead,
*                                     CONST UCHAR *puchMnemo,
*                                     USHORT usLength);
*     Input:    usLead   - lead through number
*               pszMnemo - address of mnemonics
*               usLength - length of mnemonics
*
** Return:      nothing
*===========================================================================
*/
VOID UieEchoBackMacro(USHORT usLead, CONST UCHAR *puchAddr, USHORT usSize)
{
	TCHAR        *puchMnemo, auchMnemo[UIE_LCD_COLUMN * 2] = {0};
    CONST  TCHAR *puchMsg;
    USHORT       usLength, usLoop, usRow, usWidth;

    /* --- make lead through message in near memory --- */
    usLength  = (usSize < UIE_LCD_COLUMN) ? (usSize) : (UIE_LCD_COLUMN);
    puchMnemo = auchMnemo;
    puchMsg   = (TCHAR*)puchAddr + usLead * usSize;

    for (usLoop = 0; usLoop < usLength; usLoop++) {
        *puchMnemo++ = *puchMsg++;
        *puchMnemo++ = DISP_ATTR_NORMAL;
    }

    PifRequestSem(usUieSyncDisplay);

    usRow = (uchUieRow == 0) ? (1) : (0);
	/* display enable active for normal   */
    if ((fchUieActive & UIE_DISPLAY) && (fchUieActive & UIE_ACTIVE_GET)) {
        if (PifSysConfig()->uchOperType == DISP_LCD) {  /* LCD            */
            VosDisplayAttr(DISP_OPER, usRow, 0, auchMnemo, usLength);
        } else {                            /* 2X20 or 10N10D         */
            PifDisplayAttr(DISP_OPER, usRow, 0, auchMnemo, usLength);
        }
    }

    /* --- update recovery buffer --- */
    usWidth = UieGetOperWidth();
    _tcsncpy(UieRecovery.auchOperDisp + usRow * usWidth * 2, auchMnemo, usLength * 2);

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    SHORT UieEchoBackPrintf(USHORT usRemain, USHORT usDisplay,
*                                       USHORT usRow, USHORT usColumn,
*                                       UCHAR uchAttr,
*                                       CONST UCHAR *pszFmt, ...);
*     Input:    usRemain  - 
*               usDisplay - Display Type
*               usRow     -
*               usColumn  - 
*               uchAttr   - Display Attribute
*               pszFmt    - Format
*               ...       - Argument
*
** Return:      nothing
*===========================================================================
*/
SHORT UieEchoBackPrintf(USHORT usRemain, USHORT usDisplay, USHORT usRow,
                        USHORT usColumn, UCHAR uchAttr,
                        CONST TCHAR *pszFmt, ...)
{
	static  TCHAR auchDispBuf[UIE_LCD_COLUMN * 2] = {0};
    SHORT   sRet;
    USHORT  usLen, usWidth;
    
    PifRequestSem(usUieSyncDisplay);

    sRet = UieChooseDisplay(usDisplay, 0, usColumn, uchAttr, pszFmt, (SHORT *)(&pszFmt + 1), auchDispBuf, NULL);

    usWidth = UieGetOperWidth();
    usLen   = UieFillSuppress(auchDispBuf, _T(' '), usRemain, auchDispBuf + usWidth * 2);

    if (usRemain || usLen == 0) {
        _tcsncpy(UieRecovery.auchOperDisp + (usRow * usWidth + usLen) * 2, auchDispBuf + usLen * 2, (usWidth - usLen) * 2);
    } else {                                /* A/N Entry and Space Character */
        _tcsncpy(UieRecovery.auchOperDisp + (usRow * usWidth) * 2, auchDispBuf, usWidth * 2);
    }

    if ((fchUieActive & UIE_DISPLAY) && (fchUieActive & UIE_ACTIVE_GET)) {    /* display enable active for normal */
        UieBuf2Disp(usDisplay, UieRecovery.auchOperDisp, NULL);
    }

    PifReleaseSem(usUieSyncDisplay);

    return (sRet);
}


/*
*===========================================================================
** Synopsis:    USHORT UieFillSuppress(UCHAR *puchStr, UCHAR uchFill,
*                                      USHORT usRemain, UCHAR *puchMax);
*     Input:    pszStr   - Display String 
*               uchFill  - Fill Character
*               usRemain - Number of Remian 
*
** Return:      Length of Suppress
*===========================================================================
*/
USHORT UieFillSuppress(TCHAR *puchStr, TCHAR uchFill, USHORT usRemain, TCHAR *puchMax)
{
    TCHAR *puchAddr = puchStr;

    while (*puchAddr == uchFill && puchAddr < puchMax) {
        ++puchAddr;
        ++puchAddr;                         /* Skip to Attribute Character */
    }

    while (puchAddr != puchStr && usRemain) {
        --puchAddr;
        --puchAddr;                         /* Skip to Attribute Character */
        --usRemain;
    }

    return ((puchAddr - puchStr) / 2);
}


/*
*===========================================================================
** Synopsis:    VOID UieRefreshEchoBack(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieRefreshEchoBack(VOID)
{
    USHORT  usWidth;

    PifRequestSem(usUieSyncDisplay);

    usWidth = UieGetOperWidth() * 2;                /* with attribute      */
    if (UieEcho.fchStatus & UIE_CTRL_BACK_GROUND) { /* enable back ground  */
        _tcsncpy(UieRecovery.auchOperDisp + uchUieRow * usWidth, UieEcho.auchBack + uchUieRow * usWidth, usWidth);
    } else {                                        /* disable back ground */
        _tcsncpy(UieRecovery.auchOperDisp + uchUieRow * usWidth, auchUiePrevOD + uchUieRow * usWidth, usWidth);
    }

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    USHORT UieGetOperWidth(VOID);
*
** Return:      nothing
*===========================================================================
*/
USHORT UieGetOperWidth(VOID)
{
    switch (PifSysConfig()->uchOperType) {
    case DISP_2X20:
        return (UIE_2X20_COLUMN);

    case DISP_LCD:
        //return (UIE_2X20_COLUMN);
         return (UIE_LCD_COLUMN); /* for Dollar Tree, RFC, 01/23/01 */

    case DISP_10N10D:
        return (UIE_10N10D_COLUMN);
    }
    return 0; /* dummy *//* ### Mod (2171 for Win32) V1.0 */
}

/* ====================================== */
/* ========== End of UieEcho.C ========== */
/* ====================================== */
