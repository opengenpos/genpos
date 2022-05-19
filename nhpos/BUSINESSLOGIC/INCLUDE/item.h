/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Item Module Headrer File                         
* Category    : Register Mode Item Module, NCR 2170 US Application         
* Program Name: ITEM.H
* --------------------------------------------------------------------------
* Abstract:   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-04-92: K.Maeda   : initial                                   
* Jul-20-93: K.You     : add US enhance                                   
* Feb-23-95: hkato     : R3.0
* Nov-09-95: hkato     : R3.1
* Nov-25-99: hrkato    : Saratoga
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
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

#if !defined(ITEM_H_INCLUDED)

#define ITEM_H_INCLUDED

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

extern DCURRENCY       couponAmount;

/*
* --------------------------------------------------------------------------
    Return Code From Sub Modules
* --------------------------------------------------------------------------
*/
/*
*===========================================================================
    Structure Type Define Declarations
*===========================================================================
*/

// Bit map defines for consolidating various often checked PLU characteristics or options.
// These bit map defines create patterns that can be used to simplify decision making code.
// See function ItemSalesItemPluTypeInfo() for how these are used.
#define PLU_BM_MINUS_PLU       0x00000001    // indicates ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS
#define PLU_BM_HASH_PLU        0x00000002    // indicates ParaPlu.ContPlu.auchContOther[2] & PLU_HASH
#define PLU_BM_SCALE_PLU       0x00000004    // indicates ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE
#define PLU_BM_CONDI_PLU       0x00000008    // indicates ParaPlu.ContPlu.auchContOther[2] & PLU_CONDIMENT
#define PLU_BM_USE_DEPT        0x00000010    // indicates ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL
#define PLU_BM_AFECT_DISC1     0x00000020    // indicates ParaPlu.ContPlu.auchContOther[1] & PLU_AFFECT_DISC1
#define PLU_BM_AFECT_DISC2     0x00000040    // indicates ParaPlu.ContPlu.auchContOther[1] & PLU_AFFECT_DISC2
#define PLU_BM_PRT_VAL         0x00000080    // indicates ParaPlu.ContPlu.auchContOther[0] & PLU_PRT_VAL
#define PLU_BM_ADJ_PROHIBIT_1  0x00000100    // indicates ParaPlu.ContPlu.auchContOther[4] & PLU_PROHIBIT_VAL1
#define PLU_BM_ADJ_PROHIBIT_2  0x00000200    // indicates ParaPlu.ContPlu.auchContOther[4] & PLU_PROHIBIT_VAL2
#define PLU_BM_ADJ_PROHIBIT_3  0x00000400    // indicates ParaPlu.ContPlu.auchContOther[4] & PLU_PROHIBIT_VAL3
#define PLU_BM_ADJ_PROHIBIT_4  0x00000800    // indicates ParaPlu.ContPlu.auchContOther[4] & PLU_PROHIBIT_VAL4
#define PLU_BM_ADJ_PROHIBIT_5  0x00001000    // indicates ParaPlu.ContPlu.auchContOther[4] & PLU_PROHIBIT_VAL5

/*
*===========================================================================
    Function Prototype Declarations
*===========================================================================
*/
/*
* --------------------------------------------------------------------------
    User Interface Functions Declarations
* --------------------------------------------------------------------------
*/

