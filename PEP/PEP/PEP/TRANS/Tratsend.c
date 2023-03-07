/*
* ---------------------------------------------------------------------------
* Title         :   Send to Terminal
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATSEND.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     As for this file, the function, which transmits the
*               parameter and the file to 2170, is defined.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* May-19-94 : 01.01.00 : M.Yamamoto : Changed TransTermSendCheckTotal()
* apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Jan-22-98 : 01.00.08 : M.Ozawa    : Send CLASS_PARAPLUNOMENU and CLASS_PARAFSC
*                                   : simultaniously, even if only SETUP Parameter
*                                   : is selected.
* Sep-08-98 : 03.03.00 : A.Mitsui   : Add V3.3 MLD file and WaitLoop
* Jan-28-00 : 01.00.00 : hrkato     : Saratoga
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
#include <memory.h>
#include <malloc.h>
#include	"AW_Interfaces.h"
#include <direct.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pcstbpar.h>
#include <pcstbisp.h>
#include <rmt.h>
#include <r20_pif.h>
#include <isp.h>
#include <csetk.h>     /* Adds 1993-10-09 */
#include <pcstbetk.h>  /* Adds 1993-10-09 */
#include <pcstbttl.h>  /* Adds 1994-5-19  */

#include "pep.h"
#include "pepcalc.h"
#include "pepcomm.h"
#include "pepflag.h"

#include "layout.h"
#include "trans.h"
#include "transl.h"
#include "traterm.h"
#include "tratcomm.h"
#include "FILE.H"

DWORD		dwFileSize; //ESMITH LAYOUT
static DWORD dwKeybrdFSize, dwLogoFSize, dwLogoTotalFSize, dwLogoIndexFSize;
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
** Synopsis:    short TransTermSend(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function displays the message and starts transmission.
*===========================================================================
*/
short TransTermSend(HWND hWnd)
{
    SHORT   sStatus;

    if (TransTermSendWarning(hWnd) == TRANS_TERM_COMM_ABORT_USER) {
        return FALSE;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));                      /* set wait cursor  */

    if (TransTermCommPortOpen(hWnd) == TRUE) {

        TransAbortCreateDlg(hWnd);          /* create abort dialog box */

        if (getTermFByStatus() & TRANS_TERM_FLAG_CONNECT) {    /* connect Modem */
            if ((sStatus = TransTermCommModemConnect(TransAbort.hDlg)) == TRUE) {
                PifSleep(TRANS_TERM_COMM_MODEM_WAIT);
            } else {
                TransAbortDestroyDlg();
                TransTermCommPortClose(hWnd);
                return FALSE;
            }
        }

        if ((sStatus = TransTermCommLogon(TransAbort.hDlg)) == TRUE) {
            if (TransTermPass(TransAbort.hDlg) == TRUE) {
                sStatus = TransTermSendData(TransAbort.hDlg);
            } else {
                sStatus = TRANS_TERM_COMM_ABORT_USER;
            }
        } else {
            if (getTermFByStatus() & TRANS_TERM_FLAG_CONNECT) {    /* disconnect Modem */
                TransTermCommModemDisconnect(TransAbort.hDlg);
            }
            TransAbortDestroyDlg();
            TransTermCommPortClose(hWnd);
            return FALSE;
        }

        TransTermCommLogoff(TransAbort.hDlg);

        if (getTermFByStatus() & TRANS_TERM_FLAG_CONNECT) {    /* connect Modem */
            TransTermCommModemDisconnect(TransAbort.hDlg);
        }

        if (sStatus > 0) {              /* check sending status */
            if (lTransTester == 1) {
                TransMessage(TransAbort.hDlg, MB_ICONASTERISK, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_SUC_TRANSFER);
            }
            TransResetFlexMem();
            TransResetData();
            sStatus = TRUE;
        } else {
            TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_SEND);

            sStatus = FALSE;
        }

        TransAbortDestroyDlg();
        TransTermCommPortClose(hWnd);

        return (sStatus);

    } else {
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermSendLan(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function displays the message and starts transmission. Saratoga
*===========================================================================
*/
short TransTermSendLan(HWND hWnd)
{
    SHORT   sStatus;

    if (TransTermSendWarning(hWnd) == TRANS_TERM_COMM_ABORT_USER) {
        return FALSE;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));                      /* set wait cursor  */

    if (TransTermCommLanOpen(hWnd) == TRUE) {
        TransAbortCreateDlg(hWnd);          /* create abort dialog box */

        if ((sStatus = TransTermCommLanLogon(TransAbort.hDlg)) == TRUE) {
        /* if ((sStatus = TransTermCommLogon(TransAbort.hDlg)) == TRUE) { */
            if (TransTermPass(TransAbort.hDlg) == TRUE) {
                sStatus = TransTermSendData(TransAbort.hDlg);
            } else {
                sStatus = TRANS_TERM_COMM_ABORT_USER;
            }
        } else {
            TransAbortDestroyDlg();
            TransTermCommLanClose(hWnd);
            return FALSE;
        }
        TransTermCommLanLogoff(TransAbort.hDlg);
        /* TransTermCommLogoff(TransAbort.hDlg); */

        if (sStatus > 0) {              /* check sending status */
            if (lTransTester == 1) {
                TransMessage(TransAbort.hDlg, MB_ICONASTERISK, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_SUC_TRANSFER);
            }
            TransResetFlexMem();
            TransResetData();
            sStatus = TRUE;
        } else {
            TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_SEND);
            sStatus = FALSE;
        }

        TransAbortDestroyDlg();
        TransTermCommLanClose(hWnd);

        return (sStatus);
    } else {
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermSendWarning(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE             - continue
*               TRANS_TERM_COMM_ABORT_USER - abort by user
*
** Description:
*===========================================================================
*/
short TransTermSendWarning(HWND hWnd)
{
	HANDLE		handleRead, handleWrite;
	DWORD		bytesRead, actualBytes, dwLogoTempSize;
	WCHAR		szPathName[MAX_PATH],szTempLogoIndex[MAX_PATH];
	INT			i;
	SHORT		sFile;
	FLOGOFILE	myFile;


    if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {  /* terminal is master */
        /* ----- check sending file ----- */
        if (((getTransFwUpdate() & TRANS_ACCESS_PROG) && !(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_ACT) && !(getTransFwTransTerm() & TRANS_ACCESS_ACT)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_DEPT) && !(getTransFwTransTerm() & TRANS_ACCESS_DEPT)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_PLU) && !(getTransFwTransTerm() & TRANS_ACCESS_PLU)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_WAITER) && !(getTransFwTransTerm() & TRANS_ACCESS_WAITER)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_CASHIER) && !(getTransFwTransTerm() & TRANS_ACCESS_CASHIER)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_CPN) && !(getTransFwTransTerm() & TRANS_ACCESS_CPN)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_CSTR) && !(getTransFwTransTerm() & TRANS_ACCESS_CSTR)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_ETK) && !(getTransFwTransTerm() & TRANS_ACCESS_ETK)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_RPT) && !(getTransFwTransTerm() & TRANS_ACCESS_RPT)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_PPI) && !(getTransFwTransTerm() & TRANS_ACCESS_PPI)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_MLD) && !(getTransFwTransTerm() & TRANS_ACCESS_MLD))||
			 ((getTransFwUpdate() & TRANS_ACCESS_LAYTOUCH) && !(getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH)) || //ESMITH LAYOUT
			 ((getTransFwUpdate() & TRANS_ACCESS_LAYKEYBRD) && !(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD))||
			 ((getTransFwUpdate() & TRANS_ACCESS_LOGO) && !(getTransFwTransTerm() & TRANS_ACCESS_LOGO))) {
            if (TransMessage(hWnd, MB_ICONEXCLAMATION, MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_WAR_FILE) == IDNO) {
                return TRANS_TERM_COMM_ABORT_USER;
            }
        }
        if (getTransFwTransTerm() & TRANS_ACCESS_RPT) {
            if ( TransConvertToParaFile(hWnd) == FALSE) {
                TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_CONVERT);
                return TRANS_TERM_COMM_ABORT_USER;
            }
        }

    } else {                                       /* terminal is sattelite */
        /* ----- check sending file ----- */
        if (((getTransFwUpdate() & TRANS_ACCESS_PROG) && !(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_ACT) && !(getTransFwTransTerm() & TRANS_ACCESS_ACT)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_DEPT) && !(getTransFwTransTerm() & TRANS_ACCESS_DEPT)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_CPN) && !(getTransFwTransTerm() & TRANS_ACCESS_CPN)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_CSTR) && !(getTransFwTransTerm() & TRANS_ACCESS_CSTR)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_PLU) && !(getTransFwTransTerm() & TRANS_ACCESS_PLU)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_PPI) && !(getTransFwTransTerm() & TRANS_ACCESS_PPI)) ||
            ((getTransFwUpdate() & TRANS_ACCESS_MLD) && !(getTransFwTransTerm() & TRANS_ACCESS_MLD))) {
            if (TransMessage(hWnd, MB_ICONEXCLAMATION, MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_WAR_FILE) == IDNO) {
                return TRANS_TERM_COMM_ABORT_USER;
            }
        }
    }

	//ESMITH LAYOUT
	if(getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) {
		if(szCommTouchLayoutSaveName[0] == _T('\0')) {
			if(! LayoutFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommTouchLayoutSaveName,PEP_MF_LAYTOUCH)) {
				return TRANS_TERM_COMM_ABORT_USER;
			}
		}

