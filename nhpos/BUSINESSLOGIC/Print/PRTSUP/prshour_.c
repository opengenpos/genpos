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
* Title       : Print Hourly Activity Time Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSHOUR_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupHourly()      : This Function Forms Hourly Activity Time      
*                                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
** Synopsis:    VOID PrtSupHourly( PARAHOURLYTIME *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAHOURLYTIME
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Hourly Activity Time.
*===========================================================================
*/

VOID PrtSupHourly( PARAHOURLYTIME *pData )
{

    static const TCHAR FARCONST auchPrtSupHourly1[] = _T("%8u / %2u");
    static const TCHAR FARCONST auchPrtSupHourly2[] = _T("%8u / %2u:%02u");

    USHORT      usPrtType;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Distinguish Mainor Calss */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARAHOURLYTIME_BLOCK:            /* Number of Hourly Block */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupHourly1,            /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  ( USHORT)pData->uchBlockNo);  /* Block No. */
        break;

    case CLASS_PARAHOURLYTIME_TIME:             /* Hourly Activity Time */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupHourly2,            /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  ( USHORT)pData->uchHour,      /* Hour */
                  ( USHORT)pData->uchMin);      /* Minute */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
