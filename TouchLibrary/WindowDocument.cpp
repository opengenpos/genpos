// WindowDocument.cpp : implementation file
//

#include "stdafx.h"

#include "WindowControlSample.h"
#include "WindowDocument.h"
#include "WindowText.h"
#include "WindowButton.h"
#include "WindowItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWindowDocument,CWnd);

/////////////////////////////////////////////////////////////////////////////
// CWindowDocument


const int  CWindowDocument::m_nStdDeciInchWidth = 15;//75;   // standard grid width in tenths of a inch, 10 = .10 inch
const int  CWindowDocument::m_nStdDeciInchHeight = 15;//50;  // standard grid height in tenths of a inch, 10 = .10 inch
const int  CWindowDocument::m_nStdDeciInchLeading = 0;//10;  // standard leading in tenths of a inch, 10 = .10 inch

// The units for the parameters are in grids so we are basically saying that the
// CWindowDocument is a control which covers the entire screen which on an 800x600
// resolution screen would be from 0,0 (the top left corner) to 54, 40 (the bottom
// right corner.
//    800x600  -> 54 grid columns and 40 grid rows
//   1024x768  -> 69 grid columns and 51 grid rows
//   1024x768  -> 69 grid columns and 51 grid rows   (LAYOUT_SCREENWIDTH_HIRES x LAYOUT_SCREENHEIGHT_HIRES)
//   1440x900  -> 96 grid columns and 60 grid rows   (LAYOUT_SCREENWIDTH_1440 x LAYOUT_SCREENHEIGHT_1440)
//   1920x1080 -> 128 grid columns and 72 grid rows  (LAYOUT_SCREENWIDTH_1920 x LAYOUT_SCREENHEIGHT_1920)
const int  CWindowDocument::nGridColumns = 128;
const int  CWindowDocument::nGridRows = 72;

CWindowDocument * CWindowDocument::pDoc = 0;

// constructor for CWindowDocument class
// We create a CWindowControl which will be the basic control covering the entire
// display screen.  The units are in grids so we are basically saying that the
// CWindowDocument is a control which covers the entire screen which on an 800x600
// resolution screen would be from 0,0 (the top left corner) to 54, 40 (the bottom
// right corner.  This is calculated by using the following formula:
//     number of grids = resolution divided by stdWidth
//     columns = 800 / 15 = 53.3333   we round up to 54 which gives us an even number.
// Rounding up also removes a thin white stripe which is visible along the right window
// border.
// See function CWindowDocumentExt::ValidateWindowDoc() which depends on this control
// and calculation for determining that controls fit into the display.
CWindowDocument::CWindowDocument(int nGridColumns, int nGridRows) : CWindowControl (0, 0, 0, nGridColumns, nGridRows)
{	
	//initialize attributes
	documentAttributes.signatureStart = 0xABCDEF87;
	controlAttributes.m_nType = DocumentContainer;
	CWindowGroup *wg = new CWindowGroup();
	documentAttributes.myStartupClientRect.SetRect(0,0,600,450);
	documentAttributes.myClientRect.SetRect(0,0,600,450);
	documentAttributes.uiInitialID = 1;
	CWindowDocument::pDoc = this;
	//create default no grouping window group and add to documents group list
	wg->groupAttributes.GroupID = 0;
	wg->GroupName = _T("No Grouping");
	wg->GroupDesc = _T("Windows in this group will not be grouped with any other windows");
	listGroups.AddTail(wg);

	//File Summary data initialization
	for(int x = 0; x < 3; x++){
		documentAttributes.nVersion[x] = 0;
	}
	memset(&documentAttributes.lastModified,0, sizeof(documentAttributes.lastModified));
	csPEP = _T("");
	csSummary = _T("");
	csIconDir = _T("");
}

CWindowDocument::~CWindowDocument(void)
{
	//delete all of the controls in the documents control list before 
	//we destroy our document
	POSITION currentPos = listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = listControls.GetNext (currentPos);
		ASSERT (pwc);
		delete pwc;
		listControls.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}

	//delete all of the window groups in the documents window group list before 
	//we destroy our document
	currentPos = listGroups.GetHeadPosition ();
	currentPosLast = currentPos;
	while (currentPos) {
		CWindowGroup *wg = listGroups.GetNext (currentPos);
		ASSERT (wg);
		delete wg;
		listGroups.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}
}

