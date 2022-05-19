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
:   Program Name   : ITTENDS1.C (Split Print Tender)
:  ---------------------------------------------------------------------  
:  Abstract:
:    
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:    Date  : Ver.Rev. :   Name    	: Description
: Nov/22/95: 03.01.00 :   hkato     : R3.1
: Aug/13/99: 03.05.00 :   K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: Feb/16/15: 02.02.01 : R.Chambers  : DCURRENCY, scope reduction, init variables at definition
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
#include    "csttl.h"
#include    "rfl.h"
#include    "itmlocal.h"
#include    "pmg.h"



/*==========================================================================
**  Synopsis:   SHORT   ItemTendCheckSplitTender(VOID)
*
*   Input:      
*   Output:     
*   InOut:      none
*   Return:     
*
*   Description:    Check Split Print Tender
==========================================================================*/

SHORT   ItemTendCheckSplitTender(VOID)
{
    SHORT       i;
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANGCFQUAL     *WorkGCFSpl = TrnGetGCFQualSplPointer(TRN_TYPE_SPLITA);

    if (TrnSplCheckSplit() != TRN_SPL_SPLIT) {
        return(ITM_SUCCESS);
    }

    /*----- Prohibit Partial Tender -----*/
    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {
        return(LDT_PROHBT_ADR);
    }
    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
        return(LDT_PROHBT_ADR);
    }

    /*----- Prohibit (Exist Multi Seat# Transaction) -----*/
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (WorkGCFSpl->auchFinSeat[i] != 0) {
            return(LDT_PROHBT_ADR);
        }
    }

    /*----- No Seat# -----*/
    if (!(WorkGCF->fsGCFStatus2 & GCFQUAL_USESEAT) || WorkGCF->auchUseSeat[0] == 0) {
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);
}



