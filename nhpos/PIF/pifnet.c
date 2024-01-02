/* pifnet.c : PIF winsock control functions */
/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : PIFNET MODULE
:   Category       : PIFNET MODULE, NCR 2170 US Hospitality Application
:   Program Name   : PIFNET.C (network layer interface)
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
:  Abstract:
:   PifNetSystemInfo           ; obtain network layer statistics
:   PifNetAddJoin              ; Mobile Register join functionality
:   PifNetRemoveJoin           ; Mobile Register join functionality
:   PifNetStartupDiscoverNet
:   PifNetStartup              ; main PifNet network layer initialization
:   PifNetOpen                 ; open a terminal UDP port in standard, general mode
:   PifNetOpenEx               ; extended version of PifNetOpen() allowing terminal, direct UDP, and TCP.
:   PifNetSend                 ; send a message to terminal or broadcast depending on IP address
:   PifNetReceive              ; receive a message if available or time out
:   PifNetControl              ; change connection management data such as time out period
:   PifNetLoad                 ; load the PifNet.DLL and set up the calling table. Old style code.
:  ---------------------------------------------------------------------
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 07/15/15 : 02.02.01 : R.Chambers  : changes to PifLog() to provide more data on errors.
-------------------------------------------------------------------------*/

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"

#include <mbstring.h>

#include <winsock2.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"
#include "pifnet.h"
#include <unicodeFixes.h>
#include "scf.h"

#define DEVICENAME _T("NCRPOS-")
#define PIF_NETPORT   2172
CRITICAL_SECTION g_NetCriticalSection;

static PIFHANDLETABLE   aPifNetTable[PIF_MAX_NET];
static CHAR szPifNetPort[] = "2172";

// fsNetStarted used to step through the states to an initialized
// PifNet library.
// state values are as follows:
//   0  PifNetStartup() not called yet
//   1  PifNetOpen() or PifNetOpenEx() not called yet
//   2  fully initialized
static USHORT      fsNetStarted = 0;

static XGRAMEX   aPifNetXgram[PIF_MAX_NET];
static XGRAMEX   aPifNetRcvXgram[PIF_MAX_NET];

static SHORT  PifNetOpenG(CONST XGHEADER FAR *pHeader);
static SHORT  PifNetSendG(USHORT usNet, CONST VOID FAR *pBuffer, ULONG ulBytes);
static SHORT  PifNetReceiveG(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes);
static SHORT  PifNetControlG(USHORT usNet, USHORT usFunc, ...);
static SOCKET PifNetOpenD(CONST XGHEADER FAR *pHeader);
static SHORT  PifNetSendD(USHORT usNet, CONST VOID FAR *pBuffer, ULONG ulBytes);
static SHORT  PifNetReceiveD(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes);
static SHORT  PifNetControlD(USHORT usNet, USHORT usFunc, ...);
static SOCKET PifNetOpenTcp(CONST XGHEADER FAR *pHeader);
static SHORT  PifNetSendTcp(USHORT usNet, CONST VOID FAR *pBuffer, ULONG ulBytes);
static SHORT  PifNetReceiveTcp(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes);
static SHORT  PifNetControlTcp(USHORT usNet, USHORT usFunc, ...);

static USHORT PifNetCheckPowerDown(USHORT usHandle);
static USHORT PifNetInitialize(VOID);
static VOID   PifInitHostsAddress(VOID);
static SHORT  PifGetDestHostAddress(UCHAR *puchDestAddr, UCHAR *puchSrcAddr);
SHORT  PifGetHostAddress(UCHAR *puchLocalAddr, UCHAR *puchHostAddr, TCHAR *wszHostName2);
SHORT  PifCheckHostsAddress(USHORT usSrc, UCHAR *puchSrcAddr);

static SHORT PifNetLoad(LPCTSTR    lpszName);
static VOID  PifNetFree();
static SHORT _NetInitialize(PNETINF);
static SHORT _NetOpen(PXGHEADER);
static SHORT _NetSetClusterInfo(ULONG  *pulClusterInfo);
static SHORT _NetReceive(USHORT, VOID  *, USHORT);
static SHORT _NetReceive2(USHORT, VOID  *, USHORT, PXGADDR);
static SHORT _NetSend(USHORT, VOID  *, ULONG);
static SHORT _NetControl(USHORT, USHORT, ...);
static SHORT _NetClose(USHORT);
static SHORT _NetFinalize(VOID);


/* PIFNET.DLL Interface routines */

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef SHORT (WINAPI *PLD_NETINITALIZE)(PNETINF);
typedef SHORT (WINAPI *PLD_NETOPEN)(PXGHEADER);
typedef SHORT (WINAPI *PLD_NETRECEIVE)(USHORT, VOID  *, USHORT);
typedef SHORT (WINAPI *PLD_NETRECEIVE2)(USHORT, VOID  *, USHORT, PXGADDR);
typedef SHORT (WINAPI *PLD_NETSEND)(USHORT, VOID  *, ULONG);
typedef SHORT (WINAPI *PLD_NETCONTROL)(USHORT, USHORT, ...);
typedef SHORT (WINAPI *PLD_NETCLOSE)(USHORT);
typedef SHORT (WINAPI *PLD_NETFINALIZE)(VOID);
typedef SHORT (WINAPI *PLD_NETCLUSTERINFO)(ULONG *);

typedef struct _NETLOAD_FUNCTION {
    PLD_NETINITALIZE      NetInitialize;
    PLD_NETOPEN           NetOpen;
    PLD_NETRECEIVE        NetReceive;
    PLD_NETRECEIVE2       NetReceive2;
    PLD_NETSEND           NetSend;
    PLD_NETCONTROL        NetControl;
    PLD_NETCLOSE          NetClose;
    PLD_NETFINALIZE       NetFinalize;
	PLD_NETCLUSTERINFO    NetSetClusterInfo;
} NETLOAD_FUNCTION, *PNETLOAD_FUNCTION;

typedef struct _PIFNETCNFBLK {
    NETLOAD_FUNCTION   Func;                   /* DLL Address */
    HINSTANCE       hInstance;
    HANDLE          hHandle;
} PIFNETCNFBLK, *PPIFNETCNFBLK;

PIFNETCNFBLK   PifPifNetBlk;

typedef struct _PROCADR {				// array of procedure address
	INT			iIndex;						// index to control
	LPCTSTR		lpName;						// ptr. to a proc. name
} PROCADR, *PPROCADR;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

static TCHAR wszLastTerminalHostNumber[32] = {0, 0};

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetSystemInfo (                   **
**              PIF_NETSYSTALLY *pNetTally,                         **
**              PIF_NETUSERTALLY *pNetUsers,                        **
**              USHORT *pusUsers)                                   **
**                                                                  **
**              pNetTally:  pointer to buffer for the system tally  **
**              pNetUsers:  point to buffer for user by user tally  **
**              pusUsers:  point to USHORT to hold count of users   **
**                                                                  **
**  return:     handle of network                                   **
**                                                                  **
**  Description:Retrieve the network layer statistics concerning    **
**              network communication with other terminals in       **
**              the cluster.                                        **
**              If pNetUsers is NULL (0) then user by user tally    **
**              is not provided.  This allows caller to first ask   **
**              for system tally to see how many users there are    **
**              and how large to make pNetUsers on a second call.   **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetSystemInfo (PIF_NETSYSTALLY *pNetTally, PIF_NETUSERTALLY *pNetUsers, USHORT *pusUsers)
{
	PDGINFO pInfo;
	PDGDATA pData;
	PREQINF pReqInfo;
	PUSRINF pUserInfo;
	PRSPUSR pUserTally;
	PRSPSYS pSystemInfo;
	USHORT nUsers = 0;
	USHORT iUsers = 0;
	// WARNING:  added additional USRTLY counters may require change in this buffer size allocation.
	//           may need to multiply sizeof(RSPSYS) by a larger amount to create the space needed.
	USHORT  usSizeOfInfo = (sizeof(DGINFO) + sizeof(RSPSYS) * 4 + sizeof(USRINF) * 20);
	static UCHAR PifNetworkSystemInfoBuffer [sizeof(DGINFO) + sizeof(RSPSYS) * 4 + sizeof(USRINF) * 30];
	static UCHAR PifNetworkSystemInfoData [sizeof(DGDATA) + sizeof(REQINF)];

	if (pusUsers)
	{
		nUsers = *pusUsers;
	}
	pData = (PDGDATA)&PifNetworkSystemInfoData[0];
	pReqInfo = (PREQINF) (pData->auchData);

	pData->usFunc = DIAG_GET_SYSTEM;
	pData->usBytes = sizeof(REQINF);
	pReqInfo->fResetTally = 0;
	pReqInfo->usUserPort = 0;

	pInfo = (PDGINFO) PifNetworkSystemInfoBuffer;

	// get system network statistics filling in PIF_NETSYSTALLY struct from SYSTLY struct
	PifNetControl(0, PIF_NET_DIAG, pData, pInfo, usSizeOfInfo);

	pSystemInfo = (PRSPSYS)pInfo->auchData;
	pNetTally->ulTotalSent = pSystemInfo->infTally.ulTotalSent;
	pNetTally->ulSentError = pSystemInfo->infTally.ulSentError;
	pNetTally->usLastSentError = pSystemInfo->infTally.usLastSentError;
	pNetTally->ulTotalRecv = pSystemInfo->infTally.ulTotalRecv;
	pNetTally->ulRecvError = pSystemInfo->infTally.ulRecvError;
	pNetTally->usLastRecvError = pSystemInfo->infTally.usLastRecvError;
	pNetTally->ulOutOfResources = pSystemInfo->infTally.ulOutOfResources;
	pNetTally->ulIllegalMessage = pSystemInfo->infTally.ulIllegalMessage;
	pNetTally->ulRecvUndeliver = pSystemInfo->infTally.ulRecvUndeliver;
	pNetTally->ulRecvLostTimeOut = pSystemInfo->infTally.ulRecvLostTimeOut;
	pNetTally->ulRecvLostQueueFull = pSystemInfo->infTally.ulRecvLostQueueFull;
	pNetTally->ulRecvLostNetClosed = pSystemInfo->infTally.ulRecvLostNetClosed;
	pNetTally->ulAckSentStatusBusy = pSystemInfo->infTally.ulAckSentStatusBusy;
	pNetTally->ulSendUnreachableError = pSystemInfo->infTally.ulSendUnreachableError;

	// get user specific network statistics filling in PIF_NETUSERTALLY array of structs from USRTLY array
	pUserInfo = pSystemInfo->ainfUsers;
	pData->usFunc = DIAG_GET_USER;
	pData->usBytes = sizeof(REQINF);
	pUserTally = (PRSPUSR)pInfo->auchData;

	//--------------------------------------------------------------
	// WARNING:  Adding additional counters to USRTLY and PIF_NETUSERTALLY may end up
	//           requiring a change in the size of the buffer PifNetworkSystemInfoBuffer.
	//           When adding three new counters, I suddenly was seeing problems with kitchen printer
	//           and other unusual behaviors.  This was only seen with a Release build.
	//           Richard Chambers, Jun-15-2011
	if (pNetUsers)
	{
		for ( ; nUsers > 0; nUsers--, iUsers++)
		{
			pReqInfo->fResetTally = 0;
			pNetUsers[iUsers].usUserPort = pReqInfo->usUserPort = pUserInfo[iUsers].usPortNo;
			PifNetControl(0, PIF_NET_DIAG, pData, pInfo, usSizeOfInfo);
			pNetUsers[iUsers].usTotalSent = pUserTally->infTally.usTotalSent;
			pNetUsers[iUsers].usSentError = pUserTally->infTally.usSentError;
			pNetUsers[iUsers].usTotalRecv = pUserTally->infTally.usTotalRecv;
			pNetUsers[iUsers].usRecvError = pUserTally->infTally.usRecvError;
			pNetUsers[iUsers].usRecvLost = pUserTally->infTally.usRecvLost;
			pNetUsers[iUsers].usPendingHighWater = pUserTally->infTally.usPendingHighWater;
			pNetUsers[iUsers].usRecvLostTimeOut = pUserTally->infTally.usRecvLostTimeOut;
			pNetUsers[iUsers].usRecvLostQueueFull = pUserTally->infTally.usRecvLostQueueFull;
			pNetUsers[iUsers].usRecvRequestTimeOut = pUserTally->infTally.usRecvRequestTimeOut;
			pNetUsers[iUsers].usSendRequestTimeOut = pUserTally->infTally.usSendRequestTimeOut;
			pNetUsers[iUsers].usSendTargetBusy = pUserTally->infTally.usSendTargetBusy;
			pNetUsers[iUsers].usRecvTargetClear = pUserTally->infTally.usRecvTargetClear;
			pNetUsers[iUsers].usSendTargetClear = pUserTally->infTally.usSendTargetClear;
			pNetUsers[iUsers].usRecvPutOnQueue = pUserTally->infTally.usRecvPutOnQueue;
			pNetUsers[iUsers].usRecvGivenNow = pUserTally->infTally.usRecvGivenNow;
			pNetUsers[iUsers].usRecvBadSeqNoPort = pUserTally->infTally.usRecvBadSeqNoPort;
			pNetUsers[iUsers].usNoOfPending = pUserTally->usNoOfPending;
			pNetUsers[iUsers].usRecvTakenOffQueue = pUserTally->infTally.usRecvTakenOffQueue;
			pNetUsers[iUsers].usRecvOutOfResources = pUserTally->infTally.usRecvOutOfResources;
			pNetUsers[iUsers].usAckSentStatusBusy = pUserTally->infTally.usAckSentStatusBusy;
			pNetUsers[iUsers].usRecvLostNetClosed = pUserTally->infTally.usRecvLostNetClosed;
			pNetUsers[iUsers].usRecvRequestWaitStart = pUserTally->infTally.usRecvRequestWaitStart;
			pNetUsers[iUsers].usRecvMessageAlreadyExist = pUserTally->infTally.usRecvMessageAlreadyExist;
			pNetUsers[iUsers].usSendMessageAlreadyExist = pUserTally->infTally.usSendMessageAlreadyExist;
			pNetUsers[iUsers].usExeNetSendCount = pUserTally->infTally.usExeNetSendCount;
			pNetUsers[iUsers].usExeNetReceiveCount = pUserTally->infTally.usExeNetReceiveCount;
			pNetUsers[iUsers].usTimerCheckSkipped = pUserTally->infTally.usTimerCheckSkipped;
			pNetUsers[iUsers].usTimerSendWithRecvPending = pUserTally->infTally.usTimerSendWithRecvPending;
		}
		(*pusUsers) = iUsers;
	}

	return 0;
}

