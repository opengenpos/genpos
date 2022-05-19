/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP2.C (reorder)                                       
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
:   ItemTransOpenRO()       ;   reorder
:   ItemTransROCheck()      ;   check reorder operation
:   ItemTransROKey()        ;   reorder key sequence
:   ItemTransROKeyPB()      ;   manual PB key sequence
:   ItemTransROSPVInt()     ;   supervisor intervention
:   ItemTransReorder()      ;   generate transaction open data
:
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name      : Description
:  2/14/95 : 03.00.00 :   hkato     : R3.0
: 11/17/95 : 03.01.00 :   hkato     : R3.1
:  8/19/98 : 03.03.00 :  hrkato     : V3.3 (VAT/Service)
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
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
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
**   Synopsis:  SHORT   ItemTransROCheck( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check reorder operation
==========================================================================*/

static SHORT   ItemTransROCheck( UIFREGTRANSOPEN   *UifRegTransOpen )   
{
    /* prohibit guest check open, V3.3 */
    if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_GUEST_CHECK_OPEN) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier setting CAS_NOT_GUEST_CHECK_OPEN prohibits.");
        return(LDT_PROHBT_ADR);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA || ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
        if ( ItemModLocalPtr->fsTaxable ) {
			/* sequence error if tax modifier key has been used */
            return( LDT_SEQERR_ADR );
        }
    }

    if ( ( UifRegTransOpen->fbInputStatus & INPUT_0_ONLY ) == 0 &&  UifRegTransOpen->ulAmountTransOpen == 0L ) {     /* auto reorder */
        if ( ItemModLocalPtr->fsTaxable ) {
			/* sequence error if tax modifier key has been used */
            return( LDT_SEQERR_ADR );
        }

        if ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) {
			/* sequence error if Void key has been used */
            return( LDT_SEQERR_ADR );
        }
    } else {
        if ( ItemModLocalPtr->fsTaxable & ~MOD_USTAXMOD ) {                  /* tax modifier */
            return( LDT_SEQERR_ADR );
        }
    }

    if ( RflGetSystemType () == FLEX_STORE_RECALL ) {               /* R3.0 */
		FDTPARA     WorkFDT = { 0 };

		FDTParameter(&WorkFDT);                                /* Get FDT Parameter */
        if ( WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_2 ) {          /* Delivery Terminal */
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: FLEX_STORE_RECALL - Delivery Terminal not allowed.");
            return( LDT_SEQERR_ADR );
        }
    }

    return( ITM_SUCCESS );                                          
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransROKey( ITEMTRANSOPEN   *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of reorder operation
==========================================================================*/

static SHORT   ItemTransROKey( ITEMTRANSOPEN   *ItemTransOpen )   
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    ULONG           ulGuestNo;
    SHORT           sStatus, sType;
    USHORT          usStatus;
	USHORT          usSystemType = RflGetSystemType ();

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
    if ( usSystemType == FLEX_STORE_RECALL || usSystemType == FLEX_POST_CHK ) {
		// indicate that we need the consolidated transaction data storage as well.
		// Store/Recall and Post Check both store additional memory resident guest check data
		// in a guest check stored in the Guest Check file.
        sType = 1;
    }

    sStatus = TrnGetGC( ( USHORT )ulGuestNo, sType, GCF_REORDER );  /* R3.0 */
    if ( sStatus < 0 ) {                                            /* error */
        usStatus = GusConvertError( sStatus );
        return( ( SHORT )usStatus );            
    } else if ( sStatus == 0 ) {                                    /* no size GCF */
        ItemCommonCancelGC(WorkGCF);                                /* R3.0 */
        return ( LDT_ILLEGLNO_ADR );                                /* Illegal Guest Check # */
    }

    if (WorkGCF->fsGCFStatus & GCFQUAL_BUFFER_FULL) {               /* buffer full, R3.0 */
        ItemCommonCancelGC(WorkGCF);                                /* R3.0 */
        return( LDT_PROHBT_ADR );
    }

    if ( usSystemType != FLEX_STORE_RECALL ) {               /* GCF system */
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
/*** delete V3.3
        if ( WorkMode.usWaiterID ) {                                / waiter operation /
            if ( WorkGCF->fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER     / cashier GCF /
                || WorkMode.usWaiterID != WorkGCF->usWaiterID ) {   / check waiter id /
                ItemCommonCancelGC(WorkGCF);                        / R3.0 /
                return( LDT_NOUNMT_ADR );                           / Cashier/Waiter # Unmatch /
            }
                                                                    
        } else {                                                    / cashier operation /
            if ( !( WorkGCF->fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER ) / cashier GCF /
                || WorkMode.usCashierID != WorkGCF->usCashierID ) { / check cashier id /
                ItemCommonCancelGC(WorkGCF);                        / R3.0 /
                return( LDT_NOUNMT_ADR );                           / Cashier/Waiter # Unmatch /
            }
        }
***/
    } else {                                                        /* R3.0 */
        if (TranModeQualPtr->ulCashierID != WorkGCF->ulCashierID && (WorkGCF->fsGCFStatus & GCFQUAL_DRIVE_THROUGH) == 0) {
            ItemCommonCancelGC(WorkGCF);                            /* R3.0 */
            return( LDT_PROHBT_ADR );
        }
    }

	if ( WorkGCF->fsGCFStatus & GCFQUAL_PVOID && ( TranCurQualPtr->fsCurStatus & CURQUAL_PVOID ) != CURQUAL_PVOID ) { /* p-void GCF */
		ItemCommonCancelGC(WorkGCF);                                /* R3.0 */
		return( LDT_PROHBT_ADR );        
	}

	if (WorkGCF->fsGCFStatus & GCFQUAL_WIC && (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) != CURQUAL_WIC) {
		ItemCommonCancelGC(WorkGCF);
		return(LDT_PROHBT_ADR);
	}

    if ( !( WorkGCF->fsGCFStatus & GCFQUAL_TRAINING ) ) {           /* normal */    
        if ( (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING )
            || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
            ItemCommonCancelGC(WorkGCF);                            /* R3.0 */
            return( LDT_PROHBT_ADR );        
        }
    } else {                                                        /* training */
        if ( !(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING )
            && !(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
            ItemCommonCancelGC(WorkGCF);                            /* R3.0 */
            return( LDT_PROHBT_ADR );        
        }
    }    
/* ### K.Iwata ??? #ifdef  GUEST_CHECK_MODEL  */
    /*----- Seat Number Entry for SplitCheck,   R3.1 -----*/
    if (usSystemType == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {

        if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT1)) {
            if ((sStatus = ItemTransSeatNo(ItemTransOpen, 0)) != ITM_SUCCESS) {
                ItemCommonCancelGC(WorkGCF);
                return(sStatus);
            }
        }
    }
/* ### K.Iwata ??? #endif */
    ItemTransOpen->usGuestNo = WorkGCF->usGuestNo;                  /* save GCF No */
    ItemTransOpen->uchCdv = WorkGCF->uchCdv;                        /* save CDV */
    return( ITM_SUCCESS );
}




