



// Define the structures for UNINIRAM for Release 1.4.3
//

#if !defined(INCLUDE_UNI_1_4_3_H)
#define INCLUDE_UNI_1_4_3_H



/*  RJC pragma pack uncommented here */
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#define MAX_MDC_SIZE_HGV_1_4_3          400    /* MDC Data for 1byte Data, 2172 */
#define MAX_FSC_TBL_HGV_1_4_3             9    /* Function Key Table *///RPH 12-2-3
#define MAX_FSC_NO_HGV_1_4_3            165    /* Function Key 165->300 JHHJ Database Changees 3-17-04*/
#define MAX_FSC_DATA_HGV_1_4_3            2    /* Major FSC Data/Minor FSC Data */
#define MAX_SUPLEVEL_SIZE_HGV_1_4_3       8    /* Supervisor Level */
#define MAX_ACSEC_SIZE_HGV_1_4_3        127    /* Action Code Security for Supervisor */
#define MAX_HALOKEY_NO_HGV_1_4_3         36    /* Key for setting HALO */
#define MAX_HOUR_NO_HGV_1_4_3            49    /* Data for setting Hourly Activity Time, V3.3 */
#define MAX_HM_DATA_HGV_1_4_3             2    /* Hour/Minute */
#define MAX_SLIP_SIZE_HGV_1_4_3           6    /* Slip Printer Feed Control */
#define MAX_TRANSM_NO_HGV_1_4_3         200    /* Transaction Memonics, 2172 */

#define MAX_LEAD_NO_HGV_1_4_3           150    /* Max number of Lead Through mnemonics */
#define MAX_REPO_NO_HGV_1_4_3            24    /* Report Name */
#define MAX_SPEM_NO_HGV_1_4_3            50    /* Special Mnemonics, 2172 */
#define MAX_TTBLE_SIZE_HGV_1_4_3        208    /* Ram Fix Size for setting Tax Rate */
#define MAX_TTBLDATA_SIZE_HGV_1_4_3     200    /* Tax Data Table Size for Tax Table */
#define MAX_TTBLHEAD_SIZE_HGV_1_4_3       4    /* Header Size for Tax Table */
#define MAX_TTBLRATE_SIZE_HGV_1_4_3       5    /* WDC & RATE Size for Tax Table */
#define MAX_ADJM_NO_HGV_1_4_3            20    /* Adjective Mneminics */
#define MAX_PRTMODI_NO_HGV_1_4_3          8    /* Print Modifier */

#define MAX_MDEPT_NO_HGV_1_4_3           30    /* Major Department Mnemonics */
#define MAX_CH24_NO_HGV_1_4_3            20    /* 24 Characters, 2172 */
#define MAX_PAGE_SIZE_HGV_1_4_3          19    /* Control Menu Page, */ //RPH 12-2-3 TOUCHSCREEN
#define MAX_DEST_SIZE_HGV_1_4_3           8    /* Destination Kitchen Printer */
#define MAX_FLXMEM_SIZE_HGV_1_4_3    (13+1)    /* Flexible Memory Assignment, with MLD File */
#define MAX_STOREG_SIZE_HGV_1_4_3         2    /* Store/Register No. */

