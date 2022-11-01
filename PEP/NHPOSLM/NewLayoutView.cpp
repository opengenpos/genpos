/*
-----------------------------------------------------------------------------------------------------------------------
 NHPOSLM
 Copyright (C) 2012 GSU
-----------------------------------------------------------------------------------------------------------------------
 Date	|	Changes Made																	| Version	|	By
-----------------------------------------------------------------------------------------------------------------------
 ??????		Original
 111026		Fix the following problems:																		KSo@NCR
			* Grid and ruler aligment
			* Black line in position 0 of the horizontal ruler
			* Screen flickers on LButtonDown although there is no action
-----------------------------------------------------------------------------------------------------------------------
*/
#include "stdafx.h"
#include "NewLayout.h"
#include "NewLayoutDoc.h"
#include "NewLayoutView.h"
#include "LeftView.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include "DButtonEdit.h"
#include "DListBoxEdit.h"
#include "DWinItem.h"
#include "DWindowList.h"
#include "DTextItem.h"
#include "D_OEPEdit.h"
#include "DActiveWndEdit.h"
#include "DWinLabel.h"
#include "DWindowGroup.h"

#include "WindowDocumentExt.h"
#include "WindowItemExt.h"
#include "WindowTextExt.h"
#include "WindowButton.h"
#include "WindowListBox.h"
#include "WindowListBoxExt.h"
#include "WindowLabelExt.h"

#include "WindowGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the root window is always id number 0.  Other windows
// will count off from this.  This is why the global ID in
// the layoutView starts at 1.
UINT CNewLayoutView::uiGlobalID = 0;

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView
IMPLEMENT_DYNCREATE(CNewLayoutView, CScrollView )

BEGIN_MESSAGE_MAP(CNewLayoutView, CScrollView )
	//{{AFX_MSG_MAP(CNewLayoutView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDS_POP_SETWINDOW, OnPopSetwindow)
	ON_COMMAND(IDS_POP_SETSUBWIN, OnPopSetSubWin)
	ON_COMMAND(ID_FILE_TEST, OnFileTest)
	ON_COMMAND(IDS_POPUP_COPY, OnPopupCopy)
	ON_COMMAND(IDS_POPUP_CUT, OnPopupCut)
	ON_COMMAND(IDS_POPUP_PASTE, OnPopupPaste)
	ON_COMMAND(ID_VIEW_RESOLUTION_800, OnViewResolution800)
	ON_COMMAND(ID_VIEW_RESOLUTION_1024, OnViewResolution1024)
	ON_COMMAND(ID_VIEW_RESOLUTION_1280, OnViewResolution1280)
	ON_COMMAND(ID_VIEW_RESOLUTION_1440, OnViewResolution1440)
	ON_COMMAND(ID_VIEW_RESOLUTION_1680, OnViewResolution1680)
	ON_COMMAND(ID_VIEW_RESOLUTION_1920, OnViewResolution1920)
	ON_COMMAND(ID_VIEW_RESOLUTION_2560, OnViewResolution2560)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_800, OnUpdateViewResolution800)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_1024, OnUpdateViewResolution1024)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_1280, OnUpdateViewResolution1280)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_1440, OnUpdateViewResolution1440)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_1680, OnUpdateViewResolution1680)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_1920, OnUpdateViewResolution1920)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOLUTION_2560, OnUpdateViewResolution2560)
	ON_COMMAND(ID_POP1_CHANGEDEFAULTS, OnChangeDefaults)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_TOOLBAR_WINGROUP, OnToolbarWingroup)
	ON_COMMAND(ID_TOOLBAR_ACTIVEWND, OnToolbarActiveWnd)
	ON_COMMAND(ID_TOOLBAR_BTN, OnToolbarBtn)
	ON_COMMAND(ID_TOOLBAR_OEP, OnToolbarOep)
	ON_COMMAND(ID_TOOLBAR_TXT, OnToolbarTxt)
	ON_COMMAND(ID_TOOLBAR_WND, OnToolbarWnd)
	ON_COMMAND(ID_TOOLBAR_LBL, OnToolbarLbl)
	ON_COMMAND(ID_TOOLBAR_DELETE, OnToolbarDelete)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_BUTTON32789, OnFileTest)
	ON_COMMAND(ID_POP1_ACTIVE_WINDOW_IN_GROUP, OnToolbarActiveWnd)
	ON_COMMAND(ID_TOOLBAR_REDO, OnEditRedo)
	ON_COMMAND(ID_TOOLBAR_UNDO, OnEditUndo)
	ON_COMMAND(ID_TOOLBAR_DEFAULTS, OnChangeDefaults)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView ::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView ::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView ::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView construction/destruction
CNewLayoutView::CNewLayoutView()
{
	//Layout Manager defaults to creating a layout for a terminal with an 800*600 screen
	ScreenWidth = LAYOUT_SCREENWIDTH;
	ScreenHeight = LAYOUT_SCREENHEIGHT;
	//variables to hold information for copy/cut/past
	tempText = NULL;
	tempButton = NULL;
	tempWindow = NULL;
	dragControl = NULL;
	moveDragControl = FALSE;
	uiActiveID = 1;
	m_ZoomToFit = TRUE;
}

CNewLayoutView::~CNewLayoutView() {}

BOOL CNewLayoutView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView ::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView drawing
void CNewLayoutView::OnDraw(CDC* pDC)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	GetClientRect (&(pDoc->documentAttributes.myClientRect));
	int iSaveDC = pDC->SaveDC ();
	pDC->Rectangle(pDoc->documentAttributes.myClientRect);

	CBrush brushBlue(NONSELECTED_GRID);
	CBrush brushSel(SELECTED_GRID);
	CBrush brushOutside(OUTSIDE_RESOLUTION_GRID);

	CPen penBlack;
	penBlack.CreatePen(PS_SOLID, 1, NONSELECTED_CONTROL);
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	pDC->SetBkMode (TRANSPARENT);

	// We find out the width and the height of the document window which is the underlying
	// window on which everything else is placed.  These measurements are in grid units which
	// means that this tells us how many grid rectangles across and down we are supposed to
	// draw.
	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();

	int width  = pDoc->controlAttributes.m_usWidthMultiplier;
	int height = pDoc->controlAttributes.m_usHeightMultiplier;

	//get coordinate of last left click so we can show the user where they clicked
	int xSel = cpLastLButtonUp.x;
	int ySel = cpLastLButtonUp.y;
	xSel /= CWindowButtonExt::stdWidth;
	ySel /= CWindowButtonExt::stdHeight;

	int yCell = 0, xCell = 0;
	CBrush *pBrush = NULL;
	for (yCell = 0; yCell < height; yCell++) 
	{
		for (xCell = 0; xCell < width; xCell++) 
		{
			if (isCellSelected(xCell, yCell, xSel, ySel) == true)			//(ySel == i && xSel == j)
				pBrush = &brushSel;
			else if (isCellOutside(xCell, yCell, m_pParent->ResLoWidth, m_pParent->ResLoHeight) == true)
				pBrush = &brushOutside;
			else
				pBrush = &brushBlue;

			pDC->SelectObject(pBrush);
			pDC->Rectangle(CWindowButtonExt::getRectangle (yCell, xCell));
		}
	}

	CWindowControl *pw = pDoc->GetDataFirst();
	while (pw) 
	{
		//only display windows that are non popup. If a popup window is the active window, we will show it also
		if(pw->controlAttributes.isVirtualWindow || pw->controlAttributes.m_myId == uiActiveID || pw->controlAttributes.m_nType == CWindowControl::WindowGroup) 
		{
			pw->controlAttributes.CurrentID = uiActiveID;
			pw->WindowDisplay(pDC);
			if (pw->controlAttributes.Selected == TRUE) {
				TRACE("Name:%s\n", pw->myName);
			}
		}
		pw = pDoc->GetDataNext ();
	}

	pDC->RestoreDC (iSaveDC);
	UpdateRulersInfo(RW_POSITION, GetScrollPosition(), NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView printing
BOOL CNewLayoutView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CNewLayoutView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}
void CNewLayoutView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView diagnostics
#ifdef _DEBUG
void CNewLayoutView::AssertValid() const
{
	CScrollView ::AssertValid();
}

void CNewLayoutView::Dump(CDumpContext& dc) const
{
	CScrollView ::Dump(dc);
}

