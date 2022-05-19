/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Data Setting Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 GP R2.0 Apllication Program
* Program Name: PrsAge_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*
*           The provided function names are as follows:
*
*                 VOID PrtSupAge( VOID *pData );
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* FEB-25-94: 00.00.01 : M.INOUE   : initial
* Mar-26-97: 02.00.00 : S.Kubota  : Change for Thermal printer
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
** Synopsis:    VOID PrtSupAge( PARABOUNDAGE *pData )
*
*     Input:    *pData
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function
*===========================================================================
*/

VOID PrtSupAge( PARABOUNDAGE *pData )
{
    /* Define print format */
    static const TCHAR FARCONST auchPrtSupAge[] = _T("%8u / %3d");

    USHORT  usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    PmgPrintf(usPrtType,                    /* printer type */
              auchPrtSupAge,                /* format       */
              ( USHORT)pData->uchAddress,   /* address      */
              pData->uchAgePara);           /* boundary age parameter */

}



/* === End of PrsAge_.C === */
