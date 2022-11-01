/*
* ---------------------------------------------------------------------------
* Title         :   Cashier Maintenance (AC 20)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A020.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
*               :   2002-2015, Georgia Southern University
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
** GenPOS
* Aug-04-15 : 02.02.02 : R.Chambers : combine AC152 Employee Time Keeping into AC20 dialog
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
#include    <cscas.h>
#include    <csetk.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a020.h"
#include	"..\res\resource.h"

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
static  BYTE    bConf = A20_CONF_CHK;   /* Delete Confirmation Flag */
static  CASIF   CasData;        /* Current Editting Cashier Record  */
static	BOOL	enableDPFP = FALSE;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A020DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispatched Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for
*                       Cashier Maintenance (AC 20)
* ===========================================================================
*/
BOOL    WINAPI  A020DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;        /* Configuration of SpinButton      */
    static  USHORT  unCurCas,       /* No. of Current Cashier Records   */
                    unMaxCas;       /* No. of Maximum Cashier Records   */
    int     idSpin;
    BOOL    fRet;
    WCHAR    szDesc[128];

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
		memset(&CasData, 0x00, sizeof(CasData));

        /* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A20_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);
        LoadString(hResourceDll, IDS_PEP_OK, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDOK, szDesc);
        LoadString(hResourceDll, IDS_PEP_CANCEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDCANCEL, szDesc);
        LoadString(hResourceDll, IDS_A20_OPTIONS, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_SET, szDesc);
		LoadString(hResourceDll, IDS_A20_BIO_FINGER, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_BIO_FINGER, szDesc);
		LoadString(hResourceDll, IDS_A20_BIO_FINGER_RESET, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_BIO_FINGER_RESET, szDesc);

        LoadString(hResourceDll, IDS_A20_MENT, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRMENT, szDesc);
        LoadString(hResourceDll, IDS_A20_ADDCHG, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_ADDCHG, szDesc);
        LoadString(hResourceDll, IDS_A20_DEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_DEL, szDesc);
        LoadString(hResourceDll, IDS_A20_NO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRNO, szDesc);
        LoadString(hResourceDll, IDS_A20_SPIN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRSPIN, szDesc);
        LoadString(hResourceDll, IDS_A20_NAME, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRNAME, szDesc);
        LoadString(hResourceDll, IDS_A20_NAMEMAX, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRNAMEMAX, szDesc);
        LoadString(hResourceDll, IDS_A20_CONF, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_CONF, szDesc);
        LoadString(hResourceDll, IDS_A20_CTRL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A20_STRCTRL, szDesc);

		LoadString(hResourceDll, IDS_A20_SUP_ID_LBL, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A20_SUP_ID_LBL, szDesc);
		LoadString(hResourceDll, IDS_A20_CTRLSTR_LBL, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A20_CTRLSTR_LBL, szDesc);
		LoadString(hResourceDll, IDS_A20_WINDOW_LBL, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A20_WINDOW_LBL, szDesc);

		// before starting to use the Cashier data file, check that the data in the file is
		// consistent. We have found an error in the file conversion routines that convert
		// the Cashier data file from Rel 2.1 to Rel 2.2 in which the in-use block table was
		// not properly initialized resulting in the index portion of the file with its
		// block numbers not having the corresponding in-use block bit set in the in-use block table.
		CasCheckFileBlockData();

		/* ----- Initialize dialogbox ----- */
        unCurCas = A020InitDlg(hDlg, (LPUSHORT)&unMaxCas);

        /* ----- Initialize Configulation of Spin Button ----- */
        PepSpin.nStep      = A20_SPIN_STEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
        PepSpin.nTurbo     = 0;
        PepSpin.nTurStep   = 0;
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A20_MAX; j<=IDD_A20_TERM_NO_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        /* ----- Set Target EditText ID ----- */
        idSpin = GetDlgCtrlID((HWND)lParam) - A20_SPIN_OFFSET;

        /* ----- Set Maximum Value of Data Range by Selected Button ----- */
        switch (idSpin) {
        case IDD_A20_NO:
            PepSpin.lMin       = (long)A20_CAS_MIN;
            PepSpin.lMax       = (long)A20_CAS_MAX;
            break;

        case IDD_A20_NOFROM:
            PepSpin.lMin       = (long)A20_NO_MIN;
            PepSpin.lMax       = (long)A20_NO_MAX;
            break;

        case IDD_A20_NOTO:
            PepSpin.lMin       = (long)A20_NO_MIN;
            PepSpin.lMax       = (long)A20_NO_MAX;
            break;

        case IDD_A20_CTIP:
            PepSpin.lMin       = (long)A20_CTIP_MIN;
            PepSpin.lMax       = (long)A20_CTIP_MAX;
            break;

        case IDD_A20_TEAM:
            PepSpin.lMin       = (long)A20_TEAM_MIN;
            PepSpin.lMax       = (long)A20_TEAM_MAX;
            break;

        case IDD_A20_TERMNO:
            PepSpin.lMin       = (long)A20_TERMNO_MIN;
            PepSpin.lMax       = (long)A20_TERMNO_MAX;
            break;

        case IDD_A20_CODE1:
        case IDD_A20_CODE2:
        case IDD_A20_CODE3:
            PepSpin.lMin       = (long)A20_CODE_MIN;
            PepSpin.lMax       = (long)A20_CODE_MAX;
            break;
        }

        /* ----- Spin Button Procedure ----- */
        PepSpinProc(hDlg, wParam, idSpin, (LPPEPSPIN)&PepSpin);
        SendMessage(hDlg, WM_COMMAND, MAKELONG(idSpin,EN_CHANGE), 0L);
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A20_CONF:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Delete Confirmation Flag ----- */
                if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) {
                    bConf |= A20_CONF_CHK;
                } else {
                    bConf &= ~A20_CONF_CHK;
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A20_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Set Selected Data to Each Controls ----- */
                A020SetData(hDlg, (LPCASIF)&CasData, unCurCas);
                return TRUE;
            }
            return FALSE;

        case IDD_A20_ADDCHG:
        case IDD_A20_DEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Whether Cashier No. Exist or Not ----- */
                if (GetDlgItemInt(hDlg, IDD_A20_NO, NULL, FALSE)) {
					//hold on to the cashier number in order to
					//remove the associated fingerprint record (below)
					ULONG tempCasNo = CasData.ulCashierNo;

                    /* ----- get cashier data ----- */
                    fRet = A020GetData(hDlg, LOWORD(wParam), &CasData, &unCurCas, unMaxCas);

                    if ((LOWORD(wParam) == IDD_A20_DEL) && (fRet == TRUE)) {
						//delete the fingerprint data for this individual
						// DFPRDeleteRecord(tempCasNo);
                        /* ----- Set data to maintenance-box ----- */
                        A020SetData(hDlg, (LPCASIF)&CasData, unCurCas);
                    }

                    SetFocus(GetDlgItem(hDlg, IDD_A20_LIST));
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_A20_NO:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check record existance ----- */
                A020ChkExist(hDlg);
                return TRUE;
            }
            return FALSE;

        case IDD_A20_CTIP:
        case IDD_A20_TERMNO:
             if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A020ChkData(hDlg, LOWORD(wParam))) {
                    /* ----- Text out Error Message ----- */
                    A020ShowErr(hDlg, wParam);
                    /* ----- Undo Input Data ----- */
                    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
                    /* ----- Set Focus to Error EditText ----- */
                    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
                }
                return TRUE;
            }
            return FALSE;

        case IDD_SET:
            if (HIWORD(wParam) == BN_CLICKED) {
				/* ----- Create control code dialogbox ----- */
				DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_A20_CTRL), hDlg, A020CtrlDlgProc);
				/* ----- Set control code listbox content ----- */
				A020SetCtrlList(hDlg);
				return TRUE;
            }
			return FALSE;

		case IDD_A20_BIO_FINGER:
			// if (HIWORD(wParam) == BN_CLICKED)
			// 	return DFPRBeginEnrollment(hDlg, CasData.ulCashierNo);
			return FALSE;

		case IDD_A20_BIO_FINGER_RESET:
			return FALSE;

		case IDC_BTN_SET_GROUP_ASSOCIATION:
			if (HIWORD(wParam) == BN_CLICKED) {
				//popup group association dialog
				DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_A20_GROUP_ASSOCIATIONS), hDlg, A020GroupDlgProc);				
				return TRUE;
            }
			return FALSE;

		case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Finalize DialogBox ----- */
                A020FinDlg(hDlg, wParam);
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
**  Synopsis    :   BOOL    WINAPI  A020CtrlDlgProc()
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
*               Operator Status Control Code (AC 20)
* ===========================================================================
*/
BOOL    WINAPI  A020CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static CASIF    CasCtrlData;
    WCHAR   szDesc[PEP_LOADSTRING_LEN];

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Set data to control code dialog box ----- */
        A020SetCtrlData(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A20ST01; j<=IDD_A20ST15; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
		/* ###ADD Saratoga */
        case IDD_A20ST14:
            memset((LPSTR)szDesc, 0x00, sizeof(szDesc));
            LoadString(hResourceDll, IDS_A20_STAON14, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A20ST14, szDesc);
            break;

        case IDD_A20ST15:
            memset((LPSTR)szDesc, 0x00, sizeof(szDesc));
            LoadString(hResourceDll, IDS_A20_STAON15, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A20ST15, szDesc);
            break;

		case IDD_A20_ORDERDEC_ENABLED:
			if(SendDlgItemMessage(hDlg, IDD_A20_ORDERDEC_ENABLED, BM_GETCHECK, 0, 0L))
			{
				EnableWindow(GetDlgItem(hDlg, IDD_A20ST17),TRUE);
				CheckDlgButton(hDlg, IDD_A20ST17, TRUE);
				EnableWindow(GetDlgItem(hDlg, IDD_A20ST18),TRUE);
			}else
			{
				EnableWindow(GetDlgItem(hDlg, IDD_A20ST17),FALSE);
				CheckDlgButton(hDlg, IDD_A20ST17, FALSE);
				EnableWindow(GetDlgItem(hDlg, IDD_A20ST18),FALSE);
				CheckDlgButton(hDlg, IDD_A20ST18, FALSE);
			}
			break;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
					if(IsDlgButtonChecked(hDlg, IDD_A20_ORDERDEC_ENABLED))
					{
						if((!IsDlgButtonChecked(hDlg,IDD_A20ST17))&&
							(!IsDlgButtonChecked(hDlg,IDD_A20ST18)))
						{
							return FALSE; //error
						}
					}
                    /* ----- Get Inputed Control Data to Buffer ----- */
                    A020GetCtrlData(hDlg);
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

BOOL    WINAPI  A020GroupDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam){
	int i = 0;
	int checkboxes[] = {IDC_CHECK1, IDC_CHECK2, IDC_CHECK3, IDC_CHECK4,
						IDC_CHECK5, IDC_CHECK6, IDC_CHECK7, IDC_CHECK8,
						IDC_CHECK9, IDC_CHECK10,IDC_CHECK11,IDC_CHECK12,
						IDC_CHECK13,IDC_CHECK14,IDC_CHECK15,IDC_CHECK16,
						IDC_CHECK17,IDC_CHECK18,IDC_CHECK19,IDC_CHECK20,
						IDC_CHECK21,IDC_CHECK22,IDC_CHECK23,IDC_CHECK24,
						IDC_CHECK25,IDC_CHECK26,IDC_CHECK27,IDC_CHECK28,
						IDC_CHECK29,IDC_CHECK30,IDC_CHECK31,IDC_CHECK32};
	switch(wMsg){
		case WM_INITDIALOG:
			for (i = 0; i < 32; i++)
				CheckDlgButton(hDlg, checkboxes[i], (CasData.ulGroupAssociations & (0x00000001 << i) ? TRUE : FALSE));
			return TRUE;
		case WM_COMMAND:
			switch(wParam){
			case IDC_BTN_CHECK_ALL:		//check all
				for (i = 0; i < 32; i++)
					CheckDlgButton(hDlg, checkboxes[i], TRUE);
				return TRUE;

			case IDC_BTN_CHECK_NONE:	//check none
				for (i = 0; i < 32; i++)
					CheckDlgButton(hDlg, checkboxes[i], FALSE);
				return TRUE;

			case IDC_BTN_CHECK_INVERT:	//check inversion
				for (i = 0; i < 32; i++)
					CheckDlgButton(hDlg, checkboxes[i], IsDlgButtonChecked(hDlg, checkboxes[i]) ? FALSE : TRUE);
				return TRUE;

			case IDOK:
				CasData.ulGroupAssociations = 0;	//wipe existing groups so the right ones can be set
				for (i = 0; i < 32; i++)
					if(IsDlgButtonChecked(hDlg, checkboxes[i]))
						CasData.ulGroupAssociations |= 0x1 << i;
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			default:
				return FALSE;		
		}
		default:
			return FALSE;
	}
	return FALSE;
}


