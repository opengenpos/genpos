/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO            **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-9                  **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : REG MODE MISC MODULE - ItemMisc Module                         
* Category    : REG MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: itmisc.c
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*   SHORT   ItemMiscNosale( UIFREGMISC *ItemMisc )
*   SHORT   ItemMiscLoanPickup (MAINTLOANPICKUP  *pLoanPickupData)
*   SHORT   ItemMiscPO(UIFREGMISC *pItemMisc, USHORT  usClientDisplay)
*   SHORT   ItemMiscRA( UIFREGMISC *pItemMisc, USHORT  usClientDisplay)
*   SHORT   ItemMiscTipsPO(UIFREGMISC *ItemMisc)
*   SHORT   ItemMiscMoney(UIFREGMISC *pMisc)
*   SHORT   ItemMiscPostReceipt( UIFREGMISC *pUifRegMisc )
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
* Jan-31-95: 03.00.00 : M.Ozawa     : mod. ItemMiscCheckTrans (not initialize consolidation file)
* Mar-28-95: 03.00.00 : hkato       : R3.0
* Apr-04-95: 03.00.00 : M.Ozawa     : R3.0 (Add Trans. Open I/F for LCD)
* Jun-21-95: 03.00.00 : T.Nakahata  : prohibit Tips P/O in Store/Recall
* Nov-10-95: 03.01.00 : hkato       : R3.1
* Aug-06-98: 03.03.00 : hrkato      : V3.3
* Aug-13-99: 03.05.00 : M.Teraki    : Merge GUEST_CHECK_MODEL/STORE_RECALL_MODEL
* Nov-25-99: 01.00.00 : hrkato      : Money Declaration
* Dec-16-02: 01.04.00 : cwunn		: Remote Time In/Out GSU SR 8
* Mar-31-15: 02.02.01 : R.Chambers	: simplify source, add comments, localize variables, add logs
* Apr-03-15: 02.02.01 : R.Chambers  : eliminate save/restore of TrnInformation, ensure R/J printing.
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
mhmh*/
/*
*===================================================================================
* INCLUDE FILES
* ----------------------------------------------------------------------------------
*/
#include	<tchar.h>
#include    <String.h>
#include    <Stdlib.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "regstrct.h"
#include    "pif.h"
#include    "transact.h"
#include    "log.h"
#include    "appllog.h"
#include    "fsc.h"
#include    "plu.h"
#include    "rfl.h"
#include    "uie.h"
#include    "uireg.h"
#include    "display.h"
#include    "Paraequ.h"   
#include    "Para.h"
#include    "mld.h"
#include    "item.h"
#include    "maint.h"
#include    "csstbpar.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "cscas.h"
#include    "csstbcas.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csttl.h"
#include    "csstbttl.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "fdt.h"
#include    "prtprty.h"
#include    "itmlocal.h"
#include	"prt.h" //US Customs to include PrtCancel()
#include    "pmg.h"
#include    "trans.h"
#include	<uic.h>
#include	<pifmain.h>

#include    "ConnEngineObjectIf.h"

/*===================================================================================
* STATICE AREA
* ----------------------------------------------------------------------------------
*/


ITEMMISCLOCAL    ItemMiscLocal;

static TRANINFORMATION ItemMiscSaveTrnInformation;
static ULONG           ItemMiscSaveIdentifier = 0;
static ITEMCOMMONLOCAL ItemMiscSaveItemCommonLocal;
static SHORT           ItemMiscSaveUicCheckSetHandle;

SHORT  ItemSaveTrnInformationAndLock (ULONG  ulIdentifier)
{
	SHORT  sRetStatus = 0;

	if ((ItemCommonLocal.fbCommonStatus & COMMON_PROHIBIT_SIGNOUT) != 0) {
		return (-1);
	}

	ItemMiscSaveUicCheckSetHandle = UicCheckAndSet();
	if (ItemMiscSaveUicCheckSetHandle == UIC_NOT_IDLE) {
		return -1;
	}

	if (ItemMiscSaveIdentifier == 0) {
		ItemMiscSaveIdentifier = ulIdentifier;
//		ItemMiscSaveTrnInformation = TrnInformation;
//		ItemMiscSaveItemCommonLocal = ItemCommonLocal;
		TrnInformation.TranCurQual.fbNoPrint = ~PRT_SLIP;                        /* no print status */
		TrnInformation.TranCurQual.fbCompPrint = (PRT_RECEIPT | PRT_JOURNAL);    /* compulsory print status */

		ItemCommonLocal.fbCommonStatus |= COMMON_PROHIBIT_SIGNOUT;
	} else {
		sRetStatus = -2;
	}

	return sRetStatus;
}

