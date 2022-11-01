/*
* ---------------------------------------------------------------------------
* Title         :   Set Discount/Surcharge/Bonus % Rate (AC 86)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A086.C
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
* Sep-14-98 : 03.03.00 : A.Mitsui   : V3.3
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

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a086.h"

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

#define  A86_TYPE_MDC_NONE           0x0000
#define  A86_TYPE_MDC_ONLY           0x0002

typedef struct {
	USHORT  usMdcAddr;   // if 0 then always on, otherwise MDC address for enable/disable on report
	USHORT  usMdcBit;    // if 0 then always on, otherwise MDC bit for enable/disable on report
	USHORT  usMnemType;  // type of mnemonic, P20, etc.
} A86_Rpt;

static A86_Rpt  A86_list [] = {
	{MDC_MODID13_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Item Discount #1
	{MDC_MODID23_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Item Discount #2
	{MDC_RDISC13_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #1
	{MDC_RDISC23_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #2
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 01
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 02
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 03
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 04
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 05
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 06
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 07
	{0, 0, A86_TYPE_MDC_NONE},                                   // Bonus No. 08
	{0, 0, A86_TYPE_MDC_NONE},                                   // Charge Tips No. 1
	{0, 0, A86_TYPE_MDC_NONE},                                   // Calculated Tips
	{MDC_RDISC33_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #3
	{MDC_RDISC43_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #4
	{MDC_RDISC53_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #5
	{MDC_RDISC63_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Transaction Discount #6
	{0, 0, A86_TYPE_MDC_NONE},                                   // Charge Tips No. 2
	{0, 0, A86_TYPE_MDC_NONE},                                   // Charge Tips No. 3
	{MDC_MODID33_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Item Discount #3
	{MDC_MODID43_ADR, EVEN_MDC_BIT0,  A86_TYPE_MDC_ONLY},        // Item Discount #4
	{MDC_MODID53_ADR, ODD_MDC_BIT0,  A86_TYPE_MDC_ONLY},         // Item Discount #5
	{MDC_MODID63_ADR, EVEN_MDC_BIT0,  A86_TYPE_MDC_ONLY}         // Item Discount #6
	// rest of the dialog list entries are for A86_TYPE_MDC_NONE so just ignore them.
};

static  UINT    unTopIndex,     /* Zero Based Top Index No. of ListBox      */
                unBottomIndex;  /* Zero Based Bottom Index No. of ListBox   */

static  BYTE    abMDCData[600];    /* Buffer of MDC Data       */

static UCHAR LclParaMDCRead( USHORT usAddress, UCHAR uchMaskData )
{
    UCHAR    uchMdc = 0;    // default is off in case usAddress is zero.

	// we use an address of zero to indicate
	// a report entry which is not to be changed.
	if (usAddress > 0) {
		/* calculate for fitting program address to ram address */
		USHORT    i = ((usAddress - 1) / 2);

		uchMdc = abMDCData[i];
		uchMdc &= uchMaskData;
	}
    return(uchMdc);                     /* return - masked bit pattern */
}

static UCHAR LclParaMDCWrite( USHORT usAddress, UCHAR uchMdc, UINT uiSetOn )
{
	// we use an address of zero to indicate
	// a report entry which is not to be changed.
	if (usAddress > 0) {
		/* calculate for fitting program address to ram address */
		USHORT    i = ((usAddress - 1) / 2);

		if (uiSetOn)
			abMDCData[i] |= uchMdc;
		else
			abMDCData[i] &= ~ uchMdc;

	}
    return(uchMdc);                     /* return - masked bit pattern */
}

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

static USHORT  FetchItemData (UINT  iOffset)
{
	USHORT  usValue = 0x0000;

	if (iOffset < sizeof(A86_list)/sizeof(A86_list[0])) {
		A86_Rpt  aItem = A86_list[iOffset];
		usValue = LclParaMDCRead (aItem.usMdcAddr, aItem.usMdcBit);
		usValue |= (aItem.usMnemType != A86_TYPE_MDC_NONE) ? 0x100 : 0;
	}

	return usValue;
}

