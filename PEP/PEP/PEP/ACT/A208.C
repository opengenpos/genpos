/*
* ---------------------------------------------------------------------------
* Title         :   Set Boundary Age Assignment (AC 208) 
* Category      :   Maintenance, NCR 2172 PEP for Windows (Hotel US Model)
* Program Name  :   A208.C
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
* Nov-08-99 : 00.00.01 :            : Initial
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a208.h"

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
**  Synopsis    :   BOOL    WINAPI  A208DlgProc()
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
*               Set Boundary Age (Action Code # 208)
* ===========================================================================
*/
BOOL    WINAPI  A208DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;

    int     idEdit;
    WCHAR    szDesc[128],
            szWork[128],
            szCap[128];
    USHORT  usReturnLen;
    UCHAR   aunData[A208_ADDR_MAX];
	int     unI = 0;
    BOOL    fTrans;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A208_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);

        LoadString(hResourceDll, IDS_A208_ADDRA, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A208_STRADDRA, szDesc);
        LoadString(hResourceDll, IDS_A208_ADDRB, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A208_STRADDRB, szDesc);
        LoadString(hResourceDll, IDS_A208_ADDRC, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A208_STRADDRC, szDesc);
        LoadString(hResourceDll, IDS_A208_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A208_STRDESC, szDesc);
    
        /* ----- Read Initial Data from Parameter File ----- */

        ParaAllRead(CLASS_PARABOUNDAGE,
                    (UCHAR *)aunData,
                    sizeof(aunData),
                    0,
                    &usReturnLen);
        for (unI = 0; unI < A208_ADDR_MAX; unI++) {

            /* ----- Limit Length of Input Data to EditText ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A208_ADDRA + unI,
                               EM_LIMITTEXT,
                               A208_DATA_LEN,
                               0L);
        
            /* ----- Set Initial Data to EditText ----- */

            SetDlgItemInt(hDlg, IDD_A208_ADDRA + unI, aunData[unI], FALSE);

        }

        /* ----- Set Focus to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_A208_ADDRA, EM_SETSEL, 1, MAKELONG(0, -1));

        /* ----- Initialize Configulation of SpinButton ----- */

        PepSpin.lMin       = (long)A208_DATA_MIN;
        PepSpin.lMax       = (long)A208_DATA_MAX;
        PepSpin.nStep      = A208_SPIN_STEP;
        PepSpin.nTurbo     = A208_SPIN_TURBO;
        PepSpin.nTurStep   = A208_SPIN_TURSTEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A208_ADDRA; j<=IDD_A208_ADDRC; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A208_STRADDRA; j<=IDD_A208_STRDESC; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */

        idEdit = GetDlgCtrlID((HWND)lParam);
        idEdit -= A208_SPIN_OFFSET;

        /* ----- Common SpinButton Procedure ----- */

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;   

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_A208_ADDRA:
        case IDD_A208_ADDRB:
        case IDD_A208_ADDRC:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (A208ChkDatePos(hDlg, wParam)) {

                    /* ----- Display Error Message ----- */

                    A208DispErr(hDlg, wParam);
                    SetDlgItemInt(hDlg, wParam, (WORD)PepSpin.lMin, FALSE);

                    /* ----- Set Focus to EditText ----- */

                    SendDlgItemMessage(hDlg, wParam, EM_SETSEL, 
                                       1, MAKELONG(0, -1));

                }
                return TRUE;
            }

            return FALSE;

        case IDOK:
        case IDCANCEL:
//			memset(aunData, 0, sizeof(aunData));
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {

                    for (unI = 0; unI < A208_ADDR_MAX; unI++) {

                        /* ----- Get Security Number from EditText ----- */

                        aunData[unI] = (UCHAR)(GetDlgItemInt(hDlg,
                                                     IDD_A208_ADDRA + unI,
                                                     (LPBOOL)&fTrans,
                                                     FALSE));

                        /* ----- Check Inputed Data is Valid or Not ----- */

						if (fTrans) {
							fTrans = (aunData[unI] >= (UINT)A208_DATA_MIN && aunData[unI] <= (UINT)A208_DATA_MAX);
						}

                        if (!fTrans) {

                            /* ----- Make error message ----- */

                            LoadString(hResourceDll, IDS_A208_CAPTION, szCap, PEP_STRING_LEN_MAC(szCap));
                            LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
                            wsPepf(szDesc, szWork, A208_DATA_MIN, A208_DATA_MAX);

                            /* ----- Display Error Message ----- */

                            MessageBeep(MB_ICONEXCLAMATION);
                            MessageBoxPopUp(hDlg,
                                       szDesc,
                                       szCap,
                                       MB_ICONEXCLAMATION | MB_OK);

                            /* ----- Set Input Focus to EditText ----- */

                            SendDlgItemMessage(hDlg,
                                               IDD_A208_ADDRA + unI,
                                               EM_SETSEL,
                                               TRUE,
                                               MAKELONG(0, -1));

                            SetFocus(GetDlgItem(hDlg, IDD_A208_ADDRA + unI));

                            return TRUE;

                        }
                    }

                    /* ----- Write data to parameter file ----- */

                    ParaAllWrite(CLASS_PARABOUNDAGE,
                                 (UCHAR *)aunData,
                                 sizeof(aunData),
                                 0,
                                 &usReturnLen);

                    /* ----- Set modifier flag ----- */

                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

                }

                EndDialog(hDlg, LOWORD(wParam));
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
**  Synopsis    :   BOOL    A208ChkDatePos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed date position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A208ChkDatePos(HWND hDlg, WPARAM wID)
{
    BOOL	fCheck = FALSE;
    UINT	nPos = 0;

    /* ----- Get the Inputed Data from the Specified EditText ----- */

    nPos = GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */

    if ((nPos < A208_DATA_MIN) || (nPos > A208_DATA_MAX)) {

        fCheck = TRUE;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A208DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*   MessageBox.
* ===========================================================================
*/
VOID    A208DispErr(HWND hDlg, WPARAM wID)
{
    WORD    wMin,
            wMax;
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load String from Resource ----- */

    LoadString(hResourceDll, IDS_A208_CAPTION, szCaption, PEP_CAPTION_LEN);

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wMin = A208_DATA_MIN;
    wMax = A208_DATA_MAX;

    wsPepf(szMessage, szWork, wMin, wMax);

    /* ----- Display Error Message with MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */

  	SendDlgItemMessage(hDlg, LOWORD(wID), EM_UNDO, 0, 0L);
	SetFocus(GetDlgItem(hDlg, wID));
}

/* ===== End of A208.C ===== */
