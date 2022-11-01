/*
* ---------------------------------------------------------------------------
* Title         :   Transaction Mnemonics (Prog. 20)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P020.C
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
#include    "p020.h"

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
static LPP20MNEMO lpMnemonic;    /* Points to Global Area of Mnemonic Data */
static LPP20WORK  lpWork;        /* Points to Global Area of Work Area     */

/* Select View Data of ListBox V3.3 */
static WORD     awSelectData[P20_TOTAL_MAX] = {
                    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
                   11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
                   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
                   31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                   41,  42,  43,  44,  45,  46,  47,  48,  49,  50,
                   51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
                   61,  62,  63,  64,  65,  66,  67,  68,  69,  70,
                   71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
                   81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
                   91,  92,  93,  94,  95,  96,  97,  98,  99, 100,
                  101,      103, 104, 105, 106, 107, 108, 109, 110,  // transaction mnemonic 102, TRN_WTTL_ADR, is no longer used. See P20_TOTAL_MAX.
                  111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
                  121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
                  131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
/*** NCR2172 ***/
/*                  141, 142, 143 */
                  141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
                  151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
                  161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
                  171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
                  181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
                  191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
				  201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
                  211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
                  221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
                  231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
                  241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
                  251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
                  261, 262, 263, 264, 265, 266, 267, 268, 269, 270,
                  271, 272, 273, 274, 275, 276, 277, 278, 279, 280,
                  281, 282, 283, 284, 285, 286, 287, 288, 289, 290,
                  291, 292, 293, 294, 295, 296, 297, 298, 299, 300,
                  301, 302, 303, 304, 305, 306, 307, 308, 309, 310,
                  311, 312, 313, 314, 315, 316, 317, 318, 319, 320,
                  321, 322, 323, 324, 325, 326, 327, 328, 329, 330,
                  331, 332, 333, 334, 335, 336, 337, 338, 339, 340,
                  341, 342, 343, 344, 345, 346, 347, 348, 349, 350,
                  351, 352, 353, 354, 355, 356, 357, 358, 359, 360,
                  361, 362, 363, 364, 365, 366, 367, 368, 369, 370,
                  371, 372, 373, 374, 375, 376, 377, 378, 379, 380,
                  381, 382, 383, 384, 385, 386, 387, 388, 389, 390,
                  391, 392, 393, 394, 395, 396, 397, 398, 399, 400
                };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P020DlgProc()
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
*                  Transaction Mnemonics (Program Mode # 20)
* ===========================================================================
*/
BOOL    WINAPI  P020DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
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
        hGlbMnemo = GlobalAlloc(GHND, sizeof(P20MNEMO) * P20_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P20WORK)  * P20_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P20DispAllocErr(hDlg, hGlbMnemo, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpMnemonic = (LPP20MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP20WORK)GlobalLock(hGlbWork);

			/* ----- Initialize Configulation of DialogBox ----- */
            P20InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P20_SCBAR; j<=IDD_P20_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P20_EDIT01:
        case IDD_P20_EDIT02:
        case IDD_P20_EDIT03:
        case IDD_P20_EDIT04:
        case IDD_P20_EDIT05:
        case IDD_P20_EDIT06:
        case IDD_P20_EDIT07:
        case IDD_P20_EDIT08:
        case IDD_P20_EDIT09:
        case IDD_P20_EDIT10:
        case IDD_P20_EDIT11:
        case IDD_P20_EDIT12:
        case IDD_P20_EDIT13:
        case IDD_P20_EDIT14:
        case IDD_P20_EDIT15:
        case IDD_P20_EDIT16:
        case IDD_P20_EDIT17:
        case IDD_P20_EDIT18:
        case IDD_P20_EDIT19:
        case IDD_P20_EDIT20:
            if (HIWORD(wParam) == EN_CHANGE) {
                P20GetMnemo(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDOK:
            for (nCount = 0; nCount < P20_ADDR_MAX; nCount++) {
                /* ----- Replace Double Width Key ('~' -> 0x12) ----- */
                PepReplaceMnemonic((WCHAR *)(lpWork + nCount), (WCHAR *)(lpMnemonic + nCount), (P20_MNE_LEN), PEP_MNEMO_WRITE);
            }

            ParaAllWrite(CLASS_PARATRANSMNEMO, (UCHAR *)lpMnemonic, ((P20_MNE_LEN) * 2 * P20_ADDR_MAX), 0, &usReturnLen);

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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P20_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P20ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P20ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        /* ----- Vertical Scroll Bar Control ----- */
        fReturn = P20ScrlProc(hDlg, wParam, lParam);

        /* ----- An application should return zero ----- */
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P20InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P20_SCBAR_MIN;
    unBottomIndex = P20_ONE_PAGE;

    for (wID = IDD_P20_EDIT01; wID <= IDD_P20_EDIT20; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);

        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P20_MNE_LEN, 0L);
    }

    /* ----- Read Mnemonic Data from Parameter File ----- */
    ParaAllRead(CLASS_PARATRANSMNEMO, (UCHAR *)lpMnemonic, (P20_ADDR_MAX * (P20_MNE_LEN) * 2), 0, &usReturnLen);

    for (nCount = 0; nCount < P20_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P20_MNE_LEN ), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P20RedrawText(hDlg);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, P20_SCBAR_MIN, P20_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, P20_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P20_PAGE_MAX;
		scrollInfo.nMin = P20_SCBAR_MIN;
		scrollInfo.nMax = P20_TOTAL_MAX - 1;    // range is zero to max minus 1
		scrollInfo.nPos = P20_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif
    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P20_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P20_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20GetMnemo();
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
VOID    P20GetMnemo(HWND hDlg, WORD wItem)
{
	WCHAR    szWork[P20_MNE_LEN + 1] = {0};

    /* ----- Get Inputed Data from EditText ----- */
    DlgItemGetText(hDlg, wItem, szWork, P20_MNE_LEN + 1);
    
    /* ----- Set Inputed Data to a Data Buffer ----- */
    memcpy((lpWork + (awSelectData[P20GETINDEX(wItem, unTopIndex)] - 1)), szWork, P20_MNE_LEN * 2);    /* V3.3 */
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20DispAllocErr();
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
VOID    P20DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P20_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P20_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P20, szCaption, PEP_STRING_LEN_MAC(szCaption));

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
**  Synopsis:   BOOL    P20ScrlProc();
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
BOOL    P20ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P20CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & HALO Data by New Index ----- */
            P20RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P20_ONE_PAGE;

        /* ----- Redraw Description & HALO Data by New Index ----- */
        P20RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P20CalcIndex();
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
BOOL    P20CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P20_ONE_LINE;
        if (unBottomIndex < P20_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P20_ONE_PAGE;
            unTopIndex    = P20_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P20_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P20_ONE_LINE;
        if (unBottomIndex >= P20_TOTAL_MAX) {    /* Scroll to Bottom V3.3 */
            unBottomIndex = (P20_TOTAL_MAX - 1); /* V3.3 */
            unTopIndex    = P20_SCBAR_MAX;

            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P20_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P20_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= P20_ONE_PAGE;
            if (unBottomIndex <= P20_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P20_ONE_PAGE;
                unTopIndex    = P20_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P20_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P20_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P20_ONE_PAGE;
            if (unBottomIndex >= P20_TOTAL_MAX) {    /* Scroll to Bottom  V3.3 */
                unBottomIndex = (P20_TOTAL_MAX - 1); /* V3.3 */
                unTopIndex    = P20_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P20_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P20RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
		WORD     wID = (WORD)(IDD_P20_EDIT01 + unLoop);
		WCHAR	 szTempBuff[256] = {0};        // temporary buffer to load string into. JHHJ 10-15-2004
		WCHAR    szWork[P20_DESC_LEN] = {0};

        /* ----- Load Strings from Resource ----- */
        LoadString(hResourceDll, IDS_P020DESC001 + awSelectData[unCount] - 1, szTempBuff, P20_DESC_LEN);

		//copy only the size that will fit into the buffer that is allotted, this 
		//prevents overspill from reading into buffers.
		wcsncpy(szWork, szTempBuff, P20_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */
		DlgItemRedrawText(hDlg, IDD_P20_DESC01 + unLoop, szWork);

        /* ----- Set Mnemonic Data to EditText ----- */
        DlgItemRedrawText(hDlg, wID,(WCHAR *)(lpWork + (awSelectData[P20GETINDEX(wID, unTopIndex)] - 1))); /* V3.3 */
    }
}

/* ===== End of P020.C ===== */
