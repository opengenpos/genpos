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
* Title       : Cashier Report Module
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application
* Program Name: RPTCAS.C
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
*               RptCashierRead()          : Read Cashier Report Module
*                  RptCashierEdit()       : Edit Cashier Report
*               RptCashierReset()         : Reset Cashier Report Module
*                  RptCashierIndReset()   : Reset Cashier individually
*                  RptCashierAllReset()   : Reset Cashier all
*                    RptCasAllOnlyReset() : Only reset all cashier
*                    RptCasAllRstReport() : Reset and report all cashier
*                 RptCashierOpenReset()   : Reset Specified Cashier in an emergency
*                 RptPrtCasTrng1()        : Print Training Mnemonics depend on MDC
*                 RptPrtCasTrng2()        : Print Training Mnemonics in some condition
*               RptCashierLock()          : Lock and Save Cashier No. Module
*               RptCashierUnLock()        : UnLock Cashier Module
*               RptChkCashierNo()         : Check waiter exist
*               RptPrtCasTrng()           : Check Training Cashier
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-10-99: 01.00.00   : hrkato    : Saratoga
** GenPOS
* Jul-06-17 : 02.02.02 : rchambers : code cleanup, initialize, localize variables.
* Feb-19-19 : 02.02.02 : R.Chambers : print Surcharges report for Item Discount key #3 - #6.
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
==============================================================================
;                      I N C L U D E     F I L E s
;=============================================================================
*/

#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "maint.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csop.h"
#include "csttl.h"
#include "csstbttl.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "report.h"
#include "mld.h"
#include "mldsup.h"
#include <plu.h>
#include "rptcom.h"
/*
==============================================================================
;                      C O M M O N  R A M   E R E A s
;=============================================================================
*/

static D13DIGITS CONST NULL13DIGITS = 0;

/*
*=============================================================================
**  Synopsis: TCHAR *RptCashierGetName (TCHAR *auchCashierName, ULONG ulCasNumber)
*
*       Input:  ULONG ulCasNumber
*      Output:  TCHAR *auchCashierName
*
**     Return:  TCHAR *auchCashierName
*
**  Description:
*       Read the Cashier record for the specified Cashier Id number and provide to the
*       caller the name that is specified in the Cashier record.
*===============================================================================
*/
static TCHAR *RptCashierGetName (TCHAR *auchCashierName, ULONG ulCasNumber)
{
	auchCashierName[0] = 0;    // ensure zero terminates the string.
	if (ulCasNumber > 0) {
		CASIF    Arg = {0};

		Arg.ulCashierNo = ulCasNumber;                          /* Set Cashier No            */
		SerCasIndRead(&Arg);                                    /* Get Cash background       */

		_tcsncpy (auchCashierName, Arg.auchCashierName, PARA_CASHIER_LEN);
		auchCashierName[PARA_CASHIER_LEN] = 0;    // ensure zero terminates the string.
	}
	return auchCashierName;
}


/*
*=============================================================================
**  Synopsis: SHORT RptCashierRead(UCHAR uchMinorClass,
*                                  UCHAR uchType,
*                                  USHORT usCasNumber,
*                                  UCHAR uchResetType)
*
*       Input:  UCHAR   uchMinorClass : CLASS_TTLCURDAY - AC21
*                                     : CLASS_TTLSAVDAY - AC19
*                                     : CLASS_TTLCURPTD - AC21
*                                     : CLASS_TTLSAVPTD - AC19
*               UCHAR   uchType       : RPT_ALL_READ
*                                     : RPT_IND_READ
*                                     : RPT_EOD_ALLREAD
*               USHORT  usCasNumber   :
*               UCHAR   uchResetType  : 0
*                                     : RPT_ONLY_PRT_RESET
*                                     : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful
*               RPT_ABORTED         : Aborted by User
*
**  Description:
*       Read the cashier report individualy or all.
*       Prepare for report Header status and print out Header.
*
*       This function is used by AC19, AC21, and AC135.
*         - AC19 Cashier Totals Reset
*         - AC21 Cashier Totals Report
*
*       Edit Cashier Read Report A/C 21 and Cashier Reset Report A/C 19 Type 1,
*       and Type 2 (Report).
*       Print out Trailer.
*       There is no Header printing, if the cashier file does't exist. V3.3
*===============================================================================
*/
SHORT RptCashierRead(UCHAR uchMinorClass, UCHAR uchType, ULONG ulCashierNo, UCHAR uchResetType) 
{
    UCHAR       uchACNo,
                uchRptType,
                uchSpecMnemo,
                uchRdRst;
    SHORT       sReturn = 0,
                sNo,
                sReturnCasNo;
    ULONG       ausRcvBuffer[MAX_NO_CASH];
	TTLCASHIER  TtlCas = {0};

    RptPrtStatusSet(uchMinorClass);                             /* Set J|R print status */

     /* Select Special Mnemonics for Header, V3.3 */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY :                      /* Case of Daily READ */
        uchSpecMnemo = SPC_DAIRST_ADR;              /* Set Daily Special Mnemonics */
        uchRdRst = RPT_READ_ADR;                    /* Set Read Mnemonics */        
        uchACNo = AC_CASH_READ_RPT;                 /* Set A/C Number */
        break;
   
    case CLASS_TTLSAVDAY :                      /* Case of Daily RESET */
        uchSpecMnemo = SPC_DAIRST_ADR;              /* Set Daily Special Mnemonics */
        uchRdRst = RPT_RESET_ADR;                   /* Set Reset Mnemonics */
        uchACNo = AC_CASH_RESET_RPT;                /* Set A/C Number */
        break;

    case CLASS_TTLCURPTD :                      /* Case of Period To Date READ */
        uchSpecMnemo = SPC_PTDRST_ADR;              /* Set PTD Special Mnemonics */
        uchRdRst = RPT_READ_ADR;                    /* Set Read Mnemonics */
        uchACNo = AC_CASH_READ_RPT;                 /* Set A/C Number */
        break;

    case CLASS_TTLSAVPTD :                      /* Case of Period To Date RESET */
        uchSpecMnemo = SPC_PTDRST_ADR;              /* Set PTD Special Mnemonics */
        uchRdRst = RPT_RESET_ADR;                   /* Set Reset Mnemonics */
        uchACNo = AC_CASH_RESET_RPT;                /* Set A/C Number */
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "==ERROR: RptCashierRead() Invalid uchMinorClass.");
        return(LDT_KEYOVER_ADR);    // MODULE_RPT_ID, FAULT_INVALID_DATA
    }

    if (uchType == RPT_IND_READ) {                              /* Ind. read report ? */
        sNo = 1;
        ausRcvBuffer[0] = ulCashierNo;                          /* Set cashier No */
    } else if ((uchType == RPT_ALL_READ) || (uchType == RPT_EOD_ALLREAD) || (uchType == RPT_PTD_ALLREAD)) {
        if ((sNo = SerCasAllIdRead(sizeof(ausRcvBuffer), ausRcvBuffer)) < 0) {
            return( CasConvertError(sNo) );
        }
	} else {
		NHPOS_ASSERT_TEXT(0, "==ERROR: RptCashierRead() Invalid uchType.");
        return(LDT_KEYOVER_ADR);    // MODULE_RPT_ID, FAULT_INVALID_DATA
	}

    TtlCas.uchMajorClass = CLASS_TTLCASHIER;                    /* Set Major */
    TtlCas.uchMinorClass = uchMinorClass;                       /* Set Minor for class of total */
    TtlCas.ulCashierNumber = ausRcvBuffer[0];                   /* Set cashier No */

	sReturnCasNo = TTL_NOTINFILE;
	if (sNo != 0) {
		// if this cashier has not actually been used during this period then there will not be
		// totals for this cashier so we will see Not in File (TTL_NOTINFILE) so we will just
		// report zero for every thing.
        if (((sReturnCasNo = SerTtlTotalRead(&TtlCas)) != TTL_SUCCESS) && sReturnCasNo != TTL_NOTINFILE) {
            return( TtlConvertError(sReturnCasNo) );
        }
    }

    /* Print Header Name */
    uchRptType = uchType;                                   /* Store uchType to uchRptType */
    if (uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {
        uchRptType = RPT_ALL_READ;                          /* Store uchRptType */
    }
    if ((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)) {                                                           
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    }
    if ((!(uchRptOpeCount & RPT_HEADER_PRINT)) ||               /* First Operation Case */
       ((uchType == RPT_EOD_ALLREAD) && (uchRptOpeCount & RPT_HEADER_PRINT)) ||
       ((uchType == RPT_PTD_ALLREAD) && (uchRptOpeCount & RPT_HEADER_PRINT ))) {                  
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_CAS_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        uchRdRst,                               /* Report Name Address */
                        uchRptType,                             /* Report Type */
                        uchACNo,                                /* A/C Number */
                        uchResetType,                           /* Reset Type */
                        PRT_RECEIPT | PRT_JOURNAL); /* Print Control Bit */
    }

    /* Edit Total Report */
    if (uchType == RPT_IND_READ) {                                                  /* Indivisual read? */
		if (sReturnCasNo != TTL_SUCCESS) {                               /* MDC 90 BIT3 is 1 (not print TL/CO are 0) */
			// if this cashier has not actually been used during this period then there will not be
			// totals for this cashier so we will see Not in File (TTL_NOTINFILE) so we will just
			// report zero for every thing if MDC 90 indicates we should do so.
            if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT3)) {             
				MaintMakeAbortKey();                                     /* Print ABORTED */
				MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);             /* Print Trailer */
				return(sReturn);
            }
		}
        if ((sReturn = RptCashierEdit(RptElement, uchMinorClass, &TtlCas)) == RPT_ABORTED) {    /* Aborted By User */
            MaintMakeAbortKey();                                                    /* Print ABORTED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                            /* Print Trailer */
            return(sReturn);
        } else {                                            /* Cashier Edit Success */
            if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)) {
                SerTtlIssuedReset(&TtlCas, TTL_NOTRESET);   /* Cashier issued Reset */
            }
        }
    } else if ((uchType == RPT_ALL_READ) || (uchType == RPT_EOD_ALLREAD) || (uchType == RPT_PTD_ALLREAD)) {   /* PTD All Read ? */
		SHORT     sCasNo;
        for (sCasNo = 0; sCasNo < sNo; sCasNo++) {
            memset(&TtlCas, 0, sizeof(TtlCas));                                 /* Buffer clear */
            TtlCas.uchMajorClass = CLASS_TTLCASHIER;                            /* Set Major */
            TtlCas.uchMinorClass = uchMinorClass;                               /* Set Minor */
            TtlCas.ulCashierNumber = ausRcvBuffer[sCasNo];                      /* Set cashier No */
            sReturn = SerTtlTotalRead(&TtlCas);                                 /* Get Total */

            switch(sReturn) {
            case TTL_SUCCESS:
                               /* MDC 90 BIT3 is 1 (not print TL/CO are 0) */
                if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT3)) {             
                               /* Check Daily Gross Group Total is 0 (NO Sales) */                                                 
                    if (TtlStringCheck(&TtlCas.lDGGtotal, USRPTCASLENTTL, 0) == TTL_SUCCESS) {
                        if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)) {
                            SerTtlIssuedReset(&TtlCas, TTL_NOTRESET);           /* Set Issued Reset */
                        }                                                       /* Read Report ? */
                        continue;                                               /* continue */
                    }
                }
                break;

            case TTL_NOTINFILE:                                          /* MDC 90 BIT3 is 1 (not print TL/CO are 0) */
				// if this cashier has not actually been used during this period then there will not be
				// totals for this cashier so we will see Not in File (TTL_NOTINFILE) so we will just
				// report zero for every thing if MDC 90 indicates we should do so.
                if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT3)) {             
                    continue;                                            /* continue */
                }
                break;

            default:
                RptPrtError( TtlConvertError(sReturn));                  /* Print converted Error Code */
                continue;
            }
            if (sCasNo != 0) {
                RptFeed(RPT_DEFALTFEED);                                 /* line feed     */ 
            }
            if ((sReturn = RptCashierEdit(RptElement, uchMinorClass, &TtlCas)) == RPT_ABORTED) { 
                MaintMakeAbortKey();                                            /* Print ABORTED */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                    /* Print Trailer */
                return(RPT_ABORTED);
            }
            if (((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)) &&
                (sReturn != TTL_NOTINFILE)) {
                SerTtlIssuedReset(&TtlCas, TTL_NOTRESET);
            }
        }
    }
    if ((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)) { /* Not EOD,PTD */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                            /* Print Trailer */
    } else {
        RptFeed(RPT_DEFALTFEED);
    }

    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCashierEdit(UCHAR uchMinorClass, TTLCASHIER *pTtlCas)
