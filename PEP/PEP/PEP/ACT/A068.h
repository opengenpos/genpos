/*
* ---------------------------------------------------------------------------
* Title         :   PLU Maintenance Header File (AC 63, 64, 68, 82)
* Category      :   Maintenance, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A068.H
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
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
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Jul-03-96 : 01.00.06 : M.Ozawa    : Correct Group No. Limit
* Dec-02-02 :		   : T.Parveen  : SR 67 changed next button increment by 5000
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
*       DialogBox Control ID
* ===========================================================================
*/
#define A68_SPIN_OFFSET        57
#define IDD_A68_CTRL        16300               /* Control Code DialogBox   */
#define IDD_A68_CAN         16301               /* CtrlCode DlgBox (Canada) */
#define IDD_A68_VAT         16302               /* CtrlCode DlgBox (VAT)    */
#define IDD_A68_CUR         (IDD_A68_CTRL + 3)  /* Current No. of PLU Rec.  */
#define IDD_A68_MAX         (IDD_A68_CTRL + 4)  /* Max. No. of PLU Records  */
#define IDD_A68_LIST        (IDD_A68_CTRL + 5)  /* PLU Records ListBox      */
#define IDD_A68_ADDCHG      (IDD_A68_CTRL + 6)  /* Add/Change Button        */
#define IDD_A68_DEL         (IDD_A68_CTRL + 7)  /* Delete Button            */
#define IDD_A68_NO          (IDD_A68_CTRL + 8)  /* PLU No. EditText         */
#define IDD_A68_DEPTNO      (IDD_A68_CTRL + 9)  /* Dept. No. EditText       */
#define IDD_A68_RPTCD       (IDD_A68_CTRL +10)  /* Report Code EditText     */
#define IDD_A68_BONUS       (IDD_A68_CTRL +11)  /* Bonus Ttl Index EditText */
#define IDD_A68_MNEMO       (IDD_A68_CTRL +12)  /* PLU Mnemonic EditText    */
#define IDD_A68_PRESET1     (IDD_A68_CTRL +13)  /* Preset Price #1 EditText */
#define IDD_A68_PRESET2     (IDD_A68_CTRL +14)  /* Preset Price #2 EditText */
#define IDD_A68_PRESET3     (IDD_A68_CTRL +15)  /* Preset Price #3 EditText */
#define IDD_A68_PRESET4     (IDD_A68_CTRL +16)  /* Preset Price #4 EditText */
#define IDD_A68_PRESET5     (IDD_A68_CTRL +17)  /* Preset Price #5 EditText */
#define IDD_A68_CTLLIST     (IDD_A68_CTRL +18)  /* Control Code ListBox     */
#define IDD_A68_CONF        (IDD_A68_CTRL +19)  /* Delete Confirmation      */
#define IDD_A68_DEPT_MAX    (IDD_A68_CTRL +20)  /* Department Data Range    */
#define IDD_A68_TBLNO       (IDD_A68_CTRL +21)  /* Order Entry Prompt Table No. */
#define IDD_A68_GRPNO       (IDD_A68_CTRL +22)  /* Order Entry Prompt Group No. */
#define IDD_A68_PRTPRY      (IDD_A68_CTRL +23)  /* Order Entry Prompt print Priority */
#define IDD_A68_PPI         (IDD_A68_CTRL +24)  /* PPI Code                 */

/* Saratoga */
#define IDD_A68_LINK        (IDD_A68_CTRL +25)  /* Link Plu                 */
#define IDD_A68_SALES       (IDD_A68_CTRL +26)  /* Sales Restriction Code   */
#define IDD_A68_MULTI       (IDD_A68_CTRL +27)  /* Price Multiple           */
#define IDD_A68_NORMAL      (IDD_A68_CTRL +28)  /* Search(Normal) Button    */
#define IDD_A68_EVER        (IDD_A68_CTRL +29)  /* Search(E-Version) Button */
/* ### ADD Saratoga (V1_0.02)(051800) */
#define IDD_A68_FAMILY      (IDD_A68_CTRL +30)  /* FamilyCode EditText      */
#define IDD_A68_STRING		(IDD_A68_CTRL +31)  /* Mnemonic search EditBox  */
#define IDD_A68_COLOR		(IDD_A68_CTRL +32)	/* Color Palette code		*/
#define IDD_A68_TARE		(IDD_A68_CTRL +33)  /* Tare */
#define IDD_A68_EXT_GRP_1	(IDD_A68_CTRL +34)  /* Tare */
#define IDD_A68_EXT_GRP_2	(IDD_A68_CTRL +35)  /* Tare */
#define IDD_A68_EXT_GRP_3	(IDD_A68_CTRL +36)  /* Tare */
#define IDD_A68_EXT_PRI_1	(IDD_A68_CTRL +37)  /* Tare */
#define IDD_A68_EXT_PRI_2	(IDD_A68_CTRL +38)  /* Tare */
#define IDD_A68_EXT_PRI_3	(IDD_A68_CTRL +39)  /* Tare */
#define IDD_A68_ALTMNEMO    (IDD_A68_CTRL +40)  /* PLU Alternate Mnemonic EditText    */
#define IDD_A68_EXPORT_PLU      (IDD_A68_CTRL +41)  /* Export menu - Export PLUs to an output .CSV file    */
#define IDD_A68_FILTERBY_LIST           (IDD_A68_CTRL +42)
#define IDD_A68_FILTERBY_UPDATE         (IDD_A68_CTRL +43)
#define IDD_A68_FILTERBY_DEPTNO         (IDD_A68_CTRL +44)    /* FILTEREDPLUDATA_BY_DEPTNO        */
#define IDD_A68_FILTERBY_GROUPNO        (IDD_A68_CTRL +45)    /* FILTEREDPLUDATA_BY_GROUPNO       */
#define IDD_A68_FILTERBY_PRINTPRIORITY  (IDD_A68_CTRL +46)    /* FILTEREDPLUDATA_BY_PRINTPRIORITY */
#define IDD_A68_FILTERBY_TABLENO        (IDD_A68_CTRL +47)    /* FILTEREDPLUDATA_BY_TABLENO */
#define IDD_A68_FILTERBY_SALESRESTRICT  (IDD_A68_CTRL +48)    /* FILTEREDPLUDATA_BY_SALESRESTRICT */
#define IDD_A68_FILTERBY_PPICODE        (IDD_A68_CTRL +49)    /* FILTEREDPLUDATA_BY_PPICODE */
#define IDD_A68_FILTERBY_ADJ_GROUP      (IDD_A68_CTRL +50)    /* FILTEREDPLUDATA_BY_ADJ_GROUP */
#define IDD_A68_FILTERBY_REM_DEV_MTCH   (IDD_A68_CTRL +51)    /* FILTEREDPLUDATA_BY_REM_DEV_MTCH */
#define IDD_A68_FILTERBY_REM_DEV_ANY    (IDD_A68_CTRL +52)    /* FILTEREDPLUDATA_BY_REM_DEV_ANY */
#define IDD_A68_FILTERBY_TARENO         (IDD_A68_CTRL +53)    /* FILTEREDPLUDATA_BY_TARENO */
#define IDD_A68_FILTERBY_BONUSTOTAL     (IDD_A68_CTRL +54)    /* FILTEREDPLUDATA_BY_BONUSTOTAL */
#define IDD_A68_FILTERBY_DISCITMIZR_MTCH   (IDD_A68_CTRL +55)    /* FILTEREDPLUDATA_BY_DISCITMIZR_MTCH */
#define IDD_A68_FILTERBY_TAXITMIZR_MTCH   (IDD_A68_CTRL +56)    /* FILTEREDPLUDATA_BY_TAXITMIZR_MTCH */
#define IDD_A68_MODELESS_A111           (IDD_A68_CTRL +57)    /* AC111 Tare Maintenance */
#define IDD_A68_MODELESS_A114           (IDD_A68_CTRL +58)    /* AC114 Department Maintenance */
#define IDD_A68_MODELESS_A116           (IDD_A68_CTRL +59)    /* AC116 Promotional PLU */
#define IDD_A68_MODELESS_A160           (IDD_A68_CTRL +60)    /* AC160 Order Entry Prompt Table Maintenance */
#define IDD_A68_MODELESS_A161           (IDD_A68_CTRL +61)    /* AC161 Combination Coupon Table Maintenance */
#define IDD_A68_MODELESS_A170           (IDD_A68_CTRL +62)    /* AC170 Sales Code Restriction Table */
#define IDD_A68_MODELESS_A071           (IDD_A68_CTRL +63)    /* AC71 PPI Edit dialog */

