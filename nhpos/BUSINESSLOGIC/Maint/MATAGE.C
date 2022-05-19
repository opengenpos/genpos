/*     
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintAgePara Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US GP Application Program        
* Program Name: MATAGE.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Boundary Age. Parameter.
*
*           The provided function names are as follows: 
* 
*              MaintBoundAgeRead()   : read & display Age. PARAMETER
*              MaintBoundAgeWrite()  : set & print Age. PARAMETER
*              MaintBoundAgeReport() : print all Age. PARAMETER   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* FEB-24-94: 00.00.01 : M.INOUE   : initial                                   
*          :          :           :                                    
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
#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h> 
#include <maint.h> 
#include <regstrct.h>
#include <display.h> 
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintBoundAgeRead( PARABOUNDAGE *pData )
*               
*     Input:    *pData          : pointer to structure for PARABOUNDAGE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Boundary Age Parameter.
*===========================================================================
*/

SHORT MaintBoundAgeRead( PARABOUNDAGE *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.BoundAge.uchAddress;

        /* check address */
        if (uchAddress > AGE_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || AGE_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }

    MaintWork.BoundAge.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.BoundAge.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.BoundAge));                          /* call ParaMiscRead() */

	RflGetLead(MaintWork.BoundAge.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&(MaintWork.BoundAge));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintBoundAgeWrite( PARABOUNDAGE *pData )
*               
*     Input:    *pData             : pointer to structure for PARABOUNDAGE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints Bound Age Parameter.
*===========================================================================
*/

SHORT MaintBoundAgeWrite( PARABOUNDAGE *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */

    if (MaintWork.BoundAge.uchAddress > AGE_ADR_MAX) { 	 /* range is 0 to 100 */
            return(LDT_KEYOVER_ADR);                            /* wrong data */
    } else {
        if (pData->uchAgePara > MAINT_BOUNDAGE_MAX) { 
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    }

    MaintWork.BoundAge.uchAgePara = pData->uchAgePara;
    CliParaWrite(&(MaintWork.BoundAge));                         /* call ParaMiscWrite() */

    /* control header item */
        
    MaintHeaderCtl(AC_BOUNDARY_AGE, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.BoundAge.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.BoundAge));
    
    /* set address for display next address */

    /* check address */

    MaintWork.BoundAge.uchAddress++;

    if (MaintWork.BoundAge.uchAddress > AGE_ADR_MAX) {
        MaintWork.BoundAge.uchAddress = 1;                       /* initialize address */
    }
    MaintBoundAgeRead(&(MaintWork.BoundAge));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintBoundAgeReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Bound Age Parameter.
*===========================================================================
*/

VOID MaintBoundAgeReport( VOID )
{

    UCHAR            i;
    PARABOUNDAGE     ParaBoundAge;


    /* control header item */
        
    MaintHeaderCtl(AC_BOUNDARY_AGE, RPT_ACT_ADR);
    ParaBoundAge.uchMajorClass = CLASS_PARABOUNDAGE;

    /* set journal bit & receipt bit */

    ParaBoundAge.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= AGE_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaBoundAge.uchAddress = i;
        CliParaRead(&ParaBoundAge);                             /* call ParaAgeRead() */
        PrtPrintItem(NULL, &ParaBoundAge);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}
/******** end of matage.c ********/

