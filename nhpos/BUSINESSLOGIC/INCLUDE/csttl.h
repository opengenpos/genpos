/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Header File
* Category    : TOTAL, NCR 2170 Hospitality Application
* Program Name: CSTTL.H
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
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
* Aug-11-99:03.05.00: M.Teraki   : R3.5 remove WAITER_MODEL
*
** NCR2172 **
*
* Oct-05-99:01.00.00: M.Teraki   :initial (for 2172)
* Nov-30-99:01.00.00: hrkato     :Saratoga (Money)
* Apr-19-15:02.02.01: R.Chambers :correct SavedTotals file, use STD_MAX_HOURBLK in HOURLY, etc.
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

// make sure the Flexible Memory size limitations are included.
#include "paraequ.h"

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

#define     CLI_PLU_EX_MAX_NO   20      /* Maximum No of PLU,   Saratoga */

/* Major Data Class */


/* Minor Data Class */
#define     CLASS_TTLIGNORE     0       /* No total class - used with RptPLUDayRead(), do nothing with PTD or Daily totals */
#define     CLASS_TTLCURDAY     1       /* Current Daily File (usually single day or two since last End of Day action) */
#define     CLASS_TTLSAVDAY     2       /* Previous or Saved Daily File from last reset of Current Daily File */
#define     CLASS_TTLCURPTD     3       /* Current Period To Day File (may be days or months of accumulated totals) */
#define     CLASS_TTLSAVPTD     4       /* Previous or Saved PTD File from last reset of Current PTD File */
#define		CLASS_SAVED_TOTAL   5		/* Saved File */
#define		CLASS_STORE_FORWARD	6		/* Store and Forward */

/* Maxmum Number */
//#define     TTL_MAX_DEPT       250      /* Max Dept Number Size, 2172 */
// #define     TTL_MAX_PLU      10000      /* Max PLU Number Size, Saratoga */
// #define     TTL_MAX_CASHIER    300      /* Max Cashier Number Size, Mod V3.3 - See FLEX_CAS_MAX instead. */
/* #define     TTL_MAX_CASHIER     99      Max Cashier Number Size, Mod R3.1 */
/* #define     TTL_MAX_CASHIER     32      Max Cashier Number Size */
// #define     TTL_MAX_WAITER     100		/* Max Waiter Number Size */      
//#define     TTL_MAX_SERTIME     3       /* Max Service Time Block, R3.1 */
#define     TTL_MAX_INDFIN     16       /* Max Individual-Financial (one per terminal or MAX_TERMINAL_NO), STD_NUM_OF_TERMINALS  R3.1 */

#define     TTL_WITHOUT_PTD     0       /* create total file except PTD total */
#define     TTL_WITH_PTD        1       /* create total file with PTD total */

/* Total subsystem error codes
   These error codes are converted by function TtlConvertError()
   into LDT_ type codes for use by function UieErrorMsg() to
   display errors to the operator.

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Guest Check File
   error codes defined in file csgcs.h
 */
#define     TTL_SUCCESS         0       /* Perform succesfully */                
#define     TTL_ISSUED          0       /* reset report enable */
#define     TTL_UNLOCKED        0       /* file status (Unlocked) */
#define     TTL_EOF             1       /* end of recordset *//* ### ADD 2172 Rel1.0 (01/27/00) */

#define     TTL_FAIL           (-1)
#define     TTL_BUFFER_FULL    (-2)       /* Update Total File Full */
#define     TTL_NOTINFILE      (-3)       /* specified data not in file */
#define     TTL_NOT_ISSUED     (-4)       /* reset report disabled due to totals consistency check failed.  see also TTL_NOT_ISSUED_MDC */

#define     TTL_LOCKED         (-5)       /* file status (Locked) */
#define     TTL_CHECK_RAM      (-6)       /* Check RAM Size */
#define		TTL_BKUPPLUDBFAIL  (-7)		/* Updating the Plu Ttl Database failed JHHJ */
#define     TTL_NOTCLASS       (-11)      /* Illigal Class # */
#define     TTL_NOTPTD         (-12)      /* Not Exist PTD */

#define     TTL_DELCASWAI      (-15)      /* Delete Waiter Record */
#define     TTL_DELINDFIN      (-16)      /* Delete Individual Fin. Record, STD_NUM_OF_TERMINALS */

#define     TTL_NOT_MASTER     (-20)      /* Prohibi operation */
#define		TTL_NO_READ_SIZE   (-21)		/* No Read Size Given SR 201*/

#define		TTL_NEAR_FULL	   (-22)		/* Total DB File is almost full */
#define     TTL_NOT_ISSUED_MDC  (-24)       /* reset report disabled, MDC setting indicates unused */

/* Error Code for Internal  */
#define     TTL_SIZEOVER        (-31)     /* Max Size Over for Dept, 
                                           PLU, Cashier, Waiter */
#define     TTL_NOTZERO         (-32)     /* Total Not Zero */
#define     TTL_NOTEMPTY        (-33)     /* Empty Table Full */

#define		TTL_STATUS_DCUR		(-35)		/* Create Status record error */
#define		TTL_INSERT_DCUR		(-36)		/* Insert plu total record */
#define		TTL_FIND_DCUR		(-37)		/* Find plu total record */
#define		TTL_UPDATE_DCUR		(-38)		/* update plu total record */
#define		TTL_TSTAT_GDCUR		(-39)		/* Create Status record error */
#define		TTL_TSTAT_SDCUR		(-40)		/* Create Status record error */

