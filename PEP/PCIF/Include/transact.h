/*=======================================================================*\
||  Erectronic  Cash  Register  -  90                                    ||
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.0              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         :
:   Category           :                                                  :
:   Program Name       : TRANSACT.H                                       :
:  ---------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments             :
:   Program Written    : 92-06-03 : 00.00.00     :                        :
:                      : 95-05-30 : 03.00.00     : Release 3.0            :
:                      : 95-07-19 : 03.00.04     : CURQUAL_SCALE3DIGIT    :
:                      :          :              : (0x1000 -> 0x8000000L) :
:                      : 95-11-28 : 03.01.00     : R3.1                   :
:                      : 98-10-21 : 03.03.00     : TAR 89859              :
:                      :          :              :                        :
:                      : 00-01-26 : 01.00.00     : Saratoga initial       :
:                      :          :              :                        :
:  ---------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              :
:   Memory Model       : Medium Model                                     :
:   Condition          :                                                  :
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#define     RESTITEMSIZE            0x0100
#define     RESTCONSOLISIZE         0x0100

#define     TRN_SUCCESS             0
#define     TRN_ERROR               -1

#define     TRN_MASKSTATCAN         0x0f

#define     TRN_NOCONSOLI           100

#define     TRN_STOR_GCFQUAL        0x0002  /* save/unsave GCF qualifier */
#define     TRN_STOR_CURQUAL        0x0004  /* save/unsave current qualifier */
#define     TRN_STOR_CONSOLI        0x0020  /* save/unsave consoli storage */

#define     TRN_TYPE_ITEMSTORAGE    0
#define     TRN_TYPE_CONSSTORAGE    1
#define     TRN_TYPE_NOITEMSTORAGE  2
#define     TRN_TYPE_NOCONSSTORAGE  3
#define     TRN_TYPE_SPLITA         4       /* R3.1 */
#define     TRN_TYPE_SPLITB         5       /* R3.1 */

/*----- Return of "TrnSplCheckSplit()",  R3.1 -----*/
#define     TRN_SPL_SEAT            1       /* Single Seat Transaction */
#define     TRN_SPL_MULTI           2       /* Multi Seat Transaction */
#define     TRN_SPL_TYPE2           3       /* Split Type 2 */
#define     TRN_SPL_BASE            4       /* Base Transaction */
#define     TRN_SPL_SPLIT           5       /* Split Key Depressed */

/*----- Update Type of "TrnSplGetSeatTrans(),TrnSplGetBaseTrans()",  R3.1 -----*/
#define     TRN_SPL_NOT_UPDATE      0       /* Not Update Storage (Read) */
#define     TRN_SPL_UPDATE          1       /* Update Storage */
#define     TRN_SPL_UPDATE_ITEMIZ   2       /* Update Itemizer Only */
#define     TRN_SPL_WHOLETAX_CAL    3       /* Get Base Trans. with Seat, V3.3 */

/*----- Consoli./Post Sorage status(for TrnSplGetSeatTrans) 6/5/96 -----*/  

#define     TRN_CONSOLI_STO         10      /* Consoli. Storage */
#define     TRN_POSTREC_STO         11      /* Post Rec. Storage */

/*----- Print Full Item/Print Only Total/Tender for CP Dup. Rec.(for TrnSplPrintSeat) 6/9/96 -----*/  

#define     TRN_PRT_FULL         20      /* Print Full Item */
#define     TRN_PRT_TTL          21      /* Print Only Total/Tender */

/*----- Return of "TrnTaxSystem()", V3.3 -----*/
#define     TRN_TAX_US              0       /* US Tax System */
#define     TRN_TAX_CANADA          1       /* Canada Tax System */
#define     TRN_TAX_INTL            2       /* Int'l VAT System */


/*--------------------------------------------------------------------------
*       transaction mode qualifier
--------------------------------------------------------------------------*/

    typedef  struct {
        USHORT  usCashierID;                    /* cashier ID */
        UCHAR   uchCashierSecret;               /* cashier secret code */
        UCHAR   fbCashierStatus;                /* cashier status */
        WCHAR   aszCashierMnem[NUM_OPERATOR];   /* cashier mnemonic */
        UCHAR   auchCasStatus[2];               /* cashier status, V3.3 */
        USHORT  usFromTo[2];                    /* guest check No, from/to */
        UCHAR   uchCasChgTipRate;               /* charge tips rate, V3.3 */
        UCHAR   uchCasTeamNo;                   /* team no, V3.3 */
        USHORT  usWaiterID;                     /* waiter ID */
        UCHAR   fbWaiterStatus;                 /* waiter status */
        WCHAR   aszWaiterMnem[NUM_OPERATOR];    /* waiter mnemonic */
        UCHAR   auchWaiStatus[2];               /* waiter status, V3.3 */
        UCHAR   uchWaiChgTipRate;               /* waiter tips rate, V3.3 */
        USHORT  fsModeStatus;                   /* mode status */
        UCHAR   uchTerm;                        /* Terminal Address,   R3.1 */
    } TRANMODEQUAL;



