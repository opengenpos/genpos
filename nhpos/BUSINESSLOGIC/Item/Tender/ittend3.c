/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTEND3.C (even/over tender)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTendOver()          ;   even/over tender   
:    
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date   : Ver.Rev.  : Name      : Description
: Mar-28-95: 03.00.00  : H.Kato    : R3.0
: Nov-22-95: 03.01.00  : H.Kato    : R3.1
: Aug-13-99: 03.05.00  : K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECLL_MODEL
: Nov-16-15: 02.02.01  : R.Chambers : source cleanup, localize, use CONST memory data
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
#include    "prt.h"

/*==========================================================================
**  Synopsis:   SHORT   ItemTendOverNor(UIFREGTENDER *UifRegTender,
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

static SHORT   ItemTendOverNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	ULONG           fsCurStatus;

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                       /* FS total */
    } else {
        if ( TranCurQualPtr->auchTotalStatus[0] == 0 ) {                      /* no total */
			ITEMTOTAL       ItemTotal = {0};

            ItemTendAffectTax(UifRegTender);                    /* tax */
            ItemPreviousItem(0, 0 );                            /* Saratoga */
            ItemTendTrayTotal();                                /* tray total */
            ItemTendTrayTax();                                  /* whole tax at tray transaction */   
            ItemTendVAT(UifRegTender);
            ItemTendCheckTotal(&ItemTotal, UifRegTender);       /* check total, R3.0 */
            MldDispSubTotal(MLD_TOTAL_1, ItemTotal.lMI);
        } else {                                                /* exist check total */
            ItemTendAffectTax(UifRegTender);                    /* tax */
            ItemTendTrayTotal();                                /* tray total */
            ItemTendTrayTax();                                  /* whole tax at tray transaction */   
            ItemTendVAT(UifRegTender);
            /*--- Display Total Key, R3.0 ---*/
            if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
				ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkCommon->ItemSales, 0);
                MldScrollWrite(&WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            }
            ItemPreviousItem(0, 0 );                 /* Saratoga */
        }
    }

    ItemTendMulti();                                            /* print data for multi check payment */

    ItemTendOverTender( UifRegTender, ItemTender );             /* tender */

	DisplayGiftCard();

    if ( CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);             /* drawer open */
       ItemTendCoin(ItemTender);
    }

    ItemTendAffection();                                        /* affection        */

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

	// keeping this temporary variable for now as it was the original and we
	// are not sure what functions called that may change TranCurQualPtr->fsCurStatus.
	//    Nov-16-2015 R. Chambers
	fsCurStatus = TranCurQualPtr->fsCurStatus;
    /* if (!(fsCurStatus & CURQUAL_PVOID)) { (*/
        ItemTendPrintFSChange();
    /* } */

	ItemTendSetPrintStatus (ItemTender);

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

	do {
		// determine which type of trailer to use.  the only time we should use an EPT
		// type of trailer is if this is not some kind of a split or partial tender and
		// it is an actual EPT tender.  Otherwise, we will use a standard trailer.
		if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0) {
			// this is not a partial or split tender, next check if this is a seat partial.
			if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) == 0) {
				// check if Food Stamp partial tender - applies for Amtrak Train Delay Voucher.
				if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0 && (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) == 0) {
					// ok this is not a seat partial, check to see if this is Electronic Payment
					if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT) != 0      /* exist EPT tend ? */
						&& CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {   /* EPT logo */
						// it is an Electronic Payment and we are supposed to do the EPT logo so EPT trailer it is.
						ItemTrailer( TYPE_EPT );                                /* EPT trailer      */
						break;
					}
				}
			}
		}
		// default is the standard type of non-Electronic Payment trailer.
		SetReceiptPrint(0);
		SetReceiptPrint(SET_RECEIPT_FIRST_COPY);
        ItemTrailer( TYPE_STORAGE );                            /* normal trailer   */
    } while (0);

    ItemTendClose();                                            /* transaction close */
    ItemTendDupRec();                                           /* EPT duplicate    */
    if (!(fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN))) {
        ItemTendFSChit();                                       /* generate F.S chit */
    }
    ItemTendDelGCF();                                           /* delete GCF */
    ItemTendInit();                                             /* initialize transaction information */
    ItemWICRelOnFin(fsCurStatus);
    return( ItemFinalize() );                                   /* tender finalize */
}


/*==========================================================================
**  Synopsis:   VOID    ItemTendOverTender( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:     
*
*   Description:    generate even/over tender data   
==========================================================================*/

