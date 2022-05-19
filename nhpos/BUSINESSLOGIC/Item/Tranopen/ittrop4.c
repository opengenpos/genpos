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
:   Program Name   : ITTROP4.C (multi check payment/recall)                                       
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
:   ItemTransACMul()            ;   multi check payment
:   ItemTransACMulCheck()       ;   check multi check payment
:   ItemTransACMulPreFin()      ;   finalize previous transaction
:   ItemTransACMulPreFin1()     ;   check next GCF
:   ItemTransACMulPreFin2()     ;   generate previous GCF data
:   ItemTransACMulPreFin3()     ;   generate previous GCF print data
:                       
:   ItemTransACSto()            ;   recall          deleted 
:   ItemTransACStoCheck()       ;   check recall    deleted
:   ItemTransACStoGet()         ;   get target GCF  deleted 
:   ItemTransACStoRec()         ;   generate transaction open data  deleted    
:
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name     : Description
: 95/1/12  :          :   hkato    : R3.0
: 98/7/31  : 03.03.00 :   hrkato   : V3.3 (Multi Check & Split Check)
: 99/8/11  : 03.05.00 :   K.Iwata  : R3.5 (remove WAITER_MODEL)
: 99/8/13  : 03.05.00 :   K.Iwata  : R3.5 (merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL)
:
:** NCR2171 **
: 99/8/26  : 01.00.00 :   M.Teraki : initial (for 2171)
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
#include    "csstbgcf.h"
#include    "csgcs.h"
#include    "csttl.h"
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
#include    "fdt.h"
#include	"trans.h"
#include	"prtprty.h"
#include	"cscas.h"


/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMul(UIFREGTRANSOPEN *pOpen)
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    multi check payment operation
==========================================================================*/
        
