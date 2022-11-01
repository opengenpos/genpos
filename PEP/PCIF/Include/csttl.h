/*
*===========================================================================
* Title       : TOTAL, Header File
* Category    : TOTAL, Hospitality Application
* Program Name: CSTTL.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:           :
* Jun-07-93:01.00.00: J.IKEDA   : Change TTL_MAX_CASHIER from 16 to 32
* Dec-08-93:02.00.03: K.You     : add 3 misc tenders and off line tender
* Mar-29-94:00.00.04: K.You     : expand stack size from 1500+150 to 5500+150
* Apr-08-94:00.00.04: Yoshiko.J : add TEND (for change offline tender)
* May-17-94:02.05.00: Yoshiko.J : add TTL_DELCASWAI, TtlWaiTotalCheck(), TtlCasTotalCheck(),
* Mar- 3-95:03.00.00: hkato     : R3.0
* Dec-26-95:03.01.00: T.Nakahata: R3.1 Initial
*   Add Service Time Total (Daily/PTD) and Individual Terminal Financial (Daily)
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   Add Some Discount and Void Elements at Server Total
*   Post Receipt(Parking Print) Counter at Server/Cashier Total
*   Restore Canada Tax (Non Taxable Total, Taxable[3] => Taxable[4])
* Mar-21-96:03.01.01: T.Nakahata : Cashier Total Offset USHORT to ULONG
*
* Jan-25-99:01.00.00: K.Yanagida : Saratoga initial
*
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

#if !defined(CSTTL_INCLUDED)
#define CSTTL_INCLUDED

#include "ECR.h"

// standard currency value for amounts and totals, etc.
// DCURRENCY is the type for currency amounts.
// DCLL is a macro to force a constant to be either long or long long in the
// argument list of a function call taking variable arguments such as UiePrintf().
// V_DCURRENCY is a macro which is used in the totals database update
// to correctly set the element of the VARIANT or COleVariant variable
// used to provide data for the SQL statements used with SQL Server.
#if 0
// following define is used in places where the DCURRENCY type is used to trigger other
// changes in behavior such as SQL statements in Total PLU. Not defined if not 64 bit totals.
//#define  DCURRENCY_LONGLONG

typedef short DSCOUNTER;    // used with totals for counts, non-64 bit is short but change to long
typedef long  DLCOUNTER;    // used with totals for counts
typedef long  DCURRENCY;
typedef long  DUNITPRICE;

#define V_DCURRENCY(x) V_I4(x)
#define DCLL(x)  x##L

#else
// following define is used in places where the DCURRENCY type is used to trigger other
// changes in behavior such as SQL statements in Total PLU.
#define  DCURRENCY_LONGLONG

typedef long  DSCOUNTER;    // used with totals for counts, non-64 bit is short but change to long
typedef long  DLCOUNTER;    // used with totals for counts
typedef long long	DCURRENCY;
typedef long  DUNITPRICE;

#define V_DCURRENCY(x) V_I8(x)
#define DCLL(x)  x##LL

#endif

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
#define     TTL_MAX_DEPT       250      /* Max Dept Number Size, 2172 */
#define     TTL_MAX_PLU      10000      /* Max PLU Number Size, Saratoga */
#define     TTL_MAX_CASHIER    300      /* Max Cashier Number Size, Mod V3.3 */
/* #define     TTL_MAX_CASHIER     99      Max Cashier Number Size, Mod R3.1 */
/* #define     TTL_MAX_CASHIER     32      Max Cashier Number Size */
#define     TTL_MAX_WAITER     100      /* Max Waiter Number Size */
#define     TTL_MAX_CPN        300      /* Max Coupon Number Size, R3.0 */
#define     TTL_MAX_SERTIME     3       /* Max Service Time Block, R3.1 */
#define     TTL_MAX_INDFIN     16       /* Max Indivi-Financial,   R3.1 */

#define     TTL_WITHOUT_PTD     0       /* create total file except PTD total */
#define     TTL_WITH_PTD        1       /* create total file with PTD total */

/* Error Code for user interface  */
#define     TTL_SUCCESS         0       /* Perform succesfully */
#define     TTL_ISSUED          0       /* reset report enable */
#define     TTL_UNLOCKED        0       /* file status (Unlocked) */
#define     TTL_EOF             1       /* end of recordset *//* ### ADD 2172 Rel1.0 (01/27/00) */

