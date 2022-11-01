#if defined(POSSIBLE_DEAD_CODE)
/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Header File                         
* Category    : TOTAL, NCR 2170 Hospitality Application         
* Program Name: CSTTL.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:           :                                    
* Mar-14-95:03.00.00:H.Ishida   : R3.0
* Sep-02-98:03.03.00:A.Mitsui   : V3.3 Initial
* Jan-11-00:        :K.Yanagida : Saratoga
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
                            
/* Major Data Class */


/* Mainor Data Class */
#define     CLASS_TTLCURDAY     1       /* Current Daily File */
#define     CLASS_TTLSAVDAY     2       /* Previous Daily File */
#define     CLASS_TTLCURPTD     3       /* Current PTD File */
#define     CLASS_TTLSAVPTD     4       /* Previous PTD File */
#define		CLASS_SAVED_TOTAL	5

/* Maxmum Number */
/*#define     TTL_MAX_DEPT        50    */  /* Max Dept Number Size */
/*#define     TTL_MAX_PLU         3000  */  /* Max PLU Number Size */
#define     TTL_MAX_DEPT        250     /* Max Dept Number Size, 2172 */
#define     TTL_MAX_PLU       10000     /* Max PLU Number Size, Saratoga */
#define     TTL_MAX_CASHIER     300     /* Max Cashier Number Size V3.3 32->99->300 */
#define     TTL_MAX_WAITER      100     /* Max Waiter Number Size */       
#define     TTL_MAX_CPN         300     /* Max Coupon Number Size, R3.0 */
#define     TTL_MAX_SERTIME     3       /* Max Service Time Block, R3.1 */
#define     TTL_MAX_INDFIN     16       /* Max Indivi-Financial,   R3.1 */

#define     TTL_WITHOUT_PTD     0       /* create total file except PTD total */
#define     TTL_WITH_PTD        1       /* create total file with PTD total */

/* Error Code for user interface  */
#define     TTL_SUCCESS         0       /* Perform succesfully */                
#define     TTL_ISSUED          0       /* reset report enable */
#define     TTL_UNLOCKED        0       /* file status (Unlocked) */

#define     TTL_FAIL           -1
#define     TTL_BUFFER_FULL    -2       /* Update Total File Full */
#define     TTL_NOTINFILE      -3       /* specified data not in file */
#define     TTL_NOT_ISSUED     -4       /* reset report disable */

#define     TTL_LOCKED         -5       /* file status (Locked) */
#define     TTL_CHECK_RAM      -6       /* Check RAM Size */
#define     TTL_NOTCLASS       -11      /* Illigal Class # */
#define     TTL_NOTPTD         -12      /* Not Exist PTD */

#define     TTL_DELCASWAI      -15      /* Delete Waiter Record */
#define     TTL_DELINDFIN      -16      /* Delete Individual Fin. Record */

#define     TTL_NOT_MASTER     -20      /* Prohibi operation */


/* Error Code for Internal  */
#define     TTL_SIZEOVER       -31      /* Max Size Over for Dept, 
                                           PLU, Cashier, Waiter */
#define     TTL_NOTZERO         -32     /* Total Not Zero */
#define     TTL_NOTEMPTY        -33     /* Empty Table Full */
#define     TTL_FILE_OPEN_ERR   -50     /* File Open Error */
#define     TTL_FILE_SEEK_ERR   -51     /* File Seek Error */
#define     TTL_FILE_READ_ERR   -52     /* File Read Error */
#define     TTL_FILE_HANDLE_ERR -53     /* File Handle Error */
#define     TTL_AFFECTION_ERR   -54     /* Affection Error */
#define     TTL_BAD_LENGTH      -55     /* BAD Length of TUM Read */
#define		TTL_UPD_DB_BUSY		-56

