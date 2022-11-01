#if defined(POSSIBLE_DEAD_CODE)
/*
* ---------------------------------------------------------------------------
* Title         :   FILE internal function
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEINTR.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileChangeDir():       Change directory
*               FileRefreshListbox():  Refresh listbox
*               FileRefreshDrive():    Refresh drive
*               FileCheckName():       Check file name 
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
#include    <file.h>
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
#define WIDE(s) L##s
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
static WCHAR   szFileWild[] = WIDE("*.*");

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
**  Synopsis:   void    FileRefreshDrive()
*
*   Input   :   HWND    hdlg     - handle of a dialogbox procedure
*               LPCSTR  lpszName - selected file name
**
*   Return  :   None.
*
**  Description:
*       This procedure refreshes a combobox of drive list.
* ===========================================================================
*/

void    FileRefreshDrive(HWND hdlg, LPSTR lpszName)
{
    WORD    wCo, wIdx;
    WCHAR    chDrv, szTmp[128];

    DlgDirListComboBox(hdlg, lpszName, IDD_FILE_DRVLIST, 0, DDL_DRIVES);
    wCo = (WORD)SendDlgItemMessage(hdlg,
                                   IDD_FILE_DRVLIST,
                                   CB_GETCOUNT, 0, 0L);
    DlgItemGetText(hdlg, IDD_FILE_CURDIR, szTmp, sizeof(szTmp));
    chDrv = szTmp[0];
    for (wIdx = 0; wCo > 0; wIdx++, wCo--) {
        SendDlgItemMessage(hdlg,
                           IDD_FILE_DRVLIST,
                           CB_GETLBTEXT,
                           (WPARAM)wIdx,
                           (LPARAM)(LPCSTR)szTmp);
        if (chDrv == szTmp[FILE_DRVNAME_OFS]) {
            SendDlgItemMessage(hdlg,
                                IDD_FILE_DRVLIST, CB_SETCURSEL, wIdx, 0L);
            break;
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   void    FileRefreshListbox()
*
*   Input   :   HWND    hdlg      - handle of a dialogbox procedure
*               BOOL    bTyle     - type of status
*               LPCSTR  lpszFname - selected file name
**
*   Return  :   None.
*
**  Description:
*       This procedure refreshes listboxes in the dialogbox.
* ===========================================================================
*/
void    FileRefreshListbox(HWND hdlg, BOOL  bType, LPCSTR lpszFname)
{
    WCHAR    szName[256];

    if (bType == FILE_INIT_PROC) {              /* call from WM_INITDIALOG */
        lstrcpy((LPSTR)szName, (LPCSTR)szFileWild); /* set wild-card */
    } else {
        lstrcpy((LPSTR)szName, lpszFname);      /* set file name */
    }

    /* ----- fill a file-listbox with files ----- */

    DlgDirList(hdlg, (LPSTR)szName, IDD_FILE_FILELIST, 0, DDL_READWRITE);

    /* ----- fill a directory-listbox with DIR's and set DIR-Name ----- */

    DlgDirList(hdlg,
               (LPSTR)szName,
               IDD_FILE_DIRLIST,
               IDD_FILE_CURDIR,
               DDL_DIRECTORY | DDL_EXCLUSIVE);

    /* ----- reset listboxes ----- */

    FileRefreshDrive(hdlg, (LPSTR)szName);

    SetFocus(GetDlgItem(hdlg, IDD_FILE_NAME));
}

/*
* ===========================================================================
**  Synopsis:   BOOL    FileChangeDir()
*
*   Input   :   HWND    hdlg         - handle of a dialogbox procedure
*               LPCSTR  lpszFileName - selected file name
*               int     nLen         - length of file
*               BOOL    bTyle        - type of status
**
*   Return  :   TRUE    - invalid file name is entered (wild-card is found)
*               FALSE   - valid file name is entered (wild-card is not found)
*
**  Description:
*       This procedure checkes an entered file name.
* ===========================================================================
*/
BOOL    FileChangeDir(HWND hdlg, LPWSTR lpszFileName, int nLen, BOOL bType)
{
    LPWSTR   p;

    DlgItemGetText(hdlg, IDD_FILE_NAME, lpszFileName, nLen);
    p = lpszFileName;
    while (*p) {
        if (*(p++) == '*') {
            FileRefreshListbox(hdlg, bType, (LPCSTR)lpszFileName);
            return TRUE;
        }
    }
    return FALSE;
}
/*
* ===========================================================================
**  Synopsis:   BOOL    FileCheckName()
*
*   Input   :   LPCSTR  lpszName - selected file name
**
*   Return  :   TRUE             - wild-card (* or ?) is not found
*               FALSE            - wild-card is found
*
**  Description:
*       This procedure check file name.
* ===========================================================================
*/

BOOL    FileCheckName(LPWSTR lpszName)
{
    LPCWSTR  p;
    int     n, nLen;
    WCHAR    szWork[FILE_MAXLEN];

    nLen = lstrlenW(lpszName);
    for (n = 0, p = lpszName; n < nLen; n++, p++) {
        if (*p == '*' || *p == '?') {
            break;
        }
    }
    if (n < nLen) {
        return FALSE;           /* wild-card is found */
    } else {
        p = lpszName;
        while (*p) {
            p ++;
        }
        for (n = 0; n < nLen; n++, p--) {
            if (*p == '.') {
                break;
            }
        }
        szWork[0] = '*';
        lstrcpyW(&szWork[1], p);
        lstrcpyW(lpszName, szWork);      /* reform a buffer for wild-card  */
        return TRUE;            /* wild-card is not found (add wild-card) */
    }
}

/* ===== End of FILEINTR.C ===== */
#endif