/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-2000      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.3              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         
:   Category           :                                                 
:   Program Name       : TRNCOMN.C                                       
:*------------------------------------------------------------------------
:   Abstract         
:
:*------------------------------------------------------------------------
:   Update Histories                                                      
:       Date    :   Version/Revision    :   Name    :   Description      
:     08.28.98       03.03.00               hrkato      V3.3
:     05.17.00       01.00.00               hrkato      Saratoga FVT Comments
:     12.05.02	     01.04.00				cwunn		GSU SR 61 by cwunn
:     12.04.17	     02.02.02				rchamber	replace constant indexes with TRANCANADA_ defines in places.
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>
#include    <stdlib.h>

#include    "ecr.h"
#include    "log.h"
#include    "appllog.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "rfl.h"
#include    "pif.h"

static VOID    TrnDiscTaxCanCom(USHORT fbStatus, USHORT fbTaxStatus, LONG lAmount, TRANITEMIZERS *pTrn, LONG lSign);

/*
*==========================================================================
*    Synopsis:  VOID    TrnSalesDeptUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   Update the tax data in the memory resident database for departmental PLU.
*                   We need to check the various tax type flags of the item and update
*                   the appropriate tax information.
*
*                   This is for a departmental PLU. For a standard PLU see the
*                   functions TrnTaxableUSComn() and TrnDiscTaxUSComn() below which
*                   split the item tax update and the discount update into separate
*                   functions.
*==========================================================================
*/
VOID    TrnSalesDeptUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT  usWorkMDC, usWorkMod, usWorkBit;
    SHORT   i;
    UCHAR   fchFood;

	// Transform the fbModifier bit tax settings into the standard MDC aligned bit tax settings so that we
	// can do our check on what taxable itemizers are to be updated.
	// For US Tax system, the fbModifier uses the least significant bit for the void indicator.
	// For US Tax System we also need to know if the item is food stampable item or not.
	usWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];  /* MDC page 3 settings */
    usWorkMod = pItem->fbModifier >> 1;                                   /* shift off VOID_MODIFIER indicator bit to make masking easier. */
    usWorkBit = 0x01;
    fchFood = (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_FS);

    for (i = 0; i < STD_TAX_ITEMIZERS_US; i++, usWorkBit <<= 1) {
        if ((usWorkMDC ^ usWorkMod) & usWorkBit) {
            pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[i] += pItem->lProduct * lSign;

            /* ----- check food stampable ----- */
            if (fchFood) {
                pTrn->Itemizers.TranUsItemizers.lFoodTaxable[i] += pItem->lProduct * lSign;
            }
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC1) {
				// offset to first half of the array where discount #1 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i] += pItem->lProduct * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][i] += pItem->lProduct * lSign;
                }
            }
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC2) {
				// offset to second half of the array where discount #2 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i + STD_TAX_ITEMIZERS_US] += pItem->lProduct * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][i] += pItem->lProduct * lSign;
                }
            }
        }
    }
    /* --- Food Stampable --- */
    if (fchFood) {
        pTrn->Itemizers.TranUsItemizers.lFoodStampable += pItem->lProduct * lSign;
        /* --- if affect any US Taxable #1-3 ---  */
        if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3)) {
            pTrn->Itemizers.TranUsItemizers.lFoodWholeTax += pItem->lProduct * lSign;
        }
    }
}

/*
*===========================================================================
** Synopsis:    CHAR TrnGetPigRiuleCo( LONG lPrine )
*               
*     Input:    lPrice           : price amount
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      0                : pig rule counter (minus item case)
*               1                : pig rule counter (pig rule table not assign)
*                                : pig rule counter 
*
** Description: get pig rule counter by A/C 130 used with Canadian tax calculations.
*               pig rule data located in pararam.h, UNINIRAM  Para.ParaPigRule[MAX_PIGRULE_SIZE];
*               first entry in table is Counter limit for non-GST tax.
*               next five entries are Max unit price for 1 through 5.
*===========================================================================
*/
/* structure definition for internal use */
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct  {
        UCHAR  uchPigRuleCo;                /* pig rule counter */                            
        ULONG  ulPigBreakPoint;             /* pig rule break point */
}TRNPIGRULETBL;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

static CHAR    TrnGetPigRuleCo( DCURRENCY lPrice )
{
    UCHAR           i, j, uchMaxPigRuleCo;
    USHORT          usRetLen;
	ULONG           ulPigRuleTbl[5] = {0};       // buffer to receive five Max Unit Price entries from AC130.
    TRNPIGRULETBL   SortedPigRuleTbl[5] = {0};

    /* check input price */
    if (lPrice <= 0L) {         /* minus item */
        return(0);
    }

    /* get all pig rule break point, we want the five Max. Unit Price entries so skip first entry. */
    CliParaAllRead(CLASS_PARAPIGRULE, ( UCHAR *)ulPigRuleTbl, sizeof(ulPigRuleTbl), 4, &usRetLen);

    /* check all break point is 0 */
    if ((ulPigRuleTbl[0] == 0L) && (ulPigRuleTbl[1] == 0L) &&
        (ulPigRuleTbl[2] == 0L) && (ulPigRuleTbl[3] == 0L) &&
        (ulPigRuleTbl[4] == 0L)) {
            return(1);
    }

    /* sort pig rule table */
    for (i = 0, j = 0; i < 5; i++) {
        if (ulPigRuleTbl[i]) {
            SortedPigRuleTbl[j].uchPigRuleCo = i + 1;
            SortedPigRuleTbl[j].ulPigBreakPoint = ulPigRuleTbl[i];
            uchMaxPigRuleCo = SortedPigRuleTbl[j].uchPigRuleCo + ( UCHAR)1;
            j++;
        }
    }

    /* check break point */
    for (i = 0; i < 5; i++) {
        if (( ULONG)lPrice <= SortedPigRuleTbl[i].ulPigBreakPoint) {    /* found pig rule break pint */
            return(SortedPigRuleTbl[i].uchPigRuleCo);                   
        }
        if (uchMaxPigRuleCo == (SortedPigRuleTbl[i].uchPigRuleCo + ( UCHAR)1)) {
            break;
        }
    }
    return(uchMaxPigRuleCo);
}

/*
*==========================================================================
*   Synopsis:  VOID    TrnSalesDeptCanComn(ITEMSALES *pItem,
*                           TRANITEMIZERS *pItemizer, TRANGCFQUAL *pGCF)
*
*   Input:     VOID    *TrnQual
*   Output:    none
*   InOut:     none
*   Return:    none
*
*   Description:   transaction qualifier
:   Update Histories                                                      
:   Date    :   Version/Revision    :   Name    :   Description      
:	12.05.02	01.04.00				cwunn		Modified logic for GSU SR 61

*==========================================================================
*/
VOID    TrnSalesDeptCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pItemizer,
                        TRANGCFQUAL *pGCF, LONG lSign)
{
    USHORT  usWorkMod;

    usWorkMod = pItem->fbModifier >> 1;
    usWorkMod &= TRN_MASKSTATCAN;

    if (usWorkMod == 0) {
        usWorkMod = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1]; 
        usWorkMod &= TRN_MASKSTATCAN;
        if (usWorkMod > STD_PLU_ITEMIZERS_MOD) {
            usWorkMod = STD_PLU_ITEMIZERS_MOD;
        }
        usWorkMod++;
    }

/*	Begin GSU SR 61 by cwunn modifications
 *	Logic added to properly handle carbonated beverage taxation, including voiding scenarios (GSU SR 6 by cwunn)
 *	Basic Logic:
 *		Carbonated beverage should be taxed when purchased alone, or with food items totaling more than the exempt limit
 *		Carbonated beverage should not be taxed if purchased with food items totaling less than the exempt limt
 *	Situations handled by the logic:
 *	#1	Voiding carbonated beverage purchased with food items totaling more than the exempt limit. Affect PST1 and GST
 *	#2	Voiding carbonated beverage purchased with food items totaling less than the exempt limit. Affect GST only
 *	#3	Purchasing cabronated beverage with food items totaling less than the exempt limit. Affect GST only
 *	#4	Voiding carbonated beverage with food items totaling more than exempt limit where total falls under
 *			exempt limit. Reset PST1 to 0 and affect GST.
 *	#5	Voiding carbonated beverage with food items totaling more than exempt limit where total remains over
 *			exempt limit. Affect PST1 and GST.
 *	#6	Purchasing carbonated beverage with food items totaling more than the exempt limit. Affect PST and GST.
 *	#7	Purchasing or voiding carbonated beverage when not using exempt limit taxation (Original operation of the terminal)
 *      
 */

	if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT0 ) == 0 && usWorkMod == CANTAX_CARBON) {  //if machine uses exempt limit taxation
		//tax rate table struct for obtaining the exempt meal limit (GSU SR 61 by cwunn)
		PARATAXRATETBL  WorkLimit = {0};

		WorkLimit.uchMajorClass = CLASS_PARATAXRATETBL;                
		WorkLimit.uchAddress = TXRT_NO5_ADR;        //initialize data structure for tax rates
		CliParaRead( &WorkLimit );					//obtain the exempt limit from tax rate table

		//****Add item checking to prevent PST tax reduction on void carb bev with meal item***
		//if there is some amount in meal limit itemizer AND the amount is less than the exempt limit
		if(((pItemizer->Itemizers.TranCanadaItemizers.lTaxable[0]) && (pItemizer->Itemizers.TranCanadaItemizers.lTaxable[0] < (LONG)WorkLimit.ulTaxRate))) {

			if(pItem->fbModifier & VOID_MODIFIER){	//if voiding an item
				//if NonLimitable itemizer is larger than void product price,
				// the product may not be a beverage, but may be of another taxable status,
				// so preserve taxation as determined by the system's original logic
				if(pItemizer->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable >= RflLLabs(pItem->lProduct) ){ 
					// Situation #1 it appears carb bev purchase was PST Taxed, so affect the PST1 itemizer
					pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
				}
				else {
					//else -> NonLimitable itemizer does not account for beverage purchase, hence beverage likely untaxed
					//Situation #2 it appears a carb bev was purchased PST Tax free, so do not affect PST1 itemizer, only GST
					if (pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 2] != (pItem->lProduct * -1))   //  Hardcoaded -1 for sign change   ***PDINU
					{
						pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 2] += pItem->lProduct * lSign;
					}else
					{
						//Situation #3 it appears carb bev purchase should be PST Taxed, so affect the PST itemizer
						pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
					}
				}
			}
			else { 
				//else -> not voiding
				//Situation #3 it appears carb bev purchase should not be PST Taxed, so affect the GST itemizer
				pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
			}
		}
		else { //else -> Transaction over exempt limit, or no limitable purchases,
			//If over exempt then falling back under via void carb bev, PST taxable itemizer must be set to 0.
			if(pItem->fbModifier & VOID_MODIFIER){
				//if item void will decrease total to under the exempt limit
				if((pItemizer->Itemizers.TranCanadaItemizers.lMealLimitTaxable + (pItem->lProduct * lSign) ) < (LONG)WorkLimit.ulTaxRate ){
					//Situation #4 reset PST to 0, affect GST taxable itemizer
					pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] = 0;
					pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 2] += pItem->lProduct * lSign;
				}
				else { 
					//else -> voiding the item keeps total over the exempt limit
					//Situation #5 if not falling under exempt limit by this void
					pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
				}
			}
			else {
				//else -> not voiding
				//Situation #6 affect taxable amount to appropriate itemizer
				pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
			}
		}//end else -> Transaction over exempt limit or no limitable purchases
	}// end if(using exempt limit taxation)
	else {//Situation #7 Not using exempt limit taxation, affect taxable amount to appropriate itemizer as in original operation
		pItemizer->Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += pItem->lProduct * lSign;
	}