CWindowDocumentExt * CNewLayoutView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewLayoutDoc)));
	return (&((CNewLayoutDoc*)m_pDocument)->myWindowDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutView message handlers

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CNewLayoutView::PopupButtonEditDialog (int row, int column)
//RETURN VALUE - void
//PARAMETERS - row, column - represents the point where the user has clicked
//DESCRIPTION - This function is called when the user clicks the menu to add/
//				edit buttons. Checks are done to see where the user clciked
//				and if anything exists where they clicked to determine if they
//				will be editing or adding a button. This function calls the
//				dialog for creating the buttons.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CNewLayoutView::PopupButtonEditDialog (int row, int column)
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	UINT saveID = uiActiveID;
	DefaultData df;
	pDoc->DocumentDef.AssignTo(df);
	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);

	//a window was clicked in
	if (nSearch > 1) {
		//get the window that was clicked in and cast it to a CWindowItemExt
		CWindowControl *pc = pDoc->GetDataThis();
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

		//user clicked in an inactive window (sub window)
		if(nSearch == 6){
			//call function to get the window they clicked in
			pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(pw == NULL){
				return;
			}
			//call function to see if an exixting item was clicked in and assign to nSearch
			nSearch = pw->SearchForControl(row,column);
		}

		//this window does exist
		if (pw) {
			//is there an existing control here?
			if (nSearch == 4) {
				//is this control a button
				if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::ButtonControl){
					//get the button that was clicked in and cast it to CWindowButtonExt
					CWindowControl *wc = pw->GetDataThis ();
					CWindowButtonExt *pb = CWindowButtonExt::TranslateCWindowControl (wc);
					//initialize an instance of the dialog class for editing buttons with this button
					//and the window contianing this button
					CDButtonEdit jj(pb,pw);
					//set the flag for the dialog accordint to if this button uses defualts or custom attributes
					jj.defFlag = pb->controlAttributes.useDefault;
					//set the dialogs document to the layouts document
					jj.pDoc = pDoc;

					//did the user click ok on the dialog
					if (jj.DoModal () == IDOK && jj.myAction >= 0) {
						//create a CRect to contian the screen area of this button
						CRect thisControl(pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
										  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn + jj.m_usWidthMultiplier,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow + jj.m_usHeightMultiplier);

						/*call function to verify that the edited button will not overlap
						any other buttons or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
													  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
													  uiActiveID,thisControl,
													  wc);
						/*call function to verify that the edited button will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window = pDoc->ValidateWindow(thisControl,pw);

						/* check to see if button actually overlaps controls on its own
						window, if button is calling a popup window. - CSMALL */
						BOOL popupGrid = (pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
														    pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
														    pw->controlAttributes.m_myId,thisControl,wc));

						//did both checks pass, for regular window or for popup window
						if((grid && window)||(window && popupGrid) ){
							//assign edited attributes from dialog to the button
							//if size of caption is not larger than max size of myCaption, then insert it into the structure
							//else, truncate the characters past max size and then insert
							if (sizeof(jj.m_csCaption) <= MaxSizeOfMyCaption){
								pb->myCaption = jj.m_csCaption;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, jj.m_csCaption, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pb->myCaption = tempMyCaption;
							}
							_tcsncpy_s(pb->controlAttributes.m_myCaption, pb->myCaption, MaxSizeOfMyCaption);
							pb->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

							if (sizeof(jj.m_myName) <= MaxSizeOfMyCaption){
								pb->myName = jj.m_myName;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, jj.m_myName, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pb->myName = tempMyCaption;
							}
							_tcsncpy_s(pb->controlAttributes.m_myName, pb->myName, MaxSizeOfMyCaption);
							pb->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

							pb->m_Icon = jj.m_csIcon;
							pb->controlAttributes.m_colorText = jj.colorText;
							pb->controlAttributes.lfControlFont = jj.myBtnFont;
							pb->controlAttributes.m_colorFace = jj.colorFace;
							pb->btnAttributes.m_myShape = jj.myShape;
							pb->btnAttributes.m_myPattern = jj.myPattern;
							pb->controlAttributes.m_myAction = jj.myAction;
							memcpy (&(pb->btnAttributes.m_myActionUnion), &jj.myActionUnion, sizeof (pb->btnAttributes.m_myActionUnion));
							pb->controlAttributes.m_usWidthMultiplier = jj.m_usWidthMultiplier;
							pb->controlAttributes.m_usHeightMultiplier = jj.m_usHeightMultiplier;
							pb->controlAttributes.m_nWidth = jj.m_usWidthMultiplier * CWindowButton::stdWidth;
							pb->controlAttributes.m_nHeight = jj.m_usHeightMultiplier * CWindowButton::stdHeight;
							pb->controlAttributes.useDefault = jj.defFlag;
							pb->btnAttributes.capAlignment = jj.myCapAlignment;
							pb->btnAttributes.horIconAlignment = jj.myHorIconAlignment;
							pb->btnAttributes.vertIconAlignment = jj.myVertIconAlignment;
							pb->btnAttributes.HorizontalOriented = jj.horizOrient;
							memcpy(&(pb->controlAttributes.SpecWin[0]), &(jj.m_csSpecWin[0]), sizeof(pb->controlAttributes.SpecWin));
							pb->controlAttributes.ulMaskGroupPermission = jj.m_GroupPermissions;

							//mark for repainting
							InvalidateRect (NULL, FALSE);
							pDoc->CheckPoint();
							//update left pane view
							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

							myTree->UpdateButtonControlTree(pb);
						}
						//did not pass checks
						else {
							//alert user according to which checks failed
							if(!grid)
								AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}
					}
				}
				//user clciked in a text area - let them know they cannot create a button here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl){
					AfxMessageBox( IDS_TEXTEXIST,  MB_OK,  0 );
				}
			}
			//did the user click in an area where no control exists or in an inactive window
			else if (nSearch == 3 /*|| nSearch == 6*/) {

				/*intialize an instance of the dialog class for adding buttons with this
				the window that will contian the new button*/
				CDButtonEdit jjdb(NULL,pw);

				//inistialize the flag for the button to use defaults
				jjdb.defFlag = TRUE;
				//set the dialogs document to the layouts document
				jjdb.pDoc = pDoc;

				int status = jjdb.DoModal ();
				//user clicked ok
				if (status == IDOK && jjdb.myAction >= 0) {
					//Are we creating a default number pad
					if(jjdb.myAction == IDD_P03_DEF_KEYPAD){
						//create an array to hold the buttons for the number pad, used for looping
						CArray<CWindowButtonExt *,CWindowButtonExt *> numPadButtons;
						numPadButtons.SetSize(15,1);
						//loop to create 10 buttons numbered 0-9
						for(int loop = 0; loop < 10;loop++){
							//create a new button - assign attributes
							CWindowButtonExt *pb = new CWindowButtonExt(0, row - pw->controlAttributes.m_nRow, column - pw->controlAttributes.m_nColumn);
							char buff[2];
							pb->myCaption = _itoa(loop,buff,10);
							pb->controlAttributes.m_myId = uiGlobalID++;
							pb->m_Icon = jjdb.m_csIcon;
							pb->controlAttributes.m_colorText = jjdb.colorText;
							pb->controlAttributes.lfControlFont = jjdb.myBtnFont;
							pb->controlAttributes.m_colorFace = jjdb.colorFace;
							pb->btnAttributes.m_myShape = jjdb.myShape;
							pb->btnAttributes.m_myPattern = jjdb.myPattern;
							pb->controlAttributes.m_myAction = IDD_P03_NUMBERPAD;
							pb->btnAttributes.m_myActionUnion.data.extFSC = loop;
							pb->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeNumPad;
							pb->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
							pb->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
							pb->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
							pb->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
							pb->controlAttributes.useDefault = jjdb.defFlag;
							memcpy(&(pb->controlAttributes.SpecWin[0]), &(jjdb.m_csSpecWin[0]), sizeof(pb->controlAttributes.SpecWin));
							pb->btnAttributes.capAlignment = jjdb.myCapAlignment;
							pb->btnAttributes.horIconAlignment = jjdb.myHorIconAlignment;
							pb->btnAttributes.vertIconAlignment = jjdb.myVertIconAlignment;
							pb->btnAttributes.HorizontalOriented = jjdb.horizOrient;
							CString temp;
							temp.LoadString(LOWORD(IDD_P03_NUMBERPAD));
							pb->pContainer = pw;

							numPadButtons.SetAt(loop, pb);
						}

						//store the coordinates of the top left button
						int thisRow = row - pw->controlAttributes.m_nRow;
						int thisColumn = column - pw->controlAttributes.m_nColumn;

						//adjust the coordinates for each individaul button
						numPadButtons.GetAt(7)->controlAttributes.m_nRow = thisRow;
						numPadButtons.GetAt(7)->controlAttributes.m_nColumn = thisColumn;

						numPadButtons.GetAt(8)->controlAttributes.m_nColumn = thisColumn + jjdb.m_usWidthMultiplier;

						numPadButtons.GetAt(9)->controlAttributes.m_nColumn = thisColumn + 2*(jjdb.m_usWidthMultiplier);

						numPadButtons.GetAt(4)->controlAttributes.m_nRow = thisRow + jjdb.m_usHeightMultiplier;
						numPadButtons.GetAt(4)->controlAttributes.m_nColumn = thisColumn;

						numPadButtons.GetAt(5)->controlAttributes.m_nRow = thisRow + jjdb.m_usHeightMultiplier;
						numPadButtons.GetAt(5)->controlAttributes.m_nColumn = thisColumn + jjdb.m_usWidthMultiplier;

						numPadButtons.GetAt(6)->controlAttributes.m_nRow = thisRow + jjdb.m_usHeightMultiplier;
						numPadButtons.GetAt(6)->controlAttributes.m_nColumn = thisColumn + 2*(jjdb.m_usWidthMultiplier);

						numPadButtons.GetAt(1)->controlAttributes.m_nRow = thisRow + 2*(jjdb.m_usHeightMultiplier);
						numPadButtons.GetAt(1)->controlAttributes.m_nColumn = thisColumn;

						numPadButtons.GetAt(2)->controlAttributes.m_nRow = thisRow + 2*(jjdb.m_usHeightMultiplier);
						numPadButtons.GetAt(2)->controlAttributes.m_nColumn = thisColumn + jjdb.m_usWidthMultiplier;

						numPadButtons.GetAt(3)->controlAttributes.m_nRow = thisRow + 2*(jjdb.m_usHeightMultiplier);
						numPadButtons.GetAt(3)->controlAttributes.m_nColumn = thisColumn + 2*(jjdb.m_usWidthMultiplier);

						numPadButtons.GetAt(0)->controlAttributes.m_nRow = thisRow + 3*(jjdb.m_usHeightMultiplier);
						numPadButtons.GetAt(0)->controlAttributes.m_nColumn = thisColumn /*+ 2*(jjdb.m_usWidthMultiplier)*/;

						BOOL passCheck = TRUE;
						BOOL passCheckGrid = TRUE;
						BOOL passCheckWin = TRUE;

						//loop through all 10 buttons
						for(int z = 0; z<10;z++){
							//get the button from the array at index z
							CWindowButtonExt *temp = numPadButtons.GetAt(z);
							//create a CRect to represent the buttons screen area
							CRect thisButton(pw->controlAttributes.m_nColumn + temp->controlAttributes.m_nColumn,
											 pw->controlAttributes.m_nRow + temp->controlAttributes.m_nRow,
											 pw->controlAttributes.m_nColumn + temp->controlAttributes.m_nColumn + temp->controlAttributes.m_usWidthMultiplier,
											 pw->controlAttributes.m_nRow + temp->controlAttributes.m_nRow + temp->controlAttributes.m_usHeightMultiplier);
							/*call function to verify that the edited button will not overlap
							any other buttons or text and assign result to flag*/
							BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + temp->controlAttributes.m_nRow,pw->controlAttributes.m_nColumn + temp->controlAttributes.m_nColumn,uiActiveID,thisButton, NULL);
							/*call function to verify that the edited button will not extend
							beyond the borders of the window and assign result to flag*/
							BOOL window =pDoc->ValidateWindow(thisButton,pw);

							//enter this if statement if we havent entered it before and an error did not occur
							if (passCheck){
								if(!grid)
								{	//set BOOL that another control was overlapped
									passCheckGrid = FALSE;
									//pass check does not need to be performed again
									passCheck = FALSE;
								}
								if(!window)
								{	//set BOOL that controls extended window
									passCheckWin = FALSE;
									//pass check does not need to be performed again
									passCheck = FALSE;
								}
							}//end if

							//did both checks pass?
							if(grid && window){
								//add button to the windows control list and mark for repainting
								pw->AddDataLast (temp);
								pDoc->CheckPoint();
								InvalidateRect (NULL, FALSE);
								//sending window information to be created in the left pane
								CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);
								CLeftView *myTree = (CLeftView*)m_TreeView;
								ASSERT(myTree);

								myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pw);
							}//end if
						}//end for loop

						//the checks did not pass
						if (!passCheck) {
							//alert the user about the check that did not pass
							if(!passCheckGrid)
								AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!passCheckWin)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}//end if
					}
					// we are not creating a number pad - create a normal button
					else{
						// create button and assign attributes set in dialog
						CWindowButtonExt *pb = new CWindowButtonExt(0, row - pw->controlAttributes.m_nRow, column - pw->controlAttributes.m_nColumn);

						//if size of caption is not larger than max size of myCaption, then insert it into the structure
						//else, truncate the characters past max size and then insert
						if (sizeof(jjdb.m_csCaption) <= MaxSizeOfMyCaption) {
							pb->myCaption = jjdb.m_csCaption;
						}
						else {
							TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
							_tcsncpy_s(tempMyCaption, jjdb.m_csCaption, MaxSizeOfMyCaption);
							tempMyCaption[MaxSizeOfMyCaption] = 0;
							pb->myCaption = tempMyCaption;
						}

						if (sizeof(jjdb.m_myName) <= MaxSizeOfMyCaption) {
							pb->myName = jjdb.m_myName;
						}
						else {
							TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
							_tcsncpy_s(tempMyCaption, jjdb.m_myName, MaxSizeOfMyCaption);
							tempMyCaption[MaxSizeOfMyCaption] = 0;
							pb->myName = tempMyCaption;
						}

						pb->m_Icon = jjdb.m_csIcon;
						pb->controlAttributes.m_colorText = jjdb.colorText;
						pb->controlAttributes.lfControlFont = jjdb.myBtnFont;
						pb->controlAttributes.m_colorFace = jjdb.colorFace;
						pb->btnAttributes.m_myShape = jjdb.myShape;
						pb->btnAttributes.m_myPattern = jjdb.myPattern;
						pb->controlAttributes.m_myAction = jjdb.myAction;
						memcpy (&(pb->btnAttributes.m_myActionUnion), &jjdb.myActionUnion, sizeof (pb->btnAttributes.m_myActionUnion));
						pb->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
						pb->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
						pb->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
						pb->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
						pb->controlAttributes.useDefault = jjdb.defFlag;
						pb->btnAttributes.capAlignment = jjdb.myCapAlignment;
						pb->btnAttributes.horIconAlignment = jjdb.myHorIconAlignment;
						pb->btnAttributes.vertIconAlignment = jjdb.myVertIconAlignment;
						pb->btnAttributes.HorizontalOriented = jjdb.horizOrient;
						pb->pContainer = pw;

						//create a CRect to represent the buttons screen area
						CRect thisControl(pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
										  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn + jjdb.m_usWidthMultiplier,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow + jjdb.m_usHeightMultiplier);

						/*call function to verify that the edited button will not overlap
						any other buttons or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
													  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
													  uiActiveID,thisControl, NULL);

						/*call function to verify that the edited button will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window = pDoc->ValidateWindow(thisControl,pw);

						/*checking if button actually overlaps other controls on popup window*/
						BOOL popupGrid = (pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
														    pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
														    pw->controlAttributes.m_myId,thisControl,
														    NULL));
						if((grid && window)||(window && popupGrid)){
							//add button to the windows control list and mark for repainting
							pb->controlAttributes.m_myId = uiGlobalID++;
							pw->AddDataLast (pb);
							pDoc->CheckPoint();
							InvalidateRect (NULL, FALSE);
							//sending window information to be created in the left pane
							CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);
							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

							myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pw);
						}
						// the checks did not pass
						else {
							//alert the user about the checks that failed
							if(!grid)
								AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

							delete pb;
						}
					}//create normal button
				}//user clicked ok
			}//nSearch = 3
		}//pw not null
	}//nSearch > 1
	uiActiveID = saveID;
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->ShowSelectedWindow(uiActiveID);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CNewLayoutView::PopupButtonEditDialog (int row, int column)
//RETURN VALUE - void
//PARAMETERS - row, column - represents the point where the user has clicked
//DESCRIPTION - This function is called when the user clicks the menu to add/
//				edit buttons. Checks are done to see where the user clciked
//				and if anything exists where they clicked to determine if they
//				will be editing or adding a button. This function calls the
//				dialog for creating the buttons.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CNewLayoutView::PopupListBoxEditDialog (int row, int column)
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	UINT saveID = uiActiveID;
	DefaultData df;
	pDoc->DocumentDef.AssignTo(df);
	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);

	//a window was clicked in
	if (nSearch > 1) {
		//get the window that was clicked in and cast it to a CWindowItemExt
		CWindowControl *pc = pDoc->GetDataThis();
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

		//user clicked in an inactive window (sub window)
		if(nSearch == 6){
			//call function to get the window they clicked in
			pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(pw == NULL){
				return;
			}
			//call function to see if an exixting item was clicked in and assign to nSearch
			nSearch = pw->SearchForControl(row,column);
		}

		//this window does exist
		if (pw) {
			//is there an existing control here?
			if (nSearch == 4) {
				//is this control a button
				if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::ListBoxControl){
					//get the button that was clicked in and cast it to CWindowButtonExt
					CWindowControl *wc = pw->GetDataThis ();
					CWindowListBoxExt *pb = CWindowListBoxExt::TranslateCWindowControl (wc);
					//initialize an instance of the dialog class for editing buttons with this button
					//and the window contianing this button
					CDListBoxEdit jj(pb,pw);
					//set the flag for the dialog accordint to if this button uses defualts or custom attributes
					jj.m_defFlag = pb->controlAttributes.useDefault;
					//set the dialogs document to the layouts document
					jj.pDoc = pDoc;

					//did the user click ok on the dialog
					if (jj.DoModal () == IDOK && jj.myAction >= 0) {
						//create a CRect to contian the screen area of this button
						CRect thisControl(pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
										  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn + jj.m_usWidthMultiplier,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow + jj.m_usHeightMultiplier);

						/*call function to verify that the edited button will not overlap
						any other buttons or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
													  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
													  uiActiveID,thisControl,
													  wc);
						/*call function to verify that the edited button will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window = pDoc->ValidateWindow(thisControl,pw);

						/* check to see if button actually overlaps controls on its own
						window, if button is calling a popup window. - CSMALL */
						BOOL popupGrid = (pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
														    pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
														    pw->controlAttributes.m_myId,thisControl,wc));

						//did both checks pass, for regular window or for popup window
						if((grid && window)||(window && popupGrid) ){
							//assign edited attributes from dialog to the button
							//if size of caption is not larger than max size of myCaption, then insert it into the structure
							//else, truncate the characters past max size and then insert
							if (sizeof(jj.m_csCaption) <= MaxSizeOfMyCaption){
								pb->myCaption = jj.m_csCaption;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, jj.m_csCaption, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pb->myCaption = tempMyCaption;
							}
							_tcsncpy_s(pb->controlAttributes.m_myCaption, pb->myCaption, MaxSizeOfMyCaption);
							pb->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

							if (sizeof(jj.m_myName) <= MaxSizeOfMyCaption){
								pb->myName = jj.m_myName;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, jj.m_myName, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pb->myName = tempMyCaption;
							}
							_tcsncpy_s(pb->controlAttributes.m_myName, pb->myName, MaxSizeOfMyCaption);
							pb->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

							pb->m_Icon = jj.m_csIcon;
							pb->controlAttributes.m_colorText = jj.colorText;
							pb->controlAttributes.lfControlFont = jj.myBtnFont;
							pb->controlAttributes.m_colorFace = jj.colorFace;
							pb->controlAttributes.m_usWidthMultiplier = jj.m_usWidthMultiplier;
							pb->controlAttributes.m_usHeightMultiplier = jj.m_usHeightMultiplier;
							pb->controlAttributes.m_nWidth = jj.m_usWidthMultiplier * CWindowButton::stdWidth;
							pb->controlAttributes.m_nHeight = jj.m_usHeightMultiplier * CWindowButton::stdHeight;
							pb->controlAttributes.useDefault = jj.m_defFlag;
							memcpy(&(pb->controlAttributes.SpecWin[0]), &(jj.m_csSpecWin[0]), sizeof(pb->controlAttributes.SpecWin));
							pb->controlAttributes.ulMaskGroupPermission = jj.m_GroupPermissions;

							//mark for repainting
							InvalidateRect (NULL, FALSE);
							pDoc->CheckPoint();
							//update left pane view
							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

//RJC TBD							myTree->UpdateButtonControlTree(pb);
						}
						//did not pass checks
						else {
							//alert user according to which checks failed
							if(!grid)
								AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}
					}
				}
				//user clciked in a text area - let them know they cannot create a button here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl){
					AfxMessageBox( IDS_TEXTEXIST,  MB_OK,  0 );
				}
			}
			//did the user click in an area where no control exists or in an inactive window
			else if (nSearch == 3 /*|| nSearch == 6*/) {

				/*intialize an instance of the dialog class for adding buttons with this
				the window that will contian the new button*/
				CDListBoxEdit jjdb(NULL,pw);

				//inistialize the flag for the button to use defaults
				jjdb.m_defFlag = FALSE;
				//set the dialogs document to the layouts document
				jjdb.pDoc = pDoc;

				int status = jjdb.DoModal ();
				//user clicked ok
				if (status == IDOK) {
						// create button and assign attributes set in dialog
						CWindowListBoxExt *pb = new CWindowListBoxExt(0, row - pw->controlAttributes.m_nRow, column - pw->controlAttributes.m_nColumn);

						//if size of caption is not larger than max size of myCaption, then insert it into the structure
						//else, truncate the characters past max size and then insert
						if (sizeof(jjdb.m_csCaption) <= MaxSizeOfMyCaption) {
							pb->myCaption = jjdb.m_csCaption;
						}
						else {
							TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
							_tcsncpy_s(tempMyCaption, jjdb.m_csCaption, MaxSizeOfMyCaption);
							tempMyCaption[MaxSizeOfMyCaption] = 0;
							pb->myCaption = tempMyCaption;
						}

						if (sizeof(jjdb.m_myName) <= MaxSizeOfMyCaption) {
							pb->myName = jjdb.m_myName;
						}
						else {
							TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
							_tcsncpy_s(tempMyCaption, jjdb.m_myName, MaxSizeOfMyCaption);
							tempMyCaption[MaxSizeOfMyCaption] = 0;
							pb->myName = tempMyCaption;
						}

						pb->m_Icon = jjdb.m_csIcon;
						pb->controlAttributes.m_colorText = jjdb.colorText;
						pb->controlAttributes.lfControlFont = jjdb.myBtnFont;
						pb->controlAttributes.m_colorFace = jjdb.colorFace;
						pb->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
						pb->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
						pb->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
						pb->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
						pb->controlAttributes.useDefault = jjdb.m_defFlag;
						pb->pContainer = pw;

						//create a CRect to represent the buttons screen area
						CRect thisControl(pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
										  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn + jjdb.m_usWidthMultiplier,
										  pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow + jjdb.m_usHeightMultiplier);

						/*call function to verify that the edited button will not overlap
						any other buttons or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
													  pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
													  uiActiveID,thisControl, NULL);

						/*call function to verify that the edited button will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window = pDoc->ValidateWindow(thisControl,pw);

						/*checking if button actually overlaps other controls on popup window*/
						BOOL popupGrid = (pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
														    pw->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
														    pw->controlAttributes.m_myId,thisControl,
														    NULL));
						if((grid && window)||(window && popupGrid)){
							//add button to the windows control list and mark for repainting
							pb->controlAttributes.m_myId = uiGlobalID++;
							pw->AddDataLast (pb);
							pDoc->CheckPoint();
							InvalidateRect (NULL, FALSE);
							//sending window information to be created in the left pane
							CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);
							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

							myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pw);
						}
						// the checks did not pass
						else {
							//alert the user about the checks that failed
							if(!grid)
								AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

							delete pb;
						}
				}//user clicked ok
			}//nSearch = 3
		}//pw not null
	}//nSearch > 1
	uiActiveID = saveID;
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->ShowSelectedWindow(uiActiveID);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CNewLayoutView::OnLButtonDblClk(UINT nFlags, CPoint point)
//RETURN VALUE - void
//PARAMETERS - point - point where user clicked
//				nFlags - indicates if any virtual keys are down
//DESCRIPTION - This function is called when the user double clicks the mouse in
//				the view. Function takes the point the user clcikes in and checks
//				to see if anything exists at that point and calls the appropriate
//				edit/add function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CNewLayoutView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//get the point clciked in by the user and convert coordiantes
	CPoint pt = GetDeviceScrollPosition( );
	int column = point.x + pt.x;
	int row = point.y + pt.y;

	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//get views document and assert valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);
	//a window was clicked in
	if (nSearch > 1) {
		//user clicked in an inactive window or sub window
		if(nSearch == 6){
			CWindowControl *pc = pDoc->GetDataThis ();
			CWindowItemExt *ww = CWindowItemExt::TranslateCWindowControl (pc);
			//call function to get the window they clicked in and assign to ww
			ww = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(ww, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(ww != NULL){
				CWindowControl *subControl = pDoc->SearchForSubWinItem(row, column, ww, uiActiveID);

				if(subControl->controlAttributes.m_nType == CWindowControl::TextControl){
					PopAddTextWindow (row, column);
					return;
				}
				else{
					PopupButtonEditDialog (row, column);
					return;
				}
			}
			return;
		}
		//a control exist at the point where the user clicked
		if(nSearch == 4){
			//get the active window that was clciked in and cast to CWindowItemExt
			CWindowControl *pc = pDoc->GetDataThis ();
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);
			//pw is valid and not null
			if (pw) {
				//get the window control of the window that was clciked
				CWindowControl *pwc = pw->GetDataThis ();
				//if pwc is valid and not null
				if(pwc != NULL){
					//if user clciked a text control
					if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
						//call function to edit text control
						if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowText::TypeOEP){
							PopSetOEPWin(row,column);
							return;
						}
						else{
							PopAddTextWindow(row,column);
							return;
						}
					}
					//user clciked a button item
					else if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){
						//call function to edit button control
						PopupButtonEditDialog (row, column);
						return;
					}
					//user clicked a label control
					else if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
						//set the point that was clicked so it can be used by label edit function
						cpLastRButton = point;
						OnAddLabel();
						return;
					}
					//ERROR-control type invalid
					else{
						return;
					}
				}
				//user clicked in a blank area of window - call funtion to add button control
				else{
					PopupButtonEditDialog (row, column);
					return;
				}
			}//if(pw)
		}//if(nSearch ==4)
	}//if(nSearch > 1)
	//user clciked area of documents view where nothing exist - call funtion to add window
	else{
		//set the coordinated of the point to be clciked in for the window edit function
		cpLastRButton.x = column * CWindowButtonExt::stdWidth;;
		cpLastRButton.y = row * CWindowButtonExt::stdWidth;
		//call function to add window
		OnPopSetwindow ();
		return;
	}
	//no control exists - call function to add button
	PopupButtonEditDialog (row, column);
	return;
}

void CNewLayoutView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt = GetDeviceScrollPosition( );
	point.x += pt.x;
	point.y += pt.y;

	cpLastLButtonUp = point;
	InvalidateRect(NULL, TRUE);
	CScrollView ::OnRButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//FUNCTION CNewLayoutView::OnRButtonDown(UINT nFlags, CPoint point)
//RETURN VALUE - void
//PARAMETERS - point - point where user clicked
//				nFlags - indicates if any virtual keys are down
//DESCRIPTION - This function is called when the user clicks the right mouse button
//				in the view. Funtion creates popup menu, dsplays it and tracks the
//				users selection from the menu
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CNewLayoutView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get the point clciked in by the user and convert coordiantes
	CPoint pt = GetDeviceScrollPosition( );

	point.x += pt.x;
	point.y += pt.y;
	int column = point.x;
	int row = point.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//set variable for last right button point clicked
	cpLastRButton = point;
	cpLastLButtonUp = point;
	InvalidateRect(NULL, TRUE);

	//popup menu creation
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CONTEXTMENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CRect rect;
	GetWindowRect(rect);
	//display menu and track selection of menu items
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						   (point.x - pt.x) + rect.left,
						   (point.y - pt.y) + rect.top,
						   this);
	//base class's implementation of messsage handler
	CScrollView::OnRButtonDown(nFlags,point);
}

BOOL CNewLayoutView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	//get the point clciked in by the user and convert coordiantes
	int column = cpLastRButton.x ;
	int row = cpLastRButton.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//get views documents and assert valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	switch(LOWORD(wParam)){
	case IDS_POPUP_EDITBTN:
	case IDS_POPUP_EDITWIN:
	case IDS_POPUP_EDITTEXT:
	case IDS_POPUP_EDITSUBWIN:
	case ID_POP1_EDIT_OEPWINDOW:
	case ID_TREE_EDIT_WINDOW:
		{
			/*see if anything exists where the user clicked
			0- no window was clicked in
			3- valid window but no control - create new button
			4- control exists here - edit if its a button
			6- user clicked in a window that isnt the active window*/
			int nSearch = pDoc->SearchForItem (row, column, uiActiveID);
			//a window was clicked in
			if (nSearch > 1) {
				//get the window the user clciked in and cast it to CWindowItemExt
				CWindowControl *pc = pDoc->GetDataThis ();
				CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);
				if(nSearch == 6){
					//call function to get the window they clicked in
					pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
					//if the window isnt a subwindow of the active window, exit function
					if(pw == NULL){
						return TRUE;
					}

				}
				//pw is valid and is not null
				if (pw) {
					if(((LOWORD(wParam) == IDS_POPUP_EDITWIN || LOWORD(wParam == ID_TREE_EDIT_WINDOW)) && nSearch != 6) || (LOWORD(wParam) == IDS_POPUP_EDITSUBWIN && nSearch == 6)){
						CDWinItem jjdb (pw,this);
						jjdb.defFlag = pw->controlAttributes.useDefault;

						//required for Win2000 machines - dialog will not recieve mouse clicks if this is not done
						SetCapture();
						ReleaseCapture();

						if (jjdb.DoModal() == IDOK) {

							CRect windowRect(jjdb.m_nColumn,
								jjdb.m_nRow,
								jjdb.m_nColumn + jjdb.m_usWidthMultiplier,
								jjdb.m_nRow + jjdb.m_usHeightMultiplier);

							//validate that the new window does not extend beyond its parents boundaries
							BOOL window = FALSE;
							if(pw->pContainer){
								window = pDoc->ValidateWindow(windowRect,CWindowItemExt::TranslateCWindowControl(pw->pContainer));
							}
							else{
								window = TRUE;
							}
							if(!window)	{
								//alert user and exit funtion - do not create window
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
								return TRUE;
							}

							if (!pDoc->ValidateWindowDoc (windowRect)) {
								AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
								return TRUE;
							}

							//if size of caption is not larger than max size of myCaption, then insert it into the structure
							//else, truncate the characters past max size and then insert
							if (sizeof(jjdb.m_csCaption) <= MaxSizeOfMyCaption){
								pw->myCaption = jjdb.m_csCaption;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption];
								_tcsncpy_s(tempMyCaption, jjdb.m_csCaption, MaxSizeOfMyCaption);
								pw->myCaption = tempMyCaption;
							}
							_tcsncpy_s(pw->windowAttributes.webbrowser.url, jjdb.m_csWebBrowserUrl, 255);
							pw->myName = jjdb.m_csName;
							pw->controlAttributes.m_colorText = jjdb.m_colorText;
							pw->controlAttributes.m_colorFace = jjdb.m_colorFace;
							pw->controlAttributes.m_nRow = jjdb.m_nRow;
							pw->controlAttributes.m_nColumn = jjdb.m_nColumn;
							pw->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
							pw->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
							pw->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
							pw->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
							pw->controlAttributes.isVirtualWindow = jjdb.m_nVirtual;
							pw->controlAttributes.useDefault = jjdb.defFlag;
							pw->windowAttributes.GroupNum = jjdb.m_nGroup;
							pw->windowAttributes.busRules.NumOfPicks  = jjdb.NumPicks;
							pw->controlAttributes.ulMaskGroupPermission = jjdb.m_groupPermission;
							//						pw->controlAttributes.SpecWin[UserDefaultWinIndex] = jjdb.m_csSpecWin[UserDefaultWinIndex];
							for (int i = 0; i < sizeof(pw->controlAttributes.SpecWin); i++){
								pw->controlAttributes.SpecWin[i] = jjdb.m_csSpecWin[i];
							}
							InvalidateRect(NULL,TRUE);

							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

							myTree->UpdateTreeLabel(pw);
							pDoc->CheckPoint();
							return TRUE;
						}//if (jjdb.DoModal() == IDOK)
					}//if((LOWORD(wParam) == IDS_POPUP_EDITWIN && nSearch != 6) || (LOWORD(wParam) == IDS_POPUP_EDITSUBWIN && nSearch == 6))

					//get the control in the window the user wishes to edit
					CWindowControl *pwc = pDoc->SearchForSubWinItem(row, column, pw, uiActiveID);/*pw->GetDataThis ();*/
					//pwc does exist and is valid
					if(pwc != NULL){
						//the control is a button
						if(pwc->controlAttributes.m_nType == CWindowControl::TextControl && LOWORD(wParam) == IDS_POPUP_EDITTEXT /*&& pwc->controlAttributes.m_myId == CWindowText::TypeOEP*/){
							//call function to edit a button
							PopAddTextWindow(row,column);
							return TRUE;
						}
						if(pwc->controlAttributes.m_nType == CWindowControl::TextControl && LOWORD(wParam) == ID_POP1_EDIT_OEPWINDOW && CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowText::TypeOEP){
							//call function to edit a button
							PopSetOEPWin(row,column);
							return TRUE;
						}
						if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl && LOWORD(wParam) == IDS_POPUP_EDITBTN){
							//call function to edit a button
							PopupButtonEditDialog (row, column);
							return TRUE;
						}
					}//(if(pwc != NULL)
				}//if(pw)
			}//if(nSearch>1)
		}
		break;
	case IDS_POPUP_EDITBUTTON:
		PopupButtonEditDialog (row, column);
		break;

	case IDS_POP_SET_OEPWIN:
		PopSetOEPWin(row,column);
		break;

	case IDS_POP_ADD_TEXT_WINDOW:
		PopAddTextWindow (row, column);
		break;

	case IDS_POP_WINDOW_LIST:{
		CDWindowList jj;
		jj.uiActiveID = this->uiActiveID;
		jj.pDoc = pDoc;

		if (jj.DoModal () == IDOK) {
			uiActiveID = jj.uiActiveID;
			InvalidateRect (NULL, FALSE);
		}
							 }
							 break;

	case ID_TREE_DELETE_WINDOW:
		pDoc->DeleteWndItemFromID(uiActiveID);
		InvalidateRect(NULL, FALSE);
		//add check point for redo/undo
		pDoc->CheckPoint();
		break;
	case ID_POP1_REMOVECONTROL:
		pDoc->DeleteWndItem(row, column, uiActiveID);
		InvalidateRect(NULL, FALSE);
		break;

	case ID_POP1_ADDWINDOWGROUP:
		{
			CDWindowGroup *wg = new CDWindowGroup();
			wg->DoModal();
		}
		break;

	case ID_POP1__ADDLABEL:
	case ID_POP1_EDITLBL:
		OnAddLabel();
		break;

	case ID_POP1_ADDLISTBOX:
	case ID_POP1_EDITLISTBOX:
		PopupListBoxEditDialog (row, column);
		break;

	case ID_POP1_ADDEDITBOX:
	case ID_POP1_EDITEDITBOX:
		OnAddEditBox ();
		break;
	}

	return CScrollView ::OnCommand(wParam, lParam);
}

