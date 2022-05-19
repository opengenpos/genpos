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
* Title       : Print Tare Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTARE_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupTare()     : This function forms Tare     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-24-92: 01.00.00 : J.Ikeda   : initial                                   
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
** Synopsis:    VOID PrtSupTare( PARATARE *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARATARE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Tare.
*===========================================================================
*/

VOID PrtSupTare( PARATARE *pData )
{

    static const TCHAR FARCONST auchPrtSupTare1[] = _T("%6u /\t%.2$%-4s");
    static const TCHAR FARCONST auchPrtSupTare2[] = _T("%6u /\t%.3$%-4s");

    USHORT      usPrtType;        

    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* distinguish minor class */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARATARE_001:    /* minimum unit is 0.01 */             

        PmgPrintf(usPrtType,                    /* printer type */
                  auchPrtSupTare1,              /* format */
                  ( USHORT)pData->uchAddress,   /* address */
                  ( SHORT)pData->usWeight,      /* tare */
                  pData->aszMnemonics);         /* special mnemonics */

        break;

    case CLASS_PARATARE_0001:   /* minimum unit is 0.001 or 0.005 */         

        PmgPrintf(usPrtType,                    /* printer type */
                  auchPrtSupTare2,              /* format */
                  ( USHORT)pData->uchAddress,   /* address */
                  ( SHORT)pData->usWeight,      /* tare */
                  pData->aszMnemonics);         /* special mnemonics */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
