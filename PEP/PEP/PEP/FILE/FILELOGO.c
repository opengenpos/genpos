/*
* ---------------------------------------------------------------------------
* Title         :   LOGO File Procedure
* Category      :   File, PEP 2.0 for Windows (HP US Model)
* Program Name  :   FileLOGO.C
* Copyright (C) :   2005, Georgia Southern University, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               LOGOMNGRExec():           Report Execute user selecting file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* June-10-05 : 02.00.00 : J. Hall    : Initial (2.0)
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
#include    <windows.h>
#include    <string.h>
#include    <shellapi.h>
#include    <r20_pif.h>
#include    "ecr.h"
#include    "paraequ.h"
#include    "para.h"
#include	"report.h"
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include	"file.h"
#include	"filel.h"

#define	FDNLEN			10			// length of file name + 1

WCHAR        szCommLogoSaveName[PEP_FILENAME_LEN];
WCHAR		 szCommIconSaveName[PEP_FILENAME_LEN];

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL LOGOExec(HANDLE hInst, HWND hwnd)      *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "LOGO Manager"    *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL LogoExec(HANDLE hInst, HWND hwnd, WCHAR* szLOGO, LANGID xlan)
{
    int     iLen;
    WCHAR    szDir[LOGO_MAXLEN] = {0};
    WCHAR    szExecFile[LOGO_MAXLEN] = {0};
    WCHAR    szErrMsg[LOGO_MAXLEN_TEXT] = {0};
	WCHAR    szLOGOCaption[LOGO_MAXLEN_TEXT] = {0};
	WCHAR	 szLOGOFileName[PEP_FILENAME_LEN] = {0};

	WCHAR szExec[] = WIDE("\\System32\\mspaint.exe");


	GetEnvironmentVariableW(WIDE("SystemRoot"),szExecFile, sizeof(szExecFile));
	lstrcatW(szExecFile, szExec);

	/*This is necessary for files that are on the desktop, or a location with spaces in it
	(i.e. C:\Documents and Settings\user\Desktop).  The ShellExecute command can cause errors when
	spaces are found, JHHJ*/

	//load file name of LOGO file to open
	memset(szLOGOFileName, 0x00, sizeof(szLOGOFileName));
	szLOGOFileName[0] = _T('\"');
	wcscat(szLOGOFileName, szLOGO);
	iLen = wcslen(szLOGOFileName);
	szLOGOFileName[iLen] = _T('\"');


    if (ShellExecutePep(hwnd, NULL, szExecFile, szLOGOFileName, szDir, SW_SHOWNORMAL) < 32) {
        LoadString(hResourceDll, IDS_LOGO_ERR_EXEC, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUp(hwnd,            /* output error message */
                   szErrMsg,
                   szLOGOCaption,
                   MB_ICONEXCLAMATION | MB_OK);
    }

    return TRUE;
}


