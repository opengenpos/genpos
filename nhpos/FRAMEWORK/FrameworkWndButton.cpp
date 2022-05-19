/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP.C (main/common)
: --------------------------------------------------------------------------
:    Georgia Southern University Research Services Foundation
:    donated by NCR to the research foundation in 2002 and maintained here
:    since.
:       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
:       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
:       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
:       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
:       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
:       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
:       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
:
:    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
: --------------------------------------------------------------------------
:  Abstract:
:   FrameworkWndButton.cpp : implementation file
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date    : Ve.Rev.  : Name         : Description
: 11-14-03  : 02.00.00 : R.Herrington : initial                                   
** GenPOS
: Sep-07-18 : 02.02.02 : R.Chambers   : Add FSC_PRESET_AMT #5, #6, #7, #8
: -------------------------------------------------------------------------*/

//

#include "stdafx.h"
#include "Framework.h"
#include "FrameworkWndButton.h"
#include "FrameworkWndCtrl.h"
#include "bl.h"
#include "rfl.h"
#include "uifpgequ.h"
#include "Evs.h"
#include "appllog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCTSTR ButtonCaptionTransform (TCHAR  *ptcsBufferDest, LPCTSTR ptcsBufferSrc)
{
	LPCTSTR  ptcsBufferSave = ptcsBufferSrc;

	if (ptcsBufferSrc && ptcsBufferDest) {
		ptcsBufferSave = ptcsBufferDest;
		while (*ptcsBufferSrc) {
			if (*ptcsBufferSrc == '&') {
				*ptcsBufferDest++ = *ptcsBufferSrc++;
				*ptcsBufferDest++ = '&';
			} else {
				*ptcsBufferDest++ = *ptcsBufferSrc++;
			}
		}
		*ptcsBufferDest++ = *ptcsBufferSrc++;
	}

	return ptcsBufferSave;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndButton

CFrameworkWndButton::CFrameworkWndButton(int id, int row, int column, int width, int height) :
	CWindowButton (id, row, column, width, height)
{
	m_buttonFont = 0;
	m_DialogId = 0;
	m_hBitmapFocus = 0;
	memset (auchPluNo, 0, sizeof(auchPluNo));;
	memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
	uchPluAdjMod = 0;
}

CFrameworkWndButton::CFrameworkWndButton(CWindowControl *wc) :
	CWindowButton(wc)
{
	m_buttonFont = 0;
	m_DialogId = 0;
	m_hBitmapFocus = 0;
	auchPluNo[0] = 0;
	memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
	uchPluAdjMod = 0;
}

CFrameworkWndButton::~CFrameworkWndButton()
{
	// Clean up the bitmap handle and release the object
	if (m_hBitmapFocus) {
		DeleteObject(m_hBitmapFocus);
		m_hBitmapFocus = 0;
	}
	if (m_buttonFont) {
		delete m_buttonFont;
		m_buttonFont = 0;
	}

	//	DestroyWindow();

	TRACE3("%S(%d): CFrameworkWndButton::~CFrameworkWndButton() Destroying button '%s'\n", __FILE__, __LINE__, myCaption);
}

BOOL CFrameworkWndButton::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
{
	ASSERT(pParentWnd);

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

	CRect windRect (xColumn, xRow, xColumn + controlAttributes.m_usWidthMultiplier * nWidth,
					xRow + controlAttributes.m_usHeightMultiplier * nHeight);

	if (nID)
		controlAttributes.m_myId = nID;

	TCHAR  tempCaption [256];
	ButtonCaptionTransform (tempCaption, myCaption);
	BOOL isCreated = Create (_T("BUTTON"), tempCaption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, windRect, pParentWnd, controlAttributes.m_myId);
	ASSERT(isCreated);

	if (isCreated) {
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
	}
	else {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_WINCREATE_FAIL_2);
	}
	return isCreated;
}

BOOL CFrameworkWndButton::WindowDisplay (CDC* pDC)
{
	ASSERT(pDC);

	int iSaveDC = pDC->SaveDC ();

	CPen myPen;
	myPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	CPen* pOldPen = pDC->SelectObject(&myPen);

	pDC->SetBkMode(TRANSPARENT);

	CBrush myBrush (controlAttributes.m_colorFace);
	CBrush* pOldBrush = pDC->SelectObject(&myBrush);

	int nColumn = controlAttributes.m_nColumn;
	int nRow = controlAttributes.m_nRow;

	if (pContainer) {
		nColumn += pContainer->controlAttributes.m_nColumn;
		nRow  += pContainer->controlAttributes.m_nRow;
	}
	CRect myRect (CWindowButton::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));

	pDC->Rectangle(myRect);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	pDC->RestoreDC (iSaveDC);
	return TRUE;
}
#define RED(x)		(x & 0x000000FF)
#define GREEN(x)	(x & 0x0000FF00)>>0x08
#define BLUE(x)		(x & 0x00FF0000)>>0x10
#define DARKEN(x)	(x & 0x50)<<0x01

/********************************************************
*	The following function ChangeRefreshIcon () is used by the
*	Connection Engine to allow for some types of actions such as
*	changing the icon displayed in the control.
*
*	We are extending the virtual function from WindowControl which
*	does nothing in the super class to actually change the icon here.
**/
int CFrameworkWndButton::ChangeRefreshIcon(TCHAR *tcsIconFileName)
{
	CString csTempIcon = m_Icon;

	m_Icon = _T("");

	if (m_hBitmapFocus) {
		DeleteObject (m_hBitmapFocus);
		m_hBitmapFocus = 0;
		memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
	}

	if (tcsIconFileName) {
		m_Icon = tcsIconFileName;
	}
	else {
		m_Icon = csTempIcon;
	}

	if (m_Icon.GetLength() > 0) {
		CString iconPath;
		iconPath.Format(ICON_FILES_DIR_FORMAT, m_Icon);
		m_hBitmapFocus = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (m_hBitmapFocus) {
			int iNoBytes = GetObject(m_hBitmapFocus, sizeof(BITMAP), &m_bitmapFocus);
			if (iNoBytes < 1) {
				char xBuff[128];
				sprintf (xBuff, "GetObject() failed. GetLastError = %d", GetLastError ());
				NHPOS_ASSERT_TEXT((iNoBytes > 0), xBuff);
			}
		} else {
			memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
		}
	}

	Invalidate ();
	UpdateWindow ();

	return 0;
}


void CFrameworkWndButton::DrawButton(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CFrameworkWndItem* pWndItem)
{
	ASSERT(lpDraw);

	CDC* pDC;
	CFrameworkWndButton* pButtonWnd;

	if(! pWndItem || ! lpDraw){
		return;
	}

	pButtonWnd = (CFrameworkWndButton*)pWndItem->GetDlgItem(nIDCtl);
	pDC = CDC::FromHandle(lpDraw->hDC);

	if (lpDraw->itemState == ODS_FOCUS)
	{
		pButtonWnd->DrawButtonOnDisplayFocus (pDC);
	}
	else
	{
		pButtonWnd->DrawButtonOnDisplayNoFocus (pDC);
	}
	pButtonWnd->ReleaseDC(pDC);
}

