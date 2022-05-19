/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Foreign In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFFORIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifForeignIn() : Reg Foreign In
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Dec-06-99:01.00.00:hrkato     : Saratoga
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
UISEQ FARCONST aszSeqRegForeignIn[] = {FSC_FOREIGN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifForeignIn(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Foreign In Module
*===========================================================================
*/
SHORT UifForeignIn(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegForeignIn);        /* open foreign in key sequence */
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_FOREIGN) {     /* foreign ? */
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_FC_MAX)) { /* check foreign # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;  /* set foreign class */
            UifRegData.regtender.uchMinorClass = (UCHAR)(pData->SEL.INPUT.uchMinor + CLASS_UIFOREIGNTOTAL1 - 1);
            sRetStatus = ItemTenderEntry(&UifRegData.regtender);     /* Tender Modele */
            UifRegWorkClear();                              /* clear work area */
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
            UieAccept();                                /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/* --- End of Source File --- */