void CNewLayoutView::OnPopSetSubWin()
{
	// TODO: Add your command handler code here
	//get the point that was clicked
	int column = cpLastRButton.x ;
	int row = cpLastRButton.y;

	//convert the coordinates to our grid
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/* check to see if anything exists where we clicked
	0 - no item found
	3 - user clicked in the active window but no control found at this point
	4 - user clicked at a point where a button or text area exists
	6 - user clicked in a window but it isnt the active window */
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);

	if(nSearch > 0){

		CWindowControl *pc = pDoc->GetDataThis();
		//window item that was clicked in
		CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);

		//if user clicked in a window that isnt active
		if(nSearch == 6){
			//call function to assign the subwindow with the active id to parent
			parent = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(parent, uiActiveID);
			//if a subwindow was not found with the active ID we exit function
			if(parent == NULL){
				return;
			}
		}

		//Create a dialog object to add a new window
		CDWinItem jjdb (row, column,0,0, this);
		//set attributes for dialog object
		jjdb.defFlag = TRUE;
		parent->WindowDef.AssignTo(jjdb.df);
		jjdb.m_colorFace = parent->WindowDef.defaultAttributes.WindowDefault;

		//user clicked OK - create window
		if (jjdb.DoModal () == IDOK) {
			//create a new deafult Window Item
			CWindowItemExt *pw = new CWindowItemExt (0, row, column);
			ASSERT(pw);

			//are we using Document Defaults for window attributes
			if(jjdb.defFlag) {
				//assign the Document defaults to the Window defualts
				pDoc->DocumentDef.AssignTo(pw->WindowDef);
			}
			else {
				// We will use the default settings the user designated
				// assigning the user designated defaults to the Window Defualts
				jjdb.df.AssignTo(pw->WindowDef);
			}

			//assign attributes from window creation dialog to the new Window Item

			//if size of caption is not larger than max size of myCaption, then insert it into the structure
			//else, truncate the characters past max size and then insert
			if (sizeof(jjdb.m_csCaption) <= MaxSizeOfMyCaption) {
				pw->myCaption = jjdb.m_csCaption;
			}
			else {
				TCHAR tempMyCaption[MaxSizeOfMyCaption];
				_tcsncpy_s(tempMyCaption, jjdb.m_csCaption, MaxSizeOfMyCaption);
				pw->myCaption = tempMyCaption;
			}

			_tcsncpy_s(pw->windowAttributes.webbrowser.url, jjdb.m_csWebBrowserUrl, 255);
			pw->myName = jjdb.m_csName;
			pw->controlAttributes.m_colorText = jjdb.m_colorText;
			pw->controlAttributes.m_colorFace = jjdb.m_colorFace;
			pw->controlAttributes.m_nRow = jjdb.m_nRow;
			pw->controlAttributes.m_nColumn = jjdb.m_nColumn;
			pw->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
			pw->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
			pw->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
			pw->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
			pw->controlAttributes.isVirtualWindow = jjdb.m_nVirtual;
			pw->controlAttributes.m_nColumn = column;
			pw->controlAttributes.m_nRow = row;
			pw->controlAttributes.useDefault = jjdb.defFlag;
			pw->windowAttributes.GroupNum = jjdb.m_nGroup;
			pw->windowAttributes.busRules.NumOfPicks = jjdb.NumPicks;
			pw->controlAttributes.ulMaskGroupPermission = jjdb.m_groupPermission;
			for (int i = 0; i < sizeof(pw->controlAttributes.SpecWin); i++){
				pw->controlAttributes.SpecWin[i] = jjdb.m_csSpecWin[i];
			}

			//create a CRect contianing the new windows screen area
			CRect thisControl(pw->controlAttributes.m_nColumn,
							  pw->controlAttributes.m_nRow,
							  pw->controlAttributes.m_nColumn + pw->controlAttributes.m_usWidthMultiplier ,
							  pw->controlAttributes.m_nRow + pw->controlAttributes.m_usHeightMultiplier );

			//validate that the new window does not extend beyond the document's boundaries
			if(!pDoc->ValidateWindowDoc(pw)){
				//alert user and exit funtion - do not create window
				AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
				delete pw;
				return;
			}

			//validate that the new window does not extend beyond its parent's boundaries
			if(!pDoc->ValidateWindow(thisControl, parent))	{
				//alert user and exit funtion - do not create window
				AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
				delete pw;
				return;
			}

			// new control passed the size checks and it is good.  Finalize the control
			// attributes and assign it an ID number.
			pw->controlAttributes.m_myId = uiGlobalID++;
			pw->pContainer = parent;
			pw->controlAttributes.CurrentID = uiActiveID;
			pw->windowAttributes.Show = TRUE;
			//hide the parent window
			parent->windowAttributes.Show = FALSE;

			uiActiveID = pw->controlAttributes.m_myId;
			//add the window to its parents control list
			parent->AddDataLast (pw);
			pDoc->CheckPoint();

			//get the view containing our tree control update it with the new window
			CLeftView *myTree = (CLeftView*)m_TreeView;
			ASSERT(myTree);

			myTree->InsertNewWinItem(parent->controlAttributes.m_myId,(CWindowControl *)pw);
			myTree->ShowSelectedWindow(pw->controlAttributes.m_myId);

			//hide the parent window
			parent->windowAttributes.Show = FALSE;
			InvalidateRect (NULL, FALSE);
		}//if(jjdb.DoModal())
	}//if(nSearch > 0)
}

void CNewLayoutView::OnPopSetwindow()
{
	//get the point that was clicked
	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	//convert the coordinates to our grid
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//Get the document containing this view and verify it is valid
	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*check to see if we clicked in an existing window
	nSearch = 1 if window exists at this point
	nSearch = 0 if no window exists at this point*/
	BOOL nSearch = pDoc->SearchForWindow(row,column);

	//Create a dialog object to add a new window
	CDWinItem jjdb (row, column,0,0, this);
	//set attributes for dialog object
	jjdb.defFlag = TRUE;

	//user clicked OK - create window
	if (jjdb.DoModal () == IDOK) {
		//create a new deafult Window Item
		CWindowItemExt *pw = new CWindowItemExt (0, row, column);

		//set flag to indicate we are creating a new window
		BOOL newWin = TRUE;
		//are we using Document Defaults for window attributes
		if(jjdb.defFlag){
			//assign the Document defaults to the Window defualts
			pDoc->DocumentDef.AssignTo(pw->WindowDef);
		}
		//We will use the default settings the user designated
		else{
			//assing the user designated defaults to the Window Defualts
			jjdb.df.AssignTo(pw->WindowDef);
		}

		//assign attributes from window creation dialog to the new Window Item

		//if size of caption is not larger than max size of myCaption, then insert it into the structure
		//else, truncate the characters past max size and then insert
		if (sizeof(jjdb.m_csCaption) <= MaxSizeOfMyCaption){
			pw->myCaption = jjdb.m_csCaption;
		}
		else
		{
			TCHAR tempMyCaption[MaxSizeOfMyCaption];
			_tcsncpy_s(tempMyCaption, jjdb.m_csCaption, MaxSizeOfMyCaption);
			pw->myCaption = tempMyCaption;
		}
		_tcsncpy_s(pw->windowAttributes.webbrowser.url, jjdb.m_csWebBrowserUrl, 255);
		pw->myName = jjdb.m_csName;
		pw->controlAttributes.m_colorText = jjdb.m_colorText;
		pw->controlAttributes.m_colorFace = jjdb.m_colorFace;
		pw->controlAttributes.m_nRow = jjdb.m_nRow;
		pw->controlAttributes.m_nColumn = jjdb.m_nColumn;
		pw->controlAttributes.m_usWidthMultiplier = jjdb.m_usWidthMultiplier;
		pw->controlAttributes.m_usHeightMultiplier = jjdb.m_usHeightMultiplier;
		pw->controlAttributes.m_nWidth = jjdb.m_usWidthMultiplier * CWindowButton::stdWidth;
		pw->controlAttributes.m_nHeight = jjdb.m_usHeightMultiplier * CWindowButton::stdHeight;
		pw->controlAttributes.isVirtualWindow = jjdb.m_nVirtual;
		pw->controlAttributes.m_nColumn = column;
		pw->controlAttributes.m_nRow = row;
		pw->controlAttributes.useDefault = jjdb.defFlag;
		pw->windowAttributes.GroupNum = jjdb.m_nGroup;
		pw->windowAttributes.busRules.NumOfPicks  = jjdb.NumPicks;
		pw->controlAttributes.ulMaskGroupPermission = jjdb.m_groupPermission;
		pw->windowAttributes.Show = TRUE;
		pw->controlAttributes.CurrentID = uiActiveID;
		//populating special window array
		//pw->controlAttributes.SpecWin[UserDefaultWinIndex] = jjdb.m_csSpecWin[UserDefaultWinIndex];
		for (int i = 0; i < sizeof(pw->controlAttributes.SpecWin); i++){
			pw->controlAttributes.SpecWin[i] = jjdb.m_csSpecWin[i];
		}

		//does our window extend beyond the boundaries of the document
		if(!pDoc->ValidateWindowDoc(pw)){
			//alert user and exit function - window is not created
			AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
			delete pw;
			return;
		}

		// new control passed the size checks and it is good.  Finalize the control
		// attributes and assign it an ID number.
		pw->controlAttributes.m_myId = uiGlobalID++;
		uiActiveID = pw->controlAttributes.m_myId;
		//add window to the documents control list
		pDoc->AddDataLast (pw);
		pDoc->CheckPoint();
		//get the view contianing our tree control update it with the new window
		CLeftView *myTree = (CLeftView*)m_TreeView;
		ASSERT(myTree);

		myTree->ShowSelectedWindow(pw->controlAttributes.m_myId);
		myTree->InsertNewWinItem(0,(CWindowControl *)pw);
		myTree->ShowSelectedWindow(pw->controlAttributes.m_myId);
		InvalidateRect (NULL, FALSE);
	}
}


void CNewLayoutView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

	/*Total View Size: @ 800*600 we have a 53 * 40 grid with each square 15 * 15 so our
	total size is (53*15)+2 * (40*15)+2 --> the +2 is for spacing on the side of the screen*/

	/*
	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2); /* Moved line below creation of 'CWindowDocument * pDoc' in order to
	use 'm_nStdDeciInchWidth' and then set scrolling range- CSMALL
	*/

	CWindowDocument * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Creating 'sizeTotal' with CWindowDocument constant variables, rather 
	//  than hard-coding width/height, to control scrolling size/range.  This
	//  way scroll bars are created based on the entire 'document'(layout grid)
	//  being viewed, rather than the 'screenWidth'/'screenHeight'. - CSMALL
	CSize sizeTotal( (pDoc->m_nStdDeciInchWidth)*(pDoc->nGridColumns), 
					 (pDoc->m_nStdDeciInchHeight)*(pDoc->nGridRows) );

	uiActiveID = pDoc->documentAttributes.uiInitialID;;

	SetScrollSizes(MM_TEXT,sizeTotal);

	if (pDoc->documentAttributes.myStartupClientRect.right && pDoc->documentAttributes.myStartupClientRect.bottom) {
		pDoc->documentAttributes.myStartupClientRect.right += 20;
		SetWindowPos (NULL,
					  0,
					  0,
					  pDoc->documentAttributes.myStartupClientRect.right,
					  pDoc->documentAttributes.myStartupClientRect.bottom,
					  SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOZORDER);

		RedrawWindow (NULL,
					  NULL,
					  RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);

		InvalidateRect (NULL, FALSE);
		GetParentFrame()->InvalidateRect(NULL, FALSE);
		GetParentFrame()->SetWindowPos (NULL,
										0,
										0,
										pDoc->documentAttributes.myStartupClientRect.right,
										pDoc->documentAttributes.myStartupClientRect.bottom,
										SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOZORDER);

		GetParentFrame()->RedrawWindow (NULL,
										NULL,
										RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);

		pDoc->documentAttributes.myStartupClientRect.right = pDoc->documentAttributes.myStartupClientRect.bottom = 0;
	}

	// make sure we adjust the global id generator to be past the largest id
	// of the windows in this file.
	CWindowControl *pw = pDoc->GetDataFirst ();
	while (pw) {
		if(pw->controlAttributes.m_myId > uiGlobalID){
			uiGlobalID = pw->controlAttributes.m_myId;
		}
		if(pw->controlAttributes.m_nType == CWindowControl::WindowContainer){
			UINT subID = GetSubID(CWindowItemExt::TranslateCWindowControl(pw));
			if(subID > uiGlobalID){
				uiGlobalID = subID;
			}
		}
		pw = pDoc->GetDataNext ();
	}

	uiGlobalID++;

	InvalidateRect(NULL,TRUE);


}

void CNewLayoutView::CreateTripleWindow(int row, int column, CWindowItemExt *wi, CDTextItem *ti, UINT activeID )
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowControl *pWndCntrl = pDoc->GetDataThis();
	CWindowItemExt *pWndItem = CWindowItemExt::TranslateCWindowControl(pWndCntrl);

	CRect thisControl(column ,
					  row ,
					  column  + ti->m_usWidthMultiplier,
					  row  + ti->m_usHeightMultiplier);
	BOOL grid = pDoc->IsValidGrid(row ,
								  column,
								  activeID,
								  thisControl,
								  NULL);
	BOOL window =pDoc->ValidateWindow(thisControl,wi);
	if(grid && window){
		if(ti->m_usHeightMultiplier < 3){
			AfxMessageBox(IDS_TEXTHEIGHT_ERR, MB_OK, 0);
			return;
		}


		/**************CREATE MAIN TEXT*****************************************/
		CWindowTextExt *main = new CWindowTextExt(ti->myID ,
												  row - wi->controlAttributes.m_nRow,
												  column - wi->controlAttributes.m_nColumn);

		main->textAttributes.type = (CWindowTextExt::CWindowTextType)ti->myID;
		main->controlAttributes.m_myId = uiGlobalID++;
		main->controlAttributes.m_usHeightMultiplier = ti->m_usHeightMultiplier-2;
		main->controlAttributes.m_usWidthMultiplier = ti->m_usWidthMultiplier;
		main->controlAttributes.m_nHeight = (ti->m_usHeightMultiplier-2) * CWindowButton::stdHeight;
		main->controlAttributes.m_nWidth = ti->m_usWidthMultiplier * CWindowButton::stdWidth;
		main->controlAttributes.m_colorFace = ti->colorFace;
		main->controlAttributes.m_colorText = ti->colorText;
		main->controlAttributes.useDefault = ti->defFlag;
		main->controlAttributes.lfControlFont = ti->myTextFont;
		main->pContainer = wi;

		wi->AddDataLast (main);
		InvalidateRect (NULL, FALSE);


		/**************CREATE SUB TEXT1*****************************************/
		CWindowTextExt *subGuest = new CWindowTextExt(ti->myID + 100 ,
													  row - wi->controlAttributes.m_nRow,
													  column - wi->controlAttributes.m_nColumn);

		subGuest->textAttributes.type = (CWindowTextExt::CWindowTextType)(ti->myID + 100);
		subGuest->controlAttributes.m_myId = uiGlobalID++;
		subGuest->controlAttributes.m_nRow = subGuest->controlAttributes.m_nRow + ti->m_usHeightMultiplier - 2;
		subGuest->controlAttributes.m_usHeightMultiplier = 1;
		subGuest->controlAttributes.m_usWidthMultiplier = ti->m_usWidthMultiplier;
		subGuest->controlAttributes.m_nHeight =  CWindowButton::stdHeight;
		subGuest->controlAttributes.m_nWidth = ti->m_usWidthMultiplier * CWindowButton::stdWidth;
		subGuest->controlAttributes.m_colorFace = ti->colorFace;
		subGuest->controlAttributes.m_colorText = ti->colorText;
		subGuest->controlAttributes.lfControlFont = ti->myTextFont;
		subGuest->controlAttributes.useDefault = ti->defFlag;
		subGuest->pContainer = wi;

		wi->AddDataLast (subGuest);
		InvalidateRect (NULL, FALSE);

		/**************CREATE SUB TEXT2*****************************************/
		CWindowTextExt *subTotal = new CWindowTextExt(ti->myID + 200,
													  row - wi->controlAttributes.m_nRow,
													  column - wi->controlAttributes.m_nColumn);

		subTotal->textAttributes.type = (CWindowTextExt::CWindowTextType)(ti->myID + 200);
		subTotal->controlAttributes.m_myId = uiGlobalID++;
		subTotal->controlAttributes.m_nRow = subTotal->controlAttributes.m_nRow + ti->m_usHeightMultiplier - 1;
		subTotal->controlAttributes.m_usHeightMultiplier = 1;
		subTotal->controlAttributes.m_usWidthMultiplier = ti->m_usWidthMultiplier;
		subGuest->controlAttributes.m_nHeight =  CWindowButton::stdHeight;
		subGuest->controlAttributes.m_nWidth = ti->m_usWidthMultiplier * CWindowButton::stdWidth;
		subTotal->controlAttributes.m_colorFace = ti->colorFace;
		subTotal->controlAttributes.m_colorText = ti->colorText;
		subTotal->controlAttributes.lfControlFont = ti->myTextFont;
		subTotal->controlAttributes.useDefault = ti->defFlag;
		subTotal->pContainer = wi;

		wi->AddDataLast (subTotal);
		InvalidateRect (NULL, FALSE);
		CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pWndCntrl);

		CLeftView *myTree = (CLeftView*)m_TreeView;
		myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pWndItem);
	}
	else {
		if(!grid){
			AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
			return;
		}
		if(!window){
			AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
			return;
		}
	}

}

void CNewLayoutView::UpdateTripleWindow(int row, int column, CWindowItemExt *wi, CDTextItem *ti, UINT activeID, CWindowTextExt *wt )
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//save the row and column of the existing text control, so when we create another
	//one to replace the old one we create it in the same position rather than where the
	//user clicked, if not, if we make it larger, we will get an "Extend beyond window error"
	int rowSave = wt->controlAttributes.m_nRow + wi->controlAttributes.m_nRow;
	int columnSave = wt->controlAttributes.m_nColumn + wi->controlAttributes.m_nColumn;

	if(ti->m_usHeightMultiplier < 3){
		AfxMessageBox(IDS_TEXTHEIGHT_ERR, MB_OK, 0);
		return;
	}

	CWindowControl *pwc = wi->GetDataFirst ();
	while(pwc){
		if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
			if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == (CWindowTextExt::CWindowTextType)wt->textAttributes.type ||
				CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == (CWindowTextExt::CWindowTextType)(wt->textAttributes.type + 100) ||
				CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == (CWindowTextExt::CWindowTextType)(wt->textAttributes.type + 200)){
					pDoc->DeleteWndItem(pwc->controlAttributes.m_nRow + wi->controlAttributes.m_nRow, pwc->controlAttributes.m_nColumn + wi->controlAttributes.m_nColumn, uiActiveID);

					InvalidateRect(NULL,FALSE);
			}
		}
		pwc = wi->GetDataNext ();
	}

	CreateTripleWindow(rowSave, columnSave, wi, ti, uiActiveID);

}

