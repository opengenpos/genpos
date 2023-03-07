/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display Module
* Category    : User Interface Engine, 2170 System Application
* Program Name: UieDisp.C
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UiePrintf()        - display string
*                   UieAttrPrintf()    - display string with attribute
*                   UieDescriptor()    - light descriptor
*                   UieSetDescriptor() - set descriptor mnemonics
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
* Jul-10-95  G1        O.Nakada     Added synchronous semaphore in
*                                   UieSetDescriptor() function.
* Dec-11-95  G4        O.Nakada     Corrected display type selection.
*                                   Corrected 2x20 customer display function.
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
#include <windows.h>
#ifndef _WIN32_WCE
#include <tchar.h>
#endif
#include "Scf.h"
#include "DeviceIODisplay.h"
#include "UIELDEV.h"

#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "rfl.h"
#include "vos.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uiedisp.h"
#include "BlFWif.h"

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
PifSemHandle  usUieSyncDisplay = PIF_SEM_INVALID_HANDLE;               /* synchronous semaphore    */
USHORT      usUieVosDisp = 0;                   /* handle of VOS display    */
USHORT      usUieVosDesc = 0;                   /* handle of VOS descriptor */
USHORT      usUieDispCol = 0;                   /* number of column         */
TCHAR       auchUiePrevOD[UIE_MAX_DISP_BUF];/* redisplay buffer         */
UIERECOVERY UieRecovery;                    /* recovery buffer          */
UIELCD      UieLcd;                         /* LCD status               */

/* for DLL interface */

LOAD_FUNCTION   funcODisp;
HINSTANCE       hODispDll = 0;
HANDLE          hODisp = NULL;

LOAD_FUNCTION   funcCDisp;
HINSTANCE       hCDispDll = 0;
HANDLE          hCDisp = NULL;

/* for */

