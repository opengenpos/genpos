/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//  ---------------------------------------------------------------------------
//     Georgia Southern University Research Services Foundation
//     donated by NCR to the research foundation in 2002 and maintained here since.
//        2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
//        2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
//        2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
//        2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
//        2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
//        2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
//        2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
//        2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
// 
//     moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
//     moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
//  ---------------------------------------------------------------------------
//
//  FILE NAME:  PCSampleView.cpp
//
//  PURPOSE:    Provides PCSample Main View class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "time.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"



#include "PCSample.h"
#include "PCSampleDoc.h"
#include "MainFrm.h"

// Connect Engine UI
#include "RPropertySheet.h"
#include "RPageConnect.h"
#include "RPageCashierAction.h"
#include "RPageEmployeeChange.h"
#include "RPageGuestCheck.h"

#include "PCSampleView.h"

#include "TerminalNoDlg.h"
#include "CashierNoDlg.h"
#include "GCNoDlg.h"
#include "MDeptNoDlg.h"
#include "DeptNoDlg.h"
#include "PluAssignUpdate.h"

#include "DiaCashierAddEdit.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <functional>

class CDialogDlg : public CDialog
{
	// Construction
	void(*funcDX)(CDataExchange* pDX);                         // may be invoked in CDialogDlg::DoDataExchange(CDataExchange* pDX)
	void(*funcDXpDoc)(CDataExchange* pDX, CDocument *pDoc);    // may be invoked in CDialogDlg::DoDataExchange(CDataExchange* pDX)
	void(*funcInit)(CWnd *dlgWnd);
	void(*funcOK)(CWnd *dlgWnd, CDocument *pDoc);              // may be invoked in CDialogDlg::OnOK()
	void(*funcOKData)(CWnd *dlgWnd, void *pData);

	std::function<void(CDataExchange*)> flFuncDX;              // may be invoked in CDialogDlg::DoDataExchange(CDataExchange* pDX)
	std::function<void(CWnd*)> flFuncInit;
	std::function<void(CWnd*)> flFuncOK;                       // may be invoked in CDialogDlg::OnOK()
	std::function<void(CWnd *dlgWnd, void *pData)> fdFuncOK;   // may be invoked in CDialogDlg::OnOK()

	CDocument *m_pDoc;
	void      *m_pData;

	void InitInitVars(void) {
		funcDX = nullptr;         // void(*f)(CDataExchange* pDX)
		flFuncDX = nullptr;       // std::function<void(CDataExchange*)> f
		funcDXpDoc = nullptr;     // void(*f)(CDataExchange* pDX, CDocument *pDoc)

		funcInit = nullptr;       // void(*f)(CWnd *dlgWnd), may be invoked in CDialogDlg::OnInitDialog()
		flFuncInit = nullptr;     // std::function<void(CWnd *dlgWnd)> f, may be invoked in CDialogDlg::OnInitDialog()

		funcOK = nullptr;         // void(*f)(CWnd *dlgWnd, CDocument *pDoc), may be invoked in CDialogDlg::OnOK()
		flFuncOK = nullptr;       // std::function<void(CWnd *dlgWnd)> f, may be invoked in CDialogDlg::OnOK()
		fdFuncOK = nullptr;       // std::function<void(CWnd *dlgWnd, void *pData)> f, may be invoked in CDialogDlg::OnOK()

		funcOKData = nullptr;

		m_pDoc = nullptr;
		m_pData = nullptr;
	}

	// allow for CDialogDlg::DoDataExchange(CDataExchange* pDX) to call a standard function to
	// either initialize or save dialog data.
	void InitVars(void(*f)(CDataExchange* pDX) = nullptr, CDocument *pDoc = nullptr, void *pData = nullptr) {
		InitInitVars();
		funcDX = f;                // void(*f)(CDataExchange* pDX), may be invoked in CDialogDlg::DoDataExchange(CDataExchange* pDX)
		m_pDoc = pDoc;
		m_pData= pData;
	}

	// allow for CDialogDlg::DoDataExchange(CDataExchange* pDX) to call a standard function to
	// either initialize or save dialog data. the standard function is called with a pointer to 
	// the CDocument object allowing access to that data.
	void InitVars(void(*f)(CDataExchange* pDX, CDocument *pDoc), CDocument *pDoc) {
		InitInitVars();
		funcDXpDoc = f;           // void(*f)(CDataExchange* pDX, CDocument *pDoc), may be invoked in CDialogDlg::DoDataExchange(CDataExchange* pDX)
		m_pDoc = pDoc;
	}

	void InitVars(std::function<void(CDataExchange*)> f) {
		InitInitVars();
		flFuncDX = f;           // std::function<void(CDataExchange*)> f
	}

public:
	CDialogDlg(UINT nIDTemplate, CWnd* pParentWnd, void(*f)(CDataExchange* pDX) = nullptr) : CDialog(nIDTemplate, pParentWnd) { InitVars(f); }
	CDialogDlg(UINT nIDTemplate, CWnd* pParentWnd, std::function<void(CDataExchange*)> f) : CDialog(nIDTemplate, pParentWnd) { InitVars(f); }
	CDialogDlg(UINT nIDTemplate, CDocument *pDoc, CWnd* pParentWnd, void(*f)(CDataExchange* pDX, CDocument *pDoc) = nullptr) : CDialog(nIDTemplate, pParentWnd) {
		InitVars(f, pDoc);
	}

	void SetDataExchangeCallBack(void(*f)(CDataExchange* pDX)) { funcDX = f; };
	void SetDataExchangeCallBack(std::function<void(CDataExchange*)> f) { flFuncDX = f; };

	void SetInitCallBack(void(*f)(CWnd *dlgWnd)) { funcInit = f; }
	void SetInitCallBack(std::function<void(CWnd *dlgWnd)> f) { flFuncInit = f; };

	void SetOnOkCallBack(void(*f)(CWnd *dlgWnd, CDocument *pDoc)) { funcOK = f; }
	void SetOnOkCallBack(std::function<void(CWnd *dlgWnd)> f) { flFuncOK = f; };
	void SetOnOkCallBack(void(*f)(CWnd *dlgWnd, void *pData), void *pData) { funcOKData = f; m_pData = pData; }
	void SetOnOkCallBack(std::function<void(CWnd *dlgWnd, void *pData)> f, void *pData) { fdFuncOK = f; m_pData = pData; }

	// Dialog Data
	//{{AFX_DATA(CCashierNoDlg)
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCashierNoDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CCashierNoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CDialogDlg, CDialog)
	//{{AFX_MSG_MAP(CCashierNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (funcDX) funcDX(pDX);
	if (flFuncDX) flFuncDX(pDX);
	if (funcDXpDoc) funcDXpDoc(pDX, m_pDoc);
	//{{AFX_DATA_MAP(CCashierNoDlg)
	//}}AFX_DATA_MAP
}

BOOL CDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (funcInit) funcInit(this);
	if (flFuncInit) flFuncInit(this);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDlg::OnOK()
{
	if (funcOK) funcOK(this, m_pDoc);
	if (flFuncOK) flFuncOK(this);
	if (funcOKData) funcOKData(this, m_pData);
	if (fdFuncOK) fdFuncOK(this, m_pData);
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
//
//          D Y N A M I C    O B J E C T    C R E A T I O N
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPCSampleView, CScrollView)

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPCSampleView, CScrollView)
	//{{AFX_MSG_MAP(CPCSampleView)
	ON_COMMAND(ID_READ_REGFIN, OnReadRegfin)
	ON_COMMAND(ID_READ_REGFIN_PTD, OnReadRegfinPtd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_READ_REGFIN, OnUpdateReadRegfin)
	ON_UPDATE_COMMAND_UI(ID_READ_REGFIN_PTD, OnUpdateReadRegfinPtd)
	ON_COMMAND(ID_READ_INDFIN, OnReadIndfin)
	ON_UPDATE_COMMAND_UI(ID_READ_INDFIN, OnUpdateReadIndfin)
	ON_COMMAND(ID_READ_SERVICETIME, OnReadServicetime)
	ON_UPDATE_COMMAND_UI(ID_READ_SERVICETIME, OnUpdateReadServicetime)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_READ_ETK, OnReadEtk)
	ON_UPDATE_COMMAND_UI(ID_READ_ETK, OnUpdateReadEtk)
	ON_COMMAND(ID_RESET_CASHIER, OnResetCashier)
	ON_UPDATE_COMMAND_UI(ID_RESET_CASHIER, OnUpdateResetCashier)
	ON_COMMAND(ID_RESET_SERVICETIME, OnResetServicetime)
	ON_UPDATE_COMMAND_UI(ID_RESET_SERVICETIME, OnUpdateResetServicetime)
	ON_COMMAND(ID_RESET_INDFIN, OnResetIndfin)
	ON_UPDATE_COMMAND_UI(ID_RESET_INDFIN, OnUpdateResetIndfin)
	ON_COMMAND(ID_RESET_GC_BYCASHIER, OnResetGCByCashier)
	ON_UPDATE_COMMAND_UI(ID_RESET_GC_BYCASHIER, OnUpdateResetGCByCashier)
	ON_COMMAND(ID_RESET_GC_BYGCN, OnResetGCByGCNo )
	ON_UPDATE_COMMAND_UI(ID_RESET_GC_BYGCN, OnUpdateResetGCByGCNo)
	ON_COMMAND(ID_WINDOW_PRINT, OnWindowPrint)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PRINT, OnUpdateWindowClear)
	ON_COMMAND(ID_WINDOW_CLEAR, OnWindowClear)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLEAR, OnUpdateWindowClear)
	ON_COMMAND(ID_WINDOW_PRINTLASTREPORT, OnWindowPrintLastReport)
	ON_COMMAND(ID_TOGGLE_SYLK_EXPORT, OnToggleExportSylk)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_SYLK_EXPORT, OnUpdateToggleExportSylk)
	ON_COMMAND(ID_READ_CASHIER, OnReadCashier)
	ON_UPDATE_COMMAND_UI(ID_READ_CASHIER, OnUpdateReadCashier)
	ON_COMMAND(ID_READ_GC_ALL, OnReadGCAll)
	ON_UPDATE_COMMAND_UI(ID_READ_GC_ALL, OnUpdateReadGCAll)
	ON_COMMAND(ID_READ_GC_BYGCN, OnReadGCByGCNo)
	ON_UPDATE_COMMAND_UI(ID_READ_GC_BYGCN, OnUpdateReadGCByGCNo)
	ON_COMMAND(ID_READ_GC_BYCASHIER, OnReadGCByCashier)
	ON_UPDATE_COMMAND_UI(ID_READ_GC_BYCASHIER, OnUpdateReadGCByCashier)
	ON_COMMAND(ID_RESET_OPENEDGC, OnResetOpenedGC)
	ON_UPDATE_COMMAND_UI(ID_RESET_OPENEDGC, OnUpdateResetOpenedGC)
	ON_COMMAND(ID_READ_HOURLY, OnReadHourly)
	ON_UPDATE_COMMAND_UI(ID_READ_HOURLY, OnUpdateReadHourly)
	ON_COMMAND(ID_READ_DEPT, OnReadDept)
	ON_UPDATE_COMMAND_UI(ID_READ_DEPT, OnUpdateReadDept)
	ON_COMMAND(ID_READ_PLU, OnReadPluByDept)
	ON_UPDATE_COMMAND_UI(ID_READ_PLU, OnUpdateReadPlu)
	ON_COMMAND(ID_READ_PLUEX, OnReadPluEx)
	ON_UPDATE_COMMAND_UI(ID_READ_PLUEX, OnUpdateReadPluEx)
	ON_COMMAND(ID_READ_COUPON, OnReadCoupon)
	ON_UPDATE_COMMAND_UI(ID_READ_COUPON, OnUpdateReadCoupon)
	ON_COMMAND(ID_RESET_PLU, OnResetPlu)
	ON_UPDATE_COMMAND_UI(ID_RESET_PLU, OnUpdateResetPlu)
	ON_COMMAND(ID_RESET_COUPON, OnResetCoupon)
	ON_UPDATE_COMMAND_UI(ID_RESET_COUPON, OnUpdateResetCoupon)
	ON_COMMAND(ID_RESET_ETK, OnResetETK)
	ON_UPDATE_COMMAND_UI(ID_RESET_ETK, OnUpdateResetETK)
	ON_COMMAND(ID_PARA_UPDATE_PLU, OnParaUpdatePlu)
	ON_COMMAND(ID_BCAST_SUP_AC_PARMS, OnBroadCastSupAc)
	ON_COMMAND(ID_BCAST_ALL_P_AC_PARMS, OnBroadCastAllPAc)
	ON_COMMAND(ID_BCAST_PLU_OEP_PARMS, OnBroadCastPluOepAc)
	ON_COMMAND(ID_RETRIEVE_SYSTEMSTATUS, OnRetrieveSystemStatus)
	ON_COMMAND(ID_SAVE_LOG_TO_FILE, OnSaveLogToFile)
	ON_COMMAND(ID_WINDOW_CONNECTENGINE, OnConnectEngine)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CONNECTENGINE, &CPCSampleView::OnUpdateWindowConnectengine)
	ON_COMMAND(ID_WINDOW_ALWAYSONTOP, &CPCSampleView::OnWindowAlwaysontop)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_ALWAYSONTOP, &CPCSampleView::OnUpdateWindowAlwaysontop)
	ON_COMMAND(ID_PROV_CASHIER, OnProvCashier)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_MESSAGE(WM_APP + 10, &CPCSampleView::OnWindowSetSection)
	ON_COMMAND(ID_DIAOLOG_REPORTLIST, &CPCSampleView::OnDisplayReportList)
	ON_COMMAND(ID_SCAN_FOR_MASTER, &CPCSampleView::OnScanForMaster)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CPCSampleView::CPCSampleView()
