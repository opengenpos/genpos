/*
* ---------------------------------------------------------------------------
* Title         :   Department Maintenance Header File (AC 114)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A114.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
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
/*
* ===========================================================================
*       Dialog ID 
* ===========================================================================
*/
/* ===== Control Code DialogBox ID Definition ===== */
#define IDD_A114_CTRL       11401
#define IDD_A114_CTRL_CAN   11402
#define IDD_A114_CTRL_VAT   11403

/* ===== Main DialogBox Control ID Definition ===== */

#define A114_SPIN_OFFSET    10
#define IDD_A114DEPT_LIST   11402

#define IDD_A114NO          11403
#define IDD_A114MAX         11404
#define IDD_A114HALO        11405
#define IDD_A114MAJOR       11406
#define IDD_A114BONUS       11407
#define IDD_A114MNEMO       11408
#define IDD_A114PRTPRY      11409
#define IDD_A114CTRL_LIST   11410
#define IDD_A114DEPT        11411
#define	IDD_A114TARE		11412

#define IDD_A114MAJOR_SPIN  (IDD_A114MAJOR + A114_SPIN_OFFSET)
#define IDD_A114HALO_SPIN   (IDD_A114HALO  + A114_SPIN_OFFSET)
#define IDD_A114BONUS_SPIN  (IDD_A114BONUS + A114_SPIN_OFFSET)
#define IDD_A114PRTPRY_SPIN (IDD_A114PRTPRY + A114_SPIN_OFFSET)

/*** NCR2172 ***/
#define IDD_A114DEPT_SPIN   (IDD_A114DEPT + A114_SPIN_OFFSET)

#define IDD_A114_MAINT			11423
#define IDD_A114_DEPT			11424
#define IDD_A114_HALO			11425
#define IDD_A114_MAJORDEPT		11426
#define IDD_A114_BONUSTTL		11427
#define IDD_A114_MNEM			11428
#define IDD_A114_20CHAR			11429
#define IDD_A114_PRTPRI			11430
#define IDD_A114_CTLCODES		11431
#define IDD_A114_AGETYPE		11432
#define	IDD_A114_CANTAX			11433
#define	IDD_A114_VAT			11434
#define	IDD_A114_DEPT_RNG		11435
#define	IDD_A114_HALO_RNG		11436
#define	IDD_A114_MAJOR_RNG		11437
#define	IDD_A114_BONUS_RNG		11438
#define IDD_A114_PRTPRY_RNG		11439
#define IDD_A114_DESC			11440

/* ===== Control Code DialogBox Control ID Definition ===== */
#define IDD_A114ST01    (IDD_A114PRTPRY_SPIN + 20)   /* Plus/Minus           *//* 11429*/
#define IDD_A114ST02    (IDD_A114ST01 +  1)         /* Normal/Special       */
#define IDD_A114ST03    (IDD_A114ST01 +  2)         /* Super. Intervention  */
#define IDD_A114ST04    (IDD_A114ST01 +  3)         /* Print Modifier Key   */
#define IDD_A114ST05    (IDD_A114ST01 +  4)         /* Prt.Mod. or Cond.Key */
#define IDD_A114ST06    (IDD_A114ST01 +  5)         /* Print Validation     *//* 11434 */
#define IDD_A114ST07    (IDD_A114ST01 +  6)         /* Issue Single/Double  */
#define IDD_A114ST08    (IDD_A114ST01 +  7)         /* Print Single/Double  */
#define IDD_A114ST09    (IDD_A114ST01 +  8)         /* Affect Taxable #1    */
#define IDD_A114ST10    (IDD_A114ST01 +  9)         /* Affect Taxable #2    */
#define IDD_A114ST11    (IDD_A114ST01 + 10)         /* Affect Taxable #3    */

#define IDD_A114ST29    (IDD_A114ST01 + 11)         /* FoodStamp            *//* ### ADD Saratoga (052300) */

