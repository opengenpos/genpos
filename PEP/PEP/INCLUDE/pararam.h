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

//#include <windows.h>

/*------------------------------------------------------------------------*\

                    R A M   D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/*  RJC pragma pack uncommented here */
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct	tagUNINTRAM {
	WCHAR   szVer[FILE_LEN_ID];					/* 10-1-2004 JHHJ for file version information, this area is not saved in PEP file*/
    UCHAR	ParaMDC[MAX_MDC_SIZE];				/* prog #1, beginning of the saved area of memory resident database */
    FLEXMEM	ParaFlexMem[MAX_FLXMEM_SIZE];			/* prog #2 */
    PARAFSCTBL	ParaFSC[MAX_FSC_TBL];				/* prog #3 */
    USHORT	ParaSecurityNo;					/* prog #6 */
    USHORT	ParaStoRegNo[MAX_STOREG_SIZE];			/* prog #7 */
    SUPLEVEL	ParaSupLevel[MAX_SUPLEVEL_SIZE];		/* prog #8 */
    UCHAR	ParaActCodeSec[MAX_ACSEC_SIZE];			/* prog #9 */
    UCHAR	ParaTransHALO[MAX_HALOKEY_NO];			/* prog #10 */
    UCHAR	ParaHourlyTime[MAX_HOUR_NO][MAX_HM_DATA];	/* prog #17 */
    UCHAR	ParaSlipFeedCtl[MAX_SLIP_SIZE];			/* prog #18 */
    WCHAR	ParaTransMnemo[MAX_TRANSM_NO][PARA_TRANSMNEMO_LEN];	/* prog #20 */
    WCHAR	ParaLeadThru[MAX_LEAD_NO][PARA_LEADTHRU_LEN];	/* prog #21 */
    WCHAR	ParaReportName[MAX_REPO_NO][PARA_REPORTNAME_LEN];	/* prog #22 */
    WCHAR	ParaSpeMnemo[MAX_SPEM_NO][PARA_SPEMNEMO_LEN];	/* prog #23 */
    WCHAR	ParaPCIF[MAX_PCIF_SIZE];			/* prog #39 */
    WCHAR	ParaAdjMnemo[MAX_ADJM_NO][PARA_ADJMNEMO_LEN];	/* prog #46 */
    WCHAR	ParaPrtModi[MAX_PRTMODI_NO][PARA_PRTMODI_LEN];	/* prog #47 */
    WCHAR	ParaMajorDEPT[MAX_MDEPT_NO][PARA_MAJORDEPT_LEN];	/* prog #48 */
    UCHAR	ParaAutoAltKitch[MAX_DEST_SIZE];		/* prog #49 */
    CPMDATA	ParaHotelId;					/* prog #54 */
    WCHAR	ParaChar24[MAX_CH24_NO][PARA_CHAR24_LEN];	/* prog #57 */
    UCHAR	ParaTtlKeyTyp[MAX_KEYTYPE_SIZE];		/* prog #60 */
    UCHAR	ParaTtlKeyCtl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];	/* prog #61 */
    UCHAR	ParaTend[MAX_TEND_NO];				/* prog #62 */
    PLUNO	ParaPLUNoMenu[MAX_PLUKEY_SIZE];			/* A/C #4, 2172 */
    UCHAR	ParaCtlTblMenu[MAX_PAGE_SIZE];			/* A/C #5 */
    UCHAR	ParaManuAltKitch[MAX_DEST_SIZE];		/* A/C #6 */
    ULONG	ParaCashABAssign[MAX_AB_SIZE];			/* A/C #7 */
	USHORT	ParaCashABAssignJob[MAX_AB_JOB_SIZE];	// AC #7 New in version 2.2.0
    UCHAR	ParaRound[MAX_RNDTBL_SIZE][MAX_RND_DATA];	/* A/C #84 */
    UCHAR	ParaDisc[MAX_DISCRATE_SIZE];			/* A/C #86 */
    WCHAR/*UCHAR*/	ParaPromotion[MAX_PROMO_NO][PARA_PROMOTION_LEN ];	/* A/C #88 */
    ULONG	ParaCurrency[MAX_FC_SIZE];			/* A/C #89 */
    UCHAR	ParaToneCtl[MAX_TONE_SIZE];			/* A/C #169 */
    MENUPLU	ParaMenuPLU[MAX_SETMENU_NO][MAX_SETPLU_DATA];	/* A/C #116, 2172 */
    UCHAR	ParaTaxTable[MAX_TTBLE_SIZE];			/* A/C #124-126 */
    ULONG	ParaTaxRate[MAX_TAXRATE_SIZE];			/* A/C #127 */
    ULONG   ParaMiscPara[MAX_MISC_SIZE];                /* MISC. PARAMETER */
    USHORT	ParaTare[MAX_TARE_SIZE];			/* A/C #111 */
    ULONG	ParaPresetAmount[MAX_PRESETAMOUNT_SIZE];	/* prog #15 */
    UCHAR	ParaSharedPrt[MAX_SHAREDPRT_SIZE];		/* prog #50 */
    WCHAR/*UCHAR*/	ParaSoftChk[MAX_SOFTCHK_NO][PARA_SOFTCHK_LEN];	/* A/C #87 */
    ULONG	ParaPigRule[MAX_PIGRULE_SIZE];			/* A/C #130 */
    UCHAR	ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS];	/* A/C #160 */
    UCHAR	ParaFlexDrive[MAX_FLEXDRIVE_SIZE];		/* A/C #162 */
    USHORT	ParaServiceTime[MAX_SERVICETIME_SIZE];		/* A/C #133 */
    USHORT	ParaLaborCost[MAX_LABORCOST_SIZE];		/* A/C #154 */
    DISPPARA	ParaDispPara[MAX_DISPPARA_SIZE];		/* A/C #137 */
	PARAKDS ParaKdsIp[MAX_KDSIP_SIZE];	/* SR 14 ESMITH */
	RESTRICT     ParaRestriction[ MAX_REST_SIZE ];/* AC 170, 2172 */
    UCHAR       auchParaBoundAge[ MAX_AGE_SIZE ];               /* A/C 208, 2172 */
    USHORT	ParaDeptNoMenu[MAX_DEPTKEY_SIZE];			/* A/C #4, 2172 */
	USHORT	ParaSpcCo[MAX_CO_SIZE];				/* SPECIAL COUNTER */
	ULONG	ParaAutoCoupon[MAX_AUTOCOUPON_SIZE];					/* Prog. #67  ACC Controls */ //RLZ
	NHPOSCOLORREF	ParaColorPalette[MAX_COLORPALETTE_SIZE][2];	/*A/C #209 Palette[i][0] -> Foreground color, Palette[i][1] -> Background color */
	UCHAR	ParaTtlKeyOrderDecType[MAX_TTLKEY_NO];

	CHARGEPOSTINFO ChargePostInformation;     // R_20_DATABASECHANGE
	TERMINALINFO  TerminalInformation[MAX_TERMINALINFO_SIZE];
	CHARGEPOSTINFO EPT_ENH_Information[16];     // R_20_DATABASECHANGE
	TENDERKEYINFO  TenderKeyInformation[MAX_TEND_NO];  // R_20_DATABASECHANGE
    ULONG              ParaStoreForward[MAX_MISC_SIZE];    // area used for the Store and Forward functionality
	USHORT         ParaReasonCode[MAX_REASON_CODE_ITEMS][MAX_REASON_CODE_ENTRIES];   /* reason code data: first code in range, number in range */
    USHORT         ParaSuggestedTip[MAX_SUGGESTED_TIP];
} UNINIRAM;


