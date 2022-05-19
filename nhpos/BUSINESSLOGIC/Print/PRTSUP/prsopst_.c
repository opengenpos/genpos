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
* Title       : Print Cashier/Server/GC No. for Operator/Guest Check Status 
*             : Report( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSOPST_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupOpeStatus() : form Cashier/Server/GC No print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-25-92: 01.00.00 : K.You     : initial                                   
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
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <rfl.h>

#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupOpeStatus( MAINTOPESTATUS *pData )
*               
*     Input:    *pData          : pointer to structure for MAINTOPESTATUS          
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function form Cashier/Server/GC No print format.
*===========================================================================
*/

VOID  PrtSupOpeStatus( MAINTOPESTATUS *pData )
{
    const TCHAR FARCONST *auchPrtSupOpeStatus1 = _T("   %8.8Mu");
    const TCHAR FARCONST *auchPrtSupOpeStatus2 = _T("   %4.4u");

    USHORT usPrtType;

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

	if (pData->uchMinorClass == 1)
		PmgPrintf(usPrtType, auchPrtSupOpeStatus1, RflTruncateEmployeeNumber(pData->ulOperatorId));
	else
		PmgPrintf(usPrtType, auchPrtSupOpeStatus2, pData->ulOperatorId);

}