#define     TTL_FAIL           -1
#define     TTL_BUFFER_FULL    -2       /* Update Total File Full */
#define     TTL_NOTINFILE      -3       /* specified data not in file */
#define     TTL_NOT_ISSUED     -4       /* reset report disabled due to totals consistency check failed.  see also TTL_NOT_ISSUED_MDC */

#define     TTL_LOCKED         -5       /* file status (Locked) */
#define     TTL_CHECK_RAM      -6       /* Check RAM Size */
#define     TTL_NOTCLASS       -11      /* Illigal Class # */
#define     TTL_NOTPTD         -12      /* Not Exist PTD */

#define     TTL_DELCASWAI      -15      /* Delete Waiter Record */
#define     TTL_DELINDFIN      -16      /* Delete Individual Fin. Record */

#define     TTL_NOT_MASTER     -20      /* Prohibi operation */
#define		TTL_NO_READ_SIZE   -21		/* No Read Size Given SR 201*/
#define     TTL_NOT_ISSUED_MDC  (-24)   /* reset report disabled, MDC setting indicates unused */

/* Error Code for Internal  */
#define     TTL_SIZEOVER        -31     /* Max Size Over for Dept,
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

/* BackupStatus */
#define     TTL_BK_ACCEPTED_CLEARLY      -61    /* copy request accepted (clearly) */
#define     TTL_BK_ACCEPTED_LOWSTORAGE   -62    /* copy request accepted (lowstorage) */
#define     TTL_BK_DENIED_BUSY           -63    /* copy request denied (busy) */
#define     TTL_BK_DENIED_NOHEAP         -64    /* copy request denied (noheap) */
#define     TTL_BK_DENIED_NOSTORAGE      -65    /* copy request denied (nostorage) */
#define     TTL_BK_STAT_READY            -66    /* copy status (ready) */
#define     TTL_BK_STAT_BUSY             -67    /* copy status (busy) */

/*  New Total Update File Error Conditions*/
#define		TTL_UPD_WAI_ERROR			-68
#define		TTL_UPD_PLU_ERROR			-69
#define		TTL_UPD_DEP_ERROR			-70
#define		TTL_UPD_CAS_ERROR			-71
#define		TTL_UPD_CPN_ERROR			-72

/* Stack Size */
/* #define     TTL_STACK   (1500 + 150) *//* ### MOD 2172 Rel1.0 (01/07/00) */
#define     TTL_STACK   ((1500+150)*2)  /* Total Update Module Stack Size */
#define     TTL_THREDPRI        1       /* Total TUM Priority */

#define     TTL_NOTRESET        0x01    /* Disable Total State */
#define     TTL_DELETE          0x10    /* delete database, 06/27/01 */

#define     TTL_COMP_CREATE     0x00    /* Compulsory Creation File */
#define     TTL_CHECK_CREATE    0x01    /* Check Creation File */
#define     TTL_COMP_DEL        0x00    /* Compulsory Delete File */
#define     TTL_CHECK_DEL       0x01    /* Check Delete File */

//added to be used in TtlHourly for incremental read of Hourly info 
//- should match defines from NHPOS file - ttl.h
#define     TTL_HOUR_DAYCURR    (TTL_FIN_SIZE * 4)
#define     TTL_FIN_SIZE    sizeof(TTLCSREGFIN) /* Financial Total Size */
#define     TTL_MAX_BLOCK	48
#define     TTL_STAT_SIZE       sizeof(UCHAR) + (sizeof(DATE) * 2)
#define     TTL_HOUR_SIZE   sizeof(TTLCSHOURLY) /* Hourly Total Size */
#define     TTL_HOUR_PTDCURR    (TTL_FIN_SIZE * 4) + (TTL_HOUR_SIZE * 2)
/*
*===========================================================================
*   Type Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/* -- total amount & counter -- */
typedef struct {
    DCURRENCY    lAmount;            /* Total Amount */
    DSCOUNTER    sCounter;           /* Total Counter */
}TOTAL;

/* -- total amount & counter -- */
typedef struct {
    TOTAL   OnlineTotal;        /* Online To/Co         */
    TOTAL   OfflineTotal;       /* Offline To/Co        */
} TEND;                         /* add EPT '94 Apr.08   */

