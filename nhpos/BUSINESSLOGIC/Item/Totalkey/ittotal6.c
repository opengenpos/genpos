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
:   Program Name   : ITTOTAL6.C (tray total)                                       
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTotalTT()           ;   tray total
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name      : Description
:   2/14/95: 03.00.00 :   hkato     : R3.0
:  11/10/95: 03.01.00 :   hkato     : R3.1
:   8/17/98: 03.03.00 :  hrkato     : V3.3 (VAT/Service)
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
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"


/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalTTCheck( VOID )   
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    check modifier, condition
==========================================================================*/

static SHORT   ItemTotalTTCheck( VOID )
{
    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            if ( ItemModLocalPtr->fsTaxable & ~MOD_USEXEMPT ) {
                return( LDT_SEQERR_ADR );                           /* sequence error */                 
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                           /* sequence error */                 
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            if ( ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT ) {
                return( LDT_SEQERR_ADR );                           /* sequence error */                 
            }
        } else {
            if ( ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                           /* sequence error */                 
            }
        }
    } else {
       if (ItemModLocalPtr->fsTaxable) {
           return(LDT_SEQERR_ADR);
       }
    }

	// Prior to Dec 10, 2015 use of tray total required Precheck Unbuffered System type
	// however now we are testing the use of Tray Total with other types of systems.
	//    Richard Chambers
	switch (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) {
		case CURQUAL_PRE_UNBUFFER:
		case CURQUAL_POSTCHECK:
			// allow use of Tray Total with Post Guest Check as well as Pre Guest Check Unbuffered
			break;
		default:
			return( LDT_SEQERR_ADR );
	}

    if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
		// do not allow Tray Total with New Check, Reorder, or Add Check. 
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }                                                                       

    if ( ! ( ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE ) ) {   /* start itemization */
        return( LDT_SEQERR_ADR );                                     /* sequence error */
    }                                                                       
                                                                            
    if ( CliParaMDCCheckField ( MDC_PSN_ADR, MDC_PARAM_BIT_C )  /* Compulsory Number of person before total key */
        && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER
        && ItemModLocalPtr->usCurrentPerson == 0 ) { 
        return( LDT_NOOFPRSN_ADR );                             /* Must Enter No. of Person */
    }

    return( ITM_SUCCESS );
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalTTTax(ITEMTOTAL *ItemTotal, 
*           ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
*
*   Input:      ITEMTOTAL *ItemTotal,
*               ITEMCOMMONTAX   *WorkTax )
*   Output:     ITEMTOTAL   *ItemTotal, ITEMCOMMONTAX   *WorkTax 
*   InOut:      none 
*
*   Return:     
*
*   Description:    calculate tray total tax
=========================================================================*/

static SHORT   ItemTotalTTTax(ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
{
    SHORT        sStatus;
    DCURRENCY    lTax = 0;

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                      /* V3.3 */
		ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
		TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
		TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();

		pWorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;                   /* reset USEXEMPT bit */
        if ( pWorkMod->fsTaxable & MOD_USEXEMPT ) {                   /* tax exempt case */
            pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
        }
        if (pWorkCur->fsCurStatus & CURQUAL_WIC) {            /* Saratoga */
            pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
        }
        if ( ( sStatus = ItemCurTax( &ItemTotal->auchTotalStatus[1], WorkTax ) ) != ITM_SUCCESS ) {
            return( sStatus );
        }

        pWorkMod->fsTaxable &= ~MOD_USEXEMPT;                         /* exempt state off */        
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
		ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
		TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
		TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();

        pWorkGCF->fsGCFStatus &= ~( GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT );
        if ( pWorkMod->fsTaxable & MOD_GSTEXEMPT ) {                  /* GST exempt state */        
            pWorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;               /* GST exempt transaction */
        }

        if ( pWorkMod->fsTaxable & MOD_PSTEXEMPT ) {                  /* PST exempt state */        
            pWorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;               /* PST exempt transaction */
        }

        ItemCanTax( &ItemTotal->auchTotalStatus[1], WorkTax, ITM_NOT_SPLIT );
		if(WorkTax->alTaxable[1] > 1000)
		{
			NHPOS_ASSERT(!"ItemTotalTTTax TAX ERROR");
		}
                                                                
        pWorkMod->fsTaxable &= ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT );    /* exempt state off */        
    } else {
        if ((sStatus = ItemCurVAT( &ItemTotal->auchTotalStatus[1], pVAT, ITM_NOT_SPLIT)) != ITM_SUCCESS) {
            return(sStatus);
        }       
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        lTax = pVAT->lPayment;
    } else {
        lTax = WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3];
    }

    ItemTotal->lMI += lTax;                                         /* MI */
    ItemTotal->lTax += lTax;                                        /* all tax */

    return( ITM_SUCCESS );
}



