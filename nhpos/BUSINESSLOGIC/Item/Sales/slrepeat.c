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
* Title       : Item repeat module                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLREPEAT.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesRepeat() : Item repeat module main.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun- 1-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 5-92: 00.00.01  : K.Maeda   : Code inspection held on Jun. 3
* Dec- 4-92: 01.00.00  : K.Maeda   : Add scale feature for GCA.
* Apr-11-94: 00.00.04  : K.You     : Add slip validation feature.(mod. ItemSalesRepeat)
* Nov-09-95: 03.10.00  : M.Ozawa   : Add ItemSalesRepeatCheck() for Repeat Key
*
** NCR2171 **
* Aug-26-99: 01.00.00  : M.Teraki  : initial (for 2171)
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
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "rfl.h"
#include    "fsc.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "mld.h"
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesRepeat(UIFREGSALES *pUifRegSales)
*
*    Input: UIFREGSALES *pUifRegSales
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:  
*
** Description: Repeat sales of previous PLU.

*===========================================================================
*/
SHORT   ItemSalesRepeat(CONST UIFREGSALES *pUifRegSales)
{
    UCHAR  uchSavedRepeatCo;
    UCHAR  uchItemStatus;  /* for demo */
    SHORT  sReturnStatus;
    ITEMCOMMONLOCAL    * CONST pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
	ITEMSALES          ItemSales = {0};
    
    /*--- ERROR TAX MOD. OPERATION ---*/
    if (ItemModLocalPtr->fsTaxable != 0) {    /* tax mod. operation was done */
        return(LDT_SEQERR_ADR);                 /* return sequence error */
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {   /* Saratoga */
        return(LDT_SEQERR_ADR);
    }
    /*--- ERROR TARE KEY (AUTO SCALE) OPERATION ---*/
    if (ItemModLocalPtr->ScaleData.fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
        return(LDT_TARENONSCALABLE_ADR);             /* tare key for non scalable item */
    }
    /*--- CHECK COMPULSORY OPERATION ---*/
    if ((sReturnStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {  /* check print mod. condiment */
        return(sReturnStatus);              /* compulsory status          */
    }
    if (pUifRegSales->fbModifier2) {        /* price check status */
        return(LDT_SEQERR_ADR); 
    }

	if (pCommonLocalRcvBuff->ItemSales.uchMajorClass != CLASS_ITEMSALES) {
		// repeat is for ITEMSALES items only and not for other transaction items.
        return(LDT_SEQERR_ADR); 
    }
    
	// Repeat is not allowed for a Gift Card type of item or PLU.
	if((pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) ||
		(pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
			return(LDT_SEQERR_ADR);

    /*----- ERROR CHECK -----*/
    ItemSales = pCommonLocalRcvBuff->ItemSales;    // make a copy of the current buffered item which we are going to repeat.
	ItemSales.usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ
	ItemSales.usKdsLineNo = 0;                     //reset KDS line number so that a new one will be assigned by ItemSendKdsItemGetLineNumber()

    switch(pUifRegSales->uchMinorClass) {
    case CLASS_UIDEPTREPEAT:
        if (!(ItemSales.uchMinorClass == CLASS_DEPT)) {  /* previous item isn't dept */
            return(LDT_SEQERR_ADR);
        }
        if (pUifRegSales->uchFsc == FSC_DEPT) {
            ;   /* repeat by Dept No. key */
        } else if (pUifRegSales->usDeptNo == 0) {   /* keyed Dept entry */
			PARACTLTBLMENU     MenuRcvBuff;     /* Menu page data receive buffer */
			PARADEPTNOMENU     DeptNoRcvBuff;   /* Dept No. assigned a key saved buffer */

            /* check availability of current page */
            MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
            MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + 1);
            CliParaRead(&MenuRcvBuff);

            if (MenuRcvBuff.uchPageNumber == 1) {
                return(LDT_PROHBT_ADR);     /* error if prohibited to use */
            }

			if (uchDispCurrMenuPage >= STD_TOUCH_MENU_PAGE) {
				// menu page usage is only allowed with keyboard type systems and
				// not with touchscreen installations.
                return(LDT_PROHBT_ADR);     /* error if prohibited to use */
			}

            /* get Dept No. assigned to uchFSC */
            DeptNoRcvBuff.uchMajorClass = CLASS_PARADEPTNOMENU;
            DeptNoRcvBuff.uchMenuPageNumber = uchDispCurrMenuPage;
            DeptNoRcvBuff.uchMinorFSCData = pUifRegSales->uchFsc;    /* extended FSC */
            CliParaRead(&DeptNoRcvBuff);

            if (DeptNoRcvBuff.usDeptNumber == 0) {
                return(LDT_PROHBT_ADR);     /* error if not preset */
            }

            if (DeptNoRcvBuff.usDeptNumber != ItemSales.usDeptNo) {  /* dept no. unmatch */
                return(LDT_SEQERR_ADR);
            }
        } else if (pUifRegSales->usDeptNo != ItemSales.usDeptNo) {   /* dept no. unmatch */
            return(LDT_SEQERR_ADR);
        }
        break;
    
    case CLASS_UIPLUREPEAT:
        if (!(ItemSales.uchMinorClass == CLASS_PLU || ItemSales.uchMinorClass == CLASS_SETMENU)) {   /*  previous item isn't PLU or set menu  */
            return(LDT_SEQERR_ADR);
        }
        break;
    
    default:
        break;
    }
    
    if ((ItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) ||         /* repeat not allowed on minus PLUs */
		 (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {     /* previous item was scalable item so repeat not allowed */
        return(LDT_SEQERR_ADR);
    }
    
    if (labs(ItemSales.lQTY) != ITM_SL_QTY) {     /* QTY entered on previous item so repeat not allowed */
        return(LDT_SEQERR_ADR);
    }

    if (ItemSales.fbModifier & VOID_MODIFIER) {  /* previous item is voided item so repeat not allowed */
        return(LDT_SEQERR_ADR);
    }

    /* ---- read plu file to check price change by oep free item ---- */
    if ((sReturnStatus = ItemSalesRepeatChkPriceChange(&ItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- Calculate PPI Price -----*/
    if (ItemSales.uchPPICode) {
        sReturnStatus = ItemSalesCalcPPI(&ItemSales);
        if ((sReturnStatus != ITM_SUCCESS) && (sReturnStatus != ITM_CANCEL)) {
            return(sReturnStatus);
        }
        /*--- PRESELECT VOID ---*/
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            ItemSales.lProduct *= -1L;
        }
    } else if (ItemSales.uchPM) {  /* 2172 */
        sReturnStatus = ItemSalesCalcPM(&ItemSales);
        if ((sReturnStatus != ITM_SUCCESS) && (sReturnStatus != ITM_CANCEL)) {
            return(sReturnStatus);
        }
        /*--- PRESELECT VOID ---*/
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            ItemSales.lProduct *= -1L;
        }
    }
    
    /*----- CHECK IF SEAT NO IS MODIFIED -----*/
    if (ItemSales.uchSeatNo != TranCurQualPtr->uchSeat) {
        ItemSales.uchSeatNo = TranCurQualPtr->uchSeat;
        uchSavedRepeatCo = 0;
    }

    /*----- CHECK SIZE -----*/
    if (ItemCommonCheckSize(&ItemSales, pCommonLocalRcvBuff->usSalesBuffSize) < 0) {
                                            /* only ItemSales */
        return(LDT_TAKETL_ADR);
    }

    /*----- SUPERVISOR INTERVENTION -----*/
    if ((sReturnStatus = ItemSalesSpvInt(&ItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }
  
    uchSavedRepeatCo = uchDispRepeatCo;    /* save current repeat counter */

    if ((sReturnStatus = ItemPreviousItem(&ItemSales, sizeof(ITEMSALES))) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /* send to enhanced kds, 2172 */
    sReturnStatus = ItemSendKds(&ItemSales, 0);
    if (sReturnStatus != ITM_SUCCESS) {
        
        return sReturnStatus;
    }
    
    /*----- Display Sales to LCD,   R3.0 -----*/
    MldScrollWrite(&ItemSales, MLD_NEW_ITEMIZE);
    ItemCommonDispSubTotal(&ItemSales);

    /*----- VALIDATION PRINT FOR MINUS ITEM -----*/
    uchItemStatus = 0;                      /* clear work status */

    if ((ItemSales.uchMinorClass == CLASS_DEPTMODDISC) ||
        (ItemSales.uchMinorClass == CLASS_PLUMODDISC) ||
        (ItemSales.uchMinorClass == CLASS_SETMODDISC)) {

        uchItemStatus &= ~COMP_VAL;         /* not print val. for minus item */
    } else if (ItemSales.ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        uchItemStatus &= ~COMP_VAL;         /* not print val. for condiment */
    } else {
        /* 9/21/93 if (ItemSales.ControlCode.auchStatus[0] & PLU_MINUS) { */
            if (ItemSales.ControlCode.auchPluStatus[0] & PLU_PRT_VAL) {
                uchItemStatus = COMP_VAL;
            } else if ((ItemSales.fbModifier & VOID_MODIFIER) && (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1))) { 
                uchItemStatus = COMP_VAL;   /* comp. validation for void */
            }
        /* 9/21/93 } */
    }

    if (uchItemStatus == COMP_VAL) {        /* req. validation */
		SHORT  sBuffer = ItemSales.fsPrintStatus;        /* store print status */

        ItemSales.fsPrintStatus = PRT_VALIDATION;
        TrnThrough(&ItemSales);                   /* print validation (VOID function) */
    
        ItemSales.fsPrintStatus = (sBuffer & ~PRT_VALIDATION);   /* restore print status */
    }

    /*----- MAKE I/F WITH DISPLAY MODULE -----*/
	{
		REGDISPMSG         DisplayData = {0};

		sReturnStatus = ItemSalesSalesAmtCalc(&ItemSales, &DisplayData.lAmount);
		if (sReturnStatus != ITM_SUCCESS) {
			return(sReturnStatus);
		}

		uchDispRepeatCo = (UCHAR) (uchSavedRepeatCo += 1);         /* increment repeat counter */
		ItemSalesDisplay(pUifRegSales, &ItemSales, &DisplayData);
	}

    /*----- CANCEL TAX EXEMPT KEY -----*/
    ItemCommonCancelExempt();

    /* ----- send link plu to kds, 2172 ----- */
    ItemSalesLinkPLU(&ItemSales, 3);
        
    return(ITM_SUCCESS);

}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCursorRepeat(ITEMDISC *pData);
*
*    Input: ITEMDISC *pData
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  Cursor Repeat of Sales.                   R3.1
*===========================================================================
*/
SHORT   ItemSalesCursorRepeat(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT   sStatus;
    UCHAR  uchItemStatus;
    SHORT  sCompSize;

    if ((sStatus = ItemCommonCheckComp()) != ITM_SUCCESS ) {
        return (sStatus);
    }

    if ( !(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
        if (!(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
            /* allow cursor repeat after tray total */
            return(LDT_SEQERR_ADR);
        }
    }

    if (pUifRegSales->fbModifier & VOID_MODIFIER) {
        return(LDT_SEQERR_ADR);
    }

    /*--- ERROR TAX MOD. OPERATION ---*/
    if (ItemModLocalPtr->fsTaxable != 0) {    /* tax mod. operation was done */
        return(LDT_SEQERR_ADR);                 /* return sequence error */
    }
    /*--- ERROR TARE KEY (AUTO SCALE) OPERATION ---*/
    if (ItemModLocalPtr->ScaleData.fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
        return(LDT_TARENONSCALABLE_ADR);             /* tare key for non scalable item */
    }

    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS || pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) { 
        return(LDT_PROHBT_ADR);
    }
    
    if (pItemSales->fbModifier & VOID_MODIFIER) {  /* voided item */
        return(LDT_PROHBT_ADR);
    }

    /* ---- read plu file to check price change by oep free item ---- */
    if ((sStatus = ItemSalesRepeatChkPriceChange(pItemSales)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*----- modify QTY ----*/
    if (pUifRegSales->lQTY) {
        if (pUifRegSales->lQTY % ITM_SL_QTY) {
            return(LDT_KEYOVER_ADR);
        }
        pItemSales->lQTY = pUifRegSales->lQTY;
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            pItemSales->lQTY *= -1L;
        }
    }

    if ((pUifRegSales->lQTY) || (pItemSales->uchPPICode) || (pItemSales->uchPM)) {
        if ((sStatus = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    /*--- PRESELECT VOID ---*/
    if (pItemSales->uchPPICode) {
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            pItemSales->lProduct *= -1L;
        }
    }

    /*----- CHANGE SEAT NO IF SEAT NO IS MODIFIED -----*/
    pItemSales->uchSeatNo = TranCurQualPtr->uchSeat;

    /*----- CHECK SIZE -----*/
    sCompSize = ItemCommonCheckSize(pItemSales, (USHORT)(ItemCommonLocalPtr->usSalesBuffSize +  ItemCommonLocalPtr->usDiscBuffSize));
    if (sCompSize < 0) {
        return(LDT_TAKETL_ADR);
    }

    /*----- SUPERVISOR INTERVENTION -----*/
    if ((sStatus = ItemSalesSpvInt(pItemSales)) != ITM_SUCCESS) {
         return(sStatus);
    }

	pItemSales->usUniqueID = ItemSalesGetUniqueId(); // unique id for condiment editing JHHJ
	pItemSales->usKdsLineNo = 0;                     // reset KDS line number so that a new one will be assigned by ItemSendKdsItemGetLineNumber()

    if ((sStatus = ItemPreviousItem(pItemSales, sCompSize)) != ITM_SUCCESS) {
        return(sStatus);
    }

    pUifRegSales->uchMajorClass = CLASS_UIFREGSALES;
    pUifRegSales->uchMinorClass = CLASS_UIDEPT;

	{
		REGDISPMSG  DisplayData = {0};

		sStatus = ItemSalesSalesAmtCalc(pItemSales, &DisplayData.lAmount);
		if (sStatus != ITM_SUCCESS) {   /* calculate sales amount regardlress sign */
			return(sStatus);
		}

		if ( !(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE) && (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY) ) {
			/* print out header at tray transaction */
			ItemCurPrintStatus();                               /* set print status of current qualifier */
			ItemPromotion(PRT_RECEIPT, TYPE_STORAGE);   
			ItemHeader(TYPE_STORAGE);                           /* send header print data */
																/* send transaction open data */
			ItemSalesLocal.fbSalesStatus |= SALES_ITEMIZE; 
		}

		/* prohibit condiment/print modifier entry, 10/12/98 */
		uchDispRepeatCo = 0;                    /* init repeat counter */
		/* uchDispRepeatCo = 1;                    / init repeat counter */

		ItemSalesDisplay(pUifRegSales, pItemSales, &DisplayData);
	}

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(pItemSales, 0);
    if (sStatus != ITM_SUCCESS) {
        return sStatus;
    }
    
    MldScrollWrite(pItemSales, MLD_NEW_ITEMIZE);
    ItemCommonDispSubTotal(pItemSales);

    /*----- VALIDATION PRINT FOR MINUS ITEM -----*/
    uchItemStatus = 0;                      /* clear work status */

    if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_OEPMODDISC) ||     /* R3.0 */
        (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

        uchItemStatus &= ~COMP_VAL;         /* not print val. for minus item */
    } else if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        uchItemStatus &= ~COMP_VAL;         /* not print val. for condiment */
    } else {
        /* 9/21/93 if (pItemSales->ControlCode.auchStatus[0] & PLU_MINUS) { */
            if (pItemSales->ControlCode.auchPluStatus[0] & PLU_PRT_VAL) {
                uchItemStatus = COMP_VAL;
            } else if ((pItemSales->fbModifier & VOID_MODIFIER) && (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1))) { 
                if (!(pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS)) {  /* comp. validation for void */
                    uchItemStatus = COMP_VAL;
                }
            }
        /* 9/21/93 } */
    }

    if (uchItemStatus == COMP_VAL) {        /* req. validation */
		USHORT  usBuffer;

        usBuffer = pItemSales->fsPrintStatus;       /* store print status */
        pItemSales->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItemSales);                    /* print validation (VOID function) */
    
        pItemSales->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);    /* restore print status */
    }

    /* ----- send link plu to kds, 2172 ----- */
    ItemSalesLinkPLU(pItemSales, 3);
        
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesRepeatCheck(UIFREGSALES *pUifRegSales)
*
*    InOut: UIFREGSALES *pUifRegSales
*
**Return:  CLASS_UIDEPTREPEAT   dept sales repeat
*          CLASS_UIPLUREPEAT    plu sales repeat
*          CLASS_UICOMCOUPONREPEAT coupn repeat
*          -1                   not repeatable
*
** Description: Check repeat item by repeat key operation

*===========================================================================
*/
SHORT   ItemSalesRepeatCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    /* keep compatibility to GP, saratoga */
    if (MldQueryMoveCursor(MLD_SCROLL_1) == TRUE) {

    /* if ((MldQueryMoveCursor(MLD_SCROLL_1) == TRUE) ||
        (pUifRegSales->lQTY)) { */
        return(ItemSalesCursorRepeatChk(pItemSales));
    } else {
        return(ItemSalesPrevRepeatChk(pUifRegSales, pItemSales));
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesRepeatCheck(UIFREGSALES *pUifRegSales)
*
*    InOut: UIFREGSALES *pUifRegSales
*
**Return:  CLASS_DEPT   dept sales repeat
*          CLASS_PLU    plu sales repeat
*          -1                   not repeatable
*
** Description: Check repeat item by repeat key operation

*===========================================================================
*/
SHORT   ItemSalesCursorRepeatChk(ITEMSALES *pItemSales)
{
    ITEMDISC    ItemDisc;

    /* check current active scroll */
    if (MldQueryCurScroll() != MLD_SCROLL_1) {
        return(-1);
    }

    /* read cursor position item */
    if (MldGetCursorDisplay(MLD_SCROLL_1, pItemSales, &ItemDisc, MLD_GET_SALES_DISC) != MLD_SUCCESS) {
        return (-1);
    }

    if (pItemSales->uchMajorClass == CLASS_ITEMSALES) {
        switch(pItemSales->uchMinorClass) {
        case CLASS_DEPT:                    /* dept */
        case CLASS_PLU:                     /* PLU */
        case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
        case CLASS_SETMENU:                 /* setmenu */
            return(pItemSales->uchMinorClass);
            break;
        }
    }

    return(-1);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesRepeatCheck(UIFREGSALES *pUifRegSales)
*
*    InOut: UIFREGSALES *pUifRegSales
*
**Return:  CLASS_UIDEPTREPEAT   dept sales repeat
*          CLASS_UIPLUREPEAT    plu sales repeat
*          CLASS_UICOMCOUPONREPEAT coupn repeat
*          -1                   not repeatable
*
** Description: Check repeat item by repeat key operation

*===========================================================================
*/
SHORT   ItemSalesPrevRepeatChk(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT              sReturn;
    ITEMCOMMONLOCAL    * CONST pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    if ( !(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)) {
        return(LDT_SEQERR_ADR);
    }

    if (pUifRegSales->lQTY) {
        return(-1);
    }

    if (PifSysConfig()->uchOperType == DISP_LCD) {
        /*  allow only qty repeat by LDC, to keep for GP repeat compatibility */
        if (labs(pCommonLocalRcvBuff->ItemSales.lQTY) != ITM_SL_QTY) {     /* QTY entered */
            return(ItemSalesCursorRepeatChk(pItemSales));
        }
    }

    switch (pCommonLocalRcvBuff->ItemSales.uchMajorClass) {
    case CLASS_ITEMSALES:                   /* sales repeat */
        switch (pCommonLocalRcvBuff->ItemSales.uchMinorClass) {
        case CLASS_DEPT:                    /* dept */
            pUifRegSales->uchMinorClass = CLASS_UIDEPTREPEAT; /* set dept repeat minor class */
            pUifRegSales->usDeptNo = pCommonLocalRcvBuff->ItemSales.usDeptNo;
            sReturn = CLASS_UIDEPTREPEAT;
            break;

        case CLASS_PLU:                     /* PLU */
        case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
        case CLASS_SETMENU:                 /* setmenu */
            pUifRegSales->uchMinorClass = CLASS_UIPLUREPEAT; /* set plu repeat minor class */
            sReturn = CLASS_UIPLUREPEAT;
            break;

        default:                            /* other class */
            sReturn = -1;
        }
        break;

    case CLASS_ITEMCOUPON:                  /* coupon repeat */
        switch (pCommonLocalRcvBuff->ItemSales.uchMinorClass) {
        case CLASS_COMCOUPON:                    /* combination coupon */
            sReturn = CLASS_UICOMCOUPONREPEAT;
            break;
            
        default:
            sReturn = -1;
        }
        break;

    default:                                /* other class */
        sReturn = -1;
    }

    return(sReturn);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesRepeatChkPriceChange(ITEMSALES *pItemSales)
*
*    Input  : *pItemSales
*
*    Output : nothing
*
*    InOut  : nothing
*
**Return:  ITM_SUCCESS      price is not changed
*          LDT_PROHBT_ADR   price is changed
*          
*
** Description: Check repeat plu unit price if price change is done

*===========================================================================
*/
SHORT   ItemSalesRepeatChkPriceChange(CONST ITEMSALES *pItemSales)
{
    if (RflIsSpecialPlu(pItemSales->auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0) {  /* plu sales */
		LONG    lOrgUnitPrice;
		PLUIF   PluIf;        /* PLU record receive buffer */

        _tcsncpy(PluIf.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
        PluIf.uchPluAdj = (UCHAR)(pItemSales->uchAdjective ? pItemSales->uchAdjective : 1);
        while(PluIf.uchPluAdj > 5) {
            PluIf.uchPluAdj -= 5;
        }
        if (CliOpPluIndRead(&PluIf, 0) != OP_PERFORM) {
            return(LDT_PROHBT_ADR);
        } 
        RflConv3bto4b(&lOrgUnitPrice, PluIf.ParaPlu.auchPrice);
        if (pItemSales->lUnitPrice != lOrgUnitPrice) {  /* unit price is changed */
            return(LDT_PROHBT_ADR);
        }
    }

    return (ITM_SUCCESS);
}

/****** end of file ******/
