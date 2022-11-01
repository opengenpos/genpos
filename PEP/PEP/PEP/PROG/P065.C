/*
* ---------------------------------------------------------------------------
* Title         :   Multi-Line Display Mnemonics (Prog. 65)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P065.C
* Copyright (C) :   1998, NCR Corp. E&M-OISO, All rights reserved.
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
* Sep-09-98 : 03.03.00 : A.Mitsui   : Initial
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
#include    "csop.h"

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "P065.h"

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

static LPP65MLDWORK lpMLDWork;   /* Points of Global Area of Work Data     */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P065DlgProc()
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
*                  Multi-Line Display Mnemonics (Program Mode # 65)
* ===========================================================================
*/
BOOL    WINAPI  P065DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static HGLOBAL hGlbWork;            /* Handle of Global Area (Work) */
    
    MLDIF   MLDData;
    BOOL    fReturn;                    /* Return Value of Local */
    short   nCount;
    SHORT   sStat;                      /* Return Value of OpMLDAssign */

    WCHAR    szWork[P65_ERR_LEN];
    WCHAR    szError[P65_ERR_LEN];

    switch (wMsg) {

    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Grobal Area for MLD Data ----- */
        hGlbWork  = GlobalAlloc(GHND, sizeof(P65MLDWORK) * 2);

        if (!hGlbWork){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P65DispAllocErr(hDlg, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpMLDWork = (LPP65MLDWORK)GlobalLock(hGlbWork);

            /* ----- Load MLD File Name from Resource----- */
            LoadString (hResourceDll, IDS_FILE_NAME_MLD, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Create Backup File for Cancel Key ----- */
            PepFileBackup(hDlg, szWork, PEP_FILE_BACKUP);

            if (P65InitDlg(hDlg)) {
                /* configration error */
                /* destroy dialog box */
                EndDialog(hDlg, TRUE);
            }
        }
		SendMessage (hDlg, WM_SETFONT, 0, 0);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P65_DESC01; j<=IDD_P65_MNEM; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
			case IDD_P65_EDIT01:
			case IDD_P65_EDIT02:
			case IDD_P65_EDIT03:
			case IDD_P65_EDIT04:
			case IDD_P65_EDIT05:
			case IDD_P65_EDIT06:
			case IDD_P65_EDIT07:
			case IDD_P65_EDIT08:
			case IDD_P65_EDIT09:
			case IDD_P65_EDIT10:
			   if (HIWORD(wParam) == EN_CHANGE) {
					P65GetMnemo(hDlg, LOWORD(wParam));
					return TRUE;
				}
				return FALSE;

			case IDOK:
				for (nCount = 0; nCount < P65_ADDR_MAX; nCount++) {
					/* ----- Clear MLDData  ----- */
					memset(&MLDData, 0, sizeof(MLDData));

					/* ----- set MLD Address ----- */
					MLDData.uchAddress = (lpMLDWork->MLDWork[nCount]).uchAddress;

					/* ----- Replace Double Width Key ('~' -> 0x12) ----- */
					PepReplaceMnemonic((WCHAR *)((lpMLDWork->MLDWork[nCount]).aszMnemonics),
									   (WCHAR *)(MLDData.aszMnemonics), (OP_MLD_MNEMONICS_LEN ), PEP_MNEMO_WRITE);

					/* ----- Write Mnemonics to MLD File ----- */
					if ((sStat = OpMldAssign(&MLDData, 0)) != OP_PERFORM) {
						/* ----- Make Error description ----- */
						LoadString(hResourceDll, IDS_P065_ERRUNKNOWN, szWork, PEP_STRING_LEN_MAC(szWork));
						wsPepf(szError, szWork, sStat);

						/* ----- Display Caution Message ----- */            
						DispError(hDlg, szError);

						return(TRUE);
					}
				}

				PepSetModFlag(hwndProgMain, PEP_MF_MLD, 0);

				/* ----- Delete Backup File ----- */
				PepFileDelete();

				/* ----- Unlock Global Allocated Area & Set Them Free ----- */
				GlobalUnlock(hGlbWork);
				GlobalFree(hGlbWork);

				EndDialog(hDlg, FALSE);
				return TRUE;

			case IDCANCEL:
				/* ----- Load MLD File Name from Resource----- */
				LoadString (hResourceDll, IDS_FILE_NAME_MLD, szWork, PEP_STRING_LEN_MAC(szWork));

				/* ----- Restore Cashier File with Backup File ----- */
				PepFileBackup(hDlg, szWork, PEP_FILE_RESTORE);

				/* ----- Delete Backup File ----- */
				PepFileDelete();

				/* ----- Unlock Global Allocated Area & Set Them Free ----- */
				GlobalUnlock(hGlbWork);
				GlobalFree(hGlbWork);

				EndDialog(hDlg, FALSE);
				return TRUE;
			}
			return FALSE;

    case WM_MOUSEWHEEL:
		{
			int     dWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P65_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P65ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				fReturn = P65ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

		case WM_VSCROLL:
			/* ----- Vertical Scroll Bar Control ----- */
			SendMessage (hDlg, WM_SETFONT, 0, 0);
			fReturn = P65ScrlProc(hDlg, wParam, lParam);

			/* ----- An application should return zero ----- */
			return FALSE;

		default:
			return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P65InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   TRUE                - error occured
*               FALSE               - user process is executed
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
BOOL    P65InitDlg(HWND hDlg)
{
    SHORT   sStat;                      /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    MLDIF   MLDData;
    WCHAR    szError[P65_ERR_LEN];
    WCHAR    szWork[P65_ERR_LEN];

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P65_SCBAR_MIN;
    unBottomIndex = P65_ONE_PAGE;

    for (wID = IDD_P65_EDIT01; wID <= IDD_P65_EDIT10; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P65_MNE_LEN, 0L);
    }

    /* ----- Read Initial MLD Data from MLD File ----- */
    for (nCount = 0; nCount < P65_ADDR_MAX; nCount++) {

        /* ----- Clear MLDData  ----- */
        memset(&MLDData, 0, sizeof(MLDData));

        /* ----- Set MLD Address ----- */
        MLDData.uchAddress = (UCHAR)(nCount + 1);

        /* ----- Read default MLD Mnemonic from MLD File ----- */
        if ((sStat = OpMldIndRead(&MLDData, 0)) != OP_PERFORM) {
            /* ----- Make Error description ----- */
            LoadString(hResourceDll, IDS_P065_ERRUNKNOWN, szWork, PEP_STRING_LEN_MAC(szWork));
            wsPepf(szError, szWork, sStat);

            /* ----- Display Caution Message ----- */            
            DispError(hDlg, szError);
            return(TRUE);
        }

        /* ----- Replace Double Width Key (0x12 -> '~') to Work Area ----- */
        (lpMLDWork->MLDWork[nCount]).uchAddress = (UCHAR)(nCount + 1);

        PepReplaceMnemonic((WCHAR *)(MLDData.aszMnemonics), (WCHAR *)((lpMLDWork->MLDWork[nCount]).aszMnemonics),
                           (OP_MLD_MNEMONICS_LEN), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P65RedrawText(hDlg);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P65_SCBAR), SB_CTL, P65_SCBAR_MIN, P65_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P65_SCBAR), SB_CTL, P65_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P65_PAGE_MAX;
		scrollInfo.nMin = P65_SCBAR_MIN;
		scrollInfo.nMax = MAX_MLDMNEMO_SIZE - 1;    // scroll range is zero based. this TOTAL_MAX is different from other mnemonic dialogs.
		scrollInfo.nPos = P65_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P65_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P65_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P65_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
    
    return(FALSE);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P65GetMnemo();
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
VOID    P65GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[P65_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */
    memset(szWork, 0, sizeof(szWork));
    DlgItemGetText(hDlg, wItem, szWork, P65_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */
    memset(((lpMLDWork->MLDWork[P65GETINDEX(wItem, unTopIndex)]).aszMnemonics), 0, P65_MNE_LEN * 2);

    /* ----- Set Inputed Data to a Data Buffer ----- */
    wcscpy(((lpMLDWork->MLDWork[P65GETINDEX(wItem, unTopIndex)]).aszMnemonics), szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P65DispAllocErr();
*
**  Input   :   HWND    hDlg      - Handle of a DialogBox Procedure
*               HGLOBAL hGlbWork  - Handle of a Global Area (Work)
*
**  Return  :   No return value.
*
**  Description:
*       This function shows a MessageBox for Error Message. And reset global
*     heap area, if allocation was success.
* ===========================================================================
*/
VOID    P65DispAllocErr(HWND hDlg, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P65_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P65_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P65, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, PEP_STRING_LEN_MAC(szMessage));
    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Reset Allocated Area ----- */
    if (hGlbWork) {
        GlobalFree(hGlbWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P65ScrlProc();
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
BOOL    P65ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P65CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & Mnemonic Data by New Index ----- */
            P65RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P65_ONE_PAGE;

        /* ----- Redraw Description & Mnemonic Data by New Index ----- */
        P65RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P65CalcIndex();
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
BOOL    P65CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P65_ONE_LINE;

        if (unBottomIndex < P65_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P65_ONE_PAGE;
            unTopIndex    = P65_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P65_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P65_ONE_LINE;

        if (unBottomIndex >= P65_ADDR_NO) {    /* Scroll to Bottom */
            unBottomIndex = (P65_ADDR_NO - 1);
            unTopIndex    = P65_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P65_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P65_SCBAR_MIN) {  /* Current Position isn't Top */
            unBottomIndex -= P65_ONE_PAGE;

            if (unBottomIndex <= P65_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P65_ONE_PAGE;
                unTopIndex    = P65_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P65_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P65_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P65_ONE_PAGE;
            if (unBottomIndex >= P65_ADDR_NO) {    /* Scroll to Bottom */
                unBottomIndex = (P65_ADDR_NO - 1);
                unTopIndex    = P65_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P65_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P65RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P65RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P65_DESC_LEN + 1];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
		LoadString(hResourceDll, (IDS_P065DESC001 + unCount), szWork, PEP_STRING_LEN_MAC(szWork));
		DlgItemRedrawText(hDlg, (IDD_P65_DESC01 + unLoop), szWork);

        /* ----- Set Mnemonic Data to EditText ----- */
        wID = (WORD)(IDD_P65_EDIT01 + unLoop);

        //SetDlgItemText(hDlg,wID,((lpMLDWork->MLDWork[P65GETINDEX(wID, unTopIndex)]).aszMnemonics));
		DlgItemRedrawText(hDlg,wID,((lpMLDWork->MLDWork[P65GETINDEX(wID, unTopIndex)]).aszMnemonics));
    }
}

/*
* ===========================================================================
**  Synopsis:   void    DispError();
*
**  Input   :   HWND    hDlg        -   Handle of DialogBox
*               LPCSTR  lpszErrMsg  -   Points to Error Message
*
**  Return  :   No return value
*
**  Description:
*       This function displays error message with MessageBox.
* ===========================================================================
*/
void    DispError(HWND hDlg, LPCWSTR lpszErrMsg)
{
    WCHAR    szCaption[PEP_CAPTION_LEN];

    /* ----- Load Strings from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P65, szCaption, PEP_CAPTION_LEN);

    /* ----- Popup MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, lpszErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);
}
/* ===== End of P065.C ===== */
