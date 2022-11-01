#if !defined(AFX_SENDOPMESSAGEDIALOG_H__BCAA4D8E_C531_4DD5_B49A_019E72916EC3__INCLUDED_)
#define AFX_SENDOPMESSAGEDIALOG_H__BCAA4D8E_C531_4DD5_B49A_019E72916EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendOpMessageDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendOpMessageDialog dialog

class CSendOpMessageDialog : public CDialog
{
// Construction
public:
	
	CSendOpMessageDialog(CWnd* pParent = NULL);   // standard constructor
	WCHAR  myTextBuffer[512];
	
// Dialog Data
	//{{AFX_DATA(CSendOpMessageDialog)
	enum { IDD = IDD_SEND_OP_MSG };
	CString	m_csMessageToSend;
	BOOL	m_bNoBroadcast;
	BOOL	m_bDeleteMessages;
	BOOL	m_bForceSignOut;
	BOOL	m_bAutoSignOut;
	BOOL	m_bBlockOperatorSignIn;
	BOOL	m_bUnBlockOperatorSignIn;
	BOOL	m_bForceShutDown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendOpMessageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendOpMessageDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDOPMESSAGEDIALOG_H__BCAA4D8E_C531_4DD5_B49A_019E72916EC3__INCLUDED_)
