/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
=======================================================================*\
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
:                      :          :              :                        :
:** NCR2172 **         :          :              :                        :
:                      :          :              :                        :
:   M.Teraki           : 99/10/01 : 01.00.00     : Initial(for Win32)     :
:   hrkato             : 99/12/03 : 01.00.00     : Saratoga               :
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
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

#define TRN_WORKBUFFER          (sizeof(ITEMDATASIZEUNION))    /* work buffer for compress 5/24/94 */

#define TRN_1ITEM_BYTES         250          /* storage bytes for 1 item, item plus discount, saratoga */
#define TRN_STOR_BASE_BYTES     250          /* storage base bytes for 1 transaction */
#define TRN_STOSIZE_BLOCK       4096         // max transaction data size. also used in TrnCalStoSize() to calculate ulActualSize, size same as GCF_DATA_BLOCK_SIZE

#define TRN_TEMPBUFF_SIZE       (TRN_STOSIZE_BLOCK*2)  /* temp buff max size, Saratoga */

/*----- Type of Canadian Tax Calculation Split/Normal V3.3 -----*/
#define         TRN_NOT_SPLIT           0       /* Normal */
#define         TRN_SPLIT               1       /* Split Check */
#define         TRN_NOT_DISCTAX         2       /* not discount/tax */

#define         TRN_ITEMIZE_NORMAL       1L     /* normal discount/surcharge calculations */
#define         TRN_ITEMIZE_REDUCE      -1L     /* for reduce discount/surcharge calculations */

#define			TRN_ITEMSTORE_SIZE		1000
/* #define			TRN_ITEMSTORE_SIZE		300 */


/*--------------------------------------------------------------------------
*       transaction open compressed data                       R3.1
--------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

// create a void pointer from a transaction data item record which points to the beginning of
// the actual item data record. An item data record is normally composed of two pieces,
// the non-compressed data and the compressed data.
// the very first byte of the transaction data record contains the length of the non-compressed
// data section of the record which is an offset to where the USHORT that contains the length of
// the compressed data section is located.
// see also ITEMSTORAGENONPTR() for accessing the uncompressed item record data.
#define TRANSTORAGENONPTR(x)  ((VOID *)((UCHAR *)(x) + 1))

// in some cases we want to check the major class and possible minor class of a transaction data record.
#define TRANSTORAGEDATAPTR(x) ((TRANSTORAGERECORD *)(x))

	/* The following struct provides the necessary template
	 * for a transaction item data record as stored in a transaction file.
	 * The transaction data is stored as a variable length record made up of
	 * two sections. The first section is the non-compressed data and the second
	 * section is the compressed data. The non-compressed data is the most commonly
	 * used data during processing so as to reduce the overhead of uncompressing the
	 * transaction item data every time it is accessed.
	 */
	typedef struct {
		UCHAR    uchZipSizeOffSet;      // number of bytes to USHORT with compressed length.
		UCHAR    uchMajorClass;         // major class, CLASS_ITEMSALES, etc.
		UCHAR    uchMinorClass;         // minor class indicating tender key number, etc.
		UCHAR    uchSeatNo;             /* Seat number, not used for all major classes, may have a value of 0 or 'B' for Base Transaction */
		UCHAR    fbReduceStatus;        /* Reduce Status, not used for all major classes */
	} TRANSTORAGERECORD;

	// following non-compressed section is used by several of the ITEM types
	// for their transaction records stored in the transaction data file.
    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
    }TRANSTORAGETRANSMININON;

	// changes to these two structs must be coordinated with changes to struct ITEMTRANSOPEN.
    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#,     R3.1 */
    }TRANSTORAGETRANSOPENNON;

    typedef struct {
        ULONG       ulCashierID;            /* 3: cashier ID */
        ULONG       ulWaiterID;             /* 4: waiter ID */
        USHORT      usGuestNo;              /* 6:guest check No */
        UCHAR       uchCdv;                 /* 7:guest check number check digit, MDC 12 Bit C */
        USHORT      usTableNo;              /* 8:table No */
        USHORT      usNoPerson;             /* 9:No of person */
        UCHAR       uchLineNo;              /* 10:line No */
        USHORT      fbModifier;             /* 11:tax modifier */
        DCURRENCY   lAmount;                /* 13:previous amount */
        UCHAR       auchCheckOpen[6];       /* 15:time (HH:MM:SS) and date (YY/MM/DD) check open */
        USHORT      fsPrintStatus;          /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;        /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchStatus;              /* 18:MDC status */
        UCHAR       uchTeamNo;              /* 19:Team No, V3.3 */
        UCHAR		uchHourlyOffset;        /* Hourly total block offset, TRANSTORAGETRANSOPEN */
        UCHAR       auchCheckHashKey[6];    /* date and time as mm.dd.hh.mm.ss.0, */
    }TRANSTORAGETRANSOPEN;



/*--------------------------------------------------------------------------
*        sales data
--------------------------------------------------------------------------*/

	// WARNING:  There are several places where there is a dependency
	//           on the first few members of this struct being in the
	//           same order and of the same type as some other structs.
	//           See function TrnIsSameZipDataSalesNon() for an example.
	//           Structs that need to be compatible in the first five members
	//           uchMajorClass, uchMinorClass, uchSeatno, auchPLUNo, and usDeptNo
	//           include but are not limited to the following structs:
	//              ITEMSALES, TRANSTORAGESALESNON, 
    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#, may have a value of 0 or 'B' for Base Transaction    R3.1 */
        TCHAR       auchPLUNo[NUM_PLU_LEN]; /* PLU No, 2172 */
        USHORT      usDeptNo;               /* dept No, 2172 or order declaration number (uchOrderDecNum) for CLASS_ITEMORDERDEC */
        LONG        lQTY;                   /* quantity of product sold in thousandths (due to scale units of .001). See PLU_BASE_UNIT */
        DCURRENCY   lProduct;               /* product, see also PRTIDX member lProduct dependency */
        SHORT       sCouponQTY;             /* quantity for coupon */
        SHORT       sCouponCo;              /* coupon counter */
        USHORT      usItemOffset;           /* Offset of Partner, R3.1 */
        UCHAR       uchPrintPriority;       /* print priority */
        USHORT      uchPPICode;             /* PPI Code   ESMITH */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
        USHORT      fsLabelStatus;          /* label control status from GP, 2172 */
		USHORT		usUniqueID;
		USHORT		usKdsLineNo;			// line number on the KDS display
    }TRANSTORAGESALESNON;



    typedef struct {
        TCHAR           auchPLUNo[NUM_PLU_LEN];   /* PLU No, 2172 */
        USHORT          usDeptNo;               /* dept No, 2172 */
        UCHAR           uchPrintPriority;   /* print priority */
		DUNITPRICE      lUnitPrice;         /* price */
        UCHAR           uchAdjective;       /* adjective or order declaration number (uchOrderDecNum) for CLASS_ITEMORDERDEC */
        ITEMCONTCODE    ControlCode;        /* dept, report code, status */
		UCHAR			uchCondColorPaletteCode;	/* color palette code */
    }TRANSTORAGECOND;

