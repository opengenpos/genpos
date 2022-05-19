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
* Title       : Thermal Print Hourly Activity Time Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSHOURT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupHourly() : form Hourly Activity Time print format     
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
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
/* #include <cswai.h> */
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupHourly( PARAHOURLYTIME *pData )
*               
*     Input:    *pData      : pointer to structure for PARAHOURLYTIME
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Hourly Activity Time print format.
*
*                : HOURLY ACTIVITY TIME
*
*===========================================================================
*/

VOID PrtThrmSupHourly( PARAHOURLYTIME *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupHourly1[] = _T("%18u  /  %2u");
    static const TCHAR FARCONST auchPrtThrmSupHourly2[] = _T("%18u  /  %2u:%02u");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupHourly1[] = _T("%8u / %2u");
    static const TCHAR FARCONST auchPrtSupHourly2[] = _T("%8u / %2u:%02u");

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* distinguish major class */ 
    
        switch(pData->uchMinorClass) {
        case CLASS_PARAHOURLYTIME_BLOCK:            /* number of hourly block */

            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupHourly1,        /* format */
                      ( USHORT)pData->uchAddress,   /* address */
                      ( USHORT)pData->uchBlockNo);  /* block number */
            break;
       
        case CLASS_PARAHOURLYTIME_TIME:             /* hourly activity time */
       
            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupHourly2,        /* format */
                      ( USHORT)pData->uchAddress,   /* address */
                      ( USHORT)pData->uchHour,      /* hour */
                      ( USHORT)pData->uchMin);      /* minute */
            break;

        default:
/*            PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
            return;
        }
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* distinguish major class */ 
    
        switch(pData->uchMinorClass) {
        case CLASS_PARAHOURLYTIME_BLOCK:            /* number of hourly block */
        
            PrtPrintf(PMG_PRT_JOURNAL,              /* printer Type */
                      auchPrtSupHourly1,            /* format */
                      ( USHORT)pData->uchAddress,   /* address */
                      ( USHORT)pData->uchBlockNo);  /* block number */
            break;
        
        case CLASS_PARAHOURLYTIME_TIME:             /* hourly activity time */
        
            PrtPrintf(PMG_PRT_JOURNAL,              /* printer type */
                      auchPrtSupHourly2,            /* format */
                      ( USHORT)pData->uchAddress,   /* address */
                      ( USHORT)pData->uchHour,      /* hour */
                      ( USHORT)pData->uchMin);      /* minute */
            break;
        
        default:
/*            PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
            break;                                      
        }
    }
}
/***** End of Source *****/
