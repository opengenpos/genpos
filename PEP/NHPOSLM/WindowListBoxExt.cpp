
#include "StdAfx.h"

//#include "NewLayoutView.h"
#include "WindowListBoxExt.h"
#include "Resource.h"
//#include "DButtonEdit.h"
#include "WindowDocument.h"
#include "ChildFrm.h"
#include "NewLayoutView.h"

CWindowListBoxExt::CWindowListBoxExt (int id, int row, int column) :
	CWindowListBox (id, row, column)
{
		controlAttributes.Selected = FALSE;
		m_listboxFont = 0;
}

CWindowListBoxExt::CWindowListBoxExt(CWindowControl *wc) :
	CWindowListBox(wc)
{
	controlAttributes.Selected = FALSE;
	m_listboxFont = 0;
}

CWindowListBoxExt::~CWindowListBoxExt()
{
	delete m_listboxFont;
}

BEGIN_MESSAGE_MAP(CWindowListBoxExt, CWindowListBox)
	//{{AFX_MSG_MAP(CWindowListBoxExt)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWindowListBoxExt::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	SetFocus();
	Invalidate(FALSE);
	if(pContainer){
		pContainer->OnCmdMsg(controlAttributes.m_myId,NULL,NULL,NULL);
	}

}

void CWindowListBoxExt::OnLButtonUp(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	GetTopLevelParent()->SetFocus();
	Invalidate(FALSE); //Redraw the button to be raised
}

//gets 1 grid * 1 grid squares for drawing
CRect CWindowListBoxExt::getRectangle (int row, int column)
{
	CRect myStdRect (0, 0, CWindowListBoxExt::stdWidth - CWindowListBoxExt::stdLeading,
						CWindowListBoxExt::stdHeight - CWindowListBoxExt::stdLeading );

	myStdRect.bottom = myStdRect.bottom + row * CWindowListBoxExt::stdHeight;
	myStdRect.left = myStdRect.left + column * CWindowListBoxExt::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowListBoxExt::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowListBoxExt::stdWidth;
	return myStdRect;
}

CRect CWindowListBoxExt::getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult)
{
	CRect myStdRect (0, 0, CWindowListBoxExt::stdWidth - CWindowListBoxExt::stdLeading,
						CWindowListBoxExt::stdHeight - CWindowListBoxExt::stdLeading );

	ASSERT ( usWidthMult > 0);
	ASSERT (usHeightMult > 0);

	myStdRect.left = myStdRect.left + column * CWindowListBoxExt::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowListBoxExt::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowListBoxExt::stdWidth + (usWidthMult - 1) * CWindowListBoxExt::stdWidth;
	myStdRect.bottom = myStdRect.bottom + row * CWindowListBoxExt::stdHeight + (usHeightMult - 1) * CWindowListBoxExt::stdHeight;

	return myStdRect;
}



//#define RED(x)		(x & 0x000000FF)
//#define GREEN(x)	(x & 0x0000FF00)>>0x08
//#define BLUE(x)		(x & 0x00FF0000)>>0x10
//#define DARKEN(x)	(x & 0x50)<<0x01

