/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Preset Amount Cash Tender Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPAMT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
*                                       
*               PrtSupPresetAmount()    : This Function Forms Preset Amount Cash Tender
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-02-93: 01.00.12 : K.You     : initial                                   
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
** Synopsis:    VOID PrtSupPresetAmount( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Preset Amount Cash Tender.
*===========================================================================
*/

VOID PrtSupPresetAmount( PARAPRESETAMOUNT *pData )
{

    static const TCHAR FARCONST auchPrtSupPresetAmount[] = _T("%8u / %8l$");

    USHORT      usPrtType;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    PmgPrintf(usPrtType,                    /* Printer Type */
              auchPrtSupPresetAmount,       /* Format */
              ( USHORT)pData->uchAddress,   /* Address */
              pData->ulAmount);             /* Preset Amount */
}
