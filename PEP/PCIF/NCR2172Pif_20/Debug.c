/////////////////////////////////////////////////////////////////////////////
// Debug.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Aug-25-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "debug.h"
#include "AW_Interfaces.h"


/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////

#define LOG_FILE_NAME		L"NCR2170.LOG"

HANDLE	g_hLogFile = INVALID_HANDLE_VALUE;

//	function prototypes

BOOL	OpenKey(PHKEY phKey, LPCTSTR pszDeviceKey, BOOL fWriteAccess);
BOOL	ReadDataString(HKEY hKey, LPCTSTR pszValue, LPTSTR lpszData, DWORD dwType);
BOOL	ReadDataValue(HKEY hKey, LPCTSTR pszValue, DWORD* pdwData);


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		DebugInit();
//
// Inputs:		
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID DebugInit()
{
	BOOL	fResult;
	HKEY	hKey;
	DWORD	dwData;
	DWORD	dwFileSize;

	fResult = OpenKey(&hKey, L"SOFTWARE\\NCR\\2170\\PCIF", FALSE);

	if (! fResult)
	{
		return;								// exit ...
	}

	fResult = ReadDataValue(hKey, L"LogFileControl", &dwData);

	RegCloseKey(hKey);

	if (! fResult || dwData < 1)			// error or log disabled ?
	{
		return;								// exit ...
	}

	g_hLogFile = CreateFile(LOG_FILE_NAME,
							GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							0,
							NULL);

	if (g_hLogFile == INVALID_HANDLE_VALUE)
	{
		return;								// exit ...
	}

	dwFileSize = GetFileSize(g_hLogFile, NULL);

	SetFilePointer(g_hLogFile, dwFileSize, NULL, FILE_BEGIN);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		DebugUninit();
//
// Inputs:		
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID DebugUninit()
{
	if (g_hLogFile == INVALID_HANDLE_VALUE)
	{
		return;								// exit ...
	}

	CloseHandle(g_hLogFile);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		DebugInit();
//
// Inputs:		
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID DebugPrintf(LPCTSTR lpszFormat, ...)
{
	BOOL		fResult;
	DWORD		dwBytes;
	va_list		marker;
	TCHAR		szBuffer[1024];
	SYSTEMTIME	st;

	if (g_hLogFile == INVALID_HANDLE_VALUE)
	{
		return;								// exit ...
	}

	GetLocalTime(&st);
	wsprintf (szBuffer, L"%02d/%02d/%04d %02d:%02d:%02d %04X> ",
		st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond,
		GetCurrentThreadId());
	fResult = WriteFile(g_hLogFile, szBuffer, wcslen(szBuffer), &dwBytes, NULL);

	va_start(marker, lpszFormat);
	wvsprintf(szBuffer, lpszFormat, marker);
	va_end(marker);
	fResult = WriteFile(g_hLogFile, szBuffer, wcslen(szBuffer), &dwBytes, NULL);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		OpenKey(pszDeviceKey, fWriteAccess);
//
// Inputs:		LPCTSTR		pszDeviceKey;	- device key
//				BOOL		fWriteAccess;	- 
//
// Outputs:		BOOL		fResult;		- result code
//
/////////////////////////////////////////////////////////////////////////////

BOOL OpenKey(PHKEY phKey, LPCTSTR pszDeviceKey, BOOL fWriteAccess)
{
	LONG	lResult;
	REGSAM	samDesired;

	samDesired = fWriteAccess ? (KEY_QUERY_VALUE | KEY_SET_VALUE) :
								(KEY_QUERY_VALUE);

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						   pszDeviceKey,
						   0,
						   samDesired,
						   phKey);

	// exit ...

	return (lResult == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Read the registry string
//
// Prototype:	BOOL		ReadData(hKey, pszValue, lpszData, dwType);
//
// Inputs:		HKEY		hKey;			-
//				LPCTSTR		pszValue;		-
//				LPTSTR		sData;			-
//				DWORD		dwType;			-
//
// Outputs:		BOOL		fResult;		- result code
//
/////////////////////////////////////////////////////////////////////////////

BOOL ReadDataString(HKEY hKey, LPCTSTR pszValue, LPTSTR lpszData, DWORD dwType)
{
	LONG	lResult;
	DWORD	dwRegType;
	DWORD	dwLength;

    // look for value, get data size

	lResult = RegQueryValueEx(hKey,				// handle of key to query 
							  pszValue,			// address of name of value to query 
							  NULL,				// reserved 
							  &dwRegType,		// address of buffer for value type 
							  NULL,				// address of data buffer: just return size.
							  &dwLength);		// address of data buffer size 

    if (lResult != ERROR_SUCCESS || dwRegType != dwType)
    {
		return FALSE;							// exit ...
	}

	*lpszData = L'\0';

	lResult = RegQueryValueEx(hKey,				// handle of key to query 
							  pszValue,			// address of name of value to query 
							  NULL,				// reserved 
							  &dwType,			// address of buffer for value type 
							  (PBYTE)lpszData,	// address of data buffer 
							  &dwLength );		// address of data buffer size 

	// exit ...

	return (lResult == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Read the registry string
//
// Prototype:	BOOL		CRegistry::ReadData(hKey, pszValue, pdwData);
//
// Inputs:		DWORD*		pdwData;		-
//
// Outputs:		BOOL		fResult;		- result code
//
/////////////////////////////////////////////////////////////////////////////

BOOL ReadDataValue(HKEY hKey, LPCTSTR pszValue, DWORD* pdwData)
{
	LONG	lResult;
	DWORD	dwType;
	DWORD	dwLength;

    // look for value, get data size

	lResult = RegQueryValueEx(hKey,				// handle of key to query 
							  pszValue,			// address of name of value to query 
							  NULL,				// reserved 
							  &dwType,			// address of buffer for value type 
							  NULL,				// address of data buffer: just return size.
							  &dwLength);		// address of data buffer size 

    if (lResult != ERROR_SUCCESS || dwType != REG_DWORD || dwLength == 0)
    {
		return FALSE;							// exit ...
	}


	lResult = RegQueryValueEx(hKey,				// handle of key to query 
							  pszValue,			// address of name of value to query 
							  NULL,				// reserved 
							  &dwType,			// address of buffer for value type 
							  (PBYTE)pdwData,	// address of data buffer 
							  &dwLength);		// address of data buffer size 

	// exit ...

	return (lResult == ERROR_SUCCESS);
}


///////////////////////////////	 End of File  ///////////////////////////////
