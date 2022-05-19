/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP3.C (addcheck/manual addcheck)                                       
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
:  Abstract:
:   ItemTransOpenAC()       ;   addcheck/recall
:   ItemTransACNor()        ;   normal addcheck operation
:   ItemTransACNorCheck()   ;   check addcheck operation
:   ItemTransACNorKey()     ;   addcheck key sequence
:   ItemTransAddcheck()     ;   generate transaction open data
:   ItemTransACNorAff()     ;   generate affection data
:
:   ItemTransACMan()        ;   manual addcheck
:   ItemTransACManCheck()   ;   check manual addcheck operation
:   ItemTransACKeyMan()     ;   key sequence of manual addcheck operation
:   ItemTransManAdd()       ;   generate transaction open data
:   ItemTransACManAff()     ;   generate affection data
:   ItemTransACSPVInt()     ;   supervisor intervention
:   ItemTransACManDisp()    ;   display of manual addcheck  
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name      : Description
: 92/5/28  : 00.00.01 :   hkato     : initial
: 95/1/12  :          :   hkato     : R3.0
: 95/11/17 : 03.01.00 :   hkato     : R3.1
: 98/7/31  : 03.03.00 :   hrkato    : V3.3 (New Key Sequence)
: 10/21/98 : 03.03.00 :   M.Suzuki  : TAR 89859
: 08/13/99 : 03.05.00 :   K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csstbgcf.h"
#include    "csgcs.h"
#include    "rfl.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"
#include    "pmg.h"
#include	"cscas.h"
#include    "fdt.h"


/*==========================================================================
**   Synopsis:  SHORT   ItemTransOpenAC( UIFREGTRANSOPEN *UifRegTransOpen )   
*                               
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    addcheck or recall operation to retrieve an existing guest check
*                   from the guest check file to create a transaction.
==========================================================================*/
        
