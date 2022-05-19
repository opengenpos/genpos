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
* Title       : Print Employee No Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSEMP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupEmployeeNo() : forms Employee No Assignment print format
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-30-93: 01.00.12 : J.IKEDA   : Initial                                   
* Nov-27-95: 03.01.00 : M.Ozawa   : Change print format
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
#include <string.h>
#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupEmployeeNo( PARAEMPLOYEENO *pData )
*               
*     Input:    *pData      : pointer to structure for PARAEMPLOYEENO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Employee No. Assignment print format.
*
*                    : EMPLOYEE No. ASSIGNMENT
*
*===========================================================================
*/

VOID  PrtSupEmployeeNo( PARAEMPLOYEENO *pData )
{

    /* define print format */

    static const TCHAR FARCONST auchPrtSupEmployee[] = _T("%8.8Mu\t%02u %02u %02u\n%-16s");

    USHORT  usPrtType;

    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* print EMPLOYEE NO/JOB CODE1/2/3 */

    PmgPrintf(usPrtType, auchPrtSupEmployee, RflTruncateEmployeeNumber(pData->ulEmployeeNo), ( USHORT)(pData->uchJobCode1), ( USHORT)(pData->uchJobCode2), ( USHORT)(pData->uchJobCode3), pData->aszMnemonics);
}
/***** End of Source *****/
