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
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
}PRTITEMDATA;

typedef struct {
    USHORT  fsComp;
    USHORT  fsNo;
}PRTCONTROL;

typedef struct  {
        UCHAR  uchMajorClass;          /* Major Class Data Definition */
        UCHAR  uchMinorClass;          /* Minor Class Data Definition */
        USHORT usAddress;              /* Address of MDC */
        UCHAR  uchMDCData;             /* Data of MDC */
        WCHAR  aszMDCString[4+1];      /* MDC Strings Data */
        UCHAR  uchStatus;              /* 0=with data, 1=without data */
        USHORT usPrintControl;         /* Print Control */
}PRTPARAMDC;

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
extern UCHAR   fbPrtKPSeatState;     /* save outputed kp number for Seat No, R3.1 */

extern USHORT  usPrtSlipPageLine;    /* current slip lines and pages */
extern USHORT  usPrtTHColumn;        /* thermal's numbers of columns */
extern USHORT  usPrtEJColumn;        /* journal's numbers of columns */
extern USHORT  usPrtVLColumn;        /* validation's numbers of columns */

extern PRTCONTROL PrtCompNo;         /* compulsory print & no print status save area */   

extern SHORT   fsPrtStatSav;         /* save status ( print on demand ) */ 

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

/* -- PrCTran_.c -- */
SHORT PrtChkStorage(SHORT *hsRead1, SHORT *hsRead2, TRANINFORMATION *pTran);
VOID PrtPluTicketList (TRANINFORMATION *pTran, ULONG ulPrintOptions, PluChitInfo *PluChitInfoList, int PluChitInfoListIndex, int PluChitOrderDeclareAdr);
VOID  PrtCallIPD(TRANINFORMATION  *pTran, SHORT hsStorage);
VOID PrtCallIPDTicketInfo ( TRANINFORMATION *pTran, SHORT hsStorage, PluChitInfoAll *PluChit );
VOID PrtPrintItemPrints ( TRANINFORMATION *pTran, SHORT hsStorage );
VOID  PrtKitchenCallIPD(TRANINFORMATION *pTran, SHORT hsStorage);   /* R3.1 */
BOOL  PrtIsItemInWorkBuff( USHORT usItemOffset, UCHAR  *puchWorkBuff, USHORT usBuffOffset, USHORT usBuffSize );
BOOL PrtIsIDiscInWorkBuff( USHORT usItemOffset, UCHAR  *puchWorkBuff, USHORT usBuffSize );
VOID PrtSetSalesCouponQTY(TRANINFORMATION *pTran, ITEMSALES *pItem, SHORT sFileHandle);

