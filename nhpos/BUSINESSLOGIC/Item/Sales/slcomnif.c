/*
*===========================================================================
* Title       : I/F function with Item Common Module                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLCOMNIF.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesCommonIF()   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : initial
* Jun- 5-92: 00.00.01  : K.Maeda   : 2nd code inspection on Jun, 3rd.
* Jun- 6-92: 00.00.01  : K.Maeda   : 3rd code inspection on Jun, 6th.
* Jun- 9-92: 00.00.01  : K.Maeda   : 4th code inspection on Jun, 9th.
* Dec- 4-92: 01.00.00  : K.Maeda   : Chang display I/F for scale feature.
* Jun-29-93: 01.00.12  : K.You     : Add item consoli./not consoli. feature for US enhance.
*          :           :           : (mod. ItemSalesCommonIF, ItemSalesSalesAmtCalc)
* Sep-21-93: 02.00.00  : K.You     : Inp. FVT-24 comment (mod. ItemSalesCommonIF)
* Apr-11-94: 00.00.04  : K.You     : Add slip validation feature.(mod. ItemSalesCommonIF)
* Mar- 1-95: 03.00.00  : hkato     : R3.0
* Jun-09-98: 03.03.00  : M.Ozawa   : change transaction open
*
** NCR2171 **
* Aug-26-99 : 01.00.00  : M.Teraki    : initial (for 2171)
** GenPOS Rel 2.2
* Aug-01-15 : 02.02.01  : R.Chambers  : fix problem of #/Type without reference number causing duplicate item
* Jan-21-15 : 02.02.01  : R.Chambers  : fix Cursor Void scalable items not working Rel 2.2.1.159
* Sep-21-17 : 02.02.02  : R.Chambers  : replace 1000L with ITM_SL_QTY, localize variables.
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
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "rfl.h"
#include    "uie.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "display.h"
#include    "mld.h"
#include    "itmlocal.h"
/*
*===========================================================================
**Synopsis: SHORT   ItemCommonSetupTransEnviron (VOID)
*
*    Input: None
*   Output: None
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  This is a common function that determines if the transaction environment
*                has been initialized and if not, will then do so.
*                It will get the next guest check number and initialize the transaction
*                data area so that the transaction will be processed correctly.
*===========================================================================
*/
SHORT  ItemCommonSetupTransEnviron (VOID)
{
    if((ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE) == 0) {
		/* "itemize start" indicator is not set so we need to initialize the environment */
		SHORT   sReturnStatus;
		USHORT  usGuestNo = 0;

        sReturnStatus = ItemCommonGetSRAutoNo(&usGuestNo);
        if (sReturnStatus != ITM_SUCCESS) {
            return(sReturnStatus);
        }

        sReturnStatus = ItemCommonTransOpen(usGuestNo);
        if (sReturnStatus != ITM_SUCCESS) {
            return(sReturnStatus);
        }
        
#if 0
		// do not initialize the PPI calculation buffer at this point. doing so can cause
		// the PPI calculation errors if the first item entered is a PPI item.
		// the initialize PPI calculation buffer is called by the main item entry function ItemSales()
		// when the first item of a transaction is entered.
		// we have put an additional call to ItemSalesCalcInitCom() in function ItemSalesCalculation()
		// to ensure the PPI calculation buffer is initialized before calling ItemSalesCalcPPI() the first time.
		//    Richard Chambers, Jun-02-2016 for GenPOS Rel 2.2.1
		//
		/* initialize ppi calculation buffer R3.1 */
		ItemSalesCalcInitCom();                             /* ppi initialize r3.1 2172 */
#endif

        /* set "itemize start" indicator flag */
		ItemSalesLocal.fbSalesStatus |= SALES_ITEMIZE; 
        /* DISABLE MODE CHANGABLE STATUS */
        UieModeChange(UIE_DISABLE);
    }

	return ITM_SUCCESS;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCommonIF(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: 
*   Output: None
*    InOut: None
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  
*===========================================================================
*/
SHORT   ItemSalesCommonIF(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
	UCHAR  auchMDCSave[2] = {0};
    UCHAR  uchItemStatus;
    SHORT  sReturnStatus, sReturnPrevious;
    SHORT  sCompSize;
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff;
	ITEMCOMMONLOCAL    CommonLocal = {0};
    ITEMMODLOCAL       * CONST pModLocalRcvBuff = ItemModGetLocalPtr();

    if ( ! ( ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE )             /* GCA */
        && CliParaMDCCheck( MDC_PSN_ADR, EVEN_MDC_BIT2 )                /* GCA */
        && pModLocalRcvBuff->usCurrentPerson == 0 ) {                     /* GCA */
        return( LDT_NOOFPRSN_ADR );                                     /* GCA */
    }

	if (pItemSales->uchMinorClass == 0) {
		// If this is processing a #/Type key press (FSC_NUM_TYPE) for a reference number to
		// attach to either the current item or the next item to be entered, check to
		// see if a reference number was entered.  if not then prompt for one.
		if (pItemSales->aszNumber[0][0] == 0) {
			sReturnStatus = ItemLineLeadthruPromptEntry (LDT_REFERENCENUM_ADR, pItemSales->aszNumber[0], NUM_NUMBER);
			if (sReturnStatus != ITM_SUCCESS) {
				// if cancel or some other error then just return as if never happened.
				return ITM_SUCCESS;
			}
		}
	}

	pItemSales->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;  // set the sales item hourly total block offset for purposes of totals.

    /*--- CHECK SALES PRICE ---*/
    if (((pItemSales->uchMinorClass != CLASS_DEPTMODDISC) &&
        (pItemSales->uchMinorClass != CLASS_PLUMODDISC) &&
        (pItemSales->uchMinorClass != CLASS_SETMODDISC)))
	{ 
		DCURRENCY    lAmount = 0;

        if ((sReturnStatus = ItemSalesSalesAmtCalc(pItemSales, &lAmount)) != ITM_SUCCESS) {   /* calculate current product */
            return(sReturnStatus);
        }
                                            
        if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
			DCURRENCY    lProductSave;
			DUNITPRICE   lUnitPriceSave;
			LONG         lQTYSave;

			if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)) //SR 192
			{
				USHORT	 usScroll, usCondNumber;

				pCommonLocalRcvBuff = &CommonLocal;
				usScroll = MldQueryCurScroll();			//determine which screen we are using
				if(MldGetCursorDisplay(usScroll, &pCommonLocalRcvBuff->ItemSales, &usCondNumber, MLD_GET_SALES_ONLY) < 0)
				{
					pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
				}
			}else
			{
				pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
			}

            /* save original price r3.1 */
            lQTYSave = pCommonLocalRcvBuff->ItemSales.lQTY;
            lUnitPriceSave = pCommonLocalRcvBuff->ItemSales.lUnitPrice;
            lProductSave = pCommonLocalRcvBuff->ItemSales.lProduct;

                                            /* get noun */
            if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) {
                pCommonLocalRcvBuff->ItemSales.lUnitPrice *= -1L;
                pCommonLocalRcvBuff->ItemSales.lProduct *= -1L;
            }                                       

            if (pCommonLocalRcvBuff->ItemSales.fbModifier & VOID_MODIFIER) {
                pCommonLocalRcvBuff->ItemSales.lQTY *= -1L;
                if (pCommonLocalRcvBuff->ItemSales.uchPPICode == 0) {  /* not turn over sign at ppi */
                    pCommonLocalRcvBuff->ItemSales.lProduct *= -1L;
                }
            }                               /* manipurate sign by item void */
        
            /*--- PRESELECT VOID ---*/
            if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                pCommonLocalRcvBuff->ItemSales.lQTY *= -1L;
                pCommonLocalRcvBuff->ItemSales.lProduct *= -1L;
            }

            sReturnStatus = ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &lAmount);

            /* recover original price r3.1 */
            pCommonLocalRcvBuff->ItemSales.lQTY = lQTYSave;
            pCommonLocalRcvBuff->ItemSales.lUnitPrice = lUnitPriceSave;
            pCommonLocalRcvBuff->ItemSales.lProduct = lProductSave;

            if (sReturnStatus != ITM_SUCCESS) {
                /* cal noun + condiment */
                return(sReturnStatus);
            }
        }
        
		{
			// if this is a Preselect Return of some kind then do not do a HALO check.  If this
			// is a standard Void then we do a HALO check.
			USHORT  usReturnMask = (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3);

			if ((pItemSales->fbModifier & (VOID_MODIFIER | usReturnMask)) == VOID_MODIFIER) {
				if (pItemSales->uchMinorClass == CLASS_DEPT) {
					sReturnStatus = ItemSalesComnIFVoidHALO(lAmount);
				} else {                        /* PLU */
					if (pItemSales->uchMinorClass == CLASS_PLU
						|| pItemSales->uchMinorClass == CLASS_OEPPLU     /* R3.0 */
						|| pItemSales->uchMinorClass == CLASS_SETMENU) {
	                    
						if (!(pItemSales->ControlCode.uchItemType & PRESET_PLU)) {   /* only open PLU */
							sReturnStatus = ItemSalesComnIFVoidHALO(lAmount);
						}
					}
				}
				if (sReturnStatus != ITM_SUCCESS) {
					return(sReturnStatus);
				}
			}
		}
    }       

    /*--- SET SIGN ---*/
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) {
        pItemSales->lUnitPrice *= -1L;
        pItemSales->lProduct *= -1L;
    }                                       

    /*--- SET VOID SIGN ---*/
    if (pItemSales->fbModifier & VOID_MODIFIER) {
        pItemSales->lQTY *= -1L;
        if (pItemSales->uchPPICode == 0) {  /* not turn over sign at ppi */
            pItemSales->lProduct *= -1L;
        }
    }

    /*--- PRESELECT VOID ---*/
    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
        pItemSales->lQTY *= -1L;
        pItemSales->lProduct *= -1L;
    }

    /*--- DON'T CONSOLIDATE SCALABLE DEPT/ PLU ---*/
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) {    
//        pItemSales->fbStorageStatus = NOT_ITEM_CONS;  
    }

    /* check consolidatation mode or not,   Delete R3.0 */

    /*----- SET PRINT STATUS TO pItemSales -----*/
    pItemSales->fsPrintStatus |= (PRT_SLIP | PRT_JOURNAL);   
                                            /* set journal and slip print status */
    /*----- SET TICKET STATUS -----*/
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_SINGREC) {  /* 0x40, issue single or double ? */
        pItemSales->fsPrintStatus |= PRT_SINGLE_RECPT;                                     /* issue single */
        if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_DBLREC) {  /* 0x80 */
            pItemSales->fsPrintStatus |= PRT_DOUBLE_RECPT;
        }
    }

    /* set kitchen print status */
    pItemSales->fsPrintStatus |= (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & ITM_KTCHN_SEND_MASK);

    /* set special print status */
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_SPECIAL) {
        pItemSales->fsPrintStatus |= PRT_SPCL_PRINT;
    }

    /*----- SET STATUS ACCORDING TO MDC OPTION -----*/
    auchMDCSave[0] = CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3);

/*****  not turn off journal bit, to avoid item consolidation search error

    if ((auchMDCSave[0] & ODD_MDC_BIT3) && !((pItemSales->ControlCode.auchStatus[0] & PLU_MINUS) |
                                             (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) |
                                             (pItemSales->fbModifier & VOID_MODIFIER) |
                                             (pUifRegSales->uchMinorClass == CLASS_UIMODDISC))) {

        pItemSales->fsPrintStatus &= ~PRT_JOURNAL;  / turn off journal bit /
    }
*****/

    /*----- HOURLY TOTAL AFFECTION STATUS -----*/
    if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) &&  /* V3.3 */
        (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_HASH)) {
        if (auchMDCSave[0] & ODD_MDC_BIT1) {                                         /* affect to financial total */  
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] &= ~PLU_HASH;  /* reset hash status, assume as minus */
        }
    }
    pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_5] |= (ITM_SALES_HOURLY | ITM_HASH_FINANC);  /* affect normaly */
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) {
        if (auchMDCSave[0] & ODD_MDC_BIT0) {                                                    /* not affect hourly total */  
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_5] &= ~ITM_SALES_HOURLY;     /* turn off affect status */
		}
    }                                           
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_HASH) {
        if (!(auchMDCSave[0] & ODD_MDC_BIT1)) {                                               /* not affect to financial total */  
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_5] &= ~ITM_HASH_FINANC;     /* turn off affect status */
        }
        if (!(auchMDCSave[0] & ODD_MDC_BIT2)) {                                              /* not affect to hourly total */  
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_5] &= ~ITM_SALES_HOURLY;    /* turn off affect status */
        }
    }                                                
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) {     
        /*--- AFFECTION OPTION STATUS FOR SCALABLE ---*/
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                                /* address 29 1: accumulated by 0.01 LB/Kg */
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_5] |= ITM_AFFECT_DEV_BY_10;    /* affect */
        }
    }
    //we do this if statement to protect the 
	//information of the Order declaration 
	//type, we are using the item sales
	//structure as a holder because it has
	//to act like a plu, we store the order dec
	//number in the seat number location JHHJ
    /* ---- set seat no R3.1 ---- */
	if(pItemSales->uchMinorClass != CLASS_ITEMORDERDEC)
	{
		pItemSales->uchSeatNo = TranCurQualPtr->uchSeat;
	}

    /*----- CHECK STORAGE SIZE -----*/
    pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

	if (pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMSALES && pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_ITEMORDERDEC && pItemSales->uchMinorClass == 0) {
		SHORT  sTender;

		// check to see if there is a rule for the entered Order Declare and if so does the data entered meet the rule.
		sTender = ParaTendAccountRuleNumtypeOrderDec (pCommonLocalRcvBuff->ItemSales.uchAdjective, pItemSales->aszNumber[0]);
		if (sTender < 0) {
			return(LDT_KEYOVER_ADR);
		}
	}

    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
		if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)) //SR 192
		{
			USHORT	 usScroll, usCondNumber;

			pCommonLocalRcvBuff = &CommonLocal;
			usScroll = MldQueryCurScroll();			//determine which screen we are using
			if(MldGetCursorDisplay(usScroll, &pCommonLocalRcvBuff->ItemSales, &usCondNumber, MLD_GET_SALES_ONLY) < 0)
			{
				pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
			}
		}else
		{
			pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
		}
        sCompSize = ItemCommonCheckSize(&(pCommonLocalRcvBuff->ItemSales), ITM_CONDIMENT_SIZE);
                                            /* defined as 15 */
    } else {
        sCompSize = ItemCommonCheckSize(pItemSales, (USHORT)(pCommonLocalRcvBuff->usSalesBuffSize + pCommonLocalRcvBuff->usDiscBuffSize));
    }   
    if (sCompSize < 0) {                    /* buffer full condition */
        if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
            ItemSalesCancelPrev();          /* cancel previous item */
        }
        return(LDT_TAKETL_ADR);
    }
                                            
    /*----- SUPERVISOR INTERVENTION -----*/
    if (!(ItemSalesLocal.fbSalesStatus & SALES_DISPENSER_SALES) &&
        ((pItemSales->uchMinorClass != CLASS_DEPTMODDISC) &&
         (pItemSales->uchMinorClass != CLASS_PLUMODDISC) &&
         (pItemSales->uchMinorClass != CLASS_SETMODDISC))) {

        /* ----- check non WIC item,    Saratoga  ----- */
        if ((sReturnStatus = ItemSalesCheckWIC(pItemSales)) != UIF_SUCCESS) {
            return(sReturnStatus);
        }

        /* ----- check birthday entry,  2172  ----- */
        if ((sReturnStatus = ItemSalesCheckBirth(pItemSales)) != ITM_SUCCESS) {
           ItemSalesCalcECCom(pItemSales);     /* 2172 */
           return (sReturnStatus);
        }
        
        if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
            if ((sReturnStatus = ItemSalesSpvIntForCond(pItemSales)) != ITM_SUCCESS) {
                ItemSalesCalcECCom(pItemSales);     /* 2172 */
                return(sReturnStatus);
            }
        } else {
            if ((sReturnStatus = ItemSalesSpvInt(pItemSales)) != ITM_SUCCESS) {
                ItemSalesCalcECCom(pItemSales);     /* 2172 */
                return(sReturnStatus);
            }
        }
    }

    /*--- Set Coupon Quantity,   R3.0 ---*/
#if 1
	// coupon quantity is used in several places to determine the number of items that actually exist
	// with consolidated items in the transaction data file. functionality such as cursor void will
	// use coupon quantity to determine if an item can be voided for instance.
	if (!(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT)) {
		/* 5/16/96 && !(pItemSales->ControlCode.auchStatus[0] & PLU_MINUS) */
		if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE)) {
			pItemSales->sCouponQTY = 1;    // force PLU_SCALABLE items to have a coupon quantity of 1
		} else {
			pItemSales->sCouponQTY = (SHORT)(pItemSales->lQTY / ITM_SL_QTY);
		}
	}
#else
	// the following is older source that has been this way for quite a while.
	// however testing with Cursor Void with scalable items as well as items that
	// were rung up with Item Discount #2 (CLASS_DEPTMODDISC, CLASS_PLUMODDISC, etc.)
	// is showing Not In File when Cursor Void is done with those items in Rel 2.2.1.159.
	//     Richard Chambers, Jan-21-2016
    if (pItemSales->uchMinorClass == CLASS_DEPT
        || pItemSales->uchMinorClass == CLASS_DEPTITEMDISC
        || pItemSales->uchMinorClass == CLASS_PLU
        || pItemSales->uchMinorClass == CLASS_PLUITEMDISC
        || pItemSales->uchMinorClass == CLASS_SETMENU
        || pItemSales->uchMinorClass == CLASS_SETITEMDISC
        || pItemSales->uchMinorClass == CLASS_OEPPLU
        || pItemSales->uchMinorClass == CLASS_OEPITEMDISC) { 
                                            
        if (!(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT)
            /* 5/16/96 && !(pItemSales->ControlCode.auchStatus[0] & PLU_MINUS) */
            && !(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE)) {
            pItemSales->sCouponQTY = (SHORT)(pItemSales->lQTY / ITM_SL_QTY);
        }
    }
#endif

    /* PLU Buidling */
    if (pItemSales->fsLabelStatus & ITM_CONTROL_NOTINFILE) {
        if ((sReturnStatus = ItemSalesPLUCreatBuilding(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }

    /*----- SEND TO COMMON MODULE -----*/
	if (RflSpecialPluCheck(pItemSales->auchPLUNo) == 0) {
		sReturnPrevious = ItemPreviousItem(pItemSales, sCompSize);
	} else {
		sReturnPrevious = ItemPreviousItem(0, 0);
	}
    if ((sReturnPrevious != ITM_SUCCESS) &&	(sReturnPrevious != ITM_PREVIOUS))
	{
		NHPOS_ASSERT(sReturnPrevious == ITM_SUCCESS);
        return(sReturnStatus);
    }

    /*--- MODIFIED DISC PROCESS ---*/
    pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    if ((pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPTMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_OEPMODDISC) || /* R3.0 */
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC)) {

        sReturnStatus = ItemSalesModDisc(pCommonLocalRcvBuff, pItemSales);

        if (sReturnStatus != ITM_SUCCESS) {
            ItemSalesCancelPrev();          /* cancel previous item */
                                    
            return(sReturnStatus);
        }
    }

    /*--- CLEAR TAX MODIFIER DATA EXCEPT EXEMPT KEY ---*/
    pModLocalRcvBuff->fsTaxable = 0;
    pModLocalRcvBuff->fbModifierStatus &= ~MOD_FSMOD;         /* Saratoga */
    
    /*----- SET E.C DISABLE STATUS -----*/
    if ((pItemSales->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) == VOID_MODIFIER) {
        pCommonLocalRcvBuff->fbCommonStatus |= COMMON_VOID_EC;   /* turn on disable error correction */
    }
    
    /*----- MAKE I/F WITH DISPLAY MODULE -----*/
	{
		REGDISPMSG    DisplayData = {0};

		/* get display product */
		if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
			(pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
			(pItemSales->uchMinorClass == CLASS_OEPMODDISC) ||  /* R3.0 */
			(pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

			DisplayData.lAmount = pCommonLocalRcvBuff->ItemSales.lDiscountAmount;
		} else {
			if ((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT)
				|| (pItemSales->uchMinorClass == CLASS_OEPPLU)    /* R3.0 */
				|| (pItemSales->uchMinorClass == CLASS_OEPITEMDISC)) {

				sReturnStatus = ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &DisplayData.lAmount);
				if (sReturnStatus != ITM_SUCCESS) {
												/* calculate sales amount regardress sign */
					return(sReturnStatus);
				}
			} else {
				DisplayData.lAmount = pItemSales->lProduct;
			}
		}
		uchDispRepeatCo = 1;                    /* init repeat counter */

		/*----- MANIPURATE MENU SHIFT PAGE -----*/
		if ((pItemSales->uchMinorClass == CLASS_PLU) ||
			(pItemSales->uchMinorClass == CLASS_OEPPLU) ||      /* R3.0 */
			(pItemSales->uchMinorClass == CLASS_SETMENU)) {

			ItemSalesSetDefltPage();
		}

		/*----- CANCEL TAX EXEMPT KEY -----*/
		ItemCommonCancelExempt();

		ItemSalesDisplay(pUifRegSales, pItemSales, &DisplayData);
	}

    /*----- PROCESS FOR FIRST ITEM -----*/
    /*----- Get Order# in case of Transaction Open -----*/
	sReturnStatus = ItemCommonSetupTransEnviron ();
    if (sReturnStatus != ITM_SUCCESS) {
		ItemSalesCalcECCom(pItemSales);     /* 2172 */
        return(sReturnStatus);
    }

	if(sReturnPrevious != ITM_PREVIOUS)
	{
		/* send to enhanced kds, 2172 */
		// We will send the entire local buffer item to the KDS so that the KDS
		// will see the entire item structure with condiments.
		// If this new item is actually a condiment, then it is added to the local
		// buffer item or the main item in the code above in function ItemPreviousItem(0.
		sReturnStatus = ItemSendKds(&(pCommonLocalRcvBuff->ItemSales), 0);
		if (sReturnStatus != ITM_SUCCESS) {
			return sReturnStatus;
		}
    
		//if the status indicates PLU_CONDIMENT the MLD_UPDATE_ITEMIZE
		//For US Customs also MLD_UPDATE_ITEMIZE if MajorClass is ItemSales
		//and a number is entered, but not a plu number
		/*----- Sales to LCD,   R3.0 -----*/
		// allow for using #/Type with the Total key to insert a message into transaction data
		// allow for using #/Type with the Order Declare key to insert a message into transaction data
		if (pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMSALES && pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_ITEMORDERDEC && pItemSales->uchMinorClass == 0) {
			MldScrollWrite(&(pCommonLocalRcvBuff->ItemSales), MLD_ORDER_DECLARATION);
		} else {
			if (pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMTOTAL)
				return ITM_SUCCESS;
			if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT || 
				(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2) && pItemSales->uchMajorClass == CLASS_ITEMSALES &&
				 pItemSales->aszNumber[0][0] && !pItemSales->auchPLUNo[0])) {
				MldScrollWrite(&(pCommonLocalRcvBuff->ItemSales), MLD_UPDATE_ITEMIZE);
			} else {
				if (ItemCommonLocal.ItemSales.uchMinorClass != CLASS_ITEMORDERDEC) {
					MldScrollWrite(&(pCommonLocalRcvBuff->ItemSales), MLD_NEW_ITEMIZE);
				}
				else {
					MldScrollWrite(&(pCommonLocalRcvBuff->ItemSales), MLD_ORDER_DECLARATION);
				}
			}
		}
		ItemCommonDispSubTotal(&(pCommonLocalRcvBuff->ItemSales));
	}

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
            if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_PRT_VAL) {
                uchItemStatus = COMP_VAL;
            } else if ((pItemSales->fbModifier & VOID_MODIFIER) && (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1))) { 
                if (!(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)) {   /* comp. validation for void */
                    uchItemStatus = COMP_VAL;
                }
            } else if (pItemSales->fsLabelStatus & ITM_CONTROL_NOTINFILE) {
                if (CliParaMDCCheck(MDC_PLU4_ADR, ODD_MDC_BIT0)) {
                    uchItemStatus = COMP_VAL;
                }
            }
        /* 9/21/93 } */
    }

    if (uchItemStatus == COMP_VAL) {        /* req. validation */
		USHORT  usBuffer;

        usBuffer = pItemSales->fsPrintStatus;        /* store print status */
        pItemSales->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItemSales);                     /* print validation (VOID function) */
    
        pItemSales->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);  /* restore print status */
    }

    /*----- MANIPURATE COMPULSORY BIT -----*/
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
        ItemSalesLocal.fbSalesStatus &= ~SALES_COND_COMPLSRY;    // PLU is a condiment so clear compulsory condiment key status in case it was set.
    } else { /*----- Test PLU status and set compulsory key status bit  -----*/
		ItemSalesLocal.fbSalesStatus |= (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_COMP_MODKEY ) ? SALES_PM_COMPLSRY : 0;
		ItemSalesLocal.fbSalesStatus |= (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_COMP_CODKEY ) ? SALES_COND_COMPLSRY : 0;
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID    ItemSalesSetDefltPage( VOID )
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : VOID
*         
** Description: Set default menu page No to uchCurrentMenuPage.  
*                
*===========================================================================
*/

VOID    ItemSalesSetDefltPage( VOID )
{

    if (CliParaMDCCheck(MDC_MENU_ADR, ODD_MDC_BIT1)) {      
                                            /* each PLU releases menu shift key */
        uchDispCurrMenuPage = ItemSalesLocal.uchMenuShift;   
        ItemCommonShiftPage(uchDispCurrMenuPage);          /* R3.0 */
                                            /* set default page to current */       
        if (ItemSalesLocal.uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            ItemSalesLocal.uchAdjCurShift = ItemSalesLocal.uchAdjDefShift;   /* set default adjective, 2172 */
        }
    }
}


/*
*===========================================================================
**Synopsis: SHORT    ItemSalesSalesAmtCalc(ITEMSALES *pItemSales, DCURRENCY *plCalProduct)
*
*    Input: ITEMSALES *pItemSales 
*   Output: Nothing
*    InOut: lCalProduct
**Return: : VOID
*         
** Description: Calculate product amount for display.  
*===========================================================================
*/
SHORT    ItemSalesSalesAmtCalc(CONST ITEMSALES *pItemSales, DCURRENCY *plCalProduct)
{
    USHORT     i;
	USHORT     usMaxChild = ( pItemSales->uchChildNo + pItemSales->uchCondNo + pItemSales->uchPrintModNo );
	D13DIGITS  d13CalProduct = *plCalProduct;
	DCURRENCY  dPpiPrice = ItemSalesCalcCondimentPPIOnly (pItemSales);
	LONG       lQty = ItemSalesCalcDetermineBaseQuantity(pItemSales);

//    d13CalProduct += pItemSales->lProduct;
	d13CalProduct += ItemSalesCalcDetermineBasePrice (pItemSales, lQty);

	// The condiment area is also used to communicate a linked PLU in the first condiment
	// array element if there is a linked PLU (indicated by pItemSales->uchChildNo being non-zero).
    /* link plu, saratoga */
    if (pItemSales->usLinkNo) {
        i = 0;
    } else {
        i = pItemSales->uchChildNo;
    }
    for ( ; i < usMaxChild; i++) {     /* R3.0 */
		D13DIGITS  d13ProdWork = 0;

        if (RflIsSpecialPlu(pItemSales->Condiment[i].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) == 0) {
            continue;
        }

		if (pItemSales->Condiment[i].ControlCode.uchPPICode == 0) {
			// if the item is non-scalable then multiply times the item quantity.
			// however if the item is scalable then just use the condiment price as is.
			d13ProdWork = pItemSales->Condiment[i].lUnitPrice;
			if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
				d13ProdWork *= lQty;
			}
		}
                                            /* QTY x unit price of condiment */
        if (d13ProdWork > STD_MAX_TOTAL_AMT || d13ProdWork < - STD_MAX_TOTAL_AMT) {
            return(LDT_KEYOVER_ADR);
        } else {
            d13CalProduct += d13ProdWork;

            if (d13CalProduct > STD_MAX_TOTAL_AMT || d13CalProduct < - STD_MAX_TOTAL_AMT) {
                return(LDT_KEYOVER_ADR);
            }
        }
    }

    *plCalProduct = d13CalProduct + dPpiPrice;
    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID    ItemSalesDisplay()
*
*    Input: Nothing 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : VOID
*         
** Description: Display sales data.  
*                
*===========================================================================
*/

VOID    ItemSalesDisplay(CONST UIFREGSALES *pUifRegSales, CONST ITEMSALES *pItemSales, REGDISPMSG *pDisplayData)
{
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) {    
		/*--- DISPLAY DECIMAL DATA ---*/
        pDisplayData->uchMajorClass = CLASS_REGDISP_SALES;
    
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {      /* address 29, bit 0 */
            pDisplayData->uchMinorClass = CLASS_REGDISP_SCALE3DIGIT;
        } else {
            pDisplayData->uchMinorClass = CLASS_REGDISP_SCALE2DIGIT;
        }

        /*--- DISPLAY LB/ KG MNEMONICS ---*/
		memset (pDisplayData->aszSPMnemonic, 0, sizeof(pDisplayData->aszSPMnemonic));
        RflGetSpecMnem (pDisplayData->aszSPMnemonic, SPC_LB_ADR);
        pDisplayData->lQTY = pItemSales->lQTY;
    } else {                                /* non scalable */
        pDisplayData->uchMajorClass = pUifRegSales->uchMajorClass;
        pDisplayData->uchMinorClass = pUifRegSales->uchMinorClass;
//		pDisplayData->lAmount = pItemSales->lUnitPrice;  // fix for @/For lead thru display and PPI lead thru display issues

        if (labs(pItemSales->lQTY) == ITM_SL_QTY) {
            pDisplayData->lQTY = 0L;        /* don't display QTY */
        } else {
            pDisplayData->lQTY = pItemSales->lQTY;
        }
    }

	memset (pDisplayData->aszMnemonic, 0, sizeof(pDisplayData->aszMnemonic));
    if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) {
        RflGetTranMnem (pDisplayData->aszMnemonic, TRN_MODID_ADR);   /* trans. mnemonics add. for Mod. item disc. */
    } else {
        _tcsncpy(pDisplayData->aszMnemonic, pItemSales->aszMnemonic, STD_PLU_MNEMONIC_LEN);
		memset (pDisplayData->aszAdjMnem, 0, sizeof(pDisplayData->aszAdjMnem));
        if (pItemSales->uchAdjective != 0) {
            RflGetAdj (pDisplayData->aszAdjMnem, pItemSales->uchAdjective);
        }
    }

    pDisplayData->uchFsc = pItemSales->uchSeatNo;   /* display seat no. */
    
    pDisplayData->fbSaveControl = DISP_SAVE_TYPE_0;        /* 0: not save data, 1: save display data */
        
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {
        flDispRegDescrControl |= CRED_CNTRL;    /* turn on CREDIT descriptor control flag */
    } 

    if (pItemSales->fbModifier & TAX_MOD_MASK) {  /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL;    /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= TAXMOD_CNTRL;    /* turn on descriptor control flag for condiment */
    }

    if (pItemSales->fbModifier & FOOD_MODIFIER) {   /* Saratoga */
        flDispRegDescrControl |= FOODSTAMP_CTL;
        flDispRegKeepControl |= FOODSTAMP_CTL;
    } 

    if (pItemSales->fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;    /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= VOID_CNTRL;    /* turn on descriptor control flag for condiment */
    } else {
        flDispRegDescrControl &= ~VOID_CNTRL;    /* turn off descriptor control flag for beverage dispenser R3.1 */
        flDispRegKeepControl &= ~VOID_CNTRL;    /* turn off descriptor control flag for beverage dispenser R3.1 */
    }

    if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_OEPMODDISC) ||     /* R3.0 */
        (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

        flDispRegDescrControl |= ITEMDISC_CNTRL;
        flDispRegKeepControl |= ITEMDISC_CNTRL;
    }

    if ((pItemSales->uchMinorClass == CLASS_DEPT) || (pItemSales->uchMinorClass == CLASS_DEPTMODDISC)) { 
     
        flDispRegKeepControl &= ~(TAXMOD_CNTRL | VOID_CNTRL | FOODSTAMP_CTL | ITEMDISC_CNTRL);
    }

/*this has been removed because NHPOS does not currently use an ADJ descriptor, so we now use this
	descritpor control for KDS*/
#if 0
    /* control flag for 10N10D display */
    if (pUifRegSales->uchAdjective != 0) {  /* depressed adj. key */
        flDispRegDescrControl |= ADJ_CNTRL; /* turn on adj. descriptor control flag for condiment */
        flDispRegKeepControl |= ADJ_CNTRL; /* turn on adj. descriptor control flag for condiment */
    } 
#endif

    /*--- CALL DISPLAY MODULE ---*/
    DispWrite(pDisplayData);    
    
    /*--- TURN OFF DESCRIPTOR CONTROL FLAG ---*/
    flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | FOODSTAMP_CTL /*| ADJ_CNTRL*/);  /* for 10N10D display */

    /*--- TURN OFF KEEP CONTROL FLAG ---*/
    flDispRegKeepControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | FOODSTAMP_CTL /*| ADJ_CNTRL*/);  /* for 10N10D display */

}


/*
*===========================================================================
**Synopsis: SHORT    ItemSalesSpvInt(ITEMSALES *pItemSales)
*
*    Input: ITEMSALES *pItemSales 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : 
*         
** Description: Determine if Supervisor Intervention is required for this item and
*               if so then request Supervisor confirmation.
*                
*===========================================================================
*/
SHORT    ItemSalesSpvInt(CONST ITEMSALES *pItemSales)
{
	SHORT  sRequireSpvInt = 0;      /* default is supr. int.isn't required */
    
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_REQ_SUPINT) {    // PLU marked as require supr. int.
        sRequireSpvInt = 1;  
    } else if ((pItemSales->fbModifier & VOID_MODIFIER) && CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) {   /* require supr. int. on void */
        sRequireSpvInt = 1;  
    }

    if (sRequireSpvInt) {  
		SHORT sReturnStatus;
		USHORT usControl = UieNotonFile(UIE_ENABLE);                   /* disable using scanner */

        if ((sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != UIF_SUCCESS) {
            UieNotonFile(usControl);                      /* enable scanner */
            return(sReturnStatus);
        }
        /* correct UieNotonFile position, V1.0.13 */
	    UieNotonFile(usControl);                      /* enable scanner */
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT    ItemSalesSpvIntForCond(ITEMSALES *pItemSales)
*
*    Input: ITEMSALES *pItemSales 
*           UIFREGSALES *pUifRegSales
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : 
*         
** Description: Calculate product amount for display.  
*                
*===========================================================================
*/

SHORT    ItemSalesSpvIntForCond(ITEMSALES *pItemSales)
{
    SHORT sReturnStatus;
    USHORT usControl;
    
    if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_REQ_SUPINT) {
        usControl = UieNotonFile(UIE_ENABLE);                   /* disable using scanner */
        if ((sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != UIF_SUCCESS) {

            UieNotonFile(usControl);                      /* enable scanner */
            return(sReturnStatus);
        }
        /* correct UieNotonFile position, V1.0.13 */
	    UieNotonFile(usControl);                      /* enable scanner */
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesComnIFVoidHALO()
*
*    Input: 
*
*   Output: None
*
*    InOut: *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  
*                
*===========================================================================
*/

SHORT   ItemSalesComnIFVoidHALO(DCURRENCY lAmount)
{
	PARATRANSHALO      HaloDataRcvBuff;

    HaloDataRcvBuff.uchMajorClass = CLASS_PARATRANSHALO;
    HaloDataRcvBuff.uchAddress = HALO_VOID_ADR;
                
    CliParaRead(&HaloDataRcvBuff);  /* get halo amount */    
    
    if (RflHALO(lAmount, HaloDataRcvBuff.uchHALO) != RFL_SUCCESS) {
        return(LDT_KEYOVER_ADR);
    }
    return(ITM_SUCCESS);
}



/*
*===========================================================================
**Synopsis: VOID    ItemSalesCancelPrev(VOID)
*
*    Input: 
*
*   Output: None
*
*    InOut: *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  
*                
*===========================================================================
*/

VOID    ItemSalesCancelPrev(VOID)
{
    ITEMCOMMONLOCAL     *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    ItemSalesCalcECCom(&(pCommonLocalRcvBuff->ItemSales));  /* 2172 */

    memset(&(pCommonLocalRcvBuff->ItemSales), 0, sizeof(pCommonLocalRcvBuff->ItemSales));
    pCommonLocalRcvBuff->usSalesBuffSize = 0;
    memset(&(pCommonLocalRcvBuff->ItemDisc), 0, sizeof(pCommonLocalRcvBuff->ItemDisc));
    pCommonLocalRcvBuff->usDiscBuffSize = 0;
    pCommonLocalRcvBuff->uchItemNo = 0;

    pCommonLocalRcvBuff->fbCommonStatus &= ~COMMON_VOID_EC;   /* enable error correct */

    ItemSalesLocal.fbSalesStatus &= ~ ( SALES_PM_COMPLSRY | SALES_COND_COMPLSRY | SALES_DISPENSER_SALES);

    MldECScrollWrite();                                 /* R3.0 */
    ItemCommonDispECSubTotal(0);              /* Saratoga */
}

/****** End of Source ******/