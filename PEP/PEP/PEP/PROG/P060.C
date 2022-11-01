/*
* ---------------------------------------------------------------------------
* Title         :   Total Key Type/Control Definition(Prog. 60, 61)
* Category      :   Setup, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P060.C
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Jul-27-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Sep-16-98 : 03.03.00 : A.Mitsui   : V3.3 Add Internationl VAT
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <pararam.h>
#include    "prog.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "p060.h"

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
static  P60DATA aKeyData[MAX_TTLKEY_NO];        /* data buffer */
static  WORD    wIndex;                         /* selected item index */

static  BYTE    abItemType[MAX_TTLKEY_DATA][P60_BITFORBYTE] = {
                    {P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB},
                    {P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB, P60_CB},
                    {P60_CB, P60_CB, P60_CB, P60_CB, P60_RB, P60_RB, P60_CB, P60_CB},
                    {P60_RB, P60_R3, P60_R3, P60_NU, P60_RB, P60_CB, P60_CB, P60_NU},
                    {P60_CB, P60_CB, P60_CB, P60_CB, P60_RB, P60_CB, P60_NU, P60_NU},
                    {P60_NU, P60_NU, P60_NU, P60_NU, P60_NU, P60_NU, P60_NU, P60_NU}
                };
static  BYTE    abItemCtrl[P60_FIELD_NUM][P60_CTRLTEXT_NUM] = {
                  /* 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 */
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                  /*37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 */
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                     0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1,
                     0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
					 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                 /* ### ADD Saratoga (added FoodStamp)(052400) *//* ### MAKING 052400 */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                     0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                };

