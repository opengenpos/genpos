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
* Title       : ISP Server, Abort / Write Log Functions Source File                        
* Category    : ISP Server, US Hospitality Model
* Program Name: ISPABORT.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           IspAbort()          Execute " PifAbort " function
*           IspLog()            Execute " PifLog   " function    
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-23-92:00.00.01:H.Yamaguchi: initial                                   
*          :        :           :                                    
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
#include	<tchar.h>
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspAbort(USHORT usFaultCode);
*     Input:    usFalutCode  - Falut code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Execute PifAbort, system goes to System Halt condition.
*===========================================================================
*/
VOID    IspAbort(USHORT usFaultCode)
{

    PifSleep(ISP_SLEEP_ABORT);                  /* Sleep 10 seconds */
    PifAbort(MODULE_ISP_ABORT, usFaultCode) ;   
}

/*===== END OF SOURCE =====*/