BOOL    A086ChkDataBtn(HWND hDlg, WORD wBtnId)
{
    BOOL    fReturn = FALSE;    /* Return Value of This Function */
	UINT    uiIndex = A86GETINDEXBTN(wBtnId, unTopIndex);

	if (uiIndex < sizeof(A86_list)/sizeof(A86_list[0]) &&
		A86_list[uiIndex].usMnemType == A86_TYPE_MDC_ONLY) {
		UINT     uiCheck = 0;

		uiCheck = IsDlgButtonChecked (hDlg, wBtnId);
		// update the memory resident data.
		LclParaMDCWrite (A86_list[uiIndex].usMdcAddr, A86_list[uiIndex].usMdcBit, uiCheck);
	}

    return fReturn;
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A086DlgProc()
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
*               Set Discount/Surcharge/Bonus % Rate (Action Code # 86)
* ===========================================================================
*/
BOOL    WINAPI  A086DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  BYTE    abData[A86_ADDR_MAX];   /* Data Buffer of Input Data    */

    USHORT  usReturnLen;                    /* Return Length of ParaAll     */

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize Configulation of DialogBox ----- */
        A086InitDlg(hDlg, (LPBYTE)abData);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A86_DESC01; j<=IDD_A86_STRLEN; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A86_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				A086ScrlProc(hDlg, SB_LINEDOWN, hScroll, (LPBYTE)abData);
			} else if (dWheel > 0) {
				A086ScrlProc(hDlg, SB_LINEUP, hScroll, (LPBYTE)abData);
			}
		}
        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        if (GetDlgCtrlID((HWND)lParam) == IDD_A86_SCBAR) {
            /* ----- Vertical Scroll Bar Control ----- */
            A086ScrlProc(hDlg, wParam, lParam, (LPBYTE)abData);
        } else {
            /* ----- Spin Button Control ----- */
            A086SpinProc(hDlg, wParam, lParam, (LPBYTE)abData);
        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A86_EDIT01:
        case IDD_A86_EDIT02:
        case IDD_A86_EDIT03:
        case IDD_A86_EDIT04:
        case IDD_A86_EDIT05:
        case IDD_A86_EDIT06:
        case IDD_A86_EDIT07:
        case IDD_A86_EDIT08:
        case IDD_A86_EDIT09:
        case IDD_A86_EDIT10:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A086ChkData(hDlg, LOWORD(wParam), (LPBYTE)abData)) {
                    /* ----- Text out Error Message ----- */
                    A086ShowErr(hDlg);
                    /* ----- Reset Previous Data to Data Buffer ----- */
                    A086ResetData(hDlg, LOWORD(wParam));
                }
                return TRUE;
            }
            return FALSE;

		case IDS_A86_MDC_01:
		case IDS_A86_MDC_02:
		case IDS_A86_MDC_03:
		case IDS_A86_MDC_04:
		case IDS_A86_MDC_05:
		case IDS_A86_MDC_06:
		case IDS_A86_MDC_07:
		case IDS_A86_MDC_08:
		case IDS_A86_MDC_09:
		case IDS_A86_MDC_10:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Inputed Data with Data Range ----- */
                A086ChkDataBtn(hDlg, LOWORD(wParam));
                return TRUE;
			}
			return FALSE;

        case IDOK:
            if (HIWORD(wParam) == BN_CLICKED) {
                ParaAllWrite(CLASS_PARADISCTBL, (UCHAR *)abData, sizeof(abData), 0, &usReturnLen);
				ParaAllWrite(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);
                PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
                PepSetModFlag(hwndActMain, PEP_MF_PROG, 0);
			}
			// fall through to handle the general case of exit.
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
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
**  Synopsis    :   VOID    A086InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A086InitDlg(HWND hDlg, LPBYTE lpbData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;
    WCHAR    szDesc[128];

    /* ----- set initial description, V3.3 ----- */
    LoadString(hResourceDll, IDS_A86_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
    WindowRedrawText(hDlg, szDesc);

    LoadString(hResourceDll, IDS_A86_STRDESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A86_STRDESC, szDesc);
    LoadString(hResourceDll, IDS_A86_STRLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A86_STRLEN, szDesc);

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = A86_SCBAR_MIN;
    unBottomIndex = A86_ONE_PAGE;

    for (wID = IDD_A86_EDIT01; wID <= IDD_A86_EDIT10; wID++) {
        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, wID);
        PepSetFont(hDlg, wID);
        /* ----- Set Limit Length to EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, A86_DATA_LEN, 0L);
    }

    /* ----- Read Data from Parameter File ----- */
    ParaAllRead(CLASS_PARADISCTBL, (UCHAR *)lpbData, (A86_ADDR_MAX * sizeof(BYTE)), 0, &usReturnLen);

	/* ----- Read Initial Data from Parameter File ----- */
	ParaAllRead(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);

	/* ----- Set Description to StaticText ----- */
    A086RedrawText(hDlg, lpbData);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_A86_SCBAR), SB_CTL, A86_SCBAR_MIN, A86_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_A86_SCBAR), SB_CTL, A86_SCBAR_MIN, FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_A86_EDIT01));
    SendDlgItemMessage(hDlg, IDD_A86_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A086ChkData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditId -   Control ID of Current Changed EditText  
*                   LPBYTE  lpbData -   Points to a Buffer of Data
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
BOOL    A086ChkData(HWND hDlg, WORD wEditId, LPBYTE lpbData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */
    unValue = GetDlgItemInt(hDlg, wEditId, NULL, FALSE);
    if (A86_DATA_MAX < unValue) {
        fReturn = TRUE;
    } else {
        /* ----- Set Inputed Data to a Data Buffer ----- */
        *(lpbData + A86GETINDEX(wEditId, unTopIndex)) = (BYTE)unValue;
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086ShowErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A086ShowErr(HWND hDlg)
{
    WCHAR    szCaption[A86_CAP_LEN],     /* MessageBox Caption */
            szMessage[A86_ERR_LEN],     /* Error Message Strings */
            szWork[A86_ERR_LEN];        /* Error Message Strings Work Area */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A86, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szMessage, szWork, A86_DATA_MIN, A86_DATA_MAX);

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086ResetData()
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
VOID    A086ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */
    SendDlgItemMessage(hDlg, wParam, EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */
    SendDlgItemMessage(hDlg, wParam, EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A086SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)A86_DATA_MIN;
    PepSpin.lMax       = (long)A86_DATA_MAX;
    PepSpin.nStep      = A86_SPIN_STEP;
    PepSpin.nTurbo     = A86_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set Target EditText ID ----- */
    idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A86_SPIN_OFFSET);
    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
        A086ChkData(hDlg, (WORD)idEdit, lpbData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*                   LPBYTE  lpbData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID    A086ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPBYTE lpbData)
{
    switch (LOWORD(wParam)) {
    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! A086CalcIndex(LOWORD(wParam))) {
            /* ----- Redraw Description & Data by New Index ----- */
            A086RedrawText(hDlg, lpbData);
            /* ----- Redraw Scroll Box (Thumb) ----- */
            A086RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A86_ONE_PAGE;
        /* ----- Redraw Description & Data by New Index ----- */
        A086RedrawText(hDlg, lpbData);
        /* ----- Redraw Scroll Box (Thumb) ----- */
        A086RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A086CalcIndex()
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
BOOL    A086CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {
    case SB_LINEUP:
        unBottomIndex -= A86_ONE_LINE;
        if (unBottomIndex < A86_ONE_PAGE) {     /* Scroll to Top */
            unBottomIndex = A86_ONE_PAGE;
            unTopIndex    = A86_SCBAR_MIN;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line up */
            unTopIndex   -= A86_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A86_ONE_LINE;
        if (unBottomIndex >= A86_ADDR_MAX) {    /* Scroll to Bottom */
            unBottomIndex = (A86_ADDR_MAX - 1);
            unTopIndex    = A86_SCBAR_MAX;
            fReturn       = TRUE;
        } else {                                /* Scroll to One Line down */
            unTopIndex   += A86_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A86_SCBAR_MIN) {  /* Current Position isn't  Top */
            unBottomIndex -= A86_ONE_PAGE;
            if (unBottomIndex <= A86_ONE_PAGE) {    /* Scroll to Top */
                unBottomIndex = A86_ONE_PAGE;
                unTopIndex    = A86_SCBAR_MIN;
            } else {                                /* Scroll to One Page */
                unTopIndex    -= A86_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */
            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A86_SCBAR_MAX) {  /* Current Position isn't Bottom */
            unBottomIndex += A86_ONE_PAGE;
            if (unBottomIndex >= A86_ADDR_MAX) {    /* Scroll to Bottom */
                unBottomIndex = (A86_ADDR_MAX - 1);
                unTopIndex    = A86_SCBAR_MAX;
            } else {                                /* Scroll to One Line down */
                unTopIndex   += A86_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */
            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086RedrawText()
*
**  Input       :   HWND    hDlg    - Window Handle of a DialogBox
*                   LPBYTE  lpbData - Points to a Buffer of Data
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & data to each control.
* ===========================================================================
*/
VOID    A086RedrawText(HWND hDlg, LPBYTE lpbData)
{
    UINT    unCount;
    UINT    unLoop = 0;

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
		USHORT   usItem = FetchItemData(unCount);
		WCHAR    szWork[A86_DESC_LEN];

        /* ----- Load Strings from Resource ----- */
        LoadString(hResourceDll, IDS_A86_DESC01 + unCount, szWork, A86_DESC_LEN);

        /* ----- Set Loaded Strings to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_A86_DESC01 + unLoop, szWork);

        /* ----- Set Data to EditText ----- */
        SetDlgItemInt(hDlg, IDD_A86_EDIT01 + unLoop, (UINT)*(lpbData + (BYTE)unCount), FALSE);

		if (usItem & 0xff) {
			// this item is a Surcharge
			CheckDlgButton (hDlg, IDS_A86_MDC_01 + unLoop, BST_CHECKED);
		} else {
			// this items is a Discount or it is not a Discount/Surcharge
			CheckDlgButton (hDlg, IDS_A86_MDC_01 + unLoop, BST_UNCHECKED);
		}
		EnableWindow(GetDlgItem(hDlg, IDS_A86_MDC_01 + unLoop), (usItem & 0x100));    // no mnemonic address and mnemnic can not be changed.
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A086RedrawThumb()
*
**  Input       :   HWND    hCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A086RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */
    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);
}

/* ===== End of A086.C ===== */
