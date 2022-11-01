
/*=======================================================================*\
||  Erectronic  Cash  Register  -  90                                    ||
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.0              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         :
:   Category           :                                                  :
:   Program Name       : TRANS.H                                          :
:  ---------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments             :
:   T.Nakahata         : 95/05/02 : 3.00.00      : add coupon feature and :
:                                                  print priority         :
:   T.Nakahata         : 95/05/30 : 3.00.00      : add unique trans. no.  :
:   T.Nakahata         : 95/06/22 : 3.00.00      : mod TrnRegDiscTax      :
:   T.Nakahata         : 95/06/29 : 3.00.02      : TrnPaidOrderCheckSize  :
:   T.Nakahata         : 95/07/19 : 3.00.03      : reduce work buff size  :
:   T.Nakahata         : 95/08/11 : 3.00.04      : add no-consoli feature :
:   hkato              : 95/11/17 : 3.01.00      : R3.1                   :
:                      :                         :                        :
:   K.Yanagida         : 00/01/26 : 01.00.00     : Saratoga initial       :
:                      :                         :                        :
:  ---------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              :
:                                                                         :
:   Memory Model       : Medium Model                                     :
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
*        define value 
--------------------------------------------------------------------------*/
                                                                   
#define         TRN_WORKBUFFER          sizeof( ITEMSALES )     /* work buffer for compress 5/24/94 */

#define         TRN_RESTITEMSTORAGE     0x0230                  /* item storage limit */
#define         TRN_RESTCONSOLISTORAGE  0x0230                  /* consoli storage limit */
                
#define         TRN_1ITEM_BYTES         128                     /* storage bytes for 1 item, saratoga */
/*#define         TRN_1ITEM_BYTES         68                    */  /* storage bytes for 1 item */
#define         TRN_STOR_BASE_BYTES     160                     /* storage base bytes for 1 transaction */

/* #define         TRN_TEMPBUFF_SIZE       512     *//* temp buff max size */
#define         TRN_TEMPBUFF_SIZE       1024    /* temp buff max size, Saratoga */

/*----- Type of Canadian Tax Calculation Split/Normal V3.3 -----*/
#define         TRN_NOT_SPLIT           0       /* Normal */
#define         TRN_SPLIT               1       /* Split Check */
#define         TRN_NOT_DISCTAX         2       /* not discount/tax */

#define         TRN_ITEMIZE_NORMAL      1L
#define         TRN_ITEMIZE_REDUCE      -1L     /* for reduce */

/*--------------------------------------------------------------------------
*        transaction information
--------------------------------------------------------------------------*/
                         
    TRANINFORMATION     TrnInformation;


/*--------------------------------------------------------------------------
*       transaction open compressed data                       R3.1
--------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#,     R3.1 */
    }TRANSTORAGETRANSOPENNON;
                            
    typedef struct {                        
        USHORT      usCashierID;            /* 3: cashier ID */
        USHORT      usWaiterID;             /* 4: waiter ID */
        USHORT      usGuestNo;              /* 6:guest check No */
        UCHAR       uchCdv;                 /* 7:CDV */
        USHORT      usTableNo;              /* 8:table No */
        USHORT      usNoPerson;             /* 9:No of person */
        UCHAR       uchLineNo;              /* 10:line No */
        UCHAR       fbModifier;             /* 11:tax modifier */
        LONG        lAmount;                /* 13:previous amount */
        UCHAR       auchCheckOpen[3];       /* 15:time check open */
        USHORT      fsPrintStatus;          /* 16:print status */
        UCHAR       fbStorageStatus;        /*    storage status */
        UCHAR       uchStatus;              /* 18:MDC status */
        UCHAR       uchTeamNo;              /* 19:Team No, V3.3 */
    }TRANSTORAGETRANSOPEN;



/*--------------------------------------------------------------------------
*        sales data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#,     R3.1 */
        TCHAR       auchPLUNo[NUM_PLU_LEN]; /* PLU No, 2172 */
        USHORT      usDeptNo;               /* dept No, 2172 */
        /* USHORT      usPLUNo;                / PLU No */
        /* UCHAR       uchDeptNo;              / dept No */
        LONG        lQTY;                   /* quantity */
        LONG        lProduct;               /* product */
        SHORT       sCouponQTY;             /* quantity for coupon */
        SHORT       sCouponCo;              /* coupon counter */
        USHORT      usOffset;               /* Offset of Partner, R3.1 */
        UCHAR       uchPrintPriority;       /* print priority */
        USHORT      uchPPICode;             /* PPI Code   ESMITH */
		//UCHAR       uchPPICode;             /* PPI Code   R3.1 */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
        USHORT      fsLabelStatus;          /* label control status from GP, 2172 */
		USHORT		usUniqueID;
    }TRANSTORAGESALESNON;



    typedef struct {
        TCHAR           auchPLUNo[NUM_PLU_LEN];   /* PLU No, 2172 */
        USHORT          usDeptNo;               /* dept No, 2172 */
        /* USHORT          usPLUNo;            / PLU No */
        /* UCHAR           uchDeptNo;          / dept No */
        UCHAR           uchPrintPriority;   /* print priority */
        ULONG           lUnitPrice;         /* price */
        UCHAR           uchAdjective;       /* adjective */
        ITEMCONTCODE    ControlCode;        /* dept, report code, status */
		UCHAR			uchCondColorPaletteCode;	/* color palette code */
    }TRANSTORAGECOND;



    typedef struct {
        ULONG           lUnitPrice;             /* unit price */
        UCHAR           uchAdjective;           /* adjective */
        UCHAR           fbModifier;             /* void, tax modifier */
        UCHAR           fbModifier2;                /* price check, e-version, 2172 */
        TCHAR           aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];   /* US Customs number */
        ITEMCONTCODE    ControlCode;            /* dept, report code, status */
        UCHAR           uchPrintModNo;          /* No of P.M */
        UCHAR           auchPrintModifier[STD_MAX_COND_NUM];  /* print modifier */
        USHORT          fsPrintStatus;          /* print status */
        UCHAR           uchCondNo;              /* No of condiment */
        UCHAR           uchChildNo;             /* no. of child plu */
        TRANSTORAGECOND Condiment[STD_MAX_COND_NUM];          /* condiment */
        UCHAR           uchRate;                /* modifier discount rate */
        LONG            lDiscountAmount;        /* discount amount */
        UCHAR           fbStorageStatus;        /* storage status */
        UCHAR           uchRndType;             /*  random weight type, 2172 */
        UCHAR           uchPM;                  /*  price multiple, 2172     */
        USHORT          usLinkNo;               /* link plu no, 2172 */
        USHORT          usFamilyCode;           /* family code, 2172 */
		UCHAR			uchTableNumber;
		UCHAR			uchColorPalette;		/* Color Palette code */ //colorpalette-changes
    }TRANSTORAGESALES;