void CFrameworkWndButton::DrawButtonOnDisplayFocus (CDC* pDC)
{	
	int iSaveDC = pDC->SaveDC ();

	CPen myPen;
	myPen.CreatePen(PS_NULL, 2, controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowButton::getRectangleSized (0, 0, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;
	CBrush hatchBrush;

	myBrush.CreateSolidBrush(controlAttributes.m_colorFace);

	if(btnAttributes.m_myPattern == ButtonPatternHoriz){
		hatchBrush.CreateHatchBrush(HS_HORIZONTAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)),
				DARKEN(GREEN(controlAttributes.m_colorFace)), 
				DARKEN(BLUE(controlAttributes.m_colorFace))));
	}else if(btnAttributes.m_myPattern == ButtonPatternVert){
		hatchBrush.CreateHatchBrush(HS_VERTICAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)),
				DARKEN(GREEN(controlAttributes.m_colorFace)),
				DARKEN(BLUE(controlAttributes.m_colorFace))));
	}

	CBrush* pOldBrush = pDC->SelectObject(&myBrush);
	CBrush topEdge;
	CBrush bottomEdge;
	CBrush shadowEdge;
	if(controlAttributes.m_colorFace != RGB(0x00, 0x00, 0x00)){
		shadowEdge.CreateSolidBrush(RGB(DARKEN(RED(controlAttributes.m_colorFace)),
										DARKEN(GREEN(controlAttributes.m_colorFace)), 
										DARKEN(BLUE(controlAttributes.m_colorFace))));
	}else{
		shadowEdge.CreateSolidBrush(RGB(0x7F, 0x7F, 0x7F));
	}

	topEdge.CreateSolidBrush(RGB(0x00, 0x00, 0x00));
	if(pContainer && pContainer->controlAttributes.ulMaskGroupPermission) {
		//is there a parent window, and does it have any group permissions
		//if so then we will change the button edge color
		bottomEdge.CreateSolidBrush(RGB(0xB0, 0xB0, 0xFF));
	} else if (controlAttributes.ulMaskGroupPermission != 0) {
		//does this button itself have any group permissions
		//if so then we will change the button edge color
		bottomEdge.CreateSolidBrush(RGB(0xFF, 0xB0, 0xB0));
	} else {
		bottomEdge.CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	}

	if(btnAttributes.m_myShape == ButtonShapeRect){
		pDC->SelectObject(bottomEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
		}
	}else if(btnAttributes.m_myShape == ButtonShapeElipse){
		pDC->SelectObject(bottomEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Ellipse(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->Ellipse(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);
		}
	}else if(btnAttributes.m_myShape == ButtonShapeRoundedRect){
		pDC->SelectObject(bottomEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right, myRect.bottom, 17, 17);
		pDC->SelectObject(topEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(shadowEdge);
		pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(myBrush);
		pDC->RoundRect(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2, 17, 17);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->RoundRect(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2, 17, 17);
		}
	}

//***************** Icon on Button Begin
	int	    cxSource, cySource;

	cxSource = 0;
	cySource = 0;
	if (m_Icon.GetLength() > 0) {
		if (!m_hBitmapFocus) {
			CString iconPath;
			iconPath.Format(ICON_FILES_DIR_FORMAT, m_Icon);
			m_hBitmapFocus = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (m_hBitmapFocus) {
				int iNoBytes = GetObject(m_hBitmapFocus, sizeof(BITMAP), &m_bitmapFocus);
				if (iNoBytes < 1) {
					char xBuff[128];
					sprintf (xBuff, "GetObject() failed. GetLastError = %d", GetLastError ());
					NHPOS_ASSERT_TEXT((iNoBytes > 0), xBuff);
				}
			} else {
				memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
			}
		}

		if (m_hBitmapFocus) {
			BITMAP	bitmap;
			memset (&bitmap, 0, sizeof(bitmap));
			int iNoBytes = GetObject(m_hBitmapFocus, sizeof(BITMAP), &bitmap);
			if (iNoBytes < 1) {
				char xBuff[128];
				sprintf (xBuff, "GetObject() failed. GetLastError = %d", GetLastError ());
				NHPOS_ASSERT_TEXT((iNoBytes > 0), xBuff);
			}
			NHPOS_ASSERT(bitmap.bmWidth == m_bitmapFocus.bmWidth);
			NHPOS_ASSERT(bitmap.bmHeight == m_bitmapFocus.bmHeight);
			cxSource = m_bitmapFocus.bmWidth;
			cySource = m_bitmapFocus.bmHeight;
			//Bitmaps cannot be drawn directly to the screen so a 
			//compatible memory DC is created to draw to, then the image is 
			//transfered to the screen
			CDC hdcMem;
			hdcMem.CreateCompatibleDC(pDC);

			HGDIOBJ  hpOldObject = hdcMem.SelectObject(m_hBitmapFocus);

			int xPos;
			int yPos;

			//The Horizontal and Vertical Alignment
			//For Images
			//Are set in the Layout Manager
			//the proper attribute will have to be checked against
			//for now the Image is centered on the button

			//Horizontal Alignment
			if(btnAttributes.horIconAlignment == IconAlignmentHLeft){//Image to left
				xPos = 2 + 2;
			}else if(btnAttributes.horIconAlignment == IconAlignmentHRight){//Image to right
				xPos = (myRect.right - cxSource - 3);
			}else {//Horizontal center
				xPos = ((myRect.right - cxSource) / 2) + 1;
			}
			
			//Vertical Alignment
			if(btnAttributes.vertIconAlignment == IconAlignmentVTop){//Image to top
				yPos = 2 + 2;
			}else if(btnAttributes.vertIconAlignment == IconAlignmentVBottom){//Image to bottom
				yPos = (myRect.bottom - cySource - 3);
			}else{//Vertical Center
				yPos = ((myRect.bottom - cySource) / 2) + 1;
			}

			pDC->BitBlt(xPos, yPos, cxSource, cySource, &hdcMem, 0, 0, SRCCOPY);

			hdcMem.SelectObject(hpOldObject);
		}
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

	pDC->SetTextColor(controlAttributes.m_colorText);
	CRect tempRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
	CRect textRect(tempRect);
	TCHAR  tempCaption [256];
	ButtonCaptionTransform (tempCaption, myCaption);
	CString csButtonText(tempCaption);
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

	switch(btnAttributes.capAlignment)
	{
	case CaptionAlignmentVBottom:
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);//calculate text area size on button
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
		textRect.top += 2;
		textRect.left += 2;
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
		break;

	case CaptionAlignmentVTop:
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);//calculate text area size on button
		textRect.top = tempRect.top + 2;
		textRect.bottom = tempRect.bottom;
		//the right side is equal to the buttons right side for centering text
		textRect.right = tempRect.right;
		textRect.top += 2;
		textRect.left += 2;
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK);// | DT_BOTTOM | DT_SINGLELINE);
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
			textHeight = pDC->DrawText(tempCaption, textRect, DT_CALCRECT);
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
			if(btnAttributes.capAlignment == CaptionAlignmentHLeft){//LEFT JUSTIFY
				//move the left start point one character width from left side
				textRect.left += textWidth * 5/3;
				textRect.right = textRect.left + textWidth * 5/3;
			}else if(btnAttributes.capAlignment == CaptionAlignmentHRight){//RIGHT JUSTIFY
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
			textRect.top += 2;
			textRect.left += 2;
			for(int i = 0; i < captionLen; i++)
			{
				printText = csButtonText.GetAt(i);
				pDC->DrawText(&printText, 1, textRect, DT_CENTER);
				textRect.top += textHeight / 2 + 2;
			}
			textRect.top -= 2;
			textRect.left -= 2;
		}
		break;

	case CaptionAlignmentVMiddle://vertically center and horizontally center text
		//same as default no break
	default:
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
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
		textRect.top += 2;
		textRect.left += 2;
		pDC->DrawText(tempCaption, textRect, DT_CENTER | DT_WORDBREAK);// | DT_VCENTER | DT_SINGLELINE);
		break;
	}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();
	hatchBrush.DeleteObject();
}

void CFrameworkWndButton::DrawButtonOnDisplayNoFocus (CDC* pDC)
{	
	int iSaveDC = pDC->SaveDC ();

	CPen myPen;
	myPen.CreatePen(PS_NULL, 2, controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowButton::getRectangleSized (0, 0, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;
	CBrush hatchBrush;

	myBrush.CreateSolidBrush(controlAttributes.m_colorFace);

	if(btnAttributes.m_myPattern == ButtonPatternHoriz){
		hatchBrush.CreateHatchBrush(HS_HORIZONTAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)),
				DARKEN(GREEN(controlAttributes.m_colorFace)), 
				DARKEN(BLUE(controlAttributes.m_colorFace))));
	}else if(btnAttributes.m_myPattern == ButtonPatternVert){
		hatchBrush.CreateHatchBrush(HS_VERTICAL, 
			RGB(DARKEN(RED(controlAttributes.m_colorFace)),
				DARKEN(GREEN(controlAttributes.m_colorFace)),
				DARKEN(BLUE(controlAttributes.m_colorFace))));
	}

	CBrush* pOldBrush = pDC->SelectObject(&myBrush);
	CBrush topEdge;
	CBrush bottomEdge;
	CBrush shadowEdge;
	if(controlAttributes.m_colorFace != RGB(0x00, 0x00, 0x00)){
		shadowEdge.CreateSolidBrush(RGB(DARKEN(RED(controlAttributes.m_colorFace)),
										DARKEN(GREEN(controlAttributes.m_colorFace)), 
										DARKEN(BLUE(controlAttributes.m_colorFace))));
	}else{
		shadowEdge.CreateSolidBrush(RGB(0x7F, 0x7F, 0x7F));
	}

	if(pContainer && pContainer->controlAttributes.ulMaskGroupPermission) {
		//is there a parent window, and does it have any group permissions
		//if so then we will change the button edge color
		topEdge.CreateSolidBrush(RGB(0xB0, 0xB0, 0xFF));
	} else if (controlAttributes.ulMaskGroupPermission != 0) {
		//does this button itself have any group permissions
		//if so then we will change the button edge color
		topEdge.CreateSolidBrush(RGB(0xFF, 0xB0, 0xB0));
	} else {
		topEdge.CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	}
	bottomEdge.CreateSolidBrush(RGB(0x00, 0x00, 0x00));

	if(btnAttributes.m_myShape == ButtonShapeRect){
		pDC->SelectObject(bottomEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
		}
	}else if(btnAttributes.m_myShape == ButtonShapeElipse){
		pDC->SelectObject(bottomEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Ellipse(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->Ellipse(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
		}
	}else if(btnAttributes.m_myShape == ButtonShapeRoundedRect){
		pDC->SelectObject(bottomEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right, myRect.bottom, 17, 17);
		pDC->SelectObject(topEdge);
		pDC->RoundRect(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(shadowEdge);
		pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2, 17, 17);
		pDC->SelectObject(myBrush);
		pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4, 17, 17);
		if(btnAttributes.m_myPattern != ButtonPatternNone){
			pDC->SelectObject(hatchBrush);
			pDC->RoundRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4, 17, 17);
		}
	}

