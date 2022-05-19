#ifndef __PLU_H
#define __PLU_H
/*==========================================================================*\
*   Title:
*   Category:
*   Program Name:
* ---------------------------------------------------------------------------
*   Abstract:       A common header for Mass Memory APIs
*
* ---------------------------------------------------------------------------
*   Update Histories:
* ---------------------------------------------------------------------------
*   Date    | Version   | Descriptions                          | By
* ---------------------------------------------------------------------------
* 11/08/96  | 2.00.00   | Enhanced to PLU Sequential Report | M.Ozawa
*           |           |                                       |
*** NCR2172
* 11/05/99  | 1.00.00   | Add #pragma(...)                      | K.Iwata
* 11/09/99  |           |                                       | Kato
* 12/28/99  |           | BonusStatus -> ItemType               | K.Iwata
\*==========================================================================*/

/*==========================================================================*\
;+                                                                          +
;+              S Y S T E M A T I C     C O N T R O L                       +
;+                                                                          +
\*==========================================================================*/

#if !defined (COMMENT)
    #define COMMENT                             /* to produce comment lines */
#endif

#if (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*==========================================================================*\
;+                                                                          +
;+                  A P I   T Y P E C O N T R O L                       +
;+                                                                          +
\*==========================================================================*/

#if !defined (PASCAL)                           /* declare PASCAL modifier  */
    #define PASCAL      _pascal
#endif

#if !defined (CDECL)                            /* declare CDECL modifier   */
    #define CDECL       _cdecl
#endif

#if !defined (FAR)                              /* declare FAR modifier     */
    /* #define  FAR     _far *//* ### DEL 2172 rel1.0 */
#endif

#if defined (PEP)                               /* if used as DLL API       */
    #define PLUAPI      FAR _export _loadds
#else                                           /* else, normal API         */
    #define PLUAPI      FAR
#endif

/*==========================================================================*\
;+                                                                          +
;+                  C O M M O N D E F I N I T I O N s                       +
;+                                                                          +
\*==========================================================================*/
#define PLU_NUMBER_LEN              STD_PLU_NUMBER_LEN          /* ### ADD 2172 Rel1.0      */
#define PLU_MNEMONIC_LEN            STD_PLU_MNEMONIC_LEN        /* ### ADD 2172 Rel1.0      */
#define PLU_STATUS_LEN              10          /* ### ADD 2172 Rel1.0   PARA_PLU_STATUS_LEN   */
#define PLU_UNITPRICE_NUM           5           /* ### ADD 2172 Rel1.0 (01/17/00) */

/* --- OLD Type (2170 GP Rel1.0) --- */
#define PLU_NUMBER_LEN_22           7           /* OLD Plu number size      *//* ### ADD 2172 Rel1.0 */
#define PLU_MNEMONIC_LEN_22         16          /* OLD Mnemonic size        *//* ### ADD 2172 Rel1.0 */
#define PLU_STATUS_LEN_22           3           /* OLD Plu status size      *//* ### ADD 2172 Rel1.0 */

/*==========================================================================*\
;+                                                                          +
;+                  E R R O R       C O D E s                               +
;+                                                                          +
\*==========================================================================*/

#define SPLU_COMPLETED              0           /* successfully completed   */
#define SPLU_SYSTEM_FAIL            1000        /* critical error of system */
#define SPLU_FILE_BROKEN            1001        /* file must be broken      */
#define SPLU_INVALID_DATA           1002        /* invalid data given       */
#define SPLU_INVALID_HANDLE         1003        /* invalid handle given     */
#define SPLU_TOO_MANY_USERS         1004        /* too many users accessed  */
#define SPLU_FILE_NOT_FOUND         1005        /* file not found           */
#define SPLU_DEVICE_FULL            1006        /* not enough memory        */
#define SPLU_FILE_OVERFLOW          1007        /* file overflowed          */
#define SPLU_REC_LOCKED             1008        /* record locked            */
#define SPLU_REC_DUPLICATED         1009        /* record duplicated        */
#define SPLU_REC_NOT_FOUND          1010        /* record not found         */
#define SPLU_REC_PROTECTED          1011        /* record protected         */
#define SPLU_REC_NOT_RESERVED       1012        /* record not reserved      */
#define SPLU_ADJ_VIOLATION          1013        /* PLU adjective violation  */
#define SPLU_NOT_UPDATED            1014        /* record not updated       */
#define SPLU_FUNC_CONTINUED         1015        /* function continued       */
#define SPLU_END_OF_FILE            1016        /* end of a file            */
#define SPLU_MNT_UNAPPLY            1017        /* not yet applied          */
#define SPLU_MNT_APPLIED            1018        /* completed to apply       */
#define SPLU_INVALID_ACCESS         1019        /* invalid handle access    */
#define SPLU_FUNC_NOT_AVAIL         1020        /* function not available   */
#define SPLU_FILE_LOCKED            1021        /* file locked              */
#define SPLU_INVALID_FILE           1022        /* invalid file given       */
#define SPLU_FILE_UNLOCKABLE        1023        /* cannot lock a file       */
#define SPLU_FUNC_KILLED            1024        /* function killed by others*/

