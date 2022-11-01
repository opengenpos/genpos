/* ---------------------------------------------------------------------------
* Title         :   Combination Coupon Table Maintenance (AC 161)
* Category      :   Maintenance, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A161.C
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Mar-16-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
* Aug-09-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Dec-04-95 : 03.00.01 : T.Nakahata : BUG FIXED (A161GetData)
* Sep-18-98 : 03.03.00 : A.Mitsui   : V3.3 add VAT
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
#include    <string.h>
#include    <stdlib.h>


#include    <ecr.h>
#include    "paraequ.h"
#include    "para.h"
#include    "csop.h"
#include    "rfl.h"

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a161.h"

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
static  LPCPNPARA    lpCpnPara;     /* Address of Combination Coupon Data Buffer    */
static  LPA161DESC   lpCtrlDesc;    /* Address of Control Code Description          */
static  LPA161CANADA lpCanDesc;     /* Address of Canada Tax Description    */
static  UINT         unCpnIndex;    /* Current Selected Index of Combination Coupon */
static  BYTE         bTaxMode,      /* Current Tax Mode (US / Canada)       */
                     bCtrlCode;     /* Control Code Work Area               */

static  HWND  hWndModeLess = NULL;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A161DlgProc()
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
*                       Combination Coupon Maintenance (AC 161)
* ===========================================================================
*/
BOOL    WINAPI  A161DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;    /* Configulation of SpinButton          */
	static  WORD    wWidth,     /* Initial Width of DialogBox       */
                    wHeight;    /* Initial Height of DialogBox      */
    static  USHORT  unMaxCpn;   /* Maximum Number of Combination Coupon Data JHHJ CPN 100->300*/
    static  HGLOBAL hCpnMem,    /* Handle of Global Heap                */
                    hCtrlDesc,  /* Handle of Global Heap (Description)  */
                    hCanDesc;   /* Handle of Global Heap (Canada Tax)   */
	RECT    rectDlg;            /* Client Area of DialogBox     */
	RECT    rectGroupBox;       /* Client Area of Group Box on Left side  */
	WORD    wDlgWidth;          /* Width of DialogBox           */
    int     idEdit;
    HCURSOR hCursor;
    WORD    wDlgID;
	BOOL    fStatus1,           /* ACC Dialog Flag      */
		    fStatus2;           /* ACC Dialog Flag      */
    WORD   	wID;
	HWND    hGroupBox = 0;
	BOOL	priState = 0;
	DWORD helper;

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Check Max. Record No. of Combination Coupon Data ----- */
        if (A161ReadMax(hDlg, (LPUSHORT)&unMaxCpn)) {
			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        } else {
            /* ----- Allocate Global Heap ----- */
            if (A161AllocMem(hDlg, (LPHGLOBAL)&hCpnMem, (LPHGLOBAL)&hCtrlDesc, (LPHGLOBAL)&hCanDesc)) {
                //Return Cancel to indicate failure
				EndDialog(hDlg, LOWORD(IDCANCEL));
            }
			else
			{
                /* ----- Initialize Configulation of DialogBox ----- */
                A161InitDlg(hDlg, unMaxCpn);

                GetWindowRect(hDlg, (LPRECT)&rectDlg);
				hGroupBox = GetDlgItem (hDlg, IDD_A161NO);
                GetWindowRect(hGroupBox, (LPRECT)&rectGroupBox);

				wWidth  = (WORD)(rectDlg.right  - rectDlg.left);
				wHeight = (WORD)(rectDlg.bottom - rectDlg.top);

				wDlgWidth = (WORD)(rectGroupBox.right  - rectDlg.left + 5);

				SetWindowPos(hDlg, (HWND)NULL, 0, 0, wDlgWidth, wHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

                /* ----- Initialize Configulation of SpinButton ----- */
                PepSpin.nStep      = A161_SPIN_STEP;
                PepSpin.nTurbo     = A161_SPIN_TURBO;
                PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
            }
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A161CPN_LIST; j<=IDD_A161_VER7; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A161_CAPTION1; j<=IDD_A161_PRICE_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_ENTER, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */
        idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A161_SPIN_OFFSET);

        /* ----- Set Maximum Value of Data Range by Selected Button ----- */
        switch (idEdit) {

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        case IDD_A161TAG01:
        case IDD_A161TAG02:
        case IDD_A161TAG03:
        case IDD_A161TAG04:
        case IDD_A161TAG05:
        case IDD_A161TAG06:
        case IDD_A161TAG07:

            /* ----- Check Coupon Target Type bfrom CheckBox ----- */
            if (IsDlgButtonChecked(hDlg, (IDD_A161DEP01 + idEdit - IDD_A161TAG01)) == TRUE) {
                PepSpin.lMax = (long)A161_TAGDEPTMAX;           /* Coupon Target DEPT */
            } else {

                PepSpin.lMax = (long)A161_TAGPLUMAX;            /* Coupon Target PLU  */
            }
            PepSpin.lMin = (long)A161_DATAMIN;
            break;
#endif

        case IDD_A161ADJ01:
        case IDD_A161ADJ02:
        case IDD_A161ADJ03:
        case IDD_A161ADJ04:
        case IDD_A161ADJ05:
        case IDD_A161ADJ06:
        case IDD_A161ADJ07:
            PepSpin.lMin = (long)A161_DATAMIN;
            PepSpin.lMax = (long)A161_ADJMAX;
            break;

        /* ----- Coupon Group No,   R3.0 ----- */
        case IDD_A161GRP1:
        case IDD_A161GRP2:
        case IDD_A161GRP3:
        case IDD_A161GRP4:
        case IDD_A161GRP5:
        case IDD_A161GRP6:
        case IDD_A161GRP7:
            PepSpin.lMin = (long)A161_DATAMIN;
            PepSpin.lMax = (long)A161_GRPMAX;
            break;

        default:
            PepSpin.lMin = (long)A161_DATAMIN;
            PepSpin.lMax = (long)A161_AMOUNTMAX;
            break;

        }

        if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
            PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(idEdit), EN_CHANGE), 0L);
        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A161CPN_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Set Selected Coupon Data to Each Control ----- */
                A161SetData(hDlg);
				return TRUE;
            }
            return FALSE;

        case IDD_A161ACCFLG:
            if (HIWORD(wParam) == BN_CLICKED) {
				priState = SendDlgItemMessage(hDlg,IDD_A161ACCFLG,BM_GETCHECK,0,0);
				
				if(priState){
					//SendDlgItemMessage(hDlg,IDD_AUTO_PRI,WM_ENABLE,0,0);
					EnableWindow( GetDlgItem(hDlg, IDD_AUTO_PRI) ,priState);
					A161SetAutoData(hDlg);
				}
				else
				{
					EnableWindow( GetDlgItem(hDlg, IDD_AUTO_PRI) ,priState);
					DlgItemRedrawText(hDlg, IDD_AUTO_PRI, (LPCWSTR)""); // SR 1005 - CSMALL
				}
				A161AutoCouponCheck(hDlg, unMaxCpn);
		
                return TRUE;
            }
            return FALSE;

        case IDD_A161AMOUNT:
            if (HIWORD(wParam) == EN_CHANGE) {
                return TRUE;
            }
            return FALSE;

        case IDD_A161DEP01:
        case IDD_A161DEP02:
        case IDD_A161DEP03:
        case IDD_A161DEP04:
        case IDD_A161DEP05:
        case IDD_A161DEP06:
        case IDD_A161DEP07:
            if (HIWORD(wParam) == BN_CLICKED) {
                A161DeptPluChkRng(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDD_A161TAG01:
        case IDD_A161TAG02:
        case IDD_A161TAG03:
        case IDD_A161TAG04:
        case IDD_A161TAG05:
        case IDD_A161TAG06:
        case IDD_A161TAG07:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range of Target data ----- */
                A161TagChkRng(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDD_A161ADJ01:
        case IDD_A161ADJ02:
        case IDD_A161ADJ03:
        case IDD_A161ADJ04:
        case IDD_A161ADJ05:
        case IDD_A161ADJ06:
        case IDD_A161ADJ07:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range of Adjective code ----- */
                A161ChkRng(hDlg, LOWORD(wParam), A161_ADJMAX);
                return TRUE;
            }
            return FALSE;

        /* ----- Set Group No, R3.0 ----- */
        case IDD_A161GRP1:
        case IDD_A161GRP2:
        case IDD_A161GRP3:
        case IDD_A161GRP4:
        case IDD_A161GRP5:
        case IDD_A161GRP6:
        case IDD_A161GRP7:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range of Group code ----- */
                A161ChkRng(hDlg, LOWORD(wParam), A161_GRPMAX);
                return TRUE;
            }
            return FALSE;

		case IDD_A161BONUS:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range of Bonus Index ----- */
                A161ChkRng(hDlg, LOWORD(wParam), A161_BNSMAXVALUE);
                return TRUE;
            }
            return FALSE;

		case IDD_A161SALESCODE:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range of Sales Code Restriction ----- */
                A161ChkRng(hDlg, LOWORD(wParam), 7);
                return TRUE;
            }
            return FALSE;

        case IDD_SET:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set DialogBox ID by Current Tax Mode ----- */
                wDlgID = (WORD)((bTaxMode == A161_TAX_US)  ? IDD_A161_CTRL :
                                (bTaxMode == A161_TAX_CAN) ? IDD_A161_CTRL_CAN  :
                                                             IDD_A161_CTRL_VAT);
                /* ----- Create control code dialogbox ----- */
                DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(wDlgID), hDlg, A161CtrlDlgProc);

                /* ----- Set control code listbox content ----- */
                A161SetCtrlList(hDlg, A161_LBENTER);
                return TRUE;
            }
            return FALSE;

	   case IDD_AUTO_CHANGE:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Selected Coupon Data to Each Control ----- */
                A161SetAutoData(hDlg);
                return TRUE;
            }
            return FALSE;

	   case IDD_DELETE:
            if (HIWORD(wParam) == BN_CLICKED) {
                A161Del(hDlg);
                return TRUE;
            }
            return FALSE;

       case IDD_AUTO:
            if (HIWORD(wParam) == BN_CLICKED)
			{
                GetWindowRect(hDlg, (LPRECT)&rectDlg);
				hGroupBox = GetDlgItem (hDlg, IDD_A161_CAPTION29);
                GetWindowRect(hGroupBox, (LPRECT)&rectGroupBox);

				wDlgWidth = (WORD)(rectGroupBox.right  - rectDlg.left + 5);

				SetWindowPos(hDlg, (HWND)NULL, 0, 0, wDlgWidth, wHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

				/* ----- Get PushButton / RadioButton Status ----- */
				fStatus1 = (LOWORD(wParam) == IDD_AUTO) ? FALSE : TRUE;
				fStatus2 = (LOWORD(wParam) == IDD_AUTO) ? TRUE  : FALSE;

				A161InitAutoList(hDlg,unMaxCpn);

				EnableWindow(GetDlgItem(hDlg, IDD_AUTO), fStatus1);
				EnableWindow(GetDlgItem(hDlg, IDD_A161CLOSE),  fStatus2);
				EnableWindow(GetDlgItem(hDlg, IDD_AUTO_CHANGE), FALSE);

				wID = (WORD)((LOWORD(wParam) == IDD_AUTO) ?  IDD_A161CLOSE : IDD_AUTO);

				/* ----- Set Focus to PushButton ----- */
				SetFocus(GetDlgItem(hDlg, IDD_A161_AUTO_CPN_LIST));
				SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_SETCURSEL, (WPARAM)0, 0L);
                return TRUE;
            }
            return FALSE;

		case IDD_A161CLOSE:
			if (HIWORD(wParam) == BN_CLICKED)
			{
                GetWindowRect(hDlg, (LPRECT)&rectDlg);
				hGroupBox = GetDlgItem (hDlg, IDD_A161NO);
                GetWindowRect(hGroupBox, (LPRECT)&rectGroupBox);

				wDlgWidth = (WORD)(rectGroupBox.right  - rectDlg.left + 5);

				SetWindowPos(hDlg, (HWND)NULL, 0, 0, wDlgWidth, wHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

				/* ----- Get PushButton / RadioButton Status ----- */
				fStatus1 = (LOWORD(wParam) == IDD_AUTO) ? FALSE : TRUE;
				fStatus2 = (LOWORD(wParam) == IDD_AUTO) ? TRUE  : FALSE;

				DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_RESETCONTENT, 0, 0);
				EnableWindow(GetDlgItem(hDlg, IDD_AUTO), fStatus1);
				EnableWindow(GetDlgItem(hDlg, IDD_A161CLOSE),  fStatus2);

				wID = (WORD)((LOWORD(wParam) == IDD_AUTO) ?  IDD_A161CLOSE : IDD_AUTO);

				/* ----- Set Focus to PushButton ----- */
				SetFocus(GetDlgItem(hDlg, wID));
				return TRUE;
			}
			return FALSE;

        case IDD_ENTER:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Get Inputed Data from Each Controls ----- */
                A161GetData(hDlg);
               // A161InitAutoList(hDlg,unMaxCpn);
				SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_SETCURSEL, (WPARAM)0, 0L);
                return TRUE;
            }
            return FALSE;

      case IDD_AUTO_UPSWITCH:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Get Inputed Data from Each Controls ----- */
                A161AutoPriUp(hDlg);
				EnableWindow(GetDlgItem(hDlg, IDD_AUTO_CHANGE), TRUE);
                return TRUE;
            }
            return FALSE;

	  case IDD_AUTO_DOWNSWITCH:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Get Inputed Data from Each Controls ----- */
               A161AutoPriDown(hDlg);
			   EnableWindow(GetDlgItem(hDlg, IDD_AUTO_CHANGE), TRUE);
                return TRUE;
            }
            return FALSE;

		case IDD_AUTO_PRI: // Fix for SR 1005 - Adding enforcement for min/max Coupon Priority range - CSMALL
			if (HIWORD(wParam) == EN_CHANGE )
			{
				A161ChkPriorityRng(hDlg, IDD_AUTO_PRI);
			}
        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Change cursor (arrow -> hour-glass) ----- */

                hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                ShowCursor(TRUE);

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Write data to file ----- */

					{
						PARAMDC MdcData = { 0 };

						MdcData.usAddress = MDC_AUTOCOUP_ADR;
						ParaMDCRead(&MdcData);

						if (IsDlgButtonChecked(hDlg, IDD_A161EVALENABLED)) {
							MdcData.uchMDCData |= ODD_MDC_BIT0;    // enable Evaluate Auto Coupons
						}
						else {
							MdcData.uchMDCData &= ~ODD_MDC_BIT0;  // disable Evaluate Auto Coupons
						}
						ParaMDCWrite(&MdcData);
					}
					A161WriteData(unMaxCpn);
                }

                /* ----- Frees Allocated Memory ----- */

                A161FreeMem((LPHGLOBAL)&hCpnMem, (LPHGLOBAL)&hCtrlDesc, (LPHGLOBAL)&hCanDesc);

                /* ----- Restore cursor (hour-glass -> arrow) ----- */

                ShowCursor(FALSE);
                SetCursor(hCursor);

                EndDialog(hDlg, LOWORD(wParam));

                return TRUE;
            }
        }
        return FALSE;

	case WM_DESTROY:
		hWndModeLess = NULL;
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A161CtrlDlgProc()
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
*            Combination Coupon Maintenance (AC 161)
* ===========================================================================
*/
BOOL    WINAPI  A161CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Set data to control code dialogbox ----- */

        A161SetCtrlData(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j = IDD_A161CPN_LIST; j <= IDD_A161_VER7; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A161_GROUPBOX2; j<=IDD_A161_GROUPBOX3; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Inputed Control Data to Buffer ----- */

                    A161GetCtrlData(hDlg);
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
**  Synopsis    :   BOOL    A161ReadMax()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPUINT  lpunMaxCpn  -   Address of Max. Value of Coupon Data
*
**  Return      :   BOOL    TRUE        -   Max. of Coupon Data is 0.
*                           FALSE       -   Max. of Coupon Data is valid.
*
**  Description :
*       This function gets the maximum number of coupon data. It returns TRUE,
*   if the maximum number of coupon records is invalid(0).
*   Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A161ReadMax(HWND hDlg, LPUSHORT lpunMaxCpn)
{
    BOOL        fRet = FALSE;
    PARAFLEXMEM FlexData;
    WCHAR        szCap[PEP_CAPTION_LEN],
                szMsg[256];

    /* ----- Get Max. No. of Coupon Records from Prog.#2 ----- */

    FlexData.uchAddress = FLEX_CPN_ADR;

    ParaFlexMemRead(&FlexData);

    /* ----- Set Maximum Record No. of Coupon to Data Area ----- */
    	if (FlexData.ulRecordNumber > FLEX_CPN_MAX) {
		*lpunMaxCpn = FLEX_CPN_MAX;    // set the maximum to be the maximum allowed
		FlexData.ulRecordNumber = FLEX_CPN_MAX;
	}


    /* NCR2172 */
    /* *lpunMaxCpn = (UINT)FlexData.usRecordNumber; */
    *lpunMaxCpn = (USHORT)FlexData.ulRecordNumber;

    /* ----- Determine Whether Max. of Coupon Records is valid or Not ----- */

    /* NCR2172 */
    /*if (! FlexData.usRecordNumber) {*/
    if (FlexData.ulRecordNumber < 1) {

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap, PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_A161_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A161AllocMem()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPHGLOBAL   lphCpn  -   Address of Global Heap Handle
*                   LPHGLOBAL   lphDesc -   Address of Global Heap Handle
*                   LPHGLOBAL   lphCan  -   Address of Global Heap Handle
*
**  Return      :   BOOL    TRUE    -   Memory Allocation is Failed.
*                           FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This function allocates the data buffer from the global heap.
*   It returns TRUE, if it failed in momory allocation. Otherwise it returns
*   FALSE.
* ===========================================================================
*/
BOOL   A161AllocMem(HWND hDlg, LPHGLOBAL lphCpn, LPHGLOBAL lphDesc, LPHGLOBAL lphCan)
{
    BOOL    fRet = FALSE;
    WCHAR    szCaption[PEP_CAPTION_LEN],
            szErrMsg[PEP_ALLOC_LEN];

    /* ----- Allocate Memory from Global Heap ----- */

    *lphCpn  = GlobalAlloc(GHND, (sizeof(COMCOUPONPARA) * FLEX_CPN_MAX));
    *lphDesc = GlobalAlloc(GHND, (sizeof(A161DESC)   * A161_CTRLCODE_NO));
    *lphCan  = GlobalAlloc(GHND, (sizeof(A161CANADA) * (A161_CANADA_MAX + 1)));

    /* ----- Determine Whether Memory Allocation is Success or Not ----- */

    if ((! *lphCpn) || (! *lphDesc) || (! *lphCan)) {

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,    szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;

        /* ----- Set the Allocated Area Free ----- */

        if (*lphCpn) {

            GlobalFree(*lphCpn);
        }

        if (*lphDesc) {

            GlobalFree(*lphDesc);
        }

        if (*lphCan) {

            GlobalFree(*lphCan);
        }

    } else {

        /* ----- Lock the Allocated Area ----- */

        lpCpnPara  = (LPCPNPARA)GlobalLock(*lphCpn);
        lpCtrlDesc = (LPA161DESC)GlobalLock(*lphDesc);
        lpCanDesc  = (LPA161CANADA)GlobalLock(*lphCan);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161InitDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unMaxCpn    -   Max. Record No. of Coupon
*
**  Return      :   No return value.
*
**  Description :
*       This function gets current tax mode. And then it loads the initial
*   coupon data, and sets them to listbox.
* ===========================================================================
*/
VOID    A161InitDlg(HWND hDlg, USHORT unMaxCpn)
{

    BYTE bTaxCheck1, bTaxCheck2;

    /* ----- Set Control Code Description to Allocated Area ----- */

    A161InitCtrlDesc();

    /* ----- Read Initial Coupon Data from Combination Coupon File ----- */

    A161ReadData(unMaxCpn);

    /* ----- Set Loaded Coupon Data to ListBox ----- */

    A161InitList(hDlg, unMaxCpn);

    /* ----- Limit Max. Length of Input Data to Each EditText ----- */

    SendDlgItemMessage(hDlg, IDD_A161AMOUNT,  EM_LIMITTEXT, A161_AMOUNTLEN,  0L);
    SendDlgItemMessage(hDlg, IDD_A161MNEMO,   EM_LIMITTEXT, PARA_CPN_LEN,    0L);
	SendDlgItemMessage(hDlg, IDD_A161BONUS,   EM_LIMITTEXT, A161_BNSMAX,	 0L);
	SendDlgItemMessage(hDlg, IDD_A161SALESCODE, EM_LIMITTEXT, 1,	 0L);

    SendDlgItemMessage(hDlg, IDD_A161TAG01,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG02,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG03,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG04,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG05,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG06,   EM_LIMITTEXT, A161_TAGETLEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A161TAG07,   EM_LIMITTEXT, A161_TAGETLEN,   0L);

    SendDlgItemMessage(hDlg, IDD_A161ADJ01,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ02,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ03,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ04,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ05,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ06,   EM_LIMITTEXT, A161_ADJLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161ADJ07,   EM_LIMITTEXT, A161_ADJLEN,     0L);

    /* ----- R3.0 ----- */
    SendDlgItemMessage(hDlg, IDD_A161GRP1,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP2,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP3,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP4,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP5,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP6,    EM_LIMITTEXT, A161_GRPLEN,     0L);
    SendDlgItemMessage(hDlg, IDD_A161GRP7,    EM_LIMITTEXT, A161_GRPLEN,     0L);

    /* ----- Set Fixed Font ----- */

    PepSetFont(hDlg, IDD_A161MNEMO);
    PepSetFont(hDlg, IDD_A161CPN_LIST);

    /* ----- Get Current Tax Mode Status from Prog. #1 (MDC) ----- */

    bTaxCheck1 = ParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0);
    bTaxCheck2 = ParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0);

    bTaxMode = (BYTE)(!(bTaxCheck1) ? A161_TAX_US  :
                      !(bTaxCheck2) ? A161_TAX_CAN :
                                      A161_TAX_VAT);

    /* ----- Set HighLight Bar to the Top Index of ListBox ----- */

    SendDlgItemMessage(hDlg, IDD_A161CPN_LIST, LB_SETCURSEL, 0, 0L);

    /* ----- Set Coupon Data of the Top Index to Each Control ----- */

    A161SetData(hDlg);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161FreeMem()
*
**  Input       :   LPHGLOBAL   lphCpn  -   Address of Global Heap Handle
*                   LPHGLOBAL   lphDesc -   Address of Global Heap Handle
*                   LPHGLOBAL   lphCan  -   Address of Global Heap Handle
*
**  Return      :   No return value.
*
**  Description :
*       This function frees the allocated memory from global heap.
* ===========================================================================
*/
VOID A161FreeMem(LPHGLOBAL lphCpn, LPHGLOBAL lphDesc, LPHGLOBAL lphCan)
{
    /* ----- Unlock the Allocated Area ----- */

    GlobalUnlock(*lphCpn);
    GlobalUnlock(*lphDesc);
    GlobalUnlock(*lphCan);

    /* ----- Free the Allocated Area ----- */

    GlobalFree(*lphCpn);
    GlobalFree(*lphDesc);
    GlobalFree(*lphCan);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161InitCtrlDesc()
*
**  Input       :   No input value.
*
**  Return      :   No return value.
*
**  Description :
*       This function loads a control code description and sets it to the
*   allocated area.
* ===========================================================================
*/
VOID    A161InitCtrlDesc(VOID)
{
    WORD            wI;
    UINT            wOnStrID  = IDS_A161CTRL101,
                    wOffStrID = IDS_A161CTRL001;
    LPA161DESC      lpDesc;
    LPA161CANADA    lpCanada;

    /* ----- Initialize Address of Control Code String Buffer ----- */

    lpDesc = lpCtrlDesc;

    for (wI = 0; wI < A161_CTRLCODE_NO; wI++, wOnStrID++, wOffStrID++, lpDesc++) {

        /* ----- Load Control Code String from Resorce ----- */

        LoadString(hResourceDll, wOnStrID,  lpDesc->szDescON,  A161_DESC_LEN);
        LoadString(hResourceDll, wOffStrID, lpDesc->szDescOFF, A161_DESC_LEN);
    }

    /* ----- Initialize Address of Canadian Tax String Buffer ----- */

    lpCanada = lpCanDesc;

    for (wI = 0, wOnStrID = IDS_A161CAN_0; wI <= A161_CANADA_MAX; wI++, lpCanada++) {

        /* ----- Load Canadian Tax String from Resource ----- */

        LoadString(hResourceDll, wOnStrID++, lpCanada->szCanDesc, A161_DESC_LEN);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161ReadData()
*
**  Input       :   UINT    unMaxCpn   -   Max. Value of Coupon Records
*
**  Return      :   No return value.
*
**  Description :
*       This function reads a coupon data from the Combination Coupon file, and
*   sets it to the data buffer. It reads a coupon data until the maximum number
*   of coupon records.
* ===========================================================================
*/
VOID    A161ReadData(USHORT unMaxCpn)
{
    CPNIF       CpnData;
    LPCPNPARA   lpCpn;
    USHORT        unI;

    for (unI = 1, lpCpn = lpCpnPara; unI <= unMaxCpn; unI++, lpCpn++) {

        /* ----- Set Target Coupon No. ----- */

        CpnData.uchCpnNo = unI;

        /* ----- Read Target Coupon Data from Combination Coupon File ----- */

        OpCpnIndRead(&CpnData, 0);

        /* ----- Copy Loaded Coupon Data to Coupon Data Buffer ----- */

        memcpy(lpCpn, &CpnData.ParaCpn, sizeof(COMCOUPONPARA));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161InitList()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unMaxCpn    -   Max. Value of Coupon Records
*
**  Return      :   No return value.
*
**  Description :
*       This function initializes the configulation of a coupon listbox.
* ===========================================================================
*/
VOID    A161InitList(HWND hDlg, USHORT unMaxCpn)
{
    LPCPNPARA   lpCpn;
    USHORT        unI;
    WCHAR        szWork[32],
                szDesc[32];

    /* ----- Load String from Resource ----- */

    LoadString(hResourceDll, IDS_A161_NOREC, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Create Max. Record Number Description ----- */

    wsPepf(szDesc, szWork, unMaxCpn);

    /* ----- Set Created Description to StaticText ----- */

    DlgItemRedrawText(hDlg, IDD_A161MAX, szDesc);

    for (unI = 1, lpCpn = lpCpnPara; unI <= unMaxCpn; unI++, lpCpn++) {

        /* ----- Replace Double Width Key (0x12 -> '~') ----- */

		memset(szWork, 0x00, sizeof(szDesc));
        PepReplaceMnemonic(lpCpn->aszCouponMnem, szWork, (PARA_CPN_LEN ), PEP_MNEMO_READ);

        /* ----- Create Description for Insert ListBox ----- */
		memset(szDesc, 0x00, sizeof(szDesc));
        wsPepf(szDesc, WIDE("%2u : %s"), unI, szWork);

        /* ----- Set Created Description to ListBox ----- */

        DlgItemSendTextMessage(hDlg, IDD_A161CPN_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected coupon data to each contols.
* ===========================================================================
*/
VOID    A161SetData(HWND hDlg)
{
    LPCPNPARA   lpCpn;
    WORD        wIDTag,
                wIDAdj,
                wIDGrp,
                wI,
				wIDDept;
    WCHAR        szWork[32],
                szDesc[32];

    USHORT      usI;

    WCHAR       szPlu[PLU_MAX_DIGIT + 1],
                szWorkPlu[PLU_MAX_DIGIT + 1];    /* ###ADD Saratoga */

    /* ----- Get Current Selected Index of ListBox ----- */

    unCpnIndex = (UINT)SendDlgItemMessage(hDlg, IDD_A161CPN_LIST, LB_GETCURSEL, 0, 0L);

    /* ----- Set Address of Current Selected Coupon Data to Work Area ----- */

    lpCpn = (LPCPNPARA)(lpCpnPara + unCpnIndex);

    /* ----- Load String from Resource ----- */

    LoadString(hResourceDll, IDS_A161_TITLE, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Create String of Current Selected Coupon No. ----- */

    wsPepf(szDesc, szWork, unCpnIndex + 1);

    /* ----- Set Created String to GroupBox ----- */

    DlgItemRedrawText(hDlg, IDD_A161NO, szDesc);

    /* ----- Set Amount Data to EditText ----- */

    _ultow(( ULONG)(lpCpn->ulCouponAmount), szWork, A161_RADIX_10);
    DlgItemRedrawText(hDlg, IDD_A161AMOUNT, szWork);

    /* ----- Replace Double Width Key (0x12 -> '~') ----- */

	memset(&szWork, 0x00, sizeof(szWork));
    PepReplaceMnemonic(lpCpn->aszCouponMnem, szWork, (PARA_CPN_LEN), PEP_MNEMO_READ);

    /* ----- Set Target & Adjective Data to EditText,  R3.0 ----- */
    for (wIDDept = IDD_A161DEP01, wI = 0, wIDTag = IDD_A161TAG01, wIDAdj = IDD_A161ADJ01, wIDGrp = IDD_A161GRP1;
                     wI< A161_TAGSTS_CNT; wI++, wIDTag++, wIDAdj++, wIDGrp++,wIDDept++ )
    {

/*** NCR2172
        SetDlgItemInt(hDlg, wIDTag, (UINT)lpCpn->usCouponTarget[wI], FALSE);
***/
        memset(szPlu, 0x00, sizeof(szPlu));
        memset(szWorkPlu, 0x00, sizeof(szWorkPlu));

        memcpy(szWorkPlu, lpCpn->auchCouponTarget[wI], PLU_MAX_DIGIT * sizeof(WCHAR));

        memcpy(szPlu, szWorkPlu, sizeof(szPlu));
        RflConvertPLU(szPlu, szWorkPlu);

		A161SetChkbox(hDlg, wIDDept,0, A161_CHKBIT2);

        DlgItemRedrawText(hDlg, wIDTag, szPlu);

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        DlgItemRedrawText(hDlg, wIDTag, lpCpn->auchCouponTarget[wI]);
#endif
        SetDlgItemInt(hDlg, wIDAdj, (UINT)lpCpn->uchCouponAdj[wI] & 0x0f, FALSE);
        SetDlgItemInt(hDlg, wIDGrp, (UINT)lpCpn->uchCouponAdj[wI] >> 4, FALSE);
    }

    /* ----- Set Coupon Mnemonic to EditText ----- */
    DlgItemRedrawText(hDlg, IDD_A161MNEMO, szWork);

    /* ----- Set Status of Addr.3,4 to CheckButton ----- */
    A161SetChkbox(hDlg, IDD_A161DEP01, ( BYTE)(lpCpn->uchCouponStatus[A161_TAGSTS_ADDR]), A161_CHKBIT2);

	memset(szWork, 0x00, sizeof(szWork));
	_ltow(lpCpn->usBonusIndex, szWork,10);

	DlgItemRedrawText(hDlg, IDD_A161BONUS,szWork);
    SetDlgItemInt(hDlg, IDD_A161SALESCODE, (lpCpn->uchRestrict & 0x07), FALSE);
	CheckDlgButton(hDlg, IDD_A161SUPRINTV_FLAG, (lpCpn->uchRestrict & 0x80) ? TRUE : FALSE);

	if (lpCpn->uchCouponStatus[0] & A161_AUTO_CPN) {
		//SendDlgItemMessage(hDlg,IDD_AUTO_PRI,WM_ENABLE,0,0);
		CheckDlgButton(hDlg, IDD_A161ACCFLG, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDD_AUTO_PRI), TRUE);
		A161SetAutoData(hDlg);
	}
	else
	{
		CheckDlgButton(hDlg, IDD_A161ACCFLG, FALSE);
		EnableWindow(GetDlgItem(hDlg, IDD_AUTO_PRI), FALSE);
		DlgItemRedrawText(hDlg, IDD_AUTO_PRI, (LPCWSTR)""); // SR 1005 - CSMALL
	}

	CheckDlgButton(hDlg, IDD_A161EVALENABLED, (ParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT0)) ? TRUE : FALSE);

    /* ----- Set Contents of Control Code ListBox ----- */
    A161SetCtrlList(hDlg, A161_LBSET);

    /* ----- Set Control Code Data to Work Area ----- */
    bCtrlCode = (BYTE)lpCpn->uchCouponStatus[A161_STS01_ADDR];

    /* ----- Clear UPC-E Checkbox, Saratoga ----- */
    for(usI = 0; usI < OP_CPN_TARGET_SIZE; usI++) {
        SendMessage((GetDlgItem(hDlg, IDD_A161_VER1 + usI)), BM_SETCHECK, 0, 0L);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetCtrlList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   BOOL    fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of control codes to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID    A161SetCtrlList(HWND hDlg, BOOL fMode)
{
    /* ----- Reset Contents of Control Code ListBox ----- */

    SendDlgItemMessage(hDlg, IDD_A161CTRL_LIST, LB_RESETCONTENT, 0, 0L);

    /* ----- Determine Current Tax Mode ----- */

    if (bTaxMode == A161_TAX_CAN) {

        /* ----- Set Content for Address 1 & 2 (Canadian Tax) ----- */

        A161SetCanadaDesc(hDlg, (LPCPNPARA)(lpCpnPara + unCpnIndex), fMode);

    } else if (bTaxMode == A161_TAX_US) {

        /* ----- Set Content for Address 4 & 5 (U.S. Tax) ----- */

        A161SetListContent(hDlg, A161_CHKBIT1, A161_STS01_ADDR, A161_DSCRB1, fMode);
    } else {

        /* ----- Set Content for Address 4 & 5 (International VAT) ----- */

        A161SetVATDesc(hDlg, (LPCPNPARA)(lpCpnPara + unCpnIndex), fMode);

    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetListContent()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*                   WORD    wOffset     -   String Offset of Control Code
*                   BOOL    fMode       -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of control code to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID
A161SetListContent(HWND hDlg, BYTE bChkBit, BYTE bCtrlAddr, WPARAM wOffset, BOOL fMode)
{
    BYTE    bI,
            bCtrlData;
    LPCWSTR  lpszDesc;

    /* ----- Get Target Control Code to Work Area ----- */

    bCtrlData = (BYTE)((fMode == A161_LBENTER) ? bCtrlCode :
                       (lpCpnPara + unCpnIndex)->uchCouponStatus[bCtrlAddr]);

    for(bI = 0; bI < A161_8BITS; bI++) {

        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */

        if (bChkBit & (A161_BITMASK << bI)) {

            /* ----- Check Current Bit is ON or OFF ----- */

            if (bCtrlData & (A161_BITMASK << bI)) {

                lpszDesc = (lpCtrlDesc + wOffset)->szDescON;

            } else {

                lpszDesc = (lpCtrlDesc + wOffset)->szDescOFF;
            }

            /* ----- Increment Descriptin Offset ----- */

            wOffset++;

            /* ----- Insert Description to Control Code ListBox ----- */

            DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetCanadaDesc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPDEPTPARA  lpDept  -   Address of Target Dept. Data
*                   BOOL        fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of Canadian tax to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID    A161SetCanadaDesc(HWND hDlg, LPCPNPARA lpCpn, BOOL fMode)
{
    BYTE    bDisc,
            bCanTax,
            bWork;
    LPCWSTR  lpszDesc;

    /* ----- Get Control Code Data (Address 1 & 2) ----- */

    bWork = (BYTE)((fMode == A161_LBENTER) ? bCtrlCode:
                                lpCpn->uchCouponStatus[A161_STS01_ADDR]);

    /* ----- Get Current Canadian Tax Data ----- */

    bCanTax = PEP_HIMASK(bWork);

    /* ----- Adjust Canadian Tax Data ----- */

    bCanTax = (BYTE)((bCanTax > A161_TAX_MAX) ? A161_TAX_MAX : bCanTax);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)((lpCanDesc + bCanTax)->szCanDesc));

    /* ----- Get Current Discount Item #1 & #2 Data ----- */

    bDisc = PEP_LOMASK(bWork);

    /* ----- Set Address of Single use String ----- */

    lpszDesc = ((bDisc & A161_SINGLE_ITEM) ?
                        (lpCtrlDesc + A161_SINGLEITEM)->szDescON:
                        (lpCtrlDesc + A161_SINGLEITEM)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #1 String ----- */

    lpszDesc = ((bDisc & A161_DISC_ITEM1) ?
                        (lpCtrlDesc + A161_DSCITEM1)->szDescON:
                        (lpCtrlDesc + A161_DSCITEM1)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #2 String ----- */

    lpszDesc = ((bDisc & A161_DISC_ITEM2) ?
                        (lpCtrlDesc + A161_DSCITEM2)->szDescON:
                        (lpCtrlDesc + A161_DSCITEM2)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetVATDesc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPDEPTPARA  lpDept  -   Address of Target Dept. Data
*                   BOOL        fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of International VAT to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID    A161SetVATDesc(HWND hDlg, LPCPNPARA lpCpn, BOOL fMode)
{
    BYTE    bDisc,
            bVAT,
            bWork;
    WCHAR    szDesc[A161_DESC_LEN],
            szWork[A161_DESC_LEN];
    LPCWSTR  lpszDesc;

    /* ----- Get Control Code Data (Address 1 & 2) ----- */

    bWork = (BYTE)((fMode == A161_LBENTER) ? bCtrlCode:
                                lpCpn->uchCouponStatus[A161_STS01_ADDR]);

    /* ----- Get Current International VAT Data ----- */

    bVAT = (BYTE)(bWork & A161_TAX_BIT_VAT);

    /* ----- Insert Description to Control Code ListBox ----- */

    LoadString(hResourceDll, IDS_A161VAT_0 + bVAT, szDesc, PEP_STRING_LEN_MAC(szDesc));

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));

    /* ----- Get Current VAT Serviceable Data ----- */

    bVAT = (BYTE)(bWork & A161_TAX_BIT_SERVICE);

    /* ----- Insert Description to Control Code ListBox ----- */

    LoadString(hResourceDll, IDS_A161_VATSERON, szDesc, PEP_STRING_LEN_MAC(szDesc));
    LoadString(hResourceDll, IDS_A161_VATSEROFF, szWork, PEP_STRING_LEN_MAC(szWork));

    lpszDesc = (bVAT) ? szDesc : szWork;

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Get Current Discount Item #1 & #2 Data ----- */

    bDisc = PEP_LOMASK(bWork);

    /* ----- Set Address of Single use String ----- */

    lpszDesc = ((bDisc & A161_SINGLE_ITEM) ?
                        (lpCtrlDesc + A161_SINGLEITEM)->szDescON:
                        (lpCtrlDesc + A161_SINGLEITEM)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #1 String ----- */

    lpszDesc = ((bDisc & A161_DISC_ITEM1) ?
                        (lpCtrlDesc + A161_DSCITEM1)->szDescON:
                        (lpCtrlDesc + A161_DSCITEM1)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #2 String ----- */

    lpszDesc = ((bDisc & A161_DISC_ITEM2) ?
                        (lpCtrlDesc + A161_DSCITEM2)->szDescON:
                        (lpCtrlDesc + A161_DSCITEM2)->szDescOFF);

    /* ----- Insert Description to Control Code ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A161WriteData()
*
**  Input       :   UINT    unMax   -   Max. Record No. of Coupon Data
*
**  Return      :   No return value.
*
**  Description :
*       This function writes the coupon data to the Combination coupon File.
* ===========================================================================
*/
VOID    A161WriteData(USHORT unMax)
{
    CPNIF       CpnData;
    LPCPNPARA   lpCpn;
    USHORT        unI;

    for (unI = 1, lpCpn = lpCpnPara; unI <= unMax; unI++, lpCpn++) {

        /* ----- Set Target Coupon No. ----- */

        CpnData.uchCpnNo = unI;

        /* ----- Set Target Department Data ----- */

        memcpy(&CpnData.ParaCpn, lpCpn, sizeof(COMCOUPONPARA));

        /* ----- Write data individualy ------ */

        OpCpnAssign(&CpnData, 0);
    }

    /* ----- Set File Status Flag ----- */

    PepSetModFlag(hwndActMain, PEP_MF_CPN, 0);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*                   UINT    unMax   -   Max. Value of Input Data
*
**  Return      :   No return value.
*
**  Description :
*       This function checks whether the inputed data is out of range or not.
*   It displays a error message by messagebox, if the inputed data is invalid.
* ===========================================================================
*/
VOID    A161ChkRng(HWND hDlg, WORD wEditID, UINT unMax)
{
    UINT    unValue;
    short   nMin;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szWork[PEP_OVER_LEN],
            szMsg[PEP_OVER_LEN];

    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);

    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */

    if (unValue > unMax) {

        nMin = A161_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_PEP_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, unMax);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error Occurred EditText ----- */

        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161TagChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This function checks whether the Target inputed data is out of range or not.
*   It displays a error message by messagebox, if the inputed data is invalid.
* ===========================================================================
*/
VOID    A161TagChkRng(HWND hDlg, WORD wEditID)
{
    UINT    unValue;
    short   nMin;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szWork[PEP_OVER_LEN],
            szMsg[PEP_OVER_LEN];

    WCHAR   szEdit[PLU_MAX_DIGIT + 1],    /* Saratoga */
            szMax[PLU_MAX_DIGIT + 1],     /* Saratoga */
            szMin[PLU_MAX_DIGIT + 1];

    /* ----- Get Inputed Data from EditText ----- */
/*** Sara
    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);
***/

    memset(szEdit, 0x00, sizeof(szEdit));
    unValue = DlgItemGetText(hDlg, wEditID, szEdit, PEP_STRING_LEN_MAC(szEdit));

    /* ----- Check Coupon Target Type from DlgButton ----- */

    memset(szMax, 0x00, sizeof(szMax));

    if (IsDlgButtonChecked(hDlg, (IDD_A161DEP01 + wEditID - IDD_A161TAG01)) == TRUE) {

     /*           unMax = A161_TAGDEPTMAX;  Saratoga */                   /* Coupon Target DEPT */
      memcpy(szMax, A161_TAGDEPTMAX, sizeof(A161_TAGDEPTMAX));
	  memcpy(szMin, A161_TAGDEPTMIN, 8);

    } else {

     /*           unMax = A161_TAGPLUMAX;    Saratoga */                  /* Coupon Target PLU  */
      memcpy(szMax, A161_TAGPLUMAX, sizeof(A161_TAGPLUMAX));
	  memcpy(szMin, A161_TAGPLUMIN, 26);

    }

    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */

/*** Saratoga
    if (unValue > unMax) {
***/
    szEdit[PLU_MAX_DIGIT] = 0x00;
    if (memcmp(szEdit, szMax, unValue) > 0  || memcmp(szEdit, szMin, unValue) < 0) {

        nMin = A161_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));

/* saratoga
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, unMax);
*/
        LoadString(hResourceDll, IDS_A161_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, szMax);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error Occurred EditText ----- */

        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
    }

/* Saratoga */
     else if (IsDlgButtonChecked(hDlg, (IDD_A161DEP01 + wEditID - IDD_A161TAG01)) == TRUE) {

      if(unValue > A161_DEPTLEN)                   /* Coupon Target DEPT */
      {
        nMin = A161_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_A161_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, szMax);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error Occurred EditText ----- */

        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
      }
    }

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161DeptPluChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This function checks whether the Target inputed data is out of range or not.
*   It displays a error message by messagebox, if the inputed data is invalid.
* ===========================================================================
*/
VOID    A161DeptPluChkRng(HWND hDlg, WORD wEditID)
{
    UINT    unValue;
    short   nMin;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szWork[PEP_OVER_LEN],
            szMsg[PEP_OVER_LEN];

    WCHAR szEdit[PLU_MAX_DIGIT + 1],
         szMax[PLU_MAX_DIGIT + 1];

    /* ----- Get Inputed Data from EditText ----- */
    memset(szEdit, 0x00, sizeof(szEdit));
    unValue = DlgItemGetText(hDlg, (IDD_A161TAG01 + wEditID - IDD_A161DEP01), szEdit, PEP_STRING_LEN_MAC(szEdit));

    /* ----- Check Coupon Target Type from DlgButton ----- */

    memset(szMax, 0x00, sizeof(szMax));
    if (IsDlgButtonChecked(hDlg, wEditID) == TRUE) {

           /*     unMax = A161_TAGDEPTMAX;   saratoga */                  /* Coupon Target DEPT */

      memcpy(szMax, A161_TAGDEPTMAX, sizeof(A161_TAGDEPTMAX));

    } else {

           /*     unMax = A161_TAGPLUMAX;      saratoga */                /* Coupon Target PLU  */

      memcpy(szMax, A161_TAGPLUMAX, sizeof(A161_TAGPLUMAX));
    }

    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */

    szEdit[PLU_MAX_DIGIT] = 0x00;
    if (memcmp(szEdit, szMax, unValue) > 0) {

        nMin = A161_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A161_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, szMax);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);
    }
/* Saratoga */
    else if (IsDlgButtonChecked(hDlg, wEditID) == TRUE) {

      if(unValue > A161_DEPTLEN)                   /* Coupon Target DEPT */
      {
        nMin = A161_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_A161_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, nMin, szMax);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);
        /* ----- Set Focus to Error Occurred EditText ----- */

        SetFocus(GetDlgItem(hDlg, (IDD_A161TAG01 + wEditID - IDD_A161DEP01)));
        SendDlgItemMessage(hDlg, (IDD_A161TAG01 + wEditID - IDD_A161DEP01), EM_SETSEL, 1, MAKELONG(0, -1));
      }
    }
}

/*
* ===========================================================================
**  Synopsis:   static  SHORT    A161GetPluNo();
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               BOOL        fVer      - Label Type
*               LPSTR       lpszPluNo - address of PLU No.
*
**  Return  :   TRUE                  - user process is executed
*               FALSE                 - invalid PLU No.
*
**  Description:
*       This function get PLU No. as PLU formated data.
* ===========================================================================
*/
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
BOOL   A161GetPluNo(BOOL fVer, char* lpszPluNo )
#endif
SHORT   A161GetPluNo(BOOL fVer, WCHAR* lpszPluNo )
{
    LABELANALYSIS Label;
    BOOL          sStat;

    /* ----- Initialize buffer ----- */
    memset(&Label, 0x00, sizeof(LABELANALYSIS));

    /* ----- Set flag ----- */
    if(fVer == LA_NORMAL) {
       Label.fchModifier = LA_NORMAL;   /* Normal        */
    } else {
       Label.fchModifier = LA_EMOD;     /* UPC-E Version */
    }

    memcpy(Label.aszScanPlu, lpszPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

    /* ----- Analyze PLU No. ----- */
    sStat = RflLabelAnalysis(&Label);

    if(sStat == LABEL_ERR) {

       return A161_ERR;

    } else {                   /* sStat == LABEL_OK */

       if(Label.uchType == LABEL_RANDOM) {

          if(Label.uchLookup == LA_EXE_LOOKUP) {

             memcpy(lpszPluNo, Label.aszMaskLabel, OP_PLU_LEN * sizeof(WCHAR));

             return A161_OK;

          } else {

                return A161_LOOKUP_ERR;
          }

       } else {

             memcpy(lpszPluNo, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));

             return A161_OK;
       }
    }

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161GetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed data from each controls and inputed control
*   code data to the Coupon data buffer.
* ===========================================================================
*/
VOID    A161GetData(HWND hDlg)
{
    BYTE    bI;
    WORD    wI,
            wID;
    WCHAR    szWork[32],
            szDesc[32];
    WCHAR    szMsg[256];
    WCHAR   szCouponTemp[OP_CPN_TARGET_SIZE][OP_PLU_LEN + 1];


    BOOL    fVer, bAuto;
    SHORT   usRet;

    UINT    uWI;

    memset(szCouponTemp, 0x00, sizeof(szCouponTemp));

    for (wI = 0, wID = IDD_A161TAG01; wI < A161_TAGSTS_CNT; wI++, wID++) {

        memset(szWork, 0x00, sizeof(szWork));
        uWI = DlgItemGetText(hDlg, wID, szWork, OP_PLU_LEN + 1);
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        if(IsDlgButtonChecked(hDlg, (IDD_A161DEP01 + wID - IDD_A161TAG01)) == FALSE) { /* Coupon Target PLU ?*/
#endif
           if(IsDlgButtonChecked(hDlg, (IDD_A161_VER1 + wI)) == FALSE) {               /* NORMAL Version */

              fVer = LA_NORMAL;

           } else {       /* UPC-E Version */

              fVer = LA_EMOD;
           }

           if(uWI != 0) {

              usRet = A161GetPluNo(fVer, szWork);

              if(usRet != A161_OK) {         /* Error !!! */

                 memset(szMsg, 0x00, sizeof(szMsg));
                 memset(szDesc, 0x00, sizeof(szDesc));

                 /* ----- Display caution message ----- */

                 LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szDesc, PEP_STRING_LEN_MAC(szDesc));

                 if (usRet == A161_ERR) {
                    LoadString(hResourceDll, IDS_A161_ERR_PLUNO, szMsg, PEP_STRING_LEN_MAC(szMsg));
                 } else {
                    LoadString(hResourceDll, IDS_A161_ERR_LABEL, szMsg, PEP_STRING_LEN_MAC(szMsg));
                 }

                 MessageBeep(MB_ICONEXCLAMATION);
                 MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);

                 /* ----- Set focus, and cursor selected ----- */
                 SetFocus(GetDlgItem(hDlg, IDD_A161TAG01 + wI));

                 SendDlgItemMessage(hDlg, IDD_A161TAG01 + wI, EM_SETSEL, 1, MAKELONG(-1, 0));
                 return ;
              }

           }
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        }
#endif
        /* ----- set coupontemp area from (PLU/DEPT) editbox----- */

        memcpy(szCouponTemp[wI], szWork, PLU_MAX_DIGIT * sizeof(WCHAR));

    }

    /* ----- Get AMOUNT Data from EditText ----- */

    A161SaveAmount(hDlg);

	/* ---- Save Priority Number --------*/
	A161SavePriority(hDlg);

	/* ---- Save Bonus Index --------*/
	A161SaveBonus(hDlg);

    /* ----- Get Status of Addr.1,2 to CheckButton ----- */

    (lpCpnPara + unCpnIndex)->uchCouponStatus[A161_STS01_ADDR] = bCtrlCode;

    /* Set Auto Combinational Coupon Status */
	bAuto = IsDlgButtonChecked(hDlg, IDD_A161ACCFLG);
	if(bAuto)
	{
		(lpCpnPara + unCpnIndex)->uchCouponStatus[A161_STS01_ADDR] |= A161_AUTO_CPN;
	}
	else {
		(lpCpnPara + unCpnIndex)->uchCouponStatus[A161_STS01_ADDR] &= ~A161_AUTO_CPN;
	}
	

    /* ----- Get Status of Addr.3,4 to CheckButton ----- */

    (lpCpnPara + unCpnIndex)->uchCouponStatus[A161_TAGSTS_ADDR] = A161GetChkbox(hDlg, IDD_A161DEP01, A161_CHKBIT2);

    /* ----- Get Mnemonic Data from EditText ----- */

    DlgItemGetText(hDlg, IDD_A161MNEMO, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Replace Double Width Key ('~' -> 0x12) ----- */

    PepReplaceMnemonic(szWork, (lpCpnPara + unCpnIndex)->aszCouponMnem, (PARA_CPN_LEN ), PEP_MNEMO_WRITE);

    /* ----- Create Description for Insert ListBox ----- */

    wsPepf(szDesc, WIDE("%2u : %s"), unCpnIndex + 1, szWork);

    /* ----- Delete Old Description from Department ListBox ----- */

    SendDlgItemMessage(hDlg, IDD_A161CPN_LIST, LB_DELETESTRING, (WPARAM)unCpnIndex, 0L);

    /* ----- Insert Created Description to Department ListBox ----- */

    DlgItemSendTextMessage(hDlg, IDD_A161CPN_LIST, LB_INSERTSTRING, (WPARAM)unCpnIndex, (LPARAM)(szDesc));

    /* ----- Set HighLight Bar to the Top Index of ListBox ----- */

    SendDlgItemMessage(hDlg, IDD_A161CPN_LIST, LB_SETCURSEL, (WPARAM)unCpnIndex, 0L);

    /* ----- Get Target Data from EditText ----- */

    for (wI = 0, wID = IDD_A161TAG01; wI < A161_TAGSTS_CNT; wI++, wID++)
    {
        memcpy((lpCpnPara + unCpnIndex)->auchCouponTarget[wI],
                 szCouponTemp[wI],
                 OP_PLU_LEN * sizeof(WCHAR));

/*            = (UCHAR)GetDlgItemInt(hDlg, wID, NULL, FALSE); (12/04/95)*/
    }

    /* ----- Get Adjective Data from EditText ----- */

    for (wI = 0, wID = IDD_A161ADJ01; wI < A161_TAGSTS_CNT; wI++, wID++)
    {
        (lpCpnPara + unCpnIndex)->uchCouponAdj[wI] = (UCHAR)GetDlgItemInt(hDlg, wID, NULL, FALSE);
    }

     /* ----- Get Group Data from EditText ----- */

    for (wI = 0, wID = IDD_A161GRP1; wI < A161_TAGSTS_CNT; wI++, wID++)
    {
        bI = (BYTE)(GetDlgItemInt(hDlg, wID, NULL, FALSE) << 4);
          (lpCpnPara + unCpnIndex)->uchCouponAdj[wI]    |= bI;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SaveAmount()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed amount data from edittext.
* ===========================================================================
*/
VOID    A161SaveAmount(HWND hDlg)
{
    long    lWork;
    WCHAR    szWork[32];
    DWORD   dwTextLen;


    /* ----- Get Length of Inputed Preset Price ----- */

    dwTextLen = SendDlgItemMessage(hDlg, IDD_A161AMOUNT, WM_GETTEXTLENGTH, 0, 0L);

    /* ----- Get Inputed Amount Value ----- */

    DlgItemGetText(hDlg, IDD_A161AMOUNT, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Save Amount Data to Static Work Area ----- */

    lWork = _wtol(szWork);

    if (lWork < (LONG)A161_DATAMIN) {

        lWork = (LONG)A161_DATAMIN;
    }

    (lpCpnPara + unCpnIndex)->ulCouponAmount = lWork;


}
/*
* ===========================================================================
**  Synopsis    :   VOID    A161SavePriority()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed amount data from edittext.
* ===========================================================================
*/
VOID    A161SavePriority(HWND hDlg)
{
    long    lWork;
    WCHAR    szWork[32];
    


    /* ----- Get Inputed Amount Value ----- */

    DlgItemGetText(hDlg, IDD_AUTO_PRI, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Save Amount Data to Static Work Area ----- */

    lWork = _wtol(szWork);

    if (lWork > (LONG)A161_AUTOPRIMAX) {

        lWork = (LONG)A161_AUTOPRIMAX;
    }

    (lpCpnPara + unCpnIndex)->uchAutoCPNStatus = (UCHAR)lWork;


}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SaveBonus()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed amount data from edittext.
* ===========================================================================
*/
VOID    A161SaveBonus(HWND hDlg)
{
    long    lWork;
    WCHAR   szWork[32];
    
    /* ----- Get Inputed Amount Value ----- */
    DlgItemGetText(hDlg, IDD_A161BONUS, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Save Amount Data to Static Work Area ----- */
    lWork = _wtol(szWork);
    if (lWork > A161_BNSMAXVALUE) {
        lWork = A161_BNSMAXVALUE;
    }
    (lpCpnPara + unCpnIndex)->usBonusIndex |= (USHORT)lWork;

    /* ----- Get Inputed Amount Value ----- */
    DlgItemGetText(hDlg, IDD_A161SALESCODE, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Save Amount Data to Static Work Area ----- */
    lWork = _wtol(szWork);
    if (lWork > 7) {
        lWork = 7;  // maximum value for sales code is 7, range is 0 to 7. see A68_SALES_MAX
    }

	lWork |= IsDlgButtonChecked(hDlg, IDD_A161SUPRINTV_FLAG) ? 0x80 : 0;

    (lpCpnPara + unCpnIndex)->uchRestrict = (UCHAR)lWork;

}
/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetCtrlData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by current control code.
* ===========================================================================
*/
VOID    A161SetCtrlData(HWND hDlg)
{
    /* ----- Determine Current Tax Mode  ----- */

    if (bTaxMode == A161_TAX_CAN) {     /* Canadian Tax */

        /* ----- Set Canadian Tax Data to RadioButton ----- */

        A161SetCanadaBtn(hDlg);

    } else if (bTaxMode == A161_TAX_US) {            /* U.S. Tax     */

        /* ----- Set Control Code to CheckButton for Address 1 & 2 ----- */

        A161SetChkbox(hDlg, IDD_A161TAX01, bCtrlCode, A161_CHKBIT1);

    } else {                    /* International VAT */

        /* ----- Set International VAT Data to RadioButton ----- */

        A161SetVATBtn(hDlg);

    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bBitcnt     -   Bit Pattern Check count
*
**  Return      :   No return value.
*
**  Description :
*       This function checks the checkbutton by current status code of
*   selected coupon data.
* ===========================================================================
*/
VOID    A161SetChkbox(HWND hDlg, WORD wBtnID, BYTE bChkBit, BYTE bBitcnt)
{
    BYTE    bI;
    BOOL    fCheck;

    for (bI = 0; bI < A161_8BITS; bI++) {

        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */

        if (bBitcnt & (A161_BITMASK << bI)) {

            /* ----- Get Current Bit Status ----- */

            fCheck = (bChkBit & (A161_BITMASK << bI)) ? TRUE:  FALSE;

            /* ----- Check Target CheckButton ----- */

            CheckDlgButton(hDlg, wBtnID++, fCheck);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetCanadaBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by canadian tax.
* ===========================================================================
*/
VOID    A161SetCanadaBtn(HWND hDlg)
{
    WORD    wBtnID;
    BOOL    fCheck1,
            fCheck2,
            fCheck3;

    /* ----- Get Button ID of Current Canadian Tax ----- */

    wBtnID = (WORD)((bCtrlCode & A161_TAX_BIT) + IDD_A161_CAN0);

    /* ----- Check RadioButton by Current Canadian Tax ----- */

    CheckRadioButton(hDlg, IDD_A161_CAN0, IDD_A161_CAN8, wBtnID);

    /* ----- Get Status of Address 2 (Single use, Discount Item #1 / #2) ----- */

    fCheck1 = (bCtrlCode & A161_SINGLE_ITEM) ? TRUE : FALSE;
    fCheck2 = (bCtrlCode & A161_DISC_ITEM1) ? TRUE : FALSE;
    fCheck3 = (bCtrlCode & A161_DISC_ITEM2) ? TRUE : FALSE;

    /* ----- Set Status of Addr.2 to CheckButton ----- */

    CheckDlgButton(hDlg, IDD_A161SINGLE, fCheck1);
    CheckDlgButton(hDlg, IDD_A161DIS01, fCheck2);
    CheckDlgButton(hDlg, IDD_A161DIS02, fCheck3);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetVATBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by canadian tax.
* ===========================================================================
*/
VOID    A161SetVATBtn(HWND hDlg)
{
    WORD    wBtnID;
    BOOL    fCheck1,
            fCheck2,
            fCheck3;

    /* ----- Get Button ID of Current International VAT ----- */

    wBtnID = (WORD)((bCtrlCode & A161_TAX_BIT_VAT) + IDD_A161_VAT0);

    /* ----- Check RadioButton by Current International VAT ----- */

    CheckRadioButton(hDlg, IDD_A161_VAT0, IDD_A161_VAT3, wBtnID);

    /* ----- GetStatus of VAT serviceable (Addr.4 Bit 3) ----- */

    fCheck1 = (bCtrlCode & A161_TAX_BIT_SERVICE) ? TRUE : FALSE;

    /* ----- Set Status of Addr.5 to CheckButton ----- */

    CheckDlgButton(hDlg, IDD_A161_VATSERVICE, fCheck1);

    /* ----- Get Status of Address 2 (Single use, Discount Item #1 / #2) ----- */

    fCheck1 = (bCtrlCode & A161_SINGLE_ITEM) ? TRUE : FALSE;
    fCheck2 = (bCtrlCode & A161_DISC_ITEM1) ? TRUE : FALSE;
    fCheck3 = (bCtrlCode & A161_DISC_ITEM2) ? TRUE : FALSE;

    /* ----- Set Status of Addr.2 to CheckButton ----- */

    CheckDlgButton(hDlg, IDD_A161SINGLE, fCheck1);
    CheckDlgButton(hDlg, IDD_A161DIS01, fCheck2);
    CheckDlgButton(hDlg, IDD_A161DIS02, fCheck3);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A161GetCtrlData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed control code data from each controls, and
*   then it sets them to static data buffer.
* ===========================================================================
*/
VOID    A161GetCtrlData(HWND hDlg)
{
    if (bTaxMode == A161_TAX_CAN) {

        /* ----- Get Canadian Tax Data from RadioButton ----- */

        A161GetCanadaBtn(hDlg);

    } else if (bTaxMode == A161_TAX_US) {

        /* ----- Get Data from Checkbox for Address 1 & 2 ----- */

        bCtrlCode = A161GetChkbox(hDlg, IDD_A161TAX01, A161_CHKBIT1);

    } else {

        /* ----- Get International VAT Data from RadioButton ----- */

        A161GetVATBtn(hDlg);

    }

}

/*
* ===========================================================================
**  Synopsis    :   BYTE    A161GetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    *bChkBit    -   Bit Pattern without Reserved Area Address
*                   BYTE    bBitcnt     -   Bit Pattern Cheak count
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed control code data from checkbox.
* ===========================================================================
*/
BYTE    A161GetChkbox(HWND hDlg, WORD wBtnID, BYTE bBitcnt)
{
    BYTE    bI;
    BYTE    bBottun;

    /* ----- Reset Target Control Code Area without Reserved Area ----- */

    bBottun = 0;

    for (bI = 0; bI < A161_8BITS; bI++) {

        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */

        if (bBitcnt & (A161_BITMASK << bI)) {

            /* ----- Get Data from CheckBox ----- */

            if (IsDlgButtonChecked(hDlg, wBtnID++) == TRUE) {

                bBottun |= (A161_BITMASK << bI);
            }
        }
    }

    return bBottun;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161GetCanadaBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed canadian tax data from radiobutton. And
*   then it sets them to static control code data buffer.
* ===========================================================================
*/
VOID    A161GetCanadaBtn(HWND hDlg)
{
    BYTE    bCanTax;
    UINT    unCheck;

    /* ----- Reset Address 1 & 2 Area without Reserved Area ----- */

    bCtrlCode &= A161_TAX_MASK;

    for (bCanTax = 0; bCanTax <= A161_TAX_MAX; bCanTax++) {

        /* ----- Determine Whether Target Button is Checked or Not ----- */

        unCheck = IsDlgButtonChecked(hDlg, (int)(IDD_A161_CAN0 + bCanTax));

        if (unCheck == TRUE) {

            break;
        }
    }

    /* ----- Set Inputed Canadian Tax Data to Data Buffer ----- */

    bCtrlCode |= bCanTax;

    /* ----- Determine Single use Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161SINGLE) == TRUE) {

        /* ----- Set Single use Data to Buffer ----- */

        bCtrlCode |= A161_SINGLE_ITEM;
    }

    /* ----- Determine Discount Item #1 Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161DIS01) == TRUE) {

        /* ----- Set Discount Item #1 Data to Buffer ----- */

        bCtrlCode |= A161_DISC_ITEM1;
    }

    /* ----- Determine Discount Item #2 Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161DIS02) == TRUE) {

        /* ----- Set Discount Item #2 Data to Buffer ----- */

        bCtrlCode |= A161_DISC_ITEM2;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161GetVATBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed canadian tax data from radiobutton. And
*   then it sets them to static control code data buffer.
* ===========================================================================
*/
VOID    A161GetVATBtn(HWND hDlg)
{
    BYTE    bVAT;
    UINT    unCheck;

    /* ----- Reset Address 1 & 2 Area without Reserved Area ----- */

    bCtrlCode &= A161_TAX_MASK;

    for (bVAT = 0; bVAT < A161_TAX_MAX; bVAT++) {

        /* ----- Determine Whether Target Button is Checked or Not ----- */

        unCheck = IsDlgButtonChecked(hDlg, (int)(IDD_A161_VAT0 + bVAT));

        if (unCheck == TRUE) {

            break;
        }
    }

    /* ----- Set Inputed Canadian Tax Data to Data Buffer ----- */

    bCtrlCode |= bVAT;

    /* ----- Determine VAT Serviceable Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161_VATSERVICE) == TRUE) {

        /* ----- Set Discount Item #1 Data to Buffer ----- */

        bCtrlCode |= A161_TAX_BIT_SERVICE;
    }

    /* ----- Determine Single use Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161SINGLE) == TRUE) {

        /* ----- Set Single use Data to Buffer ----- */

        bCtrlCode |= A161_SINGLE_ITEM;
    }

    /* ----- Determine Discount Item #1 Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161DIS01) == TRUE) {

        /* ----- Set Discount Item #1 Data to Buffer ----- */

        bCtrlCode |= A161_DISC_ITEM1;
    }

    /* ----- Determine Discount Item #2 Data from CheckBox ----- */

    if (IsDlgButtonChecked(hDlg, IDD_A161DIS02) == TRUE) {

        /* ----- Set Discount Item #2 Data to Buffer ----- */

        bCtrlCode |= A161_DISC_ITEM2;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetAutoData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected coupon data to each contols.
* ===========================================================================
*/
VOID    A161SetAutoData(HWND hDlg)
{
    LPCPNPARA   lpCpn;
    WCHAR       szWork[32];
	UINT		autoCoup;



    // ----- Get Current Selected Index of ListBox ----- 

   

	autoCoup = SendDlgItemMessage(hDlg,IDD_A161CPN_LIST,LB_GETCURSEL,0, 0);

	// ----- Set Address of Current Selected Coupon Data to Work Area ----- 
	lpCpn = (LPCPNPARA)(lpCpnPara + autoCoup);
	
	// Fix for SR 1005- Adding enforcement of min/max for Coupon Priority - CSMALL
	if (lpCpn->uchAutoCPNStatus < A161_PRIORITYMIN || lpCpn->uchAutoCPNStatus > A161_PRIORITYMAX)
	{
		lpCpn->uchAutoCPNStatus = A161_PRIORITYMIN;
	}
	// end SR 1005 fix

	//wcscpy((WCHAR *)szWork, (WCHAR *)lpCpn->uchAutoCPNStatus);
	 _ultow(( ULONG)(lpCpn->uchAutoCPNStatus), szWork, A161_RADIX_10);

	DlgItemRedrawText(hDlg, IDD_AUTO_PRI, szWork);

	
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetAutoData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected coupon data to each contols.
* ===========================================================================
*/
VOID A161AutoCouponCheck(HWND hDlg, USHORT unMaxCpn){
	WCHAR       szCap[PEP_CAPTION_LEN],
                szMsg[256];
	LPCPNPARA   lpCpn;
	USHORT		unNumAutoCpn = 0, unI;

	for (unI = 1, lpCpn = lpCpnPara; unI <= unMaxCpn; unI++, lpCpn++) {
		if(lpCpn->uchCouponStatus[A161_STS01_ADDR] & A161_AUTO_CPN) {
			unNumAutoCpn++;
		}
	}
	if(unNumAutoCpn > A161_AUTOPRIMAX)
		{
			 //----- Load String from Resource ----- 

			LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap, PEP_STRING_LEN_MAC(szCap));

			LoadString(hResourceDll, IDS_A161_AUTOLIMIT, szMsg, PEP_STRING_LEN_MAC(szMsg));

			 //----- Display Error Message ----- 

			MessageBeep(MB_ICONEXCLAMATION);
			MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
		return;
		}
	
}
/*
* ===========================================================================
**  Synopsis    :   VOID    A161InitList()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unMaxCpn    -   Max. Value of Coupon Records
*
**  Return      :   No return value.
*
**  Description :
*       This function initializes the configulation of a coupon listbox.
* ===========================================================================
*/
VOID    A161InitAutoList(HWND hDlg , USHORT unMaxCpn)
{
    LPCPNPARA   lpCpn, lpTempCheck = lpCpnPara;
    USHORT      unI, iCount = 0, cCount = 0, aCount = 0;
    WCHAR       szWork[32],
                szDesc[32];
	WCHAR       szCap[PEP_CAPTION_LEN],
                szMsg[256];
	WCHAR		autoCoupons[25][32];
	WCHAR		*szNull = WIDE("");





    memset(&autoCoupons,0x00,sizeof(autoCoupons));
	DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_RESETCONTENT, 0, 0);

	    for (unI = 1, lpCpn = lpCpnPara; unI <= unMaxCpn; unI++, lpCpn++) {
			if(cCount > A161_AUTOPRIMAX)
			{
				 //----- Load String from Resource ----- 

				LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap, PEP_STRING_LEN_MAC(szCap));

				LoadString(hResourceDll, IDS_A161_AUTOLIMIT, szMsg, PEP_STRING_LEN_MAC(szMsg));

				 //----- Display Error Message ----- 

				MessageBeep(MB_ICONEXCLAMATION);
				MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			if(lpCpn->uchCouponStatus[A161_STS01_ADDR] & A161_AUTO_CPN) {

				if((lpCpn->uchAutoCPNStatus == 0)||(lpCpn->uchAutoCPNStatus > 25)||(lpCpn->uchAutoCPNStatus == cCount)){
					if(!(lpTempCheck->uchAutoCPNStatus == cCount+1)){
						lpCpn->uchAutoCPNStatus = (UCHAR)cCount+1;
					}
					else{
						lpCpn->uchAutoCPNStatus = lpTempCheck->uchAutoCPNStatus + 1;
					}

				}
					//----- Replace Double Width Key (0x12 -> '~') ----- 

				memset(szWork, 0x00, sizeof(szDesc));
				PepReplaceMnemonic(lpCpn->aszCouponMnem, szWork, (PARA_CPN_LEN ), PEP_MNEMO_READ);

				// ----- Create Description for Insert ListBox ----- 
				memset(szDesc, 0x00, sizeof(szDesc));
				wsPepf(szDesc, WIDE("%2u : %s"), unI, szWork);


				/* ----- Set Created Description to ListBox ----- */
				if(0 == wcscmp(autoCoupons[((USHORT)(lpCpn->uchAutoCPNStatus))-1],szNull)){
					wcscpy(autoCoupons[((USHORT)(lpCpn->uchAutoCPNStatus))-1],szDesc);
					lpTempCheck = lpCpn;
					cCount++;
				} else {
					wcscpy(autoCoupons[((USHORT)(lpCpn->uchAutoCPNStatus))],szDesc);
					lpCpn->uchAutoCPNStatus += 1;
					lpTempCheck = lpCpn;
					cCount++;
				}

			}

	    }

		DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_RESETCONTENT, 0, 0);

		for(unI = 0; unI < cCount; unI++){
			if(!wcscmp(autoCoupons[aCount],szNull)){
				aCount++;
				DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (USHORT)unI, (LPARAM)autoCoupons[aCount]);
				aCount++;
				continue;
			} else {
				DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (USHORT)unI, (LPARAM)autoCoupons[aCount]);
				aCount++;
			}

		}

		iCount = (USHORT)DlgItemSendTextMessage(hDlg,IDD_A161_AUTO_CPN_LIST,LB_GETCOUNT,0,0);

		memset(szDesc, 0, sizeof(szDesc));
		memset(szWork, 0, sizeof(szWork));

		/* ----- Load String from Resource ----- */

		LoadString(hResourceDll, IDS_A161_NOAUTO, szWork, PEP_STRING_LEN_MAC(szWork));

		/* ----- Create Max. Record Number Description ----- */

		wsPepf(szDesc, szWork, iCount);

	    /* ----- Set Created Description to StaticText ----- */

    DlgItemRedrawText(hDlg, IDD_A161AUTOMAX, szDesc);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A161SetAutoData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected coupon data to each contols.
* ===========================================================================

VOID    A161SetAutoData(HWND hDlg)
{
    LPCPNPARA   lpCpn;
    WCHAR       szWork[32];
	UINT		autoCoup, autoPri, unI;



    // ----- Get Current Selected Index of ListBox ----- 

    autoPri = (USHORT)DlgItemSendTextMessage(hDlg,IDD_A161_AUTO_CPN_LIST,LB_GETCOUNT,0,0);

	for (unI = 0; unI <= autoPri; unI++) {


		SendDlgItemMessage(hDlg,
					IDD_A161_AUTO_CPN_LIST,
					LB_GETTEXT,
					(WPARAM)unI,
					(LPARAM)szWork);

		autoCoup = atoi((const char *)&szWork[0]);

		// ----- Set Address of Current Selected Coupon Data to Work Area ----- 

		lpCpn = (LPCPNPARA)(lpCpnPara + autoCoup-1);

		lpCpn->uchAutoCPNStatus = (UCHAR)(unI+1);

	}
}
*/
/*
* ===========================================================================
**  Synopsis    :   VOID    A161SaveAutoPri()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed amount data from edittext.
* ===========================================================================
*/
VOID	A161AutoPriUp(HWND hDlg)
{
	UINT tempIndex = 0;
	WCHAR szTemp[32], currentSel[32];

	memset(szTemp,0,sizeof(szTemp));
	memset(currentSel,0,sizeof(currentSel));
	tempIndex = (UINT)SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETCURSEL, 0, 0);
	if(tempIndex <= A161_AUTOPRIMAX){
		if(tempIndex != 0){
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETTEXT, (WPARAM)tempIndex-1, (LPARAM)szTemp);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETTEXT, (WPARAM)tempIndex, (LPARAM)currentSel);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_DELETESTRING, (WPARAM)tempIndex, 0);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_DELETESTRING, (WPARAM)tempIndex-1, 0);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (WPARAM)tempIndex-1, (LPARAM)currentSel);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (WPARAM)tempIndex, (LPARAM)szTemp);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_SETCURSEL, (WPARAM)tempIndex-1, 0L);
		}
	}



}
/*
* ===========================================================================
**  Synopsis    :   VOID    A161SaveAutoPri()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed amount data from edittext.
* ===========================================================================
*/
VOID	A161AutoPriDown(HWND hDlg)
{

	UINT tempIndex = 0, nextIndex = 0, maxCoup = 0;
	WCHAR szTemp[32], currentSel[32];


	memset(szTemp,0,sizeof(szTemp));
	memset(currentSel,0,sizeof(currentSel));
	tempIndex = (UINT)SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETCURSEL, 0, 0);

	maxCoup = (UINT)DlgItemSendTextMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETCOUNT, 0, 0);

	nextIndex = tempIndex + 1;
	if(tempIndex < (maxCoup-1)){
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETTEXT, (WPARAM)(nextIndex), (LPARAM)szTemp);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_GETTEXT, (WPARAM)tempIndex, (LPARAM)currentSel);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_DELETESTRING, (WPARAM)(nextIndex), 0);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_DELETESTRING, (WPARAM)tempIndex, 0);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (WPARAM)tempIndex, (LPARAM)szTemp);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_INSERTSTRING, (WPARAM)(nextIndex), (LPARAM)currentSel);
		SendDlgItemMessage(hDlg, IDD_A161_AUTO_CPN_LIST, LB_SETCURSEL, (WPARAM)tempIndex+1, 0L);
	}

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A161Del()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return values.
*
**  Description :
*       This function clears the fields of the current selected Coupon.
* ===========================================================================
*/
VOID    A161Del(HWND hDlg) {

    DlgItemRedrawText(hDlg, IDD_A161AMOUNT, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161MNEMO, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG01, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG02, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG03, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG04, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG05, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG06, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161TAG07, (LPCWSTR)"");
	DlgItemRedrawText(hDlg, IDD_A161ADJ01, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ02, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ03, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ04, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ05, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ06, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161ADJ07, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP1, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP2, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP3, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP4, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP5, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP6, (LPCWSTR)"0");
	DlgItemRedrawText(hDlg, IDD_A161GRP7, (LPCWSTR)"0");
		
	EnableWindow( GetDlgItem(hDlg, IDD_AUTO_PRI) ,FALSE);
	CheckDlgButton(hDlg, IDD_A161ACCFLG, FALSE);
	DlgItemRedrawText(hDlg, IDD_AUTO_PRI, (LPCWSTR)"");

	CheckDlgButton(hDlg, IDD_A161DEP01, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP02, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP03, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP04, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP05, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP06, FALSE);
	CheckDlgButton(hDlg, IDD_A161DEP07, FALSE);

	CheckDlgButton(hDlg, IDD_A161_VER1, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER2, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER3, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER4, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER5, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER6, FALSE);
	CheckDlgButton(hDlg, IDD_A161_VER7, FALSE);

	bCtrlCode = 0;
}
/* ===========================================================================
**  Synopsis    :   VOID    A161ChkPriorityRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*                   UINT    unMax   -   Max. Value of Input Data
*
**  Return      :   No return value.
*
**  Description :
*       This function checks whether the inputed data is out of range or not.
*   It displays a error message by messagebox, if the inputed data is invalid.
* ===========================================================================
* Added for SR 1005 - CSMALL
*/
VOID    A161ChkPriorityRng(HWND hDlg, WORD wEditID)
{
    UINT    unValue;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szWork[PEP_OVER_LEN],
            szMsg[PEP_OVER_LEN];

    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);

    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */

    if ( (unValue > A161_PRIORITYMAX || unValue < A161_PRIORITYMIN) && IsDlgButtonChecked(hDlg, IDD_A161ACCFLG)) {

        /* ----- Load Error Message / Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A161, szCap,  PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_PEP_OVERRANGE,    szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szMsg, szWork, A161_PRIORITYMIN, A161_PRIORITYMAX);

        /* ----- Display Error Message by MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error Occurred EditText ----- */

        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL,0,2);
    }
}


HWND A161ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A161),
                                   hParentWnd,
                                   A161DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A161ModeLessDialogWnd (void)
{
	return hWndModeLess;
}


/* ===== End of File (A161.C) ===== */