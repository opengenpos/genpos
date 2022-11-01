
#include "StdAfx.h"

//#include "NewLayoutView.h"
#include "WindowTextExt.h"
#include "WindowButtonExt.h"
#include "WindowDocument.h"


CWindowTextExt::CWindowTextExt (int id, int row, int column, int width, int height) :
	CWindowText (id, row, column, width, height)
{
		controlAttributes.m_nType = CWindowControl::TextControl;
	
}

CWindowTextExt::CWindowTextExt(CWindowControl *wc) : CWindowText(wc){}

BOOL CWindowTextExt::WindowDisplay (CDC* pDC)
{
	int nSaveDC = pDC->SaveDC();

	CPen textColorPen;
	if(!controlAttributes.Selected){
		textColorPen.CreatePen(PS_SOLID, 1, controlAttributes.m_colorText);
	}
	else{
		textColorPen.CreatePen(PS_DOT, 1, SELECTED_CONTROL);
	}
	CPen* pOldPen = (pDC->SelectObject(&textColorPen));
	
	CBrush faceColorBrush(controlAttributes.m_colorFace);
	CBrush* pOldBrush = pDC->SelectObject(&faceColorBrush);

	int nColumn = controlAttributes.m_nColumn;//textAttributes.m_nLeft;
	int nRow = controlAttributes.m_nRow;//textAttributes.m_nTop;

	if (pContainer) {
		nColumn += pContainer->controlAttributes.m_nColumn;
		nRow  += pContainer->controlAttributes.m_nRow;
	}
	//create the rectangle that represents this text window, size appropriately
	CRect window (CWindowButton::getRectangleSized (nColumn, nRow, controlAttributes.m_usWidthMultiplier, controlAttributes.m_usHeightMultiplier));
//	CRect window( getRectangleSized(nColumn, nRow,
//			nColumn+(controlAttributes.m_nWidth*CWindowButtonExt::stdWidth), nRow+(controlAttributes.m_nHeight*CWindowButtonExt::stdHeight) ) );

	pDC->Rectangle(window);
	pDC->RestoreDC(nSaveDC);
	return TRUE;
}

CRect CWindowTextExt::getRectangleSized(int column, int row, USHORT usWidth, USHORT usHeight)
{
	CRect myRect (/*m_nLeft*/column, row/*m_nTop*/, usWidth, usHeight);
	return myRect;
}


//Control Serialize serializes and deserializes the data of this control
void CWindowTextExt::ControlSerialize (CArchive &ar){
	UCHAR* pBuff = (UCHAR*) &textAttributes;
	if (ar.IsStoring())
	{
		//call base class serialization
		CWindowText::ControlSerialize (ar);
	}
	else
	{
		nEndOfArchive = 0;
		try {
			ULONG mySize;
			memset(&textAttributes,0,sizeof(_tagTextAttributes));
			ar>>mySize;

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

			ASSERT(textAttributes.signatureStart = 0xABCDEF87);
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

BEGIN_MESSAGE_MAP(CWindowTextExt, CWindowText)
	//{{AFX_MSG_MAP(CWindowTextExt)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWindowTextExt::WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale)
{
	int retVal;
	CDC *pDC = pParentWnd->GetDC ();


	int nLogPixX = 100;//pDC->GetDeviceCaps (LOGPIXELSX);
	int nLogPixY = 100;//pDC->GetDeviceCaps (LOGPIXELSY);
	int xLeadingX = nLogPixX * CWindowDocument::m_nStdDeciInchLeading / 100;
	int xLeadingY = nLogPixY * CWindowDocument::m_nStdDeciInchLeading / 100;

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
		  AfxMessageBox(_T("Couldn't register class! (Already registered?)"));
		  pEx->Delete();
	}
	retVal = Create (strMyClass, myCaption, WS_CHILD | WS_VISIBLE, windRect, pParentWnd, controlAttributes.m_myId);
	pParentWnd->ReleaseDC(pDC);

	return retVal;
}


void CWindowTextExt::OnPaint()
{
	CPaintDC    dc(this);                   // device context for painting

	int oldDCContext = dc.SaveDC ();

	CRect myRect;
	GetClientRect(myRect);

	dc.FillSolidRect(&myRect, controlAttributes.m_colorFace);

	dc.RestoreDC (oldDCContext);


}

BOOL CWindowTextExt::PreCreateWindow(CREATESTRUCT& cs) 
{
    // change windows style

  	  cs.dwExStyle |= WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

    return CWnd::PreCreateWindow(cs);
}