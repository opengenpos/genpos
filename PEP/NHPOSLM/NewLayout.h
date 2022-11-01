// NewLayout.h : main header file for the NEWLAYOUT application
//

#if !defined(AFX_NEWLAYOUT_H__CA88484F_E35A_4024_B7D5_469CA84AE8E0__INCLUDED_)
#define AFX_NEWLAYOUT_H__CA88484F_E35A_4024_B7D5_469CA84AE8E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "NewLayoutView.h"
#include "NewLayoutDoc.h"
#include "LOMCommandLineInfo.h"

static COLORREF myCustomColors[16];

// Define the standard grid width in pixels which we use for
// drawing the grid squares for all controls.  Also defined
// in WindowControl.h in the TouchLibrary files project.

// 111024 R-KSo Why define the same thing twice????
//#define STD_GRID_WIDTH    15

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutApp:
// See NewLayout.cpp for the implementation of this class
//

class CNewLayoutApp : public CWinApp
{
public:
	static unsigned long SerializationVersion;
	CNewLayoutApp();
	CList <CWindowControl *, CWindowControl *> ClipBoardList;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewLayoutApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CNewLayoutApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLAYOUT_H__CA88484F_E35A_4024_B7D5_469CA84AE8E0__INCLUDED_)
