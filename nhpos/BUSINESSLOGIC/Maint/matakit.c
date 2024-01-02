/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
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
* Title       : Manual Auto Kitchen Printer Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATAKIT.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MaintAutoAltKitchRead()     : Read Auto Alt. Kitch. Prt. Module   
*               MaintAutoAltKitchWrite()    : Write Auto Alt. Kitch. Prt. Module   
*               MaintAutoAltKitchReport()   : Report Auto Alt. Kitch. Prt. Module   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-22-92:00.00.01    :K.You      : initial                                   
*          :            :                                    
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
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintAutoAltKitchRead( PARAALTKITCH *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARAALTKITCH  
*      Output:  Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: Read Auto Alt. Kitch. Prt. Module   
*===========================================================================
*/

SHORT MaintAutoAltKitchRead( PARAALTKITCH *pData )
{
    UCHAR uchSrcPtr;

    /* Check W/ Amount */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                        /* W/o Amount */
        uchSrcPtr = ++MaintWork.AltKitch.uchSrcPtr;

        /* Check Max Address */
        if (MAX_DEST_SIZE < uchSrcPtr) {                                /* Over Max. */
            uchSrcPtr = 1;                                              /* Initialize address */
        } 
    } else {                                                            /* W/ Amount */
        uchSrcPtr = pData->uchSrcPtr;
    }
  
    /* Check Address */
    if (uchSrcPtr < 1 || MAX_DEST_SIZE < uchSrcPtr) {                   /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    /* Execute Read and Display Parameter */
    MaintWork.AltKitch.uchMajorClass = pData->uchMajorClass;                                        
    MaintWork.AltKitch.uchSrcPtr = uchSrcPtr;
    ParaRead(&(MaintWork.AltKitch));                                 
    DispWrite(&(MaintWork.AltKitch));                                   
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintAutoAltKitchWrite( PARAALTKITCH *pData )
*                  
*       Input:  *pData          : Pointer to Structure for PARAALTKITCH  
*      Output:  Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: Write Auto Alt. Kitch. Prt. Module   
*===========================================================================
*/

SHORT MaintAutoAltKitchWrite( PARAALTKITCH *pData )
{
    /* Check W/ Amount */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/o Amount */
        return(LDT_SEQERR_ADR);
    }

    /* Check Input Data Range */
    if (MAX_DEST_SIZE < pData->uchDesPtr) {  /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.AltKitch.uchDesPtr = pData->uchDesPtr;
    ParaWrite(&(MaintWork.AltKitch));   

    /* Control Header Item */
    MaintHeaderCtl(PG_AUTO_KITCH, RPT_PRG_ADR);

    MaintWork.AltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.AltKitch));

    /* Display Next Address */
    MaintWork.AltKitch.uchSrcPtr++;

    if (MAX_DEST_SIZE < MaintWork.AltKitch.uchSrcPtr) {         /* Over Max. */
        MaintWork.AltKitch.uchSrcPtr = 1;                       /* Initialize Address */

    }
    MaintAutoAltKitchRead(&(MaintWork.AltKitch));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintAutoAltKitchReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing 
*
** Description: Report Auto Alt. Kitch. Prt. Module   
*===========================================================================
*/

VOID MaintAutoAltKitchReport( VOID )
{
    PARAALTKITCH     ParaAltKitch = { 0 };

    /* Control Header Item */
    MaintHeaderCtl(PG_AUTO_KITCH, RPT_PRG_ADR);

    ParaAltKitch.uchMajorClass = CLASS_PARAAUTOALTKITCH;
    ParaAltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Report Each Item */
    for (UCHAR i = 1; i <= MAX_DEST_SIZE; i++) {

        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }

        ParaAltKitch.uchSrcPtr = i;
        ParaAutoAltKitchRead(&ParaAltKitch);
        PrtPrintItem(NULL, &ParaAltKitch);
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}


