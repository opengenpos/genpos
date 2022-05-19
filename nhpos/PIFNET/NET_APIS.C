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

/* --- global haeders --- */
# include	<windows.h>							/* Windows header			*/

/* --- C-library header --- */
#include	<memory.h>						/* memxxx() C-library header	*/
#include	<stdarg.h>						/* va_arg() etc.				*/

# include   <ecr.h>
# include   <pif.h>

/* --- our common headers --- */
#include	"piftypes.h"					/* type modifier header			*/
#include	"pifnet.h"						/* PifNet common header			*/
#include	"netmodul.h"					/* my module header				*/

/*==========================================================================*\
;+																			+
;+					L O C A L     D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- queue ID by default --- */
#define		QID_MANAGER		0xFFF0				/* queue ID of managers		*/

/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- initialization & finalization --- */
/* --- sub-routines --- */
USHORT	InvokeFunction(PMSGHDR, USHORT);		/* invoke a function		*/

/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- network module status --- */
static	fNetWorking = FALSE;					/* initialization status	*/

/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/


/*==========================================================================*\
;+																			+
;+				I n i t i a l i z e  &  F i n a l i z e						+
;+																			+
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+						A P I   P r o g r a m								+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Initialize a network
;
;   format : SHORT		PifNetInitialize(pInfo);
;
;   input  : PNETINF	pInfo;		- ptr. to information data
;
;   output : SHORT		sRet;		- status
;
;========================================================================
**/
SHORT NETENTRY	NetInitialize(PNETINF pInfo)
{
	USHORT		usRet;

	if (fNetWorking) {							/* I'm working ?			*/
		return (PIF_ERROR_NET_CRITICAL);			/* cannot do my job		*/
	}

	/* --- spawn network manager thread --- */
	if (usRet = SpawnNetManager(QID_MANAGER, pInfo)) {
		return (usRet);
	}

	/* --- mark working status --- */
	fNetWorking = TRUE;							/* set I'm working !		*/

	/* --- echo server thread --- */
	usRet = SpawnEchoServer(PORT_ECHO_SERVER);			/* spawn echo server		*/

	return (PIF_OK);
}

SHORT NETENTRY	NetSetClusterInfo(ULONG  *pulClusterInfo)
{
	extern ULONG  ulMasterTerminalSourceAddr;
	extern ULONG  ulBackupMasterTerminalSourceAddr;

	if (pulClusterInfo) {
		ulMasterTerminalSourceAddr = pulClusterInfo[0];
		ulBackupMasterTerminalSourceAddr = pulClusterInfo[1];
	}
	return 0;
}

/**
;========================================================================
;
;   function : Open a network
;
;   format : SHORT		PifNetOpen(pHeader);
;
;   input  : PXGHEADER	pHeader;	- ptr. to parameters
;
;   output : SHORT		sHandle;	- status
;
;========================================================================
**/
SHORT NETENTRY	NetOpen(PXGHEADER pHeader)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;
	POPNDAT		pOpen;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
	pOpen    = (POPNDAT)(&(pManager->datOption));

	/* --- make command packet --- */
	pManager->usHandle  = 0;					/* network handle			*/
	pManager->usCommand = CMD_OPEN;				/* command code				*/
	pManager->usLength  = sizeof(OPNDAT);		/* optional data length		*/

	/* --- make optional data --- */
	pOpen->ulFaddr = *(LPULONG)(pHeader->auchFaddr);	/* target IP address*/
	pOpen->usFport = pHeader->usFport;			/* target port no.			*/
	pOpen->usLport = pHeader->usLport;			/* local port no.			*/

	/* --- invoke a function --- */
	usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER);
	usRet = (USHORT)(usRet ? usRet : pManager->usReturn);

#ifdef _DEBUG
	debugDump (__FILE__, __LINE__, ((char *)&pHeader)-4, 62);
#endif

	return (usRet);
}

