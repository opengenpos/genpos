/*
-----------------------------------------------------------------------------------------------------------------------
 NHPOSLM
 Copyright (C) 2011 GSU
-----------------------------------------------------------------------------------------------------------------------
 Date	|	Changes Made																	| Version	|	By
-----------------------------------------------------------------------------------------------------------------------
 ??????		Original
-----------------------------------------------------------------------------------------------------------------------
*/
#if !defined __NEWLAYOUTVIEW_H__
#define __NEWLAYOUTVIEW_H__

#pragma once
#include "NewLayout.h"
#include "NewLayoutDoc.h"
#include "WindowDocumentExt.h"
#include "WindowItemExt.h"
#include "DTextItem.h"
#include "WindowControl.h"
#include "WindowButtonExt.h"
#include "DefaultData.h"
#include "WindowLabelExt.h"

enum SEARCHRC 
{
	SR_NoItemFound    = 0,
	SR_ActiveWindow   = 3,
	SR_ButtonOrText   = 4,
	SR_InactiveWindow = 6
};


class CNewLayoutView : public CScrollView 
{
protected: // create from serialization only
	CNewLayoutView();
	DECLARE_DYNCREATE(CNewLayoutView)

// Attributes
public:
	CWindowDocumentExt * GetDocument();

	static UINT uiGlobalID;
	CWindowTextExt *tempText;
	CWindowButtonExt *tempButton;
	CWindowItemExt *tempWindow;
	CWindowControl *dragControl;

	BOOL moveDragControl;

	UINT uiActiveID;
	BOOL m_ZoomToFit;
	BOOL resOne;
	BOOL resTwo;
	HMENU hMenu;
	int ScreenWidth;
	int ScreenHeight;
	int width;
	int height;
	int shiftCol;
	int shiftRow;

	POSITION pclList;
	POSITION pclListCurrent;
	
	CTreeView* m_TreeView;
	CPoint cpLastRButton;
	CPoint cpLastLButtonUp;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewLayoutView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnAddLabel();
	void OnAddEditBox();
	void OnDragWindow(int diffColumn, int diffRow, CWindowItemExt *wi);
	void ClearDocumentSelection();
	BOOL RemoveSubWin(CWindowItemExt * wi);
	UINT GetSubID(CWindowItemExt * wi);
	void ClearSelection(CWindowControl *wc);
	CWindowTextExt* CreateNewText(CWindowControl* wc,CWindowItemExt *pWin, int row, int column, int cnt);
	CWindowButtonExt* CreateNewButton(CWindowControl* wc,CWindowItemExt *pWin, int row, int column, int cnt);
	CWindowLabelExt* CreateNewLabel(CWindowControl* wc,CWindowItemExt *pWin, int row, int column, int cnt);
	CWindowItemExt * CreateNewWindow(CWindowControl* wc, int row, int column);
	void HighlightSelectionOLD(CDC *pDC, int row, int column);
	void HighlightSelection(CDC *pDC, int row, int column);
	CWindowControl* CopyControl(CWindowControl *wc,int row, int column);
	void PopupDefaultWinDlg(BOOL isNewWin, DefaultData *df);
	virtual ~CNewLayoutView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
	CPoint cpLastLButton;
	
	void PopupButtonEditDialog (int row, int column);
	void PopupListBoxEditDialog (int row, int column);
	void PopupAddGroupDialog (int row, int column);
	void PopAddTextWindow(int row, int column);
	void PopSetOEPWin(int row, int column);
	//void PasteWindow(int column, int row);
	void CreateTripleWindow(int row, int column, CWindowItemExt *wi,CDTextItem *ti, UINT activeID);
	void UpdateTripleWindow(int row, int column, CWindowItemExt *wi,CDTextItem *ti, UINT activeID, CWindowTextExt *wt);
	void UpdateRulersInfo(int nMessage, CPoint ScrollPos, CPoint Pos);

private:
	void _setPaneText(int statusBarIndex, LPCTSTR statusText);
	void _selectItem(CWindowItemExt *pItem);
	void _indicateSelectedItem(CWindowControl *pWinControl);

// Generated message map functions
protected:
	//{{AFX_MSG(CNewLayoutView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopSetwindow();
	afx_msg void OnPopSetSubWin();
	afx_msg void OnFileTest();
	afx_msg void OnPopupCopy();
	afx_msg void OnPopupCut();
	afx_msg void OnPopupPaste();
	afx_msg void OnViewResolution800();
	afx_msg void OnViewResolution1024();
	afx_msg void OnViewResolution1280();
	afx_msg void OnViewResolution1440();
	afx_msg void OnViewResolution1680();
	afx_msg void OnViewResolution1920();
	afx_msg void OnViewResolution2560();
	afx_msg void OnUpdateViewResolution800(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution1024(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution1280(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution1440(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution1680(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution1920(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResolution2560(CCmdUI* pCmdUI);
	afx_msg void OnChangeDefaults();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnToolbarWingroup();
	afx_msg void OnToolbarActiveWnd();
	afx_msg void OnToolbarBtn();
	afx_msg void OnToolbarOep();
	afx_msg void OnToolbarTxt();
	afx_msg void OnToolbarWnd();
	afx_msg void OnToolbarLbl();
	afx_msg void OnToolbarDelete();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


inline bool isCellOutside(int xCellPos, int yCellPos, int ResLoWidth, int ResLoHeight)
{
	bool res = (
		(xCellPos >= ((ResLoWidth  % STD_GRID_WIDTH)?(ResLoWidth  / STD_GRID_WIDTH+1):(ResLoWidth/STD_GRID_WIDTH) )) ||
		(yCellPos >= ((ResLoHeight % STD_GRID_WIDTH)?(ResLoHeight / STD_GRID_WIDTH+1):(ResLoHeight/STD_GRID_WIDTH) ))
		);
	return res;
}

inline bool isCellSelected(int xCellPos, int yCellPos, int xCurPos, int yCurPos)
{
	return (xCellPos == xCurPos && yCellPos == yCurPos);
}

#ifndef _DEBUG  // debug version in NewLayoutView.cpp
inline CWindowDocumentExt * CNewLayoutView::GetDocument()
   { return &(((CNewLayoutDoc*)m_pDocument)->myWindowDocument); }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__NEWLAYOUTVIEW_H__)