SHORT _NetControlArg(USHORT sNetHandle, USHORT sFunc, ...)
{
	LONG *pulArg;

	// Get the address of the rest of the argument list
	// containing the various parameters for the PifNetControl().
	pulArg = (ULONG *) &sFunc;
	return _NetControl (sNetHandle, sFunc, pulArg);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetAddJoin(TCHAR *pwszHostName,            **
**                                  UCHAR *pauchHostIpAddress,      **
**									UCHAR *pauchTerminalNo)         **
**                                                                  **
**  return:  PIF_OK                       Host added successfully   **
**           PIF_ERROR_NET_ALREADY_EXIST  Terminal no. in use       **
**           PIF_ERROR_NET_MANY_USERS     No empty slots in table   **
**                                                                  **
**  Description:start up routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetAddJoin (PIF_CLUSTER_HOST  *pHostInfo, UCHAR *pauchTerminalNo)
{
	SHORT  sRetStatus = PIF_OK;
    USHORT i;
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;

	if (*pauchTerminalNo) {
		// caller requests particular terminal number.  We will allocate it if
		// the slot is free.  If not, then return an error.
		i = *pauchTerminalNo - 1;
		if (i < PIF_NET_MAX_IP) {
			if ((pPifHostInfo[i].auchSlotStatus == PIF_CLUSTER_HOST_NIU) ||
				(memcmp (pPifHostInfo[i].auchHostIpAddress, pHostInfo->auchHostIpAddress, 4) == 0) ) {
				// The slot is free so lets allocate it to this terminal
				// or this terminal is already using this slot
				pPifHostInfo[i] = *pHostInfo;
				pPifHostInfo[i].auchSlotStatus = PIF_CLUSTER_HOST_NAMED;
			}
		}
		else {
			sRetStatus = PIF_ERROR_NET_ALREADY_EXIST;
		}
	}
	else {
		// first of all, lets look to see if this IP address is already in the table
		for (i = 0; i < PIF_NET_MAX_IP; i++) {
			if (memcmp (pPifHostInfo[i].auchHostIpAddress, pHostInfo->auchHostIpAddress, 4) == 0) {
				*pauchTerminalNo = i + 1;
				return PIF_OK;
			}
		}

		// Now lets search our host list to see if there are any slots available.
		// We will start our search with terminal 3 since terminal 1 and
		// terminal 2 are or may be special terminals (Master Terminal and
		// possibly Backup Master Terminal).  The basic assumption is that
		// the named terminal is a disconnected Satellite terminal that
		// is wanting to join the cluster.
		sRetStatus = PIF_ERROR_NET_MANY_USERS;
		for (i = 2; i < sizeof(SysConfig.PifNetHostInformation)/sizeof(SysConfig.PifNetHostInformation[0]); i++) {
			if (pPifHostInfo[i].auchSlotStatus == PIF_CLUSTER_HOST_NIU) {
				// The slot is free so lets allocate it to this terminal
				pPifHostInfo[i] = *pHostInfo;
				pPifHostInfo[i].auchSlotStatus = PIF_CLUSTER_HOST_NAMED;
				*pauchTerminalNo = i + 1;
				sRetStatus = PIF_OK;
				break;
			}
		}
	}
	return sRetStatus;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetRemoveJoin(TCHAR *pwszHostName,            **
**                                  UCHAR *pauchHostIpAddress,      **
**									UCHAR *pauchTerminalNo)         **
**                                                                  **
**  return:  PIF_OK                       Host added successfully   **
**           PIF_ERROR_NET_ALREADY_EXIST  Terminal no. in use       **
**           PIF_ERROR_NET_MANY_USERS     No empty slots in table   **
**                                                                  **
**  Description:start up routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetRemoveJoin (TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo)
{
	SHORT  sRetStatus = PIF_ERROR_NET_NOT_PROVIDED;
    USHORT i;
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;

	if (*pauchTerminalNo) {
		i = *pauchTerminalNo - 1;
		if (i < PIF_NET_MAX_IP && pPifHostInfo[i].auchSlotStatus != PIF_CLUSTER_HOST_NIU) {
			memset (pPifHostInfo[i].wszHostName, 0, sizeof(pPifHostInfo[i].wszHostName));
			memset (pPifHostInfo[i].auchHostIpAddress, 0, 4);
			pPifHostInfo[i].auchSlotStatus = PIF_CLUSTER_HOST_NIU;
			sRetStatus = PIF_OK;
		}
	}
	else {
		// lets look to see if this IP address is in the table
		for (i = 0; i < PIF_NET_MAX_IP; i++) {
			if (memcmp (pPifHostInfo[i].auchHostIpAddress, pauchHostIpAddress, 4) == 0) {
				memset (pPifHostInfo[i].wszHostName, 0, sizeof(pPifHostInfo[i].wszHostName));
				memset (pPifHostInfo[i].auchHostIpAddress, 0, 4);
				pPifHostInfo[i].auchSlotStatus = PIF_CLUSTER_HOST_NIU;
				return PIF_OK;
			}
		}
	}
	return sRetStatus;
}

SHORT  PIFENTRY PifNetUpdateMasterBackup (TCHAR *pwszMasName, UCHAR *pauchMasIpAddress, TCHAR *pwszBMasName, UCHAR *pauchBMasIpAddress)
{
	SHORT  sRetStatus = PIF_OK;
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;

	if (pwszMasName && *pwszMasName) {
		_tcscpy (pPifHostInfo[0].wszHostName, pwszMasName);
		memcpy (pPifHostInfo[0].auchHostIpAddress, pauchMasIpAddress, 4);
		pPifHostInfo[0].auchSlotStatus = PIF_CLUSTER_HOST_NAMED;
	}

	if (pwszBMasName && *pwszBMasName) {
		_tcscpy (pPifHostInfo[1].wszHostName, pwszBMasName);
		memcpy (pPifHostInfo[1].auchHostIpAddress, pauchBMasIpAddress, 4);
		pPifHostInfo[1].auchSlotStatus = PIF_CLUSTER_HOST_NAMED;
	}
	else {
		memset (pPifHostInfo + 1, 0, sizeof(pPifHostInfo[1]));
		pPifHostInfo[1].auchSlotStatus = PIF_CLUSTER_HOST_NIU;
	}
	return sRetStatus;
}

/* --- Network --- */
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetStartupDiscoverNet(VOID)                           **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description: Process the various information sources for the    **
**               IP addresses of the Master Terminal (Terminal #1)  **
**               and the Backup Master Terminal (Terminal #2) if a  **
**               Backup Master System is provisioned.               **
**                                                                  **
**               This function is normally called at GenPOS startup **
**               however if at GenPOS startup, the Master Terminal  **
**               host name (PC name) lookup does not succeed then   **
**               this function will be called multiple times as     **
**               the Notice Board functionality attempts to find    **
**               the IP address of the Master Terminal.             **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PifNetStartupDiscoverNet (VOID)
{
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;
    USHORT             i;
	USHORT             usLastTerminalNumber = 0;
    UCHAR              auchHostAddress[PIF_LEN_IP];
	TCHAR              atchLastJoinMasterName[SCF_USER_BUFFER] = {0};

    EnterCriticalSection(&g_NetCriticalSection);

	// we will not clear the host name table and rebuild it since this function may be called
	// multiple times such as cases where the Master Terminal was not found during GenPOS
	// startup and this is a Satellite Terminal.  the caller of this function must decide
	// whether to clear this data or not.
	//   memset (SysConfig.PifNetHostInformation, 0, sizeof(SysConfig.PifNetHostInformation));

	{
		DWORD   dwCount;
		DWORD   dwRet, i;
		TCHAR   atchDeviceName[SCF_USER_BUFFER] = {0};
		TCHAR   atchDllName[SCF_USER_BUFFER] = {0};

		SysConfig.usTerminalPosition = 0;

		dwRet = ScfGetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, &dwCount, atchDeviceName, atchDllName);
		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			TCHAR   *pDeviceName = atchDeviceName;
			TCHAR   *pDllName = atchDllName;

			for (i=0; i < dwCount; i++) {
				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_ISDISCONNECTED) != NULL) {
					if (_ttoi (pDllName) > 0) {
						SysConfig.usTerminalPosition |= PIF_CLUSTER_DISCONNECTED_SAT;
					}
				}

				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_JOINSTATUS) != NULL) {
					if (_ttoi (pDllName) > 0) {
						SysConfig.usTerminalPosition |= PIF_CLUSTER_JOINED_SAT;
					}
				}

				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_LASTJOINMASTER) != NULL) {
					wcscpy (atchLastJoinMasterName, pDllName);
				}

				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_LASTJOINDATE) != NULL) {
				}

				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_LASTUNJOINDATE) != NULL) {
				}

				if (wcsstr(pDeviceName, SCF_DEVICENAME_JOIN_LASTTERMINALNO) != NULL) {
					usLastTerminalNumber = _ttoi (pDllName);
					NHPOS_ASSERT_TEXT(0,"WARNING:  SCF_DEVICENAME_JOIN_LASTTERMINALNO is set in the Windows Registery.  Should it be?");
				}

				pDeviceName += wcslen(pDeviceName) + 1;
				pDllName += wcslen(pDllName) + 1;
			}
		}

		dwRet = ScfGetActiveDevice(SCF_TYPENAME_OVERRIDENAME_SAT, &dwCount, atchDeviceName, atchDllName);
		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			if (wcsstr(atchDeviceName, SCF_DEVICENAME_OVERRIDE_HOSTNAME) != NULL) {
				PifOverrideHostName (atchDllName);
			}
		}
	}

	//#if _DEBUG
		/* set as master terminal for debug */
		if (SysConfig.auchLaddr[3] == 0) {
			SysConfig.auchLaddr[0] = 192;
			SysConfig.auchLaddr[1] = 0;
			SysConfig.auchLaddr[2] = 0;
			SysConfig.auchLaddr[3] = 1;
		}
	//#endif

	for (i=0; i < PIF_NET_MAX_IP; i++) {
		pPifHostInfo[i].auchHostIpAddress[0] = INADDR_ANY;
	}

	// Used for debugging and designer testing only.  If designer creates a
	// registry entry of SCF_DEVICENAME_JOIN_LASTTERMINALNO and puts a terminal
	// number in the string value, then this code will allow that terminal number
	// to override the terminal number specified in the PC name.  This is used
	// for debugging ONLY!!!
	// See also function PifGetHostAddress() below where the terminal number is pulled
	// from the global data area containing the terminal number overriding the PC name.
	wszLastTerminalHostNumber[0] = 0;
	if (usLastTerminalNumber > 0) {
		wszLastTerminalHostNumber[0] = _T('-');
		_itot(usLastTerminalNumber, wszLastTerminalHostNumber + 1, 10);
	}

	PifGetHostAddress(&SysConfig.auchLaddr[0], &SysConfig.auchHaddr[0], 0);
	SysConfig.usTerminalPositionFromName = SysConfig.auchLaddr[3];

	if ((SysConfig.usTerminalPosition & PIF_CLUSTER_DISCONNECTED_SAT) || (SysConfig.auchLaddr[3] > PIF_NET_MAX_IP) ) {
		if ((SysConfig.usTerminalPosition & PIF_CLUSTER_DISCONNECTED_SAT) == 0){
			TCHAR   *pDeviceName;
			TCHAR   *pDllName;
			DWORD   dwRet;
			TCHAR   atchDeviceName[SCF_USER_BUFFER] = {0};
			TCHAR   atchDllName[SCF_USER_BUFFER] = {0};

			pDeviceName = atchDeviceName;
			pDllName    = atchDllName;

			_tcscpy (pDeviceName, SCF_DEVICENAME_JOIN_ISDISCONNECTED);
			_tcscpy (pDllName, SCF_DLLNAME_DISCONNECTED_YES);
			dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

			_tcscpy (pDeviceName, SCF_DEVICENAME_JOIN_JOINSTATUS);
			_tcscpy (pDllName, SCF_DLLNAME_JOINSTATUS_UNJOINED);
			dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);
		}

		SysConfig.usTerminalPosition &= ~0x001f;
		SysConfig.usTerminalPosition |= PIF_CLUSTER_DISCONNECTED_SAT;
		wcscpy (SysConfig.PifNetHostInformation[0].wszHostName, atchLastJoinMasterName);
		SysConfig.PifNetHostInformation[0].auchSlotStatus = PIF_CLUSTER_HOST_NAMED;
		if ((SysConfig.usTerminalPosition & PIF_CLUSTER_JOINED_SAT) == 0) {
			// If we are a Disconnected Satellite and we are Unjoined, then set ourselves
			// up as if we are a Master Terminal.
			memcpy (pPifHostInfo[0].auchHostIpAddress, &SysConfig.auchHaddr[0], 4);
			memset (pPifHostInfo[0].wszHostName, 0, sizeof(pPifHostInfo[0].wszHostName));
			SysConfig.PifNetHostInformation[0].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
			SysConfig.auchLaddr[3] = 1;
		}
		else {
			auchHostAddress[0] = 192;
			auchHostAddress[1] = 0;
			auchHostAddress[2] = 0;
			auchHostAddress[3] = 1;
			if (0 == PifGetHostAddrByName (atchLastJoinMasterName, pPifHostInfo[0].auchHostIpAddress)) {
				DWORD  dwError;

				PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
				dwError = WSAGetLastError();
				PifLog (MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_MAS_DISCOVERY);
			}
			else {
				pPifHostInfo[0].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
			}
		}
		SysConfig.usTerminalPosition &= ~0x001f;
		SysConfig.usTerminalPosition |= SysConfig.auchLaddr[3];
	}
	else {
//		SHORT  sHostFileHandle = PifOpenFile (_T("_HOSTNAME"), "or");
		SHORT  sHostFileHandle = -1;    // Hardcoded to skip the host file check

		if (sHostFileHandle >= 0) {
			// There is a host name file containing the name of the Master and Backup terminals.
			//
			ULONG  i, j, k;
			ULONG  ulBytesRead;
			char   xBuff[128];

			PifReadFile (sHostFileHandle, xBuff, sizeof(xBuff), &ulBytesRead);

			k = 0;
			for (i = 0, j = 0; i < ulBytesRead && k < 2; i++) {
				if (xBuff[i] == ',') {
					k++;
					j = 0;
					continue;
				}
				pPifHostInfo[k].wszHostName[j] = xBuff[i];
				j++;
			}
			PifCloseFile (sHostFileHandle);
			if (pPifHostInfo[0].wszHostName[0]) {
				pPifHostInfo[0].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
				if (0 == PifGetHostAddrByName (pPifHostInfo[0].wszHostName, pPifHostInfo[0].auchHostIpAddress)) {
					PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
					PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETOPEN), 1);
					NHPOS_ASSERT_TEXT(0, "error IP address lookup 1.");
				}
				if (memcmp(SysConfig.auchHaddr, pPifHostInfo[0].auchHostIpAddress, 4) == 0) {
					SysConfig.auchLaddr[3] = 1;
					NHPOS_NONASSERT_TEXT("host set as Master Terminal");
				}
			}
			if (pPifHostInfo[1].wszHostName[0]) {
				pPifHostInfo[1].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
				if (0 == PifGetHostAddrByName (pPifHostInfo[1].wszHostName, pPifHostInfo[1].auchHostIpAddress)) {
					PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
					PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETOPEN), 2);
					NHPOS_ASSERT_TEXT(0, "error IP address lookup 2.");
				}
				if (memcmp(SysConfig.auchHaddr, pPifHostInfo[1].auchHostIpAddress, 4) == 0) {
					SysConfig.auchLaddr[3] = 2;
					NHPOS_NONASSERT_TEXT("host set as Backup Master Terminal");
				}
			}
			SysConfig.usTerminalPositionFromName = SysConfig.auchLaddr[3];
		} else {
			// We are not a Disconnected Satellite terminal so we will just come
			// up as per normal operations that we have been doing for years.

			pPifHostInfo[0].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;

			/*
				First of all lets discover the Master and Backup Master IP addresses,
				if those terminals are in the cluster and powered up.

				The Master terminal is terminal number 1 and the Backup Master terminal
				is terminal number 2.  If the PifGetDestHostAddress() function returns
				an error then lets issue a PifLog indicating a problem with discovering
				those terminals.

				The PifGetDestHostAddress() function may fail if the target terminal is
				not turned on or has not been turned on for some time.  The PC Name in
				the directory used for host name lookup may go stale and be discarded
				if the terminal has been turned off for more than a few minutes.  It
				also takes a minute or two before the directory is updated when the
				terminal is turned on so there can be a delay between turning on a terminal
				and the host name lookup succeeding for that terminal.
			 */
			auchHostAddress[0] = 192;
			auchHostAddress[1] = 0;
			auchHostAddress[2] = 0;
			auchHostAddress[3] = 1;
			if (PifGetDestHostAddress(pPifHostInfo[0].auchHostIpAddress, auchHostAddress) != PIF_OK) {
				PifLog (MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_MAS_DISCOVERY);
			}
			else {
				pPifHostInfo[0].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
			}

			auchHostAddress[3] = 2;
			if (PifGetDestHostAddress(pPifHostInfo[1].auchHostIpAddress, auchHostAddress) != PIF_OK) {
				PifLog (MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_BMAS_DISCOVERY);
			}
			else {
				pPifHostInfo[1].auchSlotStatus = PIF_CLUSTER_HOST_STANDRD;
			}
		}
	}

	/*
		Now lets determine if this cluster may have a Backup Master terminal
		or not.  If our local IP address indicates we are terminal number 2
		then lets indicate that terminal 2 was found.  If we are some other
		terminal then if the auchAllHostAddress array indicates that we
		discovered terminal number 2 then we indicate that terminal 2 was
		found.

		This information is used in PifMain () to set the global variable usBMOption
	 */
	if (SysConfig.auchLaddr[3] == 2)
	{
		/* we are terminal 2 */
		SysConfig.ausOption[0] = 0x01; /* indicate that terminal 2 found */
	} else {
		/* we are some other terminal.  Check to see if terminal 2 found */
		if (pPifHostInfo[1].auchHostIpAddress[0] != INADDR_ANY)
		{
			SysConfig.ausOption[0] = 0x01; /*  indicate that terminal 2 found  */
		}
	}

	// We assume the terminal position (terminal number within the cluster)
	// is the same as the last octet of the IP address.
	// Check that the terminal position is valid and default to 1 if not.
	// Remember there is a difference between the local address which is more
	// of a virtual address contained in SysConfig.auchLaddr and the real host
	// IP address which is contained in SysConfig.auchHaddr
	SysConfig.usTerminalPosition &= ~0x001f;
	SysConfig.usTerminalPosition |= SysConfig.auchLaddr[3];

	// Now lets search our host list to see if there are any named hosts.
	// Named hosts are terminals whose name is different from the standard
	// naming convention.
	for (i = 0; i < sizeof(SysConfig.PifNetHostInformation)/sizeof(SysConfig.PifNetHostInformation[0]); i++) {
		if (pPifHostInfo[i].auchSlotStatus == PIF_CLUSTER_HOST_NAMED) {
			if (0 == PifGetHostAddrByName (pPifHostInfo[i].wszHostName, pPifHostInfo[i].auchHostIpAddress)) {
				PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
				pPifHostInfo[i].auchSlotStatus = PIF_CLUSTER_HOST_NIU;
				memset (pPifHostInfo[i].auchHostIpAddress, 0, PIF_LEN_IP);
			}
		}
	}

	{
		ULONG  ulHostIpAddressArray[3] = {0};
		// prepare for updating the filter so that we will see Notice Board broadcasts as these
		// are filtered based on the Master Terminal and Backup Master Terminal IP address.
		// see PIF_NET_RESTRICTED or PIF_PORT_FLAG_CLUSTER for filtering of IP traffic.
		memcpy (ulHostIpAddressArray + 0, SysConfig.PifNetHostInformation[0].auchHostIpAddress, 4);
		memcpy (ulHostIpAddressArray + 1, SysConfig.PifNetHostInformation[1].auchHostIpAddress, 4);
		// See also use of _NetSetClusterInfo() in function PifNetInitialize()
		_NetSetClusterInfo (ulHostIpAddressArray);    // This is call to NetSetClusterInfo()
		if (ulHostIpAddressArray[0]) {
			char   xBuff[128];

			sprintf(xBuff, "==NOTE: PifNetStartupDiscoverNet():");
			NHPOS_NONASSERT_TEXT(xBuff);
			sprintf(xBuff, "        Terminal 1 0x%x", ulHostIpAddressArray[0]);
			NHPOS_NONASSERT_TEXT(xBuff);
			sprintf(xBuff, "        Terminal 2 0x%x", ulHostIpAddressArray[1]);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
	}

    LeaveCriticalSection(&g_NetCriticalSection);

	return PIF_OK;
}

