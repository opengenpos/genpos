/*-----------------------------------------------------------------------*\
:   Title          : FrameworkWndText.cpp : implementation file                                 
:   Category       : NHPOS Touchscreen Application 
:   Program Name   : FrameworkWndText.cpp                                       
:  ---------------------------------------------------------------------  
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------  
:  Abstract:            
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date     : Ver.Rev. :   Name       : Description
: 04/09/2015 : 02.02.01 : R.Chambers   : added font height and width cap for Text controls
* Sep-29-15  : 2.02.01  : R.Chambers   : added reflection of messages to customer display windows.
-------------------------------------------------------------------------*/

#include "stdafx.h"
#include <string.h>

#include "Framework.h"
#include "FrameworkWndText.h"
#include "FrameworkWndCtrl.h"
#include "FrameworkWndButton.h"
//#include "Container.h"
#include "Evs.h"
#include "vos.h"
#include "ECR.h"
//#include "..\BusinessLogic\INCLUDE\regstrct.h"
#include "..\BusinessLogic\INCLUDE\mld.h"
//#include "..\BusinessLogic\mld\INCLUDE\mldlocal.h"
#include "bl.h"
#include "rfl.h"
#include "termcfg.h"
#include "paraequ.h"
#include "para.h"
#include "ecr.h"
#include "appllog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UCHAR aOEPTableNo;                           // see also CFrameworkWndText::m_aszCurrentOEPTable
static UCHAR aszOEPGroups[STD_NUM_OEP_GROUPS + 1];  // see also CFrameworkWndText::m_aszCurrentOEPGroup
static int currentGroupCount = 0;

static bool useDefaultWindow = TRUE;
static CFrameworkWndItem	*pDefaultWndItem = NULL;
static CFrameworkWndItem	*pMatchedWndItem = NULL;
static int maxOEPButtons = STD_OEP_MAX_NUM;
/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////

// for event subsystem interface
#define ATTRIB_BLACK    0x00/*0x00*/ //colorpalette-changes
#define ATTRIB_WHITE    0x01/*0x07*/
#define ATTRIB_BLINK    0x0180

#define oldATTRIB_BLINK	0x80

// for cursor

#define NUMBER_UPPER    3
#define NUMBER_DOWN     3

#define MNEMO_CHAR      256

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndText
__declspec(dllexport)
BOOL CALLBACK BlFwSetOepGroup(UCHAR *pszGroups, UCHAR uchTableNo);
BOOL CALLBACK BlFwDisplayOEPImagery (UCHAR uchCommand, UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo);
int  CALLBACK BlFwGetMaxOepItems(UCHAR *pszMaxButtons);

//---------------------------------------------------------
CFrameworkWndTextSubWindow * CFrameworkWndTextSubWindow::SubWindowArray[40] = {0};
int CFrameworkWndTextSubWindow::SubWindowArrayIndex = 1;
int CFrameworkWndTextSubWindow::CurrentSubWindowArrayIndex = 0;

CStatic   CFrameworkWndText::m_OEPCustDisplayImage;      // used to display an image. LCDWIN_ID_REG104, CWindowTextType::TypeOEPCustDisplay

int CFrameworkWndText::CheckArenaOepTable (SHORT nIdentifier)
{
	return nIdentifier != LCDWIN_ID_REG104 || nIdentifier == LCDWIN_ID_REG104 && m_ArenaOepTableList.GetCount() > 0;
}


CFrameworkWndTextSubWindow::CFrameworkWndTextSubWindow (CFrameworkWndText  *pWndText, int idWindow, int idSubWindow)
{
	TRACE2("%S(%d): CFrameworkWndTextSubWindow::CFrameworkWndTextSubWindow().\n", __FILE__, __LINE__);
	myWindowId = idWindow;
	if (myWindowId == 0)
	{
		myWindowId = LCDWIN_ID_REG102;
	}

	myWndText = pWndText;

	mySubWindowId = idSubWindow;
	bIsEvsConnected = 0;

	memset (eachOEPGroupNum, 0, sizeof(eachOEPGroupNum));
	memset (mOEPGroupNumbers, 0, sizeof(mOEPGroupNumbers));

	// If this CFrameworkWndTextSubWindow has not been assigned a CFrameworkWndText
	// then we assume it is a temporary object that is being used for some kind
	// of OEP stuff.  For instance, see CFrameworkWndItem::WindowCreate() that
	// uses a temporary CFrameworkWndTextSubWindow object as part of creating
	// a dynamic PLU window with an OEP string.
	if (myWndText && mySubWindowId == 0)
	{
		ASSERT(pWndText->m_hWnd);

		mySubWindowId = SubWindowArrayIndex;
		SubWindowArrayIndex++;
		SubWindowArray[mySubWindowId] = this;
	}
}

CFrameworkWndTextSubWindow::~CFrameworkWndTextSubWindow ()
{
	TRACE3("%S(%d): CFrameworkWndTextSubWindow::~CFrameworkWndTextSubWindow().  mySubWindowId = %d\n", __FILE__, __LINE__, mySubWindowId);
	if (bIsEvsConnected && myWindowId && myWndText->m_hWnd) {
		EvsDisconnect(myWindowId, (ULONG)myWndText->m_hWnd);
	}
	SubWindowArray[mySubWindowId] = 0;
	bIsEvsConnected = 0;
	memset (eachOEPGroupNum, 0, sizeof(eachOEPGroupNum));
	memset (mOEPGroupNumbers, 0, sizeof(mOEPGroupNumbers));
	myWindowId = 0;
	mySubWindowId = 0;
	myWndText = 0;
}

SHORT  CFrameworkWndTextSubWindow::GetGroupNoFirst ()
{
	myGroupIndex = 0;

	// Make sure that this is synchronized with the way that the
	// array eachOEPGroupNum[] is being used in CompileOepString() below.
	if (eachOEPGroupNum[myGroupIndex] > 0)
		return (eachOEPGroupNum[myGroupIndex] - 1);
	else
		return 0;
}

SHORT  CFrameworkWndTextSubWindow::GetGroupNoNext ()
{
	myGroupIndex++;
	if (myGroupIndex >= sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0]))
		return -1;
	
	// Make sure that this is synchronized with the way that the
	// array eachOEPGroupNum[] is being used in CompileOepString() below.
	if (eachOEPGroupNum[myGroupIndex] > 0)
		return (eachOEPGroupNum[myGroupIndex] - 1);
	else
		return 0;
}

bool CFrameworkWndTextSubWindow::DoesMatchGroupNumber (int nGroupNo)
{
	// Make sure that this is synchronized with the way that the
	// array eachOEPGroupNum[] is being used in CompileOepString() below.
	if (nGroupNo < sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0])) {
		return (eachOEPGroupNum[nGroupNo] == nGroupNo + 1);
	} else {
		return 0;
	}
}


CFrameworkWndTextSubWindow *CFrameworkWndTextSubWindow::SearchForSubWindow (int nIdentifier, int nGroupNo)
{
	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i] && SubWindowArray[i]->DoesMatchGroupNumber(nGroupNo))
		{
			return SubWindowArray[i];
		}
	}
	return 0;
}

CFrameworkWndTextSubWindow *CFrameworkWndTextSubWindow::SearchForSubWindow (HWND hWnd)
{
	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i]->myWndText && SubWindowArray[i]->myWndText->m_hWnd == hWnd)
			{
				return SubWindowArray[i];
			}
		}
	}
	return 0;
}

int CFrameworkWndTextSubWindow::RemoveSubWindow (HWND hWnd)
{
	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i]->myWndText && SubWindowArray[i]->myWndText->m_hWnd == hWnd)
			{
				SubWindowArray[i] = 0;
				return 1;
			}
		}
	}
	return 0;
}

int CFrameworkWndTextSubWindow::RemoveSubWindow (CFrameworkWndText  *pWndText)
{
	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i]->myWndText == pWndText)
			{
				SubWindowArray[i] = 0;
				return 1;
			}
		}
	}
	return 0;
}

int CFrameworkWndTextSubWindow::ExistWithIdentifier (int nIdentifier)
{
	int nCount = 0;

	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i] && SubWindowArray[i]->myWindowId == nIdentifier)
			{
				nCount++;
			}
		}
	}
	return nCount;
}

int CFrameworkWndTextSubWindow::ExistWithIdentifierConnected (int nIdentifier)
{
	int nCount = 0;

	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i] && SubWindowArray[i]->myWindowId == nIdentifier && SubWindowArray[i]->bIsEvsConnected)
			{
				nCount++;
			}
		}
	}
	return nCount;
}


CFrameworkWndTextSubWindow *CFrameworkWndTextSubWindow::GetNextSubWindowWithIdentifier (int nIdentifier, int &nEnumeration)
{
	for (int i = nEnumeration; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i]->myWindowId == nIdentifier)
			{
				nEnumeration = i + 1;
				return SubWindowArray[i];
			}
		}
	}
	return 0;
}

CFrameworkWndText  *CFrameworkWndTextSubWindow::FetchLastCurrentFrameworkWndTextWindow ()
{
	CFrameworkWndTextSubWindow *pSubWindow = 0;
	CFrameworkWndText          *pTextWindow = 0;

	if (CurrentSubWindowArrayIndex > 0)
	{
		pSubWindow = SubWindowArray[CurrentSubWindowArrayIndex];

		ASSERT(pSubWindow);
		ASSERT(pSubWindow->myWndText);
		pTextWindow = pSubWindow->myWndText;
	}

	return pTextWindow;
}

void CFrameworkWndTextSubWindow::SetLastCurrentFrameworkWndTextWindow (CFrameworkWndText  *meThis)
{
	for (int i = 1; i < SubWindowArrayIndex; i++)
	{
		if (SubWindowArray[i])
		{
			if (SubWindowArray[i]->myWndText)
			{
				if (SubWindowArray[i]->myWndText == meThis)
				{
					CurrentSubWindowArrayIndex = i;
					break;
				}
			}
		}
	}
}


void CFrameworkWndTextSubWindow::InitializeSubWindowSystem ()
{
	TRACE2("%S(%d): CFrameworkWndTextSubWindow::InitializeSubWindowSystem().\n", __FILE__, __LINE__);
	SubWindowArrayIndex = 1;
	CurrentSubWindowArrayIndex = 0;
	for (int i = 0; i < sizeof(SubWindowArray)/sizeof(SubWindowArray[0]); i++)
	{
		if (SubWindowArray[i])
		{
			free (SubWindowArray[i]);
			SubWindowArray[i] = 0;
		}
	}
}


void CFrameworkWndTextSubWindow::SetOepString (TCHAR *tcsOepString)
{
	if (tcsOepString)
	{
		memcpy (mOEPGroupNumbers, tcsOepString, sizeof(mOEPGroupNumbers));
		memset (eachOEPGroupNum, 0, sizeof(eachOEPGroupNum));
	}
	else
	{
		memset (mOEPGroupNumbers, 0, sizeof(mOEPGroupNumbers));
		memset (eachOEPGroupNum, 0, sizeof(eachOEPGroupNum));
	}
}

void CFrameworkWndTextSubWindow::CompileOepString (TCHAR *tcsOepString)
{
	if (tcsOepString)
	{
		SetOepString (tcsOepString);
	}

	// Compile the OEP string by parsing it and putting the required pieces
	// into the eachOEPGroupNum array.  Examples of the kinds of strings to
	// be compiled are as follows:
	//		"0"				-> this is the default group or dummy group used with Auto Coupons.
	//		"4"				-> this is an OEP window for a single group only
	//		"4-10"			-> this is an OEP window for a range of groups
	//		"4,6,8,25"		-> this is an OEP window for several, non-contiguous groups
	//		"1-5,9,12-14"	-> this is OEP window for several ranges and non-contiguous groups
	//		"5,10-"			-> this is OEP window for single group plus a range to end of possible values
	//
	// We have to take care of the special case of the 0 (zero) group which designates the
	// default OEP window.  We do this by putting into the eachOEPGroupNum [] array the
	// group number plus 1.  Then when we do searches in the array for a particular group
	// number, we have to add one to the specified group number to compare against the
	// values in the array.  See function DoesMatchGroupNumber() above.
	TCHAR *pFirstDigit = 0, *pLastDigit = 0, myDigits1[20], myDigits2[20];
	int   nValue1 = 0, nValue2 = 0;
	int   nRange = 0;
	for (TCHAR *pString = tcsOepString; *pString;)
	{
		switch (*pString)
		{
			case _T(','):
				if (! pFirstDigit)
					break;
				memset (myDigits2, 0, sizeof (myDigits2));

//				ASSERT((UCHAR *)pLastDigit >= (UCHAR *)pFirstDigit + sizeof(TCHAR));

				memmove (myDigits2, pFirstDigit, (UCHAR *)pLastDigit - (UCHAR *)pFirstDigit + sizeof(TCHAR));
				nValue2 = _ttoi (myDigits2);
				if (nValue2 >= sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0])) {
					nValue2 = sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0]) - 1;
				}
				pFirstDigit = pLastDigit = 0;
				if (nRange)
				{
					for (int i = nValue1; i <= nValue2; i++)
					{
						eachOEPGroupNum[i] = i + 1;
					}
					nRange = 0;
				}
				else
				{
					eachOEPGroupNum[nValue2] = nValue2 + 1;
				}
				pString++;
				break;

			case _T('-'):
				memset (myDigits1, 0, sizeof (myDigits1));
				if (!pFirstDigit)
				{
					pFirstDigit = pLastDigit = _T("0");
				}

//				ASSERT((UCHAR *)pLastDigit >= (UCHAR *)pFirstDigit + sizeof(TCHAR));

				memmove (myDigits1, pFirstDigit, (UCHAR *)pLastDigit - (UCHAR *)pFirstDigit + sizeof(TCHAR));
				nValue1 = _ttoi (myDigits1);
				pFirstDigit = pLastDigit = 0;
				nRange = 1;
				pString++;
				break;

			default:
				if (_istdigit(*pString))
				{
					if ( ! pFirstDigit )
					{
						pFirstDigit = pString;
						pLastDigit = pString;
					}
					else
					{
						pLastDigit = pString;
					}
				}
				else
				{
					// Ignore anything else
				}
				pString++;
				break;
		}
	}

	// If we have reached the end of string, we need to ensure that if there are any
	// expresions that we are in the middle of compiling, that we complete the procedure.
	// First check to see if we have any digits to process.  If so then we will either
	// enter a single value or a range depending on whether the range indicator is turned on.
	// We also need to see if we have the beginning of a range without an end, an open ended
	// range which means from the beginning point to the end of the possible values.
	if (pFirstDigit)
	{
		memset (myDigits2, 0, sizeof (myDigits2));

//		ASSERT((UCHAR *)pLastDigit >= (UCHAR *)pFirstDigit + sizeof(TCHAR));

		memmove (myDigits2, pFirstDigit, (UCHAR *)pLastDigit - (UCHAR *)pFirstDigit + sizeof(TCHAR));
		nValue2 = _ttoi (myDigits2);
		if (nValue2 >= sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0])) {
			nValue2 = sizeof (eachOEPGroupNum) / sizeof(eachOEPGroupNum[0]) - 1;
		}
		pFirstDigit = pLastDigit = 0;
		if (nRange)
		{
			for (int i = nValue1; i <= nValue2; i++)
			{
				eachOEPGroupNum[i] = i + 1;
			}
			nRange = 0;
		}
		else
		{
			eachOEPGroupNum[nValue2] = nValue2 + 1;
		}
	}
	else
	{
		if (nRange)
		{
			for (int i = nValue1; i < sizeof(eachOEPGroupNum)/sizeof(eachOEPGroupNum[0]); i++)
			{
				eachOEPGroupNum[i] = i + 1;
			}
			nRange = 0;
		}
	}
}


static CFrameworkWndText  *DetermineFrameworkWndTextWindow (int nIdentifier, CFrameworkWndText  *meThis)
{
    if(nIdentifier == LCDWIN_ID_REG102 || nIdentifier == LCDWIN_ID_REG209)
	{
		int calledOEPGroup = 0;
		// There are several different special OEP windows used for specific types of data.
		//   BL_OEPGROUP_STR_PREAUTH      201, display of Preauth Tenders in function ItemDisplayPreauthChooseDialog(), SLOEP_SPL_ASK
		//   BL_OEPGROUP_STR_REASONCODE   202, display of Reason Codes in function ItemDisplayOepChooseDialog()
		//   BL_OEPGROUP_STR_AUTOCOUPON   203, display of Auto Coupon in function ItemAutoCouponCheck()
		//   BL_OEPGROUP_STR_OPENCHECK    204, display of Open Guest Checks in function TrnDisplayFetchGuestCheck()
		while(aszOEPGroups[currentGroupCount] > SLOEP_BEGIN_SPL && aszOEPGroups[currentGroupCount] < SLOEP_SPL_ASK && currentGroupCount < sizeof(aszOEPGroups)/sizeof(aszOEPGroups[0]))
		{
			currentGroupCount++;
		}

		if (currentGroupCount < sizeof(aszOEPGroups)/sizeof(aszOEPGroups[0]))
		{
			// the OEP Group being requested, this is used to find 
			//	which OEP window has been designed for that group
			calledOEPGroup = aszOEPGroups[currentGroupCount];
		}

		CFrameworkWndTextSubWindow *pSubWindow = CFrameworkWndTextSubWindow::SearchForSubWindow (nIdentifier, calledOEPGroup);
		if ( pSubWindow == 0)
		{
			pSubWindow = CFrameworkWndTextSubWindow::SearchForSubWindow (nIdentifier, 0);
		}

//		ASSERT(pSubWindow);
//		ASSERT(pSubWindow->myWndText);

		if (pSubWindow && pSubWindow->myWndText)
		{
			meThis = pSubWindow->myWndText;
		}
		if (meThis) {
			meThis->m_aszCurrentOEPGroup = aszOEPGroups[currentGroupCount];
			meThis->m_aszCurrentOEPTable = aOEPTableNo;
		}
	} else if (nIdentifier == LCDWIN_ID_REG104) {
		if (CFrameworkWndText::m_OEPCustDisplayImage.m_hWnd) {
			meThis = CFrameworkWndText::GetOepCustDisplayFrameworkWndText(0);
		}

		if (meThis) {
			meThis->m_aszCurrentOEPGroup = aszOEPGroups[currentGroupCount];
			meThis->m_aszCurrentOEPTable = aOEPTableNo;
		}
	}

//	ASSERT(meThis);
	return meThis;
}

