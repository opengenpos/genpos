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
* Title       : Print(21CHAR) Employee File Maintenance Format ( SUPER & PROGRAM MODE ) 
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSETKFS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupMaintETKFl_s()    : ETK print format.
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Oct-04-93 : 02.00.01 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>
#include <string.h>
#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <csetk.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupMaintETKFl_s( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms ETK File print format.
*
*             : EMPLOYEE TIME KEEPING FILE MAINTENANCE  
*
*===========================================================================
*/

VOID  PrtSupMaintETKFl_s( MAINTETKFL *pData )  
{

    /* define print format */
    
    static const TCHAR FARCONST auchPrtSupMaintETKFl[] = _T("%4s %2s %5s %7s\n\t%7s");
    static const TCHAR FARCONST auchNumber[] = _T("%d");

    TCHAR   aszBuffer[3] = { 0, 0, 0 };
	TCHAR   aszDoubBlockNo[2 * 2 + 1] = { 0 };
	TCHAR   aszBlockNo[2 + 1] = { 0 };
    USHORT  usPrtType;

    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* make data */

    PrtSupMakeEtkFl(pData);

    if(pData->EtkField.uchJobCode) {
        _itot(pData->EtkField.uchJobCode, aszBuffer, 10);
    }
       
    /* convert Block No. to double wide */
    
    RflSPrintf(aszBlockNo, TCHARSIZEOF(aszBlockNo), auchNumber, pData->usBlockNo);
    PrtDouble(aszDoubBlockNo, (2 * 2+ 1), aszBlockNo);

    PmgPrintf(usPrtType,                   /* printer type */
              auchPrtSupMaintETKFl,        /* format */
              aszDoubBlockNo,              /* block number */
              aszBuffer,                   /* job code */
              aszPrtDate,                  /* date */
              aszPrtInTime,                /* time-in */
              aszPrtOutTime);              /* time-out */
} 
/***** End of Source *****/
