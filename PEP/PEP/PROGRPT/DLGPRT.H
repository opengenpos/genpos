/////////////////////////////////////////////////////////////////////////////
// dlgprt.h : header file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

class CPrintItem;

/////////////////////////////////////////////////////////////////////////////
// CPrintPropDlg dialog

class CPrintPropDlg : public CDialog
{
// Construction
public:
	CPrintPropDlg(CWnd* pParent = NULL);	// standard constructor
	CPrintPropDlg(int nFile, int nWidth, CPrintItem* pItem, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPrintPropDlg)
	enum { IDD = IDD_PRINT_PROP };
	CComboBox	m_CbFormat;
	int		m_nAlign;
	CString	m_strMajor;
	CString	m_strItem;
	BOOL	m_bWide;
	int		m_nWidth;
	int		m_nMinor;
	CString	m_strCond;
	CString	m_strGroup;
	//}}AFX_DATA

// Attributes
protected:
	int			m_nFile;
	int			m_nMaxWidth;
	CPrintItem*	m_pItem;

// Implementation
protected:
	const wchar_t *	GetCondition();
	BOOL		CheckWidth();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrintPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPrintDetail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