SHORT   ItemTransACMul(UIFREGTRANSOPEN *pOpen)
{
    TRANCURQUAL      *pWorkCur = TrnGetCurQualPtr();
    ITEMCOMMONLOCAL  *pItemCommonL = ItemCommonGetLocalPointer();
	ITEMTRANSOPEN    ItemTransOpen = {0};
    SHORT            sStatus;    

	if ( RflGetSystemType () == FLEX_STORE_RECALL ) {
		/* store/recall system does not support multi check payment */
        return( LDT_PROHBT_ADR );
    }

    if ( ( sStatus = ItemTransACMulCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    if ((sStatus = ItemTransACMulPreFin(pOpen ,&ItemTransOpen)) != ITM_SUCCESS) {
        return(sStatus);
    }
    
    ItemTransLocal.fbTransStatus &= ~TROP_OPENPOSTCHECK;    /* reset transaction status */
    if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER ) {
        ItemPrintStart( TYPE_STORAGE );                             /* start print service */
    } else if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
		//UCHAR    fbPrtStatus;

        //if ( PmgGetStatus( PMG_PRT_SLIP, &fbPrtStatus ) == PMG_SUCCESS ) {  THIS ALWAYS RETURNS SUCCESS JHHJ
            UieErrorMsg( LDT_PAPERCHG_ADR, UIE_WITHOUT );           /* Slip Paper Change */
        //}

        if ( CliParaMDCCheck( MDC_SLIP_ADR, EVEN_MDC_BIT0 ) ) { 
            pWorkCur->fbCompPrint |= CURQUAL_COMP_S;                  /* compulsory slip */
        }                            
        ItemPrintStart( TYPE_STORAGE );                             /* start print service */
    } else {                                                        /* postcheck */
        if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK && TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) {                      /* print transaction header */
            ItemTransLocal.fbTransStatus |= TROP_OPENPOSTCHECK;     /* set transaction status */
            ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                 /* print promotion header */
            ItemHeader( TYPE_STORAGE );                             /* p-void, training header */
        } else {
            ItemPrintStart( TYPE_POSTCHECK );                       /* start print service */
        }
    }
                                                                                                            
    if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER || ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        ItemHeader( TYPE_STORAGE );                                 /* p-void, training header */
    }

    ItemTransACMulti( &ItemTransOpen );                             /* generate transaction open data */
    TrnOpen( &ItemTransOpen );                                      /* transaction module i/F */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransOpen, 0);

    /*--- Display GCF/Order#, R3.0 ---*/
    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER || (pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER) {
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else if (CliParaMDCCheck( MDC_DISPLAY_ADR, ODD_MDC_BIT3 ) ) { 
		UCHAR   uchMinor = ItemTransOpen.uchMinorClass;
        ItemTransOpen.uchMinorClass = CLASS_REORDER;               /* for display */
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
        ItemTransOpen.uchMinorClass = uchMinor;
    } else {
        /*--- Display GCF/Order#, R3.1 ---*/
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);    /* display reorder if no storage */
        MldScrollFileWrite(&ItemTransOpen);
    }

    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI + ItemTransLocalPtr->lWorkMI);  /* add multiple check payment amount */

    ItemTransACNorAff();                                              /* generate affection data */

    /* initialize auto charge tips of previous check, V3.3 */
    memset( &(pWorkCur->auchTotalStatus[0]), 0, sizeof(pWorkCur->auchTotalStatus) );  // clear total key data
    if ((pItemCommonL->ItemDisc.uchMajorClass == CLASS_ITEMDISC ) &&
        (( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP ) ||
         ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP2 ) ||
         ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP3 )) ){

        memset(&pItemCommonL->ItemDisc, 0 , sizeof(ITEMDISC));
        pItemCommonL->usDiscBuffSize = 0;
        pItemCommonL->lChargeTip = 0L;
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMulCheck( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check multi check payment operation,          R3.1
==========================================================================*/

SHORT   ItemTransACMulCheck( VOID )   
{                      
    SHORT         sStatus;
                                                                        
    if ( ItemModLocalPtr->fsTaxable ) {   
		/* sequence error if tax modifier key has been used */
        return( LDT_SEQERR_ADR );
    }
    if ( ItemTransLocal.uchCheckCounter + 1 >= STD_MAX_MULTI_CHECK ) {   /* over limit */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

	if ( TranCurQualPtr->auchTotalStatus[0] == CLASS_UITOTAL1 ) {      /* is this subtotal key */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

	if ( ItemTransLocal.fbTransStatus & TROP_DURINGADDCHECK ) {     /* during manual addcheck */
        return( LDT_PROHBT_ADR );                                   /* Prohibit Operation */
    }
/* --- Split & Multi Check, V3.3 --- 
    if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        return(LDT_SEQERR_ADR);
    }
*/
    if (TranCurQualPtr->uchSplit != 0) {
        return(LDT_SEQERR_ADR);
    }

    /*---- guest check close restriction, V3.3 ----*/
    if ( ( sStatus = ItemCommonCheckCloseCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    return( ITM_SUCCESS );                                          
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMulPreFin(UIFREGTRANSOPEN *pOpen, ItemTransOpen *ItemTransOpen)
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    finalize previous transaction(GCF)
==========================================================================*/

SHORT   ItemTransACMulPreFin(UIFREGTRANSOPEN *pOpen, ITEMTRANSOPEN *ItemTransOpen)
{
//    USHORT          usReadLen;
    TRANINFORMATION *TrnInfo = TrnGetTranInformationPointer();
	ULONG			ulActualBytesRead;
    ULONG           ulRWOffset, ulSize;
	ULONG			ulIndexFSize;
    ULONG           ulGuestNo;
	union {
		USHORT         usVli;
		TRANGCFQUAL    gcfQual;
		TRANITEMIZERS  tItemizers;
	} gcfti;
    SHORT           sStatus;
    USHORT          usStatus;
	USHORT			usSize1;
	UCHAR			uchConsIdBuffer[MAX_CONS_IDX_FILE_SIZE];
	//11-11-03 SR 201 JHHJ

    /* --- Get Next Check#,     V3.3 --- */                                                                
    if (pOpen->uchMinorClass == CLASS_UINEWKEYSEQGC) {
        ulGuestNo = pOpen->ulAmountTransOpen;
    } else {
        if ((sStatus = ItemTransGCFNo(&ulGuestNo)) != ITM_SUCCESS) {
            ItemTransACMulDispBack(); 
            return(sStatus);
        }
    }
    
	{
		RflGcfNo  gcfCd = RflPerformGcfCd (ulGuestNo);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		ulGuestNo = gcfCd.usGcNumber;
	}
    sStatus = CliGusReadLockFH( ( USHORT )ulGuestNo, TrnInfo->hsTranPostRecStorage,
                                sizeof(USHORT) + sizeof(USHORT), TrnInfo->usTranConsStoFSize,
                                GCF_REORDER, &ulActualBytesRead);                      /* R3.0 */
    if ( sStatus < 0 ) {                                            /* error */
        usStatus = GusConvertError( sStatus );
        return( ( SHORT )usStatus );            
    } else if ( sStatus == 0 ) {                                    /* no size GCF */
        while ((sStatus = TrnCancelGC(( USHORT )ulGuestNo, GCF_NO_APPEND)) != TRN_SUCCESS) {  /* R3.0 */
            usStatus = GusConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
        return ( LDT_ILLEGLNO_ADR );                                /* Illegal Guest Check # */
    }

    ItemTransOpen->usGuestNo = ( USHORT )ulGuestNo;                     /* save GCF No */

    /* copy GCF qual to work */
    memset( &(gcfti.gcfQual), 0, sizeof(gcfti.gcfQual) ); 
    TrnReadFile_MemoryForce(sizeof(USHORT) + sizeof(USHORT), 
        &(gcfti.gcfQual), sizeof(TRANGCFQUAL), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);

	// check the Guest Check qualifier information to determine if this Guest Check should be allowed
	// to be loaded in and then modified/used by this operator.
    if ((sStatus = ItemTransACMulPreFin1(ItemTransOpen, &(gcfti.gcfQual))) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* auto charge tips, V3.3 */
    ItemTendAutoChargeTips();

    /* no error condition,  terminate previous transaction, V3.3 */
    ItemTransACMulPreFin2();          /* generate previous GCF data setting up memory resident database */

	ItemPreviousItem(0, 0);           /* Saratoga, generate multi check print amount ( US/Canada ) */

    ItemTransACMulPreFin3();          /* generate print data */
    ItemTransACMulPrint();            /* start print service */

    /* -- restore print flags -- */ /* for share print alternation status */
	{
		ITEMPRINT       WorkPrint = {0};

		WorkPrint.uchMajorClass = CLASS_ITEMPRINT;                     
		WorkPrint.uchMinorClass = CLASS_RESTART;                     
		TrnThrough( &WorkPrint );
	}

    ItemTendAffection();                                                /* generate affection data */

    while ( ( sStatus = TrnSendTotal() ) != TRN_SUCCESS ) {             /* memory affection */
        usStatus = TtlConvertError( sStatus );
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }

    TrnInitialize( TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );

    /* set GCF qual to TrnInformation */
    TrnReadFile_MemoryForce(sizeof(USHORT) + sizeof(USHORT), &(gcfti.gcfQual), sizeof(TRANGCFQUAL), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
    TrnWriteFile(sizeof(USHORT) + sizeof(USHORT), &(gcfti.gcfQual), sizeof(TRANGCFQUAL), TrnInfo->hsTranConsStorage);
    TrnPutGCFQual( &(gcfti.gcfQual) );                         /* put GCF qualifier */

    /* set tranitemizers to TrnInformation */
    TrnReadFile_MemoryForce(sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL), &(gcfti.tItemizers), sizeof(TRANITEMIZERS), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
    TrnWriteFile(sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL), &(gcfti.tItemizers), sizeof(TRANITEMIZERS), TrnInfo->hsTranConsStorage);

    TrnPutTI( &(gcfti.tItemizers) );

    /* copy consoli. data from work file to consoli. buffer file */
    ulRWOffset = sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS);

    TrnReadFile_MemoryForce(ulRWOffset, &(gcfti.usVli), sizeof(gcfti.usVli), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
    TrnWriteFile(ulRWOffset, &(gcfti.usVli), ulActualBytesRead, TrnInfo->hsTranConsStorage);


    TrnInfo = TrnGetTranInformationPointer();
    TrnInfo->usTranConsStoVli = gcfti.usVli;

    ulRWOffset += ulActualBytesRead;

    if ((TrnInfo->usTranConsStoVli + sizeof(USHORT)) <=
        (ulRWOffset - (sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS)))) {
        ;
    } else {
        do {
            TrnReadFile_MemoryForce(ulRWOffset, &gcfti, sizeof(gcfti), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
            TrnWriteFile(ulRWOffset, &gcfti, ulActualBytesRead, TrnInfo->hsTranConsStorage);

            ulRWOffset += ulActualBytesRead;

        } while ((TrnInfo->usTranConsStoVli + sizeof(USHORT)) >
            (ulRWOffset - (sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS))));
    }
    
    /* --- write print priority index to consolidate index file, R3.0 --- */
    if (RflGetSystemType () == FLEX_POST_CHK) {
        ulSize = sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS);
		//11-1103- SR 201 JHHJ
		//SR 337 Fix
        TrnReadFile_MemoryForce(ulSize, &usSize1, sizeof(USHORT), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
        if (usSize1 != 0) {
			//11-1103- SR 201 JHHJ
			TrnReadFile_MemoryForce((ulSize + usSize1 + sizeof(USHORT)), uchConsIdBuffer, sizeof(uchConsIdBuffer), TrnInfo->hsTranPostRecStorage, &ulActualBytesRead);
            ulIndexFSize = *((USHORT *)uchConsIdBuffer);
			TrnWriteFile(0, uchConsIdBuffer, ulIndexFSize, TrnInfo->hsTranConsIndex);
				/* --- Save Not Consoli Index File, 1/18/97 --- */
			TrnWriteFile(0, uchConsIdBuffer, ulIndexFSize, TrnInfo->hsTranNoConsIndex);			
        }
    }
                
    ItemTransLocal.uchCheckCounter++;                                   /* count-up check */

    ItemTransACMulDisp();
    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  VOID    ItemTransACMulDisp( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display current balance
======================================================== ==================*/

VOID    ItemTransACMulDisp( VOID )   
{
	REGDISPMSG      DispMsg = {0};

    DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;   
    DispMsg.uchMinorClass = CLASS_UIADDCHECK1;   
    DispMsg.lAmount = ItemTransLocal.lWorkMI + TranItemizersPtr->lMI;   /* multi check amount */
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* tax exempt case */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                        /* tax exempt descriptor */ 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                         /* tax exempt descriptor */
    } else {
        flDispRegDescrControl &= ~TAXEXMPT_CNTRL;                       /* tax exempt descriptor */ 
        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                        /* tax exempt descriptor */
    }

    flDispRegDescrControl |= PAYMENT_CNTRL;                             /* compulsory control */ 
    flDispRegKeepControl |= PAYMENT_CNTRL;                              /* compulsory control */ 
    DispMsg.fbSaveControl = 0;                                          /* save for redisplay */
    DispWrite( &DispMsg );                                              /* display check total */
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMulPreFin1( ItemTransOpen *ItemTransOpen,
*                           TRANGCFQUAL *pWorkGCF )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*               TRANGCFQUAL *pWorkGCF - the Transaction Qualifier data from the guest check
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    part of the procedure for setting up the transaction environment
*                   for function ItemTransACMulPreFin() after a Guest Check is
*                   loaded in from the Guest Check File.
*                   This function checks the various status to determine if the
*                   transaction is valid to be loaded in.
==========================================================================*/

SHORT   ItemTransACMulPreFin1( ITEMTRANSOPEN *ItemTransOpen, TRANGCFQUAL *pWorkGCF )   
{
/* --- Delete,  V3.3 ---
    if ( WorkMode.usWaiterID ) {                                     waiter operation 
        if ( WorkGCF.fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER           cashier GCF 
            || WorkMode.usWaiterID != WorkGCF.usWaiterID ) {         check waiter id 
            while ( ( sStatus = CliGusResetReadFlag( ItemTransOpen->usGuestNo,
                GCF_NO_APPEND) ) != TRN_SUCCESS ) {
                usStatus = GusConvertError( sStatus );
                UieErrorMsg( usStatus, UIE_WITHOUT );
            }
            return( LDT_NOUNMT_ADR );                                Cashier/Waiter # Unmatch 
        }

    } else {                                                         cashier operation V3.3
****/
        /* check guest check team no, V3.3 */
        if ( !(TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM)) {        /* not use team */
            /* V3.3 ICI */
            if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_CLOSE_ANY_CHECK) { /* not close any check */
                if (TranModeQualPtr->ulCashierID != pWorkGCF->ulCashierID) {      /* check cashier id, V3.3 */
                    ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);/* V3.3 */
                    return( LDT_NOUNMT_ADR );                           /* Cashier/Waiter # Unmatch */
                }
            }
        } else {                                                    /* use team */
            if (TranModeQualPtr->uchCasTeamNo != 0) {                       /* not head cashier */
                if (TranModeQualPtr->uchCasTeamNo != pWorkGCF->uchTeamNo) {  /* team no unmatch, V3.3 */
                    ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);/* V3.3 */
                    return( LDT_NOUNMT_ADR );                       /* Cashier/Waiter # Unmatch */
                }
            }
        }
/*** delete V3.3
        if ( WorkGCF.fsGCFStatus & GCFQUAL_NEWCHEK_CASHIER          / cashier GCF /
            && WorkMode.usCashierID != WorkGCF.usCashierID ) {      / check cashier id /

            while ( ( sStatus = CliGusResetReadFlag( ItemTransOpen->usGuestNo,
                GCF_NO_APPEND) ) != TRN_SUCCESS ) {
                usStatus = GusConvertError( sStatus );
                UieErrorMsg( usStatus, UIE_WITHOUT );
            }
            return( LDT_NOUNMT_ADR );                               / Cashier/Waiter # Unmatch /
        }
***/
/*    } V3.3    */

	if ( pWorkGCF->fsGCFStatus & GCFQUAL_PVOID ) {                    /* p-void GCF */
		if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_PVOID ) == 0 ) {
			ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);    /* V3.3 */
			return( LDT_PROHBT_ADR );        
		}
	} else {                                                        /* normal GCF */
		if ( TranCurQualPtr->fsCurStatus & CURQUAL_PVOID ) {
			ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);    /* V3.3 */
			return( LDT_PROHBT_ADR );        
		}
	}

	if (pWorkGCF->fsGCFStatus & GCFQUAL_WIC) {                        /* Saratoga */
		if ((TranCurQualPtr->fsCurStatus & CURQUAL_WIC) == 0) {
			ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);
			return(LDT_PROHBT_ADR);
		}
	} else {                                                        /* normal GCF */
		if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
			ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);
			return(LDT_PROHBT_ADR);
		}
	}

    if ( !( pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING ) ) {            /* normal */    
        if ( ( TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING )
            || ( TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
            ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);    /* V3.3 */
            return( LDT_PROHBT_ADR );        
        }
    } else {                                                        /* training */
        if ( !(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING )
            && !( TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
            ItemTransACMulPreFinError(ItemTransOpen->usGuestNo);    /* V3.3 */
            return( LDT_PROHBT_ADR );        
        }
    }    

    return( ITM_SUCCESS );
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTransACMulPreFinError(USHORT usGuestNo)
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*   Return:     
*
*   Description:    Cancel Guest Check,                             V3.3
*==========================================================================
*/
VOID    ItemTransACMulPreFinError(USHORT usGuestNo)
{
    SHORT   sStatus;

    while ((sStatus = CliGusResetReadFlag(usGuestNo, GCF_NO_APPEND)) != TRN_SUCCESS) {
		USHORT  usStatus = GusConvertError(sStatus);
        UieErrorMsg(usStatus, UIE_WITHOUT);
    }
}

/*==========================================================================
**   Synopsis:  VOID    ItemTransACMulPreFin2( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate previous GCF data setting up the transaction environment
*                   for a Guest Check that has been read in from the Guest Check file.
==========================================================================*/

VOID    ItemTransACMulPreFin2( VOID )   
{
    DCURRENCY       lTax = 0L;
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();   /* get GCF qualifier */
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();         /* get transaction itemizers */
    TRANCURQUAL     WorkCur = *TrnGetCurQualPtr();   // get a copy of the Current Qualifier
	ITEMAFFECTION   WorkAffection = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    SHORT           i;

	NHPOS_ASSERT_TEXT((ItemTransLocal.uchCheckCounter < STD_MAX_MULTI_CHECK), "**ERROR: Subscript out of range.");

    for ( i = 0; i < 3; i++ ) {
        ItemTransLocal.alAddcheck[ ItemTransLocal.uchCheckCounter ][i] = WorkTI->lService[i];  /* addcheck total */
    }

    ItemTransLocal.ausGuestNo[ ItemTransLocal.uchCheckCounter ] = WorkGCF->usGuestNo;
    ItemTransLocal.auchCdv[ ItemTransLocal.uchCheckCounter ] = WorkGCF->uchCdv;

    ItemTransLocal.alMI[ ItemTransLocal.uchCheckCounter ] = WorkTI->lMI;
    ItemTransLocal.lWorkMI += WorkTI->lMI;                              /* total amount of multi check */

    /* generate affect tax */
    WorkAffection.uchMajorClass = CLASS_ITEMAFFECTION;              /* major class */
    WorkAffection.uchMinorClass = CLASS_TAXAFFECT;                  /* minor class */
    
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax( &WorkCur.auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );   /* calculate current tax */
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
		if(WorkTax.alTaxable[1] > 100000)
		{
			// Tim Horton's of Canada was having a problem with a tax calculation error
			// which would suddenly happen and then not be seen for a while.
			// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
			//   Richard Chambers, Apr-09-2015
			NHPOS_ASSERT(!"ItemTransACMulPreFin2 TAX ERROR");
		}

        if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {
            WorkAffection.USCanVAT.USCanTax.lTaxExempt[0] = WorkTax.alTaxExempt[0];
            ItemTransLocal.alCorrectExempt[0] += WorkTax.alTaxExempt[0];
        } else {
            WorkAffection.USCanVAT.USCanTax.lTaxable[0] = WorkTax.alTaxable[0];
            ItemTransLocal.alCorrectTaxable[0] += WorkTax.alTaxable[0];
            ItemTransLocal.alCorrectTaxable[0] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0]; 
        }
        if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
            for ( i = 0; i < 3; i++ ) {
                WorkAffection.USCanVAT.USCanTax.lTaxExempt[i+1] = WorkTax.alTaxExempt[i+1];
                ItemTransLocal.alCorrectExempt[i + 1] += WorkTax.alTaxExempt[i+1];
            }
        } else {
            for ( i = 0; i < 3; i++ ) {
                WorkAffection.USCanVAT.USCanTax.lTaxable[i+1] = WorkTax.alTaxable[i+1];
                ItemTransLocal.alCorrectTaxable[i+1] += WorkTax.alTaxable[i+1];
                ItemTransLocal.alCorrectTaxable[i+1] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
            }
        }
        WorkAffection.USCanVAT.USCanTax.lTaxable[4] = WorkTax.alTaxable[4];

        for ( i = 0; i < 4; i++ ) {                                     /* save Canadian tax itemizers */
            WorkAffection.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];             /* tax for print */
            ItemTransLocal.alCorrectTax[i] += WorkTax.alTax[i];         /* tax for correction */
            ItemTransLocal.alCorrectTax[i] += WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i]; 
        }

        ItemTransLocal.sPigRuleCo += WorkGCF->sPigRuleCo; 
        
        if ( WorkCur.auchTotalStatus[1] != 0 ) {     // is there any Canadian tax that needs to be affected?
            TrnItem( &WorkAffection );               /* transaction module i/F */
        }

        /* send to enhanced kds, 2172 */
        ItemSendKds(&WorkAffection, 0);

        /* generate print tax */
        WorkAffection.uchMinorClass = CLASS_TAXPRINT;                   /* minor class */
        WorkAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if ( WorkCur.flPrintStatus & CURQUAL_POSTCHECK ) {              /* postcheck, store/recall system */
            
            if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {
                WorkAffection.USCanVAT.USCanTax.lTaxExempt[0] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
            } else {
                WorkAffection.USCanVAT.USCanTax.lTaxable[0] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
            }
            if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
                for( i = 0; i < 3; i++ ) {
                    WorkAffection.USCanVAT.USCanTax.lTaxExempt[i+1] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
                }
            } else {
                for( i = 0; i < 3; i++ ) {
                    WorkAffection.USCanVAT.USCanTax.lTaxable[i+1] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
                }
            }
            WorkAffection.USCanVAT.USCanTax.lTaxable[4] += WorkTax.alTaxable[4];
    
            for( i = 0; i < 4; i++ ) {
                WorkAffection.USCanVAT.USCanTax.lTaxAmount[i] += WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i]; 
            }

            if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {
                WorkAffection.USCanVAT.USCanTax.lTaxExempt[0] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
            } else {
                WorkAffection.USCanVAT.USCanTax.lTaxable[0] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
                WorkAffection.USCanVAT.USCanTax.lTaxAmount[0] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[0];
            }
            if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
                for ( i = 0; i < 3; i++ ) {
                    WorkAffection.USCanVAT.USCanTax.lTaxExempt[i+1] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i+1];
                }
            } else {
                for ( i = 0; i < 3; i++ ) {
                    WorkAffection.USCanVAT.USCanTax.lTaxable[i+1] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i+1];
                    WorkAffection.USCanVAT.USCanTax.lTaxAmount[i+1] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i+1];
                }
            }
        }
        TrnItem( &WorkAffection );                                      /* transaction module i/F */
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {                   /* V3.3 */
        /* generate affect tax */
        ItemCurTax( &WorkCur.auchTotalStatus[1], &WorkTax );                  /* calculate current tax */
                    
        for( i = 0; i < 3; i++ ) {                                      /* update affect taxable */
            WorkAffection.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];           /* taxable for print */
            ItemTransLocal.alCorrectTaxable[i] += WorkTax.alTaxable[i]; /* taxable for correction */
            ItemTransLocal.alCorrectTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i]; 
                                                                            
            WorkAffection.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];             /* tax for print */
            ItemTransLocal.alCorrectTax[i] += WorkTax.alTax[i];         /* tax for correction */
            ItemTransLocal.alCorrectTax[i] += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i]; 
        }

		//       ITM_TTL_FLAG_TAX_1                   ITM_TTL_FLAG_TAX_2                     ITM_TTL_FLAG_TAX_3
		//       CURQUAL_TOTAL_FLAG_TAX_1             CURQUAL_TOTAL_FLAG_TAX_2               CURQUAL_TOTAL_FLAG_TAX_3
		if ( (WorkCur.auchTotalStatus[1] & 0x01) || (WorkCur.auchTotalStatus[1] & 0x10) || (WorkCur.auchTotalStatus[2] & 0x01) ) {
            TrnItem( &WorkAffection );                                  /* transaction module i/F */
        }

        /* generate print tax */
        if ( !( ItemTransLocal.fbTransStatus & TROP_OPENPOSTCHECK ) ) {                          /* post check w/o reset */
            WorkAffection.uchMinorClass = CLASS_TAXPRINT;                   /* minor class */
            WorkAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            if ( WorkCur.flPrintStatus & CURQUAL_POSTCHECK ) {              /* postcheck, store/recall system */
                for( i = 0; i < 3; i++ ) {                                  /* update affect taxable */
                    WorkAffection.USCanVAT.USCanTax.lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i]; 
                    WorkAffection.USCanVAT.USCanTax.lTaxAmount[i] += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i]; 
                    WorkAffection.USCanVAT.USCanTax.lTaxExempt[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i];
                }
            }
            TrnItem( &WorkAffection );                                  /* transaction module i/F */
        }
    } else {
        ItemCurVAT( &WorkCur.auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
        memset(&WorkAffection, 0, sizeof(ITEMAFFECTION));
        memcpy(&WorkAffection.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
        WorkAffection.lAmount = WorkVAT.lService;
        ItemTendVATCommon(&WorkAffection, ITM_NOT_SPLIT, ITM_DISPLAY);
    }

    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
        lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
    } else {
        lTax = WorkVAT.lPayment;
    }
            
    /*----- Display Tax to LCD,   R3.0 -----*/
    WorkAffection.uchMinorClass = CLASS_TAXPRINT;
    MldScrollWrite(&WorkAffection, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(&WorkAffection);

    ItemTransLocal.alMI[ ItemTransLocal.uchCheckCounter ] += lTax;
    ItemTransLocal.lWorkMI += lTax;                 /* total amount of multi check */

    if (!(CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0))) {  /* add R2.5 CP  */ 
        WorkTI->lHourly += lTax;                    /* set hourly itemizer  */
    }
}


