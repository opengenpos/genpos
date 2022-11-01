/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Action Code Main Procedure
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   ACTION.C
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

#ifdef __DEBUG__
#include <string.h>
#endif

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "pepcomm.h"
#include    "action.h"

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
//  never used
HANDLE      hActInst;       /* Instance Handle of ACTION Window             */
HANDLE      hActCommInst;   /* Instance Handle of PEPCOMM.DLL               */
HANDLE      hActPepInst;    /* Instance Handle of PEP.EXE                   */
#endif
HWND        hwndActMain;    /* Window Handle of PEP Window                  */
LPHANDLE    lphActDlgChk1;  /* Address of Handle of Modeless DialogBox      */
LPHANDLE    lphActDlgChk2;  /* Address of Handle of Modeless DialogBox      */

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
//  variables that are set here are never used.

/*
* ===========================================================================
**  Synopsis:   VOID    PEPENTRY    ActInitialize();
*
**  Input   :   HANDLE  hPep    - instance handle of PEP.EXE
*               HANDLE  hAct    - instance handle of PEPACT.DLL
*               HANDLE  hComm   - instance handle of PEPCOMM.DLL
*
**  Return  :   None
*
**  Description:
*       This procedure initialize PEPACT.DLL.
* ===========================================================================
*/
VOID    PEPENTRY    ActInitialize(HANDLE hPep, HANDLE hAct, HANDLE hComm)
{
    /* ----- Store instance handle ----- */
    hActPepInst  = hPep;
//    hActInst     = hAct;  // replaced by RJC for 32 bit conversion DLL to LIB
    hActInst     = hPep;
    hActCommInst = hComm;
}

#endif
#if defined(POSSIBLE_DEAD_CODE)
//  never called, and it doesn't do anything
/*
* ===========================================================================
**  Synopsis:   VOID    PEPENTRY    ActFinalize();
*
**  Input   :   None
*
**  Return  :   None
*
**  Description:
*       This procedure finalize PEPACT.DLL.
* ===========================================================================
*/
VOID    PEPENTRY    ActFinalize(void)
{
    return;
}

#endif
/*
* ===========================================================================
**  Synopsis    :   BOOL    PEPENTRY  DoAction()
*
**  Input       :   HWND        hWnd         -   Window Handle of PEP Window
*                   FARPROC     lpfnProcName -   Address of Procedure Name
*                   WORD        wID          -   ID of DialogBox Template Name
*                   HINSTANCE   hPepInst     -   Instance Handle of PEP:Main
*
**  Return      :   TRUE                     -   Must be TRUE
*
**  Description:
*       This function creates a modal dialogbox of action code.
* ===========================================================================
*/
BOOL    PEPENTRY 
DoAction(HWND hWnd, DLGPROC lpfnProcName, WPARAM wID, HINSTANCE hPepInst)
{
    hwndActMain = hWnd;
    hPepInst    = hPepInst;

    /* ----- Create a Modal DialogBox from a DialogBox Template Resource ----- */
#if defined(POSSIBLE_DEAD_CODE)
// hActInst never used
#endif
    DialogBoxPopup(hResourceDll/*hActInst*/, MAKEINTRESOURCEW(wID), hWnd, lpfnProcName);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    PEPENTRY DoAction04()
*
**  Input       :   HWND        hWnd        -   Window Handle of PEP Window
*                   LPHANDLE    lphDlgChk1  -   Address of Modeless DialogBox
*                   LPHANDLE    lphDlgChk2  -   Address of Modeless DialogBox
*                   WORD        wID         -   KeyBoard DialogBox ID
*
**  Return      :   TRUE            -   Must be TRUE
\*
**  Description :
*       This function creates a modeless dialogbox of action code #04.
* ===========================================================================
*/
BOOL PEPENTRY   
DoAction04(HWND hWnd, LPHANDLE lphDlgChk1, LPHANDLE lphDlgChk2, WORD wID)
{
    /* ----- Set Address of Window Handle of Modeless ----- */
    /* -----    DialogBox to Public Work Area         ----- */

    lphActDlgChk1 = lphDlgChk1;
    lphActDlgChk2 = lphDlgChk2;

    hwndActMain = hWnd;

    /* ----- Determine Whether a Main DialogBox Already Exists or Not ----- */

    if (*lphDlgChk1) {          /* The Main DialogBox is Created */

        /* ----- Activate a Main DialogBox ----- */

        SetActiveWindow(*lphDlgChk1);

    } else {                    /* The Main DialogBox is Not Created */

        /* ----- Create a Main DialogBox (KeyBoard) ----- */

#if defined(POSSIBLE_DEAD_CODE)
// hActInst never used
#endif
        *lphDlgChk1 = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A04_MAIN),
                                   hWnd,
                                   A004DlgProc);

        /* ----- Create a Sub DialogBox (Tool Box) ----- */

#if defined(POSSIBLE_DEAD_CODE)
// hActInst never used
#endif
        *lphDlgChk2 = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(wID),
                                   hWnd,
                                   A004KeyDlgProc);

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

/* ===== End of File (Action.C) ===== */
