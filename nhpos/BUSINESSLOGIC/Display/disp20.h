/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Display for 2x20 Module Header                         
* Category    : Display, NCR 2170 US Hospitality Application        
* Program Name: DISP20.H
* --------------------------------------------------------------------------
* Abstract:   
*
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
:                      : YY-MM-DD   Ver.Rev          Comments               :
:   Program Written    : 92-05-05 : 00.00.00     :                          :
:   Update Histories   : 92-11-19 : 01.00.00     : Adds PARATARE            :
:                      : 92-11-24 : 01.00.00     : Adds PARATARE Declaration:
:                      :          :              : Prototype                :
:                      : 93-07-01 : 01.00.12     : Adds DispSupEmployeeNo20 :
:                      : 93-07-02 : 01.00.12     : Adds DispSupPresetAmount20
:                      : 93-10-05 : 02.00.01     : Adds DispSupEtkFile20()   :
:                      :          :              : DispSupMakeTime()
:                      : 99-08-11 : 03.05.00     : remove WAITER_MODEL
*** NCR2172 ***
:                      : 99-10-04 : 01.00.00     : Initial
:                                                : Add #pragma pack(...)
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

/*------------------------------------------------------------------------*\

            D E F I N I T I O N    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/* #define PifLog      PifAbort    */

/*
*------------------------------------------------------------------------------
*   2X20 DISPLAY'S DESCRIPTOR NO DEFINE DECRALATIONS
*   See functions DispRegOpDscrCntr() and DispSupOpDscrCntr() for
*   source code that uses these defines.
*------------------------------------------------------------------------------
*/
#define _2X20_COMM           0   
#define _2X20_MASTER         1   
#define _2X20_B_MASTER       2   
#define _2X20_PRT_OFFLINE    3   
#define _2X20_NEW_STORE      4   
#define _2X20_TRAIN          5    // indicates flDispRegDescrControl & TRAIN_CNTRL
#define _2X20_P_BALANCE      6  
#define _2X20_TXBLE_TRANS    7    // indicates flDispRegDescrControl & (TAXMOD_CNTRL | TAXEXMPT_CNTRL | TRANSFR_CNTRL)
#define _2X20_PAYMENT_RECALL 8   
#define _2X20_MINUS          9  /* %/(-) Descriptor */ 
#define _2X20_VOID_DELETE    10   // indicates flDispRegDescrControl & (VOID_CNTRL | EC_CNTRL)
#define _2X20_CRED_ALPHA     11   // indicates flDispRegDescrControl & CRED_CNTRL
#define _2X20_T_VOID         12   // indicates flDispRegDescrControl & TVOID_CNTRL           
#define _2X20_RECEIPT        13   // indicates flDispRegDescrControl & RECEIPT_CNTRL
#define _2X20_TOTAL_B_DUE    14  
#define _2X20_CHANGE         15   // indicates flDispRegDescrControl & CHANGE_CNTRL
#define _2x20_KDS			 16 //SR 236 JHHJ KDS Down
#define _2x20_DELAY_BALANCE	 17	//Delayed Balance JHHJ
#define _2X20_SF			 18	// Store and Forward indicator
#define _2X20_DISCON_SAT	     19  // Disconnected Satellite indicator
#define _2X20_DISCON_SAT_TOTALS	 20  // Disconnected Satellite that has totals indicator
#define _2X20_SF_TRANSFERRING	 21  // Store and Forward indicator, Transferring
                            