/* -- PrRHeadT.c -- */
VOID  PrtItemPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtChkCurSlip(TRANINFORMATION  *pTran);
VOID  PrtTraining(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtTrain_TH(TRANINFORMATION *pTran);
VOID  PrtTrain_EJ(TRANINFORMATION *pTran);
VOID  PrtTrain_SP(TRANINFORMATION *pTran);
VOID  PrtPVoid(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPReturn(TRANINFORMATION *pTran, SHORT fsPrintStatus);
VOID  PrtPVoid_TH(TRANINFORMATION *pTran);
VOID  PrtPVoid_EJ(TRANINFORMATION *pTran);
VOID  PrtPVoid_SP(TRANINFORMATION *pTran);
VOID  PrtMoneyHeader(TRANINFORMATION *pTran, SHORT fsPrintStatus);  /* Saratoga */
VOID  PrtMoneyHeader_TH(TRANINFORMATION *pTran);                    /* Saratoga */
VOID  PrtMoneyHeader_EJ(TRANINFORMATION *pTran);                    /* Saratoga */
VOID  PrtMoneyHeader_SP(TRANINFORMATION *pTran);                    /* Saratoga */
VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtPostRct_TH(TRANINFORMATION *pTran);
VOID  PrtPostRct_EJ(TRANINFORMATION *pTran);
VOID  PrtPostRct_SP(TRANINFORMATION *pTran);
VOID  PrtCPGusLine(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtCPGusLine_TH(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtCPFuncErrCode(ITEMPRINT *pItem);
VOID  PrtCPFuncErrCode_EJ(ITEMPRINT *pItem);
VOID  PrtEPTError(ITEMPRINT  *pItem);
VOID  PrtEPTStub( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
VOID  PrtEPTStubNoAcct( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
VOID  PrtEPTStub_TH( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtDispPrint(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtParkReceipt_TH( TRANINFORMATION *pTran );
VOID  PrtParkReceipt_EJ( TRANINFORMATION *pTran );
VOID  PrtParkReceipt_SP( TRANINFORMATION *pTran );
VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtNumber_TH(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtNumber_EJ(TRANINFORMATION *pTran, ITEMPRINT *pItme);
VOID PrtNumber_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtDflDispNumber( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtAgeLog_EJ(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID PrtDflAgeLog( TRANINFORMATION *pTran, ITEMPRINT *pItem );
VOID  PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtCPRespText_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtTHCPRespText(ITEMPRINT *pItem);
VOID  PrtTHCPRespTextGiftCard(TRANINFORMATION *pTran, ITEMPRINT *pItem);
VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem);        /* Saratoga */
VOID  PrtFSCredit_TH (ITEMPRINT *pItem);                            /* Saratoga */
USHORT  PrtFSCredit_VLS(TRANINFORMATION *pTran, ITEMPRINT *pItem);  /* Saratoga */

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
VOID  PrtItemMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDispMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);


/* -- PrRIModT.c -- */
VOID  PrtItemModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem);
VOID  PrtModTable_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModTable_EJ(ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModTable_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModPerson_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModPerson_EJ(ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModPerson_SP(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtModCustName_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem);
VOID  PrtModCustName_EJ(ITEMMODIFIER *pItem);
VOID  PrtModCustName_SP(TRANINFORMATION *pTran, ITEMMODIFIER *pItem);
VOID  PrtDispModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem);
VOID  PrtDflModTable(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModPerson(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType);
VOID  PrtDflModCustName( TRANINFORMATION *pTran, ITEMMODIFIER *pItem );
USHORT  PrtDispModifierForm(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer);
USHORT  PrtDflModTableForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer);
USHORT  PrtDflModPersonForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer);
USHORT PrtDflModCustNameForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer );
USHORT PrtDflModTaxForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer );
USHORT PrtDflModSeatForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer ); /* 2172*/

/* -- PrRCpnT.c -- */
VOID  PrtItemCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDispCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon   ( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_TH( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_EJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtCoupon_SP( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
VOID  PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
USHORT  PrtDispCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer );
USHORT  PrtDflCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer );

/* PrRPromT.c */
VOID  PrtPromotion(TRANINFORMATION  *pTran, SHORT fsPrintStatus);
VOID  PrtProm_TH(TRANINFORMATION  *pTran);
VOID  PrtTransactionBegin(SHORT sType, ULONG ulTransNum);
VOID  PrtTransactionEnd(SHORT sType, ULONG ulTransNum);

/* PrRTrilT.c */
VOID  PrtDoubleHeader(TRANINFORMATION  *pTran);
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
VOID  PrtDeptPLU_TH(TRANINFORMATION  *pTran,ITEMSALES *pItem);
VOID  PrtDeptPLU_EJ(TRANINFORMATION  *pTran,ITEMSALES *pItem);
VOID  PrtDeptPLU_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflDeptPLUForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflPrtModForm(TRANINFORMATION  *pTran, UIFREGSALES *pUifRegSales, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPluT.c -- */
VOID  PrtPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem); 

/* -- PrRSetT.c -- */
VOID  PrtSET(TRANINFORMATION *pTran, ITEMSALES  *pItem); 
VOID  PrtSET_TH(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtSET_EJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtSETKP_TH(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtSET_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflSETForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRModDT.c -- */
VOID  PrtModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem); 
VOID  PrtDeptPLUModDisc_TH(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtDeptPLUModDisc_EJ(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtSetModDisc_TH(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtSetModDisc_EJ(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtModDisc_SP(TRANINFORMATION *pTran,ITEMSALES *pItem);
VOID  PrtDflModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtDflSetModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT PrtDflModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */
USHORT PrtDflSetModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer); /* 2172 */ 

/* -- PrRIRDiT.c -- */
VOID  PrtItemRegDisc(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_TH(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_EJ(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtItemRegDisc_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtDflRegDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT PrtDflRegDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRTipT.c -- */
VOID  PrtChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);    
VOID  PrtChrgTip_TH(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtChrgTip_EJ(TRANINFORMATION *pTran, ITEMDISC *pItem);
VOID  PrtChrgTip_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);
ULONG PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC  *pItem);    
VOID PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem);
USHORT PrtDflChrgTipForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer); /* 2172 */

/* -- PrRPOT.c -- */
VOID  PrtPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtPaidOut_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtPaidOut_EJ(ITEMMISC *pItem);
VOID  PrtPaidOut_SP(TRANINFORMATION *pTran,ITEMMISC *pItem);
VOID  PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRROAT.c -- */
VOID  PrtRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtRecvOnAcnt_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtRecvOnAcnt_EJ(ITEMMISC *pItem);
VOID  PrtRecvOnAcnt_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtMoney_TH (ITEMMISC *pItem);                                /* Saratoga */
VOID  PrtMoney_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);         /* Saratoga */
VOID  PrtGetMoneyMnem(UCHAR uchTendMinor, USHORT *pusTran, UCHAR *puchSym);
VOID  PrtMoney(TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT PrtDflRecvOnAcntForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);
VOID  PrtMoney_EJ(ITEMMISC *pItem);                                 /* Saratoga */

/* -- PrRTDT.c -- */
VOID  PrtTipsDecld(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtTipsDecld_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtTipsDecld_EJ(ITEMMISC *pItem);
VOID  PrtTipsDecld_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtDflTipsDecld(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflTipsDecldForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRTPOT.c -- */
VOID  PrtTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtTipsPaidOut_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtTipsPaidOut_EJ(ITEMMISC *pItem);
VOID  PrtTipsPaidOut_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflTipsPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRNoSlT.c -- */
VOID  PrtNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtNoSale_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtNoSale_EJ(ITEMMISC *pItem);
VOID  PrtNoSale_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflNoSaleForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrRChkTT.c -- */
VOID  PrtCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtCheckTran_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtCheckTran_EJ(ITEMMISC *pItem);
VOID  PrtCheckTran_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtDflCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    

/* -- PrRCnclT.c -- */
VOID  PrtCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtCancel_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);
VOID  PrtCancel_EJ(ITEMMISC *pItem);
VOID  PrtCancel_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
VOID  PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
USHORT PrtDflCancelForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer);

/* -- PrREcT.c -- */
VOID  PrtErrCorr(ITEMTENDER *pItem);    
VOID  PrtErrCorr_EJ(ITEMTENDER *pItem);      

/* -- PrREtkT.c -- */
VOID  PrtEtkIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtEtkIn_TH(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtEtkIn_EJ(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtEtkIn_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem);    
VOID  PrtEtkOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem,  SHORT sType);    
VOID  PrtEtkOut_TH(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);    
VOID  PrtEtkOut_EJ(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);    
VOID  PrtEtkOut_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);    

/* -- PrRNTndT.c -- */
VOID  PrtTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);      
VOID  PrtTender_TH(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtTender_EJ(ITEMTENDER *pItem);
VOID  PrtTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem, UCHAR uchType);
VOID  PrtDflTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);      
USHORT PrtDflTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer);
SHORT CheckGiftMnem(ITEMTENDER *pItem);

/* -- PrRCPT.c -- */
VOID  PrtCPRMTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);      
VOID  PrtCPRMTender_TH(ITEMTENDER  *pItem);
VOID  PrtCPRMTender_EJ(ITEMTENDER  *pItem);
VOID  PrtCPRMTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem); 

/* -- PrRFTndT.c -- */
VOID  PrtForeignTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtForeignTender_TH(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtForeignTender_EJ(ITEMTENDER *pItem);
VOID  PrtForeignTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtDflForeignTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtEuroTender(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
VOID  PrtEuroTender_TH(TRANINFORMATION *pTran, ITEMTENDER *pItem);
VOID  PrtEuroTender_EJ(ITEMTENDER *pItem);
VOID  PrtEuroTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem);
USHORT PrtDflForeignTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer);

/* -- PrRTaxT.c -- */
VOID  PrtTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);       
VOID  PrtDflTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);       
USHORT PrtDispAffectionForm(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem, TCHAR *puchBuffer);
USHORT PrtDflTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);

