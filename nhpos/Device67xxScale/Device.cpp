/*************************************************************************
 *
 * Device.cpp
 *
 * $Workfile:: Device.cpp                                                $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000-2001.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: Device.cpp                                                 $
 *
 ************************************************************************/

#include "stdafx.h"
#include "ecr.h"
#include "DeviceIOScale.h"
#include "Device.h"
#include "Packet.h"
#include "UieScale.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////

// for System Configuration

typedef struct _ScData
{
	DWORD	dwId;
	TCHAR*	pchName;
} ScData, *PScData;

ScData s_scTable[] = {	{ 1,	_T("Port")           },
						{ 2,	SCF_DATANAME_BAUD    },
						{ 3,	SCF_DATANAME_CHARBIT },
						{ 4,	SCF_DATANAME_PARITY  },
						{ 5,	SCF_DATANAME_STOPBIT },
						{ 0,	NULL                 } };


// function control command codes

const UINT	CMD_OPEN		    =   1;		// Open() function
const UINT	CMD_CLOSE		    =   2;		// Close() function
const UINT	CMD_WRITE           =   3;		// Write() function
const UINT	CMD_READ            =   4;		// Read() function
const UINT	CMD_IOCONTROL       =   5;		// IoControl() function
const UINT	CMD_GETDEVICENAME   = 101;		// GetDeviceName() function
const UINT	CMD_GETDEVICECAPA   = 102;		// GetDeviceCapability() function
const UINT	CMD_GETPARAMETER    = 103;		// GetParameter() function
const UINT	CMD_SETPARAMETER    = 104;		// SetParameter() function
const UINT	CMD_QUIT		    = 999;		// quit the job

// function parameters

typedef struct _InfOpen
{
	LPCTSTR		lpszDeviceName;
} InfOpen, *PInfOpen;

typedef struct _InfClose
{
	HANDLE		hDevice;
} InfClose, *PInfClose;

typedef struct _InfWrite
{
	HANDLE		hDevice;
	LPVOID		lpBuffer;
	DWORD		dwNumberOfBytesToWrite;
	LPDWORD		lpNumberOfBytesWritten;
} InfWrite, *PInfWrite;

typedef struct _InfRead
{
	HANDLE		hDevice;
	LPVOID		lpBuffer;
	DWORD		dwNumberOfBytesToRead;
	LPDWORD		lpNumberOfBytesRead;
} InfRead, *PInfRead;

typedef struct _InfIoControl
{
	HANDLE		hDevice;
	DWORD		dwIoControlCode;
	LPVOID		lpInBuffer;
	DWORD		dwInBufferSize;
	LPVOID		lpOutBuffer;
	DWORD		dwOutBufferSize;
	LPDWORD		lpBytesReturned;
} InfIoControl, *PInfIoControl;

typedef struct _InfGetDeviceName
{
	LPDWORD		lpDeviceCount;
	LPTSTR		lpszDeviceName;
} InfGetDeviceName, *PInfGetDeviceName;

typedef struct _InfGetDeviceCapa
{
	LPCTSTR		lpszDeviceName;
	DWORD		dwFunc;
	DWORD		dwType;
	LPVOID		lpCapsData;
} InfGetDeviceCapa, *PInfGetDeviceCapa;

typedef struct _InfGetParameter
{
	LPCTSTR		lpszDeviceName;
	LPCTSTR		lpszDataName;
	LPDWORD		lpDataType;
	LPVOID		lpData;
	DWORD		dwNumberOfBytesToRead;
	LPDWORD		lpNumberOfBytesRead;
} InfGetParameter, *PInfGetParameter;

typedef struct _InfSetParameter
{
	LPCTSTR		lpszDeviceName;
	LPCTSTR		lpszDataName;
	DWORD		dwDataType;
	LPVOID		lpData;
	DWORD		dwNumberOfBytesToWrite;
	LPDWORD		lpNumberOfBytesWritten;
} InfSetParameter, *PInfSetParameter;

// some usefule macros

#define	ARRAYS(object, type)	(sizeof(object) / sizeof(type))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	HANDLE		CDevice::CDevice();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDevice::CDevice()
{
	// initialize

	m_bInitialized = FALSE;

	m_sName[0].LoadString(IDS_DEVICENAME1);

	m_Caps[0].dwDeviceType = SCF_TYPE_SCALE;
	m_Caps[0].dwPortType   = SCF_PORT_SERIAL;
	m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);

	m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
	m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_1200 |
	                              SCF_CAPS_BAUD_2400 |
	                              SCF_CAPS_BAUD_4800 |
	                              SCF_CAPS_BAUD_9600 |
	                              SCF_CAPS_BAUD_19200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_7;
	m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
	                              SCF_CAPS_PARITY_EVEN |
	                              SCF_CAPS_PARITY_ODD;
	m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1;
	m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

	// set default parameter

	m_Serial[0].dwBaud      = SCF_CAPS_BAUD_9600;
	m_Serial[0].dwByteSize  = SCF_CAPS_CHAR_7;
	m_Serial[0].dwParity    = SCF_CAPS_PARITY_EVEN;
	m_Serial[0].dwStopBit   = SCF_CAPS_STOP_1;
	m_Serial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

	// create event signals (auto reset & non-signaled initially)

	m_hDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	// create a device thread

	m_pThread = AfxBeginThread(DoDeviceThread, this);

	// set his priority higher a little bit

	m_pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	// do not delete automatically, I'll take care !

	m_pThread->m_bAutoDelete = FALSE;

	// wait for a done signal

	::WaitForSingleObject(m_hDone, INFINITE);
	::ResetEvent(m_hDone);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Destruction