/*==========================================================================*\
;+                                                                          +
;+                  R E C O R D F I E L D   F O R M A T                     +
;+                                                                          +
\*==========================================================================*/

/* --- record of PLU file --- */

#define PLUID_PLU_NUMBER            1           /* PLU number               */
#define PLUID_ADJECTIVE             2           /* adjective number         */
#define PLUID_DEPT_NUMBER           3           /* department number        */
#define PLUID_UNIT_PRICE            4           /* unit price               */
#define PLUID_PMUL_WEIGHT           5           /* price multiple / weight  */
#define PLUID_MNEMONIC              6           /* mnemonics                */
#define PLUID_STATUS                7           /* status                   */
#define PLUID_FAMILY_CODE           8           /* family code              *//* add 05/13/00 */
#define PLUID_MMATCH_TARE           9           /* mix-match / tare code    */
#define PLUID_REPORT_CODE           10          /* report code              */
#define PLUID_LINK_NUMBER           11          /* link number              */
#define PLUID_RESTRICT              12          /* restriction code         */
#define PLUID_LINK_PPI              13          /* link number to PPI       */
/*#define   PLUID_ITEM_COUNTS       14*/        /* item counter             *//* ### DEL 2172 Rel1.0 */
/*#define   PLUID_SALES_TOTAL       15*/        /* sales total              *//* ### DEL 2172 Rel1.0 */
/*#define   PLUID_DISC_TOTAL        16*/        /* discount totals          *//* ### DEL 2172 Rel1.0 */
#define PLUID_TABLE_NUMBER          14          /* table number             *//* ### ADD 2172 Rel1.0 */
#define PLUID_GROUP_NUMBER          15          /* group number             *//* ### ADD 2172 Rel1.0 */
#define PLUID_PRINT_PRIORITY        16          /* print priority           *//* ### ADD 2172 Rel1.0 */
#define PLUID_BONUS_STATUS          17          /* bonus total/status field, r2.0*//* ### MOD 2172 Rel1.0 (17 -> 16) */
/*#define   PLUID_FQS_VALUE         18*/        /* frequent shopper value, r2.0*//* ### DEL 2172 Rel1.0 */

typedef struct {
	LONG lAmount;
	SHORT sCounter;
}RECINV_TOTAL;// SR167 - CSMALL

typedef struct _RECPLU {                    /* format of PLU record         */
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
    USHORT  usDeptNumber;                       /* department number        *//* ### MOD 2172 rel1.0 (UCHAR->USHORT)*/
    ULONG   ulUnitPrice[PLU_UNITPRICE_NUM];     /* unit price               *//* ### ADD 2172 Rel1.0 (01/17/00) */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[PLU_MNEMONIC_LEN];      /* mnemonics                */
    UCHAR   aucStatus[PLU_STATUS_LEN];          /* status                   */
    USHORT  usFamilyCode;                       /* family code, used with coupon validation  *//* add 05/13/00 */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code, see RECMIX */ /* ### NOT PORTED FROM NCR 2172 GP software version */
    UCHAR   uchReportCode;                      /* report code              */
    USHORT  usLinkNumber;                       /* link number              */
    UCHAR   uchRestrict;                        /* restriction code, index into AC170 table */
    USHORT   uchLinkPPI;                         /* link number to PPI      ESMITH */
    UCHAR   uchTableNumber;                     /* Table No for Order Prompt, index into AC160 OEP table */
    UCHAR   uchGroupNumber;                     /* Group No for Order Prompt, column entry in AC160 OEP table */
    UCHAR   uchPrintPriority;                   /* Print Priority           */
    /*UCHAR uchBonusStatus;*/                   /* bonus total index/status, R2.0 *//* ### DEL 2172 Rel1.0 */
    UCHAR   uchItemType;                        /* item Type                *//* ### ADD 2172 Rel1.0(12.28.99)  */
	USHORT	usBonusIndex;
	// 2.1 Release Changes, SR167 - CSMALL
	ULONG	ulVoidTotalCounter;				// Void Total/Counter for each PLU
	ULONG	ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	ULONG	ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	UCHAR	auchAlphaNumRouting[4];			// Alphanumeric routing character for sending to KDS or Printers
	RECINV_TOTAL	futureTotals[10];			// basic inventory totals (on-hand, receipts/transfers, begin count,
											//   reorder count, current cost, original cost, etc.)
	WCHAR	auchAltPluName[PLU_MNEMONIC_LEN];	// Multilanguage mnemonic for each PLU
	UCHAR	uchColorPaletteCode;			// Color palette code for PLU
	UCHAR	auchStatus2[PLU_STATUS_LEN];
	USHORT  usTareInformation;                 // index into AC111 Tare Table
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
} RECPLU, *PRECPLU;

