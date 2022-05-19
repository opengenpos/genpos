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
:   Program Name   : ITTOTALS.C (service total)
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
:   2/9/96 : 03.01.00 :   hkato     : R3.1
:   8/17/98: 03.03.00 :  hrkato     : V3.3 (VAT/Service)
: 10/21/98 : 03.03.00 :  M.Suzuki   : TAR 89859
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
#include	<stdlib.h>

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
#include    "csttl.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "fdt.h"
#include    "itmlocal.h"
#include    "appllog.h"
#include    "prt.h"
#include    "pmg.h"
#include    "rfl.h"

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftCheckSpl(UIFREGTOTAL *UifTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*
   Return:
*
*   Description:    SoftCheck System & Split Print.
==========================================================================*/

SHORT   ItemTotalSESoftCheckSpl(UIFREGTOTAL *UifTotal)
{
    SHORT           sRet;
    DCURRENCY       lMI = 0;
	ITEMAFFECTION   WholeTax = {0};
    ITEMTOTAL       WholeTotal = {0};
    ITEMTOTAL       CPTotal = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    UCHAR           uchTypeIdx;

    /*----- Send Previous "Item" -----*/
    ItemTotalPrev();

    /*----- Error Check -----*/
    if ((sRet = ItemTotalSESoftCheckCheckSpl(UifTotal)) != ITM_SUCCESS) {
        return(sRet);
    }

    /* set total key status, R3.3 */
	uchTypeIdx = ItemTotalTypeIndex(UifTotal->uchMinorClass);

    if (UifTotal->uchMinorClass == CLASS_UICASINT) {
		// cashier interrupt start so change the Total Key type to perform Cashier Interrrupt.
        if (ITM_TTL_GET_TTLTYPE(uchTypeIdx) == PRT_SERVICE2) {   /* service with stub */
			WholeTotal.auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_CASINT2);  /* cashier interrupt with stub */
        } else {
			WholeTotal.auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_CASINT1);  /* cashier interrupt without stub */
        }
    } else if (UifTotal->uchMinorClass == CLASS_UIKEYOUT) {  /* V3.3 */
        if (ITM_TTL_GET_TTLTYPE(uchTypeIdx) == PRT_SERVICE2) {   /* service with stub */
			WholeTotal.auchTotalStatus[0] = ITM_TTL_GET_TTLKEY(uchTypeIdx);
        } else {
			WholeTotal.auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_SERVICE1);  /* set service total */
        }
    } else {
        WholeTotal.auchTotalStatus[0] = uchTypeIdx;
    }

	ItemTotalAuchTotalStatus (WholeTotal.auchTotalStatus, ItemTotalSESetClass(UifTotal->uchMinorClass));