/*--------------------------------------------------------------------------
*       mode qualifier status
--------------------------------------------------------------------------*/
#define     MODEQUAL_CASHIER_NOT_OPEN   0       /* cashier doesn't open */
#define     MODEQUAL_WAITER_NOT_OPEN    0       /* waiter doesn't open */

#define     MODEQUAL_CASHIER_TRAINING   0x01    /* training cashier */
#define     MODEQUAL_DRAWER_B           0x02    /* drawer B */
#define     MODEQUAL_CASHIER_B          0x04    /* cashier B */

#define     MODEQUAL_WAITER_TRAINING    0x01    /* training waiter */
#define     MODEQUAL_WAITER_LOCK        0x10    /* waiter lock */
/* #define     MODEQUAL_WAITER_LOCK        0x02    / waiter lock */

/* mode status */
#define     MODEQUAL_CANADIAN           0x0001  /* Canadian tax system */
#define     MODEQUAL_INTL               0x0004  /* Int'l VAT System,    V3.3 */
#define     MODEQUAL_TAX_MASK           0x0001  /* Canadian tax system */
#define     MODEQUAL_NORECEIPT_DEF      0x0008  /* no receipt(default) */
#define     MODEQUAL_BARTENDER          0x0010  /* Bartender transaction */
#define     MODEQUAL_CASINT             0x0020  /* Cashier Interrupt transaction, R3.3 */
        
/* auchCasStatus[0] */
#define MODE_NOT_CLOSE_OWN_CHECK     0x01        /* not close own check */
#define MODE_NOT_NON_GUEST_CHECK     0x02        /* not non-guest check transaction */
#define MODE_NOT_GUEST_CHECK_OPEN    0x04        /* not guest check open */
#define MODE_TRAINING_CASHIER        0x08        /* training cashier status */
#define MODE_NOT_SINGLE_ITEM         0x10        /* not single item sales */
#define MODE_NOT_TRANSFER_TO         0x20        /* not check transfer from */
#define MODE_NOT_TRANSFER_FROM       0x40        /* not check transfer to */
#define MODE_NOT_CLOSE_ANY_CHECK     0x80        /* not close any check */

/* auchCasStatus[1] */
#define MODE_NOT_SURROGATE           0x01        /* not surrogate sign-in */
#define MODE_FINALIZE_NON_GCF        0x02        /* release on finalize non-gcf */
#define MODE_FINALIZE_GCF            0x04        /* release on finalize gcf */
#define MODE_SECRET_CODE             0x08        /* requeset secret code */
#define MODE_NOT_CAS_INT             0x10        /* not cashier interrupt */
#define MODE_USE_TEAM                0x20        /* use team, by FVT comment */


/*--------------------------------------------------------------------------
*       transaction GCF qualifier
--------------------------------------------------------------------------*/

    typedef struct {
        ULONG   ulCashierID;                /* cashier ID */
        ULONG   ulPrevCashier;              /* previous cashier ID, V3.3 */
        USHORT  usConsNo;                   // 49:consecutive No
        ULONG   ulStoreregNo;               // store/reg No
        USHORT  usGuestNo;                  /* guest check No */
        UCHAR   uchCdv;                     /* check digit */
        UCHAR   auchCheckOpen[3];           /* time check open hh:mm:ss, R3.1 */
        UCHAR   uchTeamNo;                  /* Team No, V3.3 */
		UCHAR   uchHourlyOffset;            /* Hourly total block offset, TRANGCFQUAL */
		USHORT  usNoPerson;                 /* No of person */
        USHORT  usNoPersonMax;              /* No of person (max amount) */
        USHORT  usTableNo;                  /* table No */
        LONG    lCurBalance;                /* current balance */
        LONG    lTranBalance;               /* transfer balance */
        USHORT  fsGCFStatus;                /* current status */
        USHORT  fsGCFStatus2;               /* GCF status,         R3.0 */
        UCHAR   uchSlipLine;                /* slip print line */
        UCHAR   uchPageNo;                  /* slip page No */
        USHORT  sPigRuleCo;                 /* pig rule counter,   R3.1 */
        WCHAR   aszNumber[20];              /* account number */
        WCHAR   auchExpiraDate[4];          /* acct expiration date */
        WCHAR   aszRefNumber[24];           // reference number from processor of transaction for EEPT
        WCHAR   aszAuthCode[24];            // authorization code from processor of transaction for EEPT
        WCHAR   auchMSRData[40];            /* msr data  */
        WCHAR   auchCouponType[13];         /* single type coupon  R3.0 */
        WCHAR   aszName[NUM_NAME];          /* customer name R3.0 */
        UCHAR   auchUseSeat[9];             /* use seat#,          R3.1 */
        UCHAR   auchFinSeat[9];             /* finalize seat#,     R3.1 */
        UCHAR   auchSeatDisc[10];           /* Disc for Each Seat#, V3.3 */
    } TRANGCFQUAL;