#define IDD_A114ST12    (IDD_A114ST01 + 12)         /* Discount Item #1     *//* 11440 --> 11441 */
#define IDD_A114ST13    (IDD_A114ST01 + 13)         /* Discount Item #2     */
#define IDD_A114ST14    (IDD_A114ST01 + 14)         /* (Non) Scalable       *//* */
#define IDD_A114ST15    (IDD_A114ST01 + 15)         /* Condiment            */
#define IDD_A114ST16    (IDD_A114ST01 + 16)         /* Hash Dept.           */
#define IDD_A114ST17    (IDD_A114ST01 + 17)         /* Send Remote Prt. #1  */
#define IDD_A114ST18    (IDD_A114ST01 + 18)         /* Send Remote Prt. #2  */
#define IDD_A114ST19    (IDD_A114ST01 + 19)         /* Send Remote Prt. #3  */
#define IDD_A114ST20    (IDD_A114ST01 + 20)         /* Send Remote Prt. #4  */
#define IDD_A114ST21    (IDD_A114ST01 + 21)         /* Send Remote Prt. #5  */
#define IDD_A114ST22    (IDD_A114ST01 + 22)         /* Send Remote Prt. #6  */
#define IDD_A114ST23    (IDD_A114ST01 + 23)         /* Send Remote Prt. #7  */
#define IDD_A114ST24    (IDD_A114ST01 + 24)         /* Send Remote Prt. #8  */
#define IDD_A114ST25    (IDD_A114ST01 + 25)         /* Use Adjective Key #1 */
#define IDD_A114ST26    (IDD_A114ST01 + 26)         /* Use Adjective Key #2 */
#define IDD_A114ST27    (IDD_A114ST01 + 27)         /* Use Adjective Key #3 */
#define IDD_A114ST28    (IDD_A114ST01 + 28)         /* Use Adjective Key #4 */

#define IDD_A114ST30    (IDD_A114ST01 + 30)         /* WIC item             *//* ### ADD Saratoga (052900) */


/* ===== Control Code for Canadian Tax Mode ===== */
#define IDD_A114_CAN0   (IDD_A114ST28 + 10)     /* Meal Item            */
#define IDD_A114_CAN1   (IDD_A114_CAN0 + 1)     /* Carbonated Beverages */
#define IDD_A114_CAN2   (IDD_A114_CAN0 + 2)     /* Snack Item           */
#define IDD_A114_CAN3   (IDD_A114_CAN0 + 3)     /* Beer / Wine          */
#define IDD_A114_CAN4   (IDD_A114_CAN0 + 4)     /* Liquor               */
#define IDD_A114_CAN5   (IDD_A114_CAN0 + 5)     /* Grocery Item         */
#define IDD_A114_CAN6   (IDD_A114_CAN0 + 6)     /* Tips Item            */
#define IDD_A114_CAN7   (IDD_A114_CAN0 + 7)     /* PST1 only Item       */
#define IDD_A114_CAN8   (IDD_A114_CAN0 + 8)     /* Backed Item          */
#define IDD_A114_CAN9	(IDD_A114_CAN0 + 9)

/* ===== Control Code for International VAT Mode ===== */
#define IDD_A114_VAT0       (IDD_A114_CAN9 + 10)    /* no VAT able          */
#define IDD_A114_VAT1       (IDD_A114_VAT0 + 1)     /* VAT able No.1        */
#define IDD_A114_VAT2       (IDD_A114_VAT0 + 2)     /* VAT able No.2        */
#define IDD_A114_VAT3       (IDD_A114_VAT0 + 3)     /* VAT able No.3        */
#define IDD_A114_VATSERVICE (IDD_A114_VAT0 + 4)     /* Serviceable          */

/* Saratoga */
#define IDD_A114_MAX        (IDD_A114_VATSERVICE + 1)
#define IDD_A114_CUR        (IDD_A114_VATSERVICE + 2)

#define IDD_A114_DEL        (IDD_A114_CUR + 1)
#define IDD_A114_TYPEKEY    (IDD_A114_CUR + 2)


#define IDD_A114_TARE_SPIN		(IDD_A114TARE + A114_SPIN_OFFSET)
#define	IDD_A114_STRTARE		(IDD_A114_CUR + 3)
#define	IDD_A114_STRTARELEN		(IDD_A114_CUR + 5)
#define	IDD_A114_STSTARECCHECK	(IDD_A114_CUR + 6)

