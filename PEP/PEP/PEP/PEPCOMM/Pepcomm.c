/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Common Features
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEPCOMM.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Feb-05-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-09-98 : 03.03.00 : A.Mitsui   : Add V3.3
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <windows.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>

#include    "pepcomm.h"

#include    "csop.h"    /* V3.3 */
#include    "paraequ.h" /* V3.3 */
#include    "para.h"    /* V3.3 */
#include    "plu.h"

extern UINT uiCurrentCodePage;
/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/

#if defined(POSSIBLE_DEAD_CODE)
//unused
static HFONT   hPepMnemoFont;/* handle of mnemonic font(OEM_FIXED_FONT)  */
#endif
static HANDLE  hCommInst;    /* Handle of the DLL Instance               */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   VOID    PepCommInitialize();
*
**  Input   :   HANDLE  hPep    - instance handle of PEP.EXE
*               HANDLE  hComm   - instance handle of PEPCOMM.DLL
*
**  Return  :   None
*
**  Description:
*       This procedure initialize PEPCOMM.DLL
* ===========================================================================
*/
VOID    PepCommInitialize(HANDLE hPep, HANDLE hComm)
{
    HGDIOBJ     hObj;
    LOGFONT     lFont;

    /* ----- Store instance handle ----- */

//    hCommInst    = hComm;   RJC removed as PEPCOMM now part of PEP
    hCommInst    = hPep;

    /* ----- Get handle of mnemonic font ----- */

    hObj = GetStockObject(SYSTEM_FIXED_FONT);
    GetObject(hObj, sizeof(LOGFONT), (LPSTR)&lFont);
    lFont.lfHeight = PEP_MNEMO_HEIGHT;
#if defined(POSSIBLE_DEAD_CODE)
    hPepMnemoFont = CreateFontIndirect(&lFont);
#endif
    DeleteObject(hObj);

}
#if defined(POSSIBLE_DEAD_CODE)

/*
* ===========================================================================
**  Synopsis:   VOID    PepCommFinalize();
*
**  Input   :   None
*
**  Return  :   None
*
**  Description:
*       This procedure finalize PEPCOMM.DLL.
* ===========================================================================
*/
VOID    PepCommFinalize(void)
{
    /* ----- Delete object ----- */

    DeleteObject(hPepMnemoFont);

}

#endif
/*
* ===========================================================================
**  Synopsis:   void PepConv4Lto3C()
*
*   Input   :   BYTE FAR    *lpData - address of a 3 bytes data area
*               DWORD       dwPrice - converted price
**
*   Return  :   None
*
**  Description:
*       This procedure converts Long data (4 bytes) to 3 bytes BYTE data.
* ===========================================================================
*/
void PepConv4Lto3C(BYTE FAR *lpData, DWORD dwPrice)
{
    /* ----- Calculate column 2 ----- */
    *(lpData + PEP_CONV_CL2) = (BYTE) (dwPrice / PEP_CONV_MLT2);

    /* ----- Calculate column 1 ----- */
    *(lpData + PEP_CONV_CL1) = (BYTE)((dwPrice - PEP_CONV_MLT2 * (DWORD)(*(lpData + PEP_CONV_CL2))) / PEP_CONV_MLT1);

    /* ----- Calculate column 0 ----- */
    *lpData = (BYTE) (dwPrice
                        - PEP_CONV_MLT2 * (DWORD)(*(lpData + PEP_CONV_CL2))
                        - PEP_CONV_MLT1 * (DWORD)(*(lpData + PEP_CONV_CL2)));
}

/*
* ===========================================================================
**  Synopsis:   void  PepConv3Cto4L()
*
*   Input   :   LPBYTE  lpData    - address of a 3 bytes data area
*               LPDWORD lpdwPrice - converted price
**
*   Return  :   None
*
**  Description:
*       This procedure converts 3 bytes BYTE data to Long data (4 bytes).
* ===========================================================================
*/
void PepConv3Cto4L(LPBYTE lpData, LPDWORD lpdwPrice)
{
    /* ----- Convert data ----- */

    *lpdwPrice = (DWORD)(*lpData)
                 + PEP_CONV_MLT1 * (DWORD)(*(lpData + PEP_CONV_CL1))
                 + PEP_CONV_MLT2 * (DWORD)(*(lpData + PEP_CONV_CL2));
}

