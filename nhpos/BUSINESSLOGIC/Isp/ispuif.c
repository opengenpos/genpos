/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : User interface Module                         
* Category    : ISP Server, NCR 2170 US Hospitality Application         
* Program Name: ISPUIF.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               IspInit()          : Initialize ISP Thread
*               IspNetChkBoard()   : Check communication board
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-23-92:00.00.02:T.Kojima   : add to call "RmtInitFileTbl()" in "IspInit()"
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
* Dec-07-99:01.00.00:hrkato     : Saratoga
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
#include <tchar.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <plu.h>
#include <csstbfcc.h>
#include <storage.h>
#include <isp.h>     
#include "ispcom.h"
#include <appllog.h>
#include "rmt.h"

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/
ULONG            ulIspLockedCasNO = 0;        /* Locked a cashier #       */
LONG             lIspTotal = 0;               /* Total for pick,            Saratoga */
ULONG            ulIspPickupCasNO = 0;        /* Pickuped a cashier #,      Saratoga */
USHORT           usIspState = 0;              /* ISP state control        */
USHORT           usIspLockedGCFNO = 0;        /* Locked a GCF     #       */
USHORT           husIspOpHand = 0;            /* Saves Op Handle         */
USHORT           usIspPickupFunCode = 0;      /* Saves Pick or Loan Fun.    Saratoga */
USHORT           husIspMasHand = 0;           /* Saves Mass memory Handle,  Saratoga */
USHORT           usIspTransNoIHav = 0;        /* Saves Transaction No,      Saratoga */
SHORT            sIspCounter = 0;             /* Counter for pick,          Saratoga */
SHORT            hsIspKeyBoardHand = 0;         /* Saves Terminal Handle   */
SHORT            sIspExeError = 0;            /* Retry counter Network Error */
USHORT           usIspTest = 0;               /* ISP Test counter        */
SHORT            sIspNetError = 0;            /* Save Net work error     */       
USHORT           fsIspLockedFC = 0;           /* Locked all function flag */
USHORT           fsIspCleanUP = 0;            /* System control flag      */

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
XGRAM            IspSndBuf;               /* Sends    buffer */
XGRAM            IspRcvBuf;               /* Receives buffer */

ISPNETCONFIG     IspNetConfig;            /* NET work configration   */
ISPPREVIOUS      IspResp;                 /* Response Structure      */
ISPTIMER         IspTimer;                /* Timer Keep              */
ISPRESETLOG      IspResetLog = {0};           /* pc if reset log save area, V3.3 */

UCHAR            auchIspTmpBuf[ISP_MAX_TMPSIZE];  /* Temporary buffer size */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID IspInit(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Initialze any structs needed for Main ISP Server Thread.
*               This function is called once as a part of GenPOS start up.
*===========================================================================
*/
VOID IspInit(VOID)
{
    RmtInitFileTbl();        /* initialize remote file table */
}


/*
*===========================================================================
** Synopsis:    SHORT  IspNetChkBoard(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS or PIF_ERROR
*
** Description: Check communication Board. 
*===========================================================================
*/
SHORT  IspNetChkBoard(VOID)
{
    SYSCONFIG CONST *pSys = PifSysConfig();                      /* Resed system configulation */
    USHORT   usWork;

    for ( usWork = 0 ; usWork < PIF_LEN_PORT ; usWork ++) {
        if ( pSys->auchComPort[usWork] == DEVICE_PC ) {
            usWork = 0;
            break;
        }
    }

    if (usWork) { 
        if (((0 == pSys->auchLaddr[0]) &&
             (0 == pSys->auchLaddr[1]) &&
             (0 == pSys->auchLaddr[2])) ||    /* comm. board not provide */
             (0 == pSys->auchLaddr[3])) {
                 return(PIF_ERROR_NET_NOT_PROVIDED);  /* If not 0, then NOT PROVIDED */
        }
    }    
    
    return(ISP_SUCCESS);
}


/*===  End of Source ===*/