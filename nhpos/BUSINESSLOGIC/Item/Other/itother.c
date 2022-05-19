/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             NCR Corporation, E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9                  **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : other - ItemOther Module                         
* Category    : REG MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: ITOTHER.C
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
* ----------------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*    VOID    ItemOtherClear( VOID )
*    SHORT   ItemOtherDemand( VOID )
*    VOID    ItemOtherHeader(SHORT sLine )
*    SHORT   ItemOtherSlipRel( VOID )   
*    SHORT   ItemOtherSlipFeed( UIFREGOTHER *ItemOther)
*    VOID    ItemOtherRecFeed(VOID)
*    VOID    ItemOtherJouFeed(VOID)
*    SHORT   ItemOtherECor(VOID)
*    SHORT   ItemOtherECFS(VOID)
*    VOID    ItemOtherECFC(VOID)
*    SHORT   ItemOtherEC(VOID)
*    SHORT   ItemOtherECSPV(VOID)  - check for supervisor intervention MDC set
*    SHORT   ItemOtherECSales(ITEMCOMMONLOCAL *ItemCommonL)
*    SHORT   ItemOtherECDisc(ITEMCOMMONLOCAL *ItemCommonL)
*    SHORT   ItemOtherECCoupon(ITEMCOMMONLOCAL *ItemCommonL)
*    SHORT   ItemOtherECTend(ITEMCOMMONLOCAL *ItemCommonL)    
*    SHORT   ItemOtherComputat( UIFREGOTHER *ItemOther )
*    
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
: Jan-12-95:          : hkato       : R3.0
: Nov-09-95:          : hkato       : R3.1
: Dec-06-99: 01.00.00 : hrkato      : Saratoga
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
#include    <stdlib.h>
#include    <String.h>

#include    "ecr.h"
#include    "uie.h"
#include    "fsc.h"
#include    "log.h"
#include    "rfl.h"
#include    "appllog.h"
#include    "regstrct.h"
#include    "Paraequ.h"
#include    "Para.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "transact.h"
#include    "pif.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "prt.h"
#include    "pmg.h"
#include    "csttl.h"
#include    "csop.h"
#include    "report.h"
#include    "uireg.h"
#include    "fdt.h"
#include    "dif.h"
#include    "isp.h"
#include    "itmlocal.h"
#include	"trans.h"
#include	"maint.h"
/*
*===================================================================================
* REFERNCE SYSTEM RAM
* ----------------------------------------------------------------------------------
*/

ITEMOTHERLOCAL ItemOtherLocal;

static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* saratoga */

/*fhfh
*===========================================================================
** Synopsis:    ItemOtherGetLocal(ITEMOTHERLOCAL *ItemGetLocal)
** Synopsis:    ItemOtherPutLocal(ITEMOTHERLOCAL *ItemPutLocal)
*                                     
*     Input:    ITEMOTHERLOCAL *ItemGetLocal
*    Output:    nothing            
*
** Return:      nothing
*
** Description: ItemOtherOrder executes to display order no., in case store/recall system 
** Description: ItemOtherOrder executes to display order no., in case store/recall system 
*===========================================================================
fhfh*/

ITEMOTHERLOCAL *ItemOtherGetLocalPointer (VOID)
{
	return &ItemOtherLocal;
}



/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemOtherClear( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: This function executes "clear" function  
*===========================================================================
fhfh*/
VOID   ItemOtherClear( VOID )       
{
    /* correction10/08 */
    /* check pressing total key or not */
    if ( (TranGCFQualPtr->fsGCFStatus & ( GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT )) || ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT )) {           /* R3.0 */
        /* clear descriptor for flDispRegDescrControl*/
        flDispRegDescrControl &= ~ ( VOID_CNTRL | EC_CNTRL | ITEMDISC_CNTRL | REGDISC_CNTRL
            | CHRGTIP_CNTRL | CRED_CNTRL | TAXMOD_CNTRL | PO_CNTRL | ROA_CNTRL | FOODSTAMP_CTL 
            | TIPSPO_CNTRL | DECTIP_CNTRL | CHANGE_CNTRL | ADJ_CNTRL );

        /* clear descriptor for flDispRegKeepControl*/
        flDispRegKeepControl &= ~ ( VOID_CNTRL | EC_CNTRL | ITEMDISC_CNTRL | REGDISC_CNTRL
            | CHRGTIP_CNTRL | CRED_CNTRL | TAXMOD_CNTRL | PO_CNTRL | ROA_CNTRL | FOODSTAMP_CTL
            | TIPSPO_CNTRL | DECTIP_CNTRL | CHANGE_CNTRL | ADJ_CNTRL );
    } else{
        /* clear descriptor for flDispRegDescrControl*/
        flDispRegDescrControl &= ~ ( VOID_CNTRL | EC_CNTRL | ITEMDISC_CNTRL | REGDISC_CNTRL
            | CHRGTIP_CNTRL | CRED_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | PO_CNTRL | ROA_CNTRL 
            | TIPSPO_CNTRL | DECTIP_CNTRL | CHANGE_CNTRL | ADJ_CNTRL | FOODSTAMP_CTL);

        /* clear descriptor for flDispRegKeepControl*/
        flDispRegKeepControl &= ~ ( VOID_CNTRL | EC_CNTRL | ITEMDISC_CNTRL | REGDISC_CNTRL
            | CHRGTIP_CNTRL | CRED_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | PO_CNTRL | ROA_CNTRL 
            | TIPSPO_CNTRL | DECTIP_CNTRL | CHANGE_CNTRL | ADJ_CNTRL | FOODSTAMP_CTL);
    }

	{
		ITEMMODLOCAL    *pItemModLocal = ItemModGetLocalPtr();     

		pItemModLocal->fbModifierStatus &= ~(MOD_OFFLINE | MOD_FSMOD);
		pItemModLocal->fsTaxable = 0;                                    /* clear taxable status indicators */
		memset(&(pItemModLocal->ScaleData), 0, sizeof(ITEMMODSCALE));    /* clear scale data */
	}

    /* update and clear the display */
	{
		REGDISPMSG      RegDispMsgD = {0};

		RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;  /*set majour class */
		RegDispMsgD.uchMinorClass = CLASS_UICLEAR;      /*set minor class */
		DispWrite( &RegDispMsgD );  /*  */
	}
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherDemand( VOID )
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: ItemOtherDemand executes print on demand 
*===========================================================================
fhfh*/                  

SHORT   ItemOtherDemand( VOID )
{
    TRANCURQUAL  TranCurQualLD;
    REGDISPMSG   RegDispMsgD = {0};
    SHORT   sStatus = ITM_SUCCESS;  /* INITIALIZE STATUS */ 
    UCHAR   fbSaveNoPrint; 
    UCHAR   fbSaveCompPrint;
    UCHAR   uchSaveKitchenStorage;

    /* correction10/08 */
    /* check pressing total key or not    R3.0 */
    if (ItemModLocalPtr->fsTaxable & MOD_CANADAEXEMPT) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }
 
    TrnGetCurQual( &TranCurQualLD );  /* get quaifier data */

    /* check w/ unbuffering option or w/o */
    if ( (TranCurQualLD.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER ) {
        return(LDT_SEQERR_ADR); /* w/o option , set error status */
    }

#if defined(POSSIBLE_DEAD_CODE)
// When print on demand is called, the previous item should have already been sent
// to the transaction module. Sending again will allow operator to sign out
// in the middle of a transaction.
    if ( (sStatus = ItemTotalPrev()) != ITM_SUCCESS ) {
        return(sStatus);
    }
#endif

	/* consecutive no. and current qualifier count up on */
	// this receipt will have a different consecutive number than the original
	// to allow for the Electronic Journal entry to also have a different number.
	// change made to allow VCS EJ Viewer application to display original and copies
	// from the electronic journal.
	ItemCountCons();

    /* get & set mnemonic */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UIPRINTDEMAND;
	RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_DEMAND_ADR);
    flDispRegDescrControl &= ~ TAXEXMPT_CNTRL;  /* correction10/13 */
    flDispRegKeepControl &= ~ TAXEXMPT_CNTRL;
    DispWrite( &RegDispMsgD );
    
    if ( TranCurQualLD.flPrintStatus & CURQUAL_POSTCHECK) { 
        TranCurQualLD.uchPrintStorage = PRT_CONSOLSTORAGE;    /* postcheck */
    } else { 
        TranCurQualLD.uchPrintStorage = PRT_ITEMSTORAGE;    /* precheck */
    }

    /* save print status, compulsory print status */
    fbSaveNoPrint = TranCurQualLD.fbNoPrint;
    fbSaveCompPrint = TranCurQualLD.fbCompPrint;
    uchSaveKitchenStorage = TranCurQualLD.uchKitchenStorage;

    /* set print status, compulsory print status */
    TranCurQualLD.uchKitchenStorage = PRT_NOSTORAGE;     /* Q-004 corr. 5/17 ,  kitchen not print*/
    TranCurQualLD.fbNoPrint = CURQUAL_NO_J + CURQUAL_NO_V + CURQUAL_NO_S;
    TranCurQualLD.fbCompPrint = CURQUAL_COMP_R;
    TrnPutCurQual( &TranCurQualLD );                                 

	{
		ITEMOTHER    TrnItemOther = {0};

		TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;   /* set class code */
		TrnItemOther.uchMinorClass = CLASS_PRINTDEMAND;
		TrnOther ( &TrnItemOther );
	}

    ItemOtherHeader( 0 ); /* execute header printing */ 
                   
    /* recover print status, compulsory print status */
    TranCurQualLD.uchKitchenStorage = uchSaveKitchenStorage;
    TranCurQualLD.fbNoPrint = fbSaveNoPrint;
    TranCurQualLD.fbCompPrint = fbSaveCompPrint;
    TrnPutCurQual( &TranCurQualLD );                                 

    /* correction10/08 */
    /* after pressing total, bit off curqual */
    ItemCommonCancelExempt();

    /* set E/C performed bit */
    ItemCommonPutStatus( COMMON_VOID_EC );  

    return(ITM_SUCCESS);
}                 

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemOtherHeader(SHORT sLine)
*                                     
*     Input:    sLine       --  no. of feed line
*    Output:    nothing 
*                                     
** Return:      nothing
*
** Description: ItemOtherHeader executes printing header. 
*===========================================================================
fhfh*/
VOID    ItemOtherHeader(SHORT sLine )
{
    ITEMOTHER TrnItemOther = {0};
                     
    TrnItemOther.uchMajorClass =  CLASS_ITEMOTHER;   /* set class code */    
    TrnItemOther.uchMinorClass =  CLASS_OUTSIDEFEED;    

    TrnItemOther.lAmount = sLine; 
    TrnItemOther.fsPrintStatus = PRT_RECEIPT;

    /* check electro or 24char print */
//  This check for MDC moved to function PrtOutFeed() so that we
//  are no longer using fsPrintStatus flag to indicate this specific
//  behavior any more.  Richard Chambers  09/12/2012
//    if ( CliParaMDCCheck( MDC_SLIP_ADR, EVEN_MDC_BIT3) != 0 ) {
//        TrnOther.fsPrintStatus |= PRT_HEADER24_PRINT;
//    }
                                
    TrnThrough( &TrnItemOther );

}
/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherSlipRel( VOID )
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: ItemOtherSlipRel executes slip release. 
*===========================================================================
fhfh*/