//
// Prototype:	HANDLE		CDevice::~CDevice();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDevice::~CDevice()
{
	CPacket		pktMessage(CMD_QUIT);

	// quit our job !

	pktMessage.Post(m_pThread->m_nThreadID);// post a message
	pktMessage.WaitForReturn();				// wait for return

	// wait the thread to the end

#if	defined(_WIN32_WCE)
	DWORD		dwMilliseconds;

#if	!defined(_WIN32_WCE_EMULATION)
	dwMilliseconds = INFINITE;
#else
	// The thread terminate event does not notify correctly
	// on Windows CE 2.0 emulation environment.
	// Change the time-out interval value.
	dwMilliseconds = 100;
#endif
	::WaitForSingleObject(m_hDone, INFINITE);
	::WaitForSingleObject(m_pThread->m_hThread, dwMilliseconds);
#else	// Win32
	DWORD	dwExitCode = STILL_ACTIVE;
	BOOL	fWatch     = TRUE;
	int		nRetry     = 5;

    while ((dwExitCode == STILL_ACTIVE) && fWatch) // watch the thread
    {
        fWatch = ::GetExitCodeThread(m_pThread->m_hThread, &dwExitCode);
        ASSERT(fWatch);     // just a debug
        Sleep(100);         // pass a control to the others

		// W2K Support Modification
		if (-nRetry < 0)
		{
			break;
		}
    }
#endif

	delete m_pThread;						// Add V1.2

	::CloseHandle(m_hDone);
}

//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		CDevice::Open(lpszDeviceName);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//
// Outputs:		HANDLE		hDevice;	-
//
/////////////////////////////////////////////////////////////////////////////

