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
* Title       : Sign In Item module                         
* Category    : Item Module, NCR 2170 Hsopitality US Model Application         
* Program Name: ITSIGNIN.C
* --------------------------------------------------------------------------
* Abstract: ItemSignIn()            : Operator sign in function main

* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* May-06-92: 00.00.01  : K.Maeda   : initial                                   
* Nov-14-95: 03.01.00  : hkato     : R3.1
* Aug-11-99: 03.05.00  : M.Teraki  : Remove WAITER_MODEL
* Apr-19-99: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL
*          :           :           :       /GUEST_CHECK_MODEL
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
#include    <stdio.h>

#include    "ecr.h"
#include    "log.h"
#include    "pif.h"
#include    "uie.h"
#include    "pmg.h"
#include    "rfl.h"
#include    "fsc.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "uireg.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "cscas.h"
#include    "csstbcas.h"
#include    "cswai.h"
#include    "csstbwai.h"
#include    "csstbstb.h"
#include    "display.h"
#include    "mld.h"
#include    "appllog.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "fdt.h"
#include    "enhkds.h"
#include    "pifmain.h"
#include    "itmlocal.h"

#include    <BlFwIf.h>

#include	<ConnEngineObjectIf.h>

ITEMOPEOPEN * ItemOpeAssignCasifToItemOpeOpen (UCHAR uchUifRegOpeOpenMinorClass, ITEMOPEOPEN *pItemOpeOpen, CASIF *pCasData)
{
	memset (pItemOpeOpen, 0, sizeof(*pItemOpeOpen));

	pItemOpeOpen->uchMajorClass = CLASS_ITEMOPEOPEN;

	switch (uchUifRegOpeOpenMinorClass) {
		case CLASS_UICASHIERIN:                               /* A key cashier sign-in, R3.3 */
		case CLASS_UIWAITERIN:                                /* A key waiter sign-in, R3.3 */
			pItemOpeOpen->uchMinorClass = CLASS_CASHIERIN;
			break;

		case CLASS_UIWAITERLOCKIN:                            /* A key waiter sign-in, R3.3 */
			pItemOpeOpen->uchMinorClass = CLASS_CASHIERIN;
			pItemOpeOpen->fbCashierStatus |= MODEQUAL_WAITER_LOCK;
			break;

		case CLASS_UIB_IN:
			pItemOpeOpen->uchMinorClass = CLASS_B_IN;          /* B key cashier sign-in, R3.3 */
			pItemOpeOpen->fbCashierStatus |= MODEQUAL_CASHIER_B;      /* B key class */
            pItemOpeOpen->fbCashierStatus |= MODEQUAL_DRAWER_B;       /* assign drawer B to the cahier */
			break;

		case CLASS_UICASINTIN:                                /* A key cashier interrupt sign-in, R3.3 */
			pItemOpeOpen->uchMinorClass = CLASS_CASINTIN;
			break;

		case CLASS_UICASINTB_IN :                              /* B key cashier interrupt sign-in, R3.3 */
			pItemOpeOpen->uchMinorClass = CLASS_CASINTB_IN;
			pItemOpeOpen->fbCashierStatus |= MODEQUAL_CASHIER_B;      /* B key class */
            pItemOpeOpen->fbCashierStatus |= MODEQUAL_DRAWER_B;       /* assign drawer B to the cahier */
			break;

		case CLASS_UIBARTENDERIN :                             /* bar tender or surrogate sign-in, R3.3 */
			pItemOpeOpen->uchMinorClass = CLASS_BARTENDERIN;
			break;

		default:
			NHPOS_ASSERT_TEXT(0, "==ERROR: ItemOpeAssignCasifToItemOpeOpen() unknown uchUifRegOpeOpenMinorClass value.");
			pItemOpeOpen->uchMinorClass = CLASS_CASHIERIN;
			break;
	}

    pItemOpeOpen->ulID = pCasData->ulCashierNo;
    pItemOpeOpen->ulSecret = pCasData->ulCashierSecret;
    _tcsncpy(pItemOpeOpen->aszMnemonic, pCasData->auchCashierName, PARA_CASHIER_LEN);
    memcpy(pItemOpeOpen->auchStatus, pCasData->fbCashierStatus, PARA_LEN_CASHIER_STATUS);
    pItemOpeOpen->usFromTo[0] = pCasData->usGstCheckStartNo;      /* start of the gcf No. range */
    pItemOpeOpen->usFromTo[1] = pCasData->usGstCheckEndNo;        /* end of the gcf No. range */
    pItemOpeOpen->uchChgTipRate = pCasData->uchChgTipRate;
    pItemOpeOpen->uchTeamNo   = pCasData->uchTeamNo;
    pItemOpeOpen->usSupervisorID   = pCasData->usSupervisorID;
    pItemOpeOpen->usCtrlStrKickoff   = pCasData->usCtrlStrKickoff;
    pItemOpeOpen->usStartupWindow   = pCasData->usStartupWindow;
	pItemOpeOpen->ulGroupAssociations = pCasData->ulGroupAssociations;

    /*----- Correct drawer assigned to the cashier if not a two drawer terminal -----*/
    if (! CliParaMDCCheckField (MDC_DRAWER_ADR, MDC_PARAM_BIT_D)) {      /* not two drawer */
        pItemOpeOpen->fbCashierStatus &= ~ MODEQUAL_DRAWER_B;                   /* always assign drawer A to the cashier if not two drawer system */
    }

    /* set training status for either cashier or waiter or bar tender sign-in */
    if (pCasData->fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER) {
        pItemOpeOpen->fbCashierStatus |= MODEQUAL_CASHIER_TRAINING;  // also sets MODEQUAL_WAITER_TRAINING since same value
    }

	return pItemOpeOpen;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSignIn(UIFREGOPEOPEN *pUifRegOpeOpen)
*
*    Input: UIFREGOPEOPEN *pUifRegOpeOpen
*
*   Output: None
*
*    InOut: None
*
**Return: sReturnStatus
*
** Description: This function executes cashier sign in function by cashier
                sign in key or cashier b key.
*===========================================================================
*/
SHORT   ItemSignIn(UIFREGOPEOPEN *pUifRegOpeOpen)
{
    SHORT         sReturnStatus = LDT_PROHBT_ADR;
	ITEMOPEOPEN   ItemOpeOpen = {0};     /* ITEMOPEOPEN SignInData*/

#ifdef DEMO_VERSION
	if(pUifRegOpeOpen->ulID != 1 && pUifRegOpeOpen->ulID !=2){
		return LDT_PROHBT_ADR;
	}
#endif

    /* call functions */
    switch(pUifRegOpeOpen->uchMinorClass) {
    case CLASS_UICASHIERIN:
    case CLASS_UIB_IN:
    case CLASS_UIWAITERLOCKIN:              /* waiter lock sign in, V3.3 */
        sReturnStatus = ItemCasSignIn(pUifRegOpeOpen, &ItemOpeOpen);
        break;

/* #ifdef  GUEST_CHECK_MODEL */
    case CLASS_UIBARTENDERIN:       /* R3.1 */
        sReturnStatus = ItemBarNoKeyIn(pUifRegOpeOpen, &ItemOpeOpen);
        break;
   
    case CLASS_UICASINTIN:          /* R3.3 */
    case CLASS_UICASINTB_IN:        /* R3.3 */
        sReturnStatus = ItemCasIntSignIn(pUifRegOpeOpen, &ItemOpeOpen);
        break;
/* #endif */
    default:
        break;
    }

    /* V3.3 */
    switch(pUifRegOpeOpen->uchMinorClass) {
    case CLASS_UIWAITERLOCKIN:              /* waiter lock sign in, V3.3 */
        /* error case of server lock sign-in */
        if (sReturnStatus != UIF_SUCCESS) {
            if ((sReturnStatus != UIF_CANCEL) && (sReturnStatus != ITM_CONT)) {     /* skip, if already canceled */
                UieErrorMsg( sReturnStatus, UIE_WITHOUT );  /* display error and wait for clear key */   
            }

            if (sReturnStatus != ITM_CONT) {                /* skip, if server lock is already removed */
                ItemOtherDiaRemoveKey();                    /* dialogue function  to remove waiter key */
                ItemSignInClearDisplay(0);
            }
            return(UIF_CANCEL);
        }
        break;

    case CLASS_UIBARTENDERIN:       /* R3.1 */
        if (sReturnStatus != UIF_SUCCESS) {
            ItemSignInClearDisplay(1);
        }
        break;

    default:
        if (sReturnStatus != UIF_SUCCESS) {
            ItemSignInClearDisplay(0);
        }
    }

#ifdef DEMO_VERSION
	if(pUifRegOpeOpen->ulID != 1 && pUifRegOpeOpen->ulID !=2){
		return LDT_PROHBT_ADR;
	}
#endif

    /* open item, cons. post rec. file */
	if (sReturnStatus == UIF_SUCCESS && TrnICPOpenFile() == TRN_SUCCESS) {
		TRANCURQUAL     CurQualWork = {0};
		ITEMMISC        ItemMisc = {0};
		ITEMPRINT       ItemPrintData = {0};      
		ITEMTRANSCLOSE  ItemTransClose = {0};

		
		TrnPutCurQual(&CurQualWork);    /* initialize memory resident database to zeros with prefilled CurQualWork */

		{
			ITEMCOMMONLOCAL *pItemCommonLocal = ItemCommonGetLocalPointer();

			pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO | COMMON_VOID_EC);  // Clear the Connsecutive number flag to indicate it needs to be generated
		}

		/* consecutive no. and current qualifier count up on */
		ItemCountCons();
		CliParaSaveFarData();        // Update the consecutive number persistent store

		/* set option to current qualifier(current print status) */
		ItemCurPrintStatus();

		/* start print service */
		ItemPrintStart(TYPE_POSTCHECK);     /* store only item consoli. buffer */     

		ItemMisc.ulEmployeeNo = ItemPrintData.ulID = ItemMisc.ulID = ItemOpeOpen.ulID;
		_tcsncpy (ItemMisc.aszMnemonic, ItemOpeOpen.aszMnemonic, sizeof(ItemMisc.aszMnemonic)/sizeof(ItemMisc.aszMnemonic[0]) - 1);

		ItemPrintData.usConsNo = CurQualWork.usConsNo;

		{
			DATE_TIME DateTime;          /* for TOD read */

			PifGetDateTime(&DateTime);
			ItemMisc.usYear  = DateTime.usYear;
			ItemMisc.usMonth = DateTime.usMonth;
			ItemMisc.usDay   = DateTime.usMDay;
			ItemMisc.uchJobCode = 0;
			ItemMisc.usTimeinTime = DateTime.usHour;
			ItemMisc.usTimeinMinute = DateTime.usMin;
			ItemMisc.usTimeOutTime   = 0;
			ItemMisc.usTimeOutMinute = 0;
		}

		/* set class */
		ItemMisc.uchMajorClass = CLASS_ITEMMISC;
		ItemMisc.fbStorageStatus |= NOT_CONSOLI_STORAGE;
		ItemMisc.uchMinorClass = CLASS_SIGNIN;
		UifEmployeeSignTimeMethodString (pUifRegOpeOpen->ulStatusFlags, ItemMisc.aszNumber);

		ItemMisc.fsPrintStatus = ( PRT_JOURNAL);    /* set R/J portion */
		TrnItem(&ItemMisc);

		/* print trailer (only receipt and journal) */
		ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;      
		ItemPrintData.uchMinorClass = CLASS_TRAILER3;       
		ItemPrintData.fsPrintStatus = (PRT_JOURNAL);
	    
		TrnGetCurQual(&CurQualWork);
		ItemPrintData.ulStoreregNo = CurQualWork.ulStoreregNo;
		ItemPrintData.fbStorageStatus |= NOT_CONSOLI_STORAGE;

		TrnItem(&ItemPrintData);

		/* close transaction */
		TrnGetCurQual( &CurQualWork );  
		CurQualWork.uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
		if (  (CurQualWork.flPrintStatus & CURQUAL_GC_SYS_MASK ) != CURQUAL_PRE_UNBUFFER ) {
			CurQualWork.uchPrintStorage = PRT_ITEMSTORAGE;
		}
		CurQualWork.uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */
		TrnPutCurQual( &CurQualWork );

		ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
		ItemTransClose.uchMinorClass = CLASS_CLSETK;
		fsPrtCompul= 0;  // ensure that we only do an EJ print
		TrnClose( &ItemTransClose );

		{
			TCHAR aszSapId[16];
			_itot (ItemOpeOpen.ulID, aszSapId, 10);

			ConnEngineSendCashierActionWithAddon (CONNENGINE_ACTIONTYPE_SIGNIN, aszSapId, 0);
			{
				char  xBuff[128];

				sprintf ( xBuff, "==ACTION: Cashier Sign-in %8.8u", ItemOpeOpen.ulID);
				NHPOS_NONASSERT_NOTE("==ACTION", xBuff);
			}
		}

		{
			ITEMCOMMONLOCAL *pItemCommonLocal = 0;

			ItemCommonGetLocal(&pItemCommonLocal);
			pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO);  // Clear the Connsecutive number flag to indicate it needs to be generated
		}

		/* finalize: do not clear TRANI_MODEQUAL since it contains the Operator Id
		 *           clearing the Operator Id results in Ask #3 not working as well as improper Cashier Totals.
		 */
		TrnInitialize(TRANI_CURQUAL | TRANI_ITEM); 
		ItemInit();
	}

    return(sReturnStatus);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemCasSignIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen)
