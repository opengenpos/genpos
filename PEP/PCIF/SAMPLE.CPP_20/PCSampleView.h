/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PCSampleView.h
//
//  PURPOSE:    Main PCSample View class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCSAMPLEVIEW_H__70072F0C_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_PCSAMPLEVIEW_H__70072F0C_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CPCSampleView
//---------------------------------------------------------------------------

class CPCSampleView : public CScrollView
{
protected: // create from serialization only
	CPCSampleView();
	DECLARE_DYNCREATE(CPCSampleView)

// Attributes
public:
	CPCSampleDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCSampleView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPCSampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int    iButtonDown;
	CPoint pointButtonDown;
	CPoint pointMouseMove;
	CPoint pointButtonUp;
	CSize  sizeTotal;

// Generated message map functions
protected:
	//{{AFX_MSG(CPCSampleView)
	afx_msg void OnReadRegfin();
	afx_msg void OnReadRegfinPtd();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void CPCSampleView::OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void CPCSampleView::OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void CPCSampleView::OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateReadRegfin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReadRegfinPtd(CCmdUI* pCmdUI);
	afx_msg void OnReadIndfin();
	afx_msg void OnUpdateReadIndfin(CCmdUI* pCmdUI);
	afx_msg void OnReadServicetime();
	afx_msg void OnUpdateReadServicetime(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnReadEtk();
	afx_msg void OnUpdateReadEtk(CCmdUI* pCmdUI);
	afx_msg void OnResetCashier();
	afx_msg void OnUpdateResetCashier(CCmdUI* pCmdUI);
	afx_msg void OnResetServicetime();
	afx_msg void OnUpdateResetServicetime(CCmdUI* pCmdUI);
	afx_msg void OnResetIndfin();
	afx_msg void OnUpdateResetIndfin(CCmdUI* pCmdUI);
	afx_msg void OnResetGCByCashier();
	afx_msg void OnUpdateResetGCByCashier(CCmdUI* pCmdUI);
	afx_msg void OnResetGCByGCNo();
	afx_msg void OnUpdateResetGCByGCNo(CCmdUI* pCmdUI);
	afx_msg void OnWindowPrint();
	afx_msg void OnWindowPrintLastReport();
	afx_msg void OnWindowClear();
	afx_msg void OnUpdateWindowClear(CCmdUI* pCmdUI);
	afx_msg void OnToggleExportSylk();
	afx_msg void OnUpdateToggleExportSylk(CCmdUI* pCmdUI);
	afx_msg void OnReadCashier();
	afx_msg void OnUpdateReadCashier(CCmdUI* pCmdUI);
	afx_msg void OnReadGCAll();
	afx_msg void OnUpdateReadGCAll(CCmdUI* pCmdUI);
	afx_msg void OnReadGCByGCNo();
	afx_msg void OnUpdateReadGCByGCNo(CCmdUI* pCmdUI);
	afx_msg void OnReadGCByCashier();
	afx_msg void OnUpdateReadGCByCashier(CCmdUI* pCmdUI);
	afx_msg void OnResetOpenedGC();
	afx_msg void OnUpdateResetOpenedGC(CCmdUI* pCmdUI);
	afx_msg void OnReadHourly();
	afx_msg void OnUpdateReadHourly(CCmdUI* pCmdUI);
	afx_msg void OnReadDept();
	afx_msg void OnUpdateReadDept(CCmdUI* pCmdUI);
	afx_msg void OnReadPluByDept();
	afx_msg void OnUpdateReadPlu(CCmdUI* pCmdUI);
	afx_msg void OnReadPluEx();
	afx_msg void OnUpdateReadPluEx(CCmdUI* pCmdUI);
	afx_msg void OnReadCoupon();
	afx_msg void OnUpdateReadCoupon(CCmdUI* pCmdUI);
	afx_msg void OnResetPlu();
	afx_msg void OnUpdateResetPlu(CCmdUI* pCmdUI);
	afx_msg void OnResetCoupon();
	afx_msg void OnUpdateResetCoupon(CCmdUI* pCmdUI);
	afx_msg void OnResetETK();
	afx_msg void OnUpdateResetETK(CCmdUI* pCmdUI);
	afx_msg void OnParaUpdatePlu();
	afx_msg void OnBroadCastSupAc();
	afx_msg void OnBroadCastAllPAc();
	afx_msg void OnBroadCastPluOepAc();
	afx_msg void OnRetrieveSystemStatus();
	afx_msg void OnConnectEngine();
	afx_msg void OnSaveLogToFile();
	afx_msg void OnUpdateWindowConnectengine(CCmdUI *pCmdUI);
	afx_msg void OnProvCashier();
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg LRESULT OnWindowSetSection(WPARAM  wParam, LPARAM  lParam);
	afx_msg void OnDisplayReportList();
	afx_msg void OnScanForMaster();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CMainFrame* GetMyParent();
    VOID    SetScrollSize();
    VOID    EnableMenuItem( const CCmdUI* pCmdUI ) const;
    BOOL    ReadAndReset( CTtlIndFin& ttlIndFin,
                          const USHORT usTerminalNo,
                          const BOOL fIsEndOfDay );
    BOOL    ReadAndReset( CTtlCashier& ttlCashier,
                          CParaCashier& paraCashier );
    BOOL    ReadAndReset( CTtlPlu& ttlPlu,
                          const BOOL fIsEndOfDay );
    BOOL    ReadAndReset( CTtlCoupon& ttlCoupon,
                          const BOOL fIsEndOfDay );
    BOOL    ReadAllRecords( CGuestCheck& ttlGC,
                            const ULONG ulCashierNo,
                            const BOOL fIsResetOK = FALSE );
    VOID    ReadAllRecords( const UCHAR uchClassToRead,
                            CTtlETK& ttlETK,
                            const ULONG aulEmployeeID[],
                            const USHORT usNoOfEmployee );

    CSize   m_sizeCell;

	bool	m_bConnectEngineWindowShown;
	bool	m_bAlwaysOnTop;
	int     m_fontPointSize;

	afx_msg void OnWindowAlwaysontop();
	afx_msg void OnUpdateWindowAlwaysontop(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CPCSampleDoc::GetDocument()
//===========================================================================

#ifndef _DEBUG  // debug version in PCSampleView.cpp
inline CPCSampleDoc* CPCSampleView::GetDocument()
{
    return ( CPCSampleDoc* )m_pDocument;
}
#endif

//===========================================================================
//  FUNCTION :  CPCSampleDoc::GetMyParent()
//===========================================================================

inline CMainFrame* CPCSampleView::GetMyParent()
{
    CWnd* pMain;
    pMain = GetParent();
    return ( static_cast< CMainFrame* >( pMain ));
}

//---------------------------------------------------------------------------
//
//  MENU STATE UPDATE FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadRegfin()
//===========================================================================

inline void CPCSampleView::OnUpdateReadRegfin(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadRegfinPtd()
//===========================================================================

inline void CPCSampleView::OnUpdateReadRegfinPtd(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadCashier()
//===========================================================================

inline void CPCSampleView::OnUpdateReadCashier(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadIndfin()
//===========================================================================

inline void CPCSampleView::OnUpdateReadIndfin(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadServicetime()
//===========================================================================

inline void CPCSampleView::OnUpdateReadServicetime(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadHourly()
//===========================================================================

inline void CPCSampleView::OnUpdateReadHourly(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadEtk()
//===========================================================================

inline void CPCSampleView::OnUpdateReadEtk(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadGCByGCNo()
//===========================================================================

inline void CPCSampleView::OnUpdateReadGCAll(CCmdUI* pCmdUI)
{
	EnableMenuItem(pCmdUI);
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadGCByGCNo()
//===========================================================================

inline void CPCSampleView::OnUpdateReadGCByGCNo(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadGCByCashier()
//===========================================================================

inline void CPCSampleView::OnUpdateReadGCByCashier(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetCashier()
//===========================================================================

inline void CPCSampleView::OnUpdateResetCashier(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetServicetime()
//===========================================================================

inline void CPCSampleView::OnUpdateResetServicetime(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetIndfin()
//===========================================================================

inline void CPCSampleView::OnUpdateResetIndfin(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetETK()
//===========================================================================

inline void CPCSampleView::OnUpdateResetETK(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetGCByCashier()
//===========================================================================

inline void CPCSampleView::OnUpdateResetGCByCashier(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetGCByGCNo()
//===========================================================================

inline void CPCSampleView::OnUpdateResetGCByGCNo(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetOpenedGC()
//===========================================================================

inline void CPCSampleView::OnUpdateResetOpenedGC(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadDept()
//===========================================================================

inline void CPCSampleView::OnUpdateReadDept(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadPlu()
//===========================================================================

inline void CPCSampleView::OnUpdateReadPlu(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadPluEx()
//===========================================================================

inline void CPCSampleView::OnUpdateReadPluEx(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetPlu()
//===========================================================================

inline void CPCSampleView::OnUpdateResetPlu(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateReadCoupon()
//===========================================================================

inline void CPCSampleView::OnUpdateReadCoupon(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateResetCoupon()
//===========================================================================

inline void CPCSampleView::OnUpdateResetCoupon(CCmdUI* pCmdUI) 
{
    EnableMenuItem( pCmdUI );
}

//===========================================================================
//  FUNCTION :  CPCSampleView::OnUpdateWindowClear()
//===========================================================================

inline void CPCSampleView::OnUpdateWindowClear(CCmdUI* pCmdUI) 
{
    if ( 0 < GetDocument()->GetNoOfReportStrings())
    {
        pCmdUI->Enable( TRUE );
    }
    else
    {
        pCmdUI->Enable( FALSE );
    }
}

// handle the request from the MFC framework to update the displayed state this
// not only does a check mark against the menu item it also causes the toolbar
// icon to appear depressed if check is set or non-depressed if check is not set
inline void CPCSampleView::OnUpdateToggleExportSylk (CCmdUI* pCmdUI) 
{
    if (GetDocument()->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk)
    {
		// SYLK export is turned on so indicate status to the user. This will
		// put a check mark beside the menu item and show the toolbar button depressed
		pCmdUI->SetCheck (1);
    }
    else
    {
		// SYLK export is turned off so indicate status to the user.  This will
		// remove the check mark beside the menu item and show the toolbar button as raised.
		pCmdUI->SetCheck (0);
    }
}

inline BOOL CPCSampleView::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	static wchar_t toolTextToggleExportSylk [64] = L"Toggle SYLK export.";
	static wchar_t toolTextClearWindow [64] = L"Clear the log displayed.";
	static wchar_t toolTextConnectLan [64] = L"Log on the POS terminal through the LAN.";

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;

	switch (pNMHDR->idFrom) {
		case ID_TOGGLE_SYLK_EXPORT:
			pTTT->lpszText = toolTextToggleExportSylk;
			return TRUE;
		case ID_WINDOW_CLEAR:
			pTTT->lpszText = toolTextClearWindow;
			return TRUE;
		case ID_CONNECT_LAN_ON:
			pTTT->lpszText = toolTextConnectLan;
			return TRUE;
	}
	
	return FALSE;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSAMPLEVIEW_H__70072F0C_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////// END OF FILE ( PCSampleView.h ) ///////////////////////////
