/*
* ---------------------------------------------------------------------------
* Title         :   Set Pig Rule Table (AC 130)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A130.C
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a130.h"

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
**  Synopsis    :   BOOL    WINAPI  A130DlgProc()
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
*                   Set Pig Rule Table (Action Code # 130)
* ===========================================================================
*/
BOOL    WINAPI  A130DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;
    int     idEdit;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Load Initial Data and Initialize DialogBox ----- */

        A130InitDlg(hDlg);

        /* ----- Initialize Commom Configulation of SpinButton ----- */

        PepSpin.lMin       = 0L;
        PepSpin.nStep      = A130_SPIN_STEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */

        idEdit = GetDlgCtrlID((HWND)lParam);
        
        idEdit -= A130_SPIN_OFFSET;

        /* ----- Set Different Configulation of SpinButton ----- */

        if (idEdit == IDD_A130_COUNTER) {

            PepSpin.lMax        = A130_COUNT_MAX;
            PepSpin.nTurbo      = 0;
            PepSpin.nTurStep    = 0;

        } else {

            PepSpin.lMax        = A130_PRICE_MAX;
            PepSpin.nTurbo      = A130_SPIN_TURBO;
            PepSpin.nTurStep    = A130_SPIN_TURSTEP;
        }

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A130_COUNTER; j<=IDD_A130_CAPTION7; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A130_ADDR1_RNG; j<=IDD_A130_ADDR6_RNG; j++)
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

                    /* ----- Get Inputed Data and Write Them to File ----- */

                    A130SaveData(hDlg);
                }

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
**  Synopsis    :   VOID    A130InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from the Parameter File, and then
*   sets loaded data to each edittext.
* ===========================================================================
*/
VOID    A130InitDlg(HWND hDlg)
{
    DWORD   dwParam[MAX_PIGRULE_SIZE];
    USHORT  usRetLen;
    WORD    wI,
            wEditID,
            wMaxLen;
    WCHAR    szWork[16];

    /* ----- Load the Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAPIGRULE,
                (UCHAR *)dwParam,
                sizeof(dwParam),
                0, &usRetLen);

    for (wI = 0, wEditID = IDD_A130_COUNTER; wI < MAX_PIGRULE_SIZE; wI++, wEditID++) {

        /* ----- Limit Maximum Length of Input Data ----- */

        wMaxLen = (WORD)((wI == A130_COUNT_ADDR) ? A130_COUNT_LEN : A130_PRICE_LEN);

        SendDlgItemMessage(hDlg, wEditID, EM_LIMITTEXT, wMaxLen, 0L);

        /* ----- Set Loaded Data to Each EditText ----- */

        wsPepf(szWork, WIDE("%lu"), dwParam[wI]);

        DlgItemRedrawText(hDlg, wEditID, szWork);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A130SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from each edittext and writes them
*   to the Parameter file.
* ===========================================================================
*/
VOID    A130SaveData(HWND hDlg)
{
    DWORD   dwParam[MAX_PIGRULE_SIZE];
    long    lWork;
    USHORT  usRetLen;
    WORD    wI,
            wEditID;
    WCHAR    szWork[16];

    for (wI = 0, wEditID = IDD_A130_COUNTER; wI < MAX_PIGRULE_SIZE; wI++, wEditID++) {

        /* ----- Get Inputed Data from EditText ----- */

        DlgItemGetText(hDlg, wEditID, szWork, sizeof(szWork));

        /* ----- Convert Text Type Date to Long Type Data ----- */

        lWork = _wtol(szWork);

        /* ----- Check Whether Inputed Data is Signed or Unsigned ----- */

        lWork = (lWork < A130_DATA_MIN) ? A130_DATA_MIN : lWork;

        /* ----- Set Inputed Value to Data Buffer ----- */

        dwParam[wI] = (DWORD)lWork;
    }

    /* ----- Write the Inputed Data to Parameter File ----- */

    ParaAllWrite(CLASS_PARAPIGRULE,
                (UCHAR *)dwParam,
                sizeof(dwParam),
                0, &usRetLen);

    /* ----- Set File Status Flag to ON ----- */

    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

}

/* ===== End of File (A130.C) ===== */
