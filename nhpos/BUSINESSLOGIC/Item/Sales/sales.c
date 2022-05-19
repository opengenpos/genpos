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
* Title       : Sales module main
* Category    : Item Module, NCR 2170 US Hsopitality Model Application
* Program Name: SALES.C
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
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstract: ItemSales() : Sales item module main function
*
* --------------------------------------------------------------------------
* Update Histories
* Update Histories
*    Date  : Ver. Rev. :   Name    : Description
* Feb-14-95: 03.00.00  :   hkato   : R3.0
* Nov-09-95: 03.01.00  : M.Ozawa   : Add repeat key handling
*
** NCR2171 **
* Sep-06-99: 01.00.00  : M.Teraki  :initial (for 2171)
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
#include    <math.h>
#include    <stdarg.h>

#include    <ecr.h>
#include    <uie.h>
#include    <pif.h>
#include    <log.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    <rfl.h>
#include    <fsc.h>
#include    "itmlocal.h"
#include	<mld.h>
#include	<uireg.h>
#include	"..\..\UifReg\uiregloc.h"
#include	<cscas.h>
#include	<item.h>
#include	<trans.h>
#include    "BlFWif.h"
#include	"..\Sa\UIE\uiedev.h"


// Indicate if the ITEMSALES data is for a discount modified sales item
// of one of the supported types.  This minor class is when an ITEMSALES data
// is transformed into a discounted item using Item Discount #2.
SHORT ItemSalesDiscountClassIsMod (UCHAR uchMinorClass)
{
	return ((uchMinorClass == CLASS_DEPTMODDISC) ||
            (uchMinorClass == CLASS_PLUMODDISC) ||
            (uchMinorClass == CLASS_OEPMODDISC) ||
            (uchMinorClass == CLASS_SETMODDISC));
}

// Indicate if the ITEMSALES data is for a discounted sales item of
// one of the supported types.  This minor class is when an ITEMSALES data
// is transformed into a discounted item using item discounts other than
// item discount #2.  Item Discount #2 is a specialized discount.
// see function ItemDiscCommonIF().
SHORT ItemSalesDiscountClassIsItem (UCHAR uchMinorClass)
{
	return ((uchMinorClass == CLASS_DEPTITEMDISC) ||
            (uchMinorClass == CLASS_PLUITEMDISC) ||
            (uchMinorClass == CLASS_OEPITEMDISC) ||
            (uchMinorClass == CLASS_SETITEMDISC));
}


/*
*===========================================================================
**Synopsis: USHORT ItemClassIsOneOf (UCHAR uchMajorMinorClass, ...)
*
*    Input: UCHAR uchMajorMinorClass
*           list of zero or more class identifier followed by an argument of zero
*   Output: None
*    InOut: None
**Return: Returns 0 if no match or index of the class identifier which did match.
*
** Description: This function compares the first argument which contains either a Major
*               class (typically uchMajorClass) or a Minor class (typically uchMinorClass)
*               identifier value and compares this identifier against a list of possible identifer
*               matches. The list must be terminated by a value of zero.
*
*               If a match is found then the argument index (value of 1 to number of class identifers
*               specified) is returned. If a match is not found then a value of zero is returned.
*
*               WARNING: since there is some overlap between the values used for a Minor class, care
*                        must be taken that all values are from the same Major class identifier space.
*                        In other words, for a Major class of CLASS_UIFREGSALES you should not include
*                        Minor class identifier CLASS_UIREGDISC1 since that is for use with UIFREGDISC.
** Usage:
*    usType = ItemClassIsOneOf (pUifRegSales->uchMinorClass, CLASS_UIDEPT, CLASS_UIREPEAT, CLASS_UIPLU, 0);
*===========================================================================
*/
USHORT ItemClassIsOneOf (UCHAR uchMajorMinorClass, ...)
{
	USHORT  usMatch = 0;
	va_list argptr;

	va_start(argptr, uchMajorMinorClass);

	do {
		UCHAR  uchArg = va_arg(argptr, int);  // variable arguments are promoted to a default type, int for UCHAR.

		usMatch++;
		if (uchArg == 0 || uchArg > 127) break;
		if (uchMajorMinorClass == uchArg) return usMatch;
	} while (1);

	va_end(argptr);
	return 0;
}


