
/*
/////////////////////////////////////////////////////////////////////////////
// FILE.H :
//
// Copyright (C) 1999 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// AUG-20-1999  1.00.00  T.Koyama     initial (2171)
// SEP-17-1999           M.Teraki     Changed type definition of 'D13DIGITS'
// Feb-09-2000           K.Iwata      Added definition of '_DEBUG_BREAK'
//
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef _INC_FILE
#define _INC_FILE

#include <ecr.h>
#include <paraequ.h>

#if !defined(_INC_PARA)
#include <para.h>
#endif


#if !defined(__PLU_H)
#include <plu.h>
#endif
#if !defined(CSSTBFCC_INCLUDED)
#include <csstbfcc.h>
#endif

#if !defined(CPM_INCLUDED)
#include <cpm.h>
#endif

#if !defined(EPT_INCLUDED)
#include <eept.h>
#endif

#if !defined(CSTTL_INCLUDED)
#include <csttl.h>
#endif

#define MAX_LEAD_MAX_HGV20			168

typedef struct  {
        WCHAR/*UCHAR*/ auchHotelId[PARA_HOTELID_LEN];    /* Hotel Id save Area       */
        WCHAR/*UCHAR*/ auchSld[PARA_SLD_LEN];            /* SLD Id save area         */
        WCHAR/*UCHAR*/ auchProductId[PARA_PDT_LEN];      /* Product Id save area     */
        WCHAR/*UCHAR*/ auchTend[STD_TENDER_MAX];                     /* Tender 1 to 11 EPT Action code */
        WCHAR/*UCHAR*/ auchApproval[2];                  /* Approval code save area  */
}CPMDATA_HGV20;


#define P60GETTYPE(x)       (BYTE)((UCHAR)(x) / 21)
#define P60GETINDEX(x)      (BYTE)((UCHAR)(x) % 21)
#define P60SETTYPE(x)       (UCHAR)((BYTE)(x) * 21) 
#define P60SETINDEX(x)      (UCHAR)((BYTE)(x))



#define P60_HGV20GETTYPE(x)       (BYTE)((UCHAR)(x) / 10)
#define P60_HGV20GETINDEX(x)      (BYTE)((UCHAR)(x) % 10)
#define P60_HGV20SETTYPE(x)       (UCHAR)((BYTE)(x) * 21) 
#define P60_HGV20SETINDEX(x)      (UCHAR)((BYTE)(x))

#define MAX_MDC_SIZE_HGV20    400    /* MDC Data for 1byte Data, 2172 */
#define MAX_FSC_TBL_HGV20      11    /* Function Key Table *///RPH 12-2-3
#define MAX_FSC_NO_HGV20            300    /* Function Key 165->300 JHHJ Database Changees 3-17-04*/
#define MAX_FSC_DATA_HGV20            2    /* Major FSC Data/Minor FSC Data */
#define MAX_SUPLEVEL_SIZE_HGV20       8    /* Supervisor Level */
#define MAX_ACSEC_SIZE_HGV20        127    /* Action Code Security for Supervisor */
#define MAX_HALOKEY_NO_HGV20         36    /* Key for setting HALO */
#define MAX_HOUR_NO_HGV20            49    /* Data for setting Hourly Activity Time, V3.3 */
#define MAX_HM_DATA_HGV20             2    /* Hour/Minute */
#define MAX_SLIP_SIZE_HGV20           6    /* Slip Printer Feed Control */
#define MAX_TRANSM_NO_HGV20         400    /* Transaction Memonics, 2172 */

#define MAX_LEAD_NO_HGV20           180    /* Max number of Lead Through mnemonics */
#define MAX_REPO_NO_HGV20            24    /* Report Name */
#define MAX_SPEM_NO_HGV20            50    /* Special Mnemonics, 2172 */
#define MAX_TTBLE_SIZE_HGV20        208    /* Ram Fix Size for setting Tax Rate */
#define MAX_TTBLDATA_SIZE_HGV20     200    /* Tax Data Table Size for Tax Table */
#define MAX_TTBLHEAD_SIZE_HGV20       4    /* Header Size for Tax Table */
#define MAX_TTBLRATE_SIZE_HGV20       5    /* WDC & RATE Size for Tax Table */
#define MAX_ADJM_NO_HGV20            20    /* Adjective Mneminics */
#define MAX_PRTMODI_NO_HGV20          8    /* Print Modifier */

