/*
* ---------------------------------------------------------------------------
* Title         :   Print for FSC with FSC form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNP003F.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnP03FscDrawText():draw FSC text
*               PrnP03FscMakeStr(): make string
*               PrnP03FscDrawPage(): draw page
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-20-95 : 03.00.00 : H.Ishida   : Add shift page and key 57-69
* Sep-11-95 : 03.00.01 : T.Nakahata : Add Extended FSC (Print on Demand)
* Mar-12-96 : 03.00.05 : M.Suzuki   : R3.1
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
#include    "prnp003f.h"

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
**  Synopsis    :   VOID    PrnP03FscDrawText()
*
**  Input       :   HDC         hDC    - handle of a device context
*
**  Return      :   None
*
**  Description :
*       This procedure draw FSC layout.
* ===========================================================================
*/
VOID    PrnP03FscDrawText(HDC hDC)
{
	int nStdHeight;
	int nStdLeading;
	int nStdWidth;
	int nStdCellLeadingY, nStdCellLeadingX;
	TEXTMETRIC  tm;
	LOGFONT logfont;
	HFONT oldFont, newFont;
    WORD        wX, wY, wPage;
    WCHAR       szWorkUp[PRN_P03_BUFF_LEN + 1];
    WCHAR       szWorkDn[PRN_P03_BUFF_LEN + 1];
    RECT        rRectUp;
    RECT        rRectDn;
    HGLOBAL     hMemData;
    HGLOBAL     hMemTbl;
    HGLOBAL     hPLUTbl;
    LPPRNP03    lpData;
    LPPRNP03TBL lpTbl;
    LPP03PLU    lpMenuPLU;      /* Address of PLU Menu Buffer */

    /* ----- allocate global memory ----- */

    hMemData = GlobalAlloc(GHND,
                           PRN_P03_X_NUM * PRN_P03_Y_NUM * sizeof(PRNP03));
    if (hMemData == 0) {
        return;
    }
    lpData = (LPPRNP03)GlobalLock(hMemData);
    if (lpData == NULL) {
        GlobalFree(hMemData);
        return;
    }

    hMemTbl = GlobalAlloc(GHND,
                          sizeof(PRNP03TBL));
    if (hMemTbl == 0) {
        GlobalUnlock(hMemData);
        GlobalFree(hMemData);
        return;
    }
    lpTbl = (LPPRNP03TBL)GlobalLock(hMemTbl);
    if (lpTbl == NULL) {
        GlobalUnlock(hMemData);
        GlobalFree(hMemData);
        GlobalFree(hMemTbl);
        return;
    }

    hPLUTbl  = GlobalAlloc(GHND,
                           (DWORD)(MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU)));
    if (hPLUTbl == 0) {
        GlobalUnlock(hMemData);
        GlobalFree(hMemData);
        GlobalUnlock(hMemTbl);
        GlobalFree(hMemTbl);
        return;
    }
    lpMenuPLU = (LPP03PLU)GlobalLock(hPLUTbl);
    if (lpMenuPLU == NULL) {
        GlobalUnlock(hMemData);
        GlobalFree(hMemData);
        GlobalUnlock(hMemTbl);
        GlobalFree(hMemTbl);
        GlobalFree(hPLUTbl);
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
	// to the width a 1 point white space on each side of the cell.
	GetTextMetrics (hDC, &tm);
	nStdHeight = tm.tmHeight;
	nStdLeading = tm.tmExternalLeading;
	nStdCellLeadingY = MulDiv(1, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	nStdCellLeadingX = MulDiv(1, GetDeviceCaps(hDC, LOGPIXELSX), 72);
	nStdWidth = 16 * tm.tmAveCharWidth + nStdCellLeadingX * 2;

    /* ----- read FSC parameter ----- */

    PrnP03ReadParaFsc(lpTbl);

    /* ----- read Menu PLU parameter ----- */

    PrnP03ReadParaPLU(lpMenuPLU);

    for (wPage = 0; wPage < MAX_PAGE_NO ; wPage++) {

        /* ----- display sending message ----- */

        SendMessage(hdlgPrnAbort,
                    PM_PRN_DSPMSG,
                    IDS_PRN_DSP_P03_FSC,
                    MAKELPARAM(MAX_PAGE_NO, (wPage + 1)));

        /* ----- Set FSC parameter ----- */

        PrnP03SetParaFsc(lpData, (LPPRNP03)&lpTbl->FSC[wPage]);

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

                    PrnP03FscMakeStr(lpData + wX + wY * PRN_P03_X_NUM,
                                     szWorkUp,
                                     szWorkDn,
                                     lpMenuPLU,
                                     wPage);

                    /* ----- print out text ----- */

                    DrawTextPep(hDC,
                             szWorkUp,
                             -1,
                             (LPRECT)&rRectUp,
                             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                    DrawTextPep(hDC,
                             szWorkDn,
                             -1,
                             (LPRECT)&rRectDn,
                             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                } else {

                    /* ----- make fix key string to print ---- */

                    PrnP03MakeFix((WORD)(wX + wY * PRN_P03_X_NUM),
                                     szWorkUp);

                    /* ----- print out text ----- */

                    DrawTextPep(hDC,
                             szWorkUp,
                             -1,
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

			LoadString(hResourceDll/*hPepInst*/, IDS_PRN_P03_FSC_PAGE, szBuff, PEP_STRING_LEN_MAC(szBuff));
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

    }

	if (newFont) {
		SelectObject (hDC, oldFont);
		DeleteObject (newFont);
	}

    /* ----- release global memory ----- */

    GlobalUnlock(hMemData);
    GlobalFree(hMemData);

    GlobalUnlock(hMemTbl);
    GlobalFree(hMemTbl);

    GlobalUnlock(hPLUTbl);
    GlobalFree(hPLUTbl);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03FscMakeStr()
*
**  Input       :   LPPRNP03    lpData - address of FSC data Buffer
*                   LPSTR       lpszUp - address of string buffer up side
*                   LPSTR       lpszDn - address of string buffer down side
*                   USHORT FAR *lpMenuPLU - address of Menu PLU buffer
*                   WORD        wPageNO - page number
*
**  Return      :   None
*
**  Description :
*       This procedure make FSC string.
* ===========================================================================
*/
VOID    PrnP03FscMakeStr(LPPRNP03 lpData, LPWSTR lpszUp, LPWSTR lpszDn,
                         LPP03PLU lpMenuPLU, WORD wPageNo)
{
    WCHAR    szWork[PRN_P03_BUFF_LEN],
            szDesc[PRN_P03_BUFF_LEN*2];
    LPWSTR   lpszNewLine;
    WORD    wSize;

    switch (lpData->uchFsc) {
    case P03_KEYEDPLU:
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
    case P03_PRTDEM:    /* add R3.0 */
    case P03_SEAT:

        /* ----- load string from resouce ----- */

        LoadString(hResourceDll/*hPepProgInst*/,
                   IDS_P03_DSP_NOFUNC + lpData->uchFsc,
                   szWork,
                   PEP_STRING_LEN_MAC(szWork));

        /* ----- Check Keyed PLU ----- */
        if (lpData->uchFsc == P03_KEYEDPLU) {

            /* ----- Make String with PLU # Data ----- */

            wsPepf(szDesc, szWork,
                (lpMenuPLU + (wPageNo * FSC_PLU_MAX) + (lpData->uchExt - 1))->szPluNo);

        } else {

            /* ----- Make String with Extended FSC Data ----- */

            wsPepf(szDesc, szWork, (short)lpData->uchExt);

        }

        break;

    default:

        /* ----- load string from resouce ----- */

        LoadString(hResourceDll/*hPepProgInst*/,
                   IDS_P03_DSP_NOFUNC + lpData->uchFsc,
                   szDesc,
                   PEP_STRING_LEN_MAC(szDesc));
        break;

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
		// we assume buffers have been previously memset with zero

        wcsncpy (lpszUp, szDesc, wSize);
        wcscpy (lpszDn, lpszNewLine + 1);

    }

}
#if defined(POSSIBLE_DEAD_CODE)

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03FscDrawPage()
*
**  Input       :   HDC     hDC     - handle of a device context
*                   LPRECT  lprRect - address of rectangle buffer
*                   WORD    wPage   - page No.
*
**  Return      :   None
*
**  Description :
*       This procedure draw page on the footer.
* ===========================================================================
*/
VOID    PrnP03FscDrawPage(HDC hDC, LPRECT lprRect, WORD wPage)
{
    WCHAR    szBuff[PRN_P03_BUFF_LEN];
    WCHAR    szWork[PRN_P03_BUFF_LEN];

    /* ----- set rectangle area ----- */

    lprRect->left   = PRN_P03_DEF_X + PRN_P03_FSC_PAGE_X;
    lprRect->top    = PRN_P03_DEF_Y + PRN_P03_FSC_PAGE_Y;
    lprRect->right  = PRN_P03_DEF_X + PRN_P03_FSC_PAGE_X + PRN_P03_FSC_PAGE_W;
    lprRect->bottom = PRN_P03_DEF_Y + PRN_P03_FSC_PAGE_Y + PRN_P03_FSC_PAGE_H;

    /* ----- make string to print ----- */

    LoadString(hResourceDll/*hPepInst*/, IDS_PRN_P03_FSC_PAGE, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, (WORD)(wPage + 1));

    /* ----- print out text ----- */

    DrawTextPep(hDC,
             szWork,
             wcslen(szWork),
             lprRect,
             DT_NOCLIP | DT_CENTER | DT_SINGLELINE | DT_VCENTER);

}
#endif
/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03ReadParaFsc()
*
**  Input       :   LPPRNP03TBL   lpTbl - address of all page FSC data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure read Menu PLU parameter.
* ===========================================================================
*/
VOID    PrnP03ReadParaPLU(USHORT FAR *lpMenuPLU)
{
    USHORT      usRet;

    /* ----- read Menu PLU parameter ----- */

    memset((LPSTR)lpMenuPLU, 0, 
             MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU));

    ParaAllRead(CLASS_PARAPLUNOMENU,
                (UCHAR *)lpMenuPLU,
                (USHORT)(MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU)),
                0,
                &usRet);

}

/* ===== End of PRNP003F.C ===== */