/*
* ===========================================================================
*       String ID Definitions
* ===========================================================================
*/
#define IDS_A114CTRL001 IDS_A114
#define IDS_A114CTRL002 (IDS_A114 +  1)
#define IDS_A114CTRL003 (IDS_A114 +  2)
#define IDS_A114CTRL004 (IDS_A114 +  3)
#define IDS_A114CTRL005 (IDS_A114 +  4)
#define IDS_A114CTRL006 (IDS_A114 +  5)
#define IDS_A114CTRL007 (IDS_A114 +  6)
#define IDS_A114CTRL008 (IDS_A114 +  7)
#define IDS_A114CTRL009 (IDS_A114 +  8)
#define IDS_A114CTRL010 (IDS_A114 +  9)
#define IDS_A114CTRL011 (IDS_A114 + 10)
#define IDS_A114CTRL029 (IDS_A114 + 11)
#define IDS_A114CTRL012 (IDS_A114 + 12)
#define IDS_A114CTRL013 (IDS_A114 + 13)
#define IDS_A114CTRL014 (IDS_A114 + 14)
#define IDS_A114CTRL015 (IDS_A114 + 15)
#define IDS_A114CTRL016 (IDS_A114 + 16)
#define IDS_A114CTRL017 (IDS_A114 + 17)
#define IDS_A114CTRL018 (IDS_A114 + 18)
#define IDS_A114CTRL019 (IDS_A114 + 19)
#define IDS_A114CTRL020 (IDS_A114 + 20)
#define IDS_A114CTRL021 (IDS_A114 + 21)
#define IDS_A114CTRL022 (IDS_A114 + 22)
#define IDS_A114CTRL023 (IDS_A114 + 23)
#define IDS_A114CTRL024 (IDS_A114 + 24)
#define IDS_A114CTRL025 (IDS_A114 + 25)
#define IDS_A114CTRL026 (IDS_A114 + 26)
#define IDS_A114CTRL027 (IDS_A114 + 27)
#define IDS_A114CTRL028 (IDS_A114 + 28)

#define IDS_A114CTRL101 (IDS_A114 + 29)
#define IDS_A114CTRL102 (IDS_A114 + 30)
#define IDS_A114CTRL103 (IDS_A114 + 31)
#define IDS_A114CTRL104 (IDS_A114 + 32)
#define IDS_A114CTRL105 (IDS_A114 + 33)
#define IDS_A114CTRL106 (IDS_A114 + 34)
#define IDS_A114CTRL107 (IDS_A114 + 35)
#define IDS_A114CTRL108 (IDS_A114 + 36)
#define IDS_A114CTRL109 (IDS_A114 + 37)
#define IDS_A114CTRL110 (IDS_A114 + 38)
#define IDS_A114CTRL111 (IDS_A114 + 39)
#define IDS_A114CTRL129 (IDS_A114 + 40)		/* ### ADD Saratoga (052300) */
#define IDS_A114CTRL112 (IDS_A114 + 41)
#define IDS_A114CTRL113 (IDS_A114 + 42)
#define IDS_A114CTRL114 (IDS_A114 + 43)
#define IDS_A114CTRL115 (IDS_A114 + 44)
#define IDS_A114CTRL116 (IDS_A114 + 45)
#define IDS_A114CTRL117 (IDS_A114 + 46)
#define IDS_A114CTRL118 (IDS_A114 + 47)
#define IDS_A114CTRL119 (IDS_A114 + 48)
#define IDS_A114CTRL120 (IDS_A114 + 49)
#define IDS_A114CTRL121 (IDS_A114 + 50)
#define IDS_A114CTRL122 (IDS_A114 + 51)
#define IDS_A114CTRL123 (IDS_A114 + 52)
#define IDS_A114CTRL124 (IDS_A114 + 53)
#define IDS_A114CTRL125 (IDS_A114 + 54)
#define IDS_A114CTRL126 (IDS_A114 + 55)
#define IDS_A114CTRL127 (IDS_A114 + 56)
#define IDS_A114CTRL128 (IDS_A114 + 57)

#define IDS_A114CAN_0   (IDS_A114 + 58)
#define IDS_A114CAN_1   (IDS_A114 + 59)
#define IDS_A114CAN_2   (IDS_A114 + 60)
#define IDS_A114CAN_3   (IDS_A114 + 61)
#define IDS_A114CAN_4   (IDS_A114 + 62)
#define IDS_A114CAN_5   (IDS_A114 + 63)
#define IDS_A114CAN_6   (IDS_A114 + 64)
#define IDS_A114CAN_7   (IDS_A114 + 65)
#define IDS_A114CAN_8   (IDS_A114 + 66)

#define IDS_A114_EMPTY  (IDS_A114 + 67)
#define IDS_A114_TITLE  (IDS_A114 + 68)
#define IDS_A114_NOREC  (IDS_A114 + 69)
#define IDS_A114_NONE   (IDS_A114 + 70)

#define IDS_A114_VAT0       (IDS_A114 + 71)
#define IDS_A114_VAT1       (IDS_A114 + 72)
#define IDS_A114_VAT2       (IDS_A114 + 73)
#define IDS_A114_VAT3       (IDS_A114 + 74)
#define IDS_A114_VATSERVICE (IDS_A114 + 75)
#define IDS_A114_VATSERON   (IDS_A114 + 76)
#define IDS_A114_VATSEROFF  (IDS_A114 + 77)

