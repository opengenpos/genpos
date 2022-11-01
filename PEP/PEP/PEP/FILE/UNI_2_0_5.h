
// Define the structures for UNINIRAM for Release 2.0.5
//


#if !defined(INCLUDE_UNI_2_0_5_H)
#define INCLUDE_UNI_2_0_5_H

#include "UNI_2_0_3.h"

//NOTE!:PRIOR TO BUILD 2.0.0.31
//NOTE!:PRIOR TO BUILD 2.0.0.31
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#define MAX_PLUKEY_SIZE_HGV_2_0_5        MAX_PLUKEY_SIZE_HGV_2_0_3       /* Keyed PLU Key */
#define MAX_RNDTBL_SIZE_HGV_2_0_5        MAX_RNDTBL_SIZE_HGV_2_0_3       /* Rounding Table, 2172 */
#define MAX_RND_DATA_HGV_2_0_5           MAX_RND_DATA_HGV_2_0_3          /* Set Data for Rounding */
#define MAX_DISCRATE_SIZE_HGV_2_0_5      MAX_DISCRATE_SIZE_HGV_2_0_3     /* Discount/Bonus % Rates, V3.3 */
#define MAX_TABLE_NO_HGV_2_0_5           MAX_TABLE_NO_HGV_2_0_3          /* OEP TABLE, V3.3 */
#define MAX_TABLE_ADDRESS_HGV_2_0_5      MAX_TABLE_ADDRESS_HGV_2_0_3     /* OEP TABLE */
#define MAX_ONE_OEPTABLE_SIZE_HGV_2_0_5  MAX_ONE_OEPTABLE_SIZE_HGV_2_0_3   /* OEP TABLE */
#define MAX_SOFTCHK_NO_HGV_2_0_5         MAX_SOFTCHK_NO_HGV_2_0_3        /* 42 Characters */

#define MAX_PROMO_NO_HGV_2_0_5           MAX_PROMO_NO_HGV_2_0_3          /* Line for Promotion Message */

#define MAX_FC_SIZE_HGV_2_0_5            MAX_FC_SIZE_HGV_2_0_3           /* Currency Conversion Rate, 2172 */
#define MAX_SETMENU_NO_HGV_2_0_5         MAX_SETMENU_NO_HGV_2_0_3        /* Set Menu */
#define MAX_SETPLU_DATA_HGV_2_0_5        MAX_SETPLU_DATA_HGV_2_0_3       /* Set PLU  */
#define MAX_TAXRATE_SIZE_HGV_2_0_5       MAX_TAXRATE_SIZE_HGV_2_0_3      /* Tax Rate */

#define MAX_MDC_SIZE_HGV_2_0_5         MAX_MDC_SIZE_HGV_2_0_3        /* MDC Data for 1byte Data, 2172 */
#define MAX_FSC_TBL_HGV_2_0_5          MAX_FSC_TBL_HGV_2_0_3         /* Function Key Table *///RPH 12-2-3
#define MAX_FSC_NO_HGV_2_0_5           MAX_FSC_NO_HGV_2_0_3          /* Function Key 165->300 JHHJ Database Changees 3-17-04*/
#define MAX_FSC_DATA_HGV_2_0_5         MAX_FSC_DATA_HGV_2_0_3        /* Major FSC Data/Minor FSC Data */
#define MAX_SUPLEVEL_SIZE_HGV_2_0_5    MAX_SUPLEVEL_SIZE_HGV_2_0_3   /* Supervisor Level */
#define MAX_ACSEC_SIZE_HGV_2_0_5       MAX_ACSEC_SIZE_HGV_2_0_3      /* Action Code Security for Supervisor */
#define MAX_HALOKEY_NO_HGV_2_0_5       MAX_HALOKEY_NO_HGV_2_0_3     /* Key for setting HALO */
#define MAX_HOUR_NO_HGV_2_0_5          MAX_HOUR_NO_HGV_2_0_3        /* Data for setting Hourly Activity Time, V3.3 */
#define MAX_HM_DATA_HGV_2_0_5          MAX_HM_DATA_HGV_2_0_3        /* Hour/Minute */
#define MAX_SLIP_SIZE_HGV_2_0_5        MAX_SLIP_SIZE_HGV_2_0_3      /* Slip Printer Feed Control */
#define MAX_TRANSM_NO_HGV_2_0_5        MAX_TRANSM_NO_HGV_2_0_3      /* Transaction Memonics, 2172 */

