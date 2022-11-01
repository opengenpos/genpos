/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : para.h                                            :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-05 : 00.00.00     :                         :
:   Update Histories   : 92-11-19 : 01.00.00     : Adds PARATARE           :
:   Update Histories   : 93-06-07 : 01.00.00     : Adds Enhance Function   :
:                      : 93-06-22 : 02.00.00     : Adds PARAEMPLOYEENO     :
:                      : 93-06-22 : 01.00.12     : Adds PARASHAREDPRT      :
:                      : 93-07-02 : 01.00.12     : Adds PARAPRESETAMOUNT   :
:                      : 93-07-09 : 01.00.12     : Adds PARAPIGRULE        :
:                      : 93-08-23 : 01.00.13     : Adds PARASOFTCHK        :
:                      : 93-10-05 : 02.00.01     : Adds PARAETKFL          :
:                      : 93-11-16 : 02.00.02     : Adds PARACPMTALLY       :
:                      : 94-04-01 : 00.00.04     : change HOTELID          :
:                      : 95-02-06 : 03.00.00     : Adds uchLinkNumber      :
:                      :          :              : Adds UchClassNumber     :
:                      : 95-02-27 : 03.00.00     : Adds PARACPN            :
:                      :          :              : Chg. MAINTWORK          :
:                      : 95-03-01 : 03.00.00     : Chg. from fsCPNStatus to uchCPNStatus
:                      : 95-03-14 : 03.00.00     : Adds uchCPNAdj[] to PARACPN
:                      : 95-11-09 : 03.01.00     : Chg. PARAMDC            :
:                      :                         :                         :
:                      : 00-01-25 : 01.00.00     : Saratoga initial        :
:                      :                         :                         :
:  ----------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00A by Microsoft Corp.                :
:   Memory Model       : Midium Model                                      :
:   Condition          :                                                   :
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


/*------------------------------------------------------------------------*\

            T Y P E D E F     S T R U C T U R Es

\*------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*--- read/write Guest No. ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        USHORT usGuestNumber;         /* Guest Check Number */
        WCHAR  aszMnemonics[PARA_TRANSMNEMO_LEN+1];      /* Transaction Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAGUESTNO;

/*--- read/write PLU no. ---*/

/* 2172 */
typedef struct  {
        UCHAR  uchAdjective;          /* Adjective Number */
        UCHAR  uchModStat;            /* Modifier Status */
        WCHAR  aszPLUNumber[PLU_MAX_DIGIT+1]; /* PLU Number  + 1*/
}PLUNO;

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchMenuPageNumber;     /* K/B Page Number */
        UCHAR  uchMajorFSCData;       /* Major FSC Data */
        UCHAR  uchMinorFSCData;       /* Minor FSC Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        PLUNO  PluNo;                 /* Keyed PLU Record */
        USHORT usPrintControl;        /* Print Control */
}PARAPLUNOMENU;

/*--- read/write rounding table ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Data Class Definition */
        UCHAR  uchMinorClass;         /* Minor Data Class Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchRoundDelimit;       /* Delimit */
        UCHAR  uchRoundModules;       /* Modulus */
        CHAR   chRoundPosition;       /* Position */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARAROUNDTBL;

/*--- read/write discount rate & bonus rate ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchDiscRate;           /* Discount Rate */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARADISCTBL;

/* Trailer Print for Soft Check */

typedef struct  {
    UCHAR  uchMajorClass;                       /* Major Class Data Definition */
    UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
    UCHAR  uchAddress;                          /* Address of Mnemonics */
    WCHAR  aszMnemonics[PARA_SOFTCHK_LEN+1];    /* Char. 42 Mnemonics Data */
    UCHAR  uchStatus;                           /* 0=with data, 1=without data */
    USHORT usPrintControl;                      /* Print Control */
}PARASOFTCHK;

/* 44 Char. Mnemonics */

/* typedef struct   {                          */
/*  UCHAR  uchMajorClass;                      */   /* Major Class Data Definition */
/*      UCHAR  uchMinorClass;                  */   /* Minor Class Data Definition */
/*      UCHAR  uchAddress;                     */   /* Address of 24 Char. Mnemonics */
/*      UCHAR  aszMnemonics[PARA_CHAR44_LEN+1];*/   /* Char. 44 Mnemonics Data */
/*      UCHAR  uchStatus;                      */   /* 0=with data, 1=without data */
/*  USHORT usPrintControl;                     */   /* Print Control */
/* }PARACHAR44;                                */

/*--- read/write sales promotion header ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        WCHAR  aszProSales[PARA_PROMOTION_LEN+1];      /* Promotion Sales Header */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAPROMOTION;

/*--- read/write currency conversion ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulForeignCurrency;     /* Foreign Currency Rate */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARACURRENCYTBL;

/*--- read/write set menu of PLU, 2172 ---*/

typedef struct  {
    UCHAR  uchModStat;            /* Modifier Status */
    WCHAR  aszPLUNumber[PLU_MAX_DIGIT + 1];  /* PLU Number + 1 */
    UCHAR  uchAdjective;              /* Adjective Code of PLU */
}MENUPLU;


typedef struct  {
        UCHAR   uchMajorClass;          /* Major Class Data Definition */
        UCHAR   uchMinorClass;          /* Minor Class Data Definition */
        UCHAR   uchAddress;             /* Address */
        UCHAR   uchArrayAddr;           /* MENUPLU Array Address */
        MENUPLU SetPLU[6];              /* Parent PLU and Child PLU Set */
        WCHAR   aszLeadMnemonics[PARA_LEADTHRU_LEN+1];  /* Lead Through Mnemonics */
        UCHAR   uchPageNumber;          /* Page Number */
        UCHAR   uchStatus;              /* 0=with data, 1=without data */
        USHORT  usPrintControl;         /* Print Control */
}PARAMENUPLUTBL;