BOOL CWindowListBoxExt::WindowDisplay (CDC* pDC)
{
	int iSaveDC = pDC->SaveDC ();

	CPen penBlack;
	if(!controlAttributes.Selected){
		penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}
	else{
		penBlack.CreatePen(PS_DOT, 1, RGB(255, 0, 0));
	}
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	pDC->SetBkMode (TRANSPARENT);

	pDC->SelectObject(GetStockObject (HOLLOW_BRUSH));
	pDC->SetTextColor (controlAttributes.m_colorText);

	int nColumn = controlAttributes.m_nColumn;
	int nRow = controlAttributes.m_nRow;

	if (pContainer) {
		nColumn += pContainer->controlAttributes.m_nColumn;
		nRow  += pContainer->controlAttributes.m_nRow;
	}

	CRect myRect (CWindowListBox::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	pDC->Rectangle(myRect);

	CBrush brushButton;	brushButton.CreateSolidBrush(controlAttributes.m_colorFace);
	CBrush *pOldBrush = pDC->SelectObject(&brushButton);

	pDC->Rectangle(myRect);

	LOGFONT myLF; 

	memset(&myLF, 0x00, sizeof(LOGFONT));
	/*myLF.lfWeight = m_FontWeight;
	myLF.lfItalic = m_isItalic;
	_tcscpy_s(myLF.lfFaceName, m_FontName);
	
	myLF.lfHeight = -MulDiv(m_myFontSize/10, pDC->GetDeviceCaps(LOGPIXELSY), 72);*/
	myLF = controlAttributes.lfControlFont;

	CFont textFont;
	VERIFY(textFont.CreateFontIndirect(&myLF));
	pDC->SelectObject(&textFont);

	pDC->RestoreDC (iSaveDC);
	return TRUE;
}

void CWindowListBoxExt::ControlSerialize(CArchive &ar)
{
	UCHAR* pBuff = (UCHAR*) &lbAttributes;

	if (ar.IsStoring())
	{
		//call base class serialization
		CWindowListBox::ControlSerialize (ar);
	}
	else
	{
		//call base class serialization
		CWindowListBox::ControlSerialize (ar);
	}
}

BOOL CWindowListBoxExt::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
{
	CDC *pDC = pParentWnd->GetDC ();

	int nLogPixX = 100;//pDC->GetDeviceCaps (LOGPIXELSX);
	int nLogPixY = 100;//pDC->GetDeviceCaps (LOGPIXELSY);
	int xLeadingX = nLogPixX * CWindowDocument::m_nStdDeciInchLeading / 100;
	int xLeadingY = nLogPixY * CWindowDocument::m_nStdDeciInchLeading / 100;

	int nWidth = ((nLogPixX * scale * CWindowDocument::m_nStdDeciInchWidth) / 100) / 100;
	int nHeight = ((nLogPixY * scale * CWindowDocument::m_nStdDeciInchHeight) / 100) / 100;


	int xColumn = controlAttributes.m_nColumn * (nWidth + xLeadingX) + xLeadingX;
	int xRow = controlAttributes.m_nRow * (nHeight + xLeadingY) + xLeadingY;

	CRect windRect (xColumn, xRow, xColumn + controlAttributes.m_usWidthMultiplier * nWidth,
					xRow + controlAttributes.m_usHeightMultiplier * nHeight);

	if (nID)
		controlAttributes.m_myId = nID;

	Create (_T("LISTBOX"), myCaption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, windRect, pParentWnd, controlAttributes.m_myId);

	//////// Set the Font ////////////

	//posibly use CreatePointFontIndirect
	//if layout manager starts using a logfont
	//Needs to be changed so the button fonts
	//will work better
	if(!m_listboxFont){
		m_listboxFont = new CFont;
		m_listboxFont->CreateFontIndirect(&controlAttributes.lfControlFont);
	}
	SetFont(m_listboxFont, TRUE);
	///////////////////

	InvalidateRect (NULL, FALSE);
	pParentWnd->ReleaseDC(pDC);
	return TRUE;

}



void CWindowListBoxExt::DrawListBox(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CWindowItemExt* pWndItem)
{
	ASSERT(lpDraw);

	//get the layout view for access to WindowDocument object
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	CWindowDocument * pDoc = myView->GetDocument();
	ASSERT_VALID(pDoc);

	CDC* pDC;
	CWindowListBoxExt* pListBoxWnd;
	UINT edgeOption = EDGE_RAISED; //Set the button to be raised

	if(pWndItem){
		pListBoxWnd = (CWindowListBoxExt*)pWndItem->GetDlgItem(nIDCtl);
	}else{
		return;
	}
	if(lpDraw){
		pDC = CDC::FromHandle(lpDraw->hDC);
	}else{
		pDC = pListBoxWnd->GetDC();
	}

	int iSaveDC = pDC->SaveDC ();
	
	CPen myPen;
	myPen.CreatePen(PS_NULL, 1, pListBoxWnd->controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowListBox::getRectangleSized (0, 0, pListBoxWnd->controlAttributes.m_usWidthMultiplier, pListBoxWnd->controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;

	myBrush.CreateSolidBrush(pListBoxWnd->controlAttributes.m_colorFace);

	CBrush* pOldBrush = pDC->SelectObject(&myBrush);
	CBrush topEdge;
	CBrush bottomEdge;
	CBrush shadowEdge;
	if(pListBoxWnd->controlAttributes.m_colorFace != RGB(0x00, 0x00, 0x00)){
		shadowEdge.CreateSolidBrush(RGB(DARKEN(RED(pListBoxWnd->controlAttributes.m_colorFace)),
										DARKEN(GREEN(pListBoxWnd->controlAttributes.m_colorFace)), 
										DARKEN(BLUE(pListBoxWnd->controlAttributes.m_colorFace))));
	}else{
		shadowEdge.CreateSolidBrush(RGB(0x7F, 0x7F, 0x7F));
	}

	if(lpDraw->itemState == ODS_FOCUS){
		topEdge.CreateSolidBrush(RGB(0x00, 0x00, 0x00));
		bottomEdge.CreateSolidBrush(RGB( 0xFF, 0xFF, 0xFF));
		edgeOption |= ~EDGE_RAISED | EDGE_SUNKEN;  //Change from Raised to Sunken
	}else{
		topEdge.CreateSolidBrush(RGB( 0xFF, 0xFF, 0xFF));
		bottomEdge.CreateSolidBrush(RGB(0x00, 0x00, 0x00));
	}

	pDC->SelectObject(bottomEdge);
	pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
	pDC->SelectObject(topEdge);
	pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
	pDC->SelectObject(shadowEdge);
	pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
	pDC->SelectObject(myBrush);
	if(lpDraw->itemState == ODS_FOCUS){
		pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
	}else{
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
	}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();

	pListBoxWnd->ReleaseDC(pDC);
}
