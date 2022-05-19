// FrameworkWndDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Framework.h"
#include "FrameworkWndDoc.h"
#include "FrameworkWndCtrl.h"
#include "FrameworkWndButton.h"
#include "FrameworkWndItem.h"
#include "FrameworkWndText.h"
#include "Evs.h"
#include "blfwif.h"
#include "uie.h"
#include "appllog.h"
//#include "DFPRExports.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FRAMEWORK_WND_DOC_SCHEMA 1
IMPLEMENT_SERIAL(CFrameworkWndDoc,CWnd, FRAMEWORK_WND_DOC_SCHEMA);

static const ULONG  FRAME_TIMER_CMD_RELOADLAYOUT = 2000;
static const ULONG  FRAME_TIMER_CMD_RELOADOEP = 3000;
static const ULONG  FRAME_TIMER_CMD_AUTO_SIGN_OUT = 4000;
static const ULONG  FRAME_TIMER_CMD_FORCE_SHUTDOWN = 5000;
static const ULONG  FRAME_TIMER_CMD_SECONDS_TIMER = 6000;


/////////////////////////////////////////////////////////////////////////////
// CWindowDocumentExt

//----------   Callback functions used to implement Framework functionality from BusinessLogic

__declspec(dllexport)
BOOL CALLBACK FwReloadLayout2()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bResult;
	CFrameworkWndDoc* pFrameworkWndDoc;

	pFrameworkWndDoc = (CFrameworkWndDoc *)BlFwIfGetFrameworkTouchWindow ();

	if(pFrameworkWndDoc)
	{
		pFrameworkWndDoc->KillTimer(pFrameworkWndDoc->m_nTimer);
		pFrameworkWndDoc->m_nTimer = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
		if (pFrameworkWndDoc->m_nTimer == 0) {
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_6);
			Sleep(0);
			pFrameworkWndDoc->m_nTimer = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
		}
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}


__declspec(dllexport)
BOOL CALLBACK FwReloadOepWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bResult;
	CFrameworkWndDoc* pFrameworkWndDoc;

	pFrameworkWndDoc = (CFrameworkWndDoc *)BlFwIfGetFrameworkTouchWindow ();

	if(pFrameworkWndDoc != 0)
	{
		pFrameworkWndDoc->KillTimer(pFrameworkWndDoc->m_nTimerOep);
		pFrameworkWndDoc->m_nTimerOep = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_RELOADOEP, 3000, NULL);
		if (pFrameworkWndDoc->m_nTimerOep == 0) {
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_7);
			Sleep(0);
			pFrameworkWndDoc->m_nTimerOep = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_RELOADOEP, 3000, NULL);
		}
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

/**
	The implementation of the actual Auto Sign Out is in CasForceAutoSignOut().
	That function provides the source for the FSC_AUTO_SIGN_OUT key press.
*/
__declspec(dllexport)
VOID CALLBACK  SetAutoSignOutTime (ULONG time, SHORT mode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFrameworkWndDoc* pFrameworkWndDoc;

	pFrameworkWndDoc = (CFrameworkWndDoc *)BlFwIfGetFrameworkTouchWindow ();

	NHPOS_ASSERT_TEXT(pFrameworkWndDoc != 0, "SetAutoSignOutTime(): AfxGetMainWnd()");

	if (pFrameworkWndDoc != 0) {
		pFrameworkWndDoc->KillTimer(pFrameworkWndDoc->m_ntimerSignout);
		if(time != 0){
			pFrameworkWndDoc->m_ntimerSignout = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_AUTO_SIGN_OUT, time, NULL);
			if(mode != BLFWIF_AUTO_MODE_NO_CHANGE)
				pFrameworkWndDoc->Mode = mode;
		}
	}
}

/**
	The implementation of the actual Auto Sign Out is in CasForceAutoSignOut().
	That function provides the source for the FSC_AUTO_SIGN_OUT key press.
*/
__declspec(dllexport)
VOID CALLBACK  PerformForceShutdown ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFrameworkWndDoc* pFrameworkWndDoc;

	pFrameworkWndDoc = (CFrameworkWndDoc *)BlFwIfGetFrameworkTouchWindow ();

	NHPOS_ASSERT_TEXT(pFrameworkWndDoc != 0, "PerformForceShutdown(): AfxGetMainWnd()");

	if (pFrameworkWndDoc != 0) {
		if (pFrameworkWndDoc->m_ntimerForceShutdown)
			pFrameworkWndDoc->KillTimer(pFrameworkWndDoc->m_ntimerForceShutdown);

		pFrameworkWndDoc->m_ntimerForceShutdown = pFrameworkWndDoc->SetTimer(FRAME_TIMER_CMD_FORCE_SHUTDOWN, 8000, NULL);
	}
}

/**
	The implementation of the actual Auto Sign Out is in CasForceAutoSignOut().
	That function provides the source for the FSC_AUTO_SIGN_OUT key press.
*/
__declspec(dllexport)
VOID CALLBACK  FwProcessFrameworkMessage (void  *pFrameworkObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFrameworkWndDoc* pFrameworkWndDoc;

	pFrameworkWndDoc = (CFrameworkWndDoc *)BlFwIfGetFrameworkTouchWindow ();

	NHPOS_ASSERT_TEXT(pFrameworkWndDoc != 0, "FwProcessFrameworkMessage(): AfxGetMainWnd()");

	if (pFrameworkWndDoc != 0) {
		pFrameworkWndDoc->InternalProcessFrameworkMessage (pFrameworkObject);
	}
}

#if 0
	// Following source is from an experiment that is to be continued for the
	// downloading and unpacking of a PEP archive from a web server. The idea
	// is to provide a URL of the PEP archive, pull it down, unpack it, replace
	// the various files that need to be replaced, and then restart GenPOS.
	// Still have work to do in the following areas:
	//  - checking and unpacking the PEP archive
	//  - copying the particular components needed to the Database folder
	//  - restarting GenPOS
	//
	// It would seem that this would only apply to a Master Terminal since a
	// Satellite Terminal should obtain its provisioning from the Master Terminal.
	//
	//       Richard Chambers, Oct-02-2018

#include <WinInet.h>  // from SDK

