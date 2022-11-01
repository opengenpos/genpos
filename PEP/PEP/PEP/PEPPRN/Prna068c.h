/*
* ---------------------------------------------------------------------------
* Title         :   Header File for PLU Print comdensed form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068C.H
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : The only Print for PEP included this header file. This header
*           declares structure type define declarations, define declarations
*           and function prototype declarations to use the functions of the
*           Print for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
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
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       value definition
* ===========================================================================
*/
#define PRN_A68_COND_Y_NUM         13   /* number of record per page */
#define PRN_A68_COND_TITLE_X      100   /* default title x position of report */
#define PRN_A68_COND_TITLE_Y        0   /* default title y position of report */
#define PRN_A68_COND_DEF_X    PRN_A68_COND_TITLE_X
                                    /* default x position of report */
#define PRN_A68_COND_DEF_Y   (PRN_A68_COND_TITLE_Y + 270)
                                    /* default y position of report */
#define PRN_A68_COND_FRM_X         10   /* x frame of record */
#define PRN_A68_COND_FRM_Y         10   /* y frame of record */
#define PRN_A68_COND_UNIT_W      5000   /* unit width of record */
#define PRN_A68_COND_UNIT_H       195   /* unit hight of record */
#define PRN_A68_COND_FORM_OFF       0   /* display off for comdense form */
#define PRN_A68_COND_FORM_ON        1   /* display on for comdense form */
#define PRN_A68_COND_FEED_NUM       5   /* number of feed for 1 block */
#define PRN_A68_COND_FEED_Y        65   /* x feed of 1 block */

/*
* ===========================================================================
*       macro definition
* ===========================================================================
*/
/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
VOID    PrnA68CondDraw(HDC, WORD);
BOOL    PrnA68CondDrawRec(HDC, USHORT, WORD, WORD, WORD);
VOID    PrnA68CondDrawTitle(HDC, LPRECT, LPRECT, LPRECT, LPWSTR, LPWSTR, LPWSTR);
VOID    PrnA68CondMakeStr(VOID *, LPWSTR, LPWSTR);
BOOL    PrnA68GetPluNo(HWND hDlg, WORD wID, LPWSTR lpszNo, BOOL fVer);

/* ===== End of PRNA068C.H ===== */