SHORT PifNetStartup(VOID)
{
    WSADATA    wsaData;

#if 0
    if (PifCheckInitialReset() == TRUE) {
        PifInitHostsAddress();
    }
#endif

	// initialize the environment whether WSAStartup() works or not
	// so that things can work without having networking.

    InitializeCriticalSection(&g_NetCriticalSection);

    fsNetStarted = 0;

    PifSubHandleTableInit(aPifNetTable, PIF_MAX_NET);

	// clear the host name table and rebuild it since this is net startup.
	memset (SysConfig.PifNetHostInformation, 0, sizeof(SysConfig.PifNetHostInformation));

    if (WSAStartup(0x0202,&wsaData) == SOCKET_ERROR) {
		DWORD    dwError = WSAGetLastError();
        WSACleanup();
        PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)dwError);
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        return PIF_ERROR_NET_ERRORS;
    }

	fsNetStarted = 1;

	PifNetStartupDiscoverNet ();
	return PIF_OK;
}

SHORT  PIFENTRY PifNetDiscoverNet (VOID)
{
	return PifNetStartupDiscoverNet ();
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetOpen(CONST XGHEADER  *pHeader) **
**              pHeader:    IP Address and Port No. Information     **
**                                                                  **
**  return:     handle of network                                   **
**                                                                  **
**  Description:Opening UDP/IP by Winsock                           **
**              Same behavior as using PIF_NET_GENERALPORTMODE.     **
**              See PifNetOpenEx(pHeader, PIF_NET_GENERALPORTMODE)  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetOpen(CONST XGHEADER *pHeader)
{
    SOCKET  hHandle = INVALID_SOCKET;
    SHORT   sNet;
    SHORT   sNetHandle = -1;
    HANDLE  hMutex = INVALID_HANDLE_VALUE;
#ifdef DEBUG_PIF_OUTPUT
	TCHAR  wszDisplay[128];
#endif

    EnterCriticalSection(&g_NetCriticalSection);

	if (fsNetStarted == 0) {
		LeaveCriticalSection(&g_NetCriticalSection);
		return PIF_ERROR_NET_ERRORS;
	}

    if (fsNetStarted == 1) {
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i before PifNetInitialize Thread: %x\n"),
		__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
        PifNetInitialize();
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i after PifNetInitialize Thread: %x\n"),
		__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
        fsNetStarted = 2;
    }

    /* search free ID */
    sNet = PifSubGetNewIdReserve(aPifNetTable, PIF_MAX_NET);
    LeaveCriticalSection(&g_NetCriticalSection);
    if (sNet==PIF_ERROR_HANDLE_OVERFLOW) {
//        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usLport);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usFport);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sNet));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        return PIF_ERROR_NET_ERRORS;
    }

    /* open by pifnet.dll */
    sNetHandle = PifNetOpenG((XGHEADER *)pHeader);
    if (sNetHandle < 0) {
//        LeaveCriticalSection(&g_NetCriticalSection);
		PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
		PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usLport);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usFport);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sNetHandle));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        return PIF_ERROR_NET_ERRORS;
    }

    /* create semaphore for file read/write */

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == INVALID_HANDLE_VALUE) {
        _NetClose(sNetHandle);
//        LeaveCriticalSection(&g_NetCriticalSection);
		PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
		PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usLport);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usFport);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), sNetHandle);
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFOPENFILE, FAULT_SYSTEM);
        return PIF_ERROR_SYSTEM;
    }

    /* set Pif Handle Table */

    aPifNetTable[sNet].sPifHandle = sNet;        // set the PIF handle overwriting the reserved value.
    aPifNetTable[sNet].hWin32Socket = hHandle;
    aPifNetTable[sNet].fCompulsoryReset = TRUE;
    aPifNetTable[sNet].chMode = PIF_NET_NMODE;
    aPifNetTable[sNet].sCount = 0;
    aPifNetTable[sNet].xgHeader = *pHeader;
    aPifNetTable[sNet].sNetHandle = sNetHandle;
    aPifNetTable[sNet].hMutexHandle = hMutex;
    aPifNetTable[sNet].fsMode = PIF_NET_GENERALPORTMODE;

    return sNet;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetOpen(CONST XGHEADER FAR *pHeader **
**              pHeader:    IP Address and Port No. Information     **
**                                                                  **
**  return:     handle of network                                   **
**                                                                  **
**  Description:Opening UDP/IP by Winsock                           **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetOpenEx(CONST XGHEADER FAR *pHeader, USHORT fsMode)
{
    SOCKET  hHandle = 0;
    SHORT   sNet;//, sReturn;
    SHORT   sNetHandle = -1;
    HANDLE  hMutex = INVALID_HANDLE_VALUE;
#ifdef DEBUG_PIF_OUTPUT
	TCHAR  wszDisplay[128];
#endif
    EnterCriticalSection(&g_NetCriticalSection);

	if (fsNetStarted == 0) {
		LeaveCriticalSection(&g_NetCriticalSection);
		return PIF_ERROR_NET_ERRORS;
	}

    if (fsNetStarted == 1) {
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i PifNetOpenEx before PifNetInitialize  Thread: %x\n"),
		__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
        PifNetInitialize();
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i PifNetOpenEx after PifNetInitialize  Thread: %x\n"),
		__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
        fsNetStarted = 2;
    }

    /* search free ID */
    sNet = PifSubGetNewIdReserve(aPifNetTable, PIF_MAX_NET);
    LeaveCriticalSection(&g_NetCriticalSection);
    if (sNet==PIF_ERROR_HANDLE_OVERFLOW) {
//        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
		PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        return PIF_ERROR_NET_ERRORS;
    }

    if (fsMode & PIF_NET_DIRECTPORTMODE) {
        hHandle = PifNetOpenD((XGHEADER *)pHeader);
        if (hHandle == INVALID_SOCKET) {
//            LeaveCriticalSection(&g_NetCriticalSection);
			PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
            /* PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE); */
            return PIF_ERROR_NET_ERRORS;
        }
	} else if (fsMode & PIF_NET_TCPCONNECTMODE) {
        hHandle = PifNetOpenTcp((XGHEADER *)pHeader);
        if (hHandle == INVALID_SOCKET) {
			// TCP connect request may fail due to device not being connected or not turned on
			// or to some server which may or may not be operational. Should the TCP connect
			// request fail, it is considered an error that can be ignored and should not
			// be a critical resource required for proper functioning of GenPOS.
			//
			// TCP connections are primarily about LAN kitchen printer
//            LeaveCriticalSection(&g_NetCriticalSection);
			PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
			PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
			PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
			return PIF_ERROR_NET_ERRORS;
        }
    } else if (fsMode & PIF_NET_GENERALPORTMODE) {
        sNetHandle = PifNetOpenG((XGHEADER *)pHeader);
        if (sNetHandle < 0) {
//            LeaveCriticalSection(&g_NetCriticalSection);
			PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
			PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
 			PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sNetHandle));
 			PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
			PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
            return PIF_ERROR_NET_ERRORS;
        }
	} else {
//        LeaveCriticalSection(&g_NetCriticalSection);
		PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
		PifLog(FAULT_AT_PIFNETOPEN, FAULT_INVALID_MODE);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sNetHandle));
		PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
		PifAbort(FAULT_AT_PIFNETOPEN, FAULT_INVALID_MODE);
        return PIF_ERROR_NET_ERRORS;
	}

    /* create semaphore for file read/write */
    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == INVALID_HANDLE_VALUE) {
		DWORD  dwLastError = GetLastError();
//        LeaveCriticalSection(&g_NetCriticalSection);
		PifSubGetNewIdReserveClear(sNet, aPifNetTable, PIF_MAX_NET);
		PifLog(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
 		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)dwLastError);
		PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        return PIF_ERROR_SYSTEM;
    }

    /* set Pif Handle Table */

    aPifNetTable[sNet].sPifHandle = sNet;        // set the PIF handle overwriting the reserved value.
    aPifNetTable[sNet].hWin32Socket = hHandle;
    aPifNetTable[sNet].fCompulsoryReset = TRUE;
    aPifNetTable[sNet].chMode = PIF_NET_NMODE;
    aPifNetTable[sNet].sCount = 0;
    aPifNetTable[sNet].xgHeader = *pHeader;
    aPifNetTable[sNet].sNetHandle = sNetHandle;
    aPifNetTable[sNet].hMutexHandle = hMutex;
    aPifNetTable[sNet].fsMode = fsMode;

    return sNet;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetSend(USHORT usNet,             **
**                                      CONST VOID FAR *pBuffer,    **
**                                      USHORT usBytes)             **
**              usNet:    network handle                            **
**              pBuffer:  sending data buffer                       **
**              usBytes:  size of pBuffer                           **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_NET_ERRORS                                **
**                                                                  **
**  Description:Sending data through UDP/IP                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetSend(USHORT usNet, CONST VOID *pBuffer, ULONG ulBytes)
{
    HANDLE hMutex;
    SHORT sReturn = PIF_ERROR_SYSTEM, sStatus = 0;

    EnterCriticalSection(&g_NetCriticalSection);

    /* during suspend process */
    if (fsPifDuringPowerDown == TRUE) {
        LeaveCriticalSection(&g_NetCriticalSection);
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    sStatus = PifSubSearchId(usNet, aPifNetTable, PIF_MAX_NET);
    if (sStatus != usNet) {
        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(FAULT_AT_PIFNETSEND, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETSEND), usNet);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETSEND), (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFNETSEND, FAULT_INVALID_HANDLE);
        return PIF_ERROR_NET_ERRORS;
    }
    hMutex = aPifNetTable[usNet].hMutexHandle;

    LeaveCriticalSection(&g_NetCriticalSection);

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    WaitForSingleObject(hMutex, INFINITE);

    /* internal communication */
    /* send by pifnet.dll */
    if (aPifNetTable[usNet].fsMode & PIF_NET_GENERALPORTMODE) {
        sReturn = PifNetSendG(usNet, pBuffer, ulBytes);
    } else if (aPifNetTable[usNet].fsMode & PIF_NET_DIRECTPORTMODE) {
        /* external communication */
        sReturn = PifNetSendD(usNet, pBuffer, ulBytes);
    } else if (aPifNetTable[usNet].fsMode & PIF_NET_TCPCONNECTMODE) {
        /* external communication */
        sReturn = PifNetSendTcp(usNet, pBuffer, ulBytes);
	} else {
        PifLog(FAULT_AT_PIFNETSEND, FAULT_INVALID_MODE);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETSEND), usNet);
		PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETSEND), (USHORT)__LINE__);
        sReturn = PIF_ERROR_NET_ERRORS;
	}

    ReleaseMutex(hMutex);

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }
    return sReturn;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetReceive(USHORT usNet,             **
**                                      CONST VOID FAR *pBuffer,    **
**                                      USHORT usBytes)             **
**              usNet:    network handle                            **
**              pBuffer:  receiving data buffer                     **
**              usBytes:  size of pBuffer                           **
**                                                                  **
**  return:     length of data actually received                    **
**                                                                  **
**  Description:receiving data through UDP/IP                       **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifNetReceive(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes)
{
    SHORT sReturn, sStatus = 0;

    EnterCriticalSection(&g_NetCriticalSection);

    /* during suspend process */
    if (fsPifDuringPowerDown == TRUE) {
        LeaveCriticalSection(&g_NetCriticalSection);
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    sStatus = PifSubSearchId(usNet, aPifNetTable, PIF_MAX_NET);
    if (sStatus != usNet) {
        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(FAULT_AT_PIFNETRECEIVE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETRECEIVE), usNet);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETRECEIVE), (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFNETRECEIVE, FAULT_INVALID_HANDLE);
        return PIF_ERROR_NET_ERRORS;
    }

    LeaveCriticalSection(&g_NetCriticalSection);

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    if (aPifNetTable[usNet].fsMode & PIF_NET_GENERALPORTMODE) {
        sReturn = PifNetReceiveG(usNet, pBuffer, usBytes);
    } else  if (aPifNetTable[usNet].fsMode & PIF_NET_DIRECTPORTMODE) {
        sReturn = PifNetReceiveD(usNet, pBuffer, usBytes);
    } else if (aPifNetTable[usNet].fsMode & PIF_NET_TCPCONNECTMODE) {
        /* external communication */
        sReturn = PifNetReceiveTcp(usNet, pBuffer, usBytes);
	} else {
        PifLog(FAULT_AT_PIFNETRECEIVE, FAULT_INVALID_MODE);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETRECEIVE), usNet);
		PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETRECEIVE), (USHORT)__LINE__);
        sReturn = PIF_ERROR_NET_ERRORS;
    }

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetControl(USHORT usNet,          **
**                                      USHORT usFunc,...)          **
**              usNet:    network handle                            **
**              usFunc:   function code of PifNetControl            **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_NET_ERRORS                                **
**                                                                  **
**  Description:set misc. control of UDP/IP                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PifNetControlInternal (USHORT usNet, USHORT usFunc, ULONG  *pulArg)
{
    SHORT sReturn = PIF_ERROR_SYSTEM, sStatus = 0;

    EnterCriticalSection(&g_NetCriticalSection);

    /* during suspend process */
    if (fsPifDuringPowerDown == TRUE) {
        LeaveCriticalSection(&g_NetCriticalSection);
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    sStatus = PifSubSearchId(usNet, aPifNetTable, PIF_MAX_NET);
    if (sStatus != usNet) {
        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(MODULE_PIF_NETCONTROL, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETCONTROL), usFunc);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETCONTROL), usNet);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFNETCONTROL, FAULT_INVALID_HANDLE);
        return PIF_ERROR_NET_ERRORS;
    }

    LeaveCriticalSection(&g_NetCriticalSection);

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }

    if (aPifNetTable[usNet].fsMode & PIF_NET_GENERALPORTMODE) {
        sReturn = PifNetControlG(usNet, usFunc, pulArg);
    } else  if (aPifNetTable[usNet].fsMode & PIF_NET_DIRECTPORTMODE) {
        sReturn = PifNetControlD(usNet, usFunc, pulArg);
    } else if (aPifNetTable[usNet].fsMode & PIF_NET_TCPCONNECTMODE) {
        /* external communication */
        sReturn = PifNetControlTcp(usNet, usFunc, pulArg);
	} else {
        PifLog(MODULE_PIF_NETCONTROL, FAULT_INVALID_MODE);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), usNet);
		PifLog(MODULE_LINE_NO(MODULE_PIF_NETCONTROL), (USHORT)__LINE__);
        sReturn = PIF_ERROR_NET_ERRORS;
    }

    if (PifNetCheckPowerDown(usNet) == TRUE) {
        return PIF_ERROR_NET_POWER_FAILURE;
    }
    return sReturn;

}