/*
* ===========================================================================
**  Synopsis:   VOID PepReplaceMnemonic()
*
*   Input   :   LPBYTE  lpbDst  - address of destination string
*               LPBYTE  lpbSrc  - address of source  string
*               short   nLen    - length of source string
*               BOOL    fType   - type of replace (Read or Write)
**
*   Return  :   No return values
*
**  Description:
*       This function converts font (system font <==> 850 font) and,
*       replaces DOUBLE KEY code.
* ===========================================================================
*/
VOID PepReplaceMnemonic(CONST WCHAR *lpbSrc, WCHAR *lpbDst, short nLen, BOOL fType)
{
    /* 21RFC05284 */
    /* support code 858, and support Atl+0128 entry for euro symbol */
    /* Alt+0128 <-> 0xD5 (code 858 euro symbol) */
    
    static  WCHAR    abMnemoRead[PEP_MNEMO_CHAR] = {
                        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
                        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
                        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
                        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
                        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
                        0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
                        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
                        0xC7,0xFC,0xE9,0xE2,0xE4,0xE0,0xE5,0xE7,0xEA,0xEB,0xE8,0xEF,0xEE,0xEC,0xC4,0xC5,
                        0xC9,0xE6,0xC6,0xF4,0xF6,0xF2,0xFB,0xF9,0xFF,0xD6,0xDC,0xF8,0xA3,0xD8,0xD7,0x83,
                        0xE1,0xED,0xF3,0xFA,0xF1,0xD1,0xAA,0xBA,0xBF,0xAE,0xAC,0xBD,0xBC,0xA1,0xAB,0xBB,
                        0x98/*0x80*/,0x81,0x82,0x84,0x85,0xC1,0xC2,0xC0,0xA9,0x86,0x87,0x88,0x89,0xA2,0xA5,0x8A,
                        0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0xE3,0xC3,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0xA4,
                        0xF0,0xD0,0xCA,0xCB,0xC8,0x80/*0x98*/,0xCD,0xCE,0xCF,0x99,0x9A,0x9B,0x9C,0xA6,0xCC,0x9D,
                        0xD3,0xDF,0xD4,0xD2,0xF5,0xD5,0xB5,0xFE,0xDE,0xDA,0xDB,0xD9,0xFD,0xDD,0xAF,0xB4,
                        0xAD,0xB1,0x9E,0xBE,0xB6,0xA7,0xF7,0xB8,0xB0,0xA8,0xB7,0xB9,0xB3,0xB2,0x9F,0xA0
                    };
    static  WCHAR    abMnemoWrite[PEP_MNEMO_CHAR] = {
                        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
                        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
                        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
                        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
                        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
                        0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
                        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
                        0xD5/*0xB0*/,0xB1,0xB2,0x9F,0xB3,0xB4,0xB9,0xBA,0xBB,0xBC,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,
                        0xC5,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xB0/*0xD5*/,0xD9,0xDA,0xBB,0xDC,0xDF,0xF2,0xFE,
                        0xFF,0xAD,0xBD,0x9C,0xCF,0xBE,0xDD,0xF5,0xF9,0xB8,0xA6,0xAE,0xAA,0xF0,0xA9,0xEE,
                        0xF8,0xF1,0xFD,0xFC,0xEF,0xE6,0xF4,0xFA,0xF7,0xFB,0xA7,0xAF,0xAC,0xAB,0xF3,0xA8,
                        0xB7,0xB5,0xB6,0xC7,0x8E,0x8F,0x92,0x80,0xD4,0x90,0xD2,0xD3,0xDE,0xD6,0xD7,0xD8,
                        0xD1,0xA5,0xE3,0xE0,0xE2,0xE5,0x99,0x9E,0x9D,0xEB,0xE9,0xEA,0x9A,0xED,0xE8,0xE1,
                        0x85,0xA0,0x83,0xC6,0x84,0x86,0x91,0x87,0x8A,0x82,0x88,0x89,0x8D,0xA1,0x8C,0x8B,
                        0xD0,0xA4,0x95,0xA2,0x93,0xE4,0x94,0xF6,0x9B,0x97,0xA3,0x96,0x81,0xEC,0xE7,0x98
                    };

    short   nI;
#pragma message("** WARNING **  Must fix PepReplaceMnemonic in pepcomm.c")
    memset(lpbDst, 0, nLen * 2);
	wcsncpy(lpbDst,lpbSrc, nLen);
	if (fType == PEP_MNEMO_READ) {                  /* (850 ==> system) */
//		WideCharToMultiByte (uiCurrentCodePage, 0, lpbDst, -1, lpbSrc, nLen, NULL, NULL);
	}
	else {               /* fMode == PEP_MNEMO_WRITE (system ==> 850) */
//		MultiByteToWideChar (uiCurrentCodePage, 0, lpbDst, -1, lpbSrc, nLen);
	}

    if (fType == PEP_MNEMO_READ) {                  /* (850 ==> system) */

        /* ----- Initialize Work Area with NULL ----- */

//        memset(lpbSrc, 0, nLen + 1);

//        for (nI = 0; nI < nLen; nI++) {
//            *(lpbSrc + nI) = abMnemoRead[*(lpbDst + nI)];
//        }

        for (nI = 0; nI < (nLen - 1); nI++) {
            if (*(lpbDst + nI) == PEP_MNEMO_DOUBLE) {
				//if (*(lpbDst + nI + 1) == 0x00) {
					*(lpbDst + nI) = PEP_MNEMO_CTRL;
				nI++;
				//}
            }          
        }

    } else {               /* fMode == PEP_MNEMO_WRITE (system ==> 850) */

        /* ----- Initialize Work Area with NULL ----- */

//        memset(lpbSrc, 0, nLen);

//        for (nI = 0; nI < nLen; nI++) {
//            *(lpbSrc + nI) = abMnemoWrite[*(lpbDst + nI)];
//        }

        for (nI = 0; nI < (nLen - 1); nI++) {
            if (*(lpbDst + nI) == PEP_MNEMO_CTRL) {
				//if (*(lpbDst + nI + 1) == 0x00) {
					*(lpbDst + nI) = PEP_MNEMO_DOUBLE; 
					nI++;
				//}
			}
        }
    }
	return;
}

