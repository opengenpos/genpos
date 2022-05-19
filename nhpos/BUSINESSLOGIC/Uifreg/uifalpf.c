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
* Title       : Reg GC Information Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFALPF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           UifAlphaFist() : Alpha Name entry at cashier operation
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jul-25-95:03.00.00:   hkato   : R3.0
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
UISEQ FARCONST aszSeqRegAlphaFirst[] = {FSC_ALPHA, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT   UifAlphaFirst( KEYMSG *pData )
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Alpha Name entry at cashier operation
*===========================================================================
*/
SHORT   UifAlphaFirst( KEYMSG *pData )
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:                                                  
        UieOpenSequence( aszSeqRegAlphaFirst );
        break;                                  
                                                            
    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if ( pData->SEL.INPUT.uchMajor == FSC_ALPHA) {
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;
            UifRegData.regmodifier.uchMinorClass = CLASS_UIALPHANAME;
            sRetStatus = ItemModAlphaName();
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
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
** Synopsis:    SHORT   UifTent( KEYMSG *pData )
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Alpha Name entry at cashier operation
*===========================================================================
*/
SHORT   UifTent( KEYMSG *pData )
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:                                                  
        UieOpenSequence( aszSeqRegAlphaFirst );
        break;                                  
                                                            
    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if ( pData->SEL.INPUT.uchMajor == FSC_TENT) {
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;
            UifRegData.regmodifier.uchMinorClass = CLASS_UIALPHANAME;
			// if the operator entered a tent number with the numeric key pad before pressing FSC_TENT
			// then we will pass the number on through.  this way use of alphanumeric keyboard is
			// not required.
			sRetStatus = ItemModTent(pData->SEL.INPUT.aszKey, (USHORT)pData->SEL.INPUT.uchLen);
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