/*==========================================================================
**  Synopsis:   VOID    ItemTotalTTTaxSave(ITEMCOMMONTAX *WorkTax,
*       ITEMAFFECTION *ItemAffectTax, ITEMAFFECTION *ItemPrintTax, ITEMCOMMONVAT *pVAT)
*                               
*   Input:      ITEMCOMMONTAX   *WorkTax,
*                   ITEMAFFECTION   *ItemAffectTax, ITEMAFFECTION   *ItemPrintTax          
*   Output:     ITEMCOMMONTAX   *WorkTax,
*                   ITEMAFFECTION   *ItemAffectTax, ITEMAFFECTION   *ItemPrintTax          
*   InOut:      none
*   Return:     
*                   
*   Description:    generate tax affection data
==========================================================================*/

VOID    ItemTotalTTTaxSave(CONST ITEMCOMMONTAX *WorkTax, ITEMAFFECTION *ItemAffectTax,
                        ITEMAFFECTION *ItemPrintTax, CONST ITEMCOMMONVAT *pVAT)
{
    ItemAffectTax->uchMajorClass = CLASS_ITEMAFFECTION;             /* major class */
    ItemAffectTax->uchMinorClass = CLASS_TAXAFFECT;                 /* minor class */
                                                        
    ItemPrintTax->uchMajorClass = CLASS_ITEMAFFECTION;              /* major class */
    ItemPrintTax->uchMinorClass = CLASS_TAXPRINT;                   /* minor class */

    ItemPrintTax->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
		SHORT   i;

        for( i = 0; i < 5; i++ ) {                                      /* taxable itemizer */
            ItemAffectTax->USCanVAT.USCanTax.lTaxable[i]
                = ItemPrintTax->USCanVAT.USCanTax.lTaxable[i] = WorkTax->alTaxable[i];
        }
        for( i = 0; i < 4; i++ ) {                                      /* tax amount */
            ItemAffectTax->USCanVAT.USCanTax.lTaxAmount[i]
                = ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[i] = WorkTax->alTax[i];
            ItemAffectTax->USCanVAT.USCanTax.lTaxExempt[i]
                = ItemPrintTax->USCanVAT.USCanTax.lTaxExempt[i] = WorkTax->alTaxExempt[i];
        }
    } else {                                    /* Int'l VAT,   V3.3 */
		SHORT   i;

        for( i = 0; i < 3; i++ ) {                                      /* taxable itemizer */
			ItemAffectTax->USCanVAT.ItemVAT[i] = pVAT->ItemVAT[i];
		}
        ItemAffectTax->lAmount = pVAT->lService;
    }

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
            ItemPrintTax->USCanVAT.USCanTax.lTaxable[1]
                += ItemPrintTax->USCanVAT.USCanTax.lTaxable[2] + ItemPrintTax->USCanVAT.USCanTax.lTaxable[3];
            ItemPrintTax->USCanVAT.USCanTax.lTaxable[2] = ItemPrintTax->USCanVAT.USCanTax.lTaxable[3] = 0L;
            ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[1]
                += ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[2] + ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[3];
            ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[2] = ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L; 

        } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
            && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {  
            ItemPrintTax->USCanVAT.USCanTax.lTaxable[0] += ItemPrintTax->USCanVAT.USCanTax.lTaxable[1]
                + ItemPrintTax->USCanVAT.USCanTax.lTaxable[2] + ItemPrintTax->USCanVAT.USCanTax.lTaxable[3];
            ItemPrintTax->USCanVAT.USCanTax.lTaxable[1] = ItemPrintTax->USCanVAT.USCanTax.lTaxable[2]
                = ItemPrintTax->USCanVAT.USCanTax.lTaxable[3] = 0L;
            ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[0] += ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[1]
                + ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[2] + ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[3]; 
            ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[1] = ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[2] 
                = ItemPrintTax->USCanVAT.USCanTax.lTaxAmount[3] = 0L; 
        }
    }
}



/*==========================================================================
**  Synopsis:   VOID    ItemTotalTTAmount( ITEMTOTAL *ItemTotal )   
*
*   Input:      ITEMTOTAL   *ItemTotal       
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:     none
*
*   Description:    calculate tray total amount, save tray total/counter
==========================================================================*/