/*--- read/write %rate & reciprocal rate & service tax & canadian tax rate ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulTaxRate;             /* Tax Rate */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARATAXRATETBL;

/*--- read/write DEPT ---*/

typedef struct  {
        UCHAR  uchMajorClass;                       /* Major Class Data Definition */
        UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
        USHORT usACNumber;                          /* A/C Number */
        UCHAR  uchFieldAddr;                        /* Field Address of DEPT */
        USHORT usDEPTNumber;                       /* DEPT Number, 2172 */
        UCHAR  uchMajorDEPT;                        /* Major DEPT Number */
        UCHAR  auchDEPTStatus[MAX_DEPTSTS_SIZE];    /* Status1 ~ 9 */
        UCHAR  uchHALO;                             /* HALO */
        WCHAR  aszMnemonic[PARA_DEPT_LEN+1];        /* DEPT Name */
        UCHAR  uchPrintPriority;                    /* Print Priority */
        UCHAR  uchStatus;                           /* 0=with data, 1=without data */
        USHORT usPrintControl;                      /* Print Control */
}PARADEPT;

/*--- read/write Combination Coupon ---*/

typedef struct  {
        UCHAR  uchMajorClass;                       /* Major Class Data Definition */
        UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
        USHORT usACNumber;                          /* A/C Number */
        UCHAR  uchFieldAddr;                        /* Field Address of Combination Coupon */
        USHORT  uchCPNNumber;                        /* Combination Coupon Number */
        UCHAR  uchCPNStatus[MAX_CPNSTS_SIZE];       /* Status */
		UCHAR  uchAutoCPNStatus;					/* ACC Status */
        ULONG  ulCPNAmount;                         /* Amount */
        WCHAR  aszMnemonic[PARA_CPN_LEN+1];         /* Coupon Name */
        WCHAR  auchCPNTarget[MAX_CPNTARGET_SIZE][PLU_MAX_DIGIT];    /* PLU Number, 2172 */
        /* USHORT usCPNTarget[MAX_CPNTARGET_SIZE];     / Target Menu */
        UCHAR  uchCPNAdj[MAX_CPNTARGET_SIZE];       /* Adjective */
        UCHAR  uchStatus;                           /* 0=with data, 1=without data */
        USHORT usPrintControl;                      /* Print Control */
}PARACPN;

/*--- read/write PLU ---*/

typedef struct  {
        UCHAR  uchMajorClass;                       /* Major Class Data Definition */
        UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
        USHORT usACNumber;                          /* A/C Number */
        UCHAR  uchFieldAddr;                        /* Field Address of PLU */
        WCHAR  auchPLUNumber[PLU_MAX_DIGIT+1+1];    /* PLU Number w/ scan code, 2172 */
        UCHAR  uchPLUType;                          /* PLU Type */
        USHORT  usDEPTNumber;                       /* DEPT Number, 2172 */
        UCHAR  uchReportCode;                       /* Report Code */
        UCHAR  uchTableNumber;                      /* Table Number */
        UCHAR  uchGroupNumber;                      /* Group Number */
        UCHAR  uchPrintPriority;                    /* Print Priority */
        USHORT  uchPPICode;                          /* PPI Code R3.1 */
        UCHAR  auchPLUStatus[MAX_PLUSTS_SIZE];      /* Status1 ~ 10 */
        WCHAR  aszMnemonics[PARA_PLU_LEN+1];        /* PLU Name */
        UCHAR  auchPresetPrice[MAX_ADJECTIVE_NO][MAX_PLUPRICE_SIZE];    /* Adjective Price 1 ~ 5 */
        USHORT usLinkNumber;                        /* Link Code, 2172 */
        UCHAR  uchSalesRestriction;                 /* Sales Restriction Code, 2172 */
        UCHAR  uchPriceMultiple;                    /* Price Multiple/Unit Weight, 2172 */
        USHORT usFamilyCode;                        /* Family Code, 05/13/00 */
        UCHAR  uchStatus;                           /* 0=with data, 1=without data */
        UCHAR  uchEVersion;                         /* E-version, 2172 */
        USHORT usPrintControl;                      /* Print Control */
}PARAPLU;

/*--- read/write CSTR ---*/
typedef struct  {
        UCHAR  uchMajorCstrData;
        UCHAR  uchMinorCstrData;
}CSTRPARAENTRY;

typedef struct  {
        UCHAR  uchMajorClass;                                           /* Major Class Data Definition */
        UCHAR  uchMinorClass;                                           /* Minor Class Data Definition */
        UCHAR  uchAddr;                                            /* Field Address of PLU */
        USHORT usCstrNumber;                                             /* PLU Number */
        USHORT usCstrLength;
        CSTRPARAENTRY  ParaCstr[MAX_PARACSTR_SIZE];                          /* Status1 ~ 10 */
        UCHAR  uchStatus;                                               /* 0=with data, 1=without data */
        USHORT usPrintControl;                                          /* Print Control */
}PARACSTR;


/*--- read/write PPI ---*/
typedef struct  {
        UCHAR  uchQTY;
        ULONG  ulPrice;
}PPIPARAENTRY;

typedef struct  {
        UCHAR  uchMajorClass;                                           /* Major Class Data Definition */
        UCHAR  uchMinorClass;                                           /* Minor Class Data Definition */
        USHORT  uchAddr;                                                 /* Field Address of PPI */
        USHORT  uchPPINumber;                                            /* PPI Number */
        USHORT  uchPPILength;
        PPIPARAENTRY  ParaPPI[MAX_PARAPPI_SIZE];                        /* Status1 ~ 10 */
        UCHAR  uchStatus;                                               /* 0=with data, 1=without data */
        USHORT usPrintControl;                                          /* Print Control */
}PARAPPI;