/* --- OLD Type!!! record of PLU file (2170GP)--- *//* ### ADD 2172 Rel1.0 */

#define PLUID_PLU_NUMBER_22             1           /* PLU number               */
#define PLUID_ADJECTIVE_22              2           /* adjective number         */
#define PLUID_DEPT_NUMBER_22            3           /* department number        */
#define PLUID_UNIT_PRICE_22             4           /* unit price               */
#define PLUID_PMUL_WEIGHT_22            5           /* price multiple / weight  */
#define PLUID_MNEMONIC_22               6           /* mnemonics                */
#define PLUID_STATUS_22                 7           /* status                   */
#define PLUID_MMATCH_TARE_22            8           /* mix-match / tare code    */
#define PLUID_REPORT_CODE_22            9           /* report code              */
#define PLUID_LINK_NUMBER_22            10          /* link number              */
#define PLUID_RESTRICT_22               11          /* restriction code         */
#define PLUID_LINK_PPI_22               12          /* link number to PPI       */
#define PLUID_FAMILY_CODE_22            13          /* family code              */
#define PLUID_ITEM_COUNTS_22            14          /* item counter             */
#define PLUID_SALES_TOTAL_22            15          /* sales total              */
#define PLUID_DISC_TOTAL_22             16          /* discount totals          */
#define PLUID_BONUS_STATUS_22           17          /* bonus total/status field, r2.0*/
#define PLUID_FQS_VALUE_22              18          /* frequent shopper value, r2.0*/

typedef struct _RECPLUR22 {                 /* format of PLU record         */
    UCHAR   aucPluNumber[PLU_NUMBER_LEN_22];    /* PLU number   (7bytes)    */
    UCHAR   uchAdjective;                       /* adjective number         */
    UCHAR   uchDeptNumber;                      /* department number(1byte) */
    ULONG   ulUnitPrice;                        /* unit price               */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    UCHAR   aucMnemonic[PLU_MNEMONIC_LEN_22];   /* mnemonics (16bytes)      */
    UCHAR   aucStatus[PLU_STATUS_LEN_22];       /* status (3bytes)          */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code, see RECMIX */ /* ### NOT PORTED FROM NCR 2172 GP software version */
    UCHAR   uchReportCode;                      /* report code              */
    UCHAR   aucLinkNumber[2];                   /* link number              */
    UCHAR   uchRestrict;                        /* restriction code         */
    UCHAR   uchLinkPPI;                         /* link number to PPI       */
    USHORT  usFamilyCode;                       /* family code              *//* ### DEL 2172 Rel1,0 */
    LONG    lItemCounts;                        /* item counter             *//* ### DEL 2172 Rel1,0 */
    LONG    lSalesTotal;                        /* sales total              *//* ### DEL 2172 Rel1,0 */
    LONG    lDiscTotal;                         /* discount totals          *//* ### DEL 2172 Rel1,0 */
    UCHAR   uchBonusStatus;                     /* bonus total index/status, R2.0 */
    USHORT  usFqsValue;                         /* frequent shopper value, R2.0 *//* ### DEL 2172 Rel1,0 */
} RECPLU22, *PRECPLU22;
/* --- OLD Type!!! record of PLU file (2170GP)--- */


