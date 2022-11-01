/////////////////////////////////////////////////////////////////////////////
// dlgloop.h : header file
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
// CLoopPropDlg dialog

class CLoopPropDlg : public CDialog
{
// Construction
public:
//	CLoopPropDlg(CWnd* pParent = NULL);	// standard constructor
	CLoopPropDlg(int nFile, CLoopItem* pItem, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CLoopPropDlg)
	enum { IDD = IDD_LOOP_PROP };
	CComboBox	m_cbCond;
	CString	m_strGroup;
	CString	m_strMajor;
	//}}AFX_DATA

// Attributes
protected:
	int			m_nFile;
	CLoopItem*	m_pItem;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLoopPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeLoopCond();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
