 /*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display module Include file used in register mode
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDRIN.H                     
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 : M.Ozawa    : Initial
*
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
* Dec-15-15 : 02.02.01 : R.Chambers : replace MLD_ total type defines with standard PRT_ defines.
* Dec-15-15 : 02.02.01 : R.Chambers : replace MLD_ decimal type defines with standard PRT_ defines.
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

/*
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
#define     MLD_PRECOLUMN1       MLD_NOR_DSP1_CLM_LEN   /* 2 scroll display column */
#define     MLD_PRECOLUMN2       MLD_NOR_DSP2_CLM_LEN   /* 2 scroll display column */
#define     MLD_DR2COLUMN        MLD_DRV2_DSP1_CLM_LEN  /* 2 scroll display column */
#define     MLD_DR3COLUMN        MLD_DRV3_DSP1_CLM_LEN  /* 3 scroll display column */

#define     MLD_AMOUNT_LEN      18      /* amount (15 DCURRENCY digits + decimal point + minus) */
#define     MLD_ITEMAMT_LEN      3      /* amount (26(column) - 20(PLU mnem.) - 3(qty), 2172) */
/* #define     MLD_ITEMAMT_LEN      5      / amount (3digits + decimal point + minus) */
#define     MLD_QTY_LEN          3      /* quantity (3ditits) */
#define     MLD_GC_LEN           4      /* G/C no. length */

#define		MLD_ATTRIB_STD_VIDEO  0x00    // ColorPalette Address to display in standard video for items, etc. - see also uchColorPaletteCode
#define		MLD_ATTRIB_REV_VIDEO  0x02    // ColorPalette Address to display in reverse video for voided items, etc. - see also uchColorPaletteCode

#if 0
// STD_ADJMNEMO_LEN + 1 + STD_PLUNAME_LEN + 1 + STD_MAX_COND_NUM * (STD_PLUNAME_LEN + 1)
// PRT_MAXITEM, MLD_MAXITEM, PRT_KPMAXITEM need to be maintained to the correct size
// #define     MLD_MAXITEM        446      /* max bytes of adjective, noun, print modifier */
                                        /* 4(adj) + 1(space) + 20(noun) + 1(space)      */
                                        /* + 20 * ( 20(condiment) + 1(space))           */
/* #define     MLD_MAXITEM        160      / max bytes of adjective, noun, print modifier */
                                        /* 4(adj) + 1(space) + 12(noun) + 1(space)      */
                                        /* + 10 * ( 12(condiment) + 1(space))           */
#endif

#define     MLD_NOT_DISPLAY_PLU_NO  9950

#define     MLD_DEFAULT_REVERSE 0x00000008L /* default reverse video position */
#define     MLD_PRECHK_AMT_REV  0x10000000L // reset reverse video at amount display US Customs SCER  cwunn
											// 2 increased to 8 to shade reverse video correctly on 29 digit wide display
#define     MLD_PRECHK_AMT_REV2 0x01000000L /* reset reverse video at amount display */

#define     MLD_DRIVE_DEPT_PLU_LEN  9   /* length of plu/dept at drive through */

/*
============================================================================
+                     T Y P E    D E F I N I T I O N S
============================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
}MLDITEMDATA;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/

//Now using MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH - (ecr.h)
//extern CONST TCHAR auchMldNoDispPLULow[]; /* 2172 */
//extern CONST TCHAR auchMldNoDispPLUHigh[]; /* 2172 */

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
/** --------------------------------- **\
     R E G I S T E R   M O D E
\** --------------------------------- **/

/* -- MLDITEM.c -- */
SHORT MldLocalScrollWrite(VOID *pItem, USHORT usType, USHORT usScroll);
UCHAR  MldSetScrollSymbol(USHORT usType);

/* -- MLDSALE.c -- */
SHORT  MldItemSales(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldOrderDeclaration(ITEMSALES *pItem, USHORT usType, USHORT usScroll);

/* -- MLDDISC.c -- */
SHORT  MldItemDisc(ITEMDISC  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMISC.c -- */
SHORT   MldItemMisc(ITEMMISC *pItem, USHORT usType, USHORT usScroll);
SHORT   MldMoney(ITEMMISC *pItem, USHORT usType, USHORT usScroll);              /* Saratoga */
SHORT   MldMoneyTender(ITEMMISC *pItem, USHORT usType, USHORT usScroll);        /* Saratoga */
SHORT   MldPrechkMoney(ITEMMISC *pItem, USHORT usType, USHORT usScroll);        /* Saratoga */
SHORT   MldDriveMoney(ITEMMISC *pItem, USHORT usType, USHORT usScroll);         /* Saratoga */
SHORT   MldMoneyForeign(ITEMMISC *pItem, USHORT usType, USHORT usScroll);       /* Saratoga */
SHORT   MldPrechkMoneyForeign(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);/* Saratoga */
SHORT   MldDriveMoneyForeign(ITEMMISC *pItem, USHORT usType, USHORT usScroll);  /* Saratoga */

/* -- MLDTEND.c -- */
SHORT  MldItemTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll);

/* -- MLDTAX.c -- */
SHORT  MldItemAffection(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);

/* -- PrROpen_.c -- */
SHORT  PrtItemTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);
SHORT  PrtDispTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);

