/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Cashier sign out Item module                         
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: ITCASOUT.C
* --------------------------------------------------------------------------
* Abstract: ItemCasSignOut()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* May-29-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 4-92: 00.00.01  : K.Maeda   : Apply comments of code inspection held 
*                                    on Jun. 4                                   
* Jul-20-93: 01.00.12  : K.You     : fixed bug (mod. ItemCasRelOnFinDisp) 
* Apr-27-95: 03.00.00  : hkato     : R3.0
* Apr-19-99: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL
*          :           :           :       /GUEST_CHECK_MODEL
* Dec-16-99: 01.00.00  : hrkato    : Saratoga
** GenPOS
* Jun-15-15: 02.02.02  : rchambers : simplify code, localize variables
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
#include    <memory.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "appllog.h"
#include    "log.h"
#include    "rfl.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "cscas.h"
#include    "csstbcas.h"
#include    "csstbstb.h"
#include    "csttl.h"
#include    "csstbttl.h"
#include    "display.h"
#include    "mld.h"
#include    "fdt.h"
#include    "itmlocal.h"
#include    "BlFwIf.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemCasSignOut(UIFREGOPECLOSE *pUifRegOpeClose, ITEMOPECLOSE *pItemOpeClose)
*
*    Input: UIFREGOPECLOSE *pUifRegOpeClose, ITEMOPECLOSE *pItemOpeClose
*
*   Output: None
*
*    InOut: None
*
**Return: UIF_CAS_SIGNOUT : Return when the function performed successfully.
* 
*         UIF_SUCESS      : Return when cashier interrupt sign-out performed sucessfully
* 
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*
*         sReturnStatus : Return when TrnClose() returns error. 
*
** Description: This function executes cashier sign out function by cashier
*               sign in key or cashier B key.
*
*===========================================================================
*/
SHORT   ItemCasSignOut(UIFREGOPECLOSE *pUifRegOpeClose, ITEMOPECLOSE *pItemOpeClose)
{
    SHORT           sReturnStatus;
    USHORT          usSystemType = RflGetSystemType();
    TRANMODEQUAL    ModeQualRcvBuff = *TranModeQualPtr;   // make a copy to work from as will change
    SHORT           sFinalize = FALSE;

	sReturnStatus = ItemCommonCheckCashDrawerOpen ();
	if (sReturnStatus != 0)
		return sReturnStatus;

    /*--- VERIFY NO WAITER STILL SIGN IN ---*/
    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UIWAITERLOCKOUT: /* V3.3 */
        /* not return at this point */
        break;

    default:
        if (ModeQualRcvBuff.ulWaiterID != 0) {  /* waiter still sign in */
            return(LDT_SEQERR_ADR);             /* return sequence error */
        } 
    }

	// over ride the type if minor class is Drawer A
	if (pUifRegOpeClose->uchMinorClass == CLASS_UICASHIEROUT) {
		if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B) {
			pUifRegOpeClose->uchMinorClass = CLASS_UIB_OUT;
		}
	}

    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASHIEROUT: 
	    if (pUifRegOpeClose->fbInputStatus & INPUT_MSR) {
        	if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B) {
				pUifRegOpeClose->uchMinorClass = CLASS_UIB_OUT;
			}
		}
	}
	
    /*--- CHECK CONDITION, DEFINE MINOR CLASS ---*/
    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASHIEROUT: 
        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B) {    /* cashier B is sign in */
            return(LDT_SEQERR_ADR);                                    /* return sequence error */
        }

        /* V3.3 */
        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_WAITER_LOCK) {    /* sign-in by server lock */
            return(LDT_SEQERR_ADR);                                      /* return sequence error */
        }

        pItemOpeClose->uchMinorClass = CLASS_CASHIEROUT;
        break;

    case CLASS_UIB_OUT: 
        if (!(ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B)) {    /* cashier B is sign in */
            return(LDT_SEQERR_ADR);                                       /* return sequence error */
        }

        /* V3.3 */
        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_WAITER_LOCK) {    /* sign-in by server lock */
            return(LDT_SEQERR_ADR);                                      /* return sequence error */
        }

        pItemOpeClose->uchMinorClass = CLASS_B_OUT;

        break;

    case CLASS_UICASRELONFIN:               /* need to define the minor class */ 
        /* V3.3 */
        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_WAITER_LOCK) {  /* sign-in by server lock, key isn't removed */
            return( UIF_FINALIZE );                                    /* ignore release on finalize */
        }

        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B) {    /* signed in by B key ? */
            pItemOpeClose->uchMinorClass = CLASS_B_OUT;
        } else {                                                       /* signed in by A key ? */
            pItemOpeClose->uchMinorClass = CLASS_CASHIEROUT;
        }

        break;

    case CLASS_UICASINTOUT:     /* R3.3 */
        if ((ModeQualRcvBuff.fsModeStatus & MODEQUAL_CASINT) == 0) {  /* check whether sign-in is done actually, not sign-in */
            return(UIF_SUCCESS);
        }

        /* check msr sign-out */
        if ((pUifRegOpeClose->fbInputStatus & INPUT_MSR) && (ModeQualRcvBuff.ulCashierID == pUifRegOpeClose->ulID)) {
            /* if same cashier id is interred, complete sign-out */
            pUifRegOpeClose->uchMinorClass = CLASS_UICASHIEROUT;
            pItemOpeClose->uchMinorClass = CLASS_CASHIEROUT;
        } else {
            pItemOpeClose->uchMinorClass = CLASS_CASINTOUT;
        }