extern UNINIRAM Para;                               	/* Uninitialize RAM */


/*  RJC  pragma pack uncommented here */
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

// In some cases we do not want to define the following macros
// especially in those cases when we are using pointers with a
// UNINIRAM struct or a version of a UNINIRAM struct as in the
// case of PEP data file conversion.
#if !defined(NO_PARA_DEFINES)

#define ParaPLUNoMenu		(Para.ParaPLUNoMenu)
#define ParaRound		(Para.ParaRound)
#define ParaDisc		(Para.ParaDisc)
#define ParaOep			(Para.ParaOep)
#define ParaFlexDrive		(Para.ParaFlexDrive)
#define ParaPromotion		(Para.ParaPromotion)
#define ParaCurrency		(Para.ParaCurrency)
#define ParaMenuPLU		(Para.ParaMenuPLU)
#define ParaTaxRate	        (Para.ParaTaxRate)
#define ParaMiscPara        (Para.ParaMiscPara)
#define ParaMDC		        (Para.ParaMDC)
#define ParaFSC		        (Para.ParaFSC)
#define ParaSecurityNo	        (Para.ParaSecurityNo)
#define	ParaSupLevel	        (Para.ParaSupLevel)
#define ParaActCodeSec	        (Para.ParaActCodeSec)
#define ParaTransHALO	        (Para.ParaTransHALO)
#define ParaHourlyTime	        (Para.ParaHourlyTime)
#define ParaSlipFeedCtl	        (Para.ParaSlipFeedCtl)
#define ParaTransMnemo	        (Para.ParaTransMnemo)
#define Para_LeadThru	        (Para.ParaLeadThru) /* ### MOD Saratoga (ParaLead -> Para_Lead)(051900) */
#define ParaReportName	        (Para.ParaReportName)
#define ParaSpeMnemo	        (Para.ParaSpeMnemo)
#define ParaTaxTable	        (Para.ParaTaxTable)
#define ParaAdjMnemo	        (Para.ParaAdjMnemo)
#define ParaPrtModi	        (Para.ParaPrtModi)
#define ParaMajorDEPT	        (Para.ParaMajorDEPT)
#define ParaChar24	        (Para.ParaChar24)
#define ParaCtlTblMenu	        (Para.ParaCtlTblMenu)
#define ParaAutoAltKitch       	(Para.ParaAutoAltKitch)
#define ParaFlexMem	        (Para.ParaFlexMem)
#define ParaStoRegNo	        (Para.ParaStoRegNo)
#define ParaTtlKeyTyp	        (Para.ParaTtlKeyTyp)
#define ParaTtlKeyCtl	        (Para.ParaTtlKeyCtl)
#define ParaTend	        (Para.ParaTend)
#define ParaManuAltKitch        (Para.ParaManuAltKitch)
#define ParaCashABAssign        (Para.ParaCashABAssign)
#define ParaCashABAssignJob		(Para.ParaCashABAssignJob)
#define ParaSpcCo	        (Para.ParaSpcCo)
#define ParaPCIF	        (Para.ParaPCIF)
#define ParaToneCtl	        (Para.ParaToneCtl)
#define ParaTare	        (Para.ParaTare)
#define Para_SharedPrt	        (Para.ParaSharedPrt) /* ### MOD Saratoga (ParaSha.. -> Para_Sha..)(051900) */
#define ParaPresetAmount        (Para.ParaPresetAmount)
#define ParaSoftChk	        (Para.ParaSoftChk)
#define ParaPigRule	        (Para.ParaPigRule)
#define Para_HotelId	        (Para.ParaHotelId)   /* ### MOD Saratoga (ParaHo.. -> Para_Ho..)(051900) */
#define ParaServiceTime	        (Para.ParaServiceTime)
#define ParaLaborCost	        (Para.ParaLaborCost)
#define Para_DispPara	        (Para.ParaDispPara)  /* ### ADD Saratoga (ParaD.. -> Para_D..) (051900) */
#define ParaSpcCo		(Para.ParaSpcCo)
/* ### ADD Saratoga (051900) */
#define usSerCurStatus      (Para.usSerCurStatus)
#define ausSerCurTransNo    (Para.ausSerCurTransNo)
#define ausSerPreTransNo    (Para.ausSerPreTransNo)
#define fsSerCurBcasInf     (Para.fsSerCurBcasInf)
#define SerCurDate      (Para.SerCurDate)
#define TtlUphdata      (Para.TtlUphdata)
#define Para_CpmTally       (Para.CpmTally)
#define Para_EPTTally       (Para.EPTTally)
#define Para_KdsIp          (Para.ParaKdsIp)
#define ParaRestriction     (Para.ParaRestriction)
#define ParaBoundAge        (Para.auchParaBoundAge)
#define ParaDeptNoMenu      (Para.ParaDeptNoMenu)
#define ParaAutoCoupon		(Para.ParaAutoCoupon)
#define ParaColorPalette	(Para.ParaColorPalette)
#define ParaTtlKeyOrderDecType	(Para.ParaTtlKeyOrderDecType)

#define ParaTerminalInformationPara  (Para.TerminalInformation)
#define ParaTenderKeyInformation     (Para.TenderKeyInformation)
#define ParaFileVersion				 (Para.szVer)
#define ParaStoreForward		     (Para.ParaStoreForward)
#define ParaReasonCode          (Para.ParaReasonCode)

#endif
/****** End of Definition ******/
