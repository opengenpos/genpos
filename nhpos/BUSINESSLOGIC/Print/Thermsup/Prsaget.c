/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Data Setting Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 GP R2.0 Apllication Program
* Program Name: PrsAgeT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*
*           The provided function names are as follows:
*
*                 VOID PrtThermSupAge( VOID *pData );
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-25-97: 02.00.04 : S.Kubota  : Initial
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
** Synopsis:    VOID PrtThermSupAge( PARABOUNDAGE *pData )
*
*     Input:    *pData
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function formats data for Boundary Age parameter.
*===========================================================================
*/

VOID PrtThermSupAge( PARABOUNDAGE *pData )
{
    /* Define print format */
    static const TCHAR FARCONST auchPrtThermSupAge[] = _T("%8u / %3d");
    static const TCHAR FARCONST auchPrtSupAge[] = _T("%8u / %3d");

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThermSupAge,              /* format       */
                  ( USHORT)pData->uchAddress,   /* address      */
                  pData->uchAgePara);           /* boundary age parameter */

    }
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */
 
        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupAge,                /* format       */
                  ( USHORT)pData->uchAddress,   /* address      */
                  pData->uchAgePara);           /* boundary age parameter */
    }
}


/* === End of PrsAgeT_.C === */