SHORT   ItemTransOpenAC(UIFREGTRANSOPEN *UifRegTransOpen)
{
    SHORT           sStatus = LDT_PROHBT_ADR;

    if ( UifRegTransOpen->uchMinorClass == CLASS_UICASINTRECALL ) { /* cashier interrupt recall, R3.3 */
        sStatus = ItemTransACCIRecall( UifRegTransOpen );
    } else if ( UifRegTransOpen->uchMinorClass == CLASS_UIRECALL ) {/* recall, R3.0 */
		// we should see a recall operation only with a Store/Recall system type.
		if (RflGetSystemType () != FLEX_STORE_RECALL) {
			sStatus = LDT_PROHBT_ADR;
		} else {
			// recall guest check so either get the next one from a delivery queue if we are a
			// Drive Thru terminal or the last stored if we are a counter terminal.
			FDTPARA     WorkFDT = { 0 };

			FDTParameter(&WorkFDT);                                /* Get FDT Parameter */
			if (WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_2) {          /* Delivery Terminal */
				sStatus = ItemTransACStoDeliv( UifRegTransOpen );
			} else {
				sStatus = ItemTransACSto( UifRegTransOpen );
			}
		}
    } else {
        if (UifRegTransOpen->uchMinorClass == CLASS_UINEWKEYSEQ) {  /* V3.3 */
            sStatus = ItemTransACNor(UifRegTransOpen);
            if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {
                return(ITM_SUCCESS);
            }
        } else if (UifRegTransOpen->uchMinorClass == CLASS_UINEWKEYSEQGC) { /* V3.3 */
            if ( TranCurQualTotalStatusExist() ) {
				 sStatus = ItemTransACMul(UifRegTransOpen);
			}
			else {
                return(LDT_SEQERR_ADR);
			}
        } else if ( UifRegTransOpen->ulAmountTransOpen != 0L        /* manual addcheck */
            || UifRegTransOpen->fbInputStatus & INPUT_0_ONLY ) {
            sStatus = ItemTransACMan(UifRegTransOpen);
        } else {
            if ( TranCurQualTotalStatusExist() ) {
                sStatus = ItemTransACMul(UifRegTransOpen);
            } else {
                sStatus = ItemTransACNor( UifRegTransOpen );        /* normal addcheck */
            }
        }
    }

    if ( sStatus == ITM_SUCCESS ) {
		TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();

		TrnGetCurQualPtr()->fsCurStatus |= CURQUAL_ADDCHECK;                  /* indicate addcheck operation */
        ItemTransLocal.sPrePigRuleCo = pWorkGCF->sPigRuleCo;        /* Add 10-21-98 */
        if ( TranModeQualPtr->ulWaiterID == 0 ) {                   /* cashier operation */
            pWorkGCF->fsGCFStatus |= GCFQUAL_ADDCHECK_CASHIER;      /* addcheck is cashier */
        }
        ItemSalesGetLocalPointer()->fbSalesStatus |= SALES_ITEMIZE; /* itemize state */
        ItemSalesCalcInitCom();                                     /* ppi initialize r3.1 2172 */
        ItemModGetLocalPtr()->fsTaxable = 0;                        /* taxable status */

        UieModeChange( UIE_DISABLE );                               /* mode unchangable */
    }

    return( sStatus );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACNor( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    addcheck operation
==========================================================================*/
        
SHORT   ItemTransACNor( UIFREGTRANSOPEN *UifRegTransOpen )   
{
	ITEMTRANSOPEN   ItemTransOpen = {0};
    SHORT           sStatus;

    if ( RflGetSystemType () == FLEX_STORE_RECALL ) {
		/* addcheck not allowed with a store/recall system */
        return( LDT_PROHBT_ADR );
    }

    if ((sStatus = ItemTransACNorCheck(UifRegTransOpen)) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* --- New Key Sequence,    V3.3 --- */
    if (UifRegTransOpen->uchMinorClass == CLASS_UINEWKEYSEQ) {
        if ((sStatus = ItemTransACNewKey(UifRegTransOpen, &ItemTransOpen)) != ITM_SUCCESS) {
            return(sStatus);
        }
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {
            return(ITM_SUCCESS);
        }
    } else {
        if ((sStatus = ItemTransACNorKey(&ItemTransOpen)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    ItemTransNCPara();                                              /* set current qualifier */

    if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER 
        || ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER
        || ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            && TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) ) {

        if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            || TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) {                        /* print transaction header */

            ItemTransLocal.fbTransStatus |= TROP_OPENPOSTCHECK;     /* transaction status */
        }                

        ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                     /* print promotion header */
        ItemHeader( TYPE_STORAGE );                                 /* p-void, training header 8/24/93 */
    } else {                                                        /* postcheck, store/recall */
        ItemPrintStart( TYPE_POSTCHECK );                           /* start print service */
        ItemHeader( TYPE_CANCEL );                                  /* p-void, training header 8/24/93 */
    }

    ItemTransAddcheck( &ItemTransOpen );                            /* generate transaction open data */
    TrnOpen( &ItemTransOpen );                                      /* transaction module i/F */

    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransOpen, 0);

    /*--- Display GCF#, R3.1 ---*/
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER
        || (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER) {
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else if (CliParaMDCCheckField( MDC_DISPLAY_ADR, MDC_PARAM_BIT_A ) ) { 
		/* R3.3 */
		UCHAR   uchMinor = ItemTransOpen.uchMinorClass;
        ItemTransOpen.uchMinorClass = CLASS_REORDER;               /* for display */
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
        ItemTransOpen.uchMinorClass = uchMinor;
    } else {
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    }

	MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

    ItemTransACNorAff();                                            /* generate affection data */

    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACNorCheck( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check normal addcheck operation
==========================================================================*/

SHORT   ItemTransACNorCheck( UIFREGTRANSOPEN    *UifRegTransOpen )   
{
    /* prohibit guest check open, V3.3 */
    if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_GUEST_CHECK_OPEN) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier setting CAS_NOT_GUEST_CHECK_OPEN prohibits.");
        return(LDT_PROHBT_ADR);
    }

    if ( ItemModLocalPtr->fsTaxable ) {
		/* sequence error if tax modifier key has been used */
        return( LDT_SEQERR_ADR );
    }
    
    if ( UifRegTransOpen->fbModifierTransOpen ) {
		/* sequence error if Void key has been used */
        return( LDT_SEQERR_ADR );
    }

    if ( TranGCFQualPtr->usGuestNo ) {                              /* check multi check payment */
		// Already have a guest check started so sequence error.
        return( LDT_SEQERR_ADR );
    }

    if ( ItemTransLocal.fbTransStatus & TROP_DURINGADDCHECK ) {     /* during manual addcheck */
        return( LDT_PROHBT_ADR );                                   /* Prohibit Operation */
    }

    return( ITM_SUCCESS );                                          
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACNorKey( ItemTransOpen *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of addcheck operation
==========================================================================*/

SHORT   ItemTransACNorKey( ITEMTRANSOPEN   *ItemTransOpen )   
{
    TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    SHORT           sStatus, sType;
    USHORT          usStatus;
    ULONG           ulGuestNo;

    if ( ( sStatus = ItemTransGCFNo( &ulGuestNo ) ) != ITM_SUCCESS ) {
        ItemTransDispDef();   
        return( sStatus );
    }

	{
		RflGcfNo  gcfCd = RflPerformGcfCd (ulGuestNo);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		ulGuestNo = gcfCd.usGcNumber;
	}

    sType = 0;                                                      /* precheck system */
    if ( RflGetSystemType () == FLEX_POST_CHK ) {                   /* postcheck system */
		// indicate that we need the consolidated transaction data storage as well.
		// Store/Recall and Post Check both store additional memory resident guest check data
		// in a guest check stored in the Guest Check file.
        sType = 1;                                                  /* precheck system */
    }

    sStatus = TrnGetGC( ( USHORT )ulGuestNo, sType, GCF_REORDER );  /* R3.0 */
    if ( sStatus < 0 ) {                                            /* error */
        usStatus = GusConvertError( sStatus );
        return( ( SHORT )usStatus );            
    } else if ( sStatus == 0 ) {                                    /* no size GCF */
        ItemCommonCancelGC(pWorkGCF);                                /* R3.0 */
        return ( LDT_ILLEGLNO_ADR );                                /* Illegal Guest Check # */
    }

    /* check guest check team no, V3.3 */
    if ( !(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM)) {        /* not use team */
        /* V3.3 ICI */
        if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_CLOSE_ANY_CHECK) { /* not close any check */
            if (TranModeQualPtr->ulCashierID != pWorkGCF->ulCashierID ) {    /* check cashier id */
                ItemCommonCancelGC(pWorkGCF);                        /* R3.0 */
                return( LDT_NOUNMT_ADR );                           /* Cashier/Waiter # Unmatch */
            }
        }
    } else {                                                    /* use team */
        if (TranModeQualPtr->uchCasTeamNo != 0) {                       /* not head cashier */
            if (TranModeQualPtr->uchCasTeamNo != pWorkGCF->uchTeamNo) {  /* team no unmatch */
                ItemCommonCancelGC(pWorkGCF);                    /* R3.0 */
                return( LDT_NOUNMT_ADR );                       /* Cashier/Waiter # Unmatch */
            }
        }
    }
/*** delete V3.3
    if ( WorkMode.usWaiterID ) {                                    / waiter operation /
        if ( WorkGCF->fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER         / cashier GCF /
            || WorkMode.usWaiterID != WorkGCF->usWaiterID ) {       / check waiter id /
            ItemCommonCancelGC(WorkGCF);                            / R3.0 /
            return( LDT_NOUNMT_ADR );                               / Cashier/Waiter # Unmatch /
        }

    } else {                                                        / cashier operation /
        if ( WorkGCF->usWaiterID == 0 
            && WorkMode.usCashierID != WorkGCF->usCashierID ) {     / check cashier id /
            ItemCommonCancelGC(WorkGCF);                            / R3.0 /
            return( LDT_NOUNMT_ADR );                               / Cashier/Waiter # Unmatch /
        } 
    }
****/

	sStatus = ITM_SUCCESS;
	do {    // do once, series of tests to determine if prohibit action or not.
		if ( (pWorkGCF->fsGCFStatus & GCFQUAL_PVOID ) == GCFQUAL_PVOID && ( pWorkCur->fsCurStatus & CURQUAL_PVOID ) != CURQUAL_PVOID ) {  /* p-void GCF */
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}
		if ( (pWorkGCF->fsGCFStatus & GCFQUAL_PVOID ) != GCFQUAL_PVOID && ( pWorkCur->fsCurStatus & CURQUAL_PVOID ) == CURQUAL_PVOID ) {  /* p-void GCF */
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}

		if ((pWorkGCF->fsGCFStatus & GCFQUAL_WIC) == GCFQUAL_WIC && (pWorkCur->fsCurStatus & CURQUAL_WIC) != CURQUAL_WIC) {
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}
		if ((pWorkGCF->fsGCFStatus & GCFQUAL_WIC) != GCFQUAL_WIC && (pWorkCur->fsCurStatus & CURQUAL_WIC) == CURQUAL_WIC) {
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}

		if ( !(pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING ) ) {           /* normal */    
			if ( (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
				sStatus = ( LDT_PROHBT_ADR );
				break;
			}
		} else {                                                        /* training */
			if ( !(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) && !(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
				sStatus = ( LDT_PROHBT_ADR );
				break;
			}
		}    
	} while (0);  // do only once the first time through.
	if (sStatus != ITM_SUCCESS) {
        ItemCommonCancelGC(pWorkGCF);                                /* R3.0 */
		return( sStatus );
	}
    
    /*----- Seat Number Entry for SplitCheck,   R3.1 -----*/
    if (RflGetSystemType () == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {

        if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT1)) {
            if ((sStatus = ItemTransSeatNo(ItemTransOpen, 0)) != ITM_SUCCESS) {
                ItemCommonCancelGC(pWorkGCF);
                return(sStatus);
            }
        }
    }

    ItemTransOpen->usGuestNo = pWorkGCF->usGuestNo;                  /* save GCF No */
    ItemTransOpen->uchCdv = pWorkGCF->uchCdv;                        /* save CDV */

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACNewKey(UifRegTransOpen *pOpen, ItemTransOpen *pTransOpen)
*
*   Input:      
*   Output:     
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of NC/RO/AC operation,         V3.3
==========================================================================*/

SHORT   ItemTransACNewKey(UIFREGTRANSOPEN *UifRegTransOpen, ITEMTRANSOPEN *pTransOpen)
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    SHORT           sStatus, sType;
    USHORT          usStatus, usGuestNo;

	{
		RflGcfNo  gcfCd = RflPerformGcfCd (UifRegTransOpen->ulAmountTransOpen);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		UifRegTransOpen->ulAmountTransOpen = gcfCd.usGcNumber;
	}

    sType = 0;
    if (RflGetSystemType () == FLEX_POST_CHK) {
		// indicate that we need the consolidated transaction data storage as well.
		// Store/Recall and Post Check both store additional memory resident guest check data
		// in a guest check stored in the Guest Check file.
        sType = 1;
    }

	pTransOpen->usGuestNo = usGuestNo = (USHORT)UifRegTransOpen->ulAmountTransOpen;
    sStatus = TrnGetGC(usGuestNo, sType, GCF_REORDER);
    if (sStatus < 0) {
		/* --- Not Found -> Newcheck Transaction --- */
		REGDISPMSG      DispMsg = {0};

        if (sStatus != GCF_NOT_IN) {            /* Add TAR175812 Jun-27-2001 */
            usStatus = GusConvertError(sStatus);
            return((SHORT)usStatus);
        }
        if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT0)) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  MDC 12 Bit D setting prohibits New Check.");
            return(LDT_NTINFL_ADR);
        }
        if (TranModeQualPtr->usFromTo[0] || TranModeQualPtr->usFromTo[1]) {
            if (TranModeQualPtr->usFromTo[0] > usGuestNo || TranModeQualPtr->usFromTo[1] < usGuestNo) {
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier GC range prohibits GC # entered.");
                return(LDT_PROHBT_ADR);
            }
        }
        pTransOpen->usNoPerson = 1;
        if ((sStatus = ItemTransNoPerson(pTransOpen)) != ITM_SUCCESS) {
            return(sStatus);
        }
		pTransOpen->usTableNo = 0;
        if ((sStatus = ItemTransTableNo(pTransOpen)) != ITM_SUCCESS) {
            return(sStatus);
        }

		pTransOpen->uchSeatNo = 0;
        if (RflGetSystemType () == FLEX_POST_CHK && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
            if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT1)) {
                if ((sStatus = ItemTransSeatNo(pTransOpen, 0)) != ITM_SUCCESS) {
                    return(sStatus);
                }
            }
        }
        if ((sStatus = CliGusManAssignNo(usGuestNo)) != GCF_SUCCESS) {
            usStatus = GusConvertError(sStatus);
            return((SHORT)usStatus);
        }

        DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;
        DispMsg.uchMinorClass = CLASS_UINEWCHECK;
        RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);

		flDispRegDescrControl |= NEWCHK_CNTRL;                          /* compulsory control */ 
        flDispRegKeepControl |= NEWCHK_CNTRL;                           /* compulsory control */ 
        DispMsg.fbSaveControl = 0;                                      /* save for redisplay */
        DispWrite( &DispMsg );                                          /* display check total */

        ItemTransNCCommon(pTransOpen);
        return(ITM_SUCCESS);
    } else if (sStatus == 0) {
		/* --- No size GCF --- */
        ItemCommonCancelGC(WorkGCF);
        return (LDT_ILLEGLNO_ADR);
    } else {
		/* --- Found -> Reorder/Addcheck Transaction --- */
        /* check guest check team no, V3.3 */
        if ( !(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM)) {        /* not use team */

            /* V3.3 ICI */
            if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_CLOSE_ANY_CHECK) { /* not close any check */
                if (TranModeQualPtr->ulCashierID != WorkGCF->ulCashierID ) {    /* check cashier id */
                    ItemCommonCancelGC(WorkGCF);                        /* R3.0 */
                    return( LDT_NOUNMT_ADR );                           /* Cashier/Waiter # Unmatch */
                }
            }
        } else {                                                    /* use team */
            if (TranModeQualPtr->uchCasTeamNo != 0) {                       /* not head cashier */
                if (TranModeQualPtr->uchCasTeamNo != WorkGCF->uchTeamNo) {  /* team no unmatch */
                    ItemCommonCancelGC(WorkGCF);                    /* R3.0 */
                    return( LDT_NOUNMT_ADR );                       /* Cashier/Waiter # Unmatch */
                }
            }
        }
        if ((WorkGCF->fsGCFStatus & GCFQUAL_PVOID) == GCFQUAL_PVOID && (pWorkCur->fsCurStatus & CURQUAL_PVOID) != CURQUAL_PVOID) {
            ItemCommonCancelGC(WorkGCF);
            return(LDT_PROHBT_ADR);
        }
        if ((WorkGCF->fsGCFStatus & GCFQUAL_PVOID) != GCFQUAL_PVOID && (pWorkCur->fsCurStatus & CURQUAL_PVOID) == CURQUAL_PVOID) {
            ItemCommonCancelGC(WorkGCF);
            return(LDT_PROHBT_ADR);
        }

        if ((WorkGCF->fsGCFStatus & GCFQUAL_WIC) == GCFQUAL_WIC && (pWorkCur->fsCurStatus & CURQUAL_WIC) != CURQUAL_WIC) {
            ItemCommonCancelGC(WorkGCF);
            return(LDT_PROHBT_ADR);
        }
        if ((WorkGCF->fsGCFStatus & GCFQUAL_WIC) != GCFQUAL_WIC && (pWorkCur->fsCurStatus & CURQUAL_WIC) == CURQUAL_WIC) {
            ItemCommonCancelGC(WorkGCF);
            return(LDT_PROHBT_ADR);
        }

        if ( !(WorkGCF->fsGCFStatus & GCFQUAL_TRAINING)) {
            if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) ||
				(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
                ItemCommonCancelGC(WorkGCF);
                return(LDT_PROHBT_ADR);
            }
        } else {
            if ( !(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) &&
                !(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
                ItemCommonCancelGC(WorkGCF);
                return(LDT_PROHBT_ADR);
            }
        }    
        /*----- Seat Number Entry for SplitCheck,   R3.1 -----*/
        if (RflGetSystemType () == FLEX_POST_CHK
            && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
            && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {

            if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT1)) {
                if ((sStatus = ItemTransSeatNo(pTransOpen, 0)) != ITM_SUCCESS) {
                    ItemCommonCancelGC(WorkGCF);
                    return(sStatus);
                }
            }
        }
        pTransOpen->usGuestNo = WorkGCF->usGuestNo;
        pTransOpen->uchCdv = WorkGCF->uchCdv;

        return(ITM_SUCCESS);
    }
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransAddcheck( ITEMTRANSOPEN *ItemTransOpen )   
*                               
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    fill in the parameters for a transaction open request
*                   for an Add Check or Reorder Check or Get Check action.
*
*                   This should be not be used for Store Recall System Type.
==========================================================================*/

