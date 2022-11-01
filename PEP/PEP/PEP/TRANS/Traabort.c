/*
* ---------------------------------------------------------------------------
* Title         :   Abort Function
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRAABORT.C
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
* Apr-06-95 : H.Ishida : H.Ishida   : Add R3.0
* Sep-28-98 : 03.03.00 : A.Mitsui   : V3.3 WaitLoop, caluculate MLD
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

#include <ecr.h>

#include "pep.h"
#include "pepcomm.h"
#include "pepcalc.h"
#include "trans.h"
#include "transl.h"

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
TRANSABORT  TransAbort;

/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/
static  WORD    wTransDrawPrev;

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    short TransAbortCreateDlg(HWND hWnd, WORD wTimer)
*     Input:    hDlg   - handle of parent window
*               wTimer - Timer Counter
*
** Return:      TRUE  - Successful
*               FALSE - Unsuccessful
*
** Description:
*===========================================================================
*/
void TransAbortCreateDlg(HWND hWnd)
{
    DLGPROC     lpitDlgProc;
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    lpitDlgProc = MakeProcInstance(TransAbortDlgProc, hInst);

    TransAbort.hDlg = DialogCreation(hInst,
                                   MAKEINTRESOURCEW(IDD_TRANS_DATA_ABORT),
                                   hWnd,
                                   lpitDlgProc);

    if (!SetTimer(TransAbort.hDlg, 1, 2000, (TIMERPROC)lpitDlgProc)) {
        TransMessage(hWnd, MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION,
                     IDS_TRANS_NAME,
                     IDS_TRANS_TIMER);
    }
}

/*
*===========================================================================
** Synopsis:    short TransAbortDestroyDlg(void)
*
** Return:      TRUE  - Successful
*               FALSE - Unsuccessful
*
** Description:
*===========================================================================
*/
void TransAbortDestroyDlg(void)
{
    if (IsWindow(TransAbort.hDlg)) {
        KillTimer(TransAbort.hDlg, 1);

        DestroyWindow(TransAbort.hDlg);
        
        TransAbort.hDlg = 0;
    }
}

/*
*===========================================================================
** Synopsis:    BOOL TransAbortQueryStatus(void)
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
BOOL TransAbortQueryStatus(void)
{
    /* ----- check user abort flag ----- */
    if (TransAbort.wStatus == 1) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransAbortDlgMessage(HWND hWnd, WORD wPrimary)
*     Input:    hDlg     - handle of parent window
*               wPrimary - primary message ID
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
short TransAbortDlgMessage(HWND hWnd, UINT wPrimary)
{
    WCHAR    szMessage[TRANS_MAX_MESSAGE_LEN];

    LoadString(hResourceDll, wPrimary, szMessage, PEP_STRING_LEN_MAC(szMessage));
    DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE1, szMessage);
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransAbortDlgCalcFileMsg()
*     Input:    hDlg     - handle of parent window
*               wPrimary - primary message ID
*               nSize    - total trans size
*               nCount   - loop counter
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
short TransAbortDlgCalcFileMsg(HWND hWnd, WORD wPrimary,
                               ULONG ulSize, short nCount,WCHAR *pwchFileName)
{
    WCHAR    szWork[TRANS_MAX_MESSAGE_LEN];
    WCHAR    szMessage[TRANS_MAX_MESSAGE_LEN];
    WORD    wRate;
    DWORD   dwTotal = 0L;

    switch (wPrimary) {
    case IDS_TRANS_SEND_DEPT:
    case IDS_TRANS_RECEIVE_DEPT:
        dwTotal = PEPCALCDEPTPARA(ulSize);
        break;

    case IDS_TRANS_SEND_PLU:
    case IDS_TRANS_RECEIVE_PLU:
        dwTotal = PEPCALCPLUPARA(ulSize);
        break;

    case IDS_TRANS_SEND_PLUINDEX:
    case IDS_TRANS_RECEIVE_PLUINDEX:
        dwTotal = PEPCALCPLUIDX(ulSize);
        break;
    
    case IDS_TRANS_SEND_OEP:
    case IDS_TRANS_RECEIVE_OEP:
        dwTotal = PEPCALCOEPPARA(ulSize);
        break;
    
    case IDS_TRANS_SEND_WAITER:
    case IDS_TRANS_RECEIVE_WAITER:
        dwTotal = PEPCALCSERPARA(ulSize);
        break;

    case IDS_TRANS_SEND_CASHIER:
    case IDS_TRANS_RECEIVE_CASHIER:
        dwTotal = PEPCALCCASHPARA(ulSize);
        break;

    case IDS_TRANS_SEND_ETK:
    case IDS_TRANS_RECEIVE_ETK:
        dwTotal = PEPCALCETKPARA(ulSize);
        break;

    case IDS_TRANS_SEND_CPN:
    case IDS_TRANS_RECEIVE_CPN:
        dwTotal = PEPCALCCPNPARA(ulSize);
        break;

    case IDS_TRANS_SEND_CSTR:
    case IDS_TRANS_RECEIVE_CSTR:
        dwTotal = PEPCALCCSTRPARA(ulSize);
        break;

    case IDS_TRANS_SEND_RPT:
    case IDS_TRANS_RECEIVE_RPT:
        dwTotal = PEPCALCRPTPARA(ulSize);
        break;

    case IDS_TRANS_SEND_PPI:
    case IDS_TRANS_RECEIVE_PPI:
        dwTotal = PEPCALCPPIPARA(ulSize);
        break;

    case IDS_TRANS_SEND_MLD:
    case IDS_TRANS_RECEIVE_MLD:
        dwTotal = PEPCALCMLDPARA(ulSize);
        break;

	case IDS_TRANS_SEND_TOUCHLAY: //ESMITH LAYOUT
	case IDS_TRANS_SEND_KEYBRDLAY:
	case IDS_TRANS_RECEIVE_TOUCH:
	case IDS_TRANS_RECEIVE_KEYBRD:
	case IDS_TRANS_RECEIVE_LOGO:
	case IDS_TRANS_SEND_LOGO:
	case IDS_TRANS_SEND_FPDB:
	case IDS_TRANS_RECEIVE_FPDB:
		dwTotal = ulSize;
		break;

	case IDS_TRANS_SEND_ICON:
	case IDS_TRANS_RECEIVE_ICON:
		//IDD_TRANS_ABORT_MESSAGE3
		dwTotal = ulSize;
		DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE3, pwchFileName);
		return TRUE;
		

	default:
		break;

    }

    wRate = (WORD)((DWORD)((DWORD)((DWORD)nCount * TRANS_MEMORY_SIZE_TERM) * 100L) / dwTotal);
    LoadString(hResourceDll, IDS_TRANS_CAL_RATE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szMessage, szWork, wRate);
    DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE2, szMessage);
    if (nCount == 0) {
        TransAbortClearRate(TransAbort.hDlg);
    } else {
        TransAbortDrawRate(TransAbort.hDlg, wRate);
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransAbortDlgCalcParaMsg()
*     Input:    hDlg     - handle of parent window
*               wCount   - loop counter
*               wTotal   - total size
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
short TransAbortDlgCalcParaMsg(HWND hWnd, WORD wCount, WORD wTotal)
{
    WCHAR    szWork[TRANS_MAX_MESSAGE_LEN];
    WCHAR    szMessage[TRANS_MAX_MESSAGE_LEN];
    WORD    wRate;

    wRate = (WORD)((WORD)(wCount * 100) / wTotal);
    LoadString(hResourceDll, IDS_TRANS_CAL_RATE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szMessage, szWork, wRate);
    DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE2, szMessage);
    if (wCount == 0) {
        TransAbortClearRate(TransAbort.hDlg);
    } else {
        TransAbortDrawRate(TransAbort.hDlg, wRate);
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransAbortDlgCalcLoadMsg()
*     Input:    hDlg     - handle of parent window
*               lpszFile - the pointer of file name
*               dwSize    - current rest of trans size
*               nCount   - loop counter
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
short TransAbortDlgCalcLoadMsg(HWND hWnd, DWORD dwSize, WORD wCount)
{
    static  DWORD   dwTotal;

    WCHAR    szWork[TRANS_MAX_MESSAGE_LEN];
    WCHAR    szMessage[TRANS_MAX_MESSAGE_LEN];
    WORD    wRate;

    if (dwSize == 0L) {
        return FALSE;
    }

    if (wCount == 0) {
        dwTotal = dwSize;
    }

    wRate = (WORD)((DWORD)((DWORD)((DWORD)wCount * TRANS_MEMORY_SIZE_LOADER) * 100L) / dwTotal);
    LoadString(hResourceDll, IDS_TRANS_CAL_RATE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szMessage, szWork, wRate);
    DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE2, szMessage);
    if (wCount == 0) {
        TransAbortClearRate(TransAbort.hDlg);
    } else {
        TransAbortDrawRate(TransAbort.hDlg, wRate);
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransAbortDlgFinMessage()
*     Input:    hDlg     - handle of parent window
*
** Return:      TRUE  - request user abort
*               FALSE - not request user abort
*
** Description:
*===========================================================================
*/
short TransAbortDlgFinMessage(HWND hWnd)
{
    WCHAR    szWork[TRANS_MAX_MESSAGE_LEN];
    WCHAR    szMessage[TRANS_MAX_MESSAGE_LEN];

    LoadString(hResourceDll, IDS_TRANS_CAL_RATE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szMessage, szWork, 100);
    DlgItemRedrawText(TransAbort.hDlg, IDD_TRANS_ABORT_MESSAGE2, szMessage);
    TransAbortDrawRate(TransAbort.hDlg, 100);
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL WINAPI TransAbortDlgProc(HWND hDlg,
*                                             WORD message,
*                                             WORD wParam,
*                                             LONG lParam)
*     Input:    hDlg    - Handle of Dialog Procedure
*               message - Dispathed Message
*               wParam  - 16 Bits Message Parameter
*               lParam  - 32 Bits Message Parameter
*
** Return:      TRUE  - User Process is Executed
*               FALSE - Default Process is Exepected
*
** Description: This function is dialog box procedure of Transfer Data.
*===========================================================================
*/
BOOL WINAPI TransAbortDlgProc(HWND hDlg, UINT message,
                                  WPARAM wParam, LPARAM lParam)
{
    WCHAR        szMessage[TRANS_MAX_MESSAGE_LEN],
                szTitle[128];
//    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);

    switch (message) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        LoadString(hResourceDll, IDS_TRANS_CAP_ABO, szTitle, PEP_STRING_LEN_MAC(szTitle));
        WindowRedrawText(hDlg, szTitle);
        TransAbortInitRate(hDlg);
        return TRUE;



    case WM_TIMER:
        if (SendDlgItemMessage(hDlg, IDD_TRANS_ABORT, BM_GETSTATE, 0, 0L)) {
            LoadString(hResourceDll, IDS_TRANS_PLEASE_WAIT, szMessage, PEP_STRING_LEN_MAC(szMessage));
            DlgItemRedrawText(hDlg, IDD_TRANS_ABORT_MESSAGE1, szMessage);

            /* ----- diable abort push botton ----- */
            EnableWindow(GetDlgItem(hDlg, IDD_TRANS_ABORT), FALSE);

            /* ----- set user abort flag ----- */
            TransAbort.wStatus = 1;
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_TRANS_ABORT_MESSAGE1; j<=IDD_TRANS_ABORT_FRAME; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_TRANS_ABORT_CAPTION1, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_TRANS_ABORT:
            LoadString(hResourceDll, IDS_TRANS_PLEASE_WAIT, szMessage, PEP_STRING_LEN_MAC(szMessage));
            DlgItemRedrawText(hDlg,
                           IDD_TRANS_ABORT_MESSAGE1,
                           szMessage);

            /* ----- diable abort push botton ----- */
            EnableWindow(GetDlgItem(hDlg, IDD_TRANS_ABORT), FALSE);

            /* ----- set user abort flag ----- */
            TransAbort.wStatus = 1;
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
** Synopsis:    VOID    TransAbortDrawRate()
*     Input:    hDlg     - handle of dialog
*               wRate    - tranfer rate
*
** Return:      None
*
** Description:
*===========================================================================
*/
VOID    TransAbortDrawRate(HWND hDlg, WPARAM wRate)
{
    WORD    wI;

    for (wI = wTransDrawPrev; wI < wRate; wI++) {
        EnableWindow(GetDlgItem(hDlg, IDD_TRANS_ABORT_RATE001 + wI), TRUE);
    }
    if (wRate == 100) {
        wTransDrawPrev = 0;
    } else {
        wTransDrawPrev = wRate;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    TransAbortClearRate()
*     Input:    hDlg     - handle of dialog
*               wRate    - tranfer rate
*
** Return:      None
*
** Description:
*===========================================================================
*/
VOID    TransAbortClearRate(HWND hDlg)
{
    WORD    wI;

    for (wI = 100; wI > 0; wI--) {
        EnableWindow(GetDlgItem(hDlg, IDD_TRANS_ABORT_RATE001 + wI - 1), FALSE);
    }
    wTransDrawPrev = 0;
}

/*
*===========================================================================
** Synopsis:    VOID    TransAbortInitRate()
*     Input:    hDlg     - handle of dialog
*               wRate    - tranfer rate
*
** Return:      None
*
** Description:
*===========================================================================
*/
VOID    TransAbortInitRate(HWND hDlg)
{
    WORD    wI;

    for (wI = 0; wI < 100; wI++) {
        DlgItemRedrawText(hDlg, IDD_TRANS_ABORT_RATE001 + wI, WIDE("\x7F"));//FIX THIS RATE ON TRANS DIALOG
    }
}

/* ===== End of TRAABORT.C ===== */