/***
        if (ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B) { 
                                            / cashier B is sign in /
            return(LDT_SEQERR_ADR);         / return sequence error /
        }
***/
        break;

    case CLASS_UICASINTB_OUT:     /* R3.3 */
        if ((ModeQualRcvBuff.fsModeStatus & MODEQUAL_CASINT) == 0) {   /* check whether sign-in is done actually, not sign-in */
            return(UIF_SUCCESS);
        }
/***
        if (!(ModeQualRcvBuff.fbCashierStatus & MODEQUAL_CASHIER_B)) { 
                                            / cashier B is sign in /
            return(LDT_SEQERR_ADR);         / return sequence error /
        }
***/
        pItemOpeClose->uchMinorClass = CLASS_CASINTB_OUT;
        break;

    case CLASS_UICASINTOUT2:    /* R3.3 */
        if ((ModeQualRcvBuff.fsModeStatus & MODEQUAL_CASINT) == 0) {    /* sign-out is done aleady */
            return(UIF_CAS_SIGNOUT);
        }
        pItemOpeClose->uchMinorClass = CLASS_CASHIEROUT;
        break;

    case CLASS_UIWAITERLOCKOUT: /* V3.3 */
        if ((ModeQualRcvBuff.fbCashierStatus & MODEQUAL_WAITER_LOCK) == 0) {   /* not sign-in by server lock */
            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT );                        /* display error and wait for clear key */   
            ItemOtherDiaRemoveKey();                                           /* dialogue function  to remove waiter key */
            ItemSignInClearDisplay(1);
            return(UIF_CANCEL);
        }

        sReturnStatus = ItemCasSignOutFinalize(&sFinalize);
        if (sReturnStatus != UIF_SUCCESS) {
			ULONG  ulId = 0;
            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT ); /* display error and wait for clear key */
            do {
                ulId = ItemOtherDiaInsertKey();                   /* dialogue function  to remove waiter key */
                if (ulId != ModeQualRcvBuff.ulCashierID) {
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT );  /* display error and wait for clear key */   
                    ItemOtherDiaRemoveKey();                      /* dialogue function  to remove waiter key */
                }
            } while (ulId != ModeQualRcvBuff.ulCashierID);
            ItemSignInClearDisplay(1);
            return(UIF_CANCEL);
        }

        /* sign-out surrogate before cashier sign-out */
        if (ModeQualRcvBuff.ulWaiterID) { 
            if ((sReturnStatus = ItemBarSignOut(pUifRegOpeClose, pItemOpeClose)) != UIF_WAI_SIGNOUT) {
                return(sReturnStatus);
            }
        }
        pItemOpeClose->uchMinorClass = CLASS_CASHIEROUT;
        break;

    default:
        break;
    }

    /*--- PREPARE DATA ---*/
    /* skip cashier sign-out process at error case of new cashier sign-in */
    pItemOpeClose->uchMajorClass = CLASS_ITEMOPECLOSE;
    pItemOpeClose->ulID = ModeQualRcvBuff.ulCashierID ;
    pItemOpeClose->uchUniqueAdr = CliReadUAddr();   /*--- GET TERMINAL UNIQUE ADDRESS ---*/
    TrnClose(pItemOpeClose);      /*--- CALL TRANSACTION MODULE TO CLOSE ---*/

    /* CLASS_UICASINTOUT2 is used at the error case of new cashier sign-in */
    if (pUifRegOpeClose->uchMinorClass != CLASS_UICASINTOUT2) {
        for (;;) {
			CASIF  CasData = {0};                            /* GCA */                    

            CasData.ulCashierNo = ModeQualRcvBuff.ulCashierID;
			memcpy (CasData.fbCashierStatus, ModeQualRcvBuff.auchCasStatus, PARA_LEN_CASHIER_STATUS);

            /* R3.3 */
            if ((sReturnStatus = CliCasPreSignOut(&CasData)) != CAS_PERFORM ) {
                /* wait for previous cashier sign-out for cashier interrupt */                
                /* UieErrorMsg(CasConvertError(sReturnStatus), UIE_WITHOUT); */
                CliSleep();
                continue;
            } else {
                break;
            }
        }
        for (;;) {
            if ((sReturnStatus = TtlConvertError(TrnSendTotal())) != TRN_SUCCESS) {    /* send sign out info. to trans. module */
                UieErrorMsg(sReturnStatus, UIE_WITHOUT);                               /* loop with error message while error condition */
            } else {
                break;
            }
        }
    }

     TrnInitialize(TRANI_ITEM | TRANI_CONSOLI | TRANI_SIGNINOUT);   /* clear strage buffer */

    /*--- INITIALIZE MODIFIER LOCAL DATA ---*/
    ItemCasOutInitModLocal();

    /*--- CHECK CONDITION, DEFINE MINOR CLASS ---*/
    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASINTOUT: 
    case CLASS_UICASINTB_OUT: 
        /* do not close transaction file, for continuous sign-in */
        break;

    default:
        /*--- MANIPURATE MODE CHANGABLE STATUS ---*/
        UieModeChange(UIE_ENABLE);              /* make mode changable condition */

        /* FDT Thread Close,   R3.0 */
        if (usSystemType == FLEX_STORE_RECALL) {
            FDTClose();
        }

        /* close item, cons. post rec. file */
        /* not close for post receipt, V3.3 
        TrnICPCloseFile();
        */

        /*----- Close Split File, R3.1 -----*/
        if (usSystemType == FLEX_POST_CHK &&
			! CliParaMDCCheckField(MDC_SPLIT_GCF_ADR, MDC_PARAM_BIT_B) &&
            ! CliParaMDCCheckField(MDC_GCNO_ADR, MDC_PARAM_BIT_A)) {
            TrnCloseFileSpl();
        }
    }

    /*--- DISPLAY SIGN OUT MESSAGE ---*/
    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASHIEROUT:
    case CLASS_UIB_OUT:
    case CLASS_UICASINTOUT2:
	case CLASS_UICASRELONFIN:
        ItemCasOutDisplay();                /* display sign out message */
        break;
 