/*==========================================================================
**   Synopsis:  VOID    ItemTransACMulPreFin3( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate previous GCF print data
==========================================================================*/

VOID    ItemTransACMulPreFin3( VOID )   
{
    TRANCURQUAL  *pWorkCur = TrnGetCurQualPtr();
	ITEMPRINT    ItemPrint = {0};

    ItemPrint.uchMajorClass = CLASS_ITEMPRINT;    
    ItemPrint.uchMinorClass = CLASS_CHECKPAID;                  /* check paid */
    ItemPrint.fsPrintStatus = PRT_SLIP;   
    TrnItem( &ItemPrint );

    pWorkCur->fsCurStatus |= CURQUAL_MULTICHECK;                /* during multi check transaction */
                                
    ItemTrailer( TYPE_STORAGE );                                /* strored normal trailer */
}



/*==========================================================================
**   Synopsis:  VOID    ItemTransACMulPrint( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    decide, set print state
==========================================================================*/
        
VOID    ItemTransACMulPrint( VOID )   
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
	ITEMTRANSCLOSE  ItemTransClose = {0};

    if ( pWorkCur->flPrintStatus & CURQUAL_POSTCHECK ) {  
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;                                /* postheck, store/recall */
    } else if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;                                /* unbuffering */
    } else {
        pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;                  /* precheck */
    }

    pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;                    /* for kitchen print */

    if ( ( CliParaMDCCheck( MDC_PSELVD_ADR, EVEN_MDC_BIT1 ) == 0    /* kitchen send at pvoid */
        && ( pWorkCur->fsCurStatus & CURQUAL_PRESELECT_MASK ) )                /* preselect void */
        || ( pWorkCur->fsCurStatus & CURQUAL_TRAINING ) ) {           /* training */

        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;                  /* for kitchen print */
    }

    pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;                   
    if ( ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK )
        && ( !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) )           // if Issue Soft Check Print by Total Key
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)            // if Tax calculate at split key 
        && ( !(pWorkCur->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING)) ) {           /* 12/15/93 */
        pWorkCur->fbNoPrint |= CURQUAL_NO_R;                   
    }

    if ( pWorkCur->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP ) {    /* compulsory slip */
        pWorkCur->fbCompPrint = PRT_SLIP;                   
    } else {                                                        /* optional slip */
        pWorkCur->fbCompPrint &= ~PRT_SLIP;
    }

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;            /* major class */
    ItemTransClose.uchMinorClass = CLASS_CLSTENDER1;                /* minor class */
    TrnClose( &ItemTransClose );                                    /* transaction module i/F */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACMulti( ITEMTRANSOPEN    *ItemTransOpen )   
