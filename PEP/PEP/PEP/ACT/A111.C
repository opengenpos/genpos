/*
* ---------------------------------------------------------------------------
* Title         :   Set Tare Table (AC 111)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A111.STR
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
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
#include    "a111.h"

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
static  short   anTare[A111_ADDR_MAX];          /* Tare Data Buffer     */
static  PARAMDC mdcData;                        /* MDC Data Structure   */
static  char    chErrType,                      /* Error Type           */
                achErrData[A111_ADDR_MAX + 1];  /* Error Data Address   */
static  UINT    afuStyle[A111_ERR] = {          /* MessageBox Style     */
                    MB_OK | MB_ICONEXCLAMATION,
                    MB_OKCANCEL | MB_ICONEXCLAMATION
                };


static  HWND  hWndModeLess = NULL;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis    :   VOID    A111ModifyError()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function modifys the tare data with user selection on error
*   dialogbox.
* ===========================================================================
*/
static VOID    A111ModifyError(HWND hDlg)
{
    WCHAR    chWork;

    if (IsDlgButtonChecked(hDlg, IDD_A111EB1)) {
        for (chWork = 0; chWork < A111_ADDR_MAX; chWork++) {
            /* ----- Initialize Error Data ----- */
            anTare[achErrData[chWork]] = A111_INITIAL;
            /* ----- Exit Roop, If Not Exists Error Data ----- */
            if (achErrData[chWork + 1] == A111_ERR_INI) {
                break;
            }
        }
    } else {
        /* ---- Least Digit Illegal Error Exist ----- */
        if (chErrType & A111_MDC_B) {
            /* ----- Reset MDC Bit-B Data (OFF[0, 5] >> ON[0 - 9]) ----- */
            mdcData.uchMDCData |= A111_MDC_B;
        }

        /* ---- Data Range Over Error Exist ----- */
        if (chErrType & A111_MDC_D) {
            /* ----- Reset MDC Bit-D Data ( OFF [999] >> ON [9999]) ----- */
            mdcData.uchMDCData |= A111_MDC_D;
        }

        /* ----- Write MDC Data ----- */
        ParaMDCWrite(&mdcData);

        /* ----- Set Para File Modify Flag ----- */
        PepSetModFlag(hwndActMain, PEP_MF_PROG, 0);
    }
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A111DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description:
*       This is a dialogbox procedure for
*                  Set Tare Table (Action Code # 111)
* ===========================================================================
*/
BOOL    WINAPI  A111DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  WPARAM  wPrevID;    /* Previous Selected EditText ID    */
    static  WORD    wWidth,     /* Initial Width of DialogBox       */
                    wHeight;    /* Initial Height of DialogBox      */

    int     nRet = TRUE;        /* Return Value of Error Dialog */
    RECT    rectDlg;            /* Client Area of DialogBox     */
    WORD    wID,                /* Check Button ID Number       */
            wLimit,             /* Limit Length of Edittext     */
            wDlgWidth;          /* Width of DialogBox           */
    BOOL    fStatus1,           /* MDC Control Status Flag      */
            fStatus2,           /* MDC Control Status Flag      */
            fStatus3;           /* MDC Control Status Flag      */
    USHORT  usRetLen;           /* Return Length of ParaAll     */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Read Initial Tare Data from Parameter File ----- */

        ParaAllRead(CLASS_PARATARE,
                    (UCHAR *)anTare, sizeof(anTare), 0, &usRetLen);

        /* ----- Read Current Tax Mode (Prog 1, Addr 29) ----- */

        mdcData.usAddress = A111_MDC_ADDR;

        ParaMDCRead(&mdcData);

        /* ----- Check Data in Buffer with Length & Least Digit ----- */

        if (A111CheckData(hDlg, A111_BUFFER)) {

            /* ----- Create Modal-DialogBox ----- */

            nRet = DialogBoxPopup(hResourceDll,
                             MAKEINTRESOURCEW(IDD_A111_ERR),
                             hDlg,
                             (DLGPROC)A111ErrorDlgProc);
        }

        if (nRet == TRUE) {         /* User Selects OK Button       */

            /* ----- Initialize DialogBox ----- */

            A111InitDlg(hDlg);

            /* ----- Get Client Area of DialogBox ----- */

            GetWindowRect(hDlg, (LPRECT)&rectDlg);

            wWidth  = (WORD)(rectDlg.right  - rectDlg.left);
            wHeight = (WORD)(rectDlg.bottom - rectDlg.top);

            /* ----- Hide MDC RadioButton ----- */

            wDlgWidth = (WORD)(wWidth * A111_MULTIPLY / A111_DIVIDE);

            SetWindowPos(hDlg, (HWND)NULL, 0, 0, wDlgWidth, wHeight,
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        } else {                    /* User Selects CANCEL Button   */

            EndDialog(hDlg, LOWORD(IDCANCEL));
        }
        return TRUE;

    case WM_VSCROLL:

        /* ----- Spin Button Control ----- */

        A111SpinProc(hDlg, wParam, lParam);

        return FALSE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A111_EDIT01; j<=IDD_A111_TARE10; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A111CLOSE:

            /* ----- Check Data in EditText ----- */

            if (A111CheckData(hDlg, A111_EDITTEXT)) {

                A111SetError(hDlg, A111_OK, LOWORD(wParam));

                /* ----- Set HighLight Bar to Error Data ----- */

                wID = (WORD)(IDD_A111_EDIT01 + achErrData[A111_INITIAL]);
                wPrevID = wID;
                SetFocus(GetDlgItem(hDlg, wID));
                SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));

                return TRUE;
            }

            /* ----- return; ----- Not Used */

        case IDD_A111EXTEND:

            /* ----- Change DialogBox Size ----- */

            wDlgWidth = (WORD)((LOWORD(wParam) == IDD_A111EXTEND) ?
                        wWidth : (wWidth * A111_MULTIPLY / A111_DIVIDE));

            SetWindowPos(hDlg, (HWND)NULL, 0, 0, wDlgWidth, wHeight,
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

            /* ----- Get PushButton / RadioButton Status ----- */

            fStatus1 = (LOWORD(wParam) == IDD_A111EXTEND) ? FALSE : TRUE;
            fStatus2 = (LOWORD(wParam) == IDD_A111EXTEND) ? TRUE  : FALSE;

            if (wParam == IDD_A111EXTEND) {

                fStatus3 =  (mdcData.uchMDCData & A111_MDC_D) ? TRUE : FALSE;

            } else {

                fStatus3 =  FALSE;
            }

            /* ----- Set Each Control Enable/Disable ----- */

            EnableWindow(GetDlgItem(hDlg, IDD_A111EXTEND), fStatus1);
            EnableWindow(GetDlgItem(hDlg, IDD_A111CLOSE),  fStatus2);

            EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_B),fStatus3);
            EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_B), fStatus3);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB1),    fStatus3);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB2),    fStatus3);

            EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_D),fStatus2);
            EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_D), fStatus2);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB3),    fStatus2);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB4),    fStatus2);

            /* ----- Get ID to Set Focus ----- */

            wID = (WORD)((LOWORD(wParam) == IDD_A111EXTEND) ?  IDD_A111CLOSE :
                                                       IDD_A111EXTEND);

            /* ----- Set Focus to PushButton ----- */

            SetFocus(GetDlgItem(hDlg, wID));

            return TRUE;

        case IDD_A111_EDIT01:
        case IDD_A111_EDIT02:
        case IDD_A111_EDIT03:
        case IDD_A111_EDIT04:
        case IDD_A111_EDIT05:
        case IDD_A111_EDIT06:
        case IDD_A111_EDIT07:
        case IDD_A111_EDIT08:
        case IDD_A111_EDIT09:
        case IDD_A111_EDIT10:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Bit-B is OFF [0 or 5], and Bit-D is ON [0 - 9999] ----- */

                if ((mdcData.uchMDCData & A111_MDC_D) &&
                    (! (mdcData.uchMDCData & A111_MDC_B))) {

                    /* ----- Check Least Digit with MDC Bit-B ----- */

                    A111CheckEdit(hDlg, LOWORD(wParam));
                }
                return TRUE;

            } else if ((HIWORD(wParam) == EN_SETFOCUS) && (wPrevID != LOWORD(wParam))) {

                wPrevID = LOWORD(wParam);

                /* ----- Check Data in EditText ----- */

                if (A111CheckData(hDlg, A111_EDITTEXT)) {

                    A111SetError(hDlg, A111_OK, LOWORD(wParam));

                    /* ----- Set HighLight Bar to Error Data ----- */

                    wID = (WORD)(IDD_A111_EDIT01 + achErrData[A111_INITIAL]);
                    wPrevID = wID;
                    SetFocus(GetDlgItem(hDlg, wID));
                    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A111RB1:
        case IDD_A111RB2:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDD_A111RB2) {    /* Bit-B is ON (0 - 9) */

                    /* ----- Change MDC Data (Addr.29 Bit-B >> ON) -----*/

                    mdcData.uchMDCData |= A111_MDC_B;

                } else {                        /* Bit-B is OFF (0 or 5) */

                    /* ----- Change MDC Data (Addr.29 Bit-B >> OFF) -----*/

                    mdcData.uchMDCData &= (~A111_MDC_B);
                }

                /* ----- Set Check RadioButton ----- */

                CheckRadioButton(hDlg, IDD_A111RB1, IDD_A111RB2, LOWORD(wParam));

                /* ----- Check Data in EditText & Get Count of Error Data ----- */

                if (A111CheckData(hDlg, A111_EDITTEXT)) {

                    /* ----- Make Error Message & Reset Error Data ----- */

                    A111SetError(hDlg, A111_OKCANCEL, LOWORD(wParam));
                }

                /* ----- Set Condition String to StaticText ----- */

                A111SetCondition(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDD_A111RB3:
        case IDD_A111RB4:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDD_A111RB3) {    /* Bit-D is OFF (0 - 999) */

                    /* ----- Change MDC Data (Addr.29 Bit-D >> OFF) -----*/

                    mdcData.uchMDCData &= (~A111_MDC_D);

                    /* ----- Disable Bit-B RadioButton ----- */

                    fStatus1 = FALSE;

                } else {

                    /* ----- Change MDC Data (Addr.29 Bit-D >> ON) -----*/

                    mdcData.uchMDCData |= A111_MDC_D;

                    /* ----- Enable Bit-B RadioButton ----- */

                    fStatus1 = TRUE;
                }

                /* ----- Enable/Disable MDC-B RadioButton ----- */

                EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_B), fStatus1);
                EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_B),  fStatus1);
                EnableWindow(GetDlgItem(hDlg, IDD_A111RB1),     fStatus1);
                EnableWindow(GetDlgItem(hDlg, IDD_A111RB2),     fStatus1);

                /* ----- Set Check RadioButton ----- */

                CheckRadioButton(hDlg, IDD_A111RB3, IDD_A111RB4, wParam);

                /* ----- Check Data in EditText ----- */

                if (A111CheckData(hDlg, A111_EDITTEXT)) {

                    /* ----- Make Error Message & Reset Error Data ----- */

                    A111SetError(hDlg, A111_OKCANCEL, LOWORD(wParam));
                }

                /* ----- Set Condition String to StaticText ----- */

                A111SetCondition(hDlg);

                wLimit = (WORD)((mdcData.uchMDCData & A111_MDC_D) ?
                         A111_DON_LEN : A111_DOFF_LEN);

                /* ----- Set Limit Length to EditText ----- */

                for (wID = IDD_A111_EDIT01; wID <= IDD_A111_EDIT10; wID++) {

                    SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, wLimit, 0L);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:

            /* ----- Check Data in EditText ----- */

            if (A111CheckData(hDlg, A111_EDITTEXT)) {

                A111SetError(hDlg, A111_OK, LOWORD(wParam));

                /* ----- Set HighLight Bar to Error Data ----- */

                wID = (WORD)(IDD_A111_EDIT01 + achErrData[A111_INITIAL]);
                wPrevID = wID;
                SetFocus(GetDlgItem(hDlg, wID));
                SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));

                return  TRUE;

            } else {                    /* If Error Data Not Exist */

                /* ----- Write Tare Data ----- */

                ParaAllWrite(CLASS_PARATARE,
                             (UCHAR *)anTare, sizeof(anTare), 0, &usRetLen);

                ParaMDCWrite(&mdcData);

                PepSetModFlag(hwndActMain, (PEP_MF_PROG | PEP_MF_ACT), 0);
            }

            /* ----- return; ----- not use */

        case IDCANCEL:

            EndDialog(hDlg, LOWORD(wParam));

            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A111ErrorDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description:
