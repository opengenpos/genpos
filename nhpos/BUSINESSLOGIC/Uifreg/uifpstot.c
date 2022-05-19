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
* Title       : Reg Post Receipt Module                         
* Category    : User Interface for Reg, NCR 2170 INT'L Hospitality Application         
* Program Name: UIFPOST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPostReceptSignOut() : Reg Post Receipt
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Aug-07-98:03.03.00:M.Ozawa    : initial                                   
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
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <uic.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegPostSignOut[] = {FSC_PRT_DEMAND, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPostReceiptSignOut(KEYMSG *pData)
*
*     Input:    pData - key message pointer
*     Output:   nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg Post Receipt/Parking Module,        V3.3
*===========================================================================
*/

SHORT UifPostReceiptSignOut(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus = 0;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegPostSignOut);  /* open post receipt key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_PRT_DEMAND) {  /* post receipt ? */ 
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ( ! ((pData->SEL.INPUT.uchMinor == FSC_POSTRECEIPT) ||
                    (pData->SEL.INPUT.uchMinor == FSC_PARKING))) {
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void,number */
                return(sRetStatus);
            }
            if (!husUifRegHandle) {             /* not during transaction lock */
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                usLockStatus = 1;
            }
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;    /* set post receipt class */
            if (pData->SEL.INPUT.uchMinor == FSC_POSTRECEIPT) {     /* post receipt ? */ 
                UifRegData.regmisc.uchMinorClass = CLASS_UIPOSTRECEIPT;
            } else {
                UifRegData.regmisc.uchMinorClass = CLASS_UIPARKING;
            }
            sRetStatus = ItemMiscPostReceipt(&UifRegData.regmisc);
            UifRegWorkClear();                          /* clear work area */
            if (usLockStatus) {                         /* check transaction lock */
                if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifRegHandle = 0;
            }
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                return(SUCCESS);
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
