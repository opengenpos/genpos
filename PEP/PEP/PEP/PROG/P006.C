/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Security Code (Prog. 6) 
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P006.C
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

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p006.h"


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
**  Synopsis    :   BOOL    WINAPI  P006DlgProc()
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
*           Program Mode Security Code (Program Mode # 06)
* ===========================================================================
*/
BOOL    WINAPI  P006DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;    /* SpinButton Configulation         */
    USHORT    unData;             /* Data Aera for P006 Security No.  */
    USHORT  usReturnLen;        /* Return Length of ParaAll         */
    BOOL    fTrans;

    switch (wMsg) {

    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Limit Maximum Input Length to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_P06_EDIT, EM_LIMITTEXT, P06_DATA_LEN, 0L);
        
        /* ----- Read Initial Data from Parameter File ----- */

        ParaAllRead(CLASS_PARASECURITYNO,
                    (UCHAR *)&unData,
                    sizeof(USHORT),
                    0, &usReturnLen);

        /* ----- Set Initial Security Code to EditText ----- */

        SetDlgItemInt(hDlg, IDD_P06_EDIT, unData, FALSE);

        /* ----- Set Input Focus to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_P06_EDIT, EM_SETSEL, 1, MAKELONG(0, -1));

        /* ----- Initialize Configulation of SpinButton ----- */

        PepSpin.lMin       = 0L;
        PepSpin.lMax       = (long)P06_DATA_MAX;
        PepSpin.nStep      = P06_SPIN_STEP;
        PepSpin.nTurbo     = P06_SPIN_TURBO;
        PepSpin.nTurStep   = P06_SPIN_TURSTEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

    case WM_VSCROLL:

        /* ----- Common SpinButton Procedure (See => Pepcomm.c) ----- */

        PepSpinProc(hDlg, wParam, IDD_P06_EDIT, (LPPEPSPIN)&PepSpin);

        /* ----- An application should return zero   ----- */
        /* ----- if it processes WM_VSCROLL message. ----- */

        return FALSE;   

    case WM_SETFONT:
	
		SendDlgItemMessage(hDlg, IDD_P06_SECURITY_CODE, WM_SETFONT, (WPARAM)hResourceFont, 0);
		SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
		SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		SendDlgItemMessage(hDlg, IDD_P06_RNG, WM_SETFONT, (WPARAM)hResourceFont, 0);
		SendDlgItemMessage(hDlg, IDD_P06_EDIT, WM_SETFONT, (WPARAM)hResourceFont, 0);

		return TRUE;

	case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Security Code from EditText ----- */

                    unData = GetDlgItemInt(hDlg,
                                           IDD_P06_EDIT,
                                           (LPBOOL)&fTrans,
                                           FALSE);

                    /* ----- Check Inputed Data ----- */

                    if (fTrans == 0) {

                        /* ----- Display Error Message ----- */

                        P06DispError(hDlg);

                        return TRUE;
                        
                    } else {

                        ParaAllWrite(CLASS_PARASECURITYNO,
                                     (UCHAR *)&unData,
                                     sizeof(USHORT),
                                     0, &usReturnLen);

                        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
                    }
                }

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
**  Synopsis    :   VOID    P06DispError()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This function displays error message, and beeps error tone.
* ===========================================================================
*/
VOID    P06DispError(HWND hDlg)
{
    WCHAR    szCaption[64],
            szErrMsg[128],
            szWork[128];

    /* ----- Load Caption and Error Message ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_P06, szCaption, PEP_STRING_LEN_MAC(szCaption));

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Make Error Message with Data Range ----- */

    wsPepf(szErrMsg, szWork, P06_DATA_MIN, P06_DATA_MAX);

    /* ----- Display Error Message ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szErrMsg,
               szCaption,
               MB_ICONEXCLAMATION | MB_OK);

    SendDlgItemMessage(hDlg, IDD_P06_EDIT, EM_SETSEL, 1, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, IDD_P06_EDIT));
}

/* ===== End of P006.C ===== */
