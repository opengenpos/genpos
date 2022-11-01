/*
* ---------------------------------------------------------------------------
* Title         :   Set Department No.on Keyed Department Key (AC 10)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A115.C
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
* Nov-16-99 : 00.00.01 :            : Initial (NCR2172)
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
#include    <stdlib.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a115.h"

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
**  Synopsis    :   BOOL    WINAPI  A115DlgProc()
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
*           Set Department No.on Keyed Department Key (Action Code # 115)
* ===========================================================================
*/
BOOL    WINAPI  A115DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  USHORT    abDept[MAX_DEPTKEY_SIZE];   /* Data Buffer of Dept No. */

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        A115InitDlg(hDlg, abDept);
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A115_DESC01; j<=IDD_A115_CAPTION1; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_A115_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            A115ScrlProc(hDlg, wParam, lParam, abDept);

        } else {

            /* ----- Spin Button Control ----- */

            A115SpinProc(hDlg, wParam, lParam, abDept);

        }
        return FALSE;

    case WM_COMMAND:

          switch (LOWORD(wParam)) {

            case IDD_A115_EDIT01:
            case IDD_A115_EDIT02:
            case IDD_A115_EDIT03:
            case IDD_A115_EDIT04:
            case IDD_A115_EDIT05:
            case IDD_A115_EDIT06:
            case IDD_A115_EDIT07:
            case IDD_A115_EDIT08:
            case IDD_A115_EDIT09:
            case IDD_A115_EDIT10:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (A115GetData(hDlg, wParam, abDept)) {

                    /* ----- Text out Error Message ----- */

                    A115ShowErr(hDlg);

                    /* ----- Reset Previous Data to Data Buffer ----- */

                    A115ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;


        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARADEPTNOMENU,
                                 (UCHAR *)abDept,
                                 sizeof(abDept),
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
**  Synopsis    :   VOID    A115ResetData()
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
VOID    A115ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A115CalcIndex()
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
BOOL    A115CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= A115_ONE_LINE;

        if (unBottomIndex < A115_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = A115_ONE_PAGE;
            unTopIndex    = A115_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= A115_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A115_ONE_LINE;

        if (unBottomIndex >= A115_ADDR_MAX) {    /* Scroll to Bottom */

            unBottomIndex = (A115_ADDR_MAX - 1);
            unTopIndex    = A115_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += A115_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A115_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= A115_ONE_PAGE;

            if (unBottomIndex <= A115_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = A115_ONE_PAGE;
                unTopIndex    = A115_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= A115_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A115_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += A115_ONE_PAGE;

            if (unBottomIndex >= A115_ADDR_MAX) {    /* Scroll to Bottom */

                unBottomIndex = (A115_ADDR_MAX - 1);
                unTopIndex    = A115_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += A115_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A115GetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Current Changed EditText  
*                   USHORT* lpbData -   Points to a Buffer of Dept No. Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*    This function gets current inputed Dept No.data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Oterwise it set inputed data to Dept No. buffer, and returns FALSE.
* ===========================================================================
*/
BOOL    A115GetData(HWND hDlg, WPARAM wParam, USHORT* lpbData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
    
    if (A115_DEPT_MAX < unValue) {

        fReturn = TRUE;

    } else {

        /* ----- Set Inputed Data to a Dept No. Data Buffer ----- */
        *(lpbData + A115GETINDEX(LOWORD(wParam), unTopIndex)) = (USHORT)unValue;
    }
    return fReturn;
}
/*
* ===========================================================================
**  Synopsis    :   VOID    A115InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   USHORT* lpbData -   Points to a Buffer of Dept No.
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A115InitDlg(HWND hDlg, USHORT* lpbData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */

    unTopIndex    = A115_SCBAR_MIN;
    unBottomIndex = A115_ONE_PAGE;

    for (wID = IDD_A115_EDIT01; wID <= IDD_A115_EDIT10; wID++) {

        /* ----- Set Limit Length to EditText ----- */

        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A115_DEPT_LEN, 0L);
    }

    /* ----- Read Dept No Data from Parameter File ----- */

    ParaAllRead(CLASS_PARADEPTNOMENU,
                (UCHAR *)lpbData,
                MAX_DEPTKEY_SIZE * sizeof(USHORT),
                0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */

    A115RedrawText(hDlg, lpbData);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */

    SetScrollRange(GetDlgItem(hDlg, IDD_A115_SCBAR),
                   SB_CTL,
                   A115_SCBAR_MIN,
                   A115_SCBAR_MAX,
                   TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A115_SCBAR),
                 SB_CTL,
                 A115_SCBAR_MIN,
                 FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A115_EDIT01));
    SendDlgItemMessage(hDlg, IDD_A115_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A115RedrawText()
*
**  Input       :   HWND    hDlg    - Window Handle of a DialogBox
*                   USHORT* lpbData - Points to a Buffer of Dept No.
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & Dept data to each control.
* ===========================================================================
*/
VOID    A115RedrawText(HWND hDlg, USHORT* lpbData)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WCHAR    szWork[A115_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Load Strings from Resource ----- */

        LoadString(hResourceDll, IDS_A115_DESC01 + unCount, szWork, A115_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_A115_DESC01 + unLoop, szWork);

        /* ----- Set Dept No. to EditText ----- */

        SetDlgItemInt(hDlg,
                      IDD_A115_EDIT01 + unLoop,
                      (UINT)*(lpbData + (BYTE)unCount),
                      FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A115SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   USHORT* lpbData -   Points to a Buffer of Dept No. Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A115SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, USHORT* lpbData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)A115_DEPT_MIN;
    PepSpin.lMax       = (long)A115_DEPT_MAX;
    PepSpin.nStep      = A115_SPIN_STEP;
    PepSpin.nTurbo     = A115_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    switch (GetDlgCtrlID((HWND)lParam)) {

    case IDD_A115_SPIN01:
        idEdit = IDD_A115_EDIT01;
        break;

    case IDD_A115_SPIN02:
        idEdit = IDD_A115_EDIT02;
        break;

    case IDD_A115_SPIN03:
        idEdit = IDD_A115_EDIT03;
        break;

    case IDD_A115_SPIN04:
        idEdit = IDD_A115_EDIT04;
        break;

    case IDD_A115_SPIN05:
        idEdit = IDD_A115_EDIT05;
        break;

    case IDD_A115_SPIN06:
        idEdit = IDD_A115_EDIT06;
        break;

    case IDD_A115_SPIN07:
        idEdit = IDD_A115_EDIT07;
        break;

    case IDD_A115_SPIN08:
        idEdit = IDD_A115_EDIT08;
        break;

    case IDD_A115_SPIN09:
        idEdit = IDD_A115_EDIT09;
        break;

    case IDD_A115_SPIN10:
        idEdit = IDD_A115_EDIT10;
    }

    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

        A115GetData(hDlg, (WORD)idEdit, lpbData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A115ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   USHORT* lpbData -   Points to a Buffer of Dept No.
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID    A115ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, USHORT* lpbData)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! A115CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Dept No. Data by New Index ----- */

            A115RedrawText(hDlg, lpbData);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            A115RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A115_ONE_PAGE;

        /* ----- Redraw Description & Dept No. Data by New Index ----- */

        A115RedrawText(hDlg, lpbData);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        A115RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A115RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A115RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A115ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A115ShowErr(HWND hDlg)
{
    WCHAR    szCaption[A115_CAP_LEN],     /* MessageBox Caption */
            szMessage[A115_ERR_LEN],     /* Error Message Strings */
            szWork[A115_ERR_LEN];        /* Error Message Strings Work Area */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A115, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, A115_DEPT_MIN, A115_DEPT_MAX);

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/* ===== End of File (A115.C) ===== */