#define		TTL_STATUS_PTD_CUR	(-41)		/* Create Status record error */
#define		TTL_INSERT_PTD_CUR	(-42)		/* Insert plu total record */
#define		TTL_FIND_PTD_CUR	(-43)		/* Find plu total record */
#define		TTL_UPDATE_PTD_CUR	(-44)		/* update plu total record */
#define		TTL_TSTAT_GPTD_CUR	(-45)		/* Create Status record error */
#define		TTL_TSTAT_SPTD_CUR	(-46)		/* Create Status record error */

// The following errors are file access errors which if reported by an
// underlying function such as TtlTresetCpn() will trigger a TtlAbort().
// Typically code such as the following example is used with these error codes:
//		if (sRetvalue <= TTL_FILE_OPEN_ERR) {
//			if (sRetvalue != TTL_FILE_HANDLE_ERR) {
//				TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
//			}
//		}
#define     TTL_FILE_OPEN_ERR   (-50)     /* File Open Error */
#define     TTL_FILE_SEEK_ERR   (-51)     /* File Seek Error */
#define     TTL_FILE_READ_ERR   (-52)     /* File Read Error */
#define     TTL_FILE_HANDLE_ERR (-53)     /* File Handle Error */
#define     TTL_AFFECTION_ERR   (-54)     /* Affection Error */
#define     TTL_BAD_LENGTH      (-55)     /* BAD Length of TUM Read */
#define		TTL_UPD_DB_BUSY		(-56)

/* ### ADD Saratoga BackupStatus (060100) */
#define     TTL_BK_ACCEPTED_CLEARLY      (-61)    /* copy request accepted (clearly) */
#define     TTL_BK_ACCEPTED_LOWSTORAGE   (-62)    /* copy request accepted (lowstorage) */
#define     TTL_BK_DENIED_BUSY           (-63)    /* copy request denied (busy) */
#define     TTL_BK_DENIED_NOHEAP         (-64)    /* copy request denied (noheap) */
#define     TTL_BK_DENIED_NOSTORAGE      (-65)    /* copy request denied (nostorage) */
#define     TTL_BK_STAT_READY            (-66)    /* copy status (ready) */
#define     TTL_BK_STAT_BUSY             (-67)    /* copy status (busy) */

/*  New Total Update File Error Conditions*/
#define		TTL_UPD_WAI_ERROR			(-68)
#define		TTL_UPD_PLU_ERROR			(-69)
#define		TTL_UPD_DEP_ERROR			(-70)
#define		TTL_UPD_CAS_ERROR			(-71)
#define		TTL_UPD_CPN_ERROR			(-72)

#define     TTL_UPD_M_DOWN_ERROR        (-73)
#define     TTL_UPD_BM_DOWN_ERROR       (-74)

// Following are used to transform Network Layer (STUB_) errors into TTL_ errors
// Notice that error code is same ast STUB_ plus STUB_RETCODE.
#define		TTL_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define		TTL_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define     TTL_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define     TTL_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define     TTL_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define     TTL_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)


/* Stack Size */                                        
/*#define     TTL_STACK   (1500 + 150)*//* ### MOD 2172 Rel1.0 (01/07/00) */
#define     TTL_STACK   ((1500+150)*2)  /* Total Update Module Stack Size */
#define     TTL_THREDPRI        1       /* Total TUM Priority */

#define     TTL_NOTRESET        0x01    /* Disable Total State */
#define     TTL_DELETE          0x10    /* delete database, 06/27/01 */

#define     TTL_COMP_CREATE     0x00    /* Compulsory Creation File */
#define     TTL_CHECK_CREATE    0x01    /* Check Creation File */
#define     TTL_COMP_DEL        0x00    /* Compulsory Delete File */
#define     TTL_CHECK_DEL       0x01    /* Check Delete File */                                                           
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
    DCURRENCY  lAmount;            /* Total Amount */
	DSCOUNTER  sCounter;           /* Total Counter */
}TOTAL;

typedef struct {
    LONG       lAmount;            /* Total Amount as legacy 32 bit long */
    SHORT      sCounter;           /* Total Counter */
}TOTAL_32;

/* -- total amount & counter -- */
typedef struct {
    TOTAL   OnlineTotal;        /* Online To/Co         */
    TOTAL   OfflineTotal;       /* Offline To/Co        */
} TEND;                         /* add EPT '94 Apr.08   */

typedef struct {
    TOTAL_32   OnlineTotal;        /* Online To/Co         */
    TOTAL_32   OfflineTotal;       /* Offline To/Co        */
} TEND_32;                         /* add EPT '94 Apr.08   */

/* -- total amount & counter -- */
typedef struct {
    DCURRENCY    lAmount;            /* Total Amount */
    LONG         lCounter;           /* Total Counter */
}LTOTAL;

typedef struct {
    LONG         lAmount;            /* Total Amount as legacy 32 bit long */
    LONG         lCounter;           /* Total Counter */
}LTOTAL_32;

/* --- on hand total amount & counter,  Saratoga --- */
typedef struct {
    TOTAL       Total;              /* Total amount & counter */
    DCURRENCY   lHandTotal;         /* On Hand Total */
}TENDHT;

typedef struct {
    TOTAL_32    Total;              /* Total amount & counter */
    LONG        lHandTotal;         /* On Hand Total */
}TENDHT_32;

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
}N_DATE;
#else
/* -- start date & end date -- */
// this struct must match up with part of the struct definition for DATE_TIME
// see function TtlDateWrite().
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
    USHORT  usMDay;             /* Day */
    USHORT  usMonth;            /* Month */
}N_DATE;
#endif

