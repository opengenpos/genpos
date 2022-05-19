/////////////////////////////////////////////////////////////////////////////
// WindowItem

#include "stdafx.h"

#include "WindowControlSample.h"
#include "WindowDocument.h"
#include "WindowControl.h"
#include "WindowButton.h"
#include "WindowText.h"
#include "WindowItem.h"

//IMPLEMENT_SERIAL(CWindowItem, Cwnd, SCHEMA_WINDOWITEM)


// this window procedure is used as part of the registration procedure
// but MFC over rides the registered message proc so that this procedure
// does not seem to actually be used.
// Command processing for buttons, etc, needs to be handled in the method
// OnCmdMsg below.
static LRESULT CALLBACK CWindowItemWindowProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	return DefWindowProc (hwnd, uMsg, wParam, lParam);
}


LPCTSTR CWindowItem::myClassName = _T("LAYOUTWINDOW");
DWORD CWindowItem::myStyle = WS_CHILD | WS_VISIBLE;
BOOL  CWindowItem::bAmRegistered = FALSE;

CWindowItem::CWindowItem(UINT id /* = 0 */, int row /* = 0 */, int column /* = 0 */, int width /* = 6 */, int height /* = 6 */ )
	: CWindowControl(id, row, column, width, height)
{
	//initialize attributes 
	controlAttributes.m_nType = WindowContainer;
	memset (&windowAttributes, 0, sizeof(windowAttributes));

	windowAttributes.Show = TRUE;
	windowAttributes.signatureStart = 0xABCDEF87;
	windowAttributes.GroupNum = 0;
	windowAttributes.busRules.NumOfPicks = 0;
	myCaption.Format (_T("W id %d  pos %d:%d"), id, row, column);
	windowAttributes.oepBtnWidth = 4;
	windowAttributes.oepBtnHeight = 4;
	windowAttributes.oepBtnDown = 4;
	windowAttributes.oepBtnAcross = 3;

	windowAttributes.oepBtnShape = CWindowButton::ButtonShapeRect;
	windowAttributes.oepBtnPattern = CWindowButton::ButtonPatternNone;
	windowAttributes.oepBtnColor = RGB(220, 220, 220);
	windowAttributes.oepBtnFontColor = RGB(0, 0, 0);
}

CWindowItem::CWindowItem(CWindowControl *wc) :
	CWindowControl(wc)
{
	//initilaize attributes 
	memset (&windowAttributes, 0, sizeof(windowAttributes));

	if (!wc || CWindowItem::TranslateCWindowControl(wc) == 0) {
		//initialize attributes 
		controlAttributes.m_nType = WindowContainer;

		windowAttributes.Show = TRUE;
		windowAttributes.signatureStart = 0xABCDEF87;
		windowAttributes.GroupNum = 0;
		windowAttributes.busRules.NumOfPicks = 0;
		windowAttributes.oepBtnWidth = 4;
		windowAttributes.oepBtnHeight = 4;
		windowAttributes.oepBtnDown = 4;
		windowAttributes.oepBtnAcross = 3;

		windowAttributes.oepBtnShape = CWindowButton::ButtonShapeRect;
		windowAttributes.oepBtnPattern = CWindowButton::ButtonPatternNone;
		windowAttributes.oepBtnColor = RGB(220, 220, 220);
		windowAttributes.oepBtnFontColor = RGB(0, 0, 0);
	}
	//initilaize attributes from an existing window item,
	else{
		windowAttributes.Show = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.Show;
		windowAttributes.GroupNum = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.GroupNum;
		windowAttributes.busRules.NumOfPicks = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.busRules.NumOfPicks ;
		windowAttributes.signatureStart = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.signatureStart;
		windowAttributes.oepBtnWidth = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnWidth;
		windowAttributes.oepBtnHeight = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnHeight;
		windowAttributes.oepBtnDown = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnDown;
		windowAttributes.signatureStart = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.signatureStart;
		windowAttributes.signatureStart = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.signatureStart;
		windowAttributes.oepBtnWidth = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnWidth;
		windowAttributes.oepBtnHeight = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnHeight;
		windowAttributes.oepBtnDown = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnDown;
		windowAttributes.oepBtnAcross = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnAcross;
		windowAttributes.oepBtnColor = (CWindowItem::TranslateCWindowControl(wc))->windowAttributes.oepBtnColor;
	}
}

CWindowItem::~CWindowItem()
{
	//cycle through the window items control list and remove all controls before the window is deleted
	POSITION  currentPos = ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
		ASSERT(pwc);
		delete pwc;
		ButtonItemList.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}
}


