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
* Title       : Print Flexible Memory Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFLEX_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupFlexMem() : form Flexible Memory Assignment
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-02-92: 00.00.01 : K.You     : initial                                   
* Jul-05-93: 01.00.12 : J.IKEDA   : Adds ETK function                                   
* Mar-22-94: 00.00.04 : K.You     : Adds flex GC file function                                   
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
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupFlexMem( PARAFLEXMEM *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARACASHIERNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Flexible Memory Assignment print format.
*===========================================================================
*/

VOID  PrtSupFlexMem( PARAFLEXMEM *pData )
{

    static const TCHAR FARCONST auchPrtSupFlexMem[] = _T("%6u / %6lu %1s");  /* Define Print Format */

    TCHAR   auchPTD[4] = {0, 0, 0, 0};
    USHORT  usPrtType;


    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* check if PTD flag is exist */

/*    if (pData->uchAddress != FLEX_CAS_ADR     ,V3.3*/
    if (pData->uchAddress != FLEX_ETK_ADR
        && pData->uchAddress != FLEX_ITEMSTORAGE_ADR
        && pData->uchAddress != FLEX_CONSSTORAGE_ADR
        && pData->uchAddress != FLEX_CSTRING_ADR
        && pData->uchAddress != FLEX_PROGRPT_ADR
        && pData->uchAddress != FLEX_PPI_ADR) {
        _itot(pData->uchPTDFlag, auchPTD, 10);
    }

    /* Print Line */

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupFlexMem,                    /* Format */
              ( USHORT)(pData->uchAddress),         /* Address */
              pData->ulRecordNumber,                /* Number of Record */
              auchPTD);                             /* PTD Flag */

}

