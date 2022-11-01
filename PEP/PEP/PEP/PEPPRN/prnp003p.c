/*
* ---------------------------------------------------------------------------
* Title         :   Print for FSC with PLU key form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNP003P.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnP03PluDrawText():draw PLU key text
*               PrnP03PluReadPara():read PLU key parameter
*               PrnP03PluReadPage():read menu page parameter(AC 5)
*               PrnP03PluChkPage(): check menu page (AC 5)
*               PrnP03PluMakeStr(): make string
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-20-95 : 03.00.00 : H.Ishida   : Add shift page and key 57-69 of FSC
* Jan-22-98 : 01.00.08 : M.Ozawa    : Add print on demand key and seat no. key
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
#include    <windows.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "prog.h"
#include    "p003.h"
#include    "pepprn.h"
#include    "pepprnl.h"
#include    "prnp003.h"
#include    "prnp003p.h"

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
**  Synopsis    :   VOID    PrnP03PluDrawText()
*
**  Input       :   HDC         hDC    - handle of a device context
*
**  Return      :   None
*
**  Description :
*       This procedure draw PLU key layout.
* ===========================================================================
*/
VOID    PrnP03PluDrawText(HDC hDC)
{
	int nStdHeight;
	int nStdLeading;
	int nStdWidth;
	int nStdCellLeadingY, nStdCellLeadingX;
	TEXTMETRIC  tm;
	LOGFONT logfont;
	HFONT oldFont, newFont;
    WORD        wX, wY, wPage;
    WCHAR        szWorkUp[PRN_P03_BUFF_LEN];
    WCHAR        szWorkDn[PRN_P03_BUFF_LEN];
    RECT        rRectUp;
    RECT        rRectDn;
    HGLOBAL     hMemFsc;
    HGLOBAL     hMemPlu;
    HGLOBAL     hMemTbl;
    LPPRNP03    lpFsc;
    LPP03PLU      lpwPlu;
    LPPRNP03TBL lpTbl;
    BYTE        abData[MAX_PAGE_SIZE];
    WORD        wTotal;
    WORD        wCount;

    /* ----- allocate global memory ----- */

    if ((hMemFsc = GlobalAlloc(GHND,
                               PRN_P03_X_NUM * PRN_P03_Y_NUM * MAX_PAGE_NO
                               * sizeof(PRNP03))) == 0) {
        return;
    } else if ((hMemPlu = GlobalAlloc(GHND,
                               PRN_P03_X_NUM * PRN_P03_Y_NUM * MAX_PAGE_NO
                               * sizeof(P03PLU))) == 0) {
        GlobalFree(hMemFsc);
        return;
    } else if ((hMemTbl = GlobalAlloc(GHND, sizeof(PRNP03TBL))) == 0) {
        GlobalFree(hMemFsc);
        GlobalFree(hMemPlu);
        return;
    } else if ((lpFsc = (LPPRNP03)GlobalLock(hMemFsc)) == NULL) {
        GlobalFree(hMemFsc);
        GlobalFree(hMemPlu);
        GlobalFree(hMemTbl);
        return;
    } else if ((lpwPlu = (LPP03PLU)GlobalLock(hMemPlu)) == NULL) {
        GlobalUnlock(hMemFsc);
        GlobalFree(hMemFsc);
        GlobalFree(hMemPlu);
        GlobalFree(hMemTbl);
        return;
    } else if ((lpTbl = (LPPRNP03TBL)GlobalLock(hMemTbl)) == NULL) {
        GlobalUnlock(hMemFsc);
        GlobalUnlock(hMemPlu);
        GlobalFree(hMemFsc);
        GlobalFree(hMemPlu);
        GlobalFree(hMemTbl);
        return;
    }

	// create the font we will use for our output.
	// we want to use a 7 point font size with the ANSI character set.
	// 
	memset (&logfont, 0, sizeof (LOGFONT));

	logfont.lfHeight = -MulDiv(7, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	logfont.lfCharSet = ANSI_CHARSET ;
	newFont = CreateFontIndirect (&logfont);

	if (newFont)
		oldFont = SelectObject (hDC, newFont);

	// get the text metrics for the font and the HDC we are using.
	// compute the standard line height, the standard external leading
	// between the lines, and the standard average character width.
	// we will assume a max of 14 characters per key and we add
	// to the width a 2 point white space on each side of the cell.
	GetTextMetrics (hDC, &tm);
	nStdHeight = tm.tmHeight;
	nStdLeading = tm.tmExternalLeading;
	nStdCellLeadingY = MulDiv(1, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	nStdCellLeadingX = MulDiv(1, GetDeviceCaps(hDC, LOGPIXELSX), 72);
	nStdWidth = 16 * tm.tmAveCharWidth + nStdCellLeadingX * 2;

    /* ----- read FSC parameter ----- */

    PrnP03ReadParaFsc(lpTbl);

    for (wPage = 0; wPage < MAX_PAGE_NO; wPage++) {

        /* ----- Set FSC parameter ----- */

        PrnP03SetParaFsc((LPPRNP03)(lpFsc+ wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM),
                         (LPPRNP03)&lpTbl->FSC[wPage]);
    }

    /* ----- read PLU key parameter ----- */

    PrnP03PluReadPara(lpFsc, lpwPlu);

    /* ----- read menu page parameter ----- */

    wTotal = PrnP03PluReadPage((LPBYTE)abData);

    for (wPage = 0, wCount = 1; wPage < MAX_PAGE_NO; wPage++) {

        if (PrnP03PluChkPage(wPage, (LPBYTE)abData) == TRUE) {  /* check menu page (AC 5) */
            continue;
        }

        /* ----- display sending message ----- */

        SendMessage(hdlgPrnAbort,
                    PM_PRN_DSPMSG,
                    IDS_PRN_DSP_P03_PLU,
                    MAKELPARAM(wTotal, wCount));

        for (wX = 0; wX < PRN_P03_X_NUM; wX++) {
			rRectUp.left = wX * nStdWidth + nStdCellLeadingX;
			rRectUp.top = nStdCellLeadingY;
			rRectUp.right = rRectUp.left + nStdWidth;
			rRectUp.bottom = rRectUp.top + nStdHeight;
			rRectDn.left = wX * nStdWidth + nStdCellLeadingX;
			rRectDn.top = rRectUp.bottom + nStdCellLeadingY;
			rRectDn.right = rRectDn.left + nStdWidth;
			rRectDn.bottom = rRectDn.top + nStdHeight;
            for (wY = 0; wY < PRN_P03_Y_NUM; wY++) {

                /* ----- initialize work area ----- */

                memset(szWorkUp, 0, sizeof(szWorkUp));
                memset(szWorkDn, 0, sizeof(szWorkDn));

				// ----  print out the rectangle surrounding the text

				Rectangle(hDC, rRectUp.left - nStdCellLeadingX, rRectUp.top - nStdCellLeadingY, rRectDn.right + nStdCellLeadingX, rRectDn.bottom + nStdCellLeadingY);

                if (PrnP03ChkFix((WORD)(wX + wY * PRN_P03_X_NUM)) == TRUE) {
                                                            /* Normal Key   */
                    /* ----- make string to print ---- */

                    PrnP03PluMakeStr(lpFsc + wX + wY * PRN_P03_X_NUM
                                      + wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM,
                                     lpwPlu + wX + wY * PRN_P03_X_NUM
                                      + wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM,
                                     szWorkUp,
                                     szWorkDn);

                    /* ----- print out text ----- */

                    DrawTextPep(hDC,
                             szWorkUp,
                             wcslen(szWorkUp),
                             (LPRECT)&rRectUp,
                             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                    DrawTextPep(hDC,
                             szWorkDn,
                             wcslen(szWorkDn),
                             (LPRECT)&rRectDn,
                             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                } else {

                    /* ----- make fix key string to print ---- */

                    PrnP03MakeFix((WORD)(wX + wY * PRN_P03_X_NUM),
                                  szWorkUp);

                    /* ----- print out text ----- */

                    DrawTextPep(hDC,
                             szWorkUp,
                             wcslen(szWorkUp),
                             (LPRECT)&rRectUp,
                             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                }
				rRectUp.top = rRectDn.bottom + nStdLeading + nStdCellLeadingY;
				rRectUp.bottom = rRectUp.top + nStdHeight;
				rRectDn.top = rRectUp.bottom + nStdLeading;
				rRectDn.bottom = rRectDn.top + nStdHeight;
            }
        }

        /* ----- draw page No. ----- */

		{
			WCHAR    szBuff[PRN_P03_BUFF_LEN];
			WCHAR    szWork[PRN_P03_BUFF_LEN];

			/* ----- set rectangle area ----- */
			rRectUp.top = rRectDn.bottom + nStdLeading + nStdCellLeadingY;
			rRectUp.bottom = rRectUp.top + nStdHeight;


			/* ----- make string to print ----- */

			LoadString(hResourceDll/*hPepInst*/, IDS_PRN_P03_PLU_PAGE, szBuff, PEP_STRING_LEN_MAC(szBuff));
			wsPepf(szWork, szBuff, (WORD)(wPage + 1));

			rRectUp.left   = (PRN_P03_X_NUM * nStdWidth / 2) - (wcslen (szWork) * tm.tmMaxCharWidth / 2);
			rRectUp.right  = rRectUp.left + wcslen (szWork) * tm.tmMaxCharWidth;

			/* ----- print out text ----- */

			DrawTextPep(hDC,
					 szWork,
					 wcslen(szWork),
					 &rRectUp,
					 DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		}

        /* ----- end page ----- */

        EndPage(hDC);
		//Escape(hDC, NEWFRAME, NULL, NULL, NULL);

        /* ----- increment page counter ----- */

        wCount++;

    }

	if (newFont)
		SelectObject (hDC, oldFont);


    /* ----- release global memory ----- */

    GlobalUnlock(hMemFsc);
    GlobalUnlock(hMemPlu);
    GlobalUnlock(hMemTbl);
    GlobalFree(hMemFsc);
    GlobalFree(hMemPlu);
    GlobalFree(hMemTbl);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03PluReadPara()
*
**  Input       :   LPPRNP03    lpFsc   - address of FSC data Buffer
*                   LPWORD      lpwPlu  - address of PLU key data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure read PLU key parameter.
* ===========================================================================
*/
VOID    PrnP03PluReadPara(LPPRNP03 lpFsc, LPP03PLU lpwPlu)
{
    HGLOBAL     hMemData;
    LPP03PLU    lpwData;
    USHORT      usRet;
    WORD        wI, wJ, wPage;

    /* ----- allocate global memory ----- */
    if ((hMemData = GlobalAlloc(GHND,
                                MAX_PAGE_NO * FSC_PLU_MAX
                                * sizeof(P03PLU))) == 0) {
        return;
    } else if ((lpwData = (LPP03PLU)GlobalLock(hMemData)) == NULL) {
        GlobalFree(hMemData);
        return;
    }

    /* ----- read PLU key parameter ----- */

    memset((LPSTR)lpwData, 0, 
             MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU));

    ParaAllRead(CLASS_PARAPLUNOMENU,
                (UCHAR *)lpwData,
                (USHORT)(MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU)),
                0,
                &usRet);

    /* ----- Set loaded PLU key data to buffer area ----- */

    for (wPage = 0; wPage < MAX_PAGE_NO; wPage++) {

        for (wI = wJ = 0; wI < (PRN_P03_X_NUM * PRN_P03_Y_NUM); wI++) {

            if (PrnP03ChkFix(wI) == TRUE) {   /* Normal Key   */

                if ((lpFsc + wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM + wI)->uchFsc
                                                         == P03_KEYEDPLU) {     /* PLU key */

                    /* ----- copy to buffer ----- */

                    *(lpwPlu + wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM + wI)
                        = *(lpwData + wPage * FSC_PLU_MAX
                           + ((lpFsc + wPage * PRN_P03_X_NUM * PRN_P03_Y_NUM + wI)->uchExt) - 1);

                }

                wJ++;
            }

        }

    }

    /* ----- release global memory ----- */

    GlobalUnlock(hMemData);
    GlobalFree(hMemData);

}

/*
* ===========================================================================
**  Synopsis    :   WORD    PrnP03PluReadPage()
*
**  Input       :   LPBYTE  lpbData - address of data buffer
*
**  Return      :   WORD    wTotal  - total used pages
*
**  Description :
*       This procedure read menu page parameter(AC 5).
* ===========================================================================
*/
WORD    PrnP03PluReadPage(LPBYTE lpbData)
{
    USHORT  usRet;
    WORD    wI;
    WORD    wTotal;

    /* ----- initiaize total pages ----- */

    wTotal = (WORD)(MAX_PAGE_SIZE - 1);

    /* ----- read menu page parameter ----- */

    ParaAllRead(CLASS_PARACTLTBLMENU,
                (UCHAR *)lpbData,
                MAX_PAGE_SIZE * sizeof(BYTE),
                0,
                &usRet);

    for (wI = 1; wI < MAX_PAGE_SIZE; wI++) {
        if (*(lpbData + wI)) {
            wTotal--;
        }
    }

    return (wTotal);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    PrnP03PluChkPage()
*
**  Input       :   WORD    wPage   - page No.
*                   LPBYTE  lpbData - address of data buffer
*
**  Return      :   TRUE    - not use menu page 
*                   FALSE   - use menu page
*
**  Description :
*       This procedure check menu page (AC 5).
* ===========================================================================
*/
BOOL    PrnP03PluChkPage(WORD wPage, LPBYTE lpbData)
{
    BOOL    fRet;

    if (*(lpbData + wPage + 1)) {
        fRet = TRUE;
    } else {
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03PluMakeStr()
*
**  Input       :   LPPRNP03    lpFsc  - address of FSC data Buffer
*                   LPWORD      lpwPlu - address of PLU key data Buffer
*                   LPSTR       lpszUp - address of string buffer up side
*                   LPSTR       lpszDn - address of string buffer down side
*
**  Return      :   None
*
**  Description :
*       This procedure make FSC string.
* ===========================================================================
*/
VOID    PrnP03PluMakeStr(LPPRNP03 lpFsc,
                         LPP03PLU lpwPlu,
                         LPWSTR lpszUp,
                         LPWSTR lpszDn)
{
    WCHAR    szWork[PRN_P03_BUFF_LEN],
            szDesc[PRN_P03_BUFF_LEN*2];
    LPWSTR   lpszNewLine;
    WORD    wSize;

    switch (lpFsc->uchFsc) {
    case P03_KEYEDPLU:

        /* ----- load string from resouce ----- */

        LoadString(hResourceDll/*hPepProgInst*/,
                   IDS_P03_DSP_NOFUNC + P03_KEYEDPLU,
                   szWork,
                   PEP_STRING_LEN_MAC(szWork));

        /* ----- Make String with Extended FSC Data ----- */

        wsPepf(szDesc, szWork, lpwPlu->szPluNo);

        break;

    case P03_KEYEDDPT:
    case P03_PRTMOD:
    case P03_ADJ:
    case P03_TENDER:
    case P03_FC:
    case P03_TTL:
    case P03_ADDCHK:
    case P03_IDISC:
    case P03_RDISC:
    case P03_TAXMOD:
    case P03_PRECASH:
    case P03_DRCTSHFT:
    case P03_KEYEDCPN:
    case P03_KEYEDSTR:
    case P03_PRTDEM:    /* add 1/22/98 */
    case P03_SEAT:      /* add 1/22/98 */

        /* ----- load string from resouce ----- */

        LoadString(hResourceDll/*hPepProgInst*/,
                   IDS_P03_DSP_NOFUNC + lpFsc->uchFsc,
                   szWork,
                   PEP_STRING_LEN_MAC(szWork));

        /* ----- Make String with Extended FSC Data ----- */

        wsPepf(szDesc, szWork, lpFsc->uchExt);

        break;

    default:

        /* ----- load string from resouce ----- */

        LoadString(hResourceDll/*hPepProgInst*/,
                   IDS_P03_DSP_NOFUNC + lpFsc->uchFsc,
                   szDesc,
                   PEP_STRING_LEN_MAC(szDesc));

    }

    /* ----- get address of '\'(new line) from string ----- */

    lpszNewLine = wcschr(szDesc, PRN_P03_NEWLINE);

    if (lpszNewLine == NULL) {    /* cannot find '\' */

        /* ----- copy to buffer ----- */

        wcscpy(lpszUp, szDesc);

    } else {

        /* ----- get size of upper size string ----- */

        wSize = (WORD)(lpszNewLine - szDesc);

        /* ----- copy to buffer ----- */

        wcsncpy (lpszUp, szDesc, wSize);
        wcscpy(lpszDn, lpszNewLine + 1);

    }

}

/* ===== End of PRNP003P.C ===== */