SHORT   ItemOtherSlipRel( VOID )   
{
    ITEMOTHER   TrnItemOther = {0};
    
    /* check buffering option or not */
    if ( (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER ) {
        return(LDT_SEQERR_ADR); /* w/ buffering option, set sequence error */      
    }

    /* check receipt use or slip use */
    if ( (TranGCFQualPtr->uchSlipLine == 0 ) && ( TranGCFQualPtr->uchPageNo == 0 ) ) {
        return(LDT_SEQERR_ADR); /* use receipt */      
    }

    TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;
    TrnItemOther.uchMinorClass = CLASS_SLIPRELEASE;
    TrnItemOther.lAmount = TranGCFQualPtr->uchSlipLine;
    TrnItemOther.fsPrintStatus = PRT_SLIP; /* set slip bit on other data */       

    TrnOther ( &TrnItemOther );

    ItemOtherLocal.fbOtherStatus |= OTHER_SLIP_RELEASED;

    return(ITM_SUCCESS);
}
/*fhfh      
*===========================================================================
** Synopsis:    SHORT   ItemOtherSlipFeed( UIFREGOTHER *ItemOther )
*                                     
*     Input:    ItemOther
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: ItemOtherSlipFeed executes slip feed
*===========================================================================
fhfh*/

SHORT   ItemOtherSlipFeed( UIFREGOTHER *ItemOther)
{
    ITEMOTHER   TrnItemOther = {0};
    
    /* check buffering option or not */
    if ( (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER ) {
        return(LDT_SEQERR_ADR); /* w/ buffering option, set sequence error */      
    }
                                     
    TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;   /* set class code */   
    TrnItemOther.uchMinorClass = CLASS_INSIDEFEED;
     
    if ( ItemOther->lAmount == 0L ) {
        TrnItemOther.lAmount = TranGCFQualPtr->uchSlipLine;    
    } else {
        /* check feedline */
        if ( (long)(RflGetSlipMaxLines() - 2) < ItemOther->lAmount) {
            return(LDT_KEYOVER_ADR); 
        } 
        TrnItemOther.lAmount = ItemOther->lAmount;      
    }
   
    TrnItemOther.fsPrintStatus = PRT_SLIP; /* set slip bit on other data */       

    TrnOther( &TrnItemOther );

    ItemOtherLocal.fbOtherStatus &= ~ OTHER_SLIP_RELEASED; /* reset slip bit on other local data */
    
    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemOtherRecFeed(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing
*
** Return:      nothing 
*
** Description: ItemOtherRecFeed executes receipt side feed
*===========================================================================
fhfh*/
VOID    ItemOtherRecFeed(VOID)
{
    if ( ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE ) {
		ITEMOTHER   TrnItemOther = {0};

        TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;
        TrnItemOther.uchMinorClass = CLASS_INSIDEFEED;
        TrnItemOther.lAmount = OTHER_RECEIPTFEED;
        TrnItemOther.fsPrintStatus = PRT_RECEIPT;        
        TrnOther ( &TrnItemOther );
    } else {
        ItemOtherHeader( OTHER_RECEIPTFEED ); /* execute header printing */                    
    }                             
}                 

/*fhfh
*===========================================================================
** Synopsis:    VOID    ItemOtherJouFeed(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: ItemOtherJouFeed executes journal side feed.
*===========================================================================
fhfh*/

VOID    ItemOtherJouFeed(VOID)
{
	ITEMOTHER   TrnItemOther = {0};

    TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;
    TrnItemOther.uchMinorClass = CLASS_INSIDEFEED;
    TrnItemOther.lAmount = OTHER_JOURNALFEED; /* set 8 line */
    TrnItemOther.fsPrintStatus = PRT_JOURNAL; /* set journal bit on other data */       
    TrnOther ( &TrnItemOther );
}                 

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECor(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS, UIF_CANCEL, LDT_SEQERR_ADR
*
** Description: ItemOtherECor executes "BACK TRACK FC TOTAL STATE(ItemOtherECFC)"
*               or "ERROR CORRECT OF ITEMIZE(ItemOtherEC)"          
*===========================================================================
fhfh*/

SHORT   ItemOtherECor(VOID)
{
    SHORT   sStatus = ITM_SUCCESS;
    
    if ( ItemModLocalPtr->fsTaxable != 0 ) {
        return(LDT_SEQERR_ADR); 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {  /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check "BACK TRACK FC TOTAL STATE" or  "E/C OF ITEMIZE",  Saratoga */
    if (ItemTenderLocalPtr->fbTenderStatus[2] & (TENDER_FC1 | TENDER_FC2 | TENDER_FC3 | TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8))
	{
        ItemOtherECFC();
        ItemCommonPutStatus( COMMON_VOID_EC );   /* correction10/06 */
    } else if (!(ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) && (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {
        sStatus = ItemOtherECFS();
    } else { 
        sStatus = ItemOtherEC();
    }

    return(sStatus);
}

/*
*===========================================================================
** Synopsis:    ItemOtherECFS(VOID)
*
*     Input:    None
*    Output:    None
*
** Return:
*
** Description: Back Track all FS total state
*===========================================================================
*/
SHORT   ItemOtherECFS(VOID)
{
    /* --- Second Food Stamp Tender, V2.01.04 --- */
    if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
        return(LDT_SEQERR_ADR);
    }
	switch (ItemTenderLocalPtr->ItemFSTender.uchMinorClass) {
		case CLASS_TENDER2:    // ItemOtherECFS() check that current tender is FS tender key
		case CLASS_TENDER11:   // ItemOtherECFS() check that current tender is FS tender key
			break;
		default:
			return(LDT_SEQERR_ADR);
			break;
	}

    return(ItemTendECFS());
}

/*fhfh
*===========================================================================
** Synopsis:    ItemOtherECFC(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      nothing
*
** Description: Back Track FC total state  
*===========================================================================
fhfh*/

VOID   ItemOtherECFC(VOID)
{
	ITEMTENDERLOCAL     * const pItemTenderLocalLD = ItemTenderGetLocalPointer();
	REGDISPMSG          RegDispMsgD = {0};
 
    /* reset tender1 and tender2 */
	pItemTenderLocalLD->fbTenderStatus[2] &= ~(TENDER_FC1 | TENDER_FC2 |
        TENDER_FC3 | TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8);

    /* check partial or check total */
    if (pItemTenderLocalLD->fbTenderStatus[0] & TENDER_PARTIAL) {
		DCURRENCY           lSubTendAmount = 0;
		UIFREGTENDER        UifTender = {0};

        /*----- Split Check,   R3.1 -----*/
        if (TrnSplCheckSplit() == TRN_SPL_SEAT
            || TrnSplCheckSplit() == TRN_SPL_MULTI
            || TrnSplCheckSplit() == TRN_SPL_TYPE2)
		{
            ItemTendCalAmountSpl(&lSubTendAmount, &UifTender);
        } else {
            ItemTendCalAmount(&lSubTendAmount, &UifTender);
        }

        lSubTendAmount += ItemTransLocalPtr->lWorkMI;              /* R3.0 */

		/*  set class code for display  */
        RegDispMsgD.uchMajorClass = CLASS_UIFREGTENDER;
        RegDispMsgD.uchMinorClass = CLASS_UITENDER1;
        RflGetLead ( RegDispMsgD.aszMnemonic, LDT_BD_ADR );
        RegDispMsgD.lAmount = lSubTendAmount; 
        RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_5;           /* set type5, DISP_SAVE_TYPE_5 */
        flDispRegDescrControl |= TENDER_CNTRL;   /* set descriptor*/
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT            /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {      /* PST exempt */

            flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt */
        }
    } else {
		ITEMCOMMONLOCAL     *pItemCommonL = ItemCommonGetLocalPointer();
		ITEMTOTAL           *pItemTotal;
		USHORT              usAddress;

        pItemTotal = (ITEMTOTAL *) &(pItemCommonL->ItemSales);

        RegDispMsgD.uchMajorClass = CLASS_UIFREGTOTAL;
        RegDispMsgD.uchMinorClass = CLASS_UITOTAL2;

        switch (pItemCommonL->ItemSales.uchMajorClass) {
        case CLASS_ITEMTOTAL:
            usAddress = RflChkTtlStandardAdr(pItemTotal->uchMinorClass);
            RegDispMsgD.lAmount = pItemTotal->lMI + ItemTransLocalPtr->lWorkMI;  /* R3.0 */
            break;

        default:
            RegDispMsgD.lAmount = ItemTendFCCTotal();     /* native balance */
            usAddress = TRN_TTL2_ADR;                     /* check total */
            break;
        }

        RflGetTranMnem ( RegDispMsgD.aszMnemonic, usAddress);
        RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_1;                   /* set type1 */
        flDispRegDescrControl |= TOTAL_CNTRL;                           /* set descriptor */

        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT            /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {      /* PST exempt */

            flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                    /* tax exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt */
        }
    }
                                                    
    DispWrite( &RegDispMsgD );                                          

    if (pItemTenderLocalLD->fbTenderStatus[0] & TENDER_PARTIAL) {
        flDispRegDescrControl &= ~TOTAL_CNTRL;                          /* reset descriptor */
    } else {
        flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXEXMPT_CNTRL );     /* reset descriptor */
    }
}

/*fhfh
*===========================================================================
* Synopsis:    SHORT   ItemOtherEC(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR, UIF_CANCEL
*
** Description: Error Correct of Itemize 
*               concletely check "sales" "dicount" "tender" or "modifier"
*===========================================================================
fhfh*/
SHORT   ItemOtherEC(VOID)
{
    SHORT           sStatus = ITM_SUCCESS;
    ITEMCOMMONLOCAL *pItemCommonL = ItemCommonGetLocalPointer();

    /* check E/C performed or not */
    if ( pItemCommonL->fbCommonStatus & COMMON_VOID_EC ) {
        return(LDT_SEQERR_ADR);
    }

    /* check E/C execute or not for charge posting */
    if ( pItemCommonL->ItemDisc.uchMajorClass == CLASS_ITEMDISC ) {
        if ( (ItemOtherECSPV() ) != ITM_SUCCESS ) {
            return( sStatus );
        }
        sStatus = ItemOtherECDisc(pItemCommonL); /* discount e/c */
    } else {
        switch( pItemCommonL->ItemSales.uchMajorClass ) {
        case CLASS_ITEMSALES: 
            if ( (ItemOtherECSPV() ) != ITM_SUCCESS ) {
                return( sStatus );
            }
            /** add 8/24'93 ** check SPV intervention */
            sStatus = ItemOtherECSales(pItemCommonL);   /* sales e/c        */
            break;  

        case CLASS_ITEMTENDER:  
            if ( (ItemOtherECSPV() ) != ITM_SUCCESS ) {
                return( sStatus );
            }
            /** add 8/24'93 ** check SPV intervention */
            sStatus = ItemOtherECTend(pItemCommonL);    /* tender e/c       */
            break;  

        case CLASS_ITEMDISC:  
            if ( (ItemOtherECSPV() ) != ITM_SUCCESS ) {
                return( sStatus );
            }
            /** add 8/24'93 ** check SPV intervention */
            sStatus = ItemOtherECDisc(pItemCommonL);    /* discount e/c     */
            break; 
 
        case CLASS_ITEMCOUPON:                          /* R3.0 */
            if ( (ItemOtherECSPV() ) != ITM_SUCCESS ) {
                return( sStatus );
            }
            sStatus = ItemOtherECCoupon(pItemCommonL);  /* coupon e/c     */
            break; 

        case CLASS_ITEMMISC:                            /* misc. E/C,   Saratoga */
            if ((ItemOtherECSPV()) != ITM_SUCCESS) {
                return(sStatus);
            }
            sStatus = ItemMiscMoneyEC((ITEMMISC *)(&pItemCommonL->ItemSales));
            break;

        default:
            return(LDT_SEQERR_ADR);
        }
    }

    /*  if good status, set E/C performed bit correction10/06 */
    if ( sStatus == ITM_SUCCESS) {
        ItemCommonPutStatus( COMMON_VOID_EC );  
    } else {
        return(sStatus);
    }

    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        MldECScrollWrite2(MLD_DRIVE_2);
    } else {
        MldECScrollWrite();
    }

    return(sStatus);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECSPV(VOID)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS,UIF_CANCEL
*
** Description: ItemOtherECSPV executes checking SPV intervention
*===========================================================================
fhfh*/

SHORT   ItemOtherECSPV(VOID)
{

    /* check SPV intervention */
    if ( CliParaMDCCheck( MDC_EC_ADR, ODD_MDC_BIT0) == 0 ) {
        return(ITM_SUCCESS);
    }
    /* check SPV INTERVENTION canceled or success */
    return(ItemSPVInt(LDT_SUPINTR_ADR));

}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECSales(ITEMCOMMONLOCAL *ItemCommonL)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS
*
** Description: ItemOtherECSales executes error correction of the sales item 
*===========================================================================
fhfh*/

SHORT   ItemOtherECSales(ITEMCOMMONLOCAL *ItemCommonL)
{
    DCURRENCY       lNoAmt = 0;
    SHORT           sStatus;
    ITEMSALESLOCAL  * const pItemSalesLocalLD = ItemSalesGetLocalPointer();
	REGDISPMSG      RegDispMsgD = {0};

    /* reset compulsory bit */
	pItemSalesLocalLD->fbSalesStatus &= ~ ( SALES_PM_COMPLSRY | SALES_COND_COMPLSRY | SALES_DISPENSER_SALES);

	sStatus = ItemSalesRemoveGiftCard (&ItemCommonL->ItemSales);

    /* cancel ppi price R3.1 2172 */
    ItemSalesCalcECCom( &(ItemCommonL->ItemSales));

    /* set amount */
    switch ( ItemCommonL->ItemSales.uchMinorClass ) {

    case CLASS_DEPTMODDISC:
    case CLASS_PLUMODDISC:
    case CLASS_SETMODDISC:
    case CLASS_OEPMODDISC:                  /* R3.0 */    
        /* get amount= lDiscountAmount  for DEPTMODDISK, PLUMODDISK, SETMODDISK*/
        lNoAmt = ItemCommonL->ItemSales.lDiscountAmount;
        break;  
    
    default:
#if 1
		lNoAmt = 0;
		ItemSalesSalesAmtCalc(&(ItemCommonL->ItemSales), &lNoAmt);
#else
		// C99 modernization project - replace commonly cloned code with function
        lNoAmt = ItemCommonL->ItemSales.lProduct;
         
        for ( i = 0 ; i < ItemCommonL->ItemSales.uchCondNo 
            + ItemCommonL->ItemSales.uchPrintModNo + ItemCommonL->ItemSales.uchChildNo; i++) {
            lNoAmt +=(LONG)( ( ItemCommonL->ItemSales.lQTY / 1000L) * (ItemCommonL->ItemSales.Condiment[i].lUnitPrice) ); 
        }
#endif
        break;  
    }

    /* display E/C */
    /* set and display mnemonic E/C,amount */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UIEC;
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_0; /* set type1 */
	RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_EC_ADR);
    RegDispMsgD.lAmount = -1 * lNoAmt;    /* set amount( - xx ) */ 
    flDispRegDescrControl |= EC_CNTRL;   /* set descriptor*/
    DispWrite( &RegDispMsgD );

    flDispRegDescrControl &= ~ EC_CNTRL;   /* reset descriptor*/

    ItemCommonDispECSubTotal(&(ItemCommonL->ItemSales)); /* R3.0 */
    
    /* send to enhanced kds, 2172 */
    ItemSendKds(&(ItemCommonL->ItemSales), 1);
    
    /* ----- send link plu to kds, 2172 ----- */
    ItemCommonL->ItemSales.fbModifier |= VOID_MODIFIER;
    ItemSalesLinkPLU(&(ItemCommonL->ItemSales), 2);
        
    /* reset sales data and set it to common local data */
    ItemCommonL->uchItemNo --;      /* decrement no. of buffer */
    memset( &(ItemCommonL->ItemSales), 0, sizeof(ITEMSALES)); 
    ItemCommonL->usSalesBuffSize = 0; 
    
    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECDisc(ITEMCOMMONLOCAL *ItemCommonL)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS,UIF_CANCEL      
*
** Description: ItemOtherECDisc executes E/C,                     R3.1
*===========================================================================
fhfh*/

SHORT   ItemOtherECDisc(ITEMCOMMONLOCAL *ItemCommonL)
{
	REGDISPMSG      RegDispMsgD = {0};

    if (ItemCommonL->ItemDisc.uchMajorClass != CLASS_ITEMDISC ) {
		ITEMDISC   *ItemDisc = (ITEMDISC *) &ItemCommonL->ItemSales;

        RegDispMsgD.lAmount = -1 * ItemDisc->lAmount;
        
		// indicate with sType == 1 that we are doing an Error Correct
        ItemCommonDiscStatus(ItemDisc, 1);
        ItemCommonDispECSubTotal(&(ItemCommonL->ItemSales));
    } else {
        RegDispMsgD.lAmount = -1 * ItemCommonL->ItemDisc.lAmount;
        ItemCommonDispECSubTotal(&(ItemCommonL->ItemDisc)); /* R3.0 */
    }

    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2)
	{
		TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
        MldDispSubTotal(MLD_TOTAL_2, WorkTI->lMI);
    } else {
		TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
        MldDispSubTotal(MLD_TOTAL_1, WorkTI->lMI);
    }

    /* set and display mnemonic E/C,amount */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UIEC;
    RflGetTranMnem ( RegDispMsgD.aszMnemonic, TRN_EC_ADR);
    flDispRegDescrControl |= EC_CNTRL;   /* set descriptor*/
    DispWrite( &RegDispMsgD );

    flDispRegDescrControl &= ~ EC_CNTRL;   /* reset descriptor*/

    /* decrement no. of buffered Item */
    ItemCommonL->uchItemNo --;

    if ( ItemCommonL->ItemDisc.uchMinorClass == CLASS_ITEMDISC1) {
 
        if ( ItemCommonL->ItemSales.uchMajorClass == CLASS_ITEMSALES ) {

            if ( ItemCommonL->ItemSales.uchMinorClass == CLASS_DEPTITEMDISC ) {
                ItemCommonL->ItemSales.uchMinorClass = CLASS_DEPT;
                /*----- R3.0 -----*/
                if (!(ItemCommonL->ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
                    ItemCommonL->ItemSales.fbStorageStatus &= ~NOT_ITEM_CONS;
                }
            } else if ( ItemCommonL->ItemSales.uchMinorClass == CLASS_PLUITEMDISC ) {
                ItemCommonL->ItemSales.uchMinorClass = CLASS_PLU;
                /*----- R3.0 -----*/
                if (!(ItemCommonL->ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
                    ItemCommonL->ItemSales.fbStorageStatus &= ~NOT_ITEM_CONS;
                }
            } else if ( ItemCommonL->ItemSales.uchMinorClass == CLASS_SETITEMDISC ) {
                ItemCommonL->ItemSales.uchMinorClass = CLASS_SETMENU;
                /*----- R3.0 -----*/
                ItemCommonL->ItemSales.fbStorageStatus &= ~NOT_ITEM_CONS;
            } else if ( ItemCommonL->ItemSales.uchMinorClass == CLASS_OEPITEMDISC) {
                ItemCommonL->ItemSales.uchMinorClass = CLASS_OEPPLU;
                /*----- R3.0 -----*/
                ItemCommonL->ItemSales.fbStorageStatus &= ~NOT_ITEM_CONS;
            }
        } 
    }
    
    /* send to enhanced kds, 2172 */
    if (ItemCommonL->ItemDisc.uchMajorClass == CLASS_ITEMDISC ) {
        ItemSendKds(&(ItemCommonL->ItemDisc), 1);
    } else {
        ItemSendKds(&(ItemCommonL->ItemSales), 1);
    }
    
    /* reset Item discount */
    if (ItemCommonL->ItemDisc.uchMajorClass == CLASS_ITEMDISC ) {
        memset(&ItemCommonL->ItemDisc, 0 , sizeof(ITEMDISC));
        ItemCommonL->usDiscBuffSize = 0;
    } else {                                     
        memset(&ItemCommonL->ItemSales, 0 , sizeof(ITEMSALES));
        ItemCommonL->usSalesBuffSize = 0;
    }

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECCoupon(ITEMCOMMONLOCAL *ItemCommonL)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS,UIF_CANCEL      
*
** Description: ItemOtherECCoupon executes error correction of the coupon item
*===========================================================================
fhfh*/
SHORT   ItemOtherECCoupon(ITEMCOMMONLOCAL *ItemCommonL)
{
	REGDISPMSG  RegDispMsgD = {0};
    ITEMCOUPON  *ItemCoupon = (ITEMCOUPON *) &ItemCommonL->ItemSales;

    /* get amount from  ItemSales */
    RegDispMsgD.lAmount = -1 * ItemCoupon->lAmount;        /* set amount( - xx ) */  

    /* display E/C */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UIEC;
    RflGetTranMnem ( RegDispMsgD.aszMnemonic, TRN_EC_ADR);
    flDispRegDescrControl |= EC_CNTRL;   /* set descriptor*/
    DispWrite( &RegDispMsgD );
    flDispRegDescrControl &= ~ EC_CNTRL;   /* reset descriptor*/

    ItemCommonDispECSubTotal(&(ItemCommonL->ItemSales)); /* R3.0 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&(ItemCommonL->ItemSales), 1);
    
    /* decrement no. of buffered Item and clear the coupon data */
    ItemCommonL->uchItemNo --;
    memset(&ItemCommonL->ItemSales, 0 , sizeof(ITEMSALES));
    ItemCommonL->usSalesBuffSize = 0;

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherECTend(ITEMCOMMONLOCAL *ItemCommonL)
*                                     
*     Input:    nothing
*    Output:    nothing 
*
**   Return:    ITM_SUCCESS
*
** Description: ItemOtherECTend executes error correction of partial tender 
*===========================================================================
fhfh*/

SHORT   ItemOtherECTend(ITEMCOMMONLOCAL *ItemCommonL)    
{
	return(LDT_SEQERR_ADR);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemOtherComputat(UIFREGOTHER *ItemOther)
*                                     
*     Input:    UIFREGOTHER *ItemOther
*    Output:    nothing 
*
** Return:      ITM_SUCCESS, LDT_SEQERR_ADR
*
** Description: ItemOtherComputat executes computation.
*===========================================================================
fhfh*/

SHORT   ItemOtherComputat( UIFREGOTHER *ItemOther )
{
    ITEMTENDERLOCAL  *pItemTenderLocalLD = ItemTenderGetLocalPointer();
	REGDISPMSG       RegDispMsgD = {0}, RegDispBack = {0};
    DCURRENCY        lAmtValue;
                                     
    /* correction10/08 */
    /* check pressing total key or not    R3.0 */
    if (ItemModLocalPtr->fsTaxable & MOD_CANADAEXEMPT) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check  SavedMI <= 0L or not */
    if (pItemTenderLocalLD->lSavedMI <= 0L) { 
        return(LDT_SEQERR_ADR);
    }

    /* check  SavedMI <= ItemOther->lAmount or not */
    lAmtValue = ItemOther->lAmount - pItemTenderLocalLD->lSavedMI;
    if (lAmtValue < 0) {
        return(LDT_KEYOVER_ADR);    /* correction10/06 E09->E08 */
    }

    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UICOMPUTATION;
    RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_REGCHG_ADR);
    RegDispMsgD.lAmount = lAmtValue;    /* set amount */  
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_2; /* set type2 */
    flDispRegDescrControl |= CHANGE_CNTRL;
    ItemTendDispIn(&RegDispBack);

    DispWriteSpecial( &RegDispMsgD, &RegDispBack);

    flDispRegDescrControl &= ~ CHANGE_CNTRL;

    /* reset sales data and set to common local data */
    pItemTenderLocalLD->lSavedMI = 0L; 

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT ItemOtehrEJReverse( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Edit and Print Electronic Journal File for Reverse Function.  
*===============================================================================
*/

SHORT ItemOtherEJReverse( VOID )
{
    return( RptEJReverse() );
}

/*
*=============================================================================
**  Synopsis: VOID ItemOtherScrollUp( VOID ) 
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
VOID    ItemOtherScrollUp( VOID )
{
    /* ---- consolidation by scroll up key at 2x20 R3.1 ----- */
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        if (ItemOtherScrollUpCons() != ITM_SUCCESS) {
            return;
        }
    }

    FDTUpArrow();
}

/*
*=============================================================================
**  Synopsis: VOID ItemOtherScrollDown( VOID ) 
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
VOID    ItemOtherScrollDown( VOID )
{
    /* ---- consolidation by scroll up key at 2x20 R3.1 ----- */
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        if (ItemOtherScrollUpCons() != ITM_SUCCESS) {
            return;
        }
    }

    FDTDownArrow();
}

/*
*=============================================================================
**  Synopsis: SHORT ItemOtherScrollUpCons( VOID ) 
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT    ItemOtherScrollUpCons( VOID )
{
    if ( !(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
        if (!(TrnInformationPtr->TranCurQual.fsCurStatus & CURQUAL_TRAY)) {
            return(LDT_PROHBT_ADR);
        }
    }
    if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) ||
        (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {  /* Saratoga */
        return(LDT_PROHBT_ADR);
    }

    if (MldQueryMoveCursor(MLD_SCROLL_1) == FALSE) {
		MLDTRANSDATA      WorkMLD;

        /* item consolidation */
        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));

        /* set current itemizetion */
        if (ItemCommonLocal.ItemSales.uchMajorClass) {
            MldScrollWrite(&ItemCommonLocal.ItemSales, MLD_NEW_ITEMIZE);
            if (ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMSALES) { 
                /* display noun mnemonic */
                if (ItemCommonLocal.ItemSales.uchCondNo + ItemCommonLocal.ItemSales.uchPrintModNo) {
                    MldScrollWrite(&ItemCommonLocal.ItemSales, MLD_UPDATE_ITEMIZE);
                }
            }

            if (ItemCommonLocal.ItemDisc.uchMajorClass) {
                MldScrollWrite(&ItemCommonLocal.ItemDisc, MLD_NEW_ITEMIZE);
            }
        }
    }
    return(ITM_SUCCESS);
}
/*
*=============================================================================
**  Synopsis: VOID ItemOtherLeftDisplay(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
VOID    ItemOtherLeftDisplay(VOID)
{
    MldSetCursor(MLD_SCROLL_1);
}

/*
*=============================================================================
**  Synopsis: SHORT   ItemOtherRightArrow(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description:    Right Arrow Key at SoftCheck, Store/Recall.
*===============================================================================
*/
SHORT   ItemOtherRightArrow(VOID)
{
    USHORT  usSystemType = RflGetSystemType();

    /*----- display condiments at 2x20 display -----*/
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        if (!(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
            return(ITM_SUCCESS);
        }
        if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) ||
            (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {  /* Saratoga */
            return(LDT_PROHBT_ADR);
        }

        MldMoveCursor(0);
        return(ITM_SUCCESS);
    }

    if (usSystemType == FLEX_STORE_RECALL) {
        switch (MldQueryCurScroll()) {
        case    MLD_SCROLL_1:
        case    MLD_SCROLL_2:
            FDTRightArrow();
            break;

        case    MLD_SCROLL_3:
            return(ItemOtherForward());

        default:
            break;
        }
    }
    else if (usSystemType == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
		/*----- SoftCheck System -----*/
		SHORT  sSplitCheck = TrnSplCheckSplit();

        if (sSplitCheck == TRN_SPL_BASE) {
            return(ITM_SUCCESS);
        }

        if (sSplitCheck == TRN_SPL_SEAT
            || sSplitCheck == TRN_SPL_MULTI
            || sSplitCheck == TRN_SPL_TYPE2) {
            switch (MldQueryCurScroll()) {
            case    MLD_SCROLL_1:
                MldMoveCursor(0);
                break;

            default:
                break;
            }
        } else if (sSplitCheck == TRN_SPL_SPLIT) {
            switch (MldQueryCurScroll()) {
            case    MLD_SCROLL_1:
            case    MLD_SCROLL_2:
                MldMoveCursor(0);
                break;

            case    MLD_SCROLL_3:
                return(ItemOtherSplForward());

            default:
                break;
            }
        }
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT  ItemOtherLeftArrow(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description:    Left Arrow Key at SoftCheck, Store/Recall.
*===============================================================================
*/
SHORT   ItemOtherLeftArrow(VOID)
{
    USHORT  usSystemType = RflGetSystemType();

    /*----- display condiments at 2x20 display -----*/
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        if (!(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
            return(ITM_SUCCESS);
        }
        if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) ||
            (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {  /* Saratoga */
            return(ITM_SUCCESS);
        }

        MldMoveCursor(1);
        return(ITM_SUCCESS);
    }

    if (usSystemType == FLEX_STORE_RECALL) {
        switch (MldQueryCurScroll()) {
        case    MLD_SCROLL_1:
            break;

        case    MLD_SCROLL_2:
            if (ItemOtherBackward() == FDT_SUCCESS) {
                FDTLeftArrow();
            }
            break;

        case    MLD_SCROLL_3:
            FDTLeftArrow();
            break;

        default:
            break;
        }
    }
    else if (usSystemType == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
		/*----- SoftCheck System -----*/
		SHORT  sSplitCheck = TrnSplCheckSplit();

        if (sSplitCheck == TRN_SPL_BASE) {
            return(ITM_SUCCESS);
        }

        if (sSplitCheck == TRN_SPL_SEAT
            || sSplitCheck == TRN_SPL_MULTI
            || sSplitCheck == TRN_SPL_TYPE2) {
            switch (MldQueryCurScroll()) {
            case    MLD_SCROLL_2:
                MldMoveCursor(1);
                break;

            default:
                break;
            }
        } else if (sSplitCheck == TRN_SPL_SPLIT) {
            switch (MldQueryCurScroll()) {
            case    MLD_SCROLL_1:
                break;

            case    MLD_SCROLL_2:
                return(ItemOtherSplBackward());

            case    MLD_SCROLL_3:
                MldMoveCursor(1);
                break;

            default:
                break;
            }
        }
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT ItemOtherForward(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherForward(VOID)
{
	FDTPARA     WorkFDTPara = { 0 };
    
    if (TranModeQualPtr->ulCashierID == 0) {
        return(LDT_SEQERR_ADR);
    }

	FDTParameter(&WorkFDTPara);
    if (WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_2) {
		FDTSTATUS        WorkFDT;

		FDTGetStatus(&WorkFDT);

        if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) || WorkFDT.uchAddonOrder != 0) {
            return(ITM_SUCCESS);
        } else {
            return(FDTForward());
        } 
    } else if (WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
        return(ITM_SUCCESS);

    } else {
        return(FDTForward());
    }
}

/*
*=============================================================================
**  Synopsis: SHORT   ItemOtherBackward(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherBackward(VOID)
{
	FDTPARA     WorkFDTPara = { 0 };
    
    if (TranModeQualPtr->ulCashierID == 0) {
        return(LDT_SEQERR_ADR);
    }

	FDTParameter(&WorkFDTPara);
    if (WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_2) {
		FDTSTATUS        WorkFDT = { 0 };

		FDTGetStatus(&WorkFDT);

        if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) || WorkFDT.uchAddonOrder != 0) {
            return(ITM_SUCCESS);
        } else {
            return(FDTBackward());
        } 
    } else if (WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
        return(ITM_SUCCESS);

    } else {
        return(FDTBackward());
    }
}

/*
*=============================================================================
**  Synopsis:   SHORT ItemOtherSplForward(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherSplForward(VOID)
{
    TRANINFORMATION *TrnInf;
    TRANINFSPLIT    *SplitA, *SplitB;
    
    if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL)
        || (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_SEAT_PARTIAL)
        || (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {  /* Saratoga */
        return(LDT_SEQERR_ADR);
    }

    /*----- Create New Split File -----*/
    if (TrnSplRightArrow() != TRN_SUCCESS) {
        return(ITM_SUCCESS);
    }

    TrnSplGetDispSeat(&TrnInf, &SplitA, &SplitB);

    MldDisplayInit(MLD_DRIVE_2 | MLD_DRIVE_3, 0);

    /*----- Display Seat#1 Transaction on LCD Middle -----*/
    if (SplitA->TranGCFQual.usGuestNo != 0) {
		MLDTRANSDATA    MldTran;

        MldTran.usVliSize = SplitA->usTranConsStoVli;
        MldTran.sFileHandle = SplitA->hsTranConsStorage;
        MldTran.sIndexHandle = SplitA->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);
        MldDispSubTotal(MLD_TOTAL_2, SplitA->TranItemizers.lMI);
    }

    /*----- Display Seat#1 Transaction on LCD Right -----*/
    if (SplitB->TranGCFQual.usGuestNo != 0) {
		MLDTRANSDATA    MldTran;

        MldTran.usVliSize = SplitB->usTranConsStoVli;
        MldTran.sFileHandle = SplitB->hsTranConsStorage;
        MldTran.sIndexHandle = SplitB->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_3, &MldTran);
        MldDispSubTotal(MLD_TOTAL_3, SplitB->TranItemizers.lMI);
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT ItemOtherSplBackward(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherSplBackward(VOID)
{
    TRANINFORMATION *TrnInf;
    TRANINFSPLIT    *SplitA, *SplitB;
    
    if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL)
        || (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_SEAT_PARTIAL)
        || (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) {  /* Saratoga */
        return(LDT_SEQERR_ADR);
    }

    /*----- Create New Split File -----*/
    if (TrnSplLeftArrow() != TRN_SUCCESS) {
        MldMoveCursor(1);
        return(ITM_SUCCESS);
    }

    TrnSplGetDispSeat(&TrnInf, &SplitA, &SplitB);

    MldDisplayInit(MLD_DRIVE_2 | MLD_DRIVE_3, 0);

    /*----- Display Base Transaction on LCD Middle -----*/
    if (SplitA->TranGCFQual.usGuestNo != 0) {
		MLDTRANSDATA    MldTran;

        MldTran.usVliSize = SplitA->usTranConsStoVli;
        MldTran.sFileHandle = SplitA->hsTranConsStorage;
        MldTran.sIndexHandle = SplitA->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);
        MldDispSubTotal(MLD_TOTAL_2, SplitA->TranItemizers.lMI);
    }

    /*----- Display Base Transaction on LCD Right -----*/
    if (SplitB->TranGCFQual.usGuestNo != 0) {
		MLDTRANSDATA    MldTran;

        MldTran.usVliSize = SplitB->usTranConsStoVli;
        MldTran.sFileHandle = SplitB->hsTranConsStorage;
        MldTran.sIndexHandle = SplitB->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_3, &MldTran);
        MldDispSubTotal(MLD_TOTAL_3, SplitB->TranItemizers.lMI);
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis: VOID ItemOtherWait( VOID ) 
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: Used 
*===============================================================================
*/
SHORT   ItemOtherWait( VOID )
{
	FDTSTATUS   WorkFDT = { 0 };
	FDTPARA     WorkFDTPara = { 0 };
    
    if (TranModeQualPtr->ulCashierID == 0) {
        return(LDT_SEQERR_ADR);
    }

    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);                 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) {
        return(LDT_SEQERR_ADR);
    }                             

	// if this terminal is not a Payment terminal of some kind then nothing
	// to do and its a sequence error.
	FDTParameter(&WorkFDTPara);
    if (WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_1
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_DELIV_2
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE
        || WorkFDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
        return(LDT_SEQERR_ADR);                 
    }

	FDTGetStatus(&WorkFDT);
    return(FDTWait(WorkFDT.uchPrevPayTrans, 0));
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherPaidRecall(UIFREGOTHER *pUifRegOther)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherPaidRecall(UIFREGOTHER *pUifRegOther)
{
    SHORT            sStatus;
	FDTSTATUS        WorkFDT;
    
    if (TranModeQualPtr->ulCashierID == 0) {
        return(LDT_SEQERR_ADR);
    }

    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);                 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) {
        return(LDT_SEQERR_ADR);
    }
    
	FDTGetStatus(&WorkFDT);

    if (pUifRegOther->lAmount == 0 && WorkFDT.uchGCFOrder == 0) {
		/*----- Paid Order Recall of Own Terminal -----*/
        sStatus = FDTPaidOrderRecallOwn();
    } else {
		/*----- Paid Order Recall from GCF Module -----*/
        sStatus = FDTPaidOrderRecallGCF(pUifRegOther->lAmount);
    }

    if (sStatus == FDT_SUCCESS) {
		REGDISPMSG      DispMsg = {0};

        DispMsg.uchMajorClass = CLASS_UIFREGOTHER;
        DispMsg.uchMinorClass = CLASS_UIPAIDORDERRECALL;
        RflGetTranMnem (DispMsg.aszMnemonic, TRN_PAIDRECALL_ADR);
        DispMsg.lAmount = 0L;
        DispMsg.fbSaveControl = DISP_SAVE_TYPE_2;
        DispWrite(&DispMsg);

        ItemMiscSetStatus(MISC_POSTRECPT | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
        TrnStoGetConsToPostR();

        TrnSetConsNoToPostR(MaintCurrentSpcCo(SPCO_CONSEC_ADR));
        ItemMiscResetStatus(MISC_PARKING);
    }

    return(sStatus);
}


/**********************************************************
	Description:  This function, ItemPreviousTotalRedo(), is used
	              to redo or fix up a total that is in the local buffer
				  when a Cursor Void is done.

                  The problem is that if a Total key press is pending
				  and a Cursor Void is done, then the ITEMTOTAL data
				  contains an older total which is now incorrect because
				  a Cursor Void is being done to void an item on the display.

                  This function should be called after the itemizers have
				  been updated so that the total can be recalculated with the
				  updated information.
**/
VOID  ItemPreviousTotalRedo ()
{
	if (ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
		TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
		UIFREGTOTAL     myUifRegTotal = {0};

		pWorkCur->auchTotalStatus[0] = 0;    // clear the current total key number and type which indicates that a total has been done.

		myUifRegTotal.uchMajorClass = ItemCommonLocal.ItemSales.uchMajorClass;
		myUifRegTotal.uchMinorClass = ItemCommonLocal.ItemSales.uchMinorClass;

		ItemTotal(&myUifRegTotal);
	}
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherCursorVoid(UIFREGOTHER *pData);
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description:    Cursol Void,                            R3.1
*===============================================================================
*/
SHORT   ItemOtherCursorVoid(UIFREGOTHER *pRegOther)
{
	DCURRENCY        lItemizerBuff = 0;
    ITEMCOMMONLOCAL  *pWorkCommon = ItemCommonGetLocalPointer();
    ITEMSALESLOCAL   *pWorkSales = ItemSalesGetLocalPointer();
    TRANGCFQUAL      *WorkGCF = TrnGetGCFQualPtr();
	ITEMSALES        ItemSales = {0};
    ITEMDISC         ItemDisc = {0};
	USHORT			 usCondNumber = 0;
    SHORT            sStatus, sSize, sQty;

	// check that the key press is appropriate for this type of transaction (Return or not Return).
	// If key press is Cursor Void and this is a return, then Prohibited.
	// If key press is a Cursor Return and this is not a return, then Prohibited.
	if (pRegOther->uchMinorClass == CLASS_UICURSORVOID) {
		if (WorkGCF->fsGCFStatus & GCFQUAL_TRETURN) {
			// check MDC to see if adding items to a Return Transaction is allowed
			if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_A) == 0)
				return LDT_PROHBT_ADR;
		}
	} else {
		// previously for Amtrak we allowed an item return/exchange/refund only if processing an actual return.
		// now we allow a transaction that has been serviced to also allow an item return/exchange/refund.
		if ((WorkGCF->fsGCFStatus & (GCFQUAL_SERVICE1 | GCFQUAL_SERVICE2 | GCFQUAL_SERVICE3)) == 0) {
			if ((WorkGCF->fsGCFStatus & GCFQUAL_TRETURN) == 0)
				return LDT_PROHBT_ADR;
		}
	}

    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);                 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /*----- compulsory status 4/23/96 ----- */
    if ((sStatus = ItemCommonCheckComp()) != ITM_SUCCESS ) {
        return (sStatus);
    }

    /*----- check split check mode  -----*/
    if (ItemCommonCheckSplit() != ITM_SUCCESS) {
        return(LDT_SEQERR_ADR);
    }

    /*----- check current active scroll -----*/
    if (MldQueryCurScroll() != MLD_SCROLL_1) {
        return(LDT_SEQERR_ADR);
    }

    if (pRegOther->lAmount) {
        if (pRegOther->lAmount % 1000L) {   /* check decimal qty entry */
            return(LDT_KEYOVER_ADR);
        }
    }

	ItemPreviousItem (0, 0);  // if there is anything in memory resident data buffer then lets flush it first.

    /*----- Get Void Item from LCD Cursor -----*/
    if (MldGetCursorDisplay(MLD_SCROLL_1, &ItemSales, &usCondNumber, MLD_GET_COND_NUM) != MLD_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

	if (ItemSales.uchMajorClass == CLASS_ITEMSALES && ItemSales.uchMinorClass == CLASS_ITEMORDERDEC) {
		// do not allow a Cursor Void or a Cursor Return of any sort to be used with Order Declare.
        return(LDT_PROHBT_ADR);
	}

	if (ItemSales.uchMajorClass != CLASS_ITEMTENDER) {
		if (pRegOther->uchFsc) {
			// there is balance due so do not allow anything other than a tip return.
			return(LDT_PROHBT_ADR);
		}
		if (pRegOther->uchMinorClass == CLASS_UICURSORVOID && (ItemSales.fbModifier & RETURNS_ORIGINAL) != 0) {
			// if cursor void on an item from the original transaction then prohibit the action.
			return(LDT_PROHBT_ADR);
		}
	} else  {
		if (pRegOther->uchMinorClass != CLASS_UICURSORVOID) {
			if ((TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) && ItemTenderGetSetTipReturn (-1)) {
				return(LDT_PROHBT_ADR);
			}
		}
	}

	if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY) {
		// if we are doing Tray Total then allow Cursor Void only on items entered
		// after the last Tray Total Key press.
		if (ItemSales.usUniqueID <= ItemTotalLocalPtr->usUniqueID) {
			return(LDT_PROHBT_ADR);
		}
	}

	// If the operator has not selected a condiment then lets get an associated discount, if any,
	// as well.  We will need this to handle the proper voiding of the item and its discount.
	MldGetCursorDisplay(MLD_SCROLL_1, NULL, &ItemDisc, MLD_GET_SALES_DISC);

    /*----- Send Previous Item to Transaction Module -----*/
	if((usCondNumber) && (ItemSales.uchCondNo) && (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)))
	{
		ItemPreviousCondiment(&ItemSales.Condiment[usCondNumber - 1],MLD_CONDIMENT_VOID);
		return(ITM_SUCCESS);
	} else if (pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMSALES || pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMCOUPON ||
				pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMDISC || pWorkCommon->ItemDisc.uchMajorClass == CLASS_ITEMDISC) {
        ItemPreviousItem(NULL, 0);           /* Saratoga */
    }

    /* ----- verify terget item status ---- */
    switch(ItemSales.uchMajorClass) {
    case    CLASS_ITEMSALES:
        if ((ItemSales.fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) == VOID_MODIFIER) {
            return(LDT_PROHBT_ADR);
        }

		if (pRegOther->uchMinorClass >= CLASS_UICURSORTRETURN1 && pRegOther->uchMinorClass <= CLASS_UICURSORTRETURN3) {
			if ((ItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) != 0) {
				return(LDT_PROHBT_ADR);
			}
		}

		if (ItemSalesLocal.fbSalesStatus & SALES_TRANSACTION_DISC) {
			// if operator has done a transaction discount then do
			// not allow any changes to transaction data.
			return(LDT_PROHBT_ADR);
		}

        if (ItemSales.uchMinorClass == CLASS_DEPTMODDISC ||
            ItemSales.uchMinorClass == CLASS_PLUMODDISC ||
            ItemSales.uchMinorClass == CLASS_SETMODDISC ||
            ItemSales.uchMinorClass == CLASS_OEPMODDISC ||     /* R3.0 */
            ItemSales.uchMinorClass == CLASS_DEPTITEMDISC ||
            ItemSales.uchMinorClass == CLASS_PLUITEMDISC ||
            ItemSales.uchMinorClass == CLASS_SETITEMDISC ||
            ItemSales.uchMinorClass == CLASS_OEPITEMDISC ||
            (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) )
		{
			// now allow for a partial quantity void of a quantity of an item with a discount.
			// requested by Amtrak.  However scalable items are still prohibited if amount entered.
            if ((ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) && pRegOther->lAmount) {
                return(LDT_PROHBT_ADR);
            }
			{
				if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN1) {
					ItemSales.fbModifier |= RETURNS_MODIFIER_1;
				}
				else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN2) {
					ItemSales.fbModifier |= RETURNS_MODIFIER_2;
				}
				else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN3) {
					ItemSales.fbModifier |= RETURNS_MODIFIER_3;
				}
			}
        }
        break;

    case    CLASS_ITEMDISC:
		{
			ITEMDISC  *pDisc = (ITEMDISC *)&ItemSales;

			if (pDisc->uchMinorClass == CLASS_ITEMDISC1) {
				return(LDT_PROHBT_ADR);
			}
			if (pDisc->fbDiscModifier & VOID_MODIFIER) {
				return(LDT_PROHBT_ADR);
			}
			if (pRegOther->lAmount) {
				return(LDT_PROHBT_ADR);
			}
			if (pDisc->uchSeatNo == 'B') {
				pDisc->uchSeatNo = 0;
			}
			if (pDisc->uchMinorClass != CLASS_ITEMDISC1) {
				// clear the rate if this is regular discount or charge tip so that we do not try
				// to recalculate the discount amount and do it incorrectly.  just use the amount
				// that is being voided.
				pDisc->uchRate = 0;
			}
		}
        break;

    case    CLASS_ITEMCOUPON:
		{
			ITEMCOUPON    *pCoupon = (ITEMCOUPON *)&ItemSales;

			if (ItemSalesLocal.fbSalesStatus & SALES_TRANSACTION_DISC) {
				// if operator has done a transaction discount then do
				// not allow any changes to transaction data.
				return(LDT_PROHBT_ADR);
			}

			if (pCoupon->fbModifier & VOID_MODIFIER) {
				return(LDT_PROHBT_ADR);
			}
			if (pRegOther->lAmount) {
				return(LDT_PROHBT_ADR);
			}
		}
        break;

    case    CLASS_ITEMTENDER:
		{
			ITEMTENDER   *pTender = (ITEMTENDER *)&ItemSales;

			if ((pTender->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) == VOID_MODIFIER) {
				return(LDT_PROHBT_ADR);
			}
			if (pRegOther->uchMinorClass >= CLASS_UICURSORTRETURN1 && pRegOther->uchMinorClass <= CLASS_UICURSORTRETURN3) {
				if ((pTender->fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) != 0) {
					return(LDT_PROHBT_ADR);
				}
			}

			if (pRegOther->uchMinorClass == CLASS_UICURSORVOID && (pTender->fbModifier & RETURNS_ORIGINAL) != 0) {
				return(LDT_PROHBT_ADR);
			}
			if (pTender->lGratuity == 0) {
				return(LDT_PROHBT_ADR);
			}
		}
		break;

    default:
        return(LDT_PROHBT_ADR);
    }

    /*----- Check Void Item Size -----*/
    sSize = 0;
    if ((sStatus = ItemCommonCheckSize(&ItemSales, sSize)) < 0) {
        return(LDT_TAKETL_ADR);
    }

    if ((!(pWorkSales->fbSalesStatus & SALES_ITEMIZE)) && (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
        /* print out header at tray transaction */
        ItemCurPrintStatus();                               /* set print status of current qualifier */
        ItemPromotion(PRT_RECEIPT, TYPE_STORAGE);   
        ItemHeader(TYPE_STORAGE);                           /* send header print data */
                                                            /* send transaction open data */
        pWorkSales->fbSalesStatus |= SALES_ITEMIZE; 
        ItemSalesCalcInitCom();                             /* ppi initialize r3.1 2172 */
    }

    /*----- Search Void Item -----*/
    if (ItemSales.uchMajorClass == CLASS_ITEMSALES) {
        if (pRegOther->lAmount == 0L) {
#if 0
	// RJC  test
            if (ItemSales.uchMinorClass == CLASS_DEPTMODDISC ||
                ItemSales.uchMinorClass == CLASS_PLUMODDISC ||
                ItemSales.uchMinorClass == CLASS_SETMODDISC ||
                ItemSales.uchMinorClass == CLASS_OEPMODDISC ||     /* R3.0 */
                ItemSales.uchMinorClass == CLASS_DEPTITEMDISC ||
                ItemSales.uchMinorClass == CLASS_PLUITEMDISC ||
                ItemSales.uchMinorClass == CLASS_SETITEMDISC ||
                ItemSales.uchMinorClass == CLASS_OEPITEMDISC ||
                (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) )
			{

                sQty = 1;
            } else {
                sQty = abs((SHORT)(ItemSales.lQTY / 1000L));
            }
#else
			if (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
				sQty = 1;
			} else {
				sQty = (SHORT)(ItemSales.lQTY / 1000L);
			}
#endif
        } else {
            sQty = (SHORT)(pRegOther->lAmount / 1000L);
        }
    } else {
        sQty = 1;
    }

	if (ItemSales.uchMajorClass != CLASS_ITEMTENDER) {
		USHORT    i;
		SHORT     sStorage = TRN_TYPE_ITEMSTORAGE;

		for (i = 0; i <= NUM_SEAT_CARD; i++)
		{
			if((WorkGCF->TrnGiftCard[i].aszNumber[0] != 0) &&
				(WorkGCF->TrnGiftCard[i].usCommStatus == GIFT_COMM_FAILURE))
			{
				memset(&WorkGCF->TrnGiftCard[i], 0, sizeof(WorkGCF->TrnGiftCard[i]));
			}
		}
		if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
			sStorage = TRN_TYPE_CONSSTORAGE;
		} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
			if (WorkGCF->usGuestNo) {
				sStorage = TRN_TYPE_CONSSTORAGE;
			} else {
				sStorage = TRN_TYPE_ITEMSTORAGE;
			}
		} else {
			sStorage = TRN_TYPE_ITEMSTORAGE;
		}

		if ((sStatus = TrnVoidSearch(&ItemSales, sQty, sStorage)) != TRN_SUCCESS) {
			return(sStatus);
		}
		if (ItemDisc.uchMajorClass != 0) {
			if ((sStatus = TrnVoidSearch(&ItemDisc, sQty, sStorage)) != TRN_SUCCESS) {
				return(sStatus);
			}
		}
	}

	// Check to see if this return type has been selected for Supervisor Intervention.  If so then we will
	// do the Supervisor Intervention step before continuing with processing the return
	// if this kind of a return has already been done then assume that Supervisor Intervention is not required.
	{
		ULONG  fbModifier = TrnQualModifierReturnTypeTest ();

		if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN1) {
			if ((fbModifier & RETURNS_MODIFIER_1) == 0 && CliParaMDCCheckField(MDC_RETURNS_RTN1_ADR, MDC_PARAM_BIT_D)) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}
		else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN2) {
			if ((fbModifier & RETURNS_MODIFIER_2) == 0 && CliParaMDCCheckField(MDC_RETURNS_RTN2_ADR, MDC_PARAM_BIT_D)) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}
		else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN3) {
			if ((fbModifier & RETURNS_MODIFIER_3) == 0 && CliParaMDCCheckField(MDC_RETURNS_RTN3_ADR, MDC_PARAM_BIT_D)) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}
	}

	/*----- Void Action -----*/
    switch(ItemSales.uchMajorClass) {
    case    CLASS_ITEMSALES:
		ItemSales.sCouponQTY = sQty;      // make sure that coupon quantity is set for an item that is being cursor voided.
        if (pRegOther->lAmount) {
            ItemSales.lQTY = pRegOther->lAmount;
            if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                ItemSales.lQTY *= -1;
				ItemSales.sCouponQTY *= -1;
#if 0
				//if we are doing a cursor void, if there is an amount, we only need to
				//subtract the amount that the user has voided. JHHJ SR 861
				ItemSales.lProduct = (ItemSales.lUnitPrice * (ItemSales.lQTY/1000));
#endif
            }
        }
        /* reset reduce status set by cursor item discount, V3.3 */
        if ((ItemDisc.uchMajorClass == CLASS_ITEMDISC) && (ItemDisc.uchMinorClass == CLASS_ITEMDISC1)) {
			// set the quantity for the number of items discounted and also recalculate the
			// amount in case this is a fixed discount amount based on the quantity being voided/returned.
			ItemDisc.lAmount = (ItemDisc.lAmount * sQty) / abs(ItemDisc.lQTY);
			ItemDisc.lQTY = sQty;
            if ((ItemDisc.fbReduceStatus & REDUCE_NOT_HOURLY) == 0) {
                ItemSales.fbReduceStatus &= ~REDUCE_NOT_HOURLY;
            }
        }

		if (pRegOther->uchMinorClass == CLASS_UICURSORVOID) {
			if ((sStatus = ItemSalesCursorVoid(&ItemSales, sQty)) != ITM_SUCCESS) {
				return(sStatus);
			}

			// Item discount may be either a percentage off or a fixed amount off.
			// If the item discount is a percentage off then the member uchRate of the ITEMDISC struct will be non-zero.
			// If the item discount is a fixed amount off then the member uchRate of the ITEMDISC struct will be zero.
			// The function ItemDiscCalculation() calculates the amount based on the percentage if there is a rate
			// so it needs to know what the item price is.  If this is a fixed amount discount, the function
			// will ignore this value.
			lItemizerBuff = ItemSales.lProduct;
		} else {
			ULONG  ulCurrentModifier = TrnQualModifierReturnTypeTest();

			if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN1) {
				// now check the MDC settings for mixing return types against our current
				// state so far as return types already processed.
				if (CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_C) && (ulCurrentModifier & ~RETURNS_MODIFIER_1) != 0) {
					return LDT_BLOCKEDBYMDC_ADR;
				}
				if ((ulCurrentModifier & ~RETURNS_MODIFIER_1) != 0) {
					if ((ulCurrentModifier & RETURNS_MODIFIER_2) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_B)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
					if ((ulCurrentModifier & RETURNS_MODIFIER_3) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_A)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
				}
				ItemSales.fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_1);
                ItemSales.fbReduceStatus |= REDUCE_RETURNED;
				ItemSales.usReasonCode = 1;
			}
			else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN2) {
				// now check the MDC settings for mixing return types against our current
				// state so far as return types already processed.
				if (CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_B) && (ulCurrentModifier & ~RETURNS_MODIFIER_2) != 0) {
					return LDT_BLOCKEDBYMDC_ADR;
				}
				if ((ulCurrentModifier & ~RETURNS_MODIFIER_2) != 0) {
					if ((ulCurrentModifier & RETURNS_MODIFIER_1) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_C)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
					if ((ulCurrentModifier & RETURNS_MODIFIER_3) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_A)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
				}
				ItemSales.fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_2);
                ItemSales.fbReduceStatus |= REDUCE_RETURNED;
				ItemSales.usReasonCode = 2;
			}
			else if (pRegOther->uchMinorClass == CLASS_UICURSORTRETURN3) {
				// now check the MDC settings for mixing return types against our current
				// state so far as return types already processed.
				if (CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_A) && (ulCurrentModifier & ~RETURNS_MODIFIER_3) != 0) {
					return LDT_BLOCKEDBYMDC_ADR;
				}
				if ((ulCurrentModifier & ~RETURNS_MODIFIER_3) != 0) {
					if ((ulCurrentModifier & RETURNS_MODIFIER_1) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_C)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
					if ((ulCurrentModifier & RETURNS_MODIFIER_2) && CliParaMDCCheckField(MDC_RTN_RECALL_RULES, MDC_PARAM_BIT_B)) {
						return LDT_BLOCKEDBYMDC_ADR;
					}
				}
				ItemSales.fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_3);
                ItemSales.fbReduceStatus |= REDUCE_RETURNED;
				ItemSales.usReasonCode = 3;
			}
			else {
				NHPOS_ASSERT(pRegOther->uchMinorClass == CLASS_UICURSORTRETURN1);
			}

			/*----- SUPERVISOR INTERVENTION -----*/
			if ((sStatus = ItemSalesSpvInt(&ItemSales)) != ITM_SUCCESS) {
				// Supervisor intervention did not work so turn off the void modifier.
				ItemSales.fbModifier &= ~(VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3);
				return(sStatus);
			}

			if ((sStatus = ItemSalesCursorVoidReturns(&ItemSales, sQty)) != ITM_SUCCESS) {
				return(sStatus);
			}
			{
				ITEMSALESLOCAL *pItemSalesLocal = ItemSalesGetLocalPointer ();

				pItemSalesLocal->fbSalesStatus |= SALES_TRETURN_ITEM;   // indicate there is at least one item being returned.
			}
		}

		// Item discount may be either a percentage off or a fixed amount off.
		// If the item discount is a percentage off then the member uchRate of the ITEMDISC struct will be non-zero.
		// If the item discount is a fixed amount off then the member uchRate of the ITEMDISC struct will be zero.
		// The function ItemDiscCalculation() calculates the amount based on the percentage if there is a rate
		// so it needs to know what the item price is.  If this is a fixed amount discount, the function
		// will ignore this value.
		lItemizerBuff = ItemSales.lProduct;
        
		ItemSales.fbModifier &= ~RETURNS_ORIGINAL;  // New item so ensure original marker is off if this is a return
		ItemSales.fsLabelStatus &= ~ITM_CONTROL_RETURNORIG;
		TrnQualModifierReturnTypeSet(ItemSales.fbModifier);

		/*----- Void Action for Item Discount -----*/
		ItemDisc.usReasonCode = ItemSales.usReasonCode;               // the item discount reason code is the same as the item sales reason code
		ItemDisc.fsPrintStatus |= (ItemSales.fsPrintStatus & 0x0f);   // the item discount has the same print settings as the item sales, e.g. EJ, slip, receipt

		if (ItemDisc.uchMajorClass == CLASS_ITEMDISC) {
			// handle cursor returns with discounts if this is a cursor return
			switch (pRegOther->uchMinorClass) {
				case CLASS_UICURSORTRETURN1:
					ItemDisc.fbDiscModifier |= (RETURNS_MODIFIER_1 | RETURNS_TRANINDIC);
					ItemDisc.fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN2:
					ItemDisc.fbDiscModifier |= (RETURNS_MODIFIER_2 | RETURNS_TRANINDIC);
					ItemDisc.fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN3:
					ItemDisc.fbDiscModifier |= (RETURNS_MODIFIER_3 | RETURNS_TRANINDIC);
					ItemDisc.fbReduceStatus |= REDUCE_RETURNED;
					break;
				default:
					ItemDisc.fbDiscModifier ^= VOID_MODIFIER;
					break;
			}
			ItemDisc.fbDiscModifier &= ~RETURNS_ORIGINAL;  // New item so ensure original marker is off if this is a return
			if ((sStatus = ItemDiscCalculation(&ItemDisc, &lItemizerBuff)) != ITM_SUCCESS) {
				return(sStatus);
			}
			if ((sStatus = ItemDiscCursorVoid(&ItemDisc)) != ITM_SUCCESS) {
				return(sStatus);
			}
//			TrnStorage (&ItemSales);
//			TrnStorage (&ItemDisc);
			TrnSalesAndDisc ( &ItemSales, &ItemDisc );
			// ensure that there is no data let in the memory resident area that might get flushed to transaction storage
			// we have seen cases of discount information remaining resulting in an extra item discount record
			// added to the transaction file when doing cursor void of an item with a discount.
			ItemPreviousItemClearStore (2, 0, 0);
		} else {
			TrnStorage (&ItemSales);
		}
        break;

    case    CLASS_ITEMDISC:
		{
			ITEMDISC *itemSalesDisc = (ITEMDISC *)&ItemSales;

			if (WorkGCF->fsGCFStatus & GCFQUAL_TRETURN) {
				itemSalesDisc->fbDiscModifier |= RETURNS_TRANINDIC;  // New item so mark it as a return
			}
			itemSalesDisc->fbDiscModifier &= ~RETURNS_ORIGINAL;  // New item so ensure original marker is off if this is a return
			// handle cursor returns with discounts if this is a cursor return
			switch (pRegOther->uchMinorClass) {
				case CLASS_UICURSORTRETURN1:
					itemSalesDisc->fbDiscModifier |= RETURNS_MODIFIER_1;
					itemSalesDisc->fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN2:
					itemSalesDisc->fbDiscModifier |= RETURNS_MODIFIER_2;
					itemSalesDisc->fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN3:
					itemSalesDisc->fbDiscModifier |= RETURNS_MODIFIER_3;
					itemSalesDisc->fbReduceStatus |= REDUCE_RETURNED;
					break;
				default:
					itemSalesDisc->fbDiscModifier |= VOID_MODIFIER;      // Mark the discount as a void.
					break;
			}

			if ((sStatus = ItemDiscCalculation(itemSalesDisc, &lItemizerBuff)) != ITM_SUCCESS) {
				return(sStatus);
			}
			if ((sStatus = ItemDiscCursorVoid(itemSalesDisc)) != ITM_SUCCESS) {
				return(sStatus);
			}
			itemSalesDisc->fsPrintStatus |= (PRT_RECEIPT | PRT_JOURNAL);  // ensure printing to receipt and journal is enabled.
			TrnItem (itemSalesDisc);
			// ensure that there is no data let in the memory resident area that might get flushed to transaction storage
			// we have seen cases of discount information remaining resulting in an extra item discount record
			// added to the transaction file when doing cursor void of an item with a discount.
			ItemPreviousItemClearStore (2, 0, 0);
		}
        break;

    case    CLASS_ITEMCOUPON:
		{
			ITEMCOUPON *itemSalesCoupon = (ITEMCOUPON *)&ItemSales;

			itemSalesCoupon->fbModifier &= ~RETURNS_ORIGINAL;  // New item so ensure original marker is off if this is a return
			itemSalesCoupon->fsPrintStatus |= (PRT_RECEIPT | PRT_JOURNAL);  // ensure printing to receipt and journal is enabled.
			switch (pRegOther->uchMinorClass) {
				case CLASS_UICURSORTRETURN1:
					itemSalesCoupon->fbModifier |= RETURNS_MODIFIER_1;
					itemSalesCoupon->fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN2:
					itemSalesCoupon->fbModifier |= RETURNS_MODIFIER_2;
					itemSalesCoupon->fbReduceStatus |= REDUCE_RETURNED;
					break;
				case CLASS_UICURSORTRETURN3:
					itemSalesCoupon->fbModifier |= RETURNS_MODIFIER_3;
					itemSalesCoupon->fbReduceStatus |= REDUCE_RETURNED;
					break;
				default:
					itemSalesCoupon->fbModifier |= VOID_MODIFIER;      // Mark the discount as a void.
					break;
			}
			if ((sStatus = ItemCouponCursorVoid(itemSalesCoupon)) != ITM_SUCCESS) {
				return(sStatus);
			}
		}
        break;

    case    CLASS_ITEMTENDER:
		{
			ITEMTENDER      *pTender = (ITEMTENDER *)&ItemSales;
			TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
			ITEMSALESLOCAL  *pItemSalesLocal = ItemSalesGetLocalPointer ();

			if (pRegOther->uchMinorClass == CLASS_UICURSORVOID) {
				if ((pItemSalesLocal->fbSalesStatus | SALES_RETURN_TIPS) == 0) {
					return(LDT_PROHBT_ADR);
				}

				pTender->lTenderAmount *= -1;
				pTender->lBalanceDue *= -1;
				pTender->lChange *= -1;
				pTender->fbReduceStatus = 0;
				pTender->lGratuity *= -1;
				pTender->fsPrintStatus = (PRT_JOURNAL | PRT_RECEIPT);    // print the charge tips return in the Electronic Journal
				ITEMTENDER_CHARGETIPS(pTender).lGratuity = pTender->lGratuity;
				memset (pTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], 0, sizeof(pTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]));
				if ((sStatus = ItemSTenderCursorVoidReturns(pTender)) != ITM_SUCCESS) {
					return(sStatus);
				}
				TrnStoTender(pTender);
				pItemSalesLocal->fbSalesStatus &= ~(SALES_RETURN_TIPS);
			} else {
				if ((sStatus = TrnVoidSearchNonSalesItem((ITEMGENERICHEADER *)pTender, TRN_TYPE_CONSSTORAGE)) == ITM_SUCCESS) {
					return(LDT_PROHBT_ADR);
				}

				ITEMTENDER_CHARGETIPS(pTender).uchMinorTenderClass = pTender->uchMinorClass;
				pTender->uchMinorClass = CLASS_TEND_TIPS_RETURN;
				pTender->lTenderAmount *= -1;
				pTender->lBalanceDue *= -1;
				pTender->lChange *= -1;
				pTender->fbReduceStatus = 0;
				pTender->lGratuity *= -1;
				pTender->fsPrintStatus = (PRT_JOURNAL | PRT_RECEIPT);    // print the charge tips return in the Electronic Journal
				ITEMTENDER_CHARGETIPS(pTender).lGratuity = pTender->lGratuity;
				memset (pTender->aszCPMsgText, 0, sizeof(pTender->aszCPMsgText) - sizeof(pTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]));
				pTender->fbModifier &= ~RETURNS_ORIGINAL;  // New item so ensure original marker is off if this is a return
				switch (pRegOther->uchMinorClass) {
					case CLASS_UICURSORTRETURN1:
						pTender->fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_1);
						pTender->fbReduceStatus |= REDUCE_RETURNED;
						pTender->usReasonCode = 1;
						break;
					case CLASS_UICURSORTRETURN2:
						pTender->fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_2);
						pTender->fbReduceStatus |= REDUCE_RETURNED;
						pTender->usReasonCode = 2;
						break;
					case CLASS_UICURSORTRETURN3:
						pTender->fbModifier |= (VOID_MODIFIER | RETURNS_MODIFIER_3);
						pTender->fbReduceStatus |= REDUCE_RETURNED;
						pTender->usReasonCode = 3;
						break;
					default:
						return(LDT_PROHBT_ADR);    // do not allow a curxor void to work with charge tips.  only returns.
						break;
				}
				ItemSTenderCursorVoidReturns(pTender);

				TrnStoTender(pTender);
				// check to see if this is a Return transaction and if so then
				// at least one item has been marked as a return with a reason code.
				pItemSalesLocal->fbSalesStatus |= (SALES_TRETURN_ITEM | SALES_RETURN_TIPS);
				if (pWorkGCF->fsGCFStatus & GCFQUAL_TRETURN) {
					pItemSalesLocal->fbSalesStatus |= (SALES_TRETURN_ITEM | SALES_RETURN_TIPS);
				}
			}
		}
        break;

    default:
        return(LDT_PROHBT_ADR);
    }

    /*----- SET E.C DISABLE STATUS -----*/
    pWorkCommon->fbCommonStatus |= COMMON_VOID_EC;    /* turn on disable E.C bit */

	if (pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
		ItemPreviousTotalRedo ();
	}

    return(ITM_SUCCESS);
}


