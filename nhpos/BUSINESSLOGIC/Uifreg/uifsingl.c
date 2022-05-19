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
* Title       : Reg Single Transaction Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFSINGL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifSingleTrans() : Reg Single Transaction
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Jul-23-93:01.00.12:K.You      : memory compaction                                   
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
#include "BlFWif.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegSingleTrans[] = {FSC_PAID_OUT, FSC_ROA, FSC_NO_SALE,
                                         FSC_TIPS_PAID, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifSingleTrans(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Single Transaction Module
*===========================================================================
*/
SHORT UifSingleTrans(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegSingleTrans);      /* open single transaction key sequence */
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if ((pData->SEL.INPUT.uchMajor == FSC_PAID_OUT) ||  /* paid out ? */ 
            (pData->SEL.INPUT.uchMajor == FSC_ROA) ||       /* receive on account ? */
            (pData->SEL.INPUT.uchMajor == FSC_TIPS_PAID) || /* tips paid out ? */
            (pData->SEL.INPUT.uchMajor == FSC_NO_SALE)) {   /* no sale ? */
            if (pData->SEL.INPUT.uchMajor == FSC_NO_SALE) {   /* no sale ? */
                if ((pData->SEL.INPUT.uchLen != 0) || (uchUifRegVoid != 0)) {             /* exist input data ? */
                    return(LDT_SEQERR_ADR);
                }
            } else {
                if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                    return(sRetStatus);
                }
            }

            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regmisc.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }

            _tcsncpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, NUM_NUMBER/*sizeof(aszUifRegNumber)*/);  /* set number */
            UifRegData.regmisc.lAmount = pData->SEL.INPUT.lData;  /* set input amount */
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;    /* set single transaction class */

            if (pData->SEL.INPUT.uchMajor == FSC_PAID_OUT) {    /* paid out ? */ 
                UifRegData.regmisc.uchMinorClass = CLASS_UIPO;
                sRetStatus = ItemMiscPO(&UifRegData.regmisc, 1);         /* Misc Module */
            } else if (pData->SEL.INPUT.uchMajor == FSC_ROA) {  /* receive on account ? */ 
                UifRegData.regmisc.uchMinorClass = CLASS_UIRA;
                sRetStatus = ItemMiscRA(&UifRegData.regmisc, 1);         /* Misc Module */
            } else if (pData->SEL.INPUT.uchMajor == FSC_TIPS_PAID) {  /* tips paid out ? */ 
                UifRegData.regmisc.uchMinorClass = CLASS_UITIPSPO;
                sRetStatus = ItemMiscTipsPO(&UifRegData.regmisc);     /* Misc Module */
            } else {                                            /* no sale */
                UifRegData.regmisc.uchMinorClass = CLASS_UINOSALE;
                sRetStatus = ItemMiscNosale(&UifRegData.regmisc);     /* Misc Module */
            }

            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }

			if (sRetStatus == UIF_CAS_SIGNOUT) {        /* cashier sign out ? */
				flUifRegStatus |= UIFREG_CASHIERSIGNOUT;    /* set cashier sign out status */
			} else if (sRetStatus == UIF_WAI_SIGNOUT) {
				flUifRegStatus |= UIFREG_WAITERSIGNOUT;    /* set waiter sign out status */
			} else if (sRetStatus != UIF_FINALIZE) {          /* error ? */
                return(sRetStatus);
            }
            UieAccept();                               /* send accepted to parent */
			// if this is a Finalize but there has not been a Cashier Sign-out then
			// we need to do a clear display and display the default user screen.
			// If we have already done a Cashier Sign-out, for instance if the
			// MDC is set to automatically do a Sign-out after doing a transaction,
			// then we do not need to do this step.
			if (sRetStatus == UIF_FINALIZE) {          /* cashier sign out ? */
				UifRegClearDisplay(0);						/* clear work area */
				BlFwIfDefaultUserScreen();					/* call function to bring up default window(s) */
			}
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
