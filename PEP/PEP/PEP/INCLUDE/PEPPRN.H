/*
* ---------------------------------------------------------------------------
* Title         :   Public Header File of Print Module
* Category      :   Print, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PEPPRN.H
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : To use the functions of the Print for PEP, included
*           this header file. This header declares structure type
*           define declarations, define declarations and function
*           prototype declarations to use the functions of the Print
*           for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-03-95 : 03.00.00 : H.Ishida   : Add Table number, Group Number and Print
*                                     Priority by PLU
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
#ifndef PEPPRN_INCLUDED_DEFINED
#define PEPPRN_INCLUDED_DEFINED


#define _INC_PEPPRN

/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/
#define IDD_PRN              4001   /* print out dialogbox */
#define IDD_PRN_STOP         4002   /* print stop dialogbox */

#define IDD_PRN_COPY         4003   /* edit text to input No. of copies */
#define IDD_PRN_PRINT        4004   /* button for print out */
#define IDD_PRN_OPTION       4005   /* button for option */
#define IDD_PRN_SETUP        4006   /* button for setup */

#define IDD_PRN_P01          1010   /* check box for select P01 */
#define IDD_PRN_P02          1020   /* check box for select P02 */
#define IDD_PRN_P03          1030   /* check box for select P03 */
#define IDD_PRN_P03_FSC      1031   /* radio button for select FSC */
#define IDD_PRN_P03_PLUKEY   1032   /* radio button for select PLU key */
#define IDD_PRN_P03_BLANK    1033   /* radio button for select blank form */
#define IDD_PRN_P06          1060   /* check box for select P06 */
#define IDD_PRN_P09          1060   /* check box for select P09 */

#define IDD_PRN_A05          2050   /* radio button for select A05 */
#define IDD_PRN_A07          2070   /* radio button for select A07 */
#define IDD_PRN_A20          2200   /* radio button for select A20 */
#define IDD_PRN_A50          2500   /* radio button for select A50 */
#define IDD_PRN_A68          2680   /* radio button for select A68 */
#define IDD_PRN_A68_COND     2681   /* radio button for select condeced */
#define IDD_PRN_A68_DESC     2682   /* radio button for select description */
#define IDD_PRN_A68_RNG      2683   /* group box for PLU range */
#define IDD_PRN_A68_ALL      2684   /* radio button for all PLU record */
#define IDD_PRN_A68_NUM      2685   /* radio button for PLU No. */
#define IDD_PRN_A68_START    2686   /* edit text for select start record */
#define IDD_PRN_A68_END      2687   /* edit text for select end record */
#define IDD_PRN_A68_FROM     2688   /* static text start record */
#define IDD_PRN_A68_TO       2689   /* static text end record */
#define IDD_PRN_A114         2140   /* radio button for select A114 */

#define IDD_PRN_CAPTION1	4007

#define IDD_PRN_STOP_DSPMSG  3000   /* static text to display message */

/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
#define IDS_PRN                 20000

#define IDS_PRN_DSP_P03_FSC     (IDS_PRN + 0)
#define IDS_PRN_DSP_P03_PLU     (IDS_PRN + 1)
#define IDS_PRN_DSP_A68_COND    (IDS_PRN + 2)
#define IDS_PRN_DSP_A68_DESC    (IDS_PRN + 3)

#define IDS_PRN_P03_0           (IDS_PRN + 4)
#define IDS_PRN_P03_1           (IDS_PRN + 5)
#define IDS_PRN_P03_2           (IDS_PRN + 6)
#define IDS_PRN_P03_3           (IDS_PRN + 7)
#define IDS_PRN_P03_4           (IDS_PRN + 8)
#define IDS_PRN_P03_5           (IDS_PRN + 9)
#define IDS_PRN_P03_6           (IDS_PRN + 10)
#define IDS_PRN_P03_7           (IDS_PRN + 11)
#define IDS_PRN_P03_8           (IDS_PRN + 12)
#define IDS_PRN_P03_9           (IDS_PRN + 13)
#define IDS_PRN_P03_CLR         (IDS_PRN + 14)

#define IDS_PRN_P03_PLU_PAGE    (IDS_PRN + 15)
#define IDS_PRN_P03_FSC_PAGE    (IDS_PRN + 16)

#define IDS_PRN_A68_TITLE_UP    (IDS_PRN + 20)
#define IDS_PRN_A68_TITLE_MD    (IDS_PRN + 21)
#define IDS_PRN_A68_TITLE_DN    (IDS_PRN + 22)

