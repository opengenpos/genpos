/****************************************************************************\
||                                                                          ||
||        *=*=*=*=*=*=*=*=*                                                 ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO           ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994              ||
||    <|\/~               ~\/|>                                             ||
||   _/^\_                 _/^\_                                            ||
||                                                                          ||
\****************************************************************************/

/*==========================================================================*\
*   Title:
*   Category:
*   Program Name:
* ---------------------------------------------------------------------------
*   Compile:        MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model:   Medium Model
*   Options:        /c /AM /Gs /Os /Za /Zp /W4
* ---------------------------------------------------------------------------
*   Abstract:       A common header for Mass Memory APIs
*
* ---------------------------------------------------------------------------
*   Update Histories:
* ---------------------------------------------------------------------------
*   Date     | Version  | Descriptions                          | By
* ---------------------------------------------------------------------------
* 11/08/96   | 2.00.00  | Enhanced to PLU Sequential Report     | M.Ozawa
*            |          |                                       |
\*==========================================================================*/

/*==========================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:   $Revision$
:   $Date$
:   $Author$
:   $Log$
\*==========================================================================*/


/*==========================================================================*\
;+                                                                          +
;+              S Y S T E M A T I C      C O N T R O L                      +
;+                                                                          +
\*==========================================================================*/

/*  RJC - pragma pack missing but similar structs in pludefin.h in PLU are packed*/
#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#if !defined (COMMENT)
    #define     COMMENT                         /* to produce comment lines */
#endif

/*==========================================================================*\
;+                                                                          +
;+                  A P I    T Y P E    C O N T R O L                       +
;+                                                                          +
\*==========================================================================*/


/*==========================================================================*\
;+                                                                          +
;+                  E R R O R       C O D E s                               +
;+                                                                          +
\*==========================================================================*/

#define     SPLU_COMPLETED             0        /* successfully completed   */
#define     SPLU_SYSTEM_FAIL        1000        /* critical error of system */
#define     SPLU_FILE_BROKEN        1001        /* file must be broken      */
#define     SPLU_INVALID_DATA       1002        /* invalid data given       */
#define     SPLU_INVALID_HANDLE     1003        /* invalid handle given     */
#define     SPLU_TOO_MANY_USERS     1004        /* too many users accessed  */
#define     SPLU_FILE_NOT_FOUND     1005        /* file not found           */
#define     SPLU_DEVICE_FULL        1006        /* not enough memory        */
#define     SPLU_FILE_OVERFLOW      1007        /* file overflowed          */
#define     SPLU_REC_LOCKED         1008        /* record locked            */
#define     SPLU_REC_DUPLICATED     1009        /* record duplicated        */
#define     SPLU_REC_NOT_FOUND      1010        /* record not found         */
#define     SPLU_REC_PROTECTED      1011        /* record protected         */
#define     SPLU_REC_NOT_RESERVED   1012        /* record not reserved      */
#define     SPLU_ADJ_VIOLATION      1013        /* PLU adjective violation  */
#define     SPLU_NOT_UPDATED        1014        /* record not updated       */
#define     SPLU_FUNC_CONTINUED     1015        /* function continued       */
#define     SPLU_END_OF_FILE        1016        /* end of a file            */
#define     SPLU_MNT_UNAPPLY        1017        /* not yet applied          */
#define     SPLU_MNT_APPLIED        1018        /* completed to apply       */
#define     SPLU_INVALID_ACCESS     1019        /* invalid handle access    */
#define     SPLU_FUNC_NOT_AVAIL     1020        /* function not available   */
#define     SPLU_FILE_LOCKED        1021        /* file locked              */
#define     SPLU_INVALID_FILE       1022        /* invalid file given       */
#define     SPLU_FILE_UNLOCKABLE    1023        /* cannot lock a file       */
#define     SPLU_FUNC_KILLED        1024        /* function killed by others*/