/*** V3.3 ICI
    /----- Not Use "Not Consoli Print" Option -----/
    WholeTotal.auchTotalStatus[4] &= ~0x20;
***/

    /*----- copy total key status to charget posting data ----- */
    memcpy(&CPTotal.auchTotalStatus[1], &WholeTotal.auchTotalStatus[1], MAX_TTLKEY_DATA);

    /*----- Whole Service Total Amount -----*/
    if ((sRet = ItemTotalSESoftMI(UifTotal ,&WholeTotal)) != ITM_SUCCESS) {
        return(sRet);
    }

    ItemTotalSEDispCom(UifTotal, WholeTotal.lMI);   /* R3.3 */
    lMI = WholeTotal.lMI;

    /*----- Split Check Single Print,   V3.3 -----*/
    if (UifTotal->uchSeatNo != ITM_SPLIT_IND_TAX_PRNIT && UifTotal->uchSeatNo != ITM_SPLIT_WHOLE_TAX_PRNIT) {
        if ((sRet = ItemTotalSESoftCheckSplSingle(UifTotal, &WholeTotal, &WholeTax)) != ITM_SUCCESS) {
            return(sRet);
        }

    /* --- Split Check All Print,  V3.3 --- */
    } else {
        /* --- Pre-Authorization,  V3.3 --- */
        if (ITM_TTL_FLAG_PRE_AUTH(&WholeTotal)) {
			return(LDT_PROHBT_ADR);
		}

        /* --- Whole Tax Print Option "99", V3.3 --- */
        if (UifTotal->uchSeatNo == ITM_SPLIT_WHOLE_TAX_PRNIT) {
            ItemTotalSESoftCheckSplWhole(UifTotal, &WholeTotal, &WholeTax);
        } else {
            ItemTotalSESoftCheckSplAll(UifTotal, &WholeTotal, &WholeTax);
        }
    }

    /*----- Display Whole Tax/Total,    V3.3 -----*/
    if (UifTotal->uchSeatNo == ITM_SPLIT_WHOLE_TAX_PRNIT) {
        ItemTotalSESplDispTaxTotalWhole(&WholeTax, UifTotal, &WholeTotal);
    } else {
        ItemTotalSESplDispTaxTotal(&WholeTax, UifTotal, &WholeTotal);
    }

    /*----- Display Total to LCD,  06/28/96 -----*/
    MldDispSubTotal(MLD_TOTAL_1, lMI);

    /*----- recover Total 4/16/96 -----*/
    WholeTotal.lMI = lMI;

    /*----- Affection(Hourly, Service Total) Data -----*/
    ItemTotalSESoftAffectSpl(UifTotal, &WholeTotal);

    /*----- Drawer Open & Compulsory -----*/
    if (ITM_TTL_FLAG_DRAWER(&WholeTotal)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    ItemTrailer(TYPE_POSTCHECK);    /* to drive kitchen printer */

    /*----- Transaction Close Data -----*/
    ItemTotalSESoftCloseSpl(&WholeTotal);

    ItemTotalSEGetGC(&WholeTotal);

    /*----- Issue Total Stub 5/29/96 -----*/
    if (UifTotal->uchSeatNo == 0) {
        if (!(UifTotal->fbInputStatus & INPUT_0_ONLY)) {
            if ((ITM_TTL_FLAG_TOTAL_TYPE(&WholeTotal) == PRT_SERVICE2) ||    /* R3.3 */
                (ITM_TTL_FLAG_TOTAL_TYPE(&WholeTotal) == PRT_CASINT2)) {
                ItemTotalStubSpl(&WholeTotal, 0);
            }
        }
    } else if (UifTotal->uchSeatNo == ITM_SPLIT_WHOLE_TAX_PRNIT) { /* V3.3 */
        if ((ITM_TTL_FLAG_TOTAL_TYPE(&WholeTotal) == PRT_SERVICE2) ||    /* R3.3 */
            (ITM_TTL_FLAG_TOTAL_TYPE(&WholeTotal) == PRT_CASINT2)) {
            ItemTotalStubSpl(&WholeTotal, 0);
        }
    }
    ItemTotalSESoftGCFSave(&WholeTotal);

    ItemTotalInit();

    return(ItemFinalize());
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftCheckCheckSpl(UIFREGTOTAL *UifTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*
*   Return:
*
*   Description:    Error Check on the Service Total of the split.
==========================================================================*/

SHORT   ItemTotalSESoftCheckCheckSpl(UIFREGTOTAL *UifTotal)
{
    SHORT           i;

	if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {  // TrnSplCheckSplit()
        return(LDT_SEQERR_ADR);
	}

    /* --- Check Option Value(0-9, 99), V3.3 --- */
    if (UifTotal->uchSeatNo > NUM_SEAT && UifTotal->uchSeatNo != ITM_SPLIT_WHOLE_TAX_PRNIT) { //SR206
        return(LDT_SEQERR_ADR);
    }

    /* --- Whole Tax Print Option "99", V3.3 --- */
    if (UifTotal->uchSeatNo != 0 && UifTotal->uchSeatNo != ITM_SPLIT_WHOLE_TAX_PRNIT) {
		TRANGCFQUAL     *WorkGCF;

        /*----- Check Used Seat# -----*/
        TrnGetGCFQualSpl(&WorkGCF, TRN_TYPE_SPLITA);
        for (i = 0; i < NUM_SEAT; i++) { //SR206
            if (WorkGCF->auchFinSeat[i] == UifTotal->uchSeatNo) {
                return(LDT_PROHBT_ADR);
            }
        }

        /*----- Prohibit Used Seat# -----*/
        TrnGetGCFQual(&WorkGCF);
        for (i = 0; i < NUM_SEAT; i++) { //SR206
            if (WorkGCF->auchFinSeat[i] == UifTotal->uchSeatNo) {
                return(LDT_PROHBT_ADR);
            }
        }

        /*----- Prohibit Not Used Seat# -----*/
        for (i = 0; i < NUM_SEAT; i++) { //SR206
            if (WorkGCF->auchUseSeat[i] == UifTotal->uchSeatNo) {
                break;
            }
        }
        if (i >= NUM_SEAT) { //SR206
            return(LDT_PROHBT_ADR);
        }
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftMI(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
*   Input:
*   Output:
*   InOut:      none
*   Return:
*
*   Description:    calculate MI for split total.
*==========================================================================
*/
SHORT   ItemTotalSESoftMI(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
{
    /*----- Whole MI -----*/
    pItem->lMI = TranItemizersPtr->lMI;

    /* --- Whole Tax Print Option "99", V3.3 --- */
    if (UifTotal->uchSeatNo != ITM_SPLIT_WHOLE_TAX_PRNIT) {
		SHORT   i;

        for (i = 0; TranGCFQualPtr->auchUseSeat[i] != 0; i++) {
            if (TrnSplGetSeatTrans(TranGCFQualPtr->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
                continue;
            }

            /*----- Individual Tax -----*/
            if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
				ITEMCOMMONVAT   WorkVAT = {0};

				ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
                pItem->lMI += WorkVAT.lPayment;
            } else {
				ITEMCOMMONTAX   WorkTax = {0};

				if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
                    ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
                } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
                    ItemCurTaxSpl(&pItem->auchTotalStatus[1], &WorkTax);
                }
                pItem->lMI += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
            }
        }
        /*----- Base Transaction -----*/
        if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_ERROR) {
            return(ITM_SUCCESS);
        }

    /* --- Whole Tax Print Option "99", V3.3 --- */
    } else {
        TrnSplGetBaseTransSeat(TRN_CONSOLI_STO);
    }

    /*----- Tax of Base Transaction -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
		ITEMCOMMONVAT   WorkVAT = {0};

        ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
        pItem->lMI += WorkVAT.lPayment;
    } else {
		ITEMCOMMONTAX   WorkTax = {0};

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(&pItem->auchTotalStatus[1], &WorkTax);
        }
        pItem->lMI += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTotalSEMakeCPTotal(ITEMTOTAL *pItem)
*   Input:
*   Output:
*   InOut:      none
*   Return:
*
*   Description:    Total/Tax for Charge Posting.               V3.3
*==========================================================================
*/
VOID    ItemTotalSEMakeCPTotal(ITEMTOTAL *pItem)
{
    /*----- Whole MI -----*/
    pItem->lMI = TranItemizersPtr->lMI;

    TrnSplGetBaseTransSeat(TRN_CONSOLI_STO);

    /*----- Tax of Base Transaction -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
		ITEMCOMMONVAT   WorkVAT = {0};

		ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
        /* ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_SPLIT); */
        pItem->lTax = WorkVAT.lPayment;
        pItem->lMI += WorkVAT.lPayment;
    } else {
		ITEMCOMMONTAX   WorkTax = {0};

		if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT);
            /* ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_SPLIT); */

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTax(&pItem->auchTotalStatus[1], &WorkTax);
            /* ItemCurTaxSpl(&pItem->auchTotalStatus[1], &WorkTax); */
        }

        pItem->lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
        pItem->lMI += pItem->lTax;
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSESoftCheckSplSingle(UIFREGTOTAL *UifTotal,
*                           ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*
*   Return:
*
*   Description:    Split Check Single Print.
==========================================================================*/

SHORT   ItemTotalSESoftCheckSplSingle(UIFREGTOTAL *UifTotal,
                        ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
{
	ITEMAFFECTION   IndTax = {0};
    ITEMTOTAL       IndTotal = {0};
    DCURRENCY       lTax = 0;

    TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

    /*----- Get Target Seat# Transaction -----*/
    if (TrnSplGetSeatTrans(UifTotal->uchSeatNo, TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    /*----- Save Number 5/28/96 -----*/
    memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));

    /*----- Make Tax, Total Data -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        ItemTotalSESoftCheckVATSpl(pWholeTotal->auchTotalStatus, &IndTax, &lTax);
    } else {
        ItemTotalSESoftCheckTaxSpl(pWholeTotal->auchTotalStatus, &IndTax);
    }

    /* --- Pre-Authorization,  V3.3 --- */
    if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
		return(LDT_PROHBT_ADR);
	}

    ItemTotalSESoftItemSpl(UifTotal, pWholeTotal, &IndTax, 0, &IndTotal, lTax);

    *pWholeTax = IndTax;

    /*----- Trailer Print -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal) && (CliParaMDCCheckField(MDC_PREAUTH_PADR, MDC_PARAM_BIT_C))) { 
		ItemTrailer(TYPE_SPLITA_EPT);
	} else {
		ItemTrailer(TYPE_SPLITA);
	}

    /*----- Buffering Print 4/19/96, 6/9/96 -----*/
    if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
        TrnSplPrintSeat(UifTotal->uchSeatNo, TRN_SPL_NOT_UPDATE, 0, 0, pWholeTotal->auchTotalStatus, TRN_PRT_FULL);
    }

    /*----- Issue Total Stub 5/29/96 -----*/
    if ((ITM_TTL_FLAG_TOTAL_TYPE(pWholeTotal) == PRT_SERVICE2) ||  /* R3.3 */
        (ITM_TTL_FLAG_TOTAL_TYPE(pWholeTotal) == PRT_CASINT2)) {
        ItemTotalStubSpl(&IndTotal, UifTotal->uchSeatNo);
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
*   Synopsis:   VOID    ItemTotalSESoftCheckTaxSpl(UCHRA *puchTotal, ITEMAFFECTION *pIndTax)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax data for split file.
==========================================================================*/

VOID    ItemTotalSESoftCheckTaxSpl(UCHAR *auchTotalStatus, ITEMAFFECTION *pIndTax)
{
    /*----- Individual Tax -----*/
    memset(pIndTax, 0, sizeof(ITEMAFFECTION));
    pIndTax->uchMajorClass = CLASS_ITEMAFFECTION;
    pIndTax->uchMinorClass = CLASS_TAXPRINT;
    pIndTax->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    /*----- Make Subtotal,   R3.1 5/8/96 -----*/
    if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
		// Print the subtotal line before the tax line so we need to get the current subtotal.
		TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);

        pIndTax->lAmount = WorkTI->lMI;
    }

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
		ITEMCOMMONTAX   WorkTax = {0};

        ItemCanTax(auchTotalStatus + 1, &WorkTax, ITM_SPLIT);
        ItemCanTaxCopy1(pIndTax, &WorkTax);

        /*----- adjust tax print format 4/17/96 -----*/
        ItemCanTaxCopy2(pIndTax);
        if (auchTotalStatus[1] != 0) {    // is there any Canadian tax that needs to be affected?
            pIndTax->fbStorageStatus = NOT_ITEM_STORAGE;
            TrnItemSpl(pIndTax, TRN_TYPE_SPLITA);
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
		ITEMCOMMONTAX   WorkTax = {0};

        ItemCurTaxSpl(auchTotalStatus + 1, &WorkTax);
        ItemUSTaxCopy1(pIndTax, &WorkTax);
		// check total key tax provisioning: ITM_TTL_FLAG_TAX_1(), ITM_TTL_FLAG_TAX_2(), or ITM_TTL_FLAG_TAX_3().
        if ((auchTotalStatus[1] & 0x01)
            || (auchTotalStatus[1] & 0x10) || (auchTotalStatus[2] & 0x01)) {
            pIndTax->fbStorageStatus = NOT_ITEM_STORAGE;
            TrnItemSpl(pIndTax, TRN_TYPE_SPLITA);
        }
    }
}

/*==========================================================================
*   Synopsis:   VOID    ItemTotalSESoftCheckTaxSpl(UCHRA *puchTotal, ITEMAFFECTION *pIndTax)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax data for split file.
==========================================================================*/

VOID    ItemTotalSESoftCheckVATSpl(UCHAR *auchTotalStatus, ITEMAFFECTION *pIndVAT, DCURRENCY *plPayment)
{
	ITEMCOMMONVAT   WorkVAT = {0};

    ItemCurVAT(auchTotalStatus + 1, &WorkVAT, ITM_SPLIT);

    memset(pIndVAT, 0, sizeof(ITEMAFFECTION));
    memcpy(&pIndVAT->USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
    pIndVAT->lAmount = WorkVAT.lService;

    pIndVAT->fbStorageStatus = NOT_ITEM_STORAGE;
    ItemTendVATCommon(pIndVAT, ITM_SPLIT, ITM_NOT_DISPLAY);

    *plPayment = WorkVAT.lPayment;
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftItemSpl(UIFREGTOTAL *UifTotal,
*                           ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pIndTax,
*                           UCAHR uchTrail, ITEMTOTAL *pIndTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate service total data
==========================================================================*/

VOID    ItemTotalSESoftItemSpl(UIFREGTOTAL *UifTotal,
                ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pIndTax,
                UCHAR uchTrail, ITEMTOTAL *pIndTotal, DCURRENCY lPayment)
{
    DCURRENCY       lTax = 0;
	ITEMTOTAL       IndTotal = {0};
    TRANITEMIZERS   * const WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();

    /*----- Make Service Total Data -----*/
    IndTotal.uchMajorClass = pWholeTotal->uchMajorClass = CLASS_ITEMTOTAL;
    IndTotal.uchMinorClass = pWholeTotal->uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);

    /*----- Save Number 5/28/96 -----*/
    if (uchTrail == 0) {
        memcpy(IndTotal.aszNumber, pWholeTotal->aszNumber, sizeof(IndTotal.aszNumber));
    }

    memcpy(&IndTotal.auchTotalStatus, pWholeTotal->auchTotalStatus, sizeof(IndTotal.auchTotalStatus));

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        lTax = lPayment;
    } else {
        lTax = pIndTax->USCanVAT.USCanTax.lTaxAmount[0] + pIndTax->USCanVAT.USCanTax.lTaxAmount[1]
            + pIndTax->USCanVAT.USCanTax.lTaxAmount[2] + pIndTax->USCanVAT.USCanTax.lTaxAmount[3];
    }
    IndTotal.lMI = WorkTI->lMI + lTax;
    IndTotal.lTax = lTax;
    IndTotal.lService = WorkTI->lMI + lTax;

    IndTotal.usConsNo = pWholeTotal->usConsNo = TranCurQualPtr->usConsNo;
    IndTotal.uchLineNo = pWholeTotal->uchLineNo = WorkGCF->uchSlipLine;
/* --- V3.3 ---
    if (TranModeQualPtr->usWaiterID) {
        IndTotal.usID = pWholeTotal->usID = TranModeQualPtr->usWaiterID;
    } else {
V3.3 */
        IndTotal.ulID = pWholeTotal->ulID = TranModeQualPtr->ulCashierID;
/*    } */

    /*----- save totalkey status 4/16/96 -----*/
    TranCurQualTotalStatusPut (pWholeTotal->auchTotalStatus);

    _tcsncpy(IndTotal.aszFolioNumber, pWholeTotal->aszFolioNumber, NUM_FOLIO);
    _tcsncpy(IndTotal.auchExpiraDate, pWholeTotal->auchExpiraDate, NUM_EXPIRA);
    memcpy(IndTotal.aszCPMsgText, pWholeTotal->aszCPMsgText, NUM_CPRSPCO * NUM_CPRSPTEXT);
    TrnItemSpl(&IndTotal, TRN_TYPE_SPLITA);

    /* ---- for print control ---- */
    /*----- Save Ind. Total 5/29/96 ------*/
    *pIndTotal = IndTotal;
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftDispTaxTotal(ITEMAFFECTION *pTax,
*                                   UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Display Tax/Total on 1 LCD.
==========================================================================*/

VOID    ItemTotalSESplDispTaxTotal(ITEMAFFECTION *pTax,
                            UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal)
{
    SHORT               k;
    DCURRENCY           lSaveAmount, lTax;
	MLDTRANSDATA        WorkMLD = {0};
    TRANGCFQUAL         * const WorkGCF = TrnGetGCFQualPtr();

    /*----- Return to LCD 1 Display -----*/
    MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);
    WorkMLD.usVliSize = TrnInformationPtr->usTranConsStoVli;
    WorkMLD.sFileHandle = TrnInformationPtr->hsTranConsStorage;
    WorkMLD.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
    MldPutTransToScroll(&WorkMLD);

    /*----- re-make Total for Display 4/16/96 -----*/
    pTotal->lMI = TranItemizersPtr->lMI;

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        memset(&pTax->USCanVAT.ItemVAT[0], 0, sizeof(ITEMVAT)*NUM_VAT);
        pTax->lAmount = 0;
    } else {
		SHORT   j;

        for(j = 0; j < 4; j++) {
            pTax->USCanVAT.USCanTax.lTaxAmount[j] = 0L;
        }

        /*----- Make Sub Total 4/25/96 -----*/
        lSaveAmount = pTax->lAmount;
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            pTax->lAmount = TranItemizersPtr->lMI;
        }
    }
    lTax = 0L;

    for (k = 0; WorkGCF->auchUseSeat[k] != 0; k++) {
		ITEMCOMMONTAX       WorkTax = {0};
		ITEMCOMMONVAT       WorkVAT = {0};

        if (TrnSplGetSeatTrans(WorkGCF->auchUseSeat[k], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
            continue;
        }

        /*----- Individual Tax -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
			SHORT   j;

            ItemCanTax(&pTotal->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
            for (j = 0; j < 5; j++) {
                pTax->USCanVAT.USCanTax.lTaxable[j] += WorkTax.alTaxable[j];
            }
            for (j = 0; j < 4; j++) {
                pTax->USCanVAT.USCanTax.lTaxAmount[j] += WorkTax.alTax[j];
                pTax->USCanVAT.USCanTax.lTaxExempt[j] += WorkTax.alTaxExempt[j];
                lTax += WorkTax.alTax[j];
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
			SHORT   j;

            ItemCurTaxSpl(&pTotal->auchTotalStatus[1], &WorkTax);
            for (j = 0; j < 3; j++) {
                pTax->USCanVAT.USCanTax.lTaxable[j] += WorkTax.alTaxable[j];
                pTax->USCanVAT.USCanTax.lTaxAmount[j] += WorkTax.alTax[j];
                pTax->USCanVAT.USCanTax.lTaxExempt[j] += WorkTax.alTaxExempt[j];
                lTax += WorkTax.alTax[j];
            }
        } else {
			SHORT   j;

            ItemCurVAT(&pTotal->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
            for (j = 0; j < NUM_VAT; j++) {
                if (WorkVAT.ItemVAT[j].lVATRate) {
                    pTax->USCanVAT.ItemVAT[j].lVATRate  = WorkVAT.ItemVAT[j].lVATRate;
                }
                pTax->USCanVAT.ItemVAT[j].lVATable += WorkVAT.ItemVAT[j].lVATable;
                pTax->USCanVAT.ItemVAT[j].lVATAmt  += WorkVAT.ItemVAT[j].lVATAmt;
                pTax->USCanVAT.ItemVAT[j].lSum     += WorkVAT.ItemVAT[j].lSum;
                pTax->USCanVAT.ItemVAT[j].lAppAmt  += WorkVAT.ItemVAT[j].lAppAmt;
            }
            pTax->lAmount += WorkVAT.lService;
            lTax += WorkVAT.lPayment;
        }
    }

    /*----- Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_SUCCESS) {
		ITEMCOMMONTAX       WorkTax = {0};
		ITEMCOMMONVAT       WorkVAT = {0};

        /*----- Tax of Base Transaction -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
			SHORT   j;

            ItemCanTax(&pTotal->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
            for (j = 0; j < 5; j++) {
                pTax->USCanVAT.USCanTax.lTaxable[j] += WorkTax.alTaxable[j];
            }
            for (j = 0; j < 4; j++) {
                pTax->USCanVAT.USCanTax.lTaxAmount[j] += WorkTax.alTax[j];
                pTax->USCanVAT.USCanTax.lTaxExempt[j] += WorkTax.alTaxExempt[j];
                lTax += WorkTax.alTax[j];
            }

            /*----- adjust tax print format 4/17/96 -----*/
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                pTax->USCanVAT.USCanTax.lTaxAmount[1] += pTax->USCanVAT.USCanTax.lTaxAmount[2]
                    + pTax->USCanVAT.USCanTax.lTaxAmount[3];
                pTax->USCanVAT.USCanTax.lTaxAmount[2] = pTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {
                pTax->USCanVAT.USCanTax.lTaxAmount[0] += pTax->USCanVAT.USCanTax.lTaxAmount[1]
                    + pTax->USCanVAT.USCanTax.lTaxAmount[2] + pTax->USCanVAT.USCanTax.lTaxAmount[3];
                pTax->USCanVAT.USCanTax.lTaxAmount[1] = pTax->USCanVAT.USCanTax.lTaxAmount[2]
                    = pTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
			SHORT   j;

            ItemCurTaxSpl(&pTotal->auchTotalStatus[1], &WorkTax);
            for (j = 0; j < 3; j++) {
                pTax->USCanVAT.USCanTax.lTaxable[j] += WorkTax.alTaxable[j];
                pTax->USCanVAT.USCanTax.lTaxAmount[j] += WorkTax.alTax[j];
                pTax->USCanVAT.USCanTax.lTaxExempt[j] += WorkTax.alTaxExempt[j];
                lTax += WorkTax.alTax[j];
            }
        } else {
			SHORT   j;

            ItemCurVAT(&pTotal->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
            for (j = 0; j < NUM_VAT; j++) {
                if (WorkVAT.ItemVAT[j].lVATRate) {
                    pTax->USCanVAT.ItemVAT[j].lVATRate  = WorkVAT.ItemVAT[j].lVATRate;
                }
                pTax->USCanVAT.ItemVAT[j].lVATable += WorkVAT.ItemVAT[j].lVATable;
                pTax->USCanVAT.ItemVAT[j].lVATAmt  += WorkVAT.ItemVAT[j].lVATAmt;
                pTax->USCanVAT.ItemVAT[j].lSum     += WorkVAT.ItemVAT[j].lSum;
                pTax->USCanVAT.ItemVAT[j].lAppAmt  += WorkVAT.ItemVAT[j].lAppAmt;
            }
            pTax->lAmount += WorkVAT.lService;
            lTax += WorkVAT.lPayment;
        }
    }

    /*----- Whole Tax for Display -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        pTax->fbStorageStatus = (NOT_CONSOLI_STORAGE|NOT_ITEM_STORAGE);
        ItemTendVATCommon(pTax, ITM_NOT_SPLIT, ITM_DISPLAY);
    } else {
        pTax->uchMajorClass = CLASS_ITEMAFFECTION;
        pTax->uchMinorClass = CLASS_TAXPRINT;
        pTax->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        MldScrollWrite(pTax, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(pTax);

        /* send to enhanced kds, 2172 */
        ItemSendKds(pTax, 0);

        /*----- Restore pTax->lAmount 4/25/96 -----*/
        pTax->lAmount = lSaveAmount;
    }
    /*----- Whole Total for Display -----*/
    pTotal->uchMajorClass = CLASS_ITEMTOTAL;
    pTotal->uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);
    pTotal->lMI += lTax;
    MldScrollWrite(pTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pTotal);

    /* send to enhanced kds, 2172 */
    ItemSendKds(pTotal, 0);

    /*----- for KP printer 5/13/96 -----*/
	{
		UCHAR   fbSaveStorageStatus = pTotal->fbStorageStatus;    /* 5/13/96 */

		pTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
		TrnItem(pTotal);

		pTotal->fbStorageStatus = fbSaveStorageStatus;
	}
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftDispTaxTotalWhole(ITEMAFFECTION *pTax,
*                               UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Display Tax/Total on 1 LCD.
==========================================================================*/

VOID    ItemTotalSESplDispTaxTotalWhole(ITEMAFFECTION *pTax,
                            UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal)
{
	MLDTRANSDATA        WorkMLD = {0};

    /*----- Return to LCD 1 Display -----*/
    MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);
    WorkMLD.usVliSize = TrnInformationPtr->usTranConsStoVli;
    WorkMLD.sFileHandle = TrnInformationPtr->hsTranConsStorage;
    WorkMLD.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
    MldPutTransToScroll(&WorkMLD);

    /*----- Whole Tax for Display -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        pTax->fbStorageStatus = (NOT_CONSOLI_STORAGE|NOT_ITEM_STORAGE);
        ItemTendVATCommon(pTax, ITM_NOT_SPLIT, ITM_DISPLAY);
    } else {
        pTax->uchMajorClass = CLASS_ITEMAFFECTION;
        pTax->uchMinorClass = CLASS_TAXPRINT;
        pTax->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        MldScrollWrite(pTax, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(pTax);

        /* send to enhanced kds, 2172 */
        ItemSendKds(pTax, 0);
    }
    /*----- Whole Total for Display -----*/
    pTotal->uchMajorClass = CLASS_ITEMTOTAL;
    pTotal->uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);
    MldScrollWrite(pTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pTotal);

    /* send to enhanced kds, 2172 */
    ItemSendKds(pTotal, 0);

    /*----- for KP printer -----*/
	{
		UCHAR   fbSaveStorageStatus = pTotal->fbStorageStatus;

		pTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
		TrnItem(pTotal);

		pTotal->fbStorageStatus = fbSaveStorageStatus;
	}
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftAffectSpl(UIFREGTOTAL *UifTotal,
*                               ITEMTOTAL *WholeTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Generate Hourly, Service Total Data.  This entire function
*					sends information to be affected later on by the TotalTUM thread
*					which will add these totals to the hourly reports.
==========================================================================*/

VOID    ItemTotalSESoftAffectSpl(UIFREGTOTAL *UifTotal, ITEMTOTAL *WholeTotal)
{
    USHORT          i;
	ITEMAFFECTION   Affect = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();

    WorkGCF->lCurBalance = WorkTI->lMI;

    /*----- Make Hourly Data Class -----*/
    Affect.uchMajorClass = CLASS_ITEMAFFECTION;
    Affect.uchMinorClass = CLASS_HOURLY;
    Affect.lAmount = WorkTI->lHourly;
    Affect.sItemCounter = WorkTI->sItemCounter;
    Affect.sNoPerson = ItemTendNoPerson();
    Affect.uchOffset = TranGCFQualPtr->uchHourlyOffset;
    Affect.fbStorageStatus = NOT_CONSOLI_STORAGE;
    /* V3.3 */
    for (i = 0; i < STD_NO_BONUS_TTL; i++) {
        Affect.lBonus[i] = WorkTI->lHourlyBonus[i];                /* hourly bonus total */
    }
    TrnItem(&Affect);


	//By commenting out the below code, this alleviates the problem of the service totals
	//becoming inflated.  By removing the code below, we dont send an extra price to the Service
	//total information.
	// SR278
    /*----- Make Service Total Affection Data Class -----*/
    memset(&Affect, 0, sizeof(ITEMAFFECTION));
    Affect.uchMajorClass = CLASS_ITEMAFFECTION;
    Affect.uchMinorClass = CLASS_SERVICE;
    Affect.lAmount = WorkTI->lMI - WorkTI->lService[0] - WorkTI->lService[1] - WorkTI->lService[2];
    Affect.uchAddNum = ItemTotalSEGetSerID(WholeTotal);
    Affect.uchTotalID = ItemTotalSESetClass(UifTotal->uchMinorClass);
    Affect.fbStorageStatus = NOT_CONSOLI_STORAGE;
    TrnItem(&Affect);

    WorkTI->lService[Affect.uchAddNum] += Affect.lAmount;          /* update current service total */
    WorkTI->lHourly = 0L;                                          /* clear hourly itemizer */
    WorkTI->sItemCounter = 0;                                      /* clear item counter */
	//We now clear the hourly bonus totals, because we have already affected them to the Hourly Totals,
	//by doing this, we no longer run into the problem when the user opens up a check, rings up items, then
	//closes the check, and when they tender it out, it updates the bonuses incorrectly.
	// SR278
	memset(&WorkTI->lHourlyBonus, 0x00, sizeof(WorkTI->lHourlyBonus));
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftCloseSpl(ITEMTOTAL *pItem)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate transaction close data
==========================================================================*/

VOID    ItemTotalSESoftCloseSpl(ITEMTOTAL *pItem)
{
    SHORT           sStatus;
	ITEMTRANSCLOSE  ItemTransClose = {0};
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();

    pWorkCur->uchPrintStorage = PRT_NOSTORAGE;

    if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT3)) {
        pWorkCur->uchKitchenStorage = PRT_KIT_NET_STORAGE;    // void item is to be printed on kitchen printer
    } else {
        pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;        // void item is NOT to be printed on kitchen printer
    }

    if ((CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT1) == 0  && (pWorkCur->fsCurStatus & CURQUAL_PRESELECT_MASK))
        || (pWorkCur->fsCurStatus & CURQUAL_TRAINING))
	{
		/* if kitchen send at pvoid is turned off and transaction is a preselect void or
		   if this is a training operator
		*/
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;
    }

    if (ITM_TTL_FLAG_COMPULSORY_SLIP(pItem)) {            /* compulsory slip */
        pWorkCur->fbCompPrint = PRT_SLIP;
    }

/* 07/06/98
    if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
        pWorkCur->fsCurStatus &= ~CURQUAL_ADDCHECK;
        pWorkCur->fsCurStatus |= (CURQUAL_REORDER | CURQUAL_ADDCHECK_SERVT);
    }
*/

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSPRINT;
    TrnClose(&ItemTransClose);

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);

    while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
		USHORT  usStatus = TtlConvertError(sStatus);
        UieErrorMsg(usStatus, UIE_WITHOUT);
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftCheckSplAll(UIFREGTOTAL *UifTotal,
*                           ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*
*   Return:
*
*   Description:    Split Check All Print.
==========================================================================*/

VOID    ItemTotalSESoftCheckSplAll(UIFREGTOTAL *UifTotal,
                        ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
{
    UCHAR           uchTrail;
    SHORT           i, j;
    USHORT          usCnt = 0;
    DCURRENCY       lTax = 0L;
	ITEMTOTAL       IndTotal = {0};
    ITEMTOTAL       IndTotalTmp = {0};        /* 5/29/96 */
    ITEMAFFECTION   IndTax = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();

    /*----- Decide Print Type -----*/
    if ((UifTotal->fbInputStatus & INPUT_0_ONLY) && UifTotal->uchSeatNo == 0) {
        uchTrail = 0;
        /*----- Save Number 5/28/96 -----*/
        memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));
    } else {
        uchTrail = 1;
    }

     for (i = 0, usCnt = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++) { //SR206
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

        /*----- Make Split Transaction -----*/
        if (TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
            continue;
        }

        /*----- Make Tax, Total Data -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            ItemTotalSESoftCheckVATSpl(pWholeTotal->auchTotalStatus, &IndTax, &lTax);
        } else {
            ItemTotalSESoftCheckTaxSpl(pWholeTotal->auchTotalStatus, &IndTax);
        }
        ItemTotalSESoftItemSpl(UifTotal, pWholeTotal, &IndTax, uchTrail, &IndTotalTmp, lTax);

        /*----- Whole Taxable/Tax/Exempt -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            for (j = 0; j < NUM_VAT; j++) {
                if (IndTax.USCanVAT.ItemVAT[j].lVATRate) {
                    pWholeTax->USCanVAT.ItemVAT[j].lVATRate  = IndTax.USCanVAT.ItemVAT[j].lVATRate;
                }
                pWholeTax->USCanVAT.ItemVAT[j].lVATable += IndTax.USCanVAT.ItemVAT[j].lVATable;
                pWholeTax->USCanVAT.ItemVAT[j].lVATAmt  += IndTax.USCanVAT.ItemVAT[j].lVATAmt;
                pWholeTax->USCanVAT.ItemVAT[j].lSum     += IndTax.USCanVAT.ItemVAT[j].lSum;
                pWholeTax->USCanVAT.ItemVAT[j].lAppAmt  += IndTax.USCanVAT.ItemVAT[j].lAppAmt;
            }
            pWholeTax->lAmount += IndTax.lAmount;
        } else {
            for (j = 0; j < 5; j++) {
                pWholeTax->USCanVAT.USCanTax.lTaxable[j] += IndTax.USCanVAT.USCanTax.lTaxable[j];
            }
            for (j = 0; j < 4; j++) {
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[j] += IndTax.USCanVAT.USCanTax.lTaxAmount[j];
                pWholeTax->USCanVAT.USCanTax.lTaxExempt[j] += IndTax.USCanVAT.USCanTax.lTaxExempt[j];
            }
        }
        /*----- Trailer Print -----*/
        if (uchTrail == 0) {
            ItemTrailer(TYPE_SPLITA);
        }

        /*----- Buffering Print 4/19/96, 6/9/96 -----*/
        if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, uchTrail, usCnt, pWholeTotal->auchTotalStatus, TRN_PRT_FULL);
        }

        if (uchTrail == 0) {
            /*----- Issue Total Stub 5/29/96 -----*/
            if ((pWholeTotal->auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_SERVICE2) ||  /* R3.3 */
                (pWholeTotal->auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_CASINT2)) {
                ItemTotalStubSpl(&IndTotalTmp, WorkGCF->auchUseSeat[i]);
            }
        }
        usCnt++;
    }

    /*----- save totalkey status 4/16/96 -----*/
    TranCurQualTotalStatusPut (pWholeTotal->auchTotalStatus);

    /*----- Get Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_SUCCESS) {
		TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        /*----- Make Subtotal,   R3.1 5/8/96 -----*/
        if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                IndTax.lAmount = WorkTI->lMI;
            }
        }

        /*----- Individual Tax of Base Transaction -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&pWholeTotal->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(&pWholeTotal->auchTotalStatus[1], &WorkTax);
        } else {
            ItemCurVAT(&pWholeTotal->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
        }
        IndTax.uchMajorClass = CLASS_ITEMAFFECTION;
        IndTax.uchMinorClass = CLASS_TAXPRINT;
        IndTax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            for (i = 0; i < 5; i++) {
                IndTax.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
                pWholeTax->USCanVAT.USCanTax.lTaxable[i] += IndTax.USCanVAT.USCanTax.lTaxable[i];
            }
            for (i = 0, lTax = 0L; i < 4; i++) {
                IndTax.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
                IndTax.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[i] += IndTax.USCanVAT.USCanTax.lTaxAmount[i];
                pWholeTax->USCanVAT.USCanTax.lTaxExempt[i] += IndTax.USCanVAT.USCanTax.lTaxExempt[i];
                lTax += WorkTax.alTax[i];
            }

            /*----- adjust tax print format 4/17/96 -----*/
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                IndTax.USCanVAT.USCanTax.lTaxAmount[1] += IndTax.USCanVAT.USCanTax.lTaxAmount[2]
                    + IndTax.USCanVAT.USCanTax.lTaxAmount[3];

                /* correct 07/11/96 */
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[1] += IndTax.USCanVAT.USCanTax.lTaxAmount[2]
                    + IndTax.USCanVAT.USCanTax.lTaxAmount[3];

                IndTax.USCanVAT.USCanTax.lTaxAmount[2] = IndTax.USCanVAT.USCanTax.lTaxAmount[3] = 0L;

                /* correct 07/11/96 */
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[2] = pWholeTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {
                IndTax.USCanVAT.USCanTax.lTaxAmount[0] += IndTax.USCanVAT.USCanTax.lTaxAmount[1]
                    + IndTax.USCanVAT.USCanTax.lTaxAmount[2] + IndTax.USCanVAT.USCanTax.lTaxAmount[3];

                /* correct 07/11/96 */
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[0] += IndTax.USCanVAT.USCanTax.lTaxAmount[1]
                    + IndTax.USCanVAT.USCanTax.lTaxAmount[2] + IndTax.USCanVAT.USCanTax.lTaxAmount[3];

                IndTax.USCanVAT.USCanTax.lTaxAmount[1] = IndTax.USCanVAT.USCanTax.lTaxAmount[2]
                    = IndTax.USCanVAT.USCanTax.lTaxAmount[3] = 0L;

                /* correct 07/11/96 */
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[1] = pWholeTax->USCanVAT.USCanTax.lTaxAmount[2]
                    = pWholeTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;

            }
            TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            for (i = 0, lTax = 0L; i < 3; i++) {
                IndTax.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
                IndTax.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
                IndTax.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
                pWholeTax->USCanVAT.USCanTax.lTaxable[i] += IndTax.USCanVAT.USCanTax.lTaxable[i];
                pWholeTax->USCanVAT.USCanTax.lTaxAmount[i] += IndTax.USCanVAT.USCanTax.lTaxAmount[i];
                pWholeTax->USCanVAT.USCanTax.lTaxExempt[i] += IndTax.USCanVAT.USCanTax.lTaxExempt[i];
                lTax += WorkTax.alTax[i];
            }
            TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);
        } else {
            lTax = 0L;
            memset(&IndTax, 0, sizeof(ITEMAFFECTION));
            memcpy(&IndTax.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
            IndTax.lAmount = WorkVAT.lService;
            IndTax.fbStorageStatus = NOT_ITEM_STORAGE;
            ItemTendVATCommon(&IndTax, ITM_SPLIT, ITM_DISPLAY);

            for (j=0; j<NUM_VAT; j++) {
                if (WorkVAT.ItemVAT[j].lVATRate) {
                    pWholeTax->USCanVAT.ItemVAT[j].lVATRate  = WorkVAT.ItemVAT[j].lVATRate;
                }
                pWholeTax->USCanVAT.ItemVAT[j].lVATable += WorkVAT.ItemVAT[j].lVATable;
                pWholeTax->USCanVAT.ItemVAT[j].lVATAmt  += WorkVAT.ItemVAT[j].lVATAmt;
                pWholeTax->USCanVAT.ItemVAT[j].lSum     += WorkVAT.ItemVAT[j].lSum;
                pWholeTax->USCanVAT.ItemVAT[j].lAppAmt  += WorkVAT.ItemVAT[j].lAppAmt;
            }
            pWholeTax->lAmount += WorkVAT.lService;
            lTax += WorkVAT.lPayment;
        }

        /*----- Individual Total of Base Transaction -----*/
        if ((uchTrail == 1) && (usCnt == 0)) {              /* 5/28/96 */
            /*----- Save Number 5/28/96 -----*/
            memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));
        }
        IndTotal = *pWholeTotal;
        IndTotal.uchMajorClass = CLASS_ITEMTOTAL;
        IndTotal.uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);
        IndTotal.lMI = WorkTI->lMI + lTax;
        IndTotal.lTax = lTax;
        IndTotal.lService =  WorkTI->lMI + lTax;
        TrnItemSpl(&IndTotal, TRN_TYPE_SPLITA);

        /*----- check validation compulsory 5/15/96 -----*/
		if ( !CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
	        if (ITM_TTL_FLAG_PRINT_VALIDATION(&IndTotal)) {
				DCURRENCY       lTmpMI = IndTotal.lMI;             /* 5/15/96 */

				IndTotal.lMI = pWholeTotal->lMI;
            	IndTotal.fsPrintStatus = PRT_VALIDATION;
	            TrnThrough(&IndTotal);
    	        IndTotal.lMI = lTmpMI;
        	    IndTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	        }
	    }

        if ((uchTrail == 1) && (usCnt != 0)) {              /* 5/17/96 */
            /*----- Save Number 5/28/96 -----*/
            memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));
            ItemTotalSESoftGTtlSpl( pWholeTotal, pWholeTax );
        }

		if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal) && (CliParaMDCCheckField(MDC_PREAUTH_PADR, MDC_PARAM_BIT_C))) {
 			ItemTrailer(TYPE_SPLITA_EPT);
		} else {
			ItemTrailer(TYPE_SPLITA);
		}

        /*----- Buffering Print 4/19/96-----*/
        if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
            if (usCnt) {
                TrnSplPrintBase(uchTrail, usCnt, pWholeTotal->auchTotalStatus);
				if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
					// Duplicate print the ticket to allow for customer and merchant copies
					TrnSplPrintBase(uchTrail, usCnt, pWholeTotal->auchTotalStatus);
				}
            } else {    /* 07/06/98 */
                /* only no seat no. print after seat no. tender */
                TrnSplPrintBase(0, usCnt, pWholeTotal->auchTotalStatus);
				if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
					// Duplicate print the ticket to allow for customer and merchant copies
					TrnSplPrintBase(0, usCnt, pWholeTotal->auchTotalStatus);
				}
            }
        }
        if (uchTrail == 0) {                    /* 5/29/96 */
            /*----- Issue Total Stub 5/29/96 -----*/
            if ((ITM_TTL_FLAG_TOTAL_TYPE(pWholeTotal) == PRT_SERVICE2) ||  /* R3.3 */
                (ITM_TTL_FLAG_TOTAL_TYPE(pWholeTotal) == PRT_CASINT2)) {
                ItemTotalStubSpl(&IndTotal, 0);
            }
        }

		/*----- check validation compulsory 5/15/96 -----*/
		if ( CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_D) ) { /* both validation and slip print */
	        if (ITM_TTL_FLAG_PRINT_VALIDATION(&IndTotal)) {
				DCURRENCY       lTmpMI = IndTotal.lMI;             /* 5/15/96 */

				IndTotal.lMI = pWholeTotal->lMI;
            	IndTotal.fsPrintStatus = PRT_VALIDATION;
	            TrnThrough(&IndTotal);
    	        IndTotal.lMI = lTmpMI;
        	    IndTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        	}
        }
    /*----- No Base Trans -----*/
    } else {
        /*----- Seat Trans -----*/
        if (usCnt != 0) {

            /*----- initialize storage file (not write transaction open) 6/10/96 -----*/
            TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

            /*----- check validation compulsory 5/15/96 -----*/
			if ( !CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_D) ) { /* both validation and slip print */
    	        if (ITM_TTL_FLAG_PRINT_VALIDATION(pWholeTotal)) {
        	        pWholeTotal->fsPrintStatus = PRT_VALIDATION;
            	    TrnThrough(pWholeTotal);
                	pWholeTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	            }
	        }

            if (uchTrail == 1) {   /* 4/17/96 */
                /*----- Save Number 5/28/96 -----*/
                _tcsncpy(pWholeTotal->aszNumber, UifTotal->aszNumber, NUM_NUMBER);
                ItemTotalSESoftGTtlSpl( pWholeTotal, pWholeTax );
            }

            ItemTrailer(TYPE_SPLITA);

            /*----- Buffering Print 4/19/96-----*/
            if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
                TrnSplPrintBase(uchTrail, usCnt, pWholeTotal->auchTotalStatus);
				if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
					// Duplicate print the ticket to allow for customer and merchant copies
					TrnSplPrintBase(uchTrail, usCnt, pWholeTotal->auchTotalStatus);
				}
            }

            /*----- check validation compulsory 5/15/96 -----*/
			if ( CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_D) ) { /* both validation and slip print */
	            if (ITM_TTL_FLAG_PRINT_VALIDATION(pWholeTotal)) {
    	            pWholeTotal->fsPrintStatus = PRT_VALIDATION;
        	        TrnThrough(pWholeTotal);
            	    pWholeTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	            }
			}
        }
    }
	if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
		ItemTotalDupRec(pWholeTotal);
	}
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftCheckSplWhole(UIFREGTOTAL *UifTotal,
*                           ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*
*   Return:
*
*   Description:    Split Check All Print with Seat#.               V3.3
==========================================================================*/

VOID    ItemTotalSESoftCheckSplWhole(UIFREGTOTAL *UifTotal,
                        ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax)
{
    SHORT           i;
    USHORT          usCnt = 0;
    DCURRENCY       lTax = 0L;
	ITEMTOTAL       Total = {0};
	ITEMAFFECTION   Tax = {0};
	ITEMCOMMONTAX   WorkTax = {0};
	ITEMCOMMONVAT   WorkVAT = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   *WorkTI;

    for (i = 0, usCnt = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++, usCnt++) { //SR206
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

        /*----- Make Split Transaction -----*/
        if (TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
            continue;
        }

        /*----- Buffering Print -----*/
        if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, usCnt, pWholeTotal->auchTotalStatus, TRN_PRT_FULL);
        }
    }

    /*----- save totalkey status -----*/
    TranCurQualTotalStatusPut (pWholeTotal->auchTotalStatus);

    /*----- Get Base Transaction -----*/
    TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO);

    TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
    if (WorkTI->lMI == 0L) {
        /*----- initialize storage file (not write transaction open) 6/10/96 -----*/
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
    }

    TrnGetTI(&WorkTI);

    /*----- Make Subtotal -----*/
    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            Tax.lAmount = WorkTI->lMI;
        }
    }

    /*----- Individual Tax of Base Transaction -----*/
    memset(&WorkTax, 0, sizeof(ITEMCOMMONTAX));
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax(&pWholeTotal->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT);
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        ItemCurTax(&pWholeTotal->auchTotalStatus[1], &WorkTax);
    } else {
        ItemCurVAT(&pWholeTotal->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
    }

    Tax.uchMajorClass = CLASS_ITEMAFFECTION;
    Tax.uchMinorClass = CLASS_TAXPRINT;
    Tax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        for (i = 0; i < 5; i++) {
            Tax.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
            pWholeTax->USCanVAT.USCanTax.lTaxable[i] += Tax.USCanVAT.USCanTax.lTaxable[i];
        }
        for (i = 0, lTax = 0L; i < 4; i++) {
            Tax.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
            Tax.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
            pWholeTax->USCanVAT.USCanTax.lTaxAmount[i] += Tax.USCanVAT.USCanTax.lTaxAmount[i];
            pWholeTax->USCanVAT.USCanTax.lTaxExempt[i] += Tax.USCanVAT.USCanTax.lTaxExempt[i];
            lTax += WorkTax.alTax[i];
        }

        /*----- adjust tax print format 4/17/96 -----*/
        if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
            Tax.USCanVAT.USCanTax.lTaxAmount[1]
                += Tax.USCanVAT.USCanTax.lTaxAmount[2] + Tax.USCanVAT.USCanTax.lTaxAmount[3];

            pWholeTax->USCanVAT.USCanTax.lTaxAmount[1]
                += Tax.USCanVAT.USCanTax.lTaxAmount[2] + Tax.USCanVAT.USCanTax.lTaxAmount[3];
            Tax.USCanVAT.USCanTax.lTaxAmount[2] = Tax.USCanVAT.USCanTax.lTaxAmount[3] = 0L;
            pWholeTax->USCanVAT.USCanTax.lTaxAmount[2] = pWholeTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;

        } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
            && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {
            Tax.USCanVAT.USCanTax.lTaxAmount[0]
                += Tax.USCanVAT.USCanTax.lTaxAmount[1] + Tax.USCanVAT.USCanTax.lTaxAmount[2] + Tax.USCanVAT.USCanTax.lTaxAmount[3];
            pWholeTax->USCanVAT.USCanTax.lTaxAmount[0]
                += Tax.USCanVAT.USCanTax.lTaxAmount[1] + Tax.USCanVAT.USCanTax.lTaxAmount[2] + Tax.USCanVAT.USCanTax.lTaxAmount[3];
            Tax.USCanVAT.USCanTax.lTaxAmount[1]
                = Tax.USCanVAT.USCanTax.lTaxAmount[2] = Tax.USCanVAT.USCanTax.lTaxAmount[3] = 0L;
            pWholeTax->USCanVAT.USCanTax.lTaxAmount[1]
                = pWholeTax->USCanVAT.USCanTax.lTaxAmount[2] = pWholeTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L;
        }

    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        for (i = 0, lTax = 0L; i < 3; i++) {
            Tax.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
            Tax.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
            Tax.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
            pWholeTax->USCanVAT.USCanTax.lTaxable[i] += Tax.USCanVAT.USCanTax.lTaxable[i];
            pWholeTax->USCanVAT.USCanTax.lTaxAmount[i] += Tax.USCanVAT.USCanTax.lTaxAmount[i];
            pWholeTax->USCanVAT.USCanTax.lTaxExempt[i] += Tax.USCanVAT.USCanTax.lTaxExempt[i];
            lTax += WorkTax.alTax[i];
        }

    } else {
        memcpy(&Tax.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT)*NUM_VAT);
        memcpy(&pWholeTax->USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT)*NUM_VAT);
        Tax.lAmount += WorkVAT.lService;
        pWholeTax->lAmount += WorkVAT.lService;
        lTax = WorkVAT.lPayment;
    }
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        /* set MI to cancel for sub total print */
        Tax.lCancel = WorkTI->lMI;
        Tax.fbStorageStatus = NOT_ITEM_STORAGE;
        ItemTendVATCommon(&Tax, ITM_SPLIT, ITM_DISPLAY);
    } else {
        TrnItemSpl(&Tax, TRN_TYPE_SPLITA);
    }

    /*----- Save Number -----*/
    memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));
    Total = *pWholeTotal;
    Total.uchMajorClass = CLASS_ITEMTOTAL;
    Total.uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);
    Total.lMI = WorkTI->lMI + lTax;
    Total.lTax = lTax;
    Total.lService =  WorkTI->lMI + lTax;
    Total.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    TrnItemSpl(&Total, TRN_TYPE_SPLITA);

    /*----- check validation compulsory -----*/
	if ( !CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
	    if (ITM_TTL_FLAG_PRINT_VALIDATION(&Total)) {
    	    Total.fsPrintStatus = PRT_VALIDATION;
        	TrnThrough(&Total);
	    }
	}

    if (usCnt != 0) {
        /*----- Save Number -----*/
        memcpy(pWholeTotal->aszNumber, UifTotal->aszNumber, sizeof(pWholeTotal->aszNumber));
    }

    ItemTrailer(TYPE_SPLITA);

    /*----- Buffering Print -----*/
    if (ITM_TTL_FLAG_PRINT_BUFFERING(pWholeTotal)) {
        if (usCnt) {
            TrnSplPrintBase(1, usCnt, pWholeTotal->auchTotalStatus);
			if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
				// Duplicate print the ticket to allow for customer and merchant copies
				TrnSplPrintBase(1, usCnt, pWholeTotal->auchTotalStatus);
			}
        } else {
            /* only no seat no. print after seat no. tender */
            TrnSplPrintBase(0, usCnt, pWholeTotal->auchTotalStatus);
			if (ITM_TTL_FLAG_PRE_AUTH(pWholeTotal)) {
				// Duplicate print the ticket to allow for customer and merchant copies
				TrnSplPrintBase(0, usCnt, pWholeTotal->auchTotalStatus);
			}
        }
    }

    /*----- check validation compulsory -----*/
	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
	    if (ITM_TTL_FLAG_PRINT_VALIDATION(&Total)) {
    	    Total.fsPrintStatus = PRT_VALIDATION;
        	TrnThrough(&Total);
        }
	}
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEStoreRecall(UIFREGTOTAL *UifTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      UIFREGTOTAL *UifTotal
*   Return:
*
*   Description:    Store/Recall System.
==========================================================================*/

SHORT   ItemTotalSEStoreRecall(UIFREGTOTAL *UifTotal)
{
    SHORT           sStatus, sItemCounter;
    DCURRENCY       lHourly, lMealTaxable[2];
	ITEMTOTAL       ItemTotal = {0};
    ITEMAFFECTION   TaxAffect = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();
	TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();
    USHORT          fsLimitable = FALSE, fsBaked = FALSE;  // used only when ItemCommonTaxSystem() == ITM_TAX_CANADA
	ULONG			ulMinTotal;

    /*----- Order# -----*/
    ItemTotal.usOrderNo = pWorkGCF->usGuestNo;

    /*----- Send Previous "Item" -----*/
    ItemTotalPrev();

    if ((sStatus = ItemTotalSETaxRef(UifTotal, &ItemTotal, &TaxAffect, &WorkTax)) != ITM_SUCCESS) {
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

    /*----- SoftCheck Print -----*/
    if (ItemTotalSysType(&ItemTotal) == 7) {
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;
        pWorkCur->fbNoPrint |= CURQUAL_NO_J;
    }
    ItemTotalSEAmount(&ItemTotal);

    /*----- Prohibit Charge Posting -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		return(LDT_PROHBT_ADR);
	}

    /*===== Check is Complete, Data Update OK =====*/
	{
		if (pWorkGCF->ulCashierID == 0) {                        /* First Store */
			if (pWorkCur->fsCurStatus & CURQUAL_PVOID) {          /* preselect void */
				pWorkGCF->fsGCFStatus |= GCFQUAL_PVOID;			
				return LDT_PROHBT_ADR;							//SR 806 JHHJ
			}
			if (pWorkCur->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {  /* transaction return */
				pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;			
				return LDT_PROHBT_ADR;							//SR 806 JHHJ
			}
			if (pWorkCur->fsCurStatus & CURQUAL_TRAINING) {       /* training */
				pWorkGCF->fsGCFStatus |= GCFQUAL_TRAINING;
			}
			pWorkGCF->ulCashierID = TranModeQualPtr->ulCashierID;        /* cashier ID of GCF */
		}
	}

    /*----- Tax(Affection) Data -----*/
    ItemTotalSESoftAffectTax(&ItemTotal, &TaxAffect, lMealTaxable, fsLimitable, fsBaked);
	ItemSalesClosePpiPM();
    ItemTotalSESoftTI(&ItemTotal, &lHourly, &sItemCounter);
    ItemTotalSEStoRecGCFSave(&ItemTotal);
    ItemTotalSERestoreTI(&lHourly, &sItemCounter);

    /*----- Tax(Print) Data -----*/
    ItemTotalSESoftPrintTax(&ItemTotal, &TaxAffect);

    /*----- Hourly, Service Total Data -----*/
    ItemTotalSEPostAffect(UifTotal, &ItemTotal, &TaxAffect);

    /*----- Service Total Data -----*/
    ItemTotalSEStoRecItem(UifTotal, &ItemTotal);

    if (ITM_TTL_FLAG_DRAWER(&ItemTotal)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
	
    /*----- check pre-auth. system -----*/
    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
		SetReceiptPrint(SET_RECEIPT_FIRST_COPY);
	}

    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal) && (CliParaMDCCheckField(MDC_PREAUTH_PADR, MDC_PARAM_BIT_C))) {
		pWorkCur->fsCurStatus |= CURQUAL_PRINT_EPT_LOGO;
		ItemTrailer(TYPE_EPT2);
	} else {
		ItemTrailer(TYPE_SOFTCHECK);
	}

    ItemTotalSEOpenCheck();

	//Credit card floor limit for service totals
	if ( ((UifTotal->uchMinorClass == HALO_TEND3_ADR) && CliParaMDCCheckField(MDC_CPPARA2_ADR, MDC_PARAM_BIT_A)) || !CliParaMDCCheckField(MDC_CPPARA2_ADR, MDC_PARAM_BIT_A) )
	{
		PARAMISCPARA	ParaMiscPara = {0};

		//Read the Parameter from AC128
		ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
		ParaMiscPara.uchAddress = MISC_CREDITFLOOR_ADR;	// Address for where the floor amount is stored
		ParaMiscParaRead(&ParaMiscPara);
		ulMinTotal = ParaMiscPara.ulMiscPara;
		ParaMiscPara.uchAddress = MISC_CREDITFLOORSTATUS_ADR;	// Address for where the status bits are stored
		ParaMiscParaRead(&ParaMiscPara);

		pWorkCur->fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;   // clear the indicator and set it back below if still necessary
		/* transaction close */
		if ((ParaMiscPara.ulMiscPara & A128_CC_ENABLE) && (RflLLabs(ItemTotal.lMI) <= (LONG)ulMinTotal))
		{
			if (CliParaMDCCheckField (MDC_RCT_PRT_ADR, MDC_PARAM_BIT_C) == 0) {
				pWorkCur->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;
			}

			if ((ParaMiscPara.ulMiscPara & A128_CC_CUST))	//Do not print customer receipt   ***PDINU
			{
				pWorkCur->fbNoPrint |= CURQUAL_NO_R;
				ItemTotalSEClose(&ItemTotal);
				pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;
			}
			else
			{
				ItemTotalSEClose(&ItemTotal);
			}
		}
		else
		{
			ItemTotalSEClose(&ItemTotal);
		}
	}
	else
	{
		ItemTotalSEClose(&ItemTotal);
	}

    if ((ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_SERVICE2) || (ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_CASINT2)) { /* R3.3 */
        ItemTotalStub(&ItemTotal);
    }

    if (ITM_TTL_FLAG_PRE_AUTH(&ItemTotal)) {
		SetReceiptPrint(SET_RECEIPT_SECOND_COPY);
		
		if(!CliParaMDCCheckField(MDC_RCT_PRT_ADR, MDC_PARAM_BIT_D)){
			pWorkCur->fsCurStatus &= ~CURQUAL_MERCH_DUP;
		}
		else{
			pWorkCur->fsCurStatus &= ~CURQUAL_CUST_DUP;
		}

		ItemTendDupRec();										    /* Print the customer receipt */
    }

    ItemMiscResetStatus(MISC_PARKING);

	{
		ULONG     fsCurStatus = (pWorkCur->fsCurStatus & CURQUAL_WIC);

		ItemTotalInit();
		ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */
	}
    return(ItemFinalize());
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEStoRecItem(UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate store total data
==========================================================================*/

VOID    ItemTotalSEStoRecItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
    SHORT       sFlag = 0;

    /*----- Service Total Data -----*/
    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    if (ItemTotalSysType(ItemTotal) == 7) {
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass) + (UCHAR)CLASS_SOFTCHK_BASE_TTL;
        ItemTotal->fbStorageStatus = 0;
    } else {
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
        ItemTotal->fbStorageStatus = NOT_CONSOLI_STORAGE;
    }

    memcpy(ItemTotal->aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal->aszNumber));

    ItemTotal->usConsNo = TranCurQualPtr->usConsNo;              /* consecutive No */
    ItemTotal->uchLineNo = TranGCFQualPtr->uchSlipLine;          /* current slip station line No */

    if (TranModeQualPtr->ulWaiterID) {
        ItemTotal->ulID = TranModeQualPtr->ulWaiterID;
    } else {
        ItemTotal->ulID = TranModeQualPtr->ulCashierID;
    }

    ItemTotalSEGetGC(ItemTotal);

    if (ITM_TTL_FLAG_PRINT_VALIDATION(ItemTotal)) {
        ItemTotal->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTotal);
        ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        sFlag = 1;
    }
    if ( CliParaMDCCheckField(MDC_VALIDATION_ADR, MDC_PARAM_BIT_B) ) { /* validation slip print */
    	if ( CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_D) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    /*----- Make Display on line display, R3.3 -----*/
    ItemTotalSEDispCom(UifRegTotal, ItemTotal->lMI);

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, ItemTotal->lMI);

    if (ItemTotalSysType(ItemTotal) == 7) {                     /* S/R, Buffer Print */
        ItemTotal->uchMinorClass = ItemTotalSESetClass(UifRegTotal->uchMinorClass) + (UCHAR)CLASS_SOFTCHK_BASE_TTL;
    }

    TrnItem(ItemTotal);

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);

    if (ItemTotalSysType(ItemTotal) == 7) {                     /* S/R, Buffer Print */
        ItemTotal->uchMinorClass -= (UCHAR)CLASS_SOFTCHK_BASE_TTL;
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEStoRecGCFSave(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:   In a Store Recall System, save current guest check to the proper
*                  guest check queue depending on the terminal type to decide whether
*                  to put onto a drive thru queue or the counter terminal storage.
==========================================================================*/

VOID    ItemTotalSEStoRecGCFSave(ITEMTOTAL *ItemTotal)
{
    SHORT             sStatus, sType = GCF_COUNTER_TYPE;
    TRANGCFQUAL       * const WorkGCF = TrnGetGCFQualPtr();
	FDTPARA           WorkFDT = { 0 };

	FDTParameter(&WorkFDT);
    if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY)
	{
        sType = GCF_COUNTER_TYPE;
        WorkGCF->fsGCFStatus &= ~GCFQUAL_DRIVE_THROUGH;
    } else {                                                    /* drive through type */
		// The system type and the terminal type are set using AC162.
		//     WorkFDT.uchSysTypeTerm = 0 -> Ordering/Payment System (counter or single drive thru)
		//     WorkFDT.uchSysTypeTerm = 1 -> Ordering & Payment System (two terminal drive thru)
		//                                   terminal 1 is ordering, terminal 2 is payment
		//     WorkFDT.uchSysTypeTerm = 2 -> Ordering/Payment & Delivery System (two terminal drive thru)
		//                                   terminal 1 is ordering/payment, terminal 2 is delivery
		if (WorkFDT.uchSysTypeTerm == FX_DRIVE_SYSTYPE_1T_OP          /* Order/Payment System */
            || WorkFDT.uchSysTypeTerm == FX_DRIVE_SYSTYPE_2T_OP
			|| WorkFDT.uchSysTypeTerm == FX_DRIVE_SYSTYPE_2T_OP_D){    /* Order and Payment System */
            if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1    /* Order Termninal (Queue#1) */
                || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
                || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
                || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1) {
				if (ITM_TTL_FLAG_DT_PUSH_BACK(ItemTotal) && (WorkGCF->blRecalled == TRUE)){
					sType = GCF_DRIVE_PUSH_THROUGH_STORE;
				}
				else{
					sType = GCF_DRIVE_THROUGH_STORE;
				}
            } else {
				if (ITM_TTL_FLAG_DT_PUSH_BACK(ItemTotal) &&	(WorkGCF->blRecalled == TRUE)){
					sType = GCF_DRIVE_PUSH_THROUGH2;
				}
				else {
					sType = GCF_DRIVE_THROUGH2;
				}
            }
        } else {                                                /* Order/Payment and Delivery */
            if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1) {
				if (ITM_TTL_FLAG_DT_PUSH_BACK(ItemTotal) &&	(WorkGCF->blRecalled == TRUE)){
					sType = GCF_DRIVE_PUSH_DELIVERY1;
				}
				else {
					sType = GCF_DRIVE_DELIVERY1;
				}
            } else if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2) {
				if (ITM_TTL_FLAG_DT_PUSH_BACK(ItemTotal) &&	(WorkGCF->blRecalled == TRUE)){
					sType = GCF_DRIVE_PUSH_DELIVERY2;
				}
				else {
					sType = GCF_DRIVE_DELIVERY2;
				}
            }
			else {
				PifLog (MODULE_UI, LOG_EVENT_UI_FDT_TERMTYPE_ERR);
			}
        }
		WorkGCF->blRecalled = TRUE;
        WorkGCF->fsGCFStatus |= GCFQUAL_DRIVE_THROUGH;          /* drive through type GCF */
    }

    if (!((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER)) {
        if ( WorkGCF->usNoPerson == 0 ) {                       /* if 1'st store operation */
            WorkGCF->usNoPerson = 1;
        }

//		{
//			DATE_TIME   WorkDate;
//			PifGetDateTime( &WorkDate );                            /* get TOD */
//			WorkGCF->auchCheckOpen[0] = ( UCHAR )WorkDate.usHour;   /* hour */
//			WorkGCF->auchCheckOpen[1] = ( UCHAR )WorkDate.usMin;    /* minute */
//			WorkGCF->auchCheckOpen[2] = ( UCHAR )WorkDate.usSec;    /* Second, R3.1 */
//		}
    }

    FDTStore();                                                 /* R3.0 */

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {                    /* Saratoga */
        WorkGCF->fsGCFStatus |= GCFQUAL_WIC;
    }

    while ((sStatus = TrnSaveGC(sType, WorkGCF->usGuestNo)) !=  TRN_SUCCESS) {
		USHORT    usStatus = GusConvertError( sStatus );
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }

    if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE)
	{
        if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE) {
            if (ITM_TTL_FLAG_DISPLAY_LCD3(ItemTotal)) {
				FDTWait(TrnInformationPtr->TranGCFQual.usGuestNo, 1);
            }
        }
        FDTSaveTrans(TrnInformationPtr->TranGCFQual.usGuestNo);
    } else if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
        FDTSaveTransStorePay(TrnInformationPtr->TranGCFQual.usGuestNo);
    }
}

