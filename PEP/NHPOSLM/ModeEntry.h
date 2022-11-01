#if !defined(AFX_MODEENTRY_H__844EE646_64C9_46CE_AE01_3B8FBA9D5CEB__INCLUDED_)
#define AFX_MODEENTRY_H__844EE646_64C9_46CE_AE01_3B8FBA9D5CEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModeEntry.h : header file
//

#include "WindowButton.h"

/////////////////////////////////////////////////////////////////////////////
// CModeEntry dialog

class CModeEntry : public CDialog
{
// Construction
public:
	CModeEntry(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;

	// Dialog Data
	//{{AFX_DATA(CModeEntry)
	enum { IDD = IDD_PO3_MODEENTRY };
	CString	m_csExtMsg;
	CString	m_csExtMnemonic;
	int		m_iLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModeEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModeEntry)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEENTRY_H__844EE646_64C9_46CE_AE01_3B8FBA9D5CEB__INCLUDED_)
