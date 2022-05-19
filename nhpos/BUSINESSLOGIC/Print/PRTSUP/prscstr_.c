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
* Title       : Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCSTR_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtSupCstrData( VOID *pData );     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-20-95: 00.00.01 : M.Waki    : initial                                   
* Sep-08-95: 03.00.08 : M.Ozawa   : print alpha mnemonic downloaded by PEP
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
#include <fsc.h>
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupCstrData( MAINTCSTR *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function format data.
*===========================================================================
*/

VOID PrtSupCstrData( MAINTCSTR *pData )
{

    static const TCHAR FARCONST auchPrtSupCstr[] = _T("%4u  %4u\t%3u-%3u");
    static const TCHAR FARCONST auchPrtSupCstr2[] = _T("%4u  %4u\t   -  %1c");
    static const TCHAR FARCONST auchPrtSupCstrDelete[] = _T(" %3u");

    switch(pData->uchMinorClass){
    case CLASS_PARACSTR_MAINT_DELETE:
        PmgPrintf(PRT_RECEIPT | PRT_JOURNAL,       /* printer type */
                  auchPrtSupCstrDelete,   /* format */
                  pData->usCstrNumber);   /* C-String Number */
        break;

    default:
        if (pData->uchMajorCstrData == FSC_AN) {    /* alpha mnemonic */
            PmgPrintf(pData->usPrintControl,       /* printer type */
                      auchPrtSupCstr2,   /* format */
                      pData->usCstrNumber,       /* C-String Number */
                      pData->uchAddr,            /* Address */
                      pData->uchMinorCstrData);  /* Minor Data */
        } else {
            PmgPrintf(pData->usPrintControl,       /* printer type */
                      auchPrtSupCstr,   /* format */
                      pData->usCstrNumber,       /* C-String Number */
                      pData->uchAddr,            /* Address */
                      pData->uchMajorCstrData,   /* Major Data */
                      pData->uchMinorCstrData);  /* Minor Data */
        }
    }

}

/*** End of File ***/
