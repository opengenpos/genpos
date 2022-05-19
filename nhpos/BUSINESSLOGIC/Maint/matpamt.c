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
* Title       : MaintPresetAmount Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPAMT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Preset Amount Cash Tender.
*                                  
*           The provided function names are as follows: 
*                                           
*              MaintPresetAmountRead()      : read & display Preset Amount Cash Tender
*              MaintPresetAmountWrite()     : set & print Preset Amount Cash Tender
*              MaintPresetAmountReport()    : print all Preset Amount Cash Tender  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-02-93: 01.00.12 : K.You     : initial                                   
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
** Synopsis:    SHORT MaintPresetAmountRead( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData          : pointer to structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays Preset Amount Cash Tender.
*===========================================================================
*/

SHORT MaintPresetAmountRead( PARAPRESETAMOUNT *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.TaxRateTbl.uchAddress;

        /* check address */
        if (uchAddress > PRESET_AMT_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || PRESET_AMT_ADR_MAX < uchAddress) {        
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }
    MaintWork.PresetAmount.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.PresetAmount.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.PresetAmount));                         

	RflGetLead(MaintWork.PresetAmount.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.PresetAmount));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPresetAmountWrite( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData             : pointer to structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints Preset Amount Cash Tender.
*===========================================================================
*/

SHORT MaintPresetAmountWrite( PARAPRESETAMOUNT *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  preset amount has already checked by UI */

    MaintWork.PresetAmount.ulAmount = pData->ulAmount;
    CliParaWrite(&(MaintWork.PresetAmount));                      

    /* control header item */
        
    MaintHeaderCtl(PG_PRESET_AMT, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.PresetAmount.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.PresetAmount));
    
    /* set address for display next address */

    /* check address */

    MaintWork.PresetAmount.uchAddress++;

    if (MaintWork.PresetAmount.uchAddress > PRESET_AMT_ADR_MAX) {
        MaintWork.PresetAmount.uchAddress = 1;                    /* initialize address */
    }

    MaintPresetAmountRead(&(MaintWork.PresetAmount));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPresetAmountReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Preset Amount Cash Tender.
*===========================================================================
*/

VOID MaintPresetAmountReport( VOID )
{
                        
    UCHAR               i;
	PARAPRESETAMOUNT    ParaPresetAmount = { 0 };


    /* control header item */
        
    MaintHeaderCtl(PG_PRESET_AMT, RPT_PRG_ADR);
    ParaPresetAmount.uchMajorClass = CLASS_PARAPRESETAMOUNT;

    /* set journal bit & receipt bit */

    ParaPresetAmount.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= PRESET_AMT_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaPresetAmount.uchAddress = i;

        CliParaRead(&ParaPresetAmount);                              
        PrtPrintItem(NULL, &ParaPresetAmount);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

/****** End of Source ******/    
    
    
    
    
