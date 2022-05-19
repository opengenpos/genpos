/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T Corporation, E&M OISO        **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Coupon Module
* Category    : Item Coupon Module, AT&T 2170 US Hospitality Model Application
* Program Name: ITCOUPON.C
* --------------------------------------------------------------------------
* Abstract: ItemCoupon()
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver. Rev. :   Name    : Description
* Feb-23-95: 03.00.00  :   hkato   : R3.0
* Nov-11-95: 03.00.01  :   hkato   : R3.1
* Apr-04-96: 03.01.02  : T.Nakahata: reset target offset at CURSOR-VOID-CPN
* Jun-24-96: 03.01.09  : M.Ozawa   : allow coupon handling even if no target item is defined
* Jun-09-98: 03.03.00  : M.Ozawa   : change transaction open to common routine
* Aug-19-98: 03.03.00  :  hrkato   : V3.3 (VAT/Service)
*
** NCR2171 **
* Aug-26-99: 01.00.00  : M.Teraki  : initial (for 2171)
*
** GenPOS **
* Jun-10-15: 02.02.01  : R.Chambers  : moved define AUT_PRI_MAX for Auto Coupons to file csop.h, now CPN_AUTO_PRI_MAX
* Jun-10-15: 02.02.01  : R.Chambers  : source cleanup, localized variables, using defined constants, use const global pointers.
* Jun-23-15: 02.02.01  : R.Chambers  : implement Sales Code restriction for Auto Coupons.
* Feb-19-18: 02.02.02  : R.Chambers  : if BlFwIfSetOEPGroups(BL_OEPGROUP_STR_AUTOCOUPON) fails, issue log, return success
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
#include    <malloc.h>
#include    <string.h>
#include    <stdlib.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "uie.h"
#include	"uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "csgcs.h"
/* #include    <csstbgcf.h> */
#include    "display.h"
#include    "mld.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"
#include	"trans.h"
#include	"..\Opepara\csopin.h"
#include    "bl.h"
#include    "fsc.h"
#include    <rfl.h>


static SHORT   ItemCouponCheck(VOID);
static SHORT   ItemCouponRepeatCheck(UIFREGCOUPON *pUifRegCoupon);
static SHORT   ItemCouponGetTable(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch);
static SHORT   ItemCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch);
static SHORT   ItemSingleCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch); /* R3.1 */
static SHORT   ItemCouponSPVInt(UIFREGCOUPON *pUifRegCoupon);
static VOID    ItemCouponDisplay(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon);
static VOID    ItemCouponSetData(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon);
static VOID    ItemCouponEnd(UIFREGCOUPON *pUifRegCoupon);
static int     ItemAutoCouponFillPopUpList(ItemPopUpList *popUpList, SHORT sTotalKeyType /* from ItemTotalType() */);


DCURRENCY		couponAmount;
/* --- Saratoga --- */


