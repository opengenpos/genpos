/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1996                **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Beverage Dispenser(TMI 6400) Interface Routines
* Category    : Beverage Dispenser Interface, NCR 2170 RESTRANT MODEL
* Program Name: DIFSUB.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               DifSearchData();
*               DifSearchPlu();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Feb-14-95 : 03.01.00 : M.Ozawa      : Initial                     
*
** NCR2171 **                                         
* Aug-26-99 : 01.00.00 : Mikio Teraki :initial (for 2171)
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

#if defined(USE_2170_ORIGINAL)

/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
#include    <log.h>
#include    <uie.h>
#include    <fsc.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <appllog.h>
#include    <trans.h>
#include    <mld.h>
#include    <dif.h>
#include    "diflocal.h"
#include    "..\item\include\itmlocal.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
SHORT   DifSearchPlu(PLUIF *pPluIf, USHORT fsLink);
SHORT   DifCheckPartialTender(VOID);

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
**Synopsis: SHORT   UifSearchData()
*                                    
*    Input: 
*           
*   Output: None
*
*    InOut: 
*
**Return: DIF_SUCCESS    : Function Performed succesfully
*         
*         DIF_SEARCH_ERR : Function prohibited by parameter option.
*
** Description: This function searches PLU sent from 6400, and verify
*               the PLU is able to sell or not.
*                
*===========================================================================
*/
SHORT   DifSearchData(DIFMSG *pDifMsg)
{
    if (pDifMsg->uchMsg == DIF_POUR_REQ_MSG) {
		LABELANALYSIS   PluLabel = { 0 };
		PARADISPPARA    ParaDispPara = { 0 };
		PLUIF           PluIf = { 0 };       /* PLU record receive buffer */
		USHORT          usPluNo;
		TCHAR           auchPLUNo[PLU_MAX_DIGIT + 1] = { 0 };

        /*----- PLU PREPARATION ----- */
        /* ----- set plu number for label analysis ----- */
        usPluNo = atoi(pDifMsg->auchData);
        _itot(usPluNo, auchPLUNo, 10);
        _tcsncpy(PluLabel.aszScanPlu, auchPLUNo, NUM_PLU_LEN);

        /* ----- check UPC Velocity key ----- */
        PluLabel.fchModifier |= LA_UPC_VEL;

        /* ----- analyze PLU number from UI ----- */
        if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
            _tcsncpy(PluIf.auchPluNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
        } else {
            return(DIF_SEARCH_ERR);
        }
        PluIf.uchPluAdj = 1;      /* dummy adjective valiation */

        ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;
        ParaDispPara.usAddress = DISPENS_OFFSET_ADR;
        CliParaRead(&ParaDispPara);
        if ((SHORT)(usPluNo - ParaDispPara.usPluNumber) > 0) {
            ParaDispPara.usAddress = (usPluNo - ParaDispPara.usPluNumber);
            if (ParaDispPara.usAddress < DISPENS_OFFSET_ADR) {

                CliParaRead(&ParaDispPara);
                if (ParaDispPara.usPluNumber) {
                    memset(&PluLabel, 0, sizeof(PluLabel));

                    _itot(ParaDispPara.usPluNumber, PluLabel.aszScanPlu, 10);

                    /* ----- check UPC Velocity key ----- */
                    PluLabel.fchModifier |= LA_UPC_VEL;

                    /* ----- analyze PLU number from UI ----- */
                    if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
                        _tcsncpy(PluIf.auchPluNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
                    } else {
                        return(DIF_SEARCH_ERR);
                    }
                    if (ParaDispPara.uchAdjective) {
                        PluIf.uchPluAdj = ParaDispPara.uchAdjective;
                    }
                    while((CHAR)PluIf.uchPluAdj > 5) {
                        PluIf.uchPluAdj -= 5;
                    }
                }
            }
        }

        if (CliOpPluIndRead(&PluIf, 0) != OP_PERFORM) {
            return(DIF_SEARCH_ERR);
        } 

        if ((ParaDispPara.usPluNumber)&&(ParaDispPara.uchAdjective)) {
            PluIf.uchPluAdj = ParaDispPara.uchAdjective;
        }

        if (DifSearchPlu(&PluIf, FALSE) == DIF_SUCCESS) {

            memset(&PluLabel, 0, sizeof(PluLabel));

            _itot(PluIf.ParaPlu.usLinkNo, PluLabel.aszScanPlu, 10);
            /* ----- check UPC Velocity key ----- */
            PluLabel.fchModifier |= LA_UPC_VEL;

            /* ----- analyze PLU number from UI ----- */
            if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
                _tcsncpy(PluIf.auchPluNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
            } else {
                return(DIF_SEARCH_ERR);
            }
            if (DifSearchPlu(&PluIf, FALSE) != DIF_SUCCESS) {
                return (DIF_SEARCH_ERR);
            }
        } else {
            return (DIF_SEARCH_ERR);
        }

        return (DIF_SUCCESS);
        /* return(DifSearchPlu(&PluIf)); */
    }

    /* check total key/tender */

    if ((pDifMsg->uchMsg == DIF_NEXT_ORDER_MSG) || (pDifMsg->uchMsg == DIF_END_OF_POUR_MSG)) {
		PARADISPPARA    ParaDispPara = { 0 };

		ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;
        if (pDifMsg->uchMsg  == DIF_NEXT_ORDER_MSG) {
            ParaDispPara.usAddress = DISPENS_TOTAL1_ADR;
        } else {
            ParaDispPara.usAddress = DISPENS_TOTAL2_ADR;
        }

        CliParaRead(&ParaDispPara);

        if (ParaDispPara.usPluNumber) {
            return(DIF_SUCCESS);
        } else {
            return(DIF_SEARCH_ERR);
        }
    }

    return (DIF_SEARCH_ERR);      /* illegal message */

}

