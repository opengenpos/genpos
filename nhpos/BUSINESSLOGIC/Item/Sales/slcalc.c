/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Dept./ Open PLU sales calculation                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SALECALC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemSalesCalculation()  
*           ItemSalesCalcProduct()  R3.1
*           ItemSalesCalcPPI()      R3.1
*           ItemSalesCalcPPIEC()    R3.1
*           ItemSalesCalcPM()       2172
*           ItemSalesCalcPMEC()     2172
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : initial                                   
* Nov-21-95: 03.01.00  : M.Ozawa   : add PPI Calculation
*          :           :           :                         
*
** NCR2171 **
* Aug-26-99: 01.00.00  : M.Teraki  : initial (for 2171)
* Oct-05-99:           : M.Teraki  : Added #pragma(...)
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
#include    <stdio.h>
#include    <math.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "uie.h"
#include    "rfl.h"
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
#include    "itmlocal.h"


static ITEMSALESPPI ItemSalesPpi;
static ITEMSALESPM  ItemSalesPM;


/*
*===========================================================================
**Synopsis: DCURRENCY  ItemSalesCalcDetermineBasePrice (ITEMSALES  *pItem, LONG lQTY);
*
*    Input: ITEMSALES  *pItem      - item description
*           LONG        lQTY       - quantity of the item
*   Output: None
*    InOut: None
**Return: DCURRENCY      calculated base price of the item sans condiments
*
** Description: This function determines the base price of an item by examing
*               the traits of the item to determine how to calculate the item
*               price for the given quantity of the item.
*
*               The price returned does not include the prices of any condiments
*               associated with the item and those must be calculated separately
*               and added to the base price of the item.
*
*               This function is designed to be called to calculate a price
*               using either the item quantity or the item coupon quantity by
*               using either of those two values as the lQty value.
*               Item coupon quantity comes into play when void consolidation is
*               turned on and when the item data is either being displayed or
*               printed. Normally the quantity of the individual item is used
*               whether the quantity as originally rung up or the quantity of
*               a voided item.
*
*               The following function calculates the base price and
*               includes PLU Price Multiplier in the logic as well as
*               what was in this function before.
*
*               We are working towards a general purpose price calculation
*               function that will take the item as input and provide an
*               item price as output.
*===========================================================================
*/
DCURRENCY  ItemSalesCalcDetermineBasePrice (CONST ITEMSALES  *pItemSales, LONG lQTY)
{
	DCURRENCY  lAllPrice = 0;

	if (pItemSales->uchPPICode != 0) {
		// is a PPI item so we use what is specified for the item.
		lAllPrice = pItemSales->lProduct;
	} else if (pItemSales->uchPM != 0) {
		// is a PM (price multiple) item so we use what is specified for the item.
		lAllPrice = pItemSales->lProduct;
	} else {
		// not a PPI item so standard calculations used.
		if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) != 0) {
			// scalable item so the price is what is specified for the item.
			lAllPrice = pItemSales->lProduct;
		} else if( pItemSales->usFor != 0 && pItemSales->usForQty !=0 ) {
			// an @For item so the price is what is specified for the item.
			lAllPrice = pItemSales->lProduct;
		} else {
			// anything else is the quantity times the unit price of the item.
			lAllPrice = pItemSales->lUnitPrice * lQTY;
		}
	}

	return lAllPrice;
}

/*
*===========================================================================
**Synopsis: DCURRENCY  ItemSalesCalcDetermineTotalPrice (ITEMSALES  *pItem, LONG lQTY);
*
*    Input: ITEMSALES  *pItem      - item description
*           LONG        lQTY       - quantity of the item, lQty for scalable, lQty / 1000 for non-scalable
*   Output: None
*    InOut: None
**Return: DCURRENCY      calculated total price of the item including condiments
*
** Description: This function determines the total price of an item by starting
*               with the base price and then adding the price of any condiments
*               to the base price.
*
*               The quantity specified must be in the proper units. For scalable items
*               that would be in thousandths of a pound/kilo. For non scalable items
*               that would be a count of the number of items.
*
*               This function may be used with void consolidation and coupon quantity as
*               well as non-consolidated void display with an item weight or count. For
*               an item count, the value passed in would be pItem->lQty / 1000L since
*               pItem->lQty is normalized to thousandths to be compatiable with weights.
*
*               For scalable items we just add the price of the condiment without
*               considering the quantity of the item.
*
*               For non-scalable items we multiple the price of the condiment by
*               the quantity of the item in order to arrive at the total price.
*
*               The following function calculates the total price and
*               includes condiments to the item in the logic as well as
*               the base price of the item itself.
*
*               We are working towards a general purpose price calculation
*               function that will take the item as input and provide an
*               item price as output.
*===========================================================================
*/
DCURRENCY  ItemSalesCalcDetermineTotalPrice (CONST ITEMSALES  *pItemSales, LONG lQty)
{
	DCURRENCY  lAllPrice = ItemSalesCalcDetermineBasePrice (pItemSales, lQty);
	USHORT     usI;
	USHORT     usMaxChild = ( pItemSales->uchChildNo + pItemSales->uchCondNo + pItemSales->uchPrintModNo );

	// The condiment area is also used to communicate a linked PLU in the first condiment
	// array element if there is a linked PLU (indicated by pItemSales->uchChildNo being non-zero).
    /* link plu, saratoga */
    if (pItemSales->usLinkNo) {
        usI = 0;
    } else {
        usI = pItemSales->uchChildNo;
    }
	for ( ; usI < usMaxChild; usI++) {
		D13DIGITS  d13ProdWork = 0;

		if (RflIsSpecialPlu(pItemSales->Condiment[usI].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0) {
			// if the item is non-scalable then multiply times the item quantity.
			// however if the item is scalable then just use the condiment price as is.
			d13ProdWork = pItemSales->Condiment[usI].lUnitPrice;
			if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
				d13ProdWork *= lQty;
			}
			lAllPrice += d13ProdWork;
		}
	}

	return lAllPrice;
}

LONG  ItemSalesCalcDetermineBaseQuantity (CONST ITEMSALES *pItemSales)
{
	LONG  lQTY = 0;

    if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) != 0)
	{
		lQTY = pItemSales->lQTY;
    }
	else
	{
        lQTY = pItemSales->lQTY / ITM_SL_QTY;
    }

	return lQTY;
}

