/////////////////////////////////////////////////////////////////////////////
// dlg40cm.h : header file
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
// C40CMDlg dialog

#define	C40CM_MAX		40

class C40CMDlg : public CDialog
{
// Construction
public:
//	C40CMDlg(CWnd* pParent = NULL);	// standard constructor
	C40CMDlg(CProgrptDoc* pDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(C40CMDlg)
	enum { IDD = IDD_40CHAR_MNEMO };
	CEdit	m_editMnemo;
	CListBox	m_LbList;
	//}}AFX_DATA

// Attributes
protected:
	CProgrptDoc*	m_pDoc;
	int				m_nSel;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(C40CMDlg)
	afx_msg void OnSelchange40cmList();
	afx_msg void OnKillfocus40cmMnemo();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
