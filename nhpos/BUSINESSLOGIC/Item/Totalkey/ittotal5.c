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
:   Program Name   : ITTOTAL5.C (non-finalize total)                                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemtotalNF()           ;   non-finalize, multi check total
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name    : Description
: 95/11/10 : 03.01.00 :   hkato   : R3.1
: 96/06/24 : 03.01.09 :   M.Ozawa : send total data to KDS directly
: 98/08/17 : 03.03.00 :  hrkato   : V3.3 (VAT/Service)
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

#include    <ecr.h>
#include    <regstrct.h>
#include    <uie.h>
#include    <uireg.h>
#include    <pif.h>
#include    <rfl.h>
#include    <transact.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csstbgcf.h>
#include    <csgcs.h>   
#include    <display.h>
#include    <mld.h>
#include    <item.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"

            
/*==========================================================================
**    Synopsis: SHORT   ItemTotalNF( UIFREGTOTAL *UifRegTotal )
*
*    Input:     UIFREGTOTAL *UifRegTotal          
*   Output:     none     
*   InOut:      none
*
*   Return:     
*
*   Description:    non-finalize total/multi check total
==========================================================================*/
    
SHORT   ItemTotalNF( UIFREGTOTAL *UifRegTotal )
{
    if (ItemTransLocalPtr->uchCheckCounter == 0) {                   /* normal check total */
        if (TranCurQualPtr->uchSplit != 0) {
			/*----- Split Key -----*/
            if (TranCurQualPtr->uchSeat == 0) {
				/*----- Split Key -> Total -----*/
                return(ItemTotalSplitTotal(UifRegTotal));
            } else {
				/*----- Split Key -> Seat# -> Total -----*/
                return(ItemTotalSplTotal(UifRegTotal));     /* Check total for split */
            }
        } else {
			/*----- No Split Key -----*/
            return(ItemTotalNFTotal(UifRegTotal));
        }
    } else {                                                /* multi check payment */
        return(ItemTotalNFM( UifRegTotal));
    }
}    

/*==========================================================================
**    Synopsis: SHORT   ItemTotalNFTotal( UIFREGTOTAL *UifRegTotal )
*
*    Input:     UIFREGTOTAL *UifRegTotal          
*   Output:     none     
*   InOut:      none
*
*   Return:     
*
*   Description:    non-finalize total
==========================================================================*/

