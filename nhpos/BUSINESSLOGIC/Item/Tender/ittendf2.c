/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTENDF2.C (foreign currency partial tender)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTendFCPart()        ;   foreign currncy partial tender   
:  
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date    :  Version  :   Name    :   Description
:   95/3/28   03.00.00  : hkato     : R3.0
:   95/11/22  03.01.00  : hkato     : R3.1
: Aug-13-99: 03.05.00  : K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: Dec-06-99: 01.00.00  : hrkato    : Saratoga
: Jul-28-17: 02.02.02  : R.Chambers : removed unneeded include files, localized variables
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
#include    "paraequ.h"
#include    "para.h"
#include    "display.h"
#include    "mld.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "rfl.h"
#include    "itmlocal.h"


/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCPart(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    partial tender
==========================================================================*/

SHORT   ItemTendFCPart(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	SHORT sSplitStatus = TrnSplCheckSplit();

    /*----- Split Check,   R3.1 -----*/
    if (sSplitStatus == TRN_SPL_SEAT || sSplitStatus == TRN_SPL_MULTI ||
        sSplitStatus == TRN_SPL_TYPE2 || sSplitStatus == TRN_SPL_SPLIT) {
        return(ItemTendFCPartSpl(UifRegTender, ItemTender));
    } else {
        return(ItemTendFCPartNor(UifRegTender, ItemTender));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCPartNor(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender           
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency partial tender
==========================================================================*/

SHORT   ItemTendFCPartNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sStatus, sSize;

    if ( ( sStatus = ItemTendFCPartCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                          /* error */                
    }
                          
    if ( ( sSize = ItemTendFCPartTender( UifRegTender, ItemTender ) ) != ITM_SUCCESS ) {           
        return( LDT_TAKETL_ADR );                                   /* error */                
    }
                          
    if ( ( sStatus = ItemTendPartSPVInt( UifRegTender ) ) != ITM_SUCCESS ) {
        return( sStatus );                                          /* error */                
    }

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                           /* FS total */
        if ((ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION)  /* validation print */
            || (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1) 
            && (UifRegTender->fbModifier & VOID_MODIFIER))) {
            ItemTender->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(ItemTender);
        }

    } else {
        ItemTendAffectTax(UifRegTender);
        ItemTendTrayTotal();                                        /* tray total */
        ItemTendTrayTax();                                          /* whole tax at tray transaction */   
    }

    if ( ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_FC0_VALIDATION )  /* validation print */
        || ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1 ) 
            && ( UifRegTender->fbModifier & VOID_MODIFIER ) ) ) {  

        ItemTender->fsPrintStatus = PRT_VALIDATION;                     /* print validation */ 
        TrnThrough( ItemTender );
    }

    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */
    ItemTender->fbStorageStatus |= NOT_SEND_KDS;                    /* not send to KDS at next item R3.1 */

    if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {    /* R3.0 */
		ITEMCOMMONLOCAL *pWorkCommon = ItemCommonGetLocalPointer();

		MldScrollWrite(&pWorkCommon->ItemSales, MLD_NEW_ITEMIZE);
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&pWorkCommon->ItemSales, 0);
    }

    ItemTendVAT(UifRegTender);
    ItemPreviousItem( ItemTender, ( USHORT )sSize );                /* send previous "item" */
    ItemTendMulti();                                                /* print data for multi check */

    ItemTenderLocal.fbTenderStatus[0] |= TENDER_PARTIAL;            /* partial tend state */
    ItemTenderLocal.lTenderizer += ItemTender->lTenderAmount;       /* update tenderizer */

    ItemTenderLocal.fbTenderStatus[2] &= ~( TENDER_FC1 | TENDER_FC2| TENDER_FC3 | TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8);

	{
		TRANCURQUAL    *pWorkCur = TrnGetCurQualPtr();

		// ensure that if this is a split tender that had an electronic payment before this tender
		// that we clear the various flags used to indicate special electronic payment receipt.
		pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP | CURQUAL_PRINT_EPT_LOGO);   // clear the bits before setting them in SetReceiptPrint()
	}
    ItemTendPartDisp( UifRegTender, ItemTender );                   /* display */
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);
    
    /*--- Display Tender, R3.0 ---*/
    MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
                                                
    if ( UifRegTender->fbModifier & VOID_MODIFIER ) {               /* void tender */
        ItemCommonPutStatus( COMMON_VOID_EC );                      /* E/C disable */
    }

    return( ITM_SUCCESS );                                          /* end */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendPartCheck( VOID ) 
