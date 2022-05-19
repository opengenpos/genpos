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
* Program Name: PrsRestT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               PrtSupThermRest()   : This Function Forms Sales Restriction Table
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
=============================================================================
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
** Synopsis:    VOID PrtThermSupRes( PARARESTRICTION *pData )
*
*     Input:    *pData      : Pointer to Structure for PARARESTRICTION
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Set Sales Restriction Table.
*===========================================================================
*/

VOID PrtThermSupRest( PARARESTRICTION *pData )
{
    /* Define print format */
    static const TCHAR FARCONST auchPrtThermSupRest[] = _T("              %1u - %1u / %2u");
    static const TCHAR FARCONST auchPrtSupRest[] = _T("       %2u - %1u / %2u");


    /* Check Print Control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* Print 1st. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              1,                            /* Field        */
              (USHORT)pData->uchDate);      /* Date         */

        /* Print 2nd. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              2,                            /* Field        */
              (USHORT)pData->uchDay);       /* Day of Week  */

        /* Print 3rd. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              3,                            /* Field        */
              (USHORT)pData->uchHour1);     /* start time   */

        /* Print 4th. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              4,                            /* Field        */
              (USHORT)pData->uchMin1);      /* start time   */

        /* Print 5th. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              5,                            /* Field        */
              (USHORT)pData->uchHour2);     /* end time     */

        /* Print 6th. line */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
              auchPrtThermSupRest,             /* Format       */
              pData->uchAddress,            /* Address      */
              6,                            /* Field        */
              (USHORT)pData->uchMin2);      /* end time     */
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */                                        /* EJ */
    
        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                  pData->uchAddress,                    /* Address */
                  1,                                    /* Field */
                  ( USHORT)pData->uchDate);             /* Date */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 2,                                     /* Field */
                ( USHORT)pData->uchDay);                /* Day of Week */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 3,                                     /* Field */
                ( USHORT)pData->uchHour1);              /* start time */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 4,                                     /* Field */
                ( USHORT)pData->uchMin1);              /* start time */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 5,                                     /* Field */
                ( USHORT)pData->uchHour2);              /* end time */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupRest,                       /* Format */
                 pData->uchAddress,                     /* Address */
                 6,                                     /* Field */
                ( USHORT)pData->uchMin2);               /* end time */
    }
}


/* === End of PrsRestT.C === */