#define MAX_LEAD_NO_HGV_2_0_5          MAX_LEAD_NO_HGV_2_0_3         /* Max number of Lead Through mnemonics */
#define MAX_REPO_NO_HGV_2_0_5          MAX_REPO_NO_HGV_2_0_3         /* Report Name */
#define MAX_SPEM_NO_HGV_2_0_5          MAX_SPEM_NO_HGV_2_0_3         /* Special Mnemonics, 2172 */
#define MAX_TTBLE_SIZE_HGV_2_0_5       MAX_TTBLE_SIZE_HGV_2_0_3      /* Ram Fix Size for setting Tax Rate */
#define MAX_TTBLDATA_SIZE_HGV_2_0_5    MAX_TTBLDATA_SIZE_HGV_2_0_3   /* Tax Data Table Size for Tax Table */
#define MAX_TTBLHEAD_SIZE_HGV_2_0_5    MAX_TTBLHEAD_SIZE_HGV_2_0_3   /* Header Size for Tax Table */
#define MAX_TTBLRATE_SIZE_HGV_2_0_5    MAX_TTBLRATE_SIZE_HGV_2_0_3   /* WDC & RATE Size for Tax Table */
#define MAX_ADJM_NO_HGV_2_0_5          MAX_ADJM_NO_HGV_2_0_3         /* Adjective Mneminics */
#define MAX_PRTMODI_NO_HGV_2_0_5       MAX_PRTMODI_NO_HGV_2_0_3      /* Print Modifier */

#define MAX_MDEPT_NO_HGV_2_0_5         MAX_MDEPT_NO_HGV_2_0_3        /* Major Department Mnemonics */
#define MAX_CH24_NO_HGV_2_0_5          MAX_CH24_NO_HGV_2_0_3         /* 24 Characters, 2172 */
#define MAX_PAGE_SIZE_HGV_2_0_5        MAX_PAGE_SIZE_HGV_2_0_3       /* Control Menu Page, */ //RPH 12-2-3 TOUCHSCREEN
#define MAX_DEST_SIZE_HGV_2_0_5        MAX_DEST_SIZE_HGV_2_0_3       /* Destination Kitchen Printer */
#define MAX_FLXMEM_SIZE_HGV_2_0_5      MAX_FLXMEM_SIZE_HGV_2_0_3     /* Flexible Memory Assignment, with MLD File */
#define MAX_STOREG_SIZE_HGV_2_0_5      MAX_STOREG_SIZE_HGV_2_0_3     /* Store/Register No. */