#define MAX_KEYTYPE_SIZE_HGV_1_4_3        6    /* Total Keys are in need of type assign */
#define MAX_TTLKEY_NO_HGV_1_4_3           9    /* The number of Total Key */
#define MAX_TTLKEY_DATA_HGV_1_4_3         6    /* The number of bit */
#define MAX_AB_SIZE_HGV_1_4_3             2    /* Cashier A/Cashier B */
#define MAX_CO_SIZE_HGV_1_4_3             4    /* Special counter */
#define MAX_DEPTSTS_SIZE_HGV_1_4_3        5    /* DEPT Status Field Size R3.1 */
#define MAX_CPNTARGET_SIZE_HGV_1_4_3      7    /* Combination Coupon Target Menu Size */
#define MAX_CPNSTS_SIZE_HGV_1_4_3         2    /* Combination Coupon Status Field Size */
#define MAX_PLUSTS_SIZE_HGV_1_4_3         6    /* PLU Status Field Size R3.1 */
#define MAX_ADJECTIVE_NO_HGV_1_4_3        5    /* The number of Adjective Price */
#define MAX_DEPT_NO_HGV_1_4_3          9999    /* Max Size of PLU Number */
#define MAX_PLU_NO_HGV_1_4_3           9999    /* Max Size of PLU Number */
#define MAX_PLUPRICE_SIZE_HGV_1_4_3       3    /* Max Size of 1 PLU Price Save Area */
#define MAX_PCIF_SIZE_HGV_1_4_3          10    /* Max Size of Password for PC I/F */
#define MAX_TONE_SIZE_HGV_1_4_3           1    /* Max Size of Tone control */
#define MAX_TARE_SIZE_HGV_1_4_3          10    /* Max Size of Tare table   */
#define MAX_SHAREDPRT_SIZE_HGV_1_4_3     48    /* Max Size of Shared Printer, 2172 */
#define MAX_EMPLOYEENO_HGV_1_4_3       9999    /* Max Employee No */
#define MAX_JOB_CODE_HGV_1_4_3           99    /* Max Job Code */
#define MAX_JOB_CODE_ADDR_HGV_1_4_3      3L    /* Max Job Code Address */
#define MAX_DISPPARA_OFFSET_HGV_1_4_3 31712    /* Max Offset Code of Beverage Dispenser */
#define MAX_DISPPARA_TENDER_HGV_1_4_3    11    /* JHHJ 3-16-04 Max Tender Key Data of Beverage Dispenser */
#define MAX_DISPPARA_TOTAL_HGV_1_4_3     19    /* Max Total Key Data of Beverage Dispenser */
#define MAX_TEND_NO_HGV_1_4_3            11    /* JHHJ 3-16-04 Key for Tender Para, V3.3 */

#define MAX_PRESETAMOUNT_SIZE_HGV_1_4_3   4    /* Max Size of Preset Amount Cash Tender */
#define MAX_PIGRULE_SIZE_HGV_1_4_3       10    /* Max Size of Pig Rule Table */
#define MAX_HOTELID_SIZE_HGV_1_4_3       10    /* HOTEL ID OR SLD */
#define MAX_HOTELID_NO_HGV_1_4_3         18    /* HOTEL ID AND SLD */

#define MAX_FLEXDRIVE_SIZE_HGV_1_4_3     36    /* Max Size of SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER TABLE */
#define MAX_FXDRIVE_TERM_SIZE_HGV_1_4_3  16    /* Max Terminal# of FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL */
#define MAX_PARACSTR_SIZE_HGV_1_4_3     200    /* Max One Control String Size */

#define MAX_PARAPPI_SIZE_HGV_1_4_3       20    /* Max One PPI Price Level */
#define MAX_SERVICETIME_SIZE_HGV_1_4_3    2    /* Max Size of Service Time Parameter */
#define MAX_PROGRPT_FORMAT_HGV_1_4_3      8    /* Max no. of prog. report format */
#define MAX_PROGRPT_EODPTD_HGV_1_4_3     11    /* Max no. of record at EOD/PTD */
#define MAX_PROGRPT_PTD_HGV_1_4_3         8    /* Max no. of record at PTD */
#define MAX_LABORCOST_SIZE_HGV_1_4_3    100    /* Max no. of labor cost value */
#define MAX_DISPPARA_SIZE_HGV_1_4_3     259    /* Max no. of beverage dispenser parameter */

#define MAX_MLDMNEMO_SIZE_HGV_1_4_3     220    /* Max no. of multi-line display parameter */

