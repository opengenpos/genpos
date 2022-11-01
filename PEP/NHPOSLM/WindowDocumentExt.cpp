// WindowDocumentExt.cpp : implementation file
//

#include "stdafx.h"
#include "WindowDocumentExt.h"
#include "WindowControlSampleExt.h"

#include "WindowButtonExt.h"
#include "WindowItemExt.h"

#include "NewLayout.h"
#include "LeftView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CWindowDocumentExt,CWnd);

BEGIN_MESSAGE_MAP(CWindowDocumentExt, CWnd)
	//{{AFX_MSG_MAP(CWindowDocumentExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowDocumentExt message handlers
/////////////////////////////////////////////////////////////////////////////
// CWindowDocumentExt

CWindowDocumentExt::CWindowDocumentExt(void) : CWindowDocument ()
{
	documentAttributes.Version = ((CNewLayoutApp*)AfxGetApp())->SerializationVersion;
	m_undoLevels = 10;
	m_growsize = /*32768*/UNDO_MEMFILE_GROWSIZE;
	m_chkpt = 0;
	bModified = FALSE;
	
}

void CWindowDocumentExt::PopupDefaultDlg()
{
	DefaultEdit DefDlg;
	
	
	DefDlg.ButtonDefault = DocumentDef.defaultAttributes.ButtonDefault;
	DefDlg.WindowDefault = DocumentDef.defaultAttributes.WindowDefault;
	DefDlg.TextWinDefault = DocumentDef.defaultAttributes.TextWinDefault;

	DefDlg.FontDefault = DocumentDef.defaultAttributes.FontDefault;
	DefDlg.FontColorDefault = DocumentDef.defaultAttributes.FontColorDefault;

	DefDlg.ShapeDefault = DocumentDef.defaultAttributes.ShapeDefault;
	DefDlg.PatternDefault = DocumentDef.defaultAttributes.PatternDefault;

	DefDlg.WidthDefault = DocumentDef.defaultAttributes.WidthDefault;
	DefDlg.HeightDefault = DocumentDef.defaultAttributes.HeightDefault;

	if(DefDlg.DoModal() == IDOK){

		DocumentDef.defaultAttributes.ButtonDefault = DefDlg.ButtonDefault;
		DocumentDef.defaultAttributes.WindowDefault = DefDlg.WindowDefault;
		DocumentDef.defaultAttributes.TextWinDefault = DefDlg.TextWinDefault;
		
		DocumentDef.defaultAttributes.FontDefault = DefDlg.FontDefault;
		DocumentDef.defaultAttributes.FontColorDefault = DefDlg.FontColorDefault;

		DocumentDef.defaultAttributes.ShapeDefault = DefDlg.ShapeDefault;
		DocumentDef.defaultAttributes.PatternDefault = DefDlg.PatternDefault;
		DocumentDef.defaultAttributes.WidthDefault = DefDlg.WidthDefault;
		DocumentDef.defaultAttributes.HeightDefault = DefDlg.HeightDefault;
	}

}

CWindowDocumentExt::~CWindowDocumentExt(void)
{
	POSITION currentPos = listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = listControls.GetNext (currentPos);
		delete pwc;
		listControls.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}

    // Clear undo list
    POSITION pos = m_undolist.GetHeadPosition(); 
    CMemFile  *nextFile = NULL;
    while(pos) {
        nextFile = (CMemFile *) m_undolist.GetNext(pos);
        delete nextFile;
    }
    m_undolist.RemoveAll();

    // Clear redo list
    ClearRedoList();
}

void CWindowDocumentExt::ClearDocument (void)
{
	POSITION currentPos = listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = listControls.GetNext (currentPos);
		delete pwc;
		listControls.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}

}


