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
:   Title          : TOTAL KEY MODULE                                 
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application 
:   Program Name   : ITTOTAL4.C (finalize total)                                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemtotalFT()           ;   finalize total
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name    : Description
: 95/3/23  : 03.00.00 :   hkato   : R3.0
: 95/11/10 : 03.01.00 :   hkato   : R3.1
: 98/8/17  : 03.03.00 :  hrkato   : V3.3 (VAT/Service)
: 99/08/13 : 03.05.00 :  M.teraki : Merge STORE_RECALL_MODEL
:          :          :           :       /GUEST_CHECK_MODEL
:
:* NCR2171 **
: 99/09/06 : 01.00.00 :  M.Teraki : initial (for 2171)
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
#include    "pif.h"
#include    "regstrct.h"
#include    "uie.h"
#include    "uireg.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "rfl.h"
#include    "csstbpar.h"
#include    "csttl.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"
#include    "pmg.h"


/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFT(UIFREGTOTAL *UifRegTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    finalize total
==========================================================================*/

SHORT   ItemTotalFT(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};

    if ((sStatus = ItemTotalFTCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal.uchMinorClass = UifRegTotal->uchMinorClass;
    memcpy(ItemTotal.aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal.aszNumber));

	ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex (UifRegTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, UifRegTotal->uchMinorClass);

    /*----- During Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(ItemTotalFTSpl(UifRegTotal, &ItemTotal));

    } else {
        return(ItemTotalFTNor(UifRegTotal, &ItemTotal));
    }

}

/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalFTCheck(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    check modifier, condition
==========================================================================*/

SHORT   ItemTotalFTCheck(VOID)
{
    SHORT           sStatus;

    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {
        return(LDT_SEQERR_ADR);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
            if (ItemModLocalPtr->fsTaxable & ~MOD_USEXEMPT) {
                return(LDT_SEQERR_ADR);
            }
        } else {
            if (ItemModLocalPtr->fsTaxable) {
                return(LDT_SEQERR_ADR);
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
            if (ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT) {
                return(LDT_SEQERR_ADR);
            }
        } else {
            if (ItemModLocalPtr->fsTaxable) {
                return(LDT_SEQERR_ADR);
            }
        }
    } else {
        if (ItemModLocalPtr->fsTaxable) {
            return(LDT_SEQERR_ADR);
        }
    }

    /* --- Check New Key Sequence,  V3.3 FVT --- */
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
        return(LDT_SEQERR_ADR);
    }
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK &&
        !CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT1)) {
        return(LDT_SEQERR_ADR);
    }
    /* --- Check New Key Sequence,  V3.3 FVT --- */

    if (CliParaMDCCheck(MDC_PSN_ADR, EVEN_MDC_BIT1)             /* No person at total key */
        && (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER
        && ItemModLocalPtr->usCurrentPerson == 0) { 
        return(LDT_NOOFPRSN_ADR);
    }

    /*---- guest check close restriction, V3.3 ----*/
    if ( ( sStatus = ItemCommonCheckCloseCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    if ((TranGCFQualPtr->fsGCFStatus & GCFQUAL_WIC) || TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFTNor(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    finalize total
==========================================================================*/

SHORT   ItemTotalFTNor(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
{
    SHORT           sStatus;
	ULONG           fsCurStatus;
    ULONG           flMask;             /* 96/4/2 */
    TRANCURQUAL     WorkCur;            /* 96/4/2 */
    TRANGCFQUAL     *WorkGCF;           /* 96/4/2 */

    /* automatic charge tips, V3.3 */
    ItemTendAutoChargeTips();   /* with display on lcd */

    ItemTotalPrev();

    if ((sStatus = ItemTotalFTAmount(pItem)) != ITM_SUCCESS) {
        return(sStatus);
    }

    ItemTendAffection();                                            /* hourly, close check affection */
    ItemTendMulti();                                                /* print data for multi check payment */
    ItemTotalFTItem(UifTotal, pItem);

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       if (pItem->auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
           ItemDrawer();
           if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
               UieDrawerComp(UIE_ENABLE);
			} else {
				UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
			}
       }
    }

    ItemTotalEuroTender();                      /* Euro Trailer, V3.4 */

    ItemTrailer(TYPE_STORAGE);
    ItemTotalFTClose();
    
    /*----- prepare post receipt 96/4/2 -----*/
    TrnGetCurQual(&WorkCur);
    TrnGetGCFQual( &WorkGCF );                  /* get GCF qualifier */
    flMask = WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK;
    if ( ( flMask == CURQUAL_POSTCHECK ) || ( flMask == CURQUAL_STORE_RECALL ) ) {
        TrnStoGetConsToPostR();
        if ( !( WorkGCF->fsGCFStatus & GCFQUAL_BUFFER_PRINT ) ) {
            ItemMiscSetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );
        }
    }

    if (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_FINALIZE2) {
        pItem->lService = pItem->lMI;
        ItemTotalStub(pItem);
    }
    ItemTendDelGCF();
    ItemTotalFTSaved(pItem);

    TrnGetCurQual(&WorkCur);
    fsCurStatus = WorkCur.fsCurStatus & CURQUAL_WIC;                       /* Saratoga */
    ItemTotalInit();
    ItemWICRelOnFin(fsCurStatus);                                          /* Saratoga */
    return(ItemFinalize());
}    

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTAmount(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL *ItemTotal       
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none
*
*   Return:   
*
*   Description:    calculate tax amount, update transaction information
*                       (MI, taxable itemizer, current qualifier)
==========================================================================*/

SHORT   ItemTotalFTAmount(ITEMTOTAL *ItemTotal )
{
    SHORT           sStatus;
    DCURRENCY       lTax = 0L;
    DCURRENCY       lAmount;
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
    UIFREGTENDER    UifTender;
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};

    /*----- Total Key Discount for MI Calculation -----*/
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
            WorkGCF->fsGCFStatus &= ~(GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT);
            if ( pWorkMod->fsTaxable & MOD_GSTEXEMPT ) {          /* GST exempt state */
                WorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;      /* GST exempt transaction */
            }
            if ( pWorkMod->fsTaxable & MOD_PSTEXEMPT ) {          /* PST exempt state */
                WorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;      /* PST exempt transaction */
            }
            
            pWorkMod->fsTaxable &= ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT );    /* exempt state off */
        }
        ItemCanTax( &ItemTotal->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
		if(WorkTax.alTaxable[1] > 100000)
		{
			// Tim Horton's of Canada was having a problem with a tax calculation error
			// which would suddenly happen and then not be seen for a while.
			// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
			//   Richard Chambers, Apr-09-2015
			NHPOS_ASSERT(!"ItemTotalFTAmount TAX ERROR");
		}
    
        lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
        ItemTotal->lTax += lTax + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[0]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[1] 
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[2]
            + WorkTI->Itemizers.TranCanadaItemizers.lAffTax[3];

    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        if ( (pWorkCur->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            WorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;          /* reset USEXEMPT bit */
            if ( pWorkMod->fsTaxable & MOD_USEXEMPT ) {           /* tax exempt case */
                WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if (pWorkCur->fsCurStatus & CURQUAL_WIC) {            /* Saratoga */
                WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], &WorkTax)) != ITM_SUCCESS) {
                return(sStatus);
            }
            pWorkMod->fsTaxable &= ~MOD_USEXEMPT;                 /* exempt state off */        
        } else {
            if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], &WorkTax)) != ITM_SUCCESS) {
                return( sStatus );
            }
        }
        lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2];
        ItemTotal->lTax += lTax + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[0]
            + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[1] 
            + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[2];
    } else {                                                    /* Int'l VAT */
        if ((sStatus = ItemCurVAT(&ItemTotal->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT)) != ITM_SUCCESS) {
            return(sStatus);
        }
        ItemTotal->lTax += WorkVAT.lPayment;
        lTax = WorkVAT.lPayment;
    }

    if ( ItemTransLocalPtr->uchCheckCounter != 0 ) {            /* multi check payment */
        ItemTotal->lMI += ItemTransLocalPtr->lWorkMI;           /* transaction open local data */                
    }

    ItemTotal->lMI += WorkTI->lMI;                              /* MI */
    ItemTotal->lMI += lTax;

    if ( ( sStatus = ItemTotalSPVInt( ItemTotal ) ) != ITM_SUCCESS ) { 
        return( sStatus );                                      /* error */                
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);  /* R3.1 */

    if ( CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       if ( ItemTotal->auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
           ItemDrawer();                                            /* drawer open */
           if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
               UieDrawerComp( UIE_ENABLE );  
			} else {
				UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
			}
       }
    }

    memset(&UifTender, 0, sizeof(UIFREGTENDER));
    ItemTendAffectTax(&UifTender);
    ItemTendTrayTotal();
    ItemTendTrayTax();
    ItemTendVAT(&UifTender);                                    /* V3.3 */

    if ( pWorkCur->fsCurStatus & CURQUAL_TRAY ) {                 /* tray total transaction */
        ItemTotal->lMI = WorkTI->lMI + ItemTotalLocal.lTrayTax[0]
            + ItemTotalLocal.lTrayTax[1] + ItemTotalLocal.lTrayTax[2] + ItemTotalLocal.lTrayTax[3];
    }

    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
        if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal->lMI = lAmount;
        }
    }
    
    return( ITM_SUCCESS );      
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFTItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate finalize total data
==========================================================================*/

