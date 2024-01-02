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
* Title       : Manual Alternate Kitchen Printer Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATMKIT.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MaintManualAltKitchRead()   : Read Manual Alt. Kitch. Prt. Module   
*               MaintManualAltKitchEdit()   : Edit Manual Alt. Kitch. Prt. Module   
*               MaintManualAltKitchWrite()  : Write Manual Alt. Kitch. Prt. Module   
*               MaintManuAltKitchFin()      : Send Manual Alt. Kitch. Prt. to Kitch. Manager Module 
*               MaintManualAltKitchReport() : Report Manual Alt. Kitch. Prt. Module   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-30-92:00.00.01    :K.You      : initial                                   
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
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <csstbkps.h>
#include <rfl.h> 
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h"

/*
*===========================================================================
** Synopsis:    SHORT MaintManuAltKitchRead( PARAALTKITCH *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARAALTKITCH  
*      Output:  Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: Read Manual Alt. Kitch. Prt. Module    
*===========================================================================
*/

SHORT MaintManuAltKitchRead( PARAALTKITCH *pData )
{
    UCHAR           uchSrcPtr;

    /* Check W/ Amount */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                        /* W/o Amount */
        uchSrcPtr = ++MaintWork.AltKitch.uchSrcPtr;

        /* Check Max Address */
        if (MAX_DEST_SIZE < uchSrcPtr) {                                /* Over Max. */
            uchSrcPtr = 1;                                              /* Initialize Address */
        } 
    } else {                                                            /* W/ Data */
        uchSrcPtr = pData->uchSrcPtr;
    }
  
    /* Check Address */
    if (uchSrcPtr < 1 || MAX_DEST_SIZE < uchSrcPtr) {                   /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.AltKitch.uchMajorClass = pData->uchMajorClass;                                       
    MaintWork.AltKitch.uchSrcPtr = uchSrcPtr;
    ParaRead(&(MaintWork.AltKitch));                                 /* Read Parameter */

	RflGetLead(MaintWork.AltKitch.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.AltKitch));                                   /* Display Parameter */
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintManuAltKitchWrite( PARAALTKITCH *pData )
*                  
*       Input:  *pData          : Pointer to Structure for PARAALTKITCH  
*      Output:  Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: Write Manu Alt. Kitch. Prt. Module   
*===========================================================================
*/

SHORT MaintManuAltKitchWrite( PARAALTKITCH *pData )
{
    /* Check W/ Amount */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/o Amount */
        return(LDT_SEQERR_ADR);
    }

    /* Check Input Data Range */
    if (MAX_DEST_SIZE < pData->uchDesPtr) {                 /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.AltKitch.uchDesPtr = pData->uchDesPtr;
    ParaWrite(&(MaintWork.AltKitch));   

    /* Control Header Item */
    MaintHeaderCtl(AC_MANU_KITCH, RPT_ACT_ADR);

    MaintWork.AltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.AltKitch));

    /* Display Next Address */
    MaintWork.AltKitch.uchSrcPtr++;

    if (MAX_DEST_SIZE < MaintWork.AltKitch.uchSrcPtr) {         /* Over Max. */
        MaintWork.AltKitch.uchSrcPtr = 1;                       /* Initialize Address */
    }
    MaintManuAltKitchRead(&(MaintWork.AltKitch));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintManuAltKitchFin( VOID )
*                  
*       Input:  Nothing  
*      Output:  Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: Send Manual Alt. Kitch. Prt. Para. to Kitch. Manager Module   
*===========================================================================
*/

VOID MaintManuAltKitchFin( VOID )
{
    UCHAR   auchSndBuff[MAX_DEST_SIZE] = { 0 };

    /* Read All Manual Alt. Kitch. Prt. Parameter  */
    ParaManuAltKitchReadAll(auchSndBuff);

    /* Send Manual Alt. Kitch. Prt. to Kitch. Manager */
    SerAlternativeKP(auchSndBuff, sizeof(auchSndBuff));
}

/*
*===========================================================================
** Synopsis:    VOID MaintManuAltKitchReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing 
*
** Description: Report Manual Alt. Kitch. Prt. Module   
*===========================================================================
*/

VOID MaintManuAltKitchReport( VOID )
{
    PARAALTKITCH     ParaAltKitch = { 0 };

    /* Control Header Item */
    MaintHeaderCtl(AC_MANU_KITCH, RPT_ACT_ADR);

    ParaAltKitch.uchMajorClass = CLASS_PARAMANUALTKITCH;
    ParaAltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Report Each Item */
    for (UCHAR i = 1; i <= MAX_DEST_SIZE; i++) {

        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }

        ParaAltKitch.uchSrcPtr = i;
        ParaRead(&ParaAltKitch);       
        PrtPrintItem(NULL, &ParaAltKitch);
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

