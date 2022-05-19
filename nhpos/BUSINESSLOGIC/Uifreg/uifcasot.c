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
* Title       : Reg Cashier Sign Out Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCASOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifCashierSignOut() : Reg Cashier Sign Out
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
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
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"
#include <BlFWif.h>

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCasSignOut[] = {FSC_SIGN_IN, FSC_B, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCashierSignOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Sign Out Module
*===========================================================================
*/
SHORT UifCashierSignOut(KEYMSG *pData)
{
    SHORT   sRetStatus;
	ULONG	ulStatus[2];

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasSignOut);               /* open cashier sign out key sequence */
        break;

    case UIM_INPUT:
        if ((pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) || (pData->SEL.INPUT.uchMajor == FSC_B)) {
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
            if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) {     /* sign in key ? */
                UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
            } else {
                UifRegData.regopeclose.uchMinorClass = CLASS_UIB_OUT;
            }
			BlFwIfDrawerStatus(ulStatus);
            UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
            sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                return(SUCCESS);
            }
            if (sRetStatus == UIF_CAS_SIGNOUT) {

				PifLog(MODULE_UI, LOG_EVENT_UIFUIE_REG_SIGNOUT);                 /* Write log          */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                                                        /* success or sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT; /* set cashier sign out status */
                UieAccept();                            /* send accepted to parent */
                return(SUCCESS);
            }
            return(sRetStatus);
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT DoAutoSignOut(SHORT SignOutKey)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Do the Sign Out 
*===========================================================================
*/
SHORT  DoAutoSignOut(SHORT SignOutKey){
	SHORT	sRetStatus;
	ULONG	ulStatus[2];
	
	UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
	UifRegData.regopeclose.uchMinorClass = SignOutKey;
    UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_AUTO;

	BlFwIfDrawerStatus(ulStatus);
	sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
	UifRegWorkClear();                              /* clear work area */
	if (sRetStatus == UIF_CAS_SIGNOUT) { 
		BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
		flUifRegStatus |= UIFREG_CASHIERSIGNOUT; /* set cashier sign out status */
    }

    return (SUCCESS);
}