*       This is a dialogbox procedure for
*                  Error Correct DialogBox (Action Code # 111)
* ===========================================================================
*/
BOOL WINAPI A111ErrorDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int nRet = FALSE;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        MessageBeep(MB_ICONEXCLAMATION);

        CheckDlgButton(hDlg, IDD_A111EB1, TRUE);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A111_CAPTION1; j<=IDD_A111_GROUPBOX; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_A111EB1, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A111EB2, WM_SETFONT, (WPARAM)hResourceFont, 0);
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

                    A111ModifyError(hDlg);

                    nRet = TRUE;
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
**  Synopsis    :   VOID    A111InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function initializes the configulation of a dialogbox.
* ===========================================================================
*/
VOID    A111InitDlg(HWND hDlg) 
{
   // LPUINT  lpunTare = (LPUINT)anTare; //SR#125
    WORD    wLimit,
            wID;
	int		i = 0;  //SR#125

    /* ----- Set Maximum Length of Tare Data ----- */

    wLimit = (WORD)((mdcData.uchMDCData & A111_MDC_D) ?
             A111_DON_LEN : A111_DOFF_LEN);

    for (wID = IDD_A111_EDIT01; wID <= IDD_A111_EDIT30; wID++, i++/*lpunTare++*/) {

               /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wID);

        PepSetFont(hDlg, wID);
		/* ----- Limit Input Length to EditText ----- */

        SendDlgItemMessage(hDlg, wID,  EM_LIMITTEXT, wLimit, 0L);

        /* ----- Set Initial Tare Data to EditText ----- */

        SetDlgItemInt(hDlg, wID, (UINT)anTare[i], FALSE);
    }

    /* ----- Check RadioButton (Bit-B : IDD_A111RB1 or IDD_A111RB2) -----*/

    wID = (WORD)((mdcData.uchMDCData & A111_MDC_B) ? IDD_A111RB2 :
                                                     IDD_A111RB1);
    CheckRadioButton(hDlg, IDD_A111RB1, IDD_A111RB2, wID);

    /* ----- Check RadioButton (Bit-D : IDD_A111RB3 or IDD_A111RB4) -----*/

    wID = (WORD)((mdcData.uchMDCData & A111_MDC_D) ? IDD_A111RB4 :
                                                     IDD_A111RB3);
    CheckRadioButton(hDlg, IDD_A111RB3, IDD_A111RB4, wID);

    /* ----- Disable RadioButton & Close Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A111CLOSE),   FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_B), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_B),  FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_D), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_D),  FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111RB1),     FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111RB2),     FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111RB3),     FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A111RB4),     FALSE);

    /* ----- Set Condition String to StaticText ----- */

    A111SetCondition(hDlg);
}