// WARNING: if adding additional controls above then check value of define A68_SPIN_OFFSET!

#define IDD_A68_NO_SPIN     (IDD_A68_NO      + A68_SPIN_OFFSET)
#define IDD_A68_DEPT_SPIN   (IDD_A68_DEPTNO  + A68_SPIN_OFFSET)
#define IDD_A68_RPT_SPIN    (IDD_A68_RPTCD   + A68_SPIN_OFFSET)
#define IDD_A68_BONUS_SPIN  (IDD_A68_BONUS   + A68_SPIN_OFFSET) 
#define IDD_A68_PRE1_SPIN   (IDD_A68_PRESET1 + A68_SPIN_OFFSET)
#define IDD_A68_PRE2_SPIN   (IDD_A68_PRESET2 + A68_SPIN_OFFSET)
#define IDD_A68_PRE3_SPIN   (IDD_A68_PRESET3 + A68_SPIN_OFFSET)
#define IDD_A68_PRE4_SPIN   (IDD_A68_PRESET4 + A68_SPIN_OFFSET)
#define IDD_A68_PRE5_SPIN   (IDD_A68_PRESET5 + A68_SPIN_OFFSET)
#define IDD_A68_TBLNO_SPIN  (IDD_A68_TBLNO   + A68_SPIN_OFFSET)
#define IDD_A68_GRPNO_SPIN  (IDD_A68_GRPNO   + A68_SPIN_OFFSET)
#define IDD_A68_PRTPRY_SPIN (IDD_A68_PRTPRY  + A68_SPIN_OFFSET)
#define IDD_A68_PPI_SPIN    (IDD_A68_PPI     + A68_SPIN_OFFSET)
#define IDD_A68_COLOR_SPIN	(IDD_A68_COLOR	 + A68_SPIN_OFFSET)
#define IDD_A68_TARE_SPIN	(IDD_A68_TARE	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_GRP_1_SPIN	(IDD_A68_EXT_GRP_1	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_GRP_2_SPIN	(IDD_A68_EXT_GRP_2	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_GRP_3_SPIN	(IDD_A68_EXT_GRP_3	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_PRI_1_SPIN	(IDD_A68_EXT_PRI_1	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_PRI_2_SPIN	(IDD_A68_EXT_PRI_2	 + A68_SPIN_OFFSET)
#define IDD_A68_EXT_PRI_3_SPIN	(IDD_A68_EXT_PRI_3	 + A68_SPIN_OFFSET)

/* Saratoga */
#define IDD_A68_LINK_SPIN   (IDD_A68_LINK    + A68_SPIN_OFFSET)
#define IDD_A68_SALES_SPIN  (IDD_A68_SALES   + A68_SPIN_OFFSET)
#define IDD_A68_MULTI_SPIN  (IDD_A68_MULTI   + A68_SPIN_OFFSET)
/* ### ADD Saratoga (V1_0.02)(051800) */
#define IDD_A68_FAMILY_SPIN (IDD_A68_FAMILY  + A68_SPIN_OFFSET)


/* ===== Control Code DialogBox ===== */
    /* ----- ADDRESS 3 ----- */
#define IDD_A68_STS01   (IDD_A68_COLOR_SPIN + 1)   /* Plus/Minus(Credit) PLU   */
#define IDD_A68_STS02   (IDD_A68_STS01 + 1)      /* Normal/Special Print     */
#define IDD_A68_STS03   (IDD_A68_STS01 + 2)      /* Not/Request Supervisor   */
#define IDD_A68_STS04   (IDD_A68_STS01 + 3)      /* Compulsory Print Mod.Key */
    /* ----- ADDRESS 4 ----- */
#define IDD_A68_STS05   (IDD_A68_STS01 + 4)      /* Print Mod. or Condiment  */
#define IDD_A68_STS06   (IDD_A68_STS01 + 5)      /* Not/Print Validation     */
#define IDD_A68_STS07   (IDD_A68_STS01 + 6)      /* Not/Issue Single(Double) */
#define IDD_A68_STS08   (IDD_A68_STS01 + 7)      /* Single/Double Receipt    */
    /* ----- ADDRESS 5 ----- */
#define IDD_A68_STS09   (IDD_A68_STS01 + 8)      /* Not/Taxable Itemizer #1, PLU_AFFECT_TAX1  */
#define IDD_A68_STS10   (IDD_A68_STS01 + 9)      /* Not/Taxable Itemizer #2, PLU_AFFECT_TAX2  */
#define IDD_A68_STS11   (IDD_A68_STS01 +10)      /* Not/Taxable Itemizer #3, PLU_AFFECT_TAX3  */
    /* ----- ADDRESS 6 ----- */
#define IDD_A68_STS12   (IDD_A68_STS01 +11)      /* Not/Discount Itemizer #1, PLU_AFFECT_DISC1 */
#define IDD_A68_STS13   (IDD_A68_STS01 +12)      /* Not/Discount Itemizer #2, PLU_AFFECT_DISC2 */
    /* ----- ADDRESS 7 ----- */
#define IDD_A68_STS14   (IDD_A68_STS01 +13)      /* Not/Scalable, PLU_SCALABLE              */
#define IDD_A68_STS15   (IDD_A68_STS01 +14)      /* Normal/Condiment, PLU_CONDIMENT         */
#define IDD_A68_STS16   (IDD_A68_STS01 +15)      /* Use Own Ctrl. Code/Dept., PLU_USE_DEPTCTL */
    /* ----- ADDRESS 8 ----- */
#define IDD_A68_STS17   (IDD_A68_STS01 +16)      /* Not/Send Remote Printer1, PLU_SND_KITCH1 */
#define IDD_A68_STS18   (IDD_A68_STS01 +17)      /* Not/Send Remote Printer2, PLU_SND_KITCH2 */
#define IDD_A68_STS19   (IDD_A68_STS01 +18)      /* Not/Send Remote Printer3, PLU_SND_KITCH3 */
#define IDD_A68_STS20   (IDD_A68_STS01 +19)      /* Not/Send Remote Printer4, PLU_SND_KITCH4 */
    /* ----- ADDRESS 10 ----- */
#define IDD_A68_STS21   (IDD_A68_STS01 +20)      /* Not/Use Adjective Grp. 1, PLU_USE_ADJG1 */
#define IDD_A68_STS22   (IDD_A68_STS01 +21)      /* Not/Use Adjective Grp. 2, PLU_USE_ADJG2 */
#define IDD_A68_STS23   (IDD_A68_STS01 +22)      /* Not/Use Adjective Grp. 3, PLU_USE_ADJG3 */
#define IDD_A68_STS24   (IDD_A68_STS01 +23)      /* Not/Use Adjective Grp. 4, PLU_USE_ADJG4 */
    /* ----- ADDRESS 11 ----- */
#define IDD_A68_ADJKEY  (IDD_A68_STS01 +24)      /* Compulsory Adjective Key */
#define IDD_A68_STS25   (IDD_A68_STS01 +25)      /* Allow/Prohibit Vari. 1, PLU_PROHIBIT_VAL1   */
    /* ----- ADDRESS 12 ----- */
#define IDD_A68_STS26   (IDD_A68_STS01 +26)      /* Allow/Prohibit Vari. 2, PLU_PROHIBIT_VAL2   */
#define IDD_A68_STS27   (IDD_A68_STS01 +27)      /* Allow/Prohibit Vari. 3, PLU_PROHIBIT_VAL3   */
#define IDD_A68_STS28   (IDD_A68_STS01 +28)      /* Allow/Prohibit Vari. 4, PLU_PROHIBIT_VAL4   */
#define IDD_A68_STS29   (IDD_A68_STS01 +29)      /* Allow/Prohibit Vari. 5, PLU_PROHIBIT_VAL5   */
    /* ----- ADDRESS 18 ----- */
