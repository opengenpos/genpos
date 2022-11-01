/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Set Hotel Id & SLD (Prog. 6) 
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P054.C
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
* Dec-07-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
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

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p054.h"

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
**  Synopsis    :   BOOL    WINAPI  P054DlgProc()
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
*       Program Mode Security Code (Program Mode # 54).
* ===========================================================================
*/
BOOL    WINAPI  P054DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;           /* SpinButton Configulation         */
    USHORT  usReturnLen;               /* Return Length of ParaAll         */
    WCHAR    szWork[MAX_HOTELID_SIZE * MAX_HOTELID_NO];
    
	CPMDATA   auchData;  /* Read Buffer */
    WORD    wI,
            wID;
    int     idEdit;
    

    switch (wMsg) {

    case WM_INITDIALOG:

 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Read Initial Data from Parameter File ----- */

        ParaAllRead(CLASS_PARAHOTELID,
                   (UCHAR *)&auchData,
                   sizeof(CPMDATA),
                   0, &usReturnLen);

        for (wI = 0, wID = IDD_P54_HOTEL; wI < 24; wI++, wID++) {

           /* Clear auchData */
              
              memset(szWork, '\0', sizeof(szWork));

              switch (wID) {

                   case IDD_P54_HOTEL:
                      memcpy(szWork, auchData.auchHotelId, sizeof(auchData.auchHotelId));
                      /* ----- Limit Maximum Input Length to EditText ----- */
                      SendDlgItemMessage(hDlg, IDD_P54_HOTEL, EM_LIMITTEXT, PARA_HOTELID_LEN, 0L);
                      break;

                   case IDD_P54_SLD:
                      memcpy(szWork, auchData.auchSld, sizeof(auchData.auchSld));
                      /* ----- Limit Maximum Input Length to EditText ----- */
                      SendDlgItemMessage(hDlg, IDD_P54_SLD, EM_LIMITTEXT, PARA_SLD_LEN, 0L);
                      break;

                   case IDD_P54_PRODUCT_ID:
                      memcpy(szWork, auchData.auchProductId, sizeof(auchData.auchProductId));
                      /* ----- Limit Maximum Input Length to EditText ----- */
                      SendDlgItemMessage(hDlg, IDD_P54_PRODUCT_ID,
                                         EM_LIMITTEXT, PARA_PDT_LEN, 0L);
                      break;

                   case IDD_P54_APRVL_CODE:
                      memcpy(szWork, auchData.auchApproval, sizeof(auchData.auchApproval));
                      /* ----- Limit Maximum Input Length to EditText ----- */
                      SendDlgItemMessage(hDlg, IDD_P54_APRVL_CODE,
                                         EM_LIMITTEXT, PARA_APVL_CODE_LEN, 0L);
                      break;

                   default:
					   if(wID >= IDD_P54_TEND1 && wID <= IDD_P54_TEND20) {
						  //memcpy(szWork, auchData.auchTend, sizeof(auchData.auchTend));
						   wcsncpy(szWork,&auchData.auchTend[wID - IDD_P54_TEND1], PARA_TEND_LEN);
						  /* ----- Limit Maximum Input Length to EditText ----- */
						  SendDlgItemMessage(hDlg, wID/*IDD_P54_TEND1 + (wI - 3)*/,
											 EM_LIMITTEXT, PARA_TEND_LEN, 0L);
					   }
                       break;

              }

           /* ----- Set Initial Data to Hotel Id EditText ----- */

              DlgItemRedrawText(hDlg, wID, szWork);

        };

        /* ----- Initialize Configulation of SpinButton ----- */

        PepSpin.lMin       = P54_SLD_MIN;
        PepSpin.lMax       = P54_SLD_MAX;
        PepSpin.nStep      = P54_SPIN_STEP;
        PepSpin.nTurbo     = P54_SPIN_TURBO;
        PepSpin.nTurStep   = P54_SPIN_TURSTEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

    case WM_VSCROLL:

        /* ----- Get SpinButton ID ----- */

        idEdit = GetDlgCtrlID((HWND)lParam);
        
        /* ----- Check if SpinButton and Change Spin Max  */
        
        if (idEdit > IDD_P54_SPIN3) { /* Action code of Tender */
            PepSpin.lMin       = P54_TEND_MIN;
            PepSpin.lMax       = P54_TEND_MAX;
        } else {
            PepSpin.lMin       = P54_SLD_MIN;
            PepSpin.lMax       = P54_SLD_MAX;
        }
        
        /* ----- Set EditText ID with SpinButton ID ----- */

        idEdit -= P54_SPIN_OFFSET;

        /* ----- Common SpinButton Procedure (See => Pepcomm.c) ----- */

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;   

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P54_HOTEL; j<=IDD_P54_SLD_RANGE; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
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

                    if (P54SaveData(hDlg)) {

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
**  Synopsis    :   BOOL    P54SaveData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range.
*                           FALSE   -   Inputed Data is Correct.
*
**  Description :
*       This function gets the inputed data from each edittext. And then it
*   checks whether the data is out of range or not. It returns TRUE, if the
*   data is out of range. Otherwise it writes the data to Parameter File, and
*   returns FALSE.
* ===========================================================================
*/
BOOL    P54SaveData(HWND hDlg)
{
    WCHAR   auchData[sizeof(CPMDATA)];  /* Read Buffer */
    WORD    wI,
            wID,
            wSize;
    int     iReturnLen;
    BOOL    fRet = FALSE;

    memset(auchData, '\0', sizeof(auchData));

    for (wI = 0, wID = IDD_P54_HOTEL, wSize = 0; 
            wI < 24; wI++, wID++) {

        /* ----- Determine Whether the Data is Out of Range or Not ----- */
        if (P54ChkRng(hDlg, wID, &auchData[wSize])
            == FALSE) {

            /* ----- Display Error Message of Over Range ----- */

            P54DispError(hDlg, wID);

            fRet = TRUE;
            break;
        }
        switch (wID) {
            case IDD_P54_HOTEL: wSize += PARA_HOTELID_LEN;  
                 break;
            case IDD_P54_SLD:   wSize += PARA_SLD_LEN;      
                 break;
            case IDD_P54_PRODUCT_ID: wSize += PARA_PDT_LEN; 
                 break;
            case IDD_P54_APRVL_CODE: wSize += PARA_APVL_CODE_LEN; 
                 break;
            default: wSize += 1;
                 break;
        }
    }

    if (fRet == FALSE) {

        /* ----- Write Inputed Data to Parameter File ----- */

        ParaAllWrite(CLASS_PARAHOTELID,
                     (UCHAR *)auchData,
                     sizeof(CPMDATA),
                     0, &(USHORT)iReturnLen);

        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P54ChkRng(HWND hDlg, WORD wEditID, UCHAR *puchData)
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wEditID     -   EditText ID for Check Range
*
*   Output      :   UCHAR  *puchData    -   Inputed Data in EditText.
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range.
*                           FALSE   -   Inputed Data is Correct.
**  Description :
*       This function checks whether the inputed data is out of range or not.
*   It returns TRUE, if the data is out of range. Otherwise it retures FALSE.
* ===========================================================================
*/
BOOL    P54ChkRng(HWND hDlg, WORD wEditID, WCHAR *puchData)
{
    WCHAR    szWork[MAX_HOTELID_SIZE * 4];
    int     i, iSize, iInputLen;

    /* Set Length each item */

    if (wEditID == IDD_P54_HOTEL) {
        iSize = PARA_HOTELID_LEN;
    } else if (wEditID == IDD_P54_SLD) {
        iSize = PARA_SLD_LEN;
    } else if (wEditID == IDD_P54_PRODUCT_ID) {
        iSize = PARA_PDT_LEN;
    } else if (wEditID == IDD_P54_APRVL_CODE) {
        iSize = PARA_APVL_CODE_LEN;
    } else {
        iSize = PARA_TEND_LEN;
    }
    /* ----- Get Inputed Data from EditText ----- */

    DlgItemGetText(hDlg, wEditID, szWork, sizeof(szWork)); 

    /* ----- Get inputed Data length ------------ */

    for (iInputLen = 0; ; iInputLen++) {
        if (*(szWork + iInputLen) == '\0')
            break;
    }

    /* ----- Convert String Type Data to DWORD Type Data ----- */

    if (wEditID == IDD_P54_HOTEL) {
        /* Check alphaNumeric */
        for (i = 0; i < PARA_HOTELID_LEN && i < iInputLen; i++) {
            if (IsPepCharAlphaNumeric(*(szWork + i)) == FALSE) {
                return FALSE;
            }
        }
    } else if (wEditID == IDD_P54_PRODUCT_ID) { /* must input 10 character */
        for (i = 0; i < iSize ; i++) {
            if ((WIDE('0') > *(szWork + i)) || (*(szWork + i) > WIDE('9'))) {
                 return FALSE;
            }
        }
    } else if (wEditID == IDD_P54_APRVL_CODE) {
        /* Check AlphaNumeric */
        for (i = 0; i < PARA_APVL_CODE_LEN && i < iInputLen; i++) {
            if (IsPepCharAlphaNumeric(*(szWork + i)) == FALSE) {
                return FALSE;
            }
        }
    } else {
        for (i = 0; i < iSize && i < iInputLen; i++) {
            if (('0' > *(szWork + i)) || (*(szWork + i) > '9')) {
                 return FALSE;
            }
        }
    }

   memcpy(puchData, szWork, iSize * sizeof(WCHAR));

    return TRUE;
}
/*
* ===========================================================================
**  Synopsis    :   VOID    P54DispError(HWND hDlg, WORD wErrID)
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wErrID  -   EditText ID of Over Range Error
*
**  Return      :   No return value
*
**  Description :
*       This function displays error message, and beeps error tone.
* ===========================================================================
*/
VOID    P54DispError(HWND hDlg, WORD wErrID)
{
    WCHAR    szCaption[PEP_CAPTION_LEN],
            szErrMsg[128],
            szMessage[128];

    /* ----- Load Caption and Error Message ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_P54, szCaption, PEP_STRING_LEN_MAC(szCaption));

    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Make Error Message with Data Range ----- */
    if (wErrID == IDD_P54_HOTEL) {
        wsPepf(szErrMsg,szMessage, P54_HOTELID_MIN, P54_HOTELID_MAX);
    } else if (wErrID == IDD_P54_SLD) {
        wsPepf(szErrMsg, szMessage, P54_SLD_MIN, P54_SLD_MAX);
    } else if (wErrID == IDD_P54_PRODUCT_ID) {
        LoadString(hResourceDll, IDS_P54_OVER, szMessage, PEP_STRING_LEN_MAC(szMessage));
        wsPepf(szErrMsg, szMessage);
    } else if (wErrID == IDD_P54_APRVL_CODE) {
        wsPepf(szErrMsg, szMessage, P54_APRVL_CODE_MIN, P54_APRVL_CODE_MAX);
    } else if (wErrID >= IDD_P54_TEND1) {
        wsPepf(szErrMsg, szMessage, P54_TEND_MIN, P54_TEND_MAX);
    } 

    /* ----- Display MessageBox ----- */

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szErrMsg, szCaption,
               MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set HighLight to EditText ----- */

    SendDlgItemMessage(hDlg, wErrID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wErrID));
}

/* ===== End of P054.C ===== */

