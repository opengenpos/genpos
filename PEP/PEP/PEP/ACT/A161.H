/*
* ---------------------------------------------------------------------------
* Title         :   Coupon Maintenance (AC 161)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A161.H
* Copyright (C) :   1995. NCR Corp. E&M-OISO, All rights reserved.
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
* Mar-16-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
* Aug-09-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Apl-17-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#define	IDD_A161_AUTO		16100
#define IDD_A161_CTRL       16101
#define IDD_A161_CTRL_CAN   16102
#define IDD_A161_CTRL_VAT   16103

/* ===== Main DialogBox Control ID Definition ===== */
#define A161_TARGET_OFFSET      7
#define A161_SPIN_OFFSET        55

#define IDD_A161CPN_LIST    16103                              /* Combination Coupon List       */
#define IDD_A161NO          16104                              /* Combination Coupon Number     */
#define IDD_A161MAX         16105                              /* Flexible Menory Max of Recode */
#define IDD_A161MNEMO       16106                              /* Mnemonic                      */
#define IDD_A161AMOUNT      16107                              /* Coupon Amouont                */
#define IDD_A161_AUTO_CPN_LIST 16108						   /* Auto Coupon List Box			*/
#define IDD_A161CTRL_LIST   16109
#define IDD_A161TAX01       (IDD_A161CTRL_LIST+1)              /* Address 1 Bit 0 Taxable #1    */
#define IDD_A161TAX02       (IDD_A161TAX01+1)                  /* Address 1 Bit 1 Taxable #2    */
#define IDD_A161TAX03       (IDD_A161TAX01+2)                  /* Address 1 Bit 2 Taxable #3    */
#define IDD_A161FS          (IDD_A161TAX01+3)                  /* Food Stamp                    *//* ### ADD Saratoga (052400) */
#define IDD_A161SINGLE      (IDD_A161TAX01+4)                  /* Address 2 Bit 1 Single Type, R3.0 */
#define IDD_A161DIS01       (IDD_A161TAX01+5)                  /* Address 2 Bit 2 Disc. Item #1 */
#define IDD_A161DIS02       (IDD_A161TAX01+6)				   /* Address 2 Bit 3 Disc. Item #2 */

#define IDD_A161TAG01       (IDD_A161DIS02+10)                 /* Coupon Target Dept/Plu #1     */
#define IDD_A161TAG02       (IDD_A161TAG01+1)                  /* Coupon Target Dept/Plu #2     */
#define IDD_A161TAG03       (IDD_A161TAG01+2)                  /* Coupon Target Dept/Plu #3     */
#define IDD_A161TAG04       (IDD_A161TAG01+3)                  /* Coupon Target Dept/Plu #4     */
#define IDD_A161TAG05       (IDD_A161TAG01+4)                  /* Coupon Target Dept/Plu #5     */
#define IDD_A161TAG06       (IDD_A161TAG01+5)                  /* Coupon Target Dept/Plu #6     */
#define IDD_A161TAG07       (IDD_A161TAG01+6)                  /* Coupon Target Dept/Plu #7     */

#define IDD_A161DEP01       (IDD_A161TAG01+A161_TARGET_OFFSET) /* Address 3 Bit 0 Taget #1 Dept */
#define IDD_A161DEP02       (IDD_A161DEP01+1)                  /* Address 3 Bit 1 Taget #2 Dept */
#define IDD_A161DEP03       (IDD_A161DEP01+2)                  /* Address 3 Bit 2 Taget #3 Dept */
#define IDD_A161DEP04       (IDD_A161DEP01+3)                  /* Address 3 Bit 3 Taget #4 Dept */
#define IDD_A161DEP05       (IDD_A161DEP01+4)                  /* Address 4 Bit 0 Taget #5 Dept */
#define IDD_A161DEP06       (IDD_A161DEP01+5)                  /* Address 4 Bit 1 Taget #6 Dept */
#define IDD_A161DEP07       (IDD_A161DEP01+6)                  /* Address 4 Bit 2 Taget #7 Dept */

