/*
* ---------------------------------------------------------------------------
* Title         :   Set Promotional PLU (AC 116)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A116.C
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
* Apr-18-95 : 03.00.00 : H.Ishida   : Debug (mistake number of ListBox) 
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
#include    <stdlib.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>
#include    <rfl.h>        /* Saratoga */

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a116.h"

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
static  LPA116PROMO lpPromo;        /* Address of Promotional PLU Data  */
static  LPMENUPLU   lpPluData;      /* Address of Current PLU Records   */
static  ULONG       ulPluNum;       /* No. of PLU Records (Prog. #2)  NCR2172  */
static  short       nPromoIndex,    /* Selected Promotional PLU Address */
                    nFieldIndex;    /* Field of Promotional PLU         */
                                    /*      0 = Promotional PLU         */
                                    /*  1 - 5 = Linked PLU Item         */
static  BYTE        bDelConf = A116_DELCONF_ON;   /* Delete Confirmation status       */

static  HWND  hWndModeLess = NULL;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A116DlgProc()
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
*                   Set Promotional PLU (Action Code # 116)
* ===========================================================================
*/
BOOL    WINAPI  A116DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  char    chListType;     /* Promotional PLU-ListBox Type     */
    static  ULONG   ulRecordNo;     /* Current Exist PLU Records        */
    static  HGLOBAL hGlb,           /* Handle of Global Area            */
                    hGlbMenu;       /* Handle of Global Area            */

    LPSHORT lpnFlag;                /* Points to Index of ListBox       */
    USHORT  usReturnLen;            /* Return Value of ParaAll Func.    */
    WCHAR   uchAdj;                 /* Adjective Code Work Area         */
    short   nCount;                 /* Loop Counter                     */
    WCHAR    szCap[PEP_CAPTION_LEN], /* MessageBox Caption               */
            szWork[PEP_OVER_LEN],   /* Data Range Over Error Message    */
            szMsg[PEP_OVER_LEN];    /* Data Range Over Error Message    */
    ULONG   ulRet;                  /* Offset of PLU No. of PLU LIST    */

    /*NCR2172*/
    WCHAR   szPLU[PLU_MAX_DIGIT + 1]; /* PLU Number Work Area             */
    WCHAR   szWorkPLU[PLU_MAX_DIGIT + 1];

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Promotional PLU Data Buffer ----- */
        hGlb     = GlobalAlloc(GHND, sizeof(MENUPLU)   * FLEX_PLU_MAX);
        hGlbMenu = GlobalAlloc(GHND, sizeof(A116PROMO) * A116_MENU_MAX);

        /* ----- Determine Whether Memory Allocation is Success or Not ----- */
        if ((! hGlbMenu) || (! hGlb)) {     /* Momory Allocation Error      */
            A116ExecAllocErr(hDlg, hGlb, hGlbMenu);

			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        } else {                            /* Succeed in Memory Allocation */
            lpPluData = (LPMENUPLU)GlobalLock(hGlb);
            lpPromo   = (LPA116PROMO)GlobalLock(hGlbMenu);

            ParaAllRead(CLASS_PARAMENUPLUTBL, (UCHAR *)lpPromo, sizeof(A116PROMO) * A116_MENU_MAX, 0, &usReturnLen);

            /* ----- Initialize DialogBox ----- */
            ulRecordNo = A116InitDlg(hDlg);

            EnableWindow(GetDlgItem(hDlg, IDD_A116_NEXT), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDD_A116_PREV), FALSE);

            chListType = A116_PART_TYPE;    /* List Type = Promotion */
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_A116_MENULIST; j <= IDD_A116_ADJCODE_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        A116SpinProc(hDlg, wParam, lParam);
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A116_NEXT:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current ListBox Type ----- */
                if (! (chListType & A116_PART_TYPE)) {  /* Shows Linked PLU */
                    nPromoIndex += (short)(((nPromoIndex + 1) != A116_MENU_MAX) ? 1 : 0);

                    /* ----- Update Linked PLU ListBox ----- */
                    A116ChgList(hDlg, chListType, ulRecordNo);

                    /* ----- Set New Promotional PLU No. to GroupBox ----- */
                    LoadString(hResourceDll, IDS_A116_PARTPLU, szMsg, PEP_STRING_LEN_MAC(szMsg));
                    wsPepf(szWork, szMsg, nPromoIndex + 1);
                    DlgItemRedrawText(hDlg, IDD_A116_GROUP, szWork);
                    return TRUE;
                }
            }
            return FALSE;

        case IDD_A116_PREV:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current ListBox Type ----- */
                if (! (chListType & A116_PART_TYPE)) {  /* Shows Linked PLU */
                    nPromoIndex -= (short)(((nPromoIndex + 1) != A116_MENU_MIN) ? 1 : 0);

                    /* ----- Update Linked PLU ListBox ----- */
                    A116ChgList(hDlg, chListType, ulRecordNo);

                    /* ----- Set New Promotional PLU No. to GroupBox ----- */
                    LoadString(hResourceDll, IDS_A116_PARTPLU, szMsg, PEP_STRING_LEN_MAC(szMsg));
                    wsPepf(szWork, szMsg, nPromoIndex + 1);
                    DlgItemRedrawText(hDlg, IDD_A116_GROUP, szWork);
                    return TRUE;
                }
            }
            return FALSE;

        case IDD_A116_CONF:
            if (HIWORD(wParam) == BN_CLICKED) {
                bDelConf ^= A116_CONF_CHK;
                return TRUE;
            }
            return FALSE;

        case IDD_A116_PLU:
            if (HIWORD(wParam) == EN_CHANGE) {
				BOOL    fTrans;
				UINT    unValue = DlgItemGetText(hDlg, LOWORD(wParam), szPLU, sizeof(szPLU));

                /* ----- Disable / Enable Add/Change, Delete Button ----- */
                if(unValue != 0) {
                  fTrans = TRUE;
                } else {
                  fTrans = FALSE;
                }

                EnableWindow(GetDlgItem(hDlg, IDD_A116_ADD), fTrans);
                EnableWindow(GetDlgItem(hDlg, IDD_A116_DEL), fTrans);
                return TRUE;
            }
            return FALSE;

        case IDD_A116_ADJ:
            if (HIWORD(wParam) == EN_CHANGE) {
				UINT    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
                if (unValue > A116_ADJ_MAX) {

                    /* ----- Load Error Message from Resource ----- */
                    LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szCap, PEP_STRING_LEN_MAC(szCap));
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

                    /* ----- Create Error Message ----- */
                    wsPepf(szMsg, szWork, A116_ADJ_MIN, A116_ADJ_MAX);

                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);

                    SendMessage((HWND)lParam, EM_UNDO,   0, 0L);
                    SendMessage((HWND)lParam, EM_SETSEL, 0, MAKELONG(0, -1));
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A116_CONTENT:
            if (HIWORD(wParam) == BN_CLICKED) {
				BOOL    fEnable ;
                /* ----- Change Menu-ListBox Type ----- */
                A116ChgContent(hDlg, (LPBYTE)&chListType, ulRecordNo);
                fEnable = (chListType & A116_PART_TYPE) ? FALSE : TRUE;
                EnableWindow(GetDlgItem(hDlg, IDD_A116_NEXT), fEnable);
                EnableWindow(GetDlgItem(hDlg, IDD_A116_PREV), fEnable);
                return TRUE;
            }
            return FALSE;

        case IDD_A116_MENULIST:
            if (HIWORD(wParam) == LBN_DBLCLK) {
                if (chListType & A116_PART_TYPE) {
                    PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A116_CONTENT, BN_CLICKED), 0L);
                }
                return TRUE;
            } else if(HIWORD(wParam) == LBN_SELCHANGE) {
                lpnFlag = (LPSHORT)((chListType & A116_PART_TYPE) ? &nPromoIndex : &nFieldIndex);

                *lpnFlag = (short)SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_GETCURSEL, 0, 0L);

                RflConvertPLU(szPLU, (WCHAR*)((lpPromo + nPromoIndex)->aMenuPLU[nFieldIndex].aszPLUNumber));
                szPLU[PLU_MAX_DIGIT] = 0x00;
                DlgItemRedrawText(hDlg, IDD_A116_PLU, szPLU );
                
                SetDlgItemInt(hDlg, IDD_A116_ADJ, (lpPromo + nPromoIndex)->aMenuPLU[nFieldIndex].uchAdjective, FALSE);
                /* ----- NORMAL / UPC-E checkbox clear ----- */

                SendMessage((GetDlgItem(hDlg, IDD_A116_VER)), BM_SETCHECK, 0, 0L);
                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A116_PLU, EN_CHANGE), 0L);

                ulRet = A116FindPluNo((LPMENUPLU)&(lpPromo + nPromoIndex)->aMenuPLU[nFieldIndex], ulRecordNo);
                SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETCURSEL,   (WORD)ulRet, 0L);

                if (ulRet != A116_NO_FILE) {
                    SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETTOPINDEX, (WORD)ulRet, 0L);
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A116_ADD:
            if (HIWORD(wParam) == BN_CLICKED) {
				BOOL     fTrans;
				UINT     unValue = GetDlgItemInt(hDlg, IDD_A116_ADJ, (LPBOOL)&fTrans, FALSE);

                if (fTrans == 0) {
                    /* ----- Load String from Resource ----- */
                    LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szCap, PEP_STRING_LEN_MAC(szCap));
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

                    /* ----- Create Error Message ----- */
                    wsPepf(szMsg, szWork, A116_ADJ_MIN, A116_ADJ_MAX);
                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);
                    SendDlgItemMessage(hDlg, IDD_A116_ADJ, EM_SETSEL, 0, MAKELONG(0, -1));
                    SetFocus(GetDlgItem(hDlg, IDD_A116_ADJ));
                    return TRUE;
                } else {
                    A116ChgPromoPlu(hDlg, chListType, ulRecordNo);
                    return TRUE;
                }
            }
            return FALSE;

        case IDD_A116_DEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                A116DelPromoPlu(hDlg, chListType);
                return TRUE;
            }
            return FALSE;

        case IDD_A116_PLULIST:
            if (HIWORD(wParam) == LBN_DBLCLK) {
                /* ----- Set PLU No. to Promotional PLU ----- */
                A116ChgPromoPlu(hDlg, chListType, ulRecordNo);
                return TRUE;
            } else if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Get Current Selected Item Index ---- */
                nCount = (short)SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_GETCURSEL, 0, 0L);
                memset(szPLU, 0x00, sizeof(szPLU));
				/* ###ADD Saratoga */
                memset(szWorkPLU, 0x00, sizeof(szWorkPLU));

                RflConvertPLU(szWorkPLU, (WCHAR*)((lpPluData + nCount)->aszPLUNumber));
                /*wsPepf(szPLU, "%13s", szWorkPLU);*/
                wsPepf(szPLU, WIDE("%1s"), szWorkPLU);

                uchAdj = (lpPluData + nCount)->uchAdjective;

                /* ----- Set PLU Data to EditText ---- */
                DlgItemRedrawText(hDlg, IDD_A116_PLU, szPLU);
                SetDlgItemInt(hDlg, IDD_A116_ADJ, uchAdj, FALSE);

                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A116_PLU, EN_CHANGE), 0L); 

                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    ParaAllWrite(CLASS_PARAMENUPLUTBL, (UCHAR *)lpPromo, sizeof(A116PROMO) * A116_MENU_MAX, 0, &usReturnLen);
                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
                }

                /* ----- Unlock Global Area & Set Them Free ----- */
                GlobalUnlock(hGlbMenu);
                GlobalFree(hGlbMenu);

                GlobalUnlock(hGlb);
                GlobalFree(hGlb);

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
**  Synopsis    :   USHORT  A116InitDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   ULONG   ulRecordNo  -   No. of Current Exists PLU Records
*
**  Description :
*       This function gets the number of PLU memory from Prog. #2. And then it
*   loads all plu records, and sets them to data buffer.
*       And then it initializes the configulation of DialogBox.
* ===========================================================================
*/
ULONG  A116InitDlg(HWND hDlg)
{
    ULONG       ulRecordNo = 0L;             /* No. of Current PLU Records   */
    PARAFLEXMEM FlexData;                   /* Get Count of PLU File        */
    DWORD       dwPrice;                    /* Loaded PLU Price Work Area   */
    WCHAR        szWork[A116_DESC_LEN+ 50],      /* Description for PLU          */
                szPrice[A116_MNE_LEN],      /* PLU Price Work Area          */
                szButton[A116_DESC_LEN],    /* Description (Content Button) */
                szMnemo[A116_MNE_LEN + 1];  /* PLU Mnemonic Work Area       */
    UCHAR       uchPluType;
    short       nRet = 0;                   /* Return Value of Function     */
    long        nCount;		//RPH 12/16/02 SR#85 changed to a long was short
    BOOL        fStatus;
    
    USHORT      uI;
    BOOL        bWork = TRUE;
    ULONG       ulRet;       /* Saratoga */

    /*NCR2172*/
    WCHAR       szPLU[PLU_MAX_DIGIT + 1];
    WCHAR       szWorkPLU[PLU_MAX_DIGIT+1];

    WCHAR    szMsg[256];
    WCHAR    szDesc[128];
	PLUIF       PluFile;                    /* Structure of OpPluAllRead    */

    /* ----- Initializes Current Maintenance Menu Index ----- */
    nPromoIndex = 0; 

    /* ----- Get No. of PLU Memory from Prog. #2 ----- */
    FlexData.uchAddress = FLEX_PLU_ADR;
    ParaFlexMemRead(&FlexData);

    /* ----- Set No. of PLU Memory to Static Buffer ----- */
    ulPluNum = FlexData.ulRecordNumber;

    /* ----- Check Whether No. of PLU Memory is 0 or Not ----- */
    if (ulPluNum != 0L) {

        /* ----- Initialize Counter for Load PLU Data ----- */
        PluFile.ulCounter = 0L;
        for (; ; ulRecordNo++) {

            /* ----- Read Current Existds PLU Data from PLU File ----- */
            nRet = OpPluAllRead(&PluFile, 0);
        
            /* ----- Exit Roop, If PLU File is End or Not Exist ----- */
            /* 09/14/00 */
            if ((nRet != OP_PERFORM)) {
                break;
            }

            /* ----- Set Loaded PLU No. to Heap Buffer ----- */
            nCount = ulRecordNo;  //RPH 12/16/02 SR#85 was cast to short
            memset(szPLU, 0x00, sizeof(szPLU));
            memset(szWorkPLU, 0x00, sizeof(szWorkPLU));
            memcpy(((lpPluData + nCount)->aszPLUNumber), PluFile.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

            /* ----- Get Loaded PLU Data Type ----- */
            uchPluType = (BYTE)PluFile.ParaPlu.ContPlu.uchItemType;

            /* ----- Determine Whether PLU Type is One Adjective or Not ----- */
            if ((uchPluType == A116_OPEN_01) || (uchPluType == A116_NON_10)) {
                (lpPluData + nCount)->uchAdjective = A116_NON_ADJ;
            } else {
                (lpPluData + nCount)->uchAdjective = PluFile.uchPluAdj;
            }

			if (uchPluType == A116_OPEN_01) {
                LoadString(hResourceDll, IDS_A116_OPEN, szPrice, PEP_STRING_LEN_MAC(szPrice));
            } else {
                /* ----- Conversion Price Data (UCHAR -> ULONG) ----- */
                PepConv3Cto4L((LPBYTE)PluFile.ParaPlu.auchPrice, (LPDWORD)&dwPrice);
                wsPepf(szPrice, WIDE("%-7lu"), dwPrice);
            }
 
            /* ----- Translate Double Key Code of Mnemonic ----- */
			memset(szMnemo, 0x00, sizeof(szMnemo));
            PepReplaceMnemonic((WCHAR *)PluFile.ParaPlu.auchPluName, (WCHAR *)szMnemo, (A116_MNE_LEN ), PEP_MNEMO_READ);

            /* ----- Create Description and Set them to PLU-ListBox ----- */
            /*NCR2172*/
            memset(szPLU, 0x00, sizeof(szPLU));

            RflConvertPLU(szWorkPLU, (WCHAR*)((lpPluData + nCount)->aszPLUNumber));
            wsPepf(szPLU, WIDE("%-14s"), szWorkPLU);

            wsPepf(szWork, WIDE("%14s \t%5u    \t%-7s \t%s"), szPLU, (lpPluData + nCount)->uchAdjective, szPrice, szMnemo);
            
            /* ----- Set Description to PLU-ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A116_PLULIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
        }
    }

    /* 09/14/00 */
    if ((nRet != OP_PERFORM) && (nRet != OP_EOF) && (nRet != OP_FILE_NOT_FOUND)) {
       /* ----- Display caution message ----- */
       LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szDesc, PEP_STRING_LEN_MAC(szDesc));
       LoadString(hResourceDll, IDS_A116_ERR_PLUREC, szMsg, PEP_STRING_LEN_MAC(szMsg));
       MessageBeep(MB_ICONEXCLAMATION);
       MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);
    }

    /* ----- Set Fixed Font to Menu / PLU ListBox ----- */
    PepSetFont(hDlg, IDD_A116_PLULIST);
    PepSetFont(hDlg, IDD_A116_MENULIST);

    /* ----- Set Initial Promotional PLU Data to Menu-ListBox ----- */
    A116ChgList(hDlg, A116_PART_TYPE, ulRecordNo);

    /* ----- Set Max Length to EditText ----- */
    SendDlgItemMessage(hDlg, IDD_A116_PLU, EM_LIMITTEXT, PLU_MAX_DIGIT, 0L);
    SendDlgItemMessage(hDlg, IDD_A116_ADJ, EM_LIMITTEXT, A116_ADJ_LEN, 0L);

    /* ----- Set Initial PLU No. to EditText ----- */
    memset(szPLU, 0x00, sizeof(szPLU));
    memset(szWorkPLU, 0x00, sizeof(szWorkPLU));

    RflConvertPLU(szWorkPLU, (WCHAR*)(lpPromo->aMenuPLU[A116_PART_ADDR].aszPLUNumber));
    wsPepf(szPLU, WIDE("%s"), szWorkPLU);

    DlgItemRedrawText(hDlg, IDD_A116_PLU, szPLU);

    /* ----- Set Initial Adjective Code to EditText ----- */
    SetDlgItemInt(hDlg, IDD_A116_ADJ, lpPromo->aMenuPLU[A116_PART_ADDR].uchAdjective, FALSE);

    ulRet = A116FindPluNo((LPMENUPLU)&lpPromo->aMenuPLU[A116_PART_ADDR], ulRecordNo);

    SendDlgItemMessage(hDlg, IDD_A116_PLU, LB_SETCURSEL,   (WORD)nRet, 0L);
    SendDlgItemMessage(hDlg, IDD_A116_PLU, LB_SETTOPINDEX, (WORD)nRet, 0L);

    /* ----- Enable / Disable Button (Add/Change , Delete) ----- */
    memset(szPLU, 0x00, sizeof(szPLU));
    memcpy(szPLU, (lpPromo->aMenuPLU[A116_PART_ADDR].aszPLUNumber), PLU_MAX_DIGIT * sizeof(WCHAR));

	bWork  = TRUE;
    for ( uI = 0; uI < PLU_MAX_DIGIT && bWork == TRUE; uI++) {
		if(szPLU[uI] == 0x00) {
			bWork = FALSE;
		}
    }

    if(bWork == FALSE)
    {
      EnableWindow(GetDlgItem(hDlg, IDD_A116_ADD), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_A116_DEL), FALSE);
    }
    else
    {
      EnableWindow(GetDlgItem(hDlg, IDD_A116_ADD), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDD_A116_DEL), TRUE);
    }

    /* ----- Set Description to Content Button ----- */
    LoadString(hResourceDll, IDS_A116_CONTENT, szButton, PEP_STRING_LEN_MAC(szButton));

    DlgItemRedrawText(hDlg, IDD_A116_CONTENT, szButton);

    /* ----- Set GroupBox Description ----- */
    LoadString(hResourceDll, IDS_A116_MENUPLU, szButton, PEP_STRING_LEN_MAC(szButton));

    DlgItemRedrawText(hDlg, IDD_A116_GROUP, szButton);

    /* ----- Check Delete Confirmation CheckBox ----- */
    fStatus = (bDelConf == A116_DELCONF_ON) ? TRUE : FALSE;
    CheckDlgButton(hDlg, IDD_A116_CONF, fStatus);

    return (ulRecordNo);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116ChgPromoPlu()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   char    chType      -   Current ListBox Type 
