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
* Title       : Receipt/Journal Feed Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATFEED.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MaintFeedRec()  : Receipt Feed Key handling   
*               MaintFeedJou()  : Journal Feed Key handling   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-03-92:00.00.01    : J.Ikeda   : initial                                   
* Jun-21-93:01.00.12    : K.You     : mod. MaintFeedRec().                   
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>

#include <ecr.h>
/* #include <uie.h> */
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include "maintram.h"

/*
*======================================================================================
**   Synopsis:  VOID  MaintFeedRec( LONG lFeedLine )
*
*       Input:  lFeedLine       : Feed Line Number           
*      Output:  Nothing
*
**  Return:     Nothing          
*               
**  Description: Receipt Feed Key handling    
*======================================================================================
*/

VOID  MaintFeedRec( LONG lFeedLine )
{

    MAINTFEED   MaintFeed;

    /* check if inside transaction or outside transaction */

    if ((uchMaintOpeCo & MAINT_ALREADYOPERATE) || (uchRptOpeCount & RPT_HEADER_PRINT)) {   /* in case of inside transaction */
        MaintFeed.uchMinorClass = CLASS_MAINTFEED_INSIDE;           /* Set Minor Class */
    } else {
        MaintFeed.uchMinorClass = CLASS_MAINTFEED_OUTSIDE;          /* Set Minor Class */
    }
    MaintFeed.uchMajorClass = CLASS_MAINTFEED;                      /* Set Major Class */
    MaintFeed.lFeedAmount = lFeedLine;                              /* Set Number of Default Line Feed */
    MaintFeed.usPrtControl = PRT_RECEIPT;                     /* Set Printer Control */

    PrtPrintItem(NULL, &MaintFeed);

    if ( MaintFeed.uchMinorClass == CLASS_MAINTFEED_INSIDE ) {
        PrtShrPartialSend(MaintFeed.usPrtControl);
    }   
}

/*
*======================================================================================
**   Synopsis:  VOID  MaintFeedJou( VOID )
*
*       Input:  Nothing          
*      Output:  Nothing
*
**  Return:     Nothing          
*               
**  Description: Journal Feed Key handling    
*======================================================================================
*/

VOID  MaintFeedJou( VOID )
{

    MAINTFEED   MaintFeed;

    MaintFeed.uchMajorClass = CLASS_MAINTFEED;                      /* Set Major Class */
    MaintFeed.uchMinorClass = CLASS_MAINTFEED_INSIDE;               /* Set Minor Class */
    MaintFeed.lFeedAmount = MAINT_JOUKEYDEF_FEED;                   /* Set Number of Default Line Feed */
    MaintFeed.usPrtControl = PRT_JOURNAL;                     /* Set Printer Control */

    PrtPrintItem(NULL, &MaintFeed);
}
