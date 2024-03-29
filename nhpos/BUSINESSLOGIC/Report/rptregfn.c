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
* Title       : Register Financial Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTREGFN.C
* --------------------------------------------------------------------------
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
*       2015  -> GenPOS Rel 2.3.0 (EMV for Electronic Payment)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               RptRegFinDayRead()  :  Call Function RptRegFinRead
*               RptRegFinPTDRead()  :  Call Function RptRegFinRead
*               RptRegFinRead()     :  Get Reg. Fin. Total, print header & trailer
*               RptRegFinEdit()     :  Calculate Total Report of Reg. Fin. File
*
*               RptRegFinPrtTermNumber() : Print Terminal Number
*
*               RptIndFinDayRead()  :  Call Function RptIndFinRead
*               RptIndFinRead()     :  Get Reg. Fin. Total, print header & trailer
*               RptIndFinDayReset() :  Call Function RptIndFinIndReset/RptIndFinAllReset
*               RptIndFinIndReset() :  Reset Individulal Financial File
*               RptIndFinAllReset() :  Call Function RptIndFinAllOnlyReset/
*                                      RptIndFinAllRstReport
*               RptIndFinAllOnlyReset() : Reset Individual File
*               RptIndFinAllRstReport() : Reset & Report Individual File
*               RptIndFinLock()     :  Execute Terminal Lock
*               RptIndFinUnLock()   :  Execute Terminal Unlock
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name     : Description
* Apr-08-94 : 00.00.04 : Yoshiko.J  : delete Offline element, change tender
* May-20-94 : 02.05.00 : Yoshiko.J  : Q-011 corr. (mod. subtotal for MiscTend#6-#8) 
* Feb-29-96 : 03.01.00 : M.Ozwa     : Provide Individual Terminal Read/Reset Report
* Aug-11-99 : 03.05.00 : M.Teraki   : remove WAITER_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Jan-22-00 : 01.00.00 : hrkato     : Saratoga
* GenPOS
* Feb-12-19 : 02.02.02 : R.Chambers : handle discount subtotal similar to Cashier Report.
* Feb-19-19 : 02.02.02 : R.Chambers : print Surcharges report for Item Discount key #3 - #6.
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
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <cscas.h>
#include <csstbcas.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"


static D13DIGITS CONST NULL13DIGITS = 0;


/*
*=============================================================================
**  Synopsis: SHORT RptRegFinDayRead(UCHAR uchMinorClass, UCHAR uchType) 
*               
*       Input:  uchMinorClass
*               uchType 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Read Financial Daily report.   
*===============================================================================
*/

SHORT RptRegFinDayRead(UCHAR uchMinorClass, UCHAR uchType) 
{
    /* execute Current Daily Report */
    return(RptRegFinRead(uchMinorClass, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptRegFinPTDRead(UCHAR uchMinorClass, UCHAR uchType) 
*               
*       Input:  uchMinorClass
*               uchType 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Read Financial PTD report.
*===============================================================================
*/

SHORT RptRegFinPTDRead(UCHAR uchMinorClass, UCHAR uchType) 
{
    /* execute Current PTD Report */
    return(RptRegFinRead(uchMinorClass, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptRegFinRead(UCHAR uchMinorClass,UCHAR uchType) 
*               
*       Input:  uchMinorClass 
*               uchType 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Read Financial report.
*       Prepare for Header and current time.
*       Print out Header and Trailer.
*===============================================================================
*/


SHORT RptRegFinRead(UCHAR uchMinorClass, UCHAR uchType) 
{
    UCHAR       uchSpecMnemo,
                uchRptType,
                uchACNo,
                uchRptName;
    SHORT       sReturn;
	TTLREGFIN   TtlData = {0};                    /* Assign Register Financial Total Save Area */

    /* EOD PTD Report Not Display Out MLD */
    if((uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY)){
        uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
    }
    
    RptPrtStatusSet(uchMinorClass);                         /* Set J|R print status */

    /* Get Register Financial Total */
    TtlData.uchMajorClass = CLASS_TTLREGFIN;                /* Set Major Data Class */
    TtlData.uchMinorClass = uchMinorClass;                  /* Set Minor Data Class */

    if ((sReturn = SerTtlTotalRead(&TtlData)) != SUCCESS) { /* Read Failure */
        return( TtlConvertError(sReturn) );                 /* Return Not in File Error */
    }

    if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD))  {
		TtlGetNdate (&TtlData.ToDate);
    }
    if (RPT_MEDIA_READ == uchType) {            /* All or Flash report ? */
        uchRptName = RPT_FLASH_ADR;             /* Set media flash address */
        uchACNo = AC_MEDIA_RPT;                 /* Set media flash A/C No. */
    } else {
        uchRptName = RPT_FINANC_ADR;            /* Set financial address */
        uchACNo = AC_REGFIN_READ_RPT;           /* Set financial A/C No. */
    }

    /* Select Special Mnemonics for Header */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */        
        break;
  
    case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;

    case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */
        break;

    case CLASS_TTLSAVPTD :                                  /* Case of PTD RESET */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;

    default:
        ;
    }
    /* Print Header Name */
    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {      /* Not EOD Function */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    } 
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    uchRptName,                             /* Report Name Address */
                    uchSpecMnemo,                           /* Special Mnemonics Address */
                    uchRptType,                             /* Report Name Address */
                    0,                                      /* Not Print */
                    uchACNo,                                /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */                                       
    

    /* Edit Total Report */
    TtlData.usTerminalNumber = 0;
    if ((sReturn = RptRegFinEdit(RptElement, &TtlData, uchType, 0)) == RPT_ABORTED) {   /* Aborded By User */
        MaintMakeAbortKey();                                /* Print ABORDED */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
        return(sReturn);
    }

    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {      /* Not EOD Function */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
    } else {
        RptFeed(RPT_DEFALTFEED);                                         /* Feed            */
    }

    if (uchMinorClass == CLASS_TTLSAVDAY || uchMinorClass == CLASS_TTLSAVPTD) {        
        SerTtlIssuedReset(&TtlData, TTL_NOTRESET);
    }
    
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptRegFinEdit(TTLREGFIN *pTtlData, UCHAR uchType)
*               
*       Input:  pTtlData 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Set the data for an element of print format each time.
*       Print each element if particular MDC Bit is on. 
*       (It doesn't check Total is 0)
*       This function is just same shape of printing format.
*===============================================================================
*/

SHORT RptRegFinEdit(RptElementFunc RptElement, TTLREGFIN *pTtlData, UCHAR uchType, UCHAR uchIndAll)
{
    UCHAR  uchNo;                 /* store any Number      */
    UCHAR  uchMinor, fchFS, uchBonusIndex;
    USHORT usMDCData, usMDCData2, /* store 2-BYTE of MDC BIT */
           usReturnLen;
	D13DIGITS  lSubTotal;             /* store Sub Total       */
	D13DIGITS  lSubTotalDiscounts;    /* Discounts and coupons Sub Total       */
    
    PARACURRENCYTBL FCTbl = {0};       
    PARADISCTBL     Data = {0};
	struct {
		SHORT  sMdcAddrTtl;    // indicates if discount key is a Discount or a Surcharge
		SHORT  sBitTtl;
		SHORT  sMdcAddrRpt;    // indicates if discount is to appear on report or not
		SHORT  sBitRpt;
		SHORT  sTrnMnem;       // transaction mnemonic identifier for report mnemonic
	}  MdcListItemDiscount [] = {   // uses CliParaMDCCheckField(), index same as pTtlData->ItemDiscExtra.TtlIAmount[i]
		{MDC_MODID33_ADR, MDC_PARAM_BIT_D, MDC_RPTFIN45_ADR, MDC_PARAM_BIT_D, TRN_ITMDISC_ADR_3},   // Item Discount #3, pTtlData->ItemDiscExtra.TtlIAmount[0]
		{MDC_MODID43_ADR, MDC_PARAM_BIT_D, MDC_RPTFIN45_ADR, MDC_PARAM_BIT_C, TRN_ITMDISC_ADR_4},   // Item Discount #4, pTtlData->ItemDiscExtra.TtlIAmount[1]
		{MDC_MODID53_ADR, MDC_PARAM_BIT_D, MDC_RPTFIN45_ADR, MDC_PARAM_BIT_B, TRN_ITMDISC_ADR_5},   // Item Discount #5, pTtlData->ItemDiscExtra.TtlIAmount[2]
		{MDC_MODID63_ADR, MDC_PARAM_BIT_D, MDC_RPTFIN45_ADR, MDC_PARAM_BIT_A, TRN_ITMDISC_ADR_6},   // Item Discount #6, pTtlData->ItemDiscExtra.TtlIAmount[3]
		{0, 0, 0, 0}
	}, MdcListTransDiscount [] = {    // uses CliParaMDCCheck(), index same as for pTtlData->RegDisc[i]
		{MDC_RDISC13_ADR, ODD_MDC_BIT0, MDC_RPTFIN3_ADR, ODD_MDC_BIT2, TRN_RDISC1_ADR},      // Transaction Discount #1, pTtlData->RegDisc[0]
		{MDC_RDISC23_ADR, ODD_MDC_BIT0, MDC_RPTFIN3_ADR, ODD_MDC_BIT3, TRN_RDISC2_ADR},      // Transaction Discount #2, pTtlData->RegDisc[1]
		{MDC_RDISC33_ADR, ODD_MDC_BIT0, MDC_RPTFIN29_ADR, ODD_MDC_BIT1, TRN_RDISC3_ADR},     // Transaction Discount #3, pTtlData->RegDisc[2]
		{MDC_RDISC43_ADR, ODD_MDC_BIT0, MDC_RPTFIN29_ADR, ODD_MDC_BIT2, TRN_RDISC4_ADR},     // Transaction Discount #4, pTtlData->RegDisc[3]
		{MDC_RDISC53_ADR, ODD_MDC_BIT0, MDC_RPTFIN29_ADR, ODD_MDC_BIT3, TRN_RDISC5_ADR},     // Transaction Discount #5, pTtlData->RegDisc[4]
		{MDC_RDISC63_ADR, ODD_MDC_BIT0, MDC_RPTFIN30_ADR, EVEN_MDC_BIT0, TRN_RDISC6_ADR},    // Transaction Discount #6, pTtlData->RegDisc[5]
		{0, 0, 0, 0}
	};

    if (! RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
        RptRegFinPrtTermNumber(pTtlData->usTerminalNumber, uchIndAll);       /* TERMINAL NO         */

        RptPrtTime(TRN_PFROM_ADR, &pTtlData->FromDate);                     /* PERIOD FROM         */
        RptPrtTime(TRN_PTO_ADR, &pTtlData->ToDate);                         /* PERIOD TO           */  
        if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
            return(RPT_ABORTED);
        } 
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        RptFeed(RPT_DEFALTFEED);                                            /* Feed                */
    }

    lSubTotal = 0L;
    if (RPT_MEDIA_READ != uchType) {        /*----- All or Flash report ? -----*/
        if (!CliParaMDCCheck(MDC_RPTFIN1_ADR, ODD_MDC_BIT0)) {          /* Check MDC#101-BIT0  */
            RptElement(TRN_CURGGT_ADR, NULL, pTtlData->CGGTotal,        /* CURRENT GROSS       */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);                       /*  GROUP TL           */
        }
        if (!CliParaMDCCheck(MDC_RPTFIN1_ADR, ODD_MDC_BIT1)) {          /* Check MDC#101-BIT1  */
            RptElement(TRN_NETTTL_ADR, NULL, NULL13DIGITS,              /* NET SALES TL        */
                   pTtlData->lNetTotal, CLASS_RPTREGFIN_PRTDGG,0);     
        }
        if (!CliParaMDCCheck(MDC_RPTFIN2_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#102-BIT2  */
            RptFeed(RPT_DEFALTFEED);                                    /* Feed                */
        }                                                               /*                     */

		// Daily Gross Total section to show Daily Gross Total and the various
		// corrections to arrive at the Daily Net Total
        RptElement(TRN_DAIGGT_ADR, NULL, NULL13DIGITS,                  /* DAILY GROSS         */
               pTtlData->lDGGtotal, CLASS_RPTREGFIN_PRTDGG,0);          /* GROUP TL            */ 
        lSubTotal = pTtlData->lDGGtotal;                      

		// remove out the following types of voids and returns (values are negative so add them)
        if (!CliParaMDCCheck(MDC_RPTFIN1_ADR, ODD_MDC_BIT2)) {          /* Check MDC#101-BIT2  */
            RptElement(TRN_TRNGGT_ADR, NULL, NULL13DIGITS,              /* TRAINING GROSS TL   */
                   pTtlData->lTGGTotal, CLASS_RPTREGFIN_PRTDGG,0);     
        }
        lSubTotal += pTtlData->lTGGTotal;

        if (!CliParaMDCCheck(MDC_RPTFIN1_ADR, ODD_MDC_BIT3)) {           /* Check MDC#101-BIT3 */
            RptElement(TRN_PLUSVD_ADR, &pTtlData->PlusVoid, NULL13DIGITS, /* PLUS VOID CO/TL   */
                   0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        }
        lSubTotal += pTtlData->PlusVoid.lAmount;

        if (!CliParaMDCCheck(MDC_RPTFIN2_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#102-BIT0  */
            RptElement(TRN_PSELVD_ADR, &pTtlData->PreselectVoid,        /* PRE-SELECT VOID     */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */
        }
        lSubTotal += pTtlData->PreselectVoid.lAmount;

        if (!CliParaMDCCheckField(MDC_RPTFIN46_ADR, MDC_PARAM_BIT_D)) { /* Check MDC#102-BIT0  */
            RptElement(TRN_TRETURN1_ADR, &pTtlData->TransactionReturn,    /* transaction return  */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */
        }
        lSubTotal += pTtlData->TransactionReturn.lAmount;

        if (!CliParaMDCCheckField(MDC_RPTFIN46_ADR, MDC_PARAM_BIT_C)) { /* Check MDC#102-BIT0  */
            RptElement(TRN_TRETURN2_ADR, &pTtlData->TransactionExchng,    /* transaction return  */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */
        }
        lSubTotal += pTtlData->TransactionExchng.lAmount;

        if (!CliParaMDCCheckField(MDC_RPTFIN46_ADR, MDC_PARAM_BIT_B)) { /* Check MDC#102-BIT0  */
            RptElement(TRN_TRETURN3_ADR, &pTtlData->TransactionRefund,    /* transaction return  */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */
        }
        lSubTotal += pTtlData->TransactionRefund.lAmount;

        if (!CliParaMDCCheck(MDC_RPTFIN2_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#102-BIT1  */
            RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                  /* SUB TOTAL           */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?          */
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        if (!CliParaMDCCheck(MDC_RPTFIN2_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#102-BIT2  */
            RptFeed(RPT_DEFALTFEED);                                    /* Feed                */
        }                                                               /*                     */

		// remove out the following types of coupons and discounts (values are negative so add them)
		lSubTotalDiscounts = 0;
        if (!CliParaMDCCheck(MDC_RPTFIN2_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#102-BIT3  */
            RptElement(TRN_CONSCPN_ADR, &pTtlData->ConsCoupon,          /* COSOLIDATION COUPON */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */  
			lSubTotalDiscounts += pTtlData->ConsCoupon.lAmount;
        }
        lSubTotal += pTtlData->ConsCoupon.lAmount;

        if (!(CliParaMDCCheck(MDC_MODID13_ADR, ODD_MDC_BIT0))) {    /* Check MDC#33-BIT0   */
			// * Feb-12-2019 : 02.02.02 : R.Chambers : handle discount subtotal similar to Cashier Report.
			// if discount key is an Item Discount and not an Item Surcharge key
			// add to subtotal regardless of whether printed on report similar to Cashier Report.
            lSubTotal += pTtlData->ItemDisc.lAmount;
			if (!CliParaMDCCheck(MDC_RPTFIN3_ADR, ODD_MDC_BIT0)) {          /* Check MDC#103-BIT0  */
                RptElement(TRN_ITMDISC_ADR, &pTtlData->ItemDisc,        /* ITEM DISCOUNT CO/TL */
                      NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
				lSubTotalDiscounts += pTtlData->ItemDisc.lAmount;
            }
        }
        if (!(CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0))) {    /* Check MDC#37-BIT0   */
			// if discount key is an Item Discount and not an Item Surcharge key
			// add to subtotal regardless of whether printed on report similar to Cashier Report.
            lSubTotal += pTtlData->ModiDisc.lAmount;
			if (!CliParaMDCCheck(MDC_RPTFIN3_ADR, ODD_MDC_BIT1)) {          /* Check MDC#103-BIT1  */
                RptElement(TRN_MODID_ADR, &pTtlData->ModiDisc,          /* MODIFIED ITEM DISC. */
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);  /*  CO/TL              */  
				lSubTotalDiscounts += pTtlData->ModiDisc.lAmount;
            }
        }

		{
			int i = 0;
			for (i = 0; MdcListItemDiscount[i].sMdcAddrRpt > 0; i++) {
				if (!(CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrTtl, MdcListItemDiscount[i].sBitTtl))) {
					// * Feb-12-2019 : 02.02.02 : R.Chambers : handle discount subtotal similar to Cashier Report.
					// if discount key is an Item Discount and not an Item Surcharge key
					// add to subtotal regardless of whether printed on report similar to Cashier Report.
					lSubTotal += pTtlData->ItemDiscExtra.TtlIAmount[i].lAmount;
					if (!CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrRpt, MdcListItemDiscount[i].sBitRpt)) {
						RptElement(MdcListItemDiscount[i].sTrnMnem, &pTtlData->ItemDiscExtra.TtlIAmount[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
						lSubTotalDiscounts += pTtlData->ItemDiscExtra.TtlIAmount[i].lAmount;
					}
				}
			}
		}

		{
			int i;
			for (i = 0; MdcListTransDiscount[i].sMdcAddrTtl > 0; i++) {
				if (!(CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrTtl, MdcListTransDiscount[i].sBitTtl))) {
					// if discount key is an Transaction Discount and not a Transaction Surcharge key
					// add to subtotal regardless of whether printed on report similar to Cashier Report.
					lSubTotal += pTtlData->RegDisc[i].lAmount;
					if (!CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrRpt, MdcListTransDiscount[i].sBitRpt)) {
						RptElement(MdcListTransDiscount[i].sTrnMnem, &pTtlData->RegDisc[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
						lSubTotalDiscounts += pTtlData->RegDisc[i].lAmount;
					}
				}
			}
		}


        if (!CliParaMDCCheck(MDC_RPTFIN29_ADR, ODD_MDC_BIT0)) {          /* Check MDC#103-BIT3  */
            RptElement(TRN_COMBCPN_ADR, &pTtlData->Coupon,      /* Combination Coupon */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
			lSubTotalDiscounts += pTtlData->Coupon.lAmount;
        }
        lSubTotal += pTtlData->Coupon.lAmount;

        if (!CliParaMDCCheck(MDC_RPTFIN35_ADR, EVEN_MDC_BIT0)) {          /* Check MDC#103-BIT3  */
            RptElement(TRN_VCPN_ADR, &pTtlData->aUPCCoupon[0],            /* UPC Coupon 2172 */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
			lSubTotalDiscounts += pTtlData->aUPCCoupon[0].lAmount;
        }
        lSubTotal += pTtlData->aUPCCoupon[0].lAmount;

        if (!CliParaMDCCheck(MDC_RPTFIN4_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#104-BIT0  */
            RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                  /* SUB TOTAL           */ 
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?          */
            return(RPT_ABORTED);
        }                                                               /*                     */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        if (!CliParaMDCCheck(MDC_RPTFIN4_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#104-BIT1  */
            RptFeed(RPT_DEFALTFEED);                                    /* Feed                */ 
        }                                                               /*                     */

        if (!CliParaMDCCheck(MDC_RPTFIN4_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#104-BIT2  */
            RptElement(TRN_PO_ADR, &pTtlData->PaidOut,                  /* PAID OUT CO/TL      */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
            lSubTotal += pTtlData->PaidOut.lAmount;
        }
        if (!CliParaMDCCheck(MDC_RPTFIN4_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#104-BIT3  */
            RptElement(TRN_TIPPO_ADR, &pTtlData->TipsPaid,              /* TIPS PAID OUT CO/TL */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
            lSubTotal += pTtlData->TipsPaid.lAmount;
        }
        if (!CliParaMDCCheck(MDC_RPTFIN5_ADR, ODD_MDC_BIT0)) {          /* Check MDC#105-BIT0  */
            RptElement(TRN_RA_ADR, &pTtlData->RecvAcount,               /* RECEIVED ON ACCOUNT */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);      /*  CO/TL              */
            lSubTotal += pTtlData->RecvAcount.lAmount;                          
        }
        if (!CliParaMDCCheck(MDC_RPTFIN5_ADR, ODD_MDC_BIT1)) {          /* Check MDC#105-BIT1  */
            RptElement(TRN_CHRGTIP_ADR, &pTtlData->ChargeTips[0],       /* CHARGE TIPS CO/TL   */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlData->ChargeTips[0].lAmount;
        }
        if (!CliParaMDCCheck(MDC_RPTFIN30_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#130-BIT1  */
            RptElement(TRN_CHRGTIP2_ADR, &pTtlData->ChargeTips[1],       /* CHARGE TIPS 2 CO/TL, V3.3   */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlData->ChargeTips[1].lAmount;
        }
        if (!CliParaMDCCheck(MDC_RPTFIN30_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#130-BIT2  */
            RptElement(TRN_CHRGTIP3_ADR, &pTtlData->ChargeTips[2],       /* CHARGE TIPS 3 CO/TL, V3.3   */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlData->ChargeTips[2].lAmount;
        }
        if (!(CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1))) {      /* Check MDC#27-BIT2   */
			// * Feb-12-2019 : 02.02.02 : R.Chambers : handle discount subtotal similar to Cashier Report.
            lSubTotal += pTtlData->HashDepartment.lAmount; 
			if (!CliParaMDCCheck(MDC_RPTFIN22_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#122-BIT1  */
                RptElement(TRN_HASHDEPT_ADR, &pTtlData->HashDepartment, /* HASH DEPARTMENT     */
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);  /*  CO/TL              */
            }
        }
        if (!CliParaMDCCheck(MDC_RPTFIN34_ADR, ODD_MDC_BIT0)) {         /* Check MDC#357-BIT0  */
            RptElement(TRN_FSCRD_ADR, &pTtlData->FoodStampCredit,       /* FS Credit, Saratoga */
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);  /*  CO/TL              */
            lSubTotal += pTtlData->FoodStampCredit.lAmount; 
        }
        if (!CliParaMDCCheck(MDC_RPTFIN5_ADR, ODD_MDC_BIT2)) {          /* Check MDC#105-BIT2  */
            RptElement(TRN_TTLR_ADR, NULL, lSubTotal,                   /* TOTAL               */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?          */
            return(RPT_ABORTED);
        }                                                               /*                     */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        if (!CliParaMDCCheck(MDC_RPTFIN5_ADR, ODD_MDC_BIT3)) {          /* Check MDC#105-BIT3  */
            RptFeed(RPT_DEFALTFEED);                                    /* Feed                */
        }                                                               
    }                                                                  

    lSubTotal = 0L;
    if (!CliParaMDCCheck(MDC_RPTFIN6_ADR, EVEN_MDC_BIT0)) {             /* Check MDC#106-BIT0  */
        RptElement(TRN_TEND1_ADR, &pTtlData->CashTender.OnlineTotal,    /* CASH CO/TL          */ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->CashTender.OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN26_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#126-BIT1  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->CashTender.OfflineTotal, /* CASH CO/TL(offline) */ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->CashTender.OfflineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN6_ADR, EVEN_MDC_BIT1)) {             /* Check MDC#106-BIT1  */
        RptElement(TRN_TEND2_ADR, &pTtlData->CheckTender.OnlineTotal,   /* CHECK CO/TL         */ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->CheckTender.OnlineTotal.lAmount; 
    }
    if (!CliParaMDCCheck(MDC_RPTFIN26_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#126-BIT2  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->CheckTender.OfflineTotal,/* CHECK CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->CheckTender.OfflineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN6_ADR, EVEN_MDC_BIT2)) {             /* Check MDC#106-BIT2  */
        RptElement(TRN_TEND3_ADR, &pTtlData->ChargeTender.OnlineTotal,  /* CHARGE CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->ChargeTender.OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN26_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#126-BIT3  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->ChargeTender.OfflineTotal,/* CHARGE CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->ChargeTender.OfflineTotal.lAmount;
    }

    /*----- Get 2-BYTE from Address of MDC #105 to Address of MDC #108 -----*/
    /*----- Get 2-BYTE from Address of MDC #125 to Address of MDC #128 -----*/
    usMDCData = 0;
    usMDCData2 = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN5_ADR - 1)/2, &usReturnLen);
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData2), 2, (MDC_RPTFIN27_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 7;                                /* Shift right 7-bit    */

    for (uchNo = 0; uchNo < 5; uchNo++) {           /* MISC.#1 to #5 CO/TL  */
        if (UieReadAbortKey() == UIE_ENABLE) {      /* if Abort ?           */
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        if (!(usMDCData & 0x01)) {                               
            RptElement(TRN_TEND4_ADR + uchNo, &pTtlData->MiscTender[uchNo].OnlineTotal,   
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);    
            lSubTotal += pTtlData->MiscTender[uchNo].OnlineTotal.lAmount;
        }
        if (!(usMDCData2 & 0x01)) {                               
            RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[uchNo].OfflineTotal,   
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);    
            lSubTotal += pTtlData->MiscTender[uchNo].OfflineTotal.lAmount;
        }
        usMDCData >>= 1;                            /* Shift right 1 bit    */
        usMDCData2 >>= 1;                           /* Shift right 1 bit    */
    } /*----- End of for Loop -----*/

    /*----- Get 2-BYTE from Address of MDC #125 to Address of MDC #126 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN25_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 2;                                /* Shift right 1 bit    */
    
    for (uchNo = 0; uchNo < 3; uchNo++) {           /* MISC.#6 to #8 CO/TL  */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_TEND9_ADR + uchNo, &pTtlData->MiscTender[uchNo+5].OnlineTotal,
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlData->MiscTender[uchNo + 5].OnlineTotal.lAmount;
                                                    /* Q-011 corr. '94 5/20 */
        }
        if (!(usMDCData2 & 0x01)) {                               
            RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[uchNo+5].OfflineTotal,     
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);    
            lSubTotal += pTtlData->MiscTender[uchNo+5].OfflineTotal.lAmount;
        }
        usMDCData >>= 1;                            /* Shift right 1 bit    */
        usMDCData2 >>= 1;                           /* Shift right 1 bit in */
    }


	//Tender 12
    if (!CliParaMDCCheck(MDC_RPTFIN39_ADR, ODD_MDC_BIT2)) {             /* Check MDC#435-BIT2  */
        RptElement(TRN_TENDER12_ADR, &pTtlData->MiscTender[8].OnlineTotal,  /* Tender 12 (Misc 9) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[8].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN35_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#358-BIT3  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[8].OfflineTotal,/* Tender 12 (Misc 9) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[8].OfflineTotal.lAmount;
    }
	//Tender 13
    if (!CliParaMDCCheck(MDC_RPTFIN39_ADR, ODD_MDC_BIT3)) {             /* Check MDC#435-BIT3(A)  */
        RptElement(TRN_TENDER13_ADR, &pTtlData->MiscTender[9].OnlineTotal,  /* Tender 13 (Misc 10) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[9].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN35_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#358-BIT2(B)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[9].OfflineTotal,/* Tender 13 (Misc 10) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[9].OfflineTotal.lAmount;
    }
	//Tender 14
    if (!CliParaMDCCheck(MDC_RPTFIN40_ADR, EVEN_MDC_BIT0)) {             /* Check MDC#440-BIT0(D)  */
        RptElement(TRN_TENDER14_ADR, &pTtlData->MiscTender[10].OnlineTotal,  /* Tender 14 (Misc 11) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[10].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN35_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#358-BIT2(A)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[10].OfflineTotal,/* Tender 14 (Misc 11) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[10].OfflineTotal.lAmount;
    }
	//Tender 15
    if (!CliParaMDCCheck(MDC_RPTFIN40_ADR, EVEN_MDC_BIT1)) {             /* Check MDC#440-BIT1(C)  */
        RptElement(TRN_TENDER15_ADR, &pTtlData->MiscTender[11].OnlineTotal,  /* Tender 15 (Misc 12) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[11].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN36_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#430-BIT0(D)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[11].OfflineTotal,/* Tender 15 (Misc 12) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[11].OfflineTotal.lAmount;
    }
	//Tender 16
    if (!CliParaMDCCheck(MDC_RPTFIN40_ADR, EVEN_MDC_BIT2)) {             /* Check MDC#440-BIT2(B)  */
        RptElement(TRN_TENDER16_ADR, &pTtlData->MiscTender[12].OnlineTotal,  /* Tender 16 (Misc 13) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[12].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN36_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#430-BIT1(C)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[12].OfflineTotal,/* Tender 16 (Misc 13) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[12].OfflineTotal.lAmount;
    }
	//Tender 17
    if (!CliParaMDCCheck(MDC_RPTFIN40_ADR, EVEN_MDC_BIT3)) {             /* Check MDC#440-BIT2(A)  */
        RptElement(TRN_TENDER17_ADR, &pTtlData->MiscTender[13].OnlineTotal,  /* Tender 17 (Misc 14) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[13].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN36_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#430-BIT2(B)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[13].OfflineTotal,/* Tender 17 (Misc 14) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[13].OfflineTotal.lAmount;
    }
	//Tender 18
    if (!CliParaMDCCheck(MDC_RPTFIN41_ADR, ODD_MDC_BIT0)) {             /* Check MDC#441-BIT0(D)  */
        RptElement(TRN_TENDER18_ADR, &pTtlData->MiscTender[14].OnlineTotal,  /* Tender 18 (Misc 15) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[14].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN36_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#430-BIT2(A)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[14].OfflineTotal,/* Tender 18 (Misc 15) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[14].OfflineTotal.lAmount;
    }
	//Tender 19
    if (!CliParaMDCCheck(MDC_RPTFIN41_ADR, ODD_MDC_BIT1)) {             /* Check MDC#441-BIT1(C)  */
        RptElement(TRN_TENDER19_ADR, &pTtlData->MiscTender[15].OnlineTotal,  /* Tender 19 (Misc 16) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[15].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN37_ADR, ODD_MDC_BIT0)) {            /* Check MDC#430-BIT3(A)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[15].OfflineTotal,/* Tender 19 (Misc 16) CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[15].OfflineTotal.lAmount;
    }
	//Tender 20
    if (!CliParaMDCCheck(MDC_RPTFIN41_ADR, ODD_MDC_BIT2)) {             /* Check MDC#441-BIT2(B)  */
        RptElement(TRN_TENDER20_ADR, &pTtlData->MiscTender[16].OnlineTotal,  /* Tender 20 (Misc 17) CO/TL        */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[16].OnlineTotal.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN37_ADR, ODD_MDC_BIT1)) {            /* Check MDC#430-BIT2(A)  */
        RptElement(TRN_OFFTEND_ADR, &pTtlData->MiscTender[16].OfflineTotal,/* Tender 20 (Misc 17 CO/TL(offline)*/ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlData->MiscTender[16].OfflineTotal.lAmount;
    }
    if (RPT_MEDIA_READ != uchType) {        /*----- All or Flash report ? -----*/
        if (!CliParaMDCCheck(MDC_RPTFIN8_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#108-BIT0  */
            RptElement(TRN_STTLR_ADR, NULL, lSubTotal, 0L, CLASS_RPTREGFIN_PRTCGG,0);   /* SUB TOTAL       */
        }
        if (!CliParaMDCCheck(MDC_RPTFIN8_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#108-BIT1  */
            RptFeed(RPT_DEFALTFEED);                                    /* Feed                */
        }
    }
                                                                    
    if (!CliParaMDCCheck(MDC_RPTFIN8_ADR, EVEN_MDC_BIT2)) {             /* Check MDC#108-BIT2  */
		DCURRENCY lOtherTtlTemp = 0;

        RptElement(TRN_FT1_ADR, NULL, NULL13DIGITS,                     /* FOREIGN CURRENCY 1  */
                  (LONG)pTtlData->ForeignTotal[0].sCounter,             /*  CO/TL              */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                          /* Print 1st-line      */ 
        uchMinor = RptFCMDCChk(MDC_FC1_ADR);                            /* Check Format Type   */
        RptElement(0, NULL, NULL13DIGITS,                               /* Print 2nd-line      */
                   pTtlData->ForeignTotal[0].lAmount, uchMinor, 0);     
        FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                   
        FCTbl.uchAddress = CNV_NO1_ADR;                                 /* Set Minor for Ttl   */
        CliParaRead(&FCTbl);                                            /* Get Total           */
        RptFCRateCalc2(CNV_NO1_ADR, &lOtherTtlTemp, pTtlData->ForeignTotal[0].lAmount,     /* Calculate FC        */
                     FCTbl.ulForeignCurrency, RND_FOREIGN1_1_ADR);
        lSubTotal += lOtherTtlTemp;                                         /* Add to Sub Total    */
        RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,               /* Print 3rd-line      */ 
                   lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG, 0);                                       
    }
    if (!CliParaMDCCheck(MDC_RPTFIN8_ADR, EVEN_MDC_BIT3)) {             /* Check MDC#108-BIT3  */
		DCURRENCY lOtherTtlTemp = 0;

        RptElement(TRN_FT2_ADR, NULL, NULL13DIGITS,                     /* FOREIGN CURRENCY 2  */
                  (LONG)pTtlData->ForeignTotal[1].sCounter,             /*  CO/TL   */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                          /* Print 1st-line      */ 
        uchMinor = RptFCMDCChk(MDC_FC2_ADR);                            /* Check Format Type   */
        RptElement(0, NULL, NULL13DIGITS,                               /* Print 2nd-line      */
                   pTtlData->ForeignTotal[1].lAmount, uchMinor, 0);                                    
        FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                    /* Set Major for Ttl   */
        FCTbl.uchAddress = CNV_NO2_ADR;                                 /* Set Minor for Ttl   */
        CliParaRead(&FCTbl);                                            /* Get Total           */
        RptFCRateCalc2(CNV_NO2_ADR, &lOtherTtlTemp, pTtlData->ForeignTotal[1].lAmount,     /* Calculate FC        */
                     FCTbl.ulForeignCurrency, RND_FOREIGN2_2_ADR);
        lSubTotal += lOtherTtlTemp;                                         /* Add to Sub Total    */
        RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,               /* Print 3rd-line      */
                   lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG, 0);                                 
    }

    /* --- Saratoga --- */
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, (UCHAR *)&usMDCData, 2, (MDC_RPTFIN32_ADR - 1)/2, &usReturnLen);
    usMDCData >>= 2;
    for (uchNo = 0; uchNo < 6; uchNo++) {
        if (! (usMDCData & 0x01)) {
			DCURRENCY lOtherTtlTemp = 0;

            RptElement(TRN_FT3_ADR + uchNo, NULL, NULL13DIGITS,/* FOREIGN CURRENCY 3-8 */
                  (LONG)pTtlData->ForeignTotal[uchNo+2].sCounter,       /*  CO/TL     */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                          /* Print 1st-line */ 
            uchMinor = RptFCMDCChk((USHORT)(MDC_FC3_ADR + uchNo));      /* Check Format Type  */
            RptElement(0, NULL, NULL13DIGITS,                           /* Print 2nd-line    */
                   pTtlData->ForeignTotal[uchNo+2].lAmount, uchMinor,0);
            FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                /* Set Major for Ttl */
            FCTbl.uchAddress = CNV_NO3_ADR + uchNo;                     /* Set Minor for Ttl */
            CliParaRead(&FCTbl);                                        /* Get Total         */
            RptFCRateCalc2((UCHAR)(CNV_NO3_ADR + uchNo), &lOtherTtlTemp, 
                pTtlData->ForeignTotal[uchNo+2].lAmount, FCTbl.ulForeignCurrency,
                (UCHAR)(uchNo * 2 + RND_FOREIGN3_2_ADR));
            lSubTotal += lOtherTtlTemp;                                     /* Add to Sub Total  */
            RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,              /* Print 3rd-line    */
               lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG, 0);                   
        }
        usMDCData >>= 1;                        /* Shift right 2 bit each time */
    }
	
	//total Consistency check
	if((lSubTotal - pTtlData->CGGTotal) == 1){
		lSubTotal = lSubTotal - (lSubTotal - pTtlData->CGGTotal);
	}

    /*------------------------------------------*/
    /*  No-Assume Tender Affects to Total       */
    /*  Print out No-Assume Tender              */
    /*------------------------------------------*/

    /*----- Get 2-BYTE from Address of MDC #109 to Address of MDC #112 -----*/      /* To adjust the offset in MDC Address area*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN9_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 2;                            /* Shift right 2 bit */

    for (uchNo = 0; uchNo < 6; uchNo++) {                               /* TOTAL#3 TO #8 CO/TL */
        if (!(usMDCData & 0x01) && RptNoAssume(uchNo)) {
            RptElement(TRN_TTL3_ADR + uchNo, &pTtlData->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlData->ServiceTotal[uchNo].lAmount;
        }
        usMDCData >>= 1;                        /* Shift right 1 bit in each time */
    } /*----- End of for Loop -----*/

    if (!CliParaMDCCheck(MDC_RPTFIN9_ADR, ODD_MDC_BIT0)) {                                    /* Check MDC#109-BIT0  */
        RptElement(TRN_TTLR_ADR, NULL, lSubTotal, 0L, CLASS_RPTREGFIN_PRTCGG,0);    /* TOTAL               */                                     
    }
    if (RPT_MEDIA_READ == uchType) {            /* All or Flash report ? */
        if (uchRptMldAbortStatus) {                                     /* aborted by MLD */
            return (RPT_ABORTED);
        }
        return(SUCCESS);
    }

    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);                                                  
    }                                                 
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN9_ADR, ODD_MDC_BIT1)) {              /* Check MDC#109-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }

    /*------------------------------------------*/
    /* Not print No-Assume Tender               */  
    /*------------------------------------------*/

	// We map Total Key #3 through Total Key #8 to the Service Total Array elements 0 thru 5.
	// We map Total Key #10 through Total Key #20 to the Service Total Array elements 6 thru 16.
	// Total #1 is Cash, Total #2 and Total #9 are both food stamp.  None of these have service totals.
	{
		struct {
			USHORT  usTranAdr;
			USHORT  usMDC;
			UCHAR   uchBit;
		} reportList [] = {
			{0, 0, 0},         // legacy total #3 ignored
			{0, 0, 0},         // legacy total #4 ignored
			{0, 0, 0},         // legacy total #5 ignored
			{0, 0, 0},         // legacy total #6 ignored
			{0, 0, 0},         // legacy total #7 ignored
			{0, 0, 0},         // legacy total #8 ignored
			{TRN_TOTAL10_ADR, MDC_RPTFIN31_ADR, EVEN_MDC_BIT1},       // new total #10 begins our list
			{TRN_TOTAL11_ADR, MDC_RPTFIN31_ADR, EVEN_MDC_BIT2},       // new total #11
			{TRN_TOTAL12_ADR, MDC_RPTFIN31_ADR, EVEN_MDC_BIT3},       // new total #12
			{TRN_TOTAL13_ADR, MDC_RPTFIN37_ADR, ODD_MDC_BIT2},        // new total #13
			{TRN_TOTAL14_ADR, MDC_RPTFIN37_ADR, ODD_MDC_BIT3},        // new total #14
			{TRN_TOTAL15_ADR, MDC_RPTFIN38_ADR, EVEN_MDC_BIT0},       // new total #15
			{TRN_TOTAL16_ADR, MDC_RPTFIN38_ADR, EVEN_MDC_BIT1},       // new total #16
			{TRN_TOTAL17_ADR, MDC_RPTFIN38_ADR, EVEN_MDC_BIT2},       // new total #17
			{TRN_TOTAL18_ADR, MDC_RPTFIN38_ADR, EVEN_MDC_BIT3},       // new total #18
			{TRN_TOTAL19_ADR, MDC_RPTFIN39_ADR, ODD_MDC_BIT0},        // new total #19
			{TRN_TOTAL20_ADR, MDC_RPTFIN39_ADR, ODD_MDC_BIT1}         // new total #20
		};
		TOTAL           Totalcount[3] = {0};  /* assign for Service Tl/Co */

    /*----- Get 2-BYTE from Address of MDC #109 to Address of MDC #112 -----*/      /* To adjust the offset in MDC Address area*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN9_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 2;                            /* Shift right 2 bit */

 	// We map Total Key #3 through Total Key #8 to the Service Total Array elements 0 thru 5.
	// We map Total Key #10 through Total Key #20 to the Service Total Array elements 6 thru 16.
	// Total #1 is Cash, Total #2 and Total #9 are both food stamp.  None of these have service totals.

	for (uchNo = 0; uchNo < 6; uchNo++) {                               /* TOTAL#3 TO #8 CO/TL */
        if (!(usMDCData & 0x01) && !RptNoAssume(uchNo)) {
            RptElement(TRN_TTL3_ADR + uchNo, &pTtlData->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            RptSvscalc(uchNo, &pTtlData->ServiceTotal[uchNo], Totalcount);
        }
        usMDCData >>= 1;                        /* Shift right 1 bit in each time */
    } /*----- End of for Loop -----*/

	/* The Following totals were created by JHH and modified by PDINU */
	for ( ; uchNo < 17; uchNo++) {
	    if (!CliParaMDCCheck(reportList[uchNo].usMDC, reportList[uchNo].uchBit)) {             
			RptElement(reportList[uchNo].usTranAdr, &pTtlData->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
			lSubTotal += pTtlData->ServiceTotal[uchNo].lAmount;
			RptSvscalc(uchNo, &pTtlData->ServiceTotal[uchNo], Totalcount);
		}
	}

    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN11_ADR, ODD_MDC_BIT0)) {             /* Check MDC#111-BIT0  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                                   /*                     */

    /*----- Get 2-BYTE from Address of MDC #111 to Address of MDC #114 -----*/
    usMDCData = 0;
	/* Need to adjust the offset in MDC Address area*/
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN11_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 1;                                /* Shift right 1 bit */

    lSubTotal = 0L;
    for (uchNo = 0; uchNo < 3; uchNo++) {                               /* SERVICE TOTAL #1 TO #3 */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_SERV1_ADR + uchNo, &Totalcount[uchNo],
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += Totalcount[uchNo].lAmount;
        }
        usMDCData >>= 1;                            /* Shift right 1 bit in each time */
    } /*----- End of for Loop -----*/

    if (!CliParaMDCCheck(MDC_RPTFIN12_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#112-BIT0  */
        RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                      /* SUB TOTAL           */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN12_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#112-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                 

	}  // End of scope for array Totalcount[]

	//----------------------------------------------------------------------------------------------
	{
		DCURRENCY lOtherTtlTemp = 0;

		/*----- Get 2-BYTE from Address of MDC #111 to Address of MDC #114 -----*/
		usMDCData = 0;
		CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN11_ADR - 1)/2, &usReturnLen);
		usMDCData >>= 6;                                /* Shift right 6 bit */

		lOtherTtlTemp = 0L;
		for (uchNo = 0; uchNo < 3; uchNo++) {                               /* ADD CHECK #1 TO #3 CO/TL */
			if (!(usMDCData & 0x01)) {
				RptElement((TRN_ADCK1_ADR + uchNo), &pTtlData->AddCheckTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
				lOtherTtlTemp += pTtlData->AddCheckTotal[uchNo].lAmount;
			}
			usMDCData >>= 1;                            /* Shift right 1 bit in each time */
		} /*----- End of for Loop -----*/
		if (!CliParaMDCCheck(MDC_RPTFIN13_ADR, ODD_MDC_BIT1)) {             /* Check MDC#113-BIT1  */
			RptElement(TRN_STTLR_ADR, NULL, lOtherTtlTemp, 0, CLASS_RPTREGFIN_PRTCGG, 0);
		}
		if (!CliParaMDCCheck(MDC_RPTFIN13_ADR, ODD_MDC_BIT2)) {             /* Check MDC#113-BIT2  */
			RptElement(TRN_OTSTN_ADR, NULL, lSubTotal - lOtherTtlTemp,      /* OUTSTANDING TOTAL   */
					   0L, CLASS_RPTREGFIN_PRTCGG, 0);
		}
		if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
			return(RPT_ABORTED);
		}                                                                   /*                     */
		if(RptPauseCheck() == RPT_ABORTED){
			 return(RPT_ABORTED);
		}

		if (!CliParaMDCCheck(MDC_RPTFIN13_ADR, ODD_MDC_BIT3)) {             /* Check MDC#113-BIT3  */
			RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
		}                                                                   /*                     */
	}
	//----------------------------------------------------------------------------------------------

    lSubTotal = 0L;
    if (!CliParaMDCCheck(MDC_RPTFIN14_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#114-BIT0  */
        RptElement(TRN_CHRGTIP_ADR, &pTtlData->ChargeTips[0],           /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlData->ChargeTips[0].lAmount;
    }
    /* V3.3 */
    if (!CliParaMDCCheck(MDC_RPTFIN30_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#130-BIT3  */
        RptElement(TRN_CHRGTIP2_ADR, &pTtlData->ChargeTips[1],          /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlData->ChargeTips[1].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN31_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#274-BIT0  */
        RptElement(TRN_CHRGTIP3_ADR, &pTtlData->ChargeTips[2],          /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlData->ChargeTips[2].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN14_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#114-BIT1  */
        RptElement(TRN_TIPPO_ADR, &pTtlData->TipsPaid,                  /* TIPS PAID OUT CO/TL */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlData->TipsPaid.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTFIN14_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#114-BIT2  */
        RptElement(TRN_OTSTN_ADR, NULL, lSubTotal,                      /* OUTSTANDING TOTAL   */ 
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN14_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#114-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                                   /*                     */
	//----------------------------------------------------------------------------------------------

    if (!CliParaMDCCheck(MDC_RPTFIN15_ADR, ODD_MDC_BIT0)) {             /* Check MDC#115-BIT0  */
        RptElement(TRN_DECTIP_ADR, &pTtlData->DeclaredTips,             /* DECLARED TIPS CO/TL */ 
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN15_ADR, ODD_MDC_BIT1)) {             /* Check MDC#115-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                                   /*                     */

    /* VAT report, V3.3 */
    if ((CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) && (CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0))) {
        /* VAT #n APL   SZZZZZZZ9.99    * VAT #n APLLICABLE TOTAL
           VAT #n       SZZZZZZZ9.99    * VAT #n TOTAL            
                             .
                             .
                            X 3                                   */

        if (RptRegFinVAT(pTtlData) == RPT_ABORTED) {
            return (RPT_ABORTED);
        }
        /* Service Total of VAT, V3.3 */
        if (!CliParaMDCCheck(MDC_RPTFIN18_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#118-BIT0  */
            RptElement(TRN_TX4_ADR, NULL, NULL13DIGITS,                 /* TAX #4 TL           */
                       pTtlData->Taxable[3].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
        }
    } else {
        /* US Tax Report */
        /*----- Get 2-BYTE from Address of MDC #115 to Address of MDC #118 -----*/
        usMDCData = 0;
        CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN15_ADR - 1)/2, &usReturnLen);
        usMDCData >>= 2;                                /* Shift right 2 bit */
        fchFS = ODD_MDC_BIT1;

        for (uchNo = 0; uchNo < 3; uchNo++) {                           
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TXBL1_ADR + 3 * uchNo, NULL,            /* Taxable #1 to #3 TL */    
                           pTtlData->Taxable[uchNo].mlTaxableAmount, 0, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            usMDCData >>= 1;                            /* Shift right 1 bit in each time */
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TX1_ADR + 3 * uchNo, NULL,              /* TAX #1 to #3 TL     */
                           NULL13DIGITS, pTtlData->Taxable[uchNo].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            usMDCData >>= 1;                            /* Shift right 1 bit in each time */
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TXEXM1_ADR + 3 * uchNo, NULL,           /* TAX EXEMPTED #1 to #3 TL */
                           NULL13DIGITS, pTtlData->Taxable[uchNo].lTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTFIN34_ADR, fchFS)) {                /* Check MDC#357-BIT2-4  */
                RptElement(TRN_FSTXEXM1_ADR + uchNo, NULL,         /* FSTAX EXEMPTED #1 to #3 TL */
                   NULL13DIGITS, pTtlData->Taxable[uchNo].lFSTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            usMDCData >>= 1;                            /* Shift right 1 bit in each time */
            fchFS <<= 1;
            if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?          */
               return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED){
                return(RPT_ABORTED);
            }
        } /*----- End of for Loop -----*/

        /* ---- Canadian Tax R3.1 ---- */
        if (CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) {                 /* Check MDC#-BIT0     */
            if (!CliParaMDCCheck(MDC_RPTFIN17_ADR, ODD_MDC_BIT3)) {         /* Check MDC#117-BIT3  */
                RptElement(TRN_TXBL4_ADR, NULL,                             /* Taxable #4 TL       */
                           pTtlData->Taxable[3].mlTaxableAmount, 0, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTFIN18_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#118-BIT0  */
                RptElement(TRN_TX4_ADR, NULL, NULL13DIGITS,                 /* TAX #4 TL           */
                           pTtlData->Taxable[3].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTFIN18_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#118-BIT1  */
                RptElement(TRN_TXEXM4_ADR, NULL, NULL13DIGITS,              /* TAX EXEMPTED #4 TL  */ 
                           pTtlData->Taxable[3].lTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTFIN18_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#118-BIT2  */
                RptElement(TRN_NONTX_ADR, NULL, pTtlData->NonTaxable,       /* NON-Taxable TL      */
                           0L, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTFIN18_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#118-BIT3  */
				D13DIGITS lOtherTtlTemp = 0;

                lOtherTtlTemp += pTtlData->Taxable[1].lTaxAmount;
                lOtherTtlTemp += pTtlData->Taxable[2].lTaxAmount;
                lOtherTtlTemp += pTtlData->Taxable[3].lTaxAmount;
                RptElement(TRN_PST_ADR, NULL, lOtherTtlTemp,                    /* PST ALL TAX         */
                           0L, CLASS_RPTREGFIN_PRTCGG, 0);
            }
        }
    }

    if (!CliParaMDCCheck(MDC_RPTFIN19_ADR, ODD_MDC_BIT0)) {             /* Check MDC#119-BIT0  */
        RptElement(TRN_CONSTX_ADR, NULL, NULL13DIGITS,                  /* CONSOLI. TAX        */
                   pTtlData->lConsTax, CLASS_RPTREGFIN_PRTDGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN19_ADR, ODD_MDC_BIT1)) {             /* Check MDC#119-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                                   /*                     */
   
    /*----- Get 2-BYTE from Address of MDC #119 to Address of MDC #122 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN19_ADR - 1)/2, &usReturnLen);
    usMDCData >>= 2;                                /* Shift right 2 bit */

	//    ---   Begin Bonus Total Report  -----
	// Do the report on the bonus total values.
	// This report is done in two sections in order to
	// keep the orginal section that handles bonus total 1 through 8
	// followed by a second section that handles bonus total 9 through 100.
	// We do it this way because the numbering system for the mnemonic
	// addresses are in two discontinuous numbering sections:
	//     RATE_BONUS1_ADR & TRN_BNS1_ADR  => Bonus totals 1 through 8
	//     RATE_BONUS9_ADR & TRN_BNS_9_ADR => Bonus totals 9 through 100
	//
	{
		D13DIGITS  lOtherTtlTemp;
		DCURRENCY  lBonusAmt;             /* store caluculated amt */

		lSubTotal = lOtherTtlTemp = 0L;
		Data.uchMajorClass = CLASS_PARADISCTBL;                           
		for (uchBonusIndex = 0; uchBonusIndex < 8; uchBonusIndex++) {                               /* BONUS TL #1 to #8 CO/TL */
			Data.uchAddress = (UCHAR)(RATE_BONUS1_ADR + uchBonusIndex);
			CliParaRead(&Data);
			if (!(usMDCData & 0x01)) {
				RptElement(TRN_BNS1_ADR + uchBonusIndex, &pTtlData->Bonus[uchBonusIndex], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
				lSubTotal += pTtlData->Bonus[uchBonusIndex].lAmount;
				RflRateCalc1(&lBonusAmt, pTtlData->Bonus[uchBonusIndex].lAmount, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);
				lOtherTtlTemp += lBonusAmt;                                 /* BONUS % BONUS TOTAL # x BONUS % */
				RptElement(0, NULL, NULL13DIGITS, lBonusAmt, CLASS_RPTREGFIN_PRTBONUS, Data.uchDiscRate);
			}
			usMDCData >>= 1;                            /* Shift right 1 bit in each time */
			if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
				return(RPT_ABORTED);
			}                                                               /*                 */
			if(RptPauseCheck() == RPT_ABORTED){
				 return(RPT_ABORTED);
			}
		} /*----- End of for Loop -----*/


		//Begin SR 157 Bonus total to 100 increase
		usMDCData = 0;
		CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTFIN42_ADR - 1)/2, &usReturnLen);

		Data.uchMajorClass = CLASS_PARADISCTBL;
		// Now process bonus totals 9 through 100 or STD_NO_BONUS_TTL
		// See note above about why this report is done in two sections.
		for (uchNo = 0; uchBonusIndex < STD_NO_BONUS_TTL; uchNo++, uchBonusIndex++) {
			if (!(usMDCData & 0x01)) {
				Data.uchAddress = (UCHAR)(RATE_BONUS9_ADR + uchNo);
				CliParaRead(&Data);
				RptElement(TRN_BNS_9_ADR + uchNo, &pTtlData->Bonus[uchBonusIndex], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
				lSubTotal += pTtlData->Bonus[uchBonusIndex].lAmount;
				RflRateCalc1(&lBonusAmt, pTtlData->Bonus[uchBonusIndex].lAmount, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);
				lOtherTtlTemp += lBonusAmt;                                 /* BONUS % BONUS TOTAL # x BONUS % */
				RptElement(0, NULL, NULL13DIGITS, lBonusAmt, CLASS_RPTREGFIN_PRTBONUS, Data.uchDiscRate);
			}

			switch(uchNo)
			{
				case  0:  //After Bonus  9	NEXT MDC-> 463C
				case  1:  //After Bonus 10	NEXT MDC-> 463B
				case 11:  //After Bonus 20	NEXT MDC-> 463A
				case 21:  //After Bonus 30	NEXT MDC-> 464D
				case 31:  //After Bonus 40	NEXT MDC-> 464C
				case 41:  //After Bonus 50	NEXT MDC-> 464B
				case 51:  //After Bonus 60	NEXT MDC-> 464A
				case 61:  //After Bonus 70	NEXT MDC-> 465D
				case 71:  //After Bonus 80	NEXT MDC-> 465C
				case 81:  //After Bonus 90	NEXT MDC-> 465B
					usMDCData >>= 1;                        /* Shift right 1 bit each time */
					break;
				default:
					break;
			}

			if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
				return(RPT_ABORTED);
			}                                                           /*                 */
			if(RptPauseCheck() == RPT_ABORTED){
				 return(RPT_ABORTED);
			}
		}
		if (!CliParaMDCCheck(MDC_RPTFIN21_ADR, ODD_MDC_BIT2)) {             /* Check MDC#121-BIT2 */
			RptFeed(RPT_DEFALTFEED);                                        /* Feed               */
		}
		if (!CliParaMDCCheck(MDC_RPTFIN21_ADR, ODD_MDC_BIT3)) {                         /* Check MDC#121-BIT3 */
			RptElement(TRN_TTLR_ADR, NULL, lSubTotal, 0L, CLASS_RPTREGFIN_PRTCGG, 0);   /* ALL BONUS TOTAL    */                    
			RptElement(0, NULL, lOtherTtlTemp, 0L, CLASS_RPTREGFIN_PRTCGG, 0);              /* ALL BONUS RATE     */
		}
	}
	//    ---   End Bonus Total Report  -----

    if (!CliParaMDCCheck(MDC_RPTFIN22_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#122-BIT0 */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed               */
    }

	// 
    if (!CliParaMDCCheck(MDC_RPTFIN3_ADR, ODD_MDC_BIT0)) {              /* Check MDC#103-BIT0 */
        if (CliParaMDCCheck(MDC_MODID13_ADR, ODD_MDC_BIT0)) {           /* Check MDC#33-BIT0  */
            RptElement(TRN_ITMDISC_ADR, &pTtlData->ItemDisc,            /* ITEM DISC.         */
                      NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL             */
        }
    }
    if (!CliParaMDCCheck(MDC_RPTFIN3_ADR, ODD_MDC_BIT1)) {              /* Check MDC#103-BIT1  */
        if (CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0)) {           /* Check MDC#37-BIT0   */
            RptElement(TRN_MODID_ADR, &pTtlData->ModiDisc,              /* MODIFIED ITEM DISC. */
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);  /*  CO/TL              */  
        }
    }

	{
		int i = 0;
		for (i = 0; MdcListItemDiscount[i].sMdcAddrRpt > 0; i++) {
			if ((CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrTtl, MdcListItemDiscount[i].sBitTtl))) {
				// if discount key is an Item Surcharge key
				if (!CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrRpt, MdcListItemDiscount[i].sBitRpt)) {
					RptElement(MdcListItemDiscount[i].sTrnMnem, &pTtlData->ItemDiscExtra.TtlIAmount[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
				}
			}
		}
	}
	{
		int i;
		for (i = 0; MdcListTransDiscount[i].sMdcAddrTtl > 0; i++) {
			if ((CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrTtl, MdcListTransDiscount[i].sBitTtl))) {
				// if discount key is a Transaction Surcharge key
				if (!CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrRpt, MdcListTransDiscount[i].sBitRpt)) {
					RptElement(MdcListTransDiscount[i].sTrnMnem, &pTtlData->RegDisc[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
				}
			}
		}
	}

    if (!CliParaMDCCheck(MDC_RPTFIN22_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#122-BIT1    */
        if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {             /* Check MDC#27-BIT2     */
            RptElement(TRN_HASHDEPT_ADR, &pTtlData->HashDepartment,     /* HASH DEPARTMENT CO/TL */
                       NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);
        }
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                                   /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN22_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#122-BIT2    */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                  */
    }

    if (!CliParaMDCCheck(MDC_RPTFIN22_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#122-BIT3    */
        RptElement(TRN_TRACAN_ADR, &pTtlData->TransCancel,              /* TRAN.CANCLE CO/TL     */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN23_ADR, ODD_MDC_BIT0)) {             /* Check MDC#123-BIT0    */
        RptElement(TRN_MVD_ADR, &pTtlData->MiscVoid,                    /* MISC.VOID CO/TL       */   
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN32_ADR, ODD_MDC_BIT0)) {             /* Check MDC#343-BIT0  */
        RptElement(TRN_LOAN_ADR, &pTtlData->Loan,                       /* Loan CO/TL, Saratoga */
               NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN32_ADR, ODD_MDC_BIT1)) {             /* Check MDC#343-BIT1  */
        RptElement(TRN_PICKUP_ADR, &pTtlData->Pickup,                   /* Pickup CO/TL, Saratoga */
               NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                                   /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN23_ADR, ODD_MDC_BIT1)) {             /* Check MDC#123-BIT1    */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                  */
    }                                                                   /*                       */

    if (!CliParaMDCCheck(MDC_RPTFIN23_ADR, ODD_MDC_BIT2)) {             /* Check MDC#123-BIT2    */
        RptElement(TRN_AUD_ADR, &pTtlData->Audaction,                   /* AUDACTION CO/TL       */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                                   /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTFIN23_ADR, ODD_MDC_BIT3)) {             /* Check MDC#123-BIT3    */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                  */
    }                                                                   /*                       */

    if (!CliParaMDCCheck(MDC_RPTFIN24_ADR, EVEN_MDC_BIT0)) {            /* Check MDC#124-BIT0    */
        RptElement(TRN_NOSALE_ADR, NULL, NULL13DIGITS,                  /* NO SALE COUNTER       */
                  (LONG)(pTtlData->sNoSaleCount), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN24_ADR, EVEN_MDC_BIT1)) {            /* Check MDC#124-BIT1    */
        RptElement(TRN_ITMPROCO_ADR, NULL, NULL13DIGITS,                /* ITEM PRODUCTIVITY CO  */
                   pTtlData->lItemProductivityCount, CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN24_ADR, EVEN_MDC_BIT2)) {            /* Check MDC#124-BIT2    */
        RptElement(TRN_PSN_ADR, NULL, NULL13DIGITS,                     /* NO. OF PERSON         */ 
                   (LONG)(pTtlData->sNoOfPerson), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN24_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#124-BIT3    */
        RptElement(TRN_CUST_ADR, NULL, NULL13DIGITS,                    /* CUSTOMER COUNTER      */ 
                   (LONG)(pTtlData->sCustomerCount), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN25_ADR, ODD_MDC_BIT0)) {             /* Check MDC#125-BIT0    */
        RptElement(TRN_CHKOPN_ADR, NULL, NULL13DIGITS,                  /* NO. OF CHECKS OPENED  */
                   (LONG)(pTtlData->sNoOfChkOpen), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTFIN25_ADR, ODD_MDC_BIT1)) {             /* Check MDC#125-BIT1    */
        RptElement(TRN_CHKCLS_ADR, NULL, NULL13DIGITS,                  /* NO. OF CHECKS CLOSED  */
                   (LONG)(pTtlData->sNoOfChkClose), CLASS_RPTREGFIN_PRTNO, 0);
    }
    
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: VOID RptRegFinPrtTermNumber(USHORT usTermNo) 
*               
*       Input:  USHORT              : usTermNo
*      Output:  nothing
*
**     Return:  nothing 
*
**  Description:  
*       Print out terminal no.
*===============================================================================
*/

VOID RptRegFinPrtTermNumber(USHORT usTermNo, UCHAR uchType)
{
	RPTCASHIER      RptCashier = {0};

    if ((usTermNo < 1) || (usTermNo > TTL_MAX_INDFIN)) {
        return;     /* consolidation file report */
    }

    /*----- Print Terminal Number -----*/
    RptCashier.uchMajorClass = CLASS_RPTCASHIER;                            /* Set Major */
    RptCashier.uchMinorClass = CLASS_RPTCASHIER_TERMINAL;
    RptCashier.ulCashierNumber = usTermNo;                                  /* Set Terminal No */

    if ((uchType == RPT_IND_READ)||(uchType == RPT_IND_RESET)) {
        RptCashier.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;  /* compulsory print */
    } else if (uchType == RPT_EJ_CLUSTER_RESET) {
        RptCashier.usPrintControl = PRT_RECEIPT; /* not print on ej */
    } else {
        RptCashier.usPrintControl = usRptPrintStatus;                       /* depended on MDC */
    }

    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCashier, 0);  /* display on LCD          */ 
        RptCashier.usPrintControl &= PRT_JOURNAL;             /* Reset Receipt print status so only goes to Electronic Journal if set */
    }

    PrtPrintItem(NULL, &RptCashier);                                  /* Print */     

    RptFeed(RPT_DEFALTFEED);
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinDayRead(UCHAR uchMinorClass, UCHAR uchType,
*                                       USHORT usTermNumber, UCHAR uchResetType) 
*               
*       Input:  uchMinorClass
*               uchType 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Read Terminal Daily report.
*===============================================================================
*/

SHORT RptIndFinDayRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usTermNumber, UCHAR uchResetType) 
{
    /* execute Current Daily Report */

    return(RptIndFinRead(uchMinorClass, uchType, usTermNumber, uchResetType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinRead(UCHAR uchMinorClass, UCHAR uchType,
*                                       USHORT usTermNumber, UCHAR uchResetType) 
*               
*       Input:  uchMinorClass 
*               uchType 
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description:  
*       Read Financial report.
*       Prepare for Header and current time.
*       Print out Header and Trailer.
*===============================================================================
*/

SHORT RptIndFinRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usTermNo, UCHAR uchResetType) 
{
    
    UCHAR       uchSpecMnemo,
                uchRptType,
                uchACNo;
    SHORT       sReturn;
	TTLREGFIN   TtlData = {0};                    /* Assign Register Financial Total Save Area */

    /* EOD PTD Report Not Display Out MLD */
    if((uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY)){
        uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
    }
    
    RptPrtStatusSet(uchMinorClass);                         /* Set J|R print status */

    /* Get Register Financial Total */
    TtlData.uchMajorClass = CLASS_TTLINDFIN;                /* Set Major Data Class */
    TtlData.uchMinorClass = uchMinorClass;                  /* Set Minor Data Class */

    if ((uchType == RPT_IND_READ) || (uchType == RPT_IND_RESET)) {  /* Ind. read report ? */
        if ((usTermNo < 1) || (usTermNo > TTL_MAX_INDFIN)) {
            return(LDT_KEYOVER_ADR);
        }
        TtlData.usTerminalNumber = usTermNo;                    /* Set terminal number */
    } else {
        TtlData.usTerminalNumber = 1;   /* Read from master terminal */
    }

    /* Select Special Mnemonics for Header */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
        uchACNo = AC_IND_READ_RPT;                          /* Set A/C number */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */        
        break;
  
    case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
        uchACNo = AC_IND_RESET_RPT;                         /* Set A/C number */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;

    case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
        uchACNo = AC_IND_READ_RPT;                          /* Set A/C number */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */
        break;

    case CLASS_TTLSAVPTD :                                  /* Case of PTD RESET */
        uchACNo = AC_IND_RESET_RPT;                             /* Set A/C number */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;

    default:
		NHPOS_ASSERT_TEXT((uchMinorClass == CLASS_TTLCURDAY), "**ERROR: invalid uchMinorClass value out of range.");
        return(LDT_PROHBT_ADR);
    }

	// do the initial totals read to see if there are any there. If not then we will not
	// bother printing out the header but instead just return an error.
	// otherwise we will do the actual report.
    if ((sReturn = SerTtlTotalRead(&TtlData)) != SUCCESS) { /* Read Failure */
        if (sReturn != TTL_NOTINFILE) {
            return( TtlConvertError(sReturn) );                 /* Return Not in File Error */
        }
    }

    /* Print Header Name */
    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {      /* Not EOD Function */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    } 
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_FINANC_ADR,                         /* Report Name Address */
                    uchSpecMnemo,                           /* Special Mnemonics Address */
                    uchRptType,                             /* Report Name Address */
                    0,                                      /* Not Print */
                    uchACNo,                                /* A/C Number */
                    uchResetType,                           /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */                                       
    
    /* Edit Total Report */
    for (;;) {
		USHORT      usTerminalNumber = TtlData.usTerminalNumber;   // save the current terminal number for multi-terminal report.

        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD))  {
			// Current Daily and Period to Date totals do not have an end date in the
			// totals record so we will just use now.
			TtlGetNdate (&TtlData.ToDate);
        }

        if ((sReturn = RptRegFinEdit(RptElement, &TtlData, 0, uchType)) == RPT_ABORTED) {   /* Aborded By User */
            MaintMakeAbortKey();                                /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
            return(sReturn);
        }

        if (uchMinorClass == CLASS_TTLSAVDAY || uchMinorClass == CLASS_TTLSAVPTD) {        
            SerTtlIssuedReset(&TtlData, TTL_NOTRESET);
        }
    
        if ((uchType == RPT_IND_READ) || (uchType == RPT_IND_RESET))
            break;

		// if this is not an individual report then we need to iterate to the next
		// terminal's totals to continue the report. we clear the totals buffer
		// update the necessary total identifier data, and then fetch the next
		// terminal's financial totals. clear the totals buffer so that if the
		// terminal does not have a totals record, all totals will be zero.

        /* bug fix, saratoga */
        usTerminalNumber ++;                        /* Next terminal number  */
        if (usTerminalNumber > RflGetNoTermsInCluster()) break;

        memset(&TtlData, 0, sizeof(TtlData));

        TtlData.uchMajorClass = CLASS_TTLINDFIN;       /* Set Major Data Class */
        TtlData.uchMinorClass = uchMinorClass;         /* Set Minor Data Class */
        TtlData.usTerminalNumber = usTerminalNumber;   // set the terminal number

        if ((sReturn = SerTtlTotalRead(&TtlData)) != SUCCESS) {
			// a terminal that has not been used may not have any totals yet
			// so a not in file, totals don't exist, is not an error.
			// since we cleared the buffer above, the totals will just be zero.
            if (sReturn != TTL_NOTINFILE) {
                return( TtlConvertError(sReturn) );
            }
        }
        RptFeed(RPT_DEFALTFEED);                                         /* Feed            */
    }

    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {      /* Not EOD Function */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
    } else {
        RptFeed(RPT_DEFALTFEED);                                         /* Feed            */
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinDayReset(UCHAR uchType, UCHAR uchFunc, USHORT usTermNumber) 
*               
*       Input:  UCHAR  uchType  : RPT_IND_RESET
*                               : RPT_ALL_RESET
*                               : RPT_OPEN_RESET
*                               : RPT_EODALL_RESET 
*               UCHAR  uchFunc  : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  sReturn  : SUCCESS
*                          FAIL (FILE NOT FOUND)
*
**  Description:  
*       Check Individual, All or Open reset.
*===============================================================================
*/

SHORT RptIndFinDayReset(UCHAR uchType, UCHAR uchFunc, USHORT usTermNumber) 
{                   
    SHORT sReturn;                
                                            
    if (uchType == RPT_IND_RESET) {                             /* Individual Reset Case */         
        sReturn = RptIndFinIndReset(uchFunc, usTermNumber);  
    } else if ((uchType == RPT_ALL_RESET) || (uchType == RPT_EOD_ALLRESET)) {  /* All Reset Case */
        sReturn = RptIndFinAllReset(uchType, uchFunc); 
    }
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinIndReset(UCHAR uchFunc, USHORT usTermNumber) 
*               
*       Input:  UCHAR  uchFunc      : RPT_PRT_RESET
*                                   : RPT_ONLY_RESET
*               UCHAR  usTermNumber : Terminal Number
*      Output:  nothing
*
**     Return: TTL_SUCCESS             : Successful 
*              TTL_FIL                 : not success  
*              TTL_NOTINFILE           : not in file
*              TTL_NOT_ISSUED          : not issued yet
*              TTL_LOCKED              : locked
*
**  Description:         
*       Reset for Individul Financial Report A/C 233 Funcriotn Type 2 or 3.
*       If A/C 233 Function Type 3 - Only Reset, print Reset Header.  If not, 
*       Reset Header is printed in Read Function.
*       Print out Trailer.
*===============================================================================
*/

SHORT RptIndFinIndReset(UCHAR uchFunc, USHORT usTermNumber) 
{
    SHORT           sReturn;
	TTLREGFIN       TtlData = {0};

    if ((usTermNumber < 1) || (usTermNumber > TTL_MAX_INDFIN)) {
        return(LDT_KEYOVER_ADR);
    }

    TtlData.uchMajorClass = CLASS_TTLINDFIN;                     /* Set Major      */
    TtlData.uchMinorClass = CLASS_TTLSAVDAY;                     /* Set Minor      */
    TtlData.usTerminalNumber = usTermNumber;                     /* Set Terminal No */

    /* Check Saved buffer already print out */
    if (((sReturn = SerTtlIssuedCheck(&TtlData, TTL_NOTRESET)) != TTL_ISSUED) &&
         (sReturn != TTL_NOTINFILE)) {                          /* Not Issued yet ? */ 
        return(TtlConvertError(sReturn));                       /* Error */
    }

    /* Case 1: reset OK, reset co up. Case 2 : total = 0, error */
    TtlData.uchMinorClass = CLASS_TTLCURDAY;                         /* Set Minor      */
    if ((sReturn = SerTtlTotalReset(&TtlData, ~(RPT_RESET_INDFIN_BIT))) == TTL_SUCCESS) {  /* Reset OK ? */
        MaintIncreSpcCo(SPCO_EODRST_ADR);                           /* Reset counter up    */                      
    }

    /* Print Header Name */
    if ((uchFunc == RPT_ONLY_RESET) ||                               /* Reset Only Case */
       ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE))) {   /* Reset Error ? */
                                                
        if (uchFunc == RPT_ONLY_RESET) {
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive Counter */
		}
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class    */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_FINANC_ADR,                         /* Report Name Address */
                        SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        RPT_IND_RESET,                          /* Report Type         */
                        AC_IND_RESET_RPT,                       /* A/C Number          */
                        uchFunc,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit   */                                     
        uchRptOpeCount |= RPT_HEADER_PRINT;

        RptRegFinPrtTermNumber(usTermNumber, RPT_IND_RESET);    /* TERMINAL NO         */

        if ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {  /* Reset Error   */
            RptPrtError(TtlConvertError(sReturn));              /* Convert and Print Error */
        }
        if (uchFunc == RPT_ONLY_RESET) {
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Trailer       */
        }
    }                                                           /* MDC 90 BIT2 is 1 ? */
    return(SUCCESS);                                            /* Return SUCCESS      */
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinAllReset( UCHAR uchType,
*                                       UCHAR uchFunc)
*               
*       Input:  UCHAR  uchType  : RPT_ALL_RESET
*                               : RPT_EODALL_RESET
*               UCHAR  uchFunc  : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**  Return: TTL_SUCCESS             : Successful 
*           TTL_FAIL                : not success   
*           TTL_NOTINFILE           :
*           TTL_NOT_ISSUED          :
*           TTL_LOCKED              :
*
**  Description:  
*       Reset for Financial All Reset A/C 233 Function Type 2 or 3, Report Type 1.
*       Check Error before Header printing.  Classify Only Reset or Reset & Report.
*===============================================================================
*/

SHORT RptIndFinAllReset(UCHAR uchType, UCHAR uchFunc) 
{
    SHORT           sReturn;
    USHORT          usNoOfTerm;
	TTLREGFIN       TtlData = {0};

    TtlData.uchMajorClass = CLASS_TTLINDFIN;                         /* Set Major      */
    TtlData.uchMinorClass = CLASS_TTLSAVDAY;                         /* Set Minor      */
    TtlData.usTerminalNumber = 0;                                     /* Set Cashier No */

    /* Check Saved buffer already print out */
    if (((sReturn = SerTtlIssuedCheck(&TtlData, TTL_NOTRESET)) != TTL_ISSUED) &&
         (sReturn != TTL_NOTINFILE)) {                              /* Not Issued yet ? */ 
        return(TtlConvertError(sReturn));                           /* Error   */
    }

    usNoOfTerm = RflGetNoTermsInCluster ();
    
    if (uchFunc == RPT_ONLY_RESET) {                                /* Only Reset Report ? */
        sReturn = RptIndFinAllOnlyReset(uchType, uchFunc, usNoOfTerm);
    } else {    /*----- RPT_PRT_RESET -----*/                       /* Reset and Report ? */
        sReturn = RptIndFinAllRstReport(uchType, uchFunc, usNoOfTerm);
    }
    return(sReturn);                                                /* Return SUCCESS      */
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinAllOnlyReset(UCHAR uchType,
*                                      SHORT sNo)
*
*       Input:  UCHAR   uchType
*               SHORT   sNo
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Reset Success 
*         : RPT_RESET_FAIL  : Reset Failed in EOD Case
*
**  Description:  
*       Only Reset All Financial files.
*===============================================================================
*/
SHORT  RptIndFinAllOnlyReset(UCHAR uchType,
                          UCHAR uchFunc,
                          SHORT sNo)
{
    UCHAR       uchRptType = uchType,
                uchResetType = uchFunc;
    USHORT      usMDCBit = ~(RPT_RESET_INDFIN_BIT);
    USHORT      usFlag = 0;
    SHORT       i, sReturn; 
	TTLREGFIN   TtlData = {0};

   /* Print Header Name */
    if (uchType == RPT_EOD_ALLRESET) {
        uchRptType = RPT_ALL_RESET;
        uchResetType = 0;
        usMDCBit = RPT_RESET_INDFIN_BIT;
    }
                                                
    if (uchType != RPT_EOD_ALLRESET) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                           /* Count Up Consecutive Counter */
    }
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                          /* Minor Data Class    */              
                    RPT_ACT_ADR,                                    /* Report Name Address */
                    RPT_FINANC_ADR,                         /* Report Name Address */
                    SPC_DAIRST_ADR,                                 /* Special Mnemonics Address */
                    RPT_RESET_ADR,                                  /* Report Name Address */
                    uchRptType,                                     /* Report Type         */
                    AC_IND_RESET_RPT,                              /* A/C Number          */
                    uchResetType,                                   /* Reset Type          */
                    PRT_RECEIPT|PRT_JOURNAL);           /* Print Control Bit   */

    TtlData.uchMajorClass = CLASS_TTLINDFIN;                         /* Set Major      */
    TtlData.uchMinorClass = CLASS_TTLCURDAY;                         /* Set Minor      */
    usFlag = 0;
    for (i = 1; i <= sNo; i++) {                                    /* Get All Terminal files */
        TtlData.usTerminalNumber = i;                               /* Set Terminal No        */
        if ((sReturn = SerTtlTotalReset(&TtlData, usMDCBit)) == TTL_SUCCESS) {  /* Reset OK?         */ 
            usFlag = 1;
        } 
        if ((sReturn !=TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {  /* if Reset Error */
            RptPrtError(TtlConvertError(sReturn));                    /* Print Error # */
        }
    }

    if ((uchType != RPT_EOD_ALLRESET) && (usFlag == 1)) {
        MaintIncreSpcCo(SPCO_EODRST_ADR);                           /* Reset counter up    */                      
    }

    if (uchType != RPT_EOD_ALLRESET) {
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Trailer       */
    } else {                                                        /* One Line Feed for EOD */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }             

    if ((uchType == RPT_EOD_ALLRESET) && (usFlag != 1) && sNo) {    /* Reset Failed in EOD Case */
        return(RPT_RESET_FAIL);                                                 
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptIndFinAllRstReport(UCHAR  uchType,
*                                      UCHAR  uchFunc, 
*                                      SHORT  sNo)
*               
*       Input:  UCHAR   uchType         : RPT_ALL_RESET
*                                       : RPT_EODALL_RESET
*               UCHAR   uchFunc         : RPT_PRT_RESET
*               SHORT   sNo             :
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Reset Success 
*         : RPT_RESET_FAIL  : Reset Failed in EOD Case
*
**  Description:  
*       Reset All financial files.
*       Prepare data for Reset Header and print out.
*       Print out Reset Header.
*===============================================================================
*/ 
SHORT  RptIndFinAllRstReport(UCHAR  uchType,
                          UCHAR  uchFunc,
                          SHORT  sNo)
{
    UCHAR       uchResetType = uchFunc;
    UCHAR       uchRptType = uchType;
    USHORT      usMDCBit = ~(RPT_RESET_INDFIN_BIT); 
    USHORT      usFlag = 0; 
    SHORT       i;
	TTLREGFIN   TtlData = {0};

    if (uchType == RPT_EOD_ALLRESET) {
        uchRptType = RPT_ALL_RESET;
        uchResetType = 0;
        usMDCBit = RPT_RESET_INDFIN_BIT;
    }

    TtlData.uchMajorClass = CLASS_TTLINDFIN;                        /* Set Major      */
    TtlData.uchMinorClass = CLASS_TTLCURDAY;                         /* Set Minor      */
    for (i = 1; i <= sNo; i++) { 
		SHORT       sReturn;

        TtlData.usTerminalNumber = i;
        if ((sReturn = SerTtlTotalReset(&TtlData, usMDCBit)) == TTL_SUCCESS) {  /* Reset OK?       */
            usFlag = 1;
        } else if (sReturn != TTL_NOTINFILE) {                          /* if Reset Fail */
            if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation Case */                   
                MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class    */              
                                RPT_ACT_ADR,                            /* Report Name Address */
                                RPT_FINANC_ADR,                         /* Report Name Address */
                                SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
                                RPT_RESET_ADR,                          /* Report Name Address */
                                uchRptType,                             /* Report Type         */
                                AC_IND_RESET_RPT,                      /* A/C Number          */
                                uchResetType,                           /* Reset Type          */
                                PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit   */
                uchRptOpeCount |= RPT_HEADER_PRINT;
            }
            if ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {   /* Reset Error */
                RptPrtError(TtlConvertError(sReturn));                      /* Convert and Print Error */
            }
        }
    }

    if ((uchType != RPT_EOD_ALLRESET) && (usFlag == 1)) {
        MaintIncreSpcCo(SPCO_EODRST_ADR);                               /* Reset counter up    */                      
    }
    if ((uchType == RPT_EOD_ALLRESET) && (usFlag != 1) && sNo) {        /* Reset Failed in EOD Case */
        return(RPT_RESET_FAIL);                                                 
    }
    return(SUCCESS);
}
                                                                                          
/*
*=============================================================================
**  Synopsis: SHORT RptIndFinLock(UCHAR uchType, USHORT usTermNumber) 
*               
*       Input:  UCHAR       uchType  : RPT_IND_LOCK
*                                    : RPT_ALL_LOCK
*               USHORT      usTermNumber 
*      Output:  nothing
*
**     Return:  
*
**  Description:  
*       Lock destination terminal for reset report.
*===============================================================================
*/

SHORT RptIndFinLock(UCHAR uchType, USHORT usTermNumber) 
{
    SHORT  sReturn;  
                                                                     
    if (uchType == RPT_IND_LOCK) { 
        sReturn = SerCasIndTermLock(usTermNumber);                  /* Lock Complete */
        if (sReturn != CAS_PERFORM) {
            return(CasConvertError(sReturn));                       /* Return Error */
        }
    } else {                                                        /* All Lock Case */
        sReturn = SerCasAllTermLock();                              /* Lock Complete */
        if (sReturn != CAS_PERFORM) {
            return(CasConvertError(sReturn));                       /* Return Error */
        }
    }
    return SUCCESS; /* ### Add (2171 for Win32) V1.0 */
}

/*
*=============================================================================
**  Synopsis: VOID RptIndFinUnLock() 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  nothing 
*                 
**  Description:  
*       Unlock terminal.
*===============================================================================
*/

VOID RptIndFinUnLock() 
{
    SerCasAllTermUnLock();                                  
}

/*
*=============================================================================
**  Synopsis: SHORT RptRegFinVAT(TTLREGFIN *pTtlData)
*               
*       Input:  pTtlData    
*      Output:  nothing
*
*      Return:  SUCCESS       - Successful
*               RPT_ABORTED   - Aborted by User
*
**  Description:  
*       Formats and prints applicable total and total of 3 VAT
*       to the receipt and journal printer if particular MDC Bit is on. V3.3
*===============================================================================
*/
SHORT RptRegFinVAT(TTLREGFIN *pTtlData)
{
    static UCHAR FARCONST
            auchMDCVatTotalAdr[] = {MDC_RPTFIN15_ADR,     /* VAT #1 TOTAL */
                                    MDC_RPTFIN16_ADR,     /* VAT #2 TOTAL */
                                    MDC_RPTFIN17_ADR},    /* VAT #3 TOTAL */

            auchMDCVatTotalBit[] = {ODD_MDC_BIT3,    /* VAT #1 TOTAL */
                                    EVEN_MDC_BIT2,    /* VAT #2 TOTAL */
                                    ODD_MDC_BIT1},    /* VAT #3 TOTAL */

            auchMDCVatApAdr[] = {MDC_RPTFIN15_ADR,    /* VAT #1 APPLICABLE TOTAL */
                                 MDC_RPTFIN16_ADR,    /* VAT #2 APPLICABLE TOTAL */
                                 MDC_RPTFIN17_ADR},   /* VAT #3 APPLICABLE TOTAL */

            auchMDCVatApBit[] = {ODD_MDC_BIT2,        /* VAT #1 APPLICABLE TOTAL */
                                 EVEN_MDC_BIT1,       /* VAT #2 APPLICABLE TOTAL */
                                 ODD_MDC_BIT0},      /* VAT #3 APPLICABLE TOTAL */

            auchTransAprAddr[] = {TRN_TXBL1_ADR,      /* TRANSACTION MNEMONICS VAT #1 APPLICABLE */
                                  TRN_TXBL2_ADR,      /* TRANSACTION MNEMONICS VAT #2 APPLICABLE */
                                  TRN_TXBL3_ADR},     /* TRANSACTION MNEMONICS VAT #3 APPLICABLE */
                                                 
            auchTransAddr[] = {TRN_TX1_ADR,           /* TRANSACTION MNEMONICS VAT #1 TOTAL */
                               TRN_TX2_ADR,           /* TRANSACTION MNEMONICS VAT #2 TOTAL */
                               TRN_TX3_ADR},          /* TRANSACTION MNEMONICS VAT #3 TOTAL */

            auchVatRateAddr[] = {TXRT_NO1_ADR,        /* VAT #1 RATE */
                                 TXRT_NO2_ADR,        /* VAT #2 RATE */
                                 TXRT_NO3_ADR};       /* VAT #3 RATE */

    USHORT   usVATNo;       /* VAT number */

    for (usVATNo = 0; usVATNo < 3; usVATNo++) {
        if (!CliParaMDCCheck(auchMDCVatApAdr[usVATNo], auchMDCVatApBit[usVATNo])) {                /* Check MDC  */
            /* VAT Applicable Total */
            RptElement(auchTransAprAddr[usVATNo], NULL, pTtlData->Taxable[usVATNo].mlTaxableAmount, 0, CLASS_RPTREGFIN_PRTCGG, 0);
        }
        if (!CliParaMDCCheck(auchMDCVatTotalAdr[usVATNo], auchMDCVatTotalBit[usVATNo])) {              /* Check MDC  */
            if (CliParaMDCCheck(MDC_VAT1_ADR, ODD_MDC_BIT2)) {
                /* Exclude VAT */
                RptElement(auchTransAddr[usVATNo], NULL, NULL13DIGITS, pTtlData->Taxable[usVATNo].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
            } else {
				D13DIGITS d13VatTtl;    /* store include vat total */

                /* Include VAT */
                RptVatCalc2(&d13VatTtl, &pTtlData->Taxable[usVATNo].mlTaxableAmount, auchVatRateAddr[usVATNo]);
                RptElement(auchTransAddr[usVATNo], NULL, d13VatTtl, 0, CLASS_RPTREGFIN_PRTCGG, 0);
            }
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                            /* if Abort ?          */
           return(RPT_ABORTED);
        }
    }
    return SUCCESS;
}


SHORT  ItemGenerateAc23Report (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, USHORT usTerminalNo)
{
	SHORT       sReturn;
	USHORT      usStartTerminalNo, usEndTerminalNo;
	TTLREGFIN   TtlData = {0};                    /* Assign Register Financial Total Save Area */

    if (!fpFile) return -1;

    /* EOD PTD Report Not Display Out MLD */
	// (uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY))
	usStartTerminalNo = usEndTerminalNo = usTerminalNo;
	switch (uchMajorClass) {
		case CLASS_TTLREGFIN:
			usStartTerminalNo = 0;
			usEndTerminalNo = 0;
			break;
		case CLASS_TTLINDFIN:
			if (usTerminalNo < 1 || usTerminalNo > 16) {
				usStartTerminalNo = 1;
				usEndTerminalNo = 16;
			}
			break;
	}

	if (fpRptElementStreamFile) {
        for (; usStartTerminalNo <= usEndTerminalNo; usStartTerminalNo++) {
			/* Get Register Financial Total, either CLASS_TTLINDFIN or CLASS_TTLREGFIN */
			TtlData.uchMajorClass = uchMajorClass;                /* Set Major Data Class */
			TtlData.uchMinorClass = uchMinorClass;                  /* Set Minor Data Class */
			TtlData.usTerminalNumber = usStartTerminalNo;

			if ((sReturn = SerTtlTotalRead(&TtlData)) != SUCCESS) { /* Read Failure */
				continue;                 /* Return Not in File Error */
			}

            if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
                wchar_t  aszMnemo[PARA_TRANSMNEMO_LEN + 1] = { 0 };

                switch (uchMinorClass) {
                case CLASS_TTLCURDAY:
                    fprintf(fpRptElementStreamFile, "<h2>AC 23 Financial Report - Current Daily Totals Terminal %d</h2>\n", TtlData.usTerminalNumber);
                    break;
                case CLASS_TTLCURPTD:
                    fprintf(fpRptElementStreamFile, "<h2>AC 23 Financial Report - Current Period To Day Totals Terminal %d</h2>\n", TtlData.usTerminalNumber);
                    break;
                case CLASS_TTLSAVDAY:
                    fprintf(fpRptElementStreamFile, "<h2>AC 23 Financial Report - Saved Daily Totals Terminal %d</h2>\n", TtlData.usTerminalNumber);
                    break;
                case CLASS_TTLSAVPTD:
                    fprintf(fpRptElementStreamFile, "<h2>AC 23 Financial Report - Saved Period To Day Totals Terminal %d</h2>\n", TtlData.usTerminalNumber);
                    break;
                }

                if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {  /* DD/MM/YY */
                    fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlData.FromDate.usMDay, TtlData.FromDate.usMonth,
                        TtlData.FromDate.usYear, TtlData.FromDate.usHour, TtlData.FromDate.usMin);
                    fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlData.ToDate.usMDay, TtlData.ToDate.usMonth,
                        TtlData.ToDate.usYear, TtlData.ToDate.usHour, TtlData.ToDate.usMin);
                }
                else {  /* MM/DD/YY */
                    fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlData.FromDate.usMonth, TtlData.FromDate.usMDay,
                        TtlData.FromDate.usYear, TtlData.FromDate.usHour, TtlData.FromDate.usMin);
                    fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlData.ToDate.usMonth, TtlData.ToDate.usMDay,
                        TtlData.ToDate.usYear, TtlData.ToDate.usHour, TtlData.ToDate.usMin);
                }
                fprintf(fpRptElementStreamFile, "<table border=\"1\" cellpadding=\"8\">\n<tr><th>Name</th><th>Amount</th><th>Count</th></tr>\r\n");
            }
        
 			sReturn = RptRegFinEdit(RptElementStream, &TtlData, 0, uchMinorClass);

            if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
                fprintf(fpRptElementStreamFile, "</table>\r\n");
            }
        }
	}

    return SUCCESS;
}


/**** End of File ****/