VOID    ItemTotalFTItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    DCURRENCY       lMI;
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};
    SHORT       sFlag = 0;
    
    lMI = ItemTotal->lMI;                                           /* tender amount */

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {
        lMI *= -1L;
    }

    if ( TranCurQualPtr->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION  /* validation print */
        || ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT3 ) && lMI <= 0L ) ) {

        ItemTotal->fsPrintStatus = PRT_VALIDATION;                      /* print validation */ 
        TrnThrough( ItemTotal );
        sFlag = 1;
    }
    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;

    DispMsg.lAmount = ItemTotal->lMI;
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifRegTotal->uchMinorClass) );

    DispMsg.fbSaveControl = DISP_SAVE_TYPE_2;                       /* save for redisplay (not recovery) */
    flDispRegDescrControl |= TOTAL_CNTRL;

    ItemTendDispIn( &DispMsgBack );                                 /* generate background data */

    flDispRegKeepControl &= ~( TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL );
                                                                
    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0 ) ) {       /* receipt control */
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }

    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;   /* set default adjective, 2172 */
        }
    } 

    flDispRegDescrControl &= ~( TVOID_CNTRL | TAXEXMPT_CNTRL );

    if (ItemCommonTaxSystem() == ITM_TAX_US &&                  /* V3.3 */
        ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) &&
        ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) ) {          /* exempt */
    
        flDispRegDescrControl |= TAXEXMPT_CNTRL; 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt */
    }

    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */
    
    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                      /* print journal */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);
            
    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        TrnItemSpl(ItemTotal, TRN_TYPE_SPLITA);
        MldScrollWrite2(MLD_DRIVE_2, ItemTotal, MLD_NEW_ITEMIZE);

    } else {
        /*----- Display Total to LCD,  R3.0 -----*/
        MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(ItemTotal);
        TrnItem(ItemTotal);
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTClose( VOID )   
*
*   Input:      none           
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction close data
==========================================================================*/

VOID    ItemTotalFTClose( VOID )
{
    SHORT   sStatus;
	ITEMTRANSCLOSE  ItemTransClose = {0};
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();

    if ( pWorkCur->flPrintStatus & CURQUAL_POSTCHECK ) {  
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;            /* postcheck, store/recall */
    } else if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;                /* unbuffering */
    } else {
        pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;              /* precheck */
    }

    if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT3)) {        /* Not Print Void Item, R3.1 */
        pWorkCur->uchKitchenStorage = PRT_KIT_NET_STORAGE;
    } else {
        pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;           /* for kitchen print */
    }

    if ( ( CliParaMDCCheck( MDC_PSELVD_ADR, EVEN_MDC_BIT1 ) == 0/* kitchen send at pvoid */
        && ( pWorkCur->fsCurStatus & CURQUAL_PRESELECT_MASK ) )            /* preselect void */
        || ( pWorkCur->fsCurStatus & CURQUAL_TRAINING ) ) {       /* training */
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;              /* for kitchen print */
    }
                                                                
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK 
        && !(!CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2))       /* R3.1 */
        && !(pWorkCur->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING)    // TTL_STAT_FLAG_PRINT_BUFFERING()
        && (pWorkCur->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER
        && (TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) == 0) {        /* R3.3 */
        pWorkCur->fbNoPrint |= CURQUAL_NO_R;
    }

    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL   /* 5/8/96 */
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !(pWorkCur->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING)    // TTL_STAT_FLAG_PRINT_BUFFERING()
        && (pWorkCur->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {
        pWorkCur->fbNoPrint |= CURQUAL_NO_R;
    }

    if ( pWorkCur->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP ) {    /* compulsory slip */
        pWorkCur->fbCompPrint = PRT_SLIP;
    } else {                                                        /* optional slip */
        pWorkCur->fbCompPrint &= ~PRT_SLIP;
    }

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;        /* major class */
    ItemTransClose.uchMinorClass = CLASS_CLSTOTAL1;             /* minor class */
    TrnClose( &ItemTransClose );                                /* transaction module i/F */

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);
    
    while ( ( sStatus = TrnSendTotal() ) !=  TRN_SUCCESS ) {
		USHORT  usStatus = TtlConvertError( sStatus );        
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTSaved( ITEMTOTAL *ItemTotal )   
*
*   Input:      ITEMTOTAL   *ItemTotal         
*   Output:     none    
*   InOut:      none
*
*   Return:     
*
*   Description:    set saved MI
==========================================================================*/

VOID    ItemTotalFTSaved( ITEMTOTAL *pItemTotal )
{ 
    ITEMTENDERLOCAL *pWorkTender = ItemTenderGetLocalPointer();
                                                                
    pWorkTender->lSavedMI = pItemTotal->lMI;                       /* saved MI */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFTSpl(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    Finalize Total for Split Check                R3.1
==========================================================================*/

SHORT   ItemTotalFTSpl(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
{
    SHORT           sFlag = 0;
    SHORT           sStatus;        /* 6/6/96 */

    /* automatic charge tips, V3.3 */
    ItemTendAutoChargeTips();   /* with display on lcd */

    ItemTotalPrevSpl();
    
    if (TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        if (!(TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_USESEAT)
            || TranGCFQualPtr->auchUseSeat[0] == 0) {
           return(LDT_SEQERR_ADR);
        }
    }

    for (;;) {
        /*----- Split Key -> Seat# -> FT -----*/
        if (TranCurQualPtr->uchSeat != 0 && TranCurQualPtr->auchTotalStatus[0] == 0) {
			ITEMCOMMONTAX   WorkTax = {0};
			ITEMCOMMONVAT   WorkVAT = {0};

            ItemTendAffectTaxSpl(pItem->auchTotalStatus);
            ItemTotalSplTax(UifTotal, pItem, &WorkTax, &WorkVAT);
            
            /* --- Netherland rounding, 2172 --- */
            if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
				DCURRENCY       lAmount;

                if ( RflRound( &lAmount, pItem->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                    pItem->lMI = lAmount;
                }
            }
            
            ItemTotalFTItemSpl(UifTotal, pItem);
            /* 5/7/96 MldScrollWrite2(MLD_DRIVE_2, pItem, MLD_NEW_ITEMIZE); */
            MldDispSubTotal(MLD_TOTAL_2, pItem->lMI);
            sFlag = 0;
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(pItem, 0);
            break;
        }
        /*----- Split Key -> Seat# -> Total Key -> Tender -----*/
        if (TranCurQualPtr->uchSeat != 0 && TranCurQualPtr->auchTotalStatus[0] != 0) {
			ITEMCOMMONTAX   WorkTax = {0};
			ITEMCOMMONVAT   WorkVAT = {0};

            ItemTendAffectTaxSpl(pItem->auchTotalStatus);
            ItemTotalSplTax(UifTotal, pItem, &WorkTax, &WorkVAT);
            
            /* --- Netherland rounding, 2172 --- */
            if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
				DCURRENCY       lAmount;

                if ( RflRound( &lAmount, pItem->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                    pItem->lMI = lAmount;
                }
            }
            
            ItemTotalFTItemSpl(UifTotal, pItem);
            /* 5/7/96 MldScrollWrite2(MLD_DRIVE_2, pItem, MLD_NEW_ITEMIZE); */
            MldDispSubTotal(MLD_TOTAL_2, pItem->lMI);
            sFlag = 0;
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(pItem, 0);
            break;
        }
        /*----- Split Key -> Total Key -> Tender -----*/
        /*----- Check Status 6/6/96 -----*/
        if ((sStatus =ItemTotalFTSplitTender(pItem)) != ITM_SUCCESS) {  
            return(sStatus);
        }
        
        /* --- Netherland rounding, 2172 --- */
        if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
			DCURRENCY       lAmount;

            if ( RflRound( &lAmount, pItem->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                pItem->lMI = lAmount;
            }
        }
            
        ItemTotalFTDispSplit(UifTotal, pItem);
        ItemTendAffection();                    /* 6/7/96 hourly, close check affection */
        sFlag = 1;
        break;
    }

    if (sFlag == 0) {
        ItemTendDispSpl();
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       if (pItem->auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
           ItemDrawer();
           if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
               UieDrawerComp(UIE_ENABLE);
			} else {
				UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
			}
       }
    }

    ItemTotalEuroTender();                      /* Euro Trailer, V3.4 */

    if (sFlag == 0) {
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

        ItemTrailer(TYPE_SPLITA);
        ItemTendCloseSpl();
        /*----- Issue Total Stub Receipt 5/30/96 -----*/
        if (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_FINALIZE2) {
            ItemTotalStubSpl(pItem, TranCurQualPtr->uchSeat);
        }

        ItemTenderLocal.lTenderizer = 0L;
        ItemTenderLocal.lFCMI = 0L;
        ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SEAT_PARTIAL;

        /*----- Erase Previous Used Total Key Data -----*/
        TranCurQualTotalStatusClear();
        if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
            memset(&WorkCommon->ItemSales, 0, sizeof(ITEMSALES));
            WorkCommon->uchItemNo--;
        }
        return(UIF_SEAT_TRANS);
    } else {
        ItemTrailer(TYPE_STORAGE);
        ItemTotalFTClose();
        /*----- Issue Total Stub Receipt 5/30/96 -----*/
        if (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE == PRT_FINALIZE2) {
            ItemTotalStubSpl(pItem, 0);
        }
        ItemTendDelGCF();
        ItemTotalFTSaved(pItem);
        TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL
            | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
        return(ItemFinalize());
    }
}    

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFTItemSpl(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
*
*   Input:      
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate finalize total data
==========================================================================*/
VOID    ItemTotalFTItemSpl(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem)
{
    DCURRENCY       lMI = pItem->lMI;
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        lMI *= -1L;
    }

    if ((TranCurQualPtr->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION)
        || (CliParaMDCCheck(MDC_SIGNOUT_ADR, ODD_MDC_BIT3) && lMI <= 0L)) {
    	if ( !CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
	        pItem->fsPrintStatus = PRT_VALIDATION;
    	    TrnThrough(pItem);
    	}
	}

    DispMsg.uchMajorClass = UifTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifTotal->uchMinorClass;
    DispMsg.lAmount = pItem->lMI;
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifTotal->uchMinorClass));

    DispMsg.fbSaveControl = DISP_SAVE_TYPE_2;
    flDispRegDescrControl |= TOTAL_CNTRL;
    ItemTendDispIn(&DispMsgBack);

    flDispRegKeepControl &= ~(TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL);
                                                                
    if (CliParaMDCCheck(MDC_RECEIPT_ADR, ODD_MDC_BIT0)) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
    if (CliParaMDCCheck(MDC_MENU_ADR, ODD_MDC_BIT0) != 0) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

		uchDispCurrMenuPage = pWorkSales->uchMenuShift;
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift; /* set default adjective, 2172 */
        }
    }
    flDispRegDescrControl &= ~(TVOID_CNTRL | TAXEXMPT_CNTRL);

    if (ItemCommonTaxSystem() == ITM_TAX_US  &&                 /* V3.3 */
        ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) &&
        ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) ) {          /* exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL; 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt */
    }

    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */
    
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);

    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    /* send to enhanced kds, 2172 */
    ItemSendKds(pItem, 0);
            
    MldScrollWrite2(MLD_DRIVE_2, pItem, MLD_NEW_ITEMIZE);
    TrnItemSpl(pItem, TRN_TYPE_SPLITA);
    pItem->fbStorageStatus = NOT_CONSOLI_STORAGE;
    TrnItem(pItem);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalFTSplitTender(ITEMTOTAL *pItem)
