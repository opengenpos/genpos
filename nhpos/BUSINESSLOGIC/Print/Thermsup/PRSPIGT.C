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
* Program Name: PRSPIGT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
*                                       
*               PrtThrmSupPigRule()     : forms Pig Rule Table print format     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-16-93: 01.00.12 : K.You     : initial                                   
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
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "paraequ.h"
#include "para.h"
#include "csttl.h"
#include "cswai.h"
#include "csop.h"
#include "report.h"
#include "maint.h"
#include "regstrct.h"
#include "transact.h"
#include "pmg.h"
#include "appllog.h"

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupPigRule( PARAPIGRULE *pData )
*               
*     Input:    *pData      : pointer to structure for PARAPIGRULE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Pig Rule Table data print format.
*
*                : SET PIG RULE TABLE
*
*===========================================================================
*/

VOID PrtThrmSupPigRule( PARAPIGRULE *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupPigRule1[] = _T("%15u  /  %8l$");
    static const TCHAR FARCONST auchPrtThrmSupPigRule2[] = _T("%15u  /  %8lu");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupPigRule1[] = _T("%6u / \t%8l$");
    static const TCHAR FARCONST auchPrtSupPigRule2[] = _T("%6u / \t%2lu");


    /* distinguish minor class */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARAPIGRULE_PRICE:               /* Pig Rule Break Point */

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* Thermal Printer */

            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupPigRule1,           /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulPigRule);                /* break point */
        } 

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupPigRule1,               /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulPigRule);                /* break point */
        }
        break;

    case CLASS_PARAPIGRULE_COLIMIT:            /* Pig Rule Counter Limit */

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* Thermal Printer */

            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupPigRule2,           /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulPigRule);                /* counter limit */
        } 

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupPigRule2,               /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulPigRule);                /* counter limit */
        }
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;                                      
    }
}

/*
*===========================================================================
** Synopsis:    VOID    PrtThrmSupMiscPara(PARAMISCPARA *pData)
*               
*     Input:    *pData      : 
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms AC128
*===========================================================================
*/
VOID    PrtThrmSupMiscPara(PARAMISCPARA *pData)
{
    static const TCHAR FARCONST auchPrtThrmSupMiscPara[] = _T("%15u  /  %8l$");
    static const TCHAR FARCONST auchPrtSupMiscPara[]     = _T("%6u / \t%8l$");

    if (pData->usPrintControl & PRT_RECEIPT) {
        PrtPrintf(PMG_PRT_RECEIPT, 
            auchPrtThrmSupMiscPara, (USHORT)pData->uchAddress, pData->ulMiscPara);

        if (pData->usPrintControl & PRT_JOURNAL) {
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupMiscPara,
                (USHORT)pData->uchAddress, pData->ulMiscPara);
        }
    }
}

/***** End of Source *****/