/*==========================================================================*\
;+                                                                          +
;+                  R E C O R D   F I E L D   F O R M A T                   +
;+                                                                          +
\*==========================================================================*/

/* --- record of PLU file --- */

#define     PLUID_PLU_NUMBER         1          /* PLU number               */
#define     PLUID_ADJECTIVE          2          /* adjective number         */
#define     PLUID_DEPT_NUMBER        3          /* department number        */
#define     PLUID_UNIT_PRICE         4          /* unit price               */
#define     PLUID_PMUL_WEIGHT        5          /* price multiple / weight  */
#define     PLUID_MNEMONIC           6          /* mnemonics                */
#define     PLUID_STATUS             7          /* status                   */
#define     PLUID_MMATCH_TARE        8          /* mix-match / tare code    */
#define     PLUID_REPORT_CODE        9          /* report code              */
#define     PLUID_LINK_NUMBER       10          /* link number              */
#define     PLUID_RESTRICT          11          /* restriction code         */
#define     PLUID_LINK_PPI          12          /* link number to PPI       */
#define     PLUID_FAMILY_CODE       13          /* family code              */
#define     PLUID_ITEM_COUNTS       14          /* item counter             */
#define     PLUID_SALES_TOTAL       15          /* sales total              */
#define     PLUID_DISC_TOTAL        16          /* discount totals          */
#define     PLUID_BONUS_STATUS      17          /* bonus total/status field, r2.0*/
#define     PLUID_FQS_VALUE         18          /* frequent shopper value, r2.0*/

typedef struct _RECPLU {                    /* format of PLU record         */
    WCHAR   aucPluNumber[7];                    /* PLU number               */
    WCHAR   uchAdjective;                       /* adjective number         */
    WCHAR   uchDeptNumber;                      /* department number        */
    ULONG   ulUnitPrice;                        /* unit price               */
    WCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[16];                    /* mnemonics                */
    WCHAR   aucStatus[3];                       /* status                   */
    WCHAR   uchMixMatchTare;                    /* mix-match / tare code    */
    WCHAR   uchReportCode;                      /* report code              */
    WCHAR   aucLinkNumber[2];                   /* link number              */
    WCHAR   uchRestrict;                        /* restriction code         */
    WCHAR   uchLinkPPI;                         /* link number to PPI       */
    USHORT  usFamilyCode;                       /* family code              */
    LONG    lItemCounts;                        /* item counter             */
    LONG    lSalesTotal;                        /* sales total              */
    LONG    lDiscTotal;                         /* discount totals          */
    WCHAR   uchBonusStatus;                     /* bonus total index/status, R2.0 */
    USHORT  usFqsValue;                         /* frequent shopper value, R2.0 */
} RECPLU, *PRECPLU;

typedef struct _RECPLUR10 {                 /* format of PLU record(R1.0)   */
    UCHAR   aucPluNumber[7];                    /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
    UCHAR   uchDeptNumber;                      /* department number        */
    ULONG   ulUnitPrice;                        /* unit price               */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    UCHAR   aucMnemonic[16];                    /* mnemonics                */
    UCHAR   aucStatus[3];                       /* status                   */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code    */
    UCHAR   uchReportCode;                      /* report code              */
    UCHAR   aucLinkNumber[2];                   /* link number              */
    UCHAR   uchRestrict;                        /* restriction code         */
    UCHAR   uchLinkPPI;                         /* link number to PPI       */
    USHORT  usFamilyCode;                       /* family code              */
    LONG    lItemCounts;                        /* item counter             */
    LONG    lSalesTotal;                        /* sales total              */
    LONG    lDiscTotal;                         /* discount totals          */
} RECPLU10, *PRECPLU10;

