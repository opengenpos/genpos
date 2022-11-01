/////////////////////////////////////////////////////////////////////////////
// dlgfield.h : header file
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
// CFieldDlg dialog

#define	CFIELDDLG_ACCEPT_NOT	0
#define	CFIELDDLG_ACCEPT_FIELD	1
#define	CFIELDDLG_ACCEPT_ITEM	2

class CFieldDlg : public CDialog
{
// Construction
public:
	CFieldDlg(CWnd* pParent = NULL);	// standard constructor
	CFieldDlg(CView* pView);			// modeless constructor

// Dialog Data
	//{{AFX_DATA(CFieldDlg)
	enum { IDD = IDD_FILED_FORM };
	CButton	m_BtTop;
	CListBox	m_LbItem;
	CListBox	m_LbField;
	CListBox	m_LbGroup;
	int		m_nLoop;
	//}}AFX_DATA

// Attributes
public:
	int	        m_nOperation;
	int			m_nGroup;
	int			m_nField;
	int			m_nItem;
protected:
	int			m_nFile;
	CView*		m_pView;
	BOOL 		m_bShow;
	BOOL		m_bClose;

// Operations
public:
	static void CreateFieldDialog();
		   void CloseDialog();
	static void DestroyDialog();
	void		ShowDialog(BOOL bState, CView* pView, int nFile);
	void		ActiveDialog(BOOL bState, CView* pView, int nFile);
protected:
	BOOL		Create();
	void		UpdateView(CView* pView, int nFile);
	void		RefreshGroup(int nOperation);
	void		RefreshField();
	void		RefreshItem();
	int			GetFileIndex();
	int			GetOperation();
	void		SendInsertMessage();
	void		DisableInsertButton();

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFieldDlg)
	afx_msg void OnFieldClose();
	afx_msg void OnFieldInsert();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeFieldGroup();
	afx_msg void OnFieldLoop();
	afx_msg void OnFieldPrint();
	afx_msg void OnFieldMathe();
	afx_msg void OnSelchangeFieldField();
	afx_msg void OnSelchangeFieldItem();
	afx_msg void OnDblclkFieldField();
	afx_msg void OnDblclkFieldItem();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CFieldDlg* m_pField;				// the one and only CFieldDlg object