/* -- total amount & counter -- */
typedef struct {
    DCURRENCY    lAmount;            /* Total Amount */
	DLCOUNTER    lCounter;           /* Total Counter */
}LTOTAL;

/* --- on hand total amount & counter,  Saratoga --- */
typedef struct {
    TOTAL        Total;              /* Total amount & counter */
    DCURRENCY    lHandTotal;         /* On Hand Total */
}TENDHT;

//Workaround to avoid redefinition errors for DATE (from the Windows API)
#define DATE N_DATE
/* -- start date & end date -- */
#if defined(DCURRENCY_LONGLONG)
/* -- start date & end date -- */
// this struct must match up with part of the struct definition for DATE_TIME
// see function TtlDateWrite().
typedef struct {
	USHORT  usMin;              /* Minute */
	USHORT  usHour;             /* Hour */
	USHORT  usMDay;             /* Day */
	USHORT  usMonth;            /* Month */
	USHORT  usYear;             /* Year as 00 - 99 */
}DATE;
#else
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
    USHORT  usMDay;             /* Day */
    USHORT  usMonth;            /* Month */
}DATE;
#endif

/* -- start time & end time -- */
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
}TIME;

/* -- Tax Total -- */
typedef struct {
    D13DIGITS   mlTaxableAmount;    /* Taxable Amount */
    DCURRENCY   lTaxAmount;         /* Tax Amount */
    DCURRENCY   lTaxExempt;         /* Tax Exempt */
    DCURRENCY   lFSTaxExempt;       /* FoodStamp Tax Exempt *//* ### ADD Saratoga */
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
    DCURRENCY   lNetTotal;          /* Net Total, Release 3.1 */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    DCURRENCY   lTGGTotal;          /* Training Gross Group Total */
    TOTAL       DeclaredTips;       /* Declared Tips */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total */
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
    TOTAL       FoodStampCredit;    /* Food Stamp Credit *//* ### ADD Saratoga (052900) */
    TEND        CashTender;         /* Cash Tender          change EPT*/
    TEND        CheckTender;        /* Check Tender         change EPT*/
    TEND        ChargeTender;       /* Charge Tender        change EPT*/
    TEND        MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
    TOTAL       ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
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
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 JHHJ 3-15-04*/
    DCURRENCY   lConsTax;           /* Consolidation tax */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLREGFIN;

/* -- Hourly File Total -- */
typedef struct {
    DCURRENCY   lHourlyTotal;       /* Hourly Total */
    LONG        lItemproductivityCount; /* IP Counter */
    SHORT       sDayNoOfPerson;     /* Number of Person Counter */
    DCURRENCY   lBonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8, V3.3 */
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
	USHORT		usBlockRead;		/* Which Block we want to read from NHPOS JHHJ 9-15-05*/
    HOURLY      Total[48];          /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY;

/* ===== New Element - Service Time Total (Relase 3.1) BEGIN ===== */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Period From */
    DATE        ToDate;             /* Period To */
    TOTAL       ServiceTime[48][3]; /* Block Data of Service Time 1 to 48, V3.3 */
} TTLSERTIME;
/* ===== New Element - Service Time Total (Relase 3.1) END ===== */

/* -- Department File Total, 2172 -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
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
    UCHAR       uchAdjectNo;        /* Adjective Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUTotal;           /* PLU Total */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
}TTLPLU;


/* ADD Saratoga */

/* -- PLU File Total for SerTtlTotalReadPluEx-- */
typedef struct {
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    LTOTAL      PLUTotal;           /* PLU Total */
}TTLPLUTOTAL;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
	LTOTAL      PLUAllTotal;        /* PLU All Total */
    TTLPLUTOTAL Plu[20];            /* PLU Total */
}TTLPLUEX;


/* -- Coupon File Total, R3.0 -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usCpnNumber;       /* Coupon Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    TOTAL       CpnTotal;           /* Department Total */
}TTLCPN;