#define IDD_A161ADJ01       (IDD_A161DEP01+A161_TARGET_OFFSET) /* Taget Adjective Code #1       */
#define IDD_A161ADJ02       (IDD_A161ADJ01+1)                  /* Taget Adjective Code #2       */
#define IDD_A161ADJ03       (IDD_A161ADJ01+2)                  /* Taget Adjective Code #3       */
#define IDD_A161ADJ04       (IDD_A161ADJ01+3)                  /* Taget Adjective Code #4       */
#define IDD_A161ADJ05       (IDD_A161ADJ01+4)                  /* Taget Adjective Code #5       */
#define IDD_A161ADJ06       (IDD_A161ADJ01+5)                  /* Taget Adjective Code #6       */
#define IDD_A161ADJ07       (IDD_A161ADJ01+6)                  /* Taget Adjective Code #7       */

#define IDD_A161GRP1        (IDD_A161ADJ01+A161_TARGET_OFFSET) /* Taget Group Code #1, R3.0 */
#define IDD_A161GRP2        (IDD_A161GRP1+1)                   /* Taget Group Code #2, R3.0 */
#define IDD_A161GRP3        (IDD_A161GRP1+2)                   /* Taget Group Code #3, R3.0 */
#define IDD_A161GRP4        (IDD_A161GRP1+3)                   /* Taget Group Code #4, R3.0 */
#define IDD_A161GRP5        (IDD_A161GRP1+4)                   /* Taget Group Code #5, R3.0 */
#define IDD_A161GRP6        (IDD_A161GRP1+5)                   /* Taget Group Code #6, R3.0 */
#define IDD_A161GRP7        (IDD_A161GRP1+6)                   /* Taget Group Code #7, R3.0 */

#define IDD_A161AMOUNT_SPIN (IDD_A161AMOUNT+A161_SPIN_OFFSET)

#define IDD_A161TAG01_SPIN  (IDD_A161TAG01+A161_SPIN_OFFSET)
#define IDD_A161TAG02_SPIN  (IDD_A161TAG02+A161_SPIN_OFFSET)
#define IDD_A161TAG03_SPIN  (IDD_A161TAG03+A161_SPIN_OFFSET)
#define IDD_A161TAG04_SPIN  (IDD_A161TAG04+A161_SPIN_OFFSET)
#define IDD_A161TAG05_SPIN  (IDD_A161TAG05+A161_SPIN_OFFSET)
#define IDD_A161TAG06_SPIN  (IDD_A161TAG06+A161_SPIN_OFFSET)
#define IDD_A161TAG07_SPIN  (IDD_A161TAG07+A161_SPIN_OFFSET)

#define IDD_A161ADJ01_SPIN  (IDD_A161ADJ01+A161_SPIN_OFFSET)
#define IDD_A161ADJ02_SPIN  (IDD_A161ADJ02+A161_SPIN_OFFSET)
#define IDD_A161ADJ03_SPIN  (IDD_A161ADJ03+A161_SPIN_OFFSET)
#define IDD_A161ADJ04_SPIN  (IDD_A161ADJ04+A161_SPIN_OFFSET)
#define IDD_A161ADJ05_SPIN  (IDD_A161ADJ05+A161_SPIN_OFFSET)
#define IDD_A161ADJ06_SPIN  (IDD_A161ADJ06+A161_SPIN_OFFSET)
#define IDD_A161ADJ07_SPIN  (IDD_A161ADJ07+A161_SPIN_OFFSET)

#define IDD_A161GRP1_SPIN   (IDD_A161GRP1+A161_SPIN_OFFSET)
#define IDD_A161GRP2_SPIN   (IDD_A161GRP2+A161_SPIN_OFFSET)
#define IDD_A161GRP3_SPIN   (IDD_A161GRP3+A161_SPIN_OFFSET)
#define IDD_A161GRP4_SPIN   (IDD_A161GRP4+A161_SPIN_OFFSET)
#define IDD_A161GRP5_SPIN   (IDD_A161GRP5+A161_SPIN_OFFSET)
#define IDD_A161GRP6_SPIN   (IDD_A161GRP6+A161_SPIN_OFFSET)
#define IDD_A161GRP7_SPIN   (IDD_A161GRP7+A161_SPIN_OFFSET)

