// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "NewLayout.h"

#include "ChildFrm.h"
#include "NewLayoutView.h"
#include "NewLayoutDoc.h"
#include "LeftView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_VIEW_RULERS, OnViewRulers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RULERS, OnUpdateViewRulers)
	ON_WM_SIZE()
	ON_COMMAND(ID_TOGGLE_RULER, OnViewRulers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	ResLoWidth = 800;		//indicates the width in pixels
	ResLoHeight = 600;	//indicates the height in pixels
	m_bShowRulers = TRUE;

	m_wndSplitter1 = 0;
	m_wndSplitter2 = 0;
	m_wndSplitter3 = 0;
	m_wndSplitter4 = 0;
}

CChildFrame::~CChildFrame()
{
	if (m_wndSplitter1) {
		delete m_wndSplitter1;
		m_wndSplitter1 = 0;
	}
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	m_wndSplitter1 = new CLOMSplitterWnd();

	m_wndSplitter1->Create(this,RUNTIME_CLASS(CLeftView),NULL,pContext,TRUE);
	m_wndSplitter2 = m_wndSplitter1->AddSubDivision(RIGHT_SIDE,NULL,NULL,pContext,FALSE);
	m_wndSplitter3 = m_wndSplitter2->AddSubDivision(TOP_SIDE,RUNTIME_CLASS(CRulerCornerView),RUNTIME_CLASS(CRulerView),pContext,TRUE);
	m_wndSplitter4 = m_wndSplitter2->AddSubDivision(BOTTOM_SIDE,RUNTIME_CLASS(CRulerView),RUNTIME_CLASS(CNewLayoutView),pContext,TRUE);
	// create splitter window
/*	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CNewLayoutView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}*/
	//Add scroll bars for our tree view
	m_wndSplitter1->GetPane(0,0)->ModifyStyle(NULL, WS_HSCROLL|WS_VSCROLL ,0);
	m_wndSplitter1->SetColumnInfo(0,200,200);
	m_wndSplitter2->SetRowInfo(0,25,25);
	m_wndSplitter3->SetColumnInfo(0,25,25);

	//set the types for our 2 different ruler views so we know which is which
	GetViews();
	CRulerView* horRuler = ((CRulerView*)m_wndSplitter3->GetPane(0, 1));
	horRuler->SetRulerType(RT_HORIZONTAL);	
	
	CRulerView* vertRuler = ((CRulerView*)m_wndSplitter4->GetPane(0, 0));
	vertRuler->SetRulerType(RT_VERTICAL);
    
	//set the active pane to the one contianing the layout view
    m_wndSplitter4->SetActivePane(0, 1);

	GetWindowPlacement(&wp);

	return TRUE;

}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
//function to access the layout view
CNewLayoutView* CChildFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter4->GetPane(0, 1);
	CNewLayoutView* pView = DYNAMIC_DOWNCAST(CNewLayoutView, pWnd);
	return pView;
}


// This sets loads the correct resources for all NEW child windows created 
BOOL CChildFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	HMENU m_hMenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_NEWLAYTYPE));
    m_hMenuShared = m_hMenu;

	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}

/*This function changes the menu resources for all exisitng child windows when a new language
resource dll is loaded. If we dont do this, all windows created before the language change
will revert back to the original language once the onces created after the change are destroyed*/

void CChildFrame::ChangeMenu()
{
	
	HMENU m_hChildMenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_NEWLAYTYPE));
	ASSERT(m_hChildMenu);

	SetMenu(NULL);
	::DestroyMenu(m_hMenuShared);

	CMDIFrameWnd* frame = GetMDIFrame();;
	frame->MDISetMenu(CMenu::FromHandle(m_hChildMenu),NULL);
	m_hMenuShared = m_hChildMenu;
	frame->DrawMenuBar();

}

//get our layout and tree views and assign to member variables of the view for later access
void CChildFrame::GetViews()
{
	//get the right view 
	CWnd* pWnd = m_wndSplitter4->GetPane(0, 1);

	CNewLayoutView* list = DYNAMIC_DOWNCAST(CNewLayoutView, pWnd);
	//get the left view (the tree view)
	pWnd = m_wndSplitter1->GetPane(0, 0);
	CLeftView * tree = DYNAMIC_DOWNCAST(CLeftView, pWnd);

	tree->m_LayoutView=list;
	list->m_TreeView=tree; 

}

/*this function is called from the UpdateRulersInfo in the layout view class when ever a 
scroll action or mouse move has taken place. This function then calls the appropirate function
in the ruler view class for updating the ruler.*/
void CChildFrame::UpdateRulersInfo(stRULER_INFO stRulerInfo)
{
    m_wndSplitter3->UpdateRulersInfo(stRulerInfo, RT_HORIZONTAL);//1
	m_wndSplitter4->UpdateRulersInfo(stRulerInfo, RT_VERTICAL);//0
}

//command handler to hide/show the rulers
void CChildFrame::OnViewRulers() 
{
	m_bShowRulers = !m_bShowRulers;
    m_wndSplitter2->ShowRulers(m_bShowRulers,TRUE);
	m_wndSplitter4->ShowRulers(m_bShowRulers,TRUE);
}

//update the menu after the user selects to show/hide the rulers
void CChildFrame::OnUpdateViewRulers(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CString label;
	if(m_bShowRulers){
		label.LoadString(IDS_HIDE_RULERS);
		pCmdUI->SetText(label);
	}
	else{
		label.LoadString(IDS_SHOW_RULERS);
		pCmdUI->SetText(label);
	}
	
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(nType == SIZE_MAXIMIZED){
		
		CNewLayoutView *pView = GetRightPane();
		

		CRect myRect(0,0,800,600);
		pView->GetClientRect(&myRect);

		stRULER_INFO pRulerInfo;
		pRulerInfo.uMessage    = RW_POSITION;
		pRulerInfo.ScrollPos   = pView->GetScrollPosition();
		pRulerInfo.Pos         = NULL;
		pRulerInfo.DocSize     = myRect.Size();
		pRulerInfo.fZoomFactor = 1;
    
		UpdateRulersInfo(pRulerInfo);
		Invalidate(TRUE);
	}
}
