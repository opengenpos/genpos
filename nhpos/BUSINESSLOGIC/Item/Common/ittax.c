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
:   Title          : COMMON MODULE
:   Category       : COMMON MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTAX.C (US tax calculation)
:  ---------------------------------------------------------------------  
:  Abstract:
:   ItemCurTax()                ;   calculate current tax (us)
:   ItemCurTaxSpl()             ;   calculate current tax, split check (us)
:
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:    Date  : Ver.Rev. :   Name      : Description
: 92/6/17  : 00.00.01 :   hkato     : initial
: 95/11/21 : 03.01.00 :   hkato     : R3.1
: 98/08/21 : 03.03.00 :   hrkato    : V3.3
: 98/10/21 : 03.03.00 :   M.Suzuki  : TAR 89859
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <stdlib.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <uie.h>
#include    <regstrct.h>
#include    <transact.h>
#include	<trans.h>
#include    <item.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    "itmlocal.h"



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTax( UCHAR   *puchTotal, ITEMCOMMONTAX *WorkTax );
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     taxable, tax amount
*
*   Description:    calculate US tax
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

SHORT   ItemCurTax( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
{
    SHORT   sStatus;

    ItemCurTaxWork( puchTotal, WorkTax );                           /* generate work itemizer */
                                                                    
    if ( ( sStatus = ItemCurTaxCal( puchTotal, WorkTax ) ) != ITM_SUCCESS ) {  
        return( sStatus );
    }
                                                                    
    ItemCurTaxCor( puchTotal, WorkTax );                            /* current tax correction */
    
    return( ITM_SUCCESS );
}    



/*==========================================================================
**   Synopsis:  VOID    ItemCurTaxWork( UCHAR   *puchTotal, ITEMCOMMONTAX *WorkTax );
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate work itemizer
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

VOID    ItemCurTaxWork( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
{
    CONST TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();
               
    if ( *puchTotal & 0x01 ) {                                      /* calculate tax 1 */
        if ( *puchTotal & 0x02 ) {                                  /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }

        if ( *puchTotal & 0x04 ) {                                  /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
    
        if ( *puchTotal & 0x08 ) {                                  /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[0];
        } else {
            WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[0];
        }
        WorkTax->alTaxable[0] += WorkTrans->alCorrectTaxable[0];
    }

    if ( *puchTotal & 0x10 ) {                                      /* calculate tax 2 */
        if ( *puchTotal & 0x20 ) {                                  /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }

        if ( *puchTotal & 0x40 ) {                                  /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
    
        if ( *puchTotal & 0x80 ) {                                  /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[1];
        } else {
            WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[1];
        }
        WorkTax->alTaxable[1] += WorkTrans->alCorrectTaxable[1];
    }

    if ( *( puchTotal + 1 ) & 0x01 ) {                              /* calculate tax 3 */
        if ( *( puchTotal + 1 ) & 0x02 ) {                          /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }

        if ( *( puchTotal + 1 ) & 0x04 ) {                          /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
    
        if ( *( puchTotal + 1 ) & 0x08 ) {                          /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }                              

        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[2];
        } else {
            WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[2];
        }
        WorkTax->alTaxable[2] += WorkTrans->alCorrectTaxable[2];
    }
}    



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTaxCal( UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax );
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    calculate tax 1-3
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

SHORT   ItemCurTaxCal( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
{
    SHORT       sStatus;

    if ( *puchTotal & 0x01 ) {                                      /* calculate tax 1 */
        if ( ( sStatus = ItemCurTax1( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }

    if ( *puchTotal & 0x10 ) {                                      /* calculate tax 2 */
        if ( ( sStatus = ItemCurTax2( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }
                                                                    
    if ( *( puchTotal + 1 ) & 0x01 ) {                              /* calculate tax 3 */
        if ( ( sStatus = ItemCurTax3( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }

    return( ITM_SUCCESS );
}    



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTax1( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    calculate tax 1
==========================================================================*/

SHORT   ItemCurTax1( ITEMCOMMONTAX *WorkTax )
{           
    SHORT   sStatus;
    PARATAXRATETBL  WorkRate;
                    
    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;               
    WorkRate.uchAddress = TXRT_NO1_ADR;                             /* Tax#1 */
    CliParaRead( &WorkRate );

    if ( WorkRate.ulTaxRate != 0L ) {                               /* Tax1 Rate */
        RflRateCalc3( &WorkTax->alTax[0], WorkTax->alTaxable[0], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {                                                        /* tax table */
        if ( ( sStatus = RflTax1( &WorkTax->alTax[0], WorkTax->alTaxable[0], RFL_USTAX_1 ) ) != RFL_SUCCESS ) {
            return( sStatus );
        }
    }

    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTax2( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    calculate tax 2
==========================================================================*/

SHORT   ItemCurTax2( ITEMCOMMONTAX *WorkTax )
{           
    SHORT   sStatus;
    PARATAXRATETBL  WorkRate;
                    
    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;               
    WorkRate.uchAddress = TXRT_NO2_ADR;                             /* Tax#2 */
    CliParaRead( &WorkRate );

    if ( WorkRate.ulTaxRate != 0L ) {                               /* Tax2 Rate */
        RflRateCalc3( &WorkTax->alTax[1], WorkTax->alTaxable[1], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {                                                        /* tax table */
        if ( ( sStatus = RflTax1( &WorkTax->alTax[1], WorkTax->alTaxable[1], RFL_USTAX_2 ) ) != RFL_SUCCESS ) {
            return( sStatus );
        }
    }

    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTax3( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    calculate tax 3
==========================================================================*/

SHORT   ItemCurTax3( ITEMCOMMONTAX *WorkTax )
{           
    SHORT   sStatus;
    PARATAXRATETBL  WorkRate;
                    
    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;               
    WorkRate.uchAddress = TXRT_NO3_ADR;                             /* Tax#3 */
    CliParaRead( &WorkRate );

    if ( WorkRate.ulTaxRate != 0L ) {                               /* Tax3 Rate */
        RflRateCalc3( &WorkTax->alTax[2], WorkTax->alTaxable[2], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {                                                        /* tax table */
        if ( ( sStatus = RflTax1( &WorkTax->alTax[2], WorkTax->alTaxable[2], RFL_USTAX_3 ) ) != RFL_SUCCESS ) {
            return( sStatus );
        }
    }

    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTaxCor( UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax );
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    current tax correction
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

VOID    ItemCurTaxCor( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
{
    CONST TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();

    if ( *puchTotal & 0x01 ) {                                      /* calculate tax 1 */
        WorkTax->alTax[0] -= WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[0];
        WorkTax->alTax[0] -= WorkTrans->alCorrectTax[0];
    
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[0] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[0];
        } else {
            WorkTax->alTaxable[0] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[0];
        }
        WorkTax->alTaxable[0] -= WorkTrans->alCorrectTaxable[0];
    }
    
    if ( *puchTotal & 0x10 ) {                                      /* calculate tax 2 */
        WorkTax->alTax[1] -= WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[1];
        WorkTax->alTax[1] -= WorkTrans->alCorrectTax[1];
    
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[1] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[1];
        } else {
            WorkTax->alTaxable[1] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[1];
        }
        WorkTax->alTaxable[1] -= WorkTrans->alCorrectTaxable[1];
    }

    if ( *( puchTotal + 1 ) & 0x01 ) {                              /* calculate tax 3 */
        WorkTax->alTax[2] -= WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[2];
        WorkTax->alTax[2] -= WorkTrans->alCorrectTax[2];

        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[2] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[2];
        } else {
            WorkTax->alTaxable[2] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[2];
        }
        WorkTax->alTaxable[2] -= WorkTrans->alCorrectTaxable[2];
    }
}


/*==========================================================================
**   Synopsis:  SHORT   ItemCurTaxSpl(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax);
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax
*   InOut:      none
*
*   Return:     taxable, tax amount
*
*   Description:    calculate US tax for Splitted Check,       R3.1
==========================================================================*/

SHORT   ItemCurTaxSpl(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
{
    SHORT   sStatus;

    ItemCurTaxWorkSpl(puchTotal, WorkTax);
    if ((sStatus = ItemCurTaxCal(puchTotal, WorkTax)) != ITM_SUCCESS) {
        return(sStatus);
    }
                                                                    
    return(ITM_SUCCESS);
}    

/*==========================================================================
**   Synopsis:  VOID    ItemCurTaxWorkSpl(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax
*   InOut:      none
*
*   Return:     
*
*   Description:    generate work itemizer
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

VOID    ItemCurTaxWorkSpl(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
{
    CONST TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
               
    if (*puchTotal & 0x01) {          // ITM_TTL_FLAG_TAX_1()
        if (*puchTotal & 0x02) {      // ITM_TTL_FLAG_TAX_1_1()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if (*puchTotal & 0x04) {      // ITM_TTL_FLAG_TAX_1_2()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if (*puchTotal & 0x08) {      // ITM_TTL_FLAG_TAX_1_3()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[0] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
    }
    if (*puchTotal & 0x10) {         // ITM_TTL_FLAG_TAX_2()
        if (*puchTotal & 0x20) {      // ITM_TTL_FLAG_TAX_2_1()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if (*puchTotal & 0x40) {      // ITM_TTL_FLAG_TAX_2_2()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if (*puchTotal & 0x80) {      // ITM_TTL_FLAG_TAX_2_3()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[1] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
    }
    if (*(puchTotal + 1) & 0x01) {      // ITM_TTL_FLAG_TAX_3()
        if (*(puchTotal + 1) & 0x02) {      // ITM_TTL_FLAG_TAX_3_1()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if (*(puchTotal + 1) & 0x04) {      // ITM_TTL_FLAG_TAX_3_2()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if (*(puchTotal + 1) & 0x08) {      // ITM_TTL_FLAG_TAX_3_3()
            if (WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT) {
                WorkTax->alTaxExempt[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[2] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }                              
    }
}    



/*==========================================================================
**   Synopsis:  SHORT   ItemCanTax(UCHAR *puchTotal,
*                           ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     taxable, tax, tax exempt amount
*
*   Description:    calculate Canadian tax (simple tax/tax on tax)
==========================================================================*/

VOID    ItemCanTax(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax, SHORT sType)
{       
	CANTAXWORK  CanTaxWork = {0};
    CANTAX      CanTax = {0};
    USHORT      fsLimitable = TRUE, fsBaked = FALSE;

    ItemCanTaxWork1( &CanTaxWork, &CanTax, WorkTax, sType, &fsLimitable, &fsBaked );    /* generate 9 work itemizers */
    ItemCanTaxDisc( &CanTax, sType, fsLimitable, fsBaked );
    if ( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT1 ) ) {       /* tax on tax system */
        ItemCanTaxWork2( puchTotal, &CanTax, sType, fsLimitable, fsBaked );           /* add tax on tax itemizers */
    }
    ItemCanTaxWork3( &CanTax, WorkTax, sType );                 /* generate 5 itemizers */
    ItemCanTaxWork4( WorkTax, sType );                          /* add affect itemizers */

	ItemCanTaxSpecBakedCorr(WorkTax);

    ItemCanTaxCal( puchTotal, WorkTax );                        /* tax calculation */
    ItemCanTaxCor( puchTotal, WorkTax, sType );                 /* tax correction */

	if(WorkTax->alTaxable[1] > 10000)
	{
		NHPOS_DEBUG_DUMP(WorkTax, ITEMCOMMONTAX, sizeof(ITEMCOMMONTAX));
		NHPOS_DEBUG_DUMP(&TrnInformation, TRANINFORMATION, sizeof(TRANINFORMATION));
	}

    WorkTax->fsLimitable = fsLimitable;
    WorkTax->fsBaked = fsBaked;
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxWork1( CANTAXWORK *CanTaxWork, CANTAX *CanTax,
*                           ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      CANTAXWORK *CanTaxWork, CANTAX *CanTax, ITEMCOMMONTAX *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate 9 itemizer(simple tax/ tax on tax)
==========================================================================*/

VOID    ItemCanTaxWork1( CANTAXWORK *CanTaxWork,
            CANTAX *CanTax, ITEMCOMMONTAX *WorkTax, SHORT sType, USHORT *pfsLimitable, USHORT *pfsBaked )
{
    SHORT           i, sPigRuleCo;
    PARAPIGRULE     ParaPigRule;
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    CONST TRANITEMIZERS   *WorkTI;
                    
    if (sType == ITM_SPLIT) {                          /* R3.1 */
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    for ( i = 0; i < STD_PLU_ITEMIZERS; i++ ) {
        CanTaxWork->lTaxable[i] += WorkTI->Itemizers.TranCanadaItemizers.lTaxable[i];
    }

    /* CanTaxWork( 9 itemizers ) -> CanTax ( 9 itemizers ) */
    CanTax->lGSTItemizer += CanTaxWork->lTaxable[0];                /* meal */
    CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[0];          /* meal */
    CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[0];           /* for tax on tax */

    /* check pig rule counter */
    ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;
    ParaPigRule.uchAddress = PIG_CO_LIMIT_ADR;
    CliParaRead( &ParaPigRule );

    if (sType == ITM_SPLIT) {                                       /* R3.1 */
        sPigRuleCo = 0;

    } else {
        sPigRuleCo = WorkGCF->sPigRuleCo;
    }
    
    if ( WorkTrans->sPigRuleCo ) {
        sPigRuleCo += WorkTrans->sPigRuleCo;
    }
    /* for discount correction */
    if ( ( ! ParaPigRule.ulPigRule ) || ( ( SHORT )ParaPigRule.ulPigRule > abs( sPigRuleCo ) ) ) {
        *pfsBaked = FALSE;
	} else {
        *pfsBaked = TRUE;
	}
    if ( ( ! ParaPigRule.ulPigRule ) || ( ( SHORT )ParaPigRule.ulPigRule > abs( sPigRuleCo ) ) ) {
        CanTax->lGSTItemizer += CanTaxWork->lTaxable[8];            /* baked item */
        CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[8];      /* baked item */
        CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[8];       /* for tax on tax */
//begin CT/OT tax enhancements JHHJ
		//first we will check to see if there is any data in the special baked good taxable area
		if(CanTaxWork->lTaxable[9])
		{
			if(CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT3))
			{	//we are using the OT method, NO GST is charged 
				CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[9];      /* baked item */
			}else
			{
				//we are using the CT method, GST is charged.
				CanTax->lGSTItemizer += CanTaxWork->lTaxable[9];       /* for tax on tax */
			}
		}
//end CT/OT tax enhancements JHHJ
        if (sType != ITM_SPLIT) {                                   /* 10-21-98 */
            if (( ! ParaPigRule.ulPigRule ) || (( SHORT )ParaPigRule.ulPigRule <= abs( WorkTrans->sPrePigRuleCo ))) {
                CanTax->lGSTItemizer += WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable;       /* baked item */
                CanTax->lPST1LimitItemizer += WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable; /* baked item */
                CanTax->lGSTPST1lItemizer += WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable;  /* for tax on tax */
                WorkTax->alTaxExempt[0] = -(WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable);    /* GST exempt amount */
            }
        }
    } else {
        WorkTax->alTaxExempt[0] = CanTaxWork->lTaxable[8];          /* GST exempt amount */
        if (( ! ParaPigRule.ulPigRule ) || (( SHORT )ParaPigRule.ulPigRule > abs( WorkTrans->sPrePigRuleCo ))) { /* 10-21-98*/
            CanTax->lGSTItemizer -= WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable;       /* baked item */
            CanTax->lPST1LimitItemizer -= WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable; /* baked item */
            CanTax->lGSTPST1lItemizer -= WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable;  /* for tax on tax */
            WorkTax->alTaxExempt[0] += WorkTI->Itemizers.TranCanadaItemizers.lPigRuleTaxable;    /* GST exempt amount */
        }
    }
    ItemTotalLocal.lPigTaxExempt = WorkTax->alTaxExempt[0];         /* 10-21-98 */

    CanTax->lGSTItemizer += CanTaxWork->lTaxable[1];                /* carbonated beverage */
    if ( CanTaxWork->lTaxable[0] == 0L                              /* meal */
        && CanTaxWork->lTaxable[2] == 0L                            /* snack */
        && CanTaxWork->lTaxable[3] == 0L                            /* beer/wine */
        && CanTaxWork->lTaxable[4] == 0L                            /* liquor */
        && CanTaxWork->lTaxable[5] == 0L                            /* grocery */
        && CanTaxWork->lTaxable[6] == 0L                            /* tip */
        && CanTaxWork->lTaxable[7] == 0L                            /* PST1 only */
        && CanTaxWork->lTaxable[8] == 0L ) {                        /* baked */

        if ( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT2 )  ) {      /* not limit */
            CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[1];   /* carbonated beverage */
            CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[1];  /* for tax on tax */
            *pfsLimitable = FALSE;

        } else {                                                    /* limit */            
            CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[1];  /* carbonated beverage */
            CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[1];   /* for tax on tax */
            *pfsLimitable = TRUE;
        }
#if 1
	/* Carbonated beverage with grocery or snack item should be affected to GST and PST, 10/26/01 */
	} else
    if ( (CanTaxWork->lTaxable[0] == 0L                              /* meal */
        && CanTaxWork->lTaxable[7] == 0L                             /* PST1 only */
        && CanTaxWork->lTaxable[8] == 0L)                           /* baked */
        && (CanTaxWork->lTaxable[2] != 0L                            /* snack */
          || CanTaxWork->lTaxable[3] != 0L                            /* beer/wine */
          || CanTaxWork->lTaxable[4] != 0L                            /* liquor */
          || CanTaxWork->lTaxable[5] != 0L                            /* grocery */
          || CanTaxWork->lTaxable[6] != 0L) ) {                            /* tip */

        if (( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT2 )  ) ||      /* not limit */
        	( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 )  )) {      /* not limit */
            CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[1];   /* carbonated beverage */
            CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[1];  /* for tax on tax */
            *pfsLimitable = FALSE;

        } else {                                                    /* limit */            
            CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[1];  /* carbonated beverage */
            CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[1];   /* for tax on tax */
            *pfsLimitable = TRUE;
        }

	} else
	/* baked item which is sold more than 6 is considered as grocery item, 11/07/01 */
    if ( CanTaxWork->lTaxable[0] == 0L                              /* meal */
        && CanTaxWork->lTaxable[2] == 0L                            /* snack */
        && CanTaxWork->lTaxable[3] == 0L                            /* beer/wine */
        && CanTaxWork->lTaxable[4] == 0L                            /* liquor */
        && CanTaxWork->lTaxable[5] == 0L                            /* grocery */
        && CanTaxWork->lTaxable[6] == 0L                            /* tip */
        && CanTaxWork->lTaxable[7] == 0L                            /* PST1 only */
        && CanTaxWork->lTaxable[8] != 0L ) {                        /* baked */

	    if ( ( ! ParaPigRule.ulPigRule ) || ( ( SHORT )ParaPigRule.ulPigRule > abs( sPigRuleCo ) ) ) {

			/* assume as meal item */
	        if ( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 )  ) {      /* not limit */
	            CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[1];   /* carbonated beverage */
    	        CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[1];  /* for tax on tax */
            	*pfsLimitable = FALSE;

	        } else {                                                    /* limit */            
    	        CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[1];  /* carbonated beverage */
        	    CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[1];   /* for tax on tax */
	            *pfsLimitable = TRUE;
	        }
	        
	    } else {

			/* assume as grocery item */
	        if (( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT2 )  ) ||      /* not limit */
    	    	( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 )  )) {      /* not limit */
        	    CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[1];   /* carbonated beverage */
            	CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[1];  /* for tax on tax */
            	*pfsLimitable = FALSE;

	        } else {                                                    /* limit */            
    	        CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[1];  /* carbonated beverage */
        	    CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[1];   /* for tax on tax */
	            *pfsLimitable = TRUE;
	        }
	    }                    
#endif

    } else {                                                        /* carbon & other sales */
        if ( CliParaMDCCheck( MDC_TAX_ADR, ODD_MDC_BIT3 )  ) {      /* not limit */
            CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[1];   /* carbonated beverage */
            CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[1];  /* for tax on tax */
            *pfsLimitable = FALSE;

        } else {                                                    /* limit */            
            CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[1];  /* carbonated beverage */
            CanTax->lGSTPST1lItemizer += CanTaxWork->lTaxable[1];   /* for tax on tax */
            *pfsLimitable = TRUE;
        }
    }                    

    CanTax->lGSTItemizer += CanTaxWork->lTaxable[2];                /* snack */
    CanTax->lPST1NonLimitItemizer += CanTaxWork->lTaxable[2];       /* snack */
    CanTax->lGSTPST1nlItemizer += CanTaxWork->lTaxable[2];          /* for tax on tax */

    CanTax->lGSTItemizer += CanTaxWork->lTaxable[3];                /* beer/wine */
    CanTax->lPST2Itemizer += CanTaxWork->lTaxable[3];               /* beer/wine */
    CanTax->lGSTPST2Itemizer += CanTaxWork->lTaxable[3];            /* for tax on tax */

    CanTax->lGSTItemizer += CanTaxWork->lTaxable[4];                /* liquor */
    CanTax->lPST3Itemizer += CanTaxWork->lTaxable[4];               /* liquor */
    CanTax->lGSTPST3Itemizer += CanTaxWork->lTaxable[4];            /* for tax on tax */

    WorkTax->alTaxable[4] = CanTaxWork->lTaxable[5];                /* grocery */

    CanTax->lGSTItemizer += CanTaxWork->lTaxable[6];                /* tip */
    
    CanTax->lPST1LimitItemizer += CanTaxWork->lTaxable[7];          /* PST1 only */
}    



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxDisc(CANTAX *CanTax, SHORT sType)
*
*   Input:      CANTAX  *CanTax
*   Output:     CANTAX  *CanTax
*   InOut:      none
*
*   Return:     
*
*   Description:    discountable/tax itemizer(simple tax/ tax on tax)
==========================================================================*/

VOID    ItemCanTaxDisc(CANTAX *CanTax, SHORT sType, USHORT fsLimitable, USHORT fsBaked)
{
    CONST TRANITEMIZERS   *WorkTI;

    if (sType == ITM_SPLIT) {
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    CanTax->lGSTItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[0];               // TRANCANADA_MEAL?
    CanTax->lPST1LimitItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[1];         // TRANCANADA_CARB_BEV?
    if (fsLimitable == TRUE) {	/* 02/15/01 */
	    CanTax->lPST1LimitItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[9];     //  - should the index 9 be replaced by TRANCANADA_LIMITABLE?
	} else {
	    CanTax->lPST1NonLimitItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[9];  //  - should the index 9 be replaced by TRANCANADA_LIMITABLE?
	}
    CanTax->lPST1NonLimitItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[2];      // TRANCANADA_SNACK?
    CanTax->lPST2Itemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[3];              // TRANCANADA_BEER_WINE?
    CanTax->lPST3Itemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[4];              // TRANCANADA_LIQUOR?
    
    if (fsBaked == TRUE) {
	    CanTax->lGSTItemizer -= WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[11];          // should this be [TRANCANADA_GST] rather than [11]?
    	CanTax->lPST1LimitItemizer -= WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[12];    // should this be [TRANCANADA_PST1LIMIT] rather than [12]?
	}
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxWork2(UCHAR *puchTotal, CANTAX *CanTax, SHORT sType)
*
*   Input:      UCHAR *puchTotal, CANTAX *CanTax
*   Output:     CANTAX *CanTax
*   InOut:      none
*
*   Return:     
*
*   Description:    generate itemizer for tax on tax
==========================================================================*/

VOID    ItemCanTaxWork2(CONST UCHAR *puchTotal, CANTAX *CanTax, SHORT sType, USHORT fsLimitable, USHORT fsBaked)
{
    DCURRENCY       lTax;
    CONST TRANITEMIZERS   *WorkTI;
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    PARATAXRATETBL  WorkRate;
    
    if (sType == ITM_SPLIT) {
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO1_ADR;
    CliParaRead( &WorkRate );

    if ( *puchTotal & 0x01 ) {                                          /* calculate GST */
        if ( *puchTotal & 0x02 ) {                                      /* calculate PST1 */
            CanTax->lGSTPST1lItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[5];
			if (fsLimitable == TRUE) {
	            CanTax->lGSTPST1lItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[10];	/* limitable carbonated beverage - should the index 10 be replaced by TRANCANADA_LIMITABLE? */
			}
			if (fsBaked == TRUE) {
	            CanTax->lGSTPST1lItemizer  -= WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[13];	/* baked item correction - should the index 10 be replaced by TRANCANADA_PST1LIMIT? */
			}

            if ( WorkRate.ulTaxRate != 0L ) {
                RflRateCalc3( &lTax, CanTax->lGSTPST1lItemizer, WorkRate.ulTaxRate, RND_TAX1_ADR );
            } else {
                RflTax1( &lTax, CanTax->lGSTPST1lItemizer, RFL_USTAX_1 );
            }
        
            if (!(WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT)) {          /* V3.3 */
                CanTax->lPST1LimitItemizer += lTax;                     /* GST/PST1 amount */
            }
            CanTax->lGSTPST1nlItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[6];

			if (fsLimitable == FALSE) {
	            CanTax->lGSTPST1nlItemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[10];	/* non-limitable carbonated beverage - should the index 10 be replaced by TRANCANADA_LIMITABLE? */
			}
            if ( WorkRate.ulTaxRate != 0L ) {
                RflRateCalc3( &lTax, CanTax->lGSTPST1nlItemizer, WorkRate.ulTaxRate, RND_TAX1_ADR );
            } else {
                RflTax1( &lTax, CanTax->lGSTPST1nlItemizer, RFL_USTAX_1 );
            }
            if (!(WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT)) {          /* V3.3 */
                CanTax->lPST1NonLimitItemizer += lTax;                  /* GST/PST1 amount */
            }
        }

        if ( *puchTotal & 0x04 ) {                                      /* calculate PST2 */
            CanTax->lGSTPST2Itemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[7];
            if ( WorkRate.ulTaxRate != 0L ) {
                RflRateCalc3( &lTax, CanTax->lGSTPST2Itemizer, WorkRate.ulTaxRate, RND_TAX1_ADR );
            } else {
                RflTax1( &lTax, CanTax->lGSTPST2Itemizer, RFL_USTAX_1 );
            }
            CanTax->lPST2Itemizer += lTax;                              /* PST2 amount */
        }

        if ( *puchTotal & 0x08 ) {                                      /* calculate PST3 */
            CanTax->lGSTPST3Itemizer += WorkTI->Itemizers.TranCanadaItemizers.lDiscTax[8];
            if ( WorkRate.ulTaxRate != 0L ) {
                RflRateCalc3( &lTax, CanTax->lGSTPST3Itemizer, WorkRate.ulTaxRate, RND_TAX1_ADR );
            } else {
                RflTax1( &lTax, CanTax->lGSTPST3Itemizer, RFL_USTAX_1 );
            }
            CanTax->lPST3Itemizer += lTax;                              /* PST3 amount */
        }
    }
}


/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxWork3( CANTAX *CanTax,
*                           ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      CANTAX *CanTax, ITEMCOMMONTAX *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate 5 itemizer(simple tax/ tax on tax)
==========================================================================*/

VOID    ItemCanTaxWork3( CONST CANTAX *CanTax, ITEMCOMMONTAX *WorkTax, SHORT sType )
{
    SHORT           sPvoid = 1;
    CONST TRANITEMIZERS   *WorkTI;
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();

    if (sType == ITM_SPLIT) {
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    /* generate GST itemizer */
    WorkTax->alTaxable[0] = CanTax->lGSTItemizer;

    /* generate PST1 itemizer */
    if ( CanTax->lPST1LimitItemizer == 0L ) {                           /* PST limit */
        WorkTax->alTaxable[1] = CanTax->lPST1NonLimitItemizer;          /* PST1 itemizer */
    } else {                                                              
        if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT0 ) == 0 ) {  /* use exempt limit */
			PARATAXRATETBL  WorkLimit;

            WorkLimit.uchMajorClass = CLASS_PARATAXRATETBL;                
            WorkLimit.uchAddress = TXRT_NO5_ADR;                        /* exempt limit */
            CliParaRead( &WorkLimit );

            if ( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {                /* preselect void */
                sPvoid = -1;
            }
           
            WorkTax->lMealLimitTaxable += CanTax->lPST1LimitItemizer;
            WorkTax->lMealNonLimitTaxable += CanTax->lPST1NonLimitItemizer;

            if (RflLLabs( CanTax->lPST1LimitItemizer + WorkTI->Itemizers.TranCanadaItemizers.lMealLimitTaxable ) <= ( DCURRENCY )WorkLimit.ulTaxRate ) {                      /* under limit */
                WorkTax->alTaxable[1] = CanTax->lPST1NonLimitItemizer + WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable;

				NHPOS_ASSERT(CanTax->lPST1NonLimitItemizer < 10000);
				NHPOS_ASSERT(WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable < 10000);
			} else {                                                    /* over limit */
                WorkTax->alTaxable[1] += CanTax->lPST1LimitItemizer + CanTax->lPST1NonLimitItemizer
                    + WorkTI->Itemizers.TranCanadaItemizers.lMealLimitTaxable
                    + WorkTI->Itemizers.TranCanadaItemizers.lMealNonLimitTaxable;
            }
            WorkTax->alTaxable[1] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[1];
            WorkTax->alTaxable[1] -= WorkTrans->alCorrectTaxable[1];
        } else {                                                        /* not use exempt limit */
            WorkTax->alTaxable[1] = CanTax->lPST1NonLimitItemizer + CanTax->lPST1LimitItemizer;
        }
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
		// Tim Horton's of Canada was having a problem with a tax calculation error
		// which would suddenly happen and then not be seen for a while.
		// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
		//   Richard Chambers, Apr-09-2015
		NHPOS_ASSERT(WorkTax->alTaxable[1] < 100000);
    }

    /* generate PST2 itemizer */
    WorkTax->alTaxable[2] = CanTax->lPST2Itemizer;

    /* generate PST3 itemizer */
    WorkTax->alTaxable[3] = CanTax->lPST3Itemizer;
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxWork4( ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate 5 itemizer
==========================================================================*/

VOID    ItemCanTaxWork4( ITEMCOMMONTAX *WorkTax, SHORT sType )
{
    SHORT   i;
    CONST TRANITEMIZERS   *WorkTI;
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();
                    
    if (sType == ITM_SPLIT) {
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

	// There are four Canadian tax indicators (GST, PST1, PST2, PST3) in each itemizer array.
    for ( i = 0; i < STD_TAX_ITEMIZERS_MAX; i++ ) {
        WorkTax->alTaxable[i] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i];
        WorkTax->alTaxable[i] += WorkTrans->alCorrectTaxable[i];
    }
}    



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCal( UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     taxable, tax, tax exempt  
*
*   Description:    calculate Canadian tax
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

VOID    ItemCanTaxCal( CONST UCHAR *puchTotal,ITEMCOMMONTAX *WorkTax )
{
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();

    if ( *puchTotal & 0x01 ) {                                      /* calculate GST */
        if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {            /* GST exempt */
            WorkTax->alTaxExempt[0] += WorkTax->alTaxable[0];
            WorkTax->alTaxable[0] = 0L;

        } else {
            ItemCanTaxCalGST( WorkTax );                            /* calculate GST */
        }
    } else {
        WorkTax->alTaxable[0] = 0L;
    }

    if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {                /* PST exempt */
		SHORT   i;

        if ( *puchTotal & 0x02 ) {                                  /* calculate PST1 */
            WorkTax->alTaxExempt[1] += WorkTax->alTaxable[1];
        }
        if ( *puchTotal & 0x04 ) {                                  /* calculate PST2 */
            WorkTax->alTaxExempt[2] += WorkTax->alTaxable[2];       /* PST2 exempt amount */
        }
        if ( *puchTotal & 0x08 ) {                                  /* calculate PST3 */
            WorkTax->alTaxExempt[3] += WorkTax->alTaxable[3];       /* PST3 exempt amount */
        }
        for ( i = 1; i < STD_TAX_ITEMIZERS_MAX; i++ ) {
            WorkTax->alTaxable[i] = 0L;     // zero out PST1, PST2, and PST3 taxable itemizers after adding to tax exempt itemizers.
        }
    } else {
        if ( *puchTotal & 0x02 ) {                                  /* calculate PST1 */
            ItemCanTaxCalPST1( WorkTax );                           /* calculate PST1 */
        } else {
            WorkTax->alTaxable[1] = 0L;
            WorkTax->lMealLimitTaxable = 0L;
            WorkTax->lMealNonLimitTaxable = 0L;
        }

        if ( *puchTotal & 0x04 ) {                                  /* calculate PST2 */
            ItemCanTaxCalPST2( WorkTax );                           /* calculate PST2 */
        } else {
            WorkTax->alTaxable[2] = 0L;
        }

        if ( *puchTotal & 0x08 ) {                                  /* calculate PST3 */
            ItemCanTaxCalPST3( WorkTax );                           /* calculate PST3 */
        } else {
            WorkTax->alTaxable[3] = 0L;
        }
    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCalGST( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:       
*
*   Description:    calculate GST
==========================================================================*/

VOID    ItemCanTaxCalGST( ITEMCOMMONTAX *WorkTax )
{
    PARATAXRATETBL  WorkRate;

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO1_ADR;
    CliParaRead( &WorkRate );

    if ( WorkRate.ulTaxRate != 0L ) {
        RflRateCalc3( &WorkTax->alTax[0], WorkTax->alTaxable[0], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {
        RflTax1( &WorkTax->alTax[0], WorkTax->alTaxable[0], RFL_USTAX_1 );
    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCalPST1( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:       
*
*   Description:    calculate PST1
==========================================================================*/

VOID    ItemCanTaxCalPST1( ITEMCOMMONTAX *WorkTax )
{
    PARATAXRATETBL  WorkRate;

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO2_ADR;
    CliParaRead( &WorkRate );                                           

	NHPOS_ASSERT(WorkTax->alTax[1] < 10000);

    if ( WorkRate.ulTaxRate != 0L ) {                               
        RflRateCalc3( &WorkTax->alTax[1], WorkTax->alTaxable[1], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {
        RflTax1( &WorkTax->alTax[1], WorkTax->alTaxable[1], RFL_USTAX_2 );
    }
	NHPOS_ASSERT(WorkTax->alTax[1] < 10000);
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCalPST2( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax
*   InOut:      none
*
*   Return:       
*
*   Description:    calculate PST2
==========================================================================*/

VOID    ItemCanTaxCalPST2( ITEMCOMMONTAX *WorkTax )
{
    PARATAXRATETBL  WorkRate;

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO3_ADR;
    CliParaRead( &WorkRate );

    if ( WorkRate.ulTaxRate != 0L ) {                               
        RflRateCalc3( &WorkTax->alTax[2], WorkTax->alTaxable[2], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {
        RflTax1( &WorkTax->alTax[2], WorkTax->alTaxable[2], RFL_USTAX_3 );
    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCalPST3( ITEMCOMMONTAX *WorkTax );
*
*   Input:      ITEMCOMMONTAX   *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:       
*
*   Description:    calculate PST3
==========================================================================*/

VOID    ItemCanTaxCalPST3( ITEMCOMMONTAX *WorkTax )
{
    PARATAXRATETBL  WorkRate;

    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO4_ADR;
    CliParaRead( &WorkRate );                                           

    if ( WorkRate.ulTaxRate != 0L ) {                               
        RflRateCalc3( &WorkTax->alTax[3], WorkTax->alTaxable[3], WorkRate.ulTaxRate, RND_TAX1_ADR );
    } else {
        RflTax1( &WorkTax->alTax[3], WorkTax->alTaxable[3], RFL_USTAX_4 );
    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCor( UCHAR *puchTotal,
*                           ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    tax correction
*                   The total key tax indicator flags are provided in the call
*                   as a pointer to element auchTotalStatus[1] allowing the taxable
*                   status in auchTotalStatus[1] and auchTotalStatus[2] to be seen.
*                   Since the array of the second element, auchTotalStatus[1] is
*                   provided, we begin indexing for the tax flags with zero and not one.
==========================================================================*/

VOID    ItemCanTaxCor( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax, SHORT sType )
{
    CONST TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    CONST TRANITEMIZERS   *WorkTI;
    CONST ITEMTRANSLOCAL  *WorkTrans = ItemTransGetLocalPointer();       /* get transaction open local data */

    if (sType == ITM_SPLIT) {
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

	// There are four Canadian tax indicators (GST, PST1, PST2, PST3) in each itemizer array.
	// We go through checking whether this total key is provisioned for each of the different
	// Canadian tax indicators and modify the appropriate element of the itemizer array.

    if ( *puchTotal & 0x01 ) {                                      /* calculate GST */
        if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {            /* GST exempt */
            WorkTax->alTaxExempt[0] -= WorkTrans->alCorrectTaxable[0];
            WorkTax->alTaxExempt[0] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
        } else {
            WorkTax->alTaxable[0] -= WorkTrans->alCorrectTaxable[0];
            WorkTax->alTaxable[0] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
            WorkTax->alTax[0] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTax[0];
            WorkTax->alTax[0] -= WorkTrans->alCorrectTax[0];
        }
    }
    
    if ( *puchTotal & 0x02 ) {                                      /* calculate PST1 */
		if(WorkTax->alTaxable[1] >= 100000)
		{
			NHPOS_ASSERT_TEXT(0, "Tax Greater than 1000 in ItemCanTaxCor");
		}
        if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {            /* PST exempt */
            WorkTax->alTaxExempt[1] -= WorkTrans->alCorrectTaxable[1];
            WorkTax->alTaxExempt[1] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[1];
        } else {
            WorkTax->alTaxable[1] -= WorkTrans->alCorrectTaxable[1];
            WorkTax->alTaxable[1] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[1];
            WorkTax->alTax[1] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTax[1];
            WorkTax->alTax[1] -= WorkTrans->alCorrectTax[1];
        }
    }

    if ( *puchTotal & 0x04 ) {                                      /* calculate PST2 */
        if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {            /* PST exempt */
            WorkTax->alTaxExempt[2] -= WorkTrans->alCorrectTaxable[2];
            WorkTax->alTaxExempt[2] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[2];
        } else {
            WorkTax->alTaxable[2] -= WorkTrans->alCorrectTaxable[2];
            WorkTax->alTaxable[2] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[2];
            WorkTax->alTax[2] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTax[2];
            WorkTax->alTax[2] -= WorkTrans->alCorrectTax[2];
        }
    }

    if ( *puchTotal & 0x08 ) {                                      /* calculate PST3 */
        if ( WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {            /* PST exempt */
            WorkTax->alTaxExempt[3] -= WorkTrans->alCorrectTaxable[3];
            WorkTax->alTaxExempt[3] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[3];
        } else {
            WorkTax->alTaxable[3] -= WorkTrans->alCorrectTaxable[3];
            WorkTax->alTaxable[3] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[3];
            WorkTax->alTax[3] -= WorkTI->Itemizers.TranCanadaItemizers.lAffTax[3];
            WorkTax->alTax[3] -= WorkTrans->alCorrectTax[3];
        }
    }
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemUSTaxCopy1(ITEMAFFECTION *pAffect, ITEMCOMMONTAX *pTax)
*
*   Input:      ITEMAFFECTION *pAffect, ITEMCOMMONTAX *pTax
*   Output:     ITEMAFFECTION *pAffect
*   InOut:      none
*   Return:     
*
*   Description:    Copy (WorkTax ->ItemAffection)              V3.3
*==========================================================================
*/
VOID    ItemUSTaxCopy1(ITEMAFFECTION *pAffect, CONST ITEMCOMMONTAX *pTax)
{
    SHORT       i;
    USCANTAX    *pAffectTax = &pAffect->USCanVAT.USCanTax;

    for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
        pAffectTax->lTaxable[i] = pTax->alTaxable[i];
        pAffectTax->lTaxAmount[i] = pTax->alTax[i];
        pAffectTax->lTaxExempt[i] = pTax->alTaxExempt[i];
    }
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCopy1(ITEMAFFECTION *pAffect, ITEMCOMMONTAX *pTax)
*
*   Input:      ITEMAFFECTION *pAffect, ITEMCOMMONTAX *pTax
*   Output:     ITEMAFFECTION *pAffect
*   InOut:      none
*   Return:     
*
*   Description:    Copy (WorkTax ->ItemAffection)              V3.3
*==========================================================================
*/
VOID    ItemCanTaxCopy1(ITEMAFFECTION *pAffect, CONST ITEMCOMMONTAX *pTax)
{
    SHORT       i;
    USCANTAX    *pAffectTax = &pAffect->USCanVAT.USCanTax;

    for(i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
        pAffectTax->lTaxable[i] = pTax->alTaxable[i];
        pAffectTax->lTaxAmount[i] = pTax->alTax[i];
        pAffectTax->lTaxExempt[i] = pTax->alTaxExempt[i];
    }
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemCanTaxCopy2(ITEMAFFECTION *pAffect)
*
*   Input:      ITEMAFFECTION *pAffect
*   Output:     ITEMAFFECTION *pAffect
*   InOut:      none
*   Return:     
*
*   Description:    Copy (ItemAffection -> ItemAffection)       V3.3
*==========================================================================
*/
VOID    ItemCanTaxCopy2(ITEMAFFECTION *pAffect)
{
    USCANTAX    *pAffectTax = &pAffect->USCanVAT.USCanTax;

    /* --- GST/PST print option --- */
    if (CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT1)) {
        pAffectTax->lTaxable[1] += pAffectTax->lTaxable[2] + pAffectTax->lTaxable[3];
        pAffectTax->lTaxable[2] = pAffectTax->lTaxable[3] = 0L;

        pAffectTax->lTaxAmount[1] += pAffectTax->lTaxAmount[2] + pAffectTax->lTaxAmount[3];
        pAffectTax->lTaxAmount[2] = pAffectTax->lTaxAmount[3] = 0L;

    } else if (CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT1) == 0
        && CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT2) == 0) {  
        pAffectTax->lTaxable[0] += pAffectTax->lTaxable[1] + pAffectTax->lTaxable[2] + pAffectTax->lTaxable[3];
        pAffectTax->lTaxable[1] = pAffectTax->lTaxable[2] = pAffectTax->lTaxable[3] = 0L;

        pAffectTax->lTaxAmount[0] += pAffectTax->lTaxAmount[1] + pAffectTax->lTaxAmount[2] + pAffectTax->lTaxAmount[3];
        pAffectTax->lTaxAmount[1] = pAffectTax->lTaxAmount[2] = pAffectTax->lTaxAmount[3] = 0L;
    }
}

#if 0
/*
*==========================================================================
**   Synopsis:  SHORT   ItemCurTaxFS(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     taxable, tax amount
*
*   Description:    calculate US tax FS
*==========================================================================
*/
SHORT   ItemCurTaxFS(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
{
    SHORT   sStatus;

    /* --- Generate Taxable Itemizer --- */
    ItemCurTaxFSWork(puchTotal, WorkTax);
                                                                    
    if ( ( sStatus = ItemCurTaxCal( puchTotal, WorkTax ) ) != ITM_SUCCESS ) {  
        return( sStatus );
    }
                                                                    
    ItemCurTaxCor( puchTotal, WorkTax );                            /* current tax correction */
    
    return( ITM_SUCCESS );
}    

/*
*==========================================================================
**   Synopsis:  VOID    ItemCurTaxFSWork(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate work itemizer
*==========================================================================
*/
VOID    ItemCurTaxFSWork(UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax)
{
    TRANITEMIZERS   *WorkTI;
    TRANGCFQUAL     *WorkGCF;

    TrnGetTI( &WorkTI );                                            /* get transaction itemizers */
    TrnGetGCFQual( &WorkGCF );                                      /* get GCF qualifier */

    if ( *puchTotal & 0x01 ) {                                      /* calculate tax 1 */
        if ( *puchTotal & 0x02 ) {                                  /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if ( *puchTotal & 0x04 ) {                                  /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if ( *puchTotal & 0x08 ) {                                  /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[0]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[0]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[0];
        } else {
            WorkTax->alTaxable[0]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[0];
        }
    }

    if ( *puchTotal & 0x10 ) {                                      /* calculate tax 2 */
        if ( *puchTotal & 0x20 ) {                                  /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if ( *puchTotal & 0x40 ) {                                  /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if ( *puchTotal & 0x80 ) {                                  /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[1]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }
        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[1]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[1];
        } else {
            WorkTax->alTaxable[1]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[1];
        }
    }

    if ( *( puchTotal + 1 ) & 0x01 ) {                              /* calculate tax 3 */
        if ( *( puchTotal + 1 ) & 0x02 ) {                          /* use taxable 1 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            } else {
                WorkTax->alTaxable[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[0];
            }
        }
        if ( *( puchTotal + 1 ) & 0x04 ) {                          /* use taxable 2 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            } else {
                WorkTax->alTaxable[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[1];
            }
        }
        if ( *( puchTotal + 1 ) & 0x08 ) {                          /* use taxable 3 */
            if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {         /* Tax exempt */
                WorkTax->alTaxExempt[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            } else {
                WorkTax->alTaxable[2]
                    += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[2];
            }
        }                              

        if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {             /* Tax exempt */
            WorkTax->alTaxExempt[2]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[2];
        } else {
            WorkTax->alTaxable[2]
                += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[2];
        }
    }
}    



/*==========================================================================
**   Synopsis:  SHORT   ItemCurTaxCal( UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax );
*
*   Input:      UCHAR   *puchTotal, ITEMCOMMONTAX   *WorkTax 
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    calculate tax 1-3
==========================================================================*/

SHORT   ItemCurTaxCal( UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax )
{
    SHORT       sStatus;
    TRANGCFQUAL *WorkGCF;

    TrnGetGCFQual( &WorkGCF );                                      /* get GCF qualifier */
    if ( *puchTotal & 0x01 ) {                                      /* calculate tax 1 */
        if ( ( sStatus = ItemCurTax1( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }

    if ( *puchTotal & 0x10 ) {                                      /* calculate tax 2 */
        if ( ( sStatus = ItemCurTax2( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }
                                                                    
    if ( *( puchTotal + 1 ) & 0x01 ) {                              /* calculate tax 3 */
        if ( ( sStatus = ItemCurTax3( WorkTax ) ) != ITM_SUCCESS ) {  
            return( sStatus );
        }
    }

    return( ITM_SUCCESS );
}
#endif

/*==========================================================================
**   Synopsis:  VOID    ItemCanTaxWork1( CANTAXWORK *CanTaxWork, CANTAX *CanTax,
*                           ITEMCOMMONTAX *WorkTax, SHORT sType )
*
*   Input:      CANTAXWORK *CanTaxWork, CANTAX *CanTax, ITEMCOMMONTAX *WorkTax
*   Output:     ITEMCOMMONTAX   *WorkTax 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate 9 itemizer(simple tax/ tax on tax)
==========================================================================*/

VOID    ItemCanTaxSpecBakedCorr(ITEMCOMMONTAX *WorkTax)
{
    PARAPIGRULE     ParaPigRule;
	ITEMSALES		ItemSales;
	SHORT			sType = TrnDetermineStorageType();

	//tax rate table struct for obtaining the exempt meal limit (GSU SR 61 by cwunn)

	//this function searches through the current transaction and
	//returns an itemsales item, and will continue to do so until
	//the end of the file. A -1 in the second parameter will cause
	//this function to reset to read from the beginning of the file JHHJ.
	TrnStoItemSearchAll(&ItemSales, -1);
    while(TrnStoItemSearchAll(&ItemSales, sType) == TRN_SUCCESS) 
	{
		USHORT			fbWorkMod;    // same as fbModifier member of ITEMSALES struct

		//in the follow lines of code, 
		//we are getting the type of
		//item that this canadian item is
		fbWorkMod = ItemSales.fbModifier;
		fbWorkMod >>= 1;
		fbWorkMod &= TRN_MASKSTATCAN;

		if (fbWorkMod == 0) {
			fbWorkMod = (ItemSales.ControlCode.auchPluStatus[1] & TRN_MASKSTATCAN); 
			if (fbWorkMod > STD_PLU_ITEMIZERS_MOD) {
				fbWorkMod = STD_PLU_ITEMIZERS_MOD;
			}
			fbWorkMod++;
		}

		//if it is a special baked item type,
		//AND the special type is set to the  CT type
		if((!CliParaMDCCheck(MDC_CANTAX_ADR, EVEN_MDC_BIT3)) && (fbWorkMod == CANTAX_SPEC_BAKED))
		{
			//we check the pig counter for EACH item in the
			//transaction because it needs to be counted on its
			//own.
			/* check pig rule counter */
			ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;
			ParaPigRule.uchAddress = PIG_CO_LIMIT_ADR;
			CliParaRead( &ParaPigRule );

			//if the item itself is equal to or greater than
			//the counter, then we take that item's amount
			//from the taxable area.
			if((LONG)ParaPigRule.ulPigRule <= (ItemSales.lQTY/1000))
			{
				WorkTax->alTaxable[0] -= ItemSales.lProduct;
			}	
		}
	}

    return;
}    

/****** End of Source ******/