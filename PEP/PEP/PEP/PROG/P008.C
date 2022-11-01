/*
* ---------------------------------------------------------------------------
* Title         :   Supervisor Number/Level (Prog. 8)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P008.C
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

//#include	"stringResource.h"
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p008.h"

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
static  LPSUPLEVEL  lpSupLevel;     /* Address of Inputed Data Area */
static  HGLOBAL     hGlb;           /* Handle of Global Heap        */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P008DlgProc()
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
*           Supervisor Number/Level (Program Mode # 08)
* ===========================================================================
*/
BOOL    WINAPI  P008DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;

    int     idEdit;
    short   nCount;                 /* Counter                          */
    WORD    wID;                    /* Target Control ID for Loop       */
    USHORT  usReturnLen;            /* Return Length of ParaAllWrite    */
    UINT    unValue;
    BOOL    fTrans;
    WCHAR   szWork[128], szCap[128], szDesc[128];

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
		/* ----- Initialize Configuration of Dialog Box ----- */
        if (P08InitDlg(hDlg)) {    /* Memory Allocation Error Occurred */
            EndDialog(hDlg, TRUE);
        } else {                /* Succeeded in Memory Allocation */
            /* ----- Check Data Range of Supervisor Level ----- */
            P08ChkData(hDlg);

            /* ----- Set Focus to EditText ----- */
            SendDlgItemMessage(hDlg, IDD_P08_LEV_0, EM_SETSEL, 1, MAKELONG(0, -1));

            /* ----- Initialize Configulation of SpinButton ----- */
            PepSpin.lMax       = (long)P08_DATA_MAX;
            PepSpin.lMin       = (long)P08_DATA_MIN;
            PepSpin.nStep      = P08_SPIN_STEP;
            PepSpin.nTurbo     = P08_SPIN_TURBO;
            PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;

			for (j = IDD_P08_LEV_0; j <= IDD_P08_LEV_1; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}

			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        idEdit = GetDlgCtrlID((HWND)lParam) - P08_SPIN_OFFSET;
        if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
            if ((idEdit == IDD_P08_LEV_0) || (idEdit == IDD_P08_LEV_1)) {
                P08ChkData(hDlg);
            }
        }

        /* ----- An application should return zero   ----- */
        /* ----- if it processes WM_VSCROLL message. ----- */
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P08_LEV_0:
        case IDD_P08_LEV_1:
//        case IDD_P08_01 thru IDD_P08_20 deleted with Rel 2.2 since no longer Supervisor Ids.
            if (HIWORD(wParam) == EN_CHANGE) {
                if ((LOWORD(wParam) == IDD_P08_LEV_0) || (LOWORD(wParam) == IDD_P08_LEV_1)) {
                    /* ----- Check Data Range of Supervisor Level ----- */
                    P08ChkData(hDlg);
                }
                return TRUE;
            } else if (HIWORD(wParam) == EN_KILLFOCUS) {
				//Makes sure no value under the minimum is allowed ***PDINU
 				if (GetDlgItemInt(hDlg, LOWORD(wParam), NULL, TRUE) < P08_DATA_MIN)
				{
					/* ----- Load Error Message ----- */
					LoadString(hResourceDll, IDS_PEP_CAPTION_P08, szCap, PEP_STRING_LEN_MAC(szCap));
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
					wsPepf(szDesc, szWork, P08_DATA_MIN, P08_DATA_MAX);

					/* ----- Display Message Box ----- */
                     MessageBeep(MB_ICONEXCLAMATION);
                     MessageBoxPopUp(hDlg, szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);

					/* ----- Set Focus to Error Occurred EditText ----- */
					wID = LOWORD(wParam);
					SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
					SetFocus(GetDlgItem(hDlg, wID));

                    return TRUE;
				}

                /* ----- Get Inputed Supervisor No. from EditText ----- */
                unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
                wsPepf(szWork, WIDE("%02u"), unValue);
                DlgItemRedrawText(hDlg, LOWORD(wParam), szWork);
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    wID = IDD_P08_LEV_0;
                    for (nCount = 0; wID <= IDD_P08_LEV_1; nCount++, wID++) {
                        /* ----- Get Inputed Data from EditText ----- */
                        unValue = GetDlgItemInt(hDlg, wID, (LPBOOL)&fTrans, FALSE);
                        if (fTrans == 0) {
                            /* ----- Load Error Message ----- */
                            LoadString(hResourceDll, IDS_PEP_CAPTION_P08, szCap, PEP_STRING_LEN_MAC(szCap));
                            LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
                            wsPepf(szDesc, szWork, P08_DATA_MIN, P08_DATA_MAX);

                            /* ----- Display Message Box ----- */
                            MessageBeep(MB_ICONEXCLAMATION);
                            MessageBoxPopUp(hDlg, szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);
                            return TRUE;
                        } else {
                            /* ----- Set Supervisor No. to Buffer ----- */
                            (lpSupLevel + nCount)->usSuperNumber = (USHORT)(unValue);
                            /* ----- Initialize Secret Data ----- */
                            (lpSupLevel + nCount)->uchSecret = P08_SECRET;
                        }
                    }

                    ParaAllWrite(CLASS_PARASUPLEVEL, (UCHAR *)lpSupLevel,
                                 (sizeof(SUPLEVEL) * P08_ADDR_MAX), 0, &usReturnLen);

                    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
                }

                /* ----- Free Global Allocated Area ----- */
                GlobalUnlock(hGlb);
                GlobalFree(hGlb);
            
                EndDialog(hDlg, TRUE);
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
**  Synopsis    :   BOOL    P08InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   BOOL    TRUE    -   Memory Allocation Error Occured
*                           FALSE   -   Memory Allocation was Success
*
**  Description :
*       This function allocates the buffer from the global heap. And then
*   reads initial supervisor data from Parameter file.
* ===========================================================================
*/
BOOL    P08InitDlg(HWND hDlg)
{
    BOOL    fReturn = FALSE;            /* Return Value of This Function */
    WORD    wID;                        /* Target Control ID for Loop    */
    USHORT  usReturnLen,                /* Return Length of ParaAllRead  */
            usWork;                     /* Supervivisor No. Work Area    */
    WCHAR   szSupText[P08_SUP_LEN],     /* Supervisor No. Strings        */
            szErrMsg[P08_ERR_LEN],      /* Allocation Error Message      */
            szCaption[P08_CAP_LEN];     /* MessageBox Caption            */
    short   nCount;                     /* Loop Counter                  */
	
	//Mulitlingual RPH 4-22-03
//	InitP08DlgStrings(hDlg);
    
	/* ----- Allocate Buffer from Grobal Heap ----- */
    hGlb = GlobalAlloc(GHND, sizeof(SUPLEVEL) * P08_ADDR_MAX);
    if (! hGlb) {           /* Memory Allocation Error */
        fReturn = TRUE;
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));
		LoadString(hResourceDll, IDS_PEP_CAPTION_P08,szCaption, PEP_STRING_LEN_MAC(szCaption));

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErrMsg,szCaption, MB_OK | MB_ICONEXCLAMATION);
    } 
	else {                /* Memory Allocation was Success */

        lpSupLevel = (LPSUPLEVEL)GlobalLock(hGlb);

        /* ----- Read Initial Data from Parameter File ----- */
        ParaAllRead(CLASS_PARASUPLEVEL, (UCHAR *)lpSupLevel, (sizeof(SUPLEVEL) * P08_ADDR_MAX), 0, &usReturnLen);

        /* ----- Set Limit Length and Initial Data to EditText ----- */
        wID = IDD_P08_LEV_0;
        for (nCount = 0; wID <= IDD_P08_LEV_1; nCount++, wID++) {
            /* ----- Set Initial Data to EditText ----- */
            if (! (lpSupLevel + nCount)->usSuperNumber) {
                usWork = ((lpSupLevel + nCount)->usSuperNumber) + P08_DATA_MIN;
            } else {
                usWork = ((lpSupLevel + nCount)->usSuperNumber);
            }

            wsPepf(szSupText, WIDE("%02u"), usWork);
            DlgItemRedrawText(hDlg, wID, szSupText);

            /* ----- Set Limit Length to EditText ----- */
            SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P08_DATA_LEN, 0L);
        }
    }
    return (fReturn);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P08ChkData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No Return Value
