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
* Title       : Reg Print On Demand Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFPRDMD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifPrintOnDemand() : Reg Print On Demand
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Jul-23-93:01.00.12:K.You      : memory compaction                                   
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
UISEQ FARCONST aszSeqRegPrintOnDemand[] = {FSC_PRT_DEMAND, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPrintOnDemand(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Print On Demand Module
*===========================================================================
*/
SHORT UifPrintOnDemand(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegPrintOnDemand);    /* open print on demand key sequence */
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser *//* ### Mod (2171 for Win32) V1.0 */
        if (pData->SEL.INPUT.uchMajor == FSC_PRT_DEMAND) {  /* print on demand ? */
            if (pData->SEL.INPUT.uchMinor != FSC_PRINTDEMAND) {
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            /* UifRegData.regother.uchMajorClass = CLASS_UIFREGOTHER;   set error correct class 
            UifRegData.regother.uchMinorClass = CLASS_UIPRINTDEMAND; */
            sRetStatus = ItemOtherDemand();                 /* Other Module */
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

/****** End of Source ******/
