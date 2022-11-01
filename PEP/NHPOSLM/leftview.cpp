#include "stdafx.h"
#include "NewLayout.h"
#include "LeftView.h"
#include "WindowItemExt.h"
#include <afxcview.h>
#include "ChildFrm.h"
#include "WindowButtonExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView
IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

CLeftView::CLeftView()
{
	this->m_dwDefaultStyle |= (TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS);
}

CLeftView::~CLeftView() {}

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelectionChange)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLeftView::OnDraw(CDC* pDC) {}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CWindowDocumentExt* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewLayoutDoc)));
	return (&((CNewLayoutDoc*)m_pDocument)->myWindowDocument);
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CLeftView::OnInitialUpdate()
//RETURN VALUE - void
//PARAMETERS - none
//DESCRIPTION - This function is called upon the creation of a new document.
//				This function populates this views tree control with windows
//				contianed in the document being loaded
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::OnInitialUpdate() 
{
	TRACE("CLeftView::OnInitialUpdate\n");
	CTreeView::OnInitialUpdate();	
	
	// TODO: Add your specialized code here and/or call the base class
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	
	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//Clear out everyhting in the tree
	VERIFY(myTree->DeleteAllItems());

	CString caption;
	CString tempA;
	CString tempB;
	
	//cycle through all the documents windows
	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	
	while (currentPos) {
		//get the current window in the list
		CWindowControl *pwc = pDoc->listControls.GetNext (currentPos);
		
		//Set the caption variable according to the name of the window
		/*tempA.LoadString(IDS_WINITEM);*/
		tempB.LoadString(IDS_GROUPID);
		caption.Format(_T("%s :: %s %d"), pwc->myName,tempB, (CWindowItemExt::TranslateCWindowControl(pwc))->windowAttributes.GroupNum);
		//insert this window into the tree control
		HTREEITEM DocWin = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
													   caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, NULL, NULL);
		
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);
		
		//call function to insert and display text controls and buttons as children of DocWin
		SetControlTextItem(wi,DocWin);	
	}	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION SetControlTextItem(CWindowItem *wi, HTREEITEM ti)
//RETURN VALUE - void
//PARAMETERS -	wi - Window item to check for subwindows
//				ti - parent tree item to insert subwindows under
//DESCRIPTION - This function is called to set insert child items (text controls 
//				and buttons) into the control tree upon opening a saved file
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::SetControlTextItem(CWindowItem *wi, HTREEITEM ti)
{
	TRACE("CLeftView::SetControlTextItem\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0);

	CString caption;
	CString tempA;
	CString tempB;

	/*cycle through all CWindowItem *wi's controls 
	if wi's control list is empty, function does not add 
	any new items to the control tree*/
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	
	caption.Format(_T("Text Controls"));
	//insert this the label "Text Controls" into the tree controla
	HTREEITEM DocWinTextControls = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
													   caption, 0, 0, 0, 0, wi->controlAttributes.m_myId, ti, NULL);
	caption.Format(_T("Buttons"));
	//insert this the label "Buttons" into the tree control
	HTREEITEM DocWinButtons = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
													   caption, 0, 0, 0, 0, wi->controlAttributes.m_myId, ti, NULL);
	caption.Format(_T("Labels"));
	//insert this the label "Buttons" into the tree control
	HTREEITEM DocWinLabels = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
													   caption, 0, 0, 0, 0, wi->controlAttributes.m_myId, ti, NULL);

	//this while loop loops through all controls associated with the window, and if it is a text control
	//it inserts it under text controls, else, it inserts it under button controls, or labels under labels.
	while (currentPos) {
		//get the current control in the windows control list
		CWindowControl* wc = wi->ButtonItemList.GetNext (currentPos);
		//is the control a TextControl, if not its a button
		if(wc->controlAttributes.m_nType == CWindowControl::TextControl){

			
			//Set the caption variable according to the type of the text control
			tempB.LoadString((CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type) + IDS_TXT_SING_RCT_MAIN);
			tempA.LoadString(IDS_CMDLINE_LANG_NF);
			caption.Format(_T("%s"), tempB);

			//insert the text control under the text control label tree if the caption is not blank
			if (caption != "" && caption != tempA){
			HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, DocWinTextControls, TVI_LAST);
			}
		}
		else if (wc->controlAttributes.m_nType == CWindowControl::ButtonControl)
		{

			caption.Format(_T("%s"), wc->myCaption);
			//insert the button under the button label tree
			HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, DocWinButtons, TVI_LAST);	
			
		}
		else
		{
			caption.Format(_T("%s"), wc->myCaption);
			//insert the button under the button label tree
			HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, DocWinLabels, TVI_LAST);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION InsertNewTextControl(UINT id, CWindowControl* pwc)
