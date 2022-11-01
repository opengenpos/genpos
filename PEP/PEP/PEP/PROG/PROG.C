/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Setup
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PROG.C
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "pepcomm.h"
#include    "file.h"
#include    "prog.h"

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
#if defined(POSSIBLE_DEAD_CODE)
// never used
HANDLE      hProgInst;      /* Instance Handle of PEPPROG.DLL               */
HANDLE      hProgCommInst;  /* Instance Handle of PEPCOMM.DLL               */
#endif
HWND        hwndProgMain;   /* Window Handle of PEP Window                  */
LPHANDLE    lphProgDlgChk1; /* Address of Handle of Modeless DialogBox      */
LPHANDLE    lphProgDlgChk2; /* Address of Handle of Modeless DialogBox      */

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
#if defined(POSSIBLE_DEAD_CODE)
// doesn't do anything

/*
* ===========================================================================
**  Synopsis:   VOID    ProgFinalize();
*
**  Input   :   None
*
**  Return  :   None
*
**  Description:
*       This function finalize PEPPROG.DLL.
* ===========================================================================
*/
VOID    ProgFinalize(void)
{
    return;
}

#endif
/*
* ===========================================================================
**  Synopsis    :   BOOL    DoProgram()
*
**  Input       :   HWND    hWnd            -   Window Handle of PROG Window
*                   FARPROC lpfnProcName    -   Address of Procedure Name
*                   WORD    wID             -   ID of DialogBox Template Name
*                   HANDLE  hPepInst        -   Instance Handle of PEP:Main
*
**  Return      :   TRUE                    -   Must be TRUE
*
**  Description:
*       This function creates a modal dialogbox of program mode.
* ===========================================================================
*/
BOOL  DoProgram(HWND hWnd, DLGPROC lpfnProcName, WPARAM wID, HINSTANCE hPepInst)
{
	INT_PTR ipResult = 0;
	DWORD   dwLastError = 0;

    hwndProgMain = hWnd;
    hPepInst     = hPepInst;

    /* ----- Create a Modal DialogBox from a DialogBox Template Resource ----- */
	
    ipResult = DialogBoxPopup(hResourceDll/*hProgInst */, MAKEINTRESOURCEW(wID), hWnd, lpfnProcName);
	dwLastError = GetLastError ();

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    DoProgram01()
*
**  Input       :   HWND        hWnd        -   Window Handle of PEP Window
*                   LPHANDLE    lphDlgChk1  -   Address of Modeless DialogBox
*                   LPHANDLE    lphDlgChk2  -   Address of Modeless DialogBox
*
**  Return      :   TRUE            -   Must be TRUE
*
**  Description :
*       This function creates a modeless dialogbox of program mode #01.
* ===========================================================================
*/
BOOL  DoProgram01(HWND hWnd, LPHANDLE lphDlgChk1, LPHANDLE lphDlgChk2)
{
    /* ----- Set Address of Window Handle of Modeless ----- */
    /* -----    DialogBox to Public Work Area         ----- */

    lphProgDlgChk1 = lphDlgChk1;
    lphProgDlgChk2 = lphDlgChk2;

    hwndProgMain = hWnd;

    /* ----- Determine Whether a Main DialogBox Already Exists or Not ----- */

    if (*lphDlgChk1) {          /* The Main DialogBox is Created */

        /* ----- Activate a Main DialogBox ----- */

        SetActiveWindow(*lphDlgChk1);

    } else {                    /* The Main DialogBox is Not Created */

        /* ----- Create a Main DialogBox ----- */
        LPTSTR jj = MAKEINTRESOURCE(IDD_P01);

        *lphDlgChk1 = DialogCreation(hResourceDll/*hProgInst */,
                                   MAKEINTRESOURCEW(IDD_P01),
                                   hWnd,
                                   P001DlgProc);

        if (*lphDlgChk1 == NULL) {
            MessageBeep(MB_ICONEXCLAMATION);
        }

    }
    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    DoProgram03()
*
**  Input       :   HWND        hWnd        -   Window Handle of PEP Window
*                   LPHANDLE    lphDlgChk1  -   Address of Modeless DialogBox
*                   LPHANDLE    lphDlgChk2  -   Address of Modeless DialogBox
*                   WORD        wID         -   KeyBoard DialogBox ID
*
**  Return      :   TRUE            -   Must be TRUE
*
**  Description :
*       This function creates a modeless dialogbox of program mode #03.
* ===========================================================================
*/
BOOL DoProgram03(HWND hWnd, LPHANDLE lphDlgChk1, LPHANDLE lphDlgChk2, WPARAM wID)
{
    /* ----- Set Address of Window Handle of Modeless ----- */
    /* -----    DialogBox to Public Work Area         ----- */

    lphProgDlgChk1 = lphDlgChk1;
    lphProgDlgChk2 = lphDlgChk2;

    hwndProgMain = hWnd;

    /* ----- Determine Whether a Main DialogBox Already Exists or Not ----- */

    if (*lphDlgChk1) {          /* The Main DialogBox is Created */

        /* ----- Activate a Main DialogBox ----- */

        SetActiveWindow(*lphDlgChk1);

    } else {                    /* The Main DialogBox is Not Created */

        /* ----- Create a Main DialogBox (KeyBoard) ----- */

        *lphDlgChk1 = DialogCreation(hResourceDll,//hProgInst,
                                   MAKEINTRESOURCEW(wID),
                                   hWnd,
                                   P003KeyDlgProc);

        /* ----- Create a Sub DialogBox (Tool Box) ----- */

        *lphDlgChk2 = DialogCreation(hResourceDll,//hProgInst,
                                   MAKEINTRESOURCEW(IDD_P03_BOX),
                                   hWnd,
                                   P003ToolDlgProc);

        if ((*lphDlgChk1 == NULL) || (*lphDlgChk2 == NULL)) {

            MessageBeep(MB_ICONEXCLAMATION);

            if (*lphDlgChk1) {
                DestroyWindow(*lphDlgChk1);
            }

            if (*lphDlgChk2) {
                DestroyWindow(*lphDlgChk2);
            }
        }
    }
    return TRUE;
}

/* ===== End of File (Prog.C) ===== */
