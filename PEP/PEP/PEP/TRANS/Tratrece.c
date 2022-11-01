/*
* ---------------------------------------------------------------------------
* Title         :   Receive from Terminal
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATRECE.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
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
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Jan-22-98 : 01.00.08 : M.Ozawa    : Receive CLASS_PARAPLUNOMENU and CLASS_PARAFSC
*                                   : simultaniously, even if only SETUP Parameter
*                                   : is selected.
* Sep-08-98 : 03.03.00 : A.Mitsui   : Add V3.3
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

#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <r20_pcstbpar.h>
#include <r20_rmt.h>
#include <r20_pif.h>

#include "pep.h"
#include "pepflag.h"
#include "trans.h"
#include "transl.h"
#include "traterm.h"
#include "tratcomm.h"
#include "layout.h"
#include "pepcomm.h"
#include "file.h"
#include <direct.h>


DWORD dwFileSize;
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
** Synopsis:    short TransTermReceive(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function displays the message and starts reception.
*===========================================================================
*/
short TransTermReceive(HWND hWnd)
{
    short   sStatus;
    SHORT   sFlexRead;
    FLEXMEM FlexMem[TRANS_FLEX_ADR_MAX];

    if (TransTermReceiveWarning(hWnd) == TRANS_TERM_COMM_ABORT_USER) {
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
                sStatus = TransTermReceiveData(TransAbort.hDlg);
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
            /* ----- read new flex memory data ----- */
            ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)FlexMem, sizeof(FlexMem), 0, (USHORT *)&sFlexRead);

            if ((getTransFwTransTerm() & TRANS_ACCESS_RPT) && (getTermByTerm() < TRANS_TERM_MASTER_NUM)
    /*NCR2172*/
    /*            && (FlexMem[FLEX_PROGRPT_ADR - 1].usRecordNumber != 0)) {*/
                && (FlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber != 0L)) {
                if ( TransConvertToIniFile(TransAbort.hDlg) == FALSE) {
                     TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_CONVERT);
                    sStatus = TRANS_TERM_COMM_ABORT_USER;
                }
            }
        }
        if (sStatus > 0) {              /* check sending status */
            if (lTransTester == 1) {
                TransMessage(TransAbort.hDlg, MB_ICONASTERISK, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_SUC_TRANSFER);
            }
            TransResetFlexMem();
            TransResetData();
            PepSetModFlag(hwndPepMain, PEP_MF_OPEN, 0);
            sStatus = TRUE;
        } else {
            TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_RECEIVE);
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
** Synopsis:    short TransTermReceiveLan(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function displays the message and starts reception. Saratoga
*===========================================================================
*/
short TransTermReceiveLan(HWND hWnd)
{
    short   sStatus;
    SHORT   sFlexRead;
    FLEXMEM FlexMem[TRANS_FLEX_ADR_MAX];
	WCHAR		szPathName[MAX_PATH];
	HANDLE	handleRead, handleWrite;
	ULONG	bytesRead, actualBytes;

    if (TransTermReceiveWarning(hWnd) == TRANS_TERM_COMM_ABORT_USER) {
        return FALSE;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));                      /* set wait cursor  */

    if (TransTermCommLanOpen(hWnd) == TRUE) {
        TransAbortCreateDlg(hWnd);          /* create abort dialog box */

        if ((sStatus = TransTermCommLanLogon(TransAbort.hDlg)) == TRUE) {
        /* if ((sStatus = TransTermCommLogon(TransAbort.hDlg)) == TRUE) { */
            if (TransTermPass(TransAbort.hDlg) == TRUE) {
                sStatus = TransTermReceiveData(TransAbort.hDlg);
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
        /* ----- read new flex memory data ----- */
            ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)FlexMem, sizeof(FlexMem), 0, (USHORT *)&sFlexRead);

            if ((getTransFwTransTerm() & TRANS_ACCESS_RPT) && (getTermByTerm() < TRANS_TERM_MASTER_NUM)
                && (FlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber != 0L)) {
                if ( TransConvertToIniFile(TransAbort.hDlg) == FALSE) {
                     TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_CONVERT);
                    sStatus = TRANS_TERM_COMM_ABORT_USER;
                }
            }

			if(getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH) {
				if(szCommTouchLayoutSaveName[0] == _T('\0')) {
					if(!LayoutFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommTouchLayoutSaveName,PEP_MF_LAYTOUCH)) {
						sStatus = FALSE;
					}
				}

				if(sStatus > 0) {
#if 1
					GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
					GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
					szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path

					wcscat(szPathName, WIDE("\\"));
#endif
					wcsncat(szPathName, szTransTouchLAY, 10 - 1);
					handleRead = CreateFilePep(&szPathName[0],GENERIC_READ, FILE_SHARE_READ, NULL,
										OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
						handleWrite= CreateFilePep(&szCommTouchLayoutSaveName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
											OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
						dwFileSize = GetFileSize(handleRead, NULL);
					{
						BYTE *uchBuffer = (BYTE *) _alloca (dwFileSize);
						ReadFile(handleRead, uchBuffer, dwFileSize, &bytesRead, NULL);
						WriteFile(handleWrite, uchBuffer, dwFileSize, &actualBytes, NULL);
					}

					CloseHandle(handleWrite);
					CloseHandle(handleRead);
				}
			}

			if(getTransFwTransTerm() & TRANS_ACCESS_LOGO) {
				if(szCommLogoSaveName[0] == _T('\0')) {
					if(!LogoFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommLogoSaveName,PEP_MF_LOGO)) {
						sStatus = FALSE;
					}
				}

				if(sStatus > 0) {
#if 1
					GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
					GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
					szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path

					wcscat(szPathName, WIDE("\\"));
#endif
					wcsncat(szPathName, szTransReceiptLogo, 10 - 1);
					handleRead = CreateFilePep(&szPathName[0],GENERIC_READ, FILE_SHARE_READ, NULL,
										OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
					handleWrite= CreateFilePep(&szCommLogoSaveName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
											OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
					dwFileSize = GetFileSize(handleRead, NULL);
					{
						BYTE *uchBuffer = (BYTE *) _alloca (dwFileSize);
						ReadFile(handleRead, uchBuffer, dwFileSize, &bytesRead, NULL);
						WriteFile(handleWrite, uchBuffer, dwFileSize, &actualBytes, NULL);
					}

					CloseHandle(handleWrite);
					CloseHandle(handleRead);
				}
			}

			if(sStatus > 0) {
				if(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD) {
					if(szCommKeyboardLayoutSaveName[0] == _T('\0')) {
						if(!LayoutFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd, szCommKeyboardLayoutSaveName,PEP_MF_LAYKEYBRD)) {
							sStatus = FALSE;
						}
					}

					if(sStatus > 0) {
#if 1
						GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
						GetPepModuleFileName(NULL, szPathName, sizeof(szPathName));
						szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path

						wcscat(szPathName, WIDE("\\"));
#endif
						wcsncat(szPathName, szTransKeyboardLAY, 10 - 1);
						handleRead = CreateFilePep(&szPathName[0],GENERIC_READ, FILE_SHARE_READ, NULL,
											OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
						handleWrite= CreateFilePep(&szCommKeyboardLayoutSaveName[0], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
												OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
						dwFileSize = GetFileSize(handleRead, NULL);
						{
							BYTE *uchBuffer = (BYTE *) _alloca (dwFileSize);
							ReadFile(handleRead, uchBuffer, dwFileSize, &bytesRead, NULL);
							WriteFile(handleWrite, uchBuffer, dwFileSize, &actualBytes, NULL);
						}

						CloseHandle(handleWrite);
						CloseHandle(handleRead);
					}
				}
			}
		}

        if (sStatus > 0) {              /* check sending status */
            if (lTransTester == 1) {
                TransMessage(TransAbort.hDlg, MB_ICONASTERISK, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_SUC_TRANSFER);
            }
            TransResetFlexMem();
            TransResetData();
            PepSetModFlag(hwndPepMain, PEP_MF_OPEN, 0);
            sStatus = TRUE;
        } else {
            TransMessage(TransAbort.hDlg, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_RECEIVE);
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
** Synopsis:    short TransTermReceiveWarning(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE             - continue
*               TRANS_TERM_COMM_ABORT_USER - abort by user
*
** Description:
*===========================================================================
*/
short TransTermReceiveWarning(HWND hWnd)
{
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
                        PEP_MF_MLD |
						PEP_MF_LAYTOUCH | //ESMITH LAYOUT
						PEP_MF_LAYKEYBRD |
						PEP_MF_LOGO |
						PEP_MF_ICON,
                        0) == TRUE) {
        if (TransMessage(hWnd, MB_ICONEXCLAMATION, MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_WAR_SAVE) == IDNO) {
            return TRANS_TERM_COMM_ABORT_USER;
        }
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermReceiveData(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function receives the parameter and each file to 2170.
*===========================================================================
*/
short TransTermReceiveData(HWND hWnd)
{
    short   sStatus;
    SHORT   sFlexRead;
    FLEXMEM PrevFlexMem[TRANS_FLEX_ADR_MAX];
    FLEXMEM NewFlexMem[TRANS_FLEX_ADR_MAX];
	ULONG   ulTouchFileSize, ulKeyBrdFileSize, ulLogoFileSize; //ESMITH LAYOUT

	if(getTransFwTransTerm() & TRANS_ACCESS_LAYTOUCH)
	{
		if( ! ((ulTouchFileSize = TransTermReceiveSizeOnTerm(szTransTouchLAY)) > 0))
		{
			return 0;
		}
	}

	if(getTransFwTransTerm() & TRANS_ACCESS_LAYKEYBRD)
	{
		if( ! ((ulKeyBrdFileSize = TransTermReceiveSizeOnTerm(szTransKeyboardLAY)) > 0))
		{
			return 0;
		}
	}

	if(getTransFwTransTerm() & TRANS_ACCESS_LOGO)
	{
		if( ! ((ulLogoFileSize = TransTermReceiveSizeOnTerm(szTransReceiptLogo)) > 0))
		{
			return 0;
		}
	}

    do {         // single iteration do to allow break if errors during actions below.
        /* ----- read previous flex memory data ----- */
        ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)PrevFlexMem, sizeof(PrevFlexMem), 0, (USHORT *)&sFlexRead);

        /* ----- receive parameter ----- */
        if ((sStatus = TransTermReceiveProg(hWnd)) < 0) {
            break;
        }

        /* ----- read new flex memory data ----- */
        ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)NewFlexMem, sizeof(NewFlexMem), 0, (USHORT *)&sFlexRead);

        /* ----- check and create each files ----- */
        TransCreateFiles(NewFlexMem, PrevFlexMem);

        /* ----- receive maintenance parameter ----- */
        if ((sStatus = TransTermReceiveAct(hWnd)) < 0) {
            break;
        }

        /* ----- receive Dept. file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransDept, TRANS_ACCESS_DEPT, NewFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_DEPT)) < 0) {
            break;
        }

        /* ----- receive PLU file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransPlu, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_PLU)) < 0) {
            break;
        }

        /* ----- receive PLU Index file,    Saratoga ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransPluIndex, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_PLUINDEX)) < 0) {
            break;
        }

        /* ----- receive PLU OEP Index file,    Saratoga ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransOep, TRANS_ACCESS_PLU, NewFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_OEP)) < 0) {
            break;
        }

        /* ----- receive server file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransWaiter, TRANS_ACCESS_WAITER, NewFlexMem[FLEX_WT_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_WAITER)) < 0) {
            break;
        }

        /* ----- receive cashier file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransCashier, TRANS_ACCESS_CASHIER, NewFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_CASHIER)) < 0) {
            break;
        }
		
		// recieve Fingerprint file
		// doesn't exist in the NewFlexMem table, so passing a blocksize of 1 to indicate the file has contents to transfer
		if((sStatus = TransTermReceiveFile(hWnd, szTransFPDB, TRANS_ACCESS_CASHIER,	1, IDS_TRANS_RECEIVE_FPDB)) < 0){
			break;
		}

        /* ----- receive ETK file ----- */
        if((sStatus = TransTermReceiveFile(hWnd, szTransEtk, TRANS_ACCESS_ETK, NewFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_ETK)) < 0) {
            break;
        }

        /* ----- receive Combination Coupon file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransCpn, TRANS_ACCESS_CPN, NewFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_CPN)) < 0) {
            break;
        }

        /* ----- receive Control String file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransCstr, TRANS_ACCESS_CSTR, NewFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_CSTR)) < 0) {
            break;
        }

        /* ----- receive Programmable Report file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransProgRpt, TRANS_ACCESS_RPT, NewFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_RPT)) < 0) {
            break;
        }

		/* ----- receive Touch Layout ------- */
		if ((sStatus = TransTermReceiveFile(hWnd, szTransTouchLAY, TRANS_ACCESS_LAYTOUCH, ulTouchFileSize, IDS_TRANS_RECEIVE_TOUCH)) < 0) {
            break;
        }

		/* ----- receive Receipt Logo ------- */
		if ((sStatus = TransTermReceiveFile(hWnd, szTransReceiptLogo, TRANS_ACCESS_LOGO, ulLogoFileSize, IDS_TRANS_RECEIVE_LOGO)) < 0) {
			break;
		}

		/* ----- receive Keyboard Layout ------- */
		if ((sStatus = TransTermReceiveFile(hWnd, szTransKeyboardLAY, TRANS_ACCESS_LAYKEYBRD, ulKeyBrdFileSize, IDS_TRANS_RECEIVE_KEYBRD)) < 0) {
            break;
        }

        /* ----- receive PPI file ----- */
        if ((sStatus = TransTermReceiveFile(hWnd, szTransPPI, TRANS_ACCESS_PPI, NewFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_PPI)) < 0) {
            break;
        }

		if((sStatus = TransTermReceiveFileDir(hWnd, szCommIconSaveName, TRANS_ACCESS_ICON, 0, IDS_TRANS_RECEIVE_ICON)) < 0) {
			break;
		}


        /* ----- receive MLD file, V3.3 ----- */
        sStatus = TransTermReceiveFile(hWnd, szTransMLD, TRANS_ACCESS_MLD, NewFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber, IDS_TRANS_RECEIVE_MLD);

        /* ----- receive RSN Reason Code mnemonics file, V3.3  ----- */
        sStatus = TransTermReceiveFile(hWnd, szTransRSN, TRANS_ACCESS_MLD, 400, IDS_TRANS_SEND_MLD);
    } while (0);      // end single loop do to allow break on error during above series of actions.

    return (sStatus);
}

/*
*===========================================================================
** Synopsis:    short TransTermReceiveProg(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: As for this function, each class receives the parameter to
*               2170.
*===========================================================================
*/
short TransTermReceiveProg(HWND hWnd)
{
    short   sStatus;
    WORD    wI = 0;

    if ((sStatus = TransTermReceiveParaFlex(hWnd, CLASS_PARAFLEXMEM)) < 0) { /* P2 */
        return (sStatus);
    }
    if (getTransFwTransTerm() & TRANS_ACCESS_PROG) {
        TransAbortDlgMessage(hWnd, IDS_TRANS_RECEIVE_PROG);
        TransAbortDlgCalcParaMsg(hWnd, wI++ , TRANS_TERM_COMM_NO_PROG);

        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAMDC);   /* P1 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAFSC);   /* P3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        /* receive PLU Key Data with FSC simultaniously, 1/22/98 */
        if (!(getTransFwTransTerm() & TRANS_ACCESS_ACT)) {  /* if user not selected */
            sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPLUNOMENU);/* A4 */
            if (sStatus < 0) {
                return (sStatus);
            }
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARADEPTNOMENU);/* A115,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASECURITYNO);/* P6 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASUPLEVEL);/* P8 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAACTCODESEC);/* P9 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATRANSHALO);/* P10 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPRESETAMOUNT);/* P15 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAHOURLYTIME);/* P17 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASLIPFEEDCTL);/* P18 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATRANSMNEMO);/* P20 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARALEADTHRU);/* P21 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAREPORTNAME);/* P22 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASPECIALMNEMO);/* P23 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAADJMNEMO);/* P46 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPRTMODI);/* P47 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAMAJORDEPT);/* P48 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAAUTOALTKITCH);/* P49 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASHRPRT); /* P50 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAKDSIP);   /* P51,   Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAHOTELID); /* P54 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARACHAR24); /* P57 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATTLKEYTYP);/* P60 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATTLKEYCTL);/* P61 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATEND);/* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATERMINALINFO);/* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_TENDERKEY_INFO);/* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_FILEVERSION_INFO);/* P62 V3.3 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

		sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAAUTOCPN);/* P67 ACC*/
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }

		sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAREASONCODE);/* P67 ACC*/
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_PROG);
        if (sStatus < 0) {
            return (sStatus);
        }
        return (sStatus);
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermReceiveAct(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: As for this function, each class receives the parameter to
*               2170.
*===========================================================================
*/
short TransTermReceiveAct(HWND hWnd)
{
    short   sStatus;
    WORD    wI = 0;

    if (getTransFwTransTerm() & TRANS_ACCESS_ACT) {
        TransAbortDlgMessage(hWnd, IDS_TRANS_RECEIVE_ACT);
        TransAbortDlgCalcParaMsg(hWnd, wI++ , TRANS_TERM_COMM_NO_ACT);

        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPLUNOMENU);/* A4 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        /* receive FSC Data with PLU Key simultaniously, 1/22/98 */
        if (!(getTransFwTransTerm() & TRANS_ACCESS_PROG)) {
            sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAFSC);   /* P3 */
            if (sStatus < 0) {
                return (sStatus);
            }
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARACTLTBLMENU);/* A5 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARACASHABASSIGN);/* A7 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAROUNDTBL);/* A84 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARADISCTBL);/* A86 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASOFTCHK);/* A87 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPROMOTION);/* A88 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARACURRENCYTBL);/* A89 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATARE);  /* A111 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAMENUPLUTBL);/* A116 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATAXTBL1);/* A124 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARATAXRATETBL);/* A127 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAPIGRULE);  /* A130 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARALABORCOST); /* A133 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARADISPPARA); /* A137 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASERVICETIME); /* A154 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAOEPTBL);    /* A160 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAFXDRIVE);   /* A162 */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARARESTRICTION);   /* A170,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARABOUNDAGE);  /* A208,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
		sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARACOLORPALETTE);  /* A209,    Saratoga */ //CSMALL-colorpalette
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }
        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARAMISCPARA);/* A128,    Saratoga */
        TransAbortDlgCalcParaMsg(hWnd, wI++, TRANS_TERM_COMM_NO_ACT);
        if (sStatus < 0) {
            return (sStatus);
        }

        sStatus = TransTermReceiveParaClass(hWnd, CLASS_PARASTOREFORWARD);/* A128,    Saratoga */
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
** Synopsis:    short TransTermReceiveParaClass(HWND hWnd, UCHAR uchClass)
*     Input:    hWnd     - handle of parent window
*               uchClass - class of parameter
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function receives the parameter of the specified class
*               to 2170.
*===========================================================================
*/
SHORT TransTermReceiveParaClass(HWND hWnd, UCHAR uchClass)
{
    USHORT  usRead;
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
            /* ----- receive from 2170 ----- */
            sStatus = CliParaAllRead(uchClass, pBuffer, TRANS_MEMORY_SIZE_TERM, usStart, &usRead);

            /* ----- write to the work file ----- */
            ParaAllWrite(uchClass, pBuffer, usRead, usStart, &usWrite);

            /* ----- seek start addresss ----- */
            usStart += TRANS_MEMORY_SIZE_TERM;
        } while (usRead == TRANS_MEMORY_SIZE_TERM);

        /* ----- release global memory ----- */
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return (sStatus);
    } else {                                /* not enough memory */
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_MEMORY);
        return (TRANS_TERM_COMM_ABORT_USER);
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermReceiveParaFlex(HWND hWnd, UCHAR uchClass)
*     Input:    hWnd     - handle of parent window
*               uchClass - class of parameter
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function receives the parameter of the specified class
*               to 2170.
*===========================================================================
*/
SHORT TransTermReceiveParaFlex(HWND hWnd, UCHAR uchClass)
{
    USHORT  usRead;
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

        /* ----- receive from 2170 ----- */
        sStatus = CliParaAllRead(uchClass, pBuffer, TRANS_MEMORY_SIZE_TERM, 0, &usRead);

        /* ----- write to the work file ----- */
        ParaAllWrite(uchClass, pBuffer, usRead, 0, &usWrite);

        /* ----- release global memory ----- */
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return sStatus;
    } else {                                /* not enough memory */
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_MEMORY);
        return TRANS_TERM_COMM_ABORT_USER;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermReceiveFile(HWND hWnd, char *szFile,
*                                          WORD wFlag, ULONG ulBlockSize,
*                                          WORD wMessage)
*     Input:    hWnd       - handle of parent window
*               szFile     - file name
*               wFlag      - receiption flag
*               sBlockSize - block size of file
*               wMessage   - ID of message string
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function receives the specified file from the terminal.
*               We now just open the file once rather than opening and closing
*               it multiple times.  R.Chambers, May 20, 2015
*
*				Instead of leaving the file open continuously while receiving
*				from the terminal, this function opens the file receives data
*				and closes the file in a continuous loop until all of the data
*				has been received from the specified file on the terminal. This
*				will discourage	a sharing violation on the file if it is being
*				broadcasted or opened by another device. ESMITH
*===========================================================================
*/
short TransTermReceiveFile(HWND hWnd, WCHAR *szFile, WORD wFlag,
                           ULONG ulBlockSize, WORD wMessage)
{
    short   sStatus;                /* return status */
    SHORT   sLocalFile;             /* handle of local file */
    SHORT   sRmtFile;               /* handle of remote file */
    ULONG   sRmtRead;               /* read length */
	ULONG	ulRmtPos=0;				/* remote position of read */
	ULONG	ulRmtActualPos;			/* remote actual read position */
    HGLOBAL hBuffer;                /* handle of global allocation memory */
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

    /* ----- check reception flag ----- */
    if (getTransFwTransTerm() & wFlag) {
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
                    if ((sLocalFile = PifOpenFile(szFile, "ow")) < 0) {
                        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                     IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_CREATE, szFile);
                        sStatus = TRANS_TERM_COMM_ABORT_LOCAL;
                        break;
                    }

					/* ----- open remote file ----- */
					if ((sRmtFile = RmtOpenFile(szFile, "or")) < 0) {
#ifdef _DEBUG
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
					for(;;) {					/* no loop */
                        /* ----- display calculated receive rate ----- */
                        TransAbortDlgCalcFileMsg(hWnd, wMessage, ulBlockSize, nCount++, NULL);

						/* ---- read position in remote file ---- */
						RmtSeekFile((USHORT)sRmtFile, ulRmtPos, &ulRmtActualPos);
						if(ulRmtActualPos != ulRmtPos) {
                            sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                            break;
						}

                        /* ----- read remote file ----- */
                        sRmtRead = RmtReadFile((USHORT)sRmtFile, pBuffer, TRANS_MEMORY_SIZE_TERM);
                        if (sRmtRead > 0) {         /* read successful */
                            /* ----- write work file ----- */
                            PifWriteFile((USHORT)sLocalFile, pBuffer, sRmtRead);

							/* ----- Increment remote read position ----- */
							ulRmtPos += sRmtRead;

                            /* ----- check end of file ----- */
                            if (sRmtRead != TRANS_MEMORY_SIZE_TERM) {
                                sStatus = TRUE;
                                break;
                            }
                        } else if (sRmtRead == 0) { /* end of file */
                            sStatus = TRUE;
                            break;
                        } else {
                            sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                            break;
                        }

                        /* ----- check user abort ----- */
                        if (TransAbortQueryStatus() == TRUE) {
                            sStatus = TRANS_TERM_COMM_ABORT_USER;
                            break;
                        }
                    }

					/* ----- close remote file ----- */
					RmtCloseFile(sRmtFile);

                    /* ----- display finished receive rate ----- */
                    if (sStatus == TRUE) {
                        TransAbortDlgFinMessage(hWnd);
                    }

					/* ------- if file was successully transferred, close the local file ------- */
					if (sStatus == TRUE){
						PifCloseFile(sLocalFile);
					}
                    break;
                }

                /* ----- Initialize mass memory module, Saratoga ----- */
                PluInitialize();

                /* ----- release global memory */
                GlobalUnlock(hBuffer);
                GlobalFree(hBuffer);

                return (sStatus);
            } else {                            /* not enough memory */
                TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_MEMORY);
                return TRANS_TERM_COMM_ABORT_SYSTEM;
            }
        }
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    ULONG TransTermReceiveSizeOnTerm(WCHAR* szFileName)
*     Input:    szFileName     - file name
*
** Return:      greater than 0 then successful
*               equal 0 then unsuccessful
*
** Description: This function checks the size of the specified remote file by
*               reading the file to determine the number of bytes in it.
*
*               Since the layout or logo file has no specific size a call must
*               be made to the terminal to specify the size of the
*               current layout file. ESMITH
*===========================================================================
*/
ULONG TransTermReceiveSizeOnTerm(WCHAR* szFileName)
{
    SHORT   sRmtFile;              /* handle of remote file on terminal*/
    ULONG   ulRmtTotal = 0;        /* read length */

	/* ----- read size of layout file from terminal ---- */
	if ( (sRmtFile = RmtOpenFile(szFileName, "or")) >= 0) {
		HGLOBAL hBuffer;                /* handle of global allocation memory */

		/* ----- get global memory ----- */
		hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
		if (hBuffer)
		{
			VOID  *pBuffer = GlobalLock(hBuffer);  /* lock global memory  */

			for( ; pBuffer ; ) {
				ULONG   ulRmtRead, ulRmtAct;            /* read length */

				RmtSeekFile(sRmtFile, ulRmtTotal, &ulRmtAct);
				if(ulRmtAct != ulRmtTotal) {
					// we are unable to seek to our last position so file error.
					ulRmtTotal = 0;
					break;
				}

				ulRmtRead = RmtReadFile(sRmtFile, pBuffer, TRANS_MEMORY_SIZE_TERM);
				if ( ulRmtRead >= 0) {
					ulRmtTotal += ulRmtRead;
					if ( ulRmtRead != TRANS_MEMORY_SIZE_TERM) {
						// we read only part of the requested buffer size so we are done. No more to read.
						break;
					}
				} else {
					// error during the read so error out.
					ulRmtTotal = 0;
					break;
				}
			}
			GlobalUnlock(hBuffer);
			GlobalFree(hBuffer);
		}

		RmtCloseFile(sRmtFile);
	}

	return ulRmtTotal;
}