*
*   Input:      none      
*   Output:     none   
*   InOut:      none
*
*   Return:     
*
*   Description:    check foreign currency partial tender
==========================================================================*/

SHORT   ItemTendFCPartCheck( VOID )
{
    if ( ItemModLocalPtr->fsTaxable ) {                                      /* depress taxable/exempt */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFCPartTender( UIFREGTENDER *UifRegTender,
*                       ITEMTENDER   *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     ITEMTENDER  *ItemTender   
*   InOut:      none
*
*   Return:     
*
*   Description:    generate foreign currency partial tender data   
==========================================================================*/

SHORT   ItemTendFCPartTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT           sStatus;
    USHORT          usSize;
    PARAMDC         WorkMDC;

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = CLASS_FOREIGN1 + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1;  /* Saratoga */

    memcpy( &ItemTender->aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTender->aszNumber) );
    if (UifRegTender->aszNumber[0] != 0) {              /* E-049 corr. 4/20 */
        memcpy( &ItemTenderLocal.aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTenderLocal.aszNumber) );
    }
    ItemTender->fbModifier = UifRegTender->fbModifier;

    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN1 ||          /* Saratoga */
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN2) {
        WorkMDC.usAddress = MDC_FC1_ADR + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1;
    } else {
        WorkMDC.usAddress = MDC_FC3_ADR + UifRegTender->uchMinorClass - CLASS_UIFOREIGN3;
    }
    CliParaRead( &WorkMDC );                                        /* get target MDC */

    ItemTender->auchTendStatus[0] = WorkMDC.uchMDCData;                 /* tender MDC */

    if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN2 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN4 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN6 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN8) {        /* FC tender */
        ItemTender->auchTendStatus[0] >>= 4;
    }

    /* V3.4 */
    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_EUROFC_ADR;
    CliParaRead( &WorkMDC );                                        /* get target MDC */

    ItemTender->auchTendStatus[1] = WorkMDC.uchMDCData;                 /* tender MDC */

    if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN1 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN3 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN5 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN7) {        /* FC tender */
        ItemTender->auchTendStatus[1] >>= 4;
    } else if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN2 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN4 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN6 ||
        UifRegTender->uchMinorClass == CLASS_UIFOREIGN8) {        /* FC tender */
        ItemTender->auchTendStatus[1] >>= 6;
    }

    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* for compress size */

    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    if ( ( sStatus = ItemCommonCheckSize( ItemTender, usSize ) ) < 0 ) {   /* storage full */
		TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();

		WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;                 /* storage status */
        return( LDT_TAKETL_ADR);                                
    } else {
        return( ITM_SUCCESS );
    }
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCPartSpl(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency partial tender
==========================================================================*/

SHORT   ItemTendFCPartSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sStatus, sSize;
    SHORT           sValPrintFlag = 0;  // validation print required flag
    TRANCURQUAL     WorkCur;

    TrnGetCurQual(&WorkCur);
    /*----- Not Use Total Key -----*/
    if (WorkCur.auchTotalStatus[0] == 0) {      // if not total key pressed then
        return(LDT_SEQERR_ADR);
    }

    if ((sStatus = ItemTendFCPartCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    if ((sSize = ItemTendFCPartTender(UifRegTender, ItemTender)) != ITM_SUCCESS) {
        return(LDT_TAKETL_ADR);
    }

    if ((sStatus = ItemTendPartSPVInt(UifRegTender)) != ITM_SUCCESS) {
        return(sStatus);
    }
                        
    if ( ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_FC0_VALIDATION )  /* validation print */
        || ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1 ) 
            && ( UifRegTender->fbModifier & VOID_MODIFIER ) ) ) {  
        ItemTender->fsPrintStatus = PRT_VALIDATION;                     /* print validation */ 
        TrnThrough( ItemTender );
		sValPrintFlag = 1;
    }

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sValPrintFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */

    /*----- Split Key -> Seat# -> Total Key -> Tender -----*/
    if (WorkCur.uchSeat != 0) {
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
		ITEMTENDER      DummyTender = {0};

        ItemTendAffectTaxSpl(WorkCur.auchTotalStatus);
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
            MldScrollWrite2(MLD_DRIVE_2, &WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            TrnThroughDisp(&WorkCommon->ItemSales); /* display on KDS */
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkCommon->ItemSales, 0);
        }
        /* ---- to affect previous partial tender 06/28/96 ---- */
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTENDER) {
			ITEMTENDER      *pPrevTender = (VOID *)&(WorkCommon->ItemSales);
			UCHAR           fbStorageSave;

            fbStorageSave = pPrevTender->fbStorageStatus;
            pPrevTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
            pPrevTender->fbStorageStatus |= NOT_SEND_KDS;
            TrnItem(pPrevTender);
            pPrevTender->fbStorageStatus = fbStorageSave;
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(pPrevTender, 0);
        }

		ItemPreviousItemSpl(&DummyTender, TRN_TYPE_SPLITA); /* Saratoga */

    /*----- Split Key -> Tender -----*/
    } else {
		ITEMTOTAL       ItemTotal = {0};

        /*----- Check Status 6/6/96 -----*/
        if ((sStatus =ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {
            return(sStatus);
        }
        ItemTendSplitCheckTotal(&ItemTotal, UifRegTender);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);
    }

    ItemTender->fbStorageStatus |= NOT_SEND_KDS;
    ItemPreviousItemSpl(ItemTender, TRN_TYPE_SPLITA);

    ItemTenderLocal.fbTenderStatus[0] |= TENDER_PARTIAL;        /* partial tend state */
    ItemTenderLocal.fbTenderStatus[1] |= TENDER_SEAT_PARTIAL;
    ItemTenderLocal.lTenderizer += ItemTender->lTenderAmount;   /* update tenderizer */
    ItemTenderLocal.fbTenderStatus[2] &= ~( TENDER_FC1 | TENDER_FC2| TENDER_FC3 | TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8);

    ItemTendPartDisp( UifRegTender, ItemTender );               /* display */
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);

	sStatus = TrnSplCheckSplit();
    if (sStatus == TRN_SPL_SEAT || sStatus == TRN_SPL_MULTI || sStatus == TRN_SPL_TYPE2) {
        MldScrollWrite2(MLD_DRIVE_2, ItemTender, MLD_NEW_ITEMIZE);
    } else {
        MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
    }
                                                
    if ( UifRegTender->fbModifier & VOID_MODIFIER ) {               /* void tender */
        ItemCommonPutStatus( COMMON_VOID_EC );                      /* E/C disable */
    }

	/* release slip to allow next check endorsement */
    if (WorkCur.uchSeat == 0) {
	    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    		if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				if (TrnPrtGetSlipPageLine()) {
	    			TrnSlipRelease();
	   			    ItemTenderLocal.usSlipPageLine = (TrnPrtGetSlipPageLine() % PRT_SLIP_MAX_LINES);
   				    ItemTenderLocal.fbSlibPageStatue = 0x01;
   				    TrnPrtSetSlipPageLine(0);
   				}
   			}
   		}
   	}
   	
    return( ITM_SUCCESS );                                          /* end */
}


/****** End of Source ******/