static int GetFile (HINTERNET hOpen, TCHAR * szURL, TCHAR *szFilePath, BYTE  szTemp[4096])   
{
    DWORD dwSize;
    TCHAR szHead[15];
    HINTERNET  hConnect;

    szHead[0] = '\0';
	szTemp[0] = 0;

    if ( !(hConnect = InternetOpenUrl( hOpen, szURL, szHead, 15, INTERNET_FLAG_DONT_CACHE, 0)))
    {
		DWORD  dwlasterror = GetLastError();
		if (dwlasterror == ERROR_INTERNET_NAME_NOT_RESOLVED) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_NAME_NOT_RESOLVED - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_TIMEOUT) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_TIMEOUT - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_SERVER_UNREACHABLE) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_SERVER_UNREACHABLE - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_OPERATION_CANCELLED) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_OPERATION_CANCELLED - check LAN connectivity."));
		} else {
			CString msg;
			msg.Format (_T("Error: GetLastError() returned %d."), dwlasterror);
			AfxMessageBox (msg);
		}
        return -2;
    }

	int iStatus = 0;
	DWORD  dwTotalSize = 0;

	SHORT  sFile = PifOpenFile (szFilePath, "tx");

	do {
		if (InternetReadFile (hConnect, szTemp, 4096,  &dwSize) )
		{
			if (dwSize == 0) break;

			PifWriteFile (sFile, szTemp, dwSize);
			dwTotalSize += dwSize;
		} else {
			DWORD  dwLastError = GetLastError();
			break;
		}
	} while (TRUE);

	PifCloseFile (sFile);
	return -4;
}

// Usage:
//	TCHAR *szURL = L"https://w3.georgiasouthern.edu/neighborhoodpos/downloads/components/PARAMINI";
//	TCHAR *szFilePath = L"Franktest";
//	BYTE  szTemp[4096];
//
//  DownloadURLImage (szUrl, szFilePath, szTemp);
//
static int DownloadURLImage (TCHAR * szURL, TCHAR *szFilePath, BYTE  szTemp[4096])
{
	int result = -1;
    HINTERNET hInternet;

    return result ;

    hInternet= InternetOpen (_T("FrameWork"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);

	if (hInternet) {
		result = GetFile (hInternet, szURL, szFilePath, szTemp) ; 
		InternetCloseHandle(hInternet);
	}
    return result ;
}

#endif

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

CFrameworkWndDoc::CFrameworkWndDoc(void) : CWindowDocument ()
{
	m_ntimerSecondsTimer = m_ntimerForceShutdown = m_nTimer = m_nTimerOep = 0;
	m_pMessageDialog = 0;
	m_myObjectArrayIndex = 0;
	m_wParamWindowTranslation = 0;
}

CFrameworkWndDoc::~CFrameworkWndDoc(void)
{

	if (m_pMessageDialog) {
		delete m_pMessageDialog;
		m_pMessageDialog = 0;
	}

	POSITION currentPos = listControls.GetHeadPosition ();
	POSITION currentPosLast = currentPos;

	while (currentPos) {
		CWindowControl *bi = listControls.GetNext (currentPos);
		ASSERT (bi);
		delete bi;
		listControls.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}

	/*make sure that this list is empty - this is nescessary because a default group
	is created when the document is created. We must clear this out before opening 
	a saved document*/
	currentPos = listGroups.GetHeadPosition ();
	currentPosLast = currentPos;
	while (currentPos) {
		CWindowGroup *wg = listGroups.GetNext (currentPos);
		ASSERT (wg);
		delete wg;
		listGroups.RemoveAt (currentPosLast);
		currentPosLast = currentPos;
	}
}

// Traverse the list of controls for this document.
// Any controls that contain dynamic PLU buttons or are
// OEP windows need to be refreshed in case there have
// been OEP group changes.
// This function RefreshOepWindows() is triggered by a FRAME_TIMER_CMD_RELOADOEP timer event.
//
// See also functions FwReloadOepWindow() and BlFwIfReloadOEPGroup() which provide
// the BusinessLogic access to this functionality.
void CFrameworkWndDoc::RefreshOepWindows (void)
{
	POSITION currentPos = listControls.GetHeadPosition ();

	NHPOS_NONASSERT_TEXT("CFrameworkWndDoc::RefreshOepWindows(): Start.");

	CFrameworkWndText::BuildArenaOepTableList();

	BlInitializeOepTable();
	while (currentPos) {
		CWindowControl *bi = listControls.GetNext (currentPos);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			pw->WindowRefreshOEPGroup ();
		}
	}
	NHPOS_NONASSERT_TEXT("CFrameworkWndDoc::RefreshOepWindows(): Completed.");

	PifSleep (1000);
	BlFinalizeAndClose();    // CliParaSaveFarData() update memory resident data snapshot and the ASSRTLOG file offset pointers.
}

BOOL  CFrameworkWndDoc::InitDocumentViewWnd (void)
{
	BOOL  bDocumentViewFound = FALSE;
	POSITION currentPos = listControls.GetHeadPosition ();
	m_pScrollDocument = 0;
	while (currentPos) {
		CWindowControl *bi = listControls.GetNext (currentPos);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			if (pw->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_DOCU_VIEW) {
				m_pScrollDocument = pw->m_pScrollDocument;
				bDocumentViewFound = TRUE;
				break;
			}
		}
	}
	return bDocumentViewFound;
}

BOOL CFrameworkWndDoc::CreateDocument (CWnd *pParentWnd, UINT nID, CRect *rectSize)
{
	BOOL bStatus = FALSE;

	// enumerate through the available displays attached so that we can display
	// windows on the requested monitor if the monitor is available.
	int iCount = CFrameworkWndText::EnumDisplayMonitors();

	bStatus = CWindowDocument::CreateDocument(pParentWnd, nID, rectSize);
	// connect the event subsystem
    if (::EvsConnect(SCRMODE_ID, (ULONG)m_hWnd) != EVS_OK)
    {
        ASSERT(FALSE);
    }

#ifndef _DEBUG
	ShowCursor(FALSE);
#endif
	BlSetFrameworkFunc(CMD_FWRELOADLAY_FUNC, FwReloadLayout2);
	BlSetFrameworkFunc(CMD_FWRELOADOEP_FUNC, FwReloadOepWindow);
	BlSetFrameworkFunc(CMD_AUTO_SIGN_OUT, SetAutoSignOutTime); 
	BlSetFrameworkFunc(CMD_FORCE_SHUTDOWN, PerformForceShutdown); 
	BlSetFrameworkFunc(CMD_FRAMEWORK_MSG, FwProcessFrameworkMessage); 
	
	Invalidate(FALSE);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

	// Now we schedule a refresh of the OEP and Dynamic PLU button
	// display to give the Notice Board subsystem the opportunity
	// to initialize and find out the cluster status, especially
	// the status of Master and Backup Master terminals.  The OEP
	// and Dynamic PLU button functionality depends on being able
	// to access the PLU and OEP table information from the Master.
	if (m_nTimerOep)
		KillTimer(m_nTimerOep);
	m_nTimerOep = SetTimer(FRAME_TIMER_CMD_RELOADOEP, 3000, NULL);
	if (m_nTimerOep == 0) {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_2);
		Sleep(0);
		m_nTimerOep = SetTimer(FRAME_TIMER_CMD_RELOADOEP, 3000, NULL);
	}

	if (m_ntimerSecondsTimer)
		KillTimer(m_ntimerSecondsTimer);
	m_ntimerSecondsTimer = SetTimer(FRAME_TIMER_CMD_SECONDS_TIMER, 1000, NULL);
	if (m_ntimerSecondsTimer == 0) {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_2);
		Sleep(0);
		m_ntimerSecondsTimer = SetTimer(FRAME_TIMER_CMD_SECONDS_TIMER, 1000, NULL);
		NHPOS_ASSERT_TEXT((m_ntimerSecondsTimer != 0), "Error starting FRAME_TIMER_CMD_SECONDS_TIMER utility timer.");
	}

	return bStatus;
}