VOID    ItemTransAddcheck( ITEMTRANSOPEN *ItemTransOpen )   
{
	REGDISPMSG      DispMsg = {0};
	USHORT          usSystemType = RflGetSystemType ();

	// set up the transaction open data
    ItemTransOpen->uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen->uchMinorClass = CLASS_ADDCHECK;
    ItemTransOpen->lAmount = TranItemizersPtr->lMI;
    ItemTransOpen->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	ItemTransOpen->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    if ( usSystemType == FLEX_POST_CHK ) {                   /* postcheck */
        ItemTransOpen->fbStorageStatus = NOT_CONSOLI_STORAGE;       /* not store consoli storage */
    }                

    if ( usSystemType == FLEX_POST_CHK && ItemTransLocal.fbTransStatus & TROP_OPENPOSTCHECK ) {    /* transaction status */
        ItemTransOpen->uchMinorClass = CLASS_REORDER;               /* print PB */
        ItemTransOpen->fbStorageStatus = 0;                         /* store item/consoli storage */
    }

	// display a lead thru message indicating we are continuing a previously serviced transaction
    DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;   
    DispMsg.uchMinorClass = CLASS_UIADDCHECK1;   
    DispMsg.lAmount = ItemTransOpen->lAmount;                       /* reorder amount */
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);
    flDispRegDescrControl |= PAYMENT_CNTRL;                         /* compulsory control */ 
    flDispRegKeepControl |= PAYMENT_CNTRL;                          /* compulsory control */

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* tax exempt case */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt descriptor */ 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt descriptor */
    }
    /* R3.3 */
    if ( TranGCFQualPtr->fsGCFStatus & (GCFQUAL_PVOID | GCFQUAL_TRETURN) ) {  /* preselect void or return */
        flDispRegDescrControl |=  TVOID_CNTRL;
        flDispRegKeepControl |=  TVOID_CNTRL;
    }

    DispMsg.fbSaveControl = 0;                                      /* save for redisplay */
    DispWrite( &DispMsg );                                          /* display check total */