static SHORT  ItemFetchItemFromStorage (ITEMSALES *pItemSales, USHORT  usItemOffset)
{
    SHORT     sReturn;
	SHORT     hsStorage;
    USHORT    usStorageVli;
    USHORT    usStorageHdrSize;

	memset (pItemSales, 0, sizeof(ITEMSALES));

	sReturn = TrnStoGetStorageInfo( TrnDetermineStorageType(), &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn == TRN_SUCCESS) {
		UCHAR      auchTranStorageWork[ TRN_WORKBUFFER ];
		USHORT     usTargetItemSize;

 		sReturn = TrnStoIsItemInWorkBuff(hsStorage, usItemOffset, &auchTranStorageWork[0], sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}
		if (RflGetStorageItemRequire( CLASS_ITEMSALES, pItemSales, auchTranStorageWork, RFL_WITH_MNEM ) == 0) {
			NHPOS_ASSERT_TEXT(!"==WARNING", "==WARNING: ItemFetchItemFromStorage() called with usItemOffset not a CLASS_ITEMSALES.");
			sReturn = TRN_ERROR;
		}
    }

    return ( sReturn );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCoupon(UIFREGCOUPON *pUifRegCoupon)
*
*    Input: UIFREGDISC *pUifRegDisc
*   Output: None
*    InOut: None
** Return:
** Description:
*===========================================================================
*/
SHORT   ItemCoupon(UIFREGCOUPON  *pUifRegCoupon)
{
    ITEMCOMMONLOCAL    *WorkCommon = ItemCommonGetLocalPointer();
    SHORT		       sStatus, sSign = -1;
	ITEMCOUPON         ItemCoupon = {0};
    ITEMCOUPONSEARCH   TrnSearch = {0};
	UIFREGSALES        UifRegSales = {0};
	ITEMTRANSOPEN      TransOpenData = {0};

    /*----- Check -----*/
    if ((sStatus = ItemCouponCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*----- Check Repeat -----*/
    if (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT) {
        if ((sStatus = ItemCouponRepeatCheck(pUifRegCoupon)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    /*----- Get Couopn Table -----*/
    if (pUifRegCoupon->uchMinorClass == CLASS_UIUPCCOUPON) {
		// check to see if we should not allow a scanned in coupon per request from VCS.
		if (CliParaMDCCheckField(MDC_UPC1_ADR, MDC_PARAM_BIT_B) ) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: UPC Coupon scanned but MDC_UPC1_ADR (371) bit B Prohibits.");
			return(LDT_PROHBT_ADR);
		}

        /* UPC Coupon */
        if ((sStatus = ItemCouponUPCPrepare(pUifRegCoupon, &ItemCoupon, &TrnSearch)) != ITM_SUCCESS) {
            return(sStatus);
        }
    } else {
        /* Combination Coupon */
        if ((sStatus = ItemCouponGetTable(pUifRegCoupon, &ItemCoupon, &TrnSearch)) != ITM_SUCCESS) {
			if (sStatus != ITM_CONT) {
				return sStatus;
			} else {
				// Allow for Supervisor Intervention if MDC blocks Return type {
				if (ItemSPVIntAlways(LDT_SUPINTR_ADR) != ITM_SUCCESS) {
					return (LDT_SALESRESTRICTED);                           /* Supervisor Intervention request not successful */
				}
			}
        }
    }

    /*----- Send Previous Item to Transaction Module -----*/
    if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMSALES ||
        WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMCOUPON ||
        WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMDISC) {
        ItemPreviousItem(NULL, 0);                  /* Saratoga */
    }

    /*----- Coupon Search -----*/
    if (pUifRegCoupon->uchMinorClass == CLASS_UIUPCCOUPON) {
        /* UPC Coupn Search, 2172 Saratoga */
        sStatus = ItemCouponUPCSearch(&ItemCoupon, &TrnSearch);
        if (sStatus != ITM_SUCCESS) {
            return(sStatus);
        }

        sStatus = ItemCouponUPCSetupStatus(&ItemCoupon, &TrnSearch);
        if (sStatus != ITM_SUCCESS) {
            return(sStatus);
        }
    } else {
        /* Combination Coupon Search */
        if (  RflIsSpecialPlu (TrnSearch.aSalesItem[0].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0 || TrnSearch.aSalesItem[0].usDeptNo) {
            sStatus = ItemCouponSearch(&ItemCoupon, &TrnSearch);
            if (sStatus != TRN_SUCCESS) {
                return(sStatus);
            }
        } else {
            /* check single coupon status */
            sStatus = ItemSingleCouponSearch(&ItemCoupon, &TrnSearch);
            if (sStatus != TRN_SUCCESS) {
                return(sStatus);
            }
        }
    }

	// SR47 Check if Double/Triple Coupon Functionality is On
	// If it is on, do the appropriate computation
	if (!(CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT0))){
		ItemCouponCheckMultiple(&ItemCoupon);
	}

	// The code below is for thh PPI and Combination Coupon Precedence data.
	// It basically checks to see if the items in the coupon also have a PPI code
	// assigned to them.  If there is a PPI code assigned, it modifies the 
	// transaction to adjust for the recalculation of the PPI price, then 
	// adds the item at full price.
	// ***PDINU
	{
		USHORT	   j;
		USHORT     usNoFoundItems = 0;

		for (j = 0; j < TrnSearch.uchNoOfItem; j++) {
			ITEMSALES  ItemSales = {0};

			if (ItemFetchItemFromStorage (&ItemSales, TrnSearch.aSalesItem[j].usItemOffset) != TRN_SUCCESS) continue;

			{
				int i;

				// Search the list of precedence items to determine if we should process this as a PPI
				// item or not.  If it is a PPI item then the next question is to determine if PPI has
				// been applied to this item or not.  If not, then no need to do the void procedure
				// to remove an item from the set of PPI items.
				for (i = 0; i < WorkCommon->usCountPrecedencePLU; i++) {
					if (memcmp (WorkCommon->PrecedenceData[i].auchPLUNo, ItemSales.auchPLUNo, sizeof(WorkCommon->PrecedenceData[i].auchPLUNo)) == 0) {
						break;
					}
				}
				if (i >= WorkCommon->usCountPrecedencePLU)
					continue;

				if ((WorkCommon->PrecedenceData[i].fPpiStatus & PRECEDENCEDATA_FLAG_PPI_APPLIED) == 0)
					continue;
			}

			if ((ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_CTRL_COUPON) && ItemSales.uchPPICode != 0) {
				ITEMSALES  ItemSalesTempPrecedence;

				// since this is a PPI item with a coupon and we have coupon precedence
				// turned on, we need to separate one of the items from the PPI set of items
				// and create a new item using it.  Then we will need to apply the coupon to
				// the new item we have created.
				ItemSales.blPrecedence |= ITM_PRECEDENCE_APPLIED;
				ItemSalesTempPrecedence = ItemSales;

				ItemSales.lQTY = 1000;
				ItemSalesCursorVoid(&ItemSales, 1);
//				ItemSales.lQTY = -1000;
//				ItemSales.fbModifier |= VOID_NO_MODIFY_QTY;
				TrnStorage (&ItemSales);

				MldScrollFileWrite(&ItemSales);  //write to the MLD file and make sure it works
				ItemSales = ItemSalesTempPrecedence;

				// Turn off the void modifiers so that we can create a new item.
				ItemSales.fbModifier &= ~(VOID_NO_MODIFY_QTY | VOID_MODIFIER);
				ItemSales.lQTY = 1000;
				ItemSales.sCouponQTY = 1;
				ItemSales.sCouponCo = 0;
				ItemSales.usItemOffset = 0;

				ItemSales.uchPPICode = 0;
				ItemSales.usUniqueID = ItemSalesGetUniqueId(); // new item so unique id for condiment editing
				ItemSales.usKdsLineNo = 0;                     // new item so new KDS line

				// ensure that the correct adjective will be used in the
				// adjective calculations if this item has an adjective.
				UifRegSales.uchAdjective = ItemSales.uchAdjective;

				sStatus = ItemSalesPLU(&UifRegSales, &ItemSales); 
				if (sStatus != ITM_SUCCESS) 
					return (sStatus);

				ItemPreviousItem(NULL, 0);

				// Update the coupon search information replacing the offset of the item that has PPI
				// with the replacement item we have created to implement the coupon.
				{
					USHORT i;

					for (i = 0; i < STD_MAX_NUM_PLUDEPT_CPN; i++) {
						if (ItemCoupon.usItemOffset[i] == TrnSearch.aSalesItem[j].usItemOffset) {
							ItemCoupon.usItemOffset[i] = TrnFetchLastStoConsOffset();
							break;
						}
					}
				}
				usNoFoundItems++;
			}
		}
	}

    /*----- Check Storage Size -----*/
    sStatus = ItemCommonCheckSize(&ItemCoupon, (USHORT)(WorkCommon->usSalesBuffSize + WorkCommon->usDiscBuffSize));
    if (sStatus < 0) {
		TRANGCFQUAL   *WorkGCF = TrnGetGCFQualPtr();

		WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;
        return(LDT_TAKETL_ADR);
    }

    /*----- Supervisor Intervention -----*/
    if ((sStatus = ItemCouponSPVInt(pUifRegCoupon)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*----- Transaction Open -----*/
    if ((sStatus = ItemCommonSetupTransEnviron ()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&ItemCoupon, 0);
    if (sStatus != ITM_SUCCESS) {
        return sStatus;
    }

	{
		USHORT   usReturnType;
		USHORT   usReasonCode = 0;
		ULONG    ulTrnQualModifierReturnData = 0;

		ulTrnQualModifierReturnData = TrnQualModifierReturnData (&usReturnType, &(usReasonCode));
		if (ulTrnQualModifierReturnData & CURQUAL_PRETURN) {
			switch (usReturnType) {
				case 1:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_1);
					ItemCoupon.fbReduceStatus |= REDUCE_RETURNED;
					break;

				case 2:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_2);
					ItemCoupon.fbReduceStatus |= REDUCE_RETURNED;
					break;

				case 3:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_3);
					ItemCoupon.fbReduceStatus |= REDUCE_RETURNED;
					break;
			}
		} else if (ulTrnQualModifierReturnData & CURQUAL_TRETURN) {
			ItemCoupon.fbReduceStatus |= REDUCE_RETURNED;
			switch (usReturnType) {
				case 1:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_1);
					break;

				case 2:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_2);
					break;

				case 3:
					ItemCoupon.fbModifier |= (RETURNS_MODIFIER_3);
					break;
			}
		}
	}

    /*----- Save Coupon Data to Common Local Data -----*/
    ItemCouponSetData(pUifRegCoupon, &ItemCoupon);

    /*----- Coupon Display -----*/
    ItemCouponDisplay(pUifRegCoupon, &ItemCoupon);

    /*----- Coupon Itemize End -----*/
    ItemCouponEnd(pUifRegCoupon);

	couponAmount = 0;

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponCheck(VOID)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemCouponCheck(VOID)
{
    SHORT   sStatus;

    /*----- check non-gc transaction V3.3 -----*/
    if ( (  sStatus =  ItemCommonCheckNonGCTrans() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

	if (TranCurQualPtr->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) {
		// check MDC to see if adding items to a Return Transaction is allowed
		if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_A) == 0)
			return LDT_PROHBT_ADR;
	}

	if (ItemSalesLocalPtr->fbSalesStatus & SALES_TRANSACTION_DISC)
		return LDT_PROHBT_ADR;

    /*--- Check Tax Exempt ---*/
    if ((sStatus = ItemCommonCheckExempt()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*--- Check Compulsory Operation ---*/
    if ((sStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*--- Check Split Key Status,   R3.1 ---*/
    if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*--- Check Split State,    R3.1 ---*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI) {
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponRepeatCheck(UIFREGCOUPON *pUifRegCoupon)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemCouponRepeatCheck(UIFREGCOUPON *pUifRegCoupon)
{
    /*--- Check Taxable Modifier ---*/
    if (ItemModLocalPtr->fsTaxable != 0) {
        return(LDT_SEQERR_ADR);
    }

    /*--- Check Void ---*/
    if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
        return(LDT_SEQERR_ADR);
    }

    /*----- Check Previous Item -----*/
	{
		ITEMCOUPON       *pItemCoupon;
		ITEMCOMMONLOCAL  *WorkCommon = ItemCommonGetLocalPointer();

		pItemCoupon = (ITEMCOUPON *)&(WorkCommon->ItemSales);
		if (pItemCoupon->uchMajorClass != CLASS_ITEMCOUPON) {
			// what is stored in the common area, if anything, is not a coupon.
			return(LDT_SEQERR_ADR);
		}
		if (pItemCoupon->uchMinorClass == CLASS_UPCCOUPON) {
			return(LDT_SEQERR_ADR);
		}
		if (pItemCoupon->fbModifier & VOID_MODIFIER) {
			return(LDT_SEQERR_ADR);
		}

		/*----- Set Coupon# -----*/
		pUifRegCoupon->uchFSC = pItemCoupon->uchCouponNo;
	}

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponGetTable(UIFREGCOUPON *pUifRegCoupon,
*                   ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:  Read the coupons whose number is specified by pUifRegCoupon->uchFSC
*                and build up the criteria for searching the transaction so as
*                to find out if this coupon is applicable to this transaction.
*
*                pTrnSearch->uchNoOfItem will indicate the number of item criteria
*                to be used in the search.  The criteria may be either a particular
*                PLU for the coupon to apply or it may be a department and several
*                PLUs from the same department may be used for the coupon.
*
*                The coupon provisioning information is done through AC161, Coupon Maintenance.
*===========================================================================
*/
static SHORT   ItemCouponGetTable(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
    USHORT          i;
    SHORT           sStatus, sSign = -1;
	SHORT           sRestrictStatus = ITM_SUCCESS;
    USHORT          usPLUDept;
    CPNIF           WorkCoupon = {0};

    /*--- Get Coupon Record ---*/
    WorkCoupon.uchCpnNo = pUifRegCoupon->uchFSC;
    if ((sStatus = CliOpCpnIndRead(&WorkCoupon, 0)) != OP_PERFORM) {
		PifLog (MODULE_OP_CPN, LOG_EVENT_OP_CPN_AUTO_GETTABLE_01);
        return(OpConvertError(sStatus));
    }

    if ((pUifRegCoupon->fbModifier & VOID_MODIFIER) == 0) {
		if ( (sRestrictStatus = ItemSalesSalesRestriction (NULL, WorkCoupon.ParaCpn.uchRestrict)) == ITM_CANCEL)
			return LDT_SALESRESTRICTED;
	}

    /*--- Data Class ---*/
    pTrnSearch->uchMajorClass = pItemCoupon->uchMajorClass = CLASS_ITEMCOUPON;
    pTrnSearch->uchMinorClass = pItemCoupon->uchMinorClass = CLASS_COMCOUPON;

    /*--- Coupon# ---*/
    pTrnSearch->uchCouponNo = pItemCoupon->uchCouponNo = (UCHAR)WorkCoupon.uchCpnNo;

    /*--- Coupon Amount ---*/
    if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
        sSign *= -1;
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        sSign *= -1;
    }
    pItemCoupon->lAmount = (DCURRENCY)WorkCoupon.ParaCpn.ulCouponAmount * sSign;

	pItemCoupon->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
	pItemCoupon->usBonusIndex = WorkCoupon.ParaCpn.usBonusIndex;

    /*--- Coupon Modifier(Taxable, Void) ---*/
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {              /* V3.3 */
        if (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT) {
			ITEMCOUPON      *pWorkCoupon = (ITEMCOUPON *)&(WorkCommon->ItemSales);

			NHPOS_ASSERT(pWorkCoupon->uchMajorClass == CLASS_ITEMCOUPON);

            pItemCoupon->fbModifier = pWorkCoupon->fbModifier;
            pTrnSearch->fbModifier = pWorkCoupon->fbModifier;
        } else {
			USHORT          uchBit;

			// check taxable bit indicators MOD_TAXABLE1 throug MOD_TAXABLE9 to determine Canadian tax index to use.
			// set the Canadian tax type index for the taxable indicators in the coupon's fbModifier
            for (uchBit = MOD_TAXABLE1, i = 1; uchBit <= MOD_TAXABLE9; uchBit <<= 1, i++) {
                if (ItemModLocalPtr->fsTaxable & uchBit) {
                    pItemCoupon->fbModifier = (USHORT)(i << 1);   // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
                    pTrnSearch->fbModifier  = (USHORT)(i << 1);   // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
					break;
                }
            }
            if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
                pItemCoupon->fbModifier |= VOID_MODIFIER;
                pTrnSearch->fbModifier |= VOID_MODIFIER;
            }
        }
        /*--- Status (Taxable) ---*/
        pItemCoupon->fbStatus[0] |= (WorkCoupon.ParaCpn.uchCouponStatus[0] & MOD_CANTAXMOD);
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {           /* V3.3 */
        if (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT) {
			ITEMCOUPON      *pWorkCoupon = (ITEMCOUPON *)&(WorkCommon->ItemSales);

			NHPOS_ASSERT(pWorkCoupon->uchMajorClass == CLASS_ITEMCOUPON);

            if (pWorkCoupon->fbModifier & TAX_MODIFIER1) {
                pItemCoupon->fbModifier |= TAX_MODIFIER1;
                pTrnSearch->fbModifier |= TAX_MODIFIER1;
            }
            if (pWorkCoupon->fbModifier & TAX_MODIFIER2) {
                pItemCoupon->fbModifier |= TAX_MODIFIER2;
                pTrnSearch->fbModifier |= TAX_MODIFIER2;
            }
            if (pWorkCoupon->fbModifier & TAX_MODIFIER3) {
                pItemCoupon->fbModifier |= TAX_MODIFIER3;
                pTrnSearch->fbModifier |= TAX_MODIFIER3;
            }
            if (pWorkCoupon->fbModifier & FOOD_MODIFIER) {  /* Saratoga */
                pItemCoupon->fbModifier |= FOOD_MODIFIER;
                pTrnSearch->fbModifier |= FOOD_MODIFIER;
            }
            /*--- Status (Taxable) ---*/
            pItemCoupon->fbStatus[0] |= ((WorkCoupon.ParaCpn.uchCouponStatus[0] & MOD_USTAXMOD) << TAX_MOD_SHIFT);  // tax itemizers bits shift to fit into TAX_MOD_MASK
            if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_FOOD_MOD) {
                pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
            }
            if (pWorkCoupon->fbModifier & FOOD_MODIFIER) {
                pItemCoupon->fbStatus[0] ^= FOOD_MODIFIER;
            }
        } else {
            if (ItemModLocalPtr->fsTaxable & MOD_TAXABLE1) {
                pItemCoupon->fbModifier |= TAX_MODIFIER1;
                pTrnSearch->fbModifier |= TAX_MODIFIER1;
            }
            if (ItemModLocalPtr->fsTaxable & MOD_TAXABLE2) {
                pItemCoupon->fbModifier |= TAX_MODIFIER2;
                pTrnSearch->fbModifier |= TAX_MODIFIER2;
            }
            if (ItemModLocalPtr->fsTaxable & MOD_TAXABLE3) {
                pItemCoupon->fbModifier |= TAX_MODIFIER3;
                pTrnSearch->fbModifier |= TAX_MODIFIER3;
            }
            if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
                pItemCoupon->fbModifier |= FOOD_MODIFIER;
                pTrnSearch->fbModifier |= FOOD_MODIFIER;
            }
            if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
                pItemCoupon->fbModifier |= VOID_MODIFIER;
                pTrnSearch->fbModifier |= VOID_MODIFIER;
            }
            /*--- Status (Taxable) ---*/
            pItemCoupon->fbStatus[0] |= ((WorkCoupon.ParaCpn.uchCouponStatus[0] & MOD_USTAXMOD) << TAX_MOD_SHIFT);  // tax itemizers bits shift to fit into TAX_MOD_MASK
            if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_FOOD_MOD) {
                pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
            }
        }
    } else {                                        /* Int'l VAT,   V3.3 */
        /*--- Status (Taxable) ---*/
        pItemCoupon->fbStatus[0] |= ((WorkCoupon.ParaCpn.uchCouponStatus[0] & MOD_VATTAXMOD) << TAX_MOD_SHIFT);  // tax itemizers bits shift to fit into TAX_MOD_MASK
        if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_AFFECT_DISC1) {
            pItemCoupon->fbStatus[0] |= ITM_COUPON_AFFECT_SERV;
        }
        if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
            pItemCoupon->fbModifier |= VOID_MODIFIER;
            pTrnSearch->fbModifier |= VOID_MODIFIER;
        }
    }

    /*--- Status(Taxable/Discountable) ---*/
    if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_AFFECT_DISC1) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC1;
    }
    if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_AFFECT_DISC2) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC2;
    }
    if (!CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT0)) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_HOURLY;
    }

    /*--- Coupon Mnemonic ---*/
    _tcsncpy(pItemCoupon->aszMnemonic, WorkCoupon.ParaCpn.aszCouponMnem, STD_CPNNAME_LEN);

    /*--- Number ---*/
    _tcsncpy(pItemCoupon->aszNumber, pUifRegCoupon->aszNumber, NUM_NUMBER);
    _tcsncpy(pTrnSearch->aszNumber, pUifRegCoupon->aszNumber, NUM_NUMBER);

	pTrnSearch->uchNoOfItem = 0;

    /*--- Coupon Target Dept/PLU# ---*/
    for (i = 0, usPLUDept = 0x01; i < STD_MAX_NUM_PLUDEPT_CPN; i++, usPLUDept <<= 1) 
	{
        /* 13 digit PLU No, 2172 */
        if ( RflIsSpecialPlu(WorkCoupon.ParaCpn.auchCouponTarget[i], MLD_NO_DISP_PLU_DUMMY) == 0) break;
		// Some value, PLU number or Department number, entered in this address so process it.
		{
            if (usPLUDept & WorkCoupon.ParaCpn.uchCouponStatus[1]) 
			{ /* target is dept so check if PLU number is lower or equal to MLD_NO_DISP_PLU_HIGH */
                if (RflIsSpecialPlu(WorkCoupon.ParaCpn.auchCouponTarget[i], MLD_NO_DISP_PLU_HIGH) <= 0) 
				{
					ULONG   ulDeptNo;
					TCHAR   auchPluNo[NUM_PLU_LEN + 1] = {0};

                    _tcsncpy(auchPluNo, &WorkCoupon.ParaCpn.auchCouponTarget[i][0], NUM_PLU_LEN);
                    ulDeptNo = _ttol(auchPluNo);   // convert PLU number string into a binary value
                    ulDeptNo /= 10L;               // remove trailing digit which is PLU number check sum from PLU label processing.
                    pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].usDeptNo = (USHORT)ulDeptNo;
                    pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].fbStatus |= ITM_COUPON_DEPT_REC;
                } else {
                    /* fixed at v1.0.04*/
                    if (_tcslen(&WorkCoupon.ParaCpn.auchCouponTarget[i][0]) <= 4) 
					{
						ULONG   ulDeptNo;

                        ulDeptNo = _ttol(&WorkCoupon.ParaCpn.auchCouponTarget[i][0]);
                        if (ulDeptNo) 
						{
                            pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].usDeptNo = (USHORT)ulDeptNo;
                            pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].fbStatus |= ITM_COUPON_DEPT_REC;
                        } else 
						{
                            break;
                        }
                    } else {
                        break;
                    }
                }
            } else 
			{
                _tcsncpy(pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].auchPLUNo, &WorkCoupon.ParaCpn.auchCouponTarget[i][0], NUM_PLU_LEN);
				pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].fbStatus &= (~ITM_COUPON_DEPT_REC);  // this is a PLU not a departmental coupon
            }
        }

        /*--- Adjective# ---*/
        pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].uchAdjective = (UCHAR)(WorkCoupon.ParaCpn.uchCouponAdj[i] & 0x0f);

        /*--- Group# ---*/
        pTrnSearch->aSalesItem[pTrnSearch->uchNoOfItem].uchGroup = (UCHAR)(WorkCoupon.ParaCpn.uchCouponAdj[i] >> 4);

        /*--- Number of Coupon Target Dept/PLU# ---*/
        pTrnSearch->uchNoOfItem++;
    }

    /*--- Single Type Coupon ---*/
    if (WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_SINGLE_TYPE) {
        pTrnSearch->fbStatus |= ITM_COUPON_SINGLE;
    }

    return(sRestrictStatus);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    USHORT  i;
    SHORT   sType = TrnDetermineStorageType();
    SHORT   sStatus;

    if ((sStatus = TrnCouponSearch(pTrnSearch, sType)) != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    /*--- Coupon Target Dept/PLU# Offset ---*/
	pItemCoupon->uchNoOfItem = 0;
    for (i = 0; i < pTrnSearch->uchNoOfItem; i++) {
        if (!(pTrnSearch->aSalesItem[i].fbStatus & ITM_COUPON_NOT_FOUND)) {
			// if there is no more room then we will stop where we are even if there are more possible.
			// this prevents GenPOS crash due to memory buffer overflow.
			if (pItemCoupon->uchNoOfItem >= STD_MAX_NUM_PLUDEPT_CPN) break;
            pItemCoupon->usItemOffset[pItemCoupon->uchNoOfItem] = pTrnSearch->aSalesItem[i].usItemOffset;
            pItemCoupon->uchNoOfItem++;
        }
    }

    /*--- Single Type Coupon ---*/
    if (pTrnSearch->fbStatus & ITM_COUPON_SINGLE) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_SINGLE_TYPE;
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponSearchAllCoupons(ITEMCOUPONSEARCH *pTrnSearch)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemCouponSearchAllCoupons(ITEMCOUPONSEARCH *pTrnSearch)
{
    SHORT   sType = TrnDetermineStorageType();
    SHORT   sStatus;

    if ((sStatus = TrnStoCouponSearchAll(pTrnSearch, sType)) != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    return(ITM_SUCCESS);
}
/*
*===========================================================================
**Synopsis: SHORT ItemSingleCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemSingleCouponSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    SHORT   sType = TrnDetermineStorageType();
    SHORT   sStatus;

    if ((sStatus = TrnSingleCouponSearch(pTrnSearch, sType)) != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    /*--- Single Type Coupon ---*/
    if (pTrnSearch->fbStatus & ITM_COUPON_SINGLE) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_SINGLE_TYPE;
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponSPVInt(UIFREGCOUPON *pUifRegCoupon)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static SHORT   ItemCouponSPVInt(UIFREGCOUPON *pUifRegCoupon)
{
    /*--- SPV Int by Coupon MDC ---*/
    if ((pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPON) ||
        (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT)) {

        if (CliParaMDCCheck(MDC_COMBCOUPON_ADR, ODD_MDC_BIT0) != 0) {
            return(ItemSPVInt(LDT_SUPINTR_ADR));
        }
    }

    /*--- SPV Int by Void MDC ---*/
    if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0) != 0 ) {
            return(ItemSPVInt(LDT_SUPINTR_ADR));
        }
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID    ItemCouponDisplay(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon)
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
**  Return:
*
** Description:
*===========================================================================
*/
static VOID    ItemCouponDisplay(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon)
{
	REGDISPMSG      Display = {0};

    Display.uchMajorClass = pUifRegCoupon->uchMajorClass;
    Display.uchMinorClass = pUifRegCoupon->uchMinorClass;

	//SR 47 shows on the display that the coupon has double or triple
	//amount
	if(couponAmount < 0)
	{
		_tcscat(pItemCoupon->aszMnemonic, _T("  DBL/TPL"));
	}

    _tcsncpy(Display.aszMnemonic, pItemCoupon->aszMnemonic, NUM_DEPTPLU);
    Display.lAmount = pItemCoupon->lAmount;
    Display.uchFsc = pItemCoupon->uchSeatNo;    /* R3.1 */

    flDispRegDescrControl |= ITEMDISC_CNTRL;
    if (pItemCoupon->fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;
    }
    if (pItemCoupon->fbModifier & TAX_MOD_MASK) {
        flDispRegDescrControl |= TAXMOD_CNTRL;
    }
    if (pItemCoupon->fbModifier & FOOD_MODIFIER) {  /* Saratoga */
        flDispRegDescrControl |= FOODSTAMP_CTL;
    }

    /*--- Set Repeat Counter ---*/
    if (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT) {
        uchDispRepeatCo++;
    } else {
        uchDispRepeatCo = 1;
    }

    /*--- Set Descriptor ---*/
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {
        flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL |
                                  TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    } else {
        flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL |
                                  TAXMOD_CNTRL | TAXEXMPT_CNTRL | SUBTTL_CNTRL);
    }

    DispWrite(&Display);

    /*--- Set Descriptor ---*/
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {
        flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                   TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL | FOODSTAMP_CTL);
    } else {
        flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                   TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL |
                                   SUBTTL_CNTRL | FOODSTAMP_CTL);
    }
}

