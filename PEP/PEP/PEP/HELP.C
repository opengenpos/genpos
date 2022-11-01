/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP (Parameter Entry Program) Help Procedure
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   HELP.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /c /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
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
#include	<Windows.h>
#include	"AW_Interfaces.h"
#include    <ecr.h>

#include	"pep.h"
#include	"pepcomm.h"
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
**  Synopsis    :   BOOL    WINAPI  DoHelp()
*
**  Input       :   HWND    hWnd            -   Window Handle of PEP Window
*                   FARPROC lpfnProcName    -   Address of Procedure Name
*                   WORD    wID             -   ID of DialogBox Template Name
*
**  Return      :   TRUE                    -   Must be TRUE
*
**  Description:
*       This function creates a modal dialogbox of help mode.
* ===========================================================================
*/
BOOL    WINAPI  DoHelp(HWND hWnd, DLGPROC lpfnProcName, WORD wID)
{
    DLGPROC lpfnDlgProc;

    /* ----- Make the Address of the Prolog Code for an Exported Function ----- */

    lpfnDlgProc = MakeProcInstance(lpfnProcName, hPepInst);

    /* ----- Create a Modal DialogBox from a DialogBox Template Resource ----- */

    DialogBoxPopup(hResourceDll/*hPepInst*/, MAKEINTRESOURCEW(wID), hWnd, lpfnDlgProc);

    /* ----- Free the Function from the Data Segment ----- */

    FreeProcInstance(lpfnDlgProc);

    return TRUE;
}

/* ===== End of File (HELP.C) ===== */