void CWindowDocument::ClearDocument (void)
{
	//remove all of the controls in the documents control list
	POSITION currentPos = listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = listControls.GetNext (currentPos);
		ASSERT (pwc);
		delete pwc;
		listControls.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}
	
	//remove all window groups in the documents window group list
	currentPos = listGroups.GetHeadPosition ();
	currentPosLast = currentPos;
	while (currentPos) {
		CWindowGroup *wg = listGroups.GetNext (currentPos);
		ASSERT (wg);
		delete wg;
		listGroups.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}
}


BOOL CWindowDocument::CreateDocument (CWnd *pParentWnd, UINT nID, CRect *rectSize)
{
	BOOL bStatus = FALSE;
	CString  strMyClass;
	const TCHAR *tcsWindowName = _T("General POS 2.2");

	try
	{
	   strMyClass = AfxRegisterWndClass(
		  0,//CS_VREDRAW | CS_HREDRAW,
		  ::LoadCursor(NULL, IDC_ARROW),
		  (HBRUSH) ::GetStockObject(WHITE_BRUSH),
		  ::LoadIcon(NULL, IDI_APPLICATION));
	}
	catch (CResourceException* pEx)
	{
		  AfxMessageBox(_T("Couldn't register class! (Already registered?)"));
		  pEx->Delete();
	}

	if (pParentWnd) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE;
		CRect myRect;
		if (rectSize) {
			myRect = *rectSize;
		}
		else {
			pParentWnd->GetClientRect (&myRect);
		}
		bStatus = Create (strMyClass, tcsWindowName, dwStyle, myRect, pParentWnd, nID);
		ASSERT(bStatus);
	}
	else {
		DWORD dwStyle = WS_VISIBLE | WS_POPUP;
		DWORD dwStyleExt = WS_EX_LEFT | WS_EX_LTRREADING;
		CRect myRect (0, 0, 0, 0);
		if (rectSize) {
			myRect = *rectSize;
		}
		else {
			myRect.right = GetSystemMetrics (SM_CXSCREEN);
			myRect.bottom = GetSystemMetrics (SM_CYSCREEN);
		}
		bStatus = CreateEx (dwStyleExt, strMyClass, tcsWindowName, dwStyle, myRect, NULL, 0);
		ASSERT(bStatus);
	}

	CWindowControl *pListControl;
	POSITION currentPos = listControls.GetHeadPosition ();
	while (currentPos) {
		pListControl = listControls.GetNext (currentPos);
		ASSERT(pListControl);

		if(!pListControl->controlAttributes.isVirtualWindow){
			continue;
		}else{
			pListControl->WindowCreate (this);
		}
	}
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	return bStatus;
}


