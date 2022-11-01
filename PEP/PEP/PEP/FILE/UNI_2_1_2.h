
// Define the structures for UNINIRAM for Release 2.0.3
//


#if !defined(INCLUDE_UNI_2_1_2_H)
#define INCLUDE_UNI_2_1_2_H

#include "UNI_2_0_5.h"

//NOTE!:PRIOR TO BUILD 2.0.0.31
//NOTE!:PRIOR TO BUILD 2.0.0.31
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#define MAX_PLUKEY_SIZE_HGV_2_1_2        MAX_PLUKEY_SIZE_HGV_2_0_5       /* Keyed PLU Key */
#define MAX_RNDTBL_SIZE_HGV_2_1_2        17   /* Rounding Table, 2172 */	//changed from 15 in version 2.1.2
#define MAX_RND_DATA_HGV_2_1_2           MAX_RND_DATA_HGV_2_0_5          /* Set Data for Rounding */
#define MAX_DISCRATE_SIZE_HGV_2_1_2      120  /* Discount/Bonus % Rates, V3.3 */	//changed from 20 in version 2.1.2
#define MAX_TABLE_NO_HGV_2_1_2           MAX_TABLE_NO_HGV_2_0_5          /* OEP TABLE, V3.3 */
#define MAX_TABLE_ADDRESS_HGV_2_1_2      MAX_TABLE_ADDRESS_HGV_2_0_5     /* OEP TABLE */
#define MAX_ONE_OEPTABLE_SIZE_HGV_2_1_2  MAX_ONE_OEPTABLE_SIZE_HGV_2_0_5   /* OEP TABLE */
#define MAX_SOFTCHK_NO_HGV_2_1_2         MAX_SOFTCHK_NO_HGV_2_0_5        /* 42 Characters */

#define MAX_PROMO_NO_HGV_2_1_2           MAX_PROMO_NO_HGV_2_0_5          /* Line for Promotion Message */

#define MAX_FC_SIZE_HGV_2_1_2            MAX_FC_SIZE_HGV_2_0_5           /* Currency Conversion Rate, 2172 */
#define MAX_SETMENU_NO_HGV_2_1_2         MAX_SETMENU_NO_HGV_2_0_5        /* Set Menu */
#define MAX_SETPLU_DATA_HGV_2_1_2        MAX_SETPLU_DATA_HGV_2_0_5       /* Set PLU  */
#define MAX_TAXRATE_SIZE_HGV_2_1_2       MAX_TAXRATE_SIZE_HGV_2_0_5      /* Tax Rate */

#define MAX_MDC_SIZE_HGV_2_1_2         MAX_MDC_SIZE_HGV_2_0_5        /* MDC Data for 1byte Data, 2172 */
#define MAX_FSC_TBL_HGV_2_1_2          MAX_FSC_TBL_HGV_2_0_5         /* Function Key Table *///RPH 12-2-3
#define MAX_FSC_NO_HGV_2_1_2           MAX_FSC_NO_HGV_2_0_5          /* Function Key 165->300 JHHJ Database Changees 3-17-04*/
#define MAX_FSC_DATA_HGV_2_1_2         MAX_FSC_DATA_HGV_2_0_5        /* Major FSC Data/Minor FSC Data */
#define MAX_SUPLEVEL_SIZE_HGV_2_1_2    22	 /* Supervisor Level */		//changed from 8 in version 2.1.2
#define MAX_ACSEC_SIZE_HGV_2_1_2       MAX_ACSEC_SIZE_HGV_2_0_5      /* Action Code Security for Supervisor */
#define MAX_HALOKEY_NO_HGV_2_1_2       50    /* Key for setting HALO */	//changed from 36 in version 2.1.2
#define MAX_HOUR_NO_HGV_2_1_2          MAX_HOUR_NO_HGV_2_0_5        /* Data for setting Hourly Activity Time, V3.3 */
#define MAX_HM_DATA_HGV_2_1_2          MAX_HM_DATA_HGV_2_0_5        /* Hour/Minute */
#define MAX_SLIP_SIZE_HGV_2_1_2        MAX_SLIP_SIZE_HGV_2_0_5      /* Slip Printer Feed Control */
#define MAX_TRANSM_NO_HGV_2_1_2        MAX_TRANSM_NO_HGV_2_0_5      /* Transaction Memonics, 2172 */