BOOL CWindowItem::WindowDisplay (CDC* pDC)
{
	//cycle through the window items control list and call window display function for each control
	POSITION  currentPos = ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
		ASSERT(pwc);
		pwc->WindowDisplay (pDC);;
		currentPosLast = currentPos;
	}
	return TRUE;
}


void CWindowItem::Serialize (CArchive &ar)
{
	//call base class serialization funtion and then call ControlSerialize function for this class
	if (ar.IsStoring())
	{
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
	else
	{
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
}

void CWindowItem::ControlSerialize (CArchive &ar)
{
	UCHAR* pBuff = (UCHAR*) &windowAttributes;
	if (ar.IsStoring())
	{
		//store the default attributes for items created in this window
		WindowDef.Serialize(ar);
		//verify teh start signature to check against data corruption
		ASSERT(windowAttributes.signatureStart = 0xABCDEF87);

		//serialize attributes for this class
		ar<<sizeof(_tagWinItemAttributes);
		for(int i = 0; i < sizeof(_tagWinItemAttributes); i++){
			ar<<pBuff[i];
		}
		
		//cycle through the control list and serialize each control
		ar << ButtonItemList.GetCount ();
		POSITION pclList = ButtonItemList.GetHeadPosition ();
		CWindowControl *bp;
		while (pclList && (bp = ButtonItemList.GetNext (pclList))) {
			ASSERT(bp);
			bp->Serialize (ar);
		}
	}
	else
	{
		nEndOfArchive = 0;
		try {
			//load the default attributes for items created in this window
			WindowDef.Serialize(ar);

			ULONG mySize;
			//serialize attributes for this class
			memset(&windowAttributes,0,sizeof(_tagWinItemAttributes));
			ar>>mySize;

			ASSERT(mySize <= sizeof(_tagWinItemAttributes));

			// Because the file that is being loaded in may be from a
			// different version of Layout Manager, we must be careful
			// about just reading in the information from the file.
			// If the object stored is larger than the object in the
			// version of Layout Manager being used then we will
			// overwrite memory and cause application failure.
			ULONG  xMySize = mySize;
			UCHAR  ucTemp;

			if (xMySize > sizeof (windowAttributes)) {
				xMySize = sizeof (windowAttributes);
			}
			UINT y = 0;
			for(y = 0; y < xMySize;y++){
				ar>>pBuff[y];
			}
			for(; y < mySize;y++){
				ar>>ucTemp;
			}
			
			//verify teh start signature to check against data corruption
			ASSERT(windowAttributes.signatureStart = 0xABCDEF87);

			//make sure teh control list is empty
			POSITION pclListNext = ButtonItemList.GetHeadPosition ();
			POSITION pclList;
			CWindowControl *bp;
			if (pclListNext) {
				pclList = pclListNext;
				while (pclListNext && (bp = ButtonItemList.GetNext (pclListNext))) {
					ASSERT(bp);
					delete bp;
					ButtonItemList.RemoveAt (pclList);
					pclList = pclListNext;
				}
			}
			
			//serialize all the controls that belong to this window
			CWindowControlSample bpTemp;
			try {
				int nCount = 0;
				ar >> nCount;
				for ( ; nCount > 0; nCount--) {
					bp = bpTemp.SerializeNew (ar);
					ASSERT(bp);
					if (bp) {
						bp->pContainer = this;
						ButtonItemList.AddTail (bp);
					}
				}
			}
			catch (CArchiveException  *e) {
				nEndOfArchive = 1;
				e->Delete();
			}
			catch (CFileException *e) {
				nEndOfArchive = 1;
				e->Delete();
			}
		}
		catch (CArchiveException  *e) {
			nEndOfArchive = 1;
			e->Delete();
		}
		catch (CFileException *e) {
			nEndOfArchive = 1;
			e->Delete();
		}
	}
}


BEGIN_MESSAGE_MAP(CWindowItem, CWindowControl)
	//{{AFX_MSG_MAP(CWindowButton)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// WindowItem message handlers

/*

	Handle command messages.

	nID contains the identifier of the control and nCode contains the indication code.
	If a button is pressed, we will get a command message with nID telling us
	which button was pressed and an nCode of zero (0) indicating a button click.

	Any messages that we handle will cause us to return TRUE.

	Otherwise, call the standard parent command message handler

 */
BOOL CWindowItem::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CWindowControl *pb = GetDataFirst ();

	while (pb) {
		//is this the button we recived the message from
		if (LOWORD(pb->controlAttributes.m_myId) == nID) {
			//does this button call another window
			if (pb->controlAttributes.m_myAction == (20400 + 102)) {    // this is IDD_P03_NOFUNC plus offset for P03_MENUWIND
				CWindowButton *pbx = CWindowButton::TranslateCWindowControl (pb);
				if (pbx) {
					ASSERT(pbx->btnAttributes.m_myActionUnion.data.nWinID > 0);
					//call function to popup the appropriate window
					CWindowDocument::pDoc->ToggleWindow (this, pbx->btnAttributes.m_myActionUnion.data.nWinID);
					return TRUE;
				}
			}
			//is this a window dismiss button?
			else if (pb->controlAttributes.m_myAction == (20400 + 103)) {  // this is IDD_P03_NOFUNC plus offset for P03_MENUWINDREMOVE
				// remove the current active window and enable the previous window, if it exists
				if (this->pPrev) {
					this->pPrev->EnableWindow(TRUE);
					/*Cycle through all window items and reshow previuos window(s) if it(they)
					were hidden by popup*/
					if(!this->controlAttributes.isVirtualWindow){
						CWindowControl *pwc = CWindowDocument::pDoc->GetDataFirst ();
						while (pwc) {
							if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer && pwc != this){
								if(pwc->m_hWnd)
									pwc->EnableWindow(TRUE);
							}
							pwc = CWindowDocument::pDoc->GetDataNext ();
						}
					}
					DestroyWindow ();
				}
				else {
					WindowDestroy ();
				}
			}
			return TRUE;
		}
		pb = GetDataNext ();
	}

	return CWindowControl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

afx_msg void CWindowItem::OnPaint(void)
{
	CRect myRect;
	if(IsWindowEnabled()){
		if (GetUpdateRect(&myRect, FALSE)) {
			PAINTSTRUCT  myPaint;
			CDC *pDC = BeginPaint (&myPaint);

			ASSERT(pDC);

			GetClientRect (&myRect);

			int iSaveDC = pDC->SaveDC ();

			CPen myPen(PS_SOLID, 1, controlAttributes.m_colorFace);
			CPen* pOldPen = (CPen *)pDC->SelectObject(myPen);

			CBrush myBrush(controlAttributes.m_colorFace);
			CBrush* pOldBrush = (CBrush *) pDC->SelectObject(myBrush);
			
			pDC->Rectangle(&myRect);

			pDC->RestoreDC (iSaveDC);
			EndPaint (&myPaint);
		}
	}
}

int CWindowItem::SearchForItem (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = ButtonItemList.GetHeadPosition ();
	POSITION currentPos = 0;

	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	ASSERT(controlAttributes.m_usWidthMultiplier > 0 && controlAttributes.m_usWidthMultiplier <= 5);
	ASSERT(controlAttributes.m_usHeightMultiplier > 0 && controlAttributes.m_usHeightMultiplier <= 5);

	//cycle through control list and search for item that has specified id and contians the row and column specified
	while (pos) {
		pclListCurrent = pos;
		bi = ButtonItemList.GetNext (pos);
		ASSERT(bi);
		// do a basic sanity check on the row and column values
		ASSERT(bi->controlAttributes.m_nRow >=0 && bi->controlAttributes.m_nRow <= 100);
		ASSERT(bi->controlAttributes.m_nColumn >=0 && bi->controlAttributes.m_nColumn <= 100);
		ASSERT(bi->controlAttributes.m_usWidthMultiplier > 0 && bi->controlAttributes.m_usWidthMultiplier <= 5);
		ASSERT(bi->controlAttributes.m_usHeightMultiplier > 0 && bi->controlAttributes.m_usHeightMultiplier <= 5);
		CRect jj(bi->controlAttributes.m_nColumn + controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow + controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + + controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
				 bi->controlAttributes.m_nRow + + controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);

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

int CWindowItem::SearchForItemCorner (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = ButtonItemList.GetHeadPosition ();
	POSITION currentPos = 0;

	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);

	//cycle through control list and search for item that has specified id and contians the row and column specified as the top left grid
	while (pos) {
		pclListCurrent = pos;
		bi = ButtonItemList.GetNext (pos);
		ASSERT(bi);
		// do a basic sanity check on the row and column values
		ASSERT(bi->controlAttributes.m_nRow >=0 && bi->controlAttributes.m_nRow <= 100);
		ASSERT(bi->controlAttributes.m_nColumn >=0 && bi->controlAttributes.m_nColumn <= 100);
		CRect jj(bi->controlAttributes.m_nColumn + controlAttributes.m_nColumn, 
				 bi->controlAttributes.m_nRow + controlAttributes.m_nRow, 
				 bi->controlAttributes.m_nColumn + controlAttributes.m_nColumn + 1, 
				 bi->controlAttributes.m_nRow + controlAttributes.m_nRow + 1);
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

CWindowControl * CWindowItem::DeleteDataThis ()
{
	CWindowControl * pb;

	//are we at a current position in our control list
	if (pclListCurrent) {
		//get the item at the current position in the list and remove it
		pb = ButtonItemList.GetAt (pclListCurrent);
		ButtonItemList.RemoveAt (pclListCurrent);
		return pb;
	}
	else {
		return 0;
	}
}


/*
		scale indicates the percentage of normal size and is the percentage expressed
		as an integer.  This means if you want 90%, scale should be equal to 90.

		The standard button widths are expressed in tenths of an inch.  this means
		if a standard button width is 10 then it is 1/10 inch.  So we multiply by
		the standard button metric along the appropriate axis and then divide by
		100 to arrive at the final size of the button.
 */
BOOL CWindowItem::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
{
	ASSERT(pParentWnd);

	if (! bAmRegistered) {
		WNDCLASS myWinClass;
		LRESULT  (CALLBACK *myProc) (HWND , UINT , WPARAM , LPARAM );

		myProc = CWindowItemWindowProc;

		myWinClass.style = 0;//CS_HREDRAW | CS_VREDRAW;
		myWinClass.lpfnWndProc = myProc;
		myWinClass.cbClsExtra = 0;
		myWinClass.cbWndExtra = 0;
		myWinClass.hInstance = 0;
		myWinClass.hIcon = 0;
		myWinClass.hCursor = 0;
		myWinClass.hbrBackground = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
		myWinClass.lpszMenuName = 0;
		myWinClass.lpszClassName = myClassName;

		bAmRegistered = AfxRegisterClass(&myWinClass);
		ASSERT(bAmRegistered);
	}

	//CDC *pDC = pParentWnd->GetDC ();

	int nLogPixX = 100;//pDC->GetDeviceCaps (LOGPIXELSX);
	int nLogPixY = 100;//pDC->GetDeviceCaps (LOGPIXELSY);
	int xLeadingX = nLogPixX * CWindowDocument::m_nStdDeciInchLeading / 100;
	int xLeadingY = nLogPixY * CWindowDocument::m_nStdDeciInchLeading / 100;

	//pParentWnd->ReleaseDC(pDC);

	int nWidth = ((nLogPixX * scale * CWindowDocument::m_nStdDeciInchWidth) / 100) / 100;
	int nHeight = ((nLogPixY * scale * CWindowDocument::m_nStdDeciInchHeight) / 100) / 100;

	int xColumn = controlAttributes.m_nColumn * (nWidth + xLeadingX) + xLeadingX;
	int xRow = controlAttributes.m_nRow * (nHeight + xLeadingY) + xLeadingY;

	CRect windRect (xColumn,
					xRow,
					xColumn + controlAttributes.m_usWidthMultiplier  * (nWidth + xLeadingX) + xLeadingX,
					xRow + controlAttributes.m_usHeightMultiplier  * (nHeight + xLeadingY) + xLeadingY);

	if (nID)
		controlAttributes.m_myId = nID;

	BOOL bStat;
	if (this->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_DISPLAYTRANS) {
		// to display on second monitor we need to change the characteristics of the window being
		// created. primarily it is not a child window with a parent of the main GenPOS window.
		// if it were a child window with a parent then it will be clipped to the parent's window.
		// using extended style of WS_EX_TOPMOST if it is to be displayed on the second monitor so that
		// if the second monitor is being used for other things such as advertising, our window
		// will not be covered over.
		bStat = CreateEx (WS_EX_TOPMOST, myClassName, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, windRect, NULL, NULL);
	} else {
		bStat = Create (NULL, myCaption, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS, windRect, pParentWnd, controlAttributes.m_myId);
	}

	ASSERT (bStat);

	if (bStat) {
		CWindowControl *bi = GetDataFirst ();
		while (bi) {
			bi->WindowCreate(this, 0, 100);
			bi->EnableWindow (TRUE);
			bi->UpdateWindow ();
			bi = GetDataNext ();
		}
		if(!this->controlAttributes.isVirtualWindow){
			this->ShowWindow(SW_HIDE);
		}
	}
	return bStat;
}
