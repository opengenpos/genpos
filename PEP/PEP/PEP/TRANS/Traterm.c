/*
* ---------------------------------------------------------------------------
* Title         :   Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATERM.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     As for this file, the function concerning terminal
*               communication is defined.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-25-98 : 03.03.00 : A.Mitsui   : Add V3.3
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
#include "tratcomm.h"
#include "Shlobj.h"


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
WCHAR       szTransFile[] = WIDE("DIRECT");
DIRECTORY   TransDirList;
int         iTransDirIndex;
LONG        lTransTester;                   /* counter of loop tester */
WORD        wTransDirCmd;                   /* TRUE  - add of directory */
                                            /* FALSE - change of directory */

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
** Synopsis:    BOOL TransTermExec(HANDLE hInst, HWND hWnd)
*     Input:    hInst - instance to bind
*               hWnd  - Handle of Dialog Procedure
*
** Return:      nothing
*
** Description: This function displays the dialog box of terminal
*               communication.
*===========================================================================
*/
BOOL TransTermExec(HANDLE hInst, HWND hWnd)
{
    DLGPROC lpitDlgProc;

    lpitDlgProc = MakeProcInstance(TransTermDlgProc, hInst);
    DialogBoxPopup(hResourceDll/*hInst*/, MAKEINTRESOURCEW(IDD_TRANS_TERMINAL), hWnd, lpitDlgProc);

    FreeProcInstance(lpitDlgProc);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL WINAPI TransTermDlgProc(HWND hDlg,
*                                            WORD message,
*                                            WORD wParam,
*                                            LONG lParam)
*     Input:    hDlg    - Handle of Dialog Procedure
*               message - Dispathed Message
*               wParam  - 16 Bits Message Parameter
*               lParam  - 32 Bits Message Parameter
*
** Return:      TRUE  - User Process is Executed
*               FALSE - Default Process is Exepected
*
** Description: This function is dialog box procedure of terminal
*               communication.
*===========================================================================
*/
BOOL WINAPI TransTermDlgProc(HWND hDlg, UINT message,
                                     WPARAM wParam, LPARAM lParam)
{
    short       nCount;
    LRESULT     lStatus;
    int         iTransTab[1] = {TRANS_TERM_DIR_TAB};
    static WCHAR szLoopTest[] = WIDE("NCR2172 LOOP TEST = ");
    WCHAR       szCaption[128];
    int         iConnectSelect = 0;
    LRESULT     lConnectSelect;
	/*RPH 12/09/02 SR#77 Flag for selecting or deselecting CheckBoxes*/
	BOOL		markFlag = FALSE;
	WCHAR		szMarked [2], noDir[25];
	short		xCount;
	IMalloc     *imalloc;

	SHGetMalloc(&imalloc);
	
	switch (message) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        
		/* ----- set window caption ----- */
        LoadString(hResourceDll, IDS_TRANS_CAP_TERM, szCaption, PEP_STRING_LEN_MAC(szCaption));
        WindowRedrawText(hDlg, szCaption);

        /* ----- set connection ----- */
        for (nCount = 0; nCount < TRANS_TERM_CONNECT_MAX; nCount++) {
            LoadString(hResourceDll, IDS_TRANS_TERM_LAN + nCount, szCaption, PEP_STRING_LEN_MAC(szCaption));
            DlgItemSendTextMessage(hDlg, IDD_TRANS_TERM_CONNECT, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szCaption));
        }
        /* Saratoga */
		/*Changed to an if-else statement, so that it can decide correctly which mnemonic to use
		  and which Dialog Items to display*/

		if (getTermFByStatus() & TRANS_TERM_FLAG_LAN) 
		{
            iConnectSelect = 0;
		} else if (getTermFByStatus() & TRANS_TERM_FLAG_CONNECT) 
		{
            iConnectSelect = 1;
        } else if (getTermFByStatus() & TRANS_TERM_FLAG_SERIAL)
		{
				iConnectSelect = 2;
		}

        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_CONNECT, CB_SETCURSEL, iConnectSelect,
                           /*
                           (Trans.Term.fbyStatus & TRANS_TERM_FLAG_CONNECT)
                                ? (1) : (0), */
                           0L);

        /* ----- set terminal type ----- */
        for (nCount = 0; nCount < TRANS_TERM_TYPE_MAX; nCount++) {
            LoadString(hResourceDll, IDS_TRANS_TERM_MASTER + nCount, szCaption, PEP_STRING_LEN_MAC(szCaption));
            DlgItemSendTextMessage(hDlg, IDD_TRANS_TERM_NUMBER, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szCaption));
        }
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_NUMBER, CB_SETCURSEL, (WPARAM)getTermByTerm(), 0L);

        /* ----- set port number ----- */
        for (nCount = 0; nCount < TRANS_MAX_PORT; nCount++) {
            DlgItemSendTextMessage(hDlg, IDD_TRANS_TERM_PORT, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTransPort[nCount]));
        }
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_PORT, CB_SETCURSEL, (WPARAM)(getTermByPort() - 1), 0L);

        /* ----- set baud rate ----- */
        for (nCount = 0; nCount < TRANS_MAX_BAUD; nCount++) {
             DlgItemSendTextMessage(hDlg, IDD_TRANS_TERM_BAUD, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTransBaud[nCount]));
        }

        if (iConnectSelect == SELECT_LAN) {
           ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_BAUD_TXT), FALSE);
           ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_BAUD), FALSE);
			//RPH 12/04/2002 added to remove the (COM) PORT dialog when LAN is selected SR#72
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_PORT_TXT), FALSE);	
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_PORT), FALSE);
		   //RPH 12/10/02 SR#79 Display if LAN is Selected
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_IP), TRUE);
        } else {
           ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_BAUD_TXT), TRUE);
           ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_BAUD), TRUE);
			//RPH 12/04/2002 show when LAN is not Selected SR#72
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_PORT_TXT), TRUE);
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_PORT), TRUE);
		   //Not needed unless LAN is Selected
		   ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_IP), FALSE); //RPH 12/10/02 SR#79
		   
           SendDlgItemMessage(hDlg, IDD_TRANS_TERM_BAUD, CB_SETCURSEL, (WPARAM)TransGetIndex(getTermWBaudRate()), 0L);
        }

        /* ----- select font into the directory list box ----- */
        PepSetFont(hDlg, IDD_TRANS_TERM_DIR);

        /* ----- set tab stop of directory ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_DIR, LB_SETTABSTOPS, (WPARAM)1, (LPARAM)(iTransTab)); /* set list box tab stop */
        TransTermInsertDir(hDlg);         /* show directory */

        /* ----- set change password ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_PASS, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_PASS) ? (1) : (0)),
                           0L);

        /* ----- set all broadcast ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_ALL, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_ALL_CAST) ? (1) : (0)),
                           0L);

        /* ----- set super. broadcast ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_SUP, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_SUP_CAST) ? (1) : (0)),
                           0L);

        /* ----- set layout broadcast ----- */ //ESMITH LAYOUT
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_LAY, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_LAY_CAST) ? (1) : (0)),
                           0L);

        /* ----- set plu broadcast ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_PLU, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_PLU_CAST) ? (1) : (0)),
                           0L);

		        /* ----- set plu broadcast ----- */
        SendDlgItemMessage(hDlg, IDD_TRANS_TERM_LOGO, BM_SETCHECK,
                           (WPARAM)((getTermFByStatus() & TRANS_TERM_FLAG_LOGO_CAST) ? (1) : (0)),
                           0L);
		if(szCommIconSaveName[0])
		{
			DlgItemRedrawText(hDlg,IDD_ICON_DIR_INFO  , szCommIconSaveName);
		} else
		{
			LoadString(hResourceDll,IDS_PEP_NONE, noDir, PEP_STRING_LEN_MAC(noDir)); 
			DlgItemRedrawText(hDlg,IDD_ICON_DIR_INFO  , noDir);
		}

        /* ----- set file selection ----- */
        fButtonCtrl = TransTermSelectFilesSet(hDlg);

        /* ----- disable add/delete/send/receive push button ----- */
        TransTermControlButton(hDlg);

        /* ----- enable/disable select files ----- */
        TransTermSelectFilesEnable(hDlg);

        TransIPDefaultData();   /* saratoga */
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_TRANS; j <= IDD_TRANS_GROUPBOX4; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j = IDD_TRANS_LOADER_PORT; j <= IDD_TRANS_LOADER_GROUPBOX2; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_TRANS_TERM_TRA_PROG:
        case IDD_TRANS_TERM_TRA_ACT:
        case IDD_TRANS_TERM_TRA_DEPT:
        case IDD_TRANS_TERM_TRA_PLU: 
        case IDD_TRANS_TERM_TRA_WAI:
        case IDD_TRANS_TERM_TRA_CAS:
        case IDD_TRANS_TERM_TRA_ETK:
        case IDD_TRANS_TERM_TRA_CPN:
        case IDD_TRANS_TERM_TRA_CSTR:
        case IDD_TRANS_TERM_TRA_RPT:
        case IDD_TRANS_TERM_TRA_PPI:
        case IDD_TRANS_TERM_TRA_MLD:
		case IDD_TRANS_TERM_TRA_TOUCH: //ESMITH LAYOUT
		case IDD_TRANS_TERM_TRA_KEYBRD: //ESMITH LAYOUT
		case IDD_TRANS_TERM_TRA_LOGO:
		case IDD_TRANS_TERM_TRA_ICON:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- get file selection ----- */
                fButtonCtrl = TransTermSelectFilesGet(hDlg);

                /* ----- enable send/receive push button ----- */
                TransTermControlButton(hDlg);
                SetFocus((HWND)lParam);

                return TRUE;
            }
            return FALSE;

        case IDD_TRANS_TERM_CONNECT:        /* ###ADD Saratoga */
            /* ----- get connection ----- */
            lConnectSelect = SendDlgItemMessage(hDlg, IDD_TRANS_TERM_CONNECT, CB_GETCURSEL, 0, 0L);
            if (lConnectSelect == SELECT_LAN){
                ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_BAUD_TXT), FALSE);
                ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_BAUD), FALSE);
				//RPH 12/04/2002 SR#72 Don't Show PORT when LAN is Selected
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_PORT_TXT), FALSE);
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_PORT), FALSE);
				//RPH 12/10/02 SR#79 Display if LAN is Selected
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_IP), TRUE);
            } else {
                ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_BAUD_TXT), TRUE);
                ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_BAUD), TRUE);
				//RPH 12/04/2002 SR#72 Show PORT when LAN is Not Selected
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_LOADER_PORT_TXT), TRUE);
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_PORT), TRUE);
				//Not needed unless LAN is Selected
				ShowWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_IP), FALSE); //RPH 12/10/02 SR#79
				
				//RPH 12/10/02 Courtesy Bug fix found While fixing SR#72
				//Baud Rate was not being set in box when displayed
				SendDlgItemMessage(hDlg, IDD_TRANS_TERM_BAUD, CB_SETCURSEL, (WPARAM)TransGetIndex(getTermWBaudRate()), 0L);
            }
            return FALSE;                   /* ###ADD Saratoga */

        case IDD_TRANS_TERM_NUMBER:
            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- get dialog box status ----- */
                TransTermStatus(hDlg);

                /* ----- enable/disable select files ----- */
                TransTermSelectFilesEnable(hDlg);

				/* ----- get new send list ------ */
				TransTermSelectFilesGet(hDlg);

                /* ----- enable send/receive push button ----- */
                TransTermControlButton(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDD_TRANS_TERM_DIR:
            switch (HIWORD(wParam)) {
            case LBN_DBLCLK:
                TransTermChangeDir(hDlg);
                return TRUE;

            case LBN_SELCHANGE:
                /* ----- enable send/receive push button ----- */
                TransTermControlButton(hDlg);
                return TRUE;

            default:
                break;
            }
            break;

        case IDD_TRANS_TERM_DIR_ADD:        /* add directory */
            /* ----- set function number ----- */
            wTransDirCmd = TRUE;

            /* ----- set directory insert position ----- */
            if ((lStatus = SendDlgItemMessage(hDlg, IDD_TRANS_TERM_DIR, LB_GETCURSEL, 0, 0L)) != LB_ERR) {
                iTransDirIndex = (int)lStatus;
            } else {
                iTransDirIndex = -1;        /* last position */
            }

            /* ----- show directory dialog box ----- */
            if (TransTermDir(hDlg) == TRUE) {
                /* ----- refresh directory list box ----- */
                TransTermInsertDir(hDlg);

                /* ----- enable change/delete/send/receive push button ----- */
                TransTermControlButton(hDlg);
            }
            return TRUE;

        case IDD_TRANS_TERM_DIR_CHANGE:     /* change directory */
            TransTermChangeDir(hDlg);
            return TRUE;

        case IDD_TRANS_TERM_DIR_DELETE:     /* delete directory */
            /* ----- get directory selecting position ----- */
            if ((lStatus = SendDlgItemMessage(hDlg, IDD_TRANS_TERM_DIR, LB_GETCURSEL, 0, 0L)) != LB_ERR) {
                if (TransMessage(hDlg, MB_ICONEXCLAMATION, MB_YESNO | MB_ICONEXCLAMATION, IDS_TRANS_NAME, IDS_TRANS_DELETE) == IDYES) {
                    iTransDirIndex = (int)lStatus;

                    /* ----- delete directory item ----- */
                    TransTermDirDelete(hDlg);

                    /* ----- refresh directory list box ----- */
                    TransTermInsertDir(hDlg);

                    /* ----- disable change/delete/send/receive push button ----- */
                    TransTermControlButton(hDlg);
                }
            } else {                        /* directory not selected */
                MessageBeep(0);             /* error tone */
            }
            return TRUE;

        case IDD_TRANS_TERM_SEND:           /* send message */
        case IDD_TRANS_TERM_RECEIVE:        /* receive message */
            /* ----- get dialog box status ----- */
            TransTermStatus(hDlg);

            /* ----- get directory selecting position ----- */
            if ((lStatus = SendDlgItemMessage(hDlg, IDD_TRANS_TERM_DIR, LB_GETCURSEL, 0, 0L)) != LB_ERR) {
                iTransDirIndex = (int)lStatus;

                /* ----- read directory item ----- */
                TransTermDirRead(hDlg);

                /* ----- check loop tester ----- */
                if (_memicmp(TransDirList.szName, szLoopTest, 20 * sizeof(WCHAR)) == 0) {    /* enable */
                    lTransTester = _wtol(TransDirList.szName + 20);
                } else {
                    lTransTester = 1;
                }

                do {
                    if (LOWORD(wParam) == IDD_TRANS_TERM_SEND) {
                        /* ----- send parameter/file ----- */
                        if (getTermFByStatus() & TRANS_TERM_FLAG_LAN) {
                            /* Lan, saratoga */
                            TransTermSendLan(hDlg);
                        } else {
                            /* conventional RS232C */
                            TransTermSend(hDlg);
                        }
                    } else {
                        /* ----- receive parameter/file ----- */
                        if (getTermFByStatus() & TRANS_TERM_FLAG_LAN) {
                            /* Lan, saratoga */
                            TransTermReceiveLan(hDlg);
                        } else {
                            /* conventional RS232C */
                            TransTermReceive(hDlg);
                        }
                    }
                    /* ----- delete programmable report file ----- */
                    PifDeleteFile(szTransProgRpt);
                    /* ----- set file select status ----- */
                    TransTermSelectFilesSet(hDlg);
                } while (/*--lTransTester*/--lTransTester > 0);
            } else {                        /* directory not selected */
                MessageBeep(0);             /* error tone */
            }
            return TRUE;

        case IDD_TRANS_TERM_IP:             /* IP setting dialog box, saratoga */
            TransIP(hDlg);
            return TRUE;

		case IDD_TRANS_TERM_ICON_CHANGE:
			TransTermSetIconDirectory(hDlg);
			break;

        case IDD_TRANS_TERM_CLOSE:          /* close terminal communication */
            /* ----- get dialog box status ----- */
            TransTermStatus(hDlg);
            /* break */                     /* not use */
        case IDCANCEL:                      /* cancel */
            EndDialog(hDlg, TRUE);
            return TRUE;

        /*RPH 12/09/02 SR#77 Mark/Unmark Files if Clicked*/
		case IDD_TRANS_TERM_SET_ALL:
			markFlag = TRUE;
			/*no break because the same code is run for both the set all
			and the clear all.
			The difference is that Set all needs markFlag to be TRUE and 
			clear all needs markFlag to stay as initialized (FALSE)*/

		case IDD_TRANS_TERM_CLEAR:
			for (nCount = IDD_TRANS_TERM_TRA_PROG;
				nCount <= IDD_TRANS_TERM_TRA_ICON; nCount++) { //ESMITH LAYOUT
				    if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {  /* terminal is master */
						SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, markFlag, 0L);
					} else { /* terminal is sattelite */
						if (nCount == IDD_TRANS_TERM_TRA_PLU || nCount == IDD_TRANS_TERM_TRA_WAI || nCount == IDD_TRANS_TERM_TRA_CAS || nCount == IDD_TRANS_TERM_TRA_ETK 
							|| nCount == IDD_TRANS_TERM_TRA_RPT)
						{
							SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, FALSE, 0L);
						}
						else {
							SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, markFlag, 0L);
						}
					}
			}

            /* ----- get file selection ----- */
            fButtonCtrl = TransTermSelectFilesGet(hDlg);

            /* ----- enable send/receive push button ----- */
            TransTermControlButton(hDlg);
			return TRUE;

		case IDD_TRANS_TERM_SET_CHANGED:
			/*This steps through the Changed indicator text and the associated check box
			to find an X in the Changed indicator text and check the associated check box
			if an X is found, or uncheck the associated check box if no X is
			found (NULL) in the Changed indicator text.*/
			for (nCount = IDD_TRANS_TERM_CHG_PROG, xCount = IDD_TRANS_TERM_TRA_PROG;
				nCount <= IDD_TRANS_TERM_CHG_ICON; //ESMITH LAYOUT
				nCount++, xCount++) {
				
				DlgItemGetText(hDlg, nCount, szMarked, sizeof(szMarked));
				
				if (szMarked[0]) {
					markFlag = TRUE;
				} else {
					markFlag = FALSE;
				}
				SendDlgItemMessage(hDlg, xCount, BM_SETCHECK, markFlag, 0L);
			}

            /* ----- get file selection ----- */
            fButtonCtrl = TransTermSelectFilesGet(hDlg);

            /* ----- enable send/receive push button ----- */
            TransTermControlButton(hDlg);

			return TRUE;

		default:                            /* other message */
            return FALSE;
        }

    default:
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    void TransTermInsertDir(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      nothing
*
** Description: The data in the directory is read from the work file and
*               this function is displayed to the list box.
*===========================================================================
*/
void TransTermInsertDir(HWND hWnd)
{
    int         iCount;
    DIRECTORY   TransDirListTmp = {0};
    DWORD       dwCount;

    /* ----- delete directory item ----- */
    while (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_DELETESTRING, 0, 0L) > 0) {
        ;                                   /* dummy */
    }

    if ((iCount = getTermIDir()) > 0) {
		SHORT       sFile;                      /* file handle */

        /* ----- open directory work file ----- */
        if ((sFile = PifOpenFile(szTransFile, "or")) != PIF_ERROR_FILE_NOT_FOUND) {
            do {
				WCHAR        szDirBuffer[TRANS_TERM_DIR_LEN + 6] = {0};  // add room for "..\t" and ".." and zero terminator.

                /* ----- read directory item ----- */
                if (PifReadFile((USHORT)sFile, &TransDirListTmp, sizeof(TransDirListTmp)) != sizeof(TransDirListTmp)) {
                    break;                  /* EOF or read error */
                }

                /* ----- edit name ----- */
                wcsncpy(szDirBuffer, TransDirListTmp.szName, TRANS_TERM_DIR_NAME_LEN);
                if (wcslen(TransDirListTmp.szName) <= TRANS_TERM_DIR_NAME_LEN) {
                    wcscat(szDirBuffer, WIDE("\t"));
                } else {
                    wcscat(szDirBuffer, WIDE("..\t"));
                }
                
                /* ----- edit phone number ----- */
                wcsncat(szDirBuffer, TransDirListTmp.szNumber, TRANS_TERM_DIR_NO_LEN);
                if (wcslen(TransDirListTmp.szNumber) > TRANS_TERM_DIR_NO_LEN) {
                    wcscat(szDirBuffer, WIDE(".."));
                }

                /* ----- insert list box ----- */
                DlgItemSendTextMessage(hWnd, IDD_TRANS_TERM_DIR, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDirBuffer));
            } while (--iCount);

            /* ----- close file ----- */
            PifCloseFile((USHORT)sFile);
        } else {
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION, IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_OPEN, szTransFile);
        }
     } else {
		//no directory file, make the directory file, and default connection
		SHORT       sFile;                      /* file handle */

		PifDeleteFile(szTransFile);
        if ((sFile = PifOpenFile(szTransFile, "w")) == PIF_ERROR_FILE_NOT_FOUND) {
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION, IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_OPEN, szTransFile);
            return;
        }

        LoadString(hResourceDll, IDS_TRANS_DEFAULT_DIRECT, TransDirListTmp.szName, PEP_STRING_LEN_MAC(TransDirListTmp.szName));

