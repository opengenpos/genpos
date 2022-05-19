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
* Title       : MaintPigRule Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPIG.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Pig Rule Table.
*                                  
*           The provided function names are as follows: 
*                                   
*              MaintPigRuleRead()   : read & display Pig Rule Table
*              MaintPigRuleWrite()  : set & print Pig Rule Table
*              MaintPigRuleReport() : print all Pig Rule Report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-09-93: 01.00.12 : K.You     : initial                                   
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
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintPigRuleRead( PARAPIGRULE *pData )
*               
*     Input:    *pData          : pointer to structure for PARAPIGRULE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays Pig Rule Table.
*===========================================================================
*/

SHORT MaintPigRuleRead( PARAPIGRULE *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.PigRuleTbl.uchAddress;

        /* check address */
        if (uchAddress > PIGRULE_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || PIGRULE_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.PigRuleTbl.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.PigRuleTbl.uchAddress = uchAddress;

    /* Set Minor Class */
    if (uchAddress == PIG_CO_LIMIT_ADR) {
        MaintWork.PigRuleTbl.uchMinorClass = CLASS_PARAPIGRULE_COLIMIT;
    } else {
        MaintWork.PigRuleTbl.uchMinorClass = CLASS_PARAPIGRULE_PRICE;
    }

    CliParaRead(&(MaintWork.PigRuleTbl));                       

    /* Set Leadthru Data */
	RflGetLead(MaintWork.PigRuleTbl.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.PigRuleTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPigRuleWrite( PARAPIGRULE *pData )
*               
*     Input:    *pData             : pointer to structure for PARAPIGRULE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_KEYOVER_ADR    : error
*
** Description: This function sets & prints Pig Rule Table.
*===========================================================================
*/

SHORT MaintPigRuleWrite( PARAPIGRULE *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  pig rule has already checked by UI */

    /* check counter limit for Non GST Tax range */

    if (MaintWork.PigRuleTbl.uchAddress == PIG_CO_LIMIT_ADR) {
        if (pData->ulPigRule > MAINT_PIG_COLIMIT_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    }
    MaintWork.PigRuleTbl.ulPigRule = pData->ulPigRule;
    CliParaWrite(&(MaintWork.PigRuleTbl));                      

    /* control header item */
        
    MaintHeaderCtl(AC_PIGRULE, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.PigRuleTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.PigRuleTbl));
    
    /* set address for display next address */

    /* check address */

    MaintWork.PigRuleTbl.uchAddress++;

    if (MaintWork.PigRuleTbl.uchAddress > PIGRULE_ADR_MAX) {
        MaintWork.PigRuleTbl.uchAddress = 1;                    /* initialize address */
    }

    MaintPigRuleRead(&(MaintWork.PigRuleTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPigRuleReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Pig Rule Table.
*===========================================================================
*/

VOID MaintPigRuleReport( VOID )
{

                    
    UCHAR           i;
	PARAPIGRULE     ParaPigRule = { 0 };


    /* control header item */
        
    MaintHeaderCtl(AC_PIGRULE, RPT_ACT_ADR);
    ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;

    /* set journal bit & receipt bit */

    ParaPigRule.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= PIGRULE_ADR_MAX ; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaPigRule.uchAddress = i;

        /* Set Minor Class */

        if (i == PIG_CO_LIMIT_ADR) {
            ParaPigRule.uchMinorClass = CLASS_PARAPIGRULE_COLIMIT;
        } else {
            ParaPigRule.uchMinorClass = CLASS_PARAPIGRULE_PRICE;
        }

        CliParaRead(&ParaPigRule);                              
        PrtPrintItem(NULL, &ParaPigRule);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

/****** End of Source ******/    
    
    
    
    