typedef struct _SCNPLU {                    /* format of PLU record at scan */
    WCHAR   aucPluNumber[7];                    /* PLU number               */
    WCHAR   uchAdjective;                       /* adjective number         */
    WCHAR   uchDeptNumber;                      /* department number        */
    ULONG   ulUnitPrice;                        /* unit price               */
    WCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[16];                    /* mnemonics                */
    WCHAR   aucStatus[4];                       /* status            R2.0   */
    WCHAR   uchMixMatchTare;                    /* mix-match / tare code    */
    WCHAR   uchReportCode;                      /* report code              */
    WCHAR   aucLinkNumber[2];                   /* link number              */
    WCHAR   uchRestrict;                        /* restriction code         */
    WCHAR   uchLinkPPI;                         /* link number to PPI       */
    USHORT  usFamilyCode;                       /* family code              */
    USHORT  usFQSValue;                         /* frequent shopper value, R2.0 */
} SCNPLU, *PSCNPLU;

#define PLU_BEFORE_STATUS_LEN   33  /* auchPluNumber[7] +
                                        uchAdjective +
                                        uchDeptNumber +
                                        ulUnitPrice +
                                        uchPriceMulWeight +
                                        aucMnemonic[16] +
                                        aucStatus[4] - 1 */

#define PLU_AFTER_STATUS_LEN    8   /* uchMixMatchTae +
                                        uchReportCode +
                                        aucLinkNumber[2] +
                                        uchRestrict +
                                        uchLinkPPI +
                                        usFamilyCode +
                                        usFQSValue - usFQSValue */

/* --- base unit values of PLU's total, amount fields --- */

#define     PLU_BASE_UNIT           1000L       /* base unit value          */

/* --- contents of status flag --- */
                                            /* flag contents in 1st byte    */
#define     PLUST_SV_INTERVENT       1          /* supervisor intervention  */
#define     PLUST_PRESET_PRICE       2          /* preset price control     */
#define     PLUST_SCALE_ITEM         3          /* scalable item            */
#if !defined (COMMENT)
#define     PLUST_4TH_1ST            4          /*                          */
#define     PLUST_3RD_1ST            5          /*                          */
#define     PLUST_2ND_1ST            6          /*                          */
#endif
#define     PLUST_SINGLE_ITEM        7          /* single / listing item    */
#define     PLUST_COMPUL_SLIP        8          /* compulsory slip          */
                                            /* flag contents in 2nd byte    */
#define     PLUST_NOT_AFFECT         9          /* not affect main itemizer */
#define     PLUST_COUPON_ITEM       10          /* coupon / regular item    */
#if !defined (COMMENT)
#define     PLUST_5TH_2ND           11          /*                          */
#endif
#define     PLUST_AFFECT_DISC       12          /* affect disc. itemizer    */
#define     PLUST_FOOD_STAMP        13          /* affect food stamp        */
#define     PLUST_TAX_ITEM_3        14          /* affect tax. no. 3        */
#define     PLUST_TAX_ITEM_2        15          /* affect tax. no. 2        */
#define     PLUST_TAX_ITEM_1        16          /* affect tax. no. 1        */
                                            /* flag contents in 3rd byte    */
#if !defined (COMMENT)
#define     PLUST_7TH_3RD           17          /*                          */
#define     PLUST_6TH_3RD           18          /*                          */
#define     PLUST_5TH_3RD           19          /*                          */
#define     PLUST_4TH_3RD           20          /*                          */
#define     PLUST_3RD_3RD           21          /*                          */
#endif
#define     PLUST_PROH_QTY_TARE     22          /* prohibit Q'ty / tare     */
#define     PLUST_COMP_QTY_TARE     23          /* compulsory Q'ty / tare   */
#define     PLUST_VV_ITEM           24          /* require visual verify    */

#if !defined (COMMENT)
#define     PLUST_7TH_4TH           25          /*                          */
#define     PLUST_6TH_4TH           26          /*                          */
#define     PLUST_5TH_4TH           27          /*                          */
#define     PLUST_4TH_4TH           28          /*                          */
#define     PLUST_3RD_4TH           29          /*                          */
#define     PLUST_2ND_4TH           30          /*                          */
#define     PLUST_1ST_4TH           31          /*                          */
#define     PLUST_NON_4TH           32          /*                          */
#endif
/* --- macro's for status flag usage --- */