#ifdef _DEBUG

		wcscpy(TransDirListTmp.szPassword, WIDE("          "));
		TransTermPassConvert(TransDirListTmp.szPassword);
#endif

		PifWriteFile((USHORT)sFile, &TransDirListTmp, sizeof(DIRECTORY));
        DlgItemSendTextMessage(hWnd, IDD_TRANS_TERM_DIR, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(TransDirListTmp.szName));

		PifCloseFile(sFile);
	}

    /* ----- set cursor selected ----- */
    if ((dwCount = SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_GETCOUNT, 0, 0L)) != LB_ERR) {
        SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_SETCURSEL, (WPARAM)(dwCount - 1), 0L);
    }
}

/*
*===========================================================================
** Synopsis:    void TransTermChangeDir(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      nothing
*
** Description: The data in the directory is read from the work file and
*               this function is displayed to the list box.
*===========================================================================
*/
void TransTermChangeDir(HWND hWnd)
{
    LRESULT     lStatus;

    /* ----- get directory selecting position ----- */
    if ((lStatus = SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_GETCURSEL, 0, 0L)) != LB_ERR) {
        /* ----- set function number ----- */
        wTransDirCmd = FALSE;

        iTransDirIndex = (int)lStatus;

        /* ----- show directory dialog box ----- */
        /* ----- refresh directory list box ----- */
        TransTermDir(hWnd);
        TransTermInsertDir(hWnd);

        /* ----- set chnage password ----- */
        SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_SETCURSEL, (WPARAM)iTransDirIndex, 0L);
    } else {                        /* directory not selected */
        MessageBeep(0);             /* error tone */
    }
}

