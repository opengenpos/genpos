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
* Title       : Thermal Print Employee No Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSEMPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupEmployeeNo() : forms Employee No Assignment print format
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-30-93: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>
/* #include <pif.h> */
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <csop.h>
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupEmployeeNo( PARAEMPLOYEENO *pData )
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

VOID  PrtThrmSupEmployeeNo( PARAEMPLOYEENO *pData )
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupEmployee[] = _T("%18s\t%02u %02u %02u\n%-16s");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupEmployee[] = _T("%8.8Mu\t%02u %02u %02u\n%-16s");

    /* define thermal/EJ common print format */
    const TCHAR FARCONST *auchNumber = _T("%8.8Mu");

	TCHAR	aszDoubRepoNumb[9 * 2 + 1];
	TCHAR	aszRepoNumb[9 + 1];

    /* convert Employee No. to double wide */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, RflTruncateEmployeeNumber(pData->ulEmployeeNo));
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print EMPLOYEE NO/JOB CODE1/2/3 */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupEmployee, aszDoubRepoNumb,
                  ( USHORT)(pData->uchJobCode1), ( USHORT)(pData->uchJobCode2), ( USHORT)(pData->uchJobCode3), pData->aszMnemonics);
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupEmployee, pData->ulEmployeeNo,
					( USHORT)(pData->uchJobCode1), ( USHORT)(pData->uchJobCode2), ( USHORT)(pData->uchJobCode3), pData->aszMnemonics);
    }
}
/***** End of Source *****/
