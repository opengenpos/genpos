/*
* ---------------------------------------------------------------------------
* Title         :   Set Flexible Drivethru Parameter (AC 162)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A162.C
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
 * --------------------------------------------------------------------------
 *    Georgia Southern University Research Services Foundation
 *    donated by NCR to the research foundation in 2002 and maintained here
 *    since.
 *       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
 *       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
 *       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
 *       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
 *       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
 *       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
 *       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
 *
 *    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* May-11-95 : 03.00.01 : M.Waki     : Initial (Migrate from HP US Model)
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
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

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a162.h"

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

static  BYTE    ab162MaxChkTbl[] = { 16,16,16,16, 2, 13, 2, 13, 2, 13,
                                      2, 13, 2, 13, 2, 13, 2, 13, 2, 13,
                                      2, 13, 2, 13, 2, 13, 2, 13, 2, 13,
                                      2, 13, 2, 13, 2, 13};
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A162DlgProc()
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
*               Set Flexible Drivethru Parameter (Action Code # 162)
* ===========================================================================
*/
BOOL    WINAPI  A162DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  BYTE    abData[A162_ADDR_MAX];   /* Data Buffer of Input Data    */

    USHORT  usReturnLen;                    /* Return Length of ParaAll     */

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize Configulation of DialogBox ----- */
        A162InitDlg(hDlg, (LPBYTE)abData);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A162_DESC01; j<=IDD_A162_CAPTION2; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A162_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				A162ScrlProc(hDlg, SB_LINEDOWN, hScroll, (LPBYTE)abData);
			} else if (dWheel > 0) {
				A162ScrlProc(hDlg, SB_LINEUP, hScroll, (LPBYTE)abData);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        if (GetDlgCtrlID((HWND)lParam) == IDD_A162_SCBAR) {
            /* ----- Vertical Scroll Bar Control ----- */
            A162ScrlProc(hDlg, wParam, lParam, (LPBYTE)abData);
        } else {
            /* ----- Spin Button Control ----- */
            A162SpinProc(hDlg, wParam, lParam, (LPBYTE)abData);
        }
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_A162_EDIT01:
        case IDD_A162_EDIT02:
        case IDD_A162_EDIT03:
        case IDD_A162_EDIT04:
        case IDD_A162_EDIT05:
        case IDD_A162_EDIT06:
        case IDD_A162_EDIT07:
        case IDD_A162_EDIT08:
        case IDD_A162_EDIT09:
        case IDD_A162_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A162ChkData(hDlg, LOWORD(wParam), (LPBYTE)abData)) {
                    /* ----- Text out Error Message ----- */
                    A162ShowErr(hDlg, LOWORD(wParam));
                    /* ----- Reset Previous Data to Data Buffer ----- */
                    A162ResetData(hDlg, LOWORD(wParam));
                }
                return TRUE;
            }
            return FALSE;

		case IDD_A162_BTNSET01:
			// Set the AC162 provisioning for a Counter Terminal type of system.
			// For each terminal beginning with address pair 4 and 5 for Terminal #1:
			//   Set the System Type to 0, Ordering/Payment System
			//   Set the Terminal Type to 12, COUNTER Terminal for UNPAID TRANSACTION - FULL Screen
			memset (abData, 0, sizeof(abData));
			{
				int i;
				for (i = 4; i < A162_ADDR_MAX; ) {
					abData[i] = 0; i++;    // set the System Type
					abData[i] = 12; i++;   // set the Terminal Type
				}
			}
			/* ----- Set Description to StaticText ----- */
			A162RedrawText(hDlg, abData);
			break;

		case IDD_A162_BTNSET02:
			// Set the AC162 provisioning for a mixed Counter Terminal and Drive Thru
			// type of system.  Terminals #1 - 3 are counter terminals, Terminal # 4
			// is a Drive Thru, Side #1 as Order/Payment and Terminal #5 is Delivery.
			//
			// For each Counter terminal beginning with address pair 4 and 5 for Terminal #1:
			//   Set the System Type to 0, Ordering/Payment System
			//   Set the Terminal Type to 12, COUNTER Terminal for UNPAID TRANSACTION - FULL Screen
			// For Terminal #4
			//   Set the System Type to 
			//   Set the Terminal Type to 
			// For Terminal #5
			//   Set the System Type to 
			//   Set the Terminal Type to 
			memset (abData, 0, sizeof(abData));
			{
				int i;
				for (i = 4; i < A162_ADDR_MAX; ) {
					abData[i] = 0; i++;    // set the System Type
					abData[i] = 12; i++;   // set the Terminal Type
				}
				abData[0] = 5;       // Term # of Delivery Terminal for DT Side#1
				abData[2] = 4;       // Term # of PAYMENT Terminal for DT Side#1

				i = (4 - 1) * 2 + 4; // calculate to Terminal #4, System Type
				abData[i] = 2; i++;  // set the System Type, Order & pay at one terminal, deliver at another
				abData[i] = 1; i++;  // set the Terminal Type, Order/Payment DT Terminal for DT Side#1 - 3 part Screen
				// Terminal #5 follows immediately after Terminal #4
				abData[i] = 2; i++;  // set the System Type, Order & pay at one terminal, deliver at another
				abData[i] = 3; i++;  // set the Terminal Type, Delivery DT Terminal for DT Side#1 - 3 part Screen
			}
			/* ----- Set Description to StaticText ----- */
			A162RedrawText(hDlg, abData);
			break;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    ParaAllWrite(CLASS_PARAFXDRIVE, (UCHAR *)abData, sizeof(abData), 0, &usReturnLen);
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
**  Synopsis    :   VOID    A162InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A162InitDlg(HWND hDlg, LPBYTE lpbData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = A162_SCBAR_MIN;
    unBottomIndex = A162_ONE_PAGE;

    for (wID = IDD_A162_EDIT01; wID <= IDD_A162_EDIT10; wID++) {
        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);
        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A162_DATA_LEN, 0L);
    }

    /* ----- Read Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAFXDRIVE, (UCHAR *)lpbData, (A162_ADDR_MAX * sizeof(BYTE)), 0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */
    A162RedrawText(hDlg, lpbData);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_A162_SCBAR), SB_CTL, A162_SCBAR_MIN, A162_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_A162_SCBAR), SB_CTL, A162_SCBAR_MIN, FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_A162_EDIT01));
    SendDlgItemMessage(hDlg, IDD_A162_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A162ChkData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditId -   Control ID of Current Changed EditText  
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Oterwise it set inputed data to buffer, and returns FALSE.
* ===========================================================================
*/
BOOL    A162ChkData(HWND hDlg, WORD wEditId, LPBYTE lpbData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */
    unValue = GetDlgItemInt(hDlg, wEditId, NULL, FALSE);
    if (ab162MaxChkTbl[A162GETINDEX(wEditId, unTopIndex)] < unValue) {
        fReturn = TRUE;
    } else {
        /* ----- Set Inputed Data to a Data Buffer ----- */
        *(lpbData + A162GETINDEX(wEditId, unTopIndex)) = (BYTE)unValue;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A162ShowErr(HWND hDlg, WPARAM wParam)
{
    WCHAR    szCaption[A162_CAP_LEN],     /* MessageBox Caption */
            szMessage[A162_ERR_LEN],     /* Error Message Strings */
            szWork[A162_ERR_LEN];        /* Error Message Strings Work Area */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A162, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, A162_DATA_MIN, ab162MaxChkTbl[A162GETINDEX(LOWORD(wParam), unTopIndex)]);

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162ResetData()
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
VOID    A162ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A162SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A162_SPIN_OFFSET);

    PepSpin.lMin = (long)A162_DATA_MIN;
    PepSpin.lMax = 
    (long)ab162MaxChkTbl[A162GETINDEX((WORD)idEdit, unTopIndex)];
    PepSpin.nStep      = A162_SPIN_STEP;
    PepSpin.nTurbo     = A162_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set Target EditText ID ----- */
    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
        A162ChkData(hDlg, (WORD)idEdit, lpbData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID    A162ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    switch (LOWORD(wParam)) {
    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! A162CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & Data by New Index ----- */
            A162RedrawText(hDlg, lpbData);
            /* ----- Redraw Scroll Box (Thumb) ----- */
            A162RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A162_ONE_PAGE;
        /* ----- Redraw Description & Data by New Index ----- */
        A162RedrawText(hDlg, lpbData);
        /* ----- Redraw Scroll Box (Thumb) ----- */
        A162RedrawThumb(lParam);
		break;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A162CalcIndex()
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
BOOL    A162CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {
    case SB_LINEUP:
        unBottomIndex -= A162_ONE_LINE;
        if (unBottomIndex < A162_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = A162_ONE_PAGE;
            unTopIndex    = A162_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= A162_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A162_ONE_LINE;
        if (unBottomIndex >= A162_ADDR_MAX) {    /* Scroll to Bottom */
            unBottomIndex = (A162_ADDR_MAX - 1);
            unTopIndex    = A162_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += A162_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A162_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= A162_ONE_PAGE;
            if (unBottomIndex <= A162_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = A162_ONE_PAGE;
                unTopIndex    = A162_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= A162_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A162_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += A162_ONE_PAGE;
            if (unBottomIndex >= A162_ADDR_MAX) {    /* Scroll to Bottom */
                unBottomIndex = (A162_ADDR_MAX - 1);
                unTopIndex    = A162_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += A162_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162RedrawText()
*
**  Input       :   HWND    hDlg    - Window Handle of a DialogBox
*                   LPBYTE  lpbData - Points to a Buffer of Data
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & data to each control.
* ===========================================================================
*/
VOID    A162RedrawText(HWND hDlg, LPBYTE lpbData)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WCHAR    szWork[A162_DESC_LEN];
    WCHAR    szRengWork[A162_DESC_RENG_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
        /* ----- Load Strings from Resource ----- */
        LoadString(hResourceDll, IDS_A162_DESC01 + unCount, szWork, A162_DESC_LEN);
        /* ----- Set Loaded Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_A162_DESC01 + unLoop, szWork);
        /* ----- Load Renge Strings from Resource ----- */
        LoadString(hResourceDll, IDS_A162_RENG01 + unCount, szRengWork, A162_DESC_RENG_LEN);
        /* ----- Set Loaded Renge Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_A162_RENG01 + unLoop, szRengWork);
        /* ----- Set Data to EditText ----- */
        SetDlgItemInt(hDlg, IDD_A162_EDIT01 + unLoop, (UINT)*(lpbData + (BYTE)unCount), FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A162RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A162RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */
    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);
}

/* ===== End of A162.C ===== */
