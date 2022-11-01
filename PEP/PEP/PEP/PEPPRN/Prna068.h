/*
* ---------------------------------------------------------------------------
* Title         :   Header File for PLU Print
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PRNA068.H
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
* Mar-12-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       value definition
* ===========================================================================
*/
#define PRN_A68_BUFF_LEN          128   /* length of buffer */
#define PRN_A68_TYPE_OPEN           1   /* PLU type == open */
#define PRN_A68_TYPE_NONADJ         2   /* PLU type == non-adjective */
#define PRN_A68_TYPE_ONEADJ         3   /* PLU type == one-adjective */
#define PRN_A68_ADJ_NO1             1   /* adjective No. 1 */
#define PRN_A68_ADJ_NUM             5   /* No. of adjective (one-adjective PLU) */
#define PRN_A68_CHAR_SPC         0x20   /* character code of space */
#define PRN_A68_CONV_CENT         100L  /* convert value for dollar/cent */
#define PRN_A68_BIT_CHK          0x01   /* bit check */
#define PRN_A68_FIELD_3_4           0   /* control code field 3, 4 */
#define PRN_A68_FIELD_5_6           1   /* control code field 5, 6 */
#define PRN_A68_FIELD_7_8           2   /* control code field 7, 8 */
#define PRN_A68_FIELD_9_10          3   /* control code field 9, 10 */
#define PRN_A68_FIELD_11_12         4   /* control code field 11, 12 */
#define PRN_A68_FIELD_18            5   /* control code field 18 */
#define PRN_A68_FIELD_SIZE          4   /* field size */
#define PRN_A68_NAME_LEN           20   /* length of PLU name,  Saratoga */

/*#define PRN_A68_REC_MAX        9999    * PLU record maximum,  Saratoga */
#define PRN_A68_PLUNO_MAX        0x99   /* PLU No. maximum for 1 char.  Saratoga */
#define PRN_A68_CHAR_ASC_0       0x30   /* character code of ASCII (0), Saratoga */
#define PRN_A68_CHAR_ASC_9       0x39   /* character code of ASCII (0), Saratoga */
#define PRN_A68_LINK_LEN            4   /* length of Link No.   Saratoga */
#define PRN_A68_CONT_NUM            3   /* No. of control code  Saratoga */

#define PRN_A68_MAP_SIGN         0x01   /* control code check map */
#define PRN_A68_MAP_PRINT        0x02   /* control code check map */
#define PRN_A68_MAP_SI           0x04   /* control code check map */
#define PRN_A68_MAP_MODKEY       0x08   /* control code check map */
#define PRN_A68_MAP_MODCON       0x10   /* control code check map */
#define PRN_A68_MAP_VALID        0x20   /* control code check map */
#define PRN_A68_MAP_OPTREC       0x40   /* control code check map */
#define PRN_A68_MAP_RECTP        0x80   /* control code check map */
#define PRN_A68_MAP_TAX1         0x01   /* control code check map */
#define PRN_A68_MAP_TAX2         0x02   /* control code check map */
#define PRN_A68_MAP_TAX3         0x04   /* control code check map */
#define PRN_A68_MAP_DISC1        0x40   /* control code check map */
#define PRN_A68_MAP_DISC2        0x80   /* control code check map */
#define PRN_A68_MAP_SCALE        0x01   /* control code check map */
#define PRN_A68_MAP_COND         0x02   /* control code check map */
#define PRN_A68_MAP_CONT         0x08   /* control code check map */
#define PRN_A68_MAP_RMT1         0x10   /* control code check map */
#define PRN_A68_MAP_RMT2         0x20   /* control code check map */
#define PRN_A68_MAP_RMT3         0x40   /* control code check map */
#define PRN_A68_MAP_RMT4         0x80   /* control code check map */
#define PRN_A68_MAP_RMT5         0x01   /* control code check map */
#define PRN_A68_MAP_RMT6         0x02   /* control code check map */
#define PRN_A68_MAP_RMT7         0x04   /* control code check map */
#define PRN_A68_MAP_RMT8         0x08   /* control code check map */
#define PRN_A68_MAP_ADJ1         0x10   /* control code check map */
#define PRN_A68_MAP_ADJ2         0x20   /* control code check map */
#define PRN_A68_MAP_ADJ3         0x40   /* control code check map */
#define PRN_A68_MAP_ADJ4         0x80   /* control code check map */
#define PRN_A68_MAP_VARI1        0x08   /* control code check map */
#define PRN_A68_MAP_VARI2        0x10   /* control code check map */
#define PRN_A68_MAP_VARI3        0x20   /* control code check map */
#define PRN_A68_MAP_VARI4        0x40   /* control code check map */
#define PRN_A68_MAP_VARI5        0x80   /* control code check map */

/*
* ===========================================================================
*       macro definition
* ===========================================================================
*/
#define PRNA68GETTYPE(x)    ((UCHAR)((UCHAR)(x) >> 6))
#define PRNA68GETDEPT(x)    ((UCHAR)((UCHAR)(x) & 0x3f))
#define PRNA68GETREPT(x)    ((UCHAR)((UCHAR)(x) & 0x0f))
#define PRNA68GETBONUS(x)   ((UCHAR)((UCHAR)(x) & 0x0f))
#define PRNA68GETADJ(x)     ((UCHAR)((UCHAR)(x) & 0x07))
#define PRNA68GETTBLNO(x)   ((UCHAR)((UCHAR)(x) & 0x3f))
#define PRNA68GETGRPNO(x)   ((UCHAR)((UCHAR)(x) & 0xff))
#define PRNA68GETPRTPRY(x)  ((UCHAR)((UCHAR)(x) & 0x3f))
#define PRNA68GETPPI(x)     ((UCHAR)((UCHAR)(x) & 0xff))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
/*typedef RECPLU  FAR*    LPRECPLU; */

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
BOOL    PrnA68(HWND, HDC);
VOID    PrnA68GetRecNum(LPWORD);
VOID    PrnA68MakeName(LPCWSTR, LPWSTR);
VOID    PrnA68MakePrice(LPCSTR, LPDWORD, LPWORD);

/* ===== End of PRNA068.H ===== */