/*--- read/write MDC ---*/

typedef struct  {
        UCHAR  uchMajorClass;          /* Major Class Data Definition */
        UCHAR  uchMinorClass;          /* Minor Class Data Definition */
        USHORT usAddress;              /* Address of MDC */
        UCHAR  uchMDCData;             /* Data of MDC */
        WCHAR  aszMDCString[4+1];      /* MDC Strings Data */
        UCHAR  uchStatus;              /* 0=with data, 1=without data */
        USHORT usPrintControl;         /* Print Control */
}PARAMDC;

/*--- read/write FSC ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchPageNo;             /* Page of FSC */
        UCHAR  uchAddress;            /* Address of FSC */
        UCHAR  uchMajorFSCData;       /* Major FSC Data */
        UCHAR  uchMinorFSCData;       /* Minor FSC Data */
        PLUNO  PluNo;                 /* Keyed PLU Record, 2172 */
        USHORT usDeptNo;              /* Keyed Dept, 2172 */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAFSC;

/*---read/write security no. of program mode ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usSecurityNumber;      /* Security Number */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASECURITYNO;

/*--- read/write supervisor number & level ---*/

typedef struct  {
        USHORT usSuperNumber;         /* Supervisor Number */
        UCHAR  uchSecret;              /* Secret Number */
}SUPLEVEL;

typedef struct  {
        UCHAR  uchMajorClass;          /* Major Class Data Definition */
        UCHAR  uchMinorClass;          /* Minor Class Data Definition */
        UCHAR  uchAddress;             /* Address */
        USHORT usSuperNumber;          /* Supervisor Number */
        UCHAR  uchSecret;              /* Secret Number */
        UCHAR  uchStatus;              /* 0=with data, 1=without data */
        USHORT usPrintControl;         /* Print Control */
}PARASUPLEVEL;

/*--- read/write action code security ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        USHORT usAddress;             /* Address of Security */
        UCHAR  uchSecurityBitPatern;  /* Data of Security */
        WCHAR  aszMDCString[4 + 1];   /* MDC String Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAACTCODESEC;

/*--- read/write transaction halo ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchHALO;               /* Transaction HALO */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARATRANSHALO;

/*--- read/write hourly activity time table ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchBlockNo;            /* Number of Time Block */
        UCHAR  uchHour;               /* Hour */
        UCHAR  uchMin;                /* Minute */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAHOURLYTIME;

/*--- read/write slip printer feed control ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchFeedCtl;            /* Slip Printer Feed Control Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASLIPFEEDCTL;

/*--- read/write transaction mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Transaction Mnemonics */
        WCHAR  aszMnemonics[PARA_TRANSMNEMO_LEN+1];      /* Transaction Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARATRANSMNEMO;

/*--- read/write lead through ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Lead Through Data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];      /* Lead Through Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARALEADTHRU;

/*--- read/write report name ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Report Name */
        WCHAR  aszMnemonics[PARA_REPORTNAME_LEN+1];      /* Report Name Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAREPORTNAME;

/*--- read/write special mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Special Mnemonics */
        WCHAR  aszMnemonics[PARA_SPEMNEMO_LEN+1];      /* Special Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASPEMNEMO;

/*--- read/write PC password mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Transaction Mnemonics */
        WCHAR  aszMnemonics[PARA_PCIF_LEN+1];      /* PC Password Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAPCIF;

/*--- read/write us tax table ---*/

typedef struct {
        USHORT ausTblLen[MAX_TTBLHEAD_SIZE];    /* Tax Table Length */
        UCHAR  auchTblData[MAX_TTBLDATA_SIZE];  /* Tax Table */
}TAXDATA;

typedef struct  {
        UCHAR    uchMajorClass;         /* Major Class Data Definition */
        UCHAR    uchMinorClass;         /* Minor Class Data Definition */
        TAXDATA  TaxData;               /* Tax Table */
        UCHAR    uchStatus;             /* 0=with data, 1=without data */
        USHORT   usPrintControl;        /* Print Control */
}PARATAXTBL;

/*--- read/write adjective mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Adjective Mnemonics */
        WCHAR  aszMnemonics[PARA_ADJMNEMO_LEN+1];      /* Adjective Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAADJMNEMO;

/*--- read/write print modifier mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Print Modifier Mnemonics */
        WCHAR  aszMnemonics[PARA_PRTMODI_LEN+1];      /* Print Modifier Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAPRTMODI;

/*--- read/write major department mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Report Code Mnemonics */
        WCHAR  aszMnemonics[PARA_MAJORDEPT_LEN+1];      /* Major DEPT Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAMAJORDEPT;

/*--- read/write 24 characters mnemonics ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of 24 Char. Mnemonics */
        WCHAR  aszMnemonics[PARA_CHAR24_LEN+1];      /* Char. 24 Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARACHAR24;

/*---read/write menu page control table in reg/super mode ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchPageNumber;         /* Page Number Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARACTLTBLMENU;

/*--- assign destination kitchen printer no. ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchSrcPtr;             /* Source Kitchen Printer Number */
        UCHAR  uchDesPtr;             /* Destination Kitchen Printer Number */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARAALTKITCH;

/*---assign a number of record & PTD ---*/

typedef struct  {
        ULONG  ulRecordNumber;        /* Number of Record, 2172 */
        UCHAR  uchPTDFlag;            /* PTD Provide Flag */
}FLEXMEM;

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulRecordNumber;        /* Number of Record, 2172 */
        UCHAR  uchPTDFlag;            /* PTD Provide Flag */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAFLEXMEM;

/*--- assign store/register no. ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usStoreNo;             /* Store Number */
        USHORT usRegisterNo;          /* Register Number */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASTOREGNO;

/*--- assign characteristic type of total key ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchTypeIdx;            /* Type of Total Data ,Index of Financial Tender */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARATTLKEYTYP;

