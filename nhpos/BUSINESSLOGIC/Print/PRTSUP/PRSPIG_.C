/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Pig Rule Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTXRT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupPigRule()     : This Function Forms Pig Rule Table Data     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-12-93: 01.00.12 : M.Yamamoto: initial                                   
* May-13-00: 01.00.01 : hrkato    : AC128
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
#include <pif.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <appllog.h>

#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupPigRule(PARAPIGRULE *pData)
*               
*     Input:    *pData      : Pointer to Structure for PARAPIGRULE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Pig Rule Table Data.
*===========================================================================
*/
VOID    PrtSupPigRule(PARAPIGRULE *pData)
{
    static const TCHAR FARCONST auchPrtSupPigRule1[] = _T("%6u / \t%8l$");
    static const TCHAR FARCONST auchPrtSupPigRule2[] = _T("%6u / \t%2lu");

    USHORT      usPrtType;

    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Distinguish Mainor Calss */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARAPIGRULE_PRICE:               /* Pig Rule Break Point */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupPigRule1,           /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulPigRule);            /* Pig Rule break Point */
        break;

    case CLASS_PARAPIGRULE_COLIMIT:            /* Pig Rule Counter Limit */

        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupPigRule2,           /* Format */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulPigRule);            /* Pig Rule Counter Limit */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    PrtSupMiscPara(PARAMISCPARA *pData)
*               
*     Input:    *pData      : Pointer to Structure for PARAMISCPARA
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms AC128 (Transaction Limit)
*===========================================================================
*/
VOID    PrtSupMiscPara(PARAMISCPARA *pData)
{
    static const TCHAR FARCONST auchPrtSupMiscPara[] = _T("%6u / \t%8l$");

    USHORT      usPrtType;

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  
    PmgPrintf(usPrtType, auchPrtSupMiscPara, (USHORT)pData->uchAddress, pData->ulMiscPara);
}

/* --- End of Source File --- */