// Device71xxPrinter.h : main header file for the DEVICE71XXPRINTER DLL
//

#if !defined(AFX_DEVICE71XXPRINTER_H__D69A6609_AD68_11D3_BB97_005004B8DABD__INCLUDED_)
#define AFX_DEVICE71XXPRINTER_H__D69A6609_AD68_11D3_BB97_005004B8DABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDevice71xxPrinterApp
// See Device71xxPrinter.cpp for the implementation of this class
//

class CDevice71xxPrinterApp : public CWinApp
{
public:
	CDevice71xxPrinterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevice71xxPrinterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDevice71xxPrinterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICE71XXPRINTER_H__D69A6609_AD68_11D3_BB97_005004B8DABD__INCLUDED_)
