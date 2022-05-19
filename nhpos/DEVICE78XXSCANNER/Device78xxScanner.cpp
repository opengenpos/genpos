/*************************************************************************
 *
 * Device78xxScanner.cpp
 *
 * $Workfile:: Device78xxScanner.cpp                                     $
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
 * $History:: Device78xxScanner.cpp                                      $
 *
 ************************************************************************/

#include "stdafx.h"
#include "DeviceIO.h"
#include "Device.h"
#include "Device78xxScanner.h"

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
// CDevice78xxScannerApp

BEGIN_MESSAGE_MAP(CDevice78xxScannerApp, CWinApp)
	//{{AFX_MSG_MAP(CDevice78xxScannerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevice78xxScannerApp construction

CDevice78xxScannerApp::CDevice78xxScannerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		m_infDevice[nIndex].pDevice = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDevice78xxScannerApp object

CDevice78xxScannerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDevice78xxScannerApp initialization

BOOL CDevice78xxScannerApp::InitInstance() 
{
	::InitializeCriticalSection(&m_cs);

	return CWinApp::InitInstance();
}

int CDevice78xxScannerApp::ExitInstance() 
{
	::EnterCriticalSection(&m_cs);

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		PInfDevice	pInfo = &m_infDevice[nIndex];

		if (pInfo->pDevice)
		{
			pInfo->pDevice->Close(pInfo->pDevice);
			delete pInfo->pDevice;			// delete the device object
			pInfo->pDevice = NULL;
		}
	}

	::LeaveCriticalSection(&m_cs);
	::DeleteCriticalSection(&m_cs);

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Initialize the device object (static function)
//
// Prototype:	CDevice*	CDevice78xxScannerApp::InitDevice(lpszDeviceName);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//
// Outputs:		CDevice*	pDevice;		-
//
/////////////////////////////////////////////////////////////////////////////

CDevice* CDevice78xxScannerApp::InitDevice(LPCTSTR lpszDeviceName) 
{
	// get objects

	CDevice78xxScannerApp*	pApp    = (CDevice78xxScannerApp*)AfxGetApp();
	CDevice*				pDevice = NULL;
	PInfDevice				pFree   = NULL;

	::EnterCriticalSection(&pApp->m_cs);

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		PInfDevice	pInfo = &(pApp->m_infDevice[nIndex]);

		if (pInfo->sDevice.CompareNoCase(lpszDeviceName) == 0)
		{
			pDevice = pInfo->pDevice;
			pFree   = NULL;
			break;
		}
		else if (pFree == NULL && pInfo->pDevice == NULL)
		{
			pFree = pInfo;
		}
	}

	if (pFree)
	{
		pFree->pDevice = new CDevice();		// create a device object
		pFree->sDevice = lpszDeviceName;
		pDevice        = pFree->pDevice;
	}

	::LeaveCriticalSection(&pApp->m_cs);

	// exit ...

	return pDevice;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Uninitialize the device object (static function)
//
// Prototype:	BOOL		CDevice78xxScannerApp::UninitDevice(hDevice);
//
// Inputs:		HANDLE		hDevice;	-
//
// Outputs:		BOOL		bResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDevice78xxScannerApp::UninitDevice(HANDLE hDevice) 
{
	// get objects

	BOOL					bResult = FALSE;
	CDevice78xxScannerApp*	pApp    = (CDevice78xxScannerApp*)AfxGetApp();

	::EnterCriticalSection(&pApp->m_cs);

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		PInfDevice	pInfo = &(pApp->m_infDevice[nIndex]);

		if (hDevice == pInfo->pDevice)
		{
			delete pInfo->pDevice;			// delete the device object
			pInfo->sDevice.Empty();
			pInfo->pDevice = NULL;
			bResult        = TRUE;
			break;
		}
	}

	::LeaveCriticalSection(&pApp->m_cs);

	// exit ...

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Get the device object (static function)
//
// Prototype:	CDevice*	CDevice78xxScannerApp::GetDevice(lpszDeviceName);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//
// Outputs:		CDevice*	pDevice;		-
//
/////////////////////////////////////////////////////////////////////////////

CDevice* CDevice78xxScannerApp::GetDevice(LPCTSTR lpszDeviceName) 
{
	// get objects

	CDevice78xxScannerApp*	pApp    = (CDevice78xxScannerApp*)AfxGetApp();
	CDevice*				pDevice = NULL;

	::EnterCriticalSection(&pApp->m_cs);

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		PInfDevice	pInfo = &(pApp->m_infDevice[nIndex]);

		if (pInfo->sDevice.CompareNoCase(lpszDeviceName) == 0)
		{
			pDevice = pInfo->pDevice;
			break;
		}
	}

	::LeaveCriticalSection(&pApp->m_cs);

	// exit ...

	return pDevice;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Get the device object (static function)
//
// Prototype:	CDevice*	CDevice78xxScannerApp::GetDevice(hDevice);
//
// Inputs:		HANDLE		hDevice;	-
//
// Outputs:		CDevice*	pDevice;	-
//
/////////////////////////////////////////////////////////////////////////////

CDevice* CDevice78xxScannerApp::GetDevice(HANDLE hDevice) 
{
	// get objects

	CDevice78xxScannerApp*	pApp    = (CDevice78xxScannerApp*)AfxGetApp();
	CDevice*				pDevice = NULL;

	::EnterCriticalSection(&pApp->m_cs);

	for (int nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		PInfDevice	pInfo = &(pApp->m_infDevice[nIndex]);

		if (hDevice == pInfo->pDevice)
		{
			pDevice = pInfo->pDevice;
			break;
		}
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
	CDevice*	pDevice  = CDevice78xxScannerApp::InitDevice(lpszDeviceName);

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
	CDevice*	pDevice  = CDevice78xxScannerApp::GetDevice(hDevice);

	if (pDevice)
	{
		dwResult = pDevice->Close(hDevice);

//		if (dwResult == DEVICEIO_SUCCESS)
//		{
//			CDevice78xxScannerApp::UninitDevice(hDevice);
//		}
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
	CDevice*	pDevice  = CDevice78xxScannerApp::GetDevice(hDevice);

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
	CDevice*	pDevice  = CDevice78xxScannerApp::GetDevice(hDevice);

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
	CDevice*	pDevice  = CDevice78xxScannerApp::GetDevice(hDevice);

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

	CString		sName;
	int			nIndex;
	LPTSTR		lpszBuffer;

	// check parameters

	if (lpDeviceCount == NULL || lpszDeviceName == NULL)
	{
		return DEVICEIO_E_ILLEGAL;			// exit ...
	}

	// make up data

	*lpDeviceCount = NUMBER_OF_DEVICE;
	lpszBuffer     = lpszDeviceName;

	for (nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
	{
		sName.LoadString(IDS_DEVICENAME1 + nIndex);
		_tcscpy(lpszBuffer, sName);
		lpszBuffer += (sName.GetLength() + 1);
	}

	*lpszBuffer = _T('\0');

	// exit ...

	return DEVICEIO_SUCCESS;
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
	CDevice*	pDevice  = CDevice78xxScannerApp::InitDevice(lpszDeviceName);

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
	CDevice*	pDevice  = CDevice78xxScannerApp::InitDevice(lpszDeviceName);

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
	CDevice*	pDevice  = CDevice78xxScannerApp::InitDevice(lpszDeviceName);

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