//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
//        ItemTransOpen->usTableNo = ItemCommonUniqueTransNo();
//    }
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACManDisp( ITEMAFFECTION *ItemAffection )   
*                               
*   Input:      ITEMAFFECTION   *ItemAffection
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:     
*
*   Description:    display of manual addcheck data
==========================================================================*/

static VOID    ItemTransACManDisp( DCURRENCY  lAmount )   
{
	REGDISPMSG  DispMsg = {0};

    flDispRegDescrControl |= PAYMENT_CNTRL;                         /* addcheck control */ 
    flDispRegKeepControl |= PAYMENT_CNTRL;                          /* addcheck control */ 

    DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;
    DispMsg.uchMinorClass = CLASS_UIADDCHECK1;
    DispMsg.lAmount = lAmount;                                      /* addcheck amount */
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);
    DispMsg.fbSaveControl = 0;                                      /* save for redisplay */

    flDispRegKeepControl &= ~VOID_CNTRL;                            /* descriptor */

    DispWrite( &DispMsg );                                          /* display check total */

    flDispRegDescrControl &= ~VOID_CNTRL;                           /* descriptor */
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMan( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    manual addcheck operation
==========================================================================*/
                                                    
SHORT   ItemTransACMan( UIFREGTRANSOPEN *UifRegTransOpen )   
{
    SHORT   sStatus;    
    ITEMTRANSOPEN   ItemTransOpen = {0};
	ITEMAFFECTION   ItemAffection = {0};

    if ( ( sStatus = ItemTransACManCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    if ( ( sStatus = ItemTransACKeyMan( &ItemTransOpen ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    if ( ( sStatus = ItemTransACManAff( UifRegTransOpen, &ItemAffection ) ) != ITM_SUCCESS ) { 
        return( sStatus );
    }
 
    if ( ( sStatus = ItemTransACSPVInt( UifRegTransOpen ) ) != ITM_SUCCESS ) {       /* supervisor intervention */
        return( sStatus );
    }

    if ( ( ItemTransLocal.fbTransStatus & TROP_DURINGADDCHECK ) == 0 ) {
        ItemTransLocal.fbTransStatus |= TROP_DURINGADDCHECK;        /* during manual addcheck */

        ItemTransNCPara();  

        if ( ItemTransOpen.uchLineNo != 0 ) {                       /* input by manual PB */
			TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();          /* get GCF qualifier */
            WorkGCF->uchSlipLine = ItemTransOpen.uchLineNo;         /* line No. */
        }

        ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                     /* print promotion header */
        ItemHeader( TYPE_STORAGE );                                 /* p-void, training header */

		ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
		ItemTransOpen.uchMinorClass = CLASS_ADDCHECK;
		ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
		//if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
		//    ItemTransOpen.usTableNo = ItemCommonUniqueTransNo();
		//}
        TrnOpen( &ItemTransOpen );

        /*--- LCD Initialize,  R3.0 ---*/
        ItemCommonMldInit();

        /* send to enhanced kds, 2172 */
        ItemSendKds(&ItemTransOpen, 0);

        /*--- Display GCF/Order#, R3.0 ---*/
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    }

    TrnItem( &ItemAffection );                                      /* transaction module i/F */
    MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);                /* R3.0 */
    MldScrollFileWrite(&ItemAffection);

	ItemTransACManDisp( ItemAffection.lAmount );                    /* display of manual addcheck */

    ItemTransLocal.lManualAmt += ItemAffection.lAmount;             /* save manual PB amount */

    return( ITM_SUCCESS );
}

  

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACManCheck( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check manual addcheck operation
==========================================================================*/

SHORT   ItemTransACManCheck( VOID )   
{
    /* prohibit guest check open, V3.3 */
    if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_GUEST_CHECK_OPEN) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier setting CAS_NOT_GUEST_CHECK_OPEN prohibits.");
        return(LDT_PROHBT_ADR);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_US) {          /* V3.3 */
        if ( ItemModLocalPtr->fsTaxable & ~MOD_USTAXMOD ) {             
            return( LDT_SEQERR_ADR );
        }
    } else {                                            /* Canada/Int'l, V3.3 */
        if ( ItemModLocalPtr->fsTaxable ) {
			/* sequence error if tax modifier key has been used */
            return( LDT_SEQERR_ADR );
        }
    }
    
    if ( TranCurQualPtr->auchTotalStatus[0] != 0 ) {
		// Total key has been pressed so sequence error.
        return( LDT_SEQERR_ADR );
    }

    if ( TranGCFQualPtr->usGuestNo ) {                         /* check multi check payment */
		// Already have a guest check started so sequence error.
        return( LDT_SEQERR_ADR );
    }

    return( ITM_SUCCESS );                                          
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACKeyMan( ItemTransOpen *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of manual addcheck operation
==========================================================================*/

SHORT   ItemTransACKeyMan( ITEMTRANSOPEN *ItemTransOpen )
{
    if ( ItemTransLocal.fbTransStatus & TROP_DURINGADDCHECK ) {     /* during addcheck */
        return( ITM_SUCCESS );
    }
    
    return( ItemTransROKeyPB( ItemTransOpen ) ) ;                   /* check slip paper */
}


/*==========================================================================
**   Synopsis:  SHORT   ItemTransACManAff( UIFREGTRANSOPEN *UifRegTransOpen,
*                           ITEMAFFECTION *ItemAffection )   
*                               
*   Input:      UifRegTransOpen *UifRegTransOpen,   ITEMAFFECTION   *ItemAffection
*   Output:     ItemAffection   *ItemAffection        
*   InOut:      none                            
*
*   Return:     
*
*   Description:    generate affection parameters for Manual Add Check key from MDC
*                   and modifier key. check that there is sufficient transaction file
*                   storage space as well.
==========================================================================*/

SHORT   ItemTransACManAff( UIFREGTRANSOPEN *UifRegTransOpen, ITEMAFFECTION *ItemAffection )   
{           
    SHORT     sStatus, sPvoid = 1;
    PARAMDC   WorkMDC;

    ItemAffection->uchMajorClass = CLASS_ITEMAFFECTION;
    ItemAffection->uchMinorClass = CLASS_MANADDCHECKTOTAL;

	// There are three Manual Add Check keys available each of which has an MDC address
	// #1 -> MDC 49, #2 -> MDC 50, #3 -> MDC 51.
    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_ADDCHK1_ADR + UifRegTransOpen->uchMinorClass - CLASS_UIADDCHECK1;
    CliParaRead( &WorkMDC );                                        /* get target MDC */
    ItemAffection->uchAffecMDCstatus = WorkMDC.uchMDCData;          /* addcheck MDC */

    if ( UifRegTransOpen->uchMinorClass == CLASS_UIADDCHECK2 ) {
		// if this is Manual Add Check key #2 with an even numbered MDC address then we
		// will shift the nibble with the actual data down to translate the bit settings to
		// where the odd numbered MDC address would store them to make processing easier.
		//  Bit D - require or not Supervisor Intervention
		//  Bit C - affect or not taxable itemizer #1
		//  Bit B - affect or not taxable itemizer #2
		//  Bit A - affect or not taxable itemizer #3
        ItemAffection->uchAffecMDCstatus >>= 4;                     /* shift the MDC bits if even numbered affection */
    }                                                               

    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE1 ) == MOD_TAXABLE1 ) {
        ItemAffection->fbModifier |= TAX_MODIFIER1;                 /* taxable modifier #1 */
    }                    

    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE2 ) == MOD_TAXABLE2 ) {
        ItemAffection->fbModifier |= TAX_MODIFIER2;                 /* taxable modifier #2 */
    }                    

    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE3 ) == MOD_TAXABLE3 ) {
        ItemAffection->fbModifier |= TAX_MODIFIER3;                 /* taxable modifier #3 */
    }                    

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {                    /* preselect void */
        sPvoid = -1;    
    }

    if ( ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) == VOID_MODIFIER ) {
        ItemAffection->lAmount = UifRegTransOpen->ulAmountTransOpen * -1L * sPvoid;
    } else {
        ItemAffection->lAmount = ( LONG )UifRegTransOpen->ulAmountTransOpen * sPvoid;
    }
                                                                    
    ItemAffection->sItemCounter = 1 * sPvoid;                       /* addcheck counter */

    ItemAffection->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    ItemAffection->fbStorageStatus = NOT_CONSOLI_STORAGE; 

    ItemAffection->uchAddNum = ( UCHAR )( UifRegTransOpen->uchMinorClass - CLASS_UIADDCHECK1 ); 
                                                                                                                                            
    if ( ( sStatus = ItemCommonCheckSize( ItemAffection, 0) ) < 0 ) {      /* check storage size */
        TrnGetGCFQualPtr()->fsGCFStatus |= GCFQUAL_BUFFER_FULL;            /* storage full */
        return( LDT_TAKETL_ADR);                                
    } else {
        return( ITM_SUCCESS );
    }
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACSPVInt( UIFREGTRANSOPEN *UifRegTransOpen )   
*                               
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    supervisor intervention
==========================================================================*/
        
