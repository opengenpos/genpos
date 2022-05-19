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
* Title       : Print modifier module                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLPRNMOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemSalesPrintMod()    
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun- 2-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 4-92: 00.00.01  :   "       : Apply coments of inspection held on 
*                      :           : Jun- 6
* Dec- 4-92: 01.00.00  :   "       : Add scale feature for GCA. 
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
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
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
#include    "rfl.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPrintMod(UIFREGSALES *pUifRegSales)
*
*    Input: UIFREGSALES *pUifRegSales
*   Output: Nothing
*    InOut: Nothing
**Return: ITM_SUCCESS     : Function performed successfully
*         LDT_SEQERR_ADR  : Operation sequence error
*         LDT_KEYOVER_ADR : Total of Print Mod. and condiment exeeds 10
*         LDT_TAKETL_ADR  : Storage buffer full
*
** Description: This module executes the following functions. 
*               - Check print modifier data and send it to common module.
*               - Turn off compulsory print mod. status of item sales local data. 
*===========================================================================
*/
SHORT   ItemSalesPrintMod(UIFREGSALES *pUifRegSales)
{
    CHAR   chWork;
    SHORT  sReturnStatus;
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
    REGDISPMSG         DisplayData;
    
    if (!(pCommonLocalRcvBuff->ItemSales.uchMajorClass == CLASS_ITEMSALES &&
         (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLU ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPT ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPTMODDISC ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMENU ||
          pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC))) {
		// we can only do Print Modifier on the current, not yet stored, sales item.
		// if what is in the memory resident current transaction item storage is not
		// an ITEMSALES then it is a sequence error.
        return(LDT_SEQERR_ADR);
    }
    
    if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) { /* error minus dept and minus PLU */
        return(LDT_PROHBT_ADR); /* error print mod. for coupon PLU */
    }

    if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {
        return(LDT_PROHBT_ADR); /* error print mod. for hash PLU */
    }

    if (uchDispRepeatCo != 1) { 
        return(LDT_SEQERR_ADR); /* error if noun is repeated */
    }
    
    /*----- ERROR TAX MODIFY OR TAX EXEMPT OPERATION FOR PRINT MOD. -----*/
    if (ItemModLocalPtr->fsTaxable != 0) {   /* tax modified */
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) { /* Saratoga */
        return(LDT_SEQERR_ADR);
    }

    /* get number of print mod and condiment */    
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

    if ((sReturnStatus = ItemPreviousItem(pUifRegSales, 0)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }
    
    /* send to enhanced kds, 2172 */
    sReturnStatus = ItemSendKds(pUifRegSales, 0);
    if (sReturnStatus != ITM_SUCCESS) {
        return sReturnStatus;
    }
    
    /*----- Sales to LCD,   R3.0 -----*/
    MldScrollWrite(&ItemCommonLocal.ItemSales, MLD_UPDATE_ITEMIZE);

    /* turn off compulsory bit */
    ItemSalesLocal.fbSalesStatus &= ~(SALES_PM_COMPLSRY | SALES_COND_COMPLSRY);

    /*----- DISPLAY PRINT MOD. MNEMONICS -----*/
    memset(&DisplayData, '\0', sizeof(REGDISPMSG));   /* initialize data structure */
    /* get display amount */
    if ((pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPTMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC)) {
        
        DisplayData.lAmount = pCommonLocalRcvBuff->ItemSales.lDiscountAmount;

    } else {
        sReturnStatus = ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &DisplayData.lAmount);
        if (sReturnStatus != ITM_SUCCESS) { 
            return(sReturnStatus);
        }
    }
    
    if (pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {    
        /*--- DISPLAY DECIMAL DATA ---*/
        DisplayData.uchMajorClass = CLASS_REGDISP_SALES;
    
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {
                                            /* address 29, bit 0 */
            DisplayData.uchMinorClass = CLASS_REGDISP_SCALE3DIGIT;
        } else {
            DisplayData.uchMinorClass = CLASS_REGDISP_SCALE2DIGIT;
        }

        RflGetSpecMnem (DisplayData.aszSPMnemonic, SPC_LB_ADR);
        DisplayData.lQTY = pCommonLocalRcvBuff->ItemSales.lQTY;

    } else {                                /* non scalable */
        DisplayData.uchMajorClass = pUifRegSales->uchMajorClass;
        DisplayData.uchMinorClass = pUifRegSales->uchMinorClass;
    
        if (labs(pCommonLocalRcvBuff->ItemSales.lQTY) == 1000L) {
            DisplayData.lQTY = 0L;          /* don't display QTY */
        } else {
            DisplayData.lQTY = pCommonLocalRcvBuff->ItemSales.lQTY;
        }
    }

    /* get print modifier mnemonisc */
    RflGetMnemonicByClass (CLASS_PARAPRTMODI, DisplayData.aszMnemonic, pUifRegSales->uchPrintModifier);
 
    uchDispRepeatCo = 1;                    /* clear repeat counter */

    DisplayData.uchFsc = pCommonLocalRcvBuff->ItemSales.uchSeatNo;  /* display seat no. */

    /* set descriptor control flags */
    if (pCommonLocalRcvBuff->ItemSales.fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;   /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= VOID_CNTRL;    /* turn on descriptor control flag for condiment */
    } 

    if (pCommonLocalRcvBuff->ItemSales.fbModifier & TAX_MOD_MASK) {
                                            /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL;  /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= TAXMOD_CNTRL;   /* turn on descriptor control flag for condiment */
    } 
    if (pCommonLocalRcvBuff->ItemSales.fbModifier & FOOD_MODIFIER) {    /* Saratoga */
        flDispRegDescrControl |= FOODSTAMP_CTL;
        flDispRegKeepControl |= FOODSTAMP_CTL;
    }
    
    if ((pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_PLUMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPTMODDISC) ||
        (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_SETMODDISC)) {
     
        flDispRegDescrControl |= ITEMDISC_CNTRL;   /* for 10N10D display */
        flDispRegKeepControl |= ITEMDISC_CNTRL;    /* for 10N10D display */
    }

    /* turn on descriptor control flag */
 //   flDispRegDescrControl |= PM_CNTRL;      /* for 10N10D display */
//    flDispRegKeepControl &= ~PM_CNTRL;      /* don't keep, for 10N10D display */
    
    if (pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPT ||
        pCommonLocalRcvBuff->ItemSales.uchMinorClass == CLASS_DEPTMODDISC) {

        flDispRegKeepControl &= ~(VOID_CNTRL | TAXMOD_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL);
    }

    DispWrite(&DisplayData);

    /* turn off descriptor control flags */
    flDispRegDescrControl &= ~(VOID_CNTRL | TAXMOD_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL); 

    flDispRegKeepControl &= ~ (VOID_CNTRL | TAXMOD_CNTRL | ITEMDISC_CNTRL | FOODSTAMP_CTL);

    return(ITM_SUCCESS);
}

/****** end of file ******/