/* -- start date & end date -- */
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
    USHORT  usMDay;             /* Day */
    USHORT  usMonth;            /* Month */
}N_DATE_32;

/* -- start time & end time -- */
typedef struct {
    USHORT  usMin;              /* Minute */
    USHORT  usHour;             /* Hour */
}N_TIME;

/* -- Tax Total -- */
typedef struct {
    D13DIGITS   mlTaxableAmount;    /* Taxable Amount, reset at EOD governed by MDC_SUPER_ADR, ODD_MDC_BIT2 see TtlTresetFin() */
    DCURRENCY   lTaxAmount;         /* Tax Amount */
    DCURRENCY   lTaxExempt;         /* Tax Exempt */
    DCURRENCY   lFSTaxExempt;       /* FoodStamp Tax Exempt */
}TAXABLE;

typedef struct {
    D13DIGITS   mlTaxableAmount;    /* Taxable Amount */
    LONG        lTaxAmount;         /* Tax Amount */
    LONG        lTaxExempt;         /* Tax Exempt */
    LONG        lFSTaxExempt;       /* FoodStamp Tax Exempt */
}TAXABLE_32;

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
	TOTAL_32    TtlIAmount[4];       // Item discounts for 3 through 6
} TTLITEMDISC_32;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usTerminalNumber;   /* terminal unique number. R3.1 */
    // --------
	// --------    Beginning here must be same struct as TTLCSREGFIN
	//             Any changes from this point must be synchronized with TTLCSREGFIN
    UCHAR       uchResetStatus;     /* Reset Report Status */
    N_DATE      FromDate;           /* Preiod From */
    N_DATE      ToDate;             /* Preiod To */
    D13DIGITS   CGGTotal;           /* Current Gross Group Total */
    DCURRENCY   lNetTotal;          /* Net Total, Release 3.1 */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0 see also STD_TAX_ITEMIZERS_MAX */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    DCURRENCY   lTGGTotal;          /* Training Gross Group Total */
    TOTAL       DeclaredTips;       /* Declared Tips */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN1_ADR, MDC_RPTFIN46_ADR */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN2_ADR, MDC_RPTFIN46_ADR */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN3_ADR, MDC_RPTFIN46_ADR */
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
    TOTAL       ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
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

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usTerminalNumber;   /* terminal unique number. R3.1 */
    // --------
	// --------    Beginning here must be same struct as TTLCSREGFIN
	//             Any changes from this point must be synchronized with TTLCSREGFIN
    UCHAR       uchResetStatus;     /* Reset Report Status */
    N_DATE_32   FromDate;           /* Preiod From */
    N_DATE_32   ToDate;             /* Preiod To */
    D13DIGITS   CGGTotal;           /* Current Gross Group Total */
    LONG        lNetTotal;          /* Net Total, Release 3.1 */
    TAXABLE_32  Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    LONG        lTGGTotal;          /* Training Gross Group Total */
    TOTAL_32    DeclaredTips;       /* Declared Tips */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL_32    PlusVoid;           /* Plus Void */
    TOTAL_32    PreselectVoid;      /* Preselect Void */
    TOTAL_32    TransactionReturn;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN1_ADR, MDC_RPTFIN46_ADR */
    TOTAL_32    TransactionExchng;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN2_ADR, MDC_RPTFIN46_ADR */
    TOTAL_32    TransactionRefund;  /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN3_ADR, MDC_RPTFIN46_ADR */
    TOTAL_32    ConsCoupon;         /* Consolidated Coupon */
    TOTAL_32    ItemDisc;           /* Item Discount */
    TOTAL_32    ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC_32 ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL_32    RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL_32    Coupon;             /* Combination Coupon, R3.0 */
    TOTAL_32    PaidOut;            /* Paid Out */
    TOTAL_32    RecvAcount;         /* Received on Acount */
    TOTAL_32    TipsPaid;           /* Tips Paid Out */
    TOTAL_32    FoodStampCredit;    /* Food Stamp Credit */
    TEND_32     CashTender;         /* Cash Tender          change EPT*/
    TEND_32     CheckTender;        /* Check Tender         change EPT*/
    TEND_32     ChargeTender;       /* Charge Tender        change EPT*/
    TEND_32     MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
    TOTAL_32    ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL_32    ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL_32    AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL_32    ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL_32    Loan;               /* Loan,    Saratoga */
    TOTAL_32    Pickup;             /* Pickup,  Saratoga */
    TOTAL_32    TransCancel;        /* Transaction Cancel */
    TOTAL_32    MiscVoid;           /* Misc Void */
    TOTAL_32    Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL_32    HashDepartment;     /* Hash Department */
    TOTAL_32    Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 JHHJ 3-15-04*/
    LONG        lConsTax;           /* Consolidation tax */
    TOTAL_32    aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	TOTAL_32	ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL_32	ttlCoinLoan;			//New loan totals
	TOTAL_32	ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLREGFIN_32;

/* -- Hourly File Total -- */
typedef struct {
    DCURRENCY   lHourlyTotal;       /* Hourly Total */
    LONG        lItemproductivityCount; /* IP Counter */
    SHORT       sDayNoOfPerson;     /* Number of Person Counter */
    DCURRENCY   lBonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 100, V3.3 JHHJ 3-15-04*/
}HOURLY;

typedef struct {
    LONG        lHourlyTotal;       /* Hourly Total */
    LONG        lItemproductivityCount; /* IP Counter */
    SHORT       sDayNoOfPerson;     /* Number of Person Counter */
    LONG        lBonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 100, V3.3 JHHJ 3-15-04*/
}HOURLY_32;

