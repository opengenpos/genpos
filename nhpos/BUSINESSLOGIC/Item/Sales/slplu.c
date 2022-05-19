/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : PLU sales module                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLPLU.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesPLU() : PLU Item sales module main   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun- 1-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 5-92: 00.00.01  : K.Maeda   : Code inspection held on Jun. 3                                   
* Feb-28-95: 03.00.00  : hkato     : R3.0
** GenPOS Rel 2.2 **
* Jun-21-16: 02.02.01  : R.Chambers : Bypass ITEMSALES checks if NULL in ItemSalesSalesRestriction().
* Jun-23-16: 02.02.01  : R.Chambers : modify ItemSalesSalesRestriction() to allow restrict flags
* Jun-19-18: 02.02.02  : R.Chambers : add function ItemCheckAndSetTransDate() to ensure valid date for sales restriction
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
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "fsc.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "appllog.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "mld.h"
#include    "itmlocal.h"
#include    "eeptl.h"
#include	"rfl.h"	
#include	"trans.h"
#include	"pifmain.h"

USHORT  ItemSalesGetUniqueId (VOID)
{
	// We start off the unique id counter at 3 in order to provide room
	// for special unique ids such as for CLASS_ITEMORDERDEC or order declaration items
	static USHORT ItemSalesGlobalUniqueID = 3;

	if (ItemSalesGlobalUniqueID > 65000)
		ItemSalesGlobalUniqueID = 3;
	return ItemSalesGlobalUniqueID++;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPLU(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: UIFREGSALES *pItemSales
*
*   Output: 
*
*    InOut: None
*
**Return: Return status returned from sub module 
*
** Description: This module is the main module of Sales module.
*               This module executes the following functions.
*
*  Notes      :  There are a few special things about PLU numbers.
*
*                1) there is a range of special PLU numbers that are designed
*                   to not print and to not display in order to trigger other
*                   application behavior.  See function RflSpecialPluCheck()
*                   and use of MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH. 
*===========================================================================
*/
SHORT   ItemSalesPLU(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT   sReturnStatus;
    SHORT   sSavedReturn;
	SHORT	giftPos = 0, voidOption = 0;
    USHORT  usLinkNo = 0;
    UCHAR   uchRestrict = 0, uchTable = 0;

	sSavedReturn = ITM_SUCCESS;     /* initialize */

    /* --- initialize condiment group no R3.1 */
    if (!(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)) {
        memset(&ItemSalesLocal.auchOepGroupNo, 0, SALES_NUM_GROUP);
    }

	// The pItemSales->blPrecedence is used for Precedence data.
	// If it is set to TRUE, then there is no need to prepare 
	//  the pItemSales because a prepared one is already sent down
	// ***PDINU
	// Use the item data to set up the necessary fields in case ITM_PRECEDENCE_APPLIED for PLU_CTRL_COUPON set
	uchRestrict = 0;
	usLinkNo = pItemSales->usLinkNo;
	uchTable = pItemSales->uchTableNumber;
	if ( ! (pItemSales->blPrecedence & ITM_PRECEDENCE_APPLIED) )
	{
		if ((sReturnStatus = ItemSalesPrepare(pUifRegSales, pItemSales, &uchRestrict, &usLinkNo, &uchTable)) != ITM_SUCCESS) {
			return(sReturnStatus);
		}
	}

	{
		ULONG  fsCurStatus2 = 0;

		//For Quality Restriction  ***PDINU
		if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_NOT_ALLOW_QR))
		{
			fsCurStatus2 |= PLU_NOT_ALLOW_QR;
		}
		
		//For a PLU that requires a quantity  ***PDINU
		if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_REQUIRE_QR))
		{
			fsCurStatus2 |= PLU_REQUIRE_QR;
		}

		//Quantity Restriction  ***PDINU
		if ((fsCurStatus2 & PLU_NOT_ALLOW_QR) && pItemSales->lQTY > MIN_QTY_ALLOWED )    //Not Allowed
		{
			return(LDT_NOT_ALLOW_QR); 
		}

		//Require Quantity  ***PDINU
		if ((fsCurStatus2 & PLU_REQUIRE_QR) && (pItemSales->lQTY <= MIN_QTY_ALLOWED) )    //Quantity required
		{
			return(LDT_REQUIRE_QR); 
		}

		if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT) &&
			(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD) == 0)
		{
			return(LDT_PROHBT_ADR); 
		}
	}

	{
		TRANCURQUAL    *pWorkCur = TrnGetCurQualPtr();

   		// allow a transaction tender of 0.00 if the PLU allows it and
		// no other PLUs NOT allowing it have been rung up.  Condition is checked at Tender to
		// determine if Supervisor Intervention required or not.
		// PDINU
		if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_ALLOW_0_TEND) == 0)
		{
			pWorkCur->fsCurStatus2 |= CURQUAL_NOTALLOW_0_TEND;
		}
			
		if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
			(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
		{
			pWorkCur->fsCurStatus2 |= CURQUAL_REQUIRE_NON_GIFT;
		}
	}
    
	if (pUifRegSales->uchMinorClass != CLASS_UIDISPENSER) { /* skip restriction process at beverage dispenser */
        /* ----- check sales restriction, 2172 ----- */
        if (ItemSalesSalesRestriction(pItemSales, uchRestrict) != ITM_SUCCESS) {
            return (LDT_SALESRESTRICTED);
        }
           
        /* ----- check birthday entry,  2172  ----- */
        if ((sReturnStatus = ItemSalesCheckBirth(pItemSales)) != ITM_SUCCESS) {
           return (sReturnStatus);
        }
    }

    if ((sReturnStatus = ItemSalesCommon(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if (!(pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
        /*---  CHECK PRINT MOD. OR COMDIMENT COMPULSORY STATUS ---*/
        if ((sReturnStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
            return(sReturnStatus);
        }

		if(pItemSales->uchMinorClass == CLASS_ITEMORDERDEC)
		{
			// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
			// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
			// uncompressed portion of the record, TRANSTORAGESALESNON.
			//    Richard Chambers, Dec-21-2018
			pItemSales->ControlCode.uchItemType |= PRESET_PLU;
			pItemSales->usDeptNo = pItemSales->uchAdjective = pUifRegSales->uchFsc;  // uchAdjective is reused as order declare number, CLASS_ITEMORDERDEC
			pItemSales->usUniqueID = 1; //unique id for CLASS_ITEMORDERDEC order declaration items
		}else
		{
			pItemSales->usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ
			pItemSales->usKdsLineNo = 0;                     //reset KDS line number so that a new one will be assigned by ItemSendKdsItemGetLineNumber()

			switch(pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) {
			case PLU_NON_ADJ_TYP:
				sReturnStatus = ItemSalesNonAdj(pUifRegSales);
				break;

			case PLU_ADJ_TYP:
				sReturnStatus = ItemSalesAdj(pUifRegSales, pItemSales);
				break;

			default:
				sReturnStatus = ItemSalesDeptOpen(pUifRegSales, pItemSales);
				break;
			}
		}
    } else {
        /* condiment process */
        sReturnStatus = ItemSalesCondiment(pUifRegSales, pItemSales);
    }

    if (sReturnStatus != ITM_SUCCESS) {
        if (sReturnStatus != ITM_COMP_ADJ) {
            return(sReturnStatus);    
        }
        sSavedReturn = LDT_SEQERR_ADR;
    }  

	// check promotional PLU table, AC116, to see if this PLU has
	// any promotional items associated with it.
    sReturnStatus = ItemSalesSetMenu(pUifRegSales, pItemSales);
    if (sReturnStatus == ITM_CANCEL) {                         
        if (sSavedReturn != ITM_SUCCESS) {                     
            return(sSavedReturn);                              
        }                                                      
        
        /* ----- link plu, 2172 ----- */
        sReturnStatus = ItemSalesLinkPLU(pItemSales, 1); /* check link plu is acceptable */
        if (sReturnStatus != ITM_SUCCESS) {
			char  xBuff[128];
			sprintf (xBuff, "==PROVISION: ItemSalesPLU() PLU Link to %d failed %d.", pItemSales->usLinkNo, sReturnStatus);
			NHPOS_ASSERT_TEXT ((sReturnStatus == ITM_SUCCESS), xBuff);
            return (LDT_PROHBT_ADR);
        }

    } else if (sReturnStatus != ITM_SUCCESS) {
		// promotional PLU check found an error during processing the AC116 Promotional PLU
		// data so return with the error.
        return(sReturnStatus);
    }

    /* --- Decrease No. of Communication in case of OEP,    Dec/13/2000 --- */
    if (uchTable != 0) {
        sReturnStatus = ItemSalesOEP(pUifRegSales, pItemSales); /* R3.0 */
    } else {
        sReturnStatus = ITM_CONT;
    }

    /* --- Decrease No. of Communication in case of OEP,    Dec/13/2000 --- */
    if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
        MldDeletePopUp();
        ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
    }
    if (sReturnStatus == ITM_CONT) {
        if (sSavedReturn != ITM_SUCCESS) {
            return(sSavedReturn);
        }
    } else {
        return(sReturnStatus);
    }

    if ((sReturnStatus = ItemSalesModifier(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

	//SR 143 @/For key by cwunn
	if (pUifRegSales->usFor){ // Prevent sales calculation on @/For operation (already calculated)
		pItemSales->usFor = pUifRegSales->usFor;
		pItemSales->usForQty = pUifRegSales->usForQty;
		pItemSales->lUnitPrice = pUifRegSales->lUnitPrice;
		pItemSales->lProduct = pUifRegSales->lUnitPrice;
	
		pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;  // do not allow @/For PLU items to consolidate
	}
	else {
		if ((sReturnStatus = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
			return(sReturnStatus);
		}
	}

	if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
		pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;  // do not allow scalable PLU items to consolidate
	}

	if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) ||
		(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
	{
		// ensure that the quantity for this payment card PLU is 1.  any other value is illegal.
		if (pItemSales->lQTY != 1000 && pItemSales->lQTY != -1000) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Gift Card Qty is invalid.");
			return LDT_NOT_ALLOW_QR;
		}
	}
    
    /* price change, 2172 */
    if ((sReturnStatus = ItemSalesPriceChange(pItemSales)) == ITM_CANCEL) {
        return(UIF_CANCEL);
    } else if (sReturnStatus == ITM_PRICECHANGE) {
        /* recalculation by changed unit price */
        if (pItemSales->lProduct == 0L) {
            if ((sReturnStatus = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }
    } else if (sReturnStatus != ITM_SUCCESS) {
        return (sReturnStatus);
    }

	if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
		(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
	{	
		ITEMSALESGIFTCARD	WorkGiftCard = {0};
		//Gift cards can only be voided by the Cursor Void
		if (pItemSales->fbModifier == VOID_MODIFIER)
			return LDT_PROHBT_ADR;

		//checking for a Gift Card when doing a Transactions Void
		if (!(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS))
		{
			if(TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)){
				return LDT_PROHBT_ADR;
			}
		}
		if (TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK){
			pItemSales->lProduct *= -1;	
			pItemSales->lQTY *= -1;
		}	

		pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;
	
		//Checking to see if the gift card is to be used for FreedomPay
		WorkGiftCard.GiftCard = DetermineGiftCardItem(pItemSales);

		//obtain index of first open gift card slot
		giftPos = GetNextGCSlot();
		if (giftPos >= NUM_SEAT_CARD)
			return LDT_PROHBT_ADR;

		TrnGetTransactionInvoiceNum (WorkGiftCard.refno.auchReferncNo);

		//obtain gift card nummber and/or track2 data
		if ((sReturnStatus = GetGiftCardAccount2(&WorkGiftCard)) != ITM_SUCCESS) 
			return (sReturnStatus);

		// conditionals to determine whether the gift card is of type
		// issue, void issue, reload or void reload.
		if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD)  ||
			(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
		{
			if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD)
			{
				if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
				{	
					if (WorkGiftCard.GiftCard != IS_GIFT_CARD_FREEDOMPAY)
						if ((sReturnStatus = ItemSalesRefEntry(WorkGiftCard.refno.auchReferncNo) != ITM_SUCCESS) )
							return (UIF_CANCEL);
					
					WorkGiftCard.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
					_RflStrncpyUchar2Tchar (pItemSales->aszNumber[1], WorkGiftCard.refno.auchReferncNo, NUM_NUMBER);
				} else
					WorkGiftCard.GiftCardOperation = GIFT_CARD_RELOAD;
			} else {
				if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
				{
					if (WorkGiftCard.GiftCard != IS_GIFT_CARD_FREEDOMPAY)
						if ((sReturnStatus = ItemSalesRefEntry(WorkGiftCard.refno.auchReferncNo) != ITM_SUCCESS) )
							return (UIF_CANCEL);
					
					WorkGiftCard.GiftCardOperation = GIFT_CARD_VOID;
					_RflStrncpyUchar2Tchar (pItemSales->aszNumber[1], WorkGiftCard.refno.auchReferncNo, NUM_NUMBER);
				} else
					WorkGiftCard.GiftCardOperation = GIFT_CARD_ISSUE;
			}
		}

		// copy gift card and sales information to and from pItemSales
		WorkGiftCard.auchPluStatus = pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7];
		WorkGiftCard.aszPrice = pItemSales->lUnitPrice;
		if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
		{
			WorkGiftCard.aszPrice *= -1L;
		}
		_tcsncpy(pItemSales->aszNumber[0], WorkGiftCard.aszNumber, NUM_NUMBER);
		WorkGiftCard.usUniqueID = pItemSales->usUniqueID;
		WorkGiftCard.Used = GIFT_CARD_NOT_USED;
		// Once the Gift Number has been obtained we want to mask
		// the number that will be displayed for pre auth.
		GiftPreAuthMaskAcct(pItemSales);

		/* ----- set link plu, 2172 ----- */
		ItemSalesLinkPLU(pItemSales, 0);

		if ((sReturnStatus =ItemSalesCommonIF(pUifRegSales, pItemSales)) == ITM_SUCCESS) {
			/* --- initialize condiment group no R3.1 */
			if (!(pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
				memset(&ItemSalesLocal.auchOepGroupNo, 0, SALES_NUM_GROUP);
			}
		}
		// Save working gift card info.
		SaveGiftCard(&WorkGiftCard, giftPos);
	} else
	{
		/* ----- set link plu, 2172 ----- */
		ItemSalesLinkPLU(pItemSales, 0);
    
		if ((sReturnStatus =ItemSalesCommonIF(pUifRegSales, pItemSales)) == ITM_SUCCESS) {
			/* --- initialize condiment group no R3.1 */
			if (!(pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
				memset(&ItemSalesLocal.auchOepGroupNo, 0, SALES_NUM_GROUP);
			}
		}
	}

	/* ----- send link plu to kds, 2172 ----- */
    ItemSalesLinkPLU(pItemSales, 2);
        
    return(sReturnStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesSalesRestriction(ITEMSALES *pItemSales,
*                                             UCHAR uchRestrict);
*
*   Input:  ITEMSALES   *pItemSales
*           UCHAR       uchRestrict : Sales Code or restriction address
*
*   Output: None
*
*   InOut:  None
*
**Return: ITM_SUCCESS : PLU is not the sales restriction item
*         ITM_CONT    : PLU is the sales restriction item but Super overide allowed
*         ITM_CANCEL  : PLU is the sales restriction item
*
** Description: This function checkes the sales item or coupon whether it is a sales
*               restriction item or not. Sales Code Restriction rules are set in the
*               AC170 Sales Code Restriction table. Sales Code value is assigned to the
*               PLU using AC68 PLU Maintenance or to a Coupon using AC161 Coupon Maintenace.
*
*               The argument uchRestrict contains the Sales Code of either a PLU
*               or a coupon. In the case of PLUs the decision is made to either
*               restrict the sale of the item or not. However in the case of a
*               coupon the uchRestrict value is composed of two parts:
*                 - Sales Code in the lower nibble same as for PLU
*                 - flags in the upper nibble
*
*               We allow for the following flags and return values:
*                - 0x80 - Prompt for Supervisor Intervention if restricted (return ITEM_CONT)
*
*===========================================================================
*/
SHORT   ItemSalesSalesRestriction(ITEMSALES *pItemSales, UCHAR uchRestrict)
{
	PARARESTRICTION RestrictTbl = {0};
    UCHAR           uchDate, uchDay, uchHour, uchMin;
	UCHAR           uchRestrictSuper = (uchRestrict & 0x80);

	uchRestrict &= 0x0f;   // ensure that any flags are removed.

	if (pItemSales) {
		if (pItemSales->fbModifier & VOID_MODIFIER) {
			return (ITM_SUCCESS);
		}

		if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
			if (ItemCommonLocalPtr->ItemSales.fbModifier & VOID_MODIFIER) {
				return (ITM_SUCCESS);
			}
		}

		/*--- PRESELECT VOID ---*/
		if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) { /* preselect void trans. */
			return (ITM_SUCCESS);
		}
	}

    if (! uchRestrict) {                    /* not exist code */
        return (ITM_SUCCESS);                           /* no restriction */
    }
    if (uchRestrict > ITM_SL_RESTRICT_ANYTIME) {
        return (ITM_SUCCESS);                           /* illegal data */
    }

	do {   // one time only, multiple breaks to bottom of loop for error return indicating Restriction.
		UCHAR  uchRestrictuchDate;
		UCHAR  uchRestrictOverride;

		if (uchRestrict == ITM_SL_RESTRICT_ANYTIME) {
			break;                        /* anytime restrict */
		}

		RestrictTbl.uchMajorClass = CLASS_PARARESTRICTION;
		RestrictTbl.uchAddress    = uchRestrict;
		CliParaRead(&RestrictTbl);              /* get sales restriction table */

		uchRestrictuchDate = (RestrictTbl.uchDate & ~(RESTRICT_OVERRIDE_ANDOR | RESTRICT_OVERRIDE_AND));
		uchRestrictOverride = RestrictTbl.uchDate & (RESTRICT_OVERRIDE_ANDOR | RESTRICT_OVERRIDE_AND);

		// Make sure we have a valid transaction start date in the case of
		// this being the first item rung up and it happens to have a Sales Code
		// that triggers Sales Restrictions.
		ItemCheckAndSetTransDate();
		uchDate = (UCHAR)ItemSalesLocal.Tod.usMDay;      /* set date as day of month, 1 thru 31 */
		uchDay  = (UCHAR)(ItemSalesLocal.Tod.usWDay + 1);/* set day of week*/
		uchHour = (UCHAR)ItemSalesLocal.Tod.usHour;      /* set hour */
		uchMin  = (UCHAR)ItemSalesLocal.Tod.usMin;       /* set minute */

		/* 7/10  R2.1 time check */
		// Check two conditions to determine if we are doing an AND operation. Is the MDC set indicating AND
		// or is there an override specified indicating AND.
		if ((CliParaMDCCheck(MDC_PLU5_ADR,EVEN_MDC_BIT1) && uchRestrictOverride == 0)  || (uchRestrictOverride & RESTRICT_OVERRIDE_AND)) {
			// we are doing a Sales Restriction AND meaning that we check
			// both Day of Month and Time or Day of Week and Time.

			// if no Day of Month or Day of Week specified then we check hour range only.
			if (uchRestrictuchDate == 0) {
				uchDate = 0;   // if no Day of Month restriction then zero out Day of Month Now
			}        
			if (RestrictTbl.uchDay == 0) {
				uchDay = 0;   // if no Day of Week restriction then zero out Day of Week Now
			}

			if (uchRestrictuchDate != uchDate) {
				// not restricted for this day of the month, 1 thru 31.
				return(ITM_SUCCESS);                         /* no restriction */
			}

			if ((RestrictTbl.uchDay & RESTRICT_WEEK_DAYS_ON) == 0) {
				// We are looking at match against a single week day only
				if (RestrictTbl.uchDay != uchDay) {
					return(ITM_SUCCESS);                         /* no restriction */
				}
			} else {
				// We are looking at a match against several week days. Determine if
				// if one of the week days specified match with today.
				int kBit = 0x01;

				kBit <<= ItemSalesLocal.Tod.usWDay;  // may be shift none if Sunday so use original 0 - 6.
				if ((RestrictTbl.uchDay & kBit) == 0) {
					return(ITM_SUCCESS);                         /* no restriction */
				}
			}

			// If we reach here then we have matched either the Day of Month or Day of Week.
			// Next we check the time of day range.

			if ((RestrictTbl.uchHour1 == 0) || (RestrictTbl.uchHour2 == 0)) {
				// if time range is not specified then we will match against day only so Restricted.
				break;                          /* restriction */
			}
		} else {
			// we are doing a Sales Restriction OR meaning that we check
			// either Day of Month or Day of Week or Time.

		   if (uchRestrictuchDate == uchDate) {               /* the same date */
			  break;                            /* restriction */
		   }
			if ((RestrictTbl.uchDay & RESTRICT_WEEK_DAYS_ON) == 0) {
				// We are looking at match against a single week day only
				if (RestrictTbl.uchDay == uchDay) {         /* the same day of week */
					break;                            /* restriction */
				}
			} else {
				// We are looking at a match against several week days. Determine if
				// if one of the week days specified match with today.
				int kBit = 0x01;

				kBit <<= ItemSalesLocal.Tod.usWDay;  // may be shift none if Sunday so use original 0 - 6.
				if ((RestrictTbl.uchDay & kBit) != 0) {
					break;                            /* restriction */
				}
			}

			// If we reach here then this is not a match for Day of Month or Day of Week.
			// Now we check for time range to see if restricted due to time range.

			if ((RestrictTbl.uchHour1 == 0) || (RestrictTbl.uchHour2 == 0)) {
				// time range not specified so not restricted.
			   return (ITM_SUCCESS);                           /* ignore table */
		   }
		}

		if (RestrictTbl.uchHour1 < RestrictTbl.uchHour2) {  /* Time1 < Time2 */
			if ((RestrictTbl.uchHour1 > uchHour) || (RestrictTbl.uchHour2 < uchHour)) {
				return (ITM_SUCCESS);                       /* no restriction */
			}
			if (RestrictTbl.uchHour1 == uchHour) {  /* ? hour == start hour */
				if (RestrictTbl.uchMin1 > uchMin) {     /* ? min < start min */
					return (ITM_SUCCESS);               /* no restriction */
				}
			}
			if (RestrictTbl.uchHour2 == uchHour) {  /* ? hour == end hour */
				if (RestrictTbl.uchMin2 < uchMin) {     /* ? min > end min */
					return (ITM_SUCCESS);               /* no restriction */
				}
			}
		}
		else if (RestrictTbl.uchHour1 > RestrictTbl.uchHour2) {  /* Time1 > Time2 */
			// happens if we have a wrap around midnight as in 11PM to 6AM
			if ((RestrictTbl.uchHour1 > uchHour) && (RestrictTbl.uchHour2 < uchHour)) {
				return (ITM_SUCCESS);                       /* no restriction */
			}
			if (RestrictTbl.uchHour1 == uchHour) {  /* ? hour == start hour */
				if (RestrictTbl.uchMin1 > uchMin) {     /* ? min < start min */
					return (ITM_SUCCESS);               /* no restriction */
				}
			}
			if (RestrictTbl.uchHour2 == uchHour) {  /* ? hour == end hour */
				if (RestrictTbl.uchMin2 < uchMin) {     /* ? min > end min */
					return (ITM_SUCCESS);               /* no restriction */
				}
			}
		}

		/* R2.1   Time Check  */
		if (RestrictTbl.uchHour1 == RestrictTbl.uchHour2) { /* Time1 = Time2 */
			if (RestrictTbl.uchHour1 == uchHour) {
				if (RestrictTbl.uchMin1 < RestrictTbl.uchMin2) {
					if ((RestrictTbl.uchMin1 > uchMin) || (RestrictTbl.uchMin2 < uchMin)) {
						return (ITM_SUCCESS);
					}
				}
				if (RestrictTbl.uchMin1 > RestrictTbl.uchMin2) {
					if ((RestrictTbl.uchMin1 > uchMin) && (RestrictTbl.uchMin2 < uchMin)) {
						return (ITM_SUCCESS);
					}
				}
				if ((RestrictTbl.uchMin1 == RestrictTbl.uchMin2) && (RestrictTbl.uchMin1 != uchMin)) {
					return (ITM_SUCCESS);
				}
			}       
			else {
				return(ITM_SUCCESS);
			} 
		}
	} while (0);  // one time only through the loop body.
	
	if (uchRestrictSuper) {
		return (ITM_CONT);         /* restriction but Supervisor Intervention Allowed */
	}

    return (ITM_CANCEL);           /* restriction with no override allowed */
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCheckBirth(ITEMSALES *pItemSales, ITEMPRINT *pItemPrint)
*
*   Input:  ITEMSALES   *pItemSales
*
*   Output: None
*
*   InOut:  None
*
**Return: ITM_SUCCESS : custom age >= boundary age
*         LDT_PROHBT_ADR  : custom age < boundary age
*
** Description: This function checkes birthday entry item           V2.1
*===========================================================================
*/
SHORT   ItemSalesCheckBirth(ITEMSALES *pItemSales)
{
	SHORT       sStatus = ITM_SUCCESS;

    /*----- check Dept, PLU status are Boundary Age or Age Type -----*/
    if ( pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_BIRTHDAY_CONTROL_M ) {
		PARABOUNDAGE BoundAge = {0};

		/* initial */
		BoundAge.uchMajorClass = CLASS_PARABOUNDAGE;
		switch (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_BIRTHDAY_CONTROL_M) {
			case PLU_BIRTHDAY_CONTROL1:
				BoundAge.uchAddress = ITM_AGE_TYPE1;
				break;
			case PLU_BIRTHDAY_CONTROL2:
				BoundAge.uchAddress = ITM_AGE_TYPE2;
				break;
			case PLU_BIRTHDAY_CONTROL3:
				BoundAge.uchAddress = ITM_AGE_TYPE3;
				break;
			default:
				return sStatus;
		}
		CliParaRead(&BoundAge);         /* get boundary age */

		if (BoundAge.uchAgePara == 0) { /*  no limit */
			return (ITM_SUCCESS);
		}

       if (ItemSalesLocal.uchAge == 0) {                    /*  first Boundary age entry ? */
			DATE_TIME   DT = {0};
			USHORT      usControl;
			UCHAR       uchStatus = 1;

			/*----- display lead-thru message and get birth day -----*/
            /*----- beep the tone -----*/
            PifBeep(ITM_BEEP);
            usControl = UieNotonFile(UIE_ENABLE);           /* disable using scanner */

            /* Check Birthday */
            PifGetDateTime(&DT);

            while(uchStatus){
				ULONG     ulBirthDay = 0;
				USHORT    usBYear = 0;
				UCHAR     uchBDay = 0, uchBMonth = 0;
				CHAR      chAge = 0;

                sStatus = ItemSalesGetBirthDay(&ulBirthDay);
                if (sStatus == ITM_CANCEL) {
                    break;
                } else if (sStatus != ITM_SUCCESS) {
                    UieErrorMsg( sStatus, UIE_WITHOUT );    /* compulsory entry */
                    continue;
                }
                if(ulBirthDay == 0){
                    ItemSalesLocal.uchAge = 0;
                    sStatus = (ITM_SUCCESS);
					break;
                }

                /* set birthday */
                uchBDay   = (UCHAR)(ulBirthDay & 0x000000FF);
                uchBMonth = (UCHAR)((ulBirthDay >> 8) & 0x000000FF);
                usBYear   = (USHORT)((ulBirthDay >> 16) & 0x0000FFFF);

                if(( uchBDay < 1 ) || ( uchBDay > 31 )){
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT );
                    continue;
                } else if(( uchBMonth < 1 ) || ( uchBMonth > 12 )){
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT );
                    continue;
                } else if( usBYear > DT.usYear){
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT );
                    continue;
                }

                /* get customer age */
                chAge = ItemSalesCalAge(uchBDay, uchBMonth, usBYear);
                if( chAge <= 0 ){
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT );
                    continue;
                } else {
                    uchStatus = ITM_SUCCESS;
                    ItemSalesLocal.uchAge = (UCHAR)chAge;

                    if (BoundAge.uchAgePara <= ItemSalesLocal.uchAge) {
						ITEMPRINT   ItemPrint = {0};
						TCHAR        buf[8] = {0};
                        /* make birthday date  */
                        ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
                        ItemPrint.uchMinorClass = CLASS_AGELOG;
                        ItemPrint.fsPrintStatus = PRT_JOURNAL;
                        ItemPrint.aszNumber[0][0] = _T(' '); //US Customs
                        buf[0] = (TCHAR)(uchBMonth / 10 + 0x30);
                        buf[1] = (TCHAR)(uchBMonth % 10 + 0x30);
                        _tcsncpy(&ItemPrint.aszNumber[0][1], buf, 2);
                        ItemPrint.aszNumber[0][3] = '/'; //US Customs
                        buf[0] = (TCHAR)(uchBDay / 10 + 0x30);
                        buf[1] = (TCHAR)(uchBDay % 10 + 0x30);
                        _tcsncpy(&ItemPrint.aszNumber[0][4], buf, 2); //US Customs
                        ItemPrint.aszNumber[0][6] = '/'; //US Customs
                        buf[0] = (TCHAR)(usBYear / 1000 + 0x30);
                        buf[1] = (TCHAR)((usBYear / 100) % 10 + 0x30);
                        buf[2] = (TCHAR)((usBYear / 10) % 10 + 0x30);
                        buf[3] = (TCHAR)(usBYear % 10 + 0x30);
                        _tcsncpy(&ItemPrint.aszNumber[0][7], buf, 4); //US Customs
                        ItemPrint.aszNumber[0][11] = _T(' '); //US Customs
                        
						sStatus = ItemCommonSetupTransEnviron ();
                        if (sStatus != ITM_SUCCESS) {
                        	break;
                        }

                        /* send data to transaction module */           
                        if ( (sStatus = ItemPreviousItem( &ItemPrint, 0) ) != ITM_SUCCESS ) {
                            break;
                        }
                        /* send to enhanced kds, 2172 */
                        sStatus = ItemSendKds(&ItemPrint, 0);
                        if (sStatus != ITM_SUCCESS) {
                            break;
                        }
                   }
                }
            }

    		UieNotonFile(usControl);                  /* enable scanner */
        }

        if (BoundAge.uchAgePara > ItemSalesLocal.uchAge) {
            sStatus = (LDT_SALESRESTRICTED);
        }
    }
    
    return (sStatus);
}

/*
*===========================================================================
**Synopsis: CHAR   ItemCommonCalAge(UCHAR uchBirthDay,UCHAR uchBirthMonth,
*                                    UCHAR uchBirthYear)
*
*    Input: UCHAR uchBirthDay
*           UCHAR uchBirthMonth
*           USHORT usBirthYear
*   Output: None
*    InOut: None
*
**Return: : CHAR chAge
*
** Description: cluculate customer age
*
*===========================================================================
*/
CHAR   ItemSalesCalAge(UCHAR uchBthDay,UCHAR uchBthMonth,USHORT usBthYear)
{
	DATE_TIME   DT = {0};
    USHORT      usTODYear;
    CHAR        chAge;
    UCHAR       uchTODDay, uchTODMonth;

    /* get TOD */
    PifGetDateTime(&DT);
    uchTODDay   = (UCHAR)DT.usMDay;          /* set date */
    uchTODMonth = (UCHAR)DT.usMonth;         /* set month */
    usTODYear   = DT.usYear;                 /* set year */

    /* calculate customer age */
    chAge = (CHAR)(usTODYear - usBthYear);

    if( uchTODMonth < uchBthMonth){
        chAge -= 1;
    } else if (( uchTODMonth == uchBthMonth ) && ( uchTODDay < uchBthDay )){
        chAge -= 1;
    }

    return(chAge);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesGetBirthDay(ULONG *pulBirthDay);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  ULONG *pulBirthDay 
*
**Return: ITM_SUCCESS : Function Performed succesfully
*         ITM_CANCEL  : Function Canceled by user using cancel key
*
** Description: This function requests a birthday to be entered by the cashier
*               and returns the entered data.  The data entered is a numeric
*               string containing month (2 digits), day (2 digits), and year (4 digits)
*               however how the string is parsed depends on the settings of
*               MDC 14 Bit B.
*               The value returned is a normalized encoding of the YYYYMMDD as follows:
*                - Year in most significant 16 bits as a USHORT, bits 16 - 31
*                - Month in most significant byte of lower 16 bits as a UCHAR, bits 8 - 15
*                - Day in least significant byte of lower 16 as as UCHAR, bits 0 - 7
*===========================================================================
*/
SHORT   ItemSalesGetBirthDay(ULONG *pulBirthDay)
{
    UIFDIADATA      WorkUI = {0};
	REGDISPMSG      DispMsg = {0};

    *pulBirthDay = 0;
    /* ----- display "ENTER DEPT" ----- */
    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_BIRTHDAY;
    RflGetLead (DispMsg.aszMnemonic, LDT_BDAY_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&DispMsg);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

	UieEchoBack(UIE_ECHO_DATE_ENTRY,UIFREG_MAX_INPUT);

    if (UifDiaGuestNo(&WorkUI)) {               /* req. price  */
		UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_INPUT);     /* set no echo back */
        return (ITM_CANCEL);                 /* cancel by user */
    }

	UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);/* set no echo back */ /*UIE_ECHO_NO -> UIE_ECHO_ROW0_REG, SR 1037 - CSMALL*/

    if (WorkUI.auchFsc[0] != FSC_SCANNER) {  // allow either FSC_NUM_TYPE or FSC_SCANNER
        return (LDT_SEQERR_ADR);
    }

    if (WorkUI.uchTrack2Len) {  /* allow without amount entry */
        if (WorkUI.uchTrack2Len < UIFREG_MAX_DIGIT7) {  /* allow only MM/DD/YYYY format */
            return (LDT_KEYOVER_ADR);
        }
        if (WorkUI.uchTrack2Len > UIFREG_MAX_DIGIT8) {  /* allow only MM/DD/YYYY format */
            return (LDT_KEYOVER_ADR);
        }
    }

	// Normalize the birthday entered to a YYYMMDD format to make it easy to parse out.
    if (CliParaMDCCheckField(MDC_DRAWER_ADR, MDC_PARAM_BIT_B)) {    // ItemSalesGetBirthDay()
		// format of date entered is expected to be DDMMYYYY
		USHORT  usBYear;
		UCHAR   uchBDay, uchBMonth;
        uchBDay   = (UCHAR)(WorkUI.ulData / 10000L / 100L);
        uchBMonth = (UCHAR)(WorkUI.ulData / 10000L % 100L);
        usBYear   = (USHORT)(WorkUI.ulData % 10000L);
		*pulBirthDay = (usBYear << 16) | (uchBMonth << 8) | uchBDay;
    } else {
		// format of date entered is expected to be MMDDYYYY
		USHORT  usBYear;
		UCHAR   uchBDay, uchBMonth;
        uchBMonth = (UCHAR)(WorkUI.ulData / 10000L / 100L);
        uchBDay   = (UCHAR)(WorkUI.ulData / 10000L % 100L);
        usBYear   = (USHORT)(WorkUI.ulData % 10000L);
		*pulBirthDay = (usBYear << 16) | (uchBMonth << 8) | uchBDay;
    }

    return(ITM_SUCCESS);
    
}

/*
*===========================================================================
**Synopsis: SHORT    ItemSalesCheckWIC(ITEMSALES *pItem)
*
*    Input: ITEMSALES *pItemSales
*   Output: Nothing
*    InOut: Nothing
**Return:   UIF_SUCCESS :   Function success this process.
*
** Description: Check non WIC Item                              Saratoga
*===========================================================================
*/
SHORT   ItemSalesCheckWIC(ITEMSALES *pItem)
{
    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
		/* ----- WIC transaction so check the item entered ----- */
        if ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_WIC_ITEM) == 0) {  // WIC transaction; is it a non-WIC item?
			/* USHORT          usControl; */
			REGDISPMSG      Disp = {0};
            Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
            Disp.uchMinorClass = CLASS_REGDISP_WICMOD;
            RflGetLead (Disp.aszMnemonic, LDT_NONWIC_ADR);

            flDispRegDescrControl |= COMPLSRY_CNTRL;
            flDispRegKeepControl |= COMPLSRY_CNTRL;
            Disp.fbSaveControl = 4;
            DispWrite(&Disp);
            flDispRegDescrControl &= ~COMPLSRY_CNTRL;
            flDispRegKeepControl &= ~COMPLSRY_CNTRL;

            PifBeep(ITM_BEEP);
            /* remove NotOnFile function from WIC, 10/04/01 */
            /* usControl = UieNotonFile(UIE_ENABLE);           / disable using scanner */

            if (UifDiaWICMod() != UIF_SUCCESS) {    /* Need WIC Mod Key */
                /* UieNotonFile(usControl);                  / enable scanner */
                ItemOtherClear();
                return (UIF_CANCEL);
            }
            pItem->fbModifier |= WIC_MODIFIER;
        }
    }

    return (ITM_SUCCESS);
}


