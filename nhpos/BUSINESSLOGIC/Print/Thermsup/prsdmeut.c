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
* Title       : Thermal Print Set Dept No. on Menu Page Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDMEUT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupDeptNoMenu()   : forms Set Dept No. on Menu Page print format     
*                                                               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-26-90: 01.00.12 : M.Ozawa   : Initial                                   
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

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupDeptNoMenu( PARADEPTNOMENU *pData )
*               
*     Input:    *pData      : pointer to structure for PARADEPTNOMENU
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Set Dept No. on Menu Page.
*
*                : SET Dept NO. ON MENU PAGE
*
*===========================================================================
*/

VOID PrtThrmSupDeptNoMenu( PARADEPTNOMENU *pData )
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupDeptNoMenu[] = _T("%12u - %3u  /  %4u");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupDeptNoMenu[] = _T("%4u - %3u / %4u");

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print MENU PAGE-KEYED Dept No./Dept No. */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupDeptNoMenu, ( USHORT)pData->uchMenuPageNumber, ( USHORT)pData->uchMinorFSCData,  pData->usDeptNumber);
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        /* print MENU PAGE-KEYED Dept No./Dept No. */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupDeptNoMenu, ( USHORT)pData->uchMenuPageNumber, ( USHORT)pData->uchMinorFSCData, pData->usDeptNumber);
    }
}
/***** End of Source *****/
