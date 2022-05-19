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
* Title       : Check operation sequence for discount.                          
* Category    : Dicount Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: DISCCHK.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemDiscCheckSeq() : Discount sequence check function main   
*           ItemDiscHALOProc() : HALO check for amount overriding
*           ItemDiscGetItemizer() : Get discountable itemizser
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun-20-92: 00.00.01  : K.Maeda   : initial                                   
* Oct-19-93: 02.00.02  : K.You     : del. canada tax feature.                        
* Mar-01-95: 03.00.00  : hkato     : R3.0
* Nov-09-95: 03.01.00  : hkato     : R3.1
* Aug-05-98: 03.03.00  : hrkato    : V3.3
* Aug-08-13: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
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

#include    <string.h>
#include    <stdlib.h>
#include    <math.h>
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <rfl.h>
#include    <uie.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    <mld.h>
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetItemizerSub(VOID)
*
*    Input: ITEMDISC *pItemDisc, LONG *plItemizer 
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*
** Description: Get discountable itemizer.                        R3.1
*===========================================================================
*/
SHORT   ItemDiscGetItemizerSub(VOID)
{
    UCHAR           uchSplitFlag;
    SHORT           sStatus;
    ITEMCOMMONLOCAL *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2)
	{
        uchSplitFlag = 1;               /* Set Split System Flag 6/1/96 */
    } else {
        uchSplitFlag = 0;               /* Set Not Split System Flag 6/1/96 */
    }

    if ((pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMSALES) ||
        (pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMCOUPON) ||
        (pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMDISC))
	{
		ITEMDISC        DummyDisc = {0};

        if (uchSplitFlag == 0) {
            if ((sStatus = ItemPreviousItem(&DummyDisc, 0)) != ITM_SUCCESS) {
                return(sStatus);
            }
        } else {
            if ((sStatus = ItemPreviousItemSpl(&DummyDisc, 0)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetCursorItem(VOID)
*
*    Input: ITEMDISC *pItemDisc, LONG *plItemizer 
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*
** Description: Get Cursor Item for Item Disc. V3.3
*===========================================================================
*/
SHORT   ItemDiscGetCursorItem(ITEMDISC *pItemDisc)
{
    ITEMSALES           ItemSales;
	ITEMDISC            ItemDiscount;
    SHORT               sStatus, sQty;

	// check if transaction started or if we have entered items for a tray.
    if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0) {           /* itemize state */
        return (LDT_SEQERR_ADR);
    }

    /*----- Send Previous Item, if any, to Transaction Module -----*/
    if ((sStatus = ItemPreviousItem(0, 0)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* read cursor position item */
	if (MldGetCursorDisplay(MLD_SCROLL_1, &ItemSales, &ItemDiscount, MLD_GET_SALES_DISC) != MLD_SUCCESS) {
        return (LDT_PROHBT_ADR);
    }

	if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY) {
		// if we are doing Tray Total then allow Cursor Discount only on items entered
		// after the last Tray Total Key press.
		if (ItemSales.usUniqueID <= ItemTotalLocalPtr->usUniqueID) {
			return(LDT_PROHBT_ADR);
		}
	}

    if (ItemSales.uchMajorClass == CLASS_ITEMSALES) {
        switch(ItemSales.uchMinorClass) {
        case CLASS_DEPT:                    /* dept */
        case CLASS_PLU:                     /* PLU */
        case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
        case CLASS_SETMENU:                 /* setmenu */

			if (ItemSales.fbModifier & VOID_MODIFIER) {	/* DTREE */
				if (ItemSales.fbModifier & RETURNS_ORIGINAL) {	/* DTREE */
					// if the sales item is voided or from the original transaction then do not allow a discount
					// to be applied to it.
					return(LDT_PROHBT_ADR);
				}
				if ((ItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) == 0) {	/* DTREE */
					return(LDT_PROHBT_ADR);
				}
            }

            /* check item discount have already been done or not */
            if (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                sQty = 1;
            } else {
                sQty = (SHORT)(ItemSales.lQTY / 1000L);
            }
            if ((sStatus = TrnVoidSearch(&ItemSales, sQty, TRN_TYPE_CONSSTORAGE)) != TRN_SUCCESS) {
				// return the error code provided by TrnVoidSearch() rather than a generic Sequence Error.
				// will notify operator that item is not on file.
                return(sStatus);
            }

/**** removed by unnecessary recalculation, 07/23/01
            / ----- calculate transfer qty and price /
            if (!(ItemSales.ControlCode.auchStatus[2] & PLU_SCALABLE)) {

                if (ItemSales.uchPPICode) { / calculate ppi price /
                    if (ItemSalesOnlyCalcPPI(&ItemSales) != ITM_SUCCESS) {
                        ItemSales.lProduct = (LONG)(ItemSales.lUnitPrice * ItemSales.sCouponQTY);
                    } else {    / ppi sales price /
                        if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) { / preselect void trans. /
                            ItemSales.lProduct *= -1L;
                        }
                    }
                } else
                if (ItemSales.uchPM) { / calculate price multiple 2172 /
                    if (ItemSalesOnlyCalcPM(&ItemSales) != ITM_SUCCESS) {
                        ItemSales.lProduct = (LONG)(ItemSales.lUnitPrice * ItemSales.sCouponQTY);
                    } else {    / ppi sales price /
                        if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) { / preselect void trans. /
                            ItemSales.lProduct *= -1L;
                        }
                    }
                } else {
                    ItemSales.lProduct = (LONG)(ItemSales.lUnitPrice * ItemSales.sCouponQTY);
                }
            }
*****/
            sStatus = TrnCheckSize(&ItemSales, 0);
			if (sStatus < 0) {
                return(LDT_TAKETL_ADR);
			}
            sStatus += sizeof(ITEMDISC);  /* reduce item size */
            if (ItemCommonCheckSize(&ItemSales, sStatus) < 0) {
                return(LDT_TAKETL_ADR);
            }

            /* reduce item for item disc. link */
            TrnSplReduce (&ItemSales);
			MldScrollFileDelete (&ItemSales);

            /* set target item to previous buffer for item disc. */
            if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) ||
                ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)) {

                /* if terget item does not exist in item storage bufffer */
                if (TrnVoidSearch(&ItemSales, sQty, TRN_TYPE_ITEMSTORAGE) != TRN_SUCCESS) {
                    ItemSales.fbStorageStatus |= NOT_ITEM_STORAGE;
                }
            }
            ItemSales.fbReduceStatus |= REDUCE_NOT_HOURLY;
			ItemSales.usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ
            if (( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
                /* unbuffering print */
                /* do not print parent item duplicately */
                ItemSales.fsPrintStatus |= PRT_NO_UNBUFFING_PRINT;   
            }
            if ((sStatus = ItemPreviousItem(&ItemSales, 0)) != ITM_SUCCESS) {
                return(sStatus);
            }

            /* redisplay lcd scroll */
#if 1
			MldRedisplayToScroll (MLD_SCROLL_1);
#else
            TrnGetTranInformation(&pWorkTran);
            if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
            || ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
                && (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER)) {
                WorkMLD.usVliSize = pWorkTran->usTranConsStoVli;
                WorkMLD.sFileHandle = pWorkTran->hsTranConsStorage;
			    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {      /* Display by Prnt Priority */
	                WorkMLD.sIndexHandle = pWorkTran->hsTranConsIndex;
	            } else {
	                WorkMLD.sIndexHandle = pWorkTran->hsTranNoConsIndex;
	            }
            } else {
                WorkMLD.usVliSize = pWorkTran->usTranItemStoVli;
                WorkMLD.sFileHandle = pWorkTran->hsTranItemStorage;
			    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {      /* Display by Prnt Priority */
	                WorkMLD.sIndexHandle = pWorkTran->hsTranItemIndex;
	        	} else {
	                WorkMLD.sIndexHandle = pWorkTran->hsTranNoItemIndex;
		        }
            }
            /* display from the end */
            MldPutTrnToScrollReverse(&WorkMLD);
#endif
			//US Customs cwunn 5/1/03
			//signify that item discount has occurred for this PLU, to ensure proper display
			ItemSales.uchDiscountSignifier |= DISC_SIG_DISCOUNT;
            MldScrollWrite(&ItemSales, MLD_NEW_ITEMIZE);

			pItemDisc->fbDiscModifier &= ~RETURNS_TRANINDIC;  // this is a new sales item so we will turn off the Returns indicator

            return(ITM_SUCCESS);
            break;
        }
    }

    return(LDT_PROHBT_ADR);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCheckSeq(UIFREGDISC *pUifRegDisc,
*                       ITEMDISC *pItemDisc, LONG *plItemizerBuff)
*
*    Input: , LONG *plItemizerBuff
*
*   Output: None
*
*    InOut: None
*
**Return: COMMON_SUCCESS    : Function Performed succesfully
*         LDT_TAKETL_ADR : Function aborted with Illegal operation sequense error
*
** Description: Send buffered previous item to transaction module and 
*               put current item into saved buffer
*===========================================================================
*/
SHORT   ItemDiscCheckSeq(UIFREGDISC *pUifRegDisc, ITEMDISC *pItemDisc, DCURRENCY *plItemizerBuff)
{
    UCHAR               uchMDC;
    SHORT               sReturnStatus;
    PARADISCTBL         DiscRateRcvBuff;
	DCURRENCY           lVoidDiscount;
	BOOL                bAmtrakChargeTips = FALSE;   // allow Charge Tips to be zero for Amtrak

    /*--- CHECK COMPULSORY OPERATION ---*/
    if ((sReturnStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- check split check mode R3.1 -----*/
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {

        if (pItemDisc->uchMinorClass == CLASS_ITEMDISC1) {
            if ((sReturnStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        } else {
            if (pUifRegDisc->uchMinorClass != CLASS_UIAUTOCHARGETIP) {  /* V3.3 */
                if (!((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER)) {
                    if (TranCurQualPtr->uchSplit != 0 && TranCurQualPtr->uchSeat == 0) {
                        return(LDT_SEQERR_ADR);
                    }
                }
            }
        }
    }

    if (pItemDisc->uchMinorClass == CLASS_ITEMDISC1) {
		ITEMCOMMONLOCAL     *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

        /* Curor Item Discount, V3.3 */
        if (MldQueryMoveCursor(MLD_SCROLL_1) == TRUE) {
            if ((sReturnStatus = ItemDiscGetCursorItem(pItemDisc)) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }

		if (pCommonLocalRcvBuff->ItemSales.uchMajorClass != CLASS_ITEMSALES) {
            return(LDT_SEQERR_ADR);
        }
    
        if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {
            return(LDT_PROHBT_ADR);
        }

        if (pCommonLocalRcvBuff->ItemSales.fbModifier & RETURNS_ORIGINAL) {
            return(LDT_PROHBT_ADR);
        }

        if (!((pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPT) ||
              (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLU) ||
              (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_OEPPLU) || /* R3.0 */
              (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMENU))) {

            return(LDT_SEQERR_ADR);
        }
    
		if ((pUifRegDisc->fbModifier & RETURNS_TRANINDIC) == 0) {
			// This is not a return transaction so do a consistency check to see if this is a voided
			// item that we are discounting.
			if ((pUifRegDisc->fbModifier & VOID_MODIFIER) ^ (pCommonLocalRcvBuff->ItemSales.fbModifier & VOID_MODIFIER)) {
				return(LDT_SEQERR_ADR);
			}
		}

        _tcsncpy(pItemDisc->auchPLUNo, pCommonLocalRcvBuff->ItemSales.auchPLUNo, NUM_PLU_LEN);              /* set PLU No. of noun */
        pItemDisc->usDeptNo = pCommonLocalRcvBuff->ItemSales.usDeptNo;          /* set Dept No. of noun */

		// Set the quantity for the discount to properly update counts in totals
		if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)
			pItemDisc->lQTY = 1;                                            // Scalable so use count of one.
		else
			pItemDisc->lQTY = pCommonLocalRcvBuff->ItemSales.lQTY/1000L;    // Not scalable so divide by 1000 for number of items

        pItemDisc->uchAdjective = pCommonLocalRcvBuff->ItemSales.uchAdjective;    /* set adj. of noun */

		if (pUifRegDisc->fbModifier & RETURNS_TRANINDIC) {
			pItemDisc->fbReduceStatus |= REDUCE_RETURNED;
			pItemDisc->fbDiscModifier |= pUifRegDisc->fbModifier;
		}
    }

	bAmtrakChargeTips = (pItemDisc->uchMinorClass == CLASS_CHARGETIP || pItemDisc->uchMinorClass == CLASS_CHARGETIP2 || pItemDisc->uchMinorClass == CLASS_CHARGETIP3);
	bAmtrakChargeTips = (bAmtrakChargeTips && (pUifRegDisc->uchDiscountNo != 0));  // Check if an actual zero amount was entered or not.  UifChargeTips() sets this.

    /*--- CHECK AMOUNT COMP AT VOID ---*/
    if ((pItemDisc->uchMinorClass != CLASS_ITEMDISC1) && (pUifRegDisc->fbModifier & VOID_MODIFIER)) {
		//  For Amtrak we are allowing a charge tips amount of 0 to be specified
        if (! bAmtrakChargeTips && pUifRegDisc->lAmount == 0) {
            return(LDT_PROHBT_ADR);
        }

        /*--- DISCOUNT HALO CHECK FOR AMOUNT OVERRIDE ---*/
        if ((sReturnStatus = ItemDiscHALOProc(&(pUifRegDisc->lAmount), pItemDisc)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }

        /*--- HALO CHECK FOR ITEM VOID ---*/
        if (pUifRegDisc->fbModifier & VOID_MODIFIER) {
            if ((sReturnStatus = ItemSalesComnIFVoidHALO(pUifRegDisc->lAmount)) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }

        pItemDisc->lAmount = pUifRegDisc->lAmount;      /* set overrided discount amount */

		/* TAR191625 Canadian Tax is not correct after voided discount operation. */
	    if (pItemDisc->uchMinorClass == CLASS_REGDISC1
    	    || pItemDisc->uchMinorClass == CLASS_REGDISC2
        	|| pItemDisc->uchMinorClass == CLASS_REGDISC3
	        || pItemDisc->uchMinorClass == CLASS_REGDISC4
    	    || pItemDisc->uchMinorClass == CLASS_REGDISC5
        	|| pItemDisc->uchMinorClass == CLASS_REGDISC6) {

		    /*--- GET DISCOUNTABLE ITEMIZER ---*/
    		if ((sReturnStatus = ItemDiscGetItemizer(pItemDisc, plItemizerBuff, &lVoidDiscount)) == ITM_SUCCESS) {
			
				if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {          /* Disc for Base Trans. */
        	    	if (RflLLabs(lVoidDiscount) == RflLLabs(pUifRegDisc->lAmount)) {
						pItemDisc->fbDiscModifier |= VOIDSEARCH_MODIFIER;
		        		pUifRegDisc->fbModifier |= VOIDSEARCH_MODIFIER;
    		        } else {
						/* void search option */
        				if (CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT2)) {
            				if ((sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {
		                		return(sReturnStatus);
		            		}
		            	}
		            }
		        } else {
					pItemDisc->fbDiscModifier |= VOIDSEARCH_MODIFIER;
	        		pUifRegDisc->fbModifier |= VOIDSEARCH_MODIFIER;
	        	}
			} else {
                return(sReturnStatus);
			}
	    }

        return(ITM_SUCCESS);                /* ignore % / amount override option */
    }

    /*--- GET DISCOUNTABLE ITEMIZER ---*/
    if ((sReturnStatus = ItemDiscGetItemizer(pItemDisc, plItemizerBuff, &lVoidDiscount)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*--- CHECK ITEMIZER ---*/
    switch (pItemDisc->uchMinorClass) { /* V3.3 */
    case CLASS_CHARGETIP:
    case CLASS_CHARGETIP2:
    case CLASS_CHARGETIP3:
		if(!CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT3)) {
			if (pUifRegDisc->lAmount) { /* manual entry */
					if ((*plItemizerBuff <= 0) && ((pItemDisc->fbDiscModifier & RETURNS_TRANINDIC) == 0)) {
						// if this is not a return then do not allow a negative or zero balance.
						// however if this is a return the balance may be negative or zero.
						return(LDT_PROHBT_ADR);
					}
			}
		}
		else {
			if (!pUifRegDisc->lAmount) { /* non manual entry */
				if (pUifRegDisc->lAmount &&  CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT1) ) { /* percent override entry */
					if ((*plItemizerBuff <= 0) && ((pItemDisc->fbDiscModifier & RETURNS_TRANINDIC) == 0)) {
						// if this is not a return then do not allow a negative or zero balance.
						// however if this is a return the balance may be negative or zero.
						return(LDT_PROHBT_ADR);
					}
				}
			}
		}
    }

    /*--- DISCOUNT BY OVERRIDDEN RATE OR AMOUNT ---*/
    if (pUifRegDisc->lAmount != 0 || bAmtrakChargeTips) {   /* exist overridden amount or this is a charge tips for Amtrak */
        switch(pItemDisc->uchMinorClass) {  /* get MDC option fo overriding */
        case CLASS_ITEMDISC1:               /* address 31 */
			{
				USHORT  usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID11_ADR);  /* address 31 */

				uchMDC = (CliParaMDCCheckField(usAddress, MDC_PARAM_BIT_D) ? DISC_ALLOW_OVERRIDE : 0);
				uchMDC |= (CliParaMDCCheckField(usAddress, MDC_PARAM_BIT_C) ? DISC_RATE_OVERRIDE : 0);
			}
            break;
        
        case CLASS_REGDISC1:                /* adddress 39 */
            uchMDC = CliParaMDCCheck(MDC_RDISC11_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;
        
        case CLASS_REGDISC2:                /* address 43 */
            uchMDC = CliParaMDCCheck(MDC_RDISC21_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;
        
        case CLASS_REGDISC3:                /* R3.1 */
            uchMDC = CliParaMDCCheck(MDC_RDISC31_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;

        case CLASS_REGDISC4:                /* R3.1 */
            uchMDC = CliParaMDCCheck(MDC_RDISC41_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;

        case CLASS_REGDISC5:                /* R3.1 */
            uchMDC = CliParaMDCCheck(MDC_RDISC51_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;

        case CLASS_REGDISC6:                /* R3.1 */
            uchMDC = CliParaMDCCheck(MDC_RDISC61_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;

        case CLASS_CHARGETIP:               /* address 23 */
        case CLASS_CHARGETIP2:               /* address 23 */
        case CLASS_CHARGETIP3:               /* address 23 */
            uchMDC = CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1);
            break;

        default:
			uchMDC = 0;
			PifLog (MODULE_DISCOUNT, LOG_EVENT_UI_DISCOUNT_ERR_04);
            break;
        }

        if (!(uchMDC & DISC_ALLOW_OVERRIDE)) {
            return(LDT_PROHBT_ADR);
        }

        if (uchMDC & DISC_RATE_OVERRIDE) {
            if (pUifRegDisc->lAmount > 100L) {
                return(LDT_KEYOVER_ADR);    /* error if rate exceed 100 % */
            }
            if ((*plItemizerBuff <= 0) && ((pUifRegDisc->fbModifier & RETURNS_TRANINDIC) == 0)) {
				// if this is not a return then do not allow a negative or zero balance.
				// however if this is a return the balance may be negative or zero.
                return(LDT_PROHBT_ADR);
            }

			// The amount entered is the discount rate (1% to 100%) so set the rate and return.
			// Code later will perform the HALO check that is performed for any discount with rate indicated.
            pItemDisc->uchRate = (UCHAR) (pUifRegDisc->lAmount);   /* set overrided discount rate */
            return(ITM_SUCCESS);
        }

        /*--- AMOUNT DISCOUNT PROCESS ---*/
        /* CHECK ITEMIZER */
        switch(pItemDisc->uchMinorClass) {  /* get MDC option fo overriding */
        case CLASS_CHARGETIP:               /* address 23 */
        case CLASS_CHARGETIP2:               /* address 23 */
        case CLASS_CHARGETIP3:               /* address 23 */
            break;

        default:
            if ((*plItemizerBuff < pUifRegDisc->lAmount) && ((pUifRegDisc->fbModifier & RETURNS_TRANINDIC) == 0)) {
				// if this is not a return then do not allow a negative or zero balance.
				// however if this is a return the balance may be negative or zero.
                return(LDT_KEYOVER_ADR);
            } 
        }

        /*--- HALO CHECK FOR AMOUNT OVERRIDE ---*/
        if ((sReturnStatus = ItemDiscHALOProc(&(pUifRegDisc->lAmount), pItemDisc)) != ITM_SUCCESS) {
			if (pItemDisc->uchMinorClass != CLASS_CHARGETIP) {
				if ((sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if Discount amount is over HALO
					return(sReturnStatus);
				}
			} else {
				return(sReturnStatus);
			}
        }

        /*--- HALO CHECK FOR ITEM VOID ---*/
        if (pUifRegDisc->fbModifier & VOID_MODIFIER) {
            if ((sReturnStatus = ItemSalesComnIFVoidHALO(pUifRegDisc->lAmount)) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }

        pItemDisc->lAmount = pUifRegDisc->lAmount;       /* set overrided discount amount */
		pItemDisc->fbDiscModifier |= pUifRegDisc->fbModifier;
		if ((pUifRegDisc->fbModifier & RETURNS_TRANINDIC) != 0) {
			pItemDisc->lAmount *= -1;
		}
        return(ITM_SUCCESS);
    }
    
    /*--- DISCOUNT BY PRESET RATE ---*/
    /* GET DISCOUNT CHARGE TIP RATE */
    DiscRateRcvBuff.uchMajorClass = CLASS_PARADISCTBL; /* major class of disc rate parameter */
    switch(pUifRegDisc->uchMinorClass) {               /* get address of rate */
    case CLASS_UIITEMDISC1:
		DiscRateRcvBuff.uchAddress = ItemDiscGetRate_ITEMDISC1 (pItemDisc->uchDiscountNo);
       break;
    
    case CLASS_UIREGDISC1:                              
        DiscRateRcvBuff.uchAddress = RATE_RDISC1_ADR;  /* reg. disc. 1 address 3 */                               
        break;
    
    case CLASS_UIREGDISC2:                              
        DiscRateRcvBuff.uchAddress = RATE_RDISC2_ADR;  /* reg. disc. 2 address 4 */                               
        break;
    
    case CLASS_UIREGDISC3:                  /* R3.1 */
        DiscRateRcvBuff.uchAddress = RATE_RDISC3_ADR;
        break;
    
    case CLASS_UIREGDISC4:                  /* R3.1 */
        DiscRateRcvBuff.uchAddress = RATE_RDISC4_ADR;     
        break;
    
    case CLASS_UIREGDISC5:                  /* R3.1 */
        DiscRateRcvBuff.uchAddress = RATE_RDISC5_ADR;
        break;
    
    case CLASS_UIREGDISC6:                  /* R3.1 */
        DiscRateRcvBuff.uchAddress = RATE_RDISC6_ADR;
        break;
    
    case CLASS_UICHARGETIP:                             
        DiscRateRcvBuff.uchAddress = RATE_CRG_TIP_ADR;   /* charge tip address 13 */                                 
        break;
    
    case CLASS_UICHARGETIP2:                             
        DiscRateRcvBuff.uchAddress = RATE_CRG_TIP2_ADR;  /* charge tip address 19 */
        break;
    
    case CLASS_UICHARGETIP3:                             
        DiscRateRcvBuff.uchAddress = RATE_CRG_TIP3_ADR;  /* charge tip address 20 */
        break;
    
    case CLASS_UIAUTOCHARGETIP:
        if (pItemDisc->uchMinorClass == CLASS_AUTOCHARGETIP) {
            DiscRateRcvBuff.uchAddress = RATE_CRG_TIP_ADR;        /* charge tip address 13 */                                 
        } else
        if (pItemDisc->uchMinorClass == CLASS_AUTOCHARGETIP2) {
            DiscRateRcvBuff.uchAddress = RATE_CRG_TIP2_ADR;       /* charge tip address 19 */
        } else {
            DiscRateRcvBuff.uchAddress = RATE_CRG_TIP3_ADR;       /* charge tip address 20 */
        }
        break;
    default:
        break;
    }
        
    CliParaRead(&DiscRateRcvBuff);          /* get discount rate */
        
    if (DiscRateRcvBuff.uchDiscRate == 0) {
        return(LDT_PROHBT_ADR);
    }
       
    pItemDisc->uchRate = (UCHAR) (DiscRateRcvBuff.uchDiscRate);
    
    return(ITM_SUCCESS);
}       

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscHALOProc(LONG lAmount)
*
*    Input: LONG lAmount
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*         LDT_KEYOVER_ADR : HALO error
*
** Description: HALO Check for overrided discount amount. 
*                
*===========================================================================
*/

SHORT   ItemDiscHALOProc(DCURRENCY *plAmount, ITEMDISC *pItemDisc)
{
    PARATRANSHALO      HaloDataRcvBuff;
	USHORT             usMdcHaloCap = 0;
 
    HaloDataRcvBuff.uchMajorClass = CLASS_PARATRANSHALO;
            
    switch(pItemDisc->uchMinorClass) {
    case CLASS_ITEMDISC1:                             /* item discount #1 address */
        HaloDataRcvBuff.uchAddress = HALO_MOTERID_ADR;
		usMdcHaloCap = MDC_MODID15_ADR;
		switch (pItemDisc->uchDiscountNo) {
			case 3:
				HaloDataRcvBuff.uchAddress = HALO_DISC3_ADR;
				usMdcHaloCap = MDC_MODID35_ADR;
				break;
			case 4:
				HaloDataRcvBuff.uchAddress = HALO_DISC4_ADR;
				usMdcHaloCap = MDC_MODID45_ADR;
				break;
			case 5:
				HaloDataRcvBuff.uchAddress = HALO_DISC5_ADR;
				usMdcHaloCap = MDC_MODID55_ADR;
				break;
			case 6:
				HaloDataRcvBuff.uchAddress = HALO_DISC6_ADR;
				usMdcHaloCap = MDC_MODID65_ADR;
				break;
		}
        break;

    case CLASS_REGDISC1:                              /* item reg. disc1 address */
        HaloDataRcvBuff.uchAddress = HALO_RDISC1_ADR;
		usMdcHaloCap = MDC_RDISC15_ADR;
        break;
    
    case CLASS_REGDISC2:                              /* item reg. disc2 address */
        HaloDataRcvBuff.uchAddress = HALO_RDISC2_ADR;
		usMdcHaloCap = MDC_RDISC25_ADR;
        break;
    
    case CLASS_REGDISC3:                              /* R3.1 */
        HaloDataRcvBuff.uchAddress = HALO_RDISC3_ADR;
		usMdcHaloCap = MDC_RDISC35_ADR;
        break;
    
    case CLASS_REGDISC4:                              /* R3.1 */
        HaloDataRcvBuff.uchAddress = HALO_RDISC4_ADR;
		usMdcHaloCap = MDC_RDISC45_ADR;
        break;
    
    case CLASS_REGDISC5:                              /* R3.1 */
        HaloDataRcvBuff.uchAddress = HALO_RDISC5_ADR;
		usMdcHaloCap = MDC_RDISC55_ADR;
        break;
    
    case CLASS_REGDISC6:                              /* R3.1 */
        HaloDataRcvBuff.uchAddress = HALO_RDISC6_ADR;
		usMdcHaloCap = MDC_RDISC65_ADR;
        break;
    
    case CLASS_CHARGETIP:                             /* item charge tip address */
        HaloDataRcvBuff.uchAddress = HALO_CHRGTIP_ADR;
        break;

    /* V3.3 */
    case CLASS_CHARGETIP2:                             /* item charge tip address */
        HaloDataRcvBuff.uchAddress = HALO_CHRGTIP2_ADR;
        break;
    
    case CLASS_CHARGETIP3:                             /* item charge tip address */
        HaloDataRcvBuff.uchAddress = HALO_CHRGTIP3_ADR;
        break;
    
    default:
        return (ITM_SUCCESS);   /* auto charge tips */
        break;
    }

    CliParaRead(&HaloDataRcvBuff);  /* get halo amount */    
    
    /*--- HALO CHECK ---*/
    if (RflHALO_OverRide(plAmount, HaloDataRcvBuff.uchHALO, usMdcHaloCap, MDC_PARAM_BIT_D) != RFL_SUCCESS) {
		if (pItemDisc->uchMinorClass == CLASS_CHARGETIP) {
			// we allow for supervisor intervention to exceed charge tip amount over HALO.
			if(CliParaMDCCheckField(MDC_HALOOVR_ADR, MDC_PARAM_BIT_C)) {
				if (ItemSPVIntAlways(LDT_SUPINTR_ADR) == ITM_SUCCESS) {
					return(ITM_SUCCESS);
				}
			}
		}
        return(LDT_KEYOVER_ADR);
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetItemizer(ITEMDISC *pItemDisc, LONG *plItemizerBuff)
*
*    Input: ITEMDISC *pItemDisc, LONG *plItemizer 
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*
** Description: Get discountable itemizer.                        R3.1
*               This function is used to determine which itemizer is to be used
*               with Transaction Discounts (Regular Discounts) that have been
*               added, Regular Discounts #3, #4, #5, and #6.  With these newer
*               Transaction Discounts we allow flexibility in determining which
*               of the itemizers will be used for processing a Transaction Discount.
*
*               Transaction Discounts #1 and #2 are hard coded to always use either
*               Discount Itemizer #1 or Discount Itemizer #2 respectively.  For the
*               new ones we allow provisioning to choose which of the two Discount Itemizers
*               to use or if the taxable itemizer should be used instead of a Discount Itemizer.
*
*    TAR191625 Canadian Tax is not correct after voided discount operation.
*===========================================================================
*/
static SHORT ItemDiscGetItemizerRegDisc (USHORT usMdcAdr, TRANITEMIZERS **pItemizerRcvBuff, ITEMDISC *pItemDisc, DCURRENCY *plItemizer)
{
	SHORT  sReturnStatus;

    if ((sReturnStatus = ItemDiscGetItemizerSub()) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if (ItemDiscGetItemizerSeat(pItemDisc) == ITM_SEAT_DISC) {
        TrnGetTISpl(pItemizerRcvBuff, TRN_TYPE_SPLITA);
    }

	switch (usMdcAdr) {
		case MDC_RDISC13_ADR:    // hard coded to use Discountable Itemizer #1
			*plItemizer = (*pItemizerRcvBuff)->lDiscountable[0];
			break;

		case MDC_RDISC23_ADR:    // hard coded to use Discountable Itemizer #2
			*plItemizer = (*pItemizerRcvBuff)->lDiscountable[1]; 
			break;

		case MDC_RDISC33_ADR:
		case MDC_RDISC43_ADR:
		case MDC_RDISC53_ADR:
		case MDC_RDISC63_ADR:
			// determine, based on the MDC setting (MDC_RDISC33_ADR, MDC_RDISC43_ADR, etc.), which itemizer to use.
			// decide whether to use regular discountable 1 or regular discountable 2 itemizer or
			// instead to use the taxable itemizer for the tax system that we are using.
			if (CliParaMDCCheckField(usMdcAdr, MDC_PARAM_BIT_C)) {         // MDC_RDISC33_ADR, MDC_RDISC43_ADR, etc.
				if (CliParaMDCCheckField(usMdcAdr, MDC_PARAM_BIT_B)) {
					*plItemizer = (*pItemizerRcvBuff)->lDiscountable[1]; 
				} else {
					*plItemizer = (*pItemizerRcvBuff)->lDiscountable[0];
				}
			} else {
				*plItemizer = (*pItemizerRcvBuff)->lMI;
				/* reduce tax amount from discount calc. */
				/* 03/31/01, LePeeps */
				if (ItemCommonTaxSystem() == ITM_TAX_US) {
					int i;

    				for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
        				*plItemizer -= (*pItemizerRcvBuff)->Itemizers.TranUsItemizers.lTaxItemizer[i];
					}
				} else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
					int i;

    				for (i = 0; i < STD_TAX_ITEMIZERS_CAN; i++) {
        				*plItemizer -= (*pItemizerRcvBuff)->Itemizers.TranCanadaItemizers.lAffTax[i];
					}
				}
			}
			break;

		default:
			sReturnStatus = LDT_PROHBT_ADR;
			break;
	}

    return(sReturnStatus);
}

SHORT   ItemDiscGetItemizer(ITEMDISC *pItemDisc, DCURRENCY *plItemizer, DCURRENCY *plVoidDiscount)
{
    LONG               lSign;
    SHORT              sReturnStatus;
    TRANITEMIZERS      *ItemizerRcvBuff;
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    /*----- PRESELECT VOID -----*/
    lSign = 1L;                             /* initialize sign */
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        lSign *= -1L;
    }

    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        TrnGetTISpl(&ItemizerRcvBuff, TRN_TYPE_SPLITA);

    } else {
        TrnGetTI(&ItemizerRcvBuff);
    }

    *plVoidDiscount = 0;	/* TAR191625 */

    switch(pItemDisc->uchMinorClass) {
    case CLASS_ITEMDISC1:
		{
			LONG       lQTYSave; /* 96/4/2 */
			DUNITPRICE lUnitPriceSave; /* 96/4/2 */
			DCURRENCY  lProductSave; /* 96/4/2 */

			/* save original price r3.1 96/4/2 */
			lQTYSave = pCommonLocalRcvBuff->ItemSales.lQTY;
			lUnitPriceSave = pCommonLocalRcvBuff->ItemSales.lUnitPrice;
			lProductSave = pCommonLocalRcvBuff->ItemSales.lProduct;

			/*--- get discountable itemizer ( noun's product) ---*/
			if (pCommonLocalRcvBuff->ItemSales.fbModifier & VOID_MODIFIER) { /* was noun voided? */
				pCommonLocalRcvBuff->ItemSales.lQTY *= -1L;
				pCommonLocalRcvBuff->ItemSales.lProduct *= -1L;
			}

			/*--- preselect void trans. ? ---*/
			pCommonLocalRcvBuff->ItemSales.lQTY *= lSign;
			pCommonLocalRcvBuff->ItemSales.lProduct *= lSign;

			ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), plItemizer); /* add condiment to noun */
	                                            
			/* recover original price r3.1 96/4/2 */
			pCommonLocalRcvBuff->ItemSales.lQTY = lQTYSave;
			pCommonLocalRcvBuff->ItemSales.lUnitPrice = lUnitPriceSave;
			pCommonLocalRcvBuff->ItemSales.lProduct = lProductSave;
		}
        break;

    case CLASS_REGDISC1:
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC13_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}
        *plItemizer *= lSign;               /* sign by preselect void */
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[0];	/* TAR191625 */
        break;

    case CLASS_REGDISC2:
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC23_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}
        *plItemizer *= lSign;               /* sign by preselect void */
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[1];	/* TAR191625 */
        break;

    case CLASS_REGDISC3:                    /* R3.1 */
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC33_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}
        *plItemizer *= lSign;
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[2];	/* TAR191625 */
        break;

    case CLASS_REGDISC4:                    /* R3.1 */
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC43_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}

		*plItemizer *= lSign;
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[3];	/* TAR191625 */
        break;

    case CLASS_REGDISC5:                    /* R3.1 */
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC53_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}

		*plItemizer *= lSign;
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[4];	/* TAR191625 */
        break;

    case CLASS_REGDISC6:                    /* R3.1 */
        if ((sReturnStatus = ItemDiscGetItemizerRegDisc(MDC_RDISC63_ADR, &ItemizerRcvBuff, pItemDisc, plItemizer)) != ITM_SUCCESS) {
            return(sReturnStatus);
		}

        *plItemizer *= lSign;
        *plVoidDiscount = ItemizerRcvBuff->lVoidDiscount[5];	/* TAR191625 */
        break;

	//CLASS_CHARGETIPX must enter here to get information needed for Chargetip
	//to work properly ItemDiscGetItemizerSub needs to be called to get the info
	//This reflects the original way to charge tips addtional logic is provided below
	//on CLASS_CHARGETIP in order to accommodate SR#6
	case CLASS_CHARGETIP://RPH 3/13/3
	case CLASS_CHARGETIP2://RPH 3/13/3
	case CLASS_CHARGETIP3://RPH 3/13/3
    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
        if ((sReturnStatus = ItemDiscGetItemizerSub()) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
        *plItemizer = ItemizerRcvBuff->lMI;  /* get main itemizer */
        if (ItemCommonTaxSystem() == ITM_TAX_US) {                  /* V3.3 */
			USHORT  i;

			for (i = 0; i < 3; i++) {       /* tax1, tax2, tax3 */
                *plItemizer -= ItemizerRcvBuff->Itemizers.TranUsItemizers.lTaxItemizer[i];  /* clcurated tax amount */
            }
        }
        *plItemizer *= lSign;               /* sign by preselect void */
		*plVoidDiscount = 0;
        break;
                        
    default:
        break;
    }

	//SR#6 allow override of charge tips with zero itemizer
    /* prohibit negative itemizer */
    if (pItemDisc->uchMinorClass != CLASS_ITEMDISC1) {
		/*allow charge tips for negative itemizer*/
		switch(pItemDisc->uchMinorClass){
		case CLASS_CHARGETIP:
		case CLASS_CHARGETIP2:
		case CLASS_CHARGETIP3:
			if(!CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT3)){
				if (*plItemizer <= 0 && (pItemDisc->fbDiscModifier & RETURNS_TRANINDIC) == 0) {
					return(LDT_PROHBT_ADR);
				}
			}
			break;
		default:
			if (*plItemizer <= 0 && (TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) == 0) {
				NHPOS_ASSERT_TEXT((*plItemizer > 0L), "==PROVISION: No items in transaction are discountable.");
				return(LDT_PROHBT_ADR);
			}
		}
	}

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetItemizerSeat(ITEMDISC *pDisc)
*
*    Input: ITEMDISC *pDisc
*   Output: None
*    InOut: None
**Return: ITM_SUCCESS     : Function Performed succesfully
*
** Description: Get Seat/Base Transaction in SplitA.                V3.3
*===========================================================================
*/
SHORT   ItemDiscGetItemizerSeat(ITEMDISC *pDisc)
{
    if (pDisc->uchSeatNo == 'B') {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO);
        return(ITM_SEAT_DISC);
    } else if (pDisc->uchSeatNo != 0) {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        TrnSplGetSeatTrans(pDisc->uchSeatNo, TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA);
        return(ITM_SEAT_DISC);
    }

    return(ITM_SUCCESS);
}


/*----- End of Source -----*/