/*
*       WARNING:  Any changes made to the TRANSTORAGESALES and ITEMSALES struct
*       must be reflected in the compression function, TrnStoSalesComp() as
*       well as in the uncompression function RflGetStorageItem().
*/
    typedef struct {
		DUNITPRICE      lUnitPrice;             /* unit price */
        UCHAR           uchAdjective;           /* adjective  or order declaration number (uchOrderDecNum) for CLASS_ITEMORDERDEC */
        USHORT          fbModifier;             /* void, tax modifier, TRANSTORAGESALES */
        UCHAR           fbModifier2;            /* price check, e-version, 2172, TRANSTORAGESALES */
        TCHAR           aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];   /* US Customs number */
        ITEMCONTCODE    ControlCode;            /* dept, report code, status */
        UCHAR           uchPrintModNo;          /* No of P.M */
        UCHAR           auchPrintModifier[STD_MAX_COND_NUM];  /* print modifier */
        USHORT          fsPrintStatus;          /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR           uchCondNo;              /* No of condiment */
        UCHAR           uchChildNo;             /* no. of child plu */
        TRANSTORAGECOND Condiment[STD_MAX_COND_NUM];          /* condiment */
        UCHAR           uchRate;                /* modifier discount rate */
        DCURRENCY       lDiscountAmount;        /* discount amount */
        UCHAR           fbStorageStatus;        /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR           uchRndType;             /*  random weight type, 2172 */
        UCHAR           uchPM;                  /*  price multiple, 2172     */
        USHORT          usLinkNo;               /* link plu no, 2172 */
        USHORT          usFamilyCode;           /* family code, 2172 */
		UCHAR			uchTableNumber;
		UCHAR			uchDiscountSignifier;   //US Customs cwunn 5/1/03, signifies this item has had a discount applied.
		USHORT			usFor;					/* @/FOR storage of package price (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package price is $1.00 */
		USHORT			usForQty;				/* @/For storage of package size (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package size is 3.*/
		UCHAR			uchColorPalette;		/* Color Palette code, TRANSTORAGESALES */ //colorpalette-changes
        UCHAR			uchHourlyOffset;        /* Hourly total block offset, TRANSTORAGESALES */
		USHORT			usTareInformation;		//tare information for specific PLU.
		USHORT			blPrecedence;			// Precedence allowed?
		USHORT			usReasonCode;           //a reason code associated with this operator action. TRANSTORAGESALES
    }TRANSTORAGESALES;



/*--------------------------------------------------------------------------
*        discount data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchSeatNo;              /* Seat#, may have a value of 0 or 'B' for Base Transaction      R3.1 */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
        DCURRENCY   lAmount;                /* 6:discount amount */
        LONG        lQTY;                   /* quantity of product sold in thousandths (due to scale units of .001). See PLU_BASE_UNIT */
    }TRANSTORAGEDISCNON;

	// WARNING:  Any changes made to this struct TRANSTORAGEDISC should also be made to
	//           struct ITEMDISC which is used during the storing of a transaction.
    typedef struct {
        TCHAR       auchPLUNo[NUM_PLU_LEN]; /* PLU No, 2172 */
        UCHAR       uchAdjective;               /* 4:adjective */
        USHORT      usDeptNo;               /* dept No, 2172 */
        UCHAR       uchRate;                    /* 5:rate */
        TCHAR       aszNumber[NUM_NUMBER];            /* 7:number */
        USHORT      fbDiscModifier;             /* 9:void, tax modifier */
        UCHAR       auchDiscStatus[3];          /* 12:status */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        ULONG       ulCashierNo;                /* 14:waiter ID */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        ITEMVOIDDISC VoidDisc;				   /* TAR191625 */
		UCHAR        uchDiscountNo;         // item discount number or other identifier as to sub-type of discount, TRANSTORAGEDISC
        UCHAR        uchHourlyOffset;       /* Hourly total block offset, TRANSTORAGEDISC */
		USHORT       usReasonCode;          //a reason code associated with this operator action.
        TCHAR        aszMnemonic[NUM_DEPTPLU];     /* discount mnemonic    */
    }TRANSTORAGEDISC;



/*--------------------------------------------------------------------------
*        total key data
	WARNING:  The following struct must be similar to the struct
		ITEMTOTAL as defined in regstrct.h as the function TrnStoTotalComp()
		and possibly others depends on the similarity.
		The similarity begins with ITEMTOTAL struct member fbModifier.
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT      fbModifier;                 /* tax modifier */
        DCURRENCY   lMI;                        /* MI  */
        DCURRENCY   lService;                   /* service total */
        DCURRENCY   lTax;                       /* whole tax amount */
        TCHAR       aszNumber[NUM_NUMBER];            /* number */
        UCHAR       auchTotalStatus[NUM_TOTAL_STATUS];  /* total key parameter,   R3.1,  auchTotalStatus[0] contains total key number and type */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        USHORT      usOrderNo;                  /* order No or guest check number */
        UCHAR       uchCdv;                     /* guest check number check digit, MDC 12 Bit C */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchLineNo;                  /* 15:line No */
        USHORT      usConsNo;                   /* 16:consecutive No */
        ULONG       ulCashierNo;                /* 17:ID(cashier/waiter) */
        SHORT       sTrayCo;                    /* tray item counter */
        USHORT      usTrayNo;                   /* current tray number when using tray total. Incremented each tray total. */
        TCHAR       aszRoomNo[NUM_ROOM];               /* room number */
        TCHAR       aszGuestID[NUM_GUESTID];            /* guest ID */
        TCHAR       aszFolioNumber[NUM_FOLIO];          /* folio number */
        TCHAR       aszPostTransNo[NUM_POSTTRAN];          /* posted transaction number */
        TCHAR       aszCPText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];  /* charge posting response text */
        TCHAR       auchExpiraDate[NUM_EXPIRA];          /* expiration date       */
		USHORT      usReasonCode;                        //a reason code associated with this operator action.
    }TRANSTORAGETOTAL;