*
*    Input: UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   SHORT sReturnStatus : ITM_SUCCESS or Leadthrough nemonic address
*                                 for error condition.
*
** Description: Cashier sign in module
*===========================================================================
*/
SHORT   ItemCasSignIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen)
{
    ULONG              ulSecret = 0;
	ULONG			   SignOutTime = 0;
    PARACASHABASSIGN   CasNoBuff;   
	CASIF              CasData = {0}; 
    TRANMODEQUAL       *pModeQualBuff = TrnGetModeQualPtr();
    SHORT              sReturnStatus;

	// Asserts for array data types are the same for memcpy, memset, _tcsncpy, _tcsnset, etc
	NHPOS_ASSERT(sizeof(pItemOpeOpen->auchStatus) == sizeof(CasData.fbCashierStatus));

#ifdef DEMO_VERSION
	if(pUifRegOpeOpen->ulID != 1 && pUifRegOpeOpen->ulID !=2){
		return LDT_PROHBT_ADR;
	}
#endif

    /* V3.3 */
    if (pUifRegOpeOpen->uchMinorClass == CLASS_UIWAITERLOCKIN) {
        if( TranModeQualPtr->ulCashierID != 0 ) {
            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
            ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */

            /* another server lock is inserted at the dialog function */
            if (TranModeQualPtr->fbCashierStatus & MODEQUAL_WAITER_LOCK) {
				LONG  lReturnStatus = 0;
                do {
                    lReturnStatus = ItemOtherDiaInsertKey();                    /* dialogue function  to remove waiter key */
                    if (lReturnStatus != TranModeQualPtr->ulCashierID) {
                        UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
                        ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */
                    }
                } while (lReturnStatus != TranModeQualPtr->ulCashierID);
            }
            ItemSignInClearDisplay(1);
            return(ITM_CONT);
            /* return(UIF_CANCEL); */
        }
    }
    /*----- ignore modifier key -----*/
    ItemSignInIgnoreModifierKey();

    /*----- get cashier No. set on A or B key -----*/
    /* set data of CasNoBuff */
    if ((pUifRegOpeOpen->uchMinorClass == CLASS_UICASHIERIN) || (pUifRegOpeOpen->uchMinorClass == CLASS_UIWAITERLOCKIN)) {  /* V3.3 */
        CasNoBuff.uchAddress = ASN_AKEY_ADR;                        /* SIGNIN key address */
    } else {
        CasNoBuff.uchAddress = ASN_BKEY_ADR;                        /* B key address */
    }
    CasNoBuff.uchMajorClass = CLASS_PARACASHABASSIGN;

    /* get cashier No. set on A or B key */
    CliParaRead(&CasNoBuff);

    /* not check secret no. if secret no. is not set on msr card */
//    if (pUifRegOpeOpen->fbInputStatus & INPUT_MSR) {
//        ulSecret = pUifRegOpeOpen->ulPin;
//   }

    /*----- get cashier No. and Secret Code -----*/
    if (! CliParaMDCCheckField(MDC_CASIN_ADR, FLAG_MDC_BITD)) {    /* use cashier ID */ 
        if (pUifRegOpeOpen->ulID == 0L) {        
            return(LDT_SEQERR_ADR);                         /* operation sequence error if no cashier ID */
        }

        CasData.ulCashierNo = pUifRegOpeOpen->ulID;    /* set cashier no */

        if ( (CasNoBuff.ulCashierNo != 0) && (CasNoBuff.ulCashierNo != CasData.ulCashierNo) ) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  AC7 nonzero and AC7 setting prohibits Cashier Id.");
            return(LDT_PROHBT_ADR);         /* prohibit operation error */
        }
    } else {                                /* not use cashier ID */
        if (pUifRegOpeOpen->ulID != 0L) {        
            return(LDT_SEQERR_ADR);         /* operation sequence error */
        }

        if (CasNoBuff.ulCashierNo == 0) {
            return(LDT_KEYOVER_ADR);        /* return key over limitation error  */
        }

        CasData.ulCashierNo = CasNoBuff.ulCashierNo;
    }

    CasData.ulCashierSecret = ulSecret;
    CasData.ulCashierOption = 0;

    /*----- get cashier parameters -----*/
    if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM ) {
        if (sReturnStatus == CAS_REQUEST_SECRET_CODE) {
            /* dialog of secret no */
            if ((sReturnStatus = ItemCasSecretCode(&ulSecret)) == ITM_SUCCESS) {
                CasData.ulCashierSecret = ulSecret;

                /*----- get cashier parameters -----*/
                if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM ) {
                    return(CasConvertError(sReturnStatus));
                }
            } else {
                return(sReturnStatus);
            }
        } else {
            return(CasConvertError(sReturnStatus));
        }
    }                   
    
    /* drawer status of operator, 2172 */
    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_A) {
        if (pUifRegOpeOpen->uchMinorClass == CLASS_UICASHIERIN) {
		    if (pUifRegOpeOpen->fbInputStatus & INPUT_MSR) {
				pUifRegOpeOpen->uchMinorClass = CLASS_UIB_IN;
			} else {
	            ItemCasSignInCancel(&CasData);
	            return(LDT_PROHBT_ADR);
	        }
        }
    }
    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_B) {
        if (pUifRegOpeOpen->uchMinorClass == CLASS_UIB_IN) {
            ItemCasSignInCancel(&CasData);
            return(LDT_PROHBT_ADR);
        }
    }

	if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_2] & CAS_OPEN_CASH_DRAWER_SIGN_IN)
	{
		/*----- get cashier No. set on A or B key -----*/
		/* set data of CasNoBuff */
		if ((pUifRegOpeOpen->uchMinorClass == CLASS_UICASHIERIN) || (pUifRegOpeOpen->uchMinorClass == CLASS_UIWAITERLOCKIN)) {  /* V3.3 */
			CasNoBuff.uchAddress = ASN_AKEY_ADR;                   /* SIGNIN key address */
		} else {
			if (!(CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT0))) {  //Not two drawers
				CasNoBuff.uchAddress = ASN_AKEY_ADR;
			} else {
				CasNoBuff.uchAddress = ASN_BKEY_ADR;                  /* B key address */
			}
		}
		
		if (CasOpenDrawer(&CasData,CasNoBuff.uchAddress) == CAS_NOT_ALLOWED)  //PDINU
			return(LDT_PROHBT_ADR);
	}

	/*---- test for disallowed manual signin when MSR signin is required ----*/
	if((CasData.fbCashierStatus[CAS_CASHIERSTATUS_2] & CAS_MSR_MANUAL_SIGN_IN) && (pUifRegOpeOpen->fbInputStatus != INPUT_MSR)) // JMASON
	{
		if ( ItemSPVInt(LDT_SUPINTR_ADR) != ITM_SUCCESS )                         //success
			return( UIF_CANCEL );
	}

    /*----- prepair interface data for transaction module  -----*/
	ItemOpeAssignCasifToItemOpeOpen (pUifRegOpeOpen->uchMinorClass, pItemOpeOpen, &CasData);

    /* make interface with transaction module  */
    if ((sReturnStatus = TrnOpen(pItemOpeOpen)) != ITM_SUCCESS) {     /* no operation except debugging */
        ItemCasSignInCancel(&CasData);
        return(sReturnStatus);
    } 

    /* open item, cons. post rec. file */
    if (TrnICPOpenFile() != TRN_SUCCESS) {
        ItemCasSignInCancel(&CasData);
        return(LDT_PROHBT_ADR);
    }

    /*----- Open Split File,  R3.1 -----*/
    if (RflGetSystemType () == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
        if (TrnOpenFileSpl() != TRN_SUCCESS) {
            ItemCasSignInCancel(&CasData);
            return(LDT_PROHBT_ADR);
        }
    }

    /*----- set MISC data -----*/
    ItemSignInPutMiscData();

	/*----- Set up Auto Sign Out  -----*/
	SignOutTime = CliParaMDCCheckField(MDC_AUTO_SIGNOUT_ADR, MDC_PARAM_BYTE);
	SignOutTime = ((SignOutTime * 60) * 1000);   	/*	Need to format time from minutes to be in Miliseconds		*/
	BlFwIfSetAutoSignOutTime (SignOutTime, BLFWIF_AUTO_MODE_NO_CHANGE);

	// hide/display any group permission windows that should be hidden/revealed at Sign-in.
	// these windows are windows that show or hide based on the group permissions
	// of the operator so we want to hide everything so marked.
	// we first hide everything and then show only those things this operator
	// should be allowed to see.
	BlFwIfGroupWindowHide (0xffffffff);
	BlFwIfGroupWindowShow (CasData.ulGroupAssociations);
