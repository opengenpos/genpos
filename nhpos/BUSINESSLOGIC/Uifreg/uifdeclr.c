/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : Reg Declared Tips Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFDECLR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDeclared() : Reg Declared Tips
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"
#include "BlFWif.h"

/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegDeclared[] = {FSC_DECLAR_TIPS, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDeclared(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Declared Tips Module
*===========================================================================
*/
SHORT UifDeclared(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegDeclared);     /* open declared tips key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_DECLAR_TIPS) { /* declared ? */ 
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regmisc.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }

			NHPOS_ASSERT_ARRAYSIZE(UifRegData.regmisc.aszNumber, NUM_NUMBER); //JHHJ NHPOS ASSERT

            _tcsncpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, NUM_NUMBER);
            //memcpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number saratoga */
            /* memcpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, NUM_NUMBER);  / set number */
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;   /* set declared tips class */
            UifRegData.regmisc.uchMinorClass = CLASS_UITIPSDECLARED;
            UifRegData.regmisc.lAmount = pData->SEL.INPUT.lData;  /* set input amount */
            sRetStatus = ItemMiscTipsDec(&UifRegData.regmisc);     /* Misc Module */
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == UIF_WAI_SIGNOUT) {        /* waiter sign out ? */
                flUifRegStatus |= UIFREG_WAITERSIGNOUT; /* set waiter sign out status */
                UieAccept();                            /* send accepted to parent */
                break;
            }
            /* V3.3 */
            if (sRetStatus == UIF_CAS_SIGNOUT) {        /* cashier sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT;    /* set cashier sign out status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                            /* send accepted to parent */
                return(SUCCESS);
            }
            if (sRetStatus != UIF_FINALIZE) {           /* error ? */
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
