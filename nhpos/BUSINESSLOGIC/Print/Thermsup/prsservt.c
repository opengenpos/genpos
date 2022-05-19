
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
* Title       : Thermal Print Service Time Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSSERVT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupServiceTime() : form Service Time print format     
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
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupServiceTime( RPTSERVICE *pData )
*               
*     Input:    *pData      : pointer to structure for RPTSERVICE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Service Time print format.
*
*                : SERVICE TIME
*
*===========================================================================
*/

VOID PrtThrmSupServiceTime( RPTSERVICE *pData )
{

    /* define thermal print format */
#if defined(DCURRENCY_LONGLONG)
	static const TCHAR FARCONST auchPrtThrmSupService1[] = _T("#%1u\t%5lu  %9lu.%02u");
	static const TCHAR FARCONST auchPrtThrmSupService2[] = _T("\t%5lu  %9lu.%02u");
#else
    static const TCHAR FARCONST auchPrtThrmSupService1[] = _T("#%1u\t%5u  %9lu.%02u");
    static const TCHAR FARCONST auchPrtThrmSupService2[] = _T("\t%5u  %9lu.%02u");
#endif

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* distinguish major class */ 
    
        switch(pData->uchMinorClass) {
        case CLASS_RPTSERVICE_TTL:                  /* each service total element */

            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupService1,        /* format */
                      pData->sElement,              /* Element # */
                      pData->sCounter,              /* Sales Counter */
                      pData->lMinute,               /* Service Time(minute) */
                      ( USHORT)pData->chSecond);    /* Service Time(second) */
            break;
       
        case CLASS_RPTSERVICE_SEGTTL:               /* service total */
       
            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupService2,       /* format */
                      pData->sCounter,              /* Sales Counter */
                      pData->lMinute,               /* Service Time(minute) */
                      ( USHORT)pData->chSecond);    /* Service Time(second) */
            break;

        default:
            return;
        }
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        return;
    }
}
/***** End of Source *****/
