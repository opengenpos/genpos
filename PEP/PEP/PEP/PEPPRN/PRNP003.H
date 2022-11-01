/*
* ---------------------------------------------------------------------------
* Title         :   Header File for FSC Print
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNP003.H
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
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
* Apr-20-95 : 03.00.00 : H.Ishida   : add shift page and key 57-69
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
#define PRN_P03_X_NUM          11   /* number of keybord x */
#define PRN_P03_Y_NUM          16   /* number of keybord y */
#define PRN_P03_DEF_X          10   /* default x position of keybord */
#define PRN_P03_DEF_Y          60   /* default y position of keybord */
#define PRN_P03_FRM_X          10   /* x frame of keyboard */
#define PRN_P03_FRM_Y          10   /* y frame of keyboard */
#define PRN_P03_UNIT_W        211   /* unit width of keyboard */
#define PRN_P03_UNIT_H        179   /* unit hight of keyboard */
#define PRN_P03_NEWLINE      0x0a   /* character code of '\n' */
#define PRN_P03_BUFF_LEN       32   /* length of buffer */

#define PRN_P03_OFS_CLR       115   /* offset of clear key */
#define PRN_P03_OFS_7         137   /* offset of 7 key */
#define PRN_P03_OFS_8         138   /* offset of 8 key */
#define PRN_P03_OFS_9         139   /* offset of 9 key */
#define PRN_P03_OFS_4         148   /* offset of 4 key */
#define PRN_P03_OFS_5         149   /* offset of 5 key */
#define PRN_P03_OFS_6         150   /* offset of 6 key */
#define PRN_P03_OFS_1         159   /* offset of 1 key */
#define PRN_P03_OFS_2         160   /* offset of 2 key */
#define PRN_P03_OFS_3         161   /* offset of 3 key */
#define PRN_P03_OFS_0         170   /* offset of 0 key */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct _PRNP03 {   /* data buffer for FSC */
    UCHAR   uchFsc;             /* major FSC data           */
    UCHAR   uchExt;             /* extended FSC data        */
} PRNP03, FAR *LPPRNP03;

typedef struct _PRNP03TBL {   /* Data Buffer for Prog. #3  */
    PARAFSCTBL UNINT FSC[MAX_FSC_TBL];    /* a page FSC Data   */
} PRNP03TBL, FAR *LPPRNP03TBL;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

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
BOOL    PrnP03(HWND, HDC);
VOID    PrnP03ReadParaFsc(LPPRNP03TBL);
VOID    PrnP03SetParaFsc(LPPRNP03, LPPRNP03);
BOOL    PrnP03ChkFix(WORD);
VOID    PrnP03DrawFrame(HDC);
VOID    PrnP03MakeFix(WORD, LPWSTR);

/* ===== End of PRNP003.H ===== */