#define MAX_KDSIP_SIZE_HGV_1_4_3          8   /* KDS IP Address, 2172 */
#define PARA_KDSIP_LEN_HGV_1_4_3         4   /* KDS IP Address, 2172 */
#define MAX_KDSIP_INPUT_LEN_HGV_1_4_3    12   /* KDS IP Address for input, 2172 */

#define MAX_REST_SIZE_HGV_1_4_3           6    /* PLU Sales Restriction Table  */
#define MAX_AGE_SIZE_HGV_1_4_3            3    /* Boundary age  */
#define MAX_DEPTKEY_SIZE_HGV_1_4_3      450    /* Keyed Dept Key, 2172 */
#define MAX_MISC_SIZE_HGV_1_4_3           7    /* Misc. Parameter */

#define MAX_TERMINALINFO_SIZE_HGV_1_4_3  16    /* Max number of terminals in cluster for terminal info */

#define PLU_MAX_DIGIT_HGV_1_4_3          13    // maximum number of digits in a PLU number

#define PARA_HOTELID_LEN_HGV_1_4_3        2        /* ParaHotelId Length */
#define PARA_SLD_LEN_HGV_1_4_3            4        /* ParaHotelId's Sld Length */
#define PARA_PDT_LEN_HGV_1_4_3           10        /* ParaHotelId's Product Length */


typedef struct  {
        UCHAR   uchFsc;     /* FSC Code */
        UCHAR   uchExt;     /* Extend Code */
}FSCDATA_HGV_1_4_3;

typedef struct {
        FSCDATA_HGV_1_4_3  TblFsc[MAX_FSC_NO_HGV_1_4_3];    /* One of Page */
}PARAFSCTBL_HGV_1_4_3;


/*--- Prog. 54 for Charge Posting Data in Uniniram.asm ---*/

typedef struct  {
        UCHAR   auchHotelId[PARA_HOTELID_LEN_HGV_1_4_3];    /* Hotel Id save Area       */
        UCHAR   auchSld[PARA_SLD_LEN_HGV_1_4_3];            /* SLD Id save area         */
        UCHAR   auchProductId[PARA_PDT_LEN_HGV_1_4_3];      /* Product Id save area     */
        UCHAR   auchTend[11];                               /* Tender 1 to 11 EPT Action code */
        UCHAR   auchApproval[2];                            /* Approval code save area  */
}CPMDATA_HGV_1_4_3;

typedef struct  {
    UCHAR  uchModStat;                                 /* Modifier Status */
    UCHAR  aszPLUNumber[PLU_MAX_DIGIT_HGV_1_4_3 + 1];  /* PLU Number */
    UCHAR  uchAdjective;                               /* Adjective Code of PLU */
}MENUPLU_HGV_1_4_3;

typedef struct  {
        USHORT usSuperNumber;         /* Supervisor Number */
        UCHAR  uchSecret;              /* Secret Number */
}SUPLEVEL_HGV_1_4_3;

typedef struct  {
        ULONG  ulRecordNumber;        /* Number of Record, 2172 */
        UCHAR  uchPTDFlag;            /* PTD Provide Flag */
}FLEXMEM_HGV_1_4_3;

typedef struct  {
        USHORT usPluNumber;          /* PLU Number */
        UCHAR  uchAdjective;         /* Adjective Code */
}DISPPARA_HGV_1_4_3;

typedef struct  {
        UCHAR  uchDate;               /* Date */
        UCHAR  uchDay;                /* Day of Week */
        UCHAR  uchHour1;              /* Start Hour */
        UCHAR  uchMin1;               /* Start Minute */
        UCHAR  uchHour2;              /* End Hour */
        UCHAR  uchMin2;               /* End Minute */
}RESTRICT_HGV_1_4_3;



