/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Loan Pickup Common Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 Application Program
* Program Name: MATLPCOM.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               MaintLoanPickupCasSignIn() : Cashier Sign In for Loan/Pickup
*               MaintLoanPickupDisp()      : Loan/Pickup Amount Display
*               MaintLoanPickupCasSignOut(): Cashier Sign Out for Loan/Pickup
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev. :   Name   : Description
* Feb-23-92 : 00.00.01 :  M.Ozawa   : initial
* Oct-15-96 : 02.00.00 : T.Yatuhasi : Change MaintLoanPickupErrorCorrect,
*                                     MaintLoanPickupDisp
*                                     for Foreign Currency #3-8
* Oct-15-96 : 02.00.00 : T.Yatuhasi : Change MaintLoanPickupDisp for Bug Fix
* Nov-30-99 : 01.00.00 : hrkato     : Saratoga
* Apr-04-15 : 02.02.01 : R.Chambers : source cleanup for AC 10/11, Connection Engine
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

/**
==============================================================================
;                      I N C L U D E     F I L E s
=============================================================================
**/

#include <tchar.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "pmg.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbstb.h"
#include "csstbpar.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csttl.h"
#include "csstbttl.h"
#include "csop.h"
#include "report.h"
#include "display.h"
#include "rfl.h"

#include "maintram.h"

/*
*======================================================================================
**   Synopsis:  SHORT MaintLoanPickupCasSignIn(MAINTLOANPICKUP *pData,USHORT usDrawer )
*
*       Input:  *pData          : Pointer to Structure for MAINTLOANPICKUP
*               usDrawer        : Drawer no for open
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Cashier Sign In for Loan/Pickup Module
*======================================================================================
*/
SHORT   MaintLoanPickupCasSignIn(MAINTLOANPICKUP *pData, USHORT usDrawer)
{
	CASIF       CasIf = {0};
    SHORT       sError;

    /* Save and Read Cashier Parameter */
    if (pData->ulCashierNo == 0 || pData->ulCashierNo > MAINT_CASH_MAX) {   /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

	TrnICPOpenFile();

    CasIf.ulCashierNo = pData->ulCashierNo;                                  /* Set Cashier No. */
    sError = CliCasOpenPickupLoan(&CasIf);                                   /* Cashier Sign In  R2.0 */
    if (sError != CAS_PERFORM) {
        CliCasClosePickupLoan(&CasIf);                                      /* Cashier Sign Out R2.0 */
        return(CasConvertError(sError));
    } else if ((CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER)) {             /* Check Traning */
        CliCasClosePickupLoan(&CasIf);                                      /* Cashier Sign Out R2.0 */
        return(LDT_KEYOVER_ADR);
    }

    memset(pData->auchCashierName, '\0', sizeof(pData->auchCashierName));
    _tcsncpy(pData->auchCashierName, CasIf.auchCashierName, PARA_CASHIER_LEN);

	MaintWork.LoanPickup = *pData;                                      // Save the Loan Pickup request data

    sError = CasOpenDrawer(&CasIf, usDrawer);                           /* Drawer open */
    if (sError != CAS_PERFORM) {
        CliCasClosePickupLoan(&CasIf);                                  /* Cashier Sign Out R2.0 */
        return(CasConvertError(sError));
    }

    return(SUCCESS);
}


/*
*======================================================================================
**   Synopsis:  SHORT MaintLoanPickupCasSignOut(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Cashier Sign Out for Loan/Pickup Module
*======================================================================================
*/
SHORT   MaintLoanPickupCasSignOut(MAINTLOANPICKUP *pData)
{
	CASIF           CasIf = {0};
    SHORT           sStatus;

    CasIf.ulCashierNo = MaintWork.LoanPickup.ulCashierNo;    // set the cashier number as MaintWork will get cleared in MaintFinExt.

    if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) {
		ITEMTRANSCLOSE  Close = {0};

        Close.uchMajorClass = CLASS_ITEMTRANSCLOSE;
        Close.uchMinorClass = (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN) ? CLASS_CLS_LOAN : CLASS_CLS_PICK;  /* 09/08/00 */
        TrnClose(&Close);

        while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
			USHORT   usStatus = TtlConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }

		if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT_CE)
			MaintFinExt(CLASS_MAINTTRAILER_PRTSUP, PRT_JOURNAL);            /* Execute Finalize Procedure, 09/08/00 */
		else
			MaintFinExt(CLASS_MAINTTRAILER_PRTSUP, (PRT_JOURNAL | PRT_RECEIPT));            /* Execute Finalize Procedure, 09/08/00 */
    }
    /* Close Cashier in this Module */
    CliCasClosePickupLoan(&CasIf);

    /* Close transaction file only if amount was set,   Saratoga */
    if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) {
		/* clear consolidation storage to avoid storage full error, V1.0.12 */
        TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEM | TRANI_CONSOLI);
        /* TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEM); */

		TrnICPCloseFile();   /* close item, cons. post rec. file */
		memset(&MaintWork, 0, sizeof(MAINTWORK));   /* clear self-work */
    }
    return(SUCCESS);
}


