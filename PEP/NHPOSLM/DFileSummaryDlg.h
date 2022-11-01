#if !defined(AFX_DFILESUMMARYDLG_H__02BCBF11_C434_49AC_89B3_94081D2F4A08__INCLUDED_)
#define AFX_DFILESUMMARYDLG_H__02BCBF11_C434_49AC_89B3_94081D2F4A08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFileSummaryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFileSummaryDlg dialog

class CDFileSummaryDlg : public CDialog
{
// Construction
public:
	CDFileSummaryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFileSummaryDlg)
	enum { IDD = IDD_DLG_FILESUMMARY };
	CStatic	m_Dot2;
	CStatic	m_Dot;
	CListBox	m_listSummary;
	CListBox	m_listHistory;
	UINT	m_nMjVersion;
	UINT	m_nMnVersion;
	UINT	m_nRelease;
	CString	m_editSummary;
	CString	m_csPEPFile;
	CString	m_csIconDir;
	//}}AFX_DATA

	CWindowDocumentExt  *pDoc;
	CString csDialogTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFileSummaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFileSummaryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetIconDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFILESUMMARYDLG_H__02BCBF11_C434_49AC_89B3_94081D2F4A08__INCLUDED_)
