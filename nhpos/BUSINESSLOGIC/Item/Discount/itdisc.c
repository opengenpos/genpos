/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Discount module main.                          
* Category    : Item Common Module, NCR 2170 US Hsopitality Model Application         
* Program Name: ITDISC.C
* --------------------------------------------------------------------------
* Abstract: ItemDiscount()   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Feb-14-95: 03.00.00  :   hkato   : R3.0
* Nov-09-95: 03.01.00  :   hkato   : R3.1
* Aug-05-98: 03.03.00  :  hrkato   : V3.3
* Aug-08-13: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
*
** GenPOS **
* Oct-13-17: 02.02.02  : R. Chambers  : replace labs() with RflLLabs(), LONG to DCURRENCY, 64 bit totals
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
#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include    <stdlib.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "uie.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "display.h"
#include    "mld.h"
#include    "rfl.h"
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemDiscount(UIFREGDISC *pUifRegDisc)
*
*    Input: UIFREGDISC *pUifRegDisc
*   Output: None
*    InOut: None
**Return: 
*
** Description:                                                     V3.3
*===========================================================================
*/
SHORT   ItemDiscount(UIFREGDISC *pUifRegDisc)
{
    SHORT           sStatus;

	{
	    TRANCURQUAL *pWorkCur = TrnGetCurQualPtr();

		if ((pWorkCur->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) == CURQUAL_TRETURN) {
			pUifRegDisc->fbModifier |= RETURNS_TRANINDIC;
		}
	}
    sStatus = ItemDiscMain(pUifRegDisc);

    if (sStatus == ITM_SEAT_DISC) {
		TRANGCFQUAL     * const pWorkGC = TrnGetGCFQualPtr();
		SHORT           i, j = 0, k = 0;
		UCHAR           uchMaskSeatDisc = GCFQUAL_REG1_SEAT;
		UCHAR           auchSeat[NUM_SEAT] = {0};

        for (i = 0; i < NUM_SEAT; i++) {
            if (pWorkGC->auchUseSeat[i] == 0) {
                break;
            }
            if (TrnSplSearchFinSeatQueue(pWorkGC->auchUseSeat[i]) == TRN_SUCCESS) {
                continue;
            }
            if (!(pWorkGC->auchSeatDisc[pWorkGC->auchUseSeat[i] - 1] & (uchMaskSeatDisc << (pUifRegDisc->uchMinorClass - CLASS_UIREGDISC1)))) {
                auchSeat[j++] = pWorkGC->auchUseSeat[i];
            }
        }
        if (j == 0) {
            return(LDT_PROHBT_ADR);
        }
        for (i = 0; i < j; i++) {
            pUifRegDisc->uchSeatNo = auchSeat[i];
            if (ItemDiscMain(pUifRegDisc) != ITM_SUCCESS) {
                k++;
            }
        }
        pUifRegDisc->uchSeatNo = 'B';           /* Disc for Base Trans. */
        if (ItemDiscMain(pUifRegDisc) != ITM_SUCCESS) {
            k++;
        }
        if (k == j+1) {
            ItemOtherClear();
        }
        sStatus = ITM_SUCCESS;
    }

	{
		ITEMMODLOCAL    * const pWorkMod = ItemModGetLocalPtr();

		pWorkMod->fsTaxable &= MOD_CANADAEXEMPT;
		pWorkMod->fbModifierStatus &= ~MOD_FSMOD;             /* Saratoga */
	}

    return(sStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscMain(UIFREGDISC *pUifRegDisc)
*
*    Input: UIFREGDISC *pUifRegDisc
*   Output: None
*    InOut: None
**Return: 
*
** Description:  
*===========================================================================
*/
SHORT   ItemDiscMain(UIFREGDISC *pUifRegDisc)
{
    SHORT           sStatus;
    DCURRENCY       lItemizerBuff = 0;
    ITEMDISC        ItemDisc = {0};

    /*----- check tax exempt key depression  R3.0 -----*/
    if ((sStatus = ItemCommonCheckExempt()) != ITM_SUCCESS) {
        return(sStatus);
    }
    /*----- check split check mode R3.1 -----*/
    switch(pUifRegDisc->uchMinorClass) {
    case CLASS_UICHARGETIP:
    case CLASS_UICHARGETIP2:    /* V3.3 */
    case CLASS_UICHARGETIP3:    /* V3.3 */
    case CLASS_UIAUTOCHARGETIP: /* V3.3 */
        /* allow charge tips in split check mode */
        break;

    case CLASS_UIITEMDISC1:
		{
			ITEMSALESLOCAL  *pWorkSales;

			pWorkSales = ItemSalesGetLocalPointer();
			if (pWorkSales->fbSalesStatus & SALES_TRANSACTION_DISC)
				return LDT_PROHBT_ADR;
		}
		// fall through to do the default case as well.
    default:
        if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {
            return(sStatus);
        }
        break;
    }

    ItemDisc.uchMajorClass = CLASS_ITEMDISC;
    ItemDisc.uchDiscountNo = pUifRegDisc->uchDiscountNo;  // remember the discount number if this is a CLASS_ITEMDISC1 item discount
	ItemDisc.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;  // set the discount item hourly total block offset for purposes of totals.
    switch(pUifRegDisc->uchMinorClass) {
    case CLASS_UIITEMDISC1:
        ItemDisc.uchMinorClass = CLASS_ITEMDISC1;   // see ItemDisc.uchDiscountNo to determine subtype 1 thru 6
        break;

    case CLASS_UIREGDISC1:
        ItemDisc.uchMinorClass = CLASS_REGDISC1;
        break;

    case CLASS_UIREGDISC2:
        ItemDisc.uchMinorClass = CLASS_REGDISC2;
        break;

    case CLASS_UIREGDISC3:                  /* R3.1 */
        ItemDisc.uchMinorClass = CLASS_REGDISC3;
        break;

    case CLASS_UIREGDISC4:                  /* R3.1 */
        ItemDisc.uchMinorClass = CLASS_REGDISC4;
        break;

    case CLASS_UIREGDISC5:                  /* R3.1 */
        ItemDisc.uchMinorClass = CLASS_REGDISC5;
        break;

    case CLASS_UIREGDISC6:                  /* R3.1 */
        ItemDisc.uchMinorClass = CLASS_REGDISC6;
        break;

    case CLASS_UICHARGETIP:
        ItemDisc.uchMinorClass = CLASS_CHARGETIP;
        break;

    case CLASS_UICHARGETIP2:    /* V3.3 */
        ItemDisc.uchMinorClass = CLASS_CHARGETIP2;
        break;

    case CLASS_UICHARGETIP3:    /* V3.3 */
        ItemDisc.uchMinorClass = CLASS_CHARGETIP3;
        break;

    case CLASS_UIAUTOCHARGETIP: /* V3.3 */
		{
			UCHAR           uchType = 0;

			/* read from mode qualifier */
			/* check surrogate rate */
			if (TranModeQualPtr->ulWaiterID && TranModeQualPtr->uchWaiChgTipRate) {
				uchType = TranModeQualPtr->uchWaiChgTipRate;
			} else if (TranModeQualPtr->ulCashierID && TranModeQualPtr->uchCasChgTipRate) {
				uchType = TranModeQualPtr->uchCasChgTipRate;
			}

			if (uchType == 1) {
				ItemDisc.uchMinorClass = CLASS_AUTOCHARGETIP;
			} else if (uchType == 2) {
				ItemDisc.uchMinorClass = CLASS_AUTOCHARGETIP2;
			} else if (uchType == 3) {
				ItemDisc.uchMinorClass = CLASS_AUTOCHARGETIP3;
			} else {
				/* no auto charge tips rate is set */
//				NHPOS_NONASSERT_TEXT("CLASS_UIAUTOCHARGETIP: no rate set.");
				return (LDT_TIP_REQUEST_ADR);
			}
		}
        break;
    default:
        break;
    }

    if (ItemDisc.uchMinorClass != CLASS_ITEMDISC1) {

        if ((ItemDisc.uchMinorClass == CLASS_REGDISC1) ||
            (ItemDisc.uchMinorClass == CLASS_REGDISC2) ||
            (ItemDisc.uchMinorClass == CLASS_REGDISC3) ||
            (ItemDisc.uchMinorClass == CLASS_REGDISC4) ||
            (ItemDisc.uchMinorClass == CLASS_REGDISC5) ||
            (ItemDisc.uchMinorClass == CLASS_REGDISC6))
		{
			TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
			SHORT           sFlag = 0;

            if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK &&
                !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3) &&
                !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
				ITEMCOMMONLOCAL *pCommon = ItemCommonGetLocalPointer();

				if ((pCommon->ItemSales.uchMajorClass == CLASS_ITEMSALES) ||
                    (pCommon->ItemSales.uchMajorClass == CLASS_ITEMCOUPON) ||
                    (pCommon->ItemSales.uchMajorClass == CLASS_ITEMDISC)) {
					ITEMDISC        DummyDisc = {0};
                    if ((sStatus = ItemPreviousItem(&DummyDisc, 0)) != ITM_SUCCESS) {
                        return(sStatus);
                    }
                }
                sFlag = 1;
            }
            /* --- Disc for Each Seat#, V3.3 --- */
            if (sFlag != 0 && WorkGCF->auchUseSeat[0] != 0) {
                if (TrnSplCheckSplit() == TRN_SPL_SEAT
                    || TrnSplCheckSplit() == TRN_SPL_MULTI
                    || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
                    if ((sStatus = ItemDiscCheckQual(pUifRegDisc)) != ITM_SUCCESS) {
                        return(sStatus);
                    }
                } else {
                    if (pUifRegDisc->uchSeatNo == 0) {
						ITEMTRANSOPEN   Trans = {0};
                        if ((sStatus = ItemTransSeatNo(&Trans, 1)) != ITM_SUCCESS) {
                            return(sStatus);
                        }
                        if (Trans.uchSeatNo >= 1 && Trans.uchSeatNo <= NUM_SEAT) {
                            /* --- Prohibit Not-Used Seat# --- */
                            if (TrnSplSearchUseSeatQueue(Trans.uchSeatNo) != TRN_SUCCESS) {
                                return(LDT_PROHBT_ADR);
                            }
                            /* --- Prohibit Finalized Seat# --- */
                            if (TrnSplSearchFinSeatQueue(Trans.uchSeatNo) == TRN_SUCCESS) {
                                return(LDT_PROHBT_ADR);
                            }
                        }
                        ItemDisc.uchSeatNo = Trans.uchSeatNo;
                    } else {
                        ItemDisc.uchSeatNo = pUifRegDisc->uchSeatNo;
                    }

                    if (ItemDisc.uchSeatNo == '0') {
                        if (pUifRegDisc->lAmount != 0L) {
                            return(LDT_SEQERR_ADR);
                        }
                        return(ITM_SEAT_DISC);
                    } else if (ItemDisc.uchSeatNo == 0) {
                        sStatus = ItemDiscCheckQual(pUifRegDisc);
                    } else {
                        sStatus = ItemDiscCheckQualSpl(pUifRegDisc, &ItemDisc);
                    }
                    if (sStatus != ITM_SUCCESS) {
                        return(sStatus);
                    }
                    /* --- Prohibit Both Normal R/D and Seat R/D,   V3.3 --- */
                    if (!(pUifRegDisc->fbModifier & VOID_MODIFIER)) {
                        if ((sStatus = ItemDiscCheckNormalAndSeat(&ItemDisc)) != ITM_SUCCESS) {
                            return(sStatus);
                        }
                    }
                }
            } else {
                if ((sStatus = ItemDiscCheckQual(pUifRegDisc)) != ITM_SUCCESS) {
                    return(sStatus);
                }
            }
        } else {
			if (ItemSalesLocalPtr->fbSalesStatus & SALES_TRANSACTION_DISC)
				return LDT_PROHBT_ADR;

			if ((sStatus = ItemDiscCheckQual(pUifRegDisc)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    }

	if (pUifRegDisc->fbModifier & RETURNS_TRANINDIC) {
		ItemDisc.fbReduceStatus |= REDUCE_RETURNED;
		ItemDisc.fbDiscModifier |= pUifRegDisc->fbModifier;
	}

    lItemizerBuff = 0L;
	if ((sStatus = ItemDiscCheckSeq(pUifRegDisc, &ItemDisc, &lItemizerBuff)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if ((sStatus = ItemDiscCalculation(&ItemDisc, &lItemizerBuff)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*--- SET NUMBER ---*/
    _tcsncpy(ItemDisc.aszNumber, pUifRegDisc->aszNumber, STD_NUMBER_LEN);

    if ((sStatus = ItemDiscItemDiscModif(&ItemDisc)) != ITM_SUCCESS) {
        return(sStatus);
    }

	if ((sStatus = ItemDiscCommonIF(pUifRegDisc, &ItemDisc)) == ITM_SUCCESS) {
		if (pUifRegDisc->fbModifier & (VOID_MODIFIER | RETURNS_TRANINDIC)) {
			TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();

			switch(pUifRegDisc->uchMinorClass) {
				case CLASS_UICHARGETIP:
				case CLASS_UICHARGETIP2:    /* V3.3 */
				case CLASS_UICHARGETIP3:    /* V3.3 */
					WorkGCF->fsGCFStatus |= GCFQUAL_CHARGETIP;
					break;

				case CLASS_UIAUTOCHARGETIP: /* V3.3 */
					WorkGCF->fsGCFStatus2 |= GCFQUAL_AUTOCHARGETIP;
					break;
				default:
					break;
			}
		}
	}
    return(sStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCursorVoid(ITEMDISC *pData);
*
*    Input: ITEMDISC *pData
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  Cursor Void of Discount.                   R3.1
*===========================================================================
*/
SHORT   ItemDiscCursorVoid(ITEMDISC *pData)
{
    SHORT           sStatus;

    if (pData->uchMinorClass != CLASS_ITEMDISC1) {
        if ((sStatus = ItemDiscCursorVoidCheck(pData)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    if (pData->uchMinorClass == CLASS_REGDISC1
        || pData->uchMinorClass == CLASS_REGDISC2
        || pData->uchMinorClass == CLASS_REGDISC3
        || pData->uchMinorClass == CLASS_REGDISC4
        || pData->uchMinorClass == CLASS_REGDISC5
        || pData->uchMinorClass == CLASS_REGDISC6
        || pData->uchMinorClass == CLASS_CHARGETIP
        || pData->uchMinorClass == CLASS_CHARGETIP2 /* V3.3 */
        || pData->uchMinorClass == CLASS_CHARGETIP3) {
        pData->uchRate = 0;
    }

	if ((pData->fbDiscModifier & (VOID_MODIFIER | RETURNS_TRANINDIC)) == 0) {
		pData->lAmount *= -1L;
		pData->fbDiscModifier |= VOID_MODIFIER;
	}

	/* TAR191625 Canadian Tax is not correct after voided discount operation. */
    if (pData->uchMinorClass == CLASS_REGDISC1
        || pData->uchMinorClass == CLASS_REGDISC2
        || pData->uchMinorClass == CLASS_REGDISC3
        || pData->uchMinorClass == CLASS_REGDISC4
        || pData->uchMinorClass == CLASS_REGDISC5
        || pData->uchMinorClass == CLASS_REGDISC6)
	{
		TRANITEMIZERS   *ItemizerRcvBuff;

    	if (TrnSplCheckSplit() == TRN_SPL_SEAT || TrnSplCheckSplit() == TRN_SPL_MULTI || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        	TrnGetTISpl(&ItemizerRcvBuff, TRN_TYPE_SPLITA);
	    } else {
    	    TrnGetTI(&ItemizerRcvBuff);
	    }
    
        if (ItemDiscGetItemizerSeat(pData) == ITM_SEAT_DISC) {
            TrnGetTISpl(&ItemizerRcvBuff, TRN_TYPE_SPLITA);
        }

		if ((pData->uchSeatNo == 0) || (pData->uchSeatNo == 'B')) {          /* Disc for Base Trans. */
			/* override discount amount for marged checks */
			if (pData->lAmount < 0) {
				pData->lAmount = RflLLabs(ItemizerRcvBuff->lVoidDiscount[pData->uchMinorClass - CLASS_REGDISC1]);
				pData->lAmount *= -1L;
			} else if ( (pData->fbDiscModifier & RETURNS_TRANINDIC) == 0) {
				pData->lAmount = RflLLabs(ItemizerRcvBuff->lVoidDiscount[pData->uchMinorClass - CLASS_REGDISC1]);
			}
			pData->fbDiscModifier |= VOIDSEARCH_MODIFIER;
		} else {
			pData->fbDiscModifier |= VOIDSEARCH_MODIFIER;
		}
	}

	/** correct the order of spv int */
    if ((pData->auchDiscStatus[0] & DISC_SUP_INTV_REQD) || CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) {
        if (pData->uchMinorClass != CLASS_ITEMDISC1) {
            if ((sStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    }

    /* send to enhanced kds, 2172 */
    if (ItemSendKds(pData, 0) != ITM_SUCCESS) {
		NHPOS_ASSERT_TEXT(0, "**ERROR: ItemSendKds() failed in ItemDiscCursorVoid(). Continuing.");
    }

	{
		REGDISPMSG      Disp = {0};
		PARATRANSMNEMO  WorkMnem = { 0 };

		Disp.uchMajorClass = CLASS_UIFREGDISC;
		Disp.uchMinorClass = CLASS_UIREGDISC1;
		Disp.lAmount = pData->lAmount;

		ItemDiscCommonTranMnemonic (pData->uchMinorClass, pData->uchDiscountNo, &WorkMnem);

		_tcsncpy(Disp.aszMnemonic, WorkMnem.aszMnemonics, PARA_TRANSMNEMO_LEN);
		_tcsncpy(pData->aszMnemonic, WorkMnem.aszMnemonics, NUM_DEPTPLU);

		flDispRegDescrControl |= ITEMDISC_CNTRL;
		flDispRegDescrControl |= VOID_CNTRL;
		flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
		DispWrite(&Disp);
		flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
	}

    MldScrollWrite(pData, MLD_NEW_ITEMIZE);
	MldScrollFileWrite(pData);
    ItemCommonDispSubTotal(pData);

    if ((pData->auchDiscStatus[0] & DISC_VALDATION_PRINT) || CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1)) {
        pData->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pData);
        pData->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    }

    ItemPreviousItem(pData, 0);

	{
		ITEMSALESLOCAL  * const pWorkSales = ItemSalesGetLocalPointer();
		ULONG           ulWork = 0, ulWork2 = 0;
		SHORT           sCheckSplit = TrnSplCheckSplit();

		switch(pData->uchMinorClass) {
			case CLASS_REGDISC1:
				ulWork = GCFQUAL_REGULAR1;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_REGDISC2:
				ulWork = GCFQUAL_REGULAR2;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_REGDISC3:
				ulWork2 = GCFQUAL_REGULAR3;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_REGDISC4:
				ulWork2 = GCFQUAL_REGULAR4;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_REGDISC5:
				ulWork2 = GCFQUAL_REGULAR5;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_REGDISC6:
				ulWork2 = GCFQUAL_REGULAR6;
				pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
				break;

			case CLASS_CHARGETIP:
			case CLASS_CHARGETIP2:  /* V3.3 */
			case CLASS_CHARGETIP3:
			case CLASS_AUTOCHARGETIP:
			case CLASS_AUTOCHARGETIP2:
			case CLASS_AUTOCHARGETIP3:
				ulWork = GCFQUAL_CHARGETIP;
				ulWork2 = GCFQUAL_AUTOCHARGETIP;
				break;

			default:
				break;
		}

		if (sCheckSplit == TRN_SPL_SEAT || sCheckSplit == TRN_SPL_MULTI || sCheckSplit == TRN_SPL_TYPE2) {
			TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualSplPointer(TRN_TYPE_SPLITA);

			pWorkGCF->fsGCFStatus &= ~ulWork;
			pWorkGCF->fsGCFStatus2 &= ~ulWork2;
		} else {
			TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr ();

			if (pData->uchSeatNo == 0) {
				pWorkGCF->fsGCFStatus &= ~ulWork;
				pWorkGCF->fsGCFStatus2 &= ~ulWork2;
			} else {
				UCHAR   uchMaskSeatDisc = GCFQUAL_REG1_SEAT;

				pWorkGCF->auchSeatDisc[pData->uchSeatNo - 1] &= ~(uchMaskSeatDisc << (pData->uchMinorClass - CLASS_REGDISC1));
			}
		}
	}

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCursorVoidCheck(ITEMDISC *pData)
*
*    Input: 
*   Output: None
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*
** Description: Check relation between GCF qualifier status
*===========================================================================
*/
SHORT   ItemDiscCursorVoidCheck(ITEMDISC *pData)
{
    SHORT           sReturnStatus;

    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
		/* cashier transaction  ? */
        sReturnStatus = ITM_SUCCESS;
    } else {
        sReturnStatus = ItemDiscCheckGCFStat();
    }

    if (pData->uchSeatNo != 0 &&
        (pData->uchMinorClass == CLASS_REGDISC1 ||
        pData->uchMinorClass == CLASS_REGDISC2  ||
        pData->uchMinorClass == CLASS_REGDISC3  ||
        pData->uchMinorClass == CLASS_REGDISC4  ||
        pData->uchMinorClass == CLASS_REGDISC5  ||
        pData->uchMinorClass == CLASS_REGDISC6)) {

		if ((pData->uchSeatNo > 0) && (pData->uchSeatNo <= NUM_SEAT)) { //SR206
			TRANGCFQUAL   *GCFQualRcvBuff = TrnGetGCFQualPtr();
			UCHAR         uchMaskSeatDisc = GCFQUAL_REG1_SEAT;

            if (!(GCFQualRcvBuff->auchSeatDisc[pData->uchSeatNo - 1] & (uchMaskSeatDisc << (pData->uchMinorClass - CLASS_REGDISC1)))) {
                return(LDT_PROHBT_ADR);
            }
            return(ITM_SUCCESS);
		} else {

            return(ITM_SUCCESS);
        }

    } else {
        if (sReturnStatus == ITM_SUCCESS) {
			TRANGCFQUAL   *GCFQualRcvBuff;

            if (TrnSplCheckSplit() == TRN_SPL_SEAT
                || TrnSplCheckSplit() == TRN_SPL_MULTI
                || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
                TrnGetGCFQualSpl(&GCFQualRcvBuff, TRN_TYPE_SPLITA);
            } else {
                TrnGetGCFQual(&GCFQualRcvBuff);
            }
            switch(pData->uchMinorClass) {
            case CLASS_REGDISC1:
                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR1)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;
        
            case CLASS_REGDISC2:
                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR2)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;

            case CLASS_REGDISC3:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR3)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;
        
            case CLASS_REGDISC4:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR4)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;
        
            case CLASS_REGDISC5:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR5)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;
        
            case CLASS_REGDISC6:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR6)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;
        
            case CLASS_CHARGETIP:
            case CLASS_CHARGETIP2:
            case CLASS_CHARGETIP3:
            case CLASS_AUTOCHARGETIP:
            case CLASS_AUTOCHARGETIP2:
            case CLASS_AUTOCHARGETIP3:
                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_CHARGETIP)) { 
                    sReturnStatus = LDT_PROHBT_ADR;
                }
                break;

            default:
                break;
            }
        }
    }

    return(sReturnStatus);
}

/****** end of source ******/
