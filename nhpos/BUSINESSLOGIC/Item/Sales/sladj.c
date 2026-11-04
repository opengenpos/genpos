/*
*===========================================================================
* Title       : Adjective PLU process                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLADJ.C
* --------------------------------------------------------------------------
* Abstruct: ItemSalesAdj()   
*           ItemSalesGenerateAdj()
*           ItemSalesAdjValidation()
*           ItemSalesBreakDownAdj()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-30-92: 00.00.01  : K.Maeda   : Initial                                   
* Jun- 4-92: 00.00.01  : K.Maeda   : 2nd code inspection held on Jun. 3rd                                     
* Jun- 8-92: 00.00.01  : K.Maeda   : 3rd code inspection held on Jun. 6th                                     
* Jun- 9-92: 00.00.01  : K.Maeda   : 4th code inspection held on Jun. 9th                                     
*          :           :           :                         
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

#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
#include    <log.h>
#include    <uie.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <appllog.h>
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesAdj(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*           *pItemSales
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function executs following process.
*===========================================================================
*/
SLDTITM  ItemSalesAdj(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    UCHAR  auchAdjData[2] = { 0 };
	SHORT  sReturnStatus;

    pItemSales->uchAdjective = pUifRegSales->uchAdjective;

    /*----- CHECK 0 AMOUNT ENTRY -----*/
    if (ItemSalesLocalPtr->fbSalesStatus & SALES_ZERO_AMT) { /* 0 amount entry for non open PLU */
        return(LDT_SEQERR_ADR);
    }

    /*----- CHECK ADJ. VALIATION ENTRY -----*/
    if (pItemSales->uchAdjective == 0) {    /* adjective valiation wasn't entered */
        if ((pItemSales->ControlCode.auchPluStatus[4] & PLU_IMP_ADJKEY_MASK) == PLU_COMP_ADJKEY) {
            if (ItemSalesLocalPtr->uchAdjCurShift == 0) {
                return (LDT_SEQERR_ADR);   // RJC: 06-09-2025 was however that error code doesn't seem appropriate (ITM_COMP_ADJ);
            }
        }
        /* get adj. */
        
        if (!CliParaMDCCheck(MDC_ADJ_ADR, EVEN_MDC_BIT0)) {
            /* read menu, key default adjective, at first */
            /* use menu adjective for all PLU */
            if (ItemSalesLocalPtr->uchAdjCurShift) {    /* if default is set by menu, kdy */
                pItemSales->uchAdjective = ItemSalesLocalPtr->uchAdjCurShift;
                ItemSalesGenerateAdj(pItemSales, auchAdjData);      /* generated extended FSC code are */
                                                                    /* saved into uchAdjective         */   
                if (auchAdjData[0] == 0) { /* not hit group */
                    return(LDT_PROHBT_ADR);
                }
                
                /* generate adjective key code */
                pItemSales->uchAdjective = (auchAdjData[0]-1);
                pItemSales->uchAdjective *= ADJ_VAL_MAX;
                pItemSales->uchAdjective += ItemSalesLocalPtr->uchAdjCurShift;
                
                auchAdjData[1] = ItemSalesLocalPtr->uchAdjCurShift;
            } else {
                ItemSalesGenerateAdj(pItemSales, auchAdjData);      /* generated extended FSC code are */
                                                                    /* saved into uchAdjective         */   
                if (auchAdjData[0] == 0) { /* not hit group */
                    return(LDT_PROHBT_ADR);
                } 
            }
        } else {
            /* read plu default adjective, at first */
            ItemSalesGenerateAdj(pItemSales, auchAdjData);      /* generated extended FSC code are */
                                                            /* saved into uchAdjective         */   
            if (auchAdjData[0] == 0) { /* not hit group */
                return(LDT_PROHBT_ADR);
            }

            if (auchAdjData[1] == 0) { /* no defaut adjective */
                if (ItemSalesLocalPtr->uchAdjCurShift) {    /* if default is set by menu, kdy */
                    /* generate adjective key code */
                    pItemSales->uchAdjective = (auchAdjData[0]-1);
                    pItemSales->uchAdjective *= ADJ_VAL_MAX;
                    pItemSales->uchAdjective += ItemSalesLocalPtr->uchAdjCurShift;
                    
                    auchAdjData[1] = ItemSalesLocalPtr->uchAdjCurShift;
                } else {
                    return(LDT_PROHBT_ADR);
                }
            }
        } 
    } else {
        if ((sReturnStatus = ItemSalesBreakDownAdj(pItemSales, auchAdjData)) != ITM_SUCCESS) {
            return (sReturnStatus);
        }
    }

    if ((pItemSales->fsLabelStatus & ITM_CONTROL_NOTINFILE) == 0) {   /* not in file building, saratoga */
        /*----- get adjective price -----*/
        if (((pUifRegSales->uchAdjective - 1) % ADJ_VAL_MAX + 1) != auchAdjData[1]) {
            PLUIF  PLURecRcvBuffer = { 0 };        /* PLU record receive buffer */

            /* prepair PLU record receive buffer */
            AUCPLUNO(PLURecRcvBuffer.auchPluNo) = AUCPLUNO(pItemSales->auchPLUNo);
    	    PLURecRcvBuffer.uchPluAdj = auchAdjData[1];  /* adjective valiation */

	        if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
    	        return(OpConvertError(sReturnStatus));
        	} 

	        /* set adj. price */
	        RflConv3bto4b(&pItemSales->lUnitPrice, PLURecRcvBuffer.ParaPlu.auchPrice);
	    }
    }

    /* set default adjective code for setmenu & link plu, 2172 */
    if (pUifRegSales->uchAdjective == 0) {
        if (ItemSalesLocalPtr->uchAdjCurShift) {
            pUifRegSales->uchAdjective = pItemSales->uchAdjective;
        }
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesGenerateAdj(ITEMSALES *pItemSales, UCHAR uchAdjWriteBuff);
*
*    Input: *pUifRegSales
*           uchAdjGroup
*
*   Output: None
*
*    InOut: None
*
**Return: 
*         
*
** Description: Generate extended FSC data from Adj. Group and assumed valiation
*                
*===========================================================================
*/
VOID   ItemSalesGenerateAdj(ITEMSALES *pItemSales, UCHAR *auchAdjWriteBuff)
{
	auchAdjWriteBuff[0] = auchAdjWriteBuff[1] = 0;

    /*----- get assumed valiation -----*/
    COMPILE_CHECK(ADJ_GRP_MAX < 8);    // compile error if ADJ_GRP_MAX is larger than number of bits in UCHAR.
    for (UCHAR i = 0; i < ADJ_GRP_MAX; i++) {
        if (pItemSales->ControlCode.auchPluStatus[3] & (PLU_USE_ADJG1 << i)) {
            auchAdjWriteBuff[0] = i + 1;
            break;
        }
    }    
                                                
    auchAdjWriteBuff[1] = (UCHAR) (pItemSales->ControlCode.auchPluStatus[4] & PLU_IMP_ADJKEY_MASK);
    if (auchAdjWriteBuff[1] > ADJ_VAL_MAX) {
        auchAdjWriteBuff[1] = ADJ_VAL_MAX;
    }
        
    /* set FSC data */
    if (auchAdjWriteBuff[0] != 0) {
        pItemSales->uchAdjective = (UCHAR) ((auchAdjWriteBuff[0] - 1) * ADJ_VAL_MAX + auchAdjWriteBuff[1]);
    } else {
        pItemSales->uchAdjective = 0;
    }
}

/*
*===========================================================================
**Synopsis: UCHAR   *ItemSalesBreakDownAdj(ITEMSALES *pItemSales, UCHAR *auchAdjWriteBuff);
*
*    Input: ITEMSALES *pItemSales
*           UCHAR *auchAdjWriteBuff
*
*   Output: None
*
*    InOut: auchBDAdj : auchBDAdj[0] : Adjective Group # 1 ~ 4
*                       auchBDAdj[1] : Adjective valiation 1 ~ 5
*
**Return: ITM_SUCCESS : Function performed successfully.
*         LDT_PROHBT_ADR : Group or Valiation is prohibited to use.
*         
*
** Description: Break down FSC code of adjective to Adj. group and valiation.
*                
*===========================================================================
*/

SLDTITM  ItemSalesBreakDownAdj(CONST ITEMSALES *pItemSales, UCHAR *auchAdjWriteBuff)
{
    auchAdjWriteBuff[0] = (UCHAR) ((pItemSales->uchAdjective - 1) / ADJ_VAL_MAX + 1); /* adj. group */
    auchAdjWriteBuff[1] = (UCHAR) ((pItemSales->uchAdjective - 1) % ADJ_VAL_MAX + 1); /* adj. variation */

    if (!((pItemSales->ControlCode.auchPluStatus[3] & PLU_USE_ADJG_MASK) & (UCHAR) (PLU_USE_ADJG1 << (auchAdjWriteBuff[0] - 1)))) {
        return(LDT_PROHBT_ADR);
    }
    
    if ((pItemSales->ControlCode.auchPluStatus[4] & PLU_PROH_VAL_MASK) & (UCHAR) (PLU_PROHIBIT_VAL1 << (auchAdjWriteBuff[1] - 1))) {
        return(LDT_PROHBT_ADR);
    }
    
    return(ITM_SUCCESS);
}

/****** end of file ******/
