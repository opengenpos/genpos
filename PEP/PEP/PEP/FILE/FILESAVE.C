/*
* ---------------------------------------------------------------------------
* Title         :   Save File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILESAVE.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileSave():     Saves file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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
#include	<ecr.h>
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "file.h"
#include    "filel.h"

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileSave(HWND hwnd)                    *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function saves the file.               *|
|*                                                          *|
\************************************************************/
BOOL FileSave(HANDLE hInst, HWND hwnd)
{
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];
	HANDLE hFile;
    if (szFileSaveName[0] == '\0') {    /* no file name         */
        FileSaveAs(hInst, hwnd);        /* call Save As function */
        return TRUE;
    }

    /* open file */
	hFile = CreateFilePep(szFileSaveName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	{
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone */
        LoadString(hResourceDll, IDS_FILE_ERR_SAVE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        LoadFileCaption(hResourceDll, IDS_FILE_CAP_SAVE);
        MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor */

    /* ----- save the divided files to the binded file ----- */
    if (FileBind(hInst, hwnd, hFile) == TRUE) { /* success */
        CloseHandle(hFile);                     /* close file */

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor */
        PepResetModFlag(PEP_MF_ALL, 0);     /* reset change flag */

        return TRUE;
    } else {                                /* failure */
        CloseHandle(hFile);                     /* close file */

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor */

        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone */
        LoadString(hResourceDll, IDS_FILE_ERR_SAVE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONEXCLAMATION | MB_OK);

        return FALSE;
    }
}

/* ===== End of FILESAVE.C ===== */
