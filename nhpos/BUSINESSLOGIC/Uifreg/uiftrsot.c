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
* Title       : Reg Transfer Out Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFTRSOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifTransferOut() : Reg Transfer Out
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
UISEQ FARCONST aszSeqRegTransferOut[] = {FSC_GUEST_CHK_TRN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifTransferOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Transfer Out Module
*===========================================================================
*/
SHORT UifTransferOut(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegTransferOut);      /* open transfer out key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_GUEST_CHK_TRN) {   /* check transfer ? */
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }

            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }

            /* UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;     set transfer out class 
            UifRegData.regmisc.uchMinorClass = CLASS_UICHECKTRANSFEROUT; */
            sRetStatus = ItemMiscCheckTROut();          /* Misc Module */
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

			if (sRetStatus == UIF_CAS_SIGNOUT) {        /* cashier sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT;    /* set cashier sign out status */
            } else if ((sRetStatus != UIF_FINALIZE) && (sRetStatus != UIF_SUCCESS)) {           /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
			UifRegClearDisplay(0);						/* clear work area */
			BlFwIfDefaultUserScreen();					/* call function to bring up default window(s) */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
