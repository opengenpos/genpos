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
:   Program Name   : ITTEND2.C (partial tender)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTendPart()          ;   partial tender   
:   ItemTendPartCheck()     ;   check partial tender   
:   ItemTendPartSPVInt()    ;   supervisor intervention   
:   ItemTendPartTender()    ;   generate partial tender data   
:   ItemTendPartDisp()      ;   display of partial tender
:  
:  
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date   : Ver.Rev.  : Name      : Description
: Mar-28-95: 03.00.00  : H.Kato    : R3.0
: Nov-22-95: 03.01.00  : H.Kato    : R3.1
: Aug-13-99: 03.05.00  : K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
:: GenPOS
: Jul-28-17: 02.02.02  : R.Chambers : slimmed down function ItemTendPartDisp(), use RflGetLead().
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
#include    "rfl.h"
#include    "itmlocal.h"
#include    "prt.h"


/*==========================================================================
**  Synopsis:   SHORT   ItemTendPart(UIFREGTENDER *UifRegTender,
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

SHORT   ItemTendPart(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(ItemTendPartSpl(UifRegTender, ItemTender));

    } else {
        return(ItemTendPartNor(UifRegTender, ItemTender));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendPartNor(UIFREGTENDER *UifRegTender,
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

SHORT   ItemTendPartNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sValPrintFlag = 0;
    USHORT  fsPrintStatus;                  /* 51:print status */

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                       /* FS total */
        if ((ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION)    /* validation print required */
            || (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1) 
            && (UifRegTender->fbModifier & VOID_MODIFIER))) {
            ItemTender->fsPrintStatus = PRT_VALIDATION;
            TrnThrough(ItemTender);
            sValPrintFlag = 1;
        }
        /* --- DTREE#2 Issue#3 Check Endorsement,   Dec/18/2000, 21RFC05402 --- */
        if (!(UifRegTender->fbModifier & VOID_MODIFIER)) {
			/* V1.00.25 */
   	        ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
			/* V1.00.12 */
/***
		    TrnGetCurQual(&WorkCur);
    		if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
        		if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
	        	    ItemTendAddTaxSum(&lMI);
					ItemCommonCheckEndorse(ItemTender->uchMinorClass,
    	        	    ItemTender->lTenderAmount, ItemTendWholeTransAmount()+lMI, &sFlag);
	    	    } else {
		            ItemCommonCheckEndorse(ItemTender->uchMinorClass,
    		            ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sFlag);
    	    	}
	    	} else {
    	        ItemCommonCheckEndorse(ItemTender->uchMinorClass,
        	        ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sFlag);
        	}
***/
        }
        /* --- Dec/6/2000 --- */
        ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        ItemTender->fbStorageStatus |= NOT_SEND_KDS;            /* not send to KDS at next item R3.1 */
   		if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
			SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
	        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
			ItemTenderLocal.fbTenderStatus[0] |= (TENDER_EPT | TENDER_CPPARTIAL);

		    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
			    (TranCurQualPtr->uchSeat == 0) && !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
     				ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
	      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
			} else {
    	   		/* execute slip validation for partial ept tender, 09/12/01 */
		        if ((ItemTender->fsPrintStatus & PRT_SLIP) &&        /* Saratoga */
   	    		    (ItemTender->uchCPLineNo != 0)) {                /* Saratoga */
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
#if 1
				    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) && (TranCurQualPtr->uchSeat == 0)) {
		    	    	if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) && /* only slip validation, 03/23/01 */
    						 CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			      			/* release slip to prevent the optional listing slip */
	    					TrnSlipRelease();
   		    			}
   		    		}
   		    	}