SHORT CFrameworkWndTextSubWindow::EvsConnectSubWindow ()
{
	SHORT sRetStatus = EVS_OK;

	TRACE2("%S(%d): SHORT CFrameworkWndTextSubWindow::EvsConnectSubWindow ()\n", __FILE__, __LINE__);
	if (! ExistWithIdentifierConnected(myWindowId))
	{
		TRACE3("%S(%d): CFrameworkWndTextSubWindow::EvsConnectSubWindow()  ::EvsConnect %d.\n", __FILE__, __LINE__, myWindowId);
		sRetStatus = ::EvsConnect(myWindowId, (ULONG)myWndText->m_hWnd);
		if (sRetStatus == EVS_OK)
		{
			bIsEvsConnected = 1;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	return sRetStatus;
}

SHORT CFrameworkWndTextSubWindow::EvsDisconnectSubWindow ()
{
	SHORT sRetStatus = EVS_OK;

	TRACE2("%S(%d): SHORT CFrameworkWndTextSubWindow::EvsDisconnectSubWindow ()\n", __FILE__, __LINE__);
	if (bIsEvsConnected)
	{
		TRACE3("%S(%d): CFrameworkWndTextSubWindow::EvsDisconnectSubWindow()  ::EvsDisconnect %d.\n", __FILE__, __LINE__, myWindowId);
		sRetStatus = EvsDisconnect(myWindowId, (ULONG)myWndText->m_hWnd);
	}

	bIsEvsConnected = 0;

	return sRetStatus;
}



//---------------------------------------------------------

RECT  CFrameworkWndText::m_myMonitors[5];
int   CFrameworkWndText::m_myMonitorsIndex = 0;

BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	if (CFrameworkWndText::m_myMonitorsIndex < 5) {
		CFrameworkWndText::m_myMonitors[CFrameworkWndText::m_myMonitorsIndex] = *lprcMonitor;
		CFrameworkWndText::m_myMonitorsIndex++;
	}
	return TRUE;
}

int CFrameworkWndText::EnumDisplayMonitors (void)
{
	m_myMonitorsIndex = 0;

	::EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

	return m_myMonitorsIndex;
}


//CFrameworkWndText Constructor
CFrameworkWndText::CFrameworkWndText (int id, int row, int column, int width, int height) :
	CWindowText (id, row, column, width, height), m_nButtonCount(0)
{
	m_aszCurrentOEPGroup = 0;
	m_aszCurrentOEPTable = 0;

	m_pchText = 0;
	m_pchAttr = 0;
	m_pchLatest = 0;
	m_pchWork = 0;
	m_nIdentifier = id;
	controlAttributes.m_nRow = row;
	controlAttributes.m_nColumn = column;
	controlAttributes.m_usWidthMultiplier = width;
	controlAttributes.m_usHeightMultiplier = height;
	textAttributes.m_nLeft = 0;
	textAttributes.m_nTop = 0;
    m_nFontWidth   = controlAttributes.m_nWidth  / controlAttributes.m_nColumn; // temporary width of font
    m_nFontHeight  = controlAttributes.m_nHeight / controlAttributes.m_nRow;    // temporary height of font
    m_bLCursor     = FALSE;
    m_ptLCursor.x  = 0;
    m_ptLCursor.y  = 0;
    m_colorBG      = RGB(0xff, 0xff, 0xff); // white
	//These sizes should be based on m_nRowsInCtl and m_nColumnsInCtl
	//which depend on which text control it is
	//m_nRow and m_nColumn only specify the location
    m_pchText      = new TCHAR [controlAttributes.m_nRow * controlAttributes.m_nColumn];    // may be UNICODE
    m_pchAttr      = new TCHAR [controlAttributes.m_nRow * controlAttributes.m_nColumn];
    m_pchLatest    = new TCHAR [controlAttributes.m_nRow * controlAttributes.m_nColumn * 2];// with attribute
    m_pchWork      = new TCHAR [controlAttributes.m_nRow * controlAttributes.m_nColumn * 2];// with attribute
    ClearBuffer();
}

//CFrameworkWndText Constructor from a CWindowControl reference
CFrameworkWndText::CFrameworkWndText(CWindowControl *wc) :
	CWindowText(wc), m_nButtonCount(0)
{
	m_pchText = 0;
	m_pchAttr = 0;
	m_pchLatest = 0;
	m_pchWork = 0;
	m_sName = wc->myName;
    m_bLCursor     = FALSE;
    m_ptLCursor.x  = 0;
    m_ptLCursor.y  = 0;
	m_colorBG = wc->controlAttributes.m_colorFace;
	m_colorText = wc->controlAttributes.m_colorText;
}
//CFrameworkWndText Destructor
CFrameworkWndText::~CFrameworkWndText()
{
	TRACE3("%S(%d): CFrameworkWndText::~CFrameworkWndText() destructor called.  m_nIdentifier = %d\n", __FILE__, __LINE__, m_nIdentifier);

	if(m_pchText)
		delete [] m_pchText;
	if(m_pchAttr)
		delete [] m_pchAttr;
	if(m_pchLatest)
		delete [] m_pchLatest;
	if(m_pchWork)
		delete [] m_pchWork;

	m_Font.DeleteObject();
	m_Brush.DeleteObject();

	// The OnDestroy () for CFrameworkWndText is not being executed so
	// we do some cleanup here.  However the following code is not working
	// correctly so we need to determine what is going on.
	if (m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209)
	{
		CFrameworkWndTextSubWindow * pSubWind = CFrameworkWndTextSubWindow::SearchForSubWindow (m_hWnd);
		if (pSubWind)
		{
			TRACE2("%S(%d): pSubWind->EvsDisconnectSubWindow().\n", __FILE__, __LINE__);
			pSubWind->EvsDisconnectSubWindow ();
			//we remove the sub window from the subwindow array
			//so that when we are going through the SearchForSubWindow
			//function we dont get an exception if we get to a subwindow
			//that has already been destroyed. JHHJ
			pSubWind->RemoveSubWindow (this);
			free(pSubWind);
			TRACE2("%S(%d):  After free(pSubWind).\n", __FILE__, __LINE__);
		}
		else {
			TRACE2("%S(%d): CFrameworkWndText::~CFrameworkWndText() SearchForSubWindow() returned NULL.\n", __FILE__, __LINE__);
		}

		// Finally we need to check to see if there are any buttons that we have
		// not yet destroyed.  We do this check in order to eliminate any CFrameworkWndButton
		// objects that may be attached to an OEP window which have not yet been deleted.
		// This helps us to eliminate any memory leaks.
		CFrameworkWndItem   *pParentWndItem = NULL;
		CFrameworkWndButton *pChildWndButton;

		pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
		if (pParentWndItem) {
			if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_BACK)){	//More/Back (up)
				delete pChildWndButton;
			}
			if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_MORE)){	//Continue More (down)
				delete pChildWndButton;
			}
			if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_DONE)){  //DONE
				delete pChildWndButton;
			}
			for(int l = 0; l < m_nButtonCount; l++){
				if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_START + l)){
					delete pChildWndButton;
				}
			}
		}
		TRACE2("%S(%d):  After remaining button deletes.\n", __FILE__, __LINE__);
	}
	else
	{
		EvsDisconnect(m_nIdentifier, (ULONG)m_hWnd);
	}

	//DestroyWindow();
}

//////////////////////////////////////////////////////////////////////////////
//
// Function:    clear each buffer
//
// Prototype:   void        CFrameworkWndText::ClearBuffer();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
// Purpose: Clears the text display buffers to blank (spaces) and resets the color
//				attributes of all characters in all buffers.
//
/////////////////////////////////////////////////////////////////////////////
void CFrameworkWndText::ClearBuffer()
{
    LONG    nChars;
    TCHAR*  pchText   = m_pchText;
    TCHAR*  pchAttr   = m_pchAttr;
    TCHAR*  pchLatest = m_pchLatest;
    TCHAR*  pchWork   = m_pchWork;

    for (nChars = 0; nChars < m_nRowsInCtl * m_nColumnsInCtl; nChars++)
    {
        *pchText++   = _T(' ');             // space character (maybe UNICODE)
        *pchAttr++   = ATTRIB_BLACK;        // attribute
        *pchLatest++ = _T(' ');                 // space character
        *pchLatest++ = ATTRIB_BLACK;        // attribute
//#ifdef _DEBUG
        *pchWork++   = _T(' ');                 // space character
        *pchWork++   = ATTRIB_BLACK;        // attribute
//#endif
    }
}

/*CFrameworkWndText & CFrameworkWndText::operator = (const CDButtonEdit & cdt)
{
	myCaption = cdt.m_csCaption;
	m_colorText = cdt.colorText;
	m_colorFace = cdt.colorFace;
	m_myShape = cdt.myShape;
	m_myPattern = cdt.myPattern;
	m_myFontSize = cdt.myFontSize;
	//myAction = cdt.myAction;
	//memcpy (&myActionUnion, &cdt.myActionUnion, sizeof (myActionUnion));
	m_usWidthMultiplier = cdt.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.m_usHeightMultiplier;

	return *this;
}*/