#define IDD_A68_STS30   (IDD_A68_STS01 +30)      /* Not/Send Remote Printer5, PLU_SND_KITCH5 */
#define IDD_A68_STS31   (IDD_A68_STS01 +31)      /* Not/Send Remote Printer6, PLU_SND_KITCH6 */
#define IDD_A68_STS32   (IDD_A68_STS01 +32)      /* Not/Send Remote Printer7, PLU_SND_KITCH7 */
#define IDD_A68_STS33   (IDD_A68_STS01 +33)      /* Not/Send Remote Printer8, PLU_SND_KITCH8 */


    /* ----- Canadian Tax Mode RadioButton ----- */
#define IDD_A68_RADIO1   (IDD_A68_STS01 +34)      /* Meal Item                */
#define IDD_A68_RADIO2   (IDD_A68_STS01 +35)      /* Carbonated               */
#define IDD_A68_RADIO3   (IDD_A68_STS01 +36)      /* Snack Item               */
#define IDD_A68_RADIO4   (IDD_A68_STS01 +37)      /* Beer/Wine                */
#define IDD_A68_RADIO5   (IDD_A68_STS01 +38)      /* Liquor                   */
#define IDD_A68_RADIO6   (IDD_A68_STS01 +39)      /* Grocery                  */
#define IDD_A68_RADIO7   (IDD_A68_STS01 +40)      /* Tips Item                */
#define IDD_A68_RADIO8   (IDD_A68_STS01 +41)      /* PST1 Only                */
#define IDD_A68_RADIO9   (IDD_A68_STS01 +42)      /* Baked Item               */
#define IDD_A68_RADIOA	 (IDD_A68_STS01 +43)	  /* Baked Item NO PST		  */	

    /* ----- International VAT Mode RadioButton ----- */
#define IDD_A68_VATRADIO1   (IDD_A68_STS01 +44)      /* Meal Item                */
#define IDD_A68_VATRADIO2   (IDD_A68_STS01 +45)      /* Carbonated               */
#define IDD_A68_VATRADIO3   (IDD_A68_STS01 +46)      /* Snack Item               */
#define IDD_A68_VATRADIO4   (IDD_A68_STS01 +47)      /* Beer/Wine                */
#define IDD_A68_VATSERVICE  (IDD_A68_STS01 +48)      /* Serviceable              */

    /* ----- ADDRESS 19 ----- */
#define IDD_A68_TYPEKEY     (IDD_A68_STS01 +49)      /* Type Key (Saratoga)*/

    /* ----- ADDRESS 19 ----- *//* ### ADD Saratoga (052700) */
#define IDD_A68_STS34       (IDD_A68_STS01 +50)      /* FoodStamp          */
#define IDD_A68_STS35       (IDD_A68_STS01 +51)      /* WIC          */
#define IDD_A68_STS36		(IDD_A68_STS01 +52)      // SI and non-zero  ***PDINU
#define IDD_A68_RESTRICT_QR			(IDD_A68_STS01 +53)		 // Quantity Restriction ***PDINU
#define IDD_A68_REQUIRE_QR			(IDD_A68_STS01 +54)		 // Require Quantity  ***PDINU
	/* ----- ADDRESS 20 ----- */
#define IDD_A68_GIFT_ISSUE		(IDD_A68_STS01 +55)      /* Gift Card Type  *SSTOLTZ*/
#define IDD_A68_GIFT_RELOAD		(IDD_A68_STS01 +56)		 //Reload
#define IDD_A005_PREC01			(IDD_A68_STS01 +57)		 // PPI and Combination Coupon Precedence - DEFAULT
#define IDD_A005_PREC02			(IDD_A68_STS01 +58)		 // PPI and Combination Coupon Precedence - COUPONS
#define IDD_A68_GIFTCARD		(IDD_A68_STS01 +59)		 // Gift Card PLU item
#define IDD_A68_EPAYMENT		(IDD_A68_STS01 +60)		 // Employee Payroll Deduction and E-Meal for VCS
//#define IDD_A005_PREC03			(IDD_A68_STS01 +59)		 // PPI and Combination Coupon Precedence

/* ----- Text String ----- */
#define IDD_A68_STRDESC         100
#define IDD_A68_STRDESC2        (IDD_A68_STRDESC +  1)
#define IDD_A68_STRPLUNO        (IDD_A68_STRDESC + 11)
#define IDD_A68_STRPLULEN       (IDD_A68_STRDESC + 12)
#define IDD_A68_STRDEPTNO       (IDD_A68_STRDESC + 13)
#define IDD_A68_STRRPT          (IDD_A68_STRDESC + 14)
#define IDD_A68_STRRPTNO        (IDD_A68_STRDESC + 15)
#define IDD_A68_STRINDEX        (IDD_A68_STRDESC + 16)
#define IDD_A68_STRINDEXLEN     (IDD_A68_STRDESC + 17)
#define IDD_A68_STRTABLE        (IDD_A68_STRDESC + 18)
#define IDD_A68_STRTABLELEN     (IDD_A68_STRDESC + 19)
#define IDD_A68_STRGRP          (IDD_A68_STRDESC + 20)
#define IDD_A68_STRGRPLEN       (IDD_A68_STRDESC + 21)
#define IDD_A68_STRPRT          (IDD_A68_STRDESC + 22)
#define IDD_A68_STRPRTLEN       (IDD_A68_STRDESC + 23)
#define IDD_A68_STRPPI          (IDD_A68_STRDESC + 24)
#define IDD_A68_STRPPILEN       (IDD_A68_STRDESC + 25)
#define IDD_A68_STRMNEMO        (IDD_A68_STRDESC + 26)
#define IDD_A68_STRMNEMOMAX     (IDD_A68_STRDESC + 27)
#define IDD_A68_STRADJ          (IDD_A68_STRDESC + 28)
#define IDD_A68_STRADJ1         (IDD_A68_STRDESC + 29)
#define IDD_A68_STRADJ2         (IDD_A68_STRDESC + 30)
#define IDD_A68_STRADJ3         (IDD_A68_STRDESC + 31)
#define IDD_A68_STRADJ4         (IDD_A68_STRDESC + 32)
#define IDD_A68_STRADJ5         (IDD_A68_STRDESC + 33)
#define IDD_A68_STRADJLEN       (IDD_A68_STRDESC + 34)
#define IDD_A68_STRCODE         (IDD_A68_STRDESC + 35)
#define IDD_A68_STRPRICE        (IDD_A68_STRDESC + 36)

#define IDD_A68_CTLDESC         (IDD_A68_STRDESC + 37)

#define IDD_A68_CANDESC         (IDD_A68_STRDESC + 38)
#define IDD_A68_CANTAX          (IDD_A68_STRDESC + 39)

#define IDD_A68_VATDESC         (IDD_A68_STRDESC + 40)
#define IDD_A68_VATTAX          (IDD_A68_STRDESC + 41)

#define IDD_A68_STRLINK         (IDD_A68_STRDESC + 42)  /* Saratoga */
#define IDD_A68_STRSALES        (IDD_A68_STRDESC + 43)  /* Saratoga */
#define IDD_A68_STRMULTI        (IDD_A68_STRDESC + 44)  /* Saratoga */
#define IDD_A68_STRLINKLEN      (IDD_A68_STRDESC + 45)  /* Saratoga */
#define IDD_A68_STRSALESLEN     (IDD_A68_STRDESC + 46)  /* Saratoga */
#define IDD_A68_STRMULTILEN     (IDD_A68_STRDESC + 47)  /* Saratoga */
#define IDD_A68_STRPLUGP        (IDD_A68_STRDESC + 48)  /* Saratoga */
#define IDD_A68_TYPEGP          (IDD_A68_STRDESC + 49)  /* Saratoga */

#define IDD_A68_NEXT            (IDD_A68_STRDESC + 50)  /* Saratoga */
#define IDD_A68_TOP             (IDD_A68_STRDESC + 51)  /* Saratoga */