void CNewLayoutView::PopAddTextWindow(int row, int column)
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem(row, column, uiActiveID);
	//a window was clicked in
	if(nSearch>1){

		CWindowControl *pWndCntrl = pDoc->GetDataThis();
		CWindowItemExt *pWndItem = CWindowItemExt::TranslateCWindowControl(pWndCntrl);

		//user clicked in an inactive window (sub window)
		if(nSearch == 6){
			//call function to get the window they clicked in
			pWndItem = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pWndItem, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(pWndItem == NULL){
				return;
			}
			//call function to see if an exixting item was clicked in and assign to nSearch
			nSearch = pWndItem->SearchForControl(row,column);
		}
		//this window does exist
		if(pWndItem) {
			if(nSearch ==3){

				//window item that was clicked in
				CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pWndCntrl);

				CDTextItem thisTextArea (NULL,pWndItem);
				// CDTextItem *thisTextArea = new CDTextItem(NULL,pWndItem);

				thisTextArea.pDoc = pDoc;
				thisTextArea.m_nRow = row;
				thisTextArea.m_nColumn = column;
				thisTextArea.defFlag = TRUE;

				int status = thisTextArea.DoModal();
				if(status == IDOK) {
					BOOL cont = pDoc->ValidateText((CWindowTextExt::CWindowTextType)thisTextArea.myID);
					if(!cont){
						AfxMessageBox(IDS_TEXT_ERR, MB_OK, 0);
						return;
					}
					if(thisTextArea.myID == 1 || thisTextArea.myID == 2 || thisTextArea.myID == 3){
						CreateTripleWindow(row, column, pWndItem, &thisTextArea, uiActiveID);
						return;
					}

					CWindowTextExt *pWndText = new CWindowTextExt(thisTextArea.myID,
																  row - pWndItem->controlAttributes.m_nRow,
																  column - pWndItem->controlAttributes.m_nColumn);

					pWndText->textAttributes.type = (CWindowTextExt::CWindowTextType)thisTextArea.myID;
					pWndText->controlAttributes.m_myId = uiGlobalID++;
					pWndText->controlAttributes.m_usHeightMultiplier = thisTextArea.m_usHeightMultiplier;
					pWndText->controlAttributes.m_usWidthMultiplier = thisTextArea.m_usWidthMultiplier;
					pWndText->controlAttributes.m_nHeight = thisTextArea.m_usHeightMultiplier *  CWindowButton::stdHeight;
					pWndText->controlAttributes.m_nWidth = thisTextArea.m_usWidthMultiplier * CWindowButton::stdWidth;
					pWndText->controlAttributes.m_colorFace = thisTextArea.colorFace;
					pWndText->controlAttributes.m_colorText = thisTextArea.colorText;
					pWndText->pContainer = pWndItem;
					pWndText->controlAttributes.useDefault = thisTextArea.defFlag;
					pWndText->controlAttributes.lfControlFont = thisTextArea.myTextFont;

					CRect thisControl(pWndItem->controlAttributes.m_nColumn + pWndText->controlAttributes.m_nColumn,
									  pWndItem->controlAttributes.m_nRow + pWndText->controlAttributes.m_nRow,
									  pWndItem->controlAttributes.m_nColumn + pWndText->controlAttributes.m_nColumn + thisTextArea.m_usWidthMultiplier,
									  pWndItem->controlAttributes.m_nRow + pWndText->controlAttributes.m_nRow + thisTextArea.m_usHeightMultiplier);


					BOOL grid = pDoc->IsValidGrid(row,column,uiActiveID,thisControl,NULL);
					BOOL window =pDoc->ValidateWindow(thisControl,pWndItem);
					if(grid && window){
						pWndItem->AddDataLast (pWndText);
						pDoc->CheckPoint();
						InvalidateRect (NULL, FALSE);
					}
					else {
						if(!grid)
							AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
						if(!window)
							AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

						delete pWndText;
						return;
					}
					CLeftView *myTree = (CLeftView*)m_TreeView;
					ASSERT(myTree);
					myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pWndItem);
				}
				//Dialog exited with cancel...do nothing
			}
			else if(nSearch ==4){
				BOOL addData = FALSE;
				if(pWndItem->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl){
					CWindowTextExt *pb = 0;
					CWindowControl *wc = pWndItem->GetDataThis ();

					if(CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type >100 && CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type  <299){

						CWindowControl *pwc = pWndItem->GetDataFirst ();
						while(pwc){
							if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
								if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type - 100 ||
									CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowTextExt::TranslateCWindowControl(wc)->textAttributes.type - 200){
										pb = CWindowTextExt::TranslateCWindowControl (pwc);
										break;
								}
							}
							pwc = pWndItem->GetDataNext ();
						}
					}
					else{
						pb = CWindowTextExt::TranslateCWindowControl (wc);
					}

					CDTextItem tt(pb, pWndItem);
					BOOL cont;
					tt.pDoc = pDoc;
					tt.defFlag = pb->controlAttributes.useDefault;

					if(tt.DoModal() == IDOK){
						if(pb->textAttributes.type != tt.myID){
							cont = pDoc->ValidateText((CWindowTextExt::CWindowTextType)tt.myID);
							if(!cont){
								AfxMessageBox(IDS_TEXT_ERR, MB_OK, 0);
								return;
							}
						}

						//if ID is for tri-screen text, we cannot update controls here
						if(tt.myID == 1 || tt.myID == 2 || tt.myID == 3){
							UpdateTripleWindow(row,column,/*pb->controlAttributes.m_nRow, pb->controlAttributes.m_nColumn,*/ pWndItem, &tt, uiActiveID, pb);
							pDoc->CheckPoint();
							return;
						}
						/*if text control we are editing is tri-screen type and we are changing to non-triscreen
						we must delete sub windows of tri-screen control*/
						if((pb->textAttributes.type == CWindowText::TypeMultiReceipt1 ||
							pb->textAttributes.type == CWindowText::TypeMultiReceipt2 ||
							pb->textAttributes.type == CWindowText::TypeMultiReceipt3) &&
							(tt.myID != 1 || tt.myID != 2 || tt.myID != 3)){
								CWindowControl *bi = 0;

								POSITION pos = pDoc->listControls.GetHeadPosition ();

								while (pos){
									pDoc->pclListCurrent = pos;
									bi = pDoc->listControls.GetNext (pos);


									CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
									CWindowControl *pwc = pw->GetDataFirst ();

									while(pwc){
										if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
											if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == pb->textAttributes.type  ||
												CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == pb->textAttributes.type + 200 ||
												CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == pb->textAttributes.type + 200){
													addData = pDoc->DeleteWndItem(pwc->controlAttributes.m_nRow + pw->controlAttributes.m_nRow, pwc->controlAttributes.m_nColumn + pw->controlAttributes.m_nColumn, uiActiveID);

											}
										}
										pwc = pw->GetDataNext ();
									}
								}
						}
						CRect thisControl(pWndItem->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn,
										  pWndItem->controlAttributes.m_nRow + pb->controlAttributes.m_nRow,
										  pWndItem->controlAttributes.m_nColumn + pb->controlAttributes.m_nColumn + tt.m_usWidthMultiplier,
										  pWndItem->controlAttributes.m_nRow + pb->controlAttributes.m_nRow + tt.m_usHeightMultiplier);

						BOOL grid = pDoc->IsValidGrid(row,
													  column,
													  uiActiveID,
													  thisControl,
													  pb);

						BOOL window =pDoc->ValidateWindow(thisControl,pWndItem);

						if(grid && window){
							pb->textAttributes.type = (CWindowTextExt::CWindowTextType)tt.myID;
							pb->controlAttributes.m_nRow = tt.m_nRow;
							pb->controlAttributes.m_nColumn = tt.m_nColumn;
							pb->controlAttributes.m_usHeightMultiplier = tt.m_usHeightMultiplier;
							pb->controlAttributes.m_usWidthMultiplier = tt.m_usWidthMultiplier;
							pb->controlAttributes.m_nHeight = tt.m_usHeightMultiplier *  CWindowButton::stdHeight;
							pb->controlAttributes.m_nWidth = tt.m_usWidthMultiplier * CWindowButton::stdWidth;
							pb->controlAttributes.m_colorText = tt.colorText;
							pb->controlAttributes.m_colorFace = tt.colorFace;
							pb->controlAttributes.useDefault = tt.defFlag;
							pb->controlAttributes.lfControlFont = tt.myTextFont;

							if(addData){
								pb->pContainer = pWndItem;
								pWndItem->AddDataLast(pb);
							}

							pDoc->CheckPoint();
							CLeftView *myTree = (CLeftView*)m_TreeView;
							ASSERT(myTree);

							myTree->UpdateTextControlTree(pb);

							InvalidateRect(NULL,FALSE);
						}
						else {
							if(!grid)
								AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}
					}
				}
				else {
					AfxMessageBox(IDS_BTNEXIST,  MB_OK,  0 );
				}
			}
			//Item not found by search...do nothing
		}
		//WindowItemExt pointer invalid...do nothing
	}
	//parent document pointer invalid...do nothing
}//end OnPopAddTextWindow()


void CNewLayoutView::OnFileTest()
{
	// TODO: Add your command handler code here
	CWindowDocumentExt * pDoc = GetDocument();
	pDoc->CreateDocument();

}



/*void CNewLayoutView::OnViewScaletofitwindow()
{
// TODO: Add your command handler code here
//CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);
CSize sizeTotal = this->GetTotalSize( );
m_ZoomToFit = !m_ZoomToFit;
if(m_ZoomToFit){
SetScrollSizes(MM_TEXT,sizeTotal);
}
else
SetScaleToFitSize(sizeTotal);
}*/

/*void CNewLayoutView::OnUpdateViewScaletofitwindow(CCmdUI* pCmdUI)
{
// TODO: Add your command update UI handler code here
pCmdUI->SetCheck(!m_ZoomToFit);

}*/

// IMPORTANT NOTE:
//     The following handlers are for the setting of the target screen resolution.
//     If the maximum possible size should change then in addition to adding a new define,
//     the maximum screen size must also be changed in the following variables:
//        const int  CWindowDocument::nGridColumns = 128;
//        const int  CWindowDocument::nGridRows = 72;
void CNewLayoutView::OnViewResolution800()
{
	// TODO: Add your command handler code here
	ScreenWidth = LAYOUT_SCREENWIDTH;
	ScreenHeight = LAYOUT_SCREENHEIGHT;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/
	m_pParent->InvalidateRect(NULL,FALSE);

}

void CNewLayoutView::OnViewResolution1024()
{
	// TODO: Add your command handler code here
	ScreenWidth = LAYOUT_SCREENWIDTH_HIRES;
	ScreenHeight = LAYOUT_SCREENHEIGHT_HIRES;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);

}

void CNewLayoutView::OnViewResolution1280()
{
	ScreenWidth = LAYOUT_SCREENWIDTH_1280;
	ScreenHeight = LAYOUT_SCREENHEIGHT_1280;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);
}

void CNewLayoutView::OnViewResolution1440()
{
	ScreenWidth = LAYOUT_SCREENWIDTH_1440;
	ScreenHeight = LAYOUT_SCREENHEIGHT_1440;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);
}

void CNewLayoutView::OnViewResolution1920()
{
	ScreenWidth = LAYOUT_SCREENWIDTH_1920;
	ScreenHeight = LAYOUT_SCREENHEIGHT_1920;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);
}

void CNewLayoutView::OnViewResolution1680()
{
	ScreenWidth = LAYOUT_SCREENWIDTH_1680;
	ScreenHeight = LAYOUT_SCREENHEIGHT_1680;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);
}

void CNewLayoutView::OnViewResolution2560()
{
	ScreenWidth = LAYOUT_SCREENWIDTH_2560;
	ScreenHeight = LAYOUT_SCREENHEIGHT_2560;

	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	m_pParent->ResLoWidth = ScreenWidth;		//indicates the width in pixels
	m_pParent->ResLoHeight = ScreenHeight;	//indicates the height in pixels

	/*	CSize sizeTotal(ScreenWidth + 2,ScreenHeight + 2);		//No need to reset Scrolling - both resolutions are (and should be) able to scroll entire grid - CSMALL
	SetScrollSizes(MM_TEXT,sizeTotal);
	*/

	m_pParent->InvalidateRect(NULL,FALSE);
}


void CNewLayoutView::OnUpdateViewResolution800(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(ScreenWidth == LAYOUT_SCREENWIDTH && ScreenHeight == LAYOUT_SCREENHEIGHT){
		pCmdUI->SetCheck(TRUE);
	} else {
		pCmdUI->SetCheck(FALSE);
	}
}

void CNewLayoutView::OnUpdateViewResolution1024(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_HIRES && ScreenHeight == LAYOUT_SCREENHEIGHT_HIRES){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}

void CNewLayoutView::OnUpdateViewResolution1280(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_1280 && ScreenHeight == LAYOUT_SCREENHEIGHT_1280){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}

void CNewLayoutView::OnUpdateViewResolution1440(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_1440 && ScreenHeight == LAYOUT_SCREENHEIGHT_1440){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}

void CNewLayoutView::OnUpdateViewResolution1680(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_1680 && ScreenHeight == LAYOUT_SCREENHEIGHT_1680){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}

void CNewLayoutView::OnUpdateViewResolution1920(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_1920 && ScreenHeight == LAYOUT_SCREENHEIGHT_1920){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}

void CNewLayoutView::OnUpdateViewResolution2560(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(	ScreenWidth == LAYOUT_SCREENWIDTH_2560 && ScreenHeight == LAYOUT_SCREENHEIGHT_2560){
		pCmdUI->SetCheck(TRUE);
	}
	else {
		pCmdUI->SetCheck(FALSE);
	}

}


void CNewLayoutView::PopupDefaultWinDlg(BOOL newWin, DefaultData *df)
{
	DefaultEdit DefDlg;
	DefaultData myData;

	//if we are creating a new window, we initialize the settings to the default doc settings
	if(newWin){
		CWindowDocumentExt * pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		DefDlg.ButtonDefault = pDoc->DocumentDef.defaultAttributes.ButtonDefault;
		DefDlg.WindowDefault = pDoc->DocumentDef.defaultAttributes.WindowDefault;
		DefDlg.TextWinDefault = pDoc->DocumentDef.defaultAttributes.TextWinDefault;

		DefDlg.FontDefault = pDoc->DocumentDef.defaultAttributes.FontDefault;
		DefDlg.FontColorDefault = pDoc->DocumentDef.defaultAttributes.FontColorDefault;

		DefDlg.ShapeDefault = pDoc->DocumentDef.defaultAttributes.ShapeDefault;
		DefDlg.PatternDefault = pDoc->DocumentDef.defaultAttributes.PatternDefault;

		DefDlg.WidthDefault = pDoc->DocumentDef.defaultAttributes.WidthDefault;
		DefDlg.HeightDefault = pDoc->DocumentDef.defaultAttributes.HeightDefault;
	}
	//if we are changing defaults for an existing window, we initialize to existing window defaults
	else {
		DefDlg.ButtonDefault = df->defaultAttributes.ButtonDefault;
		DefDlg.WindowDefault = df->defaultAttributes.WindowDefault;
		DefDlg.TextWinDefault = df->defaultAttributes.TextWinDefault;

		DefDlg.FontDefault = df->defaultAttributes.FontDefault;
		DefDlg.FontColorDefault = df->defaultAttributes.FontColorDefault;


		DefDlg.ShapeDefault = df->defaultAttributes.ShapeDefault;
		DefDlg.PatternDefault = df->defaultAttributes.PatternDefault;

		DefDlg.WidthDefault = df->defaultAttributes.WidthDefault;
		DefDlg.HeightDefault = df->defaultAttributes.HeightDefault;
	}

	if(DefDlg.DoModal() == IDOK){

		df->defaultAttributes.ButtonDefault = DefDlg.ButtonDefault;
		df->defaultAttributes.WindowDefault = DefDlg.WindowDefault;
		df->defaultAttributes.TextWinDefault = DefDlg.TextWinDefault;

		df->defaultAttributes.FontDefault = DefDlg.FontDefault;
		df->defaultAttributes.FontColorDefault = DefDlg.FontColorDefault;

		df->defaultAttributes.ShapeDefault = DefDlg.ShapeDefault;
		df->defaultAttributes.PatternDefault = DefDlg.PatternDefault;
		df->defaultAttributes.WidthDefault = DefDlg.WidthDefault;
		df->defaultAttributes.HeightDefault = DefDlg.HeightDefault;
	}

}

void CNewLayoutView::OnChangeDefaults()
{
	// TODO: Add your command handler code here
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	COLORREF oldWinColor = pDoc->DocumentDef.defaultAttributes.WindowDefault;		//temporarily save old default window color
	pDoc->PopupDefaultDlg();

	CWindowControl *bi = 0;
	POSITION pos = pDoc->listControls.GetHeadPosition ();

	while (pos){
		pDoc->pclListCurrent = pos;
		bi = pDoc->listControls.GetNext (pos);

		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
		//if this window uses the document default window color, then set the window color to the new document window color
		if(pw->controlAttributes.m_colorFace == oldWinColor){
			pw->controlAttributes.m_colorFace = pDoc->DocumentDef.defaultAttributes.WindowDefault;
		}
		//if this window uses the document defaults, then set its defaults to the new document defaults
		if(pw->controlAttributes.useDefault){
			pDoc->DocumentDef.AssignTo(pw->WindowDef);
		}
		//cycle through the controls in this window and set new attributes if they use defualts
		CWindowControl *pwc = pw->GetDataFirst ();
		while(pwc){
			switch(pwc->controlAttributes.m_nType){
			case CWindowControl::TextControl:		//text
				if(pwc->controlAttributes.useDefault){
					pwc->controlAttributes.m_colorFace = pw->WindowDef.defaultAttributes.TextWinDefault;
					pwc->controlAttributes.m_colorText = pw->WindowDef.defaultAttributes.FontColorDefault;
				}
				break;
			case CWindowControl::ButtonControl:		//button
				if(pwc->controlAttributes.useDefault){
					CWindowButton *wb = CWindowButton::TranslateCWindowControl (pwc);
					wb->controlAttributes.m_colorFace = pw->WindowDef.defaultAttributes.ButtonDefault;
					wb->controlAttributes.m_colorText = pw->WindowDef.defaultAttributes.FontColorDefault;
					wb->controlAttributes.m_usHeightMultiplier = pw->WindowDef.defaultAttributes.HeightDefault;
					wb->controlAttributes.m_usWidthMultiplier = pw->WindowDef.defaultAttributes.WidthDefault;
					wb->btnAttributes.m_myShape = pw->WindowDef.defaultAttributes.ShapeDefault;
					wb->btnAttributes.m_myPattern = pw->WindowDef.defaultAttributes.PatternDefault;
				}
				break;
			}
			pwc = pw->GetDataNext ();
		}

	}
	InvalidateRect (NULL, TRUE);
}

// N-KSo Why the screen flashes on LButtonDown?
void CNewLayoutView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	moveDragControl = TRUE;

	CPoint pt = GetDeviceScrollPosition();
	point.x += pt.x;
	point.y += pt.y;

	cpLastLButton = point;

	int column = point.x;
	int row = point.y;

	column /= CWindowButtonExt::stdWidth;
	row    /= CWindowButtonExt::stdHeight;

	CDC *pDC = GetDC();
	int iSaveDC = pDC->SaveDC();
	///////////////////////////////////////////
	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);

	//a window was clicked in
	if (nSearch > 1) {
		CWindowControl *pc = pDoc->GetDataThis ();
		CWindowItemExt *ww = CWindowItemExt::TranslateCWindowControl (pc);
		//user clicked in an inactive window or sub window
		if(nSearch == 6){

			//call function to get the window they clicked in and assign to wc
			CWindowItemExt *wc = pDoc->GetSelectedDocWindow( uiActiveID);

			if (wc) {
				///////////////////////////////////////////
				//create rectangle to check to see if the
				//user selection is in the correct window
				CRect hp(wc->controlAttributes.m_nColumn,
					wc->controlAttributes.m_nRow,
					wc->controlAttributes.m_nColumn + wc->controlAttributes.m_usWidthMultiplier,
					wc->controlAttributes.m_nRow + wc->controlAttributes.m_usHeightMultiplier);

				//if the window isnt a subwindow of the active window, exit function
				if((ww->controlAttributes.m_myId == wc->controlAttributes.m_myId) && (hp.PtInRect(CPoint(column, row)) ) ){
					//PopupButtonEditDialog (row, column);
					nSearch = ww->SearchForControl(row,column);
				}
			}
			//return;
		}
		//a control exist at the point where the user clicked
		if(nSearch == 4){
			//pw is valid and not null
			if (ww) {
				//get the window control of the window that was clciked
				CWindowControl *pwc = ww->GetDataThis ();

				//if pwc is valid and not null
				if(pwc != NULL){
					if(pwc->controlAttributes.m_nType == CWindowControl::TextControl && CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowText::TypeOEP){
						dragControl = ww;
					}
					else{
						dragControl = pwc;
					}
				}
				//user clicked in a blank area of window - call funtion to add button control
				else{
					dragControl = NULL;
				}
			}//if(pw)
		}//if(nSearch ==4)
		if(nSearch == 3){
			dragControl = ww;
		}
	}//if(nSearch > 1)
	//user clciked area of documents view where nothing exist - call funtion to add window
	else{
		//set the coordinated of the point to be clciked in for the window edit function
		dragControl = NULL;
	}

	if(nFlags == MK_CONTROL + MK_LBUTTON)
	{
		HighlightSelection(pDC, row, column);
	}
	if(nFlags ==  MK_LBUTTON)
	{
		ClearSelection((CWindowControl*)pDoc);
		OnDraw(pDC);
		HighlightSelection(pDC, row, column);
	}

	pDC->RestoreDC (iSaveDC);
	CScrollView::OnLButtonDown(nFlags, point);
}