/*
* ===========================================================================
**  Synopsis:   BOOL PEPENTRY PepSpinProc();
*
**  Input   :   HWND        hDlg    -   Handle of DialogBox
*               WPARAM      wParam  -   ScrollBar Message
*               int         idEdit  -   EditText ID
*               LPPEPSPIN   lpSpin  -   Points to SpinButton Style Structure
*
**  Return  :   TRUE    -   EditText modified
*               FALSE   -   EditText not modified
*
**  Description:
*       This function controls spinbutton action.
* ===========================================================================
*/
BOOL PepSpinProc(HWND hDlg, WPARAM wParam, int idEdit, LPPEPSPIN lpSpin)
{
    static  short   nCount;

    WCHAR    szWork[PEP_SPIN_LEN];
    BOOL    fRet = FALSE;
    long    lValue;
    short   nTmpStep;

    if ((lpSpin->nTurbo != 0) && (nCount >= lpSpin->nTurbo)) {
        nTmpStep = lpSpin->nTurStep;
    } else {
        nTmpStep = lpSpin->nStep;
    }

    switch (LOWORD(wParam)) {

    case SB_LINEDOWN:       /* Down Button was Clicked */
        /* ----- Set cursor selected ----- */
        SetFocus(GetDlgItem(hDlg, idEdit));
        SendDlgItemMessage(hDlg, idEdit, EM_SETSEL, 1, 0L);

        /* ----- Get data from edittext ----- */
        memset(szWork, 0, sizeof(szWork));
        DlgItemGetText(hDlg, idEdit, szWork, sizeof(szWork));
        lValue = _wtol(szWork);

        if ((lpSpin->fSpinStyle == PEP_SPIN_ROLL) && (lValue == lpSpin->lMin)) {
            lValue = lpSpin->lMax;
            fRet = TRUE;
        } else if (lValue < lpSpin->lMin + (long)nTmpStep) {
            lValue = lpSpin->lMin;
        } else {
            lValue -= (long)nTmpStep;
            fRet = TRUE;
        }

        /* ----- Set data to edittext ----- */
        _ltow(lValue, szWork, PEP_SPIN_CONV);
        DlgItemRedrawText(hDlg, idEdit, szWork);

        nCount++;
        break;

    case SB_LINEUP:         /* Up Button was Clicked */
        /* ----- Set cursor selected ----- */
        SetFocus(GetDlgItem(hDlg, idEdit));
        SendDlgItemMessage(hDlg, idEdit, EM_SETSEL, 1, 0L);

        /* ----- Get Price Data from EditText ----- */
        memset(szWork, 0, sizeof(szWork));
        DlgItemGetText(hDlg, idEdit, szWork, sizeof(szWork));
        lValue = _wtol(szWork);

        if ((lpSpin->fSpinStyle == PEP_SPIN_ROLL) && (lValue == lpSpin->lMax)) {
            lValue = lpSpin->lMin;
            fRet = TRUE;
        } else if (lValue + (long)nTmpStep > lpSpin->lMax) {
            lValue = lpSpin->lMax;
        } else {
            lValue += (long)nTmpStep;
            fRet = TRUE;

        }

        /* ----- Set data to edittext ----- */
        _ltow(lValue, szWork, PEP_SPIN_CONV);
        DlgItemRedrawText(hDlg, idEdit, szWork);

        nCount++;
        break;

    case SB_ENDSCROLL:          /* User Selection was Finished */
        /* ----- Initialize counter ----- */
        nCount = 0;

        /* ----- Set cursor selected ----- */
        SetFocus(GetDlgItem(hDlg, idEdit));
        SendDlgItemMessage(hDlg, idEdit, EM_SETSEL, 1, MAKELONG(0, -1));
        break;

    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   void PepSetFileName()
*
**  Input   :   HWND    hWnd       - handle of window
*               LPCSTR  lpszTitle  - address of caption string
**
*   Return  :   No return values
*
**  Description:
*       This function sets a PEP window title to a opened file name.
* ===========================================================================
*/
void PepSetFileName(HWND hWnd, LPCWSTR lpszTitle)
{
    WCHAR    szWinTitle[PEP_FILENAME_LEN];
    int     cbLen;

    /* ----- Load Window Title from Resource ----- */
    LoadString(hCommInst, IDS_PEP_TITLE, szWinTitle, PEP_STRING_LEN_MAC(szWinTitle)); //RPH FIX THIS

    /* ----- Create New Window Title ----- */
    lstrcatW(szWinTitle, lpszTitle);
    cbLen = lstrlenW(szWinTitle);
    if (szWinTitle[-- cbLen] == PEPCOMM_DECPNT) {
        szWinTitle[cbLen] = '\0';
    }

    /* ----- Set New Window Title to Finded Window ----- */
    WindowRedrawText(hWnd, szWinTitle);

}

/*
* ===========================================================================
**  Synopsis:   BOOL PepMenuControl()
*
**  Input   :   HWND    hWnd    -   Handle of Window
*               WPARAM  wParam  -   Disable / Enable Menu Message
*               LPARAM  lParam  -   Handle of Specified Menu Item
*
**  Return  :   BOOL    fRet  - return status of EnableMenuItem Function
*
**  Description:
*       This procedure enables or disables popup menus.
* ===========================================================================
*/
BOOL PepMenuControl(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU       hMenu;

    switch (LOWORD(wParam)) {
    case    PEP_MENU_INDIV_OFF:
        /* ----- get pop-up menu handle ----- */
        hMenu = GetSubMenu(GetMenu(hWnd), LOWORD(lParam));

        /* ----- disable a specified menu ----- */
        EnableMenuItem(hMenu, HIWORD(lParam), PEP_MENU_GRAYED);
        break;

    case    PEP_MENU_INDIV_ON:
        /* ----- get pop-up menu handle ----- */
        hMenu = GetSubMenu(GetMenu(hWnd), LOWORD(lParam));

        /* ----- enable a specified menu ----- */
        EnableMenuItem(hMenu, HIWORD(lParam), PEP_MENU_ENABLED);
        break;

    case    PEP_MENU_OFF:
        /* ----- disable setup & maint menus ----- */
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_SETUP, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_MAINT, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_TRANS, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_REPORT, PEP_MENU_GRAYED);
        break;

    case    PEP_MENU_ON:
        /* ----- enable setup & maint menus ----- */
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_SETUP, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_MAINT, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_TRANS, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_REPORT, PEP_MENU_ENABLED);
        break;

    case    PEP_MENU_ALL_OFF:
        /* ----- disable all menus of PEP - MAIN ----- */
        EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_FILES, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_SETUP, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_MAINT, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_TRANS, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_REPORT, PEP_MENU_GRAYED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_HELPS, PEP_MENU_GRAYED);
        break;

    case    PEP_MENU_ALL_ON:
        /* ----- enable all menus of PEP - MAIN ----- */
        EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_FILES, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_SETUP, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_MAINT, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_TRANS, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_REPORT, PEP_MENU_ENABLED);
        EnableMenuItem(GetMenu(hWnd), PEP_MENU_HELPS, PEP_MENU_ENABLED);
        break;

    }
    DrawMenuBar(hWnd);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   short PepFileBackup()