VOID    ItemTotalTTAmount( ITEMTOTAL *ItemTotal )
{
    DCURRENCY    lTax = 0;
    
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                /* V3.3 */
        lTax = ItemTotalLocal.lTrayVatPayment;
    } else {
		SHORT   i;

        for ( i = 0; i < 4; i++ ) {
            lTax += ItemTotalLocal.lTrayTax[i];
        }
    }

    ItemTotal->lMI += TranItemizersPtr->lMI - ItemTotalLocal.lTrayTotal + lTax;
    ItemTotal->sTrayCo += TranItemizersPtr->sItemCounter - ItemTotalLocal.sTrayCo;
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTotalTTItem( UIFREGTOTAL *UifRegTotal,
*                           ITEMTOTAL *ItemTotal,   
*                           ITEMAFFECTION *ItemAffectTax,
*                           ITEMAFFECTION *ItemPrintTax )
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*                   ITEMAFFECTION *ItemAffectTax, ITEMAFFECTION *ItemPrintTax
*   Output:     UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal      
*                   ITEMAFFECTION *ItemAffectTax, ITEMAFFECTION *ItemPrintTax
*   InOut:      none
*
*   Return:     
*
*   Description:    generate tray total data
==========================================================================*/

static SHORT   ItemTotalTTItem (ITEMTOTAL *ItemTotal,
            ITEMAFFECTION *ItemAffectTax, ITEMAFFECTION *ItemPrintTax )
{
    SHORT           sStatus;
    USHORT          usSize;
    REGDISPMSG      DispMsg;
                                                            
    usSize = ItemCommonLocalPtr->usSalesBuffSize + ItemCommonLocalPtr->usDiscBuffSize;
    
    if ( ( sStatus = ItemCommonCheckSize( ItemTotal, usSize ) ) < 0 ) {    /* storage full */
		TRANGCFQUAL *WorkGCF = TrnGetGCFQualPtr();
        WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;                 /* storage status */
        return( LDT_TAKETL_ADR);                                
    }

    usSize += ( USHORT )sStatus;

    if ( ( sStatus = ItemCommonCheckSize( ItemAffectTax, usSize ) ) < 0 ) {
		TRANGCFQUAL *WorkGCF = TrnGetGCFQualPtr();
        WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;                 /* storage status */
        return( LDT_TAKETL_ADR);                                
    }
    usSize += ( USHORT )sStatus;
    
    if ( ( sStatus = ItemCommonCheckSize( ItemPrintTax, usSize ) ) < 0 ) {
		TRANGCFQUAL *WorkGCF = TrnGetGCFQualPtr();
        WorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;                 /* storage status */
        return( LDT_TAKETL_ADR);                                
    }
                                                            
    if ( ITM_TTL_FLAG_PRINT_VALIDATION(ItemTotal) ) {          /* validation print? */
        ItemTotal->fsPrintStatus = PRT_VALIDATION;             /* print validation */ 
        TrnThrough( ItemTotal );
    }

    ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                      /* print journal */
                                                    
    memset( &DispMsg, 0, sizeof( REGDISPMSG ) );                    /* initialize "item" */
    DispMsg.uchMajorClass = CLASS_UIFREGTOTAL;
    DispMsg.uchMinorClass = ItemTotal->uchMinorClass;
    RflGetTranMnem (DispMsg.aszMnemonic, RflChkTtlStandardAdr(ItemTotal->uchMinorClass) );
    DispMsg.lAmount = ItemTotal->lMI;                               /* display amount */
    DispMsg.fbSaveControl = 1;                                      /* save for redisplay */

    flDispRegDescrControl |= TOTAL_CNTRL;   
    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                    /* GST exempt */
        || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {              /* PST exempt */

        flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                    /* tax exempt */
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt */
    }

    DispWrite( &DispMsg );                                          /* display check total */

    /*----- Display Total to LCD,  R3.0,    Saratoga -----*/
    ItemCommonDispSubTotal(0);     /* display total by MI, Saratoga */

    flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL ); 

    return( ITM_SUCCESS );
}
 

/*==========================================================================
**  Synopsis:   VOID    ItemTotalTTTrans( ITEMTOTAL *ItemTotal,
*                               ITEMAFFECTION   *ItemAffectTax,
*                               ITEMAFFECTION   *ItemPrintTax )
*                               
*   Input:      ITEMTOTAL *ItemTotal,   ITEMAFFECTION   *ItemAffectTax, 
*                   ITEMAFFECTION   *ItemPrintTax          
*   Output:     ITEMTOTAL *ItemTotal,   ITEMAFFECTION   *ItemAffectTax, 
*                   ITEMAFFECTION   *ItemPrintTax          
*   InOut:      none
*
*   Return:     
*                   
*   Description:    transaction module i/F
==========================================================================*/

