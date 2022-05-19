/*************************************************************************
 *
 * DeviceScanner.cpp
 *
 * $Workfile:: DeviceScanner.cpp                                         $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: DeviceScanner.cpp                                          $
 *
 ************************************************************************/

#include "stdafx.h"
#include "Framework.h"
#include "DeviceIOScanner.h"
#include "DeviceIOScale.h"
#include "DeviceEngine.h"
#include "Device.h"
#include "DeviceScanner.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceScanner::CDeviceScanner();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceScanner::CDeviceScanner(CDeviceEngine* pEngine)
	: CDeviceSS(pEngine)
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceScanner::~CDeviceScanner();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceScanner::~CDeviceScanner()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Open a device
//
// Prototype:	BOOL	CDeviceScanner::Open(lpszDeviceName, lpszDllName);
//
// Inputs:		LPCTSTR	lpszDeviceName;	-
//				LPCTSTR	lpszDllName;	-
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScanner::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
	BOOL	bResult;

	bResult = CDeviceSS::Open(lpszDeviceName, lpszDllName);

	// examine status

	if (! bResult)
	{
		return FALSE;						// exit ...
	}

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

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Close the device
//
// Prototype:	BOOL	CDeviceScanner::Close();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScanner::Close()
{
	DWORD	dwResult;
	DWORD	dwMilliseconds;

	// not open ?

	if (! m_bOpened)
	{
		return FALSE;						// exit ...
	}

	// mark as end of the thread

	m_bDoMyJob = FALSE;

	// close the device

	dwResult = m_pfnClose(m_hDevice);

	// examine status

	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	// wait the thread to the end

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

	::CloseHandle(m_hDone);

	// unload the device DLL

	::FreeLibrary(m_hDll);

	// mark as closed

	m_bOpened = FALSE;

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceScanner::Enable();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScanner::Enable(BOOL bState)
{
	DWORD					dwResult;
	DWORD					dwReturned;
	DEVICEIO_SCANNER_STATE	Request;
	DEVICEIO_SCANNER_STATE	Response;

	// not open ?

	if (! m_bOpened)
	{
		return FALSE;						// exit ...
	}

	Request.fStatus = bState;

	dwResult = m_pfnIoControl(
				m_hDevice,
				DEVICEIO_CTL_SCANNER_ENABLE,
				&Request,
				sizeof(Request),
				&Response,
				sizeof(Response),
				&dwReturned);

	// examine status

	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	// exit ...

	return Response.fStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceScanner::NotOnFile();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScanner::NotOnFile(BOOL bState)
{
	DWORD					dwResult;
	DWORD					dwReturned;
	DEVICEIO_SCANNER_STATE	Request;
	DEVICEIO_SCANNER_STATE	Response;

	// not open ?

	if (! m_bOpened)
	{
		return FALSE;						// exit ...
	}

	Request.fStatus = bState;

	dwResult = m_pfnIoControl(
				m_hDevice,
				DEVICEIO_CTL_SCANNER_NOTONFILE,
				&Request,
				sizeof(Request),
				&Response,
				sizeof(Response),
				&dwReturned);

	// examine status

	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	// exit ...

	return Response.fStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceScanner::ReadWeight(pScale);
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScanner::ReadWeight(PDEVICEIO_SCALE_DATA pScale)
{
	DWORD				dwResult;
	DWORD				dwReturned;
	DEVICEIO_SCALE_DATA	Response;

	dwResult = m_pfnIoControl(
				m_hDevice,
				DEVICEIO_CTL_SCALE_READWEIGHT,
				NULL,
				0,
				&Response,
				sizeof(Response),
				&dwReturned);

	// examine status
	*pScale = Response;
	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Entry point of device thread. (static function)
//
// Prototype:	UINT		CDeviceScanner::DoDeviceThread(lpvData);
//
// Inputs:		LPVOID		lpvData;
//
// Outputs:		UINT		nResult;	- status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDeviceScanner::DoDeviceThread(LPVOID lpvData)
{
	return ((CDeviceScanner*)lpvData)->DeviceThread();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Input Device Engine thread body
//
// Prototype:	UINT		CDeviceScanner::DeviceThread();
//
// Inputs:		nothing
//
// Outputs:		UINT		nResult;	- status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDeviceScanner::DeviceThread()
{
	BOOL		bResult;
	DWORD		dwResult;
	UCHAR		aucData[256];
	DWORD		dwBytesRead;

	// initialize

	m_bDoMyJob = TRUE;

	// issue a signal to become ready

	::SetEvent(m_hDone);					// I'm ready to go !

	// do my job

	while (m_bDoMyJob)						// do my job forever
	{
		memset(aucData, '\0', sizeof(aucData));

		dwResult = m_pfnRead(
						m_hDevice,
						aucData,
						sizeof(aucData),
						&dwBytesRead);

		if (dwResult == DEVICEIO_SUCCESS)
		{
			bResult = m_pEngine->AddData(
						SCF_TYPE_SCANNER,
						aucData,
						dwBytesRead);
		}
	}

	// issue a signal to becom end of thread

	::SetEvent(m_hDone);

	// exit ...

	return 0;
}