/*==========================================================================
**  Synopsis:   SHORT   ItemOtherCurVoidRedisplay( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description: Added as a part of US Customs SCER, this function redisplays
*				all scroll data, ensuring that the mld display matches the
*				stored information.  After US Customs SCER adjusted display of
*				#/type entries, cursor voids caused a discrepancy between the
*				mld display and the stored information.  Basically, the cursor
*				void process updated the mld one way, and the storage another.
*				This function rewrites the mld display with the transaction
*				information from storage.
==========================================================================*/

SHORT   ItemOtherCurVoidRedisplay( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
	REGDISPMSG  DispMsg  = {0};
    SHORT   sSize;
    USHORT  usSize;

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                 /* major class */
    ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;      /* minor class */

    _tcsncpy( &ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], NUM_NUMBER );

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                  /* print journal */
    
    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    sSize = ItemCommonCheckSize( ItemTotal, usSize );           /* storage size */

    ItemPreviousItem( ItemTotal, ( USHORT )sSize );             /* buffering */

    if ( ItemTotal->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {      /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;                  /* print validation */ 
        TrnThrough( ItemTotal );
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.lAmount = ItemTotal->lMI + ItemTransLocalPtr->lWorkMI;       /* display amount */
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifRegTotal->uchMinorClass));
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_1;                           /* save for redisplay */

    flDispRegDescrControl |= TOTAL_CNTRL;   

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                /* GST exempt */
        || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {          /* PST exempt */

        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                /* tax exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
    }
                                                                /* R3.0 */
    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    ItemCommonDispSubTotal (NULL);

    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) ||     /* Display by Prnt Priority */
        (MldGetMldStatus() == FALSE)) {                         /* 2x20 scroll, V3.3 */

//        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
			MldRedisplayToScroll (MLD_SCROLL_1);
    }

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL ); 
                                                                /* R3.1 */
    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);    // update CurQual with new total status.
    
    TrnThroughDisp( ItemTotal );                                /* send total to KDS R3.1 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    return( ITM_SUCCESS );
}


/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherTransfer(UIFREGOTHER *pData)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: Handle transfering an item from one transaction to another using
*                the FSC_TRANSFER key.
*===============================================================================
*/
SHORT   ItemOtherTransfer(UIFREGOTHER *pRegOther)
{
    UCHAR        uchSeat;
    SHORT        sStatus, sQty;
    USHORT       usScroll;
    MLDTRANSDATA        MldTran;
    ITEMDATASIZEUNION   WorkSales;
    ITEMDISC            ItemDisc = {0};

    /*----- Error Check -----*/
    if ((sStatus = ItemOtherTransferCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (TrnSplCheckSplit() != TRN_SPL_TYPE2) {
        return(LDT_SEQERR_ADR);
    }

    /*----- Check Active LCD -----*/
    usScroll = MldQueryCurScroll();
    if (usScroll != MLD_SCROLL_1) {
        return(LDT_SEQERR_ADR);
    }

    /*----- Read Cursor Position Item -----*/
    if (MldGetCursorDisplay(usScroll, &WorkSales.ItemSalesBuff, &ItemDisc, MLD_GET_SALES_DISC) != MLD_SUCCESS) {
        return(LDT_SEQERR_ADR);
    }

	if (WorkSales.ItemHeader.uchMajorClass == CLASS_ITEMDISC) {
        if (WorkSales.ItemHeader.uchMinorClass == CLASS_ITEMDISC1) {
            return(LDT_PROHBT_ADR);     /* not transfer if item disc. is directed */
        }
    }
	
	if (WorkSales.ItemHeader.uchMajorClass == CLASS_ITEMSALES) {
        if (WorkSales.ItemSalesBuff.uchSeatNo == 'B') {
			WorkSales.ItemSalesBuff.uchSeatNo = 0;
		}
	}

	/*----- Check/Decide Transfer Qty -----*/
	if (ItemOtherTransferQty(pRegOther, &WorkSales.ItemSalesBuff, &sQty) != ITM_SUCCESS) {
		return(LDT_PROHBT_ADR);
	}

	if (TrnTransferSearch(&WorkSales, sQty, TRN_TYPE_CONSSTORAGE) != TRN_SUCCESS) {
		return(LDT_NTINFL_ADR);
	}

    if (ItemDisc.uchMajorClass != 0 && TrnTransferSearch(&ItemDisc, sQty, TRN_TYPE_CONSSTORAGE) != TRN_SUCCESS) {
        return(LDT_NTINFL_ADR);
    }

    if (ItemDisc.uchMajorClass != 0) {
		if (ItemDisc.uchMajorClass == CLASS_ITEMDISC) {
            if (sQty > 1) {
                return(LDT_PROHBT_ADR);
            }
		}
	}

    /*----- Check Input QTY -----*/
    uchSeat = (UCHAR)TrnInfSplPtrSplitA->TranGCFQual.usGuestNo;
    switch(WorkSales.ItemHeader.uchMajorClass) {
    case    CLASS_ITEMSALES:
        /* ----- calculate transfer qty and price */
        if ((WorkSales.ItemSalesBuff.ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            WorkSales.ItemSalesBuff.lQTY = ((LONG)sQty * 1000L);
            if (WorkSales.ItemSalesBuff.uchPPICode) { /* calculate ppi price */
                if (pRegOther->lAmount == 0) {
                    ;
                } else if (ItemSalesOnlyCalcPPI(&WorkSales.ItemSalesBuff) != ITM_SUCCESS) {
                    WorkSales.ItemSalesBuff.lProduct = (DCURRENCY)(WorkSales.ItemSalesBuff.lUnitPrice * (LONG)sQty);
                } else {    /* ppi sales price */
                    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                        WorkSales.ItemSalesBuff.lProduct *= -1L;
                    }
                }
            } else if (WorkSales.ItemSalesBuff.uchPM) { /* calculate price multiple 2172 */
                if (pRegOther->lAmount == 0) {
                    ;
                } else if (ItemSalesOnlyCalcPM(&WorkSales.ItemSalesBuff) != ITM_SUCCESS) {
                    WorkSales.ItemSalesBuff.lProduct = (DCURRENCY)(WorkSales.ItemSalesBuff.lUnitPrice * (LONG)sQty);
                } else {    /* ppi sales price */
                    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                        WorkSales.ItemSalesBuff.lProduct *= -1L;
                    }
                }
            } else {
                WorkSales.ItemSalesBuff.lProduct = (DCURRENCY)(WorkSales.ItemSalesBuff.lUnitPrice * (LONG)sQty);
            }
        }

        TrnSplReduce(&WorkSales.ItemSalesBuff);
        WorkSales.ItemSalesBuff.uchSeatNo = uchSeat;
        WorkSales.ItemSalesBuff.sCouponQTY = sQty;
        TrnItemSpl(&WorkSales.ItemSalesBuff, TRN_TYPE_SPLITA);
        break;

    case    CLASS_ITEMCOUPON:
		TrnSplReduce(&WorkSales.ItemCouponBuff);
        WorkSales.ItemCouponBuff.uchSeatNo = uchSeat;
        TrnItemSpl(&WorkSales.ItemCouponBuff, TRN_TYPE_SPLITA);
        break;

    default:
        return(LDT_SEQERR_ADR);
    }

    /*----- with Item Discount -----*/
    if (ItemDisc.uchMajorClass != 0) {
        switch(ItemDisc.uchMajorClass) {
        case    CLASS_ITEMDISC:
            if (sQty > 1) {
                return(LDT_PROHBT_ADR);
            }
            TrnSplReduce(&ItemDisc);
            ItemDisc.uchSeatNo = uchSeat;
            TrnItemSpl(&ItemDisc, TRN_TYPE_SPLITA);
            break;

        default:
            return(LDT_SEQERR_ADR);
        }
    }

    /*----- Display -----*/
    MldDisplayInit(MLD_DRIVE_1, 0);
    MldDisplayInit(MLD_DRIVE_2, 1);

    /*----- LCD Left -----*/
    MldTran.usVliSize = TrnInformationPtr->usTranConsStoVli;
    MldTran.sFileHandle = TrnInformationPtr->hsTranConsStorage;
    MldTran.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
    MldPutSplitTransToScroll(MLD_SCROLL_1, &MldTran);
    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);

    /*----- LCD Middle -----*/
    MldTran.usVliSize = TrnInfSplPtrSplitA->usTranConsStoVli;
    MldTran.sFileHandle = TrnInfSplPtrSplitA->hsTranConsStorage;
    MldTran.sIndexHandle = TrnInfSplPtrSplitA->hsTranConsIndex;
    MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);
    MldDispSubTotal(MLD_TOTAL_2, TrnTISplPtrSplitA->lMI);

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherTransferQty(UIFREGOTHER *pData,
*                                           ITEMSALES *pItem, SHORT *sQty)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherTransferQty(UIFREGOTHER *pData, ITEMSALES *pItem, SHORT *sQty)
{
    SHORT           sSign = 1;
    ITEMDISC        *pDisc = (ITEMDISC *)pItem;
    ITEMCOUPON      *pCpn = (ITEMCOUPON *)pItem;

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        sSign *= -1;
    }

    switch(pItem->uchMajorClass) {
    case    CLASS_ITEMSALES:
        /*----- Check Void ----*/
        if (pItem->fbModifier & VOID_MODIFIER) {
            return(LDT_PROHBT_ADR);
        }

        /*----- Check Modifier Discount -----*/
        if (pItem->uchMinorClass == CLASS_DEPTMODDISC
            || pItem->uchMinorClass == CLASS_PLUMODDISC
            || pItem->uchMinorClass == CLASS_SETMODDISC
            || pItem->uchMinorClass == CLASS_OEPMODDISC
            || pItem->uchMinorClass == CLASS_DEPTITEMDISC
            || pItem->uchMinorClass == CLASS_PLUITEMDISC
            || pItem->uchMinorClass == CLASS_SETITEMDISC
            || pItem->uchMinorClass == CLASS_OEPITEMDISC
            || (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) )
		{
            if (pData->lAmount != 0L) {
                return(LDT_PROHBT_ADR);
            }
        }

        if (pData->lAmount == 0L) {
            if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                *sQty = sSign;

            } else {
                *sQty = (SHORT)(pItem->lQTY / 1000L);
            }

        } else {
            *sQty = (SHORT)pData->lAmount * sSign;
        }
        break;

    case    CLASS_ITEMDISC:
        if (pData->lAmount != 0L) {
            return(LDT_PROHBT_ADR);
        }

        if (pDisc->fbDiscModifier & VOID_MODIFIER) {
            return(LDT_PROHBT_ADR);
        }
        *sQty = sSign;
        break;

    case    CLASS_ITEMCOUPON:
        if (pData->lAmount != 0L) {
            return(LDT_PROHBT_ADR);
        }

        if (pCpn->fbModifier & VOID_MODIFIER) {
            return(LDT_PROHBT_ADR);
        }
        *sQty = sSign;
        break;

    default:
        break;
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherTransfItem(UIFREGOTHER *pData)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherTransfItem(UIFREGOTHER *pData)
{
    UCHAR   uchPreSeat, uchCurSeat;
    SHORT   sStatus, sQty, i, sSize, sUpdate1 = 1;
    USHORT      usScroll;
	ITEMSALES       ItemSales = {0};
    ITEMDISC        ItemDisc = {0};

    /*----- Error Check -----*/
    if ((sStatus = ItemOtherTransferCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*----- Check Active LCD -----*/
    usScroll = MldQueryCurScroll();
    if (usScroll != MLD_SCROLL_1) {
        return(LDT_SEQERR_ADR);
    }

    /*----- Read Cursor Position Item -----*/
    if (MldGetCursorDisplay(usScroll, &ItemSales, &ItemDisc,MLD_GET_SALES_DISC) != MLD_SUCCESS) {
        return(LDT_SEQERR_ADR);
    }

    ItemTotalPrev();

    if (ItemSales.uchMajorClass == CLASS_ITEMDISC) {
        if (ItemSales.uchMinorClass == CLASS_ITEMDISC1) {
            return(LDT_PROHBT_ADR);     /* not transfer if item disc. is directed */
        }
        if (ItemSales.uchSeatNo == 'B') {
			ItemSales.uchSeatNo = 0;
		}
    }

    /*----- Need Seat# -----*/
    if ((sStatus = ItemOtherSeatNo(&uchCurSeat)) != ITM_SUCCESS) {
        return(sStatus);

    } else {
        if (uchCurSeat != 0) {
			TRANGCFQUAL     *WorkGCF;

            /*----- Check Used Seat# -----*/
            TrnGetGCFQualSpl(&WorkGCF, TRN_TYPE_SPLITA);
            for (i = 0; i < NUM_SEAT; i++) { //SR206
                if (WorkGCF->auchFinSeat[i] == uchCurSeat) {
                    return(LDT_PROHBT_ADR);
                }
            }

            /*----- Prohibit Used Seat# -----*/
            TrnGetGCFQual(&WorkGCF);
            for (i = 0; i < NUM_SEAT; i++) { //SR206
                if (WorkGCF->auchFinSeat[i] == uchCurSeat) {
                    return(LDT_PROHBT_ADR);
                }
            }
        }
    }

    /*----- Check/Decide Transfer Qty -----*/
    if (ItemOtherTransferQty(pData, &ItemSales, &sQty) != ITM_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    /*----- Erase Ticket Print Bit -----*/
    ItemSales.fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);

    if (ItemDisc.uchMajorClass != 0) {
        /*----- Search Consoli Storage -----*/
        if (TrnTransferSearchSalesDisc(&ItemSales, &ItemDisc, TRN_TYPE_CONSSTORAGE) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
        /*----- Search Item Storage -----*/
        if (TrnTransferSearchSalesDisc(&ItemSales, &ItemDisc, TRN_TYPE_ITEMSTORAGE) != TRN_SUCCESS) {
            sUpdate1 = 0;
        }
    } else {
        /*----- Search Consoli Storage -----*/
        if (TrnTransferSearch(&ItemSales, sQty, TRN_TYPE_CONSSTORAGE) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
        /*----- Search Item Storage -----*/
        if (TrnTransferSearch(&ItemSales, sQty, TRN_TYPE_ITEMSTORAGE) != TRN_SUCCESS) {
            sUpdate1 = 0;
        }
    }

    /*----- Check Input QTY -----*/
    switch(ItemSales.uchMajorClass) {
    case    CLASS_ITEMSALES:
        /* ----- Prohibit Transfer to Same Seat# -----*/
        if (ItemSales.uchSeatNo == uchCurSeat) {
            return(LDT_SEQERR_ADR);
        }

        /* ----- calculate transfer qty and price */
        if (!(ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {

            ItemSales.lQTY = ((LONG)sQty * 1000L);

            if (ItemSales.uchPPICode) { /* calculate ppi price */
                if (pData->lAmount == 0) {
                    ;
                } else if (ItemSalesOnlyCalcPPI(&ItemSales) != ITM_SUCCESS) {
                    ItemSales.lProduct = (ItemSales.lUnitPrice * (LONG)sQty);
                } else {    /* ppi sales price */
                    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                        ItemSales.lProduct *= -1L;
                    }
                }
            } else if (ItemSales.uchPM) { /* calculate price multiple 2172 */
                if (pData->lAmount == 0) {
                    ;
                } else if (ItemSalesOnlyCalcPM(&ItemSales) != ITM_SUCCESS) {
                    ItemSales.lProduct = (ItemSales.lUnitPrice * (LONG)sQty); 
                } else {    /* ppi sales price */
                    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                        ItemSales.lProduct *= -1L;
                    }
                }
            } else {
                ItemSales.lProduct = (ItemSales.lUnitPrice * (LONG)sQty);
            }
        }

        /*----- Check Item Size -----*/
        if (ItemDisc.uchMajorClass != 0) {
            sSize = TrnCheckSize(&ItemDisc, 0);
			if (sSize < 0) {
                return(LDT_TAKETL_ADR);
			}
        } else {
            sSize = 0;
        }
        if ((sStatus = ItemCommonCheckSize(&ItemSales, sSize)) < 0) {
            return(LDT_TAKETL_ADR);
        }

		ItemSales.usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ

        /*----- with Item Discount -----*/
        if (ItemDisc.uchMajorClass != 0) {
            TrnSplReduceStorageSalesDisc(&ItemSales, &ItemDisc);

            ItemDisc.uchSeatNo = uchCurSeat;

            if (sUpdate1 == 0) {
                ItemDisc.fbStorageStatus = NOT_ITEM_STORAGE;
            } else {
                ItemDisc.fbStorageStatus = 0;
            }

            /*----- Not Need Hourly Affection ! -----*/
            ItemDisc.fbReduceStatus |= REDUCE_NOT_HOURLY;
            TrnSplReduceItemizerSales(&ItemSales);

        } else {
            TrnSplReduce(&ItemSales);
        }

        uchPreSeat = ItemSales.uchSeatNo;
        ItemSales.uchSeatNo = uchCurSeat;
        ItemSales.sCouponQTY = sQty;

        if (sUpdate1 == 0) {
            ItemSales.fbStorageStatus = NOT_ITEM_STORAGE;
        } else {
            ItemSales.fbStorageStatus = 0;
        }

        /* 06/25/96 */
        if (ItemSales.uchMinorClass == CLASS_DEPTMODDISC
            || ItemSales.uchMinorClass == CLASS_PLUMODDISC
            || ItemSales.uchMinorClass == CLASS_SETMODDISC
            || ItemSales.uchMinorClass == CLASS_OEPMODDISC
            || ItemSales.uchMinorClass == CLASS_DEPTITEMDISC
            || ItemSales.uchMinorClass == CLASS_PLUITEMDISC
            || ItemSales.uchMinorClass == CLASS_SETITEMDISC
            || ItemSales.uchMinorClass == CLASS_OEPITEMDISC
            || (ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) ) {

//            ItemSales.fbStorageStatus |= NOT_ITEM_CONS;
        }

        /*----- Not Need Hourly Affection ! -----*/
        ItemSales.fbReduceStatus |= REDUCE_NOT_HOURLY;

        TrnItem(&ItemSales);

        /*----- with Item Discount -----*/
        if (ItemDisc.uchMajorClass != 0) {
            TrnSplReduceItemizerDisc(&ItemDisc);
            TrnItem(&ItemDisc);
        }
        break;

    case    CLASS_ITEMCOUPON:
		{
			ITEMCOUPON      *pCpn = (ITEMCOUPON *)&ItemSales;

			/* ----- Prohibit Transfer to Same Seat# -----*/
			if (pCpn->uchSeatNo == uchCurSeat) {
				return(LDT_SEQERR_ADR);
			}

			/*----- Check Item Size -----*/
			if ((sStatus = ItemCommonCheckSize(pCpn, 0)) < 0) {
				return(LDT_TAKETL_ADR);
			}

			TrnSplReduce(pCpn);

			uchPreSeat = pCpn->uchSeatNo;
			pCpn->uchSeatNo = uchCurSeat;

			if (sUpdate1 == 0) {
				pCpn->fbStorageStatus = NOT_ITEM_STORAGE;
			} else {
				pCpn->fbStorageStatus = 0;
			}

			/*----- Not Need Hourly Affection ! -----*/
			pCpn->fbReduceStatus |= REDUCE_NOT_HOURLY;

			TrnItem(pCpn);
		}
        break;

    default:
        return(LDT_SEQERR_ADR);
    }

    /*----- Display 1/3 LCD -----*/
    ItemOtherTransfDisp(uchPreSeat, uchCurSeat);

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherTransferCheck(VOID)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: 
*===============================================================================
*/
SHORT   ItemOtherTransferCheck(VOID)
{
    SHORT           sStatus;

    /*----- Only Allow PostCheck and Split System 4/23/96 -----*/
    if (!(((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) &&
         !(CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) &&
         !(CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)))) {
        return(LDT_PROHBT_ADR);
    }

    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
        return(LDT_SEQERR_ADR);
    }

    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);                 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if (ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL
        || ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
        return(LDT_SEQERR_ADR);
    }

    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI) {
        return(LDT_PROHBT_ADR);
    }

    /*----- compulsory status 4/23/96 ----- */
    if ((sStatus = ItemCommonCheckComp()) != ITM_SUCCESS ) {
        return (sStatus);
    }

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   VOID    ItemOtherTransfDisp(UCHAR uchPreSeat, UCHAR uchCurSeat)
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description:    Display 1/3 Transaction on LCD.
*===============================================================================
*/
VOID    ItemOtherTransfDisp(UCHAR uchPreSeat, UCHAR uchCurSeat)
{
	MLDTRANSDATA    MldTran = {0};

    if (TranCurQualPtr->uchSplit == 0) {
		/*----- Display on 1 LCD  -----*/
        MldTran.usVliSize = TrnInformationPtr->usTranConsStoVli;
        MldTran.sFileHandle = TrnInformationPtr->hsTranConsStorage;
        MldTran.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
        MldPutTransToScroll(&MldTran);

        MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
    } else {
		/*----- Display on 3 LCD  -----*/
		TRANITEMIZERS    *WorkTI;
		TRANINFORMATION  *TrnInf;
		TRANINFSPLIT     *SplitA, *SplitB;
		SHORT   sFlag = 0;
		UCHAR   uchMiddle = 0, uchRight = 0;

        TrnSplGetDispSeat(&TrnInf, &SplitA, &SplitB);

        /*----- Display Base Transaction -----*/
        MldDisplayInit(MLD_DRIVE_1, 0);
        MldTran.usVliSize = TrnInf->usTranConsStoVli;
        MldTran.sFileHandle = TrnInf->hsTranConsStorage;
        MldTran.sIndexHandle = TrnInf->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_1, &MldTran);

        /*----- Decide Split#1 Transaction -----*/
        if ((UCHAR)SplitA->TranGCFQual.usGuestNo == 0 && uchCurSeat != 0) {
            uchMiddle = uchCurSeat;
            sFlag = 1;
        } else if ((UCHAR)SplitA->TranGCFQual.usGuestNo == uchPreSeat ||
				(UCHAR)SplitA->TranGCFQual.usGuestNo == uchCurSeat)
		{
            uchMiddle = (UCHAR)SplitA->TranGCFQual.usGuestNo;
        }

        /*----- Decide Split#2 Transaction -----*/
        if ((UCHAR)SplitB->TranGCFQual.usGuestNo == 0
            && uchCurSeat != 0
            && sFlag == 0
            && uchCurSeat != uchMiddle) {
            uchRight = uchCurSeat;

        } else if ((UCHAR)SplitB->TranGCFQual.usGuestNo == uchPreSeat
            || (UCHAR)SplitB->TranGCFQual.usGuestNo == uchCurSeat) {
            uchRight = (UCHAR)SplitB->TranGCFQual.usGuestNo;
        }

        /*----- Display Split#1 Transaction -----*/
        if (uchMiddle != 0) {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
            TrnSplGetSeatTrans(uchMiddle, TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA);

            MldDisplayInit(MLD_DRIVE_2, 0);
            SplitA = TrnGetInfSpl(TRN_TYPE_SPLITA);
            MldTran.usVliSize = SplitA->usTranConsStoVli;
            MldTran.sFileHandle = SplitA->hsTranConsStorage;
            MldTran.sIndexHandle = SplitA->hsTranConsIndex;
            MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);

            TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
            MldDispSubTotal(MLD_TOTAL_2, WorkTI->lMI);

            TrnPutInfGuestSpl(uchMiddle, 0);
        }

        /*----- Display Split#2 Transaction -----*/
        if (uchRight != 0) {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
            TrnSplGetSeatTrans(uchRight, TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITB);

            MldDisplayInit(MLD_DRIVE_3, 0);
            SplitB = TrnGetInfSpl(TRN_TYPE_SPLITB);
            MldTran.usVliSize = SplitB->usTranConsStoVli;
            MldTran.sFileHandle = SplitB->hsTranConsStorage;
            MldTran.sIndexHandle = SplitB->hsTranConsIndex;
            MldPutSplitTransToScroll(MLD_SCROLL_3, &MldTran);

            TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITB);
            MldDispSubTotal(MLD_TOTAL_3, WorkTI->lMI);

            TrnPutInfGuestSpl(0, uchRight);
        }
    }
}