typedef struct _SCNPLU {                    /* format of PLU record at scan */
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
    USHORT  usDeptNumber;                       /* department number        */
    ULONG   ulUnitPrice[PLU_UNITPRICE_NUM];     /* unit price               *//* ### ADD 2172 Rel1.0 (01/17/00) */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[PLU_MNEMONIC_LEN];      /* mnemonics                */
    UCHAR   aucStatus[PLU_STATUS_LEN];          /* status                   */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code, see RECMIX  */ /* ### NOT PORTED FROM NCR 2172 GP software version */
    UCHAR   uchReportCode;                      /* report code              */
    USHORT  usLinkNumber;                       /* link number              */
    UCHAR   uchRestrict;                        /* restriction code         */
    USHORT   uchLinkPPI;                         /* link number to PPI      ESMITH */
    UCHAR   uchTableNumber;                     /* Table No for Order Prompt*//* ### ADD 2172 Rel1.0 */
    UCHAR   uchGroupNumber;                     /* Group No for Order Prompt*//* ### ADD 2172 Rel1.0 */
    UCHAR   uchPrintPriority;                   /* Print Priority           *//* ### ADD 2172 Rel1.0 */
} SCNPLU, *PSCNPLU;

#define PLU_BEFORE_STATUS_LEN       102          /* auchPluNumber[14] +
                                                    uchAdjective +
                                                    usDeptNumber +
                                                    ulUnitPrice[5] +
                                                    uchPriceMulWeight +
                                                    aucMnemonic[20] +
                                                    aucStatus[10] */
                                                
#define PLU_AFTER_STATUS_LEN        10          /*  uchMixMatchTare;    
                                                    uchReportCode;      
                                                    usLinkNumber;       
                                                    uchRestrict;        
                                                    uchLinkPPI;         
                                                    uchTableNumber;     
                                                    uchGroupNumber;     
                                                    uchPrintPriority;   
                                                    uchItemType;        */

/* --- base unit values of PLU's total, amount fields --- */

/* --- contents of status flag --- */
                                            /* flag contents in 1st byte    */
#define PLUST_SV_INTERVENT          1           /* supervisor intervention  */
#define PLUST_PRESET_PRICE          2           /* preset price control */
#define PLUST_SCALE_ITEM            3           /* scalable item            */
#if !defined (COMMENT)
#define PLUST_4TH_1ST               4           /*                          */
#define PLUST_3RD_1ST               5           /*                          */
#define PLUST_2ND_1ST               6           /*                          */
#endif
#define PLUST_SINGLE_ITEM           7           /* single / listing item    */
#define PLUST_COMPUL_SLIP           8           /* compulsory slip          */
                                                /* flag contents in 2nd byte*/
#define PLUST_NOT_AFFECT            9           /* not affect main itemizer */
#define PLUST_COUPON_ITEM           10          /* coupon / regular item    */
#if !defined (COMMENT)
#define PLUST_5TH_2ND               11          /*                          */
#endif
#define PLUST_AFFECT_DISC           12          /* affect disc. itemizer    */
#define PLUST_FOOD_STAMP            13          /* affect food stamp        */
#define PLUST_TAX_ITEM_3            14          /* affect tax. no. 3        */
#define PLUST_TAX_ITEM_2            15          /* affect tax. no. 2        */
#define PLUST_TAX_ITEM_1            16          /* affect tax. no. 1        */
                                            /* flag contents in 3rd byte    */
#if !defined (COMMENT)
#define PLUST_7TH_3RD               17          /*                          */
#define PLUST_6TH_3RD               18          /*                          */
#define PLUST_5TH_3RD               19          /*                          */
#define PLUST_4TH_3RD               20          /*                          */
#define PLUST_3RD_3RD               21          /*                          */
#endif
#define PLUST_PROH_QTY_TARE         22          /* prohibit Q'ty / tare     */
#define PLUST_COMP_QTY_TARE         23          /* compulsory Q'ty / tare   */
#define PLUST_VV_ITEM               24          /* require visual verify    */

#if !defined (COMMENT)
#define PLUST_7TH_4TH               25          /*                          */
#define PLUST_6TH_4TH               26          /*                          */
#define PLUST_5TH_4TH               27          /*                          */
#define PLUST_4TH_4TH               28          /*                          */
#define PLUST_3RD_4TH               29          /*                          */
#define PLUST_2ND_4TH               30          /*                          */
#define PLUST_1ST_4TH               31          /*                          */
#define PLUST_NON_4TH               32          /*                          */
#endif
/* --- macro's for status flag usage --- */

#define OFSFLG(n)       ((n - 1) / 8)           /* byte offset of flag's    */
                                                /* bit position of flag's   */
#define BITFLG(n)       (UCHAR)((UCHAR)0x80 >> ((n - 1) % 8))
                                                /* test a status flag define*/
#define DEFFLG(p, n)    (*(UCHAR *)((UCHAR *)(p) + OFSFLG(n)) & BITFLG(n))

/* --- record of Mix-Match file --- */

