

#include "StdAfx.h"
#include "WindowItemExt.h"
#include "WindowControlSampleExt.h"
#include "ChildFrm.h"
#include "NewLayoutView.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//Construct a CWindowItemExt using the specified values for the
//identifier, beginning row and column position, and the width and
//height of the CWindowItem.
//
//Also initialize the CWindowItemExt specific attributes.
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
CWindowItemExt::CWindowItemExt (int id, int row, int column, int width, int height) :
	CWindowItem (id, row, column, width, height)
{
	m_faceBrush = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//Construct a CWindowItemExt using the specified values using the 
//attributes of another CWindowItemExt
//
//Also initialize the CWindowItemExt specific attributes.
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
CWindowItemExt::CWindowItemExt(CWindowControl *wc) :
	CWindowItem(wc)
{
	m_faceBrush = 0;

}

CWindowItemExt::~CWindowItemExt()
{
	// the destructor for our base class, CWindowItem will take care of
	// the list of objects which we may have.  See CWindowItem class in
	// TouchLibrary file WindowItem.cpp.
	DeleteObject(m_faceBrush);
}

BEGIN_MESSAGE_MAP(CWindowItemExt, CWindowItem)
	//{{AFX_MSG_MAP(CWindowItemExt)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CWindowItemExt::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWindowItem::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_BTN){
		pDC->SetBkMode(TRANSPARENT);
		//this is the parent so the color of this FrameworkWndItem
		//is the color of the child windows background
		//return the HBRUSH used to paint the background of
		//button controls
		hbr = m_faceBrush;
	}
		
	// TODO: Return a different brush if the default is not desired
	return hbr;
}