/*
*===========================================================================
**Synopsis: VOID   ItemSalesCalcInitCom(VOID);
*
*    Input: None
*   Output: None
*    InOut: None
**Return: None
*
** Description: This function initialize calculation buffer.
*===========================================================================
*/
VOID ItemSalesCalcInitCom(VOID)
{
    ItemSalesCalcPPIInit();
    ItemSalesCalcPMInit();
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcECCom(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the cancel routine of calculation at E/C.
*===========================================================================
*/
SHORT   ItemSalesCalcECCom(ITEMSALES *pItemSales)
{
    ItemSalesCalcPPIEC(pItemSales);
    ItemSalesCalcPMEC(pItemSales);

    return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalculation(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function caluculates product and round the value. 
*===========================================================================
*/
SHORT   ItemSalesCalculation(ITEMSALES *pItemSales)
{
    SHORT     sReturn = ITM_SUCCESS;

	if ((ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE) == 0) {
		// the initialize PPI calculation buffer is called by the main item entry function ItemSales()
		// when the first item of a transaction is entered.
		// we have put an additional call to ItemSalesCalcInitCom() in function ItemSalesCalculation()
		// to ensure the PPI calculation buffer is initialized before calling ItemSalesCalcPPI() the first time.
		// PPI records are provisioned with AC71 in PEP to assign PPI rules
		// to the PPI Codes that have been provisioned for individual PLUs.
		//    Richard Chambers, Jun-02-2016 for GenPOS Rel 2.2.1
		/* initialize Promotional Pricing (PPI) calculation buffer R3.1 */
		ItemSalesCalcInitCom();                             /* ppi initialize r3.1 2172 */
	}

    /*----- NBS SPECIAL ROUNDING -----*/
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) {
		D13DIGITS d13ProdRemainder;
		D13DIGITS d13ProdWork = pItemSales->lQTY;

		d13ProdWork *= pItemSales->lUnitPrice;      // item quantity times (thousandths of a unit) times unit price
		d13ProdWork += 500L;                        // add rounding
		d13ProdRemainder = (d13ProdWork % 1000L);   // remember the remainder for rounding check
		d13ProdWork /= 1000L;                       // generate the actual product price, scale of units in thousandths
		/* QTY x unit price = xx...xx500  if d13ProdWork isn't odd value then round down */
		if (d13ProdRemainder == 0 && (d13ProdWork & 1) != 0) d13ProdWork--;

		if (d13ProdWork > STD_MAX_TOTAL_AMT) {   /* exceeds max. sales price */
            return(LDT_KEYOVER_ADR);
        }

        pItemSales->lProduct = d13ProdWork;
		//SR 603, PLUS that are scalable did not use the price multiple
		//option, but need to.
		ItemSalesCalcPM(pItemSales);
		sReturn = ITM_SUCCESS;
    } else {                                /* not scalable item */
        /* ----- calculate PPI befor normal calculation ----- */
        sReturn = ItemSalesCalcPPI(pItemSales);
        if (sReturn == ITM_CANCEL) {        /* if PPI is not calculated */
            pItemSales->uchPPICode = 0;     /* reset ppi code, for affection control */

            /* ----- calculate PM Product befor normal calculation ----- */
			/* ----- normal non scalable calculation ----- */
			sReturn = ItemSalesCalcPM(pItemSales);
            if (sReturn == ITM_CANCEL) {        /* if PPI is not calculated */
				D13DIGITS d13ProdWork = pItemSales->lQTY;

				d13ProdWork /= 1000L;                    // lQTY is item quantity times 1000 so divide to get actual quantity
				d13ProdWork *= pItemSales->lUnitPrice;   // number of units of item times the unit price
                /* if below max. sales price ok, otherwise data entry error */
                if (d13ProdWork > STD_MAX_TOTAL_AMT) { /* TAR 104586 */
                    return(LDT_KEYOVER_ADR);
                }
                pItemSales->lProduct = d13ProdWork;
				sReturn = ITM_SUCCESS;
            }
        }
    }

    return(sReturn);                         
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcProduct(LONG *plAmount, LONG lUnitPrice,
*                                                   LONG lQTY, USHORT usFor)
*
*    Input: *plAmount   - product
*           lUnitPrice  - unit price
*           lQTY        - quantity
*           usFor       - package quantity
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_KEYOVER_ADR: calculation error
*
** Description: This module calculates the price from package quainity
*===========================================================================
*/
static SHORT ItemSalesCalcProduct(DCURRENCY *plAmount, DCURRENCY lUnitPrice, LONG lQTY, USHORT usFor)
{
    D13DIGITS huWork;

    /* --- calcurate Unit Price / For --- */
    if (usFor > 1) {
		D13DIGITS lRemain;

		huWork = lUnitPrice;
		lRemain = (huWork % (LONG)usFor);
		huWork /= (LONG)usFor;
        lUnitPrice =  huWork;

        if (lRemain) {
            /* --- round up --- */
            if (lUnitPrice > 0) {
                lUnitPrice++;
            } else if (lUnitPrice < 0) {
                lUnitPrice = RflLLabs(lUnitPrice);
                lUnitPrice++;
                lUnitPrice *= -1L;
            }
        }
    }

    /* --- calcurate product * qty --- */
	huWork = lQTY;
	huWork *= lUnitPrice;
    *plAmount =  huWork;

    if (RflLLabs(*plAmount) > STD_MAX_TOTAL_AMT) {
        return(LDT_KEYOVER_ADR);
    } else {
        return (ITM_SUCCESS);
    }
}

/*
*===========================================================================
**Synopsis: VOID   ItemSalesCalcPPIInit(VOID);
*
*    Input: None
*   Output: None
*    InOut: None
**Return: None
*
** Description: This function initialize PPI calculation buffer.
*===========================================================================
*/
VOID ItemSalesCalcPPIInit(VOID)
{
	TRANGCFQUAL  *WorkCur = TrnGetGCFQualPtr();

    ItemSalesPpi = WorkCur->TrnSalesPpi;
}


/*
*===========================================================================
**Synopsis: VOID   ItemSalesClosePpiPM(VOID);
*
*    Input: None
*   Output: None
*    InOut: None
**Return: None
*
** Description: This function closes the buffer and puts it into the TrnInformation
*===========================================================================
*/
VOID ItemSalesClosePpiPM(VOID)
{
	TRANGCFQUAL   *WorkCur = TrnGetGCFQualPtr();
	
	if(ItemSalesPpi.PpiData[0].uchPPICode)
	{
		WorkCur->TrnSalesPpi = ItemSalesPpi;
	}
	if(ItemSalesPM.PMData[0].lPMPrice)
	{
		WorkCur->TrnSalesPM = ItemSalesPM;
	}
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesSearchForPPI(ITEMSALES *pItemSales, PPIIF *pPPIRecRcvBuffer);
*
*    Input: *pItemSales
*   Output: *pPPIRecRcvBuffer
*    InOut: none
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*         ITM_CANCEL      : Function aborted with Illegal data
*         LDT_KEYOVER_ADR : Function aborted with Illegal data
*
** Description: This function performs a lookup on the specified sales item
*               to find the current PPI state for the item and to perform
*               a set of checks on the new data in the specified sales item
*               and the current PPI state.
*===========================================================================
*/typedef struct {
	SHORT  sRet;
	USHORT usIndex;
	SHORT  sQuant;
} ItemSalesSearchResult;

ItemSalesSearchResult ItemSalesSearchForPPI (ITEMSALES *pItemSales, PPIIF *pPPIRecRcvBuffer)
{
	USHORT  sSalesPpiIndex;
    SHORT   sQuantity, sReturnStatus;
	ItemSalesSearchResult sRet = {0};

   /*
		----- search previous qty/price data from sales buffer ----

		Search through the global ItemSalesPpi.PpiData looking for an
		entry for this particular PPI table code number.  If we find
		the PPI code in the table then we know we are continuing to use
		an already created entry.  Otherwise, we will create a new entry.
		We know we need to create a new entry if we find an entry with
		a uchPPICode value of zero indicating an unused table entry before
		find this PPI table code number in the table..

		If we exceed the length of the table then the table is full so
		we will return an error of ITM_CANCEL because we have not found
		an existing entry for our uchPPICode and the table is full.
	*/
    for (sSalesPpiIndex = 0; sSalesPpiIndex < SALES_MAX_PPI; sSalesPpiIndex++) {
        if (ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode == 0) {
            ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode = pItemSales->uchPPICode;
            break;
        } else if (ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode == pItemSales->uchPPICode) {
            break;
        }
    }

	sRet.usIndex = sSalesPpiIndex;

    if (sSalesPpiIndex >= SALES_MAX_PPI) {
		sRet.sRet = ITM_CANCEL;
        return sRet;
    }

    sQuantity = (SHORT)(labs(pItemSales->lQTY) / 1000L);
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation */
        sQuantity *= -1;
    }
	sRet.sQuant = sQuantity;

    /* ----- check sign of quantity ----- */
    if (abs(sQuantity) > 32767) {  /* overflow SHORT */
		sRet.sRet = LDT_KEYOVER_ADR;
        return sRet;
    }

    /* ----- check sign of quantity ----- */
    if ((abs((LONG)sQuantity + ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty)) > 32767) {  /* overflow SHORT */
		sRet.sRet = LDT_KEYOVER_ADR;
        return sRet;
    }

	if ((sQuantity + ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty) < 0) {
		sRet.sRet = LDT_KEYOVER_ADR;
        return sRet;
	}

	/*----- GET PPI RECORD with pItemSales->uchPPI as key -----*/
    pPPIRecRcvBuffer->uchPpiCode = pItemSales->uchPPICode;
    if ((sReturnStatus = CliOpPpiIndRead(pPPIRecRcvBuffer, 0)) != OP_PERFORM) {
		sRet.sRet = ITM_CANCEL;
        return sRet;
    }

	// Check to make sure that we have at least one non-zero PPI record entry.
	// If the first PPI record contains a zero quantity then there is nothing to do.
    if (pPPIRecRcvBuffer->ParaPpi.PpiRec[0].uchQTY == 0) {
		/* if record is not set */
		sRet.sRet = ITM_CANCEL;
        return sRet;
    }

	sRet.sRet = ITM_SUCCESS;
    return sRet;
}


SHORT ItemSalesCalcPPITable(ITEMSALES *pItemSales, PPIDATA *pPpiData, PPIIF   *pPPIRecRcvBuffer)
{
	PPI_RECORD    ModifiedPpiRec[OP_MAX_PPI_LEVEL + 1] = { 0 };  /* PPI Record modified to include standard price */
	PPIIF   PPIRecRcvBuffer = { 0 };
	DCURRENCY   lCalPPIPrice;
	SHORT   sCalPPIQty, sReturnStatus;
	SHORT   sQuantity;
	SHORT   sPpiIndex, sPpiIndexMax;
	USHORT  usPassedMinTotalSaleLimit = 0;
	USHORT  fPpiStatus = 0;

	sCalPPIQty = (SHORT)(labs(pItemSales->lQTY) / 1000L);
	if ((pItemSales->fbModifier & VOID_MODIFIER)) {   /* void operation */
		sCalPPIQty *= -1;
	}

	/* ----- check sign of quantity ----- */
	if ((abs((LONG)sCalPPIQty + pPpiData->sPPIQty)) > 32767) {  /* overflow SHORT */
		return(LDT_KEYOVER_ADR);
	}

	sQuantity = (sCalPPIQty + pPpiData->sPPIQty);
	if (sQuantity < 0)
		return(LDT_KEYOVER_ADR);

	if (pPPIRecRcvBuffer == 0) {
		/*----- GET PPI RECORD with pItemSales->uchPPI as key -----*/
		PPIRecRcvBuffer.uchPpiCode = pItemSales->uchPPICode;

		if ((sReturnStatus = CliOpPpiIndRead(&PPIRecRcvBuffer, 0)) != OP_PERFORM) {
			return(ITM_CANCEL);
		}
	} else {
		PPIRecRcvBuffer = *pPPIRecRcvBuffer;
	}

	// Check to make sure that we have at least one non-zero PPI record entry.
	// If the first PPI record contains a zero quantity then there is nothing to do.
	if (PPIRecRcvBuffer.ParaPpi.PpiRec[0].uchQTY == 0)
	{    /* if record is not set */
		return(ITM_CANCEL);
	}

	for (sPpiIndex = 0; sPpiIndex < OP_MAX_PPI_LEVEL; sPpiIndex++) {
		ModifiedPpiRec[sPpiIndex + 1] = PPIRecRcvBuffer.ParaPpi.PpiRec[sPpiIndex];
	}

	// if PPIRecRcvBuffer.ParaPpi.ucBeforeInitialQty == 1 then we are to use PLU price
	// if PPIRecRcvBuffer.ParaPpi.ucBeforeInitialQty == 0 then we are to use PPI price
	if (PPIRecRcvBuffer.ParaPpi.ucBeforeInitialQty == 1)
	{
		ModifiedPpiRec[0].ulPrice = pItemSales->lUnitPrice;
	}
	else {
		DCURRENCY   lWork = 0;

		fPpiStatus |= PRECEDENCEDATA_FLAG_PPI_APPLIED; // since doing PPI PPIRecRcvBuffer.ParaPpi.ucBeforeInitialQty, turn on flag

													   // If we are doing discounts/premiums from the PLU price we do the calculations
													   // to create a price using the PLU price with the appropriate discount or premium.
													   // We then multiply the resulting price by the quantity of items since the calculated
													   // price with discount/premium is for each item.
		if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & (PPI_LOGICFLAGS01_PLUDISCOUNT | PPI_LOGICFLAGS01_PLUPREMIUM))
		{
			// if the discount/premium is zero (0) then we just use the item PLU price.
			// The discount or premium is specified as a percentage of three digits with an assummed
			// decimal point. So 123 is seen as 12.3%.
			if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUDISCOUNT)
			{
				lWork = (pItemSales->lUnitPrice * (1000 - (LONG)ModifiedPpiRec[1].ulPrice)) / 1000;
			}
			else if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUPREMIUM)
			{
				lWork = (pItemSales->lUnitPrice * ((DCURRENCY)1000 + (LONG)ModifiedPpiRec[1].ulPrice)) / 1000;
			}
		}
		else {
			if (ItemSalesCalcProduct(&lWork, (DCURRENCY)(LONG)ModifiedPpiRec[1].ulPrice, 1, (USHORT)ModifiedPpiRec[1].uchQTY) != ITM_SUCCESS) {
				return(LDT_KEYOVER_ADR);
			}
		}
		ModifiedPpiRec[0].ulPrice = (ULONG)(LONG)lWork;
	}
	ModifiedPpiRec[0].uchQTY = 0;

	// Look through the PPI table for the table entry with the maximum quantity.
	// Each table entry will have a non-zero value for the quantity until we reach
	// a table entry that has a zero in it.  When the zero quantity entry is found,
	// then we have found the end of the table which is the last non-zero entry.
	// Addressing robustness, if first entry in table is a zero entry then we will
	// process it as if the maximum quantity is zero..
	for (sPpiIndex = 1; sPpiIndex < OP_MAX_PPI_LEVEL; sPpiIndex++)
	{
		if (ModifiedPpiRec[sPpiIndex].uchQTY == 0) break;   /* read max level */
	}

	// Back off to the last non-zero PPI record.
	// PPI table entry index now points to the last good entry or max quantity entry
	sPpiIndex--;
	if (sPpiIndex < 1) {
		NHPOS_ASSERT(sPpiIndex > 0);
		return(LDT_EQUIPPROVERROR_ADR);
	}

	sPpiIndexMax = sPpiIndex;

	{
		/*	PPI MinTotalSale Enhancment - CSMALL
		Get the current transaction information.
		WorkTI->lMI contains the current subtotal for this transaction.
		pItemCommonLocal->ItemSales contains current item info that may not be in subtotal yet
		**/
		TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
		ITEMCOMMONLOCAL *pItemCommonLocal = ItemCommonGetLocalPointer();
		DCURRENCY       lSubTotal;

		lSubTotal = WorkTI->lMI + pItemCommonLocal->ItemSales.lProduct;

		usPassedMinTotalSaleLimit = 0;
		if (PPIRecRcvBuffer.ParaPpi.ulMinTotalSale != 0) {
			BOOL passedMinTotalSaleLimit = FALSE;

			usPassedMinTotalSaleLimit = 1;
			passedMinTotalSaleLimit = (lSubTotal >= (LONG)PPIRecRcvBuffer.ParaPpi.ulMinTotalSale);
			passedMinTotalSaleLimit = passedMinTotalSaleLimit || (sCalPPIQty * (LONG)ModifiedPpiRec[0].ulPrice >= (LONG)PPIRecRcvBuffer.ParaPpi.ulMinTotalSale);
			if (passedMinTotalSaleLimit) {
				usPassedMinTotalSaleLimit = 2;
			}
		}
	}

	if (usPassedMinTotalSaleLimit == 1)
	{
		// We have not passed minimum sales threshold so use item information
		// rather than the PPI table discount schedule.
		sPpiIndex = 0;
	}

	{
		SHORT  sQuantRemainder = 0;

		lCalPPIPrice = 0L;

		if (pPpiData->sPPIQty >= ModifiedPpiRec[sPpiIndexMax].uchQTY && sCalPPIQty > 0) {
			if (PPIRecRcvBuffer.ParaPpi.ucAfterMaxReached == 1) {
				// Use default PLU Price
				sPpiIndex = 0;
				lCalPPIPrice = pPpiData->lPPIPrice;
				sQuantity = sCalPPIQty;
				ModifiedPpiRec[0].ulPrice = pItemSales->lUnitPrice;
			}
		}

		for (; sPpiIndex > 0 && sQuantity > 0; )
		{
			if (sQuantity >= (SHORT)ModifiedPpiRec[sPpiIndex].uchQTY)
			{
				LONG  lPriceAmount = (LONG)ModifiedPpiRec[sPpiIndex].ulPrice;

				fPpiStatus |= PRECEDENCEDATA_FLAG_PPI_APPLIED;

				sQuantRemainder = (sQuantity - (SHORT)ModifiedPpiRec[sPpiIndex].uchQTY);

				// If we are doing discounts/premiums from the PLU price we do the calculations
				// to create a price using the PLU price with the appropriate discount or premium.
				// We then multiply the resulting price by the quantity of items since the calculated
				// price with discount/premium is for each item.
				if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & (PPI_LOGICFLAGS01_PLUDISCOUNT | PPI_LOGICFLAGS01_PLUPREMIUM))
				{
					// if the discount/premium is zero (0) then we just use the item PLU price.
					lPriceAmount = pItemSales->lUnitPrice;
					lPriceAmount *= ModifiedPpiRec[sPpiIndex].uchQTY;
					if (ModifiedPpiRec[sPpiIndex].ulPrice > 0)
					{
						if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUDISCOUNT)
						{
							lPriceAmount = (pItemSales->lUnitPrice * (1000 - (LONG)ModifiedPpiRec[sPpiIndex].ulPrice)) / 1000;
							lPriceAmount *= ModifiedPpiRec[sPpiIndex].uchQTY;
						}
						else if (PPIRecRcvBuffer.ParaPpi.ulPpiLogicFlags01 & PPI_LOGICFLAGS01_PLUPREMIUM)
						{
							lPriceAmount = (pItemSales->lUnitPrice * (1000 + (LONG)ModifiedPpiRec[sPpiIndex].ulPrice)) / 1000;
							lPriceAmount *= ModifiedPpiRec[sPpiIndex].uchQTY;
						}
					}
				}

				lCalPPIPrice += lPriceAmount;
				sQuantity -= (SHORT)ModifiedPpiRec[sPpiIndex].uchQTY;
			}
			else {
				sPpiIndex--;
			}
		}

		lCalPPIPrice += sQuantity * (LONG)ModifiedPpiRec[0].ulPrice;
	}

	/*
	----- save current price/qty -----

	We have now recalculated the amount that the total number of this
	particular item will add to the current total.  Now lets update
	our PPI totals and modify the price of the current item so that
	we can bring the transaction total in line with what it should
	be after applying PPI.
	*/
	{
		DCURRENCY  lOldPPIPrice;

		pPpiData->sPPIQty += sCalPPIQty;
		lOldPPIPrice = pPpiData->lPPIPrice;
		pPpiData->lPPIPrice = lCalPPIPrice;
		lCalPPIPrice -= lOldPPIPrice;
	}

	pItemSales->lProduct = lCalPPIPrice;      /* Modify this item's price to total correctly */
	pItemSales->blPrecedence |= fPpiStatus;  // indicate whether or not PPI has been applied to this item

	return (ITM_SUCCESS);
}

