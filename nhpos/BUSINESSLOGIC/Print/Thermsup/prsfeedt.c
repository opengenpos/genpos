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
* Title       : Thermal Feed Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFEEDT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms feed format.
*
*           The provided function names are as follows: 
* 
*                   PrtThrmSupRJFeed()     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-18-93: 00.00.01 : J.IKEDA   : Initial                                   
* Jul-02-93: 01.00.12 : J.IKEDA   : Add PrtShrEnd()                                   
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
/**------ 2170 local ------**/
#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
/* #include <csstbpar.h> */
/* #include <cswai.h> */
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <shr.h>
#include <prt.h>

#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"
#include "prtshr.h"


/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupRJFeed(MAINTFEED *pData)
*               
*     Input:    pData    : pointer to structure for MAINTFEED
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms print format.
*===========================================================================
*/

VOID  PrtThrmSupRJFeed(MAINTFEED *pData)
{
    TRANINFORMATION  Tran;

    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTFEED_OUTSIDE) {
        /* check if receipt or journal */
        if (pData->usPrtControl & PRT_RECEIPT) {
            /* set transaction information */
            Tran.TranCurQual.ulStoreregNo = 0L;
            Tran.TranCurQual.usConsNo = 0;

            /* print RECEIPT HEADER */
            PrtTHHead(&Tran);
            PrtEJInit(&Tran);                   /* initialize E/J */

            /* initialize receipt header flag */
            fbPrtTHHead = 0;
            PrtFeed(PMG_PRT_RECEIPT, ( USHORT)(pData->lFeedAmount));
            PrtFeed(PMG_PRT_RECEIPT, 3+1);        /* 3 line feed, for 7158, saratoga */
            PrtCut();                           /* cut */

            /* send shared data even if shared buffer is not full */
            PrtShrEnd();

            if ( !( fbPrtLockChk ) ) {
                /* -- reset shared/normal, aborted, alternation flag -- */
                fbPrtShrStatus = 0; 
                fbPrtAbortStatus = 0;
                fbPrtAltrStatus = 0;            
            }
        } 
    } else {                                   /* in case of inside transaction */
        /* check if receipt or journal */
        if (pData->usPrtControl & PRT_RECEIPT) {
            PrtFeed(PMG_PRT_RECEIPT, ( USHORT)(pData->lFeedAmount));
        }
    }
}
/***** End of Source *****/