/*--------------------------------------------------------------------------
*        tender data

* The TRANSTORAGETENDER struct must be in the same member order and same member sizes as
* the ITEMTENDER struct beginning with member fbModifier in order for function TrnStoTenderComp()
* to correctly work.
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
    }TRANSTORAGETENDERNON;

    typedef struct {
        USHORT      fbModifier;                 /* 3:void */
        DCURRENCY        lRound;                     /* 4:round amount */
        ULONG       ulFCRate;                   /* 5:foreign currency rate */
        DCURRENCY   lTenderAmount;              /* 7:tender amount */
        DCURRENCY   lForeignAmount;             /* 8:foreign amount */
        DCURRENCY   lBalanceDue;                /* 9:balance due */
        DCURRENCY   lChange;                    /* 10:change amount */
        DCURRENCY   lFSChange;                  /* F.S credit           */
        DCURRENCY   lFoodStamp;                 /* food stamp, TAR111547 */
        DCURRENCY   lGratuity;                  /* gratutity or charge tip ammount */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
		USHORT      usCheckTenderId;                    // Unique tender id for split checks identifying specific tender
		USHORT      usReasonCode;                       //a reason code associated with this operator action.
        TCHAR       aszRoomNo[NUM_ROOM];               /* 17:room number */
        TCHAR       aszGuestID[NUM_GUESTID];              /* 23:guest ID */
        TCHAR       aszFolioNumber[NUM_FOLIO];          /* 26:folio number */
        TCHAR       aszPostTransNo[NUM_POSTTRAN];          /* 33:posted transaction number */
        TCHAR       aszNumber[NUM_NUMBER];      /* 11:number */
        TCHAR       auchExpiraDate[NUM_EXPIRA];          /* 81:expiration date       */
        TCHAR       auchApproval[NUM_APPROVAL];            /* 85:EPT approval code     */
        TCHAR       auchMSRData[NUM_MSRDATA_PADDED];   /* MSR data */
        TCHAR       aszTendMnem[NUM_LEADTHRU];              /* Display Mnem for CP, Saratoga  */
        TCHAR       aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];  /* 40:charge posting response text, Saratoga */
		ITEMTENDERCHARGETIPS   ChargeTipsInfo;
		TCHAR       tchRoutingNumber[NUM_ROUTING_NUM];	// XEPT routing number, used for check auth
		TCHAR       tchCheckSeqNo[NUM_SEQUENCE_NUM];	//XEPT Check Sequence number, used for check auth
		UCHAR       tchAcqRefData[NUM_ACQREFDATA];	    // XEPT Acquirer Reference Number - used for pre-authorization
		ITEMTENDERAUTHCODE  authcode;                   // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		                // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO     invno;                      // XEPT Invoice number from processor of transaction for EEPT
		ITEMTENDERSRNO      srno;                       // XEPT  RecordNo - from DSI Client XML.  record identifier in current batch.
        UCHAR       auchTendStatus[3];          /* 14:status */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchCPLineNo;                /* charge posting line number */
        UCHAR       uchPrintSelect;                 /* Print Selection for EPT, Saratoga */
        UCHAR       uchTenderOff;               /* Tender Offset for EPT,   Saratoga */
        UCHAR       uchBuffOff;                 /* Offset of Saved Buffer for EPT,  Saratoga */
		UCHAR	    uchPaymentType;					    //used for contactless or swipe types of transactions
		UCHAR	    uchTenderYear;
		UCHAR	    uchTenderMonth;
		UCHAR	    uchTenderDay;
		UCHAR	    uchTenderHour;
		UCHAR	    uchTenderMinute;
		UCHAR	    uchTenderSecond;
    }TRANSTORAGETENDER;


/*--------------------------------------------------------------------------
*        misc. transaction data for transaction record compressed data
*        coordinate any changes with ITEMMISC struct.
--------------------------------------------------------------------------*/
    typedef struct {
        ULONG       ulCashierNo;                /* 3:cashier/waiter ID */
        TCHAR       aszNumber[NUM_NUMBER];      /* 5:number */
        USHORT      usGuestNo;                  /* 7:guest check No */
        UCHAR       uchCdv;                     /*   guest check number check digit, MDC 12 Bit C */
        DCURRENCY   lAmount;                    /* 8:amount */
        USHORT      fbModifier;                 /* 10:void */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        USHORT      usYear;                     /* 13:Year to be time in */
        USHORT      usMonth;                    /* 15:Month to be time in */
        USHORT      usDay;                      /* 17:Day to be time in */
        ULONG       ulEmployeeNo;               /* 19:Employee Number R3.1 */
        UCHAR       uchJobCode;                 /* 21:Job Code to be time in */
        USHORT      usTimeinTime;               /* 22:Time to be time in */
        USHORT      usTimeinMinute;             /* 24:Minute to be time in */
        USHORT      usTimeOutTime;              /* 26:Time to be time out */
        USHORT      usTimeOutMinute;            /* 28:Minute to be time out */
        TCHAR       aszMnemonic[NUM_ETK_NAME];  /* ETK mnemonic,         R3.1 */
        UCHAR       uchTendMinor;               /*  tender minor 2172 */
        LONG        lQTY;                       /*  quantity 2172    */
		DUNITPRICE  lUnitPrice;                 /*  unit price 2172  */
        UCHAR       fchStatus;                  /*  foreign cuurency status 2172 */
		USHORT      usReasonCode;               //a reason code associated with this operator action.
    }TRANSTORAGEMISC;


/*--------------------------------------------------------------------------
*       other data                              R3.1
*   Any changes to this struct TRANSTORAGEOTHER should be coordinated with changes
*   to struct ITEMOTHER and vice versa.
--------------------------------------------------------------------------*/
    typedef struct {
        DCURRENCY    lAmount;                        /* 3:amount */
        DCURRENCY    lBalanceDue;                    /* 3:amount */
        DCURRENCY    lChange;                        /* 3:change amount */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        /* V3.3 */
        UCHAR   uchAction;                      /* action code  */
        UCHAR   uchDayPtd;                      /* daily/ptd    */
        UCHAR   uchCurSave;                     /* current/saved*/
        UCHAR   uchReset;                       /* issued or not*/
        UCHAR   uchError;                       /* error code   */
        UCHAR   uchYear;                        /* year         */
        UCHAR   uchMonth;                       /* month        */
        UCHAR   uchDay;                         /* day          */
        UCHAR   uchHour;                        /* hour         */
        UCHAR   uchMin;                         /* minute       */
        TCHAR   aszMnemonic[24];                /* mnemonics, sized to OTHER_MNEM_SIZE + 1    */
        TCHAR   auchMSRData[NUM_MSRDATA_PADDED];       /* MSR data,    2172   */
        USHORT  husHandle;                      /* Tmp File Handle, 2172 */
        TCHAR   aszNumber[NUM_NUMBER];          /* number,  Saratoga */
    }TRANSTORAGEOTHER;


/*--------------------------------------------------------------------------
*        affection data transaction storage - compressed section
*        any changes must be coordinated with ITEMAFFECTION struct.
--------------------------------------------------------------------------*/

    typedef struct {
        DCURRENCY   lAmount;                    /* 3:amount */
        SHORT       sItemCounter;               /* 4:item counter */
        SHORT       sNoPerson;                  /* 5:No of person */
        SHORT       sCheckOpen;                 /*   check open counter */
        UCHAR       uchOffset;                  /* 6:hourly block offset, same as uchHourlyOffset other structs. */
        USHORT      fbModifier;                 /*    modifier */
        USCANVAT    USCanVAT;
        DCURRENCY   lService[STD_MAX_SERVICE_TOTAL];  /*13: service total */
        DCURRENCY   lCancel;                    /*14: cancel total */
        ULONG       ulCashierNo;                /*15: cashier/waiter ID */
        USHORT      usGuestNo;                  /*16: guest check No */
        UCHAR       uchTotalID;                 /*17: totalkey Id */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchAddNum;                  /*21: addcheck number */
        UCHAR       uchStatus;                  /*    MDC status */
        DCURRENCY   lBonus[STD_NO_BONUS_TTL];   /* bonus total for hourly, V3.3 */
    }TRANSTORAGEAFFECTION;