SHORT   ItemSignIn(UIFREGOPEOPEN *pData);
SHORT   ItemTransOpen(UIFREGTRANSOPEN *pData);
SHORT   ItemSalesEditCondiment(UCHAR uchType);
SHORT   ItemSales(UIFREGSALES *pData);
SHORT   ItemDiscount(UIFREGDISC *pData);
SHORT   ItemCoupon(UIFREGCOUPON *pData);
VOID    ItemCouponVoidDisp(ITEMSALES *pItemSales, ITEMCOUPON *pItemCoupon);
UCHAR   ItemTotalTypeIndex (UCHAR uchMinorClass);
SHORT   ItemTotalCashierCheckLoanTotalZero (ULONG  ulEmployeeId);
SHORT   ItemTotalAuchTotalStatus (UCHAR auchTotalStatus[NUM_TOTAL_STATUS], CONST UCHAR uchMinorClass);
SHORT   ItemTotal(UIFREGTOTAL *pData);
SHORT   ItemTenderEntry(UIFREGTENDER *pData);
SHORT   ItemModifier(UIFREGMODIFIER *pData);
SHORT   ItemSignOutTest(VOID);
SHORT   ItemSignOutWithTest(UIFREGOPECLOSE *pData);
SHORT   ItemSignOut(UIFREGOPECLOSE *pData);
SHORT   ItemModeIn(UIFREGMODEIN *pData);
SHORT   ItemMiscETK(UIFREGMISC *UifRegMisc);
SHORT   ItemMiscETKSignIn(UIFREGMISC *UifRegMisc);       //PDINU
SHORT   ItemMiscPostReceipt(UIFREGMISC *pUifRegMisc);
VOID    ItemMiscCreatePostFile( VOID );
SHORT   ItemOtherEJReverse( VOID );
SHORT   ItemMiscParking(UIFREGMISC *pUifRegMisc);
SHORT   ItemMiscMoney(UIFREGMISC *pUifRegMisc);
SHORT   ItemMiscMoneyIn(VOID);
SHORT   ItemMiscMoneyOut(VOID);
SHORT   ItemModFS(CONST UIFREGMODIFIER *ItemModifier);
VOID    ItemMiscAgeCancel(VOID);
SHORT	ItemSalesOrderDeclaration(UIFREGSALES pUifRegSales, UCHAR uchOrderDecNum,USHORT usType);
SHORT	ItemSalesAdjModify( UIFREGSALES *pUifRegSales );
SHORT   ItemOtherDemand( VOID );
SHORT   ItemOtherSlipRel( VOID );
SHORT   ItemOtherSlipFeed(UIFREGOTHER *pData );
VOID    ItemOtherRecFeed( VOID );
VOID    ItemOtherJouFeed( VOID );
SHORT   ItemOtherECor( VOID );
VOID    ItemOtherClear(VOID);
SHORT   ItemOtherComputat( UIFREGOTHER *pData );
SHORT   ItemOtherOrder(VOID);
SHORT ItemOtherWait(VOID);
SHORT ItemOtherPaidRecall(UIFREGOTHER *pData);
VOID  ItemOtherScrollUp(VOID);
VOID  ItemOtherScrollDown(VOID);
SHORT ItemOtherRightArrow(VOID);
SHORT ItemOtherLeftArrow(VOID);
VOID  ItemOtherLeftDisplay(VOID);
SHORT ItemOtherCursorVoid(UIFREGOTHER *pData);
SHORT   ItemOtherCurVoidRedisplay( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal ); //US Customs 5/6/03 cwunn
SHORT ItemOtherTransfer(UIFREGOTHER *pData);
SHORT ItemOtherTransfItem(UIFREGOTHER *pData);

SHORT   ItemSalesDiscountClassIsMod (UCHAR uchMinorClass);
SHORT   ItemSalesDiscountClassIsItem (UCHAR uchMinorClass);
USHORT  ItemClassIsOneOf (UCHAR uchMajorMinorClass, ...);  // last arg must be zero.
ULONG   ItemSalesItemPluTypeInfo (UCHAR *auchContOther);
ULONG   ItemSalesItemPluTypeInfoSelected (UCHAR *auchContOther, ULONG ulMaskSelection);
BOOL    ItemSalesItemPluTypeInfoBoolRequired (UCHAR *auchContOther, ULONG ulMaskSetRequiredOn, ULONG ulMaskSetRequiredOff);
SHORT   ItemSalesBalance(SHORT GiftCard);

BOOL   ItemSalesLocalSaleStarted(VOID);
SHORT  CheckAllowFunction();

SHORT  ItemCheckNoPurchase(UIFREGTENDER *pUifRegTender);
VOID   ItemSignInClearDisplay(USHORT usType);

//SR 143 @/For key cwunn
//Queries previous item buffer and retrieves @/for data if neccessary
VOID ItemPreviousGetForKeyData(UIFREGSALES *pCurrentItem);
VOID ItemPreviousGetForKeyDataItemSales(ITEMSALES *pCurrentItem);
VOID ItemPreviousSaveForKeyData(UIFREGSALES *pCurrentItem);
VOID ItemPreviousClearForKeyData(VOID);

VOID  GiftBalMaskAcct(ITEMTENDER *pItemTender);

