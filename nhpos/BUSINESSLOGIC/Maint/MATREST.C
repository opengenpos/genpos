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
* Title       : MaintRestriction Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US General Purpose Application Program        
* Program Name: MATREST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls PLU Sales Restriction Table.
*
*           The provided function names are as follows: 
* 
*             MaintRestRead()        : read & display parameter from Para.Module
*             MaintRestEdit()        : check data & set data in self-work area 
*             MaintRestWrite()       : write table to Para. Module & print 
*             MaintRestReport()      : print all restriction table   
*  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-20-93: 00.00.01 : M.Ozawa   : Initial for GP
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
#include "pif.h"
#include "log.h"
#include "paraequ.h" 
#include "para.h"
#include "rfl.h" 
#include "maint.h" 
#include "regstrct.h"
#include "display.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"

#include "maintram.h" 

/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/


/*
*===========================================================================
** Synopsis:    SHORT MaintRestRead( PARARESTRICTION *pData )
*
*     Input:    *pData          : pointer to structure for PARARECTRICTION
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_SEQERR_ADR  : sequence error
*
** Description: This function reads & displays parameter from Para. Module in self-work area.
*===========================================================================
*/

SHORT MaintRestRead( PARARESTRICTION *pData )
{

    UCHAR  uchAddress = 0;

    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_PARAREST_ADDRESS:    /* #/type key entry */
        /* check status */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */

            /* check whether current address is setted or not */

            uchAddress = ++MaintWork.Restriction.uchAddress;

            /* check address */

            if (uchAddress > REST_ADR_MAX) {
                uchAddress = REST_CODE1_ADR;                        /* initialize address */
            }
        } else {
            if ((pData->uchAddress == 0) || (pData->uchAddress > REST_ADR_MAX)) {
                return(LDT_KEYOVER_ADR);
            }

            uchAddress = pData->uchAddress;

        }
        break;

    default:   /* initial read */
        /* check address */

        if (pData->uchAddress < 1 || REST_ADR_MAX < pData->uchAddress) {
            return(LDT_KEYOVER_ADR);                                /* wrong data */
        }

        uchAddress = pData->uchAddress;

    }

    /* read restriction table  */

    MaintWork.Restriction.uchMajorClass = CLASS_PARARESTRICTION;
    MaintWork.Restriction.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.Restriction));                       /* call ParaDiscRead() */

    /* display field 1 data */

    MaintWork.Restriction.uchField = REST_DATE_FIELD;
    MaintWork.Restriction.uchData = MaintWork.Restriction.uchDate;

    MaintRestDispData(&MaintWork.Restriction);

    /* reset data input status */

    MaintWork.Restriction.uchStatus &= ~MAINT_DATA_INPUT;

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintRestEdit( PARARESTRICTION *pData )
*               
*     Input:    *pData               : pointer to structure for PARARESTRICTION
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS              : successful
*               LDT_SEQERR_ADR       : sequence error
*               LDT_KEYOVER_ADR      : wrong data
*
** Description: This function checks input-data from UI and sets in self-work area.
*===========================================================================
*/