/* Stack Size */
/*  New Total Update File Error Conditions*/
#define		TTL_UPD_WAI_ERROR			-68
#define		TTL_UPD_PLU_ERROR			-69
#define		TTL_UPD_DEP_ERROR			-70
#define		TTL_UPD_CAS_ERROR			-71
#define		TTL_UPD_CPN_ERROR			-72

/*#define     TTL_STACK   (1500 + 150)*//* ### MOD 2172 Rel1.0 (01/07/00) */
#define     TTL_STACK           ((1500+150)*2)  /* Total Update Module Stack Size */
#define     TTL_THREDPRI        1           /* Total TUM Priority */

#define     TTL_NOTRESET        0x01        /* Disable Total State */

#define     TTL_COMP_CREATE     0x00        /* Compulsory Creation File */
#define     TTL_CHECK_CREATE    0x01        /* Check Creation File */
#define     TTL_COMP_DEL        0x00        /* Compulsory Delete File */
#define     TTL_CHECK_DEL       0x01        /* Check Delete File */                                                           
/*
*===========================================================================
*   Type Declarations 
*===========================================================================
*/
/* -- total amount & counter -- */
typedef struct {
    LONG    lAmount;            /* Total Amount */
    SHORT   sCounter;           /* Total Counter */
}TOTAL;

/* -- total amount & counter -- */
typedef struct {
    TOTAL   OnlineTotal;                            /* Online To/Co         */
    TOTAL   OfflineTotal;                           /* Offline To/Co        */
} TEND;                                             /* add EPT '94 Apr.08   */

/* -- total amount & counter -- */
typedef struct {
    LONG    lAmount;            /* Total Amount */
    LONG    lCounter;           /* Total Counter */
}LTOTAL;

/* --- on hand total amount & counter,  Saratoga --- */
typedef struct {
    TOTAL   Total;              /* Total amount & counter */
    LONG    lHandTotal;         /* On Hand Total */
}TENDHT;

/* -- start date & end date -- */
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
    USHORT  usMDay;             /* Day */
    USHORT  usMonth;            /* Month */
}DATE;

/* -- start time & end time -- */
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
}TIME;

/* -- Tax Total -- */
typedef struct {
    D13DIGITS   mlTaxableAmount;    /* Taxable Amount */
    LONG        lTaxAmount;         /* Tax Amount */
    LONG        lTaxExempt;         /* Tax Exempt */
}TAXABLE;

/* 
*-----------------------------------------------
*  REPORT / TOTAL INTERFACE
*-----------------------------------------------
*/

/* -- Regster Financial File Total -- */
typedef struct {
	TOTAL      TtlIAmount[4];       // Item discounts for 3 through 6
} TTLITEMDISC;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usTerminalNumber;   /* terminal unique number. R3.1 */
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    D13DIGITS   CGGTotal;           /* Current Gross Group Total */
    LONG        lNetTotal;          /* Net Total, Release 3.1 */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    LONG        lTGGTotal;          /* Training Gross Group Total */
    TOTAL       DeclaredTips;       /* Declared Tips */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       ConsCoupon;         /* Consolidated Coupon */
    TOTAL       ItemDisc;           /* Item Discount */
    TOTAL       ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL       RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL       Coupon;             /* Combination Coupon, R3.0 */
    TOTAL       PaidOut;            /* Paid Out */
    TOTAL       RecvAcount;         /* Received on Acount */
    TOTAL       TipsPaid;           /* Tips Paid Out */
    TEND        CashTender;         /* Cash Tender          change EPT*/
    TEND        CheckTender;        /* Check Tender         change EPT*/
    TEND        ChargeTender;       /* Charge Tender        change EPT*/
    TEND        MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
/*    TOTAL       ForeignTotal[2];  */  /* Foreign Total 1 to 2 */
    TOTAL       ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[6];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    ###ADD Saratoga */
    TOTAL       Pickup;             /* Pickup,  ###ADD Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL       HashDepartment;     /* Hash Department */
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 */
    LONG        lConsTax;           /* Consolidation tax */
}TTLREGFIN;