SHORT   ItemTransACSPVInt( UIFREGTRANSOPEN *UifRegTransOpen )   
{
    if ( ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) == 0 ) {
        switch( UifRegTransOpen->uchMinorClass ) {
        case    CLASS_UIADDCHECK1:  
            if ( CliParaMDCCheck( MDC_ADDCHK1_ADR, ODD_MDC_BIT0 ) == 0 ) {
                return( ITM_SUCCESS );
            }
            break;

        case    CLASS_UIADDCHECK2:  
            if ( CliParaMDCCheck( MDC_ADDCHK2_ADR, EVEN_MDC_BIT0 ) == 0 ) {
               return( ITM_SUCCESS );
            }
            break;

        case    CLASS_UIADDCHECK3:  
            if ( CliParaMDCCheck( MDC_ADDCHK3_ADR, ODD_MDC_BIT0 ) == 0 ) {
                return( ITM_SUCCESS );
            }
            break;
            
        default:
            break;
        }
    } else {                                                        /* void */    
        switch( UifRegTransOpen->uchMinorClass ) {
        case    CLASS_UIADDCHECK1:  
            if ( CliParaMDCCheck( MDC_ADDCHK1_ADR, ODD_MDC_BIT0 ) == 0 
                && CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0 ) == 0 ) {
                return( ITM_SUCCESS );
            }
            break;

        case    CLASS_UIADDCHECK2:  
            if ( CliParaMDCCheck( MDC_ADDCHK2_ADR, EVEN_MDC_BIT0 ) == 0 
                && CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0 ) == 0 ) {
                return( ITM_SUCCESS );
            }
            break;

        case    CLASS_UIADDCHECK3:  
            if ( CliParaMDCCheck( MDC_ADDCHK3_ADR, ODD_MDC_BIT0 ) == 0
                && CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0 ) == 0 ) {
                return( ITM_SUCCESS );
            }
            break;
            
        default:
            break;
        }
    }

    if ( ItemCommonGetStatus( COMMON_SPVINT ) == ITM_SUCCESS ) {          /* already SPV int. */
        return( ITM_SUCCESS );
    }

    if ( ItemSPVInt(LDT_SUPINTR_ADR) == ITM_SUCCESS ) {                        /* success */
        return( ITM_SUCCESS );

    } else {                                                    /* cancel */
        return( UIF_CANCEL );
    }
}

 



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACNorAff( VOID )   
*                               
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate affection data
==========================================================================*/

