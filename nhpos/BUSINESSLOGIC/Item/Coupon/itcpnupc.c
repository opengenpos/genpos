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
* Program Name: ITCPNUPC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemCoupon()   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
*
** NCR2171 **
* May-15-00: 01.00.00  : M.Ozawa   : initial
*
** GenPOS **
* Jun-10-15: 02.02.00  : R.Chambers : check for tax system in ItemCouponUPCPrepare() for pTrnSearch->fbModifier
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
#include    "fsc.h"
#include    "uireg.h"
#include    "rfl.h"
#include    "itmlocal.h"
#include    "trans.h"

static SHORT   ItemCouponUPCGetAmt(DCURRENCY lUnitPrice, USHORT usDeptNo, LONG *plAmount);


/*
*===========================================================================
**Synopsis: SHORT ItemCouponGetTable(UIFREGCOUPON *pUifRegCoupon, 
*                   ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
*    Input:
*   Output: None
*    InOut: None
**  Return:
*
** Description: 
*===========================================================================
*/
SHORT   ItemCouponUPCPrepare(UIFREGCOUPON *pUifRegCoupon,
            ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    USHORT  i;

    /*--- Data Class ---*/
    pTrnSearch->uchMajorClass = pItemCoupon->uchMajorClass = CLASS_ITEMCOUPON;
    pTrnSearch->uchMinorClass = pItemCoupon->uchMinorClass = CLASS_UPCCOUPON;

    /*--- Coupon# ---*/
    pTrnSearch->uchCouponNo = pItemCoupon->uchCouponNo = 0;
    _tcsncpy(pItemCoupon->auchUPCCouponNo, pUifRegCoupon->auchUPCCouponNo, NUM_PLU_LEN);
    _tcsncpy(pTrnSearch->auchUPCCouponNo, pUifRegCoupon->auchUPCCouponNo, NUM_PLU_LEN);

    if (pUifRegCoupon->fbModifier & VOID_MODIFIER) {
        pItemCoupon->fbModifier |= VOID_MODIFIER;
        pTrnSearch->fbModifier |= VOID_MODIFIER;
    }
    
    // set taxable status by modifier key or override inherited coupon
	// search results modifier, if modifier key is entered
	if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
		 USHORT   uchBit;

        /* override modifier, if modifier key is entered */
        for (uchBit = MOD_TAXABLE1, i = 1; uchBit <= MOD_TAXABLE9; uchBit <<= 1, i++) {
            if (ItemModLocalPtr->fsTaxable & uchBit) {
				pTrnSearch->fbModifier &= ~ (MOD_CANTAXMOD << 1);  // clear any existing Canadian Tax type index
                pTrnSearch->fbModifier |= (USHORT)(i << 1);        // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
				break;
            }
        }
	} else if (ItemCommonTaxSystem() == ITM_TAX_US) {
		pTrnSearch->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE1) ? TAX_MODIFIER1 : 0;
		pTrnSearch->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE2) ? TAX_MODIFIER2 : 0;
		pTrnSearch->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE3) ? TAX_MODIFIER3 : 0;
		pTrnSearch->fbModifier |= (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) ? FOOD_MODIFIER : 0;
	}
	// so why is the VAT Int'l tax system not handled here?
	// Looks like tax modify is not allowed with VAT Int'l Tax System, see function ItemModTax().

	/*--- Number ---*/
    _tcsncpy(pItemCoupon->aszNumber, pUifRegCoupon->aszNumber, NUM_NUMBER);
    _tcsncpy(pTrnSearch->aszNumber, pUifRegCoupon->aszNumber, NUM_NUMBER);

    for (i = 0; i < STD_MAX_NUM_PLUDEPT_CPN; i++) {
        pTrnSearch->aSalesItem[i].fbStatus |= ITM_COUPON_NOT_FOUND;
    }

    return(ITM_SUCCESS);
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
SHORT   ItemCouponUPCSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    SHORT        sStatus;
    SHORT        sType = TrnDetermineStorageType();
    LONG         lCouponAmount;

	// Check the MDC bits to determine if we are to perform
	// a search for a transaction item to which the coupon
	// applies.  If we aren't to do an item search, then
	// we just obtain the coupon value and that is that.
	// Otherwise, we search to see if the product has been 
	// purchased.  See description of aTrnUPC table in trncpn.c.
    if (!CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
        /* Not Coupon Search */
        if ((sStatus = TrnQueryCouponUPCValue(pTrnSearch)) != TRN_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }
    } else {
        /* Coupon Search */
        if ((sStatus = TrnCouponSearch(pTrnSearch, sType)) != TRN_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }
    }
        
    if (pTrnSearch->sValue == TRN_CHECKER) {
        /* Checker Intervation */
        if ((sStatus = ItemCouponUPCGetAmt(pTrnSearch->lAmount, pTrnSearch->aSalesItem[0].usDeptNo,
                                                   &lCouponAmount)) != ITM_SUCCESS) {

            if (sStatus == ITM_CANCEL) {
                sStatus = UIF_CANCEL;
            }
            return (sStatus);
        }
        pTrnSearch->lAmount = lCouponAmount;
    }
    
    return (ITM_SUCCESS);
}

