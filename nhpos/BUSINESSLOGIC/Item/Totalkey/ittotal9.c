/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TOTAL KEY MODULE
:   Category       : TOTAL KEY MODULE, NCR 2170 GP R2.0 Application
:   Program Name   : ITTOTAL3.C (food stamp total)
:  ---------------------------------------------------------------------
:  Abstract:
:   ItemTotalFSTotal()          ;   food stamp total
:   ItemTotalFSCheck()          ;   check modifier, condition
:   ItemTotalFSItem()           ;   generate F.S total data class
:   ItemTotalFSSet()            ;   set tender local data
:   ItemTotalFSCheckTenderKey() ;   Check F.S Tender Key Status (R2.0GCA)
:
:  ---------------------------------------------------------------------
:  Update Histories
:   Date    :   Version/Revision  :   Name    :   Description
:   93/3/01     00.00.01              hkato
:   94/3/03     00.00.01              kkamada :R1.1 Wic prohibit at fstotal
:   94/3/14                           hkato   :R1.1
:   96/12/11    02.00.00              hrkato  :R2.0
:   00/5/15     01.00.01              hrkato  :Saratoga FVT
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
#include    "regstrct.h"
#include    "uie.h"
#include    "uireg.h"
#include    "pif.h"
#include    "rfl.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csttl.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "cpm.h"
#include    "eept.h"
#include    "fdt.h"
#include    "itmlocal.h"

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalFS(UIFREGTOTAL *pUifTotal)
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*   Return:
*
*   Description:    food stamp total key (ITM_TYPE_FS_TOTAL)
*==========================================================================
*/
SHORT   ItemTotalFS(UIFREGTOTAL *pUifTotal)
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};

    if ((sStatus = ItemTotalFSCheck()) != UIF_SUCCESS) {
        return(sStatus);
    }

#if 0
	ItemTotal.auchTotalStatus[0] = pUifTotal->uchMinorClass;
    CliParaAllRead(CLASS_PARATTLKEYCTL, &ItemTotal.auchTotalStatus[1], 6, (USHORT)(6 * (pUifTotal->uchMinorClass - 1)), &sStatus);
#else
	ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex (pUifTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, pUifTotal->uchMinorClass);