/*
*------------------------------------------------------------------------------
*   MASKING DATA FOR 2X20 DISPLAY'S DESCRIPTOR CONTROL
*
* The following masks are used to map functionality state indicators to a
* specific displayed indicator. A specific displayed indicator may be changed
* based on several, mutually exclusive functionality state indicators. In
* other words what a displayed indicator indicates may depend on the current
* context of the software and the action resulting from the last key press.
*
* For instance if the descriptor control flag bit map has the two indicators (bits)
* of VOID_CNTRL and/or EC_CNTRL then this represents a state of VOID_DELETE_20_CONTROL
* 
*------------------------------------------------------------------------------
*/
#define RECEIPT_20_CONTROL         RECEIPT_CNTRL   
#define PRT_OFFLINE_20_CONTROL     PRT_OFFLINE_CNTRL
#define VOID_DELETE_20_CONTROL     (VOID_CNTRL | EC_CNTRL)  
#define T_VOID_20_CONTROL          TVOID_CNTRL  
#define MINUS_20_CONTROL           (ITEMDISC_CNTRL | REGDISC_CNTRL | CHRGTIP_CNTRL)
#define CRED_ALPHA_20_CONTROL      CRED_CNTRL   
#define TXBLE_TRANS_20_CONTROL     (TAXMOD_CNTRL | TAXEXMPT_CNTRL | TRANSFR_CNTRL)   
#define TOTAL_B_DUE_20_CONTROL     (SUBTTL_CNTRL | TOTAL_CNTRL | TENDER_CNTRL | PTEND_CNTRL \
                                   | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL | DECTIP_CNTRL)
#define CHANGE_20_CONTROL          CHANGE_CNTRL   
#define NEW_STORE_20_CONTROL       (NEWCHK_CNTRL | STORE_CNTRL)   
#define P_BALANCE_20_CONTROL       PBAL_CNTRL  
#define PAYMENT_RECALL_20_CONTROL  (PAYMENT_CNTRL | RECALL_CNTRL)
#define TRAIN_20_CONTROL           TRAIN_CNTRL  
#define COMM_20_CONTROL            COMM_CNTRL
#define KDS_20_CONTROL			   ADJ_CNTRL
#define DB_20_CONTROL			   PM_CNTRL
/*                 
*------------------------------------------------------------------------------
*   2X20 DISPLAY'S DESCRIPTOR CURRENT STATUS INFOMATION
*
*  See function DispRegOpDscrCntr() which maps these defines to a specific
*  descriptor indicator in the lead thru area. Comments for the following provide
*  a clue as to which bits of global flDispRegDescrControl are mapped to specific
*  descriptor indicators in the user interface.
*
*  See also function DispRegSetOpKeep() which uses these defines to map the
*  bit mask used with flDispRegKeepControl copy of flDispRegDescrControl to
*  set the global ulDispRegOpBackStat using these defines to keep a record
*  of indicator settings so that function DispRegOpDscrCntr() can detect what
*  indicators have changed.
*------------------------------------------------------------------------------
*/
#define RECEIPT_20_STATUS              0x0001   // indicates flDispRegDescrControl & RECEIPT_CNTRL, used to set descriptor _2X20_RECEIPT
#define PRT_OFFLINE_20_STATUS          0x0002   // indicates flDispRegDescrControl & PRT_OFFLINE_CNTRL, used to set descriptor _2X20_PRT_OFFLINE
#define DELETE_20_STATUS               0x0004   // indicates flDispRegDescrControl & (VOID_CNTRL | EC_CNTRL), used to set descriptor _2X20_VOID_DELETE
#define T_VOID_20_STATUS               0x0008   // indicates flDispRegDescrControl & TVOID_CNTRL, used to set descriptor _2X20_T_VOID
#define MINUS_20_STATUS                0x0010   // indicates flDispRegDescrControl & (ITEMDISC_CNTRL | REGDISC_CNTRL | CHRGTIP_CNTRL), used to set descriptor _2X20_MINUS
#define CRED_ALPHA_20_STATUS           0x0020   // indicates flDispRegDescrControl & CRED_CNTRL, used to set descriptor _2X20_CRED_ALPHA
#define TXBLE_TRANS_20_STATUS          0x0040   // indicates flDispRegDescrControl & (TAXMOD_CNTRL | TAXEXMPT_CNTRL | TRANSFR_CNTRL), used to set descriptor _2X20_TXBLE_TRANS
#define TOTAL_B_DUE_20_STATUS          0x0080   // See define TOTAL_B_DUE_20_CONTROL above for list of flDispRegDescrControl bits that map to this.
#define CHANGE_20_STATUS               0x0100   // indicates flDispRegDescrControl & CHANGE_CNTRL, used to set descriptor _2X20_CHANGE
#define NEW_STORE_20_STATUS            0x0200   // indicates flDispRegDescrControl & (NEWCHK_CNTRL | STORE_CNTRL), used to set descriptor _2X20_NEW_STORE
#define P_BALANCE_20_STATUS            0x0400  
#define PAYMENT_RECALL_20_STATUS       0x0800
#define TRAIN_20_STATUS                0x1000   // indicates flDispRegDescrControl & TRAIN_CNTRL, used to set descriptor _2X20_TRAIN
#define COMM_20_STATUS                 0x2000  
#define KDS_20_STATUS				   0x4000   // indicates flDispRegDescrControl & ADJ_CNTRL, used to set descriptor _2x20_KDS
#define DB_20_STATUS				   0x8000	/*Delayed Balance JHHJ, indicates flDispRegKeepControl & PM_CNTRL */
/*                 
*------------------------------------------------------------------------------
*   DEFINE FOR TAX TABLE DISPLAY 
*------------------------------------------------------------------------------
*/

