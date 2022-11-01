/*
* ---------------------------------------------------------------------------
* Title         :   Tender Key Parameter (Prog. 62)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P062.C
* Copyright (C) :   1998, NCR Corp. E&M-OISO, All rights reserved.
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
* Sep-08-98 : 03.03.00 : A.Mitsui   : Initial
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
#include    "p062.h"

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
**  Synopsis    :   BOOL    WINAPI  P062DlgProc()
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
*           Tender Key Parameter (Program Mode #62)
* ===========================================================================
*/
BOOL    WINAPI  P062DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    UINT    unValue;        /* Inputed Data Value */

    switch (wMsg) {

    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        P62InitDlg(hDlg);

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P62_01; j<=IDD_P62_CAPTION1; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_P62_TEND12;j<=IDD_P62_TEND20; j++)
			{ //Tenders 12-20
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}

			//Title 2
			SendDlgItemMessage(hDlg, IDD_P62_TITLE_02, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        P62SpinProc(hDlg, wParam, lParam);

        return FALSE;
        
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P62_01:
        case IDD_P62_02:
        case IDD_P62_03:
        case IDD_P62_04:
        case IDD_P62_05:
        case IDD_P62_06:
        case IDD_P62_07:
        case IDD_P62_08:
        case IDD_P62_09:
        case IDD_P62_10:
        case IDD_P62_11:
		case IDD_P62_12:
        case IDD_P62_13:
        case IDD_P62_14:
        case IDD_P62_15:
        case IDD_P62_16:
        case IDD_P62_17:
        case IDD_P62_18:
        case IDD_P62_19:
        case IDD_P62_20:

            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Get Inputed Data from EditText ----- */
                unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

                /* ----- Check Inputed Data with Max. Value ----- */
                if ((P62_DATA_MAX < unValue) || (unValue == 9) || (unValue == 1)) {   /* Data is out of Range */
                    P62DispErr(hDlg, wParam, unValue);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    if (P62SaveData(hDlg)) {
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
**  Synopsis    :   VOID    P62InitDlg()
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
VOID    P62InitDlg(HWND hDlg)
{
    BYTE    abData[P62_ADDR_MAX];
    WORD    wEditID, wI;
    USHORT  usReturnLen;

    /* ----- Read Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARATEND, (UCHAR *)abData, sizeof(abData), 0, &usReturnLen);

    for (wI = 0; wI < P62_ADDR_MAX; wI++) {
		if (abData[wI] > P62_DATA_MAX) {
			abData[wI] = 0;
		}

        /* ----- Set Target ID of EditText ----- */
        wEditID = (WORD)(IDD_P62_01 + wI);

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wEditID);

        PepSetFont(hDlg, wEditID);

        /* ----- Set Limit Length of EditText ----- */
        SendDlgItemMessage(hDlg,  wEditID, EM_LIMITTEXT, P62_DATA_LEN, 0L);

        /* ----- Set Initial Data to EditText ----- */
        SetDlgItemInt(hDlg, wEditID, (UINT)abData[wI], FALSE);
    }

    /* ----- Set Focus to EditText (Address No.1) ----- */
    SendDlgItemMessage(hDlg, IDD_P62_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P62DispErr()
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
VOID    P62DispErr(HWND hDlg, WPARAM wParam, UINT unValue)
{
    WCHAR    szWork[PEP_OVER_LEN],           /* Error Message Work Area  */
            szMessage[PEP_OVER_LEN],        /* Strings for MessageBox   */
            szCaption[PEP_CAPTION_LEN];     /* MessageBox Caption       */


	if(unValue != 9 && unValue != 1)
	{
		/* ----- Load Strings from Resource ----- */   
		LoadString(hResourceDll, IDS_PEP_CAPTION_P62, szCaption, PEP_STRING_LEN_MAC(szCaption));
		LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

		wsPepf(szMessage,szWork, P62_DATA_MIN, P62_DATA_MAX);

		/* ----- Display Error Message by MessageBox ----- */
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

		/* ----- Set Focus to Error EditText ----- */
		SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
		SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
		SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
	}else
	{
		/* ----- Load String from Resource ----- */   
		LoadString(hResourceDll, IDS_PEP_CAPTION_P62, szCaption, PEP_STRING_LEN_MAC(szCaption));
		LoadString(hResourceDll, IDS_P62_ERR_NOTUSE, szMessage, PEP_STRING_LEN_MAC(szMessage));

		MessageBeep(MB_ICONEXCLAMATION);
		MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

		/* ----- Set Focus to Error EditText ----- */
		SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
		SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
		SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
	}
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P62SpinProc()
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
VOID    P62SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN PepSpin;        /* SpinButton Style Structure   */
    int     idEdit;         /* EditText ID for SpinButton   */

    /* ----- Initialize spin mode flag ----- */
    PepSpin.nStep      = P62_SPIN_STEP;
    PepSpin.nTurbo     = P62_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
    PepSpin.lMin       = (long)P62_DATA_MIN;
    PepSpin.lMax       = (long)P62_DATA_MAX;

    /* ----- Get SpinButton ID ----- */
    idEdit = GetDlgCtrlID((HWND)lParam);

    /* ----- Set EditText ID with SpinButton ID ----- */
    idEdit -= P62_SPIN_OFFSET;

    PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P62SaveData()
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
BOOL    P62SaveData(HWND hDlg)
{
    USHORT  usReturnLen;
    BYTE    abBuff[P62_ADDR_MAX];
    WCHAR   szCap[128], szWork[128];
    UINT    unValue;
    WORD    wEditID, wI;
    BOOL    fTrans, fRet = FALSE;

    for (wI = 0, wEditID = IDD_P62_01; wI < P62_ADDR_MAX; wI++, wEditID++) {
        /* ----- Get Inputed Data from Target EditText ----- */
        unValue = (BYTE)GetDlgItemInt(hDlg, wEditID, (LPBOOL)&fTrans, FALSE);

        if (fTrans == 0) {
            P62DispErr(hDlg, wEditID, 0);
            fRet = TRUE;
            break;
        } else if ((unValue == (UINT)P62_DATA_NOTUSE)|| (unValue == 9)) {
            /* ----- Load Strings from Resource ----- */   
            LoadString(hResourceDll, IDS_PEP_CAPTION_P62, szCap, PEP_STRING_LEN_MAC(szCap));
            LoadString(hResourceDll, IDS_P62_ERR_NOTUSE, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Display Error Message by MessageBox ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szWork, szCap, MB_OK | MB_ICONEXCLAMATION);

            /* ----- Set Focus to Error EditText ----- */
            SetFocus(GetDlgItem(hDlg, wEditID));
            SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));

            fRet = TRUE;
            break;
        } else {
            abBuff[wI] = (BYTE)unValue;
        }
    }

    if (!fRet) {
        ParaAllWrite(CLASS_PARATEND, (UCHAR *)abBuff, sizeof(abBuff), 0, &usReturnLen);
        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return (fRet);
}

/* ===== End of P062.C ===== */