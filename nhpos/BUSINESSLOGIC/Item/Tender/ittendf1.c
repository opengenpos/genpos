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
:   Program Name   : ITTENDF1.C (foreign currency without amount)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTendFCFast()        ;   w/o amount tender   
:    
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date    :  Version  :   Name    :   Description
:  2/14/95    03.00.00  : hkato     : R3.0
: 11/22/95    03.01.00  : hkato     : R3.1
: Aug-13-99: 03.05.00  : K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: Dec-06-99: 01.00.00  : hrkato    : Saratoga
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
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "itmlocal.h"
#include	"prt.h"

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCFast(UIFREGTENDER *UifRegTender, 
*                           ITEMTENDER  *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender           
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency w/o amount tender
==========================================================================*/

SHORT   ItemTendFCFast(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    /*----- During Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(ItemTendFCFastSpl(UifRegTender, ItemTender));

    } else {
        return(ItemTendFCFastNor(UifRegTender, ItemTender));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCFastNor(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency w/o amount tender
==========================================================================*/

SHORT   ItemTendFCFastNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sStatus;
	ULONG           fsCurStatus = 0;

    if ((sStatus = ItemTendFCFastCheck(UifRegTender->uchMinorClass)) != ITM_SUCCESS) {
		// if amount required is set in the MDC yet the transaction amount is less than or
		// equal to zero then there is a change due to the customer so just allow
		// processing to continue anyway.  Only display Amount Requred if a positive
		// amount meaning that the customer owes money.
		if (sStatus == LDT_AMT_ENTRY_REQUIRED_ADR && ItemTender->lRound > 0)
			return(sStatus);
    }

    if ((sStatus = ItemTendSPVInt(ItemTender)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemDrawer();
    }

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                   /* FS total */
    } else {
		ITEMTENDER      DummyTender = {0};

        if (TranCurQualPtr->auchTotalStatus[0] == 0) {      // if not total key pressed then
			ITEMTOTAL       ItemTotal = {0};

            ItemPreviousItem(&DummyTender, 0);              /* Saratoga */
            ItemTendAffectTax(UifRegTender);                /* tax */
            ItemTendTrayTotal();                            /* tray total */
            ItemTendTrayTax();                              /* whole tax at tray transaction */   
            ItemTendVAT(UifRegTender);
            ItemTendCheckTotal(&ItemTotal, UifRegTender);   /* check total, R3.0 */
            MldDispSubTotal(MLD_TOTAL_1, ItemTotal.lMI + ItemTransLocalPtr->lWorkMI);
        } else {                                            /* exist check total */
            ItemTendAffectTax(UifRegTender);                /* tax */
            ItemTendTrayTotal();                            /* tray total */
            ItemTendTrayTax();                              /* whole tax at tray transaction */   
            ItemTendVAT(UifRegTender);
            if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
				ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();     /* get common local data */

                MldScrollWrite(&WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkCommon->ItemSales, 0);
            }
            ItemPreviousItem(&DummyTender, 0);              /* Saratoga */
        }
    }

    ItemTendMulti();                                        /* print data for multi check */
    ItemTendFCFastTender(UifRegTender, ItemTender, FALSE);         /* tender */
	DisplayGiftCard();

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemDrawer();                                        /* drawer open */
    }
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
        UieDrawerComp(UIE_ENABLE);
	} else {
		UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
	}

    ItemTendAffection();                                    /* affection        */
    ItemTendEuroTender(ItemTender);                         /* Euro trailer, V3.4 */

	{
		TRANCURQUAL    *pWorkCur = TrnGetCurQualPtr();

		fsCurStatus = pWorkCur->fsCurStatus;                             /* Saratoga */
		/* if (!(fsStatus & CURQUAL_PVOID)) { */
			ItemTendPrintFSChange();
		/* } */

		// ensure that if this is a split tender that had an electronic payment before this tender
		// that we clear the various flags used to indicate special electronic payment receipt.
		pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP | CURQUAL_PRINT_EPT_LOGO);   // clear the bits before setting them in SetReceiptPrint()
	}

#if 0
	{
		PARATENDERKEYINFO TendKeyInfo = {0};

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
	}
#endif

    ItemTrailer(TYPE_STORAGE);                                  /* normal trailer   */

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
**  Synopsis:   SHORT   ItemTendFCFastCheck(VOID)
*
*   Input:      none    
*   Output:     none   
*   InOut:      none
*
*   Return:     
*
*   Description:    check foreign currency w/o amount tender
==========================================================================*/

