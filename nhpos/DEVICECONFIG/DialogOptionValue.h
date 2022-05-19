#if !defined(AFX_DIALOGOPTIONVALUE_H__AC89DA5F_D915_4021_AC8D_102053034BE8__INCLUDED_)
#define AFX_DIALOGOPTIONVALUE_H__AC89DA5F_D915_4021_AC8D_102053034BE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogOptionValue.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogOptionValue dialog

class CDialogOptionValue : public CDialog
{
// Construction
public:
	CDialogOptionValue(CWnd* pParent = NULL);   // standard constructor

	CString myNewValue;

// Dialog Data
	//{{AFX_DATA(CDialogOptionValue)
	enum { IDD = IDD_OPTION_VALUE };
	CString	m_newValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOptionValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogOptionValue)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGOPTIONVALUE_H__AC89DA5F_D915_4021_AC8D_102053034BE8__INCLUDED_)
