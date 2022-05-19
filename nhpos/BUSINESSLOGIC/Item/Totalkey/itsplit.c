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
:   Title          : TOTAL KEY MODULE                                 
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application 
:   Program Name   : ITSPLIT.C (Split Key)                                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTotalSplit()           ; Split Key
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:    Date  : Ver.Rev. :   Name    	: Description
: 11/17/95 : 03.01.00 :   hkato     : R3.1
: 08/13/99 : 03.05.00 :   M.teraki  : Merge STORE_RECALL_MODEL
:          :          :             :       /GUEST_CHECK_MODEL
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
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csttl.h"
#include    "display.h"
#include    "mld.h"
#include    "rfl.h"
#include    "trans.h"
#include    "item.h"
#include    "itmlocal.h"

static SHORT   ItemTotalSPCheck(UIFREGTOTAL *pData);
static VOID    ItemTotalSPItem(UIFREGTOTAL *UifRegTotal);

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSP(UIFREGTOTAL *UifRegTotal)
*
*   Input:      none       
*   Output:     none    
*   InOut:      UIFREGTOTAL *UifRegTotal
*
*   Return:     
*
*   Description:    Split Key #1-3
==========================================================================*/

SHORT   ItemTotalSP(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
    ITEMCOMMONLOCAL *pWorkCommon = ItemCommonGetLocalPointer();        /* 6/11/96 */

    if ((sStatus = ItemTotalSPCheck(UifRegTotal)) != ITM_SUCCESS) {
        return(sStatus);
    }

    ItemPrevCancelAutoTips();         /* cancel auto charge tips by check total, V3.3 */
    ItemTotalPrev();                  /*----- Send Previous "Item" -----*/
    TranCurQualTotalStatusClear();    /*----- Clear Prev. Total Status 6/11/96 -----*/

    if (pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
        memset(&pWorkCommon->ItemSales, 0, sizeof(ITEMSALES));   
    }                                                           

    /*----- Split Total -----*/
	{
		ITEMTOTAL       ItemTotal = {0};

		ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
		ItemTotal.uchMinorClass = CLASS_SPLIT;
		ItemTotalSPItem(UifRegTotal);
		TrnTotal(&ItemTotal);
	}

    /*----- Not Multi Seat# Tender -----*/
    if (TrnSplCheckSplit() != TRN_SPL_SEAT) {
		TRANINFORMATION *TrnInf;
		TRANINFSPLIT    *SplitA, *SplitB;

        /*----- Create Split File -----*/
        TrnSplCheck();

        /*----- Display Split Transaction -----*/
        MldSetMode(MLD_SPLIT_CHECK_SYSTEM);
        TrnSplGetDispSeat(&TrnInf, &SplitA, &SplitB);

        /*----- Display Base Transaction -----*/
        MldDisplayInit(MLD_DRIVE_1 | MLD_DRIVE_2 | MLD_DRIVE_3, 0);
        if (TrnInf->TranGCFQual.usGuestNo != 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
			MLDTRANSDATA    MldTran = {0};

            MldTran.usVliSize = TrnInf->usTranConsStoVli;
            MldTran.sFileHandle = TrnInf->hsTranConsStorage;
            MldTran.sIndexHandle = TrnInf->hsTranConsIndex;
            MldPutSplitTransToScroll(MLD_SCROLL_1, &MldTran);
            MldDispSubTotal(MLD_TOTAL_1, WorkTI->lMI);
        }

        /*----- Display Split#1 Transaction -----*/
        if (SplitA->TranGCFQual.usGuestNo != 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
			MLDTRANSDATA    MldTran = {0};

            MldTran.usVliSize = SplitA->usTranConsStoVli;
            MldTran.sFileHandle = SplitA->hsTranConsStorage;
            MldTran.sIndexHandle = SplitA->hsTranConsIndex;
            MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);
            MldDispSubTotal(MLD_TOTAL_2, WorkTI->lMI);
        }

        /*----- Display Split#2 Transaction -----*/
        if (SplitB->TranGCFQual.usGuestNo != 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITB);
			MLDTRANSDATA    MldTran = {0};

            MldTran.usVliSize = SplitB->usTranConsStoVli;
            MldTran.sFileHandle = SplitB->hsTranConsStorage;
            MldTran.sIndexHandle = SplitB->hsTranConsIndex;
            MldPutSplitTransToScroll(MLD_SCROLL_3, &MldTran);
            MldDispSubTotal(MLD_TOTAL_3, WorkTI->lMI);
        }
    }

    ItemCommonPutStatus(COMMON_VOID_EC);

    return(ITM_SUCCESS);
}    

/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalSPCheck(UIFREGTOTAL *UifRegTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    check modifier, condition
==========================================================================*/

static SHORT   ItemTotalSPCheck(UIFREGTOTAL *UifRegTotal)
{
    SHORT   sStatus;

    /*----- Previous SoftCheck System -----*/
    if(CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
		NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Sequence Error - MDC 236 Bit B");
        return(LDT_SEQERR_ADR);    // CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
    }

    if ((sStatus = ItemCommonCheckExempt()) != ITM_SUCCESS) {
        return(sStatus);
    }

    if (( sStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
        return(sStatus);
    }

    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_POSTCHECK) {
		NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Sequence Error - not Post Check System.");
        return(LDT_SEQERR_ADR);    // (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_POSTCHECK
    }

    /*----- Prohibit PostCheck System -----*/
    if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
		NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Sequence Error - MDC 12 Bit A");
        return(LDT_SEQERR_ADR);    // CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
    }

    /*----- Prohibit Cashier Trans -----*/
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
        return(LDT_SEQERR_ADR);
    }
    /* R3.3 */
    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) { /* cashier interrupt */
        return(LDT_SEQERR_ADR);
    }

    /*----- Prohibit Modifier -----*/
    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);
    }

    if (UifRegTotal->aszNumber[0] != 0) {
        return(LDT_SEQERR_ADR);
    }

    /* --- Prohibit Multi Check --- */
    if (ItemTransLocalPtr->uchCheckCounter != 0) {
        return(LDT_SEQERR_ADR);
    }                     

    /*----- During Seat# Tender -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT) {
        return(LDT_SEQERR_ADR);
    }

    /*----- During Item Transfer -----*/
    if (TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSPItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate split check data
==========================================================================*/

static VOID    ItemTotalSPItem(UIFREGTOTAL *UifRegTotal)
{
	REGDISPMSG      DispMsg = {0};

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;

    RflGetTranMnem(DispMsg.aszMnemonic, TRN_SPLIT_ADR);

    flDispRegDescrControl |= TOTAL_CNTRL;
    DispMsg.fbSaveControl = 1;
    DispWrite(&DispMsg);

    flDispRegDescrControl &= ~(TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL);
}

/****** End of Source ******/