#define     OFSFLG(n)       ((n - 1) / 8)       /* byte offset of flag's    */
                                                /* bit position of flag's   */
#define     BITFLG(n)       (UCHAR)((UCHAR)0x80 >> ((n - 1) % 8))
                                                /* test a status flag define*/
#define     DEFFLG(p, n)    (*(UCHAR *)((UCHAR *)(p) + OFSFLG(n)) & BITFLG(n))

/* --- record of Mix-Match file --- */

#define     MIXID_MIX_NUMBER         1          /* mix-match number         */
#define     MIXID_PRICE_MULTI        2          /* price multiple           */
#define     MIXID_UNIT_PRICE         3          /* unit price               */

typedef struct _RECMIX {                    /* format of mix-match record   */
    WCHAR   uchMixMatch;                        /* mix-match number         */
    WCHAR   uchPriceMulti;                      /* price multiple           */
    LONG    lUnitPrice;                         /* unit price               */
} RECMIX, *PRECMIX;

/* --- record of PPI file --- */

#define     PPIID_PPI_CODE           1          /* PPI number               */
#define     PPIID_NO_OF_LEVEL        2          /* no. of levels            */
#define     PPIID_LEVEL1             3          /* price level 1            */
#define     PPIID_PRICE1             4          /* price 1                  */
#define     PPIID_LEVEL2             5          /* price level 2            */
#define     PPIID_PRICE2             6          /* price 2                  */
#define     PPIID_LEVEL3             7          /* price level 3            */
#define     PPIID_PRICE3             8          /* price 3                  */
#define     PPIID_LEVEL4             9          /* price level 4            */
#define     PPIID_PRICE4            10          /* price 4                  */
#define     PPIID_LEVEL5            11          /* price level 5            */
#define     PPIID_PRICE5            12          /* price 5                  */
#define     PPIID_LEVEL6            13          /* price level 6            */
#define     PPIID_PRICE6            14          /* price 6                  */
#define     PPIID_LEVEL7            15          /* price level 7            */
#define     PPIID_PRICE7            16          /* price 7                  */
#define     PPIID_LEVEL8            17          /* price level 8            */
#define     PPIID_PRICE8            18          /* price 8                  */
#define     PPIID_LEVEL9            19          /* price level 9            */
#define     PPIID_PRICE9            20          /* price 9                  */
#define     PPIID_LEVEL10           21          /* price level 10           */
#define     PPIID_PRICE10           22          /* price 10                 */
#define     PPIID_LEVEL11           23          /* price level 1,   V2.1    */
#define     PPIID_PRICE11           24          /* price 1                  */
#define     PPIID_LEVEL12           25          /* price level 2            */
#define     PPIID_PRICE12           26          /* price 2                  */
#define     PPIID_LEVEL13           27          /* price level 3            */
#define     PPIID_PRICE13           28          /* price 3                  */
#define     PPIID_LEVEL14           29          /* price level 4            */
#define     PPIID_PRICE14           30          /* price 4                  */
#define     PPIID_LEVEL15           31          /* price level 5            */
#define     PPIID_PRICE15           32          /* price 5                  */
#define     PPIID_LEVEL16           33          /* price level 6            */
#define     PPIID_PRICE16           34          /* price 6                  */
#define     PPIID_LEVEL17           35          /* price level 7            */
#define     PPIID_PRICE17           36          /* price 7                  */
#define     PPIID_LEVEL18           37          /* price level 8            */
#define     PPIID_PRICE18           38          /* price 8                  */
#define     PPIID_LEVEL19           39          /* price level 9            */
#define     PPIID_PRICE19           40          /* price 9                  */
#define     PPIID_LEVEL20           41          /* price level 10           */
#define     PPIID_PRICE20           42          /* price 10                 */