//	BlFwIfWindowTranslate (1);
//	BlFwIfDisplayOEPImagery (2, 0, 0, 0);

    RflGetSetDecimalCommaType();    /* set decimal point, 2172 */

    /*----- display sign in data  -----*/
    ItemSignInDisplay(pUifRegOpeOpen, pItemOpeOpen, 0);

    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);  /* inhibit post receipt, V3.3 */

    /* start enhanced kds service, 2172 */
    KdsOpen();

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemBarNoKeyIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pWaiSignInData)
*
*    Input: UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pWaiSignInData
*   Output: Nothing
*    InOut: Nothing
*
**Return:   sReturnStatus
*
** Description: Bartender sign in module                 R3.1
*===========================================================================
*/
SHORT   ItemBarNoKeyIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen)
{
    SHORT          sReturnStatus;
	CASIF          CasData = {0};    
    ULONG          ulSecret = 0;

#ifdef DEMO_VERSION
	if(pUifRegOpeOpen->ulID != 1 && pUifRegOpeOpen->ulID !=2){
		return LDT_PROHBT_ADR;
	}
#endif

    if (RflGetSystemType () == FLEX_STORE_RECALL) {
        return(LDT_PROHBT_ADR);             
    }

    /*--- Check Cashier & Waiter Sign-in ---*/
    if (TranModeQualPtr->ulCashierID == 0) {
        return(LDT_PROHBT_ADR);             
    }
    if (TranModeQualPtr->ulWaiterID != 0) {
        return(LDT_PROHBT_ADR);             
    }

    /*--- IGNORE MODIFIER KEY ---*/
    ItemSignInIgnoreModifierKey();

    /* not check secret no. if secret no. is not set on msr card */
    if (pUifRegOpeOpen->fbInputStatus & INPUT_MSR) {
        ulSecret = pUifRegOpeOpen->ulPin;
    }

    CasData.ulCashierNo = pUifRegOpeOpen->ulID;   
    CasData.ulCashierSecret = ulSecret;
    CasData.ulCashierOption |= CAS_WAI_SIGN_IN;

    /*----- get waiter parameters -----*/
    if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM) {
        if (sReturnStatus == CAS_REQUEST_SECRET_CODE) {
            /* dialog of secret no */
            if ((sReturnStatus = ItemCasSecretCode(&ulSecret)) == ITM_SUCCESS) {
                CasData.ulCashierSecret = ulSecret;

                /*----- get cashier parameters -----*/
                if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM ) {
                    return(CasConvertError(sReturnStatus));
                }
            } else {
                return(sReturnStatus);
            }
        } else {
            return(CasConvertError(sReturnStatus));
        }
    }

    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_SURROGATE) {
        ItemCasSignInCancel(&CasData);
        return(LDT_PROHBT_ADR);
    }

    if ((CasData.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER) ^ (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
        ItemCasSignInCancel(&CasData);
        return(LDT_PROHBT_ADR);
    }

    /*----- Supervisor Intervention -----*/
    if (CliParaMDCCheck(MDC_WTIN_ADR, EVEN_MDC_BIT3)) {
        if (ItemSPVInt(LDT_SUPINTR_ADR) == UIF_CANCEL) {
           ItemCasSignInCancel(&CasData);
           return(UIF_CANCEL);
        }
    }

    /*----- prepare interface data for transaction module  -----*/
	ItemOpeAssignCasifToItemOpeOpen (pUifRegOpeOpen->uchMinorClass, pItemOpeOpen, &CasData);

    /* make interface with transaction module  */
    TrnOpen(pItemOpeOpen);  

#if 0
	// hide/display any group permission windows that should be hidden/revealed at Sign-in.
	// these windows are windows that show or hide based on the group permissions
	// of the operator so we want to hide everything so marked.
	// we first hide everything and then show only those things this operator
	// should be allowed to see.
	BlFwIfGroupWindowHide (0xffffffff);
	BlFwIfGroupWindowShow (CasData.ulGroupAssociations);
//	BlFwIfWindowTranslate (1);
#endif

	/*--- DISPLAY SIGN ON MESSAGE ---*/
    ItemSignInDisplay(pUifRegOpeOpen, pItemOpeOpen, 0);

    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);  /* inhibit post receipt, V3.3 */

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCasIntSignIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen)
*
*    Input: UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   SHORT sReturnStatus : ITM_SUCCESS or Leadthrough nemonic address
*                                 for error condition.
*
** Description: Cashier interupt sign in module, R3.3
*===========================================================================
*/
SHORT   ItemCasIntSignIn(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pItemOpeOpen)
{
    ULONG           ulSecret = 0;
    SHORT           sReturnStatus;
	USHORT          fsModeStatusSave;
	USHORT          usSystemType = RflGetSystemType ();
	CASIF           CasData = {0}; 
    TRANMODEQUAL    *pModeQualRcvBuff = TrnGetModeQualPtr();

#ifdef DEMO_VERSION
	if(pUifRegOpeOpen->ulID != 1 && pUifRegOpeOpen->ulID !=2){
		return LDT_PROHBT_ADR;
	}
#endif

    /*
	 *  read mode qualifier at first to capture a snapshot of the state.
	 *  as we do the operator setup, some of the mode qualifier data will change
	 *  in the memory resident database however we want to work with this copy
	 *  of the data to ensure that repeated use of the operator interrupt will
	 *  work correctly.
	*/
	fsModeStatusSave = pModeQualRcvBuff->fsModeStatus;

    /*----- ignore modifier key -----*/
    ItemSignInIgnoreModifierKey();

    /* prohibit at non GCF, precheck unbuffering, store/recall system */
    if (( RflGetMaxRecordNumberByType(FLEX_GC_ADR) == 0 ) ||
        ( usSystemType == FLEX_PRECHK_UNBUFFER ) ||
        ( usSystemType == FLEX_STORE_RECALL )) {
        return( LDT_PROHBT_ADR );
    }

    /* not check secret no. if secret no. is not set on msr card */
    if (pUifRegOpeOpen->fbInputStatus & INPUT_MSR) {
        ulSecret = pUifRegOpeOpen->ulPin;
    }

    CasData.ulCashierNo = pUifRegOpeOpen->ulID;     /* set cashier no */
    CasData.ulCashierSecret = ulSecret;
    CasData.ulCashierOption  = 0;

    /*----- get cashier parameters -----*/
    if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM ) {
        if (sReturnStatus == CAS_REQUEST_SECRET_CODE) {
            /* dialog of secret no */
            if ((sReturnStatus = ItemCasSecretCode(&ulSecret)) == ITM_SUCCESS) {
                CasData.ulCashierSecret = ulSecret;

                /*----- get cashier parameters -----*/
                if ((sReturnStatus = CliCasSignIn(&CasData)) != CAS_PERFORM ) {
                    return(CasConvertError(sReturnStatus));
                }
            } else {
                return(sReturnStatus);
            }
        } else {
            return(CasConvertError(sReturnStatus));
        }
    }                   

    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_CAS_INT) {
        ItemCasSignInCancel(&CasData);
        return(LDT_PROHBT_ADR);
    }

    /* drawer status of operator, 2172 */
    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_A) {
        if (pUifRegOpeOpen->uchMinorClass == CLASS_UICASINTIN) {
		    if (pUifRegOpeOpen->fbInputStatus & INPUT_MSR) {
				pUifRegOpeOpen->uchMinorClass = CLASS_UICASINTB_IN;
			} else {
	            ItemCasSignInCancel(&CasData);
    	        return(LDT_PROHBT_ADR);
    	    }
        }
    }

    if (CasData.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_B) {
        if (pUifRegOpeOpen->uchMinorClass == CLASS_UICASINTB_IN) {
            ItemCasSignInCancel(&CasData);
            return(LDT_PROHBT_ADR);
        }
    }

    /*----- prepair interface data for transaction module  -----*/
	ItemOpeAssignCasifToItemOpeOpen (pUifRegOpeOpen->uchMinorClass, pItemOpeOpen, &CasData);

    /* make interface with transaction module  */
    if ((sReturnStatus = TrnOpen(pItemOpeOpen)) != ITM_SUCCESS) {  /* no operation except debugging */
        return(sReturnStatus);
    } 

    /* if not sign-in previously */
    if ((fsModeStatusSave & MODEQUAL_CASINT) == 0) {
        /* open item, cons. post rec. file */
        if (TrnICPOpenFile() != TRN_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }

        /*----- Open Split File,  R3.1 -----*/
        if (usSystemType == FLEX_POST_CHK &&
            ! CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2) &&
            ! CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
            if (TrnOpenFileSpl() != TRN_SUCCESS) {
                return(LDT_PROHBT_ADR);
            }
        }

        /* start enhanced kds service, 2172 */
        KdsOpen();
    }

    ItemSignInPutMiscData();       /*----- set MISC data -----*/
	RflGetSetDecimalCommaType();   /* set decimal point, 2172 */