//***************** Icon on Button Begin
	int	    cxSource, cySource;

	cxSource = 0;
	cySource = 0;
	if (m_Icon.GetLength() > 0) {
		// We use the same icon for both Focus and No Focus so we
		// will just use the same handle for both.  In this case
		// we have arbitrarily decided to use m_hBitmapFocus.
		if (!m_hBitmapFocus) {
			CString iconPath;
			iconPath.Format(ICON_FILES_DIR_FORMAT, m_Icon);
			m_hBitmapFocus = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (m_hBitmapFocus) {
				int iNoBytes = GetObject(m_hBitmapFocus, sizeof(BITMAP), &m_bitmapFocus);
				if (iNoBytes < 1) {
					char xBuff[128];
					sprintf (xBuff, "GetObject() failed. GetLastError = %d", GetLastError ());
					NHPOS_ASSERT_TEXT((iNoBytes > 0), xBuff);
				}
			} else {
				memset (&m_bitmapFocus, 0, sizeof(m_bitmapFocus));
			}
		}
		if (m_hBitmapFocus) {
			BITMAP	bitmap;
			memset (&bitmap, 0, sizeof(bitmap));
			int iNoBytes = GetObject(m_hBitmapFocus, sizeof(BITMAP), &bitmap);
			if (iNoBytes < 1) {
				char xBuff[128];
				sprintf (xBuff, "GetObject() failed. GetLastError = %d", GetLastError ());
				NHPOS_ASSERT_TEXT((iNoBytes > 0), xBuff);
			}
			NHPOS_ASSERT(bitmap.bmWidth == m_bitmapFocus.bmWidth);
			NHPOS_ASSERT(bitmap.bmHeight == m_bitmapFocus.bmHeight);
			cxSource = m_bitmapFocus.bmWidth;
			cySource = m_bitmapFocus.bmHeight;
			//Bitmaps cannot be drawn directly to the screen so a 
			//compatible memory DC is created to draw to, then the image is 
			//transfered to the screen
			CDC hdcMem;
			hdcMem.CreateCompatibleDC(pDC);

			HGDIOBJ  hpOldObject = hdcMem.SelectObject(m_hBitmapFocus);

			int xPos;
			int yPos;

			//The Horizontal and Vertical Alignment
			//For Images
			//Are set in the Layout Manager
			//the proper attribute will have to be checked against
			//for now the Image is centered on the button

			//Horizontal Alignment
			if(btnAttributes.horIconAlignment == IconAlignmentHLeft){//Image to left
				xPos = 2;
			}else if(btnAttributes.horIconAlignment == IconAlignmentHRight){//Image to right
				xPos = myRect.right - cxSource - 5;
			}else {//Horizontal center
				xPos = ((myRect.right - cxSource) / 2) - 1;
			}
			
			//Vertical Alignment
			if(btnAttributes.vertIconAlignment == IconAlignmentVTop){//Image to top
				yPos = 2;
			}else if(btnAttributes.vertIconAlignment == IconAlignmentVBottom){//Image to bottom
				yPos = myRect.bottom - cySource - 5;
			}else{//Vertical Center
				yPos = ((myRect.bottom - cySource) / 2) - 1;
			}

			pDC->BitBlt(xPos, yPos, cxSource, cySource, &hdcMem, 0, 0, SRCCOPY);

			hdcMem.SelectObject(hpOldObject);
		}
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

	pDC->SetTextColor(controlAttributes.m_colorText);
	CRect tempRect(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);
	CRect textRect(tempRect);
	TCHAR  tempCaption [256];
	ButtonCaptionTransform (tempCaption, myCaption);
	CString csButtonText(tempCaption);
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

	switch(btnAttributes.capAlignment)
	{
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
			if(btnAttributes.capAlignment == CaptionAlignmentHLeft){//LEFT JUSTIFY
				//move the left start point one character width from left side
				textRect.left += textWidth * 5/3;
				textRect.right = textRect.left + textWidth * 5/3;
			}else if(btnAttributes.capAlignment == CaptionAlignmentHRight){//RIGHT JUSTIFY
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
		pDC->DrawText(csButtonText, textRect, DT_CENTER | DT_WORDBREAK);// | DT_VCENTER | DT_SINGLELINE);
		break;
	}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();
	hatchBrush.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFrameworkWndButton, CWindowButton)
	//{{AFX_MSG_MAP(CFrameworkWndButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndButton message handlers

// The following struct is used to build tables that will enable the
// translation of touchscreen button press data into keypress
typedef struct {
	UCHAR   uchASCII;
	USHORT  uchScan;
} ButtonToScanTable;

static ButtonToScanTable  ButtonScan_PrintMod [] =   // CWindowButton::BATypePrintMod
{
	{    0, 0},
	{ 0x6C, 0},
	{ 0x6D, 0},
	{ 0x6E, 0},
	{ 0x6F, 0},
	{ 0x70, 0},
	{ 0x71, 0},
	{ 0x72, 0},
	{ 0x73, 0}
};
		
static ButtonToScanTable ButtonScan_Adj [] =   // CWindowButton::BATypeAdj
{
	{    0, 0},
	{ 0x74, 0},
	{ 0x75, 0},
	{ 0x76, 0},
	{ 0x77, 0},
	{ 0x78, 0},
	{ 0x79, 0},
	{ 0x7A, 0},
	{ 0x7B, 0},
	{ 0x7C, 0},
	{ 0x7D, 0},
	{ 0x7E, 0},
	{ 0x7F, 0},
	{ 0x80, 0},
	{ 0x81, 0},
	{ 0x82, 0},
	{ 0x83, 0},
	{ 0x84, 0},
	{ 0x85, 0},
	{ 0x86, 0},
	{ 0x87, 0}
};

static ButtonToScanTable  ButtonScan_Tender [] =    // CWindowButton::BATypeTender
{
	{    0, 0},
	{ 0x88, 0},
	{ 0x89, 0},
	{ 0x8A, 0},
	{ 0x8B, 0},
	{ 0x8C, 0},
	{ 0x8D, 0},
	{ 0x8E, 0},
	{ 0x8F, 0},
	{ 0x90, 0},
	{ 0xED, 0},
	{ 0xEE, 0},
	{ 0xF2, 0},
	{ 0xF3, 0},
	{ 0xF4, 0},
	{ 0xF5, 0},
	{ 0xF6, 0},
	{ 0xF7, 0},
	{ 0xF8, 0},
	{ 0xF9, 0},
	{ 0xFA, 0}
};


static ButtonToScanTable  ButtonScan_ForeignTender [] =   // CWindowButton::BATypeForeignTender
{
	{    0, 0},
	{ 0x6B, 0},
	{ 0xE6, 0},
	{ 0xE7, 0},
	{ 0xE8, 0},
	{ 0xE9, 0},
	{ 0xEA, 0},
	{ 0xEB, 0},
	{ 0xEC, 0}
};

static ButtonToScanTable  ButtonScan_Total [] =    // CWindowButton::BATypeTotal
{
	{    0, 0},
	{ 0x91, 0},
	{ 0x92, 0},
	{ 0x93, 0},
	{ 0x94, 0},
	{ 0x95, 0},
	{ 0x96, 0},
	{ 0x97, 0},
	{ 0x98, 0},
	{ 0x99, 0},
	{ 0xFB, 0},
	{ 0xFC, 0},
	{ 0xFD, 0},
	{ 0xFE, 0},
	{ 0xFF, 0},
	{ 0xFF, 0x01},
	{ 0xFF, 0x02},
	{ 0xFF, 0x03},
	{ 0xFF, 0x04},
	{ 0xFF, 0x05},
	{ 0xFF, 0x06}
};

static ButtonToScanTable ButtonScan_AddCheck [] =   // CWindowButton::BATypeAddCheck
{
	{    0, 0},
	{ 0x9A, 0},
	{ 0x9B, 0},
	{ 0x9C, 0}
};

static ButtonToScanTable ButtonScan_ItemDisc [] =   // CWindowButton::BATypeItemDisc
{
	{    0, 0},
	{ 0x9D, 0},
	{ 0x9E, 0},
	{ 0xFF, 0x08},
	{ 0xFF, 0x09},
	{ 0xFF, 0x0A},
	{ 0xFF, 0x0B}
};

static ButtonToScanTable ButtonScan_RegDisc [] =   // CWindowButton::BATypeRegDisc
{
	{    0, 0},
	{ 0x9F, 0},
	{ 0xA0, 0},
	{ 0xA1, 0},
	{ 0xA2, 0},
	{ 0xA3, 0},
	{ 0xA4, 0}
};

static ButtonToScanTable ButtonScan_TaxMod [] =   // CWindowButton::BATypeTaxMod
{
	{    0, 0},
	{ 0xA5, 0 },
	{ 0xA6, 0 },
	{ 0xA7, 0 },
	{ 0xA8, 0 },
	{ 0xA9, 0 },
	{ 0xAA, 0 },
	{ 0xAB, 0 },
	{ 0xAC, 0 },
	{ 0xAD, 0 },
	{ 0xAE, 0 },
	{ 0xAF, 0 }
};

static ButtonToScanTable ButtonScan_SignIn [] =   // CWindowButton::BATypeSignIn
{
	{ 0xB0, 0},      // Operator Sign-in Drawer A,  FSC_SIGN_IN 0
	{ 0xB1, 0}       // Operator Sign-in Drawer B,  FSC_B 0
};

static ButtonToScanTable ButtonScan_Cancel [] =   // CWindowButton::BATypeCancel
{
	{ 0xBD, 0},     // Transaction Cancel:  FSC_CANCEL 0
	{ 0xFF, 0x30}   // Not Transaction Cancel: FSC_CANCEL 1
};

static ButtonToScanTable ButtonScan_TranVoid [] =   // CWindowButton::BATypeTranVoid
{
	{ 0xC0, 0},     // Transaction Void:  FSC_PRE_VOID 0
	{ 0xFF, 0x2A},  // Transaction Return: FSC_PRE_VOID 1
	{ 0xFF, 0x2E},  // Transaction Return: FSC_PRE_VOID 2
	{ 0xFF, 0x2F}   // Transaction Return: FSC_PRE_VOID 3
};

static ButtonToScanTable ButtonScan_PrtDemand [] =   // CWindowButton::BATypePrtDemand
{
	{    0, 0},
	{ 0xC3, 0},   // Print on Demand: FSC_PRT_DEMAND 1
	{ 0xC4, 0},   // Print on Demand: FSC_PRT_DEMAND 2
	{ 0xC5, 0},   // Print on Demand: FSC_PRT_DEMAND 3
	{ 0xFF, 0x07}, // Print on Demand:  FSC_PRT_DEMAND 4    SR 772
	{ 0xFF, 0x31} // Print on Demand:  FSC_PRT_DEMAND 5    SR 772
};

static ButtonToScanTable ButtonScan_ChrgTips [] =   // CWindowButton::BATypeChrgTips
{
	{    0, 0},
	{ 0xC6, 0},
	{ 0xC7, 0},
	{ 0xC8, 0}
};

static ButtonToScanTable 	ButtonScan_PreCash [] =    // CWindowButton::BATypePreCash, FSC_PRESET_AMT
{
	{ 0x00, 0x00},
	{ 0xCF, 0x00},    // Preset Ammount: FSC_PRESET_AMT 1
	{ 0xD0, 0x00},    // Preset Ammount: FSC_PRESET_AMT 2
	{ 0xD1, 0x00},    // Preset Ammount: FSC_PRESET_AMT 3
	{ 0xD2, 0x00},    // Preset Ammount: FSC_PRESET_AMT 4
	{ 0xFF, 0x32},    //FF32 Preset Ammount: FSC_PRESET_AMT 5
	{ 0xFF, 0x33},    //FF33 Preset Ammount: FSC_PRESET_AMT 6
	{ 0xFF, 0x34},    //FF34 Preset Ammount: FSC_PRESET_AMT 7
	{ 0xFF, 0x35}     //FF35 Preset Ammount: FSC_PRESET_AMT 8
};

static ButtonToScanTable  ButtonScan_Ask [] =    // CWindowButton::BATypeAsk
{
	{    0, 0},
	{ 0x4E, 0},
	{ 0x4F, 0},
	{ 0x50, 0},
	{ 0x51, 0},
	{ 0xDE, 0},
	{ 0xDF, 0},
	{ 0x52, 0},
	{ 0x53, 0},
	{ 0x54, 0},
	{ 0x55, 0},
	{ 0x56, 0},
	{ 0x57, 0},
	{ 0xE0, 0},
	{ 0xE1, 0},
	{ 0x58, 0},
	{ 0x59, 0},
	{ 0x5A, 0},
	{ 0x5B, 0},
	{ 0x5C, 0},
	{ 0x5D, 0}
};

static ButtonToScanTable ButtonScan_LevelSet [] =   // CWindowButton::BATypeLevelSet
{
	{    0, 0},
	{ 0xE2, 0},
	{ 0xE3, 0},
	{ 0x5E, 0},
	{ 0x5F, 0},
	{ 0x60, 0}
};

static ButtonToScanTable 	ButtonScan_NumPad [] =    // CWindowButton::BATypeNumPad
{
	{ 0x30, 0},
	{ 0x31, 0},
	{ 0x32, 0},
	{ 0x33, 0},
	{ 0x34, 0}, 
	{ 0x35, 0},
	{ 0x36, 0},
	{ 0x37, 0},
	{ 0x38, 0},
	{ 0x39, 0}
};

static ButtonToScanTable ButtonScan_OrderDec[] =	// CWindowButton::BATypeOrderDecalaration
{
	{0, 0},
	{0xFF, 0x18},
	{0xFF, 0x19},
	{0xFF, 0x1A},
	{0xFF, 0x1B},
	{0xFF, 0x1C}
};

static ButtonToScanTable ButtonScan_GiftCard[] =	// CWindowButton::BATypeGiftCard
{
	{0, 0},
	{0xFF, 0x1E},    // FSC_GIFT_CARD, 96, 1  CLASS_UIBALANCE , IS_GIFT_CARD_GENERIC
	{0xFF, 0x1F},    // FSC_GIFT_CARD, 96, 2  CLASS_FREEDOMPAY, IS_GIFT_CARD_FREEDOMPAY
	{0xFF, 0x20}     // FSC_GIFT_CARD, 96, 3  CLASS_EPAYMENT, IS_GIFT_CARD_EPAYMENT
};

static ButtonToScanTable ButtonScan_CursorVoid[] =	// CWindowButton::BATypeCursorVoid
{
	{0x49, 0x00},    // FSC_CURSOR_VOID, 74, 0
	{0xFF, 0x2B},    // FSC_CURSOR_VOID, 74, 1  CLASS_UICURSORTRETURN1
	{0xFF, 0x2C},    // FSC_CURSOR_VOID, 74, 2  CLASS_UICURSORTRETURN2
	{0xFF, 0x2D}     // FSC_CURSOR_VOID, 74, 3  CLASS_UICURSORTRETURN3
};


void CFrameworkWndButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	SetFocus();
	Invalidate(FALSE);
}