/*
*===========================================================================
**Synopsis: SHORT   UifSearchPlu()
*                                    
*    Input: 
*           
*   Output: None
*
*    InOut: 
*
**Return: DIF_SUCCESS    : Function Performed succesfully
*         
*         DIF_SEARCH_ERR : Function prohibited by parameter option.
*
** Description: This function searches PLU sent from 6400, and verify
*               the PLU is able to sell or not.
*                
*===========================================================================
*/
SHORT   DifSearchPlu(PLUIF *pPluIf, USHORT fsLink)
{
    ITEMSALES  ItemSales;
    DEPTIF     DeptIf;       /* dept record receive buffer */
    UCHAR  auchAdjData[2];

    if (ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) {
        return(DIF_SEARCH_ERR);
    }

    if (ItemCommonCheckComp() != ITM_SUCCESS ) {
        return (DIF_SEARCH_ERR);
    }
    if (ItemCommonCheckExempt() != ITM_SUCCESS ) {
        return (DIF_SEARCH_ERR);
    }
    if (ItemCommonCheckSplit() != ITM_SUCCESS) {
        return (DIF_SEARCH_ERR);
    }

    memset(&ItemSales, 0, sizeof(ITEMSALES));

    ItemSales.uchMajorClass = CLASS_ITEMSALES;
    ItemSales.uchMinorClass = CLASS_PLU;

    /*----- PLU PREPARATION ----- */
    _tcsncpy(ItemSales.auchPLUNo, pPluIf->auchPluNo, NUM_PLU_LEN);
    /* ItemSales.usPLUNo = pPluIf->usPluNo; */
    ItemSales.uchAdjective = pPluIf->uchPluAdj;

    /* move dept No. into ItemSales.uchDeptNo */
    ItemSales.usDeptNo = pPluIf->ParaPlu.ContPlu.usDept;
    ItemSales.ControlCode.uchItemType = pPluIf->ParaPlu.ContPlu.uchItemType; 
    /* ItemSales.uchDeptNo = (UCHAR)(pPluIf->ParaPlu.ContPlu.uchDept & ~PLU_TYPE_MASK_REG);
    ItemSales.ControlCode.uchDeptNo = pPluIf->ParaPlu.ContPlu.uchDept;  */
                                        /* set PLU type */
    if (ItemSales.usDeptNo == 0) {
    /* if (ItemSales.uchDeptNo == 0) { */
        return(DIF_SEARCH_ERR);
    }

    /* copy control code - remember ControlCode.auchStatus[5] used for ItemSales status info */
    memcpy(ItemSales.ControlCode.auchPluStatus, pPluIf->ParaPlu.ContPlu.auchContOther, OP_PLU_PLU_SHARED_STATUS_LEN);
    memcpy(ItemSales.ControlCode.auchPluStatus + 6, pPluIf->ParaPlu.ContPlu.auchContOther + 5, OP_PLU_OPTIONS_STATUS_LEN);
    if ((ItemSales.ControlCode.uchItemType & PLU_TYPE_MASK_REG) == 0) {
        ItemSales.ControlCode.uchItemType |= PLU_OPEN_TYP;
    }                                   /* type 0 is regarded as open PLU */
    /* if ((ItemSales.ControlCode.uchDeptNo & PLU_TYPE_MASK_REG) == 0) {
        ItemSales.ControlCode.uchDeptNo |= PLU_OPEN_TYP;
    }                                   / type 0 is regarded as open PLU */
    
    /* set QTY and unit price */
    ItemSales.lQTY = 1000L;

    if (ItemSales.ControlCode.uchItemType & PRESET_PLU) {
    /* if (ItemSales.ControlCode.uchDeptNo & PRESET_PLU) { */
                                        /* if not open PLU */
        /* set record into pItemSales */
        ItemSales.lUnitPrice = RflFunc3bto4b(pPluIf->ParaPlu.auchPrice);
    } else {
        return(DIF_SEARCH_ERR);
    }
    
    /*----- INHERIT DEPT'S CONTROL STATUS -----*/
    /* get dept record */
    DeptIf.usDeptNo = ItemSales.usDeptNo;
    if (CliOpDeptIndRead(&DeptIf, 0) != OP_PERFORM) {
        return(DIF_SEARCH_ERR);
    }
        
    ItemSalesSetupStatus(DeptIf.ParaDept.auchControlCode);      /* setup dept's control status */
    /* turn on Hash bit for PLU */
    ItemSales.ControlCode.auchPluStatus[2] &= ~PLU_HASH;        /* turn off flag set by maintemance */
    if (ItemSales.ControlCode.auchPluStatus[2] & PLU_USE_DEPTCTL) {
        /*  use dept's control code, copy dept's control code into pItemSales */
        memcpy(ItemSales.ControlCode.auchPluStatus, DeptIf.ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
        ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] = DeptIf.ParaDept.auchControlCode[4];  /* CRT No5 - No 8 see ItemSendKdsSetCRTNo() */
    } else {                            /* turn on hash bit if need */
        if (DeptIf.ParaDept.auchControlCode[2] & PLU_HASH) {
            ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] |= PLU_HASH;
        }
    }

    ItemSales.ControlCode.uchItemType &= PLU_TYPE_MASK_REG;
                                        /* clear dept No. "0xc0" */
    ItemSales.ControlCode.uchItemType |= DeptIf.ParaDept.uchMdept;
                                        /* set dept's major dept No. */
    /* ItemSales.ControlCode.uchDeptNo &= PLU_TYPE_MASK_REG; */
                                        /* clear dept No. "0xc0" */
    /* ItemSales.ControlCode.uchDeptNo |= DeptIf.ParaDept.uchMdept; */
                                        /* set dept's major dept No. */

    /*----- SET UP PLU STATUS -----*/
    if (fsLink == TRUE) {
        ItemSales.ControlCode.auchPluStatus[2] &= ~PLU_CONDIMENT; 
    }