#define MAX_MDEPT_NO_HGV20           30    /* Major Department Mnemonics */
#define MAX_CH24_NO_HGV20            20    /* 24 Characters, 2172 */
#define MAX_PAGE_SIZE_HGV20          23    /* Control Menu Page, */ //RPH 12-2-3 TOUCHSCREEN
#define MAX_DEST_SIZE_HGV20           8    /* Destination Kitchen Printer */
#define MAX_FLXMEM_SIZE_HGV20    (13+1)    /* Flexible Memory Assignment, with MLD File */
#define MAX_STOREG_SIZE_HGV20         2    /* Store/Register No. */

#define MAX_KEYTYPE_SIZE_HGV20        6    /* Total Keys are in need of type assign */
#define MAX_TTLKEY_NO_HGV20           STD_TOTAL_MAX    /* The number of Total Key */
#define MAX_TTLKEY_DATA_HGV20         6    /* The number of bit */
#define MAX_AB_SIZE_HGV20            2    /* Cashier A/Cashier B */
#define MAX_CO_SIZE_HGV20             4    /* Special counter */
#define MAX_DEPTSTS_SIZE_HGV20        5    /* DEPT Status Field Size R3.1 */
#define MAX_CPNTARGET_SIZE_HGV20      7    /* Combination Coupon Target Menu Size */
#define MAX_CPNSTS_SIZE_HGV20         2    /* Combination Coupon Status Field Size */
#define MAX_PLUSTS_SIZE_HGV20         6    /* PLU Status Field Size R3.1 */
#define MAX_ADJECTIVE_NO_HGV20        5    /* The number of Adjective Price */
#define MAX_DEPT_NO_HGV20           9999    /* Max Size of PLU Number */
#define MAX_PLU_NO_HGV20           9999    /* Max Size of PLU Number */
#define MAX_PLUPRICE_SIZE_HGV20       3    /* Max Size of 1 PLU Price Save Area */
#define MAX_PCIF_SIZE_HGV20          10    /* Max Size of Password for PC I/F */
#define MAX_TONE_SIZE_HGV20           1    /* Max Size of Tone control */
#define MAX_TARE_SIZE_HGV20          10    /* Max Size of Tare table   */
#define MAX_SHAREDPRT_SIZE_HGV20     48    /* Max Size of Shared Printer, 2172 */
#define MAX_EMPLOYEENO_HGV20       9999    /* Max Employee No */
#define MAX_JOB_CODE_HGV20           99    /* Max Job Code */
#define MAX_JOB_CODE_ADDR_HGV20      3L    /* Max Job Code Address */
#define MAX_DISPPARA_OFFSET_HGV20 31712    /* Max Offset Code of Beverage Dispenser */
#define MAX_DISPPARA_TENDER_HGV20    STD_TENDER_MAX    /* JHHJ 3-16-04 Max Tender Key Data of Beverage Dispenser */
#define MAX_DISPPARA_TOTAL_HGV20     19    /* Max Total Key Data of Beverage Dispenser */
#define MAX_TEND_NO_HGV20            STD_TENDER_MAX    /* JHHJ 3-16-04 Key for Tender Para, V3.3 */

#define MAX_PRESETAMOUNT_SIZE_HGV20   4    /* Max Size of Preset Amount Cash Tender */
#define MAX_PIGRULE_SIZE_HGV20       10    /* Max Size of Pig Rule Table */
#define MAX_HOTELID_SIZE_HGV20       10    /* HOTEL ID OR SLD */
#define MAX_HOTELID_NO_HGV20         18    /* HOTEL ID AND SLD */

#define MAX_FLEXDRIVE_SIZE_HGV20     36    /* Max Size of SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER TABLE */
#define MAX_FXDRIVE_TERM_SIZE_HGV20  16    /* Max Terminal# of FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL */
#define MAX_PARACSTR_SIZE_HGV20     400    /* Max One Control String Size */

