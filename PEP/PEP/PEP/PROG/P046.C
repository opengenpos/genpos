/*
* ---------------------------------------------------------------------------
* Title         :   Adjective Mnemonics (Prog. 46)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P046.C
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
#include    "P046.h"

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
static UINT       unTopIndex;    /* Zero Based Top Index No. of ListBox    */
static UINT       unBottomIndex; /* Zero Based Bottom Index No. of ListBox */
static LPP46MNEMO lpMnemonic;    /* Points to Global Area of Mnemonic Data */
static LPP46WORK  lpWork;        /* Points to Global Area of Work Area     */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P046DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    msg    - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialogbox procedure for 
*                  Special Mnemonics (Program Mode # 46)
* ===========================================================================
*/
BOOL    WINAPI  P046DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbMnemo;          /* Handle of Global Area (Mnemonic) */
    static  HGLOBAL hGlbWork;           /* Handle of Global Area (Work) */

    USHORT  usReturnLen;                /* Return Length of ParaAll */
    BOOL    fReturn;                    /* Return Value of Local */
    short   nCount;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Grobal Area for Mnemonic Data ----- */

        hGlbMnemo = GlobalAlloc(GHND, sizeof(P46MNEMO) * P46_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P46WORK)  * P46_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)){     /* Memory Allocation Error */

            /* ----- Text out Error Message ----- */

            P46DispAllocErr(hDlg, hGlbMnemo, hGlbWork);

            EndDialog(hDlg, TRUE);

        } else {                                /* Allocation was Success   */

            /* ----- Lock Global Allocation Area ----- */

            lpMnemonic = (LPP46MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP46WORK)GlobalLock(hGlbWork);

            /* ----- Initialize Configulation of DialogBox ----- */

            P46InitDlg(hDlg);

        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P46_SCBAR; j<=IDD_P46_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P46_EDIT01:
        case IDD_P46_EDIT02:
        case IDD_P46_EDIT03:
        case IDD_P46_EDIT04:
        case IDD_P46_EDIT05:
        case IDD_P46_EDIT06:
        case IDD_P46_EDIT07:
        case IDD_P46_EDIT08:
        case IDD_P46_EDIT09:
        case IDD_P46_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {

                P46GetMnemo(hDlg, LOWORD(wParam));

                return TRUE;
            }
            return FALSE;

        case IDOK:

            for (nCount = 0; nCount < P46_ADDR_MAX; nCount++) {

                /* ----- Replace Double Width Key ('~' -> 0x12) ----- */

                PepReplaceMnemonic((WCHAR *)(lpWork + nCount),
                                   (WCHAR *)(lpMnemonic + nCount),
                                   (P46_MNE_LEN ) ,
                                   PEP_MNEMO_WRITE);
            }

            ParaAllWrite(CLASS_PARAADJMNEMO,
                         (UCHAR *)lpMnemonic,
                         ((P46_MNE_LEN ) * P46_ADDR_MAX * 2),
                         0, &usReturnLen);

            PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);

            /* ----- return TRUE; ----- not used */

        case IDCANCEL:

            /* ----- Unlock Global Allocated Area & Set Them Free ----- */

            GlobalUnlock(hGlbMnemo);
            GlobalFree(hGlbMnemo);

            GlobalUnlock(hGlbWork);
            GlobalFree(hGlbWork);

            EndDialog(hDlg, FALSE);

            return TRUE;
        }
        return FALSE;

    case WM_MOUSEWHEEL:
		{
			int     dWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P46_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P46ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P46ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        /* ----- Vertical Scroll Bar Control ----- */

        fReturn = P46ScrlProc(hDlg, wParam, lParam);

        /* ----- An application should return zero ----- */

        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P46InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P46InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P46_SCBAR_MIN;
    unBottomIndex = P46_ONE_PAGE;

    for (wID = IDD_P46_EDIT01; wID <= IDD_P46_EDIT10; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);
        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P46_MNE_LEN, 0L);
    }

    /* ----- Read Mnemonic Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAADJMNEMO, (UCHAR *)lpMnemonic, (P46_ADDR_MAX * (P46_MNE_LEN) * 2), 0, &usReturnLen);

    for (nCount = 0; nCount < P46_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P46_MNE_LEN), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P46RedrawText(hDlg);
#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P46_SCBAR), SB_CTL, P46_SCBAR_MIN, P46_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P46_SCBAR), SB_CTL, P46_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P46_PAGE_MAX;
		scrollInfo.nMin = P46_SCBAR_MIN;
		scrollInfo.nMax = P46_ADDR_NO - 1;    // scroll range is zero based. this TOTAL_MAX is different from other mnemonic dialogs.
		scrollInfo.nPos = P46_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P46_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P46_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P46_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   VOID    P46GetMnemo();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*               WORD  wParam    - Control ID of Current Changed EditText  
*
**  Return  :   No return value
*
**  Description:
*       This function gets current inputed Mnemonic from EditText. And set them
*      to buffer.
* ===========================================================================
*/
VOID    P46GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[P46_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */

    memset(szWork, 0, sizeof(szWork));

    DlgItemGetText(hDlg, wItem, szWork, P46_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */

    memset((lpWork + P46GETINDEX(wItem, unTopIndex)), 0, P46_MNE_LEN + 1);
    
    /* ----- Set Inputed Data to a Data Buffer ----- */

    wcscpy((WCHAR *)(lpWork + P46GETINDEX(wItem, unTopIndex)), szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P46DispAllocErr();
*
**  Input   :   HWND    hDlg      - Handle of a DialogBox Procedure
*               HGLOBAL hGlbMnemo - Handle of a Global Area (Mnemonic)
*               HGLOBAL hGlbWork  - Handle of a Global Area (Work)
*
**  Return  :   No return value.
*
**  Description:
*       This function shows a MessageBox for Error Message. And reset global
*     heap area, if allocation was success.
* ===========================================================================
*/
VOID    P46DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P46_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P46_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_P46, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Reset Allocated Area ----- */

    if (hGlbMnemo) {

        GlobalFree(hGlbMnemo);
    }

    if (hGlbWork) {

        GlobalFree(hGlbWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P46ScrlProc();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*               WORD  wParam    - 16 Bits Message Parameter
*               LONG  lParam    - 32 Bits Message Parameter
*
**  Return  :   TRUE            - User Process is Executed
*               FALSE           - Default Process is Expected
*
**  Description:
*       This function is Vertical Scroll Procedure.
* ===========================================================================
*/
BOOL    P46ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! P46CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & HALO Data by New Index ----- */

            P46RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P46_ONE_PAGE;

        /* ----- Redraw Description & HALO Data by New Index ----- */

        P46RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P46CalcIndex();
*
**  Input   :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return  :   TRUE    -   Scroll up to Top or Bottom of ListBox
*               FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description:
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P46CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P46_ONE_LINE;

        if (unBottomIndex < P46_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = P46_ONE_PAGE;
            unTopIndex    = P46_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= P46_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P46_ONE_LINE;

        if (unBottomIndex >= P46_ADDR_NO) {    /* Scroll to Bottom */

            unBottomIndex = (P46_ADDR_NO - 1);
            unTopIndex    = P46_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += P46_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P46_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= P46_ONE_PAGE;

            if (unBottomIndex <= P46_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = P46_ONE_PAGE;
                unTopIndex    = P46_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= P46_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P46_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += P46_ONE_PAGE;

            if (unBottomIndex >= P46_ADDR_NO) {    /* Scroll to Bottom */

                unBottomIndex = (P46_ADDR_NO - 1);
                unTopIndex    = P46_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += P46_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P46RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P46RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P46_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Load Strings from Resource ----- */

        LoadString(hResourceDll,
                   IDS_P046DESC001 + unCount,
                   szWork,
                   P46_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_P46_DESC01 + unLoop, szWork);

        /* ----- Set Mnemonic Data to EditText ----- */

        wID = (WORD)(IDD_P46_EDIT01 + unLoop);

        DlgItemRedrawText(hDlg,
                       wID,
                       (WCHAR *)(lpWork + P46GETINDEX(wID, unTopIndex)));
    }
}

/* ===== End of P046.C ===== */
