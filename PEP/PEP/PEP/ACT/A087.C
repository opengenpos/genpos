/*
* ---------------------------------------------------------------------------
* Title         :   Trailer Print for Soft Check
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A087.C
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a087.h"

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
static  LPA87MNEMO  lpMnemonic;     /* Address of Mnemonic Data Buffer Area */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A087DlgProc()
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
*               Trailer Print for Soft Check (AC 87) 
* ===========================================================================
*/
BOOL    WINAPI  A087DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbMnemo;          /* Handle of Global Heap Area   */

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Allocate Global Memory for Mnemonic Data ----- */
        if (A087AllocMem(hDlg, (LPHGLOBAL)&hGlbMnemo)) {   
			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        } else {
            A087InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A087_DESC01; j<=IDD_A087_EPTLOGO; j++)
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
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    A087SaveData(hDlg);
                }
                /* ----- Unlock Global Allocated Area & Set Them Free ----- */
                GlobalUnlock(hGlbMnemo);
                GlobalFree(hGlbMnemo);
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
**  Synopsis    :   BOOL    A087AllocMem()
*
**  Input       :   HWND        hDlg        -   Window Handle of DialogBox
*                   LPHGLOBAL   lphGlobal   -   Address of Global Heap Handle
*
**  Return      :   BOOL    TRUE    -   Memory Allocation is Failed.
*                           FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This function allocates the mnemonic data buffer from the global heap.
*   It returns TRUE, if it failed in momory allocation. Otherwise it returns 
*   FALSE.
* ===========================================================================
*/
BOOL    A087AllocMem(HWND hDlg, LPHGLOBAL lphGlobal)
{
    BOOL    fRet = FALSE;
    WCHAR   szCaption[PEP_CAPTION_LEN], szErrMsg[PEP_ALLOC_LEN];

    *lphGlobal = GlobalAlloc(GHND, A87_ADDR_MAX * sizeof(A87MNEMO) * sizeof(WCHAR));

    if (! *lphGlobal) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A87, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,   szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;
    } else {
        lpMnemonic = (LPA87MNEMO)GlobalLock(*lphGlobal);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A087InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from the Parameter file. And then
*   it sets them to each statictext, and initializes the configulation of the
*   dialogbox.
* ===========================================================================
*/
VOID    A087InitDlg(HWND hDlg)
{
	DWORD    iIds, iIdd;
    USHORT   usRet;
    WORD     wEditID, wI;
    WCHAR    szWork[A87_MNE_LEN + 1];
    WCHAR    szDesc[128];

    /* ----- set initial description, V3.3 ----- */
    LoadString(hResourceDll, IDS_A087_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
    WindowRedrawText(hDlg, szDesc);

	for (iIds = IDS_A087_DESC01, iIdd = IDD_A087_DESC01; iIds < IDS_A087_LEN; iIds++, iIdd++) {
		LoadString(hResourceDll, iIds, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, iIdd, szDesc);
	}

    /* ----- Load Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARASOFTCHK, (UCHAR *)lpMnemonic, ((A87_MNE_LEN ) * sizeof(WCHAR) * A87_ADDR_MAX), 0, &usRet);
        
    for (wI = 0, wEditID = IDD_A087_01; wI < A87_ADDR_MAX; wI++, wEditID++) {
        /* ----- Limit Length of Input Data ----- */
        SendDlgItemMessage(hDlg, wEditID, EM_LIMITTEXT, A87_MNE_LEN, 0L);

        /* ----- Set Fixed Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wEditID);
        PepSetFont(hDlg, wEditID);

        /* ----- Replace Double Key Character (0x12 -> '~') ----- */
		memset(szWork,0x00, sizeof(szWork));
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + wI), (WCHAR *)szWork, (A87_MNE_LEN ),  PEP_MNEMO_READ);

        DlgItemRedrawText(hDlg, wEditID, szWork);
    }

    /* ----- Set Focus to EditText ----- */
    SendDlgItemMessage(hDlg, IDD_A087_01, EM_SETSEL, 1, MAKELONG(0 , -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A087SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from each edittext. And then it
*   writes them to the Parameter file.
* ===========================================================================
*/
VOID    A087SaveData(HWND hDlg)
{
    USHORT  usRet;
    WORD    wEditID,
            wI;
    WCHAR    szWork[A87_MNE_LEN + 1];

    for (wI = 0, wEditID = IDD_A087_01; wI < A87_ADDR_MAX; wI ++, wEditID++) {
        /* ----- Get Inputed Mnemonic from EditText ----- */
        DlgItemGetText(hDlg, wEditID, szWork, sizeof(szWork));
        /* ----- Replace Double Key Character ('~' -> 0x12) ----- */
        PepReplaceMnemonic((WCHAR *)szWork, (WCHAR *)(lpMnemonic + wI), (A87_MNE_LEN ), PEP_MNEMO_WRITE);
    }

    /* ----- Write Inputed Data to Parameter File ----- */
    ParaAllWrite(CLASS_PARASOFTCHK, (UCHAR *)lpMnemonic, ((A87_MNE_LEN) * sizeof(WCHAR) * A87_ADDR_MAX), 0, &usRet);
    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
}

/* ===== End of A087.C ===== */
