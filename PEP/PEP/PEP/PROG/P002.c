/*
* ---------------------------------------------------------------------------
* Title         :   Flexible Memory Assignment  (Prog. 2)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P002.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-06-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Mar-13-95 : 03.00.00 : H.Ishida   : Add Combination Coupon & Control String Size
* Apr-21-95 : 03.00.00 : H.Ishida   : Add Control String
* Jul-12-95 : 03.00.01 : T.Nakahata : bug fixed (Remove P02ChkMemUse on WM_PAINT)
* Sep-13-95 : 01.00.03 : T.Nakahata : correct calcuration (GC, Item, Cons)
* Sep-14-95 : 01.00.03 : T.Nakahata : max=99 at store/recall system
* Feb-01-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-03-98 : 03.03.00 : A.Mitsui   : Remove Server file and Change Cashier file V3.3
** GenPOS
* Aug-04-15 : 02.02.02 : R.Chambers : synchronize Number Operators and Number of Employees
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
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

//#include	"stringResource.h" //RPH 4-21-03
#include    <ecr.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <paraequ.h>
#include    "plu.h"
#include    <para.h>
#include    <csop.h>
#include    <cscas.h>
#include    <csetk.h>
#include    <csprgrpt.h>
#include    <r20_pif.h>
#include    "pep.h"

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "pepcalc.h"
#include    "file.h"
#include    "trans.h"
#include    "p002.h"
#include    "mypifdefs.h"
#include	"../RES/resource.h"

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
static LPCFCONFIG   lpFile;
static LPP02STR     lpStr;
static char         chTerm;
static BOOL         fMemChk;
static RECT         rArea = {
                        P02_AREALEFT,
                        P02_AREATOP,
                        P02_AREARIGHT,
                        P02_AREABOTTOM
                    };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P002DlgProc()
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
*       This is a dialgbox procedure for the Flexible Memory Assignment.
* ===========================================================================
*/
BOOL    WINAPI  P002DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static DWORD    dwFreeBlock,
                    dwTotalMem,
                    adwMem[MAX_FLXMEM_SIZE];
    static P02PARA  Para;
    static HGLOBAL  hMem;
	WCHAR  szWork[128];
    WORD        wIndex,
                wMode;
    WORD            wData;
    BOOL            fChk;
    DWORD           dwIndex;
    UINT            unGC;
	int		j;
    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Initialize dialogbox ----- */
        if (P02InitDlg(hDlg, &dwFreeBlock, adwMem, &Para, &hMem) == TRUE) {
            /* configration error */
            /* ----- Destroy dialogbox ----- */
            EndDialog(hDlg, FALSE);
        }
        for (dwTotalMem = 0L, wIndex = 0; wIndex < MAX_FLXMEM_SIZE; wIndex++) {
            dwTotalMem += adwMem[ wIndex ];
        }

        /* ----- Check out of memory ----- */
        fChk = P02ChkMemUse(hDlg, dwTotalMem, dwFreeBlock); 
		{
			char *szRAMSize = NULL;

			if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_8MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_8MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_16MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_16MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_24MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_24MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_32MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_32MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_40MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_40MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else if (lpFile->usMemory == (FILE_SIZE_64KB * FILE_SIZE_48MBTOKB)) {
				LoadString(hResourceDll, IDS_P02_RAM_48MB, szWork, PEP_STRING_LEN_MAC(szWork));
			} else {
				LoadString(hResourceDll, IDS_P02_RAM_UNKNOWN, szWork, PEP_STRING_LEN_MAC(szWork));
			}
			
			DlgItemRedrawText (hDlg, IDC_P02_RAM_SIZE, szWork);
		}
        /* ----- Enable/Disable to push OK button ----- */
        EnableWindow(GetDlgItem(hDlg, IDOK), fChk);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_P02EDIT_DEPT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_PLU, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_CAS, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_EJ, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_ETK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_ITEM, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_CONS, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_GC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_CPN, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_CSTR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_RPT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02EDIT_PPI, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02PTD_GC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDC_P02_RAM_SIZE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P02_MAXGC, WM_SETFONT, (WPARAM)hResourceFont, 0);
		
			for (j = IDD_P02_TERMINAL; j <= IDD_P02_ITEMB_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}

			for (j = IDD_P02_ITEM; j <= IDD_P02BYTE; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_PAINT:
        /* ----- Draw graphical memory usage ----- */
        dwTotalMem = P02DrawMemUse(hDlg, dwFreeBlock, adwMem);
        return TRUE;

    case PM_GET_FCONF:
        /* ----- Receive Address of File Configulation from PEP:Main ----- */
        lpFile = (LPCFCONFIG)lParam;
        return TRUE;

    case WM_VSCROLL:
        /* ----- Spin button procedure ----- */
        P02SpinProc(hDlg, wParam, lParam, adwMem);
        /* ----- An Application should return zero ----- */
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P02EDIT_DEPT:  /* department edit-box entry */
        case IDD_P02EDIT_PLU:   /* PLU edit-box entry */
/*        case IDD_P02EDIT_SER:   server edit-box entry */
        case IDD_P02EDIT_CAS:   /* operator edit-box entry */
        case IDD_P02EDIT_EJ:    /* E/J edit-box entry */
        case IDD_P02EDIT_ETK:   /* ETK edit-box entry */
        case IDD_P02EDIT_ITEM:  /* Item Edit-box entry */
        case IDD_P02EDIT_CONS:  /* Cons Edit-box entry */
        case IDD_P02EDIT_GC:    /* Guest Check */
        case IDD_P02EDIT_CPN:   /* Combination Coupon edit-box entry */
        case IDD_P02EDIT_CSTR:  /* Control String Size edit-box entry */
        case IDD_P02EDIT_RPT:   /* Programmable Report */
        case IDD_P02EDIT_PPI:   /* PPI */
            if (HIWORD(wParam) == EN_CHANGE) {      /* data change */
                wData = (WORD)GetWindowTextLength((HWND)lParam);
                if (wData == 0) {
                    return TRUE;
                }

                /* ----- Set mode number ----- */
                wIndex = (WORD)(LOWORD(wParam) - IDD_P02EDIT);

                /* ----- Calculate No. of blocks ----- */
                if (LOWORD(wParam) == IDD_P02EDIT_ITEM || LOWORD(wParam) == IDD_P02EDIT_CONS) {
                    wData = (WORD)GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
                      if (wData >= FLEX_ITEMSTORAGE_MIN) {  
                        adwMem[wIndex] = P02CalcBlock(hDlg, wIndex);
                      }
                } else {
                    adwMem[wIndex] = P02CalcBlock(hDlg, wIndex);
                }
                for (dwTotalMem = 0L, wIndex = 0; wIndex < MAX_FLXMEM_SIZE; wIndex++) {
                    dwTotalMem += adwMem[ wIndex ];
                }

                /* ----- Check out of memory ----- */
                fChk = P02ChkMemUse(hDlg, dwTotalMem, dwFreeBlock); 

                /* ----- Enable/Disable to push OK button ----- */
                EnableWindow(GetDlgItem(hDlg, IDOK), fChk);
                return TRUE;
            }
            return FALSE;

        case IDD_P02PTD_DEPT:   /* department PTD check-box entry */
        case IDD_P02PTD_PLU:    /* PLU PTD check-box entry */
/*        case IDD_P02PTD_SER:     server PTD check-box entry */
        case IDD_P02PTD_CPN:    /* Combination Coupon PTD check-box entry */
        case IDD_P02PTD_CAS:    /* operator PTD check-box entry */
            if (HIWORD(wParam) == BN_CLICKED) {     /* checkbox click */
                /* ----- Set mode number ----- */
                wIndex = (WORD)(LOWORD(wParam) - IDD_P02PTD);

                /* ----- Calculate No. of blocks ----- */
                adwMem[wIndex] = P02CalcBlock(hDlg, wIndex);

                /* ----- Check out of memory ----- */
                for (dwTotalMem = 0L, wIndex = 0; wIndex < MAX_FLXMEM_SIZE; wIndex++){
                    dwTotalMem += adwMem[ wIndex ];
                }
                fChk = P02ChkMemUse(hDlg, dwTotalMem, dwFreeBlock); 
                /* ----- Enable/Disable to push OK button ----- */
                EnableWindow(GetDlgItem(hDlg, IDOK), fChk);
                return TRUE;
            }
            return FALSE;

        case IDD_P02PTD_GC:    /* guest check PTD combo-box entry */
            if (HIWORD(wParam) == CBN_SELCHANGE) {  /* select change */
                /* --- display range of gc# with selected system --- */
                dwIndex = SendDlgItemMessage( hDlg, LOWORD(wParam), CB_GETCURSEL, 0, 0L );
                P02DispGCMax( hDlg, ( UCHAR )dwIndex );

                /* ----- Calculate No. of blocks ----- */
                adwMem[P02_GC] = P02CalcBlock(hDlg, P02_GC);
                adwMem[P02_CONS] = P02CalcBlock(hDlg, P02_CONS);

                unGC = GetDlgItemInt( hDlg, IDD_P02EDIT_GC, NULL, FALSE);
                if ( dwIndex == (DWORD)FLEX_STORE_RECALL ) {
                    if ( P02_STOREC_GCMAX < unGC ) {
                        EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
                        return TRUE;
                    }
                }

                /* ----- Check out of memory ----- */
                for (dwTotalMem = 0L, wIndex = 0; wIndex < MAX_FLXMEM_SIZE; wIndex++) {
                    dwTotalMem += adwMem[ wIndex ];
                }
                fChk = P02ChkMemUse(hDlg, dwTotalMem, dwFreeBlock); 
                /* ----- Enable/Disable to push OK button ----- */
                EnableWindow(GetDlgItem(hDlg, IDOK), fChk);
                return TRUE;
            }
            return FALSE;

        case IDD_P02RATE:       /* rate radio-button selected */
        case IDD_P02BYTE:       /* byte radio-button selected */
            /* ----- Set mode number ----- */
            wMode = (WORD)(LOWORD(wParam) - IDD_P02RATE);

            /* ----- Display % rate/byte  ----- */
            P02DispMemRate(hDlg, dwTotalMem, &dwFreeBlock, wMode);
            return TRUE;

        case IDOK:
        case IDCANCEL:
            /* ----- Get data and save them to file ----- */
            if (P02FinDlg(hDlg, &Para, hMem, wParam) == TRUE) {
                OpPluFileUpdate(0);

                /* ----- Destroy dialogbox ----- */
                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02InitDlg()
*
*   Input   :   HWND        hDlg    - handle of a dialogbox procedure
*               LPDWORD     lpdwFree - address of free block
*               LPDWORD     lpwMem  - address of memory useage
*               LPP02PARA   lpPara  - address of data buffer
*               LPHGLOBAL   lphMem  - handle of global memory lock for description
**
*   Return  :   TRUE                - error occured
*               FALSE               - user process is executed
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
BOOL    P02InitDlg(HWND hDlg, LPDWORD lpdwFree, LPDWORD lpdwMem,
                            LPP02PARA lpPara, LPHGLOBAL lphMem)
{
    WORD                wI, wIdx, wI2;
	UINT                wID, wTerm;
    USHORT              usReturnLen;
    WCHAR               szCap[PEP_CAPTION_LEN],
                        szErr[PEP_ALLOC_LEN],
                        szTerm[P02_TERM_LEN];
    WCHAR               szRecNo[PLU_MAX_DIGIT + 1];


//	InitDialogStrings(hDlg); 
	/* ----- Initialize memory check flag ----- */
    fMemChk = FALSE;

    /* ----- Allocate memory from global area ----- */
    *lphMem = GlobalAlloc(GHND, sizeof(P02STR));

    /* ----- Check whether keep memory or not ----- */
    if (*lphMem == 0) {
        /* ----- Get description from resource ----- */
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szErr, PEP_STRING_LEN_MAC(szErr));
        LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErr, szCap, MB_OK | MB_ICONEXCLAMATION);
        return TRUE;
    }

    /* ----- Lock memory area ----- */
    lpStr = (LPP02STR)GlobalLock(*lphMem);

    /* ----- Get description from resource ----- */
	/*added an additional FOR loop to accomodate the change, renamed some of the resources that it was reading
	from IDS_P02_.... to IDS_PEP_.... so that it can be used in multiple places instead of making new mnemonics up that were
	already defined.*/
    for (wI = 0, wID = IDS_PEP_FILE_DEPT; wI < P02_DSCRB_NO, wID <= IDS_PEP_FILE_PPI; wI++, wID++) {
		LoadString(hResourceDll, wID, lpStr->aszDscrb[wI], P02_DSCRB_LEN);
    } 
	for ( wI2 = wI, wID = IDS_P02_FILE_CLR; wI2 < P02_DSCRB_NO; wI2++, wID++){
		LoadString(hResourceDll, wID, lpStr->aszDscrb[wI2], P02_DSCRB_LEN);
	}

    /* ----- Read data from file ----- */
    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)lpPara->aCurt,
                sizeof(lpPara->aCurt), 0, (USHORT *)&usReturnLen);

    /* ----- Check RAM size and, get free block in file configration ----- */
    if (P02ChkFileConfig(hDlg, lpdwFree) == TRUE) {
        return TRUE;
    }

    /* ----- Set max entry data length ----- */
    SendDlgItemMessage(hDlg, IDD_P02EDIT_DEPT, EM_LIMITTEXT, P02_DEPT_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_PLU, EM_LIMITTEXT, P02_PLU_LEN, 0L);