/* ### ADD Saratoga (V1_0.02)(051800) */
#define IDD_A68_STRFAMILY       (IDD_A68_STRDESC + 52)  /* Saratoga */
#define IDD_A68_STRFAMILYLEN    (IDD_A68_STRDESC + 53)  /* Saratoga */
#define IDD_A68_STRINGSEARCH    (IDD_A68_STRDESC + 54)  /* Mnemonic search PuchButton */
#define IDD_A68_MNEMOPLUNO2     (IDD_A68_STRDESC + 55)

#define IDD_A68_STRCOLORLEN     (IDD_A68_STRDESC + 56)
#define IDD_A68_STRCOLOR        (IDD_A68_STRDESC + 57)

#define IDD_A68_STRTARELEN     (IDD_A68_STRDESC + 58)
#define IDD_A68_STRTARE        (IDD_A68_STRDESC + 59)

#define IDD_A68_STSTARECCHECK  (IDD_A68_STRDESC + 60)
#define IDD_A68_PPIOPTN		   (IDD_A68_STRDESC + 61)	// PPI and Combination Coupon Precedence
#define IDD_A68_STRPREC01	   (IDD_A68_STRDESC + 62)	// For PPI and Combination coupon precedence
#define IDD_A68_STRPREC02	   (IDD_A68_STRDESC + 63)	// For PPI and Combination coupon precedence
#define IDD_A68_GFTCRDOPN	   (IDD_A68_STRDESC + 64)   // Gift Card Precedence

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
    /* ----- Control Code String (Bit -> OFF (US/Canada)) ----- */
#define IDS_A68_STS_001  (IDS_A68)
#define IDS_A68_STS_002  (IDS_A68_STS_001 + 1)
#define IDS_A68_STS_003  (IDS_A68_STS_001 + 2)
#define IDS_A68_STS_004  (IDS_A68_STS_001 + 3)
#define IDS_A68_STS_005  (IDS_A68_STS_001 + 4)
#define IDS_A68_STS_006  (IDS_A68_STS_001 + 5)
#define IDS_A68_STS_007  (IDS_A68_STS_001 + 6)
#define IDS_A68_STS_008  (IDS_A68_STS_001 + 7)
#define IDS_A68_STS_009  (IDS_A68_STS_001 + 8)
#define IDS_A68_STS_010  (IDS_A68_STS_001 + 9)
#define IDS_A68_STS_011  (IDS_A68_STS_001 +10)
#define IDS_A68_STS_012  (IDS_A68_STS_001 +11)
#define IDS_A68_STS_013  (IDS_A68_STS_001 +12)
#define IDS_A68_STS_014  (IDS_A68_STS_001 +13)
#define IDS_A68_STS_015  (IDS_A68_STS_001 +14)
#define IDS_A68_STS_016  (IDS_A68_STS_001 +15)
#define IDS_A68_STS_017  (IDS_A68_STS_001 +16)
#define IDS_A68_STS_018  (IDS_A68_STS_001 +17)
#define IDS_A68_STS_019  (IDS_A68_STS_001 +18)
#define IDS_A68_STS_020  (IDS_A68_STS_001 +19)
#define IDS_A68_STS_021  (IDS_A68_STS_001 +20)
#define IDS_A68_STS_022  (IDS_A68_STS_001 +21)
#define IDS_A68_STS_023  (IDS_A68_STS_001 +22)
#define IDS_A68_STS_024  (IDS_A68_STS_001 +23)
#define IDS_A68_STS_025  (IDS_A68_STS_001 +24)
#define IDS_A68_STS_026  (IDS_A68_STS_001 +25)
#define IDS_A68_STS_027  (IDS_A68_STS_001 +26)
#define IDS_A68_STS_028  (IDS_A68_STS_001 +27)
#define IDS_A68_STS_029  (IDS_A68_STS_001 +28)
#define IDS_A68_STS_030  (IDS_A68_STS_001 +29)
#define IDS_A68_STS_031  (IDS_A68_STS_001 +30)
#define IDS_A68_STS_032  (IDS_A68_STS_001 +31)
#define IDS_A68_STS_033  (IDS_A68_STS_001 +32)
#define IDS_A68_STS_034  (IDS_A68_STS_001 +33) /* ### ADD Saratoga (052300) *//* FoodStamp */

    /* ----- Control Code String (Bit -> ON (US/Canada)) ----- */
#define IDS_A68_STS_101  (IDS_A68_STS_001 +34)
#define IDS_A68_STS_102  (IDS_A68_STS_001 +35)
#define IDS_A68_STS_103  (IDS_A68_STS_001 +36)
#define IDS_A68_STS_104  (IDS_A68_STS_001 +37)
#define IDS_A68_STS_105  (IDS_A68_STS_001 +38)
#define IDS_A68_STS_106  (IDS_A68_STS_001 +39)
#define IDS_A68_STS_107  (IDS_A68_STS_001 +40)
#define IDS_A68_STS_108  (IDS_A68_STS_001 +41)
#define IDS_A68_STS_109  (IDS_A68_STS_001 +42)
#define IDS_A68_STS_110  (IDS_A68_STS_001 +43)
#define IDS_A68_STS_111  (IDS_A68_STS_001 +44)
#define IDS_A68_STS_112  (IDS_A68_STS_001 +45)
#define IDS_A68_STS_113  (IDS_A68_STS_001 +46)
#define IDS_A68_STS_114  (IDS_A68_STS_001 +47)
#define IDS_A68_STS_115  (IDS_A68_STS_001 +48)
#define IDS_A68_STS_116  (IDS_A68_STS_001 +49)
#define IDS_A68_STS_117  (IDS_A68_STS_001 +50)
#define IDS_A68_STS_118  (IDS_A68_STS_001 +51)
#define IDS_A68_STS_119  (IDS_A68_STS_001 +52)
#define IDS_A68_STS_120  (IDS_A68_STS_001 +53)
#define IDS_A68_STS_121  (IDS_A68_STS_001 +54)
#define IDS_A68_STS_122  (IDS_A68_STS_001 +55)
#define IDS_A68_STS_123  (IDS_A68_STS_001 +56)
#define IDS_A68_STS_124  (IDS_A68_STS_001 +57)
#define IDS_A68_STS_125  (IDS_A68_STS_001 +58)
#define IDS_A68_STS_126  (IDS_A68_STS_001 +59)
#define IDS_A68_STS_127  (IDS_A68_STS_001 +60)
#define IDS_A68_STS_128  (IDS_A68_STS_001 +61)
#define IDS_A68_STS_129  (IDS_A68_STS_001 +62)
#define IDS_A68_STS_130  (IDS_A68_STS_001 +63)
#define IDS_A68_STS_131  (IDS_A68_STS_001 +64)
#define IDS_A68_STS_132  (IDS_A68_STS_001 +65)
#define IDS_A68_STS_133  (IDS_A68_STS_001 +66)
#define IDS_A68_STS_134  (IDS_A68_STS_001 +67) /* FoodStamp *//* ### ADD Saratoga (052300) */


    /* ----- Control Code Strings (Address 11, Bit 0 - 2) ----- */
#define IDS_A68_STS_ADJ0 (IDS_A68_STS_001 +68)
#define IDS_A68_STS_ADJ1 (IDS_A68_STS_001 +69)
#define IDS_A68_STS_ADJ2 (IDS_A68_STS_001 +70)
#define IDS_A68_STS_ADJ3 (IDS_A68_STS_001 +71)
#define IDS_A68_STS_ADJ4 (IDS_A68_STS_001 +72)
#define IDS_A68_STS_ADJ5 (IDS_A68_STS_001 +73)

    /* ----- Control Code Strings (Address 5, Bit 0 - 3 (Canadian Tax) ----- */
#define IDS_A68_STS_TAX0 (IDS_A68_STS_001 +74)
#define IDS_A68_STS_TAX1 (IDS_A68_STS_001 +75)
#define IDS_A68_STS_TAX2 (IDS_A68_STS_001 +76)
#define IDS_A68_STS_TAX3 (IDS_A68_STS_001 +77)
#define IDS_A68_STS_TAX4 (IDS_A68_STS_001 +78)
#define IDS_A68_STS_TAX5 (IDS_A68_STS_001 +79)
#define IDS_A68_STS_TAX6 (IDS_A68_STS_001 +80)
#define IDS_A68_STS_TAX7 (IDS_A68_STS_001 +81)
#define IDS_A68_STS_TAX8 (IDS_A68_STS_001 +82)
#define IDS_A68_STS_TAX9 (IDS_A68_STS_001 +83)

    /* ----- Warning And Error Message ----- */