//
//  PURPOSE :   Constructor of PCSample View class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleView::CPCSampleView() : iButtonDown(0), m_sizeCell( 0, 0 ), m_fontPointSize(120)
{
	this->m_bAlwaysOnTop = false;
	this->m_bConnectEngineWindowShown = false;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::~CPCSampleView()
//
//  PURPOSE :   Constructor of PCSample View class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleView::~CPCSampleView()
{
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::AssertValid()
//
//  PURPOSE :   Perform Validity check of this object.
//
//  RETURN :    No return value.
//
//===========================================================================
#ifdef _DEBUG
void CPCSampleView::AssertValid() const
{
	CView::AssertValid();
}
#endif

//===========================================================================
//
//  FUNCTION :  CPCSampleView::Dump()
//
//  PURPOSE :   Dump the contents of this object.,
//
//  RETURN :    No return value.
//
//===========================================================================
#ifdef _DEBUG
void CPCSampleView::Dump( CDumpContext& dc ) const
{
    // --- Dump the contect ob this object ---
    CView::Dump( dc );
}
#endif

//===========================================================================
//
//  FUNCTION :  CPCSampleView::GetDocument()
//
//  PURPOSE :   Get active document class of PC Sample View.
//
//  RETURN :    No return value.
//
//===========================================================================

#ifdef _DEBUG
CPCSampleDoc* CPCSampleView::GetDocument() // non-debug version is inline
{
    ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CPCSampleDoc )));
    return ( CPCSampleDoc* )m_pDocument;
}
#endif //_DEBUG

//===========================================================================
//
//  FUNCTION :  CPCSampleView::PreCreateWindow()
//
//  PURPOSE :   Called by frame work before the creation of window.
//
//  RETURN :    TRUE    - Window creation is successful.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPCSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow( cs );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnCreate()
//
//  PURPOSE :   Create main view window.
//
//  RETURN :    0   - Continue to create window.
//              -1  - Destroy to this window.
//
//===========================================================================

int CPCSampleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    int     nRet;

	nRet = CScrollView::OnCreate( lpCreateStruct );
    
    if ( nRet != -1 )
    {
        // --- set default scroll size (no scroll) ---
        SIZE    mySize;

        mySize.cx = 1;
        mySize.cy = 1;

        SetScrollSizes( MM_TEXT, mySize );
    }

	return ( nRet );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnInitialUpdate()
//
//  PURPOSE :   Called by frame work after the view first attached document.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

    // --- get device context of this view ---
    CClientDC   dc( this );

    // --- get font size and calculate height/width of 1 character ---
    dc.SelectStockObject( ANSI_FIXED_FONT );

    TEXTMETRIC  tm;

    dc.GetTextMetrics( &tm );

    m_sizeCell.cx = tm.tmAveCharWidth;
    m_sizeCell.cy = tm.tmHeight + tm.tmExternalLeading;

	// Set an initial scroll size for the CScrollView which will be
	// modified in the OnDraw () later when presenting the actual view
	// and we have the complete document and can calculate the document's
	// scrollable size properly.
	// Get the client area size of the dialog we are putting the
	// CScrollView into and pull the right edge in sufficient to
	// clear buttons on the right hand side of the dialog.
	RECT  rectSize;
	GetClientRect (&rectSize);
	rectSize.right -= 5;
	rectSize.left  += 5;

	CSize sizeTotal;
	sizeTotal.cx = rectSize.right;
	sizeTotal.cy = 1 * rectSize.bottom;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::SetScrollSize()
//
//  PURPOSE :   Set scroll size by current displaying contents.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::SetScrollSize()
{
    CPCSampleDoc*   pDoc;

    pDoc = GetDocument();

    // --- get no. of lines of report strings ---
    int   dwNoOfLines = pDoc->GetNoOfReportStrings();

    // --- set new scroll size by current displaying contents ---
    if ( 0 < dwNoOfLines )
    {
        // --- get client area of this view ---
        CRect   rcClient;
        GetClientRect( &rcClient );

        // --- get maximum row/columns in this client area ---
        int nRows;
        int nCols;

        nRows = rcClient.Height() / m_sizeCell.cy;
        nCols = rcClient.Width()  / m_sizeCell.cx;

        // --- get maximum column of report strings ---
        DWORD   dwMaxColumn = pDoc->GetMaxColumnOfStrings();

        // --- max lines/column within client area, no scroll ---
        if ( dwMaxColumn < static_cast< DWORD >( nCols ))
        {
            dwMaxColumn = 0;
        }

        if ( dwNoOfLines < nRows )
        {
            dwNoOfLines = 0;
        }

        // --- set new scroll sizes to this view ---
        CSize   total( dwMaxColumn * m_sizeCell.cx, dwNoOfLines * m_sizeCell.cy );
        CSize   page(  nCols * m_sizeCell.cx, nRows * m_sizeCell.cy );
        CSize   line(  m_sizeCell.cx, m_sizeCell.cy );

 		sizeTotal = total;
		SetScrollSizes( MM_TEXT, total, page, line );
    }
    else
    {
        // --- no scroll, because there is no report line ---
 		sizeTotal.cx = 1;
 		sizeTotal.cy = 1;
        SetScrollSizes( MM_TEXT, sizeTotal);
    }
}

// Handle mouse messages to allow for the use of touch screen scrolling.
// We handle these mouse messages so that this application can be used on
// a touchscreen terminal and the main page can be scrolled up and down.
afx_msg void CPCSampleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	pointMouseMove = pointButtonDown = point;
	iButtonDown = 1;
}

afx_msg void CPCSampleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	pointButtonUp = point;
	iButtonDown = 0;
}

afx_msg void CPCSampleView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (iButtonDown) {
		int yDiff = point.y - pointMouseMove.y;

		if (abs(yDiff) > 20) {
			CPoint curPos = GetScrollPosition  ();
			yDiff = ((yDiff * 10) + yDiff * 5) / 10;
			if (yDiff < 0) {
				curPos.y -= yDiff;
				if (curPos.y + 200 < sizeTotal.cy) {
					ScrollToPosition (curPos);
				}
			} else {
				curPos.y -= yDiff;
				if (curPos.y < 0) curPos.y = 0;
				ScrollToPosition (curPos);
			}
			pointMouseMove = point;
		}
	}
}


