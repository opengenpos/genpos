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
* Title       : Modifier operation for Item discount amount.                          
* Category    : Dicount Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: ITDSCMOD.C
* --------------------------------------------------------------------------
* Abstract: ItemDiscItemDiscModif() : Modifier operation for Item discount    
*           ItemDiscModif()         : Modifier operation for Reg. discount/ 
*                                     Charge tip    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Feb-14-95: 03.00.00  :   hkato   : R3.0
* Nov-09-95: 03.01.00  :   hkato   : R3.1
* Aug-18-98: 03.03.00  :  hrkato   : V3.3 (VAT/Service)
* Dec-05-02: 01.04.00  :  cwunn      : Add ItemChargeTipsOpen() for GSU SR 6
*										(allow void charge tips w/0 itemizer)
* Jun-23-16: 02.02.01  : R.Chambers  : localized variables, defines for magic constants in ItemDiscItemDiscModif().
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
#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "rfl.h"
#include    "uie.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemDiscItemDiscModif(ITEMDISC *pDiscData)
*
*    Input: ITEMDISC *pDiscData, 
*   Output: None
*    InOut: None
**Return: ITM_SUCCESS    : Function Performed succesfully
*         
** Description: Tax Mod, Number entry for Item discount. 
*===========================================================================
*/
SHORT   ItemDiscItemDiscModif(ITEMDISC *pItemDisc)
{
    USHORT fbWorkNounTax;
    USHORT fbWorkDiscTax;
    USHORT fbModWork;
    SHORT  sReturnStatus;
    ITEMCOMMONLOCAL    *pCmn = ItemCommonGetLocalPointer();
//    ITEMMODLOCAL       ModLocalRcvBuff;

    /*--- TAX MODIFIER PROCCESS ---*/
//    ItemModGetLocal(&ModLocalRcvBuff);      /* get tax modifier data */
    ItemDiscGetTaxMDC(pItemDisc);           /* put MDC option into pItemDisc */
            
    switch(pItemDisc->uchMinorClass) {
    case CLASS_ITEMDISC1:
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
                                            /* get noun */
            if ((pCmn->ItemSales.fbModifier & TAX_MOD_MASK) != 0) {
                fbWorkNounTax = (USHORT)((pCmn->ItemSales.fbModifier & TAX_MOD_MASK) >> 1);
                fbWorkNounTax -= 1;
            } else {
                fbWorkNounTax = (USHORT) (pCmn->ItemSales.ControlCode.auchPluStatus[1] & 0x0f);
            }

            pItemDisc->auchDiscStatus[2]  |= (fbWorkNounTax << 4);     // lower nibble used for discount itemizer, upper nibble for Canadian tax
            ItemDiscGetCanTaxMod(pItemDisc);    /* copy canadian tax mod. into pItemDisc */

            if ((pItemDisc->fbDiscModifier & TAX_MOD_MASK) != 0) {
                fbModWork = ((pItemDisc->fbDiscModifier & TAX_MOD_MASK) >> 1);
                pItemDisc->auchDiscStatus[2] &= 0x0f;  
                pItemDisc->auchDiscStatus[2] |= ((fbModWork - 1) << 4);   // lower nibble used for discount itemizer, upper nibble for Canadian tax
            }

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {           /* V3.3 */
            /*--- DISCOUNT's TAX MODIFIER PROCESS ---*/
            pItemDisc->fbDiscModifier |= (ItemModLocalPtr->fsTaxable << 1);    /* justify void bit */
            /*------------------------------------------------------------
            |         Tax staus data                     |  Bit positon   |
            |============================================+================|
            |   ItemModLocalPtr->fsTaxable (modifier)    |    .....XXX    |
            |--------------------------------------------+----------------|
            |   pItemDisc->fbModifier                    |    ....XXX.    |
            |--------------------------------------------+----------------|
            |   pItemDisc->auchStatus[0]  (disc's MDC)   |    ...XXX..    |
            |--------------------------------------------+----------------|
            |   CommonLocalRcvBuff.ItemSales.            |                |
            |   ControlCode.auchStatus[1]     (Noun)     |    .....XXX    |
             ------------------------------------------------------------*/
            
            /* justify for tax para */
            fbWorkDiscTax = (((pItemDisc->fbDiscModifier & TAX_MOD_MASK) << 1) ^ (pItemDisc->auchDiscStatus[0] & DISC_MDC_TAX_MASK));    
                                            /* modified tax status */
                                            /* get noun */
            fbWorkNounTax = ((pCmn->ItemSales.ControlCode.auchPluStatus[1] & PLU_DISC_TAX_MASK) ^ ((pCmn->ItemSales.fbModifier & TAX_MOD_MASK) >> 1));
            
            pItemDisc->auchDiscStatus[0] &= ~DISC_MDC_TAX_MASK;                      /* turn off current tax MDC */
            pItemDisc->auchDiscStatus[0] |= fbWorkDiscTax & (fbWorkNounTax << 2);    /* modified complete */
            pItemDisc->auchDiscStatus[0] |= fbWorkDiscTax & (fbWorkNounTax << 2);

            if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) { /* Saratoga */
                pItemDisc->fbDiscModifier |= FOOD_MODIFIER;
                pItemDisc->auchDiscStatus[1] ^= 0x04;
            }
            if ((pCmn->ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_FS) && (pItemDisc->auchDiscStatus[1] & 0x04)) {
                pItemDisc->auchDiscStatus[1] |= 0x04;   // DISC_AFFECT_FOODSTAMP
            } else {
                pItemDisc->auchDiscStatus[1] &= ~0x04;
            }

        } else {                        /* V3.3 */
            switch(pCmn->ItemSales.ControlCode.auchPluStatus[1] & 0x03) {
            case    INTL_VAT1:
				{
					USHORT  usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID11_ADR);  /* address 31 */

					if (CliParaMDCCheckField (usAddress, MDC_PARAM_BIT_B)) {   // MDC_MODID11_ADR and others
						pItemDisc->auchDiscStatus[0] &= ~ DISC_MDC_TAX_MASK;
						pItemDisc->auchDiscStatus[0] |= (INTL_VAT1 << 2);
					}
				}
                break;

            case    INTL_VAT2:
				{
					USHORT  usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID11_ADR);  /* address 31 */

					if (CliParaMDCCheckField (usAddress, MDC_PARAM_BIT_A)) {   // MDC_MODID11_ADR and others
						pItemDisc->auchDiscStatus[0] &= ~ DISC_MDC_TAX_MASK;
						pItemDisc->auchDiscStatus[0] |= (INTL_VAT2 << 2);
					}
				}
                break;

            case    INTL_VAT3:
				{
					USHORT  usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID12_ADR);  /* address 32 */

					if (CliParaMDCCheckField (usAddress, MDC_PARAM_BIT_D)) {   // MDC_MODID11_ADR and others
						pItemDisc->auchDiscStatus[0] &= ~ DISC_MDC_TAX_MASK;
						pItemDisc->auchDiscStatus[0] |= (INTL_VAT3 << 2);
					}
				}
                break;

            default:
                break;
            }
            if (pCmn->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_VAT_SERVICE) {
                pItemDisc->auchDiscStatus[2] |= ITM_DISC_AFFECT_SERV;
            }
        }
        sReturnStatus = ITM_SUCCESS;
        break;
    
    case CLASS_REGDISC1:
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:                /* R3.1 */
    case CLASS_REGDISC4:                /* R3.1 */
    case CLASS_REGDISC5:                /* R3.1 */
    case CLASS_REGDISC6:                /* R3.1 */
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA ||
            ItemCommonTaxSystem() == ITM_TAX_INTL) {
            if (ItemModLocalPtr->fsTaxable != 0) {  
                sReturnStatus = LDT_SEQERR_ADR;
                break;
            }
        }

        /*--- DISCOUNT's TAX MODIFIER PROCESS ---*/
        pItemDisc->fbDiscModifier |= (ItemModLocalPtr->fsTaxable << 1);    /* justify void bit */
        /* justify for tax para */
        fbWorkDiscTax = (((pItemDisc->fbDiscModifier & TAX_MOD_MASK) << 1) ^ (pItemDisc->auchDiscStatus[0] & DISC_MDC_TAX_MASK));
                                            /* modified tax status */
        
        pItemDisc->auchDiscStatus[0] &= ~DISC_MDC_TAX_MASK;       /* clear record status , 0x1c */
        pItemDisc->auchDiscStatus[0] |= fbWorkDiscTax;            /* write modified status */
        if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
            pItemDisc->auchDiscStatus[1] ^= 0x08;
        }

        sReturnStatus = ITM_SUCCESS;

        break;

    case CLASS_CHARGETIP:
    case CLASS_CHARGETIP2:                  /* V3.3 */
    case CLASS_CHARGETIP3:
    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
        if (ItemCommonTaxSystem() == ITM_TAX_US ||           /* V3.3 */
            ItemCommonTaxSystem() == ITM_TAX_INTL) {
            if (ItemModLocalPtr->fsTaxable != 0) {  
                return(LDT_SEQERR_ADR);

            } else {
                pItemDisc->auchDiscStatus[1] &= ~DISC_TIP_TAX_MASK; 
                                            /* 0x07, turn off all taxable bits */
                sReturnStatus = ITM_SUCCESS;
            }
            break;
        }

        ItemDiscGetCanTaxMod(pItemDisc);    /* copy canadian tax mod. into pItemDisc */

        if ((pItemDisc->fbDiscModifier & TAX_MOD_MASK) != 0) {
            pItemDisc->auchDiscStatus[1] &= ~DISC_TIP_TAX_MASK;    /* 0x07, turn off all taxable bits */
            fbModWork = ((pItemDisc->fbDiscModifier & TAX_MOD_MASK) >> 1);
            pItemDisc->auchDiscStatus[2] |= ((fbModWork - 1) << 4);  
            /* pItemDisc->auchStatus[1] |= (UCHAR)(fbModWork - 1); 8/23/93 */ 
        }

        if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
            return(LDT_SEQERR_ADR);
        }

        sReturnStatus = ITM_SUCCESS;
        break; 

    default:
        sReturnStatus = ITM_SUCCESS;
		PifLog (MODULE_DISCOUNT, LOG_EVENT_UI_DISCOUNT_ERR_01);
        break;
    }     

    /*--- CLEAR TAX MODIFIER DATA EXCEPT EXEMPT KEY,  R3.0, V3.3 ---
*    ItemModGetLocal(&ModLocalRcvBuff);
*    ModLocalRcvBuff.fsTaxable &= MOD_CANADAEXEMPT;
*    ItemModPutLocal(&ModLocalRcvBuff);
*/
    return(sReturnStatus);
}


/*
*===========================================================================
** Format  : USHORT ItemDiscGetMdcCheck_ITEMDISC1 (UCHAR uchDiscountNo, USHORT usAddress);
*
*   Input  : UCHAR uchDiscountNo
*            USHORT usAddress
*   Output : none
*   InOut  : none
** Return  : USHORT translated address for the specific discount number of CLASS_ITEMDISC1
*
** Synopsis: This function accepts a discount number from 1 to 6 and an MDC address
*            associated with CLASS_ITEMDISC1 such as MDC_MODID11_ADR and translates
*            that MDC address to the corresponding MDC address for the discount number.
*            This function is used by the FSC_ITEM_DISC functionality and other in order
*            to implement more than two discounts.
*
*            WARNING:  The MDC address returned may be an odd or an even address
*                      so when using the address to check an MDC setting, you must
*                      use function CliParaMDCCheckField() to perform the check on the
*                      MDC bit since that function will determine whether to use odd or even
*                      bit check semantics by looking at the address.
*===========================================================================
*/
typedef struct {
	USHORT usAddress;
	USHORT usAddressTo[7];
} MDCTranslateTableType;

static MDCTranslateTableType MdcTranslationTable [] = {
	{MDC_MODID11_ADR, {
		MDC_MODID11_ADR,
		MDC_MODID11_ADR,
		MDC_MODID21_ADR,
		MDC_MODID31_ADR,
		MDC_MODID41_ADR,
		MDC_MODID51_ADR,
		MDC_MODID61_ADR}
	},
	{MDC_MODID12_ADR, {
		MDC_MODID12_ADR,
		MDC_MODID12_ADR,
		MDC_MODID22_ADR,
		MDC_MODID32_ADR,
		MDC_MODID42_ADR,
		MDC_MODID52_ADR,
		MDC_MODID62_ADR}
	},
	{MDC_MODID13_ADR, {
		MDC_MODID13_ADR,
		MDC_MODID13_ADR,
		MDC_MODID23_ADR,
		MDC_MODID33_ADR,
		MDC_MODID43_ADR,
		MDC_MODID53_ADR,
		MDC_MODID63_ADR}
	},
	{MDC_MODID15_ADR, {
		MDC_MODID15_ADR,
		MDC_MODID15_ADR,
		MDC_MODID25_ADR,
		MDC_MODID35_ADR,
		MDC_MODID45_ADR,
		MDC_MODID55_ADR,
		MDC_MODID65_ADR}
	}
};

USHORT ItemDiscGetMdcCheck_ITEMDISC1 (UCHAR uchDiscountNo, USHORT usAddress)
{
	int i;

	if (uchDiscountNo > sizeof(MdcTranslationTable[0].usAddressTo)/sizeof(MdcTranslationTable[0].usAddressTo[0]))
		uchDiscountNo = 0;   // set to the default value

	for (i = 0; i < sizeof(MdcTranslationTable)/sizeof(MdcTranslationTable[0]); i++) {
		if (usAddress == MdcTranslationTable[i].usAddress) {
			usAddress = MdcTranslationTable[i].usAddressTo[uchDiscountNo];
			break;
		}
	}

	return usAddress;
}

typedef struct {
	USHORT usAddressMnemonic;
	USHORT usAddressRate;
	USHORT usAddressRounding;
} ItemDisc1TableType;

static ItemDisc1TableType ItemDisc1Table [] = {
		{TRN_ITMDISC_ADR,   RATE_MTRID_ADR,    RND_ITEMDISC1_ADR},
		{TRN_ITMDISC_ADR,   RATE_MTRID_ADR,    RND_ITEMDISC1_ADR},
		{TRN_MODID_ADR,     RATE_MODID_ADR,    RND_ITEMDISC2_ADR},
		{TRN_ITMDISC_ADR_3, RATE_ITMDISC3_ADR, RND_ITEMDISC1_ADR},
		{TRN_ITMDISC_ADR_4, RATE_ITMDISC4_ADR, RND_ITEMDISC1_ADR},
		{TRN_ITMDISC_ADR_5, RATE_ITMDISC5_ADR, RND_ITEMDISC1_ADR},
		{TRN_ITMDISC_ADR_6, RATE_ITMDISC6_ADR, RND_ITEMDISC1_ADR}
};

USHORT ItemDiscGetMnemonic_ITEMDISC1 (UCHAR uchDiscountNo)
{
	if (uchDiscountNo > sizeof(ItemDisc1Table)/sizeof(ItemDisc1Table[0]))
		uchDiscountNo = 0;   // set to the default value

	return ItemDisc1Table[uchDiscountNo].usAddressMnemonic;
}

USHORT ItemDiscGetRate_ITEMDISC1 (UCHAR uchDiscountNo)
{
	if (uchDiscountNo > sizeof(ItemDisc1Table)/sizeof(ItemDisc1Table[0]))
		uchDiscountNo = 0;   // set to the default value

	return ItemDisc1Table[uchDiscountNo].usAddressRate;
}



/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetTaxMDC(ITEMDISC, pItemDisc)
*
*    Input: ITEMDISC *pItemDisc,
*   Output: None
*    InOut: None
**Return: ITM_SUCCESS    : Function Performed succesfully
*         
** Description: Set MDC option data to ItemDisc->auchStatus.      R3.1
*===========================================================================
*/
VOID   ItemDiscGetTaxMDC(ITEMDISC *pItemDisc)
{
    USHORT  usOffsetAdd, usOffsetAdd2;

    switch(pItemDisc->uchMinorClass) {           
    case CLASS_ITEMDISC1:
		usOffsetAdd = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID11_ADR);
		usOffsetAdd2 = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID15_ADR);
        break;

    case CLASS_REGDISC1:            
        usOffsetAdd = MDC_RDISC11_ADR;     /* address 39 */
        usOffsetAdd2 = MDC_RDISC15_ADR;    /* address 509 */
        break;
    
    case CLASS_REGDISC2:            
        usOffsetAdd = MDC_RDISC21_ADR;     /* address 43 */
        usOffsetAdd2 = MDC_RDISC25_ADR;    /* address 510 */
        break;
    
    case CLASS_REGDISC3:                  /* R3.1 */
        usOffsetAdd = MDC_RDISC31_ADR;
        usOffsetAdd2 = MDC_RDISC35_ADR;   /* address 511 */
        break;

    case CLASS_REGDISC4:                  /* R3.1 */
        usOffsetAdd = MDC_RDISC41_ADR;
        usOffsetAdd2 = MDC_RDISC45_ADR;   /* address 512 */
        break;

    case CLASS_REGDISC5:                  /* R3.1 */
        usOffsetAdd = MDC_RDISC51_ADR;
        usOffsetAdd2 = MDC_RDISC55_ADR;   /* address 513 */
        break;

    case CLASS_REGDISC6:                  /* R3.1 */
        usOffsetAdd = MDC_RDISC61_ADR;
        usOffsetAdd2 = MDC_RDISC65_ADR;   /* address 514 */
        break;

    case CLASS_CHARGETIP:           
    case CLASS_CHARGETIP2:           
    case CLASS_CHARGETIP3:           
    case CLASS_AUTOCHARGETIP:           
    case CLASS_AUTOCHARGETIP2:           
    case CLASS_AUTOCHARGETIP3:
		// Charge Tips is a special kind of a surcharge key, the tip amount is a surcharge.
		// The page definitions for Charge Tips is different from the discount page definitions.
		//
        usOffsetAdd = MDC_CHRGTIP1_ADR;    /* address 23 */
		usOffsetAdd2 = 0;
        break;
    
    default:
		PifLog (MODULE_DISCOUNT, LOG_EVENT_UI_DISCOUNT_ERR_02);
		usOffsetAdd = MDC_MODID11_ADR;
		usOffsetAdd2 = MDC_MODID15_ADR;
        break;
    }

	// Transform the MDC settings for Discounts into the correct bit map used in the discount logic.
	// Each discount key has a set of three pages of MDC settings.  These settings control the behavior
	// of the discount such as the following:
	//   - Validation Print is required or not
	//   - Supervisor Intervention is required or not
	// While the bit map used in pItemDisc->auchStatus array is similar to the bit map for the MDC
	// pages, we have to do this transformation because the bit map used with the auchStatus array
	// is based on the address of the first page of the MDC being an odd number.  However some discount
	// keys start their MDC pages on an even address.  So to normalize the use of the bit map to
	// convey the MDC settings, we have to use the following method to ensure that all discount key
	// settings in auchStatus are normalized as if they all have an odd address.

    pItemDisc->auchDiscStatus[0] = 0;    // first page of settings form for this discount
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_A) ? ODD_MDC_BIT3 : 0);   // DISC_AFFECT_TAXABLE2 or DISC_CHRGTIP_ALLOW_ZRO_ITEM
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_B) ? ODD_MDC_BIT2 : 0);   // DISC_AFFECT_TAXABLE1 or DISC_CHRGTIP_SUP_INTV_REQD
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_C) ? ODD_MDC_BIT1 : 0);   // DISC_RATE_OVERRIDE or DISC_CHRGTIP_ALLOW_OVERRIDE
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_D) ? ODD_MDC_BIT0 : 0);   // DISC_ALLOW_OVERRIDE or DISC_CHRGTIP_ALLOW_AUTO_MAN

	usOffsetAdd += 1;                // skip to the second page of settings form for this discount
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_A) ? EVEN_MDC_BIT3 : 0);   // DISC_VALDATION_PRINT
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_B) ? EVEN_MDC_BIT2 : 0);   // DISC_SUP_INTV_REQD or DISC_CHRGTIP_AC86_12_RATE
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_C) ? EVEN_MDC_BIT1 : 0);   // DISC_NOTAFFECT_HOURLY or DISC_CHRGTIP_PRNT_SNGL_DBLE
    pItemDisc->auchDiscStatus[0] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_D) ? EVEN_MDC_BIT0 : 0);   // DISC_AFFECT_TAXABLE3 or DISC_CHRGTIP_PRNT_ITEM_RECPT

	usOffsetAdd += 1;                // skip to the third page of settings form for this discount. Item Discount and Transaction Discount varies on this page.
    pItemDisc->auchDiscStatus[1] = 0;  
    pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_A) ? ODD_MDC_BIT3 : 0);   // DISC_AFFECT_BONUS_09 or DISC_AFFECT_REGDISC_FOOD
    pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_B) ? ODD_MDC_BIT2 : 0);   // DISC_AFFECT_FOODSTAMP or DISC_AFFECT_REGDISC_1OR2
    pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_C) ? ODD_MDC_BIT1 : 0);   // DISC_AFFECT_TOTAL or DISC_AFFECT_REGDISC_AFFECT
    pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd, MDC_PARAM_BIT_D) ? ODD_MDC_BIT0 : 0);   // DISC_SURCHARGE_KEY

	if (usOffsetAdd2) {              // skip to the fourth page of settings form for this discount if there is a fourth page.
//		pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd2, MDC_PARAM_BIT_A) ? EVEN_MDC_BIT3 : 0);   // unused at pressent.
//		pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd2, MDC_PARAM_BIT_B) ? EVEN_MDC_BIT2 : 0);   // unused at pressent.
//		pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd2, MDC_PARAM_BIT_C) ? EVEN_MDC_BIT1 : 0);   // unused at pressent.
		pItemDisc->auchDiscStatus[1] |= (CliParaMDCCheckField(usOffsetAdd2, MDC_PARAM_BIT_D) ? EVEN_MDC_BIT0 : 0);   // DISC_SUPINTV_HALO
	}
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscGetCanTaxMod(VOID *pData)
*
*    Input: VOID *pData,
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: Nothing
*         
** Description: Canadian tax modifier process for charge tip. 
*                
*===========================================================================
*/

VOID    ItemDiscGetCanTaxMod(VOID *pData)
{
    USHORT        i;
	USHORT        uchBit;
    ITEMSALES     *pItemSales = pData;
    ITEMDISC      *pItemDisc = pData;

    for (uchBit = MOD_TAXABLE1, i = 1; uchBit <= MOD_TAXABLE9; uchBit <<= 1, i++) {
        if (ItemModLocalPtr->fsTaxable & uchBit) {   /* determine which modifier bit turned on */
			switch(pItemSales->uchMajorClass) {      /* hit ! */
				case CLASS_ITEMSALES:
					pItemSales->fbModifier |= (USHORT)(i << 1);   // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
					break;
	    
				case CLASS_ITEMDISC:
					pItemDisc->fbDiscModifier |= (USHORT)(i << 1);   // convert to Canadian tax type index and translate to skip VOID_MODIFIER bit
					break;
	    
				default:
					break;
            }
    
            break;        /* break for loop as only single tax type index may be used at a time */
        }
    }
}

/****** end of source ******/