/*
*===========================================================================
** Format  : DCURRENCY   ItemSalesCalcCondimentPPIRecalc (CONST ITEMSALES *pItemSales,
*                                PPIIF *condimentPPI, PPIDATA *PpiData, DCURRENCY *pCondPrice)
*
*   Input  : ITEMSALES  *pItem                          -Item Data address
*            PPIIF      condimentPPI[STD_MAX_COND_NUM]  - receives PPI table data from CliOpPpiIndRead()
*            PPIDATA    PpiData[SALES_MAX_PPI]          - PPI calculation data and current status
*            DCURRENCY  pCondPrice[STD_MAX_COND_NUM]    - optional list of recalculated condiment prices
*   Output : none
*   InOut  : none
** Return  : DCURRENCY condiment price total. this does not include item base price.
*
** Synopsis: This function recalculates condiment PLU prices by incorporating
*            any PPI price changes into the prices of condiment PLUs. Any non-PPI PLU
*            will use the standard condiment PLU price. If the condiment PLU
*            has a non-zero PPI table code specified, the price will be adjusted
*            based on the PPI table calculations.
*
*            If the caller provides an array to receive the new condiment PLU prices
*            then the recalculated prices are provided in the array at the same
*            offset of the condiment within the pItemSales->Condiment[i] array
*            of condiment PLUs in the ITEMSALES object.
*===========================================================================
*/
static DCURRENCY   ItemSalesCalcCondimentPPIRecalc (USHORT usMask, CONST ITEMSALES *pItemSales, PPIIF *condimentPPI, PPIDATA *PpiData, LONG *pCondPrice)
{
	DCURRENCY     dPricePpi = 0;
	USHORT        usI, nPPi;
	SHORT         sReturnStatus;
	USHORT        usMaxChild = ( pItemSales->uchChildNo + pItemSales->uchCondNo + pItemSales->uchPrintModNo );

	// The condiment area is also used to communicate a linked PLU in the first condiment
	// array element if there is a linked PLU (indicated by pItemSales->uchChildNo being non-zero).
    /* link plu, saratoga */
    if (pItemSales->usLinkNo) {
        usI = 0;
    } else {
        usI = pItemSales->uchChildNo;
    }

	nPPi = 0;
	dPricePpi = 0;
	for ( ; usI < usMaxChild; usI++) {
		ITEMSALES TempItemSales = {0};
		USHORT  j;

		if ((usMask & 0x0001) != 0 && pItemSales->Condiment[usI].ControlCode.uchPPICode == 0) {
			// if we are asked to skip condiments that are not PPI then do so.
			continue;
		}

		if (pItemSales->Condiment[usI].ControlCode.uchPPICode == 0) {
			// if this condiment is not a PPI PLU then we do not need to do
			// a PPI price calculation.

			dPricePpi += pItemSales->Condiment[usI].lUnitPrice;

			// if the caller has provided an optional DCURRENCY array to receive
			// the price for this condiment then lets just copy over the
			// price as provided in the condiment data. A condiment that is not a
			// PPI PLU will also not be included in the PPI calculations for
			// condiments either.
			if (pCondPrice) pCondPrice[usI] = pItemSales->Condiment[usI].lUnitPrice;
			continue;
		}

		// search the PPIIF array to see if we have already retrieved the PPI table entry for
		// the PPI table code specified in the condiment data.
		for (j = 0; j < nPPi; j++) {
			if (condimentPPI[j].uchPpiCode == 0 || condimentPPI[j].uchPpiCode == pItemSales->Condiment[usI].ControlCode.uchPPICode) {
				break;
			}
		}

		if (condimentPPI[j].uchPpiCode == 0) {
			// we have not yet retrieved this PPI table entry for the specified PPI table code
			// so lets do that and add it to our memory resident table.
			/*----- GET PPI RECORD with pItemSales->uchPPI as key -----*/
			condimentPPI[j].uchPpiCode = pItemSales->Condiment[usI].ControlCode.uchPPICode;

			if ((sReturnStatus = CliOpPpiIndRead(&condimentPPI[j], 0)) != OP_PERFORM) {
				char  xBuff[128] = {0};
				sprintf (xBuff, "** ERROR: CliOpPpiIndRead() status %d uchPpiCode = %d", sReturnStatus, condimentPPI[j].uchPpiCode);
				NHPOS_ASSERT_TEXT(sReturnStatus == OP_PERFORM, xBuff);
				return(0);
			}
			nPPi++;
		}
		// using a temporary ITEMSALES struct into which we put the condiment PLU data
		// needed by the function ItemSalesCalcPPITable() to calculate the PPI price
		// for the condiment PLU, we then use the standard PPI calculation function.
		//
		TempItemSales.lQTY = 1000;
		TempItemSales.lUnitPrice = pItemSales->Condiment[usI].lUnitPrice;
		PpiData[j].uchPPICode = condimentPPI[j].uchPpiCode;
		ItemSalesCalcPPITable(&TempItemSales, PpiData + j, condimentPPI + j);

		dPricePpi += TempItemSales.lProduct;

		// if the caller has provided an optional DCURRENCY array to receive
		// the calculated PPI price for this condiment then lets put the calculated
		// PPI price into that array at the element for this condiment PLU.
		if (pCondPrice) pCondPrice[usI] = TempItemSales.lProduct;
	}

	return dPricePpi;
}

