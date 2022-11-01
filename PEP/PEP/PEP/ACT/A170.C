/*
* ---------------------------------------------------------------------------
* Title         :   Set Sales Restriction Table of PLU (AC 170)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A170.C
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
* Nov-17-99 : 03.03.00 :            : NCR2172
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
#include    "a170.h"

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

static  HWND  hWndModeLess = NULL;


/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

#define KBITS_RESTRICT_OVERRIDE_ANDOR  (RESTRICT_OVERRIDE_ANDOR << 4)
#define KBITS_RESTRICT_OVERRIDE_AND    (RESTRICT_OVERRIDE_AND << 4)
#define KBITS_RESTRICT_DAY_MASK        0x00FF

USHORT RestrictOverrideAndToKbits (UCHAR  uchDate)
{
	USHORT  kBits = 0;

	kBits |= ((USHORT)uchDate & (RESTRICT_OVERRIDE_ANDOR | RESTRICT_OVERRIDE_AND)) << 4;

	return kBits;
}

UCHAR RestrictOverrideAndFromKbits (USHORT usKbits)
{
	UCHAR  uchDate = 0;

	if (usKbits & (KBITS_RESTRICT_OVERRIDE_ANDOR | KBITS_RESTRICT_OVERRIDE_AND)) {
		uchDate |= (usKbits & (KBITS_RESTRICT_OVERRIDE_ANDOR | KBITS_RESTRICT_OVERRIDE_AND)) >> 4;
	}

	return uchDate;
}

static struct {
	int   iId;
	HWND  hWnd;
} A170DlgTabs[10] = { {0, 0} };

BOOL    WINAPI  A170DlgChildProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
        return FALSE;
}

void  A170ModeLessChildDialogClear ()
{
	memset (A170DlgTabs, 0, sizeof(A170DlgTabs));
}

HWND A170ModeLessChildDialog (HWND hParentWnd, int nCmdShow, int iId)
{
	int   i;
	HWND  hWnd = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A170_DAYS),
                                   hParentWnd,
                                   A170DlgChildProc);
	hWnd && ShowWindow (hWnd, nCmdShow);
	for (i = 0; i < sizeof(A170DlgTabs)/sizeof(A170DlgTabs[0]); i++) {
		if (A170DlgTabs[i].hWnd == 0) {
			A170DlgTabs[i].iId = iId;
			A170DlgTabs[i].hWnd = hWnd;
			break;
		}
	}

	return hWnd;
}

HWND A170ModeLessChildDialogFind (int iId)
{
	int   i;
	HWND  hWnd = NULL;

	for (i = 0; i < sizeof(A170DlgTabs)/sizeof(A170DlgTabs[0]); i++) {
		if (A170DlgTabs[i].iId == iId) {
			hWnd = A170DlgTabs[i].hWnd;
			break;
		}
	}

	return hWnd;
}

USHORT A170ModeLessChildDialogSettings (int iId)
{
	int     i;
	USHORT  iBits = 0, kBits = 1;
	HWND hWnd = A170ModeLessChildDialogFind (iId);

	// least significant byte contains the bit mask for the days of the week.
	// the next higher byte contains the indicators for the override type or
	// whether MDC 298 is to be overriden or not.
	for (i = IDD_A170_CAPTION1; i <= IDD_A170_CAPTION7; i++, (kBits <<= 1)) {
		iBits |= IsDlgButtonChecked (hWnd, i) ? kBits : 0;
	}

	iBits |= iBits ? RESTRICT_WEEK_DAYS_ON : 0;

	iBits |= IsDlgButtonChecked(hWnd, IDD_A170_STR1) ? KBITS_RESTRICT_OVERRIDE_ANDOR : 0;
	iBits |= IsDlgButtonChecked(hWnd, IDD_A170_STR2) ? KBITS_RESTRICT_OVERRIDE_AND : 0;

	return iBits;
}


USHORT A170ModeLessChildDialogSettingsSetMask (int iId, USHORT  usMask)
{
	int     i;
	USHORT  k = 1;
	HWND    hWnd = A170ModeLessChildDialogFind (iId);

	CheckDlgButton(hWnd, IDD_A170_STR1, (usMask & KBITS_RESTRICT_OVERRIDE_ANDOR) ? TRUE : FALSE);
	CheckDlgButton(hWnd, IDD_A170_STR2, (usMask & KBITS_RESTRICT_OVERRIDE_AND) ? TRUE : FALSE);

	for (i = IDD_A170_CAPTION1; i <= IDD_A170_CAPTION7; i++, (k <<= 1)) {
		CheckDlgButton(hWnd, i, (usMask & k) ? TRUE : FALSE);
	}

	return usMask;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A170DlgProc()
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
*                   Set Rounding Table (Action Code # 84)
* ===========================================================================
*/
BOOL    WINAPI  A170DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {

    case    WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize Configulation of DialogBox ----- */
        A170InitDlg(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A170_DATE1; j<=IDD_A170_CAPTION10; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        /* ----- Spin Button Procedure ----- */
        A170SpinProc(hDlg, wParam, lParam);
        return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
		// Date part of rules, Address #1 through #6
        case    IDD_A170_DATE1:
        case    IDD_A170_DATE2:
        case    IDD_A170_DATE3:
        case    IDD_A170_DATE4:
        case    IDD_A170_DATE5:
        case    IDD_A170_DATE6:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A170ChkDatePos(hDlg, wParam)) {
                    /* ----- Display Error Message ----- */
                    A170DispErr(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

		// Week part of rules, Address #1 through #6
        case    IDD_A170_WEEK1:
        case    IDD_A170_WEEK2:
        case    IDD_A170_WEEK3:
        case    IDD_A170_WEEK4:
        case    IDD_A170_WEEK5:
        case    IDD_A170_WEEK6:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A170ChkWeekPos(hDlg, wParam)) {
                    /* ----- Display Error Message ----- */
                    A170DispErr(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

		// Start hour and End hour part of rules, Address #1 through #6
        case    IDD_A170_SHOUR1:
        case    IDD_A170_SHOUR2:
        case    IDD_A170_SHOUR3:
        case    IDD_A170_SHOUR4:
        case    IDD_A170_SHOUR5:
        case    IDD_A170_SHOUR6:
        case    IDD_A170_EHOUR1:
        case    IDD_A170_EHOUR2:
        case    IDD_A170_EHOUR3:
        case    IDD_A170_EHOUR4:
        case    IDD_A170_EHOUR5:
        case    IDD_A170_EHOUR6:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A170ChkHourPos(hDlg, wParam)) {
                    /* ----- Display Error Message ----- */
                    A170DispErr(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;
            
		// Start minute and End minute part of rules, Address #1 through #6
        case    IDD_A170_SMINUTE1:
        case    IDD_A170_SMINUTE2:
        case    IDD_A170_SMINUTE3:
        case    IDD_A170_SMINUTE4:
        case    IDD_A170_SMINUTE5:
        case    IDD_A170_SMINUTE6:
        case    IDD_A170_EMINUTE1:
        case    IDD_A170_EMINUTE2:
        case    IDD_A170_EMINUTE3:
        case    IDD_A170_EMINUTE4:
        case    IDD_A170_EMINUTE5:
        case    IDD_A170_EMINUTE6:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Data with Data Range ----- */
                if (A170ChkMinutePos(hDlg, wParam)) {
                    /* ----- Display Error Message ----- */
                    A170DispErr(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;
            
        case    IDOK:
        case    IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Get Inputed Data from EditText ----- */
                    if (A170SetData(hDlg)) {
						A170ModeLessChildDialogClear();
                        return TRUE;
                    }
                }
				A170ModeLessChildDialogClear();
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
**  Synopsis    :   VOID    A170InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initilal rounding data from Parameter File, and
*   then it sets them to specified position of edittext.
* ===========================================================================
*/
VOID    A170InitDlg(HWND hDlg)
{
	HWND     hWnd;
    RESTRICT  restTbl[MAX_REST_SIZE];
    UINT    unI, unValue;
    USHORT  usReturnLen;

    /* ----- set initial description ----- */
	{
		int      i;
		WCHAR    szDesc[128];

		LoadString(hResourceDll, IDS_A170_DESC1, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR1, szDesc);
		LoadString(hResourceDll, IDS_A170_DESC2, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR2, szDesc);
		LoadString(hResourceDll, IDS_A170_DESC3, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR3, szDesc);
		LoadString(hResourceDll, IDS_A170_DESC4, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR4, szDesc);
		LoadString(hResourceDll, IDS_A170_DESC5, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR5, szDesc);
		LoadString(hResourceDll, IDS_A170_DESC6, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A170_STR6, szDesc);

		// set the MDC checkbox text
		DlgItemRedrawText(hDlg, IDD_A170_MDC_PLU5_ADR, PepFetchMdcMnemonic (MDC_PLU5_ADR, EVEN_MDC_BIT1, 1, szDesc, PEP_STRING_LEN_MAC(szDesc)));

		// clear the child dialog table to set to a known state and
		// then lets create our child dialogs for the multiple days.
		// we do this clear here in case the AC170 dialog has been
		// opened through the AC68 dialog since closing that will bypass
		// the standard OnOk() closing functionality of this dialog
		// when used with the AC68 dialog as a modeless dialog.
		A170ModeLessChildDialogClear();
		for (i = IDD_A170_DAYS_1; i <= IDD_A170_DAYS_6; i++) {
			hWnd = GetDlgItem (hDlg, i);
			hWnd = A170ModeLessChildDialog (hWnd, 1, i);
		}
	}

    /* ---- Read Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARARESTRICTION, (UCHAR *)restTbl, sizeof(restTbl), 0, &usReturnLen);

    for (unI = 0; unI < MAX_REST_SIZE; unI++) {
		USHORT  usKbits = 0;

        /* ----- Limit Length of Input Data to EditText (Date) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_DATE1 + unI, EM_LIMITTEXT, A170_MAX_LEN, 0L);
        /* ----- Set (Date) Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchDate;
		if (unValue & (RESTRICT_OVERRIDE_ANDOR | RESTRICT_OVERRIDE_AND)) {
			usKbits = RestrictOverrideAndToKbits (unValue);
			unValue = 0;
		}
        SetDlgItemInt(hDlg, IDD_A170_DATE1 + unI, unValue, FALSE);

        /* ----- Limit Length of Input Data to EditText (Day of the week) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_WEEK1 + unI, EM_LIMITTEXT, A170_MAX_WEEKLEN, 0L);
        /* ----- Set (Day of the week) Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchDay;
		if (unValue & RESTRICT_WEEK_DAYS_ON) {
			usKbits |= unValue;
			unValue = 0;
		}
        SetDlgItemInt(hDlg, IDD_A170_WEEK1 + unI, unValue, FALSE);
		A170ModeLessChildDialogSettingsSetMask (unI + IDD_A170_DAYS_1, usKbits);

        /* ----- Limit Length of Input Data to EditText (Start Hour) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_SHOUR1 + unI, EM_LIMITTEXT, A170_MAX_LEN, 0L);
        /* ----- Set (Start Hour) Position Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchHour1;
        SetDlgItemInt(hDlg, IDD_A170_SHOUR1 + unI, unValue, FALSE);

        /* ----- Limit Length of Input Data to EditText (Minute) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_SMINUTE1 + unI, EM_LIMITTEXT, A170_MAX_LEN, 0L);
        /* ----- Set (Minute) Position Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchMin1;
        SetDlgItemInt(hDlg, IDD_A170_SMINUTE1 + unI, unValue, FALSE);

        /* ----- Limit Length of Input Data to EditText (End Hour) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_EHOUR1 + unI, EM_LIMITTEXT, A170_MAX_LEN, 0L);
        /* ----- Set (End Hour) Position Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchHour2;
        SetDlgItemInt(hDlg, IDD_A170_EHOUR1 + unI, unValue, FALSE);

        /* ----- Limit Length of Input Data to EditText (Minute) ----- */
        SendDlgItemMessage(hDlg, IDD_A170_EMINUTE1 + unI, EM_LIMITTEXT, A170_MAX_LEN, 0L);
        /* ----- Set (Minute) Position Data to Specified EditText ----- */
        unValue = (UINT)restTbl[unI].uchMin2;
        SetDlgItemInt(hDlg, IDD_A170_EMINUTE1 + unI, unValue, FALSE);
    }
	if (ParaMDCCheck (MDC_PLU5_ADR, EVEN_MDC_BIT1)) {
		// Determine if the MDC 298 Bit B is turned on indicating that a
		// Sales Restriction uses AND in its time check logic or if Bit B is turned
		// off indicating that a Sales Restriction uses OR in its time check logic.
		// See function ItemSalesSalesRestriction() in the GenPOS source code.
		CheckDlgButton (hDlg, IDD_A170_MDC_PLU5_ADR, BST_CHECKED);
	}
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A170SetData()
*
*   Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets the inputed data from each edittext and writes them
*   to Parameter file.
* ===========================================================================
*/
BOOL    A170SetData(HWND hDlg)
{
	RESTRICT  restTbl[MAX_REST_SIZE] = {0};
    USHORT  usReturnLen;
    UINT    unI;

    for (unI = 0; unI < MAX_REST_SIZE; unI++) {
		USHORT  usKbits = 0;
		WORD    wID;
		BOOL    fDate,  fWeek,  fShour,  fSminute,  fEhour,  fEminute;
		UINT    unDate, unWeek, unShour, unSminute, unEhour, unEminute;

		usKbits = A170ModeLessChildDialogSettings (IDD_A170_DAYS_1 + unI);

        /* ----- Get (Date) Data from EditText ----- */
        unDate = GetDlgItemInt(hDlg, IDD_A170_DATE1 + unI, (LPBOOL)&fDate, FALSE);

        /* ----- Get (Day of the week) Data from EditText ----- */
        unWeek = GetDlgItemInt(hDlg, IDD_A170_WEEK1 + unI, (LPBOOL)&fWeek, FALSE);
        
        /* ----- Get (Start hour) Data from EditText ----- */
        unShour = GetDlgItemInt(hDlg, IDD_A170_SHOUR1 + unI, (LPBOOL)&fShour, FALSE);
        /* ----- Get (Start minute) Data from EditText ----- */
        unSminute = GetDlgItemInt(hDlg, IDD_A170_SMINUTE1 + unI, (LPBOOL)&fSminute, FALSE);

        /* ----- Get (End hour) Data from EditText ----- */
        unEhour = GetDlgItemInt(hDlg, IDD_A170_EHOUR1 + unI, (LPBOOL)&fEhour, FALSE);
        /* ----- Get (End minute) Data from EditText ----- */
        unEminute = GetDlgItemInt(hDlg, IDD_A170_EMINUTE1 + unI, (LPBOOL)&fEminute, FALSE);

        if ((fDate == 0) || (fWeek == 0) || (fShour == 0) || 
            (fSminute == 0) || (fEhour == 0) || (fEminute == 0)) {

            /* ----- Display Error Message ----- */
            wID = (WORD)((fDate    == 0) ? IDD_A170_DATE1    :
                         (fWeek    == 0) ? IDD_A170_WEEK1    :
                         (fShour   == 0) ? IDD_A170_SHOUR1   :
                         (fSminute == 0) ? IDD_A170_SMINUTE1 :
                         (fEhour   == 0) ? IDD_A170_EHOUR1   :
                                           IDD_A170_EMINUTE1);

            A170DispErr(hDlg, (WORD)(wID + unI));
            return TRUE;
        } else if (A170ChkDatePos(hDlg, (WORD)(IDD_A170_DATE1 + unI))) {
            A170DispErr(hDlg, (WORD)(IDD_A170_DATE1 + unI));
            return TRUE;
        } else if(A170ChkWeekPos(hDlg, (WORD)(IDD_A170_WEEK1 + unI))) {
            A170DispErr(hDlg, (WORD)(IDD_A170_WEEK1 + unI));
		} else if((A170ChkHourPos(hDlg, (WORD)(IDD_A170_SHOUR1 + unI))) ||
				  (A170ChkHourPos(hDlg, (WORD)(IDD_A170_EHOUR1 + unI)))) {
            wID = (WORD)((fShour == 0) ? IDD_A170_SHOUR1 + unI  : IDD_A170_EHOUR1 + unI);
            A170DispErr(hDlg, wID);
		} else if((A170ChkMinutePos(hDlg, (WORD)(IDD_A170_SMINUTE1 + unI))) ||
				  (A170ChkMinutePos(hDlg, (WORD)(IDD_A170_EMINUTE1 + unI)))) {
            wID = (WORD)((fSminute == 0) ? IDD_A170_SMINUTE1 + unI  : IDD_A170_EMINUTE1 + unI);
            A170DispErr(hDlg, wID);
		} else {
            /* ----- Set (Date) Data to Data Buffer ----- */
			if (usKbits & (KBITS_RESTRICT_OVERRIDE_ANDOR | KBITS_RESTRICT_OVERRIDE_AND)) {
				restTbl[unI].uchDate = RestrictOverrideAndFromKbits (usKbits);
			} else {
				restTbl[unI].uchDate = (UCHAR)unDate;
			}

            /* ----- Set (Day of the week) Data to Data Buffer ----- */
			if (usKbits & RESTRICT_WEEK_DAYS_ON) {
				restTbl[unI].uchDay = (UCHAR)(usKbits & KBITS_RESTRICT_DAY_MASK);
			} else {
				restTbl[unI].uchDay = (UCHAR)unWeek;
			}

            /* ----- Set (Start hour) Position Data to Data Buffer ----- */
            restTbl[unI].uchHour1  = (UCHAR)unShour;
            /* ----- Set (Start minute) Position Data to Data Buffer ----- */
            restTbl[unI].uchMin1  = (UCHAR)unSminute;

            /* ----- Set (End hour) Position Data to Data Buffer ----- */
            restTbl[unI].uchHour2  = (UCHAR)unEhour;
            /* ----- Set (End minute) Position Data to Data Buffer ----- */
            restTbl[unI].uchMin2  = (UCHAR)unEminute;
        }
  }

    /* ---- Write Initial Data from Parameter File ----- */
    ParaAllWrite(CLASS_PARARESTRICTION, (UCHAR *)restTbl, sizeof(restTbl), 0, &usReturnLen);

	{
		PARAMDC MdcData = {0};

		MdcData.usAddress = MDC_PLU5_ADR;
		ParaMDCRead( &MdcData );

		if (IsDlgButtonChecked (hDlg, IDD_A170_MDC_PLU5_ADR)) {
			MdcData.uchMDCData |= EVEN_MDC_BIT1;    // enable Sales Restriction is AND
		} else {
			MdcData.uchMDCData &= ~ EVEN_MDC_BIT1;  // disable Sales Restriction is AND
		}
		ParaMDCWrite( &MdcData );
	}

    /* ----- Set File Status Flag ----- */
    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A170ChkDatePos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed date position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A170ChkDatePos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    UINT   nDatePos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */
    nDatePos = GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */
    if ((nDatePos <(UINT)A170_DATE_MIN) || (nDatePos > (UINT)A170_DATE_MAX)) {
        fCheck = TRUE;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A170ChkWeekPos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed Day of the week position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A170ChkWeekPos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    UINT   nWeekPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */
    nWeekPos = GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */
    if ((nWeekPos <A170_WEEK_MIN) || (nWeekPos > A170_WEEK_MAX)) {
        fCheck = TRUE;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A170ChkHourPos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed Start/End hour position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A170ChkHourPos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    UINT   nHourPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */
    nHourPos = GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */
    if ((nHourPos < (UINT)A170_HOUR_MIN) || (nHourPos > (UINT)A170_HOUR_MAX)) {
        fCheck = TRUE;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A170ChkMinutePos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed Start/End Minute position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A170ChkMinutePos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    UINT   nMinutePos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */
    nMinutePos = GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */
    if ((nMinutePos <A170_MINUTE_MIN) || (nMinutePos > A170_MINUTE_MAX)) {
        fCheck = TRUE;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A170DispErr()
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
VOID    A170DispErr(HWND hDlg, WPARAM wID)
{
    WORD    wMin, wMax;
    WCHAR   szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A170, szCaption, PEP_CAPTION_LEN);
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    if((IDD_A170_DATE1 <= LOWORD(wID)) && (LOWORD(wID) <= IDD_A170_DATE6))
    {
		wMin = A170_DATE_MIN;
        wMax = A170_DATE_MAX;
    }
    else if((IDD_A170_WEEK1 <= LOWORD(wID)) && (LOWORD(wID) <= IDD_A170_WEEK6))
    {
        wMin = A170_WEEK_MIN;
        wMax = A170_WEEK_MAX;
    }
    else if(((IDD_A170_SHOUR1 <= LOWORD(wID)) && (LOWORD(wID) <= IDD_A170_SHOUR6)) ||
            ((IDD_A170_EHOUR1 <= LOWORD(wID)) && (LOWORD(wID) <= IDD_A170_EHOUR6)))
    {
        wMin = A170_HOUR_MIN;
        wMax = A170_HOUR_MAX;
    }
    else
    {
        wMin = A170_MINUTE_MIN;
        wMax = A170_MINUTE_MAX;
    }
		
    wsPepf(szMessage, szWork, wMin, wMax);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);
    SetDlgItemInt(hDlg, LOWORD(wID), wMin, FALSE);

    /* ----- Set Focus to Error Occurred EditText ----- */
    SendDlgItemMessage(hDlg, LOWORD(wID), EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, LOWORD(wID)));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A170SpinProc()
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
VOID    A170SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    int         nEdit;

    /* ----- Initialize spin mode flag ----- */
    PepSpin.nStep      = A170_SPIN_STEP;
    PepSpin.nTurbo     = A170_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */
    nEdit = (int)(GetDlgCtrlID((HWND)lParam) - A170_SPIN_OFFSET);

    /* ----- Set minimum and maximum ----- */
    switch (nEdit) {

    case IDD_A170_DATE1:
    case IDD_A170_DATE2:
    case IDD_A170_DATE3:
    case IDD_A170_DATE4:
    case IDD_A170_DATE5:
    case IDD_A170_DATE6:
        PepSpin.lMax = (long)A170_DATE_MAX;
        PepSpin.lMin = (long)A170_DATE_MIN;
        break;

    case IDD_A170_WEEK1:
    case IDD_A170_WEEK2:
    case IDD_A170_WEEK3:
    case IDD_A170_WEEK4:
    case IDD_A170_WEEK5:
    case IDD_A170_WEEK6:
        PepSpin.lMax = (long)A170_WEEK_MAX;
        PepSpin.lMin = (long)A170_WEEK_MIN;
        break;

    case IDD_A170_SHOUR1:
    case IDD_A170_SHOUR2:
    case IDD_A170_SHOUR3:
    case IDD_A170_SHOUR4:
    case IDD_A170_SHOUR5:
    case IDD_A170_SHOUR6:
    case IDD_A170_EHOUR1:
    case IDD_A170_EHOUR2:
    case IDD_A170_EHOUR3:
    case IDD_A170_EHOUR4:
    case IDD_A170_EHOUR5:
    case IDD_A170_EHOUR6:
        PepSpin.lMax = (long)A170_HOUR_MAX;
        PepSpin.lMin = (long)A170_HOUR_MIN;
        break;

    default:
        PepSpin.lMax = (long)A170_MINUTE_MAX;
        PepSpin.lMin = (long)A170_MINUTE_MIN;
        break;
    }

    /* ----- SpinButton Procedure ----- */
    PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);
}


HWND A170ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A170),
                                   hParentWnd,
                                   A170DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A170ModeLessDialogWnd (void)
{
	return hWndModeLess;
}

/* ===== End of A170.C ===== */