SHORT MaintRestEdit( PARARESTRICTION *pData )
{

    UCHAR  uchAddress = 0;
    UCHAR  uchField = 0;

    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_PARAREST_ADDRESS:
         return(LDT_SEQERR_ADR);                                 /* sequence error */

    case CLASS_PARAREST_FIELD:

        /* check status */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
            uchField = ++MaintWork.Restriction.uchField;

            /* check address */

            if (uchField > REST_FIELD_MAX) {
                uchField = REST_DATE_FIELD;                           /* initialize field */
            }
        } else {
            if ((pData->uchField == 0) || (pData->uchField > REST_FIELD_MAX)) {
                return(LDT_KEYOVER_ADR);
            }
            uchField = pData->uchField;
        }
        uchAddress = MaintWork.Restriction.uchAddress;

        break;

    case CLASS_PARAREST_DATA:

        /* check status */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
            return(LDT_SEQERR_ADR);                                 /* sequence error */
        }

        switch (MaintWork.Restriction.uchField) {
        case REST_DATE_FIELD:
            if (pData->uchData > 31) {
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchDate = pData->uchData;

            break;

        case REST_WEEK_FIELD:
            if (pData->uchData > 7){
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchDay = pData->uchData;

            break;

        case REST_HOUR1_FIELD:
            if (pData->uchData > 24){
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchHour1 = pData->uchData;

            break;

        case REST_MINUTE1_FIELD:
            if (pData->uchData > 59){
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchMin1 = pData->uchData;

            break;

        case REST_HOUR2_FIELD:
            if (pData->uchData > 24){
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchHour2 = pData->uchData;

            break;

        case REST_MINUTE2_FIELD:
            if (pData->uchData > 59){
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.Restriction.uchMin2 = pData->uchData;

            break;

        default:
           return(SUCCESS);
        }

        uchAddress = MaintWork.Restriction.uchAddress;
        uchField = MaintWork.Restriction.uchField;
        uchField++;
        if (uchField > REST_FIELD_MAX) {
            uchField = REST_DATE_FIELD;                           /* initialize field */
        }
        MaintWork.Restriction.uchStatus |= MAINT_DATA_INPUT;
        break;

    default:
        return(SUCCESS);
    }    

    /* set data for display next old data */

    MaintWork.Restriction.uchMinorClass = pData->uchMinorClass;
    MaintWork.Restriction.uchAddress = uchAddress;
    MaintWork.Restriction.uchField = uchField;

    switch (uchField) {
    case REST_DATE_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchDate;
        break;

    case REST_WEEK_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchDay;
        break;

    case REST_HOUR1_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchHour1;
        break;

    case REST_MINUTE1_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchMin1;
        break;

    case REST_HOUR2_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchHour2;
        break;

    case REST_MINUTE2_FIELD:
        MaintWork.Restriction.uchData = MaintWork.Restriction.uchMin2;
        break;
    }

    MaintRestDispData(&MaintWork.Restriction);

    return(SUCCESS);
}                                                              

/*                                                             
*===========================================================================
** Synopsis:    SHORT MaintRestWrite( VOID )                   
*               
*     Input:    Nothing                                        
*    Output:    Nothing 
*     InOut:    Nothing                                        
*
** Return:      SUCCESS   : successful                         
*               sReturn   : error 
*                                                              
** Description: This function writes restriction table to para. module, and print out.
*===========================================================================
*/
                                                               
SHORT MaintRestWrite( VOID )                                   
{
                                                               
                                                               
    CliParaWrite(&(MaintWork.Restriction));    /* write current address table */

    /* control header item */
        
    MaintHeaderCtl(AC_PLU_SLREST, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.Restriction.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.Restriction));
    
    /* set address for display next address */

    MaintWork.Restriction.uchMinorClass = CLASS_PARAREST_READ;
    MaintWork.Restriction.uchAddress++;

    if (MaintWork.Restriction.uchAddress > REST_ADR_MAX) {
        MaintWork.Restriction.uchAddress = REST_CODE1_ADR;           /* initialize address */
    }
    MaintRestRead(&(MaintWork.Restriction));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintRestReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all restricton table from Para.Module
*===========================================================================
*/

VOID MaintRestReport( VOID )
{


    UCHAR            i;
    PARARESTRICTION  ParaRest;


    /* control header item */
        
    MaintHeaderCtl(AC_PLU_SLREST, RPT_ACT_ADR);
    ParaRest.uchMajorClass = CLASS_PARARESTRICTION;

    /* set journal bit & receipt bit */

    ParaRest.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= REST_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        ParaRest.uchAddress = i;
        CliParaRead(&ParaRest);
        PrtPrintItem(NULL, &ParaRest);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

/*
*===========================================================================
** Synopsis:    VOID MaintRestDispData(PARARESTRICTION *pData)
*               
*     Input:    *pData  : pointer to PARARESTRICTION
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data for display.
*===========================================================================
*/

VOID MaintRestDispData(PARARESTRICTION *pData)
{

    /* distinguish field address and set data for display */
    switch(pData->uchField) {
    case REST_DATE_FIELD:
    case REST_WEEK_FIELD:
    case REST_HOUR1_FIELD:
    case REST_HOUR2_FIELD:
    case REST_MINUTE1_FIELD:
    case REST_MINUTE2_FIELD:
        break;

    default:
        return;
    }

	RflGetLead(MaintWork.Restriction.aszMnemonics, LDT_DATA_ADR);     /* Request Max Price Level */
    DispWrite(&MaintWork.Restriction);
}

/*                                                             
*===========================================================================
** Synopsis:    SHORT MaintRestChkStatus( VOID )                   
*               
*     Input:    Nothing                                        
*    Output:    Nothing 
*     InOut:    Nothing                                        
*
** Return:      SUCCESS            : not inputed
*               MAINT_DATA_INPUT   : inputted
*                                                              
** Description: This function check data input status of work area.
*===========================================================================
*/
                                                               
SHORT MaintRestChkStatus( VOID )                                   
{

    if (MaintWork.Restriction.uchStatus & MAINT_DATA_INPUT) {
        return(MAINT_DATA_INPUT);
    } else {
        return(SUCCESS);
    }
}