#define IDS_A68_FLEX_EMPTY   (IDS_A68_STS_TAX9 + 1)
#define IDS_A68_MAX          (IDS_A68_STS_TAX9 + 2)
#define IDS_A68_CUR          (IDS_A68_STS_TAX9 + 3)
#define IDS_A68_DELCONF      (IDS_A68_STS_TAX9 + 4)
#define IDS_A68_OVERREC      (IDS_A68_STS_TAX9 + 5)
#define IDS_A68_CHGTYPE      (IDS_A68_STS_TAX9 + 6)

    /* ----- PLU Type String ----- */
#define IDS_A68_OPENPLU      (IDS_A68_CHGTYPE + 1)
#define IDS_A68_NONADJ       (IDS_A68_CHGTYPE + 2)
#define IDS_A68_ONEADJ       (IDS_A68_CHGTYPE + 3)

    /* ----- Department Data Range ----- */
#define IDS_A68_DEPT_MAX     (IDS_A68_ONEADJ  + 1)

    /* ----- Type Code Strings (Address 19, Bit 0 - 2) ----- */
#define IDS_A68_STS_TYPEKEY0 (IDS_A68_DEPT_MAX + 1)     /* Saratoga */
#define IDS_A68_STS_TYPEKEY1 (IDS_A68_DEPT_MAX + 2)     /* Saratoga */
#define IDS_A68_STS_TYPEKEY2 (IDS_A68_DEPT_MAX + 3)     /* Saratoga */
#define IDS_A68_STS_TYPEKEY3 (IDS_A68_DEPT_MAX + 4)     /* Saratoga */

    /* ----- Description String ----- */

#define IDS_A68_CAPTION     (IDS_A68 + 100)
#define IDS_A68_STRDESC     (IDS_A68_CAPTION +   1)
#define IDS_A68_STRDESC2    (IDS_A68_CAPTION +   2)
#define IDS_A68_ADDCHG      (IDS_A68_CAPTION +   3)
#define IDS_A68_DEL         (IDS_A68_CAPTION +   4)
#define IDS_A68_STRPLUNO    (IDS_A68_CAPTION +   5)
#define IDS_A68_STRPLULEN   (IDS_A68_CAPTION +   6)
#define IDS_A68_STRDEPTNO   (IDS_A68_CAPTION +   7)
#define IDS_A68_STRRPT      (IDS_A68_CAPTION +   8)
#define IDS_A68_STRRPTNO    (IDS_A68_CAPTION +   9)
#define IDS_A68_STRINDEX    (IDS_A68_CAPTION +  10)
#define IDS_A68_STRINDEXLEN (IDS_A68_CAPTION +  11)
#define IDS_A68_STRTABLE    (IDS_A68_CAPTION +  12)
#define IDS_A68_STRTABLELEN (IDS_A68_CAPTION +  13)
#define IDS_A68_STRGRP      (IDS_A68_CAPTION +  14)
#define IDS_A68_STRGRPLEN   (IDS_A68_CAPTION +  15)
#define IDS_A68_STRPRT      (IDS_A68_CAPTION +  16)
#define IDS_A68_STRPRTLEN   (IDS_A68_CAPTION +  17)
#define IDS_A68_STRPPI      (IDS_A68_CAPTION +  18)
#define IDS_A68_STRPPILEN   (IDS_A68_CAPTION +  19)
#define IDS_A68_STRMNEMO    (IDS_A68_CAPTION +  20)
#define IDS_A68_STRMNEMOMAX (IDS_A68_CAPTION +  21)
#define IDS_A68_STRADJ      (IDS_A68_CAPTION +  22)
#define IDS_A68_STRADJ1     (IDS_A68_CAPTION +  23)
#define IDS_A68_STRADJ2     (IDS_A68_CAPTION +  24)
#define IDS_A68_STRADJ3     (IDS_A68_CAPTION +  25)
#define IDS_A68_STRADJ4     (IDS_A68_CAPTION +  26)
#define IDS_A68_STRADJ5     (IDS_A68_CAPTION +  27)
#define IDS_A68_STRADJLEN   (IDS_A68_CAPTION +  28)
#define IDS_A68_STRCODE     (IDS_A68_CAPTION +  29)
#define IDS_A68_STRPRICE    (IDS_A68_CAPTION +  30)
#define IDS_A68_CONF        (IDS_A68_CAPTION +  31)

#define IDS_A68_CTL_CAPTION (IDS_A68_CAPTION +  32)
#define IDS_A68_CTL_01      (IDS_A68_CAPTION +  33)
#define IDS_A68_CTL_02      (IDS_A68_CAPTION +  34)
#define IDS_A68_CTL_03      (IDS_A68_CAPTION +  35)
#define IDS_A68_CTL_04      (IDS_A68_CAPTION +  36)
#define IDS_A68_CTL_05      (IDS_A68_CAPTION +  37)
#define IDS_A68_CTL_06      (IDS_A68_CAPTION +  38)
#define IDS_A68_CTL_07      (IDS_A68_CAPTION +  39)
#define IDS_A68_CTL_08      (IDS_A68_CAPTION +  40)
#define IDS_A68_CTL_09      (IDS_A68_CAPTION +  41)
#define IDS_A68_CTL_10      (IDS_A68_CAPTION +  42)
#define IDS_A68_CTL_11      (IDS_A68_CAPTION +  43)
#define IDS_A68_CTL_12      (IDS_A68_CAPTION +  44)
#define IDS_A68_CTL_13      (IDS_A68_CAPTION +  45)
#define IDS_A68_CTL_14      (IDS_A68_CAPTION +  46)
#define IDS_A68_CTL_15      (IDS_A68_CAPTION +  47)
#define IDS_A68_CTL_16      (IDS_A68_CAPTION +  48)
#define IDS_A68_CTL_17      (IDS_A68_CAPTION +  49)
#define IDS_A68_CTL_18      (IDS_A68_CAPTION +  50)
#define IDS_A68_CTL_19      (IDS_A68_CAPTION +  51)
#define IDS_A68_CTL_20      (IDS_A68_CAPTION +  52)
#define IDS_A68_CTL_21      (IDS_A68_CAPTION +  53)
#define IDS_A68_CTL_22      (IDS_A68_CAPTION +  54)
#define IDS_A68_CTL_23      (IDS_A68_CAPTION +  55)
#define IDS_A68_CTL_24      (IDS_A68_CAPTION +  56)
#define IDS_A68_CTL_25      (IDS_A68_CAPTION +  57)
#define IDS_A68_CTL_26      (IDS_A68_CAPTION +  58)
#define IDS_A68_CTL_27      (IDS_A68_CAPTION +  59)
#define IDS_A68_CTL_28      (IDS_A68_CAPTION +  60)
#define IDS_A68_CTL_29      (IDS_A68_CAPTION +  61)
#define IDS_A68_CTL_30      (IDS_A68_CAPTION +  62)
#define IDS_A68_CTL_31      (IDS_A68_CAPTION +  63)
#define IDS_A68_CTL_32      (IDS_A68_CAPTION +  64)
#define IDS_A68_CTL_33      (IDS_A68_CAPTION +  65)
#define IDS_A68_CTLDESC     (IDS_A68_CAPTION +  66)

#define IDS_A68_CAN_CAPTION (IDS_A68_CAPTION +  67)
#define IDS_A68_CAN_01      (IDS_A68_CAPTION +  68)
#define IDS_A68_CAN_02      (IDS_A68_CAPTION +  69)
#define IDS_A68_CAN_03      (IDS_A68_CAPTION +  70)
#define IDS_A68_CAN_04      (IDS_A68_CAPTION +  71)
#define IDS_A68_CAN_05      (IDS_A68_CAPTION +  72)
#define IDS_A68_CAN_06      (IDS_A68_CAPTION +  73)
#define IDS_A68_CAN_07      (IDS_A68_CAPTION +  74)
#define IDS_A68_CAN_08      (IDS_A68_CAPTION +  75)