//***END GSU SR 61 by cwunn modifications***


    /*----- Discount/Taxable Itemizer -----*/
    TrnDiscTaxCanCom(pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1], (UCHAR)(usWorkMod - 1), pItem->lProduct, pItemizer, lSign);

    if ((usWorkMod == CANTAX_BAKED) || ((CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT3)) && (usWorkMod == CANTAX_SPEC_BAKED))) {   /* baked item or MDC 16A is on and Spec Baked Item*/
		LONG    lQTYWork;

        /* update pig rule counter */
        lQTYWork = pItem->lQTY / 1000L;
        if (!lQTYWork) {
            if (pItem->lQTY < 0L) {
                lQTYWork = -1L;
            } else {
                lQTYWork = 1L;
            }
        }
        pGCF->sPigRuleCo += (SHORT)(TrnGetPigRuleCo(pItem->lUnitPrice) * lQTYWork * lSign);
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnSalesDeptIntlComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnSalesDeptIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR           uchWorkMDC, uchWorkBit;
    SHORT           i;

    uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
    uchWorkMDC &= 0x03;
    uchWorkMDC--;
    uchWorkBit = PLU_AFFECT_DISC1;

    if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_VAT_SERVICE) {  /* Serviceable */
        pTrn->Itemizers.TranIntlItemizers.lServiceable += pItem->lProduct * lSign;
        pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += pItem->lProduct * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisSer[i][uchWorkMDC] += pItem->lProduct * lSign;
                pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[i] += pItem->lProduct * lSign;
            }
        }
    } else {
        pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += pItem->lProduct * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisNon[i][uchWorkMDC] += pItem->lProduct * lSign;
            }
        }
    }
}

/*
*==========================================================================
**    Synopsis: VOID    TrnTaxableUSComn( ITEMSALES *pItem ,TRANITEMIZERS *pTrn)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update US taxable itemizer                         V3.3
*==========================================================================
*/
VOID    TrnTaxableUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT          uchWorkMDC, uchWorkMod, uchWorkBit, fchFood;
    USHORT          i, j;
    USHORT          usMaxChild = (pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo);
	TCHAR           auchDummy[NUM_PLU_LEN] = {0};

	// Transform the fbModifier bit tax settings into the standard MDC aligned bit tax settings so that we
	// can do our check on what taxable itemizers are to be updated.
	// For US Tax system, the fbModifier uses the least significant bit for the void indicator.
	// For US Tax System we also need to know if the item is food stampable item or not.

	uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];  /* MDC page 3 settings */
    uchWorkMod = pItem->fbModifier >> 1;                                   /* shift off VOID_MODIFIER indicator bit to make masking easier. */
    uchWorkBit = 0x01;
    fchFood = (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_FS);

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

	// check fbModifier indicators TAX_MODIFIER1, TAX_MODIFIER2, and TAX_MODIFIER3 against
	// PLU control indicators PLU_AFFECT_TAX1, PLU_AFFECT_TAX2, and PLU_AFFECT_TAX3 to
	// determine if a tax modifier has been used to invert the PLU control tax type indicator.
    for (i = 0; i < STD_TAX_ITEMIZERS_US; i++, uchWorkBit <<= 1) {
        if ((uchWorkMDC ^ uchWorkMod) & uchWorkBit) {
            pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[i] += pItem->lProduct * lSign;
            /* ----- check food stampable ----- */
            if (fchFood) {
                pTrn->Itemizers.TranUsItemizers.lFoodTaxable[i] += pItem->lProduct * lSign;
            }
        }
    }

    for ( i = 0; i < usMaxChild; i++ ) {
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }
        uchWorkMDC = pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
        uchWorkBit = 0x01;
           
        for (j = 0; j < STD_TAX_ITEMIZERS_US; j++, uchWorkBit <<= 1) {
            if ((uchWorkMDC ^ uchWorkMod) & uchWorkBit) {
                pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFoodTaxable[j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                }
            }
        } 
    }         
}

/*
*==========================================================================
**    Synopsis: VOID    TrnTaxableCanComn( ITEMSALES *pItem ,TRANITEMIZERS *pTrn)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update Canadian taxable itemizer                    V3.3
*==========================================================================
*/

VOID    TrnTaxableCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT          uchWorkMod;
    USHORT          i;
    USHORT          usMaxChild = (pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo);
    TCHAR           auchDummy[NUM_PLU_LEN] = {0};

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

	//For each child of the current item, assess the assigned condiment price to taxable total 
    for ( i = 0; i < usMaxChild; i++ ) {
     
        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        uchWorkMod = pItem->fbModifier >> 1;                                   /* shift off VOID_MODIFIER indicator bit to make masking easier. */
        uchWorkMod &= TRN_MASKSTATCAN;
        if ( uchWorkMod == 0 ) {
            uchWorkMod = pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
            uchWorkMod &= TRN_MASKSTATCAN;
			if (uchWorkMod >STD_PLU_ITEMIZERS_MOD) {
				uchWorkMod = STD_PLU_ITEMIZERS_MOD;
			}
            uchWorkMod++;
        }

        pTrn->Itemizers.TranCanadaItemizers.lTaxable[ uchWorkMod - 1 ] += ( pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice ) * lSign; 
    }

}

/*
*==========================================================================
**    Synopsis: VOID    TrnTaxableIntlComn( ITEMSALES *pItem ,TRANITEMIZERS *pTrn)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update intl taxable itemizer                    V3.3
*==========================================================================
*/
VOID    TrnTaxableIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR           uchWorkMDC, uchWorkBit;
    USHORT          i, j;
    USHORT          usMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;
    TCHAR           auchDummy[NUM_PLU_LEN] = {0};

    uchWorkMDC = (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1]) & 0x03;
    uchWorkMDC--;
    uchWorkBit = PLU_AFFECT_DISC1;

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

    if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_VAT_SERVICE) {  /* Serviceable */
        pTrn->Itemizers.TranIntlItemizers.lServiceable += pItem->lProduct * lSign;
        pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += pItem->lProduct * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisSer[i][uchWorkMDC] += pItem->lProduct * lSign;
                pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[i] += pItem->lProduct * lSign;
            }
        }
    } else {
        pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += pItem->lProduct * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisNon[i][uchWorkMDC] += pItem->lProduct * lSign;
            }
        }
    }
    
    /* --- Condiment --- */
    for (i = 0; i < usMaxChild; i++) {
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }
        uchWorkMDC = pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
        uchWorkMDC &= 0x03;
        uchWorkMDC--;
        uchWorkBit = PLU_AFFECT_DISC1;
         
        if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_VAT_SERVICE) {  /* Serviceable */
            pTrn->Itemizers.TranIntlItemizers.lServiceable += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
            pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
            for (j = 0; j < 2; j++, uchWorkBit <<= 1) {
                if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                    pTrn->Itemizers.TranIntlItemizers.lDisSer[j][uchWorkMDC] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                    pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                }
            }
        } else {
            pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
            for (j = 0; j < 2; j++, uchWorkBit <<= 1) {
                if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                    pTrn->Itemizers.TranIntlItemizers.lDisNon[j][uchWorkMDC] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                }
            }
        }
    }
}

/*
*==========================================================================
**    Synopsis: VOID    TrnDiscTaxUSComn( ITEMSALES *pItem ,TRANITEMIZERS *pTrn)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update US discountable/taxable itemizer          V3.3
*==========================================================================
*/
VOID    TrnDiscTaxUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT   uchWorkMDC, uchWorkMod, uchWorkBit, fchFood;
    USHORT   i, j;
    USHORT   usMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;
    TCHAR    auchDummy[NUM_PLU_LEN] = {0};

    uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];  /* MDC page 3 settings */
    uchWorkMod = pItem->fbModifier >> 1;                                   /* shift off VOID_MODIFIER indicator bit to make masking easier. */
    uchWorkBit = 0x01;
    fchFood = (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_FS);

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

	// check fbModifier indicators TAX_MODIFIER1, TAX_MODIFIER2, and TAX_MODIFIER3 against
	// PLU control indicators PLU_AFFECT_TAX1, PLU_AFFECT_TAX2, and PLU_AFFECT_TAX3 to
	// determine if a tax modifier has been used to invert the PLU control tax type indicator.
    for ( i = 0; i < STD_TAX_ITEMIZERS_US; i++, uchWorkBit <<= 1 ) {
        if ( ( uchWorkMDC ^ uchWorkMod ) & uchWorkBit ) {
            if ( pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC1 ) {
				// offset to first half of the array where discount #1 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i] += pItem->lProduct * lSign;        /* disc/tax itemizer */
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][i] += pItem->lProduct * lSign;
                }
            }
            if ( pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC2 ) {
				// offset to second half of the array where discount #2 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i + STD_TAX_ITEMIZERS_US] += pItem->lProduct * lSign;    /* disc/tax itemizer */
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][i] += pItem->lProduct * lSign;
                }
            }
        }
    }

    for ( i = 0; i < usMaxChild; i++ ) {
        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        uchWorkMDC = pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];  /* MDC page 3 settings */
        uchWorkBit = 0x01;

        for ( j = 0; j < STD_TAX_ITEMIZERS_US; j++, uchWorkBit <<= 1 ) {
            if ( ( uchWorkMDC ^ uchWorkMod ) & uchWorkBit ) {
                if ( pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC1 ) {
					// offset to first half of the array where discount #1 totals are maintained.
                    pTrn->Itemizers.TranUsItemizers.lDiscTaxable[j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                    /* ----- check food stampable ----- */
                    if (fchFood) {
                        pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                    }
                }

                if ( pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC2 ) {
					// offset to second half of the array where discount #2 totals are maintained.
                    pTrn->Itemizers.TranUsItemizers.lDiscTaxable[j + STD_TAX_ITEMIZERS_US] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                    /* ----- check food stampable ----- */
                    if (fchFood) {
                        pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][j] += (pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice) * lSign;
                    }
                }       
            }
        }
    }       
}

/*
*==========================================================================
**    Synopsis: VOID    TrnDiscTaxCanComn( ITEMSALES *pItem ,TRANITEMIZERS *pTrn)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update Canada discountable/taxable itemizer          V3.3
*==========================================================================
*/
VOID    TrnDiscTaxCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT   uchWorkMod;
    USHORT   i;
    USHORT   usMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;
    TCHAR    auchDummy[NUM_PLU_LEN] = {0};

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

    for (i = 0; i < usMaxChild; i++) {
        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        uchWorkMod = pItem->fbModifier;
        uchWorkMod >>= 1;
        uchWorkMod &= TRN_MASKSTATCAN;
        if ( uchWorkMod == 0 ) {
            uchWorkMod = pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
            uchWorkMod &= TRN_MASKSTATCAN;
			if (uchWorkMod > STD_PLU_ITEMIZERS_MOD) {
				uchWorkMod = STD_PLU_ITEMIZERS_MOD;
			}
            uchWorkMod++;
        }
    
        TrnDiscTaxCanCom( pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1], ( UCHAR )( uchWorkMod - 1 ),
            ( LONG )( pItem->lQTY / 1000 * pItem->Condiment[i].lUnitPrice ), pTrn, lSign);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnDiscTaxCanCom(UCHAR fbStatus,
*                           UCHAR fbTaxStatus, LONG lAmount, SHORT sType)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update discountable/taxable itemizer
==========================================================================*/

static VOID    TrnDiscTaxCanCom(USHORT fbStatus, USHORT fbTaxStatus, LONG lAmount, TRANITEMIZERS *pTrn, LONG lSign)
{
    /* calculate discountable/taxable itemizer */
    switch(fbTaxStatus) {
    case    8:                                                  /* baked */
    	/* save for pig rule */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][11] += lAmount * lSign;	/* GST - should the index 11 be replaced by TRANCANADA_GST? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][12] += lAmount * lSign; /* PST1 limitable - should the index 12 be replaced by TRANCANADA_PST1LIMIT? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][13] += lAmount * lSign; /* PST1 limitable for tax on tax - should the index 13 be replaced by TRANCANADA_PST1TOT? */
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][11] += lAmount * lSign;	/* GST - should the index 11 be replaced by TRANCANADA_GST? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][12] += lAmount * lSign; /* PST1 limitable - should the index 12 be replaced by TRANCANADA_PST1LIMIT? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][13] += lAmount * lSign; /* PST1 limitable for tax on tax - should the index 13 be replaced by TRANCANADA_PST1TOT? */
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][11] += lAmount * lSign;	/* GST - should the index 11 be replaced by TRANCANADA_GST? */
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][12] += lAmount * lSign; /* PST1 limitable - should the index 12 be replaced by TRANCANADA_PST1LIMIT? */
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][13] += lAmount * lSign; /* PST1 limitable for tax on tax - should the index 13 be replaced by TRANCANADA_PST1TOT? */
	/* not break */
    case    0:                                                  /* meal */
    /* case    1:                                                  / carbon */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][1] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][5] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][1] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][5] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][1] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][5] += lAmount * lSign;
        break;

    case    1:                                                  /* carbon */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][9] += lAmount * lSign;	/* limitable or non-limitable - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][10] += lAmount * lSign; /* tax on tax - should the index 9 be replaced by TRANCANADA_TAXONTAX? */
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][9] += lAmount * lSign;	/* limitable or non-limitable - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][10] += lAmount * lSign; /* tax on tax - should the index 9 be replaced by TRANCANADA_TAXONTAX? */
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][9] += lAmount * lSign;	/* limitable or non-limitable - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][10] += lAmount * lSign; /* tax on tax - should the index 9 be replaced by TRANCANADA_TAXONTAX? */
        break;

    case    2:                                                  /* snack */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][2] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][6] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][2] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][6] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][2] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][6] += lAmount * lSign;
        break;

    case    3:                                                  /* beer/wine */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][3] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][7] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][3] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][7] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][3] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][7] += lAmount * lSign;
        break;

    case    4:                                                  /* liquor */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][4] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][8] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][4] += lAmount * lSign;
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][8] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][4] += lAmount * lSign;
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][8] += lAmount * lSign;
        break;

    case    6:                                                  /* GST */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0] += lAmount * lSign;
        break;

    case    7:                                                  /* PST1 */
        if (fbStatus & PLU_AFFECT_DISC1) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][1] += lAmount * lSign;
        }
        if (fbStatus & PLU_AFFECT_DISC2) {
            pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][1] += lAmount * lSign;
        }
        /*----- set for RD with main itemizer 4/24/96 -----*/
        pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][1] += lAmount * lSign;
        break;

    default:
        break;
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnSalesModUSComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnSalesModUSComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit, fchFood;
    SHORT   i;

    uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
    uchWorkBit = 0x01;
    fchFood = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_FS;

    for (i = 0; i < STD_TAX_ITEMIZERS_US; i++, uchWorkBit <<= 1) {
        if (uchWorkMDC & uchWorkBit) {
            pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[i] += pItem->lDiscountAmount * lSign;
            /* ----- check food stampable ----- */
            if (fchFood) {
                pTrn->Itemizers.TranUsItemizers.lFoodTaxable[i] += pItem->lDiscountAmount * lSign;
            }
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC1) {
				// offset to first half of the array where discount #1 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i] += pItem->lDiscountAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][i] += pItem->lDiscountAmount * lSign;
                }
            }
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_DISC2) {
				// offset to second half of the array where discount #2 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i + STD_TAX_ITEMIZERS_US] += pItem->lDiscountAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][i] += pItem->lDiscountAmount * lSign;
                }
            }
        }
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnSalesModCanComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnSalesModCanComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC;

    if ((pItem->fbModifier & TAX_MOD_MASK) != 0) {
        uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2]; 
        uchWorkMDC >>= 4;
    } else {
        uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1]; 
    }
    uchWorkMDC &= TRN_MASKSTATCAN;
    if (uchWorkMDC >STD_PLU_ITEMIZERS_MOD) {
        uchWorkMDC = STD_PLU_ITEMIZERS_MOD;
    }

    pTrn->Itemizers.TranCanadaItemizers.lTaxable[ uchWorkMDC ] += pItem->lDiscountAmount * lSign;
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnSalesModIntlComn(ITEMSALES *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnSalesModIntlComn(CONST ITEMSALES *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit;

    uchWorkMDC = pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1];
    uchWorkMDC &= 0x03;
    uchWorkMDC--;
    uchWorkBit = PLU_AFFECT_DISC1;

    if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & PLU_AFFECT_VAT_SERVICE) {  /* Serviceable */
		SHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lServiceable += pItem->lDiscountAmount * lSign;
        pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += pItem->lDiscountAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisSer[i][uchWorkMDC] += pItem->lDiscountAmount * lSign;
                pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[i] += pItem->lDiscountAmount * lSign;
            }
        }
    } else {
		SHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += pItem->lDiscountAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisNon[i][uchWorkMDC] += pItem->lDiscountAmount * lSign;
            }
        }
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnItemDiscountUSComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnItemDiscountUSComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit, fchFood;
    SHORT   i;

    uchWorkMDC = pItem->auchDiscStatus[0];
    uchWorkBit = 0x04;    // mask for DISC_AFFECT_TAXABLE1, DISC_AFFECT_TAXABLE2, and DISC_AFFECT_TAXABLE3?
    fchFood = pItem->auchDiscStatus[1] & 0x04;   // DISC_AFFECT_FOODSTAMP Item Discount Food Stamp indicator, MDC page 3 bit B.

    for (i = 0; i < STD_TAX_ITEMIZERS_US; i++, uchWorkBit <<= 1) {
        if (uchWorkMDC & uchWorkBit) {
            pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[i] += pItem->lAmount * lSign;
            /* ----- check food stampable ----- */
            if (fchFood) {
                pTrn->Itemizers.TranUsItemizers.lFoodTaxable[i] += pItem->lAmount * lSign;
            }
            if (pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC1) {              /* disc/tax 1 */
				// offset to first half of the array where discount #1 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i] += pItem->lAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][i] += pItem->lAmount * lSign;
                }
            }
            if (pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC2) {              /* dsic/tax 2 */
				// offset to second half of the array where discount #2 totals are maintained.
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[i + STD_TAX_ITEMIZERS_US] += pItem->lAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][i] += pItem->lAmount * lSign;
                }
            }                                                       
        }
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnItemDiscountUSComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnItemDiscountCanComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit = 0;

    uchWorkMDC = pItem->auchDiscStatus[2];
    uchWorkMDC >>= 4;
    uchWorkMDC &= TRN_MASKSTATCAN;

    pTrn->Itemizers.TranCanadaItemizers.lTaxable[ uchWorkMDC ] += pItem->lAmount * lSign;

    if ( pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC1 ) {
        uchWorkBit |= PLU_AFFECT_DISC1;
    }
    if ( pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC2 ) {
        uchWorkBit |= PLU_AFFECT_DISC2;
    }
    TrnDiscTaxCanCom( uchWorkBit, uchWorkMDC, pItem->lAmount, pTrn, lSign); /* Disc/Taxable */

}

/*
*==========================================================================
*    Synopsis:  VOID    TrnItemDiscountIntlComn(ITEMDISC *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnItemDiscountIntlComn(CONST ITEMDISC *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit;

    uchWorkMDC = (pItem->auchDiscStatus[0] & DISC_MDC_TAX_MASK) >> 2;
    uchWorkMDC--;
    uchWorkBit = ITM_DISC_AFFECT_DISC1;

    if (pItem->auchDiscStatus[2] & ITM_DISC_AFFECT_SERV) {
		SHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lServiceable += pItem->lAmount * lSign;
        pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += pItem->lAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->auchDiscStatus[2] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisSer[i][uchWorkMDC] += pItem->lAmount * lSign;
                pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[i] += pItem->lAmount * lSign;
            }
        }
    } else {
		SHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += pItem->lAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->auchDiscStatus[2] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisNon[i][uchWorkMDC] += pItem->lAmount * lSign;
            }
        }
    }
}

/*
*==========================================================================
**    Synopsis: VOID TrnRegDiscTaxRateComn(ITEMDISC *pItemDisc, SHORT sType, LONG lSign)
*
*    Input:     ITEMDISC    *pItemDisc
*               LONG        lSign
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   calculate discountable/taxable (rate)      R3.1
*    TAR191625 Canadian Tax is not correct after voided discount operation.
*
*                   lSign is used with one of two defines:
*                     TRN_ITEMIZE_NORMAL -> normal calculations
*                     TRN_ITEMIZE_REDUCE -> for reduce calculations
*==========================================================================
*/
VOID    TrnRegDiscTaxRateComn(ITEMDISC *pItemDisc, SHORT sType, LONG lSign)
{
    UCHAR   uchTaxableSts, uchRegDiscAdr, fchFood;
    UCHAR   iuchDisc = 0;
    UCHAR   uchMinorClass;
    SHORT   sUseMI = 0;
    LONG    lRate;
	LONG    lDiscSign;
	DCURRENCY     *plNonAffectTaxable, *plDiscTaxable, *plFSDiscTaxable;
    TRANITEMIZERS *pTrn, *pTrnOut;
    DCURRENCY     alDiscTaxable[STD_TAX_ITEMIZERS_MAX] = {0}; /* 03/31/01, LePeeps */
	/* TAR191625 */
    DCURRENCY     *plVoidDiscNonAffectTaxable, *plVoidDiscFSDiscTaxable;

	uchMinorClass = pItemDisc->uchMinorClass - CLASS_REGDISC1;

	NHPOS_ASSERT_TEXT((uchMinorClass < STD_DISC_ITEMIZERS_MAX), "**ERROR - TrnRegDiscTaxRateComn(): uchMinorClass out of range.");
	NHPOS_ASSERT(pItemDisc->uchMinorClass != CLASS_ITEMDISC1);

    /* --- Food Stampable --- */
    fchFood = 0;	/* FS discount calculation does not work correct, but FS Total works correctly */
    /* fchFood = pItemDisc->auchStatus[1] & 0x08; correct for Transaction Discount */
    /* fchFood = pItemDisc->auchStatus[1] & 0x04; wrong */

    /*----- Decide Target File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnSplitA.TranItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pTrn = &TrnSplitB.TranItemizers;
        pTrnOut = &TrnSplitB.TranItemizers;
    } else if (pItemDisc->uchSeatNo != 0) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    } else {
        pTrn = &TrnInformation.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    }

    /* --- retrieve discount item's taxable status --- */
    uchTaxableSts      = ( UCHAR )( pItemDisc->auchDiscStatus[ 0 ] >> 2 );
    lRate              = (LONG)( pItemDisc->uchRate ) * RFL_DISC_RATE_MUL;
    plNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
	/* TAR191625 */
    plVoidDiscNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lVoidDiscNonAffectTaxable[uchMinorClass][0];

	// whether itemizers are incremented or decremented depends on whether this is a Surcharge or a Discount.
    if (pItemDisc->auchDiscStatus[1] & DISC_SURCHARGE_KEY) {
	    lDiscSign = 1L;       // surcharge so increment
	    lSign *= 1L;          // keep sign for a surcharge so to handle TRN_ITEMIZE_NORMAL or TRN_ITEMIZE_REDUCE properly
	} else {
		lDiscSign = -1L;      // discount so decrement
		lSign *= (-1L);       // invert sign for a discount so to handle TRN_ITEMIZE_NORMAL or TRN_ITEMIZE_REDUCE properly
	}

    switch (pItemDisc->uchMinorClass) {
    case CLASS_REGDISC1:
		// offset to first half of the array where discount #1 totals are maintained.
        plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[0];
        uchRegDiscAdr = RND_REGDISC1_ADR;
        iuchDisc = 0;          // Transaction Discount #1 uses discount itemizer #1 always
        break;

    case CLASS_REGDISC2:
		// offset to second half of the array where discount #2 totals are maintained.
        plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[STD_TAX_ITEMIZERS_US];
        uchRegDiscAdr = RND_REGDISC1_ADR;
        iuchDisc = 1;          // Transaction Discount #2 uses discount itemizer #2 always
        break;

    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        if (pItemDisc->auchDiscStatus[1] & ODD_MDC_BIT1) {       // DISC_AFFECT_REGDISC_AFFECT
			// NOT use main itemizer so pick either discount itemizer #1 or #2 depending on MDC setting.
            if (pItemDisc->auchDiscStatus[1] & ODD_MDC_BIT2) {   // DISC_AFFECT_REGDISC_1OR2
				// offset to second half of the array where discount #2 totals are maintained.
                plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[STD_TAX_ITEMIZERS_US];
                iuchDisc = 1;       // Transaction Discount #3, 4, 5, or 6 - MDC setting indicates affect discount itemizer #2
            } else {
				// offset to first half of the array where discount #1 totals are maintained.
                plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[0];
                iuchDisc = 0;       // Transaction Discount #3, 4, 5, or 6 - MDC setting indicates affect discount itemizer #1
            }
        } else {
			USHORT  usIndex;

			/* 03/31/01, LePeeps */
			/* main itemizer type discount */
			for ( usIndex = 0; usIndex < STD_TAX_ITEMIZERS_US; usIndex++){
				alDiscTaxable[usIndex]  = pTrn->Itemizers.TranUsItemizers.lAffectTaxable[usIndex];
				alDiscTaxable[usIndex] += pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[usIndex];
			}
			plDiscTaxable = alDiscTaxable;
            /* plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[0]; */
            sUseMI = 1;
        }
        uchRegDiscAdr = RND_REGDISC1_ADR;
        break;

    default:
        return;
    }

    /* ----- food stampable ----- */
    if (fchFood) {
        pTrn->Itemizers.TranUsItemizers.lFoodStampable += pTrn->Itemizers.TranUsItemizers.lFSDiscountable[iuchDisc] * lSign;
    }

    /* --- affect discountable/taxable item according to its status --- */
	{
		USHORT  usIndex;
		UCHAR   uchCheckBit;

		uchCheckBit = 0x01;
		for (usIndex = 0; usIndex < STD_TAX_ITEMIZERS_US; usIndex++, uchCheckBit <<= 1 ) {
			if ( uchTaxableSts & uchCheckBit ) {
				DCURRENCY    lAmount;

				/* TAR191625 */
				if (pItemDisc->VoidDisc.USVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
					lAmount = pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usIndex] * lDiscSign;
				} else {
					DCURRENCY    lDiscTaxAmt = *(plDiscTaxable + usIndex);
    				RflRateCalc1(&lAmount, lDiscTaxAmt, lRate, uchRegDiscAdr);
				}

				*(plNonAffectTaxable + usIndex) += lAmount * lSign;

				/* TAR191625 */
				if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
					*(plVoidDiscNonAffectTaxable + usIndex) += lAmount * lSign;
				}

				if ((pItemDisc->VoidDisc.USVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
					pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usIndex] = lAmount * lSign;
				}

				/* --- Use MI or SI,    R2.0 ----- */
				if (sUseMI == 1) {
					/* plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[usIndex]; */
					plFSDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lFoodTaxable[usIndex];
				} else {
					/* plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[iuchDisc * usIndex]; */
					plFSDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[iuchDisc][usIndex];
				}
				/* TAR191625 */
				plVoidDiscFSDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lVoidDiscFSDiscTaxable[uchMinorClass][usIndex];

				/* ----- check food stampable ----- */
				if (fchFood) {
					pTrn->Itemizers.TranUsItemizers.lFoodTaxable[usIndex] += *plFSDiscTaxable * lSign;

					/* ----- food stampable whole taxable ----- */
					pTrn->Itemizers.TranUsItemizers.lFoodWholeTax += *plFSDiscTaxable * lSign;
					
					/* TAR191625 */
					if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
						*plVoidDiscFSDiscTaxable += *plFSDiscTaxable * lSign;
    				}
	           		pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usIndex] = *plFSDiscTaxable * lSign;
				}
			}
		}
	}

	/* TAR191625 */
    pItemDisc->VoidDisc.USVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
}

/*
*==========================================================================
**    Synopsis: VOID TrnRegDiscTaxAmtComn( ITEMDISC *pItemDisc )
*
*    Input:     ITEMDISC    *pItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   calculate discountable/taxable (amount override)
*    TAR191625 Canadian Tax is not correct after voided discount operation.
*==========================================================================
*/
VOID    TrnRegDiscTaxAmtComn(ITEMDISC *pItemDisc, SHORT sType, LONG lSign)
{
    UCHAR   uchCheckBit, fchFood;
    UCHAR   uchTaxableSts;
    DCURRENCY     *plNonAffectTaxable;
    DCURRENCY     *plDiscTaxable;
    TRANITEMIZERS *pTrn, *pTrnOut;
    DCURRENCY     alDiscTaxable[STD_TAX_ITEMIZERS_MAX] = {0}; /* 03/31/01, LePeeps */
	/* TAR191625 */
    DCURRENCY     *plVoidDiscNonAffectTaxable, *plVoidDiscFSDiscTaxable;
    UCHAR   uchMinorClass;

	uchMinorClass = pItemDisc->uchMinorClass - CLASS_REGDISC1;

	NHPOS_ASSERT_TEXT((uchMinorClass < STD_DISC_ITEMIZERS_MAX), "**ERROR - TrnRegDiscTaxAmtComn(): uchMinorClass out of range.");

    /* --- Food Stampable --- */
    fchFood = pItemDisc->auchDiscStatus[1] & 0x08;   // DISC_AFFECT_REGDISC_FOOD  Transaction Discount Food Stamp indicator, MDC page 3 bit A, US Tax only.

    /*----- Decide Target File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnSplitA.TranItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pTrn = &TrnSplitB.TranItemizers;
        pTrnOut = &TrnSplitB.TranItemizers;
    } else if (pItemDisc->uchSeatNo != 0) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    } else {
        pTrn = &TrnInformation.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    }

    /* --- retrieve discount item's taxable status --- */
    uchTaxableSts      = ( UCHAR )( pItemDisc->auchDiscStatus[ 0 ] >> 2 );
    plNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lNonAffectTaxable[ 0 ];
	/* TAR191625 */
    plVoidDiscNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lVoidDiscNonAffectTaxable[uchMinorClass][0];

    switch ( pItemDisc->uchMinorClass ) {
    case CLASS_REGDISC1:
		// offset to first half of the array where discount #1 totals are maintained.
        plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[0];
        break;

    case CLASS_REGDISC2:
		// offset to second half of the array where discount #2 totals are maintained.
        plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[STD_TAX_ITEMIZERS_US];
        break;

    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        if (pItemDisc->auchDiscStatus[1] & ODD_MDC_BIT1) {       // DISC_AFFECT_REGDISC_AFFECT
			// NOT use main itemizer so pick either discount itemizer #1 or #2 depending on MDC setting.
            if (pItemDisc->auchDiscStatus[1] & ODD_MDC_BIT2) {   // DISC_AFFECT_REGDISC_1OR2
				// offset to second half of the array where discount #2 totals are maintained.
                plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[STD_TAX_ITEMIZERS_US];
            } else {
				// offset to first half of the array where discount #1 totals are maintained.
                plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lDiscTaxable[0];
            }
        } else {
			USHORT  usTaxableIndex;

			/* 03/31/01, LePeeps */
			/* main itemizer type discount */
			for ( usTaxableIndex = 0; usTaxableIndex < STD_TAX_ITEMIZERS_US; usTaxableIndex++){
				alDiscTaxable[usTaxableIndex]  = pTrn->Itemizers.TranUsItemizers.lAffectTaxable[usTaxableIndex];
				alDiscTaxable[usTaxableIndex] += pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[usTaxableIndex];
			}
			plDiscTaxable = alDiscTaxable;
            /* plDiscTaxable = &pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[0]; */
        }
        break;

    default:
        break;
    }

    /* --- affect discountable/taxable item according to its status --- */
    uchCheckBit = 0x01;

	/* TAR191625 */
    if (pItemDisc->VoidDisc.USVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {	/* for split tender */
		USHORT  usTaxableIndex;

	    for (usTaxableIndex = 0; usTaxableIndex < STD_TAX_ITEMIZERS_US; usTaxableIndex++, uchCheckBit <<= 1 ){
	        /* if ( uchTaxableSts & uchCheckBit ) { */

	            *(plNonAffectTaxable + usTaxableIndex) += pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] * lSign;
				if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	        	    *(plVoidDiscNonAffectTaxable + usTaxableIndex) += pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] * lSign;
	        	}

	            plVoidDiscFSDiscTaxable = &pTrnOut->Itemizers.TranUsItemizers.lVoidDiscFSDiscTaxable[uchMinorClass][usTaxableIndex];

	            /* ----- check food stampable ----- */
	            /* if (fchFood) { */
	                pTrnOut->Itemizers.TranUsItemizers.lFoodTaxable[usTaxableIndex] += pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usTaxableIndex] * lSign;

	                /* ----- food stampable whole taxable ----- */
	                pTrnOut->Itemizers.TranUsItemizers.lFoodWholeTax += pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usTaxableIndex] * lSign;

					if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
			            *plVoidDiscFSDiscTaxable += pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usTaxableIndex] * lSign;
    		        }
	            /* } */
	        /* } */
	    }
	} else {
		USHORT     usTaxableIndex;
		DCURRENCY  lAmount = pItemDisc->lAmount;

		for ( usTaxableIndex = 0; usTaxableIndex < STD_TAX_ITEMIZERS_US; usTaxableIndex++, uchCheckBit <<= 1 ){
			if ( uchTaxableSts & uchCheckBit ) {
				if ( TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {
					if ( 0L <= *( plDiscTaxable + usTaxableIndex ) + lAmount*lSign ) {
						/* TAR191625 */
						if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
    	    				*(plVoidDiscNonAffectTaxable + usTaxableIndex) -= *( plDiscTaxable + usTaxableIndex ) * lSign;
    	    			}
            			pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] -= *( plDiscTaxable + usTaxableIndex ) * lSign;
						*( plNonAffectTaxable + usTaxableIndex ) -= *( plDiscTaxable + usTaxableIndex ) * lSign;
						/* *( plNonAffectTaxable + lTaxableIndex ) = 0L; */
					} else {
						*( plNonAffectTaxable + usTaxableIndex ) += lAmount * lSign;
						/* TAR191625 */
						if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	        				*(plVoidDiscNonAffectTaxable + usTaxableIndex) += lAmount * lSign;
	        			}
            			pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] = lAmount * lSign;
					}
				} else {
					if ( *( plDiscTaxable + usTaxableIndex ) + lAmount*lSign <= 0L ) {
						/* TAR191625 */
						if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	        				*(plVoidDiscNonAffectTaxable + usTaxableIndex) -= *( plDiscTaxable + usTaxableIndex ) * lSign;
	        			}
            			pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] -= *( plDiscTaxable + usTaxableIndex ) * lSign;
						*( plNonAffectTaxable + usTaxableIndex ) -= *( plDiscTaxable + usTaxableIndex ) * lSign;
						/* *( plNonAffectTaxable + usTaxableIndex ) = 0L; */
					} else {
						*( plNonAffectTaxable + usTaxableIndex ) += lAmount * lSign;
						/* TAR191625 */
						if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	        				*(plVoidDiscNonAffectTaxable + usTaxableIndex) += lAmount * lSign;
	        			}
            			pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usTaxableIndex] = lAmount * lSign;
					}
				}
				/* --- Saratoga ---  */
				/* TAR191625 */
				plVoidDiscFSDiscTaxable = &TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lVoidDiscFSDiscTaxable[uchMinorClass][usTaxableIndex];
				if (fchFood) {
					TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += pItemDisc->lAmount * lSign;
					TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodTaxable[usTaxableIndex] += pItemDisc->lAmount * lSign;
					
					/* TAR191625 */
					if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
						*plVoidDiscFSDiscTaxable += pItemDisc->lAmount * lSign;
    				}
            		pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usTaxableIndex] = pItemDisc->lAmount * lSign;
				}
			}
		}

		/* TAR191625 */
		pItemDisc->VoidDisc.USVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
	}
}

/*
*==========================================================================
**    Synopsis: VOID TrnRegDiscTaxAmtComn( ITEMDISC *pItemDisc )
*
*    Input:     ITEMDISC    *pItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   calculate discountable/taxable (amount override)
*    TAR191625 Canadian Tax is not correct after voided discount operation.
*==========================================================================
*/
VOID    TrnRegDiscTaxAmtVoidComn(CONST ITEMDISC *pItemDisc, SHORT sType, LONG lSign)
{
    UCHAR   uchCheckBit, uchTaxableSts, fchFood;
    UCHAR   uchMinorClass;
    USHORT        usIndex; 
    DCURRENCY     *plNonAffectTaxable; 
    TRANITEMIZERS *pTrn, *pTrnOut;
    DCURRENCY     *plVoidDiscNonAffectTaxable, *plVoidDiscFSDiscTaxable;

	uchMinorClass = pItemDisc->uchMinorClass - CLASS_REGDISC1;

	NHPOS_ASSERT_TEXT((uchMinorClass < STD_DISC_ITEMIZERS_MAX), "**ERROR - TrnRegDiscTaxAmtVoidComn(): uchMinorClass out of range.");

	/* --- Food Stampable --- */
    fchFood = pItemDisc->auchDiscStatus[1] & 0x08;   // DISC_AFFECT_REGDISC_FOOD  Transaction Discount Food Stamp indicator, MDC page 3 bit A, US Tax only.

    /*----- Decide Target File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnSplitA.TranItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pTrn = &TrnSplitB.TranItemizers;
        pTrnOut = &TrnSplitB.TranItemizers;
    } else if (pItemDisc->uchSeatNo != 0) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    } else {
        pTrn = &TrnInformation.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    }

    /* --- retrieve discount item's taxable status --- */
    uchTaxableSts      = ( UCHAR )( pItemDisc->auchDiscStatus[ 0 ] >> 2 );
    plNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lNonAffectTaxable[ 0 ];

    plVoidDiscNonAffectTaxable = &pTrnOut->Itemizers.TranUsItemizers.lVoidDiscNonAffectTaxable[uchMinorClass][0];

    /* --- affect discountable/taxable item according to its status --- */
    uchCheckBit = 0x01;
    for (usIndex = 0; usIndex < STD_TAX_ITEMIZERS_US; usIndex++, uchCheckBit <<= 1 ){
        /* if ( uchTaxableSts & uchCheckBit ) { */

			if ( pItemDisc->fbDiscModifier & VOIDSEARCH_MODIFIER ) {	/* void reg. discount is found */
				if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
		            *(plNonAffectTaxable + usIndex) -= *(plVoidDiscNonAffectTaxable + usIndex) * lSign;
		            *(plVoidDiscNonAffectTaxable + usIndex) = 0;
				} else {
            		*(plNonAffectTaxable + usIndex) -= pItemDisc->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[usIndex] * lSign;
		        }
	        }

            plVoidDiscFSDiscTaxable = &pTrnOut->Itemizers.TranUsItemizers.lVoidDiscFSDiscTaxable[uchMinorClass][usIndex];

            /* ----- check food stampable ----- */
            /* if (fchFood) { */
		    	if ( pItemDisc->fbDiscModifier & VOIDSEARCH_MODIFIER ) {	/* void reg. discount is found */
					if ((pItemDisc->uchSeatNo == 0) || (pItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
                		pTrnOut->Itemizers.TranUsItemizers.lFoodTaxable[usIndex] -= *plVoidDiscFSDiscTaxable * lSign;

	                	/* ----- food stampable whole taxable ----- */
    	            	pTrnOut->Itemizers.TranUsItemizers.lFoodWholeTax -= *plVoidDiscFSDiscTaxable * lSign;
			            *plVoidDiscFSDiscTaxable = 0;
					} else {
                		pTrnOut->Itemizers.TranUsItemizers.lFoodTaxable[usIndex] -= pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usIndex] * lSign;

	                	/* ----- food stampable whole taxable ----- */
    	            	pTrnOut->Itemizers.TranUsItemizers.lFoodWholeTax -= pItemDisc->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[usIndex] * lSign;
    	            }
				}
            /* } */
        /* } */
    }

}

/*
*==========================================================================
**    Synopsis: VOID    TrnRegDiscTaxCanComn( ITEMDISC *ItemDisc, SHORT sType, LONG lSign )
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   calculate discountable/taxable 
*    TAR191625 Canadian Tax is not correct after voided discount operation.
*
*                   lSign is used with one of two defines:
*                     TRN_ITEMIZE_NORMAL -> normal calculations
*                     TRN_ITEMIZE_REDUCE -> for reduce calculations
*==========================================================================
*/
VOID    TrnRegDiscTaxCanComn(ITEMDISC *ItemDisc, SHORT sType, LONG lSign)
{
    UCHAR   uchWorkMDC;                /* MDC settings as bit map */
	UCHAR    uchWorkBit;               /* mask bit */
    UCHAR   uchMinorClass;
    TRANITEMIZERS *pTrn, *pTrnOut;
    DCURRENCY     *pDisc;
    LONG    i;
	LONG    lRedSign;
	LONG    lDiscSign;
    ULONG   lRate;
    DCURRENCY   lDiscountable = 0;

	uchMinorClass = ItemDisc->uchMinorClass - CLASS_REGDISC1;

	NHPOS_ASSERT_TEXT((uchMinorClass < STD_DISC_ITEMIZERS_MAX), "**ERROR - TrnRegDiscTaxCanComn(): uchMinorClass out of range.");

    /*----- Decide Target File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnSplitA.TranItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pTrn = &TrnSplitB.TranItemizers;
        pTrnOut = &TrnSplitB.TranItemizers;
    } else if (ItemDisc->uchSeatNo != 0) {
        pTrn = &TrnSplitA.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    } else {
        pTrn = &TrnInformation.TranItemizers;
        pTrnOut = &TrnInformation.TranItemizers;
    }

    lRate = ((LONG)(ItemDisc->uchRate) * RFL_DISC_RATE_MUL);

    uchWorkMDC = ItemDisc->auchDiscStatus[1];                           /* taxable status */
    uchWorkMDC >>= 3;                                               /* shift */

	lRedSign = lSign;

	// whether itemizers are incremented or decremented depends on whether this is a Surcharge or a Discount.
    if (ItemDisc->auchDiscStatus[1] & DISC_SURCHARGE_KEY) {
	    lDiscSign = 1L;       // surcharge so increment
	    lSign *= 1L;          // keep sign for a surcharge so to handle TRN_ITEMIZE_NORMAL or TRN_ITEMIZE_REDUCE properly
	} else {
		lDiscSign = -1L;      // discount so decrement
		lSign *= (-1L);       // invert sign for a discount so to handle TRN_ITEMIZE_NORMAL or TRN_ITEMIZE_REDUCE properly
	}

    switch (ItemDisc->uchMinorClass) {
    case CLASS_REGDISC1:
        /* --- V3.3 --- */
        pDisc = &pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0];
        lDiscountable = pTrn->lDiscountable[0];
        break;

    case CLASS_REGDISC2:
        pDisc = &pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0];
        lDiscountable = pTrn->lDiscountable[1];
        break;

    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        if (ItemDisc->auchDiscStatus[1] & ODD_MDC_BIT1) {       // DISC_AFFECT_REGDISC_AFFECT
			// NOT use main itemizer so pick either discount itemizer #1 or #2 depending on MDC setting.
            if (ItemDisc->auchDiscStatus[1] & ODD_MDC_BIT2) {   // DISC_AFFECT_REGDISC_1OR2
				// offset to where discount #2 totals are maintained.
                pDisc = &pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[1][0];
		        lDiscountable = pTrn->lDiscountable[1];
            } else {
				// offset to where discount #1 totals are maintained.
                pDisc = &pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[0][0];
		        lDiscountable = pTrn->lDiscountable[0];
            }
        } else {
            pDisc = &pTrn->Itemizers.TranCanadaItemizers.lDiscTaxable[2][0];
			lDiscountable = pTrn->lMI;
		    if (sType == TRN_TYPE_SPLITA) {
				lDiscountable -= ItemDisc->lAmount;
		    } else if (sType == TRN_TYPE_SPLITB) {
				lDiscountable -= ItemDisc->lAmount;
		    } else if (ItemDisc->uchSeatNo != 0) {
				;	/* not affected to MI */
		    } else {
				lDiscountable -= ItemDisc->lAmount;
		    }
        	for (i = 0; i < 4; i++) {
	        	lDiscountable -= pTrn->Itemizers.TranCanadaItemizers.lAffTax[i];
	        }
        }
        break;

    default:
        return;
    }

	/* emulate rate from MI */
    if (lRate == 0) {
		if (lDiscountable) {
			D13DIGITS huWork = RflLLabs(ItemDisc->lAmount);

			huWork *= 100L;
			huWork /= RflLLabs(lDiscountable);
    		lRate = huWork;
			lRate *= RFL_DISC_RATE_MUL;
		}
	}

	/* TAR191625 */
    if ( ItemDisc->fbDiscModifier & VOID_MODIFIER ) {
    	if ( ItemDisc->fbDiscModifier & VOIDSEARCH_MODIFIER ) {	/* void reg. discount is found */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
				uchWorkBit = 0x01;                                              /* mask bit */
        		for (i = 0L; i < 5L; i++, uchWorkBit <<= 1) {
            		/* if (uchWorkMDC & uchWorkBit) { */
                		pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[i]
                    		-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][i] * lRedSign;
					/* } */
    	        }
    		    /* if (uchWorkMDC & 0x01) { */
	        	    pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[5]
            		    -= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][5] * lRedSign;

	        	    pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[6]
    		            -= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][6] * lRedSign;
        
	    	        pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[7]
            		    -= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][7] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[8]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][8] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[9]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][9] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[10]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][10] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[11]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][11] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[12]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][12] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[13]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][13] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[14]
    	            	-= pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][14] * lRedSign;

		        /* } */
			} else {
				uchWorkBit = 0x01;                                              /* mask bit */
        		for (i = 0L; i < 5L; i++, uchWorkBit <<= 1) {
            		/* if (uchWorkMDC & uchWorkBit) { */
                		pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[i]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[i] * lRedSign;
					/* } */
    	        }
    		    /* if (uchWorkMDC & 0x01) { */
	        	    pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[5]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[5] * lRedSign;

	        	    pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[6]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[6] * lRedSign;
        
	    	        pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[7]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[7] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[8]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[8] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[9]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[9] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[10]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[10] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[11]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[11] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[12]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[12] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[13]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[13] * lRedSign;

        	    	pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[14]
                			-= ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[14] * lRedSign;

		        /* } */
		    }
        }

		if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	        for (i = 0L; i < (STD_PLU_ITEMIZERS+2+3); i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
    	        pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][i] = 0;
        	}
        }
    } else if ( lRate != 0L ) {                                            /* rate calculation */
		uchWorkBit = 0x01;                                              /* mask bit */
        for (i = 0L; i < 5L; i++, uchWorkBit <<= 1) {
            if (uchWorkMDC & uchWorkBit) {
				DCURRENCY   lAmount;

				/* TAR191625 */
            	if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
					lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[i] * lDiscSign;
				} else {
	                RflRateCalc1(&lAmount, *(pDisc+i), lRate, (UCHAR)RND_REGDISC1_ADR); 
	            }
                pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[i]
                    += lAmount * lSign;
				/* TAR191625 */
				if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	                pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][i]
    	                += lAmount * lSign;
    	        }
            	if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
                	ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[i] = lAmount * lSign;
                }
            }
        }
        if (uchWorkMDC & 0x01) {            /* 5/7/96 */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_GROCERY] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc + TRANCANADA_GROCERY), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[TRANCANADA_GROCERY] += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][TRANCANADA_GROCERY] += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_GROCERY] = lAmount * lSign;
	        }

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_TIP] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc + TRANCANADA_TIP), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[TRANCANADA_TIP] += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][TRANCANADA_TIP] += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_TIP] = lAmount * lSign;
	        }
        
			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_PST1_ONLY] *= lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc + TRANCANADA_PST1_ONLY), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[TRANCANADA_PST1_ONLY] += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][TRANCANADA_PST1_ONLY] += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_PST1_ONLY] = lAmount * lSign;
	        }

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_BAKED] *= lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc + TRANCANADA_BAKED), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[TRANCANADA_BAKED] += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][TRANCANADA_BAKED] += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_BAKED] = lAmount * lSign;
	        }

#if 0
			// do we need to add the following source to handle the new Canadian Tax bucket for
			// Special Baked Goods (added for Rel 2.0.3 by JHHJ) for CT/OT tax enhancements?
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_SPECL_BAKED] *= lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc + TRANCANADA_SPECL_BAKED), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[TRANCANADA_SPECL_BAKED] += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][TRANCANADA_SPECL_BAKED] += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[TRANCANADA_SPECL_BAKED] = lAmount * lSign;
	        }
#endif
		}
		
        if ( !CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT2 ) &&      /* limitable */
			 !CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 ))  {
             uchWorkBit = 0x02;
        } else if ( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT2 ) &&      /* non-limitable */
			 CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 ))  {
             uchWorkBit = 0x04;
        } else {													/* limitale or non-limitable */
			uchWorkBit = 0x02|0x04;
		}
		
        if (uchWorkMDC & uchWorkBit) {	/* carbonated beverage limitable or non-limitable - should the index 9 be replaced by TRANCANADA_LIMITABLE? */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[9] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc+9), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[9]
                += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][9]
    	            += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[9] = lAmount * lSign;
	        }
        }
        if (uchWorkMDC & 0x01) {	/* carbonated beverage limitable or non-limitable, tax on tax - should the index 10 be replaced by TRANCANADA_TAXONTAX? */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[10] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc+10), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[10]
                += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][10]
    	            += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[10] = lAmount * lSign;
	        }
		}
        if (uchWorkMDC & 0x01) {	/* baked item, GST - should the index 11 be replaced by TRANCANADA_GST? */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[11] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc+11), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[11]
                += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][11]
    	            += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[11] = lAmount * lSign;
	        }
		}
        if (uchWorkMDC & 0x02) {	/* baked item, PST1 - should the index 12 be replaced by TRANCANADA_PST1LIMIT? */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[12] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc+12), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[12]
                += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][12]
    	            += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[12] = lAmount * lSign;
	        }
		}
        if (uchWorkMDC & 0x01) {	/* baked item, tax on tax - should the index 13 be replaced by TRANCANADA_PST1TOT? */
			DCURRENCY   lAmount;

			/* TAR191625 */
            if (ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
            	lAmount = ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[13] * lDiscSign;
            } else {
	            RflRateCalc1(&lAmount, *(pDisc+13), lRate, (UCHAR)RND_REGDISC1_ADR);
	        }
            pTrnOut->Itemizers.TranCanadaItemizers.lDiscTax[13]
                += lAmount * lSign;
			/* TAR191625 */
			if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	            pTrnOut->Itemizers.TranCanadaItemizers.lVoidDiscTax[uchMinorClass][13]
    	            += lAmount * lSign;
    	    }
            if ((ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            ItemDisc->VoidDisc.CanVoidDisc.lVoidDiscTax[13] = lAmount * lSign;
	        }
		}
    }

	/* TAR191625 */
    ItemDisc->VoidDisc.CanVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
}

/*
*==========================================================================
**    Synopsis: VOID    TrnRegDiscVATCalc(ITEMDISC *pItem, SHORT sType, LONG lSign)
*
*    Input:     ITEMDISC    *pItem
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   Discount S.I. Affection
*
*                   lSign is used with one of two defines:
*                     TRN_ITEMIZE_NORMAL -> normal calculations
*                     TRN_ITEMIZE_REDUCE -> for reduce calculations
*
*       This function includes the following conditions:
*
*           - RATE discount (+ & -)
*               - for VATable and Serviceable
*               - for Non VATable and Serviceable
*               - for VATable and Non Serviceable
*
*           - Amount discount (+, void discount or amount surcharge)
*               - for VATable and Serviceable
*               - for Non VATable and Serviceable
*               - for VATable and Non Serviceable
*
*           - Amount discount (-, amount discount or void surcharge)
*               - for VATable and Serviceable
*               - for Non VATable and Serviceable
*               - for VATable and Non Serviceable                   V3.3
*==========================================================================
*/
VOID    TrnRegDiscVATCalcComn(ITEMDISC *pItem, SHORT sType, LONG lSign)
{
    UCHAR               uchBit;
    UCHAR               uchMinorClass;
    SHORT               i, sSI;
    LONG                lPVoid;
	LONG                lDiscSign;
    TRANINTLITEMIZERS   *pInt, *pIntOut;
    DCURRENCY           lAmt, lNonVS, lAmount;

	uchMinorClass = pItem->uchMinorClass - CLASS_REGDISC1;

	NHPOS_ASSERT_TEXT((uchMinorClass < STD_DISC_ITEMIZERS_MAX), "**ERROR - TrnRegDiscVATCalcComn(): uchMinorClass out of range.");

    if (pItem->lAmount == 0) {
        return;
    }

	// whether itemizers are incremented or decremented depends on whether this is a Surcharge or a Discount.
    if (pItem->auchDiscStatus[1] & DISC_SURCHARGE_KEY) {
	    lDiscSign = 1L;       // surcharge so increment
	} else {
		lDiscSign = -1L;      // discount so decrement
	}

    if (sType == TRN_TYPE_SPLITA) {
        pInt = &TrnSplitA.TranItemizers.Itemizers.TranIntlItemizers;
        pIntOut = &TrnSplitA.TranItemizers.Itemizers.TranIntlItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pInt = &TrnSplitB.TranItemizers.Itemizers.TranIntlItemizers;
        pIntOut = &TrnSplitB.TranItemizers.Itemizers.TranIntlItemizers;
    } else if (pItem->uchSeatNo != 0) {
        pInt = &TrnSplitA.TranItemizers.Itemizers.TranIntlItemizers;
        pIntOut = &TrnInformation.TranItemizers.Itemizers.TranIntlItemizers;
    } else {
        pInt = &TrnInformation.TranItemizers.Itemizers.TranIntlItemizers;
        pIntOut = &TrnInformation.TranItemizers.Itemizers.TranIntlItemizers;
    }
    uchBit = 0x04;   // mask for DISC_AFFECT_TAXABLE1, DISC_AFFECT_TAXABLE2, and DISC_AFFECT_TAXABLE3?
                                        /* get Non VAT & Servicable */
    switch (pItem->uchMinorClass) {
    case    CLASS_REGDISC1:
        lNonVS = pInt->lDiscountable_Ser[0] 
            - (pInt->lDisSer[0][0] + pInt->lDisSer[0][1] + pInt->lDisSer[0][2]);
        sSI = 0;
        break;

    case    CLASS_REGDISC2:
        lNonVS = pInt->lDiscountable_Ser[1] 
            - (pInt->lDisSer[1][0] + pInt->lDisSer[1][1] + pInt->lDisSer[1][2]);
        sSI = 1;
        break;

    case    CLASS_REGDISC3:
    case    CLASS_REGDISC4:
    case    CLASS_REGDISC5:
    case    CLASS_REGDISC6:
        if (pItem->auchDiscStatus[1] & ODD_MDC_BIT1) {       // DISC_AFFECT_REGDISC_AFFECT
			// NOT use main itemizer so pick either discount itemizer #1 or #2 depending on MDC setting.
            if (pItem->auchDiscStatus[1] & ODD_MDC_BIT2) {   // DISC_AFFECT_REGDISC_1OR2
				// use discount #2 totals.
                lNonVS = pInt->lDiscountable_Ser[1] - (pInt->lDisSer[1][0]
                    + pInt->lDisSer[1][1] + pInt->lDisSer[1][2]);
                sSI = 1;
            } else {
				// use discount #1 totals.
                lNonVS = pInt->lDiscountable_Ser[0] - (pInt->lDisSer[0][0] 
                    + pInt->lDisSer[0][1] + pInt->lDisSer[0][2]);
                sSI = 0;
            }
        } else {
            lNonVS = pInt->lDiscountable_Ser[2] - (pInt->lDisSer[2][0]
                + pInt->lDisSer[2][1] + pInt->lDisSer[2][2]);
            sSI = 2;
        }
        break;

    default:
        break;
    }
    
	/* TAR191625 */
    if ( pItem->fbDiscModifier & VOID_MODIFIER ) {
    	if ( pItem->fbDiscModifier & VOIDSEARCH_MODIFIER ) {	/* void reg. discount is found */
    	
			if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */

	        	/** VAT & SERVICE **/
		        for (i = 0; i < 3; i++) {
    		        /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
        		        continue;
            		} */
	        	    pIntOut->lVATSer[i] -= pIntOut->lVoidVATSer[uchMinorClass][i] * lSign;
    	    	}

	    	    /** Non VAT & SERVICE **/
           		pIntOut->lServiceable -= pIntOut->lVoidServiceable[uchMinorClass] * lSign;

	    	    /** VAT & Non SERVICE **/
    	    	for (i = 0; i < 3; i++) {
	    	        /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
    	    	        continue;
        	    	} */
	    	        pIntOut->lVATNon[i] -= pIntOut->lVoidVATNon[uchMinorClass][i] * lSign;
		        }
		        
		        for (i = 0; i < 3; i++) {
	        	    pIntOut->lVoidVATSer[uchMinorClass][i] = 0;
	    	        pIntOut->lVoidVATNon[uchMinorClass][i] = 0;
		        }
           		pIntOut->lVoidServiceable[uchMinorClass] = 0;

		    } else {

	        	/** VAT & SERVICE **/
		        for (i = 0; i < 3; i++) {
    		        /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
        		        continue;
            		} */
	        	    pIntOut->lVATSer[i] -= pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] * lSign;
    	    	}

	    	    /** Non VAT & SERVICE **/
           		pIntOut->lServiceable -= pItem->VoidDisc.IntlVoidDisc.lVoidServiceable * lSign;

	    	    /** VAT & Non SERVICE **/
    	    	for (i = 0; i < 3; i++) {
	    	        /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
    	    	        continue;
        	    	} */
	    	        pIntOut->lVATNon[i] -= pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] * lSign;
		        }
		        
			}
		    
    		return;
    	}
    }

    /*************************************
    *   RATE Discount S.I Affection      *
    *   (including TVOID)                *
    *************************************/

    if (0 != pItem->uchRate) {
        lSign *= (pItem->auchDiscStatus[1] & DISC_SURCHARGE_KEY) ? 1L : -1L;

        /** VAT & SERVICE **/
        for (i = 0; i < 3; i++) {
            if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
                continue;
            }
			/* TAR191625 */
            if (pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
				lAmt = pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] * lDiscSign;
			} else {
	            RflRateCalc1(&lAmt, pInt->lDisSer[sSI][i], pItem->uchRate * RFL_DISC_RATE_MUL, (UCHAR)RND_REGDISC1_ADR);
    	    }
            pIntOut->lVATSer[i] += lAmt * lSign;
            if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            pIntOut->lServiceable += lAmt * lSign;
	        }
			if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	            pIntOut->lVoidVATSer[uchMinorClass][i] += lAmt * lSign;
	            if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
					pIntOut->lVoidServiceable[uchMinorClass] += lAmt * lSign;
				}
			}
            if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] = lAmt * lSign;
	            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmt * lSign;
	        }
        }

        /** Non VAT & SERVICE **/
		/* TAR191625 */
        if (pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
			lAmt = pItem->VoidDisc.IntlVoidDisc.lVoidServiceable * lDiscSign;
		} else {
	        RflRateCalc1(&lAmt, lNonVS, pItem->uchRate * RFL_DISC_RATE_MUL, (UCHAR)RND_REGDISC1_ADR);
	    }
        pIntOut->lServiceable += lAmt * lSign;
		if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
			pIntOut->lVoidServiceable[uchMinorClass] += lAmt * lSign;
		}
        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmt * lSign;
        }

        /** VAT & Non SERVICE **/
        for (i = 0; i < 3; i++) {
            if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
                continue;
            }
			/* TAR191625 */
    	    if (pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {
				lAmt = pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] * lDiscSign;
			} else {
        	    RflRateCalc1(&lAmt, pInt->lDisNon[sSI][i], pItem->uchRate * RFL_DISC_RATE_MUL, (UCHAR)RND_REGDISC1_ADR);
            }
            pIntOut->lVATNon[i] += lAmt * lSign;
			if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
    	        pIntOut->lVoidVATNon[uchMinorClass][i] += lAmt * lSign;
    	    }
	        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] = lAmt * lSign;
	        }
        }

		/* TAR191625 */
	    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
        return;
    }

	/* TAR191625 */
	if (pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) {	/* at split check tender */

        /** VAT & SERVICE **/
        for (i = 0; i < 3; i++) {
            /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
                continue;
            } */
            pIntOut->lVATSer[i] += pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] * lSign;

			if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	            pIntOut->lVoidVATSer[uchMinorClass][i] += pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] * lSign;
			}
        }

        /** Non VAT & SERVICE **/
        pIntOut->lServiceable += pItem->VoidDisc.IntlVoidDisc.lVoidServiceable * lSign;
		if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
			pIntOut->lVoidServiceable[uchMinorClass] += pItem->VoidDisc.IntlVoidDisc.lVoidServiceable * lSign;
		}
		
        /** VAT & Non SERVICE **/
        for (i = 0; i < 3; i++) {
            /* if (0 == (pItem->auchStatus[0] & (uchBit << i))) {
                continue;
            } */
            pIntOut->lVATNon[i] += pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] * lSign;
			if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
    	        pIntOut->lVoidVATNon[uchMinorClass][i] += pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] * lSign;
    	    }
        }
		return;
	}

    /*************************************
    *  Void Discount or Amount Surcharge *
    *   (including TVOID)                *
    *************************************/
    if (TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK) {
        lPVoid = -1L;
    } else {
        lPVoid = 1L;
    }

    lAmount = pItem->lAmount;
    if (lAmount * lPVoid > 0) {

        /** VAT & SERVICE **/
        for (i = 0; i < 3; i++) {
            if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
                continue;
            }
            if (0L < pInt->lDisSer[sSI][i] * lPVoid) {
                pIntOut->lServiceable += lAmount * lSign;
                pIntOut->lVATSer[i] += lAmount * lSign;
				/* TAR191625 */
			    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
					if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
			            pIntOut->lVoidVATSer[uchMinorClass][i] += lAmount * lSign;
						pIntOut->lVoidServiceable[uchMinorClass] += lAmount * lSign;
					}
			        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
	            		pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] = lAmount * lSign;
			            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmount * lSign;
	    		    }
				}
			    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
                return;
            }
        }

        /** Non VAT & SERVICE */
        if (0L < lNonVS * lPVoid) {
            pIntOut->lServiceable += lAmount * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
					pIntOut->lVoidServiceable[uchMinorClass] += lAmount * lSign;
				}
        		if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmount * lSign;
        		}
			}
		    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
            return;
        }

        /** VAT & Non SERVICE **/
        for (i = 0; i < 3 && (lAmount * lPVoid) > 0; i++) {
            if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
                continue;
            }
            if (0L < pInt->lDisNon[sSI][i] * lPVoid) {
                pIntOut->lVATNon[i] += lAmount * lSign;
				/* TAR191625 */
			    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
					if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	    		        pIntOut->lVoidVATNon[uchMinorClass][i] += lAmount * lSign;
	    		    }
	    	    }
		        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] = lAmount * lSign;
	    	    }
			    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
                return;
            }
        }

	    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
        return;
    }

    /*************************************
    *  Amount Discount or Void Surcharge *
    *   (including TVOID)                *
    *************************************/

    /** VAT & SERVICE **/
    for (i = 0; i < 3 && (lAmount * lPVoid) < 0; i++) {
        if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
            continue;
        }
        if (0L > pInt->lDisSer[sSI][i] * lPVoid) {
            continue;
        }
        if (-lAmount * lPVoid > pInt->lDisSer[sSI][i] * lPVoid) {
            pIntOut->lVATSer[i] -= pInt->lDisSer[sSI][i] * lSign;
            pIntOut->lServiceable -= pInt->lDisSer[sSI][i] * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
		            pIntOut->lVoidVATSer[uchMinorClass][i] -= pInt->lDisSer[sSI][i] * lSign;
					pIntOut->lVoidServiceable[uchMinorClass] -= pInt->lDisSer[sSI][i] * lSign;
				}
			}
	        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
           		pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] -= pInt->lDisSer[sSI][i] * lSign;
	            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable -= pInt->lDisSer[sSI][i] * lSign;
   		    }
            lAmount += pInt->lDisSer[sSI][i] * lSign;
        } else {
            pIntOut->lVATSer[i] += lAmount * lSign;
            pIntOut->lServiceable += lAmount * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
		            pIntOut->lVoidVATSer[uchMinorClass][i] += lAmount * lSign;
					pIntOut->lVoidServiceable[uchMinorClass] += lAmount * lSign;
				}
			}
	        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
           		pItem->VoidDisc.IntlVoidDisc.lVoidVATSer[i] = lAmount * lSign;
	            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmount * lSign;
			}
            lAmount = 0L;
        }
    }
      
    /** Non VAT & SERVICE */
    if (lAmount * lPVoid < 0) {
        if (-lAmount * lPVoid > lNonVS * lPVoid) {
            pIntOut->lServiceable -= lNonVS * lSign;
            lAmount += lNonVS * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
					pIntOut->lVoidServiceable[uchMinorClass] -= lNonVS * lSign;
				}
        		if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable -= lNonVS * lSign;
        		}
			}
        } else {
            pIntOut->lServiceable += lAmount * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
					pIntOut->lVoidServiceable[uchMinorClass] += lAmount * lSign;
				}
        		if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidServiceable += lAmount * lSign;
        		}
			}
            lAmount = 0L;
        }
    }

    /** VAT & Non SERVICE **/
    for (i = 0; i < 3 && (lAmount * lPVoid) < 0; i++) {
        if (0 == (pItem->auchDiscStatus[0] & (uchBit << i))) {
            continue;
        }
        if (0L > pInt->lDisNon[sSI][i] * lPVoid) {
            continue;
        }
        if (-lAmount * lPVoid > pInt->lDisNon[sSI][i] * lPVoid) {
            pIntOut->lVATNon[i] -= pInt->lDisNon[sSI][i] * lSign;
            lAmount += pInt->lDisNon[sSI][i] * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	    	        pIntOut->lVoidVATNon[uchMinorClass][i] -= pInt->lDisNon[sSI][i] * lSign;
	    	    }
		        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] -= pInt->lDisNon[sSI][i] * lSign;
	    	    }
			}

        } else {
            pIntOut->lVATNon[i] += lAmount * lSign;
			/* TAR191625 */
		    if (( pItem->fbDiscModifier & VOID_MODIFIER ) == 0) {
				if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	    	        pIntOut->lVoidVATNon[uchMinorClass][i] += lAmount * lSign;
	    	    }
		        if ((pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus & ITM_DISC_TAXCALCED) == 0) {
		            pItem->VoidDisc.IntlVoidDisc.lVoidVATNon[i] = lAmount * lSign;
	    	    }
			}
        }
    }           

	/* TAR191625 */
    pItem->VoidDisc.IntlVoidDisc.fbVoidDiscStatus |= ITM_DISC_TAXCALCED;	/* for seat no. tender */
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnItemCouponUSComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:  The various status bits for a coupon are set up based on
*                  the type of the coupon (UPC coupon or Auto Coupon, or whatever).
*                  See function ItemCouponUPCSetupStatus() for how the coupon's status
*                  bits are set along with the modifiers.
*==========================================================================
*/
VOID    TrnItemCouponUSComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT       uchCheckBit, uchModifier, uchTaxableSts, uchDiscountSts, fchFood;
    USHORT       usTaxableIndex;

    /* --- update taxable itemizers --- */
    uchCheckBit    = TAX_MODIFIER1;
    uchModifier    = pItem->fbModifier;
    uchTaxableSts  = pItem->fbStatus[ 0 ];
    uchDiscountSts = pItem->fbStatus[ 1 ];
    /* corrected food stamp status at taxable FS, 04/04/01 */
    fchFood        = pItem->fbStatus[ 0 ] & FOOD_MODIFIER;
    /* fchFood        = pItem->fbStatus[ 1 ] & 0x04; */

    for ( usTaxableIndex = 0; usTaxableIndex < STD_TAX_ITEMIZERS_US; usTaxableIndex++, uchCheckBit <<= 1 ){
        if (( uchTaxableSts ^ uchModifier ) & uchCheckBit ) {
			pTrn->Itemizers.TranUsItemizers.lNonAffectTaxable[usTaxableIndex] += pItem->lAmount * lSign;
            if (fchFood) {
                pTrn->Itemizers.TranUsItemizers.lFoodTaxable[usTaxableIndex] += pItem->lAmount * lSign;
            }
            if ( uchDiscountSts & ITM_COUPON_AFFECT_DISC1 ) {
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[usTaxableIndex] += pItem->lAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[0][usTaxableIndex] += pItem->lAmount * lSign;
                }
            }
            if ( uchDiscountSts & ITM_COUPON_AFFECT_DISC2 ) {
                pTrn->Itemizers.TranUsItemizers.lDiscTaxable[usTaxableIndex + STD_TAX_ITEMIZERS_US] += pItem->lAmount * lSign;
                /* ----- check food stampable ----- */
                if (fchFood) {
                    pTrn->Itemizers.TranUsItemizers.lFSDiscTaxable[1][usTaxableIndex] += pItem->lAmount * lSign;
                }
            }
        }
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnItemCouponCanComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:  The various status bits for a coupon are set up based on
*                  the type of the coupon (UPC coupon or Auto Coupon, or whatever).
*                  See function ItemCouponUPCSetupStatus() for how the coupon's status
*                  bits are set along with the modifiers.
*==========================================================================
*/
VOID    TrnItemCouponCanComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    USHORT  uchWorkMDC, uchWorkBit = 0;

    uchWorkMDC = pItem->fbModifier;
    uchWorkMDC >>= 1;
    uchWorkMDC &= TRN_MASKSTATCAN;

    if (uchWorkMDC == 0) {
        uchWorkMDC = pItem->fbStatus[0];
        uchWorkMDC &= TRN_MASKSTATCAN;
        if (uchWorkMDC >STD_PLU_ITEMIZERS_MOD) {
            uchWorkMDC = STD_PLU_ITEMIZERS_MOD;
        }
        uchWorkMDC++;
    }

    pTrn->Itemizers.TranCanadaItemizers.lTaxable[uchWorkMDC - 1] += pItem->lAmount * lSign;

    if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC1) {
        uchWorkBit |= PLU_AFFECT_DISC1;
    }
    if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC2) {
        uchWorkBit |= PLU_AFFECT_DISC2;
    }

    TrnDiscTaxCanCom(uchWorkBit, uchWorkMDC, pItem->lAmount, pTrn, lSign);

}
/*
*==========================================================================
*    Synopsis:  VOID    TrnItemCouponIntlComn(ITEMCOUPON *pItem, TRANITEMIZERS *pTrn)
*
*    Input:     VOID
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   
*==========================================================================
*/
VOID    TrnItemCouponIntlComn(CONST ITEMCOUPON *pItem, TRANITEMIZERS *pTrn, LONG lSign)
{
    UCHAR   uchWorkMDC, uchWorkBit;

    uchWorkMDC = pItem->fbStatus[0];
    uchWorkMDC >>= 1;
    uchWorkMDC &= 0x03;
    uchWorkMDC -= 1;
    uchWorkBit = ITM_COUPON_AFFECT_DISC1;

    if (pItem->fbStatus[0] & ITM_COUPON_AFFECT_SERV) {
	   USHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lServiceable += pItem->lAmount * lSign;
        pTrn->Itemizers.TranIntlItemizers.lVATSer[uchWorkMDC] += pItem->lAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->fbStatus[1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisSer[i][uchWorkMDC] += pItem->lAmount * lSign;
                pTrn->Itemizers.TranIntlItemizers.lDiscountable_Ser[i] += pItem->lAmount * lSign;
            }
        }
    } else {
	   USHORT   i;

        pTrn->Itemizers.TranIntlItemizers.lVATNon[uchWorkMDC] += pItem->lAmount * lSign;
        for (i = 0; i < 2; i++, uchWorkBit <<= 1) {
            if (pItem->fbStatus[1] & uchWorkBit) {
                pTrn->Itemizers.TranIntlItemizers.lDisNon[i][uchWorkMDC] += pItem->lAmount * lSign;
            }
        }
    }

}

/* ===== End of File ( TRNCOMN.C ) ===== */