/*==========================================================================
**   Synopsis:  SHORT   ItemOtherSeatNo(UCHAR *puchSeatNo)
*
*   Input:      nothing
*   Output:     UCHAR   *puchSeatNo
*   InOut:      none
*
*   Return:     
*
*   Description:    Get Seat# for SplitCheck,         R3.1
==========================================================================*/

SHORT   ItemOtherSeatNo(UCHAR *puchSeatNo)
{
	REGDISPMSG  DispMsg = {0};
    UIFDIADATA  WorkUI;

    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_SEATNO;
    RflGetSpecMnem (DispMsg.aszMnemonic, SPC_SEAT_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&DispMsg);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;
     
    if (UifDiaSeatNo(&WorkUI) != UIF_SUCCESS || WorkUI.ulData == 0UL) { /* V3.3 */
        DispMsg.uchMajorClass = CLASS_UIFREGOTHER;  /*set majour class */
        DispMsg.uchMinorClass = CLASS_UICLEAR;      /*set minor class */
        DispWrite( &DispMsg );  /*  */
        return(UIF_CANCEL);
    }

    *puchSeatNo = (UCHAR)WorkUI.ulData;

    return(ITM_SUCCESS);
}

/*
*=============================================================================
**  Synopsis:   SHORT   ItemOtherCheckDispPara(DISMSG *pData,
                                USHORT *pusPLUNo, UCHAR *puchAdjective);
*               
*       Input:  DIFMSG  *pData
*      Output:  USHORT  *pusPLUNo
*               UCHAR   *puchAdjective
**     Return:  CLASS_ITEMSALES
*               CLASS_ITEMTENDER
*               CLASS_ITEMTOTAL
*               ITM_SUCCESS
*
**  Description: Check Beverage Dispenser Data R3.1
*===============================================================================
*/
SHORT   ItemOtherCheckDispPara(VOID *pData, USHORT *pusPLUNo, UCHAR *puchAdjective)
{
#if defined(USE_2170_ORIGINAL)
	DIFMSG          *pDifMsg;
    PARADISPPARA    ParaDispPara;
    USHORT          usPLUNo;

    pDifMsg = pData;
    ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;

    switch(pDifMsg->uchMsg) {
    case    DIF_POUR_REQ_MSG:           /* plu sales */
        usPLUNo = atoi(pDifMsg->auchData);
        if (!usPLUNo) {
            return(ITM_SUCCESS);
        }

        ParaDispPara.usAddress = DISPENS_OFFSET_ADR;
        CliParaRead(&ParaDispPara);
        if ((SHORT)(usPLUNo - ParaDispPara.usPluNumber) > 0) {
            ParaDispPara.usAddress = (usPLUNo - ParaDispPara.usPluNumber);
            if (ParaDispPara.usAddress < DISPENS_OFFSET_ADR) {
                CliParaRead(&ParaDispPara);
                if (ParaDispPara.usPluNumber) {
                    *pusPLUNo = ParaDispPara.usPluNumber;       /* convert plu no */
                    *puchAdjective = ParaDispPara.uchAdjective;
                    return(CLASS_ITEMSALES);
                }
            }
        }

        *pusPLUNo = usPLUNo;        /* use dispenser no as plu no */
        *puchAdjective = 0;
        return(CLASS_ITEMSALES);

    case    DIF_NEXT_ORDER_MSG:     /* total/tender */
    case    DIF_END_OF_POUR_MSG:
        ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;
        if (pDifMsg->uchMsg  == DIF_NEXT_ORDER_MSG) {
            ParaDispPara.usAddress = DISPENS_TOTAL1_ADR;
        } else {
            ParaDispPara.usAddress = DISPENS_TOTAL2_ADR;
        }
        CliParaRead(&ParaDispPara);
        if (ParaDispPara.usPluNumber) {
			/* temporary hardcoded 11, change to MAINT_TEND_MAX when implementing full
			tender database change JHHJ 3-16-04*/
            if (ParaDispPara.usPluNumber > MAX_DISPPARA_TENDER) {
                *pusPLUNo = ParaDispPara.usPluNumber - MAX_DISPPARA_TENDER;
                return(CLASS_ITEMTOTAL);
            } else {
                *pusPLUNo = ParaDispPara.usPluNumber;
                return(CLASS_ITEMTENDER);
            }
        } else {
            return(ITM_SUCCESS);
        }
    default:
        return(ITM_SUCCESS);
    }
#else
	return(ITM_SUCCESS);
#endif
    
}