static CHARDATA HandlePluButtonPress (UCHAR *aszPluNumber)
{
	CHARDATA        CharData;
	UCHAR			uchPresetPLU;

	uchPresetPLU = BlFwIfSetPresetPLUNumber(aszPluNumber);
	CharData.uchFlags = 0;
	CharData.uchASCII = 0xFF;//0xC9;//0x6B;
	CharData.uchScan = 0x00;

	// convert the preset index into the scan code that
	// applies to the slot in the circular buffer within the
	// Pararam.ParaPLUNoMenu[] area that corresponds to the index value.
	if (uchPresetPLU >= 1 && uchPresetPLU <= PLU_MAX_TOUCH_PRESET)
	{
		//The hardcoded 0x0C corresponds to the 
		//location of the Keyed PLU buttons
		//in CVTREGTOUCH.C held in the UIFSUP
		// subsystem.   This also needs to correspond with
		// the same area in the FSC tables that are hardcode
		// in menu 10 in uniniram.c
		CharData.uchScan = 0x0C + uchPresetPLU;
	}

	ASSERT(CharData.uchScan);
	return CharData;
}

static void FrameworkNoPermissionsDialog (void)
{
	AfxMessageBox (_T("Action blocked by Permissions."), MB_OK);
}

// Process a button press when the button up message is received.
//
void CFrameworkWndButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetTopLevelParent()->SetFocus();
	Invalidate(FALSE); //Redraw the button to be raised
	
	CWindowButton::OnLButtonUp(nFlags, point);
	CHARDATA        CharData = {0};

	PifBeep(50);

	{
		CASDATA_RTRV casdata;
		BlFwIfGetCasData(&casdata);

		//check the group permissions to see if the user is allowed to use this control

		//is there a parent window, and does it have any group permissions
		if(pContainer && pContainer->controlAttributes.ulMaskGroupPermission)
			//does the currently signed in user have permission to use this window
			if(!(pContainer->controlAttributes.ulMaskGroupPermission & casdata.ulGroupAssociations))
				//if not, is this a dismiss button
				if(btnAttributes.m_myActionUnion.type != BATypeWinDis) {
					FrameworkNoPermissionsDialog();
					return;	//disallow if it is not a dismiss button
				}
		//does this control have any group permissions set
		if(controlAttributes.ulMaskGroupPermission)
			//does the currently signed in user have permission to use this button
			if(!(controlAttributes.ulMaskGroupPermission & casdata.ulGroupAssociations)) {
				FrameworkNoPermissionsDialog();
				return;	//disallow if the user does not have permission
			}
		
		//if this point is reached, then group permission checks have been passed
	}

	// Check whether we are restricted to one or more terminal modes (SUPR, PROG, REG).

	// First check is to see if the container for this button has restrictions.
	// If so, then lets check to see if the restriction matches our current terminal mode.
	// If the terminal mode required matches the current terminal mode then we go ahead
	// and do the action.  Otherwise we will just return and do nothing.
	if (pContainer && btnAttributes.m_myActionUnion.type != CWindowButton::BATypeWinDis) {
		BOOL  bLockWindow = FALSE;

		if (pContainer->controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SPR_MASK) {
			bLockWindow = TRUE;
			if (pContainer->controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SUPR) {
				if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_SUP) {
					bLockWindow = FALSE;
				}
				else {
					// Lets request Supervisor Intervention to finish popping up this window.
					if (BLI_SUPR_INTV_APPROVED == BlUifSupIntervent(BLI_SUPR_INTV_READ)) {
						bLockWindow = FALSE;
					}
				}
			}
			if (pContainer->controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_PROG) {
				if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_PROG) {
					bLockWindow = FALSE;
				}
			}
			if (pContainer->controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_REG) {
				if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_REG) {
					bLockWindow = FALSE;
				}
			}
		}
		if (bLockWindow) {
			FrameworkNoPermissionsDialog();
			return;
		}
	}

	// We have passed the first restriction check, namely window container level restrictions.
	// The next check is on the actual button to see its restrictions.
	if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SPR_MASK) {
		BOOL  bLockWindow = TRUE;
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SUPR) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_SUP) {
				bLockWindow = FALSE;
			}
			else {
				// Lets request Supervisor Intervention to finish popping up this window.
				if (BLI_SUPR_INTV_APPROVED == BlUifSupIntervent(BLI_SUPR_INTV_READ)) {
					bLockWindow = FALSE;
				}
			}
		}
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_PROG) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_PROG) {
				bLockWindow = FALSE;
			}
		}
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_REG) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_REG) {
				bLockWindow = FALSE;
			}
		}
		if (bLockWindow) {
			FrameworkNoPermissionsDialog();
			return;
		}
	}

	CWindowControl *pMyParent = (CWindowControl *)CWindowControl::GetParent();

	{
		// process special window attributes for Connection Engine Interface, Scripting, and others.
		if (controlAttributes.SpecWin[SpecWinIndex_ConnEngine] & SpecWinEventConnEngine_Indic) {
			TCHAR  tcsDataBuffer[20] = {0};
			int    iLoop = 0;

			for (iLoop = 0; iLoop < 14; iLoop++) {
				tcsDataBuffer[iLoop] = btnAttributes.m_myActionUnion.data.PLU[iLoop];
			}

			switch(btnAttributes.m_myActionUnion.type) {
				case CWindowButton::BATypeACKey:
					BlConnEngineSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("ACKey"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				case CWindowButton::BATypeDisplayLabeledWindow:
					BlConnEngineSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("DisplayLabeledWindow"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				case CWindowButton::BATypeDismissLabeledWindow:
					BlConnEngineSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("DismissLabeledWindow"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				default:
					// This is not a type of button we want to use with Connection Engine so just ignore the button press.
					break;;
			}
		}

		if (controlAttributes.SpecWin[SpecWinIndex_ConnEngine] & SpecWinEventScript_Indic) {
			TCHAR  tcsDataBuffer[20] = {0};
			int    iLoop = 0;

			for (iLoop = 0; iLoop < 14; iLoop++) {
				tcsDataBuffer[iLoop] = btnAttributes.m_myActionUnion.data.PLU[iLoop];
			}

			switch(btnAttributes.m_myActionUnion.type) {
				case CWindowButton::BATypeACKey:
					BlScriptSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("ACKey"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				case CWindowButton::BATypeDisplayLabeledWindow:
					BlScriptSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("DisplayLabeledWindow"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				case CWindowButton::BATypeDismissLabeledWindow:
					BlScriptSendButtonPress (pMyParent->controlAttributes.m_myName, this->controlAttributes.m_myName, _T("DismissLabeledWindow"), btnAttributes.m_myActionUnion.data.extFSC, tcsDataBuffer);
					break;

				default:
					// This is not a type of button we want to use with Scripts so just ignore the button press.
					break;;
			}
		}

		if (controlAttributes.SpecWin[SpecWinIndex_ConnEngine] & SpecWinEventConnEngine_Indic) {
			return;
		}
	}

	// Check for any button processing restrictions that may apply to this button.
	// Button processing restrictions are used to limit allowed button presses when in a particular
	// mode because allowing the button would cause user interface problems.  This is not to be
	// use lightly but is intended for specific circumstances such as when processing an OEP
	// window and we want to make sure the user can not cover the OEP window with another window
	// rendering the window unavailable.
	ULONG  ulButtonRestrictions = BlRestrictButtonPressMask (BL_BUTTONRESTRICT_OP_READEXISTING, 0);
	if (ulButtonRestrictions & BL_BUTTONRESTRICT_OEP_ONLY) {
		switch (btnAttributes.m_myActionUnion.type) {
			case CWindowButton::BATypeOEPEntry:
				break;
			case CWindowButton::BATypeCancel:
				break;
			default:
				// this is not an allowed OEP window entry so ignore this key press as we are in restricted mode.
				return;
		}
	}

	CharData.uchFlags = CHARDATA_FLAGS_TOUCHUTTON;
	switch(btnAttributes.m_myActionUnion.type){
	case CWindowButton::BATypePLUNum:
		CharData.uchASCII = 0xC9;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePLU:
		CharData = HandlePluButtonPress (btnAttributes.m_myActionUnion.data.PLU);
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDept:
		//if a number is pressed before this button
		//Price Enter will need to come before the 
		//Department data
		//the PRICE GUARD will translate to 
		//the PRICE ENTER fsc if needed
		CharData.uchASCII = FSC_PRICE_GUARD_ASCII;
		CharData.uchScan = FSC_PRICE_GUARD_SCAN;
		sendButtonMessageToBL(&CharData);
		//Need to send ExtFSC information
		sendNumericData(btnAttributes.m_myActionUnion.data.PLU);
		CharData.uchASCII = 0x61;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDeptNum:
		CharData.uchASCII = 0x61;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePrintMod:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 && 
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_PrintMod)/sizeof(ButtonScan_PrintMod[0]))
		{
			CharData.uchASCII = ButtonScan_PrintMod[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_PrintMod[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeAdj:
		//if a number is pressed before this button
		//Quantity will need to come before the 
		//Adjective data
		//the QTY GUARD will translate to 
		//the QTY fsc if needed
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 && 
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_Adj)/sizeof(ButtonScan_Adj[0]))
		{
			CharData.uchASCII = FSC_QTY_GUARD_ASCII;
			CharData.uchScan = FSC_QTY_GUARD_SCAN;
			sendButtonMessageToBL(&CharData);

			CharData.uchASCII = ButtonScan_Adj[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_Adj[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeTender:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 && 
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_Tender)/sizeof(ButtonScan_Tender[0]))
		{
			CharData.uchASCII = ButtonScan_Tender[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_Tender[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeForeignTender:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_ForeignTender)/sizeof(ButtonScan_ForeignTender[0]))
		{
			CharData.uchASCII = ButtonScan_ForeignTender[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_ForeignTender[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeTotal:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_Total)/sizeof(ButtonScan_Total[0]))
		{
			CharData.uchASCII = ButtonScan_Total[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_Total[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeAddCheck:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_AddCheck)/sizeof(ButtonScan_AddCheck[0]))
		{
			CharData.uchASCII = ButtonScan_AddCheck[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_AddCheck[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeItemDisc:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_ItemDisc)/sizeof(ButtonScan_ItemDisc[0]))
		{
			CharData.uchASCII = ButtonScan_ItemDisc[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_ItemDisc[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeRegDisc:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_RegDisc)/sizeof(ButtonScan_RegDisc[0]))
		{
			CharData.uchASCII = ButtonScan_RegDisc[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_RegDisc[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeTaxMod:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_TaxMod)/sizeof(ButtonScan_TaxMod[0]))
		{
			CharData.uchASCII = ButtonScan_TaxMod[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_TaxMod[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeSignIn:
		if (btnAttributes.m_myActionUnion.data.extFSC >= 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_SignIn)/sizeof(ButtonScan_SignIn[0]))
		{
			CharData.uchASCII = ButtonScan_SignIn[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_SignIn[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeNewCheck:
		CharData.uchASCII = 0xB2;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePreBal:
		CharData.uchASCII = 0xB3;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeGCNum:
		CharData.uchASCII = 0xB4;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeGCTrans:
		CharData.uchASCII = 0xB5;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeNumPerson:
		CharData.uchASCII = 0xB6;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeTableNum:
		CharData.uchASCII = 0xB7;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeLineNum:
		CharData.uchASCII = 0xB8;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeFeed:
		CharData.uchASCII = 0xB9;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeReceipt:
		CharData.uchASCII = 0xBA;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeTipPO:
		CharData.uchASCII = 0xBB;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDecTips:
		CharData.uchASCII = 0xBC;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeNoSale:
		CharData.uchASCII = 0x65;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePO:
		CharData.uchASCII = 0x66;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeROA:
		CharData.uchASCII = 0x69;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeChgCmpt:
		CharData.uchASCII = 0x6A;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeCancel:
		if (btnAttributes.m_myActionUnion.data.extFSC >= 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_Cancel)/sizeof(ButtonScan_Cancel[0]))
		{
			CharData.uchASCII = ButtonScan_Cancel[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_Cancel[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeVoid:
		CharData.uchASCII = 0xBE;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeErCorrect:
		CharData.uchASCII = 0xBF;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeTranVoid:
		if (btnAttributes.m_myActionUnion.data.extFSC >= 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_TranVoid)/sizeof(ButtonScan_TranVoid[0]))
		{
			CharData.uchASCII = ButtonScan_TranVoid[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_TranVoid[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeQty:
		CharData.uchASCII = 0xC1;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeShift:
		//  Menu shift should not be used with touchscreen since touchscreen requires using a
		//  specific menu page that is tailored for touchscreen and buttons.
		//      Richard Chambers, Mar-21-2019
		CharData.uchASCII = 0x6B;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeNumTypeCaption:
		{
			USHORT  usKBMode_Prev;
			//We change the conversion table to use the 
			//alpha conversion table before we enter the key press
			//US Customs SCER for Rel 2.X JHHJ
			usKBMode_Prev = BlFwIfUifACChgKBType(UIF_ALPHA_KB);
			for(int t = 0; t < myCaption.GetLength(); t++){
				GetTopLevelParent()->SendMessage(WM_CHAR, (WPARAM)myCaption.GetAt(t), (LPARAM)0);
			}
			BlFwIfUifACChgKBType(usKBMode_Prev);
		}
		// drop through to add the #/Type to the end of the string.
	case CWindowButton::BATypeNumType:
		CharData.uchASCII = 0xC2;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePrtDemand:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_PrtDemand)/sizeof(ButtonScan_PrtDemand[0]))
		{
			CharData.uchASCII = ButtonScan_PrtDemand[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_PrtDemand[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeChrgTips:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_ChrgTips)/sizeof(ButtonScan_ChrgTips[0]))
		{
			CharData.uchASCII = ButtonScan_ChrgTips[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_ChrgTips[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeDblZero:
		CharData.uchASCII = 0x67;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeACKey:
		CharData.uchASCII = 0xCA;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		{
			// Pause for just a moment giving up our thread's time slice in order to allow
			// the BusinessLogic functionality to process the AC Key event and set the 
			// correct status for the function BlUifSupIntervent() or other status check.
			Sleep (50);
			CFrameworkWndItemAdHoc * pWinItem = CFrameworkWndItemAdHoc::TranslateCWindowControl (pMyParent);
			if (pWinItem) {
				pWinItem->PostMessage (WU_EVS_CREATE, 0, CWindowButton::BATypeACKey);
			}
		}
		break;

	case CWindowButton::BATypeRFeed:
		CharData.uchASCII = 0xCB;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeJFeed:
		CharData.uchASCII = 0xCC;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDecimal:
		CharData.uchASCII = 0x68;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeScaleTare:
		CharData.uchASCII = 0xCD;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeRevPrint:
		CharData.uchASCII = 0xCE;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePreCash:                          // FSC_PRESET_AMT
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_PreCash)/sizeof(ButtonScan_PreCash[0]))
		{
			CharData.uchASCII = ButtonScan_PreCash[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_PreCash[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	/*case CWindowButton::BATypeDrcShift:
		CharData.uchASCII = 0xD2;CharData.uchScan = 0x00;
		break;*/

	case CWindowButton::BATypeOffTend:
		CharData.uchASCII = 0xD3;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeTimeIn:
		CharData.uchASCII = 0xD4;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeTimeOut:
		CharData.uchASCII = 0xD5;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeCouponNum:
		CharData.uchASCII = 0xD6;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeCoupon:
		{
			//Need to send ExtFSC information
			char	uchPage[3];
			_itoa(btnAttributes.m_myActionUnion.data.PLU[0], &uchPage[0], 10);
			sendNumericData((UCHAR*)&uchPage[0]);
		}
		CharData.uchASCII = 0xD6;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeScrollUp:
		CharData.uchASCII = 0xD7;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeScrollDown:
		CharData.uchASCII = 0xD8;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeScrollLeft:
		CharData.uchASCII = 0xD9;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeScrollRight:
		CharData.uchASCII = 0xDA;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeWait:
		CharData.uchASCII = 0xDB;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePaidOrder:
		CharData.uchASCII = 0x40;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeStringNum:
		CharData.uchASCII = 0x41;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeAdjectiveMod:
		CharData.uchASCII	= 0xFF;
		CharData.uchScan	= 0x1D;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeString:
		//if a number is pressed before this button
		//we will need to clear out the ring buffer
		//so that no information is sent prior to this
		//
		CharData.uchASCII = FSC_CNT_STRING_GUARD_ASCII;
		CharData.uchScan = FSC_CNT_STRING_GUARD_SCAN;
		sendButtonMessageToBL(&CharData);
		{
			char	uchPage[16];
			_itoa(btnAttributes.m_myActionUnion.data.extFSC, &uchPage[0], 10);
			sendNumericData((UCHAR*)&uchPage[0]);
			CharData.uchASCII = 0x41;
			CharData.uchScan = 0x00;
			sendButtonMessageToBL(&CharData);
		}
		break;

	case CWindowButton::BATypeAlpha:
		CharData.uchASCII = 0x42;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeLeftDisp:
		CharData.uchASCII = 0x44;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeSurrogateNum:
		CharData.uchASCII = 0x45;CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeSplit:
		CharData.uchASCII = 0x46;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeSeatNum:
		switch(btnAttributes.m_myActionUnion.data.extFSC){
		case 1:
			CharData.uchASCII = 0x47;
			CharData.uchScan = 0x00;
			break;
		case 2:
			CharData.uchASCII = 0x48;
			CharData.uchScan = 0x00;
			break;
		}
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeItemTrans:
		CharData.uchASCII = 0xDC;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeRepeat:
		CharData.uchASCII = 0xDD;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeCursorVoid:         // FSC_CURSOR_VOID
		if (btnAttributes.m_myActionUnion.data.extFSC >= 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_CursorVoid)/sizeof(ButtonScan_CursorVoid[0]))
		{
			CharData.uchASCII = ButtonScan_CursorVoid[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_CursorVoid[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;
		break;

	case CWindowButton::BATypePriceCheck:
		CharData.uchASCII = 0x4A;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeOprInt:
		switch(btnAttributes.m_myActionUnion.data.extFSC){
		case 0:
			CharData.uchASCII = 0x4B;
			CharData.uchScan = 0x00;
			break;
		case 1:
			CharData.uchASCII = 0x4C;
			CharData.uchScan = 0x00;
			break;
		}
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeMoney:
		CharData.uchASCII = 0x4D;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeAsk:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_Ask)/sizeof(ButtonScan_Ask[0]))
		{
			CharData.uchASCII = ButtonScan_Ask[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_Ask[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeLevelSet:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_LevelSet)/sizeof(ButtonScan_LevelSet[0]))
		{
			CharData.uchASCII = ButtonScan_LevelSet[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_LevelSet[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeUPCE:
		CharData.uchASCII = 0x62;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypePriceEnt:
		CharData.uchASCII = 0x63;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeFSMod:
		CharData.uchASCII = 0xE4;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeWicTrans:
		CharData.uchASCII = 0xE5;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeWicMod:
		CharData.uchASCII = 0x64;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeClear:
		CharData.uchFlags = 0;
		CharData.uchASCII = 0x43;
		CharData.uchScan = 0x00;
		sendButtonMessageToBL(&CharData);
		{
			CFrameworkWndItemAdHoc * pWinItem = CFrameworkWndItemAdHoc::TranslateCWindowControl (pMyParent);
			if (pWinItem) {
				delete pWinItem;
				pMyParent = 0;
				TRACE2("%S(%d): CFrameworkWndButton::OnLButtonUp() BATypeClear  \n", __FILE__, __LINE__);
			}
		}
		break;

	case CWindowButton::BATypeNumPad:
		if (btnAttributes.m_myActionUnion.data.extFSC >= 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_NumPad)/sizeof(ButtonScan_NumPad[0]))
		{
			CharData.uchASCII = ButtonScan_NumPad[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan = ButtonScan_NumPad[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}
		else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeModeKey:
		CharData.uchFlags = 0;
		CharData.uchASCII = 0;CharData.uchScan = 0;

		switch(btnAttributes.m_myActionUnion.data.extFSC){
		case 0: //Lock Mode
			CharData.uchASCII = 0;
			CharData.uchScan = 0x03;
			break;
		case 1: //Register Mode
			CharData.uchASCII = 0;
			CharData.uchScan = 0x04;
			break;
		case 2: //Supervisor Mode
			CharData.uchASCII = 0;
			CharData.uchScan = 0x05;
			break;
		case 3: //Program Mode
			CharData.uchASCII = 0;
			CharData.uchScan = 0x06;
			break;
		}
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeWindow:
		{
			BOOL bRet = CWindowDocument::pDoc->ToggleWindow (this, btnAttributes.m_myActionUnion.data.nWinID);

			NHPOS_ASSERT_TEXT("==PROVISIONING" && bRet, "==PROVISIONING: BATypeWindow - Requested window id not found in Layout File.");

		}
		break;

		// both of these button actions use the same code just with different parameters
	case CWindowButton::BATypePLU_Group:
	case CWindowButton::BATypePLU_GroupTable:
		{
			CFrameworkWndItem* pWndItem = (CFrameworkWndItem*)CWindowButton::GetParent();
			UCHAR  aszCaption[64];

			// Since we are doing a PLU group, lets see if there is a PLU number
			// directly associated with this button.  If so, then we will just
			// use that PLU number.
			if (auchPluNo[0]) {
				if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_REG &&
					uchPluAdjMod > 0 && 
					uchPluAdjMod < sizeof(ButtonScan_Adj)/sizeof(ButtonScan_Adj[0]))
				{
					CharData.uchASCII = FSC_QTY_GUARD_ASCII;
					CharData.uchScan = FSC_QTY_GUARD_SCAN;
					sendButtonMessageToBL(&CharData);

					CharData.uchASCII = ButtonScan_Adj[uchPluAdjMod].uchASCII;
					CharData.uchScan = ButtonScan_Adj[uchPluAdjMod].uchScan;
					sendButtonMessageToBL(&CharData);
				}

				BlFwIfConvertPLU (&aszCaption[0], &auchPluNo[0]);
				CharData = HandlePluButtonPress (&(aszCaption[0]));
				sendButtonMessageToBL(&CharData);
				break;
			}

			if (pWndItem) {
				POSITION pclListNext = pWndItem->ButtonItemList.GetHeadPosition ();
				POSITION pclList;
				BL_PLU_INFO xPluInfo;
				int nRetStat = 0;
				USHORT  usOepIndexNumber = 0, usOepGroupNumber = 0, usFilePart = 0;

				if (btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_Group) {
					usOepIndexNumber = 0;
					usOepGroupNumber = btnAttributes.m_myActionUnion.data.extFSC;
					usFilePart = 0;
				} else if (btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_GroupTable) {
					usOepIndexNumber = btnAttributes.m_myActionUnion.data.group_table.usTableNumber;
					usOepGroupNumber = btnAttributes.m_myActionUnion.data.group_table.usGroupNumber;
					usFilePart = (btnAttributes.m_myActionUnion.data.group_table.usOptionFlags & BUTTONACTIONTYPEGROUP_PRIORITY) ? 1 : 0;
				} else {
					pclListNext = 0;  // NULL out pointer to list so that we will just not do anything.
				}

				if (pclListNext) {
					CWindowButton *bp;
					CWindowControl *pwc;

					nRetStat = BlFwIfGetGroupPluInformationFirst(usOepIndexNumber, btnAttributes.m_myActionUnion.data.extFSC, usFilePart, 0L, 0, &xPluInfo);
					pclList = pclListNext;
					while (pclListNext && (pwc = pWndItem->ButtonItemList.GetNext (pclListNext) ) && nRetStat == 0) {
						bp = CWindowButton::TranslateCWindowControl (pwc);
						if (bp && bp->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_Group && 
							btnAttributes.m_myActionUnion.data.extFSC == bp->btnAttributes.m_myActionUnion.data.extFSC) {
							if (bp == this) {
								// If this PLU is an adjective type of PLU then we will
								// need to insert the adjective before we insert the PLU
								// number into the ring buffer.  However, if we are in
								// some mode other than Register Mode then we do not insert
								// the adjective code but insert the PLU number only.
								// This allows the use of Dynamic PLU buttons from Supervisor
								// Mode with the various PLU functions, AC63/64/67/82
								if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_REG &&
									xPluInfo.uchPluAdjMod > 0 && 
									xPluInfo.uchPluAdjMod < sizeof(ButtonScan_Adj)/sizeof(ButtonScan_Adj[0]))
								{
									CharData.uchASCII = FSC_QTY_GUARD_ASCII;
									CharData.uchScan = FSC_QTY_GUARD_SCAN;
									sendButtonMessageToBL(&CharData);

									CharData.uchASCII = ButtonScan_Adj[xPluInfo.uchPluAdjMod].uchASCII;
									CharData.uchScan = ButtonScan_Adj[xPluInfo.uchPluAdjMod].uchScan;
									sendButtonMessageToBL(&CharData);
								}

								BlFwIfConvertPLU (&aszCaption[0], &(xPluInfo.auchPluNo[0]));
								CharData = HandlePluButtonPress (&(aszCaption[0]));
								sendButtonMessageToBL(&CharData);
								break;
							}
							nRetStat = BlFwIfGetGroupPluInformationNext(&xPluInfo);
						}
					}
				}
			}
		}
		break;

	case CWindowButton::BATypeWinDis:
		{
			CFrameworkWndItem* pWndItem = (CFrameworkWndItem*)CWindowButton::GetParent();

			ASSERT(pWndItem);

			BlUifSupIntervent(BLI_SUPR_INTV_CLEAR);

			if (pWndItem) {
				if (pWndItem->pPrev && pWndItem->pPrev->m_hWnd) {
					pWndItem->pPrev->EnableWindow(TRUE);
					/*Cycle through all window items and reshow previuos window(s) if it(they)
					were hidden by popup*/
					if(!pWndItem->controlAttributes.isVirtualWindow){
						CWindowControl *pwc = CWindowDocument::pDoc->GetDataFirst ();
						while (pwc) {
							if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer && pwc != pWndItem){
								if(pwc->m_hWnd)
									pwc->EnableWindow(TRUE);
							}
							pwc = CWindowDocument::pDoc->GetDataNext ();
						}
					}
					CFrameworkWndItem * pTemp = CFrameworkWndItem::TranslateCWindowControl(pWndItem->pPrev);
					if (pTemp && pTemp->m_bInsideSuperIntrv && pWndItem->m_bInsideSuperIntrv) {
						BlUifSupIntervent(BLI_SUPR_INTV_SET);
					}
				}
				
				pWndItem->DestroyWindow ();
				TRACE3("%S(%d): CFrameworkWndButton::OnLButtonUp() Destroying Window named '%s'\n", __FILE__, __LINE__, pWndItem->myName);
			}
		}
		break;

	case CWindowButton::BATypeOEPEntry:
		// handle the interaction with an OEP table displayed with buttons by processing the button
		// presses. this button code is programmatically attached to the buttons in OEP windows when
		// the OEP window is created along with the buttons filling it.
		if(strncmp((char*)btnAttributes.m_myActionUnion.data.PLU, "9", 1) == 0){  //Continue Down Arrow
			CharData.uchASCII = 0xD8;CharData.uchScan = 0x00;  // send a scroll down or FSC_SCROLL_DOWN
			sendButtonMessageToBL(&CharData);
		}else if(strncmp((char*)btnAttributes.m_myActionUnion.data.PLU, "8", 1) == 0){ //Back Up Arrow, SPL_BTN_BACK
			CharData.uchASCII = 0xD7;CharData.uchScan = 0x00;  // send a scroll up or FSC_SCROLL_UP
			sendButtonMessageToBL(&CharData);
		}else  if(strncmp((char*)btnAttributes.m_myActionUnion.data.PLU, "7", 1) == 0){ //Done button, SPL_BTN_DONE
			CharData.uchASCII = 0x43; CharData.uchScan = 0x00;    // send a Clear or FSC_CLEAR
			sendButtonMessageToBL(&CharData);
		}else{
			sendNumericData(btnAttributes.m_myActionUnion.data.PLU);
		}
		break;

	case CWindowButton::BATypeHALOoverride:
		CharData.uchASCII = 0xEF;
		CharData.uchScan = 0;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeAtFor:
		CharData.uchASCII = 0xF0;
		CharData.uchScan = 0;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeManualValidation:
		CharData.uchASCII = 0xF1;
		CharData.uchScan = 0;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeShutDown:
		{
			ULONG	exitCode;
			BL_NOTIFYDATA   BlNotify;
			_DEVICEINPUT*   pDeviceInput;
			memset(&BlNotify, 0x00, sizeof(BlNotify));

			/*CharData.uchASCII = 0;
			CharData.uchScan = 0x00;
			sendButtonMessageToBL(&CharData);*/

			pDeviceInput = &BlNotify.u.Data;

			pDeviceInput->ulDeviceId = BL_DEVICE_DATA_POWERDOWN;

			BlNotify.ulType = BL_INPUT_DATA;
			BlNotifyData(&BlNotify, NULL);
			CWindowDocument* pWinDoc = (CWindowDocument*)CWindowButton::GetTopLevelParent();
			pWinDoc->~CWindowDocument();

			GetExitCodeProcess(GetCurrentProcess(), &exitCode);
			ExitProcess(exitCode);
		}
		break;

	case CWindowButton::BATypeCharacter:
		{
			USHORT  usKBMode_Prev;
			//We change the conversion table to use the 
			//alpha conversion table before we enter the key press
			//US Customs SCER for Rel 2.X JHHJ
			usKBMode_Prev = BlFwIfUifACChgKBType(UIF_ALPHA_KB);
			for(int t = 0; t < myCaption.GetLength(); t++){
				GetTopLevelParent()->SendMessage(WM_CHAR, (WPARAM)myCaption.GetAt(t), (LPARAM)0);
			}
			BlFwIfUifACChgKBType(usKBMode_Prev);
		}
		break;

	case CWindowButton::BATypeCharacterDelete:
		{
			USHORT  usKBMode_Prev;
			//We change the conversion table to use the 
			//alpha conversion table before we enter the key press
			//US Customs SCER for Rel 2.X JHHJ
			usKBMode_Prev = BlFwIfUifACChgKBType(UIF_ALPHA_KB);
			GetTopLevelParent()->SendMessage(WM_CHAR, (WPARAM)VK_BACK, (LPARAM)0);
			BlFwIfUifACChgKBType(usKBMode_Prev);
		}
		break;
	
	case CWindowButton::BATypeProg1:
		CharData.uchFlags = 0;          // P1 is a fixed key so we use whatever code page is current
		CharData.uchASCII = 0x21;       // uchMajor or major class, FSC_P1
		CharData.uchScan = 0;           // uchMinor or minor class
		CharData.uchFlags |= CHARDATA_FLAGS_HARDKEY;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeProg2:
		CharData.uchFlags = 0;          // P2 is a fixed key so we use whatever code page is current
		CharData.uchASCII = 0x22;       // uchMajor or major class, FSC_P2
		CharData.uchScan = 0;           // uchMinor or minor class
		CharData.uchFlags |= CHARDATA_FLAGS_HARDKEY;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeProg3:
		CharData.uchFlags = 0;          // P3 is a fixed key so we use whatever code page is current
		CharData.uchASCII = 0x23;       // uchMajor or major class, FSC_P3
		CharData.uchScan = 0;           // uchMinor or minor class
		CharData.uchFlags |= CHARDATA_FLAGS_HARDKEY;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeProg4:
		CharData.uchFlags = 0;          // P4 is a fixed key so we use whatever code page is current
		CharData.uchASCII = 0x24;       // uchMajor or major class, FSC_P4
		CharData.uchScan = 0;           // uchMinor or minor class
		CharData.uchFlags |= CHARDATA_FLAGS_HARDKEY;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeProg5:
		CharData.uchFlags = 0;          // P5 is a fixed key so we use whatever code page is current
		CharData.uchASCII = 0x25;       // uchMajor or major class, FSC_P5
		CharData.uchScan = 0;           // uchMinor or minor class
		CharData.uchFlags |= CHARDATA_FLAGS_HARDKEY;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeMinimize:
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWMINIMIZED);
		break;

	case CWindowButton::BATypeEditCondiment:
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x0C;            // FSC_EDIT_CONDIMENT
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeEditCondimentTbl:
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x26;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeReceiptId:
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x17;
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeOrderDeclaration:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_OrderDec)/sizeof(ButtonScan_OrderDec[0]))
		{
			CharData.uchASCII	= ButtonScan_OrderDec[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan	= ButtonScan_OrderDec[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}else
		{
			ASSERT(0);
		}
		break;

	case CWindowButton::BATypeGiftCard:
		if (btnAttributes.m_myActionUnion.data.extFSC > 0 &&
			btnAttributes.m_myActionUnion.data.extFSC < sizeof(ButtonScan_GiftCard)/sizeof(ButtonScan_GiftCard[0]))
		{
			CharData.uchASCII	= ButtonScan_GiftCard[btnAttributes.m_myActionUnion.data.extFSC].uchASCII;
			CharData.uchScan	= ButtonScan_GiftCard[btnAttributes.m_myActionUnion.data.extFSC].uchScan;
			sendButtonMessageToBL(&CharData);
		}else
		{
			ASSERT(0);
		}
//		CharData.uchASCII = 0xFF;
//		CharData.uchScan  = 0x1E;
//		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeSuprIntrvn:
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x23;           // FSC_SUPR_INTRVN
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDocumentLaunch:
		{
			int i = btnAttributes.m_myActionUnion.data.extFSC;
			int j;
			j = i % 10;
			i /= 10;
			if (i > 9) {
				i = 0;
			}
			CharData.uchASCII = ButtonScan_NumPad[i].uchASCII;
			CharData.uchScan = ButtonScan_NumPad[i].uchScan;
			sendButtonMessageToBL(&CharData);

			CharData.uchASCII = ButtonScan_NumPad[j].uchASCII;
			CharData.uchScan = ButtonScan_NumPad[j].uchScan;
			sendButtonMessageToBL(&CharData);
		}

		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x24;           // FSC_DOC_LAUNCH
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeOperatorPickup:
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x25;           // FSC_OPR_PICKUP
		sendButtonMessageToBL(&CharData);
		break;

	case CWindowButton::BATypeDisplayLabeledWindow:
	case CWindowButton::BATypeDismissLabeledWindow:
		// with inspiration from FSC_DOC_LAUNCH
		
		//if a number is pressed before this button
		//we will need to clear out the ring buffer
		//so that no information is sent prior to this
		//with inspiration from BATypeString
		CharData.uchASCII = FSC_CNT_STRING_GUARD_ASCII;
		CharData.uchScan = FSC_CNT_STRING_GUARD_SCAN;
		sendButtonMessageToBL(&CharData);
		{
			int i;
			for (i=0;i<15;i++){		//iterates over the incoming array that contains the window label in question
				if(btnAttributes.m_myActionUnion.data.PLU[i] != 0x00){		//as long as the entry isn't blank

					CharData.uchASCII = btnAttributes.m_myActionUnion.data.PLU[i];	//push each entry into the buffer
					CharData.uchScan = 0;
					sendButtonMessageToBL(&CharData);

				}
			}
		}

		if (btnAttributes.m_myActionUnion.type == CWindowButton::BATypeDisplayLabeledWindow)
			CharData.uchScan  = 0x27;           // FSC_WINDOW_DISPLAY
		else			
			CharData.uchScan  = 0x28;           // FSC_WINDOW_DISMISS			
		
		CharData.uchASCII = 0xFF;
		sendButtonMessageToBL(&CharData);

		break;

	case CWindowButton::BATypeDisplayLabeledWindowManual:
		//assumes manual entry of the window label prior to this buttonpress
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x27;           // FSC_WINDOW_DISPLAY
		sendButtonMessageToBL(&CharData);

		break;

	case CWindowButton::BATypeDismissLabeledWindowManual:
		//assumes manual entry of the window label prior to this buttonpress
		CharData.uchASCII = 0xFF;
		CharData.uchScan  = 0x28;           // FSC_WINDOW_DISMISS
		sendButtonMessageToBL(&CharData);

		break;

	case CWindowButton::BATypeAutoSignOut:
		CharData.uchASCII = 0xFF;
		CharData.uchScan = 0x29;            // FSC_AUTO_SIGN_OUT
		sendButtonMessageToBL(&CharData);
		break;

	default:
		ASSERT(0);
		break;
	}

	//Check to see if this is the last pick for the window
	//if it is the function will close the window
	//do not check if WinDis is first because GetParent will fail
	if(btnAttributes.m_myActionUnion.type != CWindowButton::BATypeWinDis && pMyParent){
		((CFrameworkWndItem*)pMyParent)->CheckLastPick();
	}
	if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_AutoClose) {
		// if this button has the auto close attribute then when we are done with handling
		// the button press event, we will close the window in which it is located.
		CFrameworkWndItem* pWndItem = (CFrameworkWndItem*)CWindowButton::GetParent();

		ASSERT(pWndItem);

		if (pWndItem) {
			pWndItem->PopdownWindow ();
		}
	}
}


void CFrameworkWndButton::sendNumericData(UCHAR *data)
{
	int i=0;
	//BYTE keyCode;
	CHARDATA	CharData;

	CharData.uchScan = 0x00;

	CharData.uchFlags = CHARDATA_FLAGS_TOUCHUTTON;
	for ( ; *data; data++){
		if (*data >= '0' && *data <= '9')
		{
			CharData.uchASCII = *data;
			sendButtonMessageToBL(&CharData);
		}
	}
}

void CFrameworkWndButton::sendButtonMessageToBL(CHARDATA* pCharData)
{
	BL_NOTIFYDATA   BlNotify;
    _DEVICEINPUT*   pDeviceInput;

	pDeviceInput = &BlNotify.u.Data;

	pDeviceInput->ulDeviceId = BL_DEVICE_DATA_KEY;
	pDeviceInput->ulLength = sizeof(*pCharData);
	pDeviceInput->pvAddr = pCharData;

	BlNotify.ulType = BL_INPUT_DATA;
	BlNotifyData(&BlNotify, NULL);
}

void CFrameworkWndButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//if the left mouse button is down while moving
	//set the focus to that button and off of the last
	//button then redraw so user knows visually which button
	//is selected
	if(nFlags == MK_LBUTTON)
	{
		SetFocus();
		Invalidate(FALSE);
	}
	CWindowButton::OnMouseMove(nFlags, point);
}

void CFrameworkWndButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//make sure if a double click is received a left button down message
	//is sent for the second click
	//this corrects the drawing problem encountered with fast clicks on 
	//owner drawn buttons
	//standard buttons normally do send the second left button down
	
	{
		CASDATA_RTRV casdata;
		BlFwIfGetCasData(&casdata);

		//check the group permissions to see if the user is allowed to use this control

		//is there a parent window, and does it have any group permissions
		if(pContainer && pContainer->controlAttributes.ulMaskGroupPermission)
			//does the currently signed in user have permission to use this window
			if(!(pContainer->controlAttributes.ulMaskGroupPermission & casdata.ulGroupAssociations))
				//if not, is this a dismiss button
				if(btnAttributes.m_myActionUnion.type != BATypeWinDis) {
					FrameworkNoPermissionsDialog();
					return;	//disallow if it is not a dismiss button
				}
		//does this control have any group permissions set
		if(controlAttributes.ulMaskGroupPermission)
			//does the currently signed in user have permission to use this button
			if(!(controlAttributes.ulMaskGroupPermission & casdata.ulGroupAssociations)) {
				FrameworkNoPermissionsDialog();
				return;	//disallow if the user does not have permission
			}
		
		//if this point is reached, then group permission checks have been passed
	}

	POINT pt;

	::GetCursorPos(&pt);

	::MapWindowPoints(NULL,(HWND)(m_hWnd),&pt,1);

	::SendMessage((HWND)(m_hWnd),WM_LBUTTONDOWN,0,MAKELPARAM(pt.x,pt.y));

	CWindowButton::OnLButtonDblClk(nFlags, point);
}
