/*
* ---------------------------------------------------------------------------
* Title         :   Print
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PEPPRN.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PepPrn():           create print dialogbox
*               PepPrnDlgProc():    print dialogbox procedure
*               PepPrnCtrlDlgItem():control dialogbox item
*               PepPrnGetItemStat():get dialogbox item status
*               PepPrnInitFormBtn():initilaize form selection button
*               PepPrnOut():        print out
*               PepPrnSetup():      display setup dialogbox
*               PepPrnStopDlgProc():print stop dialogbox procedure
*               PepPrnDspStatMsg(): display printing message
*               PepPrnAbortProc():  print abort procedure
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jan-28-00 : 01.00.00 : hrkato     : Saratoga
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
#include    <drivinit.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "prog.h"
#include    "pepprn.h"
#include    "pepprnl.h"
#include    "prnp003.h"
#include    "prna068.h"
#include    "prna068c.h"


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
static BOOL    fPrnAbort;
HWND    hdlgPrnAbort;
PRNSET  PrnSet;

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
**  Synopsis:   BOOL  PepPrn();
*
**  Input   :   HWND    hWnd    - handle of a window procedure
*
**  Return  :   TRUE        - user process is executed.
*               FALSE       - error occured.
*
**  Description:
*       This procedure print out data.
* ===========================================================================
*/
BOOL    PepPrn(HWND hWnd)
{
    DLGPROC lpfnPrn;
    HWND    hDlgPrn;
    BOOL    fRet;

    /* ----- get instance of PepPrnDlgProc() ----- */
//    lpfnPrn = MakeProcInstance(PepPrnDlgProc, hPepInst);

    /* ----- create dialogbox ----- */
    hDlgPrn = (HWND)DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_PRN), hWnd, PepPrnDlgProc);
    if (hDlgPrn == 0) {
        fRet = FALSE;
    } else {
        fRet = TRUE;
    }

    /* ----- release instance ----- */
    FreeProcInstance(lpfnPrn);

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  PepPrnDlgProc()
*
**  Input       :   HWND    hDlg    -   handle of a dialogbox
*                   WORD    wMsg    -   dispacthed message
*                   WORD    wParam  -   16 bits message parameter
*                   LONG    lParam  -   32 bits message parameter
*
**  Return      :   TRUE            -   user process is executed
*                   FALSE           -   default process is expected
*
**  Description :
*       This is a dialogbox procedure to display stop printing.
* ===========================================================================
*/
BOOL WINAPI  PepPrnDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HWND    hWnd;
    WCHAR    szDesc[128];
    WCHAR    szCaption[16];
    BOOL    fRet;
    BOOL    fwTrans;
	int i;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- get window handle ----- */
        hWnd = GetParent(hDlg);

        /* ----- initialize number of copies ----- */
        PrnSet.wCopy     = 1;
        PrnSet.fwSelect  = 0;
        PrnSet.bFormP03  = PRN_FORM_P03_FSC;
        PrnSet.bFormA68  = PRN_FORM_A68_COND;
        PrnSet.bRngA68   = PRN_RNG_A68_ALL;
        memset(PrnSet.szStartA68, 0, sizeof(PrnSet.szStartA68));
        memset(PrnSet.szEndA68, 0, sizeof(PrnSet.szEndA68));
		for(i = 0; i < PRN_PLUNO_LEN ; i++){
			PrnSet.szStartA68 [i] = PRN_A68_CHAR_ASC_0;
			PrnSet.szEndA68 [i] = PRN_A68_CHAR_ASC_9;
		}
       // memset(PrnSet.szStartA68, PRN_A68_CHAR_ASC_0/*0x30*/, PRN_PLUNO_LEN);
       // memset(PrnSet.szEndA68, PRN_A68_CHAR_ASC_9/*0x39*/, PRN_PLUNO_LEN);

		DlgItemRedrawText(hDlg, IDD_PRN_A68_START, PrnSet.szStartA68);
		DlgItemRedrawText(hDlg, IDD_PRN_A68_END, PrnSet.szEndA68);

        /* ----- set number of copies ----- */
        SetDlgItemInt(hDlg, IDD_PRN_COPY, PrnSet.wCopy, FALSE);

        /* ----- control button ----- */
        PepPrnCtrlDlgItem(hDlg);

        /* ----- initialize form selction button ----- */
        PepPrnInitFormBtn(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_PRN_PRINT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_SETUP, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_COPY, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_P03, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_P03_FSC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_P03_PLUKEY, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_P03_BLANK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_COND, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_DESC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_ALL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_NUM, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_START, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_A68_END, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_PRN_CAPTION1, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);

		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_PRN_PRINT:
            /* ----- end dialog box ----- */
            EndDialog(hDlg, FALSE);

            /* ----- print out ----- */
            fRet = PepPrnOut(hWnd);
            return (fRet);

        case IDD_PRN_SETUP:
            /* ----- display setup dialogbox ----- */
            PepPrnSetup(hWnd);
            return TRUE;

        case IDD_PRN_COPY:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- get number of copies ----- */
                PrnSet.wCopy = (WORD)GetDlgItemInt(hDlg, IDD_PRN_COPY, &fwTrans, FALSE);
                return TRUE;
            } else {
                return FALSE;
            }

        case IDD_PRN_P03:
        case IDD_PRN_A68:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- control button ----- */
                PepPrnCtrlDlgItem(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }

        case IDD_PRN_P03_FSC:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bFormP03 = PRN_FORM_P03_FSC;
				PepPrnInitFormBtn(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_P03_PLUKEY:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bFormP03 = PRN_FORM_P03_PLUKEY;
				PepPrnInitFormBtn(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_P03_BLANK:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bFormP03 = PRN_FORM_P03_BLANK;
				PepPrnInitFormBtn(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_A68_COND:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bFormA68 = PRN_FORM_A68_COND;
				PepPrnInitFormBtn(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_A68_DESC:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bFormA68 = PRN_FORM_A68_DESC;
				PepPrnInitFormBtn(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_A68_ALL:
            if (HIWORD(wParam) == BN_CLICKED) {
				int i;
				PrnSet.bRngA68 = PRN_RNG_A68_ALL;
				//memset(PrnSet.szStartA68, 0x30, PRN_PLUNO_LEN);
				//memset(PrnSet.szEndA68, 0x39, PRN_PLUNO_LEN);
				for(i = 0; i < PRN_PLUNO_LEN ; i++){
					PrnSet.szStartA68 [i] = 0x30;
					PrnSet.szEndA68 [i] = 0x39;
				}
				PepPrnInitFormBtn(hDlg);
                PepPrnCtrlDlgItem(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_A68_NUM:
            if (HIWORD(wParam) == BN_CLICKED) {
				PrnSet.bRngA68 = PRN_RNG_A68_NUM;
				PrnA68GetPluNo(hDlg, IDD_PRN_A68_START, PrnSet.szStartA68, FALSE);
				PrnA68GetPluNo(hDlg, IDD_PRN_A68_END, PrnSet.szEndA68, FALSE);
				PepPrnInitFormBtn(hDlg);
                PepPrnCtrlDlgItem(hDlg);
                return TRUE;
            } else {
                return FALSE;
            }
			break;

        case IDD_PRN_A68_START:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- get number of start,   Saratoga ----- */
                if (PrnA68GetPluNo(hDlg, IDD_PRN_A68_START, PrnSet.szStartA68, FALSE) == FALSE) {
                    LoadString(hPepInst, IDS_PRN_A68_ERR_BADPLU, szDesc, PEP_STRING_LEN_MAC(szDesc) - 1);
					LoadString (hPepInst, IDS_PRN_A68_MB_LABEL, szCaption, PEP_STRING_LEN_MAC(szCaption) - 1);

                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hWnd, szDesc, szCaption, MB_ICONEXCLAMATION | MB_OK);

                    SetFocus(GetDlgItem(hDlg, IDD_PRN_A68_START));
                }
/* 
                wNo = (WORD)GetDlgItemInt(hDlg,
                                          IDD_PRN_A68_START,
                                          &fwTrans,
                                          FALSE);
                if (fwTrans == NULL) {
                    PrnSet.wStartA68 = 0;
                } else {
                    PrnSet.wStartA68 = wNo;
                }
*/
                return TRUE;
            } else {
                return FALSE;
            }

        case IDD_PRN_A68_END:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- get number of end, Saratoga ----- */
                if (PrnA68GetPluNo(hDlg, IDD_PRN_A68_END, PrnSet.szEndA68, FALSE) == FALSE) {
                    LoadString(hPepInst, IDS_PRN_A68_ERR_BADPLU, szDesc, PEP_STRING_LEN_MAC(szDesc) - 1);
					LoadString (hPepInst, IDS_PRN_A68_MB_LABEL, szCaption, PEP_STRING_LEN_MAC(szCaption) - 1);

                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hWnd, szDesc, szCaption, MB_ICONEXCLAMATION | MB_OK);

                    SetFocus(GetDlgItem(hDlg, IDD_PRN_A68_END));
                }
/*
                wNo = (WORD)GetDlgItemInt(hDlg,
                                          IDD_PRN_A68_END,
                                          &fwTrans,
                                          FALSE);
                if (fwTrans == NULL) {
                    PrnSet.wEndA68 = PRN_A68_REC_MAX;
                } else {
                    PrnSet.wEndA68 = wNo;
                }
*/
                return TRUE;

            } else {
                return FALSE;
            }

        case IDCANCEL:
            /* ----- end dialog box ----- */
            EndDialog(hDlg, FALSE);
            return TRUE;

        default:
            return FALSE;
        }

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID  PepPrnCtrlDlgItem();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*
**  Return  :   None
*
**  Description:
*       This procedure control dialogbox item.
* ===========================================================================
*/
VOID    PepPrnCtrlDlgItem(HWND hDlg)
{
    BOOL    fRng;

    /* ----- set print flag ----- */
    if (IsDlgButtonChecked(hDlg, IDD_PRN_P03) == TRUE) {
        PrnSet.fwSelect |= PRN_FLAG_P03;
    } else {
        PrnSet.fwSelect &= ~PRN_FLAG_P03;
    }
    if (IsDlgButtonChecked(hDlg, IDD_PRN_A68) == TRUE) {
        PrnSet.fwSelect |= PRN_FLAG_A68;
    } else {
        PrnSet.fwSelect &= ~PRN_FLAG_A68;
    }

    /* ----- set print range flag ----- */
    if ((PrnSet.bRngA68 == PRN_RNG_A68_NUM) &&
        (PrnSet.fwSelect & PRN_FLAG_A68)) {
        fRng = TRUE;
    } else {
        fRng = FALSE;
    }

    /* ----- control print button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_PRINT), PrnSet.fwSelect);

    /* ----- control FSC checkbox ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_P03_FSC), (PrnSet.fwSelect & PRN_FLAG_P03));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_P03_PLUKEY), (PrnSet.fwSelect & PRN_FLAG_P03));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_P03_BLANK), (PrnSet.fwSelect & PRN_FLAG_P03));

    /* ----- control PLU checkbox and edit-text ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_COND), (PrnSet.fwSelect & PRN_FLAG_A68));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_DESC), (PrnSet.fwSelect & PRN_FLAG_A68));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_RNG), (PrnSet.fwSelect & PRN_FLAG_A68));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_ALL), (PrnSet.fwSelect & PRN_FLAG_A68));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_NUM), (PrnSet.fwSelect & PRN_FLAG_A68));
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_START), fRng);
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_END), fRng);
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_FROM), fRng);
    EnableWindow(GetDlgItem(hDlg, IDD_PRN_A68_TO), fRng);
}

/*
* ===========================================================================
**  Synopsis:   VOID  PepPrnGetItemStat();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*
**  Return  :   None
*
**  Description:
*       This procedure get dialogbox item status.
* ===========================================================================
*/
VOID    PepPrnGetItemStat(HWND hDlg)
{
    /* ----- check FSC form selection ----- */
    if (IsDlgButtonChecked(hDlg, IDD_PRN_P03_FSC) == TRUE) {
        PrnSet.bFormP03 = PRN_FORM_P03_FSC;
    } else if (IsDlgButtonChecked(hDlg, IDD_PRN_P03_PLUKEY) == TRUE) {
        PrnSet.bFormP03 = PRN_FORM_P03_PLUKEY;
    } else {
        PrnSet.bFormP03 = PRN_FORM_P03_BLANK;
    }

    /* ----- check PLU form selection ----- */
    if (IsDlgButtonChecked(hDlg, IDD_PRN_A68_COND) == TRUE) {
        PrnSet.bFormA68 = PRN_FORM_A68_COND;
    } else {
        PrnSet.bFormA68 = PRN_FORM_A68_DESC;
    }

    /* ----- check PLU range selection ----- */
    if (IsDlgButtonChecked(hDlg, IDD_PRN_A68_ALL) == TRUE) {
        PrnSet.bRngA68 = PRN_RNG_A68_ALL;
        memset(PrnSet.szStartA68, 0x30, PRN_PLUNO_LEN);
        memset(PrnSet.szEndA68, 0x39, PRN_PLUNO_LEN);
    } else {
        PrnSet.bRngA68 = PRN_RNG_A68_NUM;
        PrnA68GetPluNo(hDlg, IDD_PRN_A68_START, PrnSet.szStartA68, FALSE);
        PrnA68GetPluNo(hDlg, IDD_PRN_A68_END, PrnSet.szEndA68, FALSE);
    }
    PepPrnInitFormBtn(hDlg);
}

/*
* ===========================================================================
**  Synopsis:   VOID  PepPrnInitFormBtn();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*
**  Return  :   None
*
**  Description:
*       This procedure initialize form selction button.
* ===========================================================================
*/
VOID    PepPrnInitFormBtn(HWND hDlg)
{
    /* ----- set radio button ----- */
    SendDlgItemMessage(hDlg, IDD_PRN_P03_FSC, BM_SETCHECK, (PrnSet.bFormP03 == PRN_FORM_P03_FSC), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_P03_PLUKEY, BM_SETCHECK, (PrnSet.bFormP03 == PRN_FORM_P03_PLUKEY), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_P03_BLANK, BM_SETCHECK, (PrnSet.bFormP03 == PRN_FORM_P03_BLANK), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_A68_COND, BM_SETCHECK, (PrnSet.bFormA68 == PRN_FORM_A68_COND), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_A68_DESC, BM_SETCHECK, (PrnSet.bFormA68 == PRN_FORM_A68_DESC), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_A68_ALL, BM_SETCHECK, (PrnSet.bRngA68 == PRN_RNG_A68_ALL), 0L);
    SendDlgItemMessage(hDlg, IDD_PRN_A68_NUM, BM_SETCHECK, (PrnSet.bRngA68 == PRN_RNG_A68_NUM), 0L);

}

/*
* ===========================================================================
**  Synopsis:   BOOL  PepPrnOut();
*
**  Input   :   HWND    hWnd    - handle of a dialogbox
*
**  Return  :   TRUE        - user process is executed.
*               FALSE       - error occured.
*
**  Description:
*       This procedure print out data.
* ===========================================================================
*/
BOOL    PepPrnOut(HWND hWnd)
{
    BOOL    fRet = FALSE;
    HDC     hDC;
    WCHAR    szPrinter[64];
    LPWSTR   lpszDriver;
    LPWSTR   lpszDevice;
    LPWSTR   lpszOutput;
    DLGPROC lpfnPrnStop;
    DLGPROC lpfnPrnAbort;

    /* ----- get printer driver name from 'win.ini' ----- */
    GetProfileStringPep(WIDE("Windows"), WIDE("device"), WIDE(""), szPrinter, sizeof(szPrinter) / sizeof(szPrinter[0]));

    /* ----- store address for CreateDC() ----- */
	{
		wchar_t *pContext;

		lpszDevice = wcstok_s(szPrinter, WIDE(","), &pContext);
		lpszDriver = wcstok_s(NULL, WIDE(","), &pContext);
		lpszOutput = wcstok_s(NULL, WIDE(","), &pContext);
	}

    /* ----- get handle of device context ----- */
    if ((hDC = CreateDCPep(lpszDriver, lpszDevice, lpszOutput, NULL)) == 0) {
        /* ----- show error message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hWnd, WIDE("Could not create hDC."), WIDE("Print"), MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    /* ----- set print abort flag (not abort) ----- */
    fPrnAbort = FALSE;

    /* ----- get instance of PepPrnStopDlgProc() ----- */

//    lpfnPrnStop = MakeProcInstance(PepPrnStopDlgProc, hPepInst);

    /* ----- create dialogbox ----- */
    hdlgPrnAbort = DialogCreation(hResourceDll/*hPepInst*/, MAKEINTRESOURCEW(IDD_PRN_STOP), hWnd, PepPrnStopDlgProc);

    /* ----- get instance of PrintAbortProc() ----- */
    lpfnPrnAbort = MakeProcInstance(PepPrnAbortProc, hPepInst);

    /* ----- set abort procedure ----- */
    Escape(hDC, SETABORTPROC, 0, lpfnPrnAbort, NULL);


    /* ----- set mapping mode of device context ----- */
    SetMapMode(hDC, MM_TEXT);

    if (PrnSet.fwSelect & PRN_FLAG_P03) {
        /* ----- print out FSC ----- */
        fRet |= PrnP03(hWnd, hDC);
    }

    if (PrnSet.fwSelect & PRN_FLAG_A68) {
        /* ----- print out PLU ----- */
        fRet |= PrnA68(hWnd, hDC);
    }

    if (fPrnAbort == FALSE) {
        /* ----- destroy dialogbox ----- */
        DestroyWindow(hdlgPrnAbort);

        /* ----- reset handle ----- */
        hdlgPrnAbort = NULL;
    }

    /* ----- enable menu item ----- */
    PepMenuControl(hwndPepMain, PEP_MENU_ALL_ON, 0L);

    /* ----- release instance ----- */
    FreeProcInstance(lpfnPrnStop);
    FreeProcInstance(lpfnPrnAbort);

    /* ----- release handle of device context ----- */
    DeleteDC(hDC);

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID  PepPrnSetup()
*
**  Input   :   HWND    hWnd    - handle of a dialogbox
*
**  Return  :   None
*
**  Description:
*       This procedure display setup dialogbox.
* ===========================================================================
*/
VOID    PepPrnSetup(HWND hWnd)
{
    WCHAR        szPrinter[64];
    WCHAR        szSysDir[128];
    WCHAR        szFullDriver[256];
	WCHAR *     lpContext;
    WCHAR *     lpszDriver;
	WCHAR *     lpszDevice;
	WCHAR *     lpszOutput;
	HMODULE     hDriver;
    DEVMODE     DevMode;
    LPFNDEVMODE lpfnDevMode;

    /* ----- get printer driver name from 'win.ini' ----- */
    GetProfileString(L"Windows", L"device", L"", szPrinter, sizeof(szPrinter)/sizeof(szPrinter[0]));

    /* ----- store address for CreateDC() ----- */
    lpszDevice = wcstok(szPrinter, L",", &lpContext);
    lpszDriver = wcstok(NULL, L",", &lpContext);
    lpszOutput = wcstok(NULL, L",", &lpContext);

    /* ----- get system directory ----- */
    GetSystemDirectory(szSysDir, sizeof(szSysDir));

    /* ----- make full path string ----- */
    lstrcpy(szFullDriver, szSysDir);
    lstrcat(szFullDriver, L"\\");
    lstrcat(szFullDriver, lpszDriver);
    lstrcat(szFullDriver, L".DRV");

    /* ----- get handle of printer driver ----- */
    hDriver = LoadLibrary(szFullDriver);
    if (hDriver > 31) {
        /* ----- call ExtDeviceMode indirectory ----- */
		// The ExtDeviceMode function is provided only for compatibility with 16 - bit applications.
		// Printer drivers without this requirement should instead use the DocumentProperties function,
		// which is described in the Microsoft Windows SDK documentation.
		//
		// The ExtDeviceMode function retrieves or modifies printer initialization information for a
		// given graphics driver, or displays a driver - supplied printer - configuration property sheet
		// for the specified printer.
		// See https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/winspool/nf-winspool-extdevicemode

		lpfnDevMode = (LPFNDEVMODE)GetProcAddress(hDriver, "ExtDeviceMode");
        if (lpfnDevMode != NULL) {

            /* ----- execute ExtDeviceMode ----- */
            (*lpfnDevMode)(hWnd, hDriver, &DevMode, lpszDevice, lpszOutput, NULL, NULL, DM_PROMPT | DM_UPDATE);

            /* ----- set active window ----- */
            SetActiveWindow(hWnd);

            /* ----- set focus to window ----- */
            SetFocus(GetDlgItem(hWnd, IDD_PRN_SETUP));
        }
    } else {
        /* ----- show error message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hWnd, WIDE("Could not load driver file."), WIDE("Print"), MB_ICONEXCLAMATION | MB_OK);
    }

    /* ----- release library from memory ----- */
    FreeLibrary(hDriver);

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  PepPrnStopDlgProc()
*
**  Input       :   HWND    hDlg    -   handle of a dialogbox
*                   WORD    wMsg    -   dispacthed message
*                   WORD    wParam  -   16 bits message parameter
*                   LONG    lParam  -   32 bits message parameter
*
**  Return      :   TRUE            -   user process is executed
*                   FALSE           -   default process is expected
*
**  Description :
*       This is a dialogbox procedure to display stop printing.
* ===========================================================================
*/
BOOL WINAPI  PepPrnStopDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- disable menu item ----- */

        PepMenuControl(hwndPepMain, PEP_MENU_ALL_OFF, 0L);

        return TRUE;

    case PM_PRN_DSPMSG:

        /* ----- display printing message ----- */

        PepPrnDspStatMsg(hDlg, wParam, lParam);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDCANCEL:

            /* ----- set print abort flag (abort) ----- */

            fPrnAbort = TRUE;

            /* ----- sound beep ----- */

            MessageBeep(MB_OK);

            /* ----- enable menu item ----- */

            PepMenuControl(hwndPepMain, PEP_MENU_ALL_ON, 0L);

            /* ----- destroy dialogbox ----- */

            DestroyWindow (hDlg);

            /* ----- reset handle ----- */

            hdlgPrnAbort = NULL;

            return TRUE;

        default:
            return FALSE;
        }

    default:
        PEP_REFER(lParam);
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PepPrnDspStatMsg()
*
**  Input       :   HWND    hDlg    -   handle of a dialogbox
*                   WORD    wParam  -   16 bits message parameter
*                   LONG    lParam  -   32 bits message parameter
*
**  Return      :   None
*
**  Description :
*       This is a procedure to display printing message.
* ===========================================================================
*/
VOID    PepPrnDspStatMsg(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    WCHAR    szWork[PRN_BUFF_LEN];
    WCHAR    szBuff[PRN_BUFF_LEN];

    /* ----- make string to display ----- */

    LoadString(hResourceDll, wParam, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, HIWORD(lParam), LOWORD(lParam));

    /* ----- display message ----- */

    DlgItemRedrawText(hDlg, IDD_PRN_STOP_DSPMSG, szWork);

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  PepPrnAbortProc()
*
**  Input       :   HDC     hdcPrinter - device context handle of printer
*                   int     nCode      - 
*
**  Return      :   TRUE            -   user process is executed
*                   FALSE           -   default process is expected
*
**  Description :
*       This is a procedure to abort printing.
* ===========================================================================
*/
BOOL WINAPI  PepPrnAbortProc(HDC hdcPrinter, int nCode)
{
    MSG     Msg;

    while ((fPrnAbort == FALSE) &&
           (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))) {

        if ((hdlgPrnAbort == NULL)
            || (! IsDialogMessage(hdlgPrnAbort, &Msg))) {
                                            /* determine processes message */

            /* ----- translate messages from virtual-key to character ----- */

            TranslateMessage((const MSG FAR *)&Msg);

            /* ----- dispatch message to window ----- */

            DispatchMessage((const MSG FAR *)&Msg);

        }
    }

    return (! fPrnAbort);

    PEP_REFER(hdcPrinter);
    PEP_REFER(nCode);
}

/* ===== End of PEPPRN.C ===== */