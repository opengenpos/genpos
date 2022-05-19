// SystemConfig.h : main header file for the SYSTEMCONFIG DLL
//

#if !defined(AFX_SYSTEMCONFIG_H__255CFED8_A817_11D3_BB93_005004B8DABD__INCLUDED_)
#define AFX_SYSTEMCONFIG_H__255CFED8_A817_11D3_BB93_005004B8DABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSystemConfigApp
// See SystemConfig.cpp for the implementation of this class
//

class CSystemConfigApp : public CWinApp
{
public:
	CSystemConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSystemConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMCONFIG_H__255CFED8_A817_11D3_BB93_005004B8DABD__INCLUDED_)
