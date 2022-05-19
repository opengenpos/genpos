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
* Title       : Thermal Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDRVTT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*                     PrtThrmSupFxDrive() : form common print format    
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-20-95: 03.00.00 : T.Satoh   : Initial                                   
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
============================================================================
                       I N C L U D E     F I L E s                         
============================================================================
**/

#include	<tchar.h>
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
/* #include <cswai.h> */
#include <maint.h> 
#include <csop.h>
#include <report.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupFxDrive( PARAFXDRIVE *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function format data.
*===========================================================================
*/

VOID PrtThrmSupFxDrive( PARAFXDRIVE *pData )
{
    static const TCHAR FARCONST auchPrtSupFxDrv[] = _T("     %4u / %4u");
    static const TCHAR FARCONST auchPrtSupFxDrvThrm[] = _T("       %8u   /   %8u");
    USHORT                      usPrtType;
    USHORT                      usPrintControl;
    USHORT                      i;

    usPrintControl = pData->usPrintControl;
    i = (USHORT)(pData->uchTblAddr - 1);

    /*
     * Check print control and Print Data
     */
    switch(pData->usPrintControl){
        case PRT_RECEIPT:
            usPrtType = PMG_PRT_RECEIPT;            /* thermal printer      */

            PrtPrintf(usPrtType,                    /* printer type         */
                      auchPrtSupFxDrvThrm,          /* format               */
                      pData->uchTblAddr,            /* Address              */
                      pData->uchTblData[i]);        /* data                 */
            break;

        case PRT_JOURNAL:
            usPrtType = PMG_PRT_JOURNAL;            /* EJ                   */

            PrtPrintf(usPrtType,                    /* printer type         */
                      auchPrtSupFxDrv,              /* format               */
                      pData->uchTblAddr,            /* Address              */
                      pData->uchTblData[i]);        /* data                 */
            break;

        case PRT_RECEIPT | PRT_JOURNAL:
            usPrtType = PMG_PRT_RECEIPT;

            PrtPrintf(usPrtType,                    /* printer type         */
                      auchPrtSupFxDrvThrm,          /* format               */
                      pData->uchTblAddr,            /* Address              */
                      pData->uchTblData[i]);        /* data                 */

            usPrtType = PMG_PRT_JOURNAL;

            PrtPrintf(usPrtType,                    /* printer type         */
                      auchPrtSupFxDrv,              /* format               */
                      pData->uchTblAddr,            /* Address              */
                      pData->uchTblData[i]);        /* data                 */
            break;

        default:    break;
    }

    return;
}
