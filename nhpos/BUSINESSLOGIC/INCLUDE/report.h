/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : report.h                                          :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments
:   Program Written    : 92-05-07 : 00.00.00  :
:   Update Histories   :          :           :
:                      : 99-08-11 : 03.05.00  : Remove WAITER_MODEL
: 
:** NCR2172 ** 
: 
:                      : 99-10-05 : 01.00.00  : initial (for Win32)
:** GenPOS Rel 2.2     :
:                      : 15-02-23 : 02.02.01  : corrected naming PRT_RANGE_RAED to RPT_RANGE_READ
:                      : 15-02-23 : 02.02.01  : condensed text, eliminating blank lines, move comments to end of line
:                      : 15-02-23 : 02.02.01  : removed L on constants to eliminate need for casts such as (UCHAR)
*===========================================================================
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
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/

#include <csttl.h>
#include <csop.h>
#include <uie.h>

extern    UCHAR   uchRptOpeCount;

/**
==============================================================================
;                  R A M   A R E A   D E C L A R A T I O Ns                         
=============================================================================
**/


/*------------------------------------------------------------------------*\

            E Q U A T I O N     D E F I N I T I O N 

\*------------------------------------------------------------------------*/


/* Minor Data Class */

#define CLASS_RPTREGFIN_PRTTTLCNT   1           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTTIME     2           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTDGG      3           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTCGG      4           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTBONUS    5           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTNO       6           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTCNT      7           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTFCP0     8           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTFCP2     9           /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTFCP3     10          /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_PRTPITEM    11          /* Print format for Reg. Fin. */
#define CLASS_RPTREGFIN_MNEMO       12          /* Print format for Reg. Fin. */

#define CLASS_RPTGUEST_PRTCHKNOWCD  13          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTCHKNOWOCD 14          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTCHKOPEN   15          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTPERSON    16          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTTBL       17          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTTIME      18          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTTTL       19          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTMNEMO     20          /* Print format for GCF */
#define CLASS_RPTGUEST_PRTDGG       21          /* Print format for GCF */

#define CLASS_RPTDEPT_PRTTIME       22          /* Print format for DEPT file */
#define CLASS_RPTDEPT_PRTMAJOR      23          /* Print format for DEPT file */
#define CLASS_RPTDEPT_PRTTTLCNT1    24          /* Print format for DEPT file */
#define CLASS_RPTDEPT_PRTTTLCNT2    25          /* Print format for DEPT file */
#define CLASS_RPTDEPT_PRTTTLCNT3    26          /* Print format for DEPT file */
#define CLASS_RPTDEPT_PRTMDEPTTTL   27          /* Print format for MDEPT file */

#define CLASS_RPTPLU_PRTDEPT            28      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLOPEN         29      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLPRSET        30      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLADJ1         31      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLCREDIT       32      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLCOUPON       33      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLMINUSADJ1    34      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLCNT          35      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTCODE            36      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLADJ2         37      /* Print format for PLU file */
#define CLASS_RPTPLU_PRTTTLMINUSADJ2    38      /* Print format for PLU file */

#define CLASS_RPTHOURLY_PRTTIME         39      /* Print Format for Hourly Activity File */
#define CLASS_RPTHOURLY_PRTASTER        40      /* Print Format for Hourly Activity File */

#define CLASS_RPTEJ_PAGE                41      /* Print format for EJ File */
#define CLASS_RPTEJ_LINE                42      /* Print format for EJ File */
#define CLASS_RPTEJ_REVERSE             43      /* Print format for EJ File */

#define CLASS_RPTWAITER_READ            44      /* print format for waiter */
#define CLASS_RPTWAITER_RESET           45      /* print format for waiter */

#define CLASS_RPTCASHIER_READ           46      /* print format for cashier */
#define CLASS_RPTCASHIER_RESET          47      /* print format for cashier */
#define CLASS_RPTCASHIER_TERMINAL       56      /* print format for terminal number */

#define CLASS_RPTEMPLOYEE_READ          48      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_RESET         49      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTMNEMO      50      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTTIME       51      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTNOIN       52      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTNOOUT      53      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTTOTAL      54      /* print format for all etk report */
#define CLASS_RPTEMPLOYEE_PRTNOTOTAL    55      /* print format for all etk report */
#define CLASS_RPTEMPLOYEE_NUMBER        56      /* print format for etk report */
#define CLASS_RPTEMPLOYEE_PRTWKTTL      57      /* print format for 1 etk total */
#define CLASS_RPTEMPLOYEE_PRTWKNOTTL    58      /* print format for 1 etk total */

#define CLASS_RPTCPN_ITEM               59      /* item print for coupon item print */
#define CLASS_RPTCPN_TOTAL              60      /* total print for total for coupon print */

#define CLASS_RPTSERVICE_TTL            61      /* Print Format for Service Time File */
#define CLASS_RPTSERVICE_SEGTTL         62      /* Print Format for Service Time File */

#define CLASS_RPTHOURLY_PERCENT         63      /* Print Format for Hourly Activity File R3.1 */

#define CLASS_RPTEJ_CONFIGURATION		64		/* Print Format for Configuration Information JHHJ 10-29-04*/
#define CLASS_RPTGUEST_PRTCHKNOWOCD_PREAUTH 65	/* Print Format for Pre-Authorization */

#define CLASS_RPTGUEST_FILEINFO         66      // Print format for GCF to print GCF info
#define CLASS_RPTGUEST_FILE_OPTIMIZED	67		// Print format for GCF to print Mnemonic.

//GroupReport - CSMALL
#define CLASS_RPTPLUGROUP_PRTGRP		68		// Print format for PLU Group Report - group line
#define CLASS_RPTPLUGROUP_PRTPLU		69		// Print format for PLU Group Report - plu line

#define CLASS_TTLHOURLY_STSTIME         1       /* Indicate Hourly Start Time */
#define CLASS_TTLHOURLY_ENDTIME         2       /* Indicate Hourly End Time */


/* Data Type Definition (from UI) */

