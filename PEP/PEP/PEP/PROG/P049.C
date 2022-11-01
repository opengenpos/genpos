/*
* ---------------------------------------------------------------------------
* Title         :   Automatic Alternative Remote Printer (Prog. 49)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P049.C
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p049.h"

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
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P049DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*           Automatic Alternative Remote Printer (Program Mode #49)
* ===========================================================================
*/
BOOL    WINAPI  P049DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    UINT    unValue;            /* Inputed Data Value   */

    switch (wMsg) {

    case WM_INITDIALOG:

 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        P49InitDlg(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P49_PRINT1; j<=IDD_P49_CAPTION2; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        P49SpinProc(hDlg, wParam, lParam);

        return FALSE;
        
    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_P49_01:
        case IDD_P49_02:
        case IDD_P49_03:
        case IDD_P49_04:
        case IDD_P49_05:
        case IDD_P49_06:
        case IDD_P49_07:
        case IDD_P49_08:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Get Inputed Data from EditText ----- */

                unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

                /* ----- Check Inputed Data with Max. Value ----- */

                if (P49_DATA_MAX < unValue) {   /* Data is out of Range */

                    P49DispErr(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    if (P49SaveData(hDlg)) {

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
**  Synopsis    :   VOID    P49InitDlg()
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
VOID    P49InitDlg(HWND hDlg)
{
    BYTE    abData[P49_ADDR_MAX];
    WORD    wEditID,
            wI;
    USHORT  usReturnLen;

    /* ----- Read Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAAUTOALTKITCH,
                (UCHAR *)abData,
                sizeof(abData),
                0, &usReturnLen);

    for (wI = 0; wI < P49_ADDR_MAX; wI++) {

        /* ----- Set Target ID of EditText ----- */

        wEditID = (WORD)(IDD_P49_01 + wI);

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wEditID);

        PepSetFont(hDlg, wEditID);

        /* ----- Set Limit Length of EditText ----- */

        SendDlgItemMessage(hDlg,  wEditID, EM_LIMITTEXT, P49_DATA_LEN, 0L);

        /* ----- Set Initial Data to EditText ----- */

        SetDlgItemInt(hDlg, wEditID, (UINT)abData[wI], FALSE);
    }

    /* ----- Set Focus to EditText (Address No.1) ----- */

    SendDlgItemMessage(hDlg, IDD_P49_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P49DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Error EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the error message from resource and displays it
*   by MessageBox. And then it sets the input focus to errored EditText.
* ===========================================================================
*/
VOID    P49DispErr(HWND hDlg, WPARAM wParam)
{
    WCHAR    szWork[PEP_OVER_LEN],           /* Error Message Work Area  */
            szMessage[PEP_OVER_LEN],        /* Strings for MessageBox   */
            szCaption[PEP_CAPTION_LEN];     /* MessageBox Caption       */

    /* ----- Load Strings from Resource ----- */   

    LoadString(hResourceDll, IDS_PEP_CAPTION_P49, szCaption, PEP_STRING_LEN_MAC(szCaption));

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, P49_DATA_MIN, P49_DATA_MAX);

    /* ----- Display Error Message by MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
    SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P49SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    P49SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN PepSpin;        /* SpinButton Style Structure   */
    int     idEdit;         /* EditText ID for SpinButton   */

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = P49_SPIN_STEP;
    PepSpin.nTurbo     = P49_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
    PepSpin.lMin       = (long)P49_DATA_MIN;
    PepSpin.lMax       = (long)P49_DATA_MAX;

    /* ----- Get SpinButton ID ----- */

    idEdit = GetDlgCtrlID((HWND)lParam);

    /* ----- Set EditText ID with SpinButton ID ----- */

    idEdit -= P49_SPIN_OFFSET;

    PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P49SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from each EditText, and then it
*   writes them to Parameter file.
* ===========================================================================
*/
BOOL    P49SaveData(HWND hDlg)
{
    USHORT  usReturnLen;
    BYTE    abBuff[P49_ADDR_MAX];
    WCHAR   szCap[128],
            szWork[128],
            szDesc[128];
    UINT    unValue;
    WORD    wEditID,
            wI;
    BOOL    fTrans,
            fRet = FALSE;

    for (wI = 0, wEditID = IDD_P49_01; wI < P49_ADDR_MAX; wI++, wEditID++) {

        /* ----- Get Inputed Data from Target EditText ----- */

        unValue = (BYTE)GetDlgItemInt(hDlg, wEditID, (LPBOOL)&fTrans, FALSE);

        if (fTrans == 0) {

            /* ----- Load Error Message from Resource ----- */

            LoadString(hResourceDll, IDS_PEP_CAPTION_P49,szCap,  PEP_STRING_LEN_MAC(szCap));

            LoadString(hResourceDll, IDS_PEP_OVERRANGE,  szWork, PEP_STRING_LEN_MAC(szWork));

            wsPepf(szDesc, szWork, P49_DATA_MIN, P49_DATA_MAX);

            /* ----- Display Error Message ----- */

            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg,
                       szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);

            /* ----- Set Input Focus to Edit Text ----- */

            SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
            SetFocus(GetDlgItem(hDlg, wEditID));

            fRet = TRUE;

            break;

        } else {

            abBuff[wI] = (BYTE)unValue;
        }
    }

    if (fRet == FALSE) {

        ParaAllWrite(CLASS_PARAAUTOALTKITCH,
                     (UCHAR *)abBuff,
                     sizeof(abBuff),
                     0, &usReturnLen);

        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return (fRet);
}

/* ===== End of P049.C ===== */
