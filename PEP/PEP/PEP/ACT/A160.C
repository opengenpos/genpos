/*
* ---------------------------------------------------------------------------
* Title         :   Assignment Terminal No. (AC 160)
* Category      :   Maintenance, NCR 2170 PEP for Windows (HP US Enhance Model)
* Program Name  :   A160.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
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
* Mar-28-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
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

#include    "ecr.h"
#include    "paraequ.h"
#include    "para.h"

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a160.h"

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
static  UINT         unTopIndex,    /* Zero Based Top Index No. of ListBox      */
                     unBottomIndex; /* Zero Based Bottom Index No. of ListBox   */
static  LPA160DATA   lpOepPara;     /* Address of Order Entry Prompt Data Buffer    */

static  HWND  hWndModeLess = NULL;


/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A160DlgProc()
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
*       Order Entry Prompt Table# Table Maintenance.
* ===========================================================================
*/
BOOL    WINAPI  A160DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{

    static  UINT    unMaxOep;   /* Maximum Number of Order Entry Prompt Data */
    static  HGLOBAL hOepMem;    /* Handle of Global Heap                */

    USHORT  usReturnLen;                    /* Return Length of ParaAll */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Global Heap ----- */

        if (A160AllocMem(hDlg, (LPHGLOBAL)&hOepMem)) {

			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));

        } else {

            /* ----- Initialize Configulation of DialogBox ----- */

            A160InitDlg(hDlg);

        }

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A160_DESC01; j<=IDD_A160_D810_SPIN; j++)
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
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_A160_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				A160ScrlProc(hDlg, SB_LINEDOWN, hScroll);
			} else if (dWheel > 0) {
				A160ScrlProc(hDlg, SB_LINEUP, hScroll);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:

        if (GetDlgCtrlID((HWND)lParam) == IDD_A160_SCBAR) {

            /* ----- Vertical Scroll Bar Control ----- */

            A160ScrlProc(hDlg, wParam, lParam);

        } else {

            /* ----- Spin Button Control ----- */

            A160SpinProc(hDlg, wParam, lParam);

        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A160_D101:
        case IDD_A160_D102:
        case IDD_A160_D103:
        case IDD_A160_D104:
        case IDD_A160_D105:
        case IDD_A160_D106:
        case IDD_A160_D107:
        case IDD_A160_D108:
        case IDD_A160_D109:
        case IDD_A160_D110:
        case IDD_A160_D201:
        case IDD_A160_D202:
        case IDD_A160_D203:
        case IDD_A160_D204:
        case IDD_A160_D205:
        case IDD_A160_D206:
        case IDD_A160_D207:
        case IDD_A160_D208:
        case IDD_A160_D209:
        case IDD_A160_D210:
        case IDD_A160_D301:
        case IDD_A160_D302:
        case IDD_A160_D303:
        case IDD_A160_D304:
        case IDD_A160_D305:
        case IDD_A160_D306:
        case IDD_A160_D307:
        case IDD_A160_D308:
        case IDD_A160_D309:
        case IDD_A160_D310:
        case IDD_A160_D401:
        case IDD_A160_D402:
        case IDD_A160_D403:
        case IDD_A160_D404:
        case IDD_A160_D405:
        case IDD_A160_D406:
        case IDD_A160_D407:
        case IDD_A160_D408:
        case IDD_A160_D409:
        case IDD_A160_D410:
        case IDD_A160_D501:
        case IDD_A160_D502:
        case IDD_A160_D503:
        case IDD_A160_D504:
        case IDD_A160_D505:
        case IDD_A160_D506:
        case IDD_A160_D507:
        case IDD_A160_D508:
        case IDD_A160_D509:
        case IDD_A160_D510:
        case IDD_A160_D601:
        case IDD_A160_D602:
        case IDD_A160_D603:
        case IDD_A160_D604:
        case IDD_A160_D605:
        case IDD_A160_D606:
        case IDD_A160_D607:
        case IDD_A160_D608:
        case IDD_A160_D609:
        case IDD_A160_D610:
        case IDD_A160_D701:
        case IDD_A160_D702:
        case IDD_A160_D703:
        case IDD_A160_D704:
        case IDD_A160_D705:
        case IDD_A160_D706:
        case IDD_A160_D707:
        case IDD_A160_D708:
        case IDD_A160_D709:
        case IDD_A160_D710:
        case IDD_A160_D801:
        case IDD_A160_D802:
        case IDD_A160_D803:
        case IDD_A160_D804:
        case IDD_A160_D805:
        case IDD_A160_D806:
        case IDD_A160_D807:
        case IDD_A160_D808:
        case IDD_A160_D809:
        case IDD_A160_D810:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Data with Data Range ----- */

                if (A160GetData(hDlg, wParam)) {

                    /* ----- Text out Error Message ----- */

                    A160DispErr(hDlg);

                    /* ----- Reset Previous Data ----- */

                    A160ResetData(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    ParaAllWrite(CLASS_PARAOEPTBL,
                                 (UCHAR *)lpOepPara,
                                 sizeof(A160DATA),
                                 0, &usReturnLen);

                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
                }

                /* ----- Frees Allocated Memory ----- */

                A160FreeMem((LPHGLOBAL)&hOepMem);


                EndDialog(hDlg, LOWORD(wParam));

                return TRUE;
            }
        }
        return FALSE;

	case WM_DESTROY:
		hWndModeLess = NULL;
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A160AllocMem()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPHGLOBAL   lphOep  -   Address of Global Heap Handle
*
**  Return      :   BOOL    TRUE    -   Memory Allocation is Failed.
*                           FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This function allocates the data buffer from the global heap.
*   It returns TRUE, if it failed in momory allocation. Otherwise it returns 
*   FALSE.
* ===========================================================================
*/
BOOL   A160AllocMem(HWND hDlg, LPHGLOBAL lphOep)
{
    BOOL    fRet = FALSE;
    WCHAR    szCaption[PEP_CAPTION_LEN],
            szErrMsg[PEP_ALLOC_LEN];

    /* ----- Allocate Memory from Global Heap ----- */

    *lphOep  = GlobalAlloc(GHND, sizeof(A160DATA));

    /* ----- Determine Whether Memory Allocation is Success or Not ----- */

    if (! *lphOep) {

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A160, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,    szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szErrMsg,
                   szCaption,
                   MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;

        /* ----- Set the Allocated Area Free ----- */

        if (*lphOep) {

            GlobalFree(*lphOep);
        }

    } else {

        /* ----- Lock the Allocated Area ----- */

        lpOepPara  = (LPA160DATA)GlobalLock(*lphOep);
    }
    return fRet;
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A160FreeMem()
*
**  Input       :   LPHGLOBAL   lphOep  -   Address of Global Heap Handle
*
**  Return      :   No return value.
*
**  Description :
*       This function frees the allocated memory from global heap.
* ===========================================================================
*/
VOID    A160FreeMem(LPHGLOBAL lphOep)
{
    /* ----- Unlock the Allocated Area ----- */

    GlobalUnlock(*lphOep);

    /* ----- Free the Allocated Area ----- */

    GlobalFree(*lphOep);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160InitDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*
**  Return      :   No Return Value
*
**  Description :
*       This function initializes the configulation of a DialogBox.
* ===========================================================================
*/
VOID    A160InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;
    UINT    unCount,
            unID,
            unID2;


    /* ----- Set Top Index & Bottom Index ----- */

    unTopIndex    = A160_SCBAR_MIN;
    unBottomIndex = A160_ONE_PAGE;

    unID = IDD_A160_D101;
    for (unCount = 0;  unCount < A160_MAX_PAGE;
                    unCount++, unID = unID + A160_EDIT_OFFSET) {

        for (unID2 = 0; unID2 < MAX_TABLE_ADDRESS; unID2++){

			/* ----- Set Limit Length to EditText ----- */

            SendDlgItemMessage(hDlg, unID + unID2,
                               EM_LIMITTEXT, A160_DATA_LEN, 0L);

        }

    }

    /* ----- Read Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAOEPTBL,
                (UCHAR *)lpOepPara,
                sizeof(A160DATA),
                0, &usReturnLen);

    /* ----- Set Description to StaticText ----- */

    A160RedrawText(hDlg);

    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */

    SetScrollRange(GetDlgItem(hDlg, IDD_A160_SCBAR),
                   SB_CTL, A160_SCBAR_MIN, A160_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */

    SetScrollPos(GetDlgItem(hDlg, IDD_A160_SCBAR),
                 SB_CTL, A160_SCBAR_MIN, FALSE);

    /* ----- Set Focus to EditText in the Top of Item ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A160_D101));
    SendDlgItemMessage(hDlg, IDD_A160_D101, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A160GetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Control ID of Current Changed EditText  
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
BOOL    A160GetData(HWND hDlg, WPARAM wParam)
{
    UINT    unValue;            /* Value of Inputed Data */
    UINT    unTable,
            unGroup;
    BOOL    fReturn = FALSE;    /* Return Value of This Function */

    /* ----- Get Inputed Data from EditText ----- */

    unValue = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
    
    if (A160_DATA_MAX < unValue) {

        fReturn = TRUE;

    } else {

        /* ----- Set Inputed Data to a Data Buffer ----- */

        unTable = (((int)LOWORD(wParam) - IDD_A160_D101) / A160_EDIT_OFFSET)
                                                          + unTopIndex;
        unGroup = ((int)LOWORD(wParam) - IDD_A160_D101) -
                      ((unTable - unTopIndex) * A160_EDIT_OFFSET);
        (lpOepPara)->ParaOep[unTable][unGroup] = (BYTE)unValue;

    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function shows a MessageBox for displaying error message.
* ===========================================================================
*/
VOID    A160DispErr(HWND hDlg)
{
    WCHAR    szCaption[PEP_CAPTION_LEN],     /* MessageBox Caption       */
            szMessage[PEP_OVER_LEN],        /* Error Message Strings    */
            szWork[PEP_OVER_LEN];           /* Error Message Work Area  */

    /* ----- Load MessageBox Caption from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A160, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szMessage, szWork, A160_DATA_MIN, A160_DATA_MAX);

    /* ----- Text out MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160ResetData()
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
VOID    A160ResetData(HWND hDlg, WPARAM wParam)
{
    /* ----- Undo Input Data ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);

    /* ----- Set Focus to Error EditText ----- */

    SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 1, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160SpinProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    A160SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    int     idEdit = 0;
    PEPSPIN PepSpin;

    PepSpin.lMin       = (long)A160_DATA_MIN;
    PepSpin.lMax       = (long)A160_DATA_MAX;
    PepSpin.nStep      = A160_SPIN_STEP;
    PepSpin.nTurbo     = A160_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A160_SPIN_OFFSET);

    if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

        A160GetData(hDlg, (WORD)idEdit);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160ScrlProc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   Specifies a Scroll Bar Code
*                   LONG        lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description:
*       This function is controls Vertical Scroll bar.
* ===========================================================================
*/
VOID A160ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        if (! A160CalcIndex(LOWORD(wParam))) {

            /* ----- Redraw Description & Data by New Index ----- */

            A160RedrawText(hDlg);

            /* ----- Redraw Scroll Box (Thumb) ----- */

            A160RedrawThumb(lParam);
        }
        break;

    case SB_THUMBTRACK:

        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */

        unTopIndex    = HIWORD(wParam);
        unBottomIndex = unTopIndex + A160_ONE_PAGE;

        /* ----- Redraw Description & Data by New Index ----- */

        A160RedrawText(hDlg);

        /* ----- Redraw Scroll Box (Thumb) ----- */

        A160RedrawThumb(lParam);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A160CalcIndex()
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
BOOL    A160CalcIndex(WORD wScrlCode)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
        unBottomIndex -= A160_ONE_LINE;

        if (unBottomIndex < A160_ONE_PAGE) {     /* Scroll to Top */

            unBottomIndex = A160_ONE_PAGE;
            unTopIndex    = A160_SCBAR_MIN;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line up */

            unTopIndex   -= A160_ONE_LINE;
        }
        break;

    case SB_LINEDOWN:
        unBottomIndex += A160_ONE_LINE;

        if (unBottomIndex >= A160_ADDR_NO) {     /* Scroll to Bottom */

            unBottomIndex = (A160_ADDR_NO - 1);
            unTopIndex    = A160_SCBAR_MAX;
            fReturn       = TRUE;

        } else {                                /* Scroll to One Line down */

            unTopIndex   += A160_ONE_LINE;
        }
        break;

    case SB_PAGEUP:
        if (unTopIndex != A160_SCBAR_MIN) {  /* Current Position isn't  Top */

            unBottomIndex -= A160_ONE_PAGE;

            if (unBottomIndex <= A160_ONE_PAGE) {    /* Scroll to Top */

                unBottomIndex = A160_ONE_PAGE;
                unTopIndex    = A160_SCBAR_MIN;

            } else {                                /* Scroll to One Page */

                unTopIndex    -= A160_ONE_PAGE;
            }
        } else {                            /* Current Position is Top */

            fReturn = TRUE;
        }
        break;

    case SB_PAGEDOWN:
        if (unTopIndex != A160_SCBAR_MAX) {  /* Current Position isn't Bottom */

            unBottomIndex += A160_ONE_PAGE;

            if (unBottomIndex >= A160_ADDR_NO) {     /* Scroll to Bottom */

                unBottomIndex = (A160_ADDR_NO - 1);
                unTopIndex    = A160_SCBAR_MAX;

            } else {                                /* Scroll to One Line down */

                unTopIndex   += A160_ONE_PAGE;
            }
        } else {                            /* Current Position is Bottom */

            fReturn = TRUE;
        }
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160RedrawText()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws description & data to each control.
* ===========================================================================
*/
VOID    A160RedrawText(HWND hDlg)
{
    UINT    unCount,
            unLoop = 0;
    UINT    unID,
            unID2;
    WCHAR    szWork[A160_DESC_LEN],
            szDesc[A160_DESC_LEN];
    
    /* ----- Load Strings from Resource ----- */

    LoadString(hResourceDll, IDS_A160_DESC, szWork, PEP_STRING_LEN_MAC(szWork));

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {

        /* ----- Create Description with Address No. ----- */

        wsPepf(szDesc, szWork, (unCount + 1)/10, (unCount + 1)%10);

        /* ----- Set Loaded Strings to StaticText ----- */

        DlgItemRedrawText(hDlg, IDD_A160_DESC01 + unLoop, szDesc);
    }

    unID = IDD_A160_D101;
    for (unCount = 0, unLoop = unTopIndex; unCount < A160_MAX_PAGE; unCount++, unLoop++) {

        for (unID2 = 0; unID2 < MAX_TABLE_ADDRESS; unID2++) { 

            /* ----- Set Data to EditText ----- */

            SetDlgItemInt(hDlg,
                          (unID + unID2),
                          (UINT)(lpOepPara->ParaOep[unLoop][unID2]),
                          FALSE);

        }

        unID = unID + A160_EDIT_OFFSET;

    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A160RedrawThumb()
*
**  Input       :   LPARAM    lCtrl   -   Window Handle of a Vertical Scroll
*
**  Return      :   No Return Value.
*
**  Description :
*       This function redraws scroll box (thumb) with new position.
* ===========================================================================
*/
VOID    A160RedrawThumb(LPARAM lCtrl)
{
    /* ----- Redraw Thumb to New Position ----- */

    SetScrollPos((HWND)lCtrl, SB_CTL, (int)unTopIndex, TRUE);

}


HWND A160ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A160),
                                   hParentWnd,
                                   A160DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A160ModeLessDialogWnd (void)
{
	return hWndModeLess;
}


/* ===== End of A160.C ===== */
