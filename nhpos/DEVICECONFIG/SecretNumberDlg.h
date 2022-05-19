#if !defined(AFX_SECRETNUMBERDLG_H__BB067596_2C9C_4FB9_AF25_90836BA282FC__INCLUDED_)
#define AFX_SECRETNUMBERDLG_H__BB067596_2C9C_4FB9_AF25_90836BA282FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SecretNumberDlg.h : header file
//

#include "SPassWd.h"
/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Default Parameters
/////////////////////////////////////////////////////////////////////////////
*/
#define INIRST_SELECTION			1
#define INIRST_ENTERSECRETNO_ERR	0

/////////////////////////////////////////////////////////////////////////////
// CSecretNumberDlg dialog

class CSecretNumberDlg : public CDialog
{
// Construction
public:
	int CheckSecretNo(const CString strKeyCode);
public:
	CSecretNumberDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSecretNumberDlg)
	enum { IDD = IDD_DIALOG_SECRET_NUMBER };
	CEdit	m_editSecretNumber;
	CString		m_strSecretNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecretNumberDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSecretNumberDlg)
	afx_msg void ON_IDOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CnSaratogaPasswd	m_PassWd;
	void HandlePinPadEntry (const TCHAR  tcsChar);
public:
	BOOL				m_bPasswdStatus;
	afx_msg void OnBnClickedButton0();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECRETNUMBERDLG_H__BB067596_2C9C_4FB9_AF25_90836BA282FC__INCLUDED_)
