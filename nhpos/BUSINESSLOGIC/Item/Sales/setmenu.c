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
* Title       : Set Menu Process                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SETMENU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Za /Zp /G1s /I                                
* --------------------------------------------------------------------------
* Abstract: ItemSalesSetMenu()
*           ItemSalesSetCheckValidation()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-30-92: 00.00.01  : K.Maeda   : Initial                                   
* Feb-27-95: 03.00.00  : hkato     : R3.0
* Apr-03-96: 03.01.02  : M.Ozawa   : Corrected KP#5 - #8 Status Copy
*
** NCR2171 **
* Sep-06-99: 01.00.00  : M.Teraki  :initial (for 2171)
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
#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "uie.h"
#include    "rfl.h"
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
#include    "itmlocal.h"

#define     PARENT_PLU          0       
#define     CHILD_PLU1          1       
#define     MAX_NO_OF_CHILD     5       

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesSetMenu(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*   Output: None
*    InOut: *pItemSales
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function is for Promotional PLU in which a list of PLUs
*               specified in the AC116 Build Sales Promotion PLU dialog of PEP
*               are added to the sales item in the condiment area of the sales
*               item data. The uchMinorClass is changed to indicate the sales
*               item is now a sales item with additional, promotional PLUs
*               attached.
*
*               Note: the uchMinorClass identifier of the ITEMSALES struct is
*                     modified by this function should there be promotional PLUs
*                     associated with the PLU that is being rung up.
*                        - CLASS_PLU  -> CLASS_SETMENU
*                        - CLASS_PLUMODDISC  -> CLASS_SETMODDISC
*===========================================================================
*/

SHORT   ItemSalesSetMenu(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    USHORT i, j;
    SHORT  sReturnStatus;
    PARAMENUPLUTBL     SetMenuRcvBuff;

    /*----- CHECK PARENT PLU -----*/
    if (pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS ||
        pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE ||
        pItemSales->ControlCode.auchPluStatus[2] & PLU_HASH ||
        pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
 
        return(ITM_CANCEL);
    }
        
    /* Get Set Menu Table */
    SetMenuRcvBuff.uchMajorClass = CLASS_PARAMENUPLUTBL;
    _tcsncpy(SetMenuRcvBuff.SetPLU[0].aszPLUNumber, pItemSales->auchPLUNo, NUM_PLU_LEN);
    SetMenuRcvBuff.uchAddress = 0;
   
    if ((sReturnStatus = CliParaMenuPLURead(&SetMenuRcvBuff)) != ITM_SUCCESS) {
        if (sReturnStatus == LDT_NTINFL_ADR) {   
            return(ITM_CANCEL);
        }
        return(sReturnStatus);
    }
   
    if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
        if (pItemSales->uchAdjective == 0) {    /* ITM_COMP_ADJ error ? */
            if (SetMenuRcvBuff.SetPLU[PARENT_PLU].uchAdjective == 0) {  /* there is no table valiation */
                return(LDT_PROHBT_ADR);         /* no adj. for adj. PLU */
            }
        }
        if ((pUifRegSales->uchAdjective == 0) && (SetMenuRcvBuff.SetPLU[PARENT_PLU].uchAdjective != 0)) { 
			UCHAR  auchAdjData[2] = {0};     /* [0] : adjective group, [1] : adj. valiation */

                              /* no adj. key entry however there is table adj. valiation */
            if ((sReturnStatus = ItemSalesSetCheckValidation(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4], SetMenuRcvBuff.SetPLU[PARENT_PLU].uchAdjective)) != ITM_SUCCESS) {
                              /* validate table adjective */
                return(sReturnStatus);
            }
        
                                                            /* change control code */     
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] &= ~PLU_IMP_ADJKEY_MASK; 
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] |= (SetMenuRcvBuff.SetPLU[PARENT_PLU].uchAdjective & PLU_IMP_ADJKEY_MASK);
            ItemSalesGenerateAdj(pItemSales, auchAdjData);  /* generate ex. FSC code */
            if (auchAdjData[0] == 0) {                      /* can't get adj. group from control code */
                return(LDT_PROHBT_ADR);
            }

	        if (((pItemSales->uchAdjective - 1) % 5 + 1) != auchAdjData[1]) {
				PLUIF    PLURecRcvBuffer = {0};

	            /* Re-read PLU adjective price */
    	        _tcsncpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
            	PLURecRcvBuffer.uchPluAdj = SetMenuRcvBuff.SetPLU[PARENT_PLU].uchAdjective;  

	            if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
    	            return(OpConvertError(sReturnStatus));
        	    } 

            	RflConv3bto4b(&pItemSales->lUnitPrice, PLURecRcvBuffer.ParaPlu.auchPrice);   /* covert 3 byte price to LONG data */
            }
        } 
    }

    /*----- CHECK CHILD PLU -----*/     
    j = 0;                                      /* intialize work counter */
    for (i = CHILD_PLU1; i <= MAX_NO_OF_CHILD; i++) {
		ITEMSALES    ChildPLU = {0};     /* initialize child PLU buffer */
		TCHAR	     auchDummy[NUM_PLU_LEN+1] = {0};
		PLUIF        PLURecRcvBuffer = {0};
		DEPTIF       DeptRecRcvBuffer = {0};
    
        if (_tcsncmp(SetMenuRcvBuff.SetPLU[i].aszPLUNumber, auchDummy, NUM_PLU_LEN) != 0) {
            _tcsncpy(PLURecRcvBuffer.auchPluNo, SetMenuRcvBuff.SetPLU[i].aszPLUNumber, NUM_PLU_LEN);
            PLURecRcvBuffer.uchPluAdj = 1;
    
            if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
                return(OpConvertError(sReturnStatus));
            }
            if ((PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) ||
                (PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[PLU_CONTROLCODE_ADR_2] & PLU_HASH) ) {
                return(LDT_PROHBT_ADR);
            }
			/* copy control code - remember ControlCode.auchStatus[5] used for ItemSales status info */
            memcpy(ChildPLU.ControlCode.auchPluStatus, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther, OP_PLU_PLU_SHARED_STATUS_LEN);
            memcpy(ChildPLU.ControlCode.auchPluStatus + 6, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther + 5, OP_PLU_OPTIONS_STATUS_LEN);
            
            if ((PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
				UCHAR  auchAdjData[2] = {0};     /* [0] : adjective group, [1] : adj. valiation */
				UCHAR  uchWork = ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4];

                if (SetMenuRcvBuff.SetPLU[i].uchAdjective == 0) { 
                                                /* not set table valiation */
                    if ((PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[PLU_CONTROLCODE_ADR_4] & PLU_IMP_ADJKEY_MASK) == PLU_COMP_ADJKEY) {
                                                /* compulsory adj. entry ? */
                        return(LDT_PROHBT_ADR);
                    }
            
                    SetMenuRcvBuff.SetPLU[i].uchAdjective = (UCHAR) (ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] & PLU_IMP_ADJKEY_MASK);
                                                /* use assumed valiation */
                } else {
                    sReturnStatus = ItemSalesSetCheckValidation(ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4], SetMenuRcvBuff.SetPLU[i].uchAdjective);
                    if (sReturnStatus != ITM_SUCCESS) {
                        return(sReturnStatus);
                    }
                                                /* change control code */     
                    ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] &= ~PLU_IMP_ADJKEY_MASK;
                    ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] |= (UCHAR) (SetMenuRcvBuff.SetPLU[i].uchAdjective & PLU_IMP_ADJKEY_MASK);
                }

                ItemSalesGenerateAdj(&ChildPLU, auchAdjData);
                if (auchAdjData[0] == 0) {
                    return(LDT_PROHBT_ADR);
                } 
                
                ChildPLU.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] = uchWork;   /* restore saved status */
            } else {
                ChildPLU.uchAdjective = 0;
            }
            
            /*--- SET CHILD PLU INTO PARENT PLU ---*/
            /* get dept record */
            DeptRecRcvBuffer.usDeptNo = PLURecRcvBuffer.ParaPlu.ContPlu.usDept;
            if (DeptRecRcvBuffer.usDeptNo == 0) {
                return(LDT_PROHBT_ADR);
            }

            if ((sReturnStatus = CliOpDeptIndRead(&DeptRecRcvBuffer, 0)) != OP_PERFORM) {
                return(OpConvertError(sReturnStatus));
            }
                        
			// This code in which the condiment data is copied from the PLU record needs
			// to correspond with the source code for creating a condiment from an ITEMSALES
			// struct in function ItemCondimentData().
			//
			// This code is for Promotional PLU in which a list of PLUs specified in
			// the AC116 Build Sales Promotion PLU dialog of PEP are added to the sales item
			// in the condiment area of the sales item data. The uchMinorClass is changed
			// to indicate the sales item is now a sales item with additional, promotional PLUs
			// attached.
			//
            _tcsncpy(pItemSales->Condiment[j].auchPLUNo, SetMenuRcvBuff.SetPLU[i].aszPLUNumber, NUM_PLU_LEN);
            pItemSales->Condiment[j].usDeptNo = PLURecRcvBuffer.ParaPlu.ContPlu.usDept;    
            pItemSales->Condiment[j].uchAdjective = ChildPLU.uchAdjective;     

            _tcsncpy(pItemSales->Condiment[j].aszMnemonic, PLURecRcvBuffer.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
            _tcsncpy(pItemSales->Condiment[j].aszMnemonicAlt, PLURecRcvBuffer.ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
            
//           pItemSales->Condiment[j].ControlCode.uchItemType = (UCHAR) (PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);  
            pItemSales->Condiment[j].ControlCode.uchItemType = PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType;
            pItemSales->Condiment[j].ControlCode.uchReportCode = PLURecRcvBuffer.ParaPlu.ContPlu.uchRept;
			pItemSales->Condiment[j].ControlCode.usBonusIndex = PLURecRcvBuffer.ParaPlu.ContPlu.usBonusIndex;
			pItemSales->Condiment[j].ControlCode.uchPPICode = PLURecRcvBuffer.ParaPlu.uchLinkPPI;

            /* --- correct for kp#5 - #8 print R3.1 --- */
            memcpy(pItemSales->Condiment[j].ControlCode.auchPluStatus, ChildPLU.ControlCode.auchPluStatus, OP_PLU_CONT_OTHER);
        
            j ++;                               /* increment arrey No. */
            pItemSales->uchChildNo++;           /* incrememt setmenu counter, R3.0 */
        }
    }
 
    /*--- CHANGE MINOR CLASS ---*/
    switch(pItemSales->uchMinorClass) {
    case CLASS_PLU:
         pItemSales->uchMinorClass = CLASS_SETMENU;   // ItemSalesSetMenu() - promotional PLU
         break;

    case CLASS_PLUMODDISC:
         pItemSales->uchMinorClass = CLASS_SETMODDISC;  // ItemSalesSetMenu() - promotional PLU
         break;

    default:
        break;
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesSetCheckValidation(UCHAR auchControlStatus, UCHAR uchAdjective);
*
*    Input: UCHAR auchControlStatus
*           UCHAR uchAdjective
*
*   Output: None
*
*    InOut: 
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_PROHBT_ADR : Function aborted with prohibit operation error
*
** Description: This function checks adjective valiation.
*                
*                  
*                
*===========================================================================
*/

SHORT   ItemSalesSetCheckValidation(UCHAR auchControlStatus, UCHAR uchAdjective)
{
    if ((auchControlStatus & PLU_PROH_VAL_MASK) & (UCHAR) (PLU_PROHIBIT_VAL1 << (uchAdjective - 1))) {
            return(LDT_PROHBT_ADR);
    } 
    return(ITM_SUCCESS);
}

/****** end of file ******/
