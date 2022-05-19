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
* Title       : Price Check module                          
* Category    : Item Module, NCR 2170 Hsopitality US Model Application         
* Program Name: SLPCHECK.C
* --------------------------------------------------------------------------
* Abstruct:  ItemSalesPriceCheck() : Display price of cursor item   
*            ItemSalesPriceChange() :
*            ItemSalesGetPrice()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver. Rev. :   Name      : Description
* Nov-8-99  :           : M.Ozawa     : initial                                   
* May-21-03 : 02.00.05  : C.Wunn      : US Customs SCER allow price entry befor Open Plu
* Aug-01-15 : 02.02.01  : R.Chambers  : add ASSRTLOGs for MDC provisioning and replace error code.
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
#include    <stdio.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "rfl.h"
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

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPriceCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: UIFREGSALES *pItemSales
*
*    Output: None
*
*    InOut: None
*
**Return: Return status returned from sub module 
*
** Description: This module displays the price of cursor item.

*===========================================================================
*/
SHORT   ItemSalesPriceCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    ITEMDISC      ItemDisc;
    USHORT        usScroll;
    SHORT         sReturnStatus;
    UCHAR         uchSavedRepeatCo;

    /* GP type price check key, 2172 */
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        return(LDT_CANCEL_ADR);
    }
    if (MldQueryMoveCursor(MLD_SCROLL_1) == FALSE) {
        /* if cursor is not moved, assume as GP Price Check */
        return(LDT_CANCEL_ADR);
    }
    
    if ( !(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)) {
        if (!(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
            /* allow price check of lcd after tray total */
            return(LDT_SEQERR_ADR);
        }
    }

    /* check current active scroll */
    usScroll = MldQueryCurScroll();
    if (usScroll != MLD_SCROLL_1) {
        return(LDT_SEQERR_ADR);
    }

    /* read cursor position item */
    if (MldGetCursorDisplay(usScroll, pItemSales, &ItemDisc, MLD_GET_SALES_DISC) != MLD_SUCCESS) {
        return (LDT_PROHBT_ADR);
    }

    if (pItemSales->uchMajorClass == CLASS_ITEMSALES) {
		REGDISPMSG  DisplayData = {0};

        switch(pItemSales->uchMinorClass) {
        case CLASS_DEPT:                    /* dept */
        case CLASS_DEPTITEMDISC:            /* dept */
        case CLASS_PLU:                     /* PLU */
        case CLASS_PLUITEMDISC:             /* PLU */
        case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
        case CLASS_OEPITEMDISC:             /* PLU w/ order entry prompt */
        case CLASS_SETMENU:                 /* setmenu */
        case CLASS_SETITEMDISC:             /* setmenu */
            DisplayData.uchMajorClass = pUifRegSales->uchMajorClass;
            DisplayData.uchMinorClass = pUifRegSales->uchMinorClass;
            sReturnStatus = ItemSalesSalesAmtCalc(pItemSales, &DisplayData.lAmount);
            if (sReturnStatus != ITM_SUCCESS) {
                /* calculate sales amount regardress sign */
                return(sReturnStatus);
            }

            _tcsncpy(DisplayData.aszMnemonic, pItemSales->aszMnemonic, STD_PLU_MNEMONIC_LEN);

            /*--- TURN OFF DESCRIPTOR CONTROL FLAG ---*/
            flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                       TAXMOD_CNTRL | FOODSTAMP_CTL | ADJ_CNTRL);  /* for 10N10D display */

            /*--- TURN OFF KEEP CONTROL FLAG ---*/
            flDispRegKeepControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                      TAXMOD_CNTRL | FOODSTAMP_CTL | ADJ_CNTRL);  /* for 10N10D display */

            uchSavedRepeatCo = uchDispRepeatCo;
            uchDispRepeatCo = 1;                    /* init repeat counter */

            /*--- CALL DISPLAY MODULE ---*/
            DispWrite(&DisplayData);    
    
            uchDispRepeatCo = uchSavedRepeatCo;
            return(SUCCESS);
            break;
        }
    }

    return(LDT_PROHBT_ADR);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPriceCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: UIFREGSALES *pItemSales
*
*    Output: None
*
*    InOut: None
*
**Return: Return status returned from sub module 
*
** Description: This module displays the price of cursor item.

*===========================================================================
*/
SHORT   ItemSalesPriceChange(ITEMSALES *pItemSales)
{
    DCURRENCY   lPrice = 0;
    SHORT       sReturn;

// Block this check to force open PLUs to prompt for a price US Customs cwunn 4/25/03
//    if ((pItemSales->fbModifier2 & PCHK_MODIFIER) == 0) {
        /* no price check */
//        return (ITM_SUCCESS);
//    }

	//no price check on preset PLU's, unless price check key pressed
    if ( ((pItemSales->ControlCode.uchItemType & PRESET_PLU)) && (!(pItemSales->fbModifier2 & PCHK_MODIFIER)) ){
		return(ITM_SUCCESS); 
	}
	
	sReturn = ItemSalesGetPrice(pItemSales, &lPrice);
    if (sReturn != ITM_SUCCESS) {
        /* cancel ppi, pm calculation */
        ItemSalesCalcECCom(pItemSales);
    }
    
    if (sReturn == ITM_PRICECHANGE) {
        if (pItemSales->uchPPICode) {
            /* set product as entered price */
            pItemSales->lProduct = lPrice;
        } else {
            pItemSales->lUnitPrice = lPrice;
            
            /* clear noun product for recalculation */
            if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) == 0) {
                pItemSales->lProduct = 0L;
            }
        }

        pItemSales->fsLabelStatus |= ITM_CONTROL_PCHG;

        return (ITM_PRICECHANGE);
    }

    return (sReturn);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesGetPrice(UIFDIADATA *pDiaData);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFDIADATA  *pDiaData
*
**Return: UIFREG_SUCCESS : Function Performed succesfully
*         UIFREG_CANCEL  : Function Canceled by user using cancel key
*
** Description: This function gets the new price at price check.
*
*===========================================================================
*/
SHORT   ItemSalesGetPrice(ITEMSALES *pItemSales, DCURRENCY *plPrice)
{
	UIFDIADATA      WorkUI = {0};
    SHORT           sReturn;

    uchDispRepeatCo = 1;                    /* init repeat counter for display */

	//US Customs 5/21/03 cwunn if user entered price before open PLU, do not prompt,
	//	simply accept the entered price
	if(!(pItemSales->lUnitPrice)  || (pItemSales->fbModifier2 & PCHK_MODIFIER) )
	{
		REGDISPMSG      DispMsg = {0};

		//User did not enter a price prior to entering Open PLU, or is peforming price check on a preset PLU
		/* ----- display "ENTER PRICE" ----- */
		DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
		DispMsg.uchMinorClass = CLASS_REGDISP_PCHECK;

		if (pItemSales->uchPM) {
			DispMsg.uchMinorClass = CLASS_REGDISP_PCHECK_PM;
			DispMsg.lAmount = pItemSales->lUnitPrice;
			DispMsg.lQTY = (LONG)(pItemSales->uchPM * 1000L);
		} else if (pItemSales->uchPPICode) {
			DispMsg.lAmount = pItemSales->lProduct;
			DispMsg.lQTY = pItemSales->lQTY;
		} else {
			DispMsg.lAmount = pItemSales->lUnitPrice;
			DispMsg.lQTY = 0L;        /* don't display QTY */
		}

		if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {
			if (DispMsg.lAmount > 0) {
				DispMsg.lAmount *= -1;
			}
		}
    
		// get lead thru message and insert the PLU mnemonic into the string area to print as well.
		RflGetLead (DispMsg.aszMnemonic, LDT_PP_ADR);
		_tcsncpy(DispMsg.aszStringData, pItemSales->aszMnemonic, STD_LEADTHRUMNEMO_LEN);

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
	}
	else 
	{
		DCURRENCY   lTempUnitPrice; //US Customs, for computing price length
		int		    length;			//US Customs, indicates price length

		//US Customs - added to retain Saratoga operation of on keyboard open PLU
		//Setup WorkUI's data as would have happened in UifDiaPrice() above
		WorkUI.lAmount = pItemSales->lUnitPrice;
		lTempUnitPrice = pItemSales->lUnitPrice;
		length = 0;
		while(lTempUnitPrice){
			length++;
			lTempUnitPrice /= 10;
		}
		WorkUI.uchTrack2Len = length;
	}

	*plPrice = WorkUI.lAmount;

    if (WorkUI.uchTrack2Len) {
		DEPTIF   DeptIf = {0};

        /* price entry */
        /*--- HALO CHECK ---*/
        /* get dept record */
        DeptIf.usDeptNo = pItemSales->usDeptNo;
        if ((sReturn = CliOpDeptIndRead(&DeptIf, 0)) != OP_PERFORM) {
            return(OpConvertError(sReturn));
        } else {
            /* velify unit price vs HALO amount */
			if (RflHALO(WorkUI.lAmount, *DeptIf.ParaDept.auchHalo) != RFL_SUCCESS) {
                return(LDT_KEYOVER_ADR);        /* return sequence error */
            }
        }

        if (pItemSales->uchPPICode) {
			DCURRENCY       lCmp = pItemSales->lProduct;
            if (lCmp < 0)
				lCmp *= -1; /* turn over the sign */

			if (WorkUI.lAmount != lCmp) {
                if (CliParaMDCCheck(MDC_PLU3_ADR, EVEN_MDC_BIT0)) {
					char  xBuff[128];
					sprintf (xBuff, "==PROVISIONING: ItemSalesGetPrice() PPI LDT_BLOCKEDBYMDC_ADR MDC 296 Bit D.");
					NHPOS_NONASSERT_NOTE ("==PROVISION", xBuff);
                    return (LDT_BLOCKEDBYMDC_ADR);
                }
                return (ITM_PRICECHANGE);
            } else {
                return(ITM_SUCCESS);
            }
        } else {
			DCURRENCY       lCmp = pItemSales->lUnitPrice;
            if (lCmp < 0)
				lCmp *= -1; /* turn over the sign */

			if (WorkUI.lAmount != lCmp) {
                if (CliParaMDCCheck(MDC_PLU3_ADR, EVEN_MDC_BIT0)) {
					char  xBuff[128];
					sprintf (xBuff, "==PROVISIONING: ItemSalesGetPrice() LDT_BLOCKEDBYMDC_ADR MDC 296 Bit D.");
					NHPOS_NONASSERT_NOTE ("==PROVISION", xBuff);
                    return (LDT_BLOCKEDBYMDC_ADR);
                }
                return (ITM_PRICECHANGE);
            } else {
                return(ITM_SUCCESS);
            }
        }
    } else {
        /* not price entry */
        if (CliParaMDCCheck(MDC_PLU3_ADR, EVEN_MDC_BIT1)) {
            /* compulsory new price entry */
			char  xBuff[128];
			sprintf (xBuff, "==PROVISIONING: ItemSalesGetPrice() LDT_BLOCKEDBYMDC_ADR MDC 296 Bit C.");
			NHPOS_NONASSERT_NOTE ("==PROVISION", xBuff);
            return (LDT_BLOCKEDBYMDC_ADR);
        }

        return(ITM_SUCCESS);
    }
}
/**** End of File ****/