/*
*======================================================================================
**   Synopsis:  SHORT MaintLoanPickupAbort( VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Cashier Sign Out of Loan/Pickup, in case of abort
*======================================================================================
*/
SHORT   MaintLoanPickupAbort(VOID)
{

    if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) {
        /* Prohibit Abort, if Amount is entered */
        return(LDT_SEQERR_ADR);
    } else {
		CASIF   CasIf = {0};
        /* Close Cashier in this Module */
        CasIf.ulCashierNo = MaintWork.LoanPickup.ulCashierNo;
        CliCasClosePickupLoan(&CasIf);
    }

    return(SUCCESS);
}

/*
*======================================================================================
*
**   Synopsis:  SHORT MaintLoanErrorCorrect(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Loan Amount Error Correct
======================================================================================
*/
SHORT   MaintLoanPickupErrorCorrect(MAINTLOANPICKUP *pData)
{
	MAINTLOANPICKUP ECData = {0};
    ITEMAFFECTION   Affect = {0};
	SHORT sError;

    if (pData->uchMinorClass != CLASS_MAINTOTHER){
        return(LDT_SEQERR_ADR);
    }
    if (MaintWork.LoanPickup.usModifier & MAINT_MODIFIER_VOID) { /* prohibit e/c of void */
        return(LDT_SEQERR_ADR);
    }

    /* Recall Previous Loan Data from MaintWork */
    ECData.lAmount = MaintWork.LoanPickup.lAmount;
    ECData.lAmount *= -1;                                     /* Calculate E/C Amount */
    ECData.lForQty = MaintWork.LoanPickup.lForQty;
    ECData.lForQty *= -1;                                     /* Calculate E/C Amount */
    ECData.lUnitAmount *= -1;                                 /* Calculate E/C Amount */
    ECData.lNativeAmount = MaintWork.LoanPickup.lNativeAmount;
    ECData.lNativeAmount *= -1;                                     /* Calculate E/C Amount */

    /* Set E/C Data */
    MaintWork.LoanPickup.usModifier |= MAINT_MODIFIER_EC;
    MaintWork.LoanPickup.lForQty = ECData.lForQty;
    MaintWork.LoanPickup.lNativeAmount = ECData.lNativeAmount;
    MaintWork.LoanPickup.lAmount = ECData.lAmount;

    switch (MaintWork.LoanPickup.uchMinorClass) {
    case CLASS_MAINTFOREIGN1:   /* add pickup total by native currency */
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        MaintWork.LoanPickup.lTotal += ECData.lNativeAmount;
        break;

    default:
        MaintWork.LoanPickup.lTotal += ECData.lAmount;
    }

    /* Display Amount */
    MaintLoanPickupDisp(&(MaintWork.LoanPickup));
    /* Print Amount */
    TrnLoanPickup(&(MaintWork.LoanPickup));    /* Transfer Data to Transaction Module */

    Affect.uchMajorClass = CLASS_ITEMAFFECTION;
    if (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTPICKUP) {   /* recover all pickup amount */
        MaintAllPickupAmountSet2(&MaintWork.LoanPickup);
        Affect.uchMinorClass = CLASS_PICKAFFECT;
    } else {
        Affect.uchMinorClass = CLASS_LOANAFFECT;
    }
    Affect.uchTotalID    = MaintWork.LoanPickup.uchMinorClass;
    Affect.lAmount       = MaintWork.LoanPickup.lAmount;
    Affect.lService[0]   = MaintWork.LoanPickup.lNativeAmount;
    
    /* check storage full, V1.0.12 */
    sError = TrnLoanPickup(&Affect);
    return(sError);
}