#define DISPSUP_TAXTBL_TYPE1    1    /* display type #1 */
#define DISPSUP_TAXTBL_TYPE2    2    /* display type #2 */
#define DISPSUP_TAXTBL_TYPE3    3    /* display type #3 */
#define DISPSUP_TAXTBL_TYPE4    4    /* display type #4 */

/*                 
*------------------------------------------------------------------------------
*   10N10D Customer Display's Descriptor Name Define Declarations
*------------------------------------------------------------------------------
*/

#define _10CU_TOTAL            0   
#define _10CU_CHANGE           5   
#define _10CU_MINUS            9      /* (-) Descriptor */   

/*                 
*------------------------------------------------------------------------------
*   2x20 Customer Display's Descriptor Name Define Declarations
*------------------------------------------------------------------------------
*/

#define _20CU_MINUS            _2X20_MINUS      /* (-) Descriptor */   
#define _20CU_TOTAL            _2X20_TOTAL_B_DUE   
#define _20CU_CHANGE           _2X20_CHANGE   


/*
*------------------------------------------------------------------------------
*   MASKING DATA FOR 10N10D Customer Display's Descriptor Control
*------------------------------------------------------------------------------
*/
                                
#define TOTAL_10CU_CONTROL         (SUBTTL_CNTRL | TOTAL_CNTRL | TENDER_CNTRL | PTEND_CNTRL \
                                    | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL | DECTIP_CNTRL)     
#define CHANGE_10CU_CONTROL        CHANGE_CNTRL   
#define MINUS_10CU_CONTROL         (VOID_CNTRL | EC_CNTRL | TVOID_CNTRL | ITEMDISC_CNTRL \
                                    | REGDISC_CNTRL | CHRGTIP_CNTRL | CRED_CNTRL)  

/*                 
*------------------------------------------------------------------------------
*   2X20 DISPLAY'S DESCRIPTOR CURRENT STATUS INFOMATION
*------------------------------------------------------------------------------
*/

#define TOTAL_10CU_STATUS              0x0001
#define CHANGE_10CU_STATUS             0x0002
#define MINUS_10CU_STATUS              0x0004  


/*                 
*------------------------------------------------------------------------------
*   QUANTITY DATA TYPE DEFINITION
*------------------------------------------------------------------------------
*/

#define NO_DEC_POINT                0
#define DEC_POINT                   1

/*                 
*------------------------------------------------------------------------------
*   SCALE DATA TYPE DEFINITION
*------------------------------------------------------------------------------
*/

