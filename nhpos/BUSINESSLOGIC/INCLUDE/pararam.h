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
:   Category           : RAM Definition                                    :
:   Program Name       : pararam.h                                             :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-03-18 : 00.00.00     :                         :
:   Update Histories   : 93-08-23 : 01.00.13     : Adds ParaSoftChk        :
:                      :          :              :                         :
:** NCR2172 **
:
:                      : 99-10-05 : 01.00.00     : initial (for Win32)
:
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

/*------------------------------------------------------------------------*\

                    R A M   D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

// NOTE:  See note below concerning range of Para UNINIRAM struct in ParaGenerateHash().

typedef struct  tagUNINTRAM {
	TCHAR    szVer[FILE_LEN_ID];					/* 10-1-2004 JHHJ for file version information*/
    UCHAR   ParaMDC[MAX_MDC_SIZE];                  /* prog #1 */
    FLEXMEM ParaFlexMem[MAX_FLXMEM_SIZE];           /* prog #2 */
    PARAFSCTBL  ParaFSC[MAX_FSC_TBL];               /* prog #3 */
    USHORT  ParaSecurityNo;                         /* prog #6 */
    USHORT  ParaStoRegNo[MAX_STOREG_SIZE];          /* prog #7 */
    SUPLEVEL    ParaSupLevel[MAX_SUPLEVEL_SIZE];                /* prog #8 */
    UCHAR   ParaActCodeSec[MAX_ACSEC_SIZE];                     /* prog #9, odd address lower nibble, even address upper nibble */
    UCHAR   ParaTransHALO[MAX_HALOKEY_NO];                      /* prog #10 */
    UCHAR   ParaHourlyTime[MAX_HOUR_NO][MAX_HM_DATA];           /* prog #17, number of blocks in [0], block interval data in the rest */
    UCHAR   ParaSlipFeedCtl[MAX_SLIP_SIZE];                     /* prog #18 */
    TCHAR   ParaTransMnemo[MAX_TRANSM_NO][PARA_TRANSMNEMO_LEN]; /* prog #20, Transaction mnemonics - CLASS_PARATRANSMNEMO */
    TCHAR   ParaLeadThru[MAX_LEAD_NO][PARA_LEADTHRU_LEN];       /* prog #21, Lead Thru mnemonics - CLASS_PARALEADTHRU */
    TCHAR   ParaReportName[MAX_REPO_NO][PARA_REPORTNAME_LEN];   /* prog #22, Report mnemonics - CLASS_PARAREPORTNAME */
    TCHAR   ParaSpeMnemo[MAX_SPEM_NO][PARA_SPEMNEMO_LEN];       /* prog #23, Special mnemonics - CLASS_PARASPECIALMNEMO  */
    TCHAR   ParaPCIF[MAX_PCIF_SIZE];                            /* prog #39 */
    TCHAR   ParaAdjMnemo[MAX_ADJM_NO][PARA_ADJMNEMO_LEN];       /* prog #46, Adjective mnemonics - CLASS_PARAADJMNEMO */
    TCHAR   ParaPrtModi[MAX_PRTMODI_NO][PARA_PRTMODI_LEN];      /* prog #47, Print Modifier mnemonics - CLASS_PARAPRTMODI */
    TCHAR   ParaMajorDEPT[MAX_MDEPT_NO][PARA_MAJORDEPT_LEN];    /* prog #48, Major Department mnemonics - CLASS_PARAMAJORDEPT */
    UCHAR   ParaAutoAltKitch[MAX_DEST_SIZE];        /* prog #49 */
    CPMDATA ParaHotelId;                    /* prog #54 */
    TCHAR   ParaChar24[MAX_CH24_NO][PARA_CHAR24_LEN];   /* prog #57 */
    UCHAR   ParaTtlKeyTyp[MAX_KEYTYPE_SIZE];        /* prog #60 */
    UCHAR   ParaTtlKeyCtl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];  /* prog #61 */
    UCHAR   ParaTend[MAX_TEND_NO];              /* prog #62 */
    PLUNO   ParaPLUNoMenu[MAX_PLUKEY_SIZE];         /* A/C #4, NCR 2172, first PLU_MAX_TOUCH_PRESET slots used for Touchscreen */
    UCHAR   ParaCtlTblMenu[MAX_PAGE_SIZE];          /* A/C #5 */
    UCHAR   ParaManuAltKitch[MAX_DEST_SIZE];        /* A/C #6 */
    ULONG   ParaCashABAssign[MAX_AB_SIZE];          /* A/C #7 */
    UCHAR   ParaRound[MAX_RNDTBL_SIZE][MAX_RND_DATA];   /* A/C #84 */
    UCHAR   ParaDisc[MAX_DISCRATE_SIZE];            /* A/C #86 */
    TCHAR   ParaPromotion[MAX_PROMO_NO][PARA_PROMOTION_LEN];    /* A/C #88 */
    ULONG   ParaCurrency[MAX_FC_SIZE];          /* A/C #89 */
    UCHAR   ParaToneCtl[MAX_TONE_SIZE];         /* A/C #169 */
    MENUPLU ParaMenuPLU[MAX_SETMENU_NO][MAX_SETPLU_DATA];   /* A/C #116, 2172 */
    UCHAR   ParaTaxTable[MAX_TTBLE_SIZE];           /* A/C #124-126, same size as TAXDATA */
    ULONG   ParaTaxRate[MAX_TAXRATE_SIZE];          /* A/C #127 */
    ULONG   ParaMiscPara[MAX_MISC_SIZE];                /* MISC. PARAMETER */
    USHORT  ParaTare[MAX_TARE_SIZE];            /* A/C #111 */
    ULONG   ParaPresetAmount[MAX_PRESETAMOUNT_SIZE];    /* prog #15. may be 4 ULONG values or 8 USHORT values for Preset Amount Tender. See also FSC_PREAMT_MAX and MDC 82 Bit A, MDC_DRAWER2_ADR */
    UCHAR   ParaSharedPrt[MAX_SHAREDPRT_SIZE];      /* prog #50 See ParaSharedPrtRead() - shared and alternate printers for 16 terminals, shared printer for 8 kitchen printers, COM port for 8 kitchen printers */
    TCHAR   ParaSoftChk[MAX_SOFTCHK_NO][PARA_SOFTCHK_LEN];  /* A/C #87 */
    ULONG   ParaPigRule[MAX_PIGRULE_SIZE];          /* A/C #130 */
    UCHAR   ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS];   /* A/C #160 */
    UCHAR   ParaFlexDrive[MAX_FLEXDRIVE_SIZE];      /* A/C #162 */
    USHORT  ParaServiceTime[MAX_SERVICETIME_SIZE];      /* A/C #133 */
    USHORT  ParaLaborCost[MAX_LABORCOST_SIZE];      /* A/C #154 */
    DISPPARA    ParaDispPara[MAX_DISPPARA_SIZE];        /* A/C #137 */
    PARAKDS	ParaKdsIp[MAX_KDSIP_SIZE];					/* SR 14 ESMITH */
	//USHORT  ParaKdsIp[MAX_KDSIP_SIZE][PARA_KDSIP_LEN];  /* Prog, 51, 2172 */
    RESTRICT     ParaRestriction[ MAX_REST_SIZE ];/* AC 170, 2172 */
    UCHAR   auchParaBoundAge[ MAX_AGE_SIZE ];    /* A/C 208, 2172 */
    USHORT  ParaDeptNoMenu[MAX_DEPTKEY_SIZE];           /* A/C #4, 2172 */
    USHORT  ParaSpcCo[MAX_CO_SIZE];             /* SPECIAL COUNTER */
    ULONG	ParaAutoCoupon[MAX_AUTOCOUPON_SIZE];				/* Prog. #67 ACC Controls */
	NHPOSCOLORREF	ParaColorPalette[MAX_COLORPALETTE_SIZE][2];	/*A/C #209 */
	UCHAR	ParaTtlKeyOrderDecType[MAX_TTLKEY_NO];

	CHARGEPOSTINFO ChargePostInformation;     // R_20_DATABASECHANGE
	TERMINALINFO  TerminalInformation[MAX_TERMINALINFO_SIZE];
	CHARGEPOSTINFO EPT_ENH_Information[16];     // R_20_DATABASECHANGE
	TENDERKEYINFO  TenderKeyInformation[MAX_TEND_NO];  // R_20_DATABASECHANGE
    ULONG          ParaStoreForward[MAX_MISC_SIZE];    // area used for the Store and Forward functionality
	USHORT      ParaReasonCode[MAX_REASON_CODE_ITEMS][MAX_REASON_CODE_ENTRIES];   /* reason code data: first code in range, number in range */
    USHORT         ParaSuggestedTip[MAX_SUGGESTED_TIP];

    USHORT  usSerCurStatus;                 /* Inquiry status */
    USHORT  ausSerCurTransNo[CLI_ALLTRANSNO];       /* Current Transaction number */
    USHORT  ausSerPreTransNo[CLI_ALLTRANSNO];       /* Previous Transaction number */
    USHORT  fsSerCurBcasInf;                /* Broadcase information */
    CLIINQDATE  SerCurDate;                 /* Inquiry Date */
    TTLCSUPHEAD TtlUphdata;                 /* Total update header */
    CPMTALLY    CpmTally;                   /* Tally counter for CPM */
    EPTTALLY   EPTTally;                    /* Tally counter for EPT,   Saratoga */
	TTLCSUPHEAD TtlUphDBdata;                 /* Total Delayed Balance update header */
	UCHAR		uchMaintDelayBalanceFlag;
	UCHAR       uchStoreForwardFlag;        // Store and Forward status bit map. See PIF_STORE_AND_FORWARD_STAT_MASK
	ULONG       ulConnEngineElectronicJournalConsecutiveNumber;
	ULONG       ulAssrtlogFileOffset;
} UNINIRAM;

