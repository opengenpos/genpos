/*              
* ---------------------------------------------------------------------------
* Title         :   Set PPI Table (AC 71)
* Category      :   Maintenance, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   A071.C
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Feb-02-96 : 03.00.05 : M.Suzuki   : Initial
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
#include    <Windows.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a071.h"

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
static  PPIIF   PPIData;
static  USHORT  usCrtIndex,
                usPPIMax;

static  HWND  hWndModeLess = NULL;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

static  USHORT  InitDialog(HWND);
static  void    SetInitData(HWND, WPARAM);
static  BOOL    GetQtyPrc(HWND);
static  USHORT  CheckQtyPrc(LPBYTE, LPDWORD, LPWORD);
static  USHORT  CompPPI(LPBYTE, LPDWORD, UCHAR, LPWORD);
static  BOOL    CheckRange(HWND, WPARAM);
static  void    DispError(HWND, LPCWSTR);
static  void    MakeEditDlg(HWND);
static  void    SetNewDesc(HWND);
static  void    ClearPPI(HWND);
static  BOOL    SpinProc(HWND, WPARAM, LPARAM);


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  A071DlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialgbox procedure for
*                   Set PPI Table of PLU (AC 71)
* ===========================================================================
*/
BOOL    WINAPI  A071DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WCHAR    szFile[PEP_CAPTION_LEN],
            szCap[PEP_CAPTION_LEN];

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Read Current No. of PLU Memory (Prog. #2) ----- */

        if (InitDialog(hDlg)) {         /* PLU File is Created */

            /* ----- Load PPI File Name from Resorece ----- */

            LoadString(hResourceDll, IDS_FILE_NAME_PPI, szCap, PEP_STRING_LEN_MAC(szCap));

            /* ----- Create Backup File for Cancel Key ----- */
    
            PepFileBackup(hDlg, szCap, PEP_FILE_BACKUP);

            /* ----- Initialize Configlation of DialogBox ----- */

            SetInitData(hDlg, IDD_A71);


        } else {                        /* PLU Fiel is not Created */

			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_A71_STRADDR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_CHANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A71_LIST:
            if (HIWORD(wParam) == LBN_DBLCLK) {

                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A71_CHANGE, BN_CLICKED), 0L);

                return TRUE;
            }
            return FALSE;

        case IDD_A71_CHANGE:
            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Get Current Selected Index of ListBox ----- */

                usCrtIndex = (USHORT)SendDlgItemMessage(hDlg,
                                                        IDD_A71_LIST,
                                                        LB_GETCURSEL,
                                                        0, 0L);
                /* ----- Make Edit DialogBox ----- */

                MakeEditDlg(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (LOWORD(wParam) == IDOK) {  /* User Selection is OK Button */

                /* ----- Modify File Flag ----- */

                PepSetModFlag(hwndActMain, PEP_MF_PPI, 0);

            } else {                /* User Selection is Cancel Button */

                LoadString(hResourceDll, IDS_FILE_NAME_PPI, szFile, PEP_STRING_LEN_MAC(szFile));

                PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
            }

            /* ----- Delete Temporary File ----- */

            PepFileDelete();

            EndDialog(hDlg, LOWORD(wParam));

            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

// WARNING:  The standard dialog proc of PPI Edit makes a back of the PLU file
//           which causes the PLU file to become corrupted if AC71 Modeless dialog
//           for PPI is used from the AC68 PLU dialog.
//           See use of functions PepFileBackup(), PepFileDelete(), etc.
//                Richard Chambers, Oct-15-2018
BOOL    WINAPI  A071DlgProcModeless(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Read Current No. of PLU Memory (Prog. #2) ----- */
        if (InitDialog(hDlg)) {         /* PLU File is Created */
            /* ----- Initialize Configlation of DialogBox ----- */
            SetInitData(hDlg, IDD_A71);
        } else {                        /* PLU Fiel is not Created */
			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_A71_STRADDR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_CHANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A71_LIST:
            if (HIWORD(wParam) == LBN_DBLCLK) {
                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A71_CHANGE, BN_CLICKED), 0L);
                return TRUE;
            }
            return FALSE;

        case IDD_A71_CHANGE:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Get Current Selected Index of ListBox ----- */
                usCrtIndex = (USHORT)SendDlgItemMessage(hDlg, IDD_A71_LIST, LB_GETCURSEL, 0, 0L);
                /* ----- Make Edit DialogBox ----- */
                MakeEditDlg(hDlg);
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (LOWORD(wParam) == IDOK) {  /* User Selection is OK Button */
                /* ----- Modify File Flag ----- */
                PepSetModFlag(hwndActMain, PEP_MF_PPI, 0);
            }
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  A071EditDlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialgbox procedure for
*                   Change PPI Table of PLU.
* ===========================================================================
*/
BOOL    WINAPI  A071EditDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN];
    BOOL    fRet = FALSE;
	BOOL	priState = 0;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Set Initial Data to Each Position ----- */

        SetInitData(hDlg, IDD_A71EDIT);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A71_QTY01; j<=IDD_A71_PRC40; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A71_GROUP; j<=IDD_A71_ADDRESS40; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}

			SendDlgItemMessage(hDlg, IDD_A71_ADVOPTIONS, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_AFTERMAX, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_COMBO_AFTERMAX, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_BEFOREQTY, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_MINTOTALSALE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_EDIT_MINTOTALSALE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_PRICEOPTIONS, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_CHK_PRICEDISCPREM, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_CHK_DISCOUNTPLU, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A71_CHK_PREMIUMPLU, WM_SETFONT, (WPARAM)hResourceFont, 0);

			SendDlgItemMessage(hDlg, IDD_A71_CLEAR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Spin button control ----- */

        SpinProc(hDlg, wParam, lParam);

        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_A71_QTY01:
        case IDD_A71_QTY02:
        case IDD_A71_QTY03:
        case IDD_A71_QTY04:
        case IDD_A71_QTY05:
        case IDD_A71_QTY06:
        case IDD_A71_QTY07:
        case IDD_A71_QTY08:
        case IDD_A71_QTY09:
        case IDD_A71_QTY10:
        case IDD_A71_QTY11:
        case IDD_A71_QTY12:
        case IDD_A71_QTY13:
        case IDD_A71_QTY14:
        case IDD_A71_QTY15:
        case IDD_A71_QTY16:
        case IDD_A71_QTY17:
        case IDD_A71_QTY18:
        case IDD_A71_QTY19:
        case IDD_A71_QTY20:
        case IDD_A71_QTY21: // adding 20 more PPI addresses
        case IDD_A71_QTY22:
        case IDD_A71_QTY23:
        case IDD_A71_QTY24:
        case IDD_A71_QTY25:
        case IDD_A71_QTY26:
        case IDD_A71_QTY27:
        case IDD_A71_QTY28:
        case IDD_A71_QTY29:
        case IDD_A71_QTY30:
        case IDD_A71_QTY31:
        case IDD_A71_QTY32:
        case IDD_A71_QTY33:
        case IDD_A71_QTY34:
        case IDD_A71_QTY35:
        case IDD_A71_QTY36:
        case IDD_A71_QTY37:
        case IDD_A71_QTY38:
        case IDD_A71_QTY39:
        case IDD_A71_QTY40:
            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Data Range ----- */

                if (CheckRange(hDlg, wParam)) {

                    /* ----- Make Error Message ----- */

                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

                    wsPepf(szMessage,
                             szWork,
                             A71_QTY_MIN, A71_QTY_MAX);

                    /* ----- Display Error Message ----- */

                    DispError(hDlg, szMessage);

                    /* ----- Set High-Light Bar and Input Focus to EditText ----- */

                    SendMessage((HWND)lParam, EM_UNDO, 0, 0L);

                    SendDlgItemMessage(hDlg,
                                       wParam,
                                       EM_SETSEL,
                                       0, MAKELONG(0, -1));

                    SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A71_CLEAR:
            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Clear PPI Table of Selected Index ----- */

                ClearPPI(hDlg);

                return TRUE;
            }
            return FALSE;

		case IDD_A71_EDIT_MINTOTALSALE:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				DlgItemGetText(hDlg, IDD_A71_EDIT_MINTOTALSALE, szWork, sizeof(szWork));
				
				if ( _wtol(szWork) > A71_PRC_MAX || _wtol(szWork) < A71_PRC_MIN )
				{
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

                    wsPepf(szMessage,
                             szWork,
                             A71_PRC_MIN, A71_PRC_MAX);

					DispError(hDlg, szMessage);

                    SendMessage((HWND)lParam, EM_UNDO, 0, 0L);

                    SendDlgItemMessage(hDlg,
                                       wParam,
                                       EM_SETSEL,
                                       0, MAKELONG(0, -1));

                    SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
				}
				return TRUE;
			}
			return FALSE;
		
			//---------------------------------------------------------------------------------
			// Manage the check boxes for the pricing override and discount and premium.
			// these are exclusive check boxes meaning the user must choose one or the other.
		case IDD_A71_CHK_PRICEDISCPREM:
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				if (BST_CHECKED == SendDlgItemMessage(hDlg, IDD_A71_CHK_DISCOUNTPLU, BM_GETSTATE, 0, 0L)) {
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_UNCHECKED, 0);
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0);
					EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), FALSE); 
					EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), FALSE);
				} else {
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_CHECKED, 0);
					EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), TRUE); 
					EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), TRUE);
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_CHECKED, 0); 
					SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			 }
			break;

		case IDD_A71_CHK_DISCOUNTPLU:
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_CHECKED, 0); 
//				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_OVERRIDEPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
			 }
			break;

		case IDD_A71_CHK_PREMIUMPLU:
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_CHECKED, 0); 
//				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_OVERRIDEPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
				SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
			 }
			break;
			//---------------------------------------------------------------------------------------

        case IDOK:

            /* ----- Get PPI Table Data (Quantity & Price) ----- */

            if (GetQtyPrc(hDlg)) {

                return TRUE;

            } else {

                OpPpiAssign(&PPIData, 0);

                fRet = TRUE;
            }
            /* ----- break; ----- Not used */

        case IDCANCEL:

            EndDialog(hDlg, LOWORD(wParam));

            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   static  USHORT  InitDialog();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*