/*--------------------------------------------------------------------------
*       GCF qualifier status  (fsGCFStatus)
--------------------------------------------------------------------------*/
#define     GCFQUAL_TRAINING            0x0001      /* training transaction */
#define     GCFQUAL_PVOID               0x0008      /* preselect void GCF */
#define     GCFQUAL_REGULAR1            0x0010      /* regular discount 1 */
#define     GCFQUAL_REGULAR2            0x0020      /* regular discount 2 */
#define     GCFQUAL_CHARGETIP           0x0040      /* charge tip */
#define     GCFQUAL_NEWCHEK_CASHIER     0x0080      /* newcheck is cashier */
#define     GCFQUAL_ADDCHECK_CASHIER    0x0100      /* addcheck is cashier */
#define     GCFQUAL_BUFFER_FULL         0x0200      /* buffer full */
#define     GCFQUAL_DRIVE_THROUGH       0x0400      /* drive through type GCF */
#define     GCFQUAL_BUFFER_PRINT        0x0800      /* buffer print for service total key */
#define     GCFQUAL_SERVICE1            0x1000      /* use service total #1 */
#define     GCFQUAL_SERVICE2            0x2000      /* use service total #2 */
#define     GCFQUAL_SERVICE3            0x4000      /* use service total #3 */
#define     GCFQUAL_USEXEMPT            0x8000      /* tax exempt transaction for US */

#define     GCFQUAL_GSTEXEMPT           0x8000      /* GST tax exempt,     R3.1 */
#define     GCFQUAL_PSTEXEMPT           0x0002      /* PST tax exempt,     R3.1 */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (fsGCFStatus2)
--------------------------------------------------------------------------*/
#define     GCFQUAL_PAYMENT_TRANS       0x0001      /* Payment Transaction */
#define     GCFQUAL_USESEAT             0x0002      /* Use Seat# at Newcheck/Reorder, R3.1 */
#define     GCFQUAL_REGULAR3            0x0010      /* regular discount 3, R3.1 */
#define     GCFQUAL_REGULAR4            0x0020      /* regular discount 4, R3.1 */
#define     GCFQUAL_REGULAR5            0x0040      /* regular discount 5, R3.1 */
#define     GCFQUAL_REGULAR6            0x0080      /* regular discount 6, R3.1 */
#define     GCFQUAL_REGULAR_SPL         0x0100      /* R/D Performed for Split, R3.1 */
#define     GCFQUAL_AUTOCHARGETIP       0x0200      /* auto charge tip, V3.3 */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (auchCouponType)
--------------------------------------------------------------------------*/
#define     GCFQUAL_SINGLE_COUPON       0x01        /* on = single type */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (auchSeatDisc[10]),               V3.3
--------------------------------------------------------------------------*/
#define     GCFQUAL_REG1_SEAT           0x0001      /* regular discount 1 */
#define     GCFQUAL_REG2_SEAT           0x0002      /* regular discount 2 */
#define     GCFQUAL_REG3_SEAT           0x0004      /* regular discount 3 */
#define     GCFQUAL_REG4_SEAT           0x0008      /* regular discount 4 */
#define     GCFQUAL_REG5_SEAT           0x0010      /* regular discount 5 */
#define     GCFQUAL_REG6_SEAT           0x0020      /* regular discount 6 */

/*--------------------------------------------------------------------------
*       transaction current qualifier
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT  usConsNo;                   /* consecutive No */
        ULONG   ulStoreregNo;               /* store/reg No */
        UCHAR   auchTotal[7];               /* total key parameter,   R3.1 */
        UCHAR   uchSplit;                   /* split key#,            R3.1 */
        UCHAR   uchPrintStorage;            /* target storage at print */
        UCHAR   uchKitchenStorage;          /* target storage at kitchen */
        UCHAR   fbNoPrint;                  /* no print status */
        UCHAR   fbCompPrint;                /* compulsory print status */
        ULONG   flPrintStatus;              /* print status */
        USHORT  fsCurStatus;                /* current status */
        USHORT  fsCurStatus2;               /* current status 2,  R3.1 */
        UCHAR   uchSeat;                    /* seat#,             R3.1 */
    } TRANCURQUAL;



