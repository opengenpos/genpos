/*
* ---------------------------------------------------------------------------
* Title         :   Print for PLU
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnA68():         print PLU
*               PrnA68GetRecNum():get No. fo PLU record
*               PrnA68ReadRec():  read PLU record
*               PrnA68MakeName(): make PLU name
*               PrnA68MakePrice():make price
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Dec-28-93 : 00.00.02 : T.Yamamoto : Add PepReplaceMnemonic() to PrnA68MakeName()
* Jan-28-00 : 01.00.00 : hrkato     : Saratoga
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

#include	<defs32bitport.h>
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
**  Synopsis:   BOOL  PrnA68();
*
**  Input   :   HWND    hWnd    - handle of a window procedure
*               HDC     hDC     - handle of a device context
*
**  Return  :   TRUE        - user process is executed.
*               FALSE       - error occured.
*
**  Description:
*       This procedure print PLU data.
* ===========================================================================
*/
BOOL    PrnA68(HWND hWnd, HDC hDC)
{
	int nStdHeight;
	int nStdLeading;
	int nStdWidth;
	int nStdCellLeadingY, nStdCellLeadingX;
	TEXTMETRIC  tm;
	LOGFONT logfont;
	HFONT oldFont, newFont;
    BOOL    fRet;
    WORD    wRecNum;
    WORD    wI;
	WCHAR    szWork[PRN_A68_BUFF_LEN];
	WCHAR    szWork2[PRN_A68_BUFF_LEN];

    /* ----- get No. of PLU records ----- */

    PrnA68GetRecNum(&wRecNum);

    if (wRecNum == 0) {         /* no record exist */

        /* ----- display caution message ----- */

		LoadString (hPepInst, IDS_PRN_A68_ERR_NOPLU, szWork, PRN_A68_BUFF_LEN);
		LoadString (hPepInst, IDS_PRN_A68_MB_LABEL, szWork2, PRN_A68_BUFF_LEN);
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hWnd,
                   WIDE("PLU has no record."),
                   WIDE("Print"),
                   MB_ICONEXCLAMATION | MB_OK);

        return FALSE;
    }

    if (Escape(hDC, STARTDOC, 7, "PEP PLU", NULL) > 0) {

		// create the font we will use for our output.
		// we want to use a 7 point font size with the ANSI character set.
		// 
		memset (&logfont, 0, sizeof (LOGFONT));

		logfont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		logfont.lfOutPrecision = OUT_TT_PRECIS;
		logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logfont.lfQuality = DEFAULT_QUALITY;
		logfont.lfPitchAndFamily = FIXED_PITCH;
		logfont.lfCharSet = ANSI_CHARSET ;
		newFont = CreateFontIndirect (&logfont);

		if (newFont)
			oldFont = SelectObject (hDC, newFont);

		// get the text metrics for the font and the HDC we are using.
		// compute the standard line height, the standard external leading
		// between the lines, and the standard average character width.
		// we will assume a max of 14 characters per key and we add
		// to the width a 1 point white space on each side of the cell.
		GetTextMetrics (hDC, &tm);
		nStdHeight = tm.tmHeight;
		nStdLeading = tm.tmExternalLeading;
		nStdCellLeadingY = MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		nStdCellLeadingX = MulDiv(2, GetDeviceCaps(hDC, LOGPIXELSX), 72);
		nStdWidth = 16 * tm.tmAveCharWidth + nStdCellLeadingX * 2;
        for (wI = 0; wI < PrnSet.wCopy; wI++) {

            if (PrnSet.bFormA68 == PRN_FORM_A68_COND) { /* condensed form */

                /* ----- draw PLU record by condensed form ----- */

                PrnA68CondDraw(hDC, wRecNum);

            } else {    /* w/ description form */

                /* ----- draw PLU record by condensed form ----- */

                PrnA68DescDraw(hDC, wRecNum);

            }

        }

        /* ----- end printing ----- */

		if (newFont) {
			SelectObject (hDC, oldFont);
			DeleteObject (newFont);
		}

        EndDoc(hDC);
		//Escape(hDC, ENDDOC, NULL, NULL, NULL);

        fRet = TRUE;

    } else {        /* print error */

        /* ----- end printing ----- */

        EndDoc(hDC);
		//Escape(hDC, ENDDOC, NULL, NULL, NULL);

        /* ----- show error message ----- */

		LoadString (hPepInst, IDS_PRN_A68_ERR_PRINTER, szWork, PRN_A68_BUFF_LEN);
		LoadString (hPepInst, IDS_PRN_A68_MB_LABEL, szWork2, PRN_A68_BUFF_LEN);
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hWnd,
                   szWork,
                   szWork2,
                   MB_ICONEXCLAMATION | MB_OK);

        fRet = FALSE;

    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68GetRecNum()
