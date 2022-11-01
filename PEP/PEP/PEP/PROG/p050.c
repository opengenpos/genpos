/*
* ---------------------------------------------------------------------------
* Title         :   Assignment Terminal No. (Prog. 50)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P050.C
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
#define _WIN32_WINNT 0x0400

#include    <Windows.h>
#include    <Windowsx.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p050.h"

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
static  UINT    unTopIndex,     /* Zero Based Top Index No. of ListBox      */
                unBottomIndex;  /* Zero Based Bottom Index No. of ListBox   */

/* NCR2172 */
static  UINT    unTopIndexRP,     /* Zero Based Top Index No. of ListBox      */
                unBottomIndexRP;  /* Zero Based Bottom Index No. of ListBox   */

/* Select View Data of ListBox NCR2172 */
static WORD     awSelectData[16] = {
                    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
                   11,  12,  13,  14,  15,  16
                };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/
static UINT P50GetAcceptableMaxValue(int);

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P050DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispathed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*       Assignment Terminal No. Installing Shared Printer (Program Mode # 50)
* ===========================================================================
*/
BOOL    WINAPI  P050DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  P50DATA aPrtData[P50_ADDR_NO];

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:

 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        P50InitDlg(hDlg, (LPP50DATA)aPrtData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P50_DESC01; j<=IDD_P50_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_MOUSEWHEEL:
		{
			int     dWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P50_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				P50ScrlProc(hDlg, SB_LINEDOWN, hScroll, (LPP50DATA)aPrtData);
			} else if (dWheel > 0) {
				P50ScrlProc(hDlg, SB_LINEUP, hScroll, (LPP50DATA)aPrtData);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_P50_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            P50ScrlProc(hDlg, wParam, lParam, (LPP50DATA)aPrtData);

        }  else {

            /* ----- Spin Button Control ----- */

            P50SpinProc(hDlg, wParam, lParam, (LPP50DATA)aPrtData);

        }

        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_P50_SHARE01:
        case IDD_P50_SHARE02:
        case IDD_P50_SHARE03:
        case IDD_P50_SHARE04:
        case IDD_P50_SHARE05:
        case IDD_P50_SHARE06:
        case IDD_P50_SHARE07:
        case IDD_P50_SHARE08:
        case IDD_P50_SHARE09:
        case IDD_P50_SHARE10:
        case IDD_P50_ALTER01:
        case IDD_P50_ALTER02:
        case IDD_P50_ALTER03:
        case IDD_P50_ALTER04:
        case IDD_P50_ALTER05:
        case IDD_P50_ALTER06:
        case IDD_P50_ALTER07:
        case IDD_P50_ALTER08:
        case IDD_P50_ALTER09:
        case IDD_P50_ALTER10:
        case IDD_P50_KPADD01:
        case IDD_P50_KPADD02:
        case IDD_P50_KPADD03:
        case IDD_P50_KPADD04:
        case IDD_P50_KPADD05:
        case IDD_P50_KPADD06:
        case IDD_P50_KPADD07:
        case IDD_P50_KPADD08:

/*** NCR2172 ***/

        case IDD_P50_KPADD09:
        case IDD_P50_KPADD10:
		case IDD_P50_KPADD11:
		case IDD_P50_KPADD12:
		case IDD_P50_KPADD13:
		case IDD_P50_KPADD14:
		case IDD_P50_KPADD15:
		case IDD_P50_KPADD16:
            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (P50GetData(hDlg, wParam, (LPP50DATA)aPrtData)) {

                    /* ----- Text out Error Message ----- */

                    P50DispErr(hDlg, wParam);

                    /* ----- Reset Previous Data ----- */

                    P50ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARASHRPRT,
                                 (UCHAR *)aPrtData,
                                 sizeof(aPrtData),
                                 0, &usReturnLen);

                    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
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
**  Synopsis    :   VOID    P50InitDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPP50DATA   lpaData -   Address of the Data Buffer
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    P50InitDlg(HWND hDlg, LPP50DATA lpaData)
{
    USHORT  usReturnLen;
    WORD    wID;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = unTopIndexRP    = P50_SCBAR_MIN;
    unBottomIndex = unBottomIndexRP = P50_ONE_PAGE;

    for (wID = 0; wID < P50_MAX_PAGE; wID++) {

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_P50_SHARE01 + wID));
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_P50_ALTER01 + wID));

        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, IDD_P50_SHARE01 + wID, EM_LIMITTEXT, P50_DATA_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_P50_ALTER01 + wID, EM_LIMITTEXT, P50_DATA_LEN, 0L);
    }

