/////////////////////////////////////////////////////////////////////////////
// mainfrm.h : interface of the CMainFrame class
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

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:
			void InitialShowWindow(UINT nCmdShow);
	static	BOOL ReadWindowPlacement(const wchar_t * pszEntry, LPWINDOWPLACEMENT pwp);
	static	void WriteWindowPlacement(const wchar_t * pszEntry,	LPWINDOWPLACEMENT pwp);
protected:

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
			long OnStatusMsg(UINT wParam, LONG lParam);
			long OnSetPathName(UINT wParam, LONG lParam);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnReportNew();
	afx_msg void OnSettingEod();
	afx_msg void OnSettingPtd();
	afx_msg void OnSetting16char();
	afx_msg void OnSetting40char();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnUpdateReportNew(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg	void OnUpdateFree();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