*
*   Input:      
*   Output:     
*   InOut:      none
*
*   Return:     
*
*   Description:    Split Print Tender
==========================================================================*/
SHORT    ItemTotalFTSplitTender(ITEMTOTAL *pItem)
{
    SHORT               i, sFlag = 0;
    SHORT               sFirstSeatFlag = 0;     /* 6/3/96 */ 
    SHORT               sSeatFlag = 0;          /* 6/7/96 */ 
    DCURRENCY           lTax = 0L;
	ITEMTOTAL           ItemTotal = {0};
    ITEMAFFECTION       Tax = {0}, IndTax = {0};
    TRANGCFQUAL         *WorkGCF = TrnGetGCFQualPtr();
    TRANINFORMATION     *WorkTran = TrnGetTranInformationPointer();
    ITEMCOMMONLOCAL     *pItemCommonL = ItemCommonGetLocalPointer();

    /*----- Check Seat Transaction Exist or Not 6/7/96 -----*/
    for (i = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++, sFlag++) { //SR206
        /*----- Check Finalize Seat# -----*/
        if (TrnSplSearchFinSeatQueue(WorkGCF->auchUseSeat[i]) == TRN_SUCCESS) {
            continue;
        }
        sSeatFlag = 1;
    }
    if (sSeatFlag == 0) {              /* 6/7/96 Not Seat Transaction */
        return(LDT_SEQERR_ADR);
    }

    /*----- Return to LCD 1 Display -----*/
	{
		MLDTRANSDATA        WorkMLD = {0};

		MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);
		WorkMLD.usVliSize = WorkTran->usTranConsStoVli;
		WorkMLD.sFileHandle = WorkTran->hsTranConsStorage;

		/*----- Display Process 4/19/96 -----*/
		WorkMLD.sIndexHandle = WorkTran->hsTranConsIndex;
		MldPutTransToScroll(&WorkMLD);
	}

    /*----- Return to LCD 1 Display -----*/
    TrnSplCancel3DispSplit();

    /* redisplay auto charge tips, V3.3 */
    if (( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP ) ||
        ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP2 ) ||
        ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP3 )) {

        pItemCommonL->ItemDisc.uchMajorClass = CLASS_ITEMDISC;
        pItemCommonL->ItemDisc.fsPrintStatus &= ~PRT_NO_UNBUFFING_PRINT;

        /* send to enhanced kds, 2172 */
        ItemSendKds(&(pItemCommonL->ItemDisc), 0);
            
        MldScrollWrite(&(pItemCommonL->ItemDisc), MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&(pItemCommonL->ItemDisc));
    }

    for (i = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++, sFlag++) { //SR206
        /*----- Check Finalize Seat# -----*/
        if (TrnSplSearchFinSeatQueue(WorkGCF->auchUseSeat[i]) == TRN_SUCCESS) {
            continue;
        }

        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

        /*----- Make Split Transaction -----*/
        TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA);

        /*----- Make Tax, Total Data -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            ItemTotalFTAffectVATSplit(pItem, &Tax);
        } else {
            ItemTotalFTAffectTaxSplit(pItem, &Tax);
        }
        ItemTotalFTSplitTotal(pItem, &ItemTotal);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);            /* Individual Total */

        /*----- Check First Seat Print or not 6/3/96 -----*/
        if (sFirstSeatFlag == 0) {              /* First Seat for Print */
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, 0, ItemTotal.auchTotalStatus, TRN_PRT_FULL);
            sFirstSeatFlag = 1;
        } else {
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, i, ItemTotal.auchTotalStatus, TRN_PRT_FULL);
        }

        /*----- Make Whole Total -----*/
        pItem->lMI += ItemTotal.lMI;
    }

    /*----- Get Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_SUCCESS) {
		TRANITEMIZERS       *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
		ITEMCOMMONTAX       WorkTax = {0};
		ITEMCOMMONVAT       WorkVAT = {0};

        /*----- Make Subtotal,   R3.1 5/8/96 -----*/
        if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                IndTax.lAmount = WorkTI->lMI; 
            }
        }

        /*----- Individual Tax of Base Transaction -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(&pItem->auchTotalStatus[1], &WorkTax);
        } else {
            ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
        }    

        IndTax.uchMajorClass = CLASS_ITEMAFFECTION;
        IndTax.uchMinorClass = CLASS_TAXPRINT;
        IndTax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            for (i = 0; i < 5; i++) {
                IndTax.USCanVAT.USCanTax.lTaxable[i] += WorkTax.alTaxable[i];
                Tax.USCanVAT.USCanTax.lTaxable[i] += IndTax.USCanVAT.USCanTax.lTaxable[i];
            }
            for (i = 0, lTax = 0L; i < 4; i++) {
                IndTax.USCanVAT.USCanTax.lTaxAmount[i] += WorkTax.alTax[i];
                IndTax.USCanVAT.USCanTax.lTaxExempt[i] += WorkTax.alTaxExempt[i];
                Tax.USCanVAT.USCanTax.lTaxAmount[i] += IndTax.USCanVAT.USCanTax.lTaxAmount[i];
                Tax.USCanVAT.USCanTax.lTaxExempt[i] += IndTax.USCanVAT.USCanTax.lTaxExempt[i];
                lTax += WorkTax.alTax[i];
            }

            /*----- adjust tax print format 4/26/96 -----*/
            ItemCanTaxCopy2(&IndTax);

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            for (i = 0, lTax = 0L; i < 3; i++) {
                IndTax.USCanVAT.USCanTax.lTaxable[i] += WorkTax.alTaxable[i];
                IndTax.USCanVAT.USCanTax.lTaxAmount[i] += WorkTax.alTax[i];
                IndTax.USCanVAT.USCanTax.lTaxExempt[i] += WorkTax.alTaxExempt[i];
                Tax.USCanVAT.USCanTax.lTaxable[i] += IndTax.USCanVAT.USCanTax.lTaxable[i];
                Tax.USCanVAT.USCanTax.lTaxAmount[i] += IndTax.USCanVAT.USCanTax.lTaxAmount[i];
                Tax.USCanVAT.USCanTax.lTaxExempt[i] += IndTax.USCanVAT.USCanTax.lTaxExempt[i];
                lTax += WorkTax.alTax[i];
            }

        } else {

            for (i=0; i<NUM_VAT; i++) {
                if (WorkVAT.ItemVAT[i].lVATRate) {
                    IndTax.USCanVAT.ItemVAT[i].lVATRate  = WorkVAT.ItemVAT[i].lVATRate;
                }
                IndTax.USCanVAT.ItemVAT[i].lVATable += WorkVAT.ItemVAT[i].lVATable;
                IndTax.USCanVAT.ItemVAT[i].lVATAmt  += WorkVAT.ItemVAT[i].lVATAmt;
                IndTax.USCanVAT.ItemVAT[i].lSum     += WorkVAT.ItemVAT[i].lSum;
                IndTax.USCanVAT.ItemVAT[i].lAppAmt  += WorkVAT.ItemVAT[i].lAppAmt;
                if (IndTax.USCanVAT.ItemVAT[i].lVATRate) {
                    Tax.USCanVAT.ItemVAT[i].lVATRate  = IndTax.USCanVAT.ItemVAT[i].lVATRate;
                }
                Tax.USCanVAT.ItemVAT[i].lVATable += IndTax.USCanVAT.ItemVAT[i].lVATable;
                Tax.USCanVAT.ItemVAT[i].lVATAmt  += IndTax.USCanVAT.ItemVAT[i].lVATAmt;
                Tax.USCanVAT.ItemVAT[i].lSum     += IndTax.USCanVAT.ItemVAT[i].lSum;
                Tax.USCanVAT.ItemVAT[i].lAppAmt  += IndTax.USCanVAT.ItemVAT[i].lAppAmt;
            }
            IndTax.lAmount += WorkVAT.lService;
            Tax.lAmount += IndTax.lAmount;
            lTax += WorkVAT.lPayment;
        }

        /*----- Add Base Ind. Tax to Hourly in TransInf 6/7/96 -----*/
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS       *WorkTIAll = TrnGetTIPtr();         /* 6/7/96 */

            WorkTIAll->lHourly += lTax;
        }

        TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);

        /*----- Individual Total of Base Transaction -----*/
        ItemTotal.lMI = WorkTI->lMI + lTax;
        ItemTotal.lTax = lTax;
        ItemTotal.lService = WorkTI->lMI + lTax;
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

        /*----- Buffering Print 4/19/96-----*/
        TrnSplPrintSeat(0, TRN_SPL_NOT_UPDATE, 1, 1, ItemTotal.auchTotalStatus, TRN_PRT_FULL); 

        /*----- Whole Total of Base Transaction -----*/
        pItem->lMI += WorkTI->lMI + lTax;
        pItem->lTax += lTax;

        sFlag++;
    }

    /*----- Re-Create Consoli Storage -----*/
    TrnInitialize(TRANI_CONSOLI);
    ItemPrintStart(TYPE_SPLIT_TENDER);

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        ItemTendVATCommon(&Tax, ITM_NOT_SPLIT, ITM_DISPLAY);

    } else {
        /*----- Whole Tax Data -----*/
        Tax.uchMajorClass = CLASS_ITEMAFFECTION;
        Tax.uchMinorClass = CLASS_TAXAFFECT;
        Tax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        Tax.fbStorageStatus = NOT_CONSOLI_STORAGE;
        TrnItem(&Tax); 
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&Tax, 0);
        
        Tax.uchMinorClass = CLASS_TAXPRINT;
        Tax.fbStorageStatus = NOT_ITEM_STORAGE;

        /*----- adjust tax print format 4/26/96 -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTaxCopy2(&Tax);
        }   

        /*----- Make Subtotal,   R3.1 4/26/96 -----*/
        for (i = 0, lTax = 0L; i < 4; i++) {
            lTax += Tax.USCanVAT.USCanTax.lTaxAmount[i];
        }
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            Tax.lAmount = pItem->lMI - lTax; 
        }

        TrnItem(&Tax);
        MldScrollWrite(&Tax, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&Tax);
    }

    /*----- During Split Tender -----*/
	{
		ITEMTENDERLOCAL   *pTenderLocal = ItemTenderGetLocalPointer();

		pTenderLocal->fbTenderStatus[0] |= TENDER_PARTIAL;
		pTenderLocal->fbTenderStatus[1] |= TENDER_SPLIT_TENDER;
	}
    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTAffectTaxSplit(ITEMTOTAL *pItem, ITEMAFFECTION *pData)
