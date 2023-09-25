/*
* ---------------------------------------------------------------------------
* Title         :   Wage Rate for Job Code (AC 154)
* Category      :   Setup, NCR 2170 PEP for Windows (US Model)
* Program Name  :   A154.C
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
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
* Jan-30-96 : 00.00.01 : M.Suzuki   : Initial
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a154.h"

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
static  UINT    unTopIndex,     /* Zero Based Top Index No. of ListBox      */
                unBottomIndex;  /* Zero Based Bottom Index No. of ListBox   */
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A154DlgProc()
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
*       This is a dialogbox procedure for Wage Rate for Job Code.
* ===========================================================================
*/
BOOL    WINAPI  A154DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  WORD    awWageRate[A154_ADDR_MAX];   /* Data Buffer of Wage Rate Data */

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        A154InitDlg(hDlg, awWageRate);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A154_DESC01; j<=IDD_A154_CAPTION1; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A154_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				A154ScrlProc(hDlg, SB_LINEDOWN, hScroll, awWageRate);
			} else if (dWheel > 0) {
				A154ScrlProc(hDlg, SB_LINEUP, hScroll, awWageRate);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_A154_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            A154ScrlProc(hDlg, wParam, lParam, awWageRate);

        } else {

            /* ----- Spin Button Control ----- */

            A154SpinProc(hDlg, wParam, lParam, awWageRate);

        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A154_EDIT01:
        case IDD_A154_EDIT02:
        case IDD_A154_EDIT03:
        case IDD_A154_EDIT04:
        case IDD_A154_EDIT05:
        case IDD_A154_EDIT06:
        case IDD_A154_EDIT07:
        case IDD_A154_EDIT08:
        case IDD_A154_EDIT09:
        case IDD_A154_EDIT10:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (A154GetData(hDlg, wParam, awWageRate)) {

                    /* ----- Text out Error Message ----- */

                    A154ShowErr(hDlg);

                    /* ----- Reset Previous Data to Data Buffer ----- */

                    A154ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARALABORCOST,
                                 (UCHAR *)awWageRate,
                                 sizeof(awWageRate),
                                 0, &usReturnLen);

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
**  Synopsis    :   VOID    A154InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   LPWORD  lpwData -   Points to a Buffer of wage Rate Data
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A154InitDlg(HWND hDlg, LPWORD lpwData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */

    unTopIndex    = A154_SCBAR_MIN;
    unBottomIndex = A154_ONE_PAGE;

    for (wID = IDD_A154_EDIT01; wID <= IDD_A154_EDIT10; wID++) {

        /* ----- Set Limit Length to EditText ----- */

        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A154_RATE_LEN, 0L);
    }

    /* ----- Read Wage Rate Data from Parameter File ----- */

    ParaAllRead(CLASS_PARALABORCOST,
                (UCHAR *)lpwData,
                (A154_ADDR_MAX * sizeof(WORD)),
                0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */

    A154RedrawText(hDlg, lpwData);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */

    SetScrollRange(GetDlgItem(hDlg, IDD_A154_SCBAR),
                   SB_CTL,
                   A154_SCBAR_MIN,
                   A154_SCBAR_MAX,
                   TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A154_SCBAR),
                 SB_CTL,
                 A154_SCBAR_MIN,
                 FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A154_EDIT01));
    SendDlgItemMessage(hDlg, IDD_A154_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A154GetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Current Changed EditText  
*                   LPWORD  lpwData -   Points to a Buffer of Wage Rate Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed wage rate data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Oterwise it set inputed data to rate buffer, and returns FALSE.
* ===========================================================================
*/
BOOL    A154GetData(HWND hDlg, WPARAM wParam, LPWORD lpwData)
{
    BOOL    fReturn = FALSE;    /* Return Value of This Function */
    DWORD   dwData;             /* Value of Inputed Data */
    WCHAR    szWork[128];

    /* ----- Get Inputed Data from EditText ----- */

    DlgItemGetText(hDlg, LOWORD(wParam), szWork, sizeof(szWork));

    dwData = (DWORD)_wtol(szWork);

    if ((DWORD)A154_RATE_MAX < dwData) {

        fReturn = TRUE;

    } else {

        /* ----- Set Inputed Data to a Wage Rate Data Buffer ----- */

        *(lpwData + A154GETINDEX(LOWORD(wParam), unTopIndex)) = (WORD)dwData;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A154ShowErr(HWND hDlg)
{
    WCHAR    szCaption[A154_CAP_LEN],     /* MessageBox Caption */
            szMessage[A154_ERR_LEN],     /* Error Message Strings */
            szWork[A154_ERR_LEN];        /* Error Message Strings Work Area */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A154, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, A154_RATE_MIN, A154_RATE_MAX);

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154ResetData()
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
VOID    A154ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPWORD  lpwData -   Points to a Buffer of Wage Rate Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A154SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPWORD lpwData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)A154_RATE_MIN;
    PepSpin.lMax       = (long)A154_RATE_MAX;
    PepSpin.nStep      = A154_SPIN_STEP;
    PepSpin.nTurbo     = A154_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    switch (GetDlgCtrlID((HWND)lParam)) {

    case IDD_A154_SPIN01:
        idEdit = IDD_A154_EDIT01;
        break;

    case IDD_A154_SPIN02:
        idEdit = IDD_A154_EDIT02;
        break;

    case IDD_A154_SPIN03:
        idEdit = IDD_A154_EDIT03;
        break;

    case IDD_A154_SPIN04:
        idEdit = IDD_A154_EDIT04;
        break;

    case IDD_A154_SPIN05:
        idEdit = IDD_A154_EDIT05;
        break;

    case IDD_A154_SPIN06:
        idEdit = IDD_A154_EDIT06;
        break;

    case IDD_A154_SPIN07:
        idEdit = IDD_A154_EDIT07;
        break;

    case IDD_A154_SPIN08:
        idEdit = IDD_A154_EDIT08;
        break;

    case IDD_A154_SPIN09:
        idEdit = IDD_A154_EDIT09;
        break;

    case IDD_A154_SPIN10:
        idEdit = IDD_A154_EDIT10;
    }

    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

        A154GetData(hDlg, (WORD)idEdit, lpwData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPWORD  lpwData -   Points to a Buffer of Wage Rate Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID    A154ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPWORD lpwData)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! A154CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Wage Rate Data by New Index ----- */

            A154RedrawText(hDlg, lpwData);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            A154RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A154_ONE_PAGE;

        /* ----- Redraw Description & WAge Rate Data by New Index ----- */

        A154RedrawText(hDlg, lpwData);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        A154RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A154CalcIndex()
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
BOOL    A154CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= A154_ONE_LINE;

        if (unBottomIndex < A154_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = A154_ONE_PAGE;
            unTopIndex    = A154_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= A154_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A154_ONE_LINE;

        if (unBottomIndex >= A154_ADDR_MAX) {    /* Scroll to Bottom */

            unBottomIndex = (A154_ADDR_MAX - 1);
            unTopIndex    = A154_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += A154_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A154_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= A154_ONE_PAGE;

            if (unBottomIndex <= A154_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = A154_ONE_PAGE;
                unTopIndex    = A154_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= A154_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A154_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += A154_ONE_PAGE;

            if (unBottomIndex >= A154_ADDR_MAX) {    /* Scroll to Bottom */

                unBottomIndex = (A154_ADDR_MAX - 1);
                unTopIndex    = A154_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += A154_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154RedrawText()
*
**  Input       :   HWND    hDlg    - Window Handle of a DialogBox
*                   LPWORD  lpwData - Points to a Buffer of Wage RAte Data
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & WAge Rate data to each control.
* ===========================================================================
*/
VOID    A154RedrawText(HWND hDlg, LPWORD lpwData)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WCHAR    szWork[A154_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Load Strings from Resource ----- */

        LoadString(hResourceDll, IDS_A154_DESC01 + unCount, szWork, A154_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_A154_DESC01 + unLoop, szWork);

        /* ----- Set Wage Rate Data to EditText ----- */

        SetDlgItemInt(hDlg,
                      IDD_A154_EDIT01 + unLoop,
                      (UINT)*(lpwData + (BYTE)unCount),
                      FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A154RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A154RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);

}


// ----------------------------------------------------------------------------


/*
* ===========================================================================
**  Synopsis    :   VOID    A154InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   LPWORD  lpwData -   Points to a Buffer of wage Rate Data
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
static VOID    A156InitDlg(HWND hDlg, LPWORD lpwData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex = A154_SCBAR_MIN;
    unBottomIndex = A154_ONE_PAGE;

    for (WORD wID = IDD_A154_EDIT01; wID <= IDD_A154_EDIT10; wID++) {
        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A154_RATE_LEN, 0L);
    }

    /* ----- Read Wage Rate Data from Parameter File ----- */
    ParaAllRead(CLASS_PARALABORCOST,
        (UCHAR*)lpwData,
        (A154_ADDR_MAX * sizeof(WORD)),
        0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */
    A154RedrawText(hDlg, lpwData);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_A154_SCBAR), SB_CTL, A154_SCBAR_MIN, A154_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_A154_SCBAR), SB_CTL, A154_SCBAR_MIN, FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_A154_EDIT01));
    SendDlgItemMessage(hDlg, IDD_A154_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

BOOL    WINAPI  A156DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  WORD    awSuggestedTipRate[A154_ADDR_MAX] = { 0 };   /* Data Buffer of Wage Rate Data */

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:
        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
//        A156InitDlg(hDlg, awSuggestedTipRate);    /* ----- Initialize Configulation of DialogBox ----- */
        return TRUE;

    case WM_SETFONT:
        if (hResourceFont) {
            for (int j = IDD_A154_DESC01; j <= IDD_A154_CAPTION1; j++)
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
        LPARAM  hScroll = (LPARAM)GetDlgItem(hDlg, IDD_A154_SCBAR);

        if (dWheel < 0) {
            A154ScrlProc(hDlg, SB_LINEDOWN, hScroll, awSuggestedTipRate);   /* ----- Vertical Scroll Bar Control ----- */
        }
        else if (dWheel > 0) {
            A154ScrlProc(hDlg, SB_LINEUP, hScroll, awSuggestedTipRate);
        }
    }

    /* ----- An application should return zero ----- */
    return FALSE;

    case WM_VSCROLL:
        if (GetDlgCtrlID((HWND)lParam) == IDD_A154_SCBAR) {
            A154ScrlProc(hDlg, wParam, lParam, awSuggestedTipRate);    /* ----- Vertical Scroll Bar Control ----- */
        }
        else {
            A154SpinProc(hDlg, wParam, lParam, awSuggestedTipRate);    /* ----- Spin Button Control ----- */
        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A154_EDIT01:
        case IDD_A154_EDIT02:
        case IDD_A154_EDIT03:
        case IDD_A154_EDIT04:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A154GetData(hDlg, wParam, awSuggestedTipRate)) {
                    A154ShowErr(hDlg);    /* ----- Text out Error Message ----- */
                    A154ResetData(hDlg, wParam);    /* ----- Reset Previous Data to Data Buffer ----- */
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARALABORCOST,
                        (UCHAR*)awSuggestedTipRate,
                        sizeof(awSuggestedTipRate),
                        0, &usReturnLen);

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

/* ===== End of A154.C ===== */