BEGIN_MESSAGE_MAP(CFrameworkWndText, CWindowText)
	//{{AFX_MSG_MAP(CFrameworkWndText)
	ON_WM_PAINT()
	ON_WM_CREATE()
    ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WU_EVS_CREATE, OnEvsCreate)
    ON_MESSAGE(WU_EVS_DELETE, OnEvsDelete)
    ON_MESSAGE(WU_EVS_UPDATE, OnEvsUpdate)
    ON_MESSAGE(WU_EVS_CURSOR, OnEvsCursor)
    ON_MESSAGE(WU_EVS_GETPARAM, OnEvsGetParam)
    ON_MESSAGE(WU_EVS_PUTPARAM, OnEvsPutParam)
    ON_MESSAGE(WM_APP_OEP_IMAGERY, OnOepImageryChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndText message handlers

/////////////////////////////////////////////////////////////////////////////
//
// Function:    create message from event subsystem
//
// Prototype:   LRESULT     CFrameworkWndText::OnEvsCreate(wParam, lParam);
//
// Inputs:      WPARAM      wParam;     -
//              LPARAM      lParam;     -
//
// Outputs:     LRESULT     lResult;    - result code, always 0
//
// Purpose: Clears the buffer for this item, shows the text window
//			The parent window contains the actual text window so if
//			we are a keyboard system using text windows, then we need
//			to show both the parent and ourselves, the text window, no
//			matter what identifier we are.
//
//			If we are touch system then the OEP window is no longer a
//			text window but is instead a window with buttons.  Therefore
//			we need to hide the text window.  We also will allow the
//			DisplayPopup () function to display the parent when it is
//			starting the popup display with row zero or the first button.
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFrameworkWndText::OnEvsCreate(WPARAM wParam, LPARAM lParam)
{
	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

	if (myThis) {
		myThis->ClearBuffer();
		if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH &&
			(m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209)) {
			m_bBackOEP = FALSE;
			myThis->ShowWindow(SW_HIDE);
			CFrameworkWndTextSubWindow::SetLastCurrentFrameworkWndTextWindow (myThis);
			BlRestrictButtonPressMask (BL_BUTTONRESTRICT_OP_ENABLE, BL_BUTTONRESTRICT_OEP_ONLY);
		} else {
			// show/hide of this window and its container are both handled here.
			// the various kinds of CFrameworkWndText controls are housed within a
			// CFrameworkWndItem window so that a single CFrameworkWndItem window may
			// contain several CFrameworkWndText controls.
			CWindowControl *hCWnd = myThis->pContainer;
			if (hCWnd) {
				int xPos = hCWnd->controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth;
				int yPos = hCWnd->controlAttributes.m_nRow * CFrameworkWndButton::stdHeight;
				if ((hCWnd->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_DISPLAYTRANS) != 0) {
					// if we are to be using the second monitor for this window then check to see if there
					// actually is a second monitor.  if so move the window and display it. if not, hide the window.
					// in method CWindowItem::WindowCreate() we create the containing CFrameworkWndItem window with
					// the extended style of WS_EX_TOPMOST if it is to be displayed on the second monitor so that
					// if the second monitor is being used for other things such as advertising, our customer display
					// will not be covered over.
					if (CFrameworkWndText::m_myMonitorsIndex > 1 && CheckArenaOepTable (m_nIdentifier)) {
						xPos += GetSystemMetrics (SM_CXSCREEN);
						::SetWindowPos (hCWnd->m_hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
						hCWnd->ShowWindow(SW_SHOW);
					} else {
						hCWnd->ShowWindow(SW_HIDE);
					}
				} else {
					hCWnd->ShowWindow(SW_SHOW);
					hCWnd->BringWindowToTop();
					if(m_nIdentifier == LCDWIN_ID_REG102 ||	m_nIdentifier == LCDWIN_ID_REG209) {
						CFrameworkWndTextSubWindow::SetLastCurrentFrameworkWndTextWindow (myThis);
					}
				}
			}
			else {
				ASSERT(0);
			}
			myThis->ShowWindow(SW_SHOW);
			myThis->BringWindowToTop();
		}
	}
	else {
		ASSERT(0);
	}
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    delete message from event subsystem
//
// Prototype:   LRESULT     CFrameworkWndText::OnEvsDelete(wParam, lParam);
//
// Inputs:      WPARAM      wParam;     -
//              LPARAM      lParam;     -
//
// Outputs:     LRESULT     lResult;    - result code, always 0
//
// Purpose:	Hides the window whose text no longer should be shown.
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFrameworkWndText::OnEvsDelete(WPARAM wParam, LPARAM lParam)
{
	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

	// If the EvsDelete message concerns a Popup window such as OEP window
	// then we need to delete the popup if we are running as Touch Screen.
	// If we are not Touch Screen, then there won't be a popup to delete.
	// With keyboard systems, the displayed OEP window is just a text window
	// with the OEP text and is not a window full of buttons.
	//
	// If the EvsDelete message concerns a Popup window or a Supervisor/Program
	// mode window, then we will do a hide of the parent window.
	// Otherwise we will just hide this text window (Receipt, etc.).
	if(m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209)
	{
		// FetchLastcurrentFrameworkWndTextWindow () may return a NULL pointer if there is no
		// last current window.  If it does, then we use the window we have.
		CFrameworkWndText *xThis = CFrameworkWndTextSubWindow::FetchLastCurrentFrameworkWndTextWindow();
		if (xThis)
			myThis = xThis;
		if (myThis && myThis->GetParent()) {
			// We will hide the displayed OEP Window and then clean up any buttons.
			// We found that without the DeletePopup() we ended up with a resource leak
			// as reported by C++ Runtime when GenPOS is terminated with AC999 from Supervisor Mode.
			myThis->GetParent()->ShowWindow(SW_HIDE);
			myThis->DeletePopup();
		}
		BlRestrictButtonPressMask (BL_BUTTONRESTRICT_OP_DISABLE, BL_BUTTONRESTRICT_OEP_ONLY);
	} else if(m_nIdentifier == LCDWIN_ID_REG104){
		if (myThis && myThis->GetParent() && myThis->m_CurrentArenaEntry.stickydisplay == 0) {
			// Hide this displayed Customer OEP Image window and then transfer ownership
			// of the image to a GDI Object so we can delete the image.
			// for discussion about CImage and using images with CStatic see posting at URL:
			// https://stackoverflow.com/questions/51086298/lifecycle-and-resource-management-when-using-cimage-with-cstatic-to-display-chan
			myThis->GetParent()->ShowWindow(SW_HIDE);
			CGdiObject myObj;
			myObj.Attach(myThis->m_OEPCustDisplayImage.SetBitmap(NULL));
		}
	} else if(m_nIdentifier == LCDWIN_ID_SUPER300){
		if (myThis && myThis->GetParent())
			myThis->GetParent()->ShowWindow(SW_HIDE);
	}else{
		if (myThis) {
			myThis->ShowWindow(SW_HIDE);
#if 0
			// the following will cause the CFrameworkWndItem container of this
			// CFrameworkWndText window to also hide. this behavior would be new
			// and any layout which has the lead thru in the same container as
			// the receipt window would see the lead thru not be available for
			// Sign-in prompt.  R.Chambers, Sep 28, 2015.
			if (myThis->GetParent())
				myThis->GetParent()->ShowWindow(SW_HIDE);
#endif
		}
	}

    // exit ...

    return 0;
}

/*
	OnEvsGetParam - get a window parameter from the framework.

	wParam - contains the indicator as to which parameter to get.

	lParam - contains the address of where to put the parameter.
 */
LRESULT CFrameworkWndText::OnEvsGetParam(WPARAM wParam, LPARAM lParam) /* used to get button dimensions from Framework - CSMALL */
{
	// Identifer LCDWIN_ID_REG102 is for OEP, CWindowText::TypeOEP
	// Identifier LCDWIN_ID_REG104 is for OEP Customer Display
	if(m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209 || m_nIdentifier == LCDWIN_ID_REG104){
		CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

		if (myThis) {
			switch (wParam)
			{
				case 1:
					*((USHORT *)lParam) = (PifSysConfig()->uchKeyType == KEYBOARD_TOUCH) ? 1 : 0;
					if (*((USHORT *)lParam)) {
						maxOEPButtons = myThis->textAttributes.oepBtnAcross * 
										myThis->textAttributes.oepBtnDown - 
										2 * myThis->textAttributes.oepBtnAcross;
						*(((USHORT *)lParam) + 1) = myThis->textAttributes.oepBtnAcross;
						*(((USHORT *)lParam) + 2) = myThis->textAttributes.oepBtnDown;
					}
					else {
						ASSERT(0);
					}
					break;

				default:
					break;
			}
		}
	}
    return 0;
}

// used to handle messages concerning the display of OEP associated imagery
// as implemented for Suggestive Selling.
// OnOepImageryChange() is the handler for the WM_APP_OEP_IMAGERY message.
LRESULT CFrameworkWndText::OnOepImageryChange(WPARAM wParam, LPARAM lParam)
{
//	int nId = LCDWIN_ID_REG104;

//	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

//	if (myThis && myThis->GetParent()) {
		// Hide this displayed Customer OEP Image window and then transfer ownership
		// of the image to a GDI Object so we can delete the image.
		// for discussion about CImage and using images with CStatic see posting at URL:
		// https://stackoverflow.com/questions/51086298/lifecycle-and-resource-management-when-using-cimage-with-cstatic-to-display-chan

		// we get the parent of the static control used to display the image, a CFrameworkWndText object.
		// then we get the parent of the static control's parent, a CFrameworkWndItem object.
		// it is the CFrameworkWndItem object which is the actual window container that we want to hide
		// since it is the CFrameworkWndItem object which will be shown.
	switch (wParam) {
		case 0:
			if (CFrameworkWndText::m_OEPCustDisplayImage.m_hWnd) {
				CWnd * p1 = CFrameworkWndText::m_OEPCustDisplayImage.GetParent();
				CWnd *p2 = p1->GetParent();

				p2->ShowWindow (SW_HIDE);         // hide the window containing m_OEPCustDisplayImage

				CGdiObject myObj;
				myObj.Attach(CFrameworkWndText::m_OEPCustDisplayImage.SetBitmap(NULL));
			}
			break;
		case 1:
			if (CFrameworkWndText::m_OEPCustDisplayImage.m_hWnd && m_ArenaOepTableList.GetCount() > 0) {
				CWnd * p1 = CFrameworkWndText::m_OEPCustDisplayImage.GetParent();
				CWnd *p2 = p1->GetParent();

				p2->ShowWindow (SW_SHOW);         // show the window containing m_OEPCustDisplayImage
			}
			break;
		case 2:
			if (CFrameworkWndText::m_OEPCustDisplayImage.m_hWnd && m_ArenaOepTableList.GetCount() > 0) {
				CWnd * p1 = CFrameworkWndText::m_OEPCustDisplayImage.GetParent();
				CWnd *p2 = p1->GetParent();

				CWindowControl *hCWnd = (CWindowControl *)p2;

				if (hCWnd) {
					int xPos = hCWnd->controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth;
					int yPos = hCWnd->controlAttributes.m_nRow * CFrameworkWndButton::stdHeight;
					if ((hCWnd->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_DISPLAYTRANS) != 0) {
						// if we are to be using the second monitor for this window then check to see if there
						// actually is a second monitor.  if so move the window and display it. if not, hide the window.
						// in method CWindowItem::WindowCreate() we create the containing CFrameworkWndItem window with
						// the extended style of WS_EX_TOPMOST if it is to be displayed on the second monitor so that
						// if the second monitor is being used for other things such as advertising, our customer display
						// will not be covered over.
						if (CFrameworkWndText::m_myMonitorsIndex > 1 && CheckArenaOepTable (m_nIdentifier)) {
							xPos += GetSystemMetrics (SM_CXSCREEN);
							::SetWindowPos (hCWnd->m_hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
							hCWnd->ShowWindow(SW_SHOW);
						} else {
							hCWnd->ShowWindow(SW_HIDE);
						}
					} else {
						hCWnd->ShowWindow(SW_SHOW);
						hCWnd->BringWindowToTop();
					}
				}

				int  iDeptNo = (lParam >> 16 & 0x00ff);
				int  iTableNo = (lParam >> 8 & 0x00ff);
				int  iGroupNo = (lParam & 0x00ff);

				m_aszCurrentOEPGroup = iGroupNo;
				m_aszCurrentOEPTable = iTableNo;

				if (ClearAndShowPopupWithImage()) {
					// we need to show the CStatic control that contains the image as well
					// as the CFrameworkText window which contains the CStatic control as well
					// as the CFrameworkItem window which contains the CFrameworkText window.
					CFrameworkWndText::m_OEPCustDisplayImage.ShowWindow (SW_SHOW);
					p1->ShowWindow (SW_SHOW);         // show the CFrameworkText window containing the CStatic
					p2->ShowWindow (SW_SHOW);         // show the CFrameworkItem window containing the CFrameworkText
				}
			}

			break;
	}
    return 0;
}

/*
	OnEvsGetParam - get a window parameter from the framework.

	wParam - contains the indicator as to which parameter to get.

	lParam - contains the address of where to put the parameter.
 */
LRESULT CFrameworkWndText::OnEvsPutParam(WPARAM wParam, LPARAM lParam) /* used to get button dimensions from Framework - CSMALL */
{
	//  following variables defined to allow for setting of conditional breakpoints.
	//  condition of myId == myCheck will cause a conditional break for messages for
	//  the window type we are looking for.
	int  myId = m_nIdentifier;
	int  myCheck = LCDWIN_ID_REG104;

	// Identifer LCDWIN_ID_REG102 is for OEP
	if(m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209 || m_nIdentifier == LCDWIN_ID_REG104)
	{
		TCHAR  tszTempText[64];
		VOSPARAM  *pVosParam = (VOSPARAM *)lParam;
		TCHAR  *tcszDisplayBackCommand = MLD_DISPLAYFLAG_DTXT_BACK;
		TCHAR  *tcszDisplayMoreCommand = MLD_DISPLAYFLAG_DTXT_MORE;
		TCHAR  *tcszDisplayDoneCommand = MLD_DISPLAYFLAG_DTXT_DONE;
		TCHAR  *tcszRemoveBackCommand = MLD_DISPLAYFLAG_RTXT_BACK;
		TCHAR  *tcszRemoveMoreCommand = MLD_DISPLAYFLAG_RTXT_MORE;
		TCHAR  *tcszRemoveDoneCommand = MLD_DISPLAYFLAG_RTXT_DONE;

		CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);
		CFrameworkWndLabel *pChildWndLabel = NULL;
		CFrameworkWndItem  *pParentWndItem = NULL;

		// myThis points to the actual OEP window that we need to update.
		// The function DetermineFrameworkWndTextWindow() determines or finds the OEP window that
		// we are supposed to use for this OEP group.  It may be a group specific OEP window
		// or it may be the default, generic OEP window.
		if (myThis) {
			switch (wParam)
			{
				case 1:
					if (m_nIdentifier == LCDWIN_ID_REG104) break;   // do not do updates of the OEP Customer Display

					if (pVosParam->tchVosString[0] == _T('@')) {
						if (_tcsncmp (pVosParam->tchVosString, tcszDisplayBackCommand, _tcslen(tcszDisplayBackCommand)) == 0) {
							myThis->DisplayPopupControlsBack (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszDisplayMoreCommand, _tcslen(tcszDisplayMoreCommand)) == 0) {
							myThis->DisplayPopupControlsMore (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszDisplayDoneCommand, _tcslen(tcszDisplayDoneCommand)) == 0) {
							myThis->DisplayPopupControlsDone (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveBackCommand, _tcslen(tcszRemoveBackCommand)) == 0) {
							myThis->DisplayPopupControlsBack (FALSE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveMoreCommand, _tcslen(tcszRemoveMoreCommand)) == 0) {
							myThis->DisplayPopupControlsMore (FALSE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveDoneCommand, _tcslen(tcszRemoveDoneCommand)) == 0) {
							myThis->DisplayPopupControlsDone (FALSE);
						}
					} else {
						_tcsncpy(tszTempText, pVosParam->tchVosString, 63);
						tszTempText[63] = 0;  // force end of string in case not set by _tcsncpy()

						myThis->DisplayPopup(pVosParam->usRow, tszTempText);
					}
					break;
				case 2:
					if (m_nIdentifier == LCDWIN_ID_REG104)
						myThis->ClearAndShowPopupWithImage();
					else
						myThis->ClearAndShowPopup();
					break;
				case 3:
					if (myThis->GetParent())
						myThis->GetParent()->ShowWindow (SW_HIDE);
					break;
				case 4:
					if (m_nIdentifier == LCDWIN_ID_REG104) break;   // do not do updates of the OEP Customer Display

					if (pVosParam->tchVosString[0] == _T('@')) {
						if (_tcsncmp (pVosParam->tchVosString, tcszDisplayBackCommand, _tcslen(tcszDisplayBackCommand)) == 0) {
							myThis->DisplayPopupControlsBack (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszDisplayMoreCommand, _tcslen(tcszDisplayMoreCommand)) == 0) {
							myThis->DisplayPopupControlsMore (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszDisplayDoneCommand, _tcslen(tcszDisplayDoneCommand)) == 0) {
							myThis->DisplayPopupControlsDone (TRUE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveBackCommand, _tcslen(tcszRemoveBackCommand)) == 0) {
							myThis->DisplayPopupControlsBack (FALSE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveMoreCommand, _tcslen(tcszRemoveMoreCommand)) == 0) {
							myThis->DisplayPopupControlsMore (FALSE);
						} else if (_tcsncmp (pVosParam->tchVosString, tcszRemoveDoneCommand, _tcslen(tcszRemoveDoneCommand)) == 0) {
							myThis->DisplayPopupControlsDone (FALSE);
						}
					} else {
						_tcsncpy(tszTempText, pVosParam->tchVosString, 63);
						tszTempText[63] = 0;  // force end of string in case not set by _tcsncpy()

						pParentWndItem = (CFrameworkWndItem *)GetParent();
						if (pParentWndItem != 0) {
							if(pChildWndLabel = (CFrameworkWndLabel*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_LABEL)){	//More/Back (up)
								pChildWndLabel->SetWindowText (tszTempText);
							}
						}
					}
					break;
				case 5:
					break;
				default:
					break;
			}
		}
	}

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    update a data from event subsystem
//
// Prototype:   LRESULT     CFrameworkWndText::OnEvsUpdate(wParam, lParam);
//
// Inputs:      WPARAM      wParam;     -
//              LPARAM      lParam;     -
//
// Outputs:     LRESULT     lResult;    - result code
//
// Puropse: Receives characters to display in text area, handles all display operations
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFrameworkWndText::OnEvsUpdate(WPARAM wParam, LPARAM lParam)
{
    SHORT   sResult;
	TCHAR   tszTempText [64];
    LONG    nRow, nColumn;
    RECT    rect;
	BOOL	bPopupWindow = FALSE;
	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

    // is my message ?
    if (wParam != (WPARAM)m_nIdentifier)    // invalid
    {
		ASSERT(0);
        return 0;                           // exit ...
    }

	if (myThis == 0) {
		ASSERT(0);
		return 0;
	}
    // get data from event subsystem

    sResult = ::EvsGetData(
                    m_nIdentifier,
                    (ULONG)m_hWnd,
                    myThis->m_pchWork,
                    myThis->m_nRowsInCtl * myThis->m_nColumnsInCtl * 2,
                    NULL);

    if (sResult != EVS_OK)
    {
        myThis->ShowWindow(SW_HIDE);
        return 0;                               // exit ...
    }

	if (m_nIdentifier == LCDWIN_ID_REG104) return 0;     // CFrameworkWndText::OnEvsUpdate() does not apply

	// Identifer LCDWIN_ID_REG102 is for OEP
	// in order to have the text be redisplayed for
	// text areas the lines are written even if the line
	// is the same because an area could be invalidated
	// but when the repaint is done the text already there
	// will not show up when the compare is removed it causes
	// problems with the OEP Display for touchscreen
	// so if that is the window being updated the code will work
	// as previously all others will redisplay all data on a paint
	// Identifer LCDWIN_ID_REG102 is for OEP
	if((m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209) &&
		PifSysConfig()->uchKeyType == KEYBOARD_TOUCH)
	{
		bPopupWindow = TRUE;
	}
    // parse the updated region by each line

	maxOEPButtons = myThis->textAttributes.oepBtnAcross * 
					myThis->textAttributes.oepBtnDown - 
					2 * myThis->textAttributes.oepBtnAcross;

    for (nRow = 0; nRow < myThis->m_nRowsInCtl; nRow++)
    {
        // is this line changed ? OR not a OEP Window
		//if nRow is at MLD_OEP_MAX_NUM, we are in the 20 element of the a 0 based
		//array, which is actually the 21st space, so we need to display this one 
		//if there is anything in it, no matter what JHHJ.
        if (((m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209) && (nRow == maxOEPButtons)) ||
			!bPopupWindow || 
			(_tcsncmp(myThis->m_pchLatest + nRow * myThis->m_nColumnsInCtl * 2,
						myThis->m_pchWork   + nRow * myThis->m_nColumnsInCtl * 2,
							myThis->m_nColumnsInCtl * 2)) != 0)
        {
            TCHAR*  pchText = myThis->m_pchText + nRow * myThis->m_nColumnsInCtl;
            TCHAR*  pchAttr = myThis->m_pchAttr + nRow * myThis->m_nColumnsInCtl;
            TCHAR*  pchWork = myThis->m_pchWork + nRow * myThis->m_nColumnsInCtl * 2;

            // convert from the event subsystem data to window data

            for (nColumn = 0; nColumn < myThis->m_nColumnsInCtl; nColumn++)
            {
                // is printing character ?

                if (*pchWork >= 0x20)
                {
                    *pchText++ = *pchWork++;
                    *pchAttr++ = *pchWork++;
                }
                // is upper cursor character ?
                else if (*pchWork == 0x18)
                {
                    *pchText++ = *pchWork++;
                    *pchAttr++ = *pchWork++;
                }
                // is down cursor character ?
                else if (*pchWork == 0x19)
                {
                    *pchText++ = *pchWork++;
                    *pchAttr++ = *pchWork++;
                }
                // others
                else
                {
                    *pchText++ = _T(' ');   // replace space character
                    pchWork++;
                    *pchAttr++ = *pchWork++;
                }
            }

            // copy the data

            _tcsncpy(myThis->m_pchLatest + nRow * myThis->m_nColumnsInCtl * 2,
                   myThis->m_pchWork   + nRow * myThis->m_nColumnsInCtl * 2,
                   myThis->m_nColumnsInCtl * 2);

			// If this is a standard text window (not OEP) then we treat this as a standard
			// text window.  However, if we are an OEP window, then we do the popup window
			// with buttons instead of text.  If an OEP Popup use the Display Popup
			// otherwise Get the text region and Invalidate so the text will be displayed
			if( ! bPopupWindow )
			{
				// calculate region of the changed line
				myThis->GetTextRegion(nRow, 0, myThis->m_nColumnsInCtl, &rect);

				// update the changed region
				myThis->InvalidateRect(&rect, FALSE);
			}
			else
			{
				int n = (myThis->m_nColumnsInCtl > 63) ? 63 : myThis->m_nColumnsInCtl;

				_tcsncpy(tszTempText, myThis->m_pchText + nRow * myThis->m_nColumnsInCtl, n);
				tszTempText[n] = 0;   // force end of string in case not part of copied string.

				myThis->DisplayPopup(nRow, tszTempText);
			}
        }
    }

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    update a cursor from event subsystem
//
// Prototype:   LRESULT     CFrameworkWndText::OnEvsCursor(wParam, lParam);
//
// Inputs:      WPARAM      wParam;     -
//              LPARAM      lParam;     -
//
// Outputs:     LRESULT     lResult;    - result code
//
// Purpose:	Handles cursor movements in a scrolling text region
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFrameworkWndText::OnEvsCursor(WPARAM wParam, LPARAM lParam)
{
    RECT    rect;
	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, this);

    // is my message ?

    if (wParam != (WPARAM)m_nIdentifier)    // invalid
    {
        return 0;                           // exit ...
    }

	ASSERT(myThis);

    if (myThis == 0)                        // invalid
    {
        return 0;                           // exit ...
    }

	if (m_nIdentifier == LCDWIN_ID_REG104) return 0;     // CFrameworkWndText::OnEvsCursor() does not apply

    myThis->m_bWCursor    = HIWORD(lParam);         // state
    myThis->m_ptWCursor.x = LOBYTE(LOWORD(lParam)); // column of cursor
    myThis->m_ptWCursor.y = HIBYTE(LOWORD(lParam)); // row of cursor

    // is same cursor state or position ?

    if (myThis->m_bLCursor    == myThis->m_bWCursor &&
        myThis->m_ptLCursor.x == myThis->m_ptWCursor.x &&
        myThis->m_ptLCursor.y == myThis->m_ptWCursor.y)
    {
        return 0;                           // exit ...
    }

    // update the changed region of previous

    myThis->GetTextRegion(myThis->m_ptLCursor.y, 0, myThis->m_nColumnsInCtl, &rect);
    myThis->InvalidateRect(&rect, FALSE);

    // update cursor state and position

    myThis->m_bLCursor  = myThis->m_bWCursor;
    myThis->m_ptLCursor = myThis->m_ptWCursor;

    // update the changed region of latest

    myThis->GetTextRegion(myThis->m_ptLCursor.y, 0, myThis->m_nColumnsInCtl, &rect);
    myThis->InvalidateRect(&rect, FALSE);

    // exit ...

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    display data
//
// Prototype:   void        CFrameworkWndText::DisplayData(dc);
//
// Inputs:      CPaintDC&   dc;         - device context object
//
// Outputs:     nothing
//
// Purpose: Displays text data in regular or double-width formats
//
/////////////////////////////////////////////////////////////////////////////

void CFrameworkWndText::DisplayData(CPaintDC& dc)
{
    CRect       rect;
    LONG        nChars;
    LONG        nRow, nColumn;
    TCHAR       chPText, chCText;
    TCHAR       chPAttr, chCAttr;
    TCHAR       achTdata[MNEMO_CHAR+1];
    int         i,j;
	LONG		lDCharacter = 0;

 //////////Display Time
	if(m_nIdentifier == LCDWIN_ID_CLOCK01 || m_nIdentifier == LCDWIN_ID_CLOCK02)
	{
		CString         sData;

		// get current date & time

		DATE_TIME DateTime;
        PifGetDateTime(&DateTime);

		// sData  = time.Format();
		{
			int iYear   = DateTime.usYear;
			int iMonth  = DateTime.usMonth;
			int iDay    = DateTime.usMDay;
			int iHour   = DateTime.usHour;

			m_nMinute = DateTime.usMin;

			if(!BlFwIfMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) // MDC OPTION FOR MM/DD/YY
			{
				if(!BlFwIfMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) //MDC FOR NOT MILITARY TIME
				{
					if (iHour >= 12) {
						iHour -= 12;
						if (iHour == 0) iHour = 12; /* 12:00PM, V1.0.04 */
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d PM"), iMonth, iDay, iYear, iHour, m_nMinute);
					}
					else {
						if (iHour == 0) iHour = 12; /* 12:00AM, V1.0.04 */
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d AM"),  iMonth, iDay,iYear, iHour, m_nMinute);
					}
				}
				else
				{
					if( iHour >= 12)
					{
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d PM"),  iMonth, iDay,  iYear, iHour, m_nMinute);
					}
					else
					{
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d AM"),  iMonth, iDay,  iYear, iHour, m_nMinute);
					}
				}
			}
			else // MDC OPTION FOR DD/MM/YY
			{
				if(!BlFwIfMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) //MDC FOR NOT MILITARY TIME
				{
					if (iHour >= 12) {
						iHour -= 12;
						if (iHour == 0) iHour = 12; /* 12:00PM, V1.0.04 */
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d PM"), iDay, iMonth,  iYear, iHour, m_nMinute);
					}
					else {
						if (iHour == 0) iHour = 12; /* 12:00AM, V1.0.04 */
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d AM"),  iDay, iMonth,  iYear, iHour, m_nMinute);
					}
				}
				else
				{
					if( iHour >= 12)
					{
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d PM"),  iDay, iMonth, iYear, iHour, m_nMinute);
					}
					else
					{
						sData.Format(TEXT("%02d/%02d/%4d %02d:%02d AM"),  iDay, iMonth, iYear, iHour, m_nMinute);
					}
				}
			}
		}

		RECT   rect = {0};

		GetClientRect(&rect);
		dc.FillSolidRect(&rect, m_colorBG);
		dc.SetTextColor(m_colorText);
		dc.DrawText(sData, sData.GetLength(), &rect, (DT_CENTER | DT_SINGLELINE | DT_VCENTER));
	} else if (m_nIdentifier == LCDWIN_LEN_REG104) {
		RECT   rect = {0};

		GetClientRect(&rect);
		dc.FillSolidRect(&rect, m_colorBG);
	} else {
///////////// display text

		for (nRow = 0; nRow < m_nRowsInCtl; nRow++)
		{
			//initialize the number of Double width characters for the row
			lDCharacter = 0;
			chPText = *(m_pchText + nRow * m_nColumnsInCtl);
			chPAttr = *(m_pchAttr + nRow * m_nColumnsInCtl);

			// parse initiale position of column

			if (chPText == _T('\x18') ||
				chPText == _T('\x19') ||
				m_nColumnsInCtl == 1)
			{
				nColumn = 0;
				nChars  = 0;                    // number of characters
			}
			else
			{
				nColumn = 1;
				nChars  = 1;                    // number of characters
			}

			while (nColumn < m_nColumnsInCtl)
			{
				chCText = *(m_pchText + nRow * m_nColumnsInCtl + nColumn);
				chCAttr = *(m_pchAttr + nRow * m_nColumnsInCtl + nColumn);

				//if a character is double width count it
				if(chCText > 0x7F){
					lDCharacter++;
				}
				// is upper cursor character ?

				if (chPText == _T('\x18'))
				{
					DisplayUpperCursor(dc, nRow, nColumn, chPAttr);
				}

				// is down cursor character ?

				else if (chPText == _T('\x19'))
				{
					DisplayDownCursor(dc, nRow, nColumn, chPAttr);
				}

				// others

				else
				{
					// is same attribute ?

					if (chCAttr == chPAttr)
					{
						nChars++;
						nColumn++;

						// is not end of column ?

						if (nColumn < m_nColumnsInCtl)
						{
							continue;               // next column ...
						}
					}


					/* display '&' correctly */
					for (i=0, j=0; i<nChars; i++, j++) {
						achTdata[j] = *(m_pchText + nRow * m_nColumnsInCtl + nColumn - nChars + i);
						if (achTdata[j] == _T('&')) {
							achTdata[++j] = _T('&');
						}
					}

					LONG    cbChars;
					cbChars = j;

					// draw text characters
					long lLeft = nColumn - nChars;
					if(lLeft > 0){
						long lDblToLeft = 0;
						for(int c = 0;c < lLeft; c++){
							//if a character is double width count it
							if(*(m_pchText + nRow * m_nColumnsInCtl + c) > 0x7F){
								lDblToLeft++;
							}
						}
						//unless the left is at the far left(0)
						//add the number of double width characters
						//to the left side of the rect to adjust the
						//for the extra width that those characters take
						lLeft += lDblToLeft;
					}
					GetTextRegion(nRow, lLeft, nChars + lDCharacter, &rect);
					 // colorpalette-changes
					if ( (m_nIdentifier == LCDWIN_ID_COMMON002) && (chPAttr & oldATTRIB_BLINK) )
					{
						chPAttr &= ~oldATTRIB_BLINK;
						chPAttr |= ATTRIB_BLINK;
					}
					dc.FillSolidRect(rect, GetBackColor(nRow, chPAttr));
					dc.SetTextColor(GetTextColor(nRow, chPAttr));
					dc.DrawText(&achTdata[0], cbChars, &rect, DT_LEFT);
				}

				chPText = chCText;
				chPAttr = chCAttr;
				nChars  = 1;
				nColumn++;
			}
	    }
	}
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:    display upper cursor
//
// Prototype:   void        CFrameworkWndText::DisplayUpperCursor(dc, nRow, nColumn, chAttr);
//
// Inputs:      CPaintDC&   dc;         - device context object
//              LONG        nRow;       - row position of cursor
//              LONG        nColumn;    - column position of cursor
//              UCHAR       chAttr;     - attribute of cursor
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFrameworkWndText::DisplayUpperCursor(CPaintDC& dc, LONG nRow, LONG nColumn, TCHAR chAttr)
{
    POINT   ptBase;
    POINT   ptCursor[NUMBER_UPPER];
    RECT    rect;

    // calculate point of upper cusor

    ptBase.x      = GetMarginLeft() + controlAttributes.lfControlFont.lfWidth  * nColumn;
    ptBase.y      = GetMarginTop()  + controlAttributes.lfControlFont.lfHeight * nRow;

    ptCursor[0].x = ptBase.x + controlAttributes.lfControlFont.lfWidth / 2;                    // top
    ptCursor[0].y = ptBase.y + GetCMarginTop();
    ptCursor[1].x = ptBase.x + controlAttributes.lfControlFont.lfWidth  - GetCMarginRight();   // right side
    ptCursor[1].y = ptBase.y + controlAttributes.lfControlFont.lfHeight - GetCMarginButtom();
    ptCursor[2].x = ptBase.x + GetCMarginLeft();                    // left side
    ptCursor[2].y = ptCursor[1].y;

    GetTextRegion(nRow, nColumn, 1, &rect);
//  dc.FillSolidRect(&rect, GetBackColor(nRow, chAttr));
    dc.FillSolidRect(&rect, m_colorBG);
    dc.Polygon(ptCursor, NUMBER_UPPER);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    display down cursor
//
// Prototype:   void        CFrameworkWndText::DisplayDownCursor(dc, nRow, nColumn, chAttr);
//
// Inputs:      CPaintDC&   dc;         - device context object
//              LONG        nRow;       - row position of cursor
//              LONG        nColumn;    - column position of cursor
//              UCHAR       chAttr;     - attribute of cursor
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFrameworkWndText::DisplayDownCursor(CPaintDC& dc, LONG nRow, LONG nColumn, TCHAR chAttr)
{
    POINT   ptBase;
    POINT   ptCursor[NUMBER_DOWN];
    RECT    rect;

    // calculate point of down cusor

    ptBase.x      = GetMarginLeft() + controlAttributes.lfControlFont.lfWidth  * nColumn;
    ptBase.y      = GetMarginTop()  + controlAttributes.lfControlFont.lfHeight * nRow;

    ptCursor[0].x = ptBase.x + GetCMarginLeft();                    // left side
    ptCursor[0].y = ptBase.y + GetCMarginTop();
    ptCursor[1].x = ptBase.x + controlAttributes.lfControlFont.lfWidth - GetCMarginRight();    // right side
    ptCursor[1].y = ptCursor[0].y;
    ptCursor[2].x = ptBase.x + controlAttributes.lfControlFont.lfWidth / 2;
    ptCursor[2].y = ptBase.y + controlAttributes.lfControlFont.lfHeight - GetCMarginButtom();  // buttom

    GetTextRegion(nRow, nColumn, 1, &rect);
//  dc.FillSolidRect(&rect, GetBackColor(nRow, chAttr));
    dc.FillSolidRect(&rect, m_colorBG);
    dc.Polygon(ptCursor, NUMBER_UPPER);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get drawing region of text data
//
// Prototype:   COLORREL    CFrameworkWndText::GetTextRegion(nRow, nColumn, nChars, pRect);
//
// Inputs:      LONG        nRow;       -
//              LONG        nColumn;    -
//              LONG        nChars;     -
//              RECT*       pRect;      -
//
// Outputs:     COLORREF    color;      -
//
/////////////////////////////////////////////////////////////////////////////

void CFrameworkWndText::GetTextRegion(LONG nRow, LONG nColumn, LONG nChars, RECT* pRect)
{
	/*
		When determining the text region the m_rectClient should be used. The m_rectClient
		varaible contains the actual size of the window. This variable is initialized
		in the OnCreate function. ESMITH
	*/
    pRect->left   = GetMarginLeft() + (m_rectClient.right/m_nColumnsInCtl)  * nColumn;
    pRect->top    = GetMarginTop() + (m_rectClient.bottom/m_nRowsInCtl) * nRow;
    pRect->right  = GetMarginLeft() + m_rectClient.right;
    pRect->bottom = pRect->top + (m_rectClient.bottom/m_nRowsInCtl);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get text character color
//
// Prototype:   COLORREL    CFrameworkWndText::GetTextColor(nRow, chAttribute);
//
// Inputs:      LONG        nRow;           -
//              UCHAR       chAttribute;    -
//
// Outputs:     COLORREF    color;          -
//
/////////////////////////////////////////////////////////////////////////////

DWORD CFrameworkWndText::GetTextColor(LONG nRow, TCHAR chAttribute)
{
    COLORREF    color = RGB(0x00, 0x00, 0xff);

	// Main windows coloring logic (for new Color Palette Code PLUs)
	if( (m_nIdentifier == LCDWIN_ID_REG100)||
		(m_nIdentifier == LCDWIN_ID_REG200)||
		(m_nIdentifier == LCDWIN_ID_REG201)||
		(m_nIdentifier == LCDWIN_ID_REG202) )
		{
			TCHAR       chAttr = chAttribute & ~0x0180;  // get attribute of text character
			PARACOLORPALETTE paraColorPal = { 0 };

			paraColorPal.uchMajorClass = CLASS_PARACOLORPALETTE;
			paraColorPal.uchAddress = chAttribute;

			BlFwIfParaRead(&paraColorPal);

			// cursor control
			if (m_bLCursor && m_ptLCursor.y == nRow)
			{
				color = RGB(0xff, 0xff, 0xff);//RGB(0x00, 0x00, 0xff);
			}
			else
			{
				// is blinking character ?

				if ( (chAttribute & ATTRIB_BLINK) && (m_nIdentifier == LCDWIN_ID_COMMON002) )
				{
					color = RGB(0xff, 0xff, 0xff);  // for mono-lcd, V1.0.11
		//            color = RGB(0x00, 0xff, 0xff);  // cyan
					return color;                   // exit ...
				}

				color = paraColorPal.crForeground;		
			}
	} else { // use normal coloring logic
		TCHAR       chAttr = chAttribute & 0x0f;   // get attribute of text character

		// cursor control
		if (m_bLCursor && m_ptLCursor.y == nRow)
		{
			// which attribute ?

			switch (chAttr)
			{
			case ATTRIB_BLACK:
				color = m_colorText;//RGB(0x00, 0x00, 0x00);  // black
				break;
			case ATTRIB_WHITE:
				color = RGB(0xff, 0xff, 0xff);//m_colorOppText;  // white
				break;
			default:
				color = RGB(0x00, 0x00, 0xff);
				break;
			}
		}
		else
		{
			// which attribute ?

			switch (chAttr)
			{
			case ATTRIB_BLACK:
				color = RGB(0xff, 0xff, 0xff);//m_colorOppText;  // white
				break;
			case ATTRIB_WHITE:
				color = m_colorText;//RGB(0x00, 0x00, 0x00);  // black
				break;
			default:
				color = RGB(0x00, 0x00, 0xff);
				break;
			}
		}
	}

    return color;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get background color
//
// Prototype:   COLORREF    CFrameworkWndText::GetBackColor(nRow, chAttribute);
//
// Inputs:      LONG        nRow;           -
//              UCHAR       chAttribute;    -
//
// Outputs:     COLORREF    color;          -
//
/////////////////////////////////////////////////////////////////////////////

DWORD CFrameworkWndText::GetBackColor(LONG nRow, TCHAR chAttribute)
{
    COLORREF    color = RGB(0x00, 0x00, 0xff);

	// Main windows coloring logic (for new Color Palette Code PLUs)
	if ((m_nIdentifier == LCDWIN_ID_REG100)||
		(m_nIdentifier == LCDWIN_ID_REG200)||
		(m_nIdentifier == LCDWIN_ID_REG201)||
		(m_nIdentifier == LCDWIN_ID_REG202) )
	{

		// get attribute of background character
		TCHAR       chAttr = (chAttribute & 0x70) >> 4;

		// cursor control
		PARACOLORPALETTE paraColorPal = { 0 };

		paraColorPal.uchMajorClass = CLASS_PARACOLORPALETTE;
		paraColorPal.uchAddress = chAttribute;

		BlFwIfParaRead(&paraColorPal);

		if (m_bLCursor && m_ptLCursor.y == nRow)
		{
			// which attribute ?
			switch (chAttribute)	
			{
			case ATTRIB_BLACK:
				color = RGB(0x00, 0x00, 0xff);  // blue
				break;
	//        case ATTRIB_WHITE:
	//            color = m_colorBG;//RGB(0xff, 0x00, 0x00);  // red
	//            break;
			default:
				color = RGB(0x00, 0x00, 0xff);
				break;
			}
		}
		else
		{
			// is blinking character ?
			if ( (chAttribute & ATTRIB_BLINK) && (m_nIdentifier == LCDWIN_ID_COMMON002) )
			{
				color = RGB(0xc0, 0xc0, 0xc0);  // for mono-lcd, V1.0.11
	//            color = RGB(0x00, 0xff, 0xff);  // cyan
				return color;                   // exit ...
			}

			// if the 'chAttribute' is 0, that means a color pallette has not been
			//	set, so the window background color should be used; otherwise, the
			//	background color identified by the pallette will be used.
			//if (chAttribute != 0)
			//{	// use palette background color
				color = paraColorPal.crBackground;
			//}
			//else
			//{	// use window background color
			//	color = m_colorBG;
			//}
			
		}
	} else { // use normal coloring logic
		// get attribute of background character
		TCHAR       chAttr = (chAttribute & 0x70) >> 4;

		// cursor control
		if (m_bLCursor && m_ptLCursor.y == nRow)
		{
			// which attribute ?
			switch (chAttr)
			{
			case ATTRIB_BLACK:
				color = RGB(0x00, 0x00, 0xff);  // blue
				break;
			case ATTRIB_WHITE:
				color = m_colorBG;//RGB(0xff, 0x00, 0x00);  // red
				break;
			default:
				color = RGB(0x00, 0x00, 0xff);
				break;
			}
		}
		else
		{
			// is blinking character ?
			if (chAttribute & oldATTRIB_BLINK)
			{
				color = RGB(0xc0, 0xc0, 0xc0);  // for mono-lcd, V1.0.11
	//            color = RGB(0x00, 0xff, 0xff);  // cyan
				return color;                   // exit ...
			}

			// which attribute ?
			switch (chAttr)
			{
			case ATTRIB_BLACK:
				color = m_colorBG;//RGB(0xff, 0xff, 0xff);  // white
				break;
			case ATTRIB_WHITE:
				color = RGB(0xff, 0x00, 0x00);
				break;
			default:
				color = RGB(0x00, 0x00, 0xff);
				break;
			}
		}
	}

    return color;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFrameworkWndText::PreCreateWindow(CREATESTRUCT& cs)
{
    // change windows style

    cs.dwExStyle |= WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

    return CWnd::PreCreateWindow(cs);
}

void CFrameworkWndText::OnPaint()
{
	CPaintDC    dc(this);                   // device context for painting

	// backup objects in the DC which we are about to replace
	// with our own color, font, and brush.

	COLORREF    oldText  = dc.GetTextColor();
	CFont*      pOldFont = dc.SelectObject(&m_Font);
	CBrush*     pOldBrush = dc.SelectObject(&m_Brush);

	// paint background

	dc.FillSolidRect(&m_rectClient, m_colorBG);
//	dc.FillSolidRect(&m_rectClient, m_colorFace);

	// display all data

	DisplayData(dc);

#ifdef DEMO_VERSION
	//place "water mark" on text areas
	dc.SetTextColor(RGB(210,210,210));
	dc.SetBkMode(TRANSPARENT);
	CRect demoRect(m_rectClient);
	CRect calRect(m_rectClient);

	dc.DrawText(_T("DEMO      "), -1, &calRect, DT_CALCRECT);
	for(int i = m_rectClient.top, k = 1; i <= m_rectClient.bottom;k++){
		for(int j = m_rectClient.left; j <= m_rectClient.right;){
			dc.DrawText(_T("DEMO      "), -1, &demoRect, DT_NOCLIP);
			j = demoRect.left += calRect.right - calRect.left;
		}
		i = demoRect.top += calRect.bottom - calRect.top;
		demoRect.left = m_rectClient.left + (k * (calRect.right - calRect.left))/10;
	}
#endif

	// restore the old objects before we return as ours are about
	// to go out of scope and be deleted.

	dc.SetTextColor(oldText);
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldBrush);

	// Do not call CWindowText::OnPaint() for painting messages
}

int CFrameworkWndText::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameworkWndTextSubWindow * pSubWind = 0;
	CFrameworkWndItem   *pParentWndItem = NULL;

	switch(textAttributes.type){
	case TypeSnglReceiptMain:              // LCDWIN_ID_REG100
		m_nIdentifier = LCDWIN_ID_REG100;  //(0x0400 + 0x0300) + 100 one Trans Window
		m_nRowsInCtl = MLD_NOR_DSP1_ROW_LEN;//m_nRow;
		m_nColumnsInCtl = MLD_NOR_DSP1_CLM_LEN;//m_nColumn;
		break;
	case TypeSnglReceiptCustDisplay:       // LCDWIN_ID_REG110
		m_nIdentifier = LCDWIN_ID_REG110;  //(0x0400 + 0x0300) + 110 one Trans Window
		m_nRowsInCtl = MLD_NOR_DSP1_ROW_LEN;//m_nRow;
		m_nColumnsInCtl = MLD_NOR_DSP1_CLM_LEN;//m_nColumn;
		break;
	case TypeMultiReceipt1:               // LCDWIN_ID_REG200
		m_nIdentifier = LCDWIN_ID_REG200; //(0x0400 + 0x0300) + 200 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_DSP1_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_DSP1_CLM_LEN;
		break;
	case TypeMultiReceipt2:               // LCDWIN_ID_REG201
		m_nIdentifier = LCDWIN_ID_REG201; //(0x0400 + 0x0300) + 201 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_DSP2_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_DSP2_CLM_LEN;
		break;
	case TypeMultiReceipt3:               // LCDWIN_ID_REG202
		m_nIdentifier = LCDWIN_ID_REG202; //(0x0400 + 0x0300) + 202 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_DSP3_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_DSP3_CLM_LEN;
		break;
	case TypeLeadThru:                    // LCDWIN_ID_COMMON001
		m_nIdentifier = LCDWIN_ID_COMMON001;	//(0x0400 + 0x0300) + 1 one or more Trans Window
		m_nRowsInCtl = ECHO_BACK_ROW_LEN;//wc->m_nRow;
		m_nColumnsInCtl = ECHO_BACK_CLM_LEN;//wc->m_nColumn;
		break;
	case TypeLeadThruCustDisplay:               // LCDWIN_ID_COMMON004
		m_nIdentifier = LCDWIN_ID_COMMON004;	//(0x0400 + 0x0300) + 1 one or more Trans Window
		m_nRowsInCtl = ECHO_BACK_ROW_LEN;//wc->m_nRow;
		m_nColumnsInCtl = ECHO_BACK_CLM_LEN;//wc->m_nColumn;
		break;
	case TypeLeadThruDescriptor:          // LCDWIN_ID_COMMON002
		m_nIdentifier = LCDWIN_ID_COMMON002;	//(0x0400 + 0x0300) + 2 one or more Trans Window
		m_nRowsInCtl = DESCRIPTOR_ROW_LEN;//wc->m_nRow;
		m_nColumnsInCtl = DESCRIPTOR_CLM_LEN;//wc->m_nColumn;
		break;
	case TypeSnglReceiptSub:             // LCDWIN_ID_REG101
		m_nIdentifier = LCDWIN_ID_REG101;  //(0x0400 + 0x0300) + 101 one Trans Window
		m_nRowsInCtl = MLD_NOR_DSP2_ROW_LEN;//wc->m_nRow;
		m_nColumnsInCtl = MLD_NOR_DSP2_CLM_LEN;//wc->m_nColumn;
		break;
	case TypeSnglReceiptScroll:         // LCDWIN_ID_REG103
		m_nIdentifier = LCDWIN_ID_REG103;  //(0x0400 + 0x0300) + 103 one Trans Window
		m_nRowsInCtl = MLD_NOR_DSP1A_ROW_LEN;//wc->m_nRow;
		m_nColumnsInCtl = MLD_NOR_DSP1A_CLM_LEN;//wc->m_nColumn;
		break;
	case TypeOEP:
		m_nIdentifier = LCDWIN_ID_REG102;  //(0x0400 + 0x0300) + 102 one Trans Window
		m_nRowsInCtl = MLD_NOR_POP_ROW_LEN;
		m_nColumnsInCtl = MLD_NOR_POP_CLM_LEN;
		// Ensure that we have hooked into the OEP groups functions.
		BlSetFrameworkFunc(CMD_SET_OEP_GROUPS, BlFwSetOepGroup);
		BlSetFrameworkFunc(CMD_GET_MAX_OEP_BUTTONS, BlFwGetMaxOepItems);
		BlSetFrameworkFunc(CMD_DISPLAY_OEP_IMAGE, BlFwDisplayOEPImagery);
		pSubWind = new CFrameworkWndTextSubWindow (this, m_nIdentifier);
		ASSERT(pSubWind);
		if (pSubWind)
			pSubWind->CompileOepString (this->textAttributes.mOEPGroupNumbers);
		break;
	case TypeOEPCustDisplay:                    // LCDWIN_ID_REG104
		m_nIdentifier = LCDWIN_ID_REG104;       // CWindowTextType::TypeOEPCustDisplay
		m_nRowsInCtl = MLD_NOR_POP_ROW_LEN;
		m_nColumnsInCtl = MLD_NOR_POP_CLM_LEN;
		break;
	case TypeClock:                             // LCDWIN_ID_CLOCK01
		m_nIdentifier = LCDWIN_ID_CLOCK01;		//CLOCK one Trans Window
		m_nRowsInCtl = 1;
		m_nColumnsInCtl = 20;
		break;
	case TypeClockCustDisplay:                  // LCDWIN_ID_CLOCK02
		m_nIdentifier = LCDWIN_ID_CLOCK02;		//CLOCK one Trans Window
		m_nRowsInCtl = 1;
		m_nColumnsInCtl = 20;
		break;
	case TypeMultiReceiptTotal1:
		m_nIdentifier = LCDWIN_ID_REG206;	//Total #1 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_TTL1_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_TTL1_CLM_LEN;
		break;
	case TypeMultiReceiptTotal2:
		m_nIdentifier = LCDWIN_ID_REG207;	//Total #2 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_TTL2_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_TTL2_CLM_LEN;
		break;
	case TypeMultiReceiptTotal3:
		m_nIdentifier = LCDWIN_ID_REG208;	//Total #3 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_TTL3_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_TTL3_CLM_LEN;
		break;
	case TypeMultiReceiptGuest1:
		m_nIdentifier = LCDWIN_ID_REG203;	//Guest #1 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_ORD1_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_ORD1_CLM_LEN;
		break;
	case TypeMultiReceiptGuest2:
		m_nIdentifier = LCDWIN_ID_REG204;	//Guest #2 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_ORD2_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_ORD2_CLM_LEN;
		break;
	case TypeMultiReceiptGuest3:
		m_nIdentifier = LCDWIN_ID_REG205;	//Guest #3 Three Trans Window
		m_nRowsInCtl = MLD_DRV3_ORD3_ROW_LEN;
		m_nColumnsInCtl = MLD_DRV3_ORD3_CLM_LEN;
		break;
	case TypeSupProg:
		m_nIdentifier = LCDWIN_ID_SUPER300;	//Supervisor/Program Window
		m_nRowsInCtl = MLD_SUPER_MODE_ROW_LEN;
		m_nColumnsInCtl = MLD_SUPER_MODE_CLM_LEN;
		break;
	default:
		m_nIdentifier = LCDWIN_ID_CLOCK01;
		m_nRowsInCtl = 1;
		m_nColumnsInCtl = 20;
		break;
	}

	controlAttributes.m_nHeight = controlAttributes.m_usHeightMultiplier * CFrameworkWndButton::stdHeight;
	controlAttributes.m_nWidth = controlAttributes.m_usWidthMultiplier * CFrameworkWndButton::stdWidth;
	textAttributes.m_nLeft = controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth;
	textAttributes.m_nTop = controlAttributes.m_nRow * CFrameworkWndButton::stdHeight;
	m_pchText      = new TCHAR [m_nRowsInCtl * m_nColumnsInCtl];    // may be UNICODE
    m_pchAttr      = new TCHAR [m_nRowsInCtl * m_nColumnsInCtl];
    m_pchLatest    = new TCHAR [m_nRowsInCtl * m_nColumnsInCtl * 2];// with attribute
    m_pchWork      = new TCHAR [m_nRowsInCtl * m_nColumnsInCtl * 2];// with attribute
	//m_nType = wc->m_nType;
	//m_Action = wc->m_myAction;

    ClearBuffer();

	if (CWindowText::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CDC *pDC = GetDC ();
	// update window size
	int nLogPixX = 100;//pDC->GetDeviceCaps(LOGPIXELSX);
	int nLogPixY = 100;//pDC->GetDeviceCaps(LOGPIXELSY);

	// ReleaseDC(pDC);

    SetWindowPos(NULL, textAttributes.m_nLeft,
				textAttributes.m_nTop,
				controlAttributes.m_nWidth,
				controlAttributes.m_nHeight,
				SWP_NOOWNERZORDER);

    // get client window area
	GetClientRect(&m_rectClient);

    LONG    cx, cy;
    BOOL    bState;

    // calculate font size
    cx = m_rectClient.right  - m_rectClient.left - GetMarginLeft() - GetMarginRight();
	cy = m_rectClient.bottom - m_rectClient.top  - GetMarginTop()  - GetMarginButtom();

    controlAttributes.lfControlFont.lfHeight = cy / m_nRowsInCtl;            // height of font
	
	/*
		For each window a calculation is done to determine the size of the characters
		that can be applied to the window by first determining the size of the rectangle
		of the window then dividing that length by the number of columns that should be
		available for that window, where the number of columns represents the total number
		of printable characters for that window. The division (cx/m_nColumnsInCtl) is considered 
		to be the average length of each character in logical units. When this value is applied 
		to the logfont structure of controlAttributes (lfControlFont), it is then stored into 
		the font variable (m_Font). The actually font that will be applied to the window will not 
		be chosen until the	font is applied to the device context with a call to "SelectObject" 
		where the GDI's font mapper will attempt to match the logical font with an existing physical
		font. If it fails to find an exact match it will provide an alternative whose
		characteristcs match as many of the requested characteristics. The calculation of the
		point size of the font must take into consideration if the window text is bold. Bold
		characters are rendered twice, where the second rendering is shifted one pixel to the
		right of the first rendering. For bold characters an extra pixel is removed for each 
		column in the total number of available	pixels: cx - m_nColumnsInCtl where 
		[cx = Total Number of Pixels] and [m_nColumnsInCtl = Number of Columns or available printable 
		characters for the window]. This extra pixel for each character will allot for the extra
		space that is needed for bold type font when printing to the window. ESMITH	
	*/
    if(controlAttributes.lfControlFont.lfWeight == FW_BOLD)
	{
		controlAttributes.lfControlFont.lfWidth  = (cx - m_nColumnsInCtl) / m_nColumnsInCtl;
	}else{
		controlAttributes.lfControlFont.lfWidth  = cx / m_nColumnsInCtl;
	}
#if 1
	// set a cap for the font height and width so that we will not get the weird looking
	// tall spikey and hard to read lettering that can show in the clock control.
	// What can happen is that a one line control is next to a two line control so
	// the text font is scaled to be twice as tall as it needs to be.
	if (controlAttributes.lfControlFont.lfHeight > 48) {
		controlAttributes.lfControlFont.lfHeight = 48;
	}
	if (controlAttributes.lfControlFont.lfWidth > 48) {
		controlAttributes.lfControlFont.lfWidth = 48;
	}
	// The scaling factors used here are from a set of experiments to visual check what the
	// text strings look like when they are corrected.
	// A width to height ratio of 3:5 (0.6) is recommended for most applications.
	// However testing indicates that in most cases of difficult to read text displayed
	// the width is quite small for instance if trying to display two 40 character lines in
	// a window that is 16 grids wide and 4 grids high or one 20 character line
	// in a window that is 8 grids wide and 4 grids high.  Each grid ia a 15x15 block of pixels.
	if (controlAttributes.lfControlFont.lfHeight > controlAttributes.lfControlFont.lfWidth * 6) {
		controlAttributes.lfControlFont.lfHeight = controlAttributes.lfControlFont.lfWidth * 6;
	}
#endif

	bState = m_Font.CreateFontIndirect(&controlAttributes.lfControlFont);
    // create brush object

	bState = m_Brush.CreateSolidBrush(m_colorText);

	if(m_nIdentifier == LCDWIN_ID_REG102 || m_nIdentifier == LCDWIN_ID_REG209 || m_nIdentifier == LCDWIN_ID_REG104) {
		if (m_nIdentifier == LCDWIN_ID_REG104) {
			m_OEPCustDisplayImage.Create(_T("A bitmap static control (B)"), 
			WS_CHILD|WS_BORDER | WS_VISIBLE | SS_BITMAP| SS_CENTERIMAGE, CRect(90,16,138,64),
			this);
		}
		// if this is an OEP Window or an OEP Customer Display Window then hide it until needed.
		GetParent()->ShowWindow(SW_HIDE);   // TypeOEP or TypeOEPCustDisplay
	}
	//
    if(m_nIdentifier == LCDWIN_ID_CLOCK01 || m_nIdentifier == LCDWIN_ID_CLOCK02){
		m_nTimer = SetTimer(m_nIdentifier, 800, NULL);
		if (m_nTimer == 0) {
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_8);
			Sleep(0);
			m_nTimer = SetTimer(m_nIdentifier, 800, NULL);
		}
	}
//if no identifier or if Clock don't connect to EVS
    if (m_nIdentifier != 0 && m_nIdentifier < LCDWIN_ID_CLOCK01)
    {
        // connect the event subsystem
		if (pSubWind)
		{
			pSubWind->EvsConnectSubWindow ();
		}
		else
		{
			if (::EvsConnect(m_nIdentifier, (ULONG)m_hWnd) != EVS_OK)
			{
				ASSERT(FALSE);
			}
		}
    }
//Show the Clock
    if(m_nIdentifier == LCDWIN_ID_CLOCK01 || m_nIdentifier == LCDWIN_ID_CLOCK02)
	{
		ShowWindow(SW_SHOW);
	}
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    replace system font to code 850 font
//
// Prototype:   TCHAR   CFrameworkWndText::Replace850(uchSrc);
//
// Inputs:      TCHAR   uchSrc;     sytem font  -
//
// Outputs:     TCHAR   code 850 font           -
//
/////////////////////////////////////////////////////////////////////////////
TCHAR CFrameworkWndText::Replace850(TCHAR uchSrc)
{
	/* 21RFC05284, support code 858 for Euro symbol */
	/* 0x20AC is Euro symbol in Unicode */

    static  TCHAR    abMnemoRead[MNEMO_CHAR] = {
                        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
                        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
                        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
                        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
                        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
                        0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
                        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x2302,/* V1.0.04 */
                        0xC7,0xFC,0xE9,0xE2,0xE4,0xE0,0xE5,0xE7,0xEA,0xEB,0xE8,0xEF,0xEE,0xEC,0xC4,0xC5,
                        0xC9,0xE6,0xC6,0xF4,0xF6,0xF2,0xFB,0xF9,0xFF,0xD6,0xDC,0xF8,0xA3,0xD8,0xD7,0x0192,/* V1.0.04*/
                        0xE1,0xED,0xF3,0xFA,0xF1,0xD1,0xAA,0xBA,0xBF,0xAE,0xAC,0xBD,0xBC,0xA1,0xAB,0xBB,
                        0x80,0x81,0x82,0x84,0x85,0xC1,0xC2,0xC0,0xA9,0x86,0x87,0x88,0x89,0xA2,0xA5,0x8A,
                        0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0xE3,0xC3,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0xA4,
                        0xF0,0xD0,0xCA,0xCB,0xC8,0x20AC/*0x98*/,0xCD,0xCE,0xCF,0x99,0x9A,0x9B,0x9C,0xA6,0xCC,0x9D,
                        0xD3,0xDF,0xD4,0xD2,0xF5,0xD5,0xB5,0xFE,0xDE,0xDA,0xDB,0xD9,0xFD,0xDD,0xAF,0xB4,
                        0xAD,0xB1,0x9E,0xBE,0xB6,0xA7,0xF7,0xB8,0xB0,0xA8,0xB7,0xB9,0xB3,0xB2,0x9F,0xA0
                    };

    TCHAR tchDest;
    tchDest = abMnemoRead[uchSrc];

#if 1
	/* V1.0.05 */
	if (tchDest == 0x20AC) tchDest = 0x20;	/* Euro symbol of Unicode does not shown on WINCE correctly */
#endif

    return tchDest;
}

void CFrameworkWndText::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == (UINT)m_nIdentifier)
	{
		//Check to see if Time has changed if so Invalidate so new time is displayed
		DATE_TIME DateTime;
		PifGetDateTime(&DateTime);
		int	iMinute = DateTime.usMin;

		if (m_nMinute != iMinute)
		{
			Invalidate(FALSE);
		}

		// NOTE: some control couldn't get WM_TIMER message in Saratoga appl.
		//       resetting timer seems meaningless, but works better.
		//       If the SetTimer () returns 0, it failed so issue PifLog()
		//       and then try once again.
        KillTimer(m_nTimer);
		Sleep(0);
        m_nTimer = SetTimer (m_nIdentifier, 800, NULL);
		if (m_nTimer == 0)
		{
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_1);
			Sleep(0);
			m_nTimer = SetTimer (m_nIdentifier, 800, NULL);
		}
    }
	else
	{
		CWnd::OnTimer(nIDEvent);
	}
}

void CFrameworkWndText::ClearAndShowPopup()
{
	CFrameworkWndItem   *pParentWndItem = NULL;
	CFrameworkWndItem	*pPreviousWndItem = NULL;
	CFrameworkWndButton *pChildWndButton = NULL;

	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
	if (pParentWndItem == 0) {
		m_nButtonCount = 0;
		return;
	}

	pParentWndItem->ShowWindow(SW_SHOW);
	pParentWndItem->BringWindowToTop();

	if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow (CFrameworkWndButton::SPL_BTN_BACK)){	//More/Back (up)
		delete pChildWndButton;
	}
	if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow (CFrameworkWndButton::SPL_BTN_MORE)){	//Continue More (down)
		delete pChildWndButton;
	}
	if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow (CFrameworkWndButton::SPL_BTN_DONE)){  //DONE
		delete pChildWndButton;
	}
	for(int l = 0; l < m_nButtonCount; l++){
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow (CFrameworkWndButton::SPL_BTN_START + l)){
			delete pChildWndButton;
		}
	}
	m_nButtonCount = 0;
}

#include <atlimage.h>



CList <CFrameworkWndText::ArenaOepTable>  CFrameworkWndText::m_ArenaOepTableList;

class CPxstr : public CString
{
public:
	CPxstr () : m_p(0) { }
	~CPxstr () {
		if (m_p) ReleaseBuffer(); m_p = 0;
	}

	PXSTR  PeekBuffer (int nLen = 512) { if (m_p == 0) m_p = GetBufferSetLength (nLen); return m_p; }


	PXSTR m_p;
private:

};

class CTextFile : public CFile
{
public:
	CTextFile () : ulBufferEndPoint(0) { }
	int ReadTextLine (CPxstr &csLine);
private:
	BYTE    lpBuff[1024];
	ULONG   ulBufferEndPoint;
};


int CTextFile::ReadTextLine (CPxstr &csLine)
{
	CPxstr::PXSTR   p = csLine.PeekBuffer ();
	int     iRet = -1;

	if (ulBufferEndPoint < 768) {
		ULONG   ulActualBytesRead = 0;

		try {
			ulActualBytesRead = this->Read (lpBuff + ulBufferEndPoint, 1024 - ulBufferEndPoint);
		}
		catch (CFileException *e) {
			throw e;
		}

		// ulBufferEndPoint points to position after last character in buffer.
		// ulBufferEndPoint now contains count of characters in the buffer.
		ulBufferEndPoint += ulActualBytesRead;
	}

	p[0] = 0;      // initialize output to an empty string.

	if (ulBufferEndPoint < 1) return iRet;

	UINT nLen = csLine.GetAllocLength ();

	for (UINT i = 0; i < nLen; i++) {
		p[i] = lpBuff[i];
		if (p[i] == L'\r' || p[i] == L'\n' || i >= ulBufferEndPoint) {
			p[i] = 0;
			if (lpBuff[i] == L'\r' && lpBuff[i+1] == L'\n') i++;
			// we need to change i value from zero based index into a one based count.
			if (ulBufferEndPoint > i) {
				memmove (lpBuff, lpBuff + 1 + i, ulBufferEndPoint - (i + 1));
				ulBufferEndPoint -= (i + 1);
			} else {
				ulBufferEndPoint = 0;
			}
			iRet = i + 1;
			break;
		}
	}

	return iRet;
}

static	struct {
		wchar_t *p1;
		wchar_t *p2;
		short    s;
		ULONG    ulMap;         // see also the corresponding member uchMap of ArenaOepTable.
	} myListEntry[] = {
	//	{L"<name>", L"</name>"},
	//	{L"<entry>", L"</entry>"},
		{L"<tableno>", L"</tableno>", 1, 0x0001},                  // CFrameworkWndText::test() case  0:
		{L"<groupno>", L"</groupno>", 1, 0x0002},                  // CFrameworkWndText::test() case  1:
		{L"<imagename>", L"</imagename>", 2, 0x0004},              // CFrameworkWndText::test() case  2:
		{L"<stickydisplay>", L"</stickydisplay>", 1, 0x0008},      // CFrameworkWndText::test() case  3:
		{L"<resizetofit>", L"</resizetofit>", 1, 0x0010},          // CFrameworkWndText::test() case  4:
		{L"<targetwindow>", L"</targetwindow>", 1, 0x0020},        // CFrameworkWndText::test() case  5:
		{L"<background>", L"</background>", 3, 0x0040},            // CFrameworkWndText::test() case  6:
		{L"<operatortext>", L"</operatortext>", 2, 0x0080},        // CFrameworkWndText::test() case  7:
		{L"<overlayimage>", L"</overlayimage>", 1, 0x0100},        // CFrameworkWndText::test() case  8:
		{L"<vertalign>", L"</vertalign>", 4, 0x0200},              // CFrameworkWndText::test() case  9:
		{L"<horizalign>", L"</horizalign>", 4, 0x0400},            // CFrameworkWndText::test() case 10:
		{L"<backimage>", L"</backimage>", 2, 0x0800},               // CFrameworkWndText::test() case 11:
		{L"<deptno>", L"</deptno>", 1, 0x1000}                     // CFrameworkWndText::test() case 12:
	};


ULONG  RflConvertStr2Ulong (wchar_t *pStr)
{
	ULONG  ulValue = 0;

	if (pStr) {
		int  iState = 0;    // 0 start, 1 - decimal, 2 - hex, 3 - octal, 4 - end
		while (*pStr && iState != 4) {
			switch (iState) {
				case 0:
					if (*pStr == L'0' && *(pStr + 1) == L'x') {
						iState = 2;
						pStr += 2;
					} else if (isdigit (*pStr)) {
						iState = 1;
					} else if (isspace (*pStr)) {
						pStr++;
					} else {
						iState = 4;
					}
					break;
				case 1:      // decimal digit conversion
					if (isdigit (*pStr)) {
						ulValue *= 10;
						ulValue += *pStr - L'0';
						pStr++;
					} else
						iState = 4;
					break;
				case 2:      // hexdecimal digit conversion
					if (isxdigit (*pStr)) {
						ulValue <<= 4;
						ulValue |= (isdigit(*pStr) ? *pStr - L'0' : (*pStr & 0x7) + 9);
						pStr++;
					} else
						iState = 4;
					break;
				default:
					iState = 4;
					break;
			}
		}
	}
	return ulValue;
}

ULONG  RflConvertStr2Position (wchar_t *pStr, wchar_t *pStrEnd)
{
	ULONG    ulMask = 0;   // indicates if numeric value or text positioning.
	ULONG    ulValue = 0;
	int      iIndex = 0;        // index into textValue for text position processing.
	wchar_t  textValue[32] = {0};

	if (pStr) {
		int  iState = 0;    // 0 start, 1 - decimal, 2 - string, 4 - end
		int  lastState = iState;
		while (*pStr && iState != 4 && pStr < pStrEnd) {
			lastState = iState;
			switch (iState) {
				case 0:
					if (isdigit (*pStr)) {
						iState = 1;
					} else if (isspace (*pStr)) {
						pStr++;
					} else {
						iState = 2;
					}
					break;
				case 1:      // decimal digit conversion
					if (isdigit (*pStr)) {
						ulValue *= 10;
						ulValue += *pStr - L'0';
						pStr++;
					} else
						iState = 4;
					break;
				case 2:      // text name conversion.
					if (! isspace (*pStr)) {
						textValue[iIndex++] = *pStr;
						pStr++;
					} else
						iState = 4;
					break;
				default:
					iState = 4;
					break;
			}
		}
		if (lastState == 2) {
			// convert the text position into an indicator.
			// one issue is that "center" is used for both vertical and horizontal alignment.
			// valid values are non-zero.
			//                               0       1         2       3        4          5
			wchar_t  *textPositioning[] = { L"", L"center", L"left", L"right", L"top", L"bottom" };

			for (int i = 1; i < sizeof(textPositioning)/sizeof(textPositioning[0]); i++) {
				if (_wcsnicmp (textPositioning[i], textValue, wcslen(textPositioning[i])) == 0) {
					// match so lets indicate which it is.
					ulMask = i << 16;
					break;
				}
			}
		}
	}
	return (ulValue | ulMask);
}

CFrameworkWndText::textPiece CFrameworkWndText::test (wchar_t *pLine, ArenaOepTable & x)
{
	textPiece  y = {0};

	for (int i = 0; i < sizeof(myListEntry)/sizeof(myListEntry[0]); i++) {
		wchar_t *p1 = wcsstr (pLine, myListEntry[i].p1);
		wchar_t *pp = wcsstr (pLine, L"#");
		wchar_t *p2 = wcsstr (pLine, myListEntry[i].p2);
		if ( p1 && !( pp && (pp < p1) ) ) {
			wchar_t *p1x = p1 + wcslen (myListEntry[i].p1);
			switch (myListEntry[i].s) {
				case 1:    // decimal numeric value
					y.iVal = _wtoi (p1x);
					y.p1 = p1x;
					y.p2 = p2;
					break;
				case 2:    // string value
					y.iVal = 0;
					y.p1 = p1x;
					y.p2 = p2;
					break;
				case 3:    // decimal numeric value unless starting with hex modifier 0x
					y.ulVal = RflConvertStr2Ulong (p1x);
					y.p1 = p1x;
					y.p2 = p2;
					break;
				case 4:    // position either as decimal numeric value or a text label.
					y.ulVal = RflConvertStr2Position (p1x, p2);
					y.p1 = p1x;
					y.p2 = p2;
					break;
			}
			switch (i) {
				case 0:                    // <tableno>
					x.tableno = y.iVal; x.ulMap |= myListEntry[i].ulMap;
					break;
				case 1:                    // <groupno>
					x.groupno = y.iVal; x.ulMap |= myListEntry[i].ulMap;
					break;
				case 2:                    // <imagename>
					{
						int j = 0;
						wchar_t imagename[512] = {0};
						for (wchar_t *pb = y.p1; pb < y.p2; pb++) {
							imagename[j++] = *pb;
						}
						x.imagename = imagename; x.ulMap |= myListEntry[i].ulMap;
					}
					break;
				case 3:                   // <stickydisplay>
					x.stickydisplay = y.iVal; x.ulMap |= myListEntry[i].ulMap;
					break;
				case 4:                   // <resizetofit>
					x.resizetofit = y.iVal;  x.ulMap |= myListEntry[i].ulMap;
					break;
				case 5:                   // <targetwindow>
					x.targetwindow = y.iVal;  x.ulMap |= myListEntry[i].ulMap;
					break;
				case 6:                   // <background>
					x.background = y.ulVal;  x.ulMap |= myListEntry[i].ulMap;
					break;
				case 7:                   // <operatortext>
					{
						int j = 0;
						wchar_t operatortext[512] = {0};
						for (wchar_t *pb = y.p1; pb < y.p2; pb++) {
							operatortext[j++] = *pb;
						}
						x.operatortext = operatortext; x.ulMap |= myListEntry[i].ulMap;
					}
					break;
				case 8:                   // <overlayimage>
					x.overlayimage = y.iVal; x.ulMap |= myListEntry[i].ulMap;
					break;
				case 9:		             // <vertalign>
					if (y.ulVal & 0x00ff0000) {
						x.vertalign = (USHORT)(0x8000 | y.ulVal >> 16);
					} else {
						x.vertalign = (USHORT)(y.ulVal & 0x00007fff);
					}
					x.ulMap |= myListEntry[i].ulMap;
					break;
				case 10:                 // <horizalign>
					if (y.ulVal & 0x00ff0000) {
						x.horizalign = (USHORT)(0x8000 | y.ulVal >> 16);
					} else {
						x.horizalign = (USHORT)(y.ulVal & 0x00007fff);
					}
					x.ulMap |= myListEntry[i].ulMap;
					break;
				case 11:                    // <backimage>
					{
						int j = 0;
						wchar_t imagename[512] = {0};
						for (wchar_t *pb = y.p1; pb < y.p2; pb++) {
							imagename[j++] = *pb;
						}
						x.backimage = imagename; x.ulMap |= myListEntry[i].ulMap;
					}
					break;
				case 12:                    // <deptno>
					x.deptno = y.iVal; x.ulMap |= myListEntry[i].ulMap;
					break;
			}
		}
	}

	return y;
}

int CFrameworkWndText::BuildArenaOepTableList ()
{	
	m_ArenaOepTableList.RemoveAll();

	TRACE0("** Start Processing file\n");
	try {
		CTextFile  xmlInput;
		xmlInput.Open (PifGetFilePath (L"PARAMIMG", "f", NULL), CFile::modeRead);

		CPxstr myP;
		int inArena = 0;
		int inEntry = 0;
		
		ArenaOepTable x = {0};
		ArenaOepTable xZero = {0};    // used to zero out x.
		while (xmlInput.ReadTextLine (myP) > 0) {
			if (RflMatch (L"< */ *arena *>", myP.m_p) && (inArena = 0) == 0 && (x = xZero, 1)) TRACE0("MATCH: /arena\n");
			if (inArena && RflMatch (L"< */ *entry *>", myP.m_p) && (inEntry = 0) == 0) {
				TRACE0("  Match: /entry\n");
				m_ArenaOepTableList.AddTail (x);
				x = xZero;
				inEntry = 0;    // force off building the current entry
			}
			if (inArena && inEntry) {
				CFrameworkWndText::textPiece y = test (myP.m_p, x);
				if (y.p1) TRACE3("  test: %p %p  %s\n", y.p1, y.p2, y.p1);
			}
			if (RflMatch (L"< *arena *>", myP.m_p) && (inArena = 1) == 1 ) TRACE0("MATCH: arena\n");
			if (inArena && RflMatch (L"< *entry *>", myP.m_p) && (inEntry = 1) == 1 && (x = xZero, 1)) TRACE0("  Match: entry\n");
			TRACE1("    TextLine %s\n", myP.m_p);
		}
	}
	catch (CFileException *e) {
		TRACE0(" CFileException opening PARAMIMG \n");
		e->Delete();
	}
	TRACE0("   End Processing file\n");

	return m_ArenaOepTableList.GetCount();
}

static struct {
	int   iIndex;
	CFrameworkWndText::ArenaOepTable  table[5];
} ArenaOepTableArea;

CFrameworkWndText::ArenaOepResult CFrameworkWndText::SearchArenaOepTableList (int iGroupNo, int iTableNo, int iDeptNo)
{
	static ArenaOepTable dummy = {0};

	POSITION pos = m_ArenaOepTableList.GetHeadPosition();
	while (pos) {
		int  iMatch = 0;
		ArenaOepTable &x = m_ArenaOepTableList.GetNext(pos);
		// check for a match against this arena entry.
		// we start the checks using the most specific criteria and then in
		// several other checks we loosen the criteria.
		// the final criteria is if the arena entry has a zero for table number
		// then only the arena group number matching is sufficient for a match.
		iMatch |= (iDeptNo && iTableNo && x.deptno == iDeptNo && x.tableno == iTableNo && x.groupno == iGroupNo);
		iMatch |= (iDeptNo == 0 && iTableNo && x.tableno == iTableNo && x.groupno == iGroupNo);
		iMatch |= (iDeptNo == 0 && iTableNo && x.tableno == iTableNo && x.groupno == 0);
		iMatch |= (iDeptNo == 0 && iTableNo == 0 && x.groupno == iGroupNo);
		iMatch |= (x.tableno == 0 && x.groupno == iGroupNo);
		if (iMatch) {
			int  iIndex = ArenaOepTableArea.iIndex++; ArenaOepTableArea.iIndex %= 5;
			ArenaOepTableArea.table[iIndex] = x;
			ArenaOepResult y = {iIndex, x};
			return y;
		}
	}

	ArenaOepResult temp = {0, dummy};

	return  temp;
}

CFrameworkWndText::ArenaOepTable & CFrameworkWndText::SearchArenaOepTableListByDept (int iDeptNo, int iGroupNo, int iTableNo)
{
	static ArenaOepTable dummy = {0};

	POSITION pos = m_ArenaOepTableList.GetHeadPosition();
	while (pos) {
		ArenaOepTable &x = m_ArenaOepTableList.GetNext(pos);
		if (iTableNo && x.deptno == iDeptNo && x.tableno == iTableNo && x.groupno == iGroupNo) return x;
		else if (iGroupNo && x.deptno == iDeptNo && x.tableno == iTableNo && x.groupno == iGroupNo) return x;
		else if (x.deptno == iDeptNo) return x;
	}

	ArenaOepTable & temp = dummy;

	return  temp;
}


int CFrameworkWndText::ClearAndShowPopupWithImage()
{
	if (m_OEPCustDisplayImage.m_hWnd && m_ArenaOepTableList.GetCount() > 0) {
		// for m_nIdentifier == LCDWIN_ID_REG104 or CWindowTextType::TypeOEPCustDisplay.
		// myThis->m_aszCurrentOEPGroup contains the actual group so we want to find an image for
		// this group and present it.

		// for discussion about CImage and using images with CStatic see posting at URL:
		// https://stackoverflow.com/questions/51086298/lifecycle-and-resource-management-when-using-cimage-with-cstatic-to-display-chan

		CFrameworkWndText::ArenaOepTable & px = SearchArenaOepTableList (m_aszCurrentOEPGroup, m_aszCurrentOEPTable).x;

		if ((px.ulMap & 0x0804) == 0) return 0;    // no imagename specified nor backimage specified.

		int iLeftSide = (controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth);
		int iTopSide = (controlAttributes.m_nRow * CFrameworkWndButton::stdHeight);
		int iHeight = controlAttributes.m_nHeight;
		int iWidth = controlAttributes.m_nWidth;

		if (m_OEPCustDisplayImage.GetBitmap( ) == NULL ) {
			// lets create the bitmap for the window which we are going to use for the display
			// of imagery. By making sure that the CStatic has a bitmap of the correct size
			// we can update the imagery using the graphics primitives plus we can make sure
			// that the background color is correct. so this allows us to make our code easier.
			CImage CImgBitMap;

			if (CImgBitMap.Create (iWidth, iHeight, 32, BI_RGB)) {
				CBrush CImgBrush;
				CRect  rect (0, 0, iWidth, iHeight);
				CImgBrush.CreateSolidBrush (px.background);

				HDC  hDc = CImgBitMap.GetDC ();

				::FillRect(hDc, &rect, CImgBrush);
				CImgBitMap.ReleaseDC();

				::SetWindowPos (m_OEPCustDisplayImage.m_hWnd, HWND_NOTOPMOST, iLeftSide, iTopSide, iWidth, iHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
				m_OEPCustDisplayImage.SetBitmap( CImgBitMap.Detach() );
			}
		}

		CImage   CImg;
		HRESULT  hrCImg = -1;       // initialize the HRESULT for the CImag or foreground image as FAILED for now.

		CImage   CImgBack;
		HRESULT  hrCImgBack = -1;  // initialize the HRESULT for the CImgBack or background image as FAILED for now.

		CONST TCHAR * filePath;

		if (px.ulMap & 0x0800) {
			// background image was specified for this arena entry so lets fetch it if it exists.
			filePath = PifGetFilePath (px.backimage, "b", NULL);
			if (filePath) {
				hrCImgBack = CImgBack.Load( filePath );
			}
		}

		if (px.ulMap & 0x0004) {
			// foreground image was specified for this arena entry so lets fetch if if it exists.
			filePath = PifGetFilePath (px.imagename, "b", NULL);
			if (filePath) {
				hrCImg = CImg.Load( filePath );
			}
		}

		// if we have either a background image or a foreground image or both then lets proceed
		// with displaying one or both.
		if( SUCCEEDED (hrCImgBack) || SUCCEEDED(hrCImg)) {
			HRESULT  hr;

			m_CurrentArenaEntry = px;

			// if we have a background image then we will do that first.
			// background image always starts at top left coordinates of 0, 0.
			// background image always replaces anything that may already be there.
			if ( SUCCEEDED (hrCImgBack)) {
				int iImgWidth = CImgBack.GetWidth();
				int iImgHeight = CImgBack.GetHeight();
				int iSetWidth = (iWidth < iImgWidth) ? iWidth : iImgWidth;
				int iSetHeight = (iHeight < iImgHeight) ? iHeight : iImgHeight;

				if (m_OEPCustDisplayImage.GetBitmap( ) == NULL ) {
					m_OEPCustDisplayImage.SetBitmap( CImgBack.Detach() );
					::SetWindowPos (m_OEPCustDisplayImage.m_hWnd, HWND_NOTOPMOST, iLeftSide, iTopSide, iWidth, iHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
				} else {
					hr = CImgBack.StretchBlt (m_OEPCustDisplayImage.GetDC()->m_hDC, iLeftSide, iTopSide, iSetWidth, iSetHeight);
				}
			}


			if ( SUCCEEDED (hrCImg)) {
				int iImgWidth = CImg.GetWidth();
				int iImgHeight = CImg.GetHeight();
				int iSetWidth = (iWidth < iImgWidth) ? iWidth : iImgWidth;
				int iSetHeight = (iHeight < iImgHeight) ? iHeight : iImgHeight;
				int iTopLeftX = 0;
				int iTopLeftY = 0;


				if ((m_CurrentArenaEntry.vertalign & 0x8000) == 0) {
					iTopLeftY = m_CurrentArenaEntry.vertalign & 0x7fff;
				} else {
					// the value in vertalign is an indicator as to whether the image is
					// to be positioned with top, center, bottom so we have to do some
					// calculations to figure the pixel offset.
					// see position indicators as designated in RflConvertStr2Position().
					switch (m_CurrentArenaEntry.vertalign & ~0x8000) {
						case 1:        // center
							iTopLeftY = iHeight/2 - iImgHeight / 2;
							if (iTopLeftY < 0) iTopLeftY = 0;
							break;
						case 4:        // top
							iTopLeftY = 0;
							break;
						case 5:        // bottom
							iTopLeftY = iHeight - iImgHeight;
							if (iTopLeftY < 0) iTopLeftY = 0;
							break;
						default:
							break;
					}
				}

				if ((m_CurrentArenaEntry.horizalign & 0x8000) == 0) {
					iTopLeftX = m_CurrentArenaEntry.horizalign & 0x7fff;
				} else {
					// the value in horizalign is an indicator as to whether the image is
					// to be positioned with left, center, right so we have to do some
					// calculations to figure the pixel offset.
					// see position indicators as designated in RflConvertStr2Position().
					switch (m_CurrentArenaEntry.horizalign & ~0x8000) {
						case 1:        // center
							iTopLeftX = iWidth/2 - iImgWidth / 2;
							if (iTopLeftX < 0) iTopLeftX = 0;
							break;
						case 2:        // left
							iTopLeftX = 0;
							break;
						case 3:        // right
							iTopLeftX = iWidth - iImgWidth;
							if (iTopLeftX < 0) iTopLeftX = 0;
							break;
						default:
							break;
					}
				}

				int      iOverLayImage = m_CurrentArenaEntry.overlayimage || SUCCEEDED (hrCImgBack);

				if (m_OEPCustDisplayImage.GetBitmap( ) == NULL ) {
					m_OEPCustDisplayImage.SetBitmap( CImg.Detach() );
					::SetWindowPos (m_OEPCustDisplayImage.m_hWnd, HWND_NOTOPMOST, iLeftSide, iTopSide, iWidth, iHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
				} else {
					if (! iOverLayImage) {
						CBrush CImgBrush;
						CRect  rect (0, 0, iWidth, iHeight);

						CImgBrush.CreateSolidBrush (px.background);

						::FillRect(m_OEPCustDisplayImage.GetDC()->m_hDC, &rect, CImgBrush);
					}

					hr = CImg.StretchBlt (m_OEPCustDisplayImage.GetDC()->m_hDC, iTopLeftX, iTopLeftY, iSetWidth, iSetHeight);
				}
			} else {
				char xBuff[128];
				sprintf (xBuff, "==ERROR: CFrameworkWndText::ClearAndShowPopupWithImage() image load failed 0x%8.8x", hrCImg);
				NHPOS_ASSERT_TEXT(SUCCEEDED(hrCImg), xBuff);
			}
			return 1;
		}
	}

	return 0;
}

void CFrameworkWndText::DisplayPopupControlsBack (BOOL bShouldExist)
{
	CFrameworkWndItem   *pParentWndItem = NULL;
	int                 iDialogId = CFrameworkWndButton::SPL_BTN_BACK;
	UCHAR               uchTransAddress = TRN_BACK_OEP_ADR;

	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
	if (pParentWndItem == 0) {
		return;
	}

	CFrameworkWndButton *pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow (iDialogId);
	if (pChildWndButton) {
		if (! bShouldExist) delete pChildWndButton;
	} else if (bShouldExist) {
		pChildWndButton = new CFrameworkWndButton(iDialogId, ((textAttributes.oepBtnDown - 1) * textAttributes.oepBtnHeight),
										0, textAttributes.oepBtnWidth, textAttributes.oepBtnHeight);

		if (pChildWndButton) {
			PARATRANSMNEMO paraTransMnemo = { 0 };

			paraTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
			paraTransMnemo.uchAddress = uchTransAddress;
			BlFwIfParaRead(&paraTransMnemo);

			pChildWndButton->m_DialogId = iDialogId;
			pChildWndButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeOEPEntry;
			strcpy((char*)pChildWndButton->btnAttributes.m_myActionUnion.data.PLU, "8");

			pChildWndButton->myCaption = paraTransMnemo.aszMnemonics;
			pChildWndButton->controlAttributes.m_usWidthMultiplier = textAttributes.oepBtnWidth;
			pChildWndButton->controlAttributes.m_usHeightMultiplier = textAttributes.oepBtnHeight;
			pChildWndButton->controlAttributes.m_colorFace = textAttributes.oepBtnColor;//Set the button's Face Color
			pChildWndButton->btnAttributes.m_myShape = textAttributes.oepBtnShape;
			pChildWndButton->btnAttributes.m_myPattern = textAttributes.oepBtnPattern;
			pChildWndButton->controlAttributes.m_colorText = textAttributes.oepBtnFontColor;
			pChildWndButton->controlAttributes.lfControlFont = textAttributes.oepBtnFont;
			pChildWndButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
			//adding icon support for OEP window buttons
			pChildWndButton->btnAttributes.horIconAlignment = textAttributes.oepBtnIconHorizAlign;
			pChildWndButton->btnAttributes.vertIconAlignment = textAttributes.oepBtnIconVertAlign;
			
			pChildWndButton->WindowCreate((CWindowControl *)pParentWndItem, pChildWndButton->m_DialogId, 100);
			pChildWndButton->UpdateWindow();

			pChildWndButton->ChangeRefreshIcon(textAttributes.oepIconFileName);
		}
	}
}

void CFrameworkWndText::DisplayPopupControlsDone (BOOL bShouldExist)
{
	CFrameworkWndItem   *pParentWndItem = NULL;
	int                 iDialogId = CFrameworkWndButton::SPL_BTN_DONE;
	UCHAR               uchTransAddress = TRN_NONE_DONE_ADR;

	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
	if (pParentWndItem == 0) {
		return;
	}

	CFrameworkWndButton *pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(iDialogId);
	if (pChildWndButton) {
		if (! bShouldExist) delete pChildWndButton;
	} else if (bShouldExist) {
		pChildWndButton = new CFrameworkWndButton(iDialogId, ((textAttributes.oepBtnDown - 1) * textAttributes.oepBtnHeight),
											((textAttributes.oepBtnAcross - 2) * textAttributes.oepBtnWidth),
											textAttributes.oepBtnWidth, textAttributes.oepBtnHeight);

		if (pChildWndButton) {
			PARATRANSMNEMO paraTransMnemo = { 0 };

			paraTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
			paraTransMnemo.uchAddress = uchTransAddress;
			BlFwIfParaRead(&paraTransMnemo);

			pChildWndButton->m_DialogId = iDialogId;
			pChildWndButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeOEPEntry;
			strcpy((char*)pChildWndButton->btnAttributes.m_myActionUnion.data.PLU, "7");

			pChildWndButton->myCaption = paraTransMnemo.aszMnemonics;
			pChildWndButton->controlAttributes.m_usWidthMultiplier = textAttributes.oepBtnWidth;
			pChildWndButton->controlAttributes.m_usHeightMultiplier = textAttributes.oepBtnHeight;
			pChildWndButton->controlAttributes.m_colorFace = textAttributes.oepBtnColor;//Set the button's Face Color
			pChildWndButton->btnAttributes.m_myShape = textAttributes.oepBtnShape;
			pChildWndButton->btnAttributes.m_myPattern = textAttributes.oepBtnPattern;
			pChildWndButton->controlAttributes.m_colorText = textAttributes.oepBtnFontColor;
			pChildWndButton->controlAttributes.lfControlFont = textAttributes.oepBtnFont;
			pChildWndButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
			//adding icon support for OEP window buttons
			pChildWndButton->btnAttributes.horIconAlignment = textAttributes.oepBtnIconHorizAlign;
			pChildWndButton->btnAttributes.vertIconAlignment = textAttributes.oepBtnIconVertAlign;
			
			pChildWndButton->WindowCreate((CWindowControl *)pParentWndItem, pChildWndButton->m_DialogId, 100);
			pChildWndButton->UpdateWindow();

			pChildWndButton->ChangeRefreshIcon(textAttributes.oepIconFileName);
		}
	}
}

void CFrameworkWndText::DisplayPopupControlsMore (BOOL bShouldExist)
{
	CFrameworkWndItem   *pParentWndItem = NULL;
	int                 iDialogId = CFrameworkWndButton::SPL_BTN_MORE;
	UCHAR               uchTransAddress = TRN_CONTINUE_ADR;

	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
	if (pParentWndItem == 0) {
		return;
	}

	CFrameworkWndButton *pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(iDialogId);
	if (pChildWndButton) {
		if (! bShouldExist) delete pChildWndButton;
	} else if (bShouldExist) {
		pChildWndButton = new CFrameworkWndButton(iDialogId, ((textAttributes.oepBtnDown - 1) * textAttributes.oepBtnHeight),
											(1 * textAttributes.oepBtnWidth),
											textAttributes.oepBtnWidth, textAttributes.oepBtnHeight);

		if (pChildWndButton) {
			PARATRANSMNEMO paraTransMnemo = { 0 };

			paraTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
			paraTransMnemo.uchAddress = uchTransAddress;
			BlFwIfParaRead(&paraTransMnemo);

			pChildWndButton->m_DialogId = iDialogId;
			pChildWndButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeOEPEntry;
			strcpy((char*)pChildWndButton->btnAttributes.m_myActionUnion.data.PLU, "9");

			pChildWndButton->myCaption = paraTransMnemo.aszMnemonics;
			pChildWndButton->controlAttributes.m_usWidthMultiplier = textAttributes.oepBtnWidth;
			pChildWndButton->controlAttributes.m_usHeightMultiplier = textAttributes.oepBtnHeight;
			pChildWndButton->controlAttributes.m_colorFace = textAttributes.oepBtnColor;//Set the button's Face Color
			pChildWndButton->btnAttributes.m_myShape = textAttributes.oepBtnShape;
			pChildWndButton->btnAttributes.m_myPattern = textAttributes.oepBtnPattern;
			pChildWndButton->controlAttributes.m_colorText = textAttributes.oepBtnFontColor;
			pChildWndButton->controlAttributes.lfControlFont = textAttributes.oepBtnFont;
			pChildWndButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
			//adding icon support for OEP window buttons
			pChildWndButton->btnAttributes.horIconAlignment = textAttributes.oepBtnIconHorizAlign;
			pChildWndButton->btnAttributes.vertIconAlignment = textAttributes.oepBtnIconVertAlign;
			
			pChildWndButton->WindowCreate((CWindowControl *)pParentWndItem, pChildWndButton->m_DialogId, 100);
			pChildWndButton->UpdateWindow();

			pChildWndButton->ChangeRefreshIcon(textAttributes.oepIconFileName);
		}
	}
}

void CFrameworkWndText::DisplayPopup(int nRow, TCHAR *tempText)
{
	UCHAR	selectionCommand[6];
	CFrameworkWndItem   *pParentWndItem = NULL;
	CFrameworkWndItem	*pPreviousWndItem = NULL;
	CFrameworkWndButton *pChildWndButton;
	CFrameworkWndButton *button;


	// The OEP text that would ordinarily show up in a text window is being changed from
	// being a series of text lines displayed to a set of buttons being displayed.
	// The text will have the form of "nn description" such as "11 RARE" or "12 MEDIUM".
	// The two digits will be in numeric order beginning with 11.
	// If we don't have leading digits and if there is no text on the line then this
	// is not something we should turn into a button so we ignore this item.

	int   niButtonPos = _ttoi (tempText);
	TCHAR aszButtonText [64] = {0};
	int   iButtonCount = nRow;	  // use the row number as the button count
	bool  continueButton = /*FALSE*/TRUE;  //Using a continue button or not

	selectionCommand[1] = 0;
	selectionCommand[2] = 0;
	selectionCommand[3] = 0;
	selectionCommand[4] = 0;
	selectionCommand[5] = 0;

	if (niButtonPos) {
		// Old style was a two digit selection number.  However with new functionality for Amtrak
		// to repurpose the OEP window for display of Preauths that need to be finalized, may be
		// more than 2 digits now.
		TCHAR  *pTempText = tempText+3;

		selectionCommand[0] = (UCHAR)tempText[0];
		selectionCommand[1] = (UCHAR)tempText[1];
		if (tempText[2] != ' ') {
			selectionCommand[2] = (UCHAR)tempText[2];
			pTempText = tempText+4;
			if (tempText[3] != ' ') {
				selectionCommand[3] = (UCHAR)tempText[3];
				pTempText = tempText+5;
				if (tempText[4] != ' ') {
					selectionCommand[4] = (UCHAR)tempText[4];
					pTempText = tempText+6;
				}
			}
		}
		_tcsncpy (aszButtonText, pTempText, 23);
		for (int k = 22; k >= 0 && aszButtonText[k] == TEXT(' '); k--) {
			aszButtonText[k] = 0;
		}
	}
	else {
		// Assume this is a Continue though it may not be.  If not,
		// the aszButtonText will be an empty string so the button will
		// not be actually created.
		selectionCommand[0] = '9';
		continueButton = TRUE;
		_tcsncpy (aszButtonText, tempText, 27);
		aszButtonText[27] = 0;
		CString myTempString (aszButtonText);
		myTempString.TrimRight();
		myTempString.TrimLeft();
		_tcsncpy (aszButtonText, myTempString, 27);
	}

	// If there was not text on the line of text other than spaces,
	// then there is nothing further to do.
	if (!niButtonPos && !aszButtonText[0]) {
		return;
	}

	// we will be using this window to display with, so set it to pParentWndItem for buttons to populate
	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;

	ASSERT(pParentWndItem);

	if (pParentWndItem == 0) {
		return;
	}

	// if we are receiving messages to display the popup window
	// check to see if the data is available.
	// If it is not, we have not received all needed data.
	// The area will be spaces if no Mnemonic data has been copied
	// When the previous buttons are not needed the text area is reset to all spaces.
	// When this happens, we delete all the old buttons

	if (nRow == 0) {
		pParentWndItem->ShowWindow(SW_SHOW);
		pParentWndItem->BringWindowToTop();

#if 0
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_BACK)){	//More/Back (up)
			delete pChildWndButton;
		}
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_MORE)){	//Continue More (down)
			delete pChildWndButton;
		}
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_DONE)){  //DONE
			delete pChildWndButton;
		}
#endif
		for(int l = 0; l < m_nButtonCount; l++){
			if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_START + l)){
				delete pChildWndButton;
			}
		}
		m_nButtonCount = 0;
	}

	// If there is already a button for this line of text, then there is
	// nothing to do.  Testing has uncovered cases where sometimes the
	// row of text will be duplicated perhaps as a part of a repaint type
	// of operation.
	pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_START + iButtonCount);

	if(pChildWndButton) {
		TRACE3("%S(%d): duplicate button, iButtonCount = %d, ", __FILE__, __LINE__, iButtonCount);
		TRACE1(" caption %s\n", pChildWndButton->myCaption);
		delete pChildWndButton;
	}

