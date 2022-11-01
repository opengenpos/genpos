/*
* ---------------------------------------------------------------------------
* Title         :   Directory for Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATDIR.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     As for this file, a necessary function for the dialog box 
*               of the directory is defined.
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
#include <r20_pif.h>

#include "pep.h"
#include "pepflag.h"
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
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    BOOL TransTermDir(HWND hWnd)
*     Input:    hWnd - handle of parent of window
*
** Return:      nothing
*
** Description: This function displays dialog box of directory.
*===========================================================================
*/
BOOL TransTermDir(HWND hWnd)
{
    int         nStatus;
    DLGPROC     lpitDlgProc;
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    lpitDlgProc = MakeProcInstance(TransTermDirDlgProc,
                                   hInst);

    nStatus = DialogBoxPopup(hResourceDll/*hInst*/,
                        MAKEINTRESOURCEW(IDD_TRANS_DIRECTORY),
                        hWnd,
                        lpitDlgProc);

    FreeProcInstance(lpitDlgProc);

    return (BOOL)nStatus;
}

/*
*===========================================================================
** Synopsis:    BOOL WINAPI TransTermDirDlgProc(HWND hDlg,
*                                               WORD message,
*                                               WORD wParam,
*                                               LONG lParam)
*     Input:    hDlg    - Handle of Dialog Procedure
*               message - Dispathed Message
*               wParam  - 16 Bits Message Parameter
*               lParam  - 32 Bits Message Parameter
*
** Return:      TRUE  - User Process is Executed
*               FALSE - Default Process is Exepected
*
** Description: This function is dialog box procedure of directory.
*===========================================================================
*/
BOOL WINAPI TransTermDirDlgProc(HWND hDlg, UINT message,
                                      WPARAM wParam, LPARAM lParam)
{
    WCHAR    szBuff[16],
            szTitle[PEP_CAPTION_LEN];

    switch (message) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        LoadString(hResourceDll, IDS_TRANS_CAP_DIR, szTitle, PEP_STRING_LEN_MAC(szTitle));
        WindowRedrawText(hDlg, szTitle);
        /* ----- check directory function ----- */
        if (wTransDirCmd == TRUE) {         /* add directory */
            TransTermDirInit();           /* initialize data */
        } else {                            /* change directory */
            TransTermDirRead(hDlg);       /* read old data */
        }

        /* ----- set name ----- */
        PepSetFont(hDlg, IDD_TRANS_DIR_NAME);
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_NAME,
                           EM_LIMITTEXT,
                           TRANS_DIR_NAME_LEN - 1,
                           0L);
        DlgItemSendTextMessage(hDlg,
                           IDD_TRANS_DIR_NAME,
                           WM_SETTEXT,
                           0,
                           (LPARAM)(TransDirList.szName));

        /* ----- set phone number ----- */
        PepSetFont(hDlg, IDD_TRANS_DIR_NUMBER);
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_NUMBER,
                           EM_LIMITTEXT,
                           TRANS_DIR_NUMBER_LEN - 1,
                           0L);
        DlgItemSendTextMessage(hDlg,
                           IDD_TRANS_DIR_NUMBER,
                           WM_SETTEXT,
                           0,
                           (LPARAM)(TransDirList.szNumber));

        /* ----- set password ----- */
        PepSetFont(hDlg, IDD_TRANS_DIR_PASS);
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_PASS,
                           EM_LIMITTEXT,
                           TRANS_DIR_PASS_LEN - 1,
                           0L);
        memset(szBuff, 0, sizeof(szBuff));
        PepReplaceMnemonic((WCHAR *)TransDirList.szPassword,
                           (WCHAR *)szBuff,
                           (TRANS_DIR_PASS_LEN - 1),
                           PEP_MNEMO_READ);
        DlgItemSendTextMessage(hDlg,
                           IDD_TRANS_DIR_PASS,
                           WM_SETTEXT,
                           0,
                           (LPARAM)(szBuff));

        /* ----- set Initiale strings ----- */
        PepSetFont(hDlg, IDD_TRANS_DIR_INIT);
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_INIT,
                           EM_LIMITTEXT,
                           TRANS_DIR_INIT_LEN - 1,
                           0L);
        DlgItemSendTextMessage(hDlg,
                           IDD_TRANS_DIR_INIT,
                           WM_SETTEXT,
                           0,
                           (LPARAM)(TransDirList.szInit));

        /* ----- set dial type ----- */
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_TONE +
                           ((TransDirList.fbyStatus & TRANS_TERM_DIR_TYPE_DIAL) ? (1) : (0)),
                           BM_SETCHECK,
                           (WPARAM)1,
                           0L);

        /* ----- set focus to name ----- */
        SendDlgItemMessage(hDlg,
                           IDD_TRANS_DIR_NAME,
                           EM_SETSEL,
                           0,
                           MAKELONG(0, -1));
        SetFocus(GetDlgItem(hDlg, IDD_TRANS_DIR_NAME));
        return FALSE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_TRANS_DIR_NAME; j<=IDD_TRANS_DIR_GROUPBOX1; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_TRANS_DIR_NAME:
            if (HIWORD(wParam) == EN_CHANGE) {
                EnableWindow(GetDlgItem(hDlg, IDOK),
                             (BOOL)SendMessage((HWND)lParam,
                                               WM_GETTEXTLENGTH,
                                               0,
                                               0L));
                return TRUE;
            }
            return FALSE;

        case IDOK:
            /* ----- get name ----- */
            DlgItemSendTextMessage(hDlg,
                               IDD_TRANS_DIR_NAME,
                               WM_GETTEXT,
                               TRANS_DIR_NAME_LEN,
                               (LPARAM)(TransDirList.szName));

            /* ----- get phone number ----- */
            DlgItemSendTextMessage(hDlg,
                               IDD_TRANS_DIR_NUMBER,
                               WM_GETTEXT,
                               TRANS_DIR_NUMBER_LEN,
                               (LPARAM)(TransDirList.szNumber));

            /* ----- get password ----- */
            memset(szBuff, 0, sizeof(szBuff));
            DlgItemSendTextMessage(hDlg,
                               IDD_TRANS_DIR_PASS,
                               WM_GETTEXT,
                               TRANS_DIR_PASS_LEN,
                               (LPARAM)(szBuff));
            PepReplaceMnemonic((WCHAR *)szBuff,
                               (WCHAR *)TransDirList.szPassword,
                               (TRANS_DIR_PASS_LEN ) * sizeof(WCHAR),
                               PEP_MNEMO_WRITE);

            /* ----- get modem initial strings ----- */
            DlgItemSendTextMessage(hDlg,
                               IDD_TRANS_DIR_INIT,
                               WM_GETTEXT,
                               TRANS_DIR_INIT_LEN,
                               (LPARAM)(TransDirList.szInit));

            /* ----- get dial type ----- */
            if (DlgItemSendTextMessage(hDlg,
                                   IDD_TRANS_DIR_TONE,
                                   BM_GETCHECK,
                                   0,
                                   0L)) {
                TransDirList.fbyStatus &= ~TRANS_TERM_DIR_TYPE_DIAL;
            } else {                        /* pulse */
                TransDirList.fbyStatus |= TRANS_TERM_DIR_TYPE_DIAL;
            }

            /* ----- update directory ----- */
            if (wTransDirCmd == TRUE) {     /* add directory */
                TransTermDirAdd(hDlg);
            } else {                        /* change directory */
                TransTermDirChange(hDlg);
            }

            /* ----- set file modifiy flag ----- */
            PepSetModFlag(hwndPepMain, PEP_MF_TRANS, 0);

            EndDialog(hDlg, TRUE);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, FALSE);
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
** Synopsis:    void TransTermDirInit(void)
*
** Return:      nothing
*
** Description: This function initializes the data in the directory.
*===========================================================================
*/
void TransTermDirInit(void)
{
    memset(&TransDirList, '\0', sizeof(DIRECTORY)); /* clear memory */
    TransDirList.wBaudRate   = 2400;                /* modem baud rate */
    TransDirList.byMnp       = 0;                   /* MNP - none */
    TransDirList.fbyStatus  &= ~TRANS_TERM_DIR_TYPE_DIAL;     /* dial type */
}