/* Saratoga */
#define IDS_A114_MAX         (IDS_A114 + 78)
#define IDS_A114_CUR         (IDS_A114 + 79)
#define IDS_A114_TYPE        (IDS_A114 + 80)
#define IDS_A114_TYPE1       (IDS_A114 + 81)
#define IDS_A114_TYPE2       (IDS_A114 + 82)
#define IDS_A114_TYPE3       (IDS_A114 + 83)

#define IDS_A114_OVERREC     (IDS_A114 + 84)

/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A114_CTRLCODE_NO   29   /* No. of Control Code Description          *//* ### MOD Saratoga (28 --> 29)(052300) */
                                /* --> added Food Stamp */
#define A114_CANADA_MAX     9   /* Max No. of Canadian Tax                  */

#define A114_HALOMAX       79   /* Maximum Value of HALO                    */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define A114_MDEPTMAX      10   /* Maximum Value of Major Department No.    */
#endif
#define A114_MDEPTMAX      30   /* Maximum Value of Major Department No.    */

#define A114_BONUSMAX      STD_NO_BONUS_TTL  /* Maximum Value of Bonus Total Index       */
#define A114_PRTPRYMAX     15   /* Maximum Value of Print Priorty           */
#define A114_DATAMIN        0   /* Minimum Value of HALO / Bonus Total      */
#define A114_MDEPTMIN       1   /* Minimum Value of Mejor Dept.             */

/*** NCR2172 ***/
#define A114_DEPTMIN        1   /* Minimum Value of Dept No.                */
#define A114_DEPTMAX     9999   /* Maximum Value of Dept No.                */
#define A114_DEPTLEN        4   /* Maximum Length of Dept No.               */

#define A114_TAREMIN        0   /* Minimum Value of Tare No.                */
#define A114_TAREMAX       30   /* Maximum Value of Tare No.                */

#define A114_HALOLEN        2   /* Maximum Length of HALO                   */
#define A114_MDEPTLEN       2   /* Maximum Length of Major Department No.   */
#define A114_BONUSLEN       3   /* Maximum Length of Bonus Total Index      */
#define A114_PRTPRYLEN      2   /* Maximum Length of Print Priorty          */
#define A114_TARELEN        2   /* Maximum Length of Tare table offset      */

#define A114_TAX_US         0   /* Current TAX Mode of US                   */
#define A114_TAX_CAN        1   /* Current TAX Mode of Canadian             */
#define A114_TAX_VAT        2   /* Current TAX Mode of VAT                  */


#define A114_TAX_BIT     0x0f   /* Canadian TAX Write Address (Bit 0,1,2,3) */
#define A114_TAX_BIT_VAT 0x03   /* International VAT Address (Bit 0,1)      */
#define A114_TAX_BIT_SERVICE 0x08 /* International VAT service               */
#define A114_TAX_MAX     0x09   /* Max. Value of Canadian Tax               */
#define A114_TAX_MAX_VAT 0x03   /* Max. Value of International VAT          */
#define A114_TAX_MASK    0x30   /* Not Used Bit (Address 4 & 5)             */
#define A114_DISC_ITEM1  0x40   /* Address 5 Bit 6 (Discountable Item #1)   */
#define A114_DISC_ITEM2  0x80   /* Address 5 Bit 7 (Discountable Item #2)   */

#define A114_LBSET          0   /* set listbox when set button pushed       */
#define A114_LBENTER        1   /* set listbox when enter button pushed     */

#define A114_CTRLCODE1      0   /* control code address 2 & 3               */
#define A114_CTRLCODE2      1   /* control code address 4 & 5               */
#define A114_CTRLCODE3      2   /* control code address 6 & 7               */
#define A114_CTRLCODE4      3   /* control code address 8 & 9               */
#define A114_CTRLCODE5      4   /* control code address 13 & 14             */
#define A114_BONUS_ADDR     5   /* Bonus Total Index Address                */
#define A114_HALO           0   /* HALO address                             */

#define A114_BITMASK     0x01   /* for bit mask                             */
#define A114_CHKBIT1     0x0f   /* bit check table for address 1 (M.Dept #) */
#define A114_CHKBIT2     0xff   /* bit check table for address 2 & 3        */

/* ### MAKING #define A114_CHKBIT3     0xc7   /* bit check table for address 4 & 5        */
#define A114_CHKBIT3     0xcF   /* bit check table for address 4 & 5        */

