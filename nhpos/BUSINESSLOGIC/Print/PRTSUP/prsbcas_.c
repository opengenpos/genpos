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
* Title       : Print Terminal No/Register No Format ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSBCAS_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: 
*           PrtSupBcas() : form Terminal Number & Register Number
*                             for Broascast/Request to Download parameter
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Oct-14-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupBcas( MAINTBCAS *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTBCAS
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Terminal Number & Register Numner print format.
*===========================================================================
*/

VOID  PrtSupBcas( MAINTBCAS *pData )
{

    /* define format */

    static const TCHAR FARCONST auchPrtSupBcas[] = _T("%6s#%03u  %03u");
    TCHAR   aszNull[1] = {'\0'};
    USHORT  usPrtType;

    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrtControl);

    PmgPrintf(usPrtType,                        /* Printer Type */
              auchPrtSupBcas,                   /* Format */
              aszNull,                          /* Null */
              ( USHORT)pData->uchTermNo,        /* Terminal Number */
              pData->usRegNo);                  /* Register Number */
}





