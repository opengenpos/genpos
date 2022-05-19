/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Modifier operation for Dept./ Open PLU sales                           
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLMOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemSalesModifier()  
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-18-92: 00.00.01  : K.Maeda   : initial
* Oct-19-93: 02.00.02  : K.You     : del. canada tax feature.
* Feb-01-96: 03.01.00  : M.Ozawa   : recover canadian tax.
* Aug-19-98: 03.03.00  : hrkato    : V3.3 (VAT/Service)
*
***GenPOS Rel 2.2
* Jun-07-16: 02.02.01  : R.Chambers : localized variables
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

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "appllog.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesModifier(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: Modify the sales item with Void, Tax modifier, Number key entry
*               depending on which modifier keys were pressed before item entry.
*===========================================================================
*/
SHORT   ItemSalesModifier(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    /*--- reverse sign if void key depressed ---*/
    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
		ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

        pItemSales->fbModifier = pCommonLocalRcvBuff->ItemSales.fbModifier;
        
        /* set price check status, 2172 */
        pItemSales->fbModifier2 = (UCHAR)(pUifRegSales->fbModifier2 & PCHK_MODIFIER);
        /* copy void, tax modifier */
    } else {
        if (pUifRegSales->uchMinorClass == CLASS_UIDISPENSER) {
            pItemSales->fbModifier = 0;     /* not set void flag at beverage dispenser sales */
            pItemSales->fbModifier2 = 0;
        } else {
            pItemSales->fbModifier = (pUifRegSales->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3));
			if (pItemSales->fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
				pItemSales->fbReduceStatus |= REDUCE_RETURNED;
			}
            /* set price check status, 2172 */
            pItemSales->fbModifier2 = (UCHAR)(pUifRegSales->fbModifier2 & (PCHK_MODIFIER|SKU_MODIFIER));
        }

        /*--- set tax mod. status of modifier local data ---*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
			// See function ItemModTaxCanada() which handles the modifier key for Canadian Tax System
            ItemDiscGetCanTaxMod(pItemSales);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {           /* V3.3 */
			// See function ItemModTaxUS() which handles the modifier key for US Tax System
			pItemSales->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE1) ? TAX_MODIFIER1 : 0;
			pItemSales->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE2) ? TAX_MODIFIER2 : 0;
			pItemSales->fbModifier |= (ItemModLocalPtr->fsTaxable & MOD_TAXABLE3) ? TAX_MODIFIER3 : 0;
            if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
                pItemSales->fbModifier |= FOOD_MODIFIER;
                pItemSales->ControlCode.auchPluStatus[1] ^= PLU_AFFECT_FS;
            }
        }
		// so why is the VAT I'ntl tax system not handled here?
		// Looks like tax modify is not allowed with VAT I'ntl Tax System, see function ItemModTax().
    }

    /*--- process for number key operation ---*/
    _tcsncpy(pItemSales->aszNumber[0], pUifRegSales->aszNumber, STD_NUMBER_LEN);
	pItemSales->aszNumber[0][STD_NUMBER_LEN] = 0;     // ensure end of string terminator exists.
    
    return(ITM_SUCCESS);
}

/****** end of file ******/