#endif

    ItemTotal.lMI = TranItemizersPtr->Itemizers.TranUsItemizers.lFoodStampable;   // calculate F.S total amount
    ItemTotalFSItem(pUifTotal, &ItemTotal);

    if (ItemTotal.auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {  /* drawer compulsory */
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
    ItemTotalFSSet(&ItemTotal);

    return(UIF_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalFSCheck(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check modifier, condition
*==========================================================================
*/
SHORT   ItemTotalFSCheck(VOID)
{
    SHORT           i, sSign = 1;
    DCURRENCY       lTax;
	ITEMAFFECTION   Affect = {0};

    if (ItemModLocalPtr->fsTaxable || (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD)) {
        return(LDT_SEQERR_ADR);
    }

    if (TranCurQualPtr->uchSplit == CLASS_SPLIT || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(LDT_SEQERR_ADR);
    }
    /* 02/14/01 */
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_STORE_RECALL) {
        if (TranGCFQualPtr->usGuestNo != 0 ||
            (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
            return(LDT_SEQERR_ADR);
        }
    }

    if (ItemTenderLocalPtr->fbTenderStatus[2] & (TENDER_FC1 | TENDER_FC2 | TENDER_FC3 | TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8)) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) {
        return(LDT_SEQERR_ADR);
    }

   	/* TAR177387 */
    if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
        return(LDT_SEQERR_ADR);
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {        /* no wic transaction */
        return(LDT_PROHBT_ADR);
    }

    ItemTotalAutoChargeTips();
    ItemTotalPrev();

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

    lTax = 0;
    for (i = 0; i < 3; i++) {
        lTax += Affect.USCanVAT.USCanTax.lTaxAmount[i];
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        sSign = -1;
    }

	if (!CliParaMDCCheckField(MDC_FS2_ADR, MDC_PARAM_BIT_B)) {
		TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

		if (sSign * (WorkTI->lMI + lTax) <= 0L
			|| sSign * WorkTI->Itemizers.TranUsItemizers.lFoodStampable <= 0L) {
			return(LDT_PROHBT_ADR);
		}
	}

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTotalFSItem(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTOTAL    *ItemTotal
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:
*
*   Description:    generate F.S total data class
*==========================================================================
*/
VOID    ItemTotalFSItem(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem)
{
	REGDISPMSG          Disp = {0};

    pItem->uchMajorClass = CLASS_ITEMTOTAL;
    pItem->uchMinorClass = CLASS_TOTAL9;

    Disp.uchMajorClass = pUifTotal->uchMajorClass;
    Disp.uchMinorClass = pUifTotal->uchMinorClass;

    RflGetTranMnem (Disp.aszMnemonic, TRN_TTL9_ADR);

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        if (pItem->lMI - ItemTenderLocalPtr->lFSTenderizer <= 0L) {
            Disp.lAmount = pItem->lMI - ItemTenderLocalPtr->lFSTenderizer;
        } else {
            Disp.lAmount = 0L;
        }
    } else {
        if (pItem->lMI - ItemTenderLocalPtr->lFSTenderizer >= 0L) {
            Disp.lAmount = pItem->lMI - ItemTenderLocalPtr->lFSTenderizer;
        } else {
            Disp.lAmount = 0L;
        }
    }
    MldDispSubTotal(MLD_TOTAL_1, Disp.lAmount);

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {        /* validation print */
        pItem->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItem);
    }
    pItem->fsPrintStatus = PRT_JOURNAL | PRT_SLIP;

    flDispRegDescrControl |= (FOODSTAMP_CTL | TOTAL_CNTRL);
    flDispRegKeepControl  |= (FOODSTAMP_CTL | TOTAL_CNTRL);
    Disp.fbSaveControl = 1;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~(FOODSTAMP_CTL | TOTAL_CNTRL);
    flDispRegKeepControl  &= ~(FOODSTAMP_CTL | TOTAL_CNTRL);

    TrnThroughDisp( pItem );
    
    ItemSendKds(pItem, 0);  /* send to enhanced kds, 2172 */
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTotalFSSet(ITEMTOTAL *pItem)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    save F.S total data class
*==========================================================================
*/
VOID    ItemTotalFSSet(ITEMTOTAL *pItem)
{
    SHORT           i;
    ITEMTENDERLOCAL *pWorkTend = ItemTenderGetLocalPointer();

    pWorkTend->ItemFSTotal = *pItem;
    memset(&pWorkTend->ItemTotal, 0, sizeof(ITEMTOTAL));

    for (i = 0; i < 3; i++) {
        pWorkTend->lWorkTaxable[i] = 0L;              /* work taxable itemizer */
        pWorkTend->lWorkTax[i] = 0L;                  /* work tax itemizer */
        pWorkTend->lWorkExempt[i] = 0L;               /* work tax exempt itemizer */
        pWorkTend->lWorkFSExempt[i] = 0L;             /* fs tax exempt itemizer */
    }

    /* V2.01.04 */
    pWorkTend->fbTenderStatus[1] |= TENDER_FS_TOTAL;
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalCFS(UIFREGTOTAL *pUifTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal      
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check total for food stamp (ITM_TYPE_CHECK_FS_TOTAL)
*==========================================================================
*/
SHORT   ItemTotalCFS(UIFREGTOTAL *pUifTotal)
{
    SHORT           sStatus;
//    SHORT           sWork;
	ITEMTOTAL       ItemTotal = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    ITEMTENDERLOCAL *WorkTend = ItemTenderGetLocalPointer();

    ItemTotalPrev();

	if (WorkTend->ItemFSTender.uchMajorClass != 0) {             /* FS Tender */
		SHORT           i;
		ITEMAFFECTION   Affect = {0};

#if 0
        ItemTotal.auchTotalStatus[0] = pUifTotal->uchMinorClass;                    
        CliParaAllRead(CLASS_PARATTLKEYCTL, &ItemTotal.auchTotalStatus[1], 6, 6, &sWork);
#else
		ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex (pUifTotal->uchMinorClass);
		ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, pUifTotal->uchMinorClass);
#endif

        if (TranGCFQualPtr->usGuestNo == 0) {
            ItemTotalCalTax(&Affect, ITM_CALTAX_TRANS_OPER);
        } else {
            ItemTotalCalTax(&Affect, ITM_CALTAX_TRANS_SR);
        }
        for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
            ItemTotal.lMI += Affect.USCanVAT.USCanTax.lTaxAmount[i];
        }
    } else {
        if ((sStatus = ItemTotalCTTax(pUifTotal, &ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal.uchMinorClass = CLASS_TOTAL2;
    ItemTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    ItemTotalCTAmount(&ItemTotal);
    ItemTotalCFSItem(pUifTotal, &ItemTotal);

    WorkTend->ItemTotal = ItemTotal;
    if ((WorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0) {
        memset(&WorkTend->ItemFSTotal, 0, sizeof(ITEMTOTAL));
    }
    WorkTend->fbTenderStatus[1] &= ~TENDER_FS_TOTAL;

    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */

    return( ITM_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalCFSItem(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data
*==========================================================================
*/
SHORT   ItemTotalCFSItem(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem)
{
    UCHAR           uchSystem = 0;
    SHORT           sSize;
    USHORT          usSize;
	REGDISPMSG      DispMsg = {0};

    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    sSize = ItemCommonCheckSize( pItem, usSize );
    ItemPreviousItem( pItem, ( USHORT )sSize );

    DispMsg.uchMajorClass = pUifTotal->uchMajorClass;
    DispMsg.uchMinorClass = pUifTotal->uchMinorClass;
    DispMsg.lAmount = pItem->lMI;
    RflGetLead (DispMsg.aszMnemonic, LDT_BD_ADR);
    DispMsg.fbSaveControl = 5;                                  /* save for redisplay, DISP_SAVE_TYPE_5*/

	flDispRegDescrControl |= TOTAL_CNTRL;   
    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) ||     /* Display byPrnt Priority */
        (MldGetMldStatus() == FALSE)) {                         /* 2x20 scroll, V3.3 */
		MLDTRANSDATA    WorkMLD;

        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL );

    TranCurQualTotalStatusPut (pItem->auchTotalStatus);
    TrnThroughDisp( pItem );
    
    /* send to enhanced kds, 2172 */
    ItemSendKds(pItem, 0);

    return( ITM_SUCCESS );
}

/***** end of ittotal9.c *****/