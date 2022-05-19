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
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: Device.cpp                                                 $
 *
 ************************************************************************/

#include "stdafx.h"
#include "Framework.h"
#include "Device.h"
#include "DeviceIO.h"
#include "unicodeFixes.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////

// misc. data structures

typedef struct _PROCADR {				// array of procedure address
	INT			iIndex;						// index to control
	LPCTSTR		lpName;						// ptr. to a proc. name
} PROCADR, *PPROCADR;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDevice::CDevice();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDevice::CDevice(CDeviceEngine* pEngine)
{
	ASSERT(pEngine);

	// initialize

	m_pEngine = pEngine;
	m_bOpened = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Destruction
//
// Prototype:	VOID	CDevice::~CDevice();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDevice::~CDevice()
{

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Open a device
//
// Prototype:	BOOL	CDevice::Open(lpszDeviceName, lpszDllName);
//
// Inputs:		LPCTSTR	lpszDeviceName;	-
//				LPCTSTR	lpszDllName;	-
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
	BOOL	bResult;

	// already opened ?

	if (m_bOpened)
	{
		return FALSE;						// exit ...
	}

	m_sDeviceName = lpszDeviceName;
	m_sDllName    = lpszDllName;

	// initialize a device DLL

	bResult = DllInit();

	// examine status

	if (! bResult)
	{
		return FALSE;						// exit ...
	}

	bResult = DllGetParameter();

	// examine status

	if (! bResult)
	{
		::FreeLibrary(m_hDll);
		return FALSE;						// exit ...
	}

	// open a device

	m_hDevice = m_pfnOpen(lpszDeviceName);

	// examine status

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		::FreeLibrary(m_hDll);
		return FALSE;						// exit ...
	}

	// mark as opened

	m_bOpened = TRUE;

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Close the device
//
// Prototype:	BOOL	CDevice::Close();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::Close()
{
	DWORD	dwResult;

	// not open ?

	if (! m_bOpened)
	{
		return FALSE;						// exit ...
	}

	// close the device

	dwResult = m_pfnClose(m_hDevice);

	// examine status

	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

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
// Prototype:	DWORD	CDevice::GetDeviceType();
//
// Inputs:		nothing
//
// Outputs:		DWORD	dwType;		-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDevice::GetDeviceType()
{
	DWORD	dwType;

	dwType = m_capSummary.dwDeviceType;

	// exit ...

	return dwType;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDevice::DllInit();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::DllInit()    // similar to LoadDevice() for loading a non-OPOS device driver DLL.
{
	BOOL		fCompleted = TRUE;		// assume good status
	FARPROC	*	ppfnProc;

	// declare procedure of System Monitor

	static	PROCADR		afnDlls[] = { {  1, _T("Open")					},
									  {  2, _T("Close")					},
									  {  3, _T("Write")					},
									  {  4, _T("Read")					},
									  {  5, _T("IoControl")				},
									  {  6, _T("GetDeviceName")			},
									  {  7, _T("GetDeviceCapability")	},
									  {  8, _T("GetParameter")			},
									  {  9, _T("SetParameter")			},
									  {  0, NULL						},
									 };

	// load System Monitor dll

	if (! (m_hDll = ::LoadLibrary(m_sDllName)))
	{
		DWORD	dwRC = ::GetLastError();
		return FALSE;
	}

	// get each procedure address

	for (PPROCADR pTable = afnDlls; pTable->lpName; pTable++)
	{
		// save the address

		switch (pTable->iIndex)
		{
		case 1:
			ppfnProc = (FARPROC *)&m_pfnOpen;
			break;
		case 2:
			ppfnProc = (FARPROC *)&m_pfnClose;
			break;
		case 3:
			ppfnProc = (FARPROC *)&m_pfnWrite;
			break;
		case 4:
			ppfnProc = (FARPROC *)&m_pfnRead;
			break;
		case 5:
			ppfnProc = (FARPROC *)&m_pfnIoControl;
			break;
		case 6:
			ppfnProc = (FARPROC *)&m_pfnGetDeviceName;
			break;
		case 7:
			ppfnProc = (FARPROC *)&m_pfnGetDeviceCapability;
			break;
		case 8:
			ppfnProc = (FARPROC *)&m_pfnGetParameter;
			break;
		case 9:
			ppfnProc = (FARPROC *)&m_pfnSetParameter;
			break;
		default:
			break;
		}

		// get the procedure address
#ifdef _WIN32_WCE
		*ppfnProc  = GetProcAddress(m_hDll, pTable->lpName);
#else
		*ppfnProc  = GetProcAddressUnicode(m_hDll, pTable->lpName);
#endif
		fCompleted = (*ppfnProc) ? fCompleted : FALSE;
	}

	if (! fCompleted)
	{
		::FreeLibrary(m_hDll);
		m_hDll = NULL;

		return FALSE;						// exit ...
	}

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDevice::DllGetParameter();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice::DllGetParameter()
{
	DWORD		dwResult;

	// initialize

	m_capSummary.dwSize = sizeof(m_capSummary);

	// get device summary

	dwResult = m_pfnGetDeviceCapability(
					m_sDeviceName,
					SCF_CAPS_SUMMARY,
					0,
					&m_capSummary);

	if (dwResult != SCF_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	// exit ...

	return TRUE;
}