*               
*       Input:  UCHAR       uchMinorClass
*               TTLCASHIER  *pTtlCas 
*      Output:  TTLCASHIER  *pTtlCas
*
**     Return:  SUCCESS         : Successful
*               RPT_ABORTED     : Aborted by User
*
**  Description:
*       Set the data for an element of print format each time.
*       This function is just same shape of printing format. V3.3
*===============================================================================
*/
SHORT RptCashierEdit(RptElementFunc RptElement, UCHAR uchMinorClass, TTLCASHIER *pTtlCas)
{
    UCHAR  uchNo,                  /* store any Number      */
           uchMinor, uchRnd, uchMDCMaskData, fchFS,uchBonusIndex;
    USHORT usMDCData, usMDCHand, usReturnLen, usMDCAddress;
	D13DIGITS  lSubTotal, d13OtherTtl;   /* store Sub Total       */
	DCURRENCY  lBonusAmt;                /* store caluculated amt */
	D13DIGITS  lSalesTotal;
    PARACURRENCYTBL FCTbl;       
    PARADISCTBL     Data;
	RPTCASHIER      RptCashier = {0};
    TOTAL           Totalcount[3];  /* assign for Service Tl/Co */
	struct {
		SHORT  sMdcAddrTtl;    // indicates if discount key is a Discount or a Surcharge
		UCHAR  sBitTtl;
		SHORT  sMdcAddrRpt;    // indicates if discount is to appear on report or not
		UCHAR  sBitRpt;
		SHORT  sTrnMnem;       // transaction mnemonic identifier for report mnemonic
	}  MdcListItemDiscount [] = {   // uses CliParaMDCCheckField(), index same as pTtlCas->ItemDiscExtra.TtlIAmount[i]
		{MDC_MODID33_ADR, MDC_PARAM_BIT_D, MDC_RPTCAS49_ADR, MDC_PARAM_BIT_D, TRN_ITMDISC_ADR_3},   // Item Discount #3, pTtlCas->ItemDiscExtra.TtlIAmount[0]
		{MDC_MODID43_ADR, MDC_PARAM_BIT_D, MDC_RPTCAS49_ADR, MDC_PARAM_BIT_C, TRN_ITMDISC_ADR_4},   // Item Discount #4, pTtlCas->ItemDiscExtra.TtlIAmount[1]
		{MDC_MODID53_ADR, MDC_PARAM_BIT_D, MDC_RPTCAS49_ADR, MDC_PARAM_BIT_B, TRN_ITMDISC_ADR_5},   // Item Discount #5, pTtlCas->ItemDiscExtra.TtlIAmount[2]
		{MDC_MODID63_ADR, MDC_PARAM_BIT_D, MDC_RPTCAS49_ADR, MDC_PARAM_BIT_A, TRN_ITMDISC_ADR_6},   // Item Discount #6, pTtlCas->ItemDiscExtra.TtlIAmount[3]
		{0, 0, 0, 0}
	}, MdcListTransDiscount [] = {    // uses CliParaMDCCheck(), index same as for pTtlCas->RegDisc[i]
		{MDC_RDISC13_ADR, ODD_MDC_BIT0, MDC_RPTCAS3_ADR, ODD_MDC_BIT0, TRN_RDISC1_ADR},        // Transaction Discount #1, pTtlCas->RegDisc[0]
		{MDC_RDISC23_ADR, ODD_MDC_BIT0, MDC_RPTCAS3_ADR, ODD_MDC_BIT1, TRN_RDISC2_ADR},        // Transaction Discount #2, pTtlCas->RegDisc[1]
		{MDC_RDISC33_ADR, ODD_MDC_BIT0, MDC_RPTCAS22_ADR, EVEN_MDC_BIT0, TRN_RDISC3_ADR},      // Transaction Discount #3, pTtlCas->RegDisc[2]
		{MDC_RDISC43_ADR, ODD_MDC_BIT0, MDC_RPTCAS22_ADR, EVEN_MDC_BIT1, TRN_RDISC4_ADR},      // Transaction Discount #4, pTtlCas->RegDisc[3]
		{MDC_RDISC53_ADR, ODD_MDC_BIT0, MDC_RPTCAS22_ADR, EVEN_MDC_BIT2, TRN_RDISC5_ADR},      // Transaction Discount #5, pTtlCas->RegDisc[4]
		{MDC_RDISC63_ADR, ODD_MDC_BIT0, MDC_RPTCAS22_ADR, EVEN_MDC_BIT3, TRN_RDISC6_ADR},      // Transaction Discount #6, pTtlCas->RegDisc[5]
		{0, 0, 0, 0}
	};

    RptPrtCasTrng1(pTtlCas->ulCashierNumber);                               /* Call PrtPrtTrng()     */ 

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
         return(RPT_ABORTED);
    }

    /*----- Print Cashier Mnimonics and Number -----*/
    RptCashier.uchMajorClass = CLASS_RPTCASHIER;                            /* Set Major */
    RptCashier.uchMinorClass = CLASS_RPTCASHIER_READ;
    RptCashier.ulCashierNumber = pTtlCas->ulCashierNumber;                  /* Set Cashier No */
    RptCashierGetName (RptCashier.aszCashMnemo, RptCashier.ulCashierNumber); /* Copy Mnemo */
    RptCashier.usPrintControl = usRptPrintStatus;                           /* Print Control Bit */
    if (! RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCashier, 0); /* display on LCD          */ 
            RptCashier.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }

        if (! RptCheckReportOnStream()) PrtPrintItem(NULL, &RptCashier);                                        /* Print */
        RptFeed(RPT_DEFALTFEED);

        /* V3.3 */
	    // Current Daily report or Current PTD report ?
        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD))  {
		    TtlGetNdate (&pTtlCas->ToDate);
        }
        RptPrtTime(TRN_PFROM_ADR, &pTtlCas->FromDate);                  /* PERIOD FROM     */
        RptPrtTime(TRN_PTO_ADR, &pTtlCas->ToDate);                      /* PERIOD TO       */  
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                           
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        RptFeed(RPT_DEFALTFEED);                                        /* Feed            */                                                 
    }

    lSubTotal = 0L;
    lSalesTotal = 0L;
    RptElement(TRN_DAIGGT_ADR, NULL, NULL13DIGITS,                  /* DAILY GROSS     */
               pTtlCas->lDGGtotal, CLASS_RPTREGFIN_PRTDGG,0);       /* GROUP TL        */ 
    lSubTotal = pTtlCas->lDGGtotal;                                 /* PLUS VOID CO/TL */
    lSalesTotal = pTtlCas->lDGGtotal;                                 /* PLUS VOID CO/TL */
    if (!CliParaMDCCheck(MDC_RPTCAS1_ADR, ODD_MDC_BIT1)) {          /* Check MDC#131-BIT1  */
        RptElement(TRN_PLUSVD_ADR, &pTtlCas->PlusVoid, NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->PlusVoid.lAmount;
    }
    lSalesTotal += pTtlCas->PlusVoid.lAmount;

    if (!CliParaMDCCheck(MDC_RPTCAS1_ADR, ODD_MDC_BIT2)) {          /* Check MDC#131-BIT2  */
        RptElement(TRN_PSELVD_ADR, &pTtlCas->PreselectVoid,         /* PRE-SELECT VOID */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL          */
        lSubTotal += pTtlCas->PreselectVoid.lAmount;
    }
    lSalesTotal += pTtlCas->PreselectVoid.lAmount;

    if (!CliParaMDCCheckField(MDC_RPTCAS50_ADR, MDC_PARAM_BIT_D)) {          /* Check MDC#131-BIT2  */
        RptElement(TRN_TRETURN1_ADR, &pTtlCas->TransactionReturn,         /* Returns #1, Returns */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL          */
        lSubTotal += pTtlCas->TransactionReturn.lAmount;
    }
    lSalesTotal += pTtlCas->TransactionReturn.lAmount;

    if (!CliParaMDCCheckField(MDC_RPTCAS50_ADR, MDC_PARAM_BIT_C)) { /* Check MDC#131-BIT2  */
        RptElement(TRN_TRETURN2_ADR, &pTtlCas->TransactionExchng,     /* Retnurns #2, Exchanges */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL          */
        lSubTotal += pTtlCas->TransactionExchng.lAmount;
    }
    lSalesTotal += pTtlCas->TransactionExchng.lAmount;

    if (!CliParaMDCCheckField(MDC_RPTCAS50_ADR, MDC_PARAM_BIT_B)) { /* Check MDC#131-BIT2  */
        RptElement(TRN_TRETURN3_ADR, &pTtlCas->TransactionRefund,     /* Returns #3, Refunds */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL          */
        lSubTotal += pTtlCas->TransactionRefund.lAmount;
    }
    lSalesTotal += pTtlCas->TransactionRefund.lAmount;

    if (!CliParaMDCCheck(MDC_RPTCAS1_ADR, ODD_MDC_BIT3)) {          /* Check MDC#131-BIT3  */
        RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                  /* SUB TOTAL       */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                               /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#132-BIT0  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */
      
    if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#132-BIT1  */
        RptElement(TRN_CONSCPN_ADR, &pTtlCas->ConsCoupon,           /* COSOLIDATION COUPON */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);   /*  CO/TL              */
        lSubTotal += pTtlCas->ConsCoupon.lAmount;
    }
    lSalesTotal += pTtlCas->ConsCoupon.lAmount;

	// ---    Print out the Discount totals section of the report    ---

    if (!(CliParaMDCCheck(MDC_MODID13_ADR, ODD_MDC_BIT0))) {           /* Check MDC#33-BIT0   ITEM DISCOUNT CO/TL */
		// if item discount key is an Item Discount key (not Surcharge key)
		lSalesTotal += pTtlCas->ItemDisc.lAmount;
		if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#132-BIT2  */
            RptElement(TRN_ITMDISC_ADR, &pTtlCas->ItemDisc, NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
            lSubTotal += pTtlCas->ItemDisc.lAmount;
        }
    }

    if (!(CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0))) {           /* Check MDC#37-BIT0   */
		// if item discount key is an Item Discount key (not Surcharge key)
		lSalesTotal += pTtlCas->ModiDisc.lAmount;
		if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#132-BIT3  MODIFIED ITEM DISC*/
            RptElement(TRN_MODID_ADR, &pTtlCas->ModiDisc, NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
            lSubTotal += pTtlCas->ModiDisc.lAmount;
        }
    }

	{
		int i = 0;
		for (i = 0; MdcListItemDiscount[i].sMdcAddrTtl > 0; i++) {
			if (!(CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrTtl, MdcListItemDiscount[i].sBitTtl))) {
				// if item discount key is an Item Discount key (not Surcharge key)
				lSalesTotal += pTtlCas->ItemDiscExtra.TtlIAmount[i].lAmount;
				if (!CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrRpt, MdcListItemDiscount[i].sBitRpt)) {
					RptElement(MdcListItemDiscount[i].sTrnMnem, &pTtlCas->ItemDiscExtra.TtlIAmount[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
					lSubTotal += pTtlCas->ItemDiscExtra.TtlIAmount[i].lAmount;
				}
			}
		}
	}

	{
		int i;
		for (i = 0; MdcListTransDiscount[i].sMdcAddrTtl > 0; i++) {
			if (!(CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrTtl, MdcListTransDiscount[i].sBitTtl))) {
				// if item discount key is an Item Discount key (not Surcharge key)
				lSalesTotal += pTtlCas->RegDisc[i].lAmount;
				if (!CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrRpt, MdcListTransDiscount[i].sBitRpt)) {
					RptElement(MdcListTransDiscount[i].sTrnMnem, &pTtlCas->RegDisc[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
					lSubTotal += pTtlCas->RegDisc[i].lAmount;
				}
			}
		}
	}

    if (!CliParaMDCCheck(MDC_RPTCAS21_ADR, ODD_MDC_BIT3)) {          /* Check MDC#151-BIT3  */
        RptElement(TRN_COMBCPN_ADR, &pTtlCas->Coupon,                /* Combination Coupon */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->Coupon.lAmount;
    }
    lSalesTotal += pTtlCas->Coupon.lAmount;

    if (!CliParaMDCCheck(MDC_RPTCAS32_ADR, EVEN_MDC_BIT1)) {          /* Check MDC#151-BIT3  */
        RptElement(TRN_VCPN_ADR, &pTtlCas->aUPCCoupon[0],             /* UPC Coupon 2172 */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->aUPCCoupon[0].lAmount;
    }
    lSalesTotal += pTtlCas->aUPCCoupon[0].lAmount;

    if (!CliParaMDCCheck(MDC_RPTCAS3_ADR, ODD_MDC_BIT2)) {          /* Check MDC#133-BIT2  */
        RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                  /* SUB TOTAL       */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                               /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS3_ADR, ODD_MDC_BIT3)) {          /* Check MDC#133-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */ 
    }                                                               /*                 */

    if (!CliParaMDCCheck(MDC_RPTCAS4_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#134-BIT0  */
        RptElement(TRN_PO_ADR, &pTtlCas->PaidOut, NULL13DIGITS,     /* PAID OUT CO/TL  */
                   0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->PaidOut.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS4_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#134-BIT1  */
        RptElement(TRN_TIPPO_ADR, &pTtlCas->TipsPaid,               /* TIPS PAID OUT CO/TL */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->TipsPaid.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS4_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#134-BIT2  */
        RptElement(TRN_RA_ADR, &pTtlCas->RecvAcount, NULL13DIGITS,  /* RECEIVED ON ACCOUNT */
                   0, CLASS_RPTREGFIN_PRTTTLCNT,0);                 /*  CO/TL              */
        lSubTotal += pTtlCas->RecvAcount.lAmount;                           
    }
    if (!CliParaMDCCheck(MDC_RPTCAS4_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#134-BIT3  */
        RptElement(TRN_CHRGTIP_ADR, &pTtlCas->ChargeTips[0],        /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[0].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS24_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#154-BIT0  */
        RptElement(TRN_CHRGTIP2_ADR, &pTtlCas->ChargeTips[1],       /* CHARGE TIPS 2 CO/TL, V3.3   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[1].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS24_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#154-BIT1  */
        RptElement(TRN_CHRGTIP3_ADR, &pTtlCas->ChargeTips[2],       /* CHARGE TIPS 3 CO/TL, V3.3   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[2].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS27_ADR, ODD_MDC_BIT0)) {         /* Check MDC#345-BIT0  */
        RptElement(TRN_LOAN_ADR, &pTtlCas->Loan,                    /* Loan CO/TL, Saratoga */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->Loan.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS27_ADR, ODD_MDC_BIT1)) {         /* Check MDC#345-BIT1  */
        RptElement(TRN_PICKUP_ADR, &pTtlCas->Pickup,                /* Pickup CO/TL, Saratoga */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->Pickup.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS17_ADR, ODD_MDC_BIT3)) {         /* Check MDC#147-BIT3  */
        if (!(CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1))) {      /* Check MDC# 27-BIT1 */
            RptElement(TRN_HASHDEPT_ADR, &pTtlCas->HashDepartment,    /* HASH DEPARTMENT */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL          */
            lSubTotal += pTtlCas->HashDepartment.lAmount; 
        }
    }
    if (!(CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1))) {          /* Check MDC# 27-BIT1 */
        lSalesTotal += pTtlCas->HashDepartment.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS32_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#350-BIT0  */
        RptElement(TRN_FSCRD_ADR, &pTtlCas->FoodStampCredit,        /* FS Credit, Saratoga */
                   NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT,0);  /*  CO/TL              */
        lSubTotal += pTtlCas->FoodStampCredit.lAmount; 
    }
    if (!CliParaMDCCheck(MDC_RPTCAS23_ADR, ODD_MDC_BIT2)) {         /* Check MDC#153-BIT2  */
        RptElement(TRN_CKTO_ADR, &pTtlCas->CheckTransTo,            /* CHECK TRANSFERRED TO, V3.3 */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);  /*   CO/TL         */
        lSubTotal += pTtlCas->CheckTransTo.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS23_ADR, ODD_MDC_BIT3)) {         /* Check MDC#153-BIT3  */
        RptElement(TRN_CKFRM_ADR, &pTtlCas->CheckTransFrom,         /* CHKECK TRANSFERRD FROM, V3.3 */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);  /*  CO/TL         */
        lSubTotal += pTtlCas->CheckTransFrom.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS5_ADR, ODD_MDC_BIT0)) {          /* Check MDC#135-BIT0  */
        RptElement(TRN_TTLR_ADR, NULL, lSubTotal,                   /* TOTAL           */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                               /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS5_ADR, ODD_MDC_BIT1)) {          /* Check MDC#135-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */

    lSubTotal = 0L;
    if (!CliParaMDCCheck(MDC_RPTCAS5_ADR, ODD_MDC_BIT2)) {          /* Check MDC#135-BIT2  */
        RptElement(TRN_TEND1_ADR, &pTtlCas->CashTender.Total,       /* CASH CO/TL      */ 
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->CashTender.Total.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS29_ADR, ODD_MDC_BIT0)) {         /* MDC 347, Saratoga */
        RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
            pTtlCas->CashTender.lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS5_ADR, ODD_MDC_BIT3)) {          /* Check MDC#135-BIT3  */
        RptElement(TRN_TEND2_ADR, &pTtlCas->CheckTender.Total,      /* CHECK CO/TL     */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->CheckTender.Total.lAmount; 
    }
    if (!CliParaMDCCheck(MDC_RPTCAS29_ADR, ODD_MDC_BIT1)) {         /* MDC 347, Saratoga */
        RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
            pTtlCas->CheckTender.lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS6_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#136-BIT0  */
        RptElement(TRN_TEND3_ADR, &pTtlCas->ChargeTender.Total,     /* CHARGE CO/TL    */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        lSubTotal += pTtlCas->ChargeTender.Total.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS29_ADR, ODD_MDC_BIT2)) {         /* MDC 347, Saratoga */
        RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
            pTtlCas->ChargeTender.lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
    }

    /*----- Get 2-BYTE from Address of MDC #135 to Address of MDC #138 -----*/
    usMDCData = usMDCHand = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS5_ADR - 1)/2, &usReturnLen);
    usMDCData >>= 5;                            /* Shift right 5 bit */

    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCHand), 2, (MDC_RPTCAS29_ADR - 1)/2, &usReturnLen);
    usMDCHand >>= 3;

    for (uchNo = 0; uchNo < 5; uchNo++) {       /* MISC.#1 to #5 CO/TL */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_TEND4_ADR + uchNo, &pTtlCas->MiscTender[uchNo].Total,
                        NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);    
            lSubTotal += pTtlCas->MiscTender[uchNo].Total.lAmount;
        }
        if (!(usMDCHand & 0x01)) {              /* Saratoga */
            RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS, pTtlCas->MiscTender[uchNo].lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
        }
        usMDCData >>= 1;                        /* Shift right 2 bit each time */
        usMDCHand >>= 1;                        /* Shift right 2 bit each time */

        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                           /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }
    }

    /*----- Get 2-BYTE from Address of MDC #151 to Address of MDC #152 -----*/
    usMDCData = usMDCHand = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS21_ADR - 1)/2, &usReturnLen);

    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCHand), 2, (MDC_RPTCAS31_ADR - 1)/2, &usReturnLen);

    for (uchNo = 0; uchNo < 3; uchNo++) {                           /* MISC.#6 to #8 CO/TL */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_TEND9_ADR + uchNo, &pTtlCas->MiscTender[uchNo + 5].Total, NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlCas->MiscTender[uchNo + 5].Total.lAmount;
        }
        if (!(usMDCHand & 0x01)) {              /* Saratoga */
            RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS, pTtlCas->MiscTender[uchNo + 5].lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
        }
        usMDCData >>= 1;                        /* Shift right 2 bit each time */
        usMDCHand >>= 1;                        /* Shift right 2 bit each time */
    }

	//TENDERS 12 -20 
	usMDCData = usMDCHand = 0;

    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS38_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 2;                        /* Shift right 4 bit*/
	
	  for (uchNo = 0; uchNo < 9; uchNo++) {                           /* MISC.#6 to #8 CO/TL */
		if (!(usMDCData & 0x01)) {
            RptElement(TRN_TENDER12_ADR + uchNo, &pTtlCas->MiscTender[uchNo + 8].Total, NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlCas->MiscTender[uchNo + 8].Total.lAmount;
		} 
		switch (uchNo)
		{
			case 0:
				usMDCAddress = MDC_RPTCAS26_ADR;
				uchMDCMaskData = EVEN_MDC_BIT1;
				break;
			case 1:
				usMDCAddress = MDC_RPTCAS26_ADR;
				uchMDCMaskData = EVEN_MDC_BIT2;
				break;
			case 2:
				usMDCAddress = MDC_RPTCAS26_ADR;
				uchMDCMaskData = EVEN_MDC_BIT3;
				break;
			case 3:
				usMDCAddress = MDC_RPTCAS35_ADR;
				uchMDCMaskData = EVEN_MDC_BIT0;
				break;
			case 4:
				usMDCAddress = MDC_RPTCAS35_ADR;
				uchMDCMaskData = EVEN_MDC_BIT1;
				break;
			case 5:
				usMDCAddress = MDC_RPTCAS35_ADR;
				uchMDCMaskData = EVEN_MDC_BIT2;
				break;
			case 6:
				usMDCAddress = MDC_RPTCAS35_ADR;
				uchMDCMaskData = EVEN_MDC_BIT3;
				break;
			case 7:
				usMDCAddress = MDC_RPTCAS36_ADR;
				uchMDCMaskData = ODD_MDC_BIT0;
				break;
			case 8:
				usMDCAddress = MDC_RPTCAS36_ADR;
				uchMDCMaskData = ODD_MDC_BIT1;
				break;
			default:
				break;
		}

        if (!CliParaMDCCheck(usMDCAddress, uchMDCMaskData)) {              /* Saratoga */
            RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
                pTtlCas->MiscTender[uchNo + 8].lHandTotal, CLASS_RPTREGFIN_PRTDGG,0);
        }
		usMDCData >>= 1;
		usMDCHand >>= 1;
    }

    if (!CliParaMDCCheck(MDC_RPTCAS7_ADR, ODD_MDC_BIT2)) {          /* Check MDC#137-BIT2  */
        RptElement(TRN_STTLR_ADR, NULL, lSubTotal,                  /* SUB TOTAL       */
                   0L, CLASS_RPTREGFIN_PRTCGG,0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS7_ADR, ODD_MDC_BIT3)) {          /* Check MDC#137-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */

    if (!CliParaMDCCheck(MDC_RPTCAS8_ADR, EVEN_MDC_BIT0)) {         /* Check MDC#138-BIT0  */
		DCURRENCY    lOtherTtlTemp = 0;                                   /* store any Amount      */

		RptElement(TRN_FT1_ADR, NULL, NULL13DIGITS,                 /* FOREIGN CURRENCY 1 */
                  (LONG)pTtlCas->ForeignTotal[0].Total.sCounter,    /*  CO/TL     */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                      /* Print 1st-line */ 
        uchMinor = RptFCMDCChk(MDC_FC1_ADR);                        /* Check Format Type  */
        RptElement(0, NULL, NULL13DIGITS,                           /* Print 2nd-line    */
                   pTtlCas->ForeignTotal[0].Total.lAmount, uchMinor, 0);
        FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                /* Set Major for Ttl */
        FCTbl.uchAddress = CNV_NO1_ADR;                             /* Set Minor for Ttl */
        CliParaRead(&FCTbl);                                        /* Get Total         */
        RptFCRateCalc2(CNV_NO1_ADR, &lOtherTtlTemp, pTtlCas->ForeignTotal[0].Total.lAmount,  /* Calculate FC      */
                     FCTbl.ulForeignCurrency, RND_FOREIGN1_1_ADR);
        lSubTotal += lOtherTtlTemp;                                     /* Add to Sub Total  */
        RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,           /* Print 3rd-line    */ 
                   lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS33_ADR, ODD_MDC_BIT0)) {
        uchMinor = RptFCMDCChk(MDC_FC1_ADR);                        /* Check Format Type  */
        RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
            pTtlCas->ForeignTotal[0].lHandTotal, uchMinor, 0);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS8_ADR, EVEN_MDC_BIT1)) {         /* Check MDC#138-BIT1  */
		DCURRENCY lOtherTtlTemp = 0;

        RptElement(TRN_FT2_ADR, NULL, NULL13DIGITS,                 /* FOREIGN CURRENCY 2 */
                  (LONG)pTtlCas->ForeignTotal[1].Total.sCounter,    /*  CO/TL     */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                      /* Print 1st-line */ 
        uchMinor = RptFCMDCChk(MDC_FC2_ADR);                        /* Check Format Type  */
        RptElement(0, NULL, NULL13DIGITS,                           /* Print 2nd-line    */
                   pTtlCas->ForeignTotal[1].Total.lAmount, uchMinor,0);
        FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                /* Set Major for Ttl */
        FCTbl.uchAddress = CNV_NO2_ADR;                             /* Set Minor for Ttl */
        CliParaRead(&FCTbl);                                        /* Get Total         */
        RptFCRateCalc2(CNV_NO2_ADR, &lOtherTtlTemp, pTtlCas->ForeignTotal[1].Total.lAmount,  /* Calculate FC      */
                     FCTbl.ulForeignCurrency, RND_FOREIGN2_2_ADR);
        lSubTotal += lOtherTtlTemp;                                     /* Add to Sub Total  */
        RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,           /* Print 3rd-line    */
               lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG,0);                   
    }
    if (!CliParaMDCCheck(MDC_RPTCAS33_ADR, ODD_MDC_BIT1)) {
        uchMinor = RptFCMDCChk(MDC_FC2_ADR);                        /* Check Format Type  */
        RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
            pTtlCas->ForeignTotal[1].lHandTotal, uchMinor, 0);
    }
    /* --- Saratoga --- */
    usMDCData = usMDCHand = 0;
    CliParaAllRead(CLASS_PARAMDC, (UCHAR *)&usMDCData, 2, (MDC_RPTCAS27_ADR - 1)/2, &usReturnLen);
    CliParaAllRead(CLASS_PARAMDC, (UCHAR *)&usMDCHand, 2, (MDC_RPTCAS33_ADR - 1)/2, &usReturnLen);
    usMDCData >>= 2;
    usMDCHand >>= 2;

    for (uchNo = 0; uchNo < 6; uchNo++) {
        if (! (usMDCData & 0x01)) {
			DCURRENCY lOtherTtlTemp = 0;

            RptElement(TRN_FT3_ADR + uchNo, NULL, NULL13DIGITS,/* FOREIGN CURRENCY 2 */
                  (LONG)pTtlCas->ForeignTotal[uchNo+2].Total.sCounter,  /*  CO/TL     */ 
                   CLASS_RPTREGFIN_PRTCNT, 0);                          /* Print 1st-line */ 
            uchMinor = RptFCMDCChk((USHORT)(MDC_FC3_ADR + uchNo));      /* Check Format Type  */
            RptElement(0, NULL, NULL13DIGITS,                           /* Print 2nd-line    */
                   pTtlCas->ForeignTotal[uchNo+2].Total.lAmount, uchMinor,0);
            FCTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                /* Set Major for Ttl */
            FCTbl.uchAddress = CNV_NO3_ADR + uchNo;                     /* Set Minor for Ttl */
            CliParaRead(&FCTbl);                                        /* Get Total         */
            uchRnd = (UCHAR)(uchNo * 2 + RND_FOREIGN3_2_ADR);
            RptFCRateCalc2((UCHAR)(CNV_NO3_ADR + uchNo), &lOtherTtlTemp, 
                pTtlCas->ForeignTotal[uchNo+2].Total.lAmount, 
                FCTbl.ulForeignCurrency, uchRnd);
            lSubTotal += lOtherTtlTemp;                                 /* Add to Sub Total  */
            RptElement(TRN_FT_EQUIVALENT, NULL, NULL13DIGITS,           /* Print 3rd-line    */
               lOtherTtlTemp, CLASS_RPTREGFIN_PRTDGG, 0);                   
        }
        if (!(usMDCHand & 0x01)) {                                      /* Saratoga */
            RptElement(TRN_ONHAND_ADR, NULL, NULL13DIGITS,              /* on Hand  */
                pTtlCas->ForeignTotal[uchNo+2].lHandTotal, uchMinor, 0);
        }
        usMDCData >>= 1;                        /* Shift right 2 bit each time */
        usMDCHand >>= 1;                        /* Shift right 2 bit each time */
    }

    /*------------------------------------------*/
    /*  No-Assume Tender Affects to Total       */
    /*  Print out No-Assume Tender              */
    /*------------------------------------------*/

    /*----- Get 2-BYTE from Address of MDC #139 to Address of MDC #142 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS9_ADR - 1)/2, &usReturnLen);

    for (uchNo = 0; uchNo < 6; uchNo++) {       /* TOTAL#3 TO #8 CO/TL */
        if (!(usMDCData & 0x01) && RptNoAssume(uchNo)) {
            RptElement((TRN_TTL3_ADR + uchNo), &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        }
        usMDCData >>= 1;                        /* Shift right 1 bit in each time */
    } /*----- End of for Loop -----*/

    if (!CliParaMDCCheck(MDC_RPTCAS8_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#138-BIT2  */
        RptElement(TRN_TTLR_ADR, NULL, lSubTotal,                   /* TOTAL         */   
                   0L, CLASS_RPTREGFIN_PRTCGG,0);                              
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?        */
        return(RPT_ABORTED);                                                 
    }                                                               /*                   */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS8_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#138-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed              */
    }                                                               /*                   */

    /*------------------------------------------*/
    /* Not print No-Assume Tender               */  
    /*------------------------------------------*/

    /*----- Get 2-BYTE from Address of MDC #139 to Address of MDC #142 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS9_ADR - 1)/2, &usReturnLen);

	// We map Total Key #3 through Total Key #8 to the Service Total Array elements 0 thru 5.
	// We map Total Key #10 through Total Key #20 to the Service Total Array elements 6 thru 16.
	// Total #1 is Cash, Total #2 and Total #9 are both food stamp.  None of these have service totals.
    memset(Totalcount, 0, sizeof(Totalcount));                      /* Initialize buffer  */
    for (uchNo = 0; uchNo < 6; uchNo++) {                           /* TOTAL#3 TO #8 CO/TL */
        if (!(usMDCData & 0x01) && !RptNoAssume(uchNo)) {
            RptElement((TRN_TTL3_ADR+uchNo), &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
        }
        usMDCData >>= 1;                    /* Shift right 1 bit each time */
        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                           /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }
    } /*----- End of for Loop -----*/    

	/*PDINU TOTALS ADDED HERE*/
	//Total 10
    if (!CliParaMDCCheck(MDC_RPTCAS31_ADR, ODD_MDC_BIT3)) {             
		RptElement(TRN_TOTAL10_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 11
    if (!CliParaMDCCheck(MDC_RPTCAS32_ADR, EVEN_MDC_BIT2)) {              
		RptElement(TRN_TOTAL11_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 12
    if (!CliParaMDCCheck(MDC_RPTCAS32_ADR, EVEN_MDC_BIT3)) {             
		RptElement(TRN_TOTAL12_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 13
    if (!CliParaMDCCheck(MDC_RPTCAS36_ADR, ODD_MDC_BIT2)) {              
		RptElement(TRN_TOTAL13_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 14
    if (!CliParaMDCCheck(MDC_RPTCAS36_ADR, ODD_MDC_BIT3)) {
		RptElement(TRN_TOTAL14_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 15
    if (!CliParaMDCCheck(MDC_RPTCAS37_ADR, EVEN_MDC_BIT0)) {              
		RptElement(TRN_TOTAL15_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 16
    if (!CliParaMDCCheck(MDC_RPTCAS37_ADR, EVEN_MDC_BIT1)) {              
		RptElement(TRN_TOTAL16_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 17
    if (!CliParaMDCCheck(MDC_RPTCAS37_ADR, EVEN_MDC_BIT2)) {              
		RptElement(TRN_TOTAL17_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 18
    if (!CliParaMDCCheck(MDC_RPTCAS37_ADR, EVEN_MDC_BIT3)) {              
		RptElement(TRN_TOTAL18_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 19
    if (!CliParaMDCCheck(MDC_RPTCAS38_ADR, ODD_MDC_BIT0)) {              
		RptElement(TRN_TOTAL19_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }
	++uchNo;    // increment to the next total key provisioning
	//Total 20
    if (!CliParaMDCCheck(MDC_RPTCAS38_ADR, ODD_MDC_BIT1)) {              
		RptElement(TRN_TOTAL20_ADR, &pTtlCas->ServiceTotal[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ServiceTotal[uchNo].lAmount;
        RptSvscalc(uchNo, &pTtlCas->ServiceTotal[uchNo], Totalcount);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS10_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#140-BIT2  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */ 
    }                                                               /*                 */

    /*----- Get 2-BYTE from Address of MDC #139 to Address of MDC #142 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS9_ADR - 1)/2, &usReturnLen);

    usMDCData >>= 7;                            /* Shift right 7 bit */

    lSubTotal = 0L;
    for (uchNo = 0; uchNo < 3; uchNo++) {       /* SERVICE TOTAL #1 TO #3 */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_SERV1_ADR + uchNo, &Totalcount[uchNo], NULL13DIGITS, 0L, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += Totalcount[uchNo].lAmount;
        }
        usMDCData >>= 1;                        /* Shift right 1 bit each time */
    }
    if (!CliParaMDCCheck(MDC_RPTCAS11_ADR, ODD_MDC_BIT2)) {         /* Check MDC#141-BIT2  */
        RptElement(TRN_STTLR_ADR, NULL, lSubTotal,               /* SUB TOTAL        */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?       */
        return(RPT_ABORTED);
    }                                                               /*                  */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS11_ADR, ODD_MDC_BIT3)) {         /* Check MDC#141-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed             */
    }                                                               /*                  */

    /*----- Get 2-BYTE from Address of MDC #141 to Address of MDC #144 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS11_ADR - 1)/2, &usReturnLen);
    usMDCData >>= 4;                            /* Shift right 4 bit */

    d13OtherTtl =0L;
    for (uchNo = 0; uchNo < 3; uchNo++) {       /* ADD CHECK #1 TO #3 CO/TL */
        if (!(usMDCData & 0x01)) {
            RptElement(TRN_ADCK1_ADR + uchNo, &pTtlCas->AddCheckTotal[uchNo], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
            d13OtherTtl += pTtlCas->AddCheckTotal[uchNo].lAmount;
        }
        usMDCData >>= 1;                        /* Shift right 1 bit each time */
    }
    if (!CliParaMDCCheck(MDC_RPTCAS12_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#142-BIT3  */
        RptElement(TRN_STTLR_ADR, NULL, d13OtherTtl,               /* SUB TOTAL       */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS13_ADR, ODD_MDC_BIT0)) {         /* Check MDC#143-BIT0  */
        RptElement(TRN_OTSTN_ADR, NULL, lSubTotal - d13OtherTtl,      /* OUTSTANDING TOTAL  */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                               /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS13_ADR, ODD_MDC_BIT1)) {         /* Check MDC#143-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */

	lSubTotal = 0L;
    if (!CliParaMDCCheck(MDC_RPTCAS13_ADR, ODD_MDC_BIT2)) {         /* Check MDC#143-BIT2  */
        RptElement(TRN_CHRGTIP_ADR, &pTtlCas->ChargeTips[0],        /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[0].lAmount;
    }
    /* V3.3 */
    if (!CliParaMDCCheck(MDC_RPTCAS25_ADR, ODD_MDC_BIT3)) {         /* Check MDC#155-BIT3  */
        RptElement(TRN_CHRGTIP2_ADR, &pTtlCas->ChargeTips[1],       /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[1].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS26_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#156-BIT0  */
        RptElement(TRN_CHRGTIP3_ADR, &pTtlCas->ChargeTips[2],       /* CHARGE TIPS CO/TL   */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->ChargeTips[2].lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS13_ADR, ODD_MDC_BIT3)) {         /* Check MDC#143-BIT3  */
        RptElement(TRN_TIPPO_ADR, &pTtlCas->TipsPaid,               /* TIPS PAID OUT CO/TL */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
        lSubTotal += pTtlCas->TipsPaid.lAmount;
    }
    if (!CliParaMDCCheck(MDC_RPTCAS14_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#144-BIT0  */
        RptElement(TRN_OTSTN_ADR, NULL, lSubTotal,               /* OUTSTANDING TOTAL   */ 
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);                                                       
    }                                                       
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    /* V3.3 */
    if (!CliParaMDCCheck(MDC_RPTCAS24_ADR, EVEN_MDC_BIT3)) {            /* Check MDC#154-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                        /* Feed                */
    }                                                                   /*                     */
    if (!CliParaMDCCheck(MDC_RPTCAS25_ADR, ODD_MDC_BIT0)) {             /* Check MDC#155-BIT0  */
        RptElement(TRN_DECTIP_ADR, &pTtlCas->DeclaredTips,              /* DECLARED TIPS CO/TL */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }

	{
		DCURRENCY lSubTotalTemp = 0;

		if (!CliParaMDCCheck(MDC_RPTCAS25_ADR, ODD_MDC_BIT1)) {             /* Check MDC#155-BIT1  */
			DCURRENCY lSalesTotalTemp = lSalesTotal;

			Data.uchMajorClass =  CLASS_PARADISCTBL;
			Data.uchAddress = RATE_CALT_ADR;
			CliParaRead(&Data);
			if (CliParaMDCCheck(MDC_WAITER_ADR, ODD_MDC_BIT0)) {            /* Check MDC#97-BIT0  */
				RflRateCalc1(&lSubTotalTemp, lSalesTotalTemp, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);
			} else {
				RflRateCalc1(&lSubTotalTemp, lSalesTotalTemp - pTtlCas->lConsTax, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);  /* daily gross includes exclude tax */
			}
			RptElement(TRN_CALC_ADR, NULL, NULL13DIGITS,                    /* N% CALCULATE TIPS TL */ 
					   lSubTotalTemp, CLASS_RPTREGFIN_PRTDGG, 0);
		}
		if (!CliParaMDCCheck(MDC_RPTCAS25_ADR, ODD_MDC_BIT2)) {             /* Check MDC#155-BIT2  */
			RptElement(TRN_OTSTN_ADR, NULL, NULL13DIGITS,                   /* OUTSTANDING TOTAL   */ 
					  (pTtlCas->DeclaredTips.lAmount - lSubTotalTemp),
					  CLASS_RPTREGFIN_PRTDGG, 0);
		}
	}

    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?         */
        return(RPT_ABORTED);
    }                                                                   /*                     */
    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS14_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#144-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }

    /* VAT report, V3.3 */
    if ((CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) &&                 /* Check MDC#-BIT0     */
        (CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0))) {             /* Check MDC#11-BIT0     */
        /* VAT #n APL   SZZZZZZZ9.99    * VAT #n APLLICABLE TOTAL
           VAT #n       SZZZZZZZ9.99    * VAT #n TOTAL            
                             .
                             .
                            X 3                                   */

        if (RptRegCasVAT(pTtlCas) == RPT_ABORTED) {
            return (RPT_ABORTED);
        }
        /* Service Total of VAT, V3.3 */
        if (!CliParaMDCCheck(MDC_RPTCAS44_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#118-BIT0  */
            RptElement(TRN_TX4_ADR, NULL, NULL13DIGITS,                 /* TAX #4 TL           */
                       pTtlCas->Taxable[3].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
        }
    } else {
        /* US Tax Report */
        /*----- Get 2-BYTE from Address of MDC #115 to Address of MDC #118 -----*/
        usMDCData = 0;
		CliParaMDCCheck(MDC_RPTCAS41_ADR, EVEN_MDC_BIT1);
        CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS41_ADR - 1)/2, &usReturnLen);
        usMDCData >>= 6;                                /* Shift right 6 bit */
        fchFS = EVEN_MDC_BIT1;

        d13OtherTtl = 0L;
        for (uchNo = 0; uchNo < 3; uchNo++) {                           
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TXBL1_ADR + 3 * uchNo, NULL,            /* Taxable #1 to #3 TL */    
                           pTtlCas->Taxable[uchNo].mlTaxableAmount, 0, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            usMDCData >>= 1;                            /* Shift right 1 bit in each time */
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TX1_ADR + 3 * uchNo, NULL,              /* TAX #1 to #3 TL     */
                           NULL13DIGITS, pTtlCas->Taxable[uchNo].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            usMDCData >>= 1;                            /* Shift right 1 bit in each time */
            if (!(usMDCData & 0x01)) {
                RptElement(TRN_TXEXM1_ADR + 3 * uchNo, NULL,           /* TAX EXEMPTED #1 to #3 TL */
                           NULL13DIGITS, pTtlCas->Taxable[uchNo].lTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTCAS45_ADR, fchFS)) {                /* Check MDC#357-BIT2-4  */
                RptElement(TRN_FSTXEXM1_ADR + uchNo, NULL,         /* FSTAX EXEMPTED #1 to #3 TL */
                   NULL13DIGITS, pTtlCas->Taxable[uchNo].lFSTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
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
            if (!CliParaMDCCheck(MDC_RPTCAS43_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#117-BIT3  */
                RptElement(TRN_TXBL4_ADR, NULL,                             /* Taxable #4 TL       */
                           pTtlCas->Taxable[3].mlTaxableAmount, 0, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTCAS44_ADR, ODD_MDC_BIT0)) {        /* Check MDC#118-BIT0  */
                RptElement(TRN_TX4_ADR, NULL, NULL13DIGITS,                 /* TAX #4 TL           */
                           pTtlCas->Taxable[3].lTaxAmount, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTCAS44_ADR, ODD_MDC_BIT1)) {        /* Check MDC#118-BIT1  */
                RptElement(TRN_TXEXM4_ADR, NULL, NULL13DIGITS,              /* TAX EXEMPTED #4 TL  */ 
                           pTtlCas->Taxable[3].lTaxExempt, CLASS_RPTREGFIN_PRTDGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTCAS44_ADR, ODD_MDC_BIT2)) {        /* Check MDC#118-BIT2  */
                RptElement(TRN_NONTX_ADR, NULL, pTtlCas->NonTaxable,       /* NON-Taxable TL     */
                           0L, CLASS_RPTREGFIN_PRTCGG, 0);
            }
            if (!CliParaMDCCheck(MDC_RPTCAS44_ADR, ODD_MDC_BIT3)) {        /* Check MDC#118-BIT3  */
                d13OtherTtl += pTtlCas->Taxable[1].lTaxAmount;
                d13OtherTtl += pTtlCas->Taxable[2].lTaxAmount;
                d13OtherTtl += pTtlCas->Taxable[3].lTaxAmount;
                RptElement(TRN_PST_ADR, NULL, d13OtherTtl,                 /* PST ALL TAX         */
                           0L, CLASS_RPTREGFIN_PRTCGG, 0);
            }
        }
    }

	if (!CliParaMDCCheck(MDC_RPTCAS14_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#144-BIT2  */
        RptElement(TRN_CONSTX_ADR, NULL, NULL13DIGITS,              /* CONSOLI. TAX    */
                   pTtlCas->lConsTax, CLASS_RPTREGFIN_PRTDGG, 0);
    }
    /* VAT report, V3.3 */
    if ((CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) &&                /* Check MDC#-BIT0     */
        (CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0))) {            /* Check MDC#11-BIT0     */
        if (!CliParaMDCCheck(MDC_RPTCAS24_ADR, EVEN_MDC_BIT2)) {       /* Check MDC#154-BIT2  */
            RptElement(TRN_TX4_ADR, NULL, NULL13DIGITS,                /* VAT SERVICE TOTAL   */
                       pTtlCas->lVATServiceTotal, CLASS_RPTREGFIN_PRTDGG, 0);
        }
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                               /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS14_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#144-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */

    /*----- Get 2-BYTE from Address of MDC #145 to Address of MDC #148 -----*/
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS15_ADR - 1)/2, &usReturnLen);

    lSubTotal = d13OtherTtl = 0L;
    Data.uchMajorClass = CLASS_PARADISCTBL;                          
    for (uchNo = 0; uchNo < 8; uchNo++) {       /* BONUS TL #1 to #8 CO/TL  */
        if (!(usMDCData & 0x01)) {
            Data.uchAddress = (UCHAR)(RATE_BONUS1_ADR+uchNo);
            CliParaRead(&Data);
            RptElement((UCHAR)(TRN_BNS1_ADR+uchNo), 
                       &pTtlCas->Bonus[uchNo], NULL13DIGITS, 0,
                       CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlCas->Bonus[uchNo].lAmount;
            RflRateCalc1(&lBonusAmt, pTtlCas->Bonus[uchNo].lAmount, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);
            d13OtherTtl += lBonusAmt;                                 /* BONUS % BONUS TOTAL # x BONUS % */
            RptElement(0, NULL, NULL13DIGITS, lBonusAmt,
                       CLASS_RPTREGFIN_PRTBONUS, Data.uchDiscRate);
        }
        usMDCData >>= 1;                        /* Shift right 1 bit each time */
        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                           /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

    }
//Begin SR 157 Bonus total to 100 increase
    usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC, ((UCHAR *)&usMDCData), 2, (MDC_RPTCAS46_ADR - 1)/2, &usReturnLen);

	uchBonusIndex = 0;
	uchBonusIndex += uchNo;
    Data.uchMajorClass = CLASS_PARADISCTBL;                          
    for (uchNo = 0; uchNo < 92; uchNo++, uchBonusIndex++) {       /* BONUS TL #1 to #8 CO/TL  */
        if (!(usMDCData & 0x01)) {
            Data.uchAddress = (UCHAR)(RATE_BONUS9_ADR+uchNo);
            CliParaRead(&Data);
            RptElement(TRN_BNS_9_ADR + uchNo, 
                       &pTtlCas->Bonus[uchBonusIndex], NULL13DIGITS, 0,
                       CLASS_RPTREGFIN_PRTTTLCNT, 0);
            lSubTotal += pTtlCas->Bonus[uchBonusIndex].lAmount;
            RflRateCalc1(&lBonusAmt, pTtlCas->Bonus[uchBonusIndex].lAmount, Data.uchDiscRate * RFL_DISC_RATE_MUL, RND_RND_NORMAL);
            d13OtherTtl += lBonusAmt;                                 /* BONUS % BONUS TOTAL # x BONUS % */
            RptElement(0, NULL, NULL13DIGITS, lBonusAmt,
                       CLASS_RPTREGFIN_PRTBONUS, Data.uchDiscRate);
        }
		switch(uchNo)
		{
			case 0:   //After Bonus 9	NEXT MDC-> 463C
			case 1:   //After Bonus 10	NEXT MDC-> 463B
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

//end SR 157 Bonus total to 100 increase

    if (!CliParaMDCCheck(MDC_RPTCAS17_ADR, ODD_MDC_BIT0)) {         /* Check MDC#147-BIT0  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }
    if (!CliParaMDCCheck(MDC_RPTCAS17_ADR, ODD_MDC_BIT1)) {         /* Check MDC#147-BIT1  */
        RptElement(TRN_TTLR_ADR, NULL, lSubTotal,                /* ALL BONUS TOTAL */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
        RptElement(0, NULL, d13OtherTtl,                           /* ALL BONUS RATE  */
                   0L, CLASS_RPTREGFIN_PRTCGG, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS17_ADR, ODD_MDC_BIT2)) {         /* Check MDC#147-BIT2  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    }                                                               /*                 */

	// ---    Print out the Surcharge totals section of the report    ---
	lSubTotal = 0;      // subtotal for Surcharge Totals
	usMDCData = 0;      // indicates if we have any Surcharge totals printed.

	if (CliParaMDCCheck(MDC_MODID13_ADR, ODD_MDC_BIT0)) {       /* Check MDC#33-BIT0   */
		// if item discount key is an Item Surcharge key
		lSubTotal += pTtlCas->ItemDisc.lAmount;
		if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT2)) {         /* Check MDC#132-BIT2  */
			usMDCData |= 1;          // indicate we have printed out a Surcharge total
			RptElement(TRN_ITMDISC_ADR, &pTtlCas->ItemDisc,          /* ITEM DISCOUNT CO/TL */
                      NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        }
    }
    if (CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0)) {       /* Check MDC#37-BIT0   */
		// if item discount key is an Item Surcharge key
		lSubTotal += pTtlCas->ModiDisc.lAmount;
		if (!CliParaMDCCheck(MDC_RPTCAS2_ADR, EVEN_MDC_BIT3)) {         /* Check MDC#132-BIT3  */
			usMDCData |= 1;          // indicate we have printed out a Surcharge total
			RptElement(TRN_MODID_ADR, &pTtlCas->ModiDisc,             /* MODIFIED ITEM DISC. */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
        }
    }

	{

		int i = 0;
		for (i = 0; MdcListItemDiscount[i].sMdcAddrTtl > 0; i++) {
			if ((CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrTtl, MdcListItemDiscount[i].sBitTtl))) {
				lSubTotal += pTtlCas->ItemDiscExtra.TtlIAmount[i].lAmount;
				if (!CliParaMDCCheckField(MdcListItemDiscount[i].sMdcAddrRpt, MdcListItemDiscount[i].sBitRpt)) {
					usMDCData |= 1;          // indicate we have printed out a Surcharge total
					RptElement(MdcListItemDiscount[i].sTrnMnem, &pTtlCas->ItemDiscExtra.TtlIAmount[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
				}
			}
		}
	}

	{
		int i;
		for (i = 0; MdcListTransDiscount[i].sMdcAddrTtl > 0; i++) {
			if ((CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrTtl, MdcListTransDiscount[i].sBitTtl))) {
				// if transaction discount key is a Transaction Surcharge key
				lSubTotal += pTtlCas->RegDisc[i].lAmount;
				if (!CliParaMDCCheck(MdcListTransDiscount[i].sMdcAddrRpt, MdcListTransDiscount[i].sBitRpt)) {
					usMDCData |= 2;          // indicate we have printed out a Surcharge total
					RptElement(MdcListTransDiscount[i].sTrnMnem, &pTtlCas->RegDisc[i], NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0); /*  CO/TL              */
				}
			}
		}
	}

#if 0
	// if the total for Surcharges is desired then print it out here.
	// not sure if this really provides much information or not.
	if (!CliParaMDCCheck(MDC_RPTCAS3_ADR, ODD_MDC_BIT2) && usMDCData != 0) {          /* Check MDC#133-BIT2  */
		RptElement(TRN_TTLR_ADR, NULL, lSubTotal,                  /* TOTAL for Surcharges       */
			0L, CLASS_RPTREGFIN_PRTCGG, 0);
	}

	if (!CliParaMDCCheck(MDC_RPTCAS3_ADR, ODD_MDC_BIT3) && usMDCData != 0) {          /* Check MDC#133-BIT3  */
		RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
	}                                                               /*                 */
#endif
	if (!CliParaMDCCheck(MDC_RPTCAS17_ADR, ODD_MDC_BIT3)) {         /* Check MDC#147-BIT3  */
        if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {         /* Check MDC#27-BIT1   */
            RptElement(TRN_HASHDEPT_ADR, &pTtlCas->HashDepartment,  /* HASH DEPARTMENT CO/TL */
                       NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT,0);
        }
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                               /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS18_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#148-BIT0  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed                  */
    }                                                               /*                       */

    if (!CliParaMDCCheck(MDC_RPTCAS18_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#148-BIT1  */
        RptElement(TRN_TRACAN_ADR, &pTtlCas->TransCancel,           /* TRAN.CANCLE CO/TL     */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS18_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#148-BIT2  */
        RptElement(TRN_MVD_ADR, &pTtlCas->MiscVoid,                 /* MISC.VOID CO/TL         */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                               /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS18_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#148-BIT3  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed                  */
    }                                                               /*                       */

    if (!CliParaMDCCheck(MDC_RPTCAS19_ADR, ODD_MDC_BIT0)) {         /* Check MDC#149-BIT0  */
        RptElement(TRN_AUD_ADR, &pTtlCas->Audaction,                /* AUDACTION CO/TL       */
                   NULL13DIGITS, 0, CLASS_RPTREGFIN_PRTTTLCNT, 0);
    }
    if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?            */
        return(RPT_ABORTED);
    }                                                               /*                       */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }

    if (!CliParaMDCCheck(MDC_RPTCAS19_ADR, ODD_MDC_BIT1)) {         /* Check MDC#149-BIT1  */
        RptFeed(RPT_DEFALTFEED);                                    /* Feed                  */
    }                                                               /*                       */

    if (!CliParaMDCCheck(MDC_RPTCAS19_ADR, ODD_MDC_BIT2)) {         /* Check MDC#149-BIT2  */
        RptElement(TRN_NOSALE_ADR, NULL, NULL13DIGITS,              /* NO SALE COUNTER       */
                   (LONG)(pTtlCas->sNoSaleCount), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS19_ADR, ODD_MDC_BIT3)) {         /* Check MDC#149-BIT3  */
        RptElement(TRN_ITMPROCO_ADR, NULL, NULL13DIGITS,            /* ITEM PRODUCTIVITY CO  */
                   pTtlCas->lItemProductivityCount, CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS20_ADR, EVEN_MDC_BIT0)) {        /* Check MDC#150-BIT0  */
        RptElement(TRN_PSN_ADR, NULL, NULL13DIGITS,                 /* NO. OF PERSON         */ 
                   (LONG)(pTtlCas->sNoOfPerson), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS20_ADR, EVEN_MDC_BIT1)) {        /* Check MDC#150-BIT1  */
        RptElement(TRN_CUST_ADR, NULL, NULL13DIGITS,                /* CUSTOMER COUNTER      */ 
                   (LONG)(pTtlCas->sCustomerCount), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS20_ADR, EVEN_MDC_BIT2)) {        /* Check MDC#150-BIT2  */
        RptElement(TRN_CHKOPN_ADR, NULL, NULL13DIGITS,              /* NO. OF CHECKS OPENED  */
                   (LONG)(pTtlCas->sNoOfChkOpen), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS20_ADR, EVEN_MDC_BIT3)) {        /* Check MDC#150-BIT3  */
        RptElement(TRN_CHKCLS_ADR, NULL, NULL13DIGITS,              /* NO. OF CHECKS CLOSED  */
                   (LONG)(pTtlCas->sNoOfChkClose), CLASS_RPTREGFIN_PRTNO, 0);
    }
    if (!CliParaMDCCheck(MDC_RPTCAS23_ADR, ODD_MDC_BIT0)) {        /* Check MDC#153-BIT0  */
        RptElement(TRN_POST_ADR, NULL, NULL13DIGITS,               /* POST RECEIPT CO. R3.1 */
                   (LONG)(pTtlCas->usPostRecCo), CLASS_RPTREGFIN_PRTNO, 0);
    }
                                                                          
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCashierReset(UCHAR uchType, UCHAR uchFunc, USHORT usCasNumber) 
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
*       Check Individual, All or Open reset. V3.3
*===============================================================================
*/
SHORT RptCashierReset(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchFunc, ULONG ulCasNumber) 
{                   
    SHORT sReturn;                
                                            
    if (uchType == RPT_IND_RESET) {                             /* Individual Reset Case */         
        sReturn = RptCashierIndReset(uchMinorClass, uchFunc, ulCasNumber);  
    } else if ((uchType == RPT_ALL_RESET) ||
               (uchType == RPT_EOD_ALLRESET) ||                 /* All Reset Case */
               (uchType == RPT_PTD_ALLRESET)) { 
        sReturn = RptCashierAllReset(uchMinorClass, uchType, uchFunc); 
    } else {                                                    /* Reset Open Status Case */
        sReturn = RptCashierOpenReset(ulCasNumber);
    }
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCashierIndReset(UCHAR uchFunc, USHORT usCasNumber) 
*               
*       Input:  UCHAR  uchFunc      : RPT_PRT_RESET
*                                   : RPT_ONLY_RESET
*               UCHAR  usCasNumber  : Cashier Number
*      Output:  nothing
*
**     Return: TTL_SUCCESS             : Successful 
*              TTL_FIL                 : not success  
*              TTL_NOTINFILE           : not in file
*              TTL_NOT_ISSUED          : not issued yet
*              TTL_LOCKED              : locked
*
**  Description:         
*       Reset for Cashier Individula Report A/C 19 Funcriotn Type 2 or 3.
*       If A/C 19 Function Type 3 - Only Reset, print Reset Header.  If not, 
*       Reset Header is printed in Read Function.
*       Check SerTtlTotalReset.  If Error occurs, printing Cas Name , ID and
*       Error Number.
*       Print out Trailer.
*===============================================================================
*/

SHORT RptCashierIndReset(UCHAR uchMinorClass, UCHAR uchFunc, ULONG ulCashierNo) 
{
    SHORT           sReturn;
	TTLCASHIER      TtlCas = {0};
    UCHAR           uchSpecMnemo, uchSpecReset;

    TtlCas.uchMajorClass = CLASS_TTLCASHIER;                    /* Set Major      */
    TtlCas.ulCashierNumber = ulCashierNo;                       /* Set Cashier No */

    /* Set Reset Daily or PTD Address */
	// Translate the Read Totals minor class into the appropriate Reset Totals minor class.
    if (uchMinorClass == CLASS_TTLCURDAY) {
        TtlCas.uchMinorClass = CLASS_TTLSAVDAY;
        uchSpecReset = SPCO_EODRST_ADR;                         /* Set Daily Reset Counter */
        uchSpecMnemo = SPC_DAIRST_ADR;                          /* Set Daily Special Mnemonics */
    } else {
        TtlCas.uchMinorClass = CLASS_TTLSAVPTD;
        uchSpecReset = SPCO_PTDRST_ADR;                         /* Set PTD Reset Counter */
        uchSpecMnemo = SPC_PTDRST_ADR;                          /* Set PTD Special Mnemonics */
    }

    /* Check Saved buffer already print out */
    if (((sReturn = SerTtlIssuedCheck(&TtlCas, TTL_NOTRESET)) != TTL_ISSUED) &&
         (sReturn != TTL_NOTINFILE)) {                          /* Not Issued yet ? */ 
        return(TtlConvertError(sReturn));                       /* Error */
    }

    /* Case 1: reset OK, reset co up. Case 2 : total = 0, error */
	// Use the original Read Totals minor class because TtlTresetCas() requires CLASS_TTLCURDAY
	// or CLASS_TTLCURPTD which it will then translate. Sigh.
    TtlCas.uchMinorClass = uchMinorClass;                         /* Set Minor      */
    if ((sReturn = SerTtlTotalReset(&TtlCas, 0)) == TTL_SUCCESS) {  /* Reset OK ? */
        MaintIncreSpcCo(uchSpecReset);                              /* Reset counter up    */                      
    }

    /* Print Header Name */
    if ((uchFunc == RPT_ONLY_RESET) ||                               /* Reset Only Case */
       ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE))) {   /* Reset Error ? */
		RPTCASHIER      RptCashier = {0};
                                                
        if (uchFunc == RPT_ONLY_RESET) {
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive Counter */
		}
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class    */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_CAS_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                         /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        RPT_IND_RESET,                          /* Report Type         */
                        AC_CASH_RESET_RPT,                      /* A/C Number          */
                        uchFunc,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit   */                                     
        uchRptOpeCount |= RPT_HEADER_PRINT;

        RptPrtCasTrng2(TtlCas.ulCashierNumber);                 /* Call PrtPrtTrng()     */ 
        RptCashier.uchMajorClass = CLASS_RPTCASHIER;            /* Set Major           */
        RptCashier.uchMinorClass = CLASS_RPTCASHIER_RESET;
        RptCashier.ulCashierNumber = TtlCas.ulCashierNumber;    /* Set Cashier No      */
        RptCashierGetName (RptCashier.aszCashMnemo, RptCashier.ulCashierNumber); /* Copy Mnemo */
        RptCashier.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;

        PrtPrintItem(NULL, &RptCashier);                        /* print out           */

        if ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {  /* Reset Error   */
            RptPrtError(TtlConvertError(sReturn));              /* Convert and Print Error */
        }
        if (uchFunc == RPT_ONLY_RESET) {
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Trailer       */
        }
    }                                                           /* MDC 90 BIT2 is 1 ? */
    if (!(CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT2))) {      /*  (clear secret code) */
        SerCasSecretClr(ulCashierNo);                           /* Ignore Error handling */
    }
    return(SUCCESS);                                            /* Return SUCCESS      */
}

/*
*=============================================================================
**  Synopsis: SHORT RptCashierAllReset( UCHAR uchType,
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
*       Reset for Cashier All Reset A/C 19 Function Type 2 or 3, Report Type 1.
*       Check Error before Header printing.  Classify Only Reset or Reset & Report.
*===============================================================================
*/
SHORT RptCashierAllReset(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchFunc) 
{
    SHORT           sReturn, sNo;
    ULONG           ausResetBuffer[MAX_NO_CASH];

    /* Check Existance of the cashier file */
    if ((sNo = SerCasAllIdRead(sizeof(ausResetBuffer), ausResetBuffer)) < 0) { 
        return(CasConvertError(sNo));                               /* Error   */
    }

	if (uchMinorClass == 0) {
		SHORT  sCasNo;

		for (sCasNo = 0; sCasNo < sNo; sCasNo++) { 
			sReturn = RptCashierOpenReset(ausResetBuffer[sCasNo]);
		}
		sReturn = SUCCESS;
	} else {
		TTLCASHIER      TtlCas = {0};

		TtlCas.uchMajorClass = CLASS_TTLCASHIER;                        /* Set Major      */
		/* V3.3 */
		// Translate the Read Totals minor class into the appropriate Reset Totals minor class
		// for SerTtlIssuedCheck().
		if (uchMinorClass == CLASS_TTLCURDAY) {
			TtlCas.uchMinorClass = CLASS_TTLSAVDAY;
		} else if (uchMinorClass == CLASS_TTLCURPTD) {
			TtlCas.uchMinorClass = CLASS_TTLSAVPTD;
		}

		/* Check Saved buffer already print out */
		if (((sReturn = SerTtlIssuedCheck(&TtlCas, TTL_NOTRESET)) != TTL_ISSUED) && (sReturn != TTL_NOTINFILE)) {                              /* Not Issued yet ? */ 
			return(TtlConvertError(sReturn));                           /* Error   */
		}
		if (uchFunc == RPT_ONLY_RESET) {                                /* Only Reset Report ? */
			sReturn = RptCasAllOnlyReset(uchMinorClass, uchType, uchFunc, sNo, ausResetBuffer);
		} else {    /*----- RPT_PRT_RESET -----*/                       /* Reset and Report ? */
			sReturn = RptCasAllRstReport(uchMinorClass, uchType, uchFunc, sNo, ausResetBuffer);
		}
	}
    return(sReturn);                                                /* Return SUCCESS      */
}

/*
*=============================================================================
**  Synopsis: SHORT RptCasAllOnlyReset(UCHAR uchType,
*                                      SHORT sNo,
*                                      USHORT *ausResetBuffer)
*
*       Input:  UCHAR   uchType
*               SHORT   sNo
*               USHORT  *ausResetBuffer
*
*      Output:  *ausResetBuffer
*
**  Return: SUCCESS         : Reset Success 
*         : RPT_RESET_FAIL  : Reset Failed in EOD Case
*
**  Description:  
*       Only Reset All cashier files.
*       Prepare data for Reset Header and print out.
*       Print out Reset Header.  Control Reset Fail Error - if SerTtlTotalReset fails,
*       print Cashier name & ID and Error Number. The Secret code is cler
*       if MDC #17 BIT1 is On.  
*       Return Error to Caller When REset Failed in EOD Case.
*===============================================================================
*/
SHORT  RptCasAllOnlyReset(UCHAR uchMinorClass,
                          UCHAR uchType,
                          UCHAR uchFunc,
                          SHORT sNo,
                          ULONG *ausResetBuffer)
{
    UCHAR       uchRptType, uchResetType, uchSpecMnemo, uchSpecReset;
    USHORT      usFlag;
    SHORT       sCasNo, sReturn; 

    /* Set Special Daily or PTD Mnemonic , Print Header */
    if (uchMinorClass == CLASS_TTLCURDAY) {
        uchSpecMnemo = SPC_DAIRST_ADR;                          /* Set Daily Special Mnemonics */
    } else {
        uchSpecMnemo = SPC_PTDRST_ADR;                          /* Set PTD Special Mnemonics */
    }

   /* Print Header Name */
    uchRptType = uchType;
    uchResetType = uchFunc;
    if (uchType == RPT_EOD_ALLRESET || uchType == RPT_PTD_ALLRESET) {
        uchRptType = RPT_ALL_RESET;
        uchResetType = 0;
    }
                                                
    if ((uchType != RPT_EOD_ALLRESET) && (uchType != RPT_PTD_ALLRESET)) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                           /* Count Up Consecutive Counter */
    }
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                          /* Minor Data Class    */              
                    RPT_ACT_ADR,                                    /* Report Name Address */
                    RPT_CAS_ADR,                                    /* Report Name Address */
                    uchSpecMnemo,                               /* Special Mnemonics Address */
                    RPT_RESET_ADR,                                  /* Report Name Address */
                    uchRptType,                                     /* Report Type         */
                    AC_CASH_RESET_RPT,                              /* A/C Number          */
                    uchResetType,                                   /* Reset Type          */
                    PRT_RECEIPT|PRT_JOURNAL);           /* Print Control Bit   */

    usFlag = 0;
    for (sCasNo = 0; sCasNo < sNo; sCasNo++) {                      /* Get All Cashier files */
		RPTCASHIER  RptCashier = {0};
		TTLCASHIER  TtlCas = {0};

		TtlCas.uchMajorClass = CLASS_TTLCASHIER;                        /* Set Major      */
		TtlCas.uchMinorClass = uchMinorClass;                           /* Set Minor      */
        TtlCas.ulCashierNumber = ausResetBuffer[sCasNo];            /* Set Cashier No        */
        if ((sReturn = SerTtlTotalReset(&TtlCas, 0)) == TTL_SUCCESS) {  /* Reset OK?         */ 
            usFlag = 1;
        } 
		RptCashier.uchMajorClass = CLASS_RPTCASHIER;                    /* Set Major             */
        RptCashier.uchMinorClass = CLASS_RPTCASHIER_RESET;
        RptPrtCasTrng2(ausResetBuffer[sCasNo]);                     /* Call PrtPrtTrng()     */ 
        RptCashier.ulCashierNumber = ausResetBuffer[sCasNo];        /* Set Cashier No        */
        RptCashierGetName (RptCashier.aszCashMnemo, RptCashier.ulCashierNumber); /* copy Mnemo */
        RptCashier.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &RptCashier);                              /* print out           */
        if ((sReturn !=TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {  /* if Reset Error */
            RptPrtError(TtlConvertError(sReturn));                    /* Print Error # */
        }
        if (!(CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT2))) {      /* Clear Secret code ? */
            SerCasSecretClr(ausResetBuffer[sCasNo]);                /* Ignore Error handle */                                 
        }     
    }
    if (((uchType != RPT_EOD_ALLRESET) && (uchType != RPT_PTD_ALLRESET)) && (usFlag == 1)) {
        if (uchMinorClass == CLASS_TTLCURDAY) {
            uchSpecReset = SPCO_EODRST_ADR;                     /* Set Daily Reset Counter */
        } else {
            uchSpecReset = SPCO_PTDRST_ADR;                     /* Set PTD Reset Counter */
        }
        MaintIncreSpcCo(uchSpecReset);                          /* Count Up Reset Counter */
    }
    if ((uchType != RPT_EOD_ALLRESET) && (uchType != RPT_PTD_ALLRESET)) {
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Trailer       */
    } else {                                                        /* One Line Feed for EOD */
        RptFeed(RPT_DEFALTFEED);                                        /* line feed     */ 
    }                                                               
    if (((uchType == RPT_EOD_ALLRESET) || (uchType == RPT_PTD_ALLRESET)) && (usFlag != 1) && sNo ) {  /* Reset Failed in EOD/PTD Case */
        return(RPT_RESET_FAIL);                                                 
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCasAllRstReport(UCHAR  uchType,
*                                      UCHAR  uchFunc, 
*                                      SHORT  sNo,
*                                      USHORT *ausResetBuffer) 
*               
*       Input:  UCHAR   uchType         : RPT_ALL_RESET
*                                       : RPT_EODALL_RESET
*               UCHAR   uchFunc         : RPT_PRT_RESET
*               SHORT   sNo             :
*               USHORT  *ausResetBuffer :
*
*      Output:  USHORT  *ausResetBuffer
*
**  Return: SUCCESS         : Reset Success 
*         : RPT_RESET_FAIL  : Reset Failed in EOD Case
*
**  Description:  
*       Reset All cashier files.
*       Prepare data for Reset Header and print out.
*       Print out Reset Header.
*       Control Reset Fail Error - if SerTtlTotalReset fails,
*       print Cashier name & ID and Error Number. The Secret code is clear
*       if MDC #17 BIT1 is On.
*===============================================================================
*/ 
SHORT  RptCasAllRstReport(UCHAR  uchMinorClass,
                          UCHAR  uchType,
                          UCHAR  uchFunc,
                          SHORT  sNo,
                          ULONG *ausResetBuffer)
{
    UCHAR       uchResetType, uchRptType, uchSpecMnemo, uchSpecReset;
    USHORT      usFlag; 
    SHORT       sCasNo, sReturn;

    uchRptType = uchType;
    uchResetType = uchFunc;
    if ((uchType == RPT_EOD_ALLRESET) || (uchType == RPT_PTD_ALLRESET)) {
        uchRptType = RPT_ALL_RESET;
        uchResetType = 0;
    }

    /* Set Special Daily or PTD Mnemonic , Print Header */
    if (uchMinorClass == CLASS_TTLCURDAY) {
        uchSpecMnemo = SPC_DAIRST_ADR;                          /* Set Daily Special Mnemonics */
    } else {
        uchSpecMnemo = SPC_PTDRST_ADR;                          /* Set PTD Special Mnemonics */
    }

    for (sCasNo = 0; sCasNo < sNo; sCasNo++) { 
		TTLCASHIER  TtlCas = {0};

		TtlCas.uchMajorClass = CLASS_TTLCASHIER;                        /* Set Major      */
		TtlCas.uchMinorClass = uchMinorClass;                           /* Set Minor      */
        TtlCas.ulCashierNumber = ausResetBuffer[sCasNo];
        if ((sReturn = SerTtlTotalReset(&TtlCas, 0)) == TTL_SUCCESS) {  /* Reset OK?       */
            usFlag = 1;
        } else if (sReturn != TTL_NOTINFILE) {                          /* if Reset Fail */
			RPTCASHIER  RptCashier = {0};

            if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation Case */                   
                MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class    */              
                                RPT_ACT_ADR,                            /* Report Name Address */
                                RPT_CAS_ADR,                            /* Report Name Address */
                                uchSpecMnemo,                         /* Special Mnemonics Address */
                                RPT_RESET_ADR,                          /* Report Name Address */
                                uchRptType,                             /* Report Type         */
                                AC_CASH_RESET_RPT,                      /* A/C Number          */
                                uchResetType,                           /* Reset Type          */
                                PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit   */
                uchRptOpeCount |= RPT_HEADER_PRINT;
            }
            RptPrtCasTrng2(ausResetBuffer[sCasNo]);                     /* Call PrtPrtTrng()     */ 
            RptCashier.ulCashierNumber = ausResetBuffer[sCasNo];        /* Set Cashier No        */
            RptCashierGetName (RptCashier.aszCashMnemo, RptCashier.ulCashierNumber); /* copy Mnemo */
			RptCashier.uchMajorClass = CLASS_RPTCASHIER;                    /* Set Major             */
            RptCashier.uchMinorClass = CLASS_RPTCASHIER_RESET;
            RptCashier.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
            PrtPrintItem(NULL, &RptCashier);                            /* print out           */
            if ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {   /* Reset Error */
                RptPrtError(TtlConvertError(sReturn));                      /* Convert and Print Error */
            }

        }
        if (!(CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT2))) {          /* Address 90 Bit2 is 0 ? */
            SerCasSecretClr(ausResetBuffer[sCasNo]);      
        }
    }
    if (((uchType != RPT_EOD_ALLRESET) && (uchType != RPT_PTD_ALLRESET)) && (usFlag == 1))   {
        if (uchMinorClass == CLASS_TTLCURDAY) {
            uchSpecReset = SPCO_EODRST_ADR;                     /* Set Daily Reset Counter */
        } else {
            uchSpecReset = SPCO_PTDRST_ADR;                     /* Set PTD Reset Counter */
        }
        MaintIncreSpcCo(uchSpecReset);                          /* Count Up Reset Counter */
    }
    if (((uchType == RPT_EOD_ALLRESET) || (uchType == RPT_PTD_ALLRESET)) && (usFlag != 1) && sNo ) {  /* Reset Failed in EOD/PTD Case */
        return(RPT_RESET_FAIL);                                                 
    }
    return(SUCCESS);
}
                                                                                          
/*                                                                                        
*=============================================================================            
**  Synopsis: SHORT RptCashierOpenReset(USHORT usCasNumber)                               
*                                                                                         
*       Input:  USHORT  usCasNumber  : Cashier Number
*      Output:  nothing
*
**  Return: PERFORM             : Closed input cashier  
*           NO_SIGNIN           : Nobody signs in
*
**  Description:  
*       Reset opened chaier file in an emergency.
*       Close cashier file compulsorily.
*       Print Header, cashier name & number and Trailer.
*===============================================================================
*/
SHORT RptCashierOpenReset(ULONG ulCasNumber) 
{
	SHORT           sStatus;
	RPTCASHIER      RptCashier = {0};

    sStatus = SerCasClose(ulCasNumber);
	if (sStatus != CAS_PERFORM)
		return sStatus;

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_PARA,                 /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    0,                                      /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address       */
                    0,                                      /* Not Print                 */
                    AC_EMG_CASHFL_CLS,                      /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */ 
                            
    RptPrtCasTrng2(ulCasNumber);                            /* Call RptPrtCasTrng() */
    RptCashier.uchMajorClass = CLASS_RPTOPENCAS;            /* Set Major                 */
    RptCashier.ulCashierNumber = ulCasNumber;               /* Set Cashier No            */
    RptCashierGetName (RptCashier.aszCashMnemo, RptCashier.ulCashierNumber); /* Copy Mnemo */
    RptCashier.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
    PrtPrintItem(NULL, &RptCashier);                        /* print out                 */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer             */
    return(SUCCESS);                                        /* Error                     */
}

/*
*=============================================================================
**  Synopsis: VOID RptPrtCasTrng1(USHORT usCasNo) 
*               
*       Input:  USHORT  usCasNo
*      Output:  nothing
*
**  Return:  nothing
*
**  Description:  
*            Print Special Mnemonics for Cashier Training if Cashier is Training
*            and MDC bit, print on R/J, is on.
*===============================================================================
*/
VOID RptPrtCasTrng1(ULONG ulCasNo)
{
	CASIF           CasIf = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
         return;
    }

    /*----- Print Training Mnemonic if only Training Cashier -----*/
    CasIf.ulCashierNo = ulCasNo;
    if (SerCasIndRead(&CasIf) == CAS_PERFORM && (CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER)) {
		RPTREGFIN       RegFin = {0};

		RegFin.uchMajorClass = CLASS_RPTREGFIN;
        RegFin.uchMinorClass = CLASS_RPTREGFIN_MNEMO;
        RegFin.usPrintControl = usRptPrintStatus;
        RflGetSpecMnem (RegFin.aszTransMnemo, SPC_TRNGID_ADR);

        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RegFin, 0); /* display on LCD          */ 
            RegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RegFin);
    }
}

/*
*=============================================================================
**  Synopsis: VOID RptPrtCasTrng2(USHORT usCasNo) 
*               
*       Input:  USHORT  usCasNo
*      Output:  nothing
*
**  Return:  nothing
*
**  Description:  
*            This function is called when A/C 19 Individual Only Reset Type, 
*            Reset & Report - Reset Error Type, Reset Only - Reset Error Type.  
*            Print Special Mnemonics for Cashier Training if Cashier is Training.
*            Training Mnemonics is printed on R/J (It doesn't depent on MDC BIT).
*===============================================================================
*/
VOID RptPrtCasTrng2(ULONG ulCasNo)
{
	CASIF           CasIf = {0};

    /*----- Print Training Mnemonic if only Training Cashier -----*/
    CasIf.ulCashierNo = ulCasNo;
    if (SerCasIndRead(&CasIf) == CAS_PERFORM && (CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER) ) {
		MAINTTRANS      MaintTrans = {0};

		MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
        MaintTrans.usPrtControl = PRT_JOURNAL | PRT_RECEIPT;
        RflGetSpecMnem (MaintTrans.aszTransMnemo, SPC_TRNGID_ADR);
        PrtPrintItem(NULL, &MaintTrans);
    }
}

/*
*=============================================================================
**  Synopsis: SHORT RptCashierLock(UCHAR uchType, USHORT usCasNumber) 
*               
*       Input:  UCHAR       uchType  : RPT_IND_LOCK
*                                    : RPT_ALL_LOCK
*               USHORT      usCasNumber 
*      Output:  nothing
*
**     Return:  CAS_PERFORM 
*               CAS_DURING_SIGNIN
*               CAS_ALREADY_LOCK
*               CAS_NOT_IN_FILE              
*
**  Description:  
*       Lock certain cashier file for reset report.
*===============================================================================
*/
SHORT RptCashierLock(UCHAR uchType, ULONG ulCasNumber) 
{
    SHORT  sReturn;  
                                                                     
    if (uchType == RPT_IND_LOCK) {                                       
        sReturn = SerCasIndLock(ulCasNumber);                       /* Lock Complete */
    } else {                                                        /* All Lock Case */
        sReturn = SerCasAllLock();                                  /* Lock Complete */
    }
    if (sReturn == CAS_OTHER_LOCK) {
        sReturn = LDT_LOCK_ADR;
    } else if (sReturn == CAS_NOT_IN_FILE) {
        sReturn = LDT_NTINFL_ADR;
    } else if (sReturn != CAS_PERFORM) {
        sReturn = LDT_PROHBT_ADR;
    }
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: VOID RptCashierUnLock(UCHAR uchType, USHORT usCasNumber) 
*               
*       Input:  UCHAR  uchType
*      Output:  nothing
*
**     Return:  nothing 
*                 
**  Description:  
*       Unlock certain cashier file.
*===============================================================================
*/

VOID RptCashierUnLock(UCHAR uchType, ULONG ulCasNumber) 
{
    
    if (uchType == RPT_IND_UNLOCK) {                             /* Individual UnLock Case */
        SerCasIndUnLock(ulCasNumber);                        
    } else {                                                    /* All Lock Case */
        SerCasAllUnLock();                                  
    }
}

/*
*=============================================================================
**  Synopsis: SHORT RptChkWaiterNo(USHORT usWaiterNumber) 
*               
*       Input:  USHORT usWaiterNumber
*      Output:  nothing
*
**  Return: LDT_KEYOVER_ADR
*           SUCCESS
*
**  Description:  
*       Check Waiter Exist.
*===============================================================================
*/
SHORT RptChkCashierNo(DCURRENCY dcCasNumber) 
{
	ULONG           ulCasNumber = (ULONG)dcCasNumber;
    SHORT           sReturn;
	CASIF           Arg = {0};

    /* Check 0 Input */
    if (!ulCasNumber) {                                        
        return(LDT_KEYOVER_ADR);
    }

    /* Check Cashier Exist */
    Arg.ulCashierNo = ulCasNumber;                        
    if((sReturn = SerCasIndRead(&Arg)) != CAS_PERFORM) {
        return(CasConvertError(sReturn));
    }

    return(SUCCESS);
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
SHORT RptRegCasVAT(TTLCASHIER *pTtlData)
{
    static USHORT FARCONST
            auchMDCVatTotalAdr[] = {MDC_RPTCAS41_ADR,     /* VAT #1 TOTAL 446*/
                                    MDC_RPTCAS42_ADR,     /* VAT #2 TOTAL 447*/
                                    MDC_RPTCAS43_ADR},    /* VAT #3 TOTAL 448*/

            auchMDCVatApAdr[] = {MDC_RPTCAS41_ADR,    /* VAT #1 APPLICABLE TOTAL 446 */
                                 MDC_RPTCAS42_ADR,    /* VAT #2 APPLICABLE TOTAL 447*/
                                 MDC_RPTCAS43_ADR};   /* VAT #3 APPLICABLE TOTAL 448*/



	static UCHAR FARCONST
            auchTransAprAddr[] = {TRN_TXBL1_ADR,      /* TRANSACTION MNEMONICS VAT #1 APPLICABLE */
                                  TRN_TXBL2_ADR,      /* TRANSACTION MNEMONICS VAT #2 APPLICABLE */
                                  TRN_TXBL3_ADR},     /* TRANSACTION MNEMONICS VAT #3 APPLICABLE */
                                                 
            auchTransAddr[] = {TRN_TX1_ADR,           /* TRANSACTION MNEMONICS VAT #1 TOTAL */
                               TRN_TX2_ADR,           /* TRANSACTION MNEMONICS VAT #2 TOTAL */
                               TRN_TX3_ADR},          /* TRANSACTION MNEMONICS VAT #3 TOTAL */

            auchVatRateAddr[] = {TXRT_NO1_ADR,        /* VAT #1 RATE */
                                 TXRT_NO2_ADR,        /* VAT #2 RATE */
                                 TXRT_NO3_ADR},       /* VAT #3 RATE */

			auchMDCVatApBit[] = {EVEN_MDC_BIT2,        /* VAT #1 APPLICABLE TOTAL */
                                 ODD_MDC_BIT1,       /* VAT #2 APPLICABLE TOTAL */
                                 EVEN_MDC_BIT0},      /* VAT #3 APPLICABLE TOTAL */
			
			auchMDCVatTotalBit[] = {EVEN_MDC_BIT3,    /* VAT #1 TOTAL */
                                    ODD_MDC_BIT2,    /* VAT #2 TOTAL */
                                    EVEN_MDC_BIT1};    /* VAT #3 TOTAL */

    USHORT   usVATNo;       /* VAT number */
    D13DIGITS d13VatTtl;    /* store include vat total */

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


SHORT  ItemGenerateAc21Report (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE *fpFile, ULONG ulCashierNo)
{
	SHORT       sReturn;
	SHORT       sNo;
    ULONG       ausRcvBuffer[MAX_NO_CASH];

    if (!fpFile) return -1;

    /* EOD PTD Report Not Display Out MLD */
	// (uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY))
    
    if (uchType == RPT_IND_READ) {                              /* Ind. read report ? */
        sNo = 1;
        ausRcvBuffer[0] = ulCashierNo; /* Set cashier No */
    } else if ((uchType == RPT_ALL_READ) || (uchType == RPT_EOD_ALLREAD) || (uchType == RPT_PTD_ALLREAD)) {
        if ((sNo = SerCasAllIdRead(sizeof(ausRcvBuffer), ausRcvBuffer)) < 0) {
            return( CasConvertError(sNo) );
        }
    }
    else {
        NHPOS_ASSERT_TEXT(0, "**ERROR: uchType invalid call to ItemGenerateAc21Report().");
        return (CasConvertError(CAS_NOT_ALLOWED));
    }

	if (RptDescriptionGetStream()) {
        if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
            switch (uchMinorClass) {
            case CLASS_TTLCURDAY:
                fprintf(RptDescriptionGetStream(), "<h2>AC 21 Cashier Report - Current Daily Totals</h2>\n");
                break;
            case CLASS_TTLCURPTD:
                fprintf(RptDescriptionGetStream(), "<h2>AC 21 Cashier Report - Current Period To Day Totals</h2>\n");
                break;
            case CLASS_TTLSAVDAY:
                fprintf(RptDescriptionGetStream(), "<h2>AC 21 Cashier Report - Saved Daily Totals</h2>\n");
                break;
            case CLASS_TTLSAVPTD:
                fprintf(RptDescriptionGetStream(), "<h2>AC 21 Cashier Report - Saved Period To Day Totals</h2>\n");
                break;
            }
        }
        
        for (SHORT sCasNo = 0; sCasNo < sNo; sCasNo++) {
			TTLCASHIER  TtlCas = {0};                    /* Assign Register Financial Total Save Area */

            TtlCas.uchMajorClass = CLASS_TTLCASHIER;                            /* Set Major */
            TtlCas.uchMinorClass = uchMinorClass;                               /* Set Minor */
            TtlCas.ulCashierNumber = ausRcvBuffer[sCasNo];                      /* Set cashier No */
            sReturn = SerTtlTotalRead(&TtlCas);                                 /* Get Total */

            switch(sReturn) {
            case TTL_SUCCESS:
                               /* MDC 90 BIT3 is 1 (not print TL/CO are 0) */
                if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT3)) {             
                               /* Check Daily Gross Group Total is 0 (NO Sales) */                                                 
                    if (TtlStringCheck(&TtlCas.lDGGtotal, USRPTCASLENTTL, 0) == TTL_SUCCESS) {
                        if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)) {
                            SerTtlIssuedReset(&TtlCas, TTL_NOTRESET);           /* Set Issued Reset */
                        }                                                       /* Read Report ? */
                        continue;                                               /* continue */
                    }
                }
                break;

            case TTL_NOTINFILE:                                          /* MDC 90 BIT3 is 1 (not print TL/CO are 0) */
				// if this cashier has not actually been used during this period then there will not be
				// totals for this cashier so we will see Not in File (TTL_NOTINFILE) so we will just
				// report zero for every thing if MDC 90 indicates we should do so.
                if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT3)) {             
                    continue;                                            /* continue */
                }
                break;

            default:
                // RptPrtError( TtlConvertError(sReturn));                  /* Print converted Error Code */
                continue;
            }
            if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
                wchar_t  aszMnemo[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                wchar_t aszCashMnemo[PARA_CASHIER_LEN + 1] = { 0 };

                RptCashierGetName(aszCashMnemo, TtlCas.ulCashierNumber); /* Copy Mnemo */
                fprintf(RptDescriptionGetStream(), "<h3>Operator %d: %S</h3>\n", TtlCas.ulCashierNumber, aszCashMnemo);

                if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {  /* DD/MM/YY */
                    fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlCas.FromDate.usMDay, TtlCas.FromDate.usMonth,
                        TtlCas.FromDate.usYear, TtlCas.FromDate.usHour, TtlCas.FromDate.usMin);
                    fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlCas.ToDate.usMDay, TtlCas.ToDate.usMonth,
                        TtlCas.ToDate.usYear, TtlCas.ToDate.usHour, TtlCas.ToDate.usMin);
                }
                else {  /* MM/DD/YY */
                    fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlCas.FromDate.usMonth, TtlCas.FromDate.usMDay,
                        TtlCas.FromDate.usYear, TtlCas.FromDate.usHour, TtlCas.FromDate.usMin);
                    fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlCas.ToDate.usMonth, TtlCas.ToDate.usMDay,
                        TtlCas.ToDate.usYear, TtlCas.ToDate.usHour, TtlCas.ToDate.usMin);
                }

                fprintf(RptDescriptionGetStream(), "<table border=\"1\" cellpadding=\"8\">\n<tr><th>Name</th><th>Amount</th><th>Count</th></tr>\n");
            }
            RptCashierEdit(RptElementStream, uchMinorClass, &TtlCas);
            if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
                fprintf(RptDescriptionGetStream(), "</table>\n");
            }
        }
    }

    return SUCCESS;
}

/* --- End of Source File --- */