/*
*=============================================================================
**  Synopsis:   VOID    ItemOtherDifEnableStatus(USHORT usType);
*               
*       Input:  USHORT  usType
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: Enable Beverage Dispenser I/F Status R3.1
*===============================================================================
*/
VOID ItemOtherDifEnableStatus(USHORT usType)
{
#if defined(USE_2170_ORIGINAL)
    if ((TranModeQualPtr->ulCashierID == 0) && (TranModeQualPtr->ulWaiterID == 0)) {
        /* before operator open */
        DifDisableStatus();
        return;
    }

    if ((TranModeQualPtr->ulWaiterID) &&
        ((TranModeQualPtr->fsModeStatus & MODEQUAL_BARTENDER) == 0) &&
        ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0)) {

        /* before guest check open */
        DifDisableStatus();
        return;
    }

    if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
        /* training operator */
        DifDisableStatus();
        return;
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        /* transaction void */
        DifDisableStatus();
        return;
    }

    if (usType) {
        DifEnableStatus2();     /* set enable status after clear key */
    } else {
        DifEnableStatus();
    }
#endif
}

/*
*=============================================================================
**  Synopsis:   VOID    ItemOtherDifDisableStatus(VOID);
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
**  Description: Disable Beverage Dispender I/F Status R3.1
*===============================================================================
*/
SHORT ItemOtherDifDisableStatus(VOID)
{
#if defined(USE_2170_ORIGINAL)
	return(DifDisableStatus());
#else
	return(FALSE);
#endif
}