SHORT	GetNextGCSlot()
{
	TRANGCFQUAL	 *WorkGCF = TrnGetGCFQualPtr();
	SHORT        index;

	for (index = 0; index <= NUM_SEAT_CARD; index++){
		if(WorkGCF->TrnGiftCard[index].GiftCard == IS_GIFT_CARD_IS_NOT)
			break;

	}
	return index;
}

/*
*===========================================================================
** Synopsis:    SHORT	GetGiftCardAccount2(ITEMSALESGIFTCARD *pWorkGift)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Get Reference Number
*===========================================================================
*/
SHORT	GetGiftCardAccount2(ITEMSALESGIFTCARD *pWorkGift)
{
	ITEMTENDER	    ItemTender = {0};
	SHORT			sPinPadRet;
	UCHAR	        auchFsc;

	sPinPadRet = GetGiftCardAccount(pWorkGift->GiftCard, &ItemTender, &auchFsc);

	if(pWorkGift->GiftCard == IS_GIFT_CARD_FREEDOMPAY){
		if(sPinPadRet != UIF_CANCEL) {
			if (auchFsc == FSC_VIRTUALKEYEVENT) {
				RflDecryptByteString ((UCHAR *)(ItemTender.auchMSRData), sizeof(ItemTender.auchMSRData));
				memcpy(pWorkGift->aszTrack2, ItemTender.auchMSRData, sizeof(pWorkGift->aszTrack2));
				RflEncryptByteString ((UCHAR *)(ItemTender.auchMSRData), sizeof(ItemTender.auchMSRData));
				sPinPadRet = UIF_SUCCESS;
			} else {
				sPinPadRet = LDT_PROHBT_ADR;
			}
		}
	} else {
		if(sPinPadRet != UIF_CANCEL) {
			if(sPinPadRet == UIF_SUCCESS || sPinPadRet == UIF_PINPADSUCCESS) {
				sPinPadRet = LDT_PROHBT_ADR;
				if(auchFsc == FSC_NUM_TYPE) {
					//saves the card number gift card
					RflDecryptByteString ((UCHAR *)(ItemTender.aszNumber), sizeof(ItemTender.aszNumber));
					memcpy(pWorkGift->aszNumber, ItemTender.aszNumber, sizeof(pWorkGift->aszNumber));
					RflEncryptByteString ((UCHAR *)(ItemTender.aszNumber), sizeof(ItemTender.aszNumber));
					sPinPadRet = UIF_SUCCESS;
				} else if (auchFsc == FSC_MSR || auchFsc == FSC_PINPAD) {
					RflDecryptByteString ((UCHAR *)(ItemTender.auchMSRData), sizeof(ItemTender.auchMSRData));
					if (ItemTender.auchMSRData[0] != 0) {
						SHORT	i;
						for(i = 0; i < NUM_MSRDATA; i++)
						{
							if(ItemTender.auchMSRData[i] == ITM_MSR_SEPA || ItemTender.auchMSRData[i] == 0)
							{
								_tcsncpy(pWorkGift->aszTrack2, ItemTender.auchMSRData, NUM_MSRDATA);
								_tcsncpy(pWorkGift->aszNumber, ItemTender.auchMSRData, i);
								break;
							}
						}
					} else {
						RflDecryptByteString ((UCHAR *)(ItemTender.aszNumber), sizeof(ItemTender.aszNumber));
						memcpy(pWorkGift->aszNumber, ItemTender.aszNumber, sizeof(pWorkGift->aszNumber));
						RflEncryptByteString ((UCHAR *)(ItemTender.aszNumber), sizeof(ItemTender.aszNumber));
					}
					RflEncryptByteString ((UCHAR *)(ItemTender.auchMSRData), sizeof(ItemTender.auchMSRData));
					sPinPadRet = UIF_SUCCESS;
				}
			} else {
				sPinPadRet = LDT_PROHBT_ADR;
			}
		}
	}
	memset (&ItemTender, 0, sizeof(ItemTender));  // clear the buffer per PABN requirements
	return sPinPadRet;
}