HANDLE CDevice::Open(LPCTSTR lpszDeviceName)
{
	InfOpen		infPacket;
	CPacket		pktMessage(CMD_OPEN, &infPacket);

	// make up command data

	infPacket.lpszDeviceName = lpszDeviceName;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return (HANDLE)pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::Close(hDevice);
//
// Inputs:		nothing
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::Close(HANDLE hDevice)
{
	InfClose	infPacket;
	CPacket		pktMessage(CMD_CLOSE, &infPacket);

	// make up command data

	infPacket.hDevice = hDevice;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::Write(hDevice, lpBuffer,
//								dwNumberOfBytesToWrite, lpNumberOfBytesWritten);
//
// Inputs:		LPVOID		lpBuffer;	-
//				DWORD		dwNumberOfBytesToWrite;
//				LPDWORD		lpNumberOfBytesWritten;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::Write(HANDLE hDevice, LPVOID lpBuffer,
	DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
	InfWrite	infPacket;
	CPacket		pktMessage(CMD_WRITE, &infPacket);

	// make up command data

	infPacket.hDevice                = hDevice;
	infPacket.lpBuffer               = lpBuffer;
	infPacket.dwNumberOfBytesToWrite = dwNumberOfBytesToWrite;
	infPacket.lpNumberOfBytesWritten = lpNumberOfBytesWritten;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::Read(hDevice, lpBuffer,
//								dwNumberOfBytesToRead, lpNumberOfBytesRead);
//
// Inputs:		LPVOID		lpBuffer;	-
//				DWORD		dwNumberOfBytesRead;
//				LPDWORD		lpNumberOfBytesRead;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::Read(HANDLE hDevice, LPVOID lpBuffer,
	DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	InfRead	infPacket;
	CPacket		pktMessage(CMD_READ, &infPacket);

	// make up command data

	infPacket.hDevice               = hDevice;
	infPacket.lpBuffer              = lpBuffer;
	infPacket.dwNumberOfBytesToRead = dwNumberOfBytesToRead;
	infPacket.lpNumberOfBytesRead   = lpNumberOfBytesRead;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::IoControl(hDevice, dwIoControlCode,
//								lpInBuffer, dwInBufferSize,
//								lpOutBuffer, dwOutBufferSize,
//								lpBytesReturned);
//
// Inputs:		HANDLE		hDevice;		-
//				DWORD		dwIoControlCode;
//				LPVOID		lpInBuffer;	-
//				DWORD		dwInBufferSize;
//				LPDWORD		lpOutBuffer;
//				DWORD		dwOutBufferSize;
//				LPDWORD		lpBytesReturned;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::IoControl(HANDLE hDevice, DWORD dwIoControlCode,
	LPVOID lpInBuffer, DWORD dwInBufferSize,
    LPVOID lpOutBuffer, DWORD dwOutBufferSize,
    LPDWORD lpBytesReturned)
{
	InfIoControl	infPacket;
	CPacket			pktMessage(CMD_IOCONTROL, &infPacket);

	// make up command data

	infPacket.hDevice         = hDevice;
	infPacket.dwIoControlCode = dwIoControlCode;
	infPacket.lpInBuffer      = lpInBuffer;
	infPacket.dwInBufferSize  = dwInBufferSize;
	infPacket.lpOutBuffer     = lpOutBuffer;
	infPacket.dwOutBufferSize = dwOutBufferSize;
	infPacket.lpBytesReturned = lpBytesReturned;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::GetDeviceName(lpDeviceCount, lpszDeviceName);
//
// Inputs:		LPVOID		lpDeviceCount;	-
//				DWORD		lpszDeviceName;	-
//
// Outputs:		DWORD		dwResult;		-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::GetDeviceName(LPDWORD lpDeviceCount, LPTSTR lpszDeviceName)
{
	InfGetDeviceName	infPacket;
	CPacket				pktMessage(CMD_GETDEVICENAME, &infPacket);

	// make up command data

	infPacket.lpDeviceCount  = lpDeviceCount;
	infPacket.lpszDeviceName = lpszDeviceName;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::GetDeviceCapability(lpszDeviceName,
//								dwFunc, dwType, lpCapsData);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//				DWORD		dwFunc;			-
//				DWORD		dwType;			-
//				LPVOID		lpCapsData;		-
//
// Outputs:		DWORD		dwResult;		-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::GetDeviceCapability(LPCTSTR lpszDeviceName, DWORD dwFunc,
	DWORD dwType, LPVOID lpCapsData)
{
	InfGetDeviceCapa	infPacket;
	CPacket				pktMessage(CMD_GETDEVICECAPA, &infPacket);

	// make up command data

	infPacket.lpszDeviceName = lpszDeviceName;
	infPacket.dwFunc         = dwFunc;
	infPacket.dwType         = dwType;
	infPacket.lpCapsData     = lpCapsData;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::GetParameter(lpszDeviceName, lpszDataName,
//								lpDataType, lpData, dwNumberOfBytesToRead,
//								lpNumberOfBytesRead);
//
// Inputs:		LPCTSTR		lpszDeviceName;			-
//				LPCTSTR		lpszDataName;			-
//				LPDWORD		lpDataType;				-
//				LPVOID		lpData;					-
//				DWORD		dwNumberOfBytesToRead;	-
//				LPDWORD		lpNumberOfBytesRead;	-
//
// Outputs:		DWORD		dwResult;				-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::GetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
	LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead)
{
	InfGetParameter	infPacket;
	CPacket			pktMessage(CMD_GETPARAMETER, &infPacket);

	// make up command data

	infPacket.lpszDeviceName        = lpszDeviceName;
	infPacket.lpszDataName          = lpszDataName;
	infPacket.lpDataType            = lpDataType;
	infPacket.lpData                = lpData;
	infPacket.dwNumberOfBytesToRead = dwNumberOfBytesToRead;
	infPacket.lpNumberOfBytesRead   = lpNumberOfBytesRead;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::SetParameter(lpszDeviceName, lpszDataName,
//								dwDataType, lpData, dwNumberOfBytesToWrite,
//								lpNumberOfBytesWritten);
//
// Inputs:		LPCTSTR		lpszDeviceName;			-
//				LPCTSTR		lpszDataName;			-
//				DWORD		dwDataType;				-
//				LPVOID		lpData;					-
//				DWORD		dwNumberOfBytesToWrite;	-
//				LPDWORD		lpNumberOfBytesWritten;	-
//
// Outputs:		DWORD		dwResult;				-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::SetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
	DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten)
{
	InfSetParameter	infPacket;
	CPacket			pktMessage(CMD_SETPARAMETER, &infPacket);

	// make up command data

	infPacket.lpszDeviceName         = lpszDeviceName;
	infPacket.lpszDataName           = lpszDataName;
	infPacket.dwDataType             = dwDataType;
	infPacket.lpData                 = lpData;
	infPacket.dwNumberOfBytesToWrite = dwNumberOfBytesToWrite;
	infPacket.lpNumberOfBytesWritten = lpNumberOfBytesWritten;

	// initialize

	pktMessage.m_nStatus = DEVICEIO_SUCCESS;	// assume good status

	// post the packet to the device thread

	pktMessage.Post(m_pThread->m_nThreadID);
	pktMessage.WaitForReturn();

	// exit ...

	return pktMessage.m_nStatus;
}


/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Entry point of device thread. (static function)
//
// Prototype:	UINT		CDevice::DoDeviceThread(lpvData);
//
// Inputs:		LPVOID		lpvData;
//
// Outputs:		UINT		nResult;	- status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDevice::DoDeviceThread(LPVOID lpvData)
{
	return ((CDevice*)lpvData)->DeviceThread();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Input Device Engine thread body
//
// Prototype:	UINT		CDevice::DeviceThread();
//
// Inputs:		nothing
//
// Outputs:		UINT		nResult;	- status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDevice::DeviceThread()
{
	DWORD		nStatus;
	BOOL		bDoMyJob = TRUE;

#if defined(_WIN32_WCE)
	MSG			msg;

	// issue a dummy call to create a message queue for my thread

	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
#else
	// create a message queue for my thread

	::GetQueueStatus(QS_ALLEVENTS);
#endif

	// initialize a device

	OnInitialize();							// initialize a device

	// issue a signal to become ready

	::SetEvent(m_hDone);					// I'm ready to go !

	// do my job

	while (bDoMyJob)						// do my job forever
	{
		DWORD		dwEvent;						// event type
		CPacket *	pPacket;						// ptr. to a packet
		LONG		nCommand;						// command code

		// wait for an event

		dwEvent  = OnWaitForEvent(TRUE);
		nCommand = 0;

		// is user message ?

		if (dwEvent == EVENT_USER_PACKET)
		{
			// get user packet

			if (! (pPacket = CPacket::GetPacket()))
			{
                continue;
			}

			// get function code

			nCommand = pPacket->m_nCommand;
		}

		// do each job

		switch (dwEvent + nCommand)			// what function ?
		{
		case EVENT_SCALE_DATA:				// scale event ?
			nStatus = OnScaleEvent();
			break;
		case EVENT_TICK_TIMER:				// timer event ?
			break;
		case EVENT_USER_PACKET + CMD_OPEN:
			nStatus = (LONG)OnOpen(
						((PInfOpen)(pPacket->m_pvData))->lpszDeviceName);
			break;
		case EVENT_USER_PACKET + CMD_CLOSE:
			nStatus = OnClose(
						((PInfClose)(pPacket->m_pvData))->hDevice);
			break;
		case EVENT_USER_PACKET + CMD_WRITE:
			nStatus = OnWrite(
						((PInfWrite)(pPacket->m_pvData))->hDevice,
						((PInfWrite)(pPacket->m_pvData))->lpBuffer,
						((PInfWrite)(pPacket->m_pvData))->dwNumberOfBytesToWrite,
						((PInfWrite)(pPacket->m_pvData))->lpNumberOfBytesWritten);
			break;
		case EVENT_USER_PACKET + CMD_READ:
			nStatus = OnRead(
						((PInfRead)(pPacket->m_pvData))->hDevice,
						((PInfRead)(pPacket->m_pvData))->lpBuffer,
						((PInfRead)(pPacket->m_pvData))->dwNumberOfBytesToRead,
						((PInfRead)(pPacket->m_pvData))->lpNumberOfBytesRead);
			break;
		case EVENT_USER_PACKET + CMD_IOCONTROL:
			nStatus = OnIoControl(
						((PInfIoControl)(pPacket->m_pvData))->hDevice,
						((PInfIoControl)(pPacket->m_pvData))->dwIoControlCode,
						((PInfIoControl)(pPacket->m_pvData))->lpInBuffer,
						((PInfIoControl)(pPacket->m_pvData))->dwInBufferSize,
						((PInfIoControl)(pPacket->m_pvData))->lpOutBuffer,
						((PInfIoControl)(pPacket->m_pvData))->dwOutBufferSize,
						((PInfIoControl)(pPacket->m_pvData))->lpBytesReturned);
			break;
		case EVENT_USER_PACKET + CMD_GETDEVICENAME:
			nStatus = OnGetDeviceName(
						((PInfGetDeviceName)(pPacket->m_pvData))->lpDeviceCount,
						((PInfGetDeviceName)(pPacket->m_pvData))->lpszDeviceName);
			break;
		case EVENT_USER_PACKET + CMD_GETDEVICECAPA:
			nStatus = OnGetDeviceCapability(
						((PInfGetDeviceCapa)(pPacket->m_pvData))->lpszDeviceName,
						((PInfGetDeviceCapa)(pPacket->m_pvData))->dwFunc,
						((PInfGetDeviceCapa)(pPacket->m_pvData))->dwType,
						((PInfGetDeviceCapa)(pPacket->m_pvData))->lpCapsData);
			break;
		case EVENT_USER_PACKET + CMD_GETPARAMETER:
			nStatus = OnGetParameter(
						((PInfGetParameter)(pPacket->m_pvData))->lpszDeviceName,
						((PInfGetParameter)(pPacket->m_pvData))->lpszDataName,
						((PInfGetParameter)(pPacket->m_pvData))->lpDataType,
						((PInfGetParameter)(pPacket->m_pvData))->lpData,
						((PInfGetParameter)(pPacket->m_pvData))->dwNumberOfBytesToRead,
						((PInfGetParameter)(pPacket->m_pvData))->lpNumberOfBytesRead);
			break;
		case EVENT_USER_PACKET + CMD_SETPARAMETER:
			nStatus = OnSetParameter(
						((PInfSetParameter)(pPacket->m_pvData))->lpszDeviceName,
						((PInfSetParameter)(pPacket->m_pvData))->lpszDataName,
						((PInfSetParameter)(pPacket->m_pvData))->dwDataType,
						((PInfSetParameter)(pPacket->m_pvData))->lpData,
						((PInfSetParameter)(pPacket->m_pvData))->dwNumberOfBytesToWrite,
						((PInfSetParameter)(pPacket->m_pvData))->lpNumberOfBytesWritten);
			break;
		case EVENT_USER_PACKET + CMD_QUIT:
			nStatus  = 0;
			bDoMyJob = FALSE;
			break;
		default:							// unknown command
			nStatus = DEVICEIO_E_ILLEGAL;
			break;
		}

		// done user message ?

		if (dwEvent == EVENT_USER_PACKET)
		{
			// dispose the message

			pPacket->m_nStatus = nStatus;	// set status
			pPacket->Dispose();				// dispose it
		}
	}

	// clean up the device

	OnFinalize();

	//

	::SetEvent(m_hDone);

	// exit ...

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Initialize a device
//
// Prototype:	BOOL	CDevice::OnInitialize();
//
// Inputs:		nothing
//
// Outputs:		BOOL	fSuccess;		- execution status
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::OnInitialize()
{
	// just initialized

	m_bOpened  = FALSE;
	m_pScale   = NULL;

	// associate a manual reset event

	m_hScale   = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	// mark as initialized

	m_pUser        = NULL;
	m_bInitialized = TRUE;

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Finalize a device
//
// Prototype:	VOID	CDevice::OnFinalize();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID CDevice::OnFinalize()
{
	// opened ?

	if (m_bOpened)							// still opened
	{
		OnClose(m_pUser);					// pass control to OnClose()
	}

	// close the event associated

	::CloseHandle(m_hScale);

	// reset my flag

	m_bInitialized = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		CDevice::OnOpen(lpszDeviceName);
//
// Inputs:		
//
// Outputs:		HANDLE		hDevice;	-
//
/////////////////////////////////////////////////////////////////////////////

HANDLE CDevice::OnOpen(LPCTSTR lpszDeviceName)
{
	BOOL	bResult;
	int		nIndex;
	USHORT	usPort, usBaud;
	UCHAR	uchFormat;
	BOOL	bScale;

	// completed to initialize device ?
	if (! m_bInitialized) {
		return INVALID_HANDLE_VALUE;
	}

	// already opened ?
	if (m_bOpened) {
		return INVALID_HANDLE_VALUE;
	}

	// get parameters
	nIndex  = GetDeviceIndex(lpszDeviceName);
	bResult = ReadParameter(nIndex);
	if (! bResult) {
		return INVALID_HANDLE_VALUE;
	}

	// make up parameter
	usPort    = GetPort(nIndex);
	usBaud    = GetBaud(nIndex);
	uchFormat = GetSerialFormat(nIndex);
	bScale    = (BOOL)(m_Caps[nIndex].dwDeviceType & SCF_TYPE_SCALE);

	// invalid port number ?
	if (usPort == 0) {
		return INVALID_HANDLE_VALUE;
	}

	// initialize the scale thread
	bResult = UieScaleInit(usPort, usBaud, uchFormat, m_hScale);
	if (! bResult) {
		return INVALID_HANDLE_VALUE;
	}

	// mark as opened
	m_pUser   = this;
	m_bOpened = TRUE;

	return (HANDLE)m_pUser;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnClose(hDevice);
//
// Inputs:		nothing
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnClose(HANDLE hDevice)
{
	BOOL	bResult;

	// not open ?

	if (! m_bOpened)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	if (m_pUser != (CDevice*)hDevice)		// invalid handle
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// terminate the scale thread

	bResult = UieScaleTerm();

	// is available scale user ?

	if (m_pScale)
	{
		m_pScale->m_nStatus = DEVICEIO_E_FAILURE;
		m_pScale->Dispose();				// dispose it

		m_pScale = NULL;
	}

	// mark as closed

	m_pUser   = NULL;
	m_bOpened = FALSE;

	// exit ...

	return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnWrite(lpBuffer,
//								dwNumberOfBytesToWrite, lpNumberOfBytesWritten);
//
// Inputs:		LPVOID		lpBuffer;	-
//				DWORD		dwNumberOfBytesToWrite;
//				LPDWORD		lpNumberOfBytesWritten;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnWrite(HANDLE hDevice, LPVOID lpBuffer,
	DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
	// exit ...

	return DEVICEIO_E_ILLEGAL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnRead(lpBuffer,
//								dwNumberOfBytesToRead, lpNumberOfBytesRead);
//
// Inputs:		LPVOID		lpBuffer;	-
//				DWORD		dwNumberOfBytesRead;
//				LPDWORD		lpNumberOfBytesRead;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnRead(HANDLE hDevice, LPVOID lpBuffer,
	DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	// exit ...

	return DEVICEIO_E_ILLEGAL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnIoControl(dwIoControlCode,
//								lpInBuffer, dwInBufferSize,
//								lpOutBuffer, dwOutBufferSize,
//								lpBytesReturned);
//
// Inputs:		DWORD		dwIoControlCode;
//				LPVOID		lpInBuffer;	-
//				DWORD		dwInBufferSize;
//				LPDWORD		lpOutBuffer;
//				DWORD		dwOutBufferSize;
//				LPDWORD		lpBytesReturned;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnIoControl(HANDLE hDevice, DWORD dwIoControlCode,
	LPVOID lpInBuffer, DWORD dwInBufferSize,
	LPVOID lpOutBuffer, DWORD dwOutBufferSize,
	LPDWORD lpBytesReturned)
{
	DWORD	dwResult;

	switch (dwIoControlCode)
	{
	case DEVICEIO_CTL_SCALE_READWEIGHT:
		dwResult = OnIoControlReadWeight(
						hDevice,
						dwIoControlCode,
						lpInBuffer,
						dwInBufferSize,
						lpOutBuffer,
						dwOutBufferSize,
						lpBytesReturned);
		break;
	case DEVICEIO_CTL_SCALE_ENABLE:
	default:
		dwResult = DEVICEIO_E_ILLEGAL;
		break;
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnIoControlReadWeight(dwIoControlCode,
//								lpInBuffer, dwInBufferSize,
//								lpOutBuffer, dwOutBufferSize,
//								lpBytesReturned);
//
// Inputs:		DWORD		dwIoControlCode;
//				LPVOID		lpInBuffer;	-
//				DWORD		dwInBufferSize;
//				LPDWORD		lpOutBuffer;
//				DWORD		dwOutBufferSize;
//				LPDWORD		lpBytesReturned;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnIoControlReadWeight(HANDLE hDevice, DWORD dwIoControlCode,
	LPVOID lpInBuffer, DWORD dwInBufferSize,
	LPVOID lpOutBuffer, DWORD dwOutBufferSize,
	LPDWORD lpBytesReturned)
{
	PDEVICEIO_SCALE_DATA	pScale;
	UIE_SCALE				Driver;
	SHORT					sResult;

	// opened ?

	if (! m_bOpened)
	{
		return DEVICEIO_E_ILLEGAL;
	}

	if (m_pUser != (CDevice*)hDevice)		// invalid handle
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// check parameters

	if (lpOutBuffer == NULL ||
		dwOutBufferSize < sizeof(DEVICEIO_SCALE_DATA) ||
		lpBytesReturned == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// read weight, sync. operation

	sResult = UieReadScale(&Driver);

	// make up response data

	pScale          = (PDEVICEIO_SCALE_DATA)lpOutBuffer;
	pScale->sStatus = sResult;
	pScale->uchUnit = Driver.uchUnit;
	pScale->uchDec  = Driver.uchDec;
	pScale->ulData  = Driver.ulData;
	*lpBytesReturned = sizeof(DEVICEIO_SCALE_DATA);

	// exit ...

	return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::GetDeviceName(lpDeviceCount, lpszDeviceName);
//
// Inputs:		LPVOID		lpDeviceCount;	-
//				DWORD		lpszDeviceName;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnGetDeviceName(LPDWORD lpDeviceCount, LPTSTR lpszDeviceName)
{
	CString		sName;

	// check parameters

	if (lpDeviceCount == NULL || lpszDeviceName == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// make up data

	*lpDeviceCount = NUMBER_OF_DEVICE;

	sName.LoadString(IDS_DEVICENAME1);
	_tcscpy(lpszDeviceName, sName);

	// exit ...

	return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::OnGetDeviceCapability(lpszDeviceName,
//								dwFunc, dwType, lpCapsData);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//				DWORD		dwFunc;			-
//				DWORD		dwType;			-
//				LPVOID		lpCapsData;		-
//
// Outputs:		DWORD		dwResult;		-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnGetDeviceCapability(LPCTSTR lpszDeviceName, DWORD dwFunc,
	DWORD dwType, LPVOID lpCapsData)
{
	DWORD				dwResult = DEVICEIO_E_ILLEGAL;
	int					nIndex;
	PDEVICE_CAPS		pCaps;
	PDEVICE_CAPS_SERIAL	pSerial;

	// check parameters

	if (lpszDeviceName == NULL || lpCapsData == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// search device

	for (nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		if (m_sName[nIndex].CompareNoCase(lpszDeviceName) == 0)
		{
			// which function ?

			switch (dwFunc)
			{
			case SCF_CAPS_SUMMARY:
				pCaps = (PDEVICE_CAPS)lpCapsData;
				if (pCaps->dwSize == m_Caps[nIndex].dwSize)
				{
					*pCaps   = m_Caps[nIndex];
					dwResult = DEVICEIO_SUCCESS;
				}
				break;
			case SCF_CAPS_HWPORT:
				pSerial = (PDEVICE_CAPS_SERIAL)lpCapsData;
				if (pSerial->dwSize == m_CapsSerial[nIndex].dwSize)
				{
					*pSerial = m_CapsSerial[nIndex];
					dwResult = DEVICEIO_SUCCESS;
				}
				break;
			case SCF_CAPS_DEVICE:
			default:
				break;
			}

			// exit loop ...

			break;
		}
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::GetParameter(lpszDeviceName, lpszDataName,
//								lpDataType, lpData, dwNumberOfBytesToRead,
//								lpNumberOfBytesRead);
//
// Inputs:		LPCTSTR		lpszDeviceName;			-
//				LPCTSTR		lpszDataName;			-
//				LPDWORD		lpDataType;				-
//				LPVOID		lpData;					-
//				DWORD		dwNumberOfBytesToRead;	-
//				LPDWORD		lpNumberOfBytesRead;	-
//
// Outputs:		DWORD		dwResult;				-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnGetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
	LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead)
{
	DWORD	dwResult;
	BOOL	bResult;
	int		nIndex;
	TCHAR	achBuffer[SCF_MAX_DATA];

	// check parameters

	if (lpszDeviceName        == NULL ||
		lpszDataName          == NULL ||
		lpData                == NULL ||
		dwNumberOfBytesToRead == 0 ||
		lpNumberOfBytesRead   == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// valid device name ?

	nIndex = GetDeviceIndex(lpszDeviceName);

	if (nIndex < 0)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	dwResult = ScfGetParameter(
				lpszDeviceName, 
				lpszDataName, 
				lpDataType, 
				(LPVOID)achBuffer, 
				sizeof(achBuffer), 
				lpNumberOfBytesRead);
	
	if (dwResult != SCF_SUCCESS)
	{
		return DEVICEIO_E_FAILURE;			// exit ...
	}

	// if the data type of the parameter fetched from the registery
	// is a string, then we will try to use the standard conversion
	// function to convert the string to the proper binary value.
	// If the conversion fails, then we will just return the actual
	// string.  This is why bResult defaults to FALSE because if the
	// data type is something other than string, then we just copy
	// the data read from the registry directly into the callers buffer.

	bResult = FALSE;

	if (*lpDataType == SCF_DATA_STRING)
	{
		bResult = ScfConvertString2Bit(
					lpszDataName,
					(LPDWORD)lpData,
					achBuffer);
	}

	if (bResult)
	{
		*lpDataType          = SCF_DATA_BINARY;
		*lpNumberOfBytesRead = sizeof(DWORD);
	}
	else
	{
		memcpy(lpData, achBuffer, *lpNumberOfBytesRead);
	}

	// exit ...

	return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDevice::SetParameter(lpszDeviceName, lpszDataName,
//								dwDataType, lpData, dwNumberOfBytesToWrite,
//								lpNumberOfBytesWritten);
//
// Inputs:		LPCTSTR		lpszDeviceName;			-
//				LPCTSTR		lpszDataName;			-
//				DWORD		dwDataType;				- 
//				LPVOID		lpData;					-
//				DWORD		dwNumberOfBytesToWrite;	-
//				LPDWORD		lpNumberOfBytesWritten;	-
//
// Outputs:		DWORD		dwResult;				-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnSetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
	DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten)
{
	DWORD	dwResult;
	BOOL	bResult;
	int		nIndex;
	TCHAR	achBuffer[SCF_MAX_DATA];

	// check parameters

	if (lpszDeviceName         == NULL ||
		lpszDataName           == NULL ||
		lpData                 == NULL ||
		dwNumberOfBytesToWrite == 0 ||
		lpNumberOfBytesWritten == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// valid device name ?

	nIndex = GetDeviceIndex(lpszDeviceName);

	if (nIndex < 0)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// binary data ?

	if (dwDataType == SCF_DATA_BINARY)
	{
		bResult = ScfConvertBit2String(
					lpszDataName,
					achBuffer,
					*(LPDWORD)lpData);
	}
	else
	{
		bResult = FALSE;
	}

	if (bResult)
	{
		dwResult = ScfSetParameter(
					lpszDeviceName,
					lpszDataName,
					SCF_DATA_STRING,
					achBuffer,
					(_tcslen(achBuffer) + 1) * sizeof(TCHAR),
					lpNumberOfBytesWritten);
	} 
	else 
	{
		dwResult = ScfSetParameter(
					lpszDeviceName,
					lpszDataName,
					dwDataType,
					lpData,
					dwNumberOfBytesToWrite,
					lpNumberOfBytesWritten);
	}

	if (dwResult != SCF_SUCCESS)
	{
		return DEVICEIO_E_FAILURE;			// exit ...
	}

	*lpNumberOfBytesWritten = dwNumberOfBytesToWrite;

	// exit ...

	return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	New scanner data event
//
// Prototype:	DWORD	CDevice::OnScaleEvent();
//
// Inputs:		nothing
//
// Outputs:		DWORD	
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnScaleEvent()
{
	// exit ...

	return DEVICEIO_E_ILLEGAL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Wait for a device event
//
// Prototype:	DWORD	CDevice::OnWaitForEvent(fPacket);
//
// Inputs:		BOOL	fPacket;		- monitor request packet
//
// Outputs:		DWORD	dwEvent;		- event type
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::OnWaitForEvent(BOOL bPacket)
{
    DWORD		dwResult, dwMasks, dwEvent, dwCounts, dwMilliseconds;
	HANDLE		ahEvents[1];
	LPHANDLE	lphEvents;

	// make up event array to be concerned

	ahEvents[0] = m_hScale;

	// needs to wait for packet message ?

	dwMasks  = bPacket ? (QS_POSTMESSAGE | QS_SENDMESSAGE) : 0;
	dwMasks |= QS_TIMER;

	// by the way, completed to initialized the device ?

	lphEvents = m_bOpened ?                 ahEvents : NULL;
	dwCounts  = m_bOpened ? ARRAYS(ahEvents, HANDLE) :    0;

#if	defined(_WIN32_WCE)
	// The MsgWaitForMultipleObjects() function does not work correctly
	// on Windows CE 2.0 and emulation environment.
	dwMilliseconds = 500;
#else
	dwMilliseconds = INFINITE;
#endif

	// wait for an event
	
	dwResult = ::MsgWaitForMultipleObjects(dwCounts,	// wait for an event
										   lphEvents,		// ptr. to event object
										   FALSE,			// one of the events
										   dwMilliseconds,	// wait forever
										   dwMasks);		// input options

	// user request event ?
	// Caution: This should be evaluated at the beginning !

	if (dwResult == WAIT_OBJECT_0 + dwCounts)
	{
		dwEvent = EVENT_USER_PACKET;
	}

	// else, scale data event ?

	else if (dwResult == (WAIT_OBJECT_0 + 0))
	{
		dwEvent = EVENT_SCALE_DATA;
		::ResetEvent(m_hScale);
	}
	
#if	defined(_WIN32_WCE)
	// else, timeout ?

	else if (dwResult == WAIT_TIMEOUT)
	{
		dwEvent = EVENT_USER_PACKET;		// maybe user packet ...
	}
#endif

	// else, unexpected events ...

	else
	{
		dwEvent = 0;
	
		ASSERT(FALSE);
	}

	// exit ...

	return dwEvent;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	int 		CDevice::GetDeviceIndex(lpszDeviceName);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//
// Outputs:		int 		bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

int CDevice::GetDeviceIndex(LPCTSTR lpszDeviceName)
{
	int		nIndex;

	// search device name

	for (nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		if (m_sName[nIndex].CompareNoCase(lpszDeviceName) == 0)
		{
			return nIndex;					// exit ...
		}
	}

	// exit ...

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		CDevice::ReadParameter(nIndex);
//
// Inputs:		int			nIndex;		-
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::ReadParameter(int nIndex)
{
	DWORD	dwResult;
	PScData	pSc;
	DWORD	dwDataType;
	TCHAR	auchData[16];
	DWORD	dwBytesRead;

	// invalid index ?

	if (nIndex < 0)
	{
		return FALSE;						// exit ...
	}

	// get my parameters

	for (pSc = s_scTable; pSc->dwId; pSc++)
	{
		dwResult = OnGetParameter(
					m_sName[nIndex], 
					pSc->pchName, 
					&dwDataType,
					&auchData,
					sizeof(auchData),
					&dwBytesRead);
	
		if (dwResult == SCF_SUCCESS)
		{
			switch (pSc->dwId)
			{
			case 1:							// port number
				m_sPort[nIndex] = auchData;
				break;
			case 2:							// baud rate
				m_Serial[nIndex].dwBaud     = *(DWORD*)auchData;
				break;
			case 3:							// data byte
				m_Serial[nIndex].dwByteSize = *(DWORD*)auchData;
				break;
			case 4:							// parity
				m_Serial[nIndex].dwParity   = *(DWORD*)auchData;
				break;
			case 5:							// stop bit
				m_Serial[nIndex].dwStopBit  = *(DWORD*)auchData;
				break;
			default:						// unknown ...
				break;
			}
		}
	}

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	USHORT		CDevice::GetPort(nIndex)
//
// Inputs:		int			nIndex;		-
//
// Outputs:		USHORT		usNumber;	-
//
/////////////////////////////////////////////////////////////////////////////

USHORT CDevice::GetPort(int nIndex)
{
	int		nPort = 0;

	if (! m_sPort[nIndex].IsEmpty())
	{
		_stscanf(m_sPort[nIndex], _T("%d"), &nPort);
	}

	// exit ...

	return (USHORT)nPort;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	USHORT		CDevice::GetBaud(nIndex)
//
// Inputs:		int			nIndex;		-
//
// Outputs:		USHORT		usNumber;	-
//
/////////////////////////////////////////////////////////////////////////////

USHORT CDevice::GetBaud(int nIndex)
{
	ULONG	ulBaud;

	switch (m_CapsSerial[nIndex].dwBaud)
	{
	case SCF_CAPS_BAUD_300:		ulBaud = 300;	break;
	case SCF_CAPS_BAUD_600:		ulBaud = 600;	break;
	case SCF_CAPS_BAUD_1200:	ulBaud = 1200;	break;
	case SCF_CAPS_BAUD_2400:	ulBaud = 2400;	break;
	case SCF_CAPS_BAUD_4800:	ulBaud = 4800;	break;
	case SCF_CAPS_BAUD_9600:	ulBaud = 9600;	break;
	case SCF_CAPS_BAUD_19200:	ulBaud = 19200;	break;
	case SCF_CAPS_BAUD_38400:	ulBaud = 38400;	break;
	case SCF_CAPS_BAUD_57600:	ulBaud = 57600;	break;
	case SCF_CAPS_BAUD_115200:	ulBaud = 115200; break;
	default:					ulBaud = 9600;	break;
	}

	// exit ...

	return ulBaud;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	UCHAR		CDevice::GetSerialFormat(nIndex)
//
// Inputs:		int			nIndex;		-
//
// Outputs:		UCHAR		usNumber;	-
//
/////////////////////////////////////////////////////////////////////////////

UCHAR CDevice::GetSerialFormat(int nIndex)
{
	UCHAR	uchFormat = 0;

	switch (m_Serial[nIndex].dwByteSize)
	{
	case SCF_CAPS_CHAR_8:		uchFormat |= 0x03;	break;
	case SCF_CAPS_CHAR_7:		uchFormat |= 0x02;	break;
	default:										break;
	}

	switch (m_Serial[nIndex].dwParity)
	{
	case SCF_CAPS_PARITY_NONE:	uchFormat |= 0x00;	break;
	case SCF_CAPS_PARITY_EVEN:	uchFormat |= 0x18;	break;
	case SCF_CAPS_PARITY_ODD:	uchFormat |= 0x08;	break;
	default:										break;
	}

	switch (m_Serial[nIndex].dwStopBit)
	{
	case SCF_CAPS_STOP_1:		uchFormat |= 0x00;	break;
	case SCF_CAPS_STOP_2:		uchFormat |= 0x04;	break;
	default:										break;
	}

	// exit ...

	return uchFormat;
}
