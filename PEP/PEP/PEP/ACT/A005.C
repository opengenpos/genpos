/*
* ---------------------------------------------------------------------------
* Title         :   Set Menu Page Control Table (AC 5)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A005.C
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
* Dec-09-99 :          : K.Yanagida : NCR2172
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
#include    "a005.h"

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
**  Synopsis    :   BOOL    WINAPI  A005DlgProc()
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
*               Set Menu Page Control Table (Action Code # 05)
* ===========================================================================
*/
BOOL    WINAPI  A005DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize the Configulation of DialogBox ----- */

        A005InitDlg(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A005_OFFDEF; j<=IDD_A005_DESC11; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

/*** NCR2172 ***/

    case WM_VSCROLL:

        /* ----- Spin Button Procedure ----- */
        A005SpinProc(hDlg, wParam, lParam);

        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

/*** NCR2172 ***/
    
        case IDD_A005_ADJED01:
        case IDD_A005_ADJED02:
        case IDD_A005_ADJED03:
        case IDD_A005_ADJED04:
        case IDD_A005_ADJED05:
        case IDD_A005_ADJED06:
        case IDD_A005_ADJED07:
        case IDD_A005_ADJED08:
        case IDD_A005_ADJED09:

            if (HIWORD(wParam) == EN_CHANGE)
            {
 
               /* ----- Check Inputed Data with Data Range ----- */

               if (A005ChkPos(hDlg, LOWORD(wParam))) {

                   /* ----- Display Error Message ----- */

                   A005DispErr(hDlg, LOWORD(wParam));
                }
               return TRUE;
            }
            
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    A005SaveData(hDlg);
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
**  Synopsis    :   VOID    A005InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from Parameter file, and then it
*   sets them to each controls.
* ===========================================================================
*/
VOID    A005InitDlg(HWND hDlg)
{
    BYTE    abData[A005_ADDR_MAX];
    WORD    wI,
            wDefID,
            wNotID = IDD_A005_NOT01;
    USHORT  usRet;
    WCHAR    szDesc[PEP_LOADSTRING_LEN + 1];

    UINT   unValue;

    /* ----- set initial description, V3.3 ----- */
    LoadString(hResourceDll, IDS_A005_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
    WindowRedrawText(hDlg, szDesc);
 
    LoadString(hResourceDll, IDS_A005_OFFDEF, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_OFFDEF, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF01, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF01, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF02, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF02, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF03, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF03, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF04, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF04, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF05, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF05, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF06, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF06, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF07, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF07, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF08, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF08, szDesc);
    LoadString(hResourceDll, IDS_A005_DEF09, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DEF09, szDesc);

    LoadString(hResourceDll, IDS_A005_OFFNOT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_OFFNOT, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT01, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT01, szDesc);    
    LoadString(hResourceDll, IDS_A005_NOT02, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT02, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT03, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT03, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT04, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT04, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT05, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT05, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT06, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT06, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT07, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT07, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT08, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT08, szDesc);
    LoadString(hResourceDll, IDS_A005_NOT09, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_NOT09, szDesc);

/*** NCR2172 ***/
    LoadString(hResourceDll, IDS_A005_ADJGRP, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_ADJGRP, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC01, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC01, szDesc);    
    LoadString(hResourceDll, IDS_A005_DESC02, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC02, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC03, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC03, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC04, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC04, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC05, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC05, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC06, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC06, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC07, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC07, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC08, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC08, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC09, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC09, szDesc);
    LoadString(hResourceDll, IDS_A005_DESC10, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A005_DESC10, szDesc);

    /* ----- Read Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARACTLTBLMENU,
                (UCHAR *)abData,
                sizeof(abData),
                0, &usRet);

    /* ----- Set Initial Data (Default Page No.) ----- */

    if (abData[A005_ADDR_DEF]) {

        wDefID = (WORD)(IDD_A005_OFFDEF + abData[A005_ADDR_DEF]);

    } else {

        wDefID = IDD_A005_DEF01;
    }
/* Saratoga 
    CheckRadioButton(hDlg, IDD_A005_DEF01, IDD_A005_DEF05, wDefID);
*/
    CheckRadioButton(hDlg, IDD_A005_DEF01, IDD_A005_DEF09, wDefID);

    /* ----- Set Initial Data (Use / Not Use Menu Page) ----- */

    for (wI = A005_PAGE_MIN; wI <= A005_PAGE_MAX; wI++, wNotID++) {

        CheckDlgButton(hDlg, wNotID, (UINT)abData[wI]);
    }

/*** NCR2172 ***/

    /* ----- Set Initial Data (Default Page No.) ----- */

    for (wI = 0; wI < A05_OFFSET - 1; wI++) {

        /* ----- Limit Length of Input Data to EditText ----- */
 
        SendDlgItemMessage(hDlg,
                           IDD_A005_ADJED01 + wI,
                           EM_LIMITTEXT,
                           A05_MAXLEN,
                           0L);

        /* ----- Set Adjective Data to Specified EditText ----- */

        unValue = (UINT)abData[A05_OFFSET + wI];


        SetDlgItemInt(hDlg,
                      IDD_A005_ADJED01 + wI,
                      (WORD)unValue,
                      FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A005SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from each controls, and then it 
*   writes them to Parameter file.
* ===========================================================================
*/
VOID    A005SaveData(HWND hDlg)
{
    BYTE    abData[A005_ADDR_MAX];
    WORD    wI,
            wNotID = IDD_A005_NOT01,
            wDefID = IDD_A005_DEF01;
    USHORT  usRet;

    UINT    unAdj;  /* NCR2172 */
    BOOL    fAdj;   /* NCR2172 */
    
	memset(abData, 0x00, sizeof( abData));
    for (wI = A005_PAGE_MIN; wI <= A005_PAGE_MAX; wI++, wNotID++, wDefID++) {

        /* ----- Check Whether the Target RadioButton is Checked or Not ----- */

        if (IsDlgButtonChecked(hDlg, wDefID)) {

            /* ----- Set the Default Menu Page No. to Buffer ----- */

            abData[A005_ADDR_DEF] = (BYTE)wI;
        }

        /* ----- Set the Use/Not Use Menu Page to Buffer ----- */

        abData[wI] = (BYTE)IsDlgButtonChecked(hDlg, wNotID);
    }

/*** NCR2172 Start ***/
    for (wI = 0; wI < A05_OFFSET - 1; wI++) {

        /* ----- Get Data from EditText ----- */

        unAdj = GetDlgItemInt(hDlg, IDD_A005_ADJED01 + wI, (LPBOOL)&fAdj, FALSE);

        if(fAdj == 0 && (wI < A05_OFFSET - 1)) {

            /* ----- Display Error Message ----- */

            A005DispErr(hDlg, (WORD)(IDD_A005_ADJED01 + wI));

            return;

        } else {

            /* ----- Set (Date) Data to Data Buffer ----- */

            abData[A05_OFFSET + wI] = (UCHAR)unAdj;

        }

    }

    /* ----- Write the Inputed Data to Parameter File ----- */

    ParaAllWrite(CLASS_PARACTLTBLMENU,
                 (UCHAR *)abData,
                 sizeof(abData),
                 0, &usRet);

    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A005ChkPos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed adjective position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A005ChkPos(HWND hDlg, WORD wItem)
{
    BOOL    fCheck = FALSE;
    UINT   nPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */

    nPos = GetDlgItemInt(hDlg, wItem, NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */

        switch (wItem) {
        
        case    IDD_A005_ADJED01:
        case    IDD_A005_ADJED02:
        case    IDD_A005_ADJED03:
        case    IDD_A005_ADJED04:
        case    IDD_A005_ADJED05:
        case    IDD_A005_ADJED06:
        case    IDD_A005_ADJED07:
        case    IDD_A005_ADJED08:
        case    IDD_A005_ADJED09:
            if ((nPos < A05_DATA_MIN) || (nPos > A05_DATA_MAX))
            {

                 fCheck = TRUE;
            }

        default:
            break;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A005DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*   MessageBox.
* ===========================================================================
*/
VOID    A005DispErr(HWND hDlg, WORD wItem)
{
    WORD    wMin,
            wMax;
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_LOADSTRING_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load String from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A05, szCaption, PEP_CAPTION_LEN);

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Make Error Message ----- */

    wMin = (WORD)A05_DATA_MIN;
    wMax = (WORD)A05_DATA_MAX;

    wsPepf(szMessage, szWork, wMin, wMax);

    /* ----- Display Error Message with MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
	
	SetDlgItemInt(hDlg, wItem, wMin, FALSE);

    /* ----- Set Focus to Error Occurred EditText ----- */

    SendDlgItemMessage(hDlg, wItem, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wItem));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A005SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WPARAM  wParam  -   Specifies a Scroll Bar Code
*                   LPARAM  lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No Return Value.
*
**  Description:
*       This function processes WM_VSCROLL message.
* ===========================================================================
*/
VOID    A005SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    int         nEdit;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = A05_SPIN_STEP;
    PepSpin.nTurbo     = A05_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */
    nEdit = GetDlgCtrlID((HWND)lParam) - A005_SPIN_OFFSET;

    /* ----- Set minimum and maximum ----- */

    switch (nEdit) {

        case    IDD_A005_ADJED01:
        case    IDD_A005_ADJED02:
        case    IDD_A005_ADJED03:
        case    IDD_A005_ADJED04:
        case    IDD_A005_ADJED05:
        case    IDD_A005_ADJED06:
        case    IDD_A005_ADJED07:
        case    IDD_A005_ADJED08:
        case    IDD_A005_ADJED09:

        PepSpin.lMax = (long)A05_DATA_MAX;

        PepSpin.lMin = (long)A05_DATA_MIN;

        break;

    default:

        break;
    }

    /* ----- SpinButton Procedure ----- */

    PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);
}

/* ===== End of File (A005.C) ===== */