#if 1
		GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
		GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
		szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
		wcscat(szPathName, WIDE("\\"));
#endif
		wcsncat(szPathName, szTransTouchLAY, 10 - 1);


		handleRead = CreateFilePep(&szCommTouchLayoutSaveName[0], GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		handleWrite= CreateFilePep(&szPathName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
							OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
		dwFileSize = GetFileSize(handleRead, NULL);
		{
			DWORD *ulBuffer = malloc(dwFileSize);
            if (ulBuffer) {
                ReadFile(handleRead, ulBuffer, dwFileSize, &bytesRead, NULL);
                WriteFile(handleWrite, ulBuffer, dwFileSize, &actualBytes, NULL);
                free(ulBuffer);
            }
		}

		CloseHandle(handleWrite);
		CloseHandle(handleRead);
	}

	if(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD){
		if(szCommKeyboardLayoutSaveName[0] == _T('\0')) {
			if(!LayoutFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommKeyboardLayoutSaveName,PEP_MF_LAYKEYBRD)) {
				return TRANS_TERM_COMM_ABORT_USER;
			}
		}

#if 1
		GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
		GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
		szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
		wcscat(szPathName, WIDE("\\"));
#endif
		wcsncat(szPathName, szTransKeyboardLAY, 10 - 1);


		handleRead = CreateFilePep(&szCommKeyboardLayoutSaveName[0], GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		handleWrite= CreateFilePep(&szPathName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
							OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
		dwKeybrdFSize = GetFileSize(handleRead, NULL);
		{
			DWORD *ulBuffer = malloc(dwKeybrdFSize);
            if (ulBuffer) {
                ReadFile(handleRead, ulBuffer, dwKeybrdFSize, &bytesRead, NULL);
                WriteFile(handleWrite, ulBuffer, dwKeybrdFSize, &actualBytes, NULL);
                free(ulBuffer);
            }
		}

		CloseHandle(handleWrite);
		CloseHandle(handleRead);
	}

	if(getTransFwTransTerm() & TRANS_ACCESS_LOGO)
	{
		sFile = PifOpenFile(szTransLogoIndex,"or");

		if(PifReadFile(sFile,&myFile, sizeof(myFile)) != sizeof(myFile))
			memset(&myFile,0,sizeof(myFile));	//reset myFile if junk was read (NSM)
		PifCloseFile(sFile);

		if(szCommLogoSaveName[0] == _T('\0')) {
			memcpy(szCommLogoSaveName,myFile.aszFileName[0], sizeof(szCommLogoSaveName));
		}

		if(szCommLogoSaveName[0] == _T('\0')) {
			if(!LogoFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommLogoSaveName,PEP_MF_LOGO)) {
				return TRANS_TERM_COMM_ABORT_USER;	//why was this commented out? (NSM)
			}
		}

#if 1
		GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
		GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
		szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
		wcscat(szPathName, WIDE("\\"));
#endif
		wcsncat(szPathName, szTransReceiptLogo, 10 - 1);

		handleRead = CreateFilePep(&szCommLogoSaveName[0], GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		handleWrite= CreateFilePep(&szPathName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
							OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);


		dwLogoFSize = GetFileSize(handleRead, NULL);
		if(dwLogoFSize ==  INVALID_FILE_SIZE)
		{
			dwLogoFSize = 0;
		}else
		{
			DWORD *ulBuffer = malloc (dwLogoFSize);
			if (ulBuffer) {
				ReadFile(handleRead, ulBuffer, dwLogoFSize, &bytesRead, NULL);
				WriteFile(handleWrite, ulBuffer, dwLogoFSize, &actualBytes, NULL);
				free (ulBuffer);
			}
		}

		CloseHandle(handleWrite);
		CloseHandle(handleRead);
		
#if 1
		GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
		GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
		szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
		wcscat(szPathName, WIDE("\\"));
#endif
		wcsncat(szPathName, szTransLogoIndex, 10 - 1);

		handleRead = CreateFilePep(&szPathName[0], GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		dwLogoIndexFSize = GetFileSize(handleRead, NULL);
		CloseHandle(handleRead);

		//Create LOGO IMAGES 
		for( i = 2; i < 10; i++)
		{
			dwLogoTempSize = 0;
#if 1
			GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
			GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
			szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
			wcscat(szPathName, WIDE("\\"));
#endif
			wsPepf(szTempLogoIndex,szTransLogoFile, i);
			wcsncat(szPathName, szTempLogoIndex, 10 - 1);


			handleRead = CreateFilePep(&myFile.aszFileName[i-1][0], GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
			handleWrite= CreateFilePep(&szPathName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
								OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);

			dwLogoTempSize = GetFileSize(handleRead, NULL);
			//fixup an invalid read so as not to attempt to alloca the entire universe (NSM)
			if(dwLogoTempSize ==  INVALID_FILE_SIZE)
				dwLogoTempSize = 0;
			dwLogoTotalFSize +=  dwLogoTempSize;
			{
				DWORD *ulBuffer = malloc (dwLogoTempSize);
				if (ulBuffer) {
					ReadFile(handleRead, ulBuffer, dwLogoTempSize, &bytesRead, NULL);
					WriteFile(handleWrite, ulBuffer, dwLogoTempSize, &actualBytes, NULL);
					free (ulBuffer);
				}
			}
			CloseHandle(handleWrite);
			CloseHandle(handleRead);

		}
	}

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermSendData(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the parameter and each file to 2170.
*===========================================================================
*/
short TransTermSendData(HWND hWnd)
{
    short   sStatTerm;
    short   sStatus;
	short	sMode;
	int		i = 0;
	WCHAR		szPathName[MAX_PATH],szTempLogoIndex[MAX_PATH];

    SHORT   sFlexRead, sBMFlag = 0;

    FLEXMEM PrevFlexMem[TRANS_FLEX_ADR_MAX];
    FLEXMEM NewFlexMem[TRANS_FLEX_ADR_MAX];

    if (TransTermCommLock(hWnd) < 0) {
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_LOCK_KEYBOARD);
        return TRANS_TERM_COMM_ABORT_LOCK;
    }

    sStatTerm = IspCheckAsMaster();   /* check terminal master/satellite */
    switch (sStatTerm) {
    case ISP_WORK_AS_MASTER:
        if (getTermByTerm() >= TRANS_TERM_MASTER_NUM) {
                                                   /* user select sattelite */
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_MASTER);
            TransTermCommUnLock(hWnd);
            return TRANS_TERM_COMM_ABORT_WORK_AS;
        }
        break;

    case ISP_WORK_AS_BMASTER:
/* --- V1.0.5
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_BMASTER);
        TransTermCommUnLock(hWnd);
        return TRANS_TERM_COMM_ABORT_WORK_AS;
--- */
        sBMFlag = 1;
        break;

    case ISP_WORK_AS_SATELLITE:
        if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {
                                                      /* user select master */
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_SATELLITE);
            TransTermCommUnLock(hWnd);
            return TRANS_TERM_COMM_ABORT_WORK_AS;
        }
        break;

    default:
        TransTermCommUnLock(hWnd);
        return TRANS_TERM_COMM_ABORT_WORK_AS;
    }

    if (sBMFlag != 0) {
        if (getTransFwTransTerm() & TRANS_ACCESS_PROG) {               /* user selected */
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_BMASTER);

            sStatus = TransTermSendParaReset(hWnd, 0);
            TransAbortDlgMessage(hWnd, IDS_TRANS_SEND_PROG);
            sStatus = TransTermSendParaClass(hWnd, CLASS_PARAFSC);  /* P3 */
            TransAbortDlgCalcParaMsg(hWnd, 8, TRANS_TERM_COMM_NO_PROG);
            if (sStatus < 0) {
                TransTermCommUnLock(hWnd);
                return sStatus;
            }
            /* send PLU Key Data with FSC simultaniously, 1/22/98 */
            sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPLUNOMENU);  /* A4 */
            TransAbortDlgCalcParaMsg(hWnd, 16, TRANS_TERM_COMM_NO_PROG);
            if (sStatus < 0) {
                TransTermCommUnLock(hWnd);
                return sStatus;
            }
            sStatus = TransTermSendParaClass(hWnd, CLASS_PARADEPTNOMENU);/* A115,    Saratoga */
            TransAbortDlgCalcParaMsg(hWnd, 25, TRANS_TERM_COMM_NO_PROG);
        }
		/* ----- send Layout Files ESMITH LAYOUT ---- */
		if ((getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) ||
			(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD)||
			(getTransFwTransTerm() & TRANS_ACCESS_LOGO)) {

			sMode = IspCheckModeOnTerm();

			if( (sMode == UIE_POSITION2) || (sMode == UIE_POSITION3)){
				if (getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) {
					sStatus = TransTermSendFile(hWnd, szTransTouchLAY, TRANS_ACCESS_LAYTOUCH, dwFileSize, IDS_TRANS_SEND_TOUCHLAY);
//					RmtReloadLayout();
				}
				if (getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD) {
					sStatus = TransTermSendFile(hWnd, szTransKeyboardLAY, TRANS_ACCESS_LAYKEYBRD, dwKeybrdFSize, IDS_TRANS_SEND_KEYBRDLAY);

//					RmtReloadLayout();
				}
				if (getTransFwTransTerm() & TRANS_ACCESS_LOGO){
					sStatus = TransTermSendFile(hWnd, szTransReceiptLogo, TRANS_ACCESS_LOGO, dwLogoFSize, IDS_TRANS_SEND_LOGO);
				}
			}	else {
				sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
			}
		}

		// Reload the layouts if we downloaded a new layout
		// or if we possibly changed P97 parameter specifying a
		// layout file change.
		if ((getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) ||
			(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD)||
			(bfPepReloadLayout))
		{
					RmtReloadLayout();
		}

        TransTermCommUnLock(hWnd);
        return TRUE;
    }

    for (;;) {
        /* ----- read previous flex memory data ----- */
        CliParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)PrevFlexMem, sizeof(PrevFlexMem), 0, (USHORT *)&sFlexRead);

        /* ----- read new flex memory data ----- */
        ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)NewFlexMem, sizeof(NewFlexMem), 0, (USHORT *)&sFlexRead);

        /* ----- check total counter of 2170 ----- */
        if ((sStatus = TransTermSendCheckTotal(hWnd, sStatTerm, NewFlexMem, PrevFlexMem)) != TRUE) {
            break;
        }

        /* ----- send setup parameter ----- */
        if ((sStatus = TransTermSendProg(hWnd)) < 0) {
            break;
        }

        /* ----- send maintenance parameter ----- */
        if ((sStatus = TransTermSendAct(hWnd)) < 0) {
            break;
        }

        /* ----- send Dept. file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransDept, TRANS_ACCESS_DEPT, NewFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_DEPT)) < 0) {
            break;
        }

        /* ----- send PLU file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransPlu, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_PLU)) < 0) {
            break;
        }

        /* ----- send PLU Index file,   Saratoga ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransPluIndex, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_PLUINDEX)) < 0) {
            break;
        }

        /* ----- send PLU OEP Index file,   Saratoga ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransOep, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber,  IDS_TRANS_SEND_OEP)) < 0) {
            break;
        }


//		RmtReloadOEPGroups();

        /* ----- send server file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransWaiter, TRANS_ACCESS_WAITER, NewFlexMem[FLEX_WT_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_WAITER)) < 0) {
            break;
        }

        if ((getTransFwTransTerm() & TRANS_ACCESS_WAITER) &&   /* user selected */
            (sStatTerm != ISP_WORK_AS_SATELLITE) && /* terminal is master */
            (NewFlexMem[FLEX_WT_ADR - 1].ulRecordNumber != 0L)) {  /*NCR2172*/
/*           SerTtlWaiTotalCheck();   *//* Check Waiter Total */
        }

        /* ----- send cashier file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransCashier, TRANS_ACCESS_CASHIER, NewFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_CASHIER)) < 0) {
            break;
        }
		// send Fingerprint DB (using cashier access flag)
		// doesn't exist in the NewFlexMem table, so passing a blocksize of 1 to indicate the file has contents to transfer
#if 0
		if ((sStatus = TransTermSendFile(hWnd, szTransFPDB, TRANS_ACCESS_CASHIER, 1, IDS_TRANS_SEND_FPDB)) < 0){
			break;
		}
#else
        // continue regardless of whether Cashier fingerprint file transfer worked or not.
        sStatus = TransTermSendFile(hWnd, szTransFPDB, TRANS_ACCESS_CASHIER, 1, IDS_TRANS_SEND_FPDB);
#endif
        if ((getTransFwTransTerm() & TRANS_ACCESS_CASHIER) &&   /* user selected */
            (sStatTerm != ISP_WORK_AS_SATELLITE) && /* terminal is master */
            (NewFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber != 0L)) { /*NCR2172*/
            SerTtlCasTotalCheck();    /* Check Cashier Total */
        }


        /* ----- send Combination Coupon file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransCpn, TRANS_ACCESS_CPN, NewFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_CPN)) < 0) {
            break;
        }

        /* ----- send Control String file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransCstr, TRANS_ACCESS_CSTR, NewFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_CSTR)) < 0) {
            break;
        }

        /* ----- send Programmable Report file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransProgRpt, TRANS_ACCESS_RPT, NewFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_RPT)) < 0) {
            break;
        }

        /* ----- send PPI file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransPPI, TRANS_ACCESS_PPI, NewFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_PPI)) < 0) {
            break;
        }

        /* ----- send ETK file ----- */
        if ((sStatus = TransTermSendFile(hWnd, szTransEtk, TRANS_ACCESS_ETK, NewFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_ETK)) < 0) {
            break;
        }

        /* ----- send MLD file, V3.3  ----- */
        sStatus = TransTermSendFile(hWnd, szTransMLD, TRANS_ACCESS_MLD, NewFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber, IDS_TRANS_SEND_MLD);

        /* ----- send RSN Reason Code mnemonics file, V3.3  ----- */
        sStatus = TransTermSendFile(hWnd, szTransRSN, TRANS_ACCESS_MLD, 400, IDS_TRANS_SEND_MLD);

		if(getTransFwTransTerm() & TRANS_ACCESS_ICON)
		{
			sStatus = TransTermSendFileDir(hWnd,szCommIconSaveName, TRANS_ACCESS_ICON,IDS_TRANS_SEND_ICON);
		}
		/* ----- send Layout Files, ESMITH LAYOUT ---- */
		if ((getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) ||
			(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD)||
			(getTransFwTransTerm() & TRANS_ACCESS_LOGO)) {

			sMode = IspCheckModeOnTerm();

			if( (sMode == UIE_POSITION2) || (sMode == UIE_POSITION3)){
				if (getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) {
					sStatus = TransTermSendFile(hWnd, szTransTouchLAY, TRANS_ACCESS_LAYTOUCH, dwFileSize, IDS_TRANS_SEND_TOUCHLAY);
//					RmtReloadLayout();
				}
				if (getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD) {
					sStatus = TransTermSendFile(hWnd, szTransKeyboardLAY, TRANS_ACCESS_LAYKEYBRD, dwKeybrdFSize, IDS_TRANS_SEND_KEYBRDLAY);
//					RmtReloadLayout();
				}
				if (getTransFwTransTerm() & TRANS_ACCESS_LOGO)
				{
					sStatus = TransTermSendFile(hWnd, szTransReceiptLogo, TRANS_ACCESS_LOGO, dwLogoFSize, IDS_TRANS_SEND_LOGO);
					
					sStatus = TransTermSendFile(hWnd, szTransLogoIndex, TRANS_ACCESS_LOGO, dwLogoIndexFSize, IDS_TRANS_SEND_LOGO);
					for(i = 1;i < 8;i++)
					{
						szPathName[0] = 0;
						wsPepf(szTempLogoIndex,szTransLogoFile, i + 1);
						wcsncat(szPathName, szTempLogoIndex, 10 - 1);

						sStatus = TransTermSendFile(hWnd, szPathName, TRANS_ACCESS_LOGO, dwLogoTotalFSize, IDS_TRANS_SEND_LOGO);
					}
				}
				}else {
				sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
			}
		}

		// Reload the layouts if we downloaded a new layout
		// or if we possibly changed P97 parameter specifying a
		// layout file change or if we downloaded PLU changes
		if ((getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) ||
			(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD)||
			(getTransFwTransTerm() & TRANS_ACCESS_PLU)||
			(bfPepReloadLayout))
		{
			RmtReloadLayout();
		}

        if ( sStatus < 0 ) {
            break;
        }
        if ((getTransFwTransTerm() & TRANS_ACCESS_ETK) &&   /* user selected */
            (sStatTerm != ISP_WORK_AS_SATELLITE) &&  /* terminal is master */
            (NewFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber != 0L)) { /*NCR2172*/
            sStatus = SerEtkCreateTotalIndex();    /* Create Total file Index */
            if ( sStatus < 0 ) {
                sStatus = FALSE;
            } else {
                sStatus = TRUE;
            }
        }
        break;
    }

    if (sStatus >= 0) {
        TransTermCommBroadcast(hWnd);       /* Add R3.1 */
    }

    TransTermCommUnLock(hWnd);

    return sStatus;
}