// Mix and Match functionality was offered with the NCR 2172 GP version of the NHPOS
// software. NCR 2172 GP was one of several versions of the software which targeted
// grocery stores.
//
// Mix and Match allowed a group of PLUs to be given the same mix-and-match identifier
// or number so that all PLUs with the same identifier would be counted towards a
// quantity which allowed a different unit price to be used for the items.
//
// When NHPOS was ported to the NCR 7448 terminal with Windows CE, the NCR 7448 NHPOS
// version of the software was created by combining NCR 2172 Hospitality and various
// features of NCR 2172 GP and other industry specific versions. The Mix and Match
// functionality was not one of the features selected for porting to the NCR 7448.
// However traces of the functionality exist in the PLU database subsystem and various
// data structs. See FILE_MIX_MATCH in PLU database subsystem.
//
//    Richard Chambers, Sep-26-2018

#define MIXID_MIX_NUMBER            1           /* mix-match number     */
#define MIXID_PRICE_MULTI           2           /* price multiple           */
#define MIXID_UNIT_PRICE            3           /* unit price               */

typedef struct _RECMIX {           /* format of mix-match record, NOT PORTED FROM NCR 2172 GP software version   */
    UCHAR   uchMixMatch;              /* mix-match number, see uchMixMatchTare in RECPLU     */
    UCHAR   uchPriceMulti;            /* price multiple           */
    LONG    lUnitPrice;               /* unit price               */
} RECMIX, *PRECMIX;


/* --- fields of maintenance record --- */

#define MNTID_BATCH_NUMBER          100         /* batch number             */
#define MNTID_TYPE                  101         /* mainte. type             */
#define MNTID_CHANGES               102         /* mainte. change fields    */
#define MNTID_STATUS                103         /* mainte. status           */

#define MNT_TYPE_ADD                1           /* add function             */
#define MNT_TYPE_REPLACE            2           /* replace function         */
#define MNT_TYPE_CHANGE             3           /* change function          */
#define MNT_TYPE_DELETE             4           /* delete function          */

#define CHGFLD(x)   ((ULONG)1L << (x - 1))      /* fields to change         */

/* --- record of PLU maintenance file --- */

typedef struct _MNTPLU {                    /* format of PLU mainte. record */
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
    USHORT  usDeptNumber;                       /* department number        */
    ULONG   ulUnitPrice[PLU_UNITPRICE_NUM];     /* unit price               *//* ### ADD 2172 Rel1.0 (01/17/00) */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[PLU_MNEMONIC_LEN];      /* mnemonics                */
    UCHAR   aucStatus[PLU_STATUS_LEN];          /* status                   */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code, see RECMIX */ /* ### NOT PORTED FROM NCR 2172 GP software version */
    UCHAR   uchReportCode;                      /* report code              */
    USHORT  usLinkNumber;                       /* link number              */
    UCHAR   uchRestrict;                        /* restriction code         */
    UCHAR   uchLinkPPI;                         /* link number to PPI       */
    UCHAR   uchTableNumber;                     /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;                     /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;                   /* Print Priority           */
    UCHAR   uchBonusStatus;                     /* bonus total index/status, R2.0 */
    USHORT  usMntBatch;                         /* mainte. batch number     */
    USHORT  usMntType;                          /* mainte. function type    */
    ULONG   flMntChanges;                       /* mainte. change fields    */
    USHORT  usMntStatus;                        /* mainte. applied status   */
} MNTPLU, *PMNTPLU;

/* --- record of PLU index file R2.0 --- */

#define IDXID_DEPT_NUMBER           1           /* department number        */
#define IDXID_PLU_NUMBER            2           /* PLU number               */
#define IDXID_ADJECTIVE             3           /* adjective number         */

typedef struct _RECIDX {                    /* format of PLU index record   */
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
} RECIDX, *PRECIDX;

/* --- interface format by item number only --- */

typedef union _ITEMNO {                     /* format by item number only   */
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* item number of PLU       */
    UCHAR   uchMixNumber;                       /* item number of mix-match */
} ITEMNO, *PITEMNO;

/*==========================================================================*\
;+                                                                          +
;+                      D E F I N I T I O N s                               +
;+                                                                          +
\*==========================================================================*/

/* --- file IDs --- */

#define FILE_ALL                    0           /* all files                */
#define FILE_PLU                    1           /* PLU file                 */
#define FILE_MIX_MATCH              2           /* mix-match file, see RECMIX, ### NOT PORTED FROM NCR 2172 GP software version   */
/*#define   FILE_PPI                3*/         /* PPI file                 *//* ### DEL 2172 Rel1.0 */
#define FILE_MAINT_PLU              11          /* maintenance PLU file     */
#define FILE_PLU_INDEX              16          /* PLU index file, R2.0     */