/*--- read/write tender key parameter,  V3.3 ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchTend;               /* Tender Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARATEND;

/*--- assign bit decide to  characteristic total key ---*/

typedef struct  {
        UCHAR  uchMajorClass;           /* Major Class Data Definition */
        UCHAR  uchMinorClass;           /* Minor Class Data Definition */
        UCHAR  uchAddress;              /* Address */
        UCHAR  uchField;                /* Field */
        UCHAR  uchTtlKeyMDCData;        /* MDC Data of Total Key */
        WCHAR  aszTtlKeyMDCString[4+1]; /* MDC Strings Data */
        UCHAR  uchStatus;               /* 0=with data, 1=without data */
        USHORT usPrintControl;          /* Print Control */
}PARATTLKEYCTL;

/*--- assign cashier no. A/B key ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usCashNumber;          /* Cashier Number */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARACASHABASSIGN;

/*--- read/write special counter ---*/

typedef struct {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usCounter;
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASPCCO;


/*--- employee number assignment ---*/        /* 93-06-22 */

typedef struct {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchFieldAddr;          /* Field Address */
        ULONG  ulEmployeeNo;          /* Employee Number R3.1 */
        UCHAR  uchJobCode1;            /* Job code 1 */
        UCHAR  uchJobCode2;            /* Job code 2 */
        UCHAR  uchJobCode3;            /* Job code 3 */
        UCHAR  uchStatus;             /* Status */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];  /* Mnemonics */
        USHORT usRptAllTtlTime;       /* Report All Total time */
        USHORT usRptAllTtlMinute;     /* Report All Total Minute */
        USHORT usPrintControl;        /* Print Control */
}PARAEMPLOYEENO;

/*--- waiter number assignment ---*/
#ifdef WAITER_MODEL
typedef struct {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchFieldAddr;          /* Field Address */
        USHORT usWaiterNo;            /* Waiter Number */
        UCHAR  uchWaitStatus;         /* Waiter Status */
        UCHAR  uchSecret;             /* Secret */
        UCHAR  uchStatus;             /* Status */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];  /* Mnemonics */
        USHORT usStartGCNo;           /* Start Guest Check Number */
        USHORT usEndGCNo;             /* End Guest Check Number */
        USHORT usPrintControl;        /* Print Control */
}PARAWAITERNO;
#endif
/*--- cashier number assignment ---*/
/***
typedef struct {
        UCHAR  uchMajorClass;         / Major Class Data Definition /
        UCHAR  uchMinorClass;         / Minor Class Data Definition /
        USHORT usCashierNo;           / Cashier Number /
        UCHAR  uchCashStatus;         / Cashier Status /
        UCHAR  uchSecret;             / Secret /
        UCHAR  uchStatus;             / Status /
        UCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; / Mnemonics /
        USHORT usPrintControl;        / Print Control /
}PARACASHIERNO;
***/

/*--- TOD Assignment ---*/

typedef struct  {
        UCHAR  uchMajorClass;           /* Major Class Data Definition */
        UCHAR  uchMinorClass;           /* Minor Class Data Definition */
        UCHAR  uchAddress;              /* Address */
        USHORT usMin;                   /* Minute */
        USHORT usHour;                  /* Hour */
        USHORT usMDay;                  /* Day */
        USHORT usMonth;                 /* Month */
        USHORT usYear;                  /* Year */
        USHORT usWDay;                  /* Day of Week, 2172 */
        UCHAR  uchStatus;               /* Status */
        USHORT usPrintControl;          /* Print Control */
}PARATOD;

/*--- read/write tone control ---*/

typedef struct  {
        UCHAR  uchMajorClass;                   /* Major Class Data Definition */
        UCHAR  uchMinorClass;                   /* Minor Class Data Definition */
        UCHAR  uchAddress;                      /* Address */
        UCHAR  uchToneCtl;                      /* Tone Control Data */
        UCHAR  uchStatus;                       /* 0=with data, 1=without data */
        WCHAR  aszLeadMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics */
        USHORT usPrintControl;                  /* Print Control */
}PARATONECTL;

/*--- read/write Tare Table ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usWeight;              /* Tare Weight */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];      /* Leadthrouh,Special Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARATARE;

/*--- read/write Shared Printer ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchTermNo;             /* Terminal Number with Shared Printer */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASHAREDPRT;


/*--- read/write Preset Amount Cash Tender ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulAmount;              /* Preset Cash Tender Amount */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];      /* Leadthrouh,Special Mnemonics Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAPRESETAMOUNT;

/*--- read/write pig rule ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulPigRule;             /* Co. Limit, Unit Price */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARAPIGRULE;

/*--- read/write service time parameter,  R3.1 ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usBorder;              /* Service Time Border Second */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];  /* Mnemonics */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARASERVICETIME;

/* maintenance ETK File */

typedef struct {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchJobCode;            /* Job code 1 */
        ULONG  ulEmployeeNo;          /* Employee Number R3.1 */
        USHORT usBlockNo;             /* Block Number */
        ULONG  ulDateTimeIn;          /* Data/Time-in Data */
        ULONG  ulTimeOut;             /* Time-out Data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];  /* Mnemonics */
        UCHAR  uchStatus;             /* Status */
        USHORT usPrintControl;        /* Print Control */
}PARAETKFL;

/*--- Set CPM / EPT Tally ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        WCHAR  auchMnemo[MAX_TALLY_MNEMO];           /* Mnemonic */
        USHORT usTallyData;           /* Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARACPMTALLY;

/*--- Hotel Id ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address of Adjective Mnemonics */
        WCHAR  aszMnemonics[MAX_HOTELID_SIZE+1];  /* Mnemonics
                                        TYPE 1: HOTEL ID TYPE 2: SLD */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        USHORT usPrintControl;        /* Print Control */
}PARAHOTELID;