/*
*===========================================================================
** Synopsis:    void TransTermStatus(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
void TransTermStatus(HWND hWnd)
{
    LRESULT     lStatus;
    TERMINAL    TermOld = getTransTerm();
    LRESULT     lConnectSelect;

    /* ----- get connection ----- */
    /* saratoga */
    lConnectSelect = SendDlgItemMessage(hWnd, IDD_TRANS_TERM_CONNECT, CB_GETCURSEL, 0, 0L);
    if (lConnectSelect == 1) {
        /* modem connection */
        andTermFByStatus( ~ (TRANS_TERM_FLAG_CONNECT | TRANS_TERM_FLAG_LAN));
        orTermFByStatus(TRANS_TERM_FLAG_CONNECT);
    } else if (lConnectSelect == 0) {
        /* LAN connection */
        andTermFByStatus( ~ (TRANS_TERM_FLAG_CONNECT | TRANS_TERM_FLAG_LAN));
		orTermFByStatus(TRANS_TERM_FLAG_LAN);
    } else {
        /* direct RS232C connection */
        andTermFByStatus(~(TRANS_TERM_FLAG_CONNECT|TRANS_TERM_FLAG_LAN));
		orTermFByStatus(IDD_TRANS_TERM_CONNECT);
		/*This OR statement basically makes the status to say that it should be in the 3rd transfer method.
		Since the 3rd transfer method has been changed from LAN to Serial, we do this Status now for a Serial or
		modem connection*/
	}
    /* ----- get terminal type ----- */
    setTermByTerm((BYTE)SendDlgItemMessage(hWnd, IDD_TRANS_TERM_NUMBER, CB_GETCURSEL, 0, 0L));

    /* ----- get port number ----- */
    lStatus = SendDlgItemMessage(hWnd, IDD_TRANS_TERM_PORT, CB_GETCURSEL, 0, 0L);
    setTermByPort((BYTE)(lStatus + 1));

    /* ----- get baud rate ----- */
    lStatus = SendDlgItemMessage(hWnd, IDD_TRANS_TERM_BAUD, CB_GETCURSEL, 0, 0L);
	if(lStatus == -1)//baud rate not set, set to default
	{
		lStatus = 0;
	}
    setTermWBaudRate((WORD)_wtoi(szTransBaud[(WORD)lStatus]));

    /* ----- get chnage password ----- */
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_PASS, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_PASS);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_PASS);
    }

    /* ----- get all broadcast ----- */
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_ALL, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_ALL_CAST);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_ALL_CAST);
    }

    /* ----- get super. broadcast ----- */
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_SUP, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_SUP_CAST);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_SUP_CAST);
    }

    /* ----- get plu broadcast ----- */
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_PLU, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_PLU_CAST);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_PLU_CAST);
    }

	/* ----- get layout broadcast ----- */ //ESMITH LAYOUT
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_LAY, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_LAY_CAST);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_LAY_CAST);
    }

		/* ----- get layout broadcast ----- */ //ESMITH LAYOUT
    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_LOGO, BM_GETCHECK, 0, 0L)) {
        orTermFByStatus(TRANS_TERM_FLAG_LOGO_CAST);
    } else {
        andTermFByStatus(~TRANS_TERM_FLAG_LOGO_CAST);
    }

    if (memcmp(getPtrTransTerm(), &TermOld, sizeof(TERMINAL))) {
        PepSetModFlag(hwndPepMain, PEP_MF_TRANS, 0);
    }
}

