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
* Title       : Print Electoric Journal Report Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 Int'l Hospitality Application Program        
* Program Name: PRSEJT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupEJ()      : This Function Forms EJ Report Print Format
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-04-93: 00.00.01 : K.you     : Initial                                   
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
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <ej.h>

#include "prtrin.h"
#include "prtsin.h"
#include "prtcom.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupEJ( RPTEJ *pData )
*               
*     Input:    *pData      : Pointer to Structure for RPTEJ
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms EJ Report Print Format.
*===========================================================================
*/

VOID  PrtSupEJ( RPTEJ *pData )
{

    static const TCHAR FARCONST auchPrtSupEJ1[] = _T("\t%6d / %-6d");
    static const TCHAR FARCONST auchPrtSupEJ2[] = _T("    %s");
    static const TCHAR FARCONST auchPrtSupEJ3[] = _T("%s");

	TCHAR	aszEditBuff[56 + 1];

    /* Initialize Buffer */

    memset(aszEditBuff, '\0', sizeof(aszEditBuff));

    switch(pData->uchMinorClass) {
    case CLASS_RPTEJ_PAGE:

        PrtPrintf(PMG_PRT_RECEIPT,                    /* Printer Type */
                  auchPrtSupEJ1,                      /* Format */
                  pData->usPageNo,                    /* EJ Page No. */
                  pData->usTtlPageNo);                /* EJ Total Page No. */
        break;

    case CLASS_RPTEJ_REVERSE:

        PrtDouble(aszEditBuff, TCHARSIZEOF(aszEditBuff), pData->aszLineData);

        PrtPrintf(PMG_PRT_RECEIPT,                    /* Printer Type */
                  auchPrtSupEJ2,                      /* Format */
                  aszEditBuff);                       /* EJ Reverse Data */

        fbPrtTHHead = 1;                              /* header printed */                        
        break;

	case CLASS_RPTEJ_CONFIGURATION:				//For Printing Configuration Information AC888 JHHJ 10-29-04
		
	 PrtPrintf(PMG_PRT_RECEIPT,                    
                  pData->aszLineData,                      
                  aszEditBuff);
		break;

    default:                            /* CLASS_RPTEJ_LINE */

        /* Edit EJ Report Format */


        aszEditBuff[0] = _T(' ');
        _tcsncpy(aszEditBuff + 1, pData->aszLineData, EJ_COLUMN );
        _tcsncpy(aszEditBuff + 1 + EJ_COLUMN, _T("  |  "), _tcslen(_T("  |  ")) - 1);
        _tcsncpy(aszEditBuff + EJ_COLUMN + _tcslen(_T("  |  ")), pData->aszLineData + EJ_COLUMN, EJ_COLUMN);
        PrtPrintf(PMG_PRT_RECEIPT,                    
                  auchPrtSupEJ3,                      
                  aszEditBuff); 
		
        //memcpy(aszEditBuff + 1, pData->aszLineData, EJ_COLUMN);
        //memcpy(aszEditBuff + 1 + EJ_COLUMN, "  |  ", sizeof("  |  ") - 1);
        //memcpy(aszEditBuff + EJ_COLUMN + sizeof("  |  "), pData->aszLineData + EJ_COLUMN, EJ_COLUMN);

        break;
    }
}

