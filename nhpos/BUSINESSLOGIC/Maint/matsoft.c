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
* Title       : Set Trailer Print Message for Soft Check Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 ENH US Hospitality Application Program        
* Program Name: MATSOFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function reads & writes Sales Promotion Message on Slip
*
*           The provided function names are as follows: 
* 
*              MaintSoftChkMsgRead()   : read & display Trailer Print Message for Soft Check 
*              MaintSoftChkMsgWrite()  : read & display Trailer Print Message for Soft Check
*              MaintSoftChkMsgReport() : report & print Trailer Print Message for Soft Check
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Aug-23-93: 01.00.13 : J.IKEDA   : Initial                                   
* Apr-04-94: 00.00.05 : Yoshiko.J : add EPT feature in GPUS
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
#include "uie.h"
#include "paraequ.h"
#include "para.h"
/* #include <cswai.h> */
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintSoftChkMsgRead( PARASOFTCHK *pData )
*               
*     Input:    *pData         : ponter to structure for PARASOFTCHK
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays Trailer Print Message for Soft Check
*===========================================================================
*/

SHORT MaintSoftChkMsgRead( PARASOFTCHK *pData )
{

    UCHAR   uchAddress;
            
    /* Initialize */

/*    memset(MaintWork.SoftChk.aszMnemonics, '\0', PARA_SOFTCHK_LEN+1); */

    /* Check Status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* Without Data */
        uchAddress = ++MaintWork.SoftChk.uchAddress;

        /* check address */

        if (uchAddress > SOFTCHK_ADR_MAX) {                         
            uchAddress = 1;                                     /* Initialize Address */
        }
    } else {                                                    /* With Data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || SOFTCHK_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                    /* Wrong Data */
    }
    MaintWork.SoftChk.uchMajorClass = pData->uchMajorClass;      /* Copy Major Class */
    MaintWork.SoftChk.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.SoftChk));                         
    DispWrite(&(MaintWork.SoftChk));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintSoftChkMsgWrite( PARASOFTCHK *pData )
*               
*     Input:    *pData         : pointer to structure for PARASOFTCHK
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function sets & prints Trailer Print Message for Soft Check.
*===========================================================================
*/

SHORT MaintSoftChkMsgWrite( PARASOFTCHK *pData )
{

    /* Check Status */
                                                                
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* Without Data */
        return(LDT_SEQERR_ADR);                                 /* Sequence Error */
    }

    _tcsncpy(MaintWork.SoftChk.aszMnemonics, pData->aszMnemonics, PARA_SOFTCHK_LEN);
    //memcpy(MaintWork.SoftChk.aszMnemonics, pData->aszMnemonics, PARA_SOFTCHK_LEN);
    CliParaWrite(&(MaintWork.SoftChk));                       

    /* Control Header Item */

    MaintHeaderCtl(AC_SOFTCHK_MSG, RPT_ACT_ADR);

    /* Set Journal Bit & Receipt Bit */

    MaintWork.SoftChk.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.SoftChk));

    /* Set Address for Display Next Address */

    MaintWork.SoftChk.uchAddress++;

    /* Check Address */

    if (MaintWork.SoftChk.uchAddress > SOFTCHK_ADR_MAX) {
        MaintWork.SoftChk.uchAddress = 1;                     /* Initialize Address */
    }
    MaintSoftChkMsgRead(&(MaintWork.SoftChk));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSoftChkMsgReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all Trailer Print Message for Soft Check.
*===========================================================================
*/

VOID MaintSoftChkMsgReport( VOID )
{

    UCHAR           i;
    PARASOFTCHK     ParaSoftChk;

    /* Initialize */

/*    memset(ParaSoftChk.aszMnemonics, '\0', PARA_SOFTCHK_LEN+1); */

    /* Control Header */

    MaintHeaderCtl(AC_SOFTCHK_MSG, RPT_ACT_ADR);

    /* Set Major Class */

    ParaSoftChk.uchMajorClass = CLASS_PARASOFTCHK;

    /* Set Journal Bit & Receipt Bit */

    ParaSoftChk.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Set Data at Every Address */

    for (i = 1; i <= SOFTCHK_ADR_MAX; i++) {

        /* Check Abort Key */

        if (UieReadAbortKey() == UIE_ENABLE) {              /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }
        ParaSoftChk.uchAddress = i;
        CliParaRead(&ParaSoftChk);                            
        PrtPrintItem(NULL, &ParaSoftChk);
    }

    /* Make Trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

















