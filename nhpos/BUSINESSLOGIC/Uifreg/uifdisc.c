/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : Reg Discount Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFDISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiscount() : Reg Discount
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Nov-13-95:03.01.00:hkato      : R3.1
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegDiscount[] = {FSC_REGULAR_DISC, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiscount(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Discount Module
*===========================================================================
*/
SHORT UifDiscount(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegDiscount);     /* open discount key sequence */
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_REGULAR_DISC) {    /* regular discount ? */ 
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_RDISC_MAX)) { /* check regular discount # */
                return(LDT_PROHBT_ADR);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regdisc.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }


			NHPOS_ASSERT_ARRAYSIZE( UifRegData.regdisc.aszNumber, NUM_NUMBER);
            _tcsncpy(UifRegData.regdisc.aszNumber, aszUifRegNumber, NUM_NUMBER);
            //memcpy(UifRegData.regdisc.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number saratoga */
            /* memcpy(UifRegData.regdisc.aszNumber, aszUifRegNumber, NUM_NUMBER);  / set number */
            UifRegData.regdisc.uchMajorClass = CLASS_UIFREGDISC;   /* set discount class */
            switch(pData->SEL.INPUT.uchMinor) {                    /* R3.1 */
            case    1:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC1;
                break;
            case    2:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC2;
                break;
            case    3:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC3;
                break;
            case    4:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC4;
                break;
            case    5:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC5;
                break;
            case    6:
                UifRegData.regdisc.uchMinorClass = CLASS_UIREGDISC6;
                break;
            default:
                return(LDT_PROHBT_ADR);
            }
            UifRegData.regdisc.lAmount = pData->SEL.INPUT.lData;  /* set input amount */
            sRetStatus = ItemDiscount(&UifRegData.regdisc);   /* Discount Modele */
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

