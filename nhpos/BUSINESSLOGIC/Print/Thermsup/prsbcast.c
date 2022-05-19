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
* Title       : Thermal Print Terminal No/Register No Format ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSBCAST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: 
*           PrtThrmSupBcas() : form Terminal Number & Register Number
*                               for Broascast/Request to Download parameter
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-15-93: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupBcas( MAINTBCAS *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTBCAS
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Terminal Number & Register Number print format.
*
*                    : BROADCAST/REQUEST TO DOWNLOAD PLU PARAMETER
*                    : BROADCAST/REQUEST TO DOWNLOAD ALL PARAMETER
*                    : BROADCAST/REQUEST TO DOWNLOAD SUPER PARAMETER
*
*===========================================================================
*/

VOID  PrtThrmSupBcas( MAINTBCAS *pData )
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupBcas[] = _T("%15s#%03u   %03u  %03u");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupBcas[] = _T("%6s#%03u  %03u  %03u");

    TCHAR   aszNull[1] = {'\0'};

    /* check print control */
    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print TERMINAL No. / REGISTER No. */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupBcas, aszNull, ( USHORT)pData->uchTermNo, pData->usRegNo, ( USHORT)pData->uchBcasStatus);
    } 
    
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ PRINTER */
        /* print TERMINAL No. / REGISTER No. */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupBcas, aszNull, ( USHORT)pData->uchTermNo, pData->usRegNo, ( USHORT)pData->uchBcasStatus);
    }
}
/***** End of Source *****/