#define MAX_LEAD_NO_HGV_2_1_2          MAX_LEAD_NO_HGV_2_0_5         /* Max number of Lead Through mnemonics */
#define MAX_REPO_NO_HGV_2_1_2          MAX_REPO_NO_HGV_2_0_5         /* Report Name */
#define MAX_SPEM_NO_HGV_2_1_2          MAX_SPEM_NO_HGV_2_0_5         /* Special Mnemonics, 2172 */
#define MAX_TTBLE_SIZE_HGV_2_1_2       MAX_TTBLE_SIZE_HGV_2_0_5      /* Ram Fix Size for setting Tax Rate */
#define MAX_TTBLDATA_SIZE_HGV_2_1_2    MAX_TTBLDATA_SIZE_HGV_2_0_5   /* Tax Data Table Size for Tax Table */
#define MAX_TTBLHEAD_SIZE_HGV_2_1_2    MAX_TTBLHEAD_SIZE_HGV_2_0_5   /* Header Size for Tax Table */
#define MAX_TTBLRATE_SIZE_HGV_2_1_2    MAX_TTBLRATE_SIZE_HGV_2_0_5   /* WDC & RATE Size for Tax Table */
#define MAX_ADJM_NO_HGV_2_1_2          MAX_ADJM_NO_HGV_2_0_5         /* Adjective Mneminics */
#define MAX_PRTMODI_NO_HGV_2_1_2       MAX_PRTMODI_NO_HGV_2_0_5      /* Print Modifier */

#define MAX_MDEPT_NO_HGV_2_1_2         MAX_MDEPT_NO_HGV_2_0_5        /* Major Department Mnemonics */
#define MAX_CH24_NO_HGV_2_1_2          MAX_CH24_NO_HGV_2_0_5         /* 24 Characters, 2172 */
#define MAX_PAGE_SIZE_HGV_2_1_2        MAX_PAGE_SIZE_HGV_2_0_5       /* Control Menu Page, */ //RPH 12-2-3 TOUCHSCREEN
#define MAX_DEST_SIZE_HGV_2_1_2        MAX_DEST_SIZE_HGV_2_0_5       /* Destination Kitchen Printer */
#define MAX_FLXMEM_SIZE_HGV_2_1_2      MAX_FLXMEM_SIZE_HGV_2_0_5     /* Flexible Memory Assignment, with MLD File */
#define MAX_STOREG_SIZE_HGV_2_1_2      MAX_STOREG_SIZE_HGV_2_0_5     /* Store/Register No. */

