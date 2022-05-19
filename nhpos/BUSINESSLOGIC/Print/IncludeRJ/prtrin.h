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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used in print module
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtRIn.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : K.Nakajima :
* Jun-24-93 : 01.00.12 : K.You      : add PRTCONTROL
* Jul-28-93 : 01.00.12 : K.You      : add PrtSoftCheckTotal, PrtSoftCheckTotal_RJ,
*           :          :            : PrtSoftCheckTotal_VL, PrtSoftCheckTotal_SP
* Aug-23-93 : 01.00.13 : R.Itoh     : add PrtSoftProm()
* Apr-04-94 : 00.00.04 : Yoshiko.J  : add PrtRJOffline(), PrtRJEPTError(),
*                                   : PrtVLOffline(), PrtSPOffline(), PrtTrailEPT_SP()
* Apr-24-94 : 00.00.05 : K.You      : add PRT_OFFCPAUTTEND, mod. PrtGetOffTend prtotype.
* May-16-94 : 02.05.00 : Yoshiko.Jim: add PrtServTotal_SPVL(), PrtSoftCheckTotal_SPVL()
* May-23-94 : 02.05.00 : K.You      : bug fixed Q-14 (chg. PRT_KPVOID  from 0x0020 to 0x0010)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PRT_CONSOL_CP_EPT)
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PRT_SEPARATOR for display on the fly.
* Mar-02-95 : 03.00.00 : T.Nakahata : Add Coupon Featue and Add Unique Trans
*                                     no. print
* Apr-17-95 : 03.00.00 : T.Nakahata : Add Transaction Mnemonic to Kitchen
* Jun-07-95 : 03.00.00 : T.Nakahata : Add Print Priority Feature
* Jul-24-95 : 03.00.04 : T.Nakahata : add Print customer name feature
* Jul-24-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Sep-13-95 : 03.00.08 : T.Nakahata : print unique tran# at Chit Rect (FVT)
*
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
* Jan-07-00 : 01.00.00 : H.Endo     : Add Include prtRJ.h for Printer RJ
* May-01-00 : 01.00.00 : M.Teraki   : Added function prototype declaraion of
*           :          :            : PrtSendKps()
* May-08-00 :          : M.Teraki   : fbPrtKPTarget removed.
* Aug-02-00 : V1.0.0.3 : M.Teraki   : Changed KP frame number to be managed for each KP
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
/* endo udpate(add) 2000/01/07 */
#include "pmg.h"
#include <prtRJ.h>
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
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
}PRTITEMDATA;

typedef struct {
    USHORT  fsComp;
    USHORT  fsNo;
}PRTCONTROL;


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
extern UCHAR   fbPrtKPSeatState;     /* save outputed kp number for Seat No, R3.1 */

extern USHORT  usPrtSlipPageLine;    /* current slip lines and pages */
extern USHORT  usPrtRJColumn;        /* receipt and journal's numbers of columns */
extern USHORT  usPrtVLColumn;        /* validation's numbers of columns */

extern PRTCONTROL PrtCompNo;         /* compulsory print & no print status save area */

extern UCHAR	fbPrtSendToStatus;		/* status ( send to file, send to printer ) */ //ESMITH PRTFILE
extern UCHAR	fchPrintDown;		/* save status of printer up or down */


//now using MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH (ecr.h)
//extern CONST TCHAR auchPrtNoPrintPLULow[]; /* 2172 */
//extern CONST TCHAR auchPrtNoPrintPLUHigh[]; /* 2172 */

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
/** --------------------------------- **\
     R E G I S T E R   M O D E
\** --------------------------------- **/

/* -- PrCTran_.c --*/
SHORT PrtChkStorage(SHORT *pRead1, SHORT *pRead2, TRANINFORMATION *pTran);
VOID  PrtCallIPD(TRANINFORMATION *pTran, SHORT hsStorage);
VOID  PrtPrintItemPrints ( TRANINFORMATION *pTran, SHORT hsStorage );
VOID  PrtKitchenCallIPD(TRANINFORMATION *pTran, SHORT hsStorage);   /* R3.1 */
BOOL PrtIsItemInWorkBuff( USHORT usItemOffset,
                          TCHAR  *puchWorkBuff,
                          USHORT usBuffOffset,
                          USHORT usBuffSize );
BOOL PrtIsIDiscInWorkBuff( USHORT usItemOffset,
                           TCHAR  *puchWorkBuff,
                           USHORT usBuffSize );
VOID PrtSetSalesCouponQTY(TRANINFORMATION *pTran, ITEMSALES *pItem, SHORT sFileHandle);

