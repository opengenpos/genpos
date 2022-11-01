/*
* ---------------------------------------------------------------------------
* Title         :   Header File for PLU Print with description form
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068D.H
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
#define PRN_A68_DESC_DSC_NUM       10   /* No. of description */
#define PRN_A68_DESC_CON_NUM       34   /* No. of control code (w/o B, adj) */
#define PRN_A68_DESC_DSC_NO         0   /* offset of PLU No. */
#define PRN_A68_DESC_DSC_NAME       1   /* offset of mnemonic */
#define PRN_A68_DESC_DSC_TYPE       2   /* offset of PLU type */
#define PRN_A68_DESC_DSC_DEPT       3   /* offset of dept */
#define PRN_A68_DESC_DSC_REPT       4   /* offset of report code */
#define PRN_A68_DESC_DSC_TBLNO      5   /* offset of table number */
#define PRN_A68_DESC_DSC_GRPNO      6   /* offset of group number */
#define PRN_A68_DESC_DSC_PRTPRY     7   /* offset of print priority */
#define PRN_A68_DESC_DSC_PPI        8   /* offset of PPI */
#define PRN_A68_DESC_DSC_PRICE      9   /* offset of preset price */
#define PRN_A68_DESC_MAIN_NUM    (PRN_A68_DESC_DSC_NUM + PRN_A68_ADJ_NUM - 1)
                                        /* number of main description */
#define PRN_A68_DESC_DEF_X        300   /* default x of w/ description form */
#define PRN_A68_DESC_DEF_Y         30   /* default y of w/ description form */
#define PRN_A68_DESC_DSC_X          0   /* default x of description */
#define PRN_A68_DESC_DSC_Y         30   /* default y of description */
#define PRN_A68_DESC_DSC_W        450   /* unit width of description */
#define PRN_A68_DESC_DSC_H         60   /* unit hight of description */
#define PRN_A68_DESC_DAT_X     (PRN_A68_DESC_DSC_X + PRN_A68_DESC_DSC_W) 
                                        /* default x of data */
#define PRN_A68_DESC_DAT_Y      PRN_A68_DESC_DSC_Y
                                        /* default y of data */
#define PRN_A68_DESC_DAT_W       1000   /* unit width of data */
#define PRN_A68_DESC_DAT_H      PRN_A68_DESC_DSC_H
                                        /* unit hight of data */
#define PRN_A68_DESC_CON_DSC_X  PRN_A68_DESC_DSC_X
                                        /* default x of control code desc. */
#define PRN_A68_DESC_CON_DSC_Y (PRN_A68_DESC_DSC_Y + PRN_A68_DESC_DSC_H * (PRN_A68_DESC_MAIN_NUM))
                                        /* default y of control code desc. */
#define PRN_A68_DESC_CON_DSC_W   1300   /* unit width of control code desc. */
#define PRN_A68_DESC_CON_DSC_H  PRN_A68_DESC_DSC_H
                                        /* unit hight of control code desc. */
#define PRN_A68_DESC_CON_DAT_X (PRN_A68_DESC_CON_DSC_X + PRN_A68_DESC_CON_DSC_W)
                                        /* default x of control code data */
#define PRN_A68_DESC_CON_DAT_Y  PRN_A68_DESC_CON_DSC_Y
                                        /* default y of control code data */
#define PRN_A68_DESC_CON_DAT_W   1000   /* unit width of control code data */
#define PRN_A68_DESC_CON_DAT_H  PRN_A68_DESC_DSC_H
                                        /* unit hight of control code data */

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
typedef struct _PRNA68POS {
    WORD    wX;     /* x position */
    WORD    wY;     /* y position */
    WORD    wW;     /* width */
    WORD    wH;     /* hight */
} PRNA68POS, FAR *LPPRNA68POS;

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
VOID    PrnA68DescDraw(HDC, WORD);
BOOL    PrnA68DescDrawRec(HDC, USHORT, WORD, WORD);
VOID    PrnA68DescDrawDsc(HDC);
VOID    PrnA68DescDrawDat(HDC hDC, VOID *);
VOID    PrnA68DescDrawConDsc(HDC);
VOID    PrnA68DescDrawConDat(HDC, VOID *);
BOOL    PrnA68DescChkCon(UINT, RECPLU  *pPlu);
VOID    PrnA68DescDrawItem(HDC, LPPRNA68POS, LPCSTR);

/* ===== End of PRNA068D.H ===== */