#define MAX_KEYTYPE_SIZE_HGV_2_1_2     MAX_KEYTYPE_SIZE_HGV_2_0_5    /* Total Keys are in need of type assign */
#define MAX_TTLKEY_NO_HGV_2_1_2        MAX_TTLKEY_NO_HGV_2_0_5       /* The number of Total Key */
#define MAX_TTLKEY_DATA_HGV_2_1_2      MAX_TTLKEY_DATA_HGV_2_0_5     /* The number of bit */
#define MAX_AB_SIZE_HGV_2_1_2          MAX_AB_SIZE_HGV_2_0_5         /* Cashier A/Cashier B */
#define MAX_CO_SIZE_HGV_2_1_2          MAX_CO_SIZE_HGV_2_0_5         /* Special counter */
#define MAX_DEPTSTS_SIZE_HGV_2_1_2     MAX_DEPTSTS_SIZE_HGV_2_0_5    /* DEPT Status Field Size R3.1 */
#define MAX_CPNTARGET_SIZE_HGV_2_1_2   MAX_CPNTARGET_SIZE_HGV_2_0_5  /* Combination Coupon Target Menu Size */
#define MAX_CPNSTS_SIZE_HGV_2_1_2      MAX_CPNSTS_SIZE_HGV_2_0_5     /* Combination Coupon Status Field Size */
#define MAX_PLUSTS_SIZE_HGV_2_1_2      MAX_PLUSTS_SIZE_HGV_2_0_5     /* PLU Status Field Size R3.1 */
#define MAX_ADJECTIVE_NO_HGV_2_1_2     MAX_ADJECTIVE_NO_HGV_2_0_5    /* The number of Adjective Price */
#define MAX_DEPT_NO_HGV_2_1_2          MAX_DEPT_NO_HGV_2_0_5         /* Max Size of PLU Number */
#define MAX_PLU_NO_HGV_2_1_2           MAX_PLU_NO_HGV_2_0_5          /* Max Size of PLU Number */
#define MAX_PLUPRICE_SIZE_HGV_2_1_2    MAX_PLUPRICE_SIZE_HGV_2_0_5   /* Max Size of 1 PLU Price Save Area */
#define MAX_PCIF_SIZE_HGV_2_1_2        MAX_PCIF_SIZE_HGV_2_0_5       /* Max Size of Password for PC I/F */
#define MAX_TONE_SIZE_HGV_2_1_2        MAX_TONE_SIZE_HGV_2_0_5       /* Max Size of Tone control */
#define MAX_TARE_SIZE_HGV_2_1_2        30     /* Max Size of Tare table   */	//changed from 10 in version 2.1.2
#define MAX_SHAREDPRT_SIZE_HGV_2_1_2   MAX_SHAREDPRT_SIZE_HGV_2_0_5  /* Max Size of Shared Printer, 2172 */
#define MAX_EMPLOYEENO_HGV_2_1_2       MAX_EMPLOYEENO_HGV_2_0_5      /* Max Employee No */
#define MAX_JOB_CODE_HGV_2_1_2         MAX_JOB_CODE_HGV_2_0_5        /* Max Job Code */
#define MAX_JOB_CODE_ADDR_HGV_2_1_2    MAX_JOB_CODE_ADDR_HGV_2_0_5   /* Max Job Code Address */
#define MAX_DISPPARA_OFFSET_HGV_2_1_2  MAX_DISPPARA_OFFSET_HGV_2_0_5  /* Max Offset Code of Beverage Dispenser */
#define MAX_DISPPARA_TENDER_HGV_2_1_2  MAX_DISPPARA_OFFSET_HGV_2_0_5  /* JHHJ 3-16-04 Max Tender Key Data of Beverage Dispenser MAX_DISPPARA_TENDER_HGV_2_0_5 == 11 */
#define MAX_DISPPARA_TOTAL_HGV_2_1_2   MAX_DISPPARA_TOTAL_HGV_2_0_5   /* Max Total Key Data of Beverage Dispenser */
#define MAX_TEND_NO_HGV_2_1_2          MAX_TEND_NO_HGV_2_0_5          /* JHHJ 3-16-04 Key for Tender Para, V3.3 */

#define MAX_PRESETAMOUNT_SIZE_HGV_2_1_2 MAX_PRESETAMOUNT_SIZE_HGV_2_0_5   /* Max Size of Preset Amount Cash Tender */
#define MAX_PIGRULE_SIZE_HGV_2_1_2      MAX_PIGRULE_SIZE_HGV_2_0_5        /* Max Size of Pig Rule Table */
#define MAX_HOTELID_SIZE_HGV_2_1_2      MAX_HOTELID_SIZE_HGV_2_0_5        /* HOTEL ID OR SLD */
#define MAX_HOTELID_NO_HGV_2_1_2        MAX_HOTELID_NO_HGV_2_0_5          /* HOTEL ID AND SLD */