static WCHAR  totalMnemonics[MAX_TTLKEY_NO][STD_TRANSMNEM_LEN + 1] = {0};

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P060DlgProc()
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
*       This is a dialgbox procedure for the Total Key Type/Control Assignment.
* ===========================================================================
*/
BOOL    WINAPI  P060DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize dialogbox ----- */
        P60InitDlg(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P60_LIST; j<=IDD_P60_KEYFUNC; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P60_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Set selected item data ----- */
                P60SetItemData(hDlg);
                return TRUE;
            } else if (HIWORD(wParam) == LBN_DBLCLK) {
                /* ----- Create definition dialogbox ----- */
                PostMessage(hDlg, WM_COMMAND, IDD_SET, 0L);
                return TRUE;
            }
            return FALSE;

		case IDD_P60_MNEMONIC:
            if (HIWORD(wParam) == EN_CHANGE) {
				WCHAR  szTemp[32] = {0};

				DlgItemGetText(hDlg, IDD_P60_MNEMONIC, szTemp, STD_TRANSMNEM_LEN + 1);
				wcsncpy (&totalMnemonics[wIndex][0], szTemp, STD_TRANSMNEM_LEN);
			}
			break;

        case IDD_P60_TYPE_CB:
        case IDD_P60_INDEX_CB: 
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                /* ----- Get selected item data ----- */
                 P60GetItemData(hDlg);
                /* ----- Change listbox item ----- */
                P60SetItemList(hDlg);
                return TRUE;
            }
            return FALSE;

		case IDD_P60_ODT_CB:
			if(HIWORD(wParam) == CBN_SELCHANGE){
				P60GetOrderDecType(hDlg);
				return TRUE;
			}
			return FALSE;

        case IDD_SET:
            if (P60GETTYPE(aKeyData[wIndex].bType) != P60_TYPE_NOTUSED) {
                /* ----- Create definition dialogbox ----- */
                DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_P60_CTRL), hDlg, P060CtrlDlgProc);
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            /* ----- Finalize dialogbox ----- */
            P60FinDlg(hDlg, wParam);
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60InitDlg()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
VOID    P60InitDlg(HWND hDlg)
{
    UINT    wI;
    WCHAR    szWork[P60_BUFF_LEN];


	for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
		if (wI < 8) {
			RflGetTranMnem (&totalMnemonics[wI][0], wI + TRN_TTL1_ADR);
		} else if (wI < 9) {
			RflGetTranMnem (&totalMnemonics[wI][0], wI - 8 + TRN_TTL9_ADR);
		} else {
			RflGetTranMnem (&totalMnemonics[wI][0], wI - 9 + TRN_TOTAL10_ADR);
		}
	}

	SendDlgItemMessage(hDlg, IDD_P60_TYPE_CB, WM_SETFONT, (WPARAM)hResourceFont, 0);
	SendDlgItemMessage(hDlg, IDD_P60_INDEX_CB, WM_SETFONT, (WPARAM)hResourceFont, 0);
    SendDlgItemMessage(hDlg, IDD_P60_MNEMONIC, EM_LIMITTEXT, STD_TRANSMNEM_LEN, 0L);
	SendDlgItemMessage(hDlg, IDD_P60_MNEMONIC, WM_SETFONT, (WPARAM)hResourceFont, 0);

    /* ----- Set description to total data type combo-box ----- */
	for (wI = 0; wI < P60_TYPE_NUM; wI++) {
        LoadString(hResourceDll, IDS_P60_TYPE0 + wI, szWork, PEP_STRING_LEN_MAC(szWork));
		DlgItemSendTextMessage (hDlg,IDD_P60_TYPE_CB,CB_INSERTSTRING,(WPARAM)-1,(LPARAM)szWork);
    }

    /* ----- Set description to financial tender index combo-box ----- */
    for (wI = 0; wI < P60_INDEX_NUM; wI++) {
        LoadString(hResourceDll, IDS_P60_INDEX0 + wI, szWork, PEP_STRING_LEN_MAC(szWork));
		DlgItemSendTextMessage (hDlg,IDD_P60_INDEX_CB,CB_INSERTSTRING,(WPARAM)-1,(LPARAM)szWork);
    }

	LoadString(hResourceDll, IDS_P60_TYPE0, szWork, PEP_STRING_LEN_MAC(szWork));
	DlgItemSendTextMessage(hDlg, IDD_P60_ODT_CB,CB_INSERTSTRING, (WPARAM)-1, (LPARAM)szWork);
	for(wI = 0; wI < P60_ODT_NUM; wI++)
	{
		DlgItemSendTextMessage(hDlg, IDD_P60_ODT_CB,CB_INSERTSTRING, (WPARAM)-1, (LPARAM)ParaTransMnemo[300+wI]);
	}
	
    /* ----- Read data and store buffer ----- */
    P60ReadParaData(hDlg);

    /* ----- Set description into listbox ----- */
    P60InitItemList(hDlg);

    /* ----- Set cursor to top of listbox ----- */
    SendDlgItemMessage(hDlg, IDD_P60_LIST, LB_SETCURSEL, 0, 0L);

    /* ----- Set selected item data ----- */
    P60SetItemData(hDlg);
	SendDlgItemMessage(hDlg, IDD_P60_ODT_CB,CB_SETCURSEL, (WPARAM)aKeyData[0].uchOrderDecType, 0);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60ReadParaData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure read data from file and store them to buffer.
* ===========================================================================
*/
VOID    P60ReadParaData(HWND hDlg)
{
    WORD    wI;
    USHORT  usReturnLen;
    UCHAR   auchParaKeyType[MAX_KEYTYPE_SIZE];
    UCHAR   auchParaKeyCtrl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];
	UCHAR	auchParaKeyOrderDec[MAX_TTLKEY_NO];

    /* ----- Read key type data (P60) from file ----- */
    ParaAllRead(CLASS_PARATTLKEYTYP, auchParaKeyType, sizeof(auchParaKeyType), 0, &usReturnLen);
	ParaAllRead(CLASS_PARATTLKEYORDERDEC, auchParaKeyOrderDec, sizeof(auchParaKeyOrderDec), 0, &usReturnLen);

    for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
         memcpy(&aKeyData[wI].uchOrderDecType, &auchParaKeyOrderDec[wI], sizeof(UCHAR));
    }

    /* ----- Store data to buffer ----- */
	// The following loops skip the first two hard coded total keys, Subtotal and Checktotal,
	// as well as the hard coded total key for Food Stamp, Total #9.
    memset(&aKeyData[P60_ADDR_1].bType, P60SETTYPE(P60_TYPE_SUBTOTAL), sizeof(BYTE));
    memset(&aKeyData[P60_ADDR_2].bType, P60SETTYPE(P60_TYPE_CHECKTOTAL), sizeof(BYTE));

    for (wI = 0; wI < 6; wI++) {    // Total Key #3 through Total Key #8, inclusive
         memcpy(&aKeyData[wI+2].bType, &auchParaKeyType[wI], sizeof(UCHAR));
    }
    for (; wI + 3 < MAX_TTLKEY_NO; wI++) {    // Total Key #10 through Total Key #20, inclusive
         memcpy(&aKeyData[wI+3].bType, &auchParaKeyType[wI], sizeof(UCHAR));
    }
	/* ### ADD Saratoga (052400) */
    memset(&aKeyData[P60_ADDR_9].bType, P60SETTYPE(P60_TYPE_FOODSTAMPTOTAL), sizeof(BYTE));

    /* ----- Read key control data (P61) from file ----- */
    ParaAllRead(CLASS_PARATTLKEYCTL, (UCHAR *)auchParaKeyCtrl, sizeof(auchParaKeyCtrl), 0, &usReturnLen);

    /* ----- Store data to buffer ----- */
