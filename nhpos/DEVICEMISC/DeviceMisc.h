// DeviceMisc.h : main header file for the DEVICEMISC DLL
//

#if !defined(AFX_DEVICEMISC_H__208568B9_AE7C_11D3_BB9B_005004B8DABD__INCLUDED_)
#define AFX_DEVICEMISC_H__208568B9_AE7C_11D3_BB9B_005004B8DABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeviceMiscApp
// See DeviceMisc.cpp for the implementation of this class
//

class CDeviceMiscApp : public CWinApp
{
public:
	CDeviceMiscApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceMiscApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDeviceMiscApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEMISC_H__208568B9_AE7C_11D3_BB9B_005004B8DABD__INCLUDED_)
