/*************************************************************************
 *
 * Win32CE.cpp
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
 * Copyright (c) NCR Corporation 1998-2001.  All rights reserved.
 *
 *************************************************************************
 *
 * $History::
 *
 ************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Include header files
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Win32CE.h"

/////////////////////////////////////////////////////////////////////////////
// Debugging control
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WinCE Wrapper functions
/////////////////////////////////////////////////////////////////////////////

//#if defined(_WIN32_WCE)

/////////////////////////////////////////////////////////////////////////////
// CmRegistry Emulation
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

CmRegistry::CmRegistry()
{
    // initialize variables

    m_hKey = (HKEY)INVALID_HANDLE_VALUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

CmRegistry::~CmRegistry()
{
    // is the registry still opened ?

    if (m_hKey != INVALID_HANDLE_VALUE)
    {
        ::RegCloseKey(m_hKey);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::OpenKey(HKEY hBaseKey, LPCTSTR pszKeyName, REGSAM samDesired)
{
    DWORD   dwDisposition;

    // is the key opened ?

    if (m_hKey != INVALID_HANDLE_VALUE)
    {
        ASSERT(FALSE);
    }

    // attempt to create the key

#if defined(_WIN32_WCE)
    LONG lRet = ::RegCreateKeyEx(hBaseKey,      // handle to an open key
                                 pszKeyName,    // address of subkey name
                                 0,             // reserved
                                 _T(""),        // address of class string
                                 0,             // special options flag
                                 0,             // desired security access
                                 NULL,          // address of key security structure
                                 &m_hKey,       // address of buffer for opened handle
                                 &dwDisposition);   // address of disposition value buffer
#else /* _WIN32_WCE */
    LONG lRet = ::RegCreateKeyEx(hBaseKey,      // handle to an open key
                                 pszKeyName,    // address of subkey name
                                 0,             // reserved
                                 _T(""),            // address of class string
                                 REG_OPTION_NON_VOLATILE, // special options flag
                                 samDesired,          // desired security access
                                 NULL,          // address of key security structure
                                 &m_hKey,       // address of buffer for opened handle
                                 &dwDisposition);   // address of disposition value buffer
