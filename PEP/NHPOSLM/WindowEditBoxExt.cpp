// WindowLabelExt.cpp: implementation of the CWindowLabelExt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newlayout.h"
#include "WindowEditBoxExt.h"
#include "WindowDocument.h"
#include "ChildFrm.h"
#include "NewLayoutView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindowEditBoxExt::CWindowEditBoxExt(int id, int row, int column) :
	CWindowEditBox (id, row, column)
{
	controlAttributes.Selected = FALSE;
	m_labelFont = 0;
}

CWindowEditBoxExt::CWindowEditBoxExt(CWindowControl *wc) :
	CWindowEditBox(wc)
{
	controlAttributes.Selected = FALSE;
	m_labelFont = 0;
}

CWindowEditBoxExt::~CWindowEditBoxExt()
{
	delete m_labelFont;
}

#if 0
void CWindowEditBoxExt::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &editboxAttributes;
	//UCHAR* pBuffExt = (UCHAR*) &windowExtAttributes;

	if (ar.IsStoring())
	{
		//call base class serialization
		CWindowLabel::ControlSerialize (ar);
	}
	else
	{
		
		nEndOfArchive = 0;
		try {

			ULONG mySize;
			memset(&editboxAttributes,0,sizeof(_tagWinEbAttributes));
			ar>>mySize;

			// Because the file that is being loaded in may be from a
			// different version of Layout Manager, we must be careful
			// about just reading in the information from the file.
			// If the object stored is larger than the object in the
			// version of Layout Manager being used then we will
			// overwrite memory and cause application failure.
			ULONG  xMySize = mySize;
			UCHAR  ucTemp;

			if (xMySize > sizeof (editboxAttributes)) {
				xMySize = sizeof (editboxAttributes);
			}
			UINT y = 0;
			for(y = 0; y < xMySize;y++){
				ar>>pBuff[y];
			}
			for(; y < mySize;y++){
				ar>>ucTemp;
			}

			ASSERT(editboxAttributes.signatureStart = 0xABCDEF87);

			//assign icon file name from TCHAR to CString 
			m_IconName = editboxAttributes.myIcon;
			
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
#endif
BEGIN_MESSAGE_MAP(CWindowEditBoxExt, CWindowEditBox)
	//{{AFX_MSG_MAP(CWindowEditBoxExt)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CWindowEditBoxExt::WindowDisplay (CDC* pDC)
{

	int iSaveDC = pDC->SaveDC ();

	CPen penBlack;
	if(!controlAttributes.Selected){
		penBlack.CreatePen(PS_SOLID, 1, NONSELECTED_CONTROL);
	}
	else{
		penBlack.CreatePen(PS_DOT, 1, SELECTED_CONTROL);
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

	CRect myRect (CWindowButton::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
//	pDC->Rectangle(myRect);

	CBrush brushButton;	brushButton.CreateSolidBrush(controlAttributes.m_colorFace);
	CBrush *pOldBrush = pDC->SelectObject(&brushButton);
	
	CPen penFace, penBorder, *pOldPenBorder = 0;

	pDC->Rectangle(&myRect);

	if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &
		(SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT | 
		SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT))
	{
		penBorder.CreatePen(PS_SOLID, 2, RGB(0,0,0));
		pOldPenBorder = pDC->SelectObject(&penBorder);
		CPoint myPoint = myRect.TopLeft ();
		pDC->MoveTo (myPoint);
		myPoint.Offset(0, myRect.Height ());
		if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERLFT)
		{
			pDC->LineTo (myPoint);
		}
		else {
			pDC->MoveTo (myPoint);
		}

		myPoint.Offset(myRect.Width (), 0);
		if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERBOT)
		{
			pDC->LineTo (myPoint);
		}
		else {
			pDC->MoveTo (myPoint);
		}

		myPoint.Offset(0, -myRect.Height ());
		if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERRHT)
		{
			pDC->LineTo (myPoint);
		}
		else {
			pDC->MoveTo (myPoint);
		}

		myPoint.Offset(-myRect.Width (), 0);
		if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERTOP)
		{
			pDC->LineTo (myPoint);
		}
		pDC->SelectObject(pOldPenBorder);
	}

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

	CRect textRect(myRect);

	CString csButtonText(myCaption);

	pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	//top of vertically centered text is down half the difference of the button's rect and the text's rect
	if(textRect.bottom <= myRect.bottom){
		textRect.top += (myRect.bottom - textRect.bottom)/ 2;
		//bottom of text rect is down the same distance as the top was moved, which is equal to
		//the current value plus the top
		textRect.bottom += (myRect.bottom - textRect.bottom)/ 2;
	}else{
		textRect.top = myRect.top;
		textRect.bottom = myRect.bottom;
	}

	textRect.right = myRect.right;
	pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);
	//pDC->SelectObject(pOldBrush);




//	pDC->Rectangle(myRect);

//	pDC->DrawText (myCaption, myRect, DT_WORDBREAK | DT_CENTER);

	pDC->RestoreDC (iSaveDC);
	return TRUE;	
}

BOOL CWindowEditBoxExt::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
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

	Create (_T("EDITBOX"),  myCaption, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, windRect, pParentWnd, controlAttributes.m_myId);


	//////// Set the Font ////////////

	//posibly use CreatePointFontIndirect
	//if layout manager starts using a logfont
	//Needs to be changed so the button fonts
	//will work better
	if(!m_labelFont){
		m_labelFont = new CFont;
		m_labelFont->CreateFontIndirect(&controlAttributes.lfControlFont);
	}
	SetFont(m_labelFont, TRUE);
	///////////////////

	//InvalidateRect (NULL, FALSE);
	pParentWnd->ReleaseDC(pDC);
	return TRUE;

}

void CWindowEditBoxExt::OnPaint(void)
{
	//get the layout view for access to WindowDocument object
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	CWindowDocument * pDoc = myView->GetDocument();
	ASSERT_VALID(pDoc);

	CRect myRect;
	GetClientRect (&myRect);

	PAINTSTRUCT  myPaint;
	CDC *pDC = BeginPaint (&myPaint);

	ASSERT(pDC);

	int iSaveDC = pDC->SaveDC ();

//	CRect myRect (CWindowButton::getRectangleSized (0, 0, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;

	myBrush.CreateSolidBrush(controlAttributes.m_colorFace);
	CBrush* pOldBrush = pDC->SelectObject(&myBrush);
	pDC->Rectangle(&myRect);



	//Text is drawn by DrawText
	//The alignment is being set by
	//setting the CRect textRect to the values of tempRect(the area for drawing text on buttons)
	//then using DrawText with DT_CALCRECT to calculating the rect of the text
	//output to the button
	//the textRect is then adjusted depending on the alignment desired
	//basically this gets the size of the area the text will take up on the button
	//and adjusts where to draw the text inside the area of the button to get the
	//alignment desired

	LOGFONT myLF; 

	memset(&myLF, 0x00, sizeof(LOGFONT));
	myLF = controlAttributes.lfControlFont;

	CFont textFont;
	VERIFY(textFont.CreateFontIndirect(&myLF));
	pDC->SelectObject(&textFont);

	pDC->SetTextColor(controlAttributes.m_colorText);
	pDC->SetBkColor(controlAttributes.m_colorFace);

	CRect tempRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
	CRect textRect(tempRect);
	CString csButtonText(myCaption);

	
	pDC->RestoreDC (iSaveDC);
	EndPaint (&myPaint);


	//Clean up Objects
	myBrush.DeleteObject();
	ReleaseDC(pDC);
}