//RETURN VALUE - void
//PARAMETERS -  id - id of the parent window of the new window. 0 if top level
//				pwc - new window to add to the tree
//DESCRIPTION - This function is called to set insert child items (text controls 
//				and buttons) into the control tree on the fly.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::InsertNewTextControl(UINT id, CWindowControl* pwc)
{
	TRACE("CLeftView::InsertNewTextControl\n");

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo Removed since redundant - myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	CString caption;
	CString tempA;
	CString tempB;

	//get the root of the tree
	HTREEITEM hCurrent = myTree->GetRootItem();
	//call function to find the parent of the window so we insert the controls under the correct tree item
	HTREEITEM parent = GetTreeItem(id,hCurrent);

	CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition();
	POSITION  currentPosLast = currentPos;
	CWindowControl* wc = wi->ButtonItemList.GetAt (currentPos);

	while (currentPos) {
		//This boolean value is used to determine if there is a item with the same name already under the text controls or buttons list
		//We use this because we do not want items to be printed on the tree more than once.
		BOOL textExists = FALSE;

		wc = wi->ButtonItemList.GetNext (currentPos);
		//is the control a TextControl
		if(wc->controlAttributes.m_nType == CWindowControl::TextControl){
			//creating tree control for the label text control
			HTREEITEM control = myTree->GetChildItem(parent);
			//creating tree subControl for the items under text control
			HTREEITEM subControl = myTree->GetChildItem(control);
			//Set the caption variable according to the type of the text control
			tempB.LoadString((CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type) + IDS_TXT_SING_RCT_MAIN);
	
			caption.Format(_T("%s"), tempB);
			
			//cycling through the items under text control to see if there is already a name there
			while (subControl != NULL)
			{
				if (myTree->GetItemData(subControl) == wc->controlAttributes.m_myId)
					textExists = TRUE;
				subControl = myTree->GetNextItem(subControl, TVGN_NEXT);
			}
			
			//we only insert the item if it has not already been inserted and the caption is not blank
			if (!textExists && caption != ""){
				//insert the text control under the text control label tree
				HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
										    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, control, TVI_LAST);

			}
		}
		//is this control a button
		else if (wc->controlAttributes.m_nType != CWindowControl::LabelControl)
		{
			HTREEITEM control = myTree->GetChildItem(parent);
			control = myTree->GetNextItem(control, TVGN_NEXT);
			HTREEITEM subControl = myTree->GetChildItem(control);
			caption.Format(_T("%s"), wc->myCaption);
		
			//cycling through the items under text control to see if there is already a name there
			while (subControl != NULL)
			{
				if (myTree->GetItemData(subControl) == wc->controlAttributes.m_myId)
					textExists = TRUE;
				subControl = myTree->GetNextItem(subControl, TVGN_NEXT);
			}
			
			//we only insert the item if it has not already been inserted and the caption is not blank
			if (!textExists && caption != "")
			{
				//insert the text control under the text control label tree
				HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
										    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, control, TVI_LAST);
			}
		}
		else //is this control a label
		{
			HTREEITEM control = myTree->GetChildItem(parent);
			control = myTree->GetNextItem(control, TVGN_NEXT);
			control = myTree->GetNextItem(control, TVGN_NEXT);
			HTREEITEM subControl = myTree->GetChildItem(control);
			caption.Format(_T("%s"), wc->myCaption);
		
			//cycling through the items under text control to see if there is already a name there
			while (subControl != NULL)
			{
				if (myTree->GetItemData(subControl) == wc->controlAttributes.m_myId)
					textExists = TRUE;
				subControl = myTree->GetNextItem(subControl, TVGN_NEXT);
			}
			
			//we only insert the item if it has not already been inserted and the caption is not blank
			if (!textExists && caption != "")
			{
				//insert the text control under the text control label tree
				HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
										    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, control, TVI_LAST);
			}

		}
	}//end while loop

	// A-KSo Make sure treeview is redrawn
	this->RedrawWindow();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CLeftView::SetTreeItem(CWindowItem *wi, HTREEITEM ti)
