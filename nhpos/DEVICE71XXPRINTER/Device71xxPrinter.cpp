// Device71xxPrinter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DeviceIO.h"
#include "DeviceIOPrinter.h"
#include "Device71xxPrinter.h"
#include "DeviceCaps.h"
#include "DevPmg.h"

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
// CDevice71xxPrinterApp

BEGIN_MESSAGE_MAP(CDevice71xxPrinterApp, CWinApp)
	//{{AFX_MSG_MAP(CDevice71xxPrinterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevice71xxPrinterApp construction

CDevice71xxPrinterApp::CDevice71xxPrinterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDevice71xxPrinterApp object

CDevice71xxPrinterApp theApp;

BOOL CDevice71xxPrinterApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
    DevPmgInitialize();
	
	return CWinApp::InitInstance();
}

int CDevice71xxPrinterApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	DevPmgFinalize();

	return CWinApp::ExitInstance();
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

	BOOL bRet;

	bRet = DevPmgOpen();

    return DevGetPrtHandle();
#if 0
    if (bRet) 
	{
		return 0;
	}
	else 
	{
		return INVALID_HANDLE_VALUE;
	}
#endif
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
    if(!DevCheckPrtHandle((SHORT)hDevice)){
        return DEVICEIO_E_ILLEGAL;
    }
	DevPmgClose();

	return 0;

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

	DWORD	dwResult;

	PDEVICEIO_PRINTER_WRITE	pPrinterWrite;

    if(!DevCheckPrtHandle((SHORT)hDevice)){
        return DEVICEIO_E_ILLEGAL;
    }

	pPrinterWrite = (PDEVICEIO_PRINTER_WRITE)lpBuffer;

	{
		DWORD	dwResult;
		CHAR    lineReset[] = "\x1b|1C";

		dwResult = DevPmgPrint((USHORT)1, (UCHAR *)lineReset, strlen(lineReset));
	}
	dwResult = DevPmgPrint( (USHORT)1, (UCHAR *)lpBuffer, (USHORT)dwNumberOfBytesToWrite);

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

	return DEVICEIO_E_ILLEGAL;

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

	DWORD	dwResult;

    if(!DevCheckPrtHandle((SHORT)hDevice)){
        return DEVICEIO_E_ILLEGAL;
    }

	dwResult   = DEVICEIO_SUCCESS;
	
	switch (dwIoControlCode)
	{
	case DEVICEIO_CTL_PRINTER_WAIT:
		DevPmgWait();
		break;
	case DEVICEIO_CTL_PRINTER_VALIDATION:
		dwResult = DevPmgBeginValidation(
			((PDEVICEIO_PRINTER_ARG_VALIDATION)lpOutBuffer)->wPrintStation);
		break;
	case DEVICEIO_CTL_PRINTER_END_VALIDATION:
		dwResult = DevPmgEndValidation(
			((PDEVICEIO_PRINTER_ARG_VALIDATION)lpOutBuffer)->wPrintStation);
		break;
	case DEVICEIO_CTL_PRINTER_FEED:
		dwResult = DevPmgFeed(
			((PDEVICEIO_PRINTER_ARG_FEED)lpOutBuffer)->wPrintStation, 
			((PDEVICEIO_PRINTER_ARG_FEED)lpOutBuffer)->wFeedLen);
		break;
	case DEVICEIO_CTL_PRINTER_IMPORTANT:
		dwResult = DevPmgBeginImportant(
			((PDEVICEIO_PRINTER_ARG_IMPORTANT)lpOutBuffer)->wPrintStation);
		break;
	case DEVICEIO_CTL_PRINTER_END_IMPORTANT:
		dwResult = DevPmgEndImportant(
			((PDEVICEIO_PRINTER_ARG_END_IMPORTANT)lpOutBuffer)->wPrintStation);
		break;
	case DEVICEIO_CTL_PRINTER_GETSTATUS:
		dwResult = DevPmgGetStatus(
			((PDEVICEIO_PRINTER_ARG_GETSTATUS)lpOutBuffer)->wPrintStation, 
			((PDEVICEIO_PRINTER_ARG_GETSTATUS)lpOutBuffer)->lpStatus);
		break;
	case DEVICEIO_CTL_PRINTER_PRTCONFIG:
		dwResult = DevPmgPrtConfig(
			((PDEVICEIO_PRINTER_ARG_PRTCONFIG)lpOutBuffer)->wPrintStation, 
			((PDEVICEIO_PRINTER_ARG_PRTCONFIG)lpOutBuffer)->lpColumn, 
			((PDEVICEIO_PRINTER_ARG_PRTCONFIG)lpOutBuffer)->lpStatus);
		break;
	case DEVICEIO_CTL_PRINTER_SET_CALLBACK:
		DevPmgSetCallBack(
			((PDEVICEIO_PRINTER_ARG_SET_CALLBACK)lpOutBuffer)->lpCallBack);
		break;
	case DEVICEIO_CTL_PRINTER_FONT:
		dwResult = DevPmgFont(
			((PDEVICEIO_PRINTER_ARG_FONT)lpOutBuffer)->wPrintStation, 
			((PDEVICEIO_PRINTER_ARG_FONT)lpOutBuffer)->wFont);
		break;
	case DEVICEIO_CTL_PRINTER_VOL_WAIT_COUNT:
		DevPmgSetValWaitCount(
			((PDEVICEIO_PRINTER_ARG_VOL_WAIT_COUNT)lpOutBuffer)->wRetryCount);
		break;
//US Customs slip printer control change for short forms, NCR 7158
// Other printer DLLs will not handle this message
	case DEVICEIO_CTL_PRINTER_SMALL_VALIDATION:
		dwResult = DevPmgBeginSmallValidation(
			((PDEVICEIO_PRINTER_ARG_VALIDATION)lpOutBuffer)->wPrintStation);
		break;
	case DEVICEIO_CTL_PRINTER_END_SMALL_VALIDATION:
		dwResult = DevPmgEndSmallValidation(
			((PDEVICEIO_PRINTER_ARG_VALIDATION)lpOutBuffer)->wPrintStation);
		break;
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

// My Implementation
CDeviceCaps		s_DeviceCaps;			// Device Capability Interface

	return (s_DeviceCaps.GetDeviceName(lpDeviceCount, lpszDeviceName));
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

// My Implementation
CDeviceCaps		s_DeviceCaps;			// Device Capability Interface


	// exit ...

	return (s_DeviceCaps.GetDeviceCapability(lpszDeviceName, dwFunc, dwType,lpCapsData));
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

// My Implementation
CDeviceCaps		s_DeviceCaps;			// Device Capability Interface


	// exit ...

	return (s_DeviceCaps.GetParameter(lpszDeviceName, lpszDataName, lpDataType, 
									  lpData, dwNumberOfBytesToRead, lpNumberOfBytesRead)); 
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

// My Implementation
CDeviceCaps		s_DeviceCaps;			// Device Capability Interface


	// exit ...

	return (s_DeviceCaps.SetParameter(lpszDeviceName, lpszDataName, dwDataType, lpData, 
									  dwNumberOfBytesToWrite, lpNumberOfBytesWritten));
}

