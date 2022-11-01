/*
* ---------------------------------------------------------------------------
* Title         :   Local Header File of Print Module
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PEPPRNL.H
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
#define PRN_FLAG_P03           0x0001   /* print flag for P03 */
#define PRN_FLAG_A68           0x0010   /* print flag for A68 */

#define PRN_FORM_P03_FSC         0x01   /* print FSC form */
#define PRN_FORM_P03_PLUKEY      0x02   /* print PLU key form */
#define PRN_FORM_P03_BLANK       0x03   /* print balnk form */

#define PRN_FORM_A68_COND        0x01   /* print comdensed form */
#define PRN_FORM_A68_DESC        0x02   /* print w/ description form */

#define PRN_RNG_A68_ALL          0x01   /* print all PLU record */
#define PRN_RNG_A68_NUM          0x02   /* print PLU No. inputed */

#define PRN_BUFF_LEN               72   /* length of buffer */
#define PRN_PLUNO_LEN              STD_PLU_NUMBER_LEN   /* length of PLU No.    Saratoga */
#define PRN_PLUENTRY_LEN           STD_PLU_NUMBER_LEN   /* length of PLU No.    Saratoga */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _PRNSET {   /* user setting to print */
    WORD    wCopy;              /* number of copies */
    WORD    fwSelect;           /* select item flag */
    BYTE    bFormP03;           /* select form for P03 */
    BYTE    bFormA68;           /* select form for A68 */
    BYTE    bRngA68;            /* select range for A68 */
    WCHAR    szStartA68[PRN_PLUNO_LEN + 1];  /* start record No. for A68 */
    WCHAR    szEndA68[PRN_PLUNO_LEN + 1];    /* end record No. for A68 */
/*    WORD    wStartA68;         * start record No. for A68 *
    WORD    wEndA68;             * end record No. for A68 */
} PRNSET, FAR *LPPRNSET;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
extern  HWND    hdlgPrnAbort;
extern  PRNSET  PrnSet;

/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
VOID    PepPrnCtrlDlgItem(HWND);
VOID    PepPrnGetItemStat(HWND);
VOID    PepPrnInitFormBtn(HWND);
BOOL    PepPrnOut(HWND);
VOID    PepPrnDspStatMsg(HWND, WPARAM, LPARAM);

/* ===== End of PEPPRNL.H ===== */