BOOL CWindowDocument::ToggleWindow (CWindowControl *pParent, UINT nId)
{
	CWindowControl *pc = 0;
	CWindowControl *pcTemp = 0, *pcTopWin = 0;
	CWnd  *pCwndHndle = 0;

	pcTemp = GetDataFirst ();
	pCwndHndle = GetTopWindow();
	while (pcTemp && pCwndHndle) {		//cycle through the documents control list
		if (pcTemp->m_hWnd == pCwndHndle->m_hWnd) {
			pcTopWin = pcTemp;
			break;
		}
		pcTemp = GetDataNext ();	//get next control in the documents list
	}

	pc = GetDataFirst ();
	while (pc) {		//cycle through the documents control list
		//check for subwindow items in this window control
		if(pc->controlAttributes.m_myId != nId){
			pcTemp = SearchNextWindowItemDown(pc, nId);
			//if a sub window item with matching id was found, assign it to pc
			if(pcTemp){
				pc = pcTemp;
			}
		}
		//if id matches and the control is a window item
		if (pc->controlAttributes.m_myId == nId) {
			if (pc->controlAttributes.m_nType == CWindowControl::WindowContainer) {
				CWindowItem *pw = CWindowItem::TranslateCWindowControl (pc);
				if (pw && pw->windowAttributes.GroupNum != 0) {
					// this window is part of a Window Group so we may need to treat it differently
					// depending on whether it is the Active Window of the Window Group or not.
					// If it is the Active Window of the Window Group then we need to treat the
					// entire group as something to be toggled. If this window is not a member of
					// a Window Group or if the window is not the Active Window of a group then
					// we just will do the toggle on the window.
					// We base the decision to PopupWindow() or PopdownWindow() the entire Window Group
					// based on the state of the window that is the target of the ToggleWindow().
					int iActiveWnd = SearchForGroupActiveWindowId(pw->windowAttributes.GroupNum);
					int iPopDown = (pc->IsWindowVisible ()) ? 1 : 0;
					CList <CWindowControl *, CWindowControl *> tempList;
					int iCount = SearchForItemByGroupNo (pw->windowAttributes.GroupNum, tempList);
					if (iActiveWnd && iCount > 1 && iActiveWnd == pw->controlAttributes.m_myId) {
						// the window is a member of a Window Group which contains more than one window
						// and this window is the Active Window of the group. So we are now going to
						// toggle not just this window but also the other windows of the Window Group
						// that this window is the Active Window of.
						if (iPopDown) {
							pw->PopdownWindow();
						} else {
							pw->PopupWindow(this);
						}
						POSITION pos = tempList.GetHeadPosition ();
						while (pos) {
							CWindowItem *pw2 = CWindowItem::TranslateCWindowControl (tempList.GetNext (pos));
							if (pw2 && pw2->controlAttributes.m_myId != nId) {
								if (iPopDown) {
									pw2->PopdownWindow();
								} else {
									pw2->PopupWindow(pw);
								}
							}
						}
						return 1;
					}
				}
				if (pc == pcTopWin && pc->IsWindowVisible ()) {
					return pc->PopdownWindow ();
				}
				else {
					return pc->PopupWindow (this);
				}
			}
		}
		pc = GetDataNext ();	//get next control in the documents list
	}

	return FALSE;
}

BOOL CWindowDocument::GetWindowName(UINT nId, TCHAR *tcsName)
{
	CWindowControl *pc = 0;
	CWindowControl *pcTemp = 0, *pcTopWin = 0;
	CWnd  *pCwndHndle = 0;

	pcTemp = GetDataFirst();
	pCwndHndle = GetTopWindow();
	while (pcTemp && pCwndHndle) {		//cycle through the documents control list
		if (pcTemp->m_hWnd == pCwndHndle->m_hWnd) {
			pcTopWin = pcTemp;
			break;
		}
		pcTemp = GetDataNext();	//get next control in the documents list
	}

	pc = GetDataFirst();
	while (pc) {		//cycle through the documents control list
						//check for subwindow items in this window control
		if (pc->controlAttributes.m_myId != nId) {
			pcTemp = SearchNextWindowItemDown(pc, nId);
			//if a sub window item with matching id was found, assign it to pc
			if (pcTemp) {
				pc = pcTemp;
			}
		}
		//if id matches and the control is a window item
		if (pc->controlAttributes.m_myId == nId && pc->controlAttributes.m_nType == CWindowControl::WindowContainer) {
			// indicate that we have found a window with this window id.
			// if a buffer is provided then copy the name of the window into it.
			if (tcsName) {
				_tcsncpy(tcsName, pc->controlAttributes.m_myName, 32);
				tcsName[31] = 0;
			}
			return TRUE;
		}
		pc = GetDataNext();	//get next control in the documents list
	}

	// indicate that we did not find a window matching this window id.
	return FALSE;
}

BOOL CWindowDocument::PopupWindow (CWindowControl *pParent, UINT nId)
{
	CWindowControl *pc = 0;
	CWindowControl *pcTemp = 0, *pcTopWin = 0;
	CWnd  *pCwndHndle = 0;

	pcTemp = GetDataFirst ();
	pCwndHndle = GetTopWindow();
	while (pcTemp && pCwndHndle) {		//cycle through the documents control list
		if (pcTemp->m_hWnd == pCwndHndle->m_hWnd) {
			pcTopWin = pcTemp;
			break;
		}
		pcTemp = GetDataNext ();	//get next control in the documents list
	}

	pc = GetDataFirst ();
	while (pc) {		//cycle through the documents control list
		//check for subwindow items in this window control
		if(pc->controlAttributes.m_myId != nId){
			pcTemp = SearchNextWindowItemDown(pc, nId);
			//if a sub window item with matching id was found, assign it to pc
			if(pcTemp){
				pc = pcTemp;
			}
		}
		//if id matches and the control is a window item
		if (pc->controlAttributes.m_myId == nId && pc->controlAttributes.m_nType == CWindowControl::WindowContainer) {
			return pc->PopupWindow (this);
		}
		pc = GetDataNext ();	//get next control in the documents list
	}

	return FALSE;
}