/*--------------------------------------------------------------------------
*       current qualifier status
--------------------------------------------------------------------------*/
#define     CURQUAL_NO_R                0x01    /* no print(receipt) */
#define     CURQUAL_NO_J                0x02    /* no print(journal) */
#define     CURQUAL_NO_V                0x04    /* no print(validation) */
#define     CURQUAL_NO_S                0x08    /* no print(slip) */
    
#define     CURQUAL_COMP_R              0x01    /* compulsory print(R) */
#define     CURQUAL_COMP_J              0x02    /* compulsory print(J) */
#define     CURQUAL_COMP_V              0x04    /* compulsory print(V) */
#define     CURQUAL_COMP_S              0x08    /* compulsory print(S) */

/* print status */

#define     CURQUAL_VAL_TYPE1           0x0000L /* cons#, id, time */
#define     CURQUAL_VAL_TYPE2           0x0001L /* cons#, id, date */
#define     CURQUAL_VAL_TYPE3           0x0003L /* cons#, time, date */
#define     CURQUAL_VAL_MASK            0x0003L /* mask */
#define     CURQUAL_TAKEKITCHEN         0x0004L /* not take to kitchen */
#define     CURQUAL_PRE_BUFFER          0x0000L /* precheck/buffering */
#define     CURQUAL_PRE_UNBUFFER        0x0010L /* precheck/unbuffering */
#define     CURQUAL_POSTCHECK           0x0020L /* postcheck */
#define     CURQUAL_STORE_RECALL        0x0030L /* store/recall */
#define     CURQUAL_GC_SYS_MASK         0x0030L /* store/recall */

#define     CURQUAL_CANADA_ALL          0x0000L /* Canadian tax print (all) */
#define     CURQUAL_CANADA_GST_PST      0x0200L /* Canadian tax print (GST/PST) */
#define     CURQUAL_CANADA_INDI         0x0100L /* Canadian tax print (individual) */
#define     CURQUAL_SETMENU_KITCHEN     0x1000L /* setmenu child PLU print (K/P) */
#define     CURQUAL_SETMENU             0x2000L /* setmenu child PLU print (R/J) */

#define     CURQUAL_DDMMYY              0x4000L     /* DD/MM/YY date format */
#define     CURQUAL_MILITARY            0x8000L     /* use military hour */
#define     CURQUAL_NO_ELECTRO          0x010000L   /* electro stamp provided */
#define     CURQUAL_NOT_PRINT_PROM      0x020000L   /* don't print promition header */
#define     CURQUAL_SINGLE_CHCK_SUM     0x040000L   /* print check sum line by single wide */
#define     CURQUAL_CD_GCF_NO           0x080000L   /* GCF no. with check digit */
#define     CURQUAL_SOFTCHECK           0x100000L   /* soft check unbuffer system */
#define     CURQUAL_PMCOND_SORT         0x200000L   /* PM/Condiment is sort, R3.0 */
#define     CURQUAL_POSTRECT            0x400000L   /* post receipt */
#define     CURQUAL_NOT_SLIP_PROM       0x800000L   /* not print promotion header at slip */
#define     CURQUAL_UNIQUE_TRANS_NO     0x1000000L  /* use unique transaction number */
#define     CURQUAL_PRINT_PRIORITY      0x2000000L  /* print priority,  R3.0 */
#define     CURQUAL_PARKING             0x4000000L  /* Parking Receipt,  R3.0 */
#define     CURQUAL_SCALE3DIGIT         0x8000000L  /* scale's digits under decimal point */

/* V3.3 */
#define     CURQUAL_NOT_PRINT_VAT       0x0200L     /* not print VAT */
#define     CURQUAL_NOT_PRINT_VATAPP    0x0400L     /* not print VATable */
#define     CURQUAL_NOT_PRINT_SER       0x0800L     /* not print service */
#define     CURQUAL_EXCLUDE_VAT         0x0040L     /* excluded vat */
#define     CURQUAL_EXCLUDE_SERV        0x0100L     /* excluded service */
                                                    /* shares with canadian tax */


