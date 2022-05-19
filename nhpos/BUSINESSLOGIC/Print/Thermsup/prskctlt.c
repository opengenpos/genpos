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
* Title       : Print Total Key Control Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSKCTLT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupTtlKeyCtl()   : form Total Key Control Data
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.IKEDA   : initial                                   
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
** Synopsis:    VOID  PrtThrmSupTtlKeyCtl( PARATTLKEYCTL *pData )
*               
*     Input:    *pData      : pointer to structure for PARATTLKEYCTL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Total Key Control data print format.
*
*                : TOTAL KEY CONTROL
*
*===========================================================================
*/

VOID  PrtThrmSupTtlKeyCtl( PARATTLKEYCTL *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupTtlKeyCtl[] = _T("%13u - %2u  /  %s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupTtlKeyCtl[] = _T("%5u - %2u / %s");

    TCHAR           aszBuffer[5];


    /* convert status data to binary ASCII data */

    PrtSupItoa(pData->uchTtlKeyMDCData, aszBuffer);             
                                                                
    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS/FILED NO./TOTAL KEY DATA */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupTtlKeyCtl,          /* format */
                  ( USHORT)pData->uchAddress,       /* address data */
                  ( USHORT)pData->uchField,         /* field data */
                  aszBuffer);                       /* status */

    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* THERMAL PRINTER */

        /* print ADDRESS/FILED NO./TOTAL KEY DATA */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupTtlKeyCtl,              /* Format */
                  ( USHORT)pData->uchAddress,       /* address data */
                  ( USHORT)pData->uchField,         /* field data */
                  aszBuffer);                       /* status */
    }
}
/***** End of Source *****/