#define RPT_TYPE_DALY           1              /* Current Daily Type File */
#define RPT_TYPE_PTD            2              /* Current PTD Type File */
#define RPT_TYPE_DALYSAVE       3              /* Previous or Saved Daily Type File */
#define RPT_TYPE_PTDSAVE        4              /* Previous or Saved PTD Type File */

/* Report Type Definition (from UI) */

#define RPT_TYPE_ALL            1              /* All Read Type */
#define RPT_TYPE_IND            2              /* Individual Read Type */
#define RPT_TYPE_WAIIND         3              /* Individual Read by Waiter Type (G/C File) */
#define RPT_TYPE_GCF_RESET      4              /* Reset the Guest Check File (G/C File) */

#define RPT_TYPE_HOURANGE       2              /* Range Read (Hourly Activity File) */

#define RPT_TYPE_MDEPTIND       2              /* Individual Read by Major DEPT Type (DEPT File) */
#define RPT_TYPE_INDDEPT        3              /* Individual Read Type (DEPT File) */

#define RPT_TYPE_DEPTIND        2              /* Individual Read by DEPT Type (PLU File) */
#define RPT_TYPE_RPTCODEIND     3              /* Individual Read by Report Code Type (PLU File) */
#define RPT_TYPE_INDPLU         4              /* Individual Read Type (PLU File) */

/* Function Type Definition (from UI) */

#define RPT_TYPE_ONLY_PRT       1              /* Only Print Type */
#define RPT_TYPE_RST_PRT        2              /* Reset and Print Type */
#define RPT_TYPE_ONLY_RST       3              /* Only Reset Type */

#define RPT_TYPE_DELETE         4              /* delete database */
#define RPT_TYPE_OPTIMIZE       5              /* optimize database */

/* Function Type for CPM/EPT Tally */

#define RPT_TYPE_CPM_PRT        1              /* ONLY CPM TALLY REPORT */
#define RPT_TYPE_CPM_RST        2              /* RESET AND REPORT CPM TALLY */
#define RPT_TYPE_EPT_PRT        3              /* ONLY EPT TALLY REPORT */
#define RPT_TYPE_EPT_RST        4              /* RESET AND REPORT CPM TALLY */

#define RPT_TYPE_DELETE			4				//DELETE SAVED TRANSACTIONS STORE AND FORWARD

/* Function Type Definition */
/* Read Type */
#define RPT_ALL_READ            1      /* All Read Type (A/C 2,21,24,26,29,41,150) */

#define RPT_IND_READ            2      /* Individual Read Type (A/C 2,21,24,26,29,41.150) */
#define RPT_RANGE_READ          2      /* Hourly Activity Report Read By Range Type ( A/C 24) */
#define RPT_MDEPT_READ          2      /* Department Report Read By Major Department Type ( A/C 26) */

#define RPT_DEPTIND_READ        3      /* Department Individual Read Type (A/C 26) */
#define RPT_ALL_MDEPT_READ      4      /* Major Department Report Type ( A/C 122) */

#define RPT_MEDIA_READ          2      /* Media Flash Report Type ( A/C 123) */

/* PLU Report Type (A/C 29) */
#define RPT_DEPT_READ           2               /* PLU Report Read By Department Type */
#define RPT_CODE_READ           3               /* PLU Report Read By Report Code Type */
#define RPT_PLUIND_READ         4               /* PLU Individual Read Type */

/* Guest Check Report By Waiter Type (A/C 2) */
#define RPT_WAITER_READ         3

/* EOD/PTD Read Report Type (A/C 18,99) */
#define RPT_EOD_ALLREAD         5              /* All Read Type of EOD */
#define RPT_PTD_ALLREAD         6              /* All Read Type of PTD */

#define RPT_FIN_READ            7              /* Read Report Finalize Type */

//Store and Forward
#define RPT_SUMMARY				1
#define RPT_DETAILED			2
#define RPT_FAILED				3

/* Reset Type */

#define RPT_ALL_RESET           1      /* All Reset Type (A/C 2,21,41) */
#define RPT_IND_RESET           2      /* Individual Reset Type (A/C 2,21,41) */

#define RPT_READ_REPORT         0               /* read report */
#define RPT_ONLY_PRT_RESET      1               /* Only Print function Type */
#define RPT_PRT_RESET           2               /* Print and Reset function Type */
#define RPT_ONLY_RESET          3               /* Only Reset function Type */
#define RPT_DELETE              4               /* delete function Type */

#define RPT_WAI_RESET           3      /* Reset by Waiter Type (G/C File) */
                                       /* EOD/PTD Reset Report Type (A/C 18,99) */
#define RPT_EOD_ALLRESET        4               /* All Reset Type of EOD */
#define RPT_PTD_ALLRESET        5               /* All Reset Type of PTD */
#define RPT_OPEN_RESET          6               /* Reset Open Status Flag Type */

/* Lock Type */
#define RPT_IND_LOCK            1               /* Individual Lock */
#define RPT_ALL_LOCK            2               /* All Lock */
#define RPT_WAI_LOCK            3               /* Individual Lock G/C File by Waiter */

/* UnLock Type */
#define RPT_IND_UNLOCK          1               /* Individual UnLock */
#define RPT_ALL_UNLOCK          2               /* All UnLock */
#define RPT_WAI_UNLOCK          3               /* Individual Unlock G/C File by Waiter */


/* Return Status */
#define RPT_ABORTED             -1              /* Aborted By User */
#define RPT_EXE_RESET           -2              /* Executed Reset Report */
#define RPT_RESET_FAIL          -3              /* All Reset Incompleted in EOD/PTD */
#define RPT_END                 -4              /* Reported */

#define RPT_FILE_NOTEXIST        0              /* File Not Exist */
#define RPT_FILE_EXIST           1              /* File Exist */

#define RPT_PAUSED              -5              /* Pause (for LCD) */

/* Defalt feed */
#define RPT_DEFALTFEED          1               /* Defalt feed line */