/*
 * Description: There are several PLU and Department characteristics that are shared between
 *              these two entities. This function is designed to be used by a variety of different
 *              PLU and Department related data structures to provide a nice bit map of these
 *              various characteristics so that a programmer no longer need to know which array
 *              element a particular characteristic's indicator is located.
 *
 *              This UCHAR array is an element of the following data structures:
 *                - auchPluStatus of ITEMCONTCODE
 *                - auchContOther of PLU_CONTROL
 *                - auchControlCode of OPDEPT_PARAENTRY
 *
 *  WARNING: It appears that only array elements 0 through 3 have the same bit map and representation
 *           between PLU_CONTROL and OPDEPT_PARAENTRY.  See function RptPLUByDEPT().
 *
 *  WARNING: It appears that only array elements 0 through 5 have the same bit map and representation
 *           between PLU_CONTROL and ITEMCONTCODE (ITEMSALES). See function ItemSalesSetMenu().
*/
ULONG  ItemSalesItemPluTypeInfo (UCHAR *auchContOther)
{
	ITEMCONTCODE      *pItemContCode = NULL;
	PLU_CONTROL       *pPluControl = NULL;
	OPDEPT_PARAENTRY  *pOpDeptParaEntry = NULL;
	ULONG  ulBitMapPluType = 0;

	// Any additions to this list should recognize that only array elements 0 through 3 should
	// be used. See the warnings above.
	ulBitMapPluType |= (auchContOther[0] & PLU_MINUS) ? PLU_BM_MINUS_PLU : 0;
	ulBitMapPluType |= (auchContOther[0] & PLU_PRT_VAL) ? PLU_BM_PRT_VAL : 0;
	ulBitMapPluType |= (auchContOther[1] & PLU_AFFECT_DISC1) ? PLU_BM_AFECT_DISC1 : 0;
	ulBitMapPluType |= (auchContOther[1] & PLU_AFFECT_DISC2) ? PLU_BM_AFECT_DISC2 : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_HASH) ? PLU_BM_HASH_PLU : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_SCALABLE) ? PLU_BM_SCALE_PLU : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_CONDIMENT) ? PLU_BM_CONDI_PLU : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_USE_DEPTCTL) ? PLU_BM_USE_DEPT : 0;
	ulBitMapPluType |= (auchContOther[4] & PLU_PROHIBIT_VAL1) ? PLU_BM_ADJ_PROHIBIT_1 : 0;
	ulBitMapPluType |= (auchContOther[4] & PLU_PROHIBIT_VAL2) ? PLU_BM_ADJ_PROHIBIT_2 : 0;
	ulBitMapPluType |= (auchContOther[4] & PLU_PROHIBIT_VAL3) ? PLU_BM_ADJ_PROHIBIT_3 : 0;
	ulBitMapPluType |= (auchContOther[4] & PLU_PROHIBIT_VAL4) ? PLU_BM_ADJ_PROHIBIT_4 : 0;
	ulBitMapPluType |= (auchContOther[4] & PLU_PROHIBIT_VAL5) ? PLU_BM_ADJ_PROHIBIT_5 : 0;

	return ulBitMapPluType;
}

#if 0
ULONG  ItemSalesItemPluKitchenPrint (UCHAR *auchContOther)
{
	ULONG  ulBitMapPluType = 0;

	ulBitMapPluType |= (auchContOther[2] & PLU_SND_KITCH1) ? PLU_BM_KP_PRINT1 : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_SND_KITCH2) ? PLU_BM_KP_PRINT2 : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_SND_KITCH3) ? PLU_BM_KP_PRINT3 : 0;
	ulBitMapPluType |= (auchContOther[2] & PLU_SND_KITCH4) ? PLU_BM_KP_PRINT4 : 0;

	ulBitMapPluType |= (auchContOther[6] & PLU_SND_KITCH5) ? PLU_BM_KP_PRINT5 : 0;
	ulBitMapPluType |= (auchContOther[6] & PLU_SND_KITCH6) ? PLU_BM_KP_PRINT6 : 0;
	ulBitMapPluType |= (auchContOther[6] & PLU_SND_KITCH7) ? PLU_BM_KP_PRINT7 : 0;
	ulBitMapPluType |= (auchContOther[6] & PLU_SND_KITCH8) ? PLU_BM_KP_PRINT8 : 0;

	return ulBitMapPluType;
}
#endif

ULONG  ItemSalesItemPluTypeInfoSelected (UCHAR *auchContOther, ULONG ulMaskSelection)
{
	return (ItemSalesItemPluTypeInfo(auchContOther) & ulMaskSelection);
}