SHORT  ItemRestoreTrnInformationAndUnlock (ULONG ulIdentifier)
{
	SHORT  sRetStatus = 0;

	if (ItemMiscSaveIdentifier && ItemMiscSaveIdentifier == ulIdentifier) {
//		TrnInformation = ItemMiscSaveTrnInformation;
//		ItemCommonLocal = ItemMiscSaveItemCommonLocal;
		ItemMiscSaveIdentifier = 0;
		UicResetFlag (ItemMiscSaveUicCheckSetHandle);
		ItemMiscSaveUicCheckSetHandle = 0;
		ItemCommonLocal.fbCommonStatus &= ~COMMON_PROHIBIT_SIGNOUT;
	} else {
		sRetStatus = -1;
	}

	return sRetStatus;
}
/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscOpenTrans( UCHAR uchMinorClass ) 
*                                     
*     Input:    usClientDisplay - indicates if write to display (1) or not (0)
*               uchMinorClass   - indicates the minor class of the miscellanous operation
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*               
** Description: This function executes making open data and transaction i/f.
*               The uchMinorClass is used to determine which of the different
*               miscellanous functions were are supposed to be opening.
*===========================================================================
fhfh*/
VOID    ItemMiscOpenTrans (UCHAR uchMajorClass, UCHAR uchMinorClass)
{
	TRANCURQUAL     *pCurQualRcvBuff = TrnGetCurQualPtr();
	ITEMTRANSOPEN   ItemTransOpen = {0};

    ItemTransOpen.uchMajorClass = uchMajorClass;
    ItemTransOpen.uchMinorClass = uchMinorClass;
    ItemTransOpen.fsPrintStatus = ( PRT_JOURNAL );

    ItemCountCons();        /* consecutive no. and current qualifier count up on */
    ItemCurPrintStatus();   /* set option to current qualifier (current print status) */

	pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;    /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */
	pCurQualRcvBuff->uchPrintStorage = PRT_ITEMSTORAGE;
	if ( (pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
		// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
		// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
		// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
		pCurQualRcvBuff->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
		/* if unbuffering system type, make sure slip compulsory bit is off */
		pCurQualRcvBuff->fbCompPrint &= ~ CURQUAL_COMP_S;  /* slip bit off*/
	}
	pCurQualRcvBuff->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */

	ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    ItemPromotion( ItemTransOpen.fsPrintStatus, TYPE_STORAGE);
    
    ItemHeader( TYPE_STORAGE );

    TrnOpen( &ItemTransOpen );  /* transaction open */
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscCloseTrans (UCHAR uchMajorClass, UCHAR uchMinorClass) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function execites making close data and transaction i/f.  
*===========================================================================
fhfh*/
VOID    ItemMiscCloseTrans (UCHAR uchMajorClass, UCHAR uchMinorClass)
{
	ITEMTRANSCLOSE  ItemTransClose = {0};

    /*
	 * the function TrnStoGetConsToPostR() must be used to transfer the last transaction
	 * to the Post Receipt storage area in order for Post Receipt or Print on Demand to work
	 * by using TrnStoPutPostRToCons() to retrieve the saved data in the Post Receipt storage area.
	**/
    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
    ItemMiscSetStatus(MISC_POSTRECPT | MISC_PRINTDEMAND);  // for No Sale allow reprint
    TrnStoGetConsToPostR();

    /* set class code */
    ItemTransClose.uchMajorClass = uchMajorClass;
    ItemTransClose.uchMinorClass = uchMinorClass;

    TrnClose( &ItemTransClose );
	ItemCommonResetLocalCounts();
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTrans (LONG  lAmount, USHORT usHaloAddress) 
*                                     
*     Input:    lAmount        - amount of the transaction
*               fbModifier     - bit map of modifiers for VOID_MODIFIER check
*               usHaloAddress  - address of the HALO used with CLASS_PARATRANSHALO
*    Output:    nothing 
*
** Return:      ITM_SUCCESS      if the checks on the data and state pass
*               LDT_SEQERR_ADR   if the state is not appropriate for this transaction
*               LDT_KEYOVER_ADR  if the amount causes a HALO check failure
*
** Description: This function check whether to executes a miscellanous transaction or not
*               based on current transaction state as well as the the amount and whether
*               the amount meets the HALO check if there are HALO values specified.
*===========================================================================
fhfh*/
static SHORT   ItemMiscCheckStateTrans (DCURRENCY  lAmount, USHORT fbModifier, USHORT usHaloAddress)
{
    PARATRANSHALO   ParaTransHaloL;
    UCHAR           uchHaloData;

    /* check if tax modifier key or food stamp modifier was used ? */
    if ( ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) { /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check p-void use ? */
    if ( TrnGetCurQualPtr()->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {
        return(LDT_SEQERR_ADR); 
    }

    /* check HALO */
    ParaTransHaloL.uchMajorClass =  CLASS_PARATRANSHALO;
    ParaTransHaloL.uchAddress = (UCHAR)usHaloAddress;
    CliParaRead( &ParaTransHaloL );
    uchHaloData = ParaTransHaloL.uchHALO;

    /* check void or not*/
    if ( fbModifier & VOID_MODIFIER ) {
        ParaTransHaloL.uchAddress = HALO_VOID_ADR;
        CliParaRead( &ParaTransHaloL );
        if (uchHaloData == 0) {                     /* correction 8/24'93   */
            uchHaloData = ParaTransHaloL.uchHALO;   /* set void halo        */
        } else if (ParaTransHaloL.uchHALO != 0) {   /* void halo isn't 0 ?  */
            if (uchHaloData > ParaTransHaloL.uchHALO) {
                uchHaloData = ParaTransHaloL.uchHALO;
            }
        }
    }

    if ( RflHALO(lAmount, uchHaloData) != RFL_SUCCESS ) {
        return(LDT_KEYOVER_ADR); 
    }

    return(ITM_SUCCESS);
}


  
/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscNosaleCashier(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, UIF_CANCEL, LDT_NOUNMT_ADR
*
** Description: This function executes to control cashier ID  R3.1, V3.3
*===========================================================================
fhfh*/
SHORT   ItemMiscNosaleCashier(VOID)
{
    TRANMODEQUAL    *pTranModeQualL = TrnGetModeQualPtr();
	UIFDIAEMPLOYEE  UifDiaEmployee = {0};
	REGDISPMSG      RegDispMsgD = {0};
    UIFDIADATA      UifDiaRtn = {0};
    SHORT           sStatus;

    /* display */
    RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    RegDispMsgD.uchMinorClass = CLASS_REGDISP_CASHIERNO;
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    RflGetLead (RegDispMsgD.aszMnemonic, LDT_ID_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL; /* set descriptor */
    flDispRegKeepControl |= COMPLSRY_CNTRL; /* set descriptor(keep) */
    DispWrite( &RegDispMsgD );
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL; /* reset descriptowr */
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL; /* reset descriptor(keep) */

    /* execute cashier no. compulsory  and check return data*/
    if ( ( sStatus = UifDiaCashier( &UifDiaRtn, UIFREG_NO_ECHO ) )  != UIF_SUCCESS ) {
        ItemTransDispDef();
        return(UIF_CANCEL);
    }   

    /* -- check cashier no. in or A/B key in -- */
    /* check A or B key */
    switch( UifDiaRtn.auchFsc[0] ) {
    case FSC_MSR:     /* MSR, R3.1 */
		if (UifCheckMsrEmployeeTrackData (UifDiaRtn.auchTrack2, UifDiaRtn.uchTrack2Len) != 0) {
			NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
			return (LDT_PROHBT_ADR);
		}

		sStatus = UifTrackToUifDiaEmployee (UifDiaRtn.auchTrack2, UifDiaRtn.uchTrack2Len, &UifDiaEmployee);
		UifDiaEmployee.ulStatusFlags = UifDiaRtn.ulStatusFlags;
		if (sStatus != UIF_SUCCESS)
			return LDT_NTINFL_ADR;

        if (UifDiaEmployee.usSwipeOperation != MSR_ID_CASHIER) {
            return(LDT_PROHBT_ADR);
        }
        if (pTranModeQualL->fbCashierStatus & MODEQUAL_CASHIER_B) {
            return(LDT_SEQERR_ADR);
        }
        break;

    case FSC_SIGN_IN: /* A key */
    case FSC_CASINT:    /* V3.3 */
        /* check drawer */
        if ( pTranModeQualL->fbCashierStatus & MODEQUAL_CASHIER_B ) {
            return( LDT_SEQERR_ADR );
        }
		UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
        break;

    case FSC_WAITER_PEN:    /* V3.3 */
        if (UifDiaRtn.auchFsc[1]) {
            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
            ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */
        } else {
			LONG   lStatus = 0;

            UieErrorMsg( LDT_SEQERR_ADR, UIE_WITHOUT ); /* display error and wait for clear key */
            do {
                lStatus = ItemOtherDiaInsertKey();                    /* dialogue function  to remove waiter key */
                if (lStatus != pTranModeQualL->ulCashierID) {
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
                    ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */
                }
            } while (lStatus != pTranModeQualL->ulCashierID);
        }
        ItemTransDispDef();
        return(UIF_CANCEL);
        break;

    default: /* B key */
        /* check drawer */
        if ( ( pTranModeQualL->fbCashierStatus & MODEQUAL_CASHIER_B ) == 0 ) {
            return( LDT_SEQERR_ADR );
        }
		UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
		break;

    }

    /* check secret code, V3.3 */
	if (pTranModeQualL->ulCashierSecret) {
		if ((UifDiaRtn.auchFsc[0] == FSC_MSR) && UifDiaEmployee.ulSwipeEmployeePin == 0L) { /* not set secret no on msr card */
            if ( UifDiaEmployee.ulSwipeEmployeeId !=  pTranModeQualL->ulCashierID ) {
                return( LDT_NOUNMT_ADR );
            }
        } else {
            if ( UifDiaEmployee.ulSwipeEmployeeId !=  pTranModeQualL->ulCashierID || UifDiaEmployee.ulSwipeEmployeePin !=  pTranModeQualL->ulCashierSecret ) {
                if( UifDiaEmployee.ulSwipeEmployeeId == 0L ) {
                    return( LDT_SEQERR_ADR );
                } else {
                    return( LDT_NOUNMT_ADR );
                }
            }
        }
    } else {
        /* check A/B key or Other */
        if ( CliParaMDCCheck(MDC_CASIN_ADR, ODD_MDC_BIT0) != 0 ) {
            if( UifDiaRtn.ulData != 0L ) {
                return( LDT_SEQERR_ADR );
            }
            if( ((pTranModeQualL->fbCashierStatus &  MODEQUAL_CASHIER_B) != 0 ) && ( UifDiaRtn.auchFsc[0] == FSC_SIGN_IN ) ) {
                return( LDT_NOUNMT_ADR );
            } else {
                if( ((pTranModeQualL->fbCashierStatus &  MODEQUAL_CASHIER_B) == 0 ) && ( UifDiaRtn.auchFsc[0] != FSC_SIGN_IN ) ) {
                    return( LDT_NOUNMT_ADR );
                }
            }
        } else {
            if ( UifDiaRtn.ulData !=  pTranModeQualL->ulCashierID ) {
                if( UifDiaRtn.ulData == 0L ) {
                    return( LDT_SEQERR_ADR );
                } else {
                    return( LDT_NOUNMT_ADR );
                }
            }
        }
    }

    return(ITM_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscNosaleOpen( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
** Description: This function makes data for transaction open.  
*===========================================================================
fhfh*/
VOID    ItemMiscNosaleOpen( VOID )
{
	ITEMTRANSOPEN   ItemTransOpen = {0};

    /* set class code */
    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = CLASS_OPENNOSALE;
    ItemTransOpen.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );
    
    /* consecutive no. and current qualifier count up on */
    ItemCountCons();

    /* set option to current qualifier(current print status) */
    ItemCurPrintStatus();

	{
		TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */

		/* check unbuffering or other */      
		/* if unbuffering, slip compulsory bit off */
		pCurQualRcvBuff->uchPrintStorage = PRT_ITEMSTORAGE;
		if ((pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
			// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
			// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
			// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
			pCurQualRcvBuff->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
			pCurQualRcvBuff->fbCompPrint &= ~ CURQUAL_COMP_S;  /* slip bit off*/
		}
		pCurQualRcvBuff->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */
	}

	ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );

    ItemHeader( TYPE_STORAGE );

    TrnOpen( &ItemTransOpen );  /* transaction open */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransOpen, 0);
    
    /*--- LCD Initialize ---*/
    MldDisplayInit(MLD_DRIVE_1, 0);

    /*--- Display Cashier/Waiter No. ---*/
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscNosaleItem(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes making no sale data and transaction i/f.  
*===========================================================================
fhfh*/

VOID    ItemMiscNosaleItem(UIFREGMISC *ItemMisc)
{
    ITEMMISC    TrnItemMisc = {0};
	REGDISPMSG  RegDispMsgD = {0}, RegDispBack = {0};

    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_NOSALE;
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );
    
    /* set print status(journal and slip) */
    TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

    /* DISPLAY */
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL; 
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
 
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
        }
    } 

    RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
    RegDispMsgD.uchMinorClass = CLASS_UINOSALE;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_NOSALE_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    ItemTendDispIn(&RegDispBack);

    DispWriteSpecial( &RegDispMsgD, &RegDispBack);
 
    TrnItem( &TrnItemMisc );

    /* send to enhanced kds, 2172 */
    ItemSendKds(&TrnItemMisc, 0);

	{
		ULONG           ulUserId = 0;
		TCHAR           aszSapId[16];

	    if (TranModeQualPtr->ulCashierID != 0)
			ulUserId = TranModeQualPtr->ulCashierID;
		else if (TranModeQualPtr->ulWaiterID != 0)
			ulUserId = TranModeQualPtr->ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

		ConnEngineSendCashierActionWithAmount (CONNENGINE_ACTIONTYPE_NOSALE, aszSapId, TrnItemMisc.aszNumber, TrnItemMisc.lAmount, 0);
	}
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemMiscNosale( UIFREGMISC *ItemMisc ) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: This function executes no sale, FSC_NO_SALE.
*               It is to document that the drawer was opened for instance if
*               breaking a dollar to give change.  The idea is that the amount
*               of money in the drawer did not change only the currency
*               denominations used.
*===========================================================================
*/
SHORT   ItemMiscNosale( UIFREGMISC *ItemMisc )
{
    SHORT   sStatus;

    /* check if tax modifier key or food stamp modifier was used ? */
    if (ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) { /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check p-void use ? */
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {
        return(LDT_SEQERR_ADR); 
    }

    /* cashier ID ok or not,    V3.3 */
    if (CliParaMDCCheckField(MDC_MENU_ADR, MDC_PARAM_BIT_A) != 0) {
        if ((sStatus = ItemMiscNosaleCashier()) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    /* check if Supervisor Intervention is required for No Sale */
    if ( CliParaMDCCheckField( MDC_MENU_ADR, MDC_PARAM_BIT_B) != 0 ) {
		/* check SPV INTERVENTION canceled or success */
		if ( ( sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
			return( sStatus );
		}
	}

	// allow for a reprint of Nosale if desired per request of VCS.
    /* inhibit post receipt,  R3.0 */
//    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

    /* make data and execute open */
    ItemMiscNosaleOpen();

    /* drawer open */
    ItemDrawer();

    /* make data and transaction i/f*/
    ItemMiscNosaleItem(ItemMisc);

    /* print trailer */
    ItemTrailer(TYPE_STORAGE);

    /* make data and execute close */
	ItemMiscCloseTrans(CLASS_ITEMTRANSCLOSE, CLASS_CLSNOSALE);

    /* data affection */ 
    while ( ( sStatus = TrnSendTotal()) !=  TRN_SUCCESS ) {
        UieErrorMsg( GusConvertError( sStatus ), UIE_WITHOUT );  /*LDT_CLRABRT_ADR*/
    }

    /* initialize transaction information */
    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    FDTCancel();

    ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */

    /* finalize */
    return( ItemFinalize() );
}     



SHORT   ItemMiscSaveInitializeEnv (CASIF  *pCasIf)
{
	SHORT  sRetStatus = 0;

	// Save Current Environment
	if (TrnICPOpenFile() == TRN_SUCCESS) {
		TRANGCFQUAL   *pGcfQualWork = TrnGetGCFQualPtr();
		TRANMODEQUAL  *pModeQualRcvBuff = TrnGetModeQualPtr();   
		TRANCURQUAL   CurQualWork = {0};

		/* initialize TranCurQual and modify the transaction state cashier id */
		TrnPutCurQual(&CurQualWork);
		pModeQualRcvBuff->ulCashierID = pGcfQualWork->ulCashierID = pCasIf->ulCashierNo;
		memcpy (pModeQualRcvBuff->aszCashierMnem, pCasIf->auchCashierName, sizeof (pModeQualRcvBuff->aszCashierMnem));

		ItemCommonGetLocalPointer()->fbCommonStatus &= ~(COMMON_CONSNO | COMMON_VOID_EC);  // Clear the Connsecutive number flag to indicate it needs to be generated

		/* consecutive no. and current qualifier count up on */
		ItemCountCons();
		CliParaSaveFarData();        // Update the consecutive number persistent store
	} else {
		NHPOS_ASSERT_TEXT(0, "**ERROR: ItemMiscSaveInitializeEnv() TrnICPOpenFile() failed.");
		ItemRestoreTrnInformationAndUnlock (127);
		return -1;
	}

	return TRN_SUCCESS;
}

SHORT   ItemMiscRestoreEnv (USHORT  usClientDisplay)
{
	SHORT  sRetStatus = 0;

    /* initialize transaction information */
    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI); 

    FDTCancel();

    ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */

    /* finalize */
	sRetStatus = ItemFinalize();

	if (!usClientDisplay) {
		// close the open transaction data files before restoring the environment.
		TrnICPCloseFile();
	}
	ItemRestoreTrnInformationAndUnlock (127);

	return sRetStatus;
}

SHORT  ItemMiscLoanPickup (MAINTLOANPICKUP  *pLoanPickupData)
{
	extern MAINTWORK MaintWork;
	SHORT  sRetStatus, sStatus;
	UCHAR  uchMinorClass = pLoanPickupData->uchMinorClass;

	if (pLoanPickupData->uchMinorClass < 1 || pLoanPickupData->uchMinorClass > 30) {
		return 1000;
	}

	// Save Current Environment
	if ((sRetStatus = ItemSaveTrnInformationAndLock (127)) < 0) {
		char xBuff[128];
		sprintf (xBuff, "ItemMiscLoanPickup(): ItemSaveTrnInformationAndLock - sRetStatus = %d", sRetStatus);
		NHPOS_ASSERT_TEXT(0, xBuff);
		return 1001;
	}

	MaintWork.LoanPickup.uchLoanPickStatus = pLoanPickupData->uchLoanPickStatus;
    pLoanPickupData->uchMinorClass = CLASS_MAINTCASIN;
	sStatus = MaintLoanPickupCasSignIn(pLoanPickupData, 0);
	if (sStatus != SUCCESS) {
		char xBuff[128];
		sprintf (xBuff, "ItemMiscLoanPickup(): sStatus = %d", sStatus);
		NHPOS_ASSERT_TEXT(0, xBuff);
		sRetStatus = ItemRestoreTrnInformationAndUnlock (127);
		return 1002;
	}

    pLoanPickupData->uchMinorClass = uchMinorClass;       /* Set Minor Class */
	/* Print & Memory Affection Loan Amount */
	MaintLoanPickupHeaderCtl(pLoanPickupData); /* R/J Header */
	if (pLoanPickupData->uchMajorClass == CLASS_MAINTLOAN) {
		sStatus = MaintLoanAmountSet(pLoanPickupData);
	} else {
		sStatus = MaintPickupAmountSet(pLoanPickupData);
	}
	if (sStatus != TRN_SUCCESS) {
		char xBuff[128];
		sprintf (xBuff, "ItemMiscLoanPickup(): MaintLoanAmountSet - sStatus = %d", sStatus);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}

    pLoanPickupData->uchMinorClass = CLASS_MAINTCASOUT;
    sStatus = MaintLoanPickupCasSignOut(pLoanPickupData);
	if (sStatus != SUCCESS) {
		char xBuff[128];
		sprintf (xBuff, "ItemMiscLoanPickup(): MaintLoanPickupCasSignOut - sStatus = %d", sStatus);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}

	if ((sRetStatus = ItemRestoreTrnInformationAndUnlock (127)) < 0) {
		char xBuff[128];
		sprintf (xBuff, "ItemMiscLoanPickup(): ItemRestoreTrnInformationAndUnlock - sRetStatus = %d", sRetStatus);
		NHPOS_ASSERT_TEXT(0, xBuff);
		return sRetStatus;
	}

	return sStatus;
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscPOSPV(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc -  pointer from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, UIF_CANCEL
*               
** Description: This function check modifier status and execute SPV.  
*===========================================================================
fhfh*/
SHORT   ItemMiscPOSPV(UIFREGMISC *ItemMisc)
{
    /* check SPV intervention */
    if ( CliParaMDCCheck( MDC_PO_ADR, ODD_MDC_BIT0) != 0 ) {
        return(ItemSPVInt(LDT_SUPINTR_ADR)); 
    } else if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0) != 0 ) {
            return(ItemSPVInt(LDT_SUPINTR_ADR)); 
        }
    }
    return(ITM_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscPOItem(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      nothing
** Description: This function executrs making data and transaction i/f.  
*===========================================================================
fhfh*/

VOID    ItemMiscPOItem(UIFREGMISC *ItemMisc, USHORT  usClientDisplay)
{
	ITEMMISC        TrnItemMisc = {0};
    SHORT           sFlag = 0;

    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_PO;

    /* set the account number entered */
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );

    /* set paid out value */
	if ((ItemMisc->fbModifier & VOID_MODIFIER) == 0 ) {
		TrnItemMisc.lAmount = -1 * (ItemMisc->lAmount);
	} else {
        /* set void bit*/
        TrnItemMisc.fbModifier |= VOID_MODIFIER;
	}

    /* validation checks if this is not from a non-Client source such as Connection Engine */
	if (usClientDisplay) {
		if (ItemMisc->fbModifier & VOID_MODIFIER ) {
			if ( CliParaMDCCheck( MDC_PO_ADR, ODD_MDC_BIT1) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			} else  if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			}
		} else {
			if ( CliParaMDCCheck( MDC_PO_ADR, ODD_MDC_BIT1) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			}
		}

		if (CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    		if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				if (sFlag == 1) {
					UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
				}
			}
		}
	}

	if (!usClientDisplay) {
		/* set print status(journal only) */
		TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL );
	} else {
		REGDISPMSG  RegDispMsgD = {0}, RegDispBack = {0};

		/* drawer open */
		ItemDrawer();

		/* set print status(journal and slip) */
		TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

		/* display "paid out", mnemonic */
		if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
			flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
			flDispRegDescrControl &= ~RECEIPT_CNTRL; 
		}
	 
		if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
			ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
			uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
			if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
				pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
			}
		} 
	    
		RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
		RegDispMsgD.uchMinorClass = CLASS_UIPO;
		RegDispMsgD.lAmount = TrnItemMisc.lAmount;
		RflGetTranMnem( RegDispMsgD.aszMnemonic, TRN_PO_ADR);
		RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
		ItemTendDispIn(&RegDispBack);
		flDispRegDescrControl |= PO_CNTRL; /* set descriptor */
		flDispRegKeepControl &= ~ VOID_CNTRL; /* reset descriptor(keep) */
		DispWriteSpecial( &RegDispMsgD, &RegDispBack);
		flDispRegDescrControl &= ~ ( PO_CNTRL | VOID_CNTRL); /* reset descriptor */

		/* send to enhanced kds, 2172 */
		ItemSendKds(&TrnItemMisc, 0);
	}

    TrnItem( &TrnItemMisc );

	{
		ULONG           ulUserId = 0;
		TRANMODEQUAL    ModeQualRcvBuff;   
		TCHAR           aszSapId[16];

		TrnGetModeQual(&ModeQualRcvBuff);

	    if (ModeQualRcvBuff.ulCashierID != 0)
			ulUserId = ModeQualRcvBuff.ulCashierID;
		else if (ModeQualRcvBuff.ulWaiterID != 0)
			ulUserId = ModeQualRcvBuff.ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

		ConnEngineSendCashierActionWithAmount (CONNENGINE_ACTIONTYPE_PAIDOUT, aszSapId, TrnItemMisc.aszNumber, TrnItemMisc.lAmount, 0);
	}
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscPO(UIFREGMISC *pItemMisc, USHORT  usClientDisplay) 
*                                     
*     Input:    ItemMisc         --  pointer from UI or Connection Engine with data needed
*               usClientDisplay  --  indicates if from UI or Connection Engine
*     Output:   nothing 
*
** Return:      status of the request
*               ITM_SUCCESS  - processing was successful
*
** Description: This function executes "paid out", FSC_PAID_OUT.
*               Data required for a Paid Out is:
*                - account number entered with a #/Type key
*                - currency amount or value paid out
*===========================================================================
fhfh*/
SHORT   ItemMiscPO(UIFREGMISC *pItemMisc, USHORT  usClientDisplay)
{
    SHORT   sStatus;
    USHORT  usRtnLead;

	if (!usClientDisplay) {
		CASIF   CasIf = {0};
		SHORT   sError;

		if ((sError = ItemSaveTrnInformationAndLock (127)) < 0)
			return 1001;

		CasIf.ulCashierNo = pItemMisc->ulEmployeeNo;
		sError = SerCasIndRead(&CasIf);
		if (sError != CAS_PERFORM) {
			return 1002;
		}

		// save the current Transaction environment and open up a new one.
		if ((sStatus = ItemMiscSaveInitializeEnv(&CasIf)) != TRN_SUCCESS) {
			char xBuff[128];
			sprintf (xBuff, "ItemMiscPO(): ItemSaveTrnInformationAndLock - sRetStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT(0, xBuff);
			return 1001;
		}
	}

	if (usClientDisplay) {
#if 0
		// we are removing this check on the entry of an account number for now.
		// reviewing the documentation for Rel 2.1 it shows that the account number
		// is actually an optional entry.
		// We may want to provide a setting to make it mandatory at some future time.
		//    R.Chambers Feb-23-2016
		if (! ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
			// In Rel 2.2.0 and Rel 2.1 we did not have this check on the entry of an Account or Reference number
			// which is part of the Electronic Journal entry.
			// Some customers such as VCS expect to not need to enter an Account Number with Paid Out.
			// Check commented out for Rel 2.3.0.8 since most customers do not use an account number
			// for this action and it is causing confusion.
//			if (pItemMisc->aszNumber[0] == 0) {
//				NHPOS_NONASSERT_NOTE("==DATA", "==DATA: account number entry required.");
//				return LDT_NUM_ADR;
//			}
		}
#endif
		/* executes po check */    
		if ( (sStatus = ItemMiscCheckStateTrans (pItemMisc->lAmount, pItemMisc->fbModifier, HALO_PAIDOUT_ADR) ) != ITM_SUCCESS ) {
			return(sStatus); 
		}

		/* check SPV */
		if ( ( sStatus = ItemMiscPOSPV(pItemMisc) ) != ITM_SUCCESS ) {
			return( sStatus );
		}
	}

	// allow for a reprint of Nosale if desired per request of VCS.
    /* inhibit post receipt,  R3.0 */
//    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

    /* make data and execute open */
	ItemMiscOpenTrans (CLASS_ITEMTRANSOPEN, CLASS_OPENPO);

    /* Transaction module i/f */
    ItemMiscPOItem(pItemMisc, usClientDisplay);

    /* print trailer */
	if (usClientDisplay)
		ItemTrailer(TYPE_STORAGE);
	else
		ItemTrailer(TYPE_STORAGE3);

    /* make data and execute close */
    ItemMiscCloseTrans(CLASS_ITEMTRANSCLOSE, CLASS_CLSPO);

    /* data affection */
	if (usClientDisplay) {
		while ( ( sStatus = TrnSendTotal()) !=  TRN_SUCCESS ) {
			usRtnLead = GusConvertError( sStatus );
			UieErrorMsg( usRtnLead, UIE_WITHOUT );
		}
	} else {
		if ((sStatus = TrnSendTotal()) !=  TRN_SUCCESS ) {
			usRtnLead = GusConvertError( sStatus );
			return usRtnLead;
		}
	}

    /* finalize */                       
    sStatus = ItemMiscRestoreEnv (usClientDisplay);        
    return sStatus;        
}                                        
                                         


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscRASPV(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*               
** Description: This function checks supervisor intervention.  
*===========================================================================
fhfh*/

SHORT   ItemMiscRASPV(UIFREGMISC *ItemMisc)
{
    /* check SPV intervention */
    if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0) != 0 ) {
            return(ItemSPVInt(LDT_SUPINTR_ADR));  /* check SPV INTERVENTION canceled or success */
        }
    } 
    return(ITM_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscRAItem(UIFREGMISC *ItemMisc, USHORT  usClientDisplay) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*
** Description: This function executes making data and transaction i/f .  
*===========================================================================
fhfh*/
VOID    ItemMiscRAItem(UIFREGMISC *ItemMisc, USHORT  usClientDisplay)
{
	ITEMMISC    TrnItemMisc = {0};

    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_RA;

    /* item no. set */
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );

    /* set RA value */
    if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        /* set void bit*/
        TrnItemMisc.fbModifier |= VOID_MODIFIER;
        TrnItemMisc.lAmount = -1 * ItemMisc->lAmount;
	} else {
		TrnItemMisc.lAmount = ItemMisc->lAmount;
	}

    /*  validation check if non-Client source such as Connection Engine */
	if (usClientDisplay) {
		SHORT   sFlag = 0;
		if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
			if ( CliParaMDCCheck( MDC_PO_ADR, ODD_MDC_BIT2) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			} else  if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			}
		} else {
			if ( CliParaMDCCheck( MDC_PO_ADR, ODD_MDC_BIT2) != 0 ) {
				TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
				TrnThrough(&TrnItemMisc);
				sFlag = 1;
			}
		}
		if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    		if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				if (sFlag == 1) {
					UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
				}
			}
		}
	}

	if (!usClientDisplay) {
		/* set print status(journal only) */
		TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL );
	} else {
		REGDISPMSG  RegDispMsgD = {0}, RegDispBack = {0};

		/* drawer open */
		ItemDrawer();

		/* set print status(journal and slip) */
		TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

		/* display "RA", mnemonic */
		if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
			flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
			flDispRegDescrControl &= ~RECEIPT_CNTRL;
		}
	 
		if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
			ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
			uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
			if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
				pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
			}
		} 

		RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
		RegDispMsgD.uchMinorClass = CLASS_UIRA;
		RegDispMsgD.lAmount = TrnItemMisc.lAmount;
		RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_RA_ADR);
		RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
		ItemTendDispIn(&RegDispBack);  /* recover mnemonics */
		flDispRegDescrControl |= ROA_CNTRL; /* set descriptor */
		flDispRegKeepControl &= ~ VOID_CNTRL; /* reset descriptor(keep) */
		DispWriteSpecial( &RegDispMsgD, &RegDispBack);
		flDispRegDescrControl &= ~ ( ROA_CNTRL | VOID_CNTRL); /* reset descriptor */

		/* send to enhanced kds, 2172 */
		ItemSendKds(&TrnItemMisc, 0);
	}

    TrnItem( &TrnItemMisc );

	{
		ULONG           ulUserId = 0;
		TCHAR           aszSapId[16];

	    if (TranModeQualPtr->ulCashierID != 0)
			ulUserId = TranModeQualPtr->ulCashierID;
		else if (TranModeQualPtr->ulWaiterID != 0)
			ulUserId = TranModeQualPtr->ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

		ConnEngineSendCashierActionWithAmount (CONNENGINE_ACTIONTYPE_RECEIVEACCOUNT, aszSapId, TrnItemMisc.aszNumber, TrnItemMisc.lAmount, 0);
	}
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscRA(UIFREGMISC *pItemMisc, USHORT  usClientDisplay) 
*                                     
*     Input:    ItemMisc         --  pointer from UI or Connection Engine with data needed
*               usClientDisplay  --  indicates if from UI or Connection Engine
*     Output:   nothing 
*
** Return:      status of the request
*               ITM_SUCCESS  - processing was successful
*
** Description: This function executes "receive on account", FSC_ROA.
*               Data required for a Receive On Account is:
*                - account number entered with a #/Type key
*                - currency amount or value received
*===========================================================================
fhfh*/
SHORT   ItemMiscRA( UIFREGMISC *pItemMisc, USHORT  usClientDisplay)
{
    SHORT   sStatus;
    USHORT  usRtnLead;

	if (!usClientDisplay) {
		CASIF   CasIf = {0};
		SHORT   sError;

		if ((sError = ItemSaveTrnInformationAndLock (127)) < 0)
			return 1001;

		CasIf.ulCashierNo = pItemMisc->ulEmployeeNo;
		sError = SerCasIndRead(&CasIf);
		if (sError != CAS_PERFORM) {
			return 1002;
		}

		// save the current Transaction environment and open up a new one.
		if ((sStatus = ItemMiscSaveInitializeEnv(&CasIf)) != TRN_SUCCESS) {
			char xBuff[128];
			sprintf (xBuff, "ItemMiscRA(): ItemSaveTrnInformationAndLock - sRetStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT(0, xBuff);
			return 1001;
		}
	}

    /* executes RA check */    
 	if (usClientDisplay) {
#if 0
		// we are removing this check on the entry of an account number for now.
		// reviewing the documentation for Rel 2.1 it shows that the account number
		// is actually an optional entry.
		// We may want to provide a setting to make it mandatory at some future time.
		//    R.Chambers Feb-23-2016
		if (! ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
			// In Rel 2.2.0 and Rel 2.1 we did not have this check on the entry of an Account or Reference number
			// which is part of the Electronic Journal entry.
			// Some customers such as VCS expect to not need to enter an Account Number with ROA.
			// Check commented out for Rel 2.3.0.8 since most customers do not use an account number
			// for this action and it is causing confusion.
//			if (pItemMisc->aszNumber[0] == 0) {
//				NHPOS_NONASSERT_NOTE("==DATA", "==DATA: account number entry required.");
//				return LDT_NUM_ADR;
//			}
		}
#endif
		if ( (sStatus = ItemMiscCheckStateTrans (pItemMisc->lAmount, pItemMisc->fbModifier, HALO_RECVACNT_ADR) ) != ITM_SUCCESS ) {
			return(sStatus); 
		}

		/* check SPV */
		if ( ( sStatus = ItemMiscRASPV(pItemMisc) ) != ITM_SUCCESS ) {
			return( sStatus );
		}
	}

	// allow for a reprint of Nosale if desired per request of VCS.
    /* inhibit post receipt,  R3.0 */
//    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

    /* make data and execute open */
    ItemMiscOpenTrans (CLASS_ITEMTRANSOPEN, CLASS_OPENRA);

    /* Transaction module i/f */
    ItemMiscRAItem(pItemMisc, usClientDisplay);

    /* print trailer */
	if (usClientDisplay)
		ItemTrailer(TYPE_STORAGE);
	else
		ItemTrailer(TYPE_STORAGE3);

    /* make data and execute close */
	ItemMiscCloseTrans (CLASS_ITEMTRANSCLOSE, CLASS_CLSRA);

    /* data affection */
	if (usClientDisplay) {
		while ( ( sStatus = TrnSendTotal()) !=  TRN_SUCCESS ) {
			usRtnLead = GusConvertError( sStatus );
			UieErrorMsg( usRtnLead, UIE_WITHOUT );
		}
	} else {
		if ((sStatus = TrnSendTotal()) !=  TRN_SUCCESS ) {
			usRtnLead = GusConvertError( sStatus );
			return usRtnLead;
		}
	}

    /* finalize */                       
    sStatus = ItemMiscRestoreEnv (usClientDisplay);        
    return sStatus;        
}



/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscTipsPOWaiter(VOID *WaiIf) 
*                                     
*     Input:    pointer -  VOID *WaiIf
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: This function executes waiter id input. R3.1, V3.3
*===========================================================================
fhfh*/

SHORT   ItemMiscTipsPOWaiter(CASIF *CasIfWk)
{
    TRANMODEQUAL    *pTranModeQualL = TrnGetModeQualPtr();
    UIFDIADATA      UifDiaRtn = {0};
	REGDISPMSG      RegDispMsgD = {0};
    CASIF           OutCasIf = {0};   /* V3.3 */
    SHORT           sStatus;
    SHORT           sErrStatus;

    /* display "waiter ID " */
    RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    RegDispMsgD.uchMinorClass = CLASS_REGDISP_CASHIERNO;    /* V3.3 */
    RflGetLead (RegDispMsgD.aszMnemonic, LDT_ID_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    flDispRegDescrControl |= COMPLSRY_CNTRL; /* set descriptor */
    flDispRegKeepControl |= COMPLSRY_CNTRL; /* set descriptor(keep) */
    DispWrite( &RegDispMsgD );
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL; /* reset descriptor */
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL; /* reset descriptor(keep) */
                                                          
    /* execute cashier no. comuplsory  and check return data, V3.3 */
    if ( ( sStatus = UifDiaCashier( &UifDiaRtn, UIFREG_ECHO ) ) != UIF_SUCCESS ) {
        ItemTransDispDef(); /* recover display message */
        return(UIF_CANCEL); /* corection10/06 */
    }

    if (UifDiaRtn.auchFsc[0] == FSC_MSR) {
		UIFDIAEMPLOYEE  UifDiaEmployee = {0};
		sStatus = UifTrackToUifDiaEmployee (UifDiaRtn.auchTrack2, UifDiaRtn.uchTrack2Len, &UifDiaEmployee);
		UifDiaEmployee.ulStatusFlags = UifDiaRtn.ulStatusFlags;        // get the UIFREGMISC_STATUS_METHOD_MASK indicators
		if (sStatus != UIF_SUCCESS)
			return LDT_NTINFL_ADR;
        if ((UifDiaEmployee.usSwipeOperation != MSR_ID_CASHIER) &&  /* V3.3 */
            (UifDiaEmployee.usSwipeOperation != MSR_ID_SERVER) &&
            (UifDiaEmployee.usSwipeOperation != MSR_ID_BARTENDER)) {
            return(LDT_PROHBT_ADR);
        }

		UifDiaRtn.ulData = UifDiaEmployee.ulSwipeEmployeeId;
    } else if (UifDiaRtn.auchFsc[0] == FSC_WAITER_PEN) {       /* V3.3 */
        if (UifDiaRtn.auchFsc[1]) {                     /* insert */
            ItemOtherDiaRemoveKey();                    /* dialogue function  to remove waiter key */
        } else {                                        /* remove */
			ULONG  ulStatus = 0;

            UifDiaRtn.ulData = ItemOtherDiaInsertKey();  /* dialogue function  to insert waiter key */
            if (pTranModeQualL->ulCashierID == UifDiaRtn.ulData) {
                ItemTransDispDef();
                return(LDT_PROHBT_ADR);
            }
            ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */
            do {
                ulStatus = ItemOtherDiaInsertKey();     /* dialogue function  to insert waiter key */
                if (ulStatus != pTranModeQualL->ulCashierID) {
                    UieErrorMsg( LDT_KEYOVER_ADR, UIE_WITHOUT ); /* display error and wait for clear key */   
                    ItemOtherDiaRemoveKey();                   /* dialogue function  to remove waiter key */
                }
            } while (ulStatus != pTranModeQualL->ulCashierID);
        }
        ItemTransDispDef();
    }

    if (pTranModeQualL->ulCashierID == UifDiaRtn.ulData) {
        return(LDT_PROHBT_ADR);
    }

    CasIfWk->ulCashierNo = UifDiaRtn.ulData;
    CasIfWk->ulCashierOption |= CAS_MISC_SIGN_IN;    /* as surrogate sign-in */

    /* execute sign in */
    CasIfWk->usUniqueAddress = (USHORT) CliReadUAddr(); /* get unique address */
    if ( ( sErrStatus = CliCasSignIn( CasIfWk ) ) != CAS_PERFORM ) {
        sStatus = CasConvertError( sErrStatus );
        return(sStatus);
    }

    /* check same mode or not */
    /*  if training waiter and normal mode, set err sts and waisignout */
    if ( CasIfWk->fbCashierStatus[0] & CAS_TRAINING_CASHIER ){
        if ( (pTranModeQualL->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) != MODEQUAL_CASHIER_TRAINING ) {
            ItemCasSignInCancel(CasIfWk); /* sign-out */
            return( LDT_PROHBT_ADR );
        }
    } else { 
        /*  if normal waiter and training mode */
        if ( (pTranModeQualL->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) == MODEQUAL_CASHIER_TRAINING ) {
            ItemCasSignInCancel(CasIfWk); /* sign-out */
            return( LDT_PROHBT_ADR );
        }
    }

    return(ITM_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscTipsPOSPV(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: This function checks supervisor intervention.  V3.3
*===========================================================================
fhfh*/
static SHORT   ItemMiscTipsPOSPV(UIFREGMISC *ItemMisc)
{
    /* check SPV intervention */
    if ( CliParaMDCCheck( MDC_TIPPO1_ADR, ODD_MDC_BIT0) != 0 ) {
        return(ItemSPVInt(LDT_SUPINTR_ADR)); 
    } else if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0) != 0 ) {
            return(ItemSPVInt(LDT_SUPINTR_ADR)); 
        }
    }

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsPOOpen(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*               
** Return:      ITM_SUCCESS
*               
** Description: This function executes making open data and .  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsPOOpen(VOID)
{
	ITEMTRANSOPEN   ItemTransOpen = {0};
    TRANCURQUAL     *pTranCurQualD = TrnGetCurQualPtr();

    /* set class code */
    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = CLASS_OPENTIPSPO;
    ItemTransOpen.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

    /* consecutive no. and current qualifier count up on */
    ItemCountCons();

    /* set option to current qualifier(current print status) */
    ItemCurPrintStatus();

	pTranCurQualD->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */

	/* check unbuffering or other */      
    /* if unbuffering, slip compulsory bit off */
	pTranCurQualD->uchPrintStorage = PRT_ITEMSTORAGE;
    if ( (pTranCurQualD->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
		// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
		// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
		// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
		pTranCurQualD->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
        pTranCurQualD->fbCompPrint &= ~ CURQUAL_COMP_S;  /* slip bit off*/
    }
	pTranCurQualD->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */

    /* check receipt issue or not */
    if ( CliParaMDCCheck( MDC_TIPPO2_ADR, EVEN_MDC_BIT0) != 0 ) {
        pTranCurQualD->fbNoPrint |= ( PRT_RECEIPT | PRT_SLIP );
    } else {
        pTranCurQualD->fbNoPrint &= ~ PRT_RECEIPT;  /* reset receipt*/
        pTranCurQualD->fbCompPrint = PRT_RECEIPT;    
    }

	ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    ItemPromotion( PRT_RECEIPT, TYPE_STORAGE);
    ItemHeader( TYPE_STORAGE );
    TrnOpen( &ItemTransOpen );  /* transaction open */

    /*--- LCD Initialize ---*/
    MldDisplayInit(MLD_DRIVE_1, 0);

    /*--- Display Cashier/Waiter No. ---*/
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsPOItem(UIFREGMISC *ItemMisc, VOID *WaiIf) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*               
** Description: This function executes making data and transacion i/f.  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsPOItem(UIFREGMISC *ItemMisc, CASIF  *CasIfWk ) 
{
	ITEMMISC    TrnItemMisc = {0};
    REGDISPMSG  RegDispMsgD = {0}, RegDispBack = {0};
    SHORT       sFlag = 0;

    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_TIPSPO;

    /* item no. set */
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );

    /* set PO value */
    TrnItemMisc.lAmount = -1*(ItemMisc->lAmount);

    TrnItemMisc.ulID = CasIfWk->ulCashierNo; /* set waiter ID*/

    /*  validation check */
    if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        TrnItemMisc.fbModifier = VOID_MODIFIER;
        TrnItemMisc.lAmount = -1 * TrnItemMisc.lAmount;
        if (  CliParaMDCCheck( MDC_TIPPO1_ADR, ODD_MDC_BIT1) != 0  
            || CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1) != 0 ) {
            TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
            TrnThrough(&TrnItemMisc);
            sFlag = 1;
        }
    } else {
        if ( CliParaMDCCheck( MDC_TIPPO1_ADR, ODD_MDC_BIT1) != 0 ) {
            TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
            TrnThrough(&TrnItemMisc);
            sFlag = 1;
        }
    }
    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    /* drawer open */
    ItemDrawer();

    /* check receipt issue or not */
    TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

    /* display "TIPPO", mnemonic */
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
 
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
        }
    } 

    RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
    RegDispMsgD.uchMinorClass = CLASS_UITIPSPO;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_TIPPO_ADR);
    RegDispMsgD.lAmount = TrnItemMisc.lAmount;
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    ItemTendDispIn(&RegDispBack);
    flDispRegDescrControl |= TIPSPO_CNTRL; /* set descriptor */
    flDispRegKeepControl &= ~ VOID_CNTRL; /* reset descriptor(keep) */
    DispWriteSpecial( &RegDispMsgD, &RegDispBack);
    flDispRegDescrControl &= ~ ( TIPSPO_CNTRL | VOID_CNTRL); /* reset descriptor */

    TrnItemMisc.ulID = CasIfWk->ulCashierNo; /* set waiter ID*/

    TrnItem( &TrnItemMisc );

	{
		ULONG           ulUserId = 0;
		int             nLength;
		TCHAR           aszSapId[16];
		TCHAR           tcsAddOnString[512], *ptcsBuffer;

		ptcsBuffer = tcsAddOnString;

	    if (TranModeQualPtr->ulCashierID != 0)
			ulUserId = TranModeQualPtr->ulCashierID;
		else if (TranModeQualPtr->ulWaiterID != 0)
			ulUserId = TranModeQualPtr->ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

#if defined(DCURRENCY_LONGLONG)
		nLength = _stprintf (ptcsBuffer, _T("<lAmount>%lld</lAmount>\r"), TrnItemMisc.lAmount);
#else
		nLength = _stprintf(ptcsBuffer, _T("<lAmount>%ld</lAmount>\r"), TrnItemMisc.lAmount);
#endif
		if (nLength >= 0)
			ptcsBuffer += nLength;
		nLength = _stprintf (ptcsBuffer, _T("<ulEmployeeIdPaidTo>%d</ulEmployeeIdPaidTo>\r"), TrnItemMisc.ulID);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		ConnEngineSendCashierActionWithAddon (_T("tips-paid-out"), aszSapId, tcsAddOnString);
	}
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsPOClose(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes making close data and transaction i/f.  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsPOClose(VOID)
{
    TRANCURQUAL     *pTranCurQual = TrnGetCurQualPtr();
	ITEMTRANSCLOSE  ItemTransClose = {0};

    /* set class code */
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSTIPSPO;
    TrnClose( &ItemTransClose );
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsPOSignOut(VOID *WaiIf) 
*                                     
*     Input:    pointer - VOID *WaiIf
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes making close data and transaction i/f.  V3.3
*===========================================================================
fhfh*/
VOID    ItemMiscTipsPOSignOut(CASIF  *CasIfWk)
{
	ITEMOPECLOSE   ItemOpeClose = {0};
    SHORT          sErrStatus;

    ItemOpeClose.uchMajorClass = CLASS_ITEMOPECLOSE;
    ItemOpeClose.uchMinorClass = CLASS_CASHIERMISCOUT;
    ItemOpeClose.ulID = CasIfWk->ulCashierNo;
    ItemOpeClose.uchUniqueAdr = CliReadUAddr();

    /* execute transaction close*/
    TrnClose( &ItemOpeClose);

    /* data affection */
    for (;;) {
        /* R3.3 */
        if ((sErrStatus = CliCasPreSignOut(CasIfWk)) != CAS_PERFORM ) {
            /* wait for previous cashier sign-out for cashier interrupt */                
            /* UieErrorMsg(CasConvertError(sReturnStatus), UIE_WITHOUT); */
            CliSleep();
            continue;
        } else {
            break;
        }
    }
    while ( (sErrStatus = TrnSendTotal() ) !=  TRN_SUCCESS ) {
		USHORT  usRtnLead = GusConvertError( sErrStatus );
        UieErrorMsg( usRtnLead, UIE_WITHOUT );
    }
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscTipsPO(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: This function executes tips paid out, FSC_TIPS_PAID.  V3.3
*               This function involves tips while function ItemMiscPO() documents
*               a general money transfer out of the drawer.
*===========================================================================
fhfh*/

SHORT   ItemMiscTipsPO(UIFREGMISC *ItemMisc)
{
	CASIF   CasIf = {0};  /* V3.3 */
    SHORT   sStatus;
    SHORT   sErrStatus;

    /* --- if current system is Store/Recall, prohibit "tips paid out"
        operation, because Store/Recall has no waiter. --- */
    if ( RflGetSystemType () == FLEX_STORE_RECALL ) {
        return ( LDT_SEQERR_ADR );
    }

	/* executes TipsPO check */
    if ( (sStatus = ItemMiscCheckStateTrans (ItemMisc->lAmount, ItemMisc->fbModifier, HALO_TIPPAID_ADR) ) != ITM_SUCCESS ) {
        return(sStatus); 
    }

    /* check Supervisor Intervention required or not */
    if ( ( sStatus = ItemMiscTipsPOSPV( ItemMisc ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    /* executes waiter id input */    
    if ( (sStatus = ItemMiscTipsPOWaiter( &CasIf ) ) != ITM_SUCCESS ) {
        return(sStatus); 
    }

	// allow for a reprint of Nosale if desired per request of VCS.
    /* inhibit post receipt,  R3.0 */
//    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

	/* make data and execute open */
    ItemMiscTipsPOOpen();

    /* Transaction module i/f */
    ItemMiscTipsPOItem(ItemMisc, &CasIf);

    /* print trailer */
    ItemTrailer(TYPE_STORAGE);

    /* make data and execute close */
    ItemMiscTipsPOClose();

    /* data affection */
    while ( ( sErrStatus = TrnSendTotal() )  !=  TRN_SUCCESS ) {
		USHORT  usRtnLead = GusConvertError( sErrStatus );
        UieErrorMsg( usRtnLead, UIE_WITHOUT );
    }

    /* initialize transaction informaation */
    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    /* sign out*/
    ItemMiscTipsPOSignOut( &CasIf );

    /* initialize transaction informaation */
    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */

    /* finalize */
    return( ItemFinalize() );

}


/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsDecOpen( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*               
** Description: This function executes making open data and transaction i/f.  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsDecOpen(VOID) 
{
    TRANCURQUAL     *pTranCurQualD = TrnGetCurQualPtr();
	ITEMTRANSOPEN   ItemTransOpen = {0};
 
    /* set class code */
    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = CLASS_OPENTIPSDECLARED;
    ItemTransOpen.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

    /* consecutive no. and current qualifier count up on */
    ItemCountCons();

    /* set option to current qualifier(current print status) */
    ItemCurPrintStatus();

	pTranCurQualD->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */

	/* check unbuffering or other */      
    /* if unbuffering, slip compulsory bit off */
	pTranCurQualD->uchPrintStorage = PRT_ITEMSTORAGE;
    if ( (pTranCurQualD->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
		// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
		// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
		// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
		pTranCurQualD->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
        pTranCurQualD->fbCompPrint &= ~ CURQUAL_COMP_S;  /* slip bit off*/
    }
	pTranCurQualD->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */

    /* check receipt issue or not */
    if ( CliParaMDCCheck( MDC_TIPPO2_ADR, EVEN_MDC_BIT0) != 0 ) {
        pTranCurQualD->fbNoPrint |= ( PRT_RECEIPT | PRT_SLIP );
    } else {
        pTranCurQualD->fbNoPrint &= ~ PRT_RECEIPT; /* reset receipt*/
        pTranCurQualD->fbCompPrint = PRT_RECEIPT;
    }

	ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    ItemPromotion( PRT_RECEIPT, TYPE_STORAGE);
    ItemHeader( TYPE_STORAGE );
    TrnOpen( &ItemTransOpen );  /* transaction open */

    /*--- LCD Initialize ---*/
    /*--- Display Cashier/Waiter No. ---*/
    MldDisplayInit(MLD_DRIVE_1, 0);
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsDecItem(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      nothing
*               
** Description: This function executes making data and transaction i/f.  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsDecItem(UIFREGMISC *ItemMisc)
{
    ITEMMISC    TrnItemMisc = {0};
	REGDISPMSG  RegDispMsgD = {0}, RegDispBack = {0};
    SHORT       sFlag = 0;
    
    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_TIPSDECLARED;

    /* item no. set */
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );

    /* set Tips Declared value */
    TrnItemMisc.lAmount = ItemMisc->lAmount;

    /*  validation check */
    if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        /* set void bit*/
        TrnItemMisc.fbModifier = VOID_MODIFIER;
        TrnItemMisc.lAmount *= -1;
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1) != 0 ) {
            TrnItemMisc.fsPrintStatus = PRT_VALIDATION; /* set validation */
            TrnThrough(&TrnItemMisc);
            sFlag = 1;
        }
    }
    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    /* check receipt issue or not */
    TrnItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );

    /* display "TIPS DECLARED", mnemonic */
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
 
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
        }
    } 

    RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
    RegDispMsgD.uchMinorClass = CLASS_UITIPSDECLARED;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_DECTIP_ADR);
    RegDispMsgD.lAmount =  TrnItemMisc.lAmount;
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    ItemTendDispIn(&RegDispBack);
    flDispRegDescrControl |= DECTIP_CNTRL; /* set descriptor */
    flDispRegKeepControl &= ~ VOID_CNTRL; /* reset descriptor(keep) */
    DispWriteSpecial( &RegDispMsgD, &RegDispBack);
    flDispRegDescrControl &= ~ ( DECTIP_CNTRL | VOID_CNTRL); /* reset descriptor */
 
    TrnItem( &TrnItemMisc );

	{
		ULONG           ulUserId = 0;
		int             nLength;
		TCHAR           aszSapId[16];
		TCHAR           tcsAddOnString[512], *ptcsBuffer;

		ptcsBuffer = tcsAddOnString;

	    if (TranModeQualPtr->ulCashierID != 0)
			ulUserId = TranModeQualPtr->ulCashierID;
		else if (TranModeQualPtr->ulWaiterID != 0)
			ulUserId = TranModeQualPtr->ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

#if defined(DCURRENCY_LONGLONG)
		nLength = _stprintf (ptcsBuffer, _T("<lAmount>%lld</lAmount>\r"), TrnItemMisc.lAmount);
#else
		nLength = _stprintf(ptcsBuffer, _T("<lAmount>%ld</lAmount>\r"), TrnItemMisc.lAmount);
#endif
		if (nLength >= 0)
			ptcsBuffer += nLength;

		ConnEngineSendCashierActionWithAddon (_T("tips-declared"), aszSapId, tcsAddOnString);
	}
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscTipsDecClose(VOID) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: This function executes making data and transaction i/f.  V3.3
*===========================================================================
fhfh*/

VOID    ItemMiscTipsDecClose(VOID)
{
    TRANCURQUAL     *pTranCurQual = TrnGetCurQualPtr();
	ITEMTRANSCLOSE  ItemTransClose = {0};

    /*
	 * the function TrnStoGetConsToPostR() must be used to transfer the last transaction
	 * to the Post Receipt storage area in order for Post Receipt or Print on Demand to work
	 * by using TrnStoPutPostRToCons() to retrieve the saved data in the Post Receipt storage area.
	**/
    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
    ItemMiscSetStatus(MISC_POSTRECPT | MISC_PRINTDEMAND);  // for No Sale allow reprint
    TrnStoGetConsToPostR();

    /* set class code */
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSTIPSDECLARED;

    TrnClose( &ItemTransClose );
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscTipsDec(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: This function executes Tips Declare.  V3.3
*===========================================================================
fhfh*/
SHORT   ItemMiscTipsDec(UIFREGMISC *ItemMisc)
{
    SHORT   sStatus;

    /* executes TipsDec check */
    if ( (sStatus = ItemMiscCheckStateTrans (ItemMisc->lAmount, ItemMisc->fbModifier, HALO_DCTIP_ADR) ) != ITM_SUCCESS ) {
        return(sStatus); 
    }

    /* check SPVITM_SUCCESS, LDT_SEQERR_ADR intervention */
    if ( ItemMisc->fbModifier & VOID_MODIFIER ) {
        if ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0) != 0 ) {
			/* check SPV INTERVENTION canceled or success */
			if ( ( sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
				return( sStatus );
			}
		}
    }

	// allow for a reprint of Nosale if desired per request of VCS.
    /* inhibit post receipt,  R3.0 */
//    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );

    /* make data and execute open */
    ItemMiscTipsDecOpen();

    /* Transaction module i/f */
    ItemMiscTipsDecItem(ItemMisc);

    /* print trailer */
    ItemTrailer(TYPE_STORAGE);

    /* make data and execute close */
    ItemMiscTipsDecClose();

    /* data affection */
    while ( ( sStatus = TrnSendTotal() ) != TRN_SUCCESS ) {
		USHORT  usRtnLead = GusConvertError( sStatus );
        UieErrorMsg( usRtnLead, UIE_WITHOUT );
    }

    /* initialize transaction informaation */
    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */

    /* finalize */
    return(ItemFinalize());

}



/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscCheckOpen(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*               
** Description: This function executes making open data and transaction i/f   
*===========================================================================
fhfh*/

VOID    ItemMiscCheckOpen(VOID)
{
	ITEMTRANSOPEN   ItemTransOpen = {0};

    ItemCurPrintStatus();  /* set option to current qualifier(current print status) */
	{
		TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */
		pCurQualRcvBuff->uchPrintStorage = PRT_ITEMSTORAGE;
		if ( (pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
			// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
			// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
			// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
			pCurQualRcvBuff->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
			/* if unbuffering system type, set slip compulsory bit off */
			pCurQualRcvBuff->fbCompPrint &= ~ CURQUAL_COMP_S;  /* slip bit off*/
		}
		pCurQualRcvBuff->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */
	}

    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = CLASS_OPENCHECKTRANSFER;
    ItemTransOpen.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );
    ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
	TrnOpen( &ItemTransOpen );  /* transaction open */

    /*--- LCD Initialize ---*/
    MldDisplayInit(MLD_DRIVE_1, 0);

    /*--- Display Cashier/Waiter No. ---*/
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);

	// display a list of outstanding guest checks for this operator who doing guest check transfer.
	// we can provide different formats of the data such as display by guest check number
	// or display by table number. the legacy is to display by guest check number.
//	ItemOtherDispGuestNo(TranModeQualPtr->ulCashierID);
    ItemOtherDispTableNo(TranModeQualPtr->ulCashierID);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTRIn(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: This function executes "check transfer in", FSC_GUEST_CHK_TRN.  
*===========================================================================
fhfh*/

SHORT   ItemMiscCheckTRIn( VOID )
{
	REGDISPMSG      RegDispMsgD = {0};
    SHORT           sStatus;

    /* check store/recall sys,         R3.0 */
    if ( RflGetSystemType () == FLEX_STORE_RECALL ) {
        return( LDT_SEQERR_ADR ); /* if store/recall ,set error */
    }

    /* check if tax modifier key or food stamp modifier was used ? */
    if ( ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) { /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check p-void use ? */
    if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {
        return(LDT_SEQERR_ADR); 
    }

    /* check SPV intervention */
    if ( CliParaMDCCheck (MDC_GCTRN_ADR, ODD_MDC_BIT0) != 0 ) {
		if( (sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
			return( sStatus );
		}
    }

    /* display GCF mnemonic */
    RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    RegDispMsgD.uchMinorClass = CLASS_REGDISP_GCNO;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_GCNO_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_1; /* set type1 */
    flDispRegDescrControl |= TRANSFR_CNTRL; /* set descriptor */
    flDispRegKeepControl |= TRANSFR_CNTRL; /* set descriptor(keep) */
    DispWrite( &RegDispMsgD );
    RegDispMsgD.fbSaveControl = (DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_4); 
    DispWrite( &RegDispMsgD );

    /* transaction open */
    ItemMiscCheckOpen();

    return(ITM_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTransCheck(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR, UIF_CANCEL
*               
** Description: This function checks whether to be able to execute 
*               "check transfer" or not.
*===========================================================================
fhfh*/

static SHORT   ItemMiscCheckTransCheck( VOID )
{
    TRANGCFQUAL     *pTranGCFQualL = TrnGetGCFQualPtr();
    TRANMODEQUAL    *pTranModeQualL = TrnGetModeQualPtr();
    TRANCURQUAL     *pTranCurQualL = TrnGetCurQualPtr();

/*** ignore GCFQUAL_NEWCHECK_CASHIER status, V3.3
    if (  TranGCFQualL->fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER ) {
        return( LDT_PROHBT_ADR );
    }
****/
    /* check same waiter or success */
/*    if ( TranGCFQualL->usCashierID == TranModeQualL.usCashierID ) {   V3.3
    if ( TranGCFQualL->usWaiterID == TranModeQualL.usWaiterID ) { V3.3
        return( LDT_PROHBT_ADR );
    }
*/
    /* check GCF(training mode) */
    if ( pTranGCFQualL->fsGCFStatus & GCFQUAL_TRAINING ) {
        if ( (pTranCurQualL->fsCurStatus & CURQUAL_TRAINING ) != CURQUAL_TRAINING ) {
            return( LDT_PROHBT_ADR );
        }
    } else { 
        if ( ( pTranCurQualL->fsCurStatus & CURQUAL_TRAINING) == CURQUAL_TRAINING) {
            return( LDT_PROHBT_ADR );
        }
    }

    /* check guest check team no, V3.3 */
    if (pTranModeQualL->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM) {               /* not use team */
        if (pTranModeQualL->uchCasTeamNo != 0) {                          /* not head cashier */
            if (pTranModeQualL->uchCasTeamNo != pTranGCFQualL->uchTeamNo) {/* team no unmatch */
                return( LDT_PROHBT_ADR );
            }
        }
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    ItemMiscCheckTransWrite(SHORT sType)
*                                     
*     Input:    nothing
*    Output:    nothing 
** Return:      ITM_SUCCESS
*               
** Description: This function write contents of GCF file.           V3.3
*===========================================================================
*/
static VOID    ItemMiscCheckTransWrite(SHORT sType, DCURRENCY lAmount)
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();

    if (sType == 0) {
        /* set previous operator ID, V3.3 */
        WorkGCF->ulPrevCashier = WorkGCF->ulCashierID;
        WorkGCF->ulCashierID = TranModeQualPtr->ulCashierID;
/*    TranGCFQualL->usPrevWaiter = TranGCFQualL->usWaiterID;
        TranGCFQualL->usWaiterID = TranModeQualL.usWaiterID; */
        
	    WorkGCF->lTranBalance = WorkGCF->lCurBalance;
    } else {
		/* add previous check amount to next check, V1.0.15 */
        if (WorkGCF->ulCashierID == TranModeQualPtr->ulCashierID) {
		    WorkGCF->lTranBalance += lAmount;
		    WorkGCF->lCurBalance  += lAmount;
		} else {
			WorkGCF->ulPrevCashier = TranModeQualPtr->ulCashierID;

		    WorkGCF->lTranBalance = WorkGCF->lCurBalance;
		}
    }
    /* WorkGCF->lTranBalance = WorkGCF->lCurBalance; */
}

/*
*===========================================================================
** Synopsis:    VOID    ItemMiscCheckTransAffect(ITEMMISC *pMiscFrom)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*               
** Description: Check Transfer Affection.                       V3.3
*
*               See as well function ItemTendAffectTransFromTo()
*               which does a similar affectation operation.
*               Waiter functionality is a legacy from NCR 2170 Hospitality
*               so this affectation may not be actually affecting the totals.
*===========================================================================
*/
static VOID    ItemMiscCheckTransAffect(ITEMMISC *pMiscFrom)
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
	ITEMAFFECTION   Affect = {0};

    /* set old waiter */
    Affect.uchMajorClass = CLASS_ITEMAFFECTION ;
    Affect.uchMinorClass = CLASS_OLDWAITER;
    if (pMiscFrom->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        Affect.lAmount = pMiscFrom->lAmount * -1L;
        Affect.ulId = pMiscFrom->ulID;
    } else {
        Affect.lAmount = WorkGCF->lCurBalance * -1L;
        Affect.ulId = WorkGCF->ulPrevCashier;
    }
    Affect.sNoPerson = WorkGCF->usNoPerson * -1;
    Affect.sCheckOpen = -1;

    /* check P-void GCF or Normal one */
    if (WorkGCF->fsGCFStatus & GCFQUAL_PVOID) {
        Affect.sNoPerson = WorkGCF->usNoPerson;
        Affect.sCheckOpen = 1;
    }
    TrnAffection (&Affect);

    /* set new waiter */
    Affect.uchMinorClass = CLASS_NEWWAITER;
    if (pMiscFrom->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        Affect.lAmount *= -1L;
    } else {    
        Affect.lAmount = WorkGCF->lCurBalance;
    }
    Affect.ulId = WorkGCF->ulCashierID;     /* set new cashier ID, V3.3 */
    Affect.sNoPerson *= -1;
    Affect.sCheckOpen *= -1;
    TrnAffection (&Affect);
}

/*
*===========================================================================
** Synopsis:    VOID    ItemMiscCheckTransfer(UIFREGMISC *UifMisc,
*                               ITEMMISC *pMiscFrom, ITEMMISC *pMiscTo)
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
** Return:      ITM_SUCCESS
*               
** Description: Transaction i/f.                                V3.3
*===========================================================================
*/
static VOID    ItemMiscCheckTransfer(UIFREGMISC *UifMisc, ITEMMISC *pMiscFrom, ITEMMISC *pMiscTo)
{
    DCURRENCY       lAmount;
    ITEMMISC        Misc = {0};
	REGDISPMSG      Disp = {0};

    Misc.uchMajorClass = CLASS_ITEMMISC;
    _tcsncpy(Misc.aszNumber, UifMisc->aszNumber, NUM_NUMBER);
    Misc.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;
    
    if (pMiscFrom->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        Misc.uchMinorClass = CLASS_CHECKTRANS_FROM;
        Misc.ulID = pMiscFrom->ulID;
        Misc.usGuestNo = pMiscFrom->usGuestNo;
        Misc.uchCdv = pMiscFrom->uchCdv;
        Misc.lAmount = lAmount = pMiscFrom->lAmount;
        TrnMisc (&Misc);

        Misc.uchMinorClass = CLASS_CHECKTRANS_TO;
        Misc.ulID = pMiscTo->ulID;
        Misc.usGuestNo = pMiscTo->usGuestNo;
        Misc.uchCdv = pMiscTo->uchCdv;
        Misc.lAmount = lAmount = pMiscTo->lAmount;
        TrnMisc (&Misc);
    } else {
		TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
        Misc.uchMinorClass = CLASS_CHECKTRANSFER;
		Misc.ulID = WorkGCF->ulPrevCashier;         /* set old cashier ID, V3.3 */
        Misc.usGuestNo = WorkGCF->usGuestNo;
        Misc.uchCdv = WorkGCF->uchCdv;
        Misc.lAmount = lAmount = WorkGCF->lCurBalance;
        TrnMisc (&Misc);
    }

    Disp.uchMajorClass = CLASS_UIFREGMISC;
    Disp.uchMinorClass = CLASS_UICHECKTRANSFER;
    RflGetTranMnem (Disp.aszMnemonic, TRN_TRNSB_ADR);
    Disp.lAmount = lAmount;
    DispWrite(&Disp);
}


/*
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTrans(UIFREGMISC *UifMisc)
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: This function executes "transfer in".           V3.3
*               This function is invoked by using the Guest Check Transfer key
*               to transfer an existing guest check from one operator to another.
*===========================================================================
*/
SHORT   ItemMiscCheckTrans(UIFREGMISC *UifMisc)
{
    TRANGCFQUAL     *pGCF = TrnGetGCFQualPtr();
    TRANINFORMATION *pTrnInfo = TrnGetTranInformationPointer();
    TRANITEMIZERS   *pItem = TrnGetTIPtr();
    TRANMODEQUAL    *pWorkMode = TrnGetModeQualPtr();
	ITEMMISC        MiscFrom = {0}, MiscTo = {0};
	TRANCONSSTORAGEHEADER    uchGCFData = {0};
    SHORT           sStatus = ITM_SUCCESS, sType = 0, sGive = 0;
	//11-1103 JHHJ SR 201

    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		// This operation not allowed with a Store Recall System.
        return(LDT_PROHBT_ADR); 
	}

    /* check if tax modifier key was used ? */
    if (ItemModGetLocalPtr()->fsTaxable != 0) {
		/* sequence error if tax modifier key has been used */
        return(LDT_SEQERR_ADR); 
    }

	{
		LONG      lGestNoRtn = _ttol(&UifMisc->aszGuestNo[0]);
		RflGcfNo  gcfCd;

		gcfCd = RflPerformGcfCd (lGestNoRtn);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		MiscFrom.usGuestNo = gcfCd.usGcNumber;
	}

	sType = 0;     // assume Pre-check type system without transaction data.
    if (RflGetSystemType () == FLEX_POST_CHK) {
		// indicate that we need the consolidated transaction data storage as well.
		// Store/Recall and Post Check both store additional memory resident guest check data
		// in a guest check stored in the Guest Check file.
        sType = 1;
    }

    /*  check GC No,    R3.0 */
    sStatus = TrnGetGC(MiscFrom.usGuestNo, sType, GCF_REORDER);
    if (sStatus < 0) {
		USHORT  usRtnLead = GusConvertError(sStatus);
        return((SHORT)usRtnLead);
#if 0
    /* --- No size GCF --- */
	// There are circumstances in which a guest check
	// may have a zero size.  For instance if a terminal
	// had created a guest check and then experienced a
	// failure before the check could be serviced or closed
	// out.  We need to take this eventuallity under consideration
	// as some countries do have problems with power and
	// those customers do need to be able to recover from a power
	// failure scenario.
	// TrnGetGC() should fix up the basic check information if the
	// check size is zero by replacing the guest check number, etc.
    } else if (sStatus == 0) {                      /* no size GCF */
        WorkGCF.usGuestNo = MiscFrom.usGuestNo;
        ItemCommonCancelGC(&WorkGCF);
        return(LDT_ILLEGLNO_ADR);
#endif
    }

    /* error check */
    if (ItemMiscCheckTransCheck() != ITM_SUCCESS) {
		TRANGCFQUAL     WorkGCF = {0};
        WorkGCF.usGuestNo = MiscFrom.usGuestNo;
        ItemCommonCancelGC(&WorkGCF);
        return(LDT_GCFSUNMT_ADR);
    }

    /* --- Check Transfer: GC to GC,    V3.3 --- */
    sGive = 0;  // indicates if this is a give and the original should be deleted.
    if (pGCF->ulCashierID == pWorkMode->ulCashierID) {
        if (pWorkMode->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_TRANSFER_TO) {
			TRANGCFQUAL     WorkGCF = {0};
            WorkGCF.usGuestNo = MiscFrom.usGuestNo;
            ItemCommonCancelGC(&WorkGCF);
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier record status CAS_NOT_TRANSFER_TO.");
            return(LDT_PROHBT_ADR);
        }
        MiscFrom.uchMajorClass = MiscTo.uchMajorClass = CLASS_ITEMMISC;
        MiscFrom.uchMinorClass = CLASS_CHECKTRANS_FROM;
        MiscTo.uchMinorClass = CLASS_CHECKTRANS_TO;
        MiscFrom.uchCdv = pGCF->uchCdv;
        MiscFrom.ulID = pWorkMode->ulCashierID;
        MiscFrom.lAmount = pItem->lMI;
        
        if ((sStatus = ItemMiscCheckTransGC2GC(&MiscTo)) != ITM_SUCCESS) {
			TRANGCFQUAL     WorkGCF = {0};
            WorkGCF.usGuestNo = MiscFrom.usGuestNo;		/* Add TAR170599 May-2-01 */
            ItemCommonCancelGC(&WorkGCF);			/* Change TAR170599 May-2-01 */
            return(sStatus);
        }
        sGive = 1;  // we are giving or transfering this guest check so delete the original.
    } else {
        /* prohibit check transfer from status, V3.3 */
        if (pWorkMode->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_TRANSFER_FROM) {
            ItemCommonCancelGC(pGCF);
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier record status CAS_NOT_TRANSFER_FROM.");
            return(LDT_PROHBT_ADR);
        }
        MiscTo.usGuestNo = MiscFrom.usGuestNo;
    }
    
    ItemCountCons();

    ItemMiscCheckTransWrite(sGive, MiscFrom.lAmount);

    /* set GCF, ITEMIZERS to consoli. file */
    /* --- Set Consoli. Storage Size,   V3.3 --- */
	uchGCFData.TranGCFQual = *pGCF;
	uchGCFData.TranItemizers = *pItem;
	uchGCFData.usVli = pTrnInfo->usTranConsStoVli;

    TrnWriteFile(sizeof(USHORT) + sizeof(USHORT), &uchGCFData.TranGCFQual,
        sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS) + sizeof(USHORT), pTrnInfo->hsTranConsStorage);

    /* save GC to GC module, V3.3 */
    if (RflGetSystemType () == FLEX_POST_CHK) {
        sType = GCF_COUNTER_TYPE;
    } else {
        sType = GCF_COUNTER_NOCONSOLI;
    }
    /* save storage data with index */
    while ((sStatus = TrnSaveGC(GCF_COUNTER_TYPE, MiscTo.usGuestNo)) != TRN_SUCCESS) {
		USHORT  usRtnLead = GusConvertError(sStatus);
        UieErrorMsg(usRtnLead, UIE_WITHOUT);
    }
    TrnInitialize(TRANI_ITEM | TRANI_CONSOLI);  /* initailize Item storage */
    ItemPromotion( PRT_RECEIPT, TYPE_THROUGH );
    ItemHeader( TYPE_THROUGH );
    ItemMiscCheckTransAffect(&MiscFrom);

	// indicate that we are doing a transfer of the current guest check.
    TrnGetCurQualPtr()->fsCurStatus |= CURQUAL_TRANSFER;    // ItemMiscCheckTrans() transfer a guest check

    ItemMiscCheckTransfer(UifMisc, &MiscFrom, &MiscTo);
    ItemTrailer(TYPE_TRANSFER);

    while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
		USHORT  usRtnLead = GusConvertError(sStatus);
        UieErrorMsg(usRtnLead, UIE_WITHOUT);
    }
    
    /* initialize transaction information */
    TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    /* --- Delete Previous GCF, V3.3 --- */
    if (sGive != 0) {
		// This was an actual transfer in which the guest check was given to someone
		// else after giving it a new guest check number.  Delete the original copy.
        while ((sStatus = CliGusDelete(MiscFrom.usGuestNo)) != GCF_SUCCESS) {
			USHORT  usRtnLead = GusConvertError(sStatus);
            UieErrorMsg(usRtnLead, UIE_WITHOUT);
        }
    }
    /* --- Target Checker Sign-Out, V3.3 --- */
    if (MiscTo.ulID != 0) {
		CASIF  CasIf = {0};
        CasIf.ulCashierNo = MiscTo.ulID;
        ItemMiscTipsPOSignOut( &CasIf );
    }
        
    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTransGC2GC(ITEMMISC *pMiscTo)
*                                     
*     Input:    UifMisc - POINTER from UI
*    Output:    nothing 
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*               
** Description: Check Transfer GC to GC,                        V3.3
*===========================================================================
*/
SHORT   ItemMiscCheckTransGC2GC(ITEMMISC *pMiscTo)
{
    ULONG            ulGuestNo;
    TRANINFORMATION  *TrnInfo = TrnGetTranInformationPointer();
    TRANGCFQUAL      *WorkGCF = TrnGetGCFQualPtr();
	ULONG			 ulActualBytesRead;
    SHORT            sStatus, sDel = 0;
    USHORT           usStatus;
	//11-11-03 JHHJ	SR 201

    /* --- Get Destination Guest Check No --- */
    if ((sStatus = ItemTransGCFNo(&ulGuestNo)) != ITM_SUCCESS) {
        ItemTransACMulDispBack(); 
        return(sStatus);
    }
    
    /* --- Check CDV of Dest. Guest Check --- */
	{
		RflGcfNo  gcfCd = RflPerformGcfCd (ulGuestNo);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		pMiscTo->usGuestNo = gcfCd.usGcNumber;
		pMiscTo->uchCdv = gcfCd.uchGcCd;
	}
    /* --- Get Dest. Guest Check into Post Receipt File Area --- */
    sStatus = CliGusReadLockFH(pMiscTo->usGuestNo, TrnInfo->hsTranPostRecStorage,
        sizeof(USHORT) + sizeof(USHORT), TrnInfo->usTranConsStoFSize, GCF_REORDER, &ulActualBytesRead);

    /* --- Create New Guest Check --- */
    sDel = 0;
    if (sStatus == GCF_NOT_IN) {
        /* --- Decide Checker No. --- */
        if ((sStatus = ItemMiscCheckTransGC2GCChecker(&pMiscTo->ulID, pMiscTo->usGuestNo)) != ITM_SUCCESS) {
            return(sStatus);
        }
        /* --- Assign Destination Guest Check --- */
        if ((sStatus = CliGusManAssignNo(pMiscTo->usGuestNo)) != GCF_SUCCESS) {
			CASIF   CasIf = {0};          /* sign-out  Start Add TAR175812 Jun-27-2001 */
            CasIf.ulCashierNo = pMiscTo->ulID;
            CasIf.usUniqueAddress = (USHORT) CliReadUAddr(); /* get unique address */
            ItemCasSignInCancel(&CasIf);        /* sign-out  End Add TAR175812 Jun-27-2001 */
            usStatus = GusConvertError(sStatus);
            return((SHORT)usStatus);
        }
        sDel = 1;
    } else if (sStatus  < 0) {
        return((SHORT)GusConvertError(sStatus));
    } else if (sStatus == 0) {
        while ((sStatus = CliGusResetReadFlag(pMiscTo->usGuestNo, GCF_NO_APPEND)) != TRN_SUCCESS) {
            usStatus = GusConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
        return(LDT_ILLEGLNO_ADR);
    }

    /* --- Add --- */
    if (sDel == 0) {
		union {
			TRANGCFQUAL    gcfQual;
			TRANITEMIZERS  gcfItemizers;
		} uchBuffer;

		memset(&uchBuffer, 0, sizeof(uchBuffer));
        TrnReadFile_MemoryForce(sizeof(USHORT) + sizeof(USHORT),
            &(uchBuffer.gcfQual), sizeof(TRANGCFQUAL), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);

        /* --- Check Mismatch between Destination GC and Target GC --- */
        if ((sStatus = ItemMiscCheckTransGC2GCCheck(&(uchBuffer.gcfQual))) != ITM_SUCCESS) {
            while ((sStatus = CliGusResetReadFlag(pMiscTo->usGuestNo, GCF_NO_APPEND)) != GCF_SUCCESS) {
                usStatus = GusConvertError(sStatus);
                UieErrorMsg(usStatus, UIE_WITHOUT);
            }
            return(LDT_ILLEGLNO_ADR);
        }
        /* --- Add GCF Qualifier --- */
        TrnSplAddGC2GCGCFQual(&(uchBuffer.gcfQual));
        pMiscTo->ulID = WorkGCF->ulCashierID;

        /* --- Add Transaction Itemizers --- */
        TrnReadFile_MemoryForce(sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL), &(uchBuffer.gcfItemizers), sizeof(TRANITEMIZERS), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
        TrnSplAddGC2GCItemizers(&(uchBuffer.gcfItemizers));

        if (RflGetSystemType () == FLEX_POST_CHK) {
            /* --- Add Item in Consoli Storage --- */
            if (TrnSplAddGC2GC() != TRN_SUCCESS) {
                while ((sStatus = CliGusResetReadFlag(pMiscTo->usGuestNo, GCF_NO_APPEND)) != GCF_SUCCESS) {
                    usStatus = GusConvertError(sStatus);
                    UieErrorMsg(usStatus, UIE_WITHOUT);
                }
                return(LDT_ILLEGLNO_ADR);
            }
        }
    }

    /* --- Change to New Cashier No. --- */
    WorkGCF->ulCashierID = pMiscTo->ulID;
    WorkGCF->usGuestNo   = pMiscTo->usGuestNo;
    WorkGCF->uchCdv      = pMiscTo->uchCdv;
    pMiscTo->lAmount     = TranItemizersPtr->lMI;

    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTransGC2GCChecker(USHORT *pusCasNo)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, UIF_CANCEL, LDT_NOUNMT_ADR
*
** Description: Get Checker No
*===========================================================================
*/
SHORT   ItemMiscCheckTransGC2GCChecker(ULONG *pulCasNo, USHORT usGuestNo)
{
	CASIF           CasIf = {0};
    SHORT           sStatus;

    /* cashier id entry by sign-in key, msr, server lock */
    if ((sStatus = ItemMiscTipsPOWaiter(&CasIf)) != ITM_SUCCESS) {
        ItemTransDispGCFNo(TRN_GCNO_ADR);
        return(sStatus); 
    }

    if (!(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM)) { /* not use team */
        if ((CasIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM) && (CasIf.uchTeamNo)) {
			/* not using team however target is a team menber and not head operator */
            ItemCasSignInCancel(&CasIf);    /* sign-out */
            return(LDT_PROHBT_ADR);
        }
    } else {
        if ((CasIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM) == 0) {
			/* using team however target is not a team menber */
            ItemCasSignInCancel(&CasIf);    /* sign-out */
            return(LDT_PROHBT_ADR);
        } else {
            if ((CasIf.uchTeamNo) && (CasIf.uchTeamNo != TranModeQualPtr->uchCasTeamNo)) {
				/* using team however target is not in this team and target is not head operator */
                ItemCasSignInCancel(&CasIf);    /* sign-out */
                return(LDT_PROHBT_ADR);
            }
        }
    }

    /* target casher's guest check no range */
    if (((CasIf.usGstCheckStartNo)&&(usGuestNo < CasIf.usGstCheckStartNo)) ||
        ((CasIf.usGstCheckEndNo)&&(usGuestNo > CasIf.usGstCheckEndNo)))
	{
        ItemCasSignInCancel(&CasIf);    /* sign-out */
        return(LDT_PROHBT_ADR);
    }

    *pulCasNo = CasIf.ulCashierNo;
    return(ITM_SUCCESS);

/*
    REGDISPMSG      Disp;
    SHORT           sStatus;
    PARALEADTHRU    ParaLead;
    TRANMODEQUAL    WorkMode;
    UIFDIADATA      Dia;  
    CASIF           CasIf, OutCas;

    *pusCasNo = 0;
    memset(&OutCas, 0, sizeof(CASIF));
    memset(&CasIf, 0, sizeof(CASIF));

    ParaLead.uchMajorClass =  CLASS_PARALEADTHRU;
    ParaLead.uchAddress = LDT_ID_ADR;
    CliParaRead(&ParaLead);

    memset(&Disp, 0, sizeof(REGDISPMSG));
    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CASHIERNO;
    Disp.fbSaveControl = 4;
    memcpy(Disp.aszMnemonic, ParaLead.aszMnemonics, NUM_LEADTHRU);
    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL;

    if ((sStatus = UifDiaCashier(&Dia, UIFREG_ECHO)) != UIF_SUCCESS) {
        ItemTransDispDef();
        return(UIF_CANCEL);
    }
    TrnGetModeQual(&WorkMode);
    if (Dia.auchFsc[0] != FSC_SIGN_IN ||
        Dia.ulData > 999UL ||
        WorkMode.usCashierID == (USHORT)Dia.ulData) {
        return(LDT_SEQERR_ADR);
    }
    CasIf.usCashierNo = (USHORT)Dia.ulData;
    CasIf.usUniqueAddress = (USHORT)CliReadUAddr();
    CasIf.fbCashierOption |= CAS_MISC_SIGN_IN;    / as surrogate sign-in /
    if ((sStatus = CliCasSignIn(&CasIf)) != CAS_PERFORM) {
        sStatus = CasConvertError(sStatus );
        return(sStatus);
    }
    if (CasIf.fbCashierStatus[0] & CAS_TRAINING_CASHIER) {
        if (!(WorkMode.fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
            ItemCasSignInCancel(&CasIf);    / sign-out /
            return(LDT_PROHBT_ADR);
        }
    } else { 
        if (WorkMode.fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
            ItemCasSignInCancel(&CasIf);    / sign-out /
            return(LDT_PROHBT_ADR);
        }
    }

    *pusCasNo = (USHORT)Dia.ulData;

    return(ITM_SUCCESS);
*/
}

/*
*==========================================================================
**   Synopsis:  SHORT   ItemMiscCheckTransGC2GCCheck(TRANGCFQUAL *pWorkGCF)
*   Input:      
*   Output:     none 
*   InOut:      none
*
*   Return:     
*   Description:    Check Check Transfer
*==========================================================================
*/
SHORT   ItemMiscCheckTransGC2GCCheck(TRANGCFQUAL *pWorkGCF)
{
    /* --- Allow Check Trnsfer To --- */
    if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_TRANSFER_TO) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier record status CAS_NOT_TRANSFER_TO.");
        return(LDT_NOUNMT_ADR);
    }

    /* --- Check Team No --- */
    if (!(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM)) {
        if (TranModeQualPtr->ulCashierID != pWorkGCF->ulCashierID) {
            return(LDT_NOUNMT_ADR);
        }
    } else {
        /* --- Not Head Cashier --- */
        if (TranModeQualPtr->uchCasTeamNo != 0) {
            if (TranModeQualPtr->uchCasTeamNo != pWorkGCF->uchTeamNo) {
                return(LDT_NOUNMT_ADR);
            }
        }
    }

    /* --- Check Mismatch between Destination GC and Target GC --- */
    if (pWorkGCF->fsGCFStatus & GCFQUAL_PVOID) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) == 0) {
            return(LDT_PROHBT_ADR);
        }
    } else {
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) {
            return(LDT_PROHBT_ADR);
        }
    }
    if (pWorkGCF->fsGCFStatus & GCFQUAL_TRETURN) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) == 0) {
            return(LDT_PROHBT_ADR);
        }
    } else {
        if (TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) {
            return(LDT_PROHBT_ADR);
        }
    }
    if (!(pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING)) {
        if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
            return(LDT_PROHBT_ADR);
        }
    } else {
        if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
            return(LDT_PROHBT_ADR);
        }
    }    

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCheckTROut(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS
*               
** Description: This function execute "transfer check out", FSC_GUEST_CHK_TRN.
*===========================================================================
fhfh*/

SHORT   ItemMiscCheckTROut(VOID)
{                   
    ITEMCOMMONLOCAL *pItemCommonL = ItemCommonGetLocalPointer();   /* correction 8/24'93   */
    TRANCURQUAL     *pTranCurQualID = TrnGetCurQualPtr();
    ITEMTRANSCLOSE  ItemTransClose = {0};
	REGDISPMSG      RegDispMsgD = {0}, RegDispBack = {0};
    SHORT           sStatus = ITM_SUCCESS;

    /* check if tax modifier key or food stamp modifier was used ? */
    if ( ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) { /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* display "waiter-name", mnemonic */
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
 
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;                     /* set default adjective, 2172 */
        }
    }
    pItemCommonL->fbCommonStatus &= ~COMMON_SPVINT;       /* reset supervisor intervention execute flag */

    RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
    RegDispMsgD.uchMinorClass = CLASS_UICHECKTRANSFEROUT;
    _tcsncpy (RegDispMsgD.aszMnemonic, TranModeQualPtr->aszCashierMnem, NUM_OPERATOR );
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    ItemTendDispIn(&RegDispBack);

    flDispRegKeepControl &= ~ TRANSFR_CNTRL; /* reset descriptor(keep) */
    flDispRegDescrControl &= ~ TRANSFR_CNTRL; /* reset descriptor */
    DispWriteSpecial( &RegDispMsgD, &RegDispBack);

    /* set majour/minor class for transaction data */
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSCHECKTRANSFER;

    /* 9/28/93 get current qualifier data */
    pTranCurQualID->uchPrintStorage = PRT_NOSTORAGE;
    pTranCurQualID->uchKitchenStorage = PRT_NOSTORAGE;   /* not send kitchen */

    TrnClose( &ItemTransClose );

    /* check consecutive no. */
    if ( ItemCommonGetStatus( COMMON_CONSNO ) != 0 ) {
        sStatus = ITM_SUCCESS;
    } else {
        /* initialize transaction informaation */
        TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
        ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */
        sStatus = ItemFinalize();
    }
	// display a list of outstanding guest checks for this operator who finalizing a check transfer.
	// we can provide different formats of the data such as display by guest check number
	// or display by table number. the legacy is to display by guest check number.
	MldScrollClear(MLD_SCROLL_2);   /* clear seat no */
//	ItemOtherDispGuestNo(TranModeQualPtr->ulCashierID);
    ItemOtherDispTableNo(TranModeQualPtr->ulCashierID);

	return sStatus;
}


/*
*===========================================================================
** Synopsis:    VOID    ItemMiscCancelDispTotal(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Display Subtotal on LCD & Erase 2x20 Display.
*===========================================================================
*/
VOID    ItemMiscCancelDispTotal(VOID)
{
	REGDISPMSG      Disp = {0};

    /*----- Display MI on LCD -----*/
    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

    /*----- Erase 2x20 Display -----*/
    Disp.uchMajorClass = CLASS_UIFREGMISC;
    Disp.uchMinorClass = CLASS_UICANCEL;
    DispWrite(&Disp);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCancelCheck(UIFREGMISC  *ItemMisc)
*                                    
*               
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: This function checks whether to be able to execute "cancel" or not.
*===========================================================================
fhfh*/

SHORT   ItemMiscCancelCheck(VOID)
{
    /* check operatable in Reg mode */
    if ( CliParaMDCCheck( MDC_CANCEL_ADR, EVEN_MDC_BIT1) != 0 ) {
        return(LDT_PROHBT_ADR);
    }

    /* check if tax modifier key or food stamp modifier was used ? */
    if ( ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) { /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check multi check */
    if ( ItemTransLocalPtr->uchCheckCounter != 0 ) {
        return(LDT_SEQERR_ADR); 
    }

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCancelSPV(VOID)
*                                    
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: This function checks "supervisor intervention.
*===========================================================================
fhfh*/

SHORT   ItemMiscCancelSPV(VOID)
{
    /* check SPV intervention */
    if ( CliParaMDCCheck( MDC_CANCEL_ADR, EVEN_MDC_BIT0) == 0 ) {
        return(ITM_SUCCESS);
    }
    /* check SPV INTERVENTION canceled or not */
    return(ItemSPVInt(LDT_SUPINTR_ADR));
}

/*fhfh
*===========================================================================
* * Synopsis:    VOID   ItemMiscCancelItem(UIFREGMISC  *ItemMisc)
*                                    
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes making data and transaction i/f .
*===========================================================================
fhfh*/

VOID    ItemMiscCancelItem(UIFREGMISC  *ItemMisc)
{
    TRANCURQUAL     *pTranCurQual = TrnGetCurQualPtr();
    ITEMMISC        TrnItemMisc = {0};
	REGDISPMSG      RegDispMsgD = {0}, RegDispBack = {0};
    SHORT           sStat = 0;      /* work status */

    /* set class code */
    TrnItemMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnItemMisc.uchMinorClass = CLASS_CANCEL;
    TrnItemMisc.lAmount = ItemMisc->lAmount;
    _tcsncpy( TrnItemMisc.aszNumber, ItemMisc->aszNumber, NUM_NUMBER );

    /* set print status(journal and slip) */
    if (  (pTranCurQual->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        TrnItemMisc.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);
    } else {
        /* in case of buffering */
        TrnItemMisc.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL);//US Customs
        pTranCurQual->uchPrintStorage = PRT_ITEMSTORAGE;
        pTranCurQual->fbCompPrint = PRT_JOURNAL;
		//US Customs
		if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT3)) {
			pTranCurQual->fbNoPrint =  ( PRT_SLIP | PRT_VALIDATION );
		} else {
			pTranCurQual->fbNoPrint =  ( PRT_RECEIPT | PRT_SLIP | PRT_VALIDATION );
		}
    }

    /* display "cancel", mnemonic */
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0) != 0 ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL; /* reset descriptor(keep) */
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    } 

    RegDispMsgD.uchMajorClass = CLASS_UIFREGMISC;
    RegDispMsgD.uchMinorClass = CLASS_UICANCEL;
    RegDispMsgD.lAmount = TrnItemMisc.lAmount;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_CANCEL_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    ItemTendDispIn(&RegDispBack);
    flDispRegDescrControl &= ~ TVOID_CNTRL; /* reset descriptor */  
    flDispRegKeepControl &= ~( TVOID_CNTRL | NEWCHK_CNTRL | PBAL_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL );

    /* check release on finalize or not (menu page)  correction 10/07*/
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;                /* get default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                         /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;   /* set default adjective, 2172 */
        }
    }
 
    DispWriteSpecial( &RegDispMsgD, &RegDispBack);

    /* --- display transaction cancel amount on lcd --- */
    MldDispSubTotal(MLD_TOTAL_1, TrnItemMisc.lAmount); /* R3.0 */

    /*--- VERIFY NO WAITER STILL SIGN IN ---*/
    if ( TranModeQualPtr->ulWaiterID != 0 ) {  /* waiter still sign in */
        sStat = CliParaMDCCheck(MDC_WTIN_ADR, EVEN_MDC_BIT2);
    } else { 
        sStat = CliParaMDCCheck(MDC_WTIN_ADR, ODD_MDC_BIT2);
    }

    if ( sStat ) {                          /* release on finalize ? */
        flDispRegDescrControl &= ~ TRAIN_CNTRL; /* reset descriptor */
        flDispRegKeepControl &= ~ TRAIN_CNTRL; /* reset descriptor */
    }

	if ((ItemSalesLocalPtr->fbSalesStatus & SALES_PREAUTH_BATCH) == 0) {
		TrnItem( &TrnItemMisc );
		/* send to enhanced kds, 2172 */
		ItemSendKds(&TrnItemMisc, 0);
	}
    
    /*--- Display Cancel,  R3.0 ---*/
    MldScrollWrite(&TrnItemMisc, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(&TrnItemMisc);
}