#define IDS_PRN_A68_PLUNO       (IDS_PRN + 23)
#define IDS_PRN_A68_MNEMO       (IDS_PRN + 24)
#define IDS_PRN_A68_TYPE        (IDS_PRN + 25)
#define IDS_PRN_A68_DEPT        (IDS_PRN + 26)
#define IDS_PRN_A68_REPT        (IDS_PRN + 27)
#define IDS_PRN_A68_TBLNO       (IDS_PRN + 28)
#define IDS_PRN_A68_GRPNO       (IDS_PRN + 29)
#define IDS_PRN_A68_PRTPRY      (IDS_PRN + 30)
#define IDS_PRN_A68_PPI         (IDS_PRN + 31)
#define IDS_PRN_A68_PRICE       (IDS_PRN + 32)
#define IDS_PRN_A68_CONT        (IDS_PRN + 33)
#define IDS_PRN_A68_DSC_SIGN    (IDS_PRN + 34)
#define IDS_PRN_A68_DSC_PRINT   (IDS_PRN + 35)
#define IDS_PRN_A68_DSC_SI      (IDS_PRN + 36)
#define IDS_PRN_A68_DSC_MODKEY  (IDS_PRN + 37)
#define IDS_PRN_A68_DSC_MODCON  (IDS_PRN + 38)
#define IDS_PRN_A68_DSC_VALID   (IDS_PRN + 39)
#define IDS_PRN_A68_DSC_OPTREC  (IDS_PRN + 40)
#define IDS_PRN_A68_DSC_RECTP   (IDS_PRN + 41)
#define IDS_PRN_A68_DSC_TAX1    (IDS_PRN + 42)
#define IDS_PRN_A68_DSC_TAX2    (IDS_PRN + 43)
#define IDS_PRN_A68_DSC_TAX3    (IDS_PRN + 44)
#define IDS_PRN_A68_DSC_DISC1   (IDS_PRN + 45)
#define IDS_PRN_A68_DSC_DISC2   (IDS_PRN + 46)
#define IDS_PRN_A68_DSC_SCALE   (IDS_PRN + 47)
#define IDS_PRN_A68_DSC_COND    (IDS_PRN + 48)
#define IDS_PRN_A68_DSC_CONT    (IDS_PRN + 49)
#define IDS_PRN_A68_DSC_RMT1    (IDS_PRN + 50)
#define IDS_PRN_A68_DSC_RMT2    (IDS_PRN + 51)
#define IDS_PRN_A68_DSC_RMT3    (IDS_PRN + 52)
#define IDS_PRN_A68_DSC_RMT4    (IDS_PRN + 53)
#define IDS_PRN_A68_DSC_RMT5    (IDS_PRN + 54)
#define IDS_PRN_A68_DSC_RMT6    (IDS_PRN + 55)
#define IDS_PRN_A68_DSC_RMT7    (IDS_PRN + 56)
#define IDS_PRN_A68_DSC_RMT8    (IDS_PRN + 57)
#define IDS_PRN_A68_DSC_ADJ1    (IDS_PRN + 58)
#define IDS_PRN_A68_DSC_ADJ2    (IDS_PRN + 59)
#define IDS_PRN_A68_DSC_ADJ3    (IDS_PRN + 60)
#define IDS_PRN_A68_DSC_ADJ4    (IDS_PRN + 61)
#define IDS_PRN_A68_DSC_VARI1   (IDS_PRN + 62)
#define IDS_PRN_A68_DSC_VARI2   (IDS_PRN + 63)
#define IDS_PRN_A68_DSC_VARI3   (IDS_PRN + 64)
#define IDS_PRN_A68_DSC_VARI4   (IDS_PRN + 65)
#define IDS_PRN_A68_DSC_VARI5   (IDS_PRN + 66)
#define IDS_PRN_A68_DSC_BONUS   (IDS_PRN + 67)
#define IDS_PRN_A68_DSC_IMP     (IDS_PRN + 68)
#define IDS_PRN_A68_TYPE_1      (IDS_PRN + 69)
#define IDS_PRN_A68_TYPE_2      (IDS_PRN + 70)
#define IDS_PRN_A68_TYPE_3      (IDS_PRN + 71)
#define IDS_PRN_A68_0_SIGN      (IDS_PRN + 72)
#define IDS_PRN_A68_0_PRINT     (IDS_PRN + 73)
#define IDS_PRN_A68_0_SI        (IDS_PRN + 74)
#define IDS_PRN_A68_0_MODKEY    (IDS_PRN + 75)
#define IDS_PRN_A68_0_MODCON    (IDS_PRN + 76)
#define IDS_PRN_A68_0_VALID     (IDS_PRN + 77)
#define IDS_PRN_A68_0_OPTREC    (IDS_PRN + 78)
#define IDS_PRN_A68_0_RECTP     (IDS_PRN + 79)
#define IDS_PRN_A68_0_TAX1      (IDS_PRN + 80)
#define IDS_PRN_A68_0_TAX2      (IDS_PRN + 81)
#define IDS_PRN_A68_0_TAX3      (IDS_PRN + 82)
#define IDS_PRN_A68_0_DISC1     (IDS_PRN + 83)
#define IDS_PRN_A68_0_DISC2     (IDS_PRN + 84)
#define IDS_PRN_A68_0_SCALE     (IDS_PRN + 85)
#define IDS_PRN_A68_0_COND      (IDS_PRN + 86)
#define IDS_PRN_A68_0_CONT      (IDS_PRN + 87)
#define IDS_PRN_A68_0_RMT1      (IDS_PRN + 88)
#define IDS_PRN_A68_0_RMT2      (IDS_PRN + 89)
#define IDS_PRN_A68_0_RMT3      (IDS_PRN + 90)
#define IDS_PRN_A68_0_RMT4      (IDS_PRN + 91)
#define IDS_PRN_A68_0_RMT5      (IDS_PRN + 92)
#define IDS_PRN_A68_0_RMT6      (IDS_PRN + 93)
#define IDS_PRN_A68_0_RMT7      (IDS_PRN + 94)
#define IDS_PRN_A68_0_RMT8      (IDS_PRN + 95)
#define IDS_PRN_A68_0_ADJ1      (IDS_PRN + 96)
#define IDS_PRN_A68_0_ADJ2      (IDS_PRN + 97)
#define IDS_PRN_A68_0_ADJ3      (IDS_PRN + 98)
#define IDS_PRN_A68_0_ADJ4      (IDS_PRN + 99)
#define IDS_PRN_A68_0_VARI1     (IDS_PRN + 100)
#define IDS_PRN_A68_0_VARI2     (IDS_PRN + 101)
#define IDS_PRN_A68_0_VARI3     (IDS_PRN + 102)
#define IDS_PRN_A68_0_VARI4     (IDS_PRN + 103)
#define IDS_PRN_A68_0_VARI5     (IDS_PRN + 104)
#define IDS_PRN_A68_1_SIGN      (IDS_PRN + 105)
#define IDS_PRN_A68_1_PRINT     (IDS_PRN + 106)
#define IDS_PRN_A68_1_SI        (IDS_PRN + 107)
#define IDS_PRN_A68_1_MODKEY    (IDS_PRN + 108)
#define IDS_PRN_A68_1_MODCON    (IDS_PRN + 109)
#define IDS_PRN_A68_1_VALID     (IDS_PRN + 110)
#define IDS_PRN_A68_1_OPTREC    (IDS_PRN + 111)
#define IDS_PRN_A68_1_RECTP     (IDS_PRN + 112)
#define IDS_PRN_A68_1_TAX1      (IDS_PRN + 113)
#define IDS_PRN_A68_1_TAX2      (IDS_PRN + 114)
#define IDS_PRN_A68_1_TAX3      (IDS_PRN + 115)
#define IDS_PRN_A68_1_DISC1     (IDS_PRN + 116)
#define IDS_PRN_A68_1_DISC2     (IDS_PRN + 117)
#define IDS_PRN_A68_1_SCALE     (IDS_PRN + 118)
#define IDS_PRN_A68_1_COND      (IDS_PRN + 119)
#define IDS_PRN_A68_1_CONT      (IDS_PRN + 120)
#define IDS_PRN_A68_1_RMT1      (IDS_PRN + 121)
#define IDS_PRN_A68_1_RMT2      (IDS_PRN + 122)
#define IDS_PRN_A68_1_RMT3      (IDS_PRN + 123)
#define IDS_PRN_A68_1_RMT4      (IDS_PRN + 124)
#define IDS_PRN_A68_1_RMT5      (IDS_PRN + 125)
#define IDS_PRN_A68_1_RMT6      (IDS_PRN + 126)
#define IDS_PRN_A68_1_RMT7      (IDS_PRN + 127)
#define IDS_PRN_A68_1_RMT8      (IDS_PRN + 128)
#define IDS_PRN_A68_1_ADJ1      (IDS_PRN + 129)
#define IDS_PRN_A68_1_ADJ2      (IDS_PRN + 130)
#define IDS_PRN_A68_1_ADJ3      (IDS_PRN + 131)
#define IDS_PRN_A68_1_ADJ4      (IDS_PRN + 132)
#define IDS_PRN_A68_1_VARI1     (IDS_PRN + 133)
#define IDS_PRN_A68_1_VARI2     (IDS_PRN + 134)
#define IDS_PRN_A68_1_VARI3     (IDS_PRN + 135)
#define IDS_PRN_A68_1_VARI4     (IDS_PRN + 136)
#define IDS_PRN_A68_1_VARI5     (IDS_PRN + 137)
#define IDS_PRN_A68_0_IMP       (IDS_PRN + 138)
#define IDS_PRN_A68_1_IMP       (IDS_PRN + 139)
#define IDS_PRN_A68_2_IMP       (IDS_PRN + 140)
#define IDS_PRN_A68_3_IMP       (IDS_PRN + 141)
#define IDS_PRN_A68_4_IMP       (IDS_PRN + 142)
#define IDS_PRN_A68_5_IMP       (IDS_PRN + 143)
#define IDS_PRN_A68_ERR_NOPLU   (IDS_PRN + 144)
#define IDS_PRN_A68_ERR_PRINTER (IDS_PRN + 145)
#define IDS_PRN_A68_MB_LABEL    (IDS_PRN + 146)
#define IDS_PRN_A68_ERR_BADPLU  (IDS_PRN + 147)

/*
* ===========================================================================
*       Value definition
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
BOOL WINAPI  PepPrnDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI  PepPrnStopDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI  PepPrnAbortProc(HDC, int);

BOOL    PepPrn(HWND);
VOID    PepPrnSetup(HWND);

#endif

/* ===== End of PEPPRN.H ===== */