/* DEPT PLU Status */
#define  RPT_OPEN               1               /* Status of OPEN */
#define  RPT_CREDIT             2               /* Status of CREDIT */
#define  RPT_HASH               3               /* Status of HASH */
#define  RPT_EOF                0xff            /* End of file in the Table */

#define  RPT_HEADER_PRINT       0x01            /* Header print */
#define  RPT_OVERFLOW           65000           /* Over flow LONG Data for percentage calculations (represent 99.99% or 9999 up to 649%) */

#define  RPT_HOUR_STSTIME       1               /* Indicate of Enter Start Time of Hourly Report */
#define  RPT_HOUR_ENDTIME       2               /* Indicate of Enter End Time of Hourly Report */

#define  RPT_GCF_READ           1               /* GCF Read Report Type  */
#define  RPT_GCF_RESET          2               /* GCF Reset Report Type  */

/* Decimal Status */
#define  RPT_DECIMAL_0          0               /* Decimal Point Nothing */
#define  RPT_DECIMAL_1          1               /* Decimal Point 1 Position */
#define  RPT_DECIMAL_2          2               /* Decimal Point 2 Position */

/* Define for EJ Report */
#define  RPT_EJ_READ            1               /* Read EJ File */
#define  RPT_EJ_RESET           2               /* Reset EJ File */
#define  RPT_EJ_CLUSTER_RESET   10               /* Reset EJ File by Cluster R3.1 */

/* Define For EOD/PTD Reset Control */
#define RPT_RESET_REGFIN_BIT    ~0x0001     /* reset consolidation file */
#define RPT_RESET_DEPT_BIT      ~0x0002     /* reset department file */
#define RPT_RESET_PLU_BIT       ~0x0004     /* reset plu file */
#define RPT_RESET_HOURLY_BIT    ~0x0008     /* reset hourly file */
#define RPT_RESET_COUPON_BIT    ~0x0080     /* reset combination coupon file */
#define RPT_RESET_INDFIN_BIT    ~0x0100     /* reset individual file */
#define RPT_RESET_SERVICE_BIT   ~0x0200     /* reset service time file */

/* Define for Programable Report Column */
#define RPT_PROGRAMABLE_COLUMN  42          /* programable report column */

/* Define For Report Pause */
#define  CID_RPTPKEYCHECK       1               /* Pause Proccess Dialog ID */

/*------------------------------------------------------------------------*\

            T Y P E D E F     S T R U C T U R Es

\*------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/* Register Financial Report Print Data Structure */

    typedef struct  {
        UCHAR   uchMajorClass;                          /* Major Class Data Ddefinition */
        UCHAR   uchMinorClass;                          /* Minor Class Data Ddefinition */
        TCHAR   aszTransMnemo[PARA_TRANSMNEMO_LEN+1];   /* Transaction Mnemonics */
        N_DATE    Date;                                   /* Preiod Date */
        D13DIGITS Total13D;                             /* 13 Digit Total */
        TOTAL   Total;                                  /* Total and Counter Data */
        LONG    lCounter;                               /* Counter for Item Productivity Cnt. */
        USHORT  usPrintControl;                         /* Print Control */
    }RPTREGFIN;

/* Hourly Activity Report Print Data Structure */

     typedef struct {
        UCHAR   uchMajorClass;                          /* Major Class Data Ddefinition */
        UCHAR   uchMinorClass;                          /* Minor Class Data Ddefinition */
        TCHAR   aszTransMnemo[PARA_TRANSMNEMO_LEN+1];   /* Transaction Mnemonics */
        N_TIME    StartTime;                              /* Start Time */
        TOTAL   Total;                                  /* Block Data of Hourly Activity Time */
        UCHAR   uchStatus;                              /* W/ W/o Status Area */
        USHORT  usPrintControl;                         /* Print Control */
    }RPTHOURLY;


/* Departmet Report Print Data Structure */

     typedef struct {
        UCHAR   uchMajorClass;                          /* Major Class Data Ddefinition */
        UCHAR   uchMinorClass;                          /* Minor Class Data Ddefinition */
        TCHAR   aszMnemo[PARA_DEPT_LEN+1];              /* Transaction,DEPT,Major DEPT Mnemonics */
        N_DATE    Date;                                   /* Preiod Date */
        USHORT  usDEPTNumber;                           /* Department Number,   Saratoga */
        UCHAR   uchMajorDEPTNo;                         /* Major DEPT Number */
        LTOTAL  DEPTTotal;                              /* Department Total */
        USHORT  usPercent;                              /* Percent. when printing with reports using %6.2l$%% format, cast to DCURRENCY */
        USHORT  usPrintControl;                         /* Print Control */
    }RPTDEPT;


/* PLU Report Print Data Structure */

     typedef struct {
        UCHAR  uchMajorClass;                           /* Major Class Data Definition */
        UCHAR  uchMinorClass;                           /* Minor Class Data Definition */
        TCHAR  auchPLUNumber[PLU_MAX_DIGIT];            /* PLU Number,      Saratoga */
        USHORT usDEPTNumber;                            /* DEPT Number,     Saratoga */
        TCHAR  aszMnemo[PARA_PLU_LEN+1];                /* Transaction,DEPT,PLU Mnemonics */
        TCHAR  aszSpeMnemo[PARA_SPEMNEMO_LEN+1];        /* Special Mnemonic (LB) */
        TCHAR  aszAdjMnemo[PARA_ADJMNEMO_LEN+1];        /* Adjective Mnemonics */
        USHORT usReportCode;                            /* Report/Dept Code,    Saratoga */
		DCURRENCY  lPresetPrice;                        /* Preset Price */
        N_DATE   Date;                                    /* Preiod Date */
        LTOTAL PLUTotal;                                /* PLU Total */
        USHORT usPercent;                               /* Percent. when printing with reports using %6.2l$%% format, cast to DCURRENCY */
        UCHAR  uchStatus;                               /* Status for Decimal Point */
        USHORT usPrintControl;                          /* Print Control */
    }RPTPLU;