/*fhfh
*===========================================================================
* * Synopsis:   VOID    ItemMiscCancelAffect(UIFREGMISC  *ItemMisc)
*                                    
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes affection.
*===========================================================================
fhfh*/

VOID    ItemMiscCancelAffect(UIFREGMISC  *ItemMisc)
{
	ITEMAFFECTION   ItemAffectionL = {0};

    /* set class code */
    ItemAffectionL.uchMajorClass = CLASS_ITEMAFFECTION;
    ItemAffectionL.uchMinorClass = CLASS_CANCELTOTAL;
    ItemAffectionL.lCancel = ItemMisc->lAmount;    /* set cancel total */
    TrnItem( &ItemAffectionL );
}

/*fhfh
*===========================================================================
* * Synopsis:   VOID    ItemMiscCancelClose( VOID )
*                                    
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes making data and transaction i/f.
*===========================================================================
fhfh*/

VOID    ItemMiscCancelClose(VOID)
{
	ITEMTRANSCLOSE  ItemTransClose = {0};

    /*
	 * the function TrnStoGetConsToPostR() must be used to transfer the last transaction
	 * to the Post Receipt storage area in order for Post Receipt or Print on Demand to work
	 * by using TrnStoPutPostRToCons() to retrieve the saved data in the Post Receipt storage area.
	**/
    ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
    ItemMiscSetStatus(MISC_POSTRECPT | MISC_PRINTDEMAND);  // for No Sale allow reprint
    TrnStoGetConsToPostR();

    /* set class code */
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSCANCEL;
    TrnClose( &ItemTransClose );
    
    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);
}

