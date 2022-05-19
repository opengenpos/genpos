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
* Title       : Adjust Tone Volume Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTONE.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*              MaintToneRead()   : read & display Tone Volume
*              MaintToneWrite()  : set & print Tone Volume
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-21-92: 00.00.01 : K.You     : initial                                   
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

#include "ecr.h"
#include "pif.h"
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
** Synopsis:    SHORT MaintToneRead( PARATONECTL *pData )
*               
*     Input:    *pData          : pointer to structure for PARATONECTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays Tone Volume.
*===========================================================================
*/

SHORT MaintToneRead(PARATONECTL *pData)
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* Without Data */
        uchAddress = ++MaintWork.Tone.uchAddress;

        /* check address */
        if (uchAddress > TONE_ADR_MAX) { 
            uchAddress = 1;                                     /* Initialize Address */
        } 
    } else {                                                    /* With Data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || TONE_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* Wrong Data */
    }
    MaintWork.Tone.uchMajorClass = pData->uchMajorClass;        /* Copy Major Class */
    MaintWork.Tone.uchAddress = uchAddress;

    CliParaRead(&(MaintWork.Tone));                             

    /* Set Leadthru Data */
	RflGetLead(MaintWork.Tone.aszLeadMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.Tone));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintToneWrite( PARATONECTL *pData )
*               
*     Input:    *pData             : pointer to structure for PARATONECTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : Successful
*               LDT_SEQERR_ADR     : Sequence Error
*
** Description: This function sets & prints TAX RATE.
*===========================================================================
*/

SHORT MaintToneWrite(PARATONECTL *pData)
{

    UCHAR   uchToneData;


    /* Check Status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Data */
        return(LDT_SEQERR_ADR);                                 
    }

    /* check tax exempt limit range */

    if (pData->uchToneCtl > MAX_TONE_VOL) {                 /* Out of Range */
        return(LDT_KEYOVER_ADR);                            
    }

    MaintWork.Tone.uchToneCtl = pData->uchToneCtl;
    CliParaWrite(&(MaintWork.Tone));

    /* Set Tone Volume to Terminal */

    uchToneData = pData->uchToneCtl;
    if (uchToneData == MAX_TONE_VOL) {
        uchToneData = 0x04;
    }

    PifToneVolume(uchToneData);

    /* Control Header Item */
        
    MaintHeaderCtl(AC_TONE_CTL, RPT_ACT_ADR);

    /* Set Journal Bit & Receipt Bit */

    MaintWork.Tone.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.Tone));
    
    /* Set Address for Display Next Address */

    /* Check Address */

    MaintWork.Tone.uchAddress++;

    if (MaintWork.Tone.uchAddress > TONE_ADR_MAX) {
        MaintWork.Tone.uchAddress = 1;                      /* Initialize Address */
    }

    MaintToneRead(&(MaintWork.Tone));
    return(SUCCESS);
}