/*
*===========================================================================
** Synopsis:    VOID MaintLoanPickupDisp( MAINTLOANPICKUP *pData )
*
*     Input:    *pData          : pointer to structure for MAINTREDISP
*    Output:    Nothing
*
** Return:      Nothing
*
*
** Description: Display Loan / Pickup Amount (Tender, Qty, Void)
*===========================================================================
*/
struct _tagMAPTENDER2MNEMONIC {
	UCHAR  uchMinorClass;
	UCHAR  uchAddress;
} myTender2MnemonicLookupTable [] = {
    {CLASS_MAINTTENDER1, TRN_TEND1_ADR},
    {CLASS_MAINTTENDER2, TRN_TEND2_ADR},
    {CLASS_MAINTTENDER3, TRN_TEND3_ADR},
    {CLASS_MAINTTENDER4, TRN_TEND4_ADR},
    {CLASS_MAINTTENDER5, TRN_TEND5_ADR},
    {CLASS_MAINTTENDER6, TRN_TEND6_ADR},
    {CLASS_MAINTTENDER7, TRN_TEND7_ADR},
    {CLASS_MAINTTENDER8, TRN_TEND8_ADR},
    {CLASS_MAINTTENDER9, TRN_TEND9_ADR},
    {CLASS_MAINTTENDER10, TRN_TEND10_ADR},
    {CLASS_MAINTTENDER11, TRN_TEND11_ADR},
    {CLASS_MAINTTENDER12, TRN_TENDER12_ADR},
    {CLASS_MAINTTENDER13, TRN_TENDER13_ADR},
    {CLASS_MAINTTENDER14, TRN_TENDER14_ADR},
    {CLASS_MAINTTENDER15, TRN_TENDER15_ADR},
    {CLASS_MAINTTENDER16, TRN_TENDER16_ADR},
    {CLASS_MAINTTENDER17, TRN_TENDER17_ADR},
    {CLASS_MAINTTENDER18, TRN_TENDER18_ADR},
    {CLASS_MAINTTENDER19, TRN_TENDER19_ADR},
    {CLASS_MAINTTENDER20, TRN_TENDER20_ADR},
    {CLASS_MAINTFOREIGN1, TRN_FT1_ADR},
    {CLASS_MAINTFOREIGN2, TRN_FT2_ADR},
    {CLASS_MAINTFOREIGN3, TRN_FT3_ADR},
    {CLASS_MAINTFOREIGN4, TRN_FT4_ADR},
    {CLASS_MAINTFOREIGN5, TRN_FT5_ADR},
    {CLASS_MAINTFOREIGN6, TRN_FT6_ADR},
    {CLASS_MAINTFOREIGN7, TRN_FT7_ADR},
    {CLASS_MAINTFOREIGN8, TRN_FT8_ADR},
	{0, 0}
};