/*==========================================================================
**  Synopsis:   SHORT    ItemTendSplitTender(UIFREGTENDER *UifTender)
*
*   Input:      
*   Output:     
*   InOut:      none
*
*   Return:     
*
*   Description:    Split Print Tender
==========================================================================*/
SHORT    ItemTendSplitTender(UIFREGTENDER *UifTender)
{
    SHORT               i;
    SHORT               sFirstSeatFlag = 0;     /* 5/18/96 */ 
    SHORT               sSeatFlag = 0;          /* 6/7/96 */ 
    DCURRENCY           lTax = 0;
    ITEMTOTAL           ItemTotal = {0}, Total = {0};
    ITEMAFFECTION       Tax = {0}, IndTax = {0};
    TRANGCFQUAL         *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL         WorkCur;
    ITEMTOTALLOCAL      TotalLocalData;         /* 5/29/96 */
    USCANTAX            *pTax = &Tax.USCanVAT.USCanTax;
    USCANTAX            *pIndTax = &IndTax.USCanVAT.USCanTax;
    ITEMCOMMONLOCAL     *pItemCommonL = ItemCommonGetLocalPointer();

    /*----- Check Seat Transaction Exist or Not 6/7/96 -----*/
    for (i = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++) { //SR206
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
		WorkMLD.usVliSize = TrnInformationPtr->usTranConsStoVli;
		WorkMLD.sFileHandle = TrnInformationPtr->hsTranConsStorage;

		/*----- Display Process 4/19/96 -----*/
		WorkMLD.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
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

        MldScrollWrite(&(pItemCommonL->ItemDisc), MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&(pItemCommonL->ItemDisc));
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&pItemCommonL->ItemDisc, 0);
    }

    TrnGetCurQual(&WorkCur);

    for (i = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++) { //SR206
        /*----- Check Finalize Seat# -----*/
        if (TrnSplSearchFinSeatQueue(WorkGCF->auchUseSeat[i]) == TRN_SUCCESS) {
            continue;
        }

        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

        /*----- Make Split Transaction -----*/
        TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA);

        /*----- Make Tax, Total Data -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            ItemTendAffectVATSplit(WorkCur.auchTotalStatus, &Tax, UifTender, &lTax, 0);    /* V3.3 */
        } else {
            ItemTendAffectTaxSplit(WorkCur.auchTotalStatus, &Tax, UifTender, 0); /* Tax */
        }
        ItemTendSplitCheckTotal(&ItemTotal, UifTender);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);            /* Individual Total */

        /*----- Check First Seat Print or not 5/18/96 -----*/
        if (sFirstSeatFlag == 0) {              /* First Seat for Print */
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, 0, ItemTotal.auchTotalStatus, TRN_PRT_FULL);
            sFirstSeatFlag = 1;
        } else {
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, i, ItemTotal.auchTotalStatus, TRN_PRT_FULL);
        }

        /*----- Make Whole Total -----*/
        Total.lMI += ItemTotal.lMI;
    }

    /*----- Get Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_SUCCESS) {
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
			TRANITEMIZERS       *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);

            ItemTendAffectVATSplit(WorkCur.auchTotalStatus, &Tax, UifTender, &lTax, 0);   /* V3.3 */

            /*----- Individual Total of Base Transaction -----*/
            ItemTotal.lMI = WorkTI->lMI + lTax;
            ItemTotal.lTax = lTax;
            ItemTotal.lService = WorkTI->lMI + lTax;
            TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(0, TRN_SPL_NOT_UPDATE, 1, 1, ItemTotal.auchTotalStatus, TRN_PRT_FULL); 

            /*----- Whole Total of Base Transaction -----*/
            Total.lMI += WorkTI->lMI + lTax;
            Total.lTax += lTax;
        } else {
			TRANITEMIZERS       *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
			ITEMCOMMONTAX       WorkTax = {0};

            /*----- Make Subtotal,   R3.1 5/8/96 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                IndTax.lAmount = WorkTI->lMI; 
            }

            /*----- Individual Tax of Base Transaction -----*/
            if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
                ItemCanTax(&WorkCur.auchTotalStatus[1], &WorkTax, ITM_SPLIT);
            } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
                ItemCurTaxSpl(&WorkCur.auchTotalStatus[1], &WorkTax);
            }
            IndTax.uchMajorClass = CLASS_ITEMAFFECTION;
            IndTax.uchMinorClass = CLASS_TAXPRINT;
            IndTax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

            if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
                for (i = 0; i < 5; i++) {
                    pIndTax->lTaxable[i] += WorkTax.alTaxable[i];
                    pTax->lTaxable[i] += pIndTax->lTaxable[i];
                }
                for (i = 0, lTax = 0L; i < 4; i++) {
                    pIndTax->lTaxAmount[i] += WorkTax.alTax[i];
                    pIndTax->lTaxExempt[i] += WorkTax.alTaxExempt[i];
                    pTax->lTaxAmount[i] += pIndTax->lTaxAmount[i];
                    pTax->lTaxExempt[i] += pIndTax->lTaxExempt[i];
                    lTax += WorkTax.alTax[i];
                }

                /*----- adjust tax print format 4/22/96 -----*/
                if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                    pIndTax->lTaxAmount[1] += pIndTax->lTaxAmount[2] + pIndTax->lTaxAmount[3];
                    pIndTax->lTaxAmount[2] = pIndTax->lTaxAmount[3] = 0L; 
                } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                    && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
                    pIndTax->lTaxAmount[0] += pIndTax->lTaxAmount[1]
                        + pIndTax->lTaxAmount[2] + pIndTax->lTaxAmount[3];
                    pIndTax->lTaxAmount[1] = pIndTax->lTaxAmount[2] = pIndTax->lTaxAmount[3] = 0L;
                }
            } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
                for (i = 0, lTax = 0L; i < 3; i++) {
                    pIndTax->lTaxable[i] += WorkTax.alTaxable[i];
                    pIndTax->lTaxAmount[i] += WorkTax.alTax[i];
                    pIndTax->lTaxExempt[i] += WorkTax.alTaxExempt[i];
                    pTax->lTaxable[i] += pIndTax->lTaxable[i];
                    pTax->lTaxAmount[i] += pIndTax->lTaxAmount[i];
                    pTax->lTaxExempt[i] += pIndTax->lTaxExempt[i];
                    lTax += WorkTax.alTax[i];
                }
            }

            /*----- Add Base Ind. Tax to Hourly in TransInf 6/7/96 -----*/
            if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
				TRANITEMIZERS       *WorkTIAll = TrnGetTIPtr();             /* 6/7/96 */

                WorkTIAll->lHourly += lTax;
            }

            TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);

            /*----- Individual Total of Base Transaction -----*/
            ItemTotal.lMI = WorkTI->lMI + lTax;
            ItemTotal.lTax = lTax;
            ItemTotal.lService = WorkTI->lMI + lTax;
            TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(0, TRN_SPL_NOT_UPDATE, 1, 1, ItemTotal.auchTotalStatus, TRN_PRT_FULL); 
            /*----- Whole Total of Base Transaction -----*/
            Total.lMI += WorkTI->lMI + lTax;
            Total.lTax += lTax;
        }
    }

    /*----- Re-Create Consoli Storage -----*/
    TrnInitialize(TRANI_CONSOLI);
    ItemPrintStart(TYPE_SPLIT_TENDER);

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        ItemTendVATCommon(&Tax, ITM_NOT_SPLIT, ITM_DISPLAY);
    } else {
        /*----- Whole Tax Data -----*/
        Tax.uchMajorClass = CLASS_ITEMAFFECTION;
        Tax.uchMinorClass = CLASS_TAXAFFECT;        /* 4/22/96 */
        Tax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnItem(&Tax);
        
        /* send to enhanced kds, 2172 */
        ItemSendKds(&Tax, 0);

        Tax.uchMinorClass = CLASS_TAXPRINT;

        /*----- adjust tax print format 4/22/96 -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                pTax->lTaxAmount[1] += pTax->lTaxAmount[2] + pTax->lTaxAmount[3];
                pTax->lTaxAmount[2] = pTax->lTaxAmount[3] = 0L;

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
                pTax->lTaxAmount[0] += pTax->lTaxAmount[1]
                    + pTax->lTaxAmount[2] + pTax->lTaxAmount[3]; 
                pTax->lTaxAmount[1] = pTax->lTaxAmount[2] = pTax->lTaxAmount[3] = 0L;
            }
        }

        /*----- Make Subtotal,   R3.1 4/26/96 -----*/
        for (i = 0, lTax = 0L; i < 4; i++) {
            lTax += pTax->lTaxAmount[i];
        }
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            Tax.lAmount = Total.lMI - lTax; 
        }

        TrnItem(&Tax);
        MldScrollWrite(&Tax, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&Tax);
    }

    /*----- Whole Total Data -----*/
    ItemTotalGetLocal(&TotalLocalData);             /* 5/29/96 */                                             
    memcpy( &Total.aszNumber[0], &TotalLocalData.aszNumber[0], sizeof(Total.aszNumber) );

    Total.uchMajorClass = CLASS_ITEMTOTAL;
    Total.uchMinorClass = TotalLocalData.uchSaveMinor;
