// FrameworkWndEditBox.cpp: implementation of the CFrameworkWndEditBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "framework.h"
#include "FrameworkWndEditBox.h"
#include "FrameworkWndCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CFrameworkWndEditBox::CFrameworkWndEditBox(int id, int row, int column, int width, int height) :
	CWindowEditBox (id, row, column, width, height)
{
	m_labelFont = 0;
	hIconBitmap = 0;
}

CFrameworkWndEditBox::CFrameworkWndEditBox(CWindowControl *wc) :
	CWindowEditBox(wc)
{
	m_labelFont = 0;
	hIconBitmap = 0;
}

CFrameworkWndEditBox::~CFrameworkWndEditBox()
{
	delete m_labelFont;

	if (hIconBitmap) {
		DeleteObject(hIconBitmap);
		hIconBitmap = 0;
	}
}



BEGIN_MESSAGE_MAP(CFrameworkWndEditBox, CWindowEditBox)
	//{{AFX_MSG_MAP(CFrameworkWndEditBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*BOOL CFrameworkWndEditBox::WindowDisplay (CDC* pDC)
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

	CRect myRect (CWindowButton::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
//	pDC->Rectangle(myRect);

	CBrush brushButton;	brushButton.CreateSolidBrush(controlAttributes.m_colorFace);
	CBrush *pOldBrush = pDC->SelectObject(&brushButton);
	
	pDC->Rectangle(myRect);

	LOGFONT myLF; 

	memset(&myLF, 0x00, sizeof(LOGFONT));
	/*myLF.lfWeight = m_FontWeight;
	myLF.lfItalic = m_isItalic;
	_tcscpy(myLF.lfFaceName, m_FontName);
	
	myLF.lfHeight = -MulDiv(m_myFontSize/10, pDC->GetDeviceCaps(LOGPIXELSY), 72);*/
/*	myLF = controlAttributes.lfControlFont;

	CFont textFont;
	VERIFY(textFont.CreateFontIndirect(&myLF));
	pDC->SelectObject(&textFont);

	CRect textRect(myRect);

	CString csButtonText(controlAttributes.m_myCaption);

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
}*/

BOOL CFrameworkWndEditBox::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
{
//	CDC *pDC = pParentWnd->GetDC ();

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

	ULONG  ulWindowAttributes = WS_CHILD | WS_VISIBLE | WS_THICKFRAME;
	if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_NOBORDER) {
		ulWindowAttributes ^= WS_THICKFRAME;
	}

	Create (_T("EDIT"),  myCaption, ulWindowAttributes, windRect, pParentWnd, controlAttributes.m_myId);

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
//	pParentWnd->ReleaseDC(pDC);
	return TRUE;

}


void CFrameworkWndEditBox::OnPaint(void)
{
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

	CPen penFace, penBlack, *pOldPen;

	if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_NOBORDER)
	{
		penFace.CreatePen(PS_SOLID, 1, controlAttributes.m_colorFace);
		pOldPen = pDC->SelectObject(&penFace);
	}

//	pDC->Rectangle(&myRect);

	if (controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] &
		(SpecWinIndex_WIN_BORDERLFT | SpecWinIndex_WIN_BORDERRHT | 
		SpecWinIndex_WIN_BORDERTOP | SpecWinIndex_WIN_BORDERBOT))
	{
		penBlack.CreatePen(PS_SOLID, 2, RGB(0,0,0));
		pOldPen = pDC->SelectObject(&penBlack);
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
	}

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