/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Transfer Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFTRNSF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifTransfer() : Reg Transfer
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jan-12-96:03.01.00:hkato      : R3.1
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
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegTransfer[] = {FSC_TRANSFER, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT   UifTransfer(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Transfer Module
*===========================================================================
*/
SHORT   UifTransfer(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;
    union {                                /* Reg Data work */
        UIFREGMODIFIER  regmodifier;                /* Modifier */
        UIFREGOTHER     regother;                   /* Other Transaction */
    } ItemData;

    memset(&ItemData, 0, sizeof(ItemData));         /* Clear Work */

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegTransfer);
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_TRANSFER) {
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {
                return(sRetStatus);
            }
            ItemData.regother.uchMajorClass = CLASS_UIFREGOTHER;
            ItemData.regother.uchMinorClass = CLASS_UITRANSFER;
            ItemData.regother.lAmount = pData->SEL.INPUT.lData; /* Transfer QTY */
            sRetStatus = ItemOtherTransfer(&ItemData.regother);
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifTransfItem(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Transfer (Itemize) Module
*===========================================================================
*/
SHORT   UifTransfItem(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;
    union {                                /* Reg Data work */
        UIFREGMODIFIER  regmodifier;                /* Modifier */
        UIFREGOTHER     regother;                   /* Other Transaction */
    } ItemData;

    memset(&ItemData, 0, sizeof(ItemData));         /* Clear Work */

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegTransfer);
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_TRANSFER) {
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {
                return(sRetStatus);
            }
            ItemData.regother.uchMajorClass = CLASS_UIFREGOTHER;
            ItemData.regother.uchMinorClass = CLASS_UITRANSFER;
            ItemData.regother.lAmount = pData->SEL.INPUT.lData; /* Transfer QTY */
            sRetStatus = ItemOtherTransfItem(&ItemData.regother);
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