#define IDS_A68_CAN_12      (IDS_A68_CAPTION +  76)
#define IDS_A68_CAN_13      (IDS_A68_CAPTION +  77)
#define IDS_A68_CAN_14      (IDS_A68_CAPTION +  78)
#define IDS_A68_CAN_15      (IDS_A68_CAPTION +  79)
#define IDS_A68_CAN_16      (IDS_A68_CAPTION +  80)
#define IDS_A68_CAN_17      (IDS_A68_CAPTION +  81)
#define IDS_A68_CAN_18      (IDS_A68_CAPTION +  82)
#define IDS_A68_CAN_19      (IDS_A68_CAPTION +  83)
#define IDS_A68_CAN_20      (IDS_A68_CAPTION +  84)
#define IDS_A68_CAN_21      (IDS_A68_CAPTION +  85)
#define IDS_A68_CAN_22      (IDS_A68_CAPTION +  86)
#define IDS_A68_CAN_23      (IDS_A68_CAPTION +  87)
#define IDS_A68_CAN_24      (IDS_A68_CAPTION +  88)
#define IDS_A68_CAN_25      (IDS_A68_CAPTION +  89)
#define IDS_A68_CAN_26      (IDS_A68_CAPTION +  90)
#define IDS_A68_CAN_27      (IDS_A68_CAPTION +  91)
#define IDS_A68_CAN_28      (IDS_A68_CAPTION +  92)
#define IDS_A68_CAN_29      (IDS_A68_CAPTION +  93)
#define IDS_A68_CAN_30      (IDS_A68_CAPTION +  94)
#define IDS_A68_CAN_31      (IDS_A68_CAPTION +  95)
#define IDS_A68_CAN_32      (IDS_A68_CAPTION +  96)
#define IDS_A68_CAN_33      (IDS_A68_CAPTION +  97)
#define IDS_A68_CANDESC     (IDS_A68_CAPTION +  98)
#define IDS_A68_CANTAX      (IDS_A68_CAPTION +  99)

#define IDS_A68_RADIO1      (IDS_A68_CAPTION + 100)
#define IDS_A68_RADIO2      (IDS_A68_CAPTION + 101)
#define IDS_A68_RADIO3      (IDS_A68_CAPTION + 102)
#define IDS_A68_RADIO4      (IDS_A68_CAPTION + 103)
#define IDS_A68_RADIO5      (IDS_A68_CAPTION + 104)
#define IDS_A68_RADIO6      (IDS_A68_CAPTION + 105)
#define IDS_A68_RADIO7      (IDS_A68_CAPTION + 106)
#define IDS_A68_RADIO8      (IDS_A68_CAPTION + 107)
#define IDS_A68_RADIO9      (IDS_A68_CAPTION + 108)
#define IDS_A68_RADIOA		(IDS_A68_CAPTION + 109)

#define IDS_A68_VATDESC     (IDS_A68_CAPTION + 110)
#define IDS_A68_VATTAX      (IDS_A68_CAPTION + 111)
#define IDS_A68_VATRADIO1   (IDS_A68_CAPTION + 112)
#define IDS_A68_VATRADIO2   (IDS_A68_CAPTION + 113)
#define IDS_A68_VATRADIO3   (IDS_A68_CAPTION + 114)
#define IDS_A68_VATRADIO4   (IDS_A68_CAPTION + 115)
#define IDS_A68_VATSERVICE  (IDS_A68_CAPTION + 116)

#define IDS_A68_VATSERON    (IDS_A68_CAPTION + 117)
#define IDS_A68_VATSEROFF   (IDS_A68_CAPTION + 118)

/* Saratoga */
#define IDS_A68_STRLINK     (IDS_A68_CAPTION + 119)
#define IDS_A68_STRSALES    (IDS_A68_CAPTION + 120)
#define IDS_A68_STRMULTI    (IDS_A68_CAPTION + 121)
#define IDS_A68_STRLINKLEN  (IDS_A68_CAPTION + 122)
#define IDS_A68_STRSALESLEN (IDS_A68_CAPTION + 123)
#define IDS_A68_STRMULTILEN (IDS_A68_CAPTION + 124)
#define IDS_A68_NORMAL      (IDS_A68_CAPTION + 125)
#define IDS_A68_EVER        (IDS_A68_CAPTION + 126)
#define IDS_A68_TYPEGP      (IDS_A68_CAPTION + 127)
#define IDS_A68_ERR_PLUNO   (IDS_A68_CAPTION + 128)
#define IDS_A68_LABEL       (IDS_A68_CAPTION + 129)

#define IDS_A68_NEXT        (IDS_A68_CAPTION + 130)  /* Saratoga */
#define IDS_A68_TOP         (IDS_A68_CAPTION + 131)  /* Saratoga */
#define IDS_A68_ERR_UNKOWN  (IDS_A68_CAPTION + 132)  /* Saratoga */

/* ### ADD Saratoga (V1_0.02)(051800) */
#define IDS_A68_STRFAMILY   (IDS_A68_CAPTION + 133)  /* Saratoga */
#define IDS_A68_STRFAMILYLEN (IDS_A68_CAPTION + 134) /* Saratoga */
#define IDS_A68_CTL_34      (IDS_A68_CAPTION + 135)  /* FoodStamp (052300) */

#define IDS_A68_ERR_PLUREC  (IDS_A68_CAPTION + 136)  /* 09/04/00 */

/* ---- ESMITH Plu Preset Types ---- */
#define IDD_A68_OPENTYPE                (IDS_A68_CAPTION + 137)
#define IDD_A68_PRESETTYPE              (IDS_A68_CAPTION + 138)
#define IDD_A68_ADJECTIVETYPE           (IDS_A68_CAPTION + 139)
#define IDD_A68_STROPEN                 (IDS_A68_CAPTION + 140)
#define IDD_A68_STRPRESET               (IDS_A68_CAPTION + 141)
#define IDD_A68_STRADJECTIVE            (IDS_A68_CAPTION + 142)
#define IDD_A68_STRPLUTYPE              (IDS_A68_CAPTION + 143)

/* CSMALL - SR 948 */
#define IDS_A68_PLU_NOTEXIST	(IDS_A68_CAPTION + 144)

/*PDINU - SR 1016 */
#define IDS_A68_ERR_NOVAL		(IDS_A68_CAPTION + 145)

#define IDS_A68_STRCOLOR		(IDS_A68_CAPTION + 146)
#define IDS_A68_STRCOLORLEN		(IDS_A68_CAPTION + 147)
#define IDS_A68_ERR_PLUMNEM		(IDS_A68_CAPTION + 148)
#define IDS_A68_STRTARE			(IDS_A68_CAPTION + 149)
#define IDS_A68_STRTARELEN		(IDS_A68_CAPTION + 150)
#define IDS_A68_STRTARECHECK    (IDS_A68_CAPTION + 151)

/* ### WARNING! (MAX --> IDS_A68_CAPTION + 135) */

/*
* ===========================================================================
*       PLU record definitions
* ===========================================================================
*/
#define A68_CTRL_SIZE       8   /* PLU Control Code field size (8)      */
/*#define A68_MNEMO_SIZE     12    PLU Mnemonic field size (12)         */
#define A68_MNEMO_SIZE     20   /* PLU Mnemonic field size (12)         */
#define A68_PRICE_SIZE      3   /* PLU Preset price field size (3)      */
#define A68_TYPE3_NUM       4   /* loop counter for one adjective PLU   */
#define A68_ADJ_NUM         5   /* loop counter for PLU preset price    */
#define A68_MAXCHK          1   /* max check item for PLU type 1, 2     */

#define A68_CTRL_NUM       34   /* number of PLU Control Codes          *//* ### MOD Saratoga (33 --> 34)(052300) */
                                /* --> added FoodStamp Bit */

#define A68_DISC_OFS        6   /* start counter for Discount Item 1/2  */
#define A68_DISC_NUM        3   /* counter for Discount Item (ListBox)  */