/*  Total.uchMinorClass = CLASS_TOTAL2; */
    Total.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    memcpy(Total.auchTotalStatus, WorkCur.auchTotalStatus, sizeof(Total.auchTotalStatus));
    TrnItem(&Total);
    MldScrollWrite(&Total, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(&Total);
    MldDispSubTotal(MLD_TOTAL_1, Total.lMI);
        
    /* send to enhanced kds, 2172 */
    ItemSendKds(&Total, 0);

    /*----- During Split Tender -----*/
    ItemTenderLocal.fbTenderStatus[0] |= TENDER_PARTIAL;
    ItemTenderLocal.fbTenderStatus[1] |= TENDER_SPLIT_TENDER;
    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendAffectTaxSplit(UCHAR *puchTotal,
*                               ITEMAFFECTION *pData, UIFREGTENDER *UifTender)
*
*   Input:      none          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate tax data for split file.
==========================================================================*/

VOID    ItemTendAffectTaxSplit(UCHAR *puchTotal,
                ITEMAFFECTION *pTax, UIFREGTENDER *UifTender, SHORT sType)
{
    if (!(ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER)) {
		SHORT           i;
		DCURRENCY       lTax;
		DCURRENCY       lSaveTaxAmount[4];          /* 4/22/96 */
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMAFFECTION   ItemAffection = {0};

        ItemTendGetTotal(puchTotal, UifTender->uchMinorClass);

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
            ItemCanTax(puchTotal + 1, &WorkTax, ITM_SPLIT);
            for(i = 0; i < 5; i++) {
                ItemAffection.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
            }
            for(i = 0; i < 4; i++) {
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
                lSaveTaxAmount[i] = WorkTax.alTax[i];               /* 4/22/96 */
                ItemAffection.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
            }
            /*----- adjust tax print format 4/22/96 -----*/
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[1] += ItemAffection.USCanVAT.USCanTax.lTaxAmount[2]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3];
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[2] = ItemAffection.USCanVAT.USCanTax.lTaxAmount[3] = 0L;

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[0] += ItemAffection.USCanVAT.USCanTax.lTaxAmount[1]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[2] + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3]; 
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[1] = ItemAffection.USCanVAT.USCanTax.lTaxAmount[2] 
                    = ItemAffection.USCanVAT.USCanTax.lTaxAmount[3] = 0L; 
            }
            if (*(puchTotal + 1) != 0) {
                ItemAffection.fbStorageStatus = NOT_ITEM_STORAGE;
                TrnItemSpl(&ItemAffection, TRN_TYPE_SPLITA);
            }
            /*----- restore TaxAmount for Canada -----*/
            for(i = 0; i < 4; i++) {
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[i] = lSaveTaxAmount[i];
            }

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(puchTotal + 1, &WorkTax);
            for(i = 0; i < 3; i++) {
                ItemAffection.USCanVAT.USCanTax.lTaxable[i] = WorkTax.alTaxable[i];
                ItemAffection.USCanVAT.USCanTax.lTaxAmount[i] = WorkTax.alTax[i];
                ItemAffection.USCanVAT.USCanTax.lTaxExempt[i] = WorkTax.alTaxExempt[i];
            }
			// Affection if ITM_TTL_FLAG_TAX_1() or ITM_TTL_FLAG_TAX_2() or ITM_TTL_FLAG_TAX_3().
            if (*(puchTotal + 1) & 0x01 
                || *(puchTotal + 1) & 0x10 || *(puchTotal + 2) & 0x01) {
                ItemAffection.fbStorageStatus = NOT_ITEM_STORAGE;
                TrnItemSpl(&ItemAffection, TRN_TYPE_SPLITA);
            }
        }

        /*----- Add Tax to Hourly in TransInf -----*/
        if (sType != 1) {   /* avoid duplicate receipt, 09/30/98 */
            if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
				TRANITEMIZERS   *WorkTI;

				TrnGetTI(&WorkTI);
                lTax = ItemAffection.USCanVAT.USCanTax.lTaxAmount[0] + ItemAffection.USCanVAT.USCanTax.lTaxAmount[1]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[2] + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3];
                WorkTI->lHourly += lTax;
            }
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
**  Synopsis:   VOID    ItemTendAffectVATSplit(UCHAR *puchTotal,
*                               ITEMAFFECTION *pData, UIFREGTENDER *UifTender)
*
*   Input:      none          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate VAT data for split file. V3.3
==========================================================================*/

