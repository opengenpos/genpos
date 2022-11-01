/*
* ---------------------------------------------------------------------------
* Title         :   Set Tax Rates (AC 127)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A127.C
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
#include    "a127.h"

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
**  Synopsis    :   BOOL    WINAPI  A127DlgProc()
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
*                       Set Tax Rates (Action Code # 127)
* ===========================================================================
*/
BOOL    WINAPI  A127DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  BYTE    bTaxMode;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize DialogBox ----- */
        A127InitDlg(hDlg, (LPBYTE)&bTaxMode);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A127MODE; j<=IDD_A127RNG5; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A127EDIT1:
        case IDD_A127EDIT2:
        case IDD_A127EDIT3:
        case IDD_A127EDIT4:
        case IDD_A127EDIT5:
            if(HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Data Range ----- */
                A127ChkRng(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Finalize DialogBox ----- */
                    A127FinDlg(hDlg, bTaxMode);
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

/*
* ===========================================================================
**  Synopsis    :   VOID    A127InitDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPBYTE  lpbTaxMode  -   Address of Tax Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from the Parameter file, and checks
*   the current tax mode. And then it sets the loaded data to each edittext.
* ===========================================================================
*/
VOID    A127InitDlg(HWND hDlg, LPBYTE lpbTaxMode)
{
    DWORD   adwTaxRate[MAX_TAXRATE_SIZE];
    USHORT  usReturnLen;
    UINT    wStrID = 0;
    WORD    wLength;
    WORD    wI;
    WCHAR   szDesc[128];
    int     nCmdShow1 = SW_SHOW;
    int     nCmdShow2 = SW_SHOW;
    BYTE    bTaxCheck1, bTaxCheck2;

    /* ----- Read Initial Data from File ----- */
    ParaAllRead(CLASS_PARATAXRATETBL, (UCHAR *)adwTaxRate, sizeof(adwTaxRate), 0, &usReturnLen);

    /* ----- Get Current Tax Mode (US/Canada/VAT) ----- */
	// Tax selection is specified using two different MDC addresses.  MDC 15 and MDC 11.
	// Both of these are odd addresses so we use the lower nibble to check the setting of Bit D.
	bTaxCheck1 = ParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0);
	bTaxCheck2 = ParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0);

    *lpbTaxMode = (BYTE)(!(bTaxCheck1) ? TAX_USA  : !(bTaxCheck2) ? TAX_CANADA : TAX_VAT);

    /* ----- Load String from Resource ----- */
    switch (*lpbTaxMode) {
    case TAX_USA:
        wStrID = IDS_A127_USMODE;
        break;

    case TAX_CANADA:
        wStrID = IDS_A127_CANMODE;
        break;

    case TAX_VAT:
        wStrID = IDS_A127_VATMODE;
        break;
    }
    
    /* ----- Set Current Tax Mode Description to StaticText ----- */
    LoadString(hResourceDll, wStrID, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A127MODE, szDesc);

    /* ----- Set Address String ID with Current Tax Mode ----- */
    switch (*lpbTaxMode) {
    case TAX_USA:
        wStrID = IDS_A127_USADDR1;
        break;

    case TAX_CANADA:
        wStrID = IDS_A127_CANADDR1;
        break;

    case TAX_VAT:
        wStrID = IDS_A127_VATADDR1;
        break;
    }

    for (wI = 0; wI < MAX_TAXRATE_SIZE; wI++) {
        /* ----- Limit the Max. Length of Input Data ----- */
        wLength = (WORD)((wI == A127_NO5_ADR) ? A127_COLUMN5 : A127_COLUMN);
        SendDlgItemMessage(hDlg, IDD_A127EDIT1 + wI, EM_LIMITTEXT, wLength, 0L);

        /* ----- Load Address Description from Resource ----- */
        LoadString(hResourceDll, wStrID + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A127DSC1 + wI, szDesc);
    }

    /* ----- Show/Hide Address, EditText and Data Range by Tax Mode ----- */
    switch (*lpbTaxMode) {
    case TAX_USA:
        nCmdShow1 = SW_HIDE;
        nCmdShow2 = SW_HIDE;
        break;

    case TAX_CANADA:
        nCmdShow1 = SW_SHOW;
        nCmdShow2 = SW_SHOW;
        break;

    case TAX_VAT:
        nCmdShow1 = SW_SHOW;
        nCmdShow2 = SW_HIDE;
        break;
    }

    ShowWindow(GetDlgItem(hDlg, IDD_A127ADDR4), nCmdShow1);
    ShowWindow(GetDlgItem(hDlg, IDD_A127ADDR5), nCmdShow2);
    ShowWindow(GetDlgItem(hDlg, IDD_A127EDIT4), nCmdShow1);
    ShowWindow(GetDlgItem(hDlg, IDD_A127EDIT5), nCmdShow2);
    ShowWindow(GetDlgItem(hDlg, IDD_A127RNG4),  nCmdShow1);
    ShowWindow(GetDlgItem(hDlg, IDD_A127RNG5),  nCmdShow2);
    ShowWindow(GetDlgItem(hDlg, IDD_A127DSC4), nCmdShow1);
    ShowWindow(GetDlgItem(hDlg, IDD_A127DSC5), nCmdShow2);

    /* ----- Set Loaded Data to Each EditText ----- */
    A127SetData(hDlg, (LPDWORD)adwTaxRate, *lpbTaxMode);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A127SetData()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPDWORD lpdwTax     -   Address of Tax Data
*                   BYTE    bTaxMode    -   Current Tax Mode Flag
*
**  Return      :   No return value
*
**  Description :
*       This function sets the loaded tax data to each edittext.
* ===========================================================================
*/
VOID    A127SetData(HWND hDlg, LPDWORD lpdwTax, BYTE bTaxMode)
{
    WCHAR   szWork[16];
    WORD    wI;
    WORD    wNoAddr = 0;
    UINT    unInt, unDec;

    /* ----- Determine Number of Address by Current Tax Mode ----- */
    switch (bTaxMode) {
    case TAX_USA:
        wNoAddr = A127_NOADDRUS;
        break;

    case TAX_CANADA:
        wNoAddr = MAX_TAXRATE_SIZE;
        break;

    case TAX_VAT:
        wNoAddr = A127_NOADDRUS2;
        break;
    }
    
    for (wI = 0; wI < wNoAddr; wI++) {
        /* ----- Convert Long Type Tax Data to Text Type Data ----- */
        if (wI == A127_NO5_ADR) {
            unInt = (UINT)*(lpdwTax + wI);
            wsPepf(szWork, WIDE("%u"), unInt);
        } else {
            unInt = (UINT)(*(lpdwTax + wI) / A127_CNVLONG);
            unDec = (UINT)(*(lpdwTax + wI) % A127_CNVLONG);
            wsPepf(szWork, WIDE("%2u.%04u"), unInt, unDec);
        }

        /* ----- Set Created Tax Data to Each EditText ----- */
        DlgItemRedrawText(hDlg, IDD_A127EDIT1 + wI, szWork);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A127ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*                   BYTE    bTaxMode    -   Current Tax Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function checks the current inputed data with data range. If the
*   inputed data is out of range, it displays the error message wth messagebox.
* ===========================================================================
*/
VOID    A127ChkRng(HWND hDlg, WORD wEditID)
{
    A127DATA    Data;
    UINT        wStrID;
    DWORD       dwMax;

    /* ----- Get Inputed Data from EditText ----- */
    if (A127GetData(hDlg, wEditID, (LPA127DATA)&Data) == FALSE) {
        return;
    }

    /* ---- Determine the Max. Value of Data Range ----- */
    if (wEditID == IDD_A127EDIT5) {
        dwMax = A127_DATAMAX5;
    } else {
        dwMax = A127_DATAMAX;
    }

    /* ----- Check Inputed Data is Out of Range ----- */
    if (Data.dwInt > dwMax) { /* Data Range Over */
		WCHAR       szCap[PEP_CAPTION_LEN], szMsg[A127_ERR_LEN];
        /* ----- Set Target String ID ----- */
        if (wEditID == IDD_A127EDIT5) {
            wStrID = IDS_A127_RNGOVER5;
        } else {
            wStrID = IDS_A127_RNGOVER;
        }

        LoadString(hResourceDll, IDS_PEP_CAPTION_A127, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, wStrID, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message with MessageBox ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

        /* ----- Undo Input Data ----- */
        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error EditText ----- */
        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A127GetData()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   WORD        wEditID     -   Target EditText ID
*                   LPA127DATA  lpData      -   Address of Data Buffer
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Invalid.
*                           FALSE   -   Inputed Data is Valid.
*
**  Description :
*       This function gets the inputed data from edittext and sets them to
*   the tax data buffer.
* ===========================================================================
*/
BOOL    A127GetData(HWND hDlg, WPARAM wEditID, LPA127DATA lpData)
{
    WCHAR   szWork[128];
    BOOL    fRet;

    if (LOWORD(wEditID) == IDD_A127EDIT5) {
		// field 5 of tax entry is for tax exempt limit for Candian tax. it is special.
		UINT    unValue, unLen;
		BOOL    fTrans;
		/* ----- Get inputed data value ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A127EDIT5, (LPBOOL)&fTrans, FALSE);   

        /* ----- Get inputed data length ----- */
        unLen = (UINT)SendDlgItemMessage(hDlg, IDD_A127EDIT5, WM_GETTEXTLENGTH, 0, 0L);   
        if ((fTrans == 0) && (unLen != 0)) {
			WCHAR   szDesc[128], szCap[128];

            /* ----- Make error message ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A127, szCap, PEP_STRING_LEN_MAC(szCap));
            LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
            wsPepf(szDesc, szWork, (WORD)A127_DATAMIN, A127_DATAMAX5);

            /* ----- Display MessageBox ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szDesc, szCap, MB_ICONEXCLAMATION | MB_OK);

            /* ----- Undo Input Data ----- */
            SendDlgItemMessage(hDlg, IDD_A127EDIT5, EM_UNDO, 0, 0L);

            /* ----- Set Input Focus to EditText ----- */
            SendDlgItemMessage(hDlg, IDD_A127EDIT5, EM_SETSEL, 1, MAKELONG(0, -1));

            fRet = FALSE;
        } else {
            lpData->dwInt = (DWORD)unValue;
            lpData->dwDec = 0L;
            fRet = TRUE;
        }
    } else {
        /* ----- Get Inputed Data from EditText As Text Type ----- */
        DlgItemGetText(hDlg, wEditID, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Convert the Inputed Data to Long Type ----- */
        fRet = A127ConvData(hDlg, wEditID, szWork, lpData);
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A127ConvData()
*
**  Input       :   LPCSTR      lpszSource  -   Address of Inputed Tax Data
*                   WORD        wID         -   Target EditText ID
*                   LPCSTR      lpszSource  -   Address of Inputed Data (Text)
*                   LPA127DATA  lpData      -   Address of Data Buffer
*
**  Output      :   DWORD   Data->dwInt     -   Integer Part of Inputed Data
*                   DWORD   Data->dwDec     -   Under Decimal Point Part of Data
*
**  Return      :   TRUE    - user process is executed
*                   FALSE   - error occured 
*
**  Description :
*       This function converts the text type tax data (including decimal point)
*   to the DWORD type tax data (not including decimal point).
* ===========================================================================
*/
BOOL    A127ConvData(HWND hDlg, WPARAM wID, LPWSTR lpszSource, LPA127DATA lpData)
{
    DWORD    dwConv;
    LPWSTR   lpszDecPt;
	WCHAR    szInt[A127_COLUMN + 1] = {0};
	WCHAR    szDec[A127_COLUMN + 1] = {0};
    WORD     wI;

    /* ----- Get Decimal Point from Target Text Data ----- */
    lpszDecPt = wcschr(lpszSource, A127_DECPNT);

    if (lpszDecPt == NULL) {    /* cannot find decimal point */
        /* ----- Set decimal point address ----- */
        lpszDecPt = (lpszSource + A127_COLUMN);
    } else {
        if (wcslen((lpszDecPt + 1)) > A127_DECCOLUMN) {
			WCHAR    szErrMsg[A127_ERR_LEN], szCaption[PEP_CAPTION_LEN];

            /* ----- Load Error Message from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A127, szCaption, PEP_STRING_LEN_MAC(szCaption));
            LoadString(hResourceDll, IDS_A127_CLMOVER, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

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
        wcsncpy(szDec, (lpszDecPt + 1), wcslen((lpszDecPt + 1)));//SR 450, was only copying 2 of the required bytes JHHJ
    }

    /* ----- Copy Upper Decimal Point Data to Work Area ----- */
     wcsncpy(szInt, lpszSource, (lpszDecPt - lpszSource));

    /* ----- Convert ASCII data to long  ----- */
    lpData->dwInt = _wtol(szInt) * A127_CNVLONG;
    lpData->dwDec = _wtol(szDec);

    /* ----- Adjust for data without 4 column ----- */
    for (wI = 0, dwConv = A127_CNVLONG; wI < A127_COLUMN + 1; wI++) {
        if (szDec[wI] == 0) {
            break;
        } else {
            dwConv /= A127_CNVDEC;
        }
    }
    lpData->dwDec *= dwConv;

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A127FinDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   BYTE    bTaxMode    -   Current Tax Mode Flag
*
**  Return      :   None
*
**  Description :
*       This function gets the inputed data from each edittext and them saves
*   them to Parameter file.
* ===========================================================================
*/
VOID    A127FinDlg(HWND hDlg, BYTE bTaxMode)
{
	DWORD       adwTaxRate[MAX_TAXRATE_SIZE] = {0};
    USHORT      usReturnLen;
    WORD        wI, wMax = 0;
    A127DATA    Data;

    switch (bTaxMode) {
    case TAX_USA:
        wMax = A127_NOADDRUS;
        break;

    case TAX_CANADA:
        wMax = MAX_TAXRATE_SIZE;
        break;

    case TAX_VAT:
        wMax = A127_NOADDRUS2;
        break;
    }

    for (wI = 0; wI < wMax; wI++) {
        /* ----- Get Inputed Data from EditText ----- */
        A127GetData(hDlg, (IDD_A127EDIT1 + wI), (LPA127DATA)&Data);
        /* ----- Convert Data to Save Parameter File ----- */
        adwTaxRate[wI] = Data.dwInt + Data.dwDec;
    }

    /* ----- Write Inputed Data to Parameter File ----- */
    ParaAllWrite(CLASS_PARATAXRATETBL, (UCHAR *)adwTaxRate, sizeof(adwTaxRate), 0, &usReturnLen);

    /* ----- Set File Status Flag ----- */
    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
}

/* ===== End of File (A127.C) ===== */