/* --- response format of PluSenseFile() --- */

typedef struct _MFINFO {                    /* file information format      */
    USHORT  usFileNo;                           /* file number              */
    ULONG   ulFileSize;                         /* file size in byte        */
    ULONG   ulMaxRec;                           /* no. of max. records      *//* ### MOD 2172 Rel1.0 (100,000item)*/
    ULONG   ulCurRec;                           /* no. of current records   *//* ### MOD 2172 Rel1.0 (100,000item)*/
} MFINFO, *PMFINFO;

/* --- parameters for PluEnterCritMode() --- */

#define FUNC_INDIV_LOCK             1           /* indiv. access with lock  */
#define FUNC_REPORT_RANGE           2           /* report by range          */
#define FUNC_REPORT_ADJECT          3           /* report by adjective item */
#define FUNC_REPORT_COND            4           /* report by conditional    */
#define FUNC_REPORT_MAINTE          5           /* report mainte. records   */
#define FUNC_APPLY_MAINTE           6           /* apply mainte. reocrds    */
#define FUNC_PURGE_MAINTE           7           /* purge mainte. records    */
#define FUNC_BOUND_MAINTE           8           /* bound mainte. records    */
#define FUNC_BLOCK_ACCESS           9           /* block read / write       */
#define FUNC_REPORT_RANGE2          10          /* report by range,compulsory hash */
#define FUNC_REPORT_COND2           11          /* report by conditional,compulsory hash*/
#define FUNC_REPORT_INDEX           12          /* report only index no.    */
#define FUNC_REPORT_R10             13          /* reprot by R1.0 PLU record for migration */