/* -- Hourly File Total -- */
typedef struct {
    LONG        lHourlyTotal;       /* Hourly Total */
    LONG        lItemproductivityCount; /* IP Counter */
    SHORT       sDayNoOfPerson;     /* Number of Person Counter */
    LONG        lBonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8, V3.3 */
}HOURLY;

/* -- Hourly Activity File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    TIME        StartTime;          /* Start Time */
    TIME        EndTime;            /* End Time */
    HOURLY      Total[48];          /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY;

/* -- Service Time Total R3.1 --*/
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Period From */
    DATE        ToDate;             /* Period To */
    TOTAL       ServiceTime[48][3]; /* Block Data of Service Time 1 to 48, V3.3 */
} TTLSERTIME;

/* -- Department File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
/*    UCHAR       uchDEPTNumber;    */  /* Department Number */
    USHORT      usDEPTNumber;      /* Department Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    UCHAR       uchMajorDEPTNo;     /* Major DEPT Number */
    LTOTAL      DEPTTotal;          /* Department Total */
    LTOTAL      DEPTAllTotal;       /* Department All Total */
}TTLDEPT;

/* -- PLU File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
/*    USHORT      usPLUNumber;*/        /* PLU Number */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUTotal;           /* PLU Total */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
}TTLPLU;

/* -- Coupon File Total, R3.0 -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usCpnNumber;        /* Coupon Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    TOTAL       CpnTotal;           /* Department Total */
}TTLCPN;

/* -- Casher File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usCashierNumber;    /* Cashier Number */
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       ConsCoupon;         /* Consolidated Coupon */
    TOTAL       ItemDisc;           /* Item Discount */
    TOTAL       ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL       RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL       Coupon;             /* Combination Coupon, R3.0 */
    TOTAL       PaidOut;            /* Paid Out */
    TOTAL       RecvAcount;         /* Received on Acount */
    TOTAL       TipsPaid;           /* Tips Paid Out */
/*    TOTAL       CashTender;      */   /* Cash Tender */
/*    TOTAL       CheckTender;     */   /* Check Tender */
/*    TOTAL       ChargeTender;    */   /* Charge Tender */
/*    TOTAL       MiscTender[8];   */   /* Misc Tender 1 to 8 */
/*    TOTAL       ForeignTotal[2]; */   /* Foreign Total 1 to 2 */
    TENDHT      CashTender;         /* Cash Tender, Saratoga */
    TENDHT      CheckTender;        /* Check Tender */
    TENDHT      ChargeTender;       /* Charge Tender */
    TENDHT      MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT      ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[6];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL       HashDepartment;     /* Hash Department */
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 */
    LONG        lConsTax;           /* Consolidation tax */
    USHORT      usPostRecCo;        /* post receipt counter, R3,1 */
    LONG        lVATServiceTotal;   /* Service Total of VAT, V3.3*/
    TOTAL       CheckTransTo;       /* Checks Transferred to, V3.3 */
    TOTAL       CheckTransFrom;     /* Checks Transferred from, V3.3 */
    TOTAL       DeclaredTips;       /* Declared Tips, V3.3 */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga *//* ### ADD Saratoga (052900) */
	//addition for release 2.1 JHHJ
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, 2.1 Release*/
	D13DIGITS   NonTaxable;         /* Non taxable Total 2.1 Release*/
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
}TTLCASHIER;

/* -- Report Structure for Read/Write -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usNumber;           /* Number (Dept,PLU,Cashier,Waiter) */
    UCHAR       uchID;              /* ID (PLU - Adjective) */
}TTLREPORT;

/*
*-----------------------------------------------
* Total Function Library / Total Update Module 
*-----------------------------------------------
*/