/* temporary hardcoded 9, change to MAX_TTLKEY_NO when implementing full
	total database change JHHJ 3-24-04*/
    for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
        memcpy((aKeyData[wI].abCtrl), auchParaKeyCtrl[wI], MAX_TTLKEY_DATA);
    }

    PEP_REFER(hDlg);    /* just refer */
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60InitItemList()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure set item string to listbox.
* ===========================================================================
*/
VOID    P60InitItemList(HWND hDlg)
{
    WORD    wI;
    WCHAR    szWork[P60_BUFF_LEN];
    WCHAR    szBuff[P60_BUFF_LEN];
    WCHAR    szDesc[P60_BUFF_LEN];
    int     anTab[P60_TABNO]  = {
                    P60_TAB_1, P60_TAB_2, P60_TAB_3
            };

    /* ----- Set Tab Stop Points to ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_P60_LIST, LB_SETTABSTOPS, P60_TABNO, (LONG)((LPINT)anTab));

    /* ----- Load insert string from resource ----- */
    LoadString(hResourceDll, IDS_P60_LIST_STR, szWork, PEP_STRING_LEN_MAC(szWork));
	SendDlgItemMessage(hDlg, IDD_P60_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);

	for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
        /* ----- Load key type string from resource ----- */
        LoadString(hResourceDll, IDS_P60_LIST_TYPE0 + P60GETTYPE(aKeyData[wI].bType), szBuff, PEP_STRING_LEN_MAC(szBuff));

        /* ----- Make insert string ----- */
        wsPepf(szDesc, szWork, wI + 1, szBuff, P60GETINDEX(aKeyData[wI].bType));

        /* ----- Insert string into listbox ----- */
		DlgItemSendTextMessage (hDlg,IDD_P60_LIST,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)szDesc);
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60SetItemData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure set selected item data to maintenance box.
* ===========================================================================
*/
VOID    P60SetItemData(HWND hDlg)
{
    WCHAR    szWork[P60_BUFF_LEN];
    UINT    wID;

    /* ----- Get selected item index ----- */
    wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P60_LIST, LB_GETCURSEL, 0, 0L);
	wcsncpy (szWork, &totalMnemonics[wIndex][0], STD_TRANSMNEM_LEN);
	szWork[STD_TRANSMNEM_LEN] = 0;   // end of string insurance
    DlgItemSendTextMessage(hDlg, IDD_P60_MNEMONIC, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szWork));

	/* ### MOD Saratoga (052400) */
    if ((wIndex == P60_ADDR_1) || (wIndex == P60_ADDR_2) || (wIndex == P60_ADDR_9)) {
	/* ### CAUTION! P60_ADDR_9 = Food Stamp Total */

        /* ----- Show/hide dialog item ----- */
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_ST), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_BF), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_CB), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_ST), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_BF), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_CB), SW_HIDE);

        /* ----- Set type of total data to static text ----- */
        wID = ((wIndex == P60_ADDR_1) ? IDS_P60_TYPE8 : IDS_P60_TYPE9);
		switch(wIndex){
			case	P60_ADDR_1 : wID = IDS_P60_TYPE8; break;
			case	P60_ADDR_2 : wID = IDS_P60_TYPE9; break;
			case	P60_ADDR_9 : wID = IDS_P60_TYPE10;break;
		}

		/* type of total data */
		LoadString(hResourceDll, wID, szWork, PEP_STRING_LEN_MAC(szWork));
		DlgItemRedrawText(hDlg, IDD_P60_TYPE_ST, szWork);
    } else {
        /* ----- Show/hide dialog item ----- */
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_CB), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_ST), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_TYPE_BF), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_CB), SW_SHOW);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_ST), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDD_P60_INDEX_BF), SW_HIDE);

        /* ----- Set type of total data to combo-box ----- */
        SendDlgItemMessage(hDlg, IDD_P60_TYPE_CB, CB_SETCURSEL, P60GETTYPE(aKeyData[wIndex].bType), 0);

        /* ----- Set index of financial tender to combo-box ----- */
        SendDlgItemMessage(hDlg, IDD_P60_INDEX_CB, CB_SETCURSEL, P60GETINDEX(aKeyData[wIndex].bType), 0);
		SendDlgItemMessage(hDlg, IDD_P60_ODT_CB, CB_SETCURSEL, aKeyData[wIndex].uchOrderDecType, 0);
    }

    /* ----- Control set button ----- */
    P60CtrlSetBtn(hDlg);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60GetItemData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure get selected item data from maintenance box.