/**
;========================================================================
;
;   function : Receive from a network
;
;   format : SHORT		PifNetReceive(usHandle, pvBuffer, usBytes);
;
;   input  : USHORT		usHandle;		- network handle
;			 PVOID		pvBuffer;		- ptr. to a buffer
;			 USHORT		usBytes;		- no. of bytes
;
;   output : SHORT		sRet;			- status
;
;========================================================================
**/
SHORT NETENTRY	NetReceive(USHORT usHandle, LPVOID pvBuffer, USHORT usBytes)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;
	PRCVDAT		pReceive;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
	pReceive = (PRCVDAT)(&(pManager->datOption));

	/* --- make command packet --- */
	pManager->usHandle  = usHandle;				/* network handle			*/
	pManager->usCommand = CMD_RECEIVE;			/* command code				*/
	pManager->usLength  = sizeof(RCVDAT);		/* optional data length		*/
	pReceive->usBytes   = usBytes;				/* receive buffer size		*/
	pReceive->pvBuffer  = pvBuffer;				/* ptr. to receive buffer	*/
	pReceive->pSender   = _NULL;				/* no. sender info.			*/

	/* --- invoke a function --- */
	if (usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER)) {
		return (usRet);
	}

	return (pManager->usReturn);
}

/**
;========================================================================
;
;   function : Receive from a network
;
;   format : SHORT		PifNetReceive2(usHandle, pvBuffer, usBytes, pSender);
;
;   input  : USHORT		usHandle;		- network handle
;			 LPVOID		pvBuffer;		- ptr. to a buffer
;			 USHORT		usBytes;		- no. of bytes
;			 PXGADDR	pSender;		- ptr. to get sender info.
;
;   output : SHORT		sRet;			- status
;
;========================================================================
**/
SHORT NETENTRY NetReceive2(USHORT usHandle, LPVOID pvBuffer, USHORT usBytes, PXGADDR pSender)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;
	PRCVDAT		pReceive;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
	pReceive = (PRCVDAT)(&(pManager->datOption));

	/* --- make command packet --- */
	pManager->usHandle  = usHandle;				/* network handle			*/
	pManager->usCommand = CMD_RECEIVE;			/* command code				*/
	pManager->usLength  = sizeof(RCVDAT);		/* optional data length		*/
	pReceive->usBytes   = usBytes;				/* receive buffer size		*/
	pReceive->pvBuffer  = pvBuffer;				/* ptr. to receive buffer	*/
	pReceive->pSender   = pSender;				/* ptr. to sender info.		*/

	/* --- invoke a function --- */
	if (usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER)) {
		return (usRet);
	}

	return (pManager->usReturn);
}

/**
;========================================================================
;
;   function : Send to a network
;
;   format : SHORT		PifNetSend(usHandle, pvBuffer, usBytes);
;
;   input  : USHORT		usHandle;		- network handle
;			 LPVOID		pvBuffer;		- ptr. to data to be sent
;			 USHORT		usBytes;		- no. of bytes
;
;   output : SHORT		sRet;			- status
;
;========================================================================
**/
SHORT NETENTRY	NetSend(USHORT usHandle, LPVOID pvBuffer, ULONG ulBytes)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;
	PSNDDAT		pSend;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	// check that number of bytes does not exceed the size of a ushort.
	if (ulBytes > 0x0000ffffL) {
		NHPOS_ASSERT(ulBytes < 0x0000ffffL);
		return PIF_ERROR_NET_BAD_DATA;
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
	pSend    = (PSNDDAT)(&(pManager->datOption));

	/* --- make command packet --- */
	pManager->usHandle  = usHandle;				/* network handle			*/
	pManager->usCommand = CMD_SEND;				/* command code				*/
	pManager->usLength  = sizeof(SNDDAT);		/* optional data length		*/
	pSend->usBytes      = (USHORT)ulBytes;				/* send data length			*/
	pSend->pvBuffer     = pvBuffer;				/* ptr. to send data		*/

	/* --- invoke a function --- */
	if (usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER)) {
		return (usRet);
	}

	return (pManager->usReturn);
}