/*==========================================================================
**   Synopsis:  SHORT   ItemTransROSPVInt( UIFREGTRANSOPEN *UifRegTransOpen )   
*                               
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none
*   InOut:      none
*
*   Return:     
*                                                               
*   Description:    supervisor intervention
==========================================================================*/
        
static SHORT   ItemTransROSPVInt( UIFREGTRANSOPEN *UifRegTransOpen )   
{                                                                   
    if ( ( UifRegTransOpen->fbInputStatus & INPUT_0_ONLY ) == 0 &&  UifRegTransOpen->ulAmountTransOpen == 0L ) {     /* auto */
        return( ITM_SUCCESS );
    }
                                                                    
    if ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) {   /* negative Previous Balance */
        if ( CliParaMDCCheck( MDC_PB_ADR, EVEN_MDC_BIT0 ) == 0
            && CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT0 ) == 0 ) {
            return( ITM_SUCCESS );
        }
    } else {                                                        /* normal Previous Balance */
        if ( CliParaMDCCheck( MDC_PB_ADR, EVEN_MDC_BIT0 ) == 0 ) { 
            return( ITM_SUCCESS );
        }
    }
                                                
    if ( ItemSPVInt(LDT_SUPINTR_ADR) == ITM_SUCCESS ) {             /* success */
        return( ITM_SUCCESS );
    } else {                                                        /* cancel */
        return( UIF_CANCEL );
    }
}


