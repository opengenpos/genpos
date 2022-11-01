/*
* ---------------------------------------------------------------------------
* Title         :   Print Modifier Mnemonics (Prog. 47)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P047.C
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

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p047.h"

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
static LPP47MNEMO lpMnemonic;   /* Points to Global Area of Mnemonic Data   */
static LPP47WORK  lpWork;       /* Points to Global Area of Work Area       */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P047DlgProc()
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
*               Print Modifier Mnemonics (Program Mode # 47)
* ===========================================================================
*/
BOOL    WINAPI  P047DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbMnemo;          /* Handle of Global Area (Mnemonic) */
    static  HGLOBAL hGlbWork;           /* Handle of Global Area (Work)     */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Grobal Area for Mnemonic Data ----- */

        hGlbMnemo = GlobalAlloc(GHND, sizeof(P47MNEMO) * P47_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P47WORK)  * P47_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)) {    /* Memory Allocation Error  */

            /* ----- Text out Error Message ----- */

            P47DispAllocErr(hDlg, hGlbMnemo, hGlbWork);

            EndDialog(hDlg, TRUE);

        } else {                                /* Allocation was Success   */

            /* ----- Lock Global Allocation Area ----- */

            lpMnemonic = (LPP47MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP47WORK)GlobalLock(hGlbWork);

            /* ----- Initialize Configulation of DialogBox ----- */

            P47InitDlg(hDlg);
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P47_01; j<=IDD_P47_CAPTION2; j++)
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

                    /* ----- Save Inputed Data to Parameter File ----- */

                    P47SaveData(hDlg);
                }

                /* ----- Unlock Global Allocated Area & Set Them Free ----- */

                GlobalUnlock(hGlbMnemo);
                GlobalFree(hGlbMnemo);

                GlobalUnlock(hGlbWork);
                GlobalFree(hGlbWork);

                EndDialog(hDlg, FALSE);

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
**  Synopsis    :   VOID    P47InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads initial data from Parameter file, and initializes
*   the contols in DialogBox.
* ===========================================================================
*/
VOID    P47InitDlg(HWND hDlg)
{
    USHORT  usRet;
    short   nCount;

    /* ----- Load Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAPRTMODI,
                (UCHAR *)lpMnemonic,
                (P47_ADDR_MAX * (P47_MNE_LEN) * 2),
                0, &usRet);

    for (nCount = 0; nCount < P47_ADDR_MAX; nCount++) {

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_P47_01 + nCount));

        PepSetFont(hDlg, IDD_P47_01 + nCount);

        /* ----- Replace Double Width Key (0x12 -> '~') ----- */

        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount),
                           (WCHAR *)(lpWork + nCount),
                           (P47_MNE_LEN ),
                           PEP_MNEMO_READ);

        /* ----- Set Limit Length to Each EditText ----- */

        SendDlgItemMessage(hDlg,
                           IDD_P47_01 + nCount,
                           EM_LIMITTEXT, 
                           P47_MNE_LEN, 0L);

        /* ----- Set Loaded Data to Each EditText ----- */

        DlgItemRedrawText(hDlg, IDD_P47_01 + nCount, (WCHAR *)(lpWork + nCount));
    }

    /* ----- Set HighLight Bar to EditText ----- */

    SendDlgItemMessage(hDlg, IDD_P47_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P47SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed data from each edittext, and then it writes
*   them to Paramater file.
* ===========================================================================
*/
VOID    P47SaveData(HWND hDlg)
{
    short   nCount;
    USHORT  usRet;

    for (nCount = 0; nCount < P47_ADDR_MAX; nCount++) {

        /* ----- Get Inputed Data from EditText ----- */

        DlgItemGetText(hDlg,
                       IDD_P47_01 + nCount,
                       (WCHAR *)(lpWork + nCount),
                       (P47_MNE_LEN + 1));

        /* ----- Replace Double Width Key ('~' -> 0x12) ----- */

        PepReplaceMnemonic((WCHAR *)(lpWork + nCount),
                           (WCHAR *)(lpMnemonic + nCount),
                           (P47_MNE_LEN ),
                           PEP_MNEMO_WRITE);
    }

    ParaAllWrite(CLASS_PARAPRTMODI,
                 (UCHAR *)lpMnemonic,
                 ((P47_MNE_LEN ) * P47_ADDR_MAX * 2),
                 0, &usRet);

    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P47DispAllocErr()
*
**  Input       :   HWND    hdlg        -   Window Handle of a DialogBox
*                   HGLOBAL hGlbMnemo   -   Handle of a Global Area (Mnemonic)
*                   HGLOBAL hGlbWork    -   Handle of a Global Area (Work)
*
**  Return      :   No return value.
*
**  Description :
*       This function shows the error message for global allocation error. And
*   it resets the global heap area, if allocation was success.
* ===========================================================================
*/
VOID    P47DispAllocErr(HWND hdlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[PEP_CAPTION_LEN],
         szMessage[PEP_ALLOC_LEN];

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_P47, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hdlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Reset Allocated Area ----- */

    if (hGlbMnemo) {

        GlobalFree(hGlbMnemo);
    }

    if (hGlbWork) {

        GlobalFree(hGlbWork);
    }
}

/* ===== End of File (P047.C) ===== */
