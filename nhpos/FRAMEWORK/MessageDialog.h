#if !defined(AFX_MESSAGEDIALOG_H__27E94E61_4837_406B_9F2B_2BC5B0FBC3A7__INCLUDED_)
#define AFX_MESSAGEDIALOG_H__27E94E61_4837_406B_9F2B_2BC5B0FBC3A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageDialog dialog

class CMessageDialog : public CDialog
{
// Construction
public:
	CMessageDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL CreateMe (CWnd * pParentWnd = 0);

	BOOL SetTextAndShow (TCHAR *lParam);

	BOOL UnshowAndRemove ();


// Dialog Data
	//{{AFX_DATA(CMessageDialog)
	enum { IDD = IDD_DIALOG_MESSAGE };
	CEdit	m_DisplayText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEDIALOG_H__27E94E61_4837_406B_9F2B_2BC5B0FBC3A7__INCLUDED_)
