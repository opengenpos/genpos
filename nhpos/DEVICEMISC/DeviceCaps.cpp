// DeviceCaps.cpp: implementation of the CDeviceCaps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceIO.h"
#include "DeviceCaps.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	CDeviceCaps::CDeviceCaps();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceCaps::CDeviceCaps()
{
#if defined (DEVICE_BEVERAGE)       || \
	defined (DEVICE_CHARGE_POSTING) || \
	defined (DEVICE_EPT)            || \
	defined (DEVICE_EPT_EX)         || \
	defined (DEVICE_KITCHEN_CRT)    || \
	defined (DEVICE_PCIF)

	// initialize

	m_bInitialized = FALSE;

	m_sName[0].LoadString(IDS_DEVICENAME1);

	m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
	m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
	m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

	m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
	m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_300  |
	                              SCF_CAPS_BAUD_600  |
	                              SCF_CAPS_BAUD_1200 |
	                              SCF_CAPS_BAUD_2400 |
	                              SCF_CAPS_BAUD_4800 |
	                              SCF_CAPS_BAUD_9600 |
		                          SCF_CAPS_BAUD_19200 |
                                  SCF_CAPS_BAUD_38400 |
                                  SCF_CAPS_BAUD_57600 |
                                  SCF_CAPS_BAUD_115200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8 |
	                              SCF_CAPS_CHAR_7;
	m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
	                              SCF_CAPS_PARITY_EVEN |
	                              SCF_CAPS_PARITY_ODD;
	m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1 |
	                              SCF_CAPS_STOP_2;
	m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;
#endif

#if defined (DEVICE_KITCHEN_PRINTER)

	// initialize

	m_bInitialized = FALSE;

	m_sName[0].LoadString(IDS_DEVICENAME1);
	m_sName[1].LoadString(IDS_DEVICENAME2);
	m_sName[2].LoadString(IDS_DEVICENAME3);
	m_sName[3].LoadString(IDS_DEVICENAME4);
	m_sName[4].LoadString(IDS_DEVICENAME5);
	m_sName[5].LoadString(IDS_DEVICENAME6);

	m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
	m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
	m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_Caps[1] = m_Caps[0];
    m_Caps[2] = m_Caps[0];
    m_Caps[3] = m_Caps[0];
    m_Caps[4] = m_Caps[0];
    m_Caps[5] = m_Caps[0];

	m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
	m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_4800 |
	                              SCF_CAPS_BAUD_9600;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8 |
	                              SCF_CAPS_CHAR_7;
	m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
	                              SCF_CAPS_PARITY_EVEN |
	                              SCF_CAPS_PARITY_ODD;
	m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1 |
	                              SCF_CAPS_STOP_2;
	m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_XONOFF;

    m_CapsSerial[1] = m_CapsSerial[0];
    m_CapsSerial[2] = m_CapsSerial[0];
    m_CapsSerial[3] = m_CapsSerial[0];
    m_CapsSerial[4] = m_CapsSerial[0];
    m_CapsSerial[5] = m_CapsSerial[0];

#endif
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Destruction
//
// Prototype:	CDeviceCaps::~CDeviceCaps();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceCaps::~CDeviceCaps()
{

}

//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		CDeviceCaps::GetDeviceName(lpDeviceCount, lpszDeviceName);
//
// Inputs:		LPVOID		lpDeviceCount;	-
//				DWORD		lpszDeviceName;
//
// Outputs:		DWORD		dwResult;	-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDeviceCaps::GetDeviceName(LPDWORD lpDeviceCount, LPTSTR lpszDeviceName)
{
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
// Prototype:	DWORD		CDeviceCaps::GetDeviceCapability(lpszDeviceName,
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

DWORD CDeviceCaps::GetDeviceCapability(LPCTSTR lpszDeviceName, 
										 DWORD dwFunc, DWORD dwType, LPVOID lpCapsData)
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
				break;

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


DWORD CDeviceCaps::GetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, 
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
//								lpData, dwNumberOfBytesToWrite, lpNumberOfBytesWritten);
//
// Inputs:		LPCTSTR		lpszDeviceName;			-
//				LPCTSTR		lpszDataName;			-
//				LPVOID		lpData;					-
//				DWORD		dwNumberOfBytesToWrite;	-
//				LPDWORD		lpNumberOfBytesWritten;	-
//
// Outputs:		DWORD		dwResult;				-
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDeviceCaps::SetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, 
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
// Function:	
//
// Prototype:	int 		CDeviceCaps::GetDeviceIndex(lpszDeviceName);
//
// Inputs:		LPCTSTR		lpszDeviceName;	-
//
// Outputs:		int 		bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

int CDeviceCaps::GetDeviceIndex(LPCTSTR lpszDeviceName)
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