/* -- Cashier File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulCashierNumber;    /* Cashier Number */
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total */
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
    TOTAL       FoodStampCredit;    /* Food Stamp Credit */
    TENDHT      CashTender;         /* Cash Tender, Saratoga */
    TENDHT      CheckTender;        /* Check Tender */
    TENDHT      ChargeTender;       /* Charge Tender */
    TENDHT      MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT      ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
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
    TOTAL       Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 100 JHHJ 03-15-04*/
    DCURRENCY   lConsTax;           /* Consolidation tax */
    /* ===== New Element (Release 3.1) BEGIN ===== */
    USHORT      usPostRecCo;        /* post receipt counter, R3,1 */
    /* ===== New Element (Release 3.1) END ===== */
    DCURRENCY   lVATServiceTotal;   /* Service Total of VAT, V3.3*/
    TOTAL       CheckTransTo;       /* Checks Transferred to, V3.3 */
    TOTAL       CheckTransFrom;     /* Checks Transferred from, V3.3 */
    TOTAL       DeclaredTips;       /* Declared Tips, V3.3 */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	//addition for release 2.1 JHHJ
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCASHIER;


/* -- Report Structure for Read/Write -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usNumber;           /* Number (Dept,PLU,Cashier,Waiter) */
    UCHAR       uchID;              /* ID (PLU - Adjective) */
}TTLREPORT;

/* -- Cashier File Total (only tender, saratoga, 07/21/00) -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulCashierNumber;    /* Cashier Number */
    DCURRENCY   lCashTender;        /* Cash Tender, Saratoga */
    DCURRENCY   lCheckTender;       /* Check Tender */
    DCURRENCY   lChargeTender;      /* Charge Tender */
    DCURRENCY   lMiscTender[STD_TENDER_MAX-3];     /* Misc Tender 1 to 8 */
    DCURRENCY   lForeignTotal[8];   /* Foreign Total 1 to 8,    Saratoga */
}TTLCASTENDER;

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
    /* ===== New Ele ment (Release 3.1) BEGIN ===== */
    DCURRENCY   lNetTotal;          /* Net Total,   Release 3.1 */
    /* ===== New Element (Release 3.1) END ===== */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    DCURRENCY   lTGGTotal;          /* Training Gross Group Total */
    TOTAL       DeclaredTips;       /* Declared Tips */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total */
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
	TOTAL       FoodStampCredit;    /* Food Stamp Credit */
    TEND        CashTender;         /* Cash Tender          change EPT*/
    TEND        CheckTender;        /* Check Tender         change EPT*/
    TEND        ChargeTender;       /* Charge Tender        change EPT*/
    TEND        MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
    TOTAL       ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
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
    DCURRENCY   lConsTax;           /* Consolidation tax */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCSREGFIN;

/* -- Hourly Activity File Total -- */
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    HOURLY      Total[48];          /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLCSHOURLY;

/* ===== New Element - Service Time Total (Release 3.1) BEGIN ===== */
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Period From */
    DATE        ToDate;             /* Period To */
    TOTAL       ServiceTime[48][3]; /* Block Data of Service Time 1 to 48, V3.3 */
} TTLCSSERTIME;
/* ===== New Element - Service Time Total (Release 3.1) END ===== */

/* ===== New Element - Individual Financial Total (Release 3.1) BEGIN ===== */
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
/* ===== New Element - Individual Financial Total (Release 3.1) END ===== */

/* -- Department File Total -- */
typedef struct {
    ULONG       ulDeptFileSize;      /* File Size */
    USHORT      usMaxDept;           /* Max Dept Number */
    USHORT      usCurDayDept;        /* Current Daily Dept Number */
    USHORT      usPreDayDept;        /* Previous Daily Dept Number */
    USHORT      usCurPTDDept;        /* Current PTD Dept Number */
    USHORT      usPrePTDDept;        /* Previous PTD Dept Number */
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
}TTLCSDEPTHEAD;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    LTOTAL      DeptAllTotal;        /* Dept All Total */
}TTLCSDEPTMISC;