#define _2_DIGIT                    0
#define _3_DIGIT                    1

/*                 
*------------------------------------------------------------------------------
*   SALES DISPLAY DATA TYPE DEFINITION
*------------------------------------------------------------------------------
*/

#define NO_R_Q                      0
#define REPEAT                      1
#define QTY                         2
#define WEIGHT                      3
#define NO_R_Q_STRING               4

#define NO_ADJ                      0
#define ADJ                         1

/*                 
*------------------------------------------------------------------------------
*   DESCRIPTOR SAVE TYPE DEFINITION
*------------------------------------------------------------------------------
*/

#define DSC_SAV                     1

/*                 
*------------------------------------------------------------------------------
*   CUSTOMER DISPLAY CONTROL DEFINITION
*------------------------------------------------------------------------------
*/

#define C_DISP                      0x01    /* Need customer display */
#define C_DISP_MASK					0x0F
#define C_DISP_INDEX(x)	((x) >> 4)			// Shift count to get mnemonic index.	

// The following defines are used to index into the table "pMnemonicOverride"
// which is located in file dispfmt20.c. Any additional items added must also
// be added to that table.
//
// NOTE: See macro C_DISP_INDEX() above which is used to create the index into
//       the table of mnemonics. Leading digit of the following constants is used
//       to index into the table so a value of 0x10 is an index value of 0x1 or
//       the second item in the table of mnemonics. Digit in the least significant
//       nibble is a set of flags such as C_DISP above.
#define C_DISP_MNE_ITEM				0x10
#define C_DISP_MNE_COUPON			0x20  // Indicates override mnemonic with "COUPON".
#define C_DISP_MNE_DISCOUNT			0x30  // Indicates override mnemonic with "DISCOUNT".
#define C_DISP_MNE_TOTAL			0x40  // Indicates override mnemonic with "TOTAL".
#define C_DISP_MNE_TENDER			0x50  // Indicates override mnemonic with "TENDER".
#define C_DISP_MNE_BLANK			0x60  // Indicates override mnemonic with "BLANK".
#define C_DISP_MNE_CHANGE			0x70  // Indicates override mnemonic with "CHANGE".

/*                 
*------------------------------------------------------------------------------
*   COMPILE OPTION FOR DEBUG
*------------------------------------------------------------------------------
*/
#define DISP_REG_DEBUG               -1     /* compile option for debug */

/*                 
*------------------------------------------------------------------------------
*   SHIFT PAGE DATA TYPE DEFINITION
*------------------------------------------------------------------------------
*/
#define DISPPAGEASCII               0x30
#define DISPPAGESPACE               0x20


/*------------------------------------------------------------------------*\

  I N T E R F A C E   D A T A    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
        ULONG   ulQTY;
        UCHAR   uchLength;
        UCHAR   uchDecimal;
        USHORT  usQtyType;
}REGDISPQTY;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*------------------------------------------------------------------------*\

        E X T E R N  V A L I A B L E S    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

extern  MAINTWORK   DispSupSaveData;        /* saved data structure for re-display */
extern  UCHAR       uchMaintMenuPage;       /* Menu Shift Page */
                                            

/*------------------------------------------------------------------------*\

            P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/*
*------------------------------------------------------------------------------
*   SUPER/PROGRAM MODE FUNCTIONS DECLARATION PROTOTYPES
*------------------------------------------------------------------------------
*/
                                                    /* Source Name  */
