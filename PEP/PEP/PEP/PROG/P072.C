/*
* ---------------------------------------------------------------------------
* Title         :   Multi-Line Display Mnemonics (Prog. 65)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P072.C
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
#include    "P072.h"

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

static LPP72RSNWORK lpRSNWork;   /* Points of Global Area of Work Data     */

static 	USHORT  P72ParaReasonCode[MAX_REASON_CODE_ITEMS][MAX_REASON_CODE_ENTRIES];

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   void    P72DispError();
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
void    P72DispError(HWND hDlg, LPCWSTR lpszErrMsg)
{
    WCHAR    szCaption[PEP_CAPTION_LEN];

    /* ----- Load Strings from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P72, szCaption, PEP_CAPTION_LEN);

    /* ----- Popup MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, lpszErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);
}
/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P072DlgProc()
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
struct {
	ULONG  ulSpinControlId;
	ULONG  ulEditControl;
	ULONG  ulType;          // 0x0001 -> Start, 0x0002 -> Range
} ListOfSpinControls[] = {
	{IDD_P72_SPIN01_1, IDD_P72_01_1, 0x0001},
	{IDD_P72_SPIN01_2, IDD_P72_01_2, 0x0002},
	{IDD_P72_SPIN02_1, IDD_P72_02_1, 0x0001},
	{IDD_P72_SPIN02_2, IDD_P72_02_2, 0x0002},
	{IDD_P72_SPIN03_1, IDD_P72_03_1, 0x0001},
	{IDD_P72_SPIN03_2, IDD_P72_03_2, 0x0002},
	{IDD_P72_SPIN04_1, IDD_P72_04_1, 0x0001},
	{IDD_P72_SPIN04_2, IDD_P72_04_2, 0x0002},
	{IDD_P72_SPIN05_1, IDD_P72_05_1, 0x0001},
	{IDD_P72_SPIN05_2, IDD_P72_05_2, 0x0002},
	{IDD_P72_SPIN06_1, IDD_P72_06_1, 0x0001},
	{IDD_P72_SPIN06_2, IDD_P72_06_2, 0x0002}
};

BOOL    WINAPI  P072DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static HGLOBAL hGlbWork;            /* Handle of Global Area (Work) */
    
    OPMNEMONICFILE   RSNData;
    BOOL    fReturn;                    /* Return Value of Local */
    short   nCount;
    SHORT   sStat;                      /* Return Value of OpMLDAssign */
	USHORT   usReturnLen;
    WCHAR    szWork[P72_ERR_LEN];
    WCHAR    szError[P72_ERR_LEN];

    switch (wMsg) {

    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		ParaAllRead(CLASS_PARAREASONCODE, &P72ParaReasonCode, sizeof(P72ParaReasonCode), 0, (USHORT *)&usReturnLen);

        /* ----- Allocate Grobal Area for MLD Data ----- */
        hGlbWork  = GlobalAlloc(GHND, sizeof(P72RSNWORK) * 2);

        if (!hGlbWork){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P72DispAllocErr(hDlg, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpRSNWork = (LPP72RSNWORK)GlobalLock(hGlbWork);

            /* ----- Load MLD File Name from Resource----- */
            LoadString (hResourceDll, IDS_FILE_NAME_RSN, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Create Backup File for Cancel Key ----- */
            PepFileBackup(hDlg, szWork, PEP_FILE_BACKUP);

            if (P72InitDlg(hDlg)) {
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
			for(j=IDD_P72_DESC01; j<=IDD_P72_MNEM; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
			case IDD_P72_EDIT01:
			case IDD_P72_EDIT02:
			case IDD_P72_EDIT03:
			case IDD_P72_EDIT04:
			case IDD_P72_EDIT05:
			case IDD_P72_EDIT06:
			case IDD_P72_EDIT07:
			case IDD_P72_EDIT08:
			case IDD_P72_EDIT09:
			case IDD_P72_EDIT10:
			   if (HIWORD(wParam) == EN_CHANGE) {
					P72GetMnemo(hDlg, LOWORD(wParam));
					return TRUE;
				}
				return FALSE;

			case IDOK:
				/* ----- Clear RSNData  ----- */
				memset(&RSNData, 0, sizeof(RSNData));
				RSNData.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;

				P72ParaReasonCode[0][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_01_1, NULL, FALSE);
				P72ParaReasonCode[0][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_01_2, NULL, FALSE);
				P72ParaReasonCode[1][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_02_1, NULL, FALSE);
				P72ParaReasonCode[1][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_02_2, NULL, FALSE);
				P72ParaReasonCode[2][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_03_1, NULL, FALSE);
				P72ParaReasonCode[2][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_03_2, NULL, FALSE);
				P72ParaReasonCode[3][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_04_1, NULL, FALSE);
				P72ParaReasonCode[3][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_04_2, NULL, FALSE);
				P72ParaReasonCode[4][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_05_1, NULL, FALSE);
				P72ParaReasonCode[4][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_05_2, NULL, FALSE);
				P72ParaReasonCode[5][0] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_06_1, NULL, FALSE);
				P72ParaReasonCode[5][1] = (DWORD)GetDlgItemInt(hDlg, IDD_P72_06_2, NULL, FALSE);

				ParaAllWrite(CLASS_PARAREASONCODE, &P72ParaReasonCode, sizeof(P72ParaReasonCode), 0, (USHORT *)&usReturnLen);

				for (nCount = 0; nCount < P72_ADDR_MAX; nCount++) {
					/* ----- set MLD Address ----- */
					RSNData.usMnemonicAddress = (lpRSNWork->RSNWork[nCount]).usMnemonicAddress;

					/* ----- Replace Double Width Key ('~' -> 0x12) ----- */
					PepReplaceMnemonic((WCHAR *)((lpRSNWork->RSNWork[nCount]).aszMnemonicValue), (WCHAR *)(RSNData.aszMnemonicValue), (OP_MLD_MNEMONICS_LEN ), PEP_MNEMO_WRITE);

					/* ----- Write Mnemonics to MLD File ----- */
					if ((sStat = OpMnemonicsFileAssign(&RSNData, 0)) != OP_PERFORM) {
						/* ----- Make Error description ----- */
						LoadString(hResourceDll, IDS_P072_ERRUNKNOWN, szWork, PEP_STRING_LEN_MAC(szWork));
						wsPepf(szError, szWork, sStat);

						/* ----- Display Caution Message ----- */            
						P72DispError(hDlg, szError);

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
				LoadString (hResourceDll, IDS_FILE_NAME_RSN, szWork, PEP_STRING_LEN_MAC(szWork));

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
				LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P72_SCBAR);

				if (dWheel < 0) {
					/* ----- Vertical Scroll Bar Control ----- */
					fReturn = P72ScrlProc(hDlg, SB_LINEDOWN, hScroll);
				} else if (dWheel > 0) {
					fReturn = P72ScrlProc(hDlg, SB_LINEUP, hScroll);
				}
			}

			/* ----- An application should return zero ----- */
			return FALSE;

		case WM_VSCROLL:
			if (GetDlgCtrlID((HWND)lParam) == IDD_P72_SCBAR) {
				/* ----- Vertical Scroll Bar Control ----- */
				SendMessage (hDlg, WM_SETFONT, 0, 0);
				fReturn = P72ScrlProc(hDlg, wParam, lParam);
			} else {
				ULONG   sourceId = GetDlgCtrlID((HWND)lParam);
				USHORT  k = 0;
				for (k = 0; k < sizeof(ListOfSpinControls)/sizeof(ListOfSpinControls[0]); k++) {
					if (sourceId == ListOfSpinControls[k].ulSpinControlId) {
						UINT  uiVal = GetDlgItemInt (hDlg, ListOfSpinControls[k].ulEditControl, NULL, FALSE);
						switch (LOWORD(wParam)) {
							case SB_LINEDOWN:
							case SB_PAGEDOWN:
								(uiVal > 0) ? uiVal-- : 0;
								SetDlgItemInt (hDlg, ListOfSpinControls[k].ulEditControl, uiVal, FALSE);
								break;
							case SB_LINEUP:
							case SB_PAGEUP:
								(uiVal < P72_ADDR_MAX) ? uiVal++ : 0;
								SetDlgItemInt (hDlg, ListOfSpinControls[k].ulEditControl, uiVal, FALSE);
								break;
						}
					}
				}
			}

			/* ----- An application should return zero ----- */
			return FALSE;

		default:
			return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P72InitDlg();
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
BOOL    P72InitDlg(HWND hDlg)
{
    SHORT   sStat;                      /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    OPMNEMONICFILE   RSNData;
    WCHAR    szError[P72_ERR_LEN];
    WCHAR    szWork[P72_ERR_LEN];

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P72_SCBAR_MIN;
    unBottomIndex = P72_ONE_PAGE;

    for (wID = IDD_P72_EDIT01; wID <= IDD_P72_EDIT10; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P72_MNE_LEN, 0L);
    }

	SetDlgItemInt(hDlg, IDD_P72_01_1, P72ParaReasonCode[0][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_01_2, P72ParaReasonCode[0][1], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_02_1, P72ParaReasonCode[1][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_02_2, P72ParaReasonCode[1][1], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_03_1, P72ParaReasonCode[2][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_03_2, P72ParaReasonCode[2][1], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_04_1, P72ParaReasonCode[3][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_04_2, P72ParaReasonCode[3][1], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_05_1, P72ParaReasonCode[4][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_05_2, P72ParaReasonCode[4][1], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_06_1, P72ParaReasonCode[5][0], FALSE);
	SetDlgItemInt(hDlg, IDD_P72_06_2, P72ParaReasonCode[5][1], FALSE);

    memset(&RSNData, 0, sizeof(RSNData));
	RSNData.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;

    /* ----- Read Initial MLD Data from MLD File ----- */
    for (nCount = 0; nCount < P72_ADDR_MAX; nCount++) {
        /* ----- Set MLD Address ----- */
		RSNData.usMnemonicAddress = (nCount + 1);

        /* ----- Read default MLD Mnemonic from MLD File ----- */
        if ((sStat = OpMnemonicsFileIndRead(&RSNData, 0)) != OP_PERFORM) {
            /* ----- Make Error description ----- */
            LoadString(hResourceDll, IDS_P072_ERRUNKNOWN, szWork, PEP_STRING_LEN_MAC(szWork));
            wsPepf(szError, szWork, sStat);

            /* ----- Display Caution Message ----- */            
            P72DispError(hDlg, szError);
            return(TRUE);
        }

        /* ----- Replace Double Width Key (0x12 -> '~') to Work Area ----- */
        (lpRSNWork->RSNWork[nCount]).usMnemonicAddress = (nCount + 1);

		PepReplaceMnemonic((WCHAR *)(RSNData.aszMnemonicValue), (WCHAR *)((lpRSNWork->RSNWork[nCount]).aszMnemonicValue), (OP_MLD_MNEMONICS_LEN), PEP_MNEMO_READ);
    }

    /* ----- Set Description to StaticText ----- */
    P72RedrawText(hDlg);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P72_SCBAR), SB_CTL, P72_SCBAR_MIN, P72_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P72_SCBAR), SB_CTL, P72_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P72_PAGE_MAX;
		scrollInfo.nMin = P72_SCBAR_MIN;
		scrollInfo.nMax = MAX_RSNMNEMO_SIZE - 1;    // scroll range is zero based. this TOTAL_MAX is different from other mnemonic dialogs.
		scrollInfo.nPos = P72_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P72_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P72_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P72_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
    
    return(FALSE);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P72GetMnemo();
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
VOID    P72GetMnemo(HWND hDlg, WORD wItem)
{
    WCHAR    szWork[P72_MNE_LEN + 1];

    /* ----- Get Inputed Data from EditText ----- */
    memset(szWork, 0, sizeof(szWork));
    DlgItemGetText(hDlg, wItem, szWork, P72_MNE_LEN + 1);

    /* ----- Reset Data Buffer ----- */
    memset(((lpRSNWork->RSNWork[P72GETINDEX(wItem, unTopIndex)]).aszMnemonicValue), 0, P72_MNE_LEN * 2);

    /* ----- Set Inputed Data to a Data Buffer ----- */
    wcscpy(((lpRSNWork->RSNWork[P72GETINDEX(wItem, unTopIndex)]).aszMnemonicValue), szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P72DispAllocErr();
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
VOID    P72DispAllocErr(HWND hDlg, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P72_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P72_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P72, szCaption, PEP_STRING_LEN_MAC(szCaption));

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
**  Synopsis:   BOOL    P72ScrlProc();
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
BOOL    P72ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P72CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & Mnemonic Data by New Index ----- */
            P72RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P72_ONE_PAGE;

        /* ----- Redraw Description & Mnemonic Data by New Index ----- */
        P72RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P72CalcIndex();
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
BOOL    P72CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P72_ONE_LINE;

        if (unBottomIndex < P72_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = P72_ONE_PAGE;
            unTopIndex    = P72_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= P72_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P72_ONE_LINE;

        if (unBottomIndex >= P72_ADDR_NO) {    /* Scroll to Bottom */
            unBottomIndex = (P72_ADDR_NO - 1);
            unTopIndex    = P72_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += P72_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P72_SCBAR_MIN) {  /* Current Position isn't Top */
            unBottomIndex -= P72_ONE_PAGE;

            if (unBottomIndex <= P72_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = P72_ONE_PAGE;
                unTopIndex    = P72_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= P72_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P72_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += P72_ONE_PAGE;
            if (unBottomIndex >= P72_ADDR_NO) {    /* Scroll to Bottom */
                unBottomIndex = (P72_ADDR_NO - 1);
                unTopIndex    = P72_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += P72_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P72RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
VOID    P72RedrawText(HWND hDlg)
{
    UINT    unCount;
    UINT    unLoop = 0;
    WORD    wID;
    WCHAR    szWork[P72_DESC_LEN + 1];

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
//		LoadString(hResourceDll, (IDS_P072DESC001 + unCount), szWork, PEP_STRING_LEN_MAC(szWork));
		swprintf_s (szWork, P72_DESC_LEN, L"Reason Code #%d", unCount+1);
		DlgItemRedrawText(hDlg, (IDD_P72_DESC01 + unLoop), szWork);

        /* ----- Set Mnemonic Data to EditText ----- */
        wID = (WORD)(IDD_P72_EDIT01 + unLoop);

		DlgItemRedrawText(hDlg,wID,((lpRSNWork->RSNWork[P72GETINDEX(wID, unTopIndex)]).aszMnemonicValue));
    }
}


/* ===== End of P072.C ===== */
