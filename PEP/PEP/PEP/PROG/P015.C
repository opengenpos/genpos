/*
* ---------------------------------------------------------------------------
* Title         :   Preset Amount for Preset Cash Key (Prog. 15)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P015.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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
#include    <Windows.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p015.h"

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

static  int  b8presetsNot4  = 0;
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P015DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*               Preset Amount for Preset Cash Key (Program Mode # 15)
* ===========================================================================
*/
BOOL    WINAPI  P015DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;        /* Configulation of SpinButton  */
    int     idEdit;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		b8presetsNot4 = ParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT3);

        /* ----- Initialize Configuration of DialogBox ----- */
        P15InitDlg(hDlg);

        /* ----- Initialize Configulation of SpinButton ----- */
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
        PepSpin.nStep      = P15_SPIN_STEP;
        PepSpin.nTurbo     = P15_SPIN_TURBO;
        PepSpin.nTurStep   = P15_SPIN_TURSTEP;
        PepSpin.lMin       = P15_DATA_MIN;
		if (b8presetsNot4) {
			PepSpin.lMax       = P15_DATA_MAX_8;
		} else {
			int j;
			PepSpin.lMax       = P15_DATA_MAX_4;
			for (j = IDD_P15_05; j <= IDD_P15_08; j++) {
				EnableWindow(GetDlgItem(hDlg, j), FALSE);
				EnableWindow(GetDlgItem(hDlg, j + P15_SPIN_OFFSET), FALSE);
			}
		}
        return TRUE;

    case WM_VSCROLL:
        /* ----- Get SpinButton ID ----- */
        idEdit = GetDlgCtrlID((HWND)lParam);

        /* ----- Set EditText ID with SpinButton ID ----- */
        idEdit -= P15_SPIN_OFFSET;
        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
        return FALSE;
        
	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_P15_01; j <= IDD_P15_DATA_RANGE; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    if (P15SaveData(hDlg)) {
                        return TRUE;
                    }
                }

                EndDialog(hDlg, FALSE);
                return TRUE;
            }
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P15InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads initial data from Parameter file, and sets loaded
*   data to each EditText. And then it limits length of input data to EditText.
* ===========================================================================
*/
VOID    P15InitDlg(HWND hDlg)
{
	DWORD   dwData[MAX_PRESETAMOUNT_SIZE] = {0};
    WORD    wI, wID;
	WORD    wMax = (b8presetsNot4) ? P15_ADDR_MAX : MAX_PRESETAMOUNT_SIZE;
    USHORT  usReturnLen;

    /* ----- Read Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAPRESETAMOUNT, (VOID *)dwData, P15_ADDR_LEN, 0, &usReturnLen);

    for (wI = 0, wID = IDD_P15_01; wI < wMax; wI++, wID++) {
		WCHAR    szWork[16];
		DWORD    dwTranslatedData = dwData[wI % MAX_PRESETAMOUNT_SIZE];

		if (b8presetsNot4) {
			if (wI >= MAX_PRESETAMOUNT_SIZE) dwTranslatedData >>= 16;
			dwTranslatedData &= 0x0000ffff;
		}

        /* ----- Set Maximum Length of Input Data ----- */
        /* ----- Set Initial Data to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P15_DATA_LEN, 0L);
        wsPepf(szWork, WIDE("%lu"), dwTranslatedData);
        DlgItemRedrawText(hDlg, wID, szWork);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P15SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range.
*                           FALSE   -   Inputed Data is Correct.
*
**  Description :
*       This function gets the inputed data from each edittext. And then it
*   checks whether the data is out of range or not. It returns TRUE, if the
*   data is out of range. Otherwise it writes the data to Parameter File, and
*   returns FALSE.
* ===========================================================================
*/
BOOL    P15SaveData(HWND hDlg)
{
    DWORD   dwData[P15_ADDR_MAX];
    WORD    wI, wID;
	WORD    wMax = (b8presetsNot4) ? P15_ADDR_MAX : MAX_PRESETAMOUNT_SIZE;
    USHORT  usReturnLen;
    BOOL    fRet = FALSE;

    for (wI = 0, wID = IDD_P15_01; wI < wMax; wI++, wID++) {
        /* ----- Determine Whether the Data is Out of Range or Not ----- */
        if (P15ChkRng(hDlg, wID, (LPDWORD)&dwData[wI])) {
            /* ----- Display Error Message of Over Range ----- */
            P15DispErr(hDlg, wID);
            fRet = TRUE;
            break;
        }
    }

    if (fRet == FALSE) {
		if (b8presetsNot4) {
			for (wI = 0; wI < MAX_PRESETAMOUNT_SIZE; wI++) {
				dwData[wI] = (dwData[wI] & 0x0000ffff) | ((dwData[wI + MAX_PRESETAMOUNT_SIZE] & 0x0000ffff) << 16);
			}
		}

        /* ----- Write Inputed Data to Parameter File ----- */
        ParaAllWrite(CLASS_PARAPRESETAMOUNT, (VOID *)dwData, P15_ADDR_LEN, 0, &usReturnLen);
        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P15ChkRng()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wEditID     -   EditText ID for Check Range
*                   LPDWORD lpdwData    -   Address of Inputed Data
*
*   Output      :   LPDWORD lpdwData    -   Inputed Data in EditText
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range.
*                           FALSE   -   Inputed Data is Correct.
**  Description :
*       This function checks whether the inputed data is out of range or not.
*   It returns TRUE, if the data is out of range. Otherwise it retures FALSE.
* ===========================================================================
*/
BOOL    P15ChkRng(HWND hDlg, WORD wEditID, LPDWORD lpdwData)
{
    BOOL    fRet = FALSE;
    WCHAR    szWork[16];
    long    lWork;

    /* ----- Get Inputed Data from EditText ----- */
    DlgItemGetText(hDlg, wEditID, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Convert String Type Data to DWORD Type Data ----- */
    lWork = _wtol(szWork);
    if (lWork < 0L) {
        lWork = 0L;
    }

    *lpdwData = (DWORD)lWork;

    /* ----- Determine Whether the Data is Out of Range or Not ----- */
    if ((LONG)*lpdwData < P15_DATA_MIN) {
        fRet = TRUE;
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P15DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wErrID  -   EditText ID of Over Range Error
*
**  Return      :   No Return Value.
*
**  Description :
*       This function displays the error message with MessageBox.
* ===========================================================================
*/
VOID    P15DispErr(HWND hDlg, WORD wErrID)
{
    WCHAR    szCaption[PEP_CAPTION_LEN], szMessage[128];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P15, szCaption, PEP_STRING_LEN_MAC(szCaption));
    LoadString(hResourceDll, IDS_P15_OVER, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Display MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set HighLight to EditText ----- */
    SetFocus(GetDlgItem(hDlg, wErrID));
    SendDlgItemMessage(hDlg, wErrID, EM_SETSEL, 0, MAKELONG(0, -1));
}

/* ===== End of File (P015.C) ===== */