typedef struct	tagUNINTRAM_HGV_1_4_3 {
    UCHAR       ParaMDC [MAX_MDC_SIZE_HGV_1_4_3];                      /* prog #1 */
    FLEXMEM_HGV_1_4_3     ParaFlexMem [MAX_FLXMEM_SIZE_HGV_1_4_3];     /* prog #2 */
    PARAFSCTBL_HGV_1_4_3  ParaFSC [MAX_FSC_TBL_HGV_1_4_3];             /* prog #3 */
    USHORT      ParaSecurityNo;                                        /* prog #6 */
    USHORT      ParaStoRegNo [MAX_STOREG_SIZE_HGV_1_4_3];              /* prog #7 */
    SUPLEVEL_HGV_1_4_3    ParaSupLevel [MAX_SUPLEVEL_SIZE_HGV_1_4_3];            /* prog #8 */
    UCHAR       ParaActCodeSec [MAX_ACSEC_SIZE_HGV_1_4_3];                 /* prog #9 */
    UCHAR       ParaTransHALO [MAX_HALOKEY_NO_HGV_1_4_3];                        /* prog #10 */
    UCHAR       ParaHourlyTime [MAX_HOUR_NO_HGV_1_4_3] [MAX_HM_DATA_HGV_1_4_3];           /* prog #17 */
    UCHAR       ParaSlipFeedCtl [MAX_SLIP_SIZE_HGV_1_4_3];                                /* prog #18 */
    UCHAR       ParaTransMnemo [MAX_TRANSM_NO_HGV_1_4_3] [PARA_TRANSMNEMO_LEN_HGV_1_4_3]; /* prog #20 */
    UCHAR       ParaLeadThru [MAX_LEAD_NO_HGV_1_4_3] [PARA_LEADTHRU_LEN_HGV_1_4_3];       /* prog #21 */
    UCHAR       ParaReportName [MAX_REPO_NO_HGV_1_4_3] [PARA_REPORTNAME_LEN_HGV_1_4_3];   /* prog #22 */
    UCHAR       ParaSpeMnemo [MAX_SPEM_NO_HGV_1_4_3] [PARA_SPEMNEMO_LEN_HGV_1_4_3];    /* prog #23 */
    UCHAR       ParaPCIF [MAX_PCIF_SIZE_HGV_1_4_3];                                    /* prog #39 */
    UCHAR       ParaAdjMnemo [MAX_ADJM_NO_HGV_1_4_3] [PARA_ADJMNEMO_LEN_HGV_1_4_3];    /* prog #46 */
    UCHAR       ParaPrtModi [MAX_PRTMODI_NO_HGV_1_4_3] [PARA_PRTMODI_LEN_HGV_1_4_3];   /* prog #47 */
    UCHAR       ParaMajorDEPT [MAX_MDEPT_NO_HGV_1_4_3] [PARA_MAJORDEPT_LEN_HGV_1_4_3]; /* prog #48 */
    UCHAR       ParaAutoAltKitch [MAX_DEST_SIZE_HGV_1_4_3];                           /* prog #49 */
    CPMDATA_HGV_1_4_3    ParaHotelId;                                                          /* prog #54 */
    UCHAR       ParaChar24 [MAX_CH24_NO_HGV_1_4_3] [PARA_CHAR24_LEN_HGV_1_4_3];       /* prog #57 */
    UCHAR       ParaTtlKeyTyp [MAX_KEYTYPE_SIZE_HGV_1_4_3];                           /* prog #60 */
    UCHAR       ParaTtlKeyCtl [MAX_TTLKEY_NO_HGV_1_4_3] [MAX_TTLKEY_DATA_HGV_1_4_3];  /* prog #61 */
    UCHAR       ParaTend [MAX_TEND_NO_HGV_1_4_3];                                     /* prog #62 */
    PLUNO       ParaPLUNoMenu [MAX_PLUKEY_SIZE_HGV_1_4_3];                            /* A/C #4, 2172 */
    UCHAR       ParaCtlTblMenu [MAX_PAGE_SIZE_HGV_1_4_3];                             /* A/C #5 */
    UCHAR       ParaManuAltKitch [MAX_DEST_SIZE_HGV_1_4_3];                           /* A/C #6 */
    USHORT      ParaCashABAssign [MAX_AB_SIZE_HGV_1_4_3];                             /* A/C #7 */
    UCHAR       ParaRound [MAX_RNDTBL_SIZE_HGV_1_4_3] [MAX_RND_DATA_HGV_1_4_3];           /* A/C #84 */
    UCHAR       ParaDisc [MAX_DISCRATE_SIZE_HGV_1_4_3];                                   /* A/C #86 */
    UCHAR       ParaPromotion [MAX_PROMO_NO_HGV_1_4_3] [PARA_PROMOTION_LEN_HGV_1_4_3];    /* A/C #88 */
    ULONG       ParaCurrency [MAX_FC_SIZE_HGV_1_4_3];                                     /* A/C #89 */
    UCHAR       ParaToneCtl [MAX_TONE_SIZE_HGV_1_4_3];                                    /* A/C #169 */
    MENUPLU_HGV_1_4_3     ParaMenuPLU [MAX_SETMENU_NO_HGV_1_4_3] [MAX_SETPLU_DATA_HGV_1_4_3];       /* A/C #116, 2172 */
    UCHAR       ParaTaxTable [MAX_TTBLE_SIZE_HGV_1_4_3];                                  /* A/C #124-126 */
    ULONG       ParaTaxRate [MAX_TAXRATE_SIZE_HGV_1_4_3];                /* A/C #127 */
    ULONG       ParaMiscPara [MAX_MISC_SIZE_HGV_1_4_3];                  /* MISC. PARAMETER */
    USHORT      ParaTare [MAX_TARE_SIZE_HGV_1_4_3];                      /* A/C #111 */
    ULONG       ParaPresetAmount [MAX_PRESETAMOUNT_SIZE_HGV_1_4_3];      /* prog #15 */
    UCHAR       ParaSharedPrt [MAX_SHAREDPRT_SIZE_HGV_1_4_3];                          /* prog #50 */
    UCHAR       ParaSoftChk [MAX_SOFTCHK_NO_HGV_1_4_3] [PARA_SOFTCHK_LEN_HGV_1_4_3];   /* A/C #87 */
    ULONG       ParaPigRule [MAX_PIGRULE_SIZE_HGV_1_4_3];                              /* A/C #130 */
    UCHAR       ParaOep [MAX_TABLE_NO_HGV_1_4_3] [MAX_TABLE_ADDRESS_HGV_1_4_3];  /* A/C #160 */
    UCHAR       ParaFlexDrive [MAX_FLEXDRIVE_SIZE_HGV_1_4_3];                    /* A/C #162 */
    USHORT      ParaServiceTime [MAX_SERVICETIME_SIZE_HGV_1_4_3];                /* A/C #133 */
    USHORT      ParaLaborCost [MAX_LABORCOST_SIZE_HGV_1_4_3];                    /* A/C #154 */
    DISPPARA_HGV_1_4_3    ParaDispPara [MAX_DISPPARA_SIZE_HGV_1_4_3];            /* A/C #137 */
    USHORT      ParaKdsIp [MAX_KDSIP_SIZE_HGV_1_4_3] [PARA_KDSIP_LEN_HGV_1_4_3];    /* Prog, 51, 2172 */
    RESTRICT_HGV_1_4_3    ParaRestriction [ MAX_REST_SIZE_HGV_1_4_3];               /* AC 170, 2172 */
    UCHAR       auchParaBoundAge [ MAX_AGE_SIZE_HGV_1_4_3];             /* A/C 208, 2172 */
    USHORT      ParaDeptNoMenu [MAX_DEPTKEY_SIZE_HGV_1_4_3];            /* A/C #4, 2172 */
    USHORT      ParaSpcCo [MAX_CO_SIZE_HGV_1_4_3];                      /* SPECIAL COUNTER */

} UNINIRAM_HGV_1_4_3;



#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif