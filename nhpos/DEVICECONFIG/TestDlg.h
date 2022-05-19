#if !defined(AFX_TESTDLG_H__C62BF5E6_F6D5_456B_98B9_472BB20F7D75__INCLUDED_)
#define AFX_TESTDLG_H__C62BF5E6_F6D5_456B_98B9_472BB20F7D75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TestDlg dialog

class TestDlg : public CDialog
{
// Construction
public:
	TestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TestDlg)
	enum { IDD = IDD_DIALOG_TEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString    m_csHostName;

	// Generated message map functions
	//{{AFX_MSG(TestDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPingTest();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__C62BF5E6_F6D5_456B_98B9_472BB20F7D75__INCLUDED_)
