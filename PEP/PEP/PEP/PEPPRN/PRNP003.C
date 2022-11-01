/*
* ---------------------------------------------------------------------------
* Title         :   Print for FSC
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNP003.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               PrnP03():           print FSC
*               PrnP03ReadParaFsc():all read parameter
*               PrnP03SetParaFsc(): set parameter
*               PrnP03ChkFix():     check fix key
*               PrnP03DrawFrame():  draw keyboard frame
*               PrnP03MakeFix():    make fix key string
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-20-95 : 03.00.00 : H.Ishida   : Add shift page and key 57-69
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
#include    "prnp003f.h"
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
**  Synopsis:   BOOL  PrnP03();
*
**  Input   :   HWND    hWnd    - handle of a window procedure
*               HDC     hDC     - handle of a device context
*
**  Return  :   TRUE        - user process is executed.
*               FALSE       - error occured.
*
**  Description:
*       This procedure print fsc data.
* ===========================================================================
*/
BOOL    PrnP03(HWND hWnd, HDC hDC)
{
    BOOL    fRet;
    WORD    wI;
	WCHAR    szWork[128];
	WCHAR    szWork2[28];

    if (Escape(hDC, STARTDOC, 7, (LPCSTR)"PEP FSC", NULL) > 0) {

        for (wI = 0; wI < PrnSet.wCopy; wI++) {

            if (PrnSet.bFormP03 == PRN_FORM_P03_FSC) {

                /* ----- draw FSC layout ----- */
                PrnP03FscDrawText(hDC);

            } else if (PrnSet.bFormP03 == PRN_FORM_P03_PLUKEY) {

                /* ----- draw PLU key layout ----- */
                PrnP03PluDrawText(hDC);

            } else {

                /* ----- display sending message ----- */

    		SendMessage(hdlgPrnAbort,
                            PM_PRN_DSPMSG,
                            IDS_PRN_DSP_P03_FSC,
                            0L);

		/* ----- draw keyboard frame  ----- */

                PrnP03DrawFrame(hDC);

                /* ----- end page ----- */

                EndPage(hDC);
				//Escape(hDC, NEWFRAME, NULL, NULL, NULL);

            }

        }

        /* ----- end printing ----- */

        EndDoc(hDC);
		//Escape(hDC, ENDDOC, NULL, NULL, NULL);

        fRet = TRUE;

    } else {        /* print error */

        /* ----- end printing ----- */

        EndDoc(hDC);
		//Escape(hDC, ENDDOC, NULL, NULL, NULL);

        /* ----- show error message ----- */

		LoadString (hResourceDll/*hPepInst*/, IDS_PRN_A68_ERR_PRINTER, szWork, PEP_STRING_LEN_MAC(szWork) - 1);
		LoadString (hResourceDll/*hPepInst*/, IDS_PRN_A68_MB_LABEL, szWork2, PEP_STRING_LEN_MAC(szWork2) - 1);
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
**  Synopsis    :   VOID    PrnP03ReadParaFsc()
*
**  Input       :   LPPRNP03TBL   lpTbl - address of all page FSC data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure read FSC parameter.
* ===========================================================================
*/
VOID    PrnP03ReadParaFsc(LPPRNP03TBL lpTbl)
{
    USHORT      usRet;

    /* ----- read FSC parameter ----- */

    memset((LPSTR)lpTbl, 0, sizeof(PRNP03TBL));

    ParaAllRead(CLASS_PARAFSC,
                (UCHAR *)lpTbl,
                sizeof(PRNP03TBL),
                0,
                &usRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03SetParaFsc()
*
**  Input       :   LPPRNP03      lpData - address of one page FSC data Buffer
**                  LPPRNP03      lpTbl  - address of all page FSC data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure Set loaded FSC data to buffer area.
* ===========================================================================
*/
VOID    PrnP03SetParaFsc(LPPRNP03 lpData, LPPRNP03 lpTbl)
{
    WORD        wI, wJ;

    /* ----- Set loaded FSC data to buffer area ----- */

    for (wI = wJ = 0; wI < (PRN_P03_X_NUM * PRN_P03_Y_NUM); wI++) {

        if (PrnP03ChkFix(wI) == TRUE) {   /* Normal Key   */

            /* ----- copy to buffer ----- */

            (lpData + wI)->uchFsc = (lpTbl + wJ)->uchFsc;
            (lpData + wI)->uchExt = (lpTbl + wJ)->uchExt;

            wJ++;
        }
    }

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    PrnP03ChkFix()
*
**  Input       :   WORD    wOfs - offset of parameter
*
**  Return      :   None
*
**  Description :
*       This procedure check FSC fix key.
* ===========================================================================
*/
BOOL    PrnP03ChkFix(WORD wOfs)
{
    BOOL    fRet;

    switch (wOfs) {
    case PRN_P03_OFS_CLR:       /* clear key */
    case PRN_P03_OFS_7:         /* 7 key */
    case PRN_P03_OFS_8:         /* 8 key */
    case PRN_P03_OFS_9:         /* 9 key */
    case PRN_P03_OFS_4:         /* 4 key */
    case PRN_P03_OFS_5:         /* 5 key */
    case PRN_P03_OFS_6:         /* 6 key */
    case PRN_P03_OFS_1:         /* 1 key */
    case PRN_P03_OFS_2:         /* 2 key */
    case PRN_P03_OFS_3:         /* 3 key */
    case PRN_P03_OFS_0:         /* 0 key */
        fRet = FALSE;
        break;

    default:
        fRet = TRUE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03DrawFrame()
*
**  Input       :   HDC         hDC    - handle of a device context
*
**  Return      :   None
*
**  Description :
*       This procedure draw keyboard frame.
* ===========================================================================
*/
VOID    PrnP03DrawFrame(HDC hDC)
{
	int nStdHeight;
	int nStdLeading;
	int nStdWidth;
	int nStdCellLeadingY, nStdCellLeadingX;
	TEXTMETRIC  tm;
	LOGFONT logfont;
	HFONT oldFont, newFont;
    RECT        rRectUp;
    RECT        rRectDn;
    WORD    wX, wY;
    int     nXpage, nYpage;

    /* ----- get validate area of device by pixel scale ----- */

    nXpage = GetDeviceCaps(hDC, HORZRES);
    nYpage = GetDeviceCaps(hDC, VERTRES);

    /* ----- save handle o fdevice context ----- */

    SaveDC(hDC);

    /* ----- set mapping mode ----- */

    SetMapMode(hDC, MM_TEXT);

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

            /* ----- draw individual frame ----- */

			Rectangle(hDC, rRectUp.left - nStdCellLeadingX, rRectUp.top - nStdCellLeadingY, rRectDn.right + nStdCellLeadingX, rRectDn.bottom + nStdCellLeadingY);
			rRectUp.top = rRectDn.bottom + nStdLeading + nStdCellLeadingY;
			rRectUp.bottom = rRectUp.top + nStdHeight;
			rRectDn.top = rRectUp.bottom + nStdLeading;
			rRectDn.bottom = rRectDn.top + nStdHeight;
        }
    }

	if (newFont) {
		SelectObject (hDC, oldFont);
		DeleteObject (newFont);
	}

    /* ----- restore handle of device context ----- */

    RestoreDC(hDC, -1);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    PrnP03MakeFix()
*
**  Input       :   WORD    wOfs   - offset of parameter
*                   LPSTR   lpszUp - address of string buffer up side
*
**  Return      :   None
*
**  Description :
*       This procedure make FSC string.
* ===========================================================================
*/
VOID    PrnP03MakeFix(WORD wOfs, LPWSTR lpszUp)
{
    UINT    wID;

    switch (wOfs) {
    case PRN_P03_OFS_CLR:       /* clear key */
        wID = IDS_PRN_P03_CLR;
        break;

    case PRN_P03_OFS_7:         /* 7 key */
        wID = IDS_PRN_P03_7;
        break;

    case PRN_P03_OFS_8:         /* 8 key */
        wID = IDS_PRN_P03_8;
        break;

    case PRN_P03_OFS_9:         /* 9 key */
        wID = IDS_PRN_P03_9;
        break;

    case PRN_P03_OFS_4:         /* 4 key */
        wID = IDS_PRN_P03_4;
        break;

    case PRN_P03_OFS_5:         /* 5 key */
        wID = IDS_PRN_P03_5;
        break;

    case PRN_P03_OFS_6:         /* 6 key */
        wID = IDS_PRN_P03_6;
        break;

    case PRN_P03_OFS_1:         /* 1 key */
        wID = IDS_PRN_P03_1;
        break;

    case PRN_P03_OFS_2:         /* 2 key */
        wID = IDS_PRN_P03_2;
        break;

    case PRN_P03_OFS_3:         /* 3 key */
        wID = IDS_PRN_P03_3;
        break;

    case PRN_P03_OFS_0:         /* 0 key */
        wID = IDS_PRN_P03_0;
        break;

    default:
        wID = 0;
        break;
    }

    /* ----- load string from resouce ----- */

    LoadString(hResourceDll/*hPepInst*/, wID, lpszUp, PRN_P03_BUFF_LEN);

}

/* ===== End of PRNP003.C ===== */