**  Return  :   Number of PLU Files. (Prog. #2)
*
**  Description:
*       This is a dialgbox procedure for
*                   initialize DialogBox Configulation.
* ===========================================================================
*/
static  USHORT  InitDialog(HWND hDlg)
{
    PARAFLEXMEM FlexData;
    WCHAR        szMsg[A71_BUFF_LEN];

    /* ----- Set address to PPI File ----- */

    FlexData.uchAddress = FLEX_PPI_ADR;

    /* ----- Read the filexible memory data of PLU File ----- */

    ParaFlexMemRead(&FlexData);

    /* if (FlexData.usRecordNumber == 0) {  NCR2172 */ /* Not yet be set the number of PLU */
    if (FlexData.ulRecordNumber == 0L) { /* Not yet be set the number of PLU */

        /* ----- Load string from resource ----- */

        LoadString(hResourceDll, IDS_A71_FLEX_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display caution message ----- */

        DispError(hDlg, szMsg);
    }
    /*  NCR2172
    usPPIMax = FlexData.usRecordNumber;
    return (FlexData.usRecordNumber);
    */
    usPPIMax = (USHORT)FlexData.ulRecordNumber;
    return ((USHORT)FlexData.ulRecordNumber);
}

/*
* ===========================================================================
**  Synopsis:   static  void    SetInitData();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               WPARAM  wInitID -   DialogBox ID for Initialize
*
**  Return  :   No Return Value.
*
**  Description:
*       This function reads PPI file and set PPI data to each position.
* ===========================================================================
*/
static  void    SetInitData(HWND hDlg, WPARAM wInitID)
{
    UINT    unAddr,
            unCount,
            unNumber,
			unIndex;
    WCHAR    szDesc[A71_BUFF_LEN],
            szWork[A71_BUFF_LEN];
    WORD    wQtyID, wPrcID;

    if (LOWORD(wInitID) == IDD_A71) {       /* initialize Main DialogBox */

        /* ----- set initial description, V3.3 ----- */

        LoadString(hResourceDll, IDS_A71_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);

        LoadString(hResourceDll, IDS_A71_CHANGE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_CHANGE, szDesc);

        /* ----- Set Number of Records to StaticText ----- */

        LoadString(hResourceDll, IDS_A71_NORECORDS, szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szDesc, szWork, usPPIMax);

        DlgItemRedrawText(hDlg, IDD_A71_NORECORDS, szDesc);

        for (unCount = 0; unCount < usPPIMax; unCount++) {

            /* ----- Load String from Resource ----- */

            LoadString(hResourceDll, IDS_A71_DESC, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Make Description ----- */

            wsPepf(szDesc, szWork, (UINT)(unCount + 1));

            /* ----- Insert String to Listbox -----*/

            DlgItemSendTextMessage(hDlg,
                               IDD_A71_LIST,
                               LB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)(szDesc));
        }

        /* ----- Set High-Light Bar to ListBox ----- */

        SendDlgItemMessage(hDlg, IDD_A71_LIST, LB_SETCURSEL, 0, 0L);

        /* ----- Set Input Focus to ListBox ----- */

        SetFocus(GetDlgItem(hDlg, IDD_A71_LIST));

    } else {                        /* Initialize Sub-DialogBox */

        /* ----- set initial description, V3.3 ----- */

        LoadString(hResourceDll, IDS_A71_EDITCAP, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);

        LoadString(hResourceDll, IDS_A71_STRADDR, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRADDR, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTYLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTYLEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICELEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICELEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRADDR2, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRADDR2, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY2, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY2, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE2, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE2, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY2LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY2LEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE2LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE2LEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRADDR3, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRADDR3, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY3, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY3, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE3, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE3, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY3LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY3LEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE3LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE3LEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRADDR4, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRADDR4, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY4, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY4, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE4, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE4, szDesc);
        LoadString(hResourceDll, IDS_A71_STRQTY4LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRQTY4LEN, szDesc);
        LoadString(hResourceDll, IDS_A71_STRPRICE4LEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_STRPRICE4LEN, szDesc);

        LoadString(hResourceDll, IDS_A71_CLEAR, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_CLEAR, szDesc);

		/* ----- Load String for Insert ComboBox ----- */

        // PPI Enhancments - CSMALL 6/7/2006
		LoadString(hResourceDll, IDS_A71_STAFTERMAX0, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemSendTextMessage(hDlg, IDD_A71_COMBO_AFTERMAX, CB_INSERTSTRING,
			-1, (LPARAM)(szDesc));
        LoadString(hResourceDll, IDS_A71_STAFTERMAX1, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemSendTextMessage(hDlg, IDD_A71_COMBO_AFTERMAX, CB_INSERTSTRING,
			-1, (LPARAM)(szDesc));

        LoadString(hResourceDll, IDS_A71_STBEFOREQTY0, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemSendTextMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, CB_INSERTSTRING,
			-1, (LPARAM)(szDesc));
        LoadString(hResourceDll, IDS_A71_STBEFOREQTY1, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemSendTextMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, CB_INSERTSTRING,
			-1, (LPARAM)(szDesc));
		
		LoadString(hResourceDll, IDS_A71_CHK_OVERRIDEPLU, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_CHK_OVERRIDEPLU, szDesc);
		LoadString(hResourceDll, IDS_A71_CHK_DISCOUNTPLU, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_CHK_DISCOUNTPLU, szDesc);
		LoadString(hResourceDll, IDS_A71_CHK_PREMIUMPLU, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A71_CHK_PREMIUMPLU, szDesc);

		/* ----- Set Description to GroupBox ----- */

        LoadString(hResourceDll, IDS_A71_GROUP, szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szDesc, szWork, (UINT)(usCrtIndex + 1));

        DlgItemRedrawText(hDlg, IDD_A71_GROUP, szDesc);

        /* ----- Initialize Static Area ----- */

        memset(&PPIData, 0, sizeof(PPIIF));
    
        /* ----- Read Initial PPI Data from PPI File ----- */

        //PPIData.uchPpiCode = (UCHAR)(usCrtIndex + 1);
		PPIData.uchPpiCode = (USHORT)(usCrtIndex + 1); //ESMITH

        OpPpiIndRead(&PPIData, 0);

		// PPI Enhancments - CSMALL 6/7/2006
        unIndex = PPIData.ParaPpi.ucAfterMaxReached;
		SendDlgItemMessage(hDlg, IDD_A71_COMBO_AFTERMAX, CB_SETCURSEL, (WPARAM)unIndex, 0L);

		unIndex = PPIData.ParaPpi.ucBeforeInitialQty;
		SendDlgItemMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, CB_SETCURSEL, (WPARAM)unIndex, 0L);

		_ultow(PPIData.ParaPpi.ulMinTotalSale, szWork, A71_CONVERT);
		DlgItemRedrawText(hDlg, IDD_A71_EDIT_MINTOTALSALE, szWork);

//	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_OVERRIDEPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
		if (PPIData.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUDISCOUNT) {
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_CHECKED, 0);
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), TRUE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), TRUE);
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_CHECKED, 0); 
		}
		else if (PPIData.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUPREMIUM) {
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_CHECKED, 0);
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), TRUE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), TRUE);
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_CHECKED, 0); 
		} else {
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
			SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0);
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), FALSE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), FALSE);
		}

        unAddr = A71_GET_ADDR_MIN(usCrtIndex);
        wQtyID = IDD_A71_QTY01;
        wPrcID = IDD_A71_PRC01;

        for (unCount = 0; unCount < A71_TABLE_MAX; unCount++, wQtyID++, wPrcID++) {
                                     
            /* ----- Set Limit Length of EditText ----- */
                                     
            SendDlgItemMessage(hDlg, wQtyID, EM_LIMITTEXT, A71_QTY_LEN,   0L);

            SendDlgItemMessage(hDlg, wPrcID, EM_LIMITTEXT, A71_PRC_LEN, 0L);

            /* ----- Make Quantity & Price Data ----- */

            unNumber = (UINT)PPIData.ParaPpi.PpiRec[unCount].uchQTY;

            _ultow(PPIData.ParaPpi.PpiRec[unCount].ulPrice, szWork, A71_CONVERT);

            /* ----- Set Quantity Data to EditText ----- */

            SetDlgItemInt(hDlg, wQtyID, unNumber, FALSE);

            /* ----- Set Price Data to EditText ----- */

            DlgItemRedrawText(hDlg, wPrcID, szWork);
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    GetQtyPrc();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*
**  Return  :   TRUE    -   Inputed Data is invalid
*               FALSE   -   Inputed Data is Leagal
*
**  Description:
*       This function gets PPI Data (Quantity & Price) from each edit text.
* ===========================================================================
*/
static  BOOL    GetQtyPrc(HWND hDlg)
{
    WCHAR    szWork[A71_BUFF_LEN];
    UINT    usRet;
    UINT    unCount;
    WORD    wQtyID, wPrcID, wErrID = 0;
    BOOL    fTrans,
            fRet = FALSE;
    LONG    lWork;
    UCHAR   auchQty[A71_TABLE_MAX];
    ULONG   aulPrice[A71_TABLE_MAX];

    wQtyID = IDD_A71_QTY01;
    wPrcID = IDD_A71_PRC01;

    for (unCount = 0; unCount < A71_TABLE_MAX; unCount++, wQtyID++, wPrcID++) {

        /* ----- Get Quantity Data from EditText ----- */

        auchQty[unCount] = (UCHAR)GetDlgItemInt(hDlg, wQtyID, &fTrans, FALSE);

        /* ----- Get Price Data from EditText ----- */

        memset(szWork, 0, sizeof(szWork));
    
        DlgItemGetText(hDlg, wPrcID, szWork, sizeof(szWork));

        lWork = _wtol(szWork);

        if (lWork < A71_PRC_MIN) {

            lWork = A71_PRC_MIN;
        }
        aulPrice[unCount] = (ULONG)lWork;
    }

    /* ----- Check Quantity & Price Table in Order ----- */

    usRet = CheckQtyPrc((LPBYTE)auchQty,
                        (LPDWORD)aulPrice,
                        (LPWORD)&wErrID);

    if (! usRet) {

        wQtyID = IDD_A71_QTY01;
        wPrcID = IDD_A71_PRC01;

        for (unCount = 0; unCount < A71_TABLE_MAX; unCount++) {

            /* ----- Get PPI Table Data (Quantity & Price) to Buffer ----- */

            PPIData.ParaPpi.PpiRec[unCount].uchQTY = auchQty[unCount];
            PPIData.ParaPpi.PpiRec[unCount].ulPrice = aulPrice[unCount];
        }

    } else {

        /* ----- Load Error Message String from Resource ----- */

        LoadString(hResourceDll, usRet, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Display Error Message ----- */

        DispError(hDlg, szWork);

        /* ----- Set High-Light Bar and Input Focus to EditText ----- */

        SendDlgItemMessage(hDlg, wErrID, EM_SETSEL, 0, MAKELONG(0, -1));

        SetFocus(GetDlgItem(hDlg, wErrID));

        fRet = TRUE;
    }

	// PPI Enhancments - CSMALL 6/7/2006
	PPIData.ParaPpi.ucAfterMaxReached = (UCHAR)SendDlgItemMessage(hDlg, IDD_A71_COMBO_AFTERMAX, CB_GETCURSEL, 0, 0L);
	if (PPIData.ParaPpi.ucAfterMaxReached == 255)
	{
		PPIData.ParaPpi.ucAfterMaxReached = 0;
	}

	PPIData.ParaPpi.ucBeforeInitialQty = (UCHAR)SendDlgItemMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, CB_GETCURSEL, 0, 0L);
	if (PPIData.ParaPpi.ucBeforeInitialQty == 255)
	{
		PPIData.ParaPpi.ucBeforeInitialQty = 0;
	}

	DlgItemGetText(hDlg, IDD_A71_EDIT_MINTOTALSALE, szWork, sizeof(szWork));
	PPIData.ParaPpi.ulMinTotalSale = _wtol(szWork);
	
	PPIData.ParaPpi.ulPpiLogicFlags01 = 0;
	//	if (BST_CHECKED == SendDlgItemMessage(hDlg, IDD_A71_CHK_OVERRIDEPLU, BM_GETSTATE, 0, 0L) {
	if (BST_CHECKED == SendDlgItemMessage(hDlg, IDD_A71_CHK_DISCOUNTPLU, BM_GETSTATE, 0, 0L)) {
		PPIData.ParaPpi.ulPpiLogicFlags01 |= PPI_LOGICFLAGS01_PLUDISCOUNT;
	} else if (BST_CHECKED == SendDlgItemMessage(hDlg, IDD_A71_CHK_PREMIUMPLU, BM_GETSTATE, 0, 0L)) {
		PPIData.ParaPpi.ulPpiLogicFlags01 |= PPI_LOGICFLAGS01_PLUPREMIUM;
	}
	return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   static  USHORT  CheckQtyPrc();
*
**  Input   :   LPBYTE  lpuchQty        -   Points to PPI Work Area (Quantity)
*               LPDWORD lpdwPrice       -   Points to PPI Work Area (Price)
*               LPWORD  lpwErrID        -   Points to Error EditText ID
*
**  Return  :   FALSE                   -   Inputed Value is Leagal
*               IDS_A71_PRC_NOT_ORDER   -   Price Data is not in order
*               IDS_A71_QTY_NOT_ORDER   -   Quantity Data is not in order
*               IDS_A71_NODATA          -   Exists 0 Data in PPI Table
*
**  Description:
*       This function checks Quantity & Price Table in Order.
* ===========================================================================
*/
static  USHORT  CheckQtyPrc(LPBYTE lpuchQty, LPDWORD lpdwPrice,
                            LPWORD lpwErrID)
{
    UINT    unCount;
    UCHAR   uchNoOfLevel = 0;
    BOOL    fSkip = FALSE;
    USHORT  usRet  = FALSE;

    for (unCount = 0; unCount < A71_TABLE_MAX; unCount++) {

        if (*(lpuchQty + unCount)) {     /* Exists Quantity Data */

            if (fSkip == TRUE) {

                usRet    = IDS_A71_NODATA;
                *lpwErrID = (WORD)(IDD_A71_QTY01 + unCount);
                break;
            }

            if (*(lpdwPrice + unCount)) {    /* Exists Price Data */

                uchNoOfLevel++;

            } else {                        /* Not Exist Price Data */

                usRet    = IDS_A71_NODATA;
                *lpwErrID = (WORD)(IDD_A71_QTY01 + unCount);
                break;
            }

        } else {                        /* Not Exists Quantity Data */

            fSkip = TRUE;

            if (*(lpdwPrice + unCount)) {      /* Exists Price Data */

                usRet    = IDS_A71_NODATA;
                *lpwErrID = (WORD)(IDD_A71_PRC01 + unCount);
                break;
            }
        }
    }

   if (usRet == FALSE) {    /* PPI Table is Leagal */

        /* ----- Compare Quantity & Price in Order ----- */

//       usRet = CompPPI(lpuchQty, lpdwPrice, uchNoOfLevel, lpwErrID);  --- User is allowed to enter data out of order *PDINU*
   }

    return (usRet);
}

/*
* ===========================================================================
**  Synopsis:   static  USHORT  CompPPI();
*
**  Input   :   LPBYTE  lpuchQty        -   Points to PPI Work Area (Quantity)
*               LPDWORD lpdwPrice       -   Points to PPI Work Area (Price)
*               UCHAR   uchNoLevel      -   No. of Level in PPI Table
*               LPWORD  lpwErrID        -   Points Error EditText ID
*
**  Return  :   FALSE                   -   Inputed Value is Leagal
*               IDS_A71_PRC_NOT_ORDER   -   Price Data is not in order
*               IDS_A71_QTY_NOT_ORDER   -   Quantity Data is not in order
*
**  Description:
*       This function compare Quantity & Price Table with Order.
* ===========================================================================
*/
static  USHORT  CompPPI(LPBYTE lpuchQty, LPDWORD lpdwPrice, UCHAR uchNoLevel,
                        LPWORD lpwErrID)
{
    USHORT  usRet = FALSE;
    UCHAR   uchCount,
            uchQty1 = 0,  uchQty2 = 0;
    ULONG   ulPrc1  = 0L, ulPrc2  = 0L;

    for (uchCount = 0; uchCount < uchNoLevel; uchCount++) {

        /* ----- Check Quantity Data in Order ----- */

        uchQty1 = uchQty2;
        uchQty2 = *(lpuchQty + uchCount);

        if (uchQty2 <= uchQty1) {

            usRet   = IDS_A71_QTY_NOT_ORDER;
            *lpwErrID = (WORD)(IDD_A71_QTY01 + uchCount);
            break;
        }

        /* ----- Check Price Data in Order ----- */

        ulPrc1 = ulPrc2;
        ulPrc2 = *(lpdwPrice + uchCount);

        if (ulPrc2 < ulPrc1) {      /* price check */

            usRet   = IDS_A71_PRC_NOT_ORDER;
            *lpwErrID = (WORD)(IDD_A71_PRC01 + uchCount);
            break;
        }
    }
    return (usRet);
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    CheckRange();
*
**  Input   :   HWND    hDlg    -   Handle of DialogBox
*               WPARAM  wID     -   ID for Inputed EditText
*
**  Return  :   TRUE    -   Inputed Value is Invalid
*               FALSE   -   Inputed Value is Leagal
*
**  Description:
*       This function checks data with range.
* ===========================================================================
*/
static  BOOL    CheckRange(HWND hDlg, WPARAM wID)
{
    BOOL    fRet = FALSE;
    UINT    unValue;

    /* ----- Get Inputed Value in EditText ----- */

    unValue = GetDlgItemInt(hDlg, LOWORD(wID), NULL, FALSE);

    if (unValue > A71_QTY_MAX) {

        fRet = TRUE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   static  void    DispError();
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
static  void    DispError(HWND hDlg, LPCWSTR lpszErrMsg)
{
    WCHAR    szCaption[PEP_CAPTION_LEN];

    /* ----- Load Strings from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A71, szCaption, PEP_CAPTION_LEN);

    /* ----- Popup MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, lpszErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis:   static  void    MakeEditDlg();
*
**  Input   :   HWND    hDlg        -   Handle of DialogBox
*
**  Return  :   No return value
*
**  Description:
*       This function makes DialogBox for edit PPI Table.
* ===========================================================================
*/
static  void    MakeEditDlg(HWND hDlg)
{
    BOOL    fRet;

    /* ----- Disable Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A71_CHANGE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);

    /* ----- Create Sub-DialogBox ----- */

    fRet = DialogBoxPopup(hResourceDll,
                     MAKEINTRESOURCEW(IDD_A71EDIT),
                     hDlg,
                     A071EditDlgProc);

    /* ----- Enable Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A71_CHANGE), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);

    /* ----- Insert New Description to ListBox ----- */

    if (fRet == TRUE) {

        SetNewDesc(hDlg);
    }
}

/*
* ===========================================================================
**  Synopsis:   static  void    SetNewDesc():
*
**  Input   :   HWND    hDlg        -   Handle of DialogBox
*
**  Return  :   No return value
*
**  Description:
*       This function replace a new descripition to ListBox.
* ===========================================================================
*/
static  void    SetNewDesc(HWND hDlg)
{
    WCHAR    szWork[A71_BUFF_LEN],
            szDesc[A71_BUFF_LEN];

    /* ----- Load Description from Resource ----- */

    LoadString(hResourceDll, IDS_A71_DESC, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Make Description for Insert ----- */

    wsPepf(szDesc, szWork, (UINT)PPIData.uchPpiCode);

    /* ----- Delete Old String from ListBox ----- */

    SendDlgItemMessage(hDlg,
                       IDD_A71_LIST,
                       LB_DELETESTRING,
                       (WORD)(usCrtIndex), 0L);

    /* ----- Insert New String to ListBox ----- */

    DlgItemSendTextMessage(hDlg,
                       IDD_A71_LIST,
                       LB_INSERTSTRING,
                       (WORD)(usCrtIndex),
                       (LPARAM)(szDesc));

    /* ----- Set High-Light Bar to Inserted Text ----- */

    SendDlgItemMessage(hDlg, IDD_A71_LIST, LB_SETCURSEL, (WORD)(usCrtIndex), 0L);
    SetFocus(GetDlgItem(hDlg, IDD_A71_LIST));
}

/*
* ===========================================================================
**  Synopsis:   static  void    ClearPPI();
*
**  Input   :   HWND    hDlg        -   Handle of DialogBox
*
**  Return  :   No return value
*
**  Description:
*       This function clears PPI Table of Selected Index of ListBox.
* ===========================================================================
*/
static  void    ClearPPI(HWND hDlg)
{
    UINT    unCount;
    WORD    wQtyID = IDD_A71_QTY01,
            wPrcID = IDD_A71_PRC01;

    /* ----- Clear Selected PPI Table ----- */

    memset(&PPIData.ParaPpi, 0, sizeof(OPPPI_PARAENTRY));

    for (unCount = 0; unCount < A71_TABLE_MAX; unCount++, wQtyID++, wPrcID++) {

        /* ----- Reset Quantity & Price Data to EditText ----- */

        SetDlgItemInt(hDlg, wQtyID, A71_QTY_MIN, FALSE);
        SetDlgItemInt(hDlg, wPrcID, (UINT)A71_PRC_MIN, FALSE);
    }

	SendDlgItemMessage(hDlg, IDD_A71_COMBO_AFTERMAX, CB_SETCURSEL, (WPARAM)0, 0L);
	SendDlgItemMessage(hDlg, IDD_A71_COMBO_BEFOREQTY, CB_SETCURSEL, (WPARAM)0, 0L);
	SetDlgItemInt(hDlg, IDD_A71_EDIT_MINTOTALSALE, A71_PRC_MIN, FALSE);
//	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_OVERRIDEPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PRICEDISCPREM), BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), BM_SETCHECK, BST_UNCHECKED, 0); 
	SendMessage(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), BM_SETCHECK, BST_UNCHECKED, 0);
	EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_DISCOUNTPLU), FALSE); 
	EnableWindow(GetDlgItem(hDlg,IDD_A71_CHK_PREMIUMPLU), FALSE);
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    SpinProc();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*               WPARAM  wParam  - 16 bits message parameter
*               LPARAM  lParam  - 32 bits message parameter
*
**  Return  :   TRUE            - User Process is Executed
*               FALSE           - Default Process is Expected
*
**  Description:
*       This function is spin procedure.
* ===========================================================================
*/
static  BOOL    SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    BOOL        fRet = FALSE;
    int         nEdit = 0;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = A71_SPIN_STEP;
    PepSpin.nTurbo     = A71_SPIN_TURBO;
    PepSpin.nTurStep   = A71_SPIN_TURSTEP;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */

    switch (GetDlgCtrlID((HWND)lParam)) {
    case IDD_A71_SPIN_QTY01:
        nEdit = IDD_A71_QTY01;
        break;
    case IDD_A71_SPIN_QTY02:
        nEdit = IDD_A71_QTY02;
        break;
    case IDD_A71_SPIN_QTY03:
        nEdit = IDD_A71_QTY03;
        break;
    case IDD_A71_SPIN_QTY04:
        nEdit = IDD_A71_QTY04;
        break;
    case IDD_A71_SPIN_QTY05:
        nEdit = IDD_A71_QTY05;
        break;
    case IDD_A71_SPIN_QTY06:
        nEdit = IDD_A71_QTY06;
        break;
    case IDD_A71_SPIN_QTY07:
        nEdit = IDD_A71_QTY07;
        break;
    case IDD_A71_SPIN_QTY08:
        nEdit = IDD_A71_QTY08;
        break;
    case IDD_A71_SPIN_QTY09:
        nEdit = IDD_A71_QTY09;
        break;
    case IDD_A71_SPIN_QTY10:
        nEdit = IDD_A71_QTY10;
        break;
    case IDD_A71_SPIN_QTY11:
        nEdit = IDD_A71_QTY11;
        break;
    case IDD_A71_SPIN_QTY12:
        nEdit = IDD_A71_QTY12;
        break;
    case IDD_A71_SPIN_QTY13:
        nEdit = IDD_A71_QTY13;
        break;
    case IDD_A71_SPIN_QTY14:
        nEdit = IDD_A71_QTY14;
        break;
    case IDD_A71_SPIN_QTY15:
        nEdit = IDD_A71_QTY15;
        break;
    case IDD_A71_SPIN_QTY16:
        nEdit = IDD_A71_QTY16;
        break;
    case IDD_A71_SPIN_QTY17:
        nEdit = IDD_A71_QTY17;
        break;
    case IDD_A71_SPIN_QTY18:
        nEdit = IDD_A71_QTY18;
        break;
    case IDD_A71_SPIN_QTY19:
        nEdit = IDD_A71_QTY19;
        break;
    case IDD_A71_SPIN_QTY20:
        nEdit = IDD_A71_QTY20;
        break;
	// adding 20 more PPI addresses
    case IDD_A71_SPIN_QTY21:
        nEdit = IDD_A71_QTY21;
        break;
    case IDD_A71_SPIN_QTY22:
        nEdit = IDD_A71_QTY22;
        break;
    case IDD_A71_SPIN_QTY23:
        nEdit = IDD_A71_QTY23;
        break;
    case IDD_A71_SPIN_QTY24:
        nEdit = IDD_A71_QTY24;
        break;
    case IDD_A71_SPIN_QTY25:
        nEdit = IDD_A71_QTY25;
        break;
    case IDD_A71_SPIN_QTY26:
        nEdit = IDD_A71_QTY26;
        break;
    case IDD_A71_SPIN_QTY27:
        nEdit = IDD_A71_QTY27;
        break;
    case IDD_A71_SPIN_QTY28:
        nEdit = IDD_A71_QTY28;
        break;
    case IDD_A71_SPIN_QTY29:
        nEdit = IDD_A71_QTY29;
        break;
    case IDD_A71_SPIN_QTY30:
        nEdit = IDD_A71_QTY30;
        break;
    case IDD_A71_SPIN_QTY31:
        nEdit = IDD_A71_QTY31;
        break;
    case IDD_A71_SPIN_QTY32:
        nEdit = IDD_A71_QTY32;
        break;
    case IDD_A71_SPIN_QTY33:
        nEdit = IDD_A71_QTY33;
        break;
    case IDD_A71_SPIN_QTY34:
        nEdit = IDD_A71_QTY34;
        break;
    case IDD_A71_SPIN_QTY35:
        nEdit = IDD_A71_QTY35;
        break;
    case IDD_A71_SPIN_QTY36:
        nEdit = IDD_A71_QTY36;
        break;
    case IDD_A71_SPIN_QTY37:
        nEdit = IDD_A71_QTY37;
        break;
    case IDD_A71_SPIN_QTY38:
        nEdit = IDD_A71_QTY38;
        break;
    case IDD_A71_SPIN_QTY39:
        nEdit = IDD_A71_QTY39;
        break;
    case IDD_A71_SPIN_QTY40:
        nEdit = IDD_A71_QTY40;
        break;
	// END adding 20 more PPI addresses
    case IDD_A71_SPIN_PRC01:
        nEdit = IDD_A71_PRC01;
        break;
    case IDD_A71_SPIN_PRC02:
        nEdit = IDD_A71_PRC02;
        break;
    case IDD_A71_SPIN_PRC03:
        nEdit = IDD_A71_PRC03;
        break;
    case IDD_A71_SPIN_PRC04:
        nEdit = IDD_A71_PRC04;
        break;
    case IDD_A71_SPIN_PRC05:
        nEdit = IDD_A71_PRC05;
        break;
    case IDD_A71_SPIN_PRC06:
        nEdit = IDD_A71_PRC06;
        break;
    case IDD_A71_SPIN_PRC07:
        nEdit = IDD_A71_PRC07;
        break;
    case IDD_A71_SPIN_PRC08:
        nEdit = IDD_A71_PRC08;
        break;
    case IDD_A71_SPIN_PRC09:
        nEdit = IDD_A71_PRC09;
        break;
    case IDD_A71_SPIN_PRC10:
        nEdit = IDD_A71_PRC10;
        break;
    case IDD_A71_SPIN_PRC11:
        nEdit = IDD_A71_PRC11;
        break;
    case IDD_A71_SPIN_PRC12:
        nEdit = IDD_A71_PRC12;
        break;
    case IDD_A71_SPIN_PRC13:
        nEdit = IDD_A71_PRC13;
        break;
    case IDD_A71_SPIN_PRC14:
        nEdit = IDD_A71_PRC14;
        break;
    case IDD_A71_SPIN_PRC15:
        nEdit = IDD_A71_PRC15;
        break;
    case IDD_A71_SPIN_PRC16:
        nEdit = IDD_A71_PRC16;
        break;
    case IDD_A71_SPIN_PRC17:
        nEdit = IDD_A71_PRC17;
        break;
    case IDD_A71_SPIN_PRC18:
        nEdit = IDD_A71_PRC18;
        break;
    case IDD_A71_SPIN_PRC19:
        nEdit = IDD_A71_PRC19;
        break;
    case IDD_A71_SPIN_PRC20:
        nEdit = IDD_A71_PRC20;
        break;
	// adding 20 more PPI addresses
    case IDD_A71_SPIN_PRC21:
        nEdit = IDD_A71_PRC21;
        break;
    case IDD_A71_SPIN_PRC22:
        nEdit = IDD_A71_PRC22;
        break;
    case IDD_A71_SPIN_PRC23:
        nEdit = IDD_A71_PRC23;
        break;
    case IDD_A71_SPIN_PRC24:
        nEdit = IDD_A71_PRC24;
        break;
    case IDD_A71_SPIN_PRC25:
        nEdit = IDD_A71_PRC25;
        break;
    case IDD_A71_SPIN_PRC26:
        nEdit = IDD_A71_PRC26;
        break;
    case IDD_A71_SPIN_PRC27:
        nEdit = IDD_A71_PRC27;
        break;
    case IDD_A71_SPIN_PRC28:
        nEdit = IDD_A71_PRC28;
        break;
    case IDD_A71_SPIN_PRC29:
        nEdit = IDD_A71_PRC29;
        break;
    case IDD_A71_SPIN_PRC30:
        nEdit = IDD_A71_PRC30;
        break;
    case IDD_A71_SPIN_PRC31:
        nEdit = IDD_A71_PRC31;
        break;
    case IDD_A71_SPIN_PRC32:
        nEdit = IDD_A71_PRC32;
        break;
    case IDD_A71_SPIN_PRC33:
        nEdit = IDD_A71_PRC33;
        break;
    case IDD_A71_SPIN_PRC34:
        nEdit = IDD_A71_PRC34;
        break;
    case IDD_A71_SPIN_PRC35:
        nEdit = IDD_A71_PRC35;
        break;
    case IDD_A71_SPIN_PRC36:
        nEdit = IDD_A71_PRC36;
        break;
    case IDD_A71_SPIN_PRC37:
        nEdit = IDD_A71_PRC37;
        break;
    case IDD_A71_SPIN_PRC38:
        nEdit = IDD_A71_PRC38;
        break;
    case IDD_A71_SPIN_PRC39:
        nEdit = IDD_A71_PRC39;
        break;
    case IDD_A71_SPIN_PRC40:
        nEdit = IDD_A71_PRC40;
        break;
	// END adding 20 more PPI addresses
    }

    /* ----- Set minimum and maximum ----- */

    switch (nEdit) {
    case IDD_A71_QTY01:
    case IDD_A71_QTY02:
    case IDD_A71_QTY03:
    case IDD_A71_QTY04:
    case IDD_A71_QTY05:
    case IDD_A71_QTY06:
    case IDD_A71_QTY07:
    case IDD_A71_QTY08:
    case IDD_A71_QTY09:
    case IDD_A71_QTY10:
    case IDD_A71_QTY11:
    case IDD_A71_QTY12:
    case IDD_A71_QTY13:
    case IDD_A71_QTY14:
    case IDD_A71_QTY15:
    case IDD_A71_QTY16:
    case IDD_A71_QTY17:
    case IDD_A71_QTY18:
    case IDD_A71_QTY19:
    case IDD_A71_QTY20:
	// adding 20 more PPI addresses
    case IDD_A71_QTY21:
    case IDD_A71_QTY22:
    case IDD_A71_QTY23:
    case IDD_A71_QTY24:
    case IDD_A71_QTY25:
    case IDD_A71_QTY26:
    case IDD_A71_QTY27:
    case IDD_A71_QTY28:
    case IDD_A71_QTY29:
    case IDD_A71_QTY30:
    case IDD_A71_QTY31:
    case IDD_A71_QTY32:
    case IDD_A71_QTY33:
    case IDD_A71_QTY34:
    case IDD_A71_QTY35:
    case IDD_A71_QTY36:
    case IDD_A71_QTY37:
    case IDD_A71_QTY38:
    case IDD_A71_QTY39:
    case IDD_A71_QTY40:
	// END adding 20 more PPI addresses
        PepSpin.lMin = (long)A71_QTY_MIN;
        PepSpin.lMax = (long)A71_QTY_MAX;
        break;
    case IDD_A71_PRC01:
    case IDD_A71_PRC02:
    case IDD_A71_PRC03:
    case IDD_A71_PRC04:
    case IDD_A71_PRC05:
    case IDD_A71_PRC06:
    case IDD_A71_PRC07:
    case IDD_A71_PRC08:
    case IDD_A71_PRC09:
    case IDD_A71_PRC10:
    case IDD_A71_PRC11:
    case IDD_A71_PRC12:
    case IDD_A71_PRC13:
    case IDD_A71_PRC14:
    case IDD_A71_PRC15:
    case IDD_A71_PRC16:
    case IDD_A71_PRC17:
    case IDD_A71_PRC18:
    case IDD_A71_PRC19:
    case IDD_A71_PRC20:
	// adding 20 more PPI addresses
    case IDD_A71_PRC21:
    case IDD_A71_PRC22:
    case IDD_A71_PRC23:
    case IDD_A71_PRC24:
    case IDD_A71_PRC25:
    case IDD_A71_PRC26:
    case IDD_A71_PRC27:
    case IDD_A71_PRC28:
    case IDD_A71_PRC29:
    case IDD_A71_PRC30:
    case IDD_A71_PRC31:
    case IDD_A71_PRC32:
    case IDD_A71_PRC33:
    case IDD_A71_PRC34:
    case IDD_A71_PRC35:
    case IDD_A71_PRC36:
    case IDD_A71_PRC37:
    case IDD_A71_PRC38:
    case IDD_A71_PRC39:
    case IDD_A71_PRC40:
	// END adding 20 more PPI addresses
        PepSpin.lMin = (long)A71_PRC_MIN;
        PepSpin.lMax = A71_PRC_MAX;
        break;
    }

    if (nEdit != 0) {

        /* ----- spin button procedure ----- */

        fRet = PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);

    }
    return (fRet);
}

// WARNING:  The standard dialog proc of PPI Edit makes a back of the PLU file
//           which causes the PLU file to become corrupted if AC71 Modeless dialog
//           for PPI is used from the AC68 PLU dialog.
//           See use of functions PepFileBackup(), PepFileDelete(), etc.
//                Richard Chambers, Oct-15-2018
HWND A071ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A71),
                                   hParentWnd,
                                   A071DlgProcModeless);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A071ModeLessDialogWnd (void)
{
	return hWndModeLess;
}


/* ===== End of A071.C ===== */