SHORT  PIFENTRY PifNetControl(USHORT usNet, USHORT usFunc, ...)
{
    ULONG  *pulArg = (ULONG*)(&usFunc);

	return PifNetControlInternal (usNet, usFunc, pulArg);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetClose(USHORT usNet)            **
**              usNet:    network handle                            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:closing network                                     **
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifNetClose(USHORT usNet)
{
    SOCKET hHandle;
    DWORD  dwError;
    HANDLE hMutex;
	SHORT  sStatus = 0;

    EnterCriticalSection(&g_NetCriticalSection);

    sStatus = PifSubSearchId(usNet, aPifNetTable, PIF_MAX_NET);
    if (sStatus != usNet) {
        LeaveCriticalSection(&g_NetCriticalSection);
        PifLog(MODULE_PIF_NETCLOSE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETCLOSE), usNet);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCLOSE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(MODULE_PIF_NETCLOSE), (USHORT)__LINE__);
        return;
    }
    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    WaitForSingleObject(hMutex, INFINITE);

    if (aPifNetTable[usNet].fCompulsoryReset == TRUE) {
        if (aPifNetTable[usNet].fsMode & PIF_NET_DIRECTPORTMODE) {
            if (hHandle != INVALID_SOCKET) {
                if (closesocket(hHandle) == SOCKET_ERROR) {
                    dwError = WSAGetLastError();
                    PifLog(MODULE_PIF_NETCLOSE, LOG_ERROR_PIFNET_CODE_01);
                    PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCLOSE), (USHORT)dwError);
                }
            }
        } else {
            if (aPifNetTable[usNet].sNetHandle >= 0) { /* not use pifnet.dll */
                _NetClose(aPifNetTable[usNet].sNetHandle);
            }
        }
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    aPifNetTable[usNet].sPifHandle = -1;
    aPifNetTable[usNet].hWin32Socket = INVALID_SOCKET;
    aPifNetTable[usNet].sNetHandle = -1;
    aPifNetTable[usNet].hMutexHandle = INVALID_HANDLE_VALUE;

    LeaveCriticalSection(&g_NetCriticalSection);
    return;

}


/* sub routins for PIF_NET_GENERALPORTMODE */

static SHORT  PifNetOpenG(CONST XGHEADER FAR *pHeader)
{
    SHORT sNetHandle, sReturn;

    /* open by pifnet.dll */
    sNetHandle = _NetOpen((XGHEADER *)pHeader);
    if (sNetHandle < 0) {
        PifLog(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sNetHandle));
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usLport);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usFport);
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SHORT_RESOURCE);
        return PIF_ERROR_NET_ERRORS;
    }

    /* not wait at NetSend */
    sReturn = _NetControlArg(sNetHandle, PIF_NET_SET_STIME, PIF_NO_WAIT);
    if (sReturn < 0) {
        _NetClose(sNetHandle);
        PifLog(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFNETOPEN), (USHORT)abs(sReturn));
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usLport);
		PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFNETOPEN), pHeader->usFport);
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFNETOPEN), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        return PIF_ERROR_SYSTEM;
    }

#if 0
	// the following logs can create a lot of logs in a short amount of time
	// in the case of a Satellite going offline because it is turned off
	// or other case where a target IP device such as network kitchen printer
	// is offline. these should probably be moved to a more appropriate place
	// in the source code.
	//    Richard Chambers, Aug-31-2018
    PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIF_NETWORKPORTCHANGE);
    PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETOPEN), sNetHandle);
    PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETOPEN), pHeader->usLport);
    PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETOPEN), pHeader->usFport);
#endif

    return sNetHandle;
}