/*--------------------------------------------------------------------------
*        discount data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#,          R3.1 */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
    }TRANSTORAGEDISCNON;
                
    typedef struct {
        UCHAR       auchPLUNo[NUM_PLU_LEN];     /* PLU No, 2172 */
        /* USHORT      usPLUNo;                    / 3:PLU No */
        UCHAR       uchAdjective;               /* 4:adjective */
        USHORT      usDeptNo;                   /* dept No, 2172 */
        /* UCHAR       uchDeptNo;                  / 4:dept No */
        UCHAR       uchRate;                    /* 5:rate */
        LONG        lAmount;                    /* 6:discount amount */
        UCHAR       aszNumber[NUM_NUMBER];            /* 7:number */
        UCHAR       fbModifier;                 /* 9:void, tax modifier */
        UCHAR       auchStatus[3];              /* 12:status */
        USHORT      fsPrintStatus;              /* 13:print status */
        USHORT      usID;                       /* 14:waiter ID */
        UCHAR       fbStorageStatus;            /*    storage status */
    }TRANSTORAGEDISC;



/*--------------------------------------------------------------------------
*        total key data
--------------------------------------------------------------------------*/

    typedef struct {                            
        UCHAR       fbModifier;                 /* tax modifier */
        LONG        lMI;                        /* MI  */
        LONG        lService;                   /* service total */
        LONG        lTax;                       /* whole tax amount */
        UCHAR       aszNumber[NUM_NUMBER];            /* number */
        UCHAR       auchTotalStatus[7];         /* total status,  R3.1 */
        USHORT      fsPrintStatus;              /* print status */
        USHORT      usOrderNo;                  /* order No */
        UCHAR       uchCdv;                     /* CDV */
        UCHAR       fbStorageStatus;            /*    storage status */
        UCHAR       uchLineNo;                  /* 15:line No */
        USHORT      usConsNo;                   /* 16:consecutive No */
        USHORT      usID;                       /* 17:ID(cashier/waiter) */
        SHORT       sTrayCo;                    /*    tray counter */
        UCHAR       aszRoomNo[NUM_ROOM];               /* room number */
        UCHAR       aszGuestID[NUM_GUESTID];              /* guest ID */
        UCHAR       aszFolioNumber[NUM_FOLIO];          /* folio number */
        UCHAR       aszPostTransNo[NUM_POSTTRAN];          /* posted transaction number */
        UCHAR       aszCPText[NUM_CPRSPCO][NUM_CPRSPTEXT];  /* charge posting response text */
        UCHAR       auchExpiraDate[NUM_POSTTRAN];          /* expiration date       */
    }TRANSTORAGETOTAL;



/*--------------------------------------------------------------------------
*        tender data
--------------------------------------------------------------------------*/
                
    typedef struct {
        UCHAR       fbModifier;                 /* 3:void */
        LONG        lRound;                     /* 4:round amount */
        ULONG       ulFCRate;                   /* 5:foreign currency rate */
        LONG        lTenderAmount;              /* 7:tender amount */
        LONG        lForeignAmount;             /* 8:foreign amount */
        LONG        lBalanceDue;                /* 9:balance due */
        LONG        lChange;                    /* 10:change amount */
        UCHAR       aszNumber[NUM_NUMBER];            /* 11:number */
        UCHAR       auchStatus[3];              /* 14:status */
        USHORT      fsPrintStatus;              /* 15:print status */
        UCHAR       aszRoomNo[NUM_ROOM];               /* 17:room number */
        UCHAR       aszGuestID[NUM_GUESTID];              /* 23:guest ID */
        UCHAR       aszFolioNumber[NUM_FOLIO];          /* 26:folio number */
        UCHAR       aszPostTransNo[NUM_POSTTRAN];          /* 33:posted transaction number */
        UCHAR       aszCPText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];  /* 40:charge posting response text, Saratoga */
        UCHAR       fbStorageStatus;            /* 80:storage status */
        UCHAR       auchExpiraDate[NUM_EXPIRA];          /* 81:expiration date       */
        UCHAR       auchApproval[NUM_APPROVAL];            /* 85:EPT approval code     */
        UCHAR       uchCPLineNo;                /* charge posting line number */
        UCHAR       auchMSRData[NUM_MSRDATA];   /* MSR data */
        UCHAR       aszMnem[NUM_LEADTHRU];              /* Display Mnem for CP, Saratoga  */
        UCHAR       uchTenderOff;               /* Tender Offset for EPT,   Saratoga */
        UCHAR       uchBuffOff;                 /* Offset of Saved Buffer for EPT,  Saratoga */
    }TRANSTORAGETENDER;

