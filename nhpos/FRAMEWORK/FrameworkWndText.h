#if !defined(AFX_FRAMEWORKWNDTEXT_H__B27BB1BA_C98F_4DD0_8F29_4B46DC839EF4__INCLUDED_)
#define AFX_FRAMEWORKWNDTEXT_H__B27BB1BA_C98F_4DD0_8F29_4B46DC839EF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameworkWndText.h : header file
//
#include "WindowText.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndText window

class CFrameworkWndText : public CWindowText
{
// Construction
public:
	CFrameworkWndText(int id, int row, int column, int width, int height);
	CFrameworkWndText(CWindowControl *wc);
	virtual ~CFrameworkWndText();

	static int EnumDisplayMonitors (void);
	static int CheckArenaOepTable (SHORT nIdentifier);

// Attributes
	struct ArenaOepTable {
		ULONG         ulMap;
		UCHAR         tableno;
		UCHAR         groupno;
		UCHAR         deptno;
		UCHAR         stickydisplay;
		UCHAR         overlayimage;
		UCHAR         resizetofit;
		UCHAR         targetwindow;
		USHORT        vertalign;
		USHORT        horizalign;
		ULONG         background;
		CString       imagename;
		CString       backimage;
		CString       operatortext;
	};

	struct ArenaOepResult {
		int            iIndex;     // index of arena item.
		ArenaOepTable  &x;         // reference to arena item.
	};

public:
	static RECT  m_myMonitors[5];
	static int   m_myMonitorsIndex;
	static CList <ArenaOepTable>  m_ArenaOepTableList;

	UCHAR            m_aszCurrentOEPGroup;   // current group number being processed
	UCHAR            m_aszCurrentOEPTable;   // current table number being processed
	ArenaOepTable    m_CurrentArenaEntry;    // do not depend on text pointed to by filePath to be correct
	static CStatic   m_OEPCustDisplayImage;      // used to display an image. LCDWIN_ID_REG104, CWindowTextType::TypeOEPCustDisplay

// Operations
public:
	struct textPiece {
		wchar_t  *p1;
		wchar_t  *p2;
		int      iVal;
		ULONG    ulVal;
	};

	static  textPiece test (wchar_t *pLine, ArenaOepTable & x);
	static  int BuildArenaOepTableList (void);
	static  ArenaOepResult SearchArenaOepTableList (int iGroupNo, int iTableNo=0, int iDeptNo=0);
	static  ArenaOepTable & SearchArenaOepTableListByDept (int iDeptNo, int iGroupNo = 0, int iTableNo = 0);
	static  CFrameworkWndText *GetOepCustDisplayFrameworkWndText (int nFrame = 0) { return (CFrameworkWndText *)m_OEPCustDisplayImage.GetParent(); }

	//{{AFX_MSG(CFrameworkWndText)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnEvsCreate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEvsDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEvsUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEvsCursor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEvsGetParam(WPARAM wParam, LPARAM lParam); /* used to get button dimensions from Framework - CSMALL */
	afx_msg LRESULT OnOepImageryChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEvsPutParam(WPARAM wParam, LPARAM lParam); /* used to get button dimensions from Framework - CSMALL */
	DECLARE_MESSAGE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameworkWndText)
	//}}AFX_VIRTUAL

// My Implementation
protected:
	BOOL		PreCreateWindow(CREATESTRUCT& cs);
	void		ClearBuffer();
	void		DisplayData(CPaintDC& dc);
	void		DisplayUpperCursor(CPaintDC& dc, LONG nRow, LONG nColumn, TCHAR uchAttr);
	void		DisplayDownCursor(CPaintDC& dc, LONG nRow, LONG nColumn, TCHAR uchAttr);
	void		DisplayPopup(int nRow, TCHAR *tszTextLine);
	void		DisplayPopupControlsBack (BOOL bShouldExist);
	void		DisplayPopupControlsDone (BOOL bShouldExist);
	void		DisplayPopupControlsMore (BOOL bShouldExist);
	void		DeletePopup();
	void		ClearAndShowPopup();
	int 		ClearAndShowPopupWithImage();

	void		GetTextRegion(LONG nRow, LONG nColumn, LONG nChars, RECT* pRect);
	DWORD		GetTextColor(LONG nRow, TCHAR chAttribute);
	DWORD		GetBackColor(LONG nRow, TCHAR chAttribute);
	LONG		GetMarginLeft()    { return 2; }
	LONG		GetMarginRight()   { return 2; }
	LONG		GetMarginTop()     { return 1; }
	LONG		GetMarginButtom()  { return 1; }
	LONG		GetCMarginLeft()   { return 2; }
	LONG		GetCMarginRight()  { return 2; }
	LONG		GetCMarginTop()    { return 2; }
	LONG		GetCMarginButtom() { return 2; }
	TCHAR		Replace850(TCHAR uchSrc);
	void		SelectItem(UINT nFlags, CPoint point);