/* Cashier Report Print Data Structure */

   typedef struct  {
       UCHAR  uchMajorClass;                            /* Major Class Data Ddefinition */
       UCHAR  uchMinorClass;                            /* Minor Class Data Ddefinition */
       TCHAR  aszTransMnemo[PARA_TRANSMNEMO_LEN+1];     /* Transaction Mnemonics */
       ULONG  ulCashierNumber;                          /* Cashier Number */
       TCHAR  aszCashMnemo[PARA_CASHIER_LEN+1];         /* Cashier Mnemonics */
       N_DATE   Date;                                     /* Preiod Date */
       D13DIGITS Total13D;                              /* 13 Digit Total */
       TOTAL  Total;                                    /* Total and Counter Data */
       USHORT usPrintControl;                           /* Print Control */
     } RPTCASHIER;

   typedef struct  {
      UCHAR     uchMajorClass;                      /* Major Class Data Ddefinition */
      UCHAR     uchMinorClass;                      /* Minor Class Data Ddefinition */
      TCHAR     aszTimeinMnemo[PARA_TRANSMNEMO_LEN+1]; /* Transaction Mnemonics */
      TCHAR     aszTimeoutMnemo[PARA_TRANSMNEMO_LEN+1]; /* Transaction Mnemonics */
      ULONG     ulEmployeeNumber;                   /* Employee Number */
      TCHAR     aszEmpMnemo[PARA_LEADTHRU_LEN+1];   /* Lead Through */
      N_DATE      Date;                               /* Preiod Date */
      USHORT    usMonth;                            /* Month to be time in */
      USHORT    usDay;                              /* Day to be time in */
      UCHAR     uchJobCode;                         /* Job Code to be time in */
      USHORT    usTimeInTime;                       /* Time to be time in */
      USHORT    usTimeInMinute;                     /* Minute to be time in */
      USHORT    usTimeOutTime;                      /* Time to be time out */
      USHORT    usTimeOutMinute;                    /* Minute to be time out */
      USHORT    usWorkTime;                         /* Time to be work time */
      USHORT    usWorkMinute;                       /* Minute to be work time */
      ULONG     ulWorkWage;                         /* Wage of work time R3.1 */
      USHORT    usTotalTime;                        /* Total Time */
      USHORT    usTotalMinute;                      /* Total Minute */
      ULONG     ulTotalWage;                        /* Total Wage R3.1 */
      USHORT    usPrintControl;                     /* Print Control */
} RPTEMPLOYEE;

   typedef struct  {
      UCHAR     uchMajorClass;                      /* Major Class Data Ddefinition */
      UCHAR     uchMinorClass;                      /* Minor Class Data Ddefinition */
      ULONG     ulEmployeeNumber;                   /* Employee Number */
      TCHAR     aszEmpMnemo[PARA_LEADTHRU_LEN+1];   /* Lead Through */
      USHORT    usPrintControl;                     /* Print Control */
} RPTEMPLOYEENO;

/* Guest Check Report Print Data Structure */

    typedef struct  {
      UCHAR       uchMajorClass;                            /* Major Class Data Ddefinition */
      UCHAR       uchMinorClass;                            /* Minor Class Data Ddefinition */
      TCHAR       aszMnemo[PARA_TRANSMNEMO_LEN+1];          /* Transaction Mnemonics */
      TCHAR       aszSpeMnemo[PARA_SPEMNEMO_LEN+1];
      ULONG       ulCashierId;                              /* Cashier/Waiter/Pre Waiter ID */
      USHORT      usGuestNo;                                /* Guest Check Number */
      UCHAR       uchCdv;                                   /* Guest check number Check Digit, MDC 12 Bit C */
      UCHAR       auchCheckOpen[2];                         /* Time check open hh:mm */
      USHORT      usNoPerson;                               /* Number of Person/Table Number */
      DCURRENCY   lCurBalance;                              /* Current Balance/Transferred Balance */
      USHORT      usGCFStatus;                              /* GCF Status */
      UCHAR       uchStatus;                                /* Control Status */
      USHORT      usPrintControl;                           /* Print Control */
    }RPTGUEST;

/* Electoric Journal Print Data Structure */

    typedef struct  {
      UCHAR   uchMajorClass;                            /* Major Class Data Ddefinition */
      UCHAR   uchMinorClass;                            /* Minor Class Data Ddefinition */
      USHORT  usPageNo;                                 /* Page No. */
      USHORT  usTtlPageNo;                              /* Total Page No. */
      TCHAR   aszLineData[EJ_REPORT_WIDE + 1];          /* 1 Line Data */
      USHORT  usPrintControl;                           /* Print Control */
    }RPTEJ;

/* Coupon Report Print Data Structure */

     typedef struct {
        UCHAR   uchMajorClass;                           /* Major Class Data Ddefinition */
        UCHAR   uchMinorClass;                           /* Minor Class Data Ddefinition */
        TCHAR   aszMnemo[PARA_CPN_LEN+1];                /* Transaction,DEPT,Major DEPT Mnemonics */
        USHORT  usCpnNumber;                             /* Coupon Number */
        LTOTAL  CpnTotal;                                /* Department Total */
        USHORT  usPrintControl;                          /* Print Control */
    }RPTCPN;

/* Service Time Report Print Data Structure */

     typedef struct {
        UCHAR   uchMajorClass;                          /* Major Class Data Ddefinition */
        UCHAR   uchMinorClass;                          /* Minor Class Data Ddefinition */
        SHORT   sElement;                               /* Element Number 1 - 3 */
		DSCOUNTER  sCounter;                            /* Counter Total */
        LONG    lMinute;                                /* Service Time : Minute */
        CHAR    chSecond;                               /* Service Time : Second */
        UCHAR   uchStatus;                              /* W/ W/o Status Area */
        USHORT  usPrintControl;                         /* Print Control */
    }RPTSERVICE;