void CNewLayoutView::ClearSelection(CWindowControl *wc)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(wc->controlAttributes.m_nType == CWindowControl::DocumentContainer){
		ClearDocumentSelection();
		return;
	}
	if(wc->controlAttributes.m_nType == CWindowControl::WindowContainer){
		CWindowItemExt* wi = CWindowItemExt::TranslateCWindowControl(wc);

		CWindowControl *bi = 0;
		POSITION pos = wi->ButtonItemList.GetHeadPosition ();

		while (pos){
			bi = wi->ButtonItemList.GetNext (pos);

			if(bi->controlAttributes.m_nType == CWindowControl::WindowContainer){
				CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
				pw->controlAttributes.Selected = FALSE;

				CWindowControl *pwc = pw->GetDataFirst ();

				while(pwc){
					(CWindowTextExt::TranslateCWindowControl(pwc))->controlAttributes.Selected = FALSE;
					pwc = pw->GetDataNext ();
				}
			}
			else{
				(CWindowTextExt::TranslateCWindowControl(bi))->controlAttributes.Selected = FALSE;
			}
		}
	}

}


void CNewLayoutView::HighlightSelectionOLD(CDC *pDC, int row, int column)
{
	TRACE("CNewLayoutView::HighlightSelectionOLD\n");
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	SEARCHRC nSearch = (SEARCHRC) pDoc->SearchForItem (row, column, uiActiveID);
	if (nSearch > SR_NoItemFound) {

		CWindowControl *pc = pDoc->GetDataThis();
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

		//Set red dotted border for selected items
		CPen wcBorder;
		wcBorder.CreatePen(PS_DOT, 1, SELECTED_CONTROL);
		CPen* pOldPen = pDC->SelectObject(&wcBorder);
		CBrush* pOldBrush = (CBrush *)pDC->SelectObject(GetStockObject (HOLLOW_BRUSH));

		if (pw) {
			//window found with correct point and ID matches active ID
			if (nSearch == SR_ActiveWindow) {
				pw->controlAttributes.Selected = !pw->controlAttributes.Selected;
				CWindowControl *pwc = pw->GetDataFirst ();
				while(pwc != NULL){
					pwc->controlAttributes.Selected = pw->controlAttributes.Selected;
					pwc = pw->GetDataNext ();
				}
				InvalidateRect(NULL,TRUE);
			}
			//control within window was selected
			if (nSearch == SR_ButtonOrText) {
				CWindowControl *wc = pw->GetDataThis ();
				wc->controlAttributes.Selected = !wc->controlAttributes.Selected;
				InvalidateRect(NULL,TRUE);
			}
			if(nSearch == SR_InactiveWindow){
				CWindowControl *pc = pDoc->GetDataThis();
				CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

				CWindowItemExt * selectedWin = pDoc->GetSelectedDocWindow(uiActiveID);
				if(selectedWin != NULL){
					CWindowControl *selectedControl = pDoc->SearchForSubWinItem(row, column, selectedWin, uiActiveID);

					if(selectedControl->controlAttributes.m_nType == CWindowControl::WindowContainer){
						CWindowItemExt *sw = CWindowItemExt::TranslateCWindowControl(selectedControl);
						sw->controlAttributes.Selected = !sw->controlAttributes.Selected;

						CWindowControl *pwc = sw->GetDataFirst ();
						while(pwc != NULL){
							pwc->controlAttributes.Selected = pwc->controlAttributes.Selected;
							pwc = sw->GetDataNext ();
						}
						InvalidateRect(NULL,TRUE);
					}
					else{
						selectedControl->controlAttributes.Selected = !selectedControl->controlAttributes.Selected;
						InvalidateRect(NULL,TRUE);
					}

				}
				else{
					//search doc list for win
					CWindowControl *bi = 0;
					POSITION pos = pDoc->listControls.GetHeadPosition ();

					while (pos) {
						pDoc->pclListCurrent = pos;
						bi = pDoc->listControls.GetNext (pos);

						CRect jj(bi->controlAttributes.m_nColumn,
							bi->controlAttributes.m_nRow,
							bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier,
							bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);

						if (jj.PtInRect (CPoint (column, row))) {
							CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);

							pw->controlAttributes.Selected = !pw->controlAttributes.Selected;
							uiActiveID = pw->controlAttributes.m_myId;
							CLeftView *myTree = (CLeftView*)m_TreeView;
							myTree->ShowSelectedWindow(uiActiveID);
							CWindowControl *pwc = pw->GetDataFirst ();
							while(pwc != NULL){
								pwc->controlAttributes.Selected = pw->controlAttributes.Selected;
								pwc = pw->GetDataNext ();
							}
						}
					}
				}
			}
		} // ***sigh***
	}
	//user clicked in a blank area
	else{

		CWindowControl *bi = 0;
		POSITION pos = pDoc->listControls.GetHeadPosition ();

		while (pos){
			pDoc->pclListCurrent = pos;
			bi = pDoc->listControls.GetNext (pos);
			if(bi->controlAttributes.m_nType != CWindowControl::WindowContainer){
				continue;
			}
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
			CRect jj(bi->controlAttributes.m_nColumn,
				bi->controlAttributes.m_nRow,
				bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier,
				bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);

			if (jj.PtInRect (CPoint (column, row))) {
				uiActiveID = bi->controlAttributes.m_myId;

				CWindowControl *pwc = pw->GetDataFirst ();
				while(pwc){
					CRect rr;

					rr.SetRect(bi->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn,
						bi->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow,
						bi->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier,
						bi->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);
					if (rr.PtInRect (CPoint (column, row))){
						pwc->controlAttributes.Selected = TRUE;
						HighlightSelection(pDC,row,column);
					}

					pwc = pw->GetDataNext ();
				}
				HighlightSelection(pDC,row,column);
			}
			else{
				pw->controlAttributes.Selected = FALSE;
				CWindowControl *pwc = pw->GetDataFirst ();

				while(pwc){
					pwc->controlAttributes.Selected = FALSE;
					pwc = pw->GetDataNext ();
				}
			}

		}
	}
	InvalidateRect (NULL, TRUE);
}

void CNewLayoutView::OnPopupCut()
{
	// TODO: Add your command handler code here
	CWindowControl * tempControl;

	CNewLayoutApp* myApp = (CNewLayoutApp*)AfxGetApp();

	myApp->ClipBoardList.RemoveAll();
	ASSERT(myApp->ClipBoardList.IsEmpty());

	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);
	CWindowControl *pc = pDoc->GetDataThis();

	if(nSearch == 6){
		//call function to get the window they clicked in
		pc = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pc), uiActiveID);
		//if the window isnt a subwindow of the active window, exit function
		if(pc == NULL){
			return;
		}
	}

	while (pc) {

		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);
		if(pw->controlAttributes.Selected){
			tempControl = CopyControl((CWindowControl*)pw,row,column);
			myApp->ClipBoardList.AddTail(tempControl);
			if(nSearch == 6){
				RemoveSubWin(pw);
			}
			else{
				pDoc->DeleteDataThis();
			}
			CLeftView *myTree = (CLeftView*)m_TreeView;
			(myTree->GetTreeCtrl()).DeleteAllItems();
			myTree->OnInitialUpdate();
			return;
		}
		else{

			POSITION  currentPos = pw->ButtonItemList.GetHeadPosition ();
			POSITION  currentPosLast = currentPos;
			while (currentPos) {
				CWindowControl *pwc = pw->ButtonItemList.GetNext (currentPos);
				// control is a text item
				if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
					if(CWindowTextExt::TranslateCWindowControl(pwc)->textAttributes.type == CWindowText::TypeOEP){
						tempControl = CopyControl(CWindowItemExt::TranslateCWindowControl(pwc->pContainer),row,column);
						myApp->ClipBoardList.AddTail(tempControl);
						pDoc->DeleteDataThis();
						continue;
					}
					CWindowTextExt *ti = CWindowTextExt::TranslateCWindowControl (pwc);
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
						pw->ButtonItemList.RemoveAt(currentPosLast);
					}
				}
				// control is a button
				else if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){
					CWindowButtonExt *ti = CWindowButtonExt::TranslateCWindowControl (pwc);
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
						pw->ButtonItemList.RemoveAt(currentPosLast);
					}
				}
				// control is a label
				else if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
					CWindowLabelExt *ti = CWindowLabelExt::TranslateCWindowControl (pwc);
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
						pw->ButtonItemList.RemoveAt(currentPosLast);
					}
				}
				currentPosLast = currentPos;
			}
		}
		pc = pDoc->GetDataNext ();
	}
	pDoc->CheckPoint();
	InvalidateRect (NULL, FALSE);

}

void CNewLayoutView::OnPopupCopy()
{

	// TODO: Add your command handler code here
	//temporary control to hold the copy of what the control to copy
	CWindowControl * tempControl;

	CNewLayoutApp* myApp = (CNewLayoutApp*)AfxGetApp();

	//clear out the clip board list on each new copy
	myApp->ClipBoardList.RemoveAll();
	ASSERT(myApp->ClipBoardList.IsEmpty());

	//get the point the user clicked
	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	//get the vies document and verify its valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control -
	4- control exists here
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);

	//get the top level window the user clicked in
	CWindowControl *pc = pDoc->GetDataThis();

	//user clicked in an inactive window (sub window)
	if(nSearch == 6){
		//call function to get the window they clicked in
		pc = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(CWindowItemExt::TranslateCWindowControl(pc), uiActiveID);
		//if the window isnt a subwindow of the active window, exit function
		if(pc == NULL){
			return;
		}
	}

	while (pc) {

		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);
		//if the window is selected
		if(pw->controlAttributes.Selected){
			//copy the the window and everything contianed in it to tempControl
			tempControl = CopyControl((CWindowControl*)pw,row,column);
			//add the copied control to the clip board list
			myApp->ClipBoardList.AddTail(tempControl);
			return;
		}
		//if a control(s) within the window are selected but the window is not
		else {
			//cycle through the windows control list to check each control within the window
			POSITION  currentPos = pw->ButtonItemList.GetHeadPosition ();
			POSITION  currentPosLast = currentPos;
			while (currentPos) {
				CWindowControl *pwc = pw->ButtonItemList.GetNext (currentPos);
				// control is a text item
				if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){
					CWindowTextExt *ti = CWindowTextExt::TranslateCWindowControl (pwc);
					//is the control selected - copy it and add to clip board list
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
					}
				}
				// control is a button
				else if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){
					CWindowButtonExt *ti = CWindowButtonExt::TranslateCWindowControl (pwc);
					//is the control selected - copy it and add to clip board list
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
					}
				}
				// control is a label
				else if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
					CWindowLabelExt *ti = CWindowLabelExt::TranslateCWindowControl (pwc);
					//is the control selected - copy it and add to clip board list
					if(ti->controlAttributes.Selected){
						tempControl = CopyControl(pwc,row,column);
						myApp->ClipBoardList.AddTail(tempControl);
					}
				}
				currentPosLast = currentPos;
			}
		}
		pc = pDoc->GetDataNext ();
	}

}

void CNewLayoutView::OnPopupPaste()
{

	CNewLayoutApp* myApp = (CNewLayoutApp*)AfxGetApp();

	shiftCol = 0;
	shiftRow = 0;
	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowItemExt *pw = NULL;

	int nSearch = pDoc->SearchForItem (row, column,uiActiveID);
	if(nSearch){
		CWindowControl *pc = pDoc->GetDataThis ();
		pw = CWindowItemExt::TranslateCWindowControl(pc);
	}
	// if the selected paste position is not part of a window,
	//  then indicate pasting not allowed and exit paste 
	//  function. -CSMALL 10/04/05
	else {
		AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
		return;
	}

	POSITION currentPos = myApp->ClipBoardList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	int cnt = 0;
	while (currentPos) {
		CWindowControl *wc = myApp->ClipBoardList.GetNext (currentPos);
		if(wc->controlAttributes.m_nType == CWindowControl::WindowContainer){

			CWindowItemExt* tempWindow = CreateNewWindow(wc,row,column);
			if(!tempWindow){
				return;
			}
			tempWindow->windowAttributes.Show = TRUE;


			//ONCE SUBWINDOWS ARE IMPLEMENTED, REMOVE THE FOLLOWING 4 LINES
			/*start remove*/
			pDoc->AddDataLast(tempWindow);
		}
		/*end remove*/
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////DO NOT REMOVE - THIS CODE IS NEEDED TO IMPLEMENT SUBWINDOWS///////////////////////////////////////////////////

		/*if(nSearch == 0){
		pDoc->AddDataLast (tempWindow);
		}
		else{
		//user is trying to paste to a subwindow
		if(nSearch == 6){
		//call function to get the window they clicked in
		pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
		//if the window isnt a subwindow of the active window, exit function
		if(pw == NULL){
		return;
		}
		}
		//create a CRect contianing the new windows screen area
		CRect thisControl(tempWindow->controlAttributes.m_nColumn,
		tempWindow->controlAttributes.m_nRow,
		tempWindow->controlAttributes.m_nColumn + tempWindow->controlAttributes.m_usWidthMultiplier ,
		tempWindow->controlAttributes.m_nRow + tempWindow->controlAttributes.m_usHeightMultiplier );
		//validate that the new window does not extend beyond its parents boundaries
		BOOL window = pDoc->ValidateWindow(thisControl,pw);

		if(!window)	{
		//alert user and exit funtion - do not create window
		AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
		return;
		}
		tempWindow->pContainer = pw;
		pw->AddDataLast(tempWindow);
		pw->windowAttributes.Show = FALSE;
		}
		uiActiveID = tempWindow->controlAttributes.m_myId;
		}*/

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(wc->controlAttributes.m_nType == CWindowControl::TextControl){
			cnt++;
			CWindowTextExt *pwc = CreateNewText(wc,pw,row,column,cnt);
			if(pwc != NULL){
				if(pDoc->ValidateText(pwc->textAttributes.type)){
					pwc->pContainer = pw;
					pw->AddDataLast (pwc);
					pw->windowAttributes.Show = TRUE;
				}
			}
			else {
				AfxMessageBox(IDS_DUP_TEXT, MB_OK, 0);
			}
		}
		if(wc->controlAttributes.m_nType == CWindowControl::ButtonControl) {
			cnt++;
			CWindowButtonExt *pwc = CreateNewButton(wc,pw,row,column,cnt);
			if(pwc != NULL) {
				pwc->pContainer = pw;
				pw->AddDataLast (pwc);
				pw->windowAttributes.Show = TRUE;
			}
		}
		if(wc->controlAttributes.m_nType == CWindowControl::LabelControl){
			cnt++;
			CWindowLabelExt *pwc = CreateNewLabel(wc,pw,row,column,cnt);
			if(pwc != NULL) {
				pwc->pContainer = pw;
				pw->AddDataLast (pwc);
				pw->windowAttributes.Show = TRUE;
			}
		}
		currentPosLast = currentPos;
	}

	UINT saveID = uiActiveID;
	CLeftView *myTree = (CLeftView*)m_TreeView;
	ASSERT(myTree);

	(myTree->GetTreeCtrl()).DeleteAllItems();
	uiActiveID = saveID;
	myTree->OnInitialUpdate();
	myTree->ShowSelectedWindow(uiActiveID);
	InvalidateRect (NULL, FALSE);
	pDoc->CheckPoint();
}

CWindowControl* CNewLayoutView::CopyControl(CWindowControl *pwc,int row, int column)
{
	//we are copying a window and everything in it
	if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl (pwc);

		//create a new window using the selected windows attributes
		CWindowItemExt* tempWindow = new CWindowItemExt(pwc);

		wi->WindowDef.AssignTo(tempWindow->WindowDef);
		tempWindow->windowAttributes.GroupNum = CWindowItemExt::TranslateCWindowControl(pwc)->windowAttributes.GroupNum;

		//cycle through all the windows controls so we can copy them
		CWindowControl *wc = wi->GetDataFirst ();
		while(wc){
			//control is a sub window
			if(wc->controlAttributes.m_nType == CWindowControl::WindowContainer){
				CWindowControl* tempSubWindow = CopyControl(wc,row,column);
				tempWindow->AddDataLast(CWindowItemExt::TranslateCWindowControl(tempSubWindow));

			}
			//control is a text area
			if(wc->controlAttributes.m_nType == CWindowControl::TextControl){

				CWindowTextExt *wt = new CWindowTextExt(wc);

				tempWindow->AddDataLast (wt);
				InvalidateRect (NULL, FALSE);

			}
			//control is a button
			if(wc->controlAttributes.m_nType == CWindowControl::ButtonControl){
				CWindowButtonExt *pb = new CWindowButtonExt(wc);

				pb->btnAttributes.capAlignment = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.capAlignment;
				pb->btnAttributes.horIconAlignment = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.horIconAlignment;
				pb->btnAttributes.vertIconAlignment = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.vertIconAlignment;
				pb->btnAttributes.HorizontalOriented = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.HorizontalOriented;
				pb->m_Icon = CWindowButtonExt::TranslateCWindowControl(wc)->m_Icon;
				pb->btnAttributes.m_myShape = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.m_myShape;
				pb->btnAttributes.m_myPattern = CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.m_myPattern;

				memcpy (&(pb->btnAttributes.m_myActionUnion), &CWindowButtonExt::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion, sizeof (pb->btnAttributes.m_myActionUnion));

				tempWindow->AddDataLast (pb);
				InvalidateRect (NULL, FALSE);
			}
			//control is a label
			if(wc->controlAttributes.m_nType == CWindowControl::LabelControl){
				CWindowLabelExt *pb = new CWindowLabelExt(wc);

				pb->labelAttributes.capAlignment = CWindowLabelExt::TranslateCWindowControl(wc)->labelAttributes.capAlignment;
				pb->labelAttributes.horIconAlignment = CWindowLabelExt::TranslateCWindowControl(wc)->labelAttributes.horIconAlignment;
				pb->labelAttributes.vertIconAlignment = CWindowLabelExt::TranslateCWindowControl(wc)->labelAttributes.vertIconAlignment;
				pb->labelAttributes.HorizontalOriented = CWindowLabelExt::TranslateCWindowControl(wc)->labelAttributes.HorizontalOriented;
				pb->m_IconName = CWindowLabelExt::TranslateCWindowControl(wc)->m_IconName;

				tempWindow->AddDataLast (pb);
				InvalidateRect (NULL, FALSE);
			}

			wc = wi->GetDataNext ();
		}
		return (CWindowControl*)tempWindow;
	}

	if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){

		CWindowTextExt* tempText = new CWindowTextExt(pwc);
		return (CWindowControl*)tempText;
	}

	if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){

		CWindowButtonExt* tempButton = new CWindowButtonExt(pwc);

		tempButton->btnAttributes.capAlignment = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.capAlignment;
		tempButton->btnAttributes.horIconAlignment = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.horIconAlignment;
		tempButton->btnAttributes.vertIconAlignment = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.vertIconAlignment;
		tempButton->btnAttributes.HorizontalOriented = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.HorizontalOriented;
		tempButton->m_Icon = CWindowButtonExt::TranslateCWindowControl(pwc)->m_Icon;
		tempButton->btnAttributes.m_myShape = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.m_myShape;
		tempButton->btnAttributes.m_myPattern = CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.m_myPattern;

		memcpy (&(tempButton->btnAttributes.m_myActionUnion), &CWindowButtonExt::TranslateCWindowControl(pwc)->btnAttributes.m_myActionUnion, sizeof (tempButton->btnAttributes.m_myActionUnion));
		tempButton->controlAttributes.useDefault = pwc->controlAttributes.useDefault;

		return (CWindowControl*)tempButton;
	}
	if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
		CWindowLabelExt *tempLabel = new CWindowLabelExt(pwc);

		tempLabel->labelAttributes.capAlignment = CWindowLabelExt::TranslateCWindowControl(pwc)->labelAttributes.capAlignment;
		tempLabel->labelAttributes.horIconAlignment = CWindowLabelExt::TranslateCWindowControl(pwc)->labelAttributes.horIconAlignment;
		tempLabel->labelAttributes.vertIconAlignment = CWindowLabelExt::TranslateCWindowControl(pwc)->labelAttributes.vertIconAlignment;
		tempLabel->labelAttributes.HorizontalOriented = CWindowLabelExt::TranslateCWindowControl(pwc)->labelAttributes.HorizontalOriented;
		tempLabel->m_IconName = CWindowLabelExt::TranslateCWindowControl(pwc)->m_IconName;

		return (CWindowControl*)tempLabel;
	}

	else
		return NULL;
}


