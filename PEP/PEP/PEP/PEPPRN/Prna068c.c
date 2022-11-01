/*
* ---------------------------------------------------------------------------
* Title         :   Print for PLU by condensed form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068C.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnA68CondDraw():     draw record by condensed form
*               PrnA68CondDrawRec():  draw each record by condensed form
*               PrnA68CondDrawTitle():draw title by condensed form
*               PrnA68CondMakeStr():  make string for condensed form
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
#include    <rfl.h>
#include    <plu.h>
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
#include    "prna068c.h"
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
**  Synopsis    :   VOID    PrnA68CondDraw()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   WORD    wNum    - number of PLU record
*
**  Return      :   None
*
**  Description :
*       This procedure draw PLU record by condensed form.
* ===========================================================================
*/
VOID    PrnA68CondDraw(HDC hDC, WORD wNum)
{
    WORD    wCoRec;
    WORD    wCoPage;
    WORD    wMaxPage;
    ECMRANGE    EcmRange;
    USHORT      usStat;
    USHORT      usHandle;
	TEXTMETRIC  tm;
	int nLinesPerPage;
	int i;

    /* ----- calculate No. of page ----- */

	// Calculate number of lines per page at the current font size and the device page height.
	// we multiply by 2 as each of the condensed printout lines are two lines.
	GetTextMetrics (hDC, &tm);
	nLinesPerPage = (tm.tmHeight + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72)) * 2;
	nLinesPerPage = (GetDeviceCaps(hDC, PHYSICALHEIGHT) - (GetDeviceCaps(hDC, PHYSICALOFFSETY) + 5) * 2) / nLinesPerPage;

	nLinesPerPage -= 4;  // leave room for the header so fudge a few more lines

    wMaxPage = (WORD)(((wNum - 1) / nLinesPerPage) + 1);

    /* ----- initialize PLU No. Saratoga ----- */
		

	memset(EcmRange.itemFrom.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
    memset(EcmRange.itemTo.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
	for(i=0; i < PRN_PLUNO_LEN; i++){
		EcmRange.itemFrom.aucPluNumber[i] = PrnSet.szStartA68[i];
		EcmRange.itemTo.aucPluNumber[i] = PrnSet.szEndA68[i];
	}
    //memcpy((LPSTR)EcmRange.itemFrom.aucPluNumber, (LPCSTR)PrnSet.szStartA68, PRN_PLUNO_LEN);
    //memcpy((LPSTR)EcmRange.itemTo.aucPluNumber, (LPCSTR)PrnSet.szEndA68, PRN_PLUNO_LEN);
    EcmRange.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);
    EcmRange.usReserve = 0;

    /* ----- enter critical mode ----- */
    usStat = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &usHandle, &EcmRange);

    for (wCoPage = 0; wCoPage < wMaxPage; wCoPage++) {

        for (wCoRec = 0; wCoRec < nLinesPerPage /* PRN_A68_COND_Y_NUM */; wCoRec++) {

            /* ----- draw each record ----- */

            if (PrnA68CondDrawRec(hDC,
                                  usHandle,
                                  wCoRec,
                                  wCoPage,
                                  wMaxPage) == FALSE) {

                /* ----- end page ----- */

                EndPage(hDC);
				//Escape(hDC, NEWFRAME, NULL, NULL, NULL);

                return;
            }

            if (wCoPage * nLinesPerPage + wCoRec + 1 >= wNum) {
				wCoPage = wMaxPage;
                break;
            }

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
**  Synopsis    :   BOOL    PrnA68CondDrawRec()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   LPWORD  lpwNo   - address of PLU No. counter
*                   WORD    wRec    - record counter for 1 page
*                   WORD    wPage   - page counter
*                   WORD    wTotal  - total page
*
**  Return      :   TRUE    -   user process is executed
*                   FALSE   -   error occurred
*
**  Description :
*       This procedure draw each record by condensed form.
* ===========================================================================
*/
BOOL    PrnA68CondDrawRec(HDC hDC,
                          USHORT usHandle,
                          WORD wRec,
                          WORD wPage,
                          WORD wTotal)
{
	RECT myRect;
	TEXTMETRIC  tm;
    WCHAR    szWorkUp[PRN_A68_BUFF_LEN];
//    WCHAR    szWorkMd[PRN_A68_BUFF_LEN];
    WCHAR    szWorkDn[PRN_A68_BUFF_LEN];
    RECPLU  PluRec;
    USHORT  usRet;
    USHORT  usStat;

    /* ----- read PLU record,   Saratoga ----- */
    do {
        usStat = PluReportRecord(usHandle, &PluRec, &usRet);

    } while (usStat == SPLU_FUNC_CONTINUED); 
    if (usStat != SPLU_COMPLETED) {
        
        return FALSE;
    }

    /* ----- display sending message ----- */

    SendMessage(hdlgPrnAbort,
                PM_PRN_DSPMSG,
                IDS_PRN_DSP_A68_COND,
                MAKELPARAM(wTotal, (WORD)(wPage + 1)));

	GetTextMetrics (hDC, &tm);

	// We will indent the printed page by 8 characters to provide a gutter for
	// stapling, hole punches, etc.
	// In the condensed form, we print two lines for each PLU.  The second line is
	// indented from our left margin by 16 characters.
    if (wRec == 0) {

        /* ----- draw title ----- */

		LoadString(hPepInst, IDS_PRN_A68_TITLE_UP, szWorkUp, PRN_A68_BUFF_LEN);
		LoadString(hPepInst, IDS_PRN_A68_TITLE_DN, szWorkDn, PRN_A68_BUFF_LEN);

		myRect.left = tm.tmAveCharWidth * 8;
		myRect.top = 0;
		myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
		myRect.right = GetDeviceCaps(hDC, PHYSICALWIDTH) - (GetDeviceCaps(hDC, PHYSICALOFFSETX) + 5) * 2;
		DrawTextPep(hDC, szWorkUp, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

		myRect.top = myRect.bottom + tm.tmExternalLeading;
		myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
		myRect.left += 16 * tm.tmAveCharWidth;
		DrawTextPep(hDC, szWorkDn, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);
    }

    /* ----- make string to print ---- */

    PrnA68CondMakeStr(&PluRec, szWorkUp, szWorkDn);

    /* ----- set rectangle area ----- */

	myRect.left = tm.tmAveCharWidth * 8;
	myRect.top = (wRec + 1) * (tm.tmHeight + tm.tmExternalLeading + MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72)) * 2;
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
	myRect.right = GetDeviceCaps(hDC, PHYSICALWIDTH) - (GetDeviceCaps(hDC, PHYSICALOFFSETX) + 5) * 2;
    DrawTextPep(hDC, szWorkUp, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

	myRect.top = myRect.bottom + tm.tmExternalLeading;
	myRect.bottom = myRect.top + tm.tmHeight + tm.tmExternalLeading;
	myRect.left += 16 * tm.tmAveCharWidth;
    DrawTextPep(hDC, szWorkDn, -1, &myRect, DT_NOCLIP | DT_SINGLELINE);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68CondMakeStr()
*
**  Input       :   LPRECPLU    lpPlu   - address of PLU record buffer
*                   LPSTR       lpszUp  - address of string buffer up side
*                   LPSTR       lpszDn  - address of string buffer down side
*
**  Return      :   None
*
**  Description :
*       This procedure make string for condensed form.
* ===========================================================================
*/
VOID    PrnA68CondMakeStr(RECPLU* lpPlu,
                          LPWSTR lpszUp,
                          LPWSTR lpszDn)
{
    WCHAR    szName[PRN_A68_NAME_LEN + 1];
    WCHAR    szWork[PRN_A68_BUFF_LEN];
    WCHAR    szLink[PRN_A68_BUFF_LEN];
    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];
    WORD    wLink;
    BYTE    bBit;
    WORD    wI;
    DWORD       dwInt;
    WORD        wDec;

    /* ----- initialize string buffer ----- */
	
    memset(lpszUp, 0, PRN_A68_BUFF_LEN * sizeof(WCHAR));
    memset(lpszDn, 0, PRN_A68_BUFF_LEN * sizeof(WCHAR));

    /* ----- get PLU No. ----- */

    memset(szPluNo, 0, sizeof(szPluNo));
	wcsncpy (szPluNo, lpPlu->aucPluNumber, /*12*/PLU_MAX_DIGIT - 1);

    /* ----- get name ----- */
	memset(szName, 0, sizeof(szName));
    PrnA68MakeName(lpPlu->aucMnemonic, szName);

    /* ----- get link No. ----- */

    memset(szWork, 0, sizeof(szWork));
    memset(szLink, 0, sizeof(szLink));

    wLink = lpPlu->usLinkNumber;

    /* ----- make upper string ----- */

    // IDS_PRN_A68_TITLE_UP    " Number       Mnemonic      Type Dept R.C. TbNo GrNo PrtP  PPI   Price "
    // IDS_PRN_A68_TITLE_DN    " F1   F2   F3   F4   F5   F6   F7   F8"

    PrnA68MakePrice((LPCSTR)lpPlu->ulUnitPrice, &dwInt, &wDec);
    wsPepf(lpszUp,
             WIDE("%14.14s %-20.20s  %1u %4u  %2u   %2u   %2u   %2u %4u %6lu.%02u"),
             szPluNo,
             szName,
             lpPlu->uchItemType,
             lpPlu->usDeptNumber,
             lpPlu->uchRestrict,
             lpPlu->uchTableNumber,
             lpPlu->uchGroupNumber,
             lpPlu->uchPrintPriority,
             lpPlu->uchLinkPPI,
			 dwInt, wDec);

	*lpszDn = 0;
    for (wI = 0; wI < (PRN_A68_DESC_CON_NUM - 1); wI++) {
        if (PrnA68DescChkCon(wI, lpPlu) == TRUE) {
            bBit = PRN_A68_COND_FORM_ON;
        } else {
            bBit = PRN_A68_COND_FORM_OFF;
        }
        wsPepf(szWork, WIDE("%u"), bBit);
        wcscat(lpszDn, szWork);
		if ( ! ((wI + 1) % 4))
			wcscat(lpszDn, WIDE(" "));
    }
}


/* ===== End of PRNA068C.C ===== */