/////////////////////////////////////////////////////////////////////////////
// dlgprtd.h : header file
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

class CInfoPrint;

/////////////////////////////////////////////////////////////////////////////
// CPrintDetailDlg dialog

#define	PDETAIL_NUM_INDEX	50

class CPrintDetailDlg : public CDialog
{
// Construction
public:
	CPrintDetailDlg(CWnd* pParent = NULL);	// standard constructor
	CPrintDetailDlg(int nFile, CPrintItem* pItem, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPrintDetailDlg)
	enum { IDD = IDD_PRINT_DETAIL };
	CButton	m_Cond2;
	CButton	m_Cond1;
	CStatic	m_Minor2;
	CStatic	m_Minor1;
	CStatic	m_Major2;
	CStatic	m_Major1;
	CStatic	m_Item2;
	CStatic	m_Item1;
	CStatic	m_Group2;
	CStatic	m_Group1;
	CComboBox	m_CbMinor2;
	CComboBox	m_CbMinor1;
	CComboBox	m_CbMajor2;
	CComboBox	m_CbMajor1;
	CComboBox	m_CbItem2;
	CComboBox	m_CbGroup2;
	CComboBox	m_CbGroup1;
	CComboBox	m_CbItem1;
	CComboBox	m_CbCond;
	//}}AFX_DATA

// Attributes
protected:
	int			m_nFile;
	CPrintItem*	m_pItem;
	BOOL		m_bCond1;
	int			m_nGroup1;
	int			m_nMajor1;
	BOOL		m_bCond2;
	int			m_nGroup2;
	int			m_nMajor2;
	int			m_nIndex[2][PDETAIL_NUM_INDEX];

// Implementation
protected:
	void		Init();
	void		SetInitData();
	void		RefreshMajor(BOOL bInit, int nLevel);
	void		RefreshMinor(BOOL bInit, int nLevel);
	BOOL		GetCondLevel(int nLevel);
	INFO_COND*	GetCond();
	BOOL		UpdateState(int nLevel);
	CInfoPrint* GetInfo(int nLevel, int nGroup, int nMajor);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrintDetailDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangePrintdCond();
	afx_msg void OnSelchangePrintdGroup1();
	afx_msg void OnSelchangePrintdGroup2();
	afx_msg void OnSelchangePrintdMajor1();
	afx_msg void OnSelchangePrintdMajor2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