#define MAX_AUTOCOUPON_SIZE_HGV_2_1_2     8

#define MAX_FLEXDRIVE_SIZE_HGV_2_1_2     MAX_FLEXDRIVE_SIZE_HGV_2_0_5      /* Max Size of SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER TABLE */
#define MAX_FXDRIVE_TERM_SIZE_HGV_2_1_2  MAX_FXDRIVE_TERM_SIZE_HGV_2_0_5   /* Max Terminal# of FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL */
#define MAX_PARACSTR_SIZE_HGV_2_1_2      MAX_PARACSTR_SIZE_HGV_2_0_5       /* Max One Control String Size MAX_PARACSTR_SIZE_HGV_2_0_5 == 200 */

#define MAX_PARAPPI_SIZE_HGV_2_1_2       40                                /* Max One PPI Price Level */
#define MAX_SERVICETIME_SIZE_HGV_2_1_2   MAX_SERVICETIME_SIZE_HGV_2_0_5    /* Max Size of Service Time Parameter */
#define MAX_PROGRPT_FORMAT_HGV_2_1_2     MAX_PROGRPT_FORMAT_HGV_2_0_5      /* Max no. of prog. report format */
#define MAX_PROGRPT_EODPTD_HGV_2_1_2     MAX_PROGRPT_EODPTD_HGV_2_0_5      /* Max no. of record at EOD/PTD */
#define MAX_PROGRPT_PTD_HGV_2_1_2        MAX_PROGRPT_PTD_HGV_2_0_5         /* Max no. of record at PTD */
#define MAX_LABORCOST_SIZE_HGV_2_1_2     MAX_LABORCOST_SIZE_HGV_2_0_5      /* Max no. of labor cost value */
#define MAX_DISPPARA_SIZE_HGV_2_1_2      MAX_DISPPARA_SIZE_HGV_2_0_5       /* Max no. of beverage dispenser parameter */

#define MAX_MLDMNEMO_SIZE_HGV_2_1_2      MAX_MLDMNEMO_SIZE_HGV_2_0_5       /* Max no. of multi-line display parameter */

#define MAX_KDSIP_SIZE_HGV_2_1_2         MAX_KDSIP_SIZE_HGV_2_0_5          /* KDS IP Address, 2172 */
#define PARA_KDSIP_LEN_HGV_2_1_2         PARA_KDSIP_LEN_HGV_2_0_5          /* KDS IP Address, 2172 */
#define MAX_KDSIP_INPUT_LEN_HGV_2_1_2    MAX_KDSIP_INPUT_LEN_HGV_2_0_5     /* KDS IP Address for input, 2172 */

#define MAX_REST_SIZE_HGV_2_1_2          MAX_REST_SIZE_HGV_2_0_5           /* PLU Sales Restriction Table  */
#define MAX_AGE_SIZE_HGV_2_1_2           MAX_AGE_SIZE_HGV_2_0_5            /* Boundary age  */
#define MAX_DEPTKEY_SIZE_HGV_2_1_2       MAX_DEPTKEY_SIZE_HGV_2_0_5        /* Keyed Dept Key, 2172 */
#define MAX_MISC_SIZE_HGV_2_1_2          9   /* Misc. Parameter */		//changed from 7 in version 2.1.2

#define MAX_TERMINALINFO_SIZE_HGV_2_1_2  MAX_TERMINALINFO_SIZE_HGV_2_0_5   /* Max number of terminals in cluster for terminal info */

#define PARA_TRANSMNEMO_LEN_HGV_2_1_2	 PARA_TRANSMNEMO_LEN_HGV_2_0_5

