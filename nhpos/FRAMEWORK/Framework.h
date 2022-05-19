/*************************************************************************
 *
 * Framework.h
 *
 * $Workfile:: Framework.h                                               $
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
 * $History:: Framework.h                                                $
 *
 ************************************************************************/

#if !defined(AFX_FRAMEWORK_H__E7F69277_9468_11D3_88D3_005004B8DA0C__INCLUDED_)
#define AFX_FRAMEWORK_H__E7F69277_9468_11D3_88D3_005004B8DA0C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#ifdef TOUCHSCREEN
#include "FrameworkWndDoc.h"
#endif

#ifdef DEVENG_REPLACEMENT
#include "DeviceEngineThread.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameworkApp:
// See Framework.cpp for the implementation of this class
//

class CDeviceEngine;

class CFrameworkApp : public CWinApp
{
public:
	CFrameworkApp();
	~CFrameworkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameworkApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFrameworkApp)
	void			OnDeviceEngineIOCTL(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// My Implementation
public:

#ifndef DEVENG_REPLACEMENT
	CDeviceEngine*	GetDeviceEngine() { return m_pDeviceEngine; }
#else
#endif

	void	        OnEvsUpdate(WPARAM wParam, LPARAM lParam);
	BOOL			InitializeLayout(LPCTSTR lpszFileName);

	HANDLE m_hEventShutdownNHPOS;  // shutdown event set by on a WM_APP_SHUTDOWN_MSG

	TCHAR  m_BiosSerialNumber[64];

protected:

#ifndef DEVENG_REPLACEMENT
	CDeviceEngine*	m_pDeviceEngine;		// device engine
#else
	CWinThread*		m_pDeviceEngineThread;
#endif


private:
	CFrameworkWndDoc touchWindow;
	BOOL     m_bSmallWindow;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEWORK_H__E7F69277_9468_11D3_88D3_005004B8DA0C__INCLUDED_)