VOID    ItemTendAffectVATSplit(UCHAR *puchTotal,ITEMAFFECTION *pTax, UIFREGTENDER *UifTender, DCURRENCY *plPayment, SHORT sType)
{
    if (!(ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER)) {
		SHORT           i;
		ITEMAFFECTION   ItemAffection = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        ItemTendGetTotal(puchTotal, UifTender->uchMinorClass);
        ItemCurVAT(puchTotal+1, &WorkVAT, ITM_SPLIT);
        
        memcpy(&ItemAffection.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
        ItemAffection.lAmount = WorkVAT.lService;

        ItemTendVATCommon(&ItemAffection, ITM_SPLIT, ITM_NOT_DISPLAY);

        /*----- Add Tax to Hourly in TransInf -----*/
        if (sType != 1) {   /* avoid duplicate receipt, 09/30/98 */
            if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
				TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

				WorkTI->lHourly += WorkVAT.lPayment;
            }
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
        *plPayment = WorkVAT.lPayment;
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendSplitCheckTotal(ITEMTOTAL *ItemTotal,
*                                                   UIFREGTENDER *UifTender)
*
*   Input:      none       
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data
==========================================================================*/

VOID    ItemTendSplitCheckTotal(ITEMTOTAL *ItemTotal, UIFREGTENDER *UifTender)
{
	UIFREGTOTAL     UifRegTotal = {0};                        
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};

    memset(ItemTotal, 0, sizeof(ITEMTOTAL));

    UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;
    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    if (TranCurQualPtr->auchTotalStatus[0] == 0) {
		// total key has not been pressed so lets see if there is a total key default
		// provisioned for this tender key.
		PARATEND        ParaTend;

		ParaTend.uchMajorClass = CLASS_PARATEND;
		ParaTend.uchAddress = UifTender->uchMinorClass;
		CliParaRead(&ParaTend);

        UifRegTotal.uchMinorClass = ParaTend.uchTend;
		//We now allow Tenders that arent assigned a total
		//to have the minor class assigned to 0, so that we
		//dont follow Total 2's control code regarding Printing
/*        if (ParaTend.uchTend == 0) {
*            UifRegTotal.uchMinorClass = CLASS_UITOTAL2;
*        }
*/
		if (ParaTend.uchTend < CLASS_UITOTAL3 || ItemTotalType(&UifRegTotal) != ITM_TYPE_NONFINALIZE) {
            ItemTotal->uchMinorClass = CLASS_TOTAL2;
        } else {
            UifRegTotal.uchMinorClass = ParaTend.uchTend;
            ItemTotal->uchMinorClass = ParaTend.uchTend;
        }
    } else {
		ITEMTOTALLOCAL  TotalLocalData;

        ItemTotalGetLocal(&TotalLocalData);
        UifRegTotal.uchMinorClass = ItemTotal->uchMinorClass = TotalLocalData.uchSaveMinor;
    }

    ItemTotalSplTax(&UifRegTotal, ItemTotal, &WorkTax, &WorkVAT);
    RflRound(&ItemTotal->lMI, ItemTotal->lMI, RND_TOTAL1_ADR);          /* Saratoga */
}        

/*==========================================================================
**  Synopsis:   VOID    ItemTendFastTenderSplit(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate tender data
==========================================================================*/

VOID    ItemTendFastTenderSplit(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    DCURRENCY   lMI;

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */

    if (ItemTender->aszNumber[0] == 0) {
        memcpy(ItemTender->aszNumber, UifRegTender->aszNumber, sizeof(ItemTender->aszNumber));
    }
    if (UifRegTender->aszNumber[0] != 0) {
        memcpy(ItemTenderLocal.aszNumber, UifRegTender->aszNumber, sizeof(ItemTenderLocal.aszNumber));
    }
    if (ItemTender->auchExpiraDate[0] != 0) {              
        _tcsncpy(ItemTenderLocal.auchExpiraDate, ItemTender->auchExpiraDate, NUM_EXPIRA);
    }

	ItemTenderMdcToStatus (UifRegTender->uchMinorClass, ItemTender->auchTendStatus);

    lMI = ItemTender->lRound;
    if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {                    /* preselect void */
        lMI *= -1L;
    }
#if 0
    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT3 ) && lMI <= 0L ) {
        ItemTendAudVal( ItemTender->lRound );                       /* audaction validation */
    }

    if ( ItemTender->auchStatus[0] & ITEMTENDER_STATUS_0_VALIDATION ) {   /* validation print */
        ItemTender->fsPrintStatus = PRT_VALIDATION;                           /* print validation */ 
        TrnThrough( ItemTender );
    }
#endif
    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */
#if 1
    if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
        ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	}
