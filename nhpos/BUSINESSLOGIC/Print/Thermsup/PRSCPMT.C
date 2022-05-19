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
* Program Name: PRSCPMT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp. 
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupCpm() : form CPM/EPT Tally print format
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
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <paraequ.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <para.h>
#include <maint.h> 
#include <pmg.h>
#include <cpm.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupCpm( MAINTCPM *pData )
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

VOID  PrtThrmSupCpm( MAINTCPM *pData )
{

    static TCHAR FARCONST auchPrtSupCpm[] = _T("%-5s");

    /* check print control */

    if (pData->uchType == CPM_CHANGE_START) {           /* uifac78.c UISUP_START_CPM */
        if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */


            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupCpm,                    /* format */
					  _T("START"));                         /* Mnemonics */
        } 
    
        if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */


            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupCpm,                    /* format */
                      _T("START"));                         /* Mnemonics */
        }
    } else {
        if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */


            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupCpm,                    /* format */
                      _T("STOP"));                          /* Mnemonics */
        } 
    
        if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */


            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupCpm,                    /* format */
                      _T("STOP"));                          /* Mnemonics */
        }
    }
}
/***** End of Source *****/

