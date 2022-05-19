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
* Title       : Manual Alternate Kitchen Printer Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATMKIT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
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
        if (MAKTN_ADR_MAX < uchSrcPtr) {                                /* Over Max. */
            uchSrcPtr = 1;                                              /* Initialize Address */
        } 
    } else {                                                            /* W/ Data */
        uchSrcPtr = pData->uchSrcPtr;
    }
  
    /* Check Address */
    if (uchSrcPtr < 1 || MAKTN_ADR_MAX < uchSrcPtr) {                   /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.AltKitch.uchMajorClass = pData->uchMajorClass;                                       
    MaintWork.AltKitch.uchSrcPtr = uchSrcPtr;
    CliParaRead(&(MaintWork.AltKitch));                                 /* Read Parameter */

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

    if (AAKTN_ADR_MAX < pData->uchDesPtr) {                 /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.AltKitch.uchDesPtr = pData->uchDesPtr;
    CliParaWrite(&(MaintWork.AltKitch));   

    /* Control Header Item */
    
    MaintHeaderCtl(AC_MANU_KITCH, RPT_ACT_ADR);

    MaintWork.AltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.AltKitch));

    /* Display Next Address */

    MaintWork.AltKitch.uchSrcPtr++;

    if (AAKTN_ADR_MAX < MaintWork.AltKitch.uchSrcPtr) {         /* Over Max. */
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

    UCHAR   auchSndBuff[MAX_DEST_SIZE];
    USHORT  usRetLen;


    /* Read All Manual Alt. Kitch. Prt. Parameter  */

    CliParaAllRead(CLASS_PARAMANUALTKITCH,          /* Major Class */
                   auchSndBuff,                     /* Read Buffer */
                   sizeof(auchSndBuff),             /* Data Length */
                   0,                               /* Start Pointer */
                   &usRetLen);                      /* Read Length */

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


    UCHAR            i;
    PARAALTKITCH     ParaAltKitch;


    /* Control Header Item */
    
    MaintHeaderCtl(AC_MANU_KITCH, RPT_ACT_ADR);

    ParaAltKitch.uchMajorClass = CLASS_PARAMANUALTKITCH;
    ParaAltKitch.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Report Each Item */

    for (i = 1; i <= MAKTN_ADR_MAX; i++) {

        /* Check Abort Key */

        if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }

        ParaAltKitch.uchSrcPtr = i;
        CliParaRead(&ParaAltKitch);       
        PrtPrintItem(NULL, &ParaAltKitch);
    }

    /* Make Trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}







                
