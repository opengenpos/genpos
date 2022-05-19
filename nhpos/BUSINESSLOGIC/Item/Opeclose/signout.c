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
* Title       : Sign out Item module main                        
* Category    : Item Module, NCR 2170 Hsopitality US Model Application         
* Program Name: SIGNOUT.C
* --------------------------------------------------------------------------
* Abstract: ItemSignOut()            : Cashier sign out function main
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* May-15-92: 00.00.01  : K.Maeda   : initial                                   
* Sep-20-93: 02.00.00  : K.You     : Bug fixed Q-8 (mod. ItemSignOut)
* Mar-08-94: 00.00.04  : K.You     : del. waiter lock feature.
*          :           :           : add flex GC file feature. (mod. ItemSignOut)              
* Apr-25-95: 03.00.00  : hkato     : R3.0
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

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <uie.h>
#include    <uireg.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <cscas.h>
#include    <csstbcas.h>
#include    <cswai.h>
#include    <csstbwai.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <pmg.h>
#include    <eept.h>
#include    "itmlocal.h"
#include	<ConnEngineObjectIf.h>
#include	<BlFWif.h>


/*
*===========================================================================
**Synopsis: SHORT   ItemSignOut(UIFREGOPECLOSE *UifRegOpeClose)
*
*    Input: UIFREGOPECLOSE *UifRegOpeClose
*   Output: None
*    InOut: None
*
**Return:   ITM_SUCCESS    : Function Performed succesfully
*           LDT_SEQERR_ADR : Illeag operationsequense
*
** Description: Sign out module main function.
*===========================================================================
*/

SHORT   ItemSignOutTest(VOID)
{
	// if the prohibit sign-out is indicated then do not allow the
	// operator to sign out of the terminal until something is done
	// to turn off the sign-out prohibit.
	if (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == 0) {
        return(LDT_PROHBT_ADR);
	}

	return (SUCCESS);
}

SHORT   ItemSignOutWithTest(UIFREGOPECLOSE *pUifRegOpeClose)
{
	// if the prohibit sign-out is indicated then do not allow the
	// operator to sign out of the terminal until something is done
	// to turn off the sign-out prohibit.
	if (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == 0) {
        return(LDT_PROHBT_ADR);
	}

	return ItemSignOut (pUifRegOpeClose);
}