VOID    ItemTendOverTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT           sValPrintFlag = 0;  // validation print required flag

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */

    if (ItemTender->aszNumber[0] == 0) {
        memcpy(ItemTender->aszNumber, UifRegTender->aszNumber, sizeof(ItemTender->aszNumber));
    }
    if (UifRegTender->aszNumber[0] != 0) {              /* E-049 corr. 4/20 */
        memcpy( &ItemTenderLocal.aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTenderLocal.aszNumber) );
    }
    if (ItemTender->auchExpiraDate[0] != 0) {              
        _tcsncpy( &ItemTenderLocal.auchExpiraDate[0], &ItemTender->auchExpiraDate[0], NUM_EXPIRA );
    }

	ItemTenderMdcToStatus (UifRegTender->uchMinorClass, ItemTender->auchTendStatus);

    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION ) {   /* validation print required */
        ItemTender->fsPrintStatus = PRT_VALIDATION;                           /* print validation */ 
        TrnThrough( ItemTender );
        sValPrintFlag = 1;
    }

    /* --- DTREE#2: Issue#3 Check Endorsement,  Dec/18/2000, 21RFC05402--- */
    if (!(UifRegTender->fbModifier & VOID_MODIFIER)) {
        ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
    }
    /* --- Dec/6/2000 --- */
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
            ItemTendAudVal(ItemTendWholeTransAmount());
	        sValPrintFlag = 1;	/* 06/22/01 */
        }
    }
    
    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */

    ItemTendPrtFSChange(ItemTender);                                /* generate FS change data */

    ItemTendOverDisp( UifRegTender, ItemTender );                   /* display */

    if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
	    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
		    (TranCurQualPtr->uchSeat == 0) && !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
			ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */

    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		} else {
	   		/* execute slip validation for partial ept tender, 09/12/01 */
    	    if ((ItemTender->fsPrintStatus & PRT_SLIP) && (ItemTender->uchCPLineNo != 0)) {                /* Saratoga */
				USHORT  fsPrintStatus;                  /* 51:print status */

	        	if (sValPrintFlag == 1) {
			        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
					        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    					} else {
		        	    	UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
			        	}
			        }
    	    	}
			    /* if (TranCurQualPtr->uchSeat) { */
			    	TrnPrtSetSlipPageLine(0);
			    /* } */
	    	   	fsPrintStatus = ItemTender->fsPrintStatus;
       			ItemTender->fsPrintStatus = PRT_SLIP;             			/* print slip/journal */
	   		    TrnThrough( ItemTender );
    	  		ItemTender->fsPrintStatus = fsPrintStatus;
      			ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	        	sValPrintFlag = 1;	/* 06/22/01 */
	        
	      		/* release slip to prevent the optional listing slip */
		        if ( !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
	    			TrnSlipRelease();
		   		}
	   		}
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
	
	// print split receipts.  needs to be here so that we have the EPT reponse text
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
**  Synopsis:   VOID    ItemTendOverDisp( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display of even/over tender data
==========================================================================*/

VOID    ItemTendOverDisp( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};

    DispMsg.uchMajorClass = UifRegTender->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTender->uchMinorClass;

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {    /* F.S tender performed */
        /* Second Food Stamp Tender, V2.01.04 */
        DispMsg.uchMinorClass = CLASS_UIFFSCHANGE;                  /* F.S class */
		DispMsg.lAmount = ItemTender->lChange;

		/* Second Food Stamp Tender, V2.01.04 */
		DispMsg.lQTY = ItemTenderLocal.lFS * 10L;
		RflGetTranMnem ( DispMsg.aszMnemonic, TRN_FSCHNG_ADR );
    } else {
        DispMsg.lAmount = ItemTender->lChange;                      /* change */
		RflGetTranMnem ( DispMsg.aszMnemonic, TRN_REGCHG_ADR );
    }

	// TODO: following was before changes which overwrites the mnemonic used regardless
	// of whether there is a food stamp change or not.
	//   Richard Chambers, Aug-28-2017
	RflGetTranMnem ( DispMsg.aszMnemonic, TRN_REGCHG_ADR );

    flDispRegDescrControl |= CHANGE_CNTRL;                          /* descriptor */
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_2;                       /* save for redisplay (not recovery) */

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {          /* PST exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
       if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    }

    ItemTendDispIn( &DispMsgBack );                                 /* generate display mnemonic data */
    flDispRegDescrControl &= ~( TVOID_CNTRL | TENDER_CNTRL );
    flDispRegKeepControl &= ~( TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL | TENDER_CNTRL | FOODSTAMP_CTL);

    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0 ) ) {       /* receipt control */
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }

    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift; /* set default adjective, 2172 */
        }
    }
    
    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */

//    MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);                     /* Saratoga */
//    MldScrollFileWrite(&WorkTend);

    flDispRegDescrControl &= ~( CHANGE_CNTRL | FOODSTAMP_CTL |TAXEXMPT_CNTRL );
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendOverSpl(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender           
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    Even/Over tender for split check that is divided into seats.
*
*                   There are two kinds of receipts generated by this function. The
*                   receipt printing behavior depends on whether a split has been done
*                   followed by specifying a particular seat number or if a split has
*                   been done using the Split Key however rather than then specifying a
*                   particular seat number and then doing a tender instead a tender is
*                   done for the entire transaction and not just a specific seat.
*
*                   The first type is a seat specific receipt in which the transaction
*                   details of a specific seat are printed.
*
*                   The second type provides a receipt for the complete transaction however
*                   the receipt details are printed seat by seat showing the items for each
*                   individual seat along with the tax calculation and the total for each
*                   seat.
*
*                   If MDC 99 and MDC 100 governing duplicated receipts for Charge Post or
*                   Electronic Payment (CP/EPT) then a duplicate receipt will also be
*                   printed for the second type.
==========================================================================*/

static SHORT   ItemTendOverSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
    SHORT       sFlag = 0;          // indicates if a specific seat or all seats to be printed.
    SHORT       sStatus;            /* 6/6/96 */

    if (TranCurQualPtr->uchSeat != 0) {
		// the split check is for a specific seat number.
		/*----- Split Key -> Seat# -> Total Key -> Tender -----*/
		ITEMTENDER  DummyTender = {0};
		UCHAR  auchTotalStatus[NUM_TOTAL_STATUS];

		ItemTendGetTotal(&auchTotalStatus[0], UifRegTender->uchMinorClass);
        ItemTendAffectTaxSpl(auchTotalStatus);            /* 5/30/96 */
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
            MldScrollWrite2(MLD_DRIVE_2, &WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            TrnThroughDisp(&WorkCommon->ItemSales); /* display on KDS */
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkCommon->ItemSales, 0);
			/* if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) { */
				/* for check endorsement calculation */
				memcpy(&ItemTenderLocal.ItemTotal, &WorkCommon->ItemSales, sizeof(ITEMTOTAL));
			/* } */
        }
        /* ---- to affect previous partial tender 06/28/96 ---- */
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTENDER) {
			ITEMTENDER  *pPrevTender = (ITEMTENDER *)&(WorkCommon->ItemSales);
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
        ItemTendOverTender(UifRegTender, ItemTender);
        ItemTendDispSpl();        
        sFlag = 0;           // receipt for single seat specified
    } else {
		// specific seat number has not been entered so print the receipt seat by seat with totals per seat.
		/*----- Split Key -> Tender -----*/
        if ((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC))
            || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC)))
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

            (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC))
            || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC)))
            && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||  
                (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF)) 
            && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2)))
		{
			// duplicated receipt indicated so copy transaction data to Post Receipt storage.
            TrnStoGetConsToPostR();         /* 6/5/96 */
        }
        /*----- Check Status 6/6/96 -----*/
        if ((sStatus = ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {  
            return(sStatus);
        }
        ItemTendOverTenderSplit(UifRegTender, ItemTender);       /* 96/4/1 */
        /* ItemTendOverTender(UifRegTender, ItemTender);       / 96/4/1 */
        /* ItemTendFastTenderSplit(UifRegTender, ItemTender); 96/4/1 */
        ItemTendAffection();
		sFlag = 1;           // remaining transaction data receipt printing requested.
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);
       ItemTendCoin(ItemTender);
    }

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

    if (sFlag == 0) {
		// specific seat was specified so print the transaction of the specific seat
        if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT          /* 5/28/96 exist EPT tend ? */
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {   /* 5/28/96 EPT logo */
            ItemTrailer( TYPE_SPLIT_EPT );                          /* 5/28/96 EPT trailer      */
        } else {
            ItemTrailer(TYPE_SPLITA);                               /* 5/28/96 */
        }
        ItemTendCloseSpl();

        ItemTenderLocal.lTenderizer = 0L;
        ItemTenderLocal.lFCMI = 0L;
        ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SEAT_PARTIAL;
        return(UIF_SEAT_TRANS);
    } else {
		// all remaining seats are to be printed using the seat by seat with tax and total per seat format.
        if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT          /* exist EPT tend ? */
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1))     /* print EPT logo */
		{
            ItemTrailer( TYPE_EPT );                                /* EPT trailer on receipt  */
        } else {
            ItemTrailer( TYPE_STORAGE );                            /* normal trailer on receipt  */
        }
        /* 5/28/96 ItemTrailer(TYPE_STORAGE); */
        ItemTendClose();
        ItemTendSplitValidation(UifRegTender, ItemTender);
        ItemTendDupRecSpl(ItemTender, UifRegTender);        // print duplicate receipt from Post Receipt storage.
        ItemTendDelGCF();
        ItemTendInit();
        return(ItemFinalize());
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendOver(UIFREGTENDER *UifRegTender,
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

SHORT   ItemTendOver( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);
       ItemTendCoin(ItemTender);
    }

    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2
        || TrnSplCheckSplit() == TRN_SPL_SPLIT)
	{
        return(ItemTendOverSpl(UifRegTender, ItemTender));
    } else {
        return(ItemTendOverNor(UifRegTender, ItemTender));
    }
}


/****** End of Source ******/
