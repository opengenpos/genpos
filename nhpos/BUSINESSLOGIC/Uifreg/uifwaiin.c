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
* Title       : Reg Waiter Sign In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFWAIIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifWaiterSignIn()    : Reg Waiter Sign In
*               UifBartenderSignIn() : Reg Bartender Sign In R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Nov-09-95:03.01.00:hkato      : R3.1
: Aug-11-99:03.05.00:K.Iwata    : R3.5 (remove WAITER_MODEL)
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
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <uic.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <display.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegBarSignIn[] = {FSC_BAR_SIGNIN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifBartenderSignIn(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Waiter Sign In Module
*===========================================================================
*/
SHORT UifBartenderSignIn(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus = 0;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegBarSignIn);        /* open waiter sign in key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT5)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            if (!husUifRegHandle) {                     /* not during transaction lock */
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                usLockStatus = 1;
            }
            UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
            UifRegData.regopeopen.uchMinorClass = CLASS_UIBARTENDERIN;
            UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData;
            sRetStatus = ItemSignIn(&UifRegData.regopeopen);
            UifRegWorkClear();
            if (sRetStatus != UIF_SUCCESS) {
                if (usLockStatus) {                     /* check transaction lock */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                }
                if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                    UieReject();                            /* send reject to parent */
                    return (SUCCESS);
                }
                return(sRetStatus);
            }
            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
            UieAccept();                                /* send accepted to parent */
        }
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}