#if 0
	// hide/display any group permission windows that should be hidden/revealed at Sign-in.
	// these windows are windows that show or hide based on the group permissions
	// of the operator so we want to hide everything so marked.
	// we first hide everything and then show only those things this operator
	// should be allowed to see.
	BlFwIfGroupWindowHide (0xffffffff);
	BlFwIfGroupWindowShow (CasData.ulGroupAssociations);
//	BlFwIfWindowTranslate (1);
#endif

	ItemSignInDisplay(pUifRegOpeOpen, pItemOpeOpen, fsModeStatusSave);    /*----- display sign in data  -----*/
    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);  /* inhibit post receipt, V3.3 */

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID    ItemSignInPutMiscData(VOID)
*
*    Input: Nothing 
*
*   Output: TrnQualData.fsModeStatus : Receipt/ No Rec. default status
*                                    : Tax system status (US or Canadian)
*           Mode changeable status of U.I Engine
*
*    InOut: None
*
**Return:   ITM_SUCCESS : Function performed successfully. 
*
** Description: This function turns on or off the above two Mode qualifier
                status bits. And always returns, ITM_SUCCESS, success status.
*===========================================================================
*/
VOID    ItemSignInPutMiscData(VOID)
{
    TRANMODEQUAL       *pModeQualRcvBuff = TrnGetModeQualPtr();   
    ITEMSALESLOCAL     *pSalesLocalRcvBuff = ItemSalesGetLocalPointer();   
    PARACTLTBLMENU     MenuPageRcvBuff; 

    /*--- set default menu page into sales local data ---*/
    /* provide structure to get default menu page     */   
    MenuPageRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;                             
    MenuPageRcvBuff.uchAddress = CTLPG_STAND_ADR;          /* address 1, default menu page */
    CliParaRead(&MenuPageRcvBuff);

    if (MenuPageRcvBuff.uchPageNumber < 1 || MenuPageRcvBuff.uchPageNumber > MAX_PAGE_NO) { /* R3.1 */
        pSalesLocalRcvBuff->uchMenuShift = 1;      /* set page No. 1 for default */
    } else {
        pSalesLocalRcvBuff->uchMenuShift = MenuPageRcvBuff.uchPageNumber;                        
    }
    
    /* default adjective by menu page, 2172 */
    MenuPageRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
    MenuPageRcvBuff.uchAddress = (UCHAR) (pSalesLocalRcvBuff->uchMenuShift + CTLPG_ADJ_PAGE1_ADR - 1);
    CliParaRead(&MenuPageRcvBuff);
    pSalesLocalRcvBuff->uchAdjDefShift = MenuPageRcvBuff.uchPageNumber;
    pSalesLocalRcvBuff->uchAdjKeyShift = 0;                                                 /* set default adjective, 2172 */
    pSalesLocalRcvBuff->uchAdjCurShift = MenuPageRcvBuff.uchPageNumber;                     /* set default adjective, 2172 */
    
    /*--- SET MODE QUALIFIER DATA ---*/
    /* set rec/ no rec key default option */
    if (CliParaMDCCheck(MDC_RECEIPT_ADR, ODD_MDC_BIT1)) {          /* no receipt default */
        pModeQualRcvBuff->fsModeStatus |= MODEQUAL_NORECEIPT_DEF;    /* set default no receipt status */
    } else { 
        pModeQualRcvBuff->fsModeStatus &= ~ MODEQUAL_NORECEIPT_DEF;    
    }
 
    /* set tax system option (US/Canadian/Int'l),   V3.3 */
	pModeQualRcvBuff->fsModeStatus &= ~ (MODEQUAL_CANADIAN | MODEQUAL_INTL);
	switch ( ItemCommonTaxSystem() ) {
		case ITM_TAX_INTL:	pModeQualRcvBuff->fsModeStatus |= MODEQUAL_INTL;  break;
		case ITM_TAX_CANADA: pModeQualRcvBuff->fsModeStatus |= MODEQUAL_CANADIAN;  break;
		case ITM_TAX_US: 	pModeQualRcvBuff->fsModeStatus &= ~ (MODEQUAL_CANADIAN | MODEQUAL_INTL);  break;
		default:
			NHPOS_ASSERT_TEXT(0, "==ERROR: ItemCommonTaxSystem() Unknown tax system.");
			break;
	}
    
    /*--- MANIPURATE MODE CHANGABLE STATUS ---*/
    if (!CliParaMDCCheck(MDC_SIGNOUT_ADR, ODD_MDC_BIT1)) {    /* don't close by key position change */ 
        UieModeChange(UIE_DISABLE);                           /* make mode changable status to disable */
    } 

    /*--- Valid Number of Validation,   R3.0  ---*/
    /* move from ItemSignInDisplay(), 06/03/98 */

    PmgSetValWaitCount(0);
    /*** removed by saratoga
    if (CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT3)) {
        PmgSetValWaitCount(2);
    } else {
        PmgSetValWaitCount(0);
    }
    ****/
}