/*
*===========================================================================
** Synopsis:    short TransTermSendCheckTotal(HWND hWnd,
*                                             SHORT sStatTerm,
*                                             void *pNewFlexMem,
*                                             void *pPrevFlexMem)
*     Input:    hWnd         - handle of parent window
*               sStatTerm    - terminal type status
*               pNewFlexMem  - new flex memory data
*               pPrevFlexMem - reveious flex memory data
*
** Return:      TRUE - successful
*               0 <    unsuccessful
*
** Description: This function checks total counter of 2170.
*===========================================================================
*/
short TransTermSendCheckTotal(HWND hWnd,
                              SHORT sStatTerm,
                              void *pNewFlexMem,
                              void *pPrevFlexMem)
{
    SHORT       sStatus;
    SHORT       fsCheckTotals;
    FLEXMEM     *pNew  = pNewFlexMem;
    FLEXMEM     *pPrev = pPrevFlexMem;
    WCHAR        szWork[TRANS_MAX_MESSAGE_LEN];
    WCHAR        szMsg[TRANS_MAX_MESSAGE_LEN];
    WCHAR        szCap[TRANS_MAX_MESSAGE_LEN];
	WCHAR		 szFile[TRANS_MAX_MESSAGE_LEN];
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    /* ----- initialize status flag ----- */
    sStatus = TRUE;

    /* ----- check department data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_DEPT)) ) &&
        (((pNew + FLEX_DEPT_ADR - 1)->ulRecordNumber != (pPrev + FLEX_DEPT_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_DEPT_ADR - 1)->uchPTDFlag != (pPrev + FLEX_DEPT_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_DEPT, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_DEPT_ADR - 1)->ulRecordNumber, (pPrev + FLEX_DEPT_ADR - 1)->uchPTDFlag);

        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- Check PLU data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_PLU)) ) &&
        (((pNew + FLEX_PLU_ADR - 1)->ulRecordNumber != (pPrev + FLEX_PLU_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_PLU_ADR - 1)->uchPTDFlag != (pPrev + FLEX_PLU_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_PLU, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_PLU_ADR - 1)->ulRecordNumber, (pPrev + FLEX_PLU_ADR - 1)->uchPTDFlag);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- Check server data changing ----- */
    if ((( !(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
        (!(getTransFwTransTerm() & TRANS_ACCESS_WAITER)))  &&
        (sStatTerm != ISP_WORK_AS_SATELLITE) &&
        (((pNew + FLEX_WT_ADR - 1)->ulRecordNumber != (pPrev + FLEX_WT_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_WT_ADR - 1)->uchPTDFlag != (pPrev + FLEX_WT_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_WAITER, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_WT_ADR - 1)->ulRecordNumber, (pPrev + FLEX_WT_ADR - 1)->uchPTDFlag);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- Check cashier data changing ----- */
    if ((( !(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
        (!(getTransFwTransTerm() & TRANS_ACCESS_CASHIER)) ) &&
        (sStatTerm != ISP_WORK_AS_SATELLITE) &&
        ((pNew + FLEX_CAS_ADR - 1)->ulRecordNumber != (pPrev + FLEX_CAS_ADR - 1)->ulRecordNumber)) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_CASHIER, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_CAS_ADR - 1)->ulRecordNumber);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }
    /* ----- Check ETK data changing ----- */
    if ((!(getTransFwTransTerm() & TRANS_ACCESS_ETK)) &&
        (sStatTerm != ISP_WORK_AS_SATELLITE) &&
        ((pNew + FLEX_ETK_ADR - 1)->ulRecordNumber != (pPrev + FLEX_ETK_ADR - 1)->ulRecordNumber)) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_ETK, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_ETK_ADR - 1)->ulRecordNumber);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- check Combination Coupon data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_CPN)) ) &&
        (((pNew + FLEX_CPN_ADR - 1)->ulRecordNumber != (pPrev + FLEX_CPN_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_CPN_ADR - 1)->uchPTDFlag != (pPrev + FLEX_CPN_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_CPN, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_CPN_ADR - 1)->ulRecordNumber, (pPrev + FLEX_CPN_ADR - 1)->uchPTDFlag);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }
    /* ----- check Control String data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_CSTR)) ) &&
        (((pNew + FLEX_CSTRING_ADR - 1)->ulRecordNumber != (pPrev + FLEX_CSTRING_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_CSTRING_ADR - 1)->uchPTDFlag != (pPrev + FLEX_CSTRING_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_CSTR, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_CSTRING_ADR - 1)->ulRecordNumber, (pPrev + FLEX_CSTRING_ADR - 1)->uchPTDFlag);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- check Programmable Report data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_RPT)) ) &&
         ((pNew + FLEX_PROGRPT_ADR - 1)->ulRecordNumber != (pPrev + FLEX_PROGRPT_ADR - 1)->ulRecordNumber)) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_RPT, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_PROGRPT_ADR - 1)->ulRecordNumber);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- check PPI data changing ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_PPI)) ) &&
         ((pNew + FLEX_PPI_ADR - 1)->ulRecordNumber != (pPrev + FLEX_PPI_ADR - 1)->ulRecordNumber)) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_PPI, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_PPI_ADR - 1)->ulRecordNumber);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- check MLD data changing, V3.3 ----- */
    if (( (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) ||
          (!(getTransFwTransTerm() & TRANS_ACCESS_MLD)) ) &&
         ((pNew + FLEX_MLD_ADR - 1)->ulRecordNumber != (pPrev + FLEX_MLD_ADR - 1)->ulRecordNumber)) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_MLD, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_MLD_ADR - 1)->ulRecordNumber);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBoxPopUp(hWnd, szWork, szCap, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        sStatus = TRANS_TERM_COMM_ABORT_SYSTEM;
    }

    /* ----- Check guest check data changing ----- */
    if (((pPrev + FLEX_GC_ADR - 1)->ulRecordNumber != 0L) &&
        (((pNew + FLEX_GC_ADR - 1)->ulRecordNumber != (pPrev + FLEX_GC_ADR - 1)->ulRecordNumber) ||
         ((pNew + FLEX_GC_ADR - 1)->uchPTDFlag != (pPrev + FLEX_GC_ADR - 1)->uchPTDFlag))) {
        MessageBeep(MB_ICONEXCLAMATION);
        LoadString (hResourceDll, IDS_TRANS_CHG_GC, szMsg, PEP_STRING_LEN_MAC(szMsg));
        wsPepf(szWork, szMsg, (pPrev + FLEX_GC_ADR - 1)->ulRecordNumber, (pPrev + FLEX_GC_ADR - 1)->uchPTDFlag);
        LoadString (hResourceDll, IDS_TRANS_NAME, szCap, PEP_STRING_LEN_MAC(szCap));
        if (MessageBoxPopUp(hWnd, szWork, szCap, MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL) == IDNO) {
            return TRANS_TERM_COMM_ABORT_USER;
        } else {
            SetCursor(LoadCursor(NULL, IDC_WAIT));      /* set wait cursor  */
            sStatus = TRUE;
        }
    }
    if (sStatus < 0) {
        return (sStatus);
    }

    /* ----- get total counter status of 2170 ----- */
    fsCheckTotals = IspCheckTotals(ISP_TTL_DEPARTMENT |
                                   ISP_TTL_PLU |
                                   ISP_TTL_WAITER |
                                   ISP_TTL_ETK |
                                   ISP_TTL_CASHIER |
                                   ISP_TTL_CPN);

	LoadString(hResourceDll, IDS_PEP_FILE_DEPT, szFile, TRANS_MAX_MESSAGE_LEN);

    if ((fsCheckTotals & ISP_TTL_DEPARTMENT) && (getTransFwTransTerm() & TRANS_ACCESS_DEPT)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
								IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }

	LoadString(hResourceDll, IDS_PEP_FILE_PLU, szFile, TRANS_MAX_MESSAGE_LEN);
    if ((fsCheckTotals & ISP_TTL_PLU) && (getTransFwTransTerm() & TRANS_ACCESS_PLU)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                               IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }


	LoadString(hResourceDll, IDS_PEP_FILE_SER, szFile, TRANS_MAX_MESSAGE_LEN);
    if ((fsCheckTotals & ISP_TTL_WAITER) && (getTransFwTransTerm() & TRANS_ACCESS_WAITER)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                               IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }

	LoadString(hResourceDll, IDS_PEP_FILE_CAS, szFile, TRANS_MAX_MESSAGE_LEN);
    if ((fsCheckTotals & ISP_TTL_CASHIER) && (getTransFwTransTerm() & TRANS_ACCESS_CASHIER)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                               IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }


	LoadString(hResourceDll, IDS_PEP_FILE_ETK, szFile, TRANS_MAX_MESSAGE_LEN);
    if ((fsCheckTotals & ISP_TTL_ETK) && (getTransFwTransTerm() & TRANS_ACCESS_ETK)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                               IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }

	LoadString(hResourceDll, IDS_PEP_FILE_CPN, szFile, TRANS_MAX_MESSAGE_LEN);
    if ((fsCheckTotals & ISP_TTL_CPN) && (getTransFwTransTerm() & TRANS_ACCESS_CPN)) {
        sStatus = (SHORT)TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                               IDS_TRANS_NAME, IDS_TRANS_WAR_CHKREPORT, szFile);

        if ( sStatus ==  IDCANCEL ) {
            sStatus = FALSE;
        } else {
            sStatus = TRUE;
        }
    }
    /* ----- check request reset report ----- */
    if (sStatus == FALSE) {
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_REPORT);
        sStatus = TRANS_TERM_COMM_ABORT_USER;
    }
    return (sStatus);
}

