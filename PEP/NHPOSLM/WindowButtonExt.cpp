
#include "StdAfx.h"

//#include "NewLayoutView.h"
#include "WindowButtonExt.h"
#include "Resource.h"
//#include "DButtonEdit.h"
#include "WindowDocument.h"
#include "ChildFrm.h"
#include "NewLayoutView.h"

CWindowButtonExt::CWindowButtonExt (int id, int row, int column) :
	CWindowButton (id, row, column)
{
		controlAttributes.Selected = FALSE;
		m_buttonFont = 0;
}

CWindowButtonExt::CWindowButtonExt(CWindowControl *wc) :
	CWindowButton(wc)
{
	controlAttributes.Selected = FALSE;
	m_buttonFont = 0;
}

CWindowButtonExt::~CWindowButtonExt()
{
	delete m_buttonFont;
}

BEGIN_MESSAGE_MAP(CWindowButtonExt, CWindowButton)
	//{{AFX_MSG_MAP(CWindowButtonExt)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWindowButtonExt::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	SetFocus();
	Invalidate(FALSE);
	if(pContainer){
		pContainer->OnCmdMsg(controlAttributes.m_myId,NULL,NULL,NULL);
	}

}

void CWindowButtonExt::OnLButtonUp(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	GetTopLevelParent()->SetFocus();
	Invalidate(FALSE); //Redraw the button to be raised
}

//gets 1 grid * 1 grid squares for drawing
CRect CWindowButtonExt::getRectangle (int row, int column)
{
	CRect myStdRect (0, 0, CWindowButtonExt::stdWidth - CWindowButtonExt::stdLeading,
						CWindowButtonExt::stdHeight - CWindowButtonExt::stdLeading );

	myStdRect.bottom = myStdRect.bottom + row * CWindowButtonExt::stdHeight;
	myStdRect.left = myStdRect.left + column * CWindowButtonExt::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowButtonExt::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowButtonExt::stdWidth;
	return myStdRect;
}

CRect CWindowButtonExt::getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult)
{
	CRect myStdRect (0, 0, CWindowButtonExt::stdWidth - CWindowButtonExt::stdLeading,
						CWindowButtonExt::stdHeight - CWindowButtonExt::stdLeading );

	ASSERT ( usWidthMult > 0);
	ASSERT (usHeightMult > 0);

	myStdRect.left = myStdRect.left + column * CWindowButtonExt::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowButtonExt::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowButtonExt::stdWidth + (usWidthMult - 1) * CWindowButtonExt::stdWidth;
	myStdRect.bottom = myStdRect.bottom + row * CWindowButtonExt::stdHeight + (usHeightMult - 1) * CWindowButtonExt::stdHeight;

	return myStdRect;
}



//#define RED(x)		(x & 0x000000FF)
//#define GREEN(x)	(x & 0x0000FF00)>>0x08
//#define BLUE(x)		(x & 0x00FF0000)>>0x10
//#define DARKEN(x)	(x & 0x50)<<0x01

BOOL CWindowButtonExt::WindowDisplay (CDC* pDC)
{
	int iSaveDC = pDC->SaveDC ();

	CPen penBlack;
	if(!controlAttributes.Selected){
		penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}
	else{
		penBlack.CreatePen(PS_DOT, 2, RGB(255, 180, 180));
	}
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	pDC->SetBkMode (TRANSPARENT);

	CBrush hatchBrush;
	if(btnAttributes.m_myPattern == ButtonPatternHoriz){
		hatchBrush.CreateHatchBrush(HS_HORIZONTAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)), DARKEN(GREEN(controlAttributes.m_colorFace)), DARKEN(BLUE(controlAttributes.m_colorFace))));
	}else if(btnAttributes.m_myPattern == ButtonPatternVert){
		hatchBrush.CreateHatchBrush(HS_VERTICAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)), DARKEN(GREEN(controlAttributes.m_colorFace)), DARKEN(BLUE(controlAttributes.m_colorFace))));
	}


	pDC->SelectObject(GetStockObject (HOLLOW_BRUSH));
	pDC->SetTextColor (controlAttributes.m_colorText);

	int nColumn = controlAttributes.m_nColumn;
	int nRow = controlAttributes.m_nRow;

	if (pContainer) {
		nColumn += pContainer->controlAttributes.m_nColumn;
		nRow  += pContainer->controlAttributes.m_nRow;
	}

	CRect myRect (CWindowButton::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	pDC->Rectangle(myRect);

	CBrush brushButton;	brushButton.CreateSolidBrush(controlAttributes.m_colorFace);
	CBrush *pOldBrush = pDC->SelectObject(&brushButton);


	if (btnAttributes.m_myShape == CWindowButton::ButtonShapeRect){
		pDC->Rectangle(myRect);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(&hatchBrush);
			pDC->Rectangle(myRect);
		}
	}else if (btnAttributes.m_myShape == CWindowButton::ButtonShapeElipse){
		pDC->Ellipse(myRect);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(&hatchBrush);
			pDC->Ellipse(myRect);
		}
	}else if (btnAttributes.m_myShape == CWindowButton::ButtonShapeRoundedRect){
		pDC->RoundRect(myRect, CPoint (17, 17));
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(&hatchBrush);
			pDC->RoundRect(myRect, CPoint (17, 17));
		}
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
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

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