/*
*===========================================================================
**Synopsis: VOID    ItemSignInDisplay(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pWaiSignInData)
*
*    Input: UIFREGOPEOPEN *pUifRegOpeOpen
*           ITEMOPEOPEN *pItemOpeOpen :  
*           USHORT       fsModeStatus : add R3.3
*
*   Output: 
*
*    InOut: None
*
**Return:    
*
** Description: This function turns on or off the above two Mode qualifier
                status bits. And always returns, ITM_SUCCESS, success status.
*===========================================================================
*/
VOID    ItemSignInDisplay(UIFREGOPEOPEN *pUifRegOpeOpen, ITEMOPEOPEN *pSignInData, USHORT fsModeStatus)
{
	USHORT       usSystemType = RflGetSystemType ();
	REGDISPMSG   DisplayData = {0};            

    /*----- display sign in data  -----*/
    DisplayData.uchMajorClass = pUifRegOpeOpen->uchMajorClass; 
    DisplayData.uchMinorClass = pUifRegOpeOpen->uchMinorClass;
    _tcsncpy(DisplayData.aszMnemonic, pSignInData->aszMnemonic, PARA_CASHIER_LEN);   /* V3.3 */

    /* set menu page into display module control data */
    uchDispCurrMenuPage = ItemSalesLocalPtr->uchMenuShift; 
    ItemCommonShiftPage(uchDispCurrMenuPage);             /* set menu page for display module */

    /* set static data of display */
	if(!(flDispRegDescrControl & PM_CNTRL))	{ //Delayed Balance JHHJ
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
 
    if ((fsModeStatus & MODEQUAL_CASINT) == 0) { /* not sign-in previously */
		USHORT     usSystem;

        /* R3.0 */
        if (usSystemType == FLEX_STORE_RECALL) {
			FDTPARA        WorkFDTPara = {0};

            usSystem = MLD_DRIVE_THROUGH_3SCROLL;
			FDTParameter(&WorkFDTPara);
            if (WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
                || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
                || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
                || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
                usSystem = MLD_DRIVE_THROUGH_1SCROLL;
            }
        } else if (usSystemType == FLEX_POST_CHK) {      /* R3.1 */
            usSystem = MLD_DRIVE_THROUGH_1SCROLL;
        } else {
            usSystem = MLD_PRECHECK_SYSTEM;
        }

        MldSetMode(usSystem);
    }

    if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
                                                       /* training operator  ? */
        flDispRegDescrControl |= TRAIN_CNTRL;          /* turn on TRAIN descriptor control flag */
        flDispRegKeepControl |= TRAIN_CNTRL;           /* turn on TRAIN descriptor control flag */
    }
    DisplayData.fbSaveControl = DISP_SAVE_TYPE_1;      /* indicate to save display data */

    DispWrite(&DisplayData);                /* call display module */

    /*--- LCD Initialize ---*/
    if (usSystemType == FLEX_STORE_RECALL) {
        MldDisplayInit(MLD_DRIVE_1, 0);
    } else {
        MldDisplayInit(MLD_DRIVE_1 | MLD_DRIVE_2, 0);
    }

    /*--- Display to LCD ---*/
    MldScrollWrite(pSignInData, MLD_NEW_ITEMIZE);

    /*--- FDT Thread Open ---*/
    if (usSystemType == FLEX_STORE_RECALL) {
        FDTOpen();
    }

    /*--- Display Opened Guest Check No at Sign-In, V3.3 ---*/
    if (CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT0) && (usSystemType != FLEX_STORE_RECALL) &&
        ((TranModeQualPtr->fsModeStatus & (MODEQUAL_BARTENDER | MODEQUAL_CASINT)) == 0)) {

		// display a list of outstanding guest checks for this operator who has just signed in.
		// we can provide different formats of the data such as display by guest check number
		// or display by table number. the legacy is to display by guest check number.
//        ItemOtherDispGuestNo(pSignInData->ulID);
        ItemOtherDispTableNo(pSignInData->ulID);
    }
}

