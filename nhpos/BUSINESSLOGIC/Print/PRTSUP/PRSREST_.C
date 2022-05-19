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
* Title       : Print Sales Restriction Table Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 GP R2.0 Application Program
* Program Name: PRSREST_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               PrtSupRest()    :
*               PrtSupRest_RJ() : This Function Forms Sales Restriction Table
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Apr-20-93: 00.00.01 : M.Ozawa   : initial
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
#include <csop.h>
#include <report.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupRest( PARARESTRICTION *pData )
*
*     Input:    *pData      : Pointer to Structure for PARARESTRICTION
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Set Sales Restriction Table.
*===========================================================================
*/

VOID PrtSupRest( PARARESTRICTION *pData )
{
    static const TCHAR FARCONST auchPrtSupRest[] = _T("       %2u - %1u / %2u");

    USHORT      usPrtType;

    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                  pData->uchAddress,                    /* Address */
                  1,                                    /* Field */
                  ( USHORT)pData->uchDate);             /* Date */

    PmgPrintf(usPrtType,                             /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 2,                                     /* Field */
                ( USHORT)pData->uchDay);                /* Day of Week */

    PmgPrintf(usPrtType,                             /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 3,                                     /* Field */
                ( USHORT)pData->uchHour1);              /* start time */

    PmgPrintf(usPrtType,                             /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 4,                                     /* Field */
                ( USHORT)pData->uchMin1);              /* start time */

    PmgPrintf(usPrtType,                             /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 5,                                     /* Field */
                ( USHORT)pData->uchHour2);              /* end time */

    PmgPrintf(usPrtType,                             /* Printer Type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 6,                                     /* Field */
                ( USHORT)pData->uchMin2);               /* end time */


}