/*
*===========================================================================
**Synopsis: VOID ItemCouponSetData(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:                                              R3.1
*===========================================================================
*/
static VOID    ItemCouponSetData(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon)
{
    /*--- Validation Print (Coupon or Void) ---*/
    if ((pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPON) || (pUifRegCoupon->uchMinorClass == CLASS_UICOMCOUPONREPEAT)) {

        if (CliParaMDCCheck(MDC_COMBCOUPON_ADR, ODD_MDC_BIT1) ||
			(CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1) && (pUifRegCoupon->fbModifier & VOID_MODIFIER)) ) {
            pItemCoupon->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(pItemCoupon);
        }
    } else {
        /* UPC Coupon */
        if (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1) && (pUifRegCoupon->fbModifier & VOID_MODIFIER)) {
            pItemCoupon->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(pItemCoupon);
        }
    }

    /*----- Print Status -----*/
    pItemCoupon->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    /*--- System is Postcheck or Store/Recall ---*/
/**** UPC coupon doesn't show the receipt of post receipt functio, TAR187493
    if (!(TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK)) {
        pItemCoupon->fbStorageStatus = NOT_CONSOLI_STORAGE;
    }
*****/

    /*----- Set Seat#,  R3.1 -----*/
    pItemCoupon->uchSeatNo = TranCurQualPtr->uchSeat;

    /*----- Save Coupon Data to Common Local Data -----*/
    ItemPreviousItem(pItemCoupon, 0);

    /*----- Display Coupon to LCD -----*/
    MldScrollWrite(pItemCoupon, MLD_NEW_ITEMIZE);
    ItemCommonDispSubTotal(pItemCoupon);
}

