/*
* ---------------------------------------------------------------------------
* Title         :   Flexible Memory Assignment  (Prog. 2)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P002.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* ===========================================================================
*
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <Windows.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

//#include	"stringResource.h" //RPH 4-21-03
#include    <ecr.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <paraequ.h>
#include	<para.h>
#include	<pararam.h>
#include    "plu.h"
#include    <para.h>
#include    <csop.h>
#include    <cscas.h>
#include    <csetk.h>
#include    <csprgrpt.h>
#include    <r20_pif.h>
#include    "pep.h"

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "pepcalc.h"
#include    "file.h"
#include    "trans.h"
#include    "p097.h"
#include    "mypifdefs.h"
#include	"../RES/resource.h"

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
BOOL	bfPepReloadLayout = FALSE; /*This will be turned on if the layout needs to be re-loaded JHHJ*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static LPCFCONFIG   lpFile;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

static TERMINALINFO *pTermInfo;
static int nTermInfoIndex;
static int nTermLayoutIndex;
static int nTermKeyboardIndex;

static 	WCHAR		*P97_Keybdtypes[] = {
		L"Touch",
		L"NCR 5932 68 Key",
		L"PC 102 Key",
		NULL
	};

//-----------------------   Multi-lingual code
typedef struct tagCodePageTable {
	WCHAR  *tcCountry;
	UINT   unFontSize;
	UINT   unCodePage;
	WORD   wLanguage;
	WORD   wSubLanguage;
	UINT   uiCharSet;
	BYTE   ucQuality;
	BYTE   ucPitch;
} CodePageTable, *PCodePageTable;

CodePageTable  P97CodePageTable [] = {
	{WIDE("chinese-simplified"), 10, 936, 0x04, 0x02, GB2312_CHARSET, DEFAULT_QUALITY, FIXED_PITCH},
	{WIDE("english"), 8, 1252, 0x09, 0x04, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("french"), 10, 1252, 0x0c, 0x04, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("german"), 10, 1252, 0x07, 0x04, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
/*	{WIDE("greek"), 10, 1253, 0x08, 0x00, GREEK_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("italian"), 10, 1252, 0x10, 0x01, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("japanese"), 10, 932, 0x11, 0x04, SHIFTJIS_CHARSET, PROOF_QUALITY, FIXED_PITCH}, */
	{WIDE("korean"), 8, 949, 0x12, 0x01, HANGUL_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH}, /* ESMITH */