typedef struct _ECMRANGE {                  /* data for report by range     */
    ITEMNO  itemFrom;                           /* item no. to be origin    */
    ITEMNO  itemTo;                             /* item no. to be end       */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMRANGE, *PECMRANGE;

typedef struct _ECMADJECT {             /* data for report adjectives   */
    ITEMNO  itemPluNumber;                      /* item no. of PLU record   */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMADJECT, *PECMADJECT;

typedef struct _ECMCOND {                   /* data for report by condition */
    UCHAR   aucCond[64];                        /* data of conditional exp. */
    USHORT  usLength;                           /* length of cond. exp.     */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMCOND, *PECMCOND;

#define REPORT_ACTIVE               0x0001      /* report active rec.       */
#define REPORT_INACTIVE             0x0002      /* report inactive rec.     */
#define REPORT_WITH_LOCK            0x0004      /* report with lock         */
#define REPORT_AS_RESET             0x0008      /* as reset report          */
#define REPORT_RETRY_LOCKED         0x0010      /* retry locked rec.        */

typedef struct _ECMMAINTE {                 /* data for report maintenance  */
    USHORT  usBatch;                            /* batch number             */
    UCHAR   aucCond[64];                        /* data of conditional exp. */
    USHORT  usLength;                           /* length of cond. exp.     */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMMAINTE, *PECMMAINTE;

typedef struct _ECMAPPLY {                  /* data for apply maintenance   */
    USHORT  usBatch;                            /* batch number             */
    USHORT  fsProg;                             /* programmability          */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMAPPLY, *PECMAPPLY;

typedef struct  _ECMPURGE {                 /* data for purge maintenance   */
    USHORT  usBatch;                            /* batch number             */
    USHORT  fsOption;                           /* options                  */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMPURGE, *PECMPURGE;

#define PURGE_APPLIED               0x0001      /* purge applied mainte. rec*/
#define PURGE_UNAPPLY               0x0002      /* purge unapply mainte. rec*/

typedef struct  _ECMBLOCK {                 /* data for block access        */
    USHORT  fsOption;                           /* options                  */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMBLOCK, *PECMBLOCK;

#define BLOCK_READ                  0x0001      /* read access              */
#define BLOCK_WRITE                 0x0002      /* write access             */

/* --- parameters for PluUpdateRecord() --- */

typedef struct _UPDINF {                    /* update information           */
    USHORT  fsControl;                          /* update control flag      */
    LONG    lSalesQty;                          /* sales quantity           */
    LONG    lSalesAmt;                          /* sales amounts            */
    LONG    lDiscQty;                           /* discount quantity        */
    LONG    lDiscAmt;                           /* discount amounts         */
} UPDINF, *PUPDINF;

#define CNTRL_EXEC_UPDATE           0x0001      /* execute an update        */
#define CNTRL_EXEC_READ             0x0002      /* respond records          */
#define CNTRL_MINUS_UPDATE          0x0004      /* negative operation       */
#define CNTRL_UNCOND_UPDATE         0x0008      /* unconditional update     */
#define CNTRL_WITH_DISCOUNT         0x0010      /* affect disc. to sales    */
#define CNTRL_IGNORE_SV_INT         0x0020      /* ignore supervisor int.   */

#define RETURN_LINK                 0x0001      /* respond link record      */
#define RETURN_MIX_MATCH            0x0002      /* respond mix-match record */
/*#define   RETURN_PPI              0x0004*/    /* respond PPI record       *//* ### DEL 2172 Rel1.0 */
#define INT_SV_REC                  0x0010      /* int. by supervisor int.  */
#define INT_VV_REC                  0x0020      /* int. by visual-verify    */
#define INT_SCALE_REC               0x0040      /* int. by scalable item    */
#define INT_MISC_REC                0x0080      /* int. by un-updatable     */
#define INT_WRONG_LINK              0x0100      /* int. by wrong link rec   */
#define MISSING_LINK                0x1000      /* missing link rec.        */
/*#define   MISSING_PPI             0x2000*/    /* missing PPI rec.         *//* ### DEL 2172 Rel1.0 */
#define MISSING_MIX_MATCH           0x4000      /* missing mix-match rec.   */

                                                /* buffer size for response */
#define PLU_UPDATE_BUFFER       \
                (USHORT)(sizeof(SCNPLU) * 2 + sizeof(RECMIX) + /*sizeof(RECPPI)*/)

/* --- seek origin point --- */

#define PLU_SB_CUR                  1           /* from current position    */
#define PLU_SB_TOP                  2           /* from top of file         */
#define PLU_SB_END                  3           /* from end of file         */

/* --- programmabilities --- */

#define PROG_PROTECT_CHANGE         0x0001      /* protect against active   */
#define PROG_PROTECT_DELETE         0x0002      /* protect against active   */
#define PROG_RESET_TOTALS           0x0004      /* reset total fields       */
#define PROG_SET_VV_FLAG            0x0008      /* set V-V flag             */

/* --- conditional expressions --- */

#define COMP_MASK                   0x0F        /* mask data for comp. data */
#define COMP_EQUAL                  0x01        /* A == B, equal            */
#define COMP_NOT_EQUAL              0x02        /* A != B, not equal        */
#define COMP_LESS_THAN              0x03        /* A    < B, less than      */
#define COMP_GREATER_THAN           0x04        /* A    > B, greater than   */
#define COMP_LESS_EQUAL             0x05        /* A <= B, less than or eq. */
#define COMP_GREATER_EQUAL          0x06        /* A >= B, greater or equal */
#define COMP_BIT_TRUE               0x07        /*  A & B,  bit true        */
#define COMP_BIT_FALSE              0x08        /* ! (A & B), bit false     */

#define LOGIC_MASK                  0xF0        /* mask data for logic data */
#define LOGIC_AND                   0x10        /* EX1 && EX2, and          */
#define LOGIC_OR                    0x20        /* EX1 || EX2, or           */
#define LOGIC_END                   0x40        /* end of expression        */

/* --- max. dept definitions for index file control --- */

#define MAX_DEPT                    99

/*==========================================================================*\
                    PLU Module Status       R1.1
\*==========================================================================*/
typedef struct _PLU_STATUS {
    UCHAR   fchControl;                         /* control flags            */
    UCHAR   uchFunc;                            /* function number          */
    USHORT  usFile;                             /* file ID accessing        */
    USHORT  fsOption;                           /* function optional data   */
} PLU_STATUS;



/*==========================================================================*\
;+                                                                          +
;+          P R O T O T Y P E   D E C L A R A T I O N s                     +
;+                                                                          +
\*==========================================================================*/

/* --- System Functions --- */

VOID PLUAPI PluInitialize(VOID);                /* initialize a module      */
VOID PLUAPI PluFinalize(VOID);                  /* finalize a module        */
VOID PLUAPI PluRefresh(VOID);                   /* refresh a module         */

/* --- File Access Functions --- */

USHORT PLUAPI PluCreateFile(USHORT, ULONG);     /* create a file            *//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT PLUAPI PluDeleteFile(USHORT);            /* delete a file            */
USHORT PLUAPI PluClearFile(USHORT);             /* clear contents of a file */
USHORT PLUAPI PluSenseFile(USHORT, PMFINFO);    /* get info. of a file      */
                                                /* enter a critical mode    */
USHORT PLUAPI PluEnterCritMode(USHORT, USHORT, USHORT *, VOID *);
USHORT PLUAPI PluExitCritMode(USHORT, USHORT);  /* exit from critical mode  */
USHORT PLUAPI PluKillCritMode(USHORT, USHORT);  /* kill critical mode users */

/* --- Record Access Functions --- */

USHORT PLUAPI PluAddRecord(USHORT, VOID *);     /* add a record             */
                                                /* replace a record         */
USHORT PLUAPI PluReplaceRecord(USHORT, VOID *, VOID *, USHORT *);
                                                /* change a record          */
USHORT PLUAPI PluChangeRecord(USHORT, VOID *, ULONG, USHORT, VOID *, USHORT *);
                                                /* delete a record          */
USHORT PLUAPI PluDeleteRecord(USHORT, VOID *, USHORT, USHORT, VOID *, USHORT *);
USHORT PLUAPI PluDeleteReserved(USHORT);        /* delete a reserved record */
USHORT PLUAPI PluResetReserved(USHORT);         /* reset a reserved record  */
                                                /* rewrite a reserved record*/
USHORT PLUAPI PluRewriteReserved(USHORT, VOID *);
USHORT PLUAPI PluCancelReserved(USHORT);        /* cancel a reserved record */

/* --- Maintenance Record Access Functions --- */

USHORT PLUAPI PluAppendMainte(USHORT, VOID *);  /* append a mainte. record  */
USHORT PLUAPI PluPurgeMainte(USHORT);           /* purge mainte. records    */
                                                /* apply mainte. records    */
USHORT PLUAPI PluApplyMainte(USHORT, VOID *, USHORT *, USHORT *);
                                                /* apply mainte. records    */
USHORT PLUAPI PluApplyMainte2(USHORT, VOID *, USHORT *, VOID *, USHORT *);

/* --- Associated Maintenance Record Functions --- */
                                                /* get associated records   */
USHORT PLUAPI PluReportMainte(USHORT, VOID *, ULONG *, USHORT *);               /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT PLUAPI PluRemoveMainte(USHORT, ULONG);   /* remove mainte. records   */  /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
                                                /* modify mainte. records   */
USHORT PLUAPI PluModifyMainte(USHORT, ULONG, VOID *);
                                                /* read mainte. records     */
USHORT PLUAPI PluReadMainte(USHORT, ULONG, VOID *, USHORT *);
                                                /* add mainte. records      */
USHORT PLUAPI PluAddMainte(USHORT, VOID *, ULONG *);    /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

/* --- Record Report Functions --- */
                                                /* read a record            */
USHORT PLUAPI PluReadRecord(USHORT usFile, PITEMNO pItemNumber, USHORT usAdj, VOID *pvBuffer, USHORT *pusRet);
                                                /* read a record w/ lock    */
USHORT PLUAPI PluReserveRecord(USHORT, PITEMNO, USHORT, VOID *, USHORT *);
                                                /* report records           */
USHORT PLUAPI PluReportRecord(USHORT, VOID *, USHORT *);

/* --- Update Record Functions --- */

USHORT PLUAPI                                   /* update a record          */
PluUpdateRecord(USHORT, PITEMNO, USHORT, PUPDINF, VOID *, USHORT *);
USHORT PLUAPI                                   /* update a record          */
PluUpdateRecord2(USHORT, PITEMNO, USHORT, PUPDINF, PUPDINF, VOID *, USHORT *);

/* --- file access in block unit --- */
                                                /* read from a file         */
USHORT PLUAPI PluReadBlock(USHORT, UCHAR *, ULONG, ULONG *);
                                                /* write to a file          */
USHORT PLUAPI PluWriteBlock(USHORT, UCHAR *, USHORT, USHORT *);
                                                /* change file ptr.         */
USHORT PLUAPI PluSeekBlock(USHORT, LONG, USHORT, ULONG *);

/* --- mirror file functions --- */
                                                /* open a mirror file       */
USHORT PLUAPI PluOpenMirror(WCHAR FAR *, USHORT *);
USHORT PLUAPI PluOpenMirror2(WCHAR FAR *, WCHAR FAR *, USHORT *);
USHORT PLUAPI PluCloseMirror(USHORT);           /* close a mirror file      */
USHORT PLUAPI PluInformStatus(USHORT, PLU_STATUS *);            /* R1.1 */

#if (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/* ************************************************************************ */


#endif  /* __PLU_H */