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
* Title       : Print Service Time Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSSERV_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupServiceTime()   : This Function Forms Service Time      
*                                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-05-95: 03.01.00 : M.Ozawa   : initial                                   
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
/* #include <log.h> */
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
** Synopsis:    VOID PrtSupServiceTime( RPTSERVICE *pData )
*               
*     Input:    *pData      : Pointer to Structure for RPTSERVICE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Service Time Total
*===========================================================================
*/

VOID PrtSupServiceTime( RPTSERVICE *pData )
{

    static const TCHAR FARCONST auchPrtSupService1[] = _T("#%1u      %5u\t%lu.%02u");
    static const TCHAR FARCONST auchPrtSupService2[] = _T("        %5u\t%lu.%02u");

    USHORT      usPrtType;


    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Distinguish Mainor Calss */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_RPTSERVICE_TTL:                  /* each service total element */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupService1,           /* Format */
                  pData->sElement,              /* Element # */
                  pData->sCounter,              /* Sales Counter */
                  pData->lMinute,               /* Service Time(minute) */
                  ( USHORT)pData->chSecond);    /* Service Time(second) */
        break;

    case CLASS_RPTSERVICE_SEGTTL:               /* service total */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupService2,           /* Format */
                  pData->sCounter,              /* Sales Counter */
                  pData->lMinute,               /* Service Time(minute) */
                  ( USHORT)pData->chSecond);    /* Service Time(second) */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
