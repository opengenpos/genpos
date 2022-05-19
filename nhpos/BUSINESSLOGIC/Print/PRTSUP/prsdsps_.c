/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Beverage Dispenser Parameter Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDSPS_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupDispPara() : form Beverage Dispenser Parameter
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
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupDispPara( PARADISPPARA *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARADISPPARA
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Beverage Dispenser Parameter print format.
*===========================================================================
*/

VOID  PrtSupDispPara( PARADISPPARA *pData )
{

    static const TCHAR FARCONST auchPrtSupDispPara[] = _T("%8u / %5u%3s");       /* Define Print Format */

    UCHAR   auchAdj[5] = {0, 0, 0, 0, 0};
    USHORT  usPrtType;


    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* check if PTD flag is exist */

    if (pData->usAddress < DISPENS_OFFSET_ADR) {
        auchAdj[0] = '-';
        _itoa(pData->uchAdjective, &auchAdj[1], 10);
    }

    /* Print Line */

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupDispPara,                   /* Format */
              pData->usAddress,                     /* Address */
              pData->usPluNumber,                   /* PLU Number */
              auchAdj);                             /* Adjective */

}

