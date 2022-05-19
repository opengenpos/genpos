#if !defined(AFX_DIALOGINITIALRESET_H__9CC00F0D_D3FA_4115_B0F2_D7F734441E75__INCLUDED_)
#define AFX_DIALOGINITIALRESET_H__9CC00F0D_D3FA_4115_B0F2_D7F734441E75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogInitialReset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogInitialReset dialog
class CDialogInitialReset : public CDialog
{
// Construction
public:
	CDialogInitialReset(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogInitialReset)
	enum { IDD = IDD_DIALOG_INITIAL_RESET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogInitialReset)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int arrayInitialReset [10];

	// Generated message map functions
	//{{AFX_MSG(CDialogInitialReset)
	afx_msg void ON_IDC_INITRESET_CLEAR();
	afx_msg void ON_IDC_INITRESET_EXIT();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int	ExecReset(const UINT uKeyCode);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGINITIALRESET_H__9CC00F0D_D3FA_4115_B0F2_D7F734441E75__INCLUDED_)