/*
*===========================================================================
** Synopsis:    SHORT	SaveGiftCard(ITEMSALESGIFTCARD *pWorkGift, SHORT giftPos)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Get Reference Number
*===========================================================================
*/
SHORT	SaveGiftCard(ITEMSALESGIFTCARD *pWorkGift, SHORT giftPos)
{
	TRANGCFQUAL *WorkGCF = TrnGetGCFQualPtr();
	SHORT       sRetValue = ITM_SUCCESS;

	// Encrypt the gift card track2 data and/or the gift card number
	if(pWorkGift->aszTrack2[0] != 0 && pWorkGift->GiftCard != IS_GIFT_CARD_FREEDOMPAY)
	{
		RflEncryptByteString ((UCHAR *)&(pWorkGift->aszNumber[0]), sizeof(pWorkGift->aszNumber));
		RflEncryptByteString ((UCHAR *)&(pWorkGift->aszTrack2[0]), sizeof(pWorkGift->aszTrack2));
	}else
	{
		RflEncryptByteString ((UCHAR *)&(pWorkGift->aszNumber[0]), sizeof(pWorkGift->aszNumber));
	}

	// store the working gift card data into the index 'giftPos'
	NHPOS_ASSERT(giftPos < sizeof(WorkGCF->TrnGiftCard)/sizeof(WorkGCF->TrnGiftCard[0]));
	if (giftPos < sizeof(WorkGCF->TrnGiftCard)/sizeof(WorkGCF->TrnGiftCard[0])) {
		WorkGCF->TrnGiftCard[giftPos] = *pWorkGift;
	} else {
		NHPOS_ASSERT_TEXT((giftPos < 1), "**ERROR:  giftPos out of range for WorkGCF->TrnGiftCard.");
		sRetValue = LDT_PROHBT_ADR;
	}

	return sRetValue;
}
/*
*===========================================================================
** Synopsis:    SHORT   ItemSalesReferenceEntry(ITEMTENDER *pItem)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Get Reference Number
*===========================================================================
*/
SHORT   ItemSalesRefEntry( UCHAR RefNo[NUM_REFERENCE])
{
	SHORT           sReturn;
	UIFDIADATA      Dia = {0};
    REGDISPMSG      Disp = {0};

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, LDT_REFERENCENUM_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	//Gets Ref. No.
	sReturn = UifDiaCP6(&Dia, NUM_REFERENCE, UIFREG_NUMERIC_INPUT);
	if (sReturn == UIF_SUCCESS) {
		if (Dia.auchFsc[0] == FSC_NUM_TYPE) {
			int i = 0;
			for (i = 0; i < (NUM_REFERENCE - 1) && (RefNo[i] = Dia.aszMnemonics[i]); i++);
			RefNo[i] = 0;
		}
		else if (Dia.auchFsc[0] == FSC_SCANNER) {
			int i = 0;
			for (i = 0; i < (NUM_REFERENCE - 1) && (RefNo[i] = Dia.aszMnemonics[i]); i++);
			RefNo[i] = 0;
		}
		return(UIF_SUCCESS);
	} else if (sReturn == UIFREG_ABORT) {
		Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
		DispWrite(&Disp);

		memset(&Disp, 0, sizeof(REGDISPMSG));
		Disp.uchMajorClass = CLASS_UIFREGOTHER;
		Disp.uchMinorClass = CLASS_UICLEAR;
		DispWrite(&Disp);
		return(UIF_CANCEL);
	}
	return(UIF_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   DetermineGiftCardItem(ITEMTENDER *pItem)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Check for FreedomPay device and plu # FSC_KEYED_PLU or FSC_PLU
*
*               DetermineGiftCardItem():  TENDERKEY_TRANTYPE_FREEDOMPAY
*
*               This function should only be called if the following control codes are
*               set in the item PLU information which indicate that this is a gift card
*               type of PLU for selling or reloading a gift card.
*
*  	if((pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
*		(pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD))
*===========================================================================
*/
static  LONG SlPluConvert (TCHAR  *tcsPluNumber)
{
	LONG  lPluNum = 0;

	if (_tcsncmp (tcsPluNumber, _T("00000"), 5) == 0) {
		TCHAR tcsTempPlu[24] = {0};

		_tcsncpy (tcsTempPlu, tcsPluNumber, 13);
		lPluNum = _tstol (tcsTempPlu);
	}

	return lPluNum;
}

SHORT	DetermineGiftCardItem(CONST ITEMSALES *pItemSales)
{
	LONG	lPluNum = 0;
	SHORT   sGiftCard = IS_GIFT_CARD_IS_NOT;

	if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) {
		sGiftCard = IS_GIFT_CARD_GENERIC;   // assume this is a generic prepaid or gift card.  Other checks below.
	}
	else if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT) {
		sGiftCard = IS_GIFT_CARD_EPAYMENT;   // assume this is a generic prepaid or gift card.  Other checks below.
	}

	lPluNum = SlPluConvert (pItemSales->auchPLUNo);
	if ((lPluNum >= ITM_PLU_FREEDOMPAY_MIN) && (lPluNum <= ITM_PLU_FREEDOMPAY_MAX))
	{
		USHORT             addr;
		CONST SYSCONFIG    *pSysConfig = PifSysConfig();

		// The PLU number is one that is assigned to a FreedomPay type of prepaid or gift card.
		//
		// searches through the tender numbers to find which one is a prepaid and uses that 
		// tender number for minor class.
		for(addr = 1; addr <= STD_TENDER_MAX; addr++){
			PARATENDERKEYINFO  TendKeyInfo = {0};
			TendKeyInfo.uchAddress = (UCHAR) addr;
			ParaTendInfoRead(&TendKeyInfo);

			// check to see if this tender key is assigned to a Prepaid transaction type.
			// if it is then we will determine if we should use this tender key information by
			// looking to see if the tender request is a FreedomPay request or just a standard
			// gift card request.
			// If the tender key is assigned to the FreedomPay action code and the tender request is a
			// FreedomPay tender, then we will use this tender key information to process
			// the tender request.
			// Otherwise, we will look for the first tender key that is setup to
			// process a prepaid (gift card) tender and use that.
			if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID ){
				int   iEptDll = TendKeyInfo.TenderKeyInfo.auchEptTend;
				if (iEptDll >= '0' && iEptDll <= '9') {
					iEptDll -= '0';  // change to binary value for index into pSysConfig->EPT_ENH_Info[]
				}
				else {
					iEptDll = 0;     // default is zero or the first pSysConfig->EPT_ENH_Info[] entry
				}

				if (pSysConfig->EPT_ENH_Info[iEptDll].ulDevice == DEVICE_EPT_EX_DLL1) {
					sGiftCard = IS_GIFT_CARD_FREEDOMPAY;  // function isFreedomPayGiftCard()
					break;
				}
			}
		}
	}

	return sGiftCard;
}
/****** end of file ******/