/* Programable Report Print Data Structure */

    typedef struct  {
      UCHAR   uchMajorClass;                            /* Major Class Data Ddefinition */
      UCHAR   uchMinorClass;                            /* Minor Class Data Ddefinition */
      TCHAR   aszLineData[RPT_PROGRAMABLE_COLUMN + 1];  /* 1 Line Data */
      USHORT  usPrintControl;                           /* Print Control */
    }RPTPROGRAM;


/* Job code summary Print Data Structure */
	typedef struct {
		UCHAR     uchMajorClass;         /* Major Class Data Definition */
		UCHAR     uchMinorClass;         /* Minor Class Data Definition */
		USHORT    usTotalTime;           /* Total Time */
		USHORT    usTotalMinute;         /* Total Minute */
		USHORT    usLaborCost;
		USHORT    usPrintControl;        /* Print Control */
		ULONG     ulTotalWage;           /* Total Wage R3.1 */
		UCHAR	  uchJobCode;            /* Job Code to be time in */
	} RPTJCSUMMARY, *PRPTJCSUMMARY;



#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

// RPTREGFIN_OUTPUT_NONE and RPTREGFIN_FOLDER_NONE in these enums must be 0 to allow initializing struct containing
// this enum to zero to indicate no output type set.
typedef enum { RPTREGFIN_OUTPUT_NONE = 0, RPTREGFIN_OUTPUT_CSV = 1, RPTREGFIN_OUTPUT_XML, RPTREGFIN_OUTPUT_HTML, RPTREGFIN_OUTPUT_HUBWORKS } RptElementOutputType;
typedef enum { RPTREGFIN_FOLDER_NONE = 0, RPTREGFIN_FOLDER_ANY = 1, RPTREGFIN_FOLDER_HIST, RPTREGFIN_FOLDER_WEB, RPTREGFIN_FOLDER_PRINT, RPTREGFIN_FOLDER_TEMP, RPTREGFIN_FOLDER_QUERY} RptElementOutputFolder;
typedef VOID  (*RptElementFunc) ( USHORT uchTransAddr, TOTAL *pTtlData, D13DIGITS Amount13, DCURRENCY  lAmount, UCHAR uchMinor, UCHAR uchRate);
typedef enum { RPTFLAGS_LOG_NONE = 0, RPTFLAGS_LOG_01 = 0x0001 } RptElementFlags;

typedef struct {
		UCHAR    uchMinor;      // minor class such as CLASS_RPTREGFIN_PRTTTLCNT used with RptElement()
		wchar_t  *aszFormat;    // the specific format to be used with this minor class for the otuput.
} RptElementOutputClassType;

typedef struct {
	RptElementOutputType       usType;          // the type of report output such as CSV, HTML, XML, etc.
    RptElementOutputFolder     usFolder;        // the folder indicator for where the output file should go
	RptElementOutputClassType  *outputClass;    // pointer to the report array mapping uchMinorClass to format specifier.
	USHORT                     usFmtCount;
} RptElementOutput;

typedef struct {
	FILE     *fpOut;                    // the output stream to be used with stdio.h output
    RptElementFlags     uFlags;         // flags and indicators of various states
	RptElementFunc      pOutputFunc;    // pointer to the output function to use
	RptElementOutput    rptDescrip;     // indicates the type of output such as XML, HTML, CSV, etc.
	USHORT   usPGACNo;                  // indicates the Action Code such as AC99, AC23, etc.
	UCHAR    uchMinorClass;             // indicates the type of totals such as CLASS_TTLSAVDAY
	UCHAR    uchType;                   // indicates the type of report such as RPT_EOD_ALLREAD
    UCHAR    uchUifACRptOnOffMldSave;   // save area for current uchUifACRptOnOffMld when created.
} RptDescription;

/*------------------------------------------------------------------------*\

            D E F I N E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*\

             P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/***** COMMON REPORT MODULE *****/

extern  FILE *fpRptElementStreamFile;    // set before using RptElementStream()

RptDescription RptDescriptionCreate (USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, RptElementOutputFolder tFolder, RptElementOutputType tOutputType, RptElementFunc pOutputFunc);
RptDescription RptDescriptionCreateMini(FILE* fpFile, RptElementOutputFolder tFolder, RptElementOutputType tOutputType, RptElementFunc pOutputFunc);
RptDescription RptDescriptionSet (RptDescription  rptDescrip);
RptElementFlags RptDescriptionSetFlag(RptElementFlags  uFlag);
int RptDescriptionCheckFlag(RptElementFlags  uFlag);
int RptDescriptionCheckType(RptElementOutputType tOutputType);
RptElementOutputType RptDescriptionGetType(void);
RptDescription RptDescriptionGet (VOID);
FILE* RptDescriptionGetStream(VOID);
RptDescription RptDescriptionSwap (RptDescription  rptDescrip);
RptDescription RptDescriptionClear (VOID);
RptDescription RptDescriptionClose(VOID);

FILE * ItemOpenHistorialReportsFolder(RptElementOutputFolder fOutputFolder, USHORT usPGACNo, UCHAR uchMinorClass, UCHAR uchType, SHORT  iYear, SHORT  iMonth, SHORT  iDay);
SHORT  ItemCloseHistorialReportsFolder (FILE *fpFile);

VOID    RptInitialize( VOID );
VOID    RptPrtStatusSet( UCHAR uchMinorClass );
VOID    RptPrtTime( USHORT uchTransAddr, N_DATE *pDate);
VOID    RptElementStream (USHORT uchTransAddr, TOTAL *pTtlData, D13DIGITS Amount13, DCURRENCY lAmount, UCHAR uchMinor, UCHAR uchBonusRate);
VOID    RptElementStream2(USHORT uchTransAddr, USHORT usId, wchar_t* aszMnemo, TOTAL* pTtlData, D13DIGITS Amount13, DCURRENCY lAmount, LONG lCounter, UCHAR uchMinor, UCHAR uchBonusRate);
VOID    RptElementHubworks (USHORT uchTransAddr, TOTAL *pTtlData, D13DIGITS Amount13, DCURRENCY lAmount, UCHAR uchMinor, UCHAR uchBonusRate);
VOID    RptElementStandardXml (USHORT uchTransAddr, TOTAL *pTtlData, D13DIGITS Amount13, DCURRENCY lAmount, UCHAR uchMinor, UCHAR uchBonusRate);
VOID    RptElement( USHORT uchTransAddr, TOTAL *pTtlData, D13DIGITS Amount13, DCURRENCY  lAmount, UCHAR uchMinor, UCHAR uchRate);
VOID    RptFeed( SHORT DefaltFeed);
VOID	RptFeedPrintNormal(SHORT DefaltFeed, USHORT *usLineCount);
SHORT   RptNoAssume(UCHAR uchNo);
VOID    RptSvscalc( UCHAR uchNo, TOTAL *pTtlData, TOTAL *Totalcount );
UCHAR   RptFCMDCChk(USHORT usMDC_Adr);      /* Saratoga */