BOOL CWindowDocument::PopupWindow (CWindowControl *pParent, CWindowControl *pc )
{
	return pc->PopupWindow (this);
}

BOOL CWindowDocument::PopupWindow (CWindowControl *pParent, TCHAR *tcWindowName )
{
	CWindowControl *bi = SearchForItemByName (tcWindowName);

	if (bi) {
		return PopupWindow (this, bi);
	}

	return FALSE;
}

BOOL CWindowDocument::PopupWindow (CWindowControl *pParent, int WindowPrefix)
{
	CWindowControl *bi = SearchForItemByPrefix(WindowPrefix);

	if (bi) {
		return PopupWindow (this, bi);
	}

	return FALSE;
}

BOOL CWindowDocument::PopdownWindow (CWindowControl *pParent, CWindowControl *pc )
{
	return pc->PopdownWindow ();
}

BOOL CWindowDocument::PopdownWindow (CWindowControl *pParent, TCHAR *tcWindowName )
{
	CWindowControl *bi = SearchForItemByName (tcWindowName);

	if (bi) {
		return PopdownWindow (this, bi);
	}

	return FALSE;
}

BOOL CWindowDocument::PopdownWindow (CWindowControl *pParent, int WindowPrefix)
{
	CWindowControl *bi = SearchForItemByPrefix(WindowPrefix);

	if (bi) {
		return PopdownWindow (this, bi);
	}

	return FALSE;
}


void CWindowDocument::Serialize( CArchive& ar )
{
	UCHAR* pBuff = (UCHAR*) &documentAttributes;

	if (ar.IsStoring())
	{
		//copy from CSting to TCHAR so we can serialize it
		lstrcpy(documentAttributes.summary ,csSummary);
		lstrcpy(documentAttributes.PEPfile ,csPEP);
		lstrcpy(documentAttributes.IconDir ,csIconDir);

		//verify the start signature to check against data corruption
		ASSERT(documentAttributes.signatureStart = 0xABCDEF87);

		documentAttributes.myStartupClientRect = documentAttributes.myClientRect;
		//serialize the documents default settings
		DocumentDef.Serialize(ar);
		//serialize the documents attributes
		ar<<sizeof(_tagWinDocAttributes);
		for(int i = 0; i < sizeof(_tagWinDocAttributes); i++){
			ar<<pBuff[i];
		}

		//get teh count of the controls in the documents control list and store 
		ar << listControls.GetCount ();

		//cycle through the list and serialize each of the controls
		POSITION currentPos = listControls.GetHeadPosition ();
		while (currentPos) {
			CWindowControl *pwc = listControls.GetNext (currentPos);
			ASSERT(pwc);
			pwc->Serialize (ar);
		}
		
		//store the number of elements in the group list
		ar<<listGroups.GetCount();
		
		CWindowGroup *pwg;
		POSITION currentGPPos = listGroups.GetHeadPosition ();
		//loop through the documents group list and store the data for each window group
		while (currentGPPos) {
			pwg = listGroups.GetNext (currentGPPos);
			ASSERT(pwg);
			pwg->Serialize (ar);
		}
	}
	else
	{
		//variables to hold the count of items in each of the documents lists
		int nCount;
		int nGPCount;

		ULONG mySize;
		memset(&documentAttributes,0,sizeof(_tagWinDocAttributes));

		//serialize the documents default settings
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

		//verify the start signature to check against data corruption
		ASSERT(documentAttributes.signatureStart = 0xABCDEF87);

		//get the count of the documents control list
		ar >> nCount;
		CWindowControlSample pwc;
		//loop through all the stored controls and create them and add them to the doc's list
		for ( ; nCount > 0; nCount--) {
			CWindowControl * pc = pwc.SerializeNew (ar);
			ASSERT(pc);
			if (pc) {
				listControls.AddTail (pc);
			}
		}
		
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
			ASSERT(wg);
			if (wg) {
				wg->Serialize(ar);
				//add the window group to the documents list
				listGroups.AddTail(wg);
			}
		}
		//copy   TCHAR to CSting  
		csSummary = documentAttributes.summary;
		csPEP = documentAttributes.PEPfile;
		csIconDir = documentAttributes.IconDir;

	}
}

