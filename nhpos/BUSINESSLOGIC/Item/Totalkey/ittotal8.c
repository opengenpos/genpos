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
:   Program Name   : ITTOTAL3.C (service total)                                       
:  ---------------------------------------------------------------------  
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
:   Memory Model       : Medium Model                                     
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemTotalCI()           ;   cashier interrupt total
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:    Date  : Ver.Rev. :   Name    	: Description
: 06/04/98 : 03.03.00 :  M.Ozawa    : Initial
: 08/19/98 : 03.03.00 :  hrkato     : V3.3 (VAT/Service)
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
#include    <transact.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <csttl.h>
#include    <display.h>
#include    <mld.h>
#include    <item.h>
#include    "itmlocal.h"

            

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSE( UIFREGTOTAL *UifRegTotal )  
*
*   Input:      none       
*   Output:     none    
*   InOut:      UIFREGTOTAL *UifRegTotal
*
*   Return:     
*
*   Description:    service total
==========================================================================*/

SHORT   ItemTotalCI(UIFREGTOTAL *UifRegTotal)
{
    SHORT           sStatus;
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();
    TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();

    if ( ! ItemSalesLocalSaleStarted() ) {           /* itemize state */
        /* if finalized, only sign-out */
        return(ITM_SUCCESS);
    }

    if ((sStatus = ItemTotalCICheck()) != ITM_SUCCESS) {
        return(sStatus);
    }
 
    if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
        /* check GCF is full */
        if ( ( sStatus = CliGusManAssignNo( pWorkGCF->usGuestNo ) ) != GCF_SUCCESS ) {
            return (SHORT)(GusConvertError( sStatus ));
        }
		pWorkCur->fsCurStatus |= CURQUAL_NEWCHECK;                    /* newcheck operation */
    } else if ((pWorkCur->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
		/* ---- for print control 07/06/98 ---- */
		pWorkCur->fsCurStatus &= ~CURQUAL_OPEMASK;
		pWorkCur->fsCurStatus |= (CURQUAL_REORDER | CURQUAL_ADDCHECK_SERVT);
    }

    if (pWorkCur->fsCurStatus & CURQUAL_PVOID ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_PVOID;                      /* p-void GCF */
    }

    if (pWorkCur->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN) ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;                    /* transaction return */
    }

    if (pWorkCur->fsCurStatus & CURQUAL_TRAINING ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_TRAINING;                   /* training transaction */
    }

    pWorkGCF->fsGCFStatus |= GCFQUAL_NEWCHEK_CASHIER;                /* newcheck is cashier */

    /*----- PreCheck/Buffering, PreCheck/Unbuffering System -----*/
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER) {
        return(ItemTotalSEPreCheck(UifRegTotal));
    } else if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
        return(LDT_PROHBT_ADR);
    } else if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
        /*----- PostCheck System -----*/
        if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
            return(ItemTotalSEPostCheck(UifRegTotal));
        } else {
            /*----- SoftCheck System -----*/
            return(ItemTotalSESoftCheck(UifRegTotal));
        }
    } else { 
		/*----- Store/Recall System -----*/
        return(LDT_PROHBT_ADR);
    }
}

/*==========================================================================        
**  Synopsis:   SHORT   ItemTotalCICheck(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    check modifier, condition for cashier interrupt
==========================================================================*/

SHORT   ItemTotalCICheck(VOID)
{
    TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();
                         
    if ( (TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) == 0 ) {
        return (LDT_SEQERR_ADR);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                  /* V3.3 */
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK) {

            if (ItemModLocalPtr->fsTaxable & ~MOD_USEXEMPT ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
            if (ItemModLocalPtr->fsTaxable & MOD_USEXEMPT ) {           /* tax exempt case */
				pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
            if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {            /* Saratoga */
				pWorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
            }
        } else {
            if (ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK) {
            if (ItemModLocalPtr->fsTaxable & ~( MOD_GSTEXEMPT | MOD_PSTEXEMPT ) ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
            if (ItemModLocalPtr->fsTaxable & MOD_GSTEXEMPT ) {          /* GST exempt case */
				pWorkGCF->fsGCFStatus |= GCFQUAL_GSTEXEMPT;
            }
            if (ItemModLocalPtr->fsTaxable & MOD_PSTEXEMPT ) {          /* PST exempt case */
				pWorkGCF->fsGCFStatus |= GCFQUAL_PSTEXEMPT;
            }
        } else {
            if (ItemModLocalPtr->fsTaxable ) {
                return( LDT_SEQERR_ADR );                       /* sequence error */                 
            }
        }
    } else {                                        /* Int'l VAT,   V3.3 */
        if (ItemModLocalPtr->fsTaxable) {
            return(LDT_SEQERR_ADR);
        }
    }

    return(ITM_SUCCESS);
}

/****** End of Source ******/
