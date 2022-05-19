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
* Title       : Thermal Print Mnemonics Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTRNT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupTrans() : Transaction Mnemonics 
*                                   Special Mnemonics     print format
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
** Synopsis:    VOID  PrtThrmSupTrans( MAINTTRANS *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTTRANS
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Transaction Mnemonics 
*                                   Special Mnemonics     print format.
*
*                    : OPERATOR/GUEST CHECK STATUS REPORT
*                    : "ABORTED"
*                    : "ADDITION"
*                    : "DELETION"
*                    : TRAINING ID for EMERGENT WAITER FILE CLOSE,
*                                      CASHIER/WAITER RESET REPORT -FUNCTION TYPE 3-
*
*===========================================================================
*/

VOID  PrtThrmSupTrans( MAINTTRANS *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupTrans[] = _T("%s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupTrans[] = _T("%s");

    /* check print control */

    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print MNEMONIC */

        PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                  auchPrtThrmSupTrans,          /* format */
                  pData->aszTransMnemo);        /* mnemonics */
    } 
    
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */    

        /* print MNEMONIC */

        PrtPrintf(PMG_PRT_JOURNAL,              /* printer type */
                  auchPrtSupTrans,              /* format */
                  pData->aszTransMnemo);        /* mnemonics */
    }
}

VOID  PrtThrmSupDspfline( MAINTDSPFLINE *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupDspfline[] = _T("%s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupDspfline[] = _T("%s");

    /* check print control */

    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print MNEMONIC */

        PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                  auchPrtThrmSupDspfline,          /* format */
                  pData->aszMnemonics);        /* mnemonics */
    } 
    
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */    

        /* print MNEMONIC */

        PrtPrintf(PMG_PRT_JOURNAL,              /* printer type */
                  auchPrtSupDspfline,              /* format */
                  pData->aszMnemonics);        /* mnemonics */
    }
}

/***** End of Source *****/