/* -- Hourly Activity File Total -- */
// WARNING: Any changes also see also struct TTLCSHOURLY
typedef struct {
    UCHAR       uchMajorClass;              /* Major Class Data definition */
    UCHAR       uchMinorClass;              /* Minor Class Data definition */
    UCHAR       uchResetStatus;             /* Reset Status, TTL_STAT_SIZE and TTLCSHOURLY starts here */
    N_DATE      FromDate;                   /* Period From */
    N_DATE        ToDate;                   /* Period To */
    N_TIME        StartTime;                /* Start Time */
    N_TIME        EndTime;                  /* End Time */
	USHORT		usBlockRead;		        /* Which Block we want to read from NHPOS JHHJ 9-15-05*/		
    HOURLY      Total[STD_MAX_HOURBLK];     /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY;

typedef struct {
    UCHAR       uchMajorClass;              /* Major Class Data definition */
    UCHAR       uchMinorClass;              /* Minor Class Data definition */
    UCHAR       uchResetStatus;             /* Reset Status, TTLCSHOURLY starts here */
    N_DATE      FromDate;                   /* Period From */
    N_DATE        ToDate;                   /* Period To */
    N_TIME        StartTime;                /* Start Time */
    N_TIME        EndTime;                  /* End Time */
	USHORT		usBlockRead;		        /* Which Block we want to read from NHPOS JHHJ 9-15-05*/		
    HOURLY      Total[1];     /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY_1;

typedef struct {
    UCHAR       uchMajorClass;              /* Major Class Data definition */
    UCHAR       uchMinorClass;              /* Minor Class Data definition */
    UCHAR       uchResetStatus;             /* Reset Status, TTLCSHOURLY starts here */
    N_DATE_32   FromDate;                   /* Period From */
    N_DATE_32   ToDate;                   /* Period To */
    N_TIME      StartTime;                /* Start Time */
    N_TIME      EndTime;                  /* End Time */
	USHORT		usBlockRead;		        /* Which Block we want to read from NHPOS JHHJ 9-15-05*/		
    HOURLY_32   Total[STD_MAX_HOURBLK];     /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY_32;

typedef struct {
    UCHAR       uchMajorClass;            /* Major Class Data definition */
    UCHAR       uchMinorClass;            /* Minor Class Data definition */
    UCHAR       uchResetStatus;           /* Reset Status, TTLCSHOURLY starts here */
    N_DATE_32   FromDate;                 /* Period From */
    N_DATE_32   ToDate;                   /* Period To */
    N_TIME      StartTime;                /* Start Time */
    N_TIME      EndTime;                  /* End Time */
	USHORT		usBlockRead;		      /* Which Block we want to read from NHPOS JHHJ 9-15-05*/		
    HOURLY_32   Total[1];                 /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLHOURLY_32_1;

/* ===== New Element - Service Time Total (Relase 3.1) BEGIN ===== */
// WARNING: Any changes also see also struct TTLCSSERTIME
typedef struct {
    UCHAR       uchMajorClass;                    /* Major Class Data definition */
    UCHAR       uchMinorClass;                    /* Minor Class Data definition */
    UCHAR       uchResetStatus;                   /* Reset Status, TTL_STAT_SIZE and TTLCSSERTIME starts here */
    N_DATE      FromDate;                         /* Period From */
    N_DATE      ToDate;                           /* Period To */
    TOTAL       ServiceTime[STD_MAX_HOURBLK][3];  /* Block Data of Service Time 1 to 48, V3.3 */
} TTLSERTIME;

typedef struct {
    UCHAR       uchMajorClass;                    /* Major Class Data definition */
    UCHAR       uchMinorClass;                    /* Minor Class Data definition */
    UCHAR       uchResetStatus;                   /* Reset Status, TTLCSSERTIME starts here */
    N_DATE_32   FromDate;                         /* Period From */
    N_DATE_32   ToDate;                           /* Period To */
    TOTAL_32    ServiceTime[STD_MAX_HOURBLK][3];  /* Block Data of Service Time 1 to 48, V3.3 */
} TTLSERTIME_32;

/* ===== New Element - Service Time Total (Relase 3.1) END ===== */

/* -- Department File Total, 2172 -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usDEPTNumber;      /* Department Number */
    UCHAR       uchResetStatus;     /* Reset Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;           /* Preiod From */
    N_DATE        ToDate;             /* Preiod To */
    UCHAR       uchMajorDEPTNo;     /* Major DEPT Number */
    LTOTAL      DEPTTotal;          /* Department Total */
    LTOTAL      DEPTAllTotal;       /* Department All Total */
}TTLDEPT;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usDEPTNumber;      /* Department Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE_32   FromDate;           /* Preiod From */
    N_DATE_32   ToDate;             /* Preiod To */
    UCHAR       uchMajorDEPTNo;     /* Major DEPT Number */
    LTOTAL_32   DEPTTotal;          /* Department Total */
    LTOTAL_32   DEPTAllTotal;       /* Department All Total */
}TTLDEPT_32;

/* -- PLU File Total -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE        FromDate;           /* Preiod From */
    N_DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUTotal;           /* PLU Total */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
}TTLPLU;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE_32   FromDate;           /* Preiod From */
    N_DATE_32   ToDate;             /* Preiod To */
    LTOTAL_32   PLUTotal;           /* PLU Total */
    LTOTAL_32   PLUAllTotal;        /* PLU All Total */
}TTLPLU_32;

/* -- PLU File Total for SerTtlTotalReadPluEx-- */
typedef struct {
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    LTOTAL      PLUTotal;           /* PLU Total */
}TTLPLUTOTAL;

