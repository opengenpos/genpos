/////////////////////////////////////////////////////////////////////////////
// dlgmathe.h : header file
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

class CMatheItem;

/////////////////////////////////////////////////////////////////////////////
// CMathePropDlg dialog

#define	MATHE_NUM_INDEX	50

class CMathePropDlg : public CDialog
{
// Construction
public:
	CMathePropDlg(CWnd* pParent = NULL);	// standard constructor
	CMathePropDlg(int nFile, CMatheItem* pItem, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CMathePropDlg)
	enum { IDD = IDD_MATHE_PROP };
	CButton	m_Ope2;
	CStatic	m_Minor2;
	CStatic	m_Minor1;
	CStatic	m_Major2;
	CStatic	m_Major1;
	CStatic	m_Item2;
	CStatic	m_Item1;
	CStatic	m_Group2;
	CStatic	m_Group1;
	CEdit	m_editValue;
	CButton	m_btnUse;
	CComboBox	m_CbMinor2;
	CComboBox	m_CbMinor1;
	CComboBox	m_CbMajor2;
	CComboBox	m_CbMajor1;
	CComboBox	m_CbItem2;
	CComboBox	m_CbItem1;
	CComboBox	m_CbGroup2;
	CComboBox	m_CbGroup1;
	CComboBox	m_CbCond;
	CString	m_strGroup;
	CString	m_strItem;
	CString	m_strMajor;
	CString	m_strMinor;
	//}}AFX_DATA

// Attributes
protected:
	int			m_nFile;
	CMatheItem*	m_pItem;
	BOOL		m_bCond1;
	int			m_nGroup1;
	int			m_nMajor1;
	BOOL		m_bCond2;
	int			m_nGroup2;
	int			m_nMajor2;
	int			m_nIndex[2][MATHE_NUM_INDEX];

// Implementation
protected:
	void		Init();
	void		SetInitData();
	void		RefreshMajor(BOOL bInit, int nLevel);
	void		RefreshMinor(BOOL bInit, int nLevel);
	BOOL		GetCondLevel(int nLevel);
	BOOL		UpdateState(int nLevel);
	CInfoPrint* GetInfo(int nLevel, int nGroup, int nMajor);
	BOOL		GetValue(LONG* plData);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMathePropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeMatheCond();
	afx_msg void OnMatheUse();
	afx_msg void OnSelchangeMatheGroup1();
	afx_msg void OnSelchangeMatheGroup2();
	afx_msg void OnSelchangeMatheMajor1();
	afx_msg void OnSelchangeMatheMajor2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
