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
* Title       : Thermal Print Rounding Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSRNDT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupRound() : forms Rounding Table Data print format    
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.IKEDA   : Initial                                   
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
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
/* #include <cswai.h> */
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupRound( PARAROUNDTBL *pData )
*               
*     Input:    *pData      : pointer to structure for PARAROUNDTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Rounding Table Data thermal print format.
*
*                 : SET ROUNDING TABLE
*
*===========================================================================
*/

VOID PrtThrmSupRound( PARAROUNDTBL *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupRound1[] = _T("%18u  /  %4u");
    static const TCHAR FARCONST auchPrtThrmSupRound2[] = _T("%18u  /  %4d");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupRound1[] = _T("%12u / %4u");
    static const TCHAR FARCONST auchPrtSupRound2[] = _T("%12u / %4d");

    USHORT  usDeliModu;

    /* distinguish major class */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARAROUNDTBL_DELI:                           /* delimitor, modules */

        usDeliModu = pData->uchRoundDelimit * 100;                                        
        usDeliModu += pData->uchRoundModules;                
        
        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ADDRESS / DELIMITOR / MODULUS */
        
            PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
                      auchPrtThrmSupRound1,                     /* format */
                      ( USHORT)pData->uchAddress,               /* address */
                      usDeliModu);                              /* delimitor, modulus Data */
        } 
        
        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ADDRESS / DELIMITOR / MODULUS */
        
            PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                      auchPrtSupRound1,                         /* format */
                      ( USHORT)pData->uchAddress,               /* address */
                      usDeliModu);                              /* delimitor, modules data */
        }
        break;

    case CLASS_PARAROUNDTBL_POSI:                           /* rounding position */

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ROUNDING POSITION */

            PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
                      auchPrtThrmSupRound2,                     /* format */
                      ( USHORT)pData->uchAddress,               /* address */
                      ( SHORT)pData->chRoundPosition);          /* position */
        } 
        
        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ROUNDING POSITION */

            PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                      auchPrtSupRound2,                         /* format */
                      ( USHORT)pData->uchAddress,               /* address */
                      ( SHORT)pData->chRoundPosition);          /* position */
        }
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;                                      
    }
}
/***** End of Source *****/