/*
*===========================================================================
**Synopsis: VOID ItemCouponEnd(UIFREGCOUPON *pUifRegCoupon)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description:
*===========================================================================
*/
static VOID    ItemCouponEnd(UIFREGCOUPON *pUifRegCoupon)
{
    ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();

    /*----- Cancel Exempt Key -----*/
    ItemCommonCancelExempt();

    /*----- Disable E/C -----*/
    if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
        ItemCommonPutStatus(COMMON_VOID_EC);
    }

    /*--- Clear Taxable Modifier ---*/
    pWorkMod->fsTaxable = 0;
    pWorkMod->fbModifierStatus &= ~MOD_FSMOD;             /* Saratoga */
}

/*
*===========================================================================
**Synopsis: VOID ItemCouponVoidDisp(ITEMCOUPON *pItemCoupon)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description: Display modified subtotal to the optional subtotal common window.
*===========================================================================
*/
VOID    ItemCouponVoidDisp(ITEMSALES *pItemSales, ITEMCOUPON *pItemCoupon)
{
    /*----- Display Coupon to LCD -----*/
    MldECScrollWrite(); /* clear parent plu */

    MldScrollWrite(pItemCoupon, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pItemCoupon);

    /* rediplay parent plu */
    MldScrollWrite(pItemSales, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pItemSales);

    ItemCommonDispSubTotal(NULL);    /* Saratoga, do not need to make adjustment to subtotal */
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCouponCursorVoid(ITEMCOUPON *pData);
*
*   Input:  ITEMCOUPON  *pData
*   Output: None
*    InOut: None
**Return:
*
** Description:  Cursor Void of Coupon.                      R3.1
*===========================================================================
*/
SHORT   ItemCouponCursorVoid(ITEMCOUPON *pItemCoupon)
{
    SHORT       sStatus;

    if (pItemCoupon->uchMinorClass == CLASS_COMCOUPON) {
        if (CliParaMDCCheck(MDC_COMBCOUPON_ADR, ODD_MDC_BIT0) || CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) {
            if ((sStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    } else {
        /* UPC Coupon */
        if (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) {
            if ((sStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    }

    pItemCoupon->lAmount *= -1L;
    pItemCoupon->fbModifier |= VOID_MODIFIER;

    /******************* bug fixed (4/4/96) ********************/

    /* --- reset linked sales item offset, because VOID-CPN has no link --- */
    memset(pItemCoupon->usItemOffset, 0x00, sizeof(pItemCoupon->usItemOffset));

    /******************* bug fixed (4/4/96) ********************/

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(pItemCoupon, 0);
    if (sStatus != ITM_SUCCESS) {
        return sStatus;
    }

	{
		REGDISPMSG  Disp = {0};

		Disp.uchMajorClass = CLASS_UIFREGCOUPON;
		Disp.uchMinorClass = CLASS_UICOMCOUPON;
		Disp.lAmount = pItemCoupon->lAmount;
		_tcsncpy(Disp.aszMnemonic, pItemCoupon->aszMnemonic, NUM_DEPTPLU);

		flDispRegDescrControl |= ITEMDISC_CNTRL;
		flDispRegDescrControl |= VOID_CNTRL;
		if (pItemCoupon->fbModifier & TAX_MOD_MASK) {
			flDispRegDescrControl |= TAXMOD_CNTRL;
		}
		if (pItemCoupon->fbModifier & FOOD_MODIFIER) {  /* Saratoga */
			flDispRegDescrControl |= FOODSTAMP_CTL;
		}
		flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | SUBTTL_CNTRL | FOODSTAMP_CTL);
		DispWrite(&Disp);
		flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL | FOODSTAMP_CTL);
	}

    MldScrollWrite (pItemCoupon, MLD_NEW_ITEMIZE);
    ItemCommonDispSubTotal (pItemCoupon);

    /*--- Validation Print (Coupon or Void) ---*/
    if (pItemCoupon->uchMinorClass == CLASS_COMCOUPON) {
        if (CliParaMDCCheck(MDC_COMBCOUPON_ADR, ODD_MDC_BIT1) || CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1)) {
            pItemCoupon->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(pItemCoupon);
            pItemCoupon->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        }
    } else {
        /* UPC Coupon */
        if (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1)) {
            pItemCoupon->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(pItemCoupon);
            pItemCoupon->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        }
    }

    ItemPreviousItem (pItemCoupon, 0);
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT ItemCouponCheckMultiple(ITEMCOUPON *ItemCoupon)
*
*   Input:  ITEMCOUPON  *pData
*   Output: None
*    InOut: None
**Return:
*SR 47 Double/Triple Coupon Functionality
** Description:  Check if the Double/Triple Coupon MDC bits are turned on,
*				 and do the proper computations if they are.             R3.1
*===========================================================================
*/
SHORT ItemCouponCheckMultiple(ITEMCOUPON *ItemCoupon)
{
	SHORT	sSign = -1;

	/*----- Double Coupon -----*/
	if(CliParaMDCCheck(MDC_DOUBLE1_ADR, EVEN_MDC_BIT3)){
		PARAMISCPARA	    ParaMiscPara;

		ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
		ParaMiscPara.uchAddress = MISC_CPNFACE2_ADR;
		CliParaRead(&ParaMiscPara);
		ItemCoupon->lDoubleAmount = ParaMiscPara.ulMiscPara * sSign;
		//Check AC128 #4 to see if coupon is over face value Limit
		if(ItemCoupon->lDoubleAmount <= ItemCoupon->lAmount)
		{
			LONG	maxPrice;

			ParaMiscPara.uchAddress = MISC_MAXCPNAMT_ADR;
			CliParaRead(&ParaMiscPara);
			maxPrice = ParaMiscPara.ulMiscPara * sSign;
			/*Check the maximum credit allowed for the Coupon AC128 #6*/
			ItemCoupon->lAmount *= 2;
			/*Perform the Double Coupon multiplication*/
			if(ItemCoupon->lAmount < maxPrice)		//if it is over the max price
			{
				ItemCoupon->lAmount = maxPrice;	//reduce the coupon back to original price JHHJ 12-8-03 needed to be set to
												//address six.
				return ITM_SUCCESS;
			}
			couponAmount = ItemCoupon->lAmount;		//external value for use in the transaction module
			ItemCoupon->fbStatus[1] = 0;		    //make coupon affect status default, because it has it's own options
			//MDC BIT 380 Effect Coupon to Hourly?
			if (!CliParaMDCCheck(MDC_DOUBLE1_ADR, EVEN_MDC_BIT0)) {
				ItemCoupon->fbStatus[1] |= ITM_COUPON_HOURLY;		//affect coupon to the hourly totals
			}
		}
	} else if(CliParaMDCCheck(MDC_TRIPLE1_ADR, EVEN_MDC_BIT3)){
		PARAMISCPARA	    ParaMiscPara;

		/*----- Triple Coupon -----*/
		ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
		ParaMiscPara.uchAddress = MISC_CPNFACE3_ADR;
		CliParaRead(&ParaMiscPara);
		ItemCoupon->lTripleAmount = ParaMiscPara.ulMiscPara * sSign;
		//Check AC128 #5 to see if coupon is over face value Limit
		if(ItemCoupon->lTripleAmount <= ItemCoupon->lAmount)
		{
			DUNITPRICE	maxPrice;

			ParaMiscPara.uchAddress = MISC_MAXCPNAMT_ADR;
			CliParaRead(&ParaMiscPara);
			maxPrice = (DUNITPRICE)ParaMiscPara.ulMiscPara * sSign;
			ItemCoupon->lAmount *=3;
			/*Check the maximum credit allowed for the Coupon AC128 #6*/
			if(ItemCoupon->lAmount < maxPrice)		//if it is over the max price defined
			{
				ItemCoupon->lAmount = maxPrice;	//reduce the coupon back to original price JHHJ 12-8-03 needed to be set to
												//address six.
				return ITM_SUCCESS;
			}
			couponAmount = ItemCoupon->lAmount;		//external value for use in the transaction module
			ItemCoupon->fbStatus[1] = 0;		    //make coupon affect status default, because it has it's own options
			//MDC BIT 382 Effect Coupon to Hourly?
			if (!CliParaMDCCheck(MDC_TRIPLE1_ADR, EVEN_MDC_BIT0)) {
				ItemCoupon->fbStatus[1] |= ITM_COUPON_HOURLY;	//affect coupon to the hourly totals
			}
		}
	}

	return ITM_SUCCESS;
}
/*
*===========================================================================
**Synopsis: SHORT ItemAutoCouponCheck(SHORT sTotalKeyType )
*
*   Input:  SHORT sTotalKeyType  - from ItemTotalType()
*   Output: None
*    InOut: None
**Return:
*Auto Combinational Coupon Checks and Functionality
*	Checks to see if Auto Combinational Coupons will be auto fill or pop-up
**		and performs the actions that are needed based on the options that
*		are set as per S.R. 832.
*
*===========================================================================
*/
SHORT ItemAutoCouponCheck(SHORT sTotalKeyType /* from ItemTotalType() */)
{
	DCURRENCY		  lTotalCouponAmount = 0;
	DCURRENCY		  lParaTotalAmount = 0;
	DCURRENCY         nTempTotalCouponAmount = 0;
	ULONG			  ulParaTotalNumber = 0;
	ITEMCOUPONSEARCH  TrnSearch = { 0 };
	SHORT             sRetStatus;
	USHORT			  numAutoCoupon = 0;
	USHORT			  usTranTotalCoupons = 0;  //number of coupons already in the transaction JHHJ
	USHORT            usItemListPriority[AUTO_CPN_NUM + 1];   // Used to hold the list of coupons by priority.  element 0 is unused.
	UIFDIADATA        UI = { 0 };

	pItemPopUpList    pItemList = 0;
	int               nCouponListCount = 0;

	// First of all, check to see if coupon stuff is allowed at
	// this point in the transaction.  If not, then there is nothing
	// else for us to do.
	if ((sRetStatus = ItemCouponCheck()) != ITM_SUCCESS)
	{
		return(sRetStatus);
	}

	// Lets fetch the global Auto Combination Coupon limits
	// First we fetch the maximum amount of coupons per transaction.
	// Then we fetch the maximum number of coupons per transaction
	{
		PARAAUTOCPN       ParaAutoCoupon = { 0 };

		ParaAutoCoupon.uchMajorClass = CLASS_PARAAUTOCPN;
		ParaAutoCoupon.uchAddress = 1;
		CliParaRead(&ParaAutoCoupon);
		lParaTotalAmount = ParaAutoCoupon.ulAmount;
	}

	{
		PARAAUTOCPN       ParaAutoCoupon = { 0 };

		ParaAutoCoupon.uchMajorClass = CLASS_PARAAUTOCPN;
		ParaAutoCoupon.uchAddress = 2;
		CliParaRead(&ParaAutoCoupon);
		ulParaTotalNumber = ParaAutoCoupon.ulAmount;
	}

	/*----- Send Previous Item to Transaction Module -----*/
	if (ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMSALES ||
		ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMCOUPON ||
		ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMDISC)
	{
		ItemPreviousItem(NULL, 0);
	}

	nCouponListCount = 0;
	pItemList = (pItemPopUpList)alloca((sizeof(usItemListPriority)/sizeof(usItemListPriority[0]) + 1) * sizeof(ItemPopUpList));
	NHPOS_ASSERT(pItemList);

	memset(pItemList, 0, (sizeof(usItemListPriority)/sizeof(usItemListPriority[0]) + 1) * sizeof(ItemPopUpList));
	memset (usItemListPriority, 0, sizeof(usItemListPriority));

	//we search to get the total number of coupons already in the transaction.
	//this will help this function add the correct amount of coupons, if
	//neccessary JHHJ
	memset(&TrnSearch, 0x00, sizeof(TrnSearch));
	sRetStatus = ItemCouponSearchAllCoupons(&TrnSearch);
    if (sRetStatus != ITM_SUCCESS)
	{
        return(sRetStatus);
    }

	//make sure were not already over the limit of coupons.
	if((TrnSearch.uchNoOfItem >= ulParaTotalNumber) && (ulParaTotalNumber))
	{
		return ITM_SUCCESS;
	}

	usTranTotalCoupons = TrnSearch.uchNoOfItem;

	//we need to make this value negative. JHHJ
	lTotalCouponAmount = (TrnSearch.lAmount *= -1);

	//update the number of coupons for use later on in the function.
	numAutoCoupon = usTranTotalCoupons;

	// We have two different types of Auto Combination Coupon functionality.
	// The first case is to automatically do the coupon selection.
	// The second case is to pop up an OEP type window with a list of the
	// possible coupons that can be used for the transaction.
	if (CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT1))
	{
		// Auto Fill Case

		// Check to see if we allow only one coupon or not.
		// If only one coupon, and we have already done a coupon,
		// then we will return immediately.
		if (0 == CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT3))
		{
			if (numAutoCoupon >= 1)
			{
				return (ITM_SUCCESS);
			}
		}

		// Lets generate a list of possible coupons to apply and then
		// apply the coupons.  We do this multiple times until we run
		// out of coupons that we can apply.

		// get the count of and a list of applicable coupons for the transaction.
		// we provide the total key type so that if this is from a food stamp total then
		// we will filter out any coupons marked NOT Affect Food Stamp Itemizer.
		nCouponListCount = ItemAutoCouponFillPopUpList(pItemList, sTotalKeyType);

		for ( ;; )
		{
			int       nCouponCount = 0;
			USHORT    usItemLoop = 0;

			for (usItemLoop = 0; usItemLoop < sizeof(usItemListPriority)/sizeof(usItemListPriority[0]) ; usItemLoop++ )
			{
				USHORT    usRecordNo = 0;

				for (usRecordNo = 0; usRecordNo < nCouponListCount ; usRecordNo++)
				{
					UIFREGCOUPON      UifRegCoupon = {0};

					if (pItemList[usRecordNo].usPriority != (usItemLoop+1))
						continue;
					
					if((lParaTotalAmount) && (lParaTotalAmount < lTotalCouponAmount + nTempTotalCouponAmount))
					{
						continue;
					}
					// Check if this is a single coupon only and if it has already been applied
					// to this transaction.  If so, then we skip it.
					if ((pItemList[usRecordNo].ucStatusBytes[0] & CPN_STATUS0_SINGLE_TYPE) &&
						(pItemList[usRecordNo].nCouponAppliedCount >= 1))
					{
						continue;
					}
					nCouponCount++;

					UifRegCoupon.uchFSC = pItemList[usRecordNo].ItemCoupon.uchCouponNo;

					nTempTotalCouponAmount = pItemList[usRecordNo].ItemCoupon.lAmount;

					//Infinite loop that will go through the transaction and apply
					//the specific prioritized coupon as many times as possible.
					for (sRetStatus = ITM_SUCCESS; sRetStatus == ITM_SUCCESS; )
					{
						// Do the global consistency checking to determine if we are
						// still within the bounds of total coupon amount.  We do the
						// check for meeting or exceeding the maximum number of coupons
						// allowed below as part of the pop up display logic.
						// We do amount check by fetching the coupon data and then using
						// it as part of our consistency checks.  If a particular coupon
						// will exceed the total amount of coupons allowed, skip it.
						if((lParaTotalAmount) && (lParaTotalAmount < lTotalCouponAmount + nTempTotalCouponAmount))
						{
							break;
						}

						// Check if this is a single coupon only and if it has already been applied
						// to this transaction.  If so, then we skip it.
						if ((pItemList[usRecordNo].ucStatusBytes[0] & CPN_STATUS0_SINGLE_TYPE) &&
							(pItemList[usRecordNo].nCouponAppliedCount >= 1))
						{
							break;
						}

						UifRegCoupon.uchMajorClass = CLASS_UIFREGCOUPON;
						UifRegCoupon.uchMinorClass = CLASS_UICOMCOUPON;

						sRetStatus = ItemCoupon(&UifRegCoupon);   /* Coupon Modele */

						if (sRetStatus == ITM_SUCCESS)
						{
							// Update the counters we use for global coupon consistency checking.
							// This check is so that the limits for total amount of all coupons applied
							// to the transaction as well as the total number of coupons that can be applied
							// to the transaction will not be exceeded.
							// We check if the total number of coupons applied has been met or exceeded.
							// Here is where we also accumulate the total coupon amounts for the check
							// in the logic above which creates the pop up window display with the list
							// of possible coupons.

							numAutoCoupon++;
							lTotalCouponAmount +=  nTempTotalCouponAmount;
							pItemList[usRecordNo].nCouponAppliedCount++;

							// Check to see if we allow only one coupon or not.
							// If only one coupon, then we will return immediately.
							if (0 == CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT3))
							{
								return(ITM_SUCCESS);
							}

							//we check to see if there is a value in the Total Number,
							//if not, we do not restrict the number of coupons to use JHHJ
							if ((ulParaTotalNumber) && (ulParaTotalNumber <= numAutoCoupon))
							{
								return(sRetStatus);
							}

							nCouponCount++;  // indicate that we have processed a coupon
						}
					}
				}
			}

			// We will repeat the above loop attempting to apply coupons until we
			// are unable to apply coupons any more.  When we reach the point
			// at which we no longer can apply coupons, then we break out.
			if (nCouponCount < 1 || sRetStatus != ITM_SUCCESS)
			{
				break;
			}
		}

		return(ITM_SUCCESS);

/********************************************/
	}
	else
	{
		MLDPOPORDER_VARY  *pDisp, *pDispTemp;
		TRANCURQUAL       CurQualRcvBuff;

		// Check to see if we allow only one coupon or not.
		// If only one coupon, and we have already done a coupon,
		// then we will return immediately.
		if (0 == CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT2))
		{
			if (numAutoCoupon >= 1)
			{
				return (ITM_SUCCESS);
			}
		}

		TrnGetCurQual(&CurQualRcvBuff);
		// Populate an OEP window with possible coupon selections that
		// will fit the transaction and allow the operator to pick the
		// appropriate coupons to apply.

		// The basic logic here is to keep popping up coupon windows to
		// allow the operator to pick coupons until the operator presses
		// the Done button.  Each time through the loop, we will create
		// the OEP display window with the number and types of coupons
		// which could possibly apply to this transaction.

		pDisp = (MLDPOPORDER_VARY *) alloca (sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
		NHPOS_ASSERT(pDisp);

		pDispTemp = (MLDPOPORDER_VARY *) alloca (sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
		NHPOS_ASSERT(pDispTemp);

		for(;;)
		{
			int      nCouponCount = 0;
			USHORT	 usPriorityLoop;
			USHORT   usItemLoop = 0;

			memset(pDisp, 0, sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
			pDisp->uchMajorClass = CLASS_POPUP;
			pDisp->uchMinorClass = CLASS_POPORDERPMT;
			RflGetLead (pDisp->aszLeadThrough, LDT_CPNNO_ADR);

			//update our coupon list so that we know what items we are still allowed to 
			//apply coupons to. JHHJ
			nCouponListCount = ItemAutoCouponFillPopUpList(pItemList, sTotalKeyType);

			for (usPriorityLoop = 1; usPriorityLoop <= STD_MAX_PRIORITY_CPNS; usPriorityLoop++)
			{  // this is the priority of coupons
				USHORT    usRecordNo = 0;

				for (usRecordNo = 0; usRecordNo < nCouponListCount ; usRecordNo++)
				{
					if (pItemList[usRecordNo].usPriority != usPriorityLoop)
						continue;
					nTempTotalCouponAmount = pItemList[usRecordNo].ItemCoupon.lAmount;
					if (CurQualRcvBuff.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) nTempTotalCouponAmount *= -1;

					if((lParaTotalAmount) && (lParaTotalAmount < lTotalCouponAmount + nTempTotalCouponAmount))
					{
						continue;
					}
					// Check if this is a single coupon only and if it has already been applied
					// to this transaction.  If so, then we skip it.
					if ((pItemList[usRecordNo].ucStatusBytes[0] & CPN_STATUS0_SINGLE_TYPE) &&
						(pItemList[usRecordNo].nCouponAppliedCount >= 1))
					{
						continue;
					}
					pDisp->OrderPmt[nCouponCount].uchOrder = pItemList[usRecordNo].ItemCoupon.uchCouponNo;
					_tcsncpy(pDisp->OrderPmt[nCouponCount].aszPluName, pItemList[usRecordNo].ItemCoupon.aszMnemonic, STD_CPNNAME_LEN);
					nCouponCount++;
				}
			}

			// If no coupons were found to be displayed, then we do not
			// display the OEP window.  We just let the caller know that
			// everything succeeded.
			if (nCouponCount < 1)
			{
                return (ITM_SUCCESS);                /* exit ...          */
			}

			// Now we popup the OEP window with the list of coupons.
			// Then we let the operator pick.  If the operator cancels
			// out, then we pop down the OEP window and return.
			// Other wise we will then process what ever the operator selected.
			// We specify standard window size for an OEP window but the size
			//  in the Layout will override this size so we need to know what
			//  the actual size from the layout is.
			{
				int nScrollPage = 0, nDisplayedItems = 0;
				VOSRECT             VosRect;
				int i = 0;

				// Create our popup window with standard sizes.
				// we must use BlFwIfSetOEPGroups() to set the group before calling
				// the function MldCreatePopUpRectWindowOnly() in order to prevent
				// OEP window close issues the first time the OEP window is used.
				if (BlFwIfSetOEPGroups(BL_OEPGROUP_STR_AUTOCOUPON, 0) < 0) {   // general purpose selection OEP window #203 for auto coupon
					NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Error creating OEP window #203 for Auto Coupon. Does it exist?");
					BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
					return (ITM_SUCCESS);                /* exit ...          */
				}
				VosRect.usRow = 0;    /* start row */
				VosRect.usCol = 0;   /* start colum */
				VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length MLD_NOR_POP_ROW_LEN */
				VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */
				nDisplayedItems = MldCreatePopUpRectWindowOnly(&VosRect);

				if (nDisplayedItems < 1)
				{
					ItemOtherClear();                   /* clear descriptor  */
					MldDeletePopUp();
					BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
					return (ITM_SUCCESS);                /* exit ...          */
				}

				// Now we begin to loop through the various pages of coupons to be displayed
				// refreshing the popup window with each screenfull.  If there are more items
				// to be displayed that will fit in this window, then we need to provide a
				// continue button so the operator can go to the next page.  The MLD system
				// will automatically provide a back button for us.
				while (1)
				{
					// Zero out our temporary display buffer then put into the temporary
					// display buffer the first page of coupons.  We will copy items from
					// the pDisp buffer which was completely filled in above into the pDispTemp
					// buffer which we use to hold only the actual number of items to be displayed.
					memset (pDispTemp, 0, sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
					*pDispTemp = *pDisp;  // copy over the header
					for (i = 0, usItemLoop = nScrollPage * nDisplayedItems;
							usItemLoop < nScrollPage * nDisplayedItems + nDisplayedItems;
							usItemLoop++, i++)
					{
						if (usItemLoop >= nCouponCount)
							break;
						pDispTemp->OrderPmt[i] = pDisp->OrderPmt[usItemLoop];
					}

					pDispTemp->ulDisplayFlags |= MLD_DISPLAYFLAGS_DONE;
					pDispTemp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_MORE;
					if (usItemLoop < nCouponCount) {
						pDispTemp->ulDisplayFlags |= MLD_DISPLAYFLAGS_MORE;
					}

					pDispTemp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_BACK;
					if (nScrollPage > 0) {
						pDispTemp->ulDisplayFlags |= MLD_DISPLAYFLAGS_BACK;
					}

					// Fill in the popup window and then let the operator make a choice
					MldUpdatePopUp(pDispTemp);
					if (UifDiaOEP(&UI, pDispTemp) != UIF_SUCCESS)
					{    /* abort by user     */
						MldDeletePopUp();
						ItemOtherClear();                   /* clear descriptor  */
						BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
						return (UIF_CANCEL);                /* exit ...          */
					}

					// If the operator choses the Continue button to go to the
					// next page, FSC_SCROLL_DOWN, then we pull the next set of coupons
					// from the pDisp array filled in above.
					// If the operator choses the Back button to go to the previous
					// page, FSC_SCROLL_UP, then we will back up to the previous set
					// of coupons from the pDisp array filled in above.
					if (UI.auchFsc[0] == FSC_SCROLL_DOWN)
					{
						if (pDispTemp->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
							nScrollPage++;
						}
						if (nScrollPage < 1) {
							nScrollPage = 0;
						}
					}
					else if (UI.auchFsc[0] == FSC_SCROLL_UP)
					{
						nScrollPage--;
						if (nScrollPage < 1)
							nScrollPage = 0;
					}
					else if (UI.auchFsc[0] == FSC_CLEAR || UI.auchFsc[0] == FSC_ERROR) {
						// whether an error or the Clear key was pressed, dismiss window and return.
						MldDeletePopUp();
						ItemOtherClear();                   /* clear descriptor  */
						BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
						UI.ulData = 0;
						return(ITM_SUCCESS);
					}
					else
					{
						break;
					}
				}

				MldDeletePopUp();
				ItemOtherClear();                   /* clear descriptor  */
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			}

			// If the operator selected a coupon, then we will process that
			// coupon applying it to the transaction.
			// If the operator did not select a coupon, then we will just
			// return to the caller, with a success status code.
			if(UI.ulData > 0)
			{
				UIFREGCOUPON      UifRegCoupon = {0};
				USHORT            usRecordNo = 0;

				// The operator selected a coupon.
				// Find out which coupon from the displayed list the operator
				// selected, get the coupon information from the coupon data file,
				// and then apply the coupon to the transaction.
				UifRegCoupon.uchMajorClass = CLASS_UIFREGCOUPON;
                UifRegCoupon.uchMinorClass = CLASS_UICOMCOUPON;

				UifRegCoupon.uchFSC = (USHORT)UI.ulData;

				sRetStatus = ItemCoupon(&UifRegCoupon);   /* Coupon Modele */
				if (sRetStatus != ITM_SUCCESS)
				{
					// if applying the coupon selected fails then we will just bail out of this function
					// and return to the calling function.
					break;
				}

				// Do the global consistency checking to determine if we are
				// still within the bounds of total coupon amount.  We do the
				// check for meeting or exceeding the maximum number of coupons
				// allowed below as part of the pop up display logic.
				// We do amount check by fetching the coupon data and then using
				// it as part of our consistency checks.  If a particular coupon
				// will exceed the total amount of coupons allowed, skip it.

				/*----- Send Previous Item to Transaction Module -----*/
				if (ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMSALES ||
					ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMCOUPON ||
					ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMDISC)
				{
					ItemPreviousItem(NULL, 0);                  /* Saratoga */
				}

				for( usRecordNo = 0; usRecordNo < nCouponListCount ; usRecordNo++ )
				{
					ITEMCOUPON        ItemAutoCoupon = {0};

					// Update the number of times this coupon has been applied so that if
					// it is a single transaction coupon, it will only be presented once.
					if (pItemList[usRecordNo].ItemCoupon.uchCouponNo == UI.ulData)
					{
						lTotalCouponAmount +=  pItemList[usRecordNo].ItemCoupon.lAmount;
						pItemList[usRecordNo].nCouponAppliedCount++;
					}

					UifRegCoupon.uchFSC = pItemList[usRecordNo].ItemCoupon.uchCouponNo;

					memset(&TrnSearch, 0, sizeof(ITEMCOUPONSEARCH));

					if (ItemCouponGetTable(&UifRegCoupon, &ItemAutoCoupon, &TrnSearch) == ITM_SUCCESS)
					{
						if (ItemCouponSearch(&ItemAutoCoupon, &TrnSearch) != TRN_SUCCESS)
						{
							usItemListPriority[pItemList[usRecordNo].usPriority] = 0;
						}
					}
				}

				// Update the counters we use for global coupon consistency checking.
				// This check is so that the limits for total amount of all coupons applied
				// to the transaction as well as the total number of coupons that can be applied
				// to the transaction will not be exceeded.
				// We check if the total number of coupons applied has been met or exceeded.
				// Here is where we also accumulate the total coupon amounts for the check
				// in the logic above which creates the pop up window display with the list
				// of possible coupons.

				numAutoCoupon++;

				// Check to see if we allow only one coupon choice or not.
				// If only one coupon, then we will return immediately.
				if (0 == CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT2))
				{
					return(sRetStatus);
				}

				//we check to see if there is a value in the Total Number,
				//if not, we do not restrict the number of coupons to use JHHJ
				if((ulParaTotalNumber) && (ulParaTotalNumber <= numAutoCoupon))
				{
					return(sRetStatus);
				}
			}
			else if(UI.ulData == 0)
			{
				return(ITM_SUCCESS);
			}
		}      // end of for (;;) - loop to check for applicable auto coupons.
	}
	return(ITM_SUCCESS);
}



static int ItemAutoCouponFillPopUpList(ItemPopUpList *popUpList, SHORT sTotalKeyType /* from ItemTotalType() */)
{
	SHORT			  sRetStatus;
	USHORT			  usRecordNo;
	int               nCouponListCount = 0;
	ULONG             ulRecordNumber = RflGetMaxRecordNumberByType(FLEX_CPN_ADR);

	// Now lets build ourselves a table of the appropriate coupons along with their priority.
	// We are using the alloca () function to allocate memory on the stack so when this
	// function returns, the memory will be automatically deallocated unlike malloc().
	
	nCouponListCount = 0;
	for( usRecordNo = 0; usRecordNo < ulRecordNumber ; usRecordNo++ )
	{
		CPNIF             WorkCoupon = {0};
		ITEMCOUPON        ItemAutoCoupon = {0};
		ITEMCOUPONSEARCH  TrnSearch = {0};
		UIFREGCOUPON      UifRegCoupon = {0};

		WorkCoupon.uchCpnNo = usRecordNo + 1;

		// Get the coupon record for this coupon and lets do
		// a check to ensure that it is a valid coupon and one
		// which should be displayed in the popup window.
		// If not, then we will just jump to the bottom of the loop
		// and look at the next coupon.
		if ((sRetStatus = CliOpCpnIndRead(&WorkCoupon, 0)) != OP_PERFORM)
		{
			PifLog (MODULE_OP_CPN, LOG_EVENT_OP_CPN_AUTO_OPCPNINDREAD);
			return(OpConvertError(sRetStatus));
		}

		if(!(WorkCoupon.ParaCpn.uchCouponStatus[0] & CPN_STATUS0_AUTOCOUPON))
		{
			// If not an auto coupon type of coupons then skip this coupon.
			continue;
		}

		//This function checks the coupons information for validity,
		//as well as moves the information from UifRegCoupon, into our
		//ItemAutoCoupon structure for use within this function.
		//Although we have already done a check to see if the
		//coupon is in the coupon file, this function checks as well
		//and will return a non ITM_SUCCESS if the coupon does not exist.
		UifRegCoupon.uchMajorClass = CLASS_ITEMCOUPON;
		UifRegCoupon.uchMinorClass = CLASS_COMCOUPON;
		UifRegCoupon.uchFSC = WorkCoupon.uchCpnNo;
		sRetStatus = ItemCouponGetTable(&UifRegCoupon, &ItemAutoCoupon, &TrnSearch);
		if (sRetStatus != ITM_SUCCESS) continue;   // coupon is restricted, LDT_SALESRESTRICTED, or an error

		// if we are doing a food stamp total then we will filter out those coupons which are marked as Not Affect Food Stamp Itemizer.
		// on the other hand if we are not doing a food stamp total then we will allow coupon marked as Affect Food Stamp Itemizer to
		// be used with the transaction.
		if (sTotalKeyType == ITM_TYPE_FS_TOTAL && (ItemAutoCoupon.fbStatus[0] & FOOD_MODIFIER) == 0) continue; // food stamp total but coupon not affect food stamp 

		//ItemCouponSearch goes through the transaction file and searches
		//for the PLUs that would apply for the coupon in the ItemAutoCoupon structure
		//if it returns TRN_SUCCESS, this coupon could apply to the transaction,
		//and we need to add it to our list.
        if (ItemCouponSearch(&ItemAutoCoupon, &TrnSearch) == TRN_SUCCESS)
		{
			//perform steps to check for if it SHOULD be applied
			if (WorkCoupon.ParaCpn.uchAutoCPNStatus > 0 && WorkCoupon.ParaCpn.uchAutoCPNStatus <= CPN_AUTO_PRI_MAX)
			{
				USHORT		usGroupNo;

				popUpList[nCouponListCount].ItemCoupon = ItemAutoCoupon;
				popUpList[nCouponListCount].ItemCoupon.lAmount *= -1;
				popUpList[nCouponListCount].ucStatusBytes[0] = WorkCoupon.ParaCpn.uchCouponStatus[0];
				for(usGroupNo = 0; usGroupNo < STD_MAX_NUM_PLUDEPT_CPN; usGroupNo++)
				{
					popUpList[nCouponListCount].uchGroups[usGroupNo] = TrnSearch.aSalesItem[usGroupNo].uchGroup;
				}
				popUpList[nCouponListCount].usPriority = (WorkCoupon.ParaCpn.uchAutoCPNStatus & 0x3f);

				nCouponListCount++;
				// Make sure that we only use as many spaces as we have in our associating
				// array.
				if (nCouponListCount >= STD_MAX_PRIORITY_CPNS)
				{
					break;
				}
			}
        }
	}

	return nCouponListCount;
}
	
/****** End of Source ******/
