/*
* ---------------------------------------------------------------------------
* Title         :   Printer Feed Control (Prog. 18)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P018.C
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
#include    "p018.h"

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
**  Synopsis    :   BOOL    WINAPI  P018DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*               Printer Feed Control (Program Mode #18)
* ===========================================================================
*/
BOOL    WINAPI  P018DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BYTE    abData[P18_ADDR_MAX];   /* Inputed Data Area            */
    UINT    unValue,                /* Inputed Data Value           */
            unMin,                  /* Minimum of Input Data        */
            unMax;                  /* Maximum of Feed Data         */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        P18InitDlg(hDlg, (LPBYTE)abData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P18_01; j<=IDD_P18_ADDRESS6_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        P18SpinProc(hDlg, wParam, lParam);

        return FALSE;
        
    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_P18_01:
        case IDD_P18_02:
		case IDD_P18_03:
        case IDD_P18_04:
        case IDD_P18_05:
        case IDD_P18_06:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Get Inputed Data from EditText ----- */

                unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

                /* ---- Set Maximum Value ----- */

                if (LOWORD(wParam) == IDD_P18_04) {

                    unMax = P18_DATA4_MAX;
                    unMin = P18_DATA4_MIN;

                }else if(LOWORD(wParam) == IDD_P18_03) {

					unMax = P18_DATA3_MAX;
                    unMin = P18_DATA_MIN;

				}else {

                    unMax = (LOWORD(wParam) == IDD_P18_02) ? P18_DATA2_MAX:
                                                     P18_DATA_MAX;
                    unMin = P18_DATA_MIN;
                }

                /* ----- Check Inputed Data with Max. Value ----- */

                if (unValue > unMax) {

                    P18DispErr(hDlg, wParam, unMin, unMax);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    if (P18SaveData(hDlg)) {

                        return TRUE;
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
**  Synopsis    :   VOID    P18InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPBYTE  lpbData -   Address of Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This function loads initial data from Parameter file, and sets loaded
*   data to each EditText. And then it limits length of input data to EditText.
* ===========================================================================
*/
VOID    P18InitDlg(HWND hDlg, LPBYTE lpbData)
{
    WORD    wEditID,
            wLength;
    USHORT  usReturnLen;

    /* ----- Read Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARASLIPFEEDCTL,
                (UCHAR *)lpbData,
                P18_ADDR_MAX,
                0, &usReturnLen);

    for (wEditID = IDD_P18_01; wEditID <= IDD_P18_06; wEditID++, lpbData++) {

		/*Remove check below, the Address was reserved, but now is being used
        /* ----- Deterine Whether the Address is Reserved or Not ----- */

        /*if ((wEditID - IDD_P18_01) == P18_ADDR_NOTUSE) {

            wEditID++;
            lpbData++;
        }*/

        /* ----- Set Limit Length of EditText ----- */

        wLength = (WORD)((wEditID == IDD_P18_04) ? P18_DATA4_LEN : P18_DATA_LEN);

        SendDlgItemMessage(hDlg, wEditID, EM_LIMITTEXT, wLength, 0L);

        /* ----- Set Initial Data to EditText ----- */

        SetDlgItemInt(hDlg, wEditID, (UINT)*(lpbData), FALSE);
    }

    /* ----- Set Focus to EditText (Address No.1) ----- */

    SendDlgItemMessage(hDlg, IDD_P18_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P18DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Error EditText ID
*                   UINT    unMin   -   Min. Value for Data Range
*                   UINT    unMax   -   Max. Value for Data Range
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the error message from resource and displays it
*   by MessageBox. And then it sets the input focus to errored EditText.
* ===========================================================================
*/
VOID    P18DispErr(HWND hDlg, WPARAM wParam, UINT unMin, UINT unMax)
{
    WCHAR    szWork[PEP_OVER_LEN],           /* Error Message Work Area  */
            szMessage[PEP_OVER_LEN],        /* Strings for MessageBox   */
            szCaption[PEP_CAPTION_LEN];     /* MessageBox Caption       */

    /* ----- Load Strings from Resource ----- */   

    LoadString(hResourceDll, IDS_PEP_CAPTION_P18, szCaption, PEP_STRING_LEN_MAC(szCaption));

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, unMin, unMax);

    /* ----- Display Error Message by MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error EditText ----- */

    SetFocus(GetDlgItem(hDlg, LOWORD(wParam)));
    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P18SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    P18SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN PepSpin;        /* SpinButton Style Structure   */
    int     idEdit;         /* EditText ID for SpinButton   */

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = P18_SPIN_STEP;
    PepSpin.nTurbo     = P18_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
    PepSpin.lMin       = (long)P18_DATA_MIN;

    /* ----- Get SpinButton ID ----- */

    idEdit = GetDlgCtrlID((HWND)lParam);

    /* ----- Set Max. Value of Data Range with Each Control ----- */

    switch (idEdit) {

    case IDD_P18_SPIN2:

        PepSpin.lMax = (long)P18_DATA2_MAX;
        break;

	case IDD_P18_SPIN3:
		
		PepSpin.lMax = (long)P18_DATA3_MAX;
		PepSpin.lMin = (long)P18_DATA_MIN;
		break;

    case IDD_P18_SPIN4:

        PepSpin.lMax = (long)P18_DATA4_MAX;
        PepSpin.lMin = (long)P18_DATA4_MIN;
        break;

    default:

        PepSpin.lMax = (long)P18_DATA_MAX;
    }

    /* ----- Set EditText ID with SpinButton ID ----- */

    idEdit -= P18_SPIN_OFFSET;

    PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P18SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE    -   Inputed Data is out of range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets the inputed data from each EditText, and then it
*   checks whether the data is correct or not. It sets the data to Parameter
*   file and then returns FALSE, if the data is correct. Otherwise it returns
*   TRUE.
* ===========================================================================
*/
BOOL    P18SaveData(HWND hDlg)
{
    BYTE    abBuff[P18_ADDR_MAX];
    UINT    unMax,
            unMin,
            unValue;
    WORD    wEditID,
            wI;
    BOOL    fRet = FALSE,
            fTrans;
    USHORT  usReturnLen;

    /* ----- Initialize Data Buffer ----- */

    memset(abBuff, 0, sizeof(abBuff));

    for (wI = 0; wI < P18_ADDR_MAX; wI++) {

		/*Remove check below, the Address was reserved, but now is being used
        /* ----- Determine Whether Address is Reserved or Not ----- */

        /*if (wI == P18_ADDR_NOTUSE) {

            wI++;
        }*/

        /* ----- Set ID of Target EditText ----- */

        wEditID = (WORD)(IDD_P18_01 + wI);

        /* ----- Get Inputed Data from Target EditText ----- */

        unValue = GetDlgItemInt(hDlg, wEditID, (LPBOOL)&fTrans, FALSE);

        /* ----- Set Maximum Value of Input Data ----- */

        if (wEditID == IDD_P18_04) {

            unMax = P18_DATA4_MAX;
            unMin = P18_DATA4_MIN;

        } else if (wEditID == IDD_P18_03) {

			unMax = P18_DATA3_MAX;
            unMin = P18_DATA_MIN;

		}else {

            unMax = (wEditID == IDD_P18_02) ? P18_DATA2_MAX : P18_DATA_MAX;
            unMin = P18_DATA_MIN;
        }

        /* ----- Determine Whether the Inputed Data is Correct or Not ----- */
                  
        if ((unValue > unMax) || (unValue < unMin) || (fTrans == 0)) {

            P18DispErr(hDlg, wEditID, unMin, unMax);

            fRet = TRUE;

            break;

        } else {                    /* Data is Correct      */

            /* ----- Set Inputed Data to Data Buffer ----- */

            abBuff[wI] = (BYTE)unValue;
        }
    }

    if (fRet == FALSE) {

        ParaAllWrite(CLASS_PARASLIPFEEDCTL,
                     (UCHAR *)abBuff,
                     sizeof(abBuff),
                     0, &usReturnLen);

        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return (fRet);
}

/* ===== End of P018.C ===== */