/*
* ===========================================================================
**  Synopsis    :   char    A111CheckData()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                                       (Use only fType = A111_EDITTEXT)
*                   BOOL    fType   -   Data Check Type
*                                       (Check Buffer   = A111_BUFFER) or
*                                       (Check EditText = A111_EDITTEXT)
*
**  Return      :   char    chCount -   Number of Error Data
*
**  Description :
*       This function checks a current tare data with the maximum value and
*   the least significant digit. It checks a tare data in data buffer, if
*   fType parameter is A111_BUFFER. Otherwise it checks a data in edittext.
* ===========================================================================
*/
WCHAR    A111CheckData(HWND hDlg, BOOL fType)
{
    short   nCount;             /* Counter              */
    WCHAR    chCount,            /* Number of Error Data */
            chLeast,            /* Flag (Bit-B: ON = 1) */
            chLimit;            /* Flag (Bit-D: ON = 1) */

    /* ----- Initialize Error Address Buffer ----- */

    memset(achErrData, A111_ERR_INI, sizeof(achErrData));

    chErrType &= ~(A111_MDC_B | A111_MDC_D);
    chLeast = (char)(mdcData.uchMDCData & A111_MDC_B);
    chLimit = (char)(mdcData.uchMDCData & A111_MDC_D);

    /* ----- Check Data with Length & Least Significant Digit ----- */

    for (nCount = 0, chCount = 0; nCount < A111_ADDR_MAX; nCount++) {

        if (fType == A111_EDITTEXT) {   /* Check EditText Data  */

            anTare[nCount] = (short)GetDlgItemInt(hDlg,
                                                  IDD_A111_EDIT01 + nCount,
                                                  NULL,
                                                  FALSE);
        }

        /* ----- MDC Bit-D is ON [0 - 9999] ----- */

        if (chLimit) {

            /* ----- MDC Bit-B is OFF [0 or 5], and Least Digit is Illegal ----- */

            if ((! chLeast) && (anTare[nCount] % A111_LSTCHKER)) {

                /* ----- Set Address No. to Error Data Buffer ----- */

                achErrData[chCount++] = (char)nCount;
                chErrType |= A111_MDC_B;
            }

        /* ----- MDC Bit-D is OFF [0 - 999], and Data is Over Range ----- */

        } else if ((! chLimit) && (anTare[nCount] > A111_DOFF_MAX)) {

            /* ----- Set Address No. to Error Data Buffer ----- */

            achErrData[chCount++] = (char)nCount;
            chErrType |= A111_MDC_D;
        }
    }
    return (chCount);              /* Return Number of Error Data  */
}