/*--------------------------------------------------------------------------
*        misc. transaction data
--------------------------------------------------------------------------*/        

    typedef struct {
        USHORT      usID;                       /* 3:cashier/waiter ID */
        UCHAR       aszNumber[NUM_NUMBER];            /* 5:number */
        USHORT      usGuestNo;                  /* 7:guest check No */
        UCHAR       uchCdv;                     /*   CDV */
        LONG        lAmount;                    /* 8:amount */
        UCHAR       fbModifier;                 /* 10:void */
        USHORT      fsPrintStatus;              /* 11:print status */
        UCHAR       fbStorageStatus;            /*    storage status */
        USHORT      usYear;                     /* 13:Year to be time in */
        USHORT      usMonth;                    /* 15:Month to be time in */
        USHORT      usDay;                      /* 17:Day to be time in */
        ULONG       ulEmployeeNo;               /* 19:Employee Number R3.1 */
        UCHAR       uchJobCode;                 /* 21:Job Code to be time in */
        USHORT      usTimeinTime;               /* 22:Time to be time in */
        USHORT      usTimeinMinute;             /* 24:Minute to be time in */
        USHORT      usTimeOutTime;              /* 26:Time to be time out */
        USHORT      usTimeOutMinute;            /* 28:Minute to be time out */
        UCHAR       aszMnemonic[NUM_ETK_NAME];  /* ETK mnemonic,         R3.1 */
        UCHAR       uchTendMinor;               /*    tender minor 2172 */
        LONG        lQTY;                       /*    quantity 2172    */
        LONG        lUnitPrice;                 /*    unit price 2172  */
        UCHAR       fchStatus;                  /*    foreign cuurency status 2172 */
    }TRANSTORAGEMISC;


/*--------------------------------------------------------------------------
*       other data                              R3.1
--------------------------------------------------------------------------*/

    typedef struct {
        LONG    lAmount;                        /* amount */
        USHORT  fsPrintStatus;                  /* print status */
    }TRANSTORAGEOTHER;


/*--------------------------------------------------------------------------
*        affection data
--------------------------------------------------------------------------*/

    typedef struct {
        LONG        lAmount;                    /* 3:amount */
        SHORT       sItemCounter;               /* 4:item counter */
        SHORT       sNoPerson;                  /* 5:No of person */
        SHORT       sCheckOpen;                 /*   check open counter */
        UCHAR       uchOffset;                  /* 6:hourly block offset */
        UCHAR       fbModifier;                 /*    modifier */
        USCANVAT    USCanVAT;
        LONG        lService[3];                /*13: service total */
        LONG        lCancel;                    /*14: cancel total */
        USHORT      usId;                       /*15: cashier/waiter ID */
        USHORT      usGuestNo;                  /*16: guest check No */
        UCHAR       uchTotalID;                 /*17: totalkey Id */
        USHORT      fsPrintStatus;              /*18:print status */
        UCHAR       fbStorageStatus;            /*    storage status */
        UCHAR       uchAddNum;                  /*21: addcheck number */
        UCHAR       uchStatus;                  /*    MDC status */
        LONG        lBonus[STD_NO_BONUS_TTL];                  /* bonus total for hourly, V3.3 */
    }TRANSTORAGEAFFECTION;



/*--------------------------------------------------------------------------
*        print data
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT      usID;                       /* 4:waiter/cashier ID */
        ULONG       ulStoreregNo;               /* 6:store/reg No */
        USHORT      usConsNo;                   /* 7:consecutive No */
        USHORT      fsPrintStatus;              /* 9:print status */
        UCHAR       fbStorageStatus;            /*    storage status */
        USHORT      usTableNo;                  /* 11:table No */
        USHORT      usGuestNo;                  /* 13:GCF No */
        UCHAR       uchCdv;                     /* 14:CDV */
        UCHAR       uchLineNo;                  /* 15:line No */
        UCHAR       aszNumber[NUM_NUMBER];      /* 280:number        *add EPT, saratoga */
        UCHAR       aszRoomNo[NUM_ROOM];        /* 19:room number */
        UCHAR       aszGuestID[NUM_GUESTID];    /* 25:guest ID */
        UCHAR       aszCPText[NUM_CPRSPCO][NUM_CPRSPTEXT];  /* 28:charge posting response text */
        USHORT      usFuncCode;                 /* 268:function code */
        SHORT       sErrorCode;                 /* 270:error code */
        LONG        lAmount;                    /* 272:amount        *add EPT*/
        UCHAR       uchStatus;                  /* 278:EPT status    *add EPT*/
        UCHAR       fbModifier;                 /* 279:Modifier      *add EPT*/
        UCHAR       auchExpiraDate[NUM_EXPIRA]; /* 300:ExpDate       *add EPT*/
        UCHAR       auchApproval[NUM_APPROVAL]; /* 304:approval code *add EPT*/
    }TRANSTORAGEPRINT;