/*fhfh
*===========================================================================
* * Synopsis:   VOID    ItemMiscCancelGCF(VOID)
*                                    
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function execute to cancel GCF.
*===========================================================================
fhfh*/

VOID    ItemMiscCancelGCF( VOID)
{
    TRANGCFQUAL     *TranGCFQualL = TrnGetGCFQualPtr();
    TRANINFORMATION *pTrnInfo = TrnGetTranInformationPointer();
	ULONG			ulActualBytesRead;//RPH SR# 201
    SHORT           sErrStatus, sType, sSize, sQueue;
    USHORT          usRtnLead;

    /* check "new check" or other */
    /* FDT,    R3.0 */
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
       if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
            while ((sErrStatus = CliGusDelete(TranGCFQualL->usGuestNo)) != GCF_SUCCESS) {
                usRtnLead = GusConvertError( sErrStatus );
                UieErrorMsg( usRtnLead, UIE_WITHOUT );
            }
        }
        FDTCancel();
    }
    if ( ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) || (TranGCFQualL->usGuestNo == 0 ) ){
        return;
    } else {
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK ) {
            while ( ( sErrStatus = CliGusDelete( TranGCFQualL->usGuestNo) ) !=  GCF_SUCCESS ) {
                usRtnLead = GusConvertError( sErrStatus );
                UieErrorMsg( usRtnLead, UIE_WITHOUT );
            }
        } else {
            /* CURQUAL_REORDER, CURQUAL_ADDCHECK */
            if ( (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER 
                && ( TranGCFQualL->uchSlipLine != 0 || TranGCFQualL->uchPageNo != 0 ) )
			{  
				ULONG ulSlipLineOffset = (ULONG)&((TRANGCFQUAL *)NULL)->uchSlipLine;

				// Update the current slip printer line number and page number of the guest check by
				// reading the data from the guest check file, updating the two data items for the slip printer
				// and then writing the data back to the guest check file.
				//RPH SR#201
				while( (sSize = CliGusIndReadFH( TranGCFQualL->usGuestNo, pTrnInfo->hsTranConsStorage, sizeof(USHORT) + sizeof(USHORT),
                                                   pTrnInfo->usTranConsStoFSize, &ulActualBytesRead) ) < 0){

                    usRtnLead = GusConvertError( sSize );
                    UieErrorMsg( usRtnLead, UIE_WITHOUT );
                }

                /* save slip line and page */
                TrnWriteFile(ulSlipLineOffset, (SHORT *)&TranGCFQualL->uchSlipLine,
							sizeof(TranGCFQualL->uchSlipLine) + sizeof(TranGCFQualL->uchPageNo),
							pTrnInfo->hsTranConsStorage);

                sType = GCF_COUNTER_TYPE;                               
                while ( ( sErrStatus = CliGusStoreFileFH( sType, TranGCFQualL->usGuestNo, pTrnInfo->hsTranConsStorage,
                                                          sizeof(USHORT) + sizeof(USHORT),
                                                          ulActualBytesRead ) ) != GCF_SUCCESS ) {

                    usRtnLead = GusConvertError( sErrStatus );
                    UieErrorMsg( usRtnLead, UIE_WITHOUT );
                }
            } else {
				FDTPARA     WorkFDT = { 0 };

				FDTParameter(&WorkFDT);
                if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL) {
                    if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1    /* Order Termninal (Queue#1) */
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
                        || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1) {
                        sQueue = GCF_APPEND_QUEUE1;
                    } else if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2 /* Order Termninal (Queue#2) */
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2
                        || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2) {
                        sQueue = GCF_APPEND_QUEUE2;
                    } else {
                        sQueue = GCF_NO_APPEND;
                    }
                } else {
                    sQueue = GCF_NO_APPEND;
                }
                while ( ( sErrStatus = CliGusResetReadFlag( TranGCFQualL->usGuestNo,
                    sQueue) ) !=  GCF_SUCCESS ) {
                    usRtnLead = GusConvertError( sErrStatus );
                    UieErrorMsg( usRtnLead, UIE_WITHOUT );
                }

                if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL) {  
                    pTrnInfo->TranGCFQual.usGuestNo = TranGCFQualL->usGuestNo;
                    if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
                        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE) {
                        FDTSaveTrans(pTrnInfo->TranGCFQual.usGuestNo);

                    } else if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
                        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
                        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
                        FDTSaveTransStorePay(pTrnInfo->TranGCFQual.usGuestNo);
                    }
                }
            }
        }
    }
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscCancel(UIFREGMISC *ItemMisc) 
*                                     
*     Input:    ItemMisc - POINTER from UI
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR, UIF_CANCEL
*               
** Description: This function execute "cancel".
*===========================================================================
fhfh*/