#define     CURQUAL_PVOID               0x0001      /* preselect void condition */
#define     CURQUAL_TRAINING            0x0002      /* training print */
#define     CURQUAL_TABLECHANGE         0x0004      /* table No changed */
#define     CURQUAL_PERSONCHANGE        0x0008      /* No of person changed */
#define     CURQUAL_CASHIER             0x0000      /* cashier operation */
#define     CURQUAL_NEWCHECK            0x0010      /* newcheck operation */
#define     CURQUAL_REORDER             0x0020      /* reorder operation */
#define     CURQUAL_ADDCHECK            0x0030      /* addcheck operatopn */
#define     CURQUAL_OPEMASK             0x0030      /* addcheck operatopn */
#define     CURQUAL_CANCEL              0x0040      /* Transaction Cancel operatopn */
#define     CURQUAL_TRANSFER            0x0080      /* Check Transfer operatopn */
#define     CURQUAL_MULTICHECK          0x0100      /* multi check payment operatopn */
#define     CURQUAL_ADDCHECK_SERVT      0x0200      /* service total at addcheck,   R3.1*/
#define     CURQUAL_TRAY                0x0800      /* tray total transaction */

/*--------------------------------------------------------------------------
*       current qualifier status (fsCurStatus2)          R3.1
--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------
*       transaction itemizers (us unique)
--------------------------------------------------------------------------*/

    typedef struct {
        LONG        lAffectTaxable[STD_TAX_ITEMIZERS];      /* taxable itemizer */
        LONG        lNonAffectTaxable[STD_TAX_ITEMIZERS];   /* taxable itemizer */
        LONG        lDiscTaxable[6];        /* discoutable taxable */
        LONG        lTaxItemizer[STD_TAX_ITEMIZERS];        /* tax itemizer */
        LONG        lPrintTaxable[STD_TAX_ITEMIZERS];       /* print taxable itemizer (postcheck) */
        LONG        lPrintTax[STD_TAX_ITEMIZERS];           /* print tax itemizer (postcheck) */
        LONG        lAffectTaxExempt[STD_TAX_ITEMIZERS];    /* tax exempt itemizer */
        LONG        lPrintTaxExempt[STD_TAX_ITEMIZERS];     /* print tax exempt itemizer (postcheck) */
    } TRANUSITEMIZERS;

/*--------------------------------------------------------------------------
*       transaction itemizers (Canadian unique)
--------------------------------------------------------------------------*/

    typedef struct {
        LONG    lTaxable[9];
        LONG    lAffTaxable[5];
        LONG    lAffTax[STD_TAX_ITEMIZERS];
        LONG    lPrtTaxable[5];
        LONG    lPrtTax[STD_TAX_ITEMIZERS];
        LONG    lDiscTaxable[STD_DISC_ITEMIZERS_MAX][9];         /* discountable/taxable itemizer 4/24/96 *///Database Changes 3-29-04 JHHJ
        LONG    lDiscTax[9];                /* discountable/tax itemizer */
        LONG    lMealLimitTaxable;
        LONG    lMealNonLimitTaxable;
        LONG    lPigRuleTaxable;            /* Add 10-21-98 */
    } TRANCANADAITEMIZERS;

/*--------------------------------------------------------------------------
*       Transaction Itemizers (Int'l Unique)                    V3.3
--------------------------------------------------------------------------*/

    typedef struct {
        LONG        lServiceable;           /* serviceable itemizer */
        LONG        lDiscountable_Ser[STD_DISC_ITEMIZERS_MAX];   /* discountable itemizer 
                                            with service, 0:SI#1, 1:SI#2, 2:MI *///Database Changes 3-29-04 JHHJ
        LONG        lVATSer[3];             /* VATable with service */
        LONG        lVATNon[3];             /* VATable without service */
        LONG        lDisSer[3][3];          /* discountable with service */
        LONG        lDisNon[3][3];          /* discountable without service */
    } TRANINTLITEMIZERS;

/*--------------------------------------------------------------------------
*       transaction itemizers
--------------------------------------------------------------------------*/

    typedef union {
        TRANUSITEMIZERS     TranUsItemizers;        /* us itemizers */
        TRANCANADAITEMIZERS TranCanadaItemizers;    /* Canadian itemizers */
        TRANINTLITEMIZERS   TranIntlItemizers;      /* Int'l itemizers, V3.3 */
    } ITEMIZERS;


