/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Notice Board Main Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBMAIN.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*             NbMain()            : NB main Idle
*               NbInitialize()    : Initialize 
*               NbExecuteNormal() : Execute Normal state 
*                 NbErrResHand()  : Execute error response handling  
*                 NbRcvMesHand()  : Execute receive message handling  
*               NbExecuteWait()   : Execute Wait state  
*               NbExecuteSend()   : Execute Send state 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
* May-11-93:00.00.01:H.Yamaguchi: Adds retry confirm BMaster.                                   
* Sep-07-93:00.00.01:H.Yamaguchi: Same As HPINT.
* JAn-19-00:01.00.00:hrkato     : Saratoga
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
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "fsc.h"
#include "log.h"
#include "uic.h"
#include "uie.h"
#include "pifmain.h"
#include "nb.h"
#include "csstbstb.h"
#include "csop.h"
#include "nbcom.h"
#include "appllog.h"
#include "paraequ.h"

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/

static USHORT  usNbState = 0;               /* Uses NB state control */
static USHORT  usNbNetErr = 0;              /* Controls a detected net error count */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID  NbMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Main NB Idle
				This is the main Notice Board thread loop.  This loop sends
				and receives Notice Board messages, updating terminal status
				as it does so.

				This thread is started from within NbInit() which initializes the global
				NbSysFlag.fsSystemInf to indicate that Master and, if provisioned, Backup
				Master terminals are UPTODATE.  NbInit() is invoked as a part of the startup
				code within CliInitialize() which in turn is invoked from UifCreateMain().

				NbInitialize() sets up the first part of the Notice Board environment
				by initializing the network layer connectivity and initialing the
				Notice Board send message buffer header, NbSndBuf.NbMesHed.  NbInitialize()
				also initializes the usNbState variable to NB_WAITSTATE.

				Notice Board behavior is also affected by the Server subsystem function
				SerInitialize() which initializes the server thread environment before starting
				the server thread network message loop in the function SerStartUp().  The
				server thread is started by UifCreateMain() in pifmain.c.

				If you look at the contents of the standard Notice Board message
				you will see that it basically contains a function code, a sequence
				number, and the current fsSystemInf status bit map.

				What the Notice Board thread does is to send Notice Board messages
				to determine if there is connectivity with the Master and Backup Master
				(if provisioned with a Backup Master) terminals and then indicating the
				status of that connectivity accordingly by changing descriptor information
				on the operator display.

				usNbState is a global variable used to indicate the next state of the
				Notice Board state machine.  It is initialized to NB_WAITSTATE by
				NbInitialize when NbMain first starts up.  After that it is updated
				by what is discovered during Notice Board message processing.

				NbSysFlag is a global variable used to remember the current status.  It
				is updated based on the results of Notice Board Messages.  The component
				fsSystemInf within NbSysFlag (a NBMESSAGE struct) is a bit map used to
				indicate the current Notice Board status.  It is modified not only by the
				Notice Board thread but also by the Server thread (through the use of the
				NbStartAsMaster() and NbStartOnline() functions within SerSendNtbStart()).
				The Notice Board status is monitored by the Server thread and the Client
				thread through the use of the NbReadAllMessage() function and these threads
				modify their behavior based on the Notice Board status.
*===========================================================================
*/
VOID  THREADENTRY NbMain(VOID)
{
	ULONG     ulParaHashStartup = 0;
    NBMESSAGE pMes;

	PifSleep (3000);    // Give other threads a chance to finish initializing first.

    NbInitialize();                              /* Initialize */
	NbSetFCtoSndBuf(NB_FCSTARTONLINE);  /* Set function code to send buffer */

	ulParaHashStartup = ParaGenerateHash();

	{
		char  xBuff[128];
		sprintf (xBuff, "==STATUS:  NbMain startup ParaGenerateHash = %u", ulParaHashStartup);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    for (;;) {                                   /* loop for ever */
		// Be very careful about modifying the time for this sleep.
		// There are a number of counters within the Notice Board thread which are
		// used to determine the Notice Board status and changes to the Notice Board
		// state machine.  Changing the sleep time modifies how often those counters
		// are updated which can result in Notice Board state change problems.
        PifSleep(1000);

		{
			// check to see if our database has changed and if so, issue an ASSRTLOG log
			// we need to dampen out multiple changes within a short time span such as we
			// would see with a file transfer.
			static  int iCounter = 0;
			ULONG  ulParaHashCurrent = ParaGenerateHash();

			if (ulParaHashCurrent != ulParaHashStartup) {
				iCounter++;
				if (iCounter > 60) {
					char  xBuff[128];

					sprintf (xBuff, "==STATUS: ulParaHashCurrent (%u) != ulParaHashStartup (%u)", ulParaHashCurrent, ulParaHashStartup);
					NHPOS_NONASSERT_TEXT (xBuff);
					ulParaHashStartup = ulParaHashCurrent;
					iCounter = 0;
				}
			}
		}

        /* --- Prohibit backup copy thread to change NB status (02/06/2001) --- */
        PifRequestSem(husNbBkupSem);

        NbReadAllMessage(&pMes);                 /* Read all flag      */        
        NbDescriptor(pMes.fsSystemInf);          /* Display descriptor */

        if ( pMes.fsSystemInf & (NB_STARTASMASTER | NB_STARTONLINE) ) {
            usNbState   = NB_WAITSTATE;          /* Set state to WAIT */
        }

		if ( pMes.auchMessage[NB_MESOFFSET0] & NB_REQ_OPERATOR_MSG) {      /* display operator message received. */
			USHORT  usMessageBufferEnum = 0xffff;
			OpDisplayOperatorMessage (&usMessageBufferEnum);
			NbResetMessage(NB_MESOFFSET0, NB_REQ_OPERATOR_MSG);
		}

        switch (usNbState) {
			case NB_NORMALSTATE :               /* Normal state         */
				NbExecuteNormal();              /* Execute Normal state */
				break;

			case NB_SENDSTATE :                 /* Sending state  */
				NbExecuteSend(pMes.fsSystemInf); /* Execute to send data state */
				break;

			default :                           /* Wait   state   */
				NbExecuteWait(pMes.fsSystemInf);                /* Execute wait for starting monitor */
				break;                          /* or delay to send a message        */
        }

        /* --- Allow backup copy thread to change NB status (02/06/2001) --- */

        PifReleaseSem(husNbBkupSem);

        /* --- Retry for PC i/F Reset Log,  V3.3 --- */
        UifCPRetryUns();
    }
}

/*
*===========================================================================
** Synopsis:    VOID  NbInitialize(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Initialize
*===========================================================================
*/
VOID  NbInitialize(VOID)
{
    SHORT     sRet;

    sRet = NbNetOpen();                         /* Open net work */
 
    switch ( sRet ) {
    
    case  PIF_ERROR_NET_NOT_PROVIDED : 
        NbEndThread() ;                         /* Go to EndThread or Abort */
        break;

    case PIF_ERROR_NET_POWER_FAILURE :
        NbNetReOpen();                          /* Re-open net work */
        break;

    default :
        break;
    }

    usNbPreDesc = 0 ;                           /* Set a flag to Display */
    usNbState   = NB_WAITSTATE;                 /* Set state to WAIT */
}

/*
*===========================================================================
** Synopsis:    VOID  NbExecuteNormal(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute normal state function
*===========================================================================
*/
VOID  NbExecuteNormal(VOID)
{
    SHORT     sRet;

    sRet = NbNetReceive();                 /* Receive data */

    if ( sRet < 0 ) {
        NbErrResHand(sRet) ;              /* Execute Error Response Handling */
    } else {
        NbRcvMesHand();                   /* Execute Receive Message Handling */
    }
}

/*
*===========================================================================
** Synopsis:    VOID  NbErrResHand(SHORT sRet)
*     Input:    sRet
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute Error response handling
*===========================================================================
*/
VOID  NbErrResHand(SHORT sRet)
{
    UCHAR   uchUadd;

    uchUadd = CliReadUAddr();           /* Get unique address */

    switch (sRet) {

	case PIF_ERROR_NET_DISCOVERY:
		// fall through and treat PIF_ERROR_NET_DISCOVERY same as PIF_ERROR_NET_TIMEOUT
	case PIF_ERROR_NET_UNREACHABLE:
		// Fall thru and treat PIF_ERROR_NET_UNREACHABLE like PIF_ERROR_NET_TIMEOUT
    case PIF_ERROR_NET_TIMEOUT :
        usNbState =  NB_SENDSTATE;              /* Set state to SEND */
        
        if ( uchUadd ==  NB_MASTERUADD ) {
			if ( NbSndBuf.NbMesHed.auchFaddr[3] == (UCHAR)NB_BMASTERUADD) { 
				usNbRcvFBM ++;            /* Increment Backup Master failure count */
			}
            NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)NB_BMASTERUADD;
        }
		else if ( uchUadd ==  NB_BMASTERUADD && usBMOption == NB_WITH_BACKUP_MASTER) {
			if ( NbSndBuf.NbMesHed.auchFaddr[3] == (UCHAR)NB_MASTERUADD) { 
				usNbRcvFMT ++;            /* Increment Master failure count */
			}
            NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)NB_MASTERUADD;
		} else {
			// this is a Satellite Terminal.  If we are a non-Backup system then
			// the only broadcaster is the Master Terminal.  If we are not seeing
			// messages from the Master Terminal then there is a problem of some
			// kind.
			if ( usBMOption == NB_WITH_BACKUP_MASTER ) {
				usNbRcvFMT ++;            /* Increment Master failure count */
				usNbRcvFBM ++;            /* Increment Backup Master failure count */
			} else {
				usNbRcvFMT ++;            /* Increment Master failure count */
			}
		}
        
        NbCheckOffline(sRet);             /* Check offline counter */
        break;

    case PIF_ERROR_NET_CLEAR:
		// do not make any changes in state or whatever.  we will just keep everything the
		// same and retry again with same settings and environment.
		break;

    case PIF_ERROR_NET_POWER_FAILURE:
        usNbPreDesc = 0;                  /* Clear previous descriptor data */
        NbResetMTOnline();                /* Reset   Master online */
        NbResetBMOnline();                /* Reset B-Master online */
        NbNetReOpen();                    /* Re-open net work */
        usNbState =  NB_WAITSTATE;        /* Set state to WAIT */

        PifRequestSem(husNbSemHand);
        NbSndBuf.NbConMes.usNbFunCode = 0;  /* Set no function code */
        PifReleaseSem(husNbSemHand);

        NbPrintPowerDown(PIF_ERROR_NET_POWER_FAILURE);  /* Power Down,  Saratoga */

        break;

    case NB_RCVSELFDATA :                 /* Received a self transmit data */ 

        break;                            /* Not used because adding to confirm */

    case PIF_ERROR_NET_NOT_PROVIDED :
 
        NbEndThread() ;                   /* Go to EndThread or Abort */
        break;

    default :

        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID  NbRcvMesHand()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute Receive Message Handling
				This function is used by NbExecuteNormal to process a
				successfully received Notice Board message contained in NbRcvBuf.

				The function code indicates the kind of Notice Board message
				which we have received.  Based on the kind of message received,
				we then determine the next step in our Notice Board state machine.

				The counters usNbRcvFMT and usNbRcvFBM are used to detect if the
				Master Terminal and the Backup Master Terminal respectively are on-line.

				NbSndBuf is the message buffer that is sent.  As a part of this
				code, we check to see if we should send a message to the Master
				terminal or the Backup Master terminal directly (by modifying the
				last octet of the IP address to be the Master or the Backup Master
				terminal) or if we should do a broadcast to all of the terminals
				in the cluster (by setting the last octet of the IP address to zero).
*===========================================================================
*/
VOID  NbRcvMesHand(VOID)
{
    switch (NbRcvBuf.NbConMes.usNbFunCode) {
    case (NB_FCSTARTASMASTER | NB_MASTERUADD):
    case NB_FCSTARTASMASTER :            /* Starts as master  */
        NbExecRcvAsMMes();               /* Execute a received as master's message */
        UicStart();                      /* Start UIC thread */

        usNbState =  NB_WAITSTATE_DELAY ; /* Set state to Delay  */
        break;

    case (NB_FCSTARTASMASTER | NB_BMASTERUADD) :  /* Check offline to B Master  */
        NbSetMTOnline();
        usNbRcvFMT = 0 ;                    /* Clear Counter  */
        usNbNetErr ++;                      /* Controls a detected net error count */
        NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)NB_MASTERUADD;  /* Set BM address */
    
        usNbState =  NB_SENDSTATE;              /* Send quick response */
        return;

    case (NB_FCSTARTONLINE | NB_MASTERUADD):
    case NB_FCSTARTONLINE   :           /* Starts online information  */
        NbExecRcvMonMes();              /* Execute a received online monitor message */

        usNbState =  NB_WAITSTATE_DELAY ; /* Set state to Delay  */

        if ( NbSndBuf.NbMesHed.auchFaddr[3] != (UCHAR)0 ) { 
            NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set all address */
            usNbState =  NB_SENDSTATE;              /* Send quick response */
        }
        break;

    case (NB_FCSTARTONLINE | NB_BMASTERUADD)  : /* Response online information  */
        NbSetBMOnline();
        usNbRcvFBM = 0 ;                          /* Clear Counter  */
        usNbState =  NB_SENDSTATE;              /* Send quick response */
        usNbNetErr ++;                      /* Controls a detected net error count */
        NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set all address */
        return;
        break;

    default :
        /*        PifLog(MODULE_NB_LOG,                  */
        /*               LOG_ERROR_NB_INVALID_FUNCODE);  */    /* Write Log */

		PifLog(MODULE_NB_ABORT, FAULT_INVALID_FUNCTION);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_ABORT), NbRcvBuf.NbConMes.usNbFunCode);
        PifAbort(MODULE_NB_ABORT, FAULT_INVALID_FUNCTION);         /* Invalid Function */
        break;

    }
    NbCheckOffline(0);                     /* Check offline counter */
}

/*
*===========================================================================
** Synopsis:    VOID  NbExecuteWait(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute wait state function
			This function's major behavior depends on whether we are a
			satellite terminal or a Master or Backup Master terminal.

			If we are a satellite terminal then we do not need to worry
			about any of the "Who's the Master Terminal" negotiation between
			the Master and Backup Master terminal.  Nor do we need to be a
			party to the "Are you up to date" status determination between
			the MAster and Backup Master terminal.

			Therefore, if we are a satellite terminal, we will just set our
			send buffer function code to NB_FCSTARTONLINE, change our state
			to the NB_NORMALSTATE state, and we are done with waiting.

			Otherwise, we have to perform the waiting step as a part of the
			Master terminal and Backup Master terminal negotiations and status
			communication.
*===========================================================================
*/
VOID  NbExecuteWait(USHORT fsSystemInf)
{
    SHORT      sRet;

    sRet = NbCheckUAddr();                  /* Check Unique address */

    if (sRet == NB_SUCCESS) {               /* If satellite, then return */ 
        NbSetFCtoSndBuf(NB_FCSTARTONLINE);  /* Set function code to send buffer */
        usNbState =  NB_NORMALSTATE;        /* Set state to NORMAL */
        return;
    }

    switch (usNbState) {

    case NB_WAITSTATE :                                   /* Execute wait */
        usNbRcvFMT = 0;                                   /* Clear FMT */
        usNbRcvFBM = 0;                                   /* Clear FBM */
        usNbNetErr = 0;                                   /* Received counter */
        sNbSavRes  = 0;                                   /* Send counter     */

        /* --- clear network buffer before normal state (02/08/2001) --- */
        NbNetClear();

        if ( fsSystemInf & NB_STARTASMASTER ) {      /* Starts as master */
            NbSetFCtoSndBuf(NB_FCSTARTASMASTER);          /* Set function code to send buffer */
            usNbState =  NB_SENDSTATE;                    /* Set state to SEND */
            return;
        }

        if ( fsSystemInf & NB_STARTONLINE  ) {       /* Starts Online */
            NbSetFCtoSndBuf(NB_FCSTARTONLINE);            /* Set function code to send buffer */
            usNbState =  NB_NORMALSTATE;                  /* Set state to NORMAL */
            return;
        }

        usNbState =  NB_NORMALSTATE;        /* Set state to NORMAL */
        break;

    default :      /* Delay to Send */
        if ( sRet == NB_BMASTERUADD ) {
            PifSleep(NB_SLEEPTIME);         /* If B-Master, then sleep 2 sec. time */
        } else {                            /* Delay 2 seconds timer */
            NbNetDelaytoSend();             /* Delay time until reach to next send time */    
        }
        usNbState =  NB_SENDSTATE;          /* Set state to SEND */
        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID  NbExecuteSend(USHORT fsSystemIn
*     Input:    fsSystemInf
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute Send state function
*===========================================================================
*/
VOID  NbExecuteSend(USHORT fsSystemInf)
{
    SHORT     sRet;
    USHORT    usTadd;

    usNbState =  NB_NORMALSTATE;         /* Set state to NORMAL */
 
    usTadd = NbCheckUAddr();               /* Check Unique address */

	// If we are a Satellite Terminal then we do not send
	// Notice Board messages so we will just return back doing nothing.
    if (usTadd == NB_SUCCESS) {            /* If satellite, then return */ 
        return;
    }

    if ( 0 == NbSndBuf.NbConMes.usNbFunCode) { /* If no function code, then return */
        return;
    }

    NbMakeConfirmMessage(usTadd, fsSystemInf); /* Check offline counter */

    if ( NbSndBuf.NbMesHed.auchFaddr[3] == (UCHAR)0 ) { 
        NbMakeSndMessage();         /* Make a message to send */
        NbPreCheckOffline();        /* Check offline counter */
    } 
    
    sRet = NbNetSend();                  /* Send data */

    if ( PIF_ERROR_NET_POWER_FAILURE != sRet ) {
        if ( usTadd == NB_MASTERUADD) {
            if ( NbSndBuf.NbMesHed.auchFaddr[3] != (UCHAR)0 ) { 
                if ( sRet < 0 )  {
                    NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set all address */
                    usNbState =  NB_SENDSTATE;                  /* Set state to SEND */
                    return;
                }
            }
        } else {
            if ( NbSndBuf.NbMesHed.auchFaddr[3] != (UCHAR)0 ) { 
                NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set All address */
                return;
            }
        }
    }

    if ( sRet < 0 )  {
        NbErrResHand(sRet) ;             /* Execute Error Response Handling */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    NbMainCE(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Main NB Idle for PC I/F Reset Log, V3.3
*===========================================================================
*/
VOID    THREADENTRY NbMainCE(VOID)
{
    for (;;) {
        PifSleep(30000);

        /* --- Retry for PC i/F Reset Log,  V3.3 --- */
        UifCPRetryUns();
    }
}

/*
*===========================================================================
** Synopsis:   VOID   NbPrintPowerDown(SHORT sStatus)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:     Print power Down Log on Journal.            Saratoga
*===========================================================================
*/
VOID   NbPrintPowerDown(SHORT sStatus)
{
    FSCTBL      Ring;
    UCHAR       uchDummy = 0;

    Ring.uchMajor = FSC_POWER_DOWN;
    Ring.uchMinor = FSC_MINOR_SUSPEND;  /* power down event is sent after suspend recovery */
    /*
    if (sStatus == PIF_ERROR_NET_POWER_FAILURE) {
        Ring.uchMinor = FSC_MINOR_POWER_DOWN;
    } else {
        Ring.uchMinor = FSC_MINOR_SUSPEND;
    }*/
    uchPowerDownLog = Ring.uchMinor;

    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */
}

/*====== End of Source File ======*/