protected:
	CString		m_sName;					// container name
	SHORT		m_nIdentifier;				// container identifier
//	LONG		m_nLeft;					// left side of the control
//	LONG		m_nTop;						// top side of the control
	//LONG		m_nWidth;					// width of the control
	//LONG		m_nHeight;					// height of the control
	LONG		m_nRowsInCtl;				// rows of text shown in the control
	LONG		m_nColumnsInCtl;			// column of text shown in the control
	LONG		m_nFontWidth;				// width of font
	LONG		m_nFontHeight;				// height of font
	BOOL		m_bLCursor;					// cursor state of latest
	BOOL		m_bWCursor;					// cursor state of work
	POINT		m_ptLCursor;				// cursor position of latest
	POINT		m_ptWCursor;				// cursor position of work
	TCHAR*		m_pchText;					// text data for window (maybe UNICODE)
	TCHAR*		m_pchAttr;					// attribute data for window
	TCHAR*		m_pchLatest;				// latest data from event subsystem (MCBS)
	TCHAR*		m_pchWork;					// event subsystem interface (MBCS)
	TCHAR		m_popButtonText[50][27 + 1];	// Text for popup Buttons
	COLORREF	m_colorBG;					// background color
	COLORREF	m_colorText;				// text color
	RECT		m_rectClient;				// region of client window
	CFont		m_Font;						// font object
	CBrush		m_Brush;					// brush object
	//unsigned long m_nType;					// control type identifier
	//int			m_Action;					// action of this control
	USHORT		usHandle;					//Window Handle for VosCreateWindow()
	UINT		m_nTimer;					// timer object
	int			m_nMinute;					// minute for clock
	LONG		m_lTopMargin;				//the top margin for the window
	bool		m_bBackOEP;					//Go Back Button for OEP
	int         m_nButtonCount;             // number of buttons on this window
	// Generated message map functions
protected:

};

//---------------------------------------------------------------------------


class CFrameworkWndTextSubWindow
{
public:
	CFrameworkWndTextSubWindow (CFrameworkWndText  *pWndText = 0, int idWindow = 0, int idSubWindow = 0);
	~CFrameworkWndTextSubWindow ();
	bool DoesMatchGroupNumber (int nGroupNo);
	void SetOepString (TCHAR *tcsOepString = 0);
	void CompileOepString (TCHAR *tcsOepString = 0);
	SHORT  EvsConnectSubWindow ();
	SHORT  EvsDisconnectSubWindow ();

	static CFrameworkWndTextSubWindow *SearchForSubWindow (int nIdentifier, int nGroupNo);
	static CFrameworkWndTextSubWindow *SearchForSubWindow (HWND hWnd);
	static CFrameworkWndTextSubWindow *GetNextSubWindowWithIdentifier (int nIdentifier, int &nEnumeration);
	static int ExistWithIdentifier (int nIdentifier);
	static int ExistWithIdentifierConnected (int nIdentifier);
	static int RemoveSubWindow (HWND hWnd);
	static int RemoveSubWindow (CFrameworkWndText  *pWndText);
	static void InitializeSubWindowSystem ();
	static void SetLastCurrentFrameworkWndTextWindow (CFrameworkWndText  *meThis);
	static CFrameworkWndText  *FetchLastCurrentFrameworkWndTextWindow ();
	short  GetGroupNoFirst ();
	short  GetGroupNoNext ();

public:
	CFrameworkWndText  *myWndText;

protected:
	int   bIsEvsConnected;
	int   mySubWindowId;
	int   myWindowId;
	int   myGroupIndex;
	UCHAR eachOEPGroupNum[256];    // see SLOEP_BEGIN_SPL and defines in sloep.c
	TCHAR mOEPGroupNumbers[30];

	static CFrameworkWndTextSubWindow * SubWindowArray[40];
	static int SubWindowArrayIndex;
	static int CurrentSubWindowArrayIndex;
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEWORKWNDTEXT_H__B27BB1BA_C98F_4DD0_8F29_4B46DC839EF4__INCLUDED_)