VOID DispSupMnemo20( VOID *pData );                 /*  dspmne10.c  */    
VOID DispSupComData20( VOID *pData );               /*  dspcom10.c  */    
VOID DispSupBiData20( VOID *pData );                /*  dspbi10.c   */    
VOID DispSupDEPTPLU20( VOID *pData );               /*  dspplu10.c  */    
VOID DispSupControl20( MAINTDSPCTL *pData );        /*  dspctl10.c  */
VOID DispSupPLUNoMenu20( PARAPLUNOMENU *pData);     /*  dsppmu10.c  */  
VOID DispSupRound20( PARAROUNDTBL *pData );         /*  dsprnd10.c  */    
VOID DispSupFSC20( PARAFSC *pData );                /*  dspfsc10.c  */    
VOID DispSupHourlyTime20( PARAHOURLYTIME *pData );  /*  dsphur10.c  */
VOID DispSupFlexMem20( PARAFLEXMEM *pData );        /*  dspflx10.c  */    
VOID DispSupStoreNo20( PARASTOREGNO *pData );       /*  dspsto10.c  */    
VOID DispSupLevel20( PARASUPLEVEL *pData );         /*  dsplvl10.c  */    
VOID DispSupCurrency20( PARACURRENCYTBL *pData);    /*  dspfc10.c   */  
VOID DispSupTaxRate20( PARATAXRATETBL *pData );     /*  dsptxr10.c  */   
VOID DispSupTtlKeyCtl20( PARATTLKEYCTL *pData);     /*  dsptcl10.c  */  
VOID DispSupCashierNo20( MAINTCASHIERIF *pData);     /*  dspcas10.c  */  
VOID DispSupTaxTbl20( MAINTTAXTBL *pData );         /*  dsptxt10.c  */    
VOID DispSupMenuPLU20( PARAMENUPLUTBL *pData);      /*  dspmep10.c  */  
VOID DispSupFLine20( MAINTDSPFLINE *pData );        /*  dspfl20.c   */
VOID DispSupRedisp20( MAINTWORK *pData );           /*  dspred20.c  */
VOID DispSupTare20( PARATARE *pData );              /*  dsptar20.c  */
VOID DispSupEmployeeNo20( PARAEMPLOYEENO *pData);    /*  dspetk20.c  */  
VOID DispSupPresetAmount20( PARAPRESETAMOUNT *pData );/* dsppamt2.c  */
VOID DispSupAutoCoupon20( PARAAUTOCPN *pData );
VOID DispSupPigRule20( PARAPIGRULE *pData );         /* dsppig20.c  */
VOID DispSupEtkFile20( MAINTETKFL *pData );          /* dspefl20.c  */
VOID DispSupOepTbl20( PARAOEPTBL *pData );           /* dspoep20.c */
VOID DispSupFxDrive20( PARAFXDRIVE *pData );         /* dspfxdr20.c */
VOID DispSupCstrTbl20( MAINTCSTR *pData);            /* dspcstr2.c */
VOID DispSupPPI20( MAINTPPI *pData);                  /* dspppi20.c */
VOID DispSupDispPara20( PARADISPPARA *pData );       /* dspdsp20.c  */
VOID DispSupDeptNoMenu20( PARADEPTNOMENU *pData);    /*  dsppmu20.c 2172 */  
VOID DispSupKdsIp20( PARAKDSIP *pData );             /* dspkds20.c 2172 */
VOID DispSupUnlockNo( PARAUNLOCKNO *pData );           /* dspkds20.c security for NHPOS */
VOID DispSupRest20( PARARESTRICTION *pData );        /* dsprst20.c 2172 */
VOID DispSupLoanPickup20(MAINTDSPLOANPICKUP *pData);/* Saratoga */
VOID DispSupMiscPara20(PARAMISCPARA *pData);        /* Saratoga */

/*
*------------------------------------------------------------------------------
*   REG MODE FUNCTIONS DECLARATION PROTOTYPES
*------------------------------------------------------------------------------
*/

VOID DispRegConvertMnemonic(REGDISPMSG *pData);
VOID DispReplaceMnemonic(TCHAR *pszString, USHORT usLength);

