/////////////////////////////////////////////////////////////////////////////
// listpdlg.h : header file
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
// CListPropDlg dialog

class CListPropDlg : public CDialog
{
// Construction
public:
//	CListPropDlg(CWnd* pParent = NULL);	// standard constructor
	CListPropDlg(CProgrptDoc* pDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CListPropDlg)
	enum { IDD = IDD_LIST_PROP };
	CComboBox	m_CbFile;
	CComboBox	m_CbDevice;
	CString	m_strTitle;
	//}}AFX_DATA

// Attributes
public:
	int				m_nFile;
	int				m_nDevice;
protected:
	CProgrptDoc*	m_pDoc;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CListPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