/*--- Prog. 54 for Charge Posting Data in Uniniram.asm ---*/

typedef struct  {
        WCHAR auchHotelId[PARA_HOTELID_LEN];    /* Hotel Id save Area       */
        WCHAR auchSld[PARA_SLD_LEN];            /* SLD Id save area         */
        WCHAR auchProductId[PARA_PDT_LEN];      /* Product Id save area     */
        WCHAR auchTend[11];                     /* Tender 1 to 11 EPT Action code */
        WCHAR auchApproval[2];                  /* Approval code save area  */
}CPMDATA;

/*--- Super. 160 for Maintenance Order Entry Prompt Table ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchTblNumber;          /* Table Number */
        UCHAR  uchTblAddr;            /* Table Address */
        UCHAR  uchOepData[MAX_ONE_OEPTABLE_SIZE];/* Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARAOEPTBL;

/*--- Super. 162 for Set Flexible Drive Through/Multi Store Recall Parameter ---*/

typedef struct  {
        UCHAR  uchMajorClass;                /* Major Class Data Definition */
        UCHAR  uchMinorClass;                /* Minor Class Data Definition */
        UCHAR  uchTblAddr;                        /* Table Address          */
        UCHAR  uchTblData[MAX_FLEXDRIVE_SIZE];    /* Data                   */
        UCHAR  uchStatus;                    /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;                    /* Print Control          */
}PARAFXDRIVE;

/*--- FSC Table ---*/

typedef struct  {
        UCHAR   uchFsc;     /* FSC Code */
        UCHAR   uchExt;     /* Extend Code */
}FSCDATA;

typedef struct {
        FSCDATA  TblFsc[MAX_FSC_NO];    /* One of Page */
}PARAFSCTBL;

/*--- read/write labor cost value ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usLaborCost;           /* Labor cost value */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARALABORCOST;


/*---read/write beverage dispenser parameter ---*/

typedef struct  {
        USHORT usPluNumber;          /* PLU Number */
        UCHAR  uchAdjective;         /* Adjective Code */
}DISPPARA;

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        USHORT usAddress;             /* Address */
        USHORT usPluNumber;          /* PLU Number */
        UCHAR  uchAdjective;         /* Adjective Code */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARADISPPARA;

/*--- read/write mld mnemonics parameter,  V3.3 ---*/

typedef struct  {
    UCHAR  uchMajorClass;                       /* Major Class Data Definition */
    UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
    UCHAR  uchAddress;                          /* Address of Mnemonics */
    WCHAR  aszMnemonics[PARA_MLDMNEMO_LEN+1];   /* Char. 32 Mnemonics Data */
    UCHAR  uchStatus;                           /* 0=with data, 1=without data */
    USHORT usPrintControl;                      /* Print Control */
}PARAMLDMNEMO;

/* SR 14 ESMITH*/
typedef struct {
	UCHAR	uchIpAddress[PARA_KDSIP_LEN];		/* IP Address  */
	USHORT	usPort;								/* Port Address of KDS  */
	USHORT	usNHPOSPort;						/* Port Address of Interface port with NHPOS */
}PARAKDS;

typedef struct {						/* Auto Combinational Coupons */
	UCHAR   uchMajorClass;
	UCHAR	uchMinorClass;
	UCHAR	uchStatus;
	UCHAR	uchAddress;
	ULONG	ulAmount;
    WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];      /* Leadthrouh,Special Mnemonics Data */
    USHORT usPrintControl;        /* Print Control */
}PARAAUTOCPN;


/* Prog #51, 2172 */

typedef struct {
    UCHAR  uchMajorClass;                       /* Major Class Data Definition */
    UCHAR  uchMinorClass;                       /* Minor Class Data Definition */
    UCHAR  uchAddress;                          /* Address of Mnemonics */
    USHORT ausIPAddress[PARA_KDSIP_LEN];            /* IP Adderss, Port No */
    WCHAR  aszInputData[MAX_KDSIP_INPUT_LEN];   /* input buffer */
    UCHAR  uchStatus;                           /* 0=with data, 1=without data */
    USHORT usPrintControl;                      /* Print Control */
}PARAKDSIP;

/*--- read/write sales restriction table, 2172 ---*/

#define RESTRICT_WEEK_DAYS_ON    0x80       // indicator used with UCHAR uchDay to indicate bit mask of days of week rather than single day.
#define RESTRICT_OVERRIDE_ANDOR  0x80       // indicator used with UCHAR uchDate to indicate override MDC Address 298 Bit C, MDC_PLU5_ADR EVEN_MDC_BIT1.
#define RESTRICT_OVERRIDE_AND    0x40       // indicates used with UCHAR uchDate to indicate AND as if MDC Address 298 Bit C is on.

typedef struct  {
        UCHAR  uchDate;               /* Date, day of month, 1 - 31 */
        UCHAR  uchDay;                /* Day of Week (1 - 7) or bit mask of multiple days of week if 0x80, RESTRICT_WEEK_DAYS_ON, set */
        UCHAR  uchHour1;              /* Start Hour */
        UCHAR  uchMin1;               /* Start Minute */
        UCHAR  uchHour2;              /* End Hour */
        UCHAR  uchMin2;               /* End Minute */
}RESTRICT;

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchField;              /* Field */
        UCHAR  uchData;               /* Inputted Data */
        UCHAR  uchDate;               /* Date, day of month, 1 - 31 */
        UCHAR  uchDay;                /* Day of Week (1 - 7) or bit mask of multiple days of week if 0x80, RESTRICT_WEEK_DAYS_ON, set */
        UCHAR  uchHour1;              /* Start Hour */
        UCHAR  uchMin1;               /* Start Minute */
        UCHAR  uchHour2;              /* End Hour */
        UCHAR  uchMin2;               /* End Minute */
        UCHAR  uchStatus;             /* Status */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics */
        USHORT usPrintControl;        /* Print Control */
}PARARESTRICTION;