/*
 *   Usage Example 1:  if PLU is scalable but is not a minus PLU nor a hash PLU then ...
 *
 *    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&
 *        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
 *         (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {
 *	  }
 *
 *	 becomes
 *
 *   if (ItemSalesItemPluTypeInfoBoolRequired(pPluIf_Dep->ParaPlu.ContPlu.auchContOther, PLU_BM_SCALE_PLU, PLU_BM_MINUS_PLU | PLU_BM_HASH_PLU)) {
 *	 }
 *
 *   Usage Example 2:  if PLU is not a minus PLU nor a hash PLU then ....
 *
 *    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&
 *        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
 *	  }
 *
 *	 becomes
 *
 *   if (ItemSalesItemPluTypeInfoBoolRequired(pPluIf_Dep->ParaPlu.ContPlu.auchContOther, 0, PLU_BM_MINUS_PLU | PLU_BM_HASH_PLU)) {
 *	 }
 *
 *  Usage Example 3: Chained if else. if PLU is a minus PLU and a hash PLU then if MDC set then .. else ...
 *                                    or if PLU is just a hash PLU then ...
 *                                    or if PLU is just a minus PLU then ...
 *                                    else then ...
 *
 *        if ((MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) && 
 *            (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS)) {
 *            if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {
 *                pTable->uchStatus = RPT_CREDIT;
 *            } else {
 *                pTable->uchStatus = RPT_HASH;
 *            }
 *        } else if (MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) {
 *            pTable->uchStatus = RPT_HASH;
 *        } else if (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS) {
 *            pTable->uchStatus = RPT_CREDIT;
 *        } else {
 *            pTable->uchStatus = RPT_OPEN;
 *        }
 *
 *    becomes
 *
 *       switch ( ItemSalesItemPluTypeInfoSelected (MDeptIf.ParaDept.auchControlCode, PLU_BM_MINUS_PLU | PLU_BM_HASH_PLU)) {
 *           case PLU_BM_MINUS_PLU | PLU_BM_HASH_PLU:
 *               if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {
 *                   pTable->uchStatus = RPT_CREDIT;
 *               } else {
 *                   pTable->uchStatus = RPT_HASH;
 *               }
 *               break;
 *           case PLU_BM_HASH_PLU:
 *               pTable->uchStatus = RPT_HASH;
 *               break;
 *           case PLU_BM_MINUS_PLU:
 *               pTable->uchStatus = RPT_CREDIT;
 *               break;
 *           default:
 *               pTable->uchStatus = RPT_OPEN;
 *               break;
 *       }
*/
BOOL  ItemSalesItemPluTypeInfoBoolRequired (UCHAR *auchContOther, ULONG ulMaskSetRequiredOn, ULONG ulMaskSetRequiredOff)
{
	ULONG  ulPluBm = ItemSalesItemPluTypeInfo(auchContOther);

	return (((ulPluBm & ulMaskSetRequiredOn) == ulMaskSetRequiredOn) && ((ulPluBm & ulMaskSetRequiredOff) == 0)) ;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSales(UIFREGSALES *pUifRegSales)
*
*    Input: UIFREGSALES pData
*   Output: None
*    InOut: None
**Return: Return status returned from sub module
*
** Description: This module is the main module of Sales module.
                This module executes the following functions.

                - Set flag bit in sales local data which indicates 0 uunit
                  price was entered.
                - Call Item module's sub modules.
                - Return the status from sub module to U.I.
*===========================================================================
*/
SHORT   ItemSales(UIFREGSALES *pUifRegSales)
{
    ITEMSALES   ItemSales = {0};
	TCHAR       aszDummy[NUM_PLU_SCAN_LEN] = {0};
    SHORT       sReturnStatus;

	// if we are processing an item then we want to make sure that
	// the Check No Purchase indicator is turned off if it was turned on
	// by entering a string with the #/Type key with MDC_CHK_NO_PURCH enabled.
	uchIsNoCheckOn = 0;    // ensure Check No Purchase is off in ItemSales().

	sReturnStatus = ItemCommonCheckCashDrawerOpen ();
	if (sReturnStatus != 0)
		return sReturnStatus;

	if (ItemSalesLocal.fbSalesStatus & SALES_PREAUTH_BATCH) {
		// if we are doing a Preauth Capture from a Preauth Batch transaction then do
		// not allow any changes other than charge tips to be added.
		return(LDT_PROHBT_ADR);
	}

	if (pUifRegSales->uchMinorClass != CLASS_ITEMORDERDEC && (ItemSalesLocal.fbSalesStatus & SALES_TRANSACTION_DISC)) {
		// if operator has done a transaction discount then do
		// not allow any changes to transaction data.
		// we make an exception if this is an Order Declare.
		return(LDT_PROHBT_ADR);
	}
	/*----- check non-gc transaction V3.3 -----*/
    if ( (  sReturnStatus =  ItemCommonCheckNonGCTrans() ) != ITM_SUCCESS ) {
        return(sReturnStatus);
    }

    /*----- check tax exempt key depression  R3.0 -----*/
    if ( (  sReturnStatus =  ItemCommonCheckExempt() ) != ITM_SUCCESS ) {
        return(sReturnStatus);
    }

    /*----- check split check mode R3.1 -----*/
    if ((sReturnStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- set zero input flag -----*/
    if (pUifRegSales->fbInputStatus != 0) {
        ItemSalesLocal.fbSalesStatus |= SALES_ZERO_AMT;        /* inducate 0 price was entered */
    } else {
        ItemSalesLocal.fbSalesStatus &= ~SALES_ZERO_AMT;
    }

    /* inhibit post receipt,  R3.0 */
    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

	if (pUifRegSales->fbInputStatus & INPUT_DEPT) {
		// Department number has been entered so we now need to request
		// a price entry for this item. See FSC_DEPT processing in
		// the function UifItem().
		ULONG   ulPrice;
		pUifRegSales->fbInputStatus &= ~INPUT_DEPT;
		if ((sReturnStatus = ItemSalesGetPriceOnly(&ulPrice)) != ITM_SUCCESS) {
			if (sReturnStatus == ITM_CANCEL) sReturnStatus = UIF_CANCEL;
			return (sReturnStatus);
		}
		pUifRegSales->lUnitPrice = ulPrice;
	}
	if (pUifRegSales->fbInputStatus & INPUT_PRICE) {
		// Price entry has been entered so now we need to request
		// a department number for this item. See FSC_PRICE_ENTER
		// processing in the function UifItem().
		USHORT usDept = 0;
		UCHAR uchMajorFsc = 0, uchMinorFsc = 0;

		pUifRegSales->fbInputStatus &= ~INPUT_PRICE;
		sReturnStatus = ItemSalesGetDeptNo(&usDept, &uchMajorFsc, &uchMinorFsc);
		if (sReturnStatus != ITM_SUCCESS) {
			if (sReturnStatus == ITM_CANCEL) sReturnStatus = UIF_CANCEL;
			return (sReturnStatus);
		}
		pUifRegSales->usDeptNo = usDept;
	}

    /* initialize ppi calculation buffer R3.1 */
    if ( ! ItemSalesLocalSaleStarted() ) {
		// the initialize PPI calculation buffer is called when the first item
		// of a transaction is entered to prepare for any PLUs set up for PPI.
		// PPI records are provisioned with AC71 in PEP to assign PPI rules
		// to the PPI Codes that have been provisioned for individual PLUs.
        ItemSalesCalcInitCom(); /* 2172 */
    }

    /*----- prepare I/F data for sub function -----*/
    memset(&ItemSales, '\0', sizeof(ITEMSALES));    /* intialize sales data */
    /*----- call functions depends on minor calss -----*/
    ItemSales.uchMajorClass = CLASS_ITEMSALES;

	{
		USHORT   usReturnType;
		ULONG    ulTrnQualModifierReturnData = 0;

		ItemSales.usReasonCode = 0;
		ulTrnQualModifierReturnData = TrnQualModifierReturnData (&usReturnType, &(ItemSales.usReasonCode));
		if (ulTrnQualModifierReturnData & CURQUAL_PRETURN) {
			switch (usReturnType) {
				case 1:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_1);
					ItemSales.fbReduceStatus |= REDUCE_RETURNED;
					break;

				case 2:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_2);
					ItemSales.fbReduceStatus |= REDUCE_RETURNED;
					break;

				case 3:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_3);
					ItemSales.fbReduceStatus |= REDUCE_RETURNED;
					break;
			}
		} else if (ulTrnQualModifierReturnData & CURQUAL_TRETURN) {
			ItemSales.fbReduceStatus |= REDUCE_RETURNED;
			switch (usReturnType) {
				case 1:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_1);
					break;

				case 2:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_2);
					break;

				case 3:
					pUifRegSales->fbModifier = ItemSales.fbModifier |= (RETURNS_MODIFIER_3);
					break;
			}
		}
	}

    switch(pUifRegSales->uchMinorClass) {
    case CLASS_UIDEPT:
        ItemSales.uchMinorClass = CLASS_DEPT;
        sReturnStatus = ItemSalesDept(pUifRegSales, &ItemSales);
		break;

    case CLASS_UIPLU:
    case CLASS_UIDISPENSER:
	case CLASS_ITEMORDERDEC:
        sReturnStatus = ItemSalesLabelProc(pUifRegSales, &ItemSales);
//		if (sReturnStatus == ITM_SUCCESS) {
//			BlFwIfDisplayOEPImagery (2, ItemSales.usDeptNo, ItemSales.uchTableNumber, ItemSales.uchGroupNumber);
//		}
		break;

        /* ItemSales.uchMinorClass = CLASS_PLU;
        return(ItemSalesPLU(pUifRegSales, &ItemSales)); */

    case CLASS_UIMODDISC:
        /* V3.3 */
        if ((pUifRegSales->usDeptNo == 0) &&
            (_tcsncmp(pUifRegSales->aszPLUNo, aszDummy, NUM_PLU_SCAN_LEN) == 0) &&
            (pUifRegSales->uchFsc == 0)) {
            /* cursor modifiered discount */
            sReturnStatus = ItemSalesCursorModDisc(pUifRegSales, &ItemSales);

        } else if ((pUifRegSales->uchMajorFsc == FSC_DEPT) || (pUifRegSales->uchMajorFsc == FSC_KEYED_DEPT)) {
            ItemSales.uchMinorClass = CLASS_DEPTMODDISC;

            /* dept modifiered discount */
            sReturnStatus = ItemSalesDept(pUifRegSales, &ItemSales);
        } else {
            /* label modifiered discount */
            sReturnStatus = ItemSalesLabelProc(pUifRegSales, &ItemSales);
        }
        break;

    case CLASS_UIPRINTMODIFIER:
        sReturnStatus = ItemSalesPrintMod(pUifRegSales);
		break;

    case CLASS_UIDEPTREPEAT:
    case CLASS_UIPLUREPEAT:
        sReturnStatus = ItemSalesRepeat(pUifRegSales);
		break;

    case CLASS_UIREPEAT:    /* R3.1 */
        switch(ItemSalesRepeatCheck(pUifRegSales, &ItemSales)) {
        case CLASS_UIDEPTREPEAT:
        case CLASS_UIPLUREPEAT:
            sReturnStatus = ItemSalesRepeat(pUifRegSales);
			break;

        case CLASS_DEPT:                    /* dept */
        case CLASS_PLU:                     /* PLU */
        case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
        case CLASS_SETMENU:                 /* setmenu */
            sReturnStatus = ItemSalesCursorRepeat(pUifRegSales, &ItemSales);
			break;

        default:
            sReturnStatus = LDT_PROHBT_ADR;
			break;
        }
        break;

    case CLASS_UIPRICECHECK:    /* R3.1 */
        sReturnStatus = ItemSalesPriceCheck(pUifRegSales, &ItemSales);
		break;

	case CLASS_UIFOR: //SR 143 @/For key cwunn
		//This isn't a complete sale until a dept/PLU is entered,
		// so store in.previous item bufer
		ItemPreviousSaveForKeyData (pUifRegSales);
        sReturnStatus = ITM_SUCCESS;    /* return after sending */
		break;

    default:    /* not use */
		sReturnStatus = LDT_PROHBT_ADR;
        break;
    }

	if (sReturnStatus <= ITM_SUCCESS) {
		// indicate that we should not allow a sign-out at this time.
		// this will prevent the operator from accidently pressing the
		// wrong key that will cause them to sign out with a transaction
		// still pending causing a PifAbort() when they try to sign back
		// in and causing problems with the guest check file as well.
		// When the check is tendered, the status bits will be cleared.
		ItemCommonPutStatus (COMMON_PROHIBIT_SIGNOUT);
	}

    return sReturnStatus;
}
/*
*===========================================================================
*    Synopsis: SHORT   ItemSalesEditCondiment(UCHAR uchType)
*
*    Input: UCHAR uchType
*    Output: None
*    InOut: None
*    Return: Return status returned from sub module
*
*    Description: This module executes the following functions.
*
*    - Send current item that is highlighted (ItemSales) into the
	   correctfunctionality. This function is only used when the
	   user has pressed the "Condiment Edit" FSC.  This key is used
	   with OEP items, because with OEP items, the condiments are
	   not visible on the screen unless the user chooses that
	   particular item first.										  SR 192
*===========================================================================
*/
SHORT   ItemSalesEditCondiment(UCHAR uchType)
{
	USHORT		CondNum = 0, sI = 0;
	SHORT   sQty = 1;
    SHORT   sResult;
	USHORT          usLinkNo;
    USHORT  fsType, usContinue = 0, usNoOfGroup = 1; /* dummy */
    UCHAR   szGroups[ITM_COND_NUM_GRPTBL + 1], actualGroup[ITM_COND_NUM_GRPTBL + 1];    /* increased TAR #129199 */
	UCHAR           uchRestrict, uchDummy;
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();
	ITEMSALES	    ItemSales = {0};
	ITEMSALES       WorkItem = {0};
	UIFREGSALES     WorkReg = {0};

	//We pull the item that the cursor is currently on, and send that information
	//into the specific functionality that is specified by uchType.
	MldGetCursorDisplay(MLD_SCROLL_1, &ItemSales, &CondNum, MLD_GET_COND_NUM);

	//We have an Item we need to check if it is valid or not
	//Then we can perform the Condiment Edit.
	switch(uchType)
	{
		case FSC_EDIT_COND_TBL:
			//Prepare the Work Buffers for this functionality
			//We are going to need two buffers on for the condiment we
			//are editing and one or the Item.
			memset(&WorkReg, 0, sizeof(WorkReg));
			memset(&WorkItem, 0, sizeof(WorkItem));

			WorkReg.uchMajorClass = ItemSales.uchMajorClass;
			WorkReg.uchMinorClass = ItemSales.uchMinorClass;
			_tcsncpy(WorkReg.aszPLUNo, ItemSales.auchPLUNo, NUM_PLU_LEN);
   			WorkReg.uchAdjective= ItemSales.uchAdjective;
			WorkReg.lQTY         = ItemSales.lQTY;
			WorkReg.fbModifier   = ItemSales.fbModifier;

			WorkReg.fbModifier2 = ItemSales.fbModifier2;

			if(ItemSales.uchCondNo > 0){
				//Set up Condiment Buffer
				WorkItem.uchMajorClass = CLASS_ITEMSALES;
				WorkItem.uchMinorClass = CLASS_PLU;
				_tcsncpy(WorkItem.auchPLUNo, ItemSales.Condiment[CondNum-1].auchPLUNo, NUM_PLU_LEN);

				//Get correct Condiment information if this function fails use
				//the Non Table Condiment Edit Functionality.
				if ((sResult = ItemSalesPrepare(&WorkReg,
					&WorkItem, &uchRestrict, &usLinkNo, &uchDummy)) != ITM_SUCCESS) {
						ItemSalesEditCondiment(FSC_EDIT_CONDIMENT);
						return SUCCESS;
				}
			} else {
				ItemSalesEditCondiment(FSC_EDIT_CONDIMENT);
				return SUCCESS;
			}
			//clear the buffers we will be using, so we dont get incorrect data
			memset(szGroups, 0x00, sizeof(szGroups));
			memset(actualGroup, 0x00, sizeof(actualGroup));

			//Pass the PLU number into this function so that we get the correct
			//group numbers to use when looking up PLUs to populate the OEP
			//window with. If this function fails it has no table sequence
			//so yuse the group functionality.
			if ((sResult = ItemSalesOEPUpdateGroupNoCondiment(WorkItem.auchPLUNo, // 2172
													 0,
													 szGroups,
													 0,
													 &fsType,
													 usContinue,
													 usNoOfGroup,
													 WorkItem.uchTableNumber)) != ITM_SUCCESS) {
				ItemSalesEditCondiment(FSC_EDIT_CONDIMENT);
				return SUCCESS;
			}

			for(sI = 0; sI < sizeof(szGroups); sI++){
				if(szGroups[sI] == SLOEP_SELFROMKEY)
				{
					actualGroup[sI] = SLOEP_DUMMY;
				}else
				{
					actualGroup[sI] = szGroups[sI];
				}
			}

			//if the cursor is on a condiment, then we will only
			//populate the OEP window with that Condiment items group
			if(CondNum) //We are choosing a certain condiment to edit
			{
				if(actualGroup[0])
				{
					//set this static variable to OEP_CONDIMENT_EDIT
					//for use in other functions later.
					pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_ADD;
					ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &WorkItem, actualGroup);
				}else
				{
					return ITM_CONT;
				}
			}else //Browse all of the OEP options
			{
				//We are adding a brand new condiment to the item,
				//so we send the entire Group list into the function
			   	pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_ADD;
				ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &WorkItem, actualGroup);
			}
			break;

		case FSC_EDIT_CONDIMENT:
			//Prepare the Work Buffers for this functionality
			//We are going to need two buffers on for the condiment we
			//are editing and one or the Item.
			if (ItemSales.fbModifier & VOID_MODIFIER) {
				return(LDT_NTINFL_ADR);
			}

			memset(&WorkReg, 0, sizeof(WorkReg));
			memset(&WorkItem, 0, sizeof(WorkItem));

			WorkReg.uchMajorClass = ItemSales.uchMajorClass;
			WorkReg.uchMinorClass = ItemSales.uchMinorClass;
			_tcsncpy(WorkReg.aszPLUNo, ItemSales.auchPLUNo, NUM_PLU_LEN);
   			WorkReg.uchAdjective= ItemSales.uchAdjective;
			WorkReg.lQTY         = ItemSales.lQTY;
			WorkReg.fbModifier   = ItemSales.fbModifier;

			WorkReg.fbModifier2 = ItemSales.fbModifier2;

			if(CondNum > 0){
				//Set up Condiment Buffer
				WorkItem.uchMajorClass = CLASS_ITEMSALES;
				WorkItem.uchMinorClass = CLASS_PLU;
				_tcsncpy(WorkItem.auchPLUNo, ItemSales.Condiment[CondNum-1].auchPLUNo, NUM_PLU_LEN);

				//Get correct Condiment information if this function fails use
				//the Non Table Condiment Edit Functionality.
				if ((sResult = ItemSalesPrepare(&WorkReg, &WorkItem, &uchRestrict, &usLinkNo, &uchDummy)) != ITM_SUCCESS) {
//						ItemSalesEditCondiment(FSC_EDIT_CONDIMENT);  remove this for now as can cause stack overflow due to recursion
//						return SUCCESS;
						return ITM_SUCCESS;
				}
			} else {
				;
			}

			//clear the buffers we will be using, so we dont get incorrect data
			memset(szGroups, 0x00, sizeof(szGroups));
			memset(actualGroup, 0x00, sizeof(actualGroup));

			//Pass the PLU number into this function so that we get the correct
			//group numbers to use when looking up PLUs to populate the OEP
			//window with.
			if(CondNum){
				//Pass the PLU number into this function so that we get the correct
				//group numbers to use when looking up PLUs to populate the OEP
				//window with. If this function fails it has no table sequence
				//so yuse the group functionality.
				if ((sResult = ItemSalesOEPUpdateGroupNoCondiment(WorkItem.auchPLUNo, // 2172
														 0,
														 szGroups,
														 0,
														 &fsType,
														 usContinue,
														 usNoOfGroup,
														 WorkItem.uchTableNumber)) != ITM_SUCCESS) {
						if(WorkItem.uchGroupNumber){
							//szGroups[0] = WorkItem.uchGroupNumber;
						}else{
							return (ITM_CONT);
						}
				}
			}
			else{
					if ((sResult = ItemSalesOEPUpdateGroupNoCondiment(ItemSales.auchPLUNo, // 2172
													 0,
													 szGroups,
													 0,
													 &fsType,
													 usContinue,
													 usNoOfGroup,
													 ItemSales.uchTableNumber)) != ITM_SUCCESS) {
							return (ITM_CONT);
					}
			}

			for(sI = 0; sI < sizeof(szGroups); sI++)
			//while(szGroups[sI] > 90)
			{
				if(szGroups[sI] == SLOEP_SELFROMKEY)
				{
					actualGroup[sI] = SLOEP_DUMMY;
				}else
				{
					actualGroup[sI] = szGroups[sI];
				}
			}
			//if the cursor is on a condiment, then we will only
			//populate the OEP window with that Condiment items group
			if(CondNum) //We are choosing a certain condiment to edit
			{
				if(WorkItem.uchGroupNumber){
					//set this static variable to OEP_CONDIMENT_EDIT
					//for use in other functions later.
					pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_EDIT;
					ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &ItemSales, &WorkItem.uchGroupNumber);
				}else if(ItemSales.uchGroupNumber){
					//set this static variable to OEP_CONDIMENT_EDIT
					//for use in other functions later.
					pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_EDIT;
					ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &ItemSales, &ItemSales.uchGroupNumber);
				}else if(actualGroup[0])
				{
					//set this static variable to OEP_CONDIMENT_EDIT
					//for use in other functions later.
					pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_EDIT;
					ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &ItemSales, actualGroup);
				}else
				{
					return ITM_CONT;
				}
			}else //Browse all of the OEP options
			{
				//We are adding a brand new condiment to the item,
				//so we send the entire Group list into the function
			    pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_ADD;
				ItemSalesOEPNextPluCondiment(&UifRegData.regsales, &ItemSales, actualGroup);
			}
			break;

		//the option has been added to move the condiment in the list up or down
		//this is the option that just moves the condiment up in the list.
		case FSC_SCROLL_UP:
			if((CondNum) &&(ItemSales.uchCondNo > 1))
			{
				sResult = ItemPreviousCondiment(&ItemSales.Condiment[CondNum - 1], MLD_CURSOR_CHANGE_UP);
				if( sResult != SUCCESS )
				{
					return sResult;

				}
				break;
			} else //There is no condiment, we are just moving the cursor up
			{
				return MLD_CURSOR_CHANGE_UP;
			}

		//the option has been added to move the condiment in the list up or down
		//this is the option that just moves the condiment down in the list.
		case FSC_SCROLL_DOWN:
			if((CondNum) &&(ItemSales.uchCondNo > 1))
			{
				sResult = ItemPreviousCondiment(&ItemSales.Condiment[CondNum - 1], MLD_CURSOR_CHANGE_DOWN);
				if( sResult != SUCCESS )
				{
					return sResult;

				}
			}else //this is just a regular cursor down call.
			{
				return MLD_CURSOR_CHANGE_DOWN;
			}

			break;
	}

	//if this variable is not set to zero, set it to zero now
	//because we are done with it.
	if(pWorkCur->fbOepCondimentEdit)
	{
		pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_RESET;
	}

	//If the OEP window is still up for some reason, close it.
	if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
        MldDeletePopUp();
		ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
	}
	return SUCCESS;
}