static SHORT  PifNetSendG(USHORT usNet, CONST VOID *pBuffer, ULONG ulBytes)
{
	BOOL     bMyAddress;
    XGRAMEX * const pxgram = pBuffer;
    UCHAR i;
    SHORT sReturn;
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;

    /* use PIFNET driver */
    bMyAddress = (pxgram->auchFaddr[0] == SysConfig.auchLaddr[0] && pxgram->auchFaddr[1] == SysConfig.auchLaddr[1] &&
						pxgram->auchFaddr[2] == SysConfig.auchLaddr[2] && pxgram->auchFaddr[3] == SysConfig.auchLaddr[3]);
    if (aPifNetTable[usNet].chMode & PIF_NET_NMODE) {
        if ((pxgram->auchFaddr[0] == 192) &&
            (pxgram->auchFaddr[1] == 0) &&
            (pxgram->auchFaddr[2] == 0) &&
            (pxgram->auchFaddr[3] <= PIF_NET_MAX_IP)) {

            if (pxgram->auchFaddr[3] == 0) {
                /* set broadcast address */
                *(LONG*)aPifNetXgram[usNet].auchFaddr = INADDR_BROADCAST;
            } else if (bMyAddress) {
                /* own address */
                /* localhost */
                aPifNetXgram[usNet].auchFaddr[0] = 127;
                aPifNetXgram[usNet].auchFaddr[1] = 0;
                aPifNetXgram[usNet].auchFaddr[2] = 0;
                aPifNetXgram[usNet].auchFaddr[3] = 1;
            } else if (pxgram->auchFaddr[2] == 0) {
                i = pxgram->auchFaddr[3];
                i--;
                if (i < PIF_NET_MAX_IP) {
					// Do target terminal IP address translation
					if (pPifHostInfo[i].auchHostIpAddress[0] == INADDR_ANY) {
						PifGetDestHostAddress(pPifHostInfo[i].auchHostIpAddress, pxgram->auchFaddr);
						if (pPifHostInfo[i].auchHostIpAddress[0] == INADDR_ANY) {
							/* target does not exists */
							return PIF_OK;
						}
					}
					memcpy(aPifNetXgram[usNet].auchFaddr, pPifHostInfo[i].auchHostIpAddress, PIF_LEN_IP);
				} else if (i > PIF_NET_MAX_IP && i < 200) {
					// This is a Disconnected Satellite
					NHPOS_ASSERT(0);
				} else {
					/* use directed ip address, Probably a broadcast address */
					memcpy(aPifNetXgram[usNet].auchFaddr, pxgram->auchFaddr, PIF_LEN_IP);
				}
            }
			else {
				/* use directed ip address, Probably a broadcast address */
				memcpy(aPifNetXgram[usNet].auchFaddr, pxgram->auchFaddr, PIF_LEN_IP);
			}
        } else if (bMyAddress) {
            /* own address */
            /* localhost */
            aPifNetXgram[usNet].auchFaddr[0] = 127;
            aPifNetXgram[usNet].auchFaddr[1] = 0;
            aPifNetXgram[usNet].auchFaddr[2] = 0;
            aPifNetXgram[usNet].auchFaddr[3] = 1;
        } else {
            /* use directed ip address, assume as PC */
            memcpy(aPifNetXgram[usNet].auchFaddr, pxgram->auchFaddr, PIF_LEN_IP);
        }
        /* set source no. at the top of data */
        aPifNetXgram[usNet].usFport = pxgram->usFport;
        aPifNetXgram[usNet].usLport = pxgram->usLport;
        aPifNetXgram[usNet].auchData[0] = SysConfig.auchLaddr[3];
        memcpy(&aPifNetXgram[usNet].auchData[1], pxgram->auchData, ulBytes-sizeof(XGHEADER));
        ulBytes++;
    } else {
        /* data only mode */
        if (aPifNetTable[usNet].xgHeader.auchFaddr[3] == 0) {
            /* set broadcast address */
            *(LONG*)aPifNetXgram[usNet].auchFaddr = INADDR_BROADCAST;
        } else {
            i = aPifNetTable[usNet].xgHeader.auchFaddr[3];
            i--;
            if (i < PIF_NET_MAX_IP) {
				if (pPifHostInfo[i].auchHostIpAddress[0] == INADDR_ANY) {
					PifGetDestHostAddress(pPifHostInfo[i].auchHostIpAddress, aPifNetTable[usNet].xgHeader.auchFaddr);
					if (pPifHostInfo[i].auchHostIpAddress[0] == INADDR_ANY) {
						/* target does not exists */
						return PIF_ERROR_NET_DISCOVERY;
					}
				}
				memcpy(aPifNetXgram[usNet].auchFaddr, pPifHostInfo[i].auchHostIpAddress, PIF_LEN_IP);
			}
			else {
			}
        }
        /* set source no. at the top of data */
        aPifNetXgram[usNet].usFport = aPifNetTable[usNet].xgHeader.usFport;
        aPifNetXgram[usNet].usLport = aPifNetTable[usNet].xgHeader.usLport;
        aPifNetXgram[usNet].auchData[0] = SysConfig.auchLaddr[3];
        memcpy(&aPifNetXgram[usNet].auchData[1], pxgram->auchData, ulBytes-sizeof(XGHEADER));
        ulBytes++;
        ulBytes += sizeof(XGHEADER);
    }

    /* send by pifnet.dll */
    sReturn = _NetSend(aPifNetTable[usNet].sNetHandle, &aPifNetXgram[usNet], ulBytes);

    return sReturn;
}

static SHORT  PifNetReceiveG(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes)
{
    SOCKET  hHandle;
    XGRAMEX *pxgram;
    SHORT sRet;
    HANDLE hMutex;
    UCHAR  i;
	PIF_CLUSTER_HOST  *pPifHostInfo = SysConfig.PifNetHostInformation;

    usBytes++;   /* add 1 byte for terminal no */

    /* receive by pifnet.dll */
    sRet = _NetReceive(aPifNetTable[usNet].sNetHandle, &aPifNetRcvXgram[usNet], usBytes);

    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    if (sRet > 0) {

        WaitForSingleObject(hMutex, INFINITE);

        if (aPifNetTable[usNet].chMode & PIF_NET_DMODE) {
            sRet -= sizeof(XGHEADER);
            sRet--;
            memcpy(pBuffer, &aPifNetRcvXgram[usNet].auchData[1], sRet);
        } else {
            pxgram = (XGRAMEX *)pBuffer;
            /* get terminal source no. from top of the data.  If out of range just return Sender's IP address. */
            if (aPifNetRcvXgram[usNet].auchData[0] != 0 && aPifNetRcvXgram[usNet].auchData[0] <= PIF_NET_MAX_IP) {
                pxgram->auchFaddr[0] = 192;
                pxgram->auchFaddr[1] = 0;
                pxgram->auchFaddr[2] = 0;
                pxgram->auchFaddr[3] = aPifNetRcvXgram[usNet].auchData[0];

                i = aPifNetRcvXgram[usNet].auchData[0];
                i--;
                if (i < PIF_NET_MAX_IP) {
					ULONG  ulHostIpAddressBefore, ulHostIpAddressAfter;
					BOOL   bUpdateClusterInfo = FALSE;

					// if this is a valid terminal then lets update the cached IP address.
					memcpy (&ulHostIpAddressBefore, SysConfig.PifNetHostInformation[i].auchHostIpAddress, 4);
                    memcpy(SysConfig.PifNetHostInformation[i].auchHostIpAddress, aPifNetRcvXgram[usNet].auchFaddr, PIF_LEN_IP);
					memcpy (&ulHostIpAddressAfter, SysConfig.PifNetHostInformation[i].auchHostIpAddress, 4);
					// now lets check to see if the IP address changed.  If it did then we will log the fact.
					// plus if the address that changed was the Master Terminal then modify the filter as well.
					if (ulHostIpAddressBefore != ulHostIpAddressAfter) {
						char xBuff[128];
						if (i == 0) {
							ULONG  ulHostIpAddressArray[3] = {0};
							// prepare for updating the filter so that we will see Notice Board broadcasts as these
							// are filtered based on the Master Terminal and Backup Master Terminal IP address.
							// see PIF_NET_RESTRICTED or PIF_PORT_FLAG_CLUSTER for filtering of IP traffic.
							memcpy (ulHostIpAddressArray + 0, SysConfig.PifNetHostInformation[0].auchHostIpAddress, 4);
							memcpy (ulHostIpAddressArray + 1, SysConfig.PifNetHostInformation[1].auchHostIpAddress, 4);
							_NetSetClusterInfo (ulHostIpAddressArray);    // This is call to NetSetClusterInfo()
						}

						PifLog(MODULE_PIF_NETRECEIVE, LOG_ERROR_PIFNET_TERM_IP_CHANGE);
						PifLog(MODULE_DATA_VALUE(MODULE_PIF_NETRECEIVE), (USHORT)(i+1));
						sprintf(xBuff, "==STATE: PifNetReceiveG():  IP change Terminal %d 0x%x", i+1, ulHostIpAddressAfter);
						NHPOS_NONASSERT_NOTE("==STATE", xBuff);
					}
                }
            } else {
                memcpy(pxgram->auchFaddr, aPifNetRcvXgram[usNet].auchFaddr, PIF_LEN_IP);
            }

            pxgram->usFport = aPifNetRcvXgram[usNet].usFport;
            pxgram->usLport = aPifNetRcvXgram[usNet].usLport;
            sRet--;
            memcpy(pxgram->auchData, &aPifNetRcvXgram[usNet].auchData[1], sRet-sizeof(XGHEADER));
        }

        ReleaseMutex(hMutex);
    }
	else {
		if (sRet == PIF_ERROR_NET_TIMEOUT) {
            i = aPifNetRcvXgram[usNet].auchData[0];
            i--;
            if (i < PIF_NET_MAX_IP && pPifHostInfo[i].auchHostIpAddress[0] == INADDR_ANY) {
				sRet = PIF_ERROR_NET_DISCOVERY;
            } else if (i >= PIF_NET_MAX_IP) {
				sRet = PIF_ERROR_NET_DISCOVERY;
			}
		}
	}

    return sRet;
}

static SHORT  PifNetControlG(USHORT usNet, USHORT usFunc, ULONG *pulArg, ...)
{
    ULONG  ulArg;

    if (usFunc == PIF_NET_RESET_COMPULSORY)
	{
        aPifNetTable[usNet].fCompulsoryReset = FALSE;
        return PIF_OK;
    }

    /* pifnet.dll */
    if (usFunc == PIF_NET_SET_MODE)
	{
        aPifNetTable[usNet].chMode = (UCHAR)*pulArg;
        ulArg = PIF_NET_NMODE;  /* not use Data only mode in pifnet.dll */
        ulArg &= (*pulArg & PIF_NET_TMODE);
        return (_NetControl(aPifNetTable[usNet].sNetHandle, usFunc, pulArg));
    }
	else
	{
        return (_NetControl(aPifNetTable[usNet].sNetHandle, usFunc, pulArg));
    }
}


/* subroutins for PIF_NET_DIRECTPORTMODE */

static SOCKET  PifNetOpenD(CONST XGHEADER FAR *pHeader)
{
    SOCKET  hHandle;
    BOOL    fBlocking;
    DWORD   dwError;
    BOOL    fOpt = TRUE;

    /*
     * Port MUST be in Network Byte Order
     */
    hHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP); // UDP socket

    if (hHandle == INVALID_SOCKET){
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_02);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        return INVALID_SOCKET;
    }
    /*
       bind() associates a local address and port combination with the
       socket just created. This is most useful when the application is a
       server that has a well-known port that clients know about in advance.
    */

    if (pHeader->usLport) {
		struct sockaddr_in addr = { 0 };

		addr.sin_family = AF_INET;
        addr.sin_port = htons(pHeader->usLport);

        /* set bind by any address */
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
        if (bind(hHandle,(struct sockaddr*)&addr,sizeof(addr) ) == SOCKET_ERROR) {
            dwError = WSAGetLastError();
            closesocket(hHandle);
            if (dwError != WSAEADDRINUSE) {
                PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_03);
                PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
                PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
            }
            return INVALID_SOCKET;
        }
    }

    if (pHeader->auchFaddr[3] == 0) {
        /* set broadcast as enable */
        if (setsockopt(hHandle, SOL_SOCKET, SO_BROADCAST, (CHAR FAR *)&fOpt, sizeof(fOpt)) == SOCKET_ERROR) {
            dwError = WSAGetLastError();
            PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_04);
            PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
            closesocket(hHandle);
            PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
            return INVALID_SOCKET;
        }
    }
 
	// Using getsockopt() we found the default Windows 7 socket buffer is 8192 bytes or 8K so we are increasing
	// the size of the send and receive network buffers in WinSock to a minimum of 16K.
	{
		const int RcvBufSize = 2048 * 16;    // network buffer size in bytes per terminal times number of terminals, setsockopt()
		int    error = 0;
		int    iOpt = 1, iOptLen = sizeof(int);

		iOptLen = sizeof(int);
		error = getsockopt (hHandle, SOL_SOCKET, SO_RCVBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < RcvBufSize) {
			iOpt = RcvBufSize;
			error = setsockopt(hHandle, SOL_SOCKET, SO_RCVBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
		iOptLen = sizeof(int);
		error = getsockopt (hHandle, SOL_SOCKET, SO_SNDBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < RcvBufSize) {
			iOpt = RcvBufSize;
			error = setsockopt(hHandle, SOL_SOCKET, SO_SNDBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
	}

    //fBlocking = FALSE;// SR 760 we turn on nonblocking, so that we dont 
						//wait for our UDP sends to return, we just continue on JHHJ
    fBlocking = TRUE;  // turn on nonblocking mode
    if (ioctlsocket(hHandle, FIONBIO, (ULONG*)&fBlocking) == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_05);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
        WSACleanup();
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        return INVALID_SOCKET;
    }

    return hHandle;
}

static SHORT  PifNetSendD(USHORT usNet, CONST VOID FAR *pBuffer, ULONG ulBytes)
{
    XGRAMEX *pxgram;
    CHAR  *pData;
    SOCKET  hHandle;
    struct sockaddr_in addr;
    int retval, len;
    fd_set writefds, exceptfds;
    DWORD dwError;
    SHORT sReturn, i;

    /* external communication */
    hHandle = aPifNetTable[usNet].hWin32Socket;

    if (aPifNetTable[usNet].chMode & PIF_NET_NMODE) {
        /* normal mode, set socket address from send buffer */
        pxgram = (XGRAMEX *)pBuffer;

        /* remove header fro m send buffer */
        len = ulBytes - sizeof(XGHEADER);
        pData = (CHAR*)pxgram->auchData;

        /* save destination port no. for Data Only Mode */
        aPifNetTable[usNet].xgHeader.usFport = pxgram->usFport;
    } else {
        /* data only mode */
        /* get port no. from saved area */
        pxgram = (XGRAMEX *)&aPifNetTable[usNet].xgHeader;

        len = ulBytes;
        pData = (CHAR *)pBuffer;
    }

    if (len <= 0) {
        /* not send 0 byte data, because server pc will be fault */
        return PIF_OK;
    }

    /* set socket address, port no. */

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pxgram->usFport);
    if (pxgram->auchFaddr[3] == 0) {
        /* set broadcast address */
        addr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
    } else {
        /* normal client/server address */
        memcpy(&addr.sin_addr.S_un.S_un_b, &pxgram->auchFaddr[0], PIF_LEN_IP);
    }

    /* wait for write event */

    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(hHandle, &writefds);
    retval = select(0, NULL, &writefds, &exceptfds, NULL);
    if (retval == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETSEND, LOG_ERROR_PIFNET_CODE_06);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETSEND), (USHORT)dwError);
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    if (retval==0){
        /* time out */
    }

    if (FD_ISSET(hHandle, &writefds)){
        retval = sendto(hHandle, pData, len, 0, (struct sockaddr *)&addr, sizeof(addr));

        if (retval != SOCKET_ERROR) {
            sReturn = PIF_OK;
        } else {
            dwError = WSAGetLastError();
			//if the return value is WASAEWOULDBLOCK
			//we will try to send again twice, and if it fails
			//after the 2 tries, we return an error.SR 760 JHHJ
			if(dwError == WSAEWOULDBLOCK)
			{
				PifSleep(50);
				for(i = 0; i < 2; i++)
				{
					retval = sendto(hHandle, pData, len, 0, (struct sockaddr *)&addr, sizeof(addr));
					if(retval != SOCKET_ERROR)
					{
						return PIF_OK;
					}
				}	
			}
			PifLog(MODULE_PIF_NETSEND, LOG_ERROR_PIFNET_CODE_07);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETSEND), (USHORT)dwError);
            sReturn = PIF_ERROR_NET_ERRORS;
        }
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }

    sReturn = PIF_OK;

    return sReturn;
}

