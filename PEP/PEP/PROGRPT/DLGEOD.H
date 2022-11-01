/////////////////////////////////////////////////////////////////////////////
// dlgeod.h : header file
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

class CProgrptDoc;

/////////////////////////////////////////////////////////////////////////////
// CEodDlg dialog

class CEodDlg : public CDialog
{
// Construction
public:
//	CEodDlg(CWnd* pParent = NULL);	// standard constructor
	CEodDlg(CProgrptDoc* pDoc, CWnd* pParent = NULL);
 
// Dialog Data
	//{{AFX_DATA(CEodDlg)
	enum { IDD = IDD_EOD_FORM };
	CStatic	m_Title;
	CStatic	m_State;
	CStatic	m_No;
	CStatic	m_File;
	CStatic	m_Device;
	CListBox	m_LbList;
	CButton	m_btnDown;
	CButton	m_btnUp;
	CListBox	m_LbObject;
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
	//{{AFX_MSG(CEodDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEodStd();
	afx_msg void OnEodOrg();
	afx_msg void OnSelchangeEodObject();
	virtual void OnCancel();
	afx_msg void OnEodUp();
	afx_msg void OnEodDown();
	afx_msg void OnSelchangeEodList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