SHORT RptPauseCheck(VOID);
SHORT RptPkeyCheck(KEYMSG *pData);
SHORT RptCheckReportOnMld(VOID);
SHORT RptCheckReportOnStream(VOID);

SHORT RptTerminalConnectStatus(USHORT (*RptSampleConfigurationLine) (TCHAR *tsLineData));
SHORT RptTerminalConfiguration(USHORT (*RptSampleConfigurationLine) (TCHAR *tsLineData));

VOID  RptVatCalc2(D13DIGITS *pd13VatTtl,D13DIGITS *pd13VatApplTtl,UCHAR uchTaxNo);

VOID   RptFCRateCalc2(UCHAR uchAddress, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);

/***** FINANCIAL REPORT *****/

SHORT   RptRegFinDayRead( UCHAR uchMinor, UCHAR uchType );
SHORT   RptRegFinPTDRead( UCHAR uchMinor, UCHAR uchType );
SHORT   RptRegFinRead( UCHAR uchMinor, UCHAR uchType);
SHORT   RptRegFinEdit( RptElementFunc func, TTLREGFIN *pTtlData, UCHAR uchType, UCHAR uchIndAll);

VOID RptRegFinPrtTermNumber(USHORT usTermNo, UCHAR uchType);

SHORT RptRegFinVAT(TTLREGFIN *pTtlData);

SHORT  ItemGenerateAc23Report (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, USHORT usTerminalNo);

/***** INDIVIDUAL FINANCIAL REPORT *****/

SHORT RptIndFinDayRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usTermNumber, UCHAR uchResetType);
SHORT RptIndFinRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usTermNo, UCHAR uchResetType);
SHORT RptIndFinDayReset(UCHAR uchType, UCHAR uchFunc, USHORT usTermNumber);
SHORT RptIndFinIndReset(UCHAR uchFunc, USHORT usTermNumber);
SHORT RptIndFinAllReset(UCHAR uchType, UCHAR uchFunc);
SHORT  RptIndFinAllOnlyReset(UCHAR uchType, UCHAR uchFunc, SHORT sNo);
SHORT  RptIndFinAllRstReport(UCHAR  uchType, UCHAR  uchFunc, SHORT  sNo);
SHORT RptIndFinLock(UCHAR uchType, USHORT usTermNumber);
VOID RptIndFinUnLock(VOID);

/***** HOURLY REPORT *****/

SHORT   RptHourlyDaySet( RPTHOURLY *pData );
SHORT   RptHourlyDayRead( UCHAR uchMinorClass, UCHAR uchType );
SHORT   RptHourlyPTDRead( UCHAR uchMinorClass, UCHAR uchType );
SHORT   RptHourlyAllRead( UCHAR uchMinorClass, UCHAR uchType );
VOID    RptHourlyElement( UCHAR      uchMinorClass,
                          USHORT     uchTransAddr,
                          USHORT     usMin,
                          USHORT     usHour,
                          LONG       lCounter,
                          DCURRENCY  lTotal );

/***** DEPT REPORT,     Saratoga (Dept# 4 Digits ) *****/

SHORT   RptDEPTRead( UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber);

/***** PLU REPORT *****/
SHORT RptPLUDayRead( UCHAR  uchMinorClass, UCHAR  uchType, USHORT usNumber, UCHAR  uchResetType, TCHAR  auchPLUNumber[PLU_MAX_DIGIT+1]);    /* Saratoga */
SHORT RptPLUPTDRead( UCHAR  uchMinorClass, UCHAR  uchType, USHORT usNumber, UCHAR  uchResetType, TCHAR  auchPLUNumber[PLU_MAX_DIGIT+1]);    /* Saratoga */
SHORT RptPLURead( UCHAR  uchMinorClass, UCHAR  uchType, USHORT usNumber, UCHAR  uchResetType, TCHAR  *puchPLUNumber);                       /* Saratoga */
SHORT RptPLUEditInd( UCHAR  uchMinorClass, UCHAR  uchType, TCHAR  *puchPLUNumber);                    /* Saratoaga */
SHORT RptPLUEditCode( UCHAR uchMinorClass, UCHAR uchType, UCHAR uchCodeNumber);
SHORT RptPLUCode( UCHAR  uchMinorClass, UCHAR  uchType, USHORT usStatus, UCHAR  uchCodeNo, LTOTAL *pTotal, UCHAR  *puchFlag);
VOID  RptCodeNamePrt(USHORT usCodeNo, UCHAR uchType);
SHORT RptPLUEditByDEPT( UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber, UCHAR uchResetType);
/*SHORT   RptPLUByPLU(UCHAR uchMinorClass, UCHAR uchType, LONG ulNo,
                    UCHAR *puchFlag, UCHAR uchResetType);  */ /* Saratoga */
/* Add 2172 Rel1.0 */
SHORT   RptPLUByPLU(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchResetType,USHORT usStatus,UCHAR* puchFlg, DCURRENCY * lSubTotal, LONG* lSubCounter,UCHAR* uchScaleOut,PLUIF* pPluIf);				   /* Saratoga */
/* Add 2172 Rel1.0 */
SHORT   RptPLUByACTPLU(UCHAR uchMinorClass, UCHAR uchType,UCHAR uchResetType, USHORT usStatus,UCHAR* puchFlg, DCURRENCY * lSubTotal, LONG * lSubCounter,UCHAR* uchScaleOut,PLUIF* pPluIf); /* Saratoga */

