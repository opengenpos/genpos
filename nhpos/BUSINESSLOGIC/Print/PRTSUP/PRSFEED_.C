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
* Title       : Receipt/Journal Feed Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFEED_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms receipt/journal feed format.
*
*           The provided function names are as follows: 
* 
*                   PrtSupRJFeed()     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-03-92: 00.00.01 : J.Ikeda   : initial                                   
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

/**------ 2170 local ------**/
#include	<tchar.h>
#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include "prtrin.h"   
#include "prtsin.h"


/*
*===========================================================================
** Synopsis:    VOID  PrtSupRJFeed(MAINTFEED *pData)
*               
*     Input:    pData    : pointer to structure for MAINTFEED
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms receipt/journal print format.
*===========================================================================
*/

VOID  PrtSupRJFeed(MAINTFEED *pData)
{
    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTFEED_OUTSIDE) {
        /* check if receipt or journal */
        if (pData->usPrtControl & PRT_RECEIPT) {
            PrtFeed(PMG_PRT_RECEIPT, ( USHORT)(pData->lFeedAmount));
            /* check if print electro or not */
            if (!CliParaMDCCheck(MDC_SLIP_ADR, EVEN_MDC_BIT3)) {    /* Electro Provide */
                PrtElec();                          /* print electro */
                PrtFeed(PMG_PRT_RECEIPT, 12);       /* 12 line feed */
                PrtCut();                           /* cut */
            } else {
                PrtFeed(PMG_PRT_RECEIPT, 4);        /* 4 line feed */
                PrtRJHead();                        /* print header */
//                PrtFeed(PMG_PRT_RECEIPT, 2);        /* 2 line feed */
                PrtCut();                           /* cut */
            }
        } else {                                    /* in case of journal */
            PrtFeed(PMG_PRT_JOURNAL, ( USHORT)(pData->lFeedAmount));
        }
    } else {                                        /* in case of inside transaction */
        /* check if receipt or journal */
        if (pData->usPrtControl & PRT_RECEIPT) {
            PrtFeed(PMG_PRT_RECEIPT, ( USHORT)(pData->lFeedAmount));
        } else {
            PrtFeed(PMG_PRT_JOURNAL, ( USHORT)(pData->lFeedAmount));
        }
    }
}
