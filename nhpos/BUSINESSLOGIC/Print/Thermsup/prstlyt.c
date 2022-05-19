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
* Title       : Thermal Print CPM/EPT Tally Format ( SUPER & PROGRAM MODE )
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
*               PrtThrmSupTally() : form CPM/EPT Tally print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov.15.93: 02.00.01 :M.Yamamoto : Init.
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
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <para.h>
#include <maint.h> 
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupTally( PARACPMTALLY *pData )
*               
*     Input:    *pData      : pointer to structure for PARAFLEXMEM
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CPM/EPT Tally Print Element.
*===========================================================================
*/

VOID  PrtThrmSupTally( PARACPMTALLY *pData )
{

    TCHAR  auchData[6] = {0,0,0,0,0,0};             /* ushort to decimal */
    /* define thermal print format */

    static TCHAR FARCONST auchPrtSupTally[] = _T("%-15s                      %5s");

    /* convert hex to decimal */

    _ultot( (ULONG)pData->usTallyData, auchData, 10);

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtSupTally,                  /* format */
                  &pData->auchMnemo,                /* Mnemonics */
                  auchData);                        /* data */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */

        /* print ADDRESS / No. of RECORD / PTD FLAG */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupTally,                  /* format */
                  &pData->auchMnemo,                /* Mnemonics */
                  auchData);                        /* data */
    }
}
/***** End of Source *****/