/* Saratoga */
/*#define A68_NO_LEN          4    Max length of PLU Number             */
#define A68_NO_LEN         STD_PLU_NUMBER_LEN   /* Max length of PLU Number             */
#define A68_PRC_LEN         7   /* Max length of PLU preset price       */
#define A68_BTTL_LEN        3   /* Max length of PLU bonus total index  */
#define A68_RPT_LEN         2   /* Max length of PLU report code        */
/* Saratoga */
/*#define A68_DEPT_LEN        2    Max length of PLU dept               */
#define A68_DEPT_LEN        4   /* Max length of PLU dept               */
#define A68_TBLNO_LEN       2   /* Max length of Table Number           */
#define A68_GRPNO_LEN       2   /* Max length of Group Number           */
#define A68_PRTPRY_LEN      2   /* Max length of Print Priority         */
#define A68_PPI_LEN         3   /* Max length of PPI                    */
/* Saratoga */
#define A68_LINK_LEN        4   /* Max length of Link PLU               */
#define A68_SALES_LEN       1   /* Max length of Sales Code             */
#define A68_MULTI_LEN       2   /* Max length of Price Multiple         */
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
#define A68_FAMILY_LEN      3   /* Max length of Family Code            */
#define A68_COLOR_LEN		3	/* Max length of Color Palette Code		*/
#define A68_TARE_LEN		2	/* Max Length of Tare code */

#define A68_CTRL_BIT        8   /* loop counter for control code check  */
#define A68_DEPT_SFT        6   /* loop counter for dept                */
#define A68_CTRL_0          0   /* PLU control code position ([0])      */
#define A68_CTRL_1          1   /* PLU control code position ([1])      */
#define A68_CTRL_2          2   /* PLU control code position ([2])      */
#define A68_CTRL_3          3   /* PLU control code position ([3])      */
#define A68_CTRL_4          4   /* PLU control code position ([4])      */
#define A68_CTRL_5          5   /* PLU control code position ([5])      */
#define A68_CTRL_6          6   /* PLU control code position ([6])      */
#define A68_CTRL_7          7   /* PLU control code position ([7])      */
#define A68_CTRL_8          8   /* PLU control code position ([8])  Saratoga */
#define A68_CTRL_9          9   /* PLU control code position ([9])  Saratoga */
#define A68_CTRL_10        10   /* PLU control code position ([10]) Saratoga */
#define A68_CTRL_11        11   /* PLU control code position ([11]) Saratoga */

#define A68_DATA_MIN        0
#define A68_DATA_COLORPALETTE_MIN	1 /* Min value for ColorPalette */
/* Saratoga 
#define A68_NO_MAX       9999
*/
#define A68_NO_MAX  9999999999999
#define A68_NO_MIN          1
/*#define A68_DEPT_MAX       50    number of Max dept.                  */
#define A68_DEPT_MAX     9999   /* number of Max dept.                  */
#define A68_DEPT_MIN        1   /* numver of min. dept.                 */
#define A68_RPT_MAX        10   /* number of PLU report code            */
#define A68_BTTL_MAX       100   /* number of Bonus Total Index          */
#define A68_TBLNO_MAX      99   /* number of Table, V3.3 (30->99)       */
#define A68_GRPNO_MAX      90   /* number of Group, R3.1                */
#define A68_PRTPRY_MAX     15   /* number of Print Priority             */
#define A68_PPI_MAX       255   /* number of PPI PDINU                 */
#define A68_COLOR_MAX	  255	/* number of Color Palette code			*/
#define A68_TARE_MAX       30	/* number of Tare table index available	*/
#define A68_PRC_MAX   9999999L
#define A68_BUFF_LEN      128

/* Saratoga */
#define A68_LINK_MAX     9999   /* Max length of Link PLU               */
#define A68_SALES_MAX       7   /* Max length of Sales Code, uchRestrict  */
#define A68_MULTI_MAX      99   /* Max length of Price Multiple         */
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
#define A68_FAMILY_MAX    999   /* Max of Family Code                   */

#define A68_ADJ_NO1      0x01   /* check for adjective PLU number 1     */
#define A68_ADJ_NO2         2   /* adjective PLU number 2               */

#define A68_TYPE_OPEN    0x01   /* PLU type definition                  */
#define A68_TYPE_NONADJ  0x02   /* PLU type definition                  */
#define A68_TYPE_ONEADJ  0x03   /* PLU type definition                  */
#define A68_TYPE_RESET   0x3F   /* PLU type masking code                */

#define A68_RADIX_10       10   /* _ultoa() radix 10                    */

#define A68_TAX_US          0   /* Current Tax Mode is U.S. Tax         */
#define A68_TAX_CAN         1   /* Current Tax Mode is Canadian Tax     */
#define A68_TAX_VAT         2   /* Current Tax Mode is VAT Tax          */

#define A68_DELCONF      0x01

#define A68_SPIN_STEP       1
#define A68_SPIN_TURBO     50
#define A68_SPIN_TURSTEP   10


#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define A68_PLU_LIST_MAX  1000L
#endif

#define A68_ON            1
#define A68_OFF           0
#define A68_PLU_LIST_MAX  30000L
#define A68_ADDPOS        5000L		//TP 12/2/2002 SR # 67 Fixed

//#define A68_ADDPOS          100L	//old modified 12/2/2002

/*
* ===========================================================================
*       PLU Control Code (Status) definitions
* ===========================================================================
*/
#define A68_BIT_CHK      0x01

/* ----- ADDRESS  3 & 4 (Control Code 3'rd byte) ----- */

#define A68_CTRL_BIT1    0xFF

/* -----    ADDRESS  3 (Control Code 3'rd byte)       ----- */
/* ----- 0x01    Plus/ Minus (PLU control code #1)    ----- */
/* ----- 0x02    Normal/ Special                      ----- */
/* ----- 0x04    Not Req./ Request Sup. Intervention  ----- */
/* ----- 0x08    Not Comp./ Compulsory print mod. key ----- */
/* -----    ADDRESS  4 (Control Code 3'rd byte)       ----- */
/* ----- 0x10    Not/ Comp. Prt. mod. or condiment key----- */
/* ----- 0x20    Not print/ Print Validation          ----- */
/* ----- 0x40    Not/ issue single or double receipt  ----- */
/* ----- 0x80    Print single/ Print double receipt   ----- */

/* ----- ADDRESS  5 & 6 (Control Code 4th byte) ----- */

#define A68_CTRL_BIT2    0xC7   /* ### MAKING *//* ### MOD Saratoga (0xC7 -> 0xCF)(052300) */
#define A68_TAX_BIT      0x0f   /* ### CAUTION! (CAN & VAT) */
#define A68_TAX_BIT_VAT  0x03
#define A68_TAX_MAX      0x09   /* ### CAUTION! (CAN & VAT) */
#define A68_TAX_MAX_VAT  0x03
#define A68_TAX_BIT_SERVICE 0x08
#define A68_FOODSTAMP_BIT_US 0x08   /* Food Stamp Bit (US only) *//* ### ADD Saratoga (052300) */
#define A68_WIC_BIT_US 0x40         /* WIC Bit (US only) *//* ### ADD Saratoga (052700) */
#define A68_TYPE_MASK  0x30         /*  (US only) *//* ### ADD Saratoga (052700) */


/* -----    ADDRESS  5 (Control Code 4th byte)        ----- */
/* ----- 0x01    Not/ Affect taxable itemizer #1      ----- */
/* ----- 0x02    Not/ Affect taxable itemizer #2      ----- */
/* ----- 0x04    Not/ Affect taxable itemizer #3      ----- */
/* ----- 0x08    Reserved (Use Canadian Tax)          ----- */
/* -----    ADDRESS  6 (Control Code 4th byte)        ----- */
/* ----- 0x40    Not/ Affect discountable itemizer #1 ----- */
/* ----- 0x80    Not/ Affect discountable itemizer #2 ----- */

/* ----- ADDRESS  7 & 8 (Control Code 5th byte) ----- */

#define A68_CTRL_BIT3    0xFB