typedef struct _RECPPIOLD {                    /* format of PPI record         */
    UCHAR   uchPPI;                             /* PPI code                 */
    UCHAR   uchNoOfLevel;                       /* no. of levels            */
    UCHAR   uchLevel1;                          /* price level 1            */
    ULONG   ulPrice1;                           /* price 1                  */
    UCHAR   uchLevel2;                          /* price level 2            */
    ULONG   ulPrice2;                           /* price 2                  */
    UCHAR   uchLevel3;                          /* price level 3            */
    ULONG   ulPrice3;                           /* price 3                  */
    UCHAR   uchLevel4;                          /* price level 4            */
    ULONG   ulPrice4;                           /* price 4                  */
    UCHAR   uchLevel5;                          /* price level 5            */
    ULONG   ulPrice5;                           /* price 5                  */
    UCHAR   uchLevel6;                          /* price level 6            */
    ULONG   ulPrice6;                           /* price 6                  */
    UCHAR   uchLevel7;                          /* price level 7            */
    ULONG   ulPrice7;                           /* price 7                  */
    UCHAR   uchLevel8;                          /* price level 8            */
    ULONG   ulPrice8;                           /* price 8                  */
    UCHAR   uchLevel9;                          /* price level 9            */
    ULONG   ulPrice9;                           /* price 9                  */
    UCHAR   uchLevel10;                         /* price level 10           */
    ULONG   ulPrice10;                          /* price 10                 */
} RECPPIOLD, *PRECPPIOLD;

typedef struct _RECPPI {                    /* format of PPI record         */
    WCHAR   uchPPI;                             /* PPI code                 */
    UCHAR   uchNoOfLevel;                       /* no. of levels            */
    UCHAR   uchLevel1;                          /* price level 1            */
    ULONG   ulPrice1;                           /* price 1                  */
    UCHAR   uchLevel2;                          /* price level 2            */
    ULONG   ulPrice2;                           /* price 2                  */
    UCHAR   uchLevel3;                          /* price level 3            */
    ULONG   ulPrice3;                           /* price 3                  */
    UCHAR   uchLevel4;                          /* price level 4            */
    ULONG   ulPrice4;                           /* price 4                  */
    UCHAR   uchLevel5;                          /* price level 5            */
    ULONG   ulPrice5;                           /* price 5                  */
    UCHAR   uchLevel6;                          /* price level 6            */
    ULONG   ulPrice6;                           /* price 6                  */
    UCHAR   uchLevel7;                          /* price level 7            */
    ULONG   ulPrice7;                           /* price 7                  */
    UCHAR   uchLevel8;                          /* price level 8            */
    ULONG   ulPrice8;                           /* price 8                  */
    UCHAR   uchLevel9;                          /* price level 9            */
    ULONG   ulPrice9;                           /* price 9                  */
    UCHAR   uchLevel10;                         /* price level 10           */
    ULONG   ulPrice10;                          /* price 10                 */
    UCHAR   uchLevel11;                         /* price level 1,   V2.1    */
    ULONG   ulPrice11;                          /* price 1                  */
    UCHAR   uchLevel12;                         /* price level 2            */
    ULONG   ulPrice12;                          /* price 2                  */
    UCHAR   uchLevel13;                         /* price level 3            */
    ULONG   ulPrice13;                          /* price 3                  */
    UCHAR   uchLevel14;                         /* price level 4            */
    ULONG   ulPrice14;                          /* price 4                  */
    UCHAR   uchLevel15;                         /* price level 5            */
    ULONG   ulPrice15;                          /* price 5                  */
    UCHAR   uchLevel16;                         /* price level 6            */
    ULONG   ulPrice16;                          /* price 6                  */
    UCHAR   uchLevel17;                         /* price level 7            */
    ULONG   ulPrice17;                          /* price 7                  */
    UCHAR   uchLevel18;                         /* price level 8            */
    ULONG   ulPrice18;                          /* price 8                  */
    UCHAR   uchLevel19;                         /* price level 9            */
    ULONG   ulPrice19;                          /* price 9                  */
    UCHAR   uchLevel20;                         /* price level 10           */
    ULONG   ulPrice20;                          /* price 10                 */
} RECPPI, *PRECPPI;

