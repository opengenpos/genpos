#if !defined(AFX_KEYPADENTRY_H__F98BA679_5282_492C_BB97_4FD3733CDFB2__INCLUDED_)
#define AFX_KEYPADENTRY_H__F98BA679_5282_492C_BB97_4FD3733CDFB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeypadEntry.h : header file
//
#include "WindowButton.h"

/////////////////////////////////////////////////////////////////////////////
// CKeypadEntry dialog

class CKeypadEntry : public CDialog
{
// Construction
public:
	CKeypadEntry(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;

// Dialog Data
	//{{AFX_DATA(CKeypadEntry)
	enum { IDD = IDD_KEYPADENTRY };
	CString	m_csExtMsg;
	CString	m_csExtMnemonic;
	int		m_iLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeypadEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeypadEntry)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYPADENTRY_H__F98BA679_5282_492C_BB97_4FD3733CDFB2__INCLUDED_)
