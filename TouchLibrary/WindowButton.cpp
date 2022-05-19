// WindowButton.cpp : implementation file
//

#include "stdafx.h"
#include "WindowDocument.h"
#include "WindowControl.h"
#include "WindowButton.h"
#include "WindowItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowButton

//constructor for CWindowButton class 
CWindowButton::CWindowButton(CWindowControl *wc) :
	CWindowControl(wc)
{
	//initialize attributes if not constructing from an existing object
	if (!wc) {
		m_Icon = _T("");
		controlAttributes.m_nType = ButtonControl;
		btnAttributes.m_myShape = ButtonShapeRect;
		btnAttributes.m_myPattern = ButtonPatternNone;
		memset (&(btnAttributes.m_myActionUnion), 0, sizeof (btnAttributes.m_myActionUnion));
		btnAttributes.capAlignment = CaptionAlignmentHCenter;
		btnAttributes.horIconAlignment = IconAlignmentHCenter;
		btnAttributes.vertIconAlignment = IconAlignmentVMiddle;
		btnAttributes.HorizontalOriented = TRUE;
		btnAttributes.signatureStart = 0xABCDEF87;
	}

}

//constructor for CWindowButton class 
CWindowButton::CWindowButton (int id, int row, int column, int width, int height) :
	CWindowControl(id, row, column, width, height)
{
	m_Icon = _T("");
	controlAttributes.m_nType = ButtonControl;
	memset (&btnAttributes, 0, sizeof (btnAttributes));
	btnAttributes.m_myShape = ButtonShapeRect;
	btnAttributes.m_myPattern = ButtonPatternNone;
	btnAttributes.capAlignment = CaptionAlignmentHCenter;
	btnAttributes.horIconAlignment = IconAlignmentHCenter;
	btnAttributes.vertIconAlignment = IconAlignmentVMiddle;
	btnAttributes.HorizontalOriented = TRUE;
	btnAttributes.signatureStart = 0xABCDEF87;
}

CWindowButton::~CWindowButton()
{
}

void CWindowButton::Serialize( CArchive& ar )
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

void CWindowButton::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &btnAttributes;

	if (ar.IsStoring())
	{
		//copy icon file name from CSting to TCHAR so we can serialize it
		lstrcpy(btnAttributes.myIcon ,m_Icon);

		//verify signature start so we can check for data corruption
		ASSERT(btnAttributes.signatureStart = 0xABCDEF87);

		ar<<sizeof(_tagWinBtnAttributes);
		for(int i = 0; i < sizeof(_tagWinBtnAttributes); i++){
			ar<<pBuff[i];
		}

	}
	else
	{
		try {
			nEndOfArchive = 0;

			ULONG mySize;
			memset(&btnAttributes,0,sizeof(_tagWinBtnAttributes));
			ar>>mySize;

			ASSERT(mySize <= sizeof(_tagWinBtnAttributes));
#if 1
			// Because the file that is being loaded in may be from a
			// different version of Layout Manager, we must be careful
			// about just reading in the information from the file.
			// If the object stored is larger than the object in the
			// version of Layout Manager being used then we will
			// overwrite memory and cause application failure.
			ULONG  xMySize = mySize;
			UCHAR  ucTemp;

			if (xMySize > sizeof(_tagWinBtnAttributes)) {
				xMySize = sizeof(_tagWinBtnAttributes);
			}
			UINT y = 0;
			for (y = 0; y < xMySize;y++) {
				ar >> pBuff[y];
			}
			for (; y < mySize;y++) {
				ar >> ucTemp;
			}

#else
			for(UINT z = 0; z < mySize;z++){
				ar>>pBuff[z];
			}
#endif
			ASSERT(btnAttributes.signatureStart = 0xABCDEF87);
			//assign icon file name from TCHAR to CString 
			m_Icon = btnAttributes.myIcon;
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


CRect CWindowButton::getRectangle (int row, int column)
{
	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	
	//Evaluates to (0,0,15,15)
	CRect myStdRect (0, 0, CWindowButton::stdWidth - CWindowButton::stdLeading,
						CWindowButton::stdHeight - CWindowButton::stdLeading );

	//adjust rectangle to be at the correct row and column
	myStdRect.bottom = myStdRect.bottom + row * CWindowButton::stdHeight;
	myStdRect.left = myStdRect.left + column * CWindowButton::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowButton::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowButton::stdWidth;
	return myStdRect;
}

CRect CWindowButton::getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult)
{
	// do a basic sanity check on the row and column values
	ASSERT(row >=0 && row <= 100);
	ASSERT(column >=0 && column <= 100);
	
	//Evaluates to (0,0,70,70)
	CRect myStdRect (0, 0, CWindowButton::stdWidth - CWindowButton::stdLeading,
						CWindowButton::stdHeight - CWindowButton::stdLeading );

	ASSERT ( usWidthMult > 0);
	ASSERT (usHeightMult > 0);

	//adjust rectangle to be at the correct row and column and have the correct heigth and width
	myStdRect.left = myStdRect.left + column * CWindowButton::stdWidth;
	myStdRect.top = myStdRect.top + row * CWindowButton::stdHeight;
	myStdRect.right = myStdRect.right + column * CWindowButton::stdWidth + (usWidthMult - 1) * CWindowButton::stdWidth;
	myStdRect.bottom = myStdRect.bottom + row * CWindowButton::stdHeight + (usHeightMult - 1) * CWindowButton::stdHeight;

	return myStdRect;
}


BEGIN_MESSAGE_MAP(CWindowButton, CWindowControl)
	//{{AFX_MSG_MAP(CWindowButton)
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
BOOL CWindowButton::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
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

	CRect windRect (xColumn, 
				    xRow, 
					xColumn + controlAttributes.m_usWidthMultiplier * nWidth, 
					xRow + controlAttributes.m_usHeightMultiplier * nHeight);

	if (nID)
		controlAttributes.m_myId = nID;

	Create (_T("BUTTON"), myCaption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_MULTILINE, windRect, pParentWnd, controlAttributes.m_myId);
	 
	InvalidateRect (NULL, FALSE);
	return TRUE;
}