/*=============== Common Functions of Service Total ===============*/

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSETaxRef(UIFREGTOTAL *UifRegTotal,
*                   ITEMTOTAL *ItemTotal, ITEMAFFECTION *Affect, ITEMCOMMONTAX *WorkTax)
*
*   Input:
*
*   Output:
*   InOut:      none
*   Return:
*
*   Description:    calculate tax for reference
=========================================================================*/

SHORT   ItemTotalSETaxRef(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal,
            ITEMAFFECTION *Affect, ITEMCOMMONTAX *WorkTax)
{
    SHORT           sStatus;
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();
    UCHAR           const uchTypeIdx = ItemTotalTypeIndex (UifRegTotal->uchMinorClass);  /* set total key status, R3.3 */

    if (UifRegTotal->uchMinorClass == CLASS_UICASINT) {
		// cashier interrupt start so change the Total Key type to perform Cashier Interrrupt.
		if (ITM_TTL_GET_TTLTYPE(uchTypeIdx) == PRT_SERVICE2) {   /* service with stub */
            ItemTotal->auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_CASINT2);  /* cashier interrupt with stub */
        } else {
            ItemTotal->auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_CASINT1);  /* cashier interrupt without stub */
        }
    } else if (UifRegTotal->uchMinorClass == CLASS_UIKEYOUT) {  /* V3.3 */
        if (ITM_TTL_GET_TTLTYPE(uchTypeIdx) == PRT_SERVICE2) {   /* service with stub */
			ItemTotal->auchTotalStatus[0] = ITM_TTL_GET_TTLTYPE(uchTypeIdx);
        } else {
            ItemTotal->auchTotalStatus[0] = ITM_TTL_CALC_TTLKEYTYPE(uchTypeIdx, PRT_SERVICE1);    /* set service total */
        }
    } else {
        ItemTotal->auchTotalStatus[0] = uchTypeIdx;
    }

	ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, ItemTotalSESetClass(UifRegTotal->uchMinorClass));

