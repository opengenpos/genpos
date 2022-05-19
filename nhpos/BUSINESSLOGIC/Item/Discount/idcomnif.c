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
* Title       : Common module interface.                          
* Category    : Dicount Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: IDCOMNIF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Za /Zp /G1s /I                                
* --------------------------------------------------------------------------
* Abstract: ItemDiscCommonIF() : I/F with item common module.    
*           ItemDiscDisplay()  : Display discount data.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun-20-92: 00.00.01  : K.Maeda   : initial                                   
* Aug-26-93: 01.00.13  : K.You     : bug fixed E-2 (mod. ItemDiscDisplay)                        
* Oct-19-93: 02.00.02  : K.You     : del canada tax feature.                        
* Feb-01-95: 03.00.00  : hkato     : R3.0
* Nov-09-95: 03.01.00  : hkato     : R3.1
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
#include    "display.h"
#include    "mld.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"


#define     USNOTCASHIER    1    

/*
*===========================================================================
**Synopsis: VOID    ItemDiscCommonTranMnemonic (UCHAR uchMinorClass, UCHAR uchDiscountNo, PARATRANSMNEMO  *pTransMnemoRcvBuff)
*
*    Input: UCHAR uchMinorClass
*           UCHAR uchDiscountNo, 
*
*   Output: PARATRANSMNEMO  *pTransMnemoRcvBuff
*
*    InOut: None
*
**Return:    None
*         
** Description: Determine the address for a particular type of discount mnemonic.
*               This function will modify the PARATRANSMNEMO  *pTransMnemoRcvBuff
*               updating the major class and the address for the particular mnemonic
*               for a particular class of discount such as item discount, regular discount, etc.
*               The function will then do the actual fetching of the mnemonic as well.
*
*               See Also:    PrtChkDiscAdr(), MldChkDiscAdr()
*===========================================================================
*/
VOID    ItemDiscCommonTranMnemonic (UCHAR uchMinorClass, UCHAR uchDiscountNo, PARATRANSMNEMO  *pTransMnemoRcvBuff)
{
	pTransMnemoRcvBuff->uchMajorClass = CLASS_PARATRANSMNEMO;

    switch(uchMinorClass) {           
    case CLASS_ITEMDISC1:
		pTransMnemoRcvBuff->uchAddress = ItemDiscGetMnemonic_ITEMDISC1 (uchDiscountNo);
        break;
    
    case CLASS_REGDISC1:            
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC1_ADR;   /* trans. mnemonics for reg. disc. #1 */
        break;
    
    case CLASS_REGDISC2:            
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC2_ADR;   /* trans. mnemonics for reg. disc. #2 */
        break;
    
    case CLASS_REGDISC3:                    /* R3.1 */
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC3_ADR;
        break;
    
    case CLASS_REGDISC4:                    /* R3.1 */
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC4_ADR;
        break;
    
    case CLASS_REGDISC5:                    /* R3.1 */
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC5_ADR;
        break;
    
    case CLASS_REGDISC6:                    /* R3.1 */
        pTransMnemoRcvBuff->uchAddress = TRN_RDISC6_ADR;
        break;
    
    case CLASS_CHARGETIP:           
        pTransMnemoRcvBuff->uchAddress = TRN_CHRGTIP_ADR;  /* trans. mnemonics for charge tip */
        break;
    
    case CLASS_CHARGETIP2:           /* V3.3 */
        pTransMnemoRcvBuff->uchAddress = TRN_CHRGTIP2_ADR; /* trans. mnemonics for charge tip */
        break;
    
    case CLASS_CHARGETIP3:           
        pTransMnemoRcvBuff->uchAddress = TRN_CHRGTIP3_ADR; /* trans. mnemonics for charge tip */
        break;
    
    default:
        return;     /* not display at auto charge tips, V3.3 */
        break;
    }
    
    CliParaRead(pTransMnemoRcvBuff);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCommonif(UIFREGDISC *pUifRegDisc, ITEMDISC *pDiscData)
*
*    Input: UIFREGDISC *pUifRegDisc,
*           ITEMDISC *pDiscData, 
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         
** Description: Tax Mod, Number entry for Item discount. 
*                
*===========================================================================
*/

#define     CTIP_ITEM_REC       0x10        /* issue ticket */
#define     CTIP_DOUBLE_REC     0x20        /* issue double ticket */

#define     DISC_REQ_SPV_INT        1
#define     DISC_REQ_VAL_PRINT      2
#define     DISC_SPV_INT_DONE       1
#define     DISC_SPV_INT_MDC_MASK   0x40
#define     DISC_VAL_MDC_MASK       0x80


SHORT   ItemDiscCommonIF(CONST UIFREGDISC *pUifRegDisc, ITEMDISC *pItemDisc)
{
    UCHAR      uchDiscStatus = 0;
    UCHAR      uchMDCSPV = DISC_SPV_INT_MDC_MASK;  // default to requires super intervention
    SHORT      sCompSize;
    SHORT      sReturnStatus;
    USHORT     usTempBuffSize;
    ITEMCOMMONLOCAL    * const pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
	PARAMDC            MDCRcvBuff = { 0 };
	SHORT      sStorage;

    switch(pItemDisc->uchMinorClass) {
    case CLASS_CHARGETIP:
    case CLASS_CHARGETIP2:      /* V3.3 */
    case CLASS_CHARGETIP3:
    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
/* --- V3.3
        if (GCFQualRcvBuff->usWaiterID != 0) {
            pItemDisc->usID = GCFQualRcvBuff->usWaiterID;
        } else {
*/
            pItemDisc->ulID = TranGCFQualPtr->ulCashierID;
/*        }
*/    }

    /*----- SET PRINT STATUS TO pItemDisc -----*/
    pItemDisc->fsPrintStatus = (PRT_JOURNAL | PRT_SLIP); 

    /* get validation/ super. int. option MDC */
    MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;

	uchMDCSPV = 0;
    switch(pUifRegDisc->uchMinorClass) {           
    case CLASS_UIITEMDISC1:
		pItemDisc->uchDiscountNo = pUifRegDisc->uchDiscountNo;
        MDCRcvBuff.usAddress = ItemDiscGetMdcCheck_ITEMDISC1(pItemDisc->uchDiscountNo, MDC_MODID12_ADR);  /* address 32 */
		uchMDCSPV |= (CliParaMDCCheckField(MDCRcvBuff.usAddress, MDC_PARAM_BIT_B) ? DISC_SPV_INT_MDC_MASK : 0);
		uchMDCSPV |= (CliParaMDCCheckField(MDCRcvBuff.usAddress, MDC_PARAM_BIT_A) ? DISC_VAL_MDC_MASK : 0);

        /*--- MANIPURATE DISCOUNTABLE STATUS FOR ITEM DISC ---*/
        if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
            pItemDisc->auchDiscStatus[2] |= ITM_DISC_AFFECT_DISC1;
        }

        if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
            pItemDisc->auchDiscStatus[2] |= ITM_DISC_AFFECT_DISC2;
        }
		if (CliParaMDCCheckField(MDC_PLU2_ADR, MDC_PARAM_BIT_D) &&
			(pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1] & (PLU_AFFECT_DISC1 | PLU_AFFECT_DISC2)) == 0) {
			// PLU does not have affect discountable itemizer turned on yet we are attempting an item discount
			// so do not allow.
			NHPOS_NONASSERT_NOTE("PROVISIONING", "==PROVISIONING: MDC 295 D - item discount attempted on PLU not affect discountable itemizers.");
			return(LDT_PROHBT_ADR);
		}
		break;

    case CLASS_UIREGDISC1:            
        MDCRcvBuff.usAddress = MDC_RDISC11_ADR;
                                            /* address 39 */
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;
    
    case CLASS_UIREGDISC2:            
        MDCRcvBuff.usAddress = MDC_RDISC21_ADR;
                                            /* address 43 */
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;
    
    case CLASS_UIREGDISC3:                  /* R3.1 */
        MDCRcvBuff.usAddress = MDC_RDISC31_ADR;
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;

    case CLASS_UIREGDISC4:                  /* R3.1 */
        MDCRcvBuff.usAddress = MDC_RDISC41_ADR;
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;

    case CLASS_UIREGDISC5:                  /* R3.1 */
        MDCRcvBuff.usAddress = MDC_RDISC51_ADR;
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;

    case CLASS_UIREGDISC6:                  /* R3.1 */
        MDCRcvBuff.usAddress = MDC_RDISC61_ADR;
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = MDCRcvBuff.uchMDCData;
        break;

    case CLASS_UICHARGETIP:           
    case CLASS_UICHARGETIP2:                /* V3.3 */
    case CLASS_UICHARGETIP3:           
        MDCRcvBuff.usAddress = MDC_CHRGTIP1_ADR;
                                            /* address 23 */
        CliParaRead(&MDCRcvBuff); 
        uchMDCSPV = (UCHAR) (MDCRcvBuff.uchMDCData << 4);
                                            /* justify bit pos */
        /*--- SET TICKET PRINT STATUS ---*/
        if (MDCRcvBuff.uchMDCData & CTIP_ITEM_REC) { 
            pItemDisc->fsPrintStatus |= PRT_SINGLE_RECPT;
                                            /* issue single receipt */
            if (MDCRcvBuff.uchMDCData & CTIP_DOUBLE_REC) { 
                pItemDisc->fsPrintStatus |= PRT_DOUBLE_RECPT;
                                            /* issue double receipt */
            }
        }

        break;
    
    case CLASS_UIAUTOCHARGETIP:             /* V3.3 */
        /* do not execute unbuffing print at the total key */
        pItemDisc->fsPrintStatus |= PRT_NO_UNBUFFING_PRINT;
        break;

    default:
		PifLog (MODULE_DISCOUNT, LOG_EVENT_UI_DISCOUNT_ERR_03);
        break;
    }

    /*----- Set Seat#,  R3.1 -----*/
    /* set set no. of parent item for cursor item discount, V3.3 */
    if (pItemDisc->uchMinorClass == CLASS_ITEMDISC1) {
        pItemDisc->uchSeatNo = pCommonLocalRcvBuff->ItemSales.uchSeatNo;
    }

	// fetch the mnemonic
	{
		PARATRANSMNEMO  WorkMnem = { 0 };

		ItemDiscCommonTranMnemonic (pItemDisc->uchMinorClass, pItemDisc->uchDiscountNo, &WorkMnem);
		_tcsncpy(pItemDisc->aszMnemonic, WorkMnem.aszMnemonics, NUM_DEPTPLU);
	}

    /*--- CHECK STRAGE SIZE ---*/
    usTempBuffSize = pCommonLocalRcvBuff->usSalesBuffSize + pCommonLocalRcvBuff->usDiscBuffSize;
    if ((sCompSize = ItemCommonCheckSize(pItemDisc, usTempBuffSize)) < 0) {
        return(LDT_TAKETL_ADR);
    }

    if ((pItemDisc->uchMinorClass == CLASS_REGDISC1) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC2) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC3) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC4) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC5) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC6)) {

		if ((pItemDisc->fbDiscModifier & VOIDSEARCH_MODIFIER ) && (pItemDisc->uchSeatNo)) {
			/* get discount taxable amount from storage */
			pItemDisc->fbDiscModifier &= ~(VOID_MODIFIER|VOIDSEARCH_MODIFIER);
			pItemDisc->lAmount *= -1L;

	    	if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
        		sStorage = TRN_TYPE_CONSSTORAGE;
	    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
		        if (TranGCFQualPtr->usGuestNo) {
	    	        sStorage = TRN_TYPE_CONSSTORAGE;
    	    	} else {
        	    	sStorage = TRN_TYPE_ITEMSTORAGE;
	        	}
	    	} else {
    	    	sStorage = TRN_TYPE_ITEMSTORAGE;
	    	}

	    	TrnVoidSearch(pItemDisc, 1, sStorage);
    		/*if ((sReturnStatus = TrnVoidSearch(pItemDisc, 1, sStorage)) != TRN_SUCCESS) {
        		return(sReturnStatus);
		    }*/
			pItemDisc->fbDiscModifier |= (VOID_MODIFIER | VOIDSEARCH_MODIFIER);
			pItemDisc->lAmount *= -1L;
		}
	}

    /*--- SUPERVISOR INTERVENTION ---*/
    uchDiscStatus &= ~DISC_REQ_SPV_INT;     /* initialize status */                         
    if (uchMDCSPV & DISC_SPV_INT_MDC_MASK) {   /* use uchMDC only for spv. int. */
                                        /* require supr. int. */
        uchDiscStatus |= DISC_REQ_SPV_INT;
    } else if ((pItemDisc->fbDiscModifier & VOID_MODIFIER) && CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) {
                                        /* require supr. int. */
        uchDiscStatus |= DISC_REQ_SPV_INT;
    }
    if (pUifRegDisc->uchMinorClass == CLASS_UIAUTOCHARGETIP) {
        uchDiscStatus &= ~DISC_REQ_SPV_INT; /* reset spv int at auto charge tips, V3.3 */
    }
    if (uchDiscStatus & DISC_REQ_SPV_INT) {
        if ((sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }

	//Allow void charge tips as first item on a check as per GSU SR 6 by cwunn 
	sReturnStatus = ItemCommonSetupTransEnviron ();
	if(sReturnStatus != ITM_SUCCESS)
		return (sReturnStatus);

    /*--- SEND TO COMMON MODULE ---*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT || TrnSplCheckSplit() == TRN_SPL_MULTI || TrnSplCheckSplit() == TRN_SPL_TYPE2){
        ItemPreviousItemSpl(pItemDisc, TRN_TYPE_SPLITA);
    } else {
        if ((sReturnStatus = ItemPreviousItem(pItemDisc, sCompSize)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }
    if ((pItemDisc->uchMinorClass == CLASS_REGDISC1) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC2) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC3) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC4) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC5) ||
        (pItemDisc->uchMinorClass == CLASS_REGDISC6))
	{
		ITEMSALESLOCAL  *pWorkSales;
		pWorkSales = ItemSalesGetLocalPointer();
		if (pItemDisc->fbDiscModifier & VOID_MODIFIER) {
			pWorkSales->fbSalesStatus &= ~SALES_TRANSACTION_DISC;
		} else {
			pWorkSales->fbSalesStatus |= SALES_TRANSACTION_DISC;
		}
	}

    /*----- Display Discount to LCD -----*/

    /* not display auto charge tips on lcd at total key depression */
    if (pUifRegDisc->uchMinorClass != CLASS_UIAUTOCHARGETIP) {
		SHORT  sSplitType;

        if (pItemDisc->uchSeatNo == 'B') {              /* V3.3 */
            pItemDisc->uchSeatNo = 0;
        }

        sSplitType = TrnSplCheckSplit();
        if (sSplitType == TRN_SPL_SEAT || sSplitType == TRN_SPL_MULTI || sSplitType == TRN_SPL_TYPE2)
		{
			TRANITEMIZERS      *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);

            MldScrollWrite2(MLD_DRIVE_2, pItemDisc, MLD_NEW_ITEMIZE);
            MldDispSubTotal(MLD_TOTAL_2, WorkTI->lMI + pItemDisc->lAmount);
        } else {
            MldScrollWrite(pItemDisc, MLD_NEW_ITEMIZE);
            ItemCommonDispSubTotal(pItemDisc);
        }

        /* send to enhanced kds, 2172 */
        sReturnStatus = ItemSendKds(pItemDisc, 0);
        if (sReturnStatus != ITM_SUCCESS) {
            return sReturnStatus;
        }
    }

    /*--- MANIPURAE STORAGE STATUS FOR ITEM DISC. ---*/
//    if (pItemDisc->uchMinorClass == CLASS_ITEMDISC1) {
//        ItemCommonGetLocal(&pCommonLocalRcvBuff);
//        pCommonLocalRcvBuff->ItemSales.fbStorageStatus |= NOT_ITEM_CONS;
//        ItemCommonPutLocal(pCommonLocalRcvBuff);
//    }

    /*----- SET E.C DISABLE STATUS -----*/
    if (pItemDisc->fbDiscModifier & VOID_MODIFIER) {
        pCommonLocalRcvBuff->fbCommonStatus |= COMMON_VOID_EC;      /* turn on disable E.C bit */
    }
    
    /*--- VALIDATION PRINTING ---*/
    uchDiscStatus &= ~DISC_REQ_VAL_PRINT;                           /* initialize status */
    if (pUifRegDisc->uchMinorClass != CLASS_UICHARGETIP) { 
        if (uchMDCSPV & DISC_VAL_MDC_MASK) {                        /* require validation print for disc. */
            uchDiscStatus |= DISC_REQ_VAL_PRINT;
        } 
    }
    if ((pItemDisc->fbDiscModifier & VOID_MODIFIER) && CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1)) {
                                        /* add. 61  */
        uchDiscStatus |= DISC_REQ_VAL_PRINT;
    }    
    if (uchDiscStatus & DISC_REQ_VAL_PRINT) {
		USHORT      usBuffer;

        usBuffer = pItemDisc->fsPrintStatus;                        /* store print status */
        pItemDisc->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItemDisc);                                      /* print validation (VOID function) */
        pItemDisc->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);        /* restore print status */
    }
 
    /*----- CANCEL TAX EXEMPT KEY -----*/
    ItemCommonCancelExempt();

    /*--- DISPLAY DISCOUNT MESSAGE ---*/
    ItemDiscDisplay(pItemDisc);

    /* --- Set Discount/Surcharge Bit in TranQual,  V3.3 --- */
    ItemCommonDiscStatus(pItemDisc, 0);

    /*--- MANIPURATE MINOR CLASS FOR ITEM DISC. 1 ---*/
    if (pItemDisc->uchMinorClass == CLASS_ITEMDISC1) {
        switch(pCommonLocalRcvBuff->ItemSales.uchMinorClass) {
        case CLASS_DEPT:
            pCommonLocalRcvBuff->ItemSales.uchMinorClass = CLASS_DEPTITEMDISC;
            break;

        case CLASS_PLU:
            pCommonLocalRcvBuff->ItemSales.uchMinorClass = CLASS_PLUITEMDISC;
            break;

        case CLASS_SETMENU:
            pCommonLocalRcvBuff->ItemSales.uchMinorClass = CLASS_SETITEMDISC;
            break;
    
        case CLASS_OEPPLU:                  /* R3.0 */ 
            pCommonLocalRcvBuff->ItemSales.uchMinorClass = CLASS_OEPITEMDISC;
            break;
    
        default:
            break;
        }
    }
    
    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID    ItemDiscDisplay()
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
VOID    ItemDiscDisplay(ITEMDISC *pItemDisc)
{
    UCHAR           uchSystem = 0;
	REGDISPMSG      DisplayData = { 0 };
	PARATRANSMNEMO  TransMnemoRcvBuff = { 0 };

    DisplayData.uchMajorClass = CLASS_UIFREGDISC;
    DisplayData.uchMinorClass = pItemDisc->uchMinorClass;
    DisplayData.lAmount = pItemDisc->lAmount;
    DisplayData.uchFsc = pItemDisc->uchSeatNo;  /* R3.1 */
 
	ItemDiscCommonTranMnemonic (pItemDisc->uchMinorClass, pItemDisc->uchDiscountNo, &TransMnemoRcvBuff);

    _tcsncpy(DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);
    _tcsncpy(pItemDisc->aszMnemonic, TransMnemoRcvBuff.aszMnemonics, NUM_DEPTPLU);

    flDispRegDescrControl |= ITEMDISC_CNTRL;

    if (pItemDisc->fbDiscModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;       /* turn on descriptor control flag for condiment */
    } 

    if (pItemDisc->fbDiscModifier & TAX_MOD_MASK) {    /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL;     /* turn on descriptor control flag for condiment */
    }

    /* check system */
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                  /* V3.3 */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
            uchSystem = USNOTCASHIER;
        }
    }
    
    uchDispRepeatCo = 1;                   /* clear repeat counter */

    if (uchSystem == USNOTCASHIER) {
        flDispRegKeepControl &=~ (VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL  | TOTAL_CNTRL | SUBTTL_CNTRL | FOODSTAMP_CTL);
    } else {
        flDispRegKeepControl &=~ (VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL  | TAXEXMPT_CNTRL | TOTAL_CNTRL |  SUBTTL_CNTRL | FOODSTAMP_CTL);
    }
                              
    DispWrite(&DisplayData);                /* display the above data */
    
    /*----- TURN OFF DESCRIPTOR CONTROL FLAG -----*/
    if (uchSystem == USNOTCASHIER) {
        flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL | FOODSTAMP_CTL);
    } else {
        flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL |  SUBTTL_CNTRL | FOODSTAMP_CTL);
    }
}

/****** End of Source ******/