void CFrameworkWndDoc::Serialize( CArchive& ar )
{
	UCHAR* pBuff = (UCHAR*) &documentAttributes;

	if (ar.IsStoring())
	{
		ASSERT(documentAttributes.signatureStart = 0xABCDEF87);

		documentAttributes.myStartupClientRect = documentAttributes.myClientRect;
		DocumentDef.Serialize(ar);
		ar<<sizeof(_tagWinDocAttributes);
		for(int i = 0; i < sizeof(_tagWinDocAttributes); i++){
			ar<<pBuff[i];
		}

		ar << listControls.GetCount ();

		POSITION currentPos = listControls.GetHeadPosition ();
		while (currentPos) {
			CWindowControl *pwc = listControls.GetNext (currentPos);
			ASSERT(pwc);
			pwc->Serialize (ar);
		}
		
		//store the number of elements in the group list
		ar<<listGroups.GetCount();
		
		CWindowGroup *pwg;
		POSITION currentGPPos = listGroups.GetHeadPosition ();
		//loop through the documents group list and store the data for each window group
		while (currentGPPos) {
			pwg = listGroups.GetNext (currentGPPos);
			ASSERT(pwg);
			pwg->Serialize (ar);
		}
	}
	else
	{
		//variables to hold the count of items in each of the documents lists
		int nCount;	
		int nGPCount;

		ULONG mySize;
		memset(&documentAttributes,0,sizeof(_tagWinDocAttributes));

		DocumentDef.Serialize(ar);
		ar>>mySize;

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (documentAttributes)) {
			xMySize = sizeof (documentAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}

		ASSERT(documentAttributes.signatureStart = 0xABCDEF87);
		//get the count of the documents control list
		ar >> nCount;

		POSITION currentPos = listControls.GetHeadPosition ();
		POSITION currentPosLast = currentPos;

		while (currentPos) {
			CWindowControl *bi = listControls.GetNext (currentPos);
			ASSERT (bi);
			delete bi;
			listControls.RemoveAt (currentPosLast);
			currentPosLast = currentPos;
		}

		CFrameworkWndControl pwc;
		//loop through all the stored controls and create them and add them to the doc's list
		for ( ; nCount > 0; nCount--) {
			CWindowControl * pc = pwc.SerializeNew (ar);
			ASSERT(pc);
			if (pc)
				listControls.AddTail (pc);
		}
		
		/*make sure that this list is empty - this is nescessary because a default group
		is created when the document is created. We must clear this out before opening 
		a saved document*/
		currentPos = listGroups.GetHeadPosition ();
		currentPosLast = currentPos;
		while (currentPos) {
			CWindowGroup *wg = listGroups.GetNext (currentPos);
			ASSERT (wg);
			delete wg;
			listGroups.RemoveAt (currentPosLast);
			currentPosLast = currentPos;
		}

		//get the number of items in the group list
		ar >> nGPCount;
		CWindowGroup *wg;
		//loop for the number of items in the list
		for( ; nGPCount > 0; nGPCount--){
			//create a new Window Group and retrieve the stored data
			wg = new CWindowGroup();
			ASSERT(wg);
			if (wg) {
				wg->Serialize(ar);
				//add the window group to the documents list
				listGroups.AddTail(wg);
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CFrameworkWndDoc, CWindowDocument)
	//{{AFX_MSG_MAP(CFrameworkWndDoc)
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WU_EVS_DFLT_LOAD, OnDefaultWinLoad)
    ON_MESSAGE(WU_EVS_POPUP_WINDOW, OnPopupWindowByName)
	ON_MESSAGE(WU_EVS_POPDOWN_WINDOW, OnPopdownWindowByName)
	ON_MESSAGE(WM_APP_CONNENGINE_MSG_RCVD, OnConnEngineMsgRcvd)
	ON_MESSAGE(WM_APP_XMLMSG_MSG_RCVD, OnXmlMsgRcvd)
	ON_MESSAGE(WM_APP_BIOMETRIC_MSG_RCVD, OnBiometricMsgRcvd)
	ON_MESSAGE(WM_APP_SHUTDOWN_MSG, OnShutdownMsgRcvd)
	ON_MESSAGE(WM_POWERBROADCAST, OnPowerMsgRcvd)
	ON_MESSAGE(WM_APP_SHOW_HIDE_GROUP, OnShowHideGroupMsgRcvd)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CFrameworkWndDoc message handlers

/*

	Search for an item that has an id as specified and for which contains
	in its area the specified row and column.

	We return the following values and meanings:
		0 - no item found that includes the specified row and column
		2 - window item found that includes specified row and column but not id
		3 - window item found that includes specified row and column and id, no sub-item found
		4 - window item fount that includes specified row and column and id, sub-item also found

 */
int CFrameworkWndDoc::SearchForItem (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		CRect jj(bi->controlAttributes.m_nColumn, bi->controlAttributes.m_nRow,
			bi->controlAttributes.m_nColumn + bi->controlAttributes.m_nWidth,
			bi->controlAttributes.m_nRow + bi->controlAttributes.m_nHeight);
		if (jj.PtInRect (CPoint (column, row))) {
			CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
			if (pw && (pw->controlAttributes.m_myId == id)) {
				CWindowControl *pwc = pw->GetDataFirst ();
				if (pwc) {
					CRect kk(pw->controlAttributes.m_nColumn+pwc->controlAttributes.m_nColumn,
						pw->controlAttributes.m_nRow+pwc->controlAttributes.m_nRow,
						pw->controlAttributes.m_nColumn+pwc->controlAttributes.m_nColumn + 
							pwc->controlAttributes.m_nWidth, 
						pw->controlAttributes.m_nRow+pwc->controlAttributes.m_nRow + pwc->controlAttributes.m_nHeight);
					while (pwc) {
						if (kk.PtInRect (CPoint (column, row))) {
							return 4;
						}
						pwc = pw->GetDataNext ();
					}
				}
				return 3;
			}
			//else if (pw)
			//	return 2;
		}
	}
	return 0;
}


CWindowControl * CFrameworkWndDoc::SearchForItemByName (TCHAR *tcWindowName)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			if ( _tcscmp (tcWindowName, pw->myName) == 0) {
				return bi;
			}
		}
	}
	return 0;
}

// SearchForItemByTypeApplyFunc() - search through the list of controls looking for a match to some
// criteria and then apply a function to any controls that match.
// This is a first version of this function used by function BlFwIfPopdownMessageAllVirtual() to implement
// layout window cleanup when doing a Sign-Out or an Auto Sign-Out (see function ItemSignOut() in signout.c.
// If a function pointer is specified then that function will be called for each matching item. The default
// function is the PopdownWindow() function.
CWindowControl * CFrameworkWndDoc::SearchForItemByTypeApplyFunc (int iType, int (*func)(CWindowControl *pw))
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			if ( ! pw->controlAttributes.isVirtualWindow) {  // is this CFrameworkWndItem a Popup Window or a Virtual Window.
				// this is a Popup Window so apply the function to it.
				if (func) {
					func (pw);
				} else {
					pw->PopdownWindow ();
				}
			}
		}
	}
	return 0;
}

CWindowControl * CFrameworkWndDoc::SearchForItemByNameInWindow (TCHAR *tcWindowName, TCHAR *tcItemName, CWindowControl *(*pWindowControlList))
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			if ( _tcscmp (tcWindowName, pw->myName) == 0) {
				CWindowControl *bi2 = pw->SearchForItemByName(tcItemName);
				if (pWindowControlList) {
					*pWindowControlList++ = pw;
					*pWindowControlList = bi2;
				}
				return bi2;
			}
		}
	}

	if (pWindowControlList) {
		*pWindowControlList++ = 0;
		*pWindowControlList = 0;
	}
	return 0;
}


CWindowControl * CFrameworkWndDoc::SearchForItemByPrefix (int WindowPrefix)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = listControls.GetNext (pos);
		ASSERT(bi);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw) {
			if (WindowPrefix == _ttoi(pw->myName)) {
				return bi;
			}
		}
	}
	return 0;
}

int CFrameworkWndDoc::SearchForItemCorner (int row, int column, UINT id)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		CRect jj(bi->controlAttributes.m_nColumn, bi->controlAttributes.m_nRow, bi->controlAttributes.m_nColumn + 1, bi->controlAttributes.m_nRow + 1);
		if (jj.PtInRect (CPoint (column, row))) {
			CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
			if (pw && (pw->controlAttributes.m_myId == id)) {
				CWindowControl *pwc = pw->GetDataFirst ();
				if (pwc) {
					CRect kk(pw->controlAttributes.m_nColumn+pwc->controlAttributes.m_nColumn,
							pw->controlAttributes.m_nRow+pwc->controlAttributes.m_nRow,
							pw->controlAttributes.m_nColumn+pwc->controlAttributes.m_nColumn + 1,
							pw->controlAttributes.m_nRow+pwc->controlAttributes.m_nRow + 1);
					while (pwc) {
						if (kk.PtInRect (CPoint (column, row))) {
							return 4;
						}
						pwc = pw->GetDataNext ();
					}
				}
				return 3;
			}
			//else if (pw)
			//	return 2;
		}
	}
	return 0;
}


//When a WM_CHAR message is picked up by the Window Document
//a message is set up and set to business logic so the Character
//is put into the program The type is BL_WM_CHAR and BlSendKeyboardData is
//a part of the Framwork Interface into business logic
void CFrameworkWndDoc::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	BL_NOTIFYDATA   BlNotify;
    _DEVICEINPUT*   pDeviceInput;

	pDeviceInput = &BlNotify.u.Data;

	pDeviceInput->ulDeviceId = BL_WM_CHAR;
	pDeviceInput->ulLength = _tcslen((TCHAR*)&nChar);
	pDeviceInput->pvAddr = &nChar;

	BlNotify.ulType = BL_INPUT_DATA;

	BlSendKeyboardData(&BlNotify, NULL);

  //	CWindowDocument::OnChar(nChar, nRepCnt, nFlags);
}

