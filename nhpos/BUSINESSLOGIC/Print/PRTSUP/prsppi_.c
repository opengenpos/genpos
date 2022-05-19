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
* Title       : Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPPI_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtSupPPI( VOID *pData );     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-95: 03.01.00 : M.Ozawa   : initial                                   
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
#include <fsc.h>
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupPPI( PARAPPI *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function format data.
*===========================================================================
*/

VOID PrtSupPPI( PARAPPI *pData )
{
    static const TCHAR FARCONST auchPrtSupPPIA[] = _T("%4u %4u /\t%3u-%6lu");
    static const TCHAR FARCONST auchPrtSupPPIB[] = _T("     %4u /\t%3u-%6lu");

    USHORT i;

    /* print first line with PPI number */
    
    PmgPrintf(pData->usPrintControl,    /* printer type */
              auchPrtSupPPIA,           /* format */
              pData->uchPPINumber,      /* PPI Number */
              1,                        /* Address */
              pData->ParaPPI[0].uchQTY,   /* QTY Data */
              pData->ParaPPI[0].ulPrice); /* Price Data */

     for (i=1; i<PPI_ADR_MAX; i++) {

        if (pData->ParaPPI[i].uchQTY == 0) break;

        PmgPrintf(pData->usPrintControl,    /* printer type */
                  auchPrtSupPPIB,           /* format */
                  i+1,                      /* Address */
                  pData->ParaPPI[i].uchQTY,   /* QTY Data */
                  pData->ParaPPI[i].ulPrice); /* Price Data */
     }

}

/*** End of File ***/
