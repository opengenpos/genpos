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
* Title       : Print Tax Rate Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTXRT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupTaxRate()     : This Function Forms Tax Rate Table Data     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
* Jul-23-93: 01.00.12 : J.IKEDA   : Maint. TAX EXEMPT LIMIT Format (%4lu->%5lu)                                   
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
** Synopsis:    VOID PrtSupTaxRate( PARATAXRATETBL *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARATAXRATETBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Tax Rate Table Data.
*===========================================================================
*/

VOID PrtSupTaxRate( PARATAXRATETBL *pData )
{

    static const TCHAR FARCONST auchPrtSupTaxRate1[] = _T("%6u / \t%.4l$%%");
    static const TCHAR FARCONST auchPrtSupTaxRate2[] = _T("%6u / \t%5lu");

    USHORT      usPrtType;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Distinguish Mainor Calss */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARATAXRATETBL_RATE:             /* Tax Rate */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupTaxRate1,           /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulTaxRate);            /* Tax Rate */
        break;

    case CLASS_PARATAXRATETBL_LIMIT:            /* Exempt Limit */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupTaxRate2,           /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulTaxRate);            /* Tax Exempt */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
