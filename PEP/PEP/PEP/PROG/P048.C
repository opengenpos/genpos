/*
* ---------------------------------------------------------------------------
* Title         :   Major Department Mnemonics (Prog. 48)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P048.C
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
#include    "p048.h"

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
static LPP48MNEMO lpMnemonic;   /* Points to Global Area of Mnemonic Data   */
static LPP48WORK  lpWork;       /* Points to Global Area of Work Area       */

/* Saratoga */
static  UINT    unTopIndex,     /* Zero Based Top Index No. of ListBox      */
                unBottomIndex;  /* Zero Based Bottom Index No. of ListBox   */

static WORD     awSelectData[P48_ADDR_MAX] = {
                    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
                   11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
                   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
                };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P048DlgProc()
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
*               Major Department Mnemonics (Program Mode # 48)
* ===========================================================================
*/
BOOL    WINAPI  P048DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbMnemo;          /* Handle of Global Area (Mnemonic) */
    static  HGLOBAL hGlbWork;           /* Handle of Global Area (Work)     */
/* Saratoga */
    BOOL    fReturn;                    /* Return Value of Local */

    switch (wMsg) {

    case WM_INITDIALOG:

 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Allocate Grobal Area for Mnemonic Data ----- */

        hGlbMnemo = GlobalAlloc(GHND, sizeof(P48MNEMO) * P48_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P48WORK)  * P48_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)) {    /* Memory Allocation Error  */

            /* ----- Text out Error Message ----- */

            P48DispAllocErr(hDlg, hGlbMnemo, hGlbWork);

            EndDialog(hDlg, TRUE);

        } else {                                /* Allocation was Success   */

            /* ----- Lock Global Allocation Area ----- */

            lpMnemonic = (LPP48MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP48WORK)GlobalLock(hGlbWork);

            /* ----- Initialize Configulation of DialogBox ----- */

            P48InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P48_SCBAR; j<=IDD_P48_CAPTION2; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P48_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P48ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P48ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_P48_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */
            fReturn = P48ScrlProc(hDlg, wParam, lParam);
        }

        return FALSE;
/* Saratoga End */

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

/* Saratoga Start*/
        case IDD_P48_01:
        case IDD_P48_02:
        case IDD_P48_03:
        case IDD_P48_04:
        case IDD_P48_05:
        case IDD_P48_06:
        case IDD_P48_07:
        case IDD_P48_08:
        case IDD_P48_09:
        case IDD_P48_10:
            if (HIWORD(wParam) == EN_CHANGE) {

                P48GetMnemo(hDlg, LOWORD(wParam));

                return TRUE;
            }
            return FALSE;