/*
*===========================================================================
** Synopsis:    void TransTermDirRead(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      nothing
*
** Description: This function reads one data in the directory.
*===========================================================================
*/
void TransTermDirRead(HWND hWnd)
{
    SHORT       sFile;                      /* handle of file */
    ULONG       ulActual;                   /* seek length */
    DIRECTORY   DirBuf;                     /* directory buffer */

    if ((sFile = PifOpenFile(szTransFile, "or")) == PIF_ERROR_FILE_NOT_FOUND) {
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOCAL_FILE_OPEN,
                     szTransFile);
        return;
    }

    PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * iTransDirIndex, &ulActual);
    PifReadFile((USHORT)sFile, &DirBuf, sizeof(DIRECTORY));
    PifCloseFile((USHORT)sFile);

    TransDirList = DirBuf;

    /* ----- convert password ----- */
    TransTermPassConvert(TransDirList.szPassword);
}

/*
*===========================================================================
** Synopsis:    void TransTermDirAdd(HWND hWnd)
*     Input:    hWnd - handle of parent of window
*
** Return:      nothing
*
** Description: This function is added to the posistion where the data in
*               the directory was specified.
*===========================================================================
*/
void TransTermDirAdd(HWND hWnd)
{
    SHORT       sFile;                      /* handle of file */
    ULONG      usRead;                     /* read length */
    ULONG       ulActual;                   /* seek length */
    HGLOBAL     hBuffer;                    /* handle of global memory */
    VOID FAR    *pBuffer;                   /* address of global memory */
    DIRECTORY   DirBuf;                     /* directory buffer */

    DirBuf = TransDirList;

    /* ----- convert password ----- */
    TransTermPassConvert(DirBuf.szPassword);

    /* ----- check number of directory item ----- */
    if (getTermIDir()) {
        /* ----- open directory work file ----- */
        if ((sFile = PifOpenFile(szTransFile, "orw")) == PIF_ERROR_FILE_NOT_FOUND) {
            TransMessage(hWnd,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_LOCAL_FILE_OPEN,
                         szTransFile);
            return;
        }

        /* ----- check insert position ----- */
        if (iTransDirIndex != -1) {                 /* current item */
            /* ----- get global memory ----- */
            hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_TERM_DIR_MEM_BUFF);
            if (hBuffer) {
                pBuffer = GlobalLock(hBuffer);
    
                /* ----- save directory item ----- */
                PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * iTransDirIndex, &ulActual);
                usRead = PifReadFile((USHORT)sFile, pBuffer, TRANS_TERM_DIR_MEM_BUFF);
                PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * iTransDirIndex, &ulActual);
                PifWriteFile((USHORT)sFile, &DirBuf, sizeof(DIRECTORY));
                PifWriteFile((USHORT)sFile, pBuffer, usRead);

                /* ----- free global memory ----- */
                GlobalUnlock(hBuffer);
                GlobalFree(hBuffer);
            } else {
                TransMessage(hWnd,
                             MB_ICONEXCLAMATION,
                             MB_OK | MB_ICONEXCLAMATION,
                             IDS_TRANS_NAME,
                             IDS_TRANS_MEMORY);
            }
        } else {                            /* last item */
            PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * getTermIDir(), &ulActual);
            PifWriteFile((USHORT)sFile, &DirBuf, sizeof(DIRECTORY));
        }
    } else {
        /* ----- create directory work file ----- */
        /* 06/30/99 */
        PifDeleteFile(szTransFile);
        if ((sFile = PifOpenFile(szTransFile, "w")) == PIF_ERROR_FILE_NOT_FOUND) {
            TransMessage(hWnd,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_LOCAL_FILE_OPEN,
                         szTransFile);
            return;
        }

        PifWriteFile((USHORT)sFile, &DirBuf, sizeof(DIRECTORY));
    }
    PifCloseFile((USHORT)sFile);

    /* ----- increment number of directory ----- */
    incrTermIDir(1);
}

