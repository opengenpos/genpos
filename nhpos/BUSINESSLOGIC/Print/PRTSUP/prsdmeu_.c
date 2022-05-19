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
* Title       : Print Set Dept No. on Menu Page Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDMEU_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupDeptNoMenu()   : This Function Forms Set Dept No. on Menu Page      
*                                                                
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-26-99: 00.00.01 : M.Ozawa     : initial                                   
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
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupDeptNoMenu( PARADEPTNOMENU *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARADEPTNOMENU
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Set Dept No. on Menu Page.
*===========================================================================
*/

VOID PrtSupDeptNoMenu( PARADEPTNOMENU *pData )
{

    static const TCHAR FARCONST auchPrtSupDeptNoMenu[] = _T("%4u - %3u / %4u");

    USHORT      usPrtType;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupDeptNoMenu,                  /* Format */
              ( USHORT)pData->uchMenuPageNumber,    /* Page Number */
              ( USHORT)pData->uchMinorFSCData,      /* Minor FSC Data */
              pData->usDeptNumber);                  /* Dept Number */

}