CWindowItemExt * CNewLayoutView::CreateNewWindow(CWindowControl *wc, int row, int column)
{
	CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl (wc);
	CWindowItemExt* tempWindow = new CWindowItemExt(wc);

	tempWindow->controlAttributes.m_myId = uiGlobalID++;
	tempWindow->controlAttributes.m_nRow = row;
	tempWindow->controlAttributes.m_nColumn = column;
	tempWindow->windowAttributes.GroupNum = wi->windowAttributes.GroupNum;
	tempWindow->windowAttributes.busRules.NumOfPicks = wi->windowAttributes.busRules.NumOfPicks;
	wi->WindowDef.AssignTo(tempWindow->WindowDef);

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//does our window extend beyond the boundaries of the document
	if(!pDoc->ValidateWindowDoc(tempWindow)){
		//alert user and exit function - window is not created
		AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);

		delete tempWindow;
		return NULL;
	}

	int moveRow = tempWindow->controlAttributes.m_nRow - wc->controlAttributes.m_nRow;
	int moveColumn = tempWindow->controlAttributes.m_nColumn - wc->controlAttributes.m_nColumn;

	CWindowControl *pwc = wi->GetDataFirst ();
	while(pwc != NULL){
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			CWindowItemExt *newSub = CreateNewWindow(pwc,pwc->controlAttributes.m_nRow + moveRow,pwc->controlAttributes.m_nColumn + moveColumn);
			if(newSub != NULL){
				newSub->windowAttributes.Show = FALSE;
				newSub->pContainer = tempWindow;
				tempWindow->AddDataLast(newSub);
			}
		}
		if(pwc->controlAttributes.m_nType == CWindowControl::TextControl){

			CWindowTextExt *newText = CreateNewText(pwc,tempWindow,row,column,1);
			if(newText != NULL){
				newText->controlAttributes.m_nRow = pwc->controlAttributes.m_nRow;
				newText->controlAttributes.m_nColumn = pwc->controlAttributes.m_nColumn;

				newText->pContainer = tempWindow;
				tempWindow->AddDataLast (newText);
			}

		}
		if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){
			CWindowButtonExt *newBtn = CreateNewButton(pwc,tempWindow,row,column,1);
			if(newBtn !=NULL){
				newBtn->controlAttributes.m_nRow = pwc->controlAttributes.m_nRow ;
				newBtn->controlAttributes.m_nColumn = pwc->controlAttributes.m_nColumn ;
				newBtn->pContainer = tempWindow;
				tempWindow->AddDataLast (newBtn);
			}

		}
		if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
			CWindowLabelExt *newLbl = CreateNewLabel(pwc,tempWindow,row,column,1);
			if(newLbl !=NULL){
				newLbl->controlAttributes.m_nRow = pwc->controlAttributes.m_nRow ;
				newLbl->controlAttributes.m_nColumn = pwc->controlAttributes.m_nColumn ;
				newLbl->pContainer = tempWindow;
				tempWindow->AddDataLast (newLbl);
			}

		}

		pwc = wi->GetDataNext ();
	}
	return tempWindow;

}

CWindowButtonExt* CNewLayoutView::CreateNewButton(CWindowControl *wc,CWindowItemExt *pWin, int row, int column,int cnt)
{

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowButtonExt *oldpwc = CWindowButtonExt::TranslateCWindowControl(wc);
	CWindowButtonExt *pwc = new CWindowButtonExt(wc);

	pwc->controlAttributes.m_myId = uiGlobalID++;
	pwc->btnAttributes.m_myShape = CWindowButtonExt::TranslateCWindowControl(oldpwc)->btnAttributes.m_myShape;
	pwc->btnAttributes.m_myPattern = CWindowButtonExt::TranslateCWindowControl(oldpwc)->btnAttributes.m_myPattern;
	pwc->controlAttributes.m_myAction = CWindowButtonExt::TranslateCWindowControl(oldpwc)->controlAttributes.m_myAction;
	pwc->m_Icon = CWindowButtonExt::TranslateCWindowControl(oldpwc)->m_Icon;
	pwc->btnAttributes.capAlignment = oldpwc->btnAttributes.capAlignment;
	pwc->btnAttributes.horIconAlignment = oldpwc->btnAttributes.horIconAlignment;
	pwc->btnAttributes.vertIconAlignment = oldpwc->btnAttributes.vertIconAlignment;
	pwc->btnAttributes.HorizontalOriented = oldpwc->btnAttributes.HorizontalOriented;
	memset (&pwc->btnAttributes.m_myActionUnion, 0, sizeof (pwc->btnAttributes.m_myActionUnion));
	memcpy (&pwc->btnAttributes.m_myActionUnion, &oldpwc->btnAttributes.m_myActionUnion, sizeof (pwc->btnAttributes.m_myActionUnion));


	if(cnt == 1){
		pwc->controlAttributes.m_nRow = row - pWin->controlAttributes.m_nRow;
		pwc->controlAttributes.m_nColumn = column - pWin->controlAttributes.m_nColumn;

		shiftCol = pwc->controlAttributes.m_nColumn - oldpwc->controlAttributes.m_nColumn;
		shiftRow = pwc->controlAttributes.m_nRow - oldpwc->controlAttributes.m_nRow;
	}
	else{
		pwc->controlAttributes.m_nRow += shiftRow;
		pwc->controlAttributes.m_nColumn += shiftCol;
	}
	CRect thisControl(pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn,
		pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow,
		pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier,
		pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);

	BOOL grid = pDoc->IsValidGrid(row,column,uiActiveID,thisControl,NULL);
	BOOL window =pDoc->ValidateWindow(thisControl,pWin);
	if(grid && window){
		return pwc;
	}
	else {
		if(!grid)
			AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
		if(!window)
			AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

		delete pwc;
		return NULL;
	}

}

CWindowLabelExt* CNewLayoutView::CreateNewLabel(CWindowControl *wc,CWindowItemExt *pWin, int row, int column,int cnt)
{

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowLabelExt *oldpwc = CWindowLabelExt::TranslateCWindowControl(wc);
	CWindowLabelExt *pwc = new CWindowLabelExt(wc);

	pwc->controlAttributes.m_myId = uiGlobalID++;
	pwc->m_IconName = CWindowLabelExt::TranslateCWindowControl(oldpwc)->m_IconName;
	pwc->labelAttributes.capAlignment = oldpwc->labelAttributes.capAlignment;
	pwc->labelAttributes.horIconAlignment = oldpwc->labelAttributes.horIconAlignment;
	pwc->labelAttributes.vertIconAlignment = oldpwc->labelAttributes.vertIconAlignment;
	pwc->labelAttributes.HorizontalOriented = oldpwc->labelAttributes.HorizontalOriented;

	if(cnt == 1){
		pwc->controlAttributes.m_nRow = row - pWin->controlAttributes.m_nRow;
		pwc->controlAttributes.m_nColumn = column - pWin->controlAttributes.m_nColumn;

		shiftCol = pwc->controlAttributes.m_nColumn - oldpwc->controlAttributes.m_nColumn;
		shiftRow = pwc->controlAttributes.m_nRow - oldpwc->controlAttributes.m_nRow;
	}
	else{
		pwc->controlAttributes.m_nRow += shiftRow;
		pwc->controlAttributes.m_nColumn += shiftCol;
	}
	CRect thisControl(pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn,
		pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow,
		pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier,
		pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);

	BOOL grid = pDoc->IsValidGrid(row,column,uiActiveID,thisControl,NULL);
	BOOL window =pDoc->ValidateWindow(thisControl,pWin);
	if(grid && window){
		return pwc;
	}
	else {
		if(!grid)
			AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
		if(!window)
			AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

		delete pwc;
		return NULL;
	}

}


CWindowTextExt* CNewLayoutView::CreateNewText(CWindowControl *wc,CWindowItemExt *pWin, int row, int column, int cnt)
{

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowTextExt *oldpwc = CWindowTextExt::TranslateCWindowControl(wc);
	CWindowTextExt *pwc = new CWindowTextExt(wc);

	BOOL cont = pDoc->ValidateText(pwc->textAttributes.type);

	if(cont){
		if(cnt == 1){
			pwc->controlAttributes.m_nRow = row - pWin->controlAttributes.m_nRow;
			pwc->controlAttributes.m_nColumn = column - pWin->controlAttributes.m_nColumn;

			shiftRow = row - pWin->controlAttributes.m_nRow;
			shiftCol = column - pWin->controlAttributes.m_nColumn;
		}
		else{
			pwc->controlAttributes.m_nRow =  shiftRow;
			pwc->controlAttributes.m_nColumn = shiftCol;
		}

		CRect thisControl(pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn,
			pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow,
			pWin->controlAttributes.m_nColumn + pwc->controlAttributes.m_nColumn + pwc->controlAttributes.m_usWidthMultiplier,
			pWin->controlAttributes.m_nRow + pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_usHeightMultiplier);


		BOOL grid = pDoc->IsValidGrid(row,column,uiActiveID,thisControl,NULL);
		BOOL window =pDoc->ValidateWindow(thisControl,pWin);

		if(grid && window){
			InvalidateRect (NULL, FALSE);
			return pwc;
		}
		else {
			if(!grid)
				AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
			if(!window)
				AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

			delete pwc;
			return NULL;
		}
	}
	else {
		AfxMessageBox(IDS_TEXT_ERR, MB_OK, 0);
		delete pwc;
		return NULL;
	}
}




UINT CNewLayoutView::GetSubID(CWindowItemExt *wi)
{
	UINT id = 0;
	UINT subID = 0;
	CWindowControl *wc = wi->GetDataFirst();
	while (wc) 
	{
		if (wc->controlAttributes.m_nType == CWindowControl::WindowContainer)
		{
			id = wc->controlAttributes.m_myId;
			subID = GetSubID(CWindowItemExt::TranslateCWindowControl(wc));
			if (subID > id)
				id = subID;
		}
		else if (wc->controlAttributes.m_myId > id)
			id = wc->controlAttributes.m_myId;
		wc = wi->GetDataNext();
	}
	return id;

}

BOOL CNewLayoutView::RemoveSubWin(CWindowItemExt *wi)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(wi->pContainer == NULL)
		return TRUE;

	CWindowItemExt *pParent = CWindowItemExt::TranslateCWindowControl(wi->pContainer);

	POSITION currentPos = pParent->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = pParent->ButtonItemList.GetNext (currentPos);
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer &&
			pwc->controlAttributes.m_myId == wi->controlAttributes.m_myId){
				delete pwc;
				pParent->ButtonItemList.RemoveAt (currentPosLast);
				pDoc->CheckPoint();
				return TRUE;
		}
		currentPosLast = currentPos;
	}
	return FALSE;

}



void CNewLayoutView::PopSetOEPWin(int row, int column)
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem(row, column, uiActiveID);

	if(nSearch ==4){
		CWindowControl *pWndCntrl = pDoc->GetDataThis();
		CWindowItemExt *pWndItem = CWindowItemExt::TranslateCWindowControl(pWndCntrl);
		if(pWndItem->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl && CWindowTextExt::TranslateCWindowControl(pWndItem->GetDataThis())->textAttributes.type == CWindowText::TypeOEP){
			CWindowTextExt *pb = 0;
			CWindowControl *wc = pWndItem->GetDataThis ();

			pb = CWindowTextExt::TranslateCWindowControl (wc);

			CD_OEPEdit tt(pb, pWndItem);
			BOOL cont;
			//tt.pDoc = pDoc;
			tt.defFlag = pb->controlAttributes.useDefault;

			if(tt.DoModal() == IDOK){
				if(pb->textAttributes.type != (CWindowTextExt::CWindowTextType)tt.myID){
					cont = pDoc->ValidateText((CWindowTextExt::CWindowTextType)tt.myID);
					if(!cont){
						AfxMessageBox(IDS_TEXT_ERR, MB_OK, 0);
						return;
					}
				}

				// update the OEP window name so that it is the same as the OEP group.
				CString tempName = "";
				tempName.Format(_T("OEP_%-20.20s"), tt.m_OEPGroupNumbers);
				pWndItem->myName = tempName;

				CRect thisControl(pb->controlAttributes.m_nColumn,
					pb->controlAttributes.m_nRow,
					pb->controlAttributes.m_nColumn + pWndItem->controlAttributes.m_nColumn + tt.m_usWidthMultiplier,
					pb->controlAttributes.m_nRow + pWndItem->controlAttributes.m_nRow + tt.m_usHeightMultiplier);

				USHORT oldWidth = pWndItem->controlAttributes.m_usWidthMultiplier;
				USHORT oldHeight = pWndItem->controlAttributes.m_usHeightMultiplier;

				pWndItem->controlAttributes.m_usWidthMultiplier = tt.m_usWidthMultiplier;
				pWndItem->controlAttributes.m_usHeightMultiplier = tt.m_usHeightMultiplier;
				//turning on the bit flag for an OEP display resting on this window
				//check file WindowControl.h for information on how this array is used
				//to mark if a window has a special display on it or not.
				for (int i = 0; i < sizeof(tt.m_csSpecWin)/sizeof(tt.m_csSpecWin[0]); i++) {
					pWndItem->controlAttributes.SpecWin[i] = tt.m_csSpecWin[i];
				}

				CWindowControl *pw = pDoc->GetDataFirst();
				pw = pDoc->GetSelectedDocWindow(uiActiveID);

				CLeftView *myTree = (CLeftView*)m_TreeView;
				ASSERT(myTree);
				myTree->UpdateTreeLabel((CWindowItemExt*)pw);

				//does our window extend beyond the boundaries of the document
				if(!pDoc->ValidateWindowDoc(pWndItem)){
					//alert user and exit function - window is not created
					AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
					pWndItem->controlAttributes.m_usWidthMultiplier = oldWidth;
					pWndItem->controlAttributes.m_usHeightMultiplier = oldHeight;
					return;
				}

				BOOL grid = pDoc->IsValidGrid(pb->controlAttributes.m_nRow,
					pb->controlAttributes.m_nColumn,
					uiActiveID,
					thisControl,
					pb);

				pWndItem->controlAttributes.m_colorFace = tt.colorFace;

				if(grid){
					//tt.saveTo(pb);
					pb->textAttributes.type = CWindowText::TypeOEP;
					pb->controlAttributes.m_myId;// = tt.myID;
					pb->controlAttributes.m_nRow = tt.mRow;
					pb->controlAttributes.m_nColumn = tt.mColumn;
					pb->controlAttributes.m_usHeightMultiplier = tt.m_usHeightMultiplier;
					pb->controlAttributes.m_usWidthMultiplier = tt.m_usWidthMultiplier;
					pb->controlAttributes.m_nHeight = tt.m_usHeightMultiplier * CWindowButton::stdHeight;
					pb->controlAttributes.m_nWidth = tt.m_usWidthMultiplier * CWindowButton::stdWidth;
					pb->controlAttributes.m_colorText = tt.colorText;
					pb->controlAttributes.m_colorFace = tt.colorFace;
					pb->controlAttributes.useDefault = tt.defFlag;
					pb->controlAttributes.lfControlFont = tt.myTextFont;
					//turning on the bit flag for an OEP display resting on this window
					//check file WindowControl.h for information on how this array is used
					//to mark if a window has a special display on it or not.
					pWndItem->controlAttributes.SpecWin[SpecWinIndex_OEP] |= SpecWinMask_OEP_FLAG;
					//OEP Specific
					pWndItem->windowAttributes.oepBtnWidth = pb->textAttributes.oepBtnWidth = tt.m_usBtnWidth;
					pWndItem->windowAttributes.oepBtnHeight = pb->textAttributes.oepBtnHeight = tt.m_usBtnHeight;
					pWndItem->windowAttributes.oepBtnColor = pb->textAttributes.oepBtnColor = tt.btnColorFace;
					pWndItem->windowAttributes.oepBtnFontColor = pb->textAttributes.oepBtnFontColor = tt.btnFontColor;
					pWndItem->windowAttributes.oepBtnShape = pb->textAttributes.oepBtnShape = tt.myShape;
					pWndItem->windowAttributes.oepBtnPattern = pb->textAttributes.oepBtnPattern = tt.myPattern;
					pWndItem->windowAttributes.oepBtnDown = pb->textAttributes.oepBtnDown = tt.m_BtnDown;
					pWndItem->windowAttributes.oepBtnAcross = pb->textAttributes.oepBtnAcross = tt.m_BtnAcross;
					pWndItem->windowAttributes.oepBtnFont = pb->textAttributes.oepBtnFont = tt.myBtnFont;
					//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
					_tcsncpy_s(pb->textAttributes.mOEPGroupNumbers, tt.m_OEPGroupNumbers, 30);
					_tcsncpy_s(pb->controlAttributes.mOEPGroupNumbers, tt.m_OEPGroupNumbers, 30);
					_tcsncpy_s(pWndItem->controlAttributes.mOEPGroupNumbers, tt.m_OEPGroupNumbers, 30);
					//added Icon support to OEP window
					pb->textAttributes.oepBtnIconHorizAlign = (CWindowButton::HorizontalIconAlignment) tt.m_nOEPIconHorizRadioSelection;
					pb->textAttributes.oepBtnIconVertAlign	= (CWindowButton::VerticalIconAlignment) tt.m_nOEPIconVertRadioSelection;
					_tcscpy_s(pb->textAttributes.oepIconFileName, tt.m_csOEPIconFileName);
					pb->csOEPIconFileName = tt.m_csOEPIconFileName;
					pb->csOEPIconFilePath = tt.m_csOEPIconFilePath;

					pb->myName = pWndItem->myName;

					pDoc->CheckPoint();
					InvalidateRect(NULL,FALSE);
				}
				else {
					if(!grid)
						AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );

				}
			}

		}
		else{
			nSearch = 3;

		}
	}

	// Create a window and OEP area
	if (nSearch == 0 || nSearch == 3 || nSearch == 6)
	{
		BOOL cont = pDoc->ValidateText(CWindowText::TypeOEP);

		/*Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		if(!cont){
		AfxMessageBox(IDS_TEXT_ERR, MB_OK, 0);
		return;
		}
		*/
		//create window to hold new OEP text object
		CWindowItemExt *pWndItem = new CWindowItemExt (0, row, column);
		//OEP creation dialog
		CD_OEPEdit  thisOEP (NULL, pWndItem);

		thisOEP.mRow = row;
		thisOEP.mColumn = column;
		thisOEP.defFlag = TRUE;

		int status = thisOEP.DoModal();
		if(status == IDOK) 
		{
			/////////////////////////////////////////////////////////////////////////
			//finish creating window to contain OEP object

			//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
			//	- Label OEP Tree Entry with OEP ID
			CString tempName = "";
			tempName.Format(_T("OEP_%-20.20s"), thisOEP.m_OEPGroupNumbers);

			pWndItem->myName = tempName;
			pWndItem->controlAttributes.m_usWidthMultiplier = thisOEP.m_usWidthMultiplier;
			pWndItem->controlAttributes.m_usHeightMultiplier = thisOEP.m_usHeightMultiplier;
			pWndItem->controlAttributes.m_nHeight = thisOEP.m_usHeightMultiplier * CWindowButton::stdHeight;
			pWndItem->controlAttributes.m_nWidth = thisOEP.m_usWidthMultiplier * CWindowButton::stdWidth;
			pWndItem->controlAttributes.isVirtualWindow = TRUE;
			pWndItem->controlAttributes.m_nColumn = column;
			pWndItem->controlAttributes.m_nRow = row;
			pWndItem->windowAttributes.GroupNum = 0;
			pWndItem->windowAttributes.Show = TRUE;
			pWndItem->controlAttributes.CurrentID = uiActiveID;
			pWndItem->controlAttributes.m_colorFace = thisOEP.colorFace;
			//turning on the bit flag for an OEP display resting on this window
			//check file WindowControl.h for information on how this array is used
			//to mark if a window has a special display on it or not.
			for (int i = 0; i < sizeof(thisOEP.m_csSpecWin)/sizeof(thisOEP.m_csSpecWin[0]); i++) 
			{
				pWndItem->controlAttributes.SpecWin[i] = thisOEP.m_csSpecWin[i];
			}

			//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
			_tcsncpy_s(pWndItem->controlAttributes.mOEPGroupNumbers, thisOEP.m_OEPGroupNumbers, 30);

			//does our window extend beyond the boundaries of the document
			if (!pDoc->ValidateWindowDoc(pWndItem))
			{
				//alert user and exit function - window is not created
				AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);

				delete pWndItem;
				return;
			}

			/////////////////////////////////////////////////////////////////////////

			CWindowTextExt *pWndText = new CWindowTextExt(thisOEP.myID,
				row - pWndItem->controlAttributes.m_nRow,
				column - pWndItem->controlAttributes.m_nColumn);

			//			thisOEP.saveTo(pWndText);

			pWndText->myName = pWndItem->myName;
			pWndText->textAttributes.type = CWindowText::TypeOEP;
			pWndText->controlAttributes.m_usHeightMultiplier = thisOEP.m_usHeightMultiplier;
			pWndText->controlAttributes.m_usWidthMultiplier = thisOEP.m_usWidthMultiplier;
			pWndText->controlAttributes.m_nHeight = thisOEP.m_usHeightMultiplier * CWindowButton::stdHeight;
			pWndText->controlAttributes.m_nWidth = thisOEP.m_usWidthMultiplier * CWindowButton::stdWidth;
			pWndText->controlAttributes.m_colorFace = thisOEP.colorFace;
			pWndText->controlAttributes.m_colorText = thisOEP.colorText;
			pWndText->pContainer = pWndItem;
			pWndText->controlAttributes.useDefault = thisOEP.defFlag;
			pWndText->controlAttributes.lfControlFont = thisOEP.myTextFont;
			//OEP Specific
			pWndItem->windowAttributes.oepBtnWidth = pWndText->textAttributes.oepBtnWidth = thisOEP.m_usBtnWidth;
			pWndItem->windowAttributes.oepBtnHeight = pWndText->textAttributes.oepBtnHeight = thisOEP.m_usBtnHeight;
			pWndItem->windowAttributes.oepBtnColor = pWndText->textAttributes.oepBtnColor = thisOEP.btnColorFace;
			pWndItem->windowAttributes.oepBtnFontColor = pWndText->textAttributes.oepBtnFontColor = thisOEP.btnFontColor;
			pWndItem->windowAttributes.oepBtnShape = pWndText->textAttributes.oepBtnShape = thisOEP.myShape;
			pWndItem->windowAttributes.oepBtnPattern = pWndText->textAttributes.oepBtnPattern = thisOEP.myPattern;
			pWndItem->windowAttributes.oepBtnDown = pWndText->textAttributes.oepBtnDown = thisOEP.m_BtnDown;
			pWndItem->windowAttributes.oepBtnAcross = pWndText->textAttributes.oepBtnAcross = thisOEP.m_BtnAcross;
			pWndItem->windowAttributes.oepBtnFont = pWndText->textAttributes.oepBtnFont = thisOEP.myBtnFont;

			//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
			_tcsncpy_s(pWndText->textAttributes.mOEPGroupNumbers, thisOEP.m_OEPGroupNumbers, 30);
			_tcsncpy_s(pWndText->controlAttributes.mOEPGroupNumbers, thisOEP.m_OEPGroupNumbers, 30);

			//added Icon support to OEP window
			pWndText->textAttributes.oepBtnIconHorizAlign	= (CWindowButton::HorizontalIconAlignment) thisOEP.m_nOEPIconHorizRadioSelection;
			pWndText->textAttributes.oepBtnIconVertAlign	= (CWindowButton::VerticalIconAlignment) thisOEP.m_nOEPIconVertRadioSelection;
			_tcscpy_s(pWndText->textAttributes.oepIconFileName, thisOEP.m_csOEPIconFileName);
			pWndText->csOEPIconFileName = thisOEP.m_csOEPIconFileName;
			pWndText->csOEPIconFilePath = thisOEP.m_csOEPIconFilePath;

			CRect thisControl(column, row,
				column + thisOEP.m_usWidthMultiplier,
				row + thisOEP.m_usHeightMultiplier);

			// Check to see that the text area will fit
			BOOL grid = pDoc->IsValidGrid(row,column, pWndItem->controlAttributes.m_myId,thisControl, NULL);
			BOOL window = pDoc->ValidateWindow(thisControl, pWndItem);
			if(grid && window){

				pWndItem->controlAttributes.m_myId = uiGlobalID++;
				uiActiveID = pWndItem->controlAttributes.m_myId;
				//add window to the documents control list
				pDoc->AddDataLast (pWndItem);
				//get the view contianing our tree control update it with the new window
				CLeftView *myTree = (CLeftView*)m_TreeView;
				ASSERT(myTree);

				myTree->InsertNewWinItem(0, pWndItem);

				pWndText->controlAttributes.m_myId = uiGlobalID++;
				pWndItem->AddDataLast (pWndText);
				pDoc->CheckPoint();

				//Additions made for Multiple-OEP Window Feature - CSMALL 1/12/06
				myTree->ShowSelectedWindow(pWndText->controlAttributes.m_myId);

				InvalidateRect (NULL, FALSE);
			}
			else 
			{
				if(!grid)
					AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
				if(!window)
					AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);

				delete pWndItem;
				delete pWndText;
			}
		}
	}
}

