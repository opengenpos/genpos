/*
* ---------------------------------------------------------------------------
* Title         :   Layout File procedure
* Category      :   Layout, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   LAYOUTFILE.C
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               LayoutFile():           Layout file user selecting file
*               LayoutFileDlgProc():    dialog procedure to Layout file
*                                       other file name
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-25-03 : 02.00.00 : J.Hall     : Initial (2.0.0)
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
#include    <commdlg.h>

#include <defs32bitport.h>

#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "report.h"
#include    "layout.h"
#include	"layoutl.h"
#include	"trans.h"
#include	"../trans/transl.h"

WCHAR        szCommTouchLayoutSaveName[PEP_FILENAME_LEN]; //ESMITH LAYOUT
WCHAR        szCommKeyboardLayoutSaveName[PEP_FILENAME_LEN];
WCHAR		 szFileLayoutDir[PEP_FILENAME_LEN];
WCHAR        szLayoutCaption[LAYOUT_MAXLEN_TEXT];

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL LayoutFile(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Layout file"     *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL LayoutFile(HANDLE hInst, HWND hwnd,WCHAR* szLayout, UINT nFlag) //ESMITH LAYOUT
{
    OPENFILENAMEW    ofn;
    WCHAR    szReportName[LAYOUT_MAXLEN];
    WCHAR    szFilter[LAYOUT_MAXLEN];
    WCHAR    szTitle[LAYOUT_MAXLEN];
    WCHAR    szErrMsg[LAYOUT_MAXLEN_TEXT];
    WCHAR    szReportCaption[LAYOUT_MAXLEN_TEXT];
    WCHAR    chReplace;
	WCHAR    szExePath[LAYOUT_MAXLEN];
	WCHAR	 szExtension[] = WIDE("dat");
    WCHAR	 *p;
	UINT     cbString, i;
	int		 loopFlag, loopCount;
	DWORD	 fileError;
    HANDLE	 hLayoutFile;

    LoadString(hResourceDll, IDS_LAYOUT_FILE, szTitle, PEP_STRING_LEN_MAC(szTitle));
    LoadString(hResourceDll, IDS_LAYOUT_EXT, szExtension, PEP_STRING_LEN_MAC(szExtension));

    if ((cbString = LoadString(hResourceDll, IDS_LAYOUT_FILTER, szFilter,
          PEP_STRING_LEN_MAC(szFilter))) == 0) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone*/
        LoadString(hResourceDll,
                   IDS_LAYOUT_ERR_LOAD, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        LoadString(hResourceDll,
                   IDS_LAYOUT_CAP_FILE,
                   szReportCaption, PEP_STRING_LEN_MAC(szReportCaption));
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
	//check to see if Layout file set in PEP exists if the file does not
	//exist set the path to the pep.exe directory otherwise we will
	//set the name of the file to "set/open" to that file
	hLayoutFile = CreateFilePep(szLayout, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	fileError = GetLastError();

	if (fileError != ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hLayoutFile);

#if 1
		GetPepTemporaryFolder(hInst, szExePath, LAYOUT_MAXLEN);
#else
		GetPepModuleFileName(hInst, szExePath, LAYOUT_MAXLEN);
		szExePath[wcslen(szExePath)-8] = '\0';  //remove pep.exe from path
#endif

		//if the Layout file is set we use the file name and remove the path
		//This is used to set the file name to search for in Open file dialog
		if(szLayout[0]	 != '\0')
		{
			LoadString(hResourceDll, IDS_LAYOUT_ERR_PATH, szTitle, PEP_STRING_LEN_MAC(szTitle));
			loopCount = 0; //counts loop at end and subtracts the extension amount to replace

			for(loopFlag = 1, p = szLayout + wcslen(szLayout); loopFlag && p > szLayout; p--)
			{
				switch(*p) {
				case '/':
				case '\\':
					loopCount--;  //adjusts the amount copied (don't include the slash)
					wcscpy(szReportName, szLayout + wcslen(szLayout) - loopCount);
					loopFlag = 0;
					break;
				}
				loopCount++;
			}
		}
/*			if((szCommTouchLayoutSaveName[0] == '\0') && (szCommKeyboardLayoutSaveName[0] == '\0'))
			{
				LoadString(hResourceDll, IDS_LAYOUT_ERR_NAME, szTitle, sizeof(szTitle));
				szReportName[0] = '\0';
			}
*/
			//if((szCommTouchLayoutSaveName[0] == '\0') && ( nFlag == 0x00008000))    // Find Window Handle for Check Box
			if(nFlag == 0x00008000)
			{
				LoadString(hResourceDll, IDS_LAYOUT_TOUCH_ERR, szTitle, PEP_STRING_LEN_MAC(szTitle));
				szReportName[0] = '\0';
			}
			else //if(szCommKeyboardLayoutSaveName[0] == '\0')
			{
				LoadString(hResourceDll, IDS_LAYOUT_KEYBOARD_ERR, szTitle, PEP_STRING_LEN_MAC(szTitle));
				szReportName[0] = '\0';
			}

	}
	else
	{
		wcscpy(szReportName, szLayout);
		CloseHandle(hLayoutFile);
	}
	if(szFileLayoutDir == '\0'){
		lstrcpyW(szFileLayoutDir, szExePath);
	}
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
	ofn.lpstrDefExt = szExtension;
    ofn.lpstrFile = szReportName;
    ofn.nMaxFile = sizeof(szReportName)/sizeof(WCHAR);
	//ofn.nMaxFile = sizeof(szReportName);
    ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = szFileLayoutDir;
    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    if (GetPepOpenFileName(&ofn)) {
        lstrcpyW(szLayout, szReportName);
        lstrcpyW(szFileLayoutDir, szReportName);
        PepSetModFlag(hwndPepMain, nFlag, 0);
        return TRUE;
    }
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    BOOL CurrentLayoutFile(HANDLE hInst, HWND hWnd)
*     Input:    hInst - instance to bind
*               hWnd  - Handle of Dialog Procedure
*
** Return:      nothing
*
** Description: This function displays the dialog box of current
*               Layout file name.
*===========================================================================
*/
BOOL CurrentLayoutFile(HANDLE hInst, HWND hWnd)
{
    DLGPROC lpitDlgProc;

    lpitDlgProc = MakeProcInstance(CurrentLayoutFileDlgProc,
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
*       This is a dialog procedure of Current Layout File Name.
* ===========================================================================
*/
BOOL    WINAPI  CurrentLayoutFileDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int     iLen=0, i;
    WCHAR    szErrMsg[LAYOUT_MAXLEN_TEXT];
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
			one layout to multiple layouts: removed to
			clear error.
		*/
		//iLen = wcslen(szCommLayoutSaveName);      /* get length */
        if (iLen == 0) {

            LoadString(hResourceDll,
                       IDS_REPORT_ERR_NAME, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

            DlgItemRedrawText(hDlg,
                           IDD_REPORT_CURRPT1,
                           szErrMsg);

            return TRUE;
        }
        for (i = 0, wID = IDD_REPORT_CURRPT1; iLen > 0; i += 43, iLen -= 43, wID++) {

            *szWork = '\0';

           // wcsncat(szWork, &szCommLayoutSaveName[i], 43);

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
/* ===== End of RPTFILE.C ===== */
