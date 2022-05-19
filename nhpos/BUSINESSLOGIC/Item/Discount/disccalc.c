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
* Title       : Calculation discount amount.                          
* Category    : Dicount Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: DISCCALU.C
* --------------------------------------------------------------------------
* Abstract: ItemDiscCalculation() : Discount amount calculation function    
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun-20-92: 00.00.01  : K.Maeda   : initial                                   
* Nov-08-93: 02.00.00  : K.You     : add charge posting feature.                        
* Nov-09-93: 03.01.00  : hkato     : R3.1
*** GenPOS **
* 09/20/17 : 02.02.02  : R.Chambers  : clean up. localized variables, use global pointers, comments.
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <regstrct.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <rfl.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCalculation(UIFREGDISC *pUifRegDisc, 
*                                       ITEMDISC *pItemDisc, 
*                                       LONG *plItemizer)
*
*    Input: UIFREGDISC *pUifRegDisc,
*           ITEMDISC *pItemDisc, 
*           LONG *plItemizer
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully, LDT_ error otherwise
*         LDT_PROHBT_ADR  : Action prohibitted.
*         LDT_KEYOVER_ADR : invalid data entered.
*         
** Description: Calculate discount amount. 
*===========================================================================
*/
SHORT   ItemDiscCalculation(ITEMDISC *pItemDisc, DCURRENCY *plItemizer)
{
    UCHAR    uchRoundType;

    /*--- CALCULATE AND ROUNDING DISCOUNT AMOUNT ---*/
    if (pItemDisc->uchRate != 0) {
		SHORT    sReturnStatus;

        switch(pItemDisc->uchMinorClass) {
        case CLASS_ITEMDISC1:
            uchRoundType = RND_ITEMDISC1_ADR;   
            break;
        
        case CLASS_REGDISC1:
        case CLASS_REGDISC2:
        case CLASS_REGDISC3:                   /* R3.1 */
        case CLASS_REGDISC4:
        case CLASS_REGDISC5:
        case CLASS_REGDISC6:
            uchRoundType = RND_REGDISC1_ADR;
            break;
        
        case CLASS_CHARGETIP:
        case CLASS_CHARGETIP2:
        case CLASS_CHARGETIP3:
        case CLASS_AUTOCHARGETIP:
        case CLASS_AUTOCHARGETIP2:
        case CLASS_AUTOCHARGETIP3:
            uchRoundType = RND_CHGTIP1_ADR;   
            break;
        
        default:
            uchRoundType = RND_REGDISC1_ADR;   
			PifLog (MODULE_DISCOUNT, LOG_EVENT_UI_DISCOUNT_ERR_05);
            break;
        }

        if ((sReturnStatus = RflRateCalc1(&pItemDisc->lAmount, *plItemizer, pItemDisc->uchRate * RFL_DISC_RATE_MUL, uchRoundType)) != RFL_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }

		/*--- HALO CHECK FOR AMOUNT OF DISCOUNT ---*/
		if ((sReturnStatus = ItemDiscHALOProc(&(pItemDisc->lAmount), pItemDisc)) != ITM_SUCCESS) {
			if (pItemDisc->uchMinorClass != CLASS_CHARGETIP) {
				if ((sReturnStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if Discount amount is over HALO
					return(sReturnStatus);
				}
			} else {
				return (sReturnStatus);
			}
		}
    }
    
    /*--- GET DISC/ SURCHARGE OPTION ---*/
	if ((pItemDisc->fbDiscModifier & (VOID_MODIFIER | RETURNS_TRANINDIC)) == 0) {
		// if this is not a void of a discount/surcharge or a return of a discount/surcharge then we need
		// to determine whether the discount is an actual discount or a surcharge.
		// If this is a void of a discount/surcharge or a return of a discount/surcharge then
		// the sign of the amount is already set.
		switch(pItemDisc->uchMinorClass) {                  /* get override MDC option */
		case CLASS_UIITEMDISC1:                             
			{
				USHORT  usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID13_ADR);  /* address 31 */

				if (! CliParaMDCCheckField(usAddress, MDC_PARAM_BIT_D)) {  /* not a surcharge, is a discount */
					pItemDisc->lAmount *= -1L;                             /* reverse the sign to make this a discount */  
				}
			}
			break;

		case CLASS_UIREGDISC1:            
			if (! CliParaMDCCheck(MDC_RDISC13_ADR, ODD_MDC_BIT0)) {   /* ! surcharge */
				pItemDisc->lAmount *= -1L;                            /* reverse the sign */  
			}
			break;
	    
		case CLASS_UIREGDISC2:            
			if (! CliParaMDCCheck(MDC_RDISC23_ADR, ODD_MDC_BIT0)) {  /* ! surcharge */
				pItemDisc->lAmount *= -1L;                           /* reverse the sign */  
			}
			break;

		case CLASS_UIREGDISC3:                                       /* R3.1 */
			if (!CliParaMDCCheck(MDC_RDISC33_ADR, ODD_MDC_BIT0)) {
				pItemDisc->lAmount *= -1L;
			}
			break;
	    
		case CLASS_UIREGDISC4:                                       /* R3.1 */
			if (!CliParaMDCCheck(MDC_RDISC43_ADR, ODD_MDC_BIT0)) {
				pItemDisc->lAmount *= -1L;
			}
			break;
	    
		case CLASS_UIREGDISC5:                                       /* R3.1 */
			if (!CliParaMDCCheck(MDC_RDISC53_ADR, ODD_MDC_BIT0)) {
				pItemDisc->lAmount *= -1L;
			}
			break;
	    
		case CLASS_UIREGDISC6:                                       /* R3.1 */
			if (!CliParaMDCCheck(MDC_RDISC63_ADR, ODD_MDC_BIT0)) {
				pItemDisc->lAmount *= -1L;
			}
			break;
	    
		default:
			// charge tips and other types do not have the discount or surcharge decision to make.
			break;
		}
	}

    /*--- MANIPURATE SIGN FOR ITEM VOID ---*/
    if (pItemDisc->fbDiscModifier & (VOID_MODIFIER | RETURNS_TRANINDIC)) {
		// if this is either a void or a return then we reverse the sign.  However if it is
		// a void of a return then we do not reverse the sign.  If this should be a Preselect Return,
		// we will detect and do a final sign reversal below.
		if ((pItemDisc->fbDiscModifier & (VOID_MODIFIER | RETURNS_TRANINDIC)) != (VOID_MODIFIER | RETURNS_TRANINDIC)) {
			pItemDisc->lAmount *= -1L;                 /* reverse the sign */
		}
    }

    if ((pItemDisc->fbDiscModifier & VOID_MODIFIER) == 0) {
		/*-----  PRESELECT VOID or PRESELECT RETURN  -----
		 *     Check to see if we are doing a Preselect Transaction Void or a Preselect Transaction Return
		 */
		if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
			pItemDisc->lAmount *= -1L;
		}
	}

    switch(pItemDisc->uchMinorClass) {
    case CLASS_CHARGETIP:       /* save chrage tip amount */
    case CLASS_CHARGETIP2:      /* V3.3 */
    case CLASS_CHARGETIP3:
    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
		{
			ITEMCOMMONLOCAL    *pCommonLocalData = ItemCommonGetLocalPointer();

			if (pItemDisc->fbDiscModifier & VOID_MODIFIER)
				pCommonLocalData->lChargeTip += pItemDisc->lAmount;
			else
				pCommonLocalData->lChargeTip = pItemDisc->lAmount;
		}

	default:
		break;
    }

    return(ITM_SUCCESS);
}

/****** end of source ******/
