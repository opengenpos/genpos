/////////////////////////////////////////////////////////////////////////////
// progrpt.h : main header file for the PROGRPT application
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "info.h"			// file information

/////////////////////////////////////////////////////////////////////////////

#define	ID_SET_PATHNAME		(WM_USER + 10)
#define	ID_FIELD_DLG		(WM_USER + 11)
#define	ID_STATUS_MSG		(WM_USER + 12)
/*#define PROGRPT_VERSION         0x0330  * 2170 HPUS Release 3.3 */
#define	PROGRPT_VERSION		0x0100 /* Saratoga Release 1.0 */
#define	PROGRPT_OK			1
#define	PROGRPT_ERROR		0
#define	PROGRPT_WARNING		(-1)

/////////////////////////////////////////////////////////////////////////////

extern	wchar_t szClassName[];
extern	CInfo* ptheInfo;				// information of report field

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp:
// See progrpt.cpp for the implementation of this class
//

class CProgrptApp : public CWinApp
{
public:
	CProgrptApp();
	~CProgrptApp();
	
	CMultiDocTemplate*	m_pListViewTemplate;
	CMultiDocTemplate*	m_pReportViewTemplate;
	UINT				m_uPrivateFmt;

// Operations
protected:
	void	ChangeIniPathName();
	BOOL	CheckIniVersion();

// Overrides
public:
	virtual BOOL InitApplication();
	virtual BOOL InitInstance();

// Implementation
			BOOL FirstInstance();
			BOOL IsFileExist(LPCTSTR lpszPathName);

	//{{AFX_MSG(CProgrptApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern	CProgrptApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
