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
* Title       : Thermal Print Beverage Dispenser Parameter Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDSPST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupDispPara() : form Beverage Dispenser Parameter print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-02-96: 03.01.00 : M.Ozawa   : initial
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupDispPara( PARADISPPARA *pData )
*               
*     Input:    *pData      : pointer to structure for PARADISPPARA
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Beverage Dispenser Parameter print format.
*
*===========================================================================
*/

VOID  PrtThrmSupDispPara( PARADISPPARA *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupDispPara[] = _T("%18u  /  %5u%3s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupDispPara[] = _T("%8u / %5u%2s");

    TCHAR   auchAdj[5] = {0, 0, 0, 0, 0};

    /* check if adjective is exist */

    if (pData->usAddress < DISPENS_OFFSET_ADR) {
        auchAdj[0] = _T('-');
        _itot(pData->uchAdjective, &auchAdj[1], 10);
    }

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupDispPara,           /* format */
                  pData->usAddress,                     /* Address */
                  pData->usPluNumber,                   /* PLU Number */
                  auchAdj);                             /* Adjective */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDispPara,               /* format */
                  pData->usAddress,                     /* Address */
                  pData->usPluNumber,                   /* PLU Number */
                  auchAdj);                             /* Adjective */
    }
}
/***** End of Source *****/