*
*   Input   :   HWND    hwnd         - handle of window
*               LPCSTR  lpszOrigFile - file name to buckup/restore
*               BOOL    fType       - buckup/restore type 
**
*   Return  :   TRUE                 - user process is executed
*               FALSE                - default process is expected
*
**  Description:
*       This procedure backup/restore to press the cancel button.
* ===========================================================================
*/
short PepFileBackup(HWND hwnd, LPCWSTR lpszOrigFile, BOOL fType)
{
    WCHAR        szTempFile[PEP_FILE_TEMP_LEN];

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    return(PepFileBackupIn(hwnd, lpszOrigFile, szTempFile, fType));

}

/*
* ===========================================================================
**  Synopsis:   void PepFileDelete()
*
*   Input   :   None
**
*   Return  :   None
*
**  Description:
*       This procedure delete backup file.
* ===========================================================================
*/
void PepFileDelete()
{
    //OFSTRUCT    OpenBuff;
    WCHAR        szWinPath[PEP_FILE_BACKUP_LEN],
                szTempPath[PEP_FILE_BACKUP_LEN],
                szTempFile[PEP_FILE_TEMP_LEN];

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    /* ----- Get PEP path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
    wsPepf(szTempPath, WIDE("%s%s"), szWinPath, szTempFile);
#else
	GetPepModuleFileName(NULL, szWinPath, sizeof(szWinPath));
	szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path

    /* ----- Make full path file name ----- */
    wsPepf(szTempPath, WIDE("%s\\%s"), szWinPath, szTempFile);
#endif

    /* ----- Open file ----- */
    DeleteFilePep(szTempPath);
}

