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
* Title       : Print Tare Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTARET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupTare()     : forms Tare Table print format    
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.Ikeda   : initial                                   
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
** Synopsis:    VOID PrtThrmSupTare( PARATARE *pData )
*               
*     Input:    *pData      : pointer to structure for PARATARE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Tare Table print format.
*
*                : SET TARE TABLE
*
*===========================================================================
*/

VOID PrtThrmSupTare( PARATARE *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupTare1[] = _T("%15u  /   %4.2$%-4s");
    static const TCHAR FARCONST auchPrtThrmSupTare2[] = _T("%15u  /   %5.3$%-4s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupTare1[] = _T("%6u /\t%.2$%-4s");
    static const TCHAR FARCONST auchPrtSupTare2[] = _T("%6u /\t%.3$%-4s");


    /* distinguish minor class */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARATARE_001:    /* minimum unit is 0.01 */             

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ADDRESS/TARE DATA/SPECIAL MNEMONICS */
            
            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupTare1,              /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      ( SHORT)pData->usWeight,          /* tare */
                      pData->aszMnemonics);             /* special mnemonics */
        }

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ADDRESS/TARE DATA/SPECIAL MNEMONICS */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupTare1,                  /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      ( SHORT)pData->usWeight,          /* tare */
                      pData->aszMnemonics);             /* special mnemonics */
        }
        break;

    case CLASS_PARATARE_0001:   /* minimum unit is 0.001 or 0.005 */         

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ADDRESS/TARE DATA/SPECIAL MNEMONICS */
            
            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupTare2,              /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      ( SHORT)pData->usWeight,          /* tare */
                      pData->aszMnemonics);             /* special mnemonics */
        }

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ADDRESS/TARE DATA/SPECIAL MNEMONICS */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupTare2,                  /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      ( SHORT)pData->usWeight,          /* tare */
                      pData->aszMnemonics);             /* special mnemonics */
        }
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;                                      
    }
}
/***** End of Source *****/