SHORT   ItemSignOut(UIFREGOPECLOSE *pUifRegOpeClose)
{
    SHORT  sReturnStatus;
	ITEMOPECLOSE       ItemOpeClose = {0};
	TRANMODEQUAL       myTranModeQual;

	// Save the Operator Id and Mnemonic to be used for the EJ entry below.
	// We get it here in order to have a copy before it has a chance to be cleared below.
	TrnGetModeQual( &myTranModeQual );

    switch(pUifRegOpeClose->uchMinorClass) {
    case CLASS_UICASHIEROUT:
    case CLASS_UIB_OUT:
    case CLASS_UICASRELONFIN:
    case CLASS_UIWAITERLOCKOUT: /* V3.3 */
        if ((sReturnStatus = ItemCasSignOut(pUifRegOpeClose, &ItemOpeClose)) != UIF_CAS_SIGNOUT) {
            return(sReturnStatus);
        }
        break;

/* #ifdef GUEST_CHECK_MODEL */
    case CLASS_UICASINTOUT:         /* R3.3 */
    case CLASS_UICASINTB_OUT:       /* R3.3 */
        sReturnStatus = ItemCasSignOut(pUifRegOpeClose, &ItemOpeClose);
        if ((sReturnStatus != UIF_CAS_SIGNOUT) ||
            (sReturnStatus != UIF_SUCCESS)) {
            return(sReturnStatus);
        }
        break;
 
    case CLASS_UICASINTOUT2:        /* R3.3 */
        sReturnStatus = ItemCasSignOut(pUifRegOpeClose, &ItemOpeClose);
        break;
 
    case CLASS_UICASINT_PREOUT:         /* R3.3 */
    case CLASS_UICASINTB_PREOUT:       /* R3.3 */
        sReturnStatus = ItemCasIntPreSignOut(pUifRegOpeClose);
        return(sReturnStatus);
        break;
/* #endif */
/* #ifdef GUEST_CHECK_MODEL */
    case CLASS_UIWAIKEYRELONFIN:
        if ((sReturnStatus = ItemWaiRelOnFinOut(&ItemOpeClose)) != UIF_WAI_SIGNOUT) {
            return(sReturnStatus);
        }
        break;

    case CLASS_UIBARTENDEROUT:                  /* R3.1 */
        if ((sReturnStatus = ItemBarSignOut(pUifRegOpeClose, &ItemOpeClose)) != UIF_WAI_SIGNOUT) {
            return(sReturnStatus);
        }
        break;
/* #endif */
    case CLASS_UIMODELOCK:
        sReturnStatus = ItemModeKeyOut(&ItemOpeClose);
        if (sReturnStatus != UIF_CAS_SIGNOUT || sReturnStatus != UIF_WAI_SIGNOUT) {
            return(sReturnStatus);
        }
        break;

    default:    /* not use */
        break;
    }

	if (pUifRegOpeClose->uchMinorClass != CLASS_UIMODELOCK) {
		if ((sReturnStatus == UIF_CAS_SIGNOUT || sReturnStatus == UIF_WAI_SIGNOUT) && TrnICPOpenFile() == TRN_SUCCESS) {
			TRANCURQUAL     CurQualWork = {0};
			ITEMMISC        ItemMisc = {0};
			ITEMPRINT       ItemPrintData = {0};      
			ITEMTRANSCLOSE  ItemTransClose = {0};

			/* initialize */
			TrnPutCurQual(&CurQualWork);    // clear the CurQual area by setting to zero.

			{
				ITEMCOMMONLOCAL *pItemCommonLocal = 0;

				ItemCommonGetLocal(&pItemCommonLocal);
				pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO | COMMON_VOID_EC);  // Clear the Connsecutive number flag to indicate it needs to be generated
			}
			/* consecutive no. and current qualifier count up on */
			ItemCountCons();
			CliParaSaveFarData();        // Update the consecutive number persistent store

			/* set option to current qualifier(current print status) */
			ItemCurPrintStatus();

			/* start print service */
			ItemPrintStart(TYPE_POSTCHECK);     /* store only item consoli. buffer */     


			/* close transaction */
			TrnGetCurQual( &CurQualWork );  
			CurQualWork.uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
			if (  (CurQualWork.flPrintStatus & CURQUAL_GC_SYS_MASK ) != CURQUAL_PRE_UNBUFFER ) {
				CurQualWork.uchPrintStorage = PRT_ITEMSTORAGE;
			}
			CurQualWork.uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */
			TrnPutCurQual( &CurQualWork );

			ItemMisc.ulEmployeeNo = ItemPrintData.ulID = ItemMisc.ulID = ItemOpeClose.ulID;
			_tcsncpy (ItemMisc.aszMnemonic, myTranModeQual.aszCashierMnem, STD_CASHIERNAME_LEN);
			ItemPrintData.usConsNo = CurQualWork.usConsNo;

			/* set class */
			ItemMisc.uchMajorClass = CLASS_ITEMMISC;
			ItemMisc.fbStorageStatus |= NOT_CONSOLI_STORAGE;
			ItemMisc.uchMinorClass = CLASS_SIGNOUT;
			UifEmployeeSignTimeMethodString (pUifRegOpeClose->ulStatusFlags, ItemMisc.aszNumber);

			// Set the Sign-out Date/Time information
			{
				DATE_TIME DateTime;          /* for TOD read */

				PifGetDateTime(&DateTime);
				ItemMisc.usYear  = DateTime.usYear;
				ItemMisc.usMonth = DateTime.usMonth;
				ItemMisc.usDay   = DateTime.usMDay;
				ItemMisc.uchJobCode = 0;
				ItemMisc.usTimeinTime = 0;
				ItemMisc.usTimeinMinute = 0;
				ItemMisc.usTimeOutTime   = DateTime.usHour;
				ItemMisc.usTimeOutMinute = DateTime.usMin;
			}

			ItemMisc.fsPrintStatus = ( PRT_JOURNAL);    /* Indicate we want Journal print only for Sign-out */
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
				_itot (ItemOpeClose.ulID, aszSapId, 10);

				ConnEngineSendCashierActionWithAddon (CONNENGINE_ACTIONTYPE_SIGNOUT, aszSapId, 0);
				{
					char  xBuff[128];

					sprintf ( xBuff, "==ACTION: Cashier Sign-out %8.8u", ItemOpeClose.ulID);
					NHPOS_NONASSERT_NOTE("==ACTION", xBuff);
				}
			}

			{
				ITEMCOMMONLOCAL *pItemCommonLocal = 0;

				ItemCommonGetLocal(&pItemCommonLocal);
				pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO);  // Clear the Connsecutive number flag to indicate it needs to be generated
			}

			BlFwIfPopdownMessageAllVirtual();  // Popdown any Popup Windows that need to be removed to get back to a default display.

			/* finalize */
			TrnInitialize(TRANI_CURQUAL | TRANI_ITEM | TRANI_MODEQUAL); 
		}
	}

    ItemInit();                                 /* initialize item local datas */
/*** allow post receipt after sign-out, V3.3 
    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);  / inhibit post receipt, R3.0 /
***/
    ItemSignOutResetMenuPage(); /* 07/07/98 */

    return(sReturnStatus);
}


/*
*===========================================================================
**Synopsis: VOID    ItemSignOutResetMenuPage(VOID)
*
*    Input: None
*   Output: None
*    InOut: None
*
**Return:   none
*
** Description: reset menu shift page by sign-out, 07/07/98
*===========================================================================
*/

VOID    ItemSignOutResetMenuPage(VOID)
{
    ITEMSALESLOCAL     *pSalesDataBuffer = ItemSalesGetLocalPointer();
    PARACTLTBLMENU     MenuPage; 

    MenuPage.uchMajorClass = CLASS_PARACTLTBLMENU;                             
    MenuPage.uchAddress = CTLPG_STAND_ADR; 
    CliParaRead(&MenuPage);

    if (MenuPage.uchPageNumber < 1 || MenuPage.uchPageNumber > MAX_PAGE_NO) {  /* 5->9 11/29/96 */
        pSalesDataBuffer->uchMenuShift = 1; 
    } else {
        pSalesDataBuffer->uchMenuShift = MenuPage.uchPageNumber;                        
    }
    uchDispCurrMenuPage = pSalesDataBuffer->uchMenuShift;
    ItemCommonShiftPage(pSalesDataBuffer->uchMenuShift);
}

/****** end of file ******/