/*
* ===========================================================================
**  Synopsis:   void PepSetFont()
*
*   Input   :   HWND    hdlg - handle of a dialogbox
*               int     nID  - identifier of a dialogbox item
**
*   Return  :   None
*
**  Description:
*       This procedure sets a OEM_FIXED_FONT font to a specified DLG control.
* ===========================================================================
*/
void PepSetFont(HWND hdlg, int nID)
{
    /* ----- set a font(OEM_FIXED_FONT) to a specified control ----- */
    SendDlgItemMessage(hdlg, nID, WM_SETFONT, (WPARAM)hResourceFont, MAKELONG(TRUE, 0));
}

/*
* ===========================================================================
**  Synopsis:   BOOL MldCreateFile()
*
*   Input   :   HWND    hwnd - handle of a dialogbox
**
*   Return  :   TRUE    MLD File create successful
*               FALSE   MLD file create false
*
**  Description:
*               MLD File Create and set default mnemonic V3.3
* ===========================================================================
*/
BOOL   MldCreateFile(HWND hwnd)
{
    short           sStat;
    UINT            sI;
    WCHAR           szErr[PEP_ALLOC_LEN], szMsg[PEP_ALLOC_LEN], szCap[PEP_CAPTION_LEN];
    MLDIF           MLDData;
    SHORT           sFlexRead;
    FLEXMEM         FlexMem[FLEX_ADR_MAX + 1];  /* w/ MLD file */

    /* ----- read previous flex memory data ----- */
    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)FlexMem, sizeof(FlexMem), 0, (USHORT *)&sFlexRead);

    /* ----- create MLD file ----- */
    if ((sStat = OpMldCreatFile(FlexMem[FLEX_MLD_ADR - 1].ulRecordNumber, 0)) != OP_PERFORM) {
        /* ----- Make Error description ----- */
        LoadString(hCommInst, IDS_MLDERR, szErr, PEP_STRING_LEN_MAC(szErr));

        if (sStat != 0) {
            wsPepf(szMsg, szErr, sStat);
        } else {
            memcpy(szMsg, szErr, sizeof(szErr));
        }

        LoadString(hCommInst, IDS_MLDERR, szCap, PEP_STRING_LEN_MAC(szCap));

        /* ----- Display Caution Message ----- */
        MessageBeep(MB_ICONQUESTION);

        MessageBoxPopUp(hwnd, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    /* ----- default mnemonic ----- */
    for (sI = 0; sI < MAX_MLDMNEMO_SIZE; sI++) {
        memset(&MLDData, 0, sizeof(MLDData));
        MLDData.uchAddress = (UCHAR)(sI + 1);

        /* ----- load default mnemonic ----- */
        LoadString(hResourceDll, IDS_MLDMNEMO001+sI, (MLDData.aszMnemonics), PEP_STRING_LEN_MAC(MLDData.aszMnemonics));

        /* ----- set default mnemonic ----- */
        if ((sStat = OpMldAssign(&MLDData, 0)) != OP_PERFORM) {
            /* ----- Make Error description ----- */
            LoadString(hResourceDll, IDS_MLDERR, szErr, PEP_STRING_LEN_MAC(szErr));

            if (sStat != 0) {
                wsPepf(szMsg, szErr, sStat);
            } else {
                memcpy(szMsg, szErr, sizeof(szErr));
            }

            LoadString(hResourceDll, IDS_MLDERR, szCap, PEP_STRING_LEN_MAC(szCap));

            /* ----- Display Caution Message ----- */
            MessageBeep(MB_ICONQUESTION);
            MessageBoxPopUp(hwnd, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }
    }

    return TRUE;
}

BOOL   RsnCreateFile(HWND hwnd)
{
    short           sStat;
    WCHAR           szErr[PEP_ALLOC_LEN], szMsg[PEP_ALLOC_LEN], szCap[PEP_CAPTION_LEN];

    /* ----- create MLD file ----- */
    if ((sStat = OpMnemonicsFileCreate(OPMNEMONICFILE_ID_REASON, 400, 0)) != OP_PERFORM) {
        /* ----- Make Error description ----- */
        LoadString(hCommInst, IDS_MLDERR, szErr, PEP_STRING_LEN_MAC(szErr));

        if (sStat != 0) {
            wsPepf(szMsg, szErr, sStat);
        } else {
            memcpy(szMsg, szErr, sizeof(szErr));
        }

        LoadString(hCommInst, IDS_MLDERR, szCap, PEP_STRING_LEN_MAC(szCap));

        /* ----- Display Caution Message ----- */
        MessageBeep(MB_ICONQUESTION);

        MessageBoxPopUp(hwnd, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}


/*
* ===========================================================================
**  Synopsis:   short PepIndexFileBackup()
*
*   Input   :   HWND    hwnd         - handle of window
*               LPCSTR  lpszOrigFile - file name to buckup/restore
*               BOOL    fType       - buckup/restore type 
**
*   Return  :   TRUE                 - user process is executed
*               FALSE                - default process is expected
*
**  Description:
*       This procedure backup/restore to press the cancel button
*       for PLU Index File, Saratoga.
* ===========================================================================
*/
short PepIndexFileBackup(HWND hwnd, LPCWSTR lpszOrigFile, BOOL fType)
{
    WCHAR       szTempFile[PEP_FILE_TEMP_LEN];

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE2, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    return(PepFileBackupIn(hwnd, lpszOrigFile, szTempFile, fType));

}

/*
* ===========================================================================
**  Synopsis:   short PepFileBackup()
*
*   Input   :   HWND    hwnd         - handle of window
*               LPCSTR  lpszOrigFile - file name to buckup/restore
*               BOOL    fType       - buckup/restore type 
**
*   Return  :   TRUE                 - user process is executed
*               FALSE                - default process is expected
*
**  Description:
*       This procedure backup/restore to press the cancel button, R2.0.
* ===========================================================================
*/
short PepFileBackupIn(HWND hwnd, LPCWSTR lpszOrigFile,
                                        LPCWSTR lpszTempFile, BOOL fType)
{
    LPWSTR   lpszBuff;
    DWORD    cbRead, cbWrite;
	BOOL	 readFinish, writeFinish;
	DWORD	 fileError = 0;
    HANDLE   hReadFile, hWriteFile;
	WCHAR    szWinPath[PEP_FILE_BACKUP_LEN] = {0};
	WCHAR    szOrigPath[PEP_FILE_BACKUP_LEN] = {0};
	WCHAR    szTempPath[PEP_FILE_BACKUP_LEN] = {0};
	WCHAR    szCap[PEP_CAPTION_LEN] = {0};
	WCHAR    szErr[PEP_ALLOC_LEN] = {0};
/*                szTempFile[PEP_FILE_TEMP_LEN]; */

    static HGLOBAL  hMem;

    /* ----- Finalize mass memory module ----- */
    PluFinalize();

    /* ----- Get windows path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
    wsPepf(szOrigPath, WIDE("%s%s"), szWinPath, lpszOrigFile);
    wsPepf(szTempPath, WIDE("%s%s"), szWinPath, lpszTempFile);
#else
	GetPepModuleFileName(NULL, szWinPath, sizeof(szWinPath));
	szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path
    //GetWindowsDirectoryPep(szWinPath, sizeof(szWinPath));

    /* ----- Load string from resource ----- */
/*
    LoadString(hCommInst,
               IDS_PEP_TEMPFILE,
               (LPSTR)szTempFile,
               sizeof(szTempFile));
*/

    /* ----- Make full path file name ----- */
    wsPepf(szOrigPath, WIDE("%s\\%s"), szWinPath, lpszOrigFile);
    wsPepf(szTempPath, WIDE("%s\\%s"), szWinPath, lpszTempFile);
#endif

    /* ----- Open file ----- */
    if (fType == PEP_FILE_BACKUP) {
        hReadFile  = CreateFilePep(szOrigPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		fileError = GetLastError();

        if (hReadFile == INVALID_HANDLE_VALUE) {
            PluInitialize();
            return FALSE;
        }
        hWriteFile = CreateFilePep(szTempPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        hReadFile  = CreateFilePep(szTempPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		fileError = GetLastError();

        if (!hReadFile || fileError == ERROR_ALREADY_EXISTS) {
            PluInitialize();
            return FALSE;
        }
        hWriteFile = CreateFilePep(szOrigPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		/*OpenFile(szOrigPath,&WriteOpenBuff,(UINT)(OF_SHARE_DENY_NONE | OF_WRITE));*/
		fileError = GetLastError();
    }

    /* ----- Allocate memory from global area ----- */
    hMem = GlobalAlloc(GHND, PEP_FILE_BUFF_LEN);

    /* ----- Check whether keep memory or not ----- */
    if (! hMem) {
        /* ----- Close file ----- */
        CloseHandle(hReadFile);
        CloseHandle(hWriteFile);

        /* ----- Load String from Resource ----- */
        LoadString(hCommInst, IDS_PEP_TITLE,   szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hCommInst, IDS_PEP_ALLOC_ERR, szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hwnd, szErr, szCap, MB_OK | MB_ICONEXCLAMATION);

        PluInitialize();
        return FALSE;
    }

    /* ----- Lock memory area ----- */
    lpszBuff = GlobalLock(hMem);

    /* ----- Copy file until end-of-file ----- */
    do {
        readFinish = ReadFile/*_lread*/(hReadFile, lpszBuff, PEP_FILE_BUFF_LEN, &cbRead, NULL);
        writeFinish = WriteFile/*_lwrite*/(hWriteFile, lpszBuff, cbRead, &cbWrite, NULL);
		fileError = GetLastError();
    } while (cbRead != 0);

    /* ----- Free memory area ----- */

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    /* ----- Close file ----- */
    CloseHandle(hReadFile);
    CloseHandle(hWriteFile);

    /* ----- Initialize mass memory module ----- */
    PluInitialize();

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   void PepIndexFileDelete()
*
*   Input   :   None
**
*   Return  :   None
*
**  Description:
*       This procedure delete backup file for Plu Index File, R2.0.
* ===========================================================================
*/
void PepIndexFileDelete()
{
    WCHAR  szWinPath[PEP_FILE_BACKUP_LEN] = {0};
    WCHAR  szTempPath[PEP_FILE_BACKUP_LEN] = {0};
    WCHAR  szTempFile[PEP_FILE_TEMP_LEN] = {0};

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE2, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    /* ----- Get windows path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
    wsPepf(szTempPath, WIDE("%s%s"), szWinPath, szTempFile);
#else
	GetPepModuleFileName(NULL, szWinPath, sizeof(szWinPath));
	szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path

    /* ----- Make full path file name ----- */
    wsPepf(szTempPath, WIDE("%s\\%s"), szWinPath, szTempFile);
#endif

    /* ----- Open file ----- */
    DeleteFilePep(szTempPath);
}

/*
* ===========================================================================
**  Synopsis:   short PepDeptFileBackup()
*
*   Input   :   HWND    hwnd         - handle of window
*               LPCSTR  lpszOrigFile - file name to buckup/restore
*               BOOL    fType       - buckup/restore type 
**
*   Return  :   TRUE                 - user process is executed
*               FALSE                - default process is expected
*
**  Description:
*       This procedure backup/restore to press the cancel button.
* ===========================================================================
*/
short PepDeptFileBackup(HWND hwnd, LPCWSTR lpszOrigFile, BOOL fType)
{
    WCHAR  szTempFile[PEP_FILE_TEMP_LEN];

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE3, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    return(PepFileBackupIn(hwnd, lpszOrigFile, szTempFile, fType));
}

/*
* ===========================================================================
**  Synopsis:   void PepDeptFileDelete()
*
*   Input   :   None
**
*   Return  :   None
*
**  Description:
*       This procedure delete backup file for Plu Index File, R2.0.
* ===========================================================================
*/
void PepDeptFileDelete()
{
	WCHAR   szWinPath[PEP_FILE_BACKUP_LEN] = {0};
    WCHAR   szTempPath[PEP_FILE_BACKUP_LEN] = {0};
    WCHAR   szTempFile[PEP_FILE_TEMP_LEN] = {0};

    /* ----- Load string from resource ----- */
    LoadString(hCommInst, IDS_PEP_TEMPFILE3, szTempFile, PEP_STRING_LEN_MAC(szTempFile));

    /* ----- Get windows path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
    wsPepf(szTempPath, WIDE("%s%s"), szWinPath, szTempFile);
#else
	GetPepModuleFileName(NULL, szWinPath, sizeof(szWinPath));
	szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path

    /* ----- Make full path file name ----- */
    wsPepf(szTempPath, WIDE("%s\\%s"), szWinPath, szTempFile);
#endif

    /* ----- Open file ----- */
    DeleteFilePep(szTempPath);
}

/* ===== End of File (PEPCOMM.C) ===== */