/*==========================================================================
**   Synopsis:  VOID    ItemTransReorder( UIFREGTGRANSOPEN *UifRegTransOpen,
                        ITEMTRANSOPEN *ItemTransOpen )   
*                               
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen, ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction open data
==========================================================================*/

static VOID    ItemTransReorder( UIFREGTRANSOPEN *UifRegTransOpen, ITEMTRANSOPEN *ItemTransOpen )   
{
    PARAMDC         WorkMDC;
	REGDISPMSG      DispMsg = {0};
    SHORT           sPvoid = 1;        
	USHORT          usSystemType = RflGetSystemType ();
                    
    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE1 ) == MOD_TAXABLE1 ) {
        ItemTransOpen->fbModifier |= TAX_MODIFIER1;                 /* taxable modifier #1 */
    }                    

    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE2 ) == MOD_TAXABLE2 ) {
        ItemTransOpen->fbModifier |= TAX_MODIFIER2;                 /* taxable modifier #2 */
    }                    

    if ( ( ItemModLocalPtr->fsTaxable & MOD_TAXABLE3 ) == MOD_TAXABLE3 ) {
        ItemTransOpen->fbModifier |= TAX_MODIFIER3;                 /* taxable modifier #3 */
    }                    

    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_PB_ADR;
    CliParaRead( &WorkMDC );                                        /* get target MDC */

    WorkMDC.uchMDCData >>= 4;                                       /* shift */
    ItemTransOpen->uchStatus = WorkMDC.uchMDCData;                  /* PB MDC */

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {                    /* preselect void */
        sPvoid = -1;    
    }

    ItemTransOpen->uchMajorClass = CLASS_ITEMTRANSOPEN;

    if ( ( UifRegTransOpen->fbInputStatus & INPUT_0_ONLY ) == 0 && UifRegTransOpen->ulAmountTransOpen == 0L ) {
        ItemTransOpen->uchMinorClass = CLASS_REORDER;               /* auto reorder */
        ItemTransOpen->lAmount = TranItemizersPtr->lMI;
    } else {
        ItemTransOpen->uchMinorClass = CLASS_MANUALPB;              /* manual Previous Balance */
        ItemTransOpen->lAmount = ( LONG )UifRegTransOpen->ulAmountTransOpen * sPvoid;
        if ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) {   /* negative entry */
            ItemTransOpen->lAmount *= -1L;
        }
        ItemTransLocal.lManualAmt = ItemTransOpen->lAmount;         /* save manual Previous Balance amount */
    }

    ItemTransOpen->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	ItemTransOpen->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    if ( usSystemType == FLEX_POST_CHK || usSystemType == FLEX_STORE_RECALL ) {             /* R3.0 */
        ItemTransOpen->fbStorageStatus = NOT_CONSOLI_STORAGE;       /* not store consoli storage */
    }  

    if ( usSystemType == FLEX_POST_CHK && ItemTransLocal.fbTransStatus & TROP_OPENPOSTCHECK ) {       /* transaction status */
        ItemTransOpen->fbStorageStatus = 0;                         /* store item/consoli storage */
    }

    DispMsg.uchMajorClass = UifRegTransOpen->uchMajorClass;
    DispMsg.uchMinorClass = CLASS_UIREORDER;
    DispMsg.lAmount = ItemTransOpen->lAmount;                       /* reorder amount */
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);
    
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* tax exempt case */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt descriptor */ 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt descriptor */
    }

    flDispRegDescrControl |= PBAL_CNTRL;                            /* compulsory control */ 
    flDispRegKeepControl |= PBAL_CNTRL;                             /* compulsory control */
    DispMsg.fbSaveControl = 0;                                      /* save for redisplay */
    flDispRegKeepControl &= ~VOID_CNTRL;                            /* descriptor */

    DispWrite( &DispMsg );                                          /* display check total */

    flDispRegDescrControl &= ~VOID_CNTRL;                           /* descriptor */

