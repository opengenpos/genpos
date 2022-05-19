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
* Title       : Thermal Print Supervisor Number/Level Assignment Format  
*             :                                   ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSSLVLT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*             PrtThrmSupSupLevel() : form Supervisor Number/Level Assignment Data     
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

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupSupLevel( PARASUPLEVEL *pData )
*               
*     Input:    *pData      : pointer to structure for PARASUPLEVEL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms SUPERVISOR NO./LEVEL ASSIGNMENT print format.
*
*                : SUPERVISOR NUMBER/LEVEL ASSIGNMENT
*
*===========================================================================
*/

VOID PrtThrmSupSupLevel( PARASUPLEVEL *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupSupLevel[] = _T("%18u  /  %3u");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupSupLevel[] = _T("%8u / %3u");


    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS / SUPERVISOR No. */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupSupLevel,           /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  pData->usSuperNumber);            /* supervisor number */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* print ADDRESS / SUPERVISOR No. */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupSupLevel,               /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  pData->usSuperNumber);            /* supervisor number */
    }
}
/***** End of Source *****/