/*
*=============================================================================
**  Synopsis:   VOID    ItemOtherDifReriteRing(VOID);
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
*   Description: rewrite message to ring buffer after clear key depression
*===============================================================================
*/
VOID ItemOtherDifRewriteRing(VOID)
{
#if defined(USE_2170_ORIGINAL)
	DifRewriteRingBuffer();
#endif
}

/*
*=============================================================================
**  Synopsis:   VOID    ItemOtherDifClearBuffer(VOID);
*               
*       Input:  Nothing
*      Output:  Nothing
**     Return:  Nothing
*
*   Description: Clear Request Message Data after Sales Process
*===============================================================================
*/
VOID ItemOtherDifClearBuffer(VOID)
{
#if defined(USE_2170_ORIGINAL)
	DifClearSavedBuffer();
#endif
}

/*
*===========================================================================
**Synopsis: VOID    ItemOtherDiaRemoveKey( VOID )
*
*    Input: none
*
*   Output: none
*
*    InOut: none
*
**Return:   noen
*
** Description: Display leadthrough message to remove waiter lock key until 
*               to be removed, V3.3.
*
*===========================================================================
*/

VOID    ItemOtherDiaRemoveKey( VOID )
{
	REGDISPMSG      DisplayData = {0};

    DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DisplayData.uchMinorClass = CLASS_REGDISP_REMOVEKEY;
    DisplayData.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    RflGetLead (DisplayData.aszMnemonic, LDT_RMVWTLOCK_ADR );

    flDispRegDescrControl |= COMPLSRY_CNTRL; /* set descriptor */
    flDispRegKeepControl |= COMPLSRY_CNTRL; /* set descriptor(keep) */
    DispWrite( &DisplayData );
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL; /* reset descriptowr */
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL; /* reset descriptor(keep) */

    UifDiaRemoveLock();                     /* wait waiter key remove */
}