#define A114_CHKBIT4     0xf7   /* bit check table for address 6 & 7        */
#define A114_CHKBIT5     0x0f   /* bit check table for address 8 (Bonus)    */
#define A114_CHKBIT6     0xf0   /* bit check table for address 9            */
#define A114_CHKBIT7     0x0f   /* bit check table for address 13           */
/*#define A114_CHKBIT7     0x0f */  /* bit check table for address 13           */
#define A114_8BITS          8   /* bits for 1 byte                          */

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define A114_CHKBIT      0xff   /* Saratoga */
#endif
#define A114_CHKBIT      0x0f   /* Saratoga */

#define A114_TYPEBIT      0x0f   /* Saratoga */

/* ### ADD Saratgoa (052900) */
#define A114_AGETYPE_BIT    (0x30)    /* Address 14 00110000B */
#define A114_WIC_BIT        (0x40)    /* Address 14 01000000B */


#define A114_DSCRB1         0   /* description offset for address 2 & 3     */
#define A114_DSCRB2         8   /* description offset for address 4 & 5     */
#define A114_DSCRB3        13   /* description offset for address 6 & 7     */
#define A114_DSCRB4        20   /* description offset for address 13         */
#define A114_DSCRB5        24   /* description offset for address 9         */
#define A114_DSCITEM1      11   /* description offset for address 5 bit-6   */
#define A114_DSCITEM2      12   /* description offset for address 5 bit-7   */

#define A114_DESC_LEN      64   /* Max. Length of Description   */

#define A114_SPIN_STEP      1
#define A114_SPIN_TURBO     0

#define A114_MNEMO_SIZE    20   /* saratoga */
/* #define A114_MNEMO_SIZE    12 */
/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/

/* typedef DEPTPARA FAR*   LPDEPTPARA;   *//* NCR2172 */
typedef OPDEPT_PARAENTRY  * LPDEPTPARA;  /* NCR2172 */

typedef struct {
    WCHAR    szDescOFF[A114_DESC_LEN];
    WCHAR    szDescON[A114_DESC_LEN];
} A114DESC,  *LPA114DESC;

typedef struct {
    WCHAR    szCanDesc[A114_DESC_LEN];
} A114CANADA,  *LPA114CANADA;

/*
* ===========================================================================
*       Macro Definition
* ===========================================================================
*/
/*
#define A114_GETBONUS(lpData) (UINT)(((LPDEPTPARA)lpData)->auchControlCode[A114_BONUS_ADDR] & 0x0f)
*/
//#define A114_GETBONUS(tempDept) (UINT)(((DEPTIF)tempDept).ParaDept.auchControlCode[A114_BONUS_ADDR] & 0x0f)
#define A114_GETBONUS(tempDept) (UINT)((tempDept).ParaDept.auchControlCode[A114_BONUS_ADDR])
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
BOOL    A114ReadMax(HWND, LPUINT);
BOOL    A114AllocMem(HWND, LPHGLOBAL, LPHGLOBAL);
VOID    A114InitDlg(HWND);
VOID    A114FreeMem(LPHGLOBAL, LPHGLOBAL);
VOID    A114InitCtrlDesc(VOID);
VOID    A114ReadData(UINT);
VOID    A114InitList(HWND, UINT);
VOID    A114SetData(HWND);
VOID    A114SetCtrlList(HWND, BOOL);
VOID    A114SetListContent(HWND, BYTE, BYTE, WORD, BOOL);
VOID    A114SetCanadaDesc(HWND, BOOL);
VOID    A114SetVATDesc(HWND, BOOL);

VOID    A114WriteData(UINT);
VOID    A114ChkRng(HWND, WORD wEditID, UINT);
VOID    A114GetData(HWND);
VOID    A114SetCtrlData(HWND);
VOID    A114SetChkbox(HWND, WPARAM, BYTE, BYTE);
VOID    A114SetCanadaBtn(HWND);
VOID    A114SetVATBtn(HWND);
VOID    A114GetCtrlData(HWND);
VOID    A114GetChkbox(HWND, WPARAM, BYTE, BYTE);
VOID    A114GetCanadaBtn(HWND);
VOID    A114GetVATBtn(HWND);

UINT    A114ReadRec(VOID);
VOID    A114CtrlButton(HWND hDlg);
VOID    A114FinDlg(HWND hDlg, WPARAM wParam);
VOID    A114SaveRec(HWND hDlg, WPARAM wDlgID, DEPTIF *pDpi);
BOOL    A114RegistData(HWND hDlg, DEPTIF *pDpi);
BOOL    A114ChkMaxRec(VOID);
VOID    A114RefreshList(HWND hDlg, WPARAM wParam);
BOOL    A114DeleteData(VOID);
HWND	A114ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A114ModeLessDialogWnd (void);

/* ===== End of File (A114.H) ===== */
