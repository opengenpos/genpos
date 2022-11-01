/*
* ---------------------------------------------------------------------------
* Title         :   Report File procedure
* Category      :   File, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   RPTFILE.C
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               ReportFile():           Report file user selecting file
*               ReportFileDlgProc():    dialog procedure to Report file
*                                       other file name
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Feb-05-96 : 03.01.00 : M.Suzuki   : Initial (HP US R3.1)
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
#include    "reportl.h"

WCHAR        szCommRptSaveName[PEP_FILENAME_LEN];
WCHAR        szReportCaption[REPORT_MAXLEN_TEXT];

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL ReportFile(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Report file"     *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL ReportFile(HANDLE hInst, HWND hwnd)
{
    OPENFILENAMEW    ofn;
//	OFSTRUCT		of;
    WCHAR    szReportName[REPORT_MAXLEN];
    WCHAR    szFilter[REPORT_MAXLEN];
    WCHAR    szTitle[REPORT_MAXLEN];
    WCHAR    szErrMsg[REPORT_MAXLEN_TEXT];
    WCHAR    szReportCaption[REPORT_MAXLEN_TEXT];
    WCHAR    chReplace;
	WCHAR    szExePath[REPORT_MAXLEN];
	WCHAR	szExtension[REPORT_MAXLEN];
    WCHAR	*p;
	UINT    cbString, i;
	int		loopFlag, loopCount;
	DWORD	fileError;
    HANDLE	hRptFile;

    LoadString(hResourceDll, IDS_REPORT_FILE, szTitle, PEP_STRING_LEN_MAC(szTitle));
	LoadString(hResourceDll, IDS_REPORT_EXT, szExtension, PEP_STRING_LEN_MAC(szExtension));

    if ((cbString = LoadString(hResourceDll, IDS_REPORT_FILTER, szFilter,
          PEP_STRING_LEN_MAC(szFilter))) == 0) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll, IDS_REPORT_ERR_LOAD, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        LoadString(hResourceDll, IDS_REPORT_CAP_FILE, szReportCaption, PEP_STRING_LEN_MAC(szReportCaption));
        MessageBoxPopUp(hwnd,            /* output error message */
                   szErrMsg,
                   szReportCaption,
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }
    chReplace = 0x7c;//szFilter[cbString - 1];
    for (i=0; szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace) {
            szFilter[i] = '\0';
        }
    }

	//RPH SR#102, 103
	//check to see if report file set in PEP exists if the file does not
	//exist set the path to the pep.exe directory otherwise we will 
	//set the name of the file to "set/open" to that file
	hRptFile = CreateFilePep(szCommRptSaveName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	fileError = GetLastError();

	if (fileError == ERROR_ALREADY_EXISTS)
		/*OpenFile(szCommRptSaveName, &of, OF_EXIST) == HFILE_ERROR)*/
	{
		CloseHandle(hRptFile);
		//RPH SR# 102, 103 Get the path of pep.exe to set as initial directory
#if 1
		GetPepTemporaryFolder(NULL, szExePath, PEP_STRING_LEN_MAC(szExePath));
#else
		GetPepModuleFileName(hInst, szExePath, REPORT_MAXLEN);
		szExePath[wcslen(szExePath)-8] = '\0';  //remove pep.exe from path
#endif

		//if the Report file is set we use the file name and remove the path
		//This is used to set the file name to search for in Open file dialog
		if(szCommRptSaveName[0]	 != '\0')
		{
			LoadString(hResourceDll, IDS_REPORT_ERR_PATH, szTitle, PEP_STRING_LEN_MAC(szTitle));
			loopCount = 0; //counts loop at end and subtracts the extension amount to replace

			for(loopFlag = 1, p = szCommRptSaveName + wcslen(szCommRptSaveName); loopFlag && p > szCommRptSaveName; p--)
			{
				switch(*p) {
				case '/':
				case '\\':
					loopCount--;  //adjusts the amount copied (don't include the slash)
					wcscpy(szReportName, szCommRptSaveName + wcslen(szCommRptSaveName) - loopCount);
					loopFlag = 0;
					break;			
				}
				loopCount++;
			}
		}
		else
		{
			LoadString(hResourceDll, IDS_REPORT_ERR_NAME, szTitle, PEP_STRING_LEN_MAC(szTitle));
			szReportName[0] = '\0';
		}
	}
	else
	{
		wcscpy(szReportName, szCommRptSaveName);
	}

    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
	ofn.lpstrDefExt = szExtension;
    ofn.lpstrFile = szReportName;
    ofn.nMaxFile = sizeof(szReportName);
    ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = szExePath;
    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    if (GetPepOpenFileName(&ofn)) {
        lstrcpyW(szCommRptSaveName, szReportName);
        PepSetModFlag(hwndPepMain, PEP_MF_RPT, 0);
        return TRUE;
    }
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    BOOL CurrentReportFile(HANDLE hInst, HWND hWnd)
*     Input:    hInst - instance to bind
*               hWnd  - Handle of Dialog Procedure
*
** Return:      nothing
*
** Description: This function displays the dialog box of current
*               report file name.
*===========================================================================
*/
BOOL CurrentReportFile(HANDLE hInst, HWND hWnd)
{
    DLGPROC lpitDlgProc;

    lpitDlgProc = MakeProcInstance(CurrentRptFileDlgProc,
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
*       This is a dialog procedure of Current Report File Name.
* ===========================================================================
*/
BOOL    WINAPI  CurrentRptFileDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int     iLen, i;
    WCHAR    szErrMsg[REPORT_MAXLEN_TEXT];
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

        iLen = wcslen(szCommRptSaveName);      /* get length */
        if (iLen == 0) {

            LoadString(hResourceDll, IDS_REPORT_ERR_NAME, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

            DlgItemRedrawText(hDlg, IDD_REPORT_CURRPT1, szErrMsg);

            return TRUE;
        }
        for (i = 0, wID = IDD_REPORT_CURRPT1; iLen > 0; i += 43, iLen -= 43, wID++) {

            *szWork = '\0';

            wcsncat(szWork, &szCommRptSaveName[i], 43);

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