/*** V3.3 ICI
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        /----- Not Use "Not Consoli Print" Option -----/
        ItemTotal->auchTotalStatus[4] &= ~0x20;
    }
****/

    /*----- Calculate Current Tax -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {                  /* V3.3 */
        ItemCanTax(&ItemTotal->auchTotalStatus[1], WorkTax, ITM_NOT_SPLIT);

        /*----- All Tax from Newcheck to Now -----*/
        ItemTotal->lTax += WorkTax->alTax[0]
            + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[0]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[1]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[2]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[3];

    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
        if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
            return(sStatus);
        }
        /*----- All Tax from Newcheck to Now -----*/
        ItemTotal->lTax
            += WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2]
            + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[0]
            + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[1]
            + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[2];
    } else {
		ITEMCOMMONVAT   WorkVAT = {0};

		ItemCurVAT(&ItemTotal->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);

        memcpy(&Affect->USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT)*NUM_VAT);
        Affect->lAmount += WorkVAT.lService;
        ItemTotal->lTax = WorkVAT.lPayment;     /* save exclude vat/service */

    }

    /*----- Affection Tax Data -----*/
    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {                    /* V3.3 */
        ItemCanTaxCopy1(Affect, WorkTax);
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftAffectTax(ITEMTOTAL *Total,
*                                   ITEMAFFECTION *Affect, DCURRENCY *lTaxable)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Make Affection Tax Data.
==========================================================================*/

VOID    ItemTotalSESoftAffectTax(ITEMTOTAL *Total,
                        ITEMAFFECTION *Affect, DCURRENCY *lTaxable, USHORT fsLimitable, USHORT fsBaked)
{
    UCHAR           i, j;
    DCURRENCY       lTax = 0L;
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();

    /*----- Affection Tax -----*/
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_TAXAFFECT;

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {                  /* V3.3 */
        if (Total->auchTotalStatus[1] != 0) {     // if any Canadian tax calculations, GST, PST1, PST2, PST3
            for(i = 0; i < 5; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            }
            for(i = 0; i < 4; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
                WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
            }
            WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0] -= ItemTotalLocal.lPigTaxExempt;   /* 10-21-98 */
            Affect->fbStorageStatus = NOT_CONSOLI_STORAGE;
            TrnItem(Affect);
        }
		// if any Canadian tax calculations (GST, PST1, PST2, PST3) and ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER() is off
        if (Total->auchTotalStatus[1] != 0 || !(Total->auchTotalStatus[2] & EVEN_MDC_BIT0)) {
            WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable += WorkTI->Itemizers.TranCanadaItemizers.lTaxable[8];   /* 10-21-98 */
            for(i = 0; i <  STD_PLU_ITEMIZERS; i++) {
                WorkTI->Itemizers.TranCanadaItemizers.lTaxable[i] = 0L;
            }
            for(i = 0; i < STD_PLU_ITEMIZERS+2+3; i++) {    // was 9+2+3
                WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[i] = 0L;
            }
            WorkTI->Itemizers.TranCanadaItemizers.lMealLimitTaxable += lTaxable[0];
            WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable += lTaxable[1];

		    /* check limitable status - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
			if (fsLimitable == TRUE) {
				for (j = 0; j < 6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][1] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			} else {
				for (j = 0; j < 6; j++) {
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][2] += WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9];
					WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][9] = 0;
				}
			}

		    /* check pig rule counter */
			if (fsBaked == FALSE) {
				for (j = 0; j < 6; j++) {
					for(i = STD_PLU_ITEMIZERS+2; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
						WorkTI->Itemizers.TranCanadaItemizers.lVoidDiscTax[j][i] = 0L;
					}
    	        }
			}

        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0                 // ITM_TTL_FLAG_TAX_1()
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0             // ITM_TTL_FLAG_TAX_2()
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0) {           // ITM_TTL_FLAG_TAX_3()
            Affect->fbStorageStatus = NOT_CONSOLI_STORAGE;
            TrnItem(Affect);
        }
        for(i = 0; i < 3; i++) {
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i] += Affect->USCanVAT.USCanTax.lTaxable[i];
            WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i] += Affect->USCanVAT.USCanTax.lTaxAmount[i];
            WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i] += Affect->USCanVAT.USCanTax.lTaxExempt[i];
        }
        if (Total->auchTotalStatus[1] & ODD_MDC_BIT0                 // ITM_TTL_FLAG_TAX_1()
            || Total->auchTotalStatus[1] & EVEN_MDC_BIT0             // ITM_TTL_FLAG_TAX_2()
            || Total->auchTotalStatus[2] & ODD_MDC_BIT0              // ITM_TTL_FLAG_TAX_3()
            || (Total->auchTotalStatus[2] & EVEN_MDC_BIT0) == 0) {   // ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER()
            for(i = 0; i < 3; i++) {
                WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i] = 0L;
            }
        }

    } else {
        /* not affect VAT at soft check */
    }

    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {                    /* V3.3 */
        lTax = Affect->USCanVAT.USCanTax.lTaxAmount[0] + Affect->USCanVAT.USCanTax.lTaxAmount[1]
            + Affect->USCanVAT.USCanTax.lTaxAmount[2] + Affect->USCanVAT.USCanTax.lTaxAmount[3];
        WorkTI->lMI += lTax;

        /*----- For Hourly -----*/
        if (!CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_D)) {
            WorkTI->lHourly += lTax;
        }
    }

	// remember our total as our current balance for the guest check.
    WorkGCF->lCurBalance = WorkTI->lMI;
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftPrintTax(ITEMTOTAL *Total, ITEMAFFECTION *Affect)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Make Print Tax Data.
==========================================================================*/