/*
* ===========================================================================
**  Synopsis    :   USHORT  A020InitDlg()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   LPUSHORT    lpunMax     -   Address of Max. Record Area
*
**  Return      :   USHORT      unCur       -   No. of Current Cashier Records.
*
**  Description :
*       This procedure gets number of maximum cashier records from prog. #2,
*   and set it to maximum records area (lpunMax). And then it initializes the
*   controls of a dialogbox.
*       It returns the number of current cashier records.
* ===========================================================================
*/
USHORT  A020InitDlg(HWND hDlg, LPUSHORT lpunMax)
{
    ULONG		aulCasNo[MAX_NO_CASH];
	USHORT		unI, unCur = 0;
    WCHAR		szMsg[A20_MEM_LEN], szCap[PEP_CAPTION_LEN];
	HMODULE		hTemp;

    memset(&CasData, 0, sizeof(CASIF));

    /* ----- Get max. value of cashier records from Prog.#2 ----- */
    *lpunMax = A020ReadMax();

    /* ----- Check user sets the No. of cashier in Prog.#2 ----- */
    if (*lpunMax == 0) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A20, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A20_FLEX_EMPTY,  szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ---- Display Caution Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
		//Flex memory is set to 0 cancel out.
        EndDialog(hDlg, LOWORD(IDCANCEL));
        return 0;
    }

    /* ----- Load Cashier File Name from Resorece ----- */
    LoadString(hResourceDll, IDS_FILE_NAME_CAS, szCap, PEP_STRING_LEN_MAC(szCap));

    /* ----- Create Backup File for Cancel Key ----- */
    PepFileBackup(hDlg, szCap, PEP_FILE_BACKUP);

    /* ----- Read All Cashier No. from Cashier File ----- */
    unCur = CasAllIdRead(sizeof(aulCasNo), aulCasNo);

    for (unI = 0; unI < unCur; unI++) {
		CASIF		CasWork = {0};

		/* ----- Read Current Exists cashier Name from Server File ----- */
        CasWork.ulCashierNo = aulCasNo[unI];
        CasIndRead(&CasWork);

        /* ----- Insert Loaded cashier Name to ListBox ----- */
        A020InitList(hDlg, (LPCASIF)&CasWork);
    }

    /* ----- Set maximum cashier records to static text ----- */
    A020SetMaxRec(hDlg, *lpunMax);

    /* ----- Set current cashier records to static text ----- */
    A020SetCurRec(hDlg, unCur);

    /* ----- Disable Buttons related to Cashiers (since none are selected) ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A20_ADDCHG),		FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A20_DEL),		    FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),		        FALSE);
	EnableWindow(GetDlgItem(hDlg, IDD_A20_BIO_FINGER),	FALSE);
	EnableWindow(GetDlgItem(hDlg, IDD_A20_BIO_FINGER_RESET), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_BTN_SET_GROUP_ASSOCIATION), FALSE);

	//determine if the drivers for digitalPersona are present before enabling the FingerPrint button
	hTemp = LoadLibrary(TEXT("dpFPApi.dll"));
	if(hTemp){
		FreeLibrary(hTemp);
		enableDPFP = TRUE;
	}

    /* ----- Set Fixed Font to EditText & ListBox ----- */
    PepSetFont(hDlg, IDD_A20_NAME);
    PepSetFont(hDlg, IDD_A20_LIST);