/*** NCR2172 Start ***/
    for (wID = 0; wID < P50_KPCP_MAX_DESC; wID++) {
        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_P50_KPADD01 + wID));
        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, IDD_P50_KPADD01 + wID, EM_LIMITTEXT, P50_DATA_LEN, 0L);
    }


/*** NCR2172 End ***/

    /* ----- Read Data from Parameter File ----- */
    ParaAllRead(CLASS_PARASHRPRT, (UCHAR *)lpaData, (P50_ADDR_NO * sizeof(P50DATA)), 0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */
    P50RedrawText(hDlg, lpaData);
#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P50_SCBAR), SB_CTL, P50_SCBAR_MIN, P50_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P50_SCBAR), SB_CTL, P50_SCBAR_MIN, FALSE);
#else
	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P50_MAX_PAGE;
		scrollInfo.nMin = P50_SCBAR_MIN;
		scrollInfo.nMax = P50_KPAD_MAX - 1;    // scroll range is zero based. this TOTAL_MAX is different from other mnemonic dialogs.
		scrollInfo.nPos = P50_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P50_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}
#endif

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P50_SHARE01));
    SendDlgItemMessage(hDlg, IDD_P50_SHARE01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :(  BOOL    P50GetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Control ID of Current Changed EditText  
*                   LPP50DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is over range.
*                           FALSE   -   Inputed Data is correct.
*
**  Description :
*       This function gets current inputed data from EditText. And then it
*   checks data with data range. If over range error occured, returns TRUE.
*   Oterwise it set inputed data to buffer, and returns FALSE.
* ===========================================================================
*/
BOOL    P50GetData(HWND hDlg, WPARAM wParam, LPP50DATA lpaData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    BYTE    wID = 0;
    UINT    unMax;              /* acceptable maximum value */
    
    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
    
    /* ----- Check if data is valid or not ----- */

    unMax = P50GetAcceptableMaxValue(LOWORD(wParam));

    if (unMax < unValue) {
        return TRUE;
    }

    /* ----- Set Inputed Data to a Data Buffer ----- */

    if (LOWORD(wParam) >= IDD_P50_KPADD01) {

        wID = P50GETINDEX(wParam, unTopIndexRP);

        switch(wID){
        case 0:                                               /* Remote Printer No.1 */
            (lpaData + P50_KPAD_BASE)->bShare = (BYTE)unValue;
            break;
        case 1:                                               /* Remote Printer No.1 */
            (lpaData + P50_KPAD_BASE)->bAlter = (BYTE)unValue;
            break;
        case 2:                                               /* Remote Printer No.2 */
            (lpaData + P50_KPAD_BASE + 1)->bShare = (BYTE)unValue;
            break;
        case 3:                                               /* Remote Printer No.2 */
            (lpaData + P50_KPAD_BASE + 1)->bAlter = (BYTE)unValue;
            break;
        case 4:                                               /* Remote Printer No.3 */
            (lpaData + P50_KPAD_BASE + 2)->bShare = (BYTE)unValue;
            break;                      
        case 5:                                               /* Remote Printer No.3 */
            (lpaData + P50_KPAD_BASE + 2)->bAlter = (BYTE)unValue;
            break;
        case 6:                                               /* Remote Printer No.4 */
            (lpaData + P50_KPAD_BASE + 3)->bShare = (BYTE)unValue;
            break;
        case 7:                                               /* Remote Printer No.4 */
            (lpaData + P50_KPAD_BASE + 3)->bAlter = (BYTE)unValue;
            break;

        case 8:                                               /* Kitchen Printer No.1 */
            (lpaData + P50_KPAD_BASE + 4)->bShare = (BYTE)unValue;
            break;
        case 9:                                               /* Kitchen Printer No.1 */
            (lpaData + P50_KPAD_BASE + 4)->bAlter = (BYTE)unValue;
            break;
        case 10:                                               /* Kitchen Printer No.2 */
            (lpaData + P50_KPAD_BASE + 5)->bShare = (BYTE)unValue;
            break;
        case 11:                                               /* Kitchen Printer No.2 */
            (lpaData + P50_KPAD_BASE + 5)->bAlter = (BYTE)unValue;
            break;
        case 12:                                               /* Kitchen Printer No.3 */
            (lpaData + P50_KPAD_BASE + 6)->bShare = (BYTE)unValue;
            break;                      
        case 13:                                               /* Kitchen Printer No.3 */
            (lpaData + P50_KPAD_BASE + 6)->bAlter = (BYTE)unValue;
            break;
        case 14:                                               /* Kitchen Printer No.4 */
            (lpaData + P50_KPAD_BASE + 7)->bShare = (BYTE)unValue;
            break;
        case 15:                                               /* Kitchen Printer No.4 */
            (lpaData + P50_KPAD_BASE + 7)->bAlter = (BYTE)unValue;
            break;
        }
    } else if (LOWORD(wParam) >= IDD_P50_ALTER01) {

        (lpaData + P50GETALTINDEX(wParam, unTopIndex))->bAlter = (BYTE)unValue;

    } else {

        (lpaData + P50GETSHRINDEX(wParam, unTopIndex))->bShare = (BYTE)unValue;

    }

    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*               :   int     idEdit  -   Control Id of control
*                                       IDD_P50_SHARExx
*                                       IDD_P50_ALTERxx
*                                       IDD_P50_KPADDxx
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    P50DispErr(HWND hDlg, int idEdit)
{
    WCHAR    szCaption[PEP_CAPTION_LEN],     /* MessageBox Caption       */
            szMessage[PEP_OVER_LEN],        /* Error Message Strings    */
            szWork[PEP_OVER_LEN];           /* Error Message Work Area  */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_P50, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, P50_DATA_MIN, P50GetAcceptableMaxValue(LOWORD(idEdit)));

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50ResetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Reset EditText
*
**  Return      :   No return value.
*
**  Description :
*       This function undoes data input to EditText.
* ===========================================================================
*/
VOID    P50ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50SpinProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*                   LPP50DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    P50SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPP50DATA lpaData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    if( GetDlgCtrlID((HWND)lParam) < P50_RMDEVICE_CP_OFFSET) { // ESMITH P50DLG CHANGE
		idEdit = GetDlgCtrlID((HWND)lParam) - P50_SPIN_OFFSET;
	}else{
		idEdit = GetDlgCtrlID((HWND)lParam) - P50_COM_OFFSET;
	}

    PepSpin.lMin       = (long)P50_DATA_MIN;
    PepSpin.lMax       = (long)P50GetAcceptableMaxValue(idEdit);
    PepSpin.nStep      = P50_SPIN_STEP;
    PepSpin.nTurbo     = P50_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

        P50GetData(hDlg, (WORD)idEdit, lpaData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50ScrlProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*                   LPP50DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID
P50ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPP50DATA lpaData)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! P50CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Data by New Index ----- */

            P50RedrawText(hDlg, lpaData);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            P50RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + P50_ONE_PAGE;

        /* ----- Redraw Description & Data by New Index ----- */

        P50RedrawText(hDlg, lpaData);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        P50RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50ScrlProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*                   LPP50DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID
P50ScrlProcRP(HWND hDlg, WPARAM wParam, LPARAM lParam, LPP50DATA lpaData)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! P50CalcIndexRP(LOWORD(wParam))) {

            /* ----- Redraw Description & Data by New Index ----- */

            P50RedrawText(hDlg, lpaData);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            P50RedrawThumbRP(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndexRP    = HIWORD(wParam);
        unBottomIndexRP = unTopIndexRP + P50_ONE_PAGE;

        /* ----- Redraw Description & Data by New Index ----- */

        P50RedrawText(hDlg, lpaData);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        P50RedrawThumbRP(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P50CalcIndex()
*
**  Input       :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return      :   BOOL    TRUE    -   Scroll up to Top or Bottom of ListBox
*                           FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description :
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P50CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= P50_ONE_LINE;

        if (unBottomIndex < P50_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = P50_ONE_PAGE;
            unTopIndex    = P50_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= P50_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += P50_ONE_LINE;

        if (unBottomIndex >= P50_ADDR_NO-8) {     /* Scroll to Bottom */

            unBottomIndex = (P50_ADDR_NO - 1-8);
            unTopIndex    = P50_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += P50_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != P50_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= P50_ONE_PAGE;

            if (unBottomIndex <= P50_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = P50_ONE_PAGE;
                unTopIndex    = P50_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= P50_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != P50_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += P50_ONE_PAGE;

            if (unBottomIndex >= P50_ADDR_NO-8) {     /* Scroll to Bottom */

                unBottomIndex = (P50_ADDR_NO - 1-8);
                unTopIndex    = P50_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += P50_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P50CalcIndexRP()
*
**  Input       :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return      :   BOOL    TRUE    -   Scroll up to Top or Bottom of ListBox
*                           FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description :
*       This function compute top index and bottom index.
* ===========================================================================
*/
BOOL    P50CalcIndexRP(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndexRP -= P50_ONE_LINE;

        if (unBottomIndexRP < P50_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndexRP = P50_ONE_PAGE;
            unTopIndexRP    = P50_SCBAR_MIN;
            fReturn         = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndexRP   -= P50_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndexRP += P50_ONE_LINE;

        if (unBottomIndexRP >= P50_ADDR_NO-8) {     /* Scroll to Bottom */

            unBottomIndexRP = (P50_ADDR_NO - 1-8);
            unTopIndexRP    = P50_SCBAR_MAX;
            fReturn         = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndexRP   += P50_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndexRP != P50_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndexRP -= P50_ONE_PAGE;

            if (unBottomIndexRP <= P50_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndexRP = P50_ONE_PAGE;
                unTopIndexRP    = P50_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndexRP    -= P50_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndexRP != P50_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndexRP += P50_ONE_PAGE;

            if (unBottomIndexRP >= P50_ADDR_NO-8) {     /* Scroll to Bottom */

                unBottomIndexRP = (P50_ADDR_NO - 1-8);
                unTopIndexRP    = P50_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndexRP   += P50_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50RedrawText()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPP50DATA   lpaData -   Address of the Data Buffer
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & data to each control.
* ===========================================================================
*/
VOID    P50RedrawText(HWND hDlg, LPP50DATA lpaData)
{
    UINT    unCount,
            unLoop = 0;
    WCHAR    szWork[P50_DESC_LEN],
            szDesc[P50_DESC_LEN];
    
    USHORT usBase;               /* NCR2172 */
    
    /* ----- Load Strings from Resource ----- */

    LoadString(hResourceDll, IDS_P50_DESC, szWork, PEP_STRING_LEN_MAC(szWork));

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Create Description with Address No. ----- */
        wsPepf(szDesc, szWork, (unCount + 1)/10, (unCount + 1)%10);

        /* ----- Set Loaded Strings to StaticText ----- */

		DlgItemRedrawText(hDlg, (WORD)(IDD_P50_DESC01 + unLoop), szDesc);
    }
    
	for(unCount = 0; unCount < P50_KPCP_MAX_DESC; unCount++) {
        /* ----- Set Loaded Strings to StaticText ----- */

		LoadString(hResourceDll, IDS_P50_KAD1 + unCount, szWork, PEP_STRING_LEN_MAC(szWork));
		DlgItemRedrawText(hDlg, IDD_P50_KPDES01 + unCount, szWork);
    }


    for (unCount = 0, unLoop = unTopIndex; unCount < P50_MAX_PAGE; unCount++, unLoop++) {

        /* ----- Set Data to EditText ----- */

        SetDlgItemInt(hDlg,
                      IDD_P50_SHARE01 + unCount,
                      (UINT)(lpaData + unLoop)->bShare,
                      FALSE);

        SetDlgItemInt(hDlg,
                      IDD_P50_ALTER01 + unCount,
                      (UINT)(lpaData + unLoop)->bAlter,
                      FALSE);
    }

    switch(unTopIndexRP) {

        case P50_INDEXTOP01:
        case P50_INDEXTOP02:
             usBase = P50_BASETOP01;

        break;

        case P50_INDEXTOP03:
        case P50_INDEXTOP04:
             usBase = P50_BASETOP02;

        break;

        case P50_INDEXTOP05:
        case P50_INDEXTOP06:

             usBase = P50_BASETOP03;
        break;

        default:

             usBase = P50_BASETOP04;
        break;

    }

	// There are 23 addresses associated with the lpaData pointer
	// each address has an alternate and a share remote device. 
	// The first 16 (0-15) represent terminals, where each one can 
	// specify an alternate	and a shared device. For the remaining 8 
	// (16-23), the alternate/shared address is used to represent 
	//  the remaining 16 ports and alternate back and forth.
	//  Ex. 
	//		Address 16(shr) is for Remote Device 1's com port
	//		Address 16(alt) is for Remote Device 2's com port
	//		...
	//		Address 20(shr) is for Com Port 1
	//		Address 20(alt) is for Com Port 2  ESMITH P50DLG CHANGE
	for(unCount = 0, unLoop = 0; unCount < P50_KPCP_MAX_DESC; unCount +=2, unLoop++) {


        /* ----- Set Data to EditText ----- */
       SetDlgItemInt(hDlg,
                     IDD_P50_KPADD01 + unCount,
                     (UINT)(lpaData + P50_KPAD_BASE + unLoop)->bShare,
                     FALSE);

       SetDlgItemInt(hDlg,
                     IDD_P50_KPADD02 + unCount,
                     (UINT)(lpaData + P50_KPAD_BASE + unLoop)->bAlter,
                     FALSE);
	}
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    P50RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P50RedrawThumbRP()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    P50RedrawThumbRP(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndexRP, TRUE);

}

/*
* ===========================================================================
**  Synopsis    :   UINT    P50GetAcceptableMaxValue()
*
**  Input       :   int     idCtrl   -   Control ID of control
*                                        IDD_P50_SHARExx
*                                        IDD_P50_ALTERxx
*                                        IDD_P50_KPADDxx
*
**  Return      :   Acceptable maximum value
*
**  Description :
*       This function returns acceptable maximum value for passed control
* ===========================================================================
*/
static UINT    P50GetAcceptableMaxValue(int idCtrl)
{
    UINT unMax = P50_DATA_MAX;

    if ((IDD_P50_KPADD01 <= idCtrl) && (idCtrl <= IDD_P50_KPADD16)) {
        if (P50GETINDEX(idCtrl, unTopIndexRP) < P50_KPAD_MAX_REMOTE_DEVICE) {
            unMax = P50_KPAD_MAX;
        }
        else {
            unMax = P50_PORT_MAX;
        }
    }

    return unMax;
}
/* ===== End of P050.C ===== */
