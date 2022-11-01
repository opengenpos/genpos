/////////////////////////////////////////////////////////////////////////////
// dlgimp.h : header file
//
// Copyright (C) 1996-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Oct-12-98           O.Nakada     HPUS V3.3
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CImportDlg dialog

class CImportDlg : public CDialog
{
// Construction
public:
//	CImportDlg(CWnd* pParent = NULL);	// standard constructor
	CImportDlg(const CString& strPathName, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CImportDlg)
	enum { IDD = IDD_IMPORT_FORM };
	CButton	m_btnImport;
	CListBox	m_LbList;
	//}}AFX_DATA

// Attributes
public:
	int			m_nReport;				// index of report
protected:
	CString		m_strPathName;
	int			m_anFile[REPORT_MAX_SHEET];

// Implementation
protected:
	CString		GetReportInfo(int nNumber, int* pnFile);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeImportList();
	afx_msg void OnImportImport();
	virtual void OnOK();
	afx_msg void OnDblclkImportList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
