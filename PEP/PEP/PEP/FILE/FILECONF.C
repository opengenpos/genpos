/*
* ---------------------------------------------------------------------------
* Title         :   Configuration for File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILECONF.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileConfig():           Sets configuration
*               FileConfigDlgProc():    dialog procedure to set configuration
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Sep-15-95 : 01.00.03 : T.Nakahata : add FILE_FAT_ADDR
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
#include	<ecr.h>
#include    "paraequ.h"
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "filel.h"

/************************************\
|*  Global Work Aria Declarations   *|
\************************************/
FCONFIG     Fconfig;

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileConfig(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Configuration"   *|
|*              dialog.                                     *|
|*                                                          *|
\************************************************************/
BOOL FileConfig(HANDLE hInst, HWND hwnd)
{
    int     iResult;
    DLGPROC InstDlgProc;
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];

    /* create dialog    */
    InstDlgProc = MakeProcInstance(FileConfigDlgProc, hInst);

    if ((iResult = DialogBoxPopup(hResourceDll,
                             MAKEINTRESOURCEW(IDD_FILE_DLG_CONFIG),
                             hwnd,
                             InstDlgProc)) == -1) {

        MessageBeep(MB_ICONEXCLAMATION);
        LoadString(hResourceDll, IDS_FILE_ERR_CREATE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        LoadFileCaption(hResourceDll, IDS_FILE_CAP_CONF);

        /* ----- output message box ----- */

        MessageBoxPopUpFileCaption(hwnd,
                   szErrMsg,
                   MB_ICONEXCLAMATION | MB_OK);

        FreeProcInstance(InstDlgProc);
        return FALSE;
    }

    FreeProcInstance(InstDlgProc);
    if (iResult == TRUE) {
        PepSetModFlag(hwnd, PEP_MF_CONF, 0);      /* set change flag  */
        return TRUE;
    } else {
        return TRUE;
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   BOOL WINAPI FileConfigDlgProc(HWND hDlg,        *|
|*                                                WORD message, *|
|*                                                WORD wParam,  *|
|*                                                LONG lParam)  *|
|*              hDlg:       handle of the dialog                *|
|*              message:    window message                      *|
|*              wParam:     message parameter                   *|
|*              lParam:     message parameter                   *|
|*                                                              *|
|* Description: This function is the dialog procedure at        *|
|*              selecting "Configuration...".                   *|
|*                                                              *|
\****************************************************************/
BOOL WINAPI FileConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WCHAR    i;
    WORD    wId;
    FCONFIG Fbackup;
    UINT    nID;
    WCHAR    szMsg[FILE_MAXLEN_TEXT];

    switch(message) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
		
		/* set terminal */
        for (i = 0, nID = IDS_FILE_TERM_MASTER; i < FILE_NUM_TERM; i++, nID++) {
            LoadString(hResourceDll, nID, szMsg, PEP_STRING_LEN_MAC(szMsg));
            DlgItemSendTextMessage(hDlg,
                               IDD_FILE_TERM,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)szMsg);
        }

        wId = (WORD)Fconfig.chTerminal;
        SendDlgItemMessage(hDlg, IDD_FILE_TERM, CB_SETCURSEL, wId, 0L);

        /* set ROM version  */
        for (i = 0, nID = IDS_FILE_VER_100; i < FILE_NUM_VER; i++, nID++) {
            LoadString(hResourceDll, nID, szMsg, PEP_STRING_LEN_MAC(szMsg));
            DlgItemSendTextMessage(hDlg,
                               IDD_FILE_VER,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)szMsg);
        }

        wId = (WORD)Fconfig.chVersion;
        SendDlgItemMessage(hDlg, IDD_FILE_VER, CB_SETCURSEL, wId, 0L);

        /*
			set KB type.  We are removing the NCR 7448 types of keyboards since
			the NCR 7448 hardware is manufacture discontinued by NCR.
			However the possible keyboard values that represent those keyboards
			will not be re-used for backwards compatibility and for testing if
			configuration contains old keyboard types.
		 */
        for (i = FILE_KB_WEDGE_68, nID = IDS_FILE_KEY_WEDGE_68; i < FILE_NUM_KB; i++, nID++) {
            LoadString(hResourceDll, nID, szMsg, PEP_STRING_LEN_MAC(szMsg));
            DlgItemSendTextMessage(hDlg,
                               IDD_FILE_KB,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)szMsg);
        }

        wId = (WORD)Fconfig.chKBType;
		if (wId < FILE_KB_WEDGE_68) {
			wId = FILE_KB_WEDGE_68;
			Fconfig.chKBType = FILE_KB_WEDGE_68;
		}
		wId -= FILE_KB_WEDGE_68;
		SendDlgItemMessage(hDlg, IDD_FILE_KB, CB_SETCURSEL, wId, 0L);

        SetFocus(GetDlgItem(hDlg, IDD_FILE_TERM));
        LoadFileCaption(hResourceDll, IDS_FILE_CAP_CONF);

        /* --- set no. of terminals --- */

        for (i = 1; i <= MAX_TERMINAL_NO; i++) {
            wsPepf(szMsg, WIDE("%d"), i );
            DlgItemSendTextMessage(hDlg,
                               IDD_FILE_NOOFTERM,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)szMsg);
        }
        wId = Fconfig.chNoOfSatellite;
        SendDlgItemMessage(hDlg, IDD_FILE_NOOFTERM, CB_SETCURSEL, wId, 0L);

        return TRUE;
    
	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_FILE; j<=IDD_FILE_CAPTION7; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDOK:
            /* back up configuration    */
            memcpy(&Fbackup, &Fconfig, sizeof(FCONFIG));

            /* get terminal     */
            Fconfig.chTerminal = (char)SendDlgItemMessage(hDlg,
                                                          IDD_FILE_TERM,
                                                          CB_GETCURSEL,
                                                          0,
                                                          0L);

            /* get ROM version  */
            Fconfig.chVersion = (char)SendDlgItemMessage(hDlg,
                                                         IDD_FILE_VER,
                                                         CB_GETCURSEL,
                                                         0,
                                                         0L);

            /*
				get KB type.  We have removed the NCR 7448 selections but
				we must keep the possible values for those keyboards for
				backwards compatibility detection.
			 */
            Fconfig.chKBType = (char)SendDlgItemMessage(hDlg,
                                                        IDD_FILE_KB,
                                                        CB_GETCURSEL,
                                                        0,
                                                        0L) + FILE_KB_WEDGE_68;

            Fconfig.usMemory = FILE_SIZE_64KB * FILE_SIZE_48MBTOKB;    // set default Flash memory 48MB

			if (Fconfig.chTerminal) {   /* satellite terminal   */
                Fconfig.ulStartAddr = (unsigned long)FILE_SADDR;
            } else {                    /* master terminal      */
                Fconfig.ulStartAddr = (unsigned long)FILE_MADDR;
            }
            Fconfig.ulStartAddr += (unsigned long)FILE_FAT_ADDR;

            /* --- get no. of terminals --- */

            Fconfig.chNoOfSatellite =
                (char)SendDlgItemMessage(hDlg,
                                         IDD_FILE_NOOFTERM,
                                         CB_GETCURSEL,
                                         0,
                                         0L);

            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
    }
    return FALSE;

    lParam = lParam;
}

/* ===== End of FILECONF.C ===== */