/*
*===========================================================================
**Synopsis: VOID    ItemSignInIgnoreModifierKey(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   Nothing 
*
** Description: Retreave display after dialogue function to send Clear key 
*               function to display module. 
*
*===========================================================================
*/
VOID    ItemSignInIgnoreModifierKey(VOID)
{
    TRANCURQUAL *pCurQualRcvBuff = TrnGetCurQualPtr();

    /*----- ignore preselect void operation by cashier -----*/
    pCurQualRcvBuff->fsCurStatus &= ~ (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN);    /* turn off preselect void */

    /*----- ignore modifier operation by cashier -----*/
    ItemCasOutInitModLocal();
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemCasSecretCode( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, UIF_CANCEL, LDT_NOUNMT_ADR
*
** Description: This function executes to control cashier ID      V3.3
*===========================================================================
fhfh*/
SHORT  ItemSetCasSecretCode (ULONG ulUifEmployeeId, ULONG ulUifEmployeePin, ULONG *ulUifEmployeePinOld)
{
	SHORT  sRetStatus;
	CASIF Casif = {0};

	Casif.ulCashierNo = ulUifEmployeeId;
	if ((sRetStatus = SerCasIndRead(&Casif)) == CAS_PERFORM) {
		if (ulUifEmployeePinOld) {
			*ulUifEmployeePinOld = Casif.ulCashierSecret;
		}
		Casif.ulCashierSecret = ulUifEmployeePin;
		sRetStatus = SerCasAssign (&Casif);
	} else {
		char  xBuff[128];
		sprintf (xBuff, " SerCasIndRead() failed: sRetStatus = %d, id %d", sRetStatus, (Casif.ulCashierNo % 100));
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	return sRetStatus;
}

SHORT   ItemCasSecretCode(ULONG *puchSecret) 
{
	REGDISPMSG   RegDispMsgD = {0};
    SHORT        sStatus ;
    UIFDIADATA   UifDiaRtn = {0};  

    /* display */
    RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    RegDispMsgD.uchMinorClass = CLASS_REGDISP_SECRET;
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    RflGetLead ( RegDispMsgD.aszMnemonic, LDT_SECRET_ADR );
    flDispRegDescrControl |= COMPLSRY_CNTRL; /* set descriptor */
    flDispRegKeepControl |= COMPLSRY_CNTRL; /* set descriptor(keep) */
    DispWrite( &RegDispMsgD );
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL; /* reset descriptowr */
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL; /* reset descriptor(keep) */

    /* execute cashier no. compulsory  and check return data*/
    if ( ( sStatus = UifDiaCashier( &UifDiaRtn, UIFREG_NO_ECHO ) )  != UIF_SUCCESS ) {
        return(UIF_CANCEL);
    }   

    /* -- check cashier no. in or A/B key in -- */
    /* check A or B key */
    switch( UifDiaRtn.auchFsc[0] ) {
    case FSC_MSR:
        return (LDT_SEQERR_ADR);
        break;

    case FSC_WAITER_PEN:
        if (UifDiaRtn.auchFsc[1]) {                     /* insert */
            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
            ItemOtherDiaRemoveKey();                    /* dialogue function  to remove waiter key */
            ItemSignInClearDisplay(0);
            return(UIF_CANCEL);
        } else {                                        /* remove */
            ItemSignInClearDisplay(0);
            return(ITM_CONT);
        }
        break;

    case FSC_AC:                   // UifDiaCashierEntry() process sign in for supervisor 
	case FSC_P1:                   // UifDiaCashierEntry() process sign in for supervisor, P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
    case FSC_NUM_TYPE:             // UifDiaCashierEntry() process sign in for supervisor 
    case FSC_SIGN_IN:              // Sign-in A key pressed for REG mode
    default:                       // Sign-in B key pressed for REG mode
        break;
    }

    *puchSecret = UifDiaRtn.ulData;

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemCasSignInCancel( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes sign-out by illegal case      V3.3
*===========================================================================
fhfh*/
VOID   ItemCasSignInCancel(VOID *pData) 
{
    SHORT   sReturnStatus;
    CASIF   *pCasData = pData;

    for (;;) {
        /* R3.3 */
        if ((sReturnStatus = CliCasPreSignOut(pCasData)) != CAS_PERFORM ) {
            /* wait for previous cashier sign-out for cashier interrupt */                
            /* UieErrorMsg(CasConvertError(sReturnStatus), UIE_WITHOUT); */
            CliSleep();
            continue;
        } else {
            break;
        }
    }

    pCasData->usUniqueAddress = CliReadUAddr();
    for(;;) {
        if ((sReturnStatus = CliCasSignOut(pCasData)) != CAS_PERFORM) {
            UieErrorMsg(CasConvertError(sReturnStatus), UIE_WITHOUT);   
        } else {
            break;
        }
    }
}

/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemCasSignInClearDisplay( USHORT usType ) 
*                                     
*     Input:    0: mode-in display
*               1: clear display
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function clears display after diaglog function, V3.3
*===========================================================================
fhfh*/
VOID   ItemSignInClearDisplay(USHORT usType) 
{
    if (usType == 0) {
		UIFREGMODEIN   UifRegModeIn = {0};

		UifRegModeIn.uchMajorClass = CLASS_UIFREGMODE;   /* set mode in class */
        UifRegModeIn.uchMinorClass = CLASS_UIREGMODEIN;
        ItemModeInDisplay(&UifRegModeIn);                /* redisplay sign-in */
    } else {
		REGDISPMSG     RegDispMsg = {0};

		RegDispMsg.uchMajorClass = CLASS_UIFREGOTHER;
        RegDispMsg.uchMinorClass = CLASS_UICLEAR;
        DispWrite( &RegDispMsg );                        /* clear display */
    }
}

/****** end of file ******/
