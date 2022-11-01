/*
* ---------------------------------------------------------------------------
* Title         :   Layout Manager File procedure
* Category      :   Layout, PEP 2.0 for Windows (HP US Model)
* Program Name  :   LAYOUTMNGREXEC.C
* Copyright (C) :   2003, Georgia Southern University, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               LayoutMNGRExec():           Report Execute user selecting file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-24-03 : 02.00.00 : J. Hall    : Initial (2.0)
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

#include    "ecr.h"
#include    "paraequ.h"
#include    "para.h"
#include	"report.h"
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "layoutl.h"
#include	"layout.h"

static WCHAR szExec[] = WIDE("\\NHPOSLM.EXE");

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL LayoutExec(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Layout Manager"  *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL LayoutExec(HANDLE hInst, HWND hwnd, WCHAR* szLayout, LANGID xlan)
{
    int     iLen;
    WCHAR    szBuffer[LAYOUT_MAXLEN];
    WCHAR    szDir[LAYOUT_MAXLEN];
    WCHAR    szExecFile[LAYOUT_MAXLEN];
    WCHAR    szErrMsg[LAYOUT_MAXLEN_TEXT];
	WCHAR    szLayoutCaption[LAYOUT_MAXLEN_TEXT];
	WCHAR	 szLayoutFileName[PEP_FILENAME_LEN];
	WORD     lgID;
	WORD     subLgID;
	WCHAR    szCmdLineLang[5];
	INT	hresult;
	HANDLE hHandle;
	DWORD lastError;



//    PARAFLEXMEM FlexData;
//    OFSTRUCT        ofs;

	/*get primary and sublanguage id's and convert to WCHARS for command line parsing*/
	lgID = PRIMARYLANGID(xlan);
	subLgID = SUBLANGID(xlan);
	wsPepf(szCmdLineLang,WIDE("0%x0%x"),lgID,subLgID);

	// find the location where the Layout Manager utility is located.
	// we use this path to build a shell execute command line.
	GetPepModuleFileName(hInst, szBuffer, LAYOUT_MAXLEN);
	iLen = wcslen(szBuffer);
	if (!((iLen > 8) && (szBuffer[iLen - 8] == '\\'))) {   /* must end in \PEP.EXE */
        LoadString(hResourceDll,
                   IDS_REPORT_ERR_PEP, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUp(hwnd,            /* output error message */
                   szErrMsg,
                   szLayoutCaption,
                   MB_ICONEXCLAMATION | MB_OK);
        return TRUE;
    }
	iLen -= 8;
	szBuffer[iLen] = '\0';

	lstrcpyW(szDir, szBuffer);
	lstrcpyW(szExecFile, szBuffer);
//	lstrcatW(szBuffer, szPep);
	lstrcatW(szExecFile, szExec);

	/*This is necessary for files that are on the desktop, or a location with spaces in it
	(i.e. C:\Documents and Settings\user\Desktop).  The ShellExecute command can cause errors when
	spaces are found, JHHJ*/

	//load file name of layout file to open
	memset(szLayoutFileName, 0x00, sizeof(szLayoutFileName));
	szLayoutFileName[0] = _T('\"');
	wcscat(szLayoutFileName, szLayout);
	iLen = wcslen(szLayoutFileName);
	szLayoutFileName[iLen] = _T('\"');

	//set language parameters so Layout Mngr will open with the same language PEP is using
	wcscat(szLayoutFileName, WIDE(" /lg:"));
	wcscat(szLayoutFileName, szCmdLineLang);
	iLen = wcslen(szLayoutFileName);
	szLayoutFileName[iLen + 1] = _T('\0');

	// create handle to layout file being opened - CSMALL 10/18/05
	hHandle = CreateFilePep(szLayout,
						GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	// get error from creating handle
	lastError = GetLastError();

	// if layout file cannot be found, prompt user to browse for file - CSMALL 10/18/05
	if (lastError != ERROR_SUCCESS) {
		LayoutFile(hPepInst, hwnd, szCommTouchLayoutSaveName,PEP_MF_LAYTOUCH);

		// after new layout chosen, close handle
		CloseHandle(hHandle);
		return TRUE;
	}
	else
		hresult = ShellExecutePep(hwnd, NULL, szExecFile, szLayoutFileName,
                 szDir, SW_SHOWNORMAL);
	CloseHandle(hHandle);
		
	if ( hresult < 32) {
        LoadString(hResourceDll,
                   IDS_LAYOUT_ERR_EXEC, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUp(hwnd,            /* output error message */ 
                   szErrMsg,
                   szLayoutCaption,
                   MB_ICONEXCLAMATION | MB_OK);
    }

    return TRUE;
}


/* ===== End of RPTEXEC.C ===== */
