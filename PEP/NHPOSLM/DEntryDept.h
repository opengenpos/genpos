#if !defined(AFX_DENTRYDEPT_H__8CD3A0EE_946A_486B_BCA9_F3245BEEEBC7__INCLUDED_)
#define AFX_DENTRYDEPT_H__8CD3A0EE_946A_486B_BCA9_F3245BEEEBC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DEntryDept.h : header file
//

#include "P003.H"
#include "WindowButtonExt.h"

/////////////////////////////////////////////////////////////////////////////
// CDEntryDept dialog

class CDEntryDept : public CDialog
{
// Construction
public:
	CDEntryDept(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;
// Dialog Data
	//{{AFX_DATA(CDEntryDept)
	enum { IDD = IDD_P03_DEPTENTRY };
	CSpinButtonCtrl	m_sbcExtFsc;
	CString	m_csExtMsg;
	CString	m_csExtMnemonic;
	CString	m_csExtFsc;
	//}}AFX_DATA

	int myRangeMax;
	int myRangeMin;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEntryDept)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEntryDept)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

class CDEntryGroupTable : public CDialog
{
// Construction
public:
	CDEntryGroupTable(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;
// Dialog Data
	//{{AFX_DATA(CDEntryDept)
	enum { IDD = IDD_DLG_OEP_TABLE };
	CSpinButtonCtrl	m_sbcExtFsc;
	CSpinButtonCtrl	m_sbcExtFsc2;
	CString	m_csExtMsg;
	CString	m_csExtMsg2;
	CString	m_csExtMnemonic;
	CString	m_csExtFsc;
	CString	m_csExtFsc2;
	USHORT  m_usOptionFlags;
	//}}AFX_DATA

	int m_myTableRangeMin;
	int m_myTableRangeMax;
	int m_myGroupRangeMin;
	int m_myGroupRangeMax;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEntryDept)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEntryDept)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedCheck20();
};

#endif // !defined(AFX_DENTRYDEPT_H__8CD3A0EE_946A_486B_BCA9_F3245BEEEBC7__INCLUDED_)
