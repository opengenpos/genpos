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
* Title       : Reg Waiter Sign Out Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFWAIOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifWaiterSignOut()    : Reg Waiter Sign Out
*               UifBartenderSignOut() : Reg Bartender Sign Out R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Nov-09-95:03.01.00:hkato      : R3.1
: Aug-11-99:03.05.00: K.Iwata   : R3.5 (remove WAITER_MODEL)
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
UISEQ FARCONST aszSeqRegBarSignOut[] = {FSC_BAR_SIGNIN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifBartenderSignOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Waiter Sign Out Module
*===========================================================================
*/
SHORT UifBartenderSignOut(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegBarSignOut);        /* open waiter sign out key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
            UifRegData.regopeclose.uchMinorClass = CLASS_UIBARTENDEROUT;
            sRetStatus = ItemSignOut(&UifRegData.regopeclose);
            UifRegWorkClear();
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_WAI_SIGNOUT){         /* error ? */
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_WAITERSIGNOUT;     /* set waiter sign out status */
            UieAccept();                                /* send accepted to parent */
        }
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}
