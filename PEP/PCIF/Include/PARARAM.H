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
:   Program Name       : pararam.h                                         :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-03-18 : 00.00.00     :                         :
:   Update Histories   : 93-08-23 : 01.00.13     : Adds ParaSoftChk        :
:                      :          :              :                         :
:                      : 00-01-25 : 01.00.00     : Saratoga initial        :
:                      :          :              :                         :
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
/*------------------------------------------------------------------------*\

                    R A M   D E C L A R A T I O N s

\*------------------------------------------------------------------------*/
    
extern UCHAR   ParaMDC[MAX_MDC_SIZE];                      /* SET MDC */                              
extern FLEXMEM   ParaFlexMem[MAX_FLXMEM_SIZE];             /* FLEXIBLE MEMORY ASSIGNMENT */           
extern PARAFSCTBL  ParaFSC[MAX_FSC_TBL];                  /* SET FSC */                              
extern USHORT  ParaSecurityNo;                             /* SECURITY NUMBER FOR PROGRAM MODE */     
extern USHORT  ParaStoRegNo[MAX_STOREG_SIZE];              /* STORE/REGISTER NO. */                   
extern SUPLEVEL   ParaSupLevel[MAX_SUPLEVEL_SIZE];         /* SUPERVISOR NUMBER/LEVEL ASSINMENT */    
extern UCHAR   ParaActCodeSec[MAX_ACSEC_SIZE];             /* ACTION CODE SECURITY */                 
extern UCHAR   ParaTransHALO[MAX_HALOKEY_NO];              /* TRANSACTION HALO */                     
extern UCHAR   ParaHourlyTime[MAX_HOUR_NO][MAX_HM_DATA];   /* HOURLY ACTIVITY TIME */                 
extern UCHAR   ParaSlipFeedCtl[MAX_SLIP_SIZE];            /* SLIP PRINTER FEED CONTROL */            
extern UCHAR   ParaTransMnemo[MAX_TRANSM_NO][PARA_TRANSMNEMO_LEN];   /* TRANSACTION MNEMONICS */                
extern UCHAR   ParaLeadThru[MAX_LEAD_NO][PARA_LEADTHRU_LEN];   /* LEAD THROUGH TABLE */                   
extern UCHAR   ParaReportName[MAX_REPO_NO][PARA_REPORTNAME_LEN];   /* REPORT NAME */                          
extern UCHAR   ParaSpeMnemo[MAX_SPEM_NO][PARA_SPEMNEMO_LEN];  /* SPECIAL MNEMONICS */                    
extern UCHAR   ParaPCIF[MAX_PCIF_SIZE];                    /* PC I/F */
extern UCHAR   ParaAdjMnemo[MAX_ADJM_NO][PARA_ADJMNEMO_LEN];   /* ADJECTIVE MNEMONICS */                  
extern UCHAR   ParaPrtModi[MAX_PRTMODI_NO][PARA_PRTMODI_LEN];  /* PRINT MODIFIER MNEMONICS */             
extern UCHAR   ParaMajorDEPT[MAX_MDEPT_NO][PARA_MAJORDEPT_LEN];  /* REPORT CODE MNEMONICS */                
extern UCHAR   ParaAutoAltKitch[MAX_DEST_SIZE];            /* AUTO ALTERNATIVE KITCHEN PTR. */        
extern CPMDATA  ParaHotelId;                               /* Prog 54 */
extern UCHAR   ParaChar24[MAX_CH24_NO][PARA_CHAR24_LEN];  /* 24 CHARACTERS MNEMONICS */              
extern UCHAR   ParaTtlKeyTyp[MAX_KEYTYPE_SIZE];            /* TOTAL KEY TYPE ASSIGNMENT */            
extern UCHAR   ParaTtlKeyCtl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];   /* TOTAL KEY CONTROL */                    
extern UCHAR   ParaTend[MAX_TEND_NO];                      /* TENDER PARAMETER,    V3.3 */
extern PLUNO   ParaPLUNoMenu[MAX_PLUKEY_SIZE];             /* SET PLU NO.ON MENU PAGE, Saratoga */
extern UCHAR   ParaCtlTblMenu[MAX_PAGE_SIZE];              /* SET CONTROL TABLE OF MENU PAGE */       
extern UCHAR   ParaManuAltKitch[MAX_DEST_SIZE];            /* MANUAL ALTERNATIVE KITCHEN PTR. */      
extern USHORT  ParaCashABAssign[MAX_AB_SIZE];              /* CASHIER A/B KEYS ASSIGNMENT */          
extern UCHAR   ParaRound[MAX_RNDTBL_SIZE][MAX_RND_DATA];   /* SET ROUNDING TABLE */                   
extern UCHAR   ParaDisc[MAX_DISCRATE_SIZE];               /* SET DISCOUNT/BONUS % RATE */             
extern UCHAR   ParaPromotion[MAX_PROMO_NO][PARA_PROMOTION_LEN];      /* SET SALES PROMOTION MESSAGE */          
extern ULONG   ParaCurrency[MAX_FC_SIZE];                 /* SET CURRENCY CONVERSION RATE */         
extern UCHAR  ParaToneCtl[MAX_TONE_SIZE];                  /* TONE CONTROL */
extern MENUPLU  ParaMenuPLU[MAX_SETMENU_NO][MAX_SETPLU_DATA];   /* ASSIGN SET MENU OF PLU */                  
extern UCHAR   ParaTaxTable[MAX_TTBLE_SIZE];              /* TAX TABLE PROGRAMMING */                
extern ULONG   ParaTaxRate[MAX_TAXRATE_SIZE];             /* SET TAX/SERVICE/RECIPROCAL RATES */     
extern ULONG   ParaMiscPara[MAX_MISC_SIZE];                     /* MISC. PARAMETER */
extern USHORT  ParaTare[MAX_TARE_SIZE];                    /* TARE TABLE   */
extern ULONG  ParaPresetAmount[MAX_PRESETAMOUNT_SIZE];     /* PRESET AMOUNT CASH TENDER */
extern UCHAR  ParaSharedPrt[MAX_SHAREDPRT_SIZE];           /* SHARED PRINTER */
extern UCHAR   ParaSoftChk[MAX_SOFTCHK_NO][PARA_SOFTCHK_LEN]; /*  87 42 CHARACTERS MNEMONICS for SOFT CHECK */              
extern ULONG   ParaPigRule[MAX_PIGRULE_SIZE];              /* 130 PIG RULE TABLE */
extern UCHAR   ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS];   /* SET ORDER ENTRY PROMPT TABLE */
extern UCHAR   ParaFlexDrive[MAX_FLEXDRIVE_SIZE];          /* SET FLEXIBLE DRIVE THROUGH PARAMETER TABLE */
extern USHORT  ParaServiceTime[MAX_SERVICETIME_SIZE];      /* SERVICE TIME PARAMETER   */
extern USHORT  ParaLaborCost[MAX_LABORCOST_SIZE];          /* LABOR COST VALUE   */
extern DISPPARA  ParaDispPara[MAX_DISPPARA_SIZE];         /* BEVERAGE DISPENSER PARAMETER */
extern PARAKDS  ParaKdsIp[MAX_KDSIP_SIZE][PARA_KDSIP_LEN];  /* Assigment IP address for KDS, 2172 */
extern RESTRICT   ParaRestriction[ MAX_REST_SIZE ];           /* Set Sales Restriction Table of PLU, 2172 */
extern UCHAR      auchParaBoundAge[ MAX_AGE_SIZE ];           /* Set Boundary Age, 2172 */
extern USHORT     ParaDeptNoMenu[MAX_DEPTKEY_SIZE];           /* Set Dept No.on Keyed Dept Key, 2172 */
extern USHORT  ParaSpcCo[MAX_CO_SIZE];                     /* SPECIAL COUNTER */

extern CHARGEPOSTINFO ChargePostInformation;     // R_20_DATABASECHANGE
extern TERMINALINFO  TerminalInformation[MAX_TERMINALINFO_SIZE];
extern CHARGEPOSTINFO EPT_ENH_Information[16];     // R_20_DATABASECHANGE


 
/****** End of Definition ******/
