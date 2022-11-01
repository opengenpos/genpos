/*
* ---------------------------------------------------------------------------
* Title         :   FILE Deletion function
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEDEL.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileDelete():        delete file
*               FileDeleteProc():    procedure to delete file
*               FileGetErrMsg():     get error message code
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
#include    "pepcomm.h"
#include    "file.h"
#include    "filel.h"
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
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    FileDelete()
*
*   Input   :   HANDLE  hInst - parent instance handle
*               HWND    hwnd  - handle of a parent window
**
*   Return  :   TRUE          - return value from a dialogbox (Normal End)
*               FALSE         - return value from a dialogbox (Otherwise)
*
**  Description:
*       This procedure creates a dialogbox of the file deletion.
* ===========================================================================
*/
BOOL    FileDelete(HANDLE hInst, HWND hwnd)
{
    BOOL    bRet;

    /* ----- make a dialogbox to delete a file ----- */

    bRet = FileDeleteProc(hInst, hwnd);

    return (bRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    FileDeleteProc()
*
*   Input   :   HANDLE  hInst - parent instance handle
*               HWND    hwnd  - handle of a parent window
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a procedure to delete a specified file.
* ===========================================================================
*/
BOOL    FileDeleteProc(HANDLE hInst, HWND hwnd)  //FIX THIS FUNCTION
{
    static  WCHAR    szFileName[FILE_MAXLEN], szWork[FILE_MAXLEN_TEXT];
    WCHAR            szTemp[FILE_MAXLEN_TEXT], szTmp[FILE_MAXLEN_TEXT];
    WCHAR            szTitle[FILE_MAXLEN];
    WCHAR            szFilter[FILE_MAXLEN];
//    OFSTRUCT        ofs;
    WCHAR            chReplace;
    UINT            cbString, i;
    OPENFILENAMEW    ofn;
	BOOL	fileError;

    /* ----- set the title ----- */

    LoadFileCaption(hResourceDll,
               IDS_FILE_CAP_DELETE);

    LoadString(hResourceDll, IDS_FILE_CAP_DELETE, szTitle, PEP_STRING_LEN_MAC(szTitle));
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
    ofn.lpstrTitle = szTitle;
    ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
    if (!GetPepOpenFileName(&ofn)) {
        return FALSE;
    }
    MessageBeep(MB_ICONQUESTION);       /* make a tone */

    /* ----- edit warning message ----- */

    LoadString(hResourceDll,
               IDS_FILE_ERR_DELETE,
               szTemp, PEP_STRING_LEN_MAC(szTemp));
    wsPepf(szWork,
             szTemp,
             szFileName);

    if (MessageBoxPopUpFileCaption(hwnd,                /* display a message */
                   szWork,
                   MB_ICONQUESTION | MB_YESNO) == IDYES) {

        /* ----- file deletion procedure ----- */

        fileError = DeleteFilePep(szFileName);
		/*OpenFile((LPCSTR)szFileName,(OFSTRUCT FAR *)&ofs,OF_DELETE);*/

        if (!fileError/*ofs.nErrCode*/) {             /* error has occurred */
            MessageBeep(MB_ICONEXCLAMATION);
            LoadString(hResourceDll,
                       IDS_FILE_ERR_DELMSG,
                       szTemp, PEP_STRING_LEN_MAC(szTemp));
            LoadString(hResourceDll,
                       FileGetErrMsg(GetLastError()),
                       szTmp, PEP_STRING_LEN_MAC(szTmp));
            wsPepf(szWork, szTemp, szTmp);
            MessageBoxPopUpFileCaption(hwnd,
                       szWork,
                       MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
        }
    }
    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   UINT    FileGetErrMsg()
*
*   Input   :   UINT    nErrCode   - error code of deletion file
*                                                   using OpenFile function
**
*   Return  :   UINT    nStrID     - the deletion error message ID
*
**  Description:
*       This procedure returns the deletion error message ID.
* ===========================================================================
*/
UINT    FileGetErrMsg(UINT nErrCode)
{
    UINT    nErrMsgID;

    if (nErrCode > FILE_DELERR_MAX) {
        nErrMsgID = IDS_FILE_DEL_ERR22;
    } else {
        nErrMsgID = (UINT)(IDS_FILE_DEL_ERR00 + nErrCode);
    }
    return (nErrMsgID);
}

/* ===== End of FILEDEL.C ===== */