/*--------------------------------------------------------------------------
*        print data
*	Any changes to this struct need to also be made in struct ITEMPRINT since
*   that struct is used in the various print routines and decompression goes there.
--------------------------------------------------------------------------*/

    typedef struct {
        ULONG       ulCashierNo;                /* 4:waiter/cashier ID */
        ULONG       ulStoreregNo;               /* 6:store/reg No. See ItemCountCons() - encoded as store number * 1000L + register number */
        USHORT      usConsNo;                   /* 7:consecutive No */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        USHORT      usTableNo;                  /* 11:table No */
        USHORT      usGuestNo;                  /* 13:guest check number */
        UCHAR       uchCdv;                     /* 14:guest check number check digit, MDC 12 Bit C */
        UCHAR       uchLineNo;                  /* 15:line No */
        TCHAR       aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];      /* 280:number        *add EPT, saratoga */
        TCHAR       aszRoomNo[NUM_ROOM];        /* 19:room number */
        TCHAR       aszGuestID[NUM_GUESTID];    /* 25:guest ID */
        TCHAR       aszCPText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];  /* message text, Saratoga */
        UCHAR       uchCPLineNo;                /* 326:charge posting line number */
        UCHAR       uchPrintSelect;             /* Print Selection for EPT, Saratoga */
        USHORT      usFuncCode;                 /* 268:function code */
        SHORT       sErrorCode;                 /* 270:error code */
        DCURRENCY   lAmount;                    /* 272:amount        *add EPT*/
        DCURRENCY   lMI;                        /* MI,  DTREE#2 Dec/18/2000 */
        UCHAR       uchStatus;                  /* 278:EPT status    *add EPT*/
        USHORT      fbModifier;                 /* 279:Modifier      *add EPT*/
        TCHAR       auchExpiraDate[NUM_EXPIRA]; /* 300:ExpDate       *add EPT*/
        TCHAR       auchApproval[NUM_APPROVAL]; /* 304:approval code *add EPT*/
		ITEMTENDERAUTHCODE  authcode;               // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		            // XEPT Reference Number generated for processor of transaction for EEPT
		UCHAR       uchUniqueIdentifier[24];    // Transaction Unique Identifier to print as bar code if trailer
    }TRANSTORAGEPRINT;



/*--------------------------------------------------------------------------
*        multi check payment data
--------------------------------------------------------------------------*/

    typedef struct{
        DCURRENCY   lService[STD_MAX_SERVICE_TOTAL];  /* 4: service total */
        USHORT      usGuestNo;                  /* 5: guest check number */
        UCHAR       uchCdv;                     /* guest check number check digiti, MDC 12 Bit C */
        DCURRENCY   lMI;                        /*    MI */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
    }TRANSTORAGEMULTICHECK;



/*--------------------------------------------------------------------------
*       operator close data
--------------------------------------------------------------------------*/

    typedef struct{
        ULONG   ulCashierNo;                    /* cashier/waiter ID */
        UCHAR   uchUniqueAdr;                   /* unique address */
    }TRANSTORAGEOPECLOSE;



/*--------------------------------------------------------------------------
*       combination coupon data used for storing coupon data.
*
*       any changes to this struct need to be coordinated with changes to ITEMCOUPON.
*
*       Also will be required changes to the uncompress function RflGetCouponItem() and the
*       compress function TrnStoCouponComp() which handle the compress/uncompress transformation.
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;
        UCHAR       uchMinorClass;
        UCHAR       uchSeatNo;              /* Seat#, may have a value of 0 or 'B' for Base Transaction      R3.1 */
        UCHAR       fbReduceStatus;         /* Reduce Status,  R3.1 */
        UCHAR       uchCouponNo;
        TCHAR       auchUPCCouponNo[NUM_PLU_LEN];
        SHORT       sVoidCo;
        USHORT      usItemOffset[ STD_MAX_NUM_PLUDEPT_CPN ];          /* Moved from Comp Area, R3.1 */
    } TRANSTORAGECOUPONNON;

    typedef struct {
        DCURRENCY   lAmount;
        DCURRENCY   lDoubleAmount;              /* coupon amount for future use */
        DCURRENCY   lTripleAmount;              /* coupon amount for future use */
        USHORT      fbModifier;
        UCHAR       fbStatus[ 2 ];
        TCHAR       aszMnemonic[NUM_DEPTPLU];   /* mnemonic */
        TCHAR       aszNumber[ NUM_NUMBER ];
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       uchNoOfItem;
        TCHAR       auchPLUNo[NUM_PLU_LEN];     /* target PLU number for affection, 2172 */
        USHORT      usDeptNo;                   /* target Dept number for affection , 2172 */
        UCHAR       uchAdjective;               /* target adjective number for affection, 2172*/
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchHourlyOffset;            /* Hourly total block offset, TRANSTORAGECOUPON */
		USHORT		usBonusIndex;				//SR 157 Bonus Totals to 100 JHHJ
		USHORT  	usReasonCode;               //a reason code associated with this operator action.
    } TRANSTORAGECOUPON;



/*--------------------------------------------------------------------------
*       sales item search data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR          auchBuffer[ TRN_WORKBUFFER ];
        TrnVliOffset   usItemOffset;
		USHORT         usItemSize;
		TrnVliOffset   usDiscOffset;
		UCHAR          *pDiscBuffer;
    } TRANSTOSALESSRCH;


/*--------------------------------------------------------------------------
*       Loan/Pick-up data,      Saratoga
--------------------------------------------------------------------------*/
#if defined(POSSIBLE_DEAD_CODE)
// This function to store a Loan or Pickup into the transaction data file is not used
// because Loan and/or Pickup (AC10 or AC11) are used from Supervisor Mode hence not put
// into a transaction or a Pickup is used from Operator Mode however it is not part of a
// transaction either.
// The transaction file with the item compressed and stored into the transaction file is
// only used for items (sales items, affectation, discounts, total/tender, etc.) that are
// part of a normal transaction. Loan and Pickup is not part of a normal transaction.
//    Richard Chambers, Oct-05-2016
    typedef struct {
        ULONG       ulCashierNo;           /* Cashier Number */
        USHORT      usModifier;            /* Loan/Pickup Modifier Status */
        DCURRENCY   lForQty;               /* For, Qty Amount */
        DCURRENCY   lUnitAmount;           /* Qty Unit Amount */
        DCURRENCY   lNativeAmount;         /* Native Amount for FC */
        UCHAR       uchFCMDC;              /* FC Control MDC */
        UCHAR       uchFCMDC2;             /* FC Control MDC, V2.2 */
        DCURRENCY   lAmount;               /* Loan/Pickup Amount */
        DCURRENCY   lTotal;                /* Loan/Pickup Total Amount */
    } TRANSTORAGELOANPICK;
#endif

	typedef struct {
        TCHAR           auchPLUNo[NUM_PLU_LEN];        /*  PLU No, 2172    */
        TCHAR           aszMnemonic[NUM_DEPTPLU];      /* mnemonic */
        TCHAR           aszMnemonicAlt[NUM_DEPTPLU];   /* alternate mnemonic */
	} TRNPLUMNEMO;

	typedef struct {
		TRNPLUMNEMO aPluMnemo[TRN_ITEMSTORE_SIZE];
	} TRNITEMSTOREPLUMNEM;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/*--------------------------------------------------------------------------
*        prototype declaration (TRANSACTION MODULE BODY)
--------------------------------------------------------------------------*/

    SHORT   TrnOpeOpen( ITEMOPEOPEN *pItemOpeOpen );
    SHORT   TrnTransOpen( ITEMTRANSOPEN *pItemTransOpen );
	SHORT   TrnSalesAndDisc ( ITEMSALES *pItemSales, ITEMDISC *pItemDisc );
    SHORT   TrnSales( ITEMSALES *pItemSales );
    SHORT   TrnDiscount( ITEMDISC *pItemDiscount );
    SHORT   TrnCoupon( ITEMCOUPON *pItemCoupon );
    SHORT   TrnTotal( ITEMTOTAL *pItemTotal );
    SHORT   TrnTender( ITEMTENDER *pItemTender );
    SHORT   TrnMisc( ITEMMISC *pItemMisc );
    SHORT   TrnModifier( ITEMMODIFIER *pItemModifier );
    VOID    TrnOther( ITEMOTHER *pItemOther );
    VOID    TrnOtherDemand( VOID );
	VOID    TrnOtherSummaryReceipt ( ITEMTENDER *pWorkOther, int iPrintItems );
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

    SHORT   TrnCreatePostRecFile( ULONG ulConsFileSize );
    SHORT   TrnCreateIndexFile( USHORT  usNoOfItem, TCHAR  *lpszIndexName, ULONG *pulCreatedSize );
    SHORT   TrnICOpenIndexFile( VOID );

    SHORT   TrnCouponVoidSearch( ITEMCOUPONSEARCH *pItemSearch, TrnStorageType   sStorageType );
    SHORT   TrnTaxSystem(VOID);
    VOID    TrnTransCloseLoanPickup(ITEMTRANSCLOSE *ItemTransClose); /* 09/08/00 */


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION QUALIFIER MODULE)
--------------------------------------------------------------------------*/

    UCHAR	*TrnGetTransactionInvoiceNum(UCHAR *auchRefNum);
	VOID	TrnSaveTransactionInvoiceNum(ITEMTENDER *pTenderToSave);
	SHORT	TrnGetTransactionInvoiceNumTenderIfValid(ITEMTENDER *pTender);
	VOID	TrnClearSavedTransactionInvoiceNum(VOID);
	UCHAR	*TrnGenerateTransactionInvoiceNum(UCHAR *auchRefNum);
	UCHAR	*TrnGenerateTransactionInvoiceNumFromTender(UCHAR *pauchRefNum, ITEMTENDER *pItemTender);
	VOID    TrnGetTransactionCardHolder(TCHAR *tchRefNum);
	VOID    TrnPutTransactionCardHolder(TCHAR *tchCardHolder);
    VOID    TrnQual( VOID *pTrnQual );
    VOID    TrnQualOpn( ITEMOPEOPEN *pItemOpeOpen );
    VOID    TrnQualTransCas( VOID );
    VOID    TrnQualTransNC( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransRO( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransSR( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualTransCash( ITEMTRANSOPEN *pItemTransOpen );
    VOID    TrnQualLoanPick(ITEMTRANSOPEN *pItem);      /* Saratoga */
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
    VOID    TrnQualOther(ITEMOTHER *pItem);             /* Saratoga */
    VOID    TrnQualCls( ITEMOPECLOSE *pItemOpeClose );
    VOID    TrnQualClsCashier( VOID );
	ULONG   TrnQualModifierReturnData (USHORT *pusReturnType, USHORT *pusReasonCode);
	ULONG   TrnQualModifierReturnTypeTest (VOID);
	ULONG   TrnQualModifierReturnTypeSet (ULONG fbModifier);

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION ITEMIZERS MODULE)
--------------------------------------------------------------------------*/

	UCHAR   TrnSalesItemIsFoodStamp (CONST UCHAR auchPluStatus[ITM_PLU_CONT_OTHER]);  // from ITEMCONTCODE, ControlCode.auchPluStatus[12]
    SHORT   TrnItemizer( VOID * );
    VOID    TrnItemTrans( ITEMTRANSOPEN *ItemTransOpen );
    VOID    TrnItemRO( ITEMTRANSOPEN *ItemTransOpen );
    VOID    TrnItemSales( ITEMSALES *ItemSales );
    VOID    TrnSalesDept(CONST ITEMSALES *ItemSales );
	VOID    TrnSalesAdjustItemizers ( ITEMGENERICHEADER *pItem );
    SHORT   TrnQTY( CONST ITEMSALES *ItemSales );
    VOID    TrnSalesPLU( ITEMSALES *ItemSales );
    VOID    TrnTaxable( ITEMSALES *ItemSales );
    VOID    TrnDiscountable( ITEMSALES *ItemSales );
    VOID    TrnDiscTax( ITEMSALES *ItemSales );
    VOID    TrnPLUQTY( ITEMSALES *ItemSales );
    VOID    TrnSalesMod( ITEMSALES *ItemSales );
    VOID    TrnItemDisc( ITEMDISC *ItemDisc );
    VOID    TrnItemDiscount( ITEMDISC *ItemDisc );
    VOID    TrnRegDiscount( ITEMDISC *ItemDisc );
    VOID    TrnRegDiscVATCalc(ITEMDISC *pDisc, SHORT sType);
    VOID    TrnChargeTip( ITEMDISC *ItemDisc );
    VOID    TrnItemAC( ITEMAFFECTION *ItemAffection );
    VOID    TrnItemCoupon( ITEMCOUPON *pItemCoupon );
    VOID    TrnItemMisc(ITEMMISC *pData);
    VOID    TrnItemTotal( ITEMTOTAL *pItemTotal );


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION STORAGE MODULE)
--------------------------------------------------------------------------*/

    SHORT   TrnStoOpen( ITEMTRANSOPEN *ItemTransOpen );
    SHORT   TrnStoOpenComp( ITEMTRANSOPEN *ItemTransOpen, UCHAR *pData );
	SHORT   TrnStoSalesAndDisc ( ITEMSALES *pItemSales, ITEMDISC *pItemDisc );
    SHORT   TrnStoSales( ITEMSALES *ItemSales );
    SHORT   TrnStoSalesItemUpdate( ITEMSALES *pItemSales );
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

    TrnVliOffset  TrnStoItemNormal( VOID *pData, SHORT sSize );
    VOID    TrnStoNoItemNormal( VOID *pData, TrnVliOffset usItemOffset );
	VOID	TrnStoPreviousCondimentIndex( TRANSTOSALESSRCH *pDuplicateItem,TRANSTOSALESSRCH *pNewItem, 
													VOID *pData, SHORT sSize, SHORT sType ); //SR 192
	VOID	TrnStoPreviousAdjectiveIndex( TRANSTOSALESSRCH *pDuplicateItem,TRANSTOSALESSRCH *pNewItem, 
													VOID *pData, SHORT sSize, SHORT sType ); //SR 192
    TrnVliOffset  TrnStoConsNormal( VOID *pData, SHORT sSize );
    VOID    TrnStoNoConsNormal( VOID *pData, TrnVliOffset usItemOffset );
	SHORT   TrnStoReplaceAppendIndex ( TRANSTORAGESALESNON *pTranNon, TrnVliOffset  usItemOffset, TrnStorageType  sStorageType );
    SHORT   TrnStoAppendIndex( TRANSTORAGESALESNON *pTranNon, TrnVliOffset  usItemOffset, TrnStorageType sStorageType );
	SHORT   TrnStoReplaceIndex ( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType );
	SHORT   TrnStoReplaceInsertIndex (TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType );
    SHORT   TrnStoInsertIndex( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType );
	SHORT   TrnStoInsertIndexFileHandle (SHORT hsIndexFile, TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset );
	SHORT   TrnStoItemSearchAll( ITEMSALES *pItemSales, TrnStorageType sStorageType);
    SHORT   TrnStoSalesSearch(ITEMSALES        *pItemSales,
                              UCHAR            *puchSrcZip,
                              TRANSTOSALESSRCH *pNormalItem,
                              TRANSTOSALESSRCH *pVoidItem,
                              TrnStorageType    sStorageType,
                              SHORT             sType);
	
    SHORT   TrnStoSalesSearchIndex(ITEMSALES     *pItemSales,
                              UCHAR              *puchSrcZip,
                              TRANSTOSALESSRCH   *pNormalItemCons,
                              TRANSTOSALESSRCH   *pVoidItemCons,
                              TRANSTOSALESSRCH   *pSameItemCons,
                              TRANSTOSALESSRCH   *pNormalItem,
                              TRANSTOSALESSRCH   *pVoidItem,
                              TRANSTOSALESSRCH   *pSameItem,
                              TrnStorageType    sStorageType,
                              SHORT             sType);
	
    SHORT   TrnStoSalesUpdate(ITEMSALES        *pItemSales,
                              TRANSTOSALESSRCH *pCompItem,
                              TRANSTOSALESSRCH *pNormalItem,
                              TRANSTOSALESSRCH *pVoidItem,
                              TrnStorageType    sStorageType);
    SHORT   TrnStoSalesUpdateIndex(ITEMSALES        *pItemSales,
                              TRANSTOSALESSRCH *pCompItem,
                              TRANSTOSALESSRCH *pNormalItem,
                              TRANSTOSALESSRCH *pVoidItem,
                              TRANSTOSALESSRCH *pSameItem,
                              TrnStorageType    sStorageType);
    SHORT   TrnStoIsPairItem( ITEMSALES *pItemSales, UCHAR *puchTarget );
	SHORT   TrnStoIsItemInWorkBuff(PifFileHandle hsStorageFile, ULONG ulReadPosition, UCHAR  *puchWorkBuff, USHORT usMaxBuffSize, USHORT *pusItemSize);
	TrnStorageType	TrnDetermineStorageType (VOID);
	SHORT   TrnStoGetStorageInfo( TrnStorageType  sStorageType, PifFileHandle  *phsStorage, USHORT *pusHdrSize, TrnVliOffset *pusVliSize );
    SHORT   TrnStoGetStorageIndexInfo( TrnStorageType  sStorageType, PifFileHandle  *phsStorage, PifFileHandle  *phsItemIndex, PifFileHandle  *phsNoItemIndex, USHORT *pusHdrSize, TrnVliOffset *pusVliSize );
    VOID    TrnStoUpdateCouponType( ITEMCOUPON *pItem );
    SHORT   TrnStoIdxUpdate( ITEMSALES  *pItemSales, TRANSTOSALESSRCH *pNormalItem, TRANSTOSALESSRCH   *pVoidItem, TrnStorageType sStorageType );
    SHORT   TrnStoNoIdxUpdate( ITEMSALES *pItemSales, TRANSTOSALESSRCH   *pNormalItem, TRANSTOSALESSRCH   *pVoidItem, TrnStorageType sStorageType, SHORT sType );

//------------------------------------------------------------------------
// Testing with GenPOS Rel 2.2.1.145 with Table Service, Post Guest Check, to
// review the functionality we discovered that using Guest Check Transfer to
// combine a guest check with a second guest check was not working properly.
// We found an error with the memory caching functionality due to a defect
// in handling read of only part of a guest check.  Disabling this functionality
// addressed the problem.
// This memory caching functionality should probably just be removed as it
// has a defect and does not seem to provide any advantage.
//   Richard Chambers, Feb-06-2015

//#define TrnReadWriteFile_Memory

#if defined(TrnReadWriteFile_Memory)
    SHORT   TrnExpandFile( SHORT hsFileHandle, ULONG ulInquirySize );
    SHORT   TrnSeekFile(SHORT hsFileHandle, ULONG ulActSize, ULONG *ulActMove);
    VOID    TrnDeleteFile(TCHAR *uchFileName);
    USHORT  TrnConvertError(SHORT sError);

    SHORT   TrnOpenFile_Memory(TCHAR *uchFileName, UCHAR *auchType);
    VOID    TrnCloseFile_Memory(SHORT hsFileHandle);

#define TrnOpenFile(uchFileName, auchType) TrnOpenFile_Memory(uchFileName, auchType)
#define TrnCloseFile(hsFileHandle) TrnCloseFile_Memory(hsFileHandle)

#else

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// various Trn functions are called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to these functions
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message(" \z")
#pragma message("  ====++++====   ----------------------------------    ====++++====")
#pragma message("  ====++++====   TrnOpenFile_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   TrnExpandFile_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   TrnSeekFile_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   TrnCloseFile_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   TrnDeleteFile_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   File: Include\\trans.h              ====++++====")
    SHORT   TrnOpenFile_Debug(CONST TCHAR *uchFileName, CONST UCHAR *auchType, CONST char *aszFilePath, int nLineNo);
    SHORT   TrnExpandFile_Debug( SHORT hsFileHandle, ULONG ulInquirySize, CONST char *aszFilePath, int nLineNo);
    SHORT   TrnSeekFile_Debug(SHORT hsFileHandle, ULONG ulActSize, ULONG *ulActMove, CONST char *aszFilePath, int nLineNo);
    VOID    TrnCloseFile_Debug(SHORT hsFileHandle, CONST char *aszFilePath, int nLineNo);
    VOID    TrnDeleteFile_Debug(CONST TCHAR *uchFileName, CONST char *aszFilePath, int nLineNo);
    USHORT  TrnConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo);

#define TrnOpenFile(uchFileName,auchType)    TrnOpenFile_Debug(uchFileName,auchType,__FILE__,__LINE__)
#define TrnExpandFile(hsFileHandle,ulInquirySize)   TrnExpandFile_Debug(hsFileHandle,ulInquirySize,__FILE__,__LINE__)
#define TrnSeekFile(hsFileHandle,ulActSize,ulActMove)  TrnSeekFile_Debug(hsFileHandle,ulActSize,ulActMove,__FILE__,__LINE__)
#define TrnCloseFile(hsFileHandle)  TrnCloseFile_Debug(hsFileHandle,__FILE__,__LINE__)
#define TrnDeleteFile(uchFileName) TrnDeleteFile_Debug(uchFileName,__FILE__,__LINE__)
#define TrnConvertError(sError) TTrnConvertError_Debug(sError,__FILE__,__LINE__)
#else
    SHORT   TrnOpenFile(CONST TCHAR *uchFileName, CONST UCHAR *auchType);
    SHORT   TrnExpandFile( PifFileHandle hsFileHandle, ULONG ulInquirySize );
    SHORT   TrnSeekFile(PifFileHandle hsFileHandle, ULONG ulActSize, ULONG *ulActMove);
    VOID    TrnCloseFile(SHORT hsFileHandle);
    VOID    TrnDeleteFile(CONST TCHAR *uchFileName);
    USHORT  TrnConvertError(SHORT sError);