//    case CLASS_UICASRELONFIN:               /* need to define the minor class */ 
//        ItemCasRelOnFinDisp();
//        break;
    
    case CLASS_UIWAITERLOCKOUT: /* V3.3 */
        if (sFinalize == TRUE) {
            ItemCasRelOnFinDisp();              /* keep tender display      */
        } else {
            ItemCasOutDisplay();                /* display sign out message */
        }
        break;

    default:
        break;                              /* CLASS_UICASRELONFIN */

    }

    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UIWAITERLOCKOUT: /* V3.3 */
        ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);  /* inhibit post receipt, V3.3 */
        break;

    default:
        break;
    }

	//The code below was added to allow a cashier to open the drawer on
	//Sign out.  Option can be set in AC20.  ***PDINU
	if (ModeQualRcvBuff.auchCasStatus[CAS_CASHIERSTATUS_2] & CAS_OPEN_CASH_DRAWER_SIGN_OUT) {
		USHORT  usAddress;
		CASIF   CasData = {0};                            /* GCA */                    

        CasData.ulCashierNo = ModeQualRcvBuff.ulCashierID;
		memcpy (CasData.fbCashierStatus, ModeQualRcvBuff.auchCasStatus, PARA_LEN_CASHIER_STATUS);

		/*----- get cashier No. set on A or B key -----*/
		if ((pUifRegOpeClose->uchMinorClass == CLASS_UICASHIERIN) || (pUifRegOpeClose->uchMinorClass == CLASS_UIWAITERLOCKIN)) {  /* V3.3 */
			usAddress = ASN_AKEY_ADR;        /* SIGNIN key address */
		} else {
			if (!(CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT0))) { //Not two drawers
				usAddress = ASN_AKEY_ADR;
			} else {
				usAddress = ASN_BKEY_ADR;     /* B key address */
			}
		}
		
		// attempt to open the drawer. whether it works or not we are commited to final sign-out.
		CasOpenDrawer(&CasData, usAddress);  //PDINU
	}

	// hide any group permission windows that should be hidden at Sign-out.
	// these windows are windows that show or hide based on the group permissions
	// of the operator so we want to hide everything so marked.
	BlFwIfGroupWindowHide (0xffffffff);
	BlFwIfWindowTranslate (0);
