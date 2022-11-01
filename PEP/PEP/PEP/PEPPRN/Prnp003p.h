/*
* ---------------------------------------------------------------------------
* Title         :   Header File for FSC Print by PLU key form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNP003P.H
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
*       Define Declarations
* ===========================================================================
*/
#define PRN_P03_PLU_PAGE_X    844   /* x position of page display */
#define PRN_P03_PLU_PAGE_Y   3050   /* y position of page display */
#define PRN_P03_PLU_PAGE_W    622   /* width of page display */
#define PRN_P03_PLU_PAGE_H    100   /* hight of page display */

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
VOID    PrnP03PluDrawText(HDC);
WORD    PrnP03PluReadPage(LPBYTE);
BOOL    PrnP03PluChkPage(WORD, LPBYTE);
VOID    PrnP03PluMakeStr(LPPRNP03, LPP03PLU, LPWSTR, LPWSTR);
VOID    PrnP03PluReadPara(LPPRNP03, LPP03PLU);

/* ===== End of PRNP003P.H ===== */