#endif

    ItemTendFastDisp(UifRegTender, ItemTender);
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

	TrnItem(ItemTender);
    MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTender);
        
    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);
}


/*==========================================================================
**  Synopsis:   VOID    ItemTendDupRecSpl(ITEMTENDER *ItemTender, UIFREGTENDER *UifTender)
*
*   Input:      ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    print duplicate receipt for CP,EPT Split Tender
==========================================================================*/

VOID    ItemTendDupRecSpl(ITEMTENDER *ItemTender, UIFREGTENDER *UifTender)
{
    if ((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC)))
        && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

        (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC)))
        && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||  
            (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF)) 
        && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2)))
	{
		TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
		UCHAR           uchPrintType;
		UCHAR           uchKitchen = pWorkCur->uchKitchenStorage;   // save current uchKitchenStorage
		UCHAR           fbCompPrint = pWorkCur->fbCompPrint;        // save fbCompPrint
		UCHAR           fbNoPrint = pWorkCur->fbNoPrint;            // save fbNoPrint

		// modify the receipt printing settings which will be restored when we exit.
        pWorkCur->fbCompPrint |= CURQUAL_COMP_R;
        pWorkCur->fbNoPrint |= (CURQUAL_NO_J | CURQUAL_NO_V | CURQUAL_NO_S);
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;

		pWorkCur->flPrintStatus |= CURQUAL_POSTRECT;	/* do not print duplicate receipt data on journal */

        if (CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT2) == 0) {
            uchPrintType = TRN_PRT_FULL;                                            /* Full Print option*/
        } else {
            uchPrintType = TRN_PRT_TTL;                                            /* Full Print option*/
        }

        ItemTendSplitTenderDup(uchPrintType, UifTender);
        ItemTender->fbStorageStatus = NOT_ITEM_STORAGE;
        ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */
        TrnItem(ItemTender);

        ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

        if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT          /* exist EPT tend ? */
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {   /* EPT logo */
            ItemTrailer( TYPE_EPT );                                /* EPT trailer      */
        } else {
            ItemTrailer( TYPE_STORAGE );                            /* normal trailer   */
        }
        ItemTendDupCloseSpl(uchPrintType);

		// restore the print settings that were modified for the duplicated receipt.
        pWorkCur->fbNoPrint = fbNoPrint;
        pWorkCur->fbCompPrint = fbCompPrint;
        pWorkCur->uchKitchenStorage = uchKitchen;              
    }
}                 