//	BlFwIfDisplayOEPImagery (0, 0, 0, 0);

	//The 0 time is set to kill the timer
	BlFwIfSetAutoSignOutTime (0, BLFWIF_AUTO_MODE_NO_CHANGE);//*****//

    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASINTOUT: 
    case CLASS_UICASINTB_OUT: 
        /* for next sign-in */
        return(UIF_SUCCESS);

    default:
        return(UIF_CAS_SIGNOUT);
    }
}
 

/*
*===========================================================================
**Synopsis: VOID   ItemCasOutDisplay(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   Nothing
* 
** Description: 
*               
*===========================================================================
*/
VOID   ItemCasOutDisplay(VOID)
{
	UIFREGMODEIN   UifRegModeInData = {0};

    /* make mode in data */
    UifRegModeInData.uchMajorClass = CLASS_UIFREGMODE; 
    UifRegModeInData.uchMinorClass = CLASS_UIREGMODEIN;
    ItemModeInDisplay(&UifRegModeInData);   /* display mode in message */
}

/*
*===========================================================================
**Synopsis: VOID   ItemCasRelOnFinDisp(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   Nothing
* 
** Description: 
*               
*===========================================================================
*/

VOID   ItemCasRelOnFinDisp(VOID)
{
	REGDISPMSG    DispMsg = {0}, DispMsgBack = {0};

    DispMsg.uchMajorClass = CLASS_UIFREGTENDER;
    DispMsg.uchMinorClass = CLASS_UITENDER1;
    DispMsg.fbSaveControl = 0x23;       /* 0x2X : don't change foreground     */
                                        /* 0xX2 : save type 3 for DispMsgBack */
    DispMsgBack.uchMajorClass = CLASS_UIFREGMODE;
    DispMsgBack.uchMinorClass = CLASS_UIREGMODEIN;
    RflGetLead (DispMsgBack.aszMnemonic, LDT_ID_ADR);

	if(!(flDispRegDescrControl & PM_CNTRL)) {	//Delayed Balance JHHJ
		flDispRegDescrControl = 0L;
	} else {
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	}
	if( !(flDispRegKeepControl & PM_CNTRL)) {
		flDispRegKeepControl = 0L;
	} else {
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	} 

    DispMsgBack.fbSaveControl = 3;      /* save for redisplay (release on finalize) */

    if (RflGetSystemType() == FLEX_STORE_RECALL) {
        MldDisplayInit(MLD_DRIVE_2 | MLD_DRIVE_3, 0);
    }

    DispWriteSpecial(&DispMsg, &DispMsgBack);
}


