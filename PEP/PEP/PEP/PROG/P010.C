/*
* ---------------------------------------------------------------------------
* Title         :   High Amount Lock-out Limit (Prog. 10)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P010.C
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
* Sep-06-98 : 03.03.00 : A.Mitsui   : V3.3
* Nov-19-99 :          : K.Yanagida : NCR2172
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
#define _WIN32_WINNT 0x0400

#include    <Windows.h>
#include    <Windowsx.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p010.h"

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
static  UINT    unTopIndex;     /* Zero Based Top Index No. of ListBox      */
static  UINT    unBottomIndex;  /* Zero Based Bottom Index No. of ListBox   */
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P010DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispathed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*           High Amount Lock-Out (HALO) Limit (Program Mode # 10)
* ===========================================================================
*/
BOOL    WINAPI  P010DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  BYTE    abHalo[P10_ADDR_MAX];   /* Data Buffer of HALO Data */

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize Configulation of DialogBox ----- */
        P10InitDlg(hDlg, abHalo);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_P10_DESC01; j <= IDD_P10_RANGE_HALO; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_MOUSEWHEEL:
		{
			int     dWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P10_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				P10ScrlProc(hDlg, SB_LINEDOWN, hScroll, abHalo);
			} else if (dWheel > 0) {
				P10ScrlProc(hDlg, SB_LINEUP, hScroll, abHalo);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

	case WM_VSCROLL:
        if (GetDlgCtrlID((HWND)lParam) == IDD_P10_SCBAR) {
            /* ----- Vertical Scroll Bar Control ----- */
            P10ScrlProc(hDlg, wParam, lParam, abHalo);
        } else {
            /* ----- Spin Button Control ----- */
            P10SpinProc(hDlg, wParam, lParam, abHalo);
        }
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P10_EDIT01:
        case IDD_P10_EDIT02:
        case IDD_P10_EDIT03:
        case IDD_P10_EDIT04:
        case IDD_P10_EDIT05:
        case IDD_P10_EDIT06:
        case IDD_P10_EDIT07:
        case IDD_P10_EDIT08:
        case IDD_P10_EDIT09:
        case IDD_P10_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (P10GetData(hDlg, wParam, abHalo)) {
                    /* ----- Text out Error Message ----- */
                    P10ShowErr(hDlg);
                    /* ----- Reset Previous Data to Data Buffer ----- */
                    P10ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDD_P10_CHECK01:
        case IDD_P10_CHECK02:
        case IDD_P10_CHECK03:
        case IDD_P10_CHECK04:
        case IDD_P10_CHECK05:
        case IDD_P10_CHECK06:
        case IDD_P10_CHECK07:
        case IDD_P10_CHECK08:
        case IDD_P10_CHECK09:
        case IDD_P10_CHECK10:
			{
				/* ----- Check Inputed Data with Data Range ----- */
				int  iCheckParam = LOWORD(wParam) + IDD_P10_EDIT01 - IDD_P10_CHECK01;
				if (P10GetData(hDlg, iCheckParam, abHalo)) {
					/* ----- Text out Error Message ----- */
					P10ShowErr(hDlg);
					/* ----- Reset Previous Data to Data Buffer ----- */
					P10ResetData(hDlg, iCheckParam);
				}
			}
            return TRUE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    ParaAllWrite(CLASS_PARATRANSHALO, (UCHAR *)abHalo, sizeof(abHalo), 0, &usReturnLen);
                    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
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
**  Synopsis    :   VOID    P10InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   LPBYTE  lpbData -   Points to a Buffer of HALO Data
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P10InitDlg(HWND hDlg, LPBYTE lpbData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P10_SCBAR_MIN;
    unBottomIndex = P10_ONE_PAGE;

    for (wID = IDD_P10_EDIT01; wID <= IDD_P10_EDIT10; wID++) {
        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P10_HALO_LEN, 0L);
    }

    /* ----- Read HALO Data from Parameter File ----- */
    ParaAllRead(CLASS_PARATRANSHALO, (UCHAR *)lpbData, (P10_ADDR_MAX * sizeof(BYTE)), 0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */
    P10RedrawText(hDlg, lpbData);
#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P10_SCBAR), SB_CTL, P10_SCBAR_MIN, P10_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P10_SCBAR), SB_CTL, P10_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P10_MAX_PAGE;
		scrollInfo.nMin = P10_SCBAR_MIN;
		scrollInfo.nMax = P10_ADDR_MAX - 1;    // range is zero to max minus 1
		scrollInfo.nPos = P10_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P10_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P10_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P10_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P10GetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Current Changed EditText  
*                   LPBYTE  lpbData -   Points to a Buffer of HALO Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed Halo data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Oterwise it set inputed data to Halo buffer, and returns FALSE.
* ===========================================================================
*/
BOOL    P10GetData(HWND hDlg, WPARAM wParam, LPBYTE lpbData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */
    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
    
    if (P10_HALO_MAX < unValue) {
        fReturn = TRUE;
    } else {
		int  iCheckParam = LOWORD(wParam) + IDD_P10_CHECK01 - IDD_P10_EDIT01;

		/* ----- Set Inputed Data to a Halo Data Buffer ----- */
		if (SendDlgItemMessage(hDlg, iCheckParam, BM_GETCHECK, 0L, 0L)) {
			unValue |= 0x80;
		}
        /* ----- Set Inputed Data to a Halo Data Buffer ----- */
        *(lpbData + P10GETINDEX(LOWORD(wParam), unTopIndex)) = (BYTE)unValue;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    P10ShowErr(HWND hDlg)
{
    WCHAR    szCaption[P10_CAP_LEN];     /* MessageBox Caption */
    WCHAR    szMessage[P10_ERR_LEN];     /* Error Message Strings */
    WCHAR    szWork[P10_ERR_LEN];        /* Error Message Strings Work Area */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P10, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, P10_HALO_MIN, P10_HALO_MAX);

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10ResetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Reset EditText
*
**  Return      :   No return value.
*
**  Description :
*       This function undoes data input to EditText.
* ===========================================================================
*/
VOID    P10ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of HALO Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    P10SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)P10_HALO_MIN;
    PepSpin.lMax       = (long)P10_HALO_MAX;
    PepSpin.nStep      = P10_SPIN_STEP;
    PepSpin.nTurbo     = P10_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    switch (GetDlgCtrlID((HWND)lParam)) {
    case IDD_P10_SPIN01:
        idEdit = IDD_P10_EDIT01;
        break;

    case IDD_P10_SPIN02:
        idEdit = IDD_P10_EDIT02;
        break;

    case IDD_P10_SPIN03:
        idEdit = IDD_P10_EDIT03;
        break;

    case IDD_P10_SPIN04:
        idEdit = IDD_P10_EDIT04;
        break;

    case IDD_P10_SPIN05:
        idEdit = IDD_P10_EDIT05;
        break;

    case IDD_P10_SPIN06:
        idEdit = IDD_P10_EDIT06;
        break;

    case IDD_P10_SPIN07:
        idEdit = IDD_P10_EDIT07;
        break;

    case IDD_P10_SPIN08:
        idEdit = IDD_P10_EDIT08;
        break;

    case IDD_P10_SPIN09:
        idEdit = IDD_P10_EDIT09;
        break;

    case IDD_P10_SPIN10:
        idEdit = IDD_P10_EDIT10;
        break;
    }

    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
        P10GetData(hDlg, (WORD)idEdit, lpbData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of HALO Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID    P10ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    switch (LOWORD(wParam)) {
    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P10CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & HALO Data by New Index ----- */
            P10RedrawText(hDlg, lpbData);
            /* ----- Redraw Scroll Box (Thumb) ----- */
            P10RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P10_ONE_PAGE;

        /* ----- Redraw Description & HALO Data by New Index ----- */
        P10RedrawText(hDlg, lpbData);
        /* ----- Redraw Scroll Box (Thumb) ----- */
        P10RedrawThumb(lParam);
		break;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P10CalcIndex()
*
**  Input       :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return      :   BOOL    TRUE    -   Scroll up to Top or Bottom of ListBox
*                           FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description :
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P10CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {
    case SB_LINEUP:
        unBottomIndex -= P10_ONE_LINE;
        if (unBottomIndex < P10_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P10_ONE_PAGE;
            unTopIndex    = P10_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P10_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P10_ONE_LINE;
        if (unBottomIndex >= P10_ADDR_MAX) {    /* Scroll to Bottom */
            unBottomIndex = (P10_ADDR_MAX - 1);
            unTopIndex    = P10_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P10_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P10_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= P10_ONE_PAGE;
            if (unBottomIndex <= P10_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P10_ONE_PAGE;
                unTopIndex    = P10_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P10_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P10_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P10_ONE_PAGE;
            if (unBottomIndex >= P10_ADDR_MAX) {    /* Scroll to Bottom */
                unBottomIndex = (P10_ADDR_MAX - 1);
                unTopIndex    = P10_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P10_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
		break;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10RedrawText()
*
**  Input       :   HWND    hDlg    - Window Handle of a DialogBox
*                   LPBYTE  lpbData - Points to a Buffer of HALO Data
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P10RedrawText(HWND hDlg, LPBYTE lpbData)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WCHAR    szWork[P10_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
        /* ----- Load Strings from Resource ----- */
        LoadString(hResourceDll, IDS_P10_DESC01 + unCount, szWork, P10_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_P10_DESC01 + unLoop, szWork);

        /* ----- Set HALO Data to EditText ----- */
		{
			UINT ulHALO = (UINT)*(lpbData + (BYTE)unCount);

			SendDlgItemMessage(hDlg, IDD_P10_CHECK01 + unLoop, BM_SETCHECK, ((ulHALO & 0x80) ? BST_CHECKED : BST_UNCHECKED), 0L);
			ulHALO &= 0x7f;
			SetDlgItemInt(hDlg, IDD_P10_EDIT01 + unLoop, ulHALO, FALSE);
		}
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P10RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    P10RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */
    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);
}

/* ===== End of P010.C ===== */
