/*
* ---------------------------------------------------------------------------
* Title         :   TAX Table Maintenance (AC 124,125,126,129)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A124.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jul-04-93 : 01.00.06 : M.Ozawa    : Correct Tax Table Save Status
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
#include    <stdlib.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a124.h"

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
static  WCHAR           szTaxType[A124_LEN_BUFF];
static  WCHAR            szTaxRate[A124_LEN_BUFF];
static  A124TAXRATE     TaxRate[A124_NUM_TYPE];
static  LPA124TAXDATA   lpTaxData;
static  USHORT          usTaxWdc;
static  WORD            wTaxItem;
static  BOOL            fTaxStat;
static  PARAMDC         MdcData;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  A124DlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialogbox procedure to maintenance the TAX Table.
* ===========================================================================
*/
BOOL    WINAPI  A124DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL         ahGlblMem[A124_NUM_TYPE];
    static  A124FILEDATA    FileData;
    static  A124BUFFDATA    BuffData;

    WORD    wI;
    BOOL    fTest;
    USHORT  usLen;

    switch (wMsg) {
    case    WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- set init data ----- */

        A124SetInitData(hDlg, ahGlblMem, &FileData, &BuffData);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A124_TEST; j<=IDD_A124_CAPTION3; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_ENTER, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
        case    IDD_SET:

            /* ----- set the entry data in the TAX Table listbox ----- */

            fTest = A124SetTaxData(hDlg, &BuffData);

            if (fTest == TRUE) {            /* check WDC */

                /* ----- sound table complete beep ----- */

                MessageBeep(MB_OK);

                /* ----- Set test mode status ----- */

                A124SetTestStat(hDlg);

            }

            return TRUE;

        case    IDD_A124_CHG:

            /* ----- change data of table listbox ----- */

            A124ChgTblData(hDlg, &BuffData);

            return TRUE;

        case    IDD_A124_DEL:

            /* ----- delete data from table listbox ----- */

            A124DelTblData(hDlg, &BuffData);

            return TRUE;

        case    IDD_A124_CLR:

            if (IDOK == A124DspError(hDlg,
                                     IDS_A124_DELETE,
                                     MB_OKCANCEL | MB_ICONQUESTION)) {

                /* ----- clear all data (table and rate) ----- */

                A124ClrAllData(hDlg, &BuffData);

            }
            return TRUE;

        case    IDD_A124_TESTMODE:

            /* ----- get number of items in the listbox ----- */

            wTaxItem = (WORD)SendDlgItemMessage(hDlg,
                                                IDD_A124_TBLLIST,
                                                LB_GETCOUNT,
                                                0,
                                                0L);
        
            /* ----- create the dialogbox to test tax value ----- */

            DialogBoxPopup(hResourceDll,
                      MAKEINTRESOURCEW(IDD_A124_TEST),
                      hDlg,
                      A124TestDlgProc);

            return TRUE;

        case    IDD_ENTER:

            /* ----- set tax rate ----- */

            A124SetTaxRate(hDlg);

            return TRUE;

        case    IDD_A124_TBLLIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {    /* click listbox item */

                /* ----- display selected data in table listbox ----- */

                A124DspTblData(hDlg);

                return TRUE;
            }
            return FALSE;

        case    IDD_A124_TAXTYPE1:
        case    IDD_A124_TAXTYPE2:
        case    IDD_A124_TAXTYPE3:
        case    IDD_A124_TAXTYPE4:

            /* ----- change edit tax type ----- */

            A124ChgEditType(hDlg, wParam, &BuffData);

            return TRUE;

        case    IDOK:

            /* ----- query unsaving data ----- */

            if (A124QuerySaveData(hDlg, TaxRate)) {   /* select a cancel */
                return TRUE;                    /* return noting to do */
            }

            /* ----- write the new tax table data ----- */

            A124ConvWriteData(&FileData, &BuffData);
            ParaAllWrite(CLASS_PARATAXTBL1,
                         (UCHAR *)&FileData,
                         sizeof(FileData),
                         0,
                         &usLen);

            /* ----- Set file status flag ----- */

            PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);

            /* ----- return not used ----- */

        case    IDCANCEL:

            /* ----- release data area ----- */

            for (wI = 0; wI < A124_NUM_TYPE; wI++) {
                GlobalUnlock(ahGlblMem[wI]);
                GlobalFree(ahGlblMem[wI]);
            }

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
**  Synopsis:   VOID    A124SetInitData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               LPHGLOBAL       lphMem - address of global memory handle
*               LPA124FILEDATA  lpFile - address of file data
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure sets initial data.
* ===========================================================================
*/
VOID    A124SetInitData(HWND hDlg,
                        LPHGLOBAL lphMem,
                        LPA124FILEDATA lpFile,
                        LPA124BUFFDATA lpBuff)
{
    USHORT  usLen;
    WCHAR    szBuff[A124_LEN_BUFF];
    WCHAR    szType[A124_LEN_BUFF];
    UINT    wI, wType, wID;

    /* ----- get data-saved-area from global heap ----- */

    *(lphMem + A124_OFS_TAX1) = GlobalAlloc(GHND, sizeof(A124TAXDATA) * MAX_TTBLDATA_SIZE);
    lpBuff->alpTaxData[A124_OFS_TAX1] = (LPA124TAXDATA)GlobalLock(*(lphMem + A124_OFS_TAX1));
    *(lphMem + A124_OFS_TAX2) = GlobalAlloc(GHND, sizeof(A124TAXDATA) * MAX_TTBLDATA_SIZE);
    lpBuff->alpTaxData[A124_OFS_TAX2] = (LPA124TAXDATA)GlobalLock(*(lphMem + A124_OFS_TAX2));
    *(lphMem + A124_OFS_TAX3) = GlobalAlloc(GHND, sizeof(A124TAXDATA) * MAX_TTBLDATA_SIZE);
    lpBuff->alpTaxData[A124_OFS_TAX3] = (LPA124TAXDATA)GlobalLock(*(lphMem + A124_OFS_TAX3));
    *(lphMem + A124_OFS_TAX4) = GlobalAlloc(GHND, sizeof(A124TAXDATA) * MAX_TTBLDATA_SIZE);
    lpBuff->alpTaxData[A124_OFS_TAX4] = (LPA124TAXDATA)GlobalLock(*(lphMem + A124_OFS_TAX4));

    lpTaxData = lpBuff->alpTaxData[0];

    memset(TaxRate, 0, sizeof(TaxRate));
    TaxRate[A124_OFS_TAX1].usWdcCount = A124_INIT_COUNT;
    TaxRate[A124_OFS_TAX2].usWdcCount = A124_INIT_COUNT;
    TaxRate[A124_OFS_TAX3].usWdcCount = A124_INIT_COUNT;
    TaxRate[A124_OFS_TAX4].usWdcCount = A124_INIT_COUNT;

    /* ----- set rate data length ----- */

    SendDlgItemMessage(hDlg,
                       IDD_A124_QUOTI,
                       EM_LIMITTEXT,
                       (WPARAM)A124_LEN_RATE,
                       0L);
    SendDlgItemMessage(hDlg,
                       IDD_A124_NUMER,
                       EM_LIMITTEXT,
                       (WPARAM)A124_LEN_RATE,
                       0L);
    SendDlgItemMessage(hDlg,
                       IDD_A124_DENOM,
                       EM_LIMITTEXT,
                       (WPARAM)A124_LEN_RATE,
                       0L);
    SendDlgItemMessage(hDlg,
                       IDD_A124_SAMT2,
                       EM_LIMITTEXT,
                       (WPARAM)A124_LEN_SAMT,
                       0L);
    SendDlgItemMessage(hDlg,
                       IDD_A124_TAMT,
                       EM_LIMITTEXT,
                       (WPARAM)A124_LEN_TAMT,
                       0L);

    /* ----- get tax mode (US/canada) in MDC data ----- */

    MdcData.usAddress = A124_TAX_ADDR;
    ParaMDCRead(&MdcData);
    if (MdcData.uchMDCData & A124_TAX_CHK) {
        LoadString(hResourceDll, IDS_A124_TYPE_CN, szType, PEP_STRING_LEN_MAC(szType));
        wType = A124_NUM_TYPE;
        wID = IDS_A124_TYPE_CN1;
        ShowWindow(GetDlgItem(hDlg, IDD_A124_TAXTYPE4), SW_SHOW);
    } else {
        LoadString(hResourceDll, IDS_A124_TYPE_US, szType, PEP_STRING_LEN_MAC(szType));
        wType = A124_NUM_TYPE - 1;
        wID = IDS_A124_TYPE_US1;
        ShowWindow(GetDlgItem(hDlg, IDD_A124_TAXTYPE4), SW_HIDE);
    }

    /* ----- set tax type description ----- */

    DlgItemRedrawText(hDlg, IDD_A124_TAXTYPE, szType);
    for (wI = 0; wI < wType; wI++) {
        LoadString(hResourceDll, wID + wI, szType, PEP_STRING_LEN_MAC(szType));
        DlgItemRedrawText(hDlg, IDD_A124_TAXTYPE1 + wI, szType);
    }

    /* ----- disable the buttons until enter button is selected ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_TESTMODE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_CLR),      FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),           FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_CHG),      FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_DEL),      FALSE);

    /* ----- disable the edit controls ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_SAMT2), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_TAMT),  FALSE);

    /* ----- read the new tax table data ----- */

    ParaAllRead(CLASS_PARATAXTBL1,
                (UCHAR *)lpFile,
                sizeof(A124FILEDATA),
                0,
                &usLen);

    A124ConvReadData(lpFile, lpBuff);
    fTaxStat = TaxRate[0].fStatus;

    /* ----- set a initial sales amount ----- */

    LoadString(hResourceDll, IDS_A124_INIT_SAMT, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szBuff);

    /* ----- set a check mark (default : tax1/GST) ----- */

    SendDlgItemMessage(hDlg, IDD_A124_TAXTYPE1, BM_SETCHECK, TRUE, 0L);

    /* ----- set description of the tax table condition ----- */

    LoadString(hResourceDll, IDS_A124_CONDIT_1, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_CONDIT, szBuff);

    /* ----- set reading data(tax1/GST) as a initial data ----- */

    A124ChgEditType(hDlg, IDD_A124_TAXTYPE1, lpBuff);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124SetTaxRate()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure sets tax rate.
* ===========================================================================
*/
VOID    A124SetTaxRate(HWND hDlg)
{
    USHORT  usQuoti;
    USHORT  usNumer;
    USHORT  usDenom;
    WORD    wI;
    BOOL    fTrans;
    WCHAR    szWork[A124_LEN_BUFF];

    /* ----- get entered rates ----- */

    usQuoti = (USHORT)GetDlgItemInt(hDlg, IDD_A124_QUOTI, &fTrans, FALSE);
    usDenom = (USHORT)GetDlgItemInt(hDlg, IDD_A124_DENOM, &fTrans, FALSE);
    usNumer = (USHORT)GetDlgItemInt(hDlg, IDD_A124_NUMER, &fTrans, FALSE);

    if ((! usQuoti) && (! usDenom)) {

        /* ----- display error message ----- */

        A124DspError(hDlg,
                     IDS_A124_0INPUT,
                     MB_OK | MB_ICONEXCLAMATION);

        /* ----- reset focus to quotient ----- */

        SetFocus(GetDlgItem(hDlg, IDD_A124_QUOTI));

    } else {
        if ((usDenom && usNumer) || ((! usDenom) && (! usNumer))) {
                                                            /* xx/xx or 0/0 */

            /* ----- set Rate entered status ----- */

            fTaxStat |= A124_CHK_RATE;

            /* ----- change a display status to data entry ----- */

            A124SetEntStat(hDlg);

            /* ----- set enterd tax rate in the buffer ----- */

            LoadString(hResourceDll, IDS_A124_TAXRATE, szWork, PEP_STRING_LEN_MAC(szWork));
            wsPepf(szTaxRate, szWork, usQuoti, usNumer, usDenom);

            /* ----- save entered data ----- */

            if (fTaxStat & A124_CHK_TAX1) {
                wI = A124_OFS_TAX1;
            } else if (fTaxStat & A124_CHK_TAX2) {
                wI = A124_OFS_TAX2;
            } else if (fTaxStat & A124_CHK_TAX3) {
                wI = A124_OFS_TAX3;
            } else {
                wI = A124_OFS_TAX4;
            }
            TaxRate[wI].fStatus = fTaxStat;
            TaxRate[wI].usQuoti = usQuoti;
            TaxRate[wI].usNumer = usNumer;
            TaxRate[wI].usDenom = usDenom;

            /* ----- set WDC(Whole Dollar Constant) ----- */

            usTaxWdc = (USHORT)((usDenom == 0)
                                ? A124_WDC_PERCENT
                                : usDenom * A124_WDC_PERCENT);

            TaxRate[wI].usWdc = usTaxWdc;

            /* ----- set WDC information ----- */

            A124ShowWdc(hDlg);

        } else {                                        /* xx/0 or 0/xx */

            /* ----- display error message ----- */

            A124DspError(hDlg,
                         IDS_A124_RATEERROR,
                         MB_OK | MB_ICONEXCLAMATION);

            /* ----- reset focus to numerator ----- */

            SetFocus(GetDlgItem(hDlg, IDD_A124_NUMER));
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A124SetTaxData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This procedure sets tax data.
* ===========================================================================
*/
BOOL    A124SetTaxData(HWND hDlg, LPA124BUFFDATA lpBuff)
{
    DWORD   dwIdx;
    BOOL    fRet;
    WCHAR    szWork[A124_LEN_BUFF];
    WCHAR    szBuff[A124_LEN_BUFF];
    ULONG   ulPrev, ulAmt, ulTax;
    long    lAmtTmp, lTaxTmp;
    USHORT  usLen;

    /* ----- get Tax data length ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        usLen = lpBuff->TaxLen.usTaxLen1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        usLen = lpBuff->TaxLen.usTaxLen2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        usLen = lpBuff->TaxLen.usTaxLen3;
    } else {
        usLen = lpBuff->TaxLen.usTaxLen4;
    }

    /* ----- get sales amount ----- */

    memset(szWork, '\0', sizeof(szWork));
    DlgItemGetText(hDlg, IDD_A124_SAMT2, szWork, A124_LEN_SAMT + 1);
    lAmtTmp = _wtol(szWork);
    if (lAmtTmp < 0) {
        ulAmt = 0L;
    } else {
        ulAmt = (ULONG)lAmtTmp;
    }

    if ((ulAmt == 0L) && (usLen != 0)) {    /* check data is input */
        A124DspError(hDlg,
                  IDS_A124_NOINPUT,
                  MB_OK | MB_ICONEXCLAMATION);
        SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));
        return FALSE;
    }

    /* ----- get tax amount ----- */

    memset(szWork, '\0', sizeof(szWork));
    DlgItemGetText(hDlg, IDD_A124_TAMT, szWork, A124_LEN_TAMT + 1);
    lTaxTmp = _wtol(szWork);
    if (lAmtTmp < 0) {
        ulTax = 0L;
    } else {
        ulTax = (ULONG)lTaxTmp;
    }

    if (A124ChkEntData(ulAmt, ulTax, lpBuff)) {     /* check effective data or not ? */
        A124DspError(hDlg,
                  IDS_A124_SMALLER,
                  MB_OK | MB_ICONEXCLAMATION);
        SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));
        return FALSE;
    }

    if (fTaxStat & A124_CHK_TEST) {      /* check test mode flag */
        A124MakeDblTbl(hDlg);
    }

    if (A124QueryMaxData(ulAmt, ulTax, lpBuff)) {     /* check over 200 bytes */
        A124DspError(hDlg, IDS_A124_ERROVER, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    /* ----- get number of items in the listbox ----- */

    dwIdx = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCOUNT, 0, 0L);

    /* ----- set these data in the tax data buffer ----- */

    ulPrev = (dwIdx == 0) ?
            0L : (ULONG)((lpTaxData + (dwIdx - 1))->ulSlsAmt + 1L);

    (lpTaxData + dwIdx)->ulSlsAmt = ulAmt;
    (lpTaxData + dwIdx)->ulTaxAmt = ulTax;

    /* ----- edit sales & tax amount to insert listbox ----- */

    LoadString(hResourceDll, IDS_A124_RATELIST, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork,
             szBuff,
             ulPrev / A124_WDC_PERCENT, ulPrev % A124_WDC_PERCENT,
             ulAmt  / A124_WDC_PERCENT, ulAmt  % A124_WDC_PERCENT,
             ulTax  / A124_WDC_PERCENT, ulTax  % A124_WDC_PERCENT);

    /* ----- insert data in the listbox ----- */

    DlgItemSendTextMessage(hDlg,
                       IDD_A124_TBLLIST,
                       LB_INSERTSTRING,
                       (WPARAM)-1,
                       (LPARAM)szWork);

    /* ----- set focus to now inserted string ----- */

    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST,
                        LB_SETCURSEL, (WPARAM)dwIdx, 0L);

    /* ----- change sales amount 1 data ----- */

    ulPrev = ulAmt + 1L;
    LoadString(hResourceDll, IDS_A124_BUFF_SAMT, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork,
             szBuff,
             ulPrev / A124_WDC_PERCENT,
             ulPrev % A124_WDC_PERCENT);
    DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szWork);

    /* ----- clear edit control areas & set caret ----- */

    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, WM_CLEAR, 0, 0L);

    SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));

    if (ulAmt >= (ULONG)(usTaxWdc * A124_WDC_COMPLETE_CHK)) {
        fRet = TRUE;                       /* table is completed */
    } else {
        fRet = FALSE;                      /* table is not completed */
    }

    /* ----- store tax table length ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        lpBuff->TaxLen.usTaxLen1 = (USHORT)(dwIdx + 1);
    } else if (fTaxStat & A124_CHK_TAX2) {
        lpBuff->TaxLen.usTaxLen2 = (USHORT)(dwIdx + 1);
    } else if (fTaxStat & A124_CHK_TAX3) {
        lpBuff->TaxLen.usTaxLen3 = (USHORT)(dwIdx + 1);
    } else {
        lpBuff->TaxLen.usTaxLen4 = (USHORT)(dwIdx + 1);
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ChgTblData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure changes tax data.
* ===========================================================================
*/
VOID    A124ChgTblData(HWND hDlg, LPA124BUFFDATA lpBuff)
{
    DWORD   dwIdx, dwCo;
    BOOL    fChk;
    WCHAR    szWork[A124_LEN_BUFF];
    WCHAR    szBuff[A124_LEN_BUFF];
    ULONG   ulPrev, ulAmt, ulTax;

    /* ----- get a current 0 based offset of a tax table listbox ----- */

    dwIdx = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCURSEL, 0, 0L);

    /* ----- get sales amount ----- */

    memset(szWork, '\0', sizeof(szWork));
    DlgItemGetText(hDlg, IDD_A124_SAMT2, szWork, A124_LEN_SAMT + 1);
    ulAmt = _wtol(szWork);

    if ((ulAmt == 0L) && (dwIdx != 0L)) {    /* check data is input */
        A124DspError(hDlg,
                     IDS_A124_NOINPUT,
                     MB_OK | MB_ICONEXCLAMATION);
        SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));
        return;
    }

    /* ----- get tax amount ----- */

    memset(szWork, '\0', sizeof(szWork));
    DlgItemGetText(hDlg, IDD_A124_TAMT, szWork, A124_LEN_TAMT + 1);
    ulTax = _wtol(szWork);

    if (A124QueryMaxData(ulAmt, ulTax, lpBuff)) {     /* check over 200 bytes */
        A124DspError(hDlg, IDS_A124_ERROVER, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if ((fChk = A124ChkChgData(hDlg, ulAmt, ulTax, lpBuff)) != TRUE) {
                                            /* check moderat data or not */
        A124DspError(hDlg,
                     (WORD)(IDS_A124_CHKCHG_1 + fChk),
                     MB_OK | MB_ICONEXCLAMATION);
        A124DspTblData(hDlg);
        return;
    }

    /* ----- set these data in the tax data buffer ----- */

    ulPrev = (dwIdx == 0) ?
                0L : (ULONG)((lpTaxData + (dwIdx - 1))->ulSlsAmt + 1L);
    (lpTaxData + dwIdx)->ulSlsAmt = ulAmt;
    (lpTaxData + dwIdx)->ulTaxAmt = ulTax;

    /* ----- edit sales & tax amount to insert listbox ----- */

    LoadString(hResourceDll, IDS_A124_RATELIST, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork,
             szBuff,
             ulPrev / A124_WDC_PERCENT, ulPrev % A124_WDC_PERCENT,
             ulAmt  / A124_WDC_PERCENT, ulAmt  % A124_WDC_PERCENT,
             ulTax  / A124_WDC_PERCENT, ulTax  % A124_WDC_PERCENT);

    /* ----- delete a specified string ----- */

    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST,
                        LB_DELETESTRING, (WPARAM)dwIdx, 0L);

    /* ----- insert data in the listbox ----- */

    DlgItemSendTextMessage(hDlg,
                       IDD_A124_TBLLIST,
                       LB_INSERTSTRING,
                       (WPARAM)dwIdx,
                       (LPARAM)szWork);

    /* ----- set focus to previous inserted string ----- */

    SendDlgItemMessage(hDlg,
                       IDD_A124_TBLLIST,
                       LB_SETCURSEL,
                       (WPARAM)dwIdx,
                       0L);

    /* ----- change next sales amount 1 in the listbox ----- */

    dwCo = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCOUNT, 0, 0L);
    if (dwIdx != (dwCo - 1)) {
        ulPrev = ulAmt + 1L;
        ulAmt  = (lpTaxData + (dwIdx + 1))->ulSlsAmt;
        ulTax  = (lpTaxData + (dwIdx + 1))->ulTaxAmt;
        LoadString(hResourceDll, IDS_A124_RATELIST, szBuff, PEP_STRING_LEN_MAC(szBuff));
        wsPepf(szWork,
                 szBuff,
                 ulPrev / A124_WDC_PERCENT, ulPrev % A124_WDC_PERCENT,
                 ulAmt  / A124_WDC_PERCENT, ulAmt  % A124_WDC_PERCENT,
                 ulTax  / A124_WDC_PERCENT, ulTax  % A124_WDC_PERCENT);
        SendDlgItemMessage(hDlg, IDD_A124_TBLLIST,
                            LB_DELETESTRING, (WPARAM)(dwIdx+1), 0L);
        DlgItemSendTextMessage(hDlg, IDD_A124_TBLLIST, LB_INSERTSTRING,
                            (WPARAM)(dwIdx+1), (LPARAM)szWork);
    }

    /* ----- set caret ----- */

    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124DelTblData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure delete item.
* ===========================================================================
*/
VOID    A124DelTblData(HWND hDlg, LPA124BUFFDATA lpBuff)
{
    DWORD   dwIdx, dwCo;
    WCHAR    szWork[A124_LEN_BUFF];
    WCHAR    szBuff[A124_LEN_BUFF];
    ULONG   ulAmt, ulTax, ulPrev;
    WORD    wI;

    dwIdx = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCURSEL, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST,
                                        LB_DELETESTRING, (WPARAM)dwIdx, 0L);

    dwCo = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCOUNT, 0, 0L);

    if (dwCo == 0L) {
        if (dwIdx == 0L) {

            /* ----- clear end of buffer ----- */

            lpTaxData->ulSlsAmt = 0L;
            lpTaxData->ulTaxAmt   = 0L;

        }

        /* ----- reset data length ----- */

        if (fTaxStat & A124_CHK_TAX1) {
            wI = A124_OFS_TAX1;
            if (lpBuff->TaxLen.usTaxLen1) {
                lpBuff->TaxLen.usTaxLen1 = 0;
            }
        } else if (fTaxStat & A124_CHK_TAX2) {
            wI = A124_OFS_TAX2;
            if (lpBuff->TaxLen.usTaxLen2) {
                lpBuff->TaxLen.usTaxLen2 = 0;
            }
        } else if (fTaxStat & A124_CHK_TAX3) {
            wI = A124_OFS_TAX3;
            if (lpBuff->TaxLen.usTaxLen3) {
                lpBuff->TaxLen.usTaxLen3 = 0;
            }
        } else {
            wI = A124_OFS_TAX4;
            if (lpBuff->TaxLen.usTaxLen4) {
                lpBuff->TaxLen.usTaxLen4 = 0;
            }
        }

        /* ----- reset sales amount1 ----- */

        LoadString(hResourceDll, IDS_A124_INIT_SAMT, szWork, PEP_STRING_LEN_MAC(szWork));
        DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szWork);

        /* ----- clear edit control areas & set caret ----- */

        SendDlgItemMessage(hDlg, IDD_A124_SAMT2, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A124_SAMT2, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A124_TAMT,  EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A124_TAMT,  WM_CLEAR,  0, 0L);

        /* ----- reset test mode status ----- */

        A124ResetTestStat(hDlg);

        /* ----- save entered data ----- */

        TaxRate[wI].fStatus      = fTaxStat;

        return;
    }

    if (dwCo != dwIdx) {
        memmove((lpTaxData + dwIdx),
                  (lpTaxData + (dwIdx + 1)),
                  (UINT)(sizeof(A124TAXDATA) * (dwCo - dwIdx)));

        ulPrev = (dwIdx == 0) ?
                        0L : (lpTaxData + (dwIdx - 1))->ulSlsAmt + 1L;
        ulAmt  = (lpTaxData + dwIdx)->ulSlsAmt;
        ulTax  = (lpTaxData + dwIdx)->ulTaxAmt;
        LoadString(hResourceDll, IDS_A124_RATELIST, szBuff, PEP_STRING_LEN_MAC(szBuff));
        wsPepf(szWork,
                 szBuff,
                 ulPrev / A124_WDC_PERCENT, ulPrev % A124_WDC_PERCENT,
                 ulAmt  / A124_WDC_PERCENT, ulAmt  % A124_WDC_PERCENT,
                 ulTax  / A124_WDC_PERCENT, ulTax  % A124_WDC_PERCENT);
        SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_DELETESTRING,
                            (WPARAM)dwIdx, 0L);
        DlgItemSendTextMessage(hDlg, IDD_A124_TBLLIST, LB_INSERTSTRING,
                            (WPARAM)dwIdx, (LPARAM)szWork);
    }
    SendDlgItemMessage(hDlg,
                       IDD_A124_TBLLIST,
                       LB_SETCURSEL,
                       (WPARAM)((dwIdx) ? dwIdx - 1 : 0),
                       0L);

    /* ----- check the test mode status bit ----- */

    if (fTaxStat & A124_CHK_TEST) {

        if (dwIdx == dwCo) {

            /* ----- reset test mode status ----- */

            A124ResetTestStat(hDlg);

            /* ----- save entered data ----- */

            if (fTaxStat & A124_CHK_TAX1) {
                wI = A124_OFS_TAX1;
            } else if (fTaxStat & A124_CHK_TAX2) {
                wI = A124_OFS_TAX2;
            } else if (fTaxStat & A124_CHK_TAX3) {
                wI = A124_OFS_TAX3;
            } else {
                wI = A124_OFS_TAX4;
            }
            TaxRate[wI].fStatus = fTaxStat;
        }
    }

    /* ----- clear end of buffer ----- */

    (lpTaxData + dwCo)->ulSlsAmt = 0L;
    (lpTaxData + dwCo)->ulTaxAmt = 0L;

    /* ----- decrement item counter ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        if (lpBuff->TaxLen.usTaxLen1) {
            --(lpBuff->TaxLen.usTaxLen1);
        }
    } else if (fTaxStat & A124_CHK_TAX2) {
        if (lpBuff->TaxLen.usTaxLen2) {
            --(lpBuff->TaxLen.usTaxLen2);
        }
    } else if (fTaxStat & A124_CHK_TAX3) {
        if (lpBuff->TaxLen.usTaxLen3) {
            --(lpBuff->TaxLen.usTaxLen3);
        }
    } else {
        if (lpBuff->TaxLen.usTaxLen4) {
            --(lpBuff->TaxLen.usTaxLen4);
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ClrAllData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure clear all.
* ===========================================================================
*/
VOID    A124ClrAllData(HWND hDlg, LPA124BUFFDATA lpBuff)
{
    WORD    wI;
    DWORD   dwIdx;
    WCHAR    szBuff[A124_LEN_BUFF];

    dwIdx = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCOUNT, 0, 0L);

    /* ----- clear the tax listbox ----- */
    
    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_RESETCONTENT, 0, 0L);

    /* ----- clear the edit controls ----- */

    SendDlgItemMessage(hDlg, IDD_A124_QUOTI, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_QUOTI, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_NUMER, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_NUMER, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_DENOM, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_DENOM, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, WM_CLEAR, 0, 0L);

    /* ----- disable the buttons ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_TESTMODE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_CLR), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),      FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_CHG), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_DEL), FALSE);
    
    /* ----- disable the edit controls ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_SAMT2), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_TAMT),  FALSE);

    /* ----- enable the rate enter button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_ENTER), TRUE);

    /* ----- enable the edit controls ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_QUOTI), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_NUMER), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_DENOM), TRUE);

    /* ----- set caret to an edit control of a tax quotient ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A124_QUOTI));

    /* ----- set a initial sales amount ----- */

    LoadString(hResourceDll, IDS_A124_INIT_SAMT, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szBuff);

    /* ----- set description of the tax table condition ----- */

    LoadString(hResourceDll, IDS_A124_CONDIT_1, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_CONDIT, szBuff);

    /* ----- clear status flag ----- */

    fTaxStat &= ~(A124_CHK_TEST | A124_CHK_RATE);

    /* ----- clear the data area ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
        lpBuff->TaxLen.usTaxLen1 = 0;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
        lpBuff->TaxLen.usTaxLen2 = 0;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
        lpBuff->TaxLen.usTaxLen3 = 0;
    } else {
        wI = A124_OFS_TAX4;
        lpBuff->TaxLen.usTaxLen4 = 0;
    }
    memset(lpBuff->alpTaxData[wI], 0, sizeof(A124TAXDATA) * MAX_TTBLDATA_SIZE);

    memset(&TaxRate[wI], 0, sizeof(A124TAXRATE));
    TaxRate[wI].fStatus      = fTaxStat;
    TaxRate[wI].usWdcCount   = A124_INIT_COUNT;

    usTaxWdc = 0;
    A124ShowWdc(hDlg);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124DspTblData()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure displays selected item.
* ===========================================================================
*/
VOID    A124DspTblData(HWND hDlg)
{
    WCHAR    szWork[A124_LEN_BUFF];
    WCHAR    szBuff[A124_LEN_BUFF];
    DWORD   dwOff;
    ULONG   ulAmt;

    /* ----- get 0 based offset of listbox ----- */

    dwOff = SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCURSEL, 0, 0L);

    /* ----- show selected data ----- */

    ulAmt = (dwOff == 0) ? 0L : (lpTaxData + (dwOff - 1))->ulSlsAmt + 1L;
    LoadString(hResourceDll, IDS_A124_BUFF_SAMT, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork,
             szBuff,
             ulAmt / A124_WDC_PERCENT,
             ulAmt % A124_WDC_PERCENT);
    DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szWork);

    wsPepf(szWork, WIDE("%5lu"), (lpTaxData + dwOff)->ulSlsAmt);
    DlgItemRedrawText(hDlg, IDD_A124_SAMT2, szWork);

    wsPepf(szWork, WIDE("%3lu"), (lpTaxData + dwOff)->ulTaxAmt);
    DlgItemRedrawText(hDlg, IDD_A124_TAMT, szWork);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ChgEditType()
*
*   Input   :   HWND            hDlg  - handle of a dialogbox procedure
*               WORD            wType - tax type
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure changes data.
* ===========================================================================
*/
VOID    A124ChgEditType(HWND hDlg, WPARAM wType, LPA124BUFFDATA lpBuff)
{
    USHORT          usLen;
    WCHAR            szWork[A124_LEN_BUFF];
    LPA124TAXDATA   lpData;
    UINT            wID, wI;

    /* ----- save current status ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {
        wI = A124_OFS_TAX4;
    }
    TaxRate[wI].fStatus = fTaxStat;
    TaxRate[wI].usWdc   = usTaxWdc;

    /* ----- set changed data ----- */

    switch (LOWORD(wType)) {
    case    IDD_A124_TAXTYPE1:
        lpData = lpTaxData = lpBuff->alpTaxData[A124_OFS_TAX1];
        if (lpBuff->TaxLen.usTaxLen1) {
            usLen = lpBuff->TaxLen.usTaxLen1;
        } else {
            usLen = 0;
        }
        wI = A124_OFS_TAX1;
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            wID = IDS_A124_TYPE_CN1;
        } else {
            wID = IDS_A124_TYPE_US1;
        }
        break;

    case    IDD_A124_TAXTYPE2:
        lpData = lpTaxData = lpBuff->alpTaxData[A124_OFS_TAX2];
        if (lpBuff->TaxLen.usTaxLen2) {
            usLen = lpBuff->TaxLen.usTaxLen2;
        } else {
            usLen = 0;
        }
        wI = A124_OFS_TAX2;
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            wID = IDS_A124_TYPE_CN2;
        } else {
            wID = IDS_A124_TYPE_US2;
        }
        break;

    case    IDD_A124_TAXTYPE3:
        lpData = lpTaxData = lpBuff->alpTaxData[A124_OFS_TAX3];
        if (lpBuff->TaxLen.usTaxLen3) {
            usLen = lpBuff->TaxLen.usTaxLen3;
        } else {
            usLen = 0;
        }
        wI = A124_OFS_TAX3;
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            wID = IDS_A124_TYPE_CN3;
        } else {
            wID = IDS_A124_TYPE_US3;
        }
        break;

    default:        /* IDD_A124_TAXTYPE4 */
        lpData = lpTaxData = lpBuff->alpTaxData[A124_OFS_TAX4];
        if (lpBuff->TaxLen.usTaxLen4) {
            usLen = lpBuff->TaxLen.usTaxLen4;
        } else {
            usLen = 0;
        }
        wI = A124_OFS_TAX4;
        wID = IDS_A124_TYPE_CN4;
        break;
    }

    /* ----- restore previous status ----- */

    fTaxStat = TaxRate[wI].fStatus;

    /* ----- set new WDC ----- */

    usTaxWdc = TaxRate[wI].usWdc;

    /* ----- set new description ----- */

    LoadString(hResourceDll, wID, szTaxType, PEP_STRING_LEN_MAC(szTaxType));
    LoadString(hResourceDll, IDS_A124_TAXRATE, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szTaxRate,
             szWork,
             TaxRate[wI].usQuoti,
             TaxRate[wI].usNumer,
             TaxRate[wI].usDenom);

    /* ----- remove all items from the listbox ----- */

    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_RESETCONTENT, 0, 0L);

    /* ----- reset listbox items ----- */

    A124ResetListbox(hDlg, usLen, lpData);


    if (fTaxStat & A124_CHK_RATE) {  /* check rate status */

        /* ----- restore the status for every tax type ----- */

        A124SetEntStat(hDlg);

    } else {

        /* ----- clear all data (table and rate) ----- */

        A124ClrAllData(hDlg, lpBuff);

    }

    if (fTaxStat & A124_CHK_TEST) {  /* check test status */

        /* ----- set test status ----- */

        A124SetTestStat(hDlg);

    } else {

        /* ----- reset test status ----- */

        A124ResetTestStat(hDlg);

    }

    /* ----- show rates in the edit control ----- */

    A124ShowRate(hDlg, wType);

    /* ----- set WDC information ----- */

    A124ShowWdc(hDlg);

    /* ----- clear edit controls ----- */

    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_SAMT2, WM_CLEAR, 0, 0L);
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A124_TAMT, WM_CLEAR, 0, 0L);

    /* ----- clear previous sales amount ----- */

    LoadString(hResourceDll, IDS_A124_INIT_SAMT, szWork, PEP_STRING_LEN_MAC(szWork));
    DlgItemRedrawText(hDlg, IDD_A124_SAMT1, szWork);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ResetListbox()