/* ===== Control Code for Canadian Tax Mode ===== */
#define IDD_A161_CAN0   (IDD_A161GRP7_SPIN + 1) /* Meal Item            */
#define IDD_A161_CAN1   (IDD_A161_CAN0 + 1)     /* Carbonated Beverages */
#define IDD_A161_CAN2   (IDD_A161_CAN0 + 2)     /* Snack Item           */
#define IDD_A161_CAN3   (IDD_A161_CAN0 + 3)     /* Beer / Wine          */
#define IDD_A161_CAN4   (IDD_A161_CAN0 + 4)     /* Liquor               */
#define IDD_A161_CAN5   (IDD_A161_CAN0 + 5)     /* Grocery Item         */
#define IDD_A161_CAN6   (IDD_A161_CAN0 + 6)     /* Tips Item            */
#define IDD_A161_CAN7   (IDD_A161_CAN0 + 7)     /* PST1 only Item       */
#define IDD_A161_CAN8   (IDD_A161_CAN0 + 8)     /* Backed Item          */
#define IDD_A161_CAN9	(IDD_A161_CAN0 + 9)		/* Baked Item CT/OT Special*/

/* ===== Control Code for International VAT Mode, V3.3 ===== */
#define IDD_A161_VAT0   (IDD_A161_CAN9 + 1)
#define IDD_A161_VAT1   (IDD_A161_VAT0 + 1)
#define IDD_A161_VAT2   (IDD_A161_VAT0 + 2)
#define IDD_A161_VAT3   (IDD_A161_VAT0 + 3)
#define IDD_A161_VATSERVICE (IDD_A161_VAT0 + 4)

/* Saratoga */

#define IDD_A161_VER1   (IDD_A161_VAT0 + 5)
#define IDD_A161_VER2   (IDD_A161_VAT0 + 6)
#define IDD_A161_VER3   (IDD_A161_VAT0 + 7)
#define IDD_A161_VER4   (IDD_A161_VAT0 + 8)
#define IDD_A161_VER5   (IDD_A161_VAT0 + 9)
#define IDD_A161_VER6   (IDD_A161_VAT0 + 10)
#define IDD_A161_VER7   (IDD_A161_VAT0 + 11)

/* Saratoga End */

#define IDD_A161_CAPTION1	16300
#define IDD_A161_CAPTION2	16301
#define IDD_A161_CAPTION3	16302
#define IDD_A161_CAPTION4	16303
#define IDD_A161_CAPTION5	16304
#define IDD_A161_CAPTION6	16305
#define IDD_A161_CAPTION7	16306
#define IDD_A161_CAPTION8	16307
#define IDD_A161_CAPTION9	16308
#define IDD_A161_CAPTION10	16309
#define IDD_A161_CAPTION11	16310
#define IDD_A161_CAPTION12	16311
#define IDD_A161_CAPTION13	16312
#define IDD_A161_CAPTION14	16313
#define IDD_A161_CAPTION15	16314
#define IDD_A161_CAPTION16	16315
#define IDD_A161_CAPTION17	16316
#define IDD_A161_CAPTION18	16317
#define IDD_A161_CAPTION19	16318
#define IDD_A161_CAPTION20	16319
#define IDD_A161_CAPTION21	16320
#define IDD_A161_CAPTION22	16321
#define IDD_A161_CAPTION23	16322
#define IDD_A161_CAPTION24	16323
#define IDD_A161_CAPTION25	16324
#define IDD_A161_CAPTION26	16325
#define IDD_A161_CAPTION27	16326
#define	IDD_A161_CAPTION28  16327
#define	IDD_A161_CAPTION29	16351
#define	IDD_A161_CAPTION30	16352
#define IDD_A161_GROUPBOX1	16328
#define IDD_A161_GROUPBOX2	16329
#define IDD_A161_GROUPBOX3	16330