#define MAX_PARAPPI_SIZE_HGV20       20    /* Max One PPI Price Level */
#define MAX_SERVICETIME_SIZE_HGV20    2    /* Max Size of Service Time Parameter */
#define MAX_PROGRPT_FORMAT_HGV20      8    /* Max no. of prog. report format */
#define MAX_PROGRPT_EODPTD_HGV20     11    /* Max no. of record at EOD/PTD */
#define MAX_PROGRPT_PTD_HGV20         8    /* Max no. of record at PTD */
#define MAX_LABORCOST_SIZE_HGV20    100    /* Max no. of labor cost value */
#define MAX_DISPPARA_SIZE_HGV20     259    /* Max no. of beverage dispenser parameter */

#define MAX_MLDMNEMO_SIZE_HGV20     220    /* Max no. of multi-line display parameter */

#define MAX_KDSIP_SIZE_HGV20          8   /* KDS IP Address, 2172 */
#define PARA_KDSIP_LEN_HGV20         4   /* KDS IP Address, 2172 */
#define MAX_KDSIP_INPUT_LEN_HGV20    12   /* KDS IP Address for input, 2172 */

#define MAX_REST_SIZE_HGV20           6    /* PLU Sales Restriction Table  */
#define MAX_AGE_SIZE_HGV20            3    /* Boundary age  */
#define MAX_DEPTKEY_SIZE_HGV20      450    /* Keyed Dept Key, 2172 */
#define MAX_MISC_SIZE_HGV20           7    /* Misc. Parameter */

#define MAX_TERMINALINFO_SIZE_HGV20  16    /* Max number of terminals in cluster for terminal info */

#define PARA_TRANSMNEMO_LEN_HGV20	 20

#define PARA_PROMOTION_LEN_HGV20      24       /* ParaPromotion */
#define PARA_TRANSMNEMO_LEN_HGV20     20       /* ParaTransMnemo */
#define PARA_LEADTHRU_LEN_HGV20       20       /* ParaLeadThru */
#define PARA_REPORTNAME_LEN_HGV20     6       /* ParaReportName */
#define PARA_SPEMNEMO_LEN_HGV20       4       /* ParaSpeMnemo */
#define PARA_ADJMNEMO_LEN_HGV20       4       /* ParaAdjMnemo */
#define PARA_PRTMODI_LEN_HGV20        10       /* ParaPrtModi */
#define PARA_MAJORDEPT_LEN_HGV20      20       /* ParaMajorDEPT */
#define PARA_CHAR24_LEN_HGV20         24       /* ParaChar24 */

#define MAX_PLUKEY_SIZE_HGV20      1485    /* Keyed PLU Key */
#define MAX_RNDTBL_SIZE_HGV20        15    /* Rounding Table, 2172 */
#define MAX_RND_DATA_HGV20            3    /* Set Data for Rounding */
#define MAX_DISCRATE_SIZE_HGV20      20    /* Discount/Bonus % Rates, V3.3 */
#define MAX_TABLE_NO_HGV20           99    /* OEP TABLE, V3.3 */
#define MAX_TABLE_ADDRESS_HGV20      10    /* OEP TABLE */
#define MAX_ONE_OEPTABLE_SIZE_HGV20  10    /* OEP TABLE */
#define MAX_SOFTCHK_NO_HGV20         10     /* 42 Characters */

#define MAX_PROMO_NO_HGV20            3    /* Line for Promotion Message */