*
*   Input   :   HWND            hDlg  - handle of a dialogbox procedure
*               USHORT          usLen - loop counter (number of items)
*               LPA124TAXDATA   lpTax - address of a listing Tax Data Buffer
**
*   Return  :   None
*
**  Description:
*       This procedure resets listbox.
* ===========================================================================
*/
VOID    A124ResetListbox(HWND hDlg, USHORT usLen, LPA124TAXDATA lpTax)
{
    ULONG   ulPrev, ulAmt = 0, ulTax;
    WORD    wI;
    WCHAR    szWork[A124_LEN_BUFF];
    WCHAR    szBuff[A124_LEN_BUFF];

    /* ----- get rate string from resource ----- */

    LoadString(hResourceDll, IDS_A124_RATELIST, szBuff, PEP_STRING_LEN_MAC(szBuff));

    for (wI = 0; wI < usLen; wI++) {

        /* ----- store tax data to buffer ----- */

        ulPrev = (wI == 0) ? 0L : ulAmt + 1L;
        ulAmt  = (lpTax + wI)->ulSlsAmt;
        ulTax  = (lpTax + wI)->ulTaxAmt;

        if ((wI != 0) && (ulAmt == 0L)) {   /* not exist table data */
            break;
        }

        /* ----- make a insert string ----- */

        wsPepf(szWork,
                 szBuff,
                 ulPrev / A124_WDC_PERCENT, ulPrev % A124_WDC_PERCENT,
                 ulAmt  / A124_WDC_PERCENT, ulAmt  % A124_WDC_PERCENT,
                 ulTax  / A124_WDC_PERCENT, ulTax  % A124_WDC_PERCENT);

        /* ----- insert data to the listbox ----- */

        DlgItemSendTextMessage(hDlg,
                           IDD_A124_TBLLIST,
                           LB_INSERTSTRING,
                           (WPARAM)-1,
                           (LPARAM)szWork);

    }

    /* ----- set a cursor to the end of a last inserted string ----- */

    SendDlgItemMessage(hDlg, IDD_A124_TBLLIST,
                                        LB_SETCURSEL, (WPARAM)(wI - 1), 0L);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ShowRate()
*
*   Input   :   HWND    hDlg  - handle of a dialogbox procedure
*               WORD    wType - tax type
**
*   Return  :   None
*
**  Description:
*       This procedure shows rate.
* ===========================================================================
*/
VOID    A124ShowRate(HWND hDlg, WPARAM wType)
{
    USHORT  usQuoti, usNumer, usDenom;
    WORD    wI;

    switch (LOWORD(wType)) {
    case IDD_A124_TAXTYPE1:
        wI = A124_OFS_TAX1;
        break;

    case IDD_A124_TAXTYPE2:
        wI = A124_OFS_TAX2;
        break;

    case IDD_A124_TAXTYPE3:
        wI = A124_OFS_TAX3;
        break;

    default:        /* IDD_A124_TAXTYPE4 */
        wI = A124_OFS_TAX4;
        break;
    }

    /* ----- store rate to buffer ----- */

    usQuoti = TaxRate[wI].usQuoti;
    usNumer = TaxRate[wI].usNumer;
    usDenom = TaxRate[wI].usDenom;

    /* ----- display rate ----- */

    SetDlgItemInt(hDlg, IDD_A124_QUOTI, usQuoti, FALSE);
    SetDlgItemInt(hDlg, IDD_A124_NUMER, usNumer, FALSE);
    SetDlgItemInt(hDlg, IDD_A124_DENOM, usDenom, FALSE);

    /* ----- set highlight ----- */

    SendDlgItemMessage(hDlg, IDD_A124_QUOTI, EM_SETSEL, 0, MAKELONG(0, -1));

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124SetEntStat()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
**
*   Return  :   None.
*
**  Description:
*       This procedure sets the status of Tax table to be entered a tax rate.
* ===========================================================================
*/
VOID    A124SetEntStat(HWND hDlg)
{
    /* ----- enable the buttons to set new data ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_CLR),   TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_SET),       TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_CHG),   TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_DEL),   TRUE);

    /* ----- enable the edit controls ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_SAMT2), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_TAMT),  TRUE);

    /* ----- disable the rate enter button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_ENTER), FALSE);

    /* ----- disable the edit controls ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_QUOTI), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_NUMER), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A124_DENOM), FALSE);

    /* ----- set focus to sales amount edit control ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A124_SAMT2));
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124SetTestStat()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure sets test status.
* ===========================================================================
*/
VOID    A124SetTestStat(HWND hDlg)
{
    WORD    wI;
    WCHAR    szBuff[A124_LEN_BUFF];

    /* ----- set test mode status ----- */

    fTaxStat |= A124_CHK_TEST;

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {
        wI = A124_OFS_TAX4;
    }
    TaxRate[wI].fStatus = fTaxStat;

    /* ----- enable test mode & OK buttons ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_TESTMODE), TRUE);
    
    /* ----- set description of the tax table condition ----- */

    LoadString(hResourceDll, IDS_A124_CONDIT_2, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_CONDIT, szBuff);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ResetTestStat()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
**
*   Return  :   None
*
**  Description:
*       This procedure resets test status.
* ===========================================================================
*/
VOID    A124ResetTestStat(HWND hDlg)
{
    WORD    wI;
    WCHAR    szBuff[A124_LEN_BUFF];

    /* ----- reset test mode status ----- */

    fTaxStat &= ~A124_CHK_TEST;

    /* correct TaxRate[wI].fStatus 07/04/96 */
    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {
        wI = A124_OFS_TAX4;
    }
    TaxRate[wI].fStatus = fTaxStat;

    /* ----- disable a test mode & OK button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A124_TESTMODE), FALSE);
    
    /* ----- set description of the tax table condition ----- */

    LoadString(hResourceDll, IDS_A124_CONDIT_1, szBuff, PEP_STRING_LEN_MAC(szBuff));
    DlgItemRedrawText(hDlg, IDD_A124_CONDIT, szBuff);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ConvReadData()
*
*   Input   :   LPA124FILEDATA  lpFile - address of file data
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure exchanges reading data.
* ===========================================================================
*/
VOID    A124ConvReadData(LPA124FILEDATA lpFile, LPA124BUFFDATA lpBuff)
{
    BOOL            fFirst;
    USHORT          usLen, usOfs, usI, usSetCo;
    ULONG           ulTax, ulAmt, ulPrev, ulOver;
    LPA124TAXDATA   lpTemp;
    A124TAXLEN      TaxLen;

    /* ----- set tax table data length ----- */

    memcpy(&(lpBuff->TaxLen), &(lpFile->TaxLen), sizeof(A124TAXLEN));

    /* ----- make tax1 data ----- */

    ulOver = 0L;
    lpTemp  = lpBuff->alpTaxData[A124_OFS_TAX1];
    usLen  = (USHORT)(lpBuff->TaxLen.usTaxLen1 - A124_SIZE_INFO);
    if (lpBuff->TaxLen.usTaxLen1 > A124_SIZE_INFO) {
        usTaxWdc                          = A124GETWDC(lpFile->auchTaxTbl[A124_OFS_WDC]);
        TaxRate[A124_OFS_TAX1].usWdc      = A124GETWDC(lpFile->auchTaxTbl[A124_OFS_WDC]);
        TaxRate[A124_OFS_TAX1].usQuoti    = A124GETQUOTI(lpFile->auchTaxTbl[A124_OFS_QUOTI]);
        TaxRate[A124_OFS_TAX1].usDenom    = A124GETDENOM(lpFile->auchTaxTbl[A124_OFS_DENOM]);
        TaxRate[A124_OFS_TAX1].usNumer    = A124GETNUMER(lpFile->auchTaxTbl[A124_OFS_NUMER]);
        TaxRate[A124_OFS_TAX1].usWdcCount = A124GETCOUNT(usTaxWdc, TaxRate[A124_OFS_TAX1].usDenom);

        for (usOfs = A124_OFS_SAMT,
             ulPrev = 0L, usI = usSetCo = 0, fFirst = FALSE;
             usOfs < MAX_TTBLDATA_SIZE;
             usOfs++, usI++) {
            ulTax = (ULONG)usI;
            ulOver = (ULONG)A124GETTAMT(lpFile->auchTaxTbl[usOfs]);
            if (ulOver == 0) {
                continue;
            }
            ulAmt = ulOver;
            for (;;) {
                if (ulOver != A124_MAX_SAMT) {
                    break;
                } else {
                    ulOver = A124GETTAMT(lpFile->auchTaxTbl[++ usOfs]);
                    ulAmt += ulOver;
                    usLen--;
                }
            }
            if (fFirst == FALSE) {
                ulAmt--;
                fFirst = TRUE;
            } else {
                ulAmt += ulPrev;
            }
            ulPrev = ulAmt;
            lpTemp->ulSlsAmt = ulAmt;
            lpTemp->ulTaxAmt = ulTax;
            lpTemp++;
            usSetCo++;
            if (usI >= usLen) {
                break;
            }
        }
        TaxRate[A124_OFS_TAX1].fStatus |= (A124_CHK_TAX1 | A124_CHK_TEST | A124_CHK_RATE);
        TaxLen.usTaxLen1 = usSetCo;
    } else {
        TaxRate[A124_OFS_TAX1].fStatus |= A124_CHK_TAX1;
        TaxLen.usTaxLen1 = 0;
    }

    /* ----- make tax2 data ----- */

    ulOver = 0L;
    lpTemp  = lpBuff->alpTaxData[A124_OFS_TAX2];
    usOfs  = lpBuff->TaxLen.usTaxLen1;
    usLen  = (USHORT)(lpBuff->TaxLen.usTaxLen2 - A124_SIZE_INFO);
    if (lpBuff->TaxLen.usTaxLen2 > A124_SIZE_INFO) {
        TaxRate[A124_OFS_TAX2].usWdc      = A124GETWDC(lpFile->auchTaxTbl[A124_OFS_WDC + usOfs]);
        TaxRate[A124_OFS_TAX2].usQuoti    = A124GETQUOTI(lpFile->auchTaxTbl[A124_OFS_QUOTI + usOfs]);
        TaxRate[A124_OFS_TAX2].usDenom    = A124GETDENOM(lpFile->auchTaxTbl[A124_OFS_DENOM + usOfs]);
        TaxRate[A124_OFS_TAX2].usNumer    = A124GETNUMER(lpFile->auchTaxTbl[A124_OFS_NUMER + usOfs]);
        TaxRate[A124_OFS_TAX2].usWdcCount = A124GETCOUNT(TaxRate[A124_OFS_TAX2].usWdc, TaxRate[A124_OFS_TAX2].usDenom);

        for (usOfs += A124_OFS_SAMT,
             ulPrev = 0L, usI = usSetCo = 0, fFirst = FALSE;
             usOfs < MAX_TTBLDATA_SIZE;
             usOfs++, usI++) {
            ulTax  = (ULONG)usI;
            ulOver = (ULONG)A124GETTAMT(lpFile->auchTaxTbl[usOfs]);
            if (ulOver == 0) {
                continue;
            }
            ulAmt = ulOver;
            for (;;) {
                if (ulOver != A124_MAX_SAMT) {
                    break;
                } else {
                    ulOver = A124GETTAMT(lpFile->auchTaxTbl[++usOfs]);
                    ulAmt += ulOver;
                    usLen--;
                }
            }
            if (fFirst == FALSE) {
                ulAmt--;
                fFirst = TRUE;
            } else {
                ulAmt += ulPrev;
            }
            ulPrev = ulAmt;
            lpTemp->ulSlsAmt = ulAmt;
            lpTemp->ulTaxAmt = ulTax;
            lpTemp++;
            usSetCo++;
            if (usI >= usLen) {
                break;
            }
        }
        TaxRate[A124_OFS_TAX2].fStatus |= (A124_CHK_TAX2 | A124_CHK_TEST | A124_CHK_RATE);
        TaxLen.usTaxLen2 = usSetCo;
    } else {
        TaxRate[A124_OFS_TAX2].fStatus |= A124_CHK_TAX2;
        TaxLen.usTaxLen2 = 0;
    }

    /* ----- make tax3 data ----- */

    ulOver = 0L;
    lpTemp  = lpBuff->alpTaxData[A124_OFS_TAX3];
    usOfs  = (USHORT)(lpBuff->TaxLen.usTaxLen1 + lpBuff->TaxLen.usTaxLen2);
    usLen  = (USHORT)(lpBuff->TaxLen.usTaxLen3 - A124_SIZE_INFO);
    if (lpBuff->TaxLen.usTaxLen3 > A124_SIZE_INFO) {
        TaxRate[A124_OFS_TAX3].usWdc      = A124GETWDC(lpFile->auchTaxTbl[A124_OFS_WDC + usOfs]);
        TaxRate[A124_OFS_TAX3].usQuoti    = A124GETQUOTI(lpFile->auchTaxTbl[A124_OFS_QUOTI + usOfs]);
        TaxRate[A124_OFS_TAX3].usDenom    = A124GETDENOM(lpFile->auchTaxTbl[A124_OFS_DENOM + usOfs]);
        TaxRate[A124_OFS_TAX3].usNumer    = A124GETNUMER(lpFile->auchTaxTbl[A124_OFS_NUMER + usOfs]);
        TaxRate[A124_OFS_TAX3].usWdcCount = A124GETCOUNT(TaxRate[A124_OFS_TAX3].usWdc, TaxRate[A124_OFS_TAX3].usDenom);

        for (usOfs += A124_OFS_SAMT,
             ulPrev = 0L, usI = usSetCo = 0, fFirst = FALSE;
             usOfs < MAX_TTBLDATA_SIZE;
             usOfs++, usI++) {
            ulTax = (ULONG)usI;
            ulOver = (ULONG)A124GETTAMT(lpFile->auchTaxTbl[usOfs]);
            if (ulOver == 0) {
                continue;
            }
            ulAmt = ulOver;
            for (;;) {
                if (ulOver != A124_MAX_SAMT) {
                    break;
                } else {
                    ulOver = A124GETTAMT(lpFile->auchTaxTbl[++usOfs]);
                    ulAmt += ulOver;
                    usLen--;
                }
            }
            if (fFirst == FALSE) {
                ulAmt--;
                fFirst = TRUE;
            } else {
                ulAmt += ulPrev;
            }
            ulPrev = ulAmt;
            lpTemp->ulSlsAmt = ulAmt;
            lpTemp->ulTaxAmt = ulTax;
            lpTemp++;
            usSetCo++;
            if (usI >= usLen) {
                break;
            }
        }
        TaxRate[A124_OFS_TAX3].fStatus |= (A124_CHK_TAX3 | A124_CHK_TEST | A124_CHK_RATE);
        TaxLen.usTaxLen3 = usSetCo;
    } else {
        TaxRate[A124_OFS_TAX3].fStatus |= A124_CHK_TAX3;
        TaxLen.usTaxLen3 = 0;
    }

    /* ----- make tax4 data ----- */

    ulOver = 0L;
    lpTemp  = lpBuff->alpTaxData[A124_OFS_TAX4];
    usOfs  = (USHORT)(lpBuff->TaxLen.usTaxLen1 + lpBuff->TaxLen.usTaxLen2 + lpBuff->TaxLen.usTaxLen3);
    usLen  = (USHORT)(lpBuff->TaxLen.usTaxLen4 - A124_SIZE_INFO);
    if (lpBuff->TaxLen.usTaxLen4 > A124_SIZE_INFO) {
        TaxRate[A124_OFS_TAX4].usWdc      = A124GETWDC(lpFile->auchTaxTbl[A124_OFS_WDC + usOfs]);
        TaxRate[A124_OFS_TAX4].usQuoti    = A124GETQUOTI(lpFile->auchTaxTbl[A124_OFS_QUOTI + usOfs]);
        TaxRate[A124_OFS_TAX4].usDenom    = A124GETDENOM(lpFile->auchTaxTbl[A124_OFS_DENOM + usOfs]);
        TaxRate[A124_OFS_TAX4].usNumer    = A124GETNUMER(lpFile->auchTaxTbl[A124_OFS_NUMER + usOfs]);
        TaxRate[A124_OFS_TAX4].usWdcCount = A124GETCOUNT(TaxRate[A124_OFS_TAX4].usWdc, TaxRate[A124_OFS_TAX4].usDenom);

        for (usOfs += A124_OFS_SAMT,
             ulPrev = 0L, usI = usSetCo = 0, fFirst = FALSE;
             usOfs < MAX_TTBLDATA_SIZE;
             usOfs++, usI++) {
            ulTax = (ULONG)usI;
            ulOver = (ULONG)A124GETTAMT(lpFile->auchTaxTbl[usOfs]);
            if ((ulOver == 0) && ((ulTax == 0) || (ulTax != 0))) {
                continue;
            }
            ulAmt = ulOver;
            for (;;) {
                if (ulOver != A124_MAX_SAMT) {
                    break;
                } else {
                    ulOver = A124GETTAMT(lpFile->auchTaxTbl[++usOfs]);
                    ulAmt += ulOver;
                    usLen--;
                }
            }
            if (fFirst == FALSE) {
                ulAmt--;
                fFirst = TRUE;
            } else {
                ulAmt += ulPrev;
            }
            ulPrev = ulAmt;
            lpTemp->ulSlsAmt = ulAmt;
            lpTemp->ulTaxAmt = ulTax;
            lpTemp++;
            usSetCo++;
            if (usI >= usLen) {
                break;
            }
        }
        TaxRate[A124_OFS_TAX4].fStatus |= (A124_CHK_TAX4 | A124_CHK_TEST | A124_CHK_RATE);
        TaxLen.usTaxLen4 = usSetCo;
    } else {
        TaxRate[A124_OFS_TAX4].fStatus |= A124_CHK_TAX4;
        TaxLen.usTaxLen4 = 0;
    }

    /* ----- store data size to buffer ----- */

    lpBuff->TaxLen.usTaxLen1 = TaxLen.usTaxLen1;
    lpBuff->TaxLen.usTaxLen2 = TaxLen.usTaxLen2;
    lpBuff->TaxLen.usTaxLen3 = TaxLen.usTaxLen3;
    lpBuff->TaxLen.usTaxLen4 = TaxLen.usTaxLen4;

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ConvWriteData()
*
*   Input   :   LPA124FILEDATA  lpFile - address of file data
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   None
*
**  Description:
*       This procedure exchanges writing data.
* ===========================================================================
*/
VOID    A124ConvWriteData(LPA124FILEDATA lpFile, LPA124BUFFDATA lpBuff)
{
    BOOL            fFirst;
    USHORT          usLen;
    WORD            wOfs, wCarry, wChk = TRUE;
    ULONG           ulAmt, ulTax, ulPrev, ulOver;
    LPA124TAXDATA   lpTemp;
    UCHAR           *puchPtr;

    /* ----- clear data area ----- */

    memset(lpFile, 0, sizeof(A124FILEDATA));
    puchPtr = lpFile->auchTaxTbl;

    /* ----- make tax1 data ----- */

    usLen = lpBuff->TaxLen.usTaxLen1;
    if (usLen && (TaxRate[A124_OFS_TAX1].fStatus & A124_CHK_TEST)) {
        lpTemp = lpBuff->alpTaxData[A124_OFS_TAX1];
        usTaxWdc = (USHORT)(TaxRate[A124_OFS_TAX1].usWdc / A124_WDC_PERCENT);
        *(puchPtr++) = (UCHAR)usTaxWdc;
        *(puchPtr++) = A124QueryTaxWdc(usTaxWdc, lpTemp, usLen);
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX1].usQuoti;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX1].usDenom;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX1].usNumer;
        wChk = (WORD)(lpTemp + (usLen - 1))->ulTaxAmt;
        wCarry = 0;
        for (wOfs = 0, ulPrev = 0L, fFirst = FALSE;
                                wOfs < MAX_TTBLDATA_SIZE; wOfs++) {
            ulAmt = lpTemp->ulSlsAmt;
            ulTax = lpTemp->ulTaxAmt + wCarry;
            if (ulTax != wOfs) {
                puchPtr += (ulTax - wOfs);
                wOfs = (WORD)ulTax;
                if (fFirst == FALSE) {
                    ulAmt++;
                }
            } else {
                if (! wOfs) {
                    ulAmt++;
                }
            }
            ulOver = ulAmt - ulPrev;
            for (;;) {
                if (ulOver >= A124_MAX_SAMT) {
                    *(puchPtr++) = A124_MAX_SAMT;
                    wOfs++;
                    wCarry++;
                    ulOver -= A124_MAX_SAMT;
                } else {
                    break;
                }
            }
            *(puchPtr++) = (UCHAR)ulOver;
            ulPrev = ulAmt;
            if (fFirst == FALSE) {
                ulPrev--;
                fFirst = TRUE;
            }
            lpTemp++;
            if (wOfs >= (wChk + wCarry)) {
                break;
            }
        }

        /* ----- set tax table data length ----- */

        lpFile->TaxLen.usTaxLen1 = (USHORT)(wOfs + A124_SIZE_INFO);

    }

    /* ----- make tax2 data ----- */

    usLen = lpBuff->TaxLen.usTaxLen2;
    if (usLen && (TaxRate[A124_OFS_TAX2].fStatus & A124_CHK_TEST)) {
        lpTemp = lpBuff->alpTaxData[A124_OFS_TAX2];
        usTaxWdc = (USHORT)(TaxRate[A124_OFS_TAX2].usWdc / A124_WDC_PERCENT);
        *(puchPtr++) = (UCHAR)usTaxWdc;
        *(puchPtr++) = A124QueryTaxWdc(usTaxWdc, lpTemp, usLen);
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX2].usQuoti;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX2].usDenom;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX2].usNumer;
        wChk = (WORD)((lpTemp + (usLen - 1))->ulTaxAmt);
        wCarry = 0;
        for (wOfs = 0, ulPrev = 0L, fFirst = FALSE;
                                wOfs < MAX_TTBLDATA_SIZE; wOfs++) {
            ulAmt = lpTemp->ulSlsAmt;
            ulTax = lpTemp->ulTaxAmt + wCarry;
            if (ulTax != wOfs) {
                puchPtr += (ulTax - wOfs);
                wOfs = (WORD)ulTax;
                if (fFirst == FALSE) {
                    ulAmt++;
                }
            } else {
                if (! wOfs) {
                    ulAmt++;
                }
            }
            ulOver = ulAmt - ulPrev;
            for (;;) {
                if (ulOver >= A124_MAX_SAMT) {
                    *(puchPtr++) = A124_MAX_SAMT;
                    wOfs++;
                    wCarry++;
                    ulOver -= A124_MAX_SAMT;
                } else {
                    break;
                }
            }
            *(puchPtr++) = (UCHAR)ulOver;
            ulPrev = ulAmt;
            if (fFirst == FALSE) {
                ulPrev--;
                fFirst = TRUE;
            }
            lpTemp++;
            if (wOfs >= (wChk + wCarry)) {
                break;
            }
        }

        /* ----- set tax table data length ----- */

        lpFile->TaxLen.usTaxLen2 = (USHORT)(wOfs + A124_SIZE_INFO);

    }

    /* ----- make tax3 data ----- */

    usLen = lpBuff->TaxLen.usTaxLen3;
    if (usLen && (TaxRate[A124_OFS_TAX3].fStatus & A124_CHK_TEST)) {
        lpTemp = lpBuff->alpTaxData[A124_OFS_TAX3];
        usTaxWdc = (USHORT)(TaxRate[A124_OFS_TAX3].usWdc / A124_WDC_PERCENT);
        *(puchPtr++) = (UCHAR)usTaxWdc;
        *(puchPtr++) = A124QueryTaxWdc(usTaxWdc, lpTemp, usLen);
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX3].usQuoti;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX3].usDenom;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX3].usNumer;
        wChk = (WORD)((lpTemp + (usLen - 1))->ulTaxAmt);
        wCarry = 0;
        for (wOfs = 0, ulPrev = 0L, fFirst = FALSE;
                                wOfs < MAX_TTBLDATA_SIZE; wOfs++) {
            ulAmt = lpTemp->ulSlsAmt;
            ulTax = lpTemp->ulTaxAmt + wCarry;
            if (ulTax != (ULONG)wOfs) {
                puchPtr += (ulTax - wOfs);
                wOfs = (WORD)ulTax;
                if (fFirst == FALSE) {
                    ulAmt++;
                }
            } else {
                if (! wOfs) {
                    ulAmt++;
                }
            }
            ulOver = ulAmt - ulPrev;
            for (;;) {
                if (ulOver >= A124_MAX_SAMT) {
                    *(puchPtr++) = A124_MAX_SAMT;
                    wOfs++;
                    wCarry++;
                    ulOver -= A124_MAX_SAMT;
                } else {
                    break;
                }
            }
            *(puchPtr++) = (UCHAR)ulOver;
            ulPrev = ulAmt;
            if (fFirst == FALSE) {
                ulPrev--;
                fFirst = TRUE;
            }
            lpTemp++;
            if (wOfs >= (wChk + wCarry)) {
                break;
            }
        }

        /* ----- set tax table data length ----- */

        lpFile->TaxLen.usTaxLen3 = (USHORT)(wOfs + A124_SIZE_INFO);

    }

    /* ----- make tax4 data ----- */

    usLen = lpBuff->TaxLen.usTaxLen4;
    if (usLen && (TaxRate[A124_OFS_TAX4].fStatus & A124_CHK_TEST)) {
        lpTemp = lpBuff->alpTaxData[A124_OFS_TAX4];
        usTaxWdc = (USHORT)(TaxRate[A124_OFS_TAX4].usWdc / A124_WDC_PERCENT);
        *(puchPtr++) = (UCHAR)usTaxWdc;
        *(puchPtr++) = A124QueryTaxWdc(usTaxWdc, lpTemp, usLen);
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX4].usQuoti;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX4].usDenom;
        *(puchPtr++) = (UCHAR)TaxRate[A124_OFS_TAX4].usNumer;
        wChk = (WORD)((lpTemp + (usLen - 1))->ulTaxAmt);
        wCarry = 0;
        for (wOfs = 0, ulPrev = 0L, fFirst = FALSE;
                                wOfs < MAX_TTBLDATA_SIZE; wOfs++) {
            ulAmt = lpTemp->ulSlsAmt;
            ulTax = lpTemp->ulTaxAmt + wCarry;
            if (ulTax != (ULONG)wOfs) {
                puchPtr += (ulTax - wOfs);
                wOfs = (WORD)ulTax;
                if (fFirst == FALSE) {
                    ulAmt++;
                }
            } else {
                if (! wOfs) {
                    ulAmt++;
                }
            }
            ulOver = ulAmt - ulPrev;
            for (;;) {
                if (ulOver >= A124_MAX_SAMT) {
                    *(puchPtr++) = A124_MAX_SAMT;
                    wOfs++;
                    wCarry++;
                    ulOver -= A124_MAX_SAMT;
                } else {
                    break;
                }
            }
            *(puchPtr++) = (UCHAR)ulOver;
            ulPrev = ulAmt;
            if (fFirst == FALSE) {
                ulPrev--;
                fFirst = TRUE;
            }
            lpTemp++;
            if (wOfs >= (wChk + wCarry)) {
                break;
            }
        }

        /* ----- set tax table data length ----- */

        lpFile->TaxLen.usTaxLen4 = (USHORT)(wOfs + A124_SIZE_INFO);

    }
}