/*--------------------------------------------------------------------------
*        multi check payment data
--------------------------------------------------------------------------*/

    typedef struct{
        LONG        lService[3];                /* 4: service total */
        UCHAR       uchCdv;                     /* CDV */
        USHORT      usGuestNo;                  /* 5: guest check No */
        LONG        lMI;                        /*    MI */
        USHORT      fsPrintStatus;              /* 6:print status */
        UCHAR       fbStorageStatus;            /*    storage status */
    }TRANSTORAGEMULTICHECK;



/*--------------------------------------------------------------------------
*       operator close data
--------------------------------------------------------------------------*/

    typedef struct{
        USHORT  usID;                           /* cashier/waiter ID */
        UCHAR   uchUniqueAdr;                   /* unique address */
    }TRANSTORAGEOPECLOSE;



/*--------------------------------------------------------------------------
*       combination coupon data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;
        UCHAR       uchMinorClass;
        UCHAR       uchSeatNo;
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
        UCHAR       uchCouponNo;
        SHORT       sVoidCo;
        USHORT      usOffset[ 7 ];          /* Moved from Comp Area, R3.1 */
    } TRANSTORAGECOUPONNON;

    typedef struct {
        LONG        lAmount;
        UCHAR       fbModifier;
        UCHAR       fbStatus[ 2 ];
        UCHAR       aszNumber[ NUM_NUMBER ];
        USHORT      fsPrintStatus;
        UCHAR       uchNoOfItem;
        UCHAR       fbStorageStatus;
		USHORT		usBonusIndex;				//SR 157 Bonus Totals to 100 JHHJ
    } TRANSTORAGECOUPON;



/*--------------------------------------------------------------------------
*       sales item search data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       auchBuffer[ TRN_WORKBUFFER ];
        USHORT      usOffset;
    } TRANSTOSALESSRCH;

/*--------------------------------------------------------------------------
*       Loan/Pick-up data,      Saratoga
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT usCashierNo;           /* Cashier Number */
        USHORT usModifier;            /* Loan/Pickup Modifier Status */
        LONG   lForQty;               /* For, Qty Amount */
        LONG   lUnitAmount;           /* Qty Unit Amount */
        LONG   lNativeAmount;         /* Native Amount for FC */
        UCHAR  uchFCMDC;              /* FC Control MDC */
        UCHAR  uchFCMDC2;             /* FC Control MDC, V2.2 */
        LONG   lAmount;               /* Loan/Pickup Amount */
        LONG   lTotal;                /* Loan/Pickup Total Amount */
    } TRANSTORAGELOANPICK;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*--------------------------------------------------------------------------
*        prototype declaration (TRANSACTION MODULE BODY)
--------------------------------------------------------------------------*/
            
    VOID    TrnOpeOpen( ITEMOPEOPEN *pItemOpeOpen );
    SHORT   TrnTransOpen( ITEMTRANSOPEN *pItemTransOpen );
    SHORT   TrnSales( ITEMSALES *pItemSales );
    SHORT   TrnDiscount( ITEMDISC *pItemDiscount );
    SHORT   TrnCoupon( ITEMCOUPON *pItemCoupon );
    SHORT   TrnTotal( ITEMTOTAL *pItemTotal );
    SHORT   TrnTender( ITEMTENDER *pItemTender );
    SHORT   TrnMisc( ITEMMISC *pItemMisc );
    SHORT   TrnModifier( ITEMMODIFIER *pItemModifier );
    VOID    TrnOther( ITEMOTHER *pItemOther );
    VOID    TrnOtherDemand( VOID );
    VOID    TrnOtherPrint( ITEMOTHER *pItemOther );
    SHORT   TrnAffection( ITEMAFFECTION *pItemAffection );
    SHORT   TrnPrint( ITEMPRINT *pItemPrint );
    VOID    TrnOpeClose( ITEMOPECLOSE *pItemOpeClose );
    VOID    TrnTransClose( ITEMTRANSCLOSE *pItemTransClose );
    VOID    TrnTransPrint( VOID );
    VOID    TrnTransCloseMoney(VOID);                       /* Saratoga */
    SHORT   TrnMulti( ITEMMULTI *pItemMulti );
    /* VOID    TrnPrintType( VOID *pItem ); move to transact.h, V3.3 */
    VOID    TrnPrintTypeNoDsp(VOID *pItem);                 /* Saratoga */

    SHORT   TrnCreatePostRecFile( USHORT usConsFileSize );
    SHORT   TrnCreateIndexFile( USHORT     usNoOfItem,
                                UCHAR FAR *lpszIndexName,
                                ULONG     *pulCreatedSize );
    SHORT   TrnICOpenIndexFile( VOID );

    SHORT   TrnCouponVoidSearch( ITEMCOUPONSEARCH *pItemSearch,
                                 SHORT             sStorageType );
    SHORT   TrnTaxSystem(VOID);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION QUALIFIER MODULE)