typedef struct {
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
    UCHAR       uchAdjectNo;        /* Adjective Number */
    LTOTAL_32   PLUTotal;           /* PLU Total */
}TTLPLUTOTAL_32;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE        FromDate;           /* Preiod From */
    N_DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
    TTLPLUTOTAL Plu[CLI_PLU_EX_MAX_NO];  /* PLU Total */
}TTLPLUEX;

typedef struct {
    UCHAR           uchMajorClass;      /* Major Class Data definition */
    UCHAR           uchMinorClass;      /* Minor Class Data definition */
    UCHAR           uchResetStatus;     /* Reset Status */
    N_DATE_32       FromDate;           /* Preiod From */
    N_DATE_32       ToDate;             /* Preiod To */
    LTOTAL_32       PLUAllTotal;        /* PLU All Total */
    TTLPLUTOTAL_32  Plu[CLI_PLU_EX_MAX_NO];  /* PLU Total */
}TTLPLUEX_32;

/* -- Coupon File Total, R3.0 -- */
// WARNING:  The first few members of the TTLCPN struct are used
//           to provide information into functions.  The area that
//           is used to return results starts at uchResetStatus.
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usCpnNumber;        /* Coupon Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE      FromDate;           /* Preiod From */
    N_DATE      ToDate;             /* Preiod To */
    TOTAL       CpnTotal;           /* Department Total */
}TTLCPN;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usCpnNumber;        /* Coupon Number */
    UCHAR       uchResetStatus;     /* Reset Status */
    N_DATE_32   FromDate;           /* Preiod From */
    N_DATE_32   ToDate;             /* Preiod To */
    TOTAL_32    CpnTotal;           /* Department Total */
}TTLCPN_32;

/* -- Cashier File Total -- */
//  WARNING:  This struct must have the same layout for members as does
//            the struct TTLCSCASTOTAL beginning with the member uchResetStatus.
//            There is a dependency on the layout being the same in several
//            places of the totals update.
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulCashierNumber;    /* Cashier Number */
    UCHAR       uchResetStatus;     /* Reset Report Status, **  TTL_STAT_SIZE and TTLCSCASTOTAL starts here */
    N_DATE        FromDate;           /* Preiod From */
    N_DATE        ToDate;             /* Preiod To */
    DCURRENCY   lDGGtotal;          /* Daily Gross Group Total: USRPTCASLENTTL begins here */
    TOTAL       PlusVoid;           /* Plus Void */
    TOTAL       PreselectVoid;      /* Preselect Void */
    TOTAL       TransactionReturn;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
    TOTAL       TransactionExchng;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
    TOTAL       TransactionRefund;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
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
    TENDHT      ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
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
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0, see also STD_TAX_ITEMIZERS_MAX */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCASHIER;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulCashierNumber;    /* Cashier Number */
    UCHAR       uchResetStatus;     /* Reset Report Status, **  TTL_STAT_SIZE and TTLCSCASTOTAL starts here */
    N_DATE_32   FromDate;           /* Preiod From */
    N_DATE_32   ToDate;             /* Preiod To */
    LONG        lDGGtotal;          /* Daily Gross Group Total: USRPTCASLENTTL begins here */
    TOTAL_32    PlusVoid;           /* Plus Void */
    TOTAL_32    PreselectVoid;      /* Preselect Void */
    TOTAL_32    TransactionReturn;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
    TOTAL_32    TransactionExchng;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
    TOTAL_32    TransactionRefund;  /* transaction return, CURQUAL_TRETURN, report MDC_RPTCAS50_ADR */
    TOTAL_32    ConsCoupon;         /* Consolidated Coupon */
    TOTAL_32    ItemDisc;           /* Item Discount */
    TOTAL_32    ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC_32  ItemDiscExtra;  /* Item discounts 3 through 6  */
    TOTAL_32    RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL_32    Coupon;             /* Combination Coupon, R3.0 */
    TOTAL_32    PaidOut;            /* Paid Out */
    TOTAL_32    RecvAcount;         /* Received on Acount */
    TOTAL_32    TipsPaid;           /* Tips Paid Out */
    TOTAL_32    FoodStampCredit;    /* Food Stamp Credit */
    TENDHT_32   CashTender;         /* Cash Tender, Saratoga */
    TENDHT_32   CheckTender;        /* Check Tender */
    TENDHT_32   ChargeTender;       /* Charge Tender */
    TENDHT_32   MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT_32   ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL_32    ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL_32    AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL_32    ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL_32    Loan;               /* Loan,    Saratoga */
    TOTAL_32    Pickup;             /* Pickup,  Saratoga */
    TOTAL_32    TransCancel;        /* Transaction Cancel */
    TOTAL_32    MiscVoid;           /* Misc Void */
    TOTAL_32    Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL_32    HashDepartment;     /* Hash Department */
    TOTAL_32    Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 100 JHHJ 03-15-04*/
    LONG        lConsTax;           /* Consolidation tax */
    /* ===== New Element (Release 3.1) BEGIN ===== */
    USHORT      usPostRecCo;        /* post receipt counter, R3,1 */
    /* ===== New Element (Release 3.1) END ===== */
    LONG        lVATServiceTotal;   /* Service Total of VAT, V3.3*/
    TOTAL_32    CheckTransTo;       /* Checks Transferred to, V3.3 */
    TOTAL_32    CheckTransFrom;     /* Checks Transferred from, V3.3 */
    TOTAL_32    DeclaredTips;       /* Declared Tips, V3.3 */
    TOTAL_32    aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	//addition for release 2.1 JHHJ
	TAXABLE_32  Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
	TOTAL_32	ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL_32	ttlCoinLoan;			//New loan totals
	TOTAL_32	ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCASHIER_32;