SHORT   ItemTendFCFastCheck(UCHAR uchMinorClass)
{
	USHORT          usMdcAmountEntryRequired = 0;

    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {
        if ( ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT ) {    
            return( LDT_SEQERR_ADR );                               /* sequence error */
        }
    }

    switch (uchMinorClass) {
    case CLASS_UIFOREIGN1:
		usMdcAmountEntryRequired = MDC_FC1_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN2:
		usMdcAmountEntryRequired = MDC_FC2_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN3:
		usMdcAmountEntryRequired = MDC_FC3_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN4:
		usMdcAmountEntryRequired = MDC_FC4_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN5:
		usMdcAmountEntryRequired = MDC_FC5_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN6:
		usMdcAmountEntryRequired = MDC_FC6_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN7:
		usMdcAmountEntryRequired = MDC_FC7_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

    case CLASS_UIFOREIGN8:
		usMdcAmountEntryRequired = MDC_FC8_ADR;      // ItemTendFCFastCheck() check Foreign currency fast check allowed
        break;

	default:
		NHPOS_ASSERT_TEXT((uchMinorClass == CLASS_UIFOREIGN1), "ItemTendFCFastCheck(): illegal foreign tender specified.");
        return( LDT_SEQERR_ADR );                               /* sequence error */
		break;
    }

	// Check to see if we are requiring amount entry for this tender key.  If so then prohibit fast tender
	// since fast tender does it for the amount of transaction without requiring amount entry.
	if (usMdcAmountEntryRequired && CliParaMDCCheckField(usMdcAmountEntryRequired, MDC_PARAM_BIT_A) ) {
		return( LDT_AMT_ENTRY_REQUIRED_ADR );
	}
    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFCFastTender( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender         
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:     
*
*   Description:    generate foreign currency tender data   
==========================================================================*/

VOID    ItemTendFCFastTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, USHORT fsSplit )
{
    SHORT       sValPrintFlag = 0;  // validation print required flag
    DCURRENCY   lMI;
    TRANCURQUAL WorkCur;
	PARAMDC     WorkMDC = {0};

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;
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

    lMI = ItemTender->lRound;

    TrnGetCurQual( &WorkCur );
    if ( WorkCur.fsCurStatus & CURQUAL_PRESELECT_MASK ) {                    /* preselect void */
        lMI *= -1;
    }

	if (fsSplit == FALSE) {	/* for splited validation and GCF */

	    /* --- Dec/6/2000 --- */
    	if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_FC0_VALIDATION ) {
 	       	ItemTender->fsPrintStatus = PRT_VALIDATION;                     /* print validation */ 
	        TrnThrough( ItemTender );
        	sValPrintFlag = 1;
	    }

    	/* --- Dec/6/2000 --- */
	    TrnGetCurQual( &WorkCur );
    	if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT3 ) && lMI <= 0L ) {
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
	    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2) && (WorkCur.fsCurStatus & CURQUAL_PRESELECT_MASK)) {
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
	    /* --- DTREE #2 --- */
    	if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
	    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
    	        if (sValPrintFlag == 1) {
			        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
				}
			}
		}

		/* request paper at previous splited gcf */
	    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
	    	if (WorkCur.uchSeat == 0) {
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

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {
        ItemTendOverDisp(UifRegTender, ItemTender);
    } else {
        ItemTendFCFastDisp(UifRegTender, ItemTender);
    }
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);
    
    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        TrnItemSpl(ItemTender, TRN_TYPE_SPLITA);
        ItemTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
        TrnItem(ItemTender);
        MldScrollWrite2(MLD_DRIVE_2, ItemTender, MLD_NEW_ITEMIZE);

    } else {
        TrnItem(ItemTender);
        MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(ItemTender);
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFCFastDisp( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display foreign currency tender amount   
==========================================================================*/

VOID    ItemTendFCFastDisp( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    REGDISPMSG      DispMsg, DispMsgBack;
    PARATRANSMNEMO  WorkMnem;

    WorkMnem.uchMajorClass = CLASS_PARATRANSMNEMO;                  
    if (UifRegTender->uchMinorClass <= CLASS_UIFOREIGN2) {          /* Saratoga */
        WorkMnem.uchAddress = TRN_FT1_ADR  + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1;
    } else {
        WorkMnem.uchAddress = TRN_FT3_ADR  + UifRegTender->uchMinorClass - CLASS_UIFOREIGN3;
    }

    CliParaRead( &WorkMnem );                                       /* get target mnemonic */

    memset( &DispMsg, 0, sizeof( REGDISPMSG ) );                    /* initialize "item" */
    DispMsg.uchMajorClass = UifRegTender->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTender->uchMinorClass;

    DispMsg.lAmount = ItemTender->lForeignAmount;                   /* tender amount */

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_3 ) {
        DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_3;               /* save for redisplay (not recovery) */

        if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_0 ) {
            DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_0;           /* save for redisplay (not recovery) */
        }
    }

    _tcsncpy( &DispMsg.aszMnemonic[0], &WorkMnem.aszMnemonics[0], NUM_TRANSMNEM );

    flDispRegDescrControl |= TENDER_CNTRL;                          /* descriptor */
    DispMsg.fbSaveControl |= 2;                                     /* save for redisplay (not recovery) */

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
         flDispRegDescrControl |= TAXEXMPT_CNTRL;                   /* tax exempt */
    }

    ItemTendDispIn( &DispMsgBack );                                 /* generate display mnemonic data */
    flDispRegKeepControl &= ~( TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL | TENDER_CNTRL );

    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0 ) ) {       /* receipt control */
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
    
    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSalesLocal = ItemSalesGetLocalPointer();
        uchDispCurrMenuPage = pWorkSalesLocal->uchMenuShift;               /* set default menu no. */
        if (pWorkSalesLocal->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSalesLocal->uchAdjCurShift = pWorkSalesLocal->uchAdjDefShift;  /* set default adjective, 2172 */
        }
    }
 
    flDispRegDescrControl &= ~TVOID_CNTRL;
    
    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */

    flDispRegDescrControl &= ~TENDER_CNTRL;                         /* descriptor */
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCFastSpl(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency w/o amount tender            R3.1
==========================================================================*/

SHORT   ItemTendFCFastSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sStatus, sSplitFlag = 0;
    TRANCURQUAL     WorkCur;

    if ((sStatus = ItemTendFCFastCheck(UifRegTender->uchMinorClass)) != ITM_SUCCESS) {
		// if amount required is set in the MDC yet the transaction amount is less than or
		// equal to zero then there is a change due to the customer so just allow
		// processing to continue anyway.  Only display Amount Requred if a positive
		// amount meaning that the customer owes money.
		if (sStatus == LDT_AMT_ENTRY_REQUIRED_ADR && ItemTender->lRound > 0)
			return(sStatus);
    }

    if ((sStatus = ItemTendSPVInt(ItemTender)) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemDrawer();
    }

    TrnGetCurQual(&WorkCur);
    for (;;) {
        /*----- Split Key -> Seat# -> Tender -----*/
        if (WorkCur.uchSeat != 0 && WorkCur.auchTotalStatus[0] == 0) {
            return(LDT_SEQERR_ADR);
        }
        /*----- Split Key -> Seat# -> Total Key -> Tender -----*/
        if (WorkCur.uchSeat != 0 && WorkCur.auchTotalStatus[0] != 0) {
			ITEMCOMMONLOCAL   *WorkCommon = ItemCommonGetLocalPointer();
			CONST ITEMTENDER  DummyTender = {0};

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
            break;
        }
        /*----- Split Key -> Total Key -> Tender -----*/
        /*----- Check Status 6/6/96 -----*/
        if ((sStatus =ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {  
            return(sStatus);
        }
        ItemTendAffection();
        sSplitFlag = 1;
        break;
    }

    if (sSplitFlag == 0) {
    	ItemTendFCFastTender(UifRegTender, ItemTender, FALSE);
        ItemTendDispSpl();        
    } else {
	    ItemTendFCFastTender(UifRegTender, ItemTender, TRUE);
	}

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemDrawer();
    }
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
        UieDrawerComp(UIE_ENABLE);
	} else {
		UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
	}

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

    if (sSplitFlag == 0) {
        ItemTrailer(TYPE_SPLITA);
        ItemTendCloseSpl();

        ItemTenderLocal.lTenderizer = 0L;
        ItemTenderLocal.lFCMI = 0L;
        ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SEAT_PARTIAL;
        return(UIF_SEAT_TRANS);

    } else {
        ItemTrailer(TYPE_STORAGE);
        ItemTendClose();
    	ItemTendFCSplitValidation(UifRegTender, ItemTender);
        ItemTendDelGCF();
        ItemTendSaved(ItemTender);
        ItemTendInit();
        return(ItemFinalize());
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFCSplitValidation( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender         
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:     
*
*   Description:    generate foreign currency tender data   
==========================================================================*/

VOID    ItemTendFCSplitValidation( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT       sValPrintFlag = 0;  // validation print required flag
    DCURRENCY   lMI;
    PARAMDC     WorkMDC;

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;
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

    lMI = ItemTender->lRound;

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {                    /* preselect void */
        lMI *= -1;
    }

    /* --- Dec/6/2000 --- */
    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_FC0_VALIDATION ) {
        ItemTender->fsPrintStatus = PRT_VALIDATION;                     /* print validation */ 
        TrnThrough( ItemTender );
        sValPrintFlag = 1;
    }

    /* --- Dec/6/2000 --- */
    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT3 ) && lMI <= 0L ) {
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
    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2) && (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK)) {
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
    /* --- DTREE #2 --- */

}

/****** end of file ******/