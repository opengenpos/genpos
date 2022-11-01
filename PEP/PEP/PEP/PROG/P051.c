/*
* ---------------------------------------------------------------------------
* Title         :   Assignment IP Address for KDS (Prog. 51)
* Category      :   Maintenance, NCR 2172 PEP for Windows (Hotel US Model)
* Program Name  :   P051.C
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
* Nov-15-93 : 00.00.01 :            : Initial (NCR2172)
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
#include    "p051.h"

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

static  BYTE    abMDCData[600];    /* Buffer of MDC Data       */

static UCHAR LclParaMDCRead(USHORT usAddress, UCHAR uchMaskData)
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

static UCHAR LclParaMDCWrite(USHORT usAddress, UCHAR uchMdc, UINT uiSetOn)
{
	// we use an address of zero to indicate
	// a report entry which is not to be changed.
	if (usAddress > 0) {
		/* calculate for fitting program address to ram address */
		USHORT    i = ((usAddress - 1) / 2);

		if (uiSetOn)
			abMDCData[i] |= uchMdc;
		else
			abMDCData[i] &= ~uchMdc;

	}
	return(uchMdc);                     /* return - masked bit pattern */
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A084DlgProc()
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
*                   Assignment IP Address for KDS (Program Mode # 51)
* ===========================================================================
*/
BOOL    WINAPI  P051DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {

    case    WM_INITDIALOG:

 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Configulation of DialogBox ----- */

        P051InitData(hDlg);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P51_IPADD11; j<=IDD_P51_STRPOSLEN2; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Spin Button Procedure ----- */

        P051SpinProc(hDlg, wParam, lParam);

        return FALSE;

    case    WM_COMMAND:

        switch (LOWORD(wParam)) {
        
        case    IDD_P51_IPADD11:
        case    IDD_P51_IPADD12:
        case    IDD_P51_IPADD13:
        case    IDD_P51_IPADD14:
        case    IDD_P51_IPADD21:
        case    IDD_P51_IPADD22:
        case    IDD_P51_IPADD23:
        case    IDD_P51_IPADD24:
        case    IDD_P51_IPADD31:
        case    IDD_P51_IPADD32:
        case    IDD_P51_IPADD33:
        case    IDD_P51_IPADD34:
        case    IDD_P51_IPADD41:
        case    IDD_P51_IPADD42:
        case    IDD_P51_IPADD43:
        case    IDD_P51_IPADD44:
        case    IDD_P51_PORT1:
        case    IDD_P51_PORT2:
        case    IDD_P51_PORT3:
        case    IDD_P51_PORT4:
		//*********************************
		//	SR 14 ESMITH
		case    IDD_P51_IPADD51:
        case    IDD_P51_IPADD52:
        case    IDD_P51_IPADD53:
        case    IDD_P51_IPADD54:
        case    IDD_P51_IPADD61:
        case    IDD_P51_IPADD62:
        case    IDD_P51_IPADD63:
        case    IDD_P51_IPADD64:
        case    IDD_P51_IPADD71:
        case    IDD_P51_IPADD72:
        case    IDD_P51_IPADD73:
        case    IDD_P51_IPADD74:
        case    IDD_P51_IPADD81:
        case    IDD_P51_IPADD82:
        case    IDD_P51_IPADD83:
        case    IDD_P51_IPADD84:
        case    IDD_P51_PORT5:
        case    IDD_P51_PORT6:
        case    IDD_P51_PORT7:
        case    IDD_P51_PORT8:
		case    IDD_P51_PORT9:
		case    IDD_P51_PORT10:
		case    IDD_P51_PORT11:
		case    IDD_P51_PORT12:
		case    IDD_P51_PORT13:
		case    IDD_P51_PORT14:
		case    IDD_P51_PORT15:
		case    IDD_P51_PORT16:
		//***********************************

            if (HIWORD(wParam) == EN_CHANGE)
            {
               /* ----- Check Inputed Data with Data Range ----- */
               if (P051ChkRndPos(hDlg, wParam)) {
                   /* ----- Display Error Message ----- */
                   P051DispErr(hDlg, wParam);
                }
               return TRUE;
            }
            
            return FALSE;
        
		case IDD_P51_MDC363BITD:
			{
				int   wBtnId = LOWORD(wParam);
				UINT  uiCheck = IsDlgButtonChecked(hDlg, wBtnId);
				// update the memory resident data.
				LclParaMDCWrite(MDC_DSP_ON_FLY3, ODD_MDC_BIT0, uiCheck);
			}
			break;

        case    IDOK:
        case    IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Get Inputed Data from EditText ----- */
                    if (P051SetData(hDlg)) {
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
**  Synopsis    :   BOOL    P051SetData()
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
BOOL    P051SetData(HWND hDlg)
{
    //USHORT  abKDSTbl[MAX_KDSIP_SIZE][PARA_KDSIP_LEN];
	PARAKDS ParaKDS[P51_MAX_ROW]; //SR 14 ESMITH 	
    USHORT  usReturnLen;
    WORD    wID;
    BOOL    fIPAdd11, fIPAdd12, fIPAdd13, fIPAdd14, fPort;
    UCHAR	uchIPAdd11, uchIPAdd12, uchIPAdd13, uchIPAdd14;
	UINT	unI, unPort, unPort2;

    for (unI = 0; unI < P51_MAX_ROW; unI++) {
        /* ----- Get IP Address Data from EditText ----- */
        uchIPAdd11 = GetDlgItemInt(hDlg, IDD_P51_IPADD11 + (unI * 4), (LPBOOL)&fIPAdd11, FALSE);
        uchIPAdd12 = GetDlgItemInt(hDlg, IDD_P51_IPADD12 + (unI * 4), (LPBOOL)&fIPAdd12, FALSE);
        uchIPAdd13 = GetDlgItemInt(hDlg, IDD_P51_IPADD13 + (unI * 4), (LPBOOL)&fIPAdd13, FALSE);
        uchIPAdd14 = GetDlgItemInt(hDlg, IDD_P51_IPADD14 + (unI * 4), (LPBOOL)&fIPAdd14, FALSE);

        if ((fIPAdd11 == 0) || (fIPAdd12 == 0) || (fIPAdd13 == 0) || (fIPAdd14 == 0)) {
            /* ----- Display Error Message ----- */
            if(fIPAdd11 == 0)
              wID = (WORD)(IDD_P51_IPADD11);
            else if(fIPAdd12 == 0)
              wID = (WORD)(IDD_P51_IPADD12);
            else if(fIPAdd13 == 0)
              wID = (WORD)(IDD_P51_IPADD13);
            else
              wID = (WORD)(IDD_P51_IPADD14);
            
            P051DispErr(hDlg, (WORD)(wID + (unI * 4)));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_IPADD11 + (unI * 4)))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_IPADD11 + (unI * 4)));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_IPADD12 + (unI * 4)))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_IPADD12 + (unI * 4)));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_IPADD13 + (unI * 4)))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_IPADD13 + (unI * 4)));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_IPADD14 + (unI * 4)))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_IPADD14 + (unI * 4)));
            return TRUE;
        } else {
            /* ----- Set Delimiter Data to Data Buffer ----- */
	        //*********
			// SR 14 ESMITH
			ParaKDS[unI].uchIpAddress[P51_IPADD1POS] = uchIPAdd11;
			ParaKDS[unI].uchIpAddress[P51_IPADD2POS] = uchIPAdd12;
			ParaKDS[unI].uchIpAddress[P51_IPADD3POS] = uchIPAdd13;
			ParaKDS[unI].uchIpAddress[P51_IPADD4POS] = uchIPAdd14;
        }
    }
    
    for (unI = 0; unI < P51_MAX_ROW; unI++) {
        /* ----- Get Port No. Data from EditText ----- */
        unPort = GetDlgItemInt(hDlg, IDD_P51_PORT1 + unI, (LPBOOL)&fPort,  FALSE);
        if (fPort == 0) {
            /* ----- Display Error Message ----- */
            wID = (WORD)(IDD_P51_PORT1);
            P051DispErr(hDlg, (WORD)(wID + unI));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_PORT1 + unI))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_PORT1 + unI));
            return TRUE;
        } else {
            /* ----- Set Port No. Data to Data Buffer ----- */
	        //*********
			// SR 14 ESMITH
            ParaKDS[unI].usPort = (USHORT)unPort;			
        }
    }
	
	//**********************************
	// SR 14 ESMITH NHPOS Interface Port
	//**********************************
	for (unI = 0; unI < P51_MAX_ROW; unI++) {
        /* ----- Get Port No. Data from EditText ----- */
        unPort2 = GetDlgItemInt(hDlg, IDD_P51_PORT9 + unI, (LPBOOL)&fPort,  FALSE);
        if (fPort == 0) {
            /* ----- Display Error Message ----- */
             wID = (WORD)(IDD_P51_PORT9);
            P051DispErr(hDlg, (WORD)(wID + unI));
            return TRUE;
        } else if (P051ChkRndPos(hDlg, (WORD)(IDD_P51_PORT9 + unI))) {
            P051DispErr(hDlg, (WORD)(IDD_P51_PORT9 + unI));
            return TRUE;
        } else {
            /* ----- Set Port No. Data to Data Buffer ----- */
            ParaKDS[unI].usNHPOSPort = (USHORT)unPort2;
        }
    }

    /* ---- Write Initial Data from Parameter File ----- */
	ParaAllWrite(CLASS_PARAKDSIP, (UCHAR *)ParaKDS, sizeof(ParaKDS), 0, &usReturnLen);
	ParaAllWrite(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);

    /* ----- Set File Status Flag ----- */
    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P051DispErr()
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
VOID    P051DispErr(HWND hDlg, WPARAM wID)
{
    WORD    wMin,
            wMax;
    WCHAR    szWork[PEP_OVER_LEN],
            szMessage[PEP_OVER_LEN],
            szCaption[PEP_CAPTION_LEN];

    /* ----- Load String from Resource ----- */

		LoadString(hResourceDll, IDS_PEP_CAPTION_P51, szCaption, PEP_CAPTION_LEN);
		LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */

    wMin = (WORD)(((IDD_P51_IPADD11 <= LOWORD(wID) ) && (LOWORD(wID) <= IDD_P51_IPADD84)) ?
                 P51_IPPOS_MIN : P51_DATA_MIN);

    wMax = (WORD)(((IDD_P51_IPADD11 <= LOWORD(wID) ) && (LOWORD(wID) <= IDD_P51_IPADD84)) ?
                 P51_IPPOS_MAX : P51_DATA_MAX);

    wsPepf(szMessage, szWork, wMin, wMax);

    /* ----- Display Error Message with MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp (hDlg,
               szMessage,
               szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */

    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}