/*----- A/C 208 Boundary age data, 2172 -----*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        UCHAR  uchAgePara;            /* Boundary Age Para */
        UCHAR  uchStatus;             /* Status */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics */
        USHORT usPrintControl;        /* Print Control */
}PARABOUNDAGE;

/*--- read/write dept no., 2172 ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchMenuPageNumber;     /* K/B Page Number */
        UCHAR  uchMajorFSCData;       /* Major FSC Data */
        UCHAR  uchMinorFSCData;       /* Minor FSC Data */
        USHORT usDeptNumber;          /* Dept Item Number */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics for LeadThru */
        USHORT usPrintControl;        /* Print Control */
}PARADEPTNOMENU;

/*--- read/write misc. parameter ---*//* ### ADD Saratoga Misc (052900) */

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        ULONG  ulMiscPara;            /* Misc. Para */
        UCHAR  uchStatus;             /* Status */
        WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics */
        USHORT usPrintControl;        /* Print Control */
}PARAMISCPARA;

// Terminal information structures to allow for the setting and retrieving
// of a particular terminal's information.
typedef struct {
	ULONG  ulCodePage;          // code page to use for MBCS, other conversions from UNICODE
	USHORT usLanguageID;        // Primary language id to use on this terminal
	USHORT usSubLanguageID;     // Sub-language id to use on this terminal
	USHORT usStoreNumber;       // The store number for this terminal
	USHORT usRegisterNumber;    // the number of this particular terminal or register
	UCHAR  uchKeyBoardType;      // keyboard type such as KEYBOARD_CONVENTION
	UCHAR  uchMenuPageDefault;   // Default menu page for this terminal (10 for touch)
	WCHAR  aszDefaultFontFamily[32];   // Font Family to use as a default
	WCHAR  aszUnLockCode[12];            // software security unlock code for terminal
	WCHAR  aszTerminalSerialNumber[16];  // serial number of the terminal
	WCHAR  aszDefaultMenuFileName[32];   // name of file containing default menu for this terminal
	UCHAR  ucIpAddress[4];      // IP address of the terminal
	UCHAR  uchOrderDeclaration; //Default Order Declaration Type
#ifdef R_20_DATABASECHANGE
	UCHAR  ucChargePostIndex;   // which charge post host this terminal is to use
	UCHAR  ucEPT_Index;         // which EPT host this terminal is to use
	UCHAR  ucEPT_EX_Index;      // which EPT EX host this terminal is to use
#endif
} TERMINALINFO;


typedef struct {
        UCHAR         uchMajorClass;         /* Major Class Data Definition */
        UCHAR         uchMinorClass;         /* Minor Class Data Definition */
        UCHAR         uchAddress;            /* Address */
		UCHAR         uchPadding;          /* padding to get to 4 byte boundary */
		TERMINALINFO  TerminalInfo;       /* information for the designated terminal */
} PARATERMINALINFO;

typedef struct {
		// New tender key info for DataCap and other electronic payment terminals
		ULONG  ulFlags0;
		ULONG  ulFlags1;              // Used to replicate MDC bits
        UCHAR  uchIPAddress[4];       // IP Address for payment terminal
        USHORT usIPPortNo;            // IP Port number for payment terminal process
		USHORT usTranType;            // Transaction Type (Credit, Debit, EBT, etc)
		USHORT usTranCode;            // Transaction Code (Sale, Return, Void, etc)
		WCHAR  tcMerchantID[24];      // Merchant Identifier

		// duplicate of the CPMDATA EPT tender option
		WCHAR  auchEptTend;

		// duplicate of tender key info from struct PARATEND
        UCHAR  uchTend;               // Tender Data
        UCHAR  uchStatus;             // 0=with data, 1=without data
        USHORT usPrintControl;        // Print Control
} TENDERKEYINFO;

typedef struct  {
        UCHAR  uchMajorClass;         // Major Class Data Definition
        UCHAR  uchMinorClass;         // Minor Class Data Definition
        UCHAR  uchAddress;            // Address
		TENDERKEYINFO  TenderKeyInfo;
}PARATENDERKEYINFO;

typedef struct {
        UCHAR  uchMajorClass;         // Major Class Data Definition
        UCHAR  uchMinorClass;         // Minor Class Data Definition
        UCHAR  uchAddress;            // Address
		USHORT usMnemonicStartAddr;   // offset to the starting mnemonic for this reason code
		USHORT usMnemonicCount;       // Count of mnemonics to be used
}PARAREASONCODEINFO;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*------------------------------------------------------------------------*\

            P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

