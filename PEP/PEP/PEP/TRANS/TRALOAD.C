#if defined(POSSIBLE_DEAD_CODE)
// This file is not used and is a candidate for removal.
/*
* ---------------------------------------------------------------------------
* Title         :   Loader Board Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRALOAD.C
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
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Oct-15-98 : 03.03.00 : O.Nakada   : Add V3.3
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
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>

#include "pep.h"
#include "pepflag.h"
#include "trans.h"
#include "transl.h"
#include "traload.h"
#include "tralcomm.h"

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
static BOOL fButtonCtrl;

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    BOOL TransLoaderExec(HANDLE hInst, HWND hWnd)
*
** Return:      nothing
*
** Description: This function displays dialog box of loader board
*               communication.
*===========================================================================
*/
BOOL TransLoaderExec(HANDLE hInst, HWND hWnd)
{
    DLGPROC lpitDlgProc;

    lpitDlgProc = MakeProcInstance(TransLoaderDlgProc, hInst);

    DialogBoxPopup(hResourceDll/*hInst*/,
              MAKEINTRESOURCE(IDD_TRANS_LOADER_BOARD),
              hWnd,
              lpitDlgProc);

    FreeProcInstance(lpitDlgProc);

    return TRUE;
}


/*
*===========================================================================
** Synopsis:    BOOL WINAPI TransLoaderDlgProc(HWND hDlg,
*                                              WORD message,
*                                              WORD wParam,
*                                              LONG lParam)
*     Input:    hDlg    - Handle of Dialog Procedure
*               message - Dispathed Message
*               wParam  - 16 Bits Message Parameter
*               lParam  - 32 Bits Message Parameter
*
** Return:      TRUE  - User Process is Executed
*               FALSE - Default Process is Exepected
*
** Description: This function is dialog box procedure of loader board
*               communication.
*===========================================================================
*/
BOOL WINAPI TransLoaderDlgProc(HWND hDlg, UINT message,
                                        WPARAM wParam, LPARAM lParam)
{
    SHORT   sCount, sRet;
    LRESULT lCursor;
    static  LOADER  Loader;
    static  UINT    fuStyle = MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL;
    char            szTitle[128];

    switch (message) {
    case WM_INITDIALOG:
        /* ----- set window caption ----- */
        LoadString(hResourceDll, IDS_TRANS_CAP_LOAD, (LPSTR)szTitle, PEP_STRING_LEN_MAC(szTitle));
        SetWindowText(hDlg, (LPCSTR)szTitle);

        /* ----- set port number ----- */
        for (sCount = 0; sCount < TRANS_MAX_PORT; sCount++) {
            SendDlgItemMessage(hDlg,
                               IDD_TRANS_LOADER_PORT,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)((LPCSTR)szTransPort[sCount]));
        }

        SendDlgItemMessage(hDlg,
                           IDD_TRANS_LOADER_PORT,
                           CB_SETCURSEL,
                           (WPARAM)(Trans.Loader.byPort - 1),
                           0L);

        /* ----- set baud rate ----- */
        for (sCount = 0; sCount < TRANS_LOADER_MAX_BAUD; sCount++) {
            SendDlgItemMessage(hDlg,
                               IDD_TRANS_LOADER_BAUD,
                               CB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)((LPCSTR)szTransBaud[sCount]));
        }
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_LOADER_BAUD,
                           CB_SETCURSEL,
                           (WPARAM)TransGetIndex(Trans.Loader.wBaudRate),
                           0L);

        memcpy(&Loader, &Trans.Loader, sizeof(LOADER));

        /* ----- set file selection ----- */
        fButtonCtrl = TransLoaderSelectFilesSet(hDlg);

        /* ----- enable/disable send/receive push button ----- */
        TransLoaderControlButton(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_PARA, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_DEPT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_PLU, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_WAI, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_CAS, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_ETK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_CPN, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_CSTR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_RPT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_PPI, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_MLD, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_SEND, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_RECEIVE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_CLOSE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_TRANS_LOADER_TRA_PARA:
        case IDD_TRANS_LOADER_TRA_DEPT:
        case IDD_TRANS_LOADER_TRA_PLU: 
        case IDD_TRANS_LOADER_TRA_WAI:
        case IDD_TRANS_LOADER_TRA_CAS:
        case IDD_TRANS_LOADER_TRA_ETK:
        case IDD_TRANS_LOADER_TRA_CPN:
        case IDD_TRANS_LOADER_TRA_CSTR:
        case IDD_TRANS_LOADER_TRA_RPT:
        case IDD_TRANS_LOADER_TRA_PPI:
        case IDD_TRANS_LOADER_TRA_MLD:
            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- get file selection ----- */
                fButtonCtrl = TransLoaderSelectFilesGet(hDlg);

                /* ----- enable/disable send/receive push button ----- */
                TransLoaderControlButton(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDD_TRANS_LOADER_SEND:
            SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */

            if ((sRet = TransLoaderSend(hDlg)) == TRANS_LOADER_OK) {
                TransResetData();

                /* ----- set file select status ----- */
                TransLoaderSelectFilesSet(hDlg);

                SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
                TransMessage(hDlg,
                             0,
                             MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL,
                             IDS_TRANS_LOADER,
                             IDS_TRANS_SUC_TRANSFER);

            } else if (sRet == TRANS_LOADER_FAIL) {
                SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
                TransMessage(hDlg,
                             0,
                             fuStyle,
                             IDS_TRANS_LOADER,
                             IDS_TRANS_LOADER_SAVEERR);
            }
            /* ----- delete programmable report file ----- */
            PifDeleteFile(szTransProgRpt);

            SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/

            TransAbortDestroyDlg();
            return TRUE;

        case IDD_TRANS_LOADER_RECEIVE:
            /* check change flag    */

            if (PepQueryModFlag(PEP_MF_PROG |
                                PEP_MF_ACT |
                                PEP_MF_DEPT |
                                PEP_MF_PLU |
                                PEP_MF_SERV |
                                PEP_MF_CASH |
                                PEP_MF_ETK |
                                PEP_MF_CPN |
                                PEP_MF_CSTR |
                                PEP_MF_RPT |
                                PEP_MF_PPI |
                                PEP_MF_MLD,
                                0) == TRUE) {
                if (TransMessage(hDlg,
                                 0,
                                 MB_OKCANCEL | MB_ICONQUESTION,
                                 IDS_TRANS_LOADER,
                                 IDS_TRANS_WAR_SAVE) == IDCANCEL) {
                    return TRUE;
                }
            }

            SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */

            if ((sRet = TransLoaderReceive(hDlg)) == TRANS_LOADER_OK) {
                TransResetData();

                SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
                TransMessage(hDlg,
                             0,
                             MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL,
                             IDS_TRANS_LOADER,
                             IDS_TRANS_SUC_TRANSFER);

            } else if (sRet == TRANS_LOADER_FAIL) {
                SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
                TransMessage(hDlg,
                             0,
                             fuStyle,
                             IDS_TRANS_LOADER,
                             IDS_TRANS_LOADER_LOADERR);
            }

            /* ----- delete programmable report file ----- */
            PifDeleteFile(szTransProgRpt);

            SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
            TransAbortDestroyDlg();
            return TRUE;

        case IDD_TRANS_LOADER_CLOSE:
            /* Get Port Number */
            lCursor = SendDlgItemMessage(hDlg,
                                         IDD_TRANS_LOADER_PORT,
                                         CB_GETCURSEL,
                                         0,
                                         0L);
            Trans.Loader.byPort = (BYTE)(lCursor + 1);

            /* Get Baud Rate */
            lCursor = SendDlgItemMessage(hDlg,
                                         IDD_TRANS_LOADER_BAUD,
                                         CB_GETCURSEL,
                                         0,
                                         0L);

            Trans.Loader.wBaudRate = (WORD)atoi(szTransBaud[(WORD)lCursor]);

            if (memcmp(&Loader, &Trans.Loader, sizeof(LOADER))) {
                PepSetModFlag(hwndPepMain, PEP_MF_TRANS, 0);
            }
            /* break */                     /* not use */

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return TRUE;

        default:
            return FALSE;
        }

    default:
        return FALSE;
    }

    lParam = lParam;
}

/*
*===========================================================================
** Synopsis:    void TransLoaderControlButton(HWND hWnd)
*     Input:    hWnd     - handle of parent window
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
void TransLoaderControlButton(HWND hWnd)
{
    BOOL    fTransCtrl;

    if (fButtonCtrl == TRUE) {
        fTransCtrl = TRUE;
    } else {
        fTransCtrl = FALSE;
    }
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_LOADER_SEND), fTransCtrl);
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_LOADER_RECEIVE), fTransCtrl);

    if (fTransCtrl == FALSE) {
        SetFocus(GetDlgItem(hWnd, IDD_TRANS_LOADER_CLOSE));
    }
}

/* ===== End of TRALOAD.C ===== */
#endif