/*
*===========================================================================
**Synopsis: VOID   ItemCasOutInitModLocal(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   Nothing
* 
** Description: 
*               
*===========================================================================
*/
VOID   ItemCasOutInitModLocal(VOID)
{
	ITEMMODLOCAL  *pModDataRcvBuff = ItemModGetLocalPtr();         /* modifier module local data receive buffer */

    pModDataRcvBuff->fbModifierStatus &= ~MOD_NORECEIPT;  /* turn off "depressed" status */
    pModDataRcvBuff->fsTaxable = 0;                       /* clear taxable status */
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCasIntPreSignOut(UIFREGOPECLOSE *pUifRegOpeClose)
*
*    Input: UIFREGOPECLOSE *pUifRegOpeClose
*
*   Output: None
*
*    InOut: None
*
**Return: UIF_SUCESS      : Return when cashier interrupt sign-out performed sucessfully
* 
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*
** Description: This function executes drawer status check of cashier interrupt sign out
*
*===========================================================================
*/
SHORT   ItemCasIntPreSignOut(UIFREGOPECLOSE *pUifRegOpeClose)
{
    /*--- CHECK CONDITION, DEFINE MINOR CLASS ---*/
    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASINT_PREOUT: 
        if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_B) {      /* cashier B is sign in */
            return(LDT_SEQERR_ADR);         /* return sequence error */
        }
        break;

    case CLASS_UICASINTB_PREOUT: 
        if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_B)) {   /* cashier A is sign in */
            return(LDT_SEQERR_ADR);         /* return sequence error */
        }
        break;

    default:
        break;
    }

    return(UIF_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCasSignOutFinalize(VOID)
*
*    Input: None
*
*   Output: None
*
*    InOut: None
*
**Return: UIF_SUCESS      : Finalize Successed
* 
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*
** Description: This function executes the finalization by server lock, V3.3
*
*===========================================================================
*/
SHORT   ItemCasSignOutFinalize(SHORT *psFinalize)
{
    SHORT           sRetStatus;

    *psFinalize = FALSE;

    if (((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0) &&          /* itemize state */
        ((TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) == 0)) {
        /* if finalized, only sign-out */
        return(UIF_SUCCESS);
    }

    if (CliParaMDCCheckField (MDC_SIGNOUT_ADR, MDC_PARAM_BIT_D)) {                 
        /* not finalize option */
        return(UIF_CANCEL);
    }

    /* execute finalize */
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
		UIFREGTENDER    UifRegTender = {0};

        /* execute fast finalize by cash */
        UifRegTender.uchMajorClass = CLASS_UIFREGTENDER;    /* set tender class */
        UifRegTender.uchMinorClass = CLASS_UITENDER1;
        UifRegTender.lTenderAmount = 0L;    /* assume as fast finalize */

        sRetStatus = ItemTenderEntry(&UifRegTender); /* Tender Modele */
        switch (sRetStatus) {
        case UIF_SUCCESS:                               /* success */
        case UIF_FINALIZE:                              /* finalize */
        case UIF_CAS_SIGNOUT:                           /* cashier sign out */
        case UIF_WAI_SIGNOUT:                           /* waiter sign out */
            *psFinalize = TRUE;
            sRetStatus = UIF_SUCCESS;
            break;
        default:
            break;
        }
    } else {
		/* non-guest check transaction, 8/9/99 */
		/* execute GCF close by service total, 50RFC05513 */
		UIFREGTOTAL     UifRegTotal = {0};

        UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;  /* set total class */
        UifRegTotal.uchMinorClass = CLASS_UIKEYOUT;
        sRetStatus = ItemTotal(&UifRegTotal);   /* Total Modele */
        switch (sRetStatus) {
        case UIF_SUCCESS:                               /* success */
        case UIF_FINALIZE:                              /* finalize */
        case UIF_CAS_SIGNOUT:                           /* cashier sign out */
        case UIF_WAI_SIGNOUT:                           /* waiter sign out */
            *psFinalize = TRUE;
            sRetStatus = UIF_SUCCESS;
            break;
        default:
            break;
        }
    }

    return (sRetStatus);
}

/*
*===========================================================================
**Synopsis: VOID   ItemCasAutoOut(VOID)
*
*    Input: None
*
*   Output: None
*
*    InOut: None
*
**Return: UIF_SUCESS      : Finalize Successed
* 
*
** Description: 
*
*===========================================================================
*/
VOID	ItemCasAutoOut(VOID)
{
	ULONG	SignOutTime = CliParaMDCCheckField(MDC_AUTO_SIGNOUT_ADR, MDC_PARAM_BYTE);

	///	Need to format time from minutes to be in Miliseconds		
	SignOutTime = ((SignOutTime * 60) * 1000);
	if(SignOutTime != 0){
		//The 0 time is set to kill the timer
		BlFwIfSetAutoSignOutTime (SignOutTime, BLFWIF_AUTO_MODE_NO_CHANGE);//*****//
	}
}

/*
*===========================================================================
**Synopsis: VOID   CasAutoSignOutReset(VOID)
*
*    Input: None
*
*   Output: None
*
*    InOut: None
*
**Return: UIF_SUCESS      : Finalize Successed
* 
*
** Description: 
*
*===========================================================================
*/
VOID	CasAutoSignOutReset()
{
	if(TranModeQualPtr->ulCashierID != 0){
		ULONG    SignOutTime = CliParaMDCCheckField(MDC_AUTO_SIGNOUT_ADR, MDC_PARAM_BYTE);
		
		///	Need to format time from minutes to be in Miliseconds		
		SignOutTime = ((SignOutTime * 60) * 1000);
		if (SignOutTime != 0) {
			//The 0 time is set to kill the timer
			BlFwIfSetAutoSignOutTime (SignOutTime, BLFWIF_AUTO_MODE_NO_CHANGE);//*****//
		}
	}
}

/****** End of Source ******/