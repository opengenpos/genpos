/*
* ---------------------------------------------------------------------------
* Title         :   Assignment IP Address  
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRANSIP.C
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
* Apl-11-00 : 00.00.01 :            : Initial (NCR2172)
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
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include "pep.h"
#include "pepflag.h"
#include "pepcomm.h"
#include "trans.h"
#include "transl.h"
#include "traterm.h"

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


static USHORT      usDhcp = 0;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

#define	TERMINAL_NAME_MAX_LENGTH 64   /* The maximum length of the hostname string */

/*
*===========================================================================
** Synopsis:    BOOL TransIP(HANDLE hInst, HWND hWnd)
*     Input:    hInst - instance to bind
*               hWnd  - Handle of Dialog Procedure
*
** Return:      nothing
*
** Description: This function displays the dialog box of IP setting.
*===========================================================================
*/
BOOL TransIP(HWND hWnd)
{
    DLGPROC lpitDlgProc;
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    lpitDlgProc = MakeProcInstance(TransIPDlgProc,
                                   hInst);
    DialogBoxPopup(hResourceDll/*hInst*/,
              MAKEINTRESOURCEW(IDD_TRANS_IPADDRESS),
              hWnd,
              lpitDlgProc);

    FreeProcInstance(lpitDlgProc);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  TransIPDlgProc()
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
*                   Assignment IP Address of terminal
* ===========================================================================
*/
BOOL    WINAPI  TransIPDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {

    case    WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        TransIPInitData(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_TRANS_IPADD11; j<=IDD_TRANS_IPDHCP; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Spin Button Procedure ----- */

        TransIPSpinProc(hDlg, wParam, lParam);

        return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
        
        case    IDD_TRANS_IPADD11:
        case    IDD_TRANS_IPADD12:
        case    IDD_TRANS_IPADD13:
        case    IDD_TRANS_IPADD14:
        case    IDD_TRANS_IPADD21:
        case    IDD_TRANS_IPADD22:
        case    IDD_TRANS_IPADD23:
        case    IDD_TRANS_IPADD24:
        case    IDD_TRANS_IPADD31:
        case    IDD_TRANS_IPADD32:
        case    IDD_TRANS_IPADD33:
        case    IDD_TRANS_IPADD34:
        case    IDD_TRANS_IPADD41:
        case    IDD_TRANS_IPADD42:
        case    IDD_TRANS_IPADD43:
        case    IDD_TRANS_IPADD44:
        case    IDD_TRANS_IPADD51:
        case    IDD_TRANS_IPADD52:
        case    IDD_TRANS_IPADD53:
        case    IDD_TRANS_IPADD54:
        case    IDD_TRANS_IPADD61:
        case    IDD_TRANS_IPADD62:
        case    IDD_TRANS_IPADD63:
        case    IDD_TRANS_IPADD64:
        case    IDD_TRANS_IPADD71:
        case    IDD_TRANS_IPADD72:
        case    IDD_TRANS_IPADD73:
        case    IDD_TRANS_IPADD74:
        case    IDD_TRANS_IPADD81:
        case    IDD_TRANS_IPADD82:
        case    IDD_TRANS_IPADD83:
        case    IDD_TRANS_IPADD84:
        case    IDD_TRANS_IPADD91:
        case    IDD_TRANS_IPADD92:
        case    IDD_TRANS_IPADD93:
        case    IDD_TRANS_IPADD94:
        case    IDD_TRANS_IPADD101:
        case    IDD_TRANS_IPADD102:
        case    IDD_TRANS_IPADD103:
        case    IDD_TRANS_IPADD104:
        case    IDD_TRANS_IPADD111:
        case    IDD_TRANS_IPADD112:
        case    IDD_TRANS_IPADD113:
        case    IDD_TRANS_IPADD114:
        case    IDD_TRANS_IPADD121:
        case    IDD_TRANS_IPADD122:
        case    IDD_TRANS_IPADD123:
        case    IDD_TRANS_IPADD124:
        case    IDD_TRANS_IPADD131:
        case    IDD_TRANS_IPADD132:
        case    IDD_TRANS_IPADD133:
        case    IDD_TRANS_IPADD134:
        case    IDD_TRANS_IPADD141:
        case    IDD_TRANS_IPADD142:
        case    IDD_TRANS_IPADD143:
        case    IDD_TRANS_IPADD144:
        case    IDD_TRANS_IPADD151:
        case    IDD_TRANS_IPADD152:
        case    IDD_TRANS_IPADD153:
        case    IDD_TRANS_IPADD154:
        case    IDD_TRANS_IPADD161:
        case    IDD_TRANS_IPADD162:
        case    IDD_TRANS_IPADD163:
        case    IDD_TRANS_IPADD164:

            if (HIWORD(wParam) == EN_CHANGE)
            {
 
               /* ----- Check Inputed Data with Data Range ----- */

               if (TransIPChkRndPos(hDlg, wParam)) {

                   /* ----- Display Error Message ----- */

                   TransIPDispErr(hDlg, wParam);
                }
               return TRUE;
            }
            
            return FALSE;
        
        case IDD_TRANS_IPDHCP:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Set Delete Confirmation Flag ----- */

                if (IsDlgButtonChecked(hDlg, wParam)) {

                    usDhcp = 1;

                } else {

                    usDhcp = 0;
                }
                return TRUE;
            }
            return FALSE;

        case    IDOK:
        case    IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Inputed Data from EditText ----- */

                    if (TransIPSetData(hDlg)) {

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
**  Synopsis    :   BOOL    TransIPSetData()
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
BOOL    TransIPSetData(HWND hDlg)
{
    USHORT  abIPTbl[TRANS_MAX_ROW][TRANS_MAX_COL];
			// each name assumed to be max of 16 characters plus 2 padding. 16 Terminals plus 1 padding
	WCHAR   szPCNameKeys[18 * 17];
	WCHAR   *pKeys = 0;
    WORD    wID;
    BOOL    fIPAdd11,
            fIPAdd12,
            fIPAdd13,
            fIPAdd14;
    UINT    unI,
            unIPAdd11,
            unIPAdd12,
            unIPAdd13,
            unIPAdd14;
    WCHAR   szIPAddress[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szPCName[TRANS_CONVERT_WORK_SIZE];
	WCHAR   szHostFileName[256] = { 0 };

	wcscat(szHostFileName, szPepAppDataFolder);
	wcscat(szHostFileName, szTransIPFileName);

	// First of all, we will clear out this section so that whatever is there
	// now will be removed.  This allows us to then put only what we want without
	// worrying about what is there now.
    WritePrivateProfileStringPep(WIDE("PCName"),
                              NULL,
                              NULL,
							szHostFileName);

	pKeys = szPCNameKeys;
    for (unI = 0; unI < TRANS_MAX_ROW; unI++) {

        /* ----- Get IP Address Data from EditText ----- */

        unIPAdd11 = GetDlgItemInt(hDlg, IDD_TRANS_IPADD11 + (unI * TRANS_MAX_COL), (LPBOOL)&fIPAdd11, FALSE);
        unIPAdd12 = GetDlgItemInt(hDlg, IDD_TRANS_IPADD12 + (unI * TRANS_MAX_COL), (LPBOOL)&fIPAdd12, FALSE);
        unIPAdd13 = GetDlgItemInt(hDlg, IDD_TRANS_IPADD13 + (unI * TRANS_MAX_COL), (LPBOOL)&fIPAdd13, FALSE);
        unIPAdd14 = GetDlgItemInt(hDlg, IDD_TRANS_IPADD14 + (unI * TRANS_MAX_COL), (LPBOOL)&fIPAdd14, FALSE);
		//DlgItemGetText (hDlg, IDD_TRANS_IPSTR1 + unI, pKeys, 18);
		DlgItemGetText (hDlg, IDD_TRANS_IPSTR1 + unI, pKeys, TERMINAL_NAME_MAX_LENGTH + 1);
		pKeys += wcslen(pKeys) + 1;

        if ((fIPAdd11 == 0) || (fIPAdd12 == 0) || (fIPAdd13 == 0) || (fIPAdd14 == 0)) {

            /* ----- Display Error Message ----- */
            if(fIPAdd11 == 0)
              wID = (WORD)(IDD_TRANS_IPADD11);
            else if(fIPAdd12 == 0)
              wID = (WORD)(IDD_TRANS_IPADD12);
            else if(fIPAdd13 == 0)
              wID = (WORD)(IDD_TRANS_IPADD13);
            else
              wID = (WORD)(IDD_TRANS_IPADD14);
            
            TransIPDispErr(hDlg, (WORD)(wID + (unI * TRANS_MAX_COL)));

            return TRUE;

        } else if (TransIPChkRndPos(hDlg, (WORD)(IDD_TRANS_IPADD11 + (unI * TRANS_MAX_COL)))) {

            TransIPDispErr(hDlg, (WORD)(IDD_TRANS_IPADD11 + (unI * TRANS_MAX_COL)));

            return TRUE;

        } else if (TransIPChkRndPos(hDlg, (WORD)(IDD_TRANS_IPADD12 + (unI * TRANS_MAX_COL)))) {

            TransIPDispErr(hDlg, (WORD)(IDD_TRANS_IPADD12 + (unI * TRANS_MAX_COL)));

            return TRUE;

        } else if (TransIPChkRndPos(hDlg, (WORD)(IDD_TRANS_IPADD13 + (unI * TRANS_MAX_COL)))) {

            TransIPDispErr(hDlg, (WORD)(IDD_TRANS_IPADD13 + (unI * TRANS_MAX_COL)));

            return TRUE;

        } else if (TransIPChkRndPos(hDlg, (WORD)(IDD_TRANS_IPADD14 + (unI * TRANS_MAX_COL)))) {

            TransIPDispErr(hDlg, (WORD)(IDD_TRANS_IPADD14 + (unI * TRANS_MAX_COL)));

            return TRUE;
        } else {
        
            /* ----- Set Delimiter Data to Data Buffer ----- */

            abIPTbl[unI][TRANS_IPADD1POS]     = (USHORT)unIPAdd11;
            abIPTbl[unI][TRANS_IPADD1POS + 1] = (USHORT)unIPAdd12;
            abIPTbl[unI][TRANS_IPADD1POS + 2] = (USHORT)unIPAdd13;
            abIPTbl[unI][TRANS_IPADD1POS + 3] = (USHORT)unIPAdd14;
        }
    }
    

    /* ---- Write Initial Data from Parameter File ----- */

	pKeys = szPCNameKeys;
    for (unI = 0; unI < TRANS_MAX_ROW; unI++) {
		// ensure that there is a key name otherwise we may end up with problems.
        if (pKeys[0])
		{
			wcscpy (szPCName, pKeys);
		}
		else
		{
			wsPepf(szPCName, WIDE("NCRPOS-%d"), unI+1);
		}
        
        wsPepf(szIPAddress, WIDE("%d:%d:%d:%d"), abIPTbl[unI][TRANS_IPADD1POS],
                                                        abIPTbl[unI][TRANS_IPADD1POS + 1],
                                                        abIPTbl[unI][TRANS_IPADD1POS + 2],
                                                        abIPTbl[unI][TRANS_IPADD1POS + 3]);

        WritePrivateProfileStringPep(WIDE("PCName"),
                                  szPCName,
                                  szIPAddress,
								szHostFileName);
		pKeys += wcslen(pKeys) + 1;
    }
    

    wsPepf(szIPAddress, WIDE("%d"), usDhcp);

    WritePrivateProfileStringPep(WIDE("Hosts"),
                              WIDE("DHCP"),
                                  szIPAddress,
							szHostFileName);

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    TransIPDispErr()
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
VOID    TransIPDispErr(HWND hDlg, WPARAM wID)
{
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN];
            /* szCaption[PEP_CAPTION_LEN]; */

    /* ----- Load String from Resource ----- */

/*    LoadString(hInst, IDS_PEP_CAPTION_IP, (LPSTR)szCaption, PEP_CAPTION_LEN);*/

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */

    wsPepf(szMessage, szWork, TRANS_IPPOS_MIN, TRANS_IPPOS_MAX);

    /* ----- Display Error Message with MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg,
               szMessage,
               WIDE("IP Address"),
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */

    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}




/*
* ===========================================================================
**  Synopsis    :   BOOL    TransIPChkRndPos()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed rounding position data
*   is within the specified range or not. It returns TRUE, if the inputed data
*   is out of range. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    TransIPChkRndPos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    short   nRndPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */

    nRndPos = (short)GetDlgItemInt(hDlg, wID, NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */

        switch (LOWORD(wID)) {
        
        case    IDD_TRANS_IPADD11:
        case    IDD_TRANS_IPADD12:
        case    IDD_TRANS_IPADD13:
        case    IDD_TRANS_IPADD14:
        case    IDD_TRANS_IPADD21:
        case    IDD_TRANS_IPADD22:
        case    IDD_TRANS_IPADD23:
        case    IDD_TRANS_IPADD24:
        case    IDD_TRANS_IPADD31:
        case    IDD_TRANS_IPADD32:
        case    IDD_TRANS_IPADD33:
        case    IDD_TRANS_IPADD34:
        case    IDD_TRANS_IPADD41:
        case    IDD_TRANS_IPADD42:
        case    IDD_TRANS_IPADD43:
        case    IDD_TRANS_IPADD44:
        case    IDD_TRANS_IPADD51:
        case    IDD_TRANS_IPADD52:
        case    IDD_TRANS_IPADD53:
        case    IDD_TRANS_IPADD54:
        case    IDD_TRANS_IPADD61:
        case    IDD_TRANS_IPADD62:
        case    IDD_TRANS_IPADD63:
        case    IDD_TRANS_IPADD64:
        case    IDD_TRANS_IPADD71:
        case    IDD_TRANS_IPADD72:
        case    IDD_TRANS_IPADD73:
        case    IDD_TRANS_IPADD74:
        case    IDD_TRANS_IPADD81:
        case    IDD_TRANS_IPADD82:
        case    IDD_TRANS_IPADD83:
        case    IDD_TRANS_IPADD84:
        case    IDD_TRANS_IPADD91:
        case    IDD_TRANS_IPADD92:
        case    IDD_TRANS_IPADD93:
        case    IDD_TRANS_IPADD94:
        case    IDD_TRANS_IPADD101:
        case    IDD_TRANS_IPADD102:
        case    IDD_TRANS_IPADD103:
        case    IDD_TRANS_IPADD104:
        case    IDD_TRANS_IPADD111:
        case    IDD_TRANS_IPADD112:
        case    IDD_TRANS_IPADD113:
        case    IDD_TRANS_IPADD114:
        case    IDD_TRANS_IPADD121:
        case    IDD_TRANS_IPADD122:
        case    IDD_TRANS_IPADD123:
        case    IDD_TRANS_IPADD124:
        case    IDD_TRANS_IPADD131:
        case    IDD_TRANS_IPADD132:
        case    IDD_TRANS_IPADD133:
        case    IDD_TRANS_IPADD134:
        case    IDD_TRANS_IPADD141:
        case    IDD_TRANS_IPADD142:
        case    IDD_TRANS_IPADD143:
        case    IDD_TRANS_IPADD144:
        case    IDD_TRANS_IPADD151:
        case    IDD_TRANS_IPADD152:
        case    IDD_TRANS_IPADD153:
        case    IDD_TRANS_IPADD154:
        case    IDD_TRANS_IPADD161:
        case    IDD_TRANS_IPADD162:
        case    IDD_TRANS_IPADD163:
        case    IDD_TRANS_IPADD164:
            if ((nRndPos < TRANS_IPPOS_MIN) || (nRndPos > TRANS_IPPOS_MAX))
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
**  Synopsis    :   VOID    TransIPInitData()
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
VOID    TransIPInitData(HWND hDlg)
{
    USHORT  abIPTbl[TRANS_MAX_ROW][TRANS_MAX_COL];
    UINT    unI1,
            unValue;
    WCHAR    szDesc[128];
    WCHAR   szIPAddress[TRANS_CONVERT_WORK_SIZE];
			// each name assumed to be max of 16 characters plus 2 padding. 16 Terminals plus 1 padding
	WCHAR   szPCNameKeys[18 * 17];
	WCHAR   *pKeys = 0;
	WCHAR   szHostFileName[256] = { 0 };

	wcscat(szHostFileName, szPepAppDataFolder);
	wcscat(szHostFileName, szTransIPFileName);

	memset (szPCNameKeys, 0, sizeof(szPCNameKeys));

	if (GetPrivateProfileStringPep(WIDE("PCName"),
                            NULL,
                            WIDE(""),
                            szPCNameKeys,
                            sizeof(szPCNameKeys) / sizeof(szPCNameKeys[0]),
						szHostFileName))
	{
	}
	else
	{
		pKeys = szPCNameKeys;
		for (unI1 = 0; unI1 < TRANS_MAX_ROW; unI1++)
		{
			wsPepf(pKeys, WIDE("NCRPOS-%d"), unI1+1);
			pKeys += wcslen (pKeys) + 1;
		}
	}

    /* ----- set initial description, V3.3 ----- */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    LoadString(hResourceDll, IDS_TRANS_IPDESC1, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR1, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC2, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR2, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC3, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR3, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC4, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR4, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC5, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR5, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC6, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR6, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC7, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR7, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC8, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR8, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC9, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR9, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC10, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR10, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC11, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR11, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC12, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR12, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC13, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR13, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC14, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR14, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC15, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR15, szDesc);
    LoadString(hResourceDll, IDS_TRANS_IPDESC16, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR16, szDesc);
#endif
    LoadString(hResourceDll, IDS_TRANS_IPDELILEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_TRANS_IPSTRDELILEN, szDesc);

    /* ---- Read Initial Data from Parameter File ----- */

	pKeys = szPCNameKeys;
    for (unI1 = 0; unI1 < TRANS_MAX_ROW; unI1++)
    {
        if (GetPrivateProfileStringPep(WIDE("PCName"),
                                    pKeys,
                                    WIDE(""),
                                    szIPAddress,
                                    sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {

            swscanf(szIPAddress, WIDE("%hd:%hd:%hd:%hd"), &abIPTbl[unI1][TRANS_IPADD1POS],
                                                            &abIPTbl[unI1][TRANS_IPADD1POS + 1],
                                                            &abIPTbl[unI1][TRANS_IPADD1POS + 2],
                                                            &abIPTbl[unI1][TRANS_IPADD1POS + 3]);
        } else {
            
            /* default IP Address */
            
            abIPTbl[unI1][TRANS_IPADD1POS] = 192;
            abIPTbl[unI1][TRANS_IPADD1POS + 1] = 168;
            abIPTbl[unI1][TRANS_IPADD1POS + 2] = 0;
            abIPTbl[unI1][TRANS_IPADD1POS + 3] = (unI1 == 0) ? 10 : (USHORT)(unI1+1);
            
        }
		pKeys += wcslen(pKeys) + 1;
    }

    /* Get IP Address */

	pKeys = szPCNameKeys;
    for (unI1 = 0; unI1 < TRANS_MAX_ROW; unI1++)
    {

		 /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
 
         SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPSTR1 + unI1 ,
                           EM_LIMITTEXT,
                           TERMINAL_NAME_MAX_LENGTH,
                           0L);
		DlgItemRedrawText(hDlg, IDD_TRANS_IPSTR1 + unI1, pKeys);

		pKeys += wcslen(pKeys) + 1;

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
 
         SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPADD11 + (unI1 * TRANS_MAX_COL),
                           EM_LIMITTEXT,
                           TRANS_MAX_IPLEN,
                           0L);


        /* ----- Set IP Address Data to Specified EditText ----- */

        unValue = (UINT)abIPTbl[unI1][TRANS_IPADD1POS];

        SetDlgItemInt(hDlg, 
                      IDD_TRANS_IPADD11 + (unI1 * TRANS_MAX_COL),
                      unValue,
                      FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
 
         SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPADD12 + (unI1 * TRANS_MAX_COL),
                           EM_LIMITTEXT,
                           TRANS_MAX_IPLEN,
                           0L);

        /* ----- Set IP Address Data to Specified EditText ----- */

        unValue = (UINT)abIPTbl[unI1][TRANS_IPADD2POS];

        SetDlgItemInt(hDlg, 
                      IDD_TRANS_IPADD12 + (unI1 * TRANS_MAX_COL),
                      unValue,
                      FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
 
         SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPADD13 + (unI1 * TRANS_MAX_COL),
                           EM_LIMITTEXT,
                           TRANS_MAX_IPLEN,
                           0L);

        /* ----- Set IP Address Data to Specified EditText ----- */

        unValue = (UINT)abIPTbl[unI1][TRANS_IPADD3POS];

        SetDlgItemInt(hDlg, 
                      IDD_TRANS_IPADD13 + (unI1 * TRANS_MAX_COL),
                      unValue,
                      FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
 
         SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPADD14 + (unI1 * TRANS_MAX_COL),
                           EM_LIMITTEXT,
                           TRANS_MAX_IPLEN,
                           0L);

        /* ----- Set IP Address Data to Specified EditText ----- */

        unValue = (UINT)abIPTbl[unI1][TRANS_IPADD4POS];

        SetDlgItemInt(hDlg, 
                      IDD_TRANS_IPADD14 + (unI1 * TRANS_MAX_COL),
                      unValue,
                      FALSE);

    }

    /* ----- set status ----- */
    if (GetPrivateProfileStringPep(WIDE("Hosts"),
                                    WIDE("DHCP"),
                                    WIDE(""),
                                    szIPAddress,
                                    sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {
        wscanf(szIPAddress, WIDE("%d"), &usDhcp);
		usDhcp = _wtoi (szIPAddress);
        
    } else {
        
        usDhcp = 0;
    }
    
    SendDlgItemMessage(hDlg,
                           IDD_TRANS_IPDHCP,
                           BM_SETCHECK,
                           usDhcp,
                           0L);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    TransIPSpinProc()
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
VOID    TransIPSpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    int         nEdit;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = TRANS_SPIN_STEP;
    PepSpin.nTurbo     = TRANS_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */

    nEdit = (int)(GetDlgCtrlID((HWND)lParam) - TRANS_SPIN_OFFSET);

    /* ----- Set minimum and maximum ----- */

    switch (nEdit) {
    case    IDD_TRANS_IPADD11:
    case    IDD_TRANS_IPADD12:
    case    IDD_TRANS_IPADD13:
    case    IDD_TRANS_IPADD14:
    case    IDD_TRANS_IPADD21:
    case    IDD_TRANS_IPADD22:
    case    IDD_TRANS_IPADD23:
    case    IDD_TRANS_IPADD24:
    case    IDD_TRANS_IPADD31:
    case    IDD_TRANS_IPADD32:
    case    IDD_TRANS_IPADD33:
    case    IDD_TRANS_IPADD34:
    case    IDD_TRANS_IPADD41:
    case    IDD_TRANS_IPADD42:
    case    IDD_TRANS_IPADD43:
    case    IDD_TRANS_IPADD44:
    case    IDD_TRANS_IPADD51:
    case    IDD_TRANS_IPADD52:
    case    IDD_TRANS_IPADD53:
    case    IDD_TRANS_IPADD54:
    case    IDD_TRANS_IPADD61:
    case    IDD_TRANS_IPADD62:
    case    IDD_TRANS_IPADD63:
    case    IDD_TRANS_IPADD64:
    case    IDD_TRANS_IPADD71:
    case    IDD_TRANS_IPADD72:
    case    IDD_TRANS_IPADD73:
    case    IDD_TRANS_IPADD74:
    case    IDD_TRANS_IPADD81:
    case    IDD_TRANS_IPADD82:
    case    IDD_TRANS_IPADD83:
    case    IDD_TRANS_IPADD84:
    case    IDD_TRANS_IPADD91:
    case    IDD_TRANS_IPADD92:
    case    IDD_TRANS_IPADD93:
    case    IDD_TRANS_IPADD94:
    case    IDD_TRANS_IPADD101:
    case    IDD_TRANS_IPADD102:
    case    IDD_TRANS_IPADD103:
    case    IDD_TRANS_IPADD104:
    case    IDD_TRANS_IPADD111:
    case    IDD_TRANS_IPADD112:
    case    IDD_TRANS_IPADD113:
    case    IDD_TRANS_IPADD114:
    case    IDD_TRANS_IPADD121:
    case    IDD_TRANS_IPADD122:
    case    IDD_TRANS_IPADD123:
    case    IDD_TRANS_IPADD124:
    case    IDD_TRANS_IPADD131:
    case    IDD_TRANS_IPADD132:
    case    IDD_TRANS_IPADD133:
    case    IDD_TRANS_IPADD134:
    case    IDD_TRANS_IPADD141:
    case    IDD_TRANS_IPADD142:
    case    IDD_TRANS_IPADD143:
    case    IDD_TRANS_IPADD144:
    case    IDD_TRANS_IPADD151:
    case    IDD_TRANS_IPADD152:
    case    IDD_TRANS_IPADD153:
    case    IDD_TRANS_IPADD154:
    case    IDD_TRANS_IPADD161:
    case    IDD_TRANS_IPADD162:
    case    IDD_TRANS_IPADD163:
    case    IDD_TRANS_IPADD164:
        PepSpin.lMax = (long)TRANS_IPPOS_MAX;
        PepSpin.lMin = (long)TRANS_IPPOS_MIN;
        break;

    default:
        break;
    }

    /* ----- SpinButton Procedure ----- */

    PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    TransIPInitData()
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
VOID    TransIPDefaultData(VOID)
{
    USHORT  abIPTbl[TRANS_MAX_COL];
    UINT    unI1;
    WCHAR   szIPAddress[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szPCName[TRANS_CONVERT_WORK_SIZE];
	WCHAR   szHostFileName[256] = { 0 };

	wcscat(szHostFileName, szPepAppDataFolder);
	wcscat(szHostFileName, szTransIPFileName);

    /* ---- Read Initial Data from Parameter File ----- */

    for (unI1 = 0; unI1 < TRANS_MAX_ROW; unI1++)
    {
        wsPepf(szPCName, WIDE("NCRPOS-%d"), unI1+1);
        
        if (GetPrivateProfileStringPep(WIDE("PCName"),
                                    szPCName,
                                    WIDE(""),
                                    szIPAddress,
                                    sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {

        } else {
            
            /* default IP Address */
            
            abIPTbl[TRANS_IPADD1POS] = 192;
            abIPTbl[TRANS_IPADD1POS + 1] = 168;
            abIPTbl[TRANS_IPADD1POS + 2] = 0;
            abIPTbl[TRANS_IPADD1POS + 3] = (USHORT)(unI1+1);
            
            wsPepf(szIPAddress, WIDE("%d:%d:%d:%d"), abIPTbl[TRANS_IPADD1POS],
                                                        abIPTbl[TRANS_IPADD1POS + 1],
                                                        abIPTbl[TRANS_IPADD1POS + 2],
                                                        abIPTbl[TRANS_IPADD1POS + 3]);

            WritePrivateProfileStringPep(WIDE("PCName"),
                                  szPCName,
                                  szIPAddress,
								szHostFileName);
        }
    }

    /* ----- set status ----- */
    if (GetPrivateProfileStringPep(WIDE("Hosts"),
                                    WIDE("DHCP"),
                                    WIDE(""),
                                    szIPAddress,
                                    sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {
        
    } else {
        
        usDhcp = 0;
        wsPepf(szIPAddress, WIDE("%d"), usDhcp);

        WritePrivateProfileStringPep(WIDE("Hosts"),
                                  WIDE("DHCP"),
                                  szIPAddress,
							szHostFileName);
    }
}


/* ===== End of TRANSIP.C ===== */