/*--------------------------------------------------------------------------
*       transaction itemizers (us/Canada)
--------------------------------------------------------------------------*/

    typedef struct {
        LONG        lMI;                       /* main itemizer */
        LONG        lDiscountable[STD_DISC_ITEMIZERS_MAX];          /* discountable itemizer */
        LONG        lHourly;                   /* hourly itemizer */
        SHORT       sItemCounter;              /* item counter */
        LONG        lService[3];               /* service total itemizer */
        LONG        lBonus[STD_NO_BONUS_TTL];                 /* bonus total of hourly, V3.3 */
        LONG        lTenderMoney[STD_TENDER_MAX];  /* tender on hand itemizer, Saratoga JHHJ 3-16-04  11->20*/
        LONG        lFCMoney[8];               /* FC on hand itemizer, Saratoga */
        ITEMIZERS   Itemizers;                 /* us/Canada unique itemizers */
    } TRANITEMIZERS;


/*--------------------------------------------------------------------------
*       transaction item storage header
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT          usFSize;            /* 1: file size */
        USHORT          usActLen;           /* 2: actual length */
        TRANMODEQUAL    TranModeQual;       /* 3: mode qualifier */
        TRANCURQUAL     TranCurQual;        /* 4: current qualifier */
        USHORT          usVli;              /* 5: storage size */
    } TRANITEMSTORAGEHEADER;


/*--------------------------------------------------------------------------
*       transaction consolidation storage header
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT          usFSize;            /* 1: file size */
        USHORT          usActLen;           /* 2: actual length */
        TRANGCFQUAL     TranGCFQual;        /* 3: GCF qualifier */
        TRANITEMIZERS   TranItemizers;      /* 4: transaction itemizers */
        USHORT          usVli;              /* 5: storage size */
    } TRANCONSSTORAGEHEADER;


/*--------------------------------------------------------------------------
*       transaction information main
--------------------------------------------------------------------------*/

    typedef struct {
        SHORT           fsTransStatus;          /* Transaction Module Status, R3.1 */
        USHORT          usGuestNo[2];           /* GCF# on LCD for Split,  R3.1 */
        TRANMODEQUAL    TranModeQual;           /* mode qualifier */
        TRANGCFQUAL     TranGCFQual;            /* GCF qualifier */
        TRANCURQUAL     TranCurQual;            /* current qualifier */
        TRANITEMIZERS   TranItemizers;          /* transaction itemizers */
        SHORT           hsTranItemStorage;      /* item storage file handle */
        USHORT          usTranItemStoFSize;     /* item storage file size */
        USHORT          usTranItemStoVli;       /* item storage Vli */
        SHORT           hsTranConsStorage;      /* consoli storage file handle */
        USHORT          usTranConsStoFSize;     /* consoli storage file size */
        USHORT          usTranConsStoVli;       /* consoli storage Vli */
        SHORT           hsTranPostRecStorage;   /* post rec. storage file handle */
        SHORT           hsTranItemIndex;        /* handle of item storage index */
        SHORT           hsTranNoItemIndex;      /* handle of not cons. storage index */
        SHORT           hsTranConsIndex;        /* handle of cons. storage index */
        SHORT           hsTranNoConsIndex;      /* handle of not cons. storage index */
    } TRANINFORMATION;


/*==========  Status of "TrnInformation.fsTransStatus"  ==========*/
#define     TRN_STATUS_SEAT_TRANS       0x01    /* During Single Seat# Transaction */
#define     TRN_STATUS_MULTI_TRANS      0x02    /* During Multi Seat# Transaction */
#define     TRN_STATUS_TYPE2_TRANS      0x04    /* During Type 2 Transaction */


/*--------------------------------------------------------------------------
*       transaction information for split,      R3.1
--------------------------------------------------------------------------*/

    typedef struct {
        TRANGCFQUAL     TranGCFQual;            /* GCF qualifier */
        TRANITEMIZERS   TranItemizers;          /* transaction itemizers */
        SHORT           hsTranConsStorage;      /* consoli storage file handle */
        USHORT          usTranConsStoFSize;     /* consoli storage file size */
        USHORT          usTranConsStoVli;       /* consoli storage Vli */
        SHORT           hsTranConsIndex;        /* handle of cons. storage index */
    } TRANINFSPLIT;


/*--------------------------------------------------------------------------
*       transaction information status,             R3.1
--------------------------------------------------------------------------*/
#define     TRANI_MODEQUAL          0x0001      /* initialize mode qualifier */
#define     TRANI_GCFQUAL           0x0002      /* initialize GCF qualifier */
#define     TRANI_CURQUAL           0x0004      /* initialize cuurent qualifier  */
#define     TRANI_ITEMIZERS         0x0008      /* initialize itemizers */
#define     TRANI_ITEM              0x0010      /* initialize item storage */
#define     TRANI_CONSOLI           0x0020      /* initialize consoli storage */

