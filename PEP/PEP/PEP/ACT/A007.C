/*
* ---------------------------------------------------------------------------
* Title         :   Cashier A/ B Key Assignment (AC 7) 
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A007.C
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
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
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
#include    "a007.h"

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
**  Synopsis    :   BOOL    WINAPI  A007DlgProc()
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
*               Cashier A / B Key Assignment (Action Code # 07)
* ===========================================================================
*/
BOOL    WINAPI  A007DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;

    int     idEdit;
    WCHAR    szDesc[128],
            szWork[128],
            szCap[128];
    USHORT  usReturnLen;
    USHORT  usJobData[MAX_AB_JOB_SIZE],
            unI;
	ULONG	alOpData[A07_ADDR_MAX];
    BOOL    fTrans;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A07_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);
        LoadString(hResourceDll, IDS_PEP_OK, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDOK, szDesc);
        LoadString(hResourceDll, IDS_PEP_CANCEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDCANCEL, szDesc);
        
        LoadString(hResourceDll, IDS_A07_AKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A07_STRAKEY, szDesc);
        LoadString(hResourceDll, IDS_A07_BKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A07_STRBKEY, szDesc);
        LoadString(hResourceDll, IDS_A07_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A07_STRDESC, szDesc);
    
        /* ----- Read Initial Data from Parameter File ----- */
        ParaAllRead(CLASS_PARACASHABASSIGN, (UCHAR *)alOpData, sizeof(alOpData), 0, &usReturnLen);

        for (unI = 0; unI < A07_ADDR_MAX; unI++) {
            /* ----- Limit Length of Input Data to EditText ----- */
            SendDlgItemMessage(hDlg, IDD_A07_AKEY + unI, EM_LIMITTEXT, A07_DATA_LEN, 0L);
        
            /* ----- Set Initial Data to EditText ----- */
            SetDlgItemInt(hDlg, IDD_A07_AKEY + unI, alOpData[unI], FALSE);
        }

		//need a read here to get job data from para
		ParaAllRead(CLASS_PARACASHABASSIGNJOB, (UCHAR *)usJobData, sizeof(usJobData), 0, &usReturnLen);

		for (unI = 0; unI < MAX_AB_JOB_SIZE; unI++) {
            /* ----- Limit Length of Input Data to EditText ----- */
			SendDlgItemMessage(hDlg, IDD_A07_CASH_A_JOBRNG1 + unI, EM_LIMITTEXT, A07_DATA_JOB_LEN, 0L);
        
            /* ----- Set Initial Data to EditText ----- */
            SetDlgItemInt(hDlg, IDD_A07_CASH_A_JOBRNG1 + unI, usJobData[unI], FALSE);
        }

        /* ----- Set Focus to EditText ----- */
        SendDlgItemMessage(hDlg, IDD_A07_AKEY, EM_SETSEL, 1, MAKELONG(0, -1));

        /* ----- Initialize Configulation of SpinButton ----- */
        PepSpin.lMin       = (long)A07_DATA_MIN;
        PepSpin.lMax       = (long)A07_DATA_MAX;
        PepSpin.nStep      = A07_SPIN_STEP;
        PepSpin.nTurbo     = A07_SPIN_TURBO;
        PepSpin.nTurStep   = A07_SPIN_TURSTEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */

        idEdit = GetDlgCtrlID((HWND)lParam);
        idEdit -= A07_SPIN_OFFSET;

        /* ----- Common SpinButton Procedure ----- */

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;   

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A07_AKEY; j<=IDD_A07_STRDESC; j++)
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

                    for (unI = 0; unI < A07_ADDR_MAX; unI++) {

                        /* ----- Get Security Number from EditText ----- */
						alOpData[unI] = GetDlgItemInt(hDlg, IDD_A07_AKEY + unI, (LPBOOL)&fTrans, FALSE);

                        /* ----- Check Inputed Data is Valid or Not ----- */
                        if (fTrans == 0) {

                            /* ----- Make error message ----- */
                            LoadString(hResourceDll, IDS_PEP_CAPTION_A07, szCap, PEP_STRING_LEN_MAC(szCap));
                            LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
                            wsPepf(szDesc, szWork, A07_DATA_MIN, A07_DATA_MAX);

                            /* ----- Display Error Message ----- */
                            MessageBeep(MB_ICONEXCLAMATION);
                            MessageBoxPopUp(hDlg, szDesc, szCap, MB_ICONEXCLAMATION | MB_OK);

                            /* ----- Set Input Focus to EditText ----- */
                            SendDlgItemMessage(hDlg, IDD_A07_AKEY + unI, EM_SETSEL, TRUE, MAKELONG(0, -1));

                            SetFocus(GetDlgItem(hDlg, IDD_A07_AKEY + unI));

                            return TRUE;
                        }
                    }

					for (unI = 0; unI < MAX_AB_JOB_SIZE; unI++)
						usJobData[unI] = GetDlgItemInt(hDlg, IDD_A07_CASH_A_JOBRNG1 + unI, (LPBOOL)&fTrans, FALSE);

                    /* ----- Write data to parameter file ----- */
                    ParaAllWrite(CLASS_PARACASHABASSIGN, (UCHAR *)alOpData, sizeof(alOpData), 0, &usReturnLen);

					//write data for Job codes
					ParaAllWrite(CLASS_PARACASHABASSIGNJOB, (UCHAR *)usJobData, sizeof(usJobData), 0, &usReturnLen);

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

/* ===== End of A007.C ===== */
