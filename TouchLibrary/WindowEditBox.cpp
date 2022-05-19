// WindowLabel.cpp: implementation of the CWindowEditBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WindowEditBox.h"
#include "WindowDocument.h"


// this window procedure is used as part of the registration procedure
// but MFC over rides the registered message proc so that this procedure
// does not seem to actually be used.
// Command processing for buttons, etc, needs to be handled in the method
// OnCmdMsg below.
static LRESULT CALLBACK CWindowItemWindowProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	return DefWindowProc (hwnd, uMsg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//constructor for CWindowEditBox class
CWindowEditBox::CWindowEditBox(CWindowControl *wc) :
	CWindowControl(wc)
{
		
	/*if(wc){
		editboxAttributes.capAlignment = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.capAlignment;
		editboxAttributes.horIconAlignment = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.horIconAlignment;
		editboxAttributes.vertIconAlignment = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.vertIconAlignment;
		editboxAttributes.HorizontalOriented = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.HorizontalOriented;
		editboxAttributes.signatureStart = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.signatureStart;
		m_IconName = (CWindowEditBox::TranslateCWindowControl(wc))->m_IconName;
		editboxAttributes.Show = (CWindowEditBox::TranslateCWindowControl(wc))->editboxAttributes.Show;
		controlAttributes.m_nType = wc->controlAttributes.m_nType;
	}*/
	if(!wc){
		editboxAttributes.capAlignment = CaptionAlignmentHCenter;
		editboxAttributes.horIconAlignment = IconAlignmentHCenter;
		editboxAttributes.vertIconAlignment = IconAlignmentVMiddle;
		editboxAttributes.HorizontalOriented = TRUE;
		editboxAttributes.signatureStart = 0xABCDEF87;
		editboxAttributes.Show = TRUE;
		controlAttributes.m_nType = EditBoxControl;
	}

}

//constructor for CWindowEditBox class 
CWindowEditBox::CWindowEditBox (int id, int row, int column, int width, int height) :
	CWindowControl(id, row, column, width, height)
{
		controlAttributes.m_nType = EditBoxControl;
		editboxAttributes.capAlignment = CaptionAlignmentHCenter;
		editboxAttributes.horIconAlignment = IconAlignmentHCenter;
		editboxAttributes.vertIconAlignment = IconAlignmentVMiddle;
		editboxAttributes.HorizontalOriented = TRUE;
		editboxAttributes.signatureStart = 0xABCDEF87;
		m_IconName = _T("");
		editboxAttributes.Show = TRUE;
}


CWindowEditBox::~CWindowEditBox()
{

}

void CWindowEditBox::Serialize (CArchive &ar)
{
	//call base class serialization funtion and then call ControlSerialize function for this class
	if (ar.IsStoring())
	{
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
	else
	{
		CWindowControl::Serialize (ar);

		ControlSerialize (ar);
	}
}

void CWindowEditBox::ControlSerialize (CArchive &ar)
{
	UCHAR* pBuff = (UCHAR*) &editboxAttributes;
	if (ar.IsStoring())
	{
		//verify teh start signature to check against data corruption
		ASSERT(editboxAttributes.signatureStart = 0xABCDEF87);

		memcpy (controlAttributes.m_myCaption, editboxAttributes.tcsLabelText, sizeof(controlAttributes.m_myCaption));
		controlAttributes.m_myCaption[MaxSizeOfMyCaption] = 0;

		//serialize attributes for this class
		ar<<sizeof(_tagWinEbAttributes);
		for(int i = 0; i < sizeof(_tagWinEbAttributes); i++){
			ar<<pBuff[i];
		}
		
	}
	else
	{
		nEndOfArchive = 0;
		try {

			ULONG mySize;
			//serialize attributes for this class
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
			
			// if this file is from an older version of Layout Manager we need to make adjustments.
			if (mySize < sizeof (editboxAttributes)) {
				// adjust the caption text into the new area.
				_tcsncpy (editboxAttributes.tcsLabelText, myCaption, sizeof(editboxAttributes.tcsLabelText)/sizeof(editboxAttributes.tcsLabelText[0]) - 1);
			}
			else {
				myCaption = editboxAttributes.tcsLabelText;
			}

			//verify teh start signature to check against data corruption
			ASSERT(editboxAttributes.signatureStart = 0xABCDEF87);
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

BOOL CWindowEditBox::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
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

	Create (NULL, myCaption, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS, windRect, pParentWnd, controlAttributes.m_myId);
	 
	InvalidateRect (NULL, FALSE);
	return TRUE;
}



BOOL CWindowEditBox::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CWindowControl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}