SHORT   ItemTotalNFTotal( UIFREGTOTAL *UifRegTotal )
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};
    ITEMTENDERLOCAL *pWorkTend = ItemTenderGetLocalPointer();

    if ( ( sStatus = ItemTotalCTNCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                              /* error */                
    }                               
                                                    
    /* automatic charge tips, V3.3 */
    ItemTotalAutoChargeTips();
    ItemTotalPrev();

    if (pWorkTend->ItemFSTender.uchMajorClass != 0) {             /* FS Tender */
		ITEMAFFECTION   Affect = {0};

        ItemTotalCTFSTax(UifRegTotal, &ItemTotal, &Affect);     /* Saratoga */
    } else {
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        if ((sStatus = ItemTotalNFTax(UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }
                                                
    ItemTotalCTAmount( &ItemTotal );                                    /* calculate check total amount */
    if ( ( sStatus = ItemTotalNFItem( UifRegTotal, &ItemTotal ) ) != ITM_SUCCESS ) {                                         
        return( sStatus );
    }

    if ( ItemTotal.auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
        ItemDrawer();                                                   /* drawer open */

        if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
            UieDrawerComp( UIE_ENABLE );  
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    pWorkTend->ItemTotal = ItemTotal;
    if ((pWorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0) {
        memset(&pWorkTend->ItemFSTotal, 0, sizeof(ITEMTOTAL));
    }
    pWorkTend->fbTenderStatus[1] &= ~TENDER_FS_TOTAL;

    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalNFTax( UIFREGTOTAL *UifRegTotal, 
*           ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal 
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none 
*
*   Return:     
*
*   Description:    calculate tax
=========================================================================*/

SHORT   ItemTotalNFTax(UIFREGTOTAL *UifRegTotal,
            ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
{
    SHORT           sStatus;
    ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
    TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();

	ItemTotal->auchTotalStatus[0] = ItemTotalTypeIndex (UifRegTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, UifRegTotal->uchMinorClass);

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            pWorkGCF->fsGCFStatus &= ~( GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT );

            if ( pWorkMod->fsTaxable & MOD_GSTEXEMPT ) {                  /* GST exempt state */        
                pWorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;               /* GST exempt transaction */
            }

            if ( pWorkMod->fsTaxable & MOD_PSTEXEMPT ) {                  /* PST exempt state */        
                pWorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;               /* PST exempt transaction */
            }

            pWorkMod->fsTaxable &= ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT );    /* exempt state off */        
        }
        ItemCanTax(&ItemTotal->auchTotalStatus[1], WorkTax, ITM_NOT_SPLIT);
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            pWorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;               /* reset USEXEMPT bit */
            if ( pWorkMod->fsTaxable & MOD_USEXEMPT ) {               /* tax exempt case */
                pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {            /* Saratoga */
                pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
                return( sStatus );
            }

            pWorkMod->fsTaxable &= ~MOD_USEXEMPT;                     /* exempt state off */        
        } else {
            if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
                return( sStatus );
            }
        }
    } else {
        if ((sStatus = ItemCurVAT(&ItemTotal->auchTotalStatus[1], pVAT, ITM_NOT_SPLIT)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        ItemTotal->lMI += pVAT->lPayment;
        ItemTotal->lTax += pVAT->lPayment;
    } else {
		DCURRENCY       lTax;
        lTax = WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3];
        ItemTotal->lMI += lTax;                                         /* MI */
        ItemTotal->lTax += lTax;                                        /* all tax */
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalNFItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate non-finalize total data
==========================================================================*/

SHORT   ItemTotalNFItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    SHORT   sSize;
    USHORT  usSize;
	REGDISPMSG  DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                 /* major class */
    ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;      /* minor class */

    memcpy( &ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], sizeof(ItemTotal->aszNumber) );

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                  /* print journal */
    
    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    sSize = ItemCommonCheckSize( ItemTotal, usSize );           /* storage size */

    ItemPreviousItem( ItemTotal, sSize );             /* buffering */

    if ( ItemTotal->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {      /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;                  /* print validation */ 
        TrnThrough( ItemTotal );
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.lAmount = ItemTotal->lMI + ItemTransLocalPtr->lWorkMI;       /* display amount */

    RflGetTranMnem ( DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifRegTotal->uchMinorClass) );

    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = 1;                                  /* save for redisplay */

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                /* GST exempt */
        || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {          /* PST exempt */

        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                /* tax exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
    }
                                                                /* R3.0 */
    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);

    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) ||     /* Display by Prnt Priority */
        (MldGetMldStatus() == FALSE)) {                         /* 2x20 scroll, V3.3 */
		MLDTRANSDATA    WorkMLD;

        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL ); 
                                                                /* R3.1 */
    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);
    
    TrnThroughDisp( ItemTotal );                                /* send total to KDS R3.1 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    return( ITM_SUCCESS );
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalNFM( UIFREGTOTAL *UifRegTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    multi check total
==========================================================================*/

SHORT   ItemTotalNFM( UIFREGTOTAL *UifRegTotal )
{               
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};

    if ( ( sStatus = ItemTotalCTMCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                              /* error */                
    } 
                                                    
    /* automatic charge tips, V3.3 */
    ItemTotalAutoChargeTips();

    ItemTotalPrev();
//SR 279
		if(TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK)
		{
			// check to see if we are to recaculate tax on an add check or not.
			if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1)
				||((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK)) // we are in multiple check mode.)//one cent tax
			{
				ITEMCOMMONTAX   WorkTax = {0};
				ITEMCOMMONVAT   WorkVAT = {0};

				if ((sStatus = ItemTotalNFTax(UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
					return( sStatus );                                              /* error */
				}
			} else {
				//SR 279  Inside the ItemTotalNFTax, before it begins to do any tax calculation it gets
				//the type of Total that the user has pressed, and send its into the TranInformation structure.
				//we do that affection here now, so that TrnInformation keeps that information.
				ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex (UifRegTotal->uchMinorClass);      /* total key type */
				ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, UifRegTotal->uchMinorClass);
			}
		}

                                    
    if ( ( sStatus = ItemTotalNFMItem( UifRegTotal, &ItemTotal ) ) != ITM_SUCCESS) {
        return( sStatus );                                              /* storage full */                
    }

    if ( ItemTotal.auchTotalStatus[2] & CURQUAL_TOTAL_COMPUL_DRAWER) {
        ItemDrawer();                                                   /* drawer open */

        if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
            UieDrawerComp( UIE_ENABLE );  
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */

    return( ITM_SUCCESS );                                          
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalNFMItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate non-finalize multi check total data
==========================================================================*/

SHORT   ItemTotalNFMItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    SHORT           sSize;
    USHORT          usSize;
    DCURRENCY       lSaveMI;    /* R3.1 */
	REGDISPMSG      DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                 /* major class */
    ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;      /* minor class */
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;          /* print journal */

    memcpy( &ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], sizeof(ItemTotal->aszNumber) );
    
    ItemTotal->lMI += TranItemizersPtr->lMI; 

    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    sSize = ItemCommonCheckSize( ItemTotal, usSize );           /* storage size */
    ItemPreviousItem( ItemTotal, sSize );                       /* buffering */

    if ( ITM_TTL_FLAG_PRINT_VALIDATION( ItemTotal) ) {          /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;              /* print validation */ 
        TrnThrough( ItemTotal );
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_1;                   /* save for redisplay */
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr (UifRegTotal->uchMinorClass));

    DispMsg.lAmount = ItemTotal->lMI + ItemTransLocalPtr->lWorkMI;       /* get transaction open local data for display amount */
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;
        if ( RflRound( &lAmount, DispMsg.lAmount, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            DispMsg.lAmount = lAmount;
        }
    }

    flDispRegDescrControl |= TOTAL_CNTRL;   

    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                /* GST exempt */
        || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {          /* PST exempt */

        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                /* tax exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* indicate tax exempt to operator */
    }
                                                                /* R3.0 */
    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {       /* Display by Prnt Priority */
		MLDTRANSDATA    WorkMLD;

        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL );   /* R3.1 */
    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);
    
    lSaveMI = ItemTotal->lMI;
    ItemTotal->lMI += ItemTransLocalPtr->lWorkMI;
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;
        if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal->lMI = lAmount;
        }
    }
    
    TrnThroughDisp( ItemTotal );      /* send total to KDS R3.1 */
    ItemSendKds(ItemTotal, 0);        /* send to enhanced kds, 2172 */

    ItemTotal->lMI = lSaveMI;

    return( ITM_SUCCESS );
}

/****** End of Source ******/
