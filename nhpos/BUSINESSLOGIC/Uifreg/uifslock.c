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
* Title       : Reg Cashier Sign In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFSLOCK.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifCashierSignIn() : Reg Cashier Sign In
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Aug-07-98:03.00.00:M.Ozawa   : initial                                   
*          :        :           :                                    
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
#include <uireg.h>
#include <display.h>
#include <appllog.h>
#include "uiregloc.h"
#include "BlFWif.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCasLockSignIn[] = {FSC_WAITER_PEN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCasLockSignInOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Sign In Module
*===========================================================================
*/
SHORT UifCasLockSignInOut(KEYMSG *pData)
{
    SHORT       sRetStatus;
    USHORT      usLockStatus = 0;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasLockSignIn);            /* open cashier sign in key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_WAITER_PEN) {  /* waiter lock */
            if (pData->SEL.INPUT.uchMinor != 0) {       /* insert waiter lock ? */
                if (!husUifRegHandle) {                     /* not during transaction lock */
                    if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                        husUifRegHandle = 0;
                        UieErrorMsg(LDT_LOCK_ADR, UIE_WITHOUT); /* error display */
                        ItemOtherDiaRemoveKey();                /* remove key */
                        UifRegData.regmodein.uchMajorClass = CLASS_UIFREGMODE;  /* set mode in class */
                        UifRegData.regmodein.uchMinorClass = CLASS_UIREGMODEIN;
                        ItemModeIn(&UifRegData.regmodein);  /* Item Mode In Modele */
                        UifRegWorkClear();              /* clear work area */
                        UieReject();                    /* send reject to parent */
                        return (SUCCESS);
                    }
                    usLockStatus = 1;
                }
                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;  /* set cashier sign in class */
                UifRegData.regopeopen.uchMinorClass = CLASS_UIWAITERLOCKIN;
                UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.uchMinor; /* set cashier number */
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                    if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                        UieReject();                            /* send reject to parent */
                        return (SUCCESS);
                    }
                    return(sRetStatus);
                }
                UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
                UieAccept();                                       /* send accepted to parent */

            } else {                                                /* remove waiter lock */

                UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set waiter sign out class */
                UifRegData.regopeclose.uchMinorClass = CLASS_UIWAITERLOCKOUT;
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    flUifRegStatus |= UIFREG_CASHIERSIGNOUT;    /* set cashier sign out status */
                } else if (sRetStatus == UIF_CANCEL) {          /* error ? */
                    UieReject();                                /* send reject to parent */
                    return(sRetStatus);
                } else {
                    return(sRetStatus);
                }
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                                       /* send accepted to parent */
            }
            break;
        }
        /* break */                                     /* not use */

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}