//RETURN VALUE - void
//PARAMETERS -	wi - Window item to check for subwindows
//				ti - parent tree item to insert subwindows under
//DESCRIPTION - This function is called to set insert child items (subwindows)
//				into the control tree. This function may be called recursively
//				if a subwindow contains subwindows within itself
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::SetTreeItem(CWindowItem *wi, HTREEITEM ti)
{
	TRACE("CLeftView::SetTreeItem\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	CString caption;
	CString tempA;
	CString tempB;

	/*cycle through all CWindowItem *wi's controls 
	if wi's control list is empty, function does not add 
	any new items to the control tree*/
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		//get the current control in the windows control list
		CWindowControl* wc = wi->ButtonItemList.GetNext (currentPos);
		//is the control a WindowItem
		if(wc->controlAttributes.m_nType == CWindowControl::WindowContainer){

			//Set the caption variable according to the name of the window
			//tempA.LoadString(IDS_SUBWINITEM);
			tempB.LoadString(IDS_GROUPID);
			caption.Format(_T("%s :: %s %d"), wc->myName, tempB, (CWindowItemExt::TranslateCWindowControl(wc))->windowAttributes.GroupNum);
			//insert this window into the tree control as a child of ti
			HTREEITEM WinControl = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, wc->controlAttributes.m_myId, ti, TVI_LAST);
			
			CWindowItemExt *pwi = CWindowItemExt::TranslateCWindowControl(wc);
			/*call function recursively to insert and display subwindow 
			items of pwi as children of WinCOntrol*/
			SetTreeItem(pwi, WinControl);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CLeftView::ShowSelectedWindow(UINT id)
//RETURN VALUE - void
//PARAMETERS - id - id of the window that has been selected
//DESCRIPTION - function is called each time the user selects a window. We 
//				cycle through the documents window list and determine if we 
//				have found the selected window. If so, we set the 
//				appropriate properties and call function the hide all the sub 
//				windows. If window was not selected we determine if a subwindow 
//				of that window was selected and if so
//				we hide the parent window. If not we display the parent window 
//				but dont select it and hide all its subwindows
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::ShowSelectedWindow(UINT id)
{
	TRACE("CLeftView::ShowSelectedWindow\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	//Clear all selections for the entire document - new window is being selected
	m_LayoutView->ClearSelection((CWindowControl *)(pDoc));
	
	//cycle through all the windows in the documents window list
	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;


	//cycle while there are still window items in the list
	while (currentPos) {
		//get the current window in the documents window list and cast it to CWindowItemExt
		CWindowControl *pwc = pDoc->listControls.GetNext (currentPos);
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);

		BOOL  bFoundWindow = FALSE;
		POSITION  currentPos2 = wi->ButtonItemList.GetHeadPosition ();
		POSITION  currentPosLast2 = currentPos2;

		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			// Look to see if this window item has the same ID as what was selected.
			// If so, we have found the window and we process the select accordingly.
			// If the current window does not have the same ID, then look to see if any
			// of this window's itesm (text controls and buttons) have the same ID as
			// the ID we are looking for.  If so, then this is the window we need to select.
			
			bFoundWindow = (pwc->controlAttributes.m_myId == id);
			while (currentPos2 && !bFoundWindow)
			{	
			    pwc = wi->ButtonItemList.GetNext (currentPos2);			
				bFoundWindow = (pwc->controlAttributes.m_myId == id);
			}
			
			//this window was not the one selected
			if (bFoundWindow) {
				//call function that will hide all sub windows of this window
				//HideWindowItem(id,wi);
				//set properties for this window to be shown and selected
				m_LayoutView->uiActiveID = wi->controlAttributes.m_myId;
				wi->windowAttributes.Show = TRUE;				
				wi->controlAttributes.Selected = TRUE;
				POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
				while (currentPos) {
					//get the next control
					CWindowControl *pwc = wi->ButtonItemList.GetNext (currentPos);
					if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl ){
						TRACE0("  Button Found %d\n");
					}
				}
			}
			else{
				/*
				//was a subwindow of this window or one of its childs subwindows selected
				if(ShowSelectedSubWindow(wi,id)){
					//we hide this window if a subwindow was selected
					wi->windowExtAttributes.Show = FALSE;
				}
				else{
					//we show the top level window if no subwindows were selected
					wi->windowExtAttributes.Show = TRUE;
				}*/
				//this window wasnt selected so set its selected property
 				wi->windowAttributes.Show = FALSE;
				wi->controlAttributes.Selected = FALSE;
				//if(!ShowSelectedSubWindow(wi,id)){
					//call function that will hide all sub windows of this window 
				//	HideWindowItem(id,wi);
			//	}
			}
		}
		
	}//end while 
	m_LayoutView->InvalidateRect(NULL,TRUE);
	return;
	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CLeftView::ShowSelectedSubWindow(UINT id)
//RETURN VALUE - BOOL
//PARAMETERS - id - id of the window that has been selected
//			   wi - parent window to check for selected subwindow
//DESCRIPTION - function searches parent window for subwindows and checks to 
//				see if a subwindow was selected. If it was, True is returned, 
//				if not this function is recursively called until no more 
//				subwindows are left to check. If now subwindow was ever found 
//				to be selected False is returned
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL CLeftView::ShowSelectedSubWindow(CWindowItemExt *wi,UINT id)
{/*
	//cycle through windows control list 
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	//cycle while there are still controls in the list
	while (currentPos) {
		// get the current control in the list
		CWindowControl *pwc = wi->ButtonItemList.GetNext(currentPos);
		//is this control a window item
	//	if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			//cast control to type CWindowItemExt
			CWindowItemExt *ww = CWindowItemExt::TranslateCWindowControl(pwc);
			//is this window the one that was selected
			if(pwc->controlAttributes.m_myId == id){
				//window is the one that was selected - return true
				ww->windowAttributes.Show = TRUE;
				ww->controlAttributes.Selected = TRUE;
				//return TRUE;
			}
			else{
				//a subwindow of this window was found  - return true
				if(ShowSelectedSubWindow(ww, id)){
					return TRUE;
				}
	
				ww->windowAttributes.Show = FALSE;
				ww->controlAttributes.Selected = FALSE;
				ShowSelectedSubWindow(ww,id);
			
		//	}
		}
	}
	//selected window was not found - return false
	return FALSE; */
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//																				   
//FUNCTION CLeftView::OnSelectionChange(NMHDR* pNMHDR, LRESULT* pResult)
//RETURN VALUE - void
//DESCRIPTION - function is called each time the user clicks on an item in the 
//				tree control. The active window id is set according to the 
//				users selection. This function then calls  ShowSelectedWindow 
//				in order to update the screen and display and activate the 
//				chosen window
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::OnSelectionChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE("CLeftView::OnSelectionChange\n");
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	//get the ID of the selected window chosen from the tree control
	UINT selection = GetTreeCtrl().GetItemData(pNMTreeView->itemNew.hItem);
	//set the LayoutView's active ID to what was selected
	m_LayoutView->uiActiveID = selection;
	//function call to show the selected window
	ShowSelectedWindow(selection);
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::InsertNewWinItem(UINT id, CWindowControl* pwc)
//RETURN VALUE - void
//PARAMETERS -  id - id of the parent window of the new window. 0 if top level
//				pwc - new window to add to the tree
//DESCRIPTION - function is called each time the user clicks on an item in the 
//				tree control. The active window id is set according to the 
//				users selection. This function then calls  ShowSelectedWindow 
//				in order to update the screen and display and activate the 
//				chosen window
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::InsertNewWinItem(UINT id, CWindowControl* pwc)
{
	TRACE("CLeftView::InsertNewWinItem\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	CString caption;
	CString tempA;
	CString tempB;
	//Set the caption variable according to the name of the window
	//caption.Format(_T("Window Item - %s :: Group ID - %d"),pwc->myName,(CWindowItemExt::TranslateCWindowControl(pwc))->GroupNum);
	//tempA.LoadString(IDS_WINITEM);
	tempB.LoadString(IDS_GROUPID);
	caption.Format(_T("%s :: %s %d"), pwc->myName, tempB, (CWindowItemExt::TranslateCWindowControl(pwc))->windowAttributes.GroupNum);
	//is the window a top level window
	HTREEITEM newItem;
	if(id==0){
		//insert window item at the top level in the tree
		newItem = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM, caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, NULL, NULL);
			
		caption.Format(_T("Text Controls"));
		
		//insert this the label "Text Controls" into the tree controla
		HTREEITEM DocWinTextControls = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
														   caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, newItem, NULL);
		caption.Format(_T("Buttons"));
		//insert this the label "Buttons" into the tree control
		HTREEITEM DocWinButtons = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
														   caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, newItem, NULL);
		caption.Format(_T("Labels"));
		//insert this the label "Buttons" into the tree control
		HTREEITEM DocWinLabels = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
														   caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, newItem, NULL);
	}
	//is the window a sub window item
	else{
		//Set the caption variable according to the name of the window
		//tempA.LoadString(IDS_SUBWINITEM);
		tempB.LoadString(IDS_GROUPID);
		caption.Format(_T("%s :: %s %d"), pwc->myName, tempB, (CWindowItemExt::TranslateCWindowControl(pwc))->windowAttributes.GroupNum);
		//get the root of the tree
		HTREEITEM hCurrent = myTree->GetRootItem();
		//call function to find the parent of the new window so we insert it under the correct tree item
		HTREEITEM parent = InsertChildItem(id,hCurrent);
		//insert the new window at the correct place
		HTREEITEM newItem = myTree->InsertItem(TVIF_TEXT | TVIF_PARAM,
												    	caption, 0, 0, 0, 0, pwc->controlAttributes.m_myId, parent, TVI_LAST);
	}
	this->InvalidateRect(NULL,TRUE);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::DeleteTreeItem(UINT id)
