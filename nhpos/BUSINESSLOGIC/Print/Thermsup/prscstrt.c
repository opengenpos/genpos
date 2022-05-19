/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Thermul Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCSTRT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtThrmSupCstrData( VOID *pData );     
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
#include "prtcom.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupCstrData( MAINTCSTR *pData )
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

VOID PrtThrmSupCstrData( MAINTCSTR *pData )
{
    /* Print Format Define */
    static const TCHAR FARCONST auchPrtThrmSupCstr[] = _T(" %4u   %4u\t%3u-%3u");
    static const TCHAR FARCONST auchPrtThrmSupCstr2[] = _T(" %4u   %4u\t   -  %1c");
    static const TCHAR FARCONST auchPrtSupCstr[] = _T(" %4u %4u\t%3u-%3u");
    static const TCHAR FARCONST auchPrtSupCstr2[] = _T(" %4u %4u\t   -  %1c");
    static const TCHAR FARCONST auchPrtThrmSupCstrDelete[] = _T(" %4u");

    switch(pData->usPrintControl){
    case PRT_RECEIPT:
        switch(pData->uchMinorClass){
        case CLASS_PARACSTR_MAINT_DELETE:
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstrDelete, pData->usCstrNumber);
            break;
        default:
            if (pData->uchMajorCstrData == FSC_AN) {    /* alpha mnemonic */
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstr2, pData->usCstrNumber, pData->uchAddr, pData->uchMinorCstrData);
            } else {
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstr, pData->usCstrNumber, pData->uchAddr, pData->uchMajorCstrData, pData->uchMinorCstrData);
            }
        }
        break;

    case PRT_JOURNAL:
        switch(pData->uchMinorClass){
        case CLASS_PARACSTR_MAINT_DELETE:
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtThrmSupCstrDelete, pData->usCstrNumber);
            break;
        default:
            if (pData->uchMajorCstrData == FSC_AN) {    /* alpha mnemonic */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCstr2, pData->usCstrNumber, pData->uchAddr, pData->uchMinorCstrData);
            } else {
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCstr, pData->usCstrNumber, pData->uchAddr, pData->uchMajorCstrData, pData->uchMinorCstrData);
            }
        }
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        switch(pData->uchMinorClass){
        case CLASS_PARACSTR_MAINT_DELETE:
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstrDelete, pData->usCstrNumber);
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtThrmSupCstrDelete, pData->usCstrNumber);
            break;
        default:
            if (pData->uchMajorCstrData == FSC_AN) {
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstr2, pData->usCstrNumber, pData->uchAddr, pData->uchMinorCstrData);
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCstr2, pData->usCstrNumber, pData->uchAddr, pData->uchMinorCstrData);
            } else {
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCstr, pData->usCstrNumber, pData->uchAddr, pData->uchMajorCstrData, pData->uchMinorCstrData);
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCstr, pData->usCstrNumber, pData->uchAddr, pData->uchMajorCstrData, pData->uchMinorCstrData);  /* Minor Data */
            }
        }
        break;
    }
}

/**** End of File ****/