VOID    UieDisplayInitEx(VOID);


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UiePrintf(USHORT usDisplay, USHORT usRow,
*                                        USHORT usColumn,
*                                        CONST UCHAR FAR *pszFmt, ...);
*     Input:    usDisplay - type of display
*               usRow     - start row
*               usColumn  - start column
*               pszFmt    - format string
*               ...       - arguments
*
** Return:      nothing
*===========================================================================
*/
SHORT DLLENTRY UiePrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn,
                         CONST TCHAR FAR *pszFmt, ...)
{
    SHORT  sRet;
    
    PifRequestSem(usUieSyncDisplay);

    sRet = UieChooseDisplay(usDisplay,
                            usRow,
                            usColumn,
                            UIE_ATTR_NORMAL,
                            pszFmt,
                            (USHORT *)(&pszFmt + 1),
                            auchUiePrevOD,
                            UieRecovery.auchCustDisp);

    UieBuf2SwitchDispRecovery(usDisplay,
                              auchUiePrevOD,
                              UieRecovery.auchCustDisp);

    PifReleaseSem(usUieSyncDisplay);

    return (sRet);
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieAttrPrintf(USHORT usDisplay, USHORT usRow,
*                                            USHORT usColumn, UCHAR uchAttr,
*                                            CONST UCHAR FAR *pszFmt, ...);
*     Input:    usDisplay - type of display
*               usRow     - start row
*               usColumn  - start column
*               uchAttr   - character attribute
*               pszFmt    - format string
*               ...       - arguments
*
** Return:      nothing
*===========================================================================
*/
SHORT DLLENTRY UieAttrPrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn,
                             UCHAR uchAttr, CONST TCHAR FAR *pszFmt, ...)
{
    SHORT  sRet;
    
    PifRequestSem(usUieSyncDisplay);

    sRet = UieChooseDisplay(usDisplay,
                            usRow,
                            usColumn,
                            uchAttr,
                            pszFmt,
                            (USHORT *)(&pszFmt + 1),
                            auchUiePrevOD,
                            UieRecovery.auchCustDisp);

    UieBuf2SwitchDispRecovery(usDisplay,
                              auchUiePrevOD,
                              UieRecovery.auchCustDisp);

    PifReleaseSem(usUieSyncDisplay);

    return (sRet);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieDescriptor(USHORT usDisplay, USHORT usNo,
*                                           USHORT usAttr);
*     Input:    usDisplay - type of display
*               usNo      - descriptor number
*               uchAttr   - attribute
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieDescriptor(USHORT usDisplay, USHORT usNo, UCHAR uchAttr)
{
    PifRequestSem(usUieSyncDisplay);

    UieLightDescr(usDisplay, usNo, uchAttr);

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetDescriptor(CONST UIEDESCLCD FAR *pInfo,
*                                              UCHAR FAR *puchBuf);
*     Input:    pInfo   - address of descriptor infomation
*               puchBuf - address of descriptor buffer
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieSetDescriptor(CONST UIEDESCLCD *pInfo,
                               CONST TCHAR *puchBuf, USHORT usCount)
{
    PifRequestSem(usUieSyncDisplay);

    UieLcd.pInfo   = pInfo;
    UieLcd.puchBuf = puchBuf;
	UieLcd.usCount = usCount;

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetStatusDisplay(USHORT usControl);
*     Input:    usControl - display status
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieSetStatusDisplay(USHORT usControl)
{
    if (usControl == UIE_ENABLE) {          /* enable Display  */
        PifRequestSem(usUieSyncDisplay);
        fchUieActive |= UIE_DISPLAY;
        PifReleaseSem(usUieSyncDisplay);
    } else if (usControl == UIE_DISABLE) {  /* disable Display */
        fchUieActive &= ~UIE_DISPLAY;
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplayInit(VOID)
{
    usUieSyncDisplay = PifCreateSem(1);     /* exclusive semaphore   */

    UieDisplayInitEx();

    /* --- check type of display device --- */

    if (pUieSysCfg->uchOperType != DISP_LCD) {  /* not LCD           */
        usUieDispCol = UIE_2X20_COLUMN;         /* number of column  */
        return;                                 /* exit ...          */
    }

    //usUieDispCol = UIE_2X20_COLUMN;         /* number of column, Dollar Tree, RFC, 01/23/01 */
     usUieDispCol = UIE_LCD_COLUMN;              // number of column

    /* --- create a 2x20 simulation window --- */

    usUieVosDisp = LCDWIN_ID_COMMON001;         /* window ID         */
    VosCreateWindow(21,                         /* start row         */
                    0,                          /* start column      */
                    UIE_LCD_ROW,                /* number of row     */
                    UIE_LCD_COLUMN,             /* number of column  */
                    VOS_FONT_NORMAL,            /* font number       */
                    VOS_C_BLACK | VOS_C_WHITE,  /* character attr.   */
                    VOS_B_NONE,                 /* border attribute  */
                    &usUieVosDisp);             /* address of handle */

    /* --- create a descriptor window --- */

    usUieVosDesc = LCDWIN_ID_COMMON002;         /* window ID         */
    VosCreateWindow(23,                         /* start row         */
                    0,                          /* start column      */
                    2,                          /* number of row     */
                    UIE_LCD_COLUMN2,            /* number of column  */
                    VOS_FONT_NORMAL,            /* font number       */
                    VOS_C_BLACK | VOS_C_WHITE,  /* character attr.   */
                    VOS_B_NONE,                 /* border attribute  */
                    &usUieVosDesc);             /* address of handle */
    VosSetWinType(usUieVosDesc, VOS_WIN_NORMAL);
}


/*
*===========================================================================
** Synopsis:    SHORT UieChooseDisplay(USHORT usDisplay, USHORT usRow,
*                                      USHORT usColumn, UCHAR uchAttr,
*                                      const UCHAR FAR *pszFmt,
*                                      SHORT *psArgs, UCHAR *puchOperBuf,
*                                      UCHAR *puchCustBuf)
*     Input:    usDisplay   - type of display
*               usRow       - start row
*               usColumn    - start column
*               uchAttr     - character attribute
*               pszFmt      - format string
*               psArgs      - arguments
*     InOut:    puchOperBuf - address of operator display buffer
*               puchCustBuf - address of customer display buffer
*
** Return:      length of format string
*===========================================================================
*/
SHORT UieChooseDisplay(USHORT usDisplay, USHORT usRow, USHORT usColumn,
                       UCHAR uchAttr, const TCHAR FAR *pszFmt,
                       SHORT *psArgs, TCHAR *puchOperBuf, TCHAR *puchCustBuf)
{
    USHORT  usLen;
    TCHAR   szEditBuf[UIE_MAX_DISP_BUF + 1];

    usLen = _RflFormStr(pszFmt,
                        psArgs,
                        szEditBuf,
                        UIE_MAX_DISP_BUF + 1);//(USHORT)sizeof(szEditBuf));

    if (usDisplay == UIE_OPER) {                    /* operator display */
        if (pUieSysCfg->uchOperType == DISP_2X20) {         /* 2x20     */
            UieDisplay2x20(usRow,
                           usColumn,
                           uchAttr,
                           szEditBuf,
                           puchOperBuf,
                           UIE_2X20_COLUMN);
        } else if (pUieSysCfg->uchOperType == DISP_LCD) {   /* LCD      */
            UieDisplay2x20(usRow,
                           usColumn,
                           (UCHAR)(uchAttr | VOS_C_WHITE),
                           szEditBuf,
                           puchOperBuf,
                           usUieDispCol);
        } else if (pUieSysCfg->uchOperType == DISP_10N10D) {/* 10N10D   */
            UieDisplay10N10D(usColumn,
                             uchAttr,
                             szEditBuf,
                             puchOperBuf);
        } else {                                        /* not provide  */
            return (0);
        }
    } else {                                        /* customer display */

        if (pUieSysCfg->uchCustType == DISP_2X20) {         /* 2x20     */
            UieDisplay2x20(usRow,
                           usColumn,
                           uchAttr,
                           szEditBuf,
                           puchCustBuf,
                           UIE_2X20_COLUMN);
        } else if (pUieSysCfg->uchCustType == DISP_10N10D) {/* 10N10D   */
            UieDisplay10N10D(usColumn,
                             uchAttr,
                             szEditBuf,
                             puchCustBuf);
        } else if (pUieSysCfg->uchCustType == DISP_4X20) {  /* 4x20, saratoga     */
            UieDisplay2x20(usRow,
                           usColumn,
                           uchAttr,
                           szEditBuf,
                           puchCustBuf,
                           UIE_2X20_COLUMN);
        } else {                                        /* not provide  */
            return (0);
        }

    }

    return (usLen);
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplay2x20(USHORT usRow, USHORT usColumn,
*                                   UCHAR uchAttr, UCHAR *pszStr,
*                                   UCHAR puchDispBuf, USHORT usWidth);
*     Input:    usRow       - start row
*               usColumn    - start column
*               uchAttr     - character attribute
*               usWidth     - width of display column
*     InOut:    pszStr      - Keep Buffer of Normal Display
*               puchDispBuf - address of display buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplay2x20(USHORT usRow, USHORT usColumn, UCHAR uchAttr,
                    TCHAR *pszStr, TCHAR *puchDispBuf, USHORT usWidth)
{
    USHORT  usLoop, usStart = 0;
    TCHAR   szBuf[UIE_LCD_COLUMN + 1];

    for (usLoop = usRow; usLoop < 2; usLoop++) {
        USHORT  usEnd = 0;
        TCHAR   *puchAddr, *puchDest;

        /* --- get address of new line character --- */

        puchAddr = (TCHAR *)_tcschr(pszStr + usStart, _T('\n'));

        if (puchAddr != NULL) {             /* found       */
            usEnd = puchAddr - (pszStr + usStart);
            *(pszStr + usEnd) = _T('\0');       /* set end of string    */
        } else {                            /* not found   */
            usLoop = 1;                     /* end of loop */
        }

        if (_tcschr(pszStr + usStart, _T('\t'))) {
            RflStrAdjust(szBuf, UIE_LCD_COLUMN + 1, pszStr + usStart, (usWidth - usColumn), RFL_FEED_OFF);
            puchAddr = szBuf;
        } else {
            USHORT usLen = tcharlen((puchAddr = pszStr + usStart));
            if (usLen > usWidth - usColumn) {
                *(pszStr + usStart + usWidth - usColumn) = '\0';
            }
        }

        puchDest = puchDispBuf + ((usRow * usWidth + usColumn) * 2);
        while (*puchAddr) {
            /* *puchDest++ = *puchAddr++; */
            if (*puchAddr == 0x12) {
                *puchDest = 0x20;    /* convert control code to space */
            }
            else {
                *puchDest = *puchAddr;
            }
            puchDest++;
            puchAddr++;
            *puchDest++ = uchAttr;
        }

        usStart = usEnd + 1;
        ++usRow;
        usColumn = 0;
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplay10N10D(USHORT usColumn, UCHAR uchAttr,
*                                     UCHAR *puchDispBuf);
*     Input:    usColumn    - start column
*               uchAttr     - character attribute
*     InOut:    puchDispBuf - 10N10D Display Buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UieDisplay10N10D(USHORT usColumn, UCHAR uchAttr, TCHAR *pszStr,
                      TCHAR *puchDispBuf)
{
    USHORT  usLoop, usLen = UIE_10N10D_COLUMN - usColumn;
    TCHAR   uchChar;
    TCHAR   *pszBuf, *puchDest = puchDispBuf + usColumn * 2;
    TCHAR   szBuf[UIE_MAX_10N10D_BUF + 1];

    for (usLoop = 0;
         ((uchChar = *(pszStr + usLoop)) != _T('\n')) && uchChar != _T('\0');
         usLoop++) {
        if (uchChar == _T('.') || uchChar == _T(',')) {
            usLen++;
        }
    }

    RflStrAdjust(szBuf, UIE_MAX_10N10D_BUF + 1, pszStr, usLen, RFL_FEED_OFF);

    pszBuf = szBuf;
    usLoop = 0;
    if (*pszBuf == _T('.') || *pszBuf == _T(',')) {
        pszBuf++;
    }
    while (*pszBuf) {
        if (*pszBuf == _T('.') || *pszBuf == _T(',')) {
            if (usLoop == 0) {
                *puchDest++ = _T(' ');
            }
        } else {
            *puchDest++ = *pszBuf++;
        }

        if (*pszBuf == _T('.')) {
            *puchDest++ = uchAttr | (UCHAR)DISP_ATTR_DP;
            ++pszBuf;
        } else if (*pszBuf == _T(',')) {
            *puchDest++ = uchAttr | (UCHAR)DISP_ATTR_COMMA;
            ++pszBuf;
        } else {
            *puchDest++ = uchAttr;
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieBuf2SwitchDispRecovery(USHORT usDisplay,
*                                              UCHAR *puchOperBuf,
*                                              UCHAR *puchCustBuf);
*     Input:    usDisplay   - type of display
*               puchOperBuf - Operator Display Buffer 
*               puchCustBuf - Customer Display Buffer 
*
** Return:      nothing
*===========================================================================
*/
VOID UieBuf2SwitchDispRecovery(USHORT usDisplay, TCHAR *puchOperBuf,
                               TCHAR *puchCustBuf)
{
    if (usDisplay == UIE_OPER) {                /* operator display  */
        if (fchUieActive & UIE_DISPLAY &&       /* display enable    */
            fchUieActive & UIE_ACTIVE_GET) {    /* active for normal */
            UieBuf2Disp(usDisplay, puchOperBuf, puchCustBuf);
        }
        memcpy(UieRecovery.auchOperDisp, puchOperBuf, sizeof(UieRecovery.auchOperDisp));/* recovery buffer   */
    } else {                                    /* customer display  */
        if (fchUieActive & UIE_DISPLAY) {       /* display enable    */
            UieBuf2Disp(usDisplay, puchOperBuf, puchCustBuf);
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieBuf2Disp(USHORT usDisplay, UCHAR *puchOperBuf,
*                                                  UCHAR *puchCustBuf);
*     Input:    usDisplay   - type of display
*     InOut:    puchOperBuf - Operator Display Buffer
*               puchCustBuf - Customer Display Buffer
*
** Return:      nothing
*===========================================================================
*/
VOID UieBuf2Disp(USHORT usDisplay, TCHAR *puchOperBuf, TCHAR *puchCustBuf)
{
    if (usDisplay == UIE_OPER) {                    /* operator display */
        if (pUieSysCfg->uchOperType == DISP_2X20) {         /* 2x20     */
            PifDisplayAttr(DISP_OPER,
                           0,
                           0,
                           puchOperBuf,
                           UIE_2X20_COLUMN);
            PifDisplayAttr(DISP_OPER,
                           1,
                           0,
                           puchOperBuf + (UIE_2X20_COLUMN * 2),
                           UIE_2X20_COLUMN);
        } else if (pUieSysCfg->uchOperType == DISP_LCD) {   /* LCD      */
            VosDisplayAttr(usUieVosDisp,
                           0,
                           0,
                           puchOperBuf,
                           usUieDispCol);
            VosDisplayAttr(usUieVosDisp,
                           1,
                           0,
                           puchOperBuf + (usUieDispCol * 2),
                           usUieDispCol);
        } else if (pUieSysCfg->uchOperType == DISP_10N10D) {/* 10N10D   */
            PifDisplayAttr(DISP_OPER,
                           0,
                           0,
                           puchOperBuf,
                           UIE_10N10D_COLUMN);
        }
    } else {                                        /* customer display */

        if (pUieSysCfg->uchCustType == DISP_2X20) { /* 2x20 display     */
            PifDisplayAttr(DISP_CUST,
                           0,
                           0,
                           puchCustBuf,
                           UIE_2X20_COLUMN);
            PifDisplayAttr(DISP_CUST,
                           1,
                           0,
                           puchCustBuf + (UIE_2X20_COLUMN * 2),
                           UIE_2X20_COLUMN);
        } else if (pUieSysCfg->uchCustType == DISP_10N10D) { /* 10N10D  */
            PifDisplayAttr(DISP_CUST,
                           0,
                           0,
                           puchCustBuf,
                           UIE_10N10D_COLUMN);
        } else if (pUieSysCfg->uchCustType == DISP_4X20) { /* 4x20 display, saratoga     */
            PifDisplayAttr(DISP_CUST,
                           0,
                           0,
                           puchCustBuf,
                           UIE_2X20_COLUMN);
            PifDisplayAttr(DISP_CUST,
                           1,
                           0,
                           puchCustBuf + (UIE_2X20_COLUMN * 2),
                           UIE_2X20_COLUMN);
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieLightDescr(USHORT usDisplay, USHORT usNo,
*                                  UCHAR uchAttr);
*     Input:    usDisplay - type of display
*               usNo      - descriptor number
*               uchAttr   - descriptor attribute
*
** Return:      nothing
*===========================================================================
*/
VOID UieLightDescr(USHORT usDisplay, USHORT usNo, UCHAR uchAttr)
{
    USHORT  usOffset, usLength;
    UCHAR   uchCurrent, uchDAttr;
	TCHAR	auchBuffer[16];
    ULONG   flNew;
	USHORT	usTemp = 0;
	USHORT	usIndex = 0;

	// If this is not true then you probably need to check the UIEDESCLCD
	// tables in file mlddesc.c above function MldSetDescriptor() to ensure
	// that you have entered the additional entry to support a new
	// indicator.
	if (usNo >= UieLcd.usCount) {
		NHPOS_ASSERT(usNo < UieLcd.usCount);
		return;
	}

    /* --- get current attribute of descriptor --- */

	usTemp = usNo;
	usIndex = 0;
	if (usNo > UIE_MAX_2X20_DESC) {
		usTemp = usNo - UIE_MAX_2X20_DESC;
		usIndex = 1;
	}

    if (usDisplay == UIE_OPER) {
        uchCurrent = (UCHAR)(UieRecovery.flOperDesc[usIndex] >> (usTemp * UIE_DESC_SHIFT));
    } else {
        if (pUieSysCfg->uchCustType == DISP_NONE) { /* not provide */
            return;                                 /* exit ...    */
        }
        uchCurrent = (UCHAR)(UieRecovery.flCustDesc[usIndex] >> (usTemp * UIE_DESC_SHIFT));
    }

#if 0
	// We will repaint the descriptor regardless of whether we think
	// it is the same attribute or not.  We are seeing problems with
	// Master/Backup being in inquiry mode but with no descriptors displayed
	// indicating the state of the terminals.  By removing this, we
	// are now getting a true indication of the inquiry and synchronization
	// state of Master and Backup.
    if (uchAttr == (UCHAR)(uchCurrent & 0x03)) { /* same attribute */
        return;                                  /* exit ...       */
    }
#endif

    /* --- update recovery buffer and ligth descriptor --- */

    flNew = UIE_DESC_MASK << (usTemp * UIE_DESC_SHIFT);

    if (usDisplay == UIE_OPER) {            /* operator display    */
        UieRecovery.flOperDesc[usIndex] &= ~flNew;
        UieRecovery.flOperDesc[usIndex] |= ((ULONG)uchAttr << (usTemp * UIE_DESC_SHIFT));

        if (pUieSysCfg->uchOperType != DISP_LCD) {  /* not LCD     */
            PifLightDescr(DISP_OPER, usNo, uchAttr);
            return;                         /* exit ...            */
        }
    } else {                                /* customer display    */
        UieRecovery.flCustDesc[usIndex] &= ~flNew;
        UieRecovery.flCustDesc[usIndex] |= ((ULONG)uchAttr << (usTemp * UIE_DESC_SHIFT));

        PifLightDescr(DISP_CUST, usNo, uchAttr);
        return;                             /* exit ...            */
    }

    /* --- check descriptor information table --- */

    if (UieLcd.pInfo == NULL || UieLcd.puchBuf == NULL) {
        return;                             /* exit ...            */
    }

    usOffset =         UieLcd.pInfo[usNo].usOffset;
    usLength = (USHORT)UieLcd.pInfo[usNo].uchLength;

    if (usLength > TCHARSIZEOF(auchBuffer)) {    /* invalid length      */
		NHPOS_ASSERT(usLength <= TCHARSIZEOF(auchBuffer));
        return;                             /* exit ...            */
    }

    /* --- make descriptor mnemonic --- */

    if (uchAttr == UIE_DESC_OFF) {
        tcharnset(auchBuffer, _T(' '), usLength);
		//_RflFMemSet(auchBuffer, ' ', usLength); /* ### Mod (2171 for Win32) V1.0 */
    } else {
        _tcsncpy(auchBuffer, UieLcd.puchBuf + usOffset, usLength); /* ### Mod (2171 for Win32) V1.0 */
    }

    /* --- set cursor position --- */

    VosSetCurPos(usUieVosDesc,
                 (USHORT)(usOffset / UIE_LCD_COLUMN2),
                 (USHORT)(usOffset % UIE_LCD_COLUMN2)); /* ### Mod (2171 for Win32) V1.0 */

    /* --- set string attrbute --- */

    if (uchAttr == UIE_DESC_BLINK) {        /* blink attribute  */
        uchDAttr = (UCHAR)(VOS_A_BLINK | VOS_C_WHITE << 4 | VOS_C_BLACK);
    } else if (uchAttr == UIE_DESC_OFF) {   /* descriptor off   */
        uchDAttr = (UCHAR)(VOS_C_BLACK << 4 | VOS_C_WHITE);
    } else {                                /* descriptor on    */
        uchDAttr = (UCHAR)(VOS_C_WHITE << 4 | VOS_C_BLACK);
    }

    /* --- display descriptor string --- */

    VosStringAttr(usUieVosDesc, auchBuffer, usLength, uchDAttr);
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayInitEx(VOID);
*
** Return:      nothing
*
** Description: This function will be called when initiale/master reset or
*               power down recovery.
*===========================================================================
*/
VOID UieDisplayInitEx(VOID)
{
    BOOL    bResult;
    DWORD   dwResult;
    DWORD   dwCount;
    TCHAR   achDevice[SCF_USER_BUFFER];
    TCHAR   achDLL[SCF_USER_BUFFER];
    TCHAR   *pchDevice;
    TCHAR   *pchDLL;

    // clear buffer

    memset(achDevice, '\0', sizeof(achDevice));
    memset(achDLL,    '\0', sizeof(achDLL));

    // get active device information for operator display

    dwResult = ScfGetActiveDevice(SCF_TYPENAME_ODISPLAY, &dwCount, achDevice, achDLL);

    if (dwResult == SCF_SUCCESS)
    {
        pchDevice = achDevice;
        pchDLL    = achDLL;

        bResult = UieLoadDevice(pchDLL, &funcODisp, &hODispDll);
        hODisp  = bResult ? funcODisp.Open(pchDevice) : NULL;
    }

    // clear buffer

    memset(achDevice, '\0', sizeof(achDevice));
    memset(achDLL,    '\0', sizeof(achDLL));

    // get active device information for customer display
    dwResult = ScfGetActiveDevice(SCF_TYPENAME_CDISPLAY, &dwCount, achDevice, achDLL);

    if (dwResult == SCF_SUCCESS)
    {
        pchDevice = achDevice;
        pchDLL    = achDLL;

        bResult = UieLoadDevice(pchDLL, &funcCDisp, &hCDispDll);
        hCDisp  = bResult ? funcCDisp.Open(pchDevice) : NULL;
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieDisplayAttrEx();
*
** Return:      nothing
*               The line of text to be displayed is contained in a TCHAR buffer
*               in which the even numbered TCHARs, TCHAR 0, 2, 4, 6, etc contain
*               a character and the odd numbered TCHARS, TCHAR 1, 3, 5, etc.
*               contain a display attribute for the preceeding character.
*               In order to display the text, we have to remove the attributes
*               TCHARs.
*===========================================================================
*/
VOID DLLENTRY UieDisplayAttrEx(USHORT usDispId, USHORT usRow, USHORT usColumn,
    CONST TCHAR *puchString, USHORT usChars)
{
    DWORD                   dwResult;
    DWORD                   dwReturned;
	TCHAR					tchSendString[256];
	static LONG				lColumnCount = 0, lRowCount =0 ;
	static TCHAR			tchDeviceName[256];
	static TCHAR 			*ptchDeviceName = NULL;
	CONST TCHAR             *pBuf1, *pBuf2;
	TCHAR					*pBuf3;
	USHORT					i = 0;

	if (ptchDeviceName == 0)
	{
		BlFwIfGetLineDisplayInformation(tchDeviceName, &lColumnCount, &lRowCount);
		ptchDeviceName = tchDeviceName;
	}

    if (usDispId == DISP_OPER)
    {
		HANDLE                  hHandle;
		PLD_IO_CONTROL          pIoControl = NULL;
		DEVICEIO_DISPLAY_DATA   Data;

        hHandle    = hODisp;
        pIoControl = funcODisp.IoControl;

		Data.nRow         = (LONG)usRow;
		Data.nColumn      = (LONG)usColumn;
		Data.uchAttribute = 0;                  /* not use */
		Data.nChars       = (LONG)usChars;
		Data.lpuchBuffer  = (TCHAR*)puchString;

		if (pIoControl)
		{
			dwResult = pIoControl(
							hHandle,
							DEVICEIO_CTL_DISPLYA_ATTRIBUTE,
							&Data,
							sizeof(Data),
							NULL,
							0,
							&dwReturned);
		}
    }
    else
    {
		if (usRow != 0)
			return;
		pBuf1 = puchString;
		pBuf2 = puchString + usChars * 2;
		pBuf3 = tchSendString;
	   if (ptchDeviceName[0] == _T('L') && ptchDeviceName[1] == _T('i')) // if(!_tcscmp(ptchDeviceName, _T("LineDisplayAPA")))
	   {
		   if (usRow == 0)
		   {
			   pBuf1 += ((usChars - lColumnCount) * 2); //multiply by 2 because we are skipping attribute characters
//			   pBuf2 += ((usChars - lColumnCount) * 2); //multiply by 2 because we are skipping attribute characters
		   }
		   
		   usChars = (USHORT)lColumnCount; // adjust number of characters to width of APA display
	   }
		// Remove attribute formatting done by business logic
		// The same area does formatting for the lead-thru 
		for (i = 0; i < usChars; i++, pBuf3++) {
			*pBuf3 = *pBuf1;
			pBuf1 += 2;
		}
//		*pBuf3 = _T('\r'); pBuf3++;

		for (i = 0; i < usChars; i++, pBuf3++) {
			*pBuf3 = *pBuf2;
			pBuf2 += 2;
		}
		*pBuf3 = 0;

		BlFwIfWriteToLineDisplay(usDispId, usRow, usColumn, tchSendString, usChars * 2);
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieLightDescrEx();
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieLightDescrEx(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr)
{
    DWORD                       dwResult;
    DWORD                       dwReturned;
    DEVICEIO_DISPLAY_DESCRIPTOR Data;
    HANDLE                      hHandle;
    PLD_IO_CONTROL              pIoControl = NULL;

    Data.nNumber      = (LONG)usDescrId;
    Data.uchAttribute = uchAttr;            /* caution !! */

    if (usDispId == DISP_OPER)
    {
        hHandle    = hODisp;
        pIoControl = funcODisp.IoControl;
    }
    else
    {
        hHandle    = hCDisp;
        pIoControl = funcCDisp.IoControl;
    }

    if (pIoControl)
    {
        dwResult = pIoControl(
                        hHandle,
                        DEVICEIO_CTL_DISPLYA_DESCRIPTOR,
                        &Data,
                        sizeof(Data),
                        NULL,
                        0,
                        &dwReturned);
    }
}


/* ====================================== */
/* ========== End of UieDisp.C ========== */
/* ====================================== */
