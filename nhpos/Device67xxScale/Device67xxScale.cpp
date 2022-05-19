/*************************************************************************
 *
 * Device67xxScale.cpp
 *
 * $Workfile:: Device67xxScale.cpp                                       $
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
 * $History:: Device67xxScale.cpp                                        $
 *
 ************************************************************************/

#include "stdafx.h"
#include "Device67xxScale.h"
#include "DeviceIO.h"
#include "Device.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CDevice67xxScaleApp

BEGIN_MESSAGE_MAP(CDevice67xxScaleApp, CWinApp)
	//{{AFX_MSG_MAP(CDevice67xxScaleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevice67xxScaleApp construction

CDevice67xxScaleApp::CDevice67xxScaleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	// initialize

	m_pDevice = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDevice67xxScaleApp object

CDevice67xxScaleApp theApp;

BOOL CDevice67xxScaleApp::InitInstance() 
{
	::InitializeCriticalSection(&m_cs);

	return CWinApp::InitInstance();
}

int CDevice67xxScaleApp::ExitInstance() 
{
	if (m_pDevice)
	{
		delete m_pDevice;					// delete the device object
		m_pDevice = NULL;
	}

	::DeleteCriticalSection(&m_cs);
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Initialize the device object (static function)
//
// Prototype:	CDevice*	CDevice67xxScaleApp::InitDevice();
//
// Inputs:		nothing
//
// Outputs:		CDevice*	pDevice;	-
//
/////////////////////////////////////////////////////////////////////////////

CDevice* CDevice67xxScaleApp::InitDevice() 
{
	// get objects

	CDevice67xxScaleApp*	pApp    = (CDevice67xxScaleApp*)AfxGetApp();
	CDevice*				pDevice = pApp->GetDevice();

	::EnterCriticalSection(&pApp->m_cs);

	// is first call ?

	if (pDevice == NULL)
	{
		pDevice = new CDevice();			// create a device object
		pApp->SetDevice(pDevice);
	}

	::LeaveCriticalSection(&pApp->m_cs);

	// exit ...

	return pDevice;
}


/////////////////////////////////////////////////////////////////////////////
// API Entry

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		Open();
//
// Inputs:		
//
// Outputs:		HANDLE		hHandle;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
HANDLE WINAPI Open(
	LPCTSTR lpszDeviceName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HANDLE		dwResult = INVALID_HANDLE_VALUE;// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->Open(lpszDeviceName);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		Close();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI Close(HANDLE hDevice)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->Close(hDevice);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		Write();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI Write(
	HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->Write(
						hDevice,
						lpBuffer,
						dwNumberOfBytesToWrite,
						lpNumberOfBytesWritten);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		Read();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI Read(
	HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD dwNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->Read(
						hDevice,
						lpBuffer,
						dwNumberOfBytesToRead,
						lpNumberOfBytesRead);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		IoControl();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI IoControl(
	HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD dwInBufferSize,
    LPVOID lpOutBuffer,
    DWORD dwOutBufferSize,
    LPDWORD lpBytesReturned)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->IoControl(
						hDevice,
						dwIoControlCode,
						lpInBuffer,
						dwInBufferSize,
						lpOutBuffer,
						dwOutBufferSize,
						lpBytesReturned);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		GetDeviceName();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI GetDeviceName(
    LPDWORD lpDeviceCount,
    LPTSTR lpszDeviceName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->GetDeviceName(
					lpDeviceCount,
					lpszDeviceName);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		GetDeviceCaps();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI GetDeviceCapability(
    LPCTSTR lpszDeviceName,
    DWORD dwFunc,
    DWORD dwType,
    LPVOID lpCapsData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->GetDeviceCapability(
					lpszDeviceName,
					dwFunc,
					dwType,
					lpCapsData);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		GetParameter();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI GetParameter(
    LPCTSTR lpszDeviceName,
    LPCTSTR lpszDataName, 
    LPDWORD lpDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToRead, 
    LPDWORD lpNumberOfBytesRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->GetParameter(
					lpszDeviceName,
					lpszDataName,
					lpDataType,
					lpData,
					dwNumberOfBytesToRead,
					lpNumberOfBytesRead);
	}

	// exit ...

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	HANDLE		SetParameter();
//
// Inputs:		
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DEVICEIO_API
DWORD WINAPI SetParameter(
    LPCTSTR lpszDeviceName,
    LPCTSTR lpszDataName, 
    DWORD dwDataType,
    LPVOID lpData,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD		dwResult = DEVICEIO_E_ILLEGAL;	// assume bad status
	CDevice*	pDevice  = CDevice67xxScaleApp::InitDevice();

	if (pDevice)
	{
		dwResult = pDevice->SetParameter(
					lpszDeviceName,
					lpszDataName,
					dwDataType,
					lpData,
					dwNumberOfBytesToWrite,
					lpNumberOfBytesWritten);
	}

	// exit ...

	return dwResult;
}