/* -- PrRHead_.c -- */
VOID  PrtItemPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtChkCurSlip(TRANINFORMATION  *pTran);
VOID  PrtTraining(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtTrain_RJ(TRANINFORMATION *pTran);
VOID  PrtTrain_SP(TRANINFORMATION *pTran);
VOID  PrtPVoid(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPReturn(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPVoid_RJ(TRANINFORMATION *pTran);
VOID  PrtPVoid_SP(TRANINFORMATION *pTran);
VOID  PrtMoneyHeader(TRANINFORMATION *pTran, SHORT fsPrintStatus);  /* Saratoga */
VOID  PrtMoneyHeader_RJ(TRANINFORMATION *pTran);                    /* Saratoga */
VOID  PrtMoneyHeader_SP(TRANINFORMATION *pTran);                    /* Saratoga */
VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtPostRct_RJ(TRANINFORMATION *pTran);
VOID  PrtPostRct_SP(TRANINFORMATION *pTran);
VOID  PrtCPGusLine(ITEMPRINT *pItem);
VOID  PrtCPGusLine_RJ(ITEMPRINT *pItem);
VOID  PrtCPFuncErrCode(ITEMPRINT *pItem);
VOID  PrtCPFuncErrCode_RJ(ITEMPRINT *pItem);
VOID  PrtEPTError(ITEMPRINT  *pItem);
VOID  PrtEPTStub(TRANINFORMATION *pTran, ITEMPRINT  *pItem);
VOID  PrtEPTStub_RJ(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtDispPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtParkReceipt_RJ( TRANINFORMATION *pTran );
VOID  PrtParkReceipt_SP( TRANINFORMATION *pTran );
VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtNumber_RJ(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtNumber_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtDflDispNumber( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtAgeLog_J(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtDflAgeLog( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem);       /* Saratoga */
VOID PrtCPRespText_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem);    /* Saratoga */
VOID PrtRJCPRespText(ITEMPRINT *pItem);                             /* Saratoga */
VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem);        /* Saratoga */
VOID  PrtFSCredit_RJ( ITEMPRINT *pItem );                           /* Saratoga */
VOID  PrtFSCredit_VL(TRANINFORMATION *pTran, ITEMPRINT *pItem);     /* Saratoga */
USHORT   PrtDispPrintForm(TRANINFORMATION  *pTran, ITEMPRINT  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflDispNumberForm( TRANINFORMATION *pTran, ITEMPRINT *pItem ,TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflAgeLogForm( TRANINFORMATION *pTran, ITEMPRINT *pItem, TCHAR *puchBuffer); /* 2172 */

VOID  PrtEndorse(TRANINFORMATION *pTran, ITEMPRINT *pItem);         /* DTREE#2 */
VOID   PrtEndorse_VL( TRANINFORMATION  *pTran, ITEMPRINT  *pItem);

/* -- PrRSale_.c -- */
VOID  PrtItemSales(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDispSales(TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT   PrtDispSalesForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRDisc_.c -- */
VOID  PrtItemDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
VOID  PrtDispDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT   PrtDispDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRMisc_.c -- */
VOID  PrtItemMisc(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtDispMisc(TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT   PrtDispMiscForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTend_.c -- */
VOID  PrtItemTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtDispTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);
USHORT   PrtDispTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer); /* 2172 */
VOID	CheckGiftCardPrt(ITEMTENDER  *pItem);

/* -- PrRCP_.c -- */
VOID  PrtCPRMTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtCPRMTender_RJ(ITEMTENDER  *pItem);
VOID  PrtCPRMTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem);

/* -- PrRTax_.c -- */
VOID  PrtItemAffection(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDispAffection(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
USHORT   PrtDispAffectionForm(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrROpen_.c -- */
VOID  PrtItemTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);
VOID  PrtDispTransOpen(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem);
USHORT   PrtDispTransOpenForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTtl_.c -- */
VOID  PrtItemTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDispTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT PrtDispTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRMul_.c -- */
VOID  PrtItemMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDispMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRIMod_.c -- */
VOID  PrtItemModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem);
VOID  PrtModTable_RJ(ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModTable_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModPerson_RJ(ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModPerson_SP(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModCustName_RJ(ITEMMODIFIER *pItem);
VOID  PrtModCustName_SP(TRANINFORMATION *pTran, ITEMMODIFIER *pItem );
VOID  PrtDispModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem);
VOID  PrtDflModTable(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModPerson(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModCustName(TRANINFORMATION *pTran, ITEMMODIFIER *pItem);
USHORT  PrtDispModifierForm(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT  PrtDflModTableForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer); /* 2172 */
USHORT  PrtDflModPersonForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflModCustNameForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer ); /* 2172 */
USHORT PrtDflModTaxForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer ); /* 2172 */
USHORT PrtDflModSeatForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer ); /* 2172*/

/* -- PrRCpn_.c -- */
VOID  PrtItemCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDispCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon   ( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_RJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_VL( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_SP( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
USHORT   PrtDispCouponForm( TRANINFORMATION  *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer ); /* 2172 */
USHORT PrtDflCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer ); /* 2172 */

/* PrRProm_.c */
VOID  PrtPromotion(TRANINFORMATION  *pTran, SHORT fsPrintStatus);
VOID  PrtProm_R(VOID);

/* PrRTril_.c */
VOID  PrtDoubleHeader(TRANINFORMATION  *pTran);
VOID  PrtTrail1(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
VOID  PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
VOID  PrtTrail1_RJ(TRANINFORMATION  *pTran, ULONG  ulStRegNo);
VOID  PrtTrail2(TRANINFORMATION  *pTran);
VOID  PrtTrail3(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
VOID  PrtSoftProm(ITEMPRINT *pItem);
VOID  PrtTrayTrail(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
VOID  PrtChkPaid(ITEMPRINT  *pItem);
VOID  PrtChkPaid_SP(VOID);

/* -- PrRDept_.c -- */
VOID  PrtDept(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtDeptPLU_RJ(TRANINFORMATION  *pTran,ITEMSALES *pItem);
VOID  PrtDeptPLUSet_VL(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDeptPLU_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflDeptPLUForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflPrtModForm(TRANINFORMATION  *pTran, UIFREGSALES *pUifRegSales, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPlu_.c -- */
VOID  PrtPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem);

/* -- PrRSet_.c -- */
VOID  PrtSET(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtSET_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtSETKP_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtSET_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflSETForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRModD_.c -- */
VOID  PrtModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtDeptPLUModDisc_RJ(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtSetModDisc_RJ(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtModDisc_VL(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtModDisc_SP(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtDflModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtDflSetModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflSetModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRIRDi_.c -- */
VOID  PrtItemRegDisc(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_RJ(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_VL(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtDflRegDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT PrtDflRegDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer);

/* -- PrRTip_.c -- */
VOID  PrtChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
VOID  PrtChrgTip_RJ(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtChrgTip_VL(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtChrgTip_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);
ULONG PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
VOID  PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT PrtDflChrgTipForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPO_.c -- */
VOID  PrtPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtPaidOut_RJ(ITEMMISC *pItem);
VOID  PrtPaidOut_VL(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtPaidOut_SP(TRANINFORMATION *pTran,ITEMMISC *pItem);
VOID  PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRROA_.c -- */
VOID  PrtRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtRecvOnAcnt_RJ(ITEMMISC *pItem);
VOID  PrtRecvOnAcnt_VL(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtRecvOnAcnt_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflRecvOnAcntForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */
VOID  PrtMoney(TRANINFORMATION *pTran, ITEMMISC *pItem);/* Saratoga */
VOID  PrtMoney_RJ(ITEMMISC *pItem);                     /* Saratoga */
VOID  PrtMoney_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtGetMoneyMnem(UCHAR uchTendMinor, USHORT *pusTran, UCHAR *puchSym);

/* -- PrRTD_.c -- */
VOID  PrtTipsDecld(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtTipsDecld_RJ(ITEMMISC *pItem);
VOID  PrtTipsDecld_VL(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtTipsDecld_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtDflTipsDecld(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflTipsDecldForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTPO_.c -- */
VOID  PrtTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtTipsPaidOut_RJ(ITEMMISC *pItem);
VOID  PrtTipsPaidOut_VL(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtTipsPaidOut_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflTipsPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRNoSl_.c -- */
VOID  PrtNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtNoSale_RJ(ITEMMISC *pItem);
VOID  PrtNoSale_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflNoSaleForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRChkT_.c -- */
VOID  PrtCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtCheckTran_RJ(ITEMMISC *pItem);
VOID  PrtCheckTran_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);

/* -- PrRCncl_.c -- */
VOID  PrtCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtCancel_RJ(ITEMMISC *pItem);
VOID  PrtCancel_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT PrtDflCancelForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrREc_.c -- */
VOID  PrtErrCorr(ITEMTENDER *pItem);
VOID  PrtErrCorr_J(ITEMTENDER *pItem);

/* -- PrREtk_.c -- */
VOID  PrtEtkIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtEtkIn_RJ(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtEtkIn_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtEtkIn_VL(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtEtkOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);
VOID  PrtEtkOut_RJ(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);
VOID  PrtEtkOut_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);
VOID  PrtEtkOut_VL(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);

/* -- PrRNTnd_.c -- */
VOID  PrtTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtTender_RJ(ITEMTENDER *pItem);
VOID  PrtTender_VL(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem, UCHAR uchType);
VOID  PrtDflTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
USHORT PrtDflTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRFTnd_.c -- */
VOID  PrtForeignTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtForeignTender_RJ(ITEMTENDER *pItem);
VOID  PrtForeignTender_VL(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtForeignTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtDflForeignTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
/* V3.4 */
VOID  PrtEuroTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtEuroTender_RJ(ITEMTENDER *pItem);
VOID  PrtEuroTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
USHORT PrtDflForeignTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTax_.c -- */
VOID  PrtTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);
VOID  PrtDflTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);

/* -- PrRUTax_.c -- */
VOID  PrtUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtUSTax_RJ(ITEMAFFECTION   *pItem);
VOID  PrtUSTax_SP(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
USHORT  PrtDflUSCanTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRCTax_.c -- */
VOID  PrtCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtCanGst(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanGst_RJ(ITEMAFFECTION *pItem);
VOID  PrtCanGst_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd_RJ(ITEMAFFECTION *pItem);
VOID  PrtCanInd_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll_RJ(ITEMAFFECTION *pItem);
VOID  PrtCanAll_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtDflCanGst(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);

/* -- PrRAdd2_.c --*/
VOID  PrtAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);
VOID  PrtAddChk2_RJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtAddChk2_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);

/* -- PrROpen_.c -- */
VOID  PrtNewChk(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtAddChk1(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtCasIn(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtStoreRecall( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem );

/* -- PrRCasI_.c -- */
VOID  PrtCasIn_RJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtCasIn_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);

/* -- PrRNchk_.c -- */
VOID  PrtNewChk_RJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtNewChk_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);

/* -- PrRReod_.c -- */
VOID  PrtReorder_RJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtReorder_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem);
USHORT PrtDflReorderForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRAdd1_.c -- */
VOID  PrtAddChk1_RJ(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtAddChk1_SP(TRANINFORMATION  *pTran, SHORT sWidthType);

/* -- PrRSRec_.c -- */
VOID  PrtStoreRecall_RJ(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtStoreRecall_SP(TRANINFORMATION  *pTran, SHORT sWidthType);

/* -- PrCFeed_.c -- */
VOID  PrtItemOther(ITEMOTHER  *pItem);
VOID  PrtSlpRel(VOID);
VOID  PrtInFeed(ITEMOTHER  *pItem);
VOID  PrtOutFeed(ITEMOTHER  *pItem);
VOID  PrtDispOther(ITEMOTHER  *pItem);
VOID  PrtResetLog(ITEMOTHER *pItem); /* V3.3 */
VOID  PrtPowerDownLog(ITEMOTHER *pItem);    /* Saratoga */

/* -- PrRTotl_.c -- */
VOID  PrtTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_RJ(ITEMTOTAL *pItem);
VOID  PrtTotalKP_RJ(ITEMTOTAL *pItem);
VOID  PrtTotalStub(ITEMTOTAL *pItem);
VOID  PrtTotalOrder(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact_SPVL(ITEMTOTAL *pItem);
VOID  PrtDflTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflForeignTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT  PrtDflTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflForeignTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRSevT_.c -- */
VOID  PrtServTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_RJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_RJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalStubPost(ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_RJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotalStubPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT PrtDflServTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflServTotalStubPostForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTray_.c -- */
VOID  PrtTrayTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_RJ(ITEMTOTAL *pItem);
VOID  PrtDflTrayTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT  PrtDflTrayTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRMulC_.c -- */
VOID  PrtMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulChk_RJ(ITEMMULTI *pItem);
VOID  PrtMulChk_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDflMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMulP_.c -- */
VOID  PrtMultiPost(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulPost_RJ(ITEMMULTI *pItem);
VOID  PrtMulPost_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMlTl_.c -- */
VOID  PrtMultiTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulTotal_RJ(ITEMMULTI *pItem);
VOID  PrtMulTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDflMultiTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMlTP_.c -- */
VOID  PrtMultiPostTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulPostTotal_RJ(ITEMMULTI *pItem);
VOID  PrtMulPostTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRVAT_.c -- */
VOID    PrtVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVATSubtotal_RJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATSubtotal_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVATServ_RJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATServ_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVAT_RJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVAT_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtRJVATAppl(UCHAR uchAdr, ITEMVAT ItemVAT, TRANINFORMATION *pTran);
VOID    PrtRJVATApplShift(UCHAR uchAdr, ITEMVAT ItemVAT, TRANINFORMATION *pTran, USHORT usColumn);
VOID    PrtDflVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtDflVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtDflVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
USHORT  PrtDflVATServForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT  PrtDflVATForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRComn_.c -- */
VOID  PrtSlipInitial(VOID);
VOID  PrtSlpCompul(VOID);
VOID  PrtRectCompul(VOID);
VOID  PrtDouble(TCHAR *pszDest, USHORT   usDestLen, TCHAR *pszSource);
VOID  PrtSDouble(TCHAR *pszDest, USHORT   usDestLen, TCHAR *pszSource); 
VOID  PrtPortion(SHORT fsPrtState);
VOID  PrtPortion2(ITEMSALES *pItem);    /* R3.1 */
VOID  PrtPortion3(SHORT fsPrtState);    /* R3.1 */
VOID  PrtGetVoid(TCHAR *aszVoidMnem, USHORT usStrlen);
SHORT PrtGetReturns(USHORT fbMod, USHORT usReasonCode, TCHAR *pszVoidMnem, USHORT usStrLen);
VOID  PrtGetOffTend(USHORT fbMod, TCHAR *aszOffMnem, USHORT usStrlen);
USHORT  PrtFeed(USHORT  usPrtType, USHORT  usLineNum);
USHORT  PrtCut(VOID);
VOID  PrtElec(VOID);
SHORT PrtChkPort(VOID);
VOID  PrtGetScale(TCHAR *pszMnem, SHORT *psDecPoint, LONG *plWeight,
                                  TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtGetDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran);
VOID  PrtGetJobInDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtGetJobOutDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC *pItem);
SHORT PrtGetTaxMod(TCHAR *pszDest, USHORT usDestLen, SHORT fsTax, USHORT fbMod);
SHORT PrtChkSlipLine(SHORT sPrintLine);
VOID  PrtGet24Mnem(TCHAR  *pszDest, USHORT usAdr);
VOID  PrtGetLead(TCHAR  *pszDest, USHORT usAdr);
TCHAR *PrtSetNumber(TCHAR  *pszDest, TCHAR  *pszNumber);
VOID  PrtSetItem(TCHAR  *pDest, ITEMSALES  *pItem, DCURRENCY *plProduct, SHORT  sType, USHORT usColumn);
USHORT  PrtSetChildPriority( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice, SHORT sType, USHORT usColumn );
USHORT  PrtSetChildNotOrder( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice, SHORT sType, USHORT usColumn );
SHORT PrtGet1Line(TCHAR *pStart, TCHAR **pEnd, USHORT usColumn);
SHORT PrtChkCasWai(TCHAR *pszMnem, ULONG ulCasID, ULONG ulWaiID);
VOID  PrtGetAllProduct(ITEMSALES *pItem, DCURRENCY *plProduct);
VOID  PrtAdjustNative(TCHAR *pszDest, DCURRENCY lAmount);
VOID  PrtFillSpace(TCHAR  *pszString, USHORT  usSize);
USHORT  PrtPrint(USHORT usPrtType, TCHAR *puchBuff, USHORT usLen);
VOID  PrtEndValidation(USHORT  usPort);
VOID  PrtSetRJColumn(VOID);
VOID  PrtPmgWait(VOID);
VOID  PrtSlipPmgWait(VOID);
SHORT PrtChkFullSP(TCHAR  *pszWork);
USHORT  PrtSPVLInit(VOID);
SHORT PrtChkVoidConsolidation(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
SHORT PrtChkPrintPluNo(ITEMSALES *pItem);
SHORT PrtChkPrintType(ITEMSALES *pItem);
VOID  PrtSetQtyDecimal(SHORT *psDecPoint, LONG  *plQty);        /* Saratoga */
VOID PrtAdjustForeign(TCHAR *pszDest, DCURRENCY lAmount, USHORT usAdr, UCHAR uchMDC);
VOID  PrtResetQtyDecimal(SHORT *psDecPoint, LONG  *plQty);
USHORT  PrtGetTaxMod2(TCHAR *pszDest, USHORT usDestLen, UCHAR uchMinorClass); /* 2172 */
SHORT   PrtGetSISymSales( TCHAR  *aszSISym, ITEMSALES *pItem );
SHORT   PrtGetSISymSalesUS( TCHAR  *aszSISym, ITEMSALES *pItem );
SHORT    PrtGetSISymSalesCanada( TCHAR  *aszSISym, ITEMSALES *pItem );
SHORT   PrtGetSISymSalesIntl( TCHAR  *aszSISym, ITEMSALES *pItem );
SHORT   PrtGetSISym( TCHAR  *aszSISym, UCHAR uchOffset1, UCHAR uchOffset2 );
SHORT    PrtGetSISymDisc( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymItemDiscUS( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymItemDiscCanada( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymItemDiscIntl( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymRegDiscUS( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymRegDiscIntl( TCHAR  *aszSISym, ITEMDISC *pItem );
SHORT    PrtGetSISymCoupon( TCHAR  *aszSISym, ITEMCOUPON *pItem );
SHORT    PrtGetSISymCouponUS( TCHAR  *aszSISym, ITEMCOUPON *pItem );
SHORT    PrtGetSISymCouponCanada( TCHAR  *aszSISym, ITEMCOUPON *pItem );
SHORT    PrtGetSISymCouponIntl( TCHAR  *aszSISym, ITEMCOUPON *pItem );
SHORT   PrtCheckTaxSystem(VOID);

/* -- PrRCmnR_.c -- */
VOID  PrtRJVoid(USHORT  fbMod);
VOID  PrtRJOffTend(USHORT  fbMod);
VOID  PrtRJNumber(TCHAR  *pszNumber);
VOID  PrtRJMnemNumber(USHORT usAdr, TCHAR  *pszNumber);
VOID  PrtRJAmtMnem(USHORT usAdr, DCURRENCY lAmount);
VOID  PrtRJZeroAmtMnemP(USHORT usAdr, DCURRENCY lAmount, SHORT sPortion);
VOID  PrtRJAmtSymEuro(USHORT usAdr, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
VOID  PrtRJMnem(USHORT usAdr, BOOL fsType);
VOID  PrtRJTaxMod(USHORT fsTax, USHORT  fbMod);
VOID  PrtRJPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtRJAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtRJAmtSymP(USHORT usAdr, DCURRENCY lAmount, BOOL fsType, SHORT sPortion);
VOID  PrtRJTblPerson(USHORT usTblNo, USHORT usNoPerson, TCHAR uchSeat, SHORT sWidthType);
VOID  PrtRJCustomerName( TCHAR *pszCustomerName );
VOID  PrtRJMultiSeatNo(TRANINFORMATION *pTran);     /* R3.1 */
VOID  PrtRJTranNum(USHORT usAdr, ULONG ulNumber);
VOID  PrtRJKitchen(UCHAR  uchKPNo);
VOID  PrtRJWaiter(ULONG ulWaiID);
VOID  PrtRJForeign1(DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus);
VOID  PrtRJForeign2(ULONG ulRate, UCHAR fbStatus2);
VOID  PrtRJEuro(UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
VOID  PrtRJSeatNo(TCHAR uchSeatNo);
VOID  PrtRJSeatNo2(TCHAR uchSeatNo);
VOID  PrtRJQty(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtRJLinkQty(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtRJWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtRJChild(UCHAR uchAdj, TCHAR *pszMnem);
VOID  PrtRJLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
VOID  PrtRItems(TRANINFORMATION *pTran, ITEMSALES  *pItem); /* R3.1 */
VOID  PrtJItems(ITEMSALES  *pItem); /* R3.1 */
VOID  PrtRJForeignQtyFor(LONG lQTY, DCURRENCY lAmount, UCHAR uchAdr, UCHAR fbStatus);    /* Saratoga */
VOID  PrtRJMoneyForeign(DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus);
VOID  PrtRJQtyFor(LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice);
VOID    PrtSupRJVoid(USHORT fsMod);
VOID  PrtRJPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);


/* VOID  PrtRJItems(ITEMSALES  *pItem); */
VOID  PrtRJTrail2(SHORT sPortion, TRANINFORMATION  *pTran);
VOID  PrtRJHead(VOID);
VOID  PrtRJDoubleHead(VOID);
VOID  PrtRJZeroAmtMnem(UCHAR uchAddress, DCURRENCY lAmount);
VOID  PrtRJGuest(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtRJMulChk(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtRFeed(USHORT usLineNum);
VOID  PrtRJWaiTaxMod(ULONG ulID,  USHORT fsTax, USHORT  fbMod);
VOID  PrtRJMnemCount(USHORT usAdr, SHORT sCount);
VOID  PrtRJTrail1_24(SHORT sPortion, TRANINFORMATION *pTran, ULONG  ulStReg);
VOID  PrtRJTickTrail_24(SHORT sPortion, TRANINFORMATION  *pTran);
VOID  PrtRJTrail1_21(SHORT sPortion, TRANINFORMATION *pTran, ULONG  ulStReg);
VOID  PrtRJTickTrail_21(SHORT sPortion, TRANINFORMATION  *pTran);
VOID  PrtRJModScale(DCURRENCY lAmount);
VOID  PrtRJEtkCode(ITEMMISC *pItem, SHORT sType);
VOID  PrtRJJobTimeIn(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);
VOID  PrtRJJobTimeOut(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtJFolioPost(TCHAR *pszFolioNo, TCHAR *pszPostTranNo);
VOID  PrtRJCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT PrtRJCPRspMsgText(TCHAR *pRspMsgText);
VOID  PrtRJOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
VOID  PrtRJEPTError(ITEMPRINT  *pItem);
VOID  PrtRJCoupon( ITEMCOUPON *pItem );
VOID  PrtRJResetLog(ITEMOTHER *pItem);      /* V3.3 */
VOID  PrtRJResetLog3(ITEMOTHER *pItem);     /* V3.3 */
VOID  PrtRJResetLog4(ITEMOTHER *pItem);     /* V3.3 */
VOID  PrtRJPowerDownLog(ITEMOTHER *pItem);  /* Saratoga */
VOID    PrtRJZAMnemShift(UCHAR uchAddress, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtRJZAMnemPShift(USHORT usAdr, DCURRENCY lAmount, SHORT sPortion, USHORT usColumn);
VOID    PrtRJAmtMnemShift(USHORT usAdr, DCURRENCY lAmount, USHORT usColumn);
VOID  PrtRPLUNo(TCHAR *puchPLUCpn);
VOID  PrtJPLUNo(TCHAR *puchPLUCpn);
VOID    PrtJPLUBuild (USHORT usAdr, TCHAR *puchPLUNo, USHORT usDeptNo);

/* -- PrRCmnVL.c  -- */
VOID  PrtVLHead(TRANINFORMATION *pTran, USHORT fbMod);
VOID  PrtVLHead1(TRANINFORMATION *pTran);
VOID  PrtVLHead2(TRANINFORMATION *pTran);
VOID  PrtVLHead3(USHORT fbMod, USHORT usReasonCode);
VOID  PrtVLTrail(TRANINFORMATION *pTran);
VOID  PrtVLCasTrail(TRANINFORMATION *pTran, ULONG ulStReg);
VOID  PrtVLNumber(TCHAR  *pszNumber);
VOID  PrtVLMnemNumber(USHORT usAdr, TCHAR  *pszNumber);
VOID  PrtVLNumberCP(TCHAR  *pszNumber, TCHAR *auchDate);
VOID  PrtVLAmtMnem(USHORT usAdr, DCURRENCY lAmount);
VOID  PrtVLAmtPerMnem(USHORT usAdr, UCHAR uchRate, SHORT fsTax, USHORT fbMod, DCURRENCY lAmount);
VOID  PrtVLCoupon( ITEMCOUPON *pItem, SHORT fsTax );
VOID  PrtVLNumQty(ITEMSALES *pItem);
VOID  PrtVLNumWeight(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtVLAmtPluMnem(ITEMSALES *pITem, SHORT fsTax);
VOID  PrtVLTender1(TCHAR *pszNumber, DCURRENCY lBalance);
VOID  PrtVLTender2(ITEMTENDER  *pItem);
VOID  PrtVLForeignTend(ITEMTENDER  *pItem);
VOID  PrtVLTipPaidOut(ULONG ulID, DCURRENCY lAmount);
VOID  PrtVLTotal(ITEMTOTAL   *pItem, DCURRENCY lAmount, USHORT usMnemAdr);
VOID  PrtVLAmtSym(USHORT  usMnemAdr, DCURRENCY lAmount, SHORT sType);
VOID  PrtVLChgTip(ULONG ulID, UCHAR uchRate, DCURRENCY lamount, USHORT fsTax, USHORT  fbMod, USHORT uchAdr);
VOID  PrtVLTray(ITEMTOTAL *pItem);
VOID  PrtSetMnemSymAmt(TCHAR  *pszDest, USHORT usDestLen, USHORT usAdr, DCURRENCY lAmount, SHORT sType);
VOID  PrtVLEtkTimeIn(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtVLEtkTimeOut(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);
VOID  PrtVLAccNum(TCHAR *pszNumber);
VOID  PrtVLOffline(USHORT usAdr, ITEMTENDER *pItem);
VOID  PrtVLPLUBuild(USHORT usAdr, TCHAR *puchPLUNo, USHORT usDeptNo);
VOID  PrtVLEndosHead(TCHAR uchPrintFormat);
VOID  PrtVLEndosTrngHead(TCHAR uchPrintFormat);
VOID  PrtVLEndosTrail(TCHAR uchPrintFormat, TRANINFORMATION *pTran);
VOID  PrtVLEndosMnemAmt(TCHAR uchPrintFormat, USHORT usAdr, DCURRENCY lAmount);
VOID  PrtVLEndosMnemCSAmt(TCHAR uchPrintFormat, USHORT usAdr, DCURRENCY lAmount);

/* -- PrRCmnVS.c -- */
VOID  PrtVSDeptPlu(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtVSCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtVSAmtMnem(TRANINFORMATION *pTran, USHORT fbMod, USHORT usAdr, DCURRENCY lAmouut);
VOID  PrtVSTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran);
VOID  PrtVSTotalAudact(ITEMTOTAL *pItem, TRANINFORMATION *pTran);
VOID  PrtVSSevTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran, DCURRENCY lAmount);
VOID  PrtVSSevTotalPost(ITEMTOTAL *pItem, TRANINFORMATION *pTran, SHORT sType);
VOID  PrtVSFCTotal(ITEMTENDER  *pItem, TRANINFORMATION *pTran);
VOID  PrtVSTender(ITEMTENDER *pItem, TRANINFORMATION *pTran);
VOID  PrtVSAmtPerMnem(TRANINFORMATION *pTran, USHORT fbMod, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtVSChgTips(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtVSTipsPO(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtVSGetMDC(TCHAR *pszBuff, TRANINFORMATION *pTran);
VOID  PrtVSGetMode(TCHAR *pszBuff, TRANINFORMATION *pTran, USHORT fbMod);

/* -- PrRCmnS_.c -- */
USHORT  PrtSPVoidNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, TCHAR *pszNumber);
USHORT  PrtSPVoidMnemNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, USHORT usAdr, TCHAR *pszNumber);
USHORT  PrtSPMnemAmt(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPDiscount(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
USHORT  PrtSPTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod);
USHORT  PrtSPMnemTaxAmt(TCHAR *pszWork, USHORT usAdr, USHORT fsTax, USHORT fbMod, DCURRENCY lAmount);
USHORT  PrtSPCoupon( TCHAR *pszDest, TCHAR *pszMnemonic, DCURRENCY lAmount );
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
USHORT  PrtSPCustomerName( TCHAR *pszDest, TCHAR *pszCustomerName );
USHORT  PrtSPMultiSeatNo( TCHAR *pszDest, TRANINFORMATION *pTran);      /* R3.1 */
USHORT  PrtSPChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  PrtSPLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
USHORT  PrtSPTipsPO(TCHAR *pszWork, ULONG ulWaiID, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPGCTransfer(TCHAR *pszWork, ITEMMISC *pItem);
USHORT  PrtSPGstChkNo(TCHAR *pszWork, ITEMMULTI *pItem);
USHORT  PrtSpForeign(TCHAR *pszWork, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
USHORT  PrtSPMnemNativeAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtSPServTaxSum(TCHAR *pszWork,USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtSPChargeTips(TCHAR *pszWork, USHORT usAdr, ULONG ulWaiID, USHORT fsTax, ITEMDISC *pItem);
USHORT  PrtSPCancel(TCHAR *pszWork, USHORT usTranAdr1, USHORT usTranAdr2, DCURRENCY lAmount);
USHORT  PrtSPTrail1(TCHAR *pszWork, TRANINFORMATION  *pTran, USHORT usSlipLine);
USHORT  PrtSPTrail2(TCHAR *pszWork, TRANINFORMATION  *pTran);
USHORT  PrtSPChkPaid(TCHAR *pszWork, USHORT usAdr);
USHORT  PrtSPTrayCount(TCHAR *pszWork, USHORT usAdr, SHORT sTrayCo);
USHORT  PrtSPServiceTotal(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType, TRANINFORMATION  *pTran, USHORT usSlipLine);
USHORT  PrtSPGCTranTrail(TCHAR  *pszWork, ITEMMISC *pItem);
USHORT  PrtSPHeader1(TCHAR *pszWork, TRANINFORMATION  *pTran, SHORT sWidthType);
USHORT  PrtSPHeader2(TCHAR *pszWork, TRANINFORMATION  *pTran, TCHAR uchSeatNo);
USHORT  PrtSPHeader3(TCHAR *pszWork, TRANINFORMATION  *pTran);
USHORT  PrtSPItems(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMSALES  *pItem);
USHORT  PrtCheckLine(USHORT  usSPCurrLine, TRANINFORMATION  *pTran);
USHORT  PrtSPEtkTimeIn(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT  PrtSPEtkTimeOut(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);
USHORT  PrtSPCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT  PrtSPCPOffTend(TCHAR *pszWork, USHORT fbMod);
USHORT  PrtSPCPRspMsgText(TCHAR *pszWork, TCHAR *pszRspMsgText);
USHORT  PrtSPOffline(TCHAR *pszWork, USHORT fbMod, TCHAR *aszDate, TCHAR *aszApproval);
VOID    PrtTrailEPT_SP(TCHAR   *pszWork);
USHORT  PrtSPTranNumber(TCHAR *pszWork, USHORT usTranAdr, TCHAR *pszNumber);
VOID    PrtSPVLTrail(TRANINFORMATION *pTran);
VOID    PrtSPVLHead(TRANINFORMATION *pTran);
VOID    PrtSPVLHead1(TRANINFORMATION *pTran);
VOID    PrtSPVLHead2(TRANINFORMATION *pTran);
VOID    PrtSPVLHead3(VOID);
USHORT  PrtSPMAmtShift(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPTaxRateAmt(TCHAR *pszWork, USHORT usAdr,  ULONG ulRate, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPTaxMnem(TCHAR  *pszWork, ITEMAFFECTION  *pItem);
USHORT  PrtSPTaxAmt(TCHAR  *pszWork, ITEMVAT  ItemVAT);
USHORT  PrtSPAmtSymEuro(TCHAR  *pszWork, USHORT usAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
USHORT  PrtSPEuro(TCHAR  *pszWork, UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
USHORT  PrtSPPLUNo(TCHAR *pszDest, TCHAR *puchPLUCpn);
USHORT  PrtSPPLUBuild (TCHAR *pszDest, USHORT usTranAdr, TCHAR *puchPLUNo, USHORT usDeptNo);
USHORT  PrtSPForeignQtyFor(TCHAR *pszWork, LONG lQTY, DCURRENCY lAmount, UCHAR uchAdr, UCHAR fbStatus);  /* Saratoga */
USHORT  PrtSPMoneyForeign(TCHAR* pszWork, DCURRENCY  lForeign, USHORT usTranAdr1, UCHAR  uchAdr2, UCHAR fbStatus);
USHORT  PrtSPQtyFor(TCHAR *pszWork, LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice);
VOID    PrtSupSPHead(TRANINFORMATION *pTran, VOID *pHeader);
SHORT   PrtSupSPHeadLine(TCHAR *pszWork, VOID *pHeader);
VOID    PrtSupSPTrailer(TRANINFORMATION *pTran, VOID *pTrail);
USHORT  PrtSupSPTrail1(TCHAR *pszWork, VOID *pItem);
USHORT  PrtSupSPTrail2(TCHAR *pszWork, TRANINFORMATION  *pTran, VOID *pItem);
SHORT   PrtSupCompSlip(VOID);
VOID    PrtSupRemoveSlip(VOID);

/* -- PrRKP_.c -- */
VOID   PrtKPInit(VOID);
VOID   PrtKPHeader(TRANINFORMATION *pTran);
VOID   PrtKPCustomerName(TCHAR *pszCustomerName);
VOID   PrtKPTrailer(TRANINFORMATION *pTran, ULONG  ulStReg);
VOID   PrtKPTotal( ITEMTOTAL *pItem );
VOID   PrtKPOperatorName(TRANINFORMATION *pTran);
VOID   PrtKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPSetMenuItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPChild(TCHAR *pszDest, ITEMSALES  *pItem, USHORT usCondNo);
USHORT PrtKPWeight(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPQty(TCHAR *pszDest, USHORT usDestLen, LONG lQty);
USHORT PrtKPSeatNo(TCHAR *pszDest, USHORT usDestLen, ITEMSALES  *pItem);
USHORT PrtKPNoun(TCHAR *pszDest, ITEMSALES  *pItem);
USHORT PrtKPPM(TCHAR *pszDest, ITEMSALES  *pItem);
SHORT  PrtChkKPStorage(TCHAR *pszBuff, UCHAR uchBuffLen, CHAR fbState, CHAR fbState2);
SHORT  PrtSetFailKP(USHORT usKPState);
VOID   PrtChkTotalKP(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
VOID   PrtChkTotalKP2(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
SHORT  PrtChkKPPort(TRANINFORMATION *pTran, ITEMSALES *pItem, UCHAR *puchPort, UCHAR fbType);
SHORT  PrtChkKPPortSub(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPPortSub2(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPEnd(VOID);
VOID   PrtTakeToKP(TRANINFORMATION *pTran, SHORT hsFilePoint);
VOID    PrtKPCheckTarget(ITEMSALES *pItem);                             /* Saratoga */
SHORT   PrtChkKPStorageEx(TCHAR *pszBuff, USHORT usBuffLen, CHAR fbPrtState, CHAR fbState, USHORT usTerm);
SHORT  PrtSendKps(TCHAR *auchSndBuffer, USHORT usSndBufLen,
                  UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,SHORT nKPNo);

/* ===== End of File ( PRTRIN.H ) ===== */
