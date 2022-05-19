#if !defined(AFX_OPOSTESTDLG_H__E84E39AB_F22C_4911_833B_D7981EAAB218__INCLUDED_)
#define AFX_OPOSTESTDLG_H__E84E39AB_F22C_4911_833B_D7981EAAB218__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OPOSTestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OPOSTestDlg dialog

class OPOSTestDlg : public CDialog
{
// Construction
public:
	OPOSTestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OPOSTestDlg)
	enum { IDD = IDD_DIALOG_OPOS_TEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OPOSTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OPOSTestDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPOSTESTDLG_H__E84E39AB_F22C_4911_833B_D7981EAAB218__INCLUDED_)
