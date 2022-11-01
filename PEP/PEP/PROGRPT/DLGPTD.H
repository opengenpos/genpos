/////////////////////////////////////////////////////////////////////////////
// dlgptd.h : header file
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

/////////////////////////////////////////////////////////////////////////////
// CPtdDlg dialog

class CPtdDlg : public CDialog
{
// Construction
public:
//	CPtdDlg(CWnd* pParent = NULL);	// standard constructor
	CPtdDlg(CProgrptDoc* pDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPtdDlg)
	enum { IDD = IDD_PTD_FORM };
	CStatic	m_Title;
	CStatic	m_State;
	CStatic	m_No;
	CStatic	m_File;
	CStatic	m_Device;
	CButton	m_btnUp;
	CListBox	m_LbObject;
	CListBox	m_LbList;
	CButton	m_btnDown;
	int		m_nStd;
	//}}AFX_DATA

// Attributes
protected:
	CProgrptDoc*	m_pDoc;
	CPtrList		m_List;
	int				m_anUnique[REPORT_MAX_SHEET];

// Implementation
protected:
	void		RefreshList();
	void		RefreshObject();
	void		EnableControl(BOOL bState);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPtdDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPtdStd();
	afx_msg void OnSelchangePtdList();
	afx_msg void OnSelchangePtdObject();
	afx_msg void OnPtdUp();
	afx_msg void OnPtdDown();
	afx_msg void OnPtdOrg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