/* -- Regster Financial File Total -- */
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    D13DIGITS   mlCGGTotal;         /* Current Gross Group Total */
    LONG        lNetTotal;          /* Net Total,   Release 3.1 */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    LONG        lTGGTotal;          /* Training Gross Group Total */
    TOTAL       DeclaredTips;       /* Declared Tips */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       ConsCoupon;         /* Consolidated Coupon */
    TOTAL       ItemDisc;           /* Item Discount */
    TOTAL       ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL       RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL       Coupon;              /* Combination Coupon, R3.0 */
    TOTAL       PaidOut;            /* Paid Out */
    TOTAL       RecvAcount;         /* Received on Acount */
    TOTAL       TipsPaid;           /* Tips Paid Out */
    TEND        CashTender;         /* Cash Tender          change EPT*/
    TEND        CheckTender;        /* Check Tender         change EPT*/
    TEND        ChargeTender;       /* Charge Tender        change EPT*/
    TEND        MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
/*    TOTAL       ForeignTotal[2];  */  /* Foreign Total 1 to 2 */
    TOTAL       ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[6];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL       HashDepartment;     /* Hash Department */
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 */
    LONG        lConsTax;           /* Consolidation tax */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
}TTLCSREGFIN;

/* -- Hourly Activity File Total -- */
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    HOURLY      Total[48];          /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLCSHOURLY;

/* -- Service Time Total R3.1 -- */
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Period From */
    DATE        ToDate;             /* Period To */
    TOTAL       ServiceTime[48][3]; /* Block Data of Service Time 1 to 48, V3.3 */
} TTLCSSERTIME;

/* -- Individual Financial Total R3.1 -- */
typedef struct {
    USHORT      usIndFinFileSize;   /* File Size */
    USHORT      usMaxTerminal;      /* Max Terminal Number */
    USHORT      usCurDayTerminal;   /* Current Daily Terminal Number */
    USHORT      usPreDayTerminal;   /* Previous Daily Terminal Number */
    USHORT      usCurDayIndexOff;   /* Current Daily Index Offset */
    USHORT      usCurDayEmptOff;    /* Current Daily Empty Offset */
    USHORT      usCurDayTotalOff;   /* Current Daily Total Offset */
    USHORT      usPreDayIndexOff;   /* Previous Daily Index Offset */
    USHORT      usPreDayEmptOff;    /* Previous Daily Empty Offset */
    USHORT      usPreDayTotalOff;   /* Previous Daily Total Offset */
} TTLCSINDHEAD;

typedef struct {
    USHORT      usTerminalNo;       /* Terminal Number */
    UCHAR       uchBlockNo;         /* Total Block Number */
} TTLCSINDINDX;

/* -- Department File Total -- */
typedef struct {
    USHORT      usDeptFileSize;     /* File Size */
    USHORT      usMaxDept;          /* Max Dept Number */
    USHORT      usCurDayTtlOff;     /* Current Daily Total Offset */
    USHORT      usPreDayTtlOff;     /* Previous Daily Total Offset */
    USHORT      usCurPTDTtlOff;     /* Current PTD Total Offset */
    USHORT      usPrePTDTtlOff;     /* Previous PTD Total Offset */
}TTLCSDEPTHEAD;


typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LTOTAL      DEPTAllTotal;       /* Department All Total */
}TTLCSDEPTMISC;