void CNewLayoutView::ClearDocumentSelection()
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowControl *bi = 0;
	POSITION pos = pDoc->listControls.GetHeadPosition ();

	while (pos){
		bi = pDoc->listControls.GetNext (pos);

		if(bi->controlAttributes.m_nType == CWindowControl::WindowContainer){
			CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);
			pw->controlAttributes.Selected = FALSE;

			CWindowControl *pwc = pw->GetDataFirst ();

			while (pwc)
			{
				if(pwc->controlAttributes.m_nType == CWindowControl::TextControl)
				{
					(CWindowTextExt::TranslateCWindowControl(pwc))->controlAttributes.Selected = FALSE;
				}
				if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl)
				{
					(CWindowButtonExt::TranslateCWindowControl(pwc))->controlAttributes.Selected = FALSE;
				}
				// if a label is selected, the previous selections need to be unselected - CSMALL 10/12/05
				if(pwc->controlAttributes.m_nType == CWindowControl::LabelControl){
					(CWindowLabelExt::TranslateCWindowControl(pwc))->controlAttributes.Selected = FALSE;
				}
				if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
					(CWindowItemExt::TranslateCWindowControl(pwc))->controlAttributes.Selected = FALSE;
					ClearSelection((CWindowControl*)pwc);
				}
				pwc = pw->GetDataNext ();
			}
		}
		else{
			if(bi->controlAttributes.m_nType == CWindowControl::TextControl){
				(CWindowTextExt::TranslateCWindowControl(bi))->controlAttributes.Selected = FALSE;
			}
			if(bi->controlAttributes.m_nType == CWindowControl::ButtonControl){
				(CWindowButtonExt::TranslateCWindowControl(bi))->controlAttributes.Selected = FALSE;
			}
		}
	}


}

void CNewLayoutView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint pt = GetDeviceScrollPosition( );
	point.x += pt.x;
	point.y += pt.y;
	cpLastLButtonUp = point;

	int newColumn = point.x / CWindowButtonExt::stdWidth;
	int newRow = point.y / CWindowButtonExt::stdHeight;

	int oldColumn = cpLastLButton.x / CWindowButtonExt::stdWidth;
	int oldRow = cpLastLButton.y / CWindowButtonExt::stdHeight;

	int diffColumn = newColumn - oldColumn;
	int diffRow = newRow - oldRow;


	CScrollView ::OnLButtonUp(nFlags, point);
	if(moveDragControl && dragControl != NULL){

		if(dragControl->controlAttributes.m_nType == CWindowControl::ButtonControl || dragControl->controlAttributes.m_nType == 1 || dragControl->controlAttributes.m_nType == 4){
			CWindowItemExt *dragParent = CWindowItemExt::TranslateCWindowControl(dragControl->pContainer);

			//create a CRect to contain the screen area of this button
			CRect thisControl(dragControl->controlAttributes.m_nColumn  + dragParent->controlAttributes.m_nColumn + diffColumn,
				dragControl->controlAttributes.m_nRow +  dragParent->controlAttributes.m_nRow + diffRow,
				dragControl->controlAttributes.m_nColumn + dragParent->controlAttributes.m_nColumn + diffColumn + dragControl->controlAttributes.m_usWidthMultiplier,
				dragControl->controlAttributes.m_nRow +  dragParent->controlAttributes.m_nRow + diffRow + dragControl->controlAttributes.m_usHeightMultiplier);

			/*call function to verify that the edited button will not overlap
			any other buttons or text and assign result to flag*/
			BOOL grid = pDoc->IsValidGrid(dragControl->controlAttributes.m_nRow + dragParent->controlAttributes.m_nRow + diffRow,
				dragControl->controlAttributes.m_nColumn + dragParent->controlAttributes.m_nColumn + diffColumn,
				dragParent->controlAttributes.m_myId,
				thisControl,
				dragControl);
			/*call function to verify that the edited button will not extend
			beyond the borders of the window and assign result to flag*/
			BOOL window =pDoc->ValidateWindow(thisControl,dragParent);

			if (grid && window)
			{
				dragControl->controlAttributes.m_nColumn += diffColumn;
				dragControl->controlAttributes.m_nRow += diffRow;
				//pDoc->CheckPoint();
			}
			//did not pass checks
			else 
			{
				//alert user according to which checks failed
				if(!grid)
					AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
				if(!window)
					AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
			}
		}
		if (dragControl->controlAttributes.m_nType == CWindowControl::WindowContainer)
		{
			if(dragControl->pContainer)
			{
				CWindowItemExt* dragParent = CWindowItemExt::TranslateCWindowControl(dragControl->pContainer);
				//create a CRect to contian the screen area of this button
				CRect thisControl(dragControl->controlAttributes.m_nColumn  + diffColumn,
					dragControl->controlAttributes.m_nRow + diffRow,
					dragControl->controlAttributes.m_nColumn + diffColumn + dragControl->controlAttributes.m_usWidthMultiplier,
					dragControl->controlAttributes.m_nRow +  diffRow + dragControl->controlAttributes.m_usHeightMultiplier);

				//validate that the new window does not extend beyond its parents boundaries
				BOOL window = pDoc->ValidateWindow(thisControl,dragParent);
				if(!window)	{
					//alert user and exit funtion - do not create window
					AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
					return;
				}
			}

			dragControl->controlAttributes.m_nColumn += diffColumn;
			dragControl->controlAttributes.m_nRow += diffRow;
			//does our window extend beyond the boundaries of the document
			if(!pDoc->ValidateWindowDoc(dragControl)){
				//alert user and exit function - window is not created
				AfxMessageBox(IDS_WIN_INVALID,MB_OK,0);
				dragControl->controlAttributes.m_nColumn -= diffColumn;
				dragControl->controlAttributes.m_nRow -= diffRow;
				return;
			}
			OnDragWindow(diffColumn,diffRow,CWindowItemExt::TranslateCWindowControl(dragControl));
			//pDoc->CheckPoint();
		}
		InvalidateRect(NULL, TRUE);
		dragControl = NULL;
		moveDragControl = FALSE;

		return;

	}
	InvalidateRect(NULL, TRUE);
}

void CNewLayoutView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt = GetDeviceScrollPosition( );
	int nColumn = (point.x + pt.x )/ CWindowButtonExt::stdWidth;
	int nRow    = (point.y + pt.y )/ CWindowButtonExt::stdHeight;
	nColumn += 1;
	nRow += 1;
	CString csLabel;
	csLabel.Format(_T("%d, %d"),nColumn,nRow);
	CMainFrame *parent = (CMainFrame *)GetParentOwner();
	if (!parent)
		return;
	_setPaneText(0, csLabel);
	UpdateRulersInfo(RW_POSITION, GetScrollPosition(), point);
}

void CNewLayoutView::OnDragWindow(int diffColumn, int diffRow, CWindowItemExt *wi)
{
	//cycle through this windows controls and make calls to display them
	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		//get the next control
		CWindowControl *pwc = wi->ButtonItemList.GetNext (currentPos);

		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			pwc->controlAttributes.m_nColumn += diffColumn;
			pwc->controlAttributes.m_nRow += diffRow;
			OnDragWindow(diffColumn,diffRow,CWindowItemExt::TranslateCWindowControl(pwc));
		}

		currentPosLast = currentPos;
	}

}

void CNewLayoutView::OnEditRedo()
{
	//save the active ID so the same window will be active after the redo
	UINT nActiveID = uiActiveID;
	//get the frame windows position and size so we keep the same size aftet  the redo
	CChildFrame *frmWnd = (CChildFrame*)GetParentFrame();
	frmWnd->GetWindowPlacement(&frmWnd->wp);

	//call redo funtion
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Redo();

	//update all views to refelct change
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->OnInitialUpdate();

	//update the window position and active window to those that were saved
	myTree->ShowSelectedWindow(nActiveID);
	frmWnd->SetWindowPlacement(&frmWnd->wp);
	uiActiveID = nActiveID;
}

void CNewLayoutView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->CanRedo());
}

void CNewLayoutView::OnEditUndo()
{
	//save the active ID so the same window will be active after the undo
	UINT nActiveID = uiActiveID;
	//get the frame windows position and size so we keep the same size aftet  the undo
	CChildFrame *frmWnd = (CChildFrame*)GetParentFrame();
	frmWnd->GetWindowPlacement(&frmWnd->wp);

	//call undo funtion
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Undo();

	//update all views to refelct change
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->OnInitialUpdate();

	//update the window position and active window to those that were saved
	myTree->ShowSelectedWindow(nActiveID);
	frmWnd->SetWindowPlacement(&frmWnd->wp);
	uiActiveID = nActiveID;
}

void CNewLayoutView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->CanUndo());
}

void CNewLayoutView::UpdateRulersInfo(int nMessage, CPoint ScrollPos, CPoint Pos)
{
	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	if (!m_pParent)
		return;
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*if (m_fZoomFactor < ZOOM_NORMAL) {
	m_bShowRulers = FALSE;
	m_pParent->ShowRulers(FALSE);
	}*/
	CRect myRect;//(0,0,800,600);
	GetClientRect(&myRect);
	stRULER_INFO pRulerInfo;
	pRulerInfo.uMessage    = nMessage;
	pRulerInfo.ScrollPos   = ScrollPos;
	pRulerInfo.Pos         = Pos;
	pRulerInfo.DocSize     = myRect.Size();
	pRulerInfo.fZoomFactor = 1;

	m_pParent->UpdateRulersInfo(pRulerInfo);
}

