/*
* ---------------------------------------------------------------------------
* Title         :   Set Color Palette (AC 209) 
* Category      :   Enhancement for Release 2.1
* Program Name  :   A209.C
* ---------------------------------------------------------------------------
* ---------------------------------------------------------------------------
* Abstract : 
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Jul-20-06 : 00.00.01 :            : Initial
*
* ===========================================================================
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
#include    "a209.h"

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
static	RECT	backgroundRect = {170,50,370,100};
static NHPOSCOLORREF	workingColor = 0,
						paraWriteColor[MAX_COLORPALETTE_SIZE][2];
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/
int repaintSample = 0;
int	colorDlgButton = 0;

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A209DlgProc()
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
*               Set Color Palette (Action Code # 209)
* ===========================================================================
*/
BOOL    WINAPI  A209DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;

    WCHAR    szDesc[128];
    USHORT  usReturnLen;
	int     unI = 0;
	WORD	wI;
	LONG lError;

	// Get the currently selected palette address from combo box
	wI = (WORD)SendDlgItemMessage(hDlg, IDD_A209_COMBO_ADR, CB_GETCURSEL, 0, 0L);

	// Draw the sample text area to display palette color selections
	A209DrawSample(hDlg, wI);

	//repaintSample++;

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A209_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);

        LoadString(hResourceDll, IDS_A209_PALETTE_ADR, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A209_STRPALETTE_ADR, szDesc);
        LoadString(hResourceDll, IDS_A209_FOREGROUND, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A209_STRFOREGROUND, szDesc);
        LoadString(hResourceDll, IDS_A209_BACKGROUND, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A209_STRBACKGROUND, szDesc);
        LoadString(hResourceDll, IDS_A209_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A209_STRDESC, szDesc);
        LoadString(hResourceDll, IDS_A209_NOTE, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A209_STRNOTE, szDesc);

		/* ----- Insert Strings into ComboBox ----- */
		wsPepf(szDesc, WIDE("Select #"));
		DlgItemSendTextMessage(hDlg, IDD_A209_COMBO_ADR, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
		for (wI = 1; wI <= A209_PALETTE_MAX; wI++) {
			wsPepf(szDesc, WIDE("%u"), wI);
			DlgItemSendTextMessage(hDlg, IDD_A209_COMBO_ADR, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
		}
		// Set Initial selection to top index, the first selection is '1' and acts as a default,
		SendDlgItemMessage(hDlg, IDD_A209_COMBO_ADR, CB_SETCURSEL, A209_PALETTE_MIN, FALSE);

		// Limit length of typed input
		SendDlgItemMessage(hDlg, IDD_A209_COMBO_ADR,CB_LIMITTEXT, A209_PALETTE_LEN, 0L);

		/* ----- Read Initial Data from Parameter File ----- */
        ParaAllRead(CLASS_PARACOLORPALETTE, (UCHAR *)paraWriteColor, sizeof(paraWriteColor), 0, &usReturnLen);
/*        for (unI = 0; unI < A209_ADDR_MAX; unI++) {

*/        
            /* ----- Set Initial Data to EditText ----- */

/*            SetDlgItemInt(hDlg, IDD_A209_PALETTE_ADR + unI, aunData[unI], FALSE);

        }

        /* ----- Set Focus to EditText ----- */

/*        SendDlgItemMessage(hDlg, IDD_A209_PALETTE_ADR, EM_SETSEL, 1, MAKELONG(0, -1));
*/
		return TRUE;

    case WM_COMMAND:
		// need to repaint when a command is entered
		repaintSample = 0;

        switch (LOWORD(wParam)) {
		case IDD_A209_COMBO_ADR:
			wI = (WORD)SendDlgItemMessage(hDlg, IDD_A209_COMBO_ADR, CB_GETCURSEL, 0, 0L);
			if (wI == 0)
			{
                EnableWindow(GetDlgItem(hDlg, IDD_A209_FOREGROUND_SET), FALSE);
                EnableWindow(GetDlgItem(hDlg, IDD_A209_BACKGROUND_SET), FALSE);
			} else {
                EnableWindow(GetDlgItem(hDlg, IDD_A209_FOREGROUND_SET), TRUE);
                EnableWindow(GetDlgItem(hDlg, IDD_A209_BACKGROUND_SET), TRUE);
			}
			return TRUE;

        case IDD_A209_FOREGROUND_SET:
        case IDD_A209_BACKGROUND_SET:
			if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Create Control Code DialogBox ----- */
                DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_A209), hDlg, A209ColorProc);
				lError = GetLastError();

				wI = (WORD)SendDlgItemMessage(hDlg, IDD_A209_COMBO_ADR, CB_GETCURSEL, 0, 0L);
				if (colorDlgButton == A209_PALETTE_OK)
				{
					switch(LOWORD(wParam)) {
						case IDD_A209_FOREGROUND_SET:
							paraWriteColor[wI][0] = workingColor;
							break;

						case IDD_A209_BACKGROUND_SET:
							paraWriteColor[wI][1] = workingColor;
							break;
						default:
							break;
					}
				}
			}

			SetFocus(hDlg);
			return TRUE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    // ----- Write data to parameter file ----- 
                    ParaAllWrite(CLASS_PARACOLORPALETTE, (UCHAR *)paraWriteColor, sizeof(paraWriteColor), 0, &usReturnLen);

                    // ----- Set modifier flag ----- 
                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
                }

                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
        }// end switch(LOWORD(wParam))

        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A209DispErr()
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
VOID    A209DispErr(HWND hDlg, WPARAM wID)
{
    WORD    wMin = A209_DATA_MIN,
            wMax = A209_DATA_MAX;
    WCHAR   szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_A209_CAPTION, szCaption, PEP_CAPTION_LEN);
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wsPepf(szMessage, szWork, wMin, wMax);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */
  	SendDlgItemMessage(hDlg, LOWORD(wID), EM_UNDO, 0, 0L);
	SetFocus(GetDlgItem(hDlg, wID));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A209DrawSample()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wIndex  -   Current Selected Index