/*
* ===========================================================================
**  Synopsis:   UCHAR   A124QueryTaxWdc()
*
*   Input   :   USHORT          usWdc   - WDC (Whole Dollar Constant)
*               LPA124TAXDATA   lpTax   - address of the Tax data buffer
*               USHORT          usLen   - number of input items
**
*   Return  :   UCHAR             - Tax on WDC
*
**  Description:
*       This procedure retrieves the Tax on WDC and returns it.
* ===========================================================================
*/
UCHAR   A124QueryTaxWdc(USHORT usWdc, LPA124TAXDATA lpTax, USHORT usLen)
{
    WORD            wI;
    ULONG           ulWdc, ulTaxWDC = 0;
    LPA124TAXDATA   lpTemp;

    /* ----- set WDC ----- */

    ulWdc = (usWdc * A124_WDC_PERCENT);

    /* ----- search tax on WDC ----- */

    for (wI = 0, lpTemp = lpTax; wI < usLen; wI++) {
        if (ulWdc <= lpTemp->ulSlsAmt) {
            ulTaxWDC = lpTemp->ulTaxAmt;
            break;
        } else {
            lpTemp++;
        }
    }
    return ((UCHAR)ulTaxWDC);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A124QueryMaxData()
*
*   Input   :   ULONG           ulSamt - sales amount
*               ULONG           ulTamt - tax amount
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   FALSE         - buffer is vacant
*               TRUE          - buffer is full
*
**  Description:
*       This procedure retrieves Max buffer size, so returns TRUE when the
*       data buffer is full. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A124QueryMaxData(ULONG ulSamt, ULONG ulTamt, LPA124BUFFDATA lpBuff)
{
    ULONG           ulAmt, ulTax, ulPrev, ulOver, ulMax, ulTmp;
    BOOL            fFirst = FALSE, fRet = FALSE;
    USHORT          usLen;
    WORD            wI, wCur, wOfs, wCarry, wChk = TRUE;
    LPA124TAXDATA   lpTemp;

    /* ----- store temporay buffer ----- */

    ulTmp = ulSamt;

    if (fTaxStat & A124_CHK_TAX1) {
        wCur = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wCur = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wCur = A124_OFS_TAX3;
    } else {
        wCur = A124_OFS_TAX4;
    }

    for (wI = 0, ulMax = 0L; wI < A124_NUM_TYPE; wI++) {
        lpTemp = lpBuff->alpTaxData[wI];
        if (wI == A124_OFS_TAX1) {
            usLen = lpBuff->TaxLen.usTaxLen1;
        } else if (wI == A124_OFS_TAX2) {
            usLen = lpBuff->TaxLen.usTaxLen2;
        } else if (wI == A124_OFS_TAX3) {
            usLen = lpBuff->TaxLen.usTaxLen3;
        } else {
            usLen = lpBuff->TaxLen.usTaxLen4;
        }
        wOfs = wCarry = 0;           /* init data counter        */
        ulPrev   = 0L;                   /* init previous data area  */
        if (usLen) {
            ulMax += A124_OFS_SAMT;        /* set table header size */
            wChk = (WORD)((lpTemp + (usLen - 1))->ulTaxAmt);
            for ( ; wOfs < MAX_TTBLDATA_SIZE; ) {
                ulAmt = lpTemp->ulSlsAmt;
                ulTax = lpTemp->ulTaxAmt + wCarry;
                if (ulTax != wOfs) {
                    ulMax += ulTax - wOfs;
                    wOfs = (WORD)ulTax;
                    if (fFirst == FALSE) {
                        ++ ulAmt;
                    }
                } else {
                    if (! wOfs) {
                        ++ ulAmt;
                    }
                }
                ulOver = ulAmt - ulPrev;
                for (;;) {
                    if (ulOver >= A124_MAX_SAMT) {
                        ++ ulMax;
                        ++ wOfs;
                        ++ wCarry;
                        ulOver -= A124_MAX_SAMT;
                    } else {
                        break;
                    }
                }
                ++ ulMax;
                ++ wOfs;
                ulPrev = ulAmt;
                if (fFirst == FALSE) {
                    -- ulPrev;
                    fFirst = TRUE;
                }
                lpTemp++;
                if (wOfs > (wChk + wCarry)) {
                    break;
                }
            }
        }
        if (wI == wCur) {
            if (! usLen) {                   /* first item entry */
                ulMax += A124_OFS_SAMT;  /* set table header size */
                ++ ulTmp;
            }
            if (ulTamt != (wOfs - wCarry)) {
                ulMax += (ulTamt - wOfs);
                if (usLen) {
                    ++ ulMax;
                }
            } else {
                ++ ulMax;
            }
            if ((ulTmp - ulPrev) >= A124_MAX_SAMT) {
                ulMax += (ulTmp / A124_MAX_SAMT);
            }
        }
    }

    if (ulMax > MAX_TTBLDATA_SIZE) {       /* buffer full */
        fRet = TRUE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A124MakeDblTbl()
*
*   Input   :   HWND    hDlg - handle of a dialog procedure
**
*   Return  :   None.
*
**  Description:
*       This procedure makes double the size of tax table.
* ===========================================================================
*/
VOID    A124MakeDblTbl(HWND hDlg)
{
    USHORT  usDenom;
    WORD    wI;

    /* ----- reset test mode status ----- */

    A124ResetTestStat(hDlg);

    /* ----- check status ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {
        wI = A124_OFS_TAX4;
    }

    /* ----- set denominator ----- */

    usDenom = TaxRate[wI].usDenom;

    /* ----- increment WDC counter ----- */

    TaxRate[wI].usWdcCount++;

    /* ----- double the size of tax table ----- */

    usTaxWdc  = (USHORT)((usDenom == 0)
                        ? A124_WDC_PERCENT : usDenom * A124_WDC_PERCENT);
    usTaxWdc *= TaxRate[wI].usWdcCount;

    /* ----- set WDC data into own buffer ----- */

    TaxRate[wI].usWdc = usTaxWdc;

    /* ----- set WDC information ----- */

    A124ShowWdc(hDlg);

}

/*
* ===========================================================================
**  Synopsis:   VOID    A124ShowWdc()
*
*   Input   :   HWND    hDlg - handle of a dialog procedure
**
*   Return  :   None.
*
**  Description:
*       This procedure shows WDC(Whole Dollar Constant) on this resource.
* ===========================================================================
*/
VOID    A124ShowWdc(HWND hDlg)
{
    WCHAR    szWork[A124_LEN_BUFF];

    wsPepf(szWork,
             WIDE("$ %u.%02u"),
             usTaxWdc / A124_WDC_PERCENT,
             usTaxWdc % A124_WDC_PERCENT);

    DlgItemRedrawText(hDlg, IDD_A124_WDC, szWork);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A124ChkEntData()
*
*   Input   :   ULONG           ulAmt - sales amount
*               ULONG           ulTax - tax amount
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   FALSE        - entry data is valid
*               TRUE         - entry data is invalid
*
**  Description:
*       This procedure checkes the entered data, whether it is valid or not.
*       So it is valid, this function returns FALSE. It's invalid, this
*       function returns TRUE.
* ===========================================================================
*/
BOOL    A124ChkEntData(ULONG ulAmt, ULONG ulTax, LPA124BUFFDATA lpBuff)
{
    WORD            wI;
    BOOL            fRet;
    USHORT          usLen;
    LPA124TAXDATA   lpTemp;

    /* ----- compute sort of tax AC ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {
        wI = A124_OFS_TAX4;
    }

    /* ----- set address of the retrieving data ----- */

    lpTemp = lpBuff->alpTaxData[wI];

    /* ----- set data buffer length ----- */

    if (wI == A124_OFS_TAX1) {
        usLen = lpBuff->TaxLen.usTaxLen1;
    } else if (wI == A124_OFS_TAX2) {
        usLen = lpBuff->TaxLen.usTaxLen2;
    } else if (wI == A124_OFS_TAX3) {
        usLen = lpBuff->TaxLen.usTaxLen3;
    } else {    /* (wI == A124_OFS_TAX4) */
        usLen = lpBuff->TaxLen.usTaxLen4;
    }

    if (usLen != 0) {     /* exist a previous data */
        if (((lpTemp + (usLen - 1))->ulSlsAmt >= ulAmt)
         || ((lpTemp + (usLen - 1))->ulTaxAmt >= ulTax)) {
                                            /* small data entry */
            fRet = TRUE;
        } else {
            fRet = FALSE;
        }
    } else {
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A124ChkChgData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               ULONG           ulAmt  - sales amount
*               ULONG           ulTax  - tax amount
*               LPA124BUFFDATA  lpBuff - address of buffer data
**
*   Return  :   TRUE               - change data is valid
*               A124_ERR_TAMT_UND  - change tax amount is under last data 
*               A124_ERR_SAMT_UND  - change sales amount is under last data 
*               A124_ERR_TAMT_OVR  - change tax amount is over next data 
*               A124_ERR_SAMT_OVR  - change sales amount is over next data 
*
**  Description:
*       This procedure checkes the entered data, whether it is valid or not.
*       If it is valid, this function returns TRUE. or it is invalid, this
*       function returns error code.
* ===========================================================================
*/
BOOL    A124ChkChgData(HWND hDlg, ULONG ulAmt,
                           ULONG ulTax, LPA124BUFFDATA lpBuff)
{
    BOOL            fRet = TRUE;                        /* return value */
    WORD            wI;
    WORD            wIdx;
    WORD            wItem;
    LPA124TAXDATA   lpTemp;

    /* ----- get selected item offset ----- */

    wIdx = (WORD)SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCURSEL, 0, 0L);

    /* ----- get number of items in the listbox ----- */

    wItem = (WORD)SendDlgItemMessage(hDlg, IDD_A124_TBLLIST, LB_GETCOUNT, 0, 0L);
        
    /* ----- compute sort of tax type ----- */

    if (fTaxStat & A124_CHK_TAX1) {
        wI = A124_OFS_TAX1;
    } else if (fTaxStat & A124_CHK_TAX2) {
        wI = A124_OFS_TAX2;
    } else if (fTaxStat & A124_CHK_TAX3) {
        wI = A124_OFS_TAX3;
    } else {    /* (fTaxStat & A124_CHK_TAX4) */
        wI = A124_OFS_TAX4;
    }

    /* ----- set address of the retrieving data ----- */

    lpTemp = lpBuff->alpTaxData[wI];

    if (wIdx != 0) {
        if ((lpTemp + (wIdx - 1))->ulTaxAmt >= ulTax) {
            fRet = A124_ERR_TAMT_UND;
        } else if ((lpTemp + (wIdx - 1))->ulSlsAmt >= ulAmt) {
            fRet = A124_ERR_SAMT_UND;
        }
    }

    if (wItem != wIdx + 1) {
        if ((lpTemp + (wIdx + 1))->ulTaxAmt <= ulTax) {
            fRet = A124_ERR_TAMT_OVR;
        } else if ((lpTemp + (wIdx + 1))->ulSlsAmt <= ulAmt) {
            fRet = A124_ERR_SAMT_OVR;
        }
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A124QuerySaveData()
*
*   Input   :   HWND            hDlg   - handle of a dialogbox
*               LPA124TAXRATE   lpRate - address of data buffer
**
*   Return  :   FALSE - all tax table is input
*               TRUE  - exist unsaving tax table
*
**  Description:
*       This procedure checkes whether Unsaving Tax Table exist nor not.
* ===========================================================================
*/
BOOL    A124QuerySaveData(HWND hDlg, LPA124TAXRATE lpRate)
{
    BOOL    fRet;
    WCHAR    szWork[A124_LEN_BUFF * 2];
    WCHAR    szBuff[A124_LEN_BUFF * 2];
    WCHAR    szSepa[A124_LEN_BUFF];
    WCHAR    szCap[PEP_CAPTION_LEN];
    BOOL    fChk = FALSE;

    /* ----- edit warning message No.1 ----- */

    LoadString(hResourceDll, IDS_PEP_CAPTION_A124, szCap, PEP_STRING_LEN_MAC(szCap));
    LoadString(hResourceDll, IDS_A124_WARNMSG1, szWork, PEP_STRING_LEN_MAC(szWork));
    LoadString(hResourceDll, IDS_A124_SEPARAT,  szSepa, PEP_STRING_LEN_MAC(szSepa));

    if (! (lpRate->fStatus & A124_CHK_TEST)) {           /* check Tax Table #1  */
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            LoadString(hResourceDll, IDS_A124_TYPE_CN1, szBuff, PEP_STRING_LEN_MAC(szBuff));
        } else {
            LoadString(hResourceDll, IDS_A124_TYPE_US1, szBuff, PEP_STRING_LEN_MAC(szBuff));
        }
        lstrcatW(szWork, szBuff);     /* append table number */
        fChk = TRUE;                                /* set check status    */
    }

    if (! ((lpRate + A124_OFS_TAX2)->fStatus & A124_CHK_TEST)) {  /* check #2 */
        if (fChk == TRUE) {
            lstrcatW(szWork, szSepa);  /* set No. separator   */
        } else {
            fChk = TRUE;                            /* set check status    */
        }
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            LoadString(hResourceDll, IDS_A124_TYPE_CN2, szBuff, PEP_STRING_LEN_MAC(szBuff));
        } else {
            LoadString(hResourceDll, IDS_A124_TYPE_US2, szBuff, PEP_STRING_LEN_MAC(szBuff));
        }
        lstrcatW(szWork, szBuff);     /* append table number */
    }

    if (! ((lpRate + A124_OFS_TAX3)->fStatus & A124_CHK_TEST)) {  /*  check #3 */
        if (fChk == TRUE) {
            lstrcatW(szWork, szSepa);  /* set No. separator   */
        } else {
            fChk = TRUE;                            /* set check status    */
        }
        if (MdcData.uchMDCData & A124_TAX_CHK) {
            LoadString(hResourceDll, IDS_A124_TYPE_CN3, szBuff, PEP_STRING_LEN_MAC(szBuff));
        } else {
            LoadString(hResourceDll, IDS_A124_TYPE_US3, szBuff, PEP_STRING_LEN_MAC(szBuff));
        }
        lstrcatW(szWork, szBuff);     /* append table number */
    }

    if (MdcData.uchMDCData & A124_TAX_CHK) {
        if (! ((lpRate + A124_OFS_TAX4)->fStatus & A124_CHK_TEST)) {  /*  check #4 */
            if (fChk == TRUE) {
                lstrcatW(szWork, szSepa); /* set No. separator   */
            } else {
                fChk = TRUE;                            /* set check status    */
            }
            LoadString(hResourceDll, IDS_A124_TYPE_CN4, szBuff, PEP_STRING_LEN_MAC(szBuff));
            lstrcatW(szWork, szBuff);     /* append table number */
        }
    }

    if (fChk == TRUE) {                             /* exist au unsaving data */

        /* ----- append warning message No.2 ----- */

        LoadString(hResourceDll, IDS_A124_WARNMSG2, szBuff, PEP_STRING_LEN_MAC(szBuff));
        lstrcatW(szWork, szBuff);       /* complete a message */

        /* ----- show an unsaving message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBoxPopUp(hDlg,
                       szWork,
                       szCap,
                       MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK) {
            fRet = FALSE;                       /* user select a OK button */
        } else {
            fRet = TRUE;                        /* user select a cancel */
        }
    } else {                                    /* All Tax Table is input */
        fRet = FALSE;
    }

    return (fRet);                          /* return result */
}

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI A124TestDlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg    - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialogbox procedure to maintenance the TAX Table.
* ===========================================================================
*/
BOOL    WINAPI  A124TestDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WCHAR    szWork[A124_LEN_BUFF];
    ULONG   ulTax, ulAmt;

    switch (wMsg) {
    case    WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- set entry digits (5 digits) ----- */

        SendDlgItemMessage(hDlg,
                           IDD_A124_TEST_SAMT,
                           EM_LIMITTEXT,
                           (WPARAM)A124_LEN_TEST,
                           0L);

        /* ----- set tax type ----- */

        DlgItemRedrawText(hDlg, IDD_A124_TEST_TYPE, szTaxType);

        /* ----- set Tax Rate ----- */

        DlgItemRedrawText(hDlg, IDD_A124_TEST_RATE, szTaxRate);

        lParam = lParam;
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A124_TEST_TYPE; j<=IDD_A124_TEST_TAX; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_ENTER, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
        case    IDD_ENTER:

            /* ----- get test amount ----- */

            memset(szWork, 0, sizeof(szWork));

            DlgItemGetText(hDlg,
                           IDD_A124_TEST_SAMT,
                           szWork,
                           A124_LEN_TEST + 1);

            /* ----- convert input data to long value ----- */
            
            ulAmt = _wtol(szWork);

            /* ----- calculate tax rate ----- */

            ulTax = A124TestCalcTaxRate(ulAmt);

            /* ----- set Tax data of test value ----- */

            wsPepf(szWork,
                     WIDE("%3lu.%02lu"),
                     ulTax / A124_WDC_PERCENT,
                     ulTax % A124_WDC_PERCENT);
            DlgItemRedrawText(hDlg, IDD_A124_TEST_TAMT, szWork);

            /* ----- select range of characters in an edit control ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A124_TEST_SAMT,
                               EM_SETSEL,
                               0,
                               MAKELONG(0, -1));

            /* ----- set focus to amount edit control ----- */

            SetFocus(GetDlgItem(hDlg, IDD_A124_TEST_SAMT));

            return TRUE;

        case    IDOK:
        case    IDCANCEL:
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
**  Synopsis:   ULONG   A124TestCalcTaxRate()
*
*   Input   :   ULONG   ulAmount - amount
**
*   Return  :   ULONG   ulTax    - tax
*
**  Description:
*       This procedure calculates tax amount.
*
*       Calculate the Tax amount as follows:
*
*                   tax = (TAXWDC * ulB) + (WDC + ulC)
*                           ulB : ulA / WDC
*                           ulC : ulA % WDC
*                           ulA : ulAmount - WDC
*
* ===========================================================================
*/
ULONG   A124TestCalcTaxRate(ULONG ulAmount)
{
    WORD            wI;
    LPA124TAXDATA   lpTemp;
    ULONG           ulA, ulB = 1, ulC;
    ULONG           ulTax = 0, ulAmt, ulTaxWDC = 0;
    BOOL            fCheck;

    /* ----- set WDC ----- */

    ulAmt = (ULONG)usTaxWdc;

    /* ----- search tax on WDC ----- */

    for (wI = 0, lpTemp = lpTaxData; wI < wTaxItem; wI++) {
        if (ulAmt <= lpTemp->ulSlsAmt) {
            ulTaxWDC = lpTemp->ulTaxAmt;
            break;
        } else {
            lpTemp++;
        }
    }

    /* ----- Is calculation data within a tax table? ----- */

    fCheck  = (BOOL)((ulAmount > usTaxWdc) ? TRUE : FALSE);

    /* ----- calculate the tax amount ----- */

    if (fCheck == FALSE) {
        ulAmt = ulAmount;
    } else {
        ulA = (ULONG)(ulAmount - usTaxWdc);
        ulB = (ULONG)(ulA / usTaxWdc);
        ulC = (ULONG)(ulA % usTaxWdc);
        ulAmt = (ULONG)(usTaxWdc + ulC);
    }
    for (wI = 0, lpTemp = lpTaxData; wI <= wTaxItem; wI++) {
        if (ulAmt <= lpTemp->ulSlsAmt) {
            ulTax = lpTemp->ulTaxAmt;
            break;
        } else {
            lpTemp++;
        }
    }
    if (fCheck == TRUE) {
        ulAmt  = (ULONG)(ulTaxWDC * ulB);
        ulTax += ulAmt;
    }
    return (ulTax);
}

/*
* ===========================================================================
**  Synopsis:   int     A124DspError()
*
*   Input   :   HWND    hDlg       - handle of a dialogbox procedure
*               WORD    wID        - Strings ID
*               UINT    fuStyle    - MessageBox Type
**
*   Return  :   This Value is a status from MessageBox function.
*
**  Description:
*       This procedure shows error message and returns its status.
* ===========================================================================
*/
int     A124DspError(HWND hDlg, WPARAM wID, UINT fuStyle)
{
    WCHAR szCaption[PEP_CAPTION_LEN];
    WCHAR szErrMsg[A124_LEN_BUFF * 2];

    LoadString(hResourceDll, IDS_PEP_CAPTION_A124, szCaption, PEP_CAPTION_LEN);
    LoadString(hResourceDll, wID, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

    if (fuStyle & MB_ICONEXCLAMATION) {
        MessageBeep(MB_ICONEXCLAMATION);
    } else if (fuStyle & MB_ICONQUESTION) {
        MessageBeep(MB_ICONQUESTION);
    }
    return (MessageBoxPopUp(hDlg, szErrMsg, szCaption, fuStyle));
}

/* ===== End of A124.C ===== */