#define MAX_KEYTYPE_SIZE_HGV_2_0_5     MAX_KEYTYPE_SIZE_HGV_2_0_3    /* Total Keys are in need of type assign */
#define MAX_TTLKEY_NO_HGV_2_0_5        MAX_TTLKEY_NO_HGV_2_0_3       /* The number of Total Key */
#define MAX_TTLKEY_DATA_HGV_2_0_5      MAX_TTLKEY_DATA_HGV_2_0_3     /* The number of bit */
#define MAX_AB_SIZE_HGV_2_0_5          MAX_AB_SIZE_HGV_2_0_3         /* Cashier A/Cashier B */
#define MAX_CO_SIZE_HGV_2_0_5          MAX_CO_SIZE_HGV_2_0_3         /* Special counter */
#define MAX_DEPTSTS_SIZE_HGV_2_0_5     MAX_DEPTSTS_SIZE_HGV_2_0_3    /* DEPT Status Field Size R3.1 */
#define MAX_CPNTARGET_SIZE_HGV_2_0_5   MAX_CPNTARGET_SIZE_HGV_2_0_3  /* Combination Coupon Target Menu Size */
#define MAX_CPNSTS_SIZE_HGV_2_0_5      MAX_CPNSTS_SIZE_HGV_2_0_3     /* Combination Coupon Status Field Size */
#define MAX_PLUSTS_SIZE_HGV_2_0_5      MAX_PLUSTS_SIZE_HGV_2_0_3     /* PLU Status Field Size R3.1 */
#define MAX_ADJECTIVE_NO_HGV_2_0_5     MAX_ADJECTIVE_NO_HGV_2_0_3    /* The number of Adjective Price */
#define MAX_DEPT_NO_HGV_2_0_5          MAX_DEPT_NO_HGV_2_0_3         /* Max Size of PLU Number */
#define MAX_PLU_NO_HGV_2_0_5           MAX_PLU_NO_HGV_2_0_3          /* Max Size of PLU Number */
#define MAX_PLUPRICE_SIZE_HGV_2_0_5    MAX_PLUPRICE_SIZE_HGV_2_0_3   /* Max Size of 1 PLU Price Save Area */
#define MAX_PCIF_SIZE_HGV_2_0_5        MAX_PCIF_SIZE_HGV_2_0_3       /* Max Size of Password for PC I/F */
#define MAX_TONE_SIZE_HGV_2_0_5        MAX_TONE_SIZE_HGV_2_0_3       /* Max Size of Tone control */
#define MAX_TARE_SIZE_HGV_2_0_5        MAX_TARE_SIZE_HGV_2_0_3       /* Max Size of Tare table   */
#define MAX_SHAREDPRT_SIZE_HGV_2_0_5   MAX_SHAREDPRT_SIZE_HGV_2_0_3  /* Max Size of Shared Printer, 2172 */
#define MAX_EMPLOYEENO_HGV_2_0_5       MAX_EMPLOYEENO_HGV_2_0_3      /* Max Employee No */
#define MAX_JOB_CODE_HGV_2_0_5         MAX_JOB_CODE_HGV_2_0_3        /* Max Job Code */
#define MAX_JOB_CODE_ADDR_HGV_2_0_5    MAX_JOB_CODE_ADDR_HGV_2_0_3   /* Max Job Code Address */
#define MAX_DISPPARA_OFFSET_HGV_2_0_5  MAX_DISPPARA_OFFSET_HGV_2_0_3  /* Max Offset Code of Beverage Dispenser */
#define MAX_DISPPARA_TENDER_HGV_2_0_5  MAX_DISPPARA_OFFSET_HGV_2_0_3  /* JHHJ 3-16-04 Max Tender Key Data of Beverage Dispenser MAX_DISPPARA_TENDER_HGV_2_0_3 == 11 */
#define MAX_DISPPARA_TOTAL_HGV_2_0_5   MAX_DISPPARA_TOTAL_HGV_2_0_3   /* Max Total Key Data of Beverage Dispenser */
#define MAX_TEND_NO_HGV_2_0_5          MAX_TEND_NO_HGV_2_0_3          /* JHHJ 3-16-04 Key for Tender Para, V3.3 */

#define MAX_PRESETAMOUNT_SIZE_HGV_2_0_5 MAX_PRESETAMOUNT_SIZE_HGV_2_0_3   /* Max Size of Preset Amount Cash Tender */
#define MAX_PIGRULE_SIZE_HGV_2_0_5      MAX_PIGRULE_SIZE_HGV_2_0_3        /* Max Size of Pig Rule Table */
#define MAX_HOTELID_SIZE_HGV_2_0_5      MAX_HOTELID_SIZE_HGV_2_0_3        /* HOTEL ID OR SLD */
#define MAX_HOTELID_NO_HGV_2_0_5        MAX_HOTELID_NO_HGV_2_0_3          /* HOTEL ID AND SLD */

#define MAX_AUTOCOUPON_SIZE_HGV_2_0_5     8