/*
* ===========================================================================
**  Synopsis    :   BOOL    P051ChkRndPos()
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
BOOL    P051ChkRndPos(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    short   nRndPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */

    nRndPos = (short)GetDlgItemInt(hDlg, LOWORD(wID), NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */

        switch (LOWORD(wID)) {
        
        case    IDD_P51_IPADD11:
        case    IDD_P51_IPADD12:
        case    IDD_P51_IPADD13:
        case    IDD_P51_IPADD14:
        case    IDD_P51_IPADD21:
        case    IDD_P51_IPADD22:
        case    IDD_P51_IPADD23:
        case    IDD_P51_IPADD24:
        case    IDD_P51_IPADD31:
        case    IDD_P51_IPADD32:
        case    IDD_P51_IPADD33:
        case    IDD_P51_IPADD34:
        case    IDD_P51_IPADD41:
        case    IDD_P51_IPADD42:
        case    IDD_P51_IPADD43:
        case    IDD_P51_IPADD44:

		//*********************************
		//	SR 14 ESMITH
        case    IDD_P51_IPADD51:
        case    IDD_P51_IPADD52:
        case    IDD_P51_IPADD53:
        case    IDD_P51_IPADD54:
        case    IDD_P51_IPADD61:
        case    IDD_P51_IPADD62:
        case    IDD_P51_IPADD63:
        case    IDD_P51_IPADD64:
        case    IDD_P51_IPADD71:
        case    IDD_P51_IPADD72:
        case    IDD_P51_IPADD73:
        case    IDD_P51_IPADD74:
        case    IDD_P51_IPADD81:
        case    IDD_P51_IPADD82:
        case    IDD_P51_IPADD83:
        case    IDD_P51_IPADD84:
		//*********************************

            if ((nRndPos < P51_IPPOS_MIN) || (nRndPos > P51_IPPOS_MAX))
            {

                 fCheck = TRUE;
            }

        case    IDD_P51_PORT1:
        case    IDD_P51_PORT2:
        case    IDD_P51_PORT3:
        case    IDD_P51_PORT4:
		//*********************************
		//	SR 14	ESMITH
        case    IDD_P51_PORT5:
        case    IDD_P51_PORT6:
        case    IDD_P51_PORT7:
        case    IDD_P51_PORT8:
		case    IDD_P51_PORT9:
		case    IDD_P51_PORT10:
		case    IDD_P51_PORT11:
		case    IDD_P51_PORT12:
		case    IDD_P51_PORT13:
		case    IDD_P51_PORT14:
		case    IDD_P51_PORT15:
		case    IDD_P51_PORT16:
		//*********************************
            if ((nRndPos < P51_DATA_MIN) || (nRndPos > P51_DATA_MAX))
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
**  Synopsis    :   VOID    P051InitData()
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
VOID    P051InitData(HWND hDlg)
{
   	PARAKDS ParaKDS[P51_MAX_ROW]; //SR 14 ESMITH 
    UINT    unI1, unValue;
    USHORT  usReturnLen;
    WCHAR    szDesc[128];

    /* ----- set initial description, V3.3 ----- */
	LoadString(hResourceDll, IDS_P51_DESC1, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR1, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC2, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR2, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC3, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR3, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC4, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR4, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC5, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR5, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC6, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR6, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC7, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR7, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC8, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR8, szDesc);
	//**************************************
	//	SR 14 ESMITH
	//
	LoadString(hResourceDll, IDS_P51_DESC9, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR9, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC10, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR10, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC11, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR11, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC12, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR12, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC13, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR13, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC14, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR14, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC15, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR15, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC16, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR16, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC17, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR17, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC18, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR18, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC19, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR19, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC20, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR20, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC21, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR21, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC22, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR22, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC23, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR23, szDesc);
	LoadString(hResourceDll, IDS_P51_DESC24, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STR24, szDesc);
	//**************************************
	LoadString(hResourceDll, IDS_P51_DELILEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STRDELILEN, szDesc);
	LoadString(hResourceDll, IDS_P51_POSLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STRPOSLEN, szDesc);
	LoadString(hResourceDll, IDS_P51_POSLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P51_STRPOSLEN2, szDesc);


    /* ---- Read Initial Data from Parameter File ----- */
	ParaAllRead(CLASS_PARAKDSIP, (UCHAR *)ParaKDS, sizeof(ParaKDS), 0, &usReturnLen);
	ParaAllRead(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);

	UINT uiChecked = LclParaMDCRead(MDC_DSP_ON_FLY3, ODD_MDC_BIT0);
	CheckDlgButton(hDlg, IDD_P51_MDC363BITD, uiChecked);

    /* Get IP Address */
    for (unI1 = 0; unI1 < P51_MAX_COL; unI1++)
    {
         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD11 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD1POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg,  IDD_P51_IPADD11 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD21 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD2POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg, IDD_P51_IPADD21 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD31 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD3POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg, IDD_P51_IPADD31 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD41 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD4POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg, IDD_P51_IPADD41 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD51 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD5POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg,  IDD_P51_IPADD51 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD61 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD6POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg, IDD_P51_IPADD61 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD71 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD7POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg,  IDD_P51_IPADD71 + unI1, unValue, FALSE);

         /* ----- Limit Length of Input Data to EditText (IP Address) ----- */
         SendDlgItemMessage(hDlg, IDD_P51_IPADD81 + unI1, EM_LIMITTEXT, P51_MAX_IPLEN, 0L);

        /* ----- Set IP Address Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[P51_IPADD8POS].uchIpAddress[unI1];
        SetDlgItemInt(hDlg, IDD_P51_IPADD81 + unI1, unValue, FALSE);
    }
    /* Get Port No.*/

    for(unI1 = 0; unI1 < P51_MAX_ROW; unI1++)
    {
        /* ----- Limit Length of Input Data to EditText (Port No.) ----- */
        SendDlgItemMessage(hDlg, IDD_P51_PORT1 + unI1, EM_LIMITTEXT, P51_MAX_PORTLEN, 0L);

        /* ----- Set Port No. Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[unI1].usPort;
        SetDlgItemInt(hDlg, IDD_P51_PORT1 + unI1, unValue, FALSE);
    }

	//***********************
	// SR 14 ESMITH
	//***********************

    /* Get Interface Port No.*/
    for(unI1 = 0; unI1 < P51_MAX_ROW; unI1++)
    {
        /* ----- Limit Length of Input Data to EditText (Port No.) ----- */
        SendDlgItemMessage(hDlg, IDD_P51_PORT9 + unI1, EM_LIMITTEXT, P51_MAX_PORTLEN, 0L);

        /* ----- Set Port No. Data to Specified EditText ----- */
        unValue = (UINT)ParaKDS[unI1].usNHPOSPort;
        SetDlgItemInt(hDlg, IDD_P51_PORT9 + unI1, unValue, FALSE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P051SpinProc()
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
VOID    P051SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    int         nEdit;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep      = P51_SPIN_STEP;
    PepSpin.nTurbo     = P51_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- Set edittext identifier ----- */

    nEdit = (int)(GetDlgCtrlID((HWND)lParam) - P51_SPIN_OFFSET);

    /* ----- Set minimum and maximum ----- */

    switch (nEdit) {

        case    IDD_P51_IPADD11:
        case    IDD_P51_IPADD12:
        case    IDD_P51_IPADD13:
        case    IDD_P51_IPADD14:
        case    IDD_P51_IPADD21:
        case    IDD_P51_IPADD22:
        case    IDD_P51_IPADD23:
        case    IDD_P51_IPADD24:
        case    IDD_P51_IPADD31:
        case    IDD_P51_IPADD32:
        case    IDD_P51_IPADD33:
        case    IDD_P51_IPADD34:
        case    IDD_P51_IPADD41:
        case    IDD_P51_IPADD42:
        case    IDD_P51_IPADD43:
        case    IDD_P51_IPADD44:
		//*******************************
		//	SR 14 ESMITH
        case    IDD_P51_IPADD51:
        case    IDD_P51_IPADD52:
        case    IDD_P51_IPADD53:
        case    IDD_P51_IPADD54:
        case    IDD_P51_IPADD61:
        case    IDD_P51_IPADD62:
        case    IDD_P51_IPADD63:
        case    IDD_P51_IPADD64:
        case    IDD_P51_IPADD71:
        case    IDD_P51_IPADD72:
        case    IDD_P51_IPADD73:
        case    IDD_P51_IPADD74:
        case    IDD_P51_IPADD81:
        case    IDD_P51_IPADD82:
        case    IDD_P51_IPADD83:
        case    IDD_P51_IPADD84:

        PepSpin.lMax = (long)P51_IPPOS_MAX;

        PepSpin.lMin = (long)P51_IPPOS_MIN;

        break;

    default:

        PepSpin.lMax = (long)P51_DATA_MAX;

        PepSpin.lMin = (long)P51_DATA_MIN;
    }

    /* ----- SpinButton Procedure ----- */

    PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);
}



/* ===== End of P051.C ===== */