//OEP-Sizing - CSMALL
	// set maximum buttons for this window based on
	//	#buttonsAcross * #buttonsDown, then subtract
	//	two rows (#buttonsAcross) to make room for 'continue'
	//	and 'done' buttons.
	//	maxOEPButtons = this->textAttributes.oepBtnAcross * this->textAttributes.oepBtnDown - 2*this->textAttributes.oepBtnAcross;
	//	maxOEPButtons = STD_OEP_MAX_NUM;
	
	// At this point it is either a button with a specific value such as "11 RARE"
	// or its a special button such as "Continue".
	// Now that we know its a valid selection, lets make a button for this item.

	m_nButtonCount++;

	button = new CFrameworkWndButton((CFrameworkWndButton::SPL_BTN_START + iButtonCount),
									((iButtonCount/textAttributes.oepBtnAcross) * textAttributes.oepBtnHeight),
									((iButtonCount%textAttributes.oepBtnAcross) * textAttributes.oepBtnWidth),
									textAttributes.oepBtnWidth, textAttributes.oepBtnHeight);

	button->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeOEPEntry;
	strcpy((char*)button->btnAttributes.m_myActionUnion.data.PLU, (char*)selectionCommand);

	button->myCaption = aszButtonText;
	button->controlAttributes.m_usWidthMultiplier = textAttributes.oepBtnWidth;
	button->controlAttributes.m_usHeightMultiplier = textAttributes.oepBtnHeight;
	button->controlAttributes.m_colorFace = textAttributes.oepBtnColor;//Set the button's Face Color
	button->btnAttributes.m_myShape = textAttributes.oepBtnShape;
	button->btnAttributes.m_myPattern = textAttributes.oepBtnPattern;
	button->controlAttributes.m_colorText = textAttributes.oepBtnFontColor;
	button->controlAttributes.lfControlFont = textAttributes.oepBtnFont;
	button->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
	button->m_DialogId = (CFrameworkWndButton::SPL_BTN_START + iButtonCount);
	//adding icon support for OEP window buttons
	button->btnAttributes.horIconAlignment = textAttributes.oepBtnIconHorizAlign;
	button->btnAttributes.vertIconAlignment = textAttributes.oepBtnIconVertAlign;
	

	button->WindowCreate((CWindowControl *)pParentWndItem, button->m_DialogId, 100);
	button->UpdateWindow();
	
	button->ChangeRefreshIcon(textAttributes.oepIconFileName);	// icon support, calling this function must happen after window creation (to avoid assert problems due to internal calls to invalidate())

#if 0
	if (pParentWndItem->windowAttributes.busRules.NumOfPicks > 0) {
		CFrameworkWndLabel *countdownlabel;

		countdownlabel = new CFrameworkWndLabel(CFrameworkWndButton::SPL_BTN_LABEL, ((textAttributes.oepBtnDown - 1) * textAttributes.oepBtnHeight),
											((textAttributes.oepBtnAcross - 1) * textAttributes.oepBtnWidth),
											textAttributes.oepBtnWidth, textAttributes.oepBtnHeight);

		countdownlabel->myCaption = _T("xx");
		countdownlabel->controlAttributes.m_usWidthMultiplier = textAttributes.oepBtnWidth;
		countdownlabel->controlAttributes.m_usHeightMultiplier = textAttributes.oepBtnHeight;
		countdownlabel->controlAttributes.m_colorFace = textAttributes.oepBtnColor;//Set the button's Face Color
		countdownlabel->controlAttributes.m_colorText = textAttributes.oepBtnFontColor;
		countdownlabel->controlAttributes.lfControlFont = textAttributes.oepBtnFont;
		countdownlabel->labelAttributes.capAlignment = CWindowLabel::CaptionAlignment::Right;
		countdownlabel->m_DialogId = CFrameworkWndButton::SPL_BTN_LABEL;

		countdownlabel->WindowCreate((CWindowControl *)pParentWndItem, countdownlabel->m_DialogId, 100);
		countdownlabel->UpdateWindow();
	}
#endif
}


// Delete the Popup OEP window that is being displayed.
void CFrameworkWndText::DeletePopup()
{
	CFrameworkWndItem* pParentWndItem;
	CFrameworkWndButton* pChildWndButton;
	pParentWndItem = (this->m_hWnd) ? (CFrameworkWndItem *)GetParent() : 0;
	m_bBackOEP = FALSE;

	TRACE3("%S(%d): CFrameworkWndButton::DeletePopup(). Deleting window named '%s'\n", __FILE__, __LINE__, myName);

	if (pParentWndItem) {
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_BACK)){
			delete pChildWndButton;
		}
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_MORE)){
			delete pChildWndButton;
		}
		if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_DONE)){
			delete pChildWndButton;
		}

		for(int i = 0; i < m_nButtonCount; i++){
			if(pChildWndButton = (CFrameworkWndButton*)pParentWndItem->GetDescendantWindow(CFrameworkWndButton::SPL_BTN_START + i)){
				delete pChildWndButton;
			}
		}
		m_nButtonCount = 0;
	}
}

void CFrameworkWndText::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_nIdentifier == LCDWIN_ID_REG100 ||
		m_nIdentifier == LCDWIN_ID_REG200 ||
		m_nIdentifier == LCDWIN_ID_REG201 ||
		m_nIdentifier == LCDWIN_ID_REG202){
		SelectItem(nFlags, point);
	}else{
		CWindowText::OnLButtonDown(nFlags, point);
	}
}

void CFrameworkWndText::SelectItem(UINT nFlags, CPoint point)
{
	int iHeightOfLine;
	int iLineCount = 0, iMaxRow;
	USHORT	usHandle;//vos window handle
	USHORT	uchMldSystem;

	iHeightOfLine = (m_rectClient.bottom - m_rectClient.top  - GetMarginTop()  - GetMarginButtom())/m_nRowsInCtl;

	for(int i = iHeightOfLine; i <= m_rectClient.bottom; i += iHeightOfLine, iLineCount++){
		if(point.y < i){
			if(iLineCount >= m_nRowsInCtl)
				iLineCount = m_nRowsInCtl - 1;
			break;
		}
	}

	//if the Line count indicates more than the available lines
	//set to the last line because there can be "extra" space
	//at the bottom of the reciept display that can be touched
	//when trying to select the last line
	//03-11-05 Update:
	//Use a function to check the status of the terminal to determine the
	//number of rows the terminal has. If the terminal is in the Store/Recall
	//system and using the 3scroll display option, the screen only has 19 lines, 
	//as opposed to 20 regularly.
	uchMldSystem = BlFwIfCheckMldSystemStatus();

	switch(uchMldSystem)
	{
		case MLD_PRECHECK_SYSTEM:
		case MLD_SUPER_MODE:
		case MLD_DRIVE_THROUGH_1SCROLL:
			iMaxRow = MLD_NOR_DSP1_ROW_LEN;
			break;
		
		case MLD_DRIVE_THROUGH_3SCROLL:
		case MLD_SPLIT_CHECK_SYSTEM:
			iMaxRow = MLD_DRV3_DSP1_ROW_LEN;
			break;

		default:
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_WNDTXT_MLDSYS_ERR);
			return;
			break;
	}

	if(iLineCount >= iMaxRow){
		iLineCount = iMaxRow - 1;
	}

	switch(m_nIdentifier)
	{
		case LCDWIN_ID_REG200:
			BlFwIfMldSetReceiptWindow(MLD_SCROLL_1);
			BlFwIfMldNotifyCursorMoved(MLD_SCROLL_1);
			usHandle = BlFwIfMldGetVosWinHandle(MLD_SCROLL_1);
			break;

		case LCDWIN_ID_REG201:
			BlFwIfMldSetReceiptWindow(MLD_SCROLL_2);
			BlFwIfMldNotifyCursorMoved(MLD_SCROLL_2);
			usHandle = BlFwIfMldGetVosWinHandle(MLD_SCROLL_2);
			break;

		case LCDWIN_ID_REG202:
			BlFwIfMldSetReceiptWindow(MLD_SCROLL_3);
			BlFwIfMldNotifyCursorMoved(MLD_SCROLL_3);
			usHandle = BlFwIfMldGetVosWinHandle(MLD_SCROLL_3);
			break;

		case LCDWIN_ID_REG100:         // CWindowText::TypeSnglReceiptMain
			BlFwIfMldNotifyCursorMoved(MLD_SCROLL_1);
			usHandle = BlFwIfMldGetVosWinHandle(MLD_SCROLL_1);
			break;

		default:
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_WNDTXT_WNDHNDL_ERR);
			return;
	}

	VosSetCurPos(usHandle, iLineCount, 0);

}

void CFrameworkWndText::OnDestroy()
{
	TRACE2("%S(%d): CFrameworkWndText::OnDestroy().\n", __FILE__, __LINE__);
	CWindowText::OnDestroy();
}


// The standard use of this function is to establish a callback to implement setting up
// the OEP group list. See use of BlSetFrameworkFunc(CMD_SET_OEP_GROUPS, BlFwSetOepGroup);
//
// Also see function BlFwIfSetOEPGroups() is provides the interface from BusinessLogic
// to Framework to provide the list of OEP groups to the FrameworkWndText class functionality.
//__declspec(dllexport)
BOOL CALLBACK BlFwSetOepGroup(UCHAR * pszGroups, UCHAR uchTableNo)    // call back to implement BlFwIfSetOEPGroups().
{
	//clear the static work area that we hold the OEP
	//group numbers in.  so that we dont get any incorrect
	//information
	pMatchedWndItem = NULL;
	useDefaultWindow = TRUE;
	switch(*pszGroups)
	{
		case BL_OEPGROUP_INCREMENT:
			if (currentGroupCount < STD_NUM_OEP_GROUPS)
				currentGroupCount++;
			break;
		case BL_OEPGROUP_RESET:
			currentGroupCount = 0;
			{
				// FetchLastcurrentFrameworkWndTextWindow () may return a NULL pointer if there is no
				// last current window.  If it does, then we use the window we have.
				CFrameworkWndText *xThis = CFrameworkWndTextSubWindow::FetchLastCurrentFrameworkWndTextWindow();
				if (xThis && xThis->GetParent())
					PostMessage (xThis->m_hWnd, WM_APP_OEP_IMAGERY, 0, 0);    // send message to OnOepImageryChange() we are doing reset of group.
			}
			break;
		// fix for SR 867 - prevents moving forward in sequence
		//	of OEP Groups when an error is incountered.  CSMALL 1/31/06
		case BL_OEPGROUP_DECREMENT:
			if (currentGroupCount > 0)
				currentGroupCount--; // in the case that there was a 'Prohibited' error
			break;
		case BL_OEPGROUP_DEFAULTGROUP:
			currentGroupCount = 0;
			memset(&aszOEPGroups, 0x00, sizeof(aszOEPGroups));
			aOEPTableNo = 0;
			break;
		default:
			memset(&aszOEPGroups, 0x00, sizeof(aszOEPGroups));
			//copy the new OEP groups into the static area
			aOEPTableNo = uchTableNo;
			memcpy(&aszOEPGroups, pszGroups, STD_NUM_OEP_GROUPS);
			currentGroupCount = 0;
			break;
	}

	return TRUE;
}

// The standard use of this function is to establish a callback to implement setting up
// the OEP group list. See use of BlSetFrameworkFunc(CMD_DISPLAY_OEP_IMAGE, BlFwDisplayImagery);
//
// Also see function BlFwIfSetOEPGroups() is provides the interface from BusinessLogic
// to Framework to provide the list of OEP groups to the FrameworkWndText class functionality.
//__declspec(dllexport)
BOOL CALLBACK BlFwDisplayOEPImagery (UCHAR uchCommand, UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo)    // call back to implement BlFwIfDisplayOEPImagery().
{
	if (CFrameworkWndText::m_OEPCustDisplayImage.m_hWnd) {
//		CFrameworkWndText::ArenaOepResult x = CFrameworkWndText::SearchArenaOepTableList (uchGroupNo, uchTableNo, uchDeptNo);

		CWnd * p1 = CFrameworkWndText::m_OEPCustDisplayImage.GetParent();

		LPARAM  lParam = 0;    // x.iIndex;

		lParam = (uchDeptNo << 16) | (uchTableNo << 8) | uchGroupNo;
//		if (x.x.ulMap) PostMessage (p1->m_hWnd, WM_APP_OEP_IMAGERY, uchCommand, lParam);    // send message to OnOepImageryChange() we are doing reset of group.
		PostMessage (p1->m_hWnd, WM_APP_OEP_IMAGERY, uchCommand, lParam);    // send message to OnOepImageryChange() we are doing reset of group.
	}

	return TRUE;
}

//__declspec(dllexport)
int CALLBACK BlFwGetMaxOepItems(UCHAR *pszMaxButtons)
{
	int m_nIdentifier = LCDWIN_ID_REG102;

	CFrameworkWndText *myThis = DetermineFrameworkWndTextWindow (m_nIdentifier, 0);

	if (myThis)
	{
		maxOEPButtons = myThis->textAttributes.oepBtnAcross * 
						myThis->textAttributes.oepBtnDown - 
						2 * myThis->textAttributes.oepBtnAcross;
		if (maxOEPButtons > 120 || maxOEPButtons < 0) {
			ASSERT(0);
			maxOEPButtons = 0;
		}
	}
	else
	{
		maxOEPButtons = 0;
	}
	if (pszMaxButtons)
	{
		*pszMaxButtons = maxOEPButtons;
	}
	return maxOEPButtons;
}