//    PepSetFont(hDlg, IDD_A20_CTRL_LIST);

    /* ----- Limit Input Length to EditText ----- */
    SendDlgItemMessage(hDlg, IDD_A20_NO,   EM_LIMITTEXT, A20_NO_LEN,   0L);
    SendDlgItemMessage(hDlg, IDD_A20_NAME, EM_LIMITTEXT, A20_NAME_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_NOFROM, EM_LIMITTEXT, A20_NOFROM_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_NOTO, EM_LIMITTEXT, A20_NOTO_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_CTIP, EM_LIMITTEXT, A20_CTIP_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_TEAM, EM_LIMITTEXT, A20_TEAM_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_TERMNO, EM_LIMITTEXT, A20_TERMNO_LEN, 0L);
	SendDlgItemMessage(hDlg, IDD_A20_SUP_ID_NO, EM_LIMITTEXT, A20_SUP_ID_LEN, 0);
	SendDlgItemMessage(hDlg, IDD_A20_CTRLSTR_NO, EM_LIMITTEXT, A20_CTRLSTR_LEN, 0);
	SendDlgItemMessage(hDlg, IDD_A20_WINDOW_NO, EM_LIMITTEXT, A20_WINDOW_LEN, 0);

    SendDlgItemMessage(hDlg, IDD_A20_CODE1, EM_LIMITTEXT, A20_CODE_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_CODE2, EM_LIMITTEXT, A20_CODE_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A20_CODE3, EM_LIMITTEXT, A20_CODE_LEN, 0L);

    /* ----- Set Focus to ListBox ----- */
    SetFocus(GetDlgItem(hDlg, IDD_A20_LIST));

    /* ----- Set delete confirmation checkbox ----- */
    CheckDlgButton(hDlg, IDD_A20_CONF, bConf);

    return (unCur);
}