VOID  ParaPLUNoMenuRead( PARAPLUNOMENU *pData );        /* read PLU No.of appointed address */
VOID  ParaPLUNoMenuWrite( PARAPLUNOMENU *pData );       /* set PLU No. in appointed address */
VOID  ParaRoundRead( PARAROUNDTBL *pData );             /* read rounding data */
VOID  ParaRoundWrite( PARAROUNDTBL *pData );            /* set rounding data */
VOID  ParaDiscRead( PARADISCTBL *pData );               /* read discount/bonus rate */
VOID  ParaDiscWrite( PARADISCTBL *pData );              /* set discount/bonus rate */
VOID  ParaSoftChkMsgRead( PARASOFTCHK *pData );         /* read mnemonics for soft check trailer */
VOID  ParaSoftChkMsgWrite( PARASOFTCHK *pData );        /* set mnemonics for soft check trailer */
/* VOID  ParaChar44Read( PARACHAR44 *pData );   */  /* read 44 character mnemonics for slip */
/* VOID  ParaChar44Write( PARACHAR44 *pData );  */  /* set 44 characters mnemonics for slip */
VOID  ParaPromotionRead( PARAPROMOTION *pData );        /* read promotion sales header */
VOID  ParaPromotionWrite( PARAPROMOTION *pData );       /* set promotion sales header */
VOID  ParaCurrencyRead( PARACURRENCYTBL *pData );       /* read foreign currency rate */
VOID  ParaCurrencyWrite( PARACURRENCYTBL *pData );      /* set foreign currency rate */
SHORT ParaMenuPLURead( PARAMENUPLUTBL *pData );         /* read set menu of PLU */
VOID  ParaMenuPLUWrite( UCHAR uchAction, PARAMENUPLUTBL *pData );  /* set/reset PLU No. of set menu */
VOID  ParaTaxRateRead( PARATAXRATETBL *pData );         /* read tax rate */
VOID  ParaTaxRateWrite( PARATAXRATETBL *pData );        /* set tax rate */
VOID  ParaMDCRead( PARAMDC *pData );                    /* read MDC data of appointed address */
VOID  ParaMDCWrite( PARAMDC *pData );                   /* set MDC data in appointed address */
UCHAR ParaMDCCheck( USHORT usAddress, UCHAR uchMaskData ); /* check if MDC data is set */
VOID  ParaFSCRead( PARAFSC *pData );                    /* read FSC data of appointed address */
VOID  ParaFSCWrite( PARAFSC *pData );                   /* set FSC data in appointed address */
VOID  ParaSecurityNoRead( PARASECURITYNO *pData );      /* read security No. */
VOID  ParaSecurityNoWrite( PARASECURITYNO *pData );     /* set security No. */
SHORT  ParaSupLevelRead( PARASUPLEVEL *pData );         /* read supervisor no. & level */
VOID  ParaSupLevelWrite( PARASUPLEVEL *pData );         /* set supervisor no. & level in appointed address */
VOID  ParaActCodeSecRead( PARAACTCODESEC *pData );      /* read action code security of appointed address */
VOID  ParaActCodeSecWrite( PARAACTCODESEC *pData);      /* set action code security in appointed address */
VOID  ParaTransHALORead( PARATRANSHALO *pData );        /* read transaction halo */
VOID  ParaTransHALOWrite( PARATRANSHALO *pData );       /* set transaction halo data */
VOID  ParaHourlyTimeRead( PARAHOURLYTIME *pData );      /* read hourly time data */
VOID  ParaHourlyTimeWrite( PARAHOURLYTIME *pData );     /* set hourly time data */
VOID  ParaSlipFeedCtlRead( PARASLIPFEEDCTL *pData );    /* read feed control data */
VOID  ParaSlipFeedCtlWrite( PARASLIPFEEDCTL *pData );   /* set feed control data */
VOID  ParaTransMnemoRead( PARATRANSMNEMO *pData );      /* read transaction mnemonics of appointed address */
VOID  ParaTransMnemoWrite( PARATRANSMNEMO *pData );     /* set transaction mnemonics in appointed address */
VOID  ParaLeadThruRead( PARALEADTHRU *pData );          /* read lead through of appointed address */
VOID  ParaLeadThruWrite( PARALEADTHRU *pData );         /* set lead through in appointed address */
VOID  ParaReportNameRead( PARAREPORTNAME *pData );      /* read report name of appointed address */
VOID  ParaReportNameWrite(PARAREPORTNAME *pData );      /* set report name in appointed address */
VOID  ParaSpeMnemoRead( PARASPEMNEMO *pData );          /* read special mnemonics of appointed address */
VOID  ParaSpeMnemoWrite( PARASPEMNEMO *pData );         /* set special mnemonics in appointed address */
VOID  ParaTaxTableRead( PARATAXTBL *pData );            /* read all tax table */
VOID  ParaTaxTableWrite( PARATAXTBL *pData );           /* set all tax table */
VOID  ParaAdjMnemoRead( PARAADJMNEMO *pData );          /* read adjective mnemonics of appointed address */
VOID  ParaAdjMnemoWrite( PARAADJMNEMO *pData );         /* set adjective mnemonics in appointed address */
VOID  ParaPrtModiRead( PARAPRTMODI *pData );            /* read print modifier mnemonics of appointed address */
VOID  ParaPrtModiWrite( PARAPRTMODI *pData );           /* set print modifier mnemonics in appointed address */
VOID  ParaMajorDEPTRead( PARAMAJORDEPT *pData );        /* read major department mnemonics of appointed address */
VOID  ParaMajorDEPTWrite( PARAMAJORDEPT *pData );       /* set major department mnemonics in appointed address */
VOID  ParaChar24Read( PARACHAR24 *pData );              /* read 24 character mnemonics of appointed address */
VOID  ParaChar24Write( PARACHAR24 *pData );             /* set 24 characters mnemonics in appointed address */
VOID  ParaPCIFRead( PARAPCIF *pData );                  /* read PC password mnemonics of appointed address */
VOID  ParaPCIFWrite( PARAPCIF *pData );                 /* set PC password mnemonics in appointed address */
VOID  ParaCtlTblMenuRead( PARACTLTBLMENU *pData );      /* read control menu page no.*/
VOID  ParaCtlTblMenuWrite( PARACTLTBLMENU *pData );     /* set control menu page no. */
VOID  ParaAutoAltKitchRead( PARAALTKITCH *pData );      /* read kitchen printer alternative list */
VOID  ParaAutoAltKitchWrite( PARAALTKITCH *pData );     /* set kitchen printer alternative */
VOID  ParaFlexMemRead( PARAFLEXMEM *pData );            /* read a number of record & flag status of PTD */
VOID  ParaFlexMemWrite( PARAFLEXMEM *pData );           /* set a number of record & flag status of PTD  */
VOID  ParaStoRegNoRead( PARASTOREGNO *pData );          /* read store/register no. */
VOID  ParaStoRegNoWrite( PARASTOREGNO *pData );         /* set store/register no. */
VOID  ParaTtlKeyTypRead( PARATTLKEYTYP *pData );        /* read characteristic type of total key */
VOID  ParaTtlKeyTypWrite( PARATTLKEYTYP *pData );       /* set characteristic type of total key */
VOID  ParaTtlKeyCtlRead( PARATTLKEYCTL *pData );        /* read bit decide to characterize total key */
VOID  ParaTtlKeyCtlWrite( PARATTLKEYCTL *pData );       /* set bit decide to characterize total key */
VOID  ParaManuAltKitchRead( PARAALTKITCH *pData );      /* read kitchen printer alternative list */
VOID  ParaManuAltKitchWrite( PARAALTKITCH *pData );     /* set kitchen printer alternative */
VOID  ParaCashABAssignRead( PARACASHABASSIGN *pData );  /* read cashier no. of A/B key */
VOID  ParaCashABAssignWrite( PARACASHABASSIGN *pData ); /* set cashier no. A/B key */
VOID  ParaSpcCoRead( PARASPCCO *pData );                /* read special counter */
VOID  ParaSpcCoWrite( PARASPCCO *pData );               /* set special counter */
VOID  ParaToneCtlRead( PARATONECTL *pData );            /* read tone control data */
VOID  ParaToneCtlWrite( PARATONECTL *pData );           /* set tone control data */
VOID  ParaTareRead( PARATARE *pData );                  /* read tare table (ParaTare.c) */
VOID  ParaTareWrite( PARATARE *pData );                 /* set tare table (ParaTare.c) */
VOID  ParaSharedPrtRead( PARASHAREDPRT *pData );        /* assignment terminal # installingshared printer */
VOID  ParaSharedPrtWrite( PARASHAREDPRT *pData );       /* assignment terminal # installingshared printer */
VOID  ParaPresetAmountRead( PARAPRESETAMOUNT *pData );  /* preset amount for preset cash key (parpamt.c) */
VOID  ParaPresetAmountWrite( PARAPRESETAMOUNT *pData ); /* preset amount for preset cash key (parapamt.c) */
VOID  ParaPigRuleRead( PARAPIGRULE *pData );            /* read pig rule table */
VOID  ParaPigRuleWrite( PARAPIGRULE *pData );           /* set pig rule table */
VOID ParaHotelIdRead( PARAHOTELID *pData );             /* read hotel id */
VOID ParaHotelIdWrite( PARAHOTELID *pData );            /* write hotel id */
VOID  ParaOepRead( PARAOEPTBL *pData );                 /* read  Order Entry Prompt Table */
VOID  ParaOepWrite( PARAOEPTBL *pData );                /* set Order Entry Prompt Table */
VOID  ParaFxDriveRead( PARAFXDRIVE *pData );            /* Read  Drive Through Parameter */
VOID  ParaFxDriveWrite( PARAFXDRIVE *pData );           /* Write Drive Through Parameter */
VOID  ParaServiceTimeRead( PARASERVICETIME *pData );    /* Read  Service Time Parameter */
VOID  ParaServiceTimeWrite( PARASERVICETIME *pData );   /* Write Service Time Parameter */
VOID  ParaLaborCostRead( PARALABORCOST *pData );        /* Read  Labor Cost Value */
VOID  ParaLaborCostWrite( PARALABORCOST *pData );       /* Write Labor Cost Value */
VOID  ParaDispParaRead( PARADISPPARA *pData );          /* Read  Beverage Dispenser Parameter */
VOID  ParaDispParaWrite( PARADISPPARA *pData );         /* Write Beverage Dispenser Parameter */
VOID  ParaTendRead(PARATEND *pData);                    /* read tender para,    V3.3 */
VOID  ParaTendWrite(PARATEND *pData);                   /* set tender para data,V3.3 */
VOID  ParaKdsIpRead(PARAKDSIP *pData);                  /* read KDS IP para,    2172 */
VOID  ParaKdsIpWrite(PARAKDSIP *pData);                 /* set KDS IP para data,2172 */
VOID  ParaRestRead( PARARESTRICTION *pData );           /* read plu rest. data, 2172 */
VOID  ParaRestWrite( PARARESTRICTION *pData );          /* write plu rest. data, 2172 */
VOID  ParaBoundAgeRead( PARABOUNDAGE *pData );          /* read boundary age, 2172 */
VOID  ParaBoundAgeWrite( PARABOUNDAGE *pData );         /* write boundary age, 2172 */
VOID  ParaDeptNoMenuRead( PARADEPTNOMENU *pData );      /* read Dept No.of appointed address, 2172 */
VOID  ParaDeptNoMenuWrite( PARADEPTNOMENU *pData );     /* set Dept No. in appointed address, 2172 */
VOID  ParaMiscParaRead( PARAMISCPARA *pData );          /* read misc. parameter */ /* ### ADD Saratoga Misc (052900) */
VOID  ParaMiscParaWrite( PARAMISCPARA *pData );         /* write misc. parameter *//* ### ADD Saratoga Misc (052900) */


SHORT ParaAllRead( UCHAR uchClass,                      /* read system ram */
                   UCHAR *puchRcvBuffer,
                   USHORT usRcvBufLen,
                   USHORT usStartPointer,
                   USHORT *pusReturnLen );
SHORT ParaAllWrite( UCHAR uchClass,                     /* write system ram */
                    UCHAR *puchWrtBuffer,
                    USHORT usRWrtBufLen,
                    USHORT usStartPointer,
                    USHORT *pusReturnLen );
VOID  ParaRead( VOID *pData );
VOID  ParaWrite( VOID *pData );

/*========= End of Source =========*/