static VOID    ItemTotalTTTrans( ITEMTOTAL *ItemTotal, ITEMAFFECTION *ItemAffectTax, 
            ITEMAFFECTION *ItemPrintTax )
{
    if ( ITM_TTL_FLAG_TAX_1(ItemTotal) ||               /* calculate tax 1? */
		 ITM_TTL_FLAG_TAX_2(ItemTotal) ||            /* calculate tax 2? */
		 ITM_TTL_FLAG_TAX_3(ItemTotal) ||            /* calculate tax 3? R3.0 */
		 CliParaMDCCheckField( MDC_TAX_ADR, MDC_PARAM_BIT_D ) ) {    /* Canadian or VAT tax system? */

        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
            ItemTendVATCommon(ItemAffectTax, ITM_NOT_SPLIT, ITM_DISPLAY);
        } else {
            TrnItem( ItemAffectTax );                                   /* transaction module i/F */
            TrnItem( ItemPrintTax );                                    /* transaction module i/F */
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(ItemAffectTax, 0);

            /*----- Display Total to LCD,  R3.0 -----*/
            ItemPrintTax->uchMinorClass = CLASS_TAXPRINT;
            MldScrollWrite(ItemPrintTax, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(ItemPrintTax);
        }
    }

    TrnItem( ItemTotal );                                           /* transaction module i/F */

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);
    
    /*----- Display Total to LCD,  R3.0 -----*/
    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalTTSave(ITEMTOTAL *ItemTotal,
*                           ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
*               
*   Input:      ITEMTOTAL *ItemTotal,   ITEMCOMMONTAX   *WorkTax          
*   output:     ITEMTOTAL *ItemTotal,   ITEMCOMMONTAX   *WorkTax          
*   InOut:      none
*
*   Return:     
*                   
*   Description:    save taxable/tax, tray amount
==========================================================================*/

VOID    ItemTotalTTSave(ITEMTOTAL *ItemTotal,
                    ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *pVAT)
{
    SHORT   i;
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

    if (ItemCommonTaxSystem() != ITM_TAX_INTL) {                /* V3.3 */
        for( i = 0; i < 5; i++ ) {                              /* taxable itemizer   R3.0 */
            ItemTotalLocal.lTrayTaxable[i] += WorkTax->alTaxable[i];
        }
        for( i = 0; i < 4; i++ ) {                              /* tax amount R3.0*/
            ItemTotalLocal.lTrayTax[i] += WorkTax->alTax[i];
        }
        for( i = 0; i < 4; i++ ) {                                  /* tax exempt amount R3.0 */                                        
            ItemTotalLocal.lTrayTaxExempt[i] += WorkTax->alTaxExempt[i];
        }
    } else {
        for (i = 0; i < NUM_VAT; i++) {
            if (pVAT->ItemVAT[i].lVATRate) {
                ItemTotalLocal.TrayItemVAT[i].lVATRate  = pVAT->ItemVAT[i].lVATRate;
            }
            ItemTotalLocal.TrayItemVAT[i].lVATable += pVAT->ItemVAT[i].lVATable;
            ItemTotalLocal.TrayItemVAT[i].lVATAmt  += pVAT->ItemVAT[i].lVATAmt;
            ItemTotalLocal.TrayItemVAT[i].lSum     += pVAT->ItemVAT[i].lSum;
            ItemTotalLocal.TrayItemVAT[i].lAppAmt  += pVAT->ItemVAT[i].lAppAmt;
        }
        ItemTotalLocal.lTrayVatService += pVAT->lService;
        ItemTotalLocal.lTrayVatPayment += pVAT->lPayment;
    }

    ItemTotalLocal.lTrayTotal += ItemTotal->lMI;                    /* tray total */
    ItemTotalLocal.sTrayCo += ItemTotal->sTrayCo;                   /* tray counter */
                                                
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        memset( &WorkTI->Itemizers.TranCanadaItemizers, 0, sizeof( TRANCANADAITEMIZERS ) );
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {              /* V3.3 */
        memset( &WorkTI->Itemizers.TranUsItemizers, 0, sizeof( TRANUSITEMIZERS ) );
    } else {
        memset(&WorkTI->Itemizers.TranIntlItemizers, 0, sizeof(TRANINTLITEMIZERS));
    }

    for ( i = 0; i < STD_DISC_ITEMIZERS_MAX; i++ ) {                                     
        WorkTI->lDiscountable[i] = 0L;                               /* discountable itemizer */
    }

	WorkGCF->fsGCFStatus &= ~(GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2);
	WorkGCF->fsGCFStatus2 &= ~(GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

    ItemTotalLocal.uchSaveMajor = ItemTotal->uchMajorClass;         /* save major class */
    ItemTotalLocal.uchSaveMinor = ItemTotal->uchMinorClass;         /* save minor class */

	TranCurQualTotalStatusClear();
}



/*==========================================================================
**    Synopsis: SHORT   ItemTotalTT( UIFREGTOTAL *UifRegTotal )
*
*    Input:     UIFREGTOTAL *UifRegTotal          
*   Output:     none     
*   InOut:      none
*
*   Return:     
*
*   Description:    tray total
==========================================================================*/

SHORT   ItemTotalTT( UIFREGTOTAL *UifRegTotal )
{
    SHORT           sStatus;
    ITEMTOTAL       ItemTotal = {0};
    ITEMAFFECTION   ItemAffectTax = {0}, ItemPrintTax = {0};
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
    ITEMCOMMONLOCAL *pWorkCommon = ItemCommonGetLocalPointer();
       
    if ( ( sStatus = ItemTotalTTCheck() ) != ITM_SUCCESS ) {
        return( sStatus );                                              /* error */                
    }
    
    if ( pWorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL ) {
        memset( &pWorkCommon->ItemSales, 0, sizeof( ITEMSALES ) );
    } else {
        ItemPreviousItem(0, 0);           /* Saratoga */
    }

    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;                     /* major class */
    ItemTotal.uchMinorClass = UifRegTotal->uchMinorClass;          /* minor class */
    memcpy( ItemTotal.aszNumber, UifRegTotal->aszNumber, sizeof(ItemTotal.aszNumber) );
    ItemTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;              /* print journal */
	ItemTotal.usTrayNo = ItemTotalLocal.usNoPerson + 1;            /* remember the tray number for this tray */
                                        
	ItemTotal.auchTotalStatus[0] = ItemTotalTypeIndex (UifRegTotal->uchMinorClass);   /* total key type, R3.1 */
    ItemTotalAuchTotalStatus (ItemTotal.auchTotalStatus, UifRegTotal->uchMinorClass);

    if ((sStatus = ItemTotalTTTax (&ItemTotal, &WorkTax, &WorkVAT)) != ITM_SUCCESS) {
        return(sStatus);
    }
    ItemTotalTTAmount( &ItemTotal );                                    /* calculate check total amount */
    ItemTotalTTTaxSave(&WorkTax, &ItemAffectTax, &ItemPrintTax, &WorkVAT);
    if ( ( sStatus = ItemTotalTTItem (&ItemTotal, &ItemAffectTax, &ItemPrintTax ) ) != ITM_SUCCESS) {
        return( sStatus );                                              /* storage full */                
    }

    /* no error condition */
    pWorkCur->fsCurStatus |= CURQUAL_TRAY;                              /* tray total transaction */

    if ( ITM_TTL_FLAG_DRAWER(&ItemTotal) ) {
        ItemDrawer();                                                   /* drawer open */

        if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
            UieDrawerComp( UIE_ENABLE );  
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }

    ItemTotalTTTrans( &ItemTotal, &ItemAffectTax, &ItemPrintTax );      /* transaction module i/F */

    ItemTrailer( TYPE_TRAY );                                           /* trailer print */
                                                        
    if ( ITM_TTL_FLAG_TOTAL_TYPE(&ItemTotal) == PRT_TRAY2 ) { 
        ItemTotal.lService = ItemTotal.lMI;
        ItemTotalStub( &ItemTotal );                                    /* stub receipt */
    }
                                                            
    ItemTotalTTSave(&ItemTotal, &WorkTax, &WorkVAT);

    ItemCommonPutStatus( COMMON_VOID_EC );                              /* E/C disable */

    pWorkSales->fbSalesStatus &= ~SALES_ITEMIZE;

    ItemTotalLocal.usNoPerson++;                                        /* tray counter (for No of person) */
	ItemTotalLocal.usUniqueID = ItemSalesGetUniqueId();                 /* remember last sales item unique id for this tray total */
                                    
    return( ITM_SUCCESS );
}

/****** End of Source ******/
