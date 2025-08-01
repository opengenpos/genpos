/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used in print module                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtIn.H                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 01.00.12 : R.itoh     : initial                                   
* Jun-30-93 : 01.00.12 : K.You      : add PRTCONTROL                                   
* Jul-28-93 : 01.00.12 : K.You      : add PrtSoftCheckTotal, PrtSoftCheckTotal_TH,
*           :          :            : PrtSoftCheckTotal_EJ, PrtSoftCheckTotal_SP
* Aug-23-93 : 01.00.13 : R.itoh     : delete PrtProm_SP(), add PrtSoftProm()
* Apr-24-94 : 00.00.05 : K.You      : add PRT_OFFCPAUTTEND, mod. PrtGetOffTend prtotype.
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: add PrtTotalAudact(), PrtTotalAudact_SPVL()
* May-16-94 : 02.05.00 : Yoshiko.Jim: add PrtServTotal_SPVL(), PrtSoftCheckTotal_SPVL()
* May-23-94 : 02.05.00 : K.You      : bug fixed Q-14 (chg. PRT_KPVOID  from 0x0020 to 0x0010)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PRT_CONSOL_CP_EPT)
* Jan-26-95 : 03.00.00 : M.Ozawa    : Recover prototypes for display on the fly
* Mar-06-95 : 03.00.00 : T.Nakahata : Add Coupon Featue and Unique Trans No.
* Apr-21-95 : 03.00.00 : T.Nakahata : Add Total Mnemonic to Kitchen Feature
* Jun-07-95 : 03.00.00 : T.Nakahata : Add Print Priority Feature
* Jul-24-95 : 03.00.04 : T.Nakahata : add Print customer name feature
* Jul-25-95 : 03.00.04 : T.Nakahata : print on journal by service total key
*
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
* Dec-16-99 : 01.00.00 : hrkato     : Saratoga
* Mar-15-00 :          : K.Iwata    :
* May-08-00 :          : M.Teraki   : fbPrtKPTarget removed.
* Jul-31-00 : V1.0.0.3 : M.Teraki   : Changed KP frame number to be managed for each KP
*
** OpenGENPOS **
*
* Jul-05-25 : 02.04.00 : R.Chambers : removed zombie function declarations. localized printer specific functions.
* Jul-05-23 : 02.04.00 : R.Chambers : using PARAMDC instead of PRTPARAMDC, ITEMCLASSHEADER instead of PRTITEMDATA.
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
#include "pmg.h"

/*
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
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
    USHORT  fsComp;
    USHORT  fsNo;
}PRTCONTROL;

// See function PrtPluTicketList() which uses these flags for formatting the print out.
#define PLUCHITINFO_FLAG_PRINT_TICKET     0x00000001     // Print the P57 address 23 and 24 ticket header info, requires usFamilyCode >= 990
#define PLUCHITINFO_FLAG_PRINT_PLUDOUBLE  0x00000002     // Print the PLU mnemonic in double high, double wide
#define PLUCHITINFO_FLAG_PRINT_PLUCOLUMN  0x00000004     // Print the PLU mnemonic and condiments as two columns

typedef struct {
    TCHAR     auchPLUNo[NUM_PLU_LEN+1];     /*  PLU No, 2172    */
	TCHAR     aszMnemonic[NUM_DEPTPLU];
	TCHAR     aszMnemonicAlt[NUM_DEPTPLU];
	TCHAR     aszAdjMnemonic[PARA_ADJMNEMO_LEN + 1];
	TCHAR     aszCondMnemonic[STD_MAX_COND_NUM][NUM_DEPTPLU];
	TCHAR     aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];
	SHORT     sQty;
	USHORT    usFamilyCode;
} PluChitInfo;

typedef struct {
	PluChitInfo  *pPluChitInfo;
	int          nPluChitInfoSize;
	int          iOrderDeclareAdr;
} PluChitInfoAll;


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
extern SHORT   fsPrtCompul;          /* compulsory portion */
extern SHORT   fsPrtNoPrtMask;       /* No-Print portion Mask */
extern SHORT   fsPrtStatus;          /* status ( request kitchen, no other, print on demand, 
                                          not initialize condition ) */
extern SHORT   fsPrtPrintPort;       /* print portion */
extern UCHAR   auchPrtFailKP[PRT_KPRT_MAX];  /* failed kitchen printer */
extern UCHAR   uchPrtCurKP;                  /* current failed kitchen printer */

extern TCHAR   uchPrtSeatNo;           /* save seat no for continual slip print R3.1 */
extern TCHAR   auchPrtSeatCtl[2];      /* save seat no data for split check print control */

/* -- for message (sent Kitchen printer) control -- */  
UCHAR  PrtKPSeatStateNotFeed(void);

extern USHORT  usPrtSlipPageLine;    /* current slip lines and pages */
extern USHORT  usPrtTHColumn;        /* thermal's numbers of columns */
extern USHORT  usPrtEJColumn;        /* journal's numbers of columns */
extern USHORT  usPrtVLColumn;        /* validation's numbers of columns */

extern PRTCONTROL PrtCompNo;         /* compulsory print & no print status save area */   


/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
/** --------------------------------- **\
     R E G I S T E R   M O D E
\** --------------------------------- **/