DCURRENCY   ItemSalesCalcCondimentPPIOnly(CONST ITEMSALES *pItemSales)
{
	PPIIF         condimentPPI[STD_MAX_COND_NUM] = { 0 };
	PPIDATA       PpiData[SALES_MAX_PPI] = {0};

	return ItemSalesCalcCondimentPPIRecalc(0x0001, pItemSales, condimentPPI, PpiData, NULL) * ItemSalesCalcDetermineBaseQuantity (pItemSales);
}

/*
*===========================================================================
** Format  : DCURRENCY  ItemSalesCalcCondimentPPIReplace(ITEMSALES *pItemSales);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : DCURRENCY condiment price total. this does not include item base price.
*
** Synopsis: This function recalculates condiment PLU prices by incorporating
*            any PPI price changes into the condiment PLUs. Any non-PPI PLU
*            will use the standard condiment PLU price. If the condiment PLU
*            has a non-zero PPI table code specified, the price will be adjusted
*            based on the PPI table calculations.
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated in
*			  ITEMSALES pItemSales.
*
*             Any non-PPI condiments will have the same price, the condiment PLU price
*             specified in the PLU data for the condiment. However any condiment that
*             has a PPI table code that is non-zero will have a PPI calculated price
*             depending on the PPI table values and the number of condiments with the
*             the same PPI table code.
*===========================================================================
*/
DCURRENCY   ItemSalesCalcCondimentPPIReplace(ITEMSALES *pItemSales)
{
	DCURRENCY   lCondimentPrice = 0;
	LONG        condLPrice[STD_MAX_COND_NUM] = { 0 };
	PPIIF       condimentPPI[STD_MAX_COND_NUM] = { 0 };
	PPIDATA     PpiData[SALES_MAX_PPI] = {0};
	USHORT      i;

	// Calculate the total of the condiments and perform PPI price calculations
	// while doing so. Request the updated condiment prices after the PPI calculations
	// so that we can then replace the current condiment prices with the recalculated
	// condiment prices which include any PPI adjustments.
	lCondimentPrice = ItemSalesCalcCondimentPPIRecalc(0, pItemSales, condimentPPI, PpiData, condLPrice) * ItemSalesCalcDetermineBaseQuantity (pItemSales);

	// Update the ITEMSALES with the recalculated condiment prices. If there are no
	// PPI adjustments then this will just be the condiment PLU prices.
	for (i = 0; i < STD_MAX_COND_NUM; i++) {
		pItemSales->Condiment[i].lUnitPrice = condLPrice[i];
	}

	return lCondimentPrice;
}

