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
* Title       : Reg Cancel Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCAN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifCancel() : Reg Cancel
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Jul-23-93:01.00.12:K.You      : memory compaction
* Nov-20-95:03.01.00:hkato      : R3.1
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
#include	<windows.h>
#include	<tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"
#include "transact.h"
#include "cscas.h"
#include "BlFwIf.h"  
#include "csetk.h"
#include "para.h"
#include "..\Item\Include\itmlocal.h"
#include "csstbpar.h"

/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCancel[] = {FSC_CANCEL, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCancel(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cancel Module
*===========================================================================
*/
SHORT UifCancel(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCancel);    /* open cancel key sequence */
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_CANCEL) {  /* cancel ? */
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
			if (pData->SEL.INPUT.uchMinor != 0) {
				// allow more than one type of Cancel Key so that we do not
				// provide a way for Cashier to inadvertently cancel a transaction.
				// requested by Amtrak who is putting a Cancel Key button on their
				// layout to only be used to Cancel a credit tender.
                return(LDT_SEQERR_ADR);
			}
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
            UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
            sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
            UifRegWorkClear();                              /* clear work area */
            switch (sRetStatus) {
            case UIF_CANCEL:                            /* cancel */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;

            case UIF_FINALIZE:                          /* finalize */
                flUifRegStatus |= UIFREG_FINALIZE;      /* set finalize status */
                UieAccept();                            /* send accepted to parent */
				UifRegClearDisplay(FSC_CANCEL);			/* clear work area */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                break;

            case UIF_CAS_SIGNOUT:                       /* cashier sign out */
                flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);
                                        /* set cashier sign out and finalize status */
                UieAccept();                            /* send accepted to parent */
                break;

            case UIF_WAI_SIGNOUT:                       /* waiter sign out */
                flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);
                                        /* set waiter sign out and finalize status */
                UieAccept();                            /* send accepted to parent */
                break;

            default:                                        
                return(sRetStatus);
            }
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT CasForceAutoSignOut()
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Auto Sign Out used with the timer and FSC key FSC_AUTO_SIGN_OUT
*===========================================================================
*/
static UIMENU FARCONST aUifReg[] = {{UifReg, CID_REG}, {NULL, 0}};
//static UIMENU FARCONST aUifSuper[] = {{UifACMode, CID_ACMODE}, {NULL, 0}};
//static UIMENU FARCONST aUifProg[] = {{UifPGMode, CID_PGMODE}, {NULL, 0}};
//static UIMENU FARCONST aUifLock[]  = {{UifLock,  CID_LOCK},  {NULL, 0}};

SHORT	CasForceAutoSignOut(USHORT usMinorFsc)
{
	SHORT	      SignOutKey;
	CHARDATA      CharData = { 0 };
	VOID         *pResult = 0;

	//check for the Sign in Key type to use for the sign out.
	if(TranModeQualPtr->sCasSignInKey == CLASS_CASHIERIN) {
		SignOutKey = CLASS_UICASHIEROUT;
    } else if(TranModeQualPtr->sCasSignInKey == CLASS_B_IN){
		SignOutKey = CLASS_UIB_OUT;
    }
	else {
		return SUCCESS;
	}

	UieMessageBoxSendMessage (WM_COMMAND, IDABORT, 0);

	UieClearRingBuf();                      /* clear ring buffer       */
	CharData.uchFlags = 0;

	//#1 End Transaction 
	//First we must get out of the transaction before we sign out
	//process cancel button to clear any missed setting or data and then sign out
	if(usMinorFsc == 1) {
		int   iRetryCount = 5;

		CharData.uchASCII = 0x43;   // FSC_CLEAR
		CharData.uchScan = 0x00;
		UiePutKeyData (2, &CharData, pResult);

		PifSleep (100);

		do {
			CharData.uchASCII = 0xBD;   // FSC_CANCEL
			CharData.uchScan = 0x00;
			UiePutKeyData (2, &CharData, pResult);
			PifSleep(250);     // CasForceAutoSignOut() give UI thread time to process the keystroke
			iRetryCount--;
		} while (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == 0 && iRetryCount > 0);  // check to see if sign-out is prohibitted

		//Added a forced sign out option
		//Cancel button
//		{
//			SHORT	      sRetStatus;
//			UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
//			UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
//			sRetStatus = ItemMiscCancel (&UifRegData.regmisc);     /* Misc Modele */

//			UifRegWorkClear();                              /* clear work area */
//			switch (sRetStatus) {
//			case UIF_FINALIZE:                          /* finalize */
//			f	lUifRegStatus |= UIFREG_FINALIZE;      /* set finalize status */
//				break;

//			case UIF_CAS_SIGNOUT:                       /* cashier sign out */
//				flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);
									/* set cashier sign out and finalize status */
//				break;

//			case UIF_WAI_SIGNOUT:                       /* waiter sign out */
//				flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);
									/* set waiter sign out and finalize status */
//				break;
//			}
//		}
	}

	//#2 Auto Sign Out
//	DoAutoSignOut (SignOutKey);

	if (SignOutKey == CLASS_UICASHIEROUT) {
		CharData.uchASCII = 0xB0;   // FSC_SIGN_IN
		CharData.uchScan = 0x00;
		UiePutKeyData (2, &CharData, pResult);

		PifSleep(500);     // CasForceAutoSignOut() give UI thread time to process the keystroke
	}
	else if (SignOutKey == CLASS_UIB_OUT) {
		CharData.uchASCII = 0xB1;   // FSC_B
		CharData.uchScan = 0x00;
		UiePutKeyData (2, &CharData, pResult);

		PifSleep(500);     // CasForceAutoSignOut() give UI thread time to process the keystroke
	}

//	if(usMinorFsc == 1) {
//        UieQuitAndInitMenu(aUifReg);
//	}

	return SUCCESS;
}
/****** End of Source ******/