VOID DispRegOpen20(REGDISPMSG *pData);
VOID DispRegTransOpen20(REGDISPMSG *pData);
VOID DispRegSales20(REGDISPMSG *pData);
VOID DispRegScale20(REGDISPMSG *pData);
VOID DispRegDisc20(REGDISPMSG *pData);
VOID DispRegCoupon20(REGDISPMSG *pData);
VOID DispRegTotal20(REGDISPMSG *pData);
VOID DispRegTender20(REGDISPMSG *pData);
VOID DispRegMisc20(REGDISPMSG *pData);
VOID DispRegModifier20(REGDISPMSG *pData);
VOID DispRegOther20(REGDISPMSG *pData);
VOID DispRegClose20(REGDISPMSG *pData);
VOID DispRegModeIn20(REGDISPMSG *pData);
VOID DispRegDisp20(REGDISPMSG *pData);
VOID DispRegGetMnemo(REGDISPMSG *pData);
VOID DispRegCompulsory20(REGDISPMSG *pData);
VOID DispRegRewrite(REGDISPMSG *pData);   

// The following functions display a mnemonic along with other data.
// The argument fusC_Disp is used to modify the behavior of the function
// to also display to the Customer Display, typically a 2x20 LCD, and to
// specify a hard coded mnemonic if MDC 28, MDC_PRINT_ITEM_MNEMO Bit B is set.
// Search for C_DISP to find instances of this usage and to locate the list
// of defined constants for the hardcoded table, pMnemonicOverride[], used.
VOID DispRegZeroMnemo( REGDISPMSG *pData, USHORT fusC_Disp);
VOID DispRegAmtMnemo(REGDISPMSG *pData, USHORT fusC_Disp);         // fusC_Disp indicates if C_DISP and hardcoded mnemonic
VOID DispRegRepeatAmtMnemo(REGDISPMSG *pData, USHORT fusC_Disp); 
VOID DispRegAmtString(REGDISPMSG *pData, USHORT fusC_Disp); 
VOID DispRegSales(REGDISPMSG *pData, USHORT fusC_Disp);    
VOID DispRegScale(REGDISPMSG *pData, UCHAR uchType, USHORT fusC_Disp);
VOID DispRegOrderEntry(REGDISPMSG *pData);    
VOID DispRegForeign(REGDISPMSG *pData, USHORT fusC_Disp);  
VOID DispRegOrderNo(REGDISPMSG *pData); 
VOID DispRegLowMnemo(REGDISPMSG *pData);  
VOID DispRegNumber(REGDISPMSG *pData);   
VOID DispRegQuantity(REGDISPMSG *pData); 
VOID DispRegLock(REGDISPMSG *pData, USHORT fusC_Disp);     
VOID DispRegModeIn(REGDISPMSG *pData, USHORT fusC_Disp);   
VOID DispRegSave(REGDISPMSG *pData);     
USHORT DispRegUpFmt(REGDISPMSG *pData1, REGDISPQTY *pData2);   
USHORT DispRegLowFmt(REGDISPMSG *pData);   
VOID DispRegGetQty(REGDISPQTY *pData);   
VOID DispRegMakeAmt(REGDISPMSG *pData);   
VOID DispRegEventTrigger(REGDISPMSG *pData);

VOID DispRegOpDscrCntr(REGDISPMSG *pData, UCHAR uchDscSave); /* Register Mode control descriptor (ope. display) */
VOID DispRegCuDscrCntr(REGDISPMSG *pData);       /* Register Mode control descriptor (cust. display) */
VOID DispRegOpEchoBack(REGDISPMSG *pData);       /* make display format for echo back */
VOID DispSupOpDscrCntr(VOID);                    /* Supervisor Mode control descriptor (ope. display) */

UCHAR DispSetPageChar(UCHAR uchArrow);
UCHAR DispSetSeatNo(UCHAR uchSeatNo);
VOID  DispRegMoney(REGDISPMSG *pData);
VOID  DispRegFoodStamp(REGDISPMSG *pData, USHORT fusC_Disp);


/***** End of File *****/