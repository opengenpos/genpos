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
* Title       : Thermal Print Flexible Memory Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFLEXT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupFlexMem() : form Flexible Memory Assignment print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
* Mar-22-94: 00.00.04 : K.You     : Adds flex GC file function
* Aug-07-95: 03.00.04 : M.Ozawa   : Adds control string function
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
#include <stdlib.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupFlexMem( PARAFLEXMEM *pData )
*               
*     Input:    *pData      : pointer to structure for PARAFLEXMEM
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Flexible Memory Assignment print format.
*
*                : FLEXIBLE MEMORY ASSIGNMENT
*
*===========================================================================
*/

VOID  PrtThrmSupFlexMem( PARAFLEXMEM *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupFlexMem[] = _T("%18u  /  %6lu %1s");       

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupFlexMem[] = _T("%8u / %6lu %1s");       

    TCHAR   auchPTD[4] = {0, 0, 0, 0};

    /* check if PTD plag is exist */

/*    if (pData->uchAddress != FLEX_CAS_ADR     ,V3.3 */
      if (pData->uchAddress != FLEX_ETK_ADR
        && pData->uchAddress != FLEX_ITEMSTORAGE_ADR
        && pData->uchAddress != FLEX_CONSSTORAGE_ADR
        && pData->uchAddress != FLEX_CSTRING_ADR
        && pData->uchAddress != FLEX_PROGRPT_ADR
        && pData->uchAddress != FLEX_PPI_ADR) { 

        /* convert PTD flag data to binary ASCII data */    

        _itot(pData->uchPTDFlag, auchPTD, 10);
    }        

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupFlexMem,            /* format */
                  ( USHORT)(pData->uchAddress),     /* address */
                  pData->ulRecordNumber,            /* number of record */
                  auchPTD);                         /* PTD flag */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupFlexMem,                /* format */
                  ( USHORT)(pData->uchAddress),     /* address */
                  pData->ulRecordNumber,            /* number of record */
                  auchPTD);                         /* PTD flag */
    }
}
/***** End of Source *****/