#endif
#endif
    SHORT   TrnChkStorageSize(SHORT sSize);
    SHORT   TrnChkStorageSizeIfSales(SHORT sSize);
	SHORT   TrnStoPreviousCondimentSales( ITEMSALES *pItemSales, USHORT usType ); //SR 139


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION COUPON MODULE)
--------------------------------------------------------------------------*/

    SHORT   TrnStoVoidCouponItem( ITEMSALES *pItemSales,
		                          UCHAR  *puchSalesItem,
                                  USHORT usSalesOffset,
                                  SHORT  sStorageType );
    BOOL    TrnStoIsSalesLinked( UCHAR *puchCoupon, USHORT usSalesOffset );
    VOID    TrnStoMakeVoidCpn( UCHAR *puchSource, ITEMCOUPON *pVoidCpn );

    SHORT   TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemCpnSearch, SHORT  sStoratgeType );
	SHORT	TrnStoCouponSearchAll( ITEMCOUPONSEARCH *pItemCpnSearch, SHORT  sStorageType );

    SHORT   TrnStoCpnItemSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                 UCHAR            *puchWorkBuffer,
                                 USHORT           usTargetItemOffset,
                                 DCURRENCY        lDiscountAmount);
    SHORT   TrnStoCpnPLUSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                ITEMSALES        *pItemSales,
                                USHORT           usTargetOffset );
    SHORT   TrnStoCpnDeptSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                                 ITEMSALES        *pItemSales,
                                 USHORT           usTargetOffset );
    SHORT   TrnStoIsCouponItem( ITEMSALES *pItemSales, UCHAR uchRecType );
    SHORT   TrnStoIsAllItemFound( ITEMCOUPONSEARCH *pItemCpnSearch );
    SHORT   TrnStoUpdateCpnCo( ITEMCOUPON *pItemCoupon, SHORT sStorageType, TRANSTORAGESALESNON *pNon ); /* saratoga */
    SHORT   TrnStoUpdateVoidCo( ITEMCOUPON *pItemCoupon,
                                ITEMCOUPON *pRetCoupon,
                                SHORT       sStorageType );
    SHORT   TrnStoCpnVoidSearch( ITEMCOUPON *pItemCoupon,
                                 ULONG     *puLOffset,
                                 SHORT      sStorageType );
    USHORT  TrnStoQueryCpnType( ITEMCOUPONSEARCH *pItem );
    SHORT   TrnStoCpnUPCSearch( ITEMCOUPONSEARCH  *pItemCpnSearch,
                          ITEMSALES         *pItemSales,
                          USHORT            usTargetOffset );
    SHORT TrnStoIsUPCCouponItem(ITEMSALES *pPLU, TCHAR *pauchCouponNo);
    LONG TrnASCII2BIN(TCHAR *puchData, TCHAR iuchStart, TCHAR iuchEnd);

	SHORT TrnStoOrderDecSearch( ITEMSALES *pItemOrderDecSearch, SHORT  sStorageType );


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT MODULE BODY)          R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnSalesSpl(ITEMSALES *ItemSales, SHORT sTranType);
    SHORT   TrnDiscountSpl(ITEMDISC *ItemDisc, SHORT sTranType);
    SHORT   TrnCouponSpl(ITEMCOUPON *pItemCoupon, SHORT sTranType);
    SHORT   TrnTotalSpl(ITEMTOTAL *ItemTotal, SHORT sTranType);
    SHORT   TrnTenderSpl(ITEMTENDER *ItemTender, SHORT sTranType);
    SHORT   TrnMiscSpl(ITEMMISC *ItemMisc, SHORT sTranType);
    SHORT   TrnAffectionSpl(ITEMAFFECTION *ItemAffection, SHORT sTranType);
    SHORT   TrnPrintSpl(ITEMPRINT *ItemPrint, SHORT sTranType);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT QUALIFIER MODULE)     R3.1
