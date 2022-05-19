// WindowButton.cpp : implementation file
//

#include "stdafx.h"
#include "WindowDocument.h"
#include "WindowControl.h"
#include "WindowListBox.h"
#include "WindowItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowListBox

//constructor for CWindowListBox class 
CWindowListBox::CWindowListBox(CWindowControl *wc) :
	CWindowControl(wc)
{
	//initialize attributes if not constructing from an existing object
	if (!wc) {
		m_Icon = _T("");
		controlAttributes.m_nType = ListBoxControl;
		lbAttributes.HorizontalOriented = TRUE;
		lbAttributes.signatureStart = 0xABCDEF87;
	}

}

//constructor for CWindowButton class 
CWindowListBox::CWindowListBox (int id, int row, int column, int width, int height) :
	CWindowControl(id, row, column, width, height)
{
	m_Icon = _T("");
	controlAttributes.m_nType = ListBoxControl;
	lbAttributes.HorizontalOriented = TRUE;
	lbAttributes.signatureStart = 0xABCDEF87;
}

CWindowListBox::~CWindowListBox()
{
}

void CWindowListBox::Serialize( CArchive& ar )
{

	//call base class serialization funtion and then call ControlSerialize function for this class
	if (ar.IsStoring())
	{
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
	else {
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
}

void CWindowListBox::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &lbAttributes;

	if (ar.IsStoring())
	{
		//copy icon file name from CSting to TCHAR so we can serialize it
		lstrcpy(lbAttributes.myIcon ,m_Icon);

		//verify signature start so we can check for data corruption
		ASSERT(lbAttributes.signatureStart = 0xABCDEF87);

		ar<<sizeof(_tagWinLbAttributes);
		for(int i = 0; i < sizeof(_tagWinLbAttributes); i++){
			ar<<pBuff[i];
		}

	}
	else
	{
		try {
			nEndOfArchive = 0;

			ULONG mySize;
			memset(&lbAttributes,0,sizeof(_tagWinLbAttributes));
			ar>>mySize;
			for(UINT z = 0; z < mySize;z++){
				ar>>pBuff[z];
			}
			ASSERT(lbAttributes.signatureStart = 0xABCDEF87);
			//assign icon file name from TCHAR to CString 
			m_Icon = lbAttributes.myIcon;
		}
		catch (CArchiveException  *e) {
			nEndOfArchive = 1;
			ASSERT(nEndOfArchive == 0);
			e->Delete();
		}
		catch (CFileException *e) {
			nEndOfArchive = 1;
			ASSERT(nEndOfArchive == 0);
			e->Delete();
		}
	}

}


CRect CWindowListBox::getRectangle (int row, int column)
{
	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	
	//Evaluates to (0,0,15,15)
	CRect myStdRect (0, 0, CWindowListBox::stdWidth - CWindowListBox::stdLeading,
						CWindowListBox::stdHeight - CWindowListBox::stdLeading );

	//adjust rectangle to be at the correct row and column
	myStdRect.bottom = myStdRect.bottom + row * CWindowListBox::stdHeight;
	myStdRect.left = myStdRect.left + column * CWindowListBox::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowListBox::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowListBox::stdWidth;
	return myStdRect;
}

CRect CWindowListBox::getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult)
{
	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	
	//Evaluates to (0,0,70,70)
	CRect myStdRect (0, 0, CWindowListBox::stdWidth - CWindowListBox::stdLeading,
						CWindowListBox::stdHeight - CWindowListBox::stdLeading );

	ASSERT ( usWidthMult > 0);
	ASSERT (usHeightMult > 0);

	//adjust rectangle to be at the correct row and column and have the correct heigth and width
	myStdRect.left = myStdRect.left + column * CWindowListBox::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowListBox::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowListBox::stdWidth + (usWidthMult - 1) * CWindowListBox::stdWidth;
	myStdRect.bottom = myStdRect.bottom + row * CWindowListBox::stdHeight + (usHeightMult - 1) * CWindowListBox::stdHeight;

	return myStdRect;
}


BEGIN_MESSAGE_MAP(CWindowListBox, CWindowControl)
	//{{AFX_MSG_MAP(CWindowListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowButton message handlers

/*
		scale indicates the percentage of normal size and is the percentage expressed
		as an integer.  This means if you want 90%, scale should be equal to 90.

		The standard button widths are expressed in tenths of an inch.  this means
		if a standard button width is 10 then it is 1/10 inch.  So we multiply by
		the standard button metric along the appropriate axis and then divide by
		100 to arrive at the final size of the button.
 */
BOOL CWindowListBox::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
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

	CRect windRect (xColumn, xRow, 
					xColumn + controlAttributes.m_usWidthMultiplier * nWidth, 
					xRow + controlAttributes.m_usHeightMultiplier * nHeight);

	if (nID)
		controlAttributes.m_myId = nID;

	Create (_T("LISTBOX"), myCaption, WS_CHILD | WS_VISIBLE, windRect, pParentWnd, controlAttributes.m_myId);
	 
	InvalidateRect (NULL, FALSE);
	return TRUE;
}

