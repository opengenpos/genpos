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
:   Program Name   : ITTENDF3.C (foreign currency even/over tender)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTendFCOver()        ;   foreign currency even/over tender   
:    
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date    :  Version  :   Name    :   Description
:   95/3/28   03.00.00  : hkato     : R3.0
:   95/11/22  03.01.00  : hkato     : R3.1
:  Aug-13-99: 03.05.00  : K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
:  Dec-06-99: 01.00.00  : hrkato    : Saratoga
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
#include    "regstrct.h"
#include    "paraequ.h"
#include    "para.h"
#include    "uireg.h"
#include    "display.h"
#include    "mld.h"
#include    "transact.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "rfl.h"
#include    "cpm.h"
#include    "itmlocal.h"
#include	"prt.h"

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCOver(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    even/over tender
==========================================================================*/

SHORT   ItemTendFCOver(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	SHORT  sSplitStatus = TrnSplCheckSplit();

    /*----- Split Check,   R3.1 -----*/
    if (sSplitStatus == TRN_SPL_SEAT
        || sSplitStatus == TRN_SPL_MULTI
        || sSplitStatus == TRN_SPL_TYPE2
        || sSplitStatus == TRN_SPL_SPLIT)
	{
        return(ItemTendFCOverSpl(UifRegTender, ItemTender));
    } else {
        return(ItemTendFCOverNor(UifRegTender, ItemTender));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCOverNor(UIFREGTENDER *UifRegTender,

*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency even/over tender
==========================================================================*/

SHORT   ItemTendFCOverNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	ULONG           fsCurStatus;
    SHORT           sStatus;
	ITEMTENDER      DummyTender = {0};

    if ( ( sStatus = ItemTendFCPartCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }   
                                                              
    if ( CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemDrawer();
       ItemTendCoin(ItemTender);
    }

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                       /* FS total */
    } else {
        ItemTendAffectTax(UifRegTender);
        ItemTendTrayTotal();                                    /* tray total */
        ItemTendTrayTax();                                      /* whole tax at tray transaction */   
    }

    if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {        /* R3.0 */
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

        MldScrollWrite(&WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&WorkCommon->ItemSales, 0);
    }
    ItemTendVAT(UifRegTender);

	ItemPreviousItem(&DummyTender, 0 );                                 /* Saratoga */
    ItemTendMulti();                                                    /* print data for multi check payment */
    ItemTendFCOverTender( UifRegTender, ItemTender, FALSE );                   /* tender */
	DisplayGiftCard();

    if ( CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemDrawer();                                                       /* drawer open */       
       ItemTendCoin(ItemTender);
    }
    if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {      /* drawer compulsory */
        UieDrawerComp( UIE_ENABLE );  
	} else {
		UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
	}

    ItemTendAffection();                                                /* affection */

    ItemTendEuroTender(ItemTender);                                     /* Euro trailer, V3.4 */

	{
		TRANCURQUAL    *pWorkCur = TrnGetCurQualPtr();

		fsCurStatus = pWorkCur->fsCurStatus;                            /* Saratoga */
		/* if (!(fsStatus & CURQUAL_PVOID)) { */
			ItemTendPrintFSChange();
		/* } */

		// ensure that if this is a split tender that had an electronic payment before this tender
		// that we clear the various flags used to indicate special electronic payment receipt.
		pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP | CURQUAL_PRINT_EPT_LOGO);   // clear the bits before setting them in SetReceiptPrint()
	}
#if 0
	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID) {
		SetReceiptPrint(SET_RECEIPT_TRAN_PREPAID);
	}
	else {
		SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
	}

    if (ItemTendIsDup()) {
		SetReceiptPrint(SET_RECEIPT_FIRST_COPY);
	}
#endif

    ItemTrailer( TYPE_STORAGE );                                /* strored normal trailer */

    ItemTendClose();                                            /* transaction close */
    ItemTendDupRec();                                           /* EPT duplicate    */
    if (!(fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN))) {
        ItemTendFSChit();                                       /* generate F.S chit */
    }
    ItemTendDelGCF();                                           /* delete GCF */
    ItemTendInit();                                             /* initialize transaction information */
    ItemWICRelOnFin(fsCurStatus);
    return(ItemFinalize());                                     /* tender finalize */
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFCOverTender( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:     
*
*   Description:    generate foreign currency even/over tender data   
==========================================================================*/

VOID    ItemTendFCOverTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, USHORT fsSplit )
{
    SHORT           sValPrintFlag = 0;  // validation print required flag
	PARAMDC         WorkMDC = {0};

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = CLASS_FOREIGN1 + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1;  /* Saratoga */

    memcpy( &ItemTender->aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTender->aszNumber) );
    if (UifRegTender->aszNumber[0] != 0) {              /* E-049 corr. 4/20 */
        memcpy( &ItemTenderLocal.aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTenderLocal.aszNumber) );
    }

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

	if (fsSplit == FALSE) {	/* for splited validation and GCF */
	    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_FC0_VALIDATION ) {  /* validation print */
	        ItemTender->fsPrintStatus = PRT_VALIDATION;                     /* print validation */ 
		    TrnThrough( ItemTender );
			sValPrintFlag = 1;
	    }

		/* --- DTREE #2,    Dec/6/2000 --- */
	    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2)) {
			if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        		if (sValPrintFlag == 1) {
	        		if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			        		UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
		    			} else {
				            UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
				        }
					}
		        }
		        ItemTendAudVal(ItemTendWholeTransAmount());            /* audaction validation */
			    sValPrintFlag = 1;
	        }
		}
	    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
			if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			    if (sValPrintFlag == 1) {
				    UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
				}
			}
		}
		/* request paper at previous splited gcf */
		if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
	    	if (TranCurQualPtr->uchSeat == 0) {
			    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
   	
						if (ItemTenderLocal.fbSlibPageStatue) {
							ITEMOTHER TrnItemOther = {0};

			   			    TrnPrtSetSlipPageLine(0);
						    if (ItemTenderLocal.usSlipPageLine == 0) {
								ItemTenderLocal.usSlipPageLine++;
							}
		    				TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;   /* set class code */   
						    TrnItemOther.uchMinorClass = CLASS_INSIDEFEED;
					    	TrnItemOther.fsPrintStatus = PRT_SLIP; /* set slip bit on other data */
        					TrnItemOther.lAmount = ItemTenderLocal.usSlipPageLine;
			   			    TrnThrough( &TrnItemOther );
			   			}
					}
				}
    		}
		}

	}

    
    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */

    ItemTendPrtFSChange(ItemTender);                                /* FS change */

    ItemTendOverDisp( UifRegTender, ItemTender );                   /* display */
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);
    
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        TrnItemSpl(ItemTender, TRN_TYPE_SPLITA);
        ItemTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
        TrnItem(ItemTender);
        MldScrollWrite2(MLD_DRIVE_2, ItemTender, MLD_NEW_ITEMIZE);

    } else {
        TrnItem(ItemTender);                                        /* transaction module i/F */
        MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(ItemTender);
    }
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCOverSpl(UIFREGTENDER *UifRegTender,

*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency even/over tender             R3.1
==========================================================================*/

SHORT   ItemTendFCOverSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT       sSplitFlag = 0;                  /* 6/7/96 */
    SHORT       sStatus;

    /*----- Not Use Total Key -----*/
    if (TranCurQualPtr->auchTotalStatus[0] == 0) {      // if not total key pressed then
        return(LDT_SEQERR_ADR);
    }

    if ((sStatus = ItemTendFCPartCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemDrawer();
       ItemTendCoin(ItemTender);
    }
    /*----- Split Key -> Seat# -> Total Key -> Tender -----*/
    if (TranCurQualPtr->uchSeat != 0) {
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
		ITEMTENDER      DummyTender = {0};

        ItemTendAffectTaxSpl(TranCurQualPtr->auchTotalStatus);
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
            MldScrollWrite2(MLD_DRIVE_2, &WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            TrnThroughDisp(&WorkCommon->ItemSales); /* display on KDS */
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkCommon->ItemSales, 0);
        }

        /* ---- to affect previous partial tender 06/28/96 ---- */
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTENDER) {
			ITEMTENDER  *pPrevTender = (VOID *)&(WorkCommon->ItemSales);
			UCHAR       fbStorageSave;

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
		ITEMTOTAL   ItemTotal = {0};

        /*----- Check Status 6/6/96 -----*/
        if ((sStatus = ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {
            return(sStatus);
        }
        ItemTendSplitCheckTotal(&ItemTotal, UifRegTender);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);
        ItemTendAffection();
        MldScrollWrite2(MLD_DRIVE_2, &ItemTotal, MLD_NEW_ITEMIZE);
        MldDispSubTotal(MLD_TOTAL_2, ItemTotal.lMI);
        TrnThroughDisp(&ItemTotal); /* display on KDS */
        sSplitFlag = 1;             /* 6/7/96 */
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&ItemTotal, 0);
    }

    if (sSplitFlag == 0) {               /* 6/7/96 */
	    ItemTendFCOverTender(UifRegTender, ItemTender, FALSE);
        ItemTendDispSpl();        
	} else {
	    ItemTendFCOverTender(UifRegTender, ItemTender, TRUE);
	}

	if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemDrawer();
       ItemTendCoin(ItemTender);
    }
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
        UieDrawerComp(UIE_ENABLE);
	} else {
		UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
	}

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

    if (sSplitFlag == 0) {               /* 6/7/96 */
        ItemTrailer(TYPE_SPLITA);
        ItemTendCloseSpl();

        ItemTenderLocal.lTenderizer = 0L;
        ItemTenderLocal.lFCMI = 0L;
        ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SEAT_PARTIAL;
        return(UIF_SEAT_TRANS);
    } else {                        /* 6/7/96 */    
        ItemTrailer(TYPE_STORAGE);
        ItemTendClose();
    	ItemTendFCSplitValidation(UifRegTender, ItemTender);
        ItemTendDelGCF();
        ItemTendSaved(ItemTender);
        ItemTendInit();
        return(ItemFinalize());
    }
}

/****** End of Source ******/