/* -- PrCTran_.c -- */
SHORT PrtChkStorage(PifFileHandle * hsRead1, PifFileHandle * hsRead2, CONST TRANINFORMATION *pTran);
VOID PrtPluTicketList (TRANINFORMATION *pTran, ULONG ulPrintOptions, PluChitInfo *PluChitInfoList, int PluChitInfoListIndex, int PluChitOrderDeclareAdr);
VOID  PrtCallIPD(TRANINFORMATION  *pTran, PifFileHandle hsStorage);
VOID PrtCallIPDTicketInfo ( TRANINFORMATION *pTran, PifFileHandle hsStorage, PluChitInfoAll *PluChit );
BOOL  PrtIsItemInWorkBuff( USHORT usItemOffset, UCHAR  *puchWorkBuff, USHORT usBuffOffset, USHORT usBuffSize );
BOOL PrtIsIDiscInWorkBuff( USHORT usItemOffset, UCHAR  *puchWorkBuff, USHORT usBuffSize );
VOID PrtSetSalesCouponQTY(CONST TRANINFORMATION *pTran, ITEMSALES *pItem, PifFileHandle sFileHandle);

/* -- PrRHeadT.c -- */
VOID  PrtItemPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtChkCurSlip(CONST TRANINFORMATION  *pTran);
VOID  PrtTraining(CONST TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPVoid(CONST TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPReturn(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtMoneyHeader(CONST TRANINFORMATION *pTran, SHORT fsPrintStatus);  /* Saratoga */
VOID  PrtPostRct(CONST TRANINFORMATION *pTran, CONST ITEMPRINT *pItem);
VOID  PrtCPGusLine(CONST TRANINFORMATION *pTran, CONST ITEMPRINT *pItem);
// VOID  PrtCPFuncErrCode(ITEMPRINT *pItem);
VOID  PrtCPFuncErrCode_EJ(CONST ITEMPRINT *pItem);
VOID  PrtEPTError(ITEMPRINT  *pItem);
VOID  PrtEPTStub(CONST TRANINFORMATION *pTran, ITEMPRINT  *pItem );
VOID  PrtEPTStubNoAcct(CONST TRANINFORMATION *pTran, ITEMPRINT  *pItem );
VOID  PrtDispPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtDflDispNumber( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtAgeLog(CONST TRANINFORMATION *pTran, CONST ITEMPRINT *pItem);
VOID PrtDflAgeLog( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtTHCPRespText(ITEMPRINT *pItem);
VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem);        /* Saratoga */

USHORT PrtDispPrintForm(TRANINFORMATION  *pTran, ITEMPRINT  *pItem, TCHAR *puchBuffer);
USHORT PrtDflDispNumberForm( TRANINFORMATION *pTran, ITEMPRINT *pItem ,TCHAR *puchBuffer);
USHORT PrtDflAgeLogForm( TRANINFORMATION *pTran, ITEMPRINT *pItem, TCHAR *puchBuffer);

/* --- DTREE#2 Check Endorsement Dec/18/2000 --- */
VOID    PrtEndorse(TRANINFORMATION *pTran, ITEMPRINT *pItem);
USHORT  PrtSPEndorsHeadH(TCHAR *pszWork, UCHAR uchAdress, TRANINFORMATION *pTran);
USHORT  PrtSPEndorsMnemH1(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPEndorsMnemH2(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPEndorsTrail2(TCHAR *pszWork, TRANINFORMATION *pTran);
USHORT  PrtSPEndorsTrail3(TCHAR *pszWork, TRANINFORMATION *pTran);
USHORT  PrtSPEndorsHeadV(TCHAR *pszWork, TRANINFORMATION *pTran);
USHORT  PrtSPEndorsMnemAmtV1(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPEndorsMnemAmtV2(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPEndorsTrail1(TCHAR *pszWork, TRANINFORMATION *pTran);

/* -- PrRSaleT.c -- */
VOID  PrtItemSales(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDispSales(TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT   PrtDispSalesForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRDiscT.c -- */
VOID  PrtItemDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
VOID  PrtDispDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT  PrtDispDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRMiscT.c -- */
VOID  PrtItemMisc(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtDispMisc(TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT   PrtDispMiscForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRTendT.c -- */
VOID  PrtItemTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtDispTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);
USHORT   PrtDispTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer);

/* -- PrRTaxT.c -- */
VOID  PrtItemAffection(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDispAffection(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);

/* -- PrROpenT.c -- */
VOID  PrtItemTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);
VOID  PrtDispTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);
USHORT   PrtDispTransOpenForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer);

/* -- PrRTtlT.c -- */
VOID  PrtItemTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDispTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT PrtDispTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrRMulT.c -- */
VOID  PrtItemMulti(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);
VOID  PrtDispMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);


/* -- PrRIModT.c -- */
VOID  PrtItemModifier(CONST TRANINFORMATION  *pTran, CONST ITEMMODIFIER *pItem);
VOID  PrtDispModifier(CONST TRANINFORMATION  *pTran, CONST ITEMMODIFIER *pItem);
VOID  PrtDflModTable(CONST TRANINFORMATION *pTran, CONST ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModPerson(CONST TRANINFORMATION *pTran, CONST ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModCustName(CONST TRANINFORMATION *pTran, CONST TCHAR* pszCustomerName);
USHORT  PrtDispModifierForm(CONST TRANINFORMATION  *pTran, CONST ITEMMODIFIER *pItem, TCHAR *puchBuffer);

/* -- PrRCpnT.c -- */
VOID  PrtItemCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDispCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon   ( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
USHORT  PrtDispCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer );
USHORT  PrtDflCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer );

/* PrRPromT.c */
VOID  PrtPromotion(CONST TRANINFORMATION  *pTran, SHORT fsPrintStatus);
VOID  PrtTransactionBegin(SHORT sType, ULONG ulTransNum);
VOID  PrtTransactionEnd(SHORT sType, ULONG ulTransNum);

/* PrRTrilT.c */
VOID  PrtDoubleHeader(TRANINFORMATION  *pTran);
VOID  PrtSoftCHK(UCHAR uchMinorClass);
VOID  PrtTrail1(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
VOID  PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT *pItem); 
VOID  PrtTrail1_TH(TRANINFORMATION  *pTran, ULONG  ulStRegNo);                          
VOID  PrtTrail1_EJ(TRANINFORMATION  *pTran, ULONG  ulStRegNo);                          
VOID  PrtTrail2(TRANINFORMATION  *pTran);    
VOID  PrtTrail3(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);    
VOID  PrtSoftProm(ITEMPRINT  *pItem);  
VOID  PrtSoftProm_EPT(ITEMPRINT  *pItem);  
VOID  PrtTrayTrail(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);    
VOID  PrtChkPaid(ITEMPRINT  *pItem);      
VOID  PrtChkPaid_SP(VOID);      

/* -- PrRDeptT.c -- */
VOID  PrtDept(TRANINFORMATION  *pTran, ITEMSALES  *pItem); 
VOID  PrtPLU(TRANINFORMATION* pTran, ITEMSALES* pItem);
VOID  PrtDeptPLU_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflDeptPLUForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflPrtModForm(TRANINFORMATION  *pTran, UIFREGSALES *pUifRegSales, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPluT.c -- */
// VOID  PrtPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem); // move to same file as PrtDept() since uses same print routines.

/* -- PrRSetT.c -- */
VOID  PrtSET(TRANINFORMATION *pTran, ITEMSALES  *pItem); 
VOID  PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflSETForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRModDT.c -- */
VOID  PrtModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem); 
VOID  PrtDflModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtDflSetModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflSetModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */ 

/* -- PrRIRDiT.c -- */
VOID  PrtItemRegDisc(CONST TRANINFORMATION *pTran, CONST ITEMDISC *pItem);
VOID  PrtDflRegDisc(CONST TRANINFORMATION  *pTran, CONST ITEMDISC  *pItem);
USHORT PrtDflRegDiscForm(CONST TRANINFORMATION  *pTran, CONST ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTipT.c -- */
VOID  PrtChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);    
ULONG PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC  *pItem);    
VOID PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT PrtDflChrgTipForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPOT.c -- */
VOID  PrtPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRROAT.c -- */
VOID  PrtRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtGetMoneyMnem(UCHAR uchTendMinor, USTRNADRS *pusTran, UCSPCADRS *puchSym);
VOID  PrtMoney(TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT PrtDflRecvOnAcntForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRTDT.c -- */
VOID  PrtTipsDecld(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem);
VOID  PrtDflTipsDecld(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem);
USHORT PrtDflTipsDecldForm(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRTPOT.c -- */
VOID  PrtTipsPaidOut(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem);    
VOID  PrtDflTipsPaidOut(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem);    
USHORT PrtDflTipsPaidOutForm(CONST TRANINFORMATION  *pTran, CONST ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRNoSlT.c -- */
VOID  PrtNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflNoSaleForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRChkTT.c -- */
VOID  PrtCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtDflCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    

/* -- PrRCnclT.c -- */
VOID  PrtCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflCancelForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrREcT.c -- */
VOID  PrtErrCorr(ITEMTENDER *pItem);    

/* -- PrREtkT.c -- */
VOID  PrtEtkIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtEtkOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem,  SHORT sType);    

/* -- PrRNTndT.c -- */
VOID  PrtTHCPRespTextGiftCard(CONST TRANINFORMATION* pTran, ITEMPRINT* pItem);
VOID  CheckGiftCardPrt(CONST TRANINFORMATION* pTran, ITEMTENDER* pItem);
VOID  PrtTender(CONST TRANINFORMATION  *pTran, ITEMTENDER *pItem);      
VOID  PrtDflTender(CONST TRANINFORMATION  *pTran, ITEMTENDER *pItem);
USHORT PrtDflTenderForm(CONST TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer);
// SHORT CheckGiftMnem(ITEMTENDER *pItem);

/* -- PrRFTndT.c -- */
VOID  PrtForeignTender(CONST TRANINFORMATION  *pTran, CONST ITEMTENDER *pItem);
VOID  PrtDflForeignTender(CONST TRANINFORMATION  *pTran, CONST ITEMTENDER *pItem);
VOID  PrtEuroTender(CONST TRANINFORMATION  *pTran, CONST ITEMTENDER *pItem);
USHORT PrtDflForeignTenderForm(CONST TRANINFORMATION  *pTran, CONST ITEMTENDER  *pItem, TCHAR *puchBuffer);

/* -- PrRTaxT.c -- */
VOID  PrtTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);       
VOID  PrtDflTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);       
USHORT PrtDispAffectionForm(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem, TCHAR *puchBuffer);
USHORT PrtDflTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);

/* -- PrRUTaxT.c -- */
VOID  PrtUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
USHORT  PrtDflUSCanTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);

/* -- PrRCTaxT.c -- */
VOID  PrtCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);
VOID  PrtDflCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);

/* -- PrRAdd2T.c --*/
VOID  PrtAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);   
VOID  PrtDflAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);   

/* -- PrROpenT.c -- */

/* -- PrRCasIT.c -- */
VOID  PrtCasIn(TRANINFORMATION* pTran, ITEMTRANSOPEN* pItem);

/* -- PrRNchkT.c -- */
VOID  PrtNewChk(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);    

/* -- PrRReodT.c -- */
VOID  PrtReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);   
VOID  PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem);
USHORT PrtDflReorderForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer);

/* -- PrRAdd1T.c -- */
VOID  PrtAddChk1(TRANINFORMATION* pTran, ITEMTRANSOPEN* pItem);

/* -- PrRSRecT.c -- */
VOID  PrtStoreRecall( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem );

/* -- PrCFeed_.c -- */
VOID  PrtItemOther(TRANINFORMATION *pTran, ITEMOTHER  *pItem);
VOID  PrtSlpRel(VOID);                             
VOID  PrtInFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem);                             
VOID  PrtOutFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem);                             
VOID  PrtResetLog(TRANINFORMATION  *pTran, ITEMOTHER *pItem);
VOID  PrtPowerDownLog(TRANINFORMATION *pTran, ITEMOTHER *pItem);    /* Saratoga */

/* -- PrRTotlT.c -- */
VOID  PrtTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalStub(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalOrder(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflForeignTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT  PrtDflTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);
USHORT PrtDflForeignTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrROrdDecT.c -- */
VOID  PrtOrderDec(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
SHORT PrtOrderDecSearch(CONST TRANINFORMATION *pTran, ITEMSALES *pItem);

/* -- PrRSevTT.c -- */
VOID  PrtServMaskAcctDate(TCHAR *pAcctNumber, ITEMTOTAL *pItem);
VOID  PrtServTotal(CONST TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost(CONST TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalStubPost(CONST TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotalStubPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT PrtDflServTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);
USHORT PrtDflServTotalStubPostForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrRTrayT.c -- */
VOID  PrtTrayTotal(CONST TRANINFORMATION *pTran, CONST ITEMTOTAL *pItem);
VOID  PrtDflTrayTotal(CONST TRANINFORMATION *pTran, CONST ITEMTOTAL *pItem);
USHORT  PrtDflTrayTotalForm(CONST TRANINFORMATION  *pTran, CONST ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrRMulCT.c -- */
VOID  PrtMulti(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);
VOID  PrtDflMulti(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);

/* -- PrRMulPT.c -- */
VOID  PrtMultiPost(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);

/* -- PrRMlTlT.c -- */
VOID  PrtMultiTotal(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);
VOID  PrtDflMultiTotal(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);

/* -- PrRMlTPT.c -- */
VOID  PrtMultiPostTotal(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem);

/* -- PrRComn_.c -- */
VOID  PrtSlipInitial(VOID);
VOID  PrtSlipInitialLP(VOID);           /* Saratoga */
VOID  PrtSlpCompul(VOID);
VOID  PrtRectCompul(VOID);
VOID  PrtDouble(TCHAR *pszDest, USHORT   usDestLen, CONST TCHAR *pszSource); 
VOID  PrtSDouble(TCHAR *pszDest, USHORT   usDestLen, CONST TCHAR *pszSource); 
VOID  PrtPortion(SHORT fsPrtState);
VOID  PrtPortion2(CONST ITEMSALES *pItem);
VOID  PrtPortion3(SHORT fsPrtState);    /* R3.1 */
VOID  PrtGetVoid(TCHAR *aszVoidMnem, USHORT usStrlen);           
SHORT PrtGetReturns(USHORT fbMod, USHORT usReasonCode, TCHAR *pszVoidMnem, USHORT usStrLen);
VOID  PrtGetOffTend(USHORT fbMod, TCHAR *aszOffMnem, USHORT usStrlen);           
USHORT  PrtFeed(USHORT  usPrtType, USHORT  usLineNum);  
USHORT  PrtLogo(USHORT  usPrtType, USHORT usSize);        
USHORT  PrtCut(VOID);                                           
SHORT PrtChkPort(VOID);                                       
VOID  PrtGetScale(TCHAR *pszMnem, SHORT *psDecPoint, LONG *plWeight, CONST TRANINFORMATION  *pTran, CONST ITEMSALES  *pItem);
VOID  PrtGetDate(TCHAR *pszDest, USHORT usDestLen, const TRANINFORMATION *pTran);
VOID  PrtGetJobInDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC  *pItem);
VOID  PrtGetJobOutDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC  *pItem);
SHORT PrtGetTaxMod(TCHAR *pszDest, USHORT usDestLen, SHORT fsTax, USHORT fbMod);
VOID  PrtGetLead(TCHAR  *pszDest, USHORT usAdr);
TCHAR *PrtSetNumber(TCHAR  *pszDest, CONST TCHAR  *pszNumber);
VOID  PrtSetItem(TCHAR  *pDest, CONST ITEMSALES  *pItem, DCURRENCY *plProduct, SHORT  sType, USHORT usColumn);
USHORT  PrtSetChildPriority( TCHAR *puchDest, CONST ITEMSALES *pItem, DCURRENCY *plAllPrice, SHORT sType, USHORT usColumn );
USHORT  PrtSetChildNotOrder( TCHAR *puchDest, CONST ITEMSALES *pItem, DCURRENCY *plAllPrice, SHORT sType, USHORT usColumn );
SHORT PrtGet1Line(TCHAR *pStart, TCHAR **pEnd, USHORT usColumn);
SHORT PrtChkCasWai(TCHAR *pszMnem, ULONG ulCasID, ULONG ulWaiID);
VOID  PrtGetAllProduct(CONST ITEMSALES *pItem, DCURRENCY *plProduct);
VOID  PrtAdjustNative(TCHAR *pszDest, DCURRENCY lAmount);
VOID  PrtFillSpace(TCHAR  *pszString, USHORT  usSize);
USHORT PrtPrint(USHORT usPrtType, CONST TCHAR *puchBuff, USHORT usLen);
USHORT PrtPrintf(USHORT usPrtType, const TCHAR  *pszFormat, ...);
USHORT PrtAdjustString(TCHAR *pszDest, TCHAR *pszSorce, USHORT usSorceLen, USHORT usColumn);
VOID  PrtPmgWait(VOID);
VOID  PrtSlipPmgWait(VOID);
SHORT PrtChkFullSP(TCHAR  *pszWork);
USHORT  PrtSPVLInit(VOID);
SHORT PrtChkVoidConsolidation(CONST TRANINFORMATION *pTran, CONST ITEMSALES *pItem);
SHORT PrtChkPrintPluNo(CONST ITEMSALES *pItem);
SHORT PrtChkPrintType(CONST ITEMSALES *pItem);
VOID  PrtSetQtyDecimal(SHORT *psDecPoint, LONG  *plQty);            /* Saratoga */
VOID  PrtResetQtyDecimal(SHORT *psDecPoint, LONG  *plQty);
USHORT  PrtGetTaxMod2(TCHAR *pszDest, USHORT usDestLen, UCHAR uchMinorClass); /* 2172 */
SHORT   PrtGetSISymSales( TCHAR  *aszSISym, CONST ITEMSALES *pItem );
SHORT   PrtGetSISymSalesUS( TCHAR  *aszSISym, CONST ITEMSALES *pItem );
SHORT    PrtGetSISymSalesCanada( TCHAR  *aszSISym, CONST ITEMSALES *pItem );
SHORT   PrtGetSISymSalesIntl( TCHAR  *aszSISym, CONST ITEMSALES *pItem );
SHORT   PrtGetSISym( TCHAR  *aszSISym, UCHAR uchOffset1, UCHAR uchOffset2 );
SHORT    PrtGetSISymDisc( TCHAR  *aszSISym, CONST ITEMDISC *pItem );
SHORT    PrtGetSISymCoupon( TCHAR  *aszSISym, CONST ITEMCOUPON *pItem );
SHORT   PrtCheckTaxSystem(VOID);

/* -- PrRCmnT_.c -- */
VOID  PrtTHVoid(USHORT  fbMod, USHORT usReasonCode);
VOID  PrtTHOffTend(USHORT  fbMod);
VOID  PrtTHNumber(CONST TCHAR  *pszNumber);
VOID  PrtTHMnemNumber(USHORT usAdr, CONST TCHAR  *pszNumber);
VOID  PrtTHAmtMnem(USTRNADRS usAdr, DCURRENCY lAmount);
VOID  PrtTHAmtTwoMnem(USTRNADRS usAdr1, USTRNADRS usAdr2, DCURRENCY lAmount, TCHAR *pCardLabel);
VOID  PrtTHAmtMnemMnem(USTRNADRS usTranAdr1, USTRNADRS usAdr2, DCURRENCY lAmount);
VOID  PrtTHMnem(USTRNADRS usTranAdr, BOOL fsType);
VOID  PrtTHAmtSymEuro(USTRNADRS usTranAdr1, UCSPCADRS uchSpcAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
VOID  PrtTHAmtTaxMnem(USTRNADRS usAdr, USHORT fsTax, USHORT  fbMod, DCURRENCY lAmount);
VOID  PrtTHTaxMod(USHORT fsTax, USHORT  fbMod);
VOID  PrtTHPerDisc(USTRNADRS usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtTHAmtSym(USTRNADRS usAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtTHOrderDec(USTRNADRS uchTranAdr);
VOID  PrtTHTblPerGC(USHORT usTblNo, USHORT usNoPerson, USHORT usGuestNo, UCHAR uchCDV, TCHAR uchSeatNo, SHORT sWidthType);
VOID PrtTHTentName(CONST TCHAR *pszCustomerName );
VOID  PrtTHTblPerson(USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType);
VOID  PrtTHCustomerName(CONST TCHAR *pszCustomerName);
VOID  PrtTHMultiSeatNo(CONST TRANINFORMATION *pTran);
VOID  PrtTHTranNum(USTRNADRS usTranAdr, ULONG ulNumber);
VOID  PrtTHKitchen(UCHAR  uchKPNo);
VOID  PrtTHWaiAmtMnem(ULONG ulWaiID, USTRNADRS usTranAdr, DCURRENCY lAmount);
VOID  PrtTHWaiGC(USTRNADRS usTranAdr, ULONG ulWaiID, USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtTHForeign(DCURRENCY lForeign, UCHAR uchMinorClass, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
VOID  PrtTHEuro(UCSPCADRS uchAdr1, LONG lForeign, UCSPCADRS uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
VOID  PrtTHSeatNo(TCHAR uchSeatNo);
VOID  PrtTHSeatNo2(TCHAR uchSeatNo);
VOID  PrtTHQty(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtTHLinkQty(TRANINFORMATION  *pTran, ITEMSALES *pItem);
VOID  PrtTHWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtTHChild(UCHAR uchAdj, TCHAR *pszMnem);
VOID  PrtTHLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
VOID  PrtTHItems(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtTHDiscItems(ITEMSALES  *pItem);
SHORT PrtTHHead(USHORT usConsNo);
VOID  PrtTHDoubleHead(VOID);
VOID  PrtTHZeroAmtMnem(USTRNADRS usAddress, DCURRENCY lAmount);
VOID  PrtTHGuest(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtTHMulChk(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtTHWaiTaxMod(ULONG ulID,  USHORT fsTax, USHORT  fbMod);
VOID  PrtTHMnemCount(USTRNADRS usAdr, SHORT sCount);
VOID  PrtTHTrail1(TRANINFORMATION *pTran);
VOID  PrtTHTrail2(TRANINFORMATION *pTran, ULONG  ulStRegNo);
VOID  PrtTHTickTrail1(TRANINFORMATION  *pTran);
VOID  PrtTHTickTrail2(TRANINFORMATION  *pTran);
VOID  PrtTHEtkCode(ITEMMISC *pItem, SHORT sType);
VOID  PrtTHEtkOut(USTRNADRS usTranAdr1, USTRNADRS usTranAdr2);
VOID  PrtTHJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtTHJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType);
VOID  PrtTHCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT PrtTHCPRspMsgText(TCHAR *pRspMsgText);
VOID  PrtTHOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
VOID  PrtTHEPTError(ITEMPRINT  *pItem);
VOID  PrtTHCoupon( ITEMCOUPON *pItem );
VOID    PrtTHMAmtShift(USTRNADRS usTranAdr, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtTHTaxRateAmt(USTRNADRS usTranAdr,  LONG lRate, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtTHVatMnem(ITEMAFFECTION  *pItem);
VOID    PrtTHVatAmt(ITEMVAT ItemVAT);
VOID    PrtTHPLUNo(TCHAR *puchPLUCpn); /* 2172 */
VOID    PrtAdjustForeign(TCHAR *pszDest, DCURRENCY lAmount, UCSPCADRS uchSpcAdr, UCHAR uchMDC);
VOID    PrtTHMoneyForeign (DCURRENCY lForeign, USTRNADRS usTranAdr1, UCSPCADRS uchSpcAdr2, UCHAR fbStatus);
VOID    PrtSupSPHead(CONST TRANINFORMATION *pTran, VOID *pHeader);
VOID    PrtSupSPTrailer(VOID *pTrail);    // MAINTTRAILER* pTrail = pItem;
SHORT   PrtSupCompSlip(VOID);
VOID    PrtSupRemoveSlip(VOID);
VOID  PrtTHPerDiscSISym(USTRNADRS usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);
VOID  PrtTHItemCount(SHORT noItems, USHORT usPrtType);
VOID  PrtTHTipTotal(USHORT  usPrtType);
VOID  PrtTHAmtGiftMnem(ITEMTENDER  *pItem);

/* -- PrRCmnJ_.c -- */
VOID  PrtEJVoid(USHORT  fbMod, USHORT usReasonCode);
VOID  PrtEJOffTend(USHORT  fbMod);
VOID  PrtEJNumber(CONST TCHAR  *pszNumber);
VOID  PrtEJMnemNumber(USHORT usAdr, TCHAR  *pszNumber);
VOID  PrtEJAmtMnem(USTRNADRS usAdr, DCURRENCY lAmount);
VOID  PrtEJMnem(USTRNADRS usAdr, BOOL fsType);
VOID  PrtEJTaxMod(USHORT fsTax, USHORT  fbMod);
VOID  PrtEJPerDisc(USTRNADRS usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtEJAmtSym(USTRNADRS usAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtEJOrderDec(USTRNADRS usTranAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtEJAmtSymEuro(USTRNADRS usTranAdr1, UCSPCADRS uchSpcAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
VOID  PrtEJTblPerson(USHORT usTblNo, USHORT usNoPerson, TCHAR uchSeatNo, SHORT sWidthType);
VOID  PrtEJCustomerName(CONST TCHAR *pszCustomerName);
VOID  PrtEJMultiSeatNo(TRANINFORMATION *pTran);
VOID  PrtEJTranNum(USTRNADRS usTranAdr, ULONG ulNumber);
VOID  PrtEJWaiter(ULONG usWaiID);
VOID  PrtEJForeign1(DCURRENCY lForeign, UCSPCADRS uchAdr, UCHAR fbStatus);
VOID  PrtEJForeign2(ULONG ulRate, UCHAR fbStatus2);
VOID  PrtEJEuro(UCSPCADRS uchAdr1, DCURRENCY lForeign, UCSPCADRS uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
VOID  PrtEJSeatNo(TCHAR uchSeatNo);
VOID  PrtEJQty(ITEMSALES  *pItem);
VOID  PrtEJLinkQty(ITEMSALES  *pItem);
VOID  PrtEJWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtEJChild(UCHAR uchAdj, TCHAR *pszMnem);
VOID  PrtEJLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
VOID  PrtEJItems(ITEMSALES  *pItem);
VOID  PrtEJDiscItems(ITEMSALES  *pItem);
VOID  PrtEJZeroAmtMnem(USTRNADRS usAddr, DCURRENCY lAmount);
VOID  PrtEJGuest(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtEJMulChk(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtEJWaiTaxMod(ULONG ulID,  USHORT fsTax, USHORT  fbMod);
VOID  PrtEJMnemCount(USHORT usTranAdr, SHORT sCount);
VOID  PrtEJTrail1(TRANINFORMATION *pTran, ULONG  ulStRegNo);
VOID  PrtEJTrail2(TRANINFORMATION  *pTran);
VOID  PrtEJInit(const TRANINFORMATION  *pTran);
VOID  PrtForceEJInit(const TRANINFORMATION  *pTran);
VOID  PrtEJWrite(CONST VOID  *pWrite, USHORT usWriteLen);
VOID  PrtEJSend(VOID);
VOID  PrtEJEtkCode(ITEMMISC *pItem, SHORT sType);  
VOID  PrtEJJobTimeIn(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);  
VOID  PrtEJJobTimeOut(TRANINFORMATION *pTran, ITEMMISC *pItem);  
VOID  PrtEJCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID);
VOID  PrtEJFolioPost(TCHAR *pszFolioNo, TCHAR *pszPostTranNo);
USHORT  PrtEJCPRspMsgText(TCHAR *pRspMsgText);
VOID  PrtEJOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
VOID  PrtEJEPTError(ITEMPRINT  *pItem);
VOID  PrtEJCoupon( ITEMCOUPON *pItem );
VOID    PrtEJResetLog(ITEMOTHER *pItem);
VOID    PrtEJResetLog3(ITEMOTHER *pItem);
VOID    PrtEJResetLog4(ITEMOTHER *pItem);
VOID    PrtEJPowerDownLog(ITEMOTHER *pItem);        /* Saratoga */
VOID    PrtEJZAMnemShift(USTRNADRS usTranAdr, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtEJAmtMnemShift(USTRNADRS usTranAdr, DCURRENCY lAmout, USHORT usColumn);
VOID  PrtEJPLUNo(TCHAR *puchPLUCpn); /* 2172 */
VOID    PrtEJPLUBuild(USHORT usTranAdr, TCHAR *puchPLUNo, USHORT usDeptNo); /* 2172 */
VOID    PrtEJMoneyForeign (DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus);
VOID  PrtEJPerDiscSISym(USTRNADRS usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);
VOID  PrtEJAmtAndMnemonic (ITEMTENDER  *pItem);

/* -- PrRCmnS_.c -- */
TCHAR*  PrtTruncDoubleString(TCHAR* aszCustomerName, size_t len, CONST TCHAR* pszCustomerName);
USHORT  PrtSPVoidNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, CONST TCHAR *pszNumber);
USHORT  PrtSPVoidMnemNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, USHORT usAdr, TCHAR *pszNumber);
USHORT  PrtSPMnemAmt(TCHAR *pszWork, USTRNADRS usAdr, DCURRENCY lAmount);
USHORT  PrtSPDiscount(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
USHORT  PrtSPTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod);
USHORT  PrtSPMnemTaxAmt(TCHAR *pszWork, USHORT usAdr, USHORT fsTax, USHORT fbMod, DCURRENCY lAmount);
/*
USHORT  PrtSPMnemTaxWeight(UCHAR *pszWork ,UCHAR uchAdr, USHORT fsTax, ITEMSALES *pItem);
*/
USHORT  PrtSPMnemTaxQty(TCHAR *pszWork, USHORT usTranAdr, USHORT fsTax, ITEMSALES *pItem);
USHORT  PrtSPSeatNo(TCHAR *pszWork, TCHAR uchSeatNo);
USHORT  PrtSPQty(TCHAR *pszWork, TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT  PrtSPLinkQty(TCHAR *pszWork, TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT  PrtSPWeight(TCHAR *pszWork, USHORT fsTax, ULONG flScale, ITEMSALES *pItem);
USHORT  PrtSPMnlWeight(TCHAR *pszWork, DCURRENCY lAmount);
USHORT  PrtSPTranMnem(TCHAR *pszWork, USHORT usAdr);
USHORT  PrtSPTblPerson(TCHAR *pszWork, USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType);
USHORT  PrtSPCustomerName( TCHAR *pszDest, CONST TCHAR *pszCustomerName );
USHORT  PrtSPMultiSeatNo( TCHAR *pszDest, CONST TRANINFORMATION *pTran);
USHORT  PrtSPChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  PrtSPLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
USHORT  PrtSPTipsPO(TCHAR *pszWork, ULONG ulWaiID, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPGCTransfer(TCHAR *pszWork, ITEMMISC *pItem);
USHORT  PrtSPGstChkNo(TCHAR *pszWork, CONST ITEMMULTI *pItem);
USHORT  PrtSpForeign(TCHAR *pszWork, DCURRENCY lForeign, UCSPCADRS uchAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
USHORT  PrtSPMnemNativeAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtSPServTaxSum(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtSPChargeTips(TCHAR *pszWork, USHORT usAdr, ULONG ulWaiID, USHORT fsTax, ITEMDISC *pItem);
USHORT  PrtSPCancel(TCHAR *pszWork, USHORT usTranAdr1, USHORT usTranAdr2, DCURRENCY lAmount);
USHORT  PrtSPTrail1(TCHAR *pszWork, CONST TRANINFORMATION  *pTran, USHORT usSlipLine);
USHORT  PrtSPTrail2(TCHAR *pszWork, CONST TRANINFORMATION  *pTran);
USHORT  PrtSPChkPaid(TCHAR *pszWork, USHORT usAdr);
USHORT  PrtSPTrayCount(TCHAR *pszWork, USHORT usAdr, SHORT sTrayCo);
USHORT  PrtSPServiceTotal(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType, CONST TRANINFORMATION  *pTran, USHORT usSlipLine);
USHORT  PrtSPGCTranTrail(TCHAR  *pszWork, ITEMMISC *pItem);
USHORT  PrtSPHeader1(TCHAR *pszWork, CONST TRANINFORMATION  *pTran, SHORT sWidthType);
USHORT  PrtSPHeader2(TCHAR *pszWork, CONST TRANINFORMATION  *pTran, TCHAR uchSeatNo);
USHORT  PrtSPHeader3(TCHAR *pszWork, CONST TRANINFORMATION  *pTran);
USHORT  PrtSPItems(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMSALES  *pItem);
USHORT  PrtCheckLine(USHORT  usSPCurrLine, CONST TRANINFORMATION  *pTran);
USHORT  PrtSPEtkTimeIn(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT  PrtSPEtkTimeOut(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);
USHORT  PrtSPCPRoomCharge(TCHAR  *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT  PrtSPCPOffTend(TCHAR *pszWork, USHORT fbMod);
USHORT  PrtSPCPRspMsgText(TCHAR *pszWork, CONST TCHAR *pszRspMsgText);
VOID    PrtSPVLTrail(CONST TRANINFORMATION *pTran);
VOID    PrtSPVLHead(CONST TRANINFORMATION *pTran);
USHORT  PrtSPOffline(TCHAR *pszWork, USHORT fbMod, CONST TCHAR *auchDate, CONST TCHAR *auchApproval);
VOID    PrtTrailEPT_SP( TCHAR   *pszWork );
USHORT  PrtSPTranNumber(TCHAR *pszWork, USHORT usTranAdr, TCHAR *pszNumber);
USHORT  PrtSPCoupon( TCHAR *pszDest, CONST TCHAR *pszMnemonic, DCURRENCY lAmount );
USHORT  PrtSPTaxMnem(TCHAR  *pszWork, CONST ITEMAFFECTION  *pItem);
USHORT  PrtSPTaxAmt(TCHAR  *pszWork, CONST ITEMVAT  ItemVAT);
USHORT  PrtSPMAmtShift(TCHAR *pszWork, USTRNADRS usAdr, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPTaxRateAmt(TCHAR *pszWork, USTRNADRS usAdr,  ULONG ulRate, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPAmtSymEuro(TCHAR  *pszWork, USTRNADRS usAdr1, UCSPCADRS uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
USHORT  PrtSPEuro(TCHAR  *pszWork, UCSPCADRS uchAdr1, DCURRENCY lForeign, UCSPCADRS uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
USHORT  PrtSPPLUNo(TCHAR *pszDest, CONST TCHAR *puchPLUCpn); /* 2172 */
USHORT  PrtSPPLUBuild (TCHAR *pszDest, USTRNADRS usTranAdr, CONST TCHAR *puchPLUNo, USHORT usDeptNo);
USHORT  PrtSPForeignQtyFor(TCHAR *pszWork, LONG lQTY, DCURRENCY lAmount, UCSPCADRS uchAdr, UCHAR fbStatus);    /* Saratoga */
USHORT  PrtSPMoneyForeign(TCHAR* pszWork, DCURRENCY  lForeign, USTRNADRS usTranAdr1, UCSPCADRS uchAdr2, UCHAR fbStatus);
USHORT  PrtSPQtyFor(TCHAR *pszWork, LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice);
USHORT  PrtSPDiscountSISym(TCHAR *pszWork, USTRNADRS usAdr, UCHAR uchRate, DCURRENCY lAmount, CONST TCHAR *pszSISym, UCHAR uchMDC);
USHORT  PrtSPCouponSISym( TCHAR *pszDest, CONST TCHAR *pszMnemonic, DCURRENCY lAmount, CONST TCHAR *pszSISym, TCHAR uchMDC);

/* -- PrRKP_.c -- */
USHORT PrtKPNumber(TCHAR *pszDest, USHORT usDestLen,TCHAR  *pszNumber);
VOID   PrtKPInit(VOID);
VOID   PrtKPHeader(CONST TRANINFORMATION *pTran);
VOID   PrtKPCustomerName(CONST TCHAR *pszCustomerName);
VOID   PrtKPTrailer(CONST TRANINFORMATION *pTran, ULONG  ulStReg);
VOID   PrtKPTotal(CONST ITEMTOTAL *pItem );
VOID   PrtKPOperatorName(CONST TRANINFORMATION *pTran);
VOID   PrtKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPSetMenuItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPChild(TCHAR *pszDest, ITEMSALES  *pItem, USHORT usCondNo);
USHORT PrtKPSeatNo(TCHAR *pszDest, USHORT usDestLen, ITEMSALES  *pItem);
USHORT PrtKPWeight(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPQty(TCHAR *pszDest, USHORT usDestLen, LONG lQty);
USHORT PrtKPNoun(TCHAR *pszDest, ITEMSALES  *pItem);
USHORT PrtKPPM(TCHAR *pszDest, ITEMSALES  *pItem);
SHORT  PrtSetFailKP(USHORT usKPState);
VOID   PrtChkTotalKP(CONST TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
VOID   PrtChkTotalKP2(CONST TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
SHORT  PrtChkKPPort(CONST TRANINFORMATION *pTran, CONST ITEMSALES *pItem, UCHAR *puchPort, UCHAR fbType);
SHORT  PrtChkKPPortSub(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPPortSub2(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPEnd(VOID);
VOID   PrtTakeToKP(TRANINFORMATION *pTran, SHORT hsStorage);
SHORT   PrtChkKPStorageEx(TCHAR *pszBuff, USHORT usBuffLen, CHAR fbPrtState, CHAR fbState, USHORT usTerm);

/* ### ADD 2172 Rel1.0 (Saratoga) */
SHORT   PrtSendKps(TCHAR *auchSndBuffer, USHORT usSndBufLen, UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,SHORT nKPNo);


/* -- PrRVATT.c -- */
VOID    PrtVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtDflVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtDflVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtDflVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
USHORT  PrtDflVATServForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);
USHORT  PrtDflVATForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);

/* -- PrRCmnt_.c -- */
VOID    PrtSupTHVoid(USHORT fsMod, USHORT usReasonCode);

/* -- Prslpjt_.c -- */
/*VOID    PrtSupLoanPickup_EJ(MAINTLOANPICKUP *pData);*/

/* -- Prrcomn_.c -- */
VOID    PrtEndValidation(USHORT  usPort);

/* ===== End of File ( PrtRIn.H ) ===== */