SHORT   ItemMiscCancel(UIFREGMISC *ItemMisc)
{
    TRANITEMIZERS   *TranItemizersL = TrnGetTIPtr();
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    ITEMSALESLOCAL  *pItemSalesLocalL = ItemSalesGetLocalPointer(); 
    ITEMTRANSLOCAL  *pItemTransLocalL = ItemTransGetLocalPointer(); 
    TRANCURQUAL     *pTranCurQualL = TrnGetCurQualPtr();
    DCURRENCY       lMiscWk = 0L;
	ITEMPRINT       ItemPrintData = {0};
    USHORT  i;
    SHORT   sStatus;
    ULONG   fsCurStatus;
    USHORT  usRtnLead;
	
    if ( ( sStatus = ItemMiscCancelCheck() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    /*----- Split Check,   R3.1 -----*/
	ItemVoidGiftCardCancel();

    if ((pTranCurQualL->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {

        if (pTranCurQualL->uchSplit != 0) {
            if (pTranCurQualL->uchSeat != 0) {
                return(LDT_PROHBT_ADR);

            } else {
				TRANINFORMATION *WorkTran = TrnGetTranInformationPointer();
				MLDTRANSDATA    WorkMLD = {0};
                /*----- Return to LCD 1 Display -----*/
                MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);

#if 1
				/*----- Display process 4/19/96-----*/
				MldPutTransToScroll(MldMainSetMldDataSplit(&WorkMLD, TRN_TYPE_CONSSTORAGE));
#else
                WorkMLD.usVliSize = WorkTran->usTranConsStoVli;
                WorkMLD.sFileHandle = WorkTran->hsTranConsStorage;

                /*----- Display process 4/19/96-----*/
                WorkMLD.sIndexHandle = WorkTran->hsTranConsIndex;
                MldPutTransToScroll(&WorkMLD);
#endif

                /*----- Return to LCD 1 Display -----*/
                TrnSplCancel3Disp();

                /*----- Display SubTotal on LCD -----*/
                ItemMiscCancelDispTotal();
                return(UIF_CANCEL);
            }
        } else {
            for (i = 0; i < NUM_SEAT; i++) {
                if (WorkGCF->auchFinSeat[i] != 0) {
                    return(LDT_PROHBT_ADR);
                }
            }
        }
    }

	if ( ( sStatus = ItemMiscCancelSPV() ) != ITM_SUCCESS ) {
		return(sStatus);
	}

    pItemSalesLocalL->fbSalesStatus &= ~ SALES_BOTH_COMPLSRY;  /* reset pm and co.*/

    if ( pTranCurQualL->fsCurStatus & CURQUAL_TRAY                    /* tray total transaction */
        && ! ( pItemSalesLocalL->fbSalesStatus & SALES_ITEMIZE ) ) {  /* itemize state */

        ItemPromotion( PRT_RECEIPT, TYPE_TRAY );
        ItemHeader( TYPE_STORAGE ) ;
    }

	ItemPreviousItem( ItemMisc, 0 );

    /* cancel auto charge tips by check total, V3.3 */
    ItemPrevCancelAutoTips();
	if ((pTranCurQualL->fsCurStatus & CURQUAL_TRETURN) != 0 && pTranCurQualL->usReturnsFrom != 0) {
		CliGusReturnsLockClear (pTranCurQualL->usReturnsFrom);
	}

    /* get MI */
	lMiscWk = 0L;
    for ( i = 0 ; i < 3 ; i++ ) {   
        lMiscWk += TranItemizersL->lService[i];
    }            

    ItemMisc->lAmount = (TranItemizersL->lMI - lMiscWk - pItemTransLocalL->lManualAmt);

    ItemMiscCancelItem(ItemMisc);

    pTranCurQualL->fsCurStatus |= CURQUAL_CANCEL;
	// Amtrak, AMT-2550 print the promotional message on canceled receipts.
//	pTranCurQualL->flPrintStatus |= CURQUAL_NOT_PRINT_PROM;

	if ((pItemSalesLocalL->fbSalesStatus & SALES_PREAUTH_BATCH) == 0) {
		ItemTrailer(TYPE_CANCEL);

		{
			ULONG           ulUserId = 0;
			TRANGCFQUAL    *pTranGCFQual = TrnGetGCFQualPtr();
			TCHAR           aszSapId[16];
			TCHAR           tcsAddOnString[512], *ptcsBuffer;
			int             nLength;

			if (TranModeQualPtr->ulCashierID != 0)
				ulUserId = TranModeQualPtr->ulCashierID;
			else if (TranModeQualPtr->ulWaiterID != 0)
				ulUserId = TranModeQualPtr->ulWaiterID;

			_itot (ulUserId, aszSapId, 10);

			ptcsBuffer = tcsAddOnString;

#if defined(DCURRENCY_LONGLONG)
			nLength = _stprintf(ptcsBuffer, _T("<gcnum>%d</gcnum>\r<lAmount>%lld</lAmount>\r"), pTranGCFQual->usGuestNo, ItemMisc->lAmount);
#else
			nLength = _stprintf(ptcsBuffer, _T("<gcnum>%d</gcnum>\r<lAmount>%ld</lAmount>\r"), pTranGCFQual->usGuestNo, ItemMisc->lAmount);
#endif
			if (nLength >= 0)
				ptcsBuffer += nLength;

			{
				TCHAR   tcsUniqueIdentifier[28];          // Unique identifer for returns as digits, binary coded decimal
				TCHAR   tcsUniqueIdentifierReturn[28];    // Unique identifer of original transaction for returns as digits, binary coded decimal

				UifConvertUniqueIdReturningString (tcsUniqueIdentifier, pTranGCFQual->uchUniqueIdentifier);
				UifConvertUniqueIdReturningString (tcsUniqueIdentifierReturn, pTranGCFQual->uchUniqueIdentifierReturn);

				nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifier>%s</uchUniqueIdentifier>\r<uchUniqueIdentifierReturn>%s</uchUniqueIdentifierReturn>\r"), tcsUniqueIdentifier, tcsUniqueIdentifierReturn);
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}

			ConnEngineSendCashierActionWithAddon (CONNENGINE_ACTIONTYPE_CANCELTRAN, aszSapId, tcsAddOnString);
		}
	}

    fsCurStatus = (pTranCurQualL->fsCurStatus & CURQUAL_WIC);                  /* Saratoga */

	//Orignial
	if ((pItemSalesLocalL->fbSalesStatus & SALES_PREAUTH_BATCH) == 0) {
		ItemMiscCancelClose();
	}

    /* initialize transaction informaation */
    TrnInitialize(TRANI_ITEM | TRANI_CONSOLI);

    ItemMiscCancelAffect(ItemMisc);

    /* data affection */
	if ((pItemSalesLocalL->fbSalesStatus & SALES_PREAUTH_BATCH) == 0) {
		while ( ( sStatus = TrnSendTotal() ) != TRN_SUCCESS ) {
			usRtnLead = GusConvertError( sStatus );
			UieErrorMsg( usRtnLead, UIE_WITHOUT );
		}
	}

    ItemMiscCancelGCF(); 

    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */

    return(ItemFinalize());
}

/*
*===========================================================================
** Synopsis:    SHORT ItemMiscETK(UIFREGMISC *UifRegMisc)
*
*       Input:  *UifRegMisc     : Pointer to Structure for UIFREGMISC  
*      Output:  nothing
*
**  Return:     UIF_SUCCESS     : Successful 
*                               : Other
*
** Description: Reg ETK Time In/Out Module
*===========================================================================
*/

SHORT ItemMiscETK(UIFREGMISC *pUifRegMisc)
{
    SHORT       sReturn;
    ETK_FIELD   ETKField;
    /* check as master */
//GSU SR 8 Time In/Out work by cwunn 12.10.2002
//Remove check if terminal is master, to allow time in from any terminal
//    if (CliCheckAsMaster() != STUB_SUCCESS) {           
//        return(LDT_PROHBT_ADR);                         
//    }

	// if the prohibit sign-out is indicated then do not allow the
	// operator to sign out of the terminal until something is done
	// to turn off the sign-out prohibit.
	if (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == 0) {
        return(LDT_PROHBT_ADR);
	}

    /* open item, cons. post rec. file */
    if (TrnICPOpenFile() != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    switch (pUifRegMisc->uchMinorClass) {
    case CLASS_UIETKIN:
        sReturn = ItemMiscETKIn(pUifRegMisc, &ETKField);
        break;
                                    
    default:                        /* case CLASS_UIETKOUT: */
        sReturn = ItemMiscETKOut(pUifRegMisc, &ETKField);
        break;
    }

    /* set data to print module */
    if ((sReturn == UIF_SUCCESS)) {
        ItemMiscETKPrint(pUifRegMisc, &ETKField);
        sReturn = UIF_SUCCESS;
    }

    /* finalize */
	// ensure that the WorkMode.auchWaiStatus[1] is cleared
	// so that we wont automatically sign-out on finalize.
    TrnInitialize(TRANI_CURQUAL | TRANI_ITEM | TRANI_MODEQUAL); 
    ItemFinalize();  // RJC should this be ItemInit(); instead?

	//decrease requested by DANP
	PifSleep(1000);
	//PifSleep(3000);

    ItemCasOutDisplay();            /* display mode in message */

    /* close item, cons. post rec. file */
    TrnICPCloseFile();
    return(sReturn);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemMiscETKSignIn(UIFREGMISC *UifRegMisc)
*
*       Input:  *UifRegMisc     : Pointer to Structure for UIFREGMISC  
*      Output:  nothing
*
**  Return:     UIF_SUCCESS     : Successful 
*                               : Other
*
** Description: ETK Time In/Out Module when user is signed in
*===========================================================================
*/

SHORT ItemMiscETKSignIn(UIFREGMISC *pUifRegMisc)
{
    SHORT       sReturn;
    ETK_FIELD   ETKField;
    /* check as master */
//GSU SR 8 Time In/Out work by cwunn 12.10.2002
//Remove check if terminal is master, to allow time in from any terminal
//    if (CliCheckAsMaster() != STUB_SUCCESS) {           
//        return(LDT_PROHBT_ADR);                         
//    }

	// if the prohibit sign-out is indicated then do not allow the
	// operator to sign out of the terminal until something is done
	// to turn off the sign-out prohibit.
	if (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == 0) {
        return(LDT_PROHBT_ADR);
	}

    /* open item, cons. post rec. file */
    if (TrnICPOpenFile() != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    switch (pUifRegMisc->uchMinorClass) {
    case CLASS_UIETKIN:
		{
			UIFDIAEMPLOYEE  UifDiaEmployee;
			SHORT           sTimeInStatus = SUCCESS;

			pUifRegMisc->sTimeInStatus = 0;
			if (pUifRegMisc->ulEmployeeNo == 0) {
				sReturn = UifRegEtkGetCheckEmployeeData (pUifRegMisc->ulEmployeeNo, &UifDiaEmployee, &sTimeInStatus);

				if (sReturn != 0 && sReturn != ETK_UNKNOWN_EMPLOYEE)
					return sReturn;

				pUifRegMisc->ulEmployeeNo = UifDiaEmployee.ulSwipeEmployeeId;
				pUifRegMisc->uchJobCode = UifDiaEmployee.usSwipeEmployeeJobCode;
				pUifRegMisc->sTimeInStatus = sTimeInStatus;
				pUifRegMisc->ulStatusFlags = UifDiaEmployee.ulStatusFlags;
			}

			sReturn = ItemMiscETKIn(pUifRegMisc, &ETKField);
		}
        break;
                                    
    default:                        /* case CLASS_UIETKOUT: */
		if (pUifRegMisc->ulEmployeeNo == 0) {
			UIFDIAEMPLOYEE  UifDiaEmployee;

			if ((sReturn = UifEmployeNumherForSignTime (&UifDiaEmployee)) != SUCCESS) {
				return(sReturn);
			}

			pUifRegMisc->ulEmployeeNo = UifDiaEmployee.ulSwipeEmployeeId;
			pUifRegMisc->uchJobCode = 0;
			pUifRegMisc->sTimeInStatus = 0;
			pUifRegMisc->ulStatusFlags = UifDiaEmployee.ulStatusFlags;
		}

        sReturn = ItemMiscETKOut(pUifRegMisc, &ETKField);
        break;
    }

	/* set data to print module */
    if ((sReturn == UIF_SUCCESS)) {
        ItemMiscETKPrint(pUifRegMisc, &ETKField);
        sReturn = UIF_SUCCESS;
    }

    /* finalize */
	// ensure that the WorkMode.auchWaiStatus[1] is cleared
	// so that we wont automatically sign-out on finalize.
    TrnInitialize(TRANI_CURQUAL | TRANI_ITEM); 
    ItemFinalize();  // RJC should this be ItemInit(); instead?

	//decrease requested by DANP
	PifSleep(1000);
	//PifSleep(3000);

    return(sReturn);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemMiscETKIn(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField)
*
*       Input:  *UifRegMisc     : Pointer to Structure for UIFREGMISC  
*    InOutput:  *ETKField       : Pointer to Structure for ETK_FIELD  
*      Output:  nothing
*
**  Return:     UIF_SUCCESS     : Successful 
**              UIF_CANCEL      : Canceled 
*                               : Other
*
** Description: Reg ETK Time Input Module             R3.1
*===========================================================================
*/

SHORT ItemMiscETKIn(UIFREGMISC *pUifRegMisc, ETK_FIELD *ETKField)
{
    SHORT           sReturn;
    SHORT           sTimeIOStat;
    USHORT          usMaxFieldNo;
    ETK_JOB         ETKJob;

    /* input data already checked by UI */
    /* check employee number and job code */
    if ((sReturn = CliEtkStatRead(pUifRegMisc->ulEmployeeNo, &sTimeIOStat, &usMaxFieldNo, &ETKJob)) != ETK_SUCCESS) {
        return(EtkConvertError(sReturn));
    }

	// The following is to work around a field issue seen on some Amtrak trains in which
	// the initialization sequence which beings with the Start Log In screen in which
	// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
	// the Inventory app for other train trip startup activities does not complete properly.
	// The result is that the LSA is unable to complete the Start Log In screen task and
	// is stuck with it partially complete. A control string is used to automate this task
	// so as part of the sanity check as to whether to allow this workaround, check that a
	// control string is running.
	//
	// This change checks if the Employee is doing Time-In when they have already done a Time-In
	// without an intervening Time-Out. If we are Amtrak and are running a control string then
	// just ignore this error.
	//      Richard Chambers, Aug-17-2018, for Amtrak only
	if (sTimeIOStat == ETK_NOT_TIME_OUT && ! ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK) && ! UieMacroRunning()) {
        return(EtkConvertError(sTimeIOStat));
    }
    if (usMaxFieldNo >= ETK_MAX_FIELD) {
        return(LDT_FLFUL_ADR);
    }

    /* check job code */
    if (pUifRegMisc->uchJobCode == 0 && (ETKJob.uchJobCode1 || ETKJob.uchJobCode2 || ETKJob.uchJobCode3)) {
        for (;;) {
			UIFDIADATA      UifDiaRtn = {0};
			REGDISPMSG      RegDispMsgD = {0};

            /* set display data */
            RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
            RegDispMsgD.uchMinorClass = CLASS_REGDISP_ETK;
            RflGetLead (RegDispMsgD.aszMnemonic, LDT_DATA_ADR);
            RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; 
            flDispRegDescrControl |= COMPLSRY_CNTRL; 
            flDispRegKeepControl |= COMPLSRY_CNTRL; 

            /* fixed evev if current page is not 1, page 1 is displayed */
            /* uchDispCurrMenuPage = 1;                / set menu no. to 1 */
            DispWrite(&RegDispMsgD);
            flDispRegDescrControl &= ~ COMPLSRY_CNTRL; 
            flDispRegKeepControl &= ~ COMPLSRY_CNTRL;

            /* execute job number entry */
            if ((sReturn = UifDiaETK(&UifDiaRtn, UIFREG_ECHO)) == UIF_SUCCESS) {
                /* check job code */
                if ((( UCHAR)UifDiaRtn.ulData != ETKJob.uchJobCode1) &&
                    (( UCHAR)UifDiaRtn.ulData != ETKJob.uchJobCode2) &&
                    (( UCHAR)UifDiaRtn.ulData != ETKJob.uchJobCode3)) {
                    UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);       /* wait clear key */   
                } else {                                                       
                    pUifRegMisc->uchJobCode = ( UCHAR)UifDiaRtn.ulData;
                    break;
                }
            } else if (sReturn == UIFREG_ABORT) {
                return(UIF_CANCEL);
            }
        }
    }
            
    /* execute ETK time in */
    sReturn = CliEtkTimeIn(pUifRegMisc->ulEmployeeNo, pUifRegMisc->uchJobCode, ETKField);

	//failure of ETK time in
   // if ((sReturn == ETK_FIELD_OVER) || (sReturn == ETK_LOCK)) {
	if ( sReturn != ETK_SUCCESS) { //SR495
        return(EtkConvertError(sReturn));
    }

	{            /* ok.  time in worked so send notification out Connection Engine */
		TCHAR aszSapId[16], aszStatus[16];
		_itot (pUifRegMisc->ulEmployeeNo, aszSapId, 10);
		_tcscpy (aszStatus, _T("ok"));
		if (pUifRegMisc->sTimeInStatus == ETK_NOT_IN_FILE)
			_tcscpy (aszStatus, _T("ok, new"));
		else if (pUifRegMisc->sTimeInStatus == ETK_FILE_NOT_FOUND)
			_tcscpy (aszStatus, _T("ok, unknown"));

		ConnEngineSendEmployeeDataChange (CONNENGINE_ACTIONTYPE_TIMEIN, aszSapId, aszStatus, pUifRegMisc->ulStatusFlags);
		{
			char  xBuff[128];

			sprintf ( xBuff, "==ACTION: Employee Time-in %8.8u", pUifRegMisc->ulEmployeeNo);
			NHPOS_NONASSERT_NOTE("==ACTION", xBuff);
		}
	}

	//GSU SR 115 by cwunn 2/4/2003
	//Added mnemonic support to visual display confirmation. Uses TRN_ETKOUT_ADR mnemonic
	{
		REGDISPMSG      RegDispMsgD = {0};

		RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
		RegDispMsgD.uchMinorClass = CLASS_UILOWMNEMO;
		RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_ETKIN_ADR);
		DispWrite(&RegDispMsgD);
	}

    return(UIF_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemMiscETKOut(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField)
*
*       Input:  *UifRegMisc     : Pointer to Structure for UIFREGMISC  
*    InOutput:  *ETKField       : Pointer to Structure for ETK_FIELD  
*      Output:  nothing
*
**  Return:     UIF_SUCCESS     : Successful 
*                               : Other
*
** Description: Reg ETK Time Input Module                   R3.1
*===========================================================================
*/

SHORT ItemMiscETKOut(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField)
{
    SHORT           sReturn;
	REGDISPMSG      RegDispMsgD = {0};

    /* input data already checked by UI */
    /* execute ETK time out */
    sReturn = CliEtkTimeOut(UifRegMisc->ulEmployeeNo, ETKField);

    if ((sReturn != ETK_SUCCESS)) {
        return(EtkConvertError(sReturn));
    }

	{            /* ok.  time in worked so send notification out Connection Engine */
		TCHAR aszSapId[16], aszStatus[16];
		_itot (UifRegMisc->ulEmployeeNo, aszSapId, 10);
		_tcscpy (aszStatus, _T("ok"));

		ConnEngineSendEmployeeDataChange (CONNENGINE_ACTIONTYPE_TIMEOUT, aszSapId, aszStatus, 0);
		{
			char  xBuff[128];

			sprintf ( xBuff, "==ACTION: Employee Time-out %8.8u", UifRegMisc->ulEmployeeNo);
			NHPOS_NONASSERT_NOTE("==ACTION", xBuff);
		}
	}

	//GSU SR 115 by cwunn 2/4/2003
	//Added mnemonic support to visual display confirmation. Uses TRN_ETKOUT_ADR mnemonic
	RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
	RegDispMsgD.uchMinorClass = CLASS_UILOWMNEMO;
	RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_ETKOUT_ADR);
	DispWrite(&RegDispMsgD);

    return(UIF_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID ItemMiscETKPrint(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField)
*
*       Input:  *UifRegMisc     : Pointer to Structure for UIFREGMISC  
*               *ETKField       : Pointer to Structure for ETK_FIELD  
*      Output:  nothing
*
**  Return:     nothing 
*
** Description: Print ETK Time In/Out Module
*===========================================================================
*/

VOID ItemMiscETKPrint(UIFREGMISC    *UifRegMisc,
                      ETK_FIELD     *ETKField)
{
	TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

    /* initialize */
    memset(pCurQualRcvBuff, 0, sizeof(TRANCURQUAL));

    /* consecutive no. and current qualifier count up on */
    ItemCountCons();

    /* set option to current qualifier(current print status) */
    ItemCurPrintStatus();

	pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */

	/* start print service */
    ItemPrintStart(TYPE_POSTCHECK);     /* store only item consoli. buffer */

	{
		ITEMMISC        ItemMisc = {0};
		ETK_JOB         WorkEtk = {0};

		/* set class */
		ItemMisc.uchMajorClass = CLASS_ITEMMISC;
		ItemMisc.fbStorageStatus |= NOT_CONSOLI_STORAGE;
		if (UifRegMisc->uchMinorClass == CLASS_UIETKIN) {
			ItemMisc.uchMinorClass = CLASS_ETKIN;
		} else {
			if (ETKField->usTimeinTime == ETK_TIME_NOT_IN) {
				ItemMisc.uchMinorClass = CLASS_ETKOUT_WOETKIN;
			} else {
				ItemMisc.uchMinorClass = CLASS_ETKOUT;
			}
		}

		/* set ETK data */
		ItemMisc.usYear = ETKField->usYear;         
		ItemMisc.usMonth = ETKField->usMonth;        
		ItemMisc.usDay = ETKField->usDay;          
		ItemMisc.ulEmployeeNo = UifRegMisc->ulEmployeeNo;   
		ItemMisc.uchJobCode = ETKField->uchJobCode;     
		ItemMisc.usTimeinTime = ETKField->usTimeinTime;   
		ItemMisc.usTimeinMinute = ETKField->usTimeinMinute; 
		ItemMisc.usTimeOutTime = ETKField->usTimeOutTime;  
		ItemMisc.usTimeOutMinute = ETKField->usTimeOutMinute;
		UifEmployeeSignTimeMethodString (UifRegMisc->ulStatusFlags, ItemMisc.aszNumber);

		/*----- R3.1 -----*/
		CliEtkIndJobRead(UifRegMisc->ulEmployeeNo, &WorkEtk, ItemMisc.aszMnemonic);

		/* check validation compulsory */
		if (CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT0)) {
			ItemMisc.fsPrintStatus = PRT_VALIDATION;        /* set validation */
			TrnThrough(&ItemMisc);
		}

		ItemMisc.fsPrintStatus = ( PRT_JOURNAL | PRT_SLIP );    /* set R/J portion */
		TrnItem(&ItemMisc);
	}

    /* print trailer (only receipt and journal) */
	{
		ITEMPRINT       ItemPrintData = {0};      

		ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;      
		ItemPrintData.uchMinorClass = CLASS_TRAILER3;       
		ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL);
	    
		ItemPrintData.ulStoreregNo = pCurQualRcvBuff->ulStoreregNo;
		ItemPrintData.fbStorageStatus |= NOT_CONSOLI_STORAGE;

		TrnItem(&ItemPrintData);
	}

    /* close transaction */
	{
		ITEMTRANSCLOSE  ItemTransClose = {0};

		pCurQualRcvBuff->uchPrintStorage = PRT_ITEMSTORAGE;
		if (  (pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
			// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
			// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
			// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
			pCurQualRcvBuff->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
		}
		pCurQualRcvBuff->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */

		ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
		ItemTransClose.uchMinorClass = CLASS_CLSETK;
		TrnClose( &ItemTransClose );
	}
}


/**===========================================================================
** Synopsis:    SHORT   ItemMiscPostReceipt( UIFREGMISC *pUifRegMisc )
*
*     Input:    *pUifRegMisc    : Pointer to Structure for UIFREGMISC
*
*    Output:    none
*
** Return:      UIF_SUCCESS     : Function performed successfully
*               UIF_CANCEL      : supervisor intervention cancel
*               LDT_SEQERR_ADR  : sequence error
*
** Description: Post Receipt/Parking Module,        R3.0,   R3.1,  V3.3
*===========================================================================*/
SHORT   ItemMiscPostReceipt( UIFREGMISC *pUifRegMisc )
{
    TRANGCFQUAL         *TranGCFQual = TrnGetGCFQualPtr();
    TRANCURQUAL         *pTranCurQual = TrnGetCurQualPtr();
    TRANMODEQUAL        *pTranModeQualL = TrnGetModeQualPtr();
    ULONG               ulModeCasIDSave;
    ITEMPRINT           ItemPrint = {0};
    CASIF               CasIf = {0};  /* V3.3 */
    SHORT               sReturnStatus;
    UCHAR               uchClass,uchCompPrintSave;

    if (pUifRegMisc->uchMinorClass == CLASS_UIPOSTRECEIPT) {
		if ((ItemMiscLocal.fsStatus & MISC_POSTRECPT) == 0) {
			return( LDT_SEQERR_ADR );
		}

        if ( CliParaMDCCheckField( MDC_POST_ADR, MDC_PARAM_BIT_D ) ) {
            if ( ( sReturnStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != UIF_SUCCESS ) {
                return( sReturnStatus );
            }
        }

		ItemMiscLocal.usCountPost++;
		if (ItemMiscLocal.usMaxReceiptsPost > 0) {
			if (ItemMiscLocal.usCountPost > ItemMiscLocal.usMaxReceiptsPost) {
				ItemMiscLocal.fsStatus &= ~MISC_POSTRECPT;
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: MISC_POSTRECPT limit reached.");
				return( LDT_BLOCKEDBYMDC_ADR );
			}
		}
    } else if (pUifRegMisc->uchMinorClass == CLASS_UIGIFTRECEIPT)
	{
		if ((ItemMiscLocal.fsStatus & MISC_GIFTRECEIPT) == 0) {
			return( LDT_SEQERR_ADR );
		}
		ItemMiscLocal.usCountGift++;
		if (ItemMiscLocal.usMaxReceiptsGift > 0) {
			if (ItemMiscLocal.usCountGift > ItemMiscLocal.usMaxReceiptsGift) {
				ItemMiscLocal.fsStatus &= ~MISC_GIFTRECEIPT;
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: MISC_GIFTRECEIPT limit reached.");
				return( LDT_BLOCKEDBYMDC_ADR );
			}
		}
    } else if (pUifRegMisc->uchMinorClass == CLASS_UIPODREPRINT)
	{
		if ((ItemMiscLocal.fsStatus & MISC_PRINTDEMAND) == 0) {
			return( LDT_SEQERR_ADR );
		}
		ItemMiscLocal.usCountDemand++;
		if (ItemMiscLocal.usMaxReceiptsDemand > 0) {
			if (ItemMiscLocal.usCountDemand > ItemMiscLocal.usMaxReceiptsDemand) {
				ItemMiscLocal.fsStatus &= ~MISC_PRINTDEMAND;
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: MISC_PRINTDEMAND limit reached.");
				return( LDT_BLOCKEDBYMDC_ADR );
			}
		}
	}else {                                            /* Parking,  R3.0 */
		if ((ItemMiscLocal.fsStatus & MISC_PARKING ) == 0 ) {
			return( LDT_SEQERR_ADR );
		}
		ItemMiscLocal.usCountPark++;
		if (ItemMiscLocal.usMaxReceiptsPark > 0) {
			if (ItemMiscLocal.usCountPark > ItemMiscLocal.usMaxReceiptsPark) {
				ItemMiscLocal.fsStatus &= ~MISC_PARKING;
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: MISC_PARKING limit reached.");
				return( LDT_BLOCKEDBYMDC_ADR );
			}
		}
    }

    if (TrnStoPutPostRToCons() != TRN_SUCCESS) {        /* copy data from post rec. file to consoli. file */
        return(LDT_PROHBT_ADR);
    }

    /* setup sign-in status for sign-out post receipt, V3.3 */
    ulModeCasIDSave = pTranModeQualL->ulCashierID;
    memset(&CasIf, 0, sizeof(CasIf));
    if ((ItemMiscLocal.ulWaiterID) && (pTranModeQualL->ulWaiterID == 0)) {
        CasIf.ulCashierNo = ItemMiscLocal.ulWaiterID;
    } else if ((ItemMiscLocal.ulCashierID) && (pTranModeQualL->ulCashierID == 0)) {
        CasIf.ulCashierNo = ItemMiscLocal.ulCashierID;
    }

    if (CasIf.ulCashierNo) {
        CasIf.usUniqueAddress = (USHORT)CliReadUAddr();
        CasIf.ulCashierOption |= CAS_MISC_SIGN_IN;    /* as surrogate sign-in */
        if ((sReturnStatus = CliCasSignIn(&CasIf)) != CAS_PERFORM) {
            sReturnStatus = CasConvertError(sReturnStatus );
            return(sReturnStatus);
        }

        /* setup for print out cashier id */
        if (pTranModeQualL->ulCashierID == 0) {
            pTranModeQualL->ulCashierID = CasIf.ulCashierNo;
            _tcsncpy(pTranModeQualL->aszCashierMnem, CasIf.auchCashierName, PARA_CASHIER_LEN);
            if ( CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER ){
                pTranModeQualL->fbCashierStatus |= MODEQUAL_CASHIER_TRAINING;
            }

        } else if (pTranModeQualL->ulWaiterID == 0) {
            pTranModeQualL->ulWaiterID = CasIf.ulCashierNo;
            _tcsncpy(pTranModeQualL->aszWaiterMnem, CasIf.auchCashierName, PARA_CASHIER_LEN);
            if ( CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER ){
                pTranModeQualL->fbWaiterStatus |= MODEQUAL_CASHIER_TRAINING;
            }
        }
    }

    /* desc. off if "release on finaleze" type */
    if ( CliParaMDCCheckField( MDC_RECEIPT_ADR, MDC_PARAM_BIT_D ) ) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
    }

    if (ulModeCasIDSave && pUifRegMisc->uchMinorClass != CLASS_UIPODREPRINT) {
		REGDISPMSG   RegDispMsg = {0}, RegDispBack = {0};
        /* display post receipt mnemonic */
        /* display only sign-in, V3.3 */
        RegDispMsg.uchMajorClass = pUifRegMisc->uchMajorClass;
        RegDispMsg.uchMinorClass = pUifRegMisc->uchMinorClass;
        RegDispMsg.fbSaveControl = DISP_SAVE_TYPE_2;
        if (pUifRegMisc->uchMinorClass == CLASS_UIPOSTRECEIPT) {
			RflGetTranMnem (RegDispMsg.aszMnemonic, TRN_POST_ADR);
        } else if (pUifRegMisc->uchMinorClass == CLASS_UIGIFTRECEIPT) {
			RflGetTranMnem (RegDispMsg.aszMnemonic, TRN_GIFT_RECEIPT_ADR);
        } else {
			RflGetTranMnem (RegDispMsg.aszMnemonic, TRN_PARKING_ADR);
        }
        ItemTendDispIn( &RegDispBack );
        DispWriteSpecial( &RegDispMsg, &RegDispBack );
    }

	/* consecutive no. and current qualifier count up on */
	// this receipt will have a different consecutive number than the original
	// to allow for the Electronic Journal entry to also have a different number.
	// change made to allow VCS EJ Viewer application to display original and copies
	// from the electronic journal.
	ItemCountCons();
	CliParaSaveFarData();        // Update the consecutive number persistent store

    /* print post recceipt header */
    TranGCFQual->uchSlipLine = 0;
    TranGCFQual->uchPageNo = 0;
	TranGCFQual->fsGCFStatus2 |= PRT_DBL_POST_RECEIPT;
    
    ItemCurPrintStatus();
	uchCompPrintSave = pTranCurQual->fbCompPrint;
	if ( CliParaMDCCheckField( MDC_POST_ADR, MDC_PARAM_BIT_C ) ) { //SR492
		pTranCurQual->fbCompPrint &= ~PRT_SLIP;         
	}
	pTranCurQual->fbNoPrint &= ~CURQUAL_NO_R;         /* reset no receipt option */	
    ItemPrintStart( TYPE_THROUGH );                 /* prin module initialize */
	pTranCurQual->fbCompPrint = uchCompPrintSave;

    memset( &ItemPrint, 0, sizeof( ITEMPRINT ) );
    ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
	if (pTranCurQual->fsCurStatus & (CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP)) {
		pTranCurQual->fsCurStatus &= ~(CURQUAL_DUPLICATE_COPY | CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);
		pTranCurQual->fsCurStatus |= CURQUAL_CUST_DUP;
	} else {
		pTranCurQual->fsCurStatus &= ~(CURQUAL_DUPLICATE_COPY | CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);
	}
    if (pUifRegMisc->uchMinorClass == CLASS_UIPOSTRECEIPT) {
        uchClass = ItemPrint.uchMinorClass = CLASS_POST_RECPT; /* post receipt header */
        pTranCurQual->flPrintStatus |= CURQUAL_POSTRECT;
		// indicate uses transaction mnemonic TRN_POST_ADR
        pTranCurQual->fsCurStatus2 |= PRT_POST_RECEIPT;
    } else if(pUifRegMisc->uchMinorClass == CLASS_UIGIFTRECEIPT)
	{
		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		uchClass = ItemPrint.uchMinorClass = CLASS_GIFT_RECEIPT;
		// indicate uses transaction mnemonic TRN_GIFT_RECEIPT_ADR
        pTranCurQual->fsCurStatus2 |= PRT_GIFT_RECEIPT;
    } else if(pUifRegMisc->uchMinorClass == CLASS_UIPODREPRINT)
	{
		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		uchClass = ItemPrint.uchMinorClass = CLASS_GIFT_RECEIPT;
		// indicate uses transaction mnemonic TRN_EJ_PODREPRINT
        pTranCurQual->fsCurStatus2 |= PRT_DEMAND_COPY;
	}else {
        uchClass = ItemPrint.uchMinorClass = CLASS_PARK_RECPT;
        pTranCurQual->flPrintStatus |= CURQUAL_PARKING;
    }

	ItemPrint.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);
    ItemPrint.fbStorageStatus |= (NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);  /* not storage neither buffer */   
    TrnThrough( &ItemPrint );

	{
		ITEMTRANSCLOSE    ItemTransClose = {0};

		/*
		 * print post receipt (receipt or slip)
		 * the function TrnStoGetConsToPostR() must be used to transfer the last
		 * transaction to the Post Receipt storage area in order for this to work.
		**/
		pTranCurQual->uchPrintStorage = PRT_CONSOLSTORAGE; /* print storage buffer */
		pTranCurQual->uchKitchenStorage = PRT_NOSTORAGE;   /* not send kitchen */
		pTranCurQual->fbNoPrint = 0;
		pTranCurQual->fbNoPrint |= (CURQUAL_NO_EPT_LOGO_SIG | CURQUAL_NO_SECONDCOPY);

		ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
		if(pUifRegMisc->uchMinorClass == CLASS_UIGIFTRECEIPT)
		{
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
			ItemTransClose.uchMinorClass = CLASS_CLS_GIFT_RECEIPT;
		} else if (pUifRegMisc->uchMinorClass == CLASS_UIPODREPRINT)
		{
			ItemTransClose.uchMinorClass = CLASS_CLSPRINT;
		} else
		{
			ItemTransClose.uchMinorClass = CLASS_CLSPOSTRECPT;
		}
		TranGCFQual->fsGCFStatus2 &= ~PRT_DBL_POST_RECEIPT;
		TrnClose( &ItemTransClose );
	}

    /* Initialize all local datas */
    ItemInit();
    TrnInitialize( TRANI_CONSOLI | TRANI_GCFQUAL | TRANI_CURQUAL );

    /*----- Affect Post/Parking Receipt Counter to Cashier File,  R3.1 -----*/
    if (pTranModeQualL->ulCashierID || pTranModeQualL->ulWaiterID) { /* V3.3 */
        ItemMiscPostReceiptAffect(uchClass);
    }

    /* sign out, V3.3 */
    if (CasIf.ulCashierNo) {
        ItemMiscTipsPOSignOut( &CasIf );

        /* clear mode qualifier of cashier */
        if (ulModeCasIDSave == 0) {
            TrnInitialize( TRANI_MODEQUAL );
        }
    }

    return( UIF_SUCCESS);
}

/**===========================================================================
** Synopsis:    VOID   ItemMiscPostReceiptAffect(UCHAR uchClass)
*
*     Input:    
*
*    Output:    
*
** Return:      none
*
** Description:     Post/Parking Receipt Counter to Cashier File
*===========================================================================*/
VOID   ItemMiscPostReceiptAffect(UCHAR uchClass)
{
    ITEMOTHER       Other = {0};
    ITEMTRANSOPEN   TransOpen = {0};
	ITEMTRANSCLOSE  TransClose = {0};
    SHORT           sStatus;

    TrnInitialize(TRANI_ITEM);

    /*----- Make Transaction Open Data -----*/
    TransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    TransOpen.uchMinorClass = CLASS_OPENPRINT;
    TrnOpen(&TransOpen);    

    /*----- Make Other Data -----*/
    Other.uchMajorClass = CLASS_ITEMOTHER;
    if (uchClass == CLASS_POST_RECPT) {
        Other.uchMinorClass = CLASS_POSTRECEIPT;
    } else {
        Other.uchMinorClass = CLASS_PARKING;
    }
    TrnItem(&Other);    

    /*----- Make Transaction Close Data -----*/
    TransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    TransClose.uchMinorClass = CLASS_CLSPRINT;
    TrnClose(&TransClose);    

    /*----- Send Affection Data -----*/
    while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
        UieErrorMsg(GusConvertError(sStatus), UIE_WITHOUT);
    }

    TrnInitialize(TRANI_ITEM);
}

/**===========================================================================
** Synopsis:    VOID   ItemMiscSetStatus( USHORT fsStatusBit )
*
*     Input:    fsStatusBit
*
*    Output:    ItemMiscLocal.fsStatus
*
** Return:      none
*
** Description: set misc local status bit.
*===========================================================================*/
VOID   ItemMiscSetStatus( USHORT fsStatusBit )

{
    ItemMiscLocal.fsStatus |= fsStatusBit;
}



/**===========================================================================
** Synopsis:    VOID   ItemMiscResetStatus( USHORT fsStatusBit )
*
*     Input:    fsStatusBit
*
*    Output:    ItemMiscLocal.fsStatus
*
** Return:      none
*
** Description: reset misc local status bit.
*===========================================================================*/

VOID   ItemMiscResetStatus( USHORT fsStatusBit )
{
    ItemMiscLocal.fsStatus &= ~fsStatusBit;
}

/**===========================================================================
** Synopsis:    VOID   ItemMiscSetNonGCStatus( VOID )
*
*     Input:    fsStatusBit
*
*    Output:    ItemMiscLocal.fsStatus
*
** Return:      none
*
** Description: reset misc local status bit.
*===========================================================================*/

VOID   ItemMiscSetNonGCStatus(VOID)
{
    ItemCommonGetLocalPointer()->fbCommonStatus |= COMMON_NON_GUEST_CHECK;
}


/*
*===========================================================================
**Synopsis: VOID    ItemMoneyHeader()
*
*    Input: None
*   Output: None
*    InOut: None
*
**Return:   None
*
** Description: Send Header data to transaction module.
*===========================================================================
*/

static VOID    ItemMoneyHeader(VOID)
{
	ITEMPRINT   ItemPrint = {0};

    ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
    ItemPrint.uchMinorClass = CLASS_MONEY_HEADER;
    ItemPrint.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    TrnItem(&ItemPrint);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscMoneyOpen(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
** Description: This function makes data for transaction open.  
*===========================================================================
fhfh*/
VOID    ItemMiscMoneyOpen(VOID)
{
	ITEMTRANSOPEN   ItemTransOpen = {0};

    ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen.uchMinorClass = CLASS_OPENMONEY;
    ItemTransOpen.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;

    ItemCountCons();
    ItemCurPrintStatus();

	{
		TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */

		/* check unbuffering or other */      
		pCurQualRcvBuff->uchPrintStorage = PRT_ITEMSTORAGE;
		if ((pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
			// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
			// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
			// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
			pCurQualRcvBuff->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
			pCurQualRcvBuff->fbCompPrint &= ~ CURQUAL_COMP_S;
		} 
		pCurQualRcvBuff->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */
	}

    ItemPrintStart(TYPE_STORAGE);
    ItemHeader(TYPE_STORAGE);
    ItemMoneyHeader();
    TrnOpen(&ItemTransOpen);

    /*--- LCD Initialize ---*/
    MldDisplayInit(MLD_DRIVE_1, 0);

    /*--- Display Cashier/Waiter No. ---*/
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscMoneyIn( VOID )
*
*     Input:    None
*    Output:    None
*
** Return:      UIF_SUCCESS, LDT_SEQERR_ADR, LDT_PROHBT_ADR
*
** Description: money declaration-in
*===========================================================================
fhfh*/
SHORT   ItemMiscMoneyIn( VOID )
{
	REGDISPMSG      RegDisplay = {0};
    SHORT           sStatus;

    /* check if tax modifier key or food stamp modifier was used ? */
    if (ItemModLocalPtr->fsTaxable != 0) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD ) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if (TranModeQualPtr->ulCashierID == 0 ) {
        return(LDT_PROHBT_ADR);
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        return(LDT_SEQERR_ADR);
    }

    if (CliParaMDCCheck(MDC_MONEY1_ADR, ODD_MDC_BIT0)) {
        return(LDT_PROHBT_ADR);
    }
    if (CliParaMDCCheck(MDC_MONEY1_ADR, ODD_MDC_BIT1)) {
        if ((sStatus = ItemSPVInt(LDT_SUPINTR_ADR)) != UIF_SUCCESS) {
            return(sStatus);
        }
    }

    RegDisplay.uchMajorClass = CLASS_UIFREGMISC;
    RegDisplay.uchMinorClass = CLASS_UIFMONEY_IN;
    RflGetTranMnem (RegDisplay.aszMnemonic, TRN_ONHAND_ADR);

    uchDispRepeatCo = 1;                    /* init repeat counter */

    DispWrite(&RegDisplay);

    if (!(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
        ItemMiscResetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
        ItemMiscMoneyOpen();
        ItemDrawer();
        UieDrawerComp(UIE_DISABLE);
        UieModeChange(UIE_DISABLE);
    }

    return(UIF_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscMoney( UIFREGOTHER  *pItemMisc )
*
*     Input:    pItemMisc - POINTER from UI
*    Output:    None
*
** Return:
*
** Description: This function is entry of tender amount.
*===========================================================================
fhfh*/

static struct {
	USHORT  usTender;
	USHORT  usMnemonicId;
	USHORT  usDisplayClass;
} myMnemonicList [] = {
	{CLASS_UIFOREIGN1, TRN_FT1_ADR,CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN2, TRN_FT2_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN3, TRN_FT3_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN4, TRN_FT4_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN5, TRN_FT5_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN6, TRN_FT6_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN7, TRN_FT7_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UIFOREIGN8, TRN_FT8_ADR, CLASS_UIFMONEY_FOREIGN},
	{CLASS_UITENDER1, TRN_TEND1_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER2, TRN_TEND2_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER3, TRN_TEND3_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER4, TRN_TEND4_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER5, TRN_TEND5_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER6, TRN_TEND6_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER7, TRN_TEND7_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER8, TRN_TEND8_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER9, TRN_TEND9_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER10, TRN_TEND10_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER11, TRN_TEND11_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER12, TRN_TENDER12_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER13, TRN_TENDER13_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER14, TRN_TENDER14_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER15, TRN_TENDER15_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER16, TRN_TENDER16_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER17, TRN_TENDER17_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER18, TRN_TENDER18_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER19, TRN_TENDER19_ADR, CLASS_UIFMONEY},
	{CLASS_UITENDER20, TRN_TENDER20_ADR, CLASS_UIFMONEY}
};

SHORT   ItemMiscMoney(UIFREGMISC *pMisc)
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    ITEMCOMMONLOCAL *pCommon = ItemCommonGetLocalPointer();
	REGDISPMSG      RegDisplay = {0};
    ITEMMISC        TrnMisc = {0};
    SHORT           sStatus, sCompSize;
    USHORT          usBuffSize;

	if (ItemTenderCheckTenderMdc(pMisc->uchTendMinor, 5, MDC_PARAM_BIT_D)) {  // allow/disallow money declaration this tender
		return(LDT_PROHBT_ADR);
	}

    if (pMisc->lQTY % 1000L) {
        return(LDT_SEQERR_ADR);
    }

    /* check money product amount */
    TrnMisc.lAmount = pMisc->lQTY / 1000L * pMisc->lAmount;

    if (pMisc->fbModifier & VOID_MODIFIER) {
		ITEMMISC   DummyMisc = {0};
        ItemPreviousItem(&DummyMisc, 0);

        switch (pMisc->uchTendMinor) {
        case    CLASS_UIFOREIGN1:
        case    CLASS_UIFOREIGN2:
        case    CLASS_UIFOREIGN3:
        case    CLASS_UIFOREIGN4:
        case    CLASS_UIFOREIGN5:
        case    CLASS_UIFOREIGN6:
        case    CLASS_UIFOREIGN7:
        case    CLASS_UIFOREIGN8:
            if (TrnMisc.lAmount > WorkTI->lFCMoney[pMisc->uchTendMinor - CLASS_UIFOREIGN1]) {
                return(LDT_KEYOVER_ADR);
            }
            break;

        default:
            if (TrnMisc.lAmount > WorkTI->lTenderMoney[pMisc->uchTendMinor - CLASS_UITENDER1]) {
                return(LDT_KEYOVER_ADR);
            }
            break;
        }
    }

    TrnMisc.uchMajorClass   = CLASS_ITEMMISC;
    TrnMisc.uchMinorClass   = CLASS_MONEY;
    switch (pMisc->uchTendMinor) {
    case    CLASS_UIFOREIGN1:
    case    CLASS_UIFOREIGN2:
    case    CLASS_UIFOREIGN3:
    case    CLASS_UIFOREIGN4:
    case    CLASS_UIFOREIGN5:
    case    CLASS_UIFOREIGN6:
    case    CLASS_UIFOREIGN7:
    case    CLASS_UIFOREIGN8:
        TrnMisc.uchTendMinor = pMisc->uchTendMinor - CLASS_UIFOREIGN1 + CLASS_FOREIGN1;
        break;

    default:
        TrnMisc.uchTendMinor = pMisc->uchTendMinor - CLASS_UITENDER1 + CLASS_TENDER1;
        break;
    }
    TrnMisc.lQTY            = pMisc->lQTY;
    TrnMisc.lUnitPrice      = pMisc->lAmount;
    TrnMisc.fbModifier      = pMisc->fbModifier;
    _tcsncpy(TrnMisc.aszNumber, pMisc->aszNumber, NUM_NUMBER);
	TrnMisc.fsPrintStatus   = PRT_JOURNAL | PRT_SLIP;

    if (pMisc->fbModifier & VOID_MODIFIER) {
        TrnMisc.lAmount *= -1L;
    }

    if((pMisc->uchTendMinor >= CLASS_UIFOREIGN1) && (pMisc->uchTendMinor <= CLASS_UIFOREIGN8)) {
		PARAMDC   WorkMDC = {0};
        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        if (pMisc->uchTendMinor <= CLASS_UIFOREIGN2) {
            WorkMDC.usAddress = MDC_FC1_ADR + pMisc->uchTendMinor - CLASS_UIFOREIGN1;
        } else {
            WorkMDC.usAddress = MDC_FC3_ADR + pMisc->uchTendMinor - CLASS_UIFOREIGN3;
        }
        CliParaRead(&WorkMDC);
        TrnMisc.fchStatus = WorkMDC.uchMDCData;

        if (pMisc->uchTendMinor == CLASS_UIFOREIGN2
            || pMisc->uchTendMinor == CLASS_UIFOREIGN4
            || pMisc->uchTendMinor == CLASS_UIFOREIGN6
            || pMisc->uchTendMinor == CLASS_UIFOREIGN8) {
            TrnMisc.fchStatus >>= 4;
        }
    }

	// Look up the mnemonic and the display class for this particular
	// tender key.
    RegDisplay.uchMajorClass = CLASS_UIFREGMISC;
	{
		int     i;

		for (i = 0; i < sizeof(myMnemonicList)/sizeof(myMnemonicList[0]); i++) {
			if (pMisc->uchTendMinor == myMnemonicList[i].usTender) {
				RflGetTranMnem (RegDisplay.aszMnemonic, myMnemonicList[i].usMnemonicId);
				RegDisplay.uchMinorClass = myMnemonicList[i].usDisplayClass;
				RegDisplay.lAmount = TrnMisc.lAmount;
				break;
			}
		}
	}

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
    if (TrnMisc.fchStatus & 0x02) {
        RegDisplay.fbSaveControl |= DISP_SAVE_DECIMAL_3;

        if (TrnMisc.fchStatus & 0x04) {
            RegDisplay.fbSaveControl |= DISP_SAVE_DECIMAL_0;
        }
    }

    if (pMisc->lQTY == 1000L) {
        RegDisplay.lQTY = 0L;
    } else {
        RegDisplay.lQTY = pMisc->lQTY;
    }

    flDispRegKeepControl &= ~VOID_CNTRL;
    DispWrite(&RegDisplay);
    flDispRegDescrControl &= ~VOID_CNTRL;

    MldScrollWrite(&TrnMisc, MLD_NEW_ITEMIZE);

    usBuffSize = pCommon->usSalesBuffSize + pCommon->usDiscBuffSize;
    if ((sCompSize = ItemCommonCheckSize(&TrnMisc, usBuffSize)) < 0) {
        return(LDT_TAKETL_ADR);
    }
    if ((sStatus = ItemPreviousItem(&TrnMisc, sCompSize)) != ITM_SUCCESS) {
        return(sStatus);
    }

    return(UIF_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemMiscMoneyOut( VOID )
*
*     Input:    None
*    Output:    None
*
** Return:      none
*
** Description: money declaration-out
*===========================================================================
fhfh*/
SHORT   ItemMiscMoneyOut(VOID)
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    REGDISPMSG      RegDisplay = {0};
    ITEMMISC        DummyMisc = {0};
	ITEMTRANSCLOSE  ItemTransClose = {0};
    SHORT           sStatus;

    ItemPreviousItem(&DummyMisc, 0);

    RegDisplay.uchMajorClass = CLASS_UIFREGMISC;
    RegDisplay.uchMinorClass = CLASS_UIFMONEY_OUT;
    RflGetTranMnem (RegDisplay.aszMnemonic, TRN_ONHAND_ADR);

    DispWrite(&RegDisplay);

    ItemTrailer(TYPE_MONEY);

    pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
		// if this is a Pre-GuestCheck Unbuffered then the printing is done as items are entered and are
		// not saved to be printed out all at once. If we do not set the uchPrintStorage correctly then we
		// will get two receipts rather than one if the P2 dialog System Type is set to Pre-GuestCheck Unbuffered.
		pWorkCur->uchPrintStorage = PRT_NOSTORAGE;        /* not print item/consoli. data */
    }
	pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;      /* not send kitchen */

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSMONEY;
    TrnClose(&ItemTransClose);

    while ((sStatus = TrnSendTotal()) != TRN_SUCCESS) {
        UieErrorMsg(GusConvertError(sStatus), UIE_WITHOUT);
    }

    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

    FDTCancel();

    ItemMiscSetNonGCStatus();   /* V3.3 for release on finalize */

    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1)) {
        UieDrawerComp(UIE_DISABLE);
    } else {
        UieDrawerComp(UIE_ENABLE);
    }

    return(ItemFinalize());
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemMiscMoneyEC( ITEMMISC   *pTrnMisc )
*
*     Input:    ITEMMISC    *pTrnMisc
*    Output:    None
*
** Return:
*
** Description: E/C of tender amount
*===========================================================================
fhfh*/

SHORT   ItemMiscMoneyEC(ITEMMISC *pTrnMisc)
{
    ITEMCOMMONLOCAL *pCommon = ItemCommonGetLocalPointer();
	REGDISPMSG      RegDisplay = {0};

    pTrnMisc->lAmount *= -1L;

    RegDisplay.uchMajorClass = CLASS_UIFREGMISC;
    RegDisplay.uchMinorClass = CLASS_UIFMONEY;
    RegDisplay.lAmount = pTrnMisc->lAmount;
    RflGetTranMnem (RegDisplay.aszMnemonic, TRN_EC_ADR);

    if (pTrnMisc->lQTY == 1000L) {
        RegDisplay.lQTY = 0L;
    } else {
        RegDisplay.lQTY = pTrnMisc->lQTY;
    }

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
    if (pTrnMisc->fchStatus & 0x02) {
        RegDisplay.fbSaveControl |= DISP_SAVE_DECIMAL_3;
        if (pTrnMisc->fchStatus & 0x04) {
            RegDisplay.fbSaveControl |= DISP_SAVE_DECIMAL_0;
        }
    }

    flDispRegKeepControl &= ~EC_CNTRL;
    DispWrite(&RegDisplay);
    flDispRegDescrControl &= ~EC_CNTRL;

    pCommon->uchItemNo--;
    memset(&pCommon->ItemSales, 0 , sizeof(ITEMSALES));
    pCommon->usSalesBuffSize = 0;

    return(UIF_SUCCESS);
}
/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemMiscAgeCancel(VOID)
*
*     Input:    None
*    Output:    None
*
** Return:
*
** Description: Reset the Age if a person rings up an age restricted item.
* 
* SR 219, when cancelling an Item that is age restricted AND that item is the
* first item rang up, when you hit cancel, it does not clear the ItemSalesLocal 
* value of the age.  When you try to ring up the next person in the line, the age
* value would still remain as the age for the person before.
*===========================================================================
fhfh*/
VOID ItemMiscAgeCancel(VOID)
{
	ItemSalesLocal.uchAge = 0;
}

/*
*===========================================================================
** Synopsis:    VOID ItemVoidGiftCardCancel(VOID)
*
*     Input:    None
*    Output:    None
*
** Return:
*
** Description:  Void a previously processed gift card.
*                See also function ItemTendGiftCard() which processes Gift Card PLUs.
*===========================================================================
*/
VOID ItemVoidGiftCardCancel(VOID)
{
	TRANGCFQUAL    *pWorkGCF = TrnGetGCFQualPtr();
	USHORT          usNo=1;
	SHORT           error;
	USHORT          index=0;
	
	for (index = 0; index <= NUM_SEAT_CARD; index++)
	{
		if (pWorkGCF->TrnGiftCard[index].Used == GIFT_CARD_USED)
		{
			ITEMTENDER      ItemTender;
			UIFREGTENDER	UifRegTender = {0};

			ItemTender = pWorkGCF->TrnGiftCard[index].ItemTender;
			UifRegTender.GiftCardOperation = pWorkGCF->TrnGiftCard[index].GiftCard;
			ItemTender.uchMajorClass = CLASS_ITEMTENDER;

			if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_ISSUE)
			{
				UifRegTender.GiftCardOperation = GIFT_CARD_VOID;
			}
			else if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_RELOAD)
			{
				UifRegTender.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
			}
			else if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID)
			{
				UifRegTender.GiftCardOperation = GIFT_CARD_ISSUE;
			}
			else if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID_RELOAD)
			{
				UifRegTender.GiftCardOperation = GIFT_CARD_RELOAD;
			}

			error = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);
		}
	}
}


/****** End of Source ******/