#define MAX_FLEXDRIVE_SIZE_HGV_2_0_5     MAX_FLEXDRIVE_SIZE_HGV_2_0_3      /* Max Size of SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER TABLE */
#define MAX_FXDRIVE_TERM_SIZE_HGV_2_0_5  MAX_FXDRIVE_TERM_SIZE_HGV_2_0_3   /* Max Terminal# of FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL */
#define MAX_PARACSTR_SIZE_HGV_2_0_5      MAX_PARACSTR_SIZE_HGV_2_0_3       /* Max One Control String Size MAX_PARACSTR_SIZE_HGV_2_0_3 == 200 */

#define MAX_PARAPPI_SIZE_HGV_2_0_5       MAX_PARAPPI_SIZE_HGV_2_0_3        /* Max One PPI Price Level */
#define MAX_SERVICETIME_SIZE_HGV_2_0_5   MAX_SERVICETIME_SIZE_HGV_2_0_3    /* Max Size of Service Time Parameter */
#define MAX_PROGRPT_FORMAT_HGV_2_0_5     MAX_PROGRPT_FORMAT_HGV_2_0_3      /* Max no. of prog. report format */
#define MAX_PROGRPT_EODPTD_HGV_2_0_5     MAX_PROGRPT_EODPTD_HGV_2_0_3      /* Max no. of record at EOD/PTD */
#define MAX_PROGRPT_PTD_HGV_2_0_5        MAX_PROGRPT_PTD_HGV_2_0_3         /* Max no. of record at PTD */
#define MAX_LABORCOST_SIZE_HGV_2_0_5     MAX_LABORCOST_SIZE_HGV_2_0_3      /* Max no. of labor cost value */
#define MAX_DISPPARA_SIZE_HGV_2_0_5      MAX_DISPPARA_SIZE_HGV_2_0_3       /* Max no. of beverage dispenser parameter */

#define MAX_MLDMNEMO_SIZE_HGV_2_0_5      MAX_MLDMNEMO_SIZE_HGV_2_0_3       /* Max no. of multi-line display parameter */

#define MAX_KDSIP_SIZE_HGV_2_0_5         MAX_KDSIP_SIZE_HGV_2_0_3          /* KDS IP Address, 2172 */
#define PARA_KDSIP_LEN_HGV_2_0_5         PARA_KDSIP_LEN_HGV_2_0_3          /* KDS IP Address, 2172 */
#define MAX_KDSIP_INPUT_LEN_HGV_2_0_5    MAX_KDSIP_INPUT_LEN_HGV_2_0_3     /* KDS IP Address for input, 2172 */

#define MAX_REST_SIZE_HGV_2_0_5          MAX_REST_SIZE_HGV_2_0_3           /* PLU Sales Restriction Table  */
#define MAX_AGE_SIZE_HGV_2_0_5           MAX_AGE_SIZE_HGV_2_0_3            /* Boundary age  */
#define MAX_DEPTKEY_SIZE_HGV_2_0_5       MAX_DEPTKEY_SIZE_HGV_2_0_3        /* Keyed Dept Key, 2172 */
#define MAX_MISC_SIZE_HGV_2_0_5          MAX_MISC_SIZE_HGV_2_0_3           /* Misc. Parameter */

#define MAX_TERMINALINFO_SIZE_HGV_2_0_5  MAX_TERMINALINFO_SIZE_HGV_2_0_3   /* Max number of terminals in cluster for terminal info */

#define PARA_TRANSMNEMO_LEN_HGV_2_0_5	 PARA_TRANSMNEMO_LEN_HGV_2_0_3

