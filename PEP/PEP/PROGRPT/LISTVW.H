/////////////////////////////////////////////////////////////////////////////
// listvw.h : interface of the CListView class
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

class CReport;

class CListView : public CFormView
{
protected: // create from serialization only
	CListView();
	DECLARE_DYNCREATE(CListView)

public:
	//{{AFX_DATA(CListView)
	enum { IDD = IDD_LIST_FORM };
	CButton	m_btnUp;
	CButton	m_btnOpen;
	CButton	m_btnNew;
	CListBox	m_lbList;
	CButton	m_btnDown;
	CButton	m_btnDelete;
	//}}AFX_DATA

// Attributes
public:
    CBrush	m_hBrush;
	CProgrptDoc* GetDocument();
	virtual void OnInitialUpdate();

// Operations
public:
	int		CreateNewReportAndFrame();
protected:

// Implementation
protected:
	BOOL	CreateNewReportFrame(CReport* pReport);
	int		ActivateReportFrame();
	void	DestroyAllReportFrames();
	void	UpDownControl();

public:
	virtual ~CListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActiveView,
								CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Generated message map functions
protected:
	//{{AFX_MSG(CListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnReportOpen();
	afx_msg void OnDblclkReportList();
	afx_msg void OnSelcancelReportList();
	afx_msg void OnSelchangeReportList();
	afx_msg void OnUpdateReportNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReportOpen(CCmdUI* pCmdUI);
	afx_msg void OnReportDelete();
	afx_msg void OnReportNew();
	afx_msg void OnClose();
	afx_msg void OnReportUp();
	afx_msg void OnReportDown();
	afx_msg void OnReportImport();
	afx_msg void OnUpdateReportImport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
//	friend class CProgrptApp;
};

#ifndef _DEBUG  // debug version in listvw.cpp
inline CProgrptDoc* CListView::GetDocument()
   { return (CProgrptDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
