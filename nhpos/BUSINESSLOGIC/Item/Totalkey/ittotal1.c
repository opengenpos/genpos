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
:   Program Name   : ITTOTAL1.C (subtotal)                                        
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTotalST()           ;   subtotal
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date    :   Version/Revision    :   Name    :   Description
:   92/5/24 :   00.00.01            :   hkato   : Initial
:   95/3/23 :   03.00.00            :   hkato   : R3.0
:   95/11/10:   03.01.00            :   hkato   : R3.1
:   96/06/24:   03.01.09            :   M.Ozawa : send total data to KDS directly
:   99/08/13:   03.05.00            :   M.teraki: Merge STORE_RECALL_MODEL
:           :                       :           :       /GUEST_CHECK_MODEL
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
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"


/*==========================================================================
**  Synopsis:   SHORT   ItemTotalST( UIFREGTOTAL *UifRegTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal       
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    subtotal
==========================================================================*/

SHORT   ItemTotalST( UIFREGTOTAL *UifRegTotal )
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};

    if ( ItemModLocalPtr->fsTaxable ) {                                 /* check modifier */
        return( LDT_SEQERR_ADR );                                       /* sequence error */                 
    }                                                   

	if (TranCurQualPtr->uchSplit == CLASS_SPLIT
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(LDT_SEQERR_ADR);
    }

	if (ItemTenderLocalPtr->ItemFSTotal.uchMajorClass != 0) {
        return(LDT_SEQERR_ADR);
    }

    ItemTotalPrev();                 

    if ((sStatus = ItemTotalSTTax(UifRegTotal, &ItemTotal)) != ITM_SUCCESS) {
        return(LDT_SEQERR_ADR);
    }

    ItemTotalSTAmount( &ItemTotal );                                    /* calculate subtotal amount */
    if ( ( sStatus = ItemTotalSTItem( UifRegTotal, &ItemTotal ) ) != ITM_SUCCESS) { /* generate subtotal data */
        return( sStatus );                                              /* storage full */                 
    }

    if ( ITM_TTL_FLAG_DRAWER(&ItemTotal) ) {  
        ItemDrawer();                                                   /* drawer open */

        if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
            UieDrawerComp( UIE_ENABLE );  
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
                                            
    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */
    ItemCommonCancelExempt();                                           /* exit tax exempt condition */
                                                        
    return( ITM_SUCCESS );                                              /* success */
}    

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSTTax(UIFREGTOTAL *UifTotal, ITEMTOTAL *Total)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal          
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none 
*
*   Return:     
*
*   Description:    calculate sub-total tax
=========================================================================*/

SHORT   ItemTotalSTTax(UIFREGTOTAL *UifTotal, ITEMTOTAL *Total)
{
    SHORT           sStatus;
//    SHORT           sWork;
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
//    TRANCURQUAL     WorkCur;

#if 0
    Total->auchTotalStatus[0] = UifTotal->uchMinorClass;
    CliParaAllRead(CLASS_PARATTLKEYCTL, &Total->auchTotalStatus[1], 6, 0, &sWork);
                                                                    /* R3.1 */
#else
	Total->auchTotalStatus[0] = ItemTotalTypeIndex (UifTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (Total->auchTotalStatus, UifTotal->uchMinorClass);
#endif

/*** V3.3 ICI 
    TrnGetCurQual(&WorkCur);
    if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        /----- Not Use "Not Consoli Print" Option -----/
        Total->auchTotalStatus[4] &= ~0x20;
    }
***/

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ((sStatus = ItemCurTax(&Total->auchTotalStatus[1], &WorkTax)) != ITM_SUCCESS) {
            return(sStatus);
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax(&Total->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT);
    } else {
        ItemCurVAT(&Total->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
    }
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        Total->lMI += WorkVAT.lPayment;
    } else {
        Total->lMI += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalSTAmount( ITEMTOTAL *ItemTotal )   
*
*   Input:      ITEMTOTAL   *ItemTotal       
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:     none
*
*   Description:    calculate subtotal amount
==========================================================================*/

VOID    ItemTotalSTAmount( ITEMTOTAL *ItemTotal )
{
    if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {                     /* tray total transaction */
        ItemTotalTTAmount( ItemTotal );
        ItemTotal->sTrayCo = 0;                                     /* not used */
    } else {                                                        /* normal subtotal */
        ItemTotal->lMI += TranItemizersPtr->lMI;
        if ( ItemTransLocalPtr->uchCheckCounter != 0 ) {                     /* multi check payment */
            ItemTotal->lMI += ItemTransLocalPtr->lWorkMI;               
        }
    }
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount = 0;

        if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal->lMI = lAmount;
        }
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSTItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate subtotal data
==========================================================================*/

SHORT   ItemTotalSTItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    SHORT           sSize;
    USHORT          usSize;
	REGDISPMSG      DispMsg = {0};
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                     /* major class */
    ItemTotal->uchMinorClass = CLASS_TOTAL1;                        /* minor class */

    memcpy( &ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], sizeof (ItemTotal->aszNumber) );
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                      /* print journal */
                                        
    if ( ( ItemTotal->auchTotalStatus[4] & CURQUAL_TOTAL_BUFFERED_SUBTOTAL) == 0      /* subtotal is buffered */
        && ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        
        usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
                                                                          
        if ( ( sSize = ItemCommonCheckSize( ItemTotal, usSize ) ) < 0 ) {  /* storage full */
            WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;            /* storage full */
            return( LDT_TAKETL_ADR);
        }
                                                                
        ItemPreviousItem( ItemTotal, ( USHORT )sSize );             /* buffering */
    } else {                                                        /* subtotal is no print */
		ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

        if ( WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL ) {
            memset( &WorkCommon->ItemSales, 0, sizeof( ITEMSALES ) );   
        }                                                           

        TranCurQualTotalStatusClear();
    }

    if ( ItemTotal->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {          /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;                      /* print validation */ 
        TrnThrough( ItemTotal );
    }
    TrnThroughDisp( ItemTotal );                                    /* send total to KDS R3.1 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.lAmount = ItemTotal->lMI;

    RflGetTranMnem (DispMsg.aszMnemonic, TRN_TTL1_ADR);

    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_1;                           /* save for redisplay */

    if ( ( TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN ) ||       /* canada system */
        ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) || /* cashier system */
        ( !( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) ) ) {     /* not exempt */
        flDispRegDescrControl &= ~TAXEXMPT_CNTRL; 
        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                        /* tax exempt */
    }

    DispWrite( &DispMsg );                                          /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);
    
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) ||      /* Display by Prnt Priority */
        (MldGetMldStatus() == FALSE) ||                          /* 2x20 scroll, V3.3 */

        (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT3)          /* display all item by cashier interupt, R3.3*/
        && (TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT)
        && (TranCurQualPtr->fsCurStatus & CURQUAL_ADDCHECK)) ||

        (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT3)          /* display all item by guest check, R3.3*/
        && (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && (TranCurQualPtr->fsCurStatus & (CURQUAL_REORDER|CURQUAL_ADDCHECK))) ||

        /* V3.3 */
        (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT3)          /* display all item by guest check, R3.3*/
        && (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
        && (TranCurQualPtr->fsCurStatus & (CURQUAL_REORDER|CURQUAL_ADDCHECK))) ) {
		MLDTRANSDATA    WorkMLD;

        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }

    flDispRegDescrControl &= ~TOTAL_CNTRL; 

    return( ITM_SUCCESS );
}

/****** End of Source ******/
