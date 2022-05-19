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
* Title       : Thermal Print Guest Check Number Format ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCNOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK Number thermal print format.
*
*     The provided function names are as follows: 
* 
*          PrtThrmSupGCNo() : form GUEST CHECK Number print format
*                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
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
** Synopsis:    VOID  PrtThrmSupGCNo( PARAGUESTNO *pData )
*               
*     Input:    *pData      : pointer to structure for PARAGUESTNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHECK NO. print format.
*
*                : GUEST CHECK NO. ASSIGNMENT
*
*===========================================================================
*/

VOID  PrtThrmSupGCNo( PARAGUESTNO *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupGCNo[] = _T("%9s%-8.8s   %4u");/*8 characters JHHJ*/

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupGCNo[] = _T("%-6s%-8.8s %4u");/*8 characters JHHJ*/
															

    TCHAR   aszNull[1] = {'\0'};

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print GUEST CHECK No. */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupGCNo,               /* format */
                  aszNull,                          /* null */
                  pData->aszMnemonics,              /* transaction mnemonics */
                  pData->usGuestNumber);            /* guest check number */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* print GUEST CHECK No. */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupGCNo,                   /* format */
                  aszNull,                          /* null */
                  pData->aszMnemonics,              /* transaction mnemonics */
                  pData->usGuestNumber);            /* guest check number */
    }     
}
/***** End of Source *****/