/*
*===========================================================================
**Synopsis: VOID    ItemOtherDiaRemoveKey( VOID )
*
*    Input: none
*
*   Output: none
*
*    InOut: none
*
**Return:   noen
*
** Description: Display leadthrough message to remove waiter lock key until 
*               to be removed, V3.3.
*
*===========================================================================
*/

ULONG    ItemOtherDiaInsertKey( VOID )
{
	REGDISPMSG   DisplayData = {0};
    UIFDIADATA   WorkUI;
    LONG         lReturn;

    DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DisplayData.uchMinorClass = CLASS_REGDISP_INSERTKEY;
    DisplayData.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    RflGetLead (DisplayData.aszMnemonic, LDT_INSWTLOCK_ADR );

    flDispRegDescrControl |= COMPLSRY_CNTRL; /* set descriptor */
    flDispRegKeepControl |= COMPLSRY_CNTRL; /* set descriptor(keep) */
    DispWrite( &DisplayData );
    flDispRegDescrControl &= ~ COMPLSRY_CNTRL; /* reset descriptowr */
    flDispRegKeepControl &= ~ COMPLSRY_CNTRL; /* reset descriptor(keep) */

    DispWrite( &DisplayData );              /* call display module */

    lReturn = UifDiaInsertLock(&WorkUI);    /* wait waiter key insert */

    lReturn = (LONG)(lReturn == UIF_SUCCESS ? WorkUI.ulData : 0);

    return (lReturn);
}

/*
*==========================================================================
**  Synopsis:   VOID   ItemOtherDispGuestNo(USHORT usCashierNo)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Display opened guest check no.  V3.3
*==========================================================================
*/
VOID   ItemOtherDispGuestNo(ULONG ulCashierNo)
{

    MLDGUESTNUM     MldGuestNum = {0};
	USHORT          ausRcvBuff[MLD_GUESTNO_NUM + 1] = {0};
    USHORT          i;

    /* read opened guest check no. from GCF */
    if (CliGusAllIdReadBW(GCF_COUNTER_TYPE, ulCashierNo, ausRcvBuff, sizeof(ausRcvBuff)) < 0) {  /* Error Case */
        return;
    }

    for (i = 0; i < MLD_GUESTNO_NUM; i++) {
        if (ausRcvBuff[i] == 0) break;
        if (ausRcvBuff[i] < 10000) {    /* not display cashier interrupt gcf see TRANSOPEN_GCNO() */

            MldGuestNum.GuestNo[i].usGuestNo = ausRcvBuff[i];
            if ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT1 ) ) {         /* CDV */
                MldGuestNum.GuestNo[i].uchCdv = RflMakeCD( ausRcvBuff[i] );
            }
        }
    }

	if(i == MLD_GUESTNO_NUM){//if display full
		//check to see if any more
		if(ausRcvBuff[i] != 0){
			MldGuestNum.GuestNo[i -1].usGuestNo = -1;
		}
	}

    if (i) {
        MldDispGuestNum(&MldGuestNum);  /* display on LCD right */
    }
}

VOID   ItemOtherDispTableNo(ULONG ulCashierNo)
{
    MLDGUESTNUM        MldGuestNum = {0};
	GCF_STATUS_STATE   ausRcvBuff[MLD_GUESTNO_NUM + 1] = {0};
    USHORT             i;

    /* read opened guest check no. from GCF */
    if (CliGusAllIdReadFiltered(GCF_COUNTER_TYPE, ulCashierNo, 0, ausRcvBuff, MLD_GUESTNO_NUM) < 0) {  /* Error Case */
        return;
    }

    for (i = 0; i < MLD_GUESTNO_NUM; i++) {
		if (ausRcvBuff[i].usGCNO == 0) break;
		MldGuestNum.GuestNo[i].usGuestNo = ausRcvBuff[i].usGCNO;
		MldGuestNum.GuestNo[i].usTableNo = ausRcvBuff[i].usTableNo;
    }

	if(i == MLD_GUESTNO_NUM){//if display full
		//check to see if any more
		if(ausRcvBuff[i].usGCNO != 0){
			MldGuestNum.GuestNo[i -1].usGuestNo = -1;
		}
	}

    if (i) {
        MldDispTableNum(&MldGuestNum);  /* display on LCD right */
    }
}
/*
*==========================================================================
**  Synopsis:   SHORT   ItemOtherResetLog(UCHAR uchAction)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Print PC i/F Reset Log                  V3.3
*==========================================================================
*/
void ItemOtherResetLogTrnItem (UCHAR uchAction, LONG lAmount)
{
    ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer ();      /* get sales local data */
    USHORT          usTrailer = 0;
	ITEMOTHER       TrnItemOther = {0};

	if ((pWorkSales->fbSalesStatus & SALES_RESETLOG_START) == 0) {
		MaintIncreSpcCo(SPCO_CONSEC_ADR);
		pWorkSales->fbSalesStatus |= SALES_RESETLOG_START;
	}

	TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;
	TrnItemOther.uchMinorClass = CLASS_RESETLOG;
	TrnItemOther.uchYear    = IspResetLog.uchYear;
	TrnItemOther.uchMonth   = IspResetLog.uchMonth;
	TrnItemOther.uchDay     = IspResetLog.uchDay;
	TrnItemOther.uchHour    = IspResetLog.uchHour;
	TrnItemOther.uchMin     = IspResetLog.uchMin;

	if (uchAction == UIF_UCHRESETLOG_NOACTION) {                            /* Mnemonic from PC */
		TrnItemOther.uchAction  = uchAction;
		_tcsncpy(TrnItemOther.aszMnemonic, IspResetLog.aszMnemonic, OTHER_MNEM_SIZE);
		memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
		TrnOther (&TrnItemOther);
		TrnItemOther.uchAction = UIF_UCHRESETLOG_TRAILERONLY;
		TrnItemOther.lAmount = lAmount;
		TrnOther (&TrnItemOther);
		pWorkSales->fbSalesStatus &= ~SALES_RESETLOG_START;
	} else if (uchAction == UIF_UCHRESETLOG_LOGONLY || uchAction == UIF_UCHRESETLOG_LOGWITHTRAILER) {/* PEP Mnem, V3.3 FVT #5 */
		if (uchAction == UIF_UCHRESETLOG_LOGWITHTRAILER) {
			uchAction = UIF_UCHRESETLOG_LOGONLY;
			usTrailer = 1;
		}
		TrnItemOther.uchAction = uchAction;
		_tcsncpy(TrnItemOther.aszMnemonic, IspResetLog.aszMnemonic, OTHER_MNEM_SIZE);
		memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
		TrnOther (&TrnItemOther);
		if (usTrailer != 0) {
			TrnItemOther.uchAction  = UIF_UCHRESETLOG_TRAILERONLY;
			TrnItemOther.lAmount    = lAmount;
			TrnOther (&TrnItemOther);
			pWorkSales->fbSalesStatus &= ~SALES_RESETLOG_START;
		}
	} else if (uchAction == UIF_UCHRESETLOG_TRAILERONLY) {  /* Trailer, V3.3 FVT #5 */
		TrnItemOther.uchAction  = uchAction;
		TrnItemOther.lAmount    = lAmount;
		TrnOther (&TrnItemOther);
		pWorkSales->fbSalesStatus &= ~SALES_RESETLOG_START;
	} else {                                                /* PCIF Reset Report */
		TrnItemOther.uchDayPtd  = IspResetLog.uchDayPtd;
		TrnItemOther.uchCurSave = IspResetLog.uchCurSave;
		TrnItemOther.uchReset   = IspResetLog.uchReset;
		TrnItemOther.lAmount    = IspResetLog.ulNumber;
		TrnItemOther.uchError   = IspResetLog.uchError;
		TrnItemOther.uchAction  = uchAction;
		TrnOther (&TrnItemOther);
		TrnItemOther.uchAction = UIF_UCHRESETLOG_TRAILERONLY;
		TrnItemOther.lAmount = lAmount;
		TrnOther (&TrnItemOther);
		pWorkSales->fbSalesStatus &= ~SALES_RESETLOG_START;
	}
}