extern UNINIRAM UNINT Para;                                 /* Uninitialize RAM */
extern UNINIRAM UNINT ParaDefault;                          /* Default, initialized copy of Para RAM */

// NOTE:  We are generating a hash code using function ParaGenerateHash() to indicate
//        to Satellite Terminals if there has been a change in the Master Terminal provisioning.
//        If the UNINIRAM is modified, check the hash function to see if it also needs to be
//        modified to include the changes in the range of the hash calculation.

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#define ParaPLUNoMenu       (Para.ParaPLUNoMenu)
#define ParaRound       (Para.ParaRound)
#define ParaDisc        (Para.ParaDisc)
#define ParaOep         (Para.ParaOep)
#define ParaFlexDrive       (Para.ParaFlexDrive)
#define ParaPromotion       (Para.ParaPromotion)
#define ParaCurrency        (Para.ParaCurrency)
#define ParaMenuPLU     (Para.ParaMenuPLU)
#define ParaTaxRate         (Para.ParaTaxRate)
#define ParaMiscPara        (Para.ParaMiscPara)
#define ParaMDC             (Para.ParaMDC)
#define ParaFSC             (Para.ParaFSC)
#define ParaSecurityNo          (Para.ParaSecurityNo)
#define ParaSupLevel            (Para.ParaSupLevel)
#define ParaActCodeSec          (Para.ParaActCodeSec)
#define ParaTransHALO           (Para.ParaTransHALO)
#define ParaHourlyTime          (Para.ParaHourlyTime)
#define ParaSlipFeedCtl         (Para.ParaSlipFeedCtl)
//#define ParaTransMnemo          (Para.ParaTransMnemo)        removed as part of C99 Upgrade project
//#define Para_LeadThru           (Para.ParaLeadThru)          removed as part of C99 Upgrade project
#define ParaReportName          (Para.ParaReportName)
//#define ParaSpeMnemo            (Para.ParaSpeMnemo)
//#define ParaTaxTable            (Para.ParaTaxTable)          removed as part of C99 Upgrade project
//#define ParaAdjMnemo            (Para.ParaAdjMnemo)
#define ParaPrtModi         (Para.ParaPrtModi)
#define ParaMajorDEPT           (Para.ParaMajorDEPT)
//#define ParaChar24          (Para.ParaChar24)                removed as part of C99 Upgrade project
#define ParaCtlTblMenu          (Para.ParaCtlTblMenu)
#define ParaAutoAltKitch        (Para.ParaAutoAltKitch)
#define ParaFlexMem         (Para.ParaFlexMem)
#define ParaStoRegNo            (Para.ParaStoRegNo)
#define ParaTtlKeyTyp           (Para.ParaTtlKeyTyp)
#define ParaTtlKeyCtl           (Para.ParaTtlKeyCtl)
#define ParaTend            (Para.ParaTend)
#define ParaManuAltKitch        (Para.ParaManuAltKitch)
//#define ParaCashABAssign        (Para.ParaCashABAssign)
//#define ParaSpcCo           (Para.ParaSpcCo)
#define ParaPCIF            (Para.ParaPCIF)
#define ParaToneCtl         (Para.ParaToneCtl)
#define ParaTare            (Para.ParaTare)
#define Para_SharedPrt          (Para.ParaSharedPrt)
#define ParaPresetAmount        (Para.ParaPresetAmount)
#define ParaSoftChk         (Para.ParaSoftChk)
#define ParaPigRule         (Para.ParaPigRule)
//#define Para_HotelId            (Para.ParaHotelId)    removed as part of C99 Upgrade project
#define ParaServiceTime         (Para.ParaServiceTime)
#define ParaLaborCost           (Para.ParaLaborCost)
#define Para_DispPara           (Para.ParaDispPara)
#define usSerCurStatus      (Para.usSerCurStatus)
//#define ausSerCurTransNo    (Para.ausSerCurTransNo)    removed as part of C99 Upgrade project
//#define ausSerPreTransNo    (Para.ausSerPreTransNo)    removed as part of C99 Upgrade project
#define fsSerCurBcasInf     (Para.fsSerCurBcasInf)
//#define SerCurDate      (Para.SerCurDate)    removed as part of C99 Upgrade project
#define TtlUphdata      (Para.TtlUphdata)
#define Para_CpmTally       (Para.CpmTally)
//#define Para_EPTTally       (Para.EPTTally)    removed as part of C99 Upgrade project
#define Para_KdsIp          (Para.ParaKdsIp)
#define ParaRestriction     (Para.ParaRestriction)
#define ParaBoundAge        (Para.auchParaBoundAge)
#define ParaDeptNoMenu      (Para.ParaDeptNoMenu)
#define ParaAutoCoupon		(Para.ParaAutoCoupon)
#define ParaColorPalette	(Para.ParaColorPalette)

#define ParaEnhEPTInfo				(Para.EPT_ENH_Information)
#define ParaChargePostInformation	(Para.ChargePostInformation)
#define ParaTerminalInformationPara  (Para.TerminalInformation)
#define ParaTenderKeyInformation     (Para.TenderKeyInformation)
#define ParaFileVersion				(Para.szVer)

#define TtlUphDBdata      (Para.TtlUphDBdata)

#define uchMaintDelayBalanceFlag (Para.uchMaintDelayBalanceFlag)

/****** End of Definition ******/