/* -- PLU File Total -- */
typedef struct {
    ULONG       ulPLUFileSize;      /* File Size */
    USHORT      usMaxPLU;           /* Max PLU Number */
    USHORT      usCurDayPLU;        /* Current Daily PLU Number */
    USHORT      usPreDayPLU;        /* Previous Daily PLU Number */
    USHORT      usCurPTDPLU;        /* Current PTD PLU Number */
    USHORT      usPrePTDPLU;        /* Previous PTD PLU Number */
    ULONG       ulCurDayMiscOff;    /* Current Daily Misc Offset */
    ULONG       ulCurDayIndexOff;   /* Current Daily Index Offset */
    ULONG       ulCurDayBlockOff;   /* Current Daily Block Offset */
    ULONG       ulCurDayEmptOff;    /* Current Daily Empty Offset */
    ULONG       ulCurDayTotalOff;   /* Current Daily Total Offset */
    ULONG       ulPreDayMiscOff;    /* Previous Daily Misc Offset */
    ULONG       ulPreDayIndexOff;   /* Previous Daily Index Offset */
    ULONG       ulPreDayBlockOff;   /* Previous Daily Block Offset */
    ULONG       ulPreDayEmptOff;    /* Previous Daily Empty Offset */
    ULONG       ulPreDayTotalOff;   /* Previous Daily Total Offset */
    ULONG       ulCurPTDMiscOff;    /* Current PTD Misc Offset */
    ULONG       ulCurPTDIndexOff;   /* Current PTD Index Offset */
    ULONG       ulCurPTDBlockOff;   /* Current PTD Block Offset */
    ULONG       ulCurPTDEmptOff;    /* Current PTD Empty Offset */
    ULONG       ulCurPTDTotalOff;   /* Current PTD Total Offset */
    ULONG       ulPrePTDMiscOff;    /* Previous PTD Misc Offset */
    ULONG       ulPrePTDIndexOff;   /* Previous PTD Index Offset */
    ULONG       ulPrePTDBlockOff;   /* Previous PTD Block Offset */
    ULONG       ulPrePTDEmptOff;    /* Previous PTD Empty Offset */
    ULONG       ulPrePTDTotalOff;   /* Previous PTD Total Offset */
}TTLCSPLUHEAD;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
}TTLCSPLUMISC;

typedef struct {
/*    USHORT      usPLUNo; */           /* PLU Number */
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN+1];/* PLU Number,  Saratoga */
    UCHAR       uchAject;           /* Adjective Number */
}TTLCSPLUINDX;

/* -- Coupon File Total, R3.0 -- */
typedef struct {
    USHORT      usCpnFileSize;      /* File Size */
    USHORT      usMaxCpn;           /* Max coupon Number */
    USHORT      usCurDayTtlOff;     /* Current Daily Total Offset */
    USHORT      usPreDayTtlOff;     /* Previous Daily Total Offset */
    USHORT      usCurPTDTtlOff;     /* Current PTD Total Offset */
    USHORT      usPrePTDTtlOff;     /* Previous PTD Total Offset */
}TTLCSCPNHEAD;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
}TTLCSCPNMISC;

/* -- Casher File Total -- */
typedef struct {
    ULONG       ulCasFileSize;      /* Cashier File Size */
    USHORT      usMaxCas;           /* Cashier Max Number */
    USHORT      usCurDayCas;        /* Current Daily Cashier Number */
    USHORT      usPreDayCas;        /* Previous Daily Cashier Number */
    USHORT      usCurPTDCas;        /* Current PTD Cashier Number, V3.3 */
    USHORT      usPrePTDCas;        /* Previous PTD Cashier Number, V3.3 */
    ULONG       ulCurDayIndexOff;   /* Current Cashier Daily Index Offset */
    ULONG       ulCurDayEmptOff;    /* Current Cashier Daily Enmpty Offset */
    ULONG       ulCurDayTotalOff;   /* Current Cashier Daily Total Offset */
    ULONG       ulPreDayIndexOff;   /* Previous Cashier Daily Index Offset */
    ULONG       ulPreDayEmptOff;    /* Previous Cashier Daily Enmpty Offset */
    ULONG       ulPreDayTotalOff;   /* Previous Cashier Daily Total Offset */
    ULONG       ulCurPTDIndexOff;   /* Current Cashier PTD Index Offset, V3.3 */
    ULONG       ulCurPTDEmptOff;    /* Current Cashier PTD Enmpty Offset, V3.3 */
    ULONG       ulCurPTDTotalOff;   /* Current Cashier PTD Total Offset, V3.3 */
    ULONG       ulPrePTDIndexOff;   /* Previous Cashier PTD Index Offset, V3.3 */
    ULONG       ulPrePTDEmptOff;    /* Previous Cashier PTD Enmpty Offset, V3.3 */
    ULONG       ulPrePTDTotalOff;   /* Previous Cashier PTD Total Offset, V3.3 */
}TTLCSCASHEAD;