--------------------------------------------------------------------------*/
                        
    VOID    TrnQual( VOID *pTrnQual );
    VOID    TrnQualOpn( ITEMOPEOPEN *pItemOpeOpen );
    VOID    TrnQualOpnCashier( ITEMOPEOPEN *pItemOpeOpen );
    VOID    TrnQualOpnWaiter( ITEMOPEOPEN *pItemOpeOpen );
    VOID    TrnQualTransCas( VOID );
    VOID    TrnQualTransNC( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransRO( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransSR( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransCash( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualLoanPick(ITEMTRANSOPEN *pItem);                 /* Saratoga */
    VOID    TrnQualTransTipsPO( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransTipsDec( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransCT( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTrans( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTotal( ITEMTOTAL *pItemTotal );
    VOID    TrnQualModifier( ITEMMODIFIER *pItemModifier );
    VOID    TrnQualModPvoid( VOID );
    VOID    TrnQualModRec( VOID );
    VOID    TrnQualModTable( ITEMMODIFIER *pItemModifier );
    VOID    TrnQualModPerson( ITEMMODIFIER *pItemModifier );
    VOID    TrnQualModLine( ITEMMODIFIER *pItemModifier );
    VOID    TrnQualModCustName( ITEMMODIFIER *pItemModifier );
    VOID    TrnQualModSeat(ITEMMODIFIER *pItemModifier);
    VOID    TrnQualItems(VOID *pItem);
    VOID    TrnQualOther(ITEMOTHER *pItem);                   /* Saratoga */
    VOID    TrnQualCls( ITEMOPECLOSE *pItemOpeClose );             
    VOID    TrnQualClsCashier( VOID );             
    VOID    TrnQualClsWaiter( VOID );             
    VOID    TrnQualClsCasInt( VOID );             


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION ITEMIZERS MODULE)
--------------------------------------------------------------------------*/

    SHORT   TrnItemizer( VOID * );
    VOID    TrnItemTrans( ITEMTRANSOPEN *ItemTransOpen );
    VOID    TrnItemRO( ITEMTRANSOPEN *ItemTransOpen );
    VOID    TrnItemSales( ITEMSALES *ItemSales );
    VOID    TrnSalesDept( ITEMSALES *ItemSales );
    SHORT   TrnQTY( ITEMSALES *ItemSales );
    VOID    TrnSalesPLU( ITEMSALES *ItemSales );
    VOID    TrnTaxable( ITEMSALES *ItemSales );
    VOID    TrnDiscountable( ITEMSALES *ItemSales );
    VOID    TrnDiscTax( ITEMSALES *ItemSales );
    VOID    TrnDiscTaxCan( UCHAR fbStatus, UCHAR fbTaxStatus, LONG lAmount );
    VOID    TrnPLUQTY( ITEMSALES *ItemSales );
    VOID    TrnSalesMod( ITEMSALES *ItemSales );
    VOID    TrnItemDisc( ITEMDISC *ItemDisc );
    VOID    TrnItemDiscount( ITEMDISC *ItemDisc );
    VOID    TrnRegDiscount( ITEMDISC *ItemDisc );
    VOID    TrnRegDiscTaxCan( ITEMDISC *ItemDisc );
    VOID    TrnRegDiscTaxRate( ITEMDISC *pItemDisc );
    VOID    TrnRegDiscTaxAmt( ITEMDISC *pItemDisc );
    VOID    TrnRegDiscVATCalc(ITEMDISC *pDisc, SHORT sType);
    VOID    TrnChargeTip( ITEMDISC *ItemDisc );
    VOID    TrnItemAC( ITEMAFFECTION *ItemAffection );
    VOID    TrnItemCoupon( ITEMCOUPON *pItemCoupon );
    VOID    TrnItemMisc(ITEMMISC *pData);
    VOID    TrnItemTotal( ITEMTOTAL *pItemTotal );
    CHAR    TrnGetPigRuleCo( LONG lPrice );


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION STORAGE MODULE)
--------------------------------------------------------------------------*/
                        
    SHORT   TrnStoOpen( ITEMTRANSOPEN *ItemTransOpen );
    SHORT   TrnStoOpenComp( ITEMTRANSOPEN *ItemTransOpen, UCHAR *pData );
    SHORT   TrnStoSales( ITEMSALES *ItemSales );
    SHORT   TrnStoSalesComp( ITEMSALES *ItemSales, UCHAR *pData );
    VOID    TrnStoSalesItemCons( ITEMSALES *ItemSales, UCHAR *pData, SHORT sSize );
    VOID    TrnStoSalesConsCons( ITEMSALES *ItemSales, UCHAR *pData, SHORT sSize );
    SHORT   TrnStoDisc( ITEMDISC *ItemDisc );
    SHORT   TrnStoDiscComp( ITEMDISC *ItemDisc, UCHAR *pData );
    SHORT   TrnStoCoupon( ITEMCOUPON *pItemCoupon );
    SHORT   TrnStoCouponComp( ITEMCOUPON *pItemCoupon, UCHAR *puchBuffer );
    SHORT   TrnStoTotal( ITEMTOTAL *ItemTotal );
    SHORT   TrnStoTotalComp( ITEMTOTAL *ItemTotal, UCHAR *pData );
    SHORT   TrnStoTender( ITEMTENDER *ItemTender );
    SHORT   TrnStoTenderComp( ITEMTENDER *ItemTender, UCHAR *pData );
    SHORT   TrnStoMisc( ITEMMISC *ItemMisc );
    SHORT   TrnStoMiscComp( ITEMMISC *ItemMisc, UCHAR *pData );
    SHORT   TrnStoOther(ITEMOTHER *ItemOther);
    SHORT   TrnStoOtherComp(ITEMOTHER *ItemOther, UCHAR *uchBuffer);
    SHORT   TrnStoAffect( ITEMAFFECTION *ItemAffection );
    SHORT   TrnStoAffectComp( ITEMAFFECTION *ItemAffection, UCHAR *pData );
    SHORT   TrnStoPrint( ITEMPRINT *ItemPrint );
    SHORT   TrnStoPrintComp( ITEMPRINT *ItemPrint, UCHAR *pData );
    SHORT   TrnStoMulti( ITEMMULTI *ItemMulti );
    SHORT   TrnStoMultiComp( ITEMMULTI *ItemMulti, UCHAR *pData );
    SHORT   TrnStoOpeClose( ITEMOPECLOSE *ItemOpeClose );
    SHORT   TrnStoOpeCloseComp( ITEMOPECLOSE *ItemOpeClose, UCHAR *pData );

    VOID    TrnStoItemNormal( VOID *pData, SHORT sSize );
    VOID    TrnStoNoItemNormal( VOID *pData, USHORT usItemOffset );
    VOID    TrnStoConsNormal( VOID *pData, SHORT sSize );
    VOID    TrnStoNoConsNormal( VOID *pData, USHORT usItemOffset );
    SHORT   TrnStoAppendIndex( TRANSTORAGESALESNON *pTranNon,
                               USHORT               usItemOffset,
                               SHORT                sStorageType );
    SHORT   TrnStoInsertIndex( TRANSTORAGESALESNON *pTranNon,
                               USHORT               usItemOffset,
                               SHORT                sStorageType );

    SHORT   TrnStoSalesSearch(ITEMSALES        *pItemSales,
                              UCHAR            *puchSrcZip,
                              TRANSTOSALESSRCH *pNormalItem,
                              TRANSTOSALESSRCH *pVoidItem,
                              SHORT             sStorageType,
                              SHORT             sType);
    SHORT   TrnStoSalesUpdate(ITEMSALES        *pItemSales,
                              UCHAR            *puchSrcZip,
                              TRANSTOSALESSRCH *pNormalItem,
                              TRANSTOSALESSRCH *pVoidItem,
                              SHORT             sStorageType);
    SHORT   TrnStoIsPairItem( ITEMSALES *pItemSales, UCHAR *puchTarget );
    SHORT   TrnStoIsItemInWorkBuff( UCHAR  *puchWorkBuff,
                                    USHORT usItemOffset,
                                    USHORT usMaxBuffSize,
                                    USHORT *pusItemSize );
    SHORT   TrnStoGetStorageInfo( SHORT  sStorageType,
                                  SHORT  *phsStorage,
                                  USHORT *pusHdrSize,
                                  USHORT *pusVliSize );
    VOID    TrnStoUpdateCouponType( ITEMCOUPON *pItem );
    SHORT   TrnStoIdxUpdate( ITEMSALES          *pItemSales,
                             TRANSTOSALESSRCH   *pNormalItem,
                             TRANSTOSALESSRCH   *pVoidItem,        
                             SHORT              sStorageType );
    SHORT   TrnStoNoIdxUpdate( ITEMSALES          *pItemSales,
                             TRANSTOSALESSRCH   *pNormalItem,
                             TRANSTOSALESSRCH   *pVoidItem,        
                             SHORT              sStorageType,
                            SHORT               sType );

    SHORT   TrnOpenFile(UCHAR FAR *uchFileName, UCHAR FAR *auchType);
    SHORT   TrnExpandFile( SHORT hsFileHandle, USHORT usInquirySize );
    SHORT   TrnSeekFile(SHORT hsFileHandle, ULONG ulActSize, ULONG *ulActMove);
    VOID    TrnCloseFile(SHORT hsFileHandle);
    VOID    TrnDeleteFile(UCHAR FAR *uchFileName);
    USHORT  TrnConvertError(SHORT sError);
    SHORT   TrnChkStorageSize(SHORT sSize);
    SHORT   TrnPaidOrderCheckSize( VOID *pData );
    SHORT   TrnStoLoanPick(VOID *pItem);                /* Saratoga */

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION COUPON MODULE)
--------------------------------------------------------------------------*/

    SHORT   TrnStoVoidCouponItem( UCHAR  *puchSalesItem,
                                  USHORT usSalesOffset,
                                  SHORT  sStorageType );
    SHORT   TrnStoIsSalesLinked( UCHAR *puchCoupon, USHORT usSalesOffset );
    VOID    TrnStoMakeVoidCpn( UCHAR *puchSource, ITEMCOUPON *pVoidCpn );

    SHORT   TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                SHORT            sStoratgeType );
    SHORT   TrnStoCpnItemSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                 UCHAR            *puchWorkBuffer,
                                 USHORT           usTargetItemOffset );
    SHORT   TrnStoCpnPLUSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                ITEMSALES        *pItemSales,
                                USHORT           *pusFoundIndex );
    SHORT   TrnStoCpnDeptSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                 ITEMSALES        *pItemSales,
                                 USHORT           *pusFoundIndex );
    SHORT   TrnStoIsCouponItem( ITEMSALES *pItemSales, UCHAR uchRecType );
    SHORT   TrnStoIsAllItemFound( ITEMCOUPONSEARCH *pItemCpnSearch );
    SHORT   TrnStoUpdateCpnCo( ITEMCOUPON *pItemCoupon, SHORT sStorageType );
    SHORT   TrnStoUpdateVoidCo( ITEMCOUPON *pItemCoupon,
                                ITEMCOUPON *pRetCoupon,
                                SHORT       sStorageType );
    SHORT   TrnStoCpnVoidSearch( ITEMCOUPON *pItemCoupon,
                                 USHORT     *pusOffset,
                                 SHORT      sStorageType );
    SHORT   TrnStoIsCpnVoidItem( ITEMCOUPON *pCouponVoid, UCHAR *puchTarget );
    USHORT  TrnStoQueryCpnType( ITEMCOUPONSEARCH *pItem );

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT MODULE BODY)          R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnSalesSpl(ITEMSALES *ItemSales, SHORT sType);
    SHORT   TrnDiscountSpl(ITEMDISC *ItemDisc, SHORT sType);
    SHORT   TrnCouponSpl(ITEMCOUPON *pItemCoupon, SHORT sType);
    SHORT   TrnTotalSpl(ITEMTOTAL *ItemTotal, SHORT sType);
    SHORT   TrnTenderSpl(ITEMTENDER *ItemTender, SHORT sType);
    SHORT   TrnMiscSpl(ITEMMISC *ItemMisc, SHORT sType);
    SHORT   TrnAffectionSpl(ITEMAFFECTION *ItemAffection, SHORT sType);
    SHORT   TrnPrintSpl(ITEMPRINT *ItemPrint, SHORT sType);
    SHORT   TrnOpenIndexFileSpl(VOID);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT QUALIFIER MODULE)     R3.1