/* -- PrRUTaxT.c -- */
VOID  PrtUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtUSTax_TH(TRANINFORMATION *pTran, ITEMAFFECTION   *pItem);
VOID  PrtUSTax_EJ(ITEMAFFECTION   *pItem);
VOID  PrtUSTax_SP(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
USHORT  PrtDflUSCanTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer);

/* -- PrRCTaxT.c -- */
VOID  PrtCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID  PrtCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanGst_TH(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanGst_EJ(ITEMAFFECTION *pItem);
VOID  PrtCanGst_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd_TH(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanInd_EJ(ITEMAFFECTION *pItem);
VOID  PrtCanInd_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll_TH(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtCanAll_EJ(ITEMAFFECTION *pItem);
VOID  PrtCanAll_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);
VOID  PrtDflCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);

/* -- PrRAdd2T.c --*/
VOID  PrtAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);   
VOID  PrtAddChk2_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtAddChk2_EJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtAddChk2_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID  PrtDflAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem);   

/* -- PrROpenT.c -- */
VOID  PrtNewChk(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);    
VOID  PrtReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);   
VOID  PrtAddChk1(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtCasIn(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtStoreRecall( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem );

/* -- PrRCasIT.c -- */
VOID  PrtCasIn_TH(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtCasIn_EJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);
VOID  PrtCasIn_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem);

/* -- PrRNchkT.c -- */
VOID  PrtNewChk_TH(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType);
VOID  PrtNewChk_EJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType);
VOID  PrtNewChk_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType);

/* -- PrRReodT.c -- */
VOID  PrtReorder_TH(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtReorder_EJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtReorder_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType);
VOID  PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem);
USHORT PrtDflReorderForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer);

/* -- PrRAdd1T.c -- */
VOID  PrtAddChk1_TH(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtAddChk1_EJ(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtAddChk1_SP(TRANINFORMATION  *pTran, SHORT sWidthType);

/* -- PrRSRecT.c -- */
VOID  PrtStoreRecall_TH(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtStoreRecall_EJ(TRANINFORMATION  *pTran, SHORT sWidthType);
VOID  PrtStoreRecall_SP(TRANINFORMATION  *pTran, SHORT sWidthType);

/* -- PrCFeed_.c -- */
VOID  PrtItemOther(TRANINFORMATION *pTran, ITEMOTHER  *pItem);
VOID  PrtSlpRel(VOID);                             
VOID  PrtInFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem);                             
VOID  PrtOutFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem);                             
VOID  PrtDispOther(ITEMOTHER  *pItem);
VOID  PrtResetLog(TRANINFORMATION  *pTran, ITEMOTHER *pItem);
VOID  PrtPowerDownLog(TRANINFORMATION *pTran, ITEMOTHER *pItem);    /* Saratoga */

/* -- PrRTotlT.c -- */
VOID  PrtTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalKP_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_EJ(ITEMTOTAL *pItem);
VOID  PrtTotalStub(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalOrder(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTotalAudact_SPVL(ITEMTOTAL *pItem);
VOID  PrtDflTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflForeignTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT  PrtDflTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);
USHORT PrtDflForeignTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrROrdDecT.c -- */
VOID  PrtOrderDec(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtOrderDec_TH(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID  PrtKPOrderDec(TRANINFORMATION *pTran, ITEMSALES *pItem );
VOID  PrtOrderDec_EJ(ITEMSALES *pItem);
VOID  PrtOrderDec_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
SHORT PrtOrderDecSearch(TRANINFORMATION *pTran, ITEMSALES *pItem);

/* -- PrRSevTT.c -- */
VOID  PrtServMaskAcctDate(TCHAR *pAcctNumber, ITEMTOTAL *pItem);
VOID  PrtServTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_EJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_EJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalPost_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtServTotalStubPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_EJ(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtSoftCheckTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtDflServTotalStubPost(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT PrtDflServTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);
USHORT PrtDflServTotalStubPostForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrRTrayT.c -- */
VOID  PrtTrayTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
VOID  PrtTrayTotal_EJ(ITEMTOTAL *pItem);
VOID  PrtDflTrayTotal(TRANINFORMATION *pTran, ITEMTOTAL *pItem);
USHORT  PrtDflTrayTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer);

/* -- PrRMulCT.c -- */
VOID  PrtMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulChk_TH(TRANINFORMATION *pTran, ITEMMULTI *pItem);
VOID  PrtMulChk_EJ(ITEMMULTI *pItem);
VOID  PrtMulChk_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDflMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMulPT.c -- */
VOID  PrtMultiPost(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulPost_TH(TRANINFORMATION *pTran, ITEMMULTI *pItem);
VOID  PrtMulPost_EJ(ITEMMULTI *pItem);
VOID  PrtMulPost_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMlTlT.c -- */
VOID  PrtMultiTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulTotal_TH(TRANINFORMATION *pTran, ITEMMULTI *pItem);
VOID  PrtMulTotal_EJ(ITEMMULTI *pItem);
VOID  PrtMulTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtDflMultiTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRMlTPT.c -- */
VOID  PrtMultiPostTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem);
VOID  PrtMulPostTotal_TH(TRANINFORMATION *pTran, ITEMMULTI *pItem);
VOID  PrtMulPostTotal_EJ(ITEMMULTI *pItem);
VOID  PrtMulPostTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);

/* -- PrRComn_.c -- */
VOID  PrtSlipInitial(VOID);
VOID  PrtSlipInitialLP(VOID);           /* Saratoga */
VOID  PrtSlpCompul(VOID);
VOID  PrtRectCompul(VOID);
VOID  PrtDouble(TCHAR *pszDest, USHORT   usDestLen, TCHAR *pszSource); 
VOID  PrtSDouble(TCHAR *pszDest, USHORT   usDestLen, TCHAR *pszSource); 
VOID  PrtPortion(SHORT fsPrtState);
VOID  PrtPortion2(ITEMSALES *pItem);
VOID  PrtPortion3(SHORT fsPrtState);    /* R3.1 */
VOID  PrtGetVoid(TCHAR *aszVoidMnem, USHORT usStrlen);           
SHORT PrtGetReturns(USHORT fbMod, USHORT usReasonCode, TCHAR *pszVoidMnem, USHORT usStrLen);
VOID  PrtGetOffTend(USHORT fbMod, TCHAR *aszOffMnem, USHORT usStrlen);           
USHORT  PrtFeed(USHORT  usPrtType, USHORT  usLineNum);  
USHORT  PrtLogo(USHORT  usPrtType, USHORT usSize);        
USHORT  PrtCut(VOID);                                           
SHORT PrtChkPort(VOID);                                       
VOID  PrtGetScale(TCHAR *pszMnem, SHORT *psDecPoint, LONG *plWeight, TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtGetDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran);
VOID  PrtGetJobInDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC  *pItem);
VOID  PrtGetJobOutDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC  *pItem);
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
USHORT PrtPrint(USHORT usPrtType, TCHAR *puchBuff, USHORT usLen);
USHORT PrtPrintf(USHORT usPrtType, const TCHAR  *pszFormat, ...);
USHORT PrtAdjustString(TCHAR *pszDest, TCHAR *pszSorce, USHORT usSorceLen, USHORT usColumn);
VOID  PrtPmgWait(VOID);
VOID  PrtSlipPmgWait(VOID);
SHORT PrtChkFullSP(TCHAR  *pszWork);
USHORT  PrtSPVLInit(VOID);
SHORT PrtChkVoidConsolidation(TRANINFORMATION *pTran, ITEMSALES *pItem);
SHORT PrtChkPrintPluNo(ITEMSALES *pItem);
SHORT PrtChkPrintType(ITEMSALES *pItem);
VOID   PrtParaRead(VOID *pDATA);
VOID  PrtSetQtyDecimal(SHORT *psDecPoint, LONG  *plQty);            /* Saratoga */
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

/* -- PrRCmnT_.c -- */
VOID  PrtTHVoid(USHORT  fbMod, USHORT usReasonCode);
VOID  PrtTHOffTend(USHORT  fbMod);
VOID  PrtTHNumber(TCHAR  *pszNumber);
VOID  PrtTHMnemNumber(USHORT usAdr, TCHAR  *pszNumber);
VOID  PrtTHAmtMnem(USHORT usAdr, DCURRENCY lAmount);
VOID  PrtTHAmtTwoMnem(USHORT usAdr1, USHORT usAdr2, DCURRENCY lAmount, TCHAR *pCardLabel);
VOID  PrtTHAmtMnemMnem(USHORT usTranAdr1, USHORT usAdr2, DCURRENCY lAmount);
VOID  PrtTHMnem(USHORT usAdr, BOOL fsType);
VOID  PrtTHAmtSymEuro(USHORT usTranAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
VOID  PrtTHAmtTaxMnem(USHORT usAdr, USHORT fsTax, USHORT  fbMod, DCURRENCY lAmount);
VOID  PrtTHTaxMod(USHORT fsTax, USHORT  fbMod);
VOID  PrtTHPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtTHAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtTHOrderDec(USHORT uchAdr);
VOID  PrtTHTblPerGC(USHORT usTblNo, USHORT usNoPerson, USHORT usGuestNo, UCHAR uchCDV, TCHAR uchSeatNo, SHORT sWidthType);
VOID PrtTHTentName( TCHAR *pszCustomerName );
VOID  PrtTHTblPerson(USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType);
VOID  PrtTHCustomerName(TCHAR *pszCustomerName);
VOID  PrtTHMultiSeatNo(TRANINFORMATION *pTran);
VOID  PrtTHTranNum(USHORT usTranAdr, ULONG ulNumber);
VOID  PrtTHKitchen(UCHAR  uchKPNo);
VOID  PrtTHWaiAmtMnem(ULONG ulWaiID, USHORT usTranAdr, DCURRENCY lAmount);
VOID  PrtTHWaiGC(USHORT usTranAdr, ULONG ulWaiID, USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtTHForeign(DCURRENCY lForeign, UCHAR uchMinorClass, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
VOID  PrtTHEuro(UCHAR uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
VOID  PrtTHSeatNo(TCHAR uchSeatNo);
VOID  PrtTHSeatNo2(TCHAR uchSeatNo);
VOID  PrtTHQty(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtTHLinkQty(TRANINFORMATION  *pTran, ITEMSALES *pItem);
VOID  PrtTHWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtTHChild(UCHAR uchAdj, TCHAR *pszMnem);
VOID  PrtTHLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
VOID  PrtTHItems(TRANINFORMATION *pTran, ITEMSALES  *pItem);
VOID  PrtTHDiscItems(ITEMSALES  *pItem);
SHORT PrtTHHead(TRANINFORMATION  *pTran);
VOID  PrtTHDoubleHead(VOID);
VOID  PrtTHZeroAmtMnem(USHORT usAddress, DCURRENCY lAmount); 
VOID  PrtTHGuest(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtTHMulChk(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtRFeed(USHORT usLineNum);
VOID  PrtTHWaiTaxMod(ULONG ulID,  USHORT fsTax, USHORT  fbMod);
VOID  PrtTHMnemCount(USHORT usAdr, SHORT sCount);
VOID  PrtTHTrail1(TRANINFORMATION *pTran);
VOID  PrtTHTrail2(TRANINFORMATION *pTran, ULONG  ulStRegNo);
VOID  PrtTHTickTrail1(TRANINFORMATION  *pTran);
VOID  PrtTHTickTrail2(TRANINFORMATION  *pTran);
VOID  PrtTHModScale(DCURRENCY lAmount);
VOID  PrtTHEtkCode(ITEMMISC *pItem, SHORT sType);
VOID  PrtTHEtkOut(USHORT usTranAdr1, USHORT usTranAdr2);
VOID  PrtTHJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC *pItem);
VOID  PrtTHJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType);
VOID  PrtTHCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT PrtTHCPRspMsgText(TCHAR *pRspMsgText);
VOID  PrtTHOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
VOID  PrtTHEPTError(ITEMPRINT  *pItem);
VOID  PrtTHCoupon( ITEMCOUPON *pItem );
VOID    PrtTHMAmtShift(USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtTHTaxRateAmt(USHORT usTranAdr,  LONG lRate, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtTHVatMnem(ITEMAFFECTION  *pItem);
VOID    PrtTHVatAmt(ITEMVAT ItemVAT);
VOID    PrtTHPLUNo(TCHAR *puchPLUCpn); /* 2172 */
VOID    PrtTHForeignQtyFor (LONG lQTY, DCURRENCY lAmount, UCHAR uchAdr, UCHAR fbStatus);    /* Saratoga */
VOID    PrtAdjustForeign(TCHAR *pszDest, DCURRENCY lAmount, UCHAR uchAdr, UCHAR uchMDC);
VOID    PrtTHMoneyForeign (DCURRENCY lForeign, USHORT usAdr1, UCHAR uchAdr2, UCHAR fbStatus);
VOID    PrtSupSPHead(TRANINFORMATION *pTran, VOID *pHeader);
SHORT   PrtSupSPHeadLine(TCHAR *pszWork, VOID *pHeader);
VOID    PrtSupSPTrailer(VOID *pTrail);
USHORT  PrtSupSPTrail1(TCHAR *pszWork, VOID *pItem);
USHORT  PrtSupSPTrail2(TCHAR *pszWork, VOID *pItem);
SHORT   PrtSupCompSlip(VOID);
VOID    PrtSupRemoveSlip(VOID);
VOID  PrtTHPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);
VOID  PrtTHItemCount(SHORT noItems, USHORT usPrtType);
VOID  PrtTHExtraCopy(USHORT *usPrintMerch, USHORT usPrtType);
VOID  PrtTHTipTotal(USHORT  usPrtType);
VOID  PrtTHAmtGiftMnem(ITEMTENDER  *pItem);

/* -- PrRCmnJ_.c -- */
VOID  PrtEJVoid(USHORT  fbMod, USHORT usReasonCode);
VOID  PrtEJOffTend(USHORT  fbMod);
VOID  PrtEJNumber(TCHAR  *pszNumber);
VOID  PrtEJMnemNumber(USHORT usAdr, TCHAR  *pszNumber);
VOID  PrtEJAmtMnem(USHORT usAdr, DCURRENCY lAmount);
VOID  PrtEJMnem(USHORT usAdr, BOOL fsType);
VOID  PrtEJTaxMod(USHORT fsTax, USHORT  fbMod);
VOID  PrtEJPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
VOID  PrtEJAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtEJOrderDec(USHORT uchAdr, DCURRENCY lAmount, BOOL fsType);
VOID  PrtEJAmtSymEuro(USHORT usTranAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
VOID  PrtEJTblPerson(USHORT usTblNo, USHORT usNoPerson, TCHAR uchSeatNo, SHORT sWidthType);
VOID  PrtEJCustomerName(TCHAR *pszCustomerName);
VOID  PrtEJMultiSeatNo(TRANINFORMATION *pTran);
VOID  PrtEJTranNum(USHORT usTranAdr, ULONG ulNumber);
VOID  PrtEJKitchen(UCHAR  uchKPNo);
VOID  PrtEJWaiter(ULONG usWaiID);
VOID  PrtEJForeign1(DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus);
VOID  PrtEJForeign2(ULONG ulRate, UCHAR fbStatus2);
VOID  PrtEJEuro(UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
VOID  PrtEJSeatNo(TCHAR uchSeatNo);
VOID  PrtEJQty(ITEMSALES  *pItem);
VOID  PrtEJLinkQty(ITEMSALES  *pItem);
VOID  PrtEJWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem);
VOID  PrtEJChild(UCHAR uchAdj, TCHAR *pszMnem);
VOID  PrtEJLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
VOID  PrtEJItems(ITEMSALES  *pItem);
VOID  PrtEJDiscItems(ITEMSALES  *pItem);
VOID  PrtEJZeroAmtMnem(USHORT usAddress, DCURRENCY lAmount); 
VOID  PrtEJGuest(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtEJMulChk(USHORT usGuestNo, UCHAR uchCDV);
VOID  PrtEJWaiTaxMod(ULONG ulID,  USHORT fsTax, USHORT  fbMod);
VOID  PrtEJMnemCount(USHORT usTranAdr, SHORT sCount);
VOID  PrtEJTrail1(TRANINFORMATION *pTran, ULONG  ulStRegNo);
VOID  PrtEJTrail2(TRANINFORMATION  *pTran);
VOID  PrtEJInit(TRANINFORMATION  *pTran);
VOID  PrtForceEJInit(TRANINFORMATION  *pTran);
VOID  PrtEJWrite(VOID  *pWrite, USHORT usWriteLen);
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
VOID    PrtEJZAMnemShift(USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn);
VOID    PrtEJAmtMnemShift(USHORT usTranAdr, DCURRENCY lAmout, USHORT usColumn);
VOID  PrtEJPLUNo(TCHAR *puchPLUCpn); /* 2172 */
VOID    PrtEJPLUBuild(USHORT usTranAdr, TCHAR *puchPLUNo, USHORT usDeptNo); /* 2172 */
VOID    PrtEJMoneyForeign (DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus);
VOID  PrtEJPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);
VOID  PrtEJAmtAndMnemonic (ITEMTENDER  *pItem);

/* -- PrRCmnS_.c -- */
USHORT  PrtSPVoidNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, TCHAR *pszNumber);
USHORT  PrtSPVoidMnemNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, USHORT usAdr, TCHAR *pszNumber);
USHORT  PrtSPMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount);
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
USHORT  PrtSPCustomerName( TCHAR *pszDest, TCHAR *pszCustomerName );
USHORT  PrtSPMultiSeatNo( TCHAR *pszDest, TRANINFORMATION *pTran);
USHORT  PrtSPChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  PrtSPLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
USHORT  PrtSPTipsPO(TCHAR *pszWork, ULONG ulWaiID, USHORT usTranAdr, DCURRENCY lAmount);
USHORT  PrtSPGCTransfer(TCHAR *pszWork, ITEMMISC *pItem);
USHORT  PrtSPGstChkNo(TCHAR *pszWork, ITEMMULTI *pItem);
USHORT  PrtSpForeign(TCHAR *pszWork, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
USHORT  PrtSPMnemNativeAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtSPServTaxSum(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
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
USHORT  PrtSPDiscItems(TCHAR  *pszWork, ITEMSALES  *pItem);
USHORT  PrtCheckLine(USHORT  usSPCurrLine, TRANINFORMATION  *pTran);
USHORT  PrtSPEtkTimeIn(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem);
USHORT  PrtSPEtkTimeOut(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);
USHORT  PrtSPCPRoomCharge(TCHAR  *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT  PrtSPCPOffTend(TCHAR *pszWork, USHORT fbMod);
USHORT  PrtSPCPRspMsgText(TCHAR *pszWork, TCHAR *pszRspMsgText);
VOID    PrtSPVLTrail(TRANINFORMATION *pTran);
VOID    PrtSPVLHead(TRANINFORMATION *pTran);
VOID    PrtSPVLHead1(TRANINFORMATION *pTran);
VOID    PrtSPVLHead2(TRANINFORMATION *pTran);
VOID    PrtSPVLHead3(VOID);
USHORT  PrtSPOffline(TCHAR *pszWork, USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
VOID    PrtTrailEPT_SP( TCHAR   *pszWork );
USHORT  PrtSPTranNumber(TCHAR *pszWork, USHORT usTranAdr, TCHAR *pszNumber);
USHORT  PrtSPCoupon( TCHAR *pszDest, TCHAR *pszMnemonic, DCURRENCY lAmount );
USHORT  PrtSPTaxMnem(TCHAR  *pszWork, ITEMAFFECTION  *pItem);
USHORT  PrtSPTaxAmt(TCHAR  *pszWork, ITEMVAT  ItemVAT);
USHORT  PrtSPMAmtShift(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPTaxRateAmt(TCHAR *pszWork, USHORT usAdr,  ULONG ulRate, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtSPAmtSymEuro(TCHAR  *pszWork, USHORT usAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus); /* V3.4 */
USHORT  PrtSPEuro(TCHAR  *pszWork, UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2); /* V3.4 */
USHORT  PrtSPPLUNo(TCHAR *pszDest, TCHAR *puchPLUCpn); /* 2172 */
USHORT  PrtSPPLUBuild (TCHAR *pszDest, USHORT usTranAdr, TCHAR *puchPLUNo, USHORT usDeptNo);
USHORT  PrtSPForeignQtyFor(TCHAR *pszWork, LONG lQTY, DCURRENCY lAmount, UCHAR uchAdr, UCHAR   fbStatus);    /* Saratoga */
USHORT PrtSPMoneyForeign(TCHAR* pszWork, DCURRENCY  lForeign, USHORT usTranAdr1, UCHAR  uchAdr2, UCHAR fbStatus);
USHORT  PrtSPQtyFor(TCHAR *pszWork, LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice);
USHORT  PrtSPDiscountSISym(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC);
USHORT  PrtSPCouponSISym( TCHAR *pszDest, TCHAR *pszMnemonic, DCURRENCY lAmount, TCHAR *pszSISym, TCHAR uchMDC);

/* -- PrRKP_.c -- */
USHORT PrtKPNumber(TCHAR *pszDest, USHORT usDestLen,TCHAR  *pszNumber);
VOID   PrtKPInit(VOID);
VOID   PrtKPHeader(TRANINFORMATION *pTran);
VOID   PrtKPCustomerName(TCHAR *pszCustomerName);
VOID   PrtKPTrailer(TRANINFORMATION *pTran, ULONG  ulStReg);
VOID   PrtKPTotal( ITEMTOTAL *pItem );
VOID   PrtKPOperatorName(TRANINFORMATION *pTran);
VOID   PrtKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPSetMenuItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPChild(TCHAR *pszDest, ITEMSALES  *pItem, USHORT usCondNo);
USHORT PrtKPSeatNo(TCHAR *pszDest, USHORT usDestLen, ITEMSALES  *pItem);
USHORT PrtKPWeight(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMSALES *pItem);
USHORT PrtKPQty(TCHAR *pszDest, USHORT usDestLen, LONG lQty);
USHORT PrtKPNoun(TCHAR *pszDest, ITEMSALES  *pItem);
USHORT PrtKPPM(TCHAR *pszDest, ITEMSALES  *pItem);
SHORT  PrtChkKPStorage(TCHAR *pszBuff, UCHAR uchBuffLen, CHAR fbState, CHAR fbState2);
SHORT  PrtSetFailKP(USHORT usKPState);
VOID   PrtChkTotalKP(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
VOID   PrtChkTotalKP2(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType);
SHORT  PrtChkKPPort(TRANINFORMATION *pTran, ITEMSALES *pItem, UCHAR *puchPort, UCHAR fbType);
SHORT  PrtChkKPPortSub(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPPortSub2(UCHAR *puchPortion,  UCHAR  fbPrtState, TRANINFORMATION  *pTran, UCHAR fbType);
USHORT PrtChkKPPortToUse (UCHAR  *auchPluStatus, TRANINFORMATION  *pTran, UCHAR fbType);
SHORT  PrtChkKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID   PrtKPEnd(VOID);
VOID   PrtTakeToKP(TRANINFORMATION *pTran, SHORT hsStorage);
VOID   PrtTakeToKPSub(TRANINFORMATION *pTran, SHORT hsStorage, USHORT usKPNo);
SHORT   PrtChkKPStorageEx(TCHAR *pszBuff, USHORT usBuffLen, CHAR fbPrtState, CHAR fbState, USHORT usTerm);
VOID    PrtKPCheckTarget(ITEMSALES *pItem);

/* ### ADD 2172 Rel1.0 (Saratoga) */
SHORT   PrtSendKps(TCHAR *auchSndBuffer, USHORT usSndBufLen, UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,SHORT nKPNo);


/* -- PrRVATT.c -- */
VOID    PrtItemVATService(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVATSubtotal_TH(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATSubtotal_EJ(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATSubtotal_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVATServ_TH(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATServ_EJ(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
VOID    PrtVATServ_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem);
VOID    PrtVAT_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVAT_EJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
VOID    PrtVAT_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
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