/*
*===========================================================================
** Synopsis:    short TransTermSendProg(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= success
*               0 <  unseccess
*
** Description: As for this function, each class transmits the parameter to
*               2170.
*===========================================================================
*/
short TransTermSendProg(HWND hWnd)
{
    short   sStatus;
    WORD    wI = 0;
    ULONG   ulI = 0;

    if (getTransFwTransTerm() & TRANS_ACCESS_PROG) {               /* user selected */
        sStatus = TransTermSendParaReset(hWnd, wI++);
        if ((sStatus = TransTermSendParaFlex(hWnd, CLASS_PARAFLEXMEM)) < 0) {/* P2 */
            TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_MEMORY);
            return (sStatus);
        }
        TransAbortDlgMessage(hWnd, IDS_TRANS_SEND_PROG);
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAMDC);  /* P1 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAFSC);  /* P3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        /* send PLU Key Data with FSC simultaniously, 1/22/98 */
        if (!(getTransFwTransTerm() & TRANS_ACCESS_ACT)) {  /* if user not select */
            sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPLUNOMENU);  /* A4 */
            if (sStatus < 0) {
                return (sStatus);
            }
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARADEPTNOMENU);/* A115,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASECURITYNO); /* P6 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASUPLEVEL); /* P8 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAACTCODESEC); /* P9 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATRANSHALO); /* P10 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPRESETAMOUNT);/* P15 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAHOURLYTIME);/* P17 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASLIPFEEDCTL);/* P18 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATRANSMNEMO);/* P20 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARALEADTHRU);  /* P21 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAREPORTNAME);/* P22 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASPECIALMNEMO);/* P23 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAADJMNEMO); /* P46 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPRTMODI); /* P47 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAMAJORDEPT); /* P48 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAAUTOALTKITCH);/* P49 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASHRPRT);   /* P50 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAKDSIP);    /* P51,   Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAHOTELID);   /* P54 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARACHAR24);   /* P57 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATTLKEYTYP); /* P60 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATTLKEYCTL); /* P61 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATEND); /* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATERMINALINFO); /* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_TENDERKEY_INFO); /* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

		sStatus = TransTermReceiveParaClass(hWnd, CLASS_FILEVERSION_INFO);/* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

		sStatus = TransTermSendParaClass(hWnd, CLASS_PARAAUTOCPN);/* P67 ACC*/
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

		sStatus = TransTermSendParaClass(hWnd, CLASS_PARATTLKEYORDERDEC);/* P67 ACC*/
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }          
		sStatus = TransTermSendParaClass(hWnd, CLASS_PARAREASONCODE);/* P72 ACC*/
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }          
		/* note that TRANS_TERM_COMM_NO_PROG in Tratcomm.h needs to be updated  if you add anymore program classes
		to be sent to the terminal.  This is what causes the percentage to to go over 100% when being sent*/

        return (sStatus);
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermSendAct(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= success
*               0 <  unseccess
*
** Description: As for this function, each class transmits the parameter to
*               2170.
*===========================================================================
*/
short TransTermSendAct(HWND hWnd)
{
    short   sStatus;
    WORD    wI = 0;

    if (getTransFwTransTerm() & TRANS_ACCESS_ACT) {               /* user selected */
        TransAbortDlgMessage(hWnd, IDS_TRANS_SEND_ACT);
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);

        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPLUNOMENU);  /* A4 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        /* send FSC Data with PLU Key simultaniously, 1/22/98 */
        if (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) { /* user not selected */
            sStatus = TransTermSendParaClass(hWnd, CLASS_PARAFSC);  /* P3 */
            if (sStatus < 0) {
                return (sStatus);
            }
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARACTLTBLMENU); /* A5 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARACASHABASSIGN);/* A7 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAROUNDTBL); /* A84 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARADISCTBL);  /* A86 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASOFTCHK);  /* A87 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPROMOTION);/* A88 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARACURRENCYTBL);/* A89 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATARE);     /* A111 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAMENUPLUTBL);/* A116 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATAXTBL1);  /* A124 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARATAXRATETBL);/* A127 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAPIGRULE);    /* A130 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARALABORCOST); /* A133 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARADISPPARA); /* A137 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASERVICETIME); /* A154 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAOEPTBL);    /* A160 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAFXDRIVE);    /* A162 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARARESTRICTION);  /* A170,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARABOUNDAGE); /* A208,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARACOLORPALETTE); /* A209,    Saratoga */ //CSMALL-colorpalette
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermSendParaClass(hWnd, CLASS_PARAMISCPARA);/* A128,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }

        sStatus = TransTermSendParaClass(hWnd, CLASS_PARASTOREFORWARD);/* A128,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }

        return (sStatus);
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermSendParaClass(HWND hWnd, UCHAR uchClass)
*     Input:    hWnd     - handle of parent window
*               uchClass - class of parameter
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the parameter of the specified class
*               to 2170.
*===========================================================================
*/
short TransTermSendParaClass(HWND hWnd, UCHAR uchClass)
{
    SHORT   sRead;
    USHORT  usWrite;
    USHORT  usStart = 0;
    SHORT   sStatus;
    HGLOBAL hBuffer;
    VOID FAR *pBuffer;

    /* ----- check abort by user ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
    if (hBuffer) {
        /* ----- lock global memory ----- */
        pBuffer = GlobalLock(hBuffer);

        do {
            /* ----- read from the work file ----- */
            ParaAllRead(uchClass, pBuffer, TRANS_MEMORY_SIZE_TERM, usStart, (USHORT *)&sRead);

            /* ----- send to 2170 ----- */
            sStatus = CliParaAllWrite(uchClass, pBuffer, (USHORT)sRead, usStart, &usWrite);

            /* ----- seek start addresss ----- */
            usStart += TRANS_MEMORY_SIZE_TERM;

        } while (usWrite == TRANS_MEMORY_SIZE_TERM);

        /* ----- release global memory ----- */
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return sStatus;
    } else {                                /* not enough memory */
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_MEMORY);
        return TRANS_TERM_COMM_ABORT_SYSTEM;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermSendParaReset()
*     Input:    hWnd     - handle of parent window
*               wCount   - counter
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the reset parameter to 2170.
*===========================================================================
*/
short TransTermSendParaReset(HWND hWnd, WORD wCount)
{
    SHORT   sRead;
    USHORT  usWrite;
    SHORT   sStatus;
    FLEXMEM aBuffer[TRANS_FLEX_ADR_MAX];

    /* ----- check abort by user ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- read from the work file ----- */
    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)aBuffer, (sizeof(FLEXMEM) * MAX_FLXMEM_SIZE), 0, (USHORT *)&sRead);

    if (getTransFwTransTerm() & TRANS_ACCESS_DEPT) {
        memset((LPSTR)&aBuffer[FLEX_DEPT_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_PLU) {
        memset((LPSTR)&aBuffer[FLEX_PLU_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_WAITER) {
        memset((LPSTR)&aBuffer[FLEX_WT_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_CASHIER) {
        memset((LPSTR)&aBuffer[FLEX_CAS_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_ETK) {
        memset((LPSTR)&aBuffer[FLEX_ETK_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_CPN) {
        memset((LPSTR)&aBuffer[FLEX_CPN_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_CSTR) {
        memset((LPSTR)&aBuffer[FLEX_CSTRING_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_RPT) {
        memset((LPSTR)&aBuffer[FLEX_PROGRPT_ADR - 1], 0, sizeof(FLEXMEM));
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_CSTR) {
        memset((LPSTR)&aBuffer[FLEX_PPI_ADR - 1], 0, sizeof(FLEXMEM));
    }

    sStatus = CliParaAllWrite(CLASS_PARAFLEXMEM, (UCHAR *)aBuffer, (USHORT)sRead, 0, &usWrite);

    /* ----- display calculated send rate ----- */
    TransAbortDlgCalcParaMsg(hWnd, wCount, TRANS_TERM_COMM_NO_PROG);

    return (sStatus);
}

/*
*===========================================================================
** Synopsis:    short TransTermSendParaFlex(HWND hWnd, UCHAR uchClass)
*     Input:    hWnd     - handle of parent window
*               uchClass - class of parameter
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the parameter of the specified class
*               to 2170.
*===========================================================================
*/
short TransTermSendParaFlex(HWND hWnd, UCHAR uchClass)
{
    SHORT   sRead;
    USHORT  usWrite;
    SHORT   sStatus;
    HGLOBAL hBuffer;
    VOID FAR *pBuffer;

    /* ----- check abort by user ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
    if (hBuffer) {
        /* ----- lock global memory ----- */
        pBuffer = GlobalLock(hBuffer);

        /* ----- read from the work file ----- */
        ParaAllRead(uchClass, pBuffer, TRANS_MEMORY_SIZE_TERM, 0, (USHORT *)&sRead);

        sStatus = CliParaAllWrite(uchClass, pBuffer, (USHORT)sRead, 0, &usWrite);

        /* ----- release global memory ----- */
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return sStatus;
    } else {                                /* not enough memory */
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_MEMORY);
        return TRANS_TERM_COMM_ABORT_SYSTEM;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermSendFile(HWND hWnd, char *szFile,
*                                       WORD wFlag, ULONG ulBlockSize,
*                                       WORD wMessage)
*     Input:    hWnd       - handle of parent window
*               szFile     - file name
*               wFlag      - transmission flag
*               sBlockSize - block size
*               wMessage   - ID of message string
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the specified file to 2170.
*===========================================================================
*/
short TransTermSendFile(HWND hWnd, WCHAR *szFile, WORD wFlag,
                        ULONG ulBlockSize, WORD wMessage)
{
    short   sStatus;                /* return status */
    SHORT   sLocalFile;             /* handle of local file */
    SHORT   sRmtFile;               /* handle of remote file */
    USHORT  usLocalRead;            /* read length */
    SHORT   sRmtWrite;              /* write length */
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    short   nCount = 0;

    /* ----- check user by abort ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- check teriminal type ----- */
    if (getTermByTerm() >= TRANS_TERM_MASTER_NUM) {
                                                /* terminal is sattelite */
        switch (wFlag) {
        case TRANS_ACCESS_WAITER:
        case TRANS_ACCESS_CASHIER:
        case TRANS_ACCESS_ETK:
        case TRANS_ACCESS_RPT:
            return TRUE;
        default:
            break;
        }
    }

    /* ----- check transmission flag ----- */
    if (getTransFwTransTerm() & wFlag) {                        /* user selected */
        TransAbortDlgMessage(hWnd, wMessage);

        if (ulBlockSize) {
            /* ----- get global memory ----- */
            hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
            if (hBuffer) {
                /* ----- lock global memory ----- */
                pBuffer = GlobalLock(hBuffer);

                /* ----- Finalize mass memory module,   Saratoga ----- */
                PluFinalize();

                for (;;) {                      /* no loop */
                    /* ----- open work file ----- */
                    if ((sLocalFile = PifOpenFile(szFile, "or")) < 0) {
                        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                     IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_OPEN, szFile);
                        sStatus = TRANS_TERM_COMM_ABORT_LOCAL;
                        break;
                    }

                    /* ----- open remote file ----- */
                    if ((sRmtFile = RmtOpenFile(szFile, "ow")) < 0) {
#ifdef PC_DEBUG
                        TransMessage(hWnd,
                                     MB_ICONEXCLAMATION,
                                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                     IDS_TRANS_NAME,
                                     IDS_TRANS_ERR_RMT_FILE_OPEN,
                                     szFile);
#endif
                        PifCloseFile(sLocalFile);
                        sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                        break;
                    }

                    /* ----- transmission ----- */
                    for (;;) {
                        /* ----- display calculated send rate ----- */
                        TransAbortDlgCalcFileMsg(hWnd, wMessage, ulBlockSize, nCount++, NULL);
                        /* ----- read work file ----- */
                        usLocalRead = PifReadFile((USHORT)sLocalFile, pBuffer, TRANS_MEMORY_SIZE_TERM);
                        if (usLocalRead > 0) {          /* read successful */
                            /* ----- write remote file ----- */
                            if ((sRmtWrite = RmtWriteFile((USHORT)sRmtFile, pBuffer, usLocalRead)) < 0) {
                                sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                                break;
                            }

                            /* ----- check end of file ----- */
                            if (usLocalRead != TRANS_MEMORY_SIZE_TERM) {
                                sStatus = TRUE;
                                break;
                            }
                        } else if (usLocalRead == 0) {  /* end of file */
                            sStatus = TRUE;
                            break;
                        } else {
                            sStatus = TRANS_TERM_COMM_ABORT_LOCAL;
                            break;
                        }

                        /* ----- check user abort ----- */
                        if (TransAbortQueryStatus() == TRUE) {
                            sStatus = TRANS_TERM_COMM_ABORT_USER;
                            break;
                        }
                    }
                    /* ----- display finished send rate ----- */
                    if (sStatus == TRUE) {
                        TransAbortDlgFinMessage(hWnd);
                    }
                    /* ----- close files ----- */
                    RmtCloseFile(sRmtFile);
                    PifCloseFile(sLocalFile);
                    break;
                }

                /* ----- Initialize mass memory module, Saratoga ----- */
                PluInitialize();

                /* ----- release global memory */
                GlobalUnlock(hBuffer);
                GlobalFree(hBuffer);

                return sStatus;
            } else {                                    /* not enough memory */
                TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                             IDS_TRANS_NAME, IDS_TRANS_MEMORY);
                return TRANS_TERM_COMM_ABORT_SYSTEM;
            }
        }
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermSendFile(HWND hWnd, char *szFile,
*                                       WORD wFlag, ULONG ulBlockSize,
*                                       WORD wMessage)
*     Input:    hWnd       - handle of parent window
*               szFile     - file name
*               wFlag      - transmission flag
*               sBlockSize - block size
*               wMessage   - ID of message string
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function transmits the specified file to 2170.
*===========================================================================
*/
short TransTermSendFileDir(HWND hWnd, WCHAR *szDir, WORD wFlag,WORD wMessage)
{
    short   sStatus, i;                /* return status */
    SHORT   sLocalFile;             /* handle of local file */
    SHORT   sRmtFile;               /* handle of remote file */
    USHORT  usLocalRead;            /* read length */
    SHORT   sRmtWrite;              /* write length */
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    short   nCount = 0;
	WIN32_FIND_DATA myFoundFile;
	WCHAR myFileName[3][20] = {WIDE("*.gif"), WIDE("*.jpg"), WIDE("*.bmp")};
	HANDLE fileSearch;
	BOOL doit = TRUE;

    /* ----- check user by abort ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- check teriminal type ----- */
    if (getTermByTerm() >= TRANS_TERM_MASTER_NUM) {
                                                /* terminal is sattelite */
        switch (wFlag) {
        case TRANS_ACCESS_WAITER:
        case TRANS_ACCESS_CASHIER:
        case TRANS_ACCESS_ETK:
        case TRANS_ACCESS_RPT:
            return TRUE;
        default:
            break;
        }
    }
	if(!szDir[0])
	{
		TransTermSetIconDirectory(hWnd);
	}
	//we need to change to this directory to look for the files
	_wchdir(szDir);

	
    /* ----- check transmission flag ----- */
    if (getTransFwTransTerm() & wFlag) {                        /* user selected */
        TransAbortDlgMessage(hWnd, wMessage);
        /* ----- get global memory ----- */
        hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
            /* ----- lock global memory ----- */
        pBuffer = GlobalLock(hBuffer);

        /* ----- Finalize mass memory module,   Saratoga ----- */
        PluFinalize();
		for(i = 0; i < 3; i++)
		{
			doit = TRUE;
			fileSearch = FindFirstFile (myFileName[i], &myFoundFile); //find the first file
        
            while (doit && fileSearch != INVALID_HANDLE_VALUE)   {    
				nCount = 0;/* no loop */

                /* ----- open work file ----- */ 
                if ((sLocalFile = PifOpenDirFile(szDir,myFoundFile.cFileName, "or")) < 0) {
                    TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                 IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_OPEN, szDir);
                    sStatus = TRANS_TERM_COMM_ABORT_LOCAL;
                    break;
                }

                /* ----- open remote file ----- */
                if ((sRmtFile = RmtOpenFile(myFoundFile.cFileName, "owi")) < 0) {
#ifdef PC_DEBUG
                    TransMessage(hWnd,
                                 MB_ICONEXCLAMATION,
                                 MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                 IDS_TRANS_NAME,
                                 IDS_TRANS_ERR_RMT_FILE_OPEN,
                                 szDir);
#endif
					//okay, maybe it doesnt exist
					if(sRmtFile = RmtOpenFile(myFoundFile.cFileName, "nwi") < 0)
					{
						PifCloseFile(sLocalFile);
						sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
						break;
					}
                }

                /* ----- transmission ----- */
                for (;;) {
                    /* ----- display calculated send rate ----- */
                    TransAbortDlgCalcFileMsg(hWnd, wMessage, myFoundFile.nFileSizeLow, nCount++,myFoundFile.cFileName);
                    /* ----- read work file ----- */
                    usLocalRead = PifReadFile((USHORT)sLocalFile, pBuffer, TRANS_MEMORY_SIZE_TERM);
                    if (usLocalRead > 0) {          /* read successful */
                        /* ----- write remote file ----- */
                        if ((sRmtWrite = RmtWriteFile((USHORT)sRmtFile, pBuffer, usLocalRead)) < 0) {
                            sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                            break;
                        }

                        /* ----- check end of file ----- */
                        if (usLocalRead != TRANS_MEMORY_SIZE_TERM) {
                            sStatus = TRUE;
                            break;
                        }
                    } else if (usLocalRead == 0) {  /* end of file */
                        sStatus = TRUE;
                        break;
                    } else {
                        sStatus = TRANS_TERM_COMM_ABORT_LOCAL;
                        break;
                    }

                    /* ----- check user abort ----- */
                    if (TransAbortQueryStatus() == TRUE) {
                        sStatus = TRANS_TERM_COMM_ABORT_USER;
                        break;
                    }
                }

                /* ----- display finished send rate ----- */
                if (sStatus == TRUE) {
                    TransAbortDlgFinMessage(hWnd);
                }
                /* ----- close files ----- */
                RmtCloseFile(sRmtFile);
                PifCloseFile(sLocalFile);
				//find the next file, if there is not one, doit will be FALSE
				doit = FindNextFile (fileSearch, &myFoundFile);
                continue;
            }
		}
	}
    PluInitialize();

    /* ----- release global memory */
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);
	return TRUE;
}
/* ===== End of TRATSEND.C ===== */