#define IDD_A161_ADDR1_ADJ_RNG	16331
#define IDD_A161_ADDR1_GRP_RNG	16332
#define IDD_A161_ADDR2_ADJ_RNG	16333
#define IDD_A161_ADDR2_GRP_RNG	16334
#define IDD_A161_ADDR3_ADJ_RNG	16335
#define IDD_A161_ADDR3_GRP_RNG	16336
#define IDD_A161_ADDR4_ADJ_RNG	16337
#define IDD_A161_ADDR4_GRP_RNG	16338
#define IDD_A161_ADDR5_ADJ_RNG	16339
#define IDD_A161_ADDR5_GRP_RNG	16340
#define IDD_A161_ADDR6_ADJ_RNG	16341
#define IDD_A161_ADDR6_GRP_RNG	16342
#define IDD_A161_ADDR7_ADJ_RNG	16343
#define IDD_A161_ADDR7_GRP_RNG	16344
#define IDD_A161_PRICE_RNG		16345
#define	IDD_A161ACCFLG			16346
#define	IDD_A161AUTOMAX			16347
#define	IDD_A161EVALENABLED		16348
#define	IDD_A161CLOSE			16448
#define	IDD_AUTO				16451
#define	IDD_AUTO_PRI			16452
#define IDD_AUTO_PRI_MNEMO		16453
#define IDD_AUTO_PRI_MNEMO2		16454
#define	IDD_AUTO_UPSWITCH		16455
#define	IDD_AUTO_DOWNSWITCH		16456
#define IDD_A161_AUTO_PRI		16457
#define IDD_AUTO_CHANGE			16458
#define IDD_A161_BONUS_CPN      16459
#define IDD_A161BONUS			16460
#define IDD_DELETE				16461
#define IDD_A161_SALESCODE_CPN  16462
#define IDD_A161SALESCODE		16463
#define IDD_A161SUPRINTV_FLAG   16464

/*
* ===========================================================================
*       String ID Definitions
* ===========================================================================
*/

#define IDS_A161_EMPTY  (IDS_A161 + 0)
#define IDS_A161_TITLE  (IDS_A161 + 1)
#define IDS_A161_NOREC  (IDS_A161 + 2)
#define IDS_A161_NONE   (IDS_A161 + 3)

#define IDS_A161CTRL001 (IDS_A161 +  4)
#define IDS_A161CTRL002 (IDS_A161 +  5)
#define IDS_A161CTRL003 (IDS_A161 +  6)
#define IDS_A161CTRL007 (IDS_A161 +  7)     /* ### ADD Saratoga FoodStamp (052400) */
#define IDS_A161CTRL004 (IDS_A161 +  8)
#define IDS_A161CTRL005 (IDS_A161 +  9)
#define IDS_A161CTRL006 (IDS_A161 +  10)
#define IDS_A161CTRL008 (IDS_A161 +  11)



#define IDS_A161CTRL101 (IDS_A161 + 12)
#define IDS_A161CTRL102 (IDS_A161 + 13)
#define IDS_A161CTRL103 (IDS_A161 + 14)		/* ### ADD Saratoga FoodStamp (052400) */
#define IDS_A161CTRL107 (IDS_A161 + 15)
#define IDS_A161CTRL104 (IDS_A161 + 16)
#define IDS_A161CTRL105 (IDS_A161 + 17)
#define IDS_A161CTRL106 (IDS_A161 + 18)
#define	IDS_A161CTRL108 (IDS_A161 + 19)


#define IDS_A161CAN_0   (IDS_A161 + 20)
#define IDS_A161CAN_1   (IDS_A161 + 21)
#define IDS_A161CAN_2   (IDS_A161 + 22)
#define IDS_A161CAN_3   (IDS_A161 + 23)
#define IDS_A161CAN_4   (IDS_A161 + 24)
#define IDS_A161CAN_5   (IDS_A161 + 25)
#define IDS_A161CAN_6   (IDS_A161 + 26)
#define IDS_A161CAN_7   (IDS_A161 + 27)
#define IDS_A161CAN_8   (IDS_A161 + 28)
#define IDS_A161CAN_9	(IDS_A161 + 29)

#define IDS_A161VAT_0       (IDS_A161 + 30)
#define IDS_A161VAT_1       (IDS_A161 + 31)
#define IDS_A161VAT_2       (IDS_A161 + 32)
#define IDS_A161VAT_3       (IDS_A161 + 33)
#define IDS_A161VATSERVICE  (IDS_A161 + 34)
#define IDS_A161_VATSERON   (IDS_A161 + 35)
#define IDS_A161_VATSEROFF  (IDS_A161 + 36)

#define IDS_A161_OVERRANGE  (IDS_A161 + 37)
#define IDS_A161_ERR_PLUNO  (IDS_A161 + 38)
#define IDS_A161_ERR_LABEL  (IDS_A161 + 39)
#define IDS_A161_NOAUTO		(IDS_A161 + 40)
#define IDS_A161_AUTOLIMIT	(IDS_A161 + 41)


/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A161_CTRLCODE_NO    7   /* No. of Control Code Description          *//* ### MOD Saratoga (6 ---> 7)(052400) */
#define A161_CANADA_MAX     9   /* Max No. of Canadian Tax                  */

#define A161_CPNMAX       100   /* Maximum Value of Coupon                  */
#define A161_AMOUNTMAX 999999L  /* Maximum Value of Amount                  */

/*#define A161_TAGPLUMAX   9999    Maximun Value of Coupon Taget PLU        */
/*#define A161_TAGDEPTMAX    50    Maximun Value of Coupon Taget DEPT       */
#define A161_TAGPLUMAX  WIDE("9999999999999")   /* Maximun Value of Coupon Taget PLU        */
#define	A161_TAGPLUMIN	WIDE("0000000000000")
#define A161_TAGDEPTMAX		WIDE("9999")   /* Maximun Value of Coupon Taget DEPT       */
#define A161_TAGDEPTMIN		WIDE("0000")

#define A161_ADJMAX         5   /* Maximun Value of Adjective Code          */
#define A161_GRPMAX         2   /* Maximun Value of Group Code,        R3.0 */
#define A161_DATAMIN        0   /* Minimum Value of HALO / Bonus Total      */
#define A161_AUTOPRIMAX		50  /* Maximum Number of Auto Coupons			*/
#define A161_CPNMIN         1   /* Minimum Value of Coupon                  */
#define A161_AMOUNTLEN      6   /* Maximum Length of Amount                 */
#define A161_BNSMAX			3   /* maximum Bonus Index */
#define A161_BNSMAXVALUE	100 /* maximum value of bonus*/

/*#define A161_TAGETLEN       4 */  /* Saratoga         */
#define A161_TAGETLEN       STD_PLU_NUMBER_LEN   /* Maximum Length of TAGET PLU/DEPT         */
#define A161_DEPTLEN        4   /* Maximum Length of TAGET PLU/DEPT         */
#define A161_ADJLEN         1   /* Maximum Length of Adjective Code         */
#define A161_GRPLEN         1   /* Maximum Length of Group Code,       R3.0 */
#define A161_PRIORITYMIN	1	/* Minimum Coupon Priority - CSMALL	(SR 1005)		*/
#define A161_PRIORITYMAX	25	/* Maximum Coupon Priority - CSMALL	(SR 1005)*/

#define A161_BITMASK     0x01   /* for bit mask                             */
#define A161_STS01_ADDR     0   /* Taxable & Disc. item Status Address      */
#define A161_STS01_CNT      6   /* Taxable & Disc. Status Check Count, R3.0 */
#define A161_TAGSTS_ADDR    1   /* Coupon Target Status Address             */
#define A161_TAGSTS_CNT     7   /* Coupon Target Status Count               */

#define A161_RADIX_10      10   /* _ultoa() radix 10                        */

/*************************** Bit Addressing for AC 161 contol status codes *****************/
                    /****************************************/
                    /* Coupon Status character array        */
                    /* => COMCOUPONPARA.uchCouponStatus[2], */
                    /*    PARACPN.uchCPNStatus[2] etc...    */
                    /*       +----+----+       +----+----+  */
                    /*   [0]:|--xx|xxxx|   [1]:|-xxx|xxxx|  */
                    /*       +----+----+       +----+----+  */
                    /* Field: adr2 adr1         adr4 adr3   */
                    /****************************************/

#define A161_TAX_US          0   /* Current Tax Mode is U.S. Tax         */
#define A161_TAX_CAN         1   /* Current Tax Mode is Canadian Tax     */
#define A161_TAX_VAT         2   /* Current Tax Mode is VAT Tax          */

#define A161_TAX_BIT     0x0f   /* Canadian TAX Write Address (Bit 0,1,2,3) */
#define A161_TAX_BIT_VAT 0x03   /* International VAT Address (Bit 0,1)      */
#define A161_TAX_MAX     0x09   /* Max. Value of Canadian Tax               */
#define A161_TAX_MAX_VAT 0x03   /* Max. Value of International VAT          */
#define A161_TAX_MASK    0x10   /* Not Used Bit (Address 1 & 2)             */

// bit masks for uchCouponStatus[0] coupon status/settings. these MUST be the same as used for AC68 PLU control code
// See ParaPlu.ContPlu.auchContOther[A68_CTRL_1] with A68_TAX_BIT_SERVICE, etc.
#define A161_AUTO_USTAXABLE1    0x01   // Address 2 Bit 0 (affects US taxable itemizer #1 or Canadian tax type index bit 0 or VAT Int'l tax type index bit 0
#define A161_AUTO_USTAXABLE2    0x02   // Address 2 Bit 1 (affects US taxable itemizer #2 or Canadian tax type index bit 1 or VAT Int'l tax type index bit 1
#define A161_AUTO_USTAXABLE3    0x04   // Address 2 Bit 2 (affects US taxable itemizer #3 or Canadian tax type index bit 2
#define A161_AUTO_FOOD_MOD      0x08   // Address 2 Bit 3 (affects US Food Stamp itemizer or Canadian tax type index bit 3 or VAT Int'l Serviceable Address ( A161_TAX_BIT_SERVICE )
#define A161_TAX_BIT_SERVICE    0x08   /* VAT Serviceable Address (Bit 3)        */
#define A161_AUTO_CPN	        0x10   /* Address 2 Bit 4 (Auto Combo Coup)		*/
#define A161_SINGLE_ITEM        0x20   /* Address 2 Bit 5 (Single use Item)        */
#define A161_DISC_ITEM1         0x40   /* Address 2 Bit 6 (Discountable Item #1), same as PLU_AFFECT_DISC1   */
#define A161_DISC_ITEM2         0x80   /* Address 2 Bit 7 (Discountable Item #2), same as PLU_AFFECT_DISC2   */


#define A161_LBSET          0   /* set listbox when set button pushed       */
#define A161_LBENTER        1   /* set listbox when enter button pushed     */

#define A161_BITMASK     0x01   /* for bit mask                             */
#define A161_CHKBIT1     0xeF   /* bit check table for address 1 & 2        *//* ### MOD Saratoga (0xe7 --> 0xeF)(052400) */
#define A161_CHKBIT2     0x7f   /* bit check table for address 3 & 4        */
#define A161_8BITS          8   /* bits for 1 byte                          */

#define A161_DSCRB1         0   /* description offset for address 1 & 2     */
#define A161_SINGLEITEM     4   /* description offset for address 2 bit-5   */
#define A161_DSCITEM1       5   /* description offset for address 2 bit-6   */
#define A161_DSCITEM2       6   /* description offset for address 2 bit-7   */