SHORT ItemOtherCheckDispPara(VOID *pData, USHORT *pusPLUNo, UCHAR *puchAdjective); /* R3.1 */
VOID  ItemOtherDifEnableStatus(USHORT usType);   /* R3.1 */
SHORT ItemOtherDifDisableStatus(VOID);  /* R3.1 */
VOID  ItemOtherDifRewriteRing(VOID);    /* R3.1 */
VOID  ItemOtherDifClearBuffer(VOID);    /* R3.1 */
VOID  ItemOtherDiaRemoveKey(VOID);      /* V3.3 */
ULONG   ItemOtherDiaInsertKey(VOID);      /* V3.3 */
VOID	ItemOtherResetLogTrnItem (UCHAR uchAction, LONG lAmount);
SHORT   ItemOtherResetLog(UCHAR uchAction); /* V3.3 */
SHORT   ItemOtherPowerDownLog(UCHAR uchLog);        /* Saratoga */
SHORT   ItemOtherLabelProc(UIFREGOTHER *pRegOther); /* 2172 */
SHORT   ItemOtherAsk(UIFREGOTHER *pItem);           /* Saratoga */

UCHAR   ItemMiscForeignTenderMdc (UCHAR uchTendMinor);
SHORT   ItemMiscNosale(UIFREGMISC *pData);
SHORT   ItemMiscPO(UIFREGMISC *pData, USHORT  usClientDisplay);
SHORT   ItemMiscRA(UIFREGMISC *pData, USHORT  usClientDisplay);
SHORT   ItemMiscTipsPO(UIFREGMISC *pData);
SHORT   ItemMiscTipsDec(UIFREGMISC *pData);
SHORT   ItemMiscCheckTRIn(VOID);
SHORT   ItemMiscCheckTrans(UIFREGMISC *pData);
SHORT   ItemMiscCheckTROut(VOID);
SHORT   ItemMiscCancel(UIFREGMISC *pData);
SHORT   ItemMiscETK(UIFREGMISC *pData);

SHORT   ItemModPvoid(UCHAR uchMinor);
SHORT   ItemModRec(VOID);
SHORT   ItemModTable(UIFREGMODIFIER *pData);
SHORT   ItemModPerson(UIFREGMODIFIER *pData);
SHORT   ItemModTax(UIFREGMODIFIER *pData);
SHORT   ItemModTare(UIFREGMODIFIER *pData);
SHORT   ItemModOffTend(VOID);
SHORT   ItemModAlphaName(VOID);
SHORT   ItemModSeatNo(UIFREGMODIFIER *pData);
SHORT   ItemModMenu(CONST UIFREGMODIFIER *pData);
SHORT   ItemModAdjMenu(CONST UIFREGMODIFIER *ItemModifier );  /* 2172 */
SHORT   ItemModNumber(UIFREGMODIFIER *ItemModifier );   /* 2172 */
SHORT   ItemModWIC(VOID);                               /* Saratoga */
SHORT   ItemModTent(CONST TCHAR *aszNameEntry, USHORT usNameEntryLen);
SHORT   ItemOtherValidation(VOID);		//SR 47 Manual Validation
VOID    ItemTendCPPrintErr( USHORT usFuncCode, SHORT sErrorCode );
VOID    ItemVoidGiftCardCancel(VOID);
SHORT   ItemModFor(CONST UIFREGSALES *pItemModifier, CONST UIFREGSALES *pRegData); //SR 143 cwunn @/For key

// slcalc.c functions in general use.
DCURRENCY  ItemSalesCalcDetermineBasePrice (CONST ITEMSALES  *pItem, LONG lQTY);
DCURRENCY  ItemSalesCalcDetermineTotalPrice (CONST ITEMSALES  *pItemSales, LONG lQTY);
LONG    ItemSalesCalcDetermineBaseQuantity (CONST ITEMSALES *pItemSales);
DCURRENCY  ItemSalesCalcCondimentPPIReplace(ITEMSALES *pItemSales);
DCURRENCY  ItemSalesCalcCondimentPPIReplaceQty(ITEMSALES *pItemSales, LONG lQty);
VOID    ItemSalesCalcInitCom(VOID);
SHORT   ItemSalesCalcECCom(ITEMSALES *pItemSales);
SHORT   ItemSalesSalesRestriction(ITEMSALES *pItemSales, UCHAR uchRestrict);
VOID	ItemSalesClosePpiPM(VOID);
SHORT   ItemSalesOnlyCalcPPI(ITEMSALES *pItemSales);    /* R3.1 */
SHORT   ItemSalesOnlyCalcPM(ITEMSALES *pItemSales);    /* 2172 */