#define PARA_PROMOTION_LEN_HGV_2_0_5     PARA_PROMOTION_LEN_HGV_2_0_3      /* ParaPromotion */
#define PARA_TRANSMNEMO_LEN_HGV_2_0_5    PARA_TRANSMNEMO_LEN_HGV_2_0_3     /* ParaTransMnemo */
#define PARA_LEADTHRU_LEN_HGV_2_0_5      PARA_LEADTHRU_LEN_HGV_2_0_3       /* ParaLeadThru */
#define PARA_REPORTNAME_LEN_HGV_2_0_5    PARA_REPORTNAME_LEN_HGV_2_0_3     /* ParaReportName */
#define PARA_SPEMNEMO_LEN_HGV_2_0_5      PARA_SPEMNEMO_LEN_HGV_2_0_3       /* ParaSpeMnemo */
#define PARA_ADJMNEMO_LEN_HGV_2_0_5      PARA_ADJMNEMO_LEN_HGV_2_0_3       /* ParaAdjMnemo */
#define PARA_PRTMODI_LEN_HGV_2_0_5       PARA_PRTMODI_LEN_HGV_2_0_3        /* ParaPrtModi */
#define PARA_MAJORDEPT_LEN_HGV_2_0_5     PARA_MAJORDEPT_LEN_HGV_2_0_3      /* ParaMajorDEPT */
#define PARA_CHAR24_LEN_HGV_2_0_5        PARA_CHAR24_LEN_HGV_2_0_3         /* ParaChar24 */

#define PARA_SOFTCHK_LEN_HGV_2_0_5       PARA_SOFTCHK_LEN_HGV_2_0_3      /* ParaSoftCheck */


#define PLU_MAX_DIGIT_HGV_2_0_5         PLU_MAX_DIGIT_HGV_2_0_3       // maximum number of digits in a PLU number

#define PARA_HOTELID_LEN_HGV_2_0_5      PARA_HOTELID_LEN_HGV_2_0_3    /* ParaHotelId Length */
#define PARA_SLD_LEN_HGV_2_0_5          PARA_SLD_LEN_HGV_2_0_3        /* ParaHotelId's Sld Length */
#define PARA_PDT_LEN_HGV_2_0_5          PARA_PDT_LEN_HGV_2_0_3        /* ParaHotelId's Product Length */


// following defines are extra defines for special uses in this file only
#define PARA_TEND_CPM_NO_HGV_2_0_5      PARA_TEND_CPM_NO_HGV_2_0_3    // the number of auchTend elements in CPMDATA for Release 2.0.0





typedef struct  {
        WCHAR   auchHotelId[PARA_HOTELID_LEN_HGV_2_0_5];    /* Hotel Id save Area       */
        WCHAR   auchSld[PARA_SLD_LEN_HGV_2_0_5];            /* SLD Id save area         */
        WCHAR   auchProductId[PARA_PDT_LEN_HGV_2_0_5];      /* Product Id save area     */
        WCHAR   auchTend[PARA_TEND_CPM_NO_HGV_2_0_5];           /* Tender 1 to 11 EPT Action code */
        WCHAR   auchApproval[2];                            /* Approval code save area  */
}CPMDATA_HGV_2_0_5;

// Terminal information structures to allow for the setting and retrieving
// of a particular terminal's information.
typedef struct {
	ULONG     ulCodePage;          // code page to use for MBCS, other conversions from UNICODE
	USHORT    usLanguageID;        // Primary language id to use on this terminal
	USHORT    usSubLanguageID;     // Sub-language id to use on this terminal
	USHORT    usStoreNumber;       // The store number for this terminal
	USHORT    usRegisterNumber;    // the number of this particular terminal or register
	UCHAR     uchKeyBoardType;      // keyboard type such as KEYBOARD_CONVENTION
	UCHAR     uchMenuPageDefault;   // Default menu page for this terminal (10 for touch)
	WCHAR     aszDefaultFontFamily[32];   // Font Family to use as a default
	WCHAR     aszUnLockCode[12];            // software security unlock code for terminal
	WCHAR     aszTerminalSerialNumber[16];  // serial number of the terminal
	WCHAR     aszDefaultMenuFileName[32];   // name of file containing default menu for this terminal
	UCHAR     ucIpAddress[4];      // IP address of the terminal
} TERMINALINFO_HGV_2_0_5;