//RETURN VALUE - void
//PARAMETERS -  id - id of the window item we want to remove from the tree
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to remove and removes it. if no matching tree 
//				item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::DeleteTreeItem(UINT id)
{
	TRACE("CLeftView::DeleteTreeItem\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//get the root of the tree
	HTREEITEM hCurrent = myTree->GetRootItem();
	
	//continue while there are still items in the tree
	while(hCurrent != NULL){
		//is this tree item the one we want to delete 
		if(myTree->GetItemData(hCurrent)==id){
			//delete from tree and exit function
			myTree->DeleteItem(hCurrent);
			return;
		}
		//this is not the tree item we want to delete
		else{
			//does this tree item have child items
			if(myTree->ItemHasChildren(hCurrent)){
				//search for and get the sub tree item we want to delete
				HTREEITEM child = GetTreeItem(id,myTree->GetChildItem(hCurrent));
				//tree item was found
				if(child != NULL){
					//remove it from the tree control and exit function
					myTree->DeleteItem(child);
					return;
				}
				//child items exist but none were the one we were looking for
				else{
					//get the next top level tree item
					hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
				}
			}
			//tree item has no children 
			else{
				//get the next top level tree item
				hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
			}
		}
	}
	//no matching tree item was found - alert user
	AfxMessageBox(IDS_WIN_NF, MB_OK, 0);
	return;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::DeleteControlTreeItem(UINT id)
//RETURN VALUE - void
//PARAMETERS -  id - id of the control item we want to remove from the tree
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to remove and removes it. if no matching tree 
//				control item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::DeleteControlTreeItem(UINT id)
{
	TRACE("CLeftView::DeleteControlTreeItem\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//cycle through all the documents windows
	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	
	//get the root of the tree
	HTREEITEM hCurrent = myTree->GetRootItem();
	//call function to find the parent of the window so we find the controls under the correct tree item
	HTREEITEM subControl = GetTreeItem(id,hCurrent);
	
	//cycle through controls until we find the control that
	while (hCurrent != NULL){
		//continue while there are still items in the tree
		while(subControl != NULL){
			//is this tree item the one we want to delete 
			if(myTree->GetItemData(subControl)==id){
				//delete from tree and exit function
				myTree->DeleteItem(subControl);
				return;
			}
			subControl = myTree->GetNextItem(subControl, TVGN_NEXT);
		}//end of 2nd while loop
		hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
	}//end of first while

	//no matching tree item was found - alert user
	AfxMessageBox(IDS_WIN_NF, MB_OK, 0);
	return;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::GetTreeItem(UINT id, HTREEITEM hCurrent)
//RETURN VALUE - HTREEITEM
//PARAMETERS -  id - id of the window item we want to find
//				hCurrent - child tree item we want to check
//DESCRIPTION - this function brings in a child tree item. We check to see if
//				it is the tree item we are looking for. If it is, we return it,
//				if not we check to see if it has children and recursively call 
//				this function. If it has no children we continue searching siblings
//				on this level
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetTreeItem(UINT id, HTREEITEM hCurrent)
{
	//TRACE("CLeftView::GetTreeItem\n");
	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//continue while there are still siblings
	while(hCurrent != NULL){
		//is this tree item we are looking for
		if(myTree->GetItemData(hCurrent)==id){
			//return this tree item 
			return hCurrent;
		}
		//this is not the tree item we are looking for
		else{
			//does this tree item have children
			if(myTree->ItemHasChildren(hCurrent)){
				//recursive function call to search for item in this items children
				HTREEITEM child = GetTreeItem(id,myTree->GetChildItem(hCurrent));
				//tree item found in children
				if(child != NULL){
					//return the found tree item
					return child;
				}
				//tree item not found in children - get the next sibling
				else{
					hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
				}
			}
			//get the next sibling item
			else{
				hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
			}
		}
	}
	//item not found - return NULL
	return NULL;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::InsertChildItem(UINT id, HTREEITEM hCurrent)
//RETURN VALUE - HTREEITEM
//PARAMETERS -  id - id of the parent window of the new window. 
//				hCurrent - parent tree item
//DESCRIPTION - function is called to find the correct tree item to add the new
//				window item under. (Find its parent). Can be called recursively
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::InsertChildItem(UINT id, HTREEITEM hCurrent)
{
	TRACE("CLeftView::InsertChildItem\n");

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//continue while there are still items in the tree
	while(hCurrent != NULL){
		/*does the ID of the window corresponding to this tree 
		item match (is the the parent of our new window)*/
		if(myTree->GetItemData(hCurrent)==id){
			//return this tree item as the parent
			return hCurrent;
		}
		/*ID of the window corresponding to this tree 
		item does not matchmatch */
		else{
			//does this tree item have children
			if(myTree->ItemHasChildren(hCurrent)){
				//recursively call this function to search for parent
				HTREEITEM child = InsertChildItem(id,myTree->GetChildItem(hCurrent));
				//was a parent found
				if(child != NULL){
					//return tree item found to be parent
					return child;
				}
				///////////////////////////////
				//no parent was found in the chil windows
				else{
					//get the next tree item on this level
					hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
				}
				/////////////////////////////////
			}
			//tree item has no children
			else{
				//get the next tree item on this level
				hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
			}
		}
	}
	//no parent found - return hCurrent which will be NULL
	return hCurrent;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CLeftView::HideWindowItem(UINT id, CWindowItemExt *wi)
//RETURN VALUE - void
//PARAMETERS - id - id of the window that has been selected
//			   wi - parent window calling this function
//DESCRIPTION - function is called to hide/show and select/deselect subwindow items. A parent window
//				is passed in and we cycle through its controls and if we find a window item, we check 
//				to see if it is the selected window or not and set its properties accordingly and then 
//				recursively call this function until no more subwindows are found
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeftView::HideWindowItem(UINT id, CWindowItemExt *wi)
{
	TRACE("CLeftView::HideWindowItem\n");
	//cycle through the windows controls
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	//keep going while there are still controls in the list
	while (currentPos) {
		//get the next control
		CWindowControl *pwc = wi->ButtonItemList.GetNext (currentPos);
		
		//is the control a window item
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			//cast the controls type to CWindowItemExt
			CWindowItemExt *pwi = CWindowItemExt::TranslateCWindowControl(pwc);
			//is this window the one that was selected
			if(pwi->controlAttributes.m_myId == id){
				//set appropriate properties and call function to hide subwindows
				pwi->windowAttributes.Show = TRUE;
				pwi->controlAttributes.Selected = TRUE;
				HideWindowItem(id,pwi);
			}
			//window is not the one selected
			else{
				//set appropriate properties and call function to check if subwindow was selected
				pwi->windowAttributes.Show = FALSE;
				pwi->controlAttributes.Selected = FALSE;
				HideWindowItem(id,pwi);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::UpdateTreeLabel(UINT id)
//RETURN VALUE - void
//PARAMETERS -  id - id of the window item we want to update the label for
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to update and updates it. if no matching tree 
//				item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::UpdateTreeLabel(CWindowItemExt* pwc)
{
	TRACE("CLeftView::UpdateTreeLabel\n");

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	
	//Create a CString to hold the caption
	CString caption = _T("");
	CString tempA = _T("");
	CString tempB = _T("");

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//get the root of the tree
	HTREEITEM hCurrent = myTree->GetRootItem();
	
	//continue while there are still items in the tree
	while(hCurrent != NULL){
		//is this tree item the one we want to update 
		if(myTree->GetItemData(hCurrent) == pwc->controlAttributes.m_myId){
			//update label and  exit function
			//caption.Format(_T("Window Item - %s :: Group ID - %d"),pwc->myName,(CWindowItemExt::TranslateCWindowControl(pwc))->GroupNum);
			//tempA.LoadString(IDS_WINITEM);
			tempB.LoadString(IDS_GROUPID);
			caption.Format(_T("%s :: %s %d"), pwc->myName, tempB, (CWindowItemExt::TranslateCWindowControl(pwc))->windowAttributes.GroupNum);
			myTree->SetItemText(hCurrent,caption);
			return;
		}
		//this is not the tree item we want to update
		else{
			//does this tree item have child items
			if(myTree->ItemHasChildren(hCurrent)){
				//search for and get the sub tree item we want to update
				HTREEITEM child = GetTreeItem(pwc->controlAttributes.m_myId,myTree->GetChildItem(hCurrent));
				//tree item was found
				if(child != NULL){
					//update label and  exit function
					//tempA.LoadString(IDS_WINITEM);
					tempB.LoadString(IDS_GROUPID);
					caption.Format(_T("%s :: %s %d"), pwc->myName, tempB, (CWindowItemExt::TranslateCWindowControl(pwc))->windowAttributes.GroupNum);
					myTree->SetItemText(child,caption);
					return;
				}
				//child items exist but none were the one we were looking for
				else{
					//get the next top level tree item
					hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
				}
			}
			//tree item has no children 
			else{
				//get the next top level tree item
				hCurrent = myTree->GetNextItem(hCurrent, TVGN_NEXT);
			}
		}
	}
	//no matching tree item was found - alert user
	AfxMessageBox(IDS_WIN_NF, MB_OK, 0);
	return;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::UpdateTextControlTree(CWindowTextExt *pb)
//RETURN VALUE - void
//PARAMETERS -  pb - text control that we are modifying
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to update and updates it. if no matching tree 
//				item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::UpdateTextControlTree(CWindowTextExt *pb)
{
	TRACE("CLeftView::UpdateTextControlTree\n");
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	
	//Create a CString to hold the caption
	CString caption = _T("");
	CString tempA = _T("");

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//get the root of the tree
	HTREEITEM parent = myTree->GetRootItem();
	while(parent != NULL){
		//creating tree control for the label text control
		HTREEITEM control = myTree->GetChildItem(parent);
		//creating tree subControl for the items under text control
		HTREEITEM subControl = myTree->GetChildItem(control);
		
		while (subControl != NULL)
		{
			if (myTree->GetItemData(subControl) == pb->controlAttributes.m_myId)
			{
				tempA.LoadString(pb->textAttributes.type + IDS_TXT_SING_RCT_MAIN);
				caption.Format(_T("%s"),tempA);
				myTree->SetItemText(subControl,caption);
			}
			subControl = myTree->GetNextItem(subControl, TVGN_NEXT);	
		
		}
		parent = myTree->GetNextItem(parent, TVGN_NEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::UpdateButtonControlTree(CWindowButtonExt* pb)
//RETURN VALUE - void
//PARAMETERS -  pb - button that we are modifying
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to update and updates it. if no matching tree 
//				item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::UpdateButtonControlTree(CWindowButtonExt* pb)
{	
	TRACE("CLeftView::UpdateButtonControlTree\n");

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	
	//Create a CString to hold the caption
	CString caption = _T("");
	CString tempA = _T("");

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//get the root of the tree
	HTREEITEM parent = myTree->GetRootItem();

	while(parent != NULL)
	{
		//creating tree control for the label text control
		HTREEITEM control = myTree->GetChildItem(parent);
		//gettings the items under the button tree
		control = myTree->GetNextItem(control, TVGN_NEXT);
		//creating tree subControl for the items under the button tree
		HTREEITEM subControl = myTree->GetChildItem(control);
		
		while (subControl != NULL)
		{
			myTree->SetItemState (subControl, 0, TVIS_SELECTED);
			if (myTree->GetItemData(subControl) == pb->controlAttributes.m_myId)
			{
				caption.Format(_T("%s"), pb->myCaption);
				myTree->SetItemText(subControl,caption);
				myTree->SetItemState (subControl, TVIS_SELECTED, TVIS_SELECTED);
			}
			subControl = myTree->GetNextItem(subControl, TVGN_NEXT);	
		}
		parent = myTree->GetNextItem(parent, TVGN_NEXT);
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																			   
//FUNCTION CLeftView::UpdateLabelControlTree(CWindowLabelExt* pl)
//RETURN VALUE - void
//PARAMETERS -  pl - label that we are modifying
//DESCRIPTION - this function traverses our tree control looking for the 
//				item we want to update and updates it. if no matching tree 
//				item was found, we diplsy an error message to the user
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CLeftView::UpdateLabelControlTree(CWindowLabelExt* pl)
{
	TRACE("CLeftView::UpdateLabelControlTree\n");

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);
	
	//Create a CString to hold the caption
	CString caption = _T("");
	CString tempA = _T("");

	//get this views Tree Control object and then modify its syle
	CTreeCtrl *myTree = &GetTreeCtrl();
	//R-KSo myTree->ModifyStyle(NULL,TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0 );

	//get the root of the tree
	HTREEITEM parent = myTree->GetRootItem();

	while(parent != NULL)
	{
		//creating tree control for the label text control
		HTREEITEM control = myTree->GetChildItem(parent);
		control = myTree->GetNextItem(control, TVGN_NEXT);
		//getting the items under the label tree
		control = myTree->GetNextItem(control, TVGN_NEXT);
		//creating tree subControl for the items under the label tree 
		HTREEITEM subControl = myTree->GetChildItem(control);
		while (subControl != NULL)
		{
			if (myTree->GetItemData(subControl) == pl->controlAttributes.m_myId)
			{
				caption.Format(_T("%s"), pl->myCaption);
				myTree->SetItemText(subControl,caption);
			}
			subControl = myTree->GetNextItem(subControl, TVGN_NEXT);	
		}
		parent = myTree->GetNextItem(parent, TVGN_NEXT);
	}
}

void CLeftView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	TRACE("CLeftView::OnRButtonDown\n");

	// TODO: Add your message handler code here and/or call default
	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = m_LayoutView->GetDocument();
	ASSERT_VALID(pDoc);

    UINT uFlags;
    HTREEITEM htItem = GetTreeCtrl().HitTest(point, &uFlags);
    if ((htItem != NULL) && (uFlags & TVHT_ONITEM)) {
            GetTreeCtrl().Select(htItem, TVGN_CARET);
			UINT m_nRCSelect = GetTreeCtrl().GetItemData(htItem);

			//set the LayoutView's active ID to what was selected
			m_LayoutView->uiActiveID = m_nRCSelect;
			//function call to show the selected window
			ShowSelectedWindow(m_nRCSelect);
			
			CWindowItemExt *pWI = pDoc->GetSelectedDocWindow(m_nRCSelect);
			
			
			/*I removed this assert statement simply because the purpose of this 
			assert statement was to determine if a window had been selected. At the time this assert was 
			put in, windows were the only items that were suppose to be in the left pane view.
			Now, there are windows, text controls, buttons, and labels in here. If you were to right
			click in debug mode on a text control, button, or label, you would receive this assert because
			it would not be a valid window. Therefore, I removed the following assert. Scott 10/1/2004
			*/

			//ASSERT(pWI);
			
			if(pWI == NULL){
				return;
			}

			m_LayoutView->cpLastRButton.x = pWI->controlAttributes.m_nColumn * CWindowButtonExt::stdWidth;
			m_LayoutView->cpLastRButton.y = pWI->controlAttributes.m_nRow * CWindowButtonExt::stdWidth;

			//popup menu creation
			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_TREE_MENU));
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			
			CRect rect;
			GetWindowRect(rect);
			//display menu and track selection of menu items
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
								   (point.x ) + rect.left, 
								   (point.y ) + rect.top, 
								   m_LayoutView);
			CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
			if (!m_pParent)
				return ;
			//set the active view back to layout view or dialog will not recieve mouse clicks
			m_pParent->SetActiveView((CView*)m_LayoutView,TRUE);
    }

	CTreeView::OnRButtonDown(nFlags, point);
}

BOOL CLeftView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	/*
		All messages that are sent to an application must be handled
		by the application or windows will create a default procedure 
		that will respond to the message that was sent. For the
		WM_MOUSEWHEEL, the default procedure that was being implemented
		when there where no items in the left view caused an error to 
		occur. To ensure this error does not occur we inform windows that 
		we have handled the message by returning TRUE from our 
		implementation of the handler for the WM_MOUSEWHEEL message.
		This will elliviate the error by telling windows that we have 
		have handled the message. ESMITH
	*/
	return TRUE;
}