*
**  Description :
*       This function determines supervisor level with inputed supervisor No.
*   And then it indicates text of supervisor level to enable or disable.
* ===========================================================================
*/
VOID    P08ChkData(HWND hDlg)
{
    UINT    unLevel0;           /* The Highest Supervisor # for Level 0 */
    UINT    unLevel1;           /* The Highest Supervisor # for Level 1 */
    BOOL    fEnableLev0 = TRUE; /* StaticText Disable/Enable Flag       */
    BOOL    fEnableLev1 = TRUE; /* StaticText Disable/Enable Flag       */
    BOOL    fEnableLev2 = TRUE; /* StaticText Disable/Enavle Flag       */

    /* ----- Get Value of Inputed Data ----- */
    unLevel0 = GetDlgItemInt(hDlg, IDD_P08_LEV_0, NULL, FALSE);
    unLevel1 = GetDlgItemInt(hDlg, IDD_P08_LEV_1, NULL, FALSE);

    /* ----- Check Range of Supervisor Level ----- */
    if (unLevel0 == P08_DATA_MAX) {
        fEnableLev1 = FALSE;
        fEnableLev2 = FALSE;
    } else if (unLevel0 >= unLevel1) {
        fEnableLev1 = FALSE;
    } else if (unLevel1 == P08_DATA_MAX) {
        fEnableLev2 = FALSE;
    }

    if (unLevel0 == P08_DATA_MIN) {
        fEnableLev0 = FALSE;
    }

    /* ----- Disable / Enable StaticText (Level 0 / Level 1 / Level 2) ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_P08_LEVEL0), fEnableLev0);
    EnableWindow(GetDlgItem(hDlg, IDD_P08_LEVEL1), fEnableLev1);
    EnableWindow(GetDlgItem(hDlg, IDD_P08_LEVEL2), fEnableLev2);
}

//Multilingual
//load strings for dialog
//RPH 4-22-03
/*VOID InitP08DlgStrings(HWND hDlg)
{
	WCHAR	szWork[128];

	LoadString(hResourceDll, IDS_PEP_OK, szWork, PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDOK, szWork);

	LoadString(hResourceDll, IDS_PEP_CANCEL, szWork, PEP_STRING_LEN_MAC(szWork));
	DlgItemRedrawText(hDlg, IDCANCEL, szWork);

}
*/
/* ===== End of P008.C ===== */