/*
* ===========================================================================
**  Synopsis    :   USHORT  A020ReadMax()
*
**  Input       :   None
*
**  Return      :   USHORT  - address of maximum records
*
**  Description :
*       This function retrieves the number of sever records that user sets
*   in Program Mode # 2.
* ===========================================================================
*/
USHORT  A020ReadMax(VOID)
{
    PARAFLEXMEM FlexData;

    /* ----- Set Target Address to Server ----- */
    FlexData.uchAddress = FLEX_CAS_ADR;

    /* ----- Read the Flexible Memory Data of Server ----- */
    ParaFlexMemRead(&FlexData);

    /* ----- Return the No. of Current Server ----- */
    /* return (FlexData.usRecordNumber); NCR2172 */
    return ((USHORT)FlexData.ulRecordNumber);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020InitList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPCASIF lpData  -   Address of Target Cashier Data
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets specified cashier data to listbox.
* ===========================================================================
*/
VOID    A020InitList(HWND hDlg, LPCASIF lpData)
{
    WCHAR    szName[A20_NAME_LEN + 1], szWork[A20_MNE_LEN * 2];

    /* ----- Replace Double Width Key (0x12 => '~') ----- */
    PepReplaceMnemonic((WCHAR *)lpData->auchCashierName, (WCHAR *)szName, (A20_NAME_LEN + 1), PEP_MNEMO_READ);

    /* ----- Create String for Insert ListBox ----- */
    wsPepf(szWork, WIDE("%8u : %s"), lpData->ulCashierNo, szName);

    /* ----- Insert Created String to ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetMaxRec()
*
**  Input       :   HWND    hDlg    -   handle of dialogbox
*                   USHORT  unMax   -   maximum cashier records
*
**  Return      :   No return value.
*
**  Description :
*       This procedure set maximum cashier records.
* ===========================================================================
*/
VOID    A020SetMaxRec(HWND hDlg, USHORT unMax)
{
    WCHAR    szWork[A20_MAX_LEN * 2], szMax[A20_MAX_LEN];

    /* ----- Make maximum cashier eecords description ----- */
    LoadString(hResourceDll, IDS_A20_MAXCAS, szMax, PEP_STRING_LEN_MAC(szMax));
    wsPepf(szWork, szMax, unMax);

    /* ----- Set description to static text ----- */
    DlgItemRedrawText(hDlg, IDD_A20_MAX, szWork);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetCurRec()
*
**  Input       :   HWND    hDlg    -   handle of dialogbox
*                   USHORT  unCur   -   current cashier records
*
**  Return      :   No return value.
*
**  Description :
*       This procedure set current cashier records.
* ===========================================================================
*/
VOID    A020SetCurRec(HWND hDlg, USHORT unCur)
{
    WCHAR    szWork[A20_MAX_LEN], szCur[A20_MAX_LEN];

    /* ----- Make current cashier eecords description ----- */
    LoadString(hResourceDll, IDS_A20_CURCAS, szCur, PEP_STRING_LEN_MAC(szCur));
    wsPepf(szWork, szCur, unCur);

    /* ----- Set description to static text ----- */
    DlgItemRedrawText(hDlg, IDD_A20_CUR, szWork);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPCASIF lpData  -   Address of Target Cashier Record
*                   USHORT  unCur   -   No. of Current Cashier Records
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets target cashier data to each controls.
* ===========================================================================
*/
VOID    A020SetData(HWND hDlg, LPCASIF lpData, USHORT unCur)
{
    DWORD   dwOff;
    WCHAR    szBuff[A20_MNE_LEN + 1];
	ETK_JOB Job = {0};

    /* ----- Enable/Disable to Push Button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A20_ADDCHG),		(BOOL)unCur);
    EnableWindow(GetDlgItem(hDlg, IDD_A20_DEL),			(BOOL)unCur);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),				(BOOL)unCur);
	if(enableDPFP){
		EnableWindow(GetDlgItem(hDlg, IDD_A20_BIO_FINGER),(BOOL)unCur);
		EnableWindow(GetDlgItem(hDlg, IDD_A20_BIO_FINGER_RESET),(BOOL)unCur);
	}
	EnableWindow(GetDlgItem(hDlg, IDC_BTN_SET_GROUP_ASSOCIATION), (BOOL)unCur);	

    if (unCur != 0) {   /* Cashier Record Exists */

        /* ----- Get Selected Item Order from ListBox ----- */
        dwOff = SendDlgItemMessage(hDlg, IDD_A20_LIST, LB_GETCURSEL, 0, 0L);

        /* ----- Get Selected String from ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_GETTEXT, (WPARAM)dwOff, (LPARAM)(szBuff));

        /* ----- Calculate Cashier No. with Loaded String ----- */
        lpData->ulCashierNo = (ULONG)_wtoi(szBuff);

        /* ----- Set Cashier No. to EditText ----- */
        SetDlgItemInt(hDlg, IDD_A20_NO, (UINT)lpData->ulCashierNo, FALSE);

        /* ----- Retrieve Target Cashier Data from Cashier File ----- */
        CasIndRead((CASIF *)lpData);
        EtkIndJobRead(lpData->ulCashierNo, &Job, szBuff);

        /* ----- Replace Double Key Code (0x12 => '~') ----- */
		memset(szBuff, 0, sizeof(szBuff));
        PepReplaceMnemonic((WCHAR *)lpData->auchCashierName, (WCHAR *)szBuff, A20_NAME_LEN, PEP_MNEMO_READ);

        /* ----- Set cashier No. to EditText ----- */
        DlgItemRedrawText(hDlg, IDD_A20_NAME, szBuff);

/********* Add V3.3 *********/

        /* ----- Set Guest Check No. ------ */
        SetDlgItemInt(hDlg, IDD_A20_NOFROM, (UINT)lpData->usGstCheckStartNo, FALSE);
        SetDlgItemInt(hDlg, IDD_A20_NOTO,   (UINT)lpData->usGstCheckEndNo, FALSE);

        /* ----- Set Charge Tip Rate ----- */
        SetDlgItemInt(hDlg, IDD_A20_CTIP, (UINT)lpData->uchChgTipRate, FALSE);

        /* ----- Set Team No. ------ */
        SetDlgItemInt(hDlg, IDD_A20_TEAM, (UINT)lpData->uchTeamNo, FALSE);

        /* ----- Set Terminal No. ------ */
        SetDlgItemInt(hDlg, IDD_A20_TERMNO, (UINT)lpData->uchTerminal, FALSE);

		// Set Supervisor Number
		SetDlgItemInt(hDlg, IDD_A20_SUP_ID_NO, (UINT)lpData->usSupervisorID, FALSE);

		// Set Control String Number
		SetDlgItemInt(hDlg, IDD_A20_CTRLSTR_NO, (UINT)lpData->usCtrlStrKickoff, FALSE);

		// Set Startup Window Number
		SetDlgItemInt(hDlg, IDD_A20_WINDOW_NO, (UINT)lpData->usStartupWindow, FALSE);
		SetDlgItemInt(hDlg, IDD_A20_CODE1, (UINT)Job.uchJobCode1, FALSE);
		SetDlgItemInt(hDlg, IDD_A20_CODE2, (UINT)Job.uchJobCode2, FALSE);
		SetDlgItemInt(hDlg, IDD_A20_CODE3, (UINT)Job.uchJobCode3, FALSE);

        /* ----- Set Status List ----- */
        A020SetCtrlList(hDlg);
		/****************************/
    } else {            /* cashier record not exist */
        /* ----- Clear Each Controls ----- */
        SendDlgItemMessage(hDlg, IDD_A20_NO,   EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_NO,   WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_NAME, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_NAME, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_NOFROM, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_NOFROM, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_NOTO, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_NOTO, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_CTIP, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_CTIP, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_TEAM, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_TEAM, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A20_TERMNO, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A20_TERMNO, WM_CLEAR,  0, 0L);
//      SendDlgItemMessage(hDlg, IDD_A20_CTRL_LIST, LB_RESETCONTENT, 0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_SUP_ID_NO, EM_SETSEL, 0, MAKELONG(0, -1));
		SendDlgItemMessage(hDlg, IDD_A20_SUP_ID_NO, WM_CLEAR,  0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_CTRLSTR_NO, EM_SETSEL, 0, MAKELONG(0, -1));
		SendDlgItemMessage(hDlg, IDD_A20_CTRLSTR_NO, WM_CLEAR,  0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_WINDOW_NO, EM_SETSEL, 0, MAKELONG(0, -1));
		SendDlgItemMessage(hDlg, IDD_A20_WINDOW_NO, WM_CLEAR,  0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_CODE1, WM_CLEAR, 0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_CODE2, WM_CLEAR, 0, 0L);
		SendDlgItemMessage(hDlg, IDD_A20_CODE3, WM_CLEAR, 0, 0L);

        /* ----- Set Focus to ListBox ----- */
        SetFocus(GetDlgItem(hDlg, IDD_A20_LIST));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetCtrlData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by current control code.
* ===========================================================================
*/
VOID    A020SetCtrlData(HWND hDlg)
{
    /* ----- Set Control Code Data to CheckButton for Address 1 & 2 ----- */
    A020SetChkbox(hDlg, IDD_A20ST01, A20_CHKBIT1, (BYTE)A20_STACODE1);

    /* ----- Set Control Code Data to CheckButton for Address 3 & 4 ----- */
    A020SetChkbox(hDlg, IDD_A20ST08, A20_CHKBIT2, (BYTE)A20_STACODE2);

	/* ----- Set Control Code Data to CheckButton for Address 5 & 6 ----- */
	A020SetChkbox(hDlg, IDD_A20ST16, A20_CHKBIT2, (BYTE)A20_STACODE3);  //PDINU

	// Set data for Address 7 & 8
	A020SetChkbox(hDlg, IDD_A20ST24, A20_CHKBIT2, (BYTE)A20_STACODE4);

	// Set data for Address 9 & 10
	A020SetChkbox(hDlg, IDD_A20ST32, A20_CHKBIT2, (BYTE)A20_STACODE5);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*
**  Return      :   No return value.
*
**  Description :
*       This function checks the checkbutton by current control code of
*   selected Operator Status data.
* ===========================================================================
*/
VOID    A020SetChkbox(HWND hDlg, WPARAM wBtnID, BYTE bChkBit, BYTE bCtrlAddr)
{
    BYTE    bI;
    BOOL    fCheck;
    WCHAR    szDesc[PEP_LOADSTRING_LEN];     /* ###ADD Saratoga */

    for (bI = 0; bI < A20_8BITS; bI++) {
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A20_BITMASK << bI)) {
			/* ----- Get Current Bit Status ----- */
            fCheck = (CasData.fbCashierStatus[bCtrlAddr] & (A20_BITMASK << bI)) ? TRUE : FALSE;

			/* ###ADD Saratoga */
            if (wBtnID == IDD_A20ST14) {
                LoadString( hResourceDll, IDS_A20_STAON14, szDesc, PEP_STRING_LEN_MAC(szDesc));
                DlgItemRedrawText(hDlg, wBtnID, szDesc);
            }
            if (wBtnID == IDD_A20ST15) {
                LoadString( hResourceDll, IDS_A20_STAON15, szDesc, PEP_STRING_LEN_MAC(szDesc));
                DlgItemRedrawText(hDlg, wBtnID, szDesc);
            }

			if (wBtnID == IDD_A20ST17) {
				if (CasData.fbCashierStatus[2] & 0x02) {
					// "Terminal Default" is enabled, so this area should be enabled
					CheckDlgButton(hDlg,IDD_A20_ORDERDEC_ENABLED, TRUE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST17), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST18), TRUE);
					// check "Terminal Default" radio button
					CheckDlgButton(hDlg, IDD_A20ST17, TRUE);
				} else if( CasData.fbCashierStatus[2] & 0x04) {
					// "Forced Entry" is enabled, so this area should be enabled
					CheckDlgButton(hDlg,IDD_A20_ORDERDEC_ENABLED, TRUE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST17), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST18), TRUE);
					// check "Forced Entry" radio button
					CheckDlgButton(hDlg, IDD_A20ST18, TRUE);
				} else {
					// neither "Terminal Default" nor "Forced Entry" are enabled, so disabled this area
					CheckDlgButton(hDlg,IDD_A20_ORDERDEC_ENABLED, FALSE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST17), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDD_A20ST18), FALSE);
				}

				wBtnID++;
				continue;	//nevermind the normal checkbox logic below
			}

            /* ----- Check Target CheckButton ----- */
            CheckDlgButton(hDlg, wBtnID++, fCheck);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetCtrlList()
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
VOID    A020SetCtrlList(HWND hDlg)
{
    /* ----- Reset Contents of Control Code ListBox ----- */
//    SendDlgItemMessage(hDlg, IDD_A20_CTRL_LIST, LB_RESETCONTENT, 0, 0L);

    /* ----- Set listbox content for address 1 & 2 ----- */
    A020SetListContent(hDlg, A20_CHKBIT1, A20_STACODE1, A20_STAOFFSET1);

    /* ----- Set listbox content for address 3 & 4 ----- */
    A020SetListContent(hDlg, A20_CHKBIT2, A20_STACODE2, A20_STAOFFSET2);

	    /* ----- Set listbox content for address 5 & 6 ----- */
    A020SetListContent(hDlg, A20_CHKBIT2, A20_STACODE3, A20_STAOFFSET3);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020SetListContent()
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
VOID A020SetListContent(HWND hDlg, BYTE bChkBit, BYTE bCtrlAddr, WPARAM wOffset)
{
    BYTE    bI;
    WCHAR    szDesc[PEP_LOADSTRING_LEN];

    for(bI = 0; bI < A20_8BITS; bI++) {
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A20_BITMASK << bI)) {
            /* ----- Check Current Bit is ON or OFF ----- */
            if (CasData.fbCashierStatus[bCtrlAddr] & (A20_BITMASK << bI)) {
                LoadString(hResourceDll, IDS_A20_STAON1 + wOffset, szDesc, PEP_STRING_LEN_MAC(szDesc));
            } else {
                LoadString(hResourceDll, IDS_A20_STAOFF1 + wOffset, szDesc, PEP_STRING_LEN_MAC(szDesc));
            }

            /* ----- Increment Descriptin Offset ----- */
            wOffset++;

            /* ----- Insert Description to Control Code ListBox ----- */
//            DlgItemSendTextMessage(hDlg, IDD_A20_CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A020GetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wID     -   Selected Button ID
*                   LPCASIF     lpData  -   Address of Cashier Data Structure
*                   LPUSHORT    lpunCur -   Address of Current Cashier reecords
*                   USHORT      unMax   -   Maximum Cashier Records
*
**  Return      :   BOOL    TRUE    -   Add/Change or Delete is Executed
*                           FALSE   -   Delete is not Executed
*
**  Description:
*       This procedure gets inputed data from each controls and set them to
*   buffer, and then saves them to cashier file, if user pressed Add/Change
*   button. Otherwise it deletes cashier data of specified cashier no. from
*   file.
*       It returns TRUE, if function was successful. Otherwise it returns
*   FALSE (User selects CANCEL button when delete confirmation is ON).
* ===========================================================================
*/
BOOL    A020GetData(HWND hDlg, WORD wEditId, LPCASIF lpData, LPUSHORT lpunCur, USHORT unMax)
{
    BOOL    fRet = TRUE;
	BOOL    fCode1, fCode2, fCode3;

    /* ----- Get Inputed Cashier No. from EditText ----- */
    lpData->ulCashierNo = (ULONG)GetDlgItemInt(hDlg, IDD_A20_NO, NULL, FALSE);

    /* ----- Check Whether Over Maximum Server Records or Not ----- */
    if (A020ChkMax(hDlg, wEditId, lpData->ulCashierNo, *lpunCur, unMax) == FALSE) {
		ETK_JOB  Job = {0};
		WCHAR    szWork[A20_MNE_LEN + 1] = {0};

        /* ----- Get Inputed Cashier Name from EditText ----- */
        DlgItemGetText(hDlg, IDD_A20_NAME, szWork, sizeof(szWork));

        /* ----- Replace Double Key Code ('~' => 0x12) ----- */
        PepReplaceMnemonic((WCHAR *)szWork, (WCHAR *)lpData->auchCashierName, A20_NAME_LEN + 1, PEP_MNEMO_WRITE);

        /* ----- Get status ----- */
        /* ----- Guest Check No. ------ */
        lpData->usGstCheckStartNo	= (USHORT)GetDlgItemInt(hDlg, IDD_A20_NOFROM, NULL, FALSE);
        lpData->usGstCheckEndNo		= (USHORT)GetDlgItemInt(hDlg, IDD_A20_NOTO, NULL, FALSE);

        /* ----- Charge Tip Rate ----- */
        lpData->uchChgTipRate		= (UCHAR)GetDlgItemInt(hDlg, IDD_A20_CTIP, NULL, FALSE);

        /* ----- Team Number ----- */
        lpData->uchTeamNo			= (UCHAR)GetDlgItemInt(hDlg, IDD_A20_TEAM, NULL, FALSE);

        /* ----- Terminal Number ----- */
        lpData->uchTerminal			= (UCHAR)GetDlgItemInt(hDlg, IDD_A20_TERMNO, NULL, FALSE);

		//Supervisor Number
		lpData->usSupervisorID		= (USHORT)GetDlgItemInt(hDlg, IDD_A20_SUP_ID_NO, NULL, FALSE);

		//Control String Kickoff
		lpData->usCtrlStrKickoff	= (USHORT)GetDlgItemInt(hDlg, IDD_A20_CTRLSTR_NO, NULL, FALSE);

		//Startup Window
		lpData->usStartupWindow		= (USHORT)GetDlgItemInt(hDlg, IDD_A20_WINDOW_NO, NULL, FALSE);

		Job.uchJobCode1 = (UCHAR)GetDlgItemInt(hDlg, IDD_A20_CODE1, (LPBOOL)&fCode1, FALSE);
		Job.uchJobCode2 = (UCHAR)GetDlgItemInt(hDlg, IDD_A20_CODE2, (LPBOOL)&fCode2, FALSE);
		Job.uchJobCode3 = (UCHAR)GetDlgItemInt(hDlg, IDD_A20_CODE3, (LPBOOL)&fCode3, FALSE);

        /* ----- Refresh ListBox ----- */
        if (A020RefreshList(hDlg, lpData, (LPUSHORT)lpunCur, wEditId) == TRUE) {
            /* ----- User Select Cancel for Delete Cashier Data ----- */
            fRet = FALSE;
        } else {
            if (wEditId == IDD_A20_ADDCHG) {
                /* ----- Add/Change Record ----- */
                CasAssign(lpData);
				EtkAssign(lpData->ulCashierNo, &Job, szWork);
            } else {
                /* ----- Delete Record ----- */
                CasDelete(lpData->ulCashierNo);
				EtkDelete(lpData->ulCashierNo);
                memset(&CasData, 0, sizeof(CASIF));
            }
        }
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020GetCtrlData()
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
VOID    A020GetCtrlData(HWND hDlg)
{
    /* ----- Get Data from Checkbox for Address 1 & 2 ----- */
    A020GetChkbox(hDlg, IDD_A20ST01, A20_CHKBIT1, (BYTE)A20_STACODE1);

    /* ----- Get Data from Checkbox for Address 3 & 4 ----- */
    A020GetChkbox(hDlg, IDD_A20ST08, A20_CHKBIT2, (BYTE)A20_STACODE2);

	/* ----- Get Data from Checkbox for Address 5 & 6 ----- */
	A020GetChkbox(hDlg, IDD_A20ST16, A20_CHKBIT2, (BYTE)A20_STACODE3);	//PDINU

	// Get data for Address 7 & 8
	A020GetChkbox(hDlg, IDD_A20ST24, A20_CHKBIT2, (BYTE)A20_STACODE4);

	// Get data for Address 7 & 8
	A020GetChkbox(hDlg, IDD_A20ST32, A20_CHKBIT2, (BYTE)A20_STACODE5);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020GetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed control code data from checkbox. And then
*   it sets them to the static control code data buffer.
* ===========================================================================
*/
VOID    A020GetChkbox(HWND hDlg, WPARAM wBtnID, BYTE bChkBit, BYTE bCtrlAddr)
{
    BYTE    bI;

    /* ----- Reset Target Control Code Area without Reserved Area ----- */
    CasData.fbCashierStatus[bCtrlAddr] &= (~bChkBit);
    for (bI = 0; bI < A20_8BITS; bI++) {
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A20_BITMASK << bI)) {
            /* ----- Get Data from CheckBox ----- */
            if (IsDlgButtonChecked(hDlg, wBtnID++) == TRUE) {
                CasData.fbCashierStatus[bCtrlAddr] |= (A20_BITMASK << bI);
            }
        }
    }
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    A020ChkMax()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   User Selected Button ID
*                   ULONG	unNo    -   Current Selected Cashier No.
*                   USHORT  unCur   -   No. of Current Cashier Records
*                   USHORT  unMax   -   No. of Maximum cashier Records
*
**  Return      :   BOOL    TRUE    -   Over Maximum cashier Records
*                           FALSE   -   Not Over Miximum cashier Records
*
**  Description :
*       This procedure determines whether number of current server records
*   over than maximum server records.
*       It returns TRUE, if number of server records over than maximum server
*   records. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A020ChkMax(HWND hDlg, WORD wEditId, ULONG unNo, USHORT unCur, USHORT unMax)
{
    WCHAR	szCap[PEP_CAPTION_LEN], szMsg[PEP_OVER_LEN];
    DWORD	dwIndex;
    BOOL	fRet = FALSE;

    if ((wEditId == IDD_A20_ADDCHG) && (unCur == unMax)) {	/* check over maximum */
        /* ----- Search Corresponding Data from Current cashier Data ----- */
        dwIndex = A020FindNo(hDlg, (UINT)unNo);
        if (dwIndex == LB_ERR) {            /* add new data */
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A20, szCap, PEP_STRING_LEN_MAC(szCap));
            LoadString(hResourceDll, IDS_A20_OVERRECORD,  szMsg, PEP_STRING_LEN_MAC(szMsg));

            /* ----- Display Caution Message ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

            /* ----- Set Selected Cashier No. to EditText ----- */
            SendDlgItemMessage(hDlg, IDD_A20_NO, EM_SETSEL, 1, MAKELONG(0, -1));

            fRet = TRUE;
        }
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   DWORD   A020FindNo()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   UINT    unCasNo -   Target cashier No.
*
**  Return      :   DWORD   dwIndex -   Zero Based Index No. of ListBox
*
**  Description:
*       This function is looking for the specified cashier number in listbox.
*   It returns the zero based index of listbox, if the specified cashier number
*   is found. Otherwise it returns LB_ERR.
* ===========================================================================
*/
DWORD   A020FindNo(HWND hDlg, UINT unCasNo)
{
    WCHAR    szTargetNo[A20_MNE_LEN];
    DWORD   dwIndex;

    /* ----- Make the String for Search for Target cashier No. ---- */
    wsPepf(szTargetNo, WIDE("%8u : "), unCasNo);	//unCasNo is 8 digits now

    /* ----- Search Corresponding Data from Current cashier Data ----- */
    dwIndex = DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szTargetNo));

    return (dwIndex);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A020RefreshList()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPCASIF     lpData  -   Address of cashier Structure
*                   LPUSHORT    lpunCur -   Address of No. of Current Records
*                   WORD        wParam  -   User Selected Button ID
*
**  Return      :   TRUE    -   Delete is Fault
*                   FALSE   -   Add/Change or Delete is Executed
*
**  Description :
*       This procedure reset contents of listbox. It returns FALSE (User selects
*   CANCEL button when delete confirmation is ON). Otherwise it returns TRUE.
* ===========================================================================
*/
BOOL    A020RefreshList(HWND hDlg, LPCASIF lpData, LPUSHORT lpunCur, WPARAM wParam)
{
    BOOL    fChk = FALSE;
    DWORD   dwIndex;
    WCHAR    szWork[A20_MNE_LEN * 2],
			 //SR385 The Mnemonic is also placed into the szBuff
			 //Some mnemonics are longer than the cashier name (20 characters)
             szBuff[A20_MNE_LEN * 2];

    /* ----- Search corresponding data from current cashier data ----- */
	memset(&szBuff, 0x00, sizeof(szBuff));

    dwIndex = A020FindNo(hDlg, (UINT)lpData->ulCashierNo);

    /* ----- Replace Double Width Key (0x12 => '~') ----- */
    PepReplaceMnemonic((WCHAR *)lpData->auchCashierName, (WCHAR *)szBuff, A20_NAME_LEN, PEP_MNEMO_READ);

    /* ----- make description for insert listbox ----- */
    wsPepf(szWork, WIDE("%8u : %s"), lpData->ulCashierNo, szBuff);		//changed to 8 digits

    if (dwIndex == LB_ERR) {
        if (wParam == IDD_A20_ADDCHG) { /* add record */
            /* ----- Insert New Data into ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

            /* ----- Set Cursor to Inserted String ----- */
            DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_SETCURSEL, (WPARAM)*lpunCur, 0L);

            /* ----- Reset Secret Code ----- */
            lpData->ulCashierSecret = 0;

            /* ----- Enable Delete and Fingerprint Buttons ----- */
            EnableWindow(GetDlgItem(hDlg, IDD_A20_DEL), TRUE);
			if(enableDPFP)
				EnableWindow(GetDlgItem(hDlg, IDD_A20_BIO_FINGER), TRUE);

            /* ----- Set Check Status ----- */
            fChk = TRUE;
        }
    } else {
        if (LOWORD(wParam) == IDD_A20_DEL) {    /* delete record */
            /* ----- Check Delete Confirmation Flag ----- */
            if (A020DispDelConf(hDlg, lpData->ulCashierNo) == IDOK) {
                /* ----- Delete Target String drom ListBox ----- */
                DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

                /* ----- Adjust ListBox Index ----- */
                dwIndex = (DWORD)(! dwIndex) ? (dwIndex  + 1) : (dwIndex);

                /* ----- Set HighLight Bar to Next Item of ListBox ----- */
                DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_SETCURSEL, (WPARAM)(dwIndex - 1), 0L);

                /* ----- Set Check Status ----- */
                fChk = TRUE;
            } else {
                return TRUE;
            }
        } else {                        /* change record */
            /* ----- Delete Old String from ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

            /* ----- Insert New Description to ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)szWork);

            /* ----- Set Cursor to Inserted String ----- */
            DlgItemSendTextMessage(hDlg, IDD_A20_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

            /* ----- Set Check Status ----- */
            fChk = FALSE;
        }
    }

    if (fChk == TRUE) {     /* ListBox is Updated   */
        /* ----- Compute the Number of Current Cashier Records ----- */
        if (wParam == IDD_A20_ADDCHG) { /* Add/Change Button is Clicked */
            (*lpunCur)++;
        } else {                        /* Delete Button is Clicked     */
            (*lpunCur)--;
        }

        /* ----- Set No. of Current Cashier Records ----- */
        LoadString(hResourceDll, IDS_A20_CURCAS, szBuff, PEP_STRING_LEN_MAC(szBuff));
        wsPepf(szWork, szBuff, *lpunCur);
        DlgItemRedrawText(hDlg, IDD_A20_CUR, szWork);
    }
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   int A020DispDelConf()
*
**  Input       :   HWND    hdlg    -   Window Handle of a DailogBox
*                   ULONG	ulNo    -   Current Selected Cashier No.
*
**  Return      :   IDOK        -   User Selection is OK
*                   IDCANCEL    -   User Selection is CANCEL
*
**  Description :
*       This function shows confirmation message for delete cashier record.
*   It returns IDOK, if user selects cashier data deletion. Otherwise it
*   return FALSE.
* ===========================================================================
*/
int A020DispDelConf(HWND hdlg, ULONG ulNo)
{
    WCHAR   szCap[PEP_CAPTION_LEN],
            szMsg[PEP_OVER_LEN],
            szWork[PEP_OVER_LEN];
    int     nRet;

    if (bConf & A20_CONF_CHK) {         /* Confirmation is ON */
        /* ----- Load Caution Message from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A20, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A20_DELCONF, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szMsg, szWork, ulNo);

        /* ----- Display Caution Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        nRet = MessageBoxPopUp(hdlg, szMsg, szCap, MB_OKCANCEL | MB_ICONEXCLAMATION);
    } else {                            /* Confirmation is OFF */
        /* ----- No Confirmation ----- */
        nRet = IDOK;
    }
    return (nRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020ChkExist()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This procedure controls push button status. If cashier no. exists in
*   edittext, it enables Add/Change button, otherwise it disables. And if
*   target cashier data is found in listbox, it enables Delete button,
*   otherwise it disables.
* ===========================================================================
*/
VOID    A020ChkExist(HWND hDlg)
{
    UINT    unNo;
    BOOL    fChk, fTrans;

    /* ----- Get Cashier No. from EditText ----- */
    unNo = GetDlgItemInt(hDlg, IDD_A20_NO, (LPBOOL)&fTrans, FALSE);
    if ((! unNo) || (fTrans == 0)) {
        fTrans = FALSE;
        unNo   = 0;
    } else {
        fTrans = TRUE;
    }

    /* ----- Search Corresponding Data from ListBox ----- */
    fChk = (A020FindNo(hDlg, unNo) == LB_ERR) ? FALSE : TRUE;

    /* ----- Enable/Disable to Add/Change Button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A20_ADDCHG),	fTrans);
    EnableWindow(GetDlgItem(hDlg, IDD_A20_DEL),		fChk);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),			fTrans);
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    A020ChkData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditId -   Control ID of Current Changed EditText
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Otherwise returns FALSE.
* ===========================================================================
*/
BOOL    A020ChkData(HWND hDlg, WORD wEditId)
{
    UINT    unValue;            /* Value of Inputed Data */
    UINT    unMax = 0;
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */
    unValue = GetDlgItemInt(hDlg, wEditId, NULL, FALSE);

    /* ----- Get ID Max Data Range ----- */
    if (wEditId == IDD_A20_CTIP) {
        unMax = (UINT)A20_CTIP_MAX;
    } else if (wEditId == IDD_A20_TERMNO) {
        unMax = (UINT)A20_TERMNO_MAX;
    }

    if (unMax < unValue) {
        fReturn = TRUE;
    }

    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A020ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A020ShowErr(HWND hDlg, WPARAM wParam)
{
    WCHAR   szCaption[PEP_CAPTION_LEN], /* MessageBox Caption */
            szMessage[PEP_OVER_LEN],    /* Error Message Strings */
            szWork[PEP_OVER_LEN];       /* Error Message Strings Work Area */
    UINT    unMin = 0;
    UINT    unMax = 0;

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A20, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Load Min & Max Data ----- */
    if (LOWORD(wParam) == IDD_A20_CTIP) {
        unMin = (UINT)A20_CTIP_MIN;
        unMax = (UINT)A20_CTIP_MAX;
    } else if (LOWORD(wParam) == IDD_A20_TERMNO) {
        unMin = (UINT)A20_TERMNO_MIN;
        unMax = (UINT)A20_TERMNO_MAX;
    }

    wsPepf(szMessage, szWork, unMin, unMax);

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A020FinDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   User Selected Button ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets file modify flag and deletes backupfile, if
*   user selects OK Button. Otherwise it stores cashier file with backup
*   file and then deletes backup file.
* ===========================================================================
*/
VOID    A020FinDlg(HWND hDlg, WPARAM wParam)
{
    WCHAR    szFile[PEP_CAPTION_LEN];
    HCURSOR hCursor;

    /* ----- Change cursor (arrow -> hour-glass) ----- */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    if (LOWORD(wParam) == IDOK) {
        /* ----- Set File Status Flag ----- */
        PepSetModFlag(hwndActMain, PEP_MF_CASH, 0);
		// DFPRFinalizeDB();	//finalize information in fingerprint database
    } else {
        /* ----- Load Cashier File Name from Resorece ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_CAS, szFile, PEP_STRING_LEN_MAC(szFile));

        /* ----- Restore Cashier File with Backup File ----- */
        PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    /* ----- Delete Backup File ----- */
    PepFileDelete();
	// DFPRDeleteTempDB(hDlg);		//removes temporary fingerprint DB file

    /* ----- Restore cursor (hour-glass -> arrow) ----- */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    /* ----- Destroy DialogBox ----- */
    EndDialog(hDlg, LOWORD(wParam));
}

/* ===== End of A020.C ===== */
