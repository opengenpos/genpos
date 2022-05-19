/*************************************************************************
 *
 * Device.h
 *
 * $Workfile:: Device.h                                                  $
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
 * $History:: Device.h                                                   $
 *
 ************************************************************************/

#if !defined(AFX_DEVICE_H__1095A1F6_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_)
#define AFX_DEVICE_H__1095A1F6_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SCF.h"


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

class CDeviceEngine;

class CDevice : public CObject
{
public:
	CDevice(CDeviceEngine* pEngine);
	virtual ~CDevice();

// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName, AFX_THREADPROC pfnThread);
	BOOL	Close();
	DWORD	GetDeviceType();

// Internal functions
protected:
	BOOL	DllInit();
	BOOL	DllGetParameter();
protected:
	// This function sould be the same as the definition of the 
	// header file (DeviceIO.h).
	// Because, this use not a static link but LoadLibrary() API.
	HANDLE	(WINAPI *m_pfnOpen)(LPCTSTR);
	DWORD	(WINAPI *m_pfnClose)(HANDLE);
	DWORD	(WINAPI *m_pfnWrite)(HANDLE, LPVOID, DWORD, LPDWORD);
	DWORD	(WINAPI *m_pfnRead)(HANDLE, LPVOID, DWORD, LPDWORD);
	DWORD	(WINAPI *m_pfnIoControl)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD);
	DWORD	(WINAPI *m_pfnGetDeviceName)(LPDWORD, LPTSTR);
	DWORD	(WINAPI *m_pfnGetDeviceCapability)(LPCTSTR, DWORD, DWORD, LPVOID);
	DWORD	(WINAPI *m_pfnGetParameter)(LPCTSTR, LPCTSTR, LPDWORD, LPVOID, DWORD, LPDWORD);
	DWORD	(WINAPI *m_pfnSetParameter)(LPCTSTR, LPCTSTR, DWORD, LPVOID, DWORD, LPDWORD);

// Internal variables.
protected:
	BOOL			m_bOpened;				//
	BOOL			m_bEnabled;				//
	CDeviceEngine*	m_pEngine;				//
	CString			m_sDeviceName;			// device name
	CString			m_sDllName;				// DLL name
	HMODULE			m_hDll;					// DLL handle
	HANDLE			m_hDevice;				// device handle
	DEVICE_CAPS		m_capSummary;
};

#endif // !defined(AFX_DEVICE_H__1095A1F6_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_)