SHORT   RptPLUByACTPLUSat(UCHAR uchMinorClass, UCHAR uchType,UCHAR uchResetType, USHORT usStatus,UCHAR* puchFlg, DCURRENCY * lSubTotal, LONG* lSubCounter,UCHAR* uchScaleOut,PLUIF* pPluIf, TTLPLU *ttlPLU); /* Saratoga */

SHORT RptPLUByDEPT( UCHAR  uchMinorClass, UCHAR  uchType, USHORT usStatus, USHORT usDEPTNumber, LTOTAL *pTotal, UCHAR  *puchFlag, UCHAR uchResetType); /* Saratoga */
VOID  RptDEPTNamePrt( USHORT usDEPTNumber, UCHAR uchType);  /* Saratoga */
VOID  RptPLUPrint( UCHAR  uchMinor,
                   LTOTAL *pPLUTotal,
                   TCHAR  *puchPLUNumbe,                    /* Saratoga */
                   TCHAR  aszMnemo[PARA_PLU_LEN+1],
                   TCHAR  aszSpeMnemo[PARA_SPEMNEMO_LEN+1],
                   TCHAR  aszAdjNemo[PARA_ADJMNEMO_LEN+1],
                   UCHAR  auchPrice[3],
                   USHORT usPercent,
                   UCHAR  uchStatus,
                   UCHAR  uchType);
VOID    RptPLUHeader( UCHAR uchMinorClass, UCHAR uchType, TCHAR *puchPluNo, UCHAR uchResetType);
USHORT  RptPLUPercent( TTLPLU *pTtlPlu, UCHAR  uchScale);
VOID  RptPLUGrandTtlPrt( USHORT usTransAddr, LTOTAL *pTotal, USHORT usStatus, UCHAR  uchWeigt, UCHAR  uchType);
SHORT RptPLUDayReset( UCHAR uchFunc);
SHORT RptPLUPTDReset( UCHAR uchFunc);
SHORT RptPLUReset( UCHAR uchMinorClass, UCHAR uchFunc);
SHORT RptPLULock( VOID );
VOID  RptPLUUnLock( VOID );
SHORT RptPLUEditAll(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchResetType);     /* Saratoga */

SHORT   RptPLUDayOptimize(UCHAR uchFunc);
SHORT   RptPLUPTDOptimize(UCHAR uchFunc);
SHORT   RptPLUOptimize(UCHAR uchMinorClass, UCHAR uchFunc);

/***** CASHIER REPORT *****/

SHORT   RptCashierRead( UCHAR uchMinorClass, UCHAR uchType, ULONG ulCashierNumber, UCHAR uchResetType);
SHORT   RptCashierEdit( RptElementFunc RptElement, UCHAR uchMinorClass, TTLCASHIER *pTtlCas);
SHORT   RptCashierReset( UCHAR uchMinorClass, UCHAR uchType, UCHAR uchFunc, ULONG ulCashierNumber);
SHORT   RptCashierIndReset( UCHAR uchMinorClass, UCHAR uchFunc, ULONG ulCashierNumber);
SHORT   RptCashierAllReset( UCHAR uchMinorClass, UCHAR uchType, UCHAR uchFunc);
SHORT   RptCasAllOnlyReset( UCHAR  uchMinorClass, UCHAR  uchType, UCHAR  uchFunc, SHORT  sNo, ULONG *ausResetBuffer);
SHORT   RptCasAllRstReport( UCHAR  uchMinorClass, UCHAR  uchType, UCHAR  uchFunc, SHORT  sNo, ULONG *ausResetBuffer);
SHORT   RptCashierOpenReset( ULONG ulCashierNumber);
VOID    RptPrtCasTrng1( ULONG ulCashierNumber );
VOID    RptPrtCasTrng2( ULONG ulCashierNumber );
SHORT   RptCashierLock(UCHAR uchType, ULONG ulCashierNumber);
VOID    RptCashierUnLock(UCHAR uchType, ULONG ulCashierNumber);
SHORT	RptRegCasVAT(TTLCASHIER *pTtlData);
SHORT   RptChkCashierNo( DCURRENCY ulCashierNumber );

SHORT  ItemGenerateAc21Report (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, ULONG ulCashierNo);

/***** GUEST CHECK FILE REPORT *****/

SHORT   RptGuestRead( UCHAR uchType, USHORT usGuestNumber );
SHORT   RptGuestAllRead( UCHAR uchType, ULONG ulCashierNumber );
SHORT   RptGuestIndRead( USHORT usNumber );
SHORT   RptGuestResetByCashier( ULONG ulCashierNumber );
SHORT   RptGuestResetByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo );
SHORT   RptGuestIndReset( USHORT usNumber );
SHORT   RptGuestOpenReset( USHORT usNumber );
SHORT   RptGuestAllEdit( UCHAR uchType, USHORT usRcvBuff[], SHORT  sNoofGC );
SHORT   RptGuestIndEdit( UCHAR uchType, USHORT usGCNumber, SHORT sFeedType );
VOID    RptGuestElement( UCHAR  uchType, UCHAR  uchMinorClass, USHORT  usTranAddr, UCHAR  uchSpeAddr, USHORT usGCNumber, UCHAR  uchCdv, ULONG  ulCashWaitNo, UCHAR  *uchCheckOpen, USHORT usNumber, D13DIGITS  d13Total );
SHORT   RptGuestLock(UCHAR uchType, ULONG ulCashierNumber );
SHORT   RptGuestUnLock( UCHAR uchType, ULONG ulCashierNumber );

SHORT   RptGuestLockByCashier( ULONG ulCashierNumber );
SHORT   RptGuestLockByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo );
SHORT   RptGuestUnlockByCashier( ULONG ulCashierNumber );
SHORT   RptGuestUnlockByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo );
USHORT  RptGuestOptimizeFile (void);