#define PARA_PROMOTION_LEN_HGV_2_1_2     PARA_PROMOTION_LEN_HGV_2_0_5      /* ParaPromotion */
#define PARA_TRANSMNEMO_LEN_HGV_2_1_2    PARA_TRANSMNEMO_LEN_HGV_2_0_5     /* ParaTransMnemo */
#define PARA_LEADTHRU_LEN_HGV_2_1_2      PARA_LEADTHRU_LEN_HGV_2_0_5       /* ParaLeadThru */
#define PARA_REPORTNAME_LEN_HGV_2_1_2    PARA_REPORTNAME_LEN_HGV_2_0_5     /* ParaReportName */
#define PARA_SPEMNEMO_LEN_HGV_2_1_2      PARA_SPEMNEMO_LEN_HGV_2_0_5       /* ParaSpeMnemo */
#define PARA_ADJMNEMO_LEN_HGV_2_1_2      PARA_ADJMNEMO_LEN_HGV_2_0_5       /* ParaAdjMnemo */
#define PARA_PRTMODI_LEN_HGV_2_1_2       PARA_PRTMODI_LEN_HGV_2_0_5        /* ParaPrtModi */
#define PARA_MAJORDEPT_LEN_HGV_2_1_2     PARA_MAJORDEPT_LEN_HGV_2_0_5      /* ParaMajorDEPT */
#define PARA_CHAR24_LEN_HGV_2_1_2        PARA_CHAR24_LEN_HGV_2_0_5         /* ParaChar24 */

#define PARA_SOFTCHK_LEN_HGV_2_1_2       PARA_SOFTCHK_LEN_HGV_2_0_5      /* ParaSoftCheck */

#define PLU_MAX_DIGIT_HGV_2_1_2         PLU_MAX_DIGIT_HGV_2_0_5       // maximum number of digits in a PLU number

#define PARA_HOTELID_LEN_HGV_2_1_2      PARA_HOTELID_LEN_HGV_2_0_5    /* ParaHotelId Length */
#define PARA_SLD_LEN_HGV_2_1_2          PARA_SLD_LEN_HGV_2_0_5        /* ParaHotelId's Sld Length */
#define PARA_PDT_LEN_HGV_2_1_2          PARA_PDT_LEN_HGV_2_0_5        /* ParaHotelId's Product Length */


// following defines are extra defines for special uses in this file only
#define PARA_TEND_CPM_NO_HGV_2_1_2      PARA_TEND_CPM_NO_HGV_2_0_5    // the number of auchTend elements in CPMDATA for Release 2.0.0

#define MAX_COLORPALETTE_SIZE_HGV_2_1_2	MAX_COLORPALETTE_SIZE_HGV_2_1_0	/* Color Palette ('0' thru '255')*/



typedef struct  {
        WCHAR   auchHotelId[PARA_HOTELID_LEN_HGV_2_1_2];    /* Hotel Id save Area       */
        WCHAR   auchSld[PARA_SLD_LEN_HGV_2_1_2];            /* SLD Id save area         */
        WCHAR   auchProductId[PARA_PDT_LEN_HGV_2_1_2];      /* Product Id save area     */
        WCHAR   auchTend[PARA_TEND_CPM_NO_HGV_2_1_2];           /* Tender 1 to 11 EPT Action code */
        WCHAR   auchApproval[2];                            /* Approval code save area  */
}CPMDATA_HGV_2_1_2;

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

	// added in version 2.1.2 (or 2.1.0?)
	UCHAR	  uchOrderDeclaration; //Default Order Declaration Type
} TERMINALINFO_HGV_2_1_2;


typedef struct  {
        UCHAR  uchAdjective;          /* Adjective Number */
        UCHAR  uchModStat;            /* Modifier Status */
        WCHAR  aszPLUNumber[PLU_MAX_DIGIT_HGV_2_1_2+1];  /* PLU Number */
}PLUNO_HGV_2_1_2;


typedef struct  {
        ULONG  ulRecordNumber;        /* Number of Record, 2172 */
        UCHAR  uchPTDFlag;            /* PTD Provide Flag */
}FLEXMEM_HGV_2_1_2;