/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL LOGOFile(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "LOGO file"     *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL LogoFile(HANDLE hInst, HWND hwnd,WCHAR* szLOGO, UINT nFlag) //ESMITH LOGO
{
    OPENFILENAMEW    ofn;
    WCHAR    szReportName[LOGO_MAXLEN];
    WCHAR    szFilter[LOGO_MAXLEN];
    WCHAR    szTitle[LOGO_MAXLEN];
    WCHAR    szErrMsg[LOGO_MAXLEN_TEXT];
    WCHAR    szReportCaption[LOGO_MAXLEN_TEXT];
    WCHAR    chReplace;
	WCHAR    szExePath[LOGO_MAXLEN];
	WCHAR	 szFileLOGODir[PEP_FILENAME_LEN];
	WCHAR	 szExtension[] = WIDE("dat");
    WCHAR	 *p;
	UINT     cbString, i;
	int		 loopFlag, loopCount;
	DWORD	 fileError;
    HANDLE	 hLOGOFile;

    LoadString(hResourceDll, IDS_LOGO_FILE, szTitle, PEP_STRING_LEN_MAC(szTitle));
    LoadString(hResourceDll, IDS_LOGO_EXT, szExtension, PEP_STRING_LEN_MAC(szExtension));

    cbString = LoadString(hResourceDll, IDS_LOGO_FILTER, szFilter, PEP_STRING_LEN_MAC(szFilter));
    if (cbString == 0) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone*/
        LoadString(hResourceDll, IDS_LOGO_ERR_LOAD, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        LoadString(hResourceDll, IDS_LOGO_CAP_FILE, szReportCaption, PEP_STRING_LEN_MAC(szReportCaption));
        MessageBoxPopUp(hwnd,            /* output error message */
                   szErrMsg,
                   szReportCaption,
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }
    chReplace = 0x7c;
    for (i=0; szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace) {
            szFilter[i] = '\0';
        }
    }

	//RPH SR#102, 103
	//check to see if LOGO file set in PEP exists if the file does not
	//exist set the path to the pep.exe directory otherwise we will
	//set the name of the file to "set/open" to that file
	hLOGOFile = CreateFilePep(szLOGO, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	fileError = GetLastError();

	if (fileError != ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hLOGOFile);

#if 1
		GetPepTemporaryFolder(NULL, szExePath, PEP_STRING_LEN_MAC(szExePath));
#else
		GetPepModuleFileName(hInst, szExePath, LOGO_MAXLEN);
		szExePath[wcslen(szExePath)-8] = '\0';  //remove pep.exe from path
#endif
		//if the LOGO file is set we use the file name and remove the path
		//This is used to set the file name to search for in Open file dialog
		if(szLOGO[0]	 != '\0')
		{
			LoadString(hResourceDll, IDS_LOGO_ERR_PATH, szTitle, PEP_STRING_LEN_MAC(szTitle));
			loopCount = 0; //counts loop at end and subtracts the extension amount to replace

			for(loopFlag = 1, p = szLOGO + wcslen(szLOGO); loopFlag && p > szLOGO; p--)
			{
				switch(*p) {
				case '/':
				case '\\':
					loopCount--;  //adjusts the amount copied (don't include the slash)
					wcscpy(szReportName, szLOGO + wcslen(szLOGO) - loopCount);
					loopFlag = 0;
					break;
				}
				loopCount++;
			}
		}
		if(nFlag == 0x00008000)
		{
			LoadString(hResourceDll, IDS_LOGO_TOUCH_ERR, szTitle, PEP_STRING_LEN_MAC(szTitle));
			szReportName[0] = '\0';
		}
		else //if(szCommKeyboardLOGOSaveName[0] == '\0')
		{
			LoadString(hResourceDll, IDS_LOGO_KEYBOARD_ERR, szTitle, PEP_STRING_LEN_MAC(szTitle));
			szReportName[0] = '\0';
		}
	}
	else
	{
		wcscpy(szReportName, szLOGO);
		CloseHandle(hLOGOFile);
	}
	if(szFileLOGODir == '\0'){
		lstrcpyW(szFileLOGODir, szExePath);
	}
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
	ofn.lpstrDefExt = szExtension;
    ofn.lpstrFile = szReportName;
    ofn.nMaxFile = PEP_STRING_LEN_MAC(szReportName);
	//ofn.nMaxFile = sizeof(szReportName);
    ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = szFileLOGODir;
    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    if (GetPepOpenFileName(&ofn)) {
        lstrcpyW(szLOGO, szReportName);
        lstrcpyW(szFileLOGODir, szReportName);
        PepSetModFlag(hwndPepMain, nFlag, 0);
        return TRUE;
    }
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    BOOL CurrentLOGOFile(HANDLE hInst, HWND hWnd)
*     Input:    hInst - instance to bind
*               hWnd  - Handle of Dialog Procedure
*
** Return:      nothing
*
** Description: This function displays the dialog box of current
*               LOGO file name.
*===========================================================================
*/
BOOL CurrentLogoFile(HANDLE hInst, HWND hWnd)
{
    DLGPROC lpitDlgProc;

    lpitDlgProc = MakeProcInstance(CurrentLogoFileDlgProc,
                                   hInst);
    DialogBoxPopup(hResourceDll,
              MAKEINTRESOURCEW(IDD_REPORT_CURRENT),
              hWnd,
              lpitDlgProc);

    FreeProcInstance(lpitDlgProc);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  CurrentRptFileDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of About DialogBox
*                   WORD    wMsg    -   Dispatched Message
*                   WORD    wParam  -   16 Bits Message Parameter
*                   LONG    lParam  -   32 Bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description :
*       This is a dialog procedure of Current LOGO File Name.
* ===========================================================================
*/
BOOL    WINAPI  CurrentLogoFileDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int     iLen=0, i;
    WCHAR    szErrMsg[LOGO_MAXLEN_TEXT];
    WCHAR    szWork[44];
    WORD    wID;                        /* Control ID for Loop */

    switch (wMsg) {

    case WM_INITDIALOG:

        /* ----- Set the Input Focus to OK Button ----- */

        SetFocus(GetDlgItem(hDlg, IDOK));

        PepSetFont(hDlg, IDD_REPORT_CURRPT1);
        PepSetFont(hDlg, IDD_REPORT_CURRPT2);
        PepSetFont(hDlg, IDD_REPORT_CURRPT3);
        PepSetFont(hDlg, IDD_REPORT_CURRPT4);
        PepSetFont(hDlg, IDD_REPORT_CURRPT5);
        PepSetFont(hDlg, IDD_REPORT_CURRPT6);

        /* Variable no longer used due to change from
			one LOGO to multiple LOGOs: removed to
			clear error.
		*/
		//iLen = wcslen(szCommLOGOSaveName);      /* get length */
        if (iLen == 0) {

            LoadString(hResourceDll, IDS_REPORT_ERR_NAME, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

            DlgItemRedrawText(hDlg, IDD_REPORT_CURRPT1, szErrMsg);

            return TRUE;
        }
        for (i = 0, wID = IDD_REPORT_CURRPT1; iLen > 0; i += 43, iLen -= 43, wID++) {
            *szWork = '\0';
            DlgItemRedrawText(hDlg, wID, szWork);
        }

        return TRUE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case    IDOK:
        case    IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Destroy DialogBox ----- */

                EndDialog(hDlg, FALSE);

                return TRUE;
            }
        }
        return FALSE;

    default:
        return FALSE;
    }
}
BOOL WINAPI LogoDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;
	static SHORT sFile = 0;
	static FLOGOFILE myLogoFileHeader;
	static WCHAR szLogoFileIndexName[256];
    WCHAR    szDesc[128];
	WCHAR    szTempString[MAX_PATH];
    USHORT  unI;
	ULONG	ulActualPosition;
	int     wIndex;
	SHORT	sRet;
	ULONG  ulGCNumMax,ulPercent;
	PARAFLEXMEM FlexData;

    switch (wMsg) {
    case WM_INITDIALOG:

		/* ----- Set address to PPI File ----- */
		FlexData.uchAddress = FLEX_GC_ADR;
		/* ----- Read the filexible memory data of PLU File ----- */
		ParaFlexMemRead(&FlexData);
		ulGCNumMax = FlexData.ulRecordNumber;

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
		LoadString(hResourceDll, IDS_LOGO_TOP, szTempString, PEP_STRING_LEN_MAC(szTempString));
		DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTempString));
		SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);

		memset(&myLogoFileHeader, 0, sizeof(myLogoFileHeader));

		LoadString(hResourceDll, IDS_FILE_NAME_LOGOIDX, szLogoFileIndexName, PEP_STRING_LEN_MAC(szLogoFileIndexName));
		sFile = PifOpenFile(szLogoFileIndexName, "erw");
		
		if (sFile >= 0) {
			PifSeekFile(sFile, 0, &ulActualPosition);
			sRet = PifReadFile(sFile,&myLogoFileHeader, sizeof(myLogoFileHeader));
			PifCloseFile(sFile);
		}

		for(unI = 1; unI < 10; unI++)
		{
			LoadString(hResourceDll, IDS_LOGO_BACK, szTempString, PEP_STRING_LEN_MAC(szTempString));
			wsPepf(szDesc,szTempString, unI);
			DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
		}
		DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_SETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
		DlgItemSendTextMessage(hDlg, IDC_LOGO_FILENAME, WM_SETTEXT, (WPARAM)-1, (LPARAM)(myLogoFileHeader.aszFileName[0]));
		EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),    FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),    FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC_USE),    FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STNDRD_USE),    FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_LOGO_FILENAME),    FALSE);
		DlgItemSendTextMessage(hDlg, IDC_LOGO_FILENAME, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szCommLogoSaveName));
		if(ulGCNumMax > 999)
		{
			SendDlgItemMessage(hDlg, IDC_EDITLOGO,  EM_LIMITTEXT, 4,  0L);
		}else
		{
			SendDlgItemMessage(hDlg, IDC_EDITLOGO,  EM_LIMITTEXT, 3,  0L);
		}


		if(myLogoFileHeader.ulSpecialPercent)
		{
			SetDlgItemInt(hDlg, IDC_EDITLOGO,  myLogoFileHeader.ulSpecialPercent, FALSE);
		}else
		{
			SetDlgItemInt(hDlg, IDC_EDITLOGO,  (WORD)0, FALSE);
		}

        PepSpin.lMin       = (long)0;
		if(myLogoFileHeader.uchStatus && LOGO_STAT_PERCENTAGE)
		{
			PepSpin.lMax = 100; // for percentage amount
			SendMessage(GetDlgItem(hDlg,IDC_PERCENT_COUNT_CHK), BM_SETCHECK, BST_CHECKED, 0);
		}else
		{
			PepSpin.lMax = ulGCNumMax;
		}

        PepSpin.nStep      = 1;
        PepSpin.nTurbo     = 0;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        /* ----- set initial description, V3.3 ----- */
        return TRUE;

    case WM_VSCROLL:

		PepSpinProc(hDlg, wParam, IDC_EDITLOGO, (LPPEPSPIN)&PepSpin);

		PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(IDC_EDITLOGO), EN_CHANGE), 0L);

		return FALSE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=0; j<=11; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

		case IDC_EDITLOGO:
            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                ulPercent = GetDlgItemInt(hDlg, IDC_EDITLOGO,NULL,FALSE);
				if((ulPercent > 100) && (myLogoFileHeader.uchStatus && LOGO_STAT_PERCENTAGE))
				{
					LogoPercentageError(hDlg);
					SetDlgItemInt(hDlg, IDC_EDITLOGO,  (WORD)0, FALSE);
					return FALSE;
				}

                return TRUE;
            }
			return FALSE;

		case IDC_SCROLLBARLOGO:
			return TRUE;

		case IDC_PERCENT_COUNT_CHK:

			ulPercent = GetDlgItemInt(hDlg, IDC_EDITLOGO,NULL,FALSE);
			myLogoFileHeader.uchStatus = SendDlgItemMessage(hDlg, IDC_PERCENT_COUNT_CHK, BM_GETCHECK, 0, 0L);
			if(myLogoFileHeader.uchStatus && LOGO_STAT_PERCENTAGE)
			{
				if(ulPercent > 100)
				{
					 LogoPercentageError(hDlg);
					 SetDlgItemInt(hDlg, IDC_EDITLOGO,  (WORD)0, FALSE);
				}
			}else
			{
				if(ulPercent > 100)
				{
					LogoPercentageError(hDlg);
					SetDlgItemInt(hDlg, IDC_EDITLOGO,  (WORD)0, FALSE);
				}
				myLogoFileHeader.uchStatus &= LOGO_STAT_PERCENTAGE;
			}
			return TRUE;


		case IDOK:

			//Begin Error checking JHHJ
			if(myLogoFileHeader.uchStatus && LOGO_STAT_PERCENTAGE)
			{
				if(myLogoFileHeader.ulSpecialPercent > 100)
				{
					LogoPercentageError(hDlg);
					SetDlgItemInt(hDlg, IDC_EDITLOGO,  (WORD)0, FALSE);
					return FALSE;
				}
			}

			sFile = PifOpenFile(szLogoFileIndexName, "erw");

			myLogoFileHeader.ulSpecialPercent = GetDlgItemInt(hDlg, IDC_EDITLOGO,NULL,FALSE);
			PifSeekFile(sFile, sizeof(myLogoFileHeader),&ulActualPosition);
			PifSeekFile(sFile, 0, &ulActualPosition);
			PifWriteFile(sFile, &myLogoFileHeader, sizeof(myLogoFileHeader));
			EndDialog(hDlg,LOWORD(wParam));
			memset(&szCommLogoSaveName, 0x00, sizeof(szCommLogoSaveName));
			memset(&myLogoFileHeader,0x00,sizeof(myLogoFileHeader));
			PifCloseFile(sFile);
			return TRUE;

        case IDCANCEL:
			memset(&szCommLogoSaveName, 0x00, sizeof(szCommLogoSaveName));
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;

		case IDC_LOGO_BROWSE:
			wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
			if(LogoFile(hPepInst, hDlg, szCommLogoSaveName, PEP_MF_LOGO))
			{
				memcpy(myLogoFileHeader.aszFileName[wIndex], szCommLogoSaveName, sizeof(szCommLogoSaveName));
				DlgItemSendTextMessage(hDlg, IDC_LOGO_FILENAME, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szCommLogoSaveName));
			}
			return TRUE;


		case IDC_LOGO_STAN:
			if (SendDlgItemMessage(hDlg, IDC_LOGO_STAN, BM_GETCHECK, 0, 0L)) {
				wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
				myLogoFileHeader.uchStandardLogo = (UCHAR)wIndex + 1;
			}else{
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
				myLogoFileHeader.uchStandardLogo = 0;
			}
			return TRUE;


		case IDC_LOGO_SPEC:
			if (SendDlgItemMessage(hDlg, IDC_LOGO_SPEC, BM_GETCHECK, 0, 0L)) {
				wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_CHECKED, 0);
				myLogoFileHeader.uchSpecialLogo = (UCHAR)wIndex + 1;
			}else{
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
				myLogoFileHeader.uchSpecialLogo = 0;
			}
			return TRUE;

		case IDC_LOGO_SPEC_USE:
				wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
				if (SendDlgItemMessage(hDlg, IDC_LOGO_SPEC_USE, BM_GETCHECK, 0, 0L))
				{
					myLogoFileHeader.uchSpecialLogo = 0;
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),FALSE);
				}else
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),TRUE);
				}
				return TRUE;

		case IDC_LOGO_STNDRD_USE:
				wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
				if (SendDlgItemMessage(hDlg, IDC_LOGO_STNDRD_USE, BM_GETCHECK, 0, 0L))
				{
					myLogoFileHeader.uchStandardLogo = 0;
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),FALSE);
				}else
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),TRUE);
				}
				return TRUE;

		case IDC_LOGO_COMBO:
			wIndex = DlgItemSendTextMessage(hDlg, IDC_LOGO_COMBO, CB_GETCURSEL, (WPARAM)0, (LPARAM)(szTempString));
			/* Make sure wIndex is not set to a value larger than the size of myLogoFileHeader.aszFileName[] below */
			if(wIndex > STD_MAX_NUM_LOGO_FILES)
			{
				wIndex=1;
			}
			if(!wIndex)
			{
				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),    FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),    FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC_USE),    FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STNDRD_USE),    FALSE);
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
			}else
			{
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
				if (SendDlgItemMessage(hDlg, IDC_LOGO_STNDRD_USE, BM_GETCHECK, 0, 0L))
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),    FALSE);
				}else
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),TRUE);
				}

				if (SendDlgItemMessage(hDlg, IDC_LOGO_SPEC_USE, BM_GETCHECK, 0, 0L))
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),    FALSE);
				}else
				{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),TRUE);
				}

				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC_USE),    TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STNDRD_USE),    TRUE);

				if( (myLogoFileHeader.uchStandardLogo)
					&&(myLogoFileHeader.uchStandardLogo == (wIndex + 1)))
				{
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_CHECKED, 0);
				}else if(myLogoFileHeader.uchStandardLogo == 0){
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_STAN), BM_SETCHECK, BST_UNCHECKED, 0);
				}else{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STAN),    FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_STNDRD_USE),    FALSE);
				}

				/****** Special Logos **********/
				if((myLogoFileHeader.uchSpecialLogo)
					&& (myLogoFileHeader.uchSpecialLogo == (wIndex + 1)))
				{
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_CHECKED, 0);
				}else if(myLogoFileHeader.uchSpecialLogo == 0){
					SendMessage(GetDlgItem(hDlg,IDC_LOGO_SPEC), BM_SETCHECK, BST_UNCHECKED, 0);
				}else{
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC),    FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_LOGO_SPEC_USE),    FALSE);
				}
			}
			DlgItemSendTextMessage(hDlg, IDC_LOGO_FILENAME, WM_SETTEXT, (WPARAM)-1, (LPARAM)(myLogoFileHeader.aszFileName[wIndex]));
        }
        return FALSE;

    default:
        return FALSE;
    }
 }
/*
* ===========================================================================
**  Synopsis    :   BOOL    A068ChkDelConf()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    IDOK    -   Delete Current PLU Record.
*                           IDCANCEL-   Not Delete Current PLU Record.
*
**  Description :
*       This procedure checks current delete confirmation flag. And then it
*   displays messagebox with warning message, if confirmation flag is on.
*       It returns IDOK, user selects delete current selected PLU record.
*   Otherwise it returns IDCANCEL.
* ===========================================================================
*/
BOOL    LogoPercentageError(HWND hDlg)
{
    BOOL     fRet = IDOK;
    WCHAR    szMsg[PEP_OVER_LEN];
    WCHAR    szCap[PEP_CAPTION_LEN];


    /* ----- Load String from Resource ----- */

    LoadString(hResourceDll, IDS_LOGO_CAPTION, szCap, PEP_STRING_LEN_MAC(szCap));

    LoadString(hResourceDll, IDS_LOGO_PERCENT_ERR, szMsg, PEP_STRING_LEN_MAC(szMsg));

    /* ----- Display MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);

    fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

    return fRet;
}
/* ===== End of FILELOGO.C ===== */