VOID    MaintLoanPickupDisp(MAINTLOANPICKUP *pData)
{
	MAINTDSPLOANPICKUP MaintDspLoanPickup = {0};
	USHORT             usTransAddress = 0;

	if (pData->uchLoanPickStatus & MAINT_DATA_INPUT_CE)
		return;

    /* check void descriptor */
    if (pData->usModifier & (MAINT_MODIFIER_VOID | MAINT_MODIFIER_EC)) {
        flDispRegDescrControl |= VOID_CNTRL;
    } else {
        flDispRegDescrControl &= ~VOID_CNTRL;
    }

    /* set transaction mnemonic */
	if (pData->uchMinorClass == CLASS_MAINTOTHER){
        if(pData->usModifier & MAINT_MODIFIER_QTY){
            usTransAddress = TRN_QTY_ADR;
        }
        if(pData->usModifier & MAINT_MODIFIER_VOID){
            if(pData->usModifier == MAINT_MODIFIER_VOID){
                pData->uchMinorClass = CLASS_MAINTDSPCTL_AMT; /* display with deicmal point */
            }
        }
	}
	else {
		int i;

		for (i = 0; myTender2MnemonicLookupTable[i].uchMinorClass != 0; i++) {
			if (pData->uchMinorClass == myTender2MnemonicLookupTable[i].uchMinorClass) {
				usTransAddress = myTender2MnemonicLookupTable[i].uchAddress;
				break;
			}
		}

		if (myTender2MnemonicLookupTable[i].uchMinorClass == 0)
			return;
	}

    /* set display mnemonics */
    if ((pData->uchMinorClass == CLASS_MAINTDSPCTL_AMT) && (pData->usModifier == MAINT_MODIFIER_VOID)) {
        RflGetSpecMnem (MaintDspLoanPickup.aszMnemonics, SPC_VOID_ADR);
    } else if (usTransAddress != 0) {
        RflGetTranMnem (MaintDspLoanPickup.aszMnemonics, usTransAddress); 
    }

    /* set MaintDspLoanPickup for Display */
    MaintDspLoanPickup.uchMajorClass = pData->uchMajorClass;
    MaintDspLoanPickup.uchMinorClass = pData->uchMinorClass;
    MaintDspLoanPickup.lAmount = pData->lAmount;    /* set amount */
    MaintDspLoanPickup.uchFCMDC = pData->uchFCMDC;  /* set decimal point */
    DispWrite(&MaintDspLoanPickup);
}