DCURRENCY   ItemSalesCalcCondimentPPIReplaceQty(ITEMSALES *pItemSales, LONG lQty)
{
	DCURRENCY   lCondimentPrice = 0;
	LONG        condLPrice[STD_MAX_COND_NUM] = { 0 };
	PPIIF       condimentPPI[STD_MAX_COND_NUM] = { 0 };
	PPIDATA     PpiData[SALES_MAX_PPI] = {0};
	USHORT      i;

	// Calculate the total of the condiments and perform PPI price calculations
	// while doing so. Request the updated condiment prices after the PPI calculations
	// so that we can then replace the current condiment prices with the recalculated
	// condiment prices which include any PPI adjustments.
	lCondimentPrice = ItemSalesCalcCondimentPPIRecalc(0, pItemSales, condimentPPI, PpiData, condLPrice) * lQty;

	// Update the ITEMSALES with the recalculated condiment prices. If there are no
	// PPI adjustments then this will just be the condiment PLU prices.
	for (i = 0; i < STD_MAX_COND_NUM; i++) {
		pItemSales->Condiment[i].lUnitPrice = condLPrice[i];
	}

	return lCondimentPrice;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcPPI(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the calculation routine of PPI PLU.
*               This module executes the following functions.
*
*          New-Qty  Old-Qty  Cal-Qty  Calulation         Content
*          -------  -------  -------  ----------    -------------------
*           ( + )    ( + )    ( + )    New - Old    normal + normal
*           ( + )    ( - )    ( + )    New - Old    void + normal (v<n)
*           ( + )    ( - )    ( - )   -New - Old    void + normal (v>n)
*           ( - )    ( + )    ( + )    New - Old    normal + void (n>v)
*           ( - )    ( + )    ( - )   -New - Old    normal + void (n<v)
*           ( - )    ( - )    ( - )   -New - Old    void + void
*
*
** Assumptions and Notes:
*
*           NOTE:  This function assumes that the item as specified in pItemSales
*           has not been added to the accumulator WorkTI->lMI.
*===========================================================================
*/
SHORT   ItemSalesCalcPPI(ITEMSALES *pItemSales)
{
    SHORT   sSalesPpiIndex;
	BOOL	blVoid = FALSE;
	USHORT  fPpiStatus = 0;

	//we dont need to do this previous item call if the current item
	//is not a condiment. JHHJ
//	if(pItemSales->ControlCode.auchStatus[2] & PLU_CONDIMENT == 0)
//	{
//		memset(&ItemSales, 0x00, sizeof(ItemSales));
//		ItemPreviousItem(&ItemSales, 0);
//	}

	// If the item does not have a PPI code or if the item has an
	// adjective code then we will not do any PPI calculations.
    if (pItemSales->uchPPICode == 0 || pItemSales->uchAdjective != 0) {
        return(ITM_CANCEL);
    }

    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return (ITM_CANCEL);
    }

    /* price change item, 2172 */
    if (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG) {
        /* not add to ppi calc buffer */
        return (ItemSalesOnlyCalcPPI(pItemSales));
    }

    /*
		----- search previous qty/price data from sales buffer ----

		Search through the global ItemSalesPpi.PpiData looking for an
		entry for this particular PPI table code number.  If we find
		the PPI code in the table then we know we are continuing to use
		an already created entry.  Otherwise, we will create a new entry.
		We know we need to create a new entry if we find an entry with
		a uchPPICode value of zero indicating an unused table entry before
		find this PPI table code number in the table..

		If we exceed the length of the table then the table is full so
		we will return an error of ITM_CANCEL because we have not found
		an existing entry for our uchPPICode and the table is full.

	*/
    for (sSalesPpiIndex = 0; sSalesPpiIndex < SALES_MAX_PPI; sSalesPpiIndex++) {
        if (ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode == 0) {
            ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode = pItemSales->uchPPICode;
            break;
        } else if (ItemSalesPpi.PpiData[sSalesPpiIndex].uchPPICode == pItemSales->uchPPICode) {
            break;
        }
    }
    if (sSalesPpiIndex >= SALES_MAX_PPI) {
        return(ITM_CANCEL);
    }

	/*
		void item is item discount parent

		Next we handle the case of if we are doing a VOID operation
		meaning that we are removing the item from the sales transaction.

		Finally, we save the current, existing PPI information for this
		transaction (all of this item previously rung up) then begin
		processing this new additional item.
	**/
    blVoid =  ((pItemSales->fbModifier & VOID_MODIFIER) != 0);
    if (blVoid) {   /* void operation */
		if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC))
		{
        	ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty -= (SHORT)(pItemSales->lQTY/1000L);
	        ItemSalesPpi.PpiData[sSalesPpiIndex].lPPIPrice -= pItemSales->lProduct;
    		pItemSales->lProduct   *= -1; 
    		
    		return (ITM_SUCCESS);
		}
	}


	ItemSalesCalcPPITable(pItemSales, &ItemSalesPpi.PpiData[sSalesPpiIndex], 0);

	// The code below keeps track of all the PLU's that have been 
	// entered into a transaction that have coupon or PPI precedence.
	// The Plu's and various information about the PLU's such as
	// department number, PLU number, PPI code and quantity of each
	// individual PLU entered are grouped together and stored into an
	// array below.  The size of the array has been determined based
	// on the MAX allowed different PLU's to be entered into a transaction.
	// ***PDINU
	if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_CTRL_COUPON)
	{
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
		USHORT	i;

		for (i = 0;i < SALES_MAX_PLU; i++)
		{
			// if it exists, update and break, or else continue to look
			if (memcmp(WorkCommon->PrecedenceData[i].auchPLUNo,pItemSales->auchPLUNo, NUM_PLU_LEN * sizeof(TCHAR)) == 0)
			{
				WorkCommon->PrecedenceData[i].fPpiStatus |= fPpiStatus;
				WorkCommon->sPPIQty[sSalesPpiIndex] = ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty;
				if ((pItemSales->fbModifier & VOID_MODIFIER)) {   /* void operation */
					WorkCommon->PrecedenceData[i].ItemSales.lQTY -= abs(pItemSales->lQTY);
				}
				else {
					WorkCommon->PrecedenceData[i].ItemSales.lQTY += abs(pItemSales->lQTY);
				}
				break;
			}
		}

		// did not find an existing record
		if (i == SALES_MAX_PLU)
		{
			for (i = 0;i < SALES_MAX_PLU; i++)
			{
				// If data exists in index, skip to next index
				if (WorkCommon->PrecedenceData[i].auchPLUNo[13])
					continue;
				// if index is empty, store information
				_tcsncpy(WorkCommon->PrecedenceData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
				WorkCommon->PrecedenceData[i].fPpiStatus |= fPpiStatus;
				WorkCommon->PrecedenceData[i].usSalesPpiIndex = sSalesPpiIndex;
				WorkCommon->PrecedenceData[i].ItemSales = *pItemSales;
				WorkCommon->sPPIQty[sSalesPpiIndex] = ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty;
				WorkCommon->usCountPrecedencePLU++;
				break;
			}
		}
	}

    return (ITM_SUCCESS);

}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcPPIEC(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the cancel routine of PPI calculation at E/C.
*===========================================================================
*/
SHORT   ItemSalesCalcPPIEC(ITEMSALES *pItemSales)
{
    SHORT  i;

	// If the item does not have a PPI code or if the item has an
	// adjective code then we will not do any PPI calculations.
    if (pItemSales->uchPPICode == 0 || pItemSales->uchAdjective != 0) {
        return(ITM_SUCCESS);
    }
    if (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG) {
        return(ITM_SUCCESS);
    }

    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return(ITM_SUCCESS);
    }
    
    /* ----- search previous qty/price data from sales buffer ---- */

    for (i=0; i<SALES_MAX_PPI; i++) {
/*
        if (ItemSalesPpi.PpiData[i].uchPPICode == 0) {
            ItemSalesPpi.PpiData[i].uchPPICode = pItemSales->uchPPICode;
            break;
        } else
*/
        if (ItemSalesPpi.PpiData[i].uchPPICode == pItemSales->uchPPICode) {
            break;
        }
    }
    if (i==SALES_MAX_PPI) {
        return(ITM_SUCCESS);
    }

    /* ----- error correct price/qty ----- */

    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation of price change */
        ItemSalesPpi.PpiData[i].sPPIQty += (SHORT)(pItemSales->lQTY/1000L);
        ItemSalesPpi.PpiData[i].lPPIPrice -= pItemSales->lProduct;
    } else {
        ItemSalesPpi.PpiData[i].sPPIQty -= (SHORT)(pItemSales->lQTY/1000L);
        ItemSalesPpi.PpiData[i].lPPIPrice -= pItemSales->lProduct;
    }
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesOnlyCalcPPI(ITEMSALES *pItemSales);
*
*    Input: nothing
*   Output: nothing
*    InOut: pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the calculate routine of PPI for split check
============================================================================
*/
SHORT   ItemSalesOnlyCalcPPI(ITEMSALES *pItemSales)
{
    PPIIF   PPIRecRcvBuffer;
    SHORT   sQuantity, sReturnStatus;
    DCURRENCY    lCalPPIPrice, lWork;
    SHORT   i;

	// If the item does not have a PPI code or if the item has an
	// adjective code then we will not do any PPI calculations.
    if (pItemSales->uchPPICode == 0 || pItemSales->uchAdjective != 0) {
        return(ITM_CANCEL);
    }
    
    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return (ITM_CANCEL);
    }

    /* ----- search previous qty/price data from sales buffer ---- */

    for (i=0; i<SALES_MAX_PPI; i++) {
        if (ItemSalesPpi.PpiData[i].uchPPICode == 0) {
            ItemSalesPpi.PpiData[i].uchPPICode = pItemSales->uchPPICode;
            break;
        } else if (ItemSalesPpi.PpiData[i].uchPPICode == pItemSales->uchPPICode) {
            break;
        }
    }
    if (i==SALES_MAX_PPI) {
        return(ITM_CANCEL);
    }

    sQuantity = (SHORT)(labs(pItemSales->lQTY)/1000L);
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation */
        sQuantity *= -1;
    }

    /* ----- check sign of quantity ----- */
    if (abs(sQuantity) > 32767) {  /* overflow SHORT */
        return(LDT_KEYOVER_ADR);
    }

    /*----- GET PPI RECORD with pItemSales->uchPPI as key -----*/
    PPIRecRcvBuffer.uchPpiCode = pItemSales->uchPPICode;

    if ((sReturnStatus = CliOpPpiIndRead(&PPIRecRcvBuffer, 0)) != OP_PERFORM) {
        return(ITM_CANCEL);
    } 
    if (PPIRecRcvBuffer.ParaPpi.PpiRec[0].uchQTY == 0) {    /* if record is not set */
        return(ITM_CANCEL);
    }
    for (i=0; i<OP_MAX_PPI_LEVEL; i++) {
        if (PPIRecRcvBuffer.ParaPpi.PpiRec[i].uchQTY == 0) break;   /* read max level */
    }

    /* ----- calculate whole product price ----- */
    for (lCalPPIPrice=0L, i--; i>=0 ; i--) {
        if (sQuantity >= (SHORT)PPIRecRcvBuffer.ParaPpi.PpiRec[i].uchQTY) {
            lCalPPIPrice += (DCURRENCY)(LONG)PPIRecRcvBuffer.ParaPpi.PpiRec[i].ulPrice;
            sQuantity -= (SHORT)PPIRecRcvBuffer.ParaPpi.PpiRec[i].uchQTY;
            i++;
        }
    }
    /* if qty is not reached to lowest price level qty */
    if ( (i < 0) && (sQuantity) ) {
        if (ItemSalesCalcProduct(&lWork, (DCURRENCY)(LONG)PPIRecRcvBuffer.ParaPpi.PpiRec[0].ulPrice, (LONG)sQuantity,
                    (USHORT)PPIRecRcvBuffer.ParaPpi.PpiRec[0].uchQTY) == ITM_SUCCESS) {

            lCalPPIPrice += lWork;
        } else {
            return(LDT_KEYOVER_ADR);
        }
    }
    if (RflLLabs(lCalPPIPrice) > STD_MAX_TOTAL_AMT) {
        return(LDT_KEYOVER_ADR);
    }

    /* ----- calculate current product price ----- */
    pItemSales->lProduct   = lCalPPIPrice;      /* allow minus sales */

    return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID   ItemSalesCalcPMInit(VOID);