/* --- fields of maintenance record --- */

#define     MNTID_BATCH_NUMBER      100         /* batch number             */
#define     MNTID_TYPE              101         /* mainte. type             */
#define     MNTID_CHANGES           102         /* mainte. change fields    */
#define     MNTID_STATUS            103         /* mainte. status           */

#define     MNT_TYPE_ADD              1         /* add function             */
#define     MNT_TYPE_REPLACE          2         /* replace function         */
#define     MNT_TYPE_CHANGE           3         /* change function          */
#define     MNT_TYPE_DELETE           4         /* delete function          */

#define     CHGFLD(x)   ((ULONG)1L << (x - 1))  /* fields to change         */

/* --- record of PLU maintenance file --- */

typedef struct _MNTPLU {                    /* format of PLU mainte. record */
    WCHAR   aucPluNumber[7];                    /* PLU number               */
    WCHAR   uchAdjective;                       /* adjective number         */
    WCHAR   uchDeptNumber;                      /* department number        */
    ULONG   ulUnitPrice;                        /* unit price               */
    WCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[16];                    /* mnemonics                */
    WCHAR   aucStatus[3];                       /* status                   */
    WCHAR   uchMixMatchTare;                    /* mix-match / tare code    */
    WCHAR   uchReportCode;                      /* report code              */
    WCHAR   aucLinkNumber[2];                   /* link number              */
    WCHAR   uchRestrict;                        /* restriction code         */
    WCHAR   uchLinkPPI;                         /* link number to PPI       */
    USHORT  usFamilyCode;                       /* family code              */
    LONG    lItemCounts;                        /* item counter             */
    LONG    lSalesTotal;                        /* sales total              */
    LONG    lDiscTotal;                         /* discount totals          */
    WCHAR   uchBonusStatus;                     /* bonus total index/status, R2.0 */
    USHORT  usFqsValue;                         /* frequent shopper value, R2.0 */
    USHORT  usMntBatch;                         /* mainte. batch number     */
    USHORT  usMntType;                          /* mainte. function type    */
    ULONG   flMntChanges;                       /* mainte. change fields    */
    USHORT  usMntStatus;                        /* mainte. applied status   */
} MNTPLU, *PMNTPLU;

/* --- record of PLU index file R2.0 --- */

#define     IDXID_DEPT_NUMBER        1          /* department number        */
#define     IDXID_PLU_NUMBER         2          /* PLU number               */
#define     IDXID_ADJECTIVE          3          /* adjective number         */

typedef struct _RECIDX {                    /* format of PLU index record   */
    WCHAR   uchDeptNumber;                      /* department number        */
    WCHAR   aucPluNumber[7];                    /* PLU number               */
    WCHAR   uchAdjective;                       /* adjective number         */
} RECIDX, *PRECIDX;

/* --- interface format by item number only --- */

typedef union _ITEMNO {                     /* format by item number only   */
    WCHAR   aucPluNumber[7];                    /* item number of PLU       */
    WCHAR   uchMixNumber;                       /* item number of mix-match */
    WCHAR   uchPpiNumber;                       /* item number of PPI       */
} ITEMNO, *PITEMNO;

/*==========================================================================*\
;+                                                                          +
;+                      D E F I N I T I O N s                               +
;+                                                                          +
\*==========================================================================*/

/* --- file IDs --- */

#define     FILE_ALL             0              /* all files                */
#define     FILE_PLU             1              /* PLU file                 */
#define     FILE_MIX_MATCH       2              /* mix-match file           */
#define     FILE_PPI             3              /* PPI file                 */
#define     FILE_MAINT_PLU      11              /* maintenance PLU file     */
#define     FILE_PLU_INDEX      16              /* PLU index file, R2.0     */