VOID    ItemTotalSESoftPrintTax(ITEMTOTAL *Total, ITEMAFFECTION *Affect)
{
    UCHAR           i;
    DCURRENCY       lWholeTax = 0L;         /* 4/24/96 */
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        Affect->fbStorageStatus = NOT_ITEM_STORAGE;
        ItemTendVATCommon(Affect, ITM_NOT_SPLIT, ITM_DISPLAY);
        return;
    }

    /*----- Print Tax -----*/
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_TAXPRINT;

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {                  /* V3.3 */
        if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT) {
            Affect->USCanVAT.USCanTax.lTaxExempt[0] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
        } else {
            Affect->USCanVAT.USCanTax.lTaxable[0] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
            Affect->USCanVAT.USCanTax.lTaxAmount[0] = WorkTI->Itemizers.TranCanadaItemizers.lAffTax[0];
        }
        if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT) {
            for (i = 0; i < 3; i++) {
                Affect->USCanVAT.USCanTax.lTaxExempt[i+1] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
            }
        } else {
            for (i = 0; i < 3; i++) {
                Affect->USCanVAT.USCanTax.lTaxable[i+1] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
                Affect->USCanVAT.USCanTax.lTaxAmount[i+1] = WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i+1];
            }
        }
        Affect->USCanVAT.USCanTax.lTaxable[4] = WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[4];

        /*----- adjust tax print format 4/17/96 -----*/
        ItemCanTaxCopy2(Affect);

        /*----- Make Subtotal,   R3.1 4/24/96 -----*/
        for( i = 0; i < 4; i++ ) {
            lWholeTax += Affect->USCanVAT.USCanTax.lTaxAmount[i];
        }
        if (CliParaMDCCheckField(MDC_TIPPO2_ADR, MDC_PARAM_BIT_B)) {
            Affect->lAmount = WorkTI->lMI - lWholeTax;
        }
        Affect->fbStorageStatus &= ~(NOT_CONSOLI_STORAGE | NOT_ITEM_STORAGE);
        Affect->fsPrintStatus = PRT_SLIP;
        if (ItemTotalSysType(Total) == 4 || ItemTotalSysType(Total) == 7) {
            TrnItem(Affect);
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
        for(i = 0; i < 3; i++) {
            Affect->USCanVAT.USCanTax.lTaxable[i] = WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
            Affect->USCanVAT.USCanTax.lTaxAmount[i] = WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
            Affect->USCanVAT.USCanTax.lTaxExempt[i] = WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i];
        }

        /*----- Make Subtotal,   R3.1 4/24/96-----*/
        for( i = 0; i < 3; i++ ) {
            lWholeTax += Affect->USCanVAT.USCanTax.lTaxAmount[i];
        }
        if (CliParaMDCCheckField(MDC_TIPPO2_ADR, MDC_PARAM_BIT_B)) {
            Affect->lAmount = WorkTI->lMI - lWholeTax;
        }
        Affect->fbStorageStatus = NOT_ITEM_STORAGE;
        Affect->fsPrintStatus = PRT_SLIP;
        if (ItemTotalSysType(Total) == 4 || ItemTotalSysType(Total) == 7) {
            TrnItem(Affect);
        }
    }

    /*----- Display Tax to LCD,  R3.0 -----*/
    MldScrollWrite(Affect, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(Affect);

    /* send to enhanced kds, 2172 */
    ItemSendKds(Affect, 0);

}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEAmount(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:     none
*
*   Description:    calculate service total amount
==========================================================================*/

VOID    ItemTotalSEAmount(ITEMTOTAL *ItemTotal)
{
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();

    ItemTotal->lMI += WorkTI->lMI;
    ItemTotal->lService += ItemTotal->lMI - WorkTI->lService[0] - WorkTI->lService[1] - WorkTI->lService[2];
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSEPostAffect(UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal, ITEMAFFECTION *Affect)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal, ITEMAFFECTION *Affect
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate print tax, hourly, service total data
==========================================================================*/

VOID    ItemTotalSEPostAffect(UIFREGTOTAL *UifRegTotal,
            ITEMTOTAL *ItemTotal, ITEMAFFECTION *Affect)
{
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();
    SHORT           i;

    /*----- Make Hourly Data -----*/
    memset(Affect, 0, sizeof(ITEMAFFECTION));
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_HOURLY;
    Affect->lAmount = WorkTI->lHourly;
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

    /*----- Make Service Total Affection Data -----*/
    memset(Affect, 0, sizeof(ITEMAFFECTION));
    Affect->uchMajorClass = CLASS_ITEMAFFECTION;
    Affect->uchMinorClass = CLASS_SERVICE;
    Affect->lAmount = ItemTotal->lService;
    Affect->uchAddNum = ItemTotalSEGetSerID(ItemTotal);
    Affect->uchTotalID = ItemTotalSESetClass(UifRegTotal->uchMinorClass);
    TrnItem(Affect);

    WorkTI->lService[Affect->uchAddNum] += Affect->lAmount;
    WorkTI->lHourly = 0L;
    WorkTI->sItemCounter = 0;
	//We now clear the hourly bonus totals, because we have already affected them to the Hourly Totals,
	//by doing this, we no longer run into the problem when the user opens up a check, rings up items, then
	//closes the check, and when they tender it out, it updates the bonuses incorrectly.
	// SR278
	memset(&WorkTI->lHourlyBonus, 0x00, sizeof(WorkTI->lHourlyBonus));
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEClose( ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate transaction close data
==========================================================================*/

VOID    ItemTotalSEClose( ITEMTOTAL *ItemTotal )
{
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();
    SHORT           sStatus;
	ITEMTRANSCLOSE  ItemTransClose = {0};

    switch (ItemTotalSysType(ItemTotal)) {
    case    2:                                                      /* Postcheck/Buffer Print */
    case    4:                                                      /* Softcheck/Buffer Print */
    case    7:                                                      /* S/R, Buffer Print */
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;                /* print consoli storage */
        break;

    case    3:                                                      /* Post, Not Print */
    case    5:                                                      /* Soft, Not Print */
    case    6:                                                      /* S/R, Not Print */
    case    8:                                                      /* S/R, Soft, Not Print */
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;
        break;

    case    1:                                                      /* Precheck/Unbuffer */
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;
        break;

    default:                                                        /* Precheck/Buffer */
        pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;
        break;
    }

    if (CliParaMDCCheckField(MDC_DEPT2_ADR, MDC_PARAM_BIT_A)) {            /* Not Print Void Item, R3.1 */
        pWorkCur->uchKitchenStorage = PRT_KIT_NET_STORAGE;
    } else {
        pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;                       /* for kitchen print */
    }
    if ( ( CliParaMDCCheckField( MDC_PSELVD_ADR, MDC_PARAM_BIT_C ) == 0    /* kitchen send at pvoid */
        && ( pWorkCur->fsCurStatus & CURQUAL_PRESELECT_MASK ) )                /* preselect void */
        || ( pWorkCur->fsCurStatus & CURQUAL_TRAINING ) ) {           /* training */
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;                  /* for kitchen print */
    }

    if ( pWorkCur->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP ) {    /* compulsory slip */
        pWorkCur->fbCompPrint = PRT_SLIP;
    }
/* 07/06/98
    if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
        pWorkCur->fsCurStatus &= ~CURQUAL_ADDCHECK;
        pWorkCur->fsCurStatus |= (CURQUAL_REORDER | CURQUAL_ADDCHECK_SERVT);
    }
*/

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;            /* major class */
    ItemTransClose.uchMinorClass = CLASS_CLSTOTAL1;                 /* minor class */
    TrnClose( &ItemTransClose );                                    /* transaction module i/F */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);

    while ( ( sStatus = TrnSendTotal() ) !=  TRN_SUCCESS ) {
		USHORT   usStatus = TtlConvertError( sStatus );
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEOpenCheck( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate open check affection data (store/recall)   R3.0
==========================================================================*/

VOID    ItemTotalSEOpenCheck( VOID )
{
	ITEMAFFECTION   WorkAffection = {0};

    if (!((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER)
        && !((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK)) {

        WorkAffection.uchMajorClass = CLASS_ITEMAFFECTION;
        WorkAffection.uchMinorClass = CLASS_CASHIEROPENCHECK;
        TrnItem(&WorkAffection);
    }
}

/*==========================================================================
**  Synopsis:   UCHAR   ItemTotalSEGetSerID( ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:     Service ID
*
*   Description:    Get service ID
==========================================================================*/

UCHAR   ItemTotalSEGetSerID( ITEMTOTAL *ItemTotal )
{
    UCHAR        i;
    TRANGCFQUAL  * const WorkGCF = TrnGetGCFQualPtr();

    if ( ITM_TTL_FLAG_ITEMIZER_SERVICE3(ItemTotal) ) {
        i = 2;                                                  /* service itemizer 3 */
        WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE3;
    } else if ( ITM_TTL_FLAG_ITEMIZER_SERVICE2(ItemTotal) ) {
        i = 1;                                                  /* service itemizer 2 */
        WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE2;
    } else {
        i = 0;                                                  /* service itemizer 1 */
        WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE1;
    }

    return(i);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalDupRec(ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    print duplicate receipt for Pre-Autho.
==========================================================================*/
VOID    ItemTotalDupRec(ITEMTOTAL *ItemTotal)
{
    if (CliParaMDCCheckField(MDC_PREAUTH_PADR, MDC_PARAM_BIT_D)) {
        if (CliParaMDCCheckField(MDC_PREAUTH_PADR, MDC_PARAM_BIT_B) == 0) {
			TRANCURQUAL * const pWorkCur = TrnGetCurQualPtr();
			ITEMOTHER   WorkOther = {0};
                                                        /* Full Print option*/
            WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
            if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER ) {
                WorkOther.uchMinorClass = CLASS_POSTRECPREBUF;
            } else {
                /* 6/3/96 WorkOther.uchMinorClass = CLASS_POSTRECEIPT; */
                WorkOther.uchMinorClass = CLASS_CP_EPT_FULLDUP;     /* 6/3/96 */
            }

            pWorkCur->uchKitchenStorage = PRT_NOSTORAGE; /* Q-004 corr. 5/17 ,  kitchen not print*/
            WorkOther.fsPrintStatus = (PRT_RECEIPT | PRT_SINGLE_RECPT);          /* only receipt     */
            TrnItem( &WorkOther );                      /* print trans.     */
        } else {
			ITEMPRINT   WorkPrint = {0};

            ItemHeader( TYPE_STB_TRAIL );               /* print header     */
            WorkPrint.uchMajorClass = CLASS_ITEMPRINT;

			// Print Acct# and date
			if (ITM_TTL_FLAG_PRE_AUTH(ItemTotal) && (CliParaMDCCheckField(MDC_PREAUTH_MASK_ADR, MDC_PARAM_BIT_D))) {
				WorkPrint.uchMinorClass = CLASS_EPT_STUB_NOACCT;
			} else {
	            WorkPrint.uchMinorClass = CLASS_EPT_STUB;
			}

            WorkPrint.lAmount = ItemTotal->lMI;             /* set total amount */
            if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {    /* pre-void ?   */
                WorkPrint.lAmount *= -1L;
            }
            WorkPrint.fbModifier = ItemTotal->fbModifier;   /* set modifier flg */
            memcpy(&WorkPrint.aszNumber[0], &ItemTotal->aszNumber[0], sizeof(WorkPrint.aszNumber[0]));/* set Acct number */
            _tcsncpy(&WorkPrint.auchExpiraDate[0], &ItemTotal->auchExpiraDate[0], NUM_EXPIRA);/* set Exe. date */
            memcpy(&WorkPrint.aszCPText[0][0], &ItemTotal->aszCPMsgText[0][0], sizeof(ItemTotal->aszCPMsgText));        /* set Response text msg. */
            WorkPrint.fsPrintStatus = (PRT_RECEIPT | PRT_SINGLE_RECPT);
            TrnThrough( &WorkPrint );                   /* print total line */
        }

		//SR 659, moved this so that it is called in both cases,
		//so that we print the trailer, previously it was only in the "else"
		//case, so the "if" statement never printed a trailer or cut the paper JHHJ 8-25-05
		ItemTrailer( TYPE_EPT2 );
#if 0
		if (CliParaMDCCheck(MDC_PREAUTH_PADR,EVEN_MDC_BIT1)) {
			ItemTrailer(TYPE_SPLITA_EPT);
		} else {
			ItemTrailer(TYPE_SPLITA);
		}
#endif
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEGetGC( ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:
*
*   Description:  For a Service Total, print order No, guest check number on a stub or
*                 chit for Post Guest Check or Store Recall System if provisioning
*                 indicates to do so.
*                  - MDC 234 Bit B "Issue chit receipt at postcheck and store/recall."
*                  - total key does not have Executed Buffering Print enabled
*                  - transaction mnemonic 86 in P20 is provisioned.
==========================================================================*/

VOID    ItemTotalSEGetGC( ITEMTOTAL *ItemTotal )
{
	ITEMTOTAL       WorkTotal = {0};
    TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();

    if (TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) {
        /* not print chit at cashier interrupt */
        return;
    }

    if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
        WorkTotal.usOrderNo = WorkGCF->usGuestNo;
        WorkTotal.uchCdv = WorkGCF->uchCdv;                      /* CDV */

        if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {
            WorkTotal.usOrderNo = 0;                            /* not print GCF# receipt */
        }
    } else {                                                    /* store/recall */
        if (WorkGCF->ulCashierID == 0) {                        /* First Store */
            if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) {          /* preselect void */
                WorkGCF->fsGCFStatus |= GCFQUAL_PVOID;
            }
            if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {  /* transaction return */
                WorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;
            }
            if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAINING) {       /* training */
                WorkGCF->fsGCFStatus |= GCFQUAL_TRAINING;
            }
            WorkGCF->ulCashierID = TranModeQualPtr->ulCashierID;        /* cashier ID of GCF */
        }
        WorkGCF->usGuestNo = ItemTotal->usOrderNo;

        WorkTotal.usOrderNo = ItemTotal->usOrderNo;             /* order No */
        if (ItemTotalSysType(ItemTotal) == 7) {                 /* S/R, Buffer Print */
            WorkTotal.usOrderNo = 0;                            /* not print GCF# receipt */
        }
    }

	// Determine if we are doing a stub or chit print for this order and if so then issue
	// the print with the order number. We need to translate the total key minor class
	// to the appropriate Order Total Key. Total Key #1 and #2 are both special purpose
	// so the Order Total keys start with CLASS_ORDER3 just as the Stub Total keys
	// start with CLASS_STUB3.
    if ( (WorkTotal.usOrderNo != 0                               /* not print at manual PB/addcheck */
         && CliParaMDCCheckField( MDC_TRANNUM_ADR, MDC_PARAM_BIT_B ) == 0 )) { /* R3.0 */

		WorkTotal.uchMajorClass = ItemTotal->uchMajorClass;
		WorkTotal.uchMinorClass = (UCHAR)(ItemTotal->uchMinorClass + CLASS_ORDER_OFFSET);
		WorkTotal.fsPrintStatus = PRT_RECEIPT | PRT_SINGLE_RECPT;           /* issue single receipt */

        ItemHeader( TYPE_STB_TRAIL );                           /* header print */
        TrnThrough( &WorkTotal );                               /* stub print */
        ItemTrailer( TYPE_STB_TRAIL );                          /* trailer print */
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSESoftGTtlSpl( ITEMTOTAL *pWholeTotal,
*                                               ITEMAFFECTION *pWholeTax )
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    generate grand tax/total data
==========================================================================*/

VOID    ItemTotalSESoftGTtlSpl( ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax )
{
    ITEMTOTAL       IndTotal = *pWholeTotal;

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
		ITEMAFFECTION   IndTax = *pWholeTax;

        /* set MI to cancel for sub total print */
        IndTax.lCancel = TranItemizersPtr->lMI;
        IndTax.fbStorageStatus = NOT_ITEM_STORAGE;
        ItemTendVATCommon(&IndTax, ITM_SPLIT, ITM_NOT_DISPLAY);
    } else {
		ITEMAFFECTION   IndTax = *pWholeTax;

        /*------ Make Subtotal 5/8/96-----*/
        if (CliParaMDCCheckField(MDC_TIPPO2_ADR, MDC_PARAM_BIT_B)) {
            IndTax.lAmount = TranItemizersPtr->lMI;
        }

        /*----- Make Tax Data -----*/
        IndTax.uchMajorClass = CLASS_ITEMAFFECTION;
        IndTax.uchMinorClass = CLASS_TAXPRINT;
        IndTax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);
    }

    /*----- Make Grand Total Data -----*/
    IndTotal.usConsNo = TranCurQualPtr->usConsNo;
    IndTotal.uchLineNo = TranGCFQualPtr->uchSlipLine;
    IndTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    TrnItemSpl(&IndTotal, TRN_TYPE_SPLITA);
}

/*==========================================================================
**  Synopsis:   VOID   ItemTotalSESoftTI(ITEMTOTAL *ItemTotal,
*                               DCURRENCY *lHourly, SHORT *sItemCounter)
*
*   Input:      ITEMTOTAL *ItemTotal, DCURRENCY *lHourly, SHORT *sItemCounter
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Clear hourly and add service total data
==========================================================================*/

VOID    ItemTotalSESoftTI(ITEMTOTAL *ItemTotal, DCURRENCY *lHourly, SHORT *sItemCounter)
{
    UCHAR   uchAddNum;
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();         /* get transaction itemizers */

    uchAddNum = ItemTotalSEGetSerID( ItemTotal );

    WorkTI->lService[uchAddNum] += ItemTotal->lService;             /* update current service total */

    *lHourly = WorkTI->lHourly;                                     /* save hourly itemizer */
    *sItemCounter = WorkTI->sItemCounter;                           /* save item counter */

    WorkTI->lHourly = 0L;                                           /* clear hourly itemizer */
    WorkTI->sItemCounter = 0;                                       /* clear item counter */
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalStubSpl( ITEMTOTAL *ItemTotal, UCHAR uchSeatNo )
*
*   Input:      ITEMTOTAL   *ItemTotal
*               UCHAR       uchSeatNo
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    print stub receipt
==========================================================================*/

VOID    ItemTotalStubSpl( ITEMTOTAL *ItemTotal, UCHAR uchSeatNo )
{
	ITEMTOTAL       WorkTotal = {0};
    ITEMPRINT       WorkPrint = {0};
    ITEMMODIFIER    WorkItemMod = {0};

    /* -- Make Total Structure -- */
    WorkTotal.uchMajorClass = ItemTotal->uchMajorClass;
    WorkTotal.uchMinorClass = ( UCHAR )( ItemTotal->uchMinorClass + CLASS_STUB_OFFSET );   /* stub */

    WorkTotal.fsPrintStatus = PRT_RECEIPT;                          /* receipt */

    WorkTotal.lMI = ItemTotal->lMI;

    memcpy( &WorkTotal.auchTotalStatus[0], &ItemTotal->auchTotalStatus[0], sizeof( WorkTotal.auchTotalStatus ) );

    /* -- Make Modifier Structure -- */
    WorkItemMod.uchMajorClass = CLASS_ITEMMODIFIER;
    WorkItemMod.uchMinorClass = CLASS_SEATNO;
    WorkItemMod.fsPrintStatus = (PRT_RECEIPT|PRT_SINGLE_RECPT);
    WorkItemMod.uchLineNo = uchSeatNo;

    /* -- restore print flags -- */
    WorkPrint.uchMajorClass = CLASS_ITEMPRINT;
    WorkPrint.uchMinorClass = CLASS_RESTART;
    TrnThrough( &WorkPrint );

    ItemHeader( TYPE_STB_TRAIL );                               /* header print */
    TrnThrough( &WorkItemMod );                                 /* seat# print */
    TrnThrough( &WorkTotal );                                   /* stub print */
    ItemTrailer( TYPE_STB_TRAIL );                              /* trailer print */
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSEDispCom(UIFREGTOTAL *UifTotal, DCURRENCY lTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Display service total amount to line display
*                   (changed from ItemTotalSESoftCheckSplDisp()), R3.3
==========================================================================*/

VOID    ItemTotalSEDispCom(UIFREGTOTAL *UifTotal, DCURRENCY lTotal)
{
	REGDISPMSG  DispMsg = {0}, DispMsgBack = {0};

    /*----- Display Data -----*/
    DispMsg.uchMajorClass = UifTotal->uchMajorClass;
    DispMsg.uchMinorClass = ItemTotalSESetClass(UifTotal->uchMinorClass);
    DispMsg.lAmount = lTotal;

    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr (ItemTotalSESetClass(UifTotal->uchMinorClass)));

    flDispRegDescrControl |= TOTAL_CNTRL;
    DispMsg.fbSaveControl = 2;

    ItemTendDispIn(&DispMsgBack);

    flDispRegKeepControl &= ~(TVOID_CNTRL | NEWCHK_CNTRL | STORE_CNTRL | PBAL_CNTRL | TAXEXMPT_CNTRL);

    if (CliParaMDCCheckField(MDC_RECEIPT_ADR, MDC_PARAM_BIT_D)) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
    if (CliParaMDCCheckField(MDC_MENU_ADR, MDC_PARAM_BIT_D) != 0) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

        uchDispCurrMenuPage = pWorkSales->uchMenuShift;
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift; /* set default adjective, 2172 */
        }
    }

    flDispRegDescrControl &= ~TVOID_CNTRL;

    DispWriteSpecial(&DispMsg, &DispMsgBack);

    flDispRegDescrControl &= ~(TOTAL_CNTRL | TAXEXMPT_CNTRL);
}

/*==========================================================================
**  Synopsis:   UCHAR    ItemTotalSESetClass(UCHAR uchMinorClass)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Replace special total key class to normal total key class, R3.3
==========================================================================*/

UCHAR    ItemTotalSESetClass(UCHAR uchMinorClass)
{
    if (uchMinorClass == CLASS_UICASINT) {  /* R3.3 */
        return(CLASS_UITOTAL8);
    }

    if (uchMinorClass == CLASS_UIKEYOUT) {  /* V3.3 */
        /* changed from total 8 to total 7 by FVT comment */
        return(CLASS_UITOTAL7);
        /* return(CLASS_UITOTAL8); */
    }

    return (uchMinorClass);
}

#if !defined(POSSIBLE_DEAD_CODE)
//  See function PrtServMaskAcctDate() which seems to be
//  what is used.  Richard Chambers, Feb-24-2009

/*==========================================================================
**  Synopsis:   VOID	ItemTotalMaskAcctDate(ITEMTOTAL *ItemTotal)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    If Pre-Authorization system, check and mask acct and date
==========================================================================*/

VOID	ItemTotalServMaskAcctDate(ITEMTOTAL *ItemTotal)
{
	PARAMDC			MaskMDC;
	int				iShownChars = 0, iMaskedChars = 0, i;

	//Check if Pre-Authorization
	if (ITM_TTL_FLAG_PRE_AUTH(ItemTotal)) {
		//Perform Mask
		if (!CliParaMDCCheckField(MDC_PREAUTH_MASK_ADR, MDC_PARAM_BIT_C)) {
			//How many to mask
			MaskMDC.uchMajorClass = CLASS_PARAMDC;
			MaskMDC.usAddress = MDC_PREAUTH_MASK_ADR;
			ParaRead(&MaskMDC);

			iShownChars = 0;
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}

			iMaskedChars = NUM_NUMBER_EPT - iShownChars;
			iMaskedChars = iMaskedChars ? iMaskedChars : 0;

			for(i=0; i < iMaskedChars; i++) {
				ItemTotal->aszNumber[i] = _T('X');
			}

			for(i=0; i < NUM_EXPIRA; i++) {
				ItemTotal->auchExpiraDate[i] = _T('X');
			}

		}
	}
}
#endif

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSERestoreTI(DCURRENCY *lHourly, SHORT *sItemCounter)
*
*   Input:      DCURRENCY *lHourly, SHORT *sItemCounter
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Clear hourly and add service total data
==========================================================================*/

VOID    ItemTotalSERestoreTI( DCURRENCY *lHourly, SHORT *sItemCounter )
{
    TRANITEMIZERS   * const WorkTI = TrnGetTIPtr();     /* get transaction itemizers */

    WorkTI->lHourly = *lHourly;                                     /* restore hourly itemizer */
    WorkTI->sItemCounter = *sItemCounter;                           /* restore item counter */
}


/****** End of Source ******/
