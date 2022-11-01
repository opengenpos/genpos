/*
* ---------------------------------------------------------------------------
* Title         :   Print for PLU by w/ description form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068D.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnA68DescDraw():      draw record
*               PrnA68DescDrawRec():   draw each record
*               PrnA68DescDrawDsc():   draw description
*               PrnA68DescDrawDat():   draw data
*               PrnA68DescDrawConDsc():draw control code description
*               PrnA68DescDrawConDat():draw control code data
*               PrnA68DescDrawItem():  draw each item
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-03-95 : 03.00.00 : H.Ishida   : Add Table number, Group Number and Print
*                                     Priority by PLU
* Jan-29-00 : 01.00.00 : hrkato     : Saratoga
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
#include    <windows.h>
#include    <string.h>

#include    <ecr.h>
#include    <plu.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "prog.h"
#include    "pepprn.h"
#include    "pepprnl.h"
#include    "prna068.h"
#include    "prna068d.h"

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
**  Synopsis    :   VOID    PrnA68DescDraw()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   WORD    wNum    - number of PLU record
*
**  Return      :   None
*
**  Description :
*       This procedure draw PLU record by w/ description form.
* ===========================================================================
*/
VOID    PrnA68DescDraw(HDC hDC, WORD wNum)
{
    WORD        wCoPage;
    ECMRANGE    EcmRange;
    USHORT      usStat;
    USHORT      usHandle;
	int i;

    /* ----- initialize item No. structure buffer,  Saratoga ----- */
	memset(EcmRange.itemFrom.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
    memset(EcmRange.itemTo.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
	for(i=0; i < PRN_PLUNO_LEN; i++){
		EcmRange.itemFrom.aucPluNumber[i] = PrnSet.szStartA68[i];
		EcmRange.itemTo.aucPluNumber[i] = PrnSet.szEndA68[i];
	}
   // memcpy((LPSTR)EcmRange.itemFrom.aucPluNumber, (LPCSTR)PrnSet.szStartA68, PRN_PLUNO_LEN);
   // memcpy((LPSTR)EcmRange.itemTo.aucPluNumber, (LPCSTR)PrnSet.szEndA68, PRN_PLUNO_LEN);
    EcmRange.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);
    EcmRange.usReserve = 0;

    /* ----- enter critical mode ----- */
    usStat = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &usHandle, &EcmRange);

    for (wCoPage = 0; wCoPage < wNum; wCoPage++) {

        /* ----- draw each record ----- */
        if (PrnA68DescDrawRec(hDC, usHandle, wCoPage, wNum) == FALSE) {
            break;
        }

        /* ----- end page ----- */
        EndPage(hDC);
		//Escape(hDC, NEWFRAME, NULL, NULL, NULL);
    }
    /* ----- exit critical mode ----- */
    usStat = PluExitCritMode(usHandle, 0);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    PrnA68DescDrawRec()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   LPWORD  lpwNo   - address of PLU No. counter
*                   WORD    wCount  - page counter
*                   WORD    wTotal  - total page
*
**  Return      :   TRUE    -   user process is executed
*                   FALSE   -   error occurred
*
**  Description :
*       This procedure draw each record by w/ description form.     Saratoga
* ===========================================================================
*/
BOOL    PrnA68DescDrawRec(HDC hDC, USHORT usHandle, WORD wCount, WORD wTotal)
{
    RECPLU  PluRec;
    USHORT  usRet;
    USHORT  usStat;

    /* ----- read PLU record ----- */
    do {
        usStat = PluReportRecord(usHandle, &PluRec, &usRet);

    } while (usStat == SPLU_FUNC_CONTINUED); 
    if (usStat != SPLU_COMPLETED) {
        
        return FALSE;
    }

    /* ----- display sending message ----- */

    SendMessage(hdlgPrnAbort,
                PM_PRN_DSPMSG,
                IDS_PRN_DSP_A68_DESC,
                MAKELPARAM(wTotal, (WORD)(wCount + 1)));

    /* ----- draw data ----- */

    PrnA68DescDrawDat(hDC, &PluRec);

    /* ----- draw control code data ----- */

    PrnA68DescDrawConDat(hDC, &PluRec);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68DescDrawDat()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   LPPLUIF lpPlu   - address of PLU record buffer
*
**  Return      :   None
*
**  Description :
*       This procedure draw data by w/ description form.
* ===========================================================================
*/
VOID    PrnA68DescDrawDat(HDC hDC, RECPLU *lpPlu)
{
    WCHAR        szWork[PRN_A68_BUFF_LEN];
    WCHAR        szWorkAll[PRN_A68_BUFF_LEN*2];
    WORD        wI;
    DWORD       dwInt;
    WORD        wDec;
	RECT myRect;
	TEXTMETRIC  tm;

	GetTextMetrics (hDC, &tm);
	myRect.left = 0;
	myRect.top = 0;
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
	myRect.right = GetDeviceCaps(hDC, PHYSICALWIDTH) - (GetDeviceCaps(hDC, PHYSICALOFFSETX) + 5) * 2;

    for (wI = 0; wI < PRN_A68_DESC_DSC_NUM; wI++) {

        /* ----- make string ----- */

		memset (szWork, 0, sizeof(szWork));
		memset (szWorkAll, 0, sizeof(szWorkAll));

        LoadString(hResourceDll/*hPepInst*/,
                   IDS_PRN_A68_PLUNO + wI,
                   szWorkAll,
                   PEP_STRING_LEN_MAC(szWork));

        switch (wI) {
        case PRN_A68_DESC_DSC_NO:
            // wsprintf((LPSTR)szWork, (LPSTR)"%s", lpPlu->aucPluNumber);
			wcsncpy (szWork, lpPlu->aucPluNumber, /*12*/PLU_MAX_DIGIT - 1);
            break;

        case PRN_A68_DESC_DSC_NAME:
            PrnA68MakeName(lpPlu->aucMnemonic, szWork);
            break;

        case PRN_A68_DESC_DSC_TYPE:
            LoadString(hResourceDll/*hPepInst*/,
                       IDS_PRN_A68_TYPE_1 - 1
                           + PRNA68GETTYPE(lpPlu->usDeptNumber),
                       szWork,
                       PEP_STRING_LEN_MAC(szWork));
            break;

        case PRN_A68_DESC_DSC_DEPT:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETDEPT(lpPlu->usDeptNumber));
            break;

        case PRN_A68_DESC_DSC_REPT:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETREPT(lpPlu->uchReportCode));
            break;

        case PRN_A68_DESC_DSC_TBLNO:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETTBLNO(lpPlu->uchTableNumber));
            break;

        case PRN_A68_DESC_DSC_GRPNO:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETGRPNO(lpPlu->uchGroupNumber));
            break;

        case PRN_A68_DESC_DSC_PRTPRY:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETPRTPRY(lpPlu->uchPrintPriority));
            break;

        case PRN_A68_DESC_DSC_PPI:
            wsPepf(szWork,
                     WIDE("%u"),
                     PRNA68GETPPI(lpPlu->uchLinkPPI));
            break;

        case PRN_A68_DESC_DSC_PRICE:
            if (PRNA68GETTYPE(lpPlu->usDeptNumber) !=
                PRN_A68_TYPE_OPEN) {
                PrnA68MakePrice((LPCSTR)lpPlu->ulUnitPrice, &dwInt, &wDec);
                wsPepf(szWork, WIDE("%5lu.%02u"), dwInt, wDec);
            }
        }

        /* ----- draw each item ----- */

		wcscat (szWorkAll, szWork);
		DrawTextPep(hDC, szWorkAll, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

        /* ----- set next string position ----- */
		myRect.top = myRect.bottom + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68DescDrawConDat()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   LPPLUIF lpPlu   - address of PLU record buffer
*
**  Return      :   None
*
**  Description :
*       This procedure draw control code data by w/ description form.
* ===========================================================================
*/
VOID    PrnA68DescDrawConDat(HDC hDC, RECPLU *lpPlu)
{
    WCHAR        szWork[PRN_A68_BUFF_LEN];
    WCHAR        szWorkAll[PRN_A68_BUFF_LEN*2];
    UINT        wI;
    UINT        wID;
	RECT myRect;
	TEXTMETRIC  tm;

	// we will beging 14 lines down from the top as we provide room for the 
	// description information at the top of the page containing PLU number, mnemonic, etc.
	GetTextMetrics (hDC, &tm);
	myRect.left = 0;
	myRect.top = 14*(tm.tmHeight + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72));
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
	myRect.right = GetDeviceCaps(hDC, PHYSICALWIDTH) - (GetDeviceCaps(hDC, PHYSICALOFFSETX) + 5) * 2;

	LoadString(hResourceDll/*hPepInst*/, IDS_PRN_A68_CONT, szWorkAll, PEP_STRING_LEN_MAC(szWork));
	DrawTextPep(hDC, szWorkAll, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

    /* ----- set next string position ----- */
	myRect.top = myRect.bottom + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;

    for (wI = 0; wI < (PRN_A68_DESC_CON_NUM - 1); wI++) {

        LoadString(hResourceDll/*hPepInst*/,
                   IDS_PRN_A68_CONT + wI + 1,
                   szWorkAll,
                   PEP_STRING_LEN_MAC(szWork));

        if (PrnA68DescChkCon(wI, lpPlu) == TRUE) {
            wID = (IDS_PRN_A68_1_SIGN + wI);
        } else {
            wID = (IDS_PRN_A68_0_SIGN + wI);
        }

        /* ----- make string ----- */

        LoadString(hResourceDll/*hPepInst*/, wID, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- draw each item ----- */
		wcscat (szWorkAll, szWork);
		DrawTextPep(hDC, szWorkAll, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

        /* ----- set next string position ----- */
		myRect.top = myRect.bottom + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;

    }

    /* ----- make string for bonus total index ----- */

    LoadString(hResourceDll/*hPepInst*/,
               IDS_PRN_A68_DSC_BONUS,
               szWorkAll,
               PEP_STRING_LEN_MAC(szWork));

	wsPepf(szWork,
         WIDE("%u"),
         PRNA68GETBONUS(lpPlu->aucStatus[PRN_A68_FIELD_9_10]));

    /* ----- draw each item ----- */

	wcscat(szWorkAll, szWork);
	DrawTextPep(hDC, szWorkAll, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

    /* ----- set next string position ----- */
	myRect.top = myRect.bottom + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;

    /* ----- make string for implied adjective key ----- */
	
    LoadString(hResourceDll/*hPepInst*/,
               IDS_PRN_A68_DSC_IMP,
               szWorkAll,
               PEP_STRING_LEN_MAC(szWork));

    wID = (IDS_PRN_A68_0_IMP + PRNA68GETADJ(lpPlu->aucStatus[PRN_A68_FIELD_11_12]));

    LoadString(hResourceDll/*hPepInst*/, wID, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- draw each item ----- */

	wcscat(szWorkAll, szWork);
	DrawTextPep(hDC, szWorkAll, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

    /* ----- set next string position ----- */
	myRect.top = myRect.bottom + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    PrnA68DescChkCon()
*
**  Input       :   UINT    wCount  - control code item counter
*                   LPPLUIF lpPlu   - address of PLU record buffer
*
**  Return      :   TRUE    - control code is option
*                   FALSE   - control code is standard
*
**  Description :
*       This procedure check control code data.
* ===========================================================================
*/
BOOL    PrnA68DescChkCon(UINT wCount, RECPLU  *pPlu)
{
    static  BYTE    bConTbl[PRN_A68_DESC_CON_NUM - 1][2] =
                        {{PRN_A68_FIELD_3_4, PRN_A68_MAP_SIGN},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_PRINT},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_SI},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_MODKEY},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_MODCON},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_VALID},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_OPTREC},
                         {PRN_A68_FIELD_3_4, PRN_A68_MAP_RECTP},
                         {PRN_A68_FIELD_5_6, PRN_A68_MAP_TAX1},
                         {PRN_A68_FIELD_5_6, PRN_A68_MAP_TAX2},
                         {PRN_A68_FIELD_5_6, PRN_A68_MAP_TAX3},
                         {PRN_A68_FIELD_5_6, PRN_A68_MAP_DISC1},
                         {PRN_A68_FIELD_5_6, PRN_A68_MAP_DISC2},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_SCALE},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_COND},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_CONT},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_RMT1},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_RMT2},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_RMT3},
                         {PRN_A68_FIELD_7_8, PRN_A68_MAP_RMT4},
                         {PRN_A68_FIELD_18,  PRN_A68_MAP_RMT5},
                         {PRN_A68_FIELD_18,  PRN_A68_MAP_RMT6},
                         {PRN_A68_FIELD_18,  PRN_A68_MAP_RMT7},
                         {PRN_A68_FIELD_18,  PRN_A68_MAP_RMT8},
                         {PRN_A68_FIELD_9_10, PRN_A68_MAP_ADJ1},
                         {PRN_A68_FIELD_9_10, PRN_A68_MAP_ADJ2},
                         {PRN_A68_FIELD_9_10, PRN_A68_MAP_ADJ3},
                         {PRN_A68_FIELD_9_10, PRN_A68_MAP_ADJ4},
                         {PRN_A68_FIELD_11_12, PRN_A68_MAP_VARI1},
                         {PRN_A68_FIELD_11_12, PRN_A68_MAP_VARI2},
                         {PRN_A68_FIELD_11_12, PRN_A68_MAP_VARI3},
                         {PRN_A68_FIELD_11_12, PRN_A68_MAP_VARI4},
                         {PRN_A68_FIELD_11_12, PRN_A68_MAP_VARI5}};

    if (pPlu->aucStatus[bConTbl[wCount][0]]
            & bConTbl[wCount][1]) {
        return TRUE;
    } else {
        return FALSE;
    }

}

/* ===== End of PRNA068D.C ===== */