* ===========================================================================
*/
VOID    P60GetItemData(HWND hDlg)
{
	/* ----- Get type of total data from combo-box ----- */
    aKeyData[wIndex].bType = P60SETTYPE(SendDlgItemMessage(hDlg, IDD_P60_TYPE_CB, CB_GETCURSEL, 0, 0L));

    /* ----- Get index of financial tender from combo-box ----- */
    aKeyData[wIndex].bType += P60SETINDEX(SendDlgItemMessage(hDlg, IDD_P60_INDEX_CB, CB_GETCURSEL, 0, 0L));

	aKeyData[wIndex].uchOrderDecType = SendDlgItemMessage(hDlg, IDD_P60_ODT_CB, CB_GETCURSEL, 0, 0L);

    /* ----- Control set button ----- */
    P60CtrlSetBtn(hDlg);
}

VOID	P60GetOrderDecType(HWND hDlg)
{
	aKeyData[wIndex].uchOrderDecType = SendDlgItemMessage(hDlg, IDD_P60_ODT_CB, CB_GETCURSEL, 0, 0L);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60CtrlSetBtn()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure control set button.
* ===========================================================================
*/
VOID    P60CtrlSetBtn(HWND hDlg)
{
    BOOL    fCtrl;

    if (P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_NOTUSED) {
        fCtrl = FALSE;
    } else {
        fCtrl = TRUE;
    }

    /* ----- Control set button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_SET), fCtrl);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60SetItemList()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure set item string to listbox.
* ===========================================================================
*/
VOID    P60SetItemList(HWND hDlg)
{
    WCHAR    szWork[P60_BUFF_LEN];
    WCHAR    szBuff[P60_BUFF_LEN];
    WCHAR    szDesc[P60_BUFF_LEN];

    /* ----- Load insert string from resource ----- */
    LoadString(hResourceDll, IDS_P60_LIST_STR, szWork, PEP_STRING_LEN_MAC(szWork));
	SendDlgItemMessage(hDlg, IDD_P60_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);

    /* ----- Load key type string from resource ----- */
    LoadString(hResourceDll, IDS_P60_LIST_TYPE0 + P60GETTYPE(aKeyData[wIndex].bType), szBuff, PEP_STRING_LEN_MAC(szBuff));

    /* ----- Make insert string ----- */
    wsPepf(szDesc, szWork, wIndex + 1, szBuff, P60GETINDEX(aKeyData[wIndex].bType));

    /* ----- Change string of listbox ----- */
    SendDlgItemMessage(hDlg,IDD_P60_LIST,LB_DELETESTRING, wIndex,0L);
	DlgItemSendTextMessage(hDlg,IDD_P60_LIST,LB_INSERTSTRING,wIndex,(LPARAM)(szDesc));

    /* ----- Set cursor selected ----- */
    SendDlgItemMessage(hDlg, IDD_P60_LIST, LB_SETCURSEL, wIndex, 0L);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60FinDlg()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wParam - selected button ID
**
*   Return  :   None
*
**  Description:
*       This procedure finalize dialogbox.
* ===========================================================================
*/
VOID    P60FinDlg(HWND hDlg, WPARAM wParam)
{
    if (LOWORD(wParam) == IDOK) {
		WORD  wI;

		/* ----- Restore data and write data----- */
        P60WriteParaData(hDlg);

		for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
			if (wI < 8) {
				RflPutTranMnem (&totalMnemonics[wI][0], wI + TRN_TTL1_ADR);
			} else if (wI < 9) {
				RflPutTranMnem (&totalMnemonics[wI][0], wI - 8 + TRN_TTL9_ADR);
			} else {
				RflPutTranMnem (&totalMnemonics[wI][0], wI - 9 + TRN_TOTAL10_ADR);
			}
		}

        /* ----- Set file status flag ----- */
        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }

    /* ----- Destroy dialogbox ----- */
    EndDialog(hDlg, FALSE);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60WriteParaData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure restore data from buffer and write them to file.
* ===========================================================================
*/
VOID    P60WriteParaData(HWND hDlg)
{
    WORD    wI;
    USHORT  usReturnLen;
    UCHAR   auchParaKeyType[MAX_KEYTYPE_SIZE];
    UCHAR   auchParaKeyCtrl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];
	UCHAR	auchParaKeyOrderDec[MAX_TTLKEY_NO];

    /* ----- Restore data from buffer ----- */
    for (wI = 0; wI < 6; wI++) {
         auchParaKeyType[wI] = aKeyData[wI + 2].bType;
    }
    for (; wI + 3 < MAX_TTLKEY_NO; wI++) {
         auchParaKeyType[wI] = aKeyData[wI + 3].bType;
    }

    /* ----- Write key type data (P60) to file ----- */
    ParaAllWrite(CLASS_PARATTLKEYTYP, auchParaKeyType, sizeof(auchParaKeyType), 0, &usReturnLen);

    /* ----- Restore data from buffer ----- */
    for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
        memcpy(auchParaKeyCtrl[wI], (aKeyData[wI].abCtrl), (MAX_TTLKEY_DATA * sizeof(BYTE)));
    }

    /* ----- Write key control data (P61) to file ----- */
    ParaAllWrite(CLASS_PARATTLKEYCTL, (UCHAR *)auchParaKeyCtrl, sizeof(auchParaKeyCtrl), 0, &usReturnLen);

    for (wI = 0; wI < MAX_TTLKEY_NO; wI++) {
         auchParaKeyOrderDec[wI] = aKeyData[wI].uchOrderDecType;
    }
    ParaAllWrite(CLASS_PARATTLKEYORDERDEC, (UCHAR *)auchParaKeyOrderDec, sizeof(auchParaKeyOrderDec), 0, &usReturnLen);

    PEP_REFER(hDlg);    /* just refer */
}

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P060CtrlDlgProc()
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
*       This is a key control definition dialgbox procedure.
* ===========================================================================
*/
BOOL    WINAPI  P060CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize key control dialogbox ----- */
        P60InitCtrlDlg(hDlg);
        /* ----- Set key control data ----- */
        P60SetCtrlData(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P60_CTRL; j<=IDD_P60_PREAUTH; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            /* ----- Get key control data ----- */
            P60GetCtrlData(hDlg);
            /* ----- break not used ----- */
        case IDCANCEL:
            EndDialog(hDlg, FALSE);
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }

    PEP_REFER(lParam);    /* just refer */
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60InitCtrlDlg()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure initialize key control dialogbox.
* ===========================================================================
*/
VOID    P60InitCtrlDlg(HWND hDlg)
{
    UINT    wI;
	int		wID, wField;
    UINT    wStrID = 0;
    WCHAR    szDesc[128];
    PARAMDC MdcData, MdcData2;
    BYTE    bTaxMode;

    /* ----- Get Current Tax Mode (US/Canada/VAT) ----- */
    MdcData.usAddress = P60_TAX_ADDR;    //  MDC_TAX_ADR
    ParaMDCRead(&MdcData);
    MdcData2.usAddress = P60_TAX_ADDR2;  // MDC_TAXINTL_ADR
    ParaMDCRead(&MdcData2);

    bTaxMode = (BYTE)((!(MdcData.uchMDCData & P60_TAX_CHK)) ? P60_TAX_US :  /* US */
                      (!(MdcData2.uchMDCData & P60_TAX_CHK)) ? P60_TAX_CA :  /* Canada */
                                                               P60_TAX_VT);   /* VAT */

    switch (bTaxMode) {
    case P60_TAX_US:        /* US Tax */
        wStrID = IDS_P60_GROUP_01US;
        break;

    case P60_TAX_CA:        /* Canadian Tax */
        wStrID = IDS_P60_GROUP_01CA;
        break;

    case P60_TAX_VT:        /* VAT */
        wStrID = IDS_P60_GROUP_01VT;
        break;
    }

    /* ----- Load Address Description from Resource ----- */
    for (wI = 0; wI < P60_TAXGROUP_NUM; wI++) {
        LoadString(hResourceDll, wStrID + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_P60_GROUP_01 + wI, szDesc);
    }

    switch (bTaxMode) {
    case P60_TAX_US:        /* US Tax */
        wStrID = IDS_P60_CTRL_01US;
        break;

    case P60_TAX_CA:        /* Canadian Tax */
        wStrID = IDS_P60_CTRL_01CA;
        break;

    case P60_TAX_VT:        /* VAT */
        wStrID = IDS_P60_CTRL_01VT;
        break;
    }

    /* ----- Load Address Description from Resource ----- */
    for (wI = 0; wI < P60_TAXCTRL_NUM; wI++) {
        LoadString(hResourceDll, wStrID + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_P60_CTRL_01 + wI, szDesc);
    }

    switch (P60GETTYPE(aKeyData[wIndex].bType)) {
    case P60_TYPE_SUBTOTAL:
        wField = P60_FIELD1;
        break;

    case P60_TYPE_CHECKTOTAL:
    case P60_TYPE_NOFINAL:
        wField = P60_FIELD2;
        break;

    case P60_TYPE_SERVICEWO:
    case P60_TYPE_SERVICEW:
        wField = P60_FIELD3;
        break;

    case P60_TYPE_FINALWO:
    case P60_TYPE_FINALW:
        wField = P60_FIELD4;
        break;

    case P60_TYPE_TRAYWO:
    case P60_TYPE_TRAYW:
        wField = P60_FIELD5;
        break;

	/* ### ADD Saratoga (052400) */
    case P60_TYPE_FOODSTAMPTOTAL:
        wField = P60_FIELD6;
        break;

    default:    /* case P60_TYPE_NOTUSED: */
        wField = P60_FIELD0;
        break;
    }

    if (wField == P60_FIELD0) {
        return;
    }

    /* ----- Enable/disable checkbox, radiobutton ----- */
    for (wI = 0, wID = IDD_P60_CTRL_01; wI < P60_CTRLTEXT_NUM; wI++, wID++) {
        EnableWindow(GetDlgItem(hDlg, wID), abItemCtrl[wField][wI]);
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60SetCtrlData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure set key control data.
* ===========================================================================
*/
VOID    P60SetCtrlData(HWND hDlg)
{
    WORD    wI, wJ, wID;

    for (wI = 0, wID = IDD_P60_CTRL_01; wI < MAX_TTLKEY_DATA; wI++) {
        for (wJ = 0; wJ < P60_BITFORBYTE; wJ++) {
            if (abItemType[wI][wJ] == P60_CB) {
                /* ----- Display Order#,   R3.0 ----- */
                if ((wID == IDD_P60_CTRL_36)
                    && (P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SERVICEWO
                        || P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SERVICEW)) {

                    if (aKeyData[wIndex].abCtrl[3] & P60_DISP_ORDER) {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    } else {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    }
                    SendDlgItemMessage(hDlg, IDD_P60_CTRL_27, BM_SETCHECK, TRUE, 0L);
                    SendDlgItemMessage(hDlg, IDD_P60_CTRL_28, BM_SETCHECK, FALSE, 0L);
                } else {
                    /* ----- Set data to checkbox ----- */
                    if (aKeyData[wIndex].abCtrl[wI] & (P60_BITCHK << wJ)) {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    } else {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    }
                }
            } else if (abItemType[wI][wJ] == P60_RB) {
                /* ----- Subtotal Buffering,   R3.0 ----- */
                if (wID == IDD_P60_CTRL_27 && P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SUBTOTAL) {
                      /* ----- Set data to radio button ----- */
                    if (aKeyData[wIndex].abCtrl[3] & P60_SUBTTL_BUFFER) {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    } else {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    }
                }   else {
                      /* ----- Set data to radio button ----- */
                    if (aKeyData[wIndex].abCtrl[wI] & (P60_BITCHK << wJ)) {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    } else {
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                        SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    }
                 }
            } else if (abItemType[wI][wJ] == P60_R3) {
                /* ----- Set data to radio button(1 for 3) ----- */
                if (aKeyData[wIndex].abCtrl[wI] & (P60_BITCHK << ++wJ)) {
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                } else if (aKeyData[wIndex].abCtrl[wI] & (P60_BITCHK << (wJ - 1))) {
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                } else {
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, TRUE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                    SendDlgItemMessage(hDlg, wID++, BM_SETCHECK, FALSE, 0L);
                }
            } else if (abItemType[wI][wJ] == P60_NU) {
                wID++;
            }
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P60GetCtrlData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure get key control data.
* ===========================================================================
*/
VOID    P60GetCtrlData(HWND hDlg)
{
    WORD    wI, wJ, wID;

    for (wI = 0, wID = IDD_P60_CTRL_01; wI < MAX_TTLKEY_DATA; wI++) {
        for (wJ = 0; wJ < P60_BITFORBYTE; wJ++) {
            if (abItemType[wI][wJ] == P60_CB) {
                /* ----- Display Order#,   R3.0 ----- */
                if ((wID == IDD_P60_CTRL_36) && (P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SERVICEWO
                    || P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SERVICEW)) {

                    if (SendDlgItemMessage(hDlg, IDD_P60_CTRL_36, BM_GETCHECK, 0, 0L)) {
                        aKeyData[wIndex].abCtrl[3] |= P60_DISP_ORDER;
                    } else {
                        aKeyData[wIndex].abCtrl[3] &= ~P60_DISP_ORDER;
                    }
                    wID++;
                } else {
                    /* ----- Get data from checkbox ----- */
                    if (SendDlgItemMessage(hDlg, wID++, BM_GETCHECK, 0, 0L)) {
                        aKeyData[wIndex].abCtrl[wI] |= (P60_BITCHK << wJ);
                    } else {
                        aKeyData[wIndex].abCtrl[wI] &= ~(P60_BITCHK << wJ);
                    }
                }
            } else if (abItemType[wI][wJ] == P60_RB) {
                /* ----- Subtotal Buffering,   R3.0 ----- */
                if (wID == IDD_P60_CTRL_27 && P60GETTYPE(aKeyData[wIndex].bType) == P60_TYPE_SUBTOTAL) {
                      /* ----- Set data to radio button ----- */
                    if (SendDlgItemMessage(hDlg, IDD_P60_CTRL_27, BM_GETCHECK, 0, 0L)) {
                        aKeyData[wIndex].abCtrl[3] &= ~P60_SUBTTL_BUFFER;
                    } else {
                        aKeyData[wIndex].abCtrl[3] |= P60_SUBTTL_BUFFER;
                    }
                    wID++;
                }   else {
                    /* ----- Get data from radio button ----- */
                    if (SendDlgItemMessage(hDlg, ++wID, BM_GETCHECK, 0, 0L)) {
                        aKeyData[wIndex].abCtrl[wI] |= (P60_BITCHK << wJ);
                    } else {
                        aKeyData[wIndex].abCtrl[wI] &= ~(P60_BITCHK << wJ);
                    }
                }

                wID++;
            } else if (abItemType[wI][wJ] == P60_R3) {
                /* ----- Get data from radio button(1 for 3) ----- */
                wID++;
                if (SendDlgItemMessage(hDlg, ++wID, BM_GETCHECK, 0, 0L)) {
                    aKeyData[wIndex].abCtrl[wI] |= (P60_BITCHK << ++wJ);
                } else {
                    aKeyData[wIndex].abCtrl[wI] &= ~(P60_BITCHK << ++wJ);
                    if (SendDlgItemMessage(hDlg, wID - 1, BM_GETCHECK, 0, 0L)) {
                        aKeyData[wIndex].abCtrl[wI] |= (P60_BITCHK << (wJ - 1));
                    } else {
                        aKeyData[wIndex].abCtrl[wI] &= ~(P60_BITCHK << (wJ - 1));
                    }
                }
                wID++;
            } else if (abItemType[wI][wJ] == P60_NU) {
                wID++;
            }
        }
    }
}

/* ===== End of P060.C ===== */
