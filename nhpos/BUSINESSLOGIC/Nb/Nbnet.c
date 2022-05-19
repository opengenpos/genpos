/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Notice Board Net Work Sub routine Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBNET.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*            NbNetOpen()         : Open net work
*            NbNetReceive()      : Receive net data  
*            NbNetSend()         : Execute Normal state 
*            NbNetReOpen()       : Re-open net work 
*            NbNetChkBoard()     : Check Communication board 
*            NbNetDelaytoSend()  : Delay time until reach to next send time             
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
* May-11-93:00.00.01:H.Yamaguchi: Delete check U-Addr at NbNetReceive.                                   
* Sep-07-93:00.00.01:H.Yamaguchi: Same As HPINT.
* 2171 for Win32
* Sep-22-99:01.00.00:K.Iwata    : V1.0 Initial
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
#include <string.h>
#include <math.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <csstbstb.h>
#include <nb.h>
#include "nbcom.h"
#include <appllog.h>
#include <rfl.h>
#include <plu.h>
#include <csstbfcc.h>

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/
// following flag used to isolate a Terminal so that it does not receive Master Terminal broadcasts.
// some LANs with subnet ranges may create conditions where by a Satellite does not receive broadcast from its Master Terminal
// When this is enabled, a Satellite Terminal will show a gray MAS.
// similar behavior is seen when a Satellite Terminal is not on the
// same subnet as the Master Terminal and a piece of network gear is
// blocking Notice Board broadcast messages, a condition seen at VCS.
//#define NOTICEBOARD_ISOLATE  1

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
static USHORT  husNbNetHand = 0;            /* Saves Net Handle */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    SHORT  NbNetOpen(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS or PIF_ERROR
*
** Description: Open Net work 
*===========================================================================
*/
SHORT  NbNetOpen(VOID)
{
    SYSCONFIG CONST  * pSys = PifSysConfig();          /* Read system configulation */
    SHORT sRet, sRetry;

    sRet = NbNetChkBoard(NB_MASTERUADD);

    if ( sRet ) { 
        return(sRet);                        /* If not 0, then NOT PROVIDED */
    }    

    memcpy ((UCHAR *)&NbSndBuf.NbMesHed.auchFaddr[0], &pSys->auchLaddr[0], PIF_LEN_IP);

    NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set All address */
    NbSndBuf.NbMesHed.usFport = ANYPORT ;       /* Set port No.    */
    NbSndBuf.NbMesHed.usLport = (PIF_PORT_NB | PIF_PORT_FLAG_CLUSTER);   /* Set port No.    */

    sRetry = 0; /* ### ADD (2171 for Win32) V1.0 */
    for (;;) {
        sRet = PifNetOpen((CONST XGHEADER *)&NbSndBuf.NbMesHed);

        if (sRet >= 0) {
            break;                          /* if success, then retun */
        }

        sRetry ++;
 
        if ( sRetry > NB_RETRYMAXTIMES ) {
            PifAbort(MODULE_NB_ABORT, FAULT_BAD_ENVIRONMENT);
        }

        PifSleep(NB_SLEEPTIME);             /* sleep time */
    }

    husNbNetHand = sRet;                        /*  Save network handle */
    NbSndBuf.NbMesHed.usFport = PIF_PORT_NB;    /* Set port No.    */

    sRet = PifNetControl(husNbNetHand, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);

    if ( sRet == PIF_OK) {                   /* Change PIF_OK to NB_SUCCESS */
        sRet = NB_SUCCESS;
    }

    return(sRet);
}

/*
*===========================================================================
** Synopsis:    SHORT  NbNetReceive(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS or PIF_ERROR
*
** Description: Receive Net work 
*===========================================================================
*/
SHORT  NbNetReceive(VOID)
{
    SHORT  sRet;
    LONG   lTime;
//	XGNAME  jjj;

    lTime = NbGetRcvTime();                  /* get timer */

    sRet = PifNetControl(husNbNetHand, PIF_NET_SET_TIME, (USHORT)lTime);

    if (sRet < 0)  {
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_RECV_CONTROL);
        return(sRet);                        /* if fail, then return */
    }

    sRet = PifNetReceive(husNbNetHand, (VOID *)&NbRcvBuf, sizeof(NbRcvBuf));

#if NOTICEBOARD_ISOLATE
	// allows for a test of what happens when a particular terminal is unable to receive
	// Notice Board broadcasts or direct Notice Board messages.
	// main purpose is to isolate a Satellite from a Master Terminal's Notice Board broadcasts.
	{
		UCHAR   uchUadd;

		uchUadd = CliReadUAddr();           /* Get unique address */
		sRet = PIF_ERROR_NET_DISCOVERY;
	}
#endif
	if (sRet < 0) {
		SYSCONFIG CONST *pSysConfig = PifSysConfig ();
		if ((*(ULONG *)(pSysConfig->PifNetHostInformation[0].auchHostIpAddress)) == 0) {
			// we do not know who our Master Terminal is so lets try to find out.
			PifNetDiscoverNet ();    // attempt to discover the Master Terminal
		}
		if (sRet == PIF_ERROR_NET_DISCOVERY || sRet == PIF_ERROR_NET_TIMEOUT || PIF_ERROR_NET_CLEAR == sRet) {
			return (sRet);
		}
		else {
			PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_RECV_RECEIVE);
			PifLog(MODULE_ERROR_NO(MODULE_NB_LOG), (USHORT)abs(sRet));
			return(sRet);                        /* if fail, then return */
		}
	}

//    sRet = PifNetControl(husNbNetHand, PIF_NET_GET_NAME, &jjj);

    if ( NB_SUCCESS != NbCheckUAddr() ) {    /* Check terminal address */
        /* Net clear if Master or Backup Master terminal  */
        PifNetControl(husNbNetHand, PIF_NET_CLEAR);
    }

    if ( sRet == sizeof(NbRcvBuf)) {      /* Check receive data length */
        sRet = NB_SUCCESS;
		NbSetMTBM_Online (NbRcvBuf.NbMesHed.auchFaddr[3]);  /* Set online indicator */
    }  else {
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_RECV_INVALIDDATA);
        sRet = NB_INVALIDDATA;                      /* Received illeagal data */
    }

    return(sRet);
}

/*
*===========================================================================
** Synopsis:    SHORT  NbNetSend(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS or PIF_ERROR
*
** Description: Send data to all terminal 
*===========================================================================
*/
SHORT  NbNetSend(VOID)
{
    SHORT sRet,sI;
                         
	// Set a timeout for the send so that if the server is
	// busy, we will get notified immediately.  Look at
	// SerRecvNextFrame () in servrmh.c in the Server subsystem
	// for an example of Server responding with STUB_BUSY.
	// PifNetControl(husNbNetHand, PIF_NET_SET_STIME, 750);

    for ( sI = 0; sI < 5 ; sI++ )
	{
        sRet = PifNetSend(husNbNetHand, (VOID *)&NbSndBuf, sizeof(NbSndBuf));

        if ( ( sRet != PIF_ERROR_NET_WRITE ) && 
             ( sRet != PIF_ERROR_NET_OFFLINE ) &&
			 ( sRet != PIF_ERROR_NET_BUSY ) )
		{
            break;
        }
        PifSleep(100);                           /* Wait 100 mili-sec */
    }

	if (5 <= sI) {
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_SEND_RETRYLIMIT);
	}
	else if (sRet == PIF_ERROR_NET_DISCOVERY) {
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_SEND_DISCOVERY);
	}
	else if (0 > sRet) {
		if (sRet != PIF_ERROR_NET_UNREACHABLE) {
			// In some cases we have seen with Amtrak the error of PIF_ERROR_NET_UNREACHABLE (-14)
			// when the LAN cable is not attached.  So rather than generating a PIFLOG once a
			// second, lets only generate a PIFLOG if an actual error.
			PifLog(MODULE_NB_LOG, LOG_ERROR_NB_SEND_PIF_ERROR);
			PifLog(MODULE_ERROR_NO(MODULE_NB_LOG), (USHORT)abs(sRet));
		}
	}

    return(sRet);
}

/*
*===========================================================================
** Synopsis:    SHORT  NbNetReOpen(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS or PIF_ERROR
*
** Description: Re-open Net work 
*===========================================================================
*/
SHORT  NbNetReOpen(VOID)
{
    SHORT     sRet;

    PifLog (MODULE_NB_LOG, LOG_EVENT_NB_NETREOPEN_START);

    for (;;) {

        PifNetClose(husNbNetHand);                  /* Close net work */

        sRet = NbNetOpen();                         /* Re-open net work */

        if ( sRet == NB_SUCCESS ) {
            break ;
        }

        if ( sRet == PIF_ERROR_NET_NOT_PROVIDED ) { 
            NbEndThread() ;                         /* Go to EndThread or Abort */
            break;
        }
        PifSleep(NB_SLEEPTIME);                     /* Sleep time */
    }

    PifLog (MODULE_NB_LOG, LOG_EVENT_NB_NETREOPEN_END);
    return(sRet);
}

/*
*===========================================================================
** Synopsis:    SHORT  NbNetChkBoard(USHORT UAdd)
*     Input:    UAdd    Unique address
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS or PIF_ERROR
*
** Description: Check communication Board status to determine if the
*               net connectivity is up and set
*===========================================================================
*/
SHORT  NbNetChkBoard(USHORT UAdd)
{
    SYSCONFIG CONST * pSys = PifSysConfig();      /* Resed system configulation */
    UCHAR uchWork;

    if ( usBMOption == NB_WITH_BACKUP_MASTER ) {    /* saratoga */
//        if ( UAdd != NB_BMASTERUADD ) {
            PifRequestSem(husNbSemHand);
            NbSysFlag.fsSystemInf &= ~NB_WITHOUTBM;  /* Reset without B-Master */
            PifReleaseSem(husNbSemHand);
//        }
    }  else {
		// If the usBMOption is not set this means that a Backup Master
		// terminal has not been provisioned.  However, the question is
		// whether the terminal's provisioning is or is not representative
		// of the cluster's provisioning.  We can run into a difference
		// in the case of a terminal replacement scenario in which a terminal
		// with a default provisioning, meaning Backup Master Terminal is not
		// provisioned, is put into a cluster that does have a Backup Master
		// terminal.  In the case of a Satellite Terminal, it doesn't really
		// matter but it does matter in two instances, replacement of a Master
		// Terminal and replacement of a Backup Master Terminal.
//        if ( UAdd != NB_BMASTERUADD ) {
            PifRequestSem(husNbSemHand);
            NbSysFlag.fsSystemInf |= NB_WITHOUTBM;  /* Set without B-Master */
            PifReleaseSem(husNbSemHand);
//			PifLog (MODULE_NB_LOG, LOG_ERROR_NB_CHKBOARD_WITHOUTBM);
//       } 
    }

    uchWork  = pSys->auchLaddr[0];           /* If Laddr[0],[1],[2]=0, then No comm B'd */
    uchWork |= pSys->auchLaddr[1];
    uchWork |= pSys->auchLaddr[2];

    if ( !(uchWork) ) { 
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_NOT_PROVIDED);
        return(PIF_ERROR_NET_NOT_PROVIDED);  /* If all 0, then NOT PROVIDED */
    } 
   
    if ( 0 == pSys->auchLaddr[3] ) {
        PifLog(MODULE_NB_LOG, LOG_ERROR_NB_NET_NOT_PROVIDED);
        return(PIF_ERROR_NET_NOT_PROVIDED);  /* If UADDR 0, then NOT PROVIDED */
    }
    return(NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID  NbNetDelaytoSend(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Delay time until reach to next send time
*===========================================================================
*/
VOID  NbNetDelaytoSend(VOID)              
{
    LONG  lTime;

    lTime  = NbGetRcvTime();                /* Get timer  ( mili second) */
    if (lTime > NB_SLEEPTIME)
        lTime -= (LONG)NB_SLEEPTIME ;           /* Substruct 2 seconds       */
    else
        lTime = 0;

    PifSleep((USHORT)lTime);                /* Sleep Time */
}

/*
*===========================================================================
** Synopsis:    SHORT  NbNetClear(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: clear Network buffer (02/08/2001)
*===========================================================================
*/
SHORT  NbNetClear(VOID)
{
	SHORT  sRet = 0;

    if ( NB_SUCCESS != NbCheckUAddr() ) {    /* Check terminal address */
		/* Net clear if Master or Backup Master terminal   */
        sRet = PifNetControl(husNbNetHand, PIF_NET_CLEAR);
    }

	return sRet;
}
/*====== End of Source File ======*/
