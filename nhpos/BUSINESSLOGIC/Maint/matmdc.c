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
* Title       : MaintMDC Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATMDC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report MDC.
*
*           The provided function names are as follows: 
* 
*              MaintMDCRead()   : read & display MDC.
*              MaintMDCWrite()  : set & print MDC data.
*              MaintMDCReport() : print MDC data report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-23-92: 00.00.01 : J.Ikeda   : initial                                   
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

/**
==============================================================================
;    Static Area Declaration                          
=============================================================================
**/

UCHAR uchMaintByteMDC;

/*
*===========================================================================
** Synopsis:    SHORT MaintMDCRead( PARAMDC *pData )
*               
*     Input:    *pData            : pointer to structure for PARAMDC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS           : successful
*               LDT_SEQERR_ADR    : sequence error
*
** Description: This function reads & displays MDC.
*===========================================================================
*/

SHORT MaintMDCRead( PARAMDC *pData )
{

    USHORT usAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        usAddress = ++MaintWork.MDC.usAddress;

        /* check address */

        if (usAddress > MDC_ADR_MAX) {                     /* MDC_ADR_MAX=94 */
            usAddress = MDC_CLI1_ADR;                      /* initialize address */
        }
    } else {                                                /* with data */
        usAddress = pData->usAddress;
    }

    /* check address */

    if (usAddress < MDC_CLI1_ADR || MDC_ADR_MAX < usAddress) {
        return(LDT_KEYOVER_ADR);                            /* out of address is */
    }

    MaintWork.MDC.uchMajorClass = pData->uchMajorClass;     /* copy major class */
    MaintWork.MDC.usAddress = usAddress;
    CliParaRead(&(MaintWork.MDC));                          /* call CliParaMDCRead() */
    uchMaintByteMDC = MaintWork.MDC.uchMDCData;

    /* check odd/even number */

    if (!(MaintWork.MDC.usAddress & 0x0001)) {             /* in case of even address */
        MaintWork.MDC.uchMDCData >>= 4;
    } else {                                                /* in case of odd address */
        MaintWork.MDC.uchMDCData &= 0x0F;
    }
    DispWrite(&(MaintWork.MDC));
    return(SUCCESS);
}    

/*
*===========================================================================
** Synopsis:    SHORT MaintMDCWrite( PARAMDC *pData )
*               
*     Input:    *pData            : pointer to structure for PARAMDC
*               uchMaintByteMDC   : saved 1 byte MDC data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS           : successful
*               LDT_SEQERR_ADR    : sequence error
*
** Description: This function sets & prints MDC data.
*===========================================================================
*/

SHORT MaintMDCWrite( PARAMDC *pData )
{

    UCHAR uchMDCBitData;


    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        return(LDT_SEQERR_ADR);                             /* sequence error */
    }

    /* check MDC data */

    if (MaintChkBinary(pData->aszMDCString, 4) == LDT_KEYOVER_ADR) {
        return(LDT_KEYOVER_ADR);                            /* wrong data */
    }

    /* convert string data into bit pattern */

    uchMDCBitData = MaintAtoStatus(pData->aszMDCString);

    /* set MDC bit data */

    if (!(MaintWork.MDC.usAddress & 0x0001)) {             /* in case of even address */
        uchMaintByteMDC &= 0x0F;
        uchMaintByteMDC |= uchMDCBitData << 4;
    } else {                                                /* in case of odd address */
        uchMaintByteMDC &= 0xF0;
        uchMaintByteMDC |= uchMDCBitData;
    }
    MaintWork.MDC.uchMDCData = uchMaintByteMDC;
    CliParaWrite(&(MaintWork.MDC));                         /* call ParaMDCWrite() */

    /* control header item */

    MaintHeaderCtl(PG_MDC, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.MDC.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);
    MaintWork.MDC.uchMDCData = uchMDCBitData;
    PrtPrintItem(NULL, &(MaintWork.MDC));

    /* set address for display next address */

    MaintWork.MDC.usAddress++;

    /* check address */

    if (MaintWork.MDC.usAddress > MDC_ADR_MAX) {
        MaintWork.MDC.usAddress = MDC_CLI1_ADR;            /* initialize address */
    }
    MaintMDCRead(&(MaintWork.MDC));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintMDCReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function prints MDC data report.
*===========================================================================
*/

VOID MaintMDCReport( VOID )
{

    USHORT    i;
    PARAMDC   ParaMDC;


    /* control header item */
    
    MaintHeaderCtl(PG_MDC, RPT_PRG_ADR);

    /* set major class */

    ParaMDC.uchMajorClass = CLASS_PARAMDC;

    /* set journal bit & receipt bit */

    ParaMDC.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = MDC_CLI1_ADR; i <= MDC_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {              /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        /* print odd address data */

        ParaMDC.usAddress = i;                             /* in case of odd address */
        CliParaRead(&(ParaMDC));                            /* call ParaMDCRead() */
        ParaMDC.uchMDCData &= 0x0F;
        PrtPrintItem(NULL, &ParaMDC);

        /* print even address data */

        ParaMDC.usAddress = ++i;                           /* in case of even address */
        CliParaRead(&(ParaMDC));                            /* call ParaMDCRead() */
        ParaMDC.uchMDCData &= 0xF0;
        ParaMDC.uchMDCData >>= 4;
        PrtPrintItem(NULL, &ParaMDC);

    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