typedef struct  {
        UCHAR  uchAdjective;          /* Adjective Number */
        UCHAR  uchModStat;            /* Modifier Status */
        WCHAR  aszPLUNumber[PLU_MAX_DIGIT_HGV_2_0_5+1];  /* PLU Number */
}PLUNO_HGV_2_0_5;


typedef struct  {
        ULONG  ulRecordNumber;        /* Number of Record, 2172 */
        UCHAR  uchPTDFlag;            /* PTD Provide Flag */
}FLEXMEM_HGV_2_0_5;

typedef struct  {
        UCHAR   uchFsc;     /* FSC Code */
        UCHAR   uchExt;     /* Extend Code */
}FSCDATA_HGV_2_0_5;

typedef struct {
        FSCDATA_HGV_2_0_5  TblFsc[MAX_FSC_NO_HGV_2_0_5];    /* One of Page */
}PARAFSCTBL_HGV_2_0_5;

typedef struct  {
        USHORT usPluNumber;          /* PLU Number */
        UCHAR  uchAdjective;         /* Adjective Code */
}DISPPARA_HGV_2_0_5;

typedef struct  {
        USHORT usSuperNumber;         /* Supervisor Number */
        UCHAR  uchSecret;              /* Secret Number */
}SUPLEVEL_HGV_2_0_5;


typedef struct  {
        UCHAR  uchDate;               /* Date */
        UCHAR  uchDay;                /* Day of Week */
        UCHAR  uchHour1;              /* Start Hour */
        UCHAR  uchMin1;               /* Start Minute */
        UCHAR  uchHour2;              /* End Hour */
        UCHAR  uchMin2;               /* End Minute */
}RESTRICT_HGV_2_0_5;

typedef struct {
	USHORT usMethod;			// access method used, ethernet, serial, etc
	USHORT usPortNo;			// port number of the charge post gateway
	UCHAR  ucIpAddress[4];      // IP address of the charge post gateway
} CHARGEPOSTINFO_HGV_2_0_5;

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
} TENDERKEYINFO_HGV_2_0_5;

typedef struct  {
    UCHAR  uchModStat;                                 /* Modifier Status */
    WCHAR  aszPLUNumber[PLU_MAX_DIGIT_HGV_2_0_5 + 1];  /* PLU Number */
    UCHAR  uchAdjective;                               /* Adjective Code of PLU */
}MENUPLU_HGV_2_0_5;

typedef struct  {
	UCHAR	uchIpAddress[PARA_KDSIP_LEN_HGV_2_0_5];    /* IP Address  */
	USHORT	usPort;                                    /* Port Address of KDS  */
	USHORT	usNHPOSPort;                               /* Port Address of Interface port with NHPOS */
}PARAKDS_HGV_2_0_5;




