/*************************************************************************
 *
 * Device78xxScanner.h
 *
 * $Workfile:: Device78xxScanner.h                                       $
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
 * $History:: Device78xxScanner.h                                        $
 *
 ************************************************************************/

#if !defined(AFX_DEVICE78XXSCANNER_H__7A763E58_A1AA_11D3_88E9_005004B8DA0C__INCLUDED_)
#define AFX_DEVICE78XXSCANNER_H__7A763E58_A1AA_11D3_88E9_005004B8DA0C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDevice78xxScannerApp
// See Device78xxScanner.cpp for the implementation of this class
//

class CDevice;

typedef struct _InfDevice
{
	CString		sDevice;			// device name
	CDevice*	pDevice;			// device object
} InfDevice, *PInfDevice;

class CDevice78xxScannerApp : public CWinApp
{
public:
	CDevice78xxScannerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevice78xxScannerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDevice78xxScannerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// public functions.
public:
	static CDevice* InitDevice(LPCTSTR lpszDeviceName);
	static BOOL		UninitDevice(HANDLE hDevice);
	static CDevice*	GetDevice(LPCTSTR lpszDeviceName);
	static CDevice*	GetDevice(HANDLE hDevice);

// Internal variables.
protected:
	CRITICAL_SECTION	m_cs;				//
	InfDevice			m_infDevice[NUMBER_OF_DEVICE];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICE78XXSCANNER_H__7A763E58_A1AA_11D3_88E9_005004B8DA0C__INCLUDED_)