*                               
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:      
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction open data
==========================================================================*/

VOID    ItemTransACMulti( ITEMTRANSOPEN   *ItemTransOpen )   
{
    ItemTransOpen->uchMajorClass = CLASS_ITEMTRANSOPEN;
    if ( ItemTransLocal.fbTransStatus & TROP_OPENPOSTCHECK ) {                      /* executed buffer print at post check */
        ItemTransOpen->uchMinorClass = CLASS_REORDER;
    } else {
        ItemTransOpen->uchMinorClass = CLASS_ADDCHECK;
    }

    ItemTransOpen->lAmount = TranItemizersPtr->lMI;
    ItemTransOpen->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
	ItemTransOpen->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    if ( !( ItemTransLocal.fbTransStatus & TROP_OPENPOSTCHECK ) ) {
            ItemTransOpen->fbStorageStatus |= NOT_CONSOLI_STORAGE;      /* not store consoli storage */
    }

//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
//        ItemTransOpen->usTableNo = ItemCommonUniqueTransNo();
//    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemtransACMulBack( VOID )   
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate back ground data
==========================================================================*/

VOID    ItemTransACMulDispBack( VOID )   
{
	REGDISPMSG  DispMsg = {0};

    DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;                       /* dummy */
    DispMsg.uchMinorClass = CLASS_UITABLENO;                            /* dummy */
    DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY;                        /* save for redisplay (release on finalize) */
    DispWrite( &DispMsg );                                              /* display check total */
                                                                        
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
}




/*==========================================================================
**   Synopsis:  SHORT   ItemTransACSto( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    recall operation
==========================================================================*/
                      
SHORT   ItemTransACSto( UIFREGTRANSOPEN *UifRegTransOpen )   
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
	ITEMTRANSOPEN   ItemTransOpen = {0};
    SHORT           sStatus;    

    if ( ItemModLocalPtr->fsTaxable ) {
		/* sequence error if tax modifier key has been used */
        return( LDT_SEQERR_ADR );
    }
    
    if ( UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER ) {
		/* sequence error if Void key has been used */
        return( LDT_SEQERR_ADR );
    }

    if ( ( sStatus = ItemTransACStoGet( UifRegTransOpen ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    ItemTransNCPara();                                              /* set current qualifier */

    ItemPrintStart( TYPE_POSTCHECK );

    ItemTransACStoRec( &ItemTransOpen );                            /* generate transaction open data */
    TrnOpen( &ItemTransOpen );                                      /* transaction module i/F */

	ItemTransACStoOrderCounterOpen(WorkGCF->usGuestNo);      /* R3.0 */

    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();

    /*--- Display GCF/Order#, R3.0 ---*/
    /* V3.3 */
    if (CliParaMDCCheck( MDC_DISPLAY_ADR, ODD_MDC_BIT3 ) ) { 
		UCHAR  uchMinor = ItemTransOpen.uchMinorClass;
        ItemTransOpen.uchMinorClass = CLASS_REORDER;               /* for display */
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
        ItemTransOpen.uchMinorClass = uchMinor;
    } else {
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
    }
    /*--- Display New Unique Transaction No. 05/29/96 ---*/
    MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
	// Commented out the following MldScrollFileWrite() call
	// because it is causing problems with Store/Recall in
	// condiment edit mode.  The recalled guest check is
	// pulled into the MLD subsystem as part of the redisplay
	// of the transaction and these kinds of non-displayable
	// items are causing the item number counts to be off.
	// Richard Chambers, Dec-20-2007
//    MldScrollFileWrite(&ItemTransOpen);

    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

    ItemTransACNorAff();                                            /* generate affection data */

	// Since we are retrieving this item, lets check to see if during the display of
	// the guest check that we encountered an order declare.
	if (MldTransactionAttributesFound (MLD_SCROLL_1) & MLD_TRANS_ATTRIB_ORDERDEC) {
		TRANCURQUAL *pWorkCur = TrnGetCurQualPtr();

		pWorkCur->fsCurStatus2 |= CURQUAL_ORDERDEC_DECLARED;
	}

    /* send to enhanced kds, 2172 */
    ItemTransOpen.usGuestNo = WorkGCF->usGuestNo;
    ItemSendKds(&ItemTransOpen, 0);

    return( ITM_SUCCESS );
}


/*==========================================================================
**   Synopsis:  SHORT   ItemTransACStoGet( UIFREGTOTAL *UifRegTransOpen )
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    get target Guest Check (order No).  This is used by Store/Recall
*                   systems only to retrieve the next stored guest check for either
*                   a Flexible Drive Thru window register or a counter register.
==========================================================================*/

SHORT   ItemTransACStoGet( UIFREGTRANSOPEN *UifRegTransOpen )   
{
    TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
	FDTPARA         WorkFDT = { 0 };
    USHORT          uchOrder;
    SHORT           sStatus;
    USHORT          usStatus, usType;

	// get the terminal type information to discover the get guest check type needed.
	FDTParameter(&WorkFDT);
    if ( WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1       /* Order/Payment Termninal (Queue#1) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1           /* Payment Termninal (Queue#1) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1             /* Order Termninal (Queue#1) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1) {
        if ( UifRegTransOpen->ulAmountTransOpen == 0L ) {
            usType = GCF_PAYMENT_QUEUE1;
        } else {
            usType = GCF_REORDER;
        }
    } else if ( WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2/* Order/Payment Termninal (Queue#2) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2           /* Payment Termninal (Queue#2) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2             /* Order Termninal (Queue#2) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2) {
        if ( UifRegTransOpen->ulAmountTransOpen == 0L ) {
            usType = GCF_PAYMENT_QUEUE2;
        } else {
            usType = GCF_REORDER;
        }
    } else if ( WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE       /* Counter Terminal (Store) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY        /* Counter Terminal (Store/Payment) */
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
        || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
        if (UifRegTransOpen->ulAmountTransOpen == 0) {
            uchOrder = FDTGetStoreQueue();
            if (uchOrder == 0) {
                return(LDT_NTINFL_ADR);    // indicate to the cashier that no more guest checks pending.
            } else {
                UifRegTransOpen->ulAmountTransOpen = uchOrder;
            }
        }
        usType = GCF_REORDER;
    }

	// indicate that we need the consolidated transaction data storage as well.
	// Store/Recall and Post Check both store additional memory resident guest check data
	// in a guest check stored in the Guest Check file.
    sStatus = TrnGetGC( ( USHORT )UifRegTransOpen->ulAmountTransOpen, 1, usType );
    if ( sStatus < 0 ) {                                            /* error */
        usStatus = GusConvertError( sStatus );
        return( ( SHORT )usStatus );            
    } else if ( sStatus == 0 ) {                                    /* no size GCF */
        ItemCommonCancelGC(pWorkGCF);                                /* R3.0 */
        return ( LDT_ILLEGLNO_ADR );                                /* Illegal Guest Check # */
    }

	//We now clear the hourly bonus totals, because we have already affected them to the Hourly Totals,
	//by doing this, we no longer run into the problem when the user opens up a check, rings up items, then
	//closes the check, and when they tender it out, it updates the bonuses incorrectly.  In This case, we have 
	//just re-opened a check that already has had been closed, and because the transaction is saved prior to being affected
	//we must clear the bonus totals, because they have already been updated to the hourly and bonus totals.
	// SR278 
	memset(TrnInformation.TranItemizers.lHourlyBonus, 0x00, sizeof(TrnInformation.TranItemizers.lHourlyBonus));

	sStatus = ITM_SUCCESS;
	do {    // do once, series of tests to determine if prohibit action or not.
		if ((pWorkGCF->fsGCFStatus & GCFQUAL_PVOID) && ( TranCurQualPtr->fsCurStatus & CURQUAL_PVOID ) == 0 ) {      /* p-void GCF */
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}
		if ((pWorkGCF->fsGCFStatus & GCFQUAL_PVOID ) == 0 && (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID)) {      /* p-void GCF */
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}

		if ((pWorkGCF->fsGCFStatus & GCFQUAL_WIC) && (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) == 0) {
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}
		if ((pWorkGCF->fsGCFStatus & GCFQUAL_WIC) == 0 && (TranCurQualPtr->fsCurStatus & CURQUAL_WIC)) {
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}

		if ( !(pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING ) ) {           /* normal */    
			if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
				sStatus = ( LDT_PROHBT_ADR );
				break;
			}
		} else {                                                        /* training */
			if ( !(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING ) && !(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) ) {
				sStatus = ( LDT_PROHBT_ADR );
				break;
			}
		}    

		if (TranModeQualPtr->ulCashierID != pWorkGCF->ulCashierID && (pWorkGCF->fsGCFStatus & GCFQUAL_DRIVE_THROUGH) == 0) {
			sStatus = ( LDT_PROHBT_ADR );
			break;
		}
	} while (0);  // do only once the first time through.

	if (sStatus != ITM_SUCCESS) {
        ItemCommonCancelGC(pWorkGCF);                                /* R3.0 */
	}
                                                                                                                                    
    return( sStatus );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransACStoRec( ITEMTRANSOPEN *ItemTransOpen )   
*                               
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction open data
==========================================================================*/

VOID    ItemTransACStoRec( ITEMTRANSOPEN   *ItemTransOpen )   
{
	REGDISPMSG      DispMsg = {0};

    ItemTransOpen->uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen->uchMinorClass = CLASS_ADDCHECK;
    ItemTransOpen->lAmount = TranItemizersPtr->lMI;
    ItemTransOpen->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    ItemTransOpen->fbStorageStatus = NOT_CONSOLI_STORAGE;           /* not store consoli storage */
	ItemTransOpen->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

    DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;   
    DispMsg.uchMinorClass = CLASS_UIADDCHECK1;   
    DispMsg.lAmount = ItemTransOpen->lAmount;                       /* reorder amount */
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* tax exempt case */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt descriptor */ 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt descriptor */
    }

    flDispRegDescrControl |= PAYMENT_CNTRL;                         /* compulsory control */ 
    flDispRegKeepControl |= PAYMENT_CNTRL;                          /* compulsory control */ 
    DispMsg.fbSaveControl = 0;                                      /* save for redisplay */
    DispWrite( &DispMsg );                                          /* display check total */

//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
//        ItemTransOpen->usTableNo = ItemCommonUniqueTransNo();
//    }
}

SHORT ItemTransGetReturnsGcf (GCNUM  usGCNumber, GCNUM usNewGCNumber)
{
	TRANGCFQUAL      *pWorkGCF = TrnGetGCFQualPtr();
	TRANCURQUAL      *pWorkCurQual = TrnGetCurQualPtr();
	TRANMODEQUAL     *pWorkMode = TrnGetModeQualPtr();
	TRANITEMIZERS    *pWorkTI = TrnGetTIPtr();
    ITEMCOMMONLOCAL  *pCommon = ItemCommonGetLocalPointer();
	TRANGCFQUAL      SaveWorkGCF;
	TRANMODEQUAL     SaveWorkMode;
	TRANITEMIZERS    SaveWorkTI;
	SHORT            sRetStatus = LDT_PROHBT_ADR, sType = 0;

	SaveWorkGCF = *pWorkGCF;
	SaveWorkMode = *pWorkMode;
	SaveWorkTI = *pWorkTI;

	{
		USHORT    usSystemType = RflGetSystemType ();

		sType = 0;                                                      /* precheck system */
		if ( usSystemType == FLEX_STORE_RECALL || usSystemType == FLEX_POST_CHK ) {
			// indicate that we need the consolidated transaction data storage as well.
			// Store/Recall and Post Check both store additional memory resident guest check data
			// in a guest check stored in the Guest Check file.
			sType = 1;                                                  /* precheck system */
		}
	}

	/*----- Read Transaction -----*/
	pWorkCurQual->fsCurStatus |= CURQUAL_TRETURN;        // indicate TTL_TUP_TRETURN for totals functionality
	pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;            /* ItemTransGetReturnsGcf(): indicate this is a transaction return */
	sRetStatus = ItemCommonTransOpen(usNewGCNumber);
	if (sRetStatus != ITM_SUCCESS) {
		pWorkCurQual->fsCurStatus &= ~CURQUAL_TRETURN;        // failed so clear TTL_TUP_TRETURN for totals functionality
		pWorkGCF->fsGCFStatus &= ~GCFQUAL_TRETURN;            /* ItemTransGetReturnsGcf(): failed so clear indication this is a transaction return */
		return(sRetStatus);
	}

    if ((sRetStatus = TrnGetGC(usGCNumber, sType, GCF_RETURNS)) > 0) {
		sRetStatus = TRN_SUCCESS;
        /*----- Check Payment Transaction -----*/
        if ((pWorkGCF->fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) == 0) {
            TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
            sRetStatus = LDT_PROHBT_ADR;
        }

        /*----- Check Normal/Training Transaction -----*/
        if (pWorkMode->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
            if (!(pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING)) {
                TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                sRetStatus = LDT_PROHBT_ADR;
            }
        } else {
            if (pWorkGCF->fsGCFStatus & GCFQUAL_TRAINING) {
                TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                sRetStatus = LDT_PROHBT_ADR;
            }
        }

		if (sRetStatus == TRN_SUCCESS) {
			int i;
			// set the unique identifier of the return to this
			for (i = 0; i < sizeof(SaveWorkGCF.uchUniqueIdentifierReturn)/sizeof(SaveWorkGCF.uchUniqueIdentifierReturn[0]); i++) {
				SaveWorkGCF.uchUniqueIdentifierReturn[i] = pWorkGCF->uchUniqueIdentifier[i];
			}

			*pWorkGCF  = SaveWorkGCF;     // reinitialize the memory resident data base with the initial transaction state
			*pWorkMode = SaveWorkMode;    // reinitialize the memory resident data base with the initial transaction state
			*pWorkTI   = SaveWorkTI;    // reinitialize the memory resident data base with the initial transaction state
			pCommon->fbCommonStatus |= COMMON_PAIDOUT_RETURNS;
			pWorkCurQual->fsCurStatus |= CURQUAL_TRETURN;        // indicate TTL_TUP_TRETURN for totals functionality
			pWorkCurQual->usReturnsFrom = usGCNumber;            // remember which guest check number in case of cancel action
			pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;            /* ItemTransGetReturnsGcf(): indicate this is a transaction return */
		}
	} else {
		pWorkCurQual->fsCurStatus &= ~CURQUAL_TRETURN;        // failed so clear TTL_TUP_TRETURN for totals functionality
		pWorkGCF->fsGCFStatus &= ~GCFQUAL_TRETURN;            /* ItemTransGetReturnsGcf(): failed so clear indication this is a transaction return */
	}

	return sRetStatus;
}

/****** end of source ******/