--------------------------------------------------------------------------*/

    VOID    TrnQualSpl(VOID *TrnQual, SHORT sType);
    VOID    TrnQualTransSpl(ITEMTRANSOPEN *pItem, SHORT sType);
    VOID    TrnQualDiscSpl(ITEMDISC *pItem, SHORT sType);

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT ITEMIZERS MODULE)     R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnItemizerSpl(VOID *TrnItemizers, SHORT sType);
    VOID    TrnItemSalesSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnSalesDeptSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnSalesPLUSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnTaxableSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnDiscTaxSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnDiscTaxCanSpl(UCHAR fbStatus,
                UCHAR fbTaxStatus, LONG lAmount, SHORT sType);
    VOID    TrnItemCouponSpl(ITEMCOUPON *pItemCoupon, SHORT sType);
    VOID    TrnSalesModSpl(ITEMSALES *ItemSales, SHORT sType);
    VOID    TrnItemDiscSpl(ITEMDISC *ItemDisc, SHORT sType);
    VOID    TrnItemDiscountSpl(ITEMDISC *ItemDisc, SHORT sType);
    VOID    TrnRegDiscountSpl(ITEMDISC *ItemDis, SHORT sTypec);
    VOID    TrnRegDiscTaxCanSpl(ITEMDISC *ItemDisc, SHORT sType);
    VOID    TrnRegDiscTaxRateSpl(ITEMDISC *pItemDisc, SHORT sType);
    VOID    TrnRegDiscTaxAmtSpl(ITEMDISC *pItemDisc, SHORT sType);
    VOID    TrnChargeTipSpl(ITEMDISC *ItemDisc, SHORT sType);
    VOID    TrnItemAffectionSpl(ITEMAFFECTION *pItem, SHORT sType);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION STORAGE MODULE)             R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnStorageSpl(VOID *Item, SHORT sType);
    SHORT   TrnStoOpenSpl(ITEMTRANSOPEN *pItem, SHORT sType);
    SHORT   TrnStoSalesSpl(ITEMSALES *pItem, SHORT sType);
    VOID    TrnStoSalesSplQty(ITEMSALES *pItem, UCHAR *puchBuffer);
    SHORT   TrnStoDiscSpl(ITEMDISC *pItem, SHORT sType);
    SHORT   TrnStoCouponSpl(ITEMCOUPON *pItem, SHORT sType);
    SHORT   TrnStoTotalSpl(ITEMTOTAL *pItem, SHORT sType);
    SHORT   TrnStoTenderSpl(ITEMTENDER *pItem, SHORT sType);
    SHORT   TrnStoMiscSpl(ITEMMISC *pItem, SHORT sType);
    SHORT   TrnStoAffectSpl(ITEMAFFECTION *pItem, SHORT sType);
    SHORT   TrnStoPrintSpl(ITEMPRINT *pItem, SHORT sType);
    VOID    TrnStoConsNormalSpl(VOID *pData, SHORT sSize, SHORT sType);
    SHORT   TrnStoAppendIndexSpl(TRANSTORAGESALESNON *pTranNon,
                USHORT usItemOffset, SHORT sType);
    SHORT   TrnStoInsertIndexSpl(TRANSTORAGESALESNON *pTranNon,
                USHORT usItemOffset, SHORT sType);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT CHECK MODULE)         R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnSplRightArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight);
    SHORT   TrnSplLeftArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight);
    VOID    TrnSplDispSeatNo1(UCHAR uchSeat, UCHAR *auchUseQueue);
    SHORT   TrnVoidSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnVoidSearchSalesMod(ITEMSALES *pData, SHORT sStorage);
    SHORT   TrnVoidSearchSalesZip(ITEMSALES *pSales1, ITEMSALES *pSales2);
    SHORT   TrnVoidSearchDisc(ITEMDISC *pData, SHORT sStorage);
    SHORT   TrnVoidSearchCoupon(ITEMCOUPON *pData, SHORT sStorage);
    VOID    TrnVoidSearchRedSalesModSub(SHORT sStorage, ITEMSALES *pItem);
    VOID    TrnSplGetSeat(UCHAR uchSeat, SHORT sType, SHORT sUpdate, UCHAR uchStoType);
    VOID    TrnSplInsertUseSeatQueue(UCHAR uchSeat);
    VOID    TrnSplRemoveUseSeatQueue(UCHAR uchSeat);
    VOID    TrnSplInsertFinSeatQueue(UCHAR uchSeat);
    VOID    TrnSplRemoveFinSeatQueue(UCHAR uchSeat);
    USHORT  TrnSplRetLength(UCHAR uchMajor, UCHAR *puchWork);
    VOID    TrnSplDecideStorage(SHORT sStorage, USHORT *usReadPos,
                USHORT *usCurSize, USHORT *husHandle);

    SHORT   TrnSplReduce(VOID *pData);
    VOID    TrnSplRedItemSalesDept(ITEMSALES *pItem);
    VOID    TrnSplRedDiscTaxCan(UCHAR fbStatus, UCHAR fbTaxStatus, LONG lAmount);
    VOID    TrnSplRedItemSalesPLU(ITEMSALES *pItem);
    VOID    TrnSplTaxable(ITEMSALES *pItem);
    VOID    TrnSplDiscTax(ITEMSALES *pItem);
    VOID    TrnSplRedItemSalesMod(ITEMSALES *pItem);
    VOID    TrnSplRedItemDiscID(ITEMDISC *pItem);
    VOID    TrnSplRedItemDiscRD(ITEMDISC *pItem);
    VOID    TrnSplRegDiscTaxCan(ITEMDISC *pItem);
    VOID    TrnSplRegDiscTaxRate(ITEMDISC *pItem);
    VOID    TrnSplRegDiscTaxAmt(ITEMDISC *pItem);
    VOID    TrnSplRegDiscVATCalc(ITEMDISC *pDisc);
    VOID    TrnSplRedItemDiscChg(ITEMDISC *pItem);
    VOID    TrnSplReduceItemizerCoupon(ITEMCOUPON *pItem);
    VOID    TrnSplReduceStorageSales(ITEMSALES *pItem);
    VOID    TrnSplRedStoSales(ITEMSALES *pItem);
    VOID    TrnSplRedStoSalesMod(ITEMSALES *pItem);
    VOID    TrnSplReduceStorageDisc(ITEMDISC *pItem);
    VOID    TrnSplReduceStorageCoupon(ITEMCOUPON *pItem);
    SHORT   TrnStoSalesUpdateRed(ITEMSALES *pItemSales, UCHAR *puchSrcZip,
                TRANSTOSALESSRCH *pNormalItem, TRANSTOSALESSRCH *pVoidItem, SHORT sStorageType);
    VOID    TrnStoSalesUpdateRedCpn(SHORT sVoidCpnCo, USHORT usItemOff);
    VOID    TrnStoNoConsNormalRed(VOID *pData, USHORT usItemOffset);
    VOID    TrnStoNoItemNormalRed(VOID *pData, USHORT usItemOffset);

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION COMMON MODULE)                  V3.3
--------------------------------------------------------------------------*/
    VOID    TrnSalesDeptUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesDeptCanComn(ITEMSALES *pItem, TRANITEMIZERS *pItemizer,
                    TRANGCFQUAL *pGCF, LONG lSign);
    VOID    TrnSalesDeptIntlComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableCanComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableIntlComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnDiscTaxUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnDiscTaxCanComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnDiscTaxCanCom(UCHAR fbStatus,
                UCHAR fbTaxStatus, LONG lAmount, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModCanComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModIntlComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountUSComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountCanComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountIntlComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnRegDiscTaxRateComn( ITEMDISC *pItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscTaxAmtComn( ITEMDISC *pItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscTaxCanComn( ITEMDISC *ItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscVATCalcComn(ITEMDISC *pDisc, SHORT sType, LONG lSign);
    VOID    TrnItemCouponUSComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemCouponCanComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemCouponIntlComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);


/*--------------------------------------------------------------------------
*        external static data
--------------------------------------------------------------------------*/
                                                                                  
    extern  TRANINFORMATION     TrnInformation;         /* transaction information */
            
    extern  UCHAR FARCONST  auchNEW_FILE_READ_WRITE[];        
    extern  UCHAR FARCONST  auchOLD_FILE_READ_WRITE[];   
    extern  UCHAR FARCONST  auchOLD_FILE_READ[];         

/* temporary file on DRAM, saratoga */
    extern UCHAR FARCONST auchTEMP_NEW_FILE_WRITE[];
    extern UCHAR FARCONST auchTEMP_NEW_FILE_READ_WRITE[];
    extern UCHAR FARCONST auchTEMP_OLD_FILE_WRITE[];
    extern UCHAR FARCONST auchTEMP_OLD_FILE_READ_WRITE[];
    extern UCHAR FARCONST auchTEMP_OLD_FILE_READ[];

    extern  TRANINFSPLIT    TrnSplitA;
    extern  UCHAR FARCONST  aszTrnConsSplitFileA[];
    extern  UCHAR FARCONST  aszTrnConsSplitIndexA[];
    extern  TRANINFSPLIT    TrnSplitB;
    extern  UCHAR FARCONST  aszTrnConsSplitFileB[];
    extern  UCHAR FARCONST  aszTrnConsSplitIndexB[];


/****** End of Definition ******/
