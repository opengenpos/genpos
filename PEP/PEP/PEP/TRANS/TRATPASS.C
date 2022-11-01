/*
* ---------------------------------------------------------------------------
* Title         :   Change Password for Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATPASS.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <pcstbisp.h>

#include "pep.h"
#include "pepcomm.h"
#include "trans.h"
#include "transl.h"
#include "traterm.h"

/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/
/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/
WCHAR    szPassword[2][TRANS_DIR_PASS_LEN];

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    BOOL TransTermPass(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function displays dialog box of directory.
*===========================================================================
*/
BOOL TransTermPass(HWND hWnd)
{
    int         iStatus = TRUE;
    DLGPROC     lpitDlgProc;
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    if (getTermFByStatus() & TRANS_TERM_FLAG_PASS) {   /* change password */
        lpitDlgProc = MakeProcInstance(TransTermPassDlgProc,
                                       hInst);

        if ((iStatus = DialogBoxPopup(hResourceDll/*hInst*/,
                                 MAKEINTRESOURCEW(IDD_TRANS_PASSWORD),
                                 hWnd,
                                 lpitDlgProc)) == TRUE) {
            IspChangePassWord((WCHAR *)szPassword[0]);
        }

        FreeProcInstance(lpitDlgProc);
        SetCursor(LoadCursor(NULL, IDC_WAIT));          /* set wait cursor  */
    }

    return (short)iStatus;
}