/* --- response format of PluSenseFile() --- */

typedef struct _MFINFO {                    /* file information format      */
    USHORT  usFileNo;                           /* file number              */
    ULONG   ulFileSize;                         /* file size in byte        */
    USHORT  usMaxRec;                           /* no. of max. records      */
    USHORT  usCurRec;                           /* no. of current records   */
} MFINFO, *PMFINFO;

/* --- parameters for PluEnterCritMode() --- */

#define     FUNC_INDIV_LOCK         1           /* indiv. access with lock  */
#define     FUNC_REPORT_RANGE       2           /* report by range          */
#define     FUNC_REPORT_ADJECT      3           /* report by adjective item */
#define     FUNC_REPORT_COND        4           /* report by conditional    */
#define     FUNC_REPORT_MAINTE      5           /* report mainte. records   */
#define     FUNC_APPLY_MAINTE       6           /* apply mainte. reocrds    */
#define     FUNC_PURGE_MAINTE       7           /* purge mainte. records    */
#define     FUNC_BOUND_MAINTE       8           /* bound mainte. records    */
#define     FUNC_BLOCK_ACCESS       9           /* block read / write       */
#define     FUNC_REPORT_RANGE2     10           /* report by range,compulsory hash */
#define     FUNC_REPORT_COND2      11           /* report by conditional,compulsory hash*/
#define     FUNC_REPORT_INDEX      12           /* report only index no. */
#define     FUNC_REPORT_R10        13           /* reprot by R1.0 PLU record for migration */