/* -- MLDTTL.c -- */
SHORT  MldItemTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMUL.c -- */
SHORT  MldItemMulti(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);

/* -- MLDIMOD.c -- */
SHORT  MldItemModifier( ITEMMODIFIER *pItem , USHORT usType, USHORT usScroll);
SHORT  MldPrechkModifier( ITEMMODIFIER *pItem , USHORT usType, USHORT usScroll);

/* -- MLDDEPT.c -- */
SHORT  MldDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 
SHORT  MldPrechkDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 
SHORT  MldDriveDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 
VOID   MldSetNoun(TCHAR *pszMnemonic, TCHAR *pszAdjMnem, ITEMSALES *pItem);
VOID   MldSetCondiment(TCHAR *pszMnemonic, ITEMSALES *pItem, UCHAR uchCond);

/* -- MLDSET.c -- */
SHORT  MldSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 
SHORT  MldPrechkSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 
SHORT  MldDriveSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll); 

/* -- MLDMODD.c -- */
SHORT  MldModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll);

/* -- MLDIRDI.c -- */
SHORT  MldItemRegDisc(ITEMDISC *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkItemRegDisc(ITEMDISC *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveItemRegDisc(ITEMDISC *pItem, USHORT usType, USHORT usScroll);

/* -- MLDTIP.c -- */
SHORT  MldChrgTip(ITEMDISC  *pItem, USHORT usType, USHORT usScroll);    

/* -- MLDCPN.c -- */
SHORT  MldItemCoupon(ITEMCOUPON *pItem, USHORT usType, USHORT usScroll);
SHORT  MldCoupon(ITEMCOUPON  *pItem, USHORT usType, USHORT usScroll);    
SHORT  MldPrechkCoupon(ITEMCOUPON  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveCoupon(ITEMCOUPON  *pItem, USHORT usType, USHORT usScroll);    

/* -- MLDCNCL.c -- */
SHORT  MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll); 
SHORT  MldDriveCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDNTND.c -- */
SHORT  MldTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll);

/* -- MLDFTND.c -- */
VOID   MldGetMoneyMnem(UCHAR uchTendMinor, USHORT *pusTran, UCHAR *puchSym);
SHORT  MldForeignTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkForeignTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveForeignTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll);

