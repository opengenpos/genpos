/*     
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintTaxRate Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTAXR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TAX RATE.
*                                  
*           The provided function names are as follows: 
* 
*              MaintTaxRateRead()   : read & display TAX RATE
*              MaintTaxRateWrite()  : set & print TAX RATE
*              MaintTaxRateReport() : print all TAX RATE REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-19-92: 00.00.01 : J.Ikeda   : initial                                   
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/

#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h" 
#include "para.h"
#include "rfl.h" 
#include "maint.h" 
#include "display.h" 
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxRateRead( PARATAXRATETBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARATAXRATE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays TAX RATE.
*===========================================================================
*/
SHORT   MaintTaxRateRead(PARATAXRATETBL *pData)
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.TaxRateTbl.uchAddress;

        /* check address */
        if (uchAddress > TXRT_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 

    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || TXRT_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.TaxRateTbl.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.TaxRateTbl.uchAddress = uchAddress;

    /* Set Minor Class */
    if (uchAddress == TXRT_NO5_ADR) {
        MaintWork.TaxRateTbl.uchMinorClass = CLASS_PARATAXRATETBL_LIMIT;
    } else {
        MaintWork.TaxRateTbl.uchMinorClass = CLASS_PARATAXRATETBL_RATE;
    }

    CliParaRead(&(MaintWork.TaxRateTbl));                       /* call ParaTaxRateRead() */

    /* Set Leadthru Data */
	RflGetLead(MaintWork.TaxRateTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&(MaintWork.TaxRateTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxRateWrite( PARATAXRATETBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARATAXRATE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints TAX RATE.
*===========================================================================
*/
SHORT   MaintTaxRateWrite(PARATAXRATETBL *pData)
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  tax rate has already checked by UI */
    /* check tax exempt limit range */
    if (MaintWork.TaxRateTbl.uchAddress == TXRT_NO5_ADR) {
        if (pData->ulTaxRate > MAINT_TAXEXEMPT_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    }
    MaintWork.TaxRateTbl.ulTaxRate = pData->ulTaxRate;
    CliParaWrite(&(MaintWork.TaxRateTbl));                      /* call ParaTaxRateWrite() */

    /* control header item */
    MaintHeaderCtl(AC_TAX_RATE, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.TaxRateTbl.usPrintControl = PRT_JOURNAL | PRT_RECEIPT;
    PrtPrintItem(NULL, &(MaintWork.TaxRateTbl));
    
    /* set address for display next address */
    /* check address */
    MaintWork.TaxRateTbl.uchAddress++;

    if (MaintWork.TaxRateTbl.uchAddress > TXRT_ADR_MAX) {
        MaintWork.TaxRateTbl.uchAddress = 1;                    /* initialize address */
    }

    MaintTaxRateRead(&(MaintWork.TaxRateTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxRateReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all TAX RATE REPORT.
*===========================================================================
*/
VOID    MaintTaxRateReport(VOID)
{
    UCHAR                i;
	PARATAXRATETBL       ParaTaxRate = { 0 };

    /* control header item */
    MaintHeaderCtl(AC_TAX_RATE, RPT_ACT_ADR);
    ParaTaxRate.uchMajorClass = CLASS_PARATAXRATETBL;

    /* set journal bit & receipt bit */
    ParaTaxRate.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= TXRT_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaTaxRate.uchAddress = i;

        /* Set Minor Class */
        if (i == TXRT_NO5_ADR) {
            ParaTaxRate.uchMinorClass = CLASS_PARATAXRATETBL_LIMIT;
        } else {
            ParaTaxRate.uchMinorClass = CLASS_PARATAXRATETBL_RATE;
        }
        CliParaRead(&ParaTaxRate);                              /* call ParaTaxRateRead() */
        PrtPrintItem(NULL, &ParaTaxRate);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/* --- End of Source --- */
