/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 7450 POS *             AT&T GIS Japan, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995				||
||    <|\/~               ~\/|>												||
||   _/^\_                 _/^\_											||
||																			||
\****************************************************************************/

/*==========================================================================*\
*	Title:
*	Category:
*	Program Name:
* ---------------------------------------------------------------------------
*	Compiler:		MS-C Ver. 6.00 A by Microsoft Corp.
*	Memory Model:
*
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*			 |			|										|
\*==========================================================================*/

/*==========================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+					I N C L U D E    F I L E s								+
;+																			+
\*==========================================================================*/

/* --- basic OS header --- */
#if defined(SARATOGA) 
# include	<windows.h>							/* Windows header			*/
# include   "pif.h"
#elif defined(SARATOGA_PC)
# include	<windows.h>							/* Windows header			*/
# include   "pif.h"
#elif	defined (POS7450)						/* 7450 POS model ?				*/
#include	<phapi.h>							/* PharLap header			*/
#elif defined (POS2170)						/* 2170 POS model ?				*/
# define	NET_DATAGRAM_ACK
# include	"ecr.h"								/* ecr common header		*/
# include	"termcfg.h"							/* terminal configuration	*/
#elif defined (SERVER_NT)					/* WindowsNT model ?			*/
# include	<windows.h>							/* Windows header			*/
#endif
/* --- C-library header --- */

#include	<memory.h>						/* memxxx() C-library header	*/

/* --- our common headers --- */

#include	"piftypes.h"					/* type modifier header			*/
#include	"pifnet.h"						/* PifNet common header			*/
#include	"netmodul.h"					/* my module header				*/

/*==========================================================================*\
;+																			+
;+					L O C A L     D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- echo server thread I/F --- */

typedef struct _ECHARG {					/* argument for NetEchoServer()	*/
	USHORT	usStatus;							/* initialized status		*/
	EVTOBJ	evtAck;								/* acknowledge signal		*/
	USHORT	usPort;								/* port no.					*/
} ECHARG, *PECHARG;

/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- echo server thread --- */

VOID	NetEchoServer(PECHARG);					/* echo server thread		*/

/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/


/*==========================================================================*\
;+																			+
;+					E c h o   S e r v e r   T h r e a d						+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Spawn echo server thread
;
;   format : USHORT		SpawnEchoServer(usPort);
;
;   input  : USHORT		usPort;			- port number
;
;   output : USHORT		usRet;			- return status
;
;========================================================================
**/

USHORT	SpawnEchoServer(USHORT usPort)
{
	ULONG		idRet;
	static ECHARG		argServer;
#ifdef DEBUG_PIFNET_OUTPUT
	TCHAR  wszDisplay[500];
#endif

#ifdef DEBUG_PIFNET_OUTPUT
	debugDump (__FILE__, __LINE__, ((char *)&usPort) - 4, 26);
#endif

	/* --- make argument --- */

	argServer.usStatus = 0;						/* init. execution status	*/
	argServer.usPort   = usPort;				/* port no. for echo server	*/

	/* --- create an event signal --- */

	SysEventCreate(&(argServer.evtAck), TRUE);	/* create a signal			*/

	/* --- create network manager thread --- */
	if ((idRet = SysBeginThread (NetEchoServer, 0, 0, PRIO_NET_ECHO, &argServer)) != -1L) {

#ifdef DEBUG_PIFNET_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  SysBeginThread NetEchoServer idRet:%i  Thread: %x\n"),
		__FILE__, __LINE__, idRet, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif

		SysEventWait(&(argServer.evtAck), WAIT_FOREVER);

#ifdef DEBUG_PIFNET_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  SysBeginThread NetEchoServer SysEventWait Thread: %x\n"),
		__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif

	}

	/* --- close the event signal --- */

	SysEventClose(&(argServer.evtAck));			/* close event signal		*/

	/* --- exit ... --- */
#ifdef DEBUG_PIFNET_OUTPUT
	wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  SysEventClose Thread: %x\n"),
	__FILE__, __LINE__, GetCurrentThreadId ());
	OutputDebugString(wszDisplay);

	debugDump (__FILE__, __LINE__, ((char *)&usPort) - 4, 26);
	return 0;
#endif
	return ((idRet == -1L) ? PIF_ERROR_NET_CRITICAL : argServer.usStatus);
}

/**
;========================================================================
;
;   function : Echo Server thread
;
;   format : VOID		NetEchoServer(pArg);
;
;   input  : PECHARG	pArg;
;
;   output : nothing
;
;========================================================================
**/

VOID	NetEchoServer(PECHARG pArg)
{
#if	defined (POS2170)
	UCHAR		aucBuffer[XGSIZE(PIF_LEN_UDATA)];	/* allocate ??? bytes	*/
#else
	UCHAR		aucBuffer[XGSIZE(1500)];		/* allocate 1.5K bytes		*/
#endif
	SHORT		sRet;
	USHORT		usNet, usTimer, usBytes, usFunc, usDiag;
	XGHEADER	hdrAddress;
	PRMTFNC		pDiag;
#ifdef DEBUG_PIFNET_OUTPUT
	TCHAR  wszDisplay[128];
#endif
	/* --- initialize --- */

	memset(&hdrAddress, '\0', sizeof(hdrAddress));
	hdrAddress.usFport = ANYPORT;				/* no specific target addr.	*/
	hdrAddress.usLport = pArg->usPort;			/* my port number			*/
	sRet               = 0;						/* assume no error			*/

	/* --- create my port --- */
#if defined(SARATOGA_PC)
	sRet  = NetOpenEx(&hdrAddress);				/* create my port			*/
#else
	sRet  = NetOpen(&hdrAddress);				/* create my port			*/
#endif

#ifdef DEBUG_PIFNET_OUTPUT
    {
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  after NetOpen sRet:%i\n"), __FILE__, __LINE__, sRet);
		OutputDebugString(wszDisplay);
	}
#endif

	usNet = (USHORT)((sRet < 0) ? (-1) : sRet);	/* compute network handle	*/

#ifdef DEBUG_PIFNET_OUTPUT
    {
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  network handle usNet:%i\n"), __FILE__, __LINE__, usNet);
		OutputDebugString(wszDisplay);
	}
#endif

	sRet  =          (sRet < 0) ? sRet : 0;		/* normalize error status	*/

	/* --- compute response buffer size for diag func. --- */

	usDiag = sizeof(aucBuffer) - XGSIZE(0) - FAROF(auchData, RMTFNC);

	/* --- respond status --- */

	pArg->usStatus = (USHORT)sRet;				/* respond my status		*/

	/* --- issue a signal --- */

	SysEventSignal(&(pArg->evtAck));			/* tell I'm ready			*/

	/* --- do my job --- */
#ifdef DEBUG_PIFNET_OUTPUT
    {
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  SysEventSingnal sRet:%i\n"), __FILE__, __LINE__, sRet);
		OutputDebugString(wszDisplay);
	}
#endif

	while (usNet != (USHORT)(-1)) {				/* do my job				*/
#ifdef DEBUG_PIFNET_OUTPUT
		{
			wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  in While Loop usNet:%i\naddress aucBuffer:%x size = %i\n")
				, __FILE__, __LINE__, usNet, aucBuffer, sizeof(aucBuffer));
			OutputDebugString(wszDisplay);
		}
#endif
			memset(aucBuffer,0xCC, sizeof(aucBuffer));
		/* --- wait for a request message --- */

		if ((sRet = NetReceive(usNet, aucBuffer, sizeof(aucBuffer))) < 0) {
#ifdef DEBUG_PIFNET_OUTPUT
			{
				wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  NetReceive sRet:%i\n"), __FILE__, __LINE__, sRet);
				OutputDebugString(wszDisplay);
			}
#endif
			SysSleep(300);							/* discard my CPU time	*/
			continue;								/* try again !			*/
		}
		
#ifdef DEBUG_PIFNET_OUTPUT
		{
			wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  NetReceive without SysSleep sRet:%i\n"), __FILE__, __LINE__, sRet);
			OutputDebugString(wszDisplay);
		}
#endif
		/* --- assume remote diag message --- */

		pDiag   = (PRMTFNC)(aucBuffer + XGSIZE(0));
		usBytes = FAROF(auchData, RMTFNC) + pDiag->usBytes;
		usFunc  = pDiag->usSign;				/* get function code		*/
		usFunc  = (usFunc == PIF_NET_REMOTE_CALL)         ? usFunc : 0;
		usFunc  = ((USHORT)sRet == (usBytes + XGSIZE(0))) ? usFunc : 0;
		usTimer = 0;							/* no send ack. timer		*/

		/* --- what function ? --- */

		switch (usFunc) {						/* what function ?			*/
		case PIF_NET_REMOTE_CALL:				/* remote diag ?			*/
			pDiag->usAux    = (USHORT)PifNetDiag(pDiag->auchData,
												 pDiag->auchData,
												 usDiag);
			pDiag->usBytes  = FAROF(auchData, DGINFO);
			pDiag->usBytes += ((PDGINFO)(pDiag->auchData))->usBytes;
			usBytes         = FAROF(auchData, RMTFNC) + pDiag->usBytes;
			usBytes        += XGSIZE(0);
			break;
		default:								/* maybe echo back ...		*/
			usBytes  = (USHORT)sRet;				/* received data length	*/
			usTimer  = ((PXGRAM)aucBuffer)->auchData[0];
			usTimer  = (sRet == FAROF(auchData, XGRAM)) ? 0 : usTimer;
			usTimer *= 1000;
			break;
		}

		/* --- set send timer value --- */

		sRet = NetControl(usNet, PIF_NET_SET_STIME, usTimer);
#ifdef DEBUG_PIFNET_OUTPUT
		{
			wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  NetControl sRet:%i\n"), __FILE__, __LINE__, sRet);
			OutputDebugString(wszDisplay);
		}
#endif

		/* --- dispose the message --- */

		sRet = NetSend(usNet, aucBuffer, usBytes);	/* echo back			*/
#ifdef DEBUG_PIFNET_OUTPUT
		{
			wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  NetSend sRet:%i\n"), __FILE__, __LINE__, sRet);
			OutputDebugString(wszDisplay);
		}
#endif

	}

	/* --- well mannered ... --- */
#ifdef DEBUG_PIFNET_OUTPUT
	{
	wsprintf(wszDisplay, TEXT("FILE: %s  LINE: %n  SysEndThread call\n"), __FILE__, __LINE__);
	OutputDebugString(wszDisplay);
	}
#endif
	
	SysEndThread();
}

/*==========================================================================*\
;+						E n d    O f    P r o g r a m						+
\*==========================================================================*/
