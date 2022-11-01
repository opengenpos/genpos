/*
* ---------------------------------------------------------------------------
* Title         :   Exit from PEP
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEEXIT.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileExit():     Exits from PEP
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
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "filel.h"

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileExit(HWND hwnd)                    *|
|*              hPepCommInst : Instance Hanlde of PEPCOMM   *|
|*              hwndPep      : handle of the main window    *|
|*                                                          *|
|* Description: This function deletes temporary files.      *|
|*                                                          *|
\************************************************************/
BOOL FileExit(HANDLE hInst, HWND hwnd)
{
    WCHAR    szErrMsg[FILE_MAXLEN];

    /* ----- Determine Whether Changes Has Been Saved or Not ----- */

    if ((PepQueryModFlag(PEP_MF_ALL, 0) == TRUE) &&
            (PepQueryModFlag(PEP_MF_OPEN, 0) == TRUE)) {
        MessageBeep(MB_ICONQUESTION);
        LoadString(hResourceDll,
                   IDS_FILE_ERR_EXIT1,
                   szErrMsg,
                   PEP_STRING_LEN_MAC(szErrMsg));
        LoadFileCaption(hResourceDll,
                   IDS_PEP_TITLE);
        if (MessageBoxPopUpFileCaption(hwnd,        /* output error message */
                       szErrMsg,
                       MB_ICONQUESTION | MB_YESNO) == IDNO) {
            return FALSE;
        }
    }

    FileTempDelete(hPepInst);           /* delete files */

    return TRUE;
}    

/* ===== End of FILEEXIT.C ===== */
