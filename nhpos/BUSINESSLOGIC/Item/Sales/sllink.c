/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : PLU sales module                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLLINK.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesLinkPLU() : PLU Link Item sales module main   
*           ItemSalesLinkPrepare() : PLU Link Item parepare routine
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
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
#include    <stdlib.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "rfl.h"
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
#include    "mld.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesLinkPLU(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, USHORT fsCheck)
*
*    Input: UIFREGSALES *pUifRegSales
*           ITEMSALES   *pItemSales
*           USHORT      fsCheck 0: send to transaction module
*                               1: check only whether acceptable or not
*                               2: send to kds
*
*   Output: 
*
*    InOut: None
*
**Return: Return status returned from sub module 
*
** Description: This module is the main module of Link PLU module.
                
*===========================================================================
*/
SHORT   ItemSalesLinkPLU(ITEMSALES *pItemSales, USHORT fsCheck)
{
	UIFREGSALES     UifRegSales = {0};
    ITEMSALES       ItemLink = {0};
    SHORT   sReturnStatus;
    SHORT   sSavedReturn;
    UCHAR   uchRestrict;

    if (pItemSales->usLinkNo == 0) {
        return ITM_SUCCESS;
    }

    sSavedReturn = ITM_SUCCESS;     /* initialize */
    
    UifRegSales.uchMajorClass = CLASS_UIFREGSALES;
    UifRegSales.uchMinorClass = CLASS_UIPLU;
    UifRegSales.lQTY = pItemSales->lQTY;
    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        UifRegSales.lQTY = ITM_SL_QTY;
    }

    if (UifRegSales.lQTY < 0) UifRegSales.lQTY *= -1;
    UifRegSales.uchAdjective = pItemSales->uchAdjective;
    UifRegSales.fbModifier = pItemSales->fbModifier;
    UifRegSales.fbModifier2 = pItemSales->fbModifier2;
    
    ItemLink.uchMajorClass = CLASS_ITEMSALES;
    ItemLink.uchMinorClass = CLASS_PLU;
    if (pItemSales->uchMinorClass == CLASS_PLUMODDISC) {
        ItemLink.uchMinorClass = CLASS_PLUMODDISC;
    }

    /*----- BRANCH OUT DEPT AND PLU CORRESPOND WITH MINOR CLASS -----*/
    switch (pItemSales->uchMinorClass) {
    case CLASS_DEPT:
    case CLASS_DEPTMODDISC:                 /* modefied discounted dept */
        return(LDT_PROHBT_ADR);         /* error if dept isn't preset */

    case CLASS_SETMENU:
    case CLASS_SETITEMDISC:
    case CLASS_SETMODDISC:
        return(ITM_SUCCESS);         /* ignore link plu at modified item disocunt */
        
    case CLASS_PLU:
    case CLASS_PLUITEMDISC:
    case CLASS_PLUMODDISC:                  /* modified discounted PLU */
        if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
            /* allow link plu to noun/label plu */
            return (LDT_PROHBT_ADR);
        } else {
			TCHAR   auchLinkNo[NUM_PLU_LEN + 1] = {0};
			LABELANALYSIS   PluLabel = {0};

			_itot(pItemSales->usLinkNo, auchLinkNo, 10);
			_tcsncpy(PluLabel.aszScanPlu, auchLinkNo, NUM_PLU_LEN);
			if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
				_tcsncpy(ItemLink.auchPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
			} else {
				return (LDT_PROHBT_ADR);
			}
		}
		break;
    }
        
    if ((sReturnStatus = ItemSalesLinkPrepare(&UifRegSales, &ItemLink, &uchRestrict)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }
    
    if (ItemLink.ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        return (LDT_PROHBT_ADR);
    }                                  

    /* ----- check sales restriction, 2172 ----- */
    if (ItemSalesSalesRestriction(&ItemLink, uchRestrict) != ITM_SUCCESS) {
        return (LDT_SALESRESTRICTED);
    }
            
    switch(ItemLink.ControlCode.uchItemType & PLU_TYPE_MASK_REG) {
    /* switch(ItemLink.ControlCode.uchDeptNo & PLU_TYPE_MASK_REG) { */
    case PLU_NON_ADJ_TYP:
        sReturnStatus = ITM_SUCCESS;
        /* sReturnStatus = ItemSalesNonAdj(&UifRegSales); */
        break;

    case PLU_ADJ_TYP:
        sReturnStatus = ItemSalesAdj(&UifRegSales, &ItemLink);
        break;

    default:
        return (LDT_PROHBT_ADR);    /* not open plu for link plu */
        break;
    }
    
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);    
    }  
    
    if ((sReturnStatus = ItemSalesModifier(&UifRegSales, &ItemLink)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if ((sReturnStatus = ItemSalesCalculation(&ItemLink)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }
    
    
    if (fsCheck == 1) {
        /* check only at parent item process */
        return (sReturnStatus);
    }

    if (fsCheck == 2) {
        /* normal sales, after parent item process */
        /* send to kds only */
        ItemLink.fbModifier |= pItemSales->fbModifier;
        if ((sReturnStatus = ItemSalesLinkCommonIF(&UifRegSales, &ItemLink, TRUE)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    } else if (fsCheck == 3) {
        /* repeat process */
        /* send to kds only */
        ItemLink.fbModifier |= pItemSales->fbModifier;
        if ((sReturnStatus = ItemSalesLinkCommonIF(&UifRegSales, &ItemLink, TRUE)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    } else {
        /* normal sales, before parent item process */
        
        if ((sReturnStatus = ItemSalesLinkCommonIF(&UifRegSales, &ItemLink, FALSE)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }

#if 1
		pItemSales->Condiment[0] = ItemCondimentData(&ItemLink);
#else
        pItemSales->Condiment[0].ControlCode.uchItemType = ItemLink.ControlCode.uchItemType;  
        _tcsncpy(pItemSales->Condiment[0].auchPLUNo, &ItemLink.auchPLUNo[0], NUM_PLU_LEN);
        pItemSales->Condiment[0].usDeptNo = ItemLink.usDeptNo;    
            
        pItemSales->Condiment[0].uchAdjective = ItemLink.uchAdjective;     

        _tcsncpy(pItemSales->Condiment[0].aszMnemonic, ItemLink.aszMnemonic, NUM_DEPTPLU);
        _tcsncpy(pItemSales->Condiment[0].aszMnemonicAlt, ItemLink.aszMnemonicAlt, NUM_DEPTPLU);
        pItemSales->Condiment[0].lUnitPrice = ItemLink.lUnitPrice; 
            
        pItemSales->Condiment[0].ControlCode.uchReportCode = ItemLink.ControlCode.uchReportCode;

        /* --- correct for kp#5 - #8 print R3.1 --- */
        memcpy(pItemSales->Condiment[0].ControlCode.auchPluStatus, &ItemLink.ControlCode.auchPluStatus[0], ITM_PLU_CONT_OTHER);
#endif

        pItemSales->uchChildNo++;           /* incrememt setmenu counter */
    }

    return(sReturnStatus);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPrepare(UIFREGSALES *pUifRegSales,
*                                    ITEMSALES *pItemSales)
*                                    
*    Input: UIFREGSALES *pUifRegSales
*           
*   Output: None
*
*    InOut: ITEMSALES *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense 
*                          error
*         LDT_PROHBT_ADR : Function prohibited by parameter option.
*
** Description: This function prepares ITEMSALES data. 
*                
*===========================================================================
*/
SHORT   ItemSalesLinkPrepare(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, UCHAR *puchRestrict)
{
    SHORT    sReturnStatus;
	DEPTIF   DeptRecRcvBuffer = {0};       /* dept record receive buffer */
	PLUIF    PLURecRcvBuffer = {0};        /* PLU record receive buffer */

    /*----- PLU PREPARATION ----- */
    /*----- GET PLU RECORD with pItemSales->usPLUNo as key -----*/
    /* prepair PLU record receive buffer */

    _tcsncpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);

	//SR 543, we no longer just assign adjective 1 to the structure, because
	//we are trying to find the price of a certain price level, so we do the
	//manipulation below to find which adjective price to read from. JHHJ
    PLURecRcvBuffer.uchPluAdj = ((pUifRegSales->uchAdjective - 1) % 5 + 1);

	//if we are still at 0, that means there is no certain price level, assign to 1
	if(PLURecRcvBuffer.uchPluAdj == 0)
	{
		PLURecRcvBuffer.uchPluAdj = 1;
	}

    if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
        return(OpConvertError(sReturnStatus));
    } 

	//checks to see if the PLU is a preset and if it is then changes the
	//adjective to 1 to get the correct price for the linked PLU.  SS
	if(PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType & PRESET_PLU )
	{
		PLURecRcvBuffer.uchPluAdj = 1;
		
		if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
			 return(OpConvertError(sReturnStatus));
		} 
	}

    /* move dept No. into pItemSales->uchDeptNo */
    pItemSales->usDeptNo = PLURecRcvBuffer.ParaPlu.ContPlu.usDept;
    pItemSales->ControlCode.uchItemType = PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType;  /* set PLU type */
    if (pItemSales->usDeptNo == 0) {
        return(LDT_PROHBT_ADR);         /* error if dept isn't preset */
    }
        
    /* copy report code */
    pItemSales->ControlCode.uchReportCode = (UCHAR) (PLURecRcvBuffer.ParaPlu.ContPlu.uchRept);
    pItemSales->uchPrintPriority = PLURecRcvBuffer.ParaPlu.uchPrintPriority;  /* R3.0 */
    _tcsncpy(pItemSales->aszMnemonic, PLURecRcvBuffer.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);

    /* Print Priority, R3.0 */
    pItemSales->uchPrintPriority = PLURecRcvBuffer.ParaPlu.uchPrintPriority; 

	/* copy control code - remember ControlCode.auchStatus[5] used for ItemSales status info */
    memcpy(pItemSales->ControlCode.auchPluStatus, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther, OP_PLU_PLU_SHARED_STATUS_LEN);
    memcpy(pItemSales->ControlCode.auchPluStatus + 6, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther + 5, OP_PLU_OPTIONS_STATUS_LEN);

    if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == 0) {
    /* type 0 is regarded as open PLU */
        return(LDT_PROHBT_ADR);         /* error if dept isn't preset */
    }                                   
    /*
    if (pItemSales->ControlCode.auchStatus[2] & PLU_CONDIMENT) {
        return(LDT_PROHBT_ADR);
            
    } */                                  
        
    /* price multiple, 2172 */
    pItemSales->uchPM = PLURecRcvBuffer.ParaPlu.uchPriceMulWeight;
    if (pItemSales->uchPM > 1) {
        return(LDT_PROHBT_ADR);
    }

    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        return(LDT_PROHBT_ADR);
    }
        
    /* set QTY and unit price */
    pItemSales->lQTY = pUifRegSales->lQTY;

    if (pItemSales->ControlCode.uchItemType & PRESET_PLU) {
                                            /* if not open PLU */
        /* set record into pItemSales */
        RflConv3bto4b(&pItemSales->lUnitPrice , PLURecRcvBuffer.ParaPlu.auchPrice);
    }
        
    /*----- INHERIT DEPT'S CONTROL STATUS -----*/
    /* get dept record */
    DeptRecRcvBuffer.usDeptNo = pItemSales->usDeptNo;
    if ((sReturnStatus = CliOpDeptIndRead(&DeptRecRcvBuffer, 0)) != OP_PERFORM) {
		char  xBuff[128];

		sprintf (xBuff, "==PROVISION: Link PLU Dept read error %d dept %d.", sReturnStatus, pItemSales->usDeptNo);
		NHPOS_ASSERT_TEXT((sReturnStatus == OP_PERFORM), xBuff);
        return(OpConvertError(sReturnStatus));
    }
            
    /* Print Priority, R3.0 */
    if (CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT1)) {
        pItemSales->uchPrintPriority = DeptRecRcvBuffer.ParaDept.uchPrintPriority;
    }

    ItemSalesSetupStatus(DeptRecRcvBuffer.ParaDept.auchControlCode);
                                            /* setup dept's control status */
    /* turn on Hash bit for PLU */
    pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_HASH;               /* turn off flag set by maintemance */
    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_USE_DEPTCTL) {    /* use dept's control code */
        /* copy dept's control code into pItemSales */
        memcpy(pItemSales->ControlCode.auchPluStatus, DeptRecRcvBuffer.ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
        pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] = DeptRecRcvBuffer.ParaDept.auchControlCode[4];   /* CRT No5 - No 8 see ItemSendKdsSetCRTNo() */
    } else {                            /* turn on hash bit if need */
        if (DeptRecRcvBuffer.ParaDept.auchControlCode[2] & PLU_HASH) {
            pItemSales->ControlCode.auchPluStatus[2] |= PLU_HASH;
        }
    }

    pItemSales->ControlCode.uchItemType &= PLU_TYPE_MASK_REG;     /* clear dept No. "0xc0" */
    /* pItemSales->ControlCode.uchDeptNo |= DeptRecRcvBuffer.ParaDept.uchMdept; */
                                            /* set dept's major dept No. */

    /*----- SET UP PLU STATUS -----*/
    if (!(pItemSales->ControlCode.uchItemType & PRESET_PLU)) {
        pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_CONDIMENT; 
    } else if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        pItemSales->ControlCode.auchPluStatus[2] &= ~ PLU_SCALABLE;   /* turn off scalable status bit */ 
    }
    pItemSales->usLinkNo = 0;

    *puchRestrict = PLURecRcvBuffer.ParaPlu.uchRestrict;

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCommonIF(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: 
*   Output: ITEMSALES *pItemSales struct updated with MDC and other info.
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  Update the ITEMSALES data with MDC settings, modify the fsPrintStatus,
*                and set the sign of the price and quantity based on whether VOID
*                transaction or item or not.
*===========================================================================
*/
SHORT   ItemSalesLinkCommonIF(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, USHORT fsSend)
{
    ITEMCOMMONLOCAL   * CONST pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
    SHORT             sReturnStatus;
    USHORT            usGuestNo = 0;
                                                                        
    /*--- SET SIGN ---*/
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {
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

    /* check consolidatation mode or not,   Delete R3.0 */

    /*----- SET PRINT STATUS TO pItemSales -----*/
    pItemSales->fsPrintStatus |= (PRT_SLIP | PRT_JOURNAL);      /* set journal and slip print status */
    /*----- SET TICKET STATUS -----*/
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_SINGREC) {      /* 0x40, issue single or double ? */
        pItemSales->fsPrintStatus |= PRT_SINGLE_RECPT;                 /* issue single */
        if (pItemSales->ControlCode.auchPluStatus[0] & PLU_DBLREC) {   /* 0x80 */
            pItemSales->fsPrintStatus |= PRT_DOUBLE_RECPT;
        }
    }

    /* set kitchen print status */
    pItemSales->fsPrintStatus |= (pItemSales->ControlCode.auchPluStatus[2] & ITM_KTCHN_SEND_MASK);

    /* set special print status */
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_SPECIAL) {
        pItemSales->fsPrintStatus |= PRT_SPCL_PRINT;
    }

    /*----- SET STATUS ACCORDING TO MDC OPTION -----*/
	{
		UCHAR  auchMDCSave[2] = {0};

		auchMDCSave[0] = CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3);

		/*----- HOURLY TOTAL AFFECTION STATUS -----*/
		if ((pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) &&  /* V3.3 */
			(pItemSales->ControlCode.auchPluStatus[2] & PLU_HASH)) {
			if (auchMDCSave[0] & ODD_MDC_BIT1) {                          /* affect to financial total */  
				pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_HASH;    /* reset hash status, assume as minus */
			}
		}
		pItemSales->ControlCode.auchPluStatus[5] |= (ITM_SALES_HOURLY | ITM_HASH_FINANC);  /* affect normaly */
		if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {
			if (auchMDCSave[0] & ODD_MDC_BIT0) {                                    /* not affect hourly total */  
				pItemSales->ControlCode.auchPluStatus[5] &= ~ITM_SALES_HOURLY;             /* turn off affect status */
			}
		}                                           
		if (pItemSales->ControlCode.auchPluStatus[2] & PLU_HASH) {
			if (!(auchMDCSave[0] & ODD_MDC_BIT1)) {                                /* not affect to financial total */  
				pItemSales->ControlCode.auchPluStatus[5] &= ~ITM_HASH_FINANC;              /* turn off affect status */
			}
			if (!(auchMDCSave[0] & ODD_MDC_BIT2)) {                               /* not affect to hourly total */  
				pItemSales->ControlCode.auchPluStatus[5] &= ~ITM_SALES_HOURLY;            /* turn off affect status */
			}
		}                                                
		if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {     
			/*--- AFFECTION OPTION STATUS FOR SCALABLE ---*/
			if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                    /* address 29 1: accumulated by 0.01 LB/Kg */
				pItemSales->ControlCode.auchPluStatus[5] |= ITM_AFFECT_DEV_BY_10;         /* affect */
			}
		}
	}
    
    /* ---- set seat no R3.1 ---- */
    pItemSales->uchSeatNo = TranCurQualPtr->uchSeat;

    /*----- CHECK STORAGE SIZE -----*/
    if (ItemCommonCheckSize(pItemSales, ITM_CONDIMENT_SIZE) < 0) {   /* buffer full condition */
        return(LDT_TAKETL_ADR);
    }
                                            
    /*--- Set Coupon Quantity,   R3.0 ---*/
    pItemSales->sCouponQTY = (SHORT)(pItemSales->lQTY / ITM_SL_QTY);
    
#if 0
    /*--- MODIFIED DISC PROCESS ---*/
    ItemCommonGetLocal(&pCommonLocalRcvBuff);
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
#endif

    /*--- CLEAR TAX MODIFIER DATA EXCEPT EXEMPT KEY ---*/
	{
		ITEMMODLOCAL   *pModLocalRcvBuff = ItemModGetLocalPtr();

		pModLocalRcvBuff->fsTaxable = 0;   /* clear taxable status indicators */
	}
    
    /*----- SET E.C DISABLE STATUS -----*/
    if (pItemSales->fbModifier & VOID_MODIFIER) {
        pCommonLocalRcvBuff->fbCommonStatus |= COMMON_VOID_EC;      /* turn on disable E.C bit */
    }
    /*----- CANCEL TAX EXEMPT KEY -----*/
    ItemCommonCancelExempt();

    if (fsSend) {
		REGDISPMSG    DisplayData = {0};
		UCHAR         uchItemStatus;
          
        /*----- MAKE I/F WITH DISPLAY MODULE -----*/
        DisplayData.lAmount = pItemSales->lProduct;

        /*----- MANIPURATE MENU SHIFT PAGE -----*/
        if ((pItemSales->uchMinorClass == CLASS_PLU) ||
            (pItemSales->uchMinorClass == CLASS_OEPPLU) ||      /* R3.0 */
            (pItemSales->uchMinorClass == CLASS_SETMENU)) {

            ItemSalesSetDefltPage();
        }
        ItemSalesDisplay(pUifRegSales, pItemSales, &DisplayData);
        
        /*----- VALIDATION PRINT FOR MINUS ITEM -----*/
        uchItemStatus = 0;                      /* clear work status */
        if (pItemSales->ControlCode.auchPluStatus[0] & PLU_PRT_VAL) {
            uchItemStatus = COMP_VAL;
        } else if ((pItemSales->fbModifier & VOID_MODIFIER) && (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1))) { 
            if (!(pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS)) {
                uchItemStatus = COMP_VAL;       /* comp. validation for void */
            }
        } else if (pItemSales->fsLabelStatus & ITM_CONTROL_NOTINFILE) {
            if (CliParaMDCCheck(MDC_PLU4_ADR, ODD_MDC_BIT1)) {
                uchItemStatus = COMP_VAL;
            }
        }

        if (uchItemStatus == COMP_VAL) {              /* req. validation */
			USHORT  usBuffer = pItemSales->fsPrintStatus;      /* store print status */

            pItemSales->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(pItemSales);                            /* print validation (VOID function) */
    
            pItemSales->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);   /* restore print status */
        }
        
        /* send to enhanced kds, 2172 */
        sReturnStatus = ItemSendKds(pItemSales, 0);
        if (sReturnStatus != ITM_SUCCESS) {
            return sReturnStatus;
        }
    }

    return(ITM_SUCCESS);
}

/****** end of file ******/
