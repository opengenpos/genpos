/*     
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Maint Labor Cost Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCOST.C
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Labor Cost Value
*
*           The provided function names are as follows: 
* 
*              MaintLaborCostRead()   : read & display Labor Cost Value
*              MaintLaborCostWrite()  : set & print Labor Cost Value
*              MaintLaborCostReport() : print all Labor Cost Value
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-20-95: 03.01.00 : M.Ozawa   : initial       
** GenPOS
* Jul-17-17: 02.02.02 : R.Chambers : use MAX_LABORCOST_SIZE, localize variables.
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintLaborCostRead( PARALABORCOST *pData )
*               
*     Input:    *pData          : pointer to structure for PARALABORCOST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Labor Cost Value
*===========================================================================
*/
SHORT MaintLaborCostRead( PARALABORCOST *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.DiscTbl.uchAddress;

        /* check address */
        if (uchAddress > MAX_LABORCOST_SIZE) { 
            uchAddress = LABOR_COST1_ADR;                       /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < LABOR_COST1_ADR || MAX_LABORCOST_SIZE < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.LaborCost.uchMajorClass = pData->uchMajorClass;     /* copy major class */
    MaintWork.LaborCost.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.LaborCost));                          /* call ParaDiscRead() */
    RflGetLead (MaintWork.LaborCost.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.LaborCost));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintLaborCostWrite( PARALABORCOST *pData )
*               
*     Input:    *pData             : pointer to structure for PARALABORCOST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints labor cost value
*===========================================================================
*/
SHORT MaintLaborCostWrite( PARALABORCOST *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */
    if (pData->usLaborCost > MAINT_LABORCOST_MAX) { 
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.LaborCost.usLaborCost = pData->usLaborCost;
    CliParaWrite(&(MaintWork.LaborCost));                         /* call ParaDiscWrite() */

    /* control header item */
    MaintHeaderCtl(AC_ETK_LABOR_COST, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.LaborCost.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.LaborCost));
    
    /* set and check address for display next address */
    MaintWork.LaborCost.uchAddress++;
    if (MaintWork.LaborCost.uchAddress > MAX_LABORCOST_SIZE) {
        MaintWork.LaborCost.uchAddress = LABOR_COST1_ADR;         /* initialize address */
    }
    MaintLaborCostRead(&(MaintWork.LaborCost));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintLaborCostReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Labor Cost Value REPORT.
*===========================================================================
*/
VOID MaintLaborCostReport( VOID )
{
    UCHAR            i;

    /* control header item */
    MaintHeaderCtl(AC_ETK_LABOR_COST, RPT_ACT_ADR);
    /* set data at every address */
    for (i = LABOR_COST1_ADR; i <= MAX_LABORCOST_SIZE; i++) {
		PARALABORCOST    ParaLaborCost = {0};

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

		ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;
		ParaLaborCost.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        ParaLaborCost.uchAddress = i;
        ParaLaborCostRead (&ParaLaborCost);
        PrtPrintItem(NULL, &ParaLaborCost);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}
