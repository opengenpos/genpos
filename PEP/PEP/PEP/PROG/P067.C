/*
* ---------------------------------------------------------------------------
* Title         :   Auto Combinational Coupon Control (Prog. 67)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P067.C
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
#include    "p067.h"

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
**  Synopsis    :   BOOL    WINAPI  P067DlgProc()
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
*              Auto COmbinational Coupon Controls (Program Mode #67)
* ===========================================================================
*/
BOOL    WINAPI  P067DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BYTE    abData[P67_ADDR_MAX];   /* Inputed Data Area            */
    UINT    unValue,                /* Inputed Data Value           */
            unMin,                  /* Minimum of Input Data        */
            unMax;                  /* Maximum of Feed Data         */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        P67InitDlg(hDlg, (LPBYTE)abData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P67_01; j<=IDD_P67_ADDRESS6_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        P67SpinProc(hDlg, wParam, lParam);

        return FALSE;
        
    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_P67_01:
        case IDD_P67_02:
		//case IDD_P67_03:
        //case IDD_P67_04:
        //case IDD_P67_05:
        //case IDD_P67_06:
		//case IDD_P67_07:
		//case IDD_P67_08:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Get Inputed Data from EditText ----- */
                unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

                /* ---- Set Maximum Value ----- */

                if (LOWORD(wParam) == IDD_P67_01) {

                    unMax = P67_DATA1_MAX;
                    unMin = P67_DATA_MIN;

                }else if(LOWORD(wParam) == IDD_P67_02) {

					unMax = P67_DATA2_MAX;
                    unMin = P67_DATA_MIN;

				}

                /* ----- Check Inputed Data with Max. Value ----- */

                if (unValue > unMax) {

                    P67DispErr(hDlg, wParam, unMin, unMax);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    if (P67SaveData(hDlg)) {

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
**  Synopsis    :   VOID    P67InitDlg()
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
VOID    P67InitDlg(HWND hDlg, LPBYTE lpbData)
{
    WORD    wEditID,
            wLength;
    USHORT  usReturnLen;
	ULONG	ulRecvFrom;

    /* ----- Read Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAAUTOCPN,
                (UCHAR *)lpbData,
                P67_ADDR_MAX,
                0, &usReturnLen);


	for (wEditID = IDD_P67_01; wEditID <= IDD_P67_06; wEditID++, lpbData+=4) {

		/*Remove check below, the Address was reserved, but now is being used
        /* ----- Deterine Whether the Address is Reserved or Not ----- */


		memcpy(&ulRecvFrom, (ULONG *)lpbData, sizeof(lpbData));
        wLength = (WORD)((wEditID == IDD_P67_01) ? P67_DATA1_LEN : P67_DATA_LEN);

        SendDlgItemMessage(hDlg, wEditID, EM_LIMITTEXT, wLength, 0L);
		
        /* ----- Set Initial Data to EditText ----- */
	
        SetDlgItemInt(hDlg, wEditID, (UINT)(ulRecvFrom), FALSE);
    }

    /* ----- Set Focus to EditText (Address No.1) ----- */

    SendDlgItemMessage(hDlg, IDD_P67_01, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P67DispErr()
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
VOID    P67DispErr(HWND hDlg, WPARAM wParam, UINT unMin, UINT unMax)
{
    WCHAR    szWork[PEP_OVER_LEN],           /* Error Message Work Area  */
            szMessage[PEP_OVER_LEN],        /* Strings for MessageBox   */
            szCaption[PEP_CAPTION_LEN];     /* MessageBox Caption       */

    /* ----- Load Strings from Resource ----- */   

    LoadString(hResourceDll, IDS_PEP_CAPTION_P67, szCaption, PEP_STRING_LEN_MAC(szCaption));

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
**  Synopsis    :   VOID    P67SpinProc()
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
VOID    P67SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN PepSpin;        /* SpinButton Style Structure   */
    int     idEdit;         /* EditText ID for SpinButton   */

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = P67_SPIN_STEP;
    PepSpin.nTurbo     = P67_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
    PepSpin.lMin       = (long)P67_DATA_MIN;

    /* ----- Get SpinButton ID ----- */

    idEdit = GetDlgCtrlID((HWND)lParam);

    /* ----- Set Max. Value of Data Range with Each Control ----- */

    switch (idEdit) {

	case IDD_P67_AUTO1:

		PepSpin.lMax = (long)P67_DATA1_MAX;
		break;
    case IDD_P67_AUTO2:

        PepSpin.lMax = (long)P67_DATA2_MAX;
        break;
/*
	case IDD_P67_AUTO3:
		
		PepSpin.lMax = (long)P67_DATA3_MAX;
		PepSpin.lMin = (long)P67_DATA_MIN;
		break;

    case IDD_P67_AUTO4:

        PepSpin.lMax = (long)P67_DATA4_MAX;
        PepSpin.lMin = (long)P67_DATA4_MIN;
        break;
*/
    default:

        PepSpin.lMax = (long)P67_DATA_MAX;
    }

    /* ----- Set EditText ID with SpinButton ID ----- */

    idEdit -= P67_AUTO_OFFSET;

    PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P67SaveData()
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
BOOL    P67SaveData(HWND hDlg)
{
    ULONG    abBuff[P67_ADDR_MAX];
    ULONG    unMax,
             unMin,
             unValue;
    WORD     wEditID,
             wI;
    BOOL     fRet = FALSE,
			 fTrans = 1;
    USHORT	 usReturnLen;
	WCHAR    szWork[16];

    /* ----- Initialize Data Buffer ----- */

    memset(abBuff, 0, sizeof(abBuff));

    for (wI = 0; wI < P67_ADDR_MAX; wI++) {

		/*Remove check below, the Address was reserved, but now is being used
        /* ----- Determine Whether Address is Reserved or Not ----- */

        /*if (wI == P67_ADDR_NOTUSE) {

            wI++;
        }*/

	 /* ----- Set ID of Target EditText ----- */

     wEditID = (WORD)(IDD_P67_01 + wI);

	 /* ----- Get Inputed Data from EditText ----- */

    DlgItemGetText(hDlg, wEditID, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Convert String Type Data to DWORD Type Data ----- */

    unValue = _wtol(szWork);

         /* ----- Set Maximum Value of Input Data ----- */

        if (wEditID == IDD_P67_01) {

            unMax = P67_DATA1_MAX;
            unMin = P67_DATA_MIN;

        } else if (wEditID == IDD_P67_02) {

			unMax = P67_DATA2_MAX;
            unMin = P67_DATA_MIN;

		}else {

            unMax = P67_DATA_MAX;
            unMin = P67_DATA_MIN;
        }

        /* ----- Determine Whether the Inputed Data is Correct or Not ----- */
                  
        if ((unValue > unMax) || (unValue < unMin) || (fTrans == 0)) {

            P67DispErr(hDlg, wEditID, unMin, unMax);

            fRet = TRUE;

            break;

        } else {                    /* Data is Correct      */

            /* ----- Set Inputed Data to Data Buffer ----- */

            abBuff[wI] = unValue;
        }
    }

    if (fRet == FALSE) {

        ParaAllWrite(CLASS_PARAAUTOCPN,
                     (UCHAR *)abBuff,
                     sizeof(abBuff),
                     0, &usReturnLen);

        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return (fRet);
}

/* ===== End of P067.C ===== */
