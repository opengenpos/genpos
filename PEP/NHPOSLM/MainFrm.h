// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F4B1773A_3FB4_4B36_AF66_0F4EA42AD0A4__INCLUDED_)
#define AFX_MAINFRM_H__F4B1773A_3FB4_4B36_AF66_0F4EA42AD0A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagCodePageTable {
	TCHAR  *tcCountry;
	ULONG  ulIdsLanguage;
	UINT   unFontSize;
	UINT   unCodePage;
	WORD   wLanguage;
	WORD   wSubLanguage;
	UINT   uiCharSet;
	BYTE   ucQuality;
	BYTE   ucPitch;
} CodePageTable, *PCodePageTable;

extern CodePageTable myCodePageTable [];

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	friend CNewLayoutView;

// Attributes
public:
//-----------------------   Multi-lingual code

CFont resourceFont;
UINT uiCurrentCodePage;
HINSTANCE		hResourceDll;  
TCHAR szTitle[30];

enum selectedLanguage{chineseSimplified = 0, english, french, german, /*greek, italian, japanese, russian,*/ spanish};


// Operations
public:
	void GetLangFont(int iLiSel, CodePageTable  myCodePageTable[]);
	UINT setCodePage(UINT iCodePage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ProcessCmdLineLang(WORD lang, WORD sublang);
	void LoadLangDLL(UINT selection);
	//BOOL HandleKeyBoardChange (UINT message, WPARAM wParam, LPARAM lParam);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeLanguage();
	afx_msg	BOOL HandleKeyBoardChange (UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F4B1773A_3FB4_4B36_AF66_0F4EA42AD0A4__INCLUDED_)