/*
*===========================================================================
** Synopsis:    short TransTermReceiveFile(HWND hWnd, char *szFile,
*                                          WORD wFlag, ULONG ulBlockSize,
*                                          WORD wMessage)
*     Input:    hWnd       - handle of parent window
*               szFile     - file name
*               wFlag      - receiption flag
*               sBlockSize - block size of file
*               wMessage   - ID of message string
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function receives the specified file from the terminal.
*				Instead of leaving the file open continuously while receiving
*				from the terminal, this function opens the file receives data
*				and closes the file in a continuous loop until all of the data
*				has been received from the specified file on the terminal. This
*				will discourage	a sharing violation on the file if it is being
*				broadcasted or opened by another device. ESMITH
*===========================================================================
*/
short TransTermReceiveFileDir(HWND hWnd, WCHAR *szDir, WORD wFlag,
                           ULONG ulBlockSize, WORD wMessage)
{
    short   sStatus;                /* return status */
	WCHAR   myFileName[3][20] = {WIDE("*.gif"), WIDE("*.jpg"), WIDE("*.bmp")};
    /* ----- check user by abort ----- */
    if (TransAbortQueryStatus() == TRUE) {
        return TRANS_TERM_COMM_ABORT_USER;
    }

    /* ----- check reception flag ----- */
    if (getTransFwTransTerm() & wFlag) {
		HGLOBAL hBuffer;                /* handle of global allocation memory */
		VOID    *pBuffer;
		short   nCount = 0, i;
		WCHAR   paszMode[8] = {0};
		CHAR    paszRmtOpenMode[8] = {0};

		switch (wFlag) {
			case TRANS_ACCESS_ICON:                 // TransTermReceiveFileDir() Determine Remote File Access Mode
				paszMode[0] = WIDE('i');            // icon folder is target for file search.
				strcpy(paszRmtOpenMode, "ori");     // icon folder is target for remote file open.
				break;
			default:
				return TRUE;                        // illegal folder specified, ignore this request.
				break;
		}

        TransAbortDlgMessage(hWnd, wMessage);

		if(!szDir[0])
		{
			TransTermSetIconDirectory(hWnd);
		}
		_wchdir(szDir);

        /* ----- allocate and lock global memory ----- */
		hBuffer = GlobalAlloc(GMEM_FIXED, TRANS_MEMORY_SIZE_TERM);
        pBuffer = GlobalLock(hBuffer);

       	for(i = 0; i < 3; i++)
		{
			SHORT  doit = 1;
			WCHAR  szFileName[RMT_FILENAME_SIZE + 1] = {0};

			doit = RmtGetFileNames(szFileName, paszMode, myFileName[i]);
			if (doit < 1) continue;

			while (doit > 0) {     /* no loop */
				ULONG   sRmtRead = 0;           /* read length */
				ULONG	ulRmtPos = 0;			/* remote position of read */
				ULONG	ulRmtActualPos = 0;		/* remote actual read position */
				SHORT   sLocalFile;             /* handle of local file */
				SHORT   sRmtFile;               /* handle of remote file */

                /* ----- open work file ----- */
                if ((sLocalFile = PifOpenDirFile(szDir, szFileName, "nw")) < 0) 
				{
					if ((sLocalFile = PifOpenDirFile(szDir, szFileName, "ow")) < 0) 
					{
						TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
									 IDS_TRANS_NAME, IDS_TRANS_ERR_LOCAL_FILE_CREATE, szFileName);
						sStatus = TRANS_TERM_COMM_ABORT_LOCAL; 
						break;
					}
                }

				/* ----- open remote file ----- */
				if ((sRmtFile = RmtOpenFile(szFileName, paszRmtOpenMode)) < 0) {
#ifdef _DEBUG
					TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
								 IDS_TRANS_NAME, IDS_TRANS_ERR_RMT_FILE_OPEN, szFileName);
#endif
					PifCloseFile(sLocalFile);
					sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
					break;
				}
				for(;;) {					/* loop to transfer the current file */
                    /* ----- display calculated receive rate ----- */
                    TransAbortDlgCalcFileMsg(hWnd, wMessage, ulBlockSize, nCount++, szFileName);

					/* ---- read position in remote file ---- */
					RmtSeekFile((USHORT)sRmtFile, ulRmtPos, &ulRmtActualPos);

					if(ulRmtActualPos != ulRmtPos) {
                        sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                        break;
					}

                    /* ----- read remote file ----- */
                    sRmtRead = RmtReadFile((USHORT)sRmtFile, pBuffer, TRANS_MEMORY_SIZE_TERM);
                    if (sRmtRead > 0) {         /* read successful */
                        /* ----- write work file ----- */
                        PifWriteFile((USHORT)sLocalFile, pBuffer, sRmtRead);

						/* ----- Increment remote read position ----- */
						ulRmtPos += sRmtRead;

                        /* ----- check end of file ----- */
                        if (sRmtRead != TRANS_MEMORY_SIZE_TERM) {
                            sStatus = TRUE;
                            break;
                        }
                    } else if (sRmtRead == 0) { /* end of file */
                        sStatus = TRUE;
                        break;
                    } else {
                        sStatus = TRANS_TERM_COMM_ABORT_REMOTE;
                        break;
                    }

                    /* ----- check user abort ----- */
                    if (TransAbortQueryStatus() == TRUE) {
                        sStatus = TRANS_TERM_COMM_ABORT_USER;
                        break;
                    }
                }

                /* ----- display finished receive rate ----- */
                if (sStatus == TRUE) {
                    TransAbortDlgFinMessage(hWnd);
                }

                /* ----- close work files ----- */
				RmtCloseFile(sRmtFile);

				/* ------- if file was successully transferred, close the local file ------- */
				PifCloseFile(sLocalFile);
				doit = RmtGetFileNames(szFileName, paszMode, myFileName[i]);
            }
        } 
		/* ----- release global memory */
		GlobalUnlock(hBuffer);
		GlobalFree(hBuffer);
    }

    return TRUE;
}

/* ===== End of TRATRECE.C ===== */