*
*    Input: None
*   Output: None
*    InOut: None
**Return: None
*
** Description: This function initialize PPI calculation buffer.
*===========================================================================
*/
VOID ItemSalesCalcPMInit(VOID)
{
	TRANGCFQUAL    *WorkCur = TrnGetGCFQualPtr();
	
    ItemSalesPM = WorkCur->TrnSalesPM;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcPM(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the calculation routine of PLU w/ Price Multiple.
*               This module executes the following functions.
*
*          New-Qty  Old-Qty  Cal-Qty  Calulation         Content
*          -------  -------  -------  ----------    -------------------
*           ( + )    ( + )    ( + )    New - Old    normal + normal
*           ( + )    ( - )    ( + )    New - Old    void + normal (v<n)
*           ( + )    ( - )    ( - )   -New - Old    void + normal (v>n)
*           ( - )    ( + )    ( + )    New - Old    normal + void (n>v)
*           ( - )    ( + )    ( - )   -New - Old    normal + void (n<v)
*           ( - )    ( - )    ( - )   -New - Old    void + void
*===========================================================================
*/
SHORT   ItemSalesCalcPM(ITEMSALES *pItemSales)
{
    SHORT   sOldPMQty, sCalPMQty, sQuantity;
    DCURRENCY    lOldPMPrice, lCalPMPrice, lCalWholePrice;
    SHORT  i,j;
    UCHAR   uchAdjective;

    if (pItemSales->uchPM <= 1) {
        return(ITM_CANCEL);
    }

    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return (ITM_CANCEL);
    }

    /* price change item, 2172 */
    if (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG) {
        /* not add to ppi calc buffer */
        return (ItemSalesOnlyCalcPM(pItemSales));
    }

    /* ----- search previous qty/price data from sales buffer ---- */

    uchAdjective = (UCHAR) ((pItemSales->uchAdjective - 1) % 5 + 1); /* adj valiation */

    for (i=0; i<SALES_MAX_PM; i++) {
        if (RflIsSpecialPlu(ItemSalesPM.PMData[i].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) == 0) {
            _tcsncpy(ItemSalesPM.PMData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
            ItemSalesPM.PMData[i].uchAdjective = uchAdjective;
            break;
        } else
        if (_tcsncmp(ItemSalesPM.PMData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN) == 0) {
            if (ItemSalesPM.PMData[i].uchAdjective == uchAdjective) {
                break;
            }
        }
    }
    if (i==SALES_MAX_PM) {
        return(ITM_CANCEL);
    }

	/* void item is item discount parent */
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation */
		if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC)) {
        	ItemSalesPM.PMData[i].sPMQty -= (SHORT)(pItemSales->lQTY/1000L);
	        ItemSalesPM.PMData[i].lPMPrice -= pItemSales->lProduct;
    		pItemSales->lProduct   = RflLLabs(pItemSales->lProduct); 
    		
    		return (ITM_SUCCESS);
		}
	}


    sOldPMQty = ItemSalesPM.PMData[i].sPMQty;
    lOldPMPrice = ItemSalesPM.PMData[i].lPMPrice;

	//SR 603 Fix, if the item is scalabe, it does not want to 
	//divide the number by 1000 because it will have to do
	//some rounding later on in the function
	if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE)
	{
		sCalPMQty = (SHORT)(labs(pItemSales->lQTY));
	} else
	{
		sCalPMQty = (SHORT)(labs(pItemSales->lQTY)/1000L);
	}
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation */
        sCalPMQty *= -1;
    }

    /* ----- check sign of quantity ----- */
    if ((LONG)(abs(sCalPMQty) + abs(sOldPMQty)) > 32767L) {  /* overflow SHORT */
        return(LDT_KEYOVER_ADR);
    }
    sQuantity = (sCalPMQty + sOldPMQty);
    sQuantity = abs(sQuantity);

    /* calculate whole amount */
    j = sQuantity/pItemSales->uchPM;
    lCalWholePrice = 0L;
    for (lCalWholePrice=0L; j>0 ; j--) {   /* add unit price, if qty reached to pm */
        lCalWholePrice += pItemSales->lUnitPrice;
        sQuantity -= pItemSales->uchPM;
    }
    lCalPMPrice = 0L;
    if (sQuantity) {  /* calculate reminded price */
        if (ItemSalesCalcProduct(&lCalPMPrice, pItemSales->lUnitPrice, sQuantity, (USHORT)pItemSales->uchPM) != ITM_SUCCESS) {
            return(LDT_KEYOVER_ADR);
        }
    }
    lCalWholePrice += lCalPMPrice;
    if (RflLLabs(lCalWholePrice) > STD_MAX_TOTAL_AMT) {
        return(LDT_KEYOVER_ADR);
    }
    if ((sCalPMQty + sOldPMQty) < 0) {    /* void operation */
        lCalWholePrice *= -1L;
    }

    /* ----- save current price/qty ----- */
    ItemSalesPM.PMData[i].sPMQty = (sCalPMQty + sOldPMQty);
    ItemSalesPM.PMData[i].lPMPrice = lCalWholePrice;

    /* ----- calculate current product price ----- */
    lCalWholePrice -= lOldPMPrice;

	//603, If it is a scalable PLU, then now divide the number
	//by 100 to find the price plus the hundredth decimal
	lCalWholePrice = RflLLabs(lCalWholePrice);                  /* allow minus sales for instance with cursor void */
	if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE)
	{
		lCalWholePrice /= 100;
		//If the price with a mod of 10 is greater than or equal
		//to 5, round up
		if((lCalWholePrice % 10) >= 5)
		{
			lCalWholePrice /= 10;
			lCalWholePrice++;
		}else
		//else keep the price where it is, take out the hundredth
		//decimal point
		{
			lCalWholePrice /= 10;
		}
	}
	pItemSales->lProduct   = lCalWholePrice;
    return (ITM_SUCCESS);

}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCalcPMEC(ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the cancel routine of PPI calculation at E/C.
*===========================================================================
*/
SHORT   ItemSalesCalcPMEC(ITEMSALES *pItemSales)
{
    SHORT  i;
    UCHAR   uchAdjective;

    if (pItemSales->uchPM == 0) {
        return(ITM_SUCCESS);
    }
    if (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG) {
        return(ITM_SUCCESS);
    }

    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return(ITM_SUCCESS);
    }
    /* ----- search previous qty/price data from sales buffer ---- */

    uchAdjective = (UCHAR) ((pItemSales->uchAdjective - 1) % 5 + 1); /* adj valiation */

    for (i=0; i<SALES_MAX_PM; i++) {
        if (_tcsncmp(ItemSalesPM.PMData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN) == 0) {
            if (ItemSalesPM.PMData[i].uchAdjective == uchAdjective) {
                break;
            }
        }
    }
    if (i==SALES_MAX_PM) {
        return(ITM_SUCCESS);
    }

    /* ----- error correct price/qty ----- */
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation of price change */
        ItemSalesPM.PMData[i].sPMQty += (SHORT)(pItemSales->lQTY/1000L);
        ItemSalesPM.PMData[i].lPMPrice += pItemSales->lProduct;
    } else {
        ItemSalesPM.PMData[i].sPMQty -= (SHORT)(pItemSales->lQTY/1000L);
        ItemSalesPM.PMData[i].lPMPrice -= pItemSales->lProduct;
    }
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesOnlyCalcPM(ITEMSALES *pItemSales);
*
*    Input: nothing
*   Output: nothing
*    InOut: pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This module is the calculate routine of PPI for split check
============================================================================
*/
SHORT   ItemSalesOnlyCalcPM(ITEMSALES *pItemSales)
{
    SHORT   sQuantity;
    DCURRENCY    lCalPMPrice;
    SHORT   i;
    UCHAR   uchAdjective;

    if (pItemSales->uchPM == 0) {
        return(ITM_CANCEL);
    }

    /* Random Weight Label by Price, 2172 */
    if (pItemSales->uchRndType & ITM_TYPE_PRICE) {
        return (ITM_CANCEL);
    }

    /* ----- search previous qty/price data from sales buffer ---- */
    uchAdjective = (UCHAR) ((pItemSales->uchAdjective - 1) % 5 + 1); /* adj valiation */

    for (i=0; i<SALES_MAX_PM; i++) {
        if (RflIsSpecialPlu(ItemSalesPM.PMData[i].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) == 0) {
            _tcsncpy(ItemSalesPM.PMData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
            ItemSalesPM.PMData[i].uchAdjective = uchAdjective;
            break;
        } else if (_tcsncmp(ItemSalesPM.PMData[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN) == 0) {
            if (ItemSalesPM.PMData[i].uchAdjective == uchAdjective) {
                break;
            }
        }
    }
    if (i==SALES_MAX_PM) {
        return(ITM_SUCCESS);
    }

    sQuantity = (SHORT)(labs(pItemSales->lQTY)/1000L);
    if (pItemSales->fbModifier & VOID_MODIFIER) {   /* void operation */
        sQuantity *= -1;
    }

    /* ----- check sign of quantity ----- */
    if (abs(sQuantity) > 32767) {  /* overflow SHORT */
        return(LDT_KEYOVER_ADR);
    }

    /* ----- calculate whole product price ----- */
    if (abs(sQuantity) == (SHORT)(pItemSales->uchPM)) {
        lCalPMPrice = pItemSales->lUnitPrice;
    } else {
        if (ItemSalesCalcProduct(&lCalPMPrice, pItemSales->lUnitPrice, sQuantity, (USHORT)pItemSales->uchPM) != ITM_SUCCESS) {
            return(LDT_KEYOVER_ADR);
        }
    }
    
    if (RflLLabs(lCalPMPrice) > STD_MAX_TOTAL_AMT) {
        return(LDT_KEYOVER_ADR);
    }

    /* ----- calculate current product price ----- */
    pItemSales->lProduct   = RflLLabs(lCalPMPrice);      /* allow minus sales */

    return (ITM_SUCCESS);

}

/****** end of file ******/
