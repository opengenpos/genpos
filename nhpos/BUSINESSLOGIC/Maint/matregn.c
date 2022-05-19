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
* Title       : MaintStoreRegNo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATREGN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report STORE/REGISTER NO.
*
*           The provided function names are as follows: 
* 
*              MaintStoRegNoRead()   : read & display STORE/REGISTER NO.
*              MaintStoRegNoWrite()  : set & print STORE/REGISTER NO.
*              MaintStoRegNoReport() : print all STORE/REGISTER NO.   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-10-92: 00.00.01 : J.Ikeda   : initial                                   
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
** Synopsis:    SHORT MaintStoRegNoRead( PARASTOREGNO *pData )
*               
*     Input:    *pData          : pointer to structure for PARASTOREGNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays STORE/REGISTER NO.
*===========================================================================
*/

SHORT MaintStoRegNoRead( PARASTOREGNO *pData )
{

    UCHAR uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.StoRegNo.uchAddress;

        /* check address */

        if (uchAddress > SREG_NO_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 

    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || SREG_NO_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.StoRegNo.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.StoRegNo.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.StoRegNo));                          /* call ParaStoRegNoRead() */
    DispWrite(&(MaintWork.StoRegNo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintStoRegNoWrite( PARASTOREGNO *pData )
*               
*     Input:    *pData             : pointer to structure for PARASTOREGNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints STORE/REGISTER NO.
*===========================================================================
*/

SHORT MaintStoRegNoWrite( PARASTOREGNO *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }
        
    /* store/register number has already checked by UI */

    MaintWork.StoRegNo.usStoreNo = pData->usStoreNo;
    MaintWork.StoRegNo.usRegisterNo = pData->usRegisterNo;
    CliParaWrite(&(MaintWork.StoRegNo));                        /* call ParaStoRegNoWrite() */

    /* control header item */
        
    MaintHeaderCtl(PG_STOREG_NO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.StoRegNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.StoRegNo));
    
    /* set address for display next address */

    /* check address */

    MaintWork.StoRegNo.uchAddress++;

    if (MaintWork.StoRegNo.uchAddress > SREG_NO_ADR_MAX) {
        MaintWork.StoRegNo.uchAddress = 1;                      /* initialize address */
    }
    MaintStoRegNoRead(&(MaintWork.StoRegNo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintStoRegNoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all STORE/REGISTER NO.
*===========================================================================
*/

VOID MaintStoRegNoReport( VOID )
{


    UCHAR          i;
    PARASTOREGNO   ParaStoRegNo;


    /* control header item */
        
    MaintHeaderCtl(PG_STOREG_NO, RPT_PRG_ADR);
    ParaStoRegNo.uchMajorClass = CLASS_PARASTOREGNO;

    /* set journal bit & receipt bit */

    ParaStoRegNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= SREG_NO_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaStoRegNo.uchAddress = i;
        CliParaRead(&ParaStoRegNo);                             /* call ParaStoRegNoRead() */
        PrtPrintItem(NULL, &ParaStoRegNo);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}
