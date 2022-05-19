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
:   Program Name   : ITTROP6.C (cashier interrupt recall)                                       
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
:   ItemTransACCIRecall()   ;   cashier interrupt recall
:   ItemTransACCIRecallGC() ;   GCF recall
:   ItemTransCasIntRecall   ;   generate transaction open data
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name      : Description
: 98/06/04 : 03.03.00 :  M.Ozawa    : initial
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

#include    <ecr.h>
#include    <uie.h>
#include    <pif.h>
#include    <uireg.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csgcs.h>
#include    <rfl.h>
#include    <display.h>
#include    <mld.h>
#include    <item.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"
#include    <pmg.h>

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACCIRecallGC( ItemTransOpen *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN   *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    recall specified transaction from Guest Check File
==========================================================================*/
static SHORT   ItemTransACCIRecallGC( ITEMTRANSOPEN   *ItemTransOpen )   
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    SHORT           sStatus, sType;
    USHORT          usGuestNo;

    sType = 0;            /* assume precheck system with no extra transaction data */
    if ( RflGetSystemType() == FLEX_POST_CHK ) {
		// indicate that we need the consolidated transaction data storage as well.
		// Store/Recall and Post Check both store additional memory resident guest check data
		// in a guest check stored in the Guest Check file.
        sType = 1;
    }

    usGuestNo = TRANSOPEN_GCNO(TranModeQualPtr->ulCashierID);
    sStatus = TrnGetGC( usGuestNo, sType, GCF_REORDER );
    if ( sStatus < 0 ) {                                            /* error */
        USHORT   usStatus = GusConvertError( sStatus );
        return( ( SHORT )usStatus );            
    } else if ( sStatus == 0 ) {                                    /* no size GCF */
        ItemCommonCancelGC(WorkGCF);                                /* R3.0 */
        return ( LDT_ILLEGLNO_ADR );                                /* Illegal Guest Check # */
    }

    if ( WorkGCF->fsGCFStatus & GCFQUAL_PVOID ) {                   /* p-void GCF */
        pWorkCur->fsCurStatus |= CURQUAL_PVOID;
    }
    if ( WorkGCF->fsGCFStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN) ) {  /* transaction return GCF */
        pWorkCur->fsCurStatus |= CURQUAL_TRETURN;
    }
    if (WorkGCF->fsGCFStatus & GCFQUAL_WIC) {                       /* Saratoga */
        pWorkCur->fsCurStatus |= CURQUAL_WIC;
    }

    ItemTransOpen->usGuestNo = WorkGCF->usGuestNo;                  /* save GCF No */
    ItemTransOpen->uchCdv = WorkGCF->uchCdv;                        /* save CDV */

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACCIRecall( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    cashier interrupt recall operation
==========================================================================*/
        
SHORT   ItemTransACCIRecall( UIFREGTRANSOPEN *UifRegTransOpen )   
{
	ITEMTRANSOPEN   ItemTransOpen = {0};
    SHORT           sStatus;    

    if ( ( sStatus = ItemTransACCIRecallGC( &ItemTransOpen ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    ItemTransNCPara();                                              /* set current qualifier */

    if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER 
        || ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_BUFFER
        || ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            && TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) ) {

        if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
            || TrnGetStorageSize( TRN_TYPE_CONSSTORAGE ) == 0 ) {                        /* print transaction header */

            ItemTransLocal.fbTransStatus |= TROP_OPENPOSTCHECK;	    /* transaction status */
        }                

        ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                     /* print promotion header */
        ItemHeader( TYPE_STORAGE );                                 /* p-void, training header 8/24/93 */
    } else {                                                        /* postcheck, store/recall */
        ItemPrintStart( TYPE_POSTCHECK );                           /* start print service */
        ItemHeader( TYPE_CANCEL );                                  /* p-void, training header 8/24/93 */
    }

    ItemTransAddcheck(&ItemTransOpen);
    ItemTransOpen.uchMinorClass = CLASS_CASINTRECALL;               /* modify the minor class to represent cashier interrupt */
    TrnOpen( &ItemTransOpen );                                      /* transaction module i/F */

    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransOpen, 0);

    /*--- Display GCF#, R3.1 ---*/
    if (  ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) 
        ||((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER)
        ||  CliParaMDCCheckField( MDC_DISPLAY_ADR, MDC_PARAM_BIT_A ) ) { 
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    } else {
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&ItemTransOpen);
    }
    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

    ItemTransACNorAff();                                            /* generate affection data */

    return( ITM_SUCCESS );

    /* dummy */
    UifRegTransOpen = UifRegTransOpen;
}


/****** End of Source ******/
