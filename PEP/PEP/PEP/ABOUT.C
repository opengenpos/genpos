/*
* ---------------------------------------------------------------------------
* Title         :   About Parameter Entry Program
* Category      :   Help, NCR 2170 PEP for Windows  (Hotel US Model)
* Program Name  :   ABOUT.C
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
* Dec-22-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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

#include    "pep.h"
#include	"pepcomm.h"
#include    "about.h"

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
**  Synopsis    :   BOOL    WINAPI  AboutDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of About DialogBox
*                   WORD    wMsg    -   Dispatched Message
*                   WORD    wParam  -   16 Bits Message Parameter
*                   LONG    lParam  -   32 Bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description :
*       This is a dialog procedure of About PEP.
* ===========================================================================
*/
BOOL    WINAPI  AboutDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static HICON hiconPep[ 3 ];
    static UINT  idIcon = 0;
    UINT    idTimer = 1;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Set the Input Focus to OK Button ----- */

        SetFocus(GetDlgItem(hDlg, IDOK));

        hiconPep[ 0 ] = LoadIcon(hPepInst, MAKEINTRESOURCE(IDI_PEPICON1));
        hiconPep[ 1 ] = LoadIcon(hPepInst, MAKEINTRESOURCE(IDI_PEPICON2));
        hiconPep[ 2 ] = LoadIcon(hPepInst, MAKEINTRESOURCE(IDI_PEPICON3));

        SendDlgItemMessage( hDlg,
                            IDD_HLP_IDX,
                            STM_SETICON,
                            (WPARAM)hiconPep[ 0 ],
                            0L);

        SetTimer( hDlg, idTimer, 3000, NULL );

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_HLP_CAPTION1; j<=IDD_HLP_CAPTION6; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_TIMER:

        idIcon++;
        if ( 2 < idIcon ) {
            idIcon = 0;
        }

        SendDlgItemMessage( hDlg,
                            IDD_HLP_IDX,
                            STM_SETICON,
                            (WPARAM)hiconPep[ idIcon ],
                            0L);
        return FALSE;

    case WM_DESTROY:

        KillTimer( hDlg, idTimer );
        DestroyIcon( hiconPep[ 0 ] );
        DestroyIcon( hiconPep[ 1 ] );
        DestroyIcon( hiconPep[ 2 ] );

        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case    IDCANCEL:
        case    IDOK:
                /* ----- Destroy DialogBox ----- */

               EndDialog(hDlg, LOWORD(wParam));

               return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/* ===== End of File (ABOUT.C) ===== */