#endif /* !_WIN32_WCE */

    // completed ?

    if (lRet != ERROR_SUCCESS)
    {
        m_hKey = (HKEY)INVALID_HANDLE_VALUE;
    }

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::WriteString(LPCTSTR pszValue, LPCTSTR pszData)
{
    DWORD   dwBytes;

    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // compute data length in byte

    dwBytes = (_tcslen(pszData) + sizeof(UCHAR)) * sizeof(TCHAR);

    // attempt to write the key

    LONG lRet = ::RegSetValueEx(m_hKey,     // handle to key to set value for
                                pszValue,   // name of the value to set
                                0,          // reserved  DWORD dwType,
                                REG_SZ,     // flag for value type
                                (const BYTE *)pszData,  // address of value data
                                dwBytes);   // size of value data);

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::Write(LPCTSTR pszValue, DWORD dwType, const BYTE *lpData, DWORD dwBytes)
{
    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // attempt to write the key

    LONG lRet = ::RegSetValueEx(m_hKey,     // handle to key to set value for
                                pszValue,   // name of the value to set
                                0,          // reserved  DWORD dwType,
                                dwType,     // flag for value type
                                lpData,     // address of value data
                                dwBytes);   // size of value data);

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::ReadString(LPCTSTR pszValue, CString& sData)
{
    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // Is pszValue have length ?
    //  * note * WinNT version of RegQueryValueEx() returns FALSE if pszValue is NULL.
    //           Win98 version of RegQueryValueEx() returns TRUE  if pszValue is NULL.
    if (pszValue == NULL)
    {
        return (FALSE);
    }

    // allocate a buffer

    TCHAR   atchBuffer[128];
    DWORD   dwType;
    DWORD   dwBytes = sizeof(atchBuffer);

    // attempt to read the registry

    LONG lRet = ::RegQueryValueEx(m_hKey,       // handle to key to query
                                  pszValue,     // address of name of value to query
                                  0,            // reserved
                                  &dwType,      // address of buffer for value type
                                  (LPBYTE)atchBuffer,   // address of data buffer
                                  &dwBytes);    // address of data buffer size

    // respond to user buffer

    if (lRet == ERROR_SUCCESS)
    {
        sData = atchBuffer;
    }
 
    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::Read(LPCTSTR pszValue, LPDWORD lpType, LPBYTE lpData, LPDWORD lpByte)
{
    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // Is pszValue have length ?
    //  * note * WinNT version of RegQueryValueEx() returns FALSE if pszValue is NULL.
    //           Win98 version of RegQueryValueEx() returns TRUE  if pszValue is NULL.
    if (pszValue == NULL)
    {
        return (FALSE);
    }

    // attempt to read the registry

    LONG lRet = ::RegQueryValueEx(m_hKey,       // handle to key to query
                                  pszValue,     // address of name of value to query
                                  0,            // reserved
                                  lpType,       // address of buffer for value type
                                  lpData,       // address of data buffer
                                  lpByte);      // address of data buffer size

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::EnumKey(DWORD dwIndex, LPTSTR lpszName, LPDWORD lpcbName)
{
    FILETIME ftLastWriteTime;

    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // attempt to read the registry

    LONG lRet = ::RegEnumKeyEx(m_hKey,      // handle to key to query
                                  dwIndex,      // 
                                  lpszName,     // address of name of key
                                  lpcbName,     // size of name buffer
                                  NULL,         // reserved
                                  NULL,         // class
                                  NULL,         // size of class buffer
                                  &ftLastWriteTime);    // last write time

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::EnumValueString(DWORD dwIndex, LPTSTR lpszName, LPDWORD lpcbName, 
                           LPTSTR lpszData, LPDWORD lpcbData)
{
    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // attempt to read the registry

    LONG lRet = ::RegEnumValue(m_hKey,      // handle to key to query
                                  dwIndex,      // 
                                  lpszName,     // address of name of key
                                  lpcbName,     // size of name buffer
                                  NULL,         // reserved
                                  NULL,         // type
                                  (LPBYTE)lpszData,     // address of data buffer
                                  lpcbData);    // size of data buffer

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::DeleteValue(LPCTSTR lpszName)
{
    // key is opened ?

    if (m_hKey == INVALID_HANDLE_VALUE)
    {
        return (FALSE);
    }

    // attempt to read the registry

    LONG lRet = ::RegDeleteValue(m_hKey,        // handle to key to query
                                 lpszName);     // address of name of key

    // exit ...

    return (lRet == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CmRegistry::GetVersionInfo(IID nIID, CString& sVersion)
{
    return (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

VOID CmRegistry::CloseKey()
{
    // close the key if opened

    if (m_hKey != INVALID_HANDLE_VALUE)
    {
        ::RegCloseKey(m_hKey);
    }

    // clear variables

    m_hKey = (HKEY)INVALID_HANDLE_VALUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

LONG CmRegistry::GetLastError()
{
    return (0);
}

/////////////////////////////////////////////////////////////////////////////
// ANSI - UNICODE conversion
/////////////////////////////////////////////////////////////////////////////

void GetGlobalSA(LPCTSTR pszObject,                 // Object specifier
                 LPCTSTR pszName,                   // Object name
                 LPSECURITY_ATTRIBUTES pAttributes, // Object security attributes
                 PSECURITY_DESCRIPTOR pSd)          // Object security descriptor
{
}

/////////////////////////////////////////////////////////////////////////////
// ANSI - UNICODE conversion
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

int ComputeAnsiToWide(LPCSTR lpString)
{
    // compute buffer size necessary for the unicode

    int iBytes = ::MultiByteToWideChar(CP_ACP,      // code page
                                       0,           // character-type options
                                       lpString,    // address of string to map
                                       -1,          // number of bytes in string
                                       NULL,        // address of wide-character buffer
                                       0);          // size of buffer

    // exit ...

    return (iBytes);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

int ConvertAnsiToWide(LPCSTR lpString, LPWSTR lpBuffer, int iSize)
{
    // compute buffer size necessary for the unicode

    int iBytes = ::MultiByteToWideChar(CP_ACP,      // code page
                                       0,           // character-type options
                                       lpString,    // address of string to map
                                       -1,          // number of bytes in string
                                       lpBuffer,    // address of wide-character buffer
                                       iSize);      // size of buffer

    // exit ...

    return (iBytes);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

int ComputeWideToAnsi(LPCWSTR lpString)
{
    // compute buffer size necessary for the unicode

    int iBytes = ::WideCharToMultiByte(CP_ACP,      // code page
                                       0,           // character-type options
                                       lpString,    // address of string to map
                                       -1,          // number of bytes in string
                                       NULL,        // address of wide-character buffer
                                       0,           // size of buffer
                                       NULL,        // address of default for unmappable characters
                                       NULL);       // address of flag set when default char. used

    // exit ...

    return (iBytes);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:
//
// Inputs:
//
// Outputs:
//
/////////////////////////////////////////////////////////////////////////////

int ConvertWideToAnsi(LPCWSTR lpString, LPSTR lpBuffer, int iSize)
{
    // compute buffer size necessary for the unicode

    int iBytes = ::WideCharToMultiByte(CP_ACP,      // code page
                                       0,           // character-type options
                                       lpString,    // address of string to map
                                       -1,          // number of bytes in string
                                       lpBuffer,    // address of wide-character buffer
                                       iSize,       // size of buffer
                                       NULL,        // address of default for unmappable characters
                                       NULL);       // address of flag set when default char. used

    // exit ...

    return (iBytes);
}

/////////////////////////////////////////////////////////////////////////////
//#endif    // _WIN32_WCE
/////////////////////// End Of Program //////////////////////////////////////