typedef struct {
    USHORT      usDeptNo;            /* Dept Number */
}TTLCSDEPTINDX;

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
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
#endif

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
    ULONG       ulCasNo;            /* Cashier Number */
    USHORT      usBlockNo;          /* Total Block Number, V3.3 */
}TTLCSCASINDX;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       ConsCoupon;         /* Consolidated Coupon */
    TOTAL       ItemDisc;           /* Item Discount */
    TOTAL       ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL       RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL       Coupon;             /* Combination Coupon, R3.0 */
    TOTAL       PaidOut;            /* Paid Out */
    TOTAL       RecvAcount;         /* Received on Acount */
    TOTAL       TipsPaid;           /* Tips Paid Out */
	TOTAL       FoodStampCredit;    /* Food Stamp Credit */
    TENDHT      CashTender;         /* Cash Tender */
    TENDHT      CheckTender;        /* Check Tender */
    TENDHT      ChargeTender;       /* Charge Tender */
    TENDHT      MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT      ForeignTotal[8];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
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
    DCURRENCY   lConsTax;           /* Consolidation tax */
    USHORT      usPostRecCo;        /* post receipt counter, R3,1 */
    DCURRENCY   lVATServiceTotal;   /* Service Total of VAT, V3.3*/
    TOTAL       CheckTransTo;       /* Checks Transferred to, V3.3 */
    TOTAL       CheckTransFrom;     /* Checks Transferred from, V3.3 */
    TOTAL       DeclaredTips;       /* Declared Tips, V3.3 */
    TOTAL       aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
		//addition for release 2.1 JHHJ
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
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

typedef struct {
                    UCHAR  uchMDEPT;
                    USHORT usDEPT;
					WCHAR  auchMnemo[STD_MAJDEPTMNEMO_LEN];
                    UCHAR  uchStatus;
                    LTOTAL DEPTTotal;
} DEPTTBL;

typedef struct {
                    USHORT usCpnNo;
					WCHAR  auchMnemo[STD_CPNNAME_LEN];
                    TOTAL CPNTotal;
} CPNTBL;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
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
SHORT  TtlPLUCreate(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod);        /* ### MOD 2172 Rel1.0 */
                                                    /* create plu total file */
SHORT  TtlPLUDeleteFile(ULONG ulPLUNum, BOOL fPTD, UCHAR uchMethod);    /* ### MOD 2172 Rel1.0 */
                                                    /* Delete dept, total file */
/* -- R3.0  -- */
SHORT  TtlCpnCreate(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* create coupon, total file */
SHORT  TtlCpnDeleteFile(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete coupon, total file */
SHORT  TtlCasCreate(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* create cashier total file */
SHORT  TtlCasDeleteFile(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete dept, total file */
SHORT  TtlWaiCreate(USHORT usWaiterNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* create waiter total file */
SHORT  TtlWaiDeleteFile(USHORT usWaiterNum, BOOL fPTD, UCHAR uchMethod);
                                                    /* Delete dept, total file */
SHORT  TtlTtlUpdCreate(USHORT usNoofItem);          /* create Total Update File */
/* ===== New Functions (Release 3.1) BEGIN ===== */
VOID   TtlIndFinCreate(VOID);                       /* create individual financial */
VOID   TtlIndFinCheckDelete(VOID);                  /* Delete individual financial */
SHORT  TtlIndFinDeleteFile(USHORT usTermNum, UCHAR uchMethod);
                                                    /* delete individual financial */
SHORT  TtlSerTimeCreate(UCHAR uchMethod);           /* create service time */
SHORT  TtlSerTimeDeleteFile(VOID);                  /* delete service time */
/* ===== New Functions (Release 3.1) END ===== */
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
SHORT  TtlStringCheck(VOID *pStrBuff, USHORT usLength, UCHAR uchCmpChar);
SHORT  TtlUpdateFileFH(USHORT usOffset, SHORT hsFileHandle, USHORT usLength);/* update total file */
USHORT TtlReadFileFH(ULONG ulOffset, VOID FAR *pData,
                     USHORT usSize, SHORT hsFileHandle);    /* read data from interfaced file handle */

VOID   TtlTtlUpdDeleteFile(VOID);                   /* Delete Total Update File */
SHORT  TtlWaiTotalCheck(VOID);                      /* Check Waiter Total   */
SHORT  TtlCasTotalCheck(VOID);                      /* Check Cashier Total  */

SHORT   TtlTenderAmountRead(TTLCASTENDER *pTender);    /* Cashier Tender Read, saratoga */

SHORT TtlPLUOptimize(UCHAR uchMinorClass);

#endif      /* CSTTL_INCLUDED */

/****** end of file ******/