/* -- Report Structure for Read/Write -- */
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulNumber;           /* Number (Dept,PLU,Cashier,Waiter) */
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
    DCURRENCY   lMiscTender[STD_TENDER_MISC];     /* Misc Tender 1 to 8 */
    DCURRENCY   lForeignTotal[STD_NO_FOREIGN_TTL];   /* Foreign Total 1 to 8,    Saratoga */
}TTLCASTENDER;

typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    ULONG       ulCashierNumber;    /* Cashier Number */
    LONG        lCashTender;        /* Cash Tender, Saratoga */
    LONG        lCheckTender;       /* Check Tender */
    LONG        lChargeTender;      /* Charge Tender */
    LONG        lMiscTender[STD_TENDER_MISC];     /* Misc Tender 1 to 8 */
    LONG        lForeignTotal[STD_NO_FOREIGN_TTL];   /* Foreign Total 1 to 8,    Saratoga */
}TTLCASTENDER_32;

/*
*-----------------------------------------------
* Total Function Library / Total Update Module 
*-----------------------------------------------
*/

/* -- Regster Financial File Total -- */
//    Any changes from this point must be synchronized with TTLREGFIN
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;           /* Preiod From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;             /* Preiod To, see also define for TTL_STAT_SIZE */
    D13DIGITS   mlCGGTotal;         /* Current Gross Group Total */
    DCURRENCY   lNetTotal;          /* Net Total,   Release 3.1 */
    TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0, see also STD_TAX_ITEMIZERS_MAX */
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
    TOTAL       ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
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

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status, TTL_STAT_SIZE starts here */
    N_DATE_32   FromDate;           /* Preiod From, see also define for TTL_STAT_SIZE */
    N_DATE_32   ToDate;             /* Preiod To, see also define for TTL_STAT_SIZE */
    D13DIGITS   mlCGGTotal;         /* Current Gross Group Total */
    LONG        lNetTotal;          /* Net Total,   Release 3.1 */
    TAXABLE_32  Taxable[4];         /* Taxable Total 1 to 4, R3.0 */
    D13DIGITS   NonTaxable;         /* Non taxable Total */
    LONG        lTGGTotal;          /* Training Gross Group Total */
    TOTAL_32    DeclaredTips;       /* Declared Tips */
    LONG        lDGGtotal;          /* Daily Gross Group Total */
    TOTAL_32    PlusVoid;           /* Plus Void */
    TOTAL_32    PreselectVoid;      /* Preselect Void */
    TOTAL_32    TransactionReturn;  /* transaction return, CURQUAL_TRETURN */
    TOTAL_32    TransactionExchng;  /* transaction return, CURQUAL_TRETURN */
    TOTAL_32    TransactionRefund;  /* transaction return, CURQUAL_TRETURN */
    TOTAL_32    ConsCoupon;         /* Consolidated Coupon */
    TOTAL_32    ItemDisc;           /* Item Discount */
    TOTAL_32    ModiDisc;           /* Modified Item Discount */
	TTLITEMDISC_32 ItemDiscExtra;      /* Item discounts 3 through 6  */
    TOTAL_32    RegDisc[6];         /* Regular Discount 1 to 6, Release 3.1 */
    TOTAL_32    Coupon;             /* Combination Coupon, R3.0 */
    TOTAL_32    PaidOut;            /* Paid Out */
    TOTAL_32    RecvAcount;         /* Received on Acount */
    TOTAL_32    TipsPaid;           /* Tips Paid Out */
    TOTAL_32    FoodStampCredit;    /* Food Stamp Credit */
    TEND_32     CashTender;         /* Cash Tender          change EPT*/
    TEND_32     CheckTender;        /* Check Tender         change EPT*/
    TEND_32     ChargeTender;       /* Charge Tender        change EPT*/
    TEND_32     MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8   change EPT*/
    TOTAL_32    ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL_32    ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL_32    AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL_32    ChargeTips[3];      /* Charge Tips, V3.3 */
    TOTAL_32    Loan;               /* Loan,    Saratoga */
    TOTAL_32    Pickup;             /* Pickup,  Saratoga */
    TOTAL_32    TransCancel;        /* Transaction Cancel */
    TOTAL_32    MiscVoid;           /* Misc Void */
    TOTAL_32    Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
    SHORT       sNoSaleCount;       /* No Sale Counter */
    LONG        lItemProductivityCount; /* Item Productivity Counter */
    SHORT       sNoOfPerson;        /* Number of Person Counter */
    SHORT       sNoOfChkOpen;       /* Number of Checks Opened */
    SHORT       sNoOfChkClose;      /* Number of Checks Closed */
    SHORT       sCustomerCount;     /* Customer Counter */
    TOTAL_32    HashDepartment;     /* Hash Department */
    TOTAL_32    Bonus[STD_NO_BONUS_TTL];           /* Bonus Total 1 to 8 */
    LONG        lConsTax;           /* Consolidation tax */
    TOTAL_32    aUPCCoupon[3];      /* Coupon (Single/Double/Triple), saratoga */
	TOTAL_32	ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL_32	ttlCoinLoan;			//New loan totals
	TOTAL_32	ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCSREGFIN_32;

/* -- Hourly Activity File Total -- */
typedef struct {
    UCHAR       uchResetStatus;            /* Reset Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;                /* Period From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;                  /* Period To, see also define for TTL_STAT_SIZE */
    HOURLY      Total[STD_MAX_HOURBLK];    /* Block Data of Hourly Activity Time 1 to 48, V3.3 */
}TTLCSHOURLY;