static SHORT  PifNetReceiveD(USHORT usNet, VOID *pBuffer, USHORT usBytes)
{
    SOCKET  hHandle;
	TIMEVAL timeout = {0}, *ptimeout = NULL;
    XGRAMEX *pxgram;
    fd_set readfds, exceptfds;
    int retval;
    DWORD dwError;
    HANDLE hMutex;

    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(hHandle, &readfds);

    if (aPifNetTable[usNet].chMode & PIF_NET_TMODE) {
		// if there is a timer value then use it otherwise no time out specified.
        timeout.tv_sec = (ULONG)((USHORT)aPifNetTable[usNet].sCount/1000);
        timeout.tv_usec = (ULONG)((USHORT)aPifNetTable[usNet].sCount%1000);
        ptimeout = &timeout;
    }

    retval = select(0, &readfds, NULL, &exceptfds, ptimeout);
    if (retval == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETRECEIVE, LOG_ERROR_PIFNET_CODE_08);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETRECEIVE), (USHORT)dwError);
        return PIF_ERROR_NET_ERRORS;
    }
    if (retval==0){
        /* time out */
        return PIF_ERROR_NET_TIMEOUT;
    }

    if (FD_ISSET(hHandle, &readfds)){
		struct sockaddr_in addr = { 0 };
		int   len;

		WaitForSingleObject(hMutex, INFINITE);

        pxgram = &aPifNetRcvXgram[usNet];
        len = sizeof(addr);
        retval = recvfrom(hHandle, (CHAR*)&(pxgram->auchData[0]), usBytes, 0,
            (struct sockaddr *)&addr, &len);

        if (retval != SOCKET_ERROR) {
            if (aPifNetTable[usNet].chMode & PIF_NET_NMODE) {
                memcpy(&pxgram->auchFaddr[0], &addr.sin_addr.S_un.S_un_b, PIF_LEN_IP);
                pxgram->usFport = ntohs(addr.sin_port);
                pxgram->usLport = aPifNetTable[usNet].xgHeader.usLport;

                retval += sizeof(XGHEADER);
                memcpy(pBuffer, pxgram, retval);
            } else {
                memcpy(pBuffer, &(pxgram->auchData[0]), retval);
            }
        } else {
            dwError = WSAGetLastError();
			PifLog(MODULE_PIF_NETRECEIVE, LOG_ERROR_PIFNET_CODE_09);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETRECEIVE), (USHORT)dwError);
			memcpy(pBuffer, &addr.sin_addr.S_un.S_un_b, PIF_LEN_IP);
            retval = PIF_ERROR_NET_ERRORS;
        }

        ReleaseMutex(hMutex);
    } else {
        retval = PIF_ERROR_NET_ERRORS;
    }

     return (SHORT)retval;
}

static SHORT  PifNetControlD(USHORT usNet, USHORT usFunc, ULONG *pulArg, ...)
{
    SOCKET hHandle;
    XGRAMEX *pxgram;
	TIMEVAL timeout = { 0 };
    fd_set readfds, exceptfds;
    int retval, len;
    DWORD dwError;
    HANDLE hMutex;

	pulArg++;  // increment to the first of the variable arguments, past usFunc

    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    switch (usFunc) {
    case PIF_NET_SET_MODE:
        aPifNetTable[usNet].chMode = (UCHAR)*pulArg;
        return PIF_OK;
        break;
    case PIF_NET_SET_TIME:
        aPifNetTable[usNet].sCount = (USHORT)*pulArg;
        return PIF_OK;
        break;
    case PIF_NET_RESET_SEQNO:
        return PIF_OK;
        break;
    case PIF_NET_CLEAR:
        /* receive data to dummy buffer */
        WaitForSingleObject(hMutex, INFINITE);
        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_SET(hHandle, &readfds);
        timeout.tv_sec = 0L;    /* not timeout */
        timeout.tv_usec = 0L;
        retval = select(0, &readfds, NULL, &exceptfds, &timeout);
        if (retval == SOCKET_ERROR) {
            dwError = WSAGetLastError();
	        PifLog(MODULE_PIF_NETCONTROL, PIF_NET_CLEAR);
			PifLog(MODULE_PIF_NETCONTROL, LOG_ERROR_PIFNET_CODE_10);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), (USHORT)dwError);
            PifAbort(FAULT_AT_PIFNETCONTROL, FAULT_SYSTEM);
        }
        pxgram = &aPifNetXgram[usNet];

        if (FD_ISSET(hHandle, &readfds)){
			struct sockaddr_in addr = { 0 };

			len = sizeof(addr);
            retval = recvfrom(hHandle, (CHAR*)&(pxgram->auchData[0]), PIF_LEN_UDATA, 0,
                (struct sockaddr *)&addr,&len);

            if (retval == SOCKET_ERROR) {
                dwError = WSAGetLastError();
	        	PifLog(MODULE_PIF_NETCONTROL, PIF_NET_CLEAR);
				PifLog(MODULE_PIF_NETCONTROL, LOG_ERROR_PIFNET_CODE_11);
	        	PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), (USHORT)dwError);
            }
        }
        ReleaseMutex(hMutex);
        return PIF_OK;
        break;
    case PIF_NET_GET_STATUS:
        return PIF_OK;
        break;
    case PIF_NET_RESET_COMPULSORY:
        aPifNetTable[usNet].fCompulsoryReset = FALSE;
        return PIF_OK;
        break;
    default:
        return PIF_OK;
        break;
    }
    return 0;

}

/* subroutines for PIF_NET_TCPCONNECTMODE */

static SOCKET  PifNetOpenTcp(CONST XGHEADER *pHeader)
{
    SOCKET  hHandle;
    BOOL    fBlocking;
    DWORD   dwError;
    BOOL    fOpt = TRUE;

    /*
     * Port MUST be in Network Byte Order
     */
    hHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP socket
    if (hHandle == INVALID_SOCKET){
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_02);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
        return INVALID_SOCKET;
    }

	{
		struct sockaddr_in addr = {0};

        addr.sin_family = AF_INET;
		memcpy (&addr.sin_addr.S_un.S_un_b, pHeader->auchFaddr, 4);
        addr.sin_port = htons(pHeader->usFport);

        if (connect(hHandle,(struct sockaddr*)&addr,sizeof(addr) ) == SOCKET_ERROR) {
            dwError = WSAGetLastError();
            closesocket(hHandle);
            if (dwError != WSAEADDRINUSE) {
                PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_03);
                PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
            }
            return INVALID_SOCKET;
        }
    }


	// Using getsockopt() we found the default Windows 7 socket buffer is 8192 bytes or 8K so we are increasing
	// the size of the send and receive network buffers in WinSock to a minimum of 16K.
	{
		const int RcvBufSize = 2048 * 16;    // network buffer size in bytes per terminal times number of terminals, setsockopt()
		int    error = 0;
		int    iOpt = 1, iOptLen = sizeof(int);

		iOptLen = sizeof(int);
		error = getsockopt (hHandle, SOL_SOCKET, SO_RCVBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < RcvBufSize) {
			iOpt = RcvBufSize;
			error = setsockopt(hHandle, SOL_SOCKET, SO_RCVBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
		iOptLen = sizeof(int);
		error = getsockopt (hHandle, SOL_SOCKET, SO_SNDBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < RcvBufSize) {
			iOpt = RcvBufSize;
			error = setsockopt(hHandle, SOL_SOCKET, SO_SNDBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
	}

    //fBlocking = FALSE;// SR 760 we turn on nonblocking, so that we dont 
						//wait for our UDP sends to return, we just continue on JHHJ
    fBlocking = TRUE;  // turn on nonblocking mode
    if (ioctlsocket(hHandle, FIONBIO, (ULONG*)&fBlocking) == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_CODE_05);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
//        WSACleanup();
//        PifAbort(FAULT_AT_PIFNETOPEN, FAULT_SYSTEM);
//        return INVALID_SOCKET;
    }

    return hHandle;
}

static SHORT  PifNetSendTcp(USHORT usNet, CONST VOID *pBuffer, ULONG ulBytes)
{
    XGRAMEX *pxgram;
    CHAR  *pData;
    SOCKET  hHandle;
	struct sockaddr_in addr = {0};
    int retval, len;
    fd_set writefds, exceptfds;
    DWORD dwError;
    SHORT sReturn, i;

    /* external communication */
    hHandle = aPifNetTable[usNet].hWin32Socket;

    if (aPifNetTable[usNet].chMode & PIF_NET_NMODE) {
        /* normal mode, set socket address from send buffer */
        pxgram = (XGRAMEX *)pBuffer;

        /* remove header fro m send buffer */
        len = ulBytes - sizeof(XGHEADER);
        pData = (CHAR*)pxgram->auchData;

        /* save destination port no. for Data Only Mode */
        aPifNetTable[usNet].xgHeader.usFport = pxgram->usFport;
    } else {
        /* data only mode */
        /* get port no. from saved area */
        pxgram = (XGRAMEX *)&aPifNetTable[usNet].xgHeader;

        len = ulBytes;
        pData = (CHAR *)pBuffer;
    }

    if (len <= 0) {
        /* not send 0 byte data, because server pc will be fault */
        return PIF_OK;
    }

    /* set socket address, port no. */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pxgram->usFport);
    if (pxgram->auchFaddr[3] == 0) {
        /* set broadcast address */
        addr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
    } else {
        /* normal client/server address */
        memcpy(&addr.sin_addr.S_un.S_un_b, &pxgram->auchFaddr[0], PIF_LEN_IP);
    }

    /* wait for write event */

    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(hHandle, &writefds);
    retval = select(0, NULL, &writefds, &exceptfds, NULL);
    if (retval == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETSEND, LOG_ERROR_PIFNET_CODE_06);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETSEND), (USHORT)dwError);
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    if (retval==0){
        /* time out */
    }

    if (FD_ISSET(hHandle, &writefds)){
        retval = sendto(hHandle, pData, len, 0, (struct sockaddr *)&addr, sizeof(addr));

        if (retval != SOCKET_ERROR) {
            sReturn = PIF_OK;
        } else {
            dwError = WSAGetLastError();
			//if the return value is WASAEWOULDBLOCK
			//we will try to send again twice, and if it fails
			//after the 2 tries, we return an error.SR 760 JHHJ
			if(dwError == WSAEWOULDBLOCK)
			{
				PifSleep(50);
				for(i = 0; i < 2; i++)
				{
					retval = sendto(hHandle, pData, len, 0, (struct sockaddr *)&addr, sizeof(addr));
					if(retval != SOCKET_ERROR)
					{
						return PIF_OK;
					}
				}	
			}
			PifLog(MODULE_PIF_NETSEND, LOG_ERROR_PIFNET_CODE_07);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETSEND), (USHORT)dwError);
            sReturn = PIF_ERROR_NET_ERRORS;
        }
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }

    sReturn = PIF_OK;

    return sReturn;
}

static SHORT  PifNetReceiveTcp(USHORT usNet, VOID *pBuffer, USHORT usBytes)
{
    SOCKET  hHandle;
    struct sockaddr_in addr;
	TIMEVAL timeout = {0}, *ptimeout = NULL;
    XGRAMEX *pxgram;
    fd_set readfds, exceptfds;
    int retval, len;
    DWORD dwError;
    HANDLE hMutex;

    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(hHandle, &readfds);

    if (aPifNetTable[usNet].chMode & PIF_NET_TMODE) {
		// if there is a timer value then use it otherwise no time out specified.
        timeout.tv_sec = (ULONG)((USHORT)aPifNetTable[usNet].sCount/1000);
        timeout.tv_usec = (ULONG)((USHORT)aPifNetTable[usNet].sCount%1000);
        ptimeout = &timeout;
    }

    retval = select(0, &readfds, NULL, &exceptfds, ptimeout);
    if (retval == SOCKET_ERROR) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETRECEIVE, LOG_ERROR_PIFNET_CODE_08);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETRECEIVE), (USHORT)dwError);
        return PIF_ERROR_NET_ERRORS;
    }
    if (retval==0){
        /* time out */
        return PIF_ERROR_NET_TIMEOUT;
    }

    if (FD_ISSET(hHandle, &readfds)){
        WaitForSingleObject(hMutex, INFINITE);

        pxgram = &aPifNetRcvXgram[usNet];
        memset(&addr, 0, sizeof(addr));
        len = sizeof(addr);
        retval = recvfrom(hHandle, (CHAR*)&(pxgram->auchData[0]), usBytes, 0,
            (struct sockaddr *)&addr, &len);

        if (retval != SOCKET_ERROR) {
            if (aPifNetTable[usNet].chMode & PIF_NET_NMODE) {
                memcpy(&pxgram->auchFaddr[0], &addr.sin_addr.S_un.S_un_b, PIF_LEN_IP);
                pxgram->usFport = ntohs(addr.sin_port);
                pxgram->usLport = aPifNetTable[usNet].xgHeader.usLport;

                retval += sizeof(XGHEADER);
                memcpy(pBuffer, pxgram, retval);
            } else {
                memcpy(pBuffer, &(pxgram->auchData[0]), retval);
            }
        } else {
            dwError = WSAGetLastError();
			PifLog(MODULE_PIF_NETRECEIVE, LOG_ERROR_PIFNET_CODE_09);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETRECEIVE), (USHORT)dwError);
			memcpy(pBuffer, &addr.sin_addr.S_un.S_un_b, PIF_LEN_IP);
            retval = PIF_ERROR_NET_ERRORS;
        }

        ReleaseMutex(hMutex);
    } else {
        retval = PIF_ERROR_NET_ERRORS;
    }

     return (SHORT)retval;
}