/**
;========================================================================
;
;   function : Control a network
;
;   format : SHORT		PifNetControl(usHandle, usFunc, ...);
;
;   input  : USHORT		usHandle;		- network handle
;			 USHORT		usFunc;			- function code
;			 ??????		...;			- specific parameters
;
;   output : SHORT		sRet;			- status
;
;========================================================================
**/
SHORT NETENTRY	NetControl(USHORT usHandle, USHORT usFunc, ULONG *pulArg, ...)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet, usValue;
	PMGRMSG		pManager;
	PCNTDAT		pControl;
	PXGNAME		pName;
	PXGADDR		pAddr;
	va_list		pvaArg;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
	pControl = (PCNTDAT)(&(pManager->datOption));

	/* --- make command packet --- */
	pManager->usHandle   = usHandle;			/* network handle			*/
	pManager->usCommand  = CMD_CONTROL;			/* command code				*/
	pManager->usLength   = sizeof(CNTDAT);		/* optional data length		*/
	pControl->usFunction = usFunc;				/* sub-function code		*/

	/* --- initialize to get a parameter --- */
	va_start(pvaArg, (*pulArg));					/* top of variable argument	*/

	/* --- do each function --- */
	switch (usFunc) {							/* what function ?			*/
	case PIF_NET_SET_MODE:						/* set a mode ?				*/
		pControl->usModeMode = va_arg(pvaArg, USHORT);	/* function mode	*/
		break;
	case PIF_NET_SET_TIME:						/* set timeout value ?		*/
		usValue              = va_arg(pvaArg, USHORT);	/* timeout value	*/
		pControl->ulTimeTime = (ULONG)usValue;			/* timeout value	*/
		break;
	case PIF_NET_SET_TIME_EX:					/* set timeout value ?		*/
		pControl->ulTimeTime = va_arg(pvaArg, ULONG);	/* timeout value	*/
		break;
	case PIF_NET_SET_STIME:						/* set timeout value ?		*/
		pControl->usStimeTime = va_arg(pvaArg, USHORT);	/* timeout value	*/
		break;
	case PIF_NET_SET_REMOTE:					/* set remote address ?		*/
		pAddr                = va_arg(pvaArg, PXGADDR);
		pControl->ulSendAddr = *(LPULONG)(pAddr->auchAddr);
		pControl->usSendPort =            pAddr->usPort;
		break;
	case PIF_NET_SET_EVENT:						/* set power down event ?	*/
		pControl->fEventEvent = va_arg(pvaArg, BOOL);	/* option value		*/
		break;
	case PIF_NET_DIAG:							/* diagnostics ?			*/
		pControl->pDiagData   = va_arg(pvaArg, PDGDATA);
		pControl->pDiagBuffer =	va_arg(pvaArg, PDGINFO);
		pControl->usDiagBytes = va_arg(pvaArg, USHORT);
		break;
	case PIF_NET_SET_LOG:						/* set LOG function addr ?	*/
		pControl->pfnAddress = va_arg(pvaArg, PFUNLOG);
		break;
	case PIF_NET_GET_USER_INFO:
        pControl->datFunc.usrInfo.pfnAddr = va_arg(pvaArg, PPIFUSRTBLINFO);
		break;
	case PIF_NET_RESET_SEQNO:					// reset the message sequence number
	case PIF_NET_CLEAR:							/* clear queued messages ?	*/
	case PIF_NET_GET_NAME:						/* get local name ?			*/
	case PIF_NET_SET_STANDBY:					/* set stand by state ?		*/
	default:									/* others ...				*/
		break;
	}

	/* --- invoke a function --- */
	if (! (usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER))) {
		/* --- respond values --- */
		usRet = pManager->usReturn;
		switch (usFunc) {
			case PIF_NET_GET_NAME:		/* get local name ?			*/
				pName                        = va_arg(pvaArg, PXGNAME);
				*(LPULONG)(pName->auchLaddr) = pControl->ulNameAddr;
				*(LPULONG)(pName->auchSaddr) = pControl->ulLastSourceAddr;
				pName->usIpPortNo            = pControl->usLastIpPort;
				pName->usLport               = pControl->usNamePort;
				break;
			default:								/* others ...				*/
				break;
		}
	}

	/* --- clean up for variable arguments --- */
	va_end(pvaArg);								/* clean up arg. pointer	*/

	return (usRet);
}

