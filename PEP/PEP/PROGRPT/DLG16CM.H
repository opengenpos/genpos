/////////////////////////////////////////////////////////////////////////////
// dlg16cm.h : header file
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
// C16CMDlg dialog

#define	C16CM_MAX		16

class C16CMDlg : public CDialog
{
// Construction
public:
//	C16CMDlg(CWnd* pParent = NULL);	// standard constructor
	C16CMDlg(CProgrptDoc* pDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(C16CMDlg)
	enum { IDD = IDD_16CHAR_MNEMO };
	CEdit	m_editMnemo;
	CListBox	m_LbList;
	//}}AFX_DATA

// Attributes
protected:
	CProgrptDoc*	m_pDoc;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(C16CMDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchange16cmList();
	afx_msg void OnKillfocus16cmMnemo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