/*	{WIDE("russian"), 10, 1251, 0x19, 0x00, RUSSIAN_CHARSET, PROOF_QUALITY, FIXED_PITCH}, */
	{WIDE("spanish"), 8, 1252, 0x0a, 0x03, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{NULL, 1252, 10, 0x00, 0x02, ANSI_CHARSET, PROOF_QUALITY, FIXED_PITCH}
};

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P097DlgProc()
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
*       This is a dialgbox procedure for the Flexible Memory Assignment.
* ===========================================================================
*/
BOOL    WINAPI  P097DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static DWORD    dwFreeBlock,
                    dwTotalMem,
                    adwMem[MAX_FLXMEM_SIZE];
    static P97PARA  Para;
    static HGLOBAL  hMem;
    BOOL            fChk;
	UCHAR			unI,
					unData;
	static int ipCheck; /* used to run for-loops to validate IP inputs - CSMALL */

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Initialize dialogbox ----- */

        if (P97InitDlg(hDlg, &dwFreeBlock, adwMem, &Para, &hMem) == TRUE) {
                                                      /* configration error */

            /* ----- Destroy dialogbox ----- */

            EndDialog(hDlg, FALSE);

        }
		SendMessage (hDlg, WM_SETFONT, 0, 0);

        /* ----- Enable/Disable to push OK button ----- */

        //EnableWindow(GetDlgItem(hDlg, IDOK), fChk);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int i;

			SendDlgItemMessage(hDlg, IDD_P97_EB_CODEPAGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_EB_REGISTERNO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_EB_STORENUMBER, WM_SETFONT, (WPARAM)hResourceFont, 0);
			//SendDlgItemMessage(hDlg, IDD_P97_EB_DEFLAYOUT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_CB_DEFLAYOUT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_CB_TERMNO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_CB_MENUPAGENO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_CB_KEYB_TYPE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P97_CB_LANGUAGE, WM_SETFONT, (WPARAM)hResourceFont, 0);

			SendDlgItemMessage(hDlg, IDC_EB_UnlockCode, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_EB_TermSerialNo, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_EB_IP1, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_EB_IP2, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_EB_IP3, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_EB_IP4, WM_SETFONT, (WPARAM)hResourceFont, 0);
		
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);

			for (i = IDD_P97_STATIC_FIRST; i <= IDD_P97_STATIC_LAST; i++) {
				SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}

		return FALSE;

    case PM_GET_FCONF:

        /* ----- Receive Address of File Configulation from PEP:Main ----- */

        lpFile = (LPCFCONFIG)lParam;
        
        return TRUE;


    case WM_COMMAND:
        switch (LOWORD(wParam)) {

		/* Check IP fields as address fields are entered - CSMALL 11/09/05 */
		case IDC_EB_IP1:
		case IDC_EB_IP2:
		case IDC_EB_IP3:
		case IDC_EB_IP4:
			if (HIWORD(wParam) == EN_UPDATE ) {
				for(ipCheck = IDC_EB_IP1; ipCheck <= IDC_EB_IP4; ipCheck++)
				{
					if ( ValidateIP(hDlg, ipCheck) ) 
					{
						DispErrorIP(hDlg, ipCheck);
						return TRUE;
					}
				}				
			}
			break;

		case  IDD_P97_CB_TERMNO:

            if (HIWORD(wParam) == CBN_SELCHANGE ) {      /* data change */

				int nIdex;
				nIdex = SendDlgItemMessage(hDlg, IDD_P97_CB_TERMNO,
                       CB_GETCURSEL, 0, 0L);

				for(ipCheck = IDC_EB_IP1; ipCheck < IDC_EB_IP4; ipCheck++)
				{
					if ( ValidateIP(hDlg, ipCheck) ) 
					{
						DispErrorIP(hDlg, ipCheck);
						return TRUE;
					}
				}
			
				if (nIdex != CB_ERR) {
					P97GetDlgData(hDlg, pTermInfo, nTermInfoIndex);
					pTermInfo = &(Para.aCurt [nIdex]); nTermInfoIndex = nIdex + 1;
					P97InitDlgData (hDlg, pTermInfo, nTermInfoIndex);
				}
			}

			break;
		case IDD_P97_CB_LANGUAGE:
            if (HIWORD(wParam) == CBN_SELCHANGE) {      /* data change */
				int nIdex = SendDlgItemMessage(hDlg, IDD_P97_CB_LANGUAGE,
                       CB_GETCURSEL, 0, 0L);
				if (nIdex != CB_ERR) {
					/* 
						Used to represent whether a user has changed the code page
						if they have not set the codepage, then when they change
						language selections the codepage for that selection will 
						change also. If they have set the codepage manually, then 
						the codepage will not change when they select a new language.
						ESMITH
					*/
					if(!SendDlgItemMessage(hDlg,
								   IDD_P97_EB_CODEPAGE,
								   EM_GETMODIFY,
								   0,
								   0))
					{
						P97GetDlgData(hDlg, pTermInfo, nTermInfoIndex);
						pTermInfo->ulCodePage = P97CodePageTable[nIdex].unCodePage;
						P97InitDlgData (hDlg, pTermInfo, nTermInfoIndex);
					}
				}
			}
			break;
        case IDOK:

			for(ipCheck = IDC_EB_IP1; ipCheck <= IDC_EB_IP4; ipCheck++)
			{
				if ( ValidateIP(hDlg, ipCheck) ) 
				{
					DispErrorIP(hDlg, ipCheck);
					return TRUE;
				}
			}

			for(unI = 0; unI < P097_ERR_CHECK; unI++)
			{
				/* ---- Check Inputed Data is Valid or Not ---- ESMITH */
				unData = (UCHAR)(GetDlgItemInt(hDlg,
                                     IDD_P97_EB_REGISTERNO + unI,
                                     (LPBOOL)&fChk,
                                     FALSE));

				if (fChk) {
					fChk = ((GetDlgItemInt(hDlg,IDD_P97_EB_REGISTERNO + unI,NULL,TRUE)  >= P097_STORENO_MIN) && 
							(GetDlgItemInt(hDlg,IDD_P97_EB_REGISTERNO + unI,NULL,TRUE) <= P097_STORENO_MAX));
				}

				if (!fChk) {

					/* ----- Display Error Message ----- */

					MessageBeep(MB_ICONEXCLAMATION);
					MessageBoxPopUp(hDlg,
							   P097_STORENO_ERR,
							   P097_CAPTION,
							   MB_ICONEXCLAMATION | MB_OK);

					/* ----- Set Input Focus to EditText ----- */

					SendDlgItemMessage(hDlg,
									   IDD_P97_EB_REGISTERNO + unI,
									   EM_SETSEL,
									   TRUE,
									   MAKELONG(0, -1));

					SetFocus(GetDlgItem(hDlg, IDD_P97_EB_REGISTERNO + unI));

					return TRUE;

				}
			}


			P97GetDlgData(hDlg, pTermInfo, nTermInfoIndex);
        case IDCANCEL:

            /* ----- Get data and save them to file ----- */

            if (P97FinDlg(hDlg, &Para, hMem, wParam) == TRUE) {

                /* ----- Destroy dialogbox ----- */

                EndDialog(hDlg, TRUE);

                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P97InitDlg()
*
*   Input   :   HWND        hDlg    - handle of a dialogbox procedure
*               LPDWORD     lpdwFree - address of free block
*               LPDWORD     lpwMem  - address of memory useage
*               LPP02PARA   lpPara  - address of data buffer
*               LPHGLOBAL   lphMem  - handle of global memory lock for description
**
*   Return  :   TRUE                - error occured
*               FALSE               - user process is executed
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
BOOL    P97InitDlg(HWND hDlg, LPDWORD lpdwFree, LPDWORD lpdwMem,
                            LPP97PARA lpPara, LPHGLOBAL lphMem)
{
	WCHAR       wcLayout[20] = L"PARAMLA";
    WORD        wI;
	WCHAR		szTemp [64];
    USHORT      usReturnLen;
	WCHAR    szWork[64];
	UINT    nID;
    WCHAR    szMsg[128];

    /* ----- Set max entry data length ----- */
    /* ---- Register Number range updated to limit 0 to 3 ---- */
    SendDlgItemMessage(hDlg, IDD_P97_EB_CODEPAGE, EM_LIMITTEXT, 5, 0L);
    SendDlgItemMessage(hDlg, IDD_P97_EB_REGISTERNO, EM_LIMITTEXT, P097_MAX_REGNOLEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P97_EB_STORENUMBER, EM_LIMITTEXT, P097_MAX_STORELEN, 0L);
    //SendDlgItemMessage(hDlg, IDD_P97_EB_DEFLAYOUT, EM_LIMITTEXT, 30, 0L);
/*CSMALL - limit sizes for unlock code, serial no
*/	SendDlgItemMessage(hDlg, IDC_EB_UnlockCode, EM_LIMITTEXT, 11, 0L);
	SendDlgItemMessage(hDlg, IDC_EB_TermSerialNo, EM_LIMITTEXT, 15, 0L);
	SendDlgItemMessage(hDlg, IDC_EB_IP1, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDC_EB_IP2, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDC_EB_IP3, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDC_EB_IP4, EM_LIMITTEXT, 3, 0L);

    /* ----- Store initial data for checking chage ----- */

    memcpy(lpPara->aPrev, lpPara->aCurt, sizeof(FLEXMEM) * MAX_FLXMEM_SIZE);

    /* ----- Set data to the edit-box ----- */
	LoadString(hResourceDll, IDS_P97_TYPE0, szWork, PEP_STRING_LEN_MAC(szWork));
	DlgItemSendTextMessage(hDlg, IDD_P97_ODT_CB,CB_INSERTSTRING, (WPARAM)-1, (LPARAM)szWork);
	for(wI = 0; wI < 5; wI++)
	{
		DlgItemSendTextMessage(hDlg, IDD_P97_ODT_CB,CB_INSERTSTRING, (WPARAM)-1, (LPARAM)ParaTransMnemo[300+wI]);
	}

	for (wI = 1; wI <=16; wI++) {
		_ltow(wI, szTemp, 10);

		DlgItemSendTextMessage(hDlg,
				IDD_P97_CB_TERMNO,
				CB_INSERTSTRING,
				(WPARAM)-1,
				(LPARAM)(szTemp));
	}

	SendDlgItemMessage(hDlg,
                       IDD_P97_CB_TERMNO,
                       CB_SETCURSEL,
                       0,
                       0L);

	for (wI = 1; wI <= 10; wI++) {
		_ltow(wI, szTemp, 10);

		DlgItemSendTextMessage(hDlg,
				IDD_P97_CB_MENUPAGENO,
				CB_INSERTSTRING,
				(WPARAM)-1,
				(LPARAM)(szTemp));
	}

	DlgItemSendTextMessage(hDlg,
			IDD_P97_CB_DEFLAYOUT,
			CB_INSERTSTRING,
			(WPARAM)-1,
			(LPARAM)(L"DEFAULT"));
	for (wI = 0; wI < 26; wI++) {
		DlgItemSendTextMessage(hDlg,
				IDD_P97_CB_DEFLAYOUT,
				CB_INSERTSTRING,
				(WPARAM)-1,
				(LPARAM)(wcLayout));
		wcLayout[6]++;
	}

	SendDlgItemMessage(hDlg,
                       IDD_P97_CB_DEFLAYOUT,
                       CB_SETCURSEL,
                       0,
                       0L);

   for (wI = 0, nID = IDS_FILE_KEY_CONVENT; wI < FILE_NUM_KB ; wI++, nID++) {
    LoadString(hResourceDll, nID, szMsg, PEP_STRING_LEN_MAC(szMsg));
    DlgItemSendTextMessage(hDlg,
                       IDD_P97_CB_KEYB_TYPE,
                       CB_INSERTSTRING,
                       (WPARAM)-1,
                       (LPARAM)szMsg);
   }

	/* Set Available Language Types ESMITH */
	for (wI = 0; P97CodePageTable[wI].tcCountry; wI++) {
		DlgItemSendTextMessage(hDlg,
			IDD_P97_CB_LANGUAGE,
			CB_INSERTSTRING,
			(WPARAM)-1,
			(LPARAM)(P97CodePageTable[wI].tcCountry));
	}

    ParaAllRead(CLASS_PARATERMINALINFO,
                (UCHAR *)lpPara->aPrev,
                sizeof(lpPara->aPrev),
                0,
                (USHORT *)&usReturnLen);

	memcpy (lpPara->aCurt, lpPara->aPrev, sizeof(lpPara->aPrev));

	pTermInfo = &(lpPara->aCurt [0]); nTermInfoIndex = 1;

	P97InitDlgData (hDlg, pTermInfo, nTermInfoIndex);

    return FALSE;
}

BOOL    P97InitDlgData(HWND hDlg, TERMINALINFO *pTermInfo, int nIndex)
{
	int nCurSel;
	int nLangSel;
	int nVal=0;

	// From the encoded keyboard type we determine which one of the
	// items in the keyboard selection ComboBox control we will select.
	// Since the ComboBox is filled with strings beginning with
	// IDS_FILE_KEY_CONVENT to the number of different keyboard types,
	// we will use a IDS_FILE_KEY_CONVENT as a zero value by subtracting
	// IDS_FILE_KEY_CONVENT from a specific IDS_FILE_KEY_aaa value to
	// derive a value in the range from 0 to the number of items in the
	// ComboBox list.  This difference will then be the value to use in the
	// cursor selection.  We use a similar method when converting from a
	// selected value in the ComboBox back to the keyboard type.
	//
	// An example of the #defines for the different keyboard description
	// strings are:
	//		#define IDS_FILE_KEY_CONVENT    (IDS_FILE_TERM_MASTER + 3)
	//		#define IDS_FILE_KEY_MICRO      (IDS_FILE_TERM_MASTER + 4)
	//		#define IDS_FILE_KEY_WEDGE_68   (IDS_FILE_TERM_MASTER + 5)
	//		#define IDS_FILE_KEY_PC_106		(IDS_FILE_TERM_MASTER + 6)
	//		#define IDS_FILE_KEY_TOUCH		(IDS_FILE_TERM_MASTER + 7)
	//
	// See function P97InitDlg() above for the ComboBox initialization.

#if (IDS_FILE_KEY_CONVENT > IDS_FILE_KEY_MICRO) || (IDS_FILE_KEY_CONVENT > IDS_FILE_KEY_TOUCH)
#pragma message("  *** WARNING:  Dependency on IDS_FILE_KEY_CONVENT numbering BROKEN.")
#endif
	if (pTermInfo->uchKeyBoardType == KEYBOARD_TOUCH) {
		nCurSel = IDS_FILE_KEY_TOUCH - IDS_FILE_KEY_CONVENT;
	}
	else if (pTermInfo->uchKeyBoardType == KEYBOARD_WEDGE_68) {
		nCurSel = IDS_FILE_KEY_WEDGE_68 - IDS_FILE_KEY_CONVENT;
	}
	else if (pTermInfo->uchKeyBoardType == KEYBOARD_WEDGE_78) {
		nCurSel = IDS_FILE_KEY_WEDGE_78 - IDS_FILE_KEY_CONVENT;
	}
	else if (pTermInfo->uchKeyBoardType == KEYBOARD_PCSTD_102) {
		nCurSel = IDS_FILE_KEY_PC_106 - IDS_FILE_KEY_CONVENT;
	}
	else if (pTermInfo->uchKeyBoardType == KEYBOARD_CONVENTION) {
		nCurSel = IDS_FILE_KEY_CONVENT - IDS_FILE_KEY_CONVENT;
	}
	else if (pTermInfo->uchKeyBoardType == KEYBOARD_MICRO) {
		nCurSel = IDS_FILE_KEY_MICRO - IDS_FILE_KEY_CONVENT;
	}else if (pTermInfo->uchKeyBoardType == KEYBOARD_TICKET) {
		nCurSel = IDS_FILE_KEY_TICKET - IDS_FILE_KEY_CONVENT;
	}else {
		// in case the keyboard type is bogus, lets set to default
		pTermInfo->uchKeyBoardType = KEYBOARD_TOUCH;
		nCurSel = IDS_FILE_KEY_TOUCH - IDS_FILE_KEY_CONVENT;
	}

	//Set the currently selected index of the keyboard type to the static variable
	//so that we can check it when we are closing the dialog. This will help us
	//determine whether or not we need to reloadthe layout on transfer.  JHHJ
	nTermKeyboardIndex = nCurSel;

	SendDlgItemMessage(hDlg,
                       IDD_P97_CB_KEYB_TYPE,
                       CB_SETCURSEL,
                       nCurSel,
                       0L);

	//  Set the menu page number to the correct value
	if (pTermInfo->uchMenuPageDefault < 1)
		pTermInfo->uchMenuPageDefault = 1;

	if (pTermInfo->uchMenuPageDefault > 10)
		pTermInfo->uchMenuPageDefault = 10;

	SendDlgItemMessage(hDlg,
                       IDD_P97_CB_MENUPAGENO,
                       CB_SETCURSEL,
                       pTermInfo->uchMenuPageDefault - 1,
                       0L);

//		DlgItemSendTextMessage(hDlg,
//			IDD_P97_EB_DEFLAYOUT,
//			WM_SETTEXT,
//			(WPARAM)-1,
//			(LPARAM)(pTermInfo->aszDefaultMenuFileName));

	if (wcsncmp (pTermInfo->aszDefaultMenuFileName, L"PARAML", 6) == 0) {
		nCurSel = pTermInfo->aszDefaultMenuFileName[6] - 0x40;
	} else {
		nCurSel = 0;
	}

	//Set the currently selected index of the layout to the static variable
	//so that we can check it when we are closing the dialog. This will help us
	//determine whether or not we need to reloadthe layout on transfer.  JHHJ
	nTermLayoutIndex = nCurSel;


		SendDlgItemMessage(hDlg,
                       IDD_P97_CB_DEFLAYOUT,
                       CB_SETCURSEL,
                       nCurSel,
                       0L);

          SetDlgItemInt(hDlg,
                        IDD_P97_EB_CODEPAGE,
                        pTermInfo->ulCodePage,
                        FALSE);

          SetDlgItemInt(hDlg,
                        IDD_P97_EB_STORENUMBER,
                        pTermInfo->usStoreNumber,
                        FALSE);

          SetDlgItemInt(hDlg,
                        IDD_P97_EB_REGISTERNO,
                        pTermInfo->usRegisterNumber,
                        FALSE);
		  /* getting EditBox value from STRUCT attributes - CSMALL 11/09/05*/
		  DlgItemSendTextMessage(hDlg,
						IDC_EB_UnlockCode,
						WM_SETTEXT,
						(WPARAM)( (sizeof(pTermInfo->aszUnLockCode))),
						(LPARAM)(pTermInfo->aszUnLockCode));
		  DlgItemSendTextMessage(hDlg,
						IDC_EB_TermSerialNo,
						WM_SETTEXT,
						(WPARAM)( (sizeof(pTermInfo->aszTerminalSerialNumber))),
						(LPARAM)(pTermInfo->aszTerminalSerialNumber));

		  SetDlgItemInt(hDlg,
						IDC_EB_IP1,
						pTermInfo->ucIpAddress[0],
						FALSE);
		  SetDlgItemInt(hDlg,
						IDC_EB_IP2,
						pTermInfo->ucIpAddress[1],
						FALSE);
		  SetDlgItemInt(hDlg,
						IDC_EB_IP3,
						pTermInfo->ucIpAddress[2],
						FALSE);
		  SetDlgItemInt(hDlg,
						IDC_EB_IP4,
						pTermInfo->ucIpAddress[3],
						FALSE);

/* ----- Set Language From Language ID ESMITH ----- */
	nLangSel = 1;  // The default for the language selector is English
	for (nVal = 0; P97CodePageTable[nVal].wLanguage; nVal++) {

		if(P97CodePageTable[nVal].wLanguage == pTermInfo->usLanguageID)
			nLangSel = nVal;
	}

	SendDlgItemMessage(hDlg,
                       IDD_P97_CB_LANGUAGE,
                       CB_SETCURSEL,
                       nLangSel,
                       0L);

	SendDlgItemMessage(hDlg, IDD_P97_ODT_CB,CB_SETCURSEL, (WPARAM)pTermInfo->uchOrderDeclaration, 0);

	return TRUE;
}

BOOL    P97GetDlgData(HWND hDlg, TERMINALINFO *pTermInfo, int nIndex)
{
	int nCurSel;
	int ipCheck;

    /*
		----- Read data from dialog box -----
		Obtain the data items from the various controls of the displayed
		dialog box and put those data into the correct terminal information
		struct.  We are using the nCurSel == IDS_FILE_KEY_TOUCH - IDS_FILE_KEY_CONVENT
		construction so as to provide some level of independence from the hard coded
		values and the sequence of the list in the ComboBox.  See how strings are
		put into the ComboBox in the function P97InitDlg() above.
	*/

	nCurSel = SendDlgItemMessage(hDlg,
                       IDD_P97_CB_KEYB_TYPE,
                       CB_GETCURSEL,
                       0,
                       0L);
	if (nCurSel == CB_ERR)
		nCurSel = 0;

	if (nCurSel == (IDS_FILE_KEY_TOUCH - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_TOUCH;
	}
	else if (nCurSel == (IDS_FILE_KEY_WEDGE_68 - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_WEDGE_68;
	}
	else if (nCurSel == (IDS_FILE_KEY_WEDGE_78 - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_WEDGE_78;
	}
	else if (nCurSel == (IDS_FILE_KEY_PC_106 - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_PCSTD_102;
	}
	else if (nCurSel == (IDS_FILE_KEY_CONVENT - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_CONVENTION;
	}
	else if (nCurSel == (IDS_FILE_KEY_MICRO - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_MICRO;
	}else if (nCurSel == (IDS_FILE_KEY_TICKET - IDS_FILE_KEY_CONVENT)) {
		pTermInfo->uchKeyBoardType = KEYBOARD_TICKET;
	}

	//We check to see if the Currently set Keyboard type is the same as the one 
	//as the one that we opened. If so, we do not need to reload the layout. JHHJ
	if(nTermKeyboardIndex != nCurSel)
	{
		bfPepReloadLayout = TRUE;
	}else
	{
		bfPepReloadLayout = FALSE;
	}

	//  Set the Language ID and the sublanguage id ESMITH
		nCurSel = SendDlgItemMessage(hDlg,
                       IDD_P97_CB_LANGUAGE,
                       CB_GETCURSEL,
                       0,
                       0L);

	pTermInfo->usLanguageID = P97CodePageTable[nCurSel].wLanguage;
	pTermInfo->usSubLanguageID = P97CodePageTable[nCurSel].wSubLanguage;

	pTermInfo->uchOrderDeclaration = SendDlgItemMessage(hDlg,
														  IDD_P97_ODT_CB,
														  CB_GETCURSEL,
														  0,
														  0L);

	//  Set the menu page number to the correct value
	nCurSel = SendDlgItemMessage(hDlg,
                       IDD_P97_CB_MENUPAGENO,
                       CB_GETCURSEL,
                       0,
                       0L);

	// Force Touchscreen key board menu page to be page 10
	if (pTermInfo->uchKeyBoardType == KEYBOARD_TOUCH)
		nCurSel = 9;

	pTermInfo->uchMenuPageDefault = nCurSel + 1;

	nCurSel = SendDlgItemMessage(hDlg,
                       IDD_P97_CB_DEFLAYOUT,
                       CB_GETCURSEL,
                       0,
                       0L);
	if(nCurSel != nTermLayoutIndex)
	{
		if (nCurSel == 0 || nCurSel > 26) {
			wcscpy (pTermInfo->aszDefaultMenuFileName, L"DEFAULT");
		} else {
			wcscpy (pTermInfo->aszDefaultMenuFileName, L"PARAMLA");
			nCurSel--;
			pTermInfo->aszDefaultMenuFileName[6] += nCurSel;
		}

		//Because we have  changed the layout file that is to be used, we need to reload the layout
		//JHHJ
		bfPepReloadLayout = TRUE;
	}else
	{
		bfPepReloadLayout = FALSE;
	}

//		DlgItemSendTextMessage(hDlg,
//			IDD_P97_EB_DEFLAYOUT,
//			WM_GETTEXT,
//			(WPARAM)(sizeof (pTermInfo->aszDefaultMenuFileName)),
//			(LPARAM)(pTermInfo->aszDefaultMenuFileName));

          pTermInfo->ulCodePage = GetDlgItemInt(hDlg,
                        IDD_P97_EB_CODEPAGE,
                        &nCurSel,
                        FALSE);

          pTermInfo->usStoreNumber = GetDlgItemInt(hDlg,
                        IDD_P97_EB_STORENUMBER,
                        &nCurSel,
                        FALSE);

          pTermInfo->usRegisterNumber = GetDlgItemInt(hDlg,
                        IDD_P97_EB_REGISTERNO,
                        &nCurSel,
                        FALSE);
		  /* setting STRUCT attributes from EditBoxes - CSMALL 11/09/05*/
		  DlgItemSendTextMessage(hDlg,
						IDC_EB_UnlockCode,
						WM_GETTEXT,
						(WPARAM)( sizeof(pTermInfo->aszUnLockCode) ),
						(LPARAM)(pTermInfo->aszUnLockCode));
		  /* // Fix for SR 802 - No need to reset STRUCT from
			 // editBox, field is read-only - CSMALL 11/30/05
		  DlgItemSendTextMessage(hDlg,
						IDC_EB_TermSerialNo,
						WM_GETTEXT,
						(WPARAM)( sizeof(pTermInfo->aszTerminalSerialNumber) ),
						(LPARAM)(pTermInfo->aszTerminalSerialNumber));
		  */
		for( ipCheck = IDC_EB_IP1; ipCheck <= IDC_EB_IP4; ipCheck++)
		{
			if ( ValidateIP(hDlg, ipCheck) ) {
				DispErrorIP(hDlg, ipCheck);
			}
			else {
				pTermInfo->ucIpAddress[ipCheck-IDC_EB_IP1] = GetDlgItemInt(hDlg,
						ipCheck,
						&nCurSel,
						FALSE);
			}
		}

  return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02FinDlg()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               LPP02PARA   lpPara - address of data buffer
*               HGLOBAL     hMem   - handle of global memory lock for description
*               WORD        wID    - parameter of dialogbox ID
**
*   Return  :   TRUE               - user process is executed.
*               FALSE              - cancel process is expected.
*
**  Description:
*       This procedure finalize dialogbox.
* ===========================================================================
*/
BOOL    P97FinDlg(HWND hDlg, LPP97PARA lpPara, HGLOBAL hMem, WPARAM wID)
{
    USHORT      usReturnLen;
    HCURSOR     hCursor;


    if (LOWORD(wID) == IDOK) {       /* OK button pushed */

        /* ----- Change cursor (arrow -> hour-glass) ----- */

        hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
        ShowCursor(TRUE);


        /* ----- Get data from dialogbox ----- */

        /* ----- Check whether size changed or not and create file ----- */

        /* ----- Write data to file ----- */

        ParaAllWrite(CLASS_PARATERMINALINFO,
                     (UCHAR *)lpPara->aCurt,
                     sizeof(lpPara->aCurt),
                     0,
                     &usReturnLen);

        /* ----- Set Transaction Flag ----- */

//        SendMessage(hwndProgMain, PM_SET_TRAFLAG, (WPARAM)Check.fwFile, 0L);

        /* ----- Set file status flag ----- */
		/* Add Status Flag to show that P97 has been changed ESMITH*/
          PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0); 

//        PepSetModFlag(hwndProgMain, Check.nModFileID, 0);

        /* ----- Restore cursor (hour-glass -> arrow) ----- */

        ShowCursor(FALSE);
        SetCursor(hCursor);

    }

    /* ----- Free memory area ----- */

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    ValidateIP(HWND hDlg, WPARAM wID)
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed IP address fields are
*		valid. It returns TRUE, if the inputed data is out of range. Otherwise
*		it returns FALSE.
* ===========================================================================
*/
BOOL    ValidateIP(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    short   nRndPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */

    nRndPos = (short)GetDlgItemInt(hDlg, wID, NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */

        switch (LOWORD(wID)) {
        
        case    IDC_EB_IP1:
        case    IDC_EB_IP2:
        case    IDC_EB_IP3:
        case    IDC_EB_IP4:
            if ((nRndPos < P097_IP_MIN) || (nRndPos > P097_IP_MAX))
            {

                 fCheck = TRUE;
            }

        default:
            break;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    DispErrorIP()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*		MessageBox.
* ===========================================================================
*/
VOID    DispErrorIP(HWND hDlg, WPARAM wID)
{
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN];
            /* szCaption[PEP_CAPTION_LEN]; */

    /* ----- Load String from Resource ----- */

/*    LoadString(hInst, IDS_PEP_CAPTION_IP, (LPSTR)szCaption, PEP_CAPTION_LEN);*/

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */

    wsPepf(szMessage, szWork, P097_IP_MIN, P097_IP_MAX);

    /* ----- Display Error Message with MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               WIDE("IP Address"),
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */

    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}


/* ===== End of P002.C ===== */