VOID    ItemTransACNorAff( VOID )   
{   
	ITEMAFFECTION   ItemAffection = {0};
    SHORT   sPvoid = 1;        

    ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;
    ItemAffection.uchMinorClass = CLASS_ADDCHECKTOTAL;
    ItemAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    ItemAffection.fbStorageStatus = NOT_CONSOLI_STORAGE; 

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {                    /* preselect void */
        sPvoid = -1;    
    }

    ItemAffection.sItemCounter = 0;                                 /* addcheck counter */
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_SERVICE1 ) {             
        ItemAffection.sItemCounter = 1 * sPvoid;                    /* addcheck counter */
    }                                           
    ItemAffection.lAmount = TranItemizersPtr->lService[0];          /* addcheck total */
    ItemAffection.uchAddNum = 0;                                    /* addcheck offset */
    TrnItem( &ItemAffection );                                      /* transaction module i/F */
    MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);                /* R3.0 */
	// Commented out the following MldScrollFileWrite() call
	// because it is causing problems with Store/Recall in
	// condiment edit mode.  The recalled guest check is
	// pulled into the MLD subsystem as part of the redisplay
	// of the transaction and these kinds of non-displayable
	// items are causing the item number counts to be off.
	// Richard Chambers, Dec-20-2007
//    MldScrollFileWrite(&ItemAffection);

    ItemAffection.sItemCounter = 0;                                 /* addcheck counter */
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_SERVICE2 ) {             
        ItemAffection.sItemCounter = 1 * sPvoid;                    /* addcheck counter */
    }                                               
    ItemAffection.lAmount = TranItemizersPtr->lService[1];          /* addcheck total */
    ItemAffection.uchAddNum = 1;                                    /* addcheck offset */
    TrnItem( &ItemAffection );                                      /* transaction module i/F */
    MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);                /* R3.0 */
//    MldScrollFileWrite(&ItemAffection);

    ItemAffection.sItemCounter = 0;                                 /* addcheck counter */
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_SERVICE3 ) {             
        ItemAffection.sItemCounter = 1 * sPvoid;                    /* addcheck counter */
    }                                           
    ItemAffection.lAmount = TranItemizersPtr->lService[2];          /* addcheck total */
    ItemAffection.uchAddNum = 2;                                    /* addcheck offset */
    TrnItem( &ItemAffection );                                      /* transaction module i/F */
    MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);                /* R3.0 */
//    MldScrollFileWrite(&ItemAffection);
}


/****** End of Source ******/