/* Saratoga End */

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Save Inputed Data to Parameter File ----- */

                    P48SaveData(hDlg);
                }

                /* ----- Unlock Global Allocated Area & Set Them Free ----- */

                GlobalUnlock(hGlbMnemo);
                GlobalFree(hGlbMnemo);

                GlobalUnlock(hGlbWork);
                GlobalFree(hGlbWork);

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
**  Synopsis:   BOOL    P48ScrlProc();
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
BOOL    P48ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! P48CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Mnemonic Data by New Index ----- */

            P48RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P48_ONE_PAGE;

        /* ----- Redraw Description & Mnemonic Data by New Index ----- */

        P48RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P48CalcIndex();
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
BOOL    P48CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P48_ONE_LINE;

        if (unBottomIndex < P48_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = P48_ONE_PAGE;
            unTopIndex    = P48_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= P48_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P48_ONE_LINE;

        if (unBottomIndex >= P48_ADDR_MAX) {    /* Scroll to Bottom, V3.3 */

            unBottomIndex = (P48_ADDR_MAX - 1);  /* V3.3 */
            unTopIndex    = P48_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += P48_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P48_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= P48_ONE_PAGE;

            if (unBottomIndex <= P48_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = P48_ONE_PAGE;
                unTopIndex    = P48_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= P48_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P48_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += P48_ONE_PAGE;

            if (unBottomIndex >= P48_ADDR_MAX) {    /* Scroll to Bottom, V3.3 */

                unBottomIndex = (P48_ADDR_MAX - 1);  /* V3.3 */
                unTopIndex    = P48_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += P48_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P48InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads initial data from Parameter file, and initializes
*   the contols in DialogBox.
* ===========================================================================
*/
VOID    P48InitDlg(HWND hDlg)
{
    USHORT  usRet;
    short   nCount;
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */
/* Saratoga Start */
    unTopIndex    = P48_SCBAR_MIN;
    unBottomIndex = P48_ONE_PAGE;
/* Saratoga End */

    /* ----- Load Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAMAJORDEPT, (UCHAR *)lpMnemonic, (P48_ADDR_MAX * (P48_MNE_LEN) * 2), 0, &usRet);

    for(wID = IDD_P48_01; wID <= IDD_P48_10;wID++) {
        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each Edit ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P48_MNE_LEN, 0L);
    }
 
    for (nCount = 0; nCount < P48_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P48_MNE_LEN ), PEP_MNEMO_READ);

        /* ----- Set Loaded Data to Each EditText ----- */
/***
        SetDlgItemText(hDlg, IDD_P48_01 + nCount, (LPCSTR)(lpWork + nCount));
***/
    }

    /* ----- Set Description to StaticText ----- */
    P48RedrawText(hDlg);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P48_SCBAR), SB_CTL, P48_SCBAR_MIN, P48_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P48_SCBAR), SB_CTL, P48_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P48_MAX_PAGE;
		scrollInfo.nMin = P48_SCBAR_MIN;
		scrollInfo.nMax = P48_ADDR_MAX - 1;    // scroll range is zero based. this TOTAL_MAX is different from other mnemonic dialogs.
		scrollInfo.nPos = P48_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P48_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    SetFocus(GetDlgItem(hDlg, IDD_P48_01));


    /* ----- Set HighLight Bar to EditText ----- */
    SendDlgItemMessage(hDlg, IDD_P48_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   VOID    P48GetMnemo();
*
**  Input   :   HWND  hdlg      - Handle of a DialogBox Procedure
*               WORD  wParam    - Control ID of Current Changed EditText  
*
**  Return  :   No return value
*
**  Description:
*       This function gets current inputed Mnemonic from EditText. And set them
*      to buffer.
* ===========================================================================
*/
VOID    P48GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[P48_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */

    memset(szWork, 0, sizeof(szWork));

    DlgItemGetText(hDlg, wItem, szWork, P48_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */

    memset((lpWork + (awSelectData[P48GETINDEX(wItem, unTopIndex)] - 1)), 0, P48_MNE_LEN + 1);
    
    /* ----- Set Inputed Data to a Data Buffer ----- */

    wcscpy((WCHAR *)(lpWork + (awSelectData[P48GETINDEX(wItem, unTopIndex)] - 1)), szWork);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P48SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed data from each edittext, and then it writes
*   them to Paramater file.
* ===========================================================================
*/
VOID    P48SaveData(HWND hDlg)
{
    short   nCount;
    USHORT  usRet;

    for (nCount = 0; nCount < P48_ADDR_MAX; nCount++) {

        /* ----- Get Inputed Data from EditText ----- */
/***
        GetDlgItemText(hDlg,
                       IDD_P48_01 + nCount,
                       (LPSTR)(lpWork + nCount),
                       (P48_MNE_LEN + 1));
***/
        /* ----- Replace Double Width Key ('~' -> 0x12) ----- */

        PepReplaceMnemonic((WCHAR *)(lpWork + nCount),
                           (WCHAR *)(lpMnemonic + nCount),
                           (P48_MNE_LEN ),
                           PEP_MNEMO_WRITE);
    }

    ParaAllWrite(CLASS_PARAMAJORDEPT,
                 (UCHAR *)lpMnemonic,
                 ((P48_MNE_LEN ) * 2 * P48_ADDR_MAX),
                 0, &usRet);

    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    
    return;
    
    hDlg = hDlg;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P48DispAllocErr()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   HGLOBAL hGlbMnemo   -   Handle of a Global Area (Mnemonic)
*                   HGLOBAL hGlbWork    -   Handle of a Global Area (Work)
*
**  Return      :   No return value.
*
**  Description :
*       This function shows the error message for global allocation error. And
*   it resets the global heap area, if allocation was success.
* ===========================================================================
*/
VOID    P48DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[PEP_CAPTION_LEN],
         szMessage[PEP_ALLOC_LEN];

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll,
               IDS_PEP_CAPTION_P48,
               szCaption,
               PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll,
               IDS_PEP_ALLOC_ERR,
               szMessage,
               PEP_STRING_LEN_MAC(szMessage));

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
**  Synopsis:   VOID    P48RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Mnemonic data to each control.
* ===========================================================================
*/
VOID    P48RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P48_DESC_LEN];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Load Strings from Resource ----- */

        LoadString(hResourceDll, (IDS_P48_DESC01 + unCount), szWork, P48_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_P48_DESC01 + unLoop, szWork);

        /* ----- Set Mnemonic Data to EditText ----- */

        wID = (WORD)(IDD_P48_01 + unLoop);

        DlgItemRedrawText(hDlg,
                       wID,
                       (WCHAR *)(lpWork + (awSelectData[P48GETINDEX(wID, unTopIndex)] -1 )));
    }
}


/* ===== End of File (P048.C) ===== */
