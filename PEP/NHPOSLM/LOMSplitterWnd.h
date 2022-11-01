#if !defined(AFX_LOMSPLITTERWND_H__B9165F94_6ED5_4456_B619_65E7294A6C89__INCLUDED_)
#define AFX_LOMSPLITTERWND_H__B9165F94_6ED5_4456_B619_65E7294A6C89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LOMSplitterWnd.h : header file
//



struct stRULER_INFO {
    UINT   uMessage;
    CPoint ScrollPos;
    CPoint Pos;
    CSize  DocSize;
    float  fZoomFactor;
};

class CLOMSplitterWnd : public CSplitterWnd  
{
public:
public:
	CLOMSplitterWnd(int nLevel = 0);
	virtual ~CLOMSplitterWnd();

	BOOL Create(CWnd* pParentWnd, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext , BOOL bVertical = TRUE, int nID = AFX_IDW_PANE_FIRST);


    CLOMSplitterWnd* m_pSubSplitterWnd[2]; // 0=Left/Top, 1=Right/Bottom

    CLOMSplitterWnd* AddSubDivision(int nSide, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext,BOOL bVertical);
    void ShowRulers(BOOL bShow = TRUE, BOOL bSave = TRUE);
    void UpdateRulersInfo(stRULER_INFO stRulerInfo, int type);

protected:   
	
	inline void SideToRowCol(int nSide, int* nRow, int*  nCol) 
	{
		if (m_bVertical) {
			*nRow = 0;
			*nCol = nSide;
		}
		else {
			*nRow = nSide;
			*nCol = 0;
		}
	};

	BOOL m_bVertical;
	int m_nLevel;
	BOOL m_bRulersVisible;

	// Generated message map functions
protected:
	//{{AFX_MSG(CLOMSplitterWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRulerView view

class CRulerView : public CView
{
protected:
	CRulerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRulerView)

// Attributes
private:
	UINT   m_rulerType;
	CPoint m_scrollPos;		// KSo What is this?
	CPoint m_lastPos;
    CSize  m_DocSize;
    float  m_fZoomFactor;

// Operations
public:
	void SetRulerType(UINT rulerType = RT_HORIZONTAL)
    {
        m_rulerType   = rulerType;
    }
    void UpdateRulersInfo(stRULER_INFO stRulerInfo);
    
private:
    void DrawTicker(CDC* pDC, CRect rulerRect, int nFactor, int nBegin = 0, BOOL bShowPos = TRUE, int nThickness = 1);
    void DrawCursorPos(CPoint NewPos);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulerView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRulerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRulerView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView view

class CRulerCornerView : public CView
{
protected:
	CRulerCornerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRulerCornerView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulerCornerView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRulerCornerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRulerCornerView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOMSPLITTERWND_H__B9165F94_6ED5_4456_B619_65E7294A6C89__INCLUDED_)