/*
* ===========================================================================
**  Synopsis    :   char    A111SetError()
*
**  Input       :   HWND    hDlg        -   Window handle of a DialogBox
*                   UINT    unStatus    -   MessegeBox Style
*                   WPARAM  wID         -   Current Selected EditText ID
*
**  Return      :   No return values
*
**  Description :
*       This function displays a error message and resets the error data.
* ===========================================================================
*/
VOID    A111SetError(HWND hDlg, UINT unStatus, WORD wID)
{
    int     nRet;
    UINT    wStrID;
    WCHAR    chCount,
            szWork[A111_BUFF_LEN],
            szBuff[A111_BUFF_LEN],
            szNum[A111_NUM_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load Strings from Resource ----- */

    memset(szWork, 0, sizeof(szWork));

    wStrID = IDS_A111_AGAIN + unStatus;

    LoadString(hResourceDll, IDS_PEP_CAPTION_A111, szCaption, PEP_STRING_LEN_MAC(szCaption));
    LoadString(hResourceDll, IDS_A111_ADDR,        szBuff,    PEP_STRING_LEN_MAC(szBuff));
    LoadString(hResourceDll, wStrID,               szWork,    PEP_STRING_LEN_MAC(szWork));

    /* ----- Create Error Message ----- */

    wcscat(szWork, szBuff);

    for (chCount = 0; chCount < A111_ADDR_MAX; chCount++) {

        if (achErrData[chCount] == A111_ERR_INI) {
            break;
        }

        /* ----- Convert Addr. Number to String ----- */

        _itow((int)(achErrData[chCount] + 1), szNum, 10);

        /* ----- Set Error Message ----- */

        wcscat(szWork, szNum);

        /* ----- Set Colomn, If Not Exists Next Error Data ----- */

        if (achErrData[chCount + 1] != A111_ERR_INI) {

            wcscat(szWork, WIDE(", "));
        }
    }

    /* ----- Popup MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    nRet = MessageBoxPopUp(hDlg,
                      szWork,
                      szCaption,
                      afuStyle[unStatus]);

    if (unStatus == A111_OK) {      /* MessageBox Style is OK   */

        return;

    } else  if (nRet == IDCANCEL) { /* Pushed Cancel Button     */

        /* ---- Least Digit Illegal Error Exist ----- */

        if (chErrType & A111_MDC_B) {

            if (wID == IDD_A111RB4) {

                /* ----- Reset MDC Bit-D Data (ON [9999] >> OFF [999]) ----- */

                mdcData.uchMDCData &= (~A111_MDC_D);

                /* ----- Check RadioButton to ON [0 - 9999] ----- */

                CheckRadioButton(hDlg, IDD_A111RB3, IDD_A111RB4, IDD_A111RB3);
                SetFocus(GetDlgItem(hDlg, IDD_A111RB3));

                /* ----- Disable Bit-B RadioButton ----- */

                EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_B), FALSE);
                EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_B),  FALSE);
                EnableWindow(GetDlgItem(hDlg, IDD_A111RB1),     FALSE);
                EnableWindow(GetDlgItem(hDlg, IDD_A111RB2),     FALSE);

            } else {

                /* ----- Reset MDC Bit-B Data ( OFF [0, 5] >> ON [0 - 9]) ----- */

                mdcData.uchMDCData |= A111_MDC_B;

                /* ----- Check RadioButton to ON [0 - 9] ----- */

                CheckRadioButton(hDlg, IDD_A111RB1, IDD_A111RB2, IDD_A111RB2);
                SetFocus(GetDlgItem(hDlg, IDD_A111RB2));
            }
        }
        /* ---- Data Range Over Error Exist ----- */

        if (chErrType & A111_MDC_D) {

            /* ----- Reset MDC Bit-D Data (OFF [999] >> ON [9999]) ----- */

            mdcData.uchMDCData |= A111_MDC_D;

            /* ----- Check RadioButton to ON [0 - 9999] ----- */

            CheckRadioButton(hDlg, IDD_A111RB3, IDD_A111RB4, IDD_A111RB4);
            SetFocus(GetDlgItem(hDlg, IDD_A111RB4));

            /* ----- Enable Bit-B RadioButton ----- */

            EnableWindow(GetDlgItem(hDlg, IDD_A111GROUP_B), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_A111TEXT_B),  TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB1),     TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_A111RB2),     TRUE);
        }

    } else {                            /* Pushed OK Button     */

        for (chCount = 0; chCount < A111_ADDR_MAX; chCount++) {

            /* ----- Exit Roop, If Not Exists Error Data ----- */

            if (achErrData[chCount] == A111_ERR_INI) {
                break;
            }

            /* ----- Initialize Error Data ----- */

            anTare[achErrData[chCount]] = A111_INITIAL;

            /* ----- Set Initilized Data to EditText ----- */

            SetDlgItemInt(hDlg,
                          IDD_A111_EDIT01 + achErrData[chCount],
                          anTare[achErrData[chCount]],
                          FALSE);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A111CheckEdit()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WPARAM  wID     -   Current Selected EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This function checks a current inputed tare data in edittext.
*   It is called when the edittext has changed with MDC bit-D status is ON 
*   [0 - 9999] and bit-B status is OFF [0 or 5].
* ===========================================================================
*/
VOID    A111CheckEdit(HWND hDlg, WORD wID)
{
    UINT    unData;                         /* Inputed Data in EditText     */
    int     nTextLength;                    /* Length of EditText Data      */
    WCHAR    szCaption[PEP_CAPTION_LEN],
            szMessage[A111_BUFF_LEN];

    nTextLength = GetWindowTextLength(GetDlgItem(hDlg, wID));

    /* ----- Max. Length Data Has Inputed ----- */
    if (nTextLength == A111_DON_LEN) {

        unData = GetDlgItemInt(hDlg, wID, NULL, FALSE);

        if (unData % A111_LSTCHKER) {       /* If Least Digit is not 0 or 5 */

            LoadString(hResourceDll, IDS_PEP_CAPTION_A111, szCaption, PEP_STRING_LEN_MAC(szCaption));
            LoadString(hResourceDll, IDS_A111_0OR5,        szMessage, PEP_STRING_LEN_MAC(szMessage));

            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg,
                       szMessage,
                       szCaption,
                       afuStyle[A111_OK]);

            /* ----- Set HighLight Bar to Error Data ----- */
            SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   char    A111SetCondition()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description:
*                Set Condition String to StaticText Function.
* ===========================================================================
*/
VOID    A111SetCondition(HWND hDlg)
{
    UINT   nID;                        /* Resource Text for StaticText */
    WCHAR    szText[A111_TEXT_LEN + 1];  /* Points to Resource Text      */

    if (mdcData.uchMDCData & A111_MDC_D) {      /* Bit-D is ON [0 - 9999] */
        if (mdcData.uchMDCData & A111_MDC_B) {      /* Bit-B is ON [0 - 9]   */
            nID = IDS_A111TEXT01;
        } else {                                    /* Bit-B is OFF [0 or 5] */
            nID = IDS_A111TEXT02;
        }
    } else {                                    /* Bit-D is OFF [0 - 999] */
        nID = IDS_A111TEXT03;
    }

    /* ----- Get description from resource ----- */
    memset(szText, '\0', sizeof(szText));
    LoadString(hResourceDll, nID, szText, PEP_STRING_LEN_MAC(szText));

    /* ----- Set Loaded Text to StaticText ----- */
    DlgItemRedrawText(hDlg, IDD_A111CONDITION, szText);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A111SpinProc();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox
*               WPARAM  wParam  - 16 bits message parameter
*               LPARAM  lParam  - 32 bits message parameter
*
**  Return  :   TRUE            - User Process is Executed
*               FALSE           - Default Process is Expected
*
**  Description:
*       This function is spin procedure.
* ===========================================================================
*/
BOOL    A111SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    BOOL        fRet = FALSE;
    int         nEdit = 0;
    UINT        unRemain,
                unValue;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.lMin       = 0L;
    PepSpin.nTurbo     = A111_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */

    nEdit = (GetDlgCtrlID((HWND)lParam) - A111_SPIN_OFFSET);

    /* ----- Set minimum and maximum ----- */

    if (mdcData.uchMDCData & A111_MDC_D) {
        if (mdcData.uchMDCData & A111_MDC_B) {
            PepSpin.lMax     = (long)A111_DON_MAX;
            PepSpin.nStep    = A111_SPIN_BON_STEP;
            PepSpin.nTurStep = A111_SPIN_BON_TURSTEP;
        } else {
            PepSpin.lMax     = (long)A111_BOFF_MAX;
            PepSpin.nStep    = A111_SPIN_BOFF_STEP;
            PepSpin.nTurStep = A111_SPIN_BOFF_TURSTEP;

            /* ----- Adjust Least Digit [0 or 5] ----- */

            unValue  = GetDlgItemInt(hDlg, nEdit, NULL, FALSE);
            unRemain = (unValue % PepSpin.nStep);

            if (unRemain) {

                if (wParam == SB_LINEDOWN) {

                    unValue -= unRemain;

                } else {

                    unRemain = (UINT)(PepSpin.nStep - unRemain);
                    unValue += unRemain;

                    if (unValue > (UINT)PepSpin.lMax) {
                        unValue = (UINT)PepSpin.lMax;
                    }
                }

                SetDlgItemInt(hDlg, nEdit, unValue, FALSE);

                return (fRet);
            }
        }
    } else {
        PepSpin.lMax     = (long)A111_DOFF_MAX;
        PepSpin.nStep    = A111_SPIN_BON_STEP;
        PepSpin.nTurStep = A111_SPIN_BON_TURSTEP;
    }

    if (nEdit != 0) {

        /* ----- spin button procedure ----- */

        fRet = PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);

    }
    return (fRet);
}

HWND A111ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A111),
                                   hParentWnd,
                                   A111DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A111ModeLessDialogWnd (void)
{
	return hWndModeLess;
}

/* ===== End of A111.C ===== */
