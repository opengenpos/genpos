#if !defined(AFX_FRAMEWORKWNDITEM_H__76B3B2F8_D890_4223_938B_32994CFC64AA__INCLUDED_)
#define AFX_FRAMEWORKWNDITEM_H__76B3B2F8_D890_4223_938B_32994CFC64AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameworkWndItem.h : header file
//
#include "WindowItem.h"
#include "FrameworkWndButton.h"
#include "CWebBrowser2.h"
#include "ScrollReport.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndItem window

class CFrameworkWndItem : public CWindowItem
{
// Construction
public:
	CFrameworkWndItem();

//Destruction
	virtual ~CFrameworkWndItem();

// Attributes
public:

	HBRUSH m_faceBrush;//The brush for the background also used for button background

// Operations
public:
	CFrameworkWndItem (int id, int row = 0, int column = 0, int width = 12, int height = 12);
	CFrameworkWndItem(CWindowControl *wc);

	BOOL WindowDisplay (CDC* pDC);
	BOOL WindowRefreshOEPGroup ();
	void ControlSerialize (CArchive &ar);
	void CheckLastPick(void);
	BOOL PopupWindow (CWindowControl *pParent);
	BOOL PopdownWindow ();
	BOOL PopupSupervisorWindow (CWindowControl *pParent);
	virtual CWindowControl * SearchForItemByName (TCHAR *tcWindowName);

	void CreatePopupButton(CFrameworkWndButton * pButton);
	void RemovePopupButton(CFrameworkWndButton * pButton = 0);

	static CFrameworkWndItem * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == WindowContainer)
			return (static_cast <CFrameworkWndItem *> (pwc));
		else
			return NULL;
	}

	int m_nPickCount;
	BOOL  m_bInsideSuperIntrv;
	CWebBrowser2      *m_pWebBrowser;
	CScrollReportWnd  *m_pScrollReport;
	CScrollDocument   *m_pScrollDocument;
	// For a rollup window we are interested in the current state of the window
	// along with the current dimensions.
	int  m_nRollupState;			// current state of the rollup window. UserDefaultWin_ROLLUP_WIN
	int  m_nRollupRow[2];			//top left grid of rollup window
	int  m_nRollupColumn[2];		//top left grid of rollup window
	int  m_nRollupHeight[2];		//window height (pixels)
	int  m_nRollupWidth[2];			//window width (pixels)


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameworkWndItem)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameworkWndItem)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CFrameworkWndItemAdHoc : public CFrameworkWndItem
{
// Construction
public:
	CFrameworkWndItemAdHoc();
	CFrameworkWndItemAdHoc (int id, int row = 0, int column = 0, int width = 12, int height = 12);
	CFrameworkWndItemAdHoc(CWindowControl *wc);

//Destruction
	virtual ~CFrameworkWndItemAdHoc();

	static CFrameworkWndItemAdHoc * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == WindowContainerAdHoc)
			return (static_cast <CFrameworkWndItemAdHoc *> (pwc));
		else
			return NULL;
	}

// Attributes
public:
	CFrameworkWndItem *m_pChainNext;
	CWindowControl    *m_pChainNextParent;

protected:
	//{{AFX_MSG(CFrameworkWndItemAdHoc)
	//}}AFX_MSG
	afx_msg LRESULT OnEvsCreate(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEWORKWNDITEM_H__76B3B2F8_D890_4223_938B_32994CFC64AA__INCLUDED_)