SHORT   ItemLineLeadthruPromptEntry (USHORT usLeadthruNumber, TCHAR *tcsBuffer, int iBufLen);
SHORT   ItemLinePausePromptEntry (USHORT usPauseNumber, TCHAR *tcsBuffer, int iBufLen);

SHORT ItemAccountLookupDisplayOnly (SHORT GiftCard, SHORT sLookupType, TCHAR *aszNumber);
SHORT ItemAccountLookup(SHORT GiftCard, SHORT sLookupType);
USHORT  ItemTenderLocalCheckTenderid (USHORT usOpCode, USHORT usNewValue);

USHORT  ItemTenderCheckTenderMdc (UCHAR uchMinorClass, UCHAR uchPageAddress, UCHAR uchField);
SHORT   ItemTendGetTotal(UCHAR *auchTotalStatus, UCHAR uchMinor);
SHORT   ItemTendNoPerson(VOID);
SHORT   ItemTendGetOffset(VOID);
VOID    ItemTransDispGCFNo(USHORT  usTranAddr);

/** following functions are defined to share between tender and report */

#define     FC_EURO_TYPE1           1           /* MI x FC Rate */
#define     FC_EURO_TYPE2           2           /* MI / FC Rate */

SHORT   ItemTendFCRateCalc1(UCHAR uchStatus, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);
SHORT   ItemTendFCRateCalc2(UCHAR uchStatus, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);
VOID    ItemTendFCRateCalcByEuroB(DCURRENCY *plForeign, DCURRENCY lMI, ULONG ulCurrency, UCHAR uchType, USHORT usConvType, UCHAR uchStatus);

//US Customs moves ItemSalesCommonIF prototype to from itmlocal.h so it can be called from uifitem.c
SHORT   ItemSalesCommonIF(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemCommonSetupTransEnviron (VOID);

SHORT   ItemCheckCustomerSettingsFlags (ULONG ulFlagsToCheck);
SHORT   ItemCommonGetStatus( UCHAR uchName );
VOID    ItemCommonPutStatus( USHORT usName );
VOID    ItemCommonResetStatus(USHORT usName);

STANZA  *ItemSalesMakeStanza(STANZA *pStanza);
VOID    ItemCommonMldInit( VOID );
SHORT   ItemCommonGetSRAutoNo(USHORT *pusGuestNo);  /* R3.3 */
SHORT   ItemCommonTransOpen(USHORT usGuestNo);      /* R3.3 */

SHORT   ItemSPVInt( UCHAR uchLeadAddr );		// JMASON -- Function added to file to allow sup. int. to be called.
SHORT   ItemSPVIntDialog( UCHAR uchLeadAddr, UCHAR uchDialogLeadAddr );
SHORT   ItemSPVIntAlways( UCHAR uchLeadAddr );

SHORT   ItemDiscCalculation(ITEMDISC *pItemDisc, DCURRENCY *plItemizer);
SHORT   ItemDiscCheckSeq(UIFREGDISC *pUifRegDisc, ITEMDISC *pItemDisc, DCURRENCY *plItemizerBuff);
SHORT   ItemDiscHALOProc(DCURRENCY *plAmount, ITEMDISC *pItemDisc);
SHORT   ItemDiscGetItemizer(ITEMDISC *pItemDisc, DCURRENCY *plItemizer, DCURRENCY *plVoidDiscount);
SHORT   ItemDiscGetItemizerSeat(ITEMDISC *pDisc);

// slvoid.c functions
SHORT   ItemDisplayOepChooseDialog (CONST TCHAR *pLeadThruMnemonic, TCHAR **pMnemonicsList, SHORT sCouponCount);
SHORT   ItemSalesCursorVoid(ITEMSALES *pItemSales, SHORT sQTY);
SHORT   ItemSalesCursorVoidReturns(ITEMSALES *pItemSales, SHORT sQTY);

SHORT ItemTransGetReturnsGcf (GCNUM  usGCNumber, GCNUM usNewGCNumber);

USHORT  ItemDiscGetMdcCheck_ITEMDISC1 (UCHAR uchDiscountNo, USHORT usAddress);
USHORT  ItemDiscGetMnemonic_ITEMDISC1 (UCHAR uchDiscountNo);
USHORT  ItemDiscGetRate_ITEMDISC1 (UCHAR uchDiscountNo);

VOID    ItemTenderClearChargeAutoTips(VOID);

#endif
/****** End of Definition ******/