/*    SendDlgItemMessage(hDlg, IDD_P02EDIT_SER, EM_LIMITTEXT, P02_SER_LEN, 0L); */
    SendDlgItemMessage(hDlg, IDD_P02EDIT_CAS, EM_LIMITTEXT, P02_CAS_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_EJ, EM_LIMITTEXT, P02_EJ_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_ETK, EM_LIMITTEXT, P02_ETK_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_GC, EM_LIMITTEXT, P02_GC_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_ITEM, EM_LIMITTEXT, P02_ITEM_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_CONS, EM_LIMITTEXT, P02_CONS_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_CPN, EM_LIMITTEXT, P02_CPN_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_CSTR, EM_LIMITTEXT, P02_CSTR_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_RPT, EM_LIMITTEXT, P02_RPT_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_P02EDIT_PPI, EM_LIMITTEXT, P02_PPI_LEN, 0L);

    /* ----- Set guest check type description to combo-box ----- */
    for (wI = 0; wI < P02_GCDSCRB_NO; wI++) {
		DlgItemSendTextMessage (hDlg, IDD_P02PTD_GC, CB_INSERTSTRING,
                 (WPARAM)-1, (LPARAM)(lpStr->aszDscrb[P02_GCDSCRB1 + wI]));
	}

    /* ----- Store initial data for checking chage ----- */
    memcpy(lpPara->aPrev, lpPara->aCurt, sizeof(FLEXMEM) * MAX_FLXMEM_SIZE);

    /* ----- Set data to the edit-box ----- */
    for (wI = 0, wID = IDD_P02EDIT; wI < MAX_FLXMEM_SIZE; wI++, wID++) {
        if( wI == 1) {
          _ltow(lpPara->aCurt[wI].ulRecordNumber, szRecNo, 10);
		  DlgItemRedrawText(hDlg, wID, szRecNo);
        } else {
          SetDlgItemInt(hDlg, wID, lpPara->aCurt[wI].ulRecordNumber, FALSE);
        }
    }

    /* ----- Set PTD data to the check-box ----- */
    for (wI = 0, wID = IDD_P02PTD; wI < P02_PTD_NO; wI++, wID++) {
        SendDlgItemMessage(hDlg, wID, BM_SETCHECK, lpPara->aCurt[wI].uchPTDFlag, 0L);
    }

    /* ----- Set Coupon PTD data to check-box ----- */
	SendDlgItemMessage(hDlg, IDD_P02PTD_CPN, BM_SETCHECK, lpPara->aCurt[P02_CPN].uchPTDFlag, 0L);

    /* ----- Set E/J overwirte data to check-box ----- */
    SendDlgItemMessage(hDlg, IDD_P02PTD_EJ, BM_SETCHECK, lpPara->aCurt[P02_EJ].uchPTDFlag, 0L);

    /* ----- Set guest check type to combo-box ----- */
	SendDlgItemMessage(hDlg, IDD_P02PTD_GC, CB_SETCURSEL, lpPara->aCurt[P02_GC].uchPTDFlag, 0L);

    /* --- set maximum no. of gc to static text --- */
    P02DispGCMax( hDlg, lpPara->aCurt[P02_GC].uchPTDFlag );

    /* ----- Initialize memory usage ----- */
    for (wIdx = 0; wIdx < MAX_FLXMEM_SIZE; wIdx++) {
        *(lpdwMem++) = P02CalcBlock(hDlg, wIdx);
    }

    /* ----- Set terminal type ----- */
    wTerm = ((chTerm == P02_TERM_MASTER) ? IDS_FILE_TERM_MASTER : IDS_FILE_TERM_SATELLITE);

    LoadString(hResourceDll, wTerm, szTerm, PEP_STRING_LEN_MAC(szTerm));
	DlgItemRedrawText(hDlg, IDD_P02_TERM, szTerm);

    /* ----- Set rate radio-button ----- */
    SendDlgItemMessage(hDlg, IDD_P02RATE, BM_SETCHECK, TRUE, 0L);

    /* ----- Set foucus to department editbox and selected ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P02EDIT_DEPT));

    SendDlgItemMessage(hDlg, IDD_P02EDIT_DEPT, EM_SETSEL, 1, MAKELONG(0, -1));
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ChkFileConfig()
*
*   Input   :   HWND        hDlg     - handle of a dialogbox procedure
*               LPDWORD     lpdwFree - address of free block
**
*   Return  :   TRUE                 - error occured
*               FALSE                - user process is executed
*
**  Description:
*       This procedure check ram size from file configuration,
*       and get free blocks.
* ===========================================================================
*/
BOOL    P02ChkFileConfig(HWND hDlg, LPDWORD lpdwFree)
{
    WCHAR    szCap[PEP_CAPTION_LEN];

    /* ----- Get file config address ----- */
//    SendMessage(hwndProgMain, PM_GET_FCONF, (WPARAM)hDlg, (LPARAM)lpFile);

	lpFile = FileConfigGet();
    /* ----- Calculate free block ----- */
    if ((ULONG)lpFile->usMemory * P02_64KB < lpFile->ulStartAddr) {
		*lpdwFree = 0;
        /* ----- Get description from resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, lpStr->aszDscrb[P02_OVERMEM], szCap, MB_ICONEXCLAMATION|MB_OK);
        return TRUE;
    } else {
		*lpdwFree = (DWORD)((((ULONG)(lpFile->usMemory) * P02_64KB) - lpFile->ulStartAddr) / P02_BLKBYTE);
    }
    /* ----- Get terminal type ----- */
    chTerm = lpFile->chTerminal;

    if (chTerm == P02_TERM_MASTER) {         /* master terminal */
        /* ----- Consider total file size ----- */
        *lpdwFree -= (DWORD)PEP_CALC_FILE_IDX;
        *lpdwFree -= (DWORD)PEP_CALC_FILE_BASETTL;
/*        *lpwFree -= PEP_CALC_FILE_SHARBUF + PEP_CALC_FILE_IDXBLK + PEP_CALC_FILE_IDXBLK;  */
        *lpdwFree -= (DWORD)(PEP_CALC_FILE_SHARBUF + PEP_CALC_FILE_IDXBLK);    /* bugfix, V3.3 */
        *lpdwFree -= P02CalcIndexBlock(PEPCALCINDTTL(lpFile->chNoOfSatellite + 1));
        *lpdwFree -= P02CalcIndexBlock(PEPCALCINDTTL(P02_INDTTL_MAX));
        /* *lpdwFree -= P02CalcIndexBlock(PEPCALCINDTTL(lpFile->chNoOfSatellite + 1)); */
    } else {                                /* satellite terminal */
        /* ----- Consider total file size ----- */
        *lpdwFree -= (DWORD)(PEP_CALC_FILE_IDX + PEP_CALC_FILE_SHARBUF + PEP_CALC_FILE_IDXBLK);   /* bugfix, V3.3 */
    }

    /* ----- Decrease MLD file size ,V3.3 ----- */
    *lpdwFree -= (DWORD)PEP_CALC_FILE_MLDBLK;
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis:   WORD    P02CalcBlock()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               WORD    wIdx    - index of edit control
**
*   Return  :   WORD    wBlock  - number of using block
*
**  Description:
*       This procedure calculate number of block on the memory.
* ===========================================================================
*/
DWORD    P02CalcBlock(HWND hDlg, WORD wIdx)
{
    P02BUFF Buff;
    DWORD   dwBlock;
    RECT    rcDraw;
    int     nLen;

    /* ----- Get data from the edit-box ----- */
    if(wIdx == P02_PLU) {
		WCHAR    szPluRecord[P02_PLU_LEN + 1] = {0};

       nLen = DlgItemGetText(hDlg, IDD_P02EDIT + wIdx, szPluRecord, sizeof(szPluRecord));
       /* ----- Convert strings to long ----- */
       Buff.dwData = _wtol(szPluRecord);
    } else {
       Buff.dwData = (DWORD)GetDlgItemInt(hDlg, IDD_P02EDIT + wIdx, NULL, FALSE);
    }

    /* ----- Get PTD data from the PTD selection ----- */
    if (wIdx == P02_GC) {
        Buff.bPtd = (BYTE)SendDlgItemMessage(hDlg, IDD_P02PTD_GC, CB_GETCURSEL, 0, 0L);
    } else {
        Buff.bPtd = (BYTE)SendDlgItemMessage(hDlg, IDD_P02PTD + wIdx, BM_GETCHECK, 0, 0L);
    }

    /* ----- Check whether the data over max or not ----- */
    if (P02ChkRng(hDlg, &Buff, wIdx) == TRUE) {       /* data over maximum */
        SendMessage(GetDlgItem(hDlg, IDD_P02EDIT + wIdx), EM_UNDO, 0, 0L);
		if(wIdx == P02_PLU) {
			WCHAR    szPluRecord[P02_PLU_LEN + 1] = {0};

			nLen = DlgItemGetText(hDlg, IDD_P02EDIT + wIdx, szPluRecord, sizeof(szPluRecord));
			/* ----- Convert strings to long ----- */
			Buff.dwData = _wtol(szPluRecord);
		} else {
			Buff.dwData = (DWORD)GetDlgItemInt(hDlg, IDD_P02EDIT + wIdx, NULL, FALSE);
		}
       /* ----- Set focus ----- */
       SetFocus(GetDlgItem(hDlg, IDD_P02EDIT + wIdx));
    }

    if ( wIdx == P02_ITEM ) {
        dwBlock = P02CalcSize_Item( &Buff );
    } else if ( wIdx == P02_CONS ) {
        dwBlock = P02CalcSize_Cons( hDlg, &Buff );
    } else {
        /* ----- Calculate file size ---- */
        P02CalcSize(&Buff, wIdx);

        /* ----- Calculate block size ----- */
        dwBlock = P02CalcIndexBlock(Buff.dwPara);
        dwBlock += P02CalcIndexBlock(Buff.dwTotal);
        if (wIdx == P02_GC) {
            if (chTerm == P02_TERM_MASTER) {
                if (Buff.bPtd == FLEX_STORE_RECALL) { /* store/recall */
                    dwBlock += (DWORD)PEP_CALC_FILE_SERVTIMETTL;
                }
            }
        }
    }

    /* ----- Redraw grph area ----- */
    GetClientRect( hDlg, &rcDraw);
    rcDraw.left   = P02_CLIENT_LEFT(rcDraw.right);
    rcDraw.right  = P02_CLIENT_RIGHT(rcDraw.right) + 29;
    rcDraw.top    = P02_CLIENT_TOP(rcDraw.bottom);
    rcDraw.bottom = P02_CLIENT_BOTTOM(rcDraw.bottom);

    InvalidateRect(hDlg, &rcDraw, TRUE);
    return (dwBlock);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P02CalcSize()
*
*   Input   :   LPP02BUFF   lpBuff         - address of buffer
*               WORD        wIndex         - index of editbox
*
*               WORD        lpBuff->dwData - item data in editbox
*               BYTE        lpBuff->bPtd   - check of PTD in checkbox/combo-box
*
*   Output  :   DWORD       lpBuff->wPara  - parameter file size
*               DWORD       lpBuff->wTotal - total file size
**
*   Return  :   None
*
**  Description:
*       This procedure calculate file size on the memory.
* ===========================================================================
*/
VOID    P02CalcSize(LPP02BUFF lpBuff, WORD wIdx)
{
    if (lpBuff->dwData != 0L) {     /* data is not 0 */
        switch (wIdx) {
        case P02_DEPT:
            lpBuff->dwPara  = PEPCALCDEPTPARA(lpBuff->dwData);
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwTotal = PEPCALCDEPTTTL(lpBuff->dwData, lpBuff->bPtd);
            } else {
                lpBuff->dwTotal = 0L;
            }
            break;

        case P02_PLU:
            if (chTerm == P02_TERM_MASTER) {    /* only Master, saratoga */
                lpBuff->dwPara  = PEPCALCPLUPARA(lpBuff->dwData);

/* ###ADD Saratoga */
                lpBuff->dwPara += PEPCALCPLUIDX(lpBuff->dwData);
            } else {
                lpBuff->dwPara  = 0;
            }
            lpBuff->dwPara += PEPCALCOEPPARA(lpBuff->dwData);

            if (chTerm == P02_TERM_MASTER) {
				if (lpBuff->dwData > P02_PLUTTL_MAX) {
                    lpBuff->dwTotal = PEPCALCPLUTTL(P02_PLUTTL_MAX, lpBuff->bPtd);
                } else {
                    lpBuff->dwTotal = PEPCALCPLUTTL(lpBuff->dwData, lpBuff->bPtd);
                }
            } else {
                lpBuff->dwTotal = 0L;
            }
            break;

/*
        case P02_SER:
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwPara  = PEPCALCSERPARA(lpBuff->wData);
                lpBuff->dwTotal = PEPCALCSERTTL(lpBuff->wData, lpBuff->bPtd);
            } else {
                lpBuff->dwPara = 0L;
                lpBuff->dwTotal = 0L;
            }
            break;

*/
        case P02_CAS:
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwPara  = PEPCALCCASHPARA(lpBuff->dwData);
                lpBuff->dwTotal = PEPCALCCASHTTL(lpBuff->dwData, lpBuff->bPtd);
            } else {
                lpBuff->dwPara = 0L;
                lpBuff->dwTotal = 0L;
            }
            break;

        case P02_EJ:
            lpBuff->dwPara  = 0L;
            if (lpBuff->dwData > P02_EJMIN) {
                lpBuff->dwTotal = PEPCALCEJTTL(lpBuff->dwData);
            } else {
                lpBuff->dwTotal = P02_EJMIN * P02_BLKBYTE;
            }
            break;

        case P02_ETK:
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwPara  = PEPCALCETKPARA(lpBuff->dwData);
                lpBuff->dwTotal = PEPCALCETKTTL(lpBuff->dwData);
            } else {
                lpBuff->dwPara  = 0L;
                lpBuff->dwTotal = 0L;
            }
            break;

        case P02_GC:
            lpBuff->dwPara = 0L;
            if (chTerm == P02_TERM_MASTER) {
                if (lpBuff->bPtd == FLEX_POST_CHK) {      /* post GC */
                    lpBuff->dwTotal = PEPCALCPOSTGC(lpBuff->dwData);
                } else if (lpBuff->bPtd == FLEX_STORE_RECALL) { /* store/recall */
                    lpBuff->dwTotal = PEPCALCSTRGC(lpBuff->dwData);
                } else {                                          /* pre GC */
                    lpBuff->dwTotal = PEPCALCPREGC(lpBuff->dwData);
                }
            } else {
                lpBuff->dwTotal = 0L;
            }
            break;

        case P02_CPN:                                     /* Combination Coupon */
            lpBuff->dwPara  = PEPCALCCPNPARA(lpBuff->dwData);
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwTotal = PEPCALCCPNTTL(lpBuff->dwData, lpBuff->bPtd);
            } else {
                lpBuff->dwTotal = 0L;
            }
            break;

        case P02_CSTR:                                     /* Control String Size */
            lpBuff->dwPara  = PEPCALCCSTRPARA(lpBuff->dwData);
            lpBuff->dwTotal = 0L;
            break;

        case P02_RPT:
            if (chTerm == P02_TERM_MASTER) {
                lpBuff->dwPara  = PEPCALCRPTPARA(lpBuff->dwData);
            } else {
                lpBuff->dwPara  = 0L;
            }
            lpBuff->dwTotal = 0L;
            break;

        case P02_PPI:                                     /* PPI Size */
            lpBuff->dwPara  = PEPCALCPPIPARA(lpBuff->dwData);
            lpBuff->dwTotal = 0L;
            break;

        default:
            lpBuff->dwPara = 0L;
            lpBuff->dwTotal = 0L;
            break;
        }
    } else {            /* data is 0 */
        /* ----- Set file size 0 ----- */
        lpBuff->dwPara = 0L;
        lpBuff->dwTotal = 0L;
    }
}