*
*   Input:      none          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate tax data for split file.
==========================================================================*/
VOID    ItemTotalFTAffectTaxSplit(ITEMTOTAL *pItem, ITEMAFFECTION *pTax)
{
    if (!(ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER)) {
		SHORT           i;
		ITEMAFFECTION   ItemAffection = {0};
		ITEMCOMMONTAX   WorkTax = {0};

        /*----- generate affection tax -----*/
        ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;
        ItemAffection.uchMinorClass = CLASS_TAXPRINT;
        ItemAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        /*----- Make Subtotal,   R3.1 5/8/96 -----*/
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
			TRANITEMIZERS   *WorkTI;
			TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
            ItemAffection.lAmount = WorkTI->lMI; 
        }

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&pItem->auchTotalStatus[1], &WorkTax, ITM_SPLIT);
            ItemCanTaxCopy1(&ItemAffection, &WorkTax);

            /*----- adjust tax print format 4/26/96 -----*/
            ItemCanTaxCopy2(&ItemAffection);

            if (pItem->auchTotalStatus[1] != 0) {    // is there any Canadian tax that needs to be affected?
                ItemAffection.fbStorageStatus = NOT_ITEM_STORAGE;
                TrnItemSpl(&ItemAffection, TRN_TYPE_SPLITA);
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(&pItem->auchTotalStatus[1], &WorkTax);
            ItemUSTaxCopy1(&ItemAffection, &WorkTax);
            
			//       ITM_TTL_FLAG_TAX_1             ITM_TTL_FLAG_TAX_2              ITM_TTL_FLAG_TAX_3
			//       CURQUAL_TOTAL_FLAG_TAX_1       CURQUAL_TOTAL_FLAG_TAX_2        CURQUAL_TOTAL_FLAG_TAX_3
			if (pItem->auchTotalStatus[1] & 0x01
                || pItem->auchTotalStatus[1] & 0x10
                || pItem->auchTotalStatus[2] & 0x01) {
                ItemAffection.fbStorageStatus = NOT_ITEM_STORAGE;
                TrnItemSpl(&ItemAffection, TRN_TYPE_SPLITA);
            }
        }

        /*----- Add Tax to Hourly in TransInf -----*/
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
			DCURRENCY       lTax;

			lTax = ItemAffection.USCanVAT.USCanTax.lTaxAmount[0]
                + ItemAffection.USCanVAT.USCanTax.lTaxAmount[1]
                + ItemAffection.USCanVAT.USCanTax.lTaxAmount[2]
                + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3];
            WorkTI->lHourly += lTax;
        }

        for(i = 0; i < 5; i++) {
            pTax->USCanVAT.USCanTax.lTaxable[i] += ItemAffection.USCanVAT.USCanTax.lTaxable[i];
        }
        for(i = 0; i < 4; i++) {
            pTax->USCanVAT.USCanTax.lTaxAmount[i] += ItemAffection.USCanVAT.USCanTax.lTaxAmount[i];
            pTax->USCanVAT.USCanTax.lTaxExempt[i] += ItemAffection.USCanVAT.USCanTax.lTaxExempt[i];
        }
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTAffectVATSplit(ITEMTOTAL *pItem, ITEMAFFECTION *pData)
*
*   Input:      none          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate VAT data for split file. V3.3
==========================================================================*/