*
**  Input       :   LPWORD  lpwNum  - address of record number
*
**  Return      :   None
*
**  Description :
*       This procedure get No. of PLU record.                   Saratoga
* ===========================================================================
*/
VOID    PrnA68GetRecNum(LPWORD lpwNum)
{
    RECPLU      PluRec;
    ECMRANGE    EcmRange;
    MFINFO      MfInfo;
    USHORT      usStat;
    USHORT      usRet;
    USHORT      usHandle;
    DWORD       dwI;

    /* ----- initialize number buffer ----- */
    *lpwNum = 0;

    /* ----- get PLU file information,  Saratoga ----- */
    usStat = PluSenseFile(FILE_PLU, &MfInfo);
    if (usStat != SPLU_COMPLETED) {   /* not yet be set the number of PLU records */
        return;
    }

    if (MfInfo.ulMaxRec != 0) {     /* PLU record exist */
        /* ----- initialize item No. structure buffer ----- */
		int i;

		memset(EcmRange.itemFrom.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
        memset(EcmRange.itemTo.aucPluNumber, 0, PRN_PLUNO_LEN * 2);
		for(i=0; i < PRN_PLUNO_LEN - 1; i++){
			EcmRange.itemFrom.aucPluNumber[i] = PrnSet.szStartA68[i];
			EcmRange.itemTo.aucPluNumber[i] = PrnSet.szEndA68[i];
		}
        //memcpy((LPSTR)EcmRange.itemFrom.aucPluNumber, (LPCSTR)PrnSet.szStartA68, PRN_PLUNO_LEN);
        //memcpy((LPSTR)EcmRange.itemTo.aucPluNumber, (LPCSTR)PrnSet.szEndA68, PRN_PLUNO_LEN);

        EcmRange.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);
        EcmRange.usReserve = 0;

        /* ----- enter critical mode ----- */
        usStat = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &usHandle, &EcmRange);

        for (dwI = 0; dwI < MfInfo.ulMaxRec; dwI++) {
            /* ----- read record ----- */
            usStat = PluReportRecord(usHandle, &PluRec, &usRet);

            if (usStat == SPLU_END_OF_FILE) {
                break;
            }
            /* bug fix, 02/27/97 */
            if (usStat == SPLU_FUNC_CONTINUED) {
                continue;
            }
            /* ----- increment number counter ----- */
            (*lpwNum)++;
        }
        /* ----- exit critical mode ----- */
        usStat = PluExitCritMode(usHandle, 0);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68MakeName()
*
**  Input       :   LPCSTR  lpSrc   - address of PLU name source
*                   LPSTR   lpszDst - address of destinate string
*
**  Return      :   None
*
**  Description :
*       This procedure make PLU name for condensed form.            Saratoga
* ===========================================================================
*/
VOID    PrnA68MakeName(LPCWSTR lpSrc, LPWSTR lpszDst)
{
	WCHAR    lpszSrc[PRN_A68_NAME_LEN + 1] = { 0 };

    /* ----- initialize buffer ----- */
	_wcsnset(lpszDst, ' ', PRN_A68_NAME_LEN );
	lpszDst[PRN_A68_NAME_LEN] = 0;

    /* ----- Replace Double Key Code (0x12 -> '~') ----- */
    PepReplaceMnemonic(lpSrc, lpszSrc, PRN_A68_NAME_LEN, PEP_MNEMO_READ);

    /* ----- copy PLU name ----- */

    //memcpy(lpszDst, lpszSrc, wcslen(lpszSrc));
	wcsncpy (lpszDst, lpszSrc, PRN_A68_NAME_LEN);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnA68MakePrice()
*
**  Input       :   LPCSTR  lpszPrc - address of price buffer
*                   LPDWORD lpdwInt - address of integer price
*                   LPWORD  lpwDec  - address of decimal price
*
**  Return      :   None
*
**  Description :
*       This procedure make price for condensed form.
* ===========================================================================
*/
VOID    PrnA68MakePrice(LPCSTR lpPrc, LPDWORD lpdwInt, LPWORD lpwDec)
{
    DWORD   dwPrice;

    /* ----- convert BYTE[3] data to DWORD data ----- */

    PepConv3Cto4L((LPBYTE)lpPrc, (LPDWORD)&dwPrice);

    /* ----- devide for decimal point ----- */

    *lpdwInt = (dwPrice / PRN_A68_CONV_CENT);
    *lpwDec = (WORD)(dwPrice % PRN_A68_CONV_CENT);

}

/*
* ===========================================================================
**  Synopsis:   BOOL    PrnA68GetPluNo();
*
**  Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               WORD    wID     - dialogbox item control ID
*               LPSTR   lpszNo  - address of start/end No.
*
**  Return  :   TRUE            - user process is executed
*               FALSE           - invalid PLU No.
*
**  Description:
*       This function get PLU No. as PLU formated data.             Saratoga
* ===========================================================================
*/
BOOL    PrnA68GetPluNo(HWND hDlg, WORD wID, LPWSTR lpszNo, BOOL fVer)
{
    short           sStat;
    LABELANALYSIS   Label;
    WORD            wI;

    /* ----- Initialize buffer ----- */
    memset((LPSTR)lpszNo, 0, PRN_PLUNO_LEN);
    memset((LPSTR)&Label, 0, sizeof(LABELANALYSIS));

    /* ----- Get PLU No. ----- */
    DlgItemGetText(hDlg, wID, Label.aszScanPlu, PLU_MAX_DIGIT + 1);

    /* ----- Check Valid Numeric Data ----- */
    for (wI = 0; wI < PLU_MAX_DIGIT; wI++) {
        if (Label.aszScanPlu[wI] == 0) {
            break;
        } else if ((Label.aszScanPlu[wI] < PRN_A68_CHAR_ASC_0) ||
                   (Label.aszScanPlu[wI] > PRN_A68_CHAR_ASC_9)) {
            return (FALSE);
        }
    }

    /* ----- Analyze PLU No. ----- */
    if (fVer) { /* set velocity status, R2.0 */
        Label.fchModifier = LA_UPC_VEL;
    }

    sStat = RflLabelAnalysis(&Label);

    if (sStat == LABEL_ERR) {
        /* ----- Convert to BCD from ASCII, Saratoga ----- */
            memcpy(lpszNo, Label.aszLookPlu, PLU_MAX_DIGIT);

    } else {                /* sStat == LABEL_OK */
        if (Label.uchType == LABEL_RANDOM) {
            if (Label.uchLookup == LA_EXE_LOOKUP) {
                /* ----- Convert to BCD from ASCII ----- */
                memcpy(lpszNo, Label.aszMaskLabel, PLU_MAX_DIGIT);

            } else {    /* Label.uchLookup == LA_NOT_LOOKUP */
                /* ----- Convert to BCD from ASCII ----- */
                memcpy(lpszNo, Label.aszMaskLabel, PLU_MAX_DIGIT);
            }

        } else {
            /* ----- Convert to BCD from ASCII ----- */
            memcpy(lpszNo, Label.aszLookPlu, PLU_MAX_DIGIT);
        }
    }

    return (TRUE);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    PrnA68ChkPluNoRange();
*
**  Input   :   none
*               
**  Return  :   TRUE            - user process is executed
*               FALSE           - invalid PLU No.
*
**  Description:
*       This function checks inputted plu no. is llegal or illegal.
* ===========================================================================
*/
BOOL    PrnA68ChkPluNoRange(VOID)
{
    WORD    wI;

    if (PrnSet.bRngA68 == PRN_RNG_A68_NUM) {

        for (wI = 0; wI < (PRN_PLUNO_LEN-1); wI++) {
            if (PrnSet.szStartA68[wI] > PrnSet.szEndA68[wI]) {
                return (FALSE);
            } else if (PrnSet.szStartA68[wI] < PrnSet.szEndA68[wI]) {
                return (TRUE);
            }
        }
        /* ----- mask c/d of plu ----- */
        if ((PrnSet.szStartA68[wI] & 0xF0) > (PrnSet.szEndA68[wI] & 0xF0)) {
            return (FALSE);
        }
    }

    return (TRUE);
}

/* ===== End of PRNA068.C ===== */