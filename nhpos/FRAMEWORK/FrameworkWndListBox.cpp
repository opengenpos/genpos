// FrameworkWndButton.cpp : implementation file
//

#include "stdafx.h"
#include "Framework.h"
#include "FrameworkWndListBox.h"
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

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndButton

CFrameworkWndListBox::CFrameworkWndListBox(int id, int row, int column, int width, int height) :
	CWindowListBox (id, row, column, width, height)
{
	m_listboxFont = 0;
	m_DialogId = 0;
	hBitmapFocus = 0;
	memset (auchPluNo, 0, sizeof(auchPluNo));;
	uchPluAdjMod = 0;
}

CFrameworkWndListBox::CFrameworkWndListBox(CWindowControl *wc) :
	CWindowListBox(wc)
{
	m_listboxFont = 0;
	m_DialogId = 0;
	hBitmapFocus = 0;
	auchPluNo[0] = 0;
	uchPluAdjMod = 0;
}

CFrameworkWndListBox::~CFrameworkWndListBox()
{
	// Clean up the bitmap handle and release the object
	if (hBitmapFocus) {
		DeleteObject(hBitmapFocus);
		hBitmapFocus = 0;
	}
	if (m_listboxFont) {
		delete m_listboxFont;
		m_listboxFont = 0;
	}

	//	DestroyWindow();

	TRACE3("%S(%d): CFrameworkWndListBox::~CFrameworkWndListBox() Destroying ListBox '%s'\n", __FILE__, __LINE__, myCaption);
}

BOOL CFrameworkWndListBox::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
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

	BOOL isCreated = Create (_T("LISTBOX"), myCaption, WS_CHILD | WS_VISIBLE | LBS_STANDARD, windRect, pParentWnd, controlAttributes.m_myId);
	ASSERT(isCreated);

	if (isCreated) {
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

		{
			TCHAR tcsBuff[128];
			int   iLoop;
			HWND  hWndControl;

			pParentWnd->GetDlgItem (controlAttributes.m_myId, &hWndControl);
			for (iLoop = 1; iLoop < 25; iLoop++) {
				_stprintf (tcsBuff, _T("String %d"), iLoop);
				::SendMessage(hWndControl, LB_ADDSTRING, 0, (LPARAM)tcsBuff);
			}
		}

		InvalidateRect (NULL, FALSE);
	}
	else {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_WINCREATE_FAIL_2);
	}
	return isCreated;
}

BOOL CFrameworkWndListBox::WindowDisplay (CDC* pDC)
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
	CRect myRect (CWindowListBox::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));

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
int CFrameworkWndListBox::ChangeRefreshIcon(TCHAR *tcsIconFileName)
{
	HBITMAP hTempBitmapFocus = hBitmapFocus;
	CString csTempIcon = m_Icon;

	m_Icon = _T("");
	hBitmapFocus = 0;

	if (hTempBitmapFocus) {
		DeleteObject (hTempBitmapFocus);
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
		hBitmapFocus = (HBITMAP)LoadImage(NULL, iconPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	Invalidate ();
	UpdateWindow ();

	return 0;
}


void CFrameworkWndListBox::DrawListBox(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CFrameworkWndItem* pWndItem)
{
	ASSERT(lpDraw);

	CDC* pDC;
	CFrameworkWndListBox* pListBoxWnd;

	if(! pWndItem || ! lpDraw){
		return;
	}

	pListBoxWnd = (CFrameworkWndListBox*)pWndItem->GetDlgItem(nIDCtl);
	pDC = CDC::FromHandle(lpDraw->hDC);

	if (lpDraw->itemState == ODS_FOCUS)
	{
		pListBoxWnd->DrawListBoxOnDisplayFocus (pDC);
	}
	else
	{
		pListBoxWnd->DrawListBoxOnDisplayNoFocus (pDC);
	}
	pListBoxWnd->ReleaseDC(pDC);
}

void CFrameworkWndListBox::DrawListBoxOnDisplayFocus (CDC* pDC)
{	
	int iSaveDC = pDC->SaveDC ();

	CPen myPen;
	myPen.CreatePen(PS_NULL, 2, controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowListBox::getRectangleSized (0, 0, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;

	myBrush.CreateSolidBrush(controlAttributes.m_colorFace);

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

		pDC->SelectObject(bottomEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Rectangle(myRect.left + 4, myRect.top + 4, myRect.right - 2, myRect.bottom - 2);


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
	CString csButtonText(myCaption);
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();
}

void CFrameworkWndListBox::DrawListBoxOnDisplayNoFocus (CDC* pDC)
{	
	int iSaveDC = pDC->SaveDC ();

	CPen myPen;
	myPen.CreatePen(PS_NULL, 2, controlAttributes.m_colorText);

	CPen* pOldPen = pDC->SelectObject(&myPen);

	CRect myRect (CWindowListBox::getRectangleSized (0, 0, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
	CBrush myBrush;

	myBrush.CreateSolidBrush(controlAttributes.m_colorFace);

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

		pDC->SelectObject(bottomEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right, myRect.bottom);
		pDC->SelectObject(topEdge);
		pDC->Rectangle(myRect.left, myRect.top, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(shadowEdge);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 2, myRect.bottom - 2);
		pDC->SelectObject(myBrush);
		pDC->Rectangle(myRect.left + 2, myRect.top + 2, myRect.right - 4, myRect.bottom - 4);


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
	CString csButtonText(myCaption);
	//if the text on the button is a space then load the resource string for space
	//and use that for the button text
	if(csButtonText.Compare(_T(" ")) == 0){
		csButtonText.Empty();
		csButtonText.LoadString(IDS_SPACE);
	}

	pDC->RestoreDC (iSaveDC);

//Clean up Objects
	myPen.DeleteObject();
	myBrush.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFrameworkWndListBox, CWindowListBox)
	//{{AFX_MSG_MAP(CFrameworkWndListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndButton message handlers



void CFrameworkWndListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Draw the button Pressed
	SetFocus();
	Invalidate(FALSE);
}


void CFrameworkWndListBox::sendNumericData(UCHAR *data)
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
			sendListBoxMessageToBL(&CharData);
		}
	}
}

void CFrameworkWndListBox::sendListBoxMessageToBL(CHARDATA* pCharData)
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