/* -----     ADDRESS  7 (Control Code 5th byte)       ----- */
/* ----- 0x01    Prohibit/ Use decimal point key      ----- */
/* ----- 0x02    Normal/ Condiment                    ----- */
/* ----- 0x08    Self control code/ Dept. control code----- */
/* -----     ADDRESS  8 (Control Code 5th byte)       ----- */
/* ----- 0x10    Not/ Send kitchen printer #1         ----- */
/* ----- 0x20    Not/ Send kitchen printer #2         ----- */
/* ----- 0x40    Not/ Send kitchen printer #3         ----- */
/* ----- 0x80    Not/ Send kitchen printer #4         ----- */

/* ----- ADDRESS  9 & 10 (Control Code 6th byte) ----- */

#define A68_CTRL_BIT4    0xF0

/* -----     ADDRESS  9 (Control Code 6th byte)       ----- */
/* ----- Bonus Total index (0 to 8 value)             ----- */
/* -----     ADDRESS 10 (Control Code 6th byte)       ----- */
/* ----- 0x10    Not use/ Use adjective group #1      ----- */
/* ----- 0x20    Not use/ Use adjective group #2      ----- */
/* ----- 0x40    Not use/ Use adjective group #3      ----- */
/* ----- 0x80    Not use/ Use adjective group #4      ----- */

/* ----- ADDRESS 11 & 12 (Control Code 7th byte) ----- */

#define A68_CTRL_BIT5    0xF8
#define A68_ADJVAR_BIT   0x05    /* Implied adjective key variation #5   */

/* -----     ADDRESS 11 (Control Code 7th byte)       ----- */
/* ----- 0x00    Adjective key is compulsory          ----- */
/* ----- 0x01    Implied adjective key variation #1   ----- */
/* ----- 0x02    Implied adjective key variation #2   ----- */
/* ----- 0x03    Implied adjective key variation #3   ----- */
/* ----- 0x04    Implied adjective key variation #4   ----- */
/* ----- 0x05    Implied adjective key variation #5   ----- */
/* -----     ADDRESS 12 (Control Code 7th byte)       ----- */
/* ----- 0x08    Allow/ Prohibit variation #1         ----- */
/* ----- 0x10    Allow/ Prohibit variation #2         ----- */
/* ----- 0x20    Allow/ Prohibit variation #3         ----- */
/* ----- 0x40    Allow/ Prohibit variation #4         ----- */
/* ----- 0x80    Allow/ Prohibit variation #5         ----- */

/* ----- ADDRESS  18 (Control Code 8th byte) ----- */

#define A68_CTRL_BIT6    0x0F


/* -----     ADDRESS  18 (Control Code 8th byte)       ----- */
/* ----- 0x10    Not/ Send kitchen printer #5         ----- */
/* ----- 0x20    Not/ Send kitchen printer #6         ----- */
/* ----- 0x40    Not/ Send kitchen printer #7         ----- */
/* ----- 0x80    Not/ Send kitchen printer #8         ----- */

/* ----- ADDRESS 19 (Control Code 7th byte) ----- */

#define A68_TYPE_BIT   0x0f                       /* Saratoga */

/* ----- 0x00    Non      ----- */
/* ----- 0x01    Type 1   ----- */
/* ----- 0x02    Type 2   ----- */
/* ----- 0x03    Type 3   ----- */

/* ----- ADDRESS 20 (Control Code 7th byte) ----- */
#define A68_CTRL_BIT7    0x8F	

//	0x01	SI and non-zero exception
//	0x02	NOT allow Qty key/ Extention
//	0x04	REQUIRE	QTY key/Extention
//	0x08	Gift Card
//  0x80	Gift Card Reload
#define A68_CTRL_GIFTCARD    0x08    // Gift Card PLU
#define A68_CTRL_EPAYMENT    0x20    // Employee Payroll Deduction, E-Meal (VCS) Plu
#define A68_CTRL_RELOAD      0x80    // if set then Reload operation, other wise Issue operation


//The following are for special bits in this byte for PPI 
//	0x00	->	0 - Allow Both PPI and Combination Coupons
//	0x10	->	1 - Allow only Combination Coupons
//	0x20	->	2 - Allow only PPI
//  0x40    ->  unused but reserved for future PPI

#define A68_CTRL_COUPON		0x10
//#define A68_CTRL_PPI		0x20	Future use


/*
* ===========================================================================
*       structure type definitions for PLU Maintenance
* ===========================================================================
*/
typedef PLUIF   FAR*    LPPLUIF;

/* ----- PLU 1 record type ----- */
//RPH 12/16/02 pack structures
#pragma pack(push, 1)

typedef struct  _A68DATA {
    USHORT  usDummy;
    BYTE    bControl[A68_CTRL_SIZE];  /* control code field           */
    WCHAR    szMnemonic[A68_MNEMO_SIZE + 1];  /* mnemonics field              */
    BYTE    bPrice[A68_PRICE_SIZE];       /* adjective price field        */
} A68DATA, FAR *LPA68DATA;

typedef struct _A68DESC{
    WCHAR    szCtrlOFF[128];
    WCHAR    szCtrlON[128];
} A68DESC, FAR *LPA68DESC;

#pragma pack(pop)

/*
* ===========================================================================
*       Macro difinitions
* ===========================================================================
*/
/* --- some useful language definitions --- */

/*** Saratoga
#define A68GETDEPTNO(x)        ((unsigned char)((unsigned char)x & 0x3f))
#define A68GETPLUTYPE(x)       ((unsigned char)((unsigned char)x >> 6))
***/
#define A68GETDEPTNO(x)        ((unsigned short)((unsigned short)x & 0x3f))
#define A68GETPLUTYPE(x)       ((unsigned short)((unsigned short)x >> 6))

#define A68GETRPTCODE(x)       ((unsigned char)((unsigned char)x & 0x0f))
#define A68GETADJKEY(x)        ((UINT)((unsigned char)x & 0x07))
#define A68GETBONUSIDX(x)      ((UINT)((unsigned char)x & 0x0f))
#define A68SETDEPTNO(x)        ((UINT)((unsigned char)x & 0xc0))

/*#define A68GETTYPEKEY(x)       ((UINT)(((unsigned char)x >> 4) & 0x0f))       /* Saratoga*/
#define A68GETTYPEKEY(x)       ((UINT)(((unsigned char)x >> 4) & 0x03))         /* ### MOD Saratoga (052700) */
/*
* ===========================================================================
*       External work area declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function prototypes
* ===========================================================================
*/
VOID    A068ReadRec(VOID);
BOOL    A068AllocMem(LPHGLOBAL);
VOID    A068InitData(HWND, WPARAM);
VOID    A068SaveChkPrc(HWND, LPDWORD, LPUINT);
VOID    A068SaveRec(HWND, WPARAM);
VOID	A068DisplayArrays(DWORD index, HWND hDlg, LPUINT lpunPluType, LPDWORD lpadwPrePrc);
VOID    A068SetRec(HWND hDlg, int idcListBox, LPUINT lpunPluType, LPDWORD lpadwPrePrc);
VOID    A068ClrRec(HWND);
VOID    A068SetCtrlList(HWND, LPA68DATA);
VOID    A068SetListDesc(HWND, int iListId, PLUIF *pPluData);
VOID    A068RefreshList(HWND, WPARAM, UINT);
BOOL    A068RegistData(HWND, UINT, LPDWORD);
BOOL    A068DeleteData(LPUINT);
BOOL    A068ChkMaxRec(UINT);
VOID    A068DispErr(HWND, LPCWSTR, UINT);
VOID    A068FinDlg(HWND, WPARAM, LPHGLOBAL);
VOID    A068CtrlButton(HWND);
VOID    A068ChkRng(HWND, WORD wEditID);
BOOL    A068ChkDelConf(HWND);
BOOL    A068ChkPluType(HWND, UINT, UINT);
BOOL    A068GetPluNo(HWND, WCHAR*, BOOL);
VOID   A068ResetEditBox(HWND);
VOID	A068ChngPresetState(HWND, UINT); //ESMITH

#if 1
BOOL   A068ResetListDesc(HWND, WPARAM);  /* Saratoga */
VOID   A068ResetDesc(HWND, RECPLU*);
SHORT  A068IntoPluFile(HWND);
#endif

/* ===== End of File (A068.H) ===== */