VOID    ItemTotalFTAffectVATSplit(ITEMTOTAL *pItem, ITEMAFFECTION *pTax)
{
    if (!(ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER)) {
		ITEMAFFECTION   ItemAffection = {0};
		ITEMCOMMONVAT   WorkVAT = {0};
		SHORT           i;

        ItemCurVAT(&pItem->auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
        
        memcpy(&ItemAffection.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
        ItemAffection.lAmount = WorkVAT.lService;

        ItemTendVATCommon(&ItemAffection, ITM_SPLIT, ITM_NOT_DISPLAY);

        /*----- Add Tax to Hourly in TransInf -----*/
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

			WorkTI->lHourly += WorkVAT.lPayment;
        }

        for (i = 0; i < NUM_VAT; i++) {
            if (WorkVAT.ItemVAT[i].lVATRate) {
                pTax->USCanVAT.ItemVAT[i].lVATRate  = WorkVAT.ItemVAT[i].lVATRate;
            }
            pTax->USCanVAT.ItemVAT[i].lVATable += WorkVAT.ItemVAT[i].lVATable;
            pTax->USCanVAT.ItemVAT[i].lVATAmt  += WorkVAT.ItemVAT[i].lVATAmt;
            pTax->USCanVAT.ItemVAT[i].lSum     += WorkVAT.ItemVAT[i].lSum;
            pTax->USCanVAT.ItemVAT[i].lAppAmt  += WorkVAT.ItemVAT[i].lAppAmt;
        }
        pTax->lAmount += WorkVAT.lService;
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTSplitTotal(ITEMTOTAL *pWholeTotal,
*                           ITEMTOTAL *pIndTotal)
*
*   Input:      none       
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate finalize total data
==========================================================================*/
VOID    ItemTotalFTSplitTotal(ITEMTOTAL *pWholeTotal, ITEMTOTAL *pIndTotal)
{
	UIFREGTOTAL     UifRegTotal = {0};                        
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};

    UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;
    UifRegTotal.uchMinorClass = pWholeTotal->uchMinorClass;

    memset(pIndTotal, 0, sizeof(ITEMTOTAL));
    pIndTotal->uchMajorClass = CLASS_ITEMTOTAL;
    pIndTotal->uchMinorClass = pWholeTotal->uchMinorClass;
    memcpy(pIndTotal->auchTotalStatus, pWholeTotal->auchTotalStatus, sizeof(pIndTotal->auchTotalStatus));
    pIndTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    ItemTotalSplTax(&UifRegTotal, pIndTotal, &WorkTax, &WorkVAT);
}        

/*==========================================================================
**  Synopsis:   VOID    ItemTotalFTDispSplit(UIFREGTOTAL *UifTotal,
*                           ITEMTOTAL *pTotal)
*
*   Input:      
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate finalize total data for display
==========================================================================*/
VOID    ItemTotalFTDispSplit(UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal)
{
    DCURRENCY       lMI = pTotal->lMI;
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        lMI *= -1L;
    }

    /*----- Validation -----*/
    if (ITM_TTL_FLAG_PRINT_VALIDATION(pTotal) || (CliParaMDCCheck(MDC_SIGNOUT_ADR, ODD_MDC_BIT3) && lMI <= 0L)) {
    	if ( !CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
	        pTotal->fsPrintStatus = PRT_VALIDATION;
    	    TrnThrough(pTotal);
		}
	}
                                                    
    DispMsg.uchMajorClass = UifTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifTotal->uchMinorClass;
    DispMsg.lAmount = pTotal->lMI;
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifTotal->uchMinorClass) );
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_2;
    flDispRegDescrControl |= TOTAL_CNTRL;
    ItemTendDispIn(&DispMsgBack);
    flDispRegKeepControl &= ~(TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL);
                                                                
    /*----- Receipt Control -----*/
    if (CliParaMDCCheck(MDC_RECEIPT_ADR, ODD_MDC_BIT0)) {
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }
    if (CliParaMDCCheck(MDC_MENU_ADR, ODD_MDC_BIT0) != 0) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

        uchDispCurrMenuPage = pWorkSales->uchMenuShift;
        if (pWorkSales->uchAdjKeyShift == 0) {                         /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;   /* set default adjective, 2172 */
        }
    }
    flDispRegDescrControl &= ~(TVOID_CNTRL | TAXEXMPT_CNTRL);

    if ( !(TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN ) &&
        ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) &&
        ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) ) {          /* exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL; 
        flDispRegKeepControl |= TAXEXMPT_CNTRL;                     /* tax exempt */
    }
    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */

    /* send to enhanced kds, 2172 */
    ItemSendKds(pTotal, 0);
        
    /*----- Whole Total Data -----*/
    pTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    MldScrollWrite(pTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pTotal);
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);
    TrnItem(pTotal);
}

/****** End of Source ******/