typedef struct _ECMRANGE {                  /* data for report by range     */
    ITEMNO  itemFrom;                           /* item no. to be origin    */
    ITEMNO  itemTo;                             /* item no. to be end       */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMRANGE, *PECMRANGE;

typedef struct _ECMADJECT {                 /* data for report adjectives   */
    ITEMNO  itemPluNumber;                      /* item no. of PLU record   */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMADJECT, *PECMADJECT;

typedef struct _ECMCOND {                   /* data for report by condition */
    WCHAR   aucCond[20];                        /* data of conditional exp. */
    USHORT  usLength;                           /* length of cond. exp.     */
    USHORT  fsOption;                           /* option flags             */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMCOND, *PECMCOND;

#define     REPORT_ACTIVE           0x0001      /* report active rec.       */
#define     REPORT_INACTIVE         0x0002      /* report inactive rec.     */
#define     REPORT_WITH_LOCK        0x0004      /* report with lock         */
#define     REPORT_AS_RESET         0x0008      /* as reset report          */
#define     REPORT_RETRY_LOCKED     0x0010      /* retry locked rec.        */

typedef struct _ECMMAINTE {                 /* data for report maintenance  */
    USHORT  usBatch;                            /* batch number             */
    WCHAR   aucCond[20];                        /* data of conditional exp. */
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

#define     PURGE_APPLIED           0x0001      /* purge applied mainte. rec*/
#define     PURGE_UNAPPLY           0x0002      /* purge unapply mainte. rec*/

typedef struct  _ECMBLOCK {                 /* data for block access        */
    USHORT  fsOption;                           /* options                  */
    USHORT  usReserve;                          /* reserved (0)             */
} ECMBLOCK, *PECMBLOCK;

#define     BLOCK_READ              0x0001      /* read access              */
#define     BLOCK_WRITE             0x0002      /* write access             */

/* --- parameters for PluUpdateRecord() --- */

typedef struct _UPDINF {                    /* update information           */
    USHORT  fsControl;                          /* update control flag      */
    LONG    lSalesQty;                          /* sales quantity           */
    LONG    lSalesAmt;                          /* sales amounts            */
    LONG    lDiscQty;                           /* discount quantity        */
    LONG    lDiscAmt;                           /* discount amounts         */
} UPDINF, *PUPDINF;

#define     CNTRL_EXEC_UPDATE       0x0001      /* execute an update        */
#define     CNTRL_EXEC_READ         0x0002      /* respond records          */
#define     CNTRL_MINUS_UPDATE      0x0004      /* negative operation       */
#define     CNTRL_UNCOND_UPDATE     0x0008      /* unconditional update     */
#define     CNTRL_WITH_DISCOUNT     0x0010      /* affect disc. to sales    */
#define     CNTRL_IGNORE_SV_INT     0x0020      /* ignore supervisor int.   */

#define     RETURN_LINK             0x0001      /* respond link record      */
#define     RETURN_MIX_MATCH        0x0002      /* respond mix-match record */
#define     RETURN_PPI              0x0004      /* respond PPI record       */
#define     INT_SV_REC              0x0010      /* int. by supervisor int.  */
#define     INT_VV_REC              0x0020      /* int. by visual-verify    */
#define     INT_SCALE_REC           0x0040      /* int. by scalable item    */
#define     INT_MISC_REC            0x0080      /* int. by un-updatable     */
#define     INT_WRONG_LINK          0x0100      /* int. by wrong link rec   */
#define     MISSING_LINK            0x1000      /* missing link rec.        */
#define     MISSING_PPI             0x2000      /* missing PPI rec.         */
#define     MISSING_MIX_MATCH       0x4000      /* missing mix-match rec.   */

                                                /* buffer size for response */
#define     PLU_UPDATE_BUFFER       \
                (USHORT)(sizeof(SCNPLU) * 2 + sizeof(RECMIX) + sizeof(RECPPI))

/* --- seek origin point --- */

#define     PLU_SB_CUR              1           /* from current position    */
#define     PLU_SB_TOP              2           /* from top of file         */
#define     PLU_SB_END              3           /* from end of file         */

/* --- programmabilities --- */

#define     PROG_PROTECT_CHANGE     0x0001      /* protect against active   */
#define     PROG_PROTECT_DELETE     0x0002      /* protect against active   */
#define     PROG_RESET_TOTALS       0x0004      /* reset total fields       */
#define     PROG_SET_VV_FLAG        0x0008      /* set V-V flag             */

/* --- conditional expressions --- */

#define     COMP_MASK               0x0F        /* mask data for comp. data */
#define     COMP_EQUAL              0x01        /* A == B, equal            */
#define     COMP_NOT_EQUAL          0x02        /* A != B, not equal        */
#define     COMP_LESS_THAN          0x03        /* A  < B, less than        */
#define     COMP_GREATER_THAN       0x04        /* A  > B, greater than     */
#define     COMP_LESS_EQUAL         0x05        /* A <= B, less than or eq. */
#define     COMP_GREATER_EQUAL      0x06        /* A >= B, greater or equal */
#define     COMP_BIT_TRUE           0x07        /*    A & B,  bit true      */
#define     COMP_BIT_FALSE          0x08        /* ! (A & B), bit false     */

#define     LOGIC_MASK              0xF0        /* mask data for logic data */
#define     LOGIC_AND               0x10        /* EX1 && EX2, and          */
#define     LOGIC_OR                0x20        /* EX1 || EX2, or           */
#define     LOGIC_END               0x40        /* end of expression        */

/* --- max. dept definitions for index file control --- */

#define     MAX_DEPT                99

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
;+          P R O T O T Y P E     D E C L A R A T I O N s                   +
;+                                                                          +
\*==========================================================================*/
VOID   PluInitialize_GP(VOID);
VOID   PluFinalize_GP(VOID);
USHORT PluEnterCritMode_GP(USHORT usFile,USHORT usFunc,USHORT *pusHandle,
					VOID *pvParam);
USHORT PluExitCritMode_GP(USHORT usHandle,USHORT usReserve);
USHORT PluReportRecord_GP(USHORT usHandle,VOID *pvBuffer,USHORT *pusRet);
USHORT PluSenseFile_GP(USHORT usFile, MFINFO *pFileInfo);

#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