#define     TRANI_GCFQUAL_SPLA      0x0100      /* Split A, GCF qualifier */
#define     TRANI_ITEMIZERS_SPLA    0x0200      /* Split A, itemizers */
#define     TRANI_CONSOLI_SPLA      0x0400      /* Split A, consoli storage */
#define     TRANI_GCFQUAL_SPLB      0x0800      /* Split B, GCF qualifier */
#define     TRANI_ITEMIZERS_SPLB    0x1000      /* Split B, itemizers */
#define     TRANI_CONSOLI_SPLB      0x2000      /* Split B, consoli storage */

/*--------------------------------------------------------------------------
*       post receipt temporary file size for pep
--------------------------------------------------------------------------*/

#define     TRNINFOW                sizeof(TRANINFORMATION)

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*--------------------------------------------------------------------------
*       prototype declaration
--------------------------------------------------------------------------*/

    SHORT   TrnOpen( VOID *TrnOpen );                           /* open */
    SHORT   TrnItem( VOID *TrnItem );                           /* itemize */
    SHORT   TrnClose( VOID *TrnClose );                         /* close */
    VOID    TrnModeIn( ITEMREGMODEIN *ItemRegModeIn );          /* reg mode-in */
    SHORT   TrnStorage(VOID *TrnStorage);                       /* Moved, R3.1 */
    VOID    TrnThrough( VOID *TrnItem );                        /* through */
    VOID    TrnThroughDisp( VOID *TrnItem );                    /* through for display on the flay R3.1 */
    SHORT   TrnGetGC( USHORT usGuestNo, SHORT sType, USHORT usType );/* get target GCF */
    SHORT   TrnQueryGC( USHORT usGuestNo );                     /* query target GCF */
    SHORT   TrnSaveGC( USHORT usType, USHORT usGuestNo );       /* save target GCF */
    SHORT   TrnCancelGC( USHORT usGuestNo, USHORT usType );     /* cancel target GCF */
    SHORT   TrnSendTotal( VOID );                               /* send target GCF */
    VOID    TrnInit( VOID );                                    /* initalize file handle value, V3.3 */
    VOID    TrnInitialize( USHORT usType );                     /* initialize transaction information */        

    SHORT   TrnCouponSearch( ITEMCOUPONSEARCH *pItemSearch,
                             SHORT             sStorageType);   /* coupon search */
    SHORT   TrnSingleCouponSearch( ITEMCOUPONSEARCH *pItemSearch,
                             SHORT             sStorageType);   /* single coupon search */

    VOID    TrnGetModeQual( TRANMODEQUAL *pData );              /* get mode qualifier */
    VOID    TrnPutModeQual( TRANMODEQUAL *pData );              /* put mode qualifier */
    VOID    TrnGetGCFQual( TRANGCFQUAL **pData );               /* get GCF qualifier */
    VOID    TrnPutGCFQual( TRANGCFQUAL *pData );                /* put GCF qualifier */
    VOID    TrnGetCurQual( TRANCURQUAL *pData );                /* get current qualifier */
    VOID    TrnPutCurQual( TRANCURQUAL *pData );                /* put current qualifier */
        
    VOID    TrnGetTI(TRANITEMIZERS **pData);                    /* get transaction itemizers */
    VOID    TrnPutTI(TRANITEMIZERS *pData);                     /* put transaction itemizers */

    SHORT   TrnCheckSize( VOID *pData, USHORT Data );           /* check storage size */
    USHORT  TrnGetStorageSize( USHORT usType );                 /* get storage size */
    SHORT   TrnCreateFile(USHORT usSize, UCHAR uchType);        /* create item/cons./post rec. storage file */
    SHORT   TrnChkAndCreFile(USHORT usSize, UCHAR uchType);     /* check and create item/cons./post rec. storage file */
    VOID    TrnChkAndDelFile(USHORT usSize, UCHAR uchType);     /* check and delete item/cons./post rec. storage file */
    USHORT  TrnReadFile(USHORT usOffset, VOID *pData,
                        USHORT usSize, SHORT hsFileHandle);
    VOID    TrnWriteFile(USHORT usOffset, VOID *pData,
                         USHORT usSize, SHORT hsFileHandle);
    VOID    TrnStoGetConsToPostR(VOID);
    SHORT   TrnStoPutPostRToCons(VOID);                     /* V3.3 */
    VOID    TrnGetTranInformation(TRANINFORMATION **TranInfo);
    VOID    TrnPutTranInformation(TRANINFORMATION *TranInfo);
    VOID    TrnPutInfGuestSpl(UCHAR uchMiddle, UCHAR uchRight);
    SHORT   TrnICPOpenFile(VOID);
    VOID    TrnICPCloseFile(VOID);
    USHORT  TrnCalStoSize(USHORT usSize, UCHAR uchType);
    USHORT  TrnConvertError(SHORT sError);

    VOID    TrnSetConsNoToPostR( USHORT usConsecutiveNo );

    VOID    TrnPrintType( VOID *pItem );                /* V3.3 */

    /*----- R3.1 -----*/
    SHORT   TrnVoidSearch(VOID *pData, SHORT sQTY, SHORT sStorage);
    VOID    TrnVoidSearchRedSalesMod(ITEMSALES *pItem);
    SHORT   TrnTransferSearch(VOID *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnTransferSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnTransferSearchSalesMod(ITEMSALES *pData, SHORT sStorage);
    SHORT   TrnTransferSearchDisc(ITEMDISC *pData, SHORT sStorage);
    SHORT   TrnTransferSearchCoupon(ITEMCOUPON *pData, SHORT sStorage);
    SHORT   TrnTransferSearchSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc, SHORT sStorage);
    VOID    TrnItemTransfer(VOID *pData);
    VOID    TrnSplReduceItemizerSales(ITEMSALES *pItem);
    VOID    TrnSplReduceItemizerDisc(ITEMDISC *pItem);
    VOID    TrnSplReduceStorageSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc);

    SHORT   TrnSplCheckSplit(VOID);
    VOID    TrnSplCheck(VOID);
    SHORT   TrnSplGetSeatTrans(UCHAR uchSeat, SHORT sUpdate, UCHAR uchStoType, SHORT sSplType);

    SHORT   TrnSplGetSeatTransCheck(UCHAR uchSeat);
    VOID    TrnSplGetDispSeat(TRANINFORMATION **TrnInf,
                TRANINFSPLIT **SplitA, TRANINFSPLIT **SplitB);
    SHORT   TrnSplRightArrow(VOID);
    SHORT   TrnSplLeftArrow(VOID);
    SHORT   TrnSplCheckFinSeatTrans(VOID);
    VOID    TrnSplPrintSeat(UCHAR uchSeat,
                SHORT sUpdate, USHORT usTrail, USHORT usAllPrint, 
                UCHAR auchTotalStatus[], UCHAR uchPrintType);
    VOID    TrnSplPrintBase(USHORT usTrail, USHORT usAllPrint, 
                UCHAR auchTotalStatus[]);
    VOID    TrnSplAddSeatTrans(UCHAR uchSeat);
    SHORT   TrnSplGetBaseTrans(SHORT sUpdate, UCHAR uchStoType);
    SHORT   TrnSplGetBaseTransSeat(UCHAR uchStoType);
    VOID    TrnSplDispSeatNo(UCHAR *uchMiddle, UCHAR *uchRight);
    VOID    TrnSplCancel3Disp(VOID);
    VOID    TrnSplCancel3DispSplit(VOID);
    SHORT   TrnSplSearchUseSeatQueue(UCHAR uchSeat);
    SHORT   TrnSplSearchFinSeatQueue(UCHAR uchSeat);
    SHORT   TrnSplAddGC2GC(VOID);
    VOID    TrnSplAddGC2GCGCFQual(UCHAR *puchBuffer);
    VOID    TrnSplAddGC2GCItemizers(TRANITEMIZERS *pItem2);

    SHORT   TrnOpenSpl(VOID *ItemOpen, SHORT sType);
    SHORT   TrnItemSpl(VOID *pItem, SHORT sType);
    VOID    TrnThroughSpl(VOID *TrnItem);
    VOID    TrnInitializeSpl(USHORT usType);
    SHORT   TrnCreateFileSpl(USHORT usNoOfItem);
    SHORT   TrnChkAndCreFileSpl(USHORT usNoOfItem);
    VOID    TrnChkAndDelFileSpl(USHORT usNoOfItem);
    SHORT   TrnOpenFileSpl(VOID);
    VOID    TrnCloseFileSpl(VOID);

    VOID    TrnGetGCFQualSpl(TRANGCFQUAL **TranGCFQual, SHORT sType);
    VOID    TrnPutGCFQualSpl(TRANGCFQUAL *TranGCFQual, SHORT sType);

    VOID    TrnGetTISpl(TRANITEMIZERS **pItem, SHORT sType);

    VOID    TrnGetInfSpl(TRANINFSPLIT **pSplit, SHORT sType);
    SHORT   TrnLoanPickup(VOID *pData);                         /* Saratoga */

/* ===== End of File (TRANSACT.H) ===== */