/**
;========================================================================
;
;   function : Close a network
;
;   format : SHORT		PifNetClose(usHandle);
;
;   input  : USHORT		usHandle;		- network handle
;
;   output : SHORT		sRet;			- status
;
;========================================================================
**/
SHORT NETENTRY	NetClose(USHORT usHandle)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_ERROR_NET_NOT_WORK);			/* cannot do my job		*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);

	/* --- make command packet --- */
	pManager->usHandle  = usHandle;				/* network handle			*/
	pManager->usCommand = CMD_CLOSE;			/* command code				*/
	pManager->usLength  = 0;					/* optional data length		*/

	/* --- invoke a function --- */
	if (usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER)) {
		return (usRet);
	}

	return (pManager->usReturn);
}

/**
;========================================================================
;
;   function : Shutdown a network
;
;   format : SHORT		PifNetFinalize();
;
;   input  : nothing
;
;   output : SHORT		sRet;		- return status
;
;========================================================================
**/
SHORT NETENTRY	NetFinalize(VOID)
{
	UCHAR		aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
	USHORT		usRet;
	PMGRMSG		pManager;

	/* --- is working ? --- */
	if (! fNetWorking) {						/* I'm working ?			*/
		return (PIF_OK);							/* ignore, but completed*/
	}

	/* --- initialize --- */
	pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);

	/* --- make command packet --- */
	pManager->usHandle  = (USHORT)(PIF_NET_HANDLE);	/* network handle		*/
	pManager->usCommand = CMD_SHUTDOWN;			/* command code				*/
	pManager->usLength  = 0;					/* optional data length		*/

	/* --- invoke a function --- */
	usRet = InvokeFunction((PMSGHDR)aucMessage, QID_MANAGER);

	usRet = (USHORT)(usRet ? usRet : pManager->usReturn);

	/* --- adjust working status by enforce --- */
	fNetWorking = FALSE;						/* I'm sleeping !			*/

	return (usRet);
}

/*==========================================================================*\
;+																			+
;+						S u b - R o u t i n e s								+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Invoke a function
;
;   format : USHORT		InvokeFunction(pMessage, usTarget);
;
;   input  : PMSGHDR	pMessage;		- ptr. to a message
;			 USHORT		usTarget;		- target queue ID
;
;   output : USHORT		usRet;			- status
;
;========================================================================
**/

USHORT	InvokeFunction(PMSGHDR pMessage, USHORT usTarget)
{
	USHORT	usRet;
	BOOL	bRet;

	/* --- initialize & complete the message --- */
	usRet              = (USHORT)(PIF_ERROR_NET_QUEUE);
	pMessage->usSender = 0;						/* set sender ID			*/

	/* --- prepare blocking state --- */
	SysEventCreate(&(pMessage->evtSignal), TRUE);	/* create event signal	*/

	/* --- request a function --- */
	if (! (usRet = SysWriteQueue(usTarget, &(pMessage->queControl)))) {
		/* --- wait for done --- */
		bRet = SysEventWait(&(pMessage->evtSignal), WAIT_FOREVER);
		/* --- examine message's reliability --- */
		usRet = (pMessage->usSender != usTarget) ? PIF_ERROR_NET_CRITICAL : usRet;
	}

	/* --- discard resources --- */
	SysEventClose(&(pMessage->evtSignal));		/* close event handle		*/
    
	return (usRet);
}
/*==========================================================================*\
;+						E n d    O f    P r o g r a m						+
\*==========================================================================*/
