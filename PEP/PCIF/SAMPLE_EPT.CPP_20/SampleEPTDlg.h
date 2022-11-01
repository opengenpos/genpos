// SampleEPTDlg.h : header file
//

#if !defined(AFX_SAMPLEEPTDLG_H__058AFB79_DDE3_4B56_AFF4_70968E7DA860__INCLUDED_)
#define AFX_SAMPLEEPTDLG_H__058AFB79_DDE3_4B56_AFF4_70968E7DA860__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ServerThread.h"


/////////////////////////////////////////////////////////////////////////////
// CSampleEPTDlg dialog

class CSampleEPTDlg : public CDialog
{
// Construction
public:
	CSampleEPTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSampleEPTDlg)
	enum { IDD = IDD_SAMPLEEPT_DIALOG };
	CString	m_csStaticHostName;
	CString	m_csStaticPortNo;
	CString	m_csPortNo;
	CString	m_csStaticHostAddr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleEPTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	ServerThread  myServerThread;

	// Generated message map functions
	//{{AFX_MSG(CSampleEPTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEEPTDLG_H__058AFB79_DDE3_4B56_AFF4_70968E7DA860__INCLUDED_)
