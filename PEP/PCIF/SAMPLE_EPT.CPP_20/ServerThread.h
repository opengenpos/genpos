
#if !defined(SERVERTHREAD_H_INCLUDED)
// Guard agains multiple inclusions
#define SERVERTHREAD_H_INCLUDED

#include <ecr.h>
#include <regstrct.h>
#include <r20_pif.h>
#include <pifctl.h>
#include <paraequ.h>
#include <r20_cpm.h>
#include <r20_ept.h>

#include "DisplayTransaction.h"

#define CLI_NET_OPEN		0x0001
#define CLI_NET_SEND		0x0002
#define CLI_NET_RECEIVE		0x0004

class ServerThread {
public:
	ServerThread (USHORT usLportL = CLI_PORT_CPM_HOST, USHORT usFportL = ANYPORT);
	~ServerThread ();

	void NetOpen (USHORT usLportL = 0, USHORT usFportL = ANYPORT);
	void NetClose (void);
	SHORT NetReceiveBuf (void);
	SHORT NetSendBuf (void);
	SHORT NetSend(void *pvSndBuffer, USHORT usDataLen);
	SHORT NetReceive(void *pvRecvBuffer, USHORT ulBufferLen);
	SHORT ProcessRequest (void);
	SHORT ProcessRequestCPM (void);
	SHORT ProcessRequestEPT (void);

	void StartServerCPM (USHORT usLportL = 0, USHORT usFportL = ANYPORT);
	void StartServerEPT (USHORT usLportL = 0, USHORT usFportL = ANYPORT);
	void StopServer (void);
	int SetDialogData (void);

public:
	int ContinueComms;
	FILE        *fpLogFile;             // pointer to the log file

	typedef enum {ServerTypeCPM = 1, ServerTypeEPT } SERVER_TYPE;

	SERVER_TYPE		myThreadType;				// what kind of a server are we?

private:
    USHORT		usLport;				// Local port number (this application)
    USHORT		usFport;				// Far port number (other application)
	USHORT		usNetHandle;            // handle for PifNet functions
	USHORT		fchNetStatus;           // network status bit map
	USHORT		usNetTimer;				// timeout for receives in milliseconds

	DWORD		dwThreadId;				// Server Thread identifier
	HANDLE		hThread;				// Server Thread handle

	XGRAMEX		ServerSndBuf;           // Send message buffer
	USHORT		ServerSndBufSize;
	XGRAMEX		ServerRcvBuf;           // Receive message buffer
	USHORT		usCliTranNo;            // transaction # of my terminal
	CDisplayTransaction  cdTrans;

};

#endif