/*
*===========================================================================
*    Synopsis: SHORT   ItemSalesOrderDeclaration(VOID)
*
*    Input: NONE
*    Output: None
*    InOut: None
*    Return: Return status returned from sub module
*
*    Description: This module executes the following functions.
*
*	 We Set the type of transaction to a specific option, then inform KDS,KPS
*	 and any other system that may need to know that information JHHJ
*===========================================================================
*/
SHORT ItemSalesOrderDeclaration(UIFREGSALES pUifRegSales, UCHAR uchOrderDecNum,USHORT usType)
{
	USHORT			usOrderDecNum = TRN_ORDER_DEC_OFFSET, usGuestNo = 0;
	SHORT           sRetStatus;

	// perform a check to see if this is a return transaction.
	// if it is then prohibit the new items.  we have found
	// in testing (Dec-11-2011) that new items added to the
	// transaction are not included in the totals update.
	// This is a quick way to correct this problem by not allowing it.
	if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
		// check MDC to see if adding items to a Return Transaction is allowed
		if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_A) == 0)
			return LDT_PROHBT_ADR;
	}

	sRetStatus = ItemCommonCheckCashDrawerOpen ();
	if (sRetStatus != 0)
		return sRetStatus;

	if((usType == CLASS_ORDER_DEC_NEW) || (usType == CLASS_ORDER_DEC_FSC))
	{
			//do a switch on the type of status the operator has, whether it
			//be force a choice of order declaration, or use the terminal default
			if(usType != CLASS_ORDER_DEC_FSC)
			{
				switch(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_2] & (CAS_ORDER_DEC_FORCE | CAS_ORDER_DEC_DEFAULT)){
					case CAS_ORDER_DEC_FORCE:
						if((TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
						{
							UieCopyKeyMsg();
							return LDT_ORDER_DEC_EVENT;
						}
						break;
					case CAS_ORDER_DEC_DEFAULT:
						//if we have not already chosen an order declaration
						//then we will find out the default set for this terminal
						//and use that JHHJ
						if((TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
						{
							PARATERMINALINFO		myTerminalInfo = {0};

							myTerminalInfo.uchMajorClass = CLASS_PARATERMINALINFO;
							myTerminalInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
							ParaTerminalInfoParaRead (&myTerminalInfo);
							uchOrderDecNum = myTerminalInfo.TerminalInfo.uchOrderDeclaration; // using default terminal specific order declare number
							if(uchOrderDecNum > 0)
							{
								pUifRegSales.uchMinorClass = CLASS_ITEMORDERDEC;
								pUifRegSales.uchMajorFsc = FSC_ORDER_DEC;   // set order declare fsc
								pUifRegSales.uchFsc = uchOrderDecNum;       // set order declaration number as extended fsc
								sRetStatus = ItemSales(&pUifRegSales); /* Sales Modele */
							}
						}
						return SUCCESS;
					default:
						break;
				}
			}

			if(! ItemSalesLocalSaleStarted() && !(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY))
			{
				if((TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
				{
					if(uchOrderDecNum == 0)                   // determine if we should use terminal default
					{
						PARATERMINALINFO		myTerminalInfo = {0};

						myTerminalInfo.uchMajorClass = CLASS_PARATERMINALINFO;
						myTerminalInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
						ParaTerminalInfoParaRead (&myTerminalInfo);
						uchOrderDecNum = myTerminalInfo.TerminalInfo.uchOrderDeclaration; // using default terminal specific order declare number
					}

					pUifRegSales.uchMinorClass = CLASS_ITEMORDERDEC;
					pUifRegSales.uchMajorFsc = FSC_ORDER_DEC;         // set order declare fsc
					pUifRegSales.uchFsc = uchOrderDecNum;             // set order declaration number as extended fsc
					if(uchOrderDecNum > 0)
					{
						sRetStatus = ItemSales(&pUifRegSales); /* Sales Module */
						return sRetStatus;
					}
				}
			}
			sRetStatus = SUCCESS;
	} else if(usType == CLASS_ORDER_DEC_MID)
	{
		ITEMSALES		myItemSales = {0};

        ItemPreviousItem(0, 0);

		usOrderDecNum += uchOrderDecNum;

		myItemSales.uchMajorClass = CLASS_ITEMSALES;
		myItemSales.uchMinorClass = CLASS_ITEMORDERDEC;
		myItemSales.lQTY = 1000;
		myItemSales.fsPrintStatus |= (PRT_SLIP | PRT_JOURNAL);
		// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
		// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
		// uncompressed portion of the record, TRANSTORAGESALESNON.
		//    Richard Chambers, Dec-21-2018
		myItemSales.usDeptNo = myItemSales.uchAdjective = uchOrderDecNum;    // uchAdjective is reused as order declare number, CLASS_ITEMORDERDEC

		pUifRegSales.uchMajorClass = CLASS_ITEMSALES;
		pUifRegSales.uchMinorClass = CLASS_ITEMORDERDEC;
		pUifRegSales.uchFsc = uchOrderDecNum;
		RflGetTranMnem (myItemSales.aszMnemonic, usOrderDecNum);
		/*----- Get Order# in case of Transaction Open -----*/
//		if (!(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)) {
//			MldScrollWrite(&myItemSales, MLD_NEW_ITEMIZE);
//		}else
//		{
//			MldScrollWrite(&myItemSales, MLD_ORDER_DECLARATION);
//		}

//		if((pWorkCur->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0) {
			// We have not processed an order declare before.
			// Lets go ahead and submit one 
			pUifRegSales.uchMinorClass = CLASS_ITEMORDERDEC;
			pUifRegSales.uchMajorFsc = FSC_ORDER_DEC; /* set plu fsc */
			pUifRegSales.uchFsc = uchOrderDecNum;
			sRetStatus = ItemSales(&pUifRegSales); /* Sales Modele */
//		}
#if 0
		else {
			ITEMSALES		myItemSales;

			memset(&myItemSales, 0x00, sizeof(myItemSales));
			myItemSales.uchMajorClass = CLASS_ITEMSALES;
			myItemSales.uchMinorClass = CLASS_ITEMORDERDEC;
			myItemSales.lQTY = 1000;
			myItemSales.fsPrintStatus |= (PRT_SLIP | PRT_JOURNAL);

			// We already have process an order declare.
			// lets just update the previous one with the new
			// order declare number
			pWorkCur->uchOrderDec = uchOrderDecNum;
			pWorkCur->fsCurStatus2 |= CURQUAL_ORDERDEC_DECLARED;

			_tcsncpy(pUifRegSales.aszPLUNo, tchPLU, NUM_PLU_LEN);
			_tcsncpy(myItemSales.auchPLUNo, tchPLU, NUM_PLU_LEN);
			myItemSales.usDeptNo = myItemSales.uchAdjective = uchOrderDecNum;    // uchAdjective is reused as order declare number, CLASS_ITEMORDERDEC
			myItemSales.usUniqueID = 1; //unique id for CLASS_ITEMORDERDEC order declaration items

			ItemSalesPrepare(&pUifRegSales,&myItemSales,0,0,0);
			TrnStoSalesItemUpdate(&myItemSales);
			ItemSendKdsOrderDeclaration(&myItemSales);
			sRetStatus = SUCCESS;
		}
#endif
	}

	return SUCCESS;
}

	/*
*===========================================================================
*    Synopsis: SHORT   ItemSalesAdjModify(UIFREGSALES *pUifRegSales)
*
*    Input: NONE
*    Output: None
*    InOut: None
*    Return: Return status returned from sub module
*
*    Description: This module executes the following functions.
*
*	 We Set the type of transaction to a specific option, then inform KDS,KPS
*	 and any other system that may need to know that information JHHJ
*===========================================================================
*/
SHORT ItemSalesAdjModify(UIFREGSALES *pUifRegSales)
{
	USHORT		  usScroll, usCondNumber = 0;
	SHORT		  sReturnStatus =0, qtyHolder = 0;
    SHORT         sQty = 0, sStorage;
	ULONG         ulUnitPrice;
	USHORT		  usRet;
	ITEMSALES	  ItemSales = {0}, CondimentWorkArea = {0};
	UIFREGSALES   test = {0};
	PLUIF         PLURecRcvBuffer = {0};        /* PLU record receive buffer */
	UCHAR         uchTest = 0;

	ItemPreviousItem (0, 0); //push previous item to transaction file

	usScroll = MldQueryCurScroll();			//determine which screen we are using
	MldGetCursorDisplay(usScroll, &ItemSales, &usCondNumber, MLD_GET_SALES_ONLY);

	if(!ItemSales.uchAdjective || (ItemSales.fbModifier & VOID_MODIFIER))
	{
		return LDT_PROHBT_ADR;
	}

	sStorage = TrnDetermineStorageType();

//    sQty = abs((SHORT)(ItemSales.lQTY / 1000L));
//    if ((sStatus = TrnVoidSearch(&ItemSales, sQty, sStorage)) != TRN_SUCCESS) {
//        return(sStatus);
//    }

	CondimentWorkArea = ItemSales;
	CondimentWorkArea.lProduct = 0 - ItemSales.lProduct;
	CondimentWorkArea.lQTY = 0 - ItemSales.lQTY;

	//we get the new adjective number for the item
	usRet = ItemSalesOEPAdjective(&test, &ItemSales, &uchTest);
	ItemSales.uchAdjective = (UCHAR) usRet;
	pUifRegSales->uchAdjective = (UCHAR) usRet;

	usRet = ItemSalesAdj(pUifRegSales, &ItemSales);
	if(usRet != SUCCESS)
	{
		return usRet;
	}

	//update PRICE!!!!
     _tcsncpy(PLURecRcvBuffer.auchPluNo, ItemSales.auchPLUNo, NUM_PLU_LEN);
    if ((pUifRegSales->uchAdjective - 1) % 5 + 1) {
	    PLURecRcvBuffer.uchPluAdj = (pUifRegSales->uchAdjective - 1) % 5 + 1;
	} else {
	    PLURecRcvBuffer.uchPluAdj = 1;      /* dummy adjective valiation */
	}

    sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0);
	RflConv3bto4b(&ulUnitPrice,PLURecRcvBuffer.ParaPlu.auchPrice);
	ItemSales.lUnitPrice = ulUnitPrice;
	ItemSales.lProduct = ItemSales.lUnitPrice * ((LONG)ItemSales.lQTY / 1000L);

	{
		UCHAR  uchMajorClassSave = ItemSales.uchMajorClass;  // save major class to change it temporarily below.

		// In the following code section, we will use the function MldScrollFileWrite()
		// to update the MLD file then use the function MldScrollWrite() to
		// update the displayed receipt window.
		// WARNING: This order must be maintained because the function MldScrollWrite()
		//          will update the scrolling region data structure and that data struct
		//          is used by MldScrollFileWrite () to determine the item number to modify.
		//          If MldScrollWrite() is called first then the line where the current cursor
		//          is located will have its item number removed and it will be zero if the
		//          condiment is the last condiment of the last item.
		ItemSales.uchMajorClass = CLASS_ITEMPREVCOND;		//Major class to preform requested previous condiment function
		MldScrollFileWrite(&ItemSales);
	
		ItemSales.uchMajorClass = uchMajorClassSave;		//Major class to preform requested previous condiment function
	}

	TrnStoSalesItemUpdate(&ItemSales);
    MldScrollWrite(&ItemSales, MLD_CONDIMENT_UPDATE);

	TrnSalesPLU(&CondimentWorkArea);

    ItemCommonDispSubTotal(&ItemSales);
	ItemSendKdsSales(&ItemSales, 0);
	TrnSalesPLU(&ItemSales);

	return SUCCESS;

}
/****** end of file ******/
