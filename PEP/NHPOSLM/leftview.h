#if !defined(AFX_LEFTVIEW_H__0A5B7E2B_0B23_43CD_8D20_AE5FBE2F7B11__INCLUDED_)
#define AFX_LEFTVIEW_H__0A5B7E2B_0B23_43CD_8D20_AE5FBE2F7B11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CLeftView.h : header file
//
#include "NewLayoutView.h"
/////////////////////////////////////////////////////////////////////////////
// CLeftView view

class CLeftView : public CTreeView
{
protected:
	CLeftView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CWindowDocumentExt* GetDocument();
	void SetTreeItem(CWindowItem *wi, HTREEITEM ti);
	void SetControlInfo(CWindowControl *wi, HTREEITEM ti);
	CNewLayoutView * m_LayoutView;
// Operations
public:
	void UpdateLabelControlTree(CWindowLabelExt* pl);
	void UpdateTextControlTree(CWindowTextExt* pb);
	void UpdateButtonControlTree(CWindowButtonExt* pwc);
	void DeleteControlTreeItem(UINT id);
	void InsertNewTextControl(UINT id, CWindowControl* pwc);
	void SetControlTextItem(CWindowItem *wi, HTREEITEM ti);
	void UpdateTreeLabel(CWindowItemExt *pwc);
	HTREEITEM GetTreeItem(UINT id, HTREEITEM hCurrent);
	void DeleteTreeItem(UINT id);
	void HideWindowItem(UINT id, CWindowItemExt* wi);
	HTREEITEM InsertChildItem(UINT id, HTREEITEM hCurrent);
	void InsertNewWinItem(UINT id, CWindowControl* pwc);
	void ShowSelectedWindow(UINT id);
	BOOL ShowSelectedSubWindow(CWindowItemExt *wi,UINT id);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnSelectionChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG  // debug version in LeftView.cpp
inline CWindowDocumentExt* CLeftView::GetDocument()
   { return (CWindowDocumentExt*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__0A5B7E2B_0B23_43CD_8D20_AE5FBE2F7B11__INCLUDED_)
