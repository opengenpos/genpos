/*
* ---------------------------------------------------------------------------
* Title         :   Service Time Parameter Maintenance (AC 133) 
* Category      :   Maintenance, NCR 2170 PEP for Windows (US Model)
* Program Name  :   A133.C
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
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
* Jan-30-96 : 03.01.00 : M.Suzuki   : Initial
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
#include    "a133.h"

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
**  Synopsis    :   BOOL    WINAPI  A133DlgProc()
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
*               Service Time Parameter Maintenance (Action Code # 133)
* ===========================================================================
*/
BOOL    WINAPI  A133DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;

    int     idEdit;
    WCHAR    szDesc[128],
            szWork[128],
            szCap[128];
    USHORT  usReturnLen;
    USHORT  aunData[A133_ADDR_MAX],
            unI;
    BOOL    fTrans;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Read Initial Data from Parameter File ----- */

        ParaAllRead(CLASS_PARASERVICETIME,
                    (UCHAR *)aunData,
                    sizeof(aunData),
                    0,
                    &usReturnLen);

        for (unI = 0; unI < A133_ADDR_MAX; unI++) {

            /* ----- Limit Length of Input Data to EditText ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A133_EDIT01 + unI,
                               EM_LIMITTEXT,
                               A133_DATA_LEN,
                               0L);
        
            /* ----- Set Initial Data to EditText ----- */

            SetDlgItemInt(hDlg, IDD_A133_EDIT01 + unI, aunData[unI], FALSE);

        }

        /* ----- Set Focus to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_A133_EDIT01, EM_SETSEL, 1, MAKELONG(0, -1));

        /* ----- Initialize Configulation of SpinButton ----- */

        PepSpin.lMin       = (long)A133_DATA_MIN;
        PepSpin.lMax       = (long)A133_DATA_MAX;
        PepSpin.nStep      = A133_SPIN_STEP;
        PepSpin.nTurbo     = A133_SPIN_TURBO;
        PepSpin.nTurStep   = A133_SPIN_TURSTEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */

        idEdit = GetDlgCtrlID((HWND)lParam);
        idEdit -= A133_SPIN_OFFSET;

        /* ----- Common SpinButton Procedure ----- */

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;   

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A133_EDIT01; j<=IDD_A133_CAPTION3; j++)
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

                    for (unI = 0; unI < A133_ADDR_MAX; unI++) {

                        /* ----- Get Security Number from EditText ----- */

                        aunData[unI] = GetDlgItemInt(hDlg,
                                                     IDD_A133_EDIT01 + unI,
                                                     (LPBOOL)&fTrans,
                                                     FALSE);

                        /* ----- Check Inputed Data is Valid or Not ----- */

                        if (!fTrans) {

                            /* ----- Make error message ----- */

                            LoadString(hResourceDll, IDS_PEP_CAPTION_A133, szCap, PEP_STRING_LEN_MAC(szCap));
                            LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
                            wsPepf(szDesc, szWork, A133_DATA_MIN, A133_DATA_MAX);

                            /* ----- Display Error Message ----- */

                            MessageBeep(MB_ICONEXCLAMATION);
                            MessageBoxPopUp(hDlg,
                                       szDesc,
                                       szCap,
                                       MB_ICONEXCLAMATION | MB_OK);

                            /* ----- Set Input Focus to EditText ----- */

                            SendDlgItemMessage(hDlg,
                                               IDD_A133_EDIT01 + unI,
                                               EM_SETSEL,
                                               TRUE,
                                               MAKELONG(0, -1));

                            SetFocus(GetDlgItem(hDlg, IDD_A133_EDIT01 + unI));

                            return TRUE;

                        }
                    }

                    /* ----- Write data to parameter file ----- */

                    ParaAllWrite(CLASS_PARASERVICETIME,
                                 (UCHAR *)aunData,
                                 sizeof(aunData),
                                 0,
                                 &usReturnLen);

                    /* ----- Set modifier flag ----- */

                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

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

/* ===== End of A133.C ===== */
