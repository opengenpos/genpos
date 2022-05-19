/*************************************************************************
 *
 * Win32CE.h
 *
 * $Workfile::
 *
 * $Revision::
 *
 * $Archive::
 *
 * $Date::
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 1998.  All rights reserved.
 *
 *************************************************************************
 *
 * $History::
 * 
 *
 ************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Wrapper functions for WinCE
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CmRegistry emulation
/////////////////////////////////////////////////////////////////////////////

class CmRegistry
{
private:
	HKEY	m_hKey;
public:
	CmRegistry();								// constructor
	virtual ~CmRegistry();						// destructor
public:
	BOOL	OpenKey(HKEY hBaseKey, LPCTSTR pszKeyName, REGSAM samDesired = KEY_READ);
	BOOL	WriteString(LPCTSTR pszValue, LPCTSTR pszData);
	BOOL	Write(LPCTSTR pszValue, DWORD dwType, const BYTE *lpData, DWORD dwBytes);
	BOOL	ReadString(LPCTSTR pszValue, CString& sData);
	BOOL	Read(LPCTSTR pszValue, LPDWORD lpType, LPBYTE lpData, LPDWORD lpByte);
	BOOL	EnumKey(DWORD dwIndex, LPTSTR lpszName, LPDWORD lpcbName);
	BOOL	EnumValueString(DWORD dwIndex, LPTSTR lpszName, LPDWORD lpcbName, LPTSTR lpszData, LPDWORD lpcbData);
	BOOL	DeleteValue(LPCTSTR lpszName);
	BOOL	GetVersionInfo(IID nIID, CString& sVersion);
	VOID	CloseKey(VOID);
	LONG	GetLastError(VOID);
};

/////////////////////////////////////////////////////////////////////////////
// ANSI - UNICODE Conversion
/////////////////////////////////////////////////////////////////////////////

int	ComputeAnsiToWide(LPCSTR lpString);
int	ConvertAnsiToWide(LPCSTR lpString, LPWSTR lpBuffer, int iSize);
int	ComputeWideToAnsi(LPCWSTR lpString);
int	ConvertWideToAnsi(LPCWSTR lpString, LPSTR lpBuffer, int iSize);

/////////////////////////////////////////////////////////////////////////////
