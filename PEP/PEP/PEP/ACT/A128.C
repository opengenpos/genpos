/*
* ---------------------------------------------------------------------------
* Title         :   Set Transaction Limit (AC 128)
* Category      :   Maintenance, NCR 2170 PEP for Windows (GP US Model)
* Program Name  :   A128.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* May-21-93 : 00.00.01 : T.Nakahata : Initial
* Jun-29-93 : 00.00.02 : T.Nakahata : Change Modify Flag Name
* Nov-16-98 : 02.01.02 : A.Mitsui   : Change Data Minimum (1 -> 0)
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
#include    "a128.h"

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
**  Synopsis:   static  void    DispError();
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               WPARAM      wID       - editbox ID
*
**  Return  :   No Return Value
*
**  Description:
*       This procedure displays Error MessageBox. And Undo data input.
* ===========================================================================
*/
static  void DispError(HWND hDlg, WORD wEditID)
{
    UINT    wString;
    WCHAR   szCaption[PEP_CAPTION_LEN], szErrMsg[PEP_OVER_LEN];

    /* ----- Set Target String ID ----- */
    wString = ((wEditID == IDD_A128EDIT1) ? IDS_A128_RNGOVER_1 : IDS_A128_RNGOVER_2);

    /* ----- Load Strings from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A128, szCaption, PEP_STRING_LEN_MAC(szCaption));
    LoadString(hResourceDll, wString, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

    /* ----- Display MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Undo Data Input ----- */
    SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);
    SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   static  VOID    SetData()
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               ULONG FAR   *lpulData - address of data buffer
*
**  Return  :   None
*
**  Description:
*       This procedure set data to editbox.
* ===========================================================================
*/
static  VOID    SetData(HWND hDlg, ULONG FAR *lpulData)
{
	WORD    wI, wID; 
    DWORD   dwInt;
    WCHAR   szWork[A128_DATA_LEN];

	memset(szWork, 0x00, sizeof(szWork));

	if (lpulData[A128_CC_BITS] & A128_CC_ENABLE)
	{
		CheckDlgButton(hDlg, IDD_A128ENABLE, TRUE);

		EnableWindow(GetDlgItem(hDlg,IDD_A128EDIT8),   TRUE);
		EnableWindow(GetDlgItem(hDlg,IDD_A128CUST),    TRUE); 
		EnableWindow(GetDlgItem(hDlg,IDD_A128MERC),    TRUE);
		EnableWindow(GetDlgItem(hDlg,IDD_A128CUST_AUTH),    TRUE); 
		EnableWindow(GetDlgItem(hDlg,IDD_A128MERC_AUTH),    TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDD_A128EDIT8),   FALSE);
		EnableWindow(GetDlgItem(hDlg,IDD_A128CUST),    FALSE); 
		EnableWindow(GetDlgItem(hDlg,IDD_A128MERC),    FALSE);
		EnableWindow(GetDlgItem(hDlg,IDD_A128CUST_AUTH),    FALSE); 
		EnableWindow(GetDlgItem(hDlg,IDD_A128MERC_AUTH),    FALSE);
	}
	
	if ((lpulData[A128_CC_BITS] & A128_CC_CUST))
	{
		CheckDlgButton(hDlg, IDD_A128CUST, TRUE);
	}
	if ((lpulData[A128_CC_BITS] & A128_CC_CUST_AUTH))
	{
		CheckDlgButton(hDlg, IDD_A128CUST_AUTH, TRUE);
	}
	
	if ((lpulData[A128_CC_BITS] & A128_CC_MERC))
	{
		CheckDlgButton(hDlg, IDD_A128MERC, TRUE);
	}
	if ((lpulData[A128_CC_BITS] & A128_CC_MERC_AUTH))
	{
		CheckDlgButton(hDlg, IDD_A128MERC_AUTH, TRUE);
	}

    for (wI = 0, wID = IDD_A128EDIT1; wI < A128_ADDR_MAX; wI++, wID++) {
		// The following code makes sure that the space reserved for the 
		// status bit info is not overwritten.   ***PDINU
		if (wID ==  A128_CC_INFO)
			continue;

        /* ----- Calculate data ----- */
        dwInt  = (DWORD)(*(lpulData + wI));

        /* ----- Make data to set ----- */
        wsPepf(szWork, WIDE("%lu"), dwInt);

        /* ----- Set data to editbox ----- */
        DlgItemRedrawText(hDlg, wID, szWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    GetData()
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               ULONG FAR   *lpulData - address of data buffer
*
**  Return  :   TRUE    -   User Process is Executed
*               FALSE   -   data is out of range.
*
**  Description:
*       This procedure get data from editbox.
* ===========================================================================
*/
static  BOOL    GetData(HWND hDlg, ULONG FAR *lpulData)
{
    DWORD   dwValue;
    WORD    wI, wID;
    BOOL    fCheck = TRUE;
    WCHAR    szWork[A128_DATA_LEN];

    for (wI = 0, wID = IDD_A128EDIT1; wI < A128_ADDR_MAX; wI++, wID++) {
		// The following code makes sure that the space reserved for the 
		// status bit info is not overwritten.   ***PDINU
		if (wID ==  A128_CC_INFO)
			continue;

        /* ----- Get data as text ----- */
        memset(szWork, 0, sizeof(szWork));

        DlgItemGetText(hDlg, wID, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Convert String to unsigned long value ----- */
        dwValue = ConvData(szWork, wID);

        if (! ChkRng(hDlg, wID, IDOK)) {
            /* ----- Display Error Message ----- */
            DispError(hDlg, wID);
            SetFocus(GetDlgItem(hDlg, wID));
            fCheck = FALSE;
            break;
        } else {
            /* ----- Set Inputed Data to Buffer ----- */
            *(lpulData + wI) = dwValue;
        }
    }
    return fCheck;
}

/*
* ===========================================================================
**  Synopsis:   static  DWORD   ConvData()
*
**  Input   :   LPSTR   lpszSource - address of source data
*               WPARAM  wID        - EditText ID
*
**  Return  :   DWORD   dwData     - decimal type data
*
**  Description:
*       This procedure get decimal type data.
* ===========================================================================
*/
static  DWORD   ConvData(LPWSTR lpszSource, WPARAM wID)
{
    WORD    wI, wLen, wLoop;
    DWORD   dwDec, dwInt, dwConv = A128_CNVLONG;
    WCHAR   szInt[A128_DATA_LEN], szDec[A128_DATA_LEN];

    /* ----- Reset work area ----- */
    memset(szInt, 0, sizeof(szInt));

    memset(szDec, 0, sizeof(szDec));

    /* ----- Set Loop Counter ----- */
    wLoop = (WORD)((wID == IDD_A128EDIT1) ? A128_COLUMN_1 : A128_COLUMN_2);

    /* ----- Get Decimal Point Position ----- */
    for (wI = 0; wI < wLoop; wI++) {
        if (*(lpszSource + wI) == A128_DECPNT) {
            /* ----- Copy Under Decimal Point Data to Buffer ----- */
			wcsncpy(szDec, (lpszSource + wI + 1), (wLoop - wI - 1));
            break;
        }
    }

    /* ----- Copy Upper Decimal Point Data to Buffer ----- */
	wcsncpy(szInt,lpszSource, wI);

    /* ----- Check Inputed Data wheter Signed or Unsigned ----- */
    if (_wtol(szInt) < A128_DATA_MIN) {
        dwInt = A128_DATA_MIN;
        dwDec = A128_DATA_MIN;
    } else {
        /* ----- Convert String (Upper Decimal Point) to Long Type ----- */
        dwInt = _wtol(szInt) * A128_CNVLONG;

        /* ----- Get Length of Under Decimal Point data ----- */
        wLen = (WORD)wcslen(szDec);

        /* ----- Adjust under decimal point data ----- */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        for (wI = wLen; wI < A128_DECCOLUMN; wI++) {

            strcat(szDec, "0");
        }
#endif

        /* ----- Convert String (Under decimal Point) to long Type ----- */
        dwDec = _wtol(szDec);
    }
    return (dwInt/* + dwDec*/);
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    ChkRng()
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               WPARAM      wEditID   - editbox ID
*               WPARAM      wType     - check type
*
**  Return  :   TRUE             - user process is executed
*               FALSE            - range over
*
**  Description:
*       This procedure check whether data over range.
* ===========================================================================
*/
static  BOOL    ChkRng(HWND hDlg, WORD wEditID, WPARAM wType)
{
    DWORD   dwValue, dwMax;
    BOOL    fCheck = TRUE;
    WCHAR    szMiscData[A128_DATA_LEN];

    /* ----- Get Inputed Value from EditText ----- */
    memset(szMiscData, 0, sizeof(szMiscData));

    DlgItemGetText(hDlg, wEditID, szMiscData, PEP_STRING_LEN_MAC(szMiscData));

    dwValue = ConvData(szMiscData, wEditID);

    /* ----- Check range over ----- */
    dwMax = (DWORD)((wEditID == IDD_A128EDIT1) ? A128_DATA_MAX1 : A128_DATA_MAX2);

    switch (LOWORD(wType)) {

    case IDOK:
        if (dwValue > dwMax) {
            fCheck = FALSE;
        }
        break;

    default:
        if (dwValue > dwMax) {
            fCheck = FALSE;
        }
    }
    return fCheck;
}


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  A128DlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialgbox procedure for 
*                  Set Misc. Limitation Data (AC 128)
* ===========================================================================
*/
BOOL    WINAPI  A128DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  ULONG              aulParaDisplay[MISC_ADR_MAX * 2];
    static  ULONG              aulParaMisc[MISC_ADR_MAX];
    static  ULONG              myParaStoreForward[MISC_ADR_MAX];

    USHORT  usReturnLen;
    WCHAR   szDesc[PEP_OVER_LEN];
    WORD    wID, wLimit;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize dialogbox ----- */
		/* ----- set initial description, R2.0 ----- */
		LoadString(hResourceDll, IDS_PEP_CAPTION_A128, szDesc, PEP_STRING_LEN_MAC(szDesc));
		WindowRedrawText(hDlg, szDesc);

		LoadString(hResourceDll, IDS_A128_GRP_FOOD, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_GRP_FOOD, szDesc);
		LoadString(hResourceDll, IDS_A128_GRP_CPN, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_GRP_CPN, szDesc);

		LoadString(hResourceDll, IDS_A128_ADDR1, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR1, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR2, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR2, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR3, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR3, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR4, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR4, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR5, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR5, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR6, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR6, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR7, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR7, szDesc);

		LoadString(hResourceDll, IDS_A128_ADDR8, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR8, szDesc);

		// CLASS_PARASTOREFORWARD begins with address 10
		LoadString(hResourceDll, IDS_A128_GRP_SF, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_GRP_SF, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR10, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR10, szDesc);
		LoadString(hResourceDll, IDS_A128_ADDR11, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A128_ADDR11, szDesc);

		for (wID = IDD_A128EDIT1; wID <= IDD_A128EDIT11; wID++) {
			/* ----- Set Maximum Length of Input Data ----- */
			wLimit = (WORD)((wID == IDD_A128EDIT1) ? A128_COLUMN_1 : A128_COLUMN_2);
			SendDlgItemMessage(hDlg, wID++, EM_LIMITTEXT, wLimit, 0L);
		}

		/* ----- Read data from file ----- */
		memset (aulParaMisc, 0, sizeof(aulParaMisc));
		ParaAllRead(CLASS_PARAMISCPARA, (UCHAR *)aulParaMisc, sizeof(aulParaMisc), 0, &usReturnLen);

		memset (myParaStoreForward, 0, sizeof(myParaStoreForward));
		ParaAllRead(CLASS_PARASTOREFORWARD, (UCHAR *)&myParaStoreForward, sizeof(myParaStoreForward), 0, &usReturnLen);

		memcpy (aulParaDisplay, aulParaMisc, sizeof(aulParaMisc));
		memcpy (aulParaDisplay + MISC_ADR_MAX, myParaStoreForward, sizeof(myParaStoreForward));

		/* ----- Set data to editbox ----- */
		SetData(hDlg, aulParaDisplay);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for (j = IDD_A128EDIT1; j <= IDD_A128_ADDR11_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A128EDIT1:
        case IDD_A128EDIT2:
        case IDD_A128EDIT3:
        case IDD_A128EDIT4:
        case IDD_A128EDIT5:
        case IDD_A128EDIT6:
/* <HQ> add act 128 */
        case IDD_A128EDIT7:
		case IDD_A128EDIT8:
		case IDD_A128EDIT10:    // Start of the Store and Forward range
		case IDD_A128EDIT11:
        
            if(HIWORD(wParam) == EN_CHANGE) {   /* editbox change */
                /* ----- Check range over ----- */
                if (! ChkRng(hDlg, LOWORD(wParam), 0)) {
                    /* ----- Display Error Message ----- */
                    DispError(hDlg, LOWORD(wParam));
                }
                return TRUE;
            }
            return FALSE;

		case IDD_A128ENABLE:
			if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) 
			{
				aulParaDisplay[A128_CC_BITS] |= A128_CC_ENABLE;

				EnableWindow(GetDlgItem(hDlg,IDD_A128EDIT8),   TRUE);
				EnableWindow(GetDlgItem(hDlg,IDD_A128CUST),    TRUE); 
				EnableWindow(GetDlgItem(hDlg,IDD_A128MERC),    TRUE);
				EnableWindow(GetDlgItem(hDlg,IDD_A128CUST_AUTH),    TRUE); 
				EnableWindow(GetDlgItem(hDlg,IDD_A128MERC_AUTH),    TRUE);
            } 
			else 
			{
      			aulParaDisplay[A128_CC_BITS] &= ~A128_CC_ENABLE;
				EnableWindow(GetDlgItem(hDlg,IDD_A128EDIT8),   FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_A128CUST),    FALSE); 
				EnableWindow(GetDlgItem(hDlg,IDD_A128MERC),    FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_A128CUST_AUTH),    FALSE); 
				EnableWindow(GetDlgItem(hDlg,IDD_A128MERC_AUTH),    FALSE);
			} 

			return TRUE;
			break;

		case IDD_A128CUST:
			if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) 
			{
				aulParaDisplay[A128_CC_BITS] |= A128_CC_CUST;
            } 
			else 
			{
      			aulParaDisplay[A128_CC_BITS] &= ~A128_CC_CUST;
			} 

			return TRUE;
			break;

		case IDD_A128CUST_AUTH:
			if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) 
			{
				aulParaDisplay[A128_CC_BITS] |= A128_CC_CUST_AUTH;
            } 
			else 
			{
      			aulParaDisplay[A128_CC_BITS] &= ~A128_CC_CUST_AUTH;
			} 

			return TRUE;
			break;

		case IDD_A128MERC:
			if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) 
			{
				aulParaDisplay[A128_CC_BITS] |= A128_CC_MERC;
            } 
			else 
			{
      			aulParaDisplay[A128_CC_BITS] &= ~A128_CC_MERC;
			} 

			return TRUE;
			break;

		case IDD_A128MERC_AUTH:
			if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) 
			{
				aulParaDisplay[A128_CC_BITS] |= A128_CC_MERC_AUTH;
            } 
			else 
			{
      			aulParaDisplay[A128_CC_BITS] &= ~A128_CC_MERC_AUTH;
			} 

			return TRUE;
			break;

        case IDOK:
            /* ----- Get data from dialogbox ----- */
            if (GetData(hDlg, aulParaDisplay)) {
                /* ----- Write data to file ----- */
				memcpy (aulParaMisc, aulParaDisplay, sizeof(aulParaMisc));
                ParaAllWrite(CLASS_PARAMISCPARA, (UCHAR *)aulParaMisc, sizeof(aulParaMisc), 0, &usReturnLen);

				memcpy (myParaStoreForward, aulParaDisplay + MISC_ADR_MAX, sizeof(myParaStoreForward));
                ParaAllWrite(CLASS_PARASTOREFORWARD, (UCHAR *)&myParaStoreForward, sizeof(myParaStoreForward), 0, &usReturnLen);
            } else {
                return TRUE;
            }

            /* ----- Set file status flag ----- */
            PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

            /* ----- break not used ----- */
        case IDCANCEL:
            /* ----- Destroy dialogbox ----- */
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}
/* ===== End of A128.C ===== */