*                   ULONG   ulRecordNo  -   No. of Current Plu Records
*
**  Return      :   No return values
*
**  Description :
*       This function gets inputed PLU No. and Adjective Code from EditText,
*   and then sets them to data buffer. And it creates the description for
*   inserts listbox.
* ===========================================================================
*/
VOID    A116ChgPromoPlu(HWND hDlg, char chType, ULONG ulRecordNo)
{
	MENUPLU MenuPlu = {0};          /* Structure for A116MakeDesc     */
    UINT    unAdj;                  /* Inputed Adjective Code Value */
    WORD    wIndex,                 /* Target Index of ListBox      */
            wWork;
    WCHAR   szWork[A116_DESC_LEN];  /* Buffer for PLU Description   */

    DWORD   dwRet;
    BOOL    fVer;

    /*NCR2172*/
	WCHAR   szPLU[PLU_MAX_DIGIT + 1] = {0};
    ULONG   ulRet;                  /* Saratoga */

    /* ----- Get Inputed PLU No./Adjective Code from EditText ----- */
    DlgItemGetText(hDlg, IDD_A116_PLU, szPLU, PLU_MAX_DIGIT);

    unAdj   = GetDlgItemInt(hDlg, IDD_A116_ADJ, NULL, FALSE);

    /* ----- NORMAL or UPC-E Version Check ----- */
    dwRet = SendMessage((GetDlgItem(hDlg, IDD_A116_VER)), BM_GETCHECK, 0, 0L);

    if(dwRet == 0L) {
       fVer = LA_NORMAL;
    } else {
       fVer = LA_EMOD;
    }

    dwRet = A116GetPluNo(fVer, szPLU);
    if(dwRet != A116_OK) {               /* ----- Error!!! -----*/
		WCHAR    szMsg[256] = {0};
		WCHAR    szDesc[128] = {0};

       /* ----- Display caution message ----- */
       LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szDesc, PEP_STRING_LEN_MAC(szDesc));

       if (dwRet == A116_ERR) {
          LoadString(hResourceDll, IDS_A116_ERR_PLUNO, szMsg, PEP_STRING_LEN_MAC(szMsg));
       } else {
          LoadString(hResourceDll, IDS_A116_LABEL, szMsg, PEP_STRING_LEN_MAC(szMsg));
       }

       MessageBeep(MB_ICONEXCLAMATION);
       MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);

       /* ----- Set focus, and cursor selected ----- */
       SetFocus(GetDlgItem(hDlg, IDD_A116_PLU));

       SendDlgItemMessage(hDlg, IDD_A116_PLU, EM_SETSEL, 1, MAKELONG(-1, 0));
       return ;
    }

    /* ----- Set Inputed Data to Buffer ----- */
    memcpy(((lpPromo + nPromoIndex)->aMenuPLU[nFieldIndex].aszPLUNumber), szPLU, PLU_MAX_DIGIT * sizeof(WCHAR));

    (lpPromo + nPromoIndex)->aMenuPLU[nFieldIndex].uchAdjective = (UCHAR)unAdj;

    /* ----- Check Current ListBox Type ----- */
    wIndex = (WORD)((chType & A116_PART_TYPE) ? nPromoIndex : nFieldIndex);

    /* ----- Create Description with Inputed PLU No. ----- */
    memcpy((MenuPlu.aszPLUNumber), szPLU, PLU_MAX_DIGIT * sizeof(WCHAR));
    MenuPlu.aszPLUNumber[PLU_MAX_DIGIT] = 0x00;
    MenuPlu.uchAdjective = (UCHAR)unAdj;

    wWork = (WORD)((chType & A116_PART_TYPE) ? wIndex + 1 : wIndex);

    A116MakeDesc((LPMENUPLU)&MenuPlu, szWork, wWork);

    /* ----- Delete Old String from Menu-ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_DELETESTRING, wIndex, 0L);

    /* ----- Set New String to Menu-ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A116_MENULIST, LB_INSERTSTRING, wIndex, (LPARAM)(szWork));

    /* ----- Set High-Light Bar to Inserted String ----- */
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETCURSEL,   wIndex, 0L);
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETTOPINDEX, wIndex, 0L);
    SetFocus(GetDlgItem(hDlg, IDD_A116_MENULIST));

    ulRet = A116FindPluNo((LPMENUPLU)&MenuPlu, ulRecordNo);
    SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETCURSEL, (WORD)ulRet, 0L);

    if (ulRet != (ULONG)A116_NO_FILE) {
        SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETTOPINDEX, (WORD)ulRet, 0L);
    }
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    A116GetPluNo();
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               BOOL        fVer      - Label Type
*               LPSTR       lpszPluNo - address of PLU No.
*
**  Return  :   TRUE                  - user process is executed
*               FALSE                 - invalid PLU No.
*
**  Description:
*       This function get PLU No. as PLU formated data.
* ===========================================================================
*/
DWORD   A116GetPluNo(BOOL fVer, WCHAR* lpszPluNo )
{
	LABELANALYSIS Label = {0};   /* ###ADD Saratoga */
    BOOL          sStat;   /* ###ADD Saratoga */

    /* ----- Set PLU label type flag ----- */
    if(fVer == LA_NORMAL) {
       Label.fchModifier = LA_NORMAL;   /* Normal        */
    } else {
       Label.fchModifier = LA_EMOD;     /* UPC-E Version */
    }

    memcpy(Label.aszScanPlu, lpszPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

    /* ----- Analyze PLU No. ----- */
    sStat = RflLabelAnalysis(&Label);

    if(sStat == LABEL_ERR) {
       return ((DWORD)A116_ERR);
    } else {                   /* sStat == LABEL_OK */
       if(Label.uchType == LABEL_RANDOM) {
          if(Label.uchLookup == LA_EXE_LOOKUP) {
             memcpy(lpszPluNo, Label.aszMaskLabel, PLU_MAX_DIGIT * sizeof(WCHAR));
             return A116_OK;
          } else {
             /* ----- Display caution message ----- */
             return ((DWORD)A116_LOOKUP_ERR);
          }
       } else {
             memcpy(lpszPluNo, Label.aszLookPlu, PLU_MAX_DIGIT * sizeof(WCHAR));
             return A116_OK;
       }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116DelPromoPlu()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   char    chType  -   Current Menu-ListBox Type
*
**  Return      :   No return values.
*
**  Description :
*       This function deletes current selected promotional / linked menu plu.
* ===========================================================================
*/
VOID    A116DelPromoPlu(HWND hDlg, char chType)
{
    MENUPLU MenuPlu;                    /* Structure for A116MakeDesc */
    short   nEndAddr,                   /* End Addr. for Roop       */
            nCount = 0;                 /* Loop Counter             */
    WORD    wIndex;                     /* Target Index of ListBox  */
    WCHAR    szWork[A116_DESC_LEN],      /* Description for PLU      */
            szMessage[A116_BUFF_LEN],   /* Delete Caution Message   */
            szCaption[PEP_CAPTION_LEN]; /* MessageBox Caption       */
    BOOL    fRet,                       /* MessageBox Return Value  */
            fCheck = FALSE,             /* Current Promo. PLU Exist */
            fExist = FALSE;             /* Data Exist Check Flag    */

    for (nCount = 0; nCount < A116_ADDR_MAX; nCount++) {
		WCHAR   szPLU[PLU_MAX_DIGIT + 1] = {0};

        /* ----- Determine Whether Data Exist in Selected Index or Not ----- */
        memcpy(szPLU, ((lpPromo + nPromoIndex)->aMenuPLU[nCount].aszPLUNumber), PLU_MAX_DIGIT * sizeof(WCHAR));
        if(memcmp(szPLU, A116_ZEROPLU, PLU_MAX_DIGIT * sizeof(WCHAR)) != 0){
            fExist = TRUE;
            break;
        }
    }

    if (fExist == TRUE) {   /* Promo./Linked PLU Data Exist in Selected Index */
        if ((nFieldIndex == A116_PART_ADDR) && (bDelConf == A116_DELCONF_ON)) {
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szCaption, PEP_STRING_LEN_MAC(szCaption));
            LoadString(hResourceDll, IDS_A116_CAUTION, szMessage, PEP_STRING_LEN_MAC(szMessage));

            MessageBeep(MB_ICONEXCLAMATION);
            fRet = MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OKCANCEL | MB_ICONEXCLAMATION);

            if (fRet == IDCANCEL) {     /* User Selection is CANCEL */
                return;
            }
        }
    }

    /* ----- Set End Address for Loop Counter ----- */
    nEndAddr = (short)((nFieldIndex == A116_PART_ADDR) ? A116_CHILD_MAX : nFieldIndex);

    for (nCount = nFieldIndex; nCount <= nEndAddr; nCount++) {
        /* ----- Reset Data Buffer ----- */
        memset(((lpPromo + nPromoIndex)->aMenuPLU[nCount].aszPLUNumber), 0x00,  PLU_MAX_DIGIT);
        (lpPromo + nPromoIndex)->aMenuPLU[nCount].aszPLUNumber[PLU_MAX_DIGIT] = 0x00;
        (lpPromo + nPromoIndex)->aMenuPLU[nCount].uchAdjective = 0;
    }

    /* ----- Set Initialized PLU No to EditText ------ */
    SetDlgItemInt(hDlg, IDD_A116_PLU, 0, FALSE);
    SetDlgItemInt(hDlg, IDD_A116_ADJ, 0, FALSE);

    /* ----- Reset Structure for A116MakeDesc Function ----- */
    memset((MenuPlu.aszPLUNumber), 0x00, PLU_MAX_DIGIT);
    MenuPlu.aszPLUNumber[PLU_MAX_DIGIT] = 0x00;

    MenuPlu.uchAdjective = 0;

    /* ----- Current LitBox Type is Child & Selected Parent PLU ----- */
    if ((! (chType & A116_PART_TYPE)) && (nFieldIndex == A116_PART_ADDR)) {
        /* ----- Reset Contents of Menu ListBox ----- */
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_RESETCONTENT, 0, 0L);

        for (nCount = 0; nCount <= A116_CHILD_MAX; nCount++) {
            /* ----- Create Description for Insert ListBox ('Not Used') ----- */
            A116MakeDesc((LPMENUPLU)&MenuPlu, szWork, (WORD)nCount);
            /* ----- Insert 'Not Used' Description to ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A116_MENULIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
        }

        /* ----- Set HighLight Bar to ListBox ----- */
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETCURSEL,   0, 0L);
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETTOPINDEX, 0, 0L);
    } else {
        if (nFieldIndex == A116_PART_ADDR) {
            /* ----- Set Index of ListBox ----- */
            wIndex = (WORD)(nPromoIndex);
            /* ----- Create Description for Insert ListBox ('Not Used') ----- */
            A116MakeDesc((LPMENUPLU)&MenuPlu, szWork, (WORD)(wIndex + 1));
        } else {
            /* ----- Set Index of ListBox ----- */
            wIndex = (WORD)(nFieldIndex);
            /* ----- Create Description for Insert ListBox ('Not Used') ----- */
            A116MakeDesc((LPMENUPLU)&MenuPlu, szWork, (WORD)(wIndex));
        }

        /* ----- Delete Old Description from ListBox ----- */
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_DELETESTRING, wIndex, 0L);

        /* ----- Insert New Description into ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A116_MENULIST, LB_INSERTSTRING, wIndex, (LPARAM)(szWork));
    
        /* ----- Set HighLight Bar to ListBox ----- */
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETCURSEL,   wIndex, 0L);
        SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETTOPINDEX, wIndex, 0L);
    }

    /* ----- Delete HighLight Bar from PLU Record ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETCURSEL, (WPARAM)-1, 0L);
    SetFocus(GetDlgItem(hDlg, IDD_A116_MENULIST));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116MakeDesc()
*
**  Input       :   LPMENUPLU   lpMenuPlu   -   Address of PLU No./Adjective
*                   LPSTR       lpszString  -   Address of Description
*                   WORD        wIndex      -   Index of ListBox Contents
*
**  Return      :   No return values.
*
**  Description :
*       This function creates the description for insert menu PLU ListBox with
*   PLU No. and Adjective Code, Index of ListBox Contets.
* ===========================================================================
*/
VOID    A116MakeDesc(LPMENUPLU lpMenuPlu, LPWSTR lpszString, WORD wIndex)
{
    PLUIF   Pif;                            /* Structure of PLU Data        */
    LPCWSTR  lpszMnemonic;                   /* Target PLU Mnemonic Pointer  */
    DWORD   dwPrice;                        /* Loaded PLU Price Data        */
    short   nReturn;                        /* Return Value of OpPluIndRead */
    WCHAR   szMnemo[A116_MNE_LEN + 1],      /* PepReplaceMnemonic Work Area */
            szPrice[A116_MNE_LEN],          /* Strings of Price Data        */
            szNoFile[A116_MNE_LEN + 1];     /* PLU Mnemonic of Not in File  */
    UCHAR   uchPluType = 0;

    /*NCR2172*/
    WCHAR   szPLU[PLU_MAX_DIGIT + 1];
    WCHAR   szWorkPLU[PLU_MAX_DIGIT + 1];

    USHORT  uI;
    BOOL    bWork  = TRUE;
    BOOL    bInput = FALSE;

    
    /* ----- Initialize Mnemonic Work Area ----- */
	memset(szMnemo,0x00,sizeof(szMnemo));
    memset(szPLU, 0x00, sizeof(szPLU));
    memcpy(szPLU, (lpMenuPlu->aszPLUNumber), PLU_MAX_DIGIT * sizeof(WCHAR));

	bWork  = TRUE;
    for ( uI = 0; uI < PLU_MAX_DIGIT && bWork == TRUE; uI++) {
       if(lpMenuPlu->aszPLUNumber[uI] == 0x00) {
          bWork = FALSE;
       } else {
          bInput = TRUE;
       }
    }
/*** NCR2172
 *   if((memcmp(szPLU, A116_ZEROPLU, PLU_MAX_DIGIT)) == 0){
***/
     if( bInput == FALSE) {
		/* Create 'Not Used' Description */
       LoadString(hResourceDll, IDS_A116_NOTUSE, szMnemo, PEP_STRING_LEN_MAC(szMnemo));

        wsPepf(lpszString, WIDE("%2u ------------- - ------- %s"), wIndex, szMnemo);
        return;
    }

    if (ulPluNum != 0L) {     /* Current PLU File Exist */
        /* ----- Set Target PLU No./Adjective to PLU Structure */
        memcpy(Pif.auchPluNo, lpMenuPlu->aszPLUNumber, PLU_MAX_DIGIT * sizeof(WCHAR));

        Pif.auchPluNo[PLU_MAX_DIGIT] = 0x00;
        Pif.uchPluAdj = (UCHAR)((lpMenuPlu->uchAdjective) ? lpMenuPlu->uchAdjective : A116_ONE_ADJ);
        nReturn = OpPluIndRead(&Pif, 0);

        /* ----- Get Loaded PLU Data Type ----- */
        uchPluType = (BYTE)Pif.ParaPlu.ContPlu.uchItemType;

        /* ----- Adjust Adjective Code with Loaded PLU Type ----- */
        if ((uchPluType == A116_OPEN_01) || (uchPluType == A116_NON_10)) {
            if (lpMenuPlu->uchAdjective) {
                nReturn = OP_NOT_IN_FILE;
            }
        } else {
            if (! lpMenuPlu->uchAdjective) {
                nReturn = OP_NOT_IN_FILE;
            }
        }
    } else {                /* Current PLU File Not Exist */
        nReturn = OP_FILE_NOT_FOUND;
    }

    if ((nReturn == OP_FILE_NOT_FOUND) || (nReturn == OP_NOT_IN_FILE)) {
        /* ----- Set Mnemonic('Not in File') to Description ----- */
        LoadString(hResourceDll, IDS_A116_NOFILE, szNoFile, PEP_STRING_LEN_MAC(szNoFile));
        lpszMnemonic = szNoFile;
        /* ----- Create String for Price Data ----- */
        wsPepf(szPrice, WIDE("%-7u"), 0);
    } else {                    /* Target PLU exists in PLU File */
        /* ----- Translate Double Key Code (0x12 -> '~') ----- */
        PepReplaceMnemonic(Pif.ParaPlu.auchPluName, szMnemo, (A116_MNE_LEN), PEP_MNEMO_READ);
        lpszMnemonic = szMnemo;

        /* ----- Determine Whether PLU Type is Open or Not ----- */
        if (uchPluType == A116_OPEN_01) {
            /* ----- Load Stirng from Resource (Open PLU) ----- */
            LoadString(hResourceDll, IDS_A116_OPEN, szPrice, PEP_STRING_LEN_MAC(szPrice));
        } else {
            /* ----- Convert Price Data (UCHAR -> DWORD) ----- */
            PepConv3Cto4L((LPBYTE)Pif.ParaPlu.auchPrice, &dwPrice);
            /* ----- Create String for Price Data ----- */
            wsPepf(szPrice, WIDE("%-7lu"), dwPrice);
        }
    }

    /* ----- Create Strings for Insert ListBox ----- */
	/* ###ADD saratoga */
    memset(szWorkPLU, 0x00, sizeof(szWorkPLU));

    RflConvertPLU(szWorkPLU, lpMenuPlu->aszPLUNumber);
    wsPepf(szPLU, WIDE("%-14s"), szWorkPLU);
    szPLU[PLU_MAX_DIGIT] = 0x00;

    wsPepf(lpszString, WIDE("%2u \t%s \t%u\t%-7s \t%s"), wIndex, szPLU, lpMenuPlu->uchAdjective, szPrice, lpszMnemonic);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116ChgList()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   char    chType      -   Menu-ListBox Type
*                   ULONG   ulRecordNo  -   No. of Current Exists PLU Records
*
**  Return      :   No return values.
*
**  Description :
*       This function changes the menu ListBox type.
* ===========================================================================
*/
VOID    A116ChgList(HWND hDlg, char chType, ULONG ulRecordNo)
{
    WORD    wIndex;                     /* Target Index of ListBox      */
    UINT    unAdj;                      /* Adjective No. for EditText   */
    short   nCount;                     /* Loop Counter                 */
    WCHAR   szWork[A116_DESC_LEN];      /* Description for PLU          */

/*NCR2172*/
    WCHAR   szPLU[PLU_MAX_DIGIT+1];     /* PLU No. for Set Edit */
    WCHAR   szWorkPLU[PLU_MAX_DIGIT+1];
    ULONG   ulRet;                      /* Saratoga, Return Value of Function */

    /* ----- Reset Field of Promotional PLU ----- */
    nFieldIndex = 0;

    /* ----- Reset Current Menu-ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_RESETCONTENT, 0, 0L);

    /* ----- Check Current ListBox Type ----- */
    if (chType & A116_PART_TYPE) {    /* Individual PLU -> Promotional PLU */
        wIndex = nPromoIndex;
        /* ----- Set Parent PLU Data to Menu-ListBox ---- */
        for (nCount = 0; nCount < A116_MENU_MAX; nCount++) {
            /* ----- Create Strings with PLU No. ----- */
            A116MakeDesc((LPMENUPLU)&(lpPromo + nCount)->aMenuPLU[A116_PART_ADDR], szWork, (WORD)(nCount + 1));

            /* ----- Insert Description into the Bottom of Menu-ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A116_MENULIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
        }
    } else {                /* Promotional PLU -> Individual PLU */
        wIndex = A116_ADDR_MIN;
        /* ----- Set Parent & Child PLU Data to Menu-ListBox ----- */
        for (nCount = 0; nCount < A116_ADDR_MAX; nCount++) {
            /* ----- Create Strings with PLU No. ----- */
            A116MakeDesc((LPMENUPLU)&(lpPromo + nPromoIndex)->aMenuPLU[nCount], szWork, (WORD)(nCount));
            /* ----- Insert Description into the Bottom of Menu-ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A116_MENULIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
        }
    }

    /* ----- Set High-Light Bar to Selected Index ----- */
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETCURSEL,   wIndex, 0L);
    SendDlgItemMessage(hDlg, IDD_A116_MENULIST, LB_SETTOPINDEX, wIndex, 0L);
    ulRet = A116FindPluNo((LPMENUPLU)&(lpPromo + nPromoIndex)->aMenuPLU[A116_PART_ADDR], ulRecordNo);
    SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETCURSEL,   (WORD)ulRet, 0L);
    SendDlgItemMessage(hDlg, IDD_A116_PLULIST, LB_SETTOPINDEX, (WORD)ulRet, 0L);

    /* ----- Set Selected PLU No./Adjective to Work Area ------ */
    memset(szPLU, 0x00, sizeof(szPLU));
    memset(szWorkPLU, 0x00, sizeof(szWorkPLU));

    RflConvertPLU(szWorkPLU, (WCHAR*)(lpPromo + nPromoIndex)->aMenuPLU[A116_PART_ADDR].aszPLUNumber);
    wsPepf(szPLU, WIDE("%13s"), szWorkPLU);

    unAdj = (UINT)(lpPromo + nPromoIndex)->aMenuPLU[A116_PART_ADDR].uchAdjective;

    /* ----- Set Selected PLU No./Adjective to EditText ----- */
    DlgItemRedrawText(hDlg, IDD_A116_PLU, szPLU);
    SetDlgItemInt(hDlg, IDD_A116_ADJ, unAdj, FALSE);
}

/*
* ===========================================================================
**  Synopsis    :   ULONG   A116FindPluNo()
*
**  Input       :   LPMENUPLU   lpMenuPlu   -   Address of MENUPLU Structure
*                   ULONG       ulRecordNo  -   No. of Current Exists PLU
*
**  Return      :   Offset No. of PLU in File.  -   Exist PLU Data.
*                   A116_NO_FILE                -   Not Exist PLU Data
*
**  Description :
*       This function searches the target PLU records for the current exists
*   PLU file with PLU No. and Adjective. It returns offset No. of PLU file, if
*   it found the target record. Otherwise it returns A116_NO_FILE.
* ===========================================================================
*/
ULONG   A116FindPluNo(LPMENUPLU lpMenuPlu, ULONG ulRecordNo)
{
    ULONG   ulRet  = (ULONG)A116_NO_FILE;

    if (ulRecordNo != 0L) {         /* Current PLU File exist */
		ULONG    ulCount;
		WCHAR    chFind = A116_NO_FILE;

        for (ulCount = 0; ((ulCount < ulRecordNo) && (chFind != A116_EXIST)); ulCount++) {
            if((memcmp(((lpPluData + ulCount)->aszPLUNumber), (lpMenuPlu->aszPLUNumber), PLU_MAX_DIGIT * sizeof(WCHAR)) == 0) &&
                ((lpPluData + ulCount)->uchAdjective == lpMenuPlu->uchAdjective)) {

                ulRet   = ulCount;
                chFind = A116_EXIST;
                break;
            }
        }
    }
    return ulRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    ChangeContemt()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DailogBox
*                   LPBYTE  lpbType     -   Address of ListBox Flag
*                   ULONG   ulRecordNo  -   No. of Current PLUs in PLU File 
*
**  Return      :   No return value.
*
**  Description :
*       This function changes the description of the content button & the 
*   gruopbox, and the listbox of the promotional PLUs.
* ===========================================================================
*/
VOID    A116ChgContent(HWND hDlg, LPBYTE lpbType, ULONG ulRecordNo)
{
    UINT    wStrID;
    WCHAR   szDesc[A116_BUFF_LEN], szWork[A116_BUFF_LEN];

    /* ----- Change ListBox Flag ----- */
    *lpbType ^= A116_PART_TYPE;

    /* ----- Set Target Content Button Description ID ----- */
    wStrID = ((*lpbType & A116_PART_TYPE) ? IDS_A116_CONTENT : IDS_A116_PREV);

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, wStrID, szDesc, PEP_STRING_LEN_MAC(szDesc));

    /* ----- Set Loaded String to Content Button ----- */
    DlgItemRedrawText(hDlg, IDD_A116_CONTENT, szDesc);

    /* ----- Set Target GroupBox Description ID ----- */
    wStrID = ((*lpbType & A116_PART_TYPE) ? IDS_A116_MENUPLU : IDS_A116_PARTPLU);

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Create Description by Each ListBox Type ----- */
    if (*lpbType & A116_PART_TYPE) {
        lstrcpyW(szDesc, szWork);
    } else {
        wsPepf(szDesc, szWork, nPromoIndex + 1);
    }

    /* ----- Set Created Description to GroupBox ----- */
    DlgItemRedrawText(hDlg, IDD_A116_GROUP, szDesc);

    /* ----- Reset Menu-ListBox with Each Type ----- */
    A116ChgList(hDlg, *lpbType, ulRecordNo);

    /* ----- Check Value of EditText(PLU) ----- */
    PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A116_PLU, EN_CHANGE), 0L);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116ExecAllocErr()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DailogBox
*                   HGLOBAL hGlbPlu     -   Handle of Global Area (PLU List)
*                   HGLOBAL hGlbPromo   -   Handle of Global Area (Promo. PLU)
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the allocation error message with messagebox.
* ===========================================================================
*/
VOID    A116ExecAllocErr(HWND hDlg, HGLOBAL hGlbPlu, HGLOBAL hGlbPromo)
{
    WCHAR    szCaption[PEP_CAPTION_LEN], szErrMsg[PEP_ALLOC_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A116, szCaption, PEP_STRING_LEN_MAC(szCaption));
    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

    /* ----- Display Error Message ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Allocated Area Free ----- */
    if (hGlbPlu) {              /* Plu File Data Area could Allocate */
        GlobalFree(hGlbPlu);
    }
	
	if (hGlbPromo) {     /* Promotional PLU Data Area could Allocate */
        GlobalFree(hGlbPromo);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A116SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A116SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    int     idEdit = 0;
	PEPSPIN PepSpin = {0};

    /* ----- Initialize Commom Configulation of SpinButton ----- */
    PepSpin.nStep      = A116_SPIN_STEP;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set Target EditText ID ----- */
    idEdit = GetDlgCtrlID((HWND)lParam);

    /* ----- Set Different Configulation of SpinButton ----- */
    if (idEdit == IDD_A116_ADJ_SPIN) {
		// spin button settings for scrollbar IDD_A116_ADJ_SPIN
		// to set a value for IDD_A116_ADJ.
        PepSpin.lMax        = A116_ADJ_MAX;
        PepSpin.lMin        = A116_ADJ_MIN;
        PepSpin.nTurbo      = 0;
        PepSpin.nTurStep    = 0;
		idEdit = IDD_A116_ADJ;             // set control identifier to edit box for data entry.
#if defined(POSSIBLE_DEAD_CODE)
    } else {
		// spin button settings for scrollbar IDD_A116_PLU_SPIN
		// to set a value for IDD_A116_PLU.

		// Following code to set the spin button settings is a holdover
		// from when PLU numbers were integers in a range of 1 to 9999
		// years ago with NCR 2170. This code is no longer used.
		//    Richard Chambers, Nov-07-2018
        PepSpin.lMax        = A116_PLU_MAX;
        PepSpin.lMin        = A116_PLU_MIN;
        PepSpin.nTurbo      = A116_SPIN_TURBO;
        PepSpin.nTurStep    = A116_SPIN_TURSTEP;
		idEdit = IDD_A116_PLU;             // set control identifier to edit box for data entry.
	}
#endif

		if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
			PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(idEdit), EN_CHANGE), 0L);
		}
    }
}


HWND A116ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A116),
                                   hParentWnd,
                                   A116DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A116ModeLessDialogWnd (void)
{
	return hWndModeLess;
}

/* ===== End of File (A116.C) ===== */
