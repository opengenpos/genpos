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
* Title       : Print Store/Register No. Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSSTO_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtsupStoRegNo()  : This Function Forms Store/Register No. Data     
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-19-92: 00.00.01 : K.You     : initial                                   
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
** Synopsis:    VOID PrtsupStoRegNo( PARASTOREGNO *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARASTOREGNO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Store/Register No. Data.
*===========================================================================
*/

VOID PrtSupStoRegNo( PARASTOREGNO *pData )
{

    static const TCHAR FARCONST auchPrtSupStoRegNo[] = _T("%8u / %04u%03u");

    USHORT      usPrtType;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    PmgPrintf(usPrtType,                    /* Printer Type */
              auchPrtSupStoRegNo,           /* Format */
              ( USHORT)pData->uchAddress,   /* Address */
              pData->usStoreNo,             /* Store No. */
              pData->usRegisterNo);         /* Register No. */

}