//this is the only CFrameworkWndDoc timer event
//if others are added addtional code will be needed
//to distinguish between them
void CFrameworkWndDoc::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CFileStatus status;//current status of PARAMLAY
	USHORT fsComStatus = 0;
	UCHAR  uchMode;
	BL_NOTIFYDATA   BlNotify;
	_DEVICEINPUT*   pDeviceInput;
	CHARDATA	CharData;
	LONG lreturn = 0;

	CharData.uchFlags = 0;

	if (nIDEvent == FRAME_TIMER_CMD_RELOADOEP)
	{
		KillTimer(m_nTimerOep);
		fsComStatus = BlGetNoticeBoardStatus(1);
		if ( !(fsComStatus & (BLI_STS_INQUIRY | BLI_STS_INQTIMEOUT | BLI_STS_STOP)) &&
			(fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)) )
		{
			RefreshOepWindows();		
		}
		else
		{
			m_nTimerOep = SetTimer(FRAME_TIMER_CMD_RELOADOEP, 2500, NULL);
			if (m_nTimerOep == 0) {
				PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_3);
				Sleep(0);
				m_nTimerOep = SetTimer(FRAME_TIMER_CMD_RELOADOEP, 2500, NULL);
			}
		}
		return;
	}
	else if (nIDEvent == FRAME_TIMER_CMD_RELOADLAYOUT)
	{
		/*
			OnTimer will only fire after a new layout has been sent to the terminal.
			Since this new layout may be a completely new layout than the one that 
			was previously on the terminal we should not allow the layout to reload.
			If the current terminal is not locked then we set the timer to fire every 
			second until the terminal is locked and at which time the layout can be 
			reloaded. ESMITH
		*/
		KillTimer(m_nTimer);
		uchMode = BlFwIfReadMode();
		if( ! ( uchMode == UIE_POSITION2 || uchMode == UIE_POSITION3 ) )
		{
			// Reschedule the timer event to reload the layout
			m_nTimer = SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
			if (m_nTimer == 0) {
				PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_4);
				Sleep(0);
				m_nTimer = SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
			}
		}
		else
		{
			// Set layout file name from system configuration ESMITH LAYOUT
			SHORT  sReloadHandle = -1;   // same as UIC_NOT_IDLE
			CString strLayoutFile, strPath(_T("\\FlashDisk\\NCR\\Saratoga\\Database\\"));
			CONST SYSCONFIG *pSysConfig = PifSysConfig();

			// Determine which layout file to use for this terminal.
			// Query the terminal information to obtain the layout file.
			// We check the name below to ensure it starts with PARAML as the
			// layout file name should be PARAMLA through PARAMLZ.  If it isn't
			// valid then we fall back to the old naming convention of if we are
			// touchscreen then use PARAMLA and if we are keyboard use PARAMLB.
			strLayoutFile = BlFwIfTerminalLayout (NULL);
			strLayoutFile.TrimLeft ();
			strLayoutFile.TrimRight ();

			if (strLayoutFile.IsEmpty () || strLayoutFile.Left(6).Compare (_T("PARAML"))) {
				if(pSysConfig->uchKeyType == KEYBOARD_TOUCH)
				{
					strLayoutFile = _T("PARAMLA");
				}
				else
				{
					strLayoutFile = _T("PARAMLB");
				}
			}

			strLayoutFile = strPath + strLayoutFile;

			//Open PARAMLAY so the status can be check
			//The second paramater of the default constructor requires that the 
			//sharing and access mode be specified. The share mode that is specified
			//is to allow read and write access to the layout file while it is being
			//reloaded to avoid sharing violations. 
			CFileException exTouch;
			CFile fTouchLayout;
			
			if (!fTouchLayout.Open (strLayoutFile, CFile::modeRead | CFile::shareDenyWrite, &exTouch)) {
				TCHAR szError[258];
				exTouch.GetErrorMessage(szError, 256);
				NHPOS_ASSERT_TEXT(0, "Error opening layout file.");
				return;
			}

			// get a keyboard and terminal lock so that we can reload the layout.
			// if we can not get the lock then something else has it and we need to wait.
			sReloadHandle = BlCheckAndSetTerminalLock(); //check to see terminal locked (not Idle)
			if(sReloadHandle == -1) { //if not idle, UIC_NOT_IDLE
				// Reschedule the timer event to reload the layout
				m_nTimer = SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
				if (m_nTimer == 0) {
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SET_TIMER_FAIL_5);
					Sleep(0);
					m_nTimer = SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL);
				}
			}
			else {
				// EvsResetWindows() and EvsCreateStatusWindows () deleted
				// as they are not necessary.  These just do underlying window
				// management data structure initialization within the application
				// that have nothing to do with the actual displayed windows.
				// See also function UieDisplayInit().
				// When the actual display window is created, then we do an
				// EvsConnect () to connect a display window (Windows XP API
				// window) to the NHPOS window management object.  When done
				// we will then use EvsDisconnect () to disconnect the Windows XP
				// display window with the underlying NHPOS window management
				// object as part of the Framework destructor.  See the
				// destructor for class CFrameworkWndText() which does an EvsDisconnect()
				// as part of object destruction.
//				EvsResetWindows();

				ClearDocument();
				CFrameworkWndTextSubWindow::InitializeSubWindowSystem ();
				BlInitializeOepTable();

				CArchive touchArchive(&fTouchLayout, CArchive::load);
				try {
					Serialize(touchArchive);
				}
				catch (CException *ex) {
					TCHAR szError[258];
					ex->GetErrorMessage(szError, 256);
					AfxMessageBox(szError, MB_OK);
					ex->Delete();
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_LAYOUT_FAILED);
					NHPOS_ASSERT_TEXT(0, "**ERROR Exception processing Layout file.");
					touchArchive.Close();
					fTouchLayout.Close ();
					return;
				}
				touchArchive.Close();     // close the archive

				CWindowControl *pListControl;
				POSITION currentPos = listControls.GetHeadPosition ();

				//we call this function before the while loop so that 
				//we have created all of our status windows, in the 
				//event of a dynamic PLU button, the terminal
				//may issue a call to the master, in which it 
				//would have to turn a descriptor one, however if this
				//function is not called before hand, no window will exist,
				//and a pif abort will be issued. JHHJ 11-17-06
				// See note above about removal of EvsResetWindows () and
				// EvsCreateStatusWindows ();
				//EvsCreateStatusWindows();

				while (currentPos) {
					pListControl = listControls.GetNext (currentPos);
					ASSERT(pListControl);

					if (pListControl->controlAttributes.m_nType != CWindowControl::WindowContainer) {
						pListControl->WindowCreate (this);
					}
					else /*if (pListControl->m_myId == 1)*/{
						pListControl->WindowCreate (this);

						//Change to work with Groups RPH
						if(!pListControl->controlAttributes.isVirtualWindow){
							pListControl->ShowWindow(SW_HIDE);
						}
					}
				}

				BlFwIfDisplayWrite(); //Call UieEchoBack to restore the mnemonic.

				RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

				//display default windows
				DefaultUserScreenLocal();
				OnShowHideGroupMsgRcvd ((WPARAM)0xffffffff, (LPARAM)2);   // must be same as BlFwIfGroupWindowHide (0xffffffff)

				BlResetTerminalLock(sReloadHandle);  //Reset Terminal Lock

				BlFinalizeAndClose();    // CliParaSaveFarData() update memory resident data snapshot and the ASSRTLOG file offset pointers.
			}

			fTouchLayout.Close();//close the file
		}
		return;
	}
	else if (nIDEvent == FRAME_TIMER_CMD_AUTO_SIGN_OUT){  // Timer event used by function SetAutoSignOutTime()
					
		KillTimer(m_ntimerSignout);

		//DO AUTO SIGN OUT
		CharData.uchASCII = 0xFF;   //Auto Sign Out key key
		CharData.uchScan  = 0x29;   // FSC_AUTO_SIGN_OUT

		pDeviceInput = &BlNotify.u.Data;

		pDeviceInput->ulDeviceId = BL_DEVICE_DATA_KEY;
		pDeviceInput->ulLength = 2;	
		pDeviceInput->pvAddr = &CharData;

		BlNotify.ulType = BL_INPUT_DATA;
		
		lreturn = BlSendKeybrdData(&BlNotify, NULL);
		if(lreturn == 0){
			m_ntimerSignout = SetTimer(FRAME_TIMER_CMD_AUTO_SIGN_OUT, 1000, NULL);
		}
		return;
	}
	else if (nIDEvent == FRAME_TIMER_CMD_FORCE_SHUTDOWN){  // Timer event used by function SetAutoSignOutTime()
					
		KillTimer(m_ntimerForceShutdown);

		//DO AUTO SIGN OUT
		CharData.uchASCII = 0xFF;   //Auto Sign Out key key
		CharData.uchScan  = 0x29;   // FSC_AUTO_SIGN_OUT

		pDeviceInput = &BlNotify.u.Data;

		pDeviceInput->ulDeviceId = BL_DEVICE_DATA_KEY;
		pDeviceInput->ulLength = 2;	
		pDeviceInput->pvAddr = &CharData;

		BlNotify.ulType = BL_INPUT_DATA;
		lreturn = BlSendKeybrdData(&BlNotify, NULL);

		pDeviceInput = &BlNotify.u.Data;
		pDeviceInput->ulDeviceId = BL_DEVICE_DATA_POWERDOWN;
		pDeviceInput->pvAddr = &CharData;

		BlNotify.ulType = BL_INPUT_DATA;
		
		lreturn = BlSendKeybrdData(&BlNotify, NULL);
		if(lreturn == 0){
			m_ntimerForceShutdown = SetTimer(FRAME_TIMER_CMD_FORCE_SHUTDOWN, 3000, NULL);
		}
		else {
//			ULONG	exitCode;

			Sleep (100);
			delete this;
//			GetExitCodeProcess(GetCurrentProcess(), &exitCode);
//			ExitProcess(exitCode);
		}
		return;
	}
	else if (nIDEvent == FRAME_TIMER_CMD_SECONDS_TIMER){  // Timer event used counting down various timers
		KillTimer(m_ntimerSecondsTimer);
		BlProcessTimerList(0);
		m_ntimerSecondsTimer = SetTimer(FRAME_TIMER_CMD_SECONDS_TIMER, 1000, NULL);    // Restart the utility timer
		return;
	}

	CWindowDocument::OnTimer(nIDEvent);
}