#endif
	     	}
	    }

        ItemPreviousItem(ItemTender, 0);
        ItemPreviousItem( 0, 0 );                               /* send the tender to transaction file */

    } else {
        if ( TranCurQualPtr->auchTotalStatus[0] == 0 ) {            // if not total key pressed then
			ITEMTOTAL       ItemTotal = {0};

            ItemTendAffectTax(UifRegTender);                        /* tax */
            ItemTendTrayTotal();                                    /* tray total */
            ItemTendTrayTax();                                      /* whole tax at tray transaction */
            ItemTendVAT(UifRegTender);
            ItemTendCheckTotal(&ItemTotal, UifRegTender);           /* check total, R3.0 */

            if ( ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION )   /* validation print required */
                || ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1 ) && ( UifRegTender->fbModifier & VOID_MODIFIER ) ) ) {  
                ItemTender->fsPrintStatus = PRT_VALIDATION;             /* print validation */ 
                TrnThrough( ItemTender );
                sValPrintFlag = 1;
            }

            /* --- DTREE#2 Issue#3 Check Endorsement,   Dec/18/2000, 21RFC05402 --- */
            if (!(UifRegTender->fbModifier & VOID_MODIFIER)) {
                ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
            }
            ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;             /* print slip/journal */
            ItemTender->fbStorageStatus |= NOT_SEND_KDS;            /* not send to KDS at next item R3.1 */
    		if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
				SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
		        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
				ItemTenderLocal.fbTenderStatus[0] |= (TENDER_EPT | TENDER_CPPARTIAL);

			    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
				    (TranCurQualPtr->uchSeat == 0) && !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
      				ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
		      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
				} else {
	    	   		/* execute slip validation for partial ept tender, 09/12/01 */
			        if ((ItemTender->fsPrintStatus & PRT_SLIP) &&        /* Saratoga */
    	    		    (ItemTender->uchCPLineNo != 0)) {                /* Saratoga */
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

					    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
						    (TranCurQualPtr->uchSeat == 0)) {
			    	    	if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) && /* only slip validation, 03/23/01 */
	    						 CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				      			/* release slip to prevent the optional listing slip */
	    						TrnSlipRelease();
	   		    			}
	   		    		}
	   		    	}
		     	}
		    }
            ItemPreviousItem(ItemTender, 0);                        /* buffering */
            ItemPreviousItem( 0, 0 );                               /* send the tender to transaction file */

        } else {                                                    /* exist check total */
            ItemTendAffectTax(UifRegTender);                        /* calculate tax */
            ItemTendTrayTotal();                                    /* tray total */
            ItemTendTrayTax();                                      /* whole tax at tray transaction */   
            if ((ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION) /* validation print required */
                || ( CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1 ) && ( UifRegTender->fbModifier & VOID_MODIFIER ) ) ) {  
                ItemTender->fsPrintStatus = PRT_VALIDATION;             /* print validation */ 
                TrnThrough( ItemTender );
                sValPrintFlag = 1;
            }
            /* --- DTREE#2 Issue#3 Check Endorsement,   Dec/18/2000, 21RFC05402 --- */
            if (!(UifRegTender->fbModifier & VOID_MODIFIER)) {
				/* V1.00.12 */
	    		if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
        			if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
					    if (ItemCommonTaxSystem() == ITM_TAX_US) {
							DCURRENCY    lMI = 0L;

	        				ItemTendAddTaxSum(&lMI);
							ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount() + lMI, &sValPrintFlag);
    	        		} else {
							ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTender->lRound, &sValPrintFlag);
    	        		}
		    	    } else {
			            ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
    	    		}
	    		} else {
    		        ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
	        	}
            }
            ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;             /* print slip/journal */
            ItemTender->fbStorageStatus |= NOT_SEND_KDS;            /* not send to KDS at next item R3.1 */
            ItemTendVAT(UifRegTender);
            if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
				ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkCommon->ItemSales, 0);
                MldScrollWrite(&WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            }

			if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
				SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
		        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
				ItemTenderLocal.fbTenderStatus[0] |= (TENDER_EPT | TENDER_CPPARTIAL);

			    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
				    (TranCurQualPtr->uchSeat == 0) && ! CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
      				ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
		      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
				} else {
        			/* execute slip validation for partial ept tender, 09/12/01 */
			        if ((ItemTender->fsPrintStatus & PRT_SLIP) &&        /* Saratoga */
    	    		    (ItemTender->uchCPLineNo != 0)) {                /* Saratoga */
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

					    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) && (TranCurQualPtr->uchSeat == 0)) {
					        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) && /* only slip validation, 03/23/01 */
    							 CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
					      		/* release slip to prevent the optional listing slip */
	    						TrnSlipRelease();
			   		    	}
		   		    	}
	   		    	}
		     	}
		    }
            ItemPreviousItem( ItemTender, 0 );                      /* buffering */
            ItemPreviousItem( 0, 0 );                               /* send the tender to transaction file */
        }
    }

    ItemTendMulti();                                            /* print data for multi check */

    ItemTenderLocal.fbTenderStatus[0] |= TENDER_PARTIAL;        /* partial tend state */

    if (ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC)) {
        ItemTenderLocal.fbTenderStatus[0] |= TENDER_CPPARTIAL;  /* CP partial tend state */
    }

    ItemTenderLocal.lTenderizer += ItemTender->lTenderAmount;   /* update tenderizer */
    if (ItemTender->auchExpiraDate[0] != 0) {               
        _tcsncpy(ItemTenderLocal.auchExpiraDate, ItemTender->auchExpiraDate, NUM_EXPIRA);
    }

	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);

    ItemTendPartDisp( UifRegTender, ItemTender );               /* display */
    MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);

    if ( UifRegTender->fbModifier & VOID_MODIFIER ) {           /* void tender */
        ItemCommonPutStatus( COMMON_VOID_EC );                  /* E/C disable */
    } else {
        ItemCommonResetStatus(COMMON_VOID_EC);                  /* E/C Enable, Saratoga */
    }

    return( ITM_SUCCESS );                                      /* end */
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTendPartTender( UIFREGTENDER *UifRegTender,
*                       ITEMTENDER   *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     ITEMTENDER  *ItemTender   
*   InOut:      none
*
*   Return:     
*
*   Description:    generate partial tender data   
==========================================================================*/

SHORT   ItemTendPartTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT           sStatus;
    USHORT          usSize;

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */

    if (ItemTender->aszNumber[0] == 0) {
        memcpy(ItemTender->aszNumber, UifRegTender->aszNumber, sizeof(ItemTender->aszNumber));
    }
    if (UifRegTender->aszNumber[0] != 0) {              /* E-049 corr. 4/20 */
        memcpy( &ItemTenderLocal.aszNumber[0], &UifRegTender->aszNumber[0], sizeof(ItemTenderLocal.aszNumber) );
    }
    ItemTender->fbModifier = UifRegTender->fbModifier;

	ItemTenderMdcToStatus (UifRegTender->uchMinorClass, ItemTender->auchTendStatus);

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
**  Synopsis:   SHORT   ItemTendPartSPVInt( UIFREGTENDER *UifRegTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender      
*   Output:     none    
*   InOut:      none
*
*   Return:     
*                                       
*   Description:    partial tender supervisor intervention
==========================================================================*/

SHORT   ItemTendPartSPVInt( UIFREGTENDER *UifRegTender )
{
    if ( (UifRegTender->fbModifier & VOID_MODIFIER) == 0 || CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT0) == 0) {
        return( ITM_SUCCESS );
    }

    if ( ItemSPVInt(LDT_SUPINTR_ADR) == ITM_SUCCESS ) {                        /* success */
        return( ITM_SUCCESS );
    } else {                                                    /* cancel */
        return( UIF_CANCEL );
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendPartDisp( UIFREGTENDER *UifRegTender,
*                       ITEMTENDER   *ItemTender )   
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display of partial tender Balance Due Operator prompt and
*                   change the state display to reflect tax exempt if needed.
==========================================================================*/
VOID    ItemTendPartDisp( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
	REGDISPMSG      DispMsg = {0};

    DispMsg.uchMajorClass = UifRegTender->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTender->uchMinorClass;

    DispMsg.lAmount = ItemTender->lBalanceDue;                      /* balance due */
    RflGetLead (DispMsg.aszMnemonic, LDT_BD_ADR);

    flDispRegDescrControl |= TENDER_CNTRL;                          /* descriptor */
    flDispRegKeepControl |= TENDER_CNTRL;                           /* descriptor */
    DispMsg.fbSaveControl = 5;                                      /* save for redisplay, DISP_SAVE_TYPE_5 */

	// set the Descriptors to indicate if tax exempt or not.
    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN ) {      /* canada system */
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT        /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {  /* PST exempt */

            flDispRegKeepControl |= TAXEXMPT_CNTRL;                 /* tax exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    } else {
       if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
            flDispRegKeepControl |= TAXEXMPT_CNTRL;                 /* tax exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    }

    flDispRegKeepControl &= ~VOID_CNTRL;                            /* descriptor */
    flDispRegDescrControl &= ~VOID_CNTRL;                           /* descriptor */

    DispWrite( &DispMsg );                                          /* display check total */

    /* send partial tender data to KDS directory R3.1 */
    ItemTender->fbStorageStatus &= ~NOT_SEND_KDS;
    TrnThroughDisp(ItemTender);
    ItemTender->fbStorageStatus |= NOT_SEND_KDS;
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendPartSpl(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender           
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    partial tender for split.
==========================================================================*/

SHORT   ItemTendPartSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT       sStatus;            /* 6/6/96 */
    SHORT       sValPrintFlag = 0;

    /*----- Not Use Total Key -----*/
    /* 5/30/96 if (TranCurQualPtr->auchTotalStatus[0] == 0) {
        return(LDT_SEQERR_ADR);
    } */

    /*----- Validation -----*/
    if ((ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION)   /* validate print required */
        || (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1) && (UifRegTender->fbModifier & VOID_MODIFIER))) {
        ItemTender->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTender);
        sValPrintFlag = 1;	/* 06/22/01 */
    }

    /* --- DTREE#2 Issue#3 Check Endorsement,   Dec/18/2000, 21RFC05402 --- */
    if (!(UifRegTender->fbModifier & VOID_MODIFIER)) {
	    if (TranCurQualPtr->uchSeat != 0) {
		    if (ItemCommonTaxSystem() == ITM_TAX_US) {
				if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) {
					ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTender->lRound, &sValPrintFlag);
				} else {
            		ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);
				}
            } else {
				ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTender->lRound + ItemTenderLocal.lTenderizer, &sValPrintFlag);
			}
        } else {
			ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTender->lRound, &sValPrintFlag);
		}
	}
	
    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

	if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