/*==========================================================================
**  Synopsis:   VOID    ItemTendSplitTenderDup(UCHAR uchPrintType, UIFREGTENDER *UifTender)
*
*   Input:      UCHAR uchPrintType 
*   Output:     
*   InOut:      none
*
*   Return:     
*
*   Description:    Split Print Tender for CP Duplicate Rec.
==========================================================================*/
VOID    ItemTendSplitTenderDup(UCHAR uchPrintType, UIFREGTENDER *UifTender)
{
    SHORT               i;
    SHORT               sFirstSeatFlag = 0;     /* 5/18/96 */ 
    DCURRENCY           lTax = 0;
	ITEMTOTAL           ItemTotal = {0}, Total = {0};
    ITEMAFFECTION       Tax = {0}, IndTax = {0};
    TRANGCFQUAL         *WorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL         WorkCur;
    ITEMTOTALLOCAL      TotalLocalData;         /* 5/29/96 */
    USCANTAX            *pTax = &Tax.USCanVAT.USCanTax;
    USCANTAX            *pIndTax = &IndTax.USCanVAT.USCanTax;

    TrnGetCurQual(&WorkCur);
    ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SPLIT_TENDER; 

    for (i = 0; i < NUM_SEAT && WorkGCF->auchUseSeat[i] != 0; i++) { //SR206
        /*----- Check Finalize Seat# -----*/
        if (TrnSplSearchFinSeatQueue(WorkGCF->auchUseSeat[i]) == TRN_SUCCESS) {
            continue;
        }

        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

        /*----- Make Split Transaction -----*/
        TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_POSTREC_STO, TRN_TYPE_SPLITA);

        /*----- Make Tax, Total Data -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            ItemTendAffectVATSplit(WorkCur.auchTotalStatus, &Tax, UifTender, &lTax, 1);    /* V3.3 */
        } else {
            ItemTendAffectTaxSplit(WorkCur.auchTotalStatus, &Tax, UifTender, 1);
        }
        ItemTendSplitCheckTotal(&ItemTotal, UifTender);
        TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);            /* Individual Total */

        /*----- Check First Seat Print or not 5/18/96 -----*/
        if (sFirstSeatFlag == 0) {              /* First Seat for Print */
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, 0, ItemTotal.auchTotalStatus, uchPrintType);
            sFirstSeatFlag = 1;
        } else {
            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, 1, i, ItemTotal.auchTotalStatus, uchPrintType);
        }

        /*----- Make Whole Total -----*/
        Total.lMI += ItemTotal.lMI;
    }

    /*----- Get Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_POSTREC_STO) == TRN_SUCCESS) {
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
			TRANITEMIZERS       *WorkTI;

            ItemTendAffectVATSplit(WorkCur.auchTotalStatus, &Tax, UifTender, &lTax, 1);   /* V3.3 */

            /*----- Individual Total of Base Transaction -----*/
            TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
            ItemTotal.lMI = WorkTI->lMI + lTax;
            ItemTotal.lTax = lTax;
            ItemTotal.lService = WorkTI->lMI + lTax;
            TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(0, TRN_SPL_NOT_UPDATE, 1, 1, ItemTotal.auchTotalStatus, TRN_PRT_FULL); 

            /*----- Whole Total of Base Transaction -----*/
            Total.lMI += WorkTI->lMI + lTax;
            Total.lTax += lTax;
        } else {
			TRANITEMIZERS       *WorkTI;
			ITEMCOMMONTAX       WorkTax = {0};

            TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);

            /*----- Make Subtotal,   R3.1 5/8/96 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                IndTax.lAmount = WorkTI->lMI; 
            }

            /*----- Individual Tax of Base Transaction -----*/
            if (TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN) {
                ItemCanTax(&WorkCur.auchTotalStatus[1], &WorkTax, ITM_SPLIT);
            } else {
                ItemCurTaxSpl(&WorkCur.auchTotalStatus[1], &WorkTax);
            }
            IndTax.uchMajorClass = CLASS_ITEMAFFECTION;
            IndTax.uchMinorClass = CLASS_TAXPRINT;
            IndTax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

            if (TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN) {
                for (i = 0; i < 5; i++) {
                    pIndTax->lTaxable[i] += WorkTax.alTaxable[i];
                    pTax->lTaxable[i] += pIndTax->lTaxable[i];
                }
                for (i = 0, lTax = 0L; i < 4; i++) {
                    pIndTax->lTaxAmount[i] += WorkTax.alTax[i];
                    pIndTax->lTaxExempt[i] += WorkTax.alTaxExempt[i];
                    pTax->lTaxAmount[i] += pIndTax->lTaxAmount[i];
                    pTax->lTaxExempt[i] += pIndTax->lTaxExempt[i];
                    lTax += WorkTax.alTax[i];
                }

                /*----- adjust tax print format 4/22/96 -----*/
                if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                    pIndTax->lTaxAmount[1] += pIndTax->lTaxAmount[2] + pIndTax->lTaxAmount[3];
                    pIndTax->lTaxAmount[2] = pIndTax->lTaxAmount[3] = 0L;

                } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                    && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
                    pIndTax->lTaxAmount[0] += pIndTax->lTaxAmount[1]
                        + pIndTax->lTaxAmount[2] + pIndTax->lTaxAmount[3];
                    pIndTax->lTaxAmount[1] = pIndTax->lTaxAmount[2] = pIndTax->lTaxAmount[3] = 0L;
                }
            } else {
                for (i = 0, lTax = 0L; i < 3; i++) {
                    pIndTax->lTaxable[i] += WorkTax.alTaxable[i];
                    pIndTax->lTaxAmount[i] += WorkTax.alTax[i];
                    pIndTax->lTaxExempt[i] += WorkTax.alTaxExempt[i];
                    pTax->lTaxable[i] += pIndTax->lTaxable[i];
                    pTax->lTaxAmount[i] += pIndTax->lTaxAmount[i];
                    pTax->lTaxExempt[i] += pIndTax->lTaxExempt[i];
                    lTax += WorkTax.alTax[i];
                }
            }
            TrnItemSpl(&IndTax, TRN_TYPE_SPLITA);

            /*----- Individual Total of Base Transaction -----*/
            ItemTotal.lMI = WorkTI->lMI + lTax;
            ItemTotal.lTax = lTax;
            ItemTotal.lService = WorkTI->lMI + lTax;
            TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

            /*----- Buffering Print 4/19/96, 6/9/96 -----*/
            TrnSplPrintSeat(0, TRN_SPL_NOT_UPDATE, 1, 1, ItemTotal.auchTotalStatus, uchPrintType); 

            /*----- Whole Total of Base Transaction -----*/
            Total.lMI += WorkTI->lMI + lTax;
            Total.lTax += lTax;
        }
    }

    /*----- Re-Create Consoli Storage -----*/
    TrnInitialize(TRANI_CONSOLI);
    ItemPrintStart(TYPE_SPLIT_TENDER);

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        Tax.fbStorageStatus = NOT_ITEM_STORAGE;
        ItemTendVATCommon(&Tax, ITM_NOT_SPLIT, ITM_NOT_DISPLAY);
    } else {
        /*----- Whole Tax Data -----*/
        Tax.uchMajorClass = CLASS_ITEMAFFECTION;
        Tax.uchMinorClass = CLASS_TAXPRINT;
        Tax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        /*----- adjust tax print format 4/22/96 -----*/
        if (TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN) {
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                pTax->lTaxAmount[1] += pTax->lTaxAmount[2] + pTax->lTaxAmount[3];
                pTax->lTaxAmount[2] = pTax->lTaxAmount[3] = 0L;

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
                pTax->lTaxAmount[0] += pTax->lTaxAmount[1] + pTax->lTaxAmount[2] + pTax->lTaxAmount[3];
                pTax->lTaxAmount[1] = pTax->lTaxAmount[2] = pTax->lTaxAmount[3] = 0L;
            }
        }

        /*----- Make Subtotal,   R3.1 4/26/96 -----*/
        for (i = 0, lTax = 0L; i < 4; i++) {
            lTax += pTax->lTaxAmount[i];
        }
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            Tax.lAmount = Total.lMI - lTax; 
        }

        Tax.fbStorageStatus = NOT_ITEM_STORAGE;
        TrnItem(&Tax);
    }

    /*----- Whole Total Data -----*/
    ItemTotalGetLocal(&TotalLocalData);             /* 5/29/96 */                                             
    memcpy( &Total.aszNumber[0], &TotalLocalData.aszNumber[0], sizeof(Total.aszNumber) );   /* 5/29/96 */

    Total.uchMajorClass = CLASS_ITEMTOTAL;
    Total.uchMinorClass = TotalLocalData.uchSaveMinor;
