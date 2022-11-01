/*
* ---------------------------------------------------------------------------
* Title         :   Report Mnemonics (Prog. 22)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P022.C
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
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3 (Remove Server Report)
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
#include    "P022.h"

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
static LPP22MNEMO lpMnemonic;    /* Points to Global Area of Mnemonic Data */
static LPP22WORK  lpWork;        /* Points to Global Area of Work Area     */

/* Select View Data of ListBox V3.3 */
static WORD     awSelectData[P22_TOTAL_MAX] = {
                    1,   2,   3,   4,   5,   6,        8,   9,  10,   // report mnemonic 7, RPT_WAT_ADR, is no longer used. See P22_TOTAL_MAX
                   11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
                   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
				   31,  32
                };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P022DlgProc()
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
*                  Report Mnemonics (Program Mode # 22)
* ===========================================================================
*/
BOOL    WINAPI  P022DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
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
        hGlbMnemo = GlobalAlloc(GHND, sizeof(P22MNEMO) * P22_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P22WORK)  * P22_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P22DispAllocErr(hDlg, hGlbMnemo, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpMnemonic = (LPP22MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP22WORK)GlobalLock(hGlbWork);

            /* ----- Initialize Configulation of DialogBox ----- */
            P22InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P22_SCBAR; j<=IDD_P22_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P22_EDIT01:
        case IDD_P22_EDIT02:
        case IDD_P22_EDIT03:
        case IDD_P22_EDIT04:
        case IDD_P22_EDIT05:
        case IDD_P22_EDIT06:
        case IDD_P22_EDIT07:
        case IDD_P22_EDIT08:
        case IDD_P22_EDIT09:
        case IDD_P22_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {
                P22GetMnemo(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDOK:
            for (nCount = 0; nCount < P22_ADDR_MAX; nCount++) {
                /* ----- Replace Double Width Key ('~' -> 0x12) ----- */
                PepReplaceMnemonic((WCHAR *)(lpWork + nCount), (WCHAR *)(lpMnemonic + nCount), (P22_MNE_LEN ), PEP_MNEMO_WRITE);
            }

            ParaAllWrite(CLASS_PARAREPORTNAME, (UCHAR *)lpMnemonic, ((P22_MNE_LEN ) * P22_ADDR_MAX * 2), 0, &usReturnLen);
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P22_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P22ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P22ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        /* ----- Vertical Scroll Bar Control ----- */
        fReturn = P22ScrlProc(hDlg, wParam, lParam);
        /* ----- An application should return zero ----- */
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P22InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P22InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P22_SCBAR_MIN;
    unBottomIndex = P22_ONE_PAGE;

    for (wID = IDD_P22_EDIT01; wID <= IDD_P22_EDIT10; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
//		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);
        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P22_MNE_LEN, 0L);
    }

    /* ----- Read Mnemonic Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAREPORTNAME, (UCHAR *)lpMnemonic, (P22_ADDR_MAX * (P22_MNE_LEN) * 2), 0, &usReturnLen);

    for (nCount = 0; nCount < P22_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P22_MNE_LEN ), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P22RedrawText(hDlg);
#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P22_SCBAR), SB_CTL, P22_SCBAR_MIN, P22_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P22_SCBAR), SB_CTL, P22_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P22_PAGE_MAX;
		scrollInfo.nMin = P22_SCBAR_MIN;
		scrollInfo.nMax = P22_TOTAL_MAX - 1;    // range is zero to max minus 1
		scrollInfo.nPos = P22_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P22_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P22_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P22_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   VOID    P22GetMnemo();
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
VOID    P22GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[P22_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */
    memset(szWork, 0, sizeof(szWork));

    DlgItemGetText(hDlg, wItem, szWork, P22_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */
    memset((lpWork + (awSelectData[P22GETINDEX(wItem, unTopIndex)] - 1)), 0, P22_MNE_LEN * 2); /* V3.3 */
    
    /* ----- Set Inputed Data to a Data Buffer ----- */
    wcscpy((WCHAR *)(lpWork + (awSelectData[P22GETINDEX(wItem, unTopIndex)] - 1)), szWork); /* V3.3 */
}

/*
* ===========================================================================
**  Synopsis:   VOID    P22DispAllocErr();
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
VOID    P22DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P22_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P22_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P22, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

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
**  Synopsis:   BOOL    P22ScrlProc();
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
BOOL    P22ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P22CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & HALO Data by New Index ----- */
            P22RedrawText(hDlg);
            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);
            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P22_ONE_PAGE;
        /* ----- Redraw Description & HALO Data by New Index ----- */
        P22RedrawText(hDlg);
        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P22CalcIndex();
*
**  Input   :   WORD  wScrlCode    - scroll code from scroll bar control

**  Return  :   TRUE    -   Scroll up to Top or Bottom of ListBox
*               FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description:
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P22CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P22_ONE_LINE;
        if (unBottomIndex < P22_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P22_ONE_PAGE;
            unTopIndex    = P22_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P22_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P22_ONE_LINE;
        if (unBottomIndex >= P22_TOTAL_MAX) {    /* Scroll to Bottom, V3.3 */
            unBottomIndex = (P22_TOTAL_MAX - 1);  /* V3.3 */
            unTopIndex    = P22_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P22_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P22_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= P22_ONE_PAGE;
            if (unBottomIndex <= P22_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P22_ONE_PAGE;
                unTopIndex    = P22_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P22_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P22_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P22_ONE_PAGE;
            if (unBottomIndex >= P22_TOTAL_MAX) {    /* Scroll to Bottom, V3.3 */
                unBottomIndex = (P22_TOTAL_MAX - 1);  /* V3.3 */
                unTopIndex    = P22_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P22_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P22RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P22RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P22_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
        /* ----- Load Strings from Resource ----- */
        LoadString(hResourceDll, IDS_P022DESC001 + (awSelectData[unCount] - 1), szWork, P22_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_P22_DESC01 + unLoop, szWork);

        /* ----- Set Mnemonic Data to EditText ----- */
        wID = (WORD)(IDD_P22_EDIT01 + unLoop);
        DlgItemRedrawText(hDlg, wID, (WCHAR *)(lpWork + (awSelectData[P22GETINDEX(wID, unTopIndex)] -1 )));
    }
}

/* ===== End of P022.C ===== */