void CFrameworkWndDoc::DefaultUserScreenLocal() 
{
	CFrameWnd* pMainWnd = (CFrameWnd*)AfxGetMainWnd();
	CFrameworkWndDoc* pFrameworkWndDoc = (pMainWnd ? (CFrameworkWndDoc*) pMainWnd : NULL);

	ASSERT(pFrameworkWndDoc);

	if (pFrameworkWndDoc) {
	
		//cycle through all the documents windows
		POSITION currentPos = pFrameworkWndDoc->listControls.GetHeadPosition ();
		
		while (currentPos) {
			//get the current window in the list
			CWindowControl *pwc = pFrameworkWndDoc->listControls.GetNext (currentPos);
			
			ASSERT(pwc);
			
			//if the control in the list is a Window
			if (pwc && (pwc->controlAttributes.m_nType == CWindowControl::WindowContainer))
			{
				//if this window was set to be a user screen default
				if (pwc->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_FLAG)
				{
					//Bring window to front or create the window
					//We allow more than one window to be designated as a default window in
					//order to allow creation of sets of windows for a default layout.
					CWindowDocument::pDoc->PopupWindow (pwc, pwc->controlAttributes.m_myId);
				}			
			}
		}
		// This method DefaultUserScreenLocal() is called a number of times so the
		// following functions for Group Show/Hide and Left/Right Handed translation
		// should not be used at this point since it will introduce Show/Hide and Translate
		// behavior after each transaction.
		//   BlFwIfGroupWindowHide (0xffffffff)
		//   BlFwIfWindowTranslate (0);
	}
	else {
		TRACE0("WARNING:  CFrameworkWndDoc::DefaultUserScreenLocal () pFrameworkWndDoc is NULL.");
	}
}

// Handle the WU_EVS_DFLT_LOAD message to reload the default windows
LRESULT CFrameworkWndDoc::OnDefaultWinLoad(WPARAM wParam, LPARAM lParam)
{
	//call function to display windows marked as user defaults
	DefaultUserScreenLocal();
	return 0;
}

// Handle the WU_EVS_POPUP_WINDOW message to popup the specified window
LRESULT CFrameworkWndDoc::OnPopupWindowByName(WPARAM wParam, LPARAM lParam)
{
	TCHAR *ptcWindowName = (TCHAR *)lParam;

	if (wParam == 0) {
		//call function to display windows marked as user defaults
		PopupWindow(0, ptcWindowName);
	}
	else if (wParam == 1 || wParam == 2) {
		// call function to display an event triggered window.  this is a
		// window that SpecWin[SpecWinIndex_EventTrigger] indicates should
		// display if a particular event happens.  See WindowControl.h for
		// a list of the various identifiers SpecWinEventTrigger_WId_
		CWindowControl *pC;
		pC = SearchForItemByEventTrigger_WId (lParam);
		if (wParam == 1 && pC) {
			PopupWindow (0, pC);
		}
		return ((pC) ? 1 : 0);
	}
	else if (wParam == 3) {           // search for window by numeric prefix. see BlFwIfPopupWindowPrefix()
		PopupWindow(0, _ttoi(ptcWindowName));
	}
	else if (wParam == 4) {           // see BlFwIfPopupMessageWindow().
		if (! m_pMessageDialog) {
			m_pMessageDialog = new CMessageDialog;
			if (m_pMessageDialog) {
				m_pMessageDialog->CreateMe();
			}
		}

		if (m_pMessageDialog && lParam) {
			m_pMessageDialog->SetTextAndShow (ptcWindowName);
		}
	} else if (wParam == 5) {        // pop up the window group by group number. see BlFwIfPopupWindowGroup().
		int iGroupNo = _ttoi(ptcWindowName);
		DisplayWindowsByGroup (iGroupNo);
	}

	return 0;
}

void CFrameworkWndDoc::DisplayWindowsByGroup (int iGroupNo)
{
	if (iGroupNo < 1) return;

	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw && (pw->windowAttributes.GroupNum == iGroupNo)) {
			pw->PopupWindow (this);       // for CFrameworkWndDoc::DisplayWindowsByGroup(). see also BlFwIfPopupWindowGroup().
		}
	}
}

void CFrameworkWndDoc::DismissWindowsByGroup (int iGroupNo)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	while (pos) {
		pclListCurrent = pos;
		bi = listControls.GetNext (pos);
		CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
		if (pw && (pw->windowAttributes.GroupNum == iGroupNo)) {
			pw->PopdownWindow ();
		}
	}
}

// Handle the WU_EVS_POPUP_WINDOW message to popdown the specified window
LRESULT CFrameworkWndDoc::OnPopdownWindowByName(WPARAM wParam, LPARAM lParam){
	if (wParam == 0)
		PopdownWindow(0,(TCHAR *)lParam);
	else if (wParam == 1){
		PopdownWindow(0, _ttoi((TCHAR *)lParam));
	}
	else if (wParam == 4) {
		if (m_pMessageDialog && lParam) {
			m_pMessageDialog->UnshowAndRemove ();
		}
	}
	else if (wParam == 5) {
		// pop up the window group by group number
		int iGroupNo = _ttoi((TCHAR *)lParam);
		DismissWindowsByGroup (iGroupNo);
	}
	else if (wParam == 6){
		SearchForItemByTypeApplyFunc(1);
	}
	//if wParam is 0 then search by the window label string
	//if wParam is 1 then search by integer prefix
	
	return 0;
}

