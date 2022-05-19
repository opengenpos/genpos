// WindowText.cpp : implementation file
//

#include "stdafx.h"
#include "WindowDocument.h"
#include "WindowControl.h"
#include "WindowText.h"
#include "WindowItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowText

CWindowText::CWindowText(CWindowControl *wc /* = 0 */) :
	CWindowControl(wc)
{
	//initlize attributes
	if (!wc || !CWindowText::TranslateCWindowControl(wc)) {
//		controlAttributes.m_nType = TextControl;
		memset(&textAttributes ,0, sizeof(TextAttributes));		//NULL unless OEP, if OEP, this will be set
		textAttributes.signatureStart = 0xABCDEF87;
		textAttributes.oepBtnDown = 4;
		textAttributes.oepBtnAcross = 4;
		textAttributes.oepBtnWidth = 3;
		textAttributes.oepBtnHeight = 3;
		textAttributes.oepBtnColor = RGB(220, 220, 220);
		textAttributes.oepBtnFontColor = RGB(0, 0, 0);
		textAttributes.oepBtnShape = CWindowButton::ButtonShapeRect;
		textAttributes.oepBtnPattern = CWindowButton::ButtonPatternNone;
	}
	else{
		memcpy(&textAttributes,&(CWindowText::TranslateCWindowControl(wc)->textAttributes),sizeof(textAttributes));
	}

	controlAttributes.m_nType = TextControl;
}

CWindowText::CWindowText (int id, int row, int column, int width, int height) :
	CWindowControl (id, row, column, width, height)
{
	//initialize attributes
	controlAttributes.m_nType = TextControl;
	memset(&textAttributes, 0, sizeof(TextAttributes));
	textAttributes.signatureStart = 0xABCDEF87;
	textAttributes.m_nTop = 0;
	textAttributes.m_nLeft = 0;
	textAttributes.type = TypeSnglReceiptMain;
	textAttributes.oepBtnDown = 4;
	textAttributes.oepBtnAcross = 4;
	textAttributes.oepBtnWidth = 3;
	textAttributes.oepBtnHeight = 3;
	textAttributes.oepBtnColor = RGB(220, 220, 220);
	textAttributes.oepBtnFontColor = RGB(0, 0, 0);
	textAttributes.oepBtnShape = CWindowButton::ButtonShapeRect;
	textAttributes.oepBtnPattern = CWindowButton::ButtonPatternNone;
}

CWindowText::~CWindowText()
{
}

void CWindowText::Serialize( CArchive& ar )
{
	if (ar.IsStoring())
	{
		CWindowControl::Serialize (ar);

		// do the rest of the derived object store operation here
		ControlSerialize (ar);
	}
	else {
		CWindowControl::Serialize (ar);

		// do the rest of the derived object load operation here
		ControlSerialize (ar);
	}
}
void CWindowText::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &textAttributes;

	if (ar.IsStoring())
	{
	
		ar<<sizeof(_tagTextAttributes);
		for(int i = 0; i < sizeof(_tagTextAttributes); i++){
			ar<<pBuff[i];
		}

	}
	else
	{
		ULONG mySize;
		memset(&textAttributes,0,sizeof(_tagTextAttributes));
		ar>>mySize;

		ASSERT(mySize <= sizeof(_tagTextAttributes));

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (textAttributes)) {
			xMySize = sizeof (textAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}

		//verify teh start signature to check against data corruption
		ASSERT(textAttributes.signatureStart = 0xABCDEF87);

	}
}


BEGIN_MESSAGE_MAP(CWindowText, CWindowControl)
	//{{AFX_MSG_MAP(CWindowText)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowText message handlers

static LRESULT CALLBACK CWindowItemWindowProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	return DefWindowProc (hwnd, uMsg, wParam, lParam);
}


/*
		scale indicates the percentage of normal size and is the percentage expressed
		as an integer.  This means if you want 90%, scale should be equal to 90.

		The standard button widths are expressed in tenths of an inch.  this means
		if a standard button width is 10 then it is 1/10 inch.  So we multiply by
		the standard button metric along the appropriate axis and then divide by
		100 to arrive at the final size of the button.
 */
BOOL CWindowText::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
{
	ASSERT(pParentWnd);

	int retVal;
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
					xColumn + controlAttributes.m_usWidthMultiplier * nWidth ,
					xRow + controlAttributes.m_usHeightMultiplier * nHeight );

	if (nID)
		controlAttributes.m_myId = nID;

	CString  strMyClass;

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
		  AfxMessageBox( _T("Couldn't register class! (Already registered?)"));
		  pEx->Delete();
	}
	retVal = Create (strMyClass, myCaption, WS_CHILD, windRect, pParentWnd, controlAttributes.m_myId);
	return retVal;
	

}