BOOL CWindowDocumentExt::CreateDocument (CWnd *pParentWnd, UINT nID, CRect *rectSize)
{
	BOOL bStatus = FALSE;
	CString  strMyClass;

	try
	{
	   strMyClass = AfxRegisterWndClass(
		  CS_VREDRAW | CS_HREDRAW,
		  ::LoadCursor(NULL, IDC_ARROW),
		  (HBRUSH) ::GetStockObject(WHITE_BRUSH),
		  ::LoadIcon(NULL, IDI_APPLICATION));
	}
	catch (CResourceException* pEx)
	{
		  AfxMessageBox(IDS_REG_ERR,MB_OK);
		  pEx->Delete();
	}

	if (pParentWnd) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_SYSMENU | WS_CAPTION |WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
		CRect myRect(100, 100, 0, 0);
		if (rectSize) {
			myRect = *rectSize;
		}
		else {
			myRect.right = GetSystemMetrics (SM_CXFULLSCREEN/*SM_CXSCREEN*/);
			myRect.bottom = GetSystemMetrics (SM_CXFULLSCREEN/*SM_CYSCREEN*/);
		}
		bStatus = Create (strMyClass, _T(""), dwStyle, myRect, pParentWnd, nID);
	}
	else {
		DWORD dwStyle = WS_VISIBLE |WS_POPUP ;
		DWORD dwStyleExt = WS_EX_LEFT | WS_EX_LTRREADING;
		CRect myRect (0, 0, 0, 0);
		if (rectSize) {
			myRect = *rectSize;
		}
		else {
			myRect.right = GetSystemMetrics (SM_CXFULLSCREEN/*SM_CXSCREEN*/);
			myRect.bottom = GetSystemMetrics (SM_CXFULLSCREEN/*SM_CYSCREEN*/);
		}
		
		bStatus = CreateEx (dwStyleExt, strMyClass, _T(""), dwStyle, myRect, NULL, 0);
		
	}
	
	
	CWindowControl *pListControl;
	POSITION currentPos = listControls.GetHeadPosition ();
	while (currentPos) {
		pListControl = listControls.GetNext (currentPos);

		if (pListControl->controlAttributes.m_nType /*!*/== CWindowControl::WindowContainer ) {
			pListControl->WindowCreate (this);
		}
		else if (pListControl->controlAttributes.m_myId == 1) {
			pListControl->WindowCreate (this);
		}
	}
	AfxMessageBox(IDS_RETLOM,  MB_OK,  0 );
	InvalidateRect (NULL, TRUE);
	this->BringWindowToTop();
	
	return bStatus;
}


void CWindowDocumentExt::Serialize( CArchive& ar )
{

	UCHAR* pBuff = (UCHAR*) &documentAttributes;

	if (ar.IsStoring())
	{
		CWindowDocument::Serialize(ar);
	}
	else
	{
		//variables to hold the count of items in each of the documents lists
		int nCount;
		int nGPCount;
		ULONG mySize;
		memset(&documentAttributes,0,sizeof(_tagWinDocAttributes));

		DocumentDef.Serialize(ar);
		ar>>mySize;

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (documentAttributes)) {
			xMySize = sizeof (documentAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}

		//get the count of the documents control list
		ar >> nCount;
		CWindowControlSampleExt pwc;
		//loop through all the stored controls and create them and add them to the doc's list
		for ( ; nCount > 0; nCount--) {
			CWindowControl *pb = pwc.SerializeNew (ar);
			listControls.AddTail (pb);
		}

		ASSERT(documentAttributes.signatureStart = 0xABCDEF87);

		/*make sure that this list is empty - this is nescessary because a default group
		is created when the document is created. We must clear this out before opening 
		a saved document*/
		POSITION currentPos = listGroups.GetHeadPosition ();
		POSITION currentPosLast = currentPos;
		while (currentPos) {
			CWindowGroup *wg = listGroups.GetNext (currentPos);
			ASSERT (wg);
			delete wg;
			listGroups.RemoveAt (currentPosLast);
			currentPosLast = currentPos;
		}

		//get the number of items in the group list
		ar >> nGPCount;
		CWindowGroup *wg;
		//loop for the number of items in the list
		for( ; nGPCount > 0; nGPCount--){
			//create a new Window Group and retrieve the stored data
			wg = new CWindowGroup();
			wg->Serialize(ar);
			//add the window group to the documents list
			listGroups.AddTail(wg);
		}
		//copy   TCHAR to CSting  
		csSummary = documentAttributes.summary;
		csPEP = documentAttributes.PEPfile;
		csIconDir = documentAttributes.IconDir;

		VerifyVersion();

	}
}

