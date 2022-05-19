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
* Title       : Print Rounding Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSRND_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtsupRound()   : This Function Forms Rounding Table Data     
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
** Synopsis:    VOID PrtsupRound( PARAROUNDTBL *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAROUNDTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Rounding Table Data.
*===========================================================================
*/

VOID PrtSupRound( PARAROUNDTBL *pData )
{

    static const TCHAR FARCONST auchPrtSupRound1[] = _T("%12u / %4u");
    static const TCHAR FARCONST auchPrtSupRound2[] = _T("%12u / %4d");

    USHORT  usPrtType;
    USHORT  usDeliModu;



    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Distinguish Mainor Calss */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARAROUNDTBL_DELI:                       /* Delimitor, Modules */

        usDeliModu = pData->uchRoundDelimit * 100;                                        
        usDeliModu += pData->uchRoundModules;                                        
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupRound1,                     /* Format */
                  ( USHORT)pData->uchAddress,           /* Address */
                  usDeliModu);                          /* Delimitor, Modules Data */
        break;

    case CLASS_PARAROUNDTBL_POSI:                       /* Decimal Position */

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupRound2,                     /* Format */
                  ( USHORT)pData->uchAddress,           /* Address */
                  ( SHORT)pData->chRoundPosition);      /* Position */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