/*
*===========================================================================
** Synopsis:    VOID MaintLoanPickupHearCtl(MAINTLOANPICKUP *pData)
*
*     Input:    *pData          : pointer to structure for MAINTREDISP
*    Output:    Nothing
*
** Return:      Nothing
*
*
** Description: Print Header for receipt/journal/slip(consective type)
*===========================================================================
*/
VOID    MaintLoanPickupHeaderCtl(MAINTLOANPICKUP *pData)
{
    UCHAR       uchMinorClass, uchAct, uchBit = 0;
    SHORT       sComp = 0, sFlag;

    if (pData->uchMajorClass == CLASS_MAINTLOAN) {
        uchMinorClass = CLASS_OPEN_LOAN;
        uchAct = AC_LOAN;
        pData->HeadTrail.uchMajorClass = CLASS_MAINTLOAN;
        uchBit = EVEN_MDC_BIT0;
    } else {
        uchMinorClass = CLASS_OPEN_PICK;
        uchAct = AC_PICKUP;
        pData->HeadTrail.uchMajorClass = CLASS_MAINTPICKUP;
        uchBit = EVEN_MDC_BIT1;
    }

    /* check amount entry for R/J Header print */
    if ((MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) == 0) {
		RPTCASHIER  RptCashier = {0};

		{
			TRANCURQUAL *pWorkCur = TrnGetCurQualPtr();

			if (CliParaMDCCheck(MDC_LOANPICK2_ADR, uchBit)) {   /* Compulsory Slip */
				pWorkCur->fbCompPrint |= CURQUAL_COMP_S;
			}
			pWorkCur->fbCompPrint = 0;
			if (pData->uchLoanPickStatus & MAINT_DATA_INPUT_CE)
				pWorkCur->fbCompPrint |= PRT_JOURNAL;
			else
				pWorkCur->fbCompPrint |= PRT_RECEIPT | PRT_JOURNAL;
		}

		MaintTrnOpen(uchMinorClass, pData->ulCashierNo, 0);

        if (MaintWork.LoanPickup.HeadTrail.usPrintControl & PRT_SLIP) {
            MaintSPHeaderCtl(&pData->HeadTrail);
        } else {
			if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT_CE)
				MaintHeaderCtlExt(uchAct, RPT_ACT_ADR, PRT_JOURNAL);
			else
				MaintHeaderCtl(uchAct, RPT_ACT_ADR);
        }

        RptCashier.uchMajorClass = CLASS_RPTCASHIER;
        if (pData->uchMajorClass == CLASS_MAINTLOAN) {
            RptCashier.uchMinorClass = CLASS_MAINTLOAN;
        } else {
            RptCashier.uchMinorClass = CLASS_MAINTPICKUP;
        }
        RptCashier.ulCashierNumber = pData->ulCashierNo;
        _tcsncpy(RptCashier.aszCashMnemo, pData->auchCashierName, PARA_CASHIER_LEN);
		if (pData->uchLoanPickStatus & MAINT_DATA_INPUT_CE)
			RptCashier.usPrintControl = PRT_JOURNAL;
		else
			RptCashier.usPrintControl = PRT_JOURNAL | PRT_RECEIPT;
        if (MaintWork.LoanPickup.HeadTrail.usPrintControl & PRT_SLIP) {
            MaintSPHeadTrailSet(&pData->HeadTrail, pData->ulCashierNo, pData->auchCashierName);
            RptCashier.usPrintControl = PRT_SLIP | PRT_JOURNAL;
            PrtPrintItem(NULL, &RptCashier);
        } else {
            PrtPrintItem(NULL, &RptCashier);
			if ((pData->uchLoanPickStatus & MAINT_DATA_INPUT_CE) == 0)
				MaintLoanPickFeed(PRT_RECEIPT);
        }
    }
    if (! CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT2)) {      /* Single Type */
        if ((MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) != 0) {
            sFlag = MaintSPTrailCtl(&MaintWork.LoanPickup.HeadTrail);
            if (pData->uchMajorClass == CLASS_MAINTLOAN) {
                if (CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT0)) {
                    UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);     /* Slip Paper Change */
                    sComp = 1;
                } else {
                    if (sFlag) {
                        UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT); /* Slip Paper Change */
                        sComp = 1;
                    }
                }
            } else {
                if (CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT1)) {
                    UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);     /* Slip Paper Change */
                    sComp = 1;
                } else {
                    if (sFlag) {
                        UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT); /* Slip Paper Change */
                        sComp = 1;
                    }
                }
            }
            if (sComp != 0) {
				TRANCURQUAL  *pWorkCur = TrnGetCurQualPtr();
				ITEMPRINT    ItemPrint = {0};

                pWorkCur->fbCompPrint |= CURQUAL_COMP_S;
                ItemPrint.uchMajorClass = CLASS_ITEMPRINT; 
                ItemPrint.uchMinorClass = CLASS_LOANPICKSTART;
                TrnThrough(&ItemPrint);
            }
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID MaintSPHeadTrailSet( MAINTSPHEADER *pData)
*
*     Input:    *pData          : pointer to structure for MAINTSPHEADER
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: Print Header on Slip
*===========================================================================
*/
VOID    MaintSPHeadTrailSet(MAINTSPHEADER *pData, ULONG ulCashierNo, TCHAR *pszMnemo)
{
    PARASTOREGNO    ParaStoRegNo;

    /******** Header Information Set *****/
    if (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN) {
        pData->usACNumber = AC_LOAN;
    } else {
        pData->usACNumber = AC_PICKUP;
    }
    pData->ulCashierNo = ulCashierNo;
    _tcsncpy(pData->aszMnemonics, pszMnemo, PARA_CASHIER_LEN);

    /******** Trailer Information **********/
    /* set EOD reset counter */
    pData->usDayRstCount = MaintCurrentSpcCo(SPCO_EODRST_ADR);

    /* read PTD reset counter */
    pData->usPTDRstCount = MaintCurrentSpcCo(SPCO_PTDRST_ADR);

    /* set supervisor number */
    pData->ulSuperNumber = ulMaintSupNumber;

    /* set store/register number */
    ParaStoRegNo.uchMajorClass = CLASS_PARASTOREGNO;
    CliParaRead(&ParaStoRegNo);  /* call ParaStoRegNoRead() */
    pData->usStoreNumber = ParaStoRegNo.usStoreNo;
    pData->usRegNumber = ParaStoRegNo.usRegisterNo;

    /* read consecutive number */
    pData->usConsCount = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSPHearCtl(USHORT usACNumber, UCHAR uchAdr)
*
*     Input:    *pData          : pointer to structure for MAINTREDISP
*    Output:    Nothing
*
** Return:      Nothing
*
*
** Description: Print Header on Slip
*===========================================================================
*/
VOID    MaintSPHeaderCtl(MAINTSPHEADER *pData)
{
    pData->uchMajorClass = CLASS_MAINTSPHEADER;
    pData->uchMinorClass = CLASS_MAINTHEADER_PARA;
    pData->usPrintControl = PRT_SLIP | PRT_JOURNAL;

    if (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN) {
        pData->usACNumber = AC_LOAN;
    } else {
        pData->usACNumber = AC_PICKUP;
    }

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);           /* increase consecutive number */
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }

    TrnThrough(pData);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSPTrailCtl(VOID)