typedef struct  {
        UCHAR   uchFsc;     /* FSC Code */
        UCHAR   uchExt;     /* Extend Code */
}FSCDATA_HGV_2_1_2;

typedef struct {
        FSCDATA_HGV_2_1_2  TblFsc[MAX_FSC_NO_HGV_2_1_2];    /* One of Page */
}PARAFSCTBL_HGV_2_1_2;

typedef struct  {
        USHORT usPluNumber;          /* PLU Number */
        UCHAR  uchAdjective;         /* Adjective Code */
}DISPPARA_HGV_2_1_2;

typedef struct  {
        USHORT usSuperNumber;         /* Supervisor Number */
        UCHAR  uchSecret;              /* Secret Number */
}SUPLEVEL_HGV_2_1_2;


typedef struct  {
        UCHAR  uchDate;               /* Date */
        UCHAR  uchDay;                /* Day of Week */
        UCHAR  uchHour1;              /* Start Hour */
        UCHAR  uchMin1;               /* Start Minute */
        UCHAR  uchHour2;              /* End Hour */
        UCHAR  uchMin2;               /* End Minute */
}RESTRICT_HGV_2_1_2;

typedef struct {
	USHORT usMethod;			// access method used, ethernet, serial, etc
	USHORT usPortNo;			// port number of the charge post gateway
	UCHAR  ucIpAddress[4];      // IP address of the charge post gateway
} CHARGEPOSTINFO_HGV_2_1_2;

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
} TENDERKEYINFO_HGV_2_1_2;

typedef struct  {
    UCHAR  uchModStat;                                 /* Modifier Status */
    WCHAR  aszPLUNumber[PLU_MAX_DIGIT_HGV_2_1_2 + 1];  /* PLU Number */
    UCHAR  uchAdjective;                               /* Adjective Code of PLU */
}MENUPLU_HGV_2_1_2;

typedef struct  {
	UCHAR	uchIpAddress[PARA_KDSIP_LEN_HGV_2_1_2];    /* IP Address  */
	USHORT	usPort;                                    /* Port Address of KDS  */
	USHORT	usNHPOSPort;                               /* Port Address of Interface port with NHPOS */
}PARAKDS_HGV_2_1_2;