/***** EOD/PTD REPORT *****/

SHORT   RptEODRead( UCHAR uchType );
SHORT   RptEODReset( UCHAR uchType );
SHORT   RptEODPTDLock( UCHAR uchACNumber );
SHORT   RptEODPTDUnLock( VOID );
SHORT   RptEODOnlyReset( VOID );
SHORT   RptEODPrtReset( VOID );
SHORT   RptEODAllIssuedChk( VOID );
VOID    RptEODPTDMakeHeader( UCHAR uchACNoAddr, UCHAR uchRptAddr, UCHAR uchSpeAddr, UCHAR uchType );
BOOL    RptEODChkFileExist( UCHAR uchAddress );
SHORT   RptPTDRead( UCHAR uchType );
SHORT   RptPTDReset( UCHAR uchType );
SHORT   RptPTDLock( VOID );
SHORT   RptPTDUnLock( VOID );
SHORT   RptPTDOnlyReset( VOID );
SHORT   RptPTDPrtReset( VOID );
SHORT   RptPTDAllIssuedChk( VOID );
BOOL    RptPTDChkFileExist( UCHAR uchAddress );
#if 0
// following functions allow for enabling better error reporting for report errors.
// they allow ASSRTLOG logs indicating the error as well as where the RptPrtError()
// function was called.
SHORT RptPrtError_Debug( SHORT sError, char *aszFilePath, int nLineNo );
#define RptPrtError(sError) RptPrtError_Debug(sError, __FILE__, __LINE__)
#else
SHORT   RptPrtError( SHORT sError );
#endif

/***** EJ REPORT *****/

SHORT RptEJRead( VOID );                                /* RptEJ.c */
SHORT RptEJReset( VOID );                               /* RptEJ.c */
SHORT RptEJClusterReset( VOID );                        /* RptEJ.c */
SHORT RptEJReverse( VOID );                             /* RptEJ.c */
SHORT Rpt_EJEdit( SHORT sType, UCHAR uchTermNo );       /* RptEJ.c */
SHORT Rpt_EJEdit_Print( SHORT sType, UCHAR uchTermNo ); /* RptEJ.c */
SHORT Rpt_EJEdit_Display( VOID );                       /* RptEJ.c */
VOID  Rpt_EJChgNULL( TCHAR *pData, UCHAR uchLen );      /* RptEJ.c */
UCHAR Rpt_EJChkShr( VOID );                             /* RptEJ.c */
SHORT Rpt_EJConfirmation(SHORT sError, SHORT sErrorType);/* RptEJ.c */
SHORT Rpt_EJPrtError( SHORT sError );                   /* RptEJ.c */

/***** EMPLOYEE REPORT *****/

SHORT   RptETKRead( UCHAR uchMinorClass, UCHAR uchType, ULONG ulEmployeeNumber, UCHAR uchResetType);
SHORT   RptETKEdit( UCHAR uchMinorClass, RPTEMPLOYEE *pRptEmployee, ULONG ulEmployeeNo);
SHORT   RptETKReset(UCHAR uchType, UCHAR uchFunc);
SHORT   RptETKAllReset(UCHAR uchType, UCHAR uchFunc);
SHORT   RptETKAllOnlyReset( UCHAR  uchType, UCHAR  uchFunc);
SHORT   RptETKAllRstReport( UCHAR  uchType, UCHAR  uchFunc);
SHORT   RptETKLock(VOID);
VOID    RptETKUnLock(VOID);

/****** Report Cpm/Ept Tally ********/
SHORT RptTallyReadOrReset( UCHAR uchType );
SHORT RptCpmTally( UCHAR uchType );
SHORT RptEptTally( UCHAR uchType );

/****** Coupon Report ******/
SHORT  RptCpnRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber);
SHORT  RptCpnHeader(UCHAR uchMinorClass, UCHAR uchType);
SHORT  RptCpnIndProc(UCHAR uchMinorClass, USHORT usCpnNumber, UCHAR uchType);
SHORT  RptCpnAllProc(UCHAR uchMinorClass, UCHAR uchType);
VOID   RptCpnGrandTtlPrt(LTOTAL *pTotal, UCHAR uchType);
SHORT  RptCpnReset(UCHAR uchMinorClass, UCHAR uchType);
SHORT RptCpnLock(VOID);
VOID  RptCpnUnLock(VOID);

SHORT  ItemGenerateAc30Report(UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE* fpFile);

/****** PLU Group Summary Report ******/
//GroupReport - CSMALL
SHORT  RptPLUGroupRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber);
SHORT  RptPLUGroupHeader(UCHAR uchMinorClass, UCHAR uchType);
SHORT  RptPLUGroupIndProc(UCHAR uchMinorClass, UCHAR uchCpnNumber, UCHAR uchType);
SHORT  RptPLUGroupAllProc(UCHAR uchMinorClass, UCHAR uchType);
VOID   RptPLUGroupGrandTtlPrt(LTOTAL *pTotal, UCHAR uchType);
SHORT  RptPLUGroupReset(UCHAR uchMinorClass, UCHAR uchType);
SHORT RptPLUGroupLock(VOID);
VOID  RptPLUGroupUnLock(VOID);

/****** Service Time Report *****/
SHORT RptServiceDayRead(UCHAR uchMinor, UCHAR uchType);
SHORT RptServicePTDRead(UCHAR uchMinor, UCHAR uchType);
SHORT RptServiceAllRead(UCHAR uchMinorClass, UCHAR uchType);
SHORT RptServiceAllEdit(TTLSERTIME *pTtlData);
VOID RptServiceTtlEdit(TOTAL *pTotal, TOTAL *pAllTotal);
SHORT RptServiceDayReset( UCHAR uchFunc);
SHORT RptServicePTDReset( UCHAR uchFunc);
SHORT RptServiceReset( UCHAR uchMinorClass, UCHAR uchFunc);
SHORT RptServiceLock( VOID );
VOID  RptServiceUnLock( VOID );
/* --- End of Header File --- */