VOID  CFrameworkWndDoc::InternalProcessFrameworkMessage (void  *pObject)
{
	FRAMEWORK  *pFrameworkObject = (FRAMEWORK  *)pObject;

	//call function to display windows marked as user defaults
	CWindowControl * WindowControlList[4] = {0, 0, 0, 0};

	SearchForItemByNameInWindow (pFrameworkObject->windowname, pFrameworkObject->tcsName, WindowControlList);

	TRY
	{
	if (WindowControlList[0] != 0 && (WindowControlList[0]->controlAttributes.SpecWin[SpecWinIndex_ConnEngine] & (SpecWinEventConnEngine_Indic | SpecWinEventScript_Indic)) != 0) {
		// window commands follow here that is actions for the window itself
		if (pFrameworkObject->displaywindow == FRAMEWORKCONTROL_INDIC_ENABLE) {
			WindowControlList[0]->PopupWindow(0);
		}
		else if (pFrameworkObject->displaywindow == FRAMEWORKCONTROL_INDIC_DISABLE) {
			WindowControlList[0]->PopdownWindow();
		}

		// check to see if there are any color changes for the window defaults
		{
			BOOL  bColorChange = FALSE;

			if (pFrameworkObject->windefaultbtncolorbg <= 0xFFFFFF) {
				bColorChange = TRUE;
				WindowControlList[0]->controlAttributes.m_colorFace = pFrameworkObject->windefaultbtncolorbg;
			}
			if (pFrameworkObject->windefaultbtncolortxt <= 0xFFFFFF) {
				bColorChange = TRUE;
				WindowControlList[0]->controlAttributes.m_colorText = pFrameworkObject->windefaultbtncolortxt;
			}

			if (bColorChange) {
				CFrameworkWndItem * wi = CFrameworkWndItem::TranslateCWindowControl (WindowControlList[0]);
				if (wi) {
					POSITION pclListNext = wi->ButtonItemList.GetHeadPosition ();
					CWindowControl *bp;
					while (pclListNext && (bp = wi->ButtonItemList.GetNext (pclListNext))) {

						CFrameworkWndButton *button = CFrameworkWndButton::TranslateCWindowControl (bp);
						if (button && (button->controlAttributes.useDefault))
						{
							button->controlAttributes.m_colorFace = WindowControlList[0]->controlAttributes.m_colorFace;
							button->controlAttributes.m_colorText = WindowControlList[0]->controlAttributes.m_colorText;
						}
					}
				}
			}
		}

		// window control commands follow here that is actions for a particular control in the window.
		// If the window control is to be hidden, hide it first than make any changs.
		// If the window control is to be shown, make any changes and then show it.
		if (WindowControlList[1] != 0 && (WindowControlList[1]->controlAttributes.SpecWin[SpecWinIndex_ConnEngine] & (SpecWinEventConnEngine_Indic | SpecWinEventScript_Indic)) != 0) {
			if (pFrameworkObject->displaycontrol == FRAMEWORKCONTROL_INDIC_DISABLE) {
				WindowControlList[1]->ShowWindow (SW_HIDE);
			}

			if (pFrameworkObject->changetext) {
				WindowControlList[1]->WindowUpdateText (pFrameworkObject->tcsText, pFrameworkObject->controlbgcolor, pFrameworkObject->controltextcolor);
			}

			if (pFrameworkObject->changebgcolor) {
				WindowControlList[1]->WindowUpdateText (0, pFrameworkObject->controlbgcolor, pFrameworkObject->controltextcolor);
			}

			if (pFrameworkObject->changeiconfile) {
				WindowControlList[1]->ChangeRefreshIcon (pFrameworkObject->tcsIconfile);
			}

			if (pFrameworkObject->displaycontrol == FRAMEWORKCONTROL_INDIC_ENABLE) {
				WindowControlList[1]->ShowWindow (SW_SHOW);
			}
		}
	}
	}
	CATCH_ALL(e)
	{
		TRACE(traceAppMsg, 0, "Warning: Uncaught exception in InternalProcessFrameworkMessage (returning %ld).\n",
			0);
	}
	END_CATCH_ALL
}

LRESULT CFrameworkWndDoc::OnXmlMsgRcvd(WPARAM wParam, LPARAM lParam)
{
	TCHAR       *ptcString = (TCHAR *)wParam;

	if (wParam == 0 && lParam == 0) {
	}
	else if (wParam != 0) {
		AllObjects  *pMyObject = (AllObjects *)wParam;
		if (pMyObject->iMessageTag == BL_CONNENGINE_MSG_FRAMEWORK) {
			InternalProcessFrameworkMessage (&(pMyObject->u.FrameWork));
			return 1;
		}
	}

	return 0;
}

// Handle the WM_APP_CONNENGINE_MSG_RCVD message to process a message received through Connection Engine
LRESULT CFrameworkWndDoc::OnConnEngineMsgRcvd(WPARAM wParam, LPARAM lParam)
{

	if (wParam == 0 && lParam == 0) {
	}
	else if (wParam != 0) {
		int         iMessageTag;
		int         myObjectIndex = ((m_myObjectArrayIndex++) % 5);
		AllObjects  *myObject;

		m_myObjectArrayIndex %= 5;
		myObject = m_myObjectArray + myObjectIndex;
		myObject->ptcString = (TCHAR *)wParam;

		// Process the message looking for one or more directives in the message text.
		// Directives are such things as:
		//    <FRAMEWORK>  -> BL_CONNENGINE_MSG_FRAMEWORK
		//    <PRINTFILE>  -> BL_CONNENGINE_MSG_PRINTFILE
		while (*(myObject->ptcString)) {
			memset (&(myObject->u), 0, sizeof(myObject->u));
			iMessageTag = BlProcessConnEngineMessageAndTag (&(myObject->ptcString), myObject);
			if (iMessageTag == BL_CONNENGINE_MSG_FRAMEWORK) {
				InternalProcessFrameworkMessage (&(myObject->u.FrameWork));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_PRINTFILE) {
				BlSubmitFileToFilePrint (myObject->u.PrintFile.tcsFileName);
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_EMPLOYEECHANGE) {
				BlProcessEmployeeChange (&(myObject->u.EmployeeChange));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_GUESTCHECK_RTRV) {
				BlProcessGuestCheck_Rtrv (&(myObject->u.GuestCheck_Rtrv));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_CASHIERACTION) {
				BlProcessCashierAction (&(myObject->u.CashierAction));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_EJENTRYACTION) {
				BlProcessEjEntryAction (&(myObject->u.EjEntryAction));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_TRANSACTION) {
				BlProcessTransaction (&(myObject->u.Transaction));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_CONNENGINE) {
				BlProcessConnEngine (myObject);
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_MACROACTION) {
				BlProcessMacroAction(&(myObject->u.MacroAction));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_DATAQUERY) {
				BlProcessDataQuery(&(myObject->u.DataQuery));
			}
			else if (iMessageTag == BL_CONNENGINE_MSG_STATECHANGE) {
				BlProcessStateChange(&(myObject->u.StateChange));
			}
			else {
				break;
			}
		}
	}

	return 0;
}