void CWindowButtonExt::ControlSerialize(CArchive &ar)
{
	UCHAR* pBuff = (UCHAR*) &btnAttributes;
//	UCHAR* pBuffExt = (UCHAR*) &btnExtAttributes;

	if (ar.IsStoring())
	{

		//call base class serialization
		CWindowButton::ControlSerialize (ar);
	}
	else
	{
		//call base class serialization
		CWindowButton::ControlSerialize (ar);
	}
}

BOOL CWindowButtonExt::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
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

	Create (_T("BUTTON"), myCaption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, windRect, pParentWnd, controlAttributes.m_myId);

	//////// Set the Font ////////////

	//posibly use CreatePointFontIndirect
	//if layout manager starts using a logfont
	//Needs to be changed so the button fonts
	//will work better
	if(!m_buttonFont){
		m_buttonFont = new CFont;
		m_buttonFont->CreateFontIndirect(&controlAttributes.lfControlFont);
	}
	SetFont(m_buttonFont, TRUE);
	///////////////////

	InvalidateRect (NULL, FALSE);
	pParentWnd->ReleaseDC(pDC);
	return TRUE;

}



void CWindowButtonExt::DrawButton(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CWindowItemExt* pWndItem)
{
	ASSERT(lpDraw);

	//get the layout view for access to WindowDocument object
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	CWindowDocument * pDoc = myView->GetDocument();
	ASSERT_VALID(pDoc);

	CDC* pDC;
	CWindowButtonExt* pButtonWnd;
	UINT edgeOption = EDGE_RAISED; //Set the button to be raised

	if(pWndItem){
		pButtonWnd = (CWindowButtonExt*)pWndItem->GetDlgItem(nIDCtl);
	}else{
		return;
	}
	if(lpDraw){
		pDC = CDC::FromHandle(lpDraw->hDC);
	}else{
		pDC = pButtonWnd->GetDC();
	}

	int iSaveDC = pDC->SaveDC ();

	
	CPen myPen;
	myPen.CreatePen(PS_NULL, 1, pButtonWnd->controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowButton::getRectangleSized (0, 0, pButtonWnd->controlAttributes.m_usWidthMultiplier, pButtonWnd->controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;
	CBrush hatchBrush;

	myBrush.CreateSolidBrush(pButtonWnd->controlAttributes.m_colorFace);

	if(pButtonWnd->btnAttributes.m_myPattern == ButtonPatternHoriz){
		hatchBrush.CreateHatchBrush(HS_HORIZONTAL, 
			RGB(DARKEN(RED(pButtonWnd->controlAttributes.m_colorFace)),
				DARKEN(GREEN(pButtonWnd->controlAttributes.m_colorFace)), 
				DARKEN(BLUE(pButtonWnd->controlAttributes.m_colorFace))));
	}else if(pButtonWnd->btnAttributes.m_myPattern == ButtonPatternVert){
		hatchBrush.CreateHatchBrush(HS_VERTICAL, 
			RGB(DARKEN(RED(pButtonWnd->controlAttributes.m_colorFace)),
				DARKEN(GREEN(pButtonWnd->controlAttributes.m_colorFace)),
				DARKEN(BLUE(pButtonWnd->controlAttributes.m_colorFace))));
	}

	CBrush* pOldBrush = pDC->SelectObject(&myBrush);
	CBrush topEdge;
	CBrush bottomEdge;
	CBrush shadowEdge;
	if(pButtonWnd->controlAttributes.m_colorFace != RGB(0x00, 0x00, 0x00)){
		shadowEdge.CreateSolidBrush(RGB(DARKEN(RED(pButtonWnd->controlAttributes.m_colorFace)),
										DARKEN(GREEN(pButtonWnd->controlAttributes.m_colorFace)), 
										DARKEN(BLUE(pButtonWnd->controlAttributes.m_colorFace))));
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

	if(pButtonWnd->btnAttributes.m_myShape == ButtonShapeRect){
		pDC->SelectObject(bottomEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		if(lpDraw->itemState == ODS_FOCUS){
			pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
			}
		}else{
			pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
			}
		}
	}else if(pButtonWnd->btnAttributes.m_myShape == ButtonShapeElipse){
		pDC->SelectObject(bottomEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		if(lpDraw->itemState == ODS_FOCUS){
			pDC->Ellipse(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->Ellipse(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
			}
		}else{
			pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
			}
		}
	}else if(pButtonWnd->btnAttributes.m_myShape == ButtonShapeRoundedRect){
		pDC->SelectObject(bottomEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right, myRect.bottom, 17, 17);
		pDC->SelectObject(topEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(shadowEdge);
		pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(myBrush);
		if(lpDraw->itemState == ODS_FOCUS){
			pDC->RoundRect(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2, 17, 17);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->RoundRect(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2, 17, 17);
			}
		}else{
			pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4, 17, 17);
			if(pButtonWnd->btnAttributes.m_myPattern != ButtonPatternNone){
				pDC->SelectObject(hatchBrush);
				pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4, 17, 17);
			}
		}
	}

//***************** Icon on Button Begin
	HBITMAP hBitmap;
	int	    cxSource, cySource;
	BITMAP	bitmap;
	CString iconPath;

	cxSource = 0;
	cySource = 0;
	hBitmap = 0;
	if (pButtonWnd->m_Icon.GetLength() > 0) {
		//iconPath.Format(_T("\\FlashDisk\\NCR\\Saratoga\\Icons\\%s"), pButtonWnd->m_Icon);
		iconPath.Format(_T("%s\\%s"), pDoc->csIconDir, pButtonWnd->m_Icon);

		hBitmap = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBitmap) {
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			cxSource = bitmap.bmWidth;
			cySource = bitmap.bmHeight;
		}
	}

	if (hBitmap) {
		//Bitmaps cannot be drawn directly to the screen so a 
		//compatible memory DC is created to draw to, then the image is 
		//transfered to the screen
		HDC hdc = lpDraw->hDC;
		HDC hdcMem = CreateCompatibleDC(hdc);

		HGDIOBJ  hpOldObject = SelectObject(hdcMem, hBitmap);

		int xPos;
		int yPos;

		//The Horizontal and Vertical Alignment
		//For Images
		//Are set in the Layout Manager
		//the proper attribute will have to be checked against
		//for now the Image is centered on the button

		//Horizontal Alignment
		if(lpDraw->itemState == ODS_FOCUS){
			if(pButtonWnd->btnAttributes.horIconAlignment == IconAlignmentHLeft){//Image to left
				xPos = 2 + 2;
			}else if(pButtonWnd->btnAttributes.horIconAlignment == IconAlignmentHRight){//Image to right
				xPos = (myRect.right - cxSource - 3);
			}else {//Horizontal center
				xPos = ((myRect.right - cxSource) / 2) + 1;
			}
		}else
		{
			if(pButtonWnd->btnAttributes.horIconAlignment == IconAlignmentHLeft){//Image to left
				xPos = 2;
			}else if(pButtonWnd->btnAttributes.horIconAlignment == IconAlignmentHRight){//Image to right
				xPos = myRect.right - cxSource - 5;
			}else {//Horizontal center
				xPos = ((myRect.right - cxSource) / 2) - 1;
			}
		}
		
		//Vertical Alignment
		if(lpDraw->itemState == ODS_FOCUS){
			if(pButtonWnd->btnAttributes.vertIconAlignment == IconAlignmentVTop){//Image to top
				yPos = 2 + 2;
			}else if(pButtonWnd->btnAttributes.vertIconAlignment == IconAlignmentVBottom){//Image to bottom
				yPos = (myRect.bottom - cySource - 3);
			}else{//Vertical Center
				yPos = ((myRect.bottom - cySource) / 2) + 1;
			}
		}else
		{
			if(pButtonWnd->btnAttributes.vertIconAlignment == IconAlignmentVTop){//Image to top
				yPos = 2;
			}else if(pButtonWnd->btnAttributes.vertIconAlignment == IconAlignmentVBottom){//Image to bottom
				yPos = myRect.bottom - cySource - 5;
			}else{//Vertical Center
				yPos = ((myRect.bottom - cySource) / 2) - 1;
			}
		}

		BitBlt(hdc, xPos, yPos, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, hpOldObject);
		
		//Clean up
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
	}
//***************** Icon on Button End

	//Text is drawn by DrawText
	//The alignment is being set by
	//setting the CRect textRect to the values of tempRect(the area for drawing text on buttons)
	//then using DrawText with DT_CALCRECT to calculating the rect of the text
	//output to the button
	//the textRect is then adjusted depending on the alignment desired
	//basically this gets the size of the area the text will take up on the button
	//and adjusts where to draw the text inside the area of the button to get the
	//alignment desired

	pDC->SetTextColor(pButtonWnd->controlAttributes.m_colorText);
	CRect tempRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
	CRect textRect(tempRect);
	CString csButtonText(pButtonWnd->myCaption);
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

	switch(pButtonWnd->btnAttributes.capAlignment){
	case CaptionAlignmentVBottom:
		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);//calculate text area size on button
		if(textRect.bottom <= tempRect.bottom){
			//top of bottom aligned text is the height/size of the button(myRect.bottom) minus
			//the height/size of the text(textRect.bottom) minus 2 to move the text off the very bottom line
			textRect.top = tempRect.bottom - textRect.bottom - 2;
		}else{
			textRect.top = tempRect.top;
		}
		textRect.bottom = tempRect.bottom;//bottom of bottom aligned text
		//the right side is equal to the buttons right side for centering text
		textRect.right = tempRect.right;
		if(lpDraw->itemState == ODS_FOCUS){
			textRect.top += 2;
			textRect.left += 2;
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		}else{
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		}
		break;

	case CaptionAlignmentVTop:
		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);//calculate text area size on button
		textRect.top = tempRect.top + 2;
		textRect.bottom = tempRect.bottom;
		//the right side is equal to the buttons right side for centering text
		textRect.right = tempRect.right;
		if(lpDraw->itemState == ODS_FOCUS){
			textRect.top += 2;
			textRect.left += 2;
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		}else{
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		}
		break;

	case CaptionAlignmentHLeft:
	case CaptionAlignmentHRight:
	case CaptionAlignmentHCenter:
		{
			int textWidth = 0;//width of single character
			int textHeight = 0;//height of single character
			int numOfCharDown = 0;

			int captionLen = csButtonText.GetLength();
			TCHAR printText; //single character to print


			//get the text height and width
			textHeight = pDC->DrawText(csButtonText, textRect, DT_CALCRECT);
			if(captionLen > 0){
				textWidth = textRect.right / captionLen;
			}
			//determine the number of characters that can be shown down and across
			if(textHeight > 0){
				numOfCharDown = tempRect.bottom / (textHeight / 2 + 2);
			}

			textRect = tempRect; //intialize the textRect to myRect values

			//if more characters exist than can be shown on a single line
			//reduce the number printed to screen as they will not be shown anyway
			if(numOfCharDown < captionLen){
				captionLen = numOfCharDown;
			}
			//Center the text vertically by finding the top starting point
			textRect.top = ((tempRect.bottom - captionLen * ((textHeight / 2) + 2))/2) - 2;
			if(textRect.top < 2){
				textRect.top = 2;
			}
			if(pButtonWnd->btnAttributes.capAlignment == CaptionAlignmentHLeft){//LEFT JUSTIFY
				//move the left start point one character width from left side
				textRect.left += textWidth * 5/3;
				textRect.right = textRect.left + textWidth * 5/3;
			}else if(pButtonWnd->btnAttributes.capAlignment == CaptionAlignmentHRight){//RIGHT JUSTIFY
				//move the left start point one character width from right side
				//multiply by two because the text is printed to the right of the
				//start point so leave one character width from side and account
				//for the width of the text to be printed
				textRect.left = tempRect.right - (textWidth * 4);
				textRect.right = textRect.left + textWidth * 4;
			}

			//for the length of the caption get the character
			//to print, center if not left or right justified
			//add the height of a character to the top
			//moving the start point down for the next character
			for(int i = 0; i < captionLen; i++)
			{
				printText = csButtonText.GetAt(i);
				if(lpDraw->itemState == ODS_FOCUS){
							textRect.top += 2;
							textRect.left += 2;
							pDC->DrawText(&printText, 1, textRect, DT_CENTER);
							textRect.top -= 2;
							textRect.left -= 2;
				}else{
							pDC->DrawText(&printText, 1, textRect, DT_CENTER);
				}
				textRect.top += textHeight / 2 + 2;
			}
		}
		break;

	case CaptionAlignmentVMiddle://vertically center and horizontally center text
		//same as default no break
	default:
		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
		//top of vertically centered text is down half the difference of the button's rect and the text's rect
		if(textRect.bottom <= tempRect.bottom){
			textRect.top = (tempRect.bottom - textRect.bottom)/ 2;
			//bottom of text rect is down the same distance as the top was moved, which is equal to
			//the current value plus the top
			textRect.bottom = tempRect.bottom;
		}else{
			textRect.top = tempRect.top;
			textRect.bottom = tempRect.bottom;
		}
		
		//the right side is equal to the buttons right side for centering text
		textRect.right = tempRect.right;
		if(lpDraw->itemState == ODS_FOCUS){
			textRect.top += 2;
			textRect.left += 2;
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_VCENTER | DT_SINGLELINE);
		}else{
			pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_VCENTER | DT_SINGLELINE);
		}
			break;
		}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();
	hatchBrush.DeleteObject();

	pButtonWnd->ReleaseDC(pDC);
}