/*
*===========================================================================
** Synopsis:    void TransTermControlButton(HWND hWnd)
*     Input:    hWnd     - handle of parent window
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
void TransTermControlButton(HWND hWnd)
{
    BOOL    fTransCtrl = FALSE;
    BOOL    fDirCtrl = FALSE;

    if (SendDlgItemMessage(hWnd, IDD_TRANS_TERM_DIR, LB_GETCOUNT, 0, 0L) > 0) {
        fDirCtrl = TRUE;
        if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {    /* terminal is master */
            fTransCtrl = fButtonCtrl;
        } else {                                            /* terminal is satellite */
            fTransCtrl = (getTransFwTransTerm() & (TRANS_ACCESS_PARA |
                                               TRANS_ACCESS_DEPT |
                                               TRANS_ACCESS_CPN  |
                                               TRANS_ACCESS_CSTR |
                                               TRANS_ACCESS_PLU  |
                                               TRANS_ACCESS_PPI  |
                                               TRANS_ACCESS_LAYTOUCH | //ESMITH LAYOUT
											   TRANS_ACCESS_LAYKEYBRD|
											   TRANS_ACCESS_LOGO |
											   TRANS_ACCESS_ICON))
                              ? TRUE : FALSE;
        }
    }
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_TERM_DIR_CHANGE), fDirCtrl);
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_TERM_DIR_DELETE), fDirCtrl);
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_TERM_SEND),    fTransCtrl);
    EnableWindow(GetDlgItem(hWnd, IDD_TRANS_TERM_RECEIVE), fTransCtrl);

    if (fDirCtrl == FALSE) {
        SetFocus(GetDlgItem(hWnd, IDD_TRANS_TERM_NUMBER));
    }
}

void TransTermSetIconDirectory(HWND hWnd)
{
	BROWSEINFO bi = {0}; 
	ITEMIDLIST *pidl;
	/*RPH 12/09/02 SR#77 Flag for selecting or deselecting CheckBoxes*/

	bi.hwndOwner = hWnd;
	bi.lpszTitle = WIDE("Select the folder where your icon files will reside");
	bi.ulFlags = BIF_RETURNONLYFSDIRS ;

	pidl = SHBrowseForFolder(&bi);
	if ( pidl != 0 )
	{
		SHGetPathFromIDList( pidl, szCommIconSaveName );
		TransAccessData(TRANS_ACCESS_ICON);
		DlgItemRedrawText(hWnd, IDD_ICON_DIR_INFO, szCommIconSaveName);
	}
}
/* ===== End of TRATERM.C ===== */