typedef struct	tagUNINTRAM_HGV_2_0_5 {
	//  szVer is removed because it is not part of the PEP data file.
	//  it is part of the UNINIRAM struct in the terminal only.
//	WCHAR       szVer [FILE_LEN_ID];                 /* 10-1-2004 JHHJ for file version information*/
    UCHAR       ParaMDC [MAX_MDC_SIZE_HGV_2_0_5];               /* prog #1 */
    FLEXMEM_HGV_2_0_5     ParaFlexMem [MAX_FLXMEM_SIZE_HGV_2_0_5];        /* prog #2 */
    PARAFSCTBL_HGV_2_0_5  ParaFSC [MAX_FSC_TBL_HGV_2_0_5];                /* prog #3 */
    USHORT      ParaSecurityNo;                                 /* prog #6 */
    USHORT      ParaStoRegNo [MAX_STOREG_SIZE_HGV_2_0_5];       /* prog #7 */
    SUPLEVEL_HGV_2_0_5    ParaSupLevel [MAX_SUPLEVEL_SIZE_HGV_2_0_5];     /* prog #8 */
    UCHAR       ParaActCodeSec [MAX_ACSEC_SIZE_HGV_2_0_5];      /* prog #9 */
    UCHAR       ParaTransHALO [MAX_HALOKEY_NO_HGV_2_0_5];       /* prog #10 */
    UCHAR       ParaHourlyTime [MAX_HOUR_NO_HGV_2_0_5] [MAX_HM_DATA_HGV_2_0_5];             /* prog #17 */
    UCHAR       ParaSlipFeedCtl [MAX_SLIP_SIZE_HGV_2_0_5];                                  /* prog #18 */
    WCHAR       ParaTransMnemo [MAX_TRANSM_NO_HGV_2_0_5] [PARA_TRANSMNEMO_LEN_HGV_2_0_5];   /* prog #20 */
    WCHAR       ParaLeadThru [MAX_LEAD_NO_HGV_2_0_5] [PARA_LEADTHRU_LEN_HGV_2_0_5];       /* prog #21 */
    WCHAR       ParaReportName [MAX_REPO_NO_HGV_2_0_5] [PARA_REPORTNAME_LEN_HGV_2_0_5];   /* prog #22 */
    WCHAR       ParaSpeMnemo [MAX_SPEM_NO_HGV_2_0_5] [PARA_SPEMNEMO_LEN_HGV_2_0_5];       /* prog #23 */
    WCHAR       ParaPCIF [MAX_PCIF_SIZE_HGV_2_0_5];                                    /* prog #39 */
    WCHAR       ParaAdjMnemo [MAX_ADJM_NO_HGV_2_0_5] [PARA_ADJMNEMO_LEN_HGV_2_0_5];    /* prog #46 */
    WCHAR       ParaPrtModi [MAX_PRTMODI_NO_HGV_2_0_5] [PARA_PRTMODI_LEN_HGV_2_0_5];	/* prog #47 */
    WCHAR       ParaMajorDEPT [MAX_MDEPT_NO_HGV_2_0_5] [PARA_MAJORDEPT_LEN_HGV_2_0_5];  /* prog #48 */
    UCHAR       ParaAutoAltKitch [MAX_DEST_SIZE_HGV_2_0_5];                             /* prog #49 */
    CPMDATA_HGV_2_0_5    ParaHotelId;                                                   /* prog #54 */
    WCHAR       ParaChar24 [MAX_CH24_NO_HGV_2_0_5] [PARA_CHAR24_LEN_HGV_2_0_5];         /* prog #57 */
    UCHAR       ParaTtlKeyTyp [MAX_KEYTYPE_SIZE_HGV_2_0_5];                             /* prog #60 */
    UCHAR       ParaTtlKeyCtl [MAX_TTLKEY_NO_HGV_2_0_5] [MAX_TTLKEY_DATA_HGV_2_0_5];    /* prog #61 */
    UCHAR       ParaTend [MAX_TEND_NO_HGV_2_0_5];                                       /* prog #62 */
    PLUNO_HGV_2_0_5       ParaPLUNoMenu [MAX_PLUKEY_SIZE_HGV_2_0_5];                              /* A/C #4, 2172 */
    UCHAR       ParaCtlTblMenu [MAX_PAGE_SIZE_HGV_2_0_5];                               /* A/C #5 */
    UCHAR       ParaManuAltKitch [MAX_DEST_SIZE_HGV_2_0_5];                            /* A/C #6 */
    USHORT      ParaCashABAssign [MAX_AB_SIZE_HGV_2_0_5];                              /* A/C #7 */
    UCHAR       ParaRound [MAX_RNDTBL_SIZE_HGV_2_0_5][MAX_RND_DATA_HGV_2_0_5];         /* A/C #84 */
    UCHAR       ParaDisc [MAX_DISCRATE_SIZE_HGV_2_0_5];                                 /* A/C #86 */
    WCHAR       ParaPromotion [MAX_PROMO_NO_HGV_2_0_5] [PARA_PROMOTION_LEN_HGV_2_0_5];	/* A/C #88 */
    ULONG       ParaCurrency [MAX_FC_SIZE_HGV_2_0_5];                                   /* A/C #89 */
    UCHAR       ParaToneCtl [MAX_TONE_SIZE_HGV_2_0_5];                                  /* A/C #169 */
    MENUPLU_HGV_2_0_5     ParaMenuPLU [MAX_SETMENU_NO_HGV_2_0_5] [MAX_SETPLU_DATA_HGV_2_0_5];     /* A/C #116, 2172 */
    UCHAR       ParaTaxTable [MAX_TTBLE_SIZE_HGV_2_0_5];                                /* A/C #124-126 */
    ULONG       ParaTaxRate [MAX_TAXRATE_SIZE_HGV_2_0_5];                               /* A/C #127 */
    ULONG       ParaMiscPara [MAX_MISC_SIZE_HGV_2_0_5];                                 /* MISC. PARAMETER */
	USHORT      ParaTare [MAX_TARE_SIZE_HGV_2_0_5];                          /* A/C #111 */
    ULONG       ParaPresetAmount [MAX_PRESETAMOUNT_SIZE_HGV_2_0_5];          /* prog #15 */
    UCHAR       ParaSharedPrt [MAX_SHAREDPRT_SIZE_HGV_2_0_5];                /* prog #50 */
    WCHAR       ParaSoftChk [MAX_SOFTCHK_NO_HGV_2_0_5] [PARA_SOFTCHK_LEN_HGV_2_0_5];     /* A/C #87 */
    ULONG       ParaPigRule [MAX_PIGRULE_SIZE_HGV_2_0_5];                                /* A/C #130 */
    UCHAR       ParaOep [MAX_TABLE_NO_HGV_2_0_5] [MAX_TABLE_ADDRESS_HGV_2_0_5];   /* A/C #160 */
    UCHAR       ParaFlexDrive [MAX_FLEXDRIVE_SIZE_HGV_2_0_5];                     /* A/C #162 */
    USHORT      ParaServiceTime [MAX_SERVICETIME_SIZE_HGV_2_0_5];                 /* A/C #133 */
    USHORT	    ParaLaborCost[MAX_LABORCOST_SIZE_HGV_2_0_5];                      /* A/C #154 */
    DISPPARA_HGV_2_0_5    ParaDispPara [MAX_DISPPARA_SIZE_HGV_2_0_5];                       /* A/C #137 */
	PARAKDS_HGV_2_0_5     ParaKdsIp [MAX_KDSIP_SIZE_HGV_2_0_5];                             /* SR 14 ESMITH */
	RESTRICT_HGV_2_0_5    ParaRestriction [MAX_REST_SIZE_HGV_2_0_5];               /* AC 170, 2172 */
    UCHAR       auchParaBoundAge [MAX_AGE_SIZE_HGV_2_0_5];               /* A/C 208, 2172 */
    USHORT      ParaDeptNoMenu [MAX_DEPTKEY_SIZE_HGV_2_0_5];             /* A/C #4, 2172 */
	USHORT      ParaSpcCo [MAX_CO_SIZE_HGV_2_0_5];                       /* SPECIAL COUNTER */
	ULONG	    ParaAutoCoupon[MAX_AUTOCOUPON_SIZE_HGV_2_0_5];					/* Prog. #67  ACC Controls */ //RLZ

	CHARGEPOSTINFO_HGV_2_0_5    ChargePostInformation;                        // R_20_DATABASECHANGE
	TERMINALINFO_HGV_2_0_5      TerminalInformation[MAX_TERMINALINFO_SIZE_HGV_2_0_5];
	CHARGEPOSTINFO_HGV_2_0_5    EPT_ENH_Information[16];                      // R_20_DATABASECHANGE
	TENDERKEYINFO_HGV_2_0_5     TenderKeyInformation[MAX_TEND_NO_HGV_2_0_5];            // R_20_DATABASECHANGE
    ULONG                       ParaStoreForward[MAX_MISC_SIZE_HGV_2_0_5];    // area used for the Store and Forward functionality

} UNINIRAM_HGV_2_0_5;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif