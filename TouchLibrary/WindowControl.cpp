// WindowControl.cpp : implementation file
//

/*

	CWindowControl is the abstract base class from which all controls are derived.

	This abstract base class contains implementation interface which all of the
	derived control types must support as well as implementations of selected
	portions of the interface.

	CWindowControl(CWindowControl *wc)
		This method provides for a mechanism to use during serialization when
		loading a set of controls from the CArchive.

		Derived classes should use the following steps for loading themselves
		in the Serialize method:
			CWindowControlSample pcsample;
			CWindowControl *pc;
			pc = pcsample.NewSerialize (ar);     // create a new CWindowControl


 */

#include "stdafx.h"
#include "WindowControl.h"
#include "WindowButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_SERIAL(CWindowControl, CWnd, SCHEMA_WINDOWCONTROL)

/////////////////////////////////////////////////////////////////////////////
// CWindowControl

int CWindowControl::m_iSpecialId = 1000000;  // special id number that is incremented with each use.

CWindowControl::CWindowControl(){
	memset (&controlAttributes, 0, sizeof(controlAttributes));
	
	controlAttributes.signatureStart = 0xABCDEF87;
	controlAttributes.m_nType = UnknownControl;
	controlAttributes.m_myId = 0;
	controlAttributes.m_nRow = 1;
	controlAttributes.m_nColumn = 1;
	controlAttributes.m_nHeight = controlAttributes.m_usHeightMultiplier * CWindowButton::stdHeight;
	controlAttributes.m_nWidth = controlAttributes.m_usWidthMultiplier * CWindowButton::stdWidth;
	controlAttributes.m_colorFace = RGB(220, 220, 220);
	controlAttributes.m_colorText = RGB(0, 0, 0);
	controlAttributes.m_usWidthMultiplier = 3;
	controlAttributes.m_usHeightMultiplier = 3;
	controlAttributes.m_myAction = 0;
	controlAttributes.ulMaskGroupPermission = 0;

	myCaption = _T("Def Control");
	myName = _T("Def Name");
	pPrev = NULL;
	pContainer = NULL;

	controlAttributes.isVirtualWindow = FALSE;
	controlAttributes.useDefault = TRUE;

	controlAttributes.CurrentID = 0;
	controlAttributes.Selected = TRUE;
}

CWindowControl::CWindowControl(CWindowControl* wc)
{
	controlAttributes.signatureStart = 0xABCDEF87;

	controlAttributes.m_nType = wc->controlAttributes.m_nType;
	controlAttributes.m_myId = wc->controlAttributes.m_myId;		
	controlAttributes.m_nRow = wc->controlAttributes.m_nRow;
	controlAttributes.m_nColumn = wc->controlAttributes.m_nColumn;
	controlAttributes.m_nHeight = wc->controlAttributes.m_nHeight;
	controlAttributes.m_nWidth = wc->controlAttributes.m_nWidth;
	controlAttributes.m_colorFace = wc->controlAttributes.m_colorFace;
	controlAttributes.m_colorText = wc->controlAttributes.m_colorText;
	controlAttributes.m_usWidthMultiplier = wc->controlAttributes.m_usWidthMultiplier;
	controlAttributes.m_usHeightMultiplier = wc->controlAttributes.m_usHeightMultiplier;
	controlAttributes.m_myAction = wc->controlAttributes.m_myAction;
	
	myCaption = wc->myCaption;
	myName = wc->myName;
	_tcsncpy_s(controlAttributes.m_myName, myName, MaxSizeOfMyCaption);
	controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

	controlAttributes.isVirtualWindow = wc->controlAttributes.isVirtualWindow;
	controlAttributes.useDefault = wc->controlAttributes.useDefault;
	controlAttributes.lfControlFont = wc->controlAttributes.lfControlFont;
	controlAttributes.CurrentID = wc->controlAttributes.CurrentID;
	controlAttributes.Selected = wc->controlAttributes.Selected;
	
	for (int i = 0; i < sizeof(wc->controlAttributes.SpecWin); i++)
		controlAttributes.SpecWin [i] = wc->controlAttributes.SpecWin [i];

	for (int i = 0; i < sizeof(controlAttributes.mOEPGroupNumbers) / sizeof(controlAttributes.mOEPGroupNumbers[0]); i++)
		controlAttributes.mOEPGroupNumbers[i] = wc->controlAttributes.mOEPGroupNumbers[i];

	controlAttributes.ulMaskGroupPermission = wc->controlAttributes.ulMaskGroupPermission;
	
	pPrev = wc->pPrev;
	pContainer = wc->pContainer;

}

//constructor for CWindowControl class
CWindowControl::CWindowControl (UINT id, int row, int column, int width , int height)
{
	if (id == 0) {
		// if id is zero then more than likely this is a temporary control.
		// we are going to create a unique id for this control.
		id = m_iSpecialId++;
	}

	memset (&controlAttributes, 0, sizeof(controlAttributes));

	controlAttributes.Selected = TRUE;
	controlAttributes.signatureStart = 0xABCDEF87;
	controlAttributes.useDefault = TRUE;
	controlAttributes.m_myId = id;
	controlAttributes.m_nType = UnknownControl;
	controlAttributes.m_nRow = row;
	controlAttributes.m_nColumn = column;
	controlAttributes.m_usHeightMultiplier = height;
	controlAttributes.m_usWidthMultiplier = width;
	controlAttributes.m_nHeight = controlAttributes.m_usHeightMultiplier * CWindowButton::stdHeight;
	controlAttributes.m_nWidth = controlAttributes.m_usWidthMultiplier * CWindowButton::stdWidth;
	controlAttributes.m_colorText = RGB(0, 0, 0);
	controlAttributes.m_colorFace = RGB(220, 220, 220);
	controlAttributes.isVirtualWindow = FALSE;
	controlAttributes.m_myAction = 0;
	controlAttributes.ulMaskGroupPermission = 0;

	myCaption = _T("Def Control");
	myName = _T("Def Name");
	_tcsncpy(controlAttributes.m_myName, myName, MaxSizeOfMyCaption);
	controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;
	pPrev = NULL;
	pContainer = NULL;
}


CWindowControl::~CWindowControl()
{
}

//copy attributes from one CWindowCOntrol object to another
CWindowControl & CWindowControl::operator = (CWindowControl & wc)
{
	controlAttributes.signatureStart = wc.controlAttributes.signatureStart;
	controlAttributes.m_nType = wc.controlAttributes.m_nType;
	controlAttributes.m_myId = wc.controlAttributes.m_myId;
	controlAttributes.m_nRow = wc.controlAttributes.m_nRow;
	controlAttributes.m_nColumn = wc.controlAttributes.m_nColumn;
	controlAttributes.m_nHeight = wc.controlAttributes.m_nHeight;
	controlAttributes.m_nWidth = wc.controlAttributes.m_nWidth;
	controlAttributes.m_colorFace = wc.controlAttributes.m_colorFace;
	controlAttributes.m_colorText = wc.controlAttributes.m_colorText;
	controlAttributes.m_usWidthMultiplier = wc.controlAttributes.m_usWidthMultiplier;
	controlAttributes.m_usHeightMultiplier = wc.controlAttributes.m_usHeightMultiplier;
	controlAttributes.m_myAction = wc.controlAttributes.m_myAction;

	myCaption = wc.myCaption;
	myName = wc.myName;
	_tcsncpy(controlAttributes.m_myName, myName, MaxSizeOfMyCaption);
	controlAttributes.m_myName[MaxSizeOfMyCaption] = 0;

	controlAttributes.isVirtualWindow = wc.controlAttributes.isVirtualWindow;
	controlAttributes.useDefault = wc.controlAttributes.useDefault;	//added NSM
	controlAttributes.lfControlFont = wc.controlAttributes.lfControlFont;
	controlAttributes.CurrentID = wc.controlAttributes.CurrentID;	//added NSM
	controlAttributes.Selected = wc.controlAttributes.Selected = FALSE;	//why false?
	
	for (int i = 0; i < sizeof(wc.controlAttributes.SpecWin); i++)
		controlAttributes.SpecWin[i] = wc.controlAttributes.SpecWin[i];

	for (int i = 0; i < sizeof(wc.controlAttributes.mOEPGroupNumbers) / sizeof(controlAttributes.mOEPGroupNumbers[0]); i++)
		controlAttributes.mOEPGroupNumbers[i] = wc.controlAttributes.mOEPGroupNumbers[i];

	controlAttributes.ulMaskGroupPermission = wc.controlAttributes.ulMaskGroupPermission;

	pPrev = wc.pPrev;
	pContainer = wc.pContainer;

	return *this;
}


void CWindowControl::Serialize (CArchive &ar)
{
	UCHAR* pBuff = (UCHAR*) &controlAttributes;

	if (ar.IsStoring())
	{
		//copy CStrings to TCHARS for serialization
		memset(controlAttributes.m_myCaption, 0, sizeof(controlAttributes.m_myCaption));
		memset(controlAttributes.m_myName, 0, sizeof(controlAttributes.m_myName));

		lstrcpyn(controlAttributes.m_myCaption ,myCaption, sizeof(controlAttributes.m_myCaption)/sizeof(controlAttributes.m_myCaption[0]) - 1);
		lstrcpyn(controlAttributes.m_myName, myName, sizeof(controlAttributes.m_myName)/sizeof(controlAttributes.m_myName[0]) - 1);

		//verify start signature to check against data corrupiton
		ASSERT(controlAttributes.signatureStart = 0xABCDEF87);

		ar<<sizeof(_tagWinControlAttributes);
		for(int i = 0; i < sizeof(_tagWinControlAttributes); i++){
			ar<<pBuff[i];
		}
	}
	else
	{
		ULONG mySize;
		memset(&controlAttributes,0,sizeof(_tagWinControlAttributes));
		ar>>mySize;

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (controlAttributes)) {
			xMySize = sizeof (controlAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}
		
		//verify start signature to check against data corrupiton
		ASSERT(controlAttributes.signatureStart = 0xABCDEF87);

		//copy TCHARs to CStrings 
		myCaption = controlAttributes.m_myCaption;
		myName = controlAttributes.m_myName;
	}
}


BEGIN_MESSAGE_MAP(CWindowControl, CWnd)
	//{{AFX_MSG_MAP(CWindowControl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowControl message handlers


BOOL CWindowControl::PopupWindow (CWindowControl *pParent)
{

	//create window if it hasnt already been created
	if(!m_hWnd){
		ASSERT (pParent);
		WindowCreate (pParent);
	}

	//if window exists, bring it to the top and show it
	if(m_hWnd) {
		ShowWindow(SW_SHOW);
		pPrev = pParent;
		BringWindowToTop();
		Invalidate(FALSE);
		UpdateWindow();
		if(controlAttributes.m_nType == CWindowControl::WindowContainer){
			SetForegroundWindow();
		}
	}
	return (m_hWnd != 0);
}

BOOL CWindowControl::PopdownWindow ()
{
	if (m_hWnd && ! controlAttributes.isVirtualWindow){
		// for new web browser control we can not just destroy
		// the window because there may be a web browser control
		// in it.  so for now we will just hide the window.  Aug-20-2013
//		DestroyWindow();
		ShowWindow(SW_HIDE);
		if (pPrev) {
			pPrev->ShowWindow(SW_SHOW);
			pPrev->BringWindowToTop();
			pPrev->Invalidate(FALSE);
			pPrev->UpdateWindow();
			if(pPrev->controlAttributes.m_nType == CWindowControl::WindowContainer){
				pPrev->SetForegroundWindow();
			}
			pPrev = 0;
		}
	}
	else if (m_hWnd ) {
		ShowWindow(SW_HIDE);
	}
	return TRUE;
}

ULONG CWindowControl::GetGroupPermission(){
	return controlAttributes.ulMaskGroupPermission;
}

void CWindowControl::SetGroupPermission(ULONG ulMaskGroupPermission){
	controlAttributes.ulMaskGroupPermission = ulMaskGroupPermission;
}

BOOL CWindowControl::WindowUpdateText (TCHAR *tcText, ULONG ulBgColor, ULONG ulTextColor)
{
	if (tcText) {
		myCaption = tcText;
		if (m_hWnd)
			SetWindowText (myCaption);
	}

	if (ulBgColor <= 0xFFFFFF)
		controlAttributes.m_colorFace = ulBgColor;

	if (ulTextColor <= 0xFFFFFF)
		controlAttributes.m_colorText = ulTextColor;

	if (m_hWnd) {
		Invalidate ();
		UpdateWindow ();
	}
	return TRUE;
}