SHORT   ItemOtherResetLog(UCHAR uchAction)
{
    if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0) {           /* itemize state */
		ItemOtherResetLogTrnItem (uchAction, 0);
		return(UIF_SUCCESS);
    } else {
        return(UIF_CANCEL);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemOtherPowerDownLog(UCHAR uchLog)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Print Power Down Log.                       Saratoga
*==========================================================================
*/
SHORT   ItemOtherPowerDownLog(UCHAR uchLog)
{
    if (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) {
		PARAMDC         WorkMDC = {0};

        /* set power down status for next power up */
        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        WorkMDC.usAddress = MDC_POWERSTATE_INDIC;                   /* MDC 400 */
        CliParaRead( &WorkMDC );                                    /* get target MDC */
        WorkMDC.uchMDCData |= EVEN_MDC_BIT3;
        CliParaWrite(&WorkMDC);                                     /* call ParaMDCWrite() */
        if (pPifSaveFarData != NULL) {               /* saratoga */
            (*pPifSaveFarData)();                           
        } 
    } else {
		PARAMDC         WorkMDC = {0};

        /* check power down status */
        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        WorkMDC.usAddress = MDC_POWERSTATE_INDIC;                   /* MDC 400 */
        CliParaRead( &WorkMDC );                                    /* get target MDC */
        if (WorkMDC.uchMDCData & EVEN_MDC_BIT3) {
			DATE_TIME   WorkDate = {0};
			ITEMOTHER   TrnItemOther = {0};
            
            TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;
            TrnItemOther.uchMinorClass = CLASS_POWERDOWNLOG;
            TrnItemOther.uchAction     = uchLog;

            PifGetDateTime(&WorkDate);
            TrnItemOther.uchYear  = (UCHAR)(WorkDate.usYear % 100);
            TrnItemOther.uchMonth = (UCHAR)WorkDate.usMonth;
            TrnItemOther.uchDay   = (UCHAR)WorkDate.usMDay;
            TrnItemOther.uchHour  = (UCHAR)WorkDate.usHour;
            TrnItemOther.uchMin   = (UCHAR)WorkDate.usMin;

            TrnOther (&TrnItemOther);

            PifLog(MODULE_UI, 6);   /* Data Lost */
       }
        
        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        WorkMDC.usAddress = MDC_POWERSTATE_INDIC;                   /* MDC 400 */
        CliParaRead( &WorkMDC );                                    /* get target MDC */
        WorkMDC.uchMDCData &= ~EVEN_MDC_BIT3;
        CliParaWrite(&WorkMDC);                                     /* call ParaMDCWrite() */
        if (pPifSaveFarData != NULL) {               /* saratoga */
            (*pPifSaveFarData)();                           
        } 
    }
    return(UIF_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemOtherLabelProc(UIFREGOTHER   *pRegOther);,
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFREGOTHER *pUifRegOther
*
**Return:   UIF_SUCCESS - function executes successfuly
*           UIF_CANCEL  - cancel this operation
*           LDT_???_ADR - return error code if it has occerred
*
** Description: This function checkes the input label type
*               and executes the appropriate procedure.         2172
*===========================================================================
*/
SHORT   ItemOtherLabelProc(UIFREGOTHER *pRegOther)
{
	LABELANALYSIS   PluLabel = {0};
    SHORT           sOffset;

    /* ----- set plu number for label analysis ----- */
    _tcscpy(PluLabel.aszScanPlu, pRegOther->aszLabelNo);

    /* ----- PLU is entered by Scanner or Key ? ----- */
    if (pRegOther->uchFsc == FSC_SCANNER) {
        PluLabel.fchModifier |= LA_SCANNER;             /* by Scanner */
        sOffset = ItemCommonLabelPosi(pRegOther->aszLabelNo);
        if (sOffset < 0) {
            return(LDT_KEYOVER_ADR);
        }
    }

    /* ----- analyze PLU number from UI ----- */
    if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {

        switch (PluLabel.uchType) {
        case    LABEL_RANDOM:
#if 0
            if (pPluLabel->fsBitMap & LA_F_FREQ) {
                ;
            } else {
#endif

            if (PluLabel.uchLookup & LA_EXE_LOOKUP) {
                /* plu label */
                if (PluLabel.fsBitMap & ~LA_F_ARTINO) {
                    /* keep HP V3.3 compatibility, if no MDC set */
                    return (LDT_PROHBT_ADR);
                }
                _tcscpy(pRegOther->aszNumber, &PluLabel.aszLookPlu[2]);
                break;
            }
        
            switch (PluLabel.fsBitMap) {
            case (LA_F_PRICE | LA_F_DEPT):
            case (LA_F_PRICE | LA_F_SKU_NO):
                /* dept label */
                return (LDT_PROHBT_ADR);
            break;

            case LA_F_NUMBER:
            case LA_F_CONS_NO:
            case LA_F_SKU_NO:
                /* assume as account number */
                _tcscpy(pRegOther->aszNumber, &PluLabel.aszLookPlu[2]);
                break;

            default:
                return (LDT_KEYOVER_ADR);
                break;
            }
            break;
            
        default:
            /* plu label */
            return (LDT_KEYOVER_ADR);
        }
    
    } else {
        return(LDT_KEYOVER_ADR);
    }
        
    return (UIF_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemOtherAsk(UIFREGOTHER *pItem)
*
*   Input:
*   Output:
*   InOut:      none
*   Return:
**  Description:    Ask Function(Pre-Inquiry, Report)           Saratoga
*==========================================================================
*/
SHORT   ItemOtherAsk(UIFREGOTHER *pItem)
{
    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);                 
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {     /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* --- Check Sign-in --- */
    if (pItem->uchFsc <= ITM_ASK_REPORT5) {
        if (TranModeQualPtr->ulCashierID == 0) {
            return(LDT_SEQERR_ADR);
        }
        if ((ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) || (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL)) { /* Saratoga */
            return(LDT_SEQERR_ADR);
        }
    } else {
        if (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) {
            return(LDT_SEQERR_ADR);
        }
    }

    switch (pItem->uchFsc) {
	case	ITM_ASK_EPT_LOOKUP:
	case    ITM_ASK_EPT:
        return(ItemOtherAskEPT(pItem));

    case    ITM_ASK_REPORT10:               // transformed into EEPT_FC2_ACTION_EPT. Now used for EMV pad reset with GenPOS Rel 2.3.0
        return(ItemOtherAskReport(pItem));

    case    ITM_ASK_REPORT6:
    case    ITM_ASK_REPORT7:
    case    ITM_ASK_REPORT8:
    case    ITM_ASK_REPORT9:
		// drop through and treat as default. The above
		// report types were part of the old electronic payment interface
		// and the old charge post functionality such as COM2170 charge post
		// application from NHPOS 1.4 and the NCR 2170 and NCR 7448 days.
		//   3/29/2021  Richard Chambers

    default:
        return(LDT_SEQERR_ADR);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemOtherAskEPT(UIFREGOTHER *pItem)
*
*   Input:
*   Output:
*   InOut:      none
*   Return:
**  Description:    Ask for EPT Function                    Saratoga
*==========================================================================
*/
SHORT   ItemOtherAskEPT(UIFREGOTHER *pItem)
{
    SHORT           sStatus;
	UIFREGTENDER    UifTender = {0};
    ITEMTENDER      Tender = {0};
	UCHAR			uchSeat;

    /* --- Check Already Authorized --- */
	uchSeat = TranCurQualPtr->uchSeat;
    if (uchSeat <= NUM_SEAT_CARD && TranGCFQualPtr->aszNumber[uchSeat][0] != '\0') {
		NHPOS_ASSERT_TEXT(0, "==ERROR: check already authorized. ItemOtherAskEPT()");
        return(LDT_PROHBT_ADR);
    }

	sStatus = ItemCommonSetupTransEnviron ();
	if(sStatus != ITM_SUCCESS)
		return (sStatus);

	fsPrtCompul = 0;
	fsPrtNoPrtMask = 0;

    /* --- Account# Entry, R2.0 GCA --- */
    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );           /* inhibit post receipt, R3.0 */

    /* --- Set Command Code --- */
	if (pItem->uchFsc == ITM_ASK_EPT_LOOKUP) {
		// this is an account look up function and we have two options for the data
		//   - lookup by last name and last 4 digits of social security number
		//   - lookup by first name and last name
		sStatus = ItemAccountLookup(IS_GIFT_CARD_EPAYMENT, GIFT_CARD_LOOKUPNAMESS4);
	} else {
		UifTender.uchMinorClass = pItem->uchFsc;
		sStatus = ItemTendEPTComm(EEPT_FC2_ASK, &UifTender, &Tender, TranModeQualPtr->ulCashierID);
	}

    if (sStatus == UIF_CANCEL) {
		REGDISPMSG      Disp = {0};

		Disp.uchMajorClass = CLASS_UIFREGOTHER;
        Disp.uchMinorClass = CLASS_UICLEAR;
        DispWrite(&Disp);
    } else {
		ITEMOTHER       Other = {0};

		Other.uchMajorClass = CLASS_ITEMOTHER;
        Other.uchMinorClass = CLASS_PREINQ_EPT;
        memcpy(Other.aszNumber, Tender.aszNumber, sizeof(Other.aszNumber));
        memcpy(Other.auchMSRData, Tender.auchMSRData, sizeof(Other.auchMSRData));
        TrnItem(&Other);
        ItemOtherClear();
		sStatus = ITM_SUCCESS;
	}

    ItemMiscResetStatus( MISC_PARKING | MISC_GIFTRECEIPT );           /* inhibit post receipt, R3.0 */

	if (sStatus != UIF_CANCEL) {
		//Next are a series of funtion calls to finalize and close this "transaction"
		// so that a normal operations may continue once the balance has been executed.
		ItemFinalize();
		ItemTendInit();
	}

    return(sStatus);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemOtherAskReport(UIFREGOTHER *pItem)
*
*   Input:
*   Output:
*   InOut:      none
*   Return:
**  Description:    Report Function                         Saratoga
*==========================================================================
*/
SHORT   ItemOtherAskReport(UIFREGOTHER *pItem)
{
    SHORT           sStatus = ITM_SUCCESS;
	UIFREGTENDER    UifTender = {0};
    ITEMTENDER      Tender = {0};
	UCHAR           uchFuncCode = EEPT_FC2_ASK;

    /* --- Set Command Code --- */
    UifTender.uchMinorClass = pItem->uchFsc;
	if (ITM_ASK_REPORT10 == pItem->uchFsc) {
		UifTender.GiftCard = TENDERKEY_TRANTYPE_EMV_PARAM;    // indicate <TranCode>EMVPadReset</TranCode>
		uchFuncCode = EEPT_FC2_ACTION_EPT;
	}

    sStatus = ItemTendEPTComm (uchFuncCode, &UifTender, &Tender, TranModeQualPtr->ulCashierID);
    if (sStatus == ITM_SUCCESS || sStatus == ITM_REJECT_ASK) {
        ItemOtherClear();
        ItemTrailer(TYPE_THROUGH_BOTH);
        return(ItemFinalize());
    } else if (sStatus == UIF_CANCEL) {
		REGDISPMSG      Disp = {0};

        Disp.uchMajorClass = CLASS_UIFREGOTHER;
        Disp.uchMinorClass = CLASS_UICLEAR;
        DispWrite(&Disp);
    }

    return(sStatus);
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemOtherValPrint(ITEMSALES *pItem)
*   Input:
*   Output:
*   InOut:      none
*   Return:
**  Description:    Perform Validation Print
*==========================================================================
*/
static VOID    ItemOtherValPrint(ITEMDATASIZEUNION *pItem)
{
	switch(pItem->ItemHeader.uchMajorClass) {
    case CLASS_ITEMSALES:
		pItem->ItemSalesBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    case CLASS_ITEMCOUPON:
		pItem->ItemCouponBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    case CLASS_ITEMDISC:
		pItem->ItemDiscBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    case CLASS_ITEMTOTAL:
		pItem->ItemTotalBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    case CLASS_ITEMTENDER:
		pItem->ItemTenderBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    case CLASS_ITEMMISC:
		pItem->ItemMiscBuff.fsPrintStatus = PRT_VALIDATION;
        break;

    default:
        break;
    }

    TrnThrough(pItem);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemOtherValidation(VOID)
*
*     Input:
*    Output:    nothing
** Return:
*
** Description: Validation Key Function
*===========================================================================
*/
SHORT   ItemOtherValidation(VOID)
{
    ITEMDATASIZEUNION  WorkSales;
	PARASLIPFEEDCTL    FeedCtl = {0};
	TRANINFORMATION    *pTran = TrnGetTranInformationPointer();
	ITEMCOMMONLOCAL    *WorkCommon = ItemCommonGetLocalPointer();
	static int		   uchValCo = -2, itemNumber = 0;
	static USHORT	   usGuestNo = 0;

	if( (itemNumber != pTran->TranItemizers.sItemCounter) || //is a new item?
		(usGuestNo != pTran->TranGCFQual.usGuestNo))  //or new transaction?
	{
		uchValCo = -2;									  //-2 is Flag for new item
		itemNumber = pTran->TranItemizers.sItemCounter;   //Set the item number to the new item number
	}	

	if( uchValCo == -1){   //-1 is set to indicate you have reached the max amount of times you can validate
		return(LDT_PROHBT_ADR);							 //return sequence error
	}

	/*--- Get Max Validation Co. (P18 Address 3)---*/
    FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
    FeedCtl.uchAddress = SLIP_MAXVAL_ADR;
    CliParaRead(&FeedCtl);

	if( uchValCo == -2)									 //if it is a new item
	{		
		uchValCo = FeedCtl.uchFeedCtl;					 //the validation counter is equal to the number of times allowed
		usGuestNo   = pTran->TranGCFQual.usGuestNo;		 //different orders should be taken into consideration also 
	}
		
	WorkSales.ItemSalesBuff = WorkCommon->ItemSales;
    if (WorkSales.ItemSalesBuff.uchMajorClass == CLASS_ITEMOPECLOSE) {
        return(ITM_SUCCESS);
    }

    /*--- Validation Print  ---*/
    ItemOtherValPrint(&WorkSales);

	//Validation counter Update
	if (uchValCo != 0) {								//if uchValCo IS 0, Validation maximum is not defined, infinate validations possible
		uchValCo--;										//decrease the counter
		if(uchValCo == 0){								//if the counter is decreased to zero, make it -1, because is cannot be 0,
			uchValCo = -1;								//because 0 means infinate number of times to validate
		}
	}

    return(ITM_SUCCESS);
}


/****** End of Source ******/