#if 1
	if ((TranCurQualPtr->uchSeat == 0) && !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
      	ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
    } else {
   		/* execute slip validation for partial ept tender, 09/12/01 */
        if ((ItemTender->fsPrintStatus & PRT_SLIP) && (ItemTender->uchCPLineNo != 0)) {                /* Saratoga */
			USHORT      fsPrintStatus;                  /* 51:print status */

	        if (sValPrintFlag == 1) {
		        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
  					} else {
			            UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
    			    }
		        }
        	}
	    	TrnPrtSetSlipPageLine(0);
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
#endif
    }

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sValPrintFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    /* ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL; */

    /*----- Split Key -> Seat# -> Total Key -> Tender -----*/
    if (TranCurQualPtr->uchSeat != 0) {
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();
		ITEMTENDER  DummyTender = {0};
		UCHAR  auchTotalStatus[NUM_TOTAL_STATUS];

		ItemTendGetTotal(&auchTotalStatus[0], UifRegTender->uchMinorClass);            /* 5/30/96 */
        /* 5/30/96 ItemTendAffectTaxSpl(WorkCur.auchTotalStatus); */
        ItemTendAffectTaxSpl(auchTotalStatus);            /* 5/30/96 */

		if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkCommon->ItemSales, 0);
            
            MldScrollWrite2(MLD_DRIVE_2, &WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
            TrnThroughDisp(&WorkCommon->ItemSales); /* display on KDS */
			/* if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) { */
				/* for check endorsement calculation */
				memcpy(&ItemTenderLocal.ItemTotal, &WorkCommon->ItemSales, sizeof(ItemTenderLocal.ItemTotal));
			/* } */
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
    } else {
		/*----- Split Key -> Tender -----*/
		ITEMTOTAL   ItemTotal = {0};

        /*----- Check Status 6/6/96 -----*/
        if ((sStatus =ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {     
            return(sStatus);
        }
        ItemTendSplitCheckTotal(&ItemTotal, UifRegTender);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);
    }

    ItemTender->fbStorageStatus |= NOT_SEND_KDS;
    ItemPreviousItemSpl(ItemTender, TRN_TYPE_SPLITA);

    ItemTenderLocal.lTenderizer += ItemTender->lTenderAmount;
    ItemTenderLocal.fbTenderStatus[1] |= TENDER_SEAT_PARTIAL;

    ItemTendPartDisp(UifRegTender, ItemTender);
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);
            
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        MldScrollWrite2(MLD_DRIVE_2, ItemTender, MLD_NEW_ITEMIZE);
    } else {
        MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
    }

    if (UifRegTender->fbModifier & VOID_MODIFIER) {
        ItemCommonPutStatus(COMMON_VOID_EC);
    }

	/* release slip to allow next check endorsement */
    if (TranCurQualPtr->uchSeat == 0) {
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
   	
    return(ITM_SUCCESS);
}

/****** End of Source ******/