typedef struct	tagUNINTRAM_HGV_2_1_2 {
	//  szVer is removed because it is not part of the PEP data file.
	//  it is part of the UNINIRAM struct in the terminal only.
//	WCHAR       szVer [FILE_LEN_ID];                 /* 10-1-2004 JHHJ for file version information*/
    UCHAR       ParaMDC [MAX_MDC_SIZE_HGV_2_1_2];               /* prog #1 */
    FLEXMEM_HGV_2_1_2     ParaFlexMem [MAX_FLXMEM_SIZE_HGV_2_1_2];        /* prog #2 */
    PARAFSCTBL_HGV_2_1_2  ParaFSC [MAX_FSC_TBL_HGV_2_1_2];                /* prog #3 */
    USHORT      ParaSecurityNo;                                 /* prog #6 */
    USHORT      ParaStoRegNo [MAX_STOREG_SIZE_HGV_2_1_2];       /* prog #7 */
    SUPLEVEL_HGV_2_1_2    ParaSupLevel [MAX_SUPLEVEL_SIZE_HGV_2_1_2];     /* prog #8 */
    UCHAR       ParaActCodeSec [MAX_ACSEC_SIZE_HGV_2_1_2];      /* prog #9 */
    UCHAR       ParaTransHALO [MAX_HALOKEY_NO_HGV_2_1_2];       /* prog #10 */
    UCHAR       ParaHourlyTime [MAX_HOUR_NO_HGV_2_1_2] [MAX_HM_DATA_HGV_2_1_2];             /* prog #17 */
    UCHAR       ParaSlipFeedCtl [MAX_SLIP_SIZE_HGV_2_1_2];                                  /* prog #18 */
    WCHAR       ParaTransMnemo [MAX_TRANSM_NO_HGV_2_1_2] [PARA_TRANSMNEMO_LEN_HGV_2_1_2];   /* prog #20 */
    WCHAR       ParaLeadThru [MAX_LEAD_NO_HGV_2_1_2] [PARA_LEADTHRU_LEN_HGV_2_1_2];       /* prog #21 */
    WCHAR       ParaReportName [MAX_REPO_NO_HGV_2_1_2] [PARA_REPORTNAME_LEN_HGV_2_1_2];   /* prog #22 */
    WCHAR       ParaSpeMnemo [MAX_SPEM_NO_HGV_2_1_2] [PARA_SPEMNEMO_LEN_HGV_2_1_2];       /* prog #23 */
    WCHAR       ParaPCIF [MAX_PCIF_SIZE_HGV_2_1_2];                                    /* prog #39 */
    WCHAR       ParaAdjMnemo [MAX_ADJM_NO_HGV_2_1_2] [PARA_ADJMNEMO_LEN_HGV_2_1_2];    /* prog #46 */
    WCHAR       ParaPrtModi [MAX_PRTMODI_NO_HGV_2_1_2] [PARA_PRTMODI_LEN_HGV_2_1_2];	/* prog #47 */
    WCHAR       ParaMajorDEPT [MAX_MDEPT_NO_HGV_2_1_2] [PARA_MAJORDEPT_LEN_HGV_2_1_2];  /* prog #48 */
    UCHAR       ParaAutoAltKitch [MAX_DEST_SIZE_HGV_2_1_2];                             /* prog #49 */
    CPMDATA_HGV_2_1_2    ParaHotelId;                                                   /* prog #54 */
    WCHAR       ParaChar24 [MAX_CH24_NO_HGV_2_1_2] [PARA_CHAR24_LEN_HGV_2_1_2];         /* prog #57 */
    UCHAR       ParaTtlKeyTyp [MAX_KEYTYPE_SIZE_HGV_2_1_2];                             /* prog #60 */
    UCHAR       ParaTtlKeyCtl [MAX_TTLKEY_NO_HGV_2_1_2] [MAX_TTLKEY_DATA_HGV_2_1_2];    /* prog #61 */
    UCHAR       ParaTend [MAX_TEND_NO_HGV_2_1_2];                                       /* prog #62 */
    PLUNO_HGV_2_1_2       ParaPLUNoMenu [MAX_PLUKEY_SIZE_HGV_2_1_2];                              /* A/C #4, 2172 */
    UCHAR       ParaCtlTblMenu [MAX_PAGE_SIZE_HGV_2_1_2];                               /* A/C #5 */
    UCHAR       ParaManuAltKitch [MAX_DEST_SIZE_HGV_2_1_2];                            /* A/C #6 */
    ULONG		ParaCashABAssign [MAX_AB_SIZE_HGV_2_1_2];                              /* A/C #7 */
    UCHAR       ParaRound [MAX_RNDTBL_SIZE_HGV_2_1_2][MAX_RND_DATA_HGV_2_1_2];         /* A/C #84 */
    UCHAR       ParaDisc [MAX_DISCRATE_SIZE_HGV_2_1_2];                                 /* A/C #86 */
    WCHAR       ParaPromotion [MAX_PROMO_NO_HGV_2_1_2] [PARA_PROMOTION_LEN_HGV_2_1_2];	/* A/C #88 */
    ULONG       ParaCurrency [MAX_FC_SIZE_HGV_2_1_2];                                   /* A/C #89 */
    UCHAR       ParaToneCtl [MAX_TONE_SIZE_HGV_2_1_2];                                  /* A/C #169 */
    MENUPLU_HGV_2_1_2     ParaMenuPLU [MAX_SETMENU_NO_HGV_2_1_2] [MAX_SETPLU_DATA_HGV_2_1_2];     /* A/C #116, 2172 */
    UCHAR       ParaTaxTable [MAX_TTBLE_SIZE_HGV_2_1_2];                                /* A/C #124-126 */
    ULONG       ParaTaxRate [MAX_TAXRATE_SIZE_HGV_2_1_2];                               /* A/C #127 */
    ULONG       ParaMiscPara [MAX_MISC_SIZE_HGV_2_1_2];                                 /* MISC. PARAMETER */
	USHORT      ParaTare [MAX_TARE_SIZE_HGV_2_1_2];                          /* A/C #111 */
    ULONG       ParaPresetAmount [MAX_PRESETAMOUNT_SIZE_HGV_2_1_2];          /* prog #15 */
    UCHAR       ParaSharedPrt [MAX_SHAREDPRT_SIZE_HGV_2_1_2];                /* prog #50 */
    WCHAR       ParaSoftChk [MAX_SOFTCHK_NO_HGV_2_1_2] [PARA_SOFTCHK_LEN_HGV_2_1_2];     /* A/C #87 */
    ULONG       ParaPigRule [MAX_PIGRULE_SIZE_HGV_2_1_2];                                /* A/C #130 */
    UCHAR       ParaOep [MAX_TABLE_NO_HGV_2_1_2] [MAX_TABLE_ADDRESS_HGV_2_1_2];   /* A/C #160 */
    UCHAR       ParaFlexDrive [MAX_FLEXDRIVE_SIZE_HGV_2_1_2];                     /* A/C #162 */
    USHORT      ParaServiceTime [MAX_SERVICETIME_SIZE_HGV_2_1_2];                 /* A/C #133 */
    USHORT	    ParaLaborCost[MAX_LABORCOST_SIZE_HGV_2_1_2];                      /* A/C #154 */
    DISPPARA_HGV_2_1_2    ParaDispPara [MAX_DISPPARA_SIZE_HGV_2_1_2];                       /* A/C #137 */
	PARAKDS_HGV_2_1_2     ParaKdsIp [MAX_KDSIP_SIZE_HGV_2_1_2];                             /* SR 14 ESMITH */
	RESTRICT_HGV_2_1_2    ParaRestriction [MAX_REST_SIZE_HGV_2_1_2];               /* AC 170, 2172 */
    UCHAR       auchParaBoundAge [MAX_AGE_SIZE_HGV_2_1_2];               /* A/C 208, 2172 */
    USHORT      ParaDeptNoMenu [MAX_DEPTKEY_SIZE_HGV_2_1_2];             /* A/C #4, 2172 */
	USHORT      ParaSpcCo [MAX_CO_SIZE_HGV_2_1_2];                       /* SPECIAL COUNTER */
	ULONG	    ParaAutoCoupon[MAX_AUTOCOUPON_SIZE_HGV_2_1_2];					/* Prog. #67  ACC Controls */ //RLZ

	// beginning of entries added in version 2.1.2
	NHPOSCOLORREF	ParaColorPalette[MAX_COLORPALETTE_SIZE_HGV_2_1_2][2];	/*A/C #209 Palette[i][0] -> Foreground color, Palette[i][1] -> Background color */
	UCHAR		ParaTtlKeyOrderDecType[MAX_TTLKEY_NO_HGV_2_1_2];
	// end new entries

	CHARGEPOSTINFO_HGV_2_1_2    ChargePostInformation;                        // R_20_DATABASECHANGE
	TERMINALINFO_HGV_2_1_2      TerminalInformation[MAX_TERMINALINFO_SIZE_HGV_2_1_2];
	CHARGEPOSTINFO_HGV_2_1_2    EPT_ENH_Information[16];                      // R_20_DATABASECHANGE
	TENDERKEYINFO_HGV_2_1_2     TenderKeyInformation[MAX_TEND_NO_HGV_2_1_2];            // R_20_DATABASECHANGE
    ULONG                       ParaStoreForward[MAX_MISC_SIZE_HGV_2_1_2];    // area used for the Store and Forward functionality

} UNINIRAM_HGV_2_1_2;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif