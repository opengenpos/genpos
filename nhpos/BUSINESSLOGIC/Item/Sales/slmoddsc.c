/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : I/F function with Item Common Module                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLMODDSC.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesModDiscCheck()
*           ItemSalesModDiscHALO()
*           ItemSalesModDisc()
*           ItemSalesModDiscCal()
*           ItemSalesModDiscModifier()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Dec- 7-92: 01.00.00  : K.Maeda   : 0 price check for dept mod. disc.                                   
* Aug-23-93: 01.00.12  : K.You     : bug fixed. (mod. ItemSalesModDiscModifier)                        
* Oct-19-93: 02.00.02  : K.You     : del. canada tax feature.   
* Feb-01-96: 03.01.00  : M.Ozawa   : recover canada tax feature. 
* Aug-19-98: 03.03.00  : hrkato    : V3.3 (VAT/Service)
*
** NCR2171 **
* Sep-06-99: 01.00.00  : M.Teraki  :initial (for 2171)
** GenPOS 2.2.1 **
* Jun-22-16: 02.02.01  : R.Chambers : Removed ItemSalesModDiscGetTaxMDC(), moved code to ItemSalesModDiscModifier()
* Jun-23-16: 02.02.01  : R.Chambers : Canadian tax in ItemSalesModDiscModifier() used wrong index for ControlCode.auchPluStatus[]
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
#include    <csstbpar.h>
#include    <display.h>
#include    <mld.h>
#include    "itmlocal.h"

static SHORT    ItemSalesModDiscCal(ITEMSALES *pItemSales, DCURRENCY lItemizer);

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesModDiscCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, UCHAR *uchRate)
*
*    Input: UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS     : Function Performed succesfully
*         LDT_SEQERR_ADR  : Function aborted with Illegal operation sequense error
*         LDT_PROHBT_ADR  : Prohibitted operation
*         LDT_KEYOVER_ADR : Illegal data entry error
*
** Description: Check operation sequence for modified discount. 
*                
*===========================================================================
*/

SHORT   ItemSalesModDiscCheck(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT  sReturnStatus;
    
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {   /* error iff minus item */
        return(LDT_PROHBT_ADR);
    }

    if (pItemSales->fbModifier & VOID_MODIFIER) {
        return(LDT_SEQERR_ADR);
    }
 
    /*--- CHECK UINIT PRICE OF DEPT MOD DISC ---*/
    if (pItemSales->uchMinorClass == CLASS_DEPTMODDISC) {
        if (pItemSales->lUnitPrice == 0) {
            return(LDT_PROHBT_ADR);
        }
    }

    if (pUifRegSales->lDiscountAmount == 0) {       /* disc. amount didn't enter */
		PARADISCTBL        DiscRateRcvBuff = {0};

        /*---  GET DISCOUNT RATE ---*/
        DiscRateRcvBuff.uchMajorClass = CLASS_PARADISCTBL;   /* item disc. 1 address */                               
        DiscRateRcvBuff.uchAddress = RATE_MODID_ADR;         /* Mod. disc. address */                               
        CliParaRead(&DiscRateRcvBuff);          /* get rate */
        
        if ((pItemSales->uchRate = DiscRateRcvBuff.uchDiscRate) == 0) {
            return(LDT_PROHBT_ADR);
        }
    } else {       
		UCHAR  uchMDC = CliParaMDCCheck(MDC_MODID21_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT1)); /* adddress 35 */
        
        if (!(uchMDC & DISC_ALLOW_OVERRIDE)) {  /* ! allow overriding */ 
            return(LDT_PROHBT_ADR);
        }
        if (uchMDC & DISC_RATE_OVERRIDE) {
            if (pUifRegSales->lDiscountAmount > 100L) {
                return(LDT_KEYOVER_ADR);
            }
            pItemSales->uchRate = (UCHAR) (pUifRegSales->lDiscountAmount);   /* set overrided discount rate */
            return(ITM_SUCCESS);
        }
     
        /*--- CHECK OVERRIDDEN AMOUNT FOR DEPT MOD DISC ---*/
        if (pItemSales->uchMinorClass == CLASS_DEPTMODDISC) {
            if (pUifRegSales->lDiscountAmount > pItemSales->lUnitPrice) {
                return(LDT_KEYOVER_ADR);
            }
        }

        /*--- HALO CHECK FOR AMOUNT OVERRIDE ---*/
        if ((sReturnStatus = ItemSalesModDiscHALO(pUifRegSales)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }

        /*--- HALO CHECK FOR VOID HALO AT AMOUNT OVERRIDE ---*/
        if (pUifRegSales->fbModifier & VOID_MODIFIER) {
            if ((sReturnStatus = ItemSalesComnIFVoidHALO(pUifRegSales->lDiscountAmount)) != ITM_SUCCESS) {   /* disable E.C */
                return(sReturnStatus);
            }
        }

        pItemSales->lDiscountAmount = pUifRegSales->lDiscountAmount;   /* set overrided discount amount */
    }
    
    return(ITM_SUCCESS);
}    
    
    
/*
*===========================================================================
**Synopsis: SHORT   ItemDiscModDiscHALO(UIFREGSALES *pUifRegSales)
*
*    Input: UIFREGSALES *pUifRegSales
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*
** Description: Executes HALO check for discount amount overridden.
*                
*===========================================================================
*/

SHORT   ItemSalesModDiscHALO(CONST UIFREGSALES *pUifRegSales)
{
    PARATRANSHALO      HaloDataRcvBuff;

    HaloDataRcvBuff.uchMajorClass = CLASS_PARATRANSHALO;
    HaloDataRcvBuff.uchAddress = HALO_MODIFID_ADR;
                
    CliParaRead(&HaloDataRcvBuff);          /* get halo amount */    
    
    if (RflHALO(pUifRegSales->lDiscountAmount, HaloDataRcvBuff.uchHALO) != RFL_SUCCESS) {
        return(LDT_KEYOVER_ADR);
    }
    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesModDisc(ITEMCOMMONLOCAL *pItemComnLocal, 
*                                    ITEMSALES *pItemSales)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: 
*         
*
** Description: Modified discount process. 
*                
*===========================================================================
*/
SHORT   ItemSalesModDisc(ITEMCOMMONLOCAL *pItemComnLocal, ITEMSALES *pItemSales)
{
	DCURRENCY   lItemizer;
	LONG        lSign = 1L;
	LONG        lDiscSign = 1L;
	SHORT       sReturnStatus;
	SHORT       sCompSize;
	ITEMSALES   PrevItemWork = pItemComnLocal->ItemSales;

    if (PrevItemWork.fbModifier & VOID_MODIFIER) {
        lSign *= -1L;
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {       /* check preselect void status */
        lSign *= -1L;
    }

    /*--- GET DISC/ SURCHARGE OPTION ---*/
    if (! CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0)) {   /* discount */
        lDiscSign *= -1L;                   /* sign for discount */  
    }

    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        pItemComnLocal->ItemSales.lDiscountAmount *= lSign;       /* void, p-void sign */
        pItemComnLocal->ItemSales.lDiscountAmount *= lDiscSign;   /* apply disc's sign */  
    }

    PrevItemWork.lQTY *= lSign;             /* reflect the above condition */
    PrevItemWork.lProduct *= lSign;        

    lItemizer = 0;                          /* init buffer */
    sReturnStatus = ItemSalesSalesAmtCalc(&PrevItemWork, &lItemizer);
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*--- CHECK ITEMIZER ---*/
    if (lItemizer < 0) {
        return(LDT_PROHBT_ADR);
    }

    if (pItemComnLocal->ItemSales.uchRate != 0) {     
        sReturnStatus = ItemSalesModDiscCal(&(pItemComnLocal->ItemSales), lItemizer);
    }                                        /* put calculated value */    
    
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    pItemComnLocal->ItemSales.lDiscountAmount *= lSign;       /* void, p-void sign */ 
    pItemComnLocal->ItemSales.lDiscountAmount *= lDiscSign;   /* discount sign */ 

    if (!(pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
        
        /*--- TAX MOD PROCESS ---*/
        ItemSalesModDiscModifier(&(pItemComnLocal->ItemSales));
    
        /*--- CANCEL PLU/ DEPT STATUS ---*/
        ItemSalesModDiscCancelStatus(&(pItemComnLocal->ItemSales));

        /*----- SET PRINT STATUS -----*/
        pItemComnLocal->ItemSales.fsPrintStatus |= (PRT_SLIP | PRT_JOURNAL); 

        /*--- SET PRT_VALIDATION STATUS ---*/
        ItemSalesModDiscValidation(&(pItemComnLocal->ItemSales));

        /*--- SET AFFECTION STATUS ---*/
        ItemSalesModDiscAffection(&(pItemComnLocal->ItemSales));
    }

    pItemSales->ControlCode.auchPluStatus[0] &= ~PLU_PRT_VAL;      /* turn off current validation status */
    /*--- DON'T CONSOLIDATE MODIFIED DISCOUNT ---*/
//    pItemComnLocal->ItemSales.fbStorageStatus |= NOT_ITEM_CONS;  

    /*----- CHECK STRAGE SIZE -----*/
    if ((sCompSize = ItemCommonCheckSize(&(pItemComnLocal->ItemSales), 0)) < 0) {
        return(LDT_TAKETL_ADR);
    }

    pItemComnLocal->usSalesBuffSize = sCompSize;    /* update buffered size */
    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        if ((sReturnStatus = ItemSalesSpvIntForCond(pItemSales)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    } else {
        if ((sReturnStatus = ItemSalesSpvIntForModDisc(pItemComnLocal)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesModDiscCancelStatus(ITEMSALES *pItemSales)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: 
*         
*
** Description:  
*                
*===========================================================================
*/
VOID   ItemSalesModDiscCancelStatus(ITEMSALES *pItemSales)
{

    /*----- CANCEL PRINT STATUS -----*/
    pItemSales->fsPrintStatus &= ~(PRT_SLIP | PRT_JOURNAL | PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT | PRT_SPCL_PRINT);

    pItemSales->fsPrintStatus &= ~(pItemSales->ControlCode.auchPluStatus[2] & ITM_KTCHN_SEND_MASK);

    /*----- CANCEL AFFECTION STATUS -----*/
    pItemSales->ControlCode.auchPluStatus[5] &= ~(ITM_HASH_FINANC | ITM_SALES_HOURLY | ITM_MOD_DISC_SUR | ITM_MOD_AFFECT_DEPT_PLU | ITM_AFFECT_DEV_BY_10);

}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesModDiscCal(ITEMSALES *pItemSales, DCURRENCY lItemizer)
*
*    Input: ITEMSALES *pItemSales, DCURRENCY lItemizer
*
*   Output: None
*
*    InOut: 
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function caluculates product and round the value. 
*                
*===========================================================================
*/

static SHORT    ItemSalesModDiscCal(ITEMSALES *pItemSales, DCURRENCY lItemizer)
{
    /*--- CALCULATE AND ROUNDING DISCOUNT AMOUNT ---*/
    if (pItemSales->uchRate != 0) {
		SHORT  sReturnStatus = RflRateCalc1(&(pItemSales->lDiscountAmount), lItemizer, pItemSales->uchRate * RFL_DISC_RATE_MUL, RND_ITEMDISC2_ADR);

        if (sReturnStatus != RFL_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID    ItemSalesModDiscModifier(ITEMSALES *pItemSales)
*
*    Input: ITEMSALES *pItemSales
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

VOID   ItemSalesModDiscModifier(ITEMSALES *pItemSales)
{
                                            /* get disc.'s justified taxable MDC */
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemDiscGetCanTaxMod(pItemSales);   /* get canadian tax mod */
        if ((pItemSales->fbModifier & TAX_MOD_MASK) != 0) {
			USHORT     fbModWork = ((pItemSales->fbModifier & TAX_MOD_MASK) >> 1);  // right justify to eliminate void flag.
            
            /* pItemSales->ControlCode.auchPluStatus[1] &= ~PLU_DISC_TAX_MASK;
            pItemSales->ControlCode.auchPluStatus[1] |= (UCHAR)(fbModWork - 1); 8/23/93 */  
            // pItemSales->ControlCode.auchPluStatus[2] |= ((fbModWork - 1) << 4);  06/22/2016 found error during code review, R.Chambers

			// see functions ItemCanTaxSpecBakedCorr() and PrtGetSISymSalesCanada() for use of this data - 06/22/2016, R.Chambers
			// Questions: why is one subtracted and then in the various places used, one is added back? Why is this data duplicated?  R.Chambers
            pItemSales->ControlCode.auchPluStatus[1] &= ~TRN_MASKSTATCAN;    // clear Canadian tax index area.
            pItemSales->ControlCode.auchPluStatus[1] |= (UCHAR)(fbModWork - 1);
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
		UCHAR      fbNounTax;
		UCHAR      fbDiscTax;
		USHORT     fbTaxMDC;
		PARAMDC    MDC;

        pItemSales->fbModifier |= (ItemModLocalPtr->fsTaxable << 1);    /* put modifier data, shift to skip over void indicator */
        /*--- NOUN's TAX STATUS ---*/
        fbNounTax = (UCHAR)(pItemSales->ControlCode.auchPluStatus[1] & PLU_DISC_TAX_MASK);

            /*------------------------------------------------------------
            |         Tax staus data                     |  Bit positon   |
            |============================================+================|
            |   ItemModLocalPtr->fsTaxable (modifier)    |    .....XXX    |
            |--------------------------------------------+----------------|
			|   Discount's MDC  (35 and 36)              |    ...XXX..    |
			|--------------------------------------------+----------------|
            |   pItemSales->fbModifier                   |    ....XXX.    |
            |--------------------------------------------+----------------|
            |   pItemSales->auchStatus[1]                |    .....XXX    |
             ------------------------------------------------------------*/
        
        /*--- DISCOUNT's TAX STATUS ---*/
		//    Question: Why is it Item Discount #2 MDC data that is being used?
		//              R.Chambers 6/22/2016
		MDC.uchMajorClass = CLASS_PARAMDC;       /* get MDC add. 35 to 36 */
		MDC.usAddress = MDC_MODID21_ADR;
		CliParaRead(&MDC);
		fbTaxMDC = (USHORT) ((MDC.uchMDCData & DISC_MDC_TAX_MASK) >> 2);
 
		fbDiscTax = (UCHAR) (((pItemSales->fbModifier & TAX_MOD_MASK) >> 1) ^ fbTaxMDC);    /* modify disc's MDC */
        pItemSales->ControlCode.auchPluStatus[1] &= ~PLU_DISC_TAX_MASK;
        pItemSales->ControlCode.auchPluStatus[1] |= (fbNounTax & fbDiscTax);

        MDC.uchMajorClass   = CLASS_PARAMDC;            /* Saratoga */
        MDC.usAddress       = MDC_MODID23_ADR;
        CliParaRead(&MDC);
        if (((MDC.uchMDCData & ODD_MDC_BIT2) && !(pItemSales->fbModifier & FOOD_MODIFIER)) ||
            (!(MDC.uchMDCData & ODD_MDC_BIT2) && (pItemSales->fbModifier & FOOD_MODIFIER))) {
            ;
        } else {
            pItemSales->ControlCode.auchPluStatus[1] &= ~PLU_AFFECT_FS;
        }

    } else {    /* V3.3 */
        switch(pItemSales->ControlCode.auchPluStatus[1] & 0x03) {
        case    INTL_VAT1:
            if (!CliParaMDCCheck(MDC_MODID21_ADR, ODD_MDC_BIT2)) {
                pItemSales->ControlCode.auchPluStatus[1] &= ~(INTL_VAT1|INTL_VAT2|INTL_VAT3);
            }
            break;

        case    INTL_VAT2:
            if (!CliParaMDCCheck(MDC_MODID21_ADR, ODD_MDC_BIT3)) {
                pItemSales->ControlCode.auchPluStatus[1] &= ~(INTL_VAT1|INTL_VAT2|INTL_VAT3);
            }
            break;

        case    INTL_VAT3:
            if (!CliParaMDCCheck(MDC_MODID21_ADR, EVEN_MDC_BIT0)) {
                pItemSales->ControlCode.auchPluStatus[1] &= ~(INTL_VAT1|INTL_VAT2|INTL_VAT3);
            }
            break;

        default:
            break;
        }
    }
}

/*
*===========================================================================
**Synopsis: VOID    ItemSalesModDiscValidation(ITEMSALES *pItemSales)
*
*    Input: ITEMSALES *pItemSales
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
VOID    ItemSalesModDiscValidation(ITEMSALES *pItemSales)
{
    if ((pItemSales->fbModifier & VOID_MODIFIER) &&       /* void key depressed */
         CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1)) {   /* comp validation for void */
         pItemSales->fsPrintStatus |= PRT_VALIDATION;
    } else if (CliParaMDCCheck(MDC_MODID22_ADR, EVEN_MDC_BIT3)) {   /* validation print option */
         pItemSales->fsPrintStatus |= PRT_VALIDATION;
    }
}


/*
*===========================================================================
**Synopsis: VOID    ItemSalesModDiscAffection(ITEMSALES *pItemSales)
*
*    Input: ITEMSALES *pItemSales
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
VOID    ItemSalesModDiscAffection(ITEMSALES *pItemSales) 
{
	UCHAR  uchMDCSave;

    /*----- HOURLY TOTAL AFFECTION STATUS -----*/
    if (!CliParaMDCCheck(MDC_MODID22_ADR, EVEN_MDC_BIT1)) {              /* affect hourly */
        pItemSales->ControlCode.auchPluStatus[5] |= ITM_SALES_HOURLY;    /* turn on affect status */
    }

    uchMDCSave = CliParaMDCCheck(MDC_MODID23_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT1)); 

    if (uchMDCSave & ODD_MDC_BIT0) {        /* surcharge option */
        pItemSales->ControlCode.auchPluStatus[5] |= ITM_MOD_DISC_SUR; 
    }

    if (uchMDCSave & ODD_MDC_BIT1) {
        pItemSales->ControlCode.auchPluStatus[5] |= ITM_MOD_AFFECT_DEPT_PLU;  /* affect discount amount to dept, PLU */
    }
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesSpvIntForModDisc(ITEMSALES *pItemSales)
*
*    Input: 
*
*   Output: 
*
*    InOut: 
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         
*
** Description:  
*                
*===========================================================================
*/
SHORT   ItemSalesSpvIntForModDisc(ITEMCOMMONLOCAL *pItemComnLocal)
{
	SHORT  sRequireSpvInt = 0;

    if (pItemComnLocal->ItemSales.ControlCode.auchPluStatus[0] & PLU_REQ_SUPINT) {
        sRequireSpvInt = 1;  
    } else if (CliParaMDCCheck(MDC_MODID22_ADR, EVEN_MDC_BIT2)) {   /* check Supervisor Intervention MDC */
        sRequireSpvInt = 1;
    } else if ((pItemComnLocal->ItemSales.fbModifier & VOID_MODIFIER) && CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0)) { 
        sRequireSpvInt = 1;
    }

    if (sRequireSpvInt) {  
		SHORT  sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR);

        if (sReturnStatus != UIF_SUCCESS) {
            return(sReturnStatus);
        }

        pItemComnLocal->fbCommonStatus |= COMMON_SPVINT;     /* set already done flag */
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCursorModDisc(UIFREGSALES *pUifRegSales)
*
*    InOut: UIFREGSALES *pUifRegSales
*
**Return:  CLASS_UIDEPTREPEAT   dept sales repeat
*          CLASS_UIPLUREPEAT    plu sales repeat
*          CLASS_UICOMCOUPONREPEAT coupn repeat
*          -1                   not repeatable
*
** Description:  Cursor Modifier Discount, V3.3

*===========================================================================
*/
SHORT   ItemSalesCursorModDisc(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    if (MldQueryMoveCursor(MLD_SCROLL_1) == TRUE) {
		ITEMDISC    ItemDisc;

        /* read cursor position item */
        if (MldGetCursorDisplay(MLD_SCROLL_1, pItemSales, &ItemDisc, MLD_GET_SALES_DISC) != MLD_SUCCESS) {
            return(LDT_CANCEL_ADR);
        }

        if (pItemSales->uchMajorClass == CLASS_ITEMSALES) {
            switch(pItemSales->uchMinorClass) {
            case CLASS_DEPT:                    /* dept */
            case CLASS_PLU:                     /* PLU */
            case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
            case CLASS_SETMENU:                 /* setmenu */
                return(ItemSalesCursorModDiscIn(pUifRegSales, pItemSales));
                break;
            }
        }

        return(LDT_PROHBT_ADR);
    } else {
        return(LDT_CANCEL_ADR);
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCursorModDiscIn(ITEMDISC *pData);
*
*    Input: ITEMDISC *pData
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  Cursor Modifier Discount, V3.3
*===========================================================================
*/
SHORT   ItemSalesCursorModDiscIn(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT   sStatus;
    SHORT   sCompSize;
	ITEMCOMMONLOCAL    * CONST pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    if ((sStatus = ItemCommonCheckComp()) != ITM_SUCCESS ) {
        return (sStatus);
    }

    /* replace minor class to modifiered item discount */
    if (pItemSales->uchMinorClass == CLASS_DEPT) {
        pItemSales->uchMinorClass = CLASS_DEPTMODDISC;
    } else
    if (pItemSales->uchMinorClass == CLASS_PLU) {
        pItemSales->uchMinorClass = CLASS_PLUMODDISC;
    } else
    if (pItemSales->uchMinorClass == CLASS_SETMENU) {
        pItemSales->uchMinorClass = CLASS_SETMODDISC;
    }

    /*----- SEQ CHECK SUB-ROUTINE -----*/
    sStatus = ItemSalesModDiscCheck(pUifRegSales, pItemSales);
    if (sStatus != ITM_SUCCESS) {
        return(sStatus);
    }

    if ( !(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)) {
        if (!(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
            /* allow cursor repeat after tray total */
            return(LDT_SEQERR_ADR);
        }
    }

    /***
    if (pUifRegSales->fbModifier & VOID_MODIFIER) {
        return(LDT_SEQERR_ADR);
    }***/

    /*--- ERROR TAX MOD. OPERATION ---*/
    if (ItemModLocalPtr->fsTaxable != 0) {    /* tax mod. operation was done */
        return(LDT_SEQERR_ADR);                 /* return sequence error */
    }
    /*--- ERROR TARE KEY (AUTO SCALE) OPERATION ---*/
    if (ItemModLocalPtr->ScaleData.fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
        return(LDT_TARENONSCALABLE_ADR);             /* tare key for non scalable item */
    }

    /***
    if (pItemSales->ControlCode.auchStatus[0] & PLU_MINUS ||
        pItemSales->ControlCode.auchStatus[2] & PLU_SCALABLE) { 
        return(LDT_PROHBT_ADR);
    }
    
    if (pItemSales->fbModifier & VOID_MODIFIER) {  / voided item /
        return(LDT_PROHBT_ADR);
    }***/

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

    /*----- CHECK STRAGE SIZE -----*/
    if ((sCompSize = ItemCommonCheckSize(&(pCommonLocalRcvBuff->ItemSales), 0)) < 0) {
		return (LDT_TAKETL_ADR);
    }

    if ((sStatus = ItemPreviousItem(pItemSales, sCompSize)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*--- MODIFIED DISC PROCESS ---*/
    sStatus = ItemSalesModDisc(pCommonLocalRcvBuff, pItemSales);
    if (sStatus != ITM_SUCCESS) {
        ItemSalesCancelPrev();          /* cancel previous item */
        return(sStatus);
    }
    pItemSales->lDiscountAmount = pCommonLocalRcvBuff->ItemSales.lDiscountAmount;

    if ( !(ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE) && (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY) ) {
        /* print out header at tray transaction */
        ItemCurPrintStatus();                               /* set print status of current qualifier */
        ItemPromotion(PRT_RECEIPT, TYPE_STORAGE);   
        ItemHeader(TYPE_STORAGE);                           /* send header print data */
                                                            /* send transaction open data */
        ItemSalesLocal.fbSalesStatus |= SALES_ITEMIZE; 
    }

    uchDispRepeatCo = 0;                    /* prohibit condiment entry */

	{
		REGDISPMSG  DisplayData = {0};

		DisplayData.lAmount = pItemSales->lDiscountAmount;
		ItemSalesDisplay(pUifRegSales, pItemSales, &DisplayData);
	}

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(pItemSales, 0);
    if (sStatus != ITM_SUCCESS) {
        return sStatus;
    }
    
    MldScrollWrite(pItemSales, MLD_NEW_ITEMIZE);
    ItemCommonDispSubTotal(pItemSales);

    if (pItemSales->fsPrintStatus & PRT_VALIDATION) {
		USHORT  usBuffer = pItemSales->fsPrintStatus;     /* save print status */

        pItemSales->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItemSales);             /* print validation (VOID function) */
    
        pItemSales->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);
    }

    return(ITM_SUCCESS);
}

/**** End of File ***/
