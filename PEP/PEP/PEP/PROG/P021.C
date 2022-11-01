/*
* ---------------------------------------------------------------------------
* Title         :   Lead-Through Messages (Prog. 21)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P021.C
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
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
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
#include    "P021.h"

#include <mbctype.h>
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
static LPP21MNEMO lpMnemonic;    /* Points to Global Area of Mnemonic Data */
static LPP21WORK  lpWork;        /* Points to Global Area of Work Area     */

/* Select View Data of ListBox V3.3 */
static WORD     awSelectData[P21_TOTAL_MAX] = {
                    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
                   11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
                   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
                   31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                   41,  42,  43,  44,       46,  47,  48,  49,  50,  // lead thru mnemonic 45, LDT_WTNO_ADR, is no longer used. See P21_TOTAL_MAX
                   51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
                   61,  62,  63,  64,  65,  66,  67,  68,  69,  70,
                   71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
                   81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
                   91,  92,  93,  94,  95,  96,  97,  98,  99, 100,
                  101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
                  111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
/*** NCR2172 ***/
                  121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
                  131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
                  141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
                  151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
                  161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
                  171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
                  181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
                  191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
                  201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
                  211, 212, 213, 214, 215, 216, 217, 218, 219, 220

                };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P021DlgProc()
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
*                  Lead-Through Messages (Program Mode # 21)
* ===========================================================================
*/
BOOL    WINAPI  P021DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbMnemo;          /* Handle of Global Area (Mnemonic) */
    static  HGLOBAL hGlbWork;           /* Handle of Global Area (Work) */

    USHORT  usReturnLen;                /* Return Length of ParaAll */
    BOOL    fReturn;                    /* Return Value of Local */
    short   nCount;

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /*
			----- Allocate Global Area for Mnemonic Data -----
			We are allocating space for the maximum number of possible lead thru
			mnemonics.  We allocate two bytes per character as we are working with
			wide characters (UNICODE) and not ANSI characters.
		*/
        hGlbMnemo = GlobalAlloc(GHND, sizeof(P21MNEMO) * P21_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P21WORK)  * P21_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P21DispAllocErr(hDlg, hGlbMnemo, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpMnemonic = (LPP21MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP21WORK)GlobalLock(hGlbWork);

            /* ----- Initialize Configulation of DialogBox ----- */
            P21InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P21_SCBAR; j<=IDD_P21_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P21_EDIT01:
        case IDD_P21_EDIT02:
        case IDD_P21_EDIT03:
        case IDD_P21_EDIT04:
        case IDD_P21_EDIT05:
        case IDD_P21_EDIT06:
        case IDD_P21_EDIT07:
        case IDD_P21_EDIT08:
        case IDD_P21_EDIT09:
        case IDD_P21_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {
                P21GetMnemo(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDOK:
            for (nCount = 0; nCount < P21_ADDR_MAX; nCount++) {
                /* ----- Replace Double Width Key ('~' -> 0x12) ----- */
                PepReplaceMnemonic((WCHAR *)(lpWork + nCount), (WCHAR *)(lpMnemonic + nCount), (P21_MNE_LEN), PEP_MNEMO_WRITE);
            }

            ParaAllWrite(CLASS_PARALEADTHRU, (UCHAR *)lpMnemonic, ((P21_MNE_LEN) * P21_ADDR_MAX * 2), 0, &usReturnLen);
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P21_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P21ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P21ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        /* ----- Vertical Scroll Bar Control ----- */
        fReturn = P21ScrlProc(hDlg, wParam, lParam);

        /* ----- An application should return zero ----- */
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P21InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P21InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P21_SCBAR_MIN;
    unBottomIndex = P21_ONE_PAGE;

    for (wID = IDD_P21_EDIT01; wID <= IDD_P21_EDIT10; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P21_MNE_LEN, 0L);
    }

    /* ----- Read Mnemonic Data from Parameter File ----- */
    ParaAllRead(CLASS_PARALEADTHRU, (UCHAR *)lpMnemonic, (P21_ADDR_MAX * (P21_MNE_LEN + 1) * 2), 0, &usReturnLen);

    for (nCount = 0; nCount < P21_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P21_MNE_LEN ), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P21RedrawText(hDlg);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P21_SCBAR), SB_CTL, P21_SCBAR_MIN, P21_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P21_SCBAR), SB_CTL, P21_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P21_PAGE_MAX;
		scrollInfo.nMin = P21_SCBAR_MIN;
		scrollInfo.nMax = P21_TOTAL_MAX - 1;    // range is zero to max minus 1
		scrollInfo.nPos = P21_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P21_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P21_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P21_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   VOID    P21GetMnemo();
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
VOID    P21GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[ P21_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */
    memset(szWork, 0, sizeof(szWork));

    DlgItemGetText(hDlg, wItem, szWork, P21_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */
    memset((lpWork + (awSelectData[P21GETINDEX(wItem, unTopIndex)] - 1)), 0, P21_MNE_LEN * 2);//RPH FIX THIS
    
    /* ----- Set Inputed Data to a Data Buffer ----- */
    wcscpy((WCHAR *)(lpWork + (awSelectData[P21GETINDEX(wItem, unTopIndex)] - 1)), szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P21DispAllocErr();
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
VOID    P21DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P21_CAP_LEN+1];        /* MessageBox Caption */
    WCHAR szMessage[P21_ERR_LEN+1];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P21, szCaption, P21_CAP_LEN);

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, P21_ERR_LEN);

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
**  Synopsis:   BOOL    P21ScrlProc();
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
BOOL    P21ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P21CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & HALO Data by New Index ----- */
            P21RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);
            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P21_ONE_PAGE;

        /* ----- Redraw Description & HALO Data by New Index ----- */
        P21RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P21CalcIndex();
*
**  Input   :   WORD  wScrlCode    - scroll code from scroll bar control

**  Return  :   TRUE    -   Scroll up to Top or Bottom of ListBox
*               FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description:
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P21CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P21_ONE_LINE;
        if (unBottomIndex < P21_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P21_ONE_PAGE;
            unTopIndex    = P21_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P21_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P21_ONE_LINE;
        if (unBottomIndex >= P21_TOTAL_MAX) {     /* Scroll to Bottom, V3.3 */
            unBottomIndex = (P21_TOTAL_MAX - 1);    /* V3.3 */
            unTopIndex    = P21_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P21_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P21_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= P21_ONE_PAGE;
            if (unBottomIndex <= P21_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P21_ONE_PAGE;
                unTopIndex    = P21_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P21_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P21_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P21_ONE_PAGE;
            if (unBottomIndex >= P21_TOTAL_MAX) {       /* Scroll to Bottom, V3.3 */
                unBottomIndex = (P21_TOTAL_MAX - 1);    /* V3.3 */
                unTopIndex    = P21_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P21_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P21RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P21RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P21_DESC_LEN+1];

	memset (szWork, 0, sizeof(szWork));
	for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
        /* ----- Load Strings from Resource ----- */
		SendDlgItemMessage(hDlg, IDD_P21_DESC01 + unLoop, WM_SETFONT, (WPARAM)hResourceFont, 0);

        LoadString(hResourceDll, IDS_P021DESC001 + (awSelectData[unCount] - 1), szWork, P21_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_P21_DESC01 + unLoop, szWork);
        /* ----- Set Mnemonic Data to EditText ----- */
        wID = (WORD)(IDD_P21_EDIT01 + unLoop);

        DlgItemRedrawText(hDlg, wID, (WCHAR *)(lpWork + (awSelectData[P21GETINDEX(wID, unTopIndex)] - 1)));
    }
}

/* ===== End of P021.C ===== */
