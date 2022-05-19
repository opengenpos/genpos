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
* Title       : Supervisor Level Assignment Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATSUPL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                                       
*              MaintSupLevelRead()      : Read & Display Supervisor Level
*              MaintSupLevelWrite()     : Set & Print Supervisor Level
*              MaintSupLevelReport()    : Print All Supervisor Level   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-18-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include <ecr.h>
#include <uie.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 
                                              
/*
*===========================================================================
** Synopsis:    SHORT MaintSupLevelRead( PARASUPLEVEL *pData )
*               
*     Input:    *pData          : pointer to structure for PARASUPLEVEL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : Wrong Data
*
** Description: This Function Reads & Displays Supervisor Level.
*===========================================================================
*/

SHORT MaintSupLevelRead( PARASUPLEVEL *pData )
{


    UCHAR uchAddress;


   
    /* Check Status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* Without Data */
        uchAddress = ++MaintWork.SupLevel.uchAddress;

        /* Check Address */

        if (uchAddress > SPNO_ADR_MAX) { 
            uchAddress = 1;                                     /* Initialize Address */
        }
    } else {                                                    /* With Data */
        uchAddress = pData->uchAddress;
    }

    /* Check Address */

    if (uchAddress < 1 || SPNO_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* Wrong Data */
    }
    MaintWork.SupLevel.uchMajorClass = pData->uchMajorClass;    /* Copy Major class */
    MaintWork.SupLevel.uchAddress = uchAddress;
    MaintWork.SupLevel.usSuperNumber = 0;
    CliParaRead(&(MaintWork.SupLevel));                         
    DispWrite(&(MaintWork.SupLevel));
    return(SUCCESS);

}

/*
*===========================================================================
** Synopsis:    SHORT MaintSupLevelWrite( PARASUPLEVEL *pData )
*               
*     Input:    *pData             : pointer to structure for PARASUPLEVEL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : Successful
*               LDT_KEYOVER_ADR    : Wrong Data
*               LDT_SEQERR_ADR     : Sequence Error
*
** Description: This Function Sets & Print Supervisor Number.
*===========================================================================
*/

SHORT MaintSupLevelWrite( PARASUPLEVEL *pData )
{



    /* Check Status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* Without Data */
        return(LDT_SEQERR_ADR);                                 /* Sequence Error */
    }

    /* Check Supervisor Range */

    if (pData->usSuperNumber < MAINT_SUPERNO_MIN || pData->usSuperNumber > MAINT_SUPERNO_MAX) {     /* Out of Range */
        return(LDT_KEYOVER_ADR);                                
    }

    MaintWork.SupLevel.usSuperNumber = pData->usSuperNumber;
    MaintWork.SupLevel.uchSecret = 0;
    CliParaWrite(&(MaintWork.SupLevel));                        

    /* Control Header Item */
        
    MaintHeaderCtl(PG_SUPLEVEL, RPT_PRG_ADR);

    /* Set Journal Bit & Receipt Bit */

    MaintWork.SupLevel.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.SupLevel));
    
    /* Set Address for Display Next Address */

    /* Check Address */

    MaintWork.SupLevel.uchAddress++;

    if (MaintWork.SupLevel.uchAddress > SPNO_ADR_MAX) {
        MaintWork.SupLevel.uchAddress = 1;                      /* Initialize Address */
    }

    MaintSupLevelRead(&(MaintWork.SupLevel));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSupLevelReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This Function Prints All Supervisor Number.
*===========================================================================
*/

VOID MaintSupLevelReport( VOID )
{


    UCHAR           i;
    PARASUPLEVEL    ParaSupLevel;


    /* Control Header Item */
        
    MaintHeaderCtl(PG_SUPLEVEL, RPT_PRG_ADR);
    ParaSupLevel.uchMajorClass = CLASS_PARASUPLEVEL;

    /* Set Journal Bit & Receipt Bit */

    ParaSupLevel.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Set Data at Every Address */

    for (i = 1; i <= SPNO_ADR_MAX; i++) {

        /* Check Abort Key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }
        ParaSupLevel.uchAddress = i;
        ParaSupLevel.usSuperNumber = 0;
        CliParaRead(&ParaSupLevel);                             
        PrtPrintItem(NULL, &ParaSupLevel);
    }

    /* Make Trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}


















                                                                                            
