/*************************************************************************
 *
 * DeviceDirectMSR.h
 *
 * $Workfile:: DeviceDirectMSR.h                                         $
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
 * $History:: DeviceDirectMSR.h                                          $
 *
 ************************************************************************/

#if !defined(AFX_DEVICEDIRECTMSR_H__814C3278_A973_11D3_88F3_005004B8DA0C__INCLUDED_)
#define AFX_DEVICEDIRECTMSR_H__814C3278_A973_11D3_88F3_005004B8DA0C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeviceDirectMSRApp
// See DeviceDirectMSR.cpp for the implementation of this class
//

class CDevice;

class CDeviceDirectMSRApp : public CWinApp
{
public:
	CDeviceDirectMSRApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceDirectMSRApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDeviceDirectMSRApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// public functions.
public:
	static CDevice*	InitDevice();
	static CDevice*	GetDevice()
						{ return ((CDeviceDirectMSRApp*)AfxGetApp())->m_pDevice; }
	static void		SetDevice(CDevice* pDevice)
						{ ((CDeviceDirectMSRApp*)AfxGetApp())->m_pDevice = pDevice; }

// Internal variables.
protected:
	CRITICAL_SECTION	m_cs;				//
	CDevice*			m_pDevice;			// device object
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEDIRECTMSR_H__814C3278_A973_11D3_88F3_005004B8DA0C__INCLUDED_)
