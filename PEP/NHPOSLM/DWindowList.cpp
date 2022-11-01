// DWindowList.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayoutDoc.h"
#include "NewLayout.h"
#include "DWindowList.h"
#include "DWinItem.h"
#include "NewLayoutView.h"
#include "LeftView.h"
#include "ChildFrm.h"


#include "WindowItemExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDWindowList dialog


CDWindowList::CDWindowList(CWnd* pParent /*=NULL*/)
	: CDialog(CDWindowList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDWindowList)
	//}}AFX_DATA_INIT
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	myView = pChild->GetRightPane();
	pDoc = myView->GetDocument();
	forWinBtn = FALSE;

}


void CDWindowList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDWindowList)
	DDX_Control(pDX, IDC_DIALOG5_SHOWPROP, m_PropButton);
	DDX_Control(pDX, IDC_TREE1, m_DocControlTree);
	//}}AFX_DATA_MAP


	if (pDX->m_bSaveAndValidate) {
		if(!forWinBtn){
			myView->uiActiveID = uiActiveID;
		}
	}
	else {
		uiActiveID = myView->uiActiveID;
	}

}


BEGIN_MESSAGE_MAP(CDWindowList, CDialog)
	//{{AFX_MSG_MAP(CDWindowList)
	ON_BN_CLICKED(IDC_DIALOG5_SHOWPROP, OnDialog5Showprop)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWindowList message handlers

void CDWindowList::OnOK() 
{
	// TODO: Add extra validation here
	CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
	myTree->ShowSelectedWindow(uiActiveID);
	
	CDialog::OnOK();
}

void CDWindowList::OnDialog5Showprop() 
{
	// TODO: Add your control notification handler code here
	
	CWindowItemExt *pWI = GetSelectedWin(uiActiveID);
	if(!pWI){
		return;
	}
	
	CDWinItem jjdb (pWI);
	jjdb.defFlag = pWI->controlAttributes.useDefault;
	if (jjdb.DoModal () == IDOK) {

		CRect windowRect(jjdb.m_nColumn,
						 jjdb.m_nRow,
						 jjdb.m_nColumn + jjdb.m_usWidthMultiplier,
						 jjdb.m_nRow + jjdb.m_usHeightMultiplier);

		if (!pDoc->ValidateWindowDoc(windowRect)) {
			AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
			return;
		}
		//if pContianer isnt Null, this is a sub window, verify it doesnt extend beyond parents boundarys
		if(pWI->pContainer){
			CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl(pWI->pContainer);
			if(!pDoc->ValidateWindow(windowRect,parent)){
				AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
				return;
			}
		}

		pWI->myCaption = jjdb.m_csCaption;
		_tcsncpy_s(pWI->controlAttributes.m_myCaption, pWI->myCaption, MaxSizeOfMyCaption);
		pWI->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

		pWI->myName = jjdb.m_csName;
		_tcsncpy_s(pWI->controlAttributes.m_myName, pWI->myName, MaxSizeOfMyCaption);
		pWI->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

		pWI->controlAttributes.m_colorText = jjdb.m_colorText;
		pWI->controlAttributes.m_colorFace = jjdb.m_colorFace;
		pWI->controlAttributes.m_nRow = jjdb.m_nRow;
		pWI->controlAttributes.m_nColumn = jjdb.m_nColumn;
		pWI->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
		pWI->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
		pWI->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
		pWI->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
		pWI->controlAttributes.isVirtualWindow = jjdb.m_nVirtual;
		pWI->controlAttributes.useDefault = jjdb.defFlag;
		pWI->windowAttributes.GroupNum = jjdb.m_nGroup;
		pWI->windowAttributes.busRules.NumOfPicks  = jjdb.NumPicks;
		for (int i = 0; i < sizeof(pWI->controlAttributes.SpecWin); i++){
			pWI->controlAttributes.SpecWin[i] = jjdb.m_csSpecWin[i];
		}

		CLeftView* myTree = (CLeftView*)(myView->m_TreeView);
		myTree->UpdateTreeLabel(pWI);
	}

}



BOOL CDWindowList::OnInitDialog() 
{
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();

	CWindowDocumentExt * myDoc = myView->GetDocument();
	m_DocControlTree.ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );
	CString caption;
	CString temp;

	POSITION currentPos = myDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = myDoc->listControls.GetNext (currentPos);

		temp.LoadString(IDS_WINITEM);
		caption.Format(_T("%s %s"),temp,pwc->myName);
		HTREEITEM DocWin = m_DocControlTree.InsertItem(TVIF_TEXT | TVIF_PARAM,
													   caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, NULL, NULL);
		
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);
		SetTreeItem(wi,DocWin);

	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDWindowList::SetTreeItem(CWindowItem *wi, HTREEITEM ti)
{

	CString caption;
	CString temp;

	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl* wc = wi->ButtonItemList.GetNext (currentPos);
		if(wc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			temp.LoadString(IDS_SUBWINITEM);
			caption.Format(_T("%s %s"),temp,wc->myName);
			HTREEITEM WinControl = m_DocControlTree.InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, ti, TVI_LAST);
	
			
			CWindowItemExt *pwi = CWindowItemExt::TranslateCWindowControl(wc);
			SetTreeItem(pwi, WinControl);
		}
	}
}
	
CWindowItemExt* CDWindowList::GetSelectedWin(UINT id)
{
	CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
	myTree->ShowSelectedWindow(uiActiveID);

	myView->ClearSelection((CWindowControl*)pDoc);

	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = pDoc->listControls.GetNext (currentPos);
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);

		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			if(wi->controlAttributes.m_myId == id){
				return wi;
			}
			else{
				if(GetSelectedSubWin(wi,id) != NULL){
					return GetSelectedSubWin(wi,id);
				}
			}
		}
		
	}
	return NULL;
}

CWindowItemExt* CDWindowList::GetSelectedSubWin(CWindowItemExt *wi, UINT id)
{
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	while (currentPos) {
		CWindowControl *pwc = wi->ButtonItemList.GetNext(currentPos);
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			CWindowItemExt *ww = CWindowItemExt::TranslateCWindowControl(pwc);
			if(pwc->controlAttributes.m_myId == id){
				return CWindowItemExt::TranslateCWindowControl(pwc);
			}
			else{
				if(GetSelectedSubWin(ww, id) != NULL){
					return GetSelectedSubWin(ww, id);
				}
			}
		}
	}
	return NULL;

}

void CDWindowList::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	pNMTreeView->itemNew.lParam;
	// TODO: Add your control notification handler code here
	uiActiveID = m_DocControlTree.GetItemData(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}
