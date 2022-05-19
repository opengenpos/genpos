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
* Title       : Print Error Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSERR_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms Error print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupErrorCode() : form Error print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-02-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include <stdlib.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupErrorCode( MAINTERRORCODE *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Error Code print format.
*===========================================================================
*/

VOID  PrtSupErrorCode( MAINTERRORCODE *pData )
{
    static const TCHAR FARCONST auchPrtSupError[] = _T("E- %4d");

    USHORT usPrtType;

    /* check print control */
    switch(pData->usPrtControl) {
    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        return;
    }

    PmgPrintf(usPrtType,                          /* printer type */
              auchPrtSupError,                    /* format */
              pData->sErrorCode);                 /* mnemonics */

}