void CNewLayoutView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateRulersInfo(RW_HSCROLL, GetScrollPosition(), NULL);
	Invalidate(FALSE);
	CScrollView ::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CNewLayoutView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateRulersInfo(RW_VSCROLL, GetScrollPosition(), NULL);
	Invalidate(FALSE);
	CScrollView ::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CNewLayoutView::OnAddLabel()
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get the point that was clicked
	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	//convert the coordinates to our grid
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	UINT saveID = uiActiveID;

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);
	//a window was clicked in
	if (nSearch > 1) {
		//get the window that was clicked in and cast it to a CWindowItemExt
		CWindowControl *pc = pDoc->GetDataThis();
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

		//user clicked in an inactive window (sub window)
		if(nSearch == 6){
			//call function to get the window they clicked in
			pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(pw == NULL){
				return;
			}
			//call function to see if an exixting item was clicked in and assign to nSearch
			nSearch = pw->SearchForControl(row,column);
		}

		//this window does exist
		if (pw) {
			//is there an existing control here?
			if (nSearch == 4) {
				//is this control a label
				if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::LabelControl){
					//get the label that was clicked in and cast it to CWindowLabelExt
					CWindowControl *wc = pw->GetDataThis ();
					CWindowLabelExt *pl = CWindowLabelExt::TranslateCWindowControl (wc);
					//initialize an instance of the dialog class for editing label with this label
					CDWinLabel *winLbl = new CDWinLabel(pl);

					//did the user click ok on the dialog
					if (winLbl->DoModal () == IDOK) {
						//create a CRect to contian the screen area of this label
						CRect thisControl(pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
							pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
							pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn + winLbl->m_usWidthMultiplier,
							pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow + winLbl->m_usHeightMultiplier);

						/*call function to verify that the edited label will not overlap
						any other labels, buttons, or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
							pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
							uiActiveID,thisControl,
							wc);
						/*call function to verify that the edited label will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window =pDoc->ValidateWindow(thisControl,pw);
						//did both checks pass
						if(grid && window){
							//assign edited attributes from dialog to the label

							//if size of caption is not larger than max size of myCaption, then insert it into the structure
							//else, truncate the characters past max size and then insert
							if (sizeof(winLbl->m_csCaption) <= MaxSizeOfMyCaption){
								pl->myCaption = winLbl->m_csCaption;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, winLbl->m_csCaption, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pl->myCaption = tempMyCaption;
							}
							_tcsncpy_s(pl->controlAttributes.m_myCaption, pl->myCaption, MaxSizeOfMyCaption);
							pl->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

							_tcsncpy_s(pl->labelAttributes.tcsLabelText, pl->myCaption, sizeof(pl->labelAttributes.tcsLabelText)/sizeof(pl->labelAttributes.tcsLabelText[0])-1);
							pl->labelAttributes.tcsLabelText[sizeof(pl->labelAttributes.tcsLabelText)/sizeof(pl->labelAttributes.tcsLabelText[0])-1] = 0;

							if (sizeof(winLbl->m_myName) <= MaxSizeOfMyCaption){
								pl->myName = winLbl->m_myName;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, winLbl->m_myName, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pl->myName = tempMyCaption;
							}
							_tcsncpy_s(pl->controlAttributes.m_myName, pl->myName, MaxSizeOfMyCaption);
							pl->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

							pl->controlAttributes.m_colorText = winLbl->colorText;
							pl->controlAttributes.m_colorFace = winLbl->colorFace;
							pl->controlAttributes.lfControlFont = winLbl->myLblFont;
							pl->controlAttributes.m_usWidthMultiplier = winLbl->m_usWidthMultiplier;
							pl->controlAttributes.m_usHeightMultiplier = winLbl->m_usHeightMultiplier;
							pl->controlAttributes.m_nWidth = winLbl->m_nWidthMultiplier * CWindowButton::stdWidth;
							pl->controlAttributes.m_nHeight = winLbl->m_nHeightMultiplier * CWindowButton::stdHeight;
							pl->controlAttributes.isVirtualWindow = TRUE;
							pl->labelAttributes.Show = TRUE;
							pl->m_IconName = winLbl->m_csIcon;
							pl->labelAttributes.capAlignment = winLbl->myCapAlignment;
							pl->labelAttributes.horIconAlignment = winLbl->myHorIconAlignment;
							pl->labelAttributes.vertIconAlignment = winLbl->myVertIconAlignment;
							pl->labelAttributes.HorizontalOriented = winLbl->horizOrient;
							pl->labelAttributes.PickCounter = winLbl->isCounterLabel;
							pl->pContainer = pw;

							for (int i = 0; i < sizeof(pl->controlAttributes.SpecWin); i++){
								pl->controlAttributes.SpecWin[i] = winLbl->m_csSpecWin[i];
							}

							pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &= ~(SpecWinIndex_WIN_NOBORDER |
								SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT |
								SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT);

							if (winLbl->bSpecialBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_NOBORDER;
							if (winLbl->bLeftBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERLFT;
							if (winLbl->bRightBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERRHT;
							if (winLbl->bTopBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERTOP;
							if (winLbl->bBottomBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERBOT;

							//mark for repainting
							InvalidateRect (NULL, FALSE);
							pDoc->CheckPoint();
							CLeftView *myTree = (CLeftView*)m_TreeView;
							myTree->UpdateLabelControlTree(pl);

						}
						//did not pass checks
						else {
							//alert user according to which checks failed
							if(!grid)
								AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}
					}
				}
				//user clciked in a text area - let them know they cannot create a label here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl){
					AfxMessageBox( IDS_TEXTEXIST,  MB_OK,  0 );
				}
				//user clciked in a btn area - let them know they cannot create a label here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::ButtonControl){
					AfxMessageBox( IDS_BTNEXISTS,  MB_OK,  0 );
				}
			}
			//did the user click in an area where no control exists or in an inactive window
			else if (nSearch == 3 /*|| nSearch == 6*/) {

				/*intialize an instance of the dialog class for adding label*/
				CDWinLabel *winLbl = new CDWinLabel((CWindowLabelExt *)NULL);

				int status = winLbl->DoModal ();
				//user clicked ok
				if (status == IDOK) {


					// create label and assign attributes set in dialog
					CWindowLabelExt *pl = new CWindowLabelExt(0, row - pw->controlAttributes.m_nRow, column - pw->controlAttributes.m_nColumn);
					//if size of caption is not larger than max size of myCaption, then insert it into the structure
					//else, truncate the characters past max size and then insert
					if (sizeof(winLbl->m_csCaption) <= MaxSizeOfMyCaption){
						pl->myCaption = winLbl->m_csCaption;
					}
					else
					{
						TCHAR tempMyCaption[MaxSizeOfMyCaption];
						_tcsncpy_s(tempMyCaption, winLbl->m_csCaption, MaxSizeOfMyCaption);
						pl->myCaption = tempMyCaption;
					}
					_tcsncpy_s(pl->controlAttributes.m_myCaption, pl->myCaption, MaxSizeOfMyCaption);
					pl->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

					_tcsncpy_s(pl->labelAttributes.tcsLabelText, pl->myCaption, sizeof(pl->labelAttributes.tcsLabelText)/sizeof(pl->labelAttributes.tcsLabelText[0])-1);
					pl->labelAttributes.tcsLabelText[sizeof(pl->labelAttributes.tcsLabelText)/sizeof(pl->labelAttributes.tcsLabelText[0])-1] = 0;

					if (sizeof(winLbl->m_myName) <= MaxSizeOfMyCaption){
						pl->myName = winLbl->m_myName;
					}
					else
					{
						TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
						_tcsncpy_s(tempMyCaption, winLbl->m_myName, MaxSizeOfMyCaption);
						tempMyCaption[MaxSizeOfMyCaption] = 0;
						pl->myName = tempMyCaption;
					}
					_tcsncpy_s(pl->controlAttributes.m_myName, pl->myName, MaxSizeOfMyCaption);
					pl->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

					pl->controlAttributes.m_colorText = winLbl->colorText;
					pl->controlAttributes.m_colorFace = winLbl->colorFace;
					pl->controlAttributes.lfControlFont = winLbl->myLblFont;
					pl->controlAttributes.m_usWidthMultiplier = winLbl->m_usWidthMultiplier;
					pl->controlAttributes.m_usHeightMultiplier = winLbl->m_usHeightMultiplier;
					pl->controlAttributes.m_nWidth = winLbl->m_nWidthMultiplier * CWindowButton::stdWidth;
					pl->controlAttributes.m_nHeight = winLbl->m_nHeightMultiplier * CWindowButton::stdHeight;
					pl->controlAttributes.isVirtualWindow = TRUE;
					pl->controlAttributes.m_myId = uiGlobalID++;
					pl->labelAttributes.Show = TRUE;
					pl->m_IconName = winLbl->m_csIcon;
					pl->labelAttributes.capAlignment = winLbl->myCapAlignment;
					pl->labelAttributes.horIconAlignment = winLbl->myHorIconAlignment;
					pl->labelAttributes.vertIconAlignment = winLbl->myVertIconAlignment;
					pl->labelAttributes.HorizontalOriented = winLbl->horizOrient;
					pl->labelAttributes.PickCounter = winLbl->isCounterLabel;
					pl->pContainer = pw;

					for (int i = 0; i < sizeof(pl->controlAttributes.SpecWin); i++){
						pl->controlAttributes.SpecWin[i] = winLbl->m_csSpecWin[i];
					}

					pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &= ~(SpecWinIndex_WIN_NOBORDER |
						SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT |
						SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT);

					if (winLbl->bSpecialBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_NOBORDER;
					if (winLbl->bLeftBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERLFT;
					if (winLbl->bRightBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERRHT;
					if (winLbl->bTopBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERTOP;
					if (winLbl->bBottomBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERBOT;

					//create a CRect to represent the labels screen area
					CRect thisControl(pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
						pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
						pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn + winLbl->m_usWidthMultiplier,
						pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow + winLbl->m_usHeightMultiplier);

					/*call function to verify that the edited label will not overlap
					any other buttons, labels or text and assign result to flag*/
					BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,uiActiveID,thisControl, NULL);
					/*call function to verify that the edited button will not extend
					beyond the borders of the window and assign result to flag*/
					BOOL window =pDoc->ValidateWindow(thisControl,pw);
					//did both checks pass
					if(grid && window){
						//add button to the windows control list and mark for repainting
						pw->AddDataLast (pl);
						CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);
						CLeftView *myTree = (CLeftView*)m_TreeView;
						myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pw);
						pDoc->CheckPoint();
						InvalidateRect (NULL, FALSE);
					}
					// the checks did not pass
					else {
						//alert the user about the checks that failed
						if(!grid)
							AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
						if(!window)
							AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
					}

				}//user clicked ok
			}//nSearch = 3
		}//pw not null
	}//nSearch > 1
	uiActiveID = saveID;
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->ShowSelectedWindow(uiActiveID);
}

void CNewLayoutView::OnAddEditBox()
{
	//get the layouts document and verify it is valid
	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get the point that was clicked
	int column = cpLastRButton.x;
	int row = cpLastRButton.y;

	//convert the coordinates to our grid
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	UINT saveID = uiActiveID;

	/*see if anything exists where the user clicked
	0- no window was clicked in
	3- valid window but no control - create new button
	4- control exists here - edit if its a button
	6- user clicked in a window that isnt the active window*/
	int nSearch = pDoc->SearchForItem (row, column, uiActiveID);
	//a window was clicked in
	if (nSearch > 1) {
		//get the window that was clicked in and cast it to a CWindowItemExt
		CWindowControl *pc = pDoc->GetDataThis();
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (pc);

		//user clicked in an inactive window (sub window)
		if(nSearch == 6){
			//call function to get the window they clicked in
			pw = pDoc->GetSelectedDocWindow(uiActiveID);//GetSelectedWindow(pw, uiActiveID);
			//if the window isnt a subwindow of the active window, exit function
			if(pw == NULL){
				return;
			}
			//call function to see if an exixting item was clicked in and assign to nSearch
			nSearch = pw->SearchForControl(row,column);
		}

		//this window does exist
		if (pw) {
			//is there an existing control here?
			if (nSearch == 4) {
				//is this control a label
				if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::EditBoxControl){
					//get the label that was clicked in and cast it to CWindowLabelExt
					CWindowControl *wc = pw->GetDataThis ();
					CWindowEditBoxExt *pl = CWindowEditBoxExt::TranslateCWindowControl (wc);
					//initialize an instance of the dialog class for editing label with this label
					CDWinLabel *winLbl = new CDWinLabel(pl);

					//did the user click ok on the dialog
					if (winLbl->DoModal () == IDOK) {
						//create a CRect to contian the screen area of this label
						CRect thisControl(pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
							pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
							pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn + winLbl->m_usWidthMultiplier,
							pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow + winLbl->m_usHeightMultiplier);

						/*call function to verify that the edited label will not overlap
						any other labels, buttons, or text and assign result to flag*/
						BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
							pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
							uiActiveID,thisControl,
							wc);
						/*call function to verify that the edited label will not extend
						beyond the borders of the window and assign result to flag*/
						BOOL window =pDoc->ValidateWindow(thisControl,pw);
						//did both checks pass
						if(grid && window){
							//assign edited attributes from dialog to the label

							//if size of caption is not larger than max size of myCaption, then insert it into the structure
							//else, truncate the characters past max size and then insert
							if (sizeof(winLbl->m_csCaption) <= MaxSizeOfMyCaption){
								pl->myCaption = winLbl->m_csCaption;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, winLbl->m_csCaption, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pl->myCaption = tempMyCaption;
							}
							_tcsncpy_s(pl->controlAttributes.m_myCaption, pl->myCaption, MaxSizeOfMyCaption);
							pl->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

							_tcsncpy_s(pl->editboxAttributes.tcsLabelText, pl->myCaption, sizeof(pl->editboxAttributes.tcsLabelText)/sizeof(pl->editboxAttributes.tcsLabelText[0])-1);
							pl->editboxAttributes.tcsLabelText[sizeof(pl->editboxAttributes.tcsLabelText)/sizeof(pl->editboxAttributes.tcsLabelText[0])-1] = 0;

							if (sizeof(winLbl->m_myName) <= MaxSizeOfMyCaption){
								pl->myName = winLbl->m_myName;
							}
							else
							{
								TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
								_tcsncpy_s(tempMyCaption, winLbl->m_myName, MaxSizeOfMyCaption);
								tempMyCaption[MaxSizeOfMyCaption] = 0;
								pl->myName = tempMyCaption;
							}
							_tcsncpy_s(pl->controlAttributes.m_myName, pl->myName, MaxSizeOfMyCaption);
							pl->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

							pl->controlAttributes.m_colorText = winLbl->colorText;
							pl->controlAttributes.m_colorFace = winLbl->colorFace;
							pl->controlAttributes.lfControlFont = winLbl->myLblFont;
							pl->controlAttributes.m_usWidthMultiplier = winLbl->m_usWidthMultiplier;
							pl->controlAttributes.m_usHeightMultiplier = winLbl->m_usHeightMultiplier;
							pl->controlAttributes.m_nWidth = winLbl->m_nWidthMultiplier * CWindowButton::stdWidth;
							pl->controlAttributes.m_nHeight = winLbl->m_nHeightMultiplier * CWindowButton::stdHeight;
							pl->controlAttributes.isVirtualWindow = TRUE;
							pl->editboxAttributes.Show = TRUE;
							pl->m_IconName = winLbl->m_csIcon;
							pl->editboxAttributes.HorizontalOriented = winLbl->horizOrient;
							pl->pContainer = pw;

							for (int i = 0; i < sizeof(pl->controlAttributes.SpecWin); i++){
								pl->controlAttributes.SpecWin[i] = winLbl->m_csSpecWin[i];
							}

							pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &= ~(SpecWinIndex_WIN_NOBORDER |
								SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT |
								SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT);

							if (winLbl->bSpecialBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_NOBORDER;
							if (winLbl->bLeftBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERLFT;
							if (winLbl->bRightBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERRHT;
							if (winLbl->bTopBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERTOP;
							if (winLbl->bBottomBorder)
								pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERBOT;

							//mark for repainting
							InvalidateRect (NULL, FALSE);
							pDoc->CheckPoint();
							CLeftView *myTree = (CLeftView*)m_TreeView;
//RJC TEMP							myTree->UpdateLabelControlTree(pl);

						}
						//did not pass checks
						else {
							//alert user according to which checks failed
							if(!grid)
								AfxMessageBox( IDS_OVERLAP_ERR,  MB_OK,  0 );
							if(!window)
								AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
						}
					}
				}
				//user clciked in a text area - let them know they cannot create a label here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::TextControl){
					AfxMessageBox( IDS_TEXTEXIST,  MB_OK,  0 );
				}
				//user clciked in a btn area - let them know they cannot create a label here
				else if(pw->GetDataThis()->controlAttributes.m_nType == CWindowControl::ButtonControl){
					AfxMessageBox( IDS_BTNEXISTS,  MB_OK,  0 );
				}
			}
			//did the user click in an area where no control exists or in an inactive window
			else if (nSearch == 3 /*|| nSearch == 6*/) {

				/*intialize an instance of the dialog class for adding label*/
				CDWinLabel *winLbl = new CDWinLabel((CWindowEditBoxExt *)NULL);

				int status = winLbl->DoModal ();
				//user clicked ok
				if (status == IDOK) {


					// create label and assign attributes set in dialog
					CWindowEditBoxExt *pl = new CWindowEditBoxExt(0, row - pw->controlAttributes.m_nRow, column - pw->controlAttributes.m_nColumn);
					//if size of caption is not larger than max size of myCaption, then insert it into the structure
					//else, truncate the characters past max size and then insert
					if (sizeof(winLbl->m_csCaption) <= MaxSizeOfMyCaption){
						pl->myCaption = winLbl->m_csCaption;
					}
					else
					{
						TCHAR tempMyCaption[MaxSizeOfMyCaption];
						_tcsncpy_s(tempMyCaption, winLbl->m_csCaption, MaxSizeOfMyCaption);
						pl->myCaption = tempMyCaption;
					}
					_tcsncpy_s(pl->controlAttributes.m_myCaption, pl->myCaption, MaxSizeOfMyCaption);
					pl->controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

					_tcsncpy_s(pl->editboxAttributes.tcsLabelText, pl->myCaption, sizeof(pl->editboxAttributes.tcsLabelText)/sizeof(pl->editboxAttributes.tcsLabelText[0])-1);
					pl->editboxAttributes.tcsLabelText[sizeof(pl->editboxAttributes.tcsLabelText)/sizeof(pl->editboxAttributes.tcsLabelText[0])-1] = 0;

					if (sizeof(winLbl->m_myName) <= MaxSizeOfMyCaption){
						pl->myName = winLbl->m_myName;
					}
					else
					{
						TCHAR tempMyCaption[MaxSizeOfMyCaption+1];
						_tcsncpy_s(tempMyCaption, winLbl->m_myName, MaxSizeOfMyCaption);
						tempMyCaption[MaxSizeOfMyCaption] = 0;
						pl->myName = tempMyCaption;
					}
					_tcsncpy_s(pl->controlAttributes.m_myName, pl->myName, MaxSizeOfMyCaption);
					pl->controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

					pl->controlAttributes.m_colorText = winLbl->colorText;
					pl->controlAttributes.m_colorFace = winLbl->colorFace;
					pl->controlAttributes.lfControlFont = winLbl->myLblFont;
					pl->controlAttributes.m_usWidthMultiplier = winLbl->m_usWidthMultiplier;
					pl->controlAttributes.m_usHeightMultiplier = winLbl->m_usHeightMultiplier;
					pl->controlAttributes.m_nWidth = winLbl->m_nWidthMultiplier * CWindowButton::stdWidth;
					pl->controlAttributes.m_nHeight = winLbl->m_nHeightMultiplier * CWindowButton::stdHeight;
					pl->controlAttributes.isVirtualWindow = TRUE;
					pl->controlAttributes.m_myId = uiGlobalID++;
					pl->editboxAttributes.Show = TRUE;
					pl->m_IconName = winLbl->m_csIcon;
					pl->editboxAttributes.HorizontalOriented = winLbl->horizOrient;
					pl->pContainer = pw;

					for (int i = 0; i < sizeof(pl->controlAttributes.SpecWin); i++){
						pl->controlAttributes.SpecWin[i] = winLbl->m_csSpecWin[i];
					}

					pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &= ~(SpecWinIndex_WIN_NOBORDER |
						SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT |
						SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT);

					if (winLbl->bSpecialBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_NOBORDER;
					if (winLbl->bLeftBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERLFT;
					if (winLbl->bRightBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERRHT;
					if (winLbl->bTopBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERTOP;
					if (winLbl->bBottomBorder)
						pl->controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] |= SpecWinIndex_WIN_BORDERBOT;

					//create a CRect to represent the labels screen area
					CRect thisControl(pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,
						pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,
						pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn + winLbl->m_usWidthMultiplier,
						pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow + winLbl->m_usHeightMultiplier);

					/*call function to verify that the edited label will not overlap
					any other buttons, labels or text and assign result to flag*/
					BOOL grid = pDoc->IsValidGrid(pw->controlAttributes.m_nRow + pl->controlAttributes.m_nRow,pw->controlAttributes.m_nColumn + pl->controlAttributes.m_nColumn,uiActiveID,thisControl, NULL);
					/*call function to verify that the edited button will not extend
					beyond the borders of the window and assign result to flag*/
					BOOL window =pDoc->ValidateWindow(thisControl,pw);
					//did both checks pass
					if(grid && window){
						//add button to the windows control list and mark for repainting
						pw->AddDataLast (pl);
						CWindowItemExt *parent = CWindowItemExt::TranslateCWindowControl (pc);
						CLeftView *myTree = (CLeftView*)m_TreeView;
						myTree->InsertNewTextControl(parent->controlAttributes.m_myId,(CWindowControl *)pw);
						pDoc->CheckPoint();
						InvalidateRect (NULL, FALSE);
					}
					// the checks did not pass
					else {
						//alert the user about the checks that failed
						if(!grid)
							AfxMessageBox(IDS_OVERLAP_ERR,  MB_OK,  0 );
						if(!window)
							AfxMessageBox(IDS_WINDOW_ERR, MB_OK,0);
					}

				}//user clicked ok
			}//nSearch = 3
		}//pw not null
	}//nSearch > 1
	uiActiveID = saveID;
	CLeftView *myTree = (CLeftView*)m_TreeView;
	myTree->ShowSelectedWindow(uiActiveID);
}


void CNewLayoutView::_setPaneText(int statusBarIndex, LPCTSTR statusText)
{
	CMainFrame *parent = (CMainFrame *)GetParentOwner();
	if (parent != NULL)
		parent->m_wndStatusBar.SetPaneText(statusBarIndex, statusText, TRUE);
}

void CNewLayoutView::_selectItem(CWindowItemExt *pItem)
{
	TRACE("Hit me");
}

void CNewLayoutView::_indicateSelectedItem(CWindowControl *pWinControl)
{
	if (pWinControl->controlAttributes.Selected == TRUE)
	{
		CString tmpTxt;
		//tmpTxt.Format(_T("Name:%s, Caption:%s, Type:%d\n"), pWinControl->myName, pWinControl->myCaption, pWinControl->controlAttributes.m_nType);
		switch (pWinControl->controlAttributes.m_nType)
		{
		case CWindowControl::TextControl:
			{
			// TODO
			CWindowTextExt *pTextControl = CWindowTextExt::TranslateCWindowControl(pWinControl);
			tmpTxt.LoadString((CWindowTextExt::TranslateCWindowControl(pWinControl)->textAttributes.type) + IDS_TXT_SING_RCT_MAIN);
			}
			break;
		case CWindowControl::ButtonControl:
			tmpTxt.Format(_T("BUTTON - %s"), pWinControl->myCaption);
			((CLeftView *)m_TreeView)->UpdateButtonControlTree ((CWindowButtonExt *)pWinControl);
			break;
		case CWindowControl::WindowContainer:
			tmpTxt.Format(_T("WINDOW - %s"), pWinControl->myName);
			break;
		case CWindowControl::LabelControl:
			tmpTxt.Format(_T("LABEL  - %s"), pWinControl->myCaption);
			break;
		case CWindowControl::ListBoxControl:
			tmpTxt.Format(_T("LISTBOX  - %s"), pWinControl->myCaption);
			break;
		case CWindowControl::DocumentContainer:
			break;
		case CWindowControl::WindowGroup:
			break;
		case CWindowControl::WindowContainerAdHoc:
			break;
		default:
			ASSERT(0);
			break;
		}
		TRACE3("%S(%d): CNewLayoutView::_indicateSelectedItem().  text \"%s\".\n", __FILE__, __LINE__, tmpTxt);
		this->_setPaneText(1, tmpTxt);
	}
}

void CNewLayoutView::HighlightSelection(CDC *pDC, int row, int column)
{
	//TRACE("CNewLayoutView::HighlightSelection\n");

	CWindowDocumentExt * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWindowItemExt *pWin = NULL;
	CPen *pOldPen;
	CBrush* pOldBrush;

	SEARCHRC nSearch = (SEARCHRC) pDoc->SearchForItem(row, column, uiActiveID);
	if (nSearch != SR_NoItemFound) 
	{
		pWin = CWindowItemExt::TranslateCWindowControl(pDoc->GetDataThis());
		CPen wcBorder;
		pOldPen = pDC->SelectObject(&wcBorder);
		pOldBrush = (CBrush *)pDC->SelectObject(GetStockObject(HOLLOW_BRUSH));
	}

	switch (nSearch)
	{
		case SR_ActiveWindow:
			{
				pWin->controlAttributes.Selected = !pWin->controlAttributes.Selected;
				_indicateSelectedItem(pWin);

				CWindowControl *pwc = pWin->GetDataFirst();
				while(pwc != NULL)
				{
					pwc->controlAttributes.Selected = pWin->controlAttributes.Selected;
					pwc = pWin->GetDataNext ();
				}
				InvalidateRect (NULL, TRUE);
			}
			break;
		case SR_ButtonOrText:
			{
				CWindowControl *pwc = pWin->GetDataThis ();
				pwc->controlAttributes.Selected = !pwc->controlAttributes.Selected;
				_indicateSelectedItem(pwc);
				InvalidateRect (NULL, TRUE);
			}
			break;
		case SR_InactiveWindow:
			this->_setPaneText(1, _T("Inactive WINDOW"));
			TRACE("SR_InactiveWindow\n");
			break;
		case SR_NoItemFound:
			this->_setPaneText(1, _T(""));
			TRACE("SR_NoItemFound\n");
			break;
		default:
			TRACE("Unknown SR\n");
			break;
	}
	//InvalidateRect (NULL, TRUE);
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//			The following functions are the handlers for	 //
//			the toolbar buttons. They essentially call		 //
//			the same functions that the pop menu calls		 //
//			but in some cases, the cpLastRClick or			 //
//			cpLastLclick point needs to be set before		 //
//			the noraml functions are called					 //
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void CNewLayoutView::OnToolbarWingroup()
{
	CDWindowGroup *wg = new CDWindowGroup();
	wg->DoModal();
	return;
}

void CNewLayoutView::OnToolbarActiveWnd()
{
	CDActiveWndEdit *we = new CDActiveWndEdit();
	we->DoModal();
	return;
}

void CNewLayoutView::OnToolbarBtn()
{
	//get the point last point clickedin by the user and convert coordiantes
	int column = cpLastLButtonUp.x ;
	int row = cpLastLButtonUp.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	PopupButtonEditDialog (row, column);
}

void CNewLayoutView::OnToolbarOep()
{
	int column = cpLastLButtonUp.x ;
	int row = cpLastLButtonUp.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;
	PopSetOEPWin(row,column);
}

void CNewLayoutView::OnToolbarTxt()
{
	int column = cpLastLButtonUp.x ;
	int row = cpLastLButtonUp.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	PopAddTextWindow (row, column);
}

void CNewLayoutView::OnToolbarWnd()
{
	cpLastRButton = cpLastLButtonUp;
	OnPopSetwindow();
}

void CNewLayoutView::OnToolbarLbl()
{
	cpLastRButton = cpLastLButtonUp;
	OnAddLabel();
}

void CNewLayoutView::OnToolbarDelete()
{
	//get views documents and assert valid
	int column = cpLastLButtonUp.x ;
	int row = cpLastLButtonUp.y;
	column /= CWindowButtonExt::stdWidth;
	row /= CWindowButtonExt::stdHeight;

	CWindowDocumentExt* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->DeleteWndItem(row, column, uiActiveID);
	InvalidateRect(NULL, FALSE);
}

void CNewLayoutView::OnEditCopy()
{
	cpLastRButton = cpLastLButtonUp;
	OnPopupCopy();
}

void CNewLayoutView::OnEditCut()
{
	cpLastRButton = cpLastLButtonUp;
	OnPopupCut();
}

void CNewLayoutView::OnEditPaste()
{
	cpLastRButton = cpLastLButtonUp;
	OnPopupPaste();
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//			End Toolbar Command Handlers					 //
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