void CWindowItemExt::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->CtlType == ODT_BUTTON)
	{
		CWindowButtonExt::DrawButton(nIDCtl, lpDrawItemStruct, this);
	}else{
		CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowItemExt::WindowDisplay (CDC* pDC)
//RETURN VALUE - BOOL
//PARAMETERS - pDC - device context for drawing
//DESCRIPTION - function is called to display window items. Windows are drawn or not drawn
//				and borders can differ according to the Show and Selected properties. This 
//				also calls functions to display other controls contained in the window
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWindowItemExt::WindowDisplay (CDC* pDC)
{
	int iSaveDC = pDC->SaveDC ();
	
	//create pen for the window border
	CPen penWinBorder;
	//get the windows color
	COLORREF WinColor = this->controlAttributes.m_colorFace;

	//is the window the currently active window
	if(this->controlAttributes.CurrentID == this->controlAttributes.m_myId){
		int i;
		int j;
		//create brush with the color of the window face
		CBrush WinFaceBrush(WinColor);
		CBrush brushSel(SELECTED_GRID);	//yellow - to show selected coordiante

		//get the layout view for access to the last left click
		CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
		CNewLayoutView *myView = pChild->GetRightPane();

		//get coordinate of last left click so we can show the user where they clicked
		int xSel = myView->cpLastLButtonUp.x;
		int ySel = myView->cpLastLButtonUp.y;
		xSel /= CWindowButtonExt::stdWidth;
		ySel /= CWindowButtonExt::stdHeight;

		//loop to paint the window the correct color
		for (i = this->controlAttributes.m_nRow; i < this->controlAttributes.m_usHeightMultiplier + this->controlAttributes.m_nRow; i++) {
			for (j = this->controlAttributes.m_nColumn; j < this->controlAttributes.m_usWidthMultiplier + this->controlAttributes.m_nColumn; j++) {
				if(ySel == i && xSel == j){
					pDC->SelectObject(&brushSel);
				}
				else{
					pDC->SelectObject(&WinFaceBrush);
				}
				pDC->Rectangle(CWindowButtonExt::getRectangle (i, j));
			}
		}
		//is the window not selected(
		if(!controlAttributes.Selected){
			//create a border the same color as the window
			penWinBorder.CreatePen(PS_SOLID, 3, WinColor);
		}
		//is the window selected
		else{
			//created a red dotted border
			penWinBorder.CreatePen(PS_DOT, 1, SELECTED_CONTROL);
		}
	}
	//the window is not the currently active window
	else{
		//is the property for the window to be shown set to true (nested windows)
		if(windowAttributes.Show){
			//is the window not selected
			if(!controlAttributes.Selected){
				//create a black border
				penWinBorder.CreatePen(PS_SOLID, 2, NONSELECTED_CONTROL);
			}
			//is the window selected
			else{
				//create a red dotted border
				penWinBorder.CreatePen(PS_DOT, 1, SELECTED_CONTROL);
			}
		}
		//is the property for the window to be shown set to false (nested windows)
		else{
			//verify that the windows has a valid parent
			if(pContainer){
				//if the windows parent is another window and its parents property is set to show it
				if(pContainer->controlAttributes.m_nType == CWindowControl::WindowContainer
					&& CWindowItemExt::TranslateCWindowControl(pContainer)->windowAttributes.Show){
					//create a dotted border
					penWinBorder.CreatePen(PS_DOT, 1, RGB(0, 0, 255)/*WinColor*/);
				}
				else{
					//create a null border (hiding the window)
					penWinBorder.CreatePen(PS_NULL, 1, RGB(0, 0, 255)/*WinColor*/);
				}
			}
		}
	}
	
	//device context operations
	CPen* pOldPen = pDC->SelectObject(&penWinBorder);
	CBrush* pOldBrush = (CBrush *)pDC->SelectObject(GetStockObject (HOLLOW_BRUSH));
	pDC->Rectangle(CWindowButtonExt::getRectangleSized (controlAttributes.m_nColumn, controlAttributes.m_nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));

	//cycle through this windows controls and make calls to display them
	POSITION  currentPos = ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		//get the next control
		CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
		pwc->controlAttributes.m_myId;
		//if we are showing this window or the control in the list is a window itme
		if(this->windowAttributes.Show || pwc->controlAttributes.m_nType == CWindowControl::WindowContainer ){
			//make sure the current active ID is known to this control
			pwc->controlAttributes.CurrentID = this->controlAttributes.CurrentID;
			//call function to display this control
			pwc->WindowDisplay (pDC);
		}

		//currentPosLast = currentPos;
	}

	pDC->RestoreDC (iSaveDC);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//																								   
//FUNCTION CWindowItemExt::ControlSerialize (CArchive &ar)
//RETURN VALUE - void
//PARAMETERS - pDC - CArchive object for serialization
//DESCRIPTION - Serialization for CWindowItemExt class. If we are saving, we pass the call 
//				to the base class. If we are loading, we serialize here
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void CWindowItemExt::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &windowAttributes;
	//UCHAR* pBuffExt = (UCHAR*) &windowExtAttributes;

	if (ar.IsStoring())
	{
		//call base class serialization
		CWindowItem::ControlSerialize (ar);
	}
	else
	{
		
		nEndOfArchive = 0;
		try {

			//load base class attibutes
			WindowDef.Serialize(ar);

			ULONG mySize;
			memset(&windowAttributes,0,sizeof(_tagWinItemAttributes));
			ar>>mySize;

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

			ASSERT(windowAttributes.signatureStart = 0xABCDEF87);

			POSITION pclListNext = ButtonItemList.GetHeadPosition ();
			POSITION pclList;
			CWindowControl *bp;
			if (pclListNext) {
				pclList = pclListNext;
				while (pclListNext && (bp = ButtonItemList.GetNext (pclListNext))) {
					delete bp;
					ButtonItemList.RemoveAt (pclList);
					pclList = pclListNext;
				}
			}

			CWindowControlSampleExt bpTemp;
			try {
				int nCount = 0;
				ar >> nCount;
				for ( ; nCount > 0; nCount--) {
					bp = bpTemp.SerializeNew (ar);
					bp->pContainer = this;
					ButtonItemList.AddTail (bp);
				}
			}
			catch (CArchiveException  *e) {
				e->Delete();
			}
			catch (CFileException *e) {
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


void CWindowItemExt::RemoveData(POSITION location)
{
	ButtonItemList.RemoveAt(location);
}


int CWindowItemExt::SearchForControl(int row, int column)
{
	//create a NULL WindowControl
	CWindowControl *bi = 0;

	POSITION pos = ButtonItemList.GetHeadPosition ();
	//while there are still controls left in the windows control list
	while (pos) {
		pclListCurrent = pos;
		//get the next control in the list and asign to bi
		bi = ButtonItemList.GetNext (pos);
		
		CRect kk;
		//control is a Text Area
		if(bi->controlAttributes.m_nType == CWindowControl::TextControl )
			kk.SetRect(/*pw->m_nColumn*/ + bi->controlAttributes.m_nColumn, 
					   /*pw->m_nRow*/ + bi->controlAttributes.m_nRow, 
					   /*pw->m_nColumn*/ + bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
					   /*pw->m_nRow*/ + bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);

		//control is a button
		if(bi->controlAttributes.m_nType == CWindowControl::ButtonControl)
			kk.SetRect(controlAttributes.m_nColumn + bi->controlAttributes.m_nColumn, 
					   controlAttributes.m_nRow + bi->controlAttributes.m_nRow, 
					   controlAttributes.m_nColumn + bi->controlAttributes.m_nColumn + bi->controlAttributes.m_usWidthMultiplier, 
					   controlAttributes.m_nRow + bi->controlAttributes.m_nRow + bi->controlAttributes.m_usHeightMultiplier);
		
		//is the point the user clicked contianed in the control
		if (kk.PtInRect (CPoint (column, row))) {
			return 4;
		}
	}
	//window item found with active ID but no control found at this point
	return 3;
}




BOOL CWindowItemExt::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
{
	int iRet;
	
	iRet = CWindowItem::WindowCreate (pParentWnd, nID, scale);

	if(!m_faceBrush){
		m_faceBrush = CreateSolidBrush(controlAttributes.m_colorFace);
	}

	return iRet;
}