/*    Total.uchMinorClass = CLASS_TOTAL2; */
    Total.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    memcpy(Total.auchTotalStatus, WorkCur.auchTotalStatus, sizeof(Total.auchTotalStatus));
    Total.fbStorageStatus = NOT_ITEM_STORAGE;
    TrnItem(&Total);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendDupCloseSpl( UCHAR uchPrintType )   
*
*   Input:      UCHAR uchPrintType 
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction close data for CP duplicate rec. in split 
==========================================================================*/

VOID    ItemTendDupCloseSpl( UCHAR uchPrintType )
{
	ITEMTRANSCLOSE  ItemTransClose = {0};
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();

    if ( uchPrintType == TRN_PRT_FULL ) {
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;                /* postheck, store/recall */
    } else {
        pWorkCur->uchPrintStorage = PRT_CONSOL_CP_EPT;                /* only print total/tender */
    }
    pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;                      /* for kitchen print */
    pWorkCur->fbCompPrint &= ~PRT_SLIP;
                                                                    
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;            /* major class */
    ItemTransClose.uchMinorClass = CLASS_CLSTENDER1;                /* minor class */
    TrnClose( &ItemTransClose );                                    /* transaction module i/F */
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

VOID    ItemTendOverTenderSplit( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
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

    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */
#if 1
    if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
        ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	}
#endif

    ItemTendPrtFSChange(ItemTender);                                /* generate FS change data */

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
**  Synopsis:   VOID    ItemTendSplitValidation( UIFREGTENDER *UifRegTender,
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

VOID    ItemTendSplitValidation(CONST UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    DCURRENCY   lMI;
    SHORT       sValPrintFlag = 0;
    USHORT      fsPrintStatus;                  /* 51:print status */

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
    ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */

    if (ItemTender->aszNumber[0] == 0) {
        memcpy(ItemTender->aszNumber, UifRegTender->aszNumber, sizeof(ItemTender->aszNumber));
    }
    if (UifRegTender->aszNumber[0] != 0) {
        memcpy(ItemTenderLocal.aszNumber, UifRegTender->aszNumber, sizeof(ItemTenderLocal.aszNumber));
    }
    if (ItemTender->auchExpiraDate[0] != 0) {              
        _tcsncpy(ItemTenderLocal.auchExpiraDate, ItemTender->auchExpiraDate, NUM_EXPIRA);
    }

	ItemTenderMdcToStatus (UifRegTender->uchMinorClass, ItemTender->auchTendStatus);

    lMI = ItemTender->lRound;
    if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {                    /* preselect void */
        lMI *= -1L;
    }

    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION ) {  /* validation print required */
        ItemTender->fsPrintStatus = PRT_VALIDATION;                          /* print validation */ 
        TrnThrough( ItemTender );
        sValPrintFlag = 1;
    }

    /* --- DTREE#2: Issue#3 Check Endorsement,  Dec/18/2000, 21RFC05402 --- */
    ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);

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
        ItemTendAudVal( ItemTendWholeTransAmount() );                       /* audaction validation */
        /*ItemTendAudVal( ItemTender->lRound );                       / audaction validation */
        sValPrintFlag = 1;
    }
    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2) && (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN))) {
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
    if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
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
	    	TrnPrtSetSlipPageLine(0);
	       	fsPrintStatus = ItemTender->fsPrintStatus;
       		ItemTender->fsPrintStatus = PRT_SLIP;             			/* print slip */
   		    TrnThrough( ItemTender );
      		ItemTender->fsPrintStatus = fsPrintStatus;
      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	        sValPrintFlag = 1;	/* 06/22/01 */
      		
      		/* release slip to prevent the optional listing slip */
	        if ( !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
				TrnSlipRelease ();
	   		} else {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
     	}
    }
}
/****** end of file ******/
