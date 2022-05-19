/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Flexible Drive Through Parameter ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDRVT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtSupFxDrive( PARAFXDRIVE *pData )
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-17-95: 03.00.00 : T.Satoh   : initial                                   
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
#include <csop.h>
#include <report.h>
#include <maint.h>
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupFxDrive( PARAFXDRIVE *pData )
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
VOID PrtSupFxDrive( PARAFXDRIVE *pData )
{
    static const TCHAR FARCONST auchPrtSupFxDrive[] = _T("     %4u / %4u");
    USHORT                      usPrtType;
    USHORT                      usPrintControl;
    UCHAR                       i;

    usPrintControl = pData->usPrintControl;

    /*
     * check print control
     */
    switch(pData->usPrintControl) {
    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    i = (UCHAR)(pData->uchTblAddr - 1);             /* Set Table Index      */

    PmgPrintf(usPrtType,                            /* printer type         */
              auchPrtSupFxDrive,                    /* format               */
              pData->uchTblAddr,                    /* Table Address        */
              pData->uchTblData[i]);                /* Table data[]         */

    return;
}