#define A161_DESC_LEN      64   /* Max. Length of Description   */

#define A161_SPIN_STEP      1
#define A161_SPIN_TURBO     0

#define A161_OK             0
#define A161_ERR           -1
#define A161_LOOKUP_ERR    -2
#define A161_MULTIPLY       5       /* ACC Hide DlgBox Size         */
#define A161_DIVIDE         9       /* ACC Hide DlgBox Size         */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef COMCOUPONPARA  *   LPCPNPARA;

typedef struct {
    WCHAR    szDescOFF[A161_DESC_LEN];
    WCHAR    szDescON[A161_DESC_LEN];
} A161DESC,  *LPA161DESC;

typedef struct {
    WCHAR    szCanDesc[A161_DESC_LEN];
} A161CANADA,  *LPA161CANADA;

/*
* ===========================================================================
*       Macro Definition
* ===========================================================================
*/

/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/

BOOL    A161ReadMax(HWND hDlg, LPUSHORT lpunMaxCpn);
BOOL    A161AllocMem(HWND hDlg, LPHGLOBAL lphCpn, LPHGLOBAL lphDesc, LPHGLOBAL lphCan);
VOID    A161InitDlg(HWND hDlg, USHORT unMaxCpn);
VOID    A161FreeMem(LPHGLOBAL lphCpn, LPHGLOBAL lphDesc, LPHGLOBAL lphCan);
VOID    A161InitCtrlDesc(VOID);
VOID    A161ReadData(USHORT unMaxCpn);
VOID    A161InitList(HWND hDlg, USHORT unMaxCpn);
VOID    A161SetData(HWND hDlg);
VOID    A161SetCtrlList(HWND hDlg, BOOL fMode);
VOID    A161SetListContent(HWND hDlg, BYTE bChkBit, BYTE bCtrlAddr, WPARAM wOffset, BOOL fMode);
VOID    A161SetCanadaDesc(HWND hDlg, LPCPNPARA lpCpn, BOOL fMode);
VOID    A161SetVATDesc(HWND, LPCPNPARA, BOOL);
VOID    A161WriteData(USHORT unMax);
VOID    A161ChkRng(HWND hDlg, WORD wEditID, UINT unMax);
VOID    A161TagChkRng(HWND hDlg, WORD wEditID);
VOID    A161GetData(HWND hDlg);
VOID    A161SaveAmount(HWND hDlg);
VOID    A161SavePriority(HWND hDlg);
VOID    A161SetChkbox(HWND hDlg, WORD wBtnID, BYTE bChkBit, BYTE bBitcnt);
BYTE    A161GetChkbox(HWND hDlg, WORD wBtnID, BYTE bBitcnt);
VOID    A161DeptPluChkRng(HWND hDlg, WORD wEditID);
VOID    A161SetCtrlData(HWND hDlg);
VOID    A161SetCanadaBtn(HWND hDlg);
VOID    A161SetVATBtn(HWND);
VOID    A161GetCtrlData(HWND hDlg);
VOID    A161GetCanadaBtn(HWND hDlg);
VOID    A161GetVATBtn(HWND);
SHORT   A161GetPluNo(BOOL, WCHAR*);
VOID    A161InitAutoList(HWND hDlg, USHORT unMaxCpn);
VOID    A161SetAutoData(HWND hDlg);
VOID	A161AutoCouponCheck(HWND hDlg, USHORT unMaxCpn);
VOID	A161AutoPriUp(HWND hDlg);
VOID	A161AutoPriDown(HWND hDlg);
VOID    A161SaveBonus(HWND hDlg);
VOID    A161Del(HWND hDlg);
VOID	A161ChkPriorityRng(HWND hDlg, WORD wEditID);
HWND	A161ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A161ModeLessDialogWnd (void);

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
BOOL    A161GetPluNo(BOOL, char*);
#endif
/* ===== End of File (A161.H) ===== */
