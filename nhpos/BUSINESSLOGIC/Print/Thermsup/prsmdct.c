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
* Title       : Thermal Print Bit Pattern print format 
*                                                   ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSMDCT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*              PrtThrmSupMDC() : form MDC
*                                     Action Code Security
*                                                          print format.
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-16-93 : 01.00.12 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>

#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <prt.h>

#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupMDC( VOID *pData )
*               
*     Input:    *pData      : pointer to structure for PARAMDC / PARAACTCODESEC
*                                                      
*    Output:    Nothing                                
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms BIT PATTERN print format.
*                    
*                    : MACHINE DEFINITION CODE
*                    : ACTION CODE SECURITY
*
*===========================================================================
*/

VOID  PrtThrmSupMDC( VOID *pData )
{
    TCHAR           aszBuffer[8] = { 0 };
    UCHAR           uchMajorClass = ITEMCLASSPTR(pData)->uchMajorClass;
    USHORT          usAddress;
    USHORT          usPrintControl;
    PARAMDC         *pParaMDC = pData;
    PARAACTCODESEC  *pParaActCode = pData;

    /* distinguish major class */ 
    switch (uchMajorClass) {
    case CLASS_PARAMDC:                                             /* MDC */
        usAddress = pParaMDC->usAddress;
        PrtSupItoa(pParaMDC->uchMDCData, aszBuffer);                /* convert status data to binary ASCII data */
        usPrintControl = pParaMDC->usPrintControl;
        break;

    case CLASS_PARAACTCODESEC:                                      /* Action Code Security */
        usAddress = pParaActCode->usAddress;
        PrtSupItoa(pParaActCode->uchSecurityBitPatern, aszBuffer);  /* convert status data to binary ASCII data */
        usPrintControl = pParaActCode->usPrintControl;
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        return;

    }

    /* check print status */
    if (usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        static const TCHAR  auchPrtThrmSupMDC[] = _T("%18u  /  %s");       /* define thermal print format */        
        /* print ADDRESS / BIT PATTERN */
        PrtPrintf(PMG_PRT_RECEIPT,  auchPrtThrmSupMDC, usAddress, aszBuffer);                   /* status */
    } 
    
    if (usPrintControl & PRT_JOURNAL) {  /* EJ */
        static const TCHAR  auchPrtSupMDC[] = _T("%8u / %s");     /* define EJ print format */            
        /* print ADDRESS / BIT PATTERN */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupMDC, usAddress, aszBuffer);                   /* status */
    }
}
/***** End of Source *****/