--------------------------------------------------------------------------*/

    VOID    TrnQualTransSpl(ITEMTRANSOPEN *pItem, TrnStorageType  sStorageType);
    VOID    TrnQualDiscSpl(ITEMDISC *pItem, TrnStorageType  sStorageType);

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT ITEMIZERS MODULE)     R3.1
--------------------------------------------------------------------------*/

    VOID    TrnItemSalesSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnSalesDeptSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnSalesPLUSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnTaxableSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnDiscTaxSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnItemCouponSpl(ITEMCOUPON *pItemCoupon, TrnStorageType sTranType);
    VOID    TrnSalesModSpl(ITEMSALES *ItemSales, TrnStorageType sTranType);
    VOID    TrnItemDiscSpl(ITEMDISC *ItemDisc, TrnStorageType sTranType);
    VOID    TrnItemDiscountSpl(ITEMDISC *ItemDisc, TrnStorageType sTranType);
    VOID    TrnRegDiscountSpl(ITEMDISC *ItemDis, TrnStorageType sTranType);
    VOID    TrnChargeTipSpl(ITEMDISC *ItemDisc, TrnStorageType sTranType);
    VOID    TrnItemAffectionSpl(ITEMAFFECTION *pItem, TrnStorageType sTranType);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION STORAGE MODULE)             R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnStoOpenSpl(ITEMTRANSOPEN *pItem, TrnStorageType sTranType);
    SHORT   TrnStoSalesSpl(ITEMSALES *pItem, TrnStorageType sTranType);
    SHORT   TrnStoDiscSpl(ITEMDISC *pItem, TrnStorageType sTranType);
    SHORT   TrnStoCouponSpl(ITEMCOUPON *pItem, TrnStorageType sTranType);
    SHORT   TrnStoTotalSpl(ITEMTOTAL *pItem, TrnStorageType sTranType);
    SHORT   TrnStoTenderSpl(ITEMTENDER *pItem, TrnStorageType sTranType);
    SHORT   TrnStoMiscSpl(ITEMMISC *pItem, TrnStorageType sTranType);
    SHORT   TrnStoAffectSpl(ITEMAFFECTION *pItem, TrnStorageType sTranType);
    SHORT   TrnStoPrintSpl(ITEMPRINT *pItem, TrnStorageType sTranType);
    USHORT  TrnStoConsNormalSpl(VOID *pData, SHORT sSize, TrnStorageType sTranType);
    SHORT   TrnStoAppendIndexSpl(TRANSTORAGESALESNON *pTranNon, USHORT usItemOffset, TrnStorageType sTranType);
    SHORT   TrnStoInsertIndexSpl(TRANSTORAGESALESNON *pTranNon, USHORT usItemOffset, TrnStorageType sTranType);


/*--------------------------------------------------------------------------
*        prototype (TRANSACTION SPLIT CHECK MODULE)         R3.1
--------------------------------------------------------------------------*/

    SHORT   TrnSplRightArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight);
    SHORT   TrnSplLeftArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight);
    VOID    TrnSplDispSeatNo1(UCHAR uchSeat, UCHAR *auchUseQueue);
    SHORT   TrnVoidSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnVoidSearchSalesMod(ITEMSALES *pData, SHORT sStorage);
    SHORT   TrnVoidSearchSalesZip(ITEMSALES *pSales1, ITEMSALES *pSales2);
	SHORT   TrnVoidSearchNonSalesItem(ITEMGENERICHEADER *pData, SHORT sStorage);
    SHORT   TrnVoidSearchDisc(ITEMDISC *pData, SHORT sStorage);
	SHORT   TrnVoidSearchRegDisc(ITEMDISC *pData, SHORT sStorage);
    SHORT   TrnVoidSearchCoupon(ITEMCOUPON *pData, SHORT sStorage);
    VOID    TrnVoidSearchRedSalesModSub(SHORT sStorage, ITEMSALES *pItem);
    VOID    TrnSplInsertUseSeatQueue(UCHAR uchSeat);
    VOID    TrnSplInsertFinSeatQueue(UCHAR uchSeat);
    VOID    TrnSplRemoveFinSeatQueue(UCHAR uchSeat);
    SHORT   TrnSplDecideStorage(TrnStorageType sStorageType, ULONG *ulReadPos, ULONG *ulCurSize, PifFileHandle *husHandle);

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
    SHORT   TrnSplReduceStorageSales(ITEMSALES *pItem);
    SHORT   TrnSplRedStoSales(ITEMSALES *pItem);
    VOID    TrnSplRedStoSalesMod(ITEMSALES *pItem);
    VOID    TrnSplReduceStorageDisc(ITEMDISC *pItem);
    VOID    TrnSplReduceStorageCoupon(ITEMCOUPON *pItem);
    SHORT   TrnStoSalesUpdateRed(ITEMSALES *pItemSales, TRANSTOSALESSRCH   *pCompItem,
                TRANSTOSALESSRCH *pNormalItem, TRANSTOSALESSRCH *pVoidItem, SHORT sStorageType);
    VOID    TrnStoSalesUpdateRedCpn(SHORT sVoidCpnCo, USHORT usItemOff);

/*--------------------------------------------------------------------------
*        prototype (TRANSACTION COMMON MODULE)                  V3.3
--------------------------------------------------------------------------*/
    VOID    TrnSalesDeptUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesDeptCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pItemizer,
                    TRANGCFQUAL *pGCF, LONG lSign);
    VOID    TrnSalesDeptIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnTaxableIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnDiscTaxUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnDiscTaxCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnSalesModIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountUSComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountCanComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemDiscountIntlComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnRegDiscTaxRateComn( ITEMDISC *pItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscTaxAmtComn( ITEMDISC *pItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscTaxAmtVoidComn(CONST ITEMDISC *pItemDisc, SHORT sType, LONG lSign );	/* TAR191625 */
    VOID    TrnRegDiscTaxCanComn( ITEMDISC *ItemDisc, SHORT sType, LONG lSign );
    VOID    TrnRegDiscVATCalcComn(ITEMDISC *pDisc, SHORT sType, LONG lSign);
    VOID    TrnItemCouponUSComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemCouponCanComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);
    VOID    TrnItemCouponIntlComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign);

    VOID    TrnRegDiscTaxCanComnWork( TRANITEMIZERS *pTrn, SHORT sType, USHORT *pfsLimitable, USHORT *pfsBaked );

VOID TrnStoItemSalesPLUMnemo( ITEMSALES *pItemSales );
USHORT TrnItemGetPLUMnemo(TCHAR  *pszPLUMnem, TCHAR  *pszPLUMnemAlt, TCHAR  *auchPLUNo);
VOID TrnItemStoPLUMnemo(TCHAR  *pszPLUMnem, TCHAR  *pszPLUMnemAlt, TCHAR  *auchPLUNo);

/*--------------------------------------------------------------------------
*        external static data
--------------------------------------------------------------------------*/

	// See the new functions for accessing the various parts of the memory resident database TrnInformation.
	// At some point we would like to eliminate any direct use of TrnInformation.
	//
	// These global pointers are designed to provide a read only access to the data:
	//    TrnInformationPtr, TranModeQualPtr, TranGCFQualPtr, TranCurQualPtr
    extern  TRANINFORMATION     TrnInformation;         /* transaction information */

    extern  TRANINFSPLIT    TrnSplitA;
    extern  TCHAR FARCONST  aszTrnConsSplitFileA[];
    extern  TCHAR FARCONST  aszTrnConsSplitIndexA[];
    extern  TRANINFSPLIT    TrnSplitB;
    extern  TCHAR FARCONST  aszTrnConsSplitFileB[];
    extern  TCHAR FARCONST  aszTrnConsSplitIndexB[];


/****** End of Definition ******/