// Handle the WM_APP_BIOMETRIC_MSG_RCVD message to process a message received from a biometric device
LRESULT CFrameworkWndDoc::OnBiometricMsgRcvd(WPARAM wParam, LPARAM lParam){
#if defined(POSSIBLE_DEAD_CODE)
	BL_NOTIFYDATA	BlNotify;
	MSR_BUFFER		MSRBuff;

	ULONG employeeID = 0;
	if (wParam == 0) {  // same as WN_COMPLETED so we have a finger scan ready to verify
		_DEVICEINPUT*	pDeviceInput = &(BlNotify.u.Data);

		DFPRVerifyFingerEvent(wParam,lParam,&employeeID);

		memset(&MSRBuff, 0x00, sizeof(MSR_BUFFER));
		MSRBuff.uchPaymentType = BIOMETRICS_ENTRY;
		MSRBuff.uchLength2 = swprintf_s(MSRBuff.auchTrack2,L"00000%08u00000",employeeID);

		memset(&BlNotify, 0x00, sizeof(BlNotify));

		pDeviceInput->ulDeviceId = BL_DEVICE_DATA_MSR;
		pDeviceInput->ulLength = sizeof(MSR_BUFFER);
		pDeviceInput->pvAddr = &MSRBuff;

		BlNotify.ulType = BL_INPUT_DATA;
		BlNotifyData(&BlNotify, NULL);
	}
#endif
	return 0;
}

// Handle the WM_APP_SHUTDOWN_MSG message to process a message indicating to close GenPOS
// We need to provide time for GenPOS to finish shutting down so we are using an indirection
// through an event to a shutdown thread FrameworkShutdownThread() in order to provide the
// additional time for things like final control string processing, etc.
// Without this we can see asserts from the Evs subsystem as it tries to update windows
// which no longer exist.
LRESULT CFrameworkWndDoc::OnShutdownMsgRcvd(WPARAM wParam, LPARAM lParam)
{
	CFrameworkApp *myApp = (CFrameworkApp *)AfxGetApp();

	SetEvent (myApp->m_hEventShutdownNHPOS);
//	PostMessage (WM_CLOSE, 0, 0);
	return 0;
}

// Handle the WM_POWERBROADCAST message to process a message concerning power management
// such as going to Sleep or Waking Up.
LRESULT CFrameworkWndDoc::OnPowerMsgRcvd(WPARAM wParam, LPARAM lParam)
{

	switch (wParam) {
		case PBT_APMQUERYSUSPEND:
			TRACE0("PBT_APMQUERYSUSPEND  received\n");
			break;
		case PBT_APMPOWERSTATUSCHANGE:
			TRACE0("PBT_APMPOWERSTATUSCHANGE  received\n");
			break;
		case PBT_APMRESUMEAUTOMATIC:
			TRACE0("PBT_APMRESUMEAUTOMATIC  received\n");
			break;
		case PBT_APMRESUMECRITICAL:
			TRACE0("PBT_APMRESUMECRITICAL  received\n");
			break;
		case PBT_APMRESUMESUSPEND:
			TRACE0("PBT_APMRESUMESUSPEND  received\n");
			break;
		case PBT_APMSUSPEND:
			TRACE0("PBT_APMSUSPEND  received\n");
			break;
	}

	return 0;
}

void CFrameworkWndDoc::ShowHideGroupMsgRcvd (WPARAM wParam, LPARAM lParam) 
{
	CFrameWnd* pMainWnd = (CFrameWnd*)AfxGetMainWnd();
	CFrameworkWndDoc* pFrameworkWndDoc = (pMainWnd ? (CFrameworkWndDoc*) pMainWnd : NULL);

	ASSERT(pFrameworkWndDoc);

	if (pFrameworkWndDoc) {
		pFrameworkWndDoc->OnShowHideGroupMsgRcvd (wParam, lParam);
	}
}


// Handle the WM_APP_SHOW_HIDE_GROUP message to process Show and Hide of Windows that
// do not have the same Group Permissions as the wParam parameter. The Group Permissions
// are used as a way to prohibit access to some functionality by an Employee who is Signed-in.
// This function looks through the window list and shows or hides any windows based on the
// group permissions.  The purpose of this function is to declutter the screen when appropriate.
// This function is invoked from within BusinessLogic with one of the following functions.
//  	BlFwIfGroupWindowHide (0xffffffff);
//  	BlFwIfGroupWindowShow (CasData.ulGroupAssociations);
//      BlFwIfWindowTranslate(bTranslate);
LRESULT CFrameworkWndDoc::OnShowHideGroupMsgRcvd(WPARAM wParam, LPARAM lParam)
{
	CWindowControl *bi = 0;
	POSITION pos = listControls.GetHeadPosition ();

	if (lParam == 1 || lParam == 2) {
		int iShowWin = (lParam == 1) ? SW_SHOW : SW_HIDE;

		USHORT  usKeyLockPos = BlGetKeyboardLockPosition();

		while (pos) {
			bi = listControls.GetNext (pos);
			CFrameworkWndItem *pw = CFrameworkWndItem::TranslateCWindowControl (bi);
			if (pw && pw->controlAttributes.ulMaskGroupPermission != 0) {
				if ((pw->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_SHOWHIDE_GROUP) != 0) {
					// Show the window if it is part of the group (group bit is turned on)
					// and hide the window if it is not part of the group (group bit is turned off).
					if ((pw->controlAttributes.ulMaskGroupPermission & wParam) != 0) {
						pw->ShowWindow (iShowWin);
						if (iShowWin == SW_SHOW) {
							::SetWindowPos (pw->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						}
					}
				}
			}
		}
	} else if (lParam == 3) {
		m_wParamWindowTranslation = (wParam != 0);

		while (pos) {
			bi = listControls.GetNext (pos);
			if (bi) {
				int xPos = bi->controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth;
				int yPos = bi->controlAttributes.m_nRow * CFrameworkWndButton::stdHeight;
				if (m_wParamWindowTranslation && (bi->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_WINTRANSLATE) == 0) {
					// translate from right side of Y-axis to left side of Y-axis
					// then swap left and right top corners then shift the window back
					// to the right side of the Y-axis.
					xPos *= -1;
					xPos -= bi->controlAttributes.m_nWidth;
					xPos += GetSystemMetrics (SM_CXSCREEN);
				} else if ((bi->controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_DISPLAYTRANS) != 0) {
					xPos += GetSystemMetrics (SM_CXSCREEN);
				}
				::SetWindowPos (bi->m_hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
			}
		}
	}

	return 0;
}
