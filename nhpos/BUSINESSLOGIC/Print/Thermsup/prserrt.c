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
* Title       : Thermal Print Error Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSERRT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Error print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupErrorCode() : form Error print format
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
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupErrorCode( MAINTERRORCODE *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTERRORCODE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Error Code print format.
*===========================================================================
*/

VOID  PrtThrmSupErrorCode( MAINTERRORCODE *pData )
{

    /*  define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupError[] = _T("E- %4d");

    /*  define EJ print format */

    static const TCHAR FARCONST auchPrtSupError[] = _T("E- %4d");

    /* check print control */

    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ERROR CODE */

        PrtPrintf(PMG_PRT_RECEIPT,                    /* printer type */
                  auchPrtThrmSupError,                /* format */
                  pData->sErrorCode);                 /* error code */
    }

    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */

        /* print ERROR CODE */

        PrtPrintf(PMG_PRT_JOURNAL,                    /* printer type */
                  auchPrtSupError,                    /* format */
                  pData->sErrorCode);                 /* error code */
    }
}
/***** End of Source *****/