*
**  Return      :   No return values
*
**  Description :
*       This function draws the Sample area, to show the selected colors for
*		the selected palette address (combobox).
* ===========================================================================
*/
VOID    A209DrawSample(HWND hDlg, WORD wIndex)
{
	WCHAR       *pSampleText = L"  SampleText  ";
	HDC			hDC = GetDC(hDlg);
	LPRECT		pBackgroundRect = &backgroundRect;
	HBRUSH		solidBrush;
	COLORREF	backgroundColor = paraWriteColor[wIndex][1];
	COLORREF	foregroundColor = paraWriteColor[wIndex][0];

	//SetROP2(hDC, R2_NOT);
	solidBrush = CreateSolidBrush(backgroundColor);
	FillRect(hDC, &backgroundRect, solidBrush);
	DeleteObject(solidBrush);

	SetBkColor(hDC, backgroundColor);
	SetTextColor(hDC, foregroundColor);

	DrawText(hDC, pSampleText, wcslen (pSampleText), pBackgroundRect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);

	ReleaseDC(hDlg, hDC);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A209ColorProc()
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
*                       PLU Maintenance Control Code.
* ===========================================================================
*/
BOOL    WINAPI  A209ColorProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	CHOOSECOLOR colorSelection;			// common dialog box structure
	static COLORREF acrCustClr[16];		// array of customer colors
	static DWORD rgbCurrent;			// initial color selection

    switch (wMsg) 
	{
		case    WM_INITDIALOG:
			memset(&colorSelection, 0x00, sizeof(CHOOSECOLOR));

			colorSelection.lStructSize = sizeof(CHOOSECOLOR);
			colorSelection.hwndOwner = NULL;
			colorSelection.lpCustColors = (LPDWORD) acrCustClr;
			colorSelection.rgbResult = rgbCurrent;
			//colorSelection.Flags = CC_SOLIDCOLOR;
			colorSelection.Flags = CC_FULLOPEN;
			
			if (ChooseColor(&colorSelection) )
			{
				DWORD dlgError = CommDlgExtendedError();

				switch (dlgError) {
				case CDERR_DIALOGFAILURE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_FINDRESFAILURE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_INITIALIZATION:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_LOADRESFAILURE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_LOCKRESFAILURE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_MEMALLOCFAILURE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_NOHINSTANCE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_NOHOOK:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_NOTEMPLATE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				case CDERR_STRUCTSIZE:
					colorSelection.Flags = CC_FULLOPEN;
					return FALSE;
				default:
					workingColor = colorSelection.rgbResult;
					colorDlgButton = A209_PALETTE_OK;
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}			
			} else {
				colorDlgButton = A209_PALETTE_CANCEL;
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
        
			/* ----- Set Current Selected PLU Data to Each Controls ----- */
			//A068InitData(hDlg, IDD_A68_CTRL);

			SetFocus(GetDlgItem(hDlg, IDD_A209));

			return TRUE;
    }

    return FALSE;
}


/* ===== End of A209.C ===== */