CRect CWindowDocument::StdRectangle(void)
{
	CDC junk;

	junk.CreateCompatibleDC (NULL);

	int nLogPixelInchWidth = 100;
	int nLogPixelInchHeight = 100;

	return CRect (0, 0, nLogPixelInchWidth, nLogPixelInchHeight);
}



BEGIN_MESSAGE_MAP(CWindowDocument, CWnd)
	//{{AFX_MSG_MAP(CWindowDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowDocument message handlers

int CWindowDocument::SearchForItem (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();
	POSITION currentPos = 0;

	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	ASSERT(controlAttributes.m_usWidthMultiplier > 0 && controlAttributes.m_usWidthMultiplier <= 5);
	ASSERT(controlAttributes.m_usHeightMultiplier > 0 && controlAttributes.m_usHeightMultiplier <= 5);

	//cycle through the items in the documents control list
	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CRect jj(bi->controlAttributes.m_nColumn, bi->controlAttributes.m_nRow, bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);
		// do a basic sanity check on the row and column values
		ASSERT(bi->controlAttributes.m_nRow >=0 && bi->controlAttributes.m_nRow <= 100);
		ASSERT(bi->controlAttributes.m_nColumn >=0 && bi->controlAttributes.m_nColumn <= 100);
		ASSERT(bi->controlAttributes.m_usWidthMultiplier > 0 && bi->controlAttributes.m_usWidthMultiplier <= 5);
		ASSERT(bi->controlAttributes.m_usHeightMultiplier > 0 && bi->controlAttributes.m_usHeightMultiplier <= 5);
		//if this controls id matches the specified id and the specified point lies in this control
		if (jj.PtInRect (CPoint (column, row)) && bi->controlAttributes.m_myId == id) {
			return 2;
		}
		else if (! currentPos) {
			currentPos = pclListCurrent;
		}
	}
	if (currentPos) {
		pclListCurrent = currentPos;
		return 1;
	}
	return 0;
}


CWindowControl * CWindowDocument::SearchForItemByName (TCHAR *tcWindowName)
{
	if (tcWindowName) {
		CWindowControl *bi = 0;
		POSITION pos = listControls.GetHeadPosition ();

		//cycle through the items in the documents control list
		while (pos) {
			bi = listControls.GetNext (pos);
			ASSERT(bi);
			if ( _tcscmp (tcWindowName, bi->controlAttributes.m_myName) == 0) {
				return bi;
			}
		}
	}
	return 0;
}

CWindowControl * CWindowDocument::SearchForItemByNameInWindow (TCHAR *tcWindowName, TCHAR *tcItemName, CWindowControl *(*pWindowControlList))
{
	if (tcWindowName) {
		CWindowControl *bi = 0;
		POSITION pos = listControls.GetHeadPosition ();

		//cycle through the items in the documents control list
		while (pos) {
			bi = listControls.GetNext (pos);
			ASSERT(bi);
			if (bi && _tcscmp (tcWindowName, bi->controlAttributes.m_myName) == 0) {
				CWindowControl *bi2 = bi->SearchForItemByName(tcItemName);
				if (pWindowControlList) {
					*pWindowControlList++ = bi;
					*pWindowControlList = bi2;
				}
				return bi2;
			}
		}
	}
	return 0;
}


CWindowControl * CWindowDocument::SearchForItemByPrefix (int WindowPrefix)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		if (WindowPrefix == _ttoi(bi->controlAttributes.m_myName)) {
			return bi;
		}
	}
	
	return 0;
}

CWindowControl * CWindowDocument::SearchForItemByEventTrigger_WId (int SpecWinEventTrigger_WId)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		if (bi->controlAttributes.SpecWin[SpecWinIndex_EventTrigger] & SpecWinEventTrigger_Indic) {
			if ((bi->controlAttributes.SpecWin[SpecWinIndex_EventTrigger] & SpecWinEventTrigger_Mask) == SpecWinEventTrigger_WId)
			return bi;
		}
	}

	return 0;
}


