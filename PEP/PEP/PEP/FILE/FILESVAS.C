/*
* ---------------------------------------------------------------------------
* Title         :   Save As procedure
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILESVAS.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileSaveAs():           Saves file as user selecting file
*               FileSaveAsProc():       procedure to save file as
*                                       other file name
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#include	<ecr.h>
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "filel.h"

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileSaveAs(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Save As" dialog. *|
|*                                                          *|
\************************************************************/
BOOL FileSaveAs(HANDLE hInst, HWND hwnd)
{
    int     iResult;

    LoadFileCaption(hResourceDll,
               IDS_FILE_CAP_SAVEAS);

    iResult = FileSaveAsProc(hInst, hwnd);
    if (iResult == TRUE) {
        PepResetModFlag(PEP_MF_ALL, 0);     /* reset change flag    */
        return TRUE;
    } else {
        return FALSE;
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   BOOL FileSaveAsProc(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                         *|
|*              hwnd:   handle of the main window               *|
|*                                                              *|
|* Description: This function is the procedure at               *|
|*              selecting "Save As...".                         *|
\****************************************************************/
BOOL FileSaveAsProc(HANDLE hInst, HWND hwnd)
{
    static  WCHAR    szFileName[FILE_MAXLEN];
    WCHAR            szWork[FILE_MAXLEN];
    WCHAR            szFilter[FILE_MAXLEN];
    WCHAR            szFileTitle[FILE_MAXLEN];
    WCHAR            chReplace;
    UINT            cbString, i;
    OPENFILENAMEW    ofn;
	//RPH 12/16/02 SR#80 to allow mutiple file extensions
	WCHAR			*useExtension;
	WCHAR			*p;
	int				loopFlag, loopCount;
	DWORD	fileError;
	HANDLE	hFile;

    if ((cbString = LoadString(hResourceDll, IDS_FILE_FILTER, szFilter,
          PEP_STRING_LEN_MAC(szFilter))) == 0) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_LOAD, szWork, PEP_STRING_LEN_MAC(szWork));
        MessageBoxPopUpFileCaption(hwnd,            /* output error message */
                   szWork,
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }
	// Modify the filter string to make it a proper filter string by replacing
	// the special character with a binary 0 ('\0') so that the filter
	// will work properly.  Normally the special character is a |
    chReplace = 0x7c;//szFilter[cbString - 1];
    for (i=0; szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace) {
            szFilter[i] = '\0';
        }
    }

    memset(&ofn, 0, sizeof(OPENFILENAME));
    szFileName[0] = '\0';
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    if (!GetPepSaveFileName(&ofn)) {
        return FALSE;
    }

	// check to see if the filename selected has an extension.
	// check the filter index returned in ofn to determine the
	// filter that was active at the time the user did save as.
	// if no extension put filter extension on the end.
	// If the filename already has an extension, replace that extension with
	// the filter file extension.
	switch (ofn.nFilterIndex){
	case 1:
		useExtension = WIDE(".pep");
		break;
	case 2:
		useExtension = WIDE(".dat");
		break;
	case 3:
		useExtension = WIDE(".prm");
		break;
	default:
		useExtension = NULL;
		break;
	}

	if(useExtension != NULL){
		// replace extension with filter extension or add filter extension to the end of the filename
	
		loopCount = 0; //counts loop at end and subtracts the extension amount to replace

		for(loopFlag = 1, p = szFileName + wcslen(szFileName); loopFlag && p > szFileName; p--) {
			
			switch(*p) {
			case '/':
			case '\\':
			case ':':
				loopCount = 0; //append to end of string by setting to zero
				/* break left out, falls through*/
			case '.':
				//replace the title and file name extension with the filter extension
				wcscpy(szFileTitle + wcslen(szFileTitle) - loopCount, useExtension);
				wcscpy(szFileName + wcslen(szFileName) - loopCount, useExtension);
				loopFlag = 0;
				break;			
			}
			loopCount++;
		}

	}

	hFile = CreateFilePep(szFileName, GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	fileError = GetLastError();

    if (fileError == ERROR_ALREADY_EXISTS) {
                                            /* file exists */
        MessageBeep(MB_ICONQUESTION);       /* make a tone */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_OVER,
                   szWork, PEP_STRING_LEN_MAC(szWork));
        if (MessageBoxPopUpFileCaption(hwnd,    /* output message   */
                       szWork,
                       MB_ICONQUESTION | MB_YESNO) == IDNO) {
            return FALSE;
        }
    }

//	CloseHandle(hFile);

//	hFile = CreateFilePep(szFileName, GENERIC_WRITE,
//						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
//						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	fileError = GetLastError();

    if (fileError == -1) {

            MessageBeep(MB_ICONEXCLAMATION);    /* make a tone */
            LoadString(hResourceDll,
                       IDS_FILE_ERR_SAVE,
                       szWork, PEP_STRING_LEN_MAC(szWork));
            MessageBoxPopUpFileCaption(hwnd,    /* output error message */
                       szWork,
                       MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */

    if (FileBind(hPepInst, hwnd, hFile)) {  /* bind file    */

        CloseHandle(hFile);             /* close file   */

        /* save file name   */
        lstrcpyW(szFileSaveName, szFileName);

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/

        /* ----- set file title ----- */
        wsPepf(szWork, WIDE(" - %s"), szFileTitle);
        PepSetFileName(hwndPepMain, szWork);         /* set file name    */
        return TRUE;

    } else {
        CloseHandle(hFile);             /* close file   */

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/

        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_SAVE,
                   szWork, PEP_STRING_LEN_MAC(szWork));
        MessageBoxPopUpFileCaption(hwnd,    /* output error message */
                   szWork,
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;

    }
}

/* ===== End of FILESVAS.C ===== */