int CWindowDocumentExt::SearchForItemCorner (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		CRect jj(bi->controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + 1, 
				 bi->controlAttributes.m_nRow + 1);
		if (jj.PtInRect (CPoint (column, row))) {
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
			if (pw && (pw->controlAttributes.m_myId == id)) {
				CWindowControl *pwc = pw->GetDataFirst ();
				if (pwc) {
					CRect kk(pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn, 
							 pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow, 
							 pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + 1, 
							 pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + 1);
					while (pwc) {
						if (kk.PtInRect (CPoint (column, row))) {
							return 4;
						}
						pwc = pw->GetDataNext ();
					}
				}
				return 3;
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::DeleteWndItem(int row, int column, UINT id)
//RETURN VALUE - BOOL
//ATTRIBUTES - row,column -- indicate the point that was clicked, 
//				id -- the ID of the current active 
//			   window
//DESCRIPTION - function checks to see if it can find a window item in the document control list
//				with the corresponding point and ID. If a window item is found but not with the 
//				correct ID, a function is called to find a subwindow with the correct ID if it 
//				exists. Once the correct window is found, the control clicked on is searched for 
//				and deleted. If no controls are in the window, the window is deleted
//Note: row indicates the y value and column indicates the x value of the point we are looking for 
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::DeleteWndItem(int row, int column, UINT id)
{
	CWindowControl *pCurScreenControl =0;
	
	//cycle through all items in the document's control list
	POSITION listPosition = listControls.GetHeadPosition();

	//point clicked in -- control we wish to delete should contain this point
	CPoint requestPoint(column, row);

	while(listPosition){
		pclListCurrent = listPosition;
		
		//current control we are checking
		pCurScreenControl = listControls.GetNext(listPosition);
		//build a rectangle with the current screen's area
		CRect curScreen(pCurScreenControl->controlAttributes.m_nColumn, 
					    pCurScreenControl->controlAttributes.m_nRow,
						pCurScreenControl->controlAttributes.m_nColumn + pCurScreenControl->controlAttributes.m_usWidthMultiplier,
						pCurScreenControl->controlAttributes.m_nRow + pCurScreenControl->controlAttributes.m_usHeightMultiplier);
		//check that the requested point is inside the current screens area
		if(curScreen.PtInRect(requestPoint)) {
			CWindowItemExt *pScreenItem = CWindowItemExt::TranslateCWindowControl(pCurScreenControl);

			//is current screen the item we are looking in?(WindowItem's ID should match the Active ID
			if(pScreenItem && (pScreenItem->controlAttributes.m_myId == id)){
				//call funtion to delete control/window
				BOOL deletion = DeleteWndControl(pScreenItem,row,column);
				
				//if no controls were deleted
				if(!deletion){
					// verify no controls left in window, if controls are left, we dont delete the window
					// if window item list is empty and window was selected for deletion, we delete it,
					//remove it from the documents window list and exit function
					if(!pScreenItem->GetDataCount()){
	
						CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
						ASSERT(pFrame);

						CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
						ASSERT(pChild);

						CNewLayoutView *myView = pChild->GetRightPane();
						ASSERT(myView);

						CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
						ASSERT(myTree);

						myTree->DeleteTreeItem(pScreenItem->controlAttributes.m_myId);

						delete pScreenItem;
						listControls.RemoveAt (pclListCurrent);
						CheckPoint();
						return TRUE;
					}
				}
				// a control was deleted in the DeleteWndControl function call, exit function
				else{
					CheckPoint();
					return deletion;
				}
			}
			//Check if window item found contianing point but it's ID is not the active ID
			if(pScreenItem && (pScreenItem->controlAttributes.m_myId != id)){
				BOOL deletion;
				//get the subwindow that contians the active id if it exists
				pScreenItem = GetSelectedWindow(pScreenItem, id);
				if(pScreenItem != NULL){
					//call funtion to delete control/window
					deletion = DeleteWndControl(pScreenItem,row,column);
				
					//if no controls were deleted
					if(!deletion){
						// verify no controls left in window, if controls are left, we dont delete the window
						// if window item list is empty and window was selected for deletion, we delete it,
						//remove it from its parents control list and exit function
						if(!pScreenItem->GetDataCount()){
							//get parent control
							CWindowControl* parent = pScreenItem->pContainer;
							ASSERT(parent);

							CWindowControl *bi;
							POSITION pos = CWindowItem::TranslateCWindowControl(parent)->ButtonItemList.GetHeadPosition();

							CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
							ASSERT(pFrame);

							CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
							ASSERT(pChild);

							//cycle thru parents control list to find list position and remove it from the list
							while (pos){
								pclListCurrent = pos;
								bi = CWindowItem::TranslateCWindowControl(parent)->ButtonItemList.GetNext (pos);
								if(bi->controlAttributes.m_myId == pScreenItem->controlAttributes.m_myId){
											
									CNewLayoutView *myView = pChild->GetRightPane();
									ASSERT(myView);

									CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
									ASSERT(myTree);

									myTree->DeleteTreeItem(pScreenItem->controlAttributes.m_myId);

									delete pScreenItem;
									CWindowItem::TranslateCWindowControl(parent)->ButtonItemList.RemoveAt(pclListCurrent);
									CheckPoint();
									break;
								}
							
							}
							//window removed -- exit function
							return TRUE;
						}
					}
					// a control was deleted in the DeleteWndControl function call, exit function
					else{
						CheckPoint();
						return deletion;
					}
				}
			}
		}
	}
	//nothing was deleted, exit function
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::DeleteWndControl(CWindowItemExt *pScreenItem, int row, int column)
//RETURN VALUE - BOOL
//ATTRIBUTES - pScreenItem - WIndowItem continaing the control to be deleted or the window to delete
//			   row,column -- indicate the point that was clicked, 
//DESCRIPTION - window item with the correct ID is passed to this function. Controls are checked 
//				against the point clicked. If a control is found in those points, it is deleted
//				if not, the window is deleted.
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::DeleteWndControl(CWindowItemExt *pScreenItem, int row, int column)
{	
	//point clicked in -- control we wish to delete should contain this point
	CPoint requestPoint(column, row);

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	ASSERT(pFrame);

	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	ASSERT(pChild);

	CNewLayoutView *myView = pChild->GetRightPane();
	ASSERT(myView);

	CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
	ASSERT(myTree);

	//Cycle through all items in the current screen
	CWindowControl *pCurScreenItem = pScreenItem->GetDataFirst();
	while(pCurScreenItem){
		//build a rectangle to hold the controls screen area - 
		CRect screenItemArea(0,0,0,0);
		switch (pCurScreenItem->controlAttributes.m_nType) {
			// sub-window
			case CWindowControl::WindowContainer:
				screenItemArea.SetRect(pCurScreenItem->controlAttributes.m_nColumn, 
									   pCurScreenItem->controlAttributes.m_nRow,
									   pCurScreenItem->controlAttributes.m_nColumn + pCurScreenItem->controlAttributes.m_usWidthMultiplier,
									   pCurScreenItem->controlAttributes.m_nRow + pCurScreenItem->controlAttributes.m_usHeightMultiplier);
				break;
			// button or label
			case CWindowControl::TextControl:
				// fall through
			case CWindowControl::ButtonControl:
				// fall through
			case CWindowControl::LabelControl:
				 screenItemArea.SetRect(pScreenItem->controlAttributes.m_nColumn + pCurScreenItem->controlAttributes.m_nColumn,
					pScreenItem->controlAttributes.m_nRow + pCurScreenItem->controlAttributes.m_nRow,
					pScreenItem->controlAttributes.m_nColumn + pCurScreenItem->controlAttributes.m_nColumn + pCurScreenItem->controlAttributes.m_usWidthMultiplier,
					pScreenItem->controlAttributes.m_nRow + pCurScreenItem->controlAttributes.m_nRow + pCurScreenItem->controlAttributes.m_usHeightMultiplier);
				break;
			default:
				TRACE1("   Possible ERROR (WindowDocumentExt::DeleteWndControl): pCurScreenItem->controlAttributes.m_nType = %d", pCurScreenItem->controlAttributes.m_nType);
				ASSERT(0);
				break;
		}
		//check that the requested point is inside the current control's area (meaning user clicked this control)
		if(screenItemArea.PtInRect(requestPoint)){

			//Delete the current item(s)
			//Check to see if control we are deleting is a triple screen text area
			if(pCurScreenItem->controlAttributes.m_nType == CWindowControl::TextControl &&
				(CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type == CWindowTextExt::TypeMultiReceipt1 || 
				CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type == CWindowTextExt::TypeMultiReceipt2 || 
			   CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type  == CWindowTextExt::TypeMultiReceipt3 ||
			   (CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type > (CWindowTextExt::CWindowTextType)100 && CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type  < (CWindowTextExt::CWindowTextType)299))){

				CWindowControl *pwc = pScreenItem->GetDataFirst ();

				//cycle through the windows control list and delete all three corresponding text items
				while(pwc){
					if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
						CWindowTextExt *ti = CWindowTextExt::TranslateCWindowControl(pwc);
						ASSERT(ti);

						if(ti->textAttributes.type == CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type || 
						   ti->textAttributes.type == CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type - 100 || 
						   ti->textAttributes.type == CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type - 200 ||
						   ti->textAttributes.type == CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type + 100 || 
						   ti->textAttributes.type == CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type + 200){

							pwc = pScreenItem->GetDataThis();
							ASSERT(pwc);

							CString tempB;
							CString caption;
							//loading the string of the text control multi displays
							tempB.LoadString((CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type) + IDS_TXT_SING_RCT_MAIN);
							caption.Format(_T("%s"), tempB);

							//calling function to delete the tree item associated with this control, unless it is blank
							if (caption != ""){
								//calling function to delete the item on the left side on the appropiate tree
								//we pass the ID of the control that we want deleted
								myTree->DeleteControlTreeItem(pwc->controlAttributes.m_myId);
							}
							pScreenItem->DeleteDataThis();
						}
					}
					pwc = pScreenItem->GetDataNext ();
				}
					
				//triple text area deleted - exit function
				return TRUE;
			}
			else if(pCurScreenItem->controlAttributes.m_nType == CWindowControl::TextControl && CWindowTextExt::TranslateCWindowControl(pCurScreenItem)->textAttributes.type == CWindowText::TypeOEP){
				pScreenItem->DeleteDataThis();
				return DeleteWndItem( row, column, pScreenItem->controlAttributes.m_myId);
			}
			// if single text area or button
			else{
				//if control is a subwindow and contains other controls, we dont delete it
				if(pCurScreenItem->controlAttributes.m_nType == CWindowControl::WindowContainer){
					pCurScreenItem = pScreenItem->GetDataNext();
					continue;
				}
				else{

					CWindowControl *pwc = pScreenItem->GetDataThis();
					ASSERT(pwc);

					//calling function to delete the item on the left side on the appropiate tree
					//we pass the ID of the control that we want deleted
					myTree->DeleteControlTreeItem(pwc->controlAttributes.m_myId);
					
					//delete item and exit function
					pScreenItem->DeleteDataThis();
					return TRUE;
				}
			}
		}
		//get next control in list if the selected one was not found
		pCurScreenItem = pScreenItem->GetDataNext();
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::PreTranslateMessage(MSG* pMsg) 
//RETURN VALUE - BOOL
//ATTRIBUTES - pMsg - Windows message captured by function 
//DESCRIPTION - this function is used to dismiss the test window by pressing the escape key.
//				this is nescessary since the test window is full screen
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	//Check to see if the escape key was pressed
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{	
		//destroy test window
		this->DestroyWindow();
		return TRUE;
	}
	return CWnd::PreTranslateMessage( pMsg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::IsValidGrid(int row, int column, UINT id, CRect rect, CWindowControl *wc) 
//RETURN VALUE - BOOL
//ATTRIBUTES - row, column - indicate the point where we are trying to create the new control
//			   id - ID of the active window we are trying to create the control in
//			   rect - represents the screen area to be covered by the new control
//			   wc - this is passes when the user tries to create a button over an existing one
//					he will actually edit the old one but this is passed so that the old button
//					will not interfere with the creation of the new one
//DESCRIPTION - this function checks the screen area of a new control to be created to ensure it 
//				does not overlap any existing controls
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::IsValidGrid(int row, int column, UINT id, CRect rect, CWindowControl *wc)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();
	
	//cycle through document list of controls to search for window we are creating control in
	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		//create rect containing windows screen ares
		CRect jj(bi->controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
				 bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);

		//see if desired point is in the windows screen area
		if (jj.PtInRect (CPoint (column, row))) {
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
			//see if the window containing the point has the correct ID
			if (pw && (pw->controlAttributes.m_myId == id)) {

				//cycle through windows controls to check each one
				CWindowControl *pwc = pw->GetDataFirst ();
				while (pwc) {
						//build a rectangle to hold the controls screen area - 
						CRect  kk;
	
						kk.SetRect(pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn, 
								   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow, 
								   pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier, 
								   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);

						//check to verify we are creating a new control or editng an old one
						if (wc==NULL || (wc!=NULL && pwc != wc)){
							//check to endsure the new control doesnt overlap the exisitng  control
							if (kk.IntersectRect(rect,kk)) 
								//control overlaps an existing control - cannot create
								return FALSE;
						}	
						pwc = pw->GetDataNext ();
				
				}
			}
			if (pw && (pw->controlAttributes.m_myId != id)) {
				//if current window is found but ID doesnt match, get the correct window if it exists
				pw = GetSelectedWindow(pw, id);
				if(pw != NULL){
					//cycle through windows controls to check each one
					CWindowControl *pwc = pw->GetDataFirst ();
					while (pwc) {
							//build a rectangle to hold the controls screen area - Text Areas and Buttons
							//currently use different techniques to figure location, so logic is different for each
							CRect  kk;

							kk.SetRect(pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn, 
									   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow, 
									   pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier, 
									   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);

							//check to verify we are creating a new control or editng an old one
							if (wc==NULL || (wc!=NULL && pwc != wc)){
								//check to endsure the new control doesnt overlap the exisitng  control
								if (kk.IntersectRect(rect,kk)) 
									//control overlaps an existing control - cannot create
									return FALSE;
							}	
							pwc = pw->GetDataNext ();
					
					}
				}
			}
		}
	}			
	// control does not overlap any exisiting controls
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::ValidateText(UINT Id) 
//RETURN VALUE - BOOL
//ATTRIBUTES -  id - ID of the text area we are trying to create
//DESCRIPTION - this function checks to make sure only one of each text item is created. If 
//				you try to create a type of text that already exists, you will get an error
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::ValidateText(CWindowTextExt::CWindowTextType type)
{
	//cycle through controls to check ID's of each text item
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();
	
	while (pos){
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);

		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
		CWindowControl *pwc = pw->GetDataFirst ();

		while(pwc){
			//if text, validate its ID
			if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
				// if ID already exists, cannot create
				if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == type){
					return FALSE;
				}
			}
			//if window item, call recursive function to check its list of control
			if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
				//if function call invalidated text, cannot create
				BOOL retr = ValidateSubWinText(type, pw);
				if(!retr){
					return retr;
				}
			}
			pwc = pw->GetDataNext ();
		}
	
	}
	//can create  text, ID doesnt exist yet
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::ValidateSubWinText(UINT Id) 
//RETURN VALUE - BOOL
//ATTRIBUTES -  id - ID of the text area we are trying to create
//DESCRIPTION - this function checks to make sure only one of each text item is created. If 
//				you try to create a type of text that already exists, you will get an error. This 
//				is similar to CWindowDocumentExt::ValidateText except it is recursive and checks
//				window item lists whereas the other checks the document level lists
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::ValidateSubWinText(CWindowTextExt::CWindowTextType type, CWindowItemExt* wi)
{
	//cycle through windows list of controls to validate text items
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	while (currentPos) {
		CWindowControl *pwc = wi->ButtonItemList.GetNext(currentPos);
		//if text, validate its ID
		if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
			// if ID already exists, cannot create
			if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == type){
				return FALSE;
			}
		}
		//if window item, call recursive function to check its list of control
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			//if function call invalidated text, cannot create
			BOOL retr = ValidateSubWinText(type,CWindowItemExt::TranslateCWindowControl(pwc));
			if(!retr){
				return retr;
			}
		}
	}
	//can create  text, ID doesnt exist yet
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::ValidateWindow(CRect rect, CWindowItemExt *wi  )
//RETURN VALUE - BOOL
//ATTRIBUTES -  rect - screen area of the control being created
//				wi - window item the control is being created in
//DESCRIPTION - this function checks to make sure that the control being created does not extend
//				beyond the window it is being created in
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::ValidateWindow(CRect rect, CWindowItemExt *wi  )
{
	//create rect with screen area of the window to create the item in
	CRect windowRect(wi->controlAttributes.m_nColumn,
					 wi->controlAttributes.m_nRow,
					 wi->controlAttributes.m_nColumn + wi->controlAttributes.m_usWidthMultiplier,
					 wi->controlAttributes.m_nRow + wi->controlAttributes.m_usHeightMultiplier);

	//verify the control is within the bound of the windows recr
	if(rect.top < windowRect.top || 
	   rect.bottom > windowRect.bottom || 
	   rect.right > windowRect.right || rect.left < windowRect.left)

		return FALSE;

	else 
		return TRUE;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::ValidateWindowDoc(CWindowControl *wi  )
//RETURN VALUE - BOOL
//ATTRIBUTES -  rect - screen area of the document
//				wi - window item to be created
//DESCRIPTION - this function checks to make sure that the window being created does not extend
//				beyond the window of the documentit is being created in
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::ValidateWindowDoc(CWindowControl *wi  )
{
	//create a rect with the windows screen area
	CRect windowRect(wi->controlAttributes.
					 m_nColumn,wi->controlAttributes.m_nRow,
					 wi->controlAttributes.m_nColumn + wi->controlAttributes.m_usWidthMultiplier,
					 wi->controlAttributes.m_nRow + wi->controlAttributes.m_usHeightMultiplier);
	return (ValidateWindowDoc(windowRect));
}

BOOL CWindowDocumentExt::ValidateWindowDoc(CRect &windowRect )
{
	CRect myDoc(this->controlAttributes.m_nColumn,
				this->controlAttributes.m_nRow,
				this->controlAttributes.m_nColumn + this->controlAttributes.m_usWidthMultiplier,
				this->controlAttributes.m_nRow + this->controlAttributes.m_usHeightMultiplier);

	// verify the window is within the bounds of the document
	if(myDoc.top > windowRect.top || 
	   myDoc.bottom < windowRect.bottom || 
	   myDoc.right < windowRect.right || 
	   myDoc.left > windowRect.left)

		return FALSE;
	else 
		return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CWindowDocumentExt::SearchForWindow(int row, int column)
//RETURN VALUE - BOOL
//ATTRIBUTES -  row, column - represent the point that was clicked
//DESCRIPTION - this function checks to see if the user clicked a point contained
//				in an existing window. If so, TRUE is returned, if not FALSE 
//				is returned
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::SearchForWindow(int row, int column)
{
	//initialize a null WindowControl
	CWindowControl *bi = 0;
	//get the first position if the Documents Window List
	POSITION pos = listControls.GetHeadPosition ();

	//while there are still windows in the list
	while (pos) {
		pclListCurrent = pos;
		//get the next window in the list and assign it to bi
		bi = listControls.GetNext (pos);
		//form a CRect representing the windows screen area
		CRect jj(bi->controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
				 bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);
		//is the point clicked contianed in an existing window
		if (jj.PtInRect (CPoint (column, row))) {
			return TRUE;
		}
		
	}
	//no window found containing this point
	return FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
// Return a window control at the specified point.
// 111027 A-KSo
//---------------------------------------------------------------------------------------------------------------------
CWindowControl *CWindowDocumentExt::GetWindowControl(int row, int col)
{
	CWindowControl *pControl = NULL;
	POSITION pos = listControls.GetHeadPosition();

	while (pos != NULL) 
	{
		pclListCurrent = pos;
		pControl = listControls.GetNext (pos);
		CRect controlRect(pControl->controlAttributes.m_nColumn, 
				 pControl->controlAttributes.m_nRow, 
				 pControl->controlAttributes.m_nColumn + pControl->controlAttributes.m_usWidthMultiplier, 
				 pControl->controlAttributes.m_nRow + pControl->controlAttributes.m_usHeightMultiplier);

		if (controlRect.PtInRect (CPoint (col, row))) 
			break;
	}
	if (pos == NULL) pControl = NULL;
	return pControl;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CWindowDocumentExt::SearchForItem (int row, int column, UINT id)
//RETURN VALUE - int 0 - no item found
//					 3 - user clicked in the active window but no control found at this point
//					 4 - user clicked at a point where a button or text area exists
//					 6 - user clicked in a window but it isnt the active window
//ATTRIBUTES -  row, column - represent the point that was clicked
//				id - id of the active window in the document list
//DESCRIPTION - Function searches for an item that has an id as specified 
//				and for which contains in its area the specified row and column.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int CWindowDocumentExt::SearchForItem (int row, int column, UINT id)
{	
	int nfReturn = 0;
	//create a NULL WindowControl
	CWindowControl *bi = 0;

	POSITION pos = listControls.GetHeadPosition ();
	//while there are still controls left in the documetns window list
	while (pos) {
		pclListCurrent = pos;
		//get the next window item in the list and asign to bi
		bi = listControls.GetNext (pos);
		
		//create a CRect with the windows screen area
		CRect jj(bi->controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
				 bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);
		
		//is the point the user clicked in is in this window
		if (jj.PtInRect (CPoint (column, row))) {
			//cast the window control to a window item
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
			
			//if pw is valid and its the active window
			if (pw && (pw->controlAttributes.m_myId == id)) {
				//get the first control in the windows control list
				CWindowControl *pwc = pw->GetDataFirst ();
				//while there are still controls in teh windows list
				while (pwc) {
					//create a CRect to represent the controls screen area
					CRect kk;

					kk.SetRect(pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn, 
							   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow, 
							   pw->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier, 
							   pw->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);
					
					//is the point the user clicked contianed in the control
					if (kk.PtInRect (CPoint (column, row))) {
						return 4;
					}
					//user didnt click in this control, get the next contol in the windows list
					pwc = pw->GetDataNext ();
				}
				//window item found with active ID but no control found at this point
				return 3;
			}
			//pw is valid but it isnt the active window
			else if (pw && (pw->controlAttributes.m_myId != id)){
				nfReturn = 6;
			}
		}
	}
	//no item found
	return nfReturn;
}

CWindowControl*  CWindowDocumentExt::SearchForSubWinItem(int row, int column, CWindowItemExt *wi, UINT id)
{
	ASSERT(wi->controlAttributes.m_myId = id);
	CWindowControl *pwc = 0;

	pwc = wi->GetDataFirst();

	CRect jj;
	
	while (pwc) {
		CRect kk;

		kk.SetRect(wi->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn, 
				   wi->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow, 
				   wi->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier, 
				   wi->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);


		if (kk.PtInRect (CPoint (column, row))) {
			return pwc;
		}
		else{
			pwc = wi->GetDataNext ();
		}
	}
	return (CWindowControl*)wi;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CWindowDocumentExt::GetSelectedWindow(CWindowItemExt *wi, UINT id)
//RETURN VALUE - CWindowItemExt*
//ATTRIBUTES -  wi- window the user clicked in if 1st entry
//				 into function. Subsequent entries, it is a subwindow of that 
//				 window
//				id - id of the active window in the document list
//DESCRIPTION - Recursive function to cycle through all CWindowItem's control 
//				list searching for the active window.Recursive because each 
//				window item list can contian other window items with list that 
//				may contian more window items and so forth
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CWindowItemExt* CWindowDocumentExt::GetSelectedWindow(CWindowItemExt *wi, UINT id)
{
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	//while the window has controls left in its list
	while (currentPos) {
		//get the next control
		CWindowControl *pwc = wi->ButtonItemList.GetNext(currentPos);
		//is it a Window Item
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			//is the Window Item ID the active ID
			if(pwc->controlAttributes.m_myId == id){
				//return this window 
				return CWindowItemExt::TranslateCWindowControl(pwc);
			}
			//this window item is not the active window
			else{
				//does this window have any subwindows that matches the active ID
				if(GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pwc), id) != NULL){
					//return the window with the matching ID
					return GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pwc),id);
				}
			}
		}
	}
	//no window found with the matching ID
	return NULL;

}

