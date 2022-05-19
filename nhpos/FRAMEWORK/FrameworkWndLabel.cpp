// FrameworkWndLabel.cpp: implementation of the CFrameworkWndLabel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "framework.h"
#include "FrameworkWndLabel.h"
#include "FrameworkWndCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CFrameworkWndLabel::CFrameworkWndLabel(int id, int row, int column, int width, int height) :
	CWindowLabel (id, row, column, width, height)
{
	m_labelFont = 0;
	hIconBitmap = 0;
}

CFrameworkWndLabel::CFrameworkWndLabel(CWindowControl *wc) :
	CWindowLabel(wc)
{
	m_labelFont = 0;
	hIconBitmap = 0;
}

CFrameworkWndLabel::~CFrameworkWndLabel()
{
	delete m_labelFont;

	if (hIconBitmap) {
		DeleteObject(hIconBitmap);
		hIconBitmap = 0;
	}
}


/********************************************************
*	The following function ChangeRefreshIcon () is used by the
*	Connection Engine to allow for some types of actions such as
*	changing the icon displayed in the control.
*
*	We are extending the virtual function from WindowControl which
*	does nothing in the super class to actually change the icon here.
**/
int CFrameworkWndLabel::ChangeRefreshIcon(TCHAR *tcsIconFileName)
{
	HBITMAP hTempBitmapFocus = hIconBitmap;
	CString csTempIcon = m_IconName;

	m_IconName = _T("");
	hIconBitmap = 0;

	if (hTempBitmapFocus) {
		DeleteObject (hTempBitmapFocus);
	}

	if (tcsIconFileName) {
		m_IconName = tcsIconFileName;
	}
	else {
		m_IconName = csTempIcon;
	}

	if (m_IconName.GetLength() > 0) {
		CString iconPath;
		iconPath.Format(ICON_FILES_DIR_FORMAT, m_IconName);
		hIconBitmap = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	Invalidate ();
	UpdateWindow ();

	return 0;
}



BEGIN_MESSAGE_MAP(CFrameworkWndLabel, CWindowLabel)
	//{{AFX_MSG_MAP(CWindowTextExt)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*BOOL CFrameworkWndLabel::WindowDisplay (CDC* pDC)
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

BOOL CFrameworkWndLabel::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
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

	Create (_T("STATIC"),  myCaption, ulWindowAttributes, windRect, pParentWnd, controlAttributes.m_myId);

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


void CFrameworkWndLabel::OnPaint(void)
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

	pDC->Rectangle(&myRect);

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
//***************** Icon on Button Begin
	int	    cxSource, cySource;

	cxSource = 0;
	cySource = 0;
	if (m_IconName.GetLength() > 0) {
		if (!hIconBitmap) {
			CString iconPath;
			iconPath.Format(_T("\\FlashDisk\\NCR\\Saratoga\\Icons\\%s"), m_IconName);
			hIconBitmap = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
	}

	if (hIconBitmap) {
		BITMAP	bitmap;
		GetObject(hIconBitmap, sizeof(BITMAP), &bitmap);
		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight;

		if (cxSource > myRect.right)
			cxSource = myRect.right;

		if (cySource > myRect.bottom)
			cySource = myRect.bottom;

		//Bitmaps cannot be drawn directly to the screen so a 
		//compatible memory DC is created to draw to, then the image is 
		//transfered to the screen
		CDC hdcMem;
		hdcMem.CreateCompatibleDC(pDC);

		HGDIOBJ  hpOldObject = hdcMem.SelectObject(hIconBitmap);

		int xPos;
		int yPos;

		//The Horizontal and Vertical Alignment
		//For Images
		//Are set in the Layout Manager
		//the proper attribute will have to be checked against
		//for now the Image is centered on the button

		//Horizontal Alignment
		if(labelAttributes.horIconAlignment == IconAlignmentHLeft){//Image to left
			xPos = 2;
		}else if(labelAttributes.horIconAlignment == IconAlignmentHRight){//Image to right
			xPos = myRect.right - cxSource - 5;
		}else {//Horizontal center
			xPos = ((myRect.right - cxSource) / 2) - 1;
		}
		
		//Vertical Alignment
		if(labelAttributes.vertIconAlignment == IconAlignmentVTop){//Image to top
			yPos = 2;
		}else if(labelAttributes.vertIconAlignment == IconAlignmentVBottom){//Image to bottom
			yPos = myRect.bottom - cySource - 5;
		}else{//Vertical Center
			yPos = ((myRect.bottom - cySource) / 2) - 1;
		}

		pDC->BitBlt(xPos, yPos, cxSource, cySource, &hdcMem, 0, 0, SRCCOPY);

		hdcMem.SelectObject(hpOldObject);
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

	switch(labelAttributes.capAlignment){
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

		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		
		break;

	case CaptionAlignmentVTop:
		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);//calculate text area size on button
		textRect.top = tempRect.top + 2;
		textRect.bottom = tempRect.bottom;
		//the right side is equal to the buttons right side for centering text
		textRect.right = tempRect.right;

		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		
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
			if(labelAttributes.capAlignment == CaptionAlignmentHLeft){//LEFT JUSTIFY
				//move the left start point one character width from left side
				textRect.left += textWidth * 5/3;
				textRect.right = textRect.left + textWidth * 5/3;
			}else if(labelAttributes.capAlignment == CaptionAlignmentHRight){//RIGHT JUSTIFY
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
	
				pDC->DrawText(&printText, 1, textRect, DT_CENTER);
				
				//textRect.top += textHeight / 2 + 2;
				textRect.top += textHeight /*/ 2*/ - 2;
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

		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_VCENTER | DT_SINGLELINE);
		
		break;
	}

	pDC->RestoreDC (iSaveDC);
	EndPaint (&myPaint);

	//Clean up Objects
	myBrush.DeleteObject();
	ReleaseDC(pDC);
}