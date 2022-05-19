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
* Title       : Condiment PLU module                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLCOND.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemSalesCondiment()   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : Initial issue                                   
* May-29-92: 00.00.01  : K.Maeda   : Imprement PLU functions for Jun. 6th link 
*                                    version.                                   
* Jun- 5-92: 00.00.01  : K.Maeda   : Comments of 1st code inspection Jun 3rd  
* Jun- 6-92: 00.00.01  : K.Maeda   : Comments of 2nd code inspection Jun 6th  
* Jun- 9-92: 00.00.01  : K.Maeda   : Comments of 3rd code inspection Jun 9th  
* Feb-27-95: 03.00.00  : hkato     : R3.0
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
#include    <math.h>

#include    "ecr.h"
#include    "rfl.h"
#include    "pif.h"
#include    "appllog.h"
#include    "log.h"
#include    "uie.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "itmlocal.h"
#include    "mld.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCondiment(UIFREGSALES *pUifRegSales,
*                                      ITEMSALES *pItemSales)
*    Input: UIFREGSALES *pUifRegSales
*   Output: None
*    InOut: ITEMSALES *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense 
*                          error
*         LDT_PROHBT_ADR : Function prohibited by parameter option.
*
** Description: Condiment PLU processing . 
*===========================================================================
*/

SHORT   ItemSalesCondiment(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
    SHORT              sReturnStatus;
	ITEMCOMMONLOCAL    CommonLocalEdit = {0};
    CHAR               chWork;

    /*----- ERROR AFTER REPEAT OPERATION -----*/
    if (uchDispRepeatCo != 1) { 
    /* if (uchDispRepeatCo > 1) {   10/12/98 */
        return(LDT_SEQERR_ADR); /* error if noun is repeated */
    }
    /*----- CHECK 0 AMOUNT ENTRY -----*/
    if (ItemSalesLocal.fbSalesStatus & SALES_ZERO_AMT) { /* 0 amount entry for non open PLU */
        return(LDT_SEQERR_ADR);
    }
    /*----- CHECK ADJ. VALIATION ENTRY -----*/
    if (pUifRegSales->uchAdjective != 0) {    /* adjective valiation wasn't entered */
        return(LDT_SEQERR_ADR);
    }
    /*----- ERROR VOID OPERATION FOR CONDIMENT -----*/
    if (pUifRegSales->fbModifier & VOID_MODIFIER) {    /* voided */
        return(LDT_SEQERR_ADR);
    }
    /*----- ERROR MODIFIED DISC. OPERATION FOR CONDIMENT -----*/
    if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) {    
        return(LDT_SEQERR_ADR);
    }
    /*----- ERROR NUMBER ENTRY BEFORE CONDIMENT -----*/
    if (pUifRegSales->aszNumber[0] != '\0') {    /* number entered */
        return(LDT_SEQERR_ADR);
    }
    /*----- ERROR TAX MODIFY OPERATION FOR CONDIMENT -----*/
    if (ItemModLocalPtr->fsTaxable != 0) {    /* tax modified */
        return(LDT_SEQERR_ADR);
    }
    /*----- CHECK PREVIOUS ITEM -----*/
	if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)) //SR 192
	{
		USHORT  usCondNumber;
		USHORT  usScroll = MldQueryCurScroll();			//determine which screen we are using

		if(MldGetCursorDisplay(usScroll, &CommonLocalEdit.ItemSales, &usCondNumber, MLD_GET_SALES_ONLY) >= 0)
		{
			// if we allow condiment editing and if a condiment is selected then use that instead.
			pCommonLocalRcvBuff = &CommonLocalEdit;
		}
	}

    if (pCommonLocalRcvBuff->ItemSales.uchMajorClass != CLASS_ITEMSALES) {
        return(LDT_SEQERR_ADR);
    }
    if (!(pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLU ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMENU || /* R3.0 */
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC)) {
        return(LDT_PROHBT_ADR);
    }
    if (!(pCommonLocalRcvBuff->ItemSales.ControlCode.uchItemType & PRESET_PLU)) {
        return(LDT_PROHBT_ADR);
    }
    if ((pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) ||
        (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) ||
        (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[2] & PLU_HASH)) {
        return(LDT_PROHBT_ADR);
    }
    if(pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC) {
        pUifRegSales->uchMinorClass = CLASS_UIMODDISC;          /* inherit noun's mod. disc. */
    }    

    chWork = (CHAR)(pCommonLocalRcvBuff->ItemSales.uchPrintModNo
                 + pCommonLocalRcvBuff->ItemSales.uchChildNo      /* R3.0 */
                 + pCommonLocalRcvBuff->ItemSales.uchCondNo);

    if (chWork >= STD_MAX_COND_NUM) {
		// check if there is an empty slot in the item sales data for print modifier and
		// condiments for this print modifier.
        return(LDT_KEYOVER_ADR);
    }
    
    /*----- CHECK IF SEAT NO IS MODIFIED R3.1 -----*/
    if (pCommonLocalRcvBuff->ItemSales.uchSeatNo != TranCurQualPtr->uchSeat) {
        return(LDT_SEQERR_ADR);
    }

    /* error if type of condiment isn't identical with noun */     
    if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) ^   /* not identical */
        (pCommonLocalRcvBuff->ItemSales.ControlCode.uchItemType & PLU_TYPE_MASK_REG)) {

        if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
            return(LDT_PROHBT_ADR);
        }
    }
        
    /*----- ONE ADJECTIVE CONDIMENT PROCESS -----*/
    if (pItemSales->ControlCode.uchItemType & ADJ_PLU) {
		UCHAR  auchAdjDataBuff[2] = {0};
        
        /*----- CHECK ADJ. ASSUMED VALIATION OF NOUN -----*/
        /* inherit adjective of noun */
        if (pItemSales->uchAdjective != pCommonLocalRcvBuff->ItemSales.uchAdjective) {
			PLUIF   PLURecRcvBuffer = {0};        /* PLU record receive buffer */

	        pItemSales->uchAdjective = pCommonLocalRcvBuff->ItemSales.uchAdjective;
    	    if ((sReturnStatus = ItemSalesBreakDownAdj(pItemSales, auchAdjDataBuff)) != ITM_SUCCESS) {
        	    return (sReturnStatus);
	        }

    	    /*----- get adjective price -----*/
        	/* prepair PLU record receive buffer */
    	    _tcsncpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
	        PLURecRcvBuffer.uchPluAdj = auchAdjDataBuff[1];  /* adjective valiation */

    	    if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
        	    return(OpConvertError(sReturnStatus));
	        } 

    	    /* set adj. price */
   	    	RflConv3bto4b(&pItemSales->lUnitPrice, PLURecRcvBuffer.ParaPlu.auchPrice);
   	    } else {
			/* from OEP */
    	    if ((sReturnStatus = ItemSalesBreakDownAdj(pItemSales, auchAdjDataBuff)) != ITM_SUCCESS) {
        	    return (sReturnStatus);
	        }
		}
    } else {
        /*----- get group no of condiment -----*/
#if 0
        /* prepair PLU record receive buffer */
        memset(&PLURecRcvBuffer, '\0', sizeof(PLUIF));
        _tcsncpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
        //memcpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
        /* PLURecRcvBuffer.usPluNo = pItemSales->usPLUNo; */
        PLURecRcvBuffer.uchPluAdj = 1;

        if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
            return(OpConvertError(sReturnStatus));
        }
#endif
    }

    /* ---- verify group no of condiment R3.1 ---- */
	{
		SHORT   i;

		for (i = 0; i < SALES_NUM_GROUP; i++) {
			if (ItemSalesLocal.auchOepGroupNo[i] == 0) break;

			if (pItemSales->uchGroupNumber == ItemSalesLocal.auchOepGroupNo[i]) {
				break;
			}
		}
		if (i) {
			if (ItemSalesLocal.auchOepGroupNo[i] == 0) { /* not terget plu, retry */
				return(LDT_PROHBT_ADR);
			}
		}
	}

    /*----- INHERIT NOUN's PARAMETER -----*/
    /* inherit QTY */
    pItemSales->lQTY = labs(pCommonLocalRcvBuff->ItemSales.lQTY);         

	// SR 963 - Inheriting taxable status from main item is based on MDC setting. CSMALL
	/* inherit taxable status */
    if (! CliParaMDCCheckField (MDC_TAXINTL_ADR, MDC_PARAM_BIT_B))
	{
		UCHAR  fbNounTax = (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[1] & PLU_DISC_TAX_MASK);

	   pItemSales->ControlCode.auchPluStatus[1] &= ~PLU_DISC_TAX_MASK;    /* 0x07 */
	   pItemSales->ControlCode.auchPluStatus[1] |= fbNounTax;
	}
	// END SR 963

    if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_FS) { /* Saratoga */
        pItemSales->ControlCode.auchPluStatus[1] |= PLU_AFFECT_FS;
    } else {
        pItemSales->ControlCode.auchPluStatus[1] &= ~PLU_AFFECT_FS;
    }

	/* inherit discountable status */
	{
		UCHAR   fbNounDiscStat = (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[1] & (PLU_AFFECT_DISC1 | PLU_AFFECT_DISC2));

		pItemSales->ControlCode.auchPluStatus[1] &= ~(PLU_AFFECT_DISC1 | PLU_AFFECT_DISC2);
		pItemSales->ControlCode.auchPluStatus[1] |= fbNounDiscStat;
	}
    
    /*----- CHANGE MINOR CLASS -----*/
    if (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC  /* R3.0 */
        || pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC
        || pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_OEPMODDISC) {
        pItemSales->uchMinorClass = CLASS_PLUMODDISC;
    }

    return(ITM_SUCCESS);
}

/****** end of file ******/
