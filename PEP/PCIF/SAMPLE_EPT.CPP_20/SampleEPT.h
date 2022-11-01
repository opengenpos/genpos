// SampleEPT.h : main header file for the SAMPLEEPT application
//

#if !defined(AFX_SAMPLEEPT_H__117D70CF_92A3_4BBE_9509_3F7443AE72A4__INCLUDED_)
#define AFX_SAMPLEEPT_H__117D70CF_92A3_4BBE_9509_3F7443AE72A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSampleEPTApp:
// See SampleEPT.cpp for the implementation of this class
//

class CSampleEPTApp : public CWinApp
{
public:
	CSampleEPTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleEPTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSampleEPTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEEPT_H__117D70CF_92A3_4BBE_9509_3F7443AE72A4__INCLUDED_)
