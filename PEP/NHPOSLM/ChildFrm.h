// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__376BAAC1_C6B7_4743_8522_624F9F377DC0__INCLUDED_)
#define AFX_CHILDFRM_H__376BAAC1_C6B7_4743_8522_624F9F377DC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "LOMSplitterWnd.h"


class CNewLayoutView;
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:


// Operations
public:
	void GetViews();
	CNewLayoutView* GetRightPane();
    void UpdateRulersInfo(stRULER_INFO stRulerInfo);
	int ResLoWidth;		//indicates the width in pixels
	int ResLoHeight;	//indicates the height in pixels
	BOOL m_bShowRulers;
	WINDOWPLACEMENT wp;		//variable to hold info about window during undo/redo so we can keep old window state
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ChangeMenu();
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:

	CLOMSplitterWnd *m_wndSplitter1;
	CLOMSplitterWnd *m_wndSplitter2;
	CLOMSplitterWnd *m_wndSplitter3;
	CLOMSplitterWnd *m_wndSplitter4;

	//{{AFX_MSG(CChildFrame)
	afx_msg void OnViewRulers();
	afx_msg void OnUpdateViewRulers(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__376BAAC1_C6B7_4743_8522_624F9F377DC0__INCLUDED_)

