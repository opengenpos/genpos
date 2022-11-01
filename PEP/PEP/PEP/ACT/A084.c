/*
* ---------------------------------------------------------------------------
* Title         :   Set Rounding Table (AC 84)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A084.C
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
* Sep-14-98 : 03.03.00 : A.Mitsui   : V3.3
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

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a084.h"

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
**  Synopsis    :   BOOL    WINAPI  A084DlgProc()
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
*                       Set Rounding Table (Action Code 84).
* ===========================================================================
*/
BOOL    WINAPI  A084DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  BYTE    abRndData[A84_ADDR_MAX][A84_ADDR_SIZE];

    USHORT  usReturn;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        InitDlg(hDlg, (LPBYTE)abRndData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A84_DESC01; j<=IDD_A84_RANGE; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A84_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				ScrollProc(hDlg, SB_LINEDOWN, hScroll, (LPBYTE)abRndData);
			} else if (dWheel > 0) {
				ScrollProc(hDlg, SB_LINEUP, hScroll, (LPBYTE)abRndData);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_A84_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            ScrollProc(hDlg, wParam, lParam, (LPBYTE)abRndData);

        } else {

            /* ----- Spin Button Control ----- */

            SpinProc(hDlg, wParam, lParam, (LPBYTE)abRndData);
        }

        /* ----- An Application Should Return Zero ----- */

        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A84_DMT01:
        case IDD_A84_DMT02:
        case IDD_A84_DMT03:
        case IDD_A84_DMT04:
        case IDD_A84_DMT05:
        case IDD_A84_DMT06:
        case IDD_A84_DMT07:
        case IDD_A84_DMT08:
        case IDD_A84_DMT09:
        case IDD_A84_DMT10:
        case IDD_A84_MOD01:
        case IDD_A84_MOD02:
        case IDD_A84_MOD03:
        case IDD_A84_MOD04:
        case IDD_A84_MOD05:
        case IDD_A84_MOD06:
        case IDD_A84_MOD07:
        case IDD_A84_MOD08:
        case IDD_A84_MOD09:
        case IDD_A84_MOD10:

            if (HIWORD(wParam) == EN_CHANGE) {

                SetData(hDlg, wParam, (LPBYTE)abRndData);

                return TRUE;
            }
            return FALSE;

        case IDD_A84_RND01:
        case IDD_A84_RND02:
        case IDD_A84_RND03:
        case IDD_A84_RND04:
        case IDD_A84_RND05:
        case IDD_A84_RND06:
        case IDD_A84_RND07:
        case IDD_A84_RND08:
        case IDD_A84_RND09:
        case IDD_A84_RND10:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (GetRndData(hDlg, wParam, (LPBYTE)abRndData)) {

                    /* ----- Text out Error Message ----- */

                    ShowError(hDlg);

                    /* ----- Reset Previous Data to Data Buffer ----- */

                    ResetRndData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARAROUNDTBL,
                                 (UCHAR *)abRndData,
                                 sizeof(abRndData),
                                 0, &usReturn);

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
**  Synopsis    :   static  void    InitDlg()
*
**  Input       :   HWND   hDlg     -   Window Handle of a DialogBox
*                   LPBYTE lpabRnd  -   Address of Rounding Table Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This function loads a initial rounding data from the Parameter file,
*   and initializes the configulation of a DialogBox.
* ===========================================================================
*/
static  void    InitDlg(HWND hDlg, LPBYTE lpabRnd)
{
    USHORT  usReturn;
    WORD    wI;
    WCHAR        szDesc[128];

    /* ----- set initiali description ----- */
/***
    LoadString(hActCommInst, IDS_PEP_CAPTION_A84, szDesc, PEP_STRING_LEN_MAC(szDesc));
    SetWindowText(hDlg, szDesc);
***/
    LoadString(hResourceDll, IDS_A84_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A84_DESC, szDesc);
    LoadString(hResourceDll, IDS_A84_RANGE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A84_RANGE, szDesc);

    /* ----- Load Initial Rounding Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAROUNDTBL,
                (UCHAR *)lpabRnd,
                (A84_ADDR_MAX * A84_ADDR_SIZE),
                0, &usReturn);

    for (wI = 0; wI < A84_MAX_PAGE; wI++) {

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_A84_DMT01 + wI));
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_A84_MOD01 + wI));
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_A84_RND01 + wI));

        PepSetFont(hDlg, wI);

        /* ----- Limit Maximum Length of Input Data to Each EditText ----- */

        SendDlgItemMessage(hDlg, IDD_A84_DMT01 + wI,
                           EM_LIMITTEXT, A84_DATA_LEN, 0L);

        SendDlgItemMessage(hDlg, IDD_A84_MOD01 + wI,
                           EM_LIMITTEXT, A84_DATA_LEN, 0L);

        SendDlgItemMessage(hDlg, IDD_A84_RND01 + wI,
                           EM_LIMITTEXT, A84_DATA_LEN, 0L);
    }

    /* ----- Set Top Index & Bottom Index of Scroll Bar ----- */

    unTopIndex    = A84_SCBAR_MIN;
    unBottomIndex = A84_ONE_PAGE;

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */

    SetScrollRange(GetDlgItem(hDlg, IDD_A84_SCBAR), SB_CTL,
                   A84_SCBAR_MIN, A84_SCBAR_MAX, TRUE);

    /* ----- Set Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A84_SCBAR), SB_CTL,
                 A84_SCBAR_MIN, FALSE);

    /* ----- Set Description & Raounding Data to Each Controls ----- */

    RedrawText(hDlg, lpabRnd);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A84_MOD01));
    SendDlgItemMessage(hDlg, IDD_A84_MOD01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   static  void    RedrawText()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPBYTE  lpabRnd -   Address of Rounding Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This function displays description and rounding table data to
*   each controls.
* ===========================================================================
*/
static  void    RedrawText(HWND hDlg, LPBYTE lpabRnd)
{
    LPBYTE  lpRndData;
    WCHAR    szWork[128];
    short   nRndData;
    UINT    unI,
            unLoop = 0;

    /* ----- Set Address of Top Index to Temporary Area ----- */

    lpRndData = lpabRnd + (BYTE)(unTopIndex * A84_ADDR_SIZE);

    for (unI = unTopIndex; unI <= unBottomIndex; unI++, unLoop++) {

        /* ----- Load Strings from Resource ----- */

        LoadString(hResourceDll, IDS_A84_DESC01 + unI, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_A84_DESC01 + unLoop, szWork);

        /* ----- Set Rounding Data to EditText ----- */

        SetDlgItemInt(hDlg, IDD_A84_DMT01 + unLoop,
                      (UINT)*(lpRndData + A84_DMT_ADDR), FALSE);

        SetDlgItemInt(hDlg, IDD_A84_MOD01 + unLoop, 
                      (UINT)*(lpRndData + A84_MOD_ADDR), FALSE);

        nRndData = A84_GETRNDPOSI(*(lpRndData + A84_RND_ADDR));

        SetDlgItemInt(hDlg, IDD_A84_RND01 + unLoop, (UINT)nRndData, TRUE);

        lpRndData += A84_ADDR_SIZE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   static  void    SetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Current Inputed EditText ID
*                   LPBYTE  lpabRnd -   Address of Rounding Table Data Buffer
*
**  Return      :   BOOL    TRUE    -   Inputed Data is out of range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed modules and delimiter data from EditText.
*   And it checks whether inputed data is out of range.
*       It returns TRUE, if inputed data is over range. Otherwise it
*   returns FALSE.
* ===========================================================================
*/
static  void    SetData(HWND hDlg, WPARAM wParam, LPBYTE lpabRnd)
{
    short   nValue;             /* Value of Inputed Data            */
    BYTE    bAddr,
            bField;

    /* ----- Get Inputed Data from EditText ----- */

    nValue = (short)GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

    /* ----- Set Inputed Data to Data Buffer ----- */

    bField = (BYTE)((IDD_A84_MOD01 <= LOWORD(wParam)) ? A84_MOD_ADDR:
                                                A84_DMT_ADDR);

    bAddr = (BYTE)((IDD_A84_MOD01 <= LOWORD(wParam)) ? A84GETMODADR(LOWORD(wParam), unTopIndex):
                                               A84GETDMTADR(LOWORD(wParam), unTopIndex));

    *(lpabRnd + (bAddr * A84_ADDR_SIZE) + bField) = (BYTE)nValue;

}

/*
* ===========================================================================
**  Synopsis    :   static  BOOL    GetRndData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Current Inputed EditText ID
*                   LPBYTE  lpabRnd -   Address of Rounding Table Data Buffer
*
**  Return      :   BOOL    TRUE    -   Inputed Data is out of range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed rounding data from EditText.
*   And it checks whether inputed data is out of range.
*       It returns TRUE, if inputed data is over range. Otherwise it
*   returns FALSE.
* ===========================================================================
*/
static  BOOL    GetRndData(HWND hDlg, WPARAM wParam, LPBYTE lpabRnd)
{
    short   nValue;             /* Value of Inputed Data            */
    BYTE    bAddr;              /* Address offset from lpabRnd      */
    BOOL    fReturn = FALSE;    /* Return Value of This Function    */

    /* ----- Get Inputed Data from EditText ----- */

    nValue = (short)GetDlgItemInt(hDlg, LOWORD(wParam), NULL, TRUE);

    /* ----- Determine Whether Inputed Data is Out of Range or Not ----- */

    if ((nValue < A84_RND_MIN) || (nValue > A84_RND_MAX)) {

        fReturn = TRUE;

    } else {

        /* ----- Set Inputed Data to Data Buffer ----- */

        bAddr = (BYTE)(LOWORD(wParam) - IDD_A84_RND01);

        *(lpabRnd + (bAddr * A84_ADDR_SIZE)
                  + (unTopIndex * A84_ADDR_SIZE) + A84_RND_ADDR)
        = (BYTE)nValue;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   static  void    ShowError()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for Error Message.
* ===========================================================================
*/
static  void    ShowError(HWND hDlg)
{
    WCHAR    szCaption[PEP_CAPTION_LEN], /* MessageBox Caption       */
            szMessage[PEP_OVER_LEN],    /* Error Message Strings    */
            szWork[PEP_OVER_LEN];       /* Error Message Work Area  */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A84, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, A84_RND_MIN, A84_RND_MAX);

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   static  void    ResetRndData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Target EditText ID
*
**  Return      :   No return value
*
**  Description :
*       This function undoes data input to EditText.
* ===========================================================================
*/
static  void    ResetRndData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

	//SR 424, we prevent the user from having 2 characters in the editbox when
	//we reset.
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   static  void    SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpabRnd -   Address of Rounding Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This function is SpinButton Procedure.
* ===========================================================================
*/
static  void    SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpabRnd)
{
    BOOL    fReturn = FALSE;
    int     idEdit = 0;
    PEPSPIN PepSpin;

    /* ----- Get Linked EditText ID with SpinButton ID ----- */

    idEdit = GetDlgCtrlID((HWND)lParam) - A84_SPIN_OFFSET;

    /* ----- Set Common Configulation of SpinButton ----- */

    PepSpin.nStep      = A84_SPIN_STEP;
    PepSpin.nTurbo     = A84_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set Individual Configulation of SpinButton ----- */

    if ((IDD_A84_RND01 <= idEdit) && (idEdit <= IDD_A84_RND10)) {

        PepSpin.lMin = (long)A84_RND_MIN;
        PepSpin.lMax = (long)A84_RND_MAX;

    } else {

        PepSpin.lMin = 0L;
        PepSpin.lMax = (long)A84_DMT_MAX;
    }

    fReturn = PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

    if (fReturn == TRUE) {

        if ((IDD_A84_RND01 <= idEdit) && (idEdit <= IDD_A84_RND10)) {

            GetRndData(hDlg, (WORD)idEdit, lpabRnd);

        } else {

            SetData(hDlg, (WORD)idEdit, lpabRnd);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   static  void    ScrollProc()
*
**  Input       :   HWND    hDlg    -   Handle of a DialogBox Procedure
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpabRnd -   Points to a Buffer of HALO Data
*
**  Return      :   No return value.
*
**  Description :
*       This function is Vertical Scroll Procedure.
* ===========================================================================
*/
static  void    ScrollProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpabRnd)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & HALO Data by New Index ----- */

            RedrawText(hDlg, lpabRnd);

            /* ----- Redraw Thumb to New Position ----- */

            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A84_ONE_PAGE;

        /* ----- Redraw Description & HALO Data by New Index ----- */

        RedrawText(hDlg, lpabRnd);

        /* ----- Redraw Thumb to New Position ----- */

        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   static  BOOL    CalcIndex()
*
**  Input       :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return      :   BOOL    TRUE    -   Scroll up to Top or Bottom of ListBox
*                           FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description :
*       This function computes top index and bottom index.
* ===========================================================================
*/
static  BOOL    CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:

        unBottomIndex -= A84_ONE_LINE;

        if (unBottomIndex < A84_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = A84_ONE_PAGE;
            unTopIndex    = A84_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= A84_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:

        unBottomIndex += A84_ONE_LINE;

        if (unBottomIndex >= A84_ADDR_MAX) {    /* Scroll to Bottom */

            unBottomIndex = (A84_ADDR_MAX - 1);
            unTopIndex    = A84_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += A84_ONE_LINE;
        }
        break;

    case SB_PAGEUP:

        if (unTopIndex != A84_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= A84_ONE_PAGE;

            if (unBottomIndex <= A84_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = A84_ONE_PAGE;
                unTopIndex    = A84_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= A84_ONE_PAGE;
            }

        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:

        if (unTopIndex != A84_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += A84_ONE_PAGE;

            if (unBottomIndex >= A84_ADDR_MAX) {    /* Scroll to Bottom */

                unBottomIndex = (A84_ADDR_MAX - 1);
                unTopIndex    = A84_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += A84_ONE_PAGE;
            }

        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/* ===== End of A084.C ===== */
