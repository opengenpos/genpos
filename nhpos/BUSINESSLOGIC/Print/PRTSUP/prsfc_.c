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
* Title       : Print Foreign Currency Conversion Rate Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFC_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupCurrency()    : This Function Forms Foreign Currency Conversion Rate  
*                                                
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
* Nov-19-92: 01.00.00 : K.You     : Chg Decimal Point from 6 to 5                                   
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
** Synopsis:    VOID PrtSupCurrency( PARACURRENCYTBL *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARACURRENCYTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Foreign Currency Conversion Rate.
*===========================================================================
*/

VOID PrtSupCurrency( PARACURRENCYTBL *pData )
{

    static const TCHAR FARCONST auchPrtSupCurrency[] = _T("%8u / \t%.*l$");

    USHORT      usPrtType;
    SHORT       sDecPoint;

    /* V3.4 */
    if (pData->uchStatus & MAINT_6DECIMAL_DATA) {

        sDecPoint = MAINT_6DECIMAL;

    } else {

        sDecPoint = MAINT_5DECIMAL;
    }
    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupCurrency,                   /* Format */
              ( USHORT)pData->uchAddress,           /* Address */
              sDecPoint,
              pData->ulForeignCurrency);            /* Currency Rate */
}