//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnDraw()
//
//  PURPOSE :   Draw documents on client area of this view.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnDraw(
    CDC* pDC                    // points to device context of this view
    )
{
	CFont font;
	font.CreatePointFont(m_fontPointSize, _T("Courier New"), pDC);
	pDC->SelectObject(font);

	TEXTMETRIC  tm;

	pDC->GetTextMetrics(&tm);

	m_sizeCell.cx = tm.tmAveCharWidth;
	m_sizeCell.cy = tm.tmHeight + tm.tmExternalLeading;

	// --- this view draws text with fixed font ---
//    pDC->SelectStockObject( ANSI_FIXED_FONT );

	CPCSampleDoc* pDoc;
    pDoc = GetDocument();

    // --- get client area of this view ---
    CRect   rcClient;
    GetClientRect( &rcClient );

    // --- get currnt scroll position ---
    CPoint  ptOrigin;
    ptOrigin = GetScrollPosition();

    // --- calculate first/last line of report string in client area ---
    int nFirstLine;
    int nLastLine;
    int nY;

    nFirstLine = ptOrigin.y / m_sizeCell.cy;
    nLastLine  = rcClient.Height() / m_sizeCell.cy + 2 + nFirstLine;
    nY         = nFirstLine * m_sizeCell.cy;

    // --- set tab space as 4 characters ---
    int nTabPosition;
    nTabPosition = m_sizeCell.cx * 4;

    // --- draw report string between first and last line of client area ---
    for ( ; nFirstLine < nLastLine; nFirstLine++ )
    {
        CString str;

        // --- get report string by line# ---
        if ( pDoc->GetReportString( str, nFirstLine ))
        {
            // --- draw string with tab expand ---
            pDC->TabbedTextOut( 0, nY, str, 1, &nTabPosition, 0 );
        }

        // --- points to next line to draw ---
        nY += m_sizeCell.cy;
    }

	pDC->SelectStockObject( ANSI_FIXED_FONT );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnUpdate()
//
//  PURPOSE :   Called by frame work after the document is modified.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnUpdate(
    CView*      pSender,        // points to the view that modified
    LPARAM      lHint,          // information about modifies
    CObject*    pHint           // information about modifies
    )
{
    // --- set horizontal/vertical scroll sizes ---
    SetScrollSize();

    // --- is this modification called by CPCSampleDoc ? ---
    if ( lHint != 0 )
    {
        // --- this call is CPCSampleDoc::SetReportString() ---
        // --- get client rect of this view ---
        CRect   rcClient;
        GetClientRect( &rcClient );

        // --- get maximum rows/columns in this client area ---
        int nRows;
        int nCols;

        nRows = rcClient.Height() / m_sizeCell.cy;
        nCols = rcClient.Width()  / m_sizeCell.cx;

        // --- get maximum rows/columns in report string ---
        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();

        // --- get current scroll position ---
        POINT   ptOrigin;
        ptOrigin = GetScrollPosition();

        // --- get maximum horizontal/vertical scroll size ---
        CSize   size;
        size = GetTotalSize();

        // --- scroll to the last line of report string ---
        POINT   pt;
        pt.x = ptOrigin.x;
        pt.y = size.cy;

        ScrollToPosition( pt );
    }

    CScrollView::OnUpdate( pSender, lHint, pHint );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnSize()
//
//  PURPOSE :   Size this view window to new size.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnSize(
    UINT    nType,              // type of resize
    int     cx,                 // new horizontal size
    int     cy                  // new vertical size
    ) 
{
	CScrollView::OnSize(nType, cx, cy);

    // --- calculate horizontal/vertical scroll size by new window size ---
    SetScrollSize();	
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnKeyDown()
//
//  PURPOSE :   Scroll view window, if arrow, page up/down key is pressed.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnKeyDown(
    UINT    nChar,              // inputed character VK code
    UINT    nRepCnt,            // repeat count
    UINT    nFlags )            // additional key flags
{
    // --- get current scroll position ---
    CPoint  ptOrigin;
    ptOrigin = GetScrollPosition();

    // --- get maximum horizontal/vertical scroll size ---
    POINT   pt;
    pt.x = ptOrigin.x;
    pt.y = ptOrigin.y;

    CSize   sizeMax;
    sizeMax = GetTotalSize();

    CRect   rcClient;

    switch ( nChar )
    {
    case VK_UP:     // UP ARROW
        // --- scroll to 1 line up ---
        pt.y -= m_sizeCell.cy;
        if ( pt.y < 0 )
        {
            pt.y = 0;
        }
        ScrollToPosition( pt );
        break;

    case VK_DOWN:   // DOWN ARROW
        // --- scroll to 1 line down ---
        pt.y += m_sizeCell.cy;
        ScrollToPosition( pt );
        break;

    case VK_PRIOR:  // PAGE UP
        // --- scroll to 1 page up ---
        GetClientRect( &rcClient );

        pt.y -= rcClient.Height();
        if ( pt.y < 0 )
        {
            pt.y = 0;
        }
        ScrollToPosition( pt );
        break;

    case VK_NEXT:   // PAGE DOWN
        // --- scroll to 1 page down ---
        GetClientRect( &rcClient );

        pt.y += rcClient.Height();
        ScrollToPosition( pt );
        break;

    case VK_RIGHT:  // RIGHT ARROW
        // --- scroll to 1 characters right ---
        GetClientRect( &rcClient );

        if ( rcClient.Width() < sizeMax.cx )
        {
            pt.x += m_sizeCell.cx;
            if ( sizeMax.cx < pt.x )
            {
                pt.x = sizeMax.cx - rcClient.Width();
            }
            ScrollToPosition( pt );
        }
        break;

    case VK_LEFT:   // LEFT ARROW
        // --- scroll to 1 characters left ---
        pt.x -= m_sizeCell.cx;
        if ( pt.x < 0 )
        {
            pt.x = 0;
        }
        ScrollToPosition( pt );
        break;

    default:        // OTHER KEYS
        // --- do nothing ---
        break;
    }

	CScrollView::OnKeyDown( nChar, nRepCnt, nFlags );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnWindowClear()
//
//  PURPOSE :   Clear all of contents in view window.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnWindowClear()
{
    // --- clear all of report strings is document ---
    GetDocument()->ClearAllReport();
}

// handle the menu selection event generated by either selecting the menu item
// from the menu or by clicking on the icon in the toolbar.
void CPCSampleView::OnToggleExportSylk()
{
	// Exclusive Or to toggle the indicator bit from 0 to 1 and 1 to 0.
	GetDocument()->ulReportOptionsMap ^= CPCSampleDoc::ulReportOptionsExportSylk;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadRegfin()
//
//  PURPOSE :   Issue register financial read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadRegfin() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

	ASSERT(pDoc);

    // --- read current daily register financial total ---
    CTtlRegFin  ttlRegFin;

    if ( ttlRegFin.Read( CLASS_TTLCURDAY ))
    {
		CParaMdcMem   mdcMem;

		mdcMem.Read();
		pDoc->m_discountRate.Read();
		pDoc->m_currencyRate.Read();
		pDoc->m_totalKeyData.Read();
		pDoc->m_transactionMnemonics.Read();

		// --- report header and body of read report ---
        pDoc->ReportHeader( ttlRegFin );
        pDoc->ReportTotal( ttlRegFin, mdcMem );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_REGFIN_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "FINANCIAL - DAILY READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLREGFIN, pcsample::ERR_TTLREAD, ttlRegFin.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadRegfin()
//
//  PURPOSE :   Issue register financial read report (PTD total).
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadRegfinPtd() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

	ASSERT(pDoc);
	
	// --- read current PTD register financial total ---
    CTtlRegFin  ttlRegFin;

    if ( ttlRegFin.Read( CLASS_TTLCURPTD ))
    {
		CParaMdcMem   mdcMem;

		mdcMem.Read();
		pDoc->m_discountRate.Read();
		pDoc->m_currencyRate.Read();
		pDoc->m_totalKeyData.Read();
		pDoc->m_transactionMnemonics.Read();

		// --- report header and body of read report ---
        pDoc->ReportHeader( ttlRegFin );
        pDoc->ReportTotal( ttlRegFin, mdcMem );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_REGFIN_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "FINANCIAL - PTD READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLREGFIN, pcsample::ERR_TTLREAD, ttlRegFin.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadIndfin()
//
//  PURPOSE :   Issue individual register financial read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadIndfin() 
{
    CTerminalNoDlg  dlg;

    // --- show terminal# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();

		ASSERT(pDoc);
		
		// --- read current daily total of selected terminal ---
        CTtlIndFin  ttlIndFin;

        if ( ttlIndFin.Read( CLASS_TTLCURDAY, dlg.m_bTerminalNo ))
        {
			CParaMdcMem   mdcMem;

			mdcMem.Read();
			pDoc->m_discountRate.Read();
			pDoc->m_currencyRate.Read();
			pDoc->m_totalKeyData.Read();
			pDoc->m_transactionMnemonics.Read();

			// --- report header and body of read report ---
            pDoc->ReportHeader( ttlIndFin );
            pDoc->ReportTotal( ttlIndFin, mdcMem );

            // --- print read report log on journal paper ---
            GetMyParent()->PrintOnJournal( AC_IND_READ_RPT );
            GetMyParent()->PrintOnJournal( CString( "IND-FIN - DAILY READ" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_TTLREAD, ttlIndFin.GetLastError(), dlg.m_bTerminalNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetIndfin()
//
//  PURPOSE :   Issue individual register financial reset report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetIndfin() 
{
    CTerminalNoDlg  dlg;

    // --- show terminal# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {

		USHORT  usTerminalNo = dlg.m_bTerminalNo;

        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();

        CSignInLock SignIn;

        // --- lock terminal to sign in operator ---
        if ( SignIn.Lock( usTerminalNo ))
        {
            BOOL    fResetAtEOD;
            fResetAtEOD = FALSE;

            CTtlIndFin  ttlIndFin;

            // --- read saved daily total of selected terminal ---
            ReadAndReset( ttlIndFin, usTerminalNo, fResetAtEOD );

            // --- print reset report log on journal paper ---
            GetMyParent()->PrintOnJournal( AC_IND_RESET_RPT );
            GetMyParent()->PrintOnJournal( CString( "IND-FIN - DAILY RESET" ));

            // --- unlock locked terminal before exit function ---
            if ( ! SignIn.Unlock())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_INDUNLOCK, SignIn.GetLastError(), usTerminalNo );
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_INDLOCK, SignIn.GetLastError(), usTerminalNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadCashier()
//
//  PURPOSE :   Issue cashier read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadCashier()
{
	CDiaCashierSelect   dlg;

	dlg.LoadListControl();

    // --- show cashier# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
		ULONG  ulCashierNo = dlg.m_ulCashierNo;

        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();

        // --- read current daily total of selected cashier ---
        CTtlCashier ttlCashier;

        if ( ttlCashier.Read( CLASS_TTLCURDAY, ulCashierNo ))
        {
			CParaMdcMem         mdcMem;

			mdcMem.Read();
			pDoc->m_discountRate.Read();
			pDoc->m_currencyRate.Read();
			pDoc->m_totalKeyData.Read();
			pDoc->m_transactionMnemonics.Read();

			// --- report header and body of read report ---
            pDoc->ReportHeader( ttlCashier );
            pDoc->ReportTotal( ttlCashier, mdcMem );

            // --- print read report log on journal paper ---
            GetMyParent()->PrintOnJournal( AC_CASH_READ_RPT );
            GetMyParent()->PrintOnJournal( CString( "CASHIER - DAILY READ" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_TTLREAD, ttlCashier.GetLastError(), ulCashierNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetCashier()
//
//  PURPOSE :   Issue cashier reset report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetCashier() 
{
	CDiaCashierSelect   dlg;

	dlg.LoadListControl();

    // --- show cashier# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
		ULONG  ulCashierNo = dlg.m_ulCashierNo;

        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();

        CParaCashier    paraCashier;

        // --- read cashier parameter by inputed cashier# ---
        if ( paraCashier.Read( ulCashierNo ))
        {
            COperatorLock   Operator;

            // --- lock cashier to log on terminal ---
            if ( Operator.Lock( ulCashierNo ))
            {
                CTtlCashier ttlCashier;

                // --- read saved daily total of selected cashier ---
                ReadAndReset( ttlCashier, paraCashier );

                // --- print reset report log on journal paper ---
                GetMyParent()->PrintOnJournal( AC_CASH_RESET_RPT );
                GetMyParent()->PrintOnJournal( CString( "CASHIER - DAILY RESET" ));

                // --- unlock locked cashier before exit function ---
                if ( ! Operator.Unlock())
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_INDUNLOCK, Operator.GetLastError(), ulCashierNo);
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_INDLOCK, Operator.GetLastError(), ulCashierNo );
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_INDREAD, paraCashier.GetLastError(), ulCashierNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadDept()
//
//  PURPOSE :   Issue department read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadDept() 
{
    CMDeptNoDlg dlg;
	
    // --- show major department# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CPCSampleDoc*   pDoc;
        pDoc = GetDocument();


        CTtlDept    ttlDept;
        CParaDept   paraDept;

        // --- initialize parameter object before function call ---
        paraDept.Initialize();
		
        BOOL        fHeaderPrinted;
        fHeaderPrinted = FALSE;

        // --- read dept record belongs to selected major dept# ---
		pDoc->llReportTotalAmount = pDoc->lReportTotalCount = 0;
        while ( paraDept.Read( dlg.m_bMajorDeptNo ))
        {
			// --- read department total by retrieved dept# ---
            USHORT   usDeptNo;
            usDeptNo = paraDept.getDeptNo();
            if ( ttlDept.Read( CLASS_TTLCURDAY, usDeptNo ))
            {
                if ( ! fHeaderPrinted )
                {
                    // --- report department report header ---
                    pDoc->ReportHeader( ttlDept );
                    fHeaderPrinted = TRUE;
                }

                // --- report body of department report ---
                pDoc->ReportTotal( ttlDept, paraDept );
            }
        }

		CString str;
		str.Format(_T("  TOTALS  \t\t\t\t\t\t\t\t\t\t %d\t%16s"), pDoc->lReportTotalCount, pDoc->MakeCurrencyString(pDoc->llReportTotalAmount));
		pDoc->SetReportString(str);		
		str.Format(_T("*** END OF DEPARTMENT DETAILS ***"));
		pDoc->SetReportString(str);		

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_DEPTSALE_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "DEPT - DAILY READ" ));
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadPluEx()
//
//  PURPOSE :   Issue PLU read report for the last twenty plu values.
//
//  RETURN :    No return value.
//
//===========================================================================
//ESMITH SR 275
void CPCSampleView::OnReadPluEx()
{
	BOOL	 fSuccess = FALSE;

    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

	CTtlPlu     ttlPlu;
	fSuccess = pcsample::ReadPluEx( ttlPlu, CLASS_TTLCURDAY, pDoc);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadPlu()
//
//  PURPOSE :   Issue PLU read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadPluByDept() 
{
    CDeptNoDlg dlg;

    // --- show PLU# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CPCSampleDoc*   pDoc = GetDocument();

        // --- get flexible memory in 2170 ---
        CParaFlexMem    paraFlexMem;

        if ( paraFlexMem.Read())
        {
            // --- determine inputed dept# is valid or not ---
            USHORT  usMaxDeptNo;
            usMaxDeptNo  = (USHORT)paraFlexMem.getMemorySize( FLEX_DEPT_ADR );

            if ( usMaxDeptNo )
            {
                // --- input dept# is valid ---

                CTtlPlu     ttlPlu;

                BOOL    fHeaderPrinted;
				BOOL    fSuccess = FALSE;	// ADD Saratoga
                fHeaderPrinted = FALSE;

				// --- read dept record belongs to selected major dept# ---
				pDoc->llReportTotalAmount = pDoc->lReportTotalCount = 0;

				// --- read all PLU records belonging to the dept# ---
				if ((USHORT)dlg.m_nDeptNo)
					fSuccess = pcsample::ReadAllRecords( CLASS_TTLCURDAY, ttlPlu, (USHORT)dlg.m_nDeptNo, fHeaderPrinted, pDoc );
				else
					fSuccess = pcsample::ReadAllRecords( CLASS_TTLCURDAY, ttlPlu, fHeaderPrinted, pDoc );

				// --- print read report log on journal paper ---
				if( fSuccess )
				{
					CString str;
					str.Format(_T("  TOTALS  \t\t\t\t\t\t\t\t\t\t\t\t\t\t %d\t%16s"), pDoc->lReportTotalCount, pDoc->MakeCurrencyString(pDoc->llReportTotalAmount));
					pDoc->SetReportString(str);
					str.Format(_T("*** END OF PLU REPORT ***"));
					pDoc->SetReportString(str);

					// --- print read report log on journal paper ---
					GetMyParent()->PrintOnJournal( AC_PLUSALE_READ_RPT );
					GetMyParent()->PrintOnJournal( CString( "PLU - DAILY READ" ));
				}
				else
				{
	                // --- inputed dept# is invalid ---
		            pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLREAD, TTL_NOTINFILE );
				}
            }
            else
            {
                // --- inputed dept# is invalid ---
                pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLREAD, TTL_NOTINFILE );
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraFlexMem.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetPlu()
//
//  PURPOSE :   Issue PLU reset report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetPlu()
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    COperatorLock   Operator;

    // --- lock all of operaror to sign in terminal ---
    if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
    {
        BOOL    fResetAtEOD;
        fResetAtEOD = FALSE;

        CTtlPlu ttlPlu;

        // --- read and reset all of PLU total ---
        ReadAndReset( ttlPlu, fResetAtEOD );

        // --- print reset report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_PLU_RESET_RPT );
        GetMyParent()->PrintOnJournal( CString( "PLU - DAILY RESET" ));

        // --- unlock locked operator before exit function ---
        if ( ! Operator.Unlock())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadCoupon()
//
//  PURPOSE :   Issue Coupon read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadCoupon() 
{
    CTtlCoupon  ttlCoupon;

    // --- read and report all of coupon totals ---
    pcsample::ReadAllRecords( CLASS_TTLCURDAY, ttlCoupon, GetDocument());

    // --- print read report log on journal paper ---
    GetMyParent()->PrintOnJournal( AC_CPN_READ_RPT );
    GetMyParent()->PrintOnJournal( CString( "COUPON - DAILY READ" ));
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetCoupon()
//
//  PURPOSE :   Issue Coupon reset report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetCoupon() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    COperatorLock   Operator;

    // --- lock all of operator to sign in terminal ---
    if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
    {
        BOOL    fResetAtEOD;
        fResetAtEOD = FALSE;

        CTtlCoupon  ttlCoupon;

        // --- read and reset all of coupon totals ---
        ReadAndReset( ttlCoupon, fResetAtEOD );

        // --- print reset report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_CPN_RESET_RPT );
        GetMyParent()->PrintOnJournal( CString( "COUPON - DAILY RESET" ));

        // --- unlock locked operator before exit function ---
        if ( ! Operator.Unlock())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadHourly()
//
//  PURPOSE :   Issue hourly activity read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadHourly() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CTtlHourly  ttlHourly;

    // --- read current daily total ---
    if ( ttlHourly.Read( CLASS_TTLCURDAY ))
    {
        // --- report header and body of hourly activity report ---
        pDoc->ReportHeader( ttlHourly );
        pDoc->ReportTotal( ttlHourly );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_HOURLY_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "HOURLY - DAILY READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLHOURLY, pcsample::ERR_TTLREAD, ttlHourly.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadServTime()
//
//  PURPOSE :   Issue service time read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadServicetime()
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CTtlServTime    ttlServTime;

    // --- read current daily total ---
    if ( ttlServTime.Read( CLASS_TTLCURDAY ))
    {
        // --- report header and body of service time report ---
        pDoc->ReportHeader( ttlServTime );
        pDoc->ReportTotal( ttlServTime );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_SERVICE_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "SERVICE - DAILY READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_TTLREAD, ttlServTime.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetServTime()
//
//  PURPOSE :   Issue service time reset report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetServicetime() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    COperatorLock   Operator;

    // --- lock all of operator to sign in terminal ---
    if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
    {
        CTtlServTime    ttlServTime;

        // --- determine the total is able to reset or not ---
        if ( ttlServTime.IsResetOK())
        {
            // --- the total is able to reset ---
            BOOL    fResetAtEOD;
            fResetAtEOD = FALSE;

            // --- reset service time total ---
            if ( ttlServTime.Reset( fResetAtEOD ))
            {
                // --- read saved daily total ---
                if ( ttlServTime.Read( CLASS_TTLSAVDAY ))
                {
                    // --- report header and body of service time ---
                    pDoc->ReportHeader( ttlServTime );
                    pDoc->ReportTotal( ttlServTime );

                    // --- set the total as able to reset ---
                    if ( ! ttlServTime.ResetAsOK())
                    {
                        // --- function is failed, and report its error ---
                        pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_TTLISSRST, ttlServTime.GetLastError());
                    }

                    // --- print reset report log on journal paper ---
                    GetMyParent()->PrintOnJournal( AC_SERVICE_RESET_RPT );
                    GetMyParent()->PrintOnJournal( CString( "SERVICE - DAILY RESET" ));
                }
                else
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_TTLREAD, ttlServTime.GetLastError());
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_RESET, ttlServTime.GetLastError());
            }
        }
        else
        {
            // --- the total is not able to reset ---
            pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_ISSUED, ttlServTime.GetLastError());
        }

        if ( ! Operator.Unlock())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadEtk()
//
//  PURPOSE :   Issue ETK read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadEtk()
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    USHORT  usNoOfEmployee;
    ULONG*  paulEmployeeID;

    // --- prepare buffer to store all of employee# ---
    paulEmployeeID = new ULONG [ ETK_MAX_EMPLOYEE ];

    CTtlETK ttlETK;

    // --- get all of existing employee# ---
    if ( ttlETK.GetAllETKNo( CLASS_TTLCURDAY, paulEmployeeID, sizeof( ULONG ) * ETK_MAX_EMPLOYEE, usNoOfEmployee ))
    {
        // --- report header of ETK report ---
        pDoc->ReportHeader( ttlETK );

        // --- report body of ETK report ---
        ReadAllRecords( CLASS_TTLCURDAY, ttlETK, paulEmployeeID, usNoOfEmployee );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_ETK_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "ETK - DAILY READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_ALLIDREAD, ttlETK.GetLastError());
    }

    // --- discard buffer before exit function ---
    delete [] paulEmployeeID;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetEtk()
//
//  PURPOSE :   Issue ETK read report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetETK() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CTtlETK ttlETK;

    // --- lock all of ETK before reset total ---
    if ( ttlETK.Lock())
    {
        // --- determine the total is able to reset or not ---
        if ( ttlETK.IsResetOK())
        {
            // --- reset the ETK total ---
            if ( ttlETK.Reset())
            {
                USHORT  usNoOfEmployee;
                ULONG*  paulEmployeeID;

                // --- prepare buffer to store all of employee# ---
                paulEmployeeID = new ULONG [ ETK_MAX_EMPLOYEE ];

                // --- get all of existing employee# ---
                if ( ttlETK.GetAllETKNo( CLASS_TTLSAVDAY, paulEmployeeID, sizeof( ULONG ) * ETK_MAX_EMPLOYEE, usNoOfEmployee ))
                {
                    // --- report header of ETK report ---
                    pDoc->ReportHeader( ttlETK );

                    // --- report body of ETK report ---
                    ReadAllRecords( CLASS_TTLSAVDAY, ttlETK, paulEmployeeID, usNoOfEmployee );

                    // --- set the total as able to reset ---
                    if ( ! ttlETK.ResetAsOK())
                    {
                        // --- function is failed, and report its error ---
                        pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_RESET, ttlETK.GetLastError());
                    }

                    // --- print reset report log on journal paper ---
                    GetMyParent()->PrintOnJournal( AC_ETK_RESET_RPT );
                    GetMyParent()->PrintOnJournal( CString( "ETK - DAILY RESET" ));
                }
                else
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_ALLIDREAD, ttlETK.GetLastError());
                }

                // --- discard buffer before exit function ---
                delete [] paulEmployeeID;
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_RESET, ttlETK.GetLastError());
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_ISSUED, ttlETK.GetLastError());
        }

        if ( ! ttlETK.Unlock())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_ALLUNLOCK, ttlETK.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_ALLLOCK, ttlETK.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadGCByGCNo()
//
//  PURPOSE :   Issue Guest Check read report by guest check#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadGCAll()
{
	CGuestCheck     ttlGC;
	CPCSampleDoc*   pDoc = GetDocument();

	BOOL    fSuccess;
	fSuccess = FALSE;

	// --- read guest check, if guest check file is not empty ---
	if (!ttlGC.IsEmpty())
	{
		USHORT  usNoOfGC = 0;
		USHORT* pausGCNo = new USHORT[GCF_MAX_GCF_NUMBER];

		// --- read all of existing guest check by cashier# ---
		if (ttlGC.GetAllGCNo(pausGCNo, sizeof(USHORT) * GCF_MAX_GCF_NUMBER, usNoOfGC))
		{
			// --- report header of guest check report ---
			pDoc->ReportHeader(ttlGC, 0);

			for (USHORT usI = 0; usI < usNoOfGC; usI++)
			{
				// --- read guest check by retrieved guest check# ---
				if (ttlGC.Read(pausGCNo[usI]))
				{
					// --- report body of guest check report ---
					pDoc->ReportTotal(ttlGC);
				}
				else
				{
					// --- function is failed, and report its error ---
					pDoc->ReportError(CLASS_TTLGUEST, pcsample::ERR_TTLREAD, ttlGC.GetLastError(), pausGCNo[usI]);
				}
			}

			fSuccess = TRUE;
		}
		else
		{
			// --- function is failed, and report its error ---
			pDoc->ReportError(CLASS_TTLGUEST, pcsample::ERR_ALLIDREAD, ttlGC.GetLastError());
		}

		// --- discard allocated buffer before exit function ---
		delete[] pausGCNo;
	}
	else
	{
		// --- function is failed, and report its error ---
		pDoc->ReportError(CLASS_TTLGUEST, pcsample::ERR_DNE, ttlGC.GetLastError());
	}
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadGCByGCNo()
//
//  PURPOSE :   Issue Guest Check read report by guest check#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadGCByGCNo() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CGCNoDlg    dlg;

    // --- show guest check# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CGuestCheck ttlGC;

        // --- read guest check total by inputed guest check# ---
        if ( ttlGC.Read( dlg.m_sGCNo ))
        {
            // --- report header and body of guest check report ---
            pDoc->ReportHeader( ttlGC );
            pDoc->ReportTotal ( ttlGC );

            // --- print reset report log on journal paper ---
            GetMyParent()->PrintOnJournal( AC_GCFL_READ_RPT );
            GetMyParent()->PrintOnJournal( CString( "GCF - READ" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_DNE, ttlGC.GetLastError(), dlg.m_sGCNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetGCByGCNo()
//
//  PURPOSE :   Issue Guest Check reset report by guest check#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetGCByGCNo() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CGCNoDlg    dlg;

    // --- show guest check# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        COperatorLock   Operator;

        // --- lock all of operator before reset total ---
        if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
        {
            CGuestCheck ttlGC;

            // --- read and lock guest check by inputed guest check# ---
            if ( ttlGC.ReadAndLock( dlg.m_sGCNo ))
            {
                // --- report header and body of guest check report ---
                pDoc->ReportHeader( ttlGC );
                pDoc->ReportTotal ( ttlGC );

                // --- reset guest check total ---
                if ( ! ttlGC.Reset( dlg.m_sGCNo ))
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_RESET, ttlGC.GetLastError(), dlg.m_sGCNo );
                }

                // --- print reset report log on journal paper ---
                GetMyParent()->PrintOnJournal( AC_GCFL_RESET_RPT );
                GetMyParent()->PrintOnJournal( CString( "GCF - RESET" ));
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_TTLREAD, ttlGC.GetLastError(), dlg.m_sGCNo );
            }

            // --- unlock locked operator before exit function ---
            if ( ! Operator.Unlock())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnReadGCByCashier()
//
//  PURPOSE :   Issue Guest Check read report by cashier#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnReadGCByCashier()
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

	CDiaCashierSelect   dlg;

	dlg.LoadListControl();

    // --- show cashier# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CGuestCheck ttlGC;

        BOOL    fIsResetOK;
        fIsResetOK = FALSE;

        // --- read and report guest check ---
        ReadAllRecords( ttlGC, dlg.m_ulCashierNo, fIsResetOK );

        // --- print read report log on journal paper ---
        GetMyParent()->PrintOnJournal( AC_GCFL_READ_RPT );
        GetMyParent()->PrintOnJournal( CString( "GCF - READ" ));
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetGCByCashier()
//
//  PURPOSE :   Issue Guest Check reset report by cashier#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetGCByCashier() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

	CDiaCashierSelect   dlg;

	dlg.LoadListControl();

    // --- show cashier# entry dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        COperatorLock   Operator;

        // --- lock all of operator before reset total ---
        if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
        {
            CGuestCheck ttlGC;

            // --- lock all of guest check before reset total ---
            if ( ttlGC.Lock( pcttl::LOCK_ALL_GC ))
            {
                BOOL    fIsResetOK;
                fIsResetOK = TRUE;

                // --- read and reset guest check ---
                ReadAllRecords( ttlGC, dlg.m_ulCashierNo, fIsResetOK );

                // --- unlock locked guest check before exit function ---
                if ( ! ttlGC.Unlock())
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_ALLUNLOCK, ttlGC.GetLastError());
                }

                // --- print reset report log on journal paper ---
                GetMyParent()->PrintOnJournal( AC_GCFL_RESET_RPT );
                GetMyParent()->PrintOnJournal( CString( "GCF - RESET" ));
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_ALLLOCK, ttlGC.GetLastError());
            }

            // --- unlock locked operator before exit function ---
            if ( ! Operator.Unlock())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
        }
    }
}


//===========================================================================
//
//  FUNCTION :  CPCSampleView::OnResetOpenedGC()
//
//  PURPOSE :   Close opened guest check by guest check#.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::OnResetOpenedGC() 
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CGCNoDlg    dlg;

    // --- show guest check# dialog on screen ---
    if ( dlg.DoModal() == IDOK )
    {
        CGuestCheck ttlGC;

        // --- close the chosen guest check (reset opened flag) ---
        if ( ttlGC.CloseOpenedGC( dlg.m_sGCNo ))
        {
            // --- report message as guest check is closed ---
            pDoc->ReportCloseGC( dlg.m_sGCNo );

            // --- print reset report log on journal paper ---
            GetMyParent()->PrintOnJournal( AC_EMG_GCFL_CLS );
            GetMyParent()->PrintOnJournal( CString( "GCF - EMERGENCY CLOSE" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_TTLISSRST, ttlGC.GetLastError(), dlg.m_sGCNo );
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::EnableMenuItem()
//
//  PURPOSE :   Enable menu item by logged on terminal or not.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleView::EnableMenuItem(
    const CCmdUI* pCmdUI        // points to the item that modified
    ) const
{
    CMainFrame* pMainFrame;

    pMainFrame = static_cast< CMainFrame* >( GetParent() );

    BOOL    fEnable;

    // --- enable/disable menu item by logged on terminal ---
    fEnable = pMainFrame->IsLoggedOn();

    (const_cast<CCmdUI*>( pCmdUI ))->Enable( fEnable );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAndReset()
//
//  PURPOSE :   read and reset the individual financial total.
//
//  RETURN :    No return value.
//
//===========================================================================

BOOL CPCSampleView::ReadAndReset(
    CTtlIndFin& ttlIndFin,      // individual financial total to read
    const USHORT usTerminalNo,  // terminal# to read total
    const BOOL fIsEndOfDay      // indicate call by EOD reset or not
    )
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    BOOL    fSuccess;
    fSuccess = FALSE;

    // --- is the total able to reset ? ---
    if ( ttlIndFin.IsResetOK( usTerminalNo ))
    {
        // --- reset the total ---
        if ( ttlIndFin.Reset( usTerminalNo, fIsEndOfDay ))
        {
            // --- read saved daily total ---
            if ( ttlIndFin.Read( CLASS_TTLSAVDAY, usTerminalNo ))
            {
				CParaMdcMem   mdcMem;

				mdcMem.Read();
				pDoc->m_discountRate.Read();
				pDoc->m_currencyRate.Read();
				pDoc->m_totalKeyData.Read();
				pDoc->m_transactionMnemonics.Read();

				// --- report header and body of total ---
                pDoc->ReportHeader( ttlIndFin );
                pDoc->ReportTotal( ttlIndFin, mdcMem );

                // --- set the total as able to reset ---
                fSuccess = ttlIndFin.ResetAsOK( usTerminalNo );

                if ( ! fSuccess )
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_TTLISSRST, ttlIndFin.GetLastError(), usTerminalNo );
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_TTLREAD, ttlIndFin.GetLastError(), usTerminalNo );
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_RESET, ttlIndFin.GetLastError(), usTerminalNo );
        }
    }
    else
    {
        // --- the total is not able to reset ---
        pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_ISSUED, ttlIndFin.GetLastError(), usTerminalNo );
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAndReset()
//
//  PURPOSE :   read and reset the cashier total.
//
//  RETURN :    No return value.
//
//===========================================================================

BOOL CPCSampleView::ReadAndReset(
    CTtlCashier& ttlCashier,    // cashier total to read
    CParaCashier& paraCashier   // cashier parameter to read
    )
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    // --- get cashier no. to read ---
    ULONG  ulCashierNo;
    ulCashierNo = paraCashier.getCashierNo();

    BOOL    fSuccess;
    fSuccess = FALSE;

    // --- is the total able to reset ? ---
    if ( ttlCashier.IsResetOK( ulCashierNo ))
    {
        // --- reset the total ---
        if ( ttlCashier.Reset( ulCashierNo ))
        {
            // --- read saved daily total ---
            if ( ttlCashier.Read( CLASS_TTLSAVDAY, ulCashierNo ))
            {
				CParaMdcMem         mdcMem;

				mdcMem.Read();
				pDoc->m_discountRate.Read();
				pDoc->m_currencyRate.Read();
				pDoc->m_totalKeyData.Read();
				pDoc->m_transactionMnemonics.Read();

				// --- report header and body of total ---
                pDoc->ReportHeader( ttlCashier );
                pDoc->ReportTotal( ttlCashier, mdcMem);
                pDoc->ReportParameter( paraCashier );

                // --- set the total as able to reset ---
                fSuccess = ttlCashier.ResetAsOK( ulCashierNo );

                if ( ! fSuccess )
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_TTLISSRST, ttlCashier.GetLastError(), ulCashierNo );
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_TTLREAD, ttlCashier.GetLastError(), ulCashierNo );
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_RESET, ttlCashier.GetLastError(), ulCashierNo );
        }
    }
    else
    {
        // --- the total is not able to reset ---
        pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ISSUED, ttlCashier.GetLastError(), ulCashierNo );
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAndReset()
//
//  PURPOSE :   read and reset the PLU total.
//
//  RETURN :    No return value.
//
//  CTtlPlu&    ttlPlu,         // PLU total to read
//  const BOOL  fIsEndOfDay     // indicate called by EOD report
//===========================================================================

BOOL CPCSampleView::ReadAndReset(CTtlPlu & ttlPlu, const BOOL  fIsEndOfDay)
{
    CPCSampleDoc*   pDoc = GetDocument();

    BOOL    fSuccess;
    fSuccess = FALSE;

    // --- is the total able to reset ? ---
    if ( ttlPlu.IsResetOK())
    {
        // --- reset the total ---
        if ( ttlPlu.Reset( fIsEndOfDay ))
        {
            CParaFlexMem    paraFlexMem;

            // --- get flexible memory size in 2170 ---
            if ( paraFlexMem.Read())
            {
                USHORT   usDeptNo;
				USHORT   usMaxDeptNo;

                fSuccess     = TRUE;
                usDeptNo    = 1;
                usMaxDeptNo  = (USHORT)paraFlexMem.getMemorySize( FLEX_DEPT_ADR );

                BOOL    fHeaderPrinted;
                fHeaderPrinted = FALSE;

				if (pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
					CString fileName (pDoc->csSheetNamePrefix + _T("Totals_PLU.slk"));
					pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

					CString str (L"ID;PPCSample\r\n");
					pDoc->SetSpreadSheetString (str);

					// --- read all of resetted total ---
					pDoc->nSheetCol = 1;
					pDoc->nSheetRow = 1;

					str.Format(_T("C;Y%d;X%d;K\"PLU No.\"\r\nC;Y%d;X%d;K\"PLU Type\"\r\nC;Y%d;X%d;K\"PLU Name\"\r\nC;Y%d;X%d;K\"PLU Dept.\"\r\nC;Y%d;X%d;K\"PLU Count\"\r\nC;Y%d;X%d;K\"PLU Amount\"\r\n"),
						pDoc->nSheetRow, pDoc->nSheetCol,
						pDoc->nSheetRow, pDoc->nSheetCol + 1,
						pDoc->nSheetRow, pDoc->nSheetCol + 2,
						pDoc->nSheetRow, pDoc->nSheetCol + 3,
						pDoc->nSheetRow, pDoc->nSheetCol + 4,
						pDoc->nSheetRow, pDoc->nSheetCol + 5);
					pDoc->nSheetRow++;
				}

                // --- read all of PLU total by department# ---
                while ( fSuccess && ( usDeptNo <= usMaxDeptNo ))
                {
                    // --- read and report saved daily total ---
                    fSuccess = pcsample::ReadAllRecords( CLASS_TTLSAVDAY, ttlPlu, usDeptNo, fHeaderPrinted, pDoc );
                    usDeptNo++;
                }
				char *pHeader = "E\r\n";
				if (!FAILED(pDoc->hSheetFile.m_hFile)) pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

				if (!FAILED(pDoc->hSheetFile.m_hFile)) pDoc->hSheetFile.Close ();

                if ( fSuccess )
                {
                    // --- set the total as able to reset ---
                    fSuccess = ttlPlu.ResetAsOK();

                    if ( ! fSuccess )
                    {
                        // --- function is failed, and report its error ---
                        pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLISSRST, ttlPlu.GetLastError());
                    }
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraFlexMem.GetLastError());
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_RESET, ttlPlu.GetLastError());
        }
    }
    else
    {
        // --- the total is not able to reset ---
        pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_ISSUED, ttlPlu.GetLastError());
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAndReset()
//
//  PURPOSE :   read and reset the coupon total.
//
//  RETURN :    No return value.
//
//  CTtlCoupon& ttlCoupon,      // coupon total to read
//  const BOOL  fIsEndOfDay     // indicate called by EOD report
//===========================================================================
BOOL CPCSampleView::ReadAndReset(CTtlCoupon& ttlCoupon, const BOOL  fIsEndOfDay)
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    BOOL    fSuccess;
    fSuccess = FALSE;

    // --- is the total able to reset ? ---
    if ( ttlCoupon.IsResetOK())
    {
        // --- reset the total ---
        if ( ttlCoupon.Reset( fIsEndOfDay ))
        {
            // --- read and report saved daily total ---
            if ( pcsample::ReadAllRecords( CLASS_TTLSAVDAY, ttlCoupon, pDoc ))
            {
                // --- set the total as able to reset ---
                fSuccess = ttlCoupon.ResetAsOK();

                if ( ! fSuccess )
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( CLASS_TTLCPN, pcsample::ERR_TTLISSRST, ttlCoupon.GetLastError());
                }
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCPN, pcsample::ERR_RESET, ttlCoupon.GetLastError());
        }
    }
    else
    {
        // --- the total is not able to reset ---
        pDoc->ReportError( CLASS_TTLCPN, pcsample::ERR_ISSUED, ttlCoupon.GetLastError());
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAllRecords()
//
//  PURPOSE :   read all of existing ETK totals.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CPCSampleView::ReadAllRecords(
    const UCHAR uchClassToRead, // current or saved daily/PTD total
    CTtlETK&    ttlETK,         // ETK total to read
    const ULONG aulEmployeeID[],    // array of employee ID to read
    const USHORT    usNoOfEmployee  // no. of employees in aulEmployeeID
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) || ( uchClassToRead == CLASS_TTLSAVDAY ));
    ASSERT( usNoOfEmployee <= ETK_MAX_EMPLOYEE );
    ASSERT( ! IsBadReadPtr( aulEmployeeID, sizeof( ULONG ) * ETK_MAX_EMPLOYEE ));

    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    CParaETK    paraETK;

    // --- read all of existing employee records ---
    for ( USHORT usI = 0; usI < usNoOfEmployee; usI++ )
    {
        BOOL    fIsEndOfField;
        USHORT  usFieldNo;

        // --- read ETK total of first field ---
        usFieldNo = 0;

        if ( ttlETK.Read( uchClassToRead, aulEmployeeID[ usI ], usFieldNo, fIsEndOfField ))
        {
            // --- if first field is not empty, read next field ---
            // --- read ETK parameter ---
            if ( paraETK.Read( aulEmployeeID[ usI ] ))
            {
                // --- report body of ETK report ---
                pDoc->ReportTotal( ttlETK, paraETK );

                while ( ! fIsEndOfField )
                {
                    // --- read ETK total of next field ---
                    if ( ttlETK.Read( uchClassToRead, aulEmployeeID[ usI ], usFieldNo, fIsEndOfField ))
                    {
                        // --- report body of ETK report ---
                        pDoc->ReportTotal( ttlETK );
                    }
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_INDREAD, paraETK.GetLastError(), aulEmployeeID[ usI ] );
            }
        }
		else
		{
			pDoc->ReportError( CLASS_RPTEMPLOYEE, pcsample::ERR_NVRCLKDIN, paraETK.GetLastError(), aulEmployeeID[ usI ] );
		}
	}
}

//===========================================================================
//
//  FUNCTION :  CPCSampleView::ReadAllRecords()
//
//  PURPOSE :   read all of existing guest check totals.
//
//  RETURN :    No return value.
//
//===========================================================================

BOOL CPCSampleView::ReadAllRecords(
    CGuestCheck& ttlGC,         // guest check to read
    const ULONG ulCashierNo,   // cashier number to read
    const BOOL fIsResetOK       // indicate be able to reset after read
    )
{
    CPCSampleDoc*   pDoc;
    pDoc = GetDocument();

    BOOL    fSuccess;
    fSuccess = FALSE;

    // --- read guest check, if guest check file is not empty ---
    if ( ! ttlGC.IsEmpty())
    {
        USHORT  usNoOfGC = 0;
        USHORT* pausGCNo = new USHORT [ GCF_MAX_GCF_NUMBER ];

        // --- read all of existing guest check by cashier# ---
        if ( ttlGC.GetAllGCNoByOperator( ulCashierNo, pausGCNo, sizeof( USHORT ) * GCF_MAX_GCF_NUMBER, usNoOfGC ))
        {
            // --- report header of guest check report ---
            pDoc->ReportHeader( ttlGC, ulCashierNo );

            for ( USHORT usI = 0; usI < usNoOfGC; usI++ )
            {
                // --- read guest check by retrieved guest check# ---
                if ( ttlGC.Read( pausGCNo[ usI ] ))
                {
                    // --- report body of guest check report ---
                    pDoc->ReportTotal( ttlGC );

                    // --- set the total as able to reset ---
                    if ( fIsResetOK && ( ! ttlGC.Reset( pausGCNo[ usI ] )))
                    {
                        // --- function is failed, and report its error ---
                        pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_RESET, ttlGC.GetLastError(), pausGCNo[ usI ]);
                    }
                }
                else
                {
                    // --- function is failed, and report its error ---

                    pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_TTLREAD, ttlGC.GetLastError(), pausGCNo[ usI ]);
                }
            }

            fSuccess = TRUE;
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_ALLIDREAD, ttlGC.GetLastError());
        }

        // --- discard allocated buffer before exit function ---
        delete [] pausGCNo;
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_DNE, ttlGC.GetLastError());
    }

    return ( fSuccess );
}

//////////////// END OF FILE ( PCSampleView.cpp ) ///////////////////////////

void CPCSampleView::OnParaUpdatePlu() 
{
	CPluAssignUpdate dialogPluAssignUpdate;

	dialogPluAssignUpdate.DoModal ();
}

void CPCSampleView::OnBroadCastSupAc() 
{
	UCHAR uchNbFlag = NB_REQSUPER;

	IspReqBroadcast(uchNbFlag);
}

void CPCSampleView::OnBroadCastAllPAc() 
{
	UCHAR uchNbFlag = NB_REQALLPARA;

	IspReqBroadcast(uchNbFlag);
}
void CPCSampleView::OnBroadCastPluOepAc() 
{
	UCHAR uchNbFlag = NB_REQPLU;

	IspReqBroadcast(uchNbFlag);
}

void CPCSampleView::OnRetrieveSystemStatus() 
{
	USHORT   usSystemStatus;

	SerOpRetrieveSystemStatus(&usSystemStatus);
}

//===========================================================================
// FUNCTION:	CPCSampleView::OnWindowPrint()
// PURPOSE:	Print the content of the document to Windows printer
// RETURN:		No return value.
//
// 110831 
//===========================================================================
void CPCSampleView::OnWindowPrint()
{
    CPCSampleDoc *pDoc = GetDocument();

	ASSERT(pDoc);

	if (pDoc->m_listReportStr.IsEmpty() == TRUE)
		return;

	CPrintDialog *printDlg = new CPrintDialog(FALSE, PD_ALLPAGES | PD_RETURNDC, NULL);

    // Initialize some of the fields in PRINTDLG structure.
    printDlg->m_pd.nMinPage = printDlg->m_pd.nMaxPage = 1;
    printDlg->m_pd.nFromPage = printDlg->m_pd.nToPage = 1;

    // Display Windows print dialog box.
    printDlg->DoModal();

    // Obtain a handle to the device context.
    HDC hdcPrn = printDlg->GetPrinterDC();
    if (hdcPrn != NULL)
	{
		CDC *pDC = new CDC;
		pDC->Attach(hdcPrn);

		// Use fixed-width font
		CFont font;
		font.CreatePointFont(m_fontPointSize, _T("Courier New"), pDC);
		pDC->SelectObject(font);

		// Determine the spacing between lines
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		int yInc = tm.tmHeight + tm.tmExternalLeading + 2;

		int iLogPixelsHeight = GetDeviceCaps(hdcPrn, LOGPIXELSY);  // Number of pixels per logical inch along the screen height
		int iLogPixelsWidth = GetDeviceCaps(hdcPrn, LOGPIXELSX);   // Number of pixels per logical inch along the screen width.

		int iPageMarginLeft = pDoc->m_pageSetup.rtMargin.left;
		int iPageMarginTop = pDoc->m_pageSetup.rtMargin.top;

		int iPageHeight = GetDeviceCaps(hdcPrn, PHYSICALHEIGHT);     // For printing devices: the height of the physical page, in device units.
		int iPageWidth = GetDeviceCaps(hdcPrn, PHYSICALWIDTH);     // For printing devices: the height of the physical page, in device units.

		int iTopMargin = GetDeviceCaps(hdcPrn, PHYSICALOFFSETY);   // For printing devices: the distance from the top edge of the physical page to the top edge of the printable area,
		if (iTopMargin < iPageMarginTop) iTopMargin = iPageMarginTop;
		if (iTopMargin < iLogPixelsHeight / 3) iTopMargin = iLogPixelsHeight / 3;     // default is third of an inch of top margin

		int iLeftMargin = GetDeviceCaps(hdcPrn, PHYSICALOFFSETX);
		if (iLeftMargin < iPageMarginLeft) iLeftMargin = iPageMarginLeft;
		if (iLeftMargin < iLogPixelsWidth / 3) iLeftMargin = iLogPixelsWidth / 3;    // default is third of an inch of left margin

		int yPos = iTopMargin;
		int xPos = iLeftMargin;

		pDC->StartDoc(_T("PCSample"));

		POSITION  posRptList = pDoc->m_listReportList.GetHeadPosition();
		INT_PTR   rptListOffset = 0;
		POSITION pos = pDoc->m_listReportStr.GetHeadPosition();
		while (posRptList != NULL) {
			CPCSampleDoc::ListReportList sectionHeaderBegin = pDoc->m_listReportList.GetNext(posRptList);
			int  iPageNo = 1;
			int  iColNo = 1;
			CString  lineHeader;
			int  xPosRight = xPos + iPageWidth / 3;

			CPCSampleDoc::ListReportList sectionHeaderEnd;
			if (posRptList != NULL) {
				sectionHeaderEnd = pDoc->m_listReportList.GetNext(posRptList);
			} else {
				sectionHeaderEnd.m_ListOffset = 0x7fffffff;
			}

			lineHeader.Format (L"%s - Page %d", sectionHeaderBegin.m_SectionTitle, iPageNo);

			int  iLine = 0;
			while (pos != NULL)
			{
				if (yPos >= iPageHeight - (iTopMargin * 2))    // times 2 as page height - top margin - bottom margin.
				{
					do {
						if (sectionHeaderBegin.m_bTwoColumn) {
							if (iColNo == 1) {
								iColNo = 2;
								yPos = iTopMargin + yInc * 2;
								xPos = iLeftMargin + iPageWidth / 2;
								break;
							}
						}
						pDC->EndPage();
						iPageNo++;
						iColNo = 1;
						yPos = iTopMargin;
						xPos = iLeftMargin;
						pDC->StartPage();
						lineHeader.Format(L"%s - Page %d", sectionHeaderBegin.m_SectionTitle, iPageNo);
						pDC->TextOut(xPosRight, yPos, lineHeader);
						yPos += yInc * 2;
					} while (false);
				}
				else if (rptListOffset == sectionHeaderBegin.m_ListOffset)
				{
					pDC->EndPage();
					iPageNo = 1;
					iColNo = 1;
					yPos = iTopMargin;
					xPos = iLeftMargin;
					pDC->StartPage();
					lineHeader.Format(L"%s - Page %d", sectionHeaderBegin.m_SectionTitle, iPageNo);
					pDC->TextOut(xPosRight, yPos, lineHeader);
					yPos += yInc * 2;
				}
				else if (rptListOffset >= sectionHeaderEnd.m_ListOffset)
				{
					pDC->EndPage();
					iPageNo = 1;
					iColNo = 1;
					yPos = iTopMargin;
					xPos = iLeftMargin;
					pDC->StartPage();
					sectionHeaderBegin = sectionHeaderEnd;
					if (posRptList != NULL) {
						sectionHeaderEnd = pDoc->m_listReportList.GetNext(posRptList);
					} else {
						sectionHeaderBegin.m_ListOffset = 0x7fffffff;
						sectionHeaderEnd.m_ListOffset = 0x7fffffff;
					}
					lineHeader.Format(L"%s - Page %d", sectionHeaderBegin.m_SectionTitle, iPageNo);
					pDC->TextOut(xPosRight, yPos, lineHeader);
					yPos += yInc * 2;
				}
				CString line = (CString)(pDoc->m_listReportStr.GetNext(pos));
				pDC->TextOut(xPos, yPos, line);
				yPos += yInc;
				rptListOffset++;
			}
		}

        pDC->EndPage();
        pDC->EndDoc();
        pDC->Detach();
        delete pDC;
	}
	delete printDlg;
}

//===========================================================================
// FUNCTION:	CPCSampleView::OnWindowPrintLastReport()
// PURPOSE:	Print the content of the document to Windows printer
// RETURN:		No return value.
//
// 110831 
//===========================================================================
void CPCSampleView::OnWindowPrintLastReport()
{
    CPCSampleDoc *pDoc = GetDocument();

	ASSERT(pDoc);

	if (pDoc->m_listReportStr.IsEmpty() == TRUE)
		return;

	CPrintDialog *printDlg = new CPrintDialog(FALSE, PD_ALLPAGES | PD_RETURNDC, NULL);

    // Initialize some of the fields in PRINTDLG structure.
    printDlg->m_pd.nMinPage = printDlg->m_pd.nMaxPage = 1;
    printDlg->m_pd.nFromPage = printDlg->m_pd.nToPage = 1;

    // Display Windows print dialog box.
    printDlg->DoModal();

    // Obtain a handle to the device context.
    HDC hdcPrn = printDlg->GetPrinterDC();
    if (hdcPrn != NULL)
	{
		CDC *pDC = new CDC;
		pDC->Attach(hdcPrn);

		// Use fixed-width font
		CFont font;
		font.CreatePointFont(m_fontPointSize, _T("Courier New"), pDC);
		pDC->SelectObject(font);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		int yInc = tm.tmHeight + tm.tmExternalLeading + 2;

		int iLogPixelsHeight = GetDeviceCaps(hdcPrn, LOGPIXELSY);  // Number of pixels per logical inch along the screen height
		int iLogPixelsWidth = GetDeviceCaps(hdcPrn, LOGPIXELSX);   // Number of pixels per logical inch along the screen width.

		int iPageMarginLeft = pDoc->m_pageSetup.rtMargin.left;
		int iPageMarginTop = pDoc->m_pageSetup.rtMargin.top;

		int iPageHeight = GetDeviceCaps(hdcPrn, PHYSICALHEIGHT);     // For printing devices: the height of the physical page, in device units.
		int iPageWidth = GetDeviceCaps(hdcPrn, PHYSICALWIDTH);     // For printing devices: the height of the physical page, in device units.

		int iTopMargin = GetDeviceCaps(hdcPrn, PHYSICALOFFSETY);   // For printing devices: the distance from the top edge of the physical page to the top edge of the printable area,
		if (iTopMargin < iLogPixelsHeight / 2) iTopMargin = iLogPixelsHeight / 2;     // default is half an inch of top margin
		int iLeftMargin = GetDeviceCaps(hdcPrn, PHYSICALOFFSETX);
		if (iLeftMargin < iLogPixelsWidth / 2) iLeftMargin = iLogPixelsWidth / 2;    // default is half an inch of left margin

		int yPos = iTopMargin;
		int xPos = iLeftMargin;

		POSITION pos = pDoc->m_listReportStr.GetHeadPosition();
		pDC->StartDoc(_T("PCSample"));
		int  iLine = 0;
		while (pos != NULL)
		{
			if (yPos >= iPageHeight - iTopMargin)
			{
				pDC->EndPage();
				yPos = iTopMargin;
				pDC->StartPage();
			}
			CString line = (CString)(pDoc->m_listReportStr.GetNext(pos));
			pDC->TextOut(xPos, yPos, line);
			yPos += yInc;
		};

        pDC->EndPage();
        pDC->EndDoc();
        pDC->Detach();
        delete pDC;
	}
	delete printDlg;
}

void CPCSampleView::OnConnectEngine()
{
	static RPropertySheet *mainSheet = NULL;
	if (mainSheet == NULL)
	{
		mainSheet = new RPropertySheet(_T("Connect Engine"));
		RPageConnect *pageConnect = new RPageConnect;
		RPageCashierAction *pageCA = new RPageCashierAction;
		RPageEmployeeChange *pageEC = new RPageEmployeeChange;
		RPageGuestCheck *pageGC = new RPageGuestCheck;

		mainSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW;
		mainSheet->AddPage(pageConnect);
		mainSheet->AddPage(pageCA);
		mainSheet->AddPage(pageEC);
		mainSheet->AddPage(pageGC);
		CWnd *mainWindow = ::AfxGetMainWnd();
		mainSheet->Create(mainWindow, WS_POPUP|WS_CAPTION);
	}
	if (mainSheet != NULL)
	{
		m_bConnectEngineWindowShown = !m_bConnectEngineWindowShown;
		int swShow = m_bConnectEngineWindowShown == true ? SW_SHOW : SW_HIDE;
		mainSheet->ShowWindow(swShow);
	}
}

void CPCSampleView::OnUpdateWindowConnectengine(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_bConnectEngineWindowShown);
}

void CPCSampleView::OnWindowAlwaysontop()
{
	this->m_bAlwaysOnTop = !this->m_bAlwaysOnTop;
	CWnd *mainWindow = ::AfxGetMainWnd();
	const CWnd *winPos = this->m_bAlwaysOnTop == true ? &wndTopMost : &wndNoTopMost;
	mainWindow->SetWindowPos(winPos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CPCSampleView::OnUpdateWindowAlwaysontop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_bAlwaysOnTop);
}

// Save what is displayed in the PCSample view to a log file.
// You can do the following with this functionality.
//  - if there is nothing displayed in the view then the file is truncated
//  - if there is something displayed in the view then what is there is appended to the file
//
// You can clear the file by doing a save to file after clearing the view.
// You can pick particular things to save to the file by doing a clear followed
// by performing a particular command or report.
//
// More work to do later such as allowing a file name to be chosen.
void CPCSampleView::OnSaveLogToFile()
{
	CPCSampleDoc *pDoc = GetDocument();

	int nCount = pDoc->GetNoOfReportStrings();

	CString csText(_T("Nothing to save to output file pcsamplelog.log"));

	if (nCount > 0) {
		FILE *hFile = fopen ("pcsamplelog.log", "a");
		if (hFile) {
			POSITION  myPos = pDoc->m_listReportStr.GetHeadPosition();
			while (myPos) {
				fprintf (hFile, "%-64.64S\n", (LPCWSTR)pDoc->m_listReportStr.GetNext(myPos));
			}
			fclose (hFile);
			csText = _T("PCSample screen log saved to file pcsamplelog.log");
		} else {
			csText = _T("WARNING: Unable to open output file pcsamplelog.log for writing.");
		}
	} else {
		FILE *hFile = fopen ("pcsamplelog.log", "w");
		if (hFile) {
			fclose (hFile);
			csText = _T("PCSample screen log pcsamplelog.log truncated.");
		}
	}
	AfxMessageBox (csText);
}

void CPCSampleView::OnProvCashier()
{
	CDiaCashierAddEdit myDialog;

	myDialog.LoadListControl ();
	myDialog.DoModal ();
}


LRESULT CPCSampleView::OnWindowSetSection(WPARAM  wParam, LPARAM  lParam)
{
	CPCSampleDoc *pDoc = GetDocument();

	CPoint  ptOrigin;
	ptOrigin = GetScrollPosition();

	wParam *= m_sizeCell.cy;
	ptOrigin.y = wParam;

	ScrollToPosition(ptOrigin);

	this->UpdateWindow();

	return 0;
}

void CPCSampleView::OnDisplayReportList ()
{
	CPCSampleDoc *pDoc = GetDocument();

#if 0
	CDialogDlg myDialog(IDD_DIALOG_REPORTLIST, this, std::function<void(CDataExchange*)> ([pDoc](CDataExchange* pDX) {
		if (pDX->m_bSaveAndValidate) {
		}
		else {
			POSITION pos = NULL;

			do {
				CPCSampleDoc::ListReportList sectionHeader;
				CListBox *x = static_cast<CListBox *>(pDX->m_pDlgWnd->GetDlgItem(IDC_LIST1));

				pos = pDoc->GetReportSectionHeader(pos, sectionHeader);
				x->AddString(sectionHeader.m_SectionTitle);
			} while (pos);
		}
	}));

	myDialog.SetOnOkCallBack(std::function<void(CWnd *)> ([pDoc](CWnd *dlgWnd) {
		CListBox *x = static_cast<CListBox *>(dlgWnd->GetDlgItem(IDC_LIST1));
		int iPtr = x->GetCurSel();
		POSITION pos = NULL;

		do {
			CPCSampleDoc::ListReportList sectionHeader;

			pos = pDoc->GetReportSectionHeader(pos, sectionHeader);
			if (iPtr < 1) {
				pDoc->MoveToReportSectionHeader(sectionHeader.m_ListOffset);
				break;
			}
			iPtr--;
		} while (pos);
	}));

#else
	CDialogDlg myDialog(IDD_DIALOG_REPORTLIST, pDoc, this, [](CDataExchange* pDX, CDocument *pDoc) {
		if (pDX->m_bSaveAndValidate) {
		}
		else {
			CPCSampleDoc *pDocDoc = static_cast<CPCSampleDoc *>(pDoc);
			POSITION pos = NULL;

			do {
				CPCSampleDoc::ListReportList sectionHeader;
				CListBox *x = static_cast<CListBox *>(pDX->m_pDlgWnd->GetDlgItem(IDC_LIST1));

				pos = pDocDoc->GetReportSectionHeader(pos, sectionHeader);
				x->AddString(sectionHeader.m_SectionTitle);
			} while (pos);
		}
	});

	myDialog.SetOnOkCallBack([](CWnd *dlgWnd, CDocument *pDoc) {
		CPCSampleDoc *pDocDoc = static_cast<CPCSampleDoc *>(pDoc);
		CListBox *x = static_cast<CListBox *>(dlgWnd->GetDlgItem(IDC_LIST1));
		int iPtr = x->GetCurSel();
		POSITION pos = NULL;

		do {
			CPCSampleDoc::ListReportList sectionHeader;

			pos = pDocDoc->GetReportSectionHeader(pos, sectionHeader);
			if (iPtr < 1) {
				pDocDoc->MoveToReportSectionHeader(sectionHeader.m_ListOffset);
				break;
			}
			iPtr--;
		} while (pos);
	});
#endif

	myDialog.DoModal();

}

int ScanForMaster(ULONG *pulList, int nListSize)
{
	INT   iSocket;

	/* --- open a socket --- */
	if ((iSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
		return 0;
	}

	// Bind the socket to any address and the specified port.
	int iResult = 0;
	struct sockaddr_in RecvAddr;

	unsigned short Port = 2172;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(iSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult != 0) {
		return 0;
	}

	ULONG *pulListStart = pulList;

	for (int iCount = 0; iCount < 3; iCount++) {
		/* --- wait for a message --- */
		struct sockaddr_in  sinClient = { 0 };
		INT                 iClient = sizeof(sinClient);
		SHORT               sRead;
		CHAR                uchRecvBuffer[512] = { 0 };

		if ((sRead = recvfrom(iSocket, uchRecvBuffer, sizeof(uchRecvBuffer), 0, (struct sockaddr *)&sinClient, &iClient)) > 0) {
			ULONG *pul;
			for (pul = pulListStart; pul < pulList; pul++) {
				if (*pul == sinClient.sin_addr.S_un.S_addr) break;
			}
			if (pul >= pulList) *pulList++ = sinClient.sin_addr.S_un.S_addr;
		}

		/* --- critical error ? --- */
		if (sRead < 0)  break;
	}

	closesocket(iSocket);

	return pulList - pulListStart;
}

void CPCSampleView::OnScanForMaster()
{
	TCHAR buffer[128] = { 0 };

	CDialogDlg myDialog(IDD_DIALOG_REPORTLIST, this, std::function<void(CDataExchange*)>([](CDataExchange* pDX) {
		if (pDX->m_bSaveAndValidate) {
		}
		else {
			ULONG ipList[20] = { 0 };

			int iCount = ScanForMaster(ipList, 20);

			CListBox *x = static_cast<CListBox *>(pDX->m_pDlgWnd->GetDlgItem(IDC_LIST1));
			for (int i = 0; i < iCount; i++) {
				wchar_t buffer[128] = { 0 };
				UCHAR  ipAdr1 = (ipList[i] >> 24) & 0xff;
				UCHAR  ipAdr2 = (ipList[i] >> 16) & 0xff;
				UCHAR  ipAdr3 = (ipList[i] >> 8) & 0xff;
				UCHAR  ipAdr4 = (ipList[i] & 0xff);
				wsprintf(buffer, L"%d.%d.%d.%d", ipAdr4, ipAdr3, ipAdr2, ipAdr1);
				x->AddString(buffer);
			}
		}
	}));

	
	myDialog.SetOnOkCallBack(std::function<void(CWnd *, void*)>([](CWnd *dlgWnd, void *pData) {
		CListBox *x = static_cast<CListBox *>(dlgWnd->GetDlgItem(IDC_LIST1));
		int iPtr = x->GetCurSel();

		// if there is a selected item and a data area to put the selected data
		// then lets fetch the selected item and save it into the data area.
		if (iPtr >= 0 && pData) {
			x->GetText(iPtr, (TCHAR *)pData);
		}
	}), (void *)buffer);

	myDialog.DoModal();

	if (buffer[0]) {
		UCHAR  ucAddrs[4] = { 0 }, *pAddrs = ucAddrs;

		for (TCHAR *pChar = buffer; *pChar && pAddrs < ucAddrs + 4; pChar++) {
			if (*pChar >= '0' && *pChar <= '9') {
				*pAddrs *= 10;
				*pAddrs += *pChar - '0';
			}
			else {
				pAddrs++;
			}
		}
	}
}