typedef struct {
    USHORT      usCasNo;            /* Cashier Number */
    USHORT      usBlockNo;          /* Total Block Number, V3.3 */
}TTLCSCASINDX;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN */
    TOTAL       ConsCoupon;         /* Consolidated Coupon */
    TOTAL       ItemDisc;           /* Item Discount */
    TOTAL       ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL       RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL       Coupon;             /* Combination Coupon, R3.0 */
    TOTAL       PaidOut;            /* Paid Out */
    TOTAL       RecvAcount;         /* Received on Acount */
    TOTAL       TipsPaid;           /* Tips Paid Out */
/*    TOTAL       CashTender;     */    /* Cash Tender */
/*    TOTAL       CheckTender;    */    /* Check Tender */
/*    TOTAL       ChargeTender;   */    /* Charge Tender */
/*    TOTAL       MiscTender[8];  */    /* Misc Tender 1 to 8 */
/*    TOTAL       ForeignTotal[2];*/    /* Foreign Total 1 to 2 */
    TENDHT      CashTender;         /* Cash Tender */
    TENDHT      CheckTender;        /* Check Tender */
    TENDHT      ChargeTender;       /* Charge Tender */
    TENDHT      MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT      ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[6];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL       HashDepartment;     /* Hash Department */
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 */
    LONG        lConsTax;           /* Consolidation tax */
    USHORT      usPostRecCo;        /* post receipt counter, R3,1 */
    LONG        lVATServiceTotal;   /* Service Total of VAT, V3.3*/
    TOTAL       CheckTransTo;       /* Checks Transferred to, V3.3 */
    TOTAL       CheckTransFrom;     /* Checks Transferred from, V3.3 */
    TOTAL       DeclaredTips;       /* Declared Tips, V3.3 */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
		//addition for release 2.1 JHHJ
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
}TTLCSCASTOTAL;

typedef struct {
    USHORT      usReadPointer;      /* Read Pointer */
    USHORT      usWritePointer;     /* Write Pointer */
    USHORT      usCurReadPointer;   /* Current Read Pointer */
    UCHAR       uchTtlFlag0;        /* Total Flag Area */
    UCHAR       uchResetFlag;       /* Master reset Flag */
    USHORT      usSvCurReadPointer;    /* Saved Current Read Pointer */
}TTLCSUPHEAD;

typedef struct { 
	DATE  dtCreationDate;							//Date of creation, so that we know when to destroy file
	USHORT usYear;
	ULONG ulFileSize;									//size of the File
	ULONG ulIndTerminalOffset[STD_NUM_OF_TERMINALS];	//offset to each individual terminal totals
	ULONG ulFinancialTotalOffset;						//offset to total financial totals
	ULONG ulDeptTotalOffset;							//offset to dept totals
	ULONG ulHourlyTotalOffset;							//offset to hourly totals
	ULONG ulServiceTotalOffset;							//offset to service totals
	ULONG ulCashierTotalOffset[STD_NUM_OF_CASHIERS];	//offset to cashier totals
	ULONG ulCouponTotalOffset;							//offset to coupon totals
	ULONG ulPluTotalBeginOffset;						//CLASS_TTLPLU, TTLPLU - offset to PLU totals begins
	ULONG ulPLUTotalCounter;
}SAVTTLFILEINDEX;


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   TtlInitial(USHORT usPowerUpMode);            /* initialize Total function */
VOID   TtlBaseCreate(VOID);                         /* create base total file */
SHORT  TtlDeptCreate(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod);  
                                                    /* create dept, total file */
