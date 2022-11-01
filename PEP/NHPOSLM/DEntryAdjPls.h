#if !defined(AFX_DENTRYADJPLS_H__25EB915A_0BE7_473C_BCD7_5E01CCC65935__INCLUDED_)
#define AFX_DENTRYADJPLS_H__25EB915A_0BE7_473C_BCD7_5E01CCC65935__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DEntryAdjPls.h : header file
//

#include "P003.H"
#include "WindowButton.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CDEntry dialog

class CDEntry : public CDialog
{
// Construction
public:
	CDEntry(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;
	
// Dialog Data
	//{{AFX_DATA(CDEntry)
	enum { IDD = IDD_P03_ENTRY };
	CSpinButtonCtrl	m_sbcEntry;
	CString	m_csExtMsg;
	CString	m_csExtMnemonic;
	CString	m_csRange;
	UINT	m_uiExtFsc;
	CString m_csExtFsc;
	//}}AFX_DATA

	UINT myRangeMax;
	UINT myRangeMin;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEntry)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_cExtFsc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DENTRYADJPLS_H__25EB915A_0BE7_473C_BCD7_5E01CCC65935__INCLUDED_)