/*
* ===========================================================================
**  Synopsis:   WORD    P02CalcIndexBlock()
*
*   Input   :   DWORD   dwSize    - file size
**
*   Return  :   WORD    wBlockRet - number of block
*
**  Description:
*       This procedure calculate number of block on the memory.
* ===========================================================================
*/
DWORD  P02CalcIndexBlock(DWORD dwSize)
{
    DWORD   dwBlockRet, dwBlock = 0L;

    /* ----- Count number of block ----- */
    if (dwSize != 0L) {
        while (dwBlock < (DWORD)P02_MAXBLOCK) {
            dwBlock++;
            if (dwSize <= (dwBlock * (DWORD)P02_BLKBYTE)) {
                break;
            }
        }
    }

    /* ----- Consider index file ----- */
    if (dwBlock <= (DWORD)P02_IDXBLKLV1) {
        dwBlockRet = dwBlock;
    } else if ((dwBlock > (DWORD)P02_IDXBLKLV1) && (dwBlock <= (DWORD)P02_IDXBLKLV2)) {
        dwBlockRet = (dwBlock + (DWORD)PEP_CALC_FILE_IDXBLK);
    } else {
        dwBlockRet = (dwBlock + (DWORD)(PEP_CALC_FILE_IDXBLK + PEP_CALC_FILE_IDXBLK)
                        + (dwBlock - (DWORD)P02_IDXBLKLV1) / (DWORD)P02_IDXBLKBYTE);
    }

    return (dwBlockRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ChkAllDataRng()
*
*   Input   :   HWND        hDlg           - handle of a dialogbox procedure
**
*   Return  :   TRUE                       - over range error
*               FALSE                      - default process is expected
*
**  Description:
*       This procedure does the final check of all of the data ranges
*		prior to saving values and exiting the dialog procedure.
* ===========================================================================
*/
BOOL	P02ChkAllDataRng(HWND hDlg)
{
	DWORD	dwMax;
	WORD	wMin, wValue, wID;
	WORD    wCasValue = 0, wEtkValue = 0;
	BOOL	fRet = FALSE, fTrans;
	int i;	//iterator variable

	struct P02Item {
		int valueID;	//the origin of the data being checked (in the dialog)
		WORD wMin;		//the minimum acceptable value to check against
		DWORD dwMax;	//the maximum acceptable value to check against
	} P02Items[] = {
		{IDD_P02EDIT_DEPT,	FLEX_P2_MIN,	FLEX_DEPT_MAX},
		{IDD_P02EDIT_PLU,	FLEX_P2_MIN,	FLEX_PLU_MAX},
		{IDD_P02EDIT_CAS,	FLEX_P2_MIN,	FLEX_CAS_MAX},
		{IDD_P02EDIT_EJ,	FLEX_P2_MIN,	FLEX_EJ_MAX},
		{IDD_P02EDIT_ETK,	FLEX_P2_MIN,	FLEX_ETK_MAX},
		{IDD_P02EDIT_ITEM,	FLEX_ITEMSTORAGE_MIN,	FLEX_ITEMSTORAGE_MAX},
		{IDD_P02EDIT_CONS,	FLEX_ITEMSTORAGE_MIN,	FLEX_ITEMSTORAGE_MAX},
		{IDD_P02EDIT_GC,	FLEX_P2_MIN,	FLEX_GC_MAX},	//guest check default maximum (if store-recall is turned on it should be different)
		{IDD_P02EDIT_CPN,	FLEX_P2_MIN,	FLEX_CPN_MAX},
		{IDD_P02EDIT_CSTR,	FLEX_P2_MIN,	FLEX_CSTRING_MAX},
		{IDD_P02EDIT_RPT,	FLEX_P2_MIN,	FLEX_PROGRPT_MAX},
		{IDD_P02EDIT_PPI,	FLEX_P2_MIN,	FLEX_PPI_MAX}
	};

	//check if store-recall is turned on
	//change the guest-check max accordingly
	if(SendDlgItemMessage(hDlg, IDD_P02PTD_GC, CB_GETCURSEL, 0, 0) == FLEX_STORE_RECALL)
		P02Items[7].dwMax = P02_STOREC_GCMAX;

	for (i = 0; i < sizeof(P02Items)/sizeof(P02Items[0]); i++){
		wID = P02Items[i].valueID;
		wMin = P02Items[i].wMin;
		dwMax = P02Items[i].dwMax;
		
		wValue = (WORD)GetDlgItemInt(hDlg, wID, &fTrans, FALSE);
		if (wID == IDD_P02EDIT_CAS) wCasValue = wValue;
		if (wID == IDD_P02EDIT_ETK) wEtkValue = wValue;
		if ((wValue > dwMax) || (wValue < wMin) || (fTrans == 0)) {
			WCHAR   szWork[PEP_OVER_LEN] = {0}, szErr[PEP_OVER_LEN] = {0}, szCap[PEP_CAPTION_LEN] = {0};

			/* ----- Get description from resource ----- */
			/* ----- Make description for message box ----- */
			LoadString(hResourceDll, IDS_P02_OVERRANGE, szErr, PEP_STRING_LEN_MAC(szErr));			
			wsPepf(szWork, szErr, wMin, dwMax);

			LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
			
			/* ----- Display caution message ----- */
			MessageBeep(MB_ICONEXCLAMATION);
			MessageBoxPopUp(hDlg, szWork, szCap, MB_ICONEXCLAMATION | MB_OK);

			/* ----- Undo last operation in edit control ----- */
			SendMessage(GetDlgItem(hDlg, wID), EM_UNDO, 0, 0L);

			/* ----- Set focus ----- */    
			SetFocus(GetDlgItem(hDlg, wID));

			fRet = TRUE;
			break;
		}
	}
	return fRet;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ChkRng()
*
*   Input   :   HWND        hDlg           - handle of a dialogbox procedure
*               LPP02BUFF   lpBuff         - address of parameter
*               WORD        wIndex         - index of edit-control
*
*               WORD        lpBuff->dwData - item data in editbox
*               BYTE        lpBuff->bPtd   - check of PTD in checkbox/combo-box
**
*   Return  :   TRUE                       - over range error
*               FALSE                      - default process is expected
*
**  Description:
*       This procedure check over data.
* ===========================================================================
*/
BOOL    P02ChkRng(HWND hDlg, LPP02BUFF lpBuff, WORD wIdx)
{
    WORD    wMin = 0;
    DWORD   dwMax = 0L;	

    switch (LOWORD(wIdx)) {
    case P02_DEPT:
        dwMax = FLEX_DEPT_MAX;
        break;

    case P02_PLU:
        dwMax = FLEX_PLU_MAX;
        break;

/*
    case P02_SER:
        wMax = FLEX_WT_MAX;
        break;
*/

    case P02_CAS:
        dwMax = FLEX_CAS_MAX;
        break;

    case P02_EJ:
        dwMax = FLEX_EJ_MAX;
        break;

    case P02_ETK:
        dwMax = FLEX_ETK_MAX;
        break;

    case P02_GC:
        if ( lpBuff->bPtd != FLEX_STORE_RECALL ) {
            dwMax = FLEX_GC_MAX;
        } else {
            dwMax = P02_STOREC_GCMAX;
        }
        break;

    case P02_CPN:
        dwMax = FLEX_CPN_MAX;
        break;

    case P02_CSTR:
        dwMax = FLEX_CSTRING_MAX;
        break;

    case P02_ITEM:
    case P02_CONS:
        dwMax = FLEX_ITEMSTORAGE_MAX;
        wMin = FLEX_ITEMSTORAGE_MIN;
        break;

    case P02_RPT:
        dwMax = FLEX_PROGRPT_MAX;
        break;

    case P02_PPI:
        dwMax = FLEX_PPI_MAX;
        break;
    }

    if (lpBuff->dwData > dwMax || lpBuff->dwData < (DWORD)wMin) {
		WCHAR  szWork[PEP_OVER_LEN] = {0};
		WCHAR  szErr[PEP_OVER_LEN] = {0}, szCap[PEP_CAPTION_LEN] = {0};

        /* ----- Get error message format from resource ----- */
        LoadString(hResourceDll, IDS_P02_OVERRANGE, szErr, PEP_STRING_LEN_MAC(szErr));            /* ###ADD Saratoga */
        wsPepf(szWork, szErr, wMin, dwMax);

		LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szWork, szCap, MB_ICONEXCLAMATION | MB_OK);
        return TRUE;
    }
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02FinDlg()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               LPP02PARA   lpPara - address of data buffer
*               HGLOBAL     hMem   - handle of global memory lock for description
*               WORD        wID    - parameter of dialogbox ID
**
*   Return  :   TRUE               - user process is executed.
*               FALSE              - cancel process is expected.
*
**  Description:
*       This procedure finalize dialogbox.
* ===========================================================================
*/
BOOL    P02FinDlg(HWND hDlg, LPP02PARA lpPara, HGLOBAL hMem, WPARAM wID)
{
    USHORT      usReturnLen;
    P02CHECK    Check;
    HCURSOR     hCursor;

    if (LOWORD(wID) == IDOK) {       /* OK button pushed */
        /* ----- Change cursor (arrow -> hour-glass) ----- */
        hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
        ShowCursor(TRUE);

        /* ----- Initialize checking flag ----- */
        Check.nModFileID = P02_INITFLAG;
        Check.fwFile     = P02_INITFLAG;
		
		if(P02ChkAllDataRng(hDlg) == TRUE) {
            return FALSE;
        }

        /* ----- Get data from dialogbox ----- */
        if (P02GetData(hDlg, lpPara) == TRUE) {
            SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
            SetFocus(GetDlgItem(hDlg, wID));
            return FALSE;
        }

        /* ----- Check whether size changed or not and create file ----- */
        if (P02ChkFileChg(hDlg, lpPara, &Check) == TRUE) {
            /* ----- Restore cursor (hour-glass -> arrow) ----- */
            ShowCursor(FALSE);
            SetCursor(hCursor);
            return FALSE;
        }

        /* ----- Write data to file ----- */
        ParaAllWrite(CLASS_PARAFLEXMEM, (UCHAR *)lpPara->aCurt, sizeof(lpPara->aCurt), 0, &usReturnLen);

        /* ----- Set Transaction Flag ----- */
        SendMessage(hwndProgMain, PM_SET_TRAFLAG, (WPARAM)Check.fwFile, 0L);

        /* ----- Set file status flag ----- */
        PepSetModFlag(hwndProgMain, Check.nModFileID, 0);

        /* ----- Restore cursor (hour-glass -> arrow) ----- */
        ShowCursor(FALSE);
        SetCursor(hCursor);
    }

    /* ----- Free memory area ----- */
    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02GetData()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               LPP02PARA   lpPara - address of data buffer
**
*   Return  :   BOOL    TRUE    -   Inputed Data is Invalid.
*                       FALSE   -   Inputed Data is Valid.
*
**  Description:
*       This procedure get data from dialogbox.
* ===========================================================================
*/
BOOL    P02GetData(HWND hDlg, LPP02PARA lpPara)
{
    WORD    wMin, wMax;
    P02BUFF P02Buff;
    WCHAR   szWork[PEP_OVER_LEN],
            szErr[PEP_OVER_LEN],
            szCap[PEP_CAPTION_LEN],
			szWorkRecordNo[7];
    int     nLen, i;
    ULONG   ulRecordNo = 0;

	struct P02Control {
		int index;		//the index of the data in the LPP02PARA table
		int control;	//the origin of the data being checked (in the dialog)
	} P02EditBoxes[] = {
		{P02_DEPT,	IDD_P02EDIT_DEPT},
		{P02_PLU,	IDD_P02EDIT_PLU},
		{P02_CAS,	IDD_P02EDIT_CAS},
		{P02_EJ,	IDD_P02EDIT_EJ},
		{P02_ETK,	IDD_P02EDIT_ETK},
		{P02_ITEM,	IDD_P02EDIT_ITEM},
		{P02_CONS,	IDD_P02EDIT_CONS},
		{P02_GC,	IDD_P02EDIT_GC},
		{P02_CPN,	IDD_P02EDIT_CPN},
		{P02_CSTR,	IDD_P02EDIT_CSTR},
		{P02_RPT,	IDD_P02EDIT_RPT},
		{P02_PPI,	IDD_P02EDIT_PPI}
	};

	struct PTD_Checkbox {
		int bufIndex;
		int checkbox;
	} PTDCheckbox[] = {
		{P02_DEPT,	IDD_P02PTD_DEPT},
		{P02_PLU,	IDD_P02PTD_PLU},
	//	{P02_SER,	IDD_P02PTD_SER},	//server not used anymore
		{P02_CAS,	IDD_P02PTD_CAS},
		{P02_EJ,	IDD_P02PTD_EJ},
		{P02_CPN,	IDD_P02PTD_CPN},
	};
    
#if 1
		/* ----- Get data from editbox ----- */
	for (i = 0; i < sizeof(P02EditBoxes) / sizeof(P02EditBoxes[0]); i++){
        memset((LPSTR)szWorkRecordNo, 0x00, sizeof(szWorkRecordNo));
        nLen = DlgItemGetText(hDlg, P02EditBoxes[i].control, szWorkRecordNo, sizeof(szWorkRecordNo));
        ulRecordNo = _wtol(szWorkRecordNo);
        lpPara->aCurt[P02EditBoxes[i].index].ulRecordNumber = ulRecordNo;

        /* ----- Check Inputed Data Whether Minus or Plus ----- */
        if (P02EditBoxes[i].control == IDD_P02EDIT_ITEM || P02EditBoxes[i].control == IDD_P02EDIT_CONS) {
            if (lpPara->aCurt[P02EditBoxes[i].index].ulRecordNumber < FLEX_ITEMSTORAGE_MIN || 
                lpPara->aCurt[P02EditBoxes[i].index].ulRecordNumber > FLEX_ITEMSTORAGE_MAX) {

                /* ----- Display Error Message ----- */
                P02Buff.dwData = P02_MAXBLOCK;   /* Set Error Data to Buffer */
				if (P02ChkRng(hDlg, &P02Buff, P02EditBoxes[i].index)) {
                    SendDlgItemMessage(hDlg, P02EditBoxes[i].control, EM_UNDO, 0, 0);
                    SetFocus(GetDlgItem(hDlg, P02EditBoxes[i].control));
                    return TRUE;
                }

				SendDlgItemMessage(hDlg, P02EditBoxes[i].control, EM_SETSEL, 1, MAKELONG(0, -1));
                SetFocus(GetDlgItem(hDlg, P02EditBoxes[i].control));
                return TRUE;
			} else if (P02EditBoxes[i].control == IDD_P02EDIT_CONS &&
						lpPara->aCurt[P02_ITEM].ulRecordNumber > lpPara->aCurt[P02_CONS].ulRecordNumber) {

                /* ----- Get description from resource ----- */
                LoadString(hResourceDll, IDS_PEP_OVERRANGE, szErr, PEP_STRING_LEN_MAC(szErr));
                LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
                                   
                /* ----- Make description for message box ----- */
                wMin = (WORD)(lpPara->aCurt[P02_ITEM].ulRecordNumber);
                wMax = FLEX_ITEMSTORAGE_MAX;
                wsPepf(szWork, szErr, wMin, wMax);

                /* ----- Display caution message ----- */
                MessageBeep(MB_ICONEXCLAMATION);
				MessageBoxPopUp(hDlg, szWork, szCap, MB_ICONEXCLAMATION | MB_OK);

                /* ----- Set focus ----- */        
                SetFocus(GetDlgItem(hDlg, IDD_P02EDIT_CONS));
                return TRUE;                
            }
        } else if (nLen == 0) {
            /* ----- Display Error Message ----- */
            P02Buff.dwData = P02_MAXBLOCK;   /* Set Error Data to Buffer */
			if (P02ChkRng(hDlg, (LPP02BUFF)&P02Buff, P02EditBoxes[i].index) == TRUE) {                
				SendDlgItemMessage(hDlg, P02EditBoxes[i].control, EM_UNDO, 0, 0);
                SetFocus(GetDlgItem(hDlg, P02EditBoxes[i].control));
                return TRUE;
            }

            SendDlgItemMessage(hDlg, P02EditBoxes[i].control, EM_SETSEL, 1, MAKELONG(0, -1));
            SetFocus(GetDlgItem(hDlg, P02EditBoxes[i].control));
            return TRUE;
        }
    }
#else
	/* ----- Get data from editbox ----- */
    /* ----- except MLD file (MAX_FLXMEM_SIZE -1), V3.3 ----- */
    for (wI = 0, wID = IDD_P02EDIT; wI < MAX_FLXMEM_SIZE-1; wI++, wID++) {
        memset((LPSTR)szWorkRecordNo, 0x00, sizeof(szWorkRecordNo));
        ulRecordNo = 0L;
        nLen = DlgItemGetText(hDlg, wID, szWorkRecordNo, sizeof(szWorkRecordNo));

        ulRecordNo = _wtol(szWorkRecordNo);
        lpPara->aCurt[wI].ulRecordNumber = ulRecordNo;

        /* ----- Ignore Server No. Data (editbox nothing) ,V3.3----- */
        if (wID == IDD_P02EDIT_SER) {
            nLen = TRUE;  /* dummy data */
        }

        /* ----- Check Inputed Data Whether Minus or Plus ----- */
        if (wID == IDD_P02EDIT_ITEM || wID == IDD_P02EDIT_CONS) {
            if (lpPara->aCurt[wI].ulRecordNumber < FLEX_ITEMSTORAGE_MIN || 
                lpPara->aCurt[wI].ulRecordNumber > FLEX_ITEMSTORAGE_MAX) {

                /* ----- Display Error Message ----- */
                P02Buff.dwData = P02_MAXBLOCK;   /* Set Error Data to Buffer */
                if (P02ChkRng(hDlg, (LPP02BUFF)&P02Buff, wI) == TRUE) {
                    SendMessage(GetDlgItem(hDlg, IDD_P02EDIT + wID), EM_UNDO, 0, 0L);
                    /* ----- Set focus ----- */
                    SetFocus(GetDlgItem(hDlg, IDD_P02EDIT + wID));
                    return TRUE;
                }

                SendDlgItemMessage(hDlg, wID, EM_SETSEL, 1, MAKELONG(0, -1));
                SetFocus(GetDlgItem(hDlg, wID));
                return TRUE;
            } else if (wID == IDD_P02EDIT_CONS) {
                if (lpPara->aCurt[wI - 1].ulRecordNumber >  /* IDD_P02EDIT_ITEM */
                    lpPara->aCurt[wI].ulRecordNumber) {     /* IDD_P02EDIT_CONS */

                    /* ----- Get description from resource ----- */
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szErr, PEP_STRING_LEN_MAC(szErr));
                    LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
                                       
                    /* ----- Make description for message box ----- */
                    wMin = (WORD)(lpPara->aCurt[wI - 1].ulRecordNumber);

                    wMax = FLEX_ITEMSTORAGE_MAX;
                    wsPepf(szWork, szErr, wMin, wMax);

                    /* ----- Display caution message ----- */
                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hDlg, szWork, szCap, MB_ICONEXCLAMATION | MB_OK);

                    /* ----- Set focus ----- */
                    SetFocus(GetDlgItem(hDlg, IDD_P02EDIT + wID));
                    return TRUE;
                }
            }
        } else if (nLen == 0) {
            /* ----- Display Error Message ----- */
            P02Buff.dwData = P02_MAXBLOCK;   /* Set Error Data to Buffer */
            if (P02ChkRng(hDlg, (LPP02BUFF)&P02Buff, wI) == TRUE) {
                SendMessage(GetDlgItem(hDlg, IDD_P02EDIT + wID), EM_UNDO, 0, 0L);
                /* ----- Set focus ----- */
                SetFocus(GetDlgItem(hDlg, IDD_P02EDIT + wID));
                return TRUE;
            }

            SendDlgItemMessage(hDlg, wID, EM_SETSEL, 1, MAKELONG(0, -1));
            SetFocus(GetDlgItem(hDlg, wID));
            return TRUE;
        }
    }
#endif

#if 1
	/* ----- Get PTD data from check-box ----- */
	for (i = 0; i < sizeof(PTDCheckbox) / sizeof(PTDCheckbox[0]); i++)
		lpPara->aCurt[PTDCheckbox[i].bufIndex].uchPTDFlag = (UCHAR)SendDlgItemMessage(hDlg, PTDCheckbox[i].checkbox, BM_GETCHECK, 0, 0);
#else
	/* ----- Get PTD data from check-box ----- */
    for (wI = 0, wID = IDD_P02PTD; wI < P02_PTD_NO; wI++) {
        lpPara->aCurt[wI].uchPTDFlag = (UCHAR)SendDlgItemMessage(hDlg, wID++, BM_GETCHECK, 0, 0);
    }

    /* ----- Get Coupon PTD data from check-box ----- */
    lpPara->aCurt[P02_CPN].uchPTDFlag = (UCHAR)SendDlgItemMessage(hDlg, IDD_P02PTD_CPN, BM_GETCHECK, 0, 0);
    
	/* ----- Get E/J overwirte data from check-box ----- */
    lpPara->aCurt[P02_EJ].uchPTDFlag = (UCHAR)SendDlgItemMessage(hDlg, IDD_P02PTD_EJ, BM_GETCHECK, 0, 0);
#endif

	/* ----- Get guest check type from combo-box ----- */
    lpPara->aCurt[P02_GC].uchPTDFlag = (UCHAR)SendDlgItemMessage(hDlg, IDD_P02PTD_GC, CB_GETCURSEL, 0, 0);

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ChkFileChg()
*
*   Input   :   HWND        hDlg   - handle of a dialog procedure
*               LPP02PARA   lpPara - address of data buffer
*               LPP02CHECK  lpChk  - address of check flag structure
**
*   Output  :   UINT    lpChk->nModFileID - file modify flag
*               WORD    lpChk->fwFile     - trans modify flag
*
*   Return  :   TRUE               - cacel process is expected
*               FALSE              - user process is executed
*
**  Description:
*       This procedure check file size change
*       as comparing between previous data and maintenanced data,
*       and create department, PLU, server, cashier files.
* ===========================================================================
*/
BOOL    P02ChkFileChg(HWND hDlg, LPP02PARA lpPara, LPP02CHECK lpChk)
{
    BOOL    fRet, fChk = P02_INITFLAG;
    SHORT   sStat;
    USHORT  usStat;
    MFINFO  Info;

    WCHAR    szPluRecord[P02_PLU_LEN + 1];

    if ((lpPara->aCurt[P02_DEPT].ulRecordNumber != lpPara->aPrev[P02_DEPT].ulRecordNumber)
        || (lpPara->aCurt[P02_DEPT].uchPTDFlag != lpPara->aPrev[P02_DEPT].uchPTDFlag)) {
                                                /* department data change */
        if (lpPara->aCurt[P02_DEPT].ulRecordNumber == 0L) {   /* data is 0 */
            /* ----- Execute error process ----- */
            if (P02ExecErrProc(hDlg, lpPara, P02_DEPT, lpChk) == TRUE) {
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }

            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (lpPara->aCurt[P02_DEPT].ulRecordNumber < lpPara->aPrev[P02_DEPT].ulRecordNumber) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_DEPT, lpChk) == TRUE) {
                /* Execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }

            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            if ((sStat = OpDeptCreatFile((USHORT)(lpPara->aCurt[P02_DEPT].ulRecordNumber),0)) != OP_PERFORM) {
                /* create department file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }

            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_DEPT;
            lpChk->fwFile     |= TRANS_ACCESS_DEPT;
        }
    }

    /* --- Saratoga --- */
    if (lpPara->aCurt[P02_PLU].ulRecordNumber != lpPara->aPrev[P02_PLU].ulRecordNumber) { /* PLU data change */
        if (lpPara->aPrev[P02_PLU].ulRecordNumber != 0L) {
            /* ----- Get PLU regular file information ----- */
            usStat = PluSenseFile(FILE_PLU, &Info);
        } else {
            /* ----- Initialize PLU regular file information ----- */
            Info.ulCurRec = 0;
            usStat = SPLU_FILE_NOT_FOUND;
        }

        if (lpPara->aCurt[P02_PLU].ulRecordNumber == 0L) {   /* data is 0 */
            if (P02ExecErrProc(hDlg, lpPara, P02_PLU, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (usStat == SPLU_FILE_NOT_FOUND) {         /* new create */
            /* ----- Create plu regular file ----- */
            if ((usStat = PluCreateFile(FILE_PLU, lpPara->aCurt[P02_PLU].ulRecordNumber)) != SPLU_COMPLETED) {
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg,
                            (UINT)((usStat == SPLU_DEVICE_FULL) ? IDS_P02_ERR_DISKFULL : IDS_P02_ERR_UNKOWN),
                            (SHORT)usStat);

                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            } else {
                /* ----- Create Index file, R2.0 ----- */
                if ((usStat = PluCreateFile(FILE_PLU_INDEX, lpPara->aCurt[P02_PLU].ulRecordNumber)) != SPLU_COMPLETED) {
                    /* ----- Display caution message ----- */
                    P02DispCaution(hDlg,
                                (UINT)((usStat == SPLU_DEVICE_FULL) ? IDS_P02_ERR_DISKFULL : IDS_P02_ERR_UNKOWN),
                                (SHORT)usStat);

                    /* ----- Set flag to file change ----- */
                    fChk |= P02_CHG_FILE;
                } else {
                  /* ----- Set checking flag ----- */
                  lpChk->nModFileID |= PEP_MF_PLU;
                  lpChk->fwFile     |= TRANS_ACCESS_PLU;
                }
            }
        } else if (lpPara->aCurt[P02_PLU].ulRecordNumber < Info.ulCurRec) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_PLU, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {                                            /* file extended */
            /* ----- Re-create PLU regular file ----- */
            if (RecreatePluFile(hDlg, lpPara->aCurt[P02_PLU].ulRecordNumber) == SPLU_COMPLETED) {
                /* ----- Re-create PLU Index file ----- */
                if (RecreatePluIndexFile(hDlg, lpPara->aCurt[P02_PLU].ulRecordNumber) == SPLU_COMPLETED) {
                   /* ----- Set checking flag ----- */
                   lpChk->nModFileID |= PEP_MF_PLU;
                   lpChk->fwFile     |= TRANS_ACCESS_PLU;
                   /* ----- Set flag to user hope change ----- */
                   fChk |= P02_CHG_USER;
                } else {
                   /* ----- Restore previous PLU No. ----- */
                   lpPara->aCurt[P02_PLU].ulRecordNumber = lpPara->aPrev[P02_PLU].ulRecordNumber;
                   /* ----- Convert a long integer to a string, Saratoga ----- */
                   memset((LPSTR)szPluRecord, 0x00, sizeof(szPluRecord));
                   _ltow(lpPara->aCurt[P02_PLU].ulRecordNumber, szPluRecord, 10);
                   DlgItemRedrawText(hDlg, IDD_P02EDIT_PLU, szPluRecord);
                   /* ----- Set flag to file change ----- */
                   fChk |= P02_CHG_FILE;
                }
            } else {
                /* ----- Restore previous PLU No. ----- */
                lpPara->aCurt[P02_PLU].ulRecordNumber = lpPara->aPrev[P02_PLU].ulRecordNumber;
               /* ----- Convert a long integer to a string, Saratoga ----- */
                memset((LPSTR)szPluRecord, 0x00, sizeof(szPluRecord));
                _ltow(lpPara->aCurt[P02_PLU].ulRecordNumber, szPluRecord, 10);
                DlgItemRedrawText(hDlg, IDD_P02EDIT_PLU, szPluRecord);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }
        }
    }
    /* --- Saratoga --- */
#if defined(POSSIBLE_DEAD_CODE)
    if ((lpPara->aCurt[P02_SER].ulRecordNumber != lpPara->aPrev[P02_SER].ulRecordNumber)
        || (lpPara->aCurt[P02_SER].uchPTDFlag != lpPara->aPrev[P02_SER].uchPTDFlag)) {
        /* server data change */
        if (lpPara->aCurt[P02_SER].ulRecordNumber == 0L) {    /* data is 0 */
            if (P02ExecErrProc(hDlg, lpPara, P02_SER, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if ((sStat = WaiCreatFile((UCHAR)(lpPara->aCurt[P02_SER].ulRecordNumber))) ==  WAI_NO_COMPRESS_FILE) {
            /* create server file */
            if (P02ExecErrProc(hDlg, lpPara, P02_SER, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (sStat != WAI_PERFORM) {
            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_SERV;
            lpChk->fwFile     |= TRANS_ACCESS_WAITER;
        }
    }
#endif

    if ((lpPara->aCurt[P02_CAS].ulRecordNumber != lpPara->aPrev[P02_CAS].ulRecordNumber)
         || (lpPara->aCurt[P02_CAS].uchPTDFlag != lpPara->aPrev[P02_CAS].uchPTDFlag)) {
        /* operator data change */
        if (lpPara->aCurt[P02_CAS].ulRecordNumber == 0L) {  /* data is 0 */
            if (P02ExecErrProc(hDlg, lpPara, P02_CAS, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if ((sStat = CasCreatFile((USHORT)(lpPara->aCurt[P02_CAS].ulRecordNumber))) == CAS_NO_COMPRESS_FILE) {  /* V3.3 */
            /* create cashier file */
            if (P02ExecErrProc(hDlg, lpPara, P02_CAS, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (sStat != CAS_PERFORM) {
            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_CASH;
            lpChk->fwFile     |= TRANS_ACCESS_CASHIER;
        }
    }

    if (lpPara->aCurt[P02_ETK].ulRecordNumber != lpPara->aPrev[P02_ETK].ulRecordNumber) {
        /* ETK data change */
        if (lpPara->aCurt[P02_ETK].ulRecordNumber == 0L) {  /* data is 0 */
            if (P02ExecErrProc(hDlg, lpPara, P02_ETK, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (lpPara->aCurt[P02_ETK].ulRecordNumber < lpPara->aPrev[P02_ETK].ulRecordNumber) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_ETK, lpChk) == TRUE) {
                /* execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            if ((sStat = EtkCreatFile((USHORT)(lpPara->aCurt[P02_ETK].ulRecordNumber))) !=  ETK_SUCCESS) {
                /* create ETK file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_ETK;
            lpChk->fwFile     |= TRANS_ACCESS_ETK;
        }
    }

    if ((lpPara->aCurt[P02_CPN].ulRecordNumber != lpPara->aPrev[P02_CPN].ulRecordNumber)
        || (lpPara->aCurt[P02_CPN].uchPTDFlag != lpPara->aPrev[P02_CPN].uchPTDFlag)) {
        /* Combination Coupon data change */
        if (lpPara->aCurt[P02_CPN].ulRecordNumber == 0L) {   /* data is 0 */
            /* ----- Execute error process ----- */
            if (P02ExecErrProc(hDlg, lpPara, P02_CPN, lpChk) == TRUE) {
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (lpPara->aCurt[P02_CPN].ulRecordNumber < lpPara->aPrev[P02_CPN].ulRecordNumber) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_CPN, lpChk) == TRUE) {
               /* Execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            if ((sStat = OpCpnCreatFile((USHORT)lpPara->aCurt[P02_CPN].ulRecordNumber,0)) != OP_PERFORM) {
                /* create department file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_CPN;
            lpChk->fwFile     |= TRANS_ACCESS_CPN;
        }
    }

    if (lpPara->aCurt[P02_CSTR].ulRecordNumber != lpPara->aPrev[P02_CSTR].ulRecordNumber) {
        /* Control String data change */
        if (lpPara->aCurt[P02_CSTR].ulRecordNumber == 0L) {   /* data is 0 */
            /* ----- Execute error process ----- */
            if (P02ExecErrProc(hDlg, lpPara, P02_CSTR, lpChk) == TRUE) {
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (OP_NO_COMPRESS_FILE == OpCstrCreatFile((USHORT)(lpPara->aCurt[P02_CSTR].ulRecordNumber), 0)) {
            if (P02ExecErrProc(hDlg, lpPara, P02_CSTR, lpChk) == TRUE) {
                /* Execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_CSTR;
            lpChk->fwFile     |= TRANS_ACCESS_CSTR;
        }
    }

    if (lpPara->aCurt[P02_RPT].ulRecordNumber != lpPara->aPrev[P02_RPT].ulRecordNumber) {
        /* Programmable Report data change */
        if (lpPara->aCurt[P02_RPT].ulRecordNumber == 0L) {   /* data is 0 */
            /* ----- Execute error process ----- */
            if (P02ExecErrProc(hDlg, lpPara, P02_RPT, lpChk) == TRUE) {
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (lpPara->aCurt[P02_RPT].ulRecordNumber < lpPara->aPrev[P02_RPT].ulRecordNumber) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_RPT, lpChk) == TRUE) {
                /* Execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            if ((sStat = ProgRptCreate((UCHAR)(lpPara->aCurt[P02_RPT].ulRecordNumber))) != PROGRPT_SUCCESS) {
                /* create Programmable Report file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_RPT;
            lpChk->fwFile     |= TRANS_ACCESS_RPT;
        }
    }

    if (lpPara->aCurt[P02_PPI].ulRecordNumber != lpPara->aPrev[P02_PPI].ulRecordNumber) {
        /* PPI data change */
        if (lpPara->aCurt[P02_PPI].ulRecordNumber == 0L) {   /* data is 0 */
            /* ----- Execute error process ----- */
            if (P02ExecErrProc(hDlg, lpPara, P02_PPI, lpChk) == TRUE) {
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else if (lpPara->aCurt[P02_PPI].ulRecordNumber < lpPara->aPrev[P02_PPI].ulRecordNumber) {
            /* file compress */
            if (P02ExecErrProc(hDlg, lpPara, P02_PPI, lpChk) == TRUE) {
                /* Execute error process */
                /* ----- Set flag to user hope change ----- */
                fChk |= P02_CHG_USER;
            }
            /* ----- Set flag to file change ----- */
            fChk |= P02_CHG_FILE;
        } else {
            if ((sStat = OpPpiCreatFile((USHORT)(lpPara->aCurt[P02_PPI].ulRecordNumber),0)) != OP_PERFORM) {
                /* create PPI file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set flag to file change ----- */
                fChk |= P02_CHG_FILE;
            }
            /* ----- Set checking flag ----- */
            lpChk->nModFileID |= PEP_MF_PPI;
            lpChk->fwFile     |= TRANS_ACCESS_PPI;
        }
    }

    if ((fChk & P02_CHG_FILE) && (! (fChk & P02_CHG_USER))) {
        /* user does not want to change file size */
        /* ----- Set return value ----- */
        fRet = TRUE;
    } else {
        /* ----- Set file flag for paramter change ----- */
        lpChk->nModFileID |= PEP_MF_PROG;
        /* ----- Set return value ----- */
        fRet = FALSE;
    }

    /* ----- delete programmable report file ----- */
    ProgRptCreate(0);
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ExecErrProc()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               LPP02PARA   lpPara - address of data buffer
*               WORD        wIdx   - which file creating
*               LPP02CHECK  lpChk  - address of check flag structure
**
*   Output  :   UINT        lpChk->nModFileID  - file modify flag
*               WORD        lpChk->fwFile      - trans modify flag
*
*   Return  :   TRUE               - user process is executed
*               FALSE              - cacel process is expected
*
**  Description:
*       This procedure execute error process.
* ===========================================================================
*/
BOOL    P02ExecErrProc(HWND hDlg, LPP02PARA lpPara,
                                WORD wIdx, LPP02CHECK lpChk)
{
    WORD    wID;
    WCHAR    szWork[P02_ERROR_LEN], szCap[PEP_CAPTION_LEN];
    int     nChk;
    BOOL    fRet = TRUE;
    SHORT   sStat;
    USHORT  usStat;
	//converts string to WIDE
    WCHAR    szOEP[] = WIDE("PARAMOEP");

    if (lpPara->aCurt[wIdx].ulRecordNumber == 0L) {
        /* ----- Make error description ----- */
        wsPepf(szWork, WIDE("%s%s"), lpStr->aszDscrb[wIdx], lpStr->aszDscrb[P02_FILE_EXT]);
    } else {
        /* ----- Make error description ----- */
        wsPepf(szWork, WIDE("%s%s"), lpStr->aszDscrb[wIdx], lpStr->aszDscrb[P02_FILE_CLR]);
    }
    /* ----- Get description from resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
    /* ----- Display caution message ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    nChk = MessageBoxPopUp(hDlg, szWork, szCap, MB_OKCANCEL | MB_ICONEXCLAMATION);
    if (nChk == IDOK) {             /* OK button selected */
        switch (wIdx) {
        case P02_DEPT:
            if (lpPara->aCurt[P02_DEPT].ulRecordNumber) {
                /* ----- Delete dept file 1.0.9 ----- */
                OpDeptCreatFile(0, 0);
            }

            if ((sStat = OpDeptCreatFile((USHORT)(lpPara->aCurt[P02_DEPT].ulRecordNumber), 0)) != OP_PERFORM) {
                /* re-create department file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_DEPT;
                lpChk->fwFile     |= TRANS_ACCESS_DEPT;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_PLU:
            if (lpPara->aCurt[P02_PLU].ulRecordNumber) {
                /* ----- Delete PLU file,   Saratoga ----- */
                PluDeleteFile(FILE_PLU);
                PluDeleteFile(FILE_PLU_INDEX);
            }
            /* ----- Delete PLU file,   Saratoga ----- */
            /* When PLU recordnumber is 0, PLU file is deleted with PLU Index file. */
            /* ---------   PluCreateFile() Function delete PLU and PLU Index file,  */
            /*             When PLU recordnumber is 0                               */
            if ((usStat = PluCreateFile(FILE_PLU, lpPara->aCurt[P02_PLU].ulRecordNumber)) != SPLU_COMPLETED) {
                /* re-create PLU regular file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg,
                            (UINT)((usStat == SPLU_DEVICE_FULL) ? IDS_P02_ERR_DISKFULL : IDS_P02_ERR_UNKOWN),
                            (SHORT)usStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Create Index file, 1.0.9 ----- */
                if (lpPara->aCurt[P02_PLU].ulRecordNumber) {
                    if ((usStat = PluCreateFile(FILE_PLU_INDEX, lpPara->aCurt[P02_PLU].ulRecordNumber)) != SPLU_COMPLETED) {
                        /* ----- Display caution message ----- */
                        P02DispCaution(hDlg,
                                    (UINT)((usStat == SPLU_DEVICE_FULL) ? IDS_P02_ERR_DISKFULL : IDS_P02_ERR_UNKOWN),
                                    (SHORT)usStat);
                        /* ----- Set end dialog status ----- */
                        fRet = FALSE;
                     }
                 }
                 /* ----- Delete OEP file, Saratoga ----- */
                 PifDeleteFile(szOEP);
                 /* ----- Set checking flag ----- */
                 lpChk->nModFileID |= PEP_MF_PLU;
                 lpChk->fwFile     |= TRANS_ACCESS_PLU;
                 /* ----- Set end dialog status ----- */
                 fRet = TRUE;
            }
            break;

#if defined(POSSIBLE_DEAD_CODE)
        case P02_SER:
            if (lpPara->aCurt[P02_SER].ulRecordNumber) {
                /* ----- Delete server file ----- */
                WaiCreatFile(0);
            }

            if ((sStat = WaiCreatFile((UCHAR)(lpPara->aCurt[P02_SER].ulRecordNumber))) != WAI_PERFORM) {
                /* re-create server file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_SERV;
                lpChk->fwFile     |= TRANS_ACCESS_WAITER;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;
#endif

        case P02_CAS:
            if (lpPara->aCurt[P02_CAS].ulRecordNumber) {
                /* ----- Delete cashier file ----- */
                CasCreatFile(0);
            }
            if ((sStat = CasCreatFile((USHORT)(lpPara->aCurt[P02_CAS].ulRecordNumber))) != CAS_PERFORM) { /* V3.3 */
                /* re-create cashier file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_CASH;
                lpChk->fwFile     |= TRANS_ACCESS_CASHIER;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_ETK:
            if (lpPara->aCurt[P02_ETK].ulRecordNumber) {
                /* ----- Delete etk file V1.0.9----- */
                EtkCreatFile(0);
            }

            if ((sStat = EtkCreatFile((USHORT)(lpPara->aCurt[P02_ETK].ulRecordNumber))) != ETK_SUCCESS) {
                /* re-create ETK file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_ETK;
                lpChk->fwFile     |= TRANS_ACCESS_ETK;

                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_CPN:
            if ((sStat = OpCpnCreatFile((USHORT)lpPara->aCurt[P02_CPN].ulRecordNumber, 0)) != OP_PERFORM) {
                /* re-create Combination Coupon regular file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_CPN;
                lpChk->fwFile     |= TRANS_ACCESS_CPN;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_CSTR:
            if (lpPara->aCurt[P02_CSTR].ulRecordNumber) {
                /* ----- Delete Control String file ----- */
                OpCstrCreatFile(0, 0);
            }

            if ((sStat = OpCstrCreatFile((USHORT)(lpPara->aCurt[P02_CSTR].ulRecordNumber), 0)) != OP_PERFORM) {
                /* re-create Control String file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_CSTR;
                lpChk->fwFile     |= TRANS_ACCESS_CSTR;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_RPT:
            if ((sStat = ProgRptCreate((UCHAR)(lpPara->aCurt[P02_RPT].ulRecordNumber))) != PROGRPT_SUCCESS) {
                /* re-create programmable report file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_RPT;
                lpChk->fwFile     |= TRANS_ACCESS_RPT;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;

        case P02_PPI:
            if ((sStat = OpPpiCreatFile((USHORT)(lpPara->aCurt[P02_PPI].ulRecordNumber), 0)) != OP_PERFORM) {
                /* re-create PPI file */
                /* ----- Display caution message ----- */
                P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, sStat);
                /* ----- Set end dialog status ----- */
                fRet = FALSE;
            } else {
                /* ----- Set checking flag ----- */
                lpChk->nModFileID |= PEP_MF_PPI;
                lpChk->fwFile     |= TRANS_ACCESS_PPI;
                /* ----- Set end dialog status ----- */
                fRet = TRUE;
            }
            break;
        }
    } else {                /* Cancel button selected */
        /* ----- Set end dialog status ----- */
        fRet = FALSE;
        switch (wIdx) {
        case P02_DEPT:
            /* ----- Restore previous department No. ----- */
            lpPara->aCurt[P02_DEPT].ulRecordNumber = lpPara->aPrev[P02_DEPT].ulRecordNumber;
            break;

        case P02_PLU:
            /* ----- Restore previous PLU No. ----- */
            lpPara->aCurt[P02_PLU].ulRecordNumber = lpPara->aPrev[P02_PLU].ulRecordNumber;
            break;

#if defined(POSSIBLE_DEAD_CODE)
        case P02_SER:
            /* ----- Restore previous server No. ----- */
            lpPara->aCurt[P02_SER].ulRecordNumber = lpPara->aPrev[P02_SER].ulRecordNumber;
            break;
#endif

        case P02_CAS:
            /* ----- Restore previous cashier No. ----- */
            lpPara->aCurt[P02_CAS].ulRecordNumber = lpPara->aPrev[P02_CAS].ulRecordNumber;
            break;

        case P02_ETK:
            /* ----- Restore previous ETK No. ----- */
            lpPara->aCurt[P02_ETK].ulRecordNumber = lpPara->aPrev[P02_ETK].ulRecordNumber;
            break;

        case P02_CPN:
            /* ----- Restore previous Combination Coupon No. ----- */
            lpPara->aCurt[P02_CPN].ulRecordNumber = lpPara->aPrev[P02_CPN].ulRecordNumber;
            break;

        case P02_CSTR:
            /* ----- Restore previous Control String No. ----- */
            lpPara->aCurt[P02_CSTR].ulRecordNumber = lpPara->aPrev[P02_CSTR].ulRecordNumber;
            break;

        case P02_RPT:
            /* ----- Restore previous programmable report No. ----- */
            lpPara->aCurt[P02_RPT].ulRecordNumber = lpPara->aPrev[P02_RPT].ulRecordNumber;
            break;

        case P02_PPI:
            /* ----- Restore previous PPI No. ----- */
            lpPara->aCurt[P02_PPI].ulRecordNumber = lpPara->aPrev[P02_PPI].ulRecordNumber;
            break;
        }

        /* ----- Set data to the edit-box ----- */
        for (wIdx = 0, wID = IDD_P02EDIT; wIdx < MAX_FLXMEM_SIZE; wIdx++) {
            SetDlgItemInt(hDlg, wID++, (WORD)(lpPara->aCurt[wIdx].ulRecordNumber), FALSE);
        }
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   WORD    P02DrawMemUse()
*
*   Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               WORD        wFree     - free block
*               LPWORD      lpwMem    - address of memory useage
**
*   Return  :   WORD        wMemTotal - total memory usage
*
**  Description:
*       This procedure draw graphical memory usage.
* ===========================================================================
*/
DWORD  P02DrawMemUse(HWND hDlg, DWORD dwFree, LPDWORD lpdwMem)
{
    HDC         hDC;
    PAINTSTRUCT Paint;

    WORD        wI, wMode;
    DWORD       dwMemTotal = 0L;
    HPEN        hPenFrame;
    HBRUSH      hBrushBack, hBrushText;
    LOGPEN      LogPenFrame;        /* frame pen style */
    LOGBRUSH    LogBruText,         /* text brush style */
                LogBruBack;         /* back brush style */
    RECT        rBar = {
                    P02_AREALEFT + P02_AREAEDGE,
                    P02_AREABOTTOM,
                    P02_AREARIGHT - P02_AREAEDGE,
                    P02_AREABOTTOM,
                };
    RECT        rClient;

    /* -----  Get DC handle ----- */
    hDC = BeginPaint(hDlg, &Paint);

    SetMapMode(hDC, MM_ANISOTROPIC);
    GetClientRect(hDlg, (LPRECT)&rClient);
    SetViewportExtEx(hDC, rClient.right, rClient.bottom, NULL);
    SetViewportOrgEx(hDC, 0, 0, NULL);
    SetWindowExtEx(hDC, 2100, 1350, NULL);

    /* ----- Set Drawing Mode ----- */
    SetROP2(hDC, R2_COPYPEN);

    /* ----- Set pen and brush style ----- */
    LogPenFrame.lopnStyle   = PS_SOLID;
    LogPenFrame.lopnWidth.x = P02_PEN_WIDTH;
    LogPenFrame.lopnWidth.y = P02_PEN_WIDTH;
    LogPenFrame.lopnColor   = GetSysColor(COLOR_WINDOWFRAME);
    LogBruText.lbStyle = PS_SOLID;
    LogBruText.lbHatch = 0;
    LogBruText.lbColor = GetSysColor(COLOR_WINDOWTEXT);
    LogBruBack.lbStyle = PS_SOLID;
    LogBruBack.lbHatch = 0;
    LogBruBack.lbColor = GetSysColor(COLOR_WINDOW);

    /* ----- Create pen and brush ----- */
    hPenFrame  = CreatePenIndirect((LPLOGPEN)&LogPenFrame);
    hBrushText = CreateBrushIndirect((LPLOGBRUSH)&LogBruText);
    hBrushBack = CreateBrushIndirect((LPLOGBRUSH)&LogBruBack);

    /* ----- Set object to white pen ----- */
    SelectObject(hDC, hPenFrame);
    SelectObject(hDC, hBrushBack);

    /* ----- Draw frame ----- */
    Rectangle(hDC, rArea.left, rArea.top, rArea.right, rArea.bottom);

    /* ----- Calculate memory usage rate ----- */
    for (wI = 0; wI < MAX_FLXMEM_SIZE; wI++) {
        dwMemTotal += *(lpdwMem + (DWORD)wI);
    }

    rBar.top -= (int)(dwMemTotal * P02_AREAHIGHT / dwFree);

    /* ----- Check rate radio-button status ----- */
    wMode = (WORD)SendDlgItemMessage(hDlg, IDD_P02BYTE, BM_GETCHECK, 0, 0L);

    /* ----- Display memory usage % rate ----- */
    P02DispMemRate(hDlg, dwMemTotal, &dwFree, wMode);

    /* ----- Set object to black pen ----- */
    SelectObject(hDC, hBrushText);

    /* ----- Draw memory usage garph ----- */
    if (rBar.top < P02_AREATOP) {
        rBar.top = P02_AREATOP;
    }
    Rectangle(hDC, rBar.left, rBar.top, rBar.right, rBar.bottom);

    /* ----- Release DC handle ----- */
    EndPaint(hDlg, &Paint);

    /* ----- Release object handle ----- */
    DeleteObject(hPenFrame);
    DeleteObject(hBrushText);
    DeleteObject(hBrushBack);

    return (dwMemTotal);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02ChkMemUse()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               DWORD       dwTtl   - total memory usage
*               DWORD       dwFree  - free memory  
**
*   Return  :   TRUE               - user process is executed
*               FALSE              - default process is expected
*
**  Description:
*       This procedure check graph data.
* ===========================================================================
*/
BOOL    P02ChkMemUse(HWND hDlg, DWORD dwTtl, DWORD dwFree)
{
    if (dwTtl > dwFree) {   /* out of memory */
		WCHAR    szCap[PEP_CAPTION_LEN];

        /* ----- Set memory check flag ----- */
        fMemChk = TRUE;

        /* ----- Get description from resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, lpStr->aszDscrb[P02_OVERMEM], szCap, MB_ICONEXCLAMATION|MB_OK);
        return FALSE;
    }
    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P02DispMemRate()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               WORD    wMemUse - memory usage
*               LPWORD  lpwFree - address of free block
*               WORD    wMode   - % rate/byte display mode selection
**
*   Return  :   None
*
**  Description:
*       This procedure display memory usage % rate.
* ===========================================================================
*/
VOID    P02DispMemRate(HWND hDlg, DWORD dwMemUse,
                                LPDWORD lpdwFree, WORD wMode)
{
	WCHAR    szWork[P02_DISP_LEN] = {0};
    DWORD   dwInt,dwFree, dwDec;

    /* ----- Reset temporary buffer area ----- */
    if (wMode == P02_BYTE) {        /* byte display mode */
        /* ----- Calculate byte ----- */
        dwInt  = (dwMemUse / (DWORD)(P02_CALCBYTE));
        dwFree = (*lpdwFree / (DWORD)(P02_CALCBYTE));
        /* ----- Make description ----- */
        wsPepf(szWork, WIDE("%luK/%luK"), dwInt, dwFree);
    } else {    /* (wMode == P02_RATE) % rate display mode */
        /* ----- Calculate % rate ----- */
        dwInt = ((dwMemUse * (DWORD)P02_CALC) / *lpdwFree);
        dwDec = (((dwMemUse * (DWORD)P02_CALC) % *lpdwFree) * (DWORD)P02_DEC / *lpdwFree);
        /* ----- Make description ----- */
        wsPepf(szWork, WIDE("%3lu.%1lu%%"), dwInt, dwDec);
    }

    /* ----- Display rate ----- */
    DlgItemRedrawText(hDlg, IDD_P02MEMUSE, szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P02DispCaution()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               UINT    unID    - string ID
*               SHORT   sStat   - error code
**
*   Return  :   None
*
**  Description:
*       This procedure display caution message.
* ===========================================================================
*/
VOID    P02DispCaution(HWND hDlg, UINT unID, SHORT sStat)
{
    WCHAR    szCap[PEP_CAPTION_LEN],
            szErr[P02_ERROR_LEN],
            szMsg[P02_ERROR_LEN];
#if defined(POSSIBLE_DEAD_CODE)
// hInst never used
    HANDLE  hInst;

    hInst = (unID == IDS_PEP_ALLOC_ERR) ? hProgCommInst : hProgInst;
#endif

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P02, szCap, PEP_STRING_LEN_MAC(szCap));
    LoadString(hResourceDll, unID, szErr, PEP_STRING_LEN_MAC(szErr));

    /* ----- Make description ------ */
    if (sStat != 0) {
        wsPepf(szMsg, szErr, sStat);
    } else {
        memcpy(szMsg, szErr, sizeof(szErr));
    }

    /* ----- Display caution message ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P02SpinProc();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*               WPARAM  wParam  - 16 bits message parameter
*               LPARAM  lParam  - 32 bits message parameter
*               LPDWORD  lpwBuff - address of data buffer
*
**  Return  :   TRUE            - User Process is Executed
*               FALSE           - Default Process is Expected
*
**  Description:
*       This function is spin procedure.
* ===========================================================================
*/
BOOL    P02SpinProc(HWND hDlg, WPARAM wParam,
                             LPARAM lParam, LPDWORD lpdwBuff)
{
    PEPSPIN     PepSpin;
    BOOL        fRet = FALSE;
    int         nEdit = 0;
    WORD        wIndex/*, wIdW = 0*/;
    DWORD       dwIdx;

    /* ----- Initialize spin mode flag ----- */
    PepSpin.lMin       = 0L;
    PepSpin.nStep      = P02_SPIN_STEP;
    PepSpin.nTurbo     = P02_SPIN_TURBO;
    PepSpin.nTurStep   = P02_SPIN_TURSTEP;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */
    switch (GetDlgCtrlID((HWND)lParam)) {
    case IDD_P02SPIN_DEPT:
        nEdit = IDD_P02EDIT_DEPT;
        PepSpin.lMax = (long)FLEX_DEPT_MAX;
        break;

    case IDD_P02SPIN_PLU:
        nEdit = IDD_P02EDIT_PLU;
        PepSpin.lMax = (long)FLEX_PLU_MAX;
        break;
/*
    case IDD_P02SPIN_SER:
        nEdit = IDD_P02EDIT_SER;
        PepSpin.lMax = (long)FLEX_WT_MAX;
        break;
*/
    case IDD_P02SPIN_CAS:
        nEdit = IDD_P02EDIT_CAS;
        PepSpin.lMax = (long)FLEX_CAS_MAX;
        break;

    case IDD_P02SPIN_EJ:
        nEdit = IDD_P02EDIT_EJ;
        PepSpin.lMax = (long)FLEX_EJ_MAX;
        break;

    case IDD_P02SPIN_ETK:
        nEdit = IDD_P02EDIT_ETK;
        PepSpin.lMax = (long)FLEX_ETK_MAX;
        break;

    case IDD_P02SPIN_GC:
        nEdit = IDD_P02EDIT_GC;
        dwIdx = SendDlgItemMessage( hDlg, IDD_P02PTD_GC, CB_GETCURSEL, 0, 0L );
        if ( dwIdx != ( DWORD )FLEX_STORE_RECALL ) {
            PepSpin.lMax = (long)FLEX_GC_MAX;
        } else {
            PepSpin.lMax = (long)P02_STOREC_GCMAX;
        }
        break;

    case IDD_P02SPIN_CPN:
        nEdit = IDD_P02EDIT_CPN;
        PepSpin.lMax = (long)FLEX_CPN_MAX;
        break;

    case IDD_P02SPIN_CSTR:
        nEdit = IDD_P02EDIT_CSTR;
        PepSpin.lMax = (long)FLEX_CSTRING_MAX;
        break;
    case IDD_P02SPIN_ITEM:
        nEdit = IDD_P02EDIT_ITEM;
        PepSpin.lMax = (long)FLEX_ITEMSTORAGE_MAX;
        PepSpin.lMin = (long)FLEX_ITEMSTORAGE_MIN;
#if defined(POSSIBLE_DEAD_CODE)
// wIdW doesn't do anything
        wIdW = IDD_P02SPIN_ITEM;
#endif
        break;

    case IDD_P02SPIN_CONS:
        nEdit = IDD_P02EDIT_CONS;
        PepSpin.lMax = (long)FLEX_ITEMSTORAGE_MAX;
        PepSpin.lMin = (long)FLEX_ITEMSTORAGE_MIN;
#if defined(POSSIBLE_DEAD_CODE)
// wIdW doesn't do anything
        wIdW = IDD_P02SPIN_CONS;
#endif
        break;

    case IDD_P02SPIN_RPT:
        nEdit = IDD_P02EDIT_RPT;
        PepSpin.lMax = (long)FLEX_PROGRPT_MAX;
        break;

    case IDD_P02SPIN_PPI:
        nEdit = IDD_P02EDIT_PPI;
        PepSpin.lMax = (long)FLEX_PPI_MAX;
        break;
    }

    if (nEdit != 0) {
        /* ----- spin button procedure ----- */
        fRet = PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);
        if (fMemChk == FALSE) {
            /* ----- Calculate No. of blocks ----- */
            wIndex = (WORD)(nEdit - IDD_P02EDIT);
            *(lpdwBuff + wIndex) = P02CalcBlock(hDlg, wIndex);
        } else {
            /* ----- Set memory check flag ----- */
            fMemChk = FALSE;
        }
    }
    if (fRet == TRUE ) {    /* ***** add v1.01.03 ***** */
		HWND  hCtrl = GetDlgItem(hDlg, nEdit);
        PostMessage(hDlg, WM_COMMAND, MAKELONG(nEdit, EN_CHANGE), (LPARAM)hCtrl);
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   WORD    P02CalcSize_Item()
*
*   Input   :   LPP02BUFF   lpBuff         - address of buffer
*
*               WORD        lpBuff->dwData  - item data in editbox
*               BYTE        lpBuff->bPtd   - check of PTD in checkbox/combo-box
*
*   Return  :   DWORD    dwBlock  -   no. of block size
*
**  Description:
*       This procedure calculate file size on the memory.
* ===========================================================================
*/
DWORD    P02CalcSize_Item( LPP02BUFF lpBuff )
{
    DWORD   dwBlock;
    DWORD   dwItemFile;
    DWORD   dwItemIdx;
    DWORD   dwTtlUpd;
    DWORD   dwRemotePtr;

    if ( FLEX_ITEMSTORAGE_MIN <= lpBuff->dwData ) { 
        dwItemFile = PEPCALCITEMBOUND( lpBuff->dwData );
        dwItemIdx  = PEPCALCPRTPRTY( lpBuff->dwData );
        if ( chTerm == P02_TERM_MASTER ) {
            dwRemotePtr = PEPCALCRMTPTR( lpBuff->dwData);
            dwTtlUpd    = ( dwItemFile < P02_TTLUPMINSIZE ) ? P02_TTLUPMINSIZE : PEPCALCTOTALUP( lpBuff->dwData );
        } else {
            dwRemotePtr = 0UL;
            dwTtlUpd    = 0UL;
        }
    } else {
        dwItemFile  = 0UL;
        dwItemIdx   = 0UL;
        dwRemotePtr = 0UL;
        dwTtlUpd    = 0UL;
    }

    /* --- calculate block size of each item file --- */
    dwBlock = P02CalcIndexBlock( dwItemFile );   /* item storage file */
    dwBlock += P02CalcIndexBlock( dwItemIdx );   /* print priority idx 1 */
    dwBlock += P02CalcIndexBlock( dwItemIdx );   /* print priority idx 2 */
    dwBlock += P02CalcIndexBlock( dwRemotePtr ); /* remote ptr spool 1 */
    dwBlock += P02CalcIndexBlock( dwRemotePtr ); /* remote ptr spool 2 */
    dwBlock += P02CalcIndexBlock( dwTtlUpd );    /* total update */

    return ( dwBlock );
}

/*
* ===========================================================================
**  Synopsis:   WORD    P02CalcSize_Cons()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               LPP02BUFF   lpBuff         - address of buffer
*
*               WORD        lpBuff->dwData - item data in editbox
*               BYTE        lpBuff->bPtd   - check of PTD in checkbox/combo-box
*
*   Return  :   DWORD    dwBlock  -   no. of block size
*
**  Description:
*       This procedure calculate file size on the memory.
* ===========================================================================
*/
DWORD    P02CalcSize_Cons( HWND hDlg, LPP02BUFF lpBuff )
{
    DWORD   dwBlock;
    DWORD   dwConsFile;
    DWORD   dwConsIdx;
    DWORD   dwPostRec;
    DWORD   dwSerTmp;
    BYTE    bPtd;

    if ( FLEX_ITEMSTORAGE_MIN <= lpBuff->dwData ) {
        dwConsFile = PEPCALCCONSBOUND( lpBuff->dwData );
        dwConsIdx  = PEPCALCPRTPRTY( lpBuff->dwData );
        dwPostRec  = PEPCALCPOSTRCT( lpBuff->dwData );
        dwSerTmp   = PEPCALCSERV( lpBuff->dwData );
    } else {
        dwConsFile = 0UL;
        dwConsIdx  = 0UL;
        dwPostRec  = 0UL;
        dwSerTmp   = 0UL;
    }

    /* --- calculate block size of each item file --- */
    dwBlock = P02CalcIndexBlock( dwConsFile );   /* cons storage file */
    dwBlock += P02CalcIndexBlock( dwConsFile );  /* Multi Line Disp file */
    dwBlock += P02CalcIndexBlock( dwConsIdx );   /* print priority idx 1 */
    dwBlock += P02CalcIndexBlock( dwConsIdx );   /* print priority idx 2 */
    dwBlock += P02CalcIndexBlock( dwConsIdx );   /* print priority for MLD */
    dwBlock += P02CalcIndexBlock( dwPostRec );   /* post receipt file */
    dwBlock += P02CalcIndexBlock( dwSerTmp );    /* server module temp file */

    /* ----- Get PTD data from the PTD selection of GC ----- */
    bPtd = (BYTE)SendDlgItemMessage(hDlg, IDD_P02PTD_GC, CB_GETCURSEL, 0, 0L);
    if (bPtd == FLEX_STORE_RECALL) { /* store/recall */
        dwBlock += P02CalcIndexBlock( dwConsFile );  /* Flex Drive Thru file 1 */
        dwBlock += P02CalcIndexBlock( dwConsFile );  /* Flex Drive Thru file 2 */
    } else if (bPtd == FLEX_POST_CHK) { /* post check */
        dwBlock += P02CalcIndexBlock( dwConsFile );  /* Split file 1 */
        dwBlock += P02CalcIndexBlock( dwConsFile );  /* Split file 2 */
        dwBlock += P02CalcIndexBlock( dwConsIdx );   /* Split idx 1 */
        dwBlock += P02CalcIndexBlock( dwConsIdx );   /* Split idx 2 */
    }

    return ( dwBlock );
}

/*
* ===========================================================================
**  Synopsis:   VOID    P02DispGCMax()
*
*   Input   :   HWND    hDlg        -   handle of dialog
*               UCHAR   uchGCType   -   type of guest check system
**
*   Return  :   None
*
**  Description:
*       This procedure display guest check number range.
* ===========================================================================
*/
VOID P02DispGCMax( HWND hDlg, UCHAR uchGCType )
{
    UINT    wID;
    WCHAR    szCap[ PEP_CAPTION_LEN ];

    switch ( uchGCType ) {
    case FLEX_STORE_RECALL:
        wID = IDS_P02_MAX_STOREC;
        break;

    case FLEX_PRECHK_BUFFER:
    case FLEX_PRECHK_UNBUFFER:
    case FLEX_POST_CHK:
    default:
        wID = IDS_P02_MAX_GC;
        break;
    }

	LoadString(hResourceDll, wID, szCap, PEP_STRING_LEN_MAC(szCap));
	DlgItemRedrawText(hDlg, IDD_P02_MAXGC, szCap);
}

/*
* ===========================================================================
**  Synopsis:   static  USHORT  RecreatePluFile()
*
*   Input   :   HWND    hDlg   - handle of a dialog procedure
*               ULONG  ulRec  - No. of records
**
*   Return  :   USHORT  usStat - return valu from mass memory module
*
**  Description:
*       This procedure re-create PLU regular record file.
* ===========================================================================
*/
static  USHORT  RecreatePluFile(HWND hDlg, ULONG ulRec)
{
    HGLOBAL     hMem;
    LPSTR       lpszBuff;
    WCHAR        szTempFile[P02_PATH_LEN],
                szPath[P02_PATH_LEN],
                szFile[P02_TEMP_LEN];
    ECMBLOCK    EcmBlock;
    ECMCOND     EcmCond;
    USHORT      usHandle, usMirror, usRead, usStat;
//    OFSTRUCT    TempBuff;
    HANDLE       hTempFile;
    RECPLU      RecPlu;
//    UINT        unSize;
	DWORD		bytesWritten;

    if ((hMem = GlobalAlloc(GHND, P02_GLOBAL_LEN)) == 0) {
        /* allocate memory from global area */
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_PEP_ALLOC_ERR, 0);
        return TRUE;
    }

    if ((lpszBuff = GlobalLock(hMem)) == NULL) { /* lock memory area */
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_PEP_ALLOC_ERR, 0);

        /* ----- Free memory area ----- */
        GlobalFree(hMem);
        return TRUE;
    }

    /* ----- Set block access mode ----- */
    memset(&EcmBlock, 0, sizeof(ECMBLOCK));
    EcmBlock.fsOption = BLOCK_READ;

    /* ----- Enter critical mode (to open plu file) ----- */
    if ((usStat = PluEnterCritMode(FILE_PLU, FUNC_BLOCK_ACCESS, &usHandle, &EcmBlock)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    /* ----- Load string from resource ----- */
    LoadString(hResourceDll, IDS_P02_TEMPFILE, szFile, PEP_STRING_LEN_MAC(szFile));
    /* ----- Get windows path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szPath, PEP_STRING_LEN_MAC(szPath));
    wsPepf(szTempFile, WIDE("%s%s"),szPath,szFile);
#else
	GetPepModuleFileName(NULL, szPath, sizeof(szPath));
	szPath[wcslen(szPath)-8] = '\0';  //remove pep.exe from path
    //GetWindowsDirectoryPep(szPath, sizeof(szPath));
    /* ----- Make full path file name ----- */
    wsPepf(szTempFile, WIDE("%s\\%s"), szPath, szFile);
#endif

    /* ----- Open temporary file ----- */
    if ((hTempFile = CreateFilePep(szTempFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) /*OpenFile((LPCSTR)szTempFile,
                              &TempBuff,
                              (UINT)(OF_SHARE_DENY_NONE | OF_WRITE | OF_CREATE))*/) == INVALID_HANDLE_VALUE) {

        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)(SPLU_SYSTEM_FAIL - 1));

        /* ----- Exit critical mode ----- */
        PluExitCritMode(usHandle, 0);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return TRUE;
    }

    for (;;) {
        /* ----- Read plu file by block ----- */
        if ((usStat = PluReadBlock(usHandle, (WCHAR *)lpszBuff, P02_GLOBAL_LEN, &usRead)) != SPLU_COMPLETED) {
            break;
        }

        /* ----- Copy file to mirror file----- */
        WriteFile(hTempFile, lpszBuff, usRead, &bytesWritten, NULL);
        if (bytesWritten != usRead) {
            /* ----- Store error code ----- */
            usStat = SPLU_DEVICE_FULL;

            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_DISKFULL, (SHORT)usStat);

            /* ----- Close temporary file ----- */
            CloseHandle(hTempFile);

            /* ----- Exit critical mode ----- */
            PluExitCritMode(usHandle, 0);

            /* ----- Free memory area ----- */
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return (usStat);
        }
    }

    /* ----- Exit critical mode ----- */
    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close temporary file ----- */
    CloseHandle(hTempFile);

    /* ----- Create PLU file ----- */
    if ((usStat = PluCreateFile(FILE_PLU, ulRec)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);
    }

    /* ----- Create PLU Index file ----- */
    if ((usStat = PluCreateFile(FILE_PLU_INDEX, ulRec)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);
    }

    /* ----- Open mirror file ----- */
    if ((usStat = PluOpenMirror((WCHAR FAR *)szFile, &usMirror)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

        /* ----- Delete temporary file ----- */
        DeleteFilePep(szTempFile);
		//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    /* ----- Set report mode ----- */
    memset(&EcmCond, 0, sizeof(ECMCOND));
    EcmCond.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);

    /* ----- Enter critical mode (to get plu report) ----- */
    if ((usStat = PluEnterCritMode(usMirror, FUNC_REPORT_COND, &usHandle, &EcmCond)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

        /* ----- Close mirror file ----- */
        PluCloseMirror(usMirror);

        /* ----- Delete temporary file ----- */
        DeleteFilePep(szTempFile);
		//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    for (;;) {
        /* ----- Read record from mirror file ----- */
        if ((usStat = PluReportRecord(usHandle, &RecPlu, &usRead)) != SPLU_COMPLETED) {
            break;
        }

        /* ----- Assign record to PLU regular file ----- */
        if ((usStat = PluAddRecord(FILE_PLU, &RecPlu)) != SPLU_COMPLETED) {
            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

            /* ----- Exit critical mode ----- */
            PluExitCritMode(usHandle, 0);

            /* ----- Close mirror file ----- */
            PluCloseMirror(usMirror);

            /* ----- Delete temporary file ----- */
            DeleteFilePep(szTempFile);
			//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

            /* ----- Free memory area ----- */
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return (usStat);
        }
    }

    /* ----- Exit critical mode ----- */
    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close mirror file ----- */
    usStat = PluCloseMirror(usMirror);

    /* ----- Delete temporary file ----- */
	DeleteFilePep(szTempFile);
	//    OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

    /* ----- Free memory area ----- */
    GlobalUnlock(hMem);
    GlobalFree(hMem);
    return (usStat);
}

/*
* ===========================================================================
**  Synopsis:   static  USHORT  RecreatePluIndexFile()
*
*   Input   :   HWND    hDlg   - handle of a dialog procedure
*               ULONG   ulRec  - No. of records
**
*   Return  :   USHORT  usStat - return valu from mass memory module
*
**  Description:
*       This procedure re-create PLU index record file.
* ===========================================================================
*/
static  USHORT  RecreatePluIndexFile(HWND hDlg, ULONG ulRec)
{
    HGLOBAL     hMem;
    LPSTR       lpszBuff;
    WCHAR       szTempFile[P02_PATH_LEN],
                szPath[P02_PATH_LEN],
                szFile[P02_TEMP_LEN];
    ECMBLOCK    EcmBlock;
    ECMCOND     EcmCond;
    USHORT      usHandle, usMirror, usRead, usStat;
//    OFSTRUCT    TempBuff;
    HANDLE      hTempFile;
    RECPLU      RecPlu;
    RECIDX      RecPluIndex;
//    UINT        unSize;
	DWORD		bytesWritten;

    if ((hMem = GlobalAlloc(GHND, P02_GLOBAL_LEN)) == 0) {
        /* allocate memory from global area */
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_PEP_ALLOC_ERR, 0);
        return TRUE;
    }

    if ((lpszBuff = GlobalLock(hMem)) == NULL) { /* lock memory area */
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_PEP_ALLOC_ERR, 0);

        /* ----- Free memory area ----- */
        GlobalFree(hMem);
        return TRUE;
    }

    /* ----- Set block access mode ----- */
    memset((LPSTR)&EcmBlock, 0, sizeof(ECMBLOCK));
    EcmBlock.fsOption = BLOCK_READ;

    /* ----- Enter critical mode (to open plu file) ----- */
    if ((usStat = PluEnterCritMode(FILE_PLU, FUNC_BLOCK_ACCESS, &usHandle, &EcmBlock)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);
        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    /* ----- Load string from resource ----- */
    LoadString(hResourceDll, IDS_P02_TEMPFILE, szFile, PEP_STRING_LEN_MAC(szFile));
    /* ----- Get windows path ----- */
#if 1
	GetPepTemporaryFolder(NULL, szPath, PEP_STRING_LEN_MAC(szPath));
    wsPepf(szTempFile, WIDE("%s%s"),szPath,szFile);
#else
	GetPepModuleFileName(NULL, szPath, sizeof(szPath));
	szPath[wcslen(szPath)-8] = '\0';  //remove pep.exe from path
    //GetWindowsDirectoryPep(szPath, sizeof(szPath));
    /* ----- Make full path file name ----- */
    wsPepf(szTempFile, WIDE("%s\\%s"),szPath,szFile);
#endif

    /* ----- Open temporary file ----- */
    hTempFile = CreateFilePep(szTempFile, GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hTempFile == INVALID_HANDLE_VALUE) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)(SPLU_SYSTEM_FAIL - 1));

        /* ----- Exit critical mode ----- */
        PluExitCritMode(usHandle, 0);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return TRUE;
    }

    for (;;) {
        /* ----- Read plu file by block ----- */
        if ((usStat = PluReadBlock(usHandle, (WCHAR *)lpszBuff, P02_GLOBAL_LEN, &usRead)) != SPLU_COMPLETED) {
            break;
        }

        /* ----- Copy file to mirror file----- */
        WriteFile(hTempFile, lpszBuff, usRead, &bytesWritten, NULL);
        if (bytesWritten != usRead) {
            /* ----- Store error code ----- */
            usStat = SPLU_DEVICE_FULL;
            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_DISKFULL, (SHORT)usStat);
            /* ----- Close temporary file ----- */
            CloseHandle(hTempFile);
            /* ----- Exit critical mode ----- */
            PluExitCritMode(usHandle, 0);
            /* ----- Free memory area ----- */
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return (usStat);
        }
    }

    /* ----- Exit critical mode ----- */
    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close temporary file ----- */
    CloseHandle(hTempFile);

    /* ----- Create PLU Index file ----- */
    if ((usStat = PluCreateFile(FILE_PLU_INDEX, ulRec)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);
    }

    /* ----- Open mirror file ----- */
    if ((usStat = PluOpenMirror((WCHAR FAR *)szFile, &usMirror)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);
        /* ----- Delete temporary file ----- */
		DeleteFilePep(szTempFile);
        //OpenFile(szTempFile, &TempBuff, OF_DELETE);
        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    /* ----- Set report mode ----- */
    memset((LPSTR)&EcmCond, 0, sizeof(ECMCOND));
    EcmCond.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);

    /* ----- Enter critical mode (to get plu report) ----- */
    if ((usStat = PluEnterCritMode(usMirror, FUNC_REPORT_COND, &usHandle, &EcmCond)) != SPLU_COMPLETED) {
        /* ----- Display caution message ----- */
        P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

        /* ----- Close mirror file ----- */
        PluCloseMirror(usMirror);

        /* ----- Delete temporary file ----- */
		DeleteFilePep(szTempFile);
        //OpenFile(szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        return (usStat);
    }

    for (;;) {
        /* ----- Read record from mirror file ----- */
        if ((usStat = PluReportRecord(usHandle, &RecPlu, &usRead)) != SPLU_COMPLETED) {
            break;
        }

        /* ----- Set Index Record ---- */
        memcpy(&RecPluIndex.aucPluNumber, &RecPlu.aucPluNumber, OP_PLU_LEN * sizeof(WCHAR));
        RecPluIndex.uchAdjective = RecPlu.uchAdjective;

        /* ----- Assign record to PLU regular file ----- */
        if ((usStat = PluAddRecord(FILE_PLU_INDEX, &RecPluIndex)) != SPLU_COMPLETED) {
            /* ----- Display caution message ----- */
            P02DispCaution(hDlg, IDS_P02_ERR_UNKOWN, (SHORT)usStat);

            /* ----- Exit critical mode ----- */
            PluExitCritMode(usHandle, 0);

            /* ----- Close mirror file ----- */
            PluCloseMirror(usMirror);

            /* ----- Delete temporary file ----- */
			DeleteFilePep(szTempFile);
            //OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

            /* ----- Free memory area ----- */
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return (usStat);
        }
    }

    /* ----- Exit critical mode ----- */
    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close mirror file ----- */
    usStat = PluCloseMirror(usMirror);

    /* ----- Delete temporary file ----- */
	DeleteFilePep(szTempFile);
    //OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

    /* ----- Free memory area ----- */
    GlobalUnlock(hMem);
    GlobalFree(hMem);
    return (usStat);
}

//Initialize Strings for Dialog
//A part of the Multilingual translation requirements
//RPH 4-22-03
/*VOID InitDialogStrings(HWND hDlg)
{
 
	WCHAR szWork[128];
	
	LoadString(hResourceDll, IDS_PEP_TERMTYPE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_TERMINAL, szWork);
	
	LoadString(hResourceDll, IDS_PEP_OK, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDOK, szWork);

    LoadString(hResourceDll, IDS_PEP_CANCEL, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDCANCEL, szWork);

    LoadString(hResourceDll, IDS_P02_PTD, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02PTD_DEPT, szWork);

    LoadString(hResourceDll, IDS_P02_PTD, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02PTD_CAS, szWork);

    LoadString(hResourceDll, IDS_P02_OVERRIDE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02PTD_EJ, szWork);
	
    LoadString(hResourceDll, IDS_P02_PTD, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02PTD_CPN, szWork);

    LoadString(hResourceDll, IDS_P02_PTD, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02PTD_PLU, szWork);

    LoadString(hResourceDll, IDS_P02_DEPT, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_DEPT, szWork);

    LoadString(hResourceDll, IDS_PEP_PLU, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_PLU, szWork);

    LoadString(hResourceDll, IDS_P02_OPERATOR, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_OPERATOR, szWork);

    LoadString(hResourceDll, IDS_P02_EMPLOYEE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_EMPLOYEE, szWork);

    LoadString(hResourceDll, IDS_P02_EJ, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_EJ, szWork);

    LoadString(hResourceDll, IDS_P02_CTLSTR, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_CTLSTR, szWork);

    LoadString(hResourceDll, IDS_P02_COUPON, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_COUPON, szWork);

    LoadString(hResourceDll, IDS_P02_PPI, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_PPI, szWork);

    LoadString(hResourceDll, IDS_P02_PROGRPT, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_PROGRPT, szWork);

    LoadString(hResourceDll, IDS_P02_GUEST, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_GUEST, szWork);

    LoadString(hResourceDll, IDS_P02_ITEMA, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_ITEMA, szWork);

    LoadString(hResourceDll, IDS_P02_ITEMB, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_ITEMB, szWork);

    LoadString(hResourceDll, IDS_P02_MEM, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_MEM, szWork);

    LoadString(hResourceDll, IDS_P02_RATE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02RATE, szWork);

    LoadString(hResourceDll, IDS_P02_BYTE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02BYTE, szWork);

    LoadString(hResourceDll, IDS_P02_AB, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_AB, szWork);

    LoadString(hResourceDll, IDS_P02_RAM_SIZE, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDC_P02_RAM_SIZE, szWork);

    LoadString(hResourceDll, IDS_P02_NUMITEM, szWork,PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDD_P02_NUMITEM, szWork);

}
*/
/* ===== End of P002.C ===== */