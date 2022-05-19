/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TOTAL KEY MODULE
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTOTAL3.C (service total)
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
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------
:  Abstract:
:   ItemTotalSE()           ;   service total
:
:  ---------------------------------------------------------------------
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 11/15/95 : 03.01.00 :   hkato     : R3.1 (Re-Write)
: 01/19/98 : 03.01.16 :  M.Ozawa    : Add Seat# Tender Status Check for
:                                     Charge Tips Storage Control.
:  8/17/98 : 03.03.00 :  hrkato     : V3.3 (VAT/Service)
: 10/21/98 : 03.03.00 :  M.Suzuki   : TAR 89859
: 08/11/99 : 03.05.00  : M.Teraki   : Remove WAITER_MODEL
: 08/13/99 : 03.05.00 :  M.teraki   : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "regstrct.h"
#include    "uie.h"
#include    "uireg.h"
#include    "pif.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "display.h"
#include    "mld.h"
#include    "fdt.h"
#include    "pmg.h"
#include    "item.h"
#include    "itmlocal.h"



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSE( UIFREGTOTAL *UifRegTotal )
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifRegTotal
*   SideEffects: modifies TrnInformation.TranCurQual
*
*   Return:
*
*   Description:    service total
==========================================================================*/

SHORT   ItemTotalSE(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();

    if ((sStatus = ItemTotalSECheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* cancel auto charge tips by check total, V3.3 */
    ItemPrevCancelAutoTips();

    /* ---- for print control 07/06/98 ---- */
    if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
        pWorkCur->fsCurStatus &= ~CURQUAL_OPEMASK;
        pWorkCur->fsCurStatus |= (CURQUAL_REORDER | CURQUAL_ADDCHECK_SERVT);
    } else
	{
		pWorkCur->fsCurStatus |= CURQUAL_SERVTOTAL; //SR 597, We set this for later checking JHHJ
	}

    /*----- PreCheck/Buffering, PreCheck/Unbuffering System -----*/
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER
        || (pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
        if (UifRegTotal->fbInputStatus & INPUT_0_ONLY) {
            return(LDT_SEQERR_ADR);
        }
        return(ItemTotalSEPreCheck(UifRegTotal));
    } else if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
        /*----- PostCheck System -----*/
        if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
            if (UifRegTotal->fbInputStatus & INPUT_0_ONLY) {
                return(LDT_SEQERR_ADR);
            }
            return(ItemTotalSEPostCheck(UifRegTotal));

        } else {
            /*----- SoftCheck System -----*/
            if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
                if (UifRegTotal->fbInputStatus & INPUT_0_ONLY) {
                    return(LDT_SEQERR_ADR);
                }
                return(ItemTotalSESoftCheck(UifRegTotal));
            } else {
				/*----- SoftCheck(Split) System -----*/
                /* --- check seat# tender status, 01/19/98 --- */
                /*----- Split Key -> Seat# -> Total -----*/
                if (pWorkCur->uchSplit != 0) {
                    if (pWorkCur->uchSeat != 0) {
						//SR 533, we need to set the status back to what it was before
						//we entered this function, if we dont, no other kind of totals will
						//be allowed to be done.
						pWorkCur->fsCurStatus &=  ~(CURQUAL_REORDER | CURQUAL_ADDCHECK_SERVT);
						pWorkCur->fsCurStatus |= CURQUAL_OPEMASK;
                        return(LDT_SEQERR_ADR);
                    }
                }
                return(ItemTotalSESoftCheckSpl(UifRegTotal));
            }
        }
    }
    else {
		/*----- Store/Recall System -----*/
        if (UifRegTotal->fbInputStatus & INPUT_0_ONLY) {
            return(LDT_SEQERR_ADR);
        }
        return(ItemTotalSEStoreRecall(UifRegTotal));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSECheck(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*   SideEffects: modifies TrnInformation.TranGCFQual
*
*   Return:
*
*   Description:    check modifier, condition for Pre/Post/Store.
==========================================================================*/
SHORT   ItemTotalSECheck(VOID)
{
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();

    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT ) {    /* R3.3 */
        return( LDT_SEQERR_ADR );
    } else if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) {
		FDTPARA     WorkFDT = { 0 };

        if ( TranModeQualPtr->ulWaiterID ) {                                /* waiter */
            return( LDT_SEQERR_ADR );
        }
		FDTParameter(&WorkFDT);
        if (WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_2) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Incorrect Term Type. FX_DRIVE_DELIV_1 or FX_DRIVE_DELIV_2 required.");
            return(LDT_SEQERR_ADR);
        }
    } else if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Transaction Operation Type condition check failed ItemTotalSECheck()");
        return( LDT_SEQERR_ADR );                               /* sequence error */
    }

    /*----- R3.1,   V3.3 -----
    if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER
        || (WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
        if ((WorkCur.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
            return(LDT_SEQERR_ADR);
        }
    }
*/
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
            || ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
            && (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_REORDER)) {

            if ( ItemModLocalPtr->fsTaxable & ~MOD_USEXEMPT ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */
            }
            if ( ItemModLocalPtr->fsTaxable & MOD_USEXEMPT ) {           /* tax exempt case */
                WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {            /* Saratoga */
                WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {       /* V3.3 */
        if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) ||
            ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
            && (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_REORDER)) {

            if ( ItemModLocalPtr->fsTaxable & ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT ) ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */
            }
            if ( ItemModLocalPtr->fsTaxable & MOD_GSTEXEMPT ) {          /* GST exempt case */
                WorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;
            }
            if ( ItemModLocalPtr->fsTaxable & MOD_PSTEXEMPT ) {          /* PST exempt case */
                WorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */
            }
        }
    }

    /* prohibit service total after multiple check payment, V3.3 */
    if (TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK) {     /* during multi check transaction */
        return( LDT_SEQERR_ADR );                       /* sequence error */
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPreCheck(UIFREGTOTAL *UifRegTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifRegTotal
*
*   Return:
*
*   Description:    PreCheck System.
==========================================================================*/

SHORT   ItemTotalSEPreCheck(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
	ULONG           fsCurStatus;
	ITEMTOTAL       ItemTotal = {0};
    ITEMAFFECTION   ItemAffection = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    USHORT          fsLimitable = FALSE, fsBaked = FALSE;  // used only when ItemCommonTaxSystem() == ITM_TAX_CANADA

    /*----- Send Previous "Item" -----*/
    ItemTotalPrev();

    /*----- Tax Calculation for MI -----*/
    if ((sStatus = ItemTotalSETaxRef(UifRegTotal, &ItemTotal, &ItemAffection, &WorkTax)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        ItemTotal.lMI += ItemTotal.lTax;
    } else {
        ItemTotal.lMI += ItemAffection.USCanVAT.USCanTax.lTaxAmount[0]
            + ItemAffection.USCanVAT.USCanTax.lTaxAmount[1]
            + ItemAffection.USCanVAT.USCanTax.lTaxAmount[2]
            + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3];

       fsLimitable = WorkTax.fsLimitable;
       fsBaked = WorkTax.fsBaked;
    }
    ItemTotalSEAmount(&ItemTotal);

    /*----- check pre-auth. system -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		return(LDT_PROHBT_ADR);
	}

    /*----- Update Data ----*/
    /* R3.3 */
    if (ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT1 ||
		ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT2) {
        ItemTotalSEOpenCheck();
    }
    ItemTotalSEPreAffect(UifRegTotal, &ItemTotal, &ItemAffection, &WorkTax, fsLimitable, fsBaked);
    ItemTotalSEPreItem(UifRegTotal, &ItemTotal);

    if (ITM_TTL_FLAG_DRAWER(&ItemTotal)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
    /*----- check pre-auth. system -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal) && (CliParaMDCCheck(MDC_PREAUTH_PADR,EVEN_MDC_BIT1))) {
		ItemTrailer(TYPE_EPT2);
	} else {
		ItemTrailer(TYPE_STORAGE);
	}

    ItemTotalSEClose(&ItemTotal);

    if ((ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_SERVICE2) || /* R3.3 */
        (ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT2)) {
        ItemTotalStub(&ItemTotal);
    }

    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
        ItemTotalDupRec(&ItemTotal);                            /* CP duplicate */
    }

    ItemTotalSEPreGCFSave();

    fsCurStatus = (TranCurQualPtr->fsCurStatus & CURQUAL_WIC);             /* Saratoga */
    ItemTotalInit();
    ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */
    return(ItemFinalize());
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPreAffect(UIFREGTOTAL *UifRegTotal,
*                       ITEMTOTAL *Total, ITEMAFFECTION *Affect,
*                       ITEMCOMMONTAX *WorkTax)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate affection tax, hourly, service total data
==========================================================================*/

VOID    ItemTotalSEPreAffect(UIFREGTOTAL *UifRegTotal,
            ITEMTOTAL *Total, ITEMAFFECTION *Affect, ITEMCOMMONTAX *WorkTax, USHORT fsLimitable, USHORT fsBaked)
{
    USHORT          i, j;
    DCURRENCY       lTax = 0;
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();

    /*----- Make Tax Data Class -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        for(i = 0; i < 3; i++) {
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
        }
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0                /* US tax 1 ITM_TTL_FLAG_TAX_1() */
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0            /* US tax 2 ITM_TTL_FLAG_TAX_2() */
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0             /* US tax 3 ITM_TTL_FLAG_TAX_3() */
            || (Total->auchTotalStatus[2] & EVEN_MDC_BIT0) == 0) {  /* clear itemizer ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER() */

            for(i = 0; i < 3; i++) {
                WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i] = 0L;
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {           /* V3.3 */
		// Canadian tax indicators use with total key provisioning is there are four tax indicators (GST, PST1, PST2, PST3)
		// and these are indicated by bits in the second byte of the total key status.
        if (Total->auchTotalStatus[1] != 0) {   // at least one Canadian tax itemizer in use
            for(i = 0; i < 5; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            }
            for(i = 0; i < 4; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
            }
            WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0] -= ItemTotalLocal.lPigTaxExempt;   /* 10-21-98 */
        }
        if (Total->auchTotalStatus[1] != 0 || !(Total->auchTotalStatus[2] & EVEN_MDC_BIT0)) {
			// if at least one Canadian tax itemizer in use and we are to clear the tax itemizers, do so.
            WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable += WorkTI->Itemizers.TranCanadaItemizers.lTaxable[8];   /* 10-21-98 */
            for(i = 0; i < STD_PLU_ITEMIZERS; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lTaxable[i] = 0L;
            }
            for(i = 0; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
                WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[i] = 0L;
            }
            WorkTI->Itemizers.TranCanadaItemizers.lMealLimitTaxable += WorkTax->lMealLimitTaxable;
            WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable += WorkTax->lMealNonLimitTaxable;

		    /* check limitable status - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
			if (fsLimitable == TRUE) {
				for (j=0; j<6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][1] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			} else {
				for (j=0; j<6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][2] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			}

		    /* check pig rule counter */
			if (fsBaked == FALSE) {
				for (j=0; j<6; j++) {
					for(i = STD_PLU_ITEMIZERS+2; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
						WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][i] = 0L;
					}
    	        }
			}
        }
    } else {
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0                /* International tax 1 */
            || Total->auchTotalStatus[1] & ODD_MDC_BIT1             /* International tax 2 */
            || Total->auchTotalStatus[1] & ODD_MDC_BIT2             /* International tax 3 */
            || (Total->auchTotalStatus[2] & EVEN_MDC_BIT0) == 0) {  /* clear itemizer */
			// International tax indicators use with total key provisioning is there are three tax indicators
			// and these are indicated by bits in the second byte of the total key status similar to Canadian tax.
            WorkTI->Itemizers.TranIntlItemizers.lServiceable = 0L;
            for(i = 0; i < 3; i++) {
                WorkTI->Itemizers.TranIntlItemizers.lVATSer[i] = 0L;
                WorkTI->Itemizers.TranIntlItemizers.lVATNon[i] = 0L;
            }
            /* memset(&(WorkTI->Itemizers.TranIntlItemizers), 0, sizeof(TRANINTLITEMIZERS)); */
        }
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        lTax = Total->lTax;                     /* save exclude vat/service */
    } else {
        lTax = Affect->USCanVAT.USCanTax.lTaxAmount[0] + Affect->USCanVAT.USCanTax.lTaxAmount[1]
            + Affect->USCanVAT.USCanTax.lTaxAmount[2] + Affect->USCanVAT.USCanTax.lTaxAmount[3];
    }
    WorkTI->lMI += lTax;
    WorkGCF->lCurBalance = WorkTI->lMI;

    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {                    /* V3.3 */
        Affect->uchMajorClass = CLASS_ITEMAFFECTION;
        Affect->uchMinorClass = CLASS_TAXAFFECT;
    }

    if (ItemCommonTaxSystem() == ITM_TAX_US) {              /* V3.3 */
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0         /* US tax 1 ITM_TTL_FLAG_TAX_1() */
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0     /* US tax 2 ITM_TTL_FLAG_TAX_2() */
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0) {   /* US tax 3 ITM_TTL_FLAG_TAX_3() */
            TrnItem(Affect);

            /* send to enhanced kds, 2172 */
            ItemSendKds(Affect, 0);

            Affect->uchMinorClass = CLASS_TAXPRINT;
            Affect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

            /*----- Make Subtotal,   R3.1 4/24/96 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                Affect->lAmount = WorkTI->lMI - lTax;
            }
            TrnItem(Affect);
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {           /* V3.3 */
        if (Total->auchTotalStatus[1] != 0) {       // at least one Canadian tax itemizer in use
            TrnItem(Affect);

            /* send to enhanced kds, 2172 */
            ItemSendKds(Affect, 0);

            Affect->uchMinorClass = CLASS_TAXPRINT;
            Affect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

            /*----- Make Subtotal,   R3.1 4/24/96 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                Affect->lAmount = WorkTI->lMI - lTax;
            }
            /*----- adjust tax print format 4/17/96 -----*/
            ItemCanTaxCopy2(Affect);
            TrnItem(Affect);
        }
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        ItemTendVATCommon(Affect, ITM_NOT_SPLIT, ITM_DISPLAY);
    } else {
        lTax = Affect->USCanVAT.USCanTax.lTaxAmount[0] + Affect->USCanVAT.USCanTax.lTaxAmount[1]
            + Affect->USCanVAT.USCanTax.lTaxAmount[2] + Affect->USCanVAT.USCanTax.lTaxAmount[3];

        /*----- Display Tax to LCD,  R3.0 -----*/
        MldScrollWrite(Affect, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(Affect);
    }

    /*----- Make Hourly Data Class -----*/
    memset(Affect, 0, sizeof(ITEMAFFECTION));
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_HOURLY;

    if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0)) {             /* HPUS R2.5 CP */
        Affect->lAmount = WorkTI->lHourly;
    } else {
        Affect->lAmount = WorkTI->lHourly + lTax;
    }
    Affect->sItemCounter = WorkTI->sItemCounter;
    Affect->sNoPerson = ItemTendNoPerson();
    Affect->uchOffset = TranGCFQualPtr->uchHourlyOffset;
    /* V3.3 */

	//This assignment causes the Bonus totals in TtlHourupdate to update the appropriate
	//bonus total in the hourly activity reports.
    for (i = 0; i < STD_NO_BONUS_TTL; i++) {
        Affect->lBonus[i] = WorkTI->lHourlyBonus[i];                /* hourly bonus total */
    }
    TrnItem(Affect);

	//By commenting out the below code, this alleviates the problem of the service totals
	//becoming inflated.  By removing the code below, we dont send an extra price to the Service
	//total information.
	// SR278
    /*----- Make Service Total Affection Data Class -----*/
	memset(Affect, 0, sizeof(ITEMAFFECTION));
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_SERVICE;
    Affect->lAmount = Total->lService;
    Affect->uchAddNum = ItemTotalSEGetSerID(Total);
    Affect->uchTotalID = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
	TrnItem(Affect);

    WorkTI->lService[Affect->uchAddNum] += Affect->lAmount;         /* update current service total */
    WorkTI->lHourly = 0L;                                           /* clear hourly itemizer */
    //WorkTI->sItemCounter = 0;                                       /* clear item counter */
	//We now clear the hourly bonus totals, because we have already affected them to the Hourly Totals,
	//by doing this, we no longer run into the problem when the user opens up a check, rings up items, then
	//closes the check, and when they tender it out, it updates the bonuses incorrectly.
	// SR278
	memset(&WorkTI->lHourlyBonus, 0x00, sizeof(WorkTI->lHourlyBonus));
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPreItem(UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate service total data
==========================================================================*/
VOID    ItemTotalSEPreItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
    TRANGCFQUAL   * const WorkGCF = TrnGetGCFQualPtr();
    SHORT         sFlag = 0;

    /*----- Make Service Total Data Class -----*/
    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
    ItemTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
    memcpy(&ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], sizeof(ItemTotal->aszNumber));

    ItemTotal->usConsNo = TranCurQualPtr->usConsNo;                         /* consecutive No */
    ItemTotal->uchLineNo = WorkGCF->uchSlipLine;                    /* line No */
    ItemTotal->ulID = TranModeQualPtr->ulCashierID;                         /* cashier ID */

    if (ItemTotal->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {            /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;                      /* print validation */
        TrnThrough(ItemTotal);
        sFlag = 1;
    }

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

	ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                      /* print journal */
    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

	TrnItem(ItemTotal);

    /*----- Make Display on line display, R3.3 -----*/
    ItemTotalSEDispCom(UifRegTotal, ItemTotal->lMI);

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, ItemTotal->lMI);

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEPreGCFSave(VOID)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    save current GCF
==========================================================================*/
VOID    ItemTotalSEPreGCFSave(VOID)
{
    if (TranGCFQualPtr->usGuestNo != 0) {
		SHORT       sStatus;

        while ((sStatus = TrnSaveGC(GCF_COUNTER_NOCONSOLI, TranGCFQualPtr->usGuestNo)) != TRN_SUCCESS) {
			USHORT      usStatus = GusConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
    }
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPostCheck(UIFREGTOTAL *UifRegTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifRegTotal
*
*   Return:
*
*   Description:    PostCheck System.
==========================================================================*/
SHORT   ItemTotalSEPostCheck(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
	ULONG           fsCurStatus;
	ITEMTOTAL       ItemTotal = {0};
    ITEMAFFECTION   TaxAffect = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    USHORT          fsLimitable = FALSE, fsBaked = FALSE;  // used only when ItemCommonTaxSystem() == ITM_TAX_CANADA

    /*----- Send Previous "Item" -----*/
    ItemTotalPrev();

    if ((sStatus = ItemTotalSETaxRef(UifRegTotal, &ItemTotal, &TaxAffect, &WorkTax)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        ItemTotal.lMI += ItemTotal.lTax;
    } else {
        ItemTotal.lMI += TaxAffect.USCanVAT.USCanTax.lTaxAmount[0]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[1]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[2]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[3];

       fsLimitable = WorkTax.fsLimitable;
       fsBaked = WorkTax.fsBaked;
    }
    ItemTotalSEAmount(&ItemTotal);

    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		return(LDT_PROHBT_ADR);
	}

    /*----- Check is Complete, Data Update OK -----*/
    /* R3.3 */
    if ((ItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_CASINT1) ||
        (ItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_CASINT2)) {
        ItemTotalSEOpenCheck();
    }
    /*----- Tax(Affection, Print) Data -----*/
    ItemTotalSEPostAffectTax(&ItemTotal, &TaxAffect, &WorkTax, fsLimitable, fsBaked);

    /*----- Hourly, Service Total Data -----*/
    ItemTotalSEPostAffect(UifRegTotal, &ItemTotal, &TaxAffect);

    /*----- Service Total Data -----*/
    ItemTotalSEPostItem(UifRegTotal, &ItemTotal);

    /*----- Drawer Open & Compulsory -----*/
    if (ITM_TTL_FLAG_DRAWER(&ItemTotal)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    /*----- PostCheck Buffering Print -----*/	
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal) && (CliParaMDCCheck(MDC_PREAUTH_PADR, EVEN_MDC_BIT1))) {
		ItemTrailer(TYPE_EPT2);
	} else if (ItemTotalSysType(&ItemTotal) == 2) {
        WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_PRINT;
        ItemTrailer(TYPE_STORAGE);
    } else {
        ItemTrailer(TYPE_POSTCHECK);
    }

    /*----- Transaction Close Data -----*/
    ItemTotalSEClose(&ItemTotal);

    if ((ItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_SERVICE2) || /* R3.3 */
        (ItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_CASINT2)) {
        ItemTotalStub(&ItemTotal);
    }
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
        ItemTotalDupRec(&ItemTotal);
    }

    ItemTotalSEPostGCFSave(&ItemTotal);

    fsCurStatus = TranCurQualPtr->fsCurStatus & CURQUAL_WIC;                       /* Saratoga */
    ItemTotalInit();
    ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */
    return(ItemFinalize());
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPostAffectTax(ITEMTOTAL *Total,
*                           ITEMAFFECTION *Affect, ITEMCOMMONTAX *WorkTax)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Make Affection/Print Tax Data.
==========================================================================*/

VOID    ItemTotalSEPostAffectTax(ITEMTOTAL *Total,
                        ITEMAFFECTION *Affect, ITEMCOMMONTAX *WorkTax, USHORT fsLimitable, USHORT fsBaked)
{
    SHORT          i, j;
    DCURRENCY      lTax = 0, lWholeTax = 0;
    DCURRENCY      lWholePrtTax = 0;          /* 5/13/96 */
    TRANGCFQUAL    * const WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS  * const WorkTI = TrnGetTIPtr();

    /*----- Affection Tax -----*/
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_TAXAFFECT;

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {                  /* V3.3 */
        if (Total->auchTotalStatus[1] != 0) {        // at least one Canadian tax itemizer in use
            for(i = 0; i < 5; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            }
            for(i = 0; i < 4; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
                lWholeTax += WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i];  /* 5/13/96 */
                lWholePrtTax += WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i];
            }
            WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0] -= ItemTotalLocal.lPigTaxExempt;   /* 10-21-98 */
            Affect->fbStorageStatus = NOT_CONSOLI_STORAGE;
            TrnItem(Affect);

            /* send to enhanced kds, 2172 */
            ItemSendKds(Affect, 0);
        }
        if (Total->auchTotalStatus[1] != 0 || !(Total->auchTotalStatus[2] & EVEN_MDC_BIT0)) {
            WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable += WorkTI->Itemizers.TranCanadaItemizers.lTaxable[8];   /* 10-21-98 */
            for(i = 0; i < STD_PLU_ITEMIZERS; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lTaxable[i] = 0L;
            }
            for(i = 0; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
                WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[i] = 0L;
            }
            WorkTI->Itemizers.TranCanadaItemizers.lMealLimitTaxable += WorkTax->lMealLimitTaxable;
            WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable += WorkTax->lMealNonLimitTaxable;

		    /* check limitable status - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
			if (fsLimitable == TRUE) {
				for (j=0; j<6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][1] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			} else {
				for (j=0; j<6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][2] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			}

		    /* check pig rule counter */
			if (fsBaked == FALSE) {
				for (j=0; j<6; j++) {
					for(i = STD_PLU_ITEMIZERS+2; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
						WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][i] = 0L;
					}
    	        }
			}
        }

    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
        for(i = 0; i < 3; i++) {
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
            lWholeTax += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i]; /* 5/13/96 */
            lWholePrtTax += WorkTI->Itemizers.TranUsItemizers.lPrintTax[i];
        }
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0          /* US tax 1 ITM_TTL_FLAG_TAX_1() */
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0      /* US tax 2 ITM_TTL_FLAG_TAX_2() */
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0) {    /* US tax 3 ITM_TTL_FLAG_TAX_3() */
            Affect->fbStorageStatus = NOT_CONSOLI_STORAGE;
            TrnItem(Affect);

            /* send to enhanced kds, 2172 */
            ItemSendKds(Affect, 0);
        }
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0                /* US tax 1 ITM_TTL_FLAG_TAX_1() */
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0            /* US tax 2 ITM_TTL_FLAG_TAX_2() */
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0             /* US tax 3 ITM_TTL_FLAG_TAX_3() */
            || (Total->auchTotalStatus[2] & EVEN_MDC_BIT0) == 0) {  /* clear itemizer ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER() */
            for(i = 0; i < 3; i++) {
                WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i] = 0L;
            }
        }
    } else {    // must be ITM_TAX_INTL instead.
        if ((ItemTotalSysType(Total) == 2) &&
            (Total->auchTotalStatus[1] & ODD_MDC_BIT0
            || Total->auchTotalStatus[1] & ODD_MDC_BIT1
            || Total->auchTotalStatus[1] & ODD_MDC_BIT2)) {

            WorkTI->Itemizers.TranIntlItemizers.lServiceable = 0L;
            for(i = 0; i < 3; i++) {
                WorkTI->Itemizers.TranIntlItemizers.lVATSer[i] = 0L;
                WorkTI->Itemizers.TranIntlItemizers.lVATNon[i] = 0L;
            }
        }
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        lTax = Total->lTax;                     /* save exclude vat/service */
    } else {
        lTax = Affect->USCanVAT.USCanTax.lTaxAmount[0] + Affect->USCanVAT.USCanTax.lTaxAmount[1]
            + Affect->USCanVAT.USCanTax.lTaxAmount[2] + Affect->USCanVAT.USCanTax.lTaxAmount[3];
    }
    WorkTI->lMI += lTax;

    /*----- For Hourly -----*/
    if (!CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0)) {
        WorkTI->lHourly += lTax;
    }

    WorkGCF->lCurBalance = WorkTI->lMI;

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        if (ItemTotalSysType(Total) == 3) {
            Affect->fbStorageStatus = (NOT_ITEM_STORAGE|NOT_CONSOLI_STORAGE);
        }
        ItemTendVATCommon(Affect, ITM_NOT_SPLIT, ITM_DISPLAY);
    } else {

        /*----- Make Subtotal,   R3.1 4/25/96 -----*/
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {   /* 5/13/96 */
            Affect->lAmount = WorkTI->lMI - lWholeTax + lWholePrtTax;
        }

        /*----- Print Tax -----*/
        if (ItemTotalSysType(Total) == 2 || ItemTotalSysType(Total) == 3) {
            Affect->uchMajorClass = CLASS_ITEMAFFECTION;
            Affect->uchMinorClass = CLASS_TAXPRINT;

            if (ItemCommonTaxSystem() == ITM_TAX_US) {                  /* V3.3 */
                for (i = 0; i < 3; i++) {
                    Affect->USCanVAT.USCanTax.lTaxable[i]
                        = WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i] - WorkTI->Itemizers.TranUsItemizers.lPrintTaxable[i];
                    Affect->USCanVAT.USCanTax.lTaxAmount[i]
                        = WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i] - WorkTI->Itemizers.TranUsItemizers.lPrintTax[i];
                    Affect->USCanVAT.USCanTax.lTaxExempt[i]
                        = WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i] - WorkTI->Itemizers.TranUsItemizers.lPrintTaxExempt[i];

                    if (ItemTotalSysType(Total) == 2) {
                        WorkTI->Itemizers.TranUsItemizers.lPrintTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
                        WorkTI->Itemizers.TranUsItemizers.lPrintTax[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
                        WorkTI->Itemizers.TranUsItemizers.lPrintTaxExempt[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
                    }
                }
                if (ItemTotalSysType(Total) == 2) {
                    TrnPutTI(WorkTI);
                    Affect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
                    Affect->fbStorageStatus = NOT_ITEM_STORAGE;
                    TrnItem(Affect);
                }

            } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {       /* V3.3 */

                /*----- Postheck -----*/
                if (WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT) {
                    Affect->USCanVAT.USCanTax.lTaxExempt[0]
                        = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
                } else {
                    Affect->USCanVAT.USCanTax.lTaxable[0]
                        = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
                    Affect->USCanVAT.USCanTax.lTaxAmount[0]
                        = WorkTI->Itemizers.TranCanadaItemizers.lAffTax[0] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[0];
                }
                if (WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT) {
                    for ( i = 0; i < 3; i++ ) {
                        Affect->USCanVAT.USCanTax.lTaxExempt[i+1]
                            = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i+1];
                    }
                } else {
                    for (i = 0; i < 3; i++) {
                        Affect->USCanVAT.USCanTax.lTaxable[i+1]
                            = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i+1];
                        Affect->USCanVAT.USCanTax.lTaxAmount[i+1]
                            = WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i+1] - WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i+1];
                    }
                }
                Affect->USCanVAT.USCanTax.lTaxable[4] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[4];

                if (ItemTotalSysType(Total) == 2) {  // Print buffering of total key in a Post Guest Check System.
                    for (i = 0; i < 5; i++) {
                        WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
                    }
                    for (i = 0; i < 4; i++) {
                        WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
                        WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
                    }
                }

                /*----- adjust tax print format 4/25/96 -----*/
                ItemCanTaxCopy2(Affect);

                if (ItemTotalSysType(Total) == 2) {     /* 4/25/96 */
                    Affect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
                    Affect->fbStorageStatus = NOT_ITEM_STORAGE;
                    TrnItem(Affect);
                }
            }
        }

        MldScrollWrite(Affect, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(Affect);
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPostItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate service total data
==========================================================================*/
VOID    ItemTotalSEPostItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
    UCHAR       uchMinorClass;
    SHORT       sFlag = 0;

    /*----- Make Service Total Data -----*/
    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;

    /*----- Postcheck/Buffer Print -----*/
    if (ItemTotalSysType(ItemTotal) == 2) {
        ItemTotal->uchMinorClass = CLASS_TOTALPOSTCHECK;
        ItemTotal->fbStorageStatus = 0;
    } else {
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
        ItemTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
    }

    memcpy(ItemTotal->aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal->aszNumber));

    ItemTotal->usConsNo = TranCurQualPtr->usConsNo;             /* consecutive No */
    ItemTotal->uchLineNo = TranGCFQualPtr->uchSlipLine;         /* line No */

    if (TranModeQualPtr->ulWaiterID) {
        ItemTotal->ulID = TranModeQualPtr->ulWaiterID;
    } else {
        ItemTotal->ulID = TranModeQualPtr->ulCashierID;
    }

    ItemTotalSEGetGC(ItemTotal);                                /* Print GCF# */

    if (ITM_TTL_FLAG_PRINT_VALIDATION(ItemTotal)) {
        ItemTotal->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTotal);
        sFlag = 1;
    }
    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    TrnItem(ItemTotal);

    /*----- Make Display on line display, R3.3 -----*/
    ItemTotalSEDispCom(UifRegTotal, ItemTotal->lMI);

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, ItemTotal->lMI);

    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);

    /* send to enhanced kds, 2172 */
    if (ItemTotal->uchMinorClass == CLASS_TOTALPOSTCHECK) {
        uchMinorClass = ItemTotal->uchMinorClass;
        ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;
        ItemSendKds(ItemTotal, 0);
        ItemTotal->uchMinorClass = uchMinorClass;
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEPostGCFSave(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    save current GCF
==========================================================================*/
VOID    ItemTotalSEPostGCFSave(ITEMTOTAL *ItemTotal)
{
    SHORT       sStatus, sType;

    /*----- Buffering Print -----*/
    if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {
        sType = GCF_COUNTER_NOCONSOLI;
    } else {
        sType = GCF_COUNTER_TYPE;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    while ((sStatus = TrnSaveGC(sType, TranGCFQualPtr->usGuestNo)) != TRN_SUCCESS) {
		USHORT      usStatus = GusConvertError(sStatus);
        UieErrorMsg(usStatus, UIE_WITHOUT);
    }
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftCheck(UIFREGTOTAL *UifRegTotal)
*
*   Input:      none
*   Output:     none
*   InOut:
*
*   Return:
*
*   Description:    SoftCheck System.
==========================================================================*/
SHORT   ItemTotalSESoftCheck(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus, sItemCounter;
	ULONG           fsCurStatus;
	DCURRENCY       lHourly = 0, lMealTaxable[2] = {0};
	ITEMTOTAL       ItemTotal = {0};
    ITEMAFFECTION   TaxAffect = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();
    USHORT          fsLimitable = FALSE, fsBaked = FALSE;  // used only when ItemCommonTaxSystem() == ITM_TAX_CANADA

    /*----- Send Previous "Item" -----*/
    ItemTotalPrev();

    if ((sStatus = ItemTotalSETaxRef(UifRegTotal, &ItemTotal, &TaxAffect, &WorkTax)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        ItemTotal.lMI += ItemTotal.lTax;
    } else {
        /*----- Save Canadian Tax Data for Affection -----*/
        lMealTaxable[0] = WorkTax.lMealLimitTaxable;
        lMealTaxable[1] = WorkTax.lMealNonLimitTaxable;

        ItemTotal.lMI += TaxAffect.USCanVAT.USCanTax.lTaxAmount[0]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[1]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[2]
            + TaxAffect.USCanVAT.USCanTax.lTaxAmount[3];

		fsLimitable = WorkTax.fsLimitable;
        fsBaked = WorkTax.fsBaked;
    }
    ItemTotalSEAmount(&ItemTotal);

    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		return(LDT_PROHBT_ADR);
	}

    /*===== Check is Complete, Data Update OK =====*/
    pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;        /* print consoli storage */
    pWorkCur->fbNoPrint |= CURQUAL_NO_J;                  /* no print journal */

    /* R3.3 */
    if ((ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT1) ||
        (ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT2)) {
        ItemTotalSEOpenCheck();
    }
    /*----- Tax(Affection) Data -----*/
    ItemTotalSESoftAffectTax(&ItemTotal, &TaxAffect, lMealTaxable, fsLimitable, fsBaked);

    ItemTotalSESoftTI(&ItemTotal, &lHourly, &sItemCounter);
    ItemTotalSESoftGCFSave(&ItemTotal);
    ItemTotalSERestoreTI(&lHourly, &sItemCounter);

    /*----- Tax(Print) Data -----*/
    ItemTotalSESoftPrintTax(&ItemTotal, &TaxAffect);

    /*----- Hourly, Service Total Data -----*/
    ItemTotalSEPostAffect(UifRegTotal, &ItemTotal, &TaxAffect);

    /*----- Service Total Data -----*/
    ItemTotalSESoftItem(UifRegTotal, &ItemTotal);

    /*----- Drawer Open & Compulsory -----*/
    if (ITM_TTL_FLAG_DRAWER(&ItemTotal)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
    
	/*----- check pre-auth. system -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal) && (CliParaMDCCheck(MDC_PREAUTH_PADR,EVEN_MDC_BIT1))) {
		ItemTrailer(TYPE_EPT2);
	} else {
		ItemTrailer(TYPE_SOFTCHECK);
	}

    /*----- Transaction Close Data -----*/
    ItemTotalSEClose(&ItemTotal);

    if ((ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_SERVICE2) || /* R3.3 */
        (ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT2)) {
        ItemTotalStub(&ItemTotal);
    }
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
        ItemTotalDupRec(&ItemTotal);
    }

    fsCurStatus = pWorkCur->fsCurStatus & CURQUAL_WIC;                     /* Saratoga */
    ItemTotalInit();
    ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */
    return(ItemFinalize());
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftGCFSave(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    save current GCF
==========================================================================*/

VOID    ItemTotalSESoftGCFSave(ITEMTOTAL *ItemTotal)
{
    SHORT       sStatus;
    TRANGCFQUAL * const WorkGCF = TrnGetGCFQualPtr();

    if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {
        WorkGCF->uchSlipLine = 0;
        WorkGCF->uchPageNo = 0;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    while ((sStatus = TrnSaveGC(GCF_COUNTER_TYPE, WorkGCF->usGuestNo)) != TRN_SUCCESS) {
		USHORT      usStatus = GusConvertError(sStatus);
        UieErrorMsg(usStatus, UIE_WITHOUT);
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate service total data
==========================================================================*/

VOID    ItemTotalSESoftItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
    SHORT       sFlag = 0;

    /*----- Make Service Total Data -----*/
    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    if (ItemTotalSysType(ItemTotal) == 4) {                 /* Softcheck/Buffer Print */
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass) + (UCHAR)CLASS_SOFTCHK_BASE_TTL;
        ItemTotal->fbStorageStatus = 0;
    } else {
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
        ItemTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
    }

    memcpy(ItemTotal->aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal->aszNumber));

    ItemTotal->usConsNo = TranCurQualPtr->usConsNo;                 /* consecutive No */
    ItemTotal->uchLineNo = TranGCFQualPtr->uchSlipLine;            /* line No */

    if (TranModeQualPtr->ulWaiterID) {
        ItemTotal->ulID = TranModeQualPtr->ulWaiterID;
    } else {
        ItemTotal->ulID = TranModeQualPtr->ulCashierID;
    }

    ItemTotalSEGetGC(ItemTotal);                            /* Print GCF# */

    if (ITM_TTL_FLAG_PRINT_VALIDATION(ItemTotal)) {
        ItemTotal->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTotal);
        sFlag = 1;
    }
    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    /*----- Make Display on line display, R3.3 -----*/
    ItemTotalSEDispCom(UifRegTotal, ItemTotal->lMI);

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, ItemTotal->lMI);

    if (ItemTotalSysType(ItemTotal) == 4) {                     /* Softcheck/Buffer Print */
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass) + (UCHAR)CLASS_SOFTCHK_BASE_TTL;
    }

    TrnItem(ItemTotal);

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    /*----- Display Total to LCD,  R3.0 -----*/
    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);

    /*----- SoftCheck Buffer Print -----*/
    if (ItemTotalSysType(ItemTotal) == 4) {
        ItemTotal->uchMinorClass -= (UCHAR)CLASS_SOFTCHK_BASE_TTL;
    }
}

/****** End of Source ******/
