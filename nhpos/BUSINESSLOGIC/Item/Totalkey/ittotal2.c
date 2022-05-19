/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TOTAL KEY MODULE                                 
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application 
:   Program Name   : ITTOTAL2.C (check total/multi check payment total)                                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTotalCT()           ;   check total/multi check payment
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
:  2/14/95 : 03.00.00 :   hkato     : R3.0
: 11/10/95 : 03.01.00 :   hkato     : R3.1
: 96/06/24 : 03.01.09 :   M.Ozawa   : send total data to KDS directly
: 98/08/06 : 03.03.00 :   hrkato    : V3.3
: 99/08/13 : 03.05.00 :   M.teraki  : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
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


#define     USNOTCASHIER    1    

/*==========================================================================
**    Synopsis: SHORT   ItemTotalCT( UIFREGTOTAL *UifRegTotal )
*
*    Input:     UIFREGTOTAL *UifRegTotal          
*   Output:     none     
*   InOut:      none
*
*   Return:     
*
*   Description:    check total/multi check payment
==========================================================================*/
    
SHORT   ItemTotalCT(UIFREGTOTAL *UifRegTotal)
{
    if (ItemTransLocalPtr->uchCheckCounter == 0) {                   /* normal check total */
        /*----- Split Key -----*/
        if (TranCurQualPtr->uchSplit != 0) {
            if (TranCurQualPtr->uchSeat == 0) {
				/*----- Split Key -> Total -----*/
                return(ItemTotalSplitTotal(UifRegTotal));
            } else {
				/*----- Split Key -> Seat# -> Total -----*/
                return(ItemTotalSplTotal(UifRegTotal));     /* Check total for split */
            }
        } else {
			/*----- No Split Key -----*/
            return(ItemTotalCTotal(UifRegTotal));
        }
    } else {                                                /* multi check payment */
        return(ItemTotalCTM(UifRegTotal));
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalCTotal( UIFREGTOTAL *UifRegTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal      
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check total
==========================================================================*/

SHORT   ItemTotalCTotal( UIFREGTOTAL *UifRegTotal )
{
    SHORT           sStatus;
    ITEMTOTAL       ItemTotal = {0};
    ITEMTENDERLOCAL *pWorkTend = ItemTenderGetLocalPointer();

    if ( ( sStatus = ItemTotalCTNCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                      /* error */                
    }

    /* automatic charge tips, V3.3 */
    ItemTotalAutoChargeTips();
    ItemTotalPrev();                                            /* for MI calculation */

    if (pWorkTend->ItemFSTender.uchMajorClass != 0) {             /* FS Tender */
		ITEMAFFECTION   Affect = {0};

        ItemTotalCTFSTax(UifRegTotal, &ItemTotal, &Affect);     /* Saratoga */
    } else {
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        if ((sStatus = ItemTotalCTTax(UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    ItemTotalCTAmount( &ItemTotal );                                    /* calculate check total amount */
    if ( ( sStatus = ItemTotalCTItem( UifRegTotal, &ItemTotal ) ) != ITM_SUCCESS) {
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

    pWorkTend->ItemTotal = ItemTotal;
    if ((pWorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0) {
        memset(&pWorkTend->ItemFSTotal, 0, sizeof(ITEMTOTAL));
    }
    pWorkTend->fbTenderStatus[1] &= ~TENDER_FS_TOTAL;

    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */

    return( ITM_SUCCESS );
}

/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalCTNCheck( VOID )   
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     LDT_SEQERR_ADR
*               LDT_NOOFPRSN_ADR
*
*   Description:    check modifier, condition
==========================================================================*/

SHORT   ItemTotalCTNCheck( VOID )
{
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                  /* V3.3 */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            if ( ItemModLocalPtr->fsTaxable & ~MOD_USEXEMPT ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {       /* V3.3 */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            if ( ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        }
    } else {
        if (ItemModLocalPtr->fsTaxable) {
            return(LDT_SEQERR_ADR);
        }
    }
    /* --- Allow Tender at NewCheck,    V3.3 FVT --- */
    if (CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT1)) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
            return(LDT_SEQERR_ADR);
        }
    } else {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
            || (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
            return(LDT_SEQERR_ADR);
        }
    }                                                           

    if ( CliParaMDCCheck( MDC_PSN_ADR, EVEN_MDC_BIT1 )          /* No person at total key */
        && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER
        && ItemModLocalPtr->usCurrentPerson == 0 ) { 
        return( LDT_NOOFPRSN_ADR );                             /* Enter No. of Person */
    }

    return( ITM_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTotalCTFSTax(UIFREGTOTAL *pUifTotal, 
*                                   ITEMTOTAL *pItem, ITEMAFFECTION *pTax)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal 
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none 
*   Return:     
*
*   Description:    calculate tax                               Saratoga
*=========================================================================
*/
VOID    ItemTotalCTFSTax(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem, ITEMAFFECTION *pTax)
{
    SHORT           i;
    TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();
    ITEMMODLOCAL    * const pWorkMod = ItemModGetLocalPtr();

    pItem->auchTotalStatus[0] = ItemTotalTypeIndex (pUifTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (pItem->auchTotalStatus, pUifTotal->uchMinorClass);

    if (pWorkGCF->usGuestNo == 0) {
        ItemTotalCalTax(pTax, 0);                       /* after FS Tender */
    } else {
        ItemTotalCalTax(pTax, 1);                       /* after FS Tender */
    }

    if (pWorkMod->fsTaxable & MOD_USEXEMPT) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
        for (i = 0; i < 3; i++) {
            pTax->USCanVAT.USCanTax.lTaxExempt[i] = pTax->USCanVAT.USCanTax.lTaxable[i];
            pTax->USCanVAT.USCanTax.lTaxAmount[i] = pTax->USCanVAT.USCanTax.lTaxable[i] = 0L;
        }
		pWorkMod->fsTaxable &= ~MOD_USEXEMPT;
    } else {
        for (i = 0; i < 3; i++) {
            pItem->lMI += pTax->USCanVAT.USCanTax.lTaxAmount[i];
        }
        pWorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTotalCTTax(UIFREGTOTAL *UifRegTotal, 
*                   ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *WorkVAT)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal 
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none 
*   Return:     
*
*   Description:    calculate tax                   R3.1    V3.3
*=========================================================================
*/
SHORT   ItemTotalCTTax(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal,
                            ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *WorkVAT)
{
    SHORT           sStatus;
    DCURRENCY       lTax;
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
			
	// We now allow Tenders that arent assigned a total to have the
	// minor class assigned to 0, so that we don't follow
	// Total Key #2 (Check Total) control code regarding Total Key Type
	if(UifRegTotal->uchMinorClass)
	{
		ItemTotal->auchTotalStatus[0] = UifRegTotal->uchMinorClass;                    
		ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, CLASS_TOTAL2);
	}
                                                                    /* R3.1 */
    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {                        /* Saratoga */
        pWorkMod->fsTaxable |= MOD_USEXEMPT;
    }

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            WorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;               /* reset USEXEMPT bit */
            if ( pWorkMod->fsTaxable & MOD_USEXEMPT ) {               /* tax exempt case */
                WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
                return( sStatus );
            }

            pWorkMod->fsTaxable &= ~MOD_USEXEMPT;                     /* exempt state off */        
        } else {
			//SR 279 JHHJ
			//This if statement now allows the user to decide whether or not to recalculate
			//tax when two checks are added together.
			if((TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK))
			{
				if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1)
					|| ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK)) // we are in multiple check mode.)//one cent tax
				{
					if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
					return( sStatus );
					}
				}
			}else
			{

				if ((sStatus = ItemCurTax(&ItemTotal->auchTotalStatus[1], WorkTax)) != ITM_SUCCESS) {
					return( sStatus );
				}
			}
        }

    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            WorkGCF->fsGCFStatus &= ~( GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT );
            if ( pWorkMod->fsTaxable & MOD_GSTEXEMPT ) {              /* GST exempt state */        
                WorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;           /* GST exempt transaction */
            }
            if ( pWorkMod->fsTaxable & MOD_PSTEXEMPT ) {              /* PST exempt state */        
                WorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;           /* PST exempt transaction */
            }
            pWorkMod->fsTaxable &= ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT );/* exempt state off */        
        }
        ItemCanTax(&ItemTotal->auchTotalStatus[1], WorkTax, ITM_NOT_SPLIT);
    } else {
        if ((sStatus = ItemCurVAT(&ItemTotal->auchTotalStatus[1], WorkVAT, ITM_NOT_SPLIT)) != ITM_SUCCESS) {
            return(sStatus);
        }       
    }
    
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        ItemTotal->lMI += WorkVAT->lPayment;
        ItemTotal->lTax += WorkVAT->lPayment;
    } else {
        lTax = WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3];
        ItemTotal->lMI += lTax;                                         /* MI */
        ItemTotal->lTax += lTax;                                        /* all tax */
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalCTAmount( ITEMTOTAL *ItemTotal )   
*
*   Input:      ITEMTOTAL   *ItemTotal       
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:     none
*
*   Description:    calculate check total amount
==========================================================================*/

VOID    ItemTotalCTAmount( ITEMTOTAL *ItemTotal )
{
	UIFREGTENDER    UifTender = {0};
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
                        
	TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
//    if ( pWorkCur->fsCurStatus & CURQUAL_TRAY ) {                     /* tray transaction */
        ItemTendCalAmount(&ItemTotal->lMI, &UifTender);
//    } else {                                                        /* normal total */
//		ITEMTENDERLOCAL WorkTend;
//		ItemTenderGetLocal(&WorkTend);
//        ItemTotal->lMI += WorkTI->lMI - WorkTend.lTenderizer
//            - WorkTend.lFSTenderizer + WorkTend.lFSChange + WorkTend.lFS;
//    }
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;

        if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal->lMI = lAmount;
        }
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalCTItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data
==========================================================================*/

SHORT   ItemTotalCTItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    UCHAR           uchSystem = 0;
    SHORT           sSize;
    USHORT          usSize;
	REGDISPMSG      DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                 /* major class */
    ItemTotal->uchMinorClass = CLASS_TOTAL2;                    /* minor class */

    memcpy( ItemTotal->aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal->aszNumber) );
              
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                  /* print journal */

    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    sSize = ItemCommonCheckSize( ItemTotal, usSize );           /* storage size */

    ItemPreviousItem( ItemTotal, sSize );                       /* buffering */

    if ( ItemTotal->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_VALIDATION) {      /* validation print */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;                  /* print validation */ 
        TrnThrough( ItemTotal );
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;

    DispMsg.lAmount = ItemTotal->lMI + ItemTransLocalPtr->lWorkMI;       /* display amount */

    RflGetTranMnem( DispMsg.aszMnemonic, TRN_TTL2_ADR );

    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = 1;                                  /* save for redisplay */

    /* check system */
    if (ItemCommonTaxSystem() == ITM_TAX_US) {
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
            uchSystem = USNOTCASHIER;
        }
    }
    if ( (TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT)              /* GST(US) exempt */
        || (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT)             /* US exempt */
        || (TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT) ) {        /* PST exempt */

        if (uchSystem != USNOTCASHIER) {
            flDispRegKeepControl &= ~TAXEXMPT_CNTRL;            /* tax exempt */
        }
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
    }

    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) ||     /* Display by Prnt Priority */
        (MldGetMldStatus() == FALSE)) {                         /* 2x20 scroll, V3.3 */
		MLDTRANSDATA    WorkMLD = {0};

        if (!CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
            && (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
            && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) { /* R3.1 */

            if (TranCurQualPtr->uchSplit != 0) {
                MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);
            }
        }
        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL );

    if ( (uchSystem == USNOTCASHIER) && (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) ) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    TrnThroughDisp( ItemTotal );                                /* send total to KDS R3.1 */
    
    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    return( ITM_SUCCESS );
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalCTM( UIFREGTOTAL *UifRegTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    multi check payment
==========================================================================*/

SHORT   ItemTotalCTM( UIFREGTOTAL *UifRegTotal )
{
    SHORT           sStatus;
    ITEMTOTAL       ItemTotal = {0};
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};

    if ( ( sStatus = ItemTotalCTMCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                              /* error */                
    } 
                                                    
    /* automatic charge tips, V3.3 */
    ItemTotalAutoChargeTips();

    ItemTotalPrev();

    if ((sStatus = ItemTotalCTTax(UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
        return( sStatus );                                              /* error */                
    }
                                                                    
    if ( ( sStatus = ItemTotalCTMItem( UifRegTotal, &ItemTotal ) ) != ITM_SUCCESS) {
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
**  Synopsis:   SHORT   ItemTotalCTMCheck( VOID )   
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    check multi check payment condition,          R3.1
==========================================================================*/

SHORT   ItemTotalCTMCheck(VOID)
{
    if (ItemCommonTaxSystem() == ITM_TAX_US ||                  /* V3.3 */
        ItemCommonTaxSystem() == ITM_TAX_INTL) {
        if ( ItemModLocalPtr->fsTaxable ) {
            return( LDT_SEQERR_ADR );                           /* sequence error */                 
        }

    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT ) {
            return( LDT_SEQERR_ADR );                           /* sequence error */                 
        }
    }

    if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER 
        || ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_NEWCHECK 
        || ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_REORDER ) {
               
        return( LDT_SEQERR_ADR );                               /* sequence error */
    }                                                           
/* --- Split & Multi Check, V3.3 --- 
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        return(LDT_SEQERR_ADR);
    }
*/
    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalCTMItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal )   
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate multi check total data
==========================================================================*/

SHORT   ItemTotalCTMItem( UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal )
{
    UCHAR           uchSystem = 0;
    SHORT           sSize;
    USHORT          usSize;
    DCURRENCY       lSaveMI;    /* R3.1 */
	REGDISPMSG      DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;                 /* major class */
    ItemTotal->uchMinorClass = CLASS_TOTAL2;                    /* minor class */

    memcpy( ItemTotal->aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal->aszNumber) );
              
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                  /* print journal */

    ItemTotal->lMI += TranItemizersPtr->lMI; 

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
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;

        if ( RflRound( &lAmount, DispMsg.lAmount, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            DispMsg.lAmount = lAmount;
        }
    }
            
    RflGetTranMnem( DispMsg.aszMnemonic, TRN_TTL2_ADR );

    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = 1;                                  /* save for redisplay */

    /* check system */
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
            uchSystem = USNOTCASHIER;
        }
    }
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT        /* GST exempt */
        || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {  /* PST exempt */

        if (uchSystem != USNOTCASHIER) {
            flDispRegKeepControl &= ~TAXEXMPT_CNTRL;            /* tax exempt */
        }
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
    }

    DispWrite( &DispMsg );                                      /* display check total */

    /*----- Display Total to LCD,  R3.0 -----*/
    MldDispSubTotal(MLD_TOTAL_1, DispMsg.lAmount);
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {       /* Display by Prnt Priority */
		MLDTRANSDATA    WorkMLD = {0};

        MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
    }

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL ); 

    if ( (uchSystem == USNOTCASHIER) && (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) ) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);    /* R3.1 */

    lSaveMI = ItemTotal->lMI;
    ItemTotal->lMI += ItemTransLocalPtr->lWorkMI;
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;

        if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal->lMI = lAmount;
        }
    }
    
    TrnThroughDisp( ItemTotal );     /* send total to KDS R3.1 */
    ItemSendKds(ItemTotal, 0);       /* send to enhanced kds, 2172 */

    ItemTotal->lMI = lSaveMI;

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSplTotal(UIFREGTOTAL *UifRegTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    Check Total for Split Check.               R3.1
==========================================================================*/

SHORT   ItemTotalSplTotal(UIFREGTOTAL *pTotal)
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};
    ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};

    if ((sStatus = ItemTotalSplCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* automatic charge tips, V3.3 */
    ItemTotalAutoChargeTips();

    ItemTotalPrevSpl();

    /*----- Seat/Type 2 Tender -----*/
    ItemTotalSplTax(pTotal, &ItemTotal, &WorkTax, &WorkVAT);
    
    /* --- Netherland rounding, 2172 --- */
    if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
		DCURRENCY       lAmount;

        if ( RflRound( &lAmount, ItemTotal.lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
            ItemTotal.lMI = lAmount;
        }
    }
    
    ItemTotalSplItem(pTotal, &ItemTotal);

    if (TTL_STAT_FLAG_DRAWER (ItemTotal.auchTotalStatus)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    ItemCommonPutStatus(COMMON_VOID_EC);

    return(ITM_SUCCESS);
}

/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalSplCheck(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*

*   Description:    Check Modifier, Condition
==========================================================================*/

SHORT   ItemTotalSplCheck(VOID)
{
    if (ItemModLocalPtr->fsTaxable) {
        return(LDT_SEQERR_ADR);
    }

    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
        || (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) { 
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSplTax( UIFREGTOTAL *UifRegTotal, 
*           ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal 
*   Output:     ITEMTOTAL   *ItemTotal 
*   InOut:      none 
*
*   Return:     
*
*   Description:    calculate tax for splitted transaction      R3.1
=========================================================================*/

SHORT   ItemTotalSplTax(UIFREGTOTAL *UifRegTotal,
            ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
{
    DCURRENCY       lTax;
//    TRANCURQUAL     WorkCur;
    TRANITEMIZERS   *WorkTI;

    ItemTotal->auchTotalStatus[0] = ItemTotalTypeIndex(UifRegTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, UifRegTotal->uchMinorClass);

/**** V3.3 ICI
    TrnGetCurQual(&WorkCur);
    if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        /----- Not Use "Not Consoli Print" Option -----/
        ItemTotal->auchTotalStatus[4] &= ~0x20;
    }
****/
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax(&ItemTotal->auchTotalStatus[1], WorkTax, ITM_SPLIT);
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        ItemCurTaxSpl(&ItemTotal->auchTotalStatus[1], WorkTax);
    } else {
        ItemCurVAT(&ItemTotal->auchTotalStatus[1], pVAT, ITM_SPLIT);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        lTax = pVAT->lPayment;
    } else {
        lTax = WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3];
    }

    ItemTotal->lMI += lTax;
    ItemTotal->lTax += lTax;

    TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
    ItemTotal->lMI += WorkTI->lMI;

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSplItem(UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data for split
==========================================================================*/

SHORT   ItemTotalSplItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
	REGDISPMSG      DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    if (UifRegTotal->uchMinorClass == CLASS_UITOTAL2) {
        ItemTotal->uchMinorClass = CLASS_TOTAL2;
    } else {
        ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;
    }
    memcpy( &ItemTotal->aszNumber[0], &UifRegTotal->aszNumber[0], sizeof(ItemTotal->aszNumber) ); /* 5/24/96 */

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    ItemPreviousItemSpl(ItemTotal, TRN_TYPE_SPLITA);

    if (ITM_TTL_FLAG_PRINT_VALIDATION (ItemTotal)) {
        ItemTotal->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTotal);
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.lAmount = ItemTotal->lMI;
    RflGetTranMnem(DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifRegTotal->uchMinorClass));
    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = 1;

    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    DispWrite(&DispMsg);

    /*----- Display Total to LCD Middle -----*/
    MldDispSubTotal(MLD_TOTAL_2, DispMsg.lAmount);

    /*----- Display by Print Priority -----*/
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {
		TRANINFSPLIT    *WorkSpl = TrnGetInfSpl(TRN_TYPE_SPLITA);
		MLDTRANSDATA    WorkMLD = {0};

        WorkMLD.usVliSize = WorkSpl->usTranConsStoVli;
        WorkMLD.sFileHandle = WorkSpl->hsTranConsStorage;
        WorkMLD.sIndexHandle = WorkSpl->hsTranConsIndex;
        MldPutSplitTransToScroll(MLD_SCROLL_2, &WorkMLD);
    }

    flDispRegDescrControl &= ~(TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL);

    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);
    
    TrnThroughDisp( ItemTotal );                                /* send total to KDS R3.1 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSplitTotal(UIFREGTOTAL *UifRegTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal      
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    check total
==========================================================================*/

SHORT   ItemTotalSplitTotal(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
	ITEMTOTAL       ItemTotal = {0};
    UIFREGTENDER    UifTender = {0};

    if ((sStatus = ItemTotalSplCheck()) != ITM_SUCCESS) {
        return(sStatus);
    }

    /* automatic charge tips, V3.3 */
    if (ItemTotalAutoChargeTips() == ITM_SUCCESS) {
        ItemTotalPrev();                                            /* for MI calculation */
    }

    /*----- Get Total Status 6/4/96 -----*/
    ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex(UifRegTotal->uchMinorClass);
	ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, UifRegTotal->uchMinorClass);

/**** V3.3 ICI
    if ((WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        /----- Not Use "Not Consoli Print" Option -----/
        ItemTotal.auchTotalStatus[4] &= ~0x20;
    }
***/
    /*----- Save Total Status to CurQual 6/10/96 -----*/
    TranCurQualTotalStatusPut (ItemTotal.auchTotalStatus);

    /*----- Split Tender -----*/
    if ((sStatus = ItemTendCalAmountSplit(&ItemTotal.lMI, &UifTender)) != ITM_SUCCESS) {
        return(sStatus);
    }

    ItemTotalSplitItem(UifRegTotal, &ItemTotal);

    if (TTL_STAT_FLAG_DRAWER(ItemTotal.auchTotalStatus)) {
        ItemDrawer();
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT1) == 0) {
            UieDrawerComp(UIE_ENABLE);
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    ItemCommonPutStatus(COMMON_VOID_EC);

    return(ITM_SUCCESS);
}


/*==========================================================================
**  Synopsis:   VOID    ItemTotalSplitItem(UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal)
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data for split
==========================================================================*/

VOID    ItemTotalSplitItem(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal)
{
	REGDISPMSG      DispMsg = {0};

    ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
    if (UifRegTotal->uchMinorClass == CLASS_UITOTAL2) {
        ItemTotal->uchMinorClass = CLASS_TOTAL2;
    } else {
        ItemTotal->uchMinorClass = UifRegTotal->uchMinorClass;
    }

    ItemTotalLocal.uchSaveMajor = CLASS_ITEMTOTAL;
    ItemTotalLocal.uchSaveMinor = ItemTotal->uchMinorClass;

    memcpy( &ItemTotalLocal.aszNumber[0], &UifRegTotal->aszNumber[0], sizeof(ItemTotalLocal.aszNumber) ); /* 5/24/96 */
    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

	if (ITM_TTL_FLAG_PRINT_VALIDATION(ItemTotal)) {
        ItemTotal->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(ItemTotal);
    }

    DispMsg.uchMajorClass = UifRegTotal->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTotal->uchMinorClass;
    DispMsg.lAmount = ItemTotal->lMI;
    RflGetTranMnem(DispMsg.aszMnemonic, RflChkTtlStandardAdr(UifRegTotal->uchMinorClass));
    flDispRegDescrControl |= TOTAL_CNTRL;   
    DispMsg.fbSaveControl = 1;

    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    DispWrite(&DispMsg);

    flDispRegDescrControl &= ~(TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL);

    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
    }

    TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);

    TrnThroughDisp( ItemTotal );                                /* send total to KDS R3.1 */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);
}

/*----- End of Source -----*/