/*
*===========================================================================
** Synopsis:    void TransTermDirChange(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      nothing
*
** Description: This function changes the data in the directory.
*===========================================================================
*/
void TransTermDirChange(HWND hWnd)
{
    SHORT       sFile;                      /* handle of file */
    ULONG       ulActual;                   /* seek length */
    DIRECTORY   DirBuf;                     /* directory buffer */

    DirBuf = TransDirList;

    /* ----- convert password ----- */
    TransTermPassConvert(DirBuf.szPassword);

    if ((sFile = PifOpenFile(szTransFile, "orw")) == PIF_ERROR_FILE_NOT_FOUND) {
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOCAL_FILE_OPEN,
                     szTransFile);
        return;
    }

    PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * iTransDirIndex, &ulActual);
    PifWriteFile((USHORT)sFile, &DirBuf, sizeof(DIRECTORY));
    PifCloseFile((USHORT)sFile);
}

/*
*===========================================================================
** Synopsis:    void TransTermDirDelete(HWND hWnd)
*     Input:    hWnd - handle of parent of window
*
** Return:      nothing
*
** Description: This function deletes one data in the directory.
*===========================================================================
*/
void TransTermDirDelete(HWND hWnd)
{
    SHORT   sFile;
    SHORT   sStatus;
    ULONG  usRead;
    ULONG   ulActual;
    HGLOBAL hBuffer;
    VOID FAR *pBuffer;

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_TERM_DIR_MEM_BUFF);
    if (hBuffer) {
        /* ----- lock global memory ----- */
        pBuffer = GlobalLock(hBuffer);
    
        /* ----- read directory items ----- */
        if ((sFile = PifOpenFile(szTransFile, "or")) == PIF_ERROR_FILE_NOT_FOUND) {
            TransMessage(hWnd,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_LOCAL_FILE_OPEN,
                         szTransFile);
            return;
        }
        usRead = PifReadFile((USHORT)sFile, pBuffer, (sizeof(DIRECTORY) * iTransDirIndex));
        sStatus = PifSeekFile((USHORT)sFile, sizeof(DIRECTORY) * (iTransDirIndex + 1), &ulActual);
        if (sStatus == PIF_OK) {
            usRead += PifReadFile((USHORT)sFile, (UCHAR FAR *)pBuffer + usRead, (TRANS_TERM_DIR_MEM_BUFF - usRead));
        }
        PifCloseFile((USHORT)sFile);
        
        /* ----- delete work file ----- */
        PifDeleteFile(szTransFile);

        /* ----- create work file ----- */
        if ((sFile = PifOpenFile(szTransFile, "nw")) == PIF_ERROR_FILE_EXIST) {
            TransMessage(hWnd,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_LOCAL_FILE_CREATE,
                         szTransFile);
            return;
        }
        PifWriteFile((USHORT)sFile, pBuffer, usRead);
        PifCloseFile((USHORT)sFile);

        /* ----- release global memory ----- */
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        /* ----- decrement number of directory ----- */

		incrTermIDir(-1);
        if (getTermIDir() == 0) {
            /* ----- delete work file ----- */
            PifDeleteFile(szTransFile);
        }
    } else {                                /* not enough memory */
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION,
                     IDS_TRANS_NAME,
                     IDS_TRANS_MEMORY);
    }
}

/* ===== End of TRADIR.C ===== */