/*
*============================================================================
**Synopsis: SHORT   ItemCouponUPCGetAmt(LONG lUnitPrice, USHORT usDeptNo,
*                                         LONG *plAmount)
*
*    Input: LONG        lUnitPrice
*           USHORT      usDeptNo
*           
*
*   Output: LONG        *lAmount
*
*    InOut: None
*
**Return:   ITM_SUCCESS
*           ITM_CANCEL
*
** Description: This module gets the Coupon amount by operator entry.
*
*============================================================================
*/
static SHORT ItemCouponUPCGetAmt(DCURRENCY lUnitPrice, USHORT usDeptNo, LONG *plAmount)
{
	UIFDIADATA      WorkUI = {0};
    REGDISPMSG      DispMsg = {0};
    SHORT           sReturn;

    uchDispRepeatCo = 1;                    /* init repeat counter for display */

    /* ----- display "ENTER PRICE" ----- */
    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_PCHECK;
    RflGetLead (DispMsg.aszMnemonic, LDT_PP_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
        
    DispWrite(&DispMsg);
         
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    if (UifDiaPrice(&WorkUI)) {               /* req. price  */
        return (ITM_CANCEL);                 /* cancel by user */
    }
    
    if (WorkUI.auchFsc[0] != FSC_PRICE_ENTER) {
        return (LDT_SEQERR_ADR);
    }

    if (WorkUI.ulData) {
        /* price entry */
        if (lUnitPrice) {
            if (lUnitPrice < (DCURRENCY)(LONG)WorkUI.ulData) {
                return(LDT_KEYOVER_ADR);        /* return sequence error */
            }
        }
        /*--- HALO CHECK ---*/
        if (usDeptNo) {
			DEPTIF     DeptIf = {0};

            /* get dept record */
            DeptIf.usDeptNo = usDeptNo;
            if ((sReturn = CliOpDeptIndRead(&DeptIf, 0)) != OP_PERFORM) {
                return(OpConvertError(sReturn));
            } else {
                /* velify unit price vs HALO amount */
                if (RflHALO(WorkUI.ulData, *DeptIf.ParaDept.auchHalo) != RFL_SUCCESS) {
                    return(LDT_KEYOVER_ADR);        /* return sequence error */
                }
            }
        }
    } else {
        return (LDT_PROHBT_ADR);
    }

    *plAmount = WorkUI.ulData;
    
    return (ITM_SUCCESS);
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
** Description: Setup the coupons taxable status which depends on MDC settings as
*               well as the tax settings of the item which is being used with the
*               coupon.
*               See also function TrnStoCpnUPCSearch() which copies the tax status
*               from the item the coupon is used against.
*===========================================================================
*/
SHORT   ItemCouponUPCSetupStatus(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch)
{
    USHORT      i;

    /*--- Coupon Amount ---*/
	{
		LONG            lSign = -1L;

		if (pItemCoupon->fbModifier & VOID_MODIFIER) {
			lSign *= -1L;
		}

		if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
			lSign *= -1L;
		}

		pItemCoupon->lAmount = (pTrnSearch->lAmount * lSign);
	}

    /*--- Coupon Modifier(Taxable, Void) ---*/
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {              /* V3.3 */
		USHORT    uchBit, fbModifier;

        if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
			PARAMDC         MDCRcvBuff;

            /* coupon search */
            MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;
            MDCRcvBuff.usAddress = MDC_UPC3_ADR;
            CliParaRead(&MDCRcvBuff);
            MDCRcvBuff.uchMDCData = MDCRcvBuff.uchMDCData>> 2;
            if (MDCRcvBuff.uchMDCData & MOD_USTAXMOD) { /* affect to tax */
                /* inherit taxable modifier from the item the coupon has matched */
                pItemCoupon->fbModifier &= ~TAX_MOD_MASK;
                pItemCoupon->fbModifier |= (pTrnSearch->fbModifier & TAX_MOD_MASK);
            }
        }

        /* override modifier, if modifier key is entered */
        fbModifier = 0;
        for (uchBit = MOD_TAXABLE1, i = 1; uchBit <= MOD_TAXABLE9; uchBit <<= 1, i++) {
            if (ItemModLocalPtr->fsTaxable & uchBit) {
                fbModifier = (USHORT)(i << 1);   // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
				break;
            }
        }
        if (fbModifier) {
            pItemCoupon->fbModifier &= ~TAX_MOD_MASK;
            pItemCoupon->fbModifier |= fbModifier;
        }
        /*--- Status (Taxable) ---*/
		// Canadian Tax coupon logic uses taxable indicators different than US Tax system and International Tax coupon logic.
		// Canadian Tax system uses four bit tax index in least significant nibble.
		// See funtion TrnItemCoupon() and see function TrnItemCouponCanComn() for specific Canadian Tax logic.
        if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
            /* coupon search */
            if (pItemCoupon->fbModifier & VOID_MODIFIER) {
                pItemCoupon->fbStatus[0] = pTrnSearch->auchStatus[0];
            } else {
				PARAMDC         MDCRcvBuff;

                MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;
                MDCRcvBuff.usAddress = MDC_UPC3_ADR;
                CliParaRead(&MDCRcvBuff);

                /* inherit taxable status from the item the coupon has matched */
                pItemCoupon->fbStatus[0] = pTrnSearch->auchStatus[0];
                pItemCoupon->fbStatus[0] &= (UCHAR)((MDCRcvBuff.uchMDCData >> 2) & MOD_USTAXMOD);
                pItemCoupon->fbStatus[0] &= MOD_CANTAXMOD;
            }
        } else {
			PARAMDC         MDCRcvBuff;

            /* not coupon search */
            MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;
            MDCRcvBuff.usAddress = MDC_UPC3_ADR;
            CliParaRead(&MDCRcvBuff);
            pItemCoupon->fbStatus[0] &= (UCHAR)((MDCRcvBuff.uchMDCData >> 2) & MOD_USTAXMOD);  
            pItemCoupon->fbStatus[0] &= MOD_CANTAXMOD;
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {           /* V3.3 */
        if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
            /* execute coupon search MDC 371 enabled */
            /* inherit taxable modifier from search results, from the item the coupon has matched */
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT2)) {
                if (pTrnSearch->fbModifier & TAX_MODIFIER1) {
                    pItemCoupon->fbModifier |= TAX_MODIFIER1;
                }
            }
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT3)) {
                if (pTrnSearch->fbModifier & TAX_MODIFIER2) {
                    pItemCoupon->fbModifier |= TAX_MODIFIER2;
                }
            }
            if (CliParaMDCCheck(MDC_UPC4_ADR, EVEN_MDC_BIT0)) {
                if (pTrnSearch->fbModifier & TAX_MODIFIER3) {
                    pItemCoupon->fbModifier |= TAX_MODIFIER3;
                }
            }
            if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT2)) {
                if (pTrnSearch->fbModifier & FOOD_MODIFIER) {
                    pItemCoupon->fbModifier |= FOOD_MODIFIER;
                }
            }
		}

        // set US taxable status by modifier key or override inherited coupon
		// search results modifier, if modifier key is entered
		pItemCoupon->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE1) ? TAX_MODIFIER1 : 0;
		pItemCoupon->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE2) ? TAX_MODIFIER2 : 0;
		pItemCoupon->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE3) ? TAX_MODIFIER3 : 0;
		pItemCoupon->fbModifier |= (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) ? FOOD_MODIFIER : 0;

        /*--- Status (Taxable) ---*/
		// US Tax system and International Tax coupon logic uses taxable indicators different than Canadian Tax coupon logic.
		// US Tax system uses three tax flags, 0x02, 0x04, and 0x08 with least significant bit unused.
		// The check during tax affectation uses fbModifier and fbStatus[0] to determine tax itemizers to affect.
		// See function TrnItemCoupon() and see function TrnItemCouponUSComn() for specific US Tax logic.
        if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
            /* coupon search */
            if (pItemCoupon->fbModifier & VOID_MODIFIER) {
                pItemCoupon->fbStatus[0] = pTrnSearch->auchStatus[0];
                pItemCoupon->fbStatus[0] <<= TAX_MOD_SHIFT;        // shift taxable indicators as VOID_MODIFIER indicator is least significant bit.
            } else {
				PARAMDC         MDCRcvBuff;

                MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;
                MDCRcvBuff.usAddress = MDC_UPC3_ADR;
                CliParaRead(&MDCRcvBuff);

				// MDC 373 and 374 contain the coupon tax itemizer indicators (373 bits B and A, 374 Bit D
				// so we access both nibbles at same time then shift to line up itemizer bits properly.
                pItemCoupon->fbStatus[0] = (UCHAR)(pTrnSearch->auchStatus[0] & MOD_USTAXMOD);
                pItemCoupon->fbStatus[0] &= (UCHAR)((MDCRcvBuff.uchMDCData >> 2) & MOD_USTAXMOD);
                pItemCoupon->fbStatus[0] <<= TAX_MOD_SHIFT;        // shift taxable indicators as VOID_MODIFIER indicator is least significant bit.
                /* affect to food stamp */
                if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT2)) {
                    if (pTrnSearch->auchStatus[0] & CPN_STATUS0_FOOD_MOD) {
                        pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
                    }
                    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {
                        if (!(pTrnSearch->fbModifier & FOOD_MODIFIER)) {
                            pItemCoupon->fbStatus[0] ^= FOOD_MODIFIER;  /* turn over by current modifier status */
                        }
                    }
                } else {
                    /* not affect by default */
                    if (pItemCoupon->fbModifier & FOOD_MODIFIER) {
                        pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
                    }
                }   
            }
        } else {
            /* not coupon search */
			// clear the status to prep for setting the bits and then sett the proper bits
			// depending on the MDC settings.
			pItemCoupon->fbStatus[0] = 0;
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT2)) {
                pItemCoupon->fbStatus[0] |= TAX_MODIFIER1; 
            }
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT3)) {
                pItemCoupon->fbStatus[0] |= TAX_MODIFIER2;
            }
            if (CliParaMDCCheck(MDC_UPC4_ADR, EVEN_MDC_BIT0)) {
                pItemCoupon->fbStatus[0] |= TAX_MODIFIER3;
            }
            /* affect to food stamp */
            if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT2)) {
                pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
            } else {
                /* not affect by default */
                if (pItemCoupon->fbModifier & FOOD_MODIFIER) {
                    pItemCoupon->fbStatus[0] |= FOOD_MODIFIER;
                }
            }
        }
    } else {                                        /* Int'l VAT,   V3.3 */
        /*--- Status (Taxable) ---*/
        if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
            /* coupon search */
            if (pItemCoupon->fbModifier & VOID_MODIFIER) {
				// if we are processing a coupon void then just use the status as is
                pItemCoupon->fbStatus[0] = (pTrnSearch->auchStatus[0] << 1);  // affect tax itemizers, translate left 1 bit to skip void bit.
            } else {
				// determine the VAT Int'l Tax index and determine if the coupon amount is to affect that tax itemizer or not.
				// based on the MDC settings for how coupons affect tax itemizers. See TrnItemCoupon().
                switch(pTrnSearch->auchStatus[0] & MOD_VATTAXMOD) {
                case    INTL_VAT1:
                    if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT2)) {
                        pItemCoupon->fbStatus[0] = (INTL_VAT1 << 1);  // affect tax itemizer #1, translate left 1 bit to skip void bit.
                    }
                    break;

                case    INTL_VAT2:
                    if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT3)) {
                        pItemCoupon->fbStatus[0] = (INTL_VAT2 << 1);  // affect tax itemizer #2, translate left 1 bit to skip void bit.
                    }
                    break;

                case    INTL_VAT3:
                    if (CliParaMDCCheck(MDC_UPC4_ADR, EVEN_MDC_BIT0)) {
                        pItemCoupon->fbStatus[0] = (INTL_VAT3 << 1);  // affect tax itemizer #3, translate left 1 bit to skip void bit.
                    }
                    break;

                default:
                    break;
                }
                if (pTrnSearch->auchStatus[0] & CPN_STATUS0_AFFECT_DISC1) {
                    pItemCoupon->fbStatus[0] |= ITM_COUPON_AFFECT_SERV;
                }
            }
        } else {
            /* not coupon search so determine VAT Int'l Tax index for tax itemizer to affect based on MDC settings only */
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT2)) {
                pItemCoupon->fbStatus[0] = (INTL_VAT1 << 1);      // affect tax itemizer #1, translate left 1 bit to skip void bit.
            }
            if (CliParaMDCCheck(MDC_UPC3_ADR, ODD_MDC_BIT3)) {
                pItemCoupon->fbStatus[0] = (INTL_VAT2 << 1);      // affect tax itemizer #2, translate left 1 bit to skip void bit.
            }
            if (CliParaMDCCheck(MDC_UPC4_ADR, EVEN_MDC_BIT0)) {
                pItemCoupon->fbStatus[0] = (INTL_VAT3 << 1);      // affect tax itemizer #3, translate left 1 bit to skip void bit.
            }
        }
    }

    /*--- Status(Taxable/Discountable), V3.3 ---*/
    if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT0) ) {
        /* coupon search */
        if (pItemCoupon->fbModifier & VOID_MODIFIER) {
            pItemCoupon->fbStatus[1] = pTrnSearch->auchStatus[1];
        } else {
          if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT3)) {
                if (pTrnSearch->auchStatus[0] & CPN_STATUS0_AFFECT_DISC1) {
                    pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC1;
                }
            }
            if (CliParaMDCCheck(MDC_UPC6_ADR, EVEN_MDC_BIT0)) {
                if (pTrnSearch->auchStatus[0] & CPN_STATUS0_AFFECT_DISC2) {
                    pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC2;
                }
            }
        }
    } else {
        /* not coupon search */
        if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT3)) {
            pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC1;
        }
        if (CliParaMDCCheck(MDC_UPC6_ADR, EVEN_MDC_BIT0)) {
            pItemCoupon->fbStatus[1] |= ITM_COUPON_AFFECT_DISC2;
        }
    }
    if (!CliParaMDCCheck(MDC_UPC4_ADR, EVEN_MDC_BIT1)) {
        pItemCoupon->fbStatus[1] |= ITM_COUPON_HOURLY;
    }
    
    /*--- Coupon Mnemonic ---*/
	RflGetTranMnem (pItemCoupon->aszMnemonic, TRN_VCPN_ADR);

    /* copy plu no, for plu affection */
    if (CliParaMDCCheck(MDC_UPC5_ADR, ODD_MDC_BIT1)) {
        _tcsncpy(pItemCoupon->auchPLUNo, pTrnSearch->aSalesItem[0].auchPLUNo, NUM_PLU_LEN);
        pItemCoupon->uchAdjective = pTrnSearch->aSalesItem[0].uchAdjective;
        pItemCoupon->usDeptNo = pTrnSearch->aSalesItem[0].usDeptNo;
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

    return(ITM_SUCCESS);
}       

/****** End of Source ******/