/*
*===========================================================================
** Synopsis:    BOOL WINAPI TransTermPassDlgProc(HWND hDlg,
*                                            WORD message,
*                                            WORD wParam,
*                                            LONG lParam)
*     Input:    hDlg    - Handle of Dialog Procedure
*               message - Dispathed Message
*               wParam  - 16 Bits Message Parameter
*               lParam  - 32 Bits Message Parameter
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function is dialog box procedure of change password.
*===========================================================================
*/
BOOL WINAPI TransTermPassDlgProc(HWND hDlg, UINT message,
                                     WPARAM wParam, LPARAM lParam)
{
    static BYTE byMode;
    static BYTE byRetry;
    static WORD wUpdate;
    WCHAR       szMessage[TRANS_MAX_MESSAGE_LEN],
                szBuff[16],
                szTitle[PEP_CAPTION_LEN];
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);

    switch (message) {
    case WM_INITDIALOG:
		
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        LoadString(hResourceDll, IDS_TRANS_CAP_PAS, szTitle, PEP_STRING_LEN_MAC(szTitle));
        WindowRedrawText(hDlg, szTitle);
        wUpdate = 1;
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_PASS_UPDATE,
                           BM_SETCHECK,
                           wUpdate,
                           0L);

        /* ----- clear password edit control ----- */
        *szPassword[0] = 0;
        DlgItemRedrawText(hDlg, IDD_TRANS_PASS_WORD, szPassword[0]);

        /* ----- set font ----- */
        PepSetFont(hDlg, IDD_TRANS_PASS_WORD);

        /* ----- set password length ----- */
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_PASS_WORD,
                           EM_LIMITTEXT,
                           TRANS_DIR_PASS_LEN - 1,
                           0L);

        /* ----- set focus to password ----- */
        SetFocus(GetDlgItem(hDlg, IDD_TRANS_PASS_WORD));

        /* ----- set status ----- */
        byMode  = TRANS_TERM_PASS_MODE_NEW;      /* enter new password */
        byRetry = TRANS_TERM_PASS_RETRY;         /* retry counter */
        return FALSE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_TRANS_PASS_UPDATE; j<=IDD_TRANS_PASS_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            switch (byMode) {
            case TRANS_TERM_PASS_MODE_OLD:
                /* ----- get password ----- */
                memset(szBuff, 0, sizeof(szBuff));
                DlgItemSendTextMessage(hDlg,
                                   IDD_TRANS_PASS_WORD,
                                   WM_GETTEXT,
                                   TRANS_DIR_PASS_LEN,
                                   (LPARAM)((LPSTR)szBuff));
                PepReplaceMnemonic((WCHAR *)szBuff,
                                   (WCHAR *)szPassword[0],
                                   (TRANS_DIR_PASS_LEN ) * sizeof(WCHAR),
                                   PEP_MNEMO_WRITE);

                /* ----- check old password ----- */
                if (wcscmp(TransDirList.szPassword, szPassword[0])) {   /* error */
                    if (--byRetry == 0) {
                        EndDialog(hDlg, FALSE);
                        return TRUE;
                    }

                    TransMessage(hDlg,
                                 MB_ICONASTERISK,
                                 MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL,
                                 IDS_TRANS_NAME,
                                 IDS_TRANS_ERR_PASS_INVALID);
                } else {                                            /* successful */
                    /* ----- set mnemonic ----- */
                    LoadString(hResourceDll, IDS_TRANS_ENTER, szMessage, PEP_STRING_LEN_MAC(szMessage));
                    DlgItemRedrawText(hDlg, IDD_TRANS_PASS_MNEMO, szMessage);

                    byMode  = TRANS_TERM_PASS_MODE_NEW;
                    byRetry = TRANS_TERM_PASS_RETRY;
                }

                /* ----- clear password ----- */
                *szPassword[0] = 0;
                DlgItemRedrawText(hDlg, IDD_TRANS_PASS_WORD, szPassword[0]);
                return TRUE;

            case TRANS_TERM_PASS_MODE_NEW:
                /* ----- get password ----- */
                memset(szBuff, 0, sizeof(szBuff));
                DlgItemSendTextMessage(hDlg,
                                   IDD_TRANS_PASS_WORD,
                                   WM_GETTEXT,
                                   TRANS_DIR_PASS_LEN,
                                   (LPARAM)(szBuff));
                PepReplaceMnemonic((WCHAR *)szBuff,
                                   (WCHAR *)szPassword[0],
                                   (TRANS_DIR_PASS_LEN ) * sizeof(WCHAR),
                                   PEP_MNEMO_WRITE);

               /* ----- set mnemonic ----- */
               LoadString(hResourceDll, IDS_TRANS_REENTER, szMessage, PEP_STRING_LEN_MAC(szMessage));
               DlgItemRedrawText(hDlg, IDD_TRANS_PASS_MNEMO, szMessage);

                /* ----- clear password ----- */
                *szPassword[1] = 0;
                DlgItemRedrawText(hDlg, IDD_TRANS_PASS_WORD, szPassword[1]);

                byMode = TRANS_TERM_PASS_MODE_RE;
                return TRUE;

            case TRANS_TERM_PASS_MODE_RE:
                /* ----- get password ----- */
                memset(szBuff, 0, sizeof(szBuff));
                DlgItemSendTextMessage(hDlg,
                                   IDD_TRANS_PASS_WORD,
                                   WM_GETTEXT,
                                   TRANS_DIR_PASS_LEN,
                                   (LPARAM)((LPSTR)szBuff));
                PepReplaceMnemonic((WCHAR *)szBuff,
                                   (WCHAR *)szPassword[1],
                                   (TRANS_DIR_PASS_LEN ) * sizeof(WCHAR),
                                   PEP_MNEMO_WRITE);

                /* ----- check new password ----- */
                if (wcscmp(szPassword[0], szPassword[1])) {     /* error */
                    if (--byRetry == 0) {
                        TransMessage(hDlg,
                                     MB_ICONASTERISK,
                                     MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL,
                                     IDS_TRANS_NAME,
                                     IDS_TRANS_ERR_PASS_OVER);
                        EndDialog(hDlg, FALSE);
                        return TRUE;
                    }

                    TransMessage(hDlg,
                                 MB_ICONASTERISK,
                                 MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL,
                                 IDS_TRANS_NAME,
                                 IDS_TRANS_ERR_PASS_INVALID);

                    /* ----- set mnemonic ----- */
                    LoadString(hResourceDll, IDS_TRANS_ENTER, szMessage, PEP_STRING_LEN_MAC(szMessage));
                    DlgItemRedrawText(hDlg, IDD_TRANS_PASS_MNEMO, szMessage);

                    /* ----- clear password ----- */
                    *szPassword[0] = 0;
                    DlgItemRedrawText(hDlg, IDD_TRANS_PASS_WORD, szPassword[0]);

                    byMode = TRANS_TERM_PASS_MODE_NEW;
                    return TRUE;
                } else {                                /* successful */
                    /* ----- get update password ----- */
                    if (SendDlgItemMessage(hDlg,
                                           IDD_TRANS_PASS_UPDATE,
                                           BM_GETCHECK,
                                           0,
                                           0L)) {
                        /* ----- set new password ----- */
                        wcscpy(TransDirList.szPassword, szPassword[0]);

                        TransTermDirChange(hDlg);
                    }

                    EndDialog(hDlg, TRUE);
                    return TRUE;
                }

            default:                                    /* not use */
                break;
            }
            return FALSE;

        case IDCANCEL:
            EndDialog(hDlg, FALSE);
            return TRUE;

        default:
            break;
        }

    default:
        return FALSE;
    }

    lParam = lParam;
}

/*
*===========================================================================
** Synopsis:    void TransTermPassConvert(char *pBuffer)
*     Input:    pBuffer - pointer of buffer
*
** Return:      nothing
*
** Description: This function convert form ascii code to hiden code for
*               password character.
*===========================================================================
*/
void TransTermPassConvert(WCHAR *pBuffer)
{
    while (*pBuffer) {
        *pBuffer ^= TRANS_TERM_PASS_MASK;
        ++pBuffer;
    }
    *pBuffer = 0;
}

/* ===== End of TRAPASS.C ===== */