SHORT  TtlDeptDeleteFile(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete dept, total file */
/*SHORT  TtlPLUCreate(USHORT usPLUNum, BOOL fPTD, UCHAR uchMethod);    */
SHORT  TtlPLUCreate(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod);    	/* ### MOD 2172 Rel1.0 */
                                                    /* create plu total file */
/*SHORT  TtlPLUDeleteFile(USHORT usPLUNum, BOOL fPTD, UCHAR uchMethod);*/
SHORT  TtlPLUDeleteFile(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod);	/* ### MOD 2172 Rel1.0 */
                                                    /* Delete dept, total file */
/* -- R3.0  -- */
/* SHORT  TtlCpnCreate(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);               */
                                                      /* create coupon, total file */
/* SHORT  TtlCpnDeleteFile(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);           */
                                                      /* Delete coupon, total file */
SHORT  TtlCasCreate(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod); 
                                                    /* create cashier total file */
SHORT  TtlCasDeleteFile(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete dept, total file */
SHORT  TtlWaiCreate(USHORT usWaiterNum, BOOL fPTD, UCHAR uchMethod); 
                                                    /* create waiter total file */
SHORT  TtlWaiDeleteFile(USHORT usWaiterNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete dept, total file */
/* VOID   TtlTtlUpdCreate(VOID);                      *//* create Total Update File */ 
SHORT  TtlTtlUpdCreate(USHORT usNoofItem);          /* create Total Update File */
VOID   TtlIndFinCreate(VOID);                       /* create individual financial */
VOID   TtlIndFinCheckDelete(VOID);                  /* Delete individual financial */
SHORT  TtlIndFinDeleteFile(USHORT usTermNum, UCHAR uchMethod);
                                                    /* delete individual financial */
SHORT  TtlSerTimeCreate(UCHAR uchMethod);           /* create service time */
SHORT  TtlSerTimeDeleteFile(VOID);                  /* delete service time */
SHORT  TtlTotalRead(VOID *pTotal);                  /* read total file */
SHORT  TtlTotalReset(VOID *pTotal, USHORT usMDCBit);/* reset total file */
SHORT  TtlTotalDelete(VOID *pTotal);                /* delete inidividual total data */
SHORT  TtlIssuedSet(VOID *pTotal, UCHAR uchStat);   /* set report issued flag */
SHORT  TtlIssuedReset(VOID *pTotal, UCHAR uchStat); /* set report issued flag */
SHORT  TtlIssuedCheck(VOID *pTotal, UCHAR uchStat); /* check issued flag's status */
SHORT  TtlLock(VOID);                               /* file access lock */
VOID   TtlUnlock(VOID);                             /* unlock file access */
SHORT  TtlUpdateFile(VOID *pTotal, USHORT usLength);/* update total file */
SHORT  TtlTotalCheck(VOID *pTotal);                 /* Check Total */
SHORT  TtlReqBackup(VOID);                          /* receive total file */
SHORT  TtlRespBackup(UCHAR *puchReqBuf, USHORT usSize, UCHAR *puchRcvBuf, USHORT *pusRcvLen); /* send response data */
SHORT  TtlReadWrtChk(VOID);                         /* check read&write point in total update file */
USHORT TtlConvertError(SHORT sError);               /* Convert error code  */
SHORT TtlStringCheck(VOID *pStrBuff, USHORT usLength, UCHAR uchCmpChar);
/* SHORT  TtlUpdateFileFH(USHORT usOffset, SHORT hsFileHandle, USHORT usLength);/* update total file */
/* USHORT TtlReadFileFH(ULONG ulOffset, VOID FAR *pData,                                                 */
/*                      USHORT usSize, SHORT hsFileHandle);    /* read data from interfaced file handle */

/* VOID   TtlTtlUpdDeleteFile(VOID);                   /* Delete Total Update File */
/* SHORT  TtlWaiTotalCheck(VOID);                      /* Check Waiter Total   */ 
/* SHORT  TtlCasTotalCheck(VOID);                      /* Check Cashier Total  */ 

/****** end of file ******/
#endif