/* -- MLDTAX.c -- */
SHORT  MldTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDUTAX.c -- */
SHORT  MldUSTax(ITEMAFFECTION   *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkUSTax(ITEMAFFECTION   *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveUSTax(ITEMAFFECTION   *pItem, USHORT usType, USHORT usScroll);

/* -- MLDCTAX.c -- */
SHORT  MldCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);

/* -- MLDADD2.c --*/
SHORT  MldAddChk2(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkAddChk2(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDOOPN.c -- */
SHORT  MldItemOpeOpen(ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDSIGN.c -- */
SHORT  MldCashierSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCashierSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldWaiterSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkWaiterSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDOPEN.c -- */
SHORT  MldItemTransOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldMiscOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkMiscOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDCASI.c -- */
SHORT  MldCasIn(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkCasIn(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldStoreRecall(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveStoreRecall(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDNCHK.c -- */
SHORT  MldNewChk(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);    
SHORT  MldPrechkNewChk( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);
VOID  MldWriteGuestNo( ITEMTRANSOPEN *pData);
SHORT  MldSplitNewChk( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);

/* -- MLDREOD.c -- */
SHORT  MldReorder(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkReorder(ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);
SHORT  MldPrechkReorder2(ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);

/* -- MLDADD1.c -- */
SHORT  MldAddChk1( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);
SHORT  MldPrechkAddChk1( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll);

/* -- MLDTOTL.c -- */
SHORT  MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);

/* -- MLDSEVT.c -- */
SHORT  MldServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldSoftCheckTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkSoftChkTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
VOID   MldServMaskAcctDate(TCHAR *pAcctNo, ITEMTOTAL *pItem);

/* -- MLDTRAY.c -- */
SHORT  MldTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
SHORT  MldDriveTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMULC.c -- */
SHORT  MldMulti(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkMulti(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMULP.c -- */
SHORT  MldMultiPost(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkMultiPost(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMLTL.c -- */
SHORT  MldMultiTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkMultiTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);

/* -- MLDMLTP.c -- */
SHORT  MldMultiPostTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
SHORT  MldPrechkMultiPostTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);

/* -- MLDHEAD -- */
SHORT  MldItemPrint(CONST ITEMPRINT *pItem, USHORT usType, USHORT usScroll);

/* -- MLDVAT -- */
SHORT MldVATSubtotal(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT MldPrechkVATSubtotal(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT MldDriveVATSubtotal(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT MldPrechkVATServ(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT MldDriveVATServ(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT MldVAT(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
SHORT MldPrechkVAT(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);
SHORT MldDriveVAT(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll);

/* -- MLDRGCOM.c -- */
USHORT  MldVoidOffline(TCHAR *pszWork, USHORT fbMod, USHORT usDispType);
USHORT  MldNumber(TCHAR *pszWork, TCHAR *pszNumber, USHORT usDispType);
USHORT  MldMnemNumber(TCHAR *pszWork, USHORT usAdr, TCHAR *pszNumber, USHORT usDispType);
USHORT  MldMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType);
USHORT  MldFSMnemAmt(TCHAR *pszWork, USHORT usAdr, LONG lFS, DCURRENCY lAmount, USHORT usDispType);
USHORT  MldDiscount(TCHAR *pszWork, UCHAR uchSeatNo, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, USHORT usDispType);
USHORT  MldCpn( TCHAR *pszDest, UCHAR uchSeatNo, TCHAR *pszMnemonic, DCURRENCY lAmount, USHORT usDispType);
USHORT  MldTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod, USHORT usDispType);
USHORT  MldWeight(TCHAR *pszWork, ULONG flScale, ITEMSALES *pItem, USHORT usDispType);
USHORT  MldMnlWeight(TCHAR *pszWork, USHORT usDispType);
USHORT  MldTranMnem(TCHAR *pszWork, USHORT usAdr, USHORT usDispType);
USHORT  MldSeatNo(TCHAR *pszWork, UCHAR uchSeatNo, USHORT usDispType);
USHORT  MldChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem, USHORT usDispType);
USHORT  MldPrechkChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  MldDriveChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  MldGstChkNo(TCHAR *pszWork, ITEMMULTI *pItem, USHORT usDispType);
USHORT  MldForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2, USHORT usType);
USHORT  MldDoubleMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType);
USHORT  MldChargeTips(TCHAR *pszWork, ULONG ulWaiID, USHORT usDispType);
USHORT  MldTrayCount(TCHAR *pszWork, USHORT usAdr, SHORT sTrayCo, USHORT usDispType);
USHORT  MldItems(TCHAR  *pszWork, ITEMSALES  *pItem, USHORT usPrice, LONG *lAttrib, USHORT usDisptype, USHORT usType);

USHORT  MldCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID, USHORT usDispType);
USHORT  MldCPRspMsgText(TCHAR *pszWork, TCHAR *pszRspMsgText, USHORT usDispType);
USHORT MldCPDriveRspMsgText(TCHAR *pszWork, TCHAR pszRspMsgText[], USHORT usDispType);
USHORT  MldOffline(TCHAR *pszWork, TCHAR *aszDate, TCHAR *aszApproval, USHORT usDispType);
USHORT  MldFolioPost(TCHAR *pszWork, UCHAR *pszFolioNo, UCHAR *pszPostTranNo, USHORT usDispType);
SHORT   MldDummyScrollDisplay(USHORT usType, USHORT usScroll);
SHORT MldCheckReverseVideo(VOID);
USHORT  MldMnemAmtShift(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType);

VOID  MldSetItem(TCHAR  *pDest, ITEMSALES  *pItem, DCURRENCY *plProduct, USHORT usAmtLen, LONG *lAttrib, USHORT usDispType);//US Customs added usDispType
SHORT MldGet1Line(TCHAR *pStart, TCHAR **pEnd, USHORT usColumn);
SHORT  MldChkVoidConsolidation(ITEMSALES *pItem);
USHORT  MldLinkPlu(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, LONG lQTY, DCURRENCY lPrice, USHORT usDispType);

 USHORT	MldGiftBalance(ITEMTENDER *pItem, USHORT usType,USHORT usScroll);
 USHORT	MldGiftVoidIssue(ITEMTENDER *pItem, USHORT usType,USHORT usScroll);
 USHORT  MldGiftReload(ITEMTENDER *pItem, USHORT usType,USHORT usScroll);
 USHORT  MldGiftVoidReload(ITEMTENDER *pItem, USHORT usType,USHORT usScroll);
 USHORT	MldGiftIssue(ITEMTENDER *pItem, USHORT usType,USHORT usScroll);
//SR 143 @/For Key cwunn
USHORT MldAtFor(TCHAR  *pszWork, ITEMSALES  *pItem, USHORT usPrice, LONG *plAttrib, USHORT usDispType, USHORT usType);
VOID MldSetAtFor(TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,	USHORT usAmtLen, LONG *plAttrib, USHORT usDispType);


/*** End of File ***/
