/*
* ---------------------------------------------------------------------------
* Title         :   Beverrage Dispenser Parameter (A/C 137)
* Category      :   Setup, NCR 2170 PEP for Windows (US R3.1 Model)
* Program Name  :   A137.C
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
* Mar-08-96 : 03.00.05 : M.Suzuki   : Initial (HP US R3.1 Model)
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a137.h"

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
static  WORD    wGunNo,         /* Gun Number                               */
                wType;          /* Drink Type                               */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A137DlgProc()
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
*       Set Beverrage Dispenser Parameter (A/C # 137)
* ===========================================================================
*/
BOOL    WINAPI  A137DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  A137DATA aDispData[A137_ADDR_MAX];

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        A137InitDlg(hDlg, (LPA137DATA)aDispData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A137_DESC01; j<=IDD_A137_STATIC_RNG; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A137_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				A137ScrlProc(hDlg, SB_LINEDOWN, hScroll, (LPA137DATA)aDispData);
			} else if (dWheel > 0) {
				A137ScrlProc(hDlg, SB_LINEUP, hScroll, (LPA137DATA)aDispData);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_A137_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            A137ScrlProc(hDlg, wParam, lParam, (LPA137DATA)aDispData);

        } else {

            /* ----- Spin Button Control ----- */

            A137SpinProc(hDlg, wParam, lParam, (LPA137DATA)aDispData);

        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A137_PLU01:
        case IDD_A137_PLU02:
        case IDD_A137_PLU03:
        case IDD_A137_PLU04:
        case IDD_A137_PLU05:
        case IDD_A137_PLU06:
        case IDD_A137_PLU07:
        case IDD_A137_PLU08:
        case IDD_A137_PLU09:
        case IDD_A137_PLU10:
        case IDD_A137_ADJ01:
        case IDD_A137_ADJ02:
        case IDD_A137_ADJ03:
        case IDD_A137_ADJ04:
        case IDD_A137_ADJ05:
        case IDD_A137_ADJ06:
        case IDD_A137_ADJ07:
        case IDD_A137_ADJ08:
        case IDD_A137_ADJ09:
        case IDD_A137_ADJ10:
        case IDD_A137_OFFSETCODE:
        case IDD_A137_NEXTORDER:
        case IDD_A137_ENDPOUR:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (A137GetData(hDlg, wParam, (LPA137DATA)aDispData)) {

                    /* ----- Text out Error Message ----- */

                    A137DispErr(hDlg, wParam);

                    /* ----- Reset Previous Data ----- */

                    A137ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_A137_GUN:

            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- Set Gun No. ----- */

                A137SetGunData(hDlg, (LPA137DATA)aDispData);

                return TRUE;
            }
            return FALSE;

        case    IDD_A137_TYPE:

            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- Set Drink Type ----- */

                A137SetTypeData(hDlg, (LPA137DATA)aDispData);

                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARADISPPARA,
                                 (UCHAR *)aDispData,
                                 sizeof(aDispData),
                                 0, &usReturnLen);

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

/*
* ===========================================================================
**  Synopsis    :   VOID    A137InitDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPA137DATA   lpaData -   Address of the Data Buffer
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A137InitDlg(HWND hDlg, LPA137DATA lpaData)
{
    USHORT  usReturnLen;
    WORD    wID, wI;
    WCHAR   szCaption[PEP_CAPTION_LEN],
            szMessage[PEP_OVER_LEN],
			szWork[A137_DESC_LEN]; 
	int i;

    /* ----- Set Top Index & Bottom Index ----- */

    unTopIndex    = A137_SCBAR_MIN;
    unBottomIndex = A137_ONE_PAGE;
    wGunNo  = 0;
    wType  = 0;

    for (wID = 0; wID < A137_MAX_PAGE; wID++) {

        /* ----- Change Font to Fixed Font ----- */
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_A137_PLU01 + wID));
		EditBoxProcFactory (NULL, hDlg, (USHORT)(IDD_A137_ADJ01 + wID));

        PepSetFont(hDlg, wID);
        /* ----- Set Limit Length to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_A137_PLU01 + wID,
                           EM_LIMITTEXT, A137_PLU_DATA_LEN, 0L);

        SendDlgItemMessage(hDlg, IDD_A137_ADJ01 + wID,
                           EM_LIMITTEXT, A137_ADJ_DATA_LEN, 0L);
    }

    /* ----- Set Limit Length to EditText ----- */

    SendDlgItemMessage(hDlg, IDD_A137_OFFSETCODE,
                       EM_LIMITTEXT, A137_OFFSET_DATA_LEN, 0L);

    /* ----- Set Limit Length to EditText ----- */

    SendDlgItemMessage(hDlg, IDD_A137_NEXTORDER,
                       EM_LIMITTEXT, A137_NEXTORDER_DATA_LEN, 0L);

    /* ----- Set Limit Length to EditText ----- */

    SendDlgItemMessage(hDlg, IDD_A137_ENDPOUR,
                       EM_LIMITTEXT, A137_ENDPOUR_DATA_LEN, 0L);

    /* ----- Read Data from Parameter File ----- */

    ParaAllRead(CLASS_PARADISPPARA,
                (UCHAR *)lpaData,
                (A137_ADDR_MAX * sizeof(A137DATA)),
                0, &usReturnLen);

	/* ----- Check for Values Beyond Limit ----- */

	for(i=0; i<A137_DRINK_MAX; i++){
		if((lpaData+i)->wPlu > A137_PLU_DATA_MAX){
			(lpaData+i)->wPlu = 0;
			LoadString(hResourceDll, IDS_PEP_CAPTION_A137, szCaption, PEP_STRING_LEN_MAC(szCaption));
			LoadString(hResourceDll, IDS_A137_PLU_OVER, szWork, PEP_STRING_LEN_MAC(szWork));
			wsPepf(szMessage, szWork, i+1, A137_PLU_DATA_MAX);
			MessageBeep(MB_ICONEXCLAMATION);
			MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
		}
		if((lpaData+i)->bAdj > A137_ADJ_DATA_MAX){
			(lpaData+i)->bAdj = 0;
			LoadString(hResourceDll, IDS_PEP_CAPTION_A137, szCaption, PEP_STRING_LEN_MAC(szCaption));
			LoadString(hResourceDll, IDS_A137_ADJ_OVER, szWork, PEP_STRING_LEN_MAC(szWork));
			wsPepf(szMessage, szWork, i+1, A137_ADJ_DATA_MAX);
			MessageBeep(MB_ICONEXCLAMATION);
			MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
		}
	}

    /* ----- Set Description to StaticText ----- */

    A137RedrawText(hDlg, lpaData);

    /* ----- Set Loaded Strings to StaticText ----- */
    LoadString(hResourceDll, IDS_A137_OFFSETCODE, szWork, PEP_STRING_LEN_MAC(szWork));
    DlgItemRedrawText(hDlg, IDD_A137_OFFSETDES, szWork);

    /* ----- Set Loaded Strings to StaticText ----- */
    LoadString(hResourceDll, IDS_A137_NEXTORDER, szWork, PEP_STRING_LEN_MAC(szWork));
    DlgItemRedrawText(hDlg, IDD_A137_NEXTORDERDES, szWork);

    /* ----- Set Loaded Strings to StaticText ----- */
    LoadString(hResourceDll, IDS_A137_ENDPOUR, szWork, PEP_STRING_LEN_MAC(szWork));
    DlgItemRedrawText(hDlg, IDD_A137_ENDPOURDES, szWork);

    /* ----- Set Data to EditText ----- */
    SetDlgItemInt(hDlg,
                  IDD_A137_OFFSETCODE,
                  (UINT)(lpaData + A137_OFFSET_BASE)->wPlu,
                  FALSE);

    SetDlgItemInt(hDlg,
                  IDD_A137_NEXTORDER,
                  (UINT)(lpaData + A137_NEXTORDER_BASE)->wPlu,
                  FALSE);

    SetDlgItemInt(hDlg,
                  IDD_A137_ENDPOUR,
                  (UINT)(lpaData + A137_ENDPOUR_BASE)->wPlu,
                  FALSE);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */

    SetScrollRange(GetDlgItem(hDlg, IDD_A137_SCBAR),
                   SB_CTL, A137_SCBAR_MIN, A137_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A137_SCBAR),
                 SB_CTL, A137_SCBAR_MIN, FALSE);

    /* ----- Insert Gun No. to ComboBox ----- */

    for (wI = 0; wI < A137_GUN_NO_MAX; wI++) {

        wsPepf(szWork, WIDE("%u"), (short)(wI + A137_DEF_GUNNO));
        DlgItemSendTextMessage(hDlg,
                           IDD_A137_GUN,
                           CB_INSERTSTRING,
                           (WPARAM)-1,
                           (LPARAM)(szWork));
    }

    SendDlgItemMessage(hDlg, IDD_A137_GUN, CB_SETCURSEL, 0, 0L);

    /* ----- Set type description to combo-box ----- */

    for (wI = 0; wI < A137_TYPE_NO_MAX; wI++) {
        /* ----- Set Loaded Strings to StaticText ----- */
        LoadString(hResourceDll, IDS_A137_TYPE_NORMAL + wI, szWork, PEP_STRING_LEN_MAC(szWork));
        DlgItemSendTextMessage(hDlg,
                           IDD_A137_TYPE,
                           CB_INSERTSTRING,
                           (WPARAM)-1,
                           (LPARAM)(szWork));
    }

    SendDlgItemMessage(hDlg, IDD_A137_TYPE, CB_SETCURSEL, 0, 0L);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A137_PLU01));
    SendDlgItemMessage(hDlg, IDD_A137_PLU01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :(  BOOL    A137GetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Control ID of Current Changed EditText  
*                   LPA137DATA   lpaData -   Points to a Buffer of Data
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
BOOL    A137GetData(HWND hDlg, WPARAM wParam, LPA137DATA lpaData)
{
    UINT    unValue;            /* Value of Inputed Data */
    BOOL    fReturn = FALSE;    /* Return Value of This Function */
    LONG    lWork;
    WCHAR    szWork[A137_DESC_LEN];

    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
    
    if (LOWORD(wParam) == IDD_A137_OFFSETCODE) {

        /* ----- Get Data from EditText ----- */

        memset(szWork, 0, sizeof(szWork));
    
        DlgItemGetText(hDlg, LOWORD(wParam), szWork, PEP_STRING_LEN_MAC(szWork));

        lWork = _wtol(szWork);

        if (A137_OFFSET_DATA_MAX < lWork) {

            fReturn = TRUE;

        } else {

            /* ----- Set Inputed Data to a Data Buffer ----- */

            (lpaData + A137_OFFSET_BASE)->wPlu = (WORD)unValue;
        }
    } else if (LOWORD(wParam) == IDD_A137_NEXTORDER) {

        if (A137_NEXTORDER_DATA_MAX < unValue) {

            fReturn = TRUE;

        } else {

            /* ----- Set Inputed Data to a Data Buffer ----- */

            (lpaData + A137_NEXTORDER_BASE)->wPlu = (WORD)unValue;
        }
    } else if (LOWORD(wParam) == IDD_A137_ENDPOUR) {

        if (A137_ENDPOUR_DATA_MAX < unValue) {

            fReturn = TRUE;

        } else {

            /* ----- Set Inputed Data to a Data Buffer ----- */

            (lpaData + A137_ENDPOUR_BASE)->wPlu = (WORD)unValue;
        }
    } else if (LOWORD(wParam) >= IDD_A137_ADJ01) {

        if (A137_ADJ_DATA_MAX < unValue) {

            fReturn = TRUE;

        } else {

            (lpaData + A137GETADJINDEX(wParam, unTopIndex)
             + (wGunNo * A137_GUN_MAX) 
             + (wType * A137_TYPE_MAX))->bAdj= (BYTE)unValue;
        }

    } else {

        if (A137_PLU_DATA_MAX < unValue) {

            fReturn = TRUE;

        } else {

           (lpaData + A137GETPLUINDEX(wParam, unTopIndex)
           + (wGunNo * A137_GUN_MAX) 
           + (wType * A137_TYPE_MAX))->wPlu= (WORD)unValue;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Control ID of Current Changed EditText  
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A137DispErr(HWND hDlg, WPARAM wParam)
{
    WCHAR    szCaption[PEP_CAPTION_LEN],     /* MessageBox Caption       */
            szMessage[PEP_OVER_LEN],        /* Error Message Strings    */
            szWork[PEP_OVER_LEN];           /* Error Message Work Area  */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A137, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    if (LOWORD(wParam) == IDD_A137_OFFSETCODE) {

        wsPepf(szMessage, szWork, A137_DATA_MIN, A137_OFFSET_DATA_MAX);

    } else if (LOWORD(wParam) == IDD_A137_NEXTORDER) {

        wsPepf(szMessage, szWork, A137_DATA_MIN, A137_NEXTORDER_DATA_MAX);

    } else if (LOWORD(wParam) == IDD_A137_ENDPOUR) {

        wsPepf(szMessage, szWork, A137_DATA_MIN, A137_ENDPOUR_DATA_MAX);

    } else if (LOWORD(wParam) >= IDD_A137_ADJ01) {

        wsPepf(szMessage, szWork, A137_DATA_MIN, A137_ADJ_DATA_MAX);

    } else {

        wsPepf(szMessage, szWork, A137_DATA_MIN, A137_PLU_DATA_MAX);

    }

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137ResetData()
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
VOID    A137ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137SpinProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*                   LPA137DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A137SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPA137DATA lpaData)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)A137_DATA_MIN;
    PepSpin.nStep      = A137_SPIN_STEP;
    PepSpin.nTurbo     = A137_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A137_SPIN_OFFSET);

    if (idEdit == IDD_A137_OFFSETCODE) {

        PepSpin.lMax = (long)A137_OFFSET_DATA_MAX;

    } else if (idEdit == IDD_A137_NEXTORDER) {

        PepSpin.lMax = (long)A137_NEXTORDER_DATA_MAX;

    } else if (idEdit == IDD_A137_ENDPOUR) {

        PepSpin.lMax = (long)A137_ENDPOUR_DATA_MAX;

    } else if (idEdit >= IDD_A137_ADJ01) {

        PepSpin.lMax = (long)A137_ADJ_DATA_MAX;

    } else {

        PepSpin.lMax = (long)A137_PLU_DATA_MAX;

    }
    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

        A137GetData(hDlg, (WORD)idEdit, lpaData);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137ScrlProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*                   LPA137DATA   lpaData -   Points to a Buffer of Data
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID
A137ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, LPA137DATA lpaData)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! A137CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Data by New Index ----- */

            A137RedrawText(hDlg, lpaData + (wGunNo * A137_GUN_MAX) 
                           + (wType * A137_TYPE_MAX));

            /* ----- Redraw Scroll Box (Thumb) ----- */

            A137RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A137_ONE_PAGE;

        /* ----- Redraw Description & Data by New Index ----- */

        A137RedrawText(hDlg, lpaData + (wGunNo * A137_GUN_MAX) 
                       + (wType * A137_TYPE_MAX));

        /* ----- Redraw Scroll Box (Thumb) ----- */

        A137RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A137CalcIndex()
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
BOOL    A137CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= A137_ONE_LINE;

        if (unBottomIndex < A137_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = A137_ONE_PAGE;
            unTopIndex    = A137_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= A137_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A137_ONE_LINE;

        if (unBottomIndex >= A137_DRINK_MAX) {     /* Scroll to Bottom */

            unBottomIndex = (A137_DRINK_MAX - 1);
            unTopIndex    = A137_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += A137_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A137_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= A137_ONE_PAGE;

            if (unBottomIndex <= A137_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = A137_ONE_PAGE;
                unTopIndex    = A137_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= A137_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A137_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += A137_ONE_PAGE;

            if (unBottomIndex >= A137_DRINK_MAX) {     /* Scroll to Bottom */

                unBottomIndex = (A137_DRINK_MAX - 1);
                unTopIndex    = A137_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += A137_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137RedrawText()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPA137DATA   lpaData -   Address of the Data Buffer
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & data to each control.
* ===========================================================================
*/
VOID    A137RedrawText(HWND hDlg, LPA137DATA lpaData)
{
    UINT    unCount,
            unLoop = 0;
    WCHAR    szWork[A137_DESC_LEN],
            szDesc[A137_DESC_LEN];
    
    /* ----- Load Strings from Resource ----- */

    LoadString(hResourceDll, IDS_A137_DESC, szWork, PEP_STRING_LEN_MAC(szWork));

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Create Description with Address No. ----- */

        wsPepf(szDesc, szWork, (unCount + 1)/10, (unCount + 1)%10);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_A137_DESC01 + unLoop, szDesc);
    }

    for (unCount = 0, unLoop = unTopIndex; unCount < A137_DRINK_MAX; unCount++, unLoop++) {

        /* ----- Set Data to EditText ----- */

        SetDlgItemInt(hDlg,
                      IDD_A137_PLU01 + unCount,
                      (UINT)(lpaData + unLoop)->wPlu,
                      FALSE);

        SetDlgItemInt(hDlg,
                      IDD_A137_ADJ01 + unCount,
                      (UINT)(lpaData + unLoop)->bAdj,
                      FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A137RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137SetGunData()
*
**  Input       :   HWND         hDlg        -   Window Handle of a DialogBox
*                   LPA137DATA   lpaData     -   Address of the Data Buffer
*
**  Return      :   WORD        wGun         -   Current Selected Gun No.
*
**  Description :
*       This function gets the number of current selected Gun. And 
*       sets the PLU Number of a selected Gun to edittext.
*       It returns the number of current selected Gun.
* ===========================================================================
*/
VOID    A137SetGunData(HWND hDlg, LPA137DATA lpaData)
{

    unTopIndex    = A137_SCBAR_MIN;
    unBottomIndex = A137_ONE_PAGE;

    /* ----- Get Gun No. from ComboBox ----- */

    wGunNo = (WORD)SendDlgItemMessage(hDlg, IDD_A137_GUN,
                                       CB_GETCURSEL,
                                       0, 0L);

    /* ----- Set Description to StaticText ----- */

    A137RedrawText(hDlg, lpaData + (wGunNo * A137_GUN_MAX) 
                   + (wType * A137_TYPE_MAX));

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A137_SCBAR),
                 SB_CTL, A137_SCBAR_MIN, TRUE);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A137_PLU01));
    SendDlgItemMessage(hDlg, IDD_A137_PLU01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A137SetTypeData()
*
**  Input       :   HWND         hDlg        -   Window Handle of a DialogBox
*                   LPA137DATA   lpaData     -   Address of the Data Buffer
*
**  Return      :   WORD        wType        -   Current Selected Type
*
**  Description :
*       This function gets the number of current selected type. And 
*       sets the PLU Number of a selected type to edittext.
*       It returns the number of current selected type.
* ===========================================================================
*/
VOID    A137SetTypeData(HWND hDlg, LPA137DATA lpaData)
{

    unTopIndex    = A137_SCBAR_MIN;
    unBottomIndex = A137_ONE_PAGE;

    /* ----- Get Gun No. from ComboBox ----- */

    wType = (WORD)SendDlgItemMessage(hDlg, IDD_A137_TYPE,
                                       CB_GETCURSEL,
                                       0, 0L);

    /* ----- Set Description to StaticText ----- */

    A137RedrawText(hDlg, lpaData + (wGunNo * A137_GUN_MAX) 
                   + (wType * A137_TYPE_MAX));

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A137_SCBAR),
                 SB_CTL, A137_SCBAR_MIN, TRUE);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A137_PLU01));
    SendDlgItemMessage(hDlg, IDD_A137_PLU01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/* ===== End of A137.C ===== */