CWindowItemExt* CWindowDocumentExt::GetSelectedDocWindow( UINT id)
{
	POSITION  currentPos = listControls.GetHeadPosition ();
	//while the document has windows left in its list
	while (currentPos) {
		//get the next control
		CWindowControl *pwc = listControls.GetNext(currentPos);
		//is it a Window Item
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			//is the Window Item ID the active ID
			if(pwc->controlAttributes.m_myId == id){
				//return this window 
				return CWindowItemExt::TranslateCWindowControl(pwc);
			}
			//this window item is not the active window
			else{
				//does this window have any subwindows that matches the active ID
				if(GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pwc), id) != NULL){
					//return the window with the matching ID
					return GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pwc),id);
				}
			}
		}
	}
	//no window found with the matching ID
	return NULL;

}

void CWindowDocumentExt::VerifyVersion()
{
	unsigned long currentVersion = ((CNewLayoutApp*)AfxGetApp())->SerializationVersion;
	unsigned long fileVersion = documentAttributes.Version;

	if(fileVersion != currentVersion){
		AfxMessageBox(IDS_FILEVER_MSG,MB_OK,0);
		documentAttributes.Version = currentVersion;
	}	
}

void CWindowDocumentExt::InitializeDefaults()
{
	LOGFONT lf;
	//CDC *hdc = GetDC();
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -11;//-MulDiv(8, hdc->GetDeviceCaps(  LOGPIXELSY), 72);                         
//	ReleaseDC( hdc);
	lf.lfWeight = FW_NORMAL;
                       
	_tcscpy_s(lf.lfFaceName, _T("Arial"));

	
	DocumentDef.defaultAttributes.WidthDefault = 3;
	DocumentDef.defaultAttributes.HeightDefault = 3;
	
	DocumentDef.defaultAttributes.FontColorDefault = DEF_FONT_COLOR;
	DocumentDef.defaultAttributes.ButtonDefault = DEF_BTN_COLOR;
	DocumentDef.defaultAttributes.WindowDefault = DEF_WND_COLOR;
	DocumentDef.defaultAttributes.TextWinDefault = DEF_TXT_COLOR;
	DocumentDef.defaultAttributes.ShapeDefault = CWindowButton::ButtonShapeRect;
	DocumentDef.defaultAttributes.PatternDefault = CWindowButton::ButtonPatternNone;
	DocumentDef.defaultAttributes.FontDefault = lf;
	PopupDefaultDlg();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowDocumentExt::DeleteWndItemFromID(int row, int column, UINT id)
//RETURN VALUE - BOOL
//ATTRIBUTES -  
//				id -- the ID of the current active 
//			   window
//DESCRIPTION - function deletes window items when user selects a window to delete from teh tree 
//				control. Uses ID only, not row and column. If the window contains otehr controls
//				the user is alerted and asked if they still want to delte the window 
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWindowDocumentExt::DeleteWndItemFromID(UINT id)
{
	CWindowControl *pCurScreenControl =0;
	int retVal;
	//get the selected window item
	CWindowItemExt *winRemove = GetSelectedDocWindow(id);
	ASSERT(winRemove);

	if (! winRemove) {
		return FALSE;
	}

	//if the window contains controls, alert the user and see if they still wish to delete it
	if(winRemove->GetDataCount()){
		retVal = AfxMessageBox(IDS_WND_DEL_CONFIRM,MB_YESNO,0);
		if(retVal == IDNO){
			//do not delete
			return FALSE;
		}
		//we want to delete the window so remove all of its child controls
		else{
			POSITION  currentPos = winRemove->ButtonItemList.GetHeadPosition ();
			POSITION  currentPosLast = currentPos;
			while (currentPos) {
				CWindowControl *pwc = winRemove->ButtonItemList.GetNext (currentPos);
				ASSERT(pwc);
				delete pwc;
				winRemove->ButtonItemList.RemoveAt (currentPosLast);
				currentPosLast = currentPos;
			}
		}
	}
			
	//cycle through all items in the document's control list until we find the window so we can remove it
	POSITION listPosition = listControls.GetHeadPosition();
	while(listPosition){
		pclListCurrent = listPosition;
		
		//current control we are checking
		pCurScreenControl = listControls.GetNext(listPosition);

		CWindowItemExt *pScreenItem = CWindowItemExt::TranslateCWindowControl(pCurScreenControl);

		//is current window the item we are looking in?(WindowItem's ID should match the Active ID
		if(pScreenItem && (pScreenItem->controlAttributes.m_myId == id)){

			//get the tree view so we can remove the window from the tree item list
			CMDIFrameWnd *pFrame = 
							 (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

			CChildFrame *pChild = 
						 (CChildFrame *) pFrame->GetActiveFrame();

			CNewLayoutView *myView = pChild->GetRightPane();
			CLeftView *myTree = (CLeftView*)(myView->m_TreeView);
			myTree->DeleteTreeItem(pScreenItem->controlAttributes.m_myId);
			//verify the window is empty and delete it and remove from the documents list
			ASSERT(pScreenItem->GetDataCount() == 0);
			delete pScreenItem;
			listControls.RemoveAt (pclListCurrent);
			return TRUE;	
		}
	}
	return FALSE;
}

