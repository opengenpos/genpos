/////////////////////////////////////////////////////////////////////////////
// reportvw.h : header file
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
// CReportView view

#define	RV_ACT_NONE		0
#define	RV_ACT_INSERT	1
#define	RV_ACT_MOVELINE	2
#define	RV_ACT_MOVEITEM	3
#define	RV_ACT_MOVERECT	4

class CReportView : public CScrollView
{
	DECLARE_DYNCREATE(CReportView)
protected:
	CReportView();			// protected constructor used by dynamic creation

// Attributes
public:
	CProgrptDoc* GetDocument();
protected:
	BOOL	m_bPrint;
	BOOL	m_bMathe;
	int		m_nAction;

// Operations
public:
	BOOL	GetPrintState() { return m_bPrint; }
	BOOL	GetMatheState() { return m_bMathe; }
protected:
	long	OnFieldDlg(UINT wParam, LONG lParam);

// Implementation
public:
	virtual ~CReportView();
protected:
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual	void OnInitialUpdate();		// first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActiveView,
								CView* pDeactiveView);

	// Generated message map functions
	//{{AFX_MSG(CReportView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnReportSettitle();
	afx_msg void OnReportInsertfield();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnViewPrint();
	afx_msg void OnViewMathe();
	afx_msg void OnUpdateViewPrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewMathe(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditInsertrow();
	afx_msg void OnUpdateEditInsertrow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReportInsertfield(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in reportvw.cpp
inline CProgrptDoc* CReportView::GetDocument()
   { return (CProgrptDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