/* ===== New Element - Service Time Total (Release 3.1) BEGIN ===== */
typedef struct {
    UCHAR       uchResetStatus;                   /* Reset Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;                       /* Period From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;                         /* Period To, see also define for TTL_STAT_SIZE */
    TOTAL       ServiceTime[STD_MAX_HOURBLK][3];  /* Block Data of Service Time 1 to 48, V3.3 */
} TTLCSSERTIME;

typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status */
    N_DATE      FromDate;           /* Preiod From */
    N_DATE      ToDate;             /* Preiod To */
} TTLCSRESETSTATUS;

#define     TTL_STAT_SIZE       (sizeof(UCHAR) + (sizeof(N_DATE) * 2))   /* Total status header offset for Report, uchResetStatus + FromDate + ToDate */

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
    UCHAR       uchResetStatus;     /* Reset Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;           /* Period From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;             /* Period To, see also define for TTL_STAT_SIZE */
    LTOTAL      DeptAllTotal;        /* Dept All Total */
}TTLCSDEPTMISC;

typedef struct {
    USHORT      usDeptNo;            /* Dept Number */
}TTLCSDEPTINDX;


#ifdef  __DEL_2172  /* 08.30.15, RJC */
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
    N_DATE        FromDate;           /* Preiod From */
    N_DATE        ToDate;             /* Preiod To */
    LTOTAL      PLUAllTotal;        /* PLU All Total */
}TTLCSPLUMISC;

typedef struct {
    WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN+1];/* PLU Number,  Saratoga */
    UCHAR       uchAject;           /* Adjective Number */
}TTLCSPLUINDX;
#endif

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
    UCHAR       uchResetStatus;     /* Reset Status, see also define for TTL_STAT_SIZE */
    N_DATE        FromDate;           /* Preiod From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;             /* Preiod To, see also define for TTL_STAT_SIZE */
}TTLCSCPNMISC;

/* -- Cashier File Total -- */
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

//  WARNING:  This struct must have the same layout for members as does
//            the struct TTLCASHIER beginning with the member uchResetStatus.
//            There is a dependency on the layout being the same in several
//            places of the totals update.
typedef struct {
    UCHAR       uchResetStatus;     /* Reset Report Status, TTL_STAT_SIZE starts here */
    N_DATE        FromDate;           /* Preiod From, see also define for TTL_STAT_SIZE */
    N_DATE        ToDate;             /* Preiod To, see also define for TTL_STAT_SIZE */
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
    TENDHT      CashTender;         /* Cash Tender */
    TENDHT      CheckTender;        /* Check Tender */
    TENDHT      ChargeTender;       /* Charge Tender */
    TENDHT      MiscTender[STD_TENDER_MAX-3];      /* Misc Tender 1 to 8 */
    TENDHT      ForeignTotal[STD_NO_FOREIGN_TTL];    /* Foreign Total 1 to 8,    Saratoga */
    TOTAL       ServiceTotal[STD_TENDER_MAX-3];    /* Service Total 3 to 8, 10-20 JHHJ */
    TOTAL       AddCheckTotal[3];   /* Add Check Total 1 to 3 */
    TOTAL       ChargeTips[3];      /* Charge Tips */
    TOTAL       Loan;               /* Loan,    Saratoga */
    TOTAL       Pickup;             /* Pickup,  Saratoga */
    TOTAL       TransCancel;        /* Transaction Cancel */
    TOTAL       MiscVoid;           /* Misc Void */
    TOTAL       Audaction;          /* Audaction total or number and amount of tenders of zero or negative amount */
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
	TAXABLE     Taxable[4];         /* Taxable Total 1 to 4, R3.0, see also STD_TAX_ITEMIZERS_MAX */
	D13DIGITS   NonTaxable;         /* Non taxable Total */
	TOTAL		ttlFutureTotals[STD_NUM_FUTURE_TTLS]; /* Future totals for Release 2.1 JHHJ*/
	TOTAL		ttlCoinLoan;			//New loan totals
	TOTAL		ttlCoinAmountDispenced;	//New loan totals
	USHORT		usCoinLowCounter;		//New loan counter
	USHORT		usCoinAmountDispancedCounter;  //New loan counter
	USHORT		usCoinLoanCounter;		//New loan counter
}TTLCSCASTOTAL;

typedef struct {
    ULONG      usReadPointer;      /* Read Pointer */
    ULONG      usWritePointer;     /* Write Pointer */
    ULONG      usCurReadPointer;   /* Current Read Pointer */
    UCHAR       uchTtlFlag0;        /* Total Flag Area */
    UCHAR       uchResetFlag;       /* Master reset Flag */
    ULONG      usSvCurReadPointer;    /* Saved Current Read Pointer */
}TTLCSUPHEAD;


// WARNING:  Any changes to this struct or how these offsets are used
//           should also be reflected in function TtlSavCreateIndexFile()
//           which creates the SavedTotals file and initializes the various
//           offsets in this header for the SavedTotals file.
//           Function TtlWriteSavFile() will perform the actual write of the
//           totals record.
typedef struct { 
	N_DATE  dtCreationDate;							//Date of creation, so that we know when to destroy file
	USHORT usYear;
	ULONG ulFileSize;									//size of the File in bytes.
	ULONG ulIndTerminalOffset[STD_NUM_OF_TERMINALS];	//CLASS_TTLINDFIN, TTLCSREGFIN - offset to each individual terminal totals
	ULONG ulFinancialTotalOffset;						//CLASS_TTLEODRESET, TTLCSREGFIN - offset to total financial totals
	ULONG ulDeptTotalOffset;							//CLASS_TTLDEPT, DEPTTBL * (FLEX_DEPT_MAX + 1) - offset to dept totals, plus one for RPT_EOF record indicating end of list.
	ULONG ulHourlyTotalOffset;							//CLASS_TTLHOURLY, TTLCSHOURLY - offset to hourly totals
	ULONG ulServiceTotalOffset;							//CLASS_TTLSERVICE, TTLCSSERTIME - offset to service totals
	ULONG ulCashierTotalOffset[FLEX_CAS_MAX];	        //CLASS_TTLCASHIER, TTLCASHIER - offset to cashier totals
	ULONG ulCouponTotalOffset;							//CLASS_TTLCPN, CPNTBL * FLEX_CPN_MAX - offset to coupon totals
	ULONG ulPluTotalBeginOffset;						//CLASS_TTLPLU, TTLPLU - offset to PLU totals begins
	ULONG ulPLUTotalCounter;                            //count of number of PLU totals stored beginning at ulPluTotalBeginOffset
}SAVTTLFILEINDEX;


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
SHORT  TtlReinitialTotalFiles (VOID);
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
SHORT  TtlCpnCreate(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);         /* create coupon, total file */
SHORT  TtlCpnDeleteFile(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod);     /* Delete coupon, total file */
SHORT  TtlCasCreate(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod);     /* create cashier total file */
SHORT  TtlCasDeleteFile(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod); /* Delete dept, total file */
SHORT  TtlTtlUpdCreate(USHORT usNoofItem);          /* create Total Update File */
/* ===== New Functions (Release 3.1) BEGIN ===== */
VOID   TtlIndFinCreate(VOID);                       /* create individual financial */
VOID   TtlIndFinCheckDelete(VOID);                  /* Delete individual financial */
SHORT  TtlIndFinDeleteFile(USHORT usTermNum, UCHAR uchMethod);  /* delete individual financial */
SHORT  TtlSerTimeCreate(UCHAR uchMethod);           /* create service time */
SHORT  TtlSerTimeDeleteFile(VOID);                  /* delete service time */
/* ===== New Functions (Release 3.1) END ===== */
SHORT  TtlTotalRead(VOID *pTotal, USHORT *pDataLen); /* read total file */
SHORT  TtlTotalReadNoSemCheck(VOID *pTotal, USHORT *pDataLen);
SHORT  TtlTotalIncrRead(VOID *pTotal);              /* read total file incrementally*/
SHORT  TtlTotalReset(VOID *pTotal, USHORT usMDCBit);/* reset total file */
SHORT  TtlTotalDelete(VOID *pTotal);                /* delete inidividual total data */
SHORT  TtlIssuedSet(VOID *pTotal, UCHAR uchStat);   /* set report issued flag */
SHORT  TtlIssuedReset(VOID *pTotal, UCHAR uchStat); /* set report issued flag */
SHORT  TtlIssuedCheck(VOID *pTotal, UCHAR uchStat); /* check issued flag's status */
USHORT TtlIssuedCheckAllReset(UCHAR uchMinorClass); /* audit type check on various totals' issued flag status */
SHORT  TtlLock(VOID);                               /* file access lock */
VOID   TtlUnlock(VOID);                             /* unlock file access */
SHORT  TtlUpdateFile(VOID *pTotal, ULONG ulLength);/* update total file */
SHORT  TtlTotalCheck(VOID *pTotal);                 /* Check Total */
SHORT  TtlReqBackup(VOID);                          /* receive total file */
SHORT  TtlRespBackup(UCHAR *puchReqBuf, USHORT usSize, UCHAR *puchRcvBuf, USHORT *pusRcvLen); /* send response data */
SHORT  TtlReadWrtChk(VOID);                         /* check read&write point in total update file */

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   TtlConvertError_Debug() is ENABLED    ====++++====  \z")
#define TtlConvertError(sError) TtlConvertError_Debug(sError, __FILE__, __LINE__)
USHORT TtlConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
USHORT TtlConvertError(SHORT sError);               /* Convert error code  */
#endif
SHORT  TtlStringCheck(VOID *pStrBuff, USHORT usLength, UCHAR uchCmpChar);
SHORT  TtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, ULONG ulLength, UCHAR *puchDelayBalance);/* update total file */
SHORT TtlReadFileFH(ULONG ulOffset, VOID FAR *pData,       
                     ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead);    /* read data from interfaced file handle */

VOID   TtlTtlUpdDeleteFile(VOID);                   /* Delete Total Update File */
SHORT  TtlWaiTotalCheck(VOID);                      /* Check Waiter Total   */ 
SHORT  TtlCasTotalCheck(VOID);                      /* Check Cashier Total  */
SHORT   TtlTenderAmountRead(TTLCASTENDER *pTender);    /* Cashier Tender Read, saratoga */

SHORT TtlPLUOptimize(UCHAR uchMinorClass);

ULONG  TtlWriteSavFile(UCHAR uchClass, VOID *puchTmpBuff, ULONG ulFileSize, USHORT usNumber);
VOID   TtlCloseSavFile(VOID);
SHORT  TtlSavCpnSavFile(UCHAR uchMinorClass, UCHAR uchType);
SHORT  TtlSavCpnSavFileBuff(TOTAL *pTotal, USHORT usMaxCpn);

#endif      /* CSTTL_INCLUDED */

/****** end of file ******/