//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
//        ItemTransOpen->usTableNo = ItemCommonUniqueTransNo();
//    }
}


/*==========================================================================
**   Synopsis:  SHORT   ItemTransROKeyPB( ItemTransOpen *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of manual PB operation which is used with
*                   Pre-Check type systems to allow for adding to a slip by
*                   starting with a Previous Balance for the slip.
*                   Some sites such as diners such as Le Peeps use slips
*                   with printers with slip print station and the waitress
*                   will have an apron with a pocket full of slips, one for
*                   each table and the guest check information is kept by
*                   printing on the slip rather than electronically.
==========================================================================*/

SHORT   ItemTransROKeyPB( ITEMTRANSOPEN *ItemTransOpen )
{
//    UCHAR   fbPrtStatus;
    PARASLIPFEEDCTL WorkSlip;
	REGDISPMSG      DispMsg = {0};
    UIFDIADATA      WorkUI = {0};
    USHORT          usSystemType = RflGetSystemType ();

    if ( usSystemType == FLEX_POST_CHK || usSystemType == FLEX_STORE_RECALL ) {             /* R3.0 */
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  FLEX_POST_CHK and FLEX_STORE_RECALL prohibits Previous Balance.");
        return( LDT_PROHBT_ADR );        
    }

  //  if ( PmgGetStatus( PMG_PRT_SLIP, &fbPrtStatus ) == PMG_SUCCESS ) {  ALWAYS RETURNS SUCCESS JHHJ
        DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
        DispMsg.uchMinorClass = CLASS_REGDISP_LINENO;               /* line No */
        RflGetLead (DispMsg.aszMnemonic, LDT_LINE_ADR);

        flDispRegDescrControl |= COMPLSRY_CNTRL;                    /* compulsory control */ 
        flDispRegKeepControl |= COMPLSRY_CNTRL;                     /* compulsory control */ 
        DispMsg.fbSaveControl = 4;                                  /* save for redisplay */
        DispWrite( &DispMsg );                                      /* display check total */

        flDispRegDescrControl &= ~COMPLSRY_CNTRL;                   /* compulsory control */ 
        flDispRegKeepControl &= ~COMPLSRY_CNTRL;                    /* compulsory control */ 
                                     
        if ( UifDiaLineNo( &WorkUI ) != UIF_SUCCESS ) {             /* Cancel */
            ItemTransDispDef();                                     /* default display */
            return( UIF_CANCEL );
        }

        WorkSlip.uchMajorClass = CLASS_PARASLIPFEEDCTL;             /* SLIP PRINTER FEED CONTROL */      
        WorkSlip.uchAddress = SLIP_MAXLINE_ADR;                     /* Max Line # for Slip Printer */ 
        CliParaRead( &WorkSlip );
        if ( ( UCHAR )( WorkSlip.uchFeedCtl - 2 ) < ( UCHAR )WorkUI.ulData ) {
            return( LDT_KEYOVER_ADR );                              /* Over Limitation */
        }       

        ItemTransOpen->uchLineNo = ( UCHAR )WorkUI.ulData;          /* line No */
//}

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransOpenRO( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*                   
*   Description:    reorder operation
==========================================================================*/
        
SHORT   ItemTransOpenRO( UIFREGTRANSOPEN    *UifRegTransOpen )   
{
    TRANGCFQUAL      *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL      *pWorkCur = TrnGetCurQualPtr();
	ITEMTRANSOPEN    ItemTransOpen = {0};
    SHORT            sStatus;    

    if ( ( sStatus = ItemTransROCheck( UifRegTransOpen ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    if ( ( UifRegTransOpen->fbInputStatus & INPUT_0_ONLY ) == 0 && UifRegTransOpen->ulAmountTransOpen == 0L ) {     /* auto */
        if ( ( sStatus = ItemTransROKey( &ItemTransOpen ) ) != ITM_SUCCESS ) {
            return( sStatus );
        }
    } else {                                                        /* manual PB */
        if ( ( sStatus = ItemTransROKeyPB( &ItemTransOpen ) ) != ITM_SUCCESS ) {
            return( sStatus );
        }
    }

    if ( ( sStatus = ItemTransROSPVInt( UifRegTransOpen ) ) != ITM_SUCCESS ) { 
        return( sStatus );
    }

    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );           /* inhibit post receipt, R3.0 */

    ItemTransNCPara();                                              /* set current qualifier */

    if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER 
        || ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER
        || ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            && TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) ) {

        if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            || TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) {                        /* print transaction header */

            ItemTransLocal.fbTransStatus |= TROP_OPENPOSTCHECK;     /* transaction status */
        }                

        if ( ItemTransOpen.uchLineNo != 0 ) {                       /* input by manual PB */
            WorkGCF->uchSlipLine = ItemTransOpen.uchLineNo;         /* line No. */
        }

        ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                     /* print promotion header */
        ItemHeader( TYPE_STORAGE );                                 /* p-void, training header 8/24/93 */
    } else {                                                        /* postcheck, store/recall */
        ItemPrintStart( TYPE_POSTCHECK );                           /* start print service */
        ItemHeader( TYPE_CANCEL );                                  /* p-void, training header 8/24/93 */
    }

    ItemTransReorder( UifRegTransOpen, &ItemTransOpen );            /* generate transaction open data */
    TrnOpen( &ItemTransOpen );                                      /* transaction module i/F */

    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransOpen, 0);

    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER ||
		(pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER) {
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else if (CliParaMDCCheckField( MDC_DISPLAY_ADR, MDC_PARAM_BIT_A ) &&
		(pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
		/* R3.3 */
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else if ( CliParaMDCCheckField( MDC_DISPLAY_ADR, MDC_PARAM_BIT_A ) &&
		(pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
		/* V3.3 */
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else {
        /*--- Display GCF/Order#, R3.1 ---*/
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);    /* display reorder if no storage */
        MldScrollFileWrite(&ItemTransOpen);
    }

	MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

/* ### K.Iwata ??? #ifdef  STORE_RECALL_MODEL */

	ItemTransACStoOrderCounterOpen(WorkGCF->usGuestNo);      /* R3.0 */
/* ### K.Iwata ??? #endif */
    ItemTransLocal.sPrePigRuleCo = WorkGCF->sPigRuleCo;             /* Add 10-21-98 */
    pWorkCur->fsCurStatus |= CURQUAL_REORDER;                       /* reorder operation */

    ItemSalesGetLocalPointer()->fbSalesStatus |= SALES_ITEMIZE;     /* itemize state */
    ItemSalesCalcInitCom();                                         /* ppi initialize r3.1 2172 */

    ItemModGetLocalPtr()->fsTaxable = 0;                            /* taxable status */

    UieModeChange(UIE_DISABLE);                                     /* mode unchangable */

    return( ITM_SUCCESS );
}

/****** End of Source ******/
