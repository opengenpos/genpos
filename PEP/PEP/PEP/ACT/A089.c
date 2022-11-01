/*
* ---------------------------------------------------------------------------
* Title         :   Set Currency Conversion Rates (AC 89)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A089.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jun-16-99 : 03.04.01 : hrkato     : Euro Enhancements
* Mar-22-00 : 01.00.01 : K.Yanagida : Saratoga
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
#include    <Windows.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a089.h"

typedef struct  _A89DATA {
    DWORD   dwInt;
    DWORD   dwDec;
} A89DATA, FAR *LPA89DATA;


/*
* ===========================================================================
**  Synopsis:   static  short   CheckFCMDC(short nID)
*
*   Input   :   short   nI; 	0-1, Foreign Currency ID
**
*   Return  :   TRUE/FALSE
*
**  Description:
*       This procedure indicates which of the two Foreign Currency conversion rate
*       formats to use.  The format depends on the settings for the Foreign Currency
*       tender as set in MDC 278 through MDC 281 (two currency tenders per MDC page).
* ===========================================================================
*/
static  short   CheckFCMDC(short nI)
{
	PARAMDC MDC = {0};
	struct {
		USHORT  usAddress;
		UCHAR   uchMask;
	} MdcSettings[] = {
			{MDC_EUROFC_ADR,  EVEN_MDC_BIT0},
			{MDC_EUROFC_ADR,  EVEN_MDC_BIT2},
			{MDC_EUROFC2_ADR, ODD_MDC_BIT0},
			{MDC_EUROFC2_ADR, ODD_MDC_BIT2},
			{MDC_EUROFC3_ADR, EVEN_MDC_BIT0},
			{MDC_EUROFC3_ADR, EVEN_MDC_BIT2},
			{MDC_EUROFC4_ADR, ODD_MDC_BIT0},
			{MDC_EUROFC4_ADR, ODD_MDC_BIT2}
		};

    MDC.usAddress = MdcSettings[nI].usAddress;
    ParaMDCRead(&MDC);

	if (MDC.uchMDCData & MdcSettings[nI].uchMask) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A089SetData()
*
**  Input       :   HWND    hDlg            -   Window Handle of a DialogBox
*                   LPDWORD lpdwCurrency    -   Address of Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the loaded initial data to each edittext with 
*   decimal type data.
* ===========================================================================
*/
VOID    A089SetData(HWND hDlg, LPDWORD lpdwCurrency)
{
    WCHAR   szWork[16];
    WCHAR   szRangePrompt[32];
    WORD    wID, wI;
    UINT    unInt;
    DWORD   dwDec;

    for (wI = 0, wID = IDD_A89EDIT1; wI < MAX_FC_SIZE; wI++, wID++) {
		SHORT  sFcOffset = (wID - IDD_A89EDIT1);
		/* ----- Make data for seeting to edit text ----- */
		if (! CheckFCMDC(sFcOffset)) {
			unInt = (UINT) (*(lpdwCurrency + wI) / A89_CNVLONG);
			dwDec = (DWORD)(*(lpdwCurrency + wI) % A89_CNVLONG);
			wsPepf(szWork, WIDE("%u.%05lu"), unInt, dwDec);
			wcscpy (szRangePrompt, L"0.00001  - 9999.99999");
		} else {
            unInt = (UINT) (*(lpdwCurrency + wI) / A89_CNVLONG2);
            dwDec = (DWORD)(*(lpdwCurrency + wI) % A89_CNVLONG2);
            wsPepf(szWork, WIDE("%u.%06lu"), unInt, dwDec);
			wcscpy (szRangePrompt, L"0.000001 -  999.999999");
		}
        /* ----- Set data to editbox ----- */
        DlgItemRedrawText(hDlg, wID, szWork);
        DlgItemRedrawText(hDlg, IDD_A89EDRANGE1 + sFcOffset, szRangePrompt);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A089ConvData()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   WORD        wID         -   Target EditText ID
*                   LPCSTR      lpszSource  -   Address of Inputed Data (Text)
*                   LPA89DATA   lpData      -   Address of Data Buffer
*
**  Output      :   DWORD   Data->dwInt     -   Integer Part of Inputed Data
*                   DWORD   Data->dwDec     -   Under Decimal Point Part of Data
*
**  Return      :   TRUE    - user process is executed
*                   FALSE   - error occured 
*
**  Description :
*       This function converts the text type data to long type data.
* ===========================================================================
*/
BOOL    A089ConvData(HWND hDlg, WPARAM wID, LPWSTR lpszSource, LPA89DATA lpData)
{
    DWORD    dwConv;
    LPWSTR   lpszDecPt;
    WCHAR    szInt[A89_LIMITLEN + 1], szDec[A89_LIMITLEN + 1];
    WORD     wI, nLen;
    WCHAR    szErrMsg[A89_ERR_LEN], szCaption[PEP_CAPTION_LEN];

    /* ----- Reset Work Area ----- */
    memset(szInt, 0, sizeof(szInt));
    memset(szDec, 0, sizeof(szDec));

    /* ----- Get Decimal Point from Target Text Data ----- */
    lpszDecPt = wcschr(lpszSource, A89_DECPNT);
    if (lpszDecPt == NULL) {    /* cannot find decimal point */
        /* ----- Set decimal point address ----- */
        lpszDecPt = (lpszSource + A89_LIMITLEN);
    } else {
        if (! CheckFCMDC((short)(wID - IDD_A89EDIT1))) {
            if (wcslen((lpszDecPt + 1)) > A89_DECCOLUMN) {

                /* ----- Load Error Message from Resource ----- */
                LoadString(hResourceDll, IDS_PEP_CAPTION_A89, szCaption, PEP_STRING_LEN_MAC(szCaption));
                LoadString(hResourceDll, IDS_A89_CLMOVR, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

                /* ----- Display Error Message ----- */
                MessageBeep(MB_ICONEXCLAMATION);
                MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_ICONEXCLAMATION | MB_OK);

                /* ----- Undo Data Input ----- */
                SendDlgItemMessage(hDlg, wID, EM_UNDO, 0, 0L);

                /* ----- Set Foucus to Error Data ----- */
                SetFocus(GetDlgItem(hDlg, wID));
                SendDlgItemMessage(hDlg, wID, EM_SETSEL, 1, MAKELONG(0, -1));
                return FALSE;
            }

            /* ----- Copy Under Decimal Point Data to Work Area ----- */
			wcsncpy(szDec, (lpszDecPt + 1), A89_DECCOLUMN);//SR 390
		} else {
            if (wcslen((lpszDecPt + 1)) > A89_DECCOLUMN2) {
                /* ----- Load Error Message from Resource ----- */
                LoadString(hResourceDll, IDS_PEP_CAPTION_A89, szCaption, PEP_STRING_LEN_MAC(szCaption));
                LoadString(hResourceDll, IDS_A89_CLMOVR2, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
                /* ----- Display Error Message ----- */
                MessageBeep(MB_ICONEXCLAMATION);
                MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_ICONEXCLAMATION | MB_OK);

                /* ----- Undo Data Input ----- */
                SendDlgItemMessage(hDlg, wID, EM_UNDO, 0, 0L);

                /* ----- Set Foucus to Error Data ----- */
                SetFocus(GetDlgItem(hDlg, wID));
                SendDlgItemMessage(hDlg, wID, EM_SETSEL, 1, MAKELONG(0, -1));
                return FALSE;
			}
            /* ----- Copy Under Decimal Point Data to Work Area ----- */
			wcsncpy(szDec, (lpszDecPt + 1), A89_DECCOLUMN2); //SR 390
        }
    }

    /* ----- Copy Upper Decimal Point Data to Work Area ----- */
	wcsncpy(szInt, lpszSource, (lpszDecPt - lpszSource));//SR 390

    /* ----- Convert ASCII data to long  ----- */
    lpData->dwInt = _wtol(szInt);
    lpData->dwDec = _wtol(szDec);

    /* ----- Adjust for data without 4 column ----- */
	if (! CheckFCMDC((short)(wID - IDD_A89EDIT1))) {
		nLen = A89_DECCOLUMN + 1;
		dwConv = A89_CNVLONG;

	} else {
		nLen = A89_DECCOLUMN2 + 1;
		dwConv = A89_CNVLONG2;
	}

    for (wI = 0; wI < nLen; wI++) {
        if (szDec[wI] == 0) {
            break;
        } else {
            dwConv /= A89_CNVDEC;
        }
    }
    lpData->dwDec *= dwConv;

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A089GetData()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   WORD        wEditID     -   Target EditText ID
*                   LPA89DATA   lpData      -   Address of Data Buffer
*
**  Output      :   DWORD   lpData->dwInt   -   Integer Part of Inputed Data
*                   DWORD   lpData->dwDec   -   Under Decimal Point Part of Data
*
**  Return      :   TRUE    - user process is executed
*                   FALSE   - error occured 
*
**  Description :
*       This function gets the inputed data from the target edittext. And it
*   returns the address of inputed data structure.
* ===========================================================================
*/
BOOL    A089GetData(HWND hDlg, WPARAM wEditID, LPA89DATA lpData)
{
	WCHAR        szWork[16] = {0};

    /* ----- Get Inputed Data As Text ----- */
    DlgItemGetText(hDlg, wEditID, szWork, sizeof(szWork));

    /* ----- Convert the Inputed Data to Long Type ----- */
    return (A089ConvData(hDlg, wEditID, szWork, lpData));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A089ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This function determines whether the inputed data is out of range or
*   not. It displays the messagebox for shows the error message, if the data
*   is out of range.
* ===========================================================================
*/
VOID    A089ChkRng(HWND hDlg, WORD wEditID)
{
    A89DATA     Data;
    WCHAR       szErrMsg[A89_ERR_LEN], szCaption[PEP_CAPTION_LEN];

    /* ----- Get Inputed Data from EditText ----- */
    if (A089GetData(hDlg, wEditID, (LPA89DATA)&Data) == FALSE) {
        return;
    }

    /* ----- Determine Whether the Data is out of Range or Not ----- */
    if (! CheckFCMDC((short)(wEditID - IDD_A89EDIT1))) {
        if (Data.dwInt > A89_MAX) { /* Data Range Over */

            /* ----- Load Error Message from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A89, szCaption, PEP_STRING_LEN_MAC(szCaption));
            LoadString(hResourceDll, IDS_A89_RNGOVR, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

            /* ----- Display Error Message ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_ICONEXCLAMATION | MB_OK);

            /* ----- Undo Data Input ----- */
            SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

            /* ----- Set Foucus to Error Data ----- */
            SetFocus(GetDlgItem(hDlg, wEditID));
            SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
        }
	} else {
        if (Data.dwInt >  A89_MAX2) {
            /* ----- Load Error Message from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A89, szCaption, PEP_STRING_LEN_MAC(szCaption));
            LoadString(hResourceDll, IDS_A89_RNGOVR2, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

            /* ----- Display Error Message ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_ICONEXCLAMATION | MB_OK);

            /* ----- Undo Data Input ----- */
            SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

            /* ----- Set Foucus to Error Data ----- */
            SetFocus(GetDlgItem(hDlg, wEditID));
            SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
		}
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A089FinDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from edittext and converts the text
*   type data to long type data. And then it writes them to the Parameter file.
* ===========================================================================
*/
VOID    A089FinDlg(HWND hDlg)
{
    DWORD       adwCurrency[MAX_FC_SIZE];
    USHORT      usRet;
    WORD        wEditID, wI;
    A89DATA     Data;

    for (wI = 0, wEditID = IDD_A89EDIT1; wI < MAX_FC_SIZE; wI++, wEditID++) {
        /* ----- Get Inputed Data from EditText ----- */
        A089GetData(hDlg, wEditID, (LPA89DATA)&Data);

        /* ----- Convert Data to Save Parameter File ----- */
        if (! CheckFCMDC((short)(wEditID - IDD_A89EDIT1))) {
            adwCurrency[wI] = Data.dwInt * A89_CNVLONG + Data.dwDec;
        } else {
            adwCurrency[wI] = Data.dwInt * A89_CNVLONG2 + Data.dwDec;
        }
    }

    /* ----- Write Inputed Data to Parameter File ----- */
    ParaAllWrite(CLASS_PARACURRENCYTBL, (UCHAR *)adwCurrency, sizeof(adwCurrency), 0, &usRet);

    /* ----- Set File Status Flag ----- */
    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A089InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from the Parameter file, and then
*   it sets them to each edittext with modified decimal type data.
* ===========================================================================
*/
VOID    A089InitDlg(HWND hDlg)
{
    DWORD   adwCurrency[MAX_FC_SIZE];
    USHORT  usRet;
    WORD    wID;

    /* ----- Read Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARACURRENCYTBL, (UCHAR *)adwCurrency, sizeof(adwCurrency), 0, &usRet);

    /* ----- Limit the Length of Input Data ----- */
    for (wID = IDD_A89EDIT1; wID <= IDD_A89EDIT8; wID++) {
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A89_LIMITLEN, 0L);
    }

    /* ----- Set the Loaded Data to Each EditText ----- */
    A089SetData(hDlg, (LPDWORD)adwCurrency);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A089DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*               Set Currency Conversion Rates (Action Code # 89)
* ===========================================================================
*/
BOOL    WINAPI  A089DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize DialogBox ----- */
        A089InitDlg(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_A89EDIT1; j <= IDD_A89_ADD8; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j = IDD_A89EDRANGE1; j <= IDD_A89EDRANGE8; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A89EDIT1:
        case IDD_A89EDIT2:
        case IDD_A89EDIT3:	/* ###ADD Saratoga */
        case IDD_A89EDIT4:      /* ###ADD Saratoga */
        case IDD_A89EDIT5:      /* ###ADD Saratoga */
        case IDD_A89EDIT6:      /* ###ADD Saratoga */
        case IDD_A89EDIT7:      /* ###ADD Saratoga */
        case IDD_A89EDIT8:      /* ###ADD Saratoga */
            if(HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check range over ----- */
                A089ChkRng(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Finalize dialogbox ----- */
                    A089FinDlg(hDlg);
                }
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/* ===== End of A089.C ===== */