*
*     Input:    *pData          : pointer to structure for MAINTREDISP
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: Print Header on Slip
*===========================================================================
*/
BOOL    MaintSPTrailCtl(MAINTSPHEADER *pData)
{
/*    pData->uchMajorClass = CLASS_MAINTSPTRAILER;  */
	MAINTTRAILER     MaintTrailer = {0};
    PARASTOREGNO     ParaStoRegNo;
    DATE_TIME        DateTime;

    MaintTrailer.uchMajorClass = CLASS_MAINTSPTRAILER;
    if (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN) {
        MaintTrailer.uchMinorClass = AC_LOAN;
    } else {
        MaintTrailer.uchMinorClass = AC_PICKUP;
    }
    
    /* set supervisor number */
    MaintTrailer.ulSuperNumber = ulMaintSupNumber;

    /* set EOD reset counter mnemonics */
    RflGetSpecMnem (MaintTrailer.aszDayRstMnemo, SPC_DAIRST_ADR);

    /* set PTD reset counter mnemonics */
    RflGetSpecMnem (MaintTrailer.aszPTDRstMnemo, SPC_PTDRST_ADR);

    /* get copy of EOD reset counter */
    MaintTrailer.usDayRstCount = MaintCurrentSpcCo(SPCO_EODRST_ADR);

    /* get copy of PTD reset counter */
    MaintTrailer.usPTDRstCount = MaintCurrentSpcCo(SPCO_PTDRST_ADR);

    /* set store/register number */
    ParaStoRegNo.uchMajorClass = CLASS_PARASTOREGNO;
    CliParaRead(&ParaStoRegNo);
    MaintTrailer.usStoreNumber = ParaStoRegNo.usStoreNo;
    MaintTrailer.usRegNumber = ParaStoRegNo.usRegisterNo;

    /* get copy of consecutive number */
    MaintTrailer.usConsCount = MaintCurrentSpcCo(SPCO_CONSEC_ADR);

    /* set time */
    PifGetDateTime(&DateTime);
    MaintTrailer.usMin = DateTime.usMin;
    MaintTrailer.usHour = DateTime.usHour; 
    MaintTrailer.usMDay = DateTime.usMDay;
    MaintTrailer.usMonth = DateTime.usMonth;
    MaintTrailer.usYear = DateTime.usYear;

    /* set print control */
    MaintTrailer.usPrtControl = PRT_JOURNAL | PRT_SLIP;
    PrtPrintItem(NULL, &MaintTrailer);

    if (MaintTrailer.uchStatus != 0) {      /* Slip Print Executed */
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    VOID MaintTrnOpen(UCHAR uchMinorClass, MAINTLOANPICKUP *pData
*
*     Input:    uchMinorClass          : minor class for transaction open
*    Output:    Nothing
*
** Return:      Nothing
*
*
** Description: Set Slip Status for Transaction Open
*===========================================================================
*/
VOID    MaintTrnOpen(UCHAR uchMinorClass, ULONG ulCashierNo, DCURRENCY lAmount)
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
	ITEMTRANSOPEN   ItemTransOpen = {0};
    ITEMPRINT       ItemPrint = {0};

    /* --- open item, cons. post rec. file --- */
    TrnICPOpenFile();

    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
        pWorkCur->flPrintStatus |= CURQUAL_DDMMYY;     
    }
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
        pWorkCur->flPrintStatus |= CURQUAL_MILITARY;     
    }
	if (uchMinorClass == CLASS_OPEN_TRANSBATCH) {
        pWorkCur->fbCompPrint |= (CURQUAL_COMP_R | CURQUAL_COMP_J);     
	}
	// Amtrak, AMT-2531 - Finalization receipt should follow MDC setting.
    pWorkCur->flPrintStatus &= ~CURQUAL_UNIQUE_TRANS_NO;    /* unique trans# */
    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 )) {
		// we are using MDC 234 Bit D to decide if we are to use a Unique Transaction Number or not.
		// however this also controls whether the guest check number is printed double size or not.
		// if MDC 234 Bit C is off then print standard size if MDC 234 Bit D is off as well.
		// if MDC 234 Bit C is on then
		//   - if MDC 234 Bit D is off, no printing of Unique Transaction Number and print single size guest check number
		//   - if MDC 234 Bit D is on, no printing of Unique Transaction Number but print double size guest check number
        pWorkCur->flPrintStatus |= CURQUAL_UNIQUE_TRANS_NO;    /* unique trans# */
    }

    ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
	switch (uchMinorClass) {
		case CLASS_OPEN_LOAN:
		case CLASS_OPEN_PICK:
			ItemPrint.uchMinorClass = CLASS_LOANPICKSTART;
			TrnThrough( &ItemPrint );
			break;

		case CLASS_OPEN_TRANSBATCH:
			ItemPrint.uchMinorClass = CLASS_START;
			ItemPrint.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL);
			ItemPrint.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);  /* store into item and consoli storage */
			ItemTransOpen.lAmount = lAmount;
			ItemTransOpen.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL);
            TrnItem(&ItemPrint);            /* stored print of promotion header */                                        
			break;
	}

    /* ----- send transaction open data ----- */
    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = uchMinorClass;
    ItemTransOpen.ulCashierID   = ulCashierNo;
	ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    /* ----- slip condition set ----- */
    TrnOpen(&ItemTransOpen);  /* open transaction */
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintLoanPickupFinalize(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Cashier Finalize for Loan/Pickup Module
*======================================================================================
*/
SHORT   MaintLoanPickupFinalize(MAINTLOANPICKUP *pData)
{
    CASIF    CasIf = {0};

    if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT) {
		ITEMTRANSCLOSE  Close = {0};
		SHORT           sStatus;

        Close.uchMajorClass = CLASS_ITEMTRANSCLOSE;
        Close.uchMinorClass = (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN) ? CLASS_CLS_LOAN : CLASS_CLS_PICK;  /* 09/08/00 */
        TrnClose(&Close);

        while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
			USHORT  usStatus = TtlConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
        MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure, 09/08/00 */
    }
    /* Close Cashier in this Module */
    CasIf.ulCashierNo = MaintWork.LoanPickup.ulCashierNo;
    CliCasClosePickupLoan(&CasIf);
    return(SUCCESS);
}
/* --- End of Source File --- */