#define MAX_FC_SIZE_HGV20             8    /* Currency Conversion Rate, 2172 */
#define MAX_SETMENU_NO_HGV20         20    /* Set Menu */
#define MAX_SETPLU_DATA_HGV20         6    /* Set PLU  */
#define MAX_TAXRATE_SIZE_HGV20       5    /* Tax Rate */
#define PARA_SOFTCHK_LEN_HGV20        42       /* ParaSoftCheck */

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct	tagUNINTRAM_HGV20 {
    UCHAR	HGV20_ParaMDC[MAX_MDC_SIZE_HGV20];				/* prog #1 */
    FLEXMEM	HGV20_ParaFlexMem[MAX_FLXMEM_SIZE_HGV20];			/* prog #2 */
    PARAFSCTBL	HGV20_ParaFSC[MAX_FSC_TBL_HGV20];				/* prog #3 */
    USHORT	HGV20_ParaSecurityNo;					/* prog #6 */
    USHORT	HGV20_ParaStoRegNo[MAX_STOREG_SIZE_HGV20];			/* prog #7 */
    SUPLEVEL	HGV20_ParaSupLevel[MAX_SUPLEVEL_SIZE_HGV20];		/* prog #8 */
    UCHAR	HGV20_ParaActCodeSec[MAX_ACSEC_SIZE_HGV20];			/* prog #9 */
    UCHAR	HGV20_ParaTransHALO[MAX_HALOKEY_NO_HGV20];			/* prog #10 */
    UCHAR	HGV20_ParaHourlyTime[MAX_HOUR_NO_HGV20][MAX_HM_DATA_HGV20];	/* prog #17 */
    UCHAR	HGV20_ParaSlipFeedCtl[MAX_SLIP_SIZE_HGV20];			/* prog #18 */
    WCHAR	HGV20_ParaTransMnemo[MAX_TRANSM_NO_HGV20][PARA_TRANSMNEMO_LEN_HGV20];	/* prog #20 */
    WCHAR	HGV20_ParaLeadThru[MAX_LEAD_NO_HGV20][PARA_LEADTHRU_LEN_HGV20];	/* prog #21 */
    WCHAR	HGV20_ParaReportName[MAX_REPO_NO_HGV20][PARA_REPORTNAME_LEN_HGV20];	/* prog #22 */
    WCHAR	HGV20_ParaSpeMnemo[MAX_SPEM_NO_HGV20][PARA_SPEMNEMO_LEN_HGV20];	/* prog #23 */
    WCHAR	HGV20_ParaPCIF[MAX_PCIF_SIZE_HGV20];			/* prog #39 */
    WCHAR	HGV20_ParaAdjMnemo[MAX_ADJM_NO_HGV20][PARA_ADJMNEMO_LEN_HGV20];	/* prog #46 */
    WCHAR	HGV20_ParaPrtModi[MAX_PRTMODI_NO_HGV20][PARA_PRTMODI_LEN_HGV20];	/* prog #47 */
    WCHAR	HGV20_ParaMajorDEPT[MAX_MDEPT_NO_HGV20][PARA_MAJORDEPT_LEN_HGV20];	/* prog #48 */
    UCHAR	HGV20_ParaAutoAltKitch[MAX_DEST_SIZE_HGV20];		/* prog #49 */
    CPMDATA_HGV20	HGV20_ParaHotelId;					/* prog #54 */
    WCHAR	HGV20_ParaChar24[MAX_CH24_NO_HGV20][PARA_CHAR24_LEN_HGV20];	/* prog #57 */
    UCHAR	HGV20_ParaTtlKeyTyp[MAX_KEYTYPE_SIZE_HGV20];		/* prog #60 */
    UCHAR	HGV20_ParaTtlKeyCtl[MAX_TTLKEY_NO_HGV20][MAX_TTLKEY_DATA_HGV20];	/* prog #61 */
    UCHAR	HGV20_ParaTend[MAX_TEND_NO_HGV20];				/* prog #62 */
    PLUNO	HGV20_ParaPLUNoMenu[MAX_PLUKEY_SIZE_HGV20];			/* A/C #4, 2172 */
    UCHAR	HGV20_ParaCtlTblMenu[MAX_PAGE_SIZE_HGV20];			/* A/C #5 */
    UCHAR	HGV20_ParaManuAltKitch[MAX_DEST_SIZE_HGV20];		/* A/C #6 */
    USHORT	HGV20_ParaCashABAssign[MAX_AB_SIZE_HGV20];			/* A/C #7 */
    UCHAR	HGV20_ParaRound[MAX_RNDTBL_SIZE_HGV20][MAX_RND_DATA_HGV20];	/* A/C #84 */
    UCHAR	HGV20_ParaDisc[MAX_DISCRATE_SIZE];			/* A/C #86 */
    WCHAR/*UCHAR*/	HGV20_ParaPromotion[MAX_PROMO_NO_HGV20][PARA_PROMOTION_LEN_HGV20 ];	/* A/C #88 */
    ULONG	HGV20_ParaCurrency[MAX_FC_SIZE_HGV20];			/* A/C #89 */
    UCHAR	HGV20_ParaToneCtl[MAX_TONE_SIZE_HGV20];			/* A/C #169 */
    MENUPLU	HGV20_ParaMenuPLU[MAX_SETMENU_NO_HGV20][MAX_SETPLU_DATA_HGV20];	/* A/C #116, 2172 */
    UCHAR	HGV20_ParaTaxTable[MAX_TTBLE_SIZE_HGV20];			/* A/C #124-126 */
    ULONG	HGV20_ParaTaxRate[MAX_TAXRATE_SIZE_HGV20];			/* A/C #127 */
    ULONG   HGV20_ParaMiscPara[MAX_MISC_SIZE_HGV20];                /* MISC. PARAMETER */
    USHORT	HGV20_ParaTare[MAX_TARE_SIZE_HGV20];			/* A/C #111 */
    ULONG	HGV20_ParaPresetAmount[MAX_PRESETAMOUNT_SIZE_HGV20];	/* prog #15 */
    UCHAR	HGV20_ParaSharedPrt[MAX_SHAREDPRT_SIZE_HGV20];		/* prog #50 */
    WCHAR/*UCHAR*/	HGV20_ParaSoftChk[MAX_SOFTCHK_NO_HGV20][PARA_SOFTCHK_LEN_HGV20];	/* A/C #87 */
    ULONG	HGV20_ParaPigRule[MAX_PIGRULE_SIZE_HGV20];			/* A/C #130 */
    UCHAR	HGV20_ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS_HGV20];	/* A/C #160 */
    UCHAR	HGV20_ParaFlexDrive[MAX_FLEXDRIVE_SIZE_HGV20];		/* A/C #162 */
    USHORT	HGV20_ParaServiceTime[MAX_SERVICETIME_SIZE_HGV20];		/* A/C #133 */
    USHORT	HGV20_ParaLaborCost[MAX_LABORCOST_SIZE_HGV20];		/* A/C #154 */
    DISPPARA	HGV20_ParaDispPara[MAX_DISPPARA_SIZE_HGV20];		/* A/C #137 */
	PARAKDS HGV20_ParaKdsIp[MAX_KDSIP_SIZE_HGV20];	/* SR 14 ESMITH */
	RESTRICT     HGV20_ParaRestriction[ MAX_REST_SIZE_HGV20 ];/* AC 170, 2172 */
    UCHAR       HGV20_auchParaBoundAge[ MAX_AGE_SIZE_HGV20 ];               /* A/C 208, 2172 */
    USHORT	HGV20_ParaDeptNoMenu[MAX_DEPTKEY_SIZE_HGV20];			/* A/C #4, 2172 */
    USHORT	HGV20_ParaSpcCo[MAX_CO_SIZE_HGV20];				/* SPECIAL COUNTER */

	CHARGEPOSTINFO HGV20_ChargePostInformation;     // R_20_DATABASECHANGE
	TERMINALINFO  HGV20_TerminalInformation[MAX_TERMINALINFO_SIZE_HGV20];
	CHARGEPOSTINFO HGV20_EPT_ENH_Information[16];     // R_20_DATABASECHANGE
	TENDERKEYINFO  HGV20_TenderKeyInformation[MAX_TEND_NO_HGV20];  // R_20_DATABASECHANGE

	USHORT  usSerCurStatus;                 /* Inquiry status */
    USHORT  ausSerCurTransNo[CLI_ALLTRANSNO];       /* Current Transaction number */
    USHORT  ausSerPreTransNo[CLI_ALLTRANSNO];       /* Previous Transaction number */
    USHORT  fsSerCurBcasInf;                /* Broadcase information */
    CLIINQDATE  SerCurDate;                 /* Inquiry Date */
    TTLCSUPHEAD TtlUphdata;                 /* Total update header */
    CPMTALLY    CpmTally;                   /* Tally counter for CPM */
    EPTTALLY   EPTTally;                    /* Tally counter for EPT,   Saratoga */

} UNINIRAM_HGV20; //Release 30

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
#define RELEASE_30_SIZE 94780




#endif