static SHORT  PifNetControlTcp(USHORT usNet, USHORT usFunc, ULONG *pulArg, ...)
{
    SOCKET hHandle;
    XGRAMEX *pxgram;
    struct sockaddr_in addr;
    TIMEVAL timeout;
    fd_set readfds, exceptfds;
    int retval, len;
    DWORD dwError;
    HANDLE hMutex;

	pulArg++;  // increment to the first of the variable arguments, past usFunc

    hHandle = aPifNetTable[usNet].hWin32Socket;
    hMutex = aPifNetTable[usNet].hMutexHandle;

    switch (usFunc) {
    case PIF_NET_SET_MODE:
        aPifNetTable[usNet].chMode = (UCHAR)*pulArg;
        return PIF_OK;
        break;
    case PIF_NET_SET_TIME:
        aPifNetTable[usNet].sCount = (USHORT)*pulArg;
        return PIF_OK;
        break;
    case PIF_NET_RESET_SEQNO:
        return PIF_OK;
        break;
    case PIF_NET_CLEAR:
        /* receive data to dummy buffer */
        WaitForSingleObject(hMutex, INFINITE);
        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_SET(hHandle, &readfds);
        timeout.tv_sec = 0L;    /* not timeout */
        timeout.tv_usec = 0L;
        retval = select(0, &readfds, NULL, &exceptfds, &timeout);
        if (retval == SOCKET_ERROR) {
            dwError = WSAGetLastError();
	        PifLog(MODULE_PIF_NETCONTROL, PIF_NET_CLEAR);
			PifLog(MODULE_PIF_NETCONTROL, LOG_ERROR_PIFNET_CODE_10);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), (USHORT)dwError);
            PifAbort(FAULT_AT_PIFNETCONTROL, FAULT_SYSTEM);
        }
        pxgram = &aPifNetXgram[usNet];

        if (FD_ISSET(hHandle, &readfds)){
            memset(&addr, 0, sizeof(addr));
            len = sizeof(addr);
            retval = recvfrom(hHandle, (CHAR*)&(pxgram->auchData[0]), PIF_LEN_UDATA, 0,
                (struct sockaddr *)&addr,&len);

            if (retval == SOCKET_ERROR) {
                dwError = WSAGetLastError();
	        	PifLog(MODULE_PIF_NETCONTROL, PIF_NET_CLEAR);
				PifLog(MODULE_PIF_NETCONTROL, LOG_ERROR_PIFNET_CODE_11);
	        	PifLog(MODULE_ERROR_NO(MODULE_PIF_NETCONTROL), (USHORT)dwError);
            }
        }
        ReleaseMutex(hMutex);
        return PIF_OK;
        break;
    case PIF_NET_GET_STATUS:
        return PIF_OK;
        break;
    case PIF_NET_RESET_COMPULSORY:
        aPifNetTable[usNet].fCompulsoryReset = FALSE;
        return PIF_OK;
        break;
    default:
        return PIF_OK;
        break;
    }
    return 0;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifCloseHandle(USHORT usPowerDown)
**              usPowerDown:    0: finalize process
**                              1: power down recovery process
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:close each device handle at finalize or power down recovery **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifNetCloseHandle(USHORT usPowerDown)
{
    int i;

    EnterCriticalSection(&g_NetCriticalSection);

    for (i=0; i<PIF_MAX_NET; i++) {
        if (aPifNetTable[i].hWin32Socket != INVALID_SOCKET) {
            if (usPowerDown == TRUE) {
                /* close handle by power down recovery */
                if (aPifNetTable[i].fCompulsoryReset == TRUE) {
					shutdown(aPifNetTable[i].hWin32Socket, SD_SEND);
                    closesocket(aPifNetTable[i].hWin32Socket);
                    aPifNetTable[i].hWin32Socket = INVALID_SOCKET;
                }
            } else {
                /* unconditional closing */
				shutdown(aPifNetTable[i].hWin32Socket, SD_SEND);
                closesocket(aPifNetTable[i].hWin32Socket);
                aPifNetTable[i].hWin32Socket = INVALID_SOCKET;
            }
        }
        if (aPifNetTable[i].sNetHandle >= 0) {
            if (usPowerDown == TRUE) {
                /* close handle by power down recovery */
                if (aPifNetTable[i].fCompulsoryReset == TRUE) {
                    _NetClose(aPifNetTable[i].sNetHandle);
                    aPifNetTable[i].sNetHandle = -1;
                }

            } else {
                /* unconditional closing */
                _NetClose(aPifNetTable[i].sNetHandle);
                aPifNetTable[i].sNetHandle = -1;
            }
        }
    }

    LeaveCriticalSection(&g_NetCriticalSection);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PifCheckPowerDown(USHORT usHandle)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
static USHORT PifNetCheckPowerDown(USHORT usHandle)
{
    USHORT usReturn = FALSE;

    EnterCriticalSection(&g_NetCriticalSection);

    if (aPifNetTable[usHandle].fCompulsoryReset == TRUE) {
        if (aPifNetTable[usHandle].fsMode & PIF_NET_DIRECTPORTMODE) {
            /* winsock direct */
            if (aPifNetTable[usHandle].hWin32Socket == INVALID_SOCKET) {
                usReturn = TRUE;
            } else {
                usReturn = FALSE;
            }
        } else if (aPifNetTable[usHandle].fsMode & PIF_NET_GENERALPORTMODE) {
            /* pifnet.dll */
            if (aPifNetTable[usHandle].sNetHandle < 0) {
                usReturn = TRUE;
            } else {
                usReturn = FALSE;
            }
        } else if (aPifNetTable[usHandle].fsMode & PIF_NET_TCPCONNECTMODE) {
            /* winsock direct */
            if (aPifNetTable[usHandle].hWin32Socket == INVALID_SOCKET) {
                usReturn = TRUE;
            } else {
                usReturn = FALSE;
            }
		}
        /* during suspend process */
        if (fsPifDuringPowerDown == TRUE) {
            usReturn = TRUE;
        }
    }

    LeaveCriticalSection(&g_NetCriticalSection);

    return usReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetFinalize(VOID)                          **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:finalize routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifNetFinalize(VOID)
{
    /* pifnet.dll */
    /* NetFinalize(); */
    PifNetFree();
    DeleteCriticalSection(&g_NetCriticalSection);
    WSACleanup();
}

/* internal routines */

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetInitialize(VOID)                           **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:start up routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
static USHORT  PifNetInitialize(VOID)
{
    LONG    lRet;
    HKEY    hKey;
    DWORD   dwDisposition, dwType, dwBytes;
	SHORT   sRetNetInit = 0;
	NETINF  netinf = { 0 };
    CHAR    szData[64];
    TCHAR   wszData[32];
    TCHAR   wszKey[128];

	strcpy (szData, szPifNetPort);    // copy the default port to out temp buffer.

    // attempt to create/open the key and look to see if an alternate UDP communication port is specified.
    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, NETWORKKEY);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
        dwBytes = sizeof(wszData);
        lRet = RegQueryValueEx(hKey, PORT, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
			memset(szData, 0, sizeof(szData));	/* 09/04/01 */
            wcstombs(szData, wszData, sizeof(szData));
        }
    }

    netinf.pszLocalPort  = szData;                /* ptr. to local port no.   */
    netinf.pszRemotePort = szData;                /* ptr. to remote port no.  */

    if (PifNetLoad(TEXT("PIFNET.DLL")) == TRUE) {

        /* use default vaule in PIFNET module */
        netinf.usMaxUsers    = 0;                   /* max. no. of users        */
        netinf.usMaxQueues   = 0;                   /* max. no. of queues       */
        netinf.usMaxLength   = 0;                   /* max. no. of message bytes*/
        netinf.usWorkMemory  = 0;                   /* max. no. of working mem. */
        netinf.usIdleTime    = 0;                   /* idle time in receiving   */

        /* pifnet.dll */
        sRetNetInit = _NetInitialize(&netinf);
		NHPOS_ASSERT_TEXT((sRetNetInit == PIF_OK), "**WARNING: _NetInitialize() failed.");
    }
	else {
		NHPOS_ASSERT_TEXT (0, "==WARNING: PifNetInitialize() _NetInitialize() failed.");
		return (USHORT)PIF_ERROR_NET_ERRORS;
	}

	{
		ULONG  ulHostIpAddressArray[3];

		memset (ulHostIpAddressArray, 0, sizeof(ulHostIpAddressArray));
		memcpy (ulHostIpAddressArray + 0, SysConfig.PifNetHostInformation[0].auchHostIpAddress, 4);
		memcpy (ulHostIpAddressArray + 1, SysConfig.PifNetHostInformation[1].auchHostIpAddress, 4);
		_NetSetClusterInfo (ulHostIpAddressArray);    // This is call to NetSetClusterInfo()
		{
			char   xBuff[128];

			sprintf(xBuff, "==NOTE: PifNetInitialize(): sRetNetInit = %d  port %s", sRetNetInit, netinf.pszLocalPort);
			NHPOS_NONASSERT_TEXT(xBuff);
			sprintf(xBuff, "        Terminal 1 0x%x", ulHostIpAddressArray[0]);
			NHPOS_NONASSERT_TEXT(xBuff);
			sprintf(xBuff, "        Terminal 2 0x%x", ulHostIpAddressArray[1]);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
	}
    return PIF_OK;
}

static SHORT  PifGetClusterPrefix (TCHAR  *aszLocalName, TCHAR  *wszHostName, TCHAR *wszHostSuffix)
{
    int     len, i;
	SHORT  sRet = 0;

    len = _tcslen(aszLocalName);
    for (i = 0; i < len; i++) {
        if (aszLocalName[i] == _T('-')) {
            // found the suffix indicator now determine the terminal number
            break;
        }
    }

    if (i + 1 < len ) {
        // if there is a suffix indicator and suffix then provide the terminal number
		sRet = i;
		_tcscpy (wszHostSuffix, aszLocalName + i + 2);
		aszLocalName[i+1] = _T('\0');
		_tcscpy(wszHostName, aszLocalName);
    }
	else {
        // if there is not a suffix indicator and suffix then assume suffix of 1, Master Terminal
		_tcscpy (wszHostSuffix, _T("1"));
        _tcscpy(wszHostName, aszLocalName);
        sRet = -1;
	}
	return sRet;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifGetDestHostAddress(UCHAR *puchDestAddr,    **
**                                          UCHAR *puchSrcAddr)     **
**                                                                  **
**  return:     PIF_OK  -> host found                               **
**              PIF_ERROR_NET_UNREACHABLE -> host lookup failed     **
**              PIF_ERROR_NET_NOT_FOUND -> invalid host name        **
**                                                                  **
**  Description: get destination host address from hostname         **
**                                                                  **
**              The PifGetDestHostAddress() function may fail if    **
**              the target terminal is not turned on or has not     **
**              been turned on for some time.  The PC Name in       **
**              the directory used for host name lookup may go      **
**              stale and be discarded if the terminal has been     **
**              turned off for more than a few minutes.  It also    **
**              takes a minute or two before the directory is       **
**              updated when the terminal is turned on so there     **
**              can be a delay between turning on a terminal and    **
**              the host name lookup succeeding for that terminal.  **
**********************************************************************
fhfh*/
static SHORT PifGetDestHostAddress(UCHAR *puchDestAddr, UCHAR *puchSrcAddr)
{
    DWORD  dwError = 0;
    USHORT usSrc;
	SHORT  sRet = PIF_OK;
    TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = DEVICENAME;
	TCHAR  aszLocalName[PIF_LEN_HOST_NAME + 1] = { 0 };
    TCHAR  wszHostSuffix[4];

	WSASetLastError (0);     // clear any existing error code in Windows Sockets.
    if ((puchSrcAddr[0] == 192) &&
        (puchSrcAddr[1] == 0) &&
        (puchSrcAddr[2] == 0) &&
        (puchSrcAddr[3] <= PIF_NET_MAX_IP)) {

        /*
			Caller is requesting IP address for communication within the
			NeighborhoodPOS cluster.  Terminal number is in the last octet
			of the IP address so we create a target host name by concatenating
			the cluster hostname prefix, such as NCRPOS-, with the terminal
			number to create the target POS host name.  Then lookup the IP
			address of the hostname we have created.

			We assume that the caller has initialized the puchDestAddr array
			to a known value.  Several places in the pifnet code depend on
			checking if a destination address is known by comparing the
			destination address with INADDR_ANY.
		 */

		PifGetLocalHostName(aszLocalName);
		if (0 > PifGetClusterPrefix (aszLocalName, wszHostName, wszHostSuffix)) {
            NHPOS_ASSERT_TEXT(0, "** ERROR - host name incorrect format. no suffix found. Forcing.");
			PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIFNET_BAD_NAME);

            // host does not have a suffix so is not in the form of HOST-nn where nn is the terminal number.
            // we will assume this is supposed to be a single Master Terminal. check that the terminal
            // requested is terminal 1. It may be terminal 2 and we want to provide an error in that case.
            if (puchSrcAddr[3] != 1) {
			    sRet = PIF_ERROR_NET_NOT_FOUND;
			    puchDestAddr[0] = INADDR_ANY;
            }
            //
            // WARNING: PC name may be stale if terminal is off and this function may fail.
            if (puchSrcAddr[3] == 1 && 0 == PifGetHostAddrByName(wszHostName, puchDestAddr)) {
                dwError = WSAGetLastError();
                PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
                PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
                sRet = PIF_ERROR_NET_UNREACHABLE;
                puchDestAddr[0] = INADDR_ANY;
            }
        }
		else {
			usSrc = (USHORT)puchSrcAddr[3];
			if (usSrc/100) {
				wszHostSuffix[0] = (TCHAR)((TCHAR)(usSrc/100) | 0x30);
				wszHostSuffix[1] = (TCHAR)((TCHAR)((usSrc%100)/10) | 0x30);
				wszHostSuffix[2] = (TCHAR)((TCHAR)(usSrc%10) | 0x30);
				wszHostSuffix[3] = 0;
			} else if (usSrc/10) {
				wszHostSuffix[0] = (TCHAR)((TCHAR)(usSrc/10) | 0x30);
				wszHostSuffix[1] = (TCHAR)((TCHAR)(usSrc%10) | 0x30);
				wszHostSuffix[2] = 0;
			} else {
				wszHostSuffix[0] = (TCHAR)(usSrc | 0x30);
				wszHostSuffix[1] = 0;
			}

			_tcscat (wszHostName, wszHostSuffix);
			// WARNING: PC name may be stale if terminal is off and this function may fail.
			if (0 == PifGetHostAddrByName (wszHostName, puchDestAddr)) {
				dwError = WSAGetLastError();
				PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
				PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
				sRet = PIF_ERROR_NET_UNREACHABLE;
				puchDestAddr[0] = INADDR_ANY;
			}
		}
    } else {
        /* for PC communication such as for KDS or other non POS target */
        memcpy(puchDestAddr, puchSrcAddr, PIF_LEN_IP);
    }

	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: PifGetDestHostAddress() sRet = %d  dwError = %d dest %d.%d.%d.%d", sRet, dwError,
			puchDestAddr[0], puchDestAddr[1], puchDestAddr[2], puchDestAddr[3]);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	return sRet;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifGetHostAddress(UCHAR *puchLocalAddr,       **
**                                      UCHAR *puchHostAddr);       **
**                                                                  **
**  return:     PIF_OK  -> host found                               **
**              PIF_ERROR_NET_UNREACHABLE -> host lookup failed     **
**                                                                  **
**  Description:get my address from my hostname                     **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PifGetHostAddress(UCHAR *puchLocalAddr, UCHAR *puchHostAddr, TCHAR *wszHostName2)
{
    TCHAR  *paszHost;
    DWORD  dwError = 0;
    USHORT usSrc;
	SHORT  sRet = PIF_OK;
	TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };

    PifGetLocalHostName(wszHostName);

	if (wszHostName2) {
		_tcscpy (wszHostName2, wszHostName);
	}

	// Used for debugging only with the SCF_DEVICENAME_JOIN_LASTTERMINALNO registery
	// setting.  See function PifNetStartup() above where the terminal number
	// string is set in this shared area.
	if (wszLastTerminalHostNumber[0]) {
		_tcscpy (wszHostName, wszLastTerminalHostNumber);
	}

    /* device name "NCR2172-xx" */
    paszHost = _tcschr (wszHostName, _T('-'));

    if (paszHost != NULL) {
        paszHost++;

        usSrc = (USHORT)_ttoi (paszHost);

        puchLocalAddr[0] = 192;
        puchLocalAddr[1] = 0;
        puchLocalAddr[2] = 0;
        puchLocalAddr[3] = (UCHAR)usSrc; /* terminal no. */
    }
	else {
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIFNET_BAD_NAME);
	}

	WSASetLastError (0);     // clear any existing error code in Windows Sockets.
	if (0 == PifGetHostAddrByName (wszHostName, puchHostAddr)) {
        dwError = WSAGetLastError();
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
        memset (puchHostAddr, 0, PIF_LEN_IP);
		sRet = PIF_ERROR_NET_UNREACHABLE;
	}
	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: PifGetHostAddress() sRet = %d, dwError = %d host %d.%d.%d.%d", sRet, dwError,
			puchHostAddr[0], puchHostAddr[1], puchHostAddr[2], puchHostAddr[3]);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	return sRet;
}

#if 0
// following functions are not currently used.
// RJC
SHORT PifCheckHostsAddress(USHORT usSrc, UCHAR *puchSrcAddr)
{
    TCHAR  wszHostName[PIF_LEN_HOST_NAME] = DEVICENAME;
    TCHAR  wszNum[4];
    UCHAR  auchIPAddress[PIF_LEN_IP+1];
    /* reserved for Multi-Cluster System */
    TCHAR  wszLocalName[PIF_LEN_HOST_NAME];
    int     len, i;

    memset(wszLocalName, 0, sizeof(wszLocalName));
    PifGetLocalHostName(wszLocalName);
    len = _tcslen(wszLocalName);
    for (i=0; i < len; i++) {
        if (wszLocalName[i] == _T('-')) {
            wszLocalName[i+1] = _T('\0');
            break;
        }
    }

    if (i < len) {
        memcpy (wszHostName, wszLocalName, sizeof(wszHostName));
    }
	else {
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIFNET_BAD_NAME);
	}

    /* 2172 cluster communication */

    if (usSrc/100) {
        wszNum[0] = (TCHAR)((TCHAR)(usSrc/100) | 0x30);
        wszNum[1] = (TCHAR)((TCHAR)((usSrc%100)/10) | 0x30);
        wszNum[2] = (TCHAR)((TCHAR)(usSrc%10) | 0x30);
        wszNum[3] = 0;
    } else
    if (usSrc/10) {
        wszNum[0] = (TCHAR)((TCHAR)(usSrc/10) | 0x30);
        wszNum[1] = (TCHAR)((TCHAR)(usSrc%10) | 0x30);
        wszNum[2] = 0;
    } else {
        wszNum[0] = (TCHAR)(usSrc | 0x30);
        wszNum[1] = 0;
    }

    /* device name "NCR2172-xx" */
    _tcscat(wszHostName, wszNum);

    if (PifGetHostsAddress(wszHostName, auchIPAddress) == PIF_OK) {
        if (memcmp(auchIPAddress, puchSrcAddr, PIF_LEN_IP) == 0) {
            /* same cluster message */
            return TRUE;
        } else {
            /* another cluster message */
            return FALSE;
        }
    } else {
        return TRUE;
    }
}

static VOID PifInitHostsAddress(VOID)
{
    TCHAR  aszHostName[PIF_LEN_HOST_NAME];
    TCHAR  aszName[PIF_LEN_HOST_NAME];
    TCHAR  aszNum[4];
    USHORT i, len;

    PifGetLocalHostName(aszHostName);

    len = (USHORT)_tcslen(aszHostName);
    for (i=0; i< len; i++) {
        if (aszHostName[i] == _T('-')) {
            aszHostName[i+1] = _T('\0');
            break;
        }
    }
    if (i==len) {
        /* not set as "NCRPOS-1" format */
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIFNET_BAD_NAME);
        return;
    }

    /* delete previous host registry befor network start at initial reset */

    for (i=1; i<=PIF_NET_MAX_IP; i++) {
        if (i/100) {
            aszNum[0] = (TCHAR)((TCHAR)(i/100) | 0x30);
            aszNum[1] = (TCHAR)((TCHAR)((i%100)/10) | 0x30);
            aszNum[2] = (TCHAR)((TCHAR)(i%10) | 0x30);
            aszNum[3] = 0;
        } else
        if (i/10) {
            aszNum[0] = (TCHAR)((TCHAR)(i/10) | 0x30);
            aszNum[1] = (TCHAR)((TCHAR)(i%10) | 0x30);
            aszNum[2] = 0;
        } else {
            aszNum[0] = (TCHAR)(i | 0x30);
            aszNum[1] = 0;
        }

        /* device name "NCR2172-xx" */
        _tcsncpy(aszName, aszHostName, PIF_LEN_HOST_NAME);
        _tcscat(aszName, aszNum);

        PifRemoveHostsAddress((const TCHAR*)aszName);
    }
}
#endif

static SHORT PifNetLoad(LPCTSTR    lpszName)    // load a library similar to LoadDevice().
{
    PNETLOAD_FUNCTION pLoadFunction;
    BOOL        fCompleted = TRUE;      // assume good status
    FARPROC *   ppfnProc;
    PPROCADR    pTable;
    DWORD       dwRC;
#ifdef DEBUG_PIF_OUTPUT
	TCHAR  wszDisplay[128];
#endif
    static  PROCADR     afnDlls[] = { {  1, TEXT("NetInitialize")      },
                                      {  2, TEXT("NetOpen")            },
                                      {  3, TEXT("NetReceive")         },
                                      {  4, TEXT("NetReceive2")        },
                                      {  5, TEXT("NetSend")            },
                                      {  6, TEXT("NetControl")         },
                                      {  7, TEXT("NetClose")           },
                                      {  8, TEXT("NetFinalize")        },
                                      {  9, TEXT("NetSetClusterInfo")  },
                                      {  0, NULL               },
                                     };


    if (NULL == (PifPifNetBlk.hInstance = LoadLibrary(lpszName)))
    {
        dwRC = GetLastError();
        PifPifNetBlk.hInstance = NULL;
        return FALSE;
    }

    pLoadFunction = &PifPifNetBlk.Func;
#ifdef DEBUG_PIF_OUTPUT
	wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  PifNetLoad library loaded Thread: %x\n"),	__FILE__, __LINE__, GetCurrentThreadId ());
	OutputDebugString(wszDisplay);
#endif
    // get each procedure address

    for (pTable = afnDlls; pTable->lpName; pTable++)
    {
        // save the address
        switch (pTable->iIndex)
        {
        case 1:
            ppfnProc = (FARPROC *)&pLoadFunction->NetInitialize;
            break;
        case 2:
            ppfnProc = (FARPROC *)&pLoadFunction->NetOpen;
            break;
        case 3:
            ppfnProc = (FARPROC *)&pLoadFunction->NetReceive;
            break;
        case 4:
            ppfnProc = (FARPROC *)&pLoadFunction->NetReceive2;
            break;
        case 5:
            ppfnProc = (FARPROC *)&pLoadFunction->NetSend;
            break;
        case 6:
            ppfnProc = (FARPROC *)&pLoadFunction->NetControl;
            break;
        case 7:
            ppfnProc = (FARPROC *)&pLoadFunction->NetClose;
            break;
        case 8:
            ppfnProc = (FARPROC *)&pLoadFunction->NetFinalize;
            break;
        case 9:
            ppfnProc = (FARPROC *)&pLoadFunction->NetSetClusterInfo;
            break;
        default:
            ppfnProc = NULL;
            break;
        }
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i ppfnProc = %x NULL Thread: %x\n"),	__FILE__, __LINE__, ppfnProc, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
        // get the procedure address
        *ppfnProc  = GetProcAddressUnicode(PifPifNetBlk.hInstance, pTable->lpName);
        fCompleted = (*ppfnProc) ? fCompleted : FALSE;
    }

    if (! fCompleted)
    {
#ifdef DEBUG_PIF_OUTPUT
		wsprintf(wszDisplay, TEXT("FILE: %S  LINE: %i  PifNetLoad FreeLibrary called Thread: %x\n"),	__FILE__, __LINE__, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
		FreeLibrary(PifPifNetBlk.hInstance);
        PifPifNetBlk.hInstance = NULL;
		return FALSE;
    }

    // exit ...
    return TRUE;
}

static VOID PifNetFree(VOID)
{
    if (PifPifNetBlk.hInstance != NULL)
    {
        FreeLibrary(PifPifNetBlk.hInstance);
    }
}

static SHORT _NetInitialize(PNETINF pnetinf)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetInitialize(pnetinf);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetOpen(PXGHEADER pxgheader)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetOpen(pxgheader);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetSetClusterInfo(ULONG  *pulClusterInfo)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetSetClusterInfo(pulClusterInfo);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}


static SHORT _NetReceive(USHORT usHandle, VOID  *pBuffer, USHORT usBytes)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetReceive(
                    usHandle,
                    pBuffer,
                    usBytes);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetReceive2(USHORT usHandle, VOID  *pBuffer, USHORT usBytes, PXGADDR pxgaddr)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetReceive2(
                    usHandle,
                    pBuffer,
                    usBytes,
                    pxgaddr);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetSend(USHORT usHandle, VOID  *pBuffer, ULONG ulBytes)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetSend(
                    usHandle,
                    pBuffer,
                    ulBytes);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetControl(USHORT usHandle, USHORT usFunc, ULONG  *pulArg, ...)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetControl(
                    usHandle,
                    usFunc,
                    pulArg);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetClose(USHORT usHandle)
{

    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetClose(usHandle);
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

static SHORT _NetFinalize(VOID)
{
    SHORT sReturn;

    if (PifPifNetBlk.hInstance != NULL) {
        sReturn =   PifPifNetBlk.Func.NetFinalize();
    } else {
        sReturn = PIF_ERROR_NET_ERRORS;
    }
    return sReturn;
}

/* end of pifnet.c */