CWindowControl * CWindowDocument::SearchForItemByWindowId (int iWindowNo)
{
	CWindowControl *bi = 0;

	if (iWindowNo > 0) {
		POSITION pos = listControls.GetHeadPosition ();

		//cycle through the items in the documents control list
		while (pos) {
			bi = listControls.GetNext (pos);
			ASSERT(bi);
			CWindowItem *pw = CWindowItem::TranslateCWindowControl (bi);
			if (pw) {
				if (iWindowNo == pw->controlAttributes.m_myId) {
					break;
				}
			}
		}
	}

	return bi;
}

CWindowControl * CWindowDocument::SearchForGroupActiveWindow (int iGroupNo)
{
	int iWndNo = SearchForGroupActiveWindowId (iGroupNo);

	if (iWndNo) {
		return SearchForItemByWindowId(iWndNo);
	}
	return 0;
}

int CWindowDocument::SearchForGroupActiveWindowId (int iGroupNo)
{
	if (iGroupNo > 0) {
		POSITION currentPos = listGroups.GetHeadPosition ();
		while (currentPos) {
			CWindowGroup *wg = listGroups.GetNext (currentPos);
			ASSERT (wg);
			if (wg && wg->groupAttributes.GroupID == iGroupNo) {
				return (wg->groupAttributes.nActiveWnd);
			}
		}
	}

	return 0;
}

int  CWindowDocument::SearchForItemByGroupNo (int iGroupNo, CList <CWindowControl *, CWindowControl *> &list)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	int iCount = 0;

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CWindowItem * pw = CWindowItem::TranslateCWindowControl (bi);
		if (pw) {
			if (pw->windowAttributes.GroupNum == iGroupNo) {
				list.AddTail (pw);
				iCount++;
			}
		}
	}

	return iCount;
}

int CWindowDocument::SearchForItemCorner (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();
	POSITION currentPos = 0;

	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);

	//cycle through the items in the documents control list
	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		// do a basic sanity check on the row and column values
		ASSERT(bi->controlAttributes.m_nRow >=0 && bi->controlAttributes.m_nRow <= 100);
		ASSERT(bi->controlAttributes.m_nColumn >=0 && bi->controlAttributes.m_nColumn <= 100);
		CRect jj(bi->controlAttributes.m_nColumn, bi->controlAttributes.m_nRow, bi->controlAttributes.m_nColumn + 1, bi->controlAttributes.m_nRow + 1);
		//if this controls id matches the specified id and the specified point lies in this controls top left grid
		if (jj.PtInRect (CPoint (column, row)) && bi->controlAttributes.m_myId == id) {
			return 2;
		}
		else if (! currentPos) {
			currentPos = pclListCurrent;
		}
	}
	if (currentPos) {
		pclListCurrent = currentPos;
		return 1;
	}
	return 0;
}

CWindowControl * CWindowDocument::DeleteDataThis ()
{
	CWindowControl * pb;

	//are we at a current position in the documents control list
	if (pclListCurrent) {
		//get the contrl at the current position in the list and remove it
		pb = listControls.GetAt (pclListCurrent);
		ASSERT(pb);
		listControls.RemoveAt (pclListCurrent);
		return pb;
	}
	else {
		return 0;
	}
}

CWindowControl * CWindowDocument::SearchNextWindowItemDown(CWindowControl *pc, UINT nId)
{
	CWindowControl* pwc;
	CWindowControl* pcTemp;

	//is the control a window ite,
	if(pc->controlAttributes.m_nType == CWindowControl::WindowContainer){
		//get the first control in the window items control list
		pwc = ((CWindowItem*)pc)->GetDataFirst();
		while(pwc){
			//if this controls id does not match the specified id
			if(pwc->controlAttributes.m_myId != nId){
				//is it a window item
				if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
					//recursively call this function to search for sub window items of this subwindow item
					pcTemp = SearchNextWindowItemDown(pwc, nId);
					//was a sub window item found
					if(pcTemp){
						return pcTemp;
					}
				}
			}
			//this control id matches the specified id - check to see if it is a window item
			else if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
				return pwc;
			}
			//get the next control in the list
			pwc = ((CWindowItem*)pc)->GetDataNext();
		}
	}
	//no subwindow item was found - return NULL
	return NULL;
}