#if 0
    if (!(ItemSales.ControlCode.uchItemType & PRESET_PLU)) {
    /* if (!(ItemSales.ControlCode.uchDeptNo & PRESET_PLU)) { */
        ItemSales.ControlCode.auchStatus[2] &= ~PLU_CONDIMENT; 
    } else if (ItemSales.ControlCode.auchStatus[2] & PLU_CONDIMENT) {
        ItemSales.ControlCode.auchStatus[2] &=~ PLU_SCALABLE;
                                        /* turn off scalable status bit */ 
    }
#endif
    /*----- SET UP COMMON STATUS -----*/
    ItemSalesSetupStatus(ItemSales.ControlCode.auchPluStatus);

    switch(ItemSales.ControlCode.uchItemType & PLU_TYPE_MASK_REG) {
    /* switch(ItemSales.ControlCode.uchDeptNo & PLU_TYPE_MASK_REG) { */
    case PLU_NON_ADJ_TYP:
        break;

    case PLU_ADJ_TYP:
        if (ItemSales.uchAdjective) {
            if (ItemSalesBreakDownAdj(&ItemSales, auchAdjData) != ITM_SUCCESS) {
                return(DIF_SEARCH_ERR);
            }
        } else {
            if ((ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_4] & PLU_IMP_ADJKEY_MASK) == PLU_COMP_ADJKEY) {
                return(DIF_SEARCH_ERR);
            }
        
            ItemSalesGenerateAdj(&ItemSales, auchAdjData);      /* generated extended FSC code are */
                                                            /* saved into uchAdjective         */   
            if (auchAdjData[0] == 0) { /* not hit group */
                return(DIF_SEARCH_ERR);
            } 
        }    
        break;

    default:    /* open plu */
        return(DIF_SEARCH_ERR);
    }

    if (ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) {
        return(DIF_SEARCH_ERR);
    }
    if (ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & (PLU_SCALABLE|PLU_CONDIMENT)) {
        return(DIF_SEARCH_ERR);
    }

    if (!(ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) && (DeptIf.ParaDept.auchControlCode[2] & PLU_SCALABLE)) { 
        return(DIF_SEARCH_ERR);
    }

    /* ----- check sales restriction, 2172 ----- */

    if (ItemSalesSalesRestriction(&ItemSales, pPluIf->ParaPlu.uchRestrict) != ITM_SUCCESS) {
        return (DIF_SEARCH_ERR);
    }
            
    /* ----- check birthday entry,  2172  ----- */
    if (ItemSalesCheckBirth(&ItemSales) != ITM_SUCCESS) {
        return (DIF_SEARCH_ERR);
    }

    /*----- CHECK STORAGE SIZE -----*/
    if (TrnCheckSize(&ItemSales, TRN_1ITEM_BYTES*2) < 0) {
        return(DIF_SEARCH_ERR);
    }
                                            
    return(DIF_SUCCESS);
}

#endif

/* ====================================== */
/* ========== End of DIF.C ============== */
/* ====================================== */
