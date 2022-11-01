/////////////////////////////////////////////////////////////////////////////
// progrdoc.cpp : implementation of the CProgrptDoc class
//
// Copyright (C) 1996-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Oct-12-98           O.Nakada     HPUS V3.3
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "progrdoc.h"
#include "reportvw.h"
#include "report.h"
#include "dlgsize.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define	DOC_WIDE_CHAR		0x12
#define	DOC_WIDE_SHOW		'~'

static	wchar_t szHeader[]  = L"Header";
static	wchar_t szVersion[] = L"Version";
static	wchar_t szFmtVer[]  = L"%X";
static	wchar_t szParam[]   = L"Parameters";
static	wchar_t szBlock[]   = L"Block";
static	wchar_t szDate[]    = L"Date";
static	wchar_t szTime[]    = L"Time";
static	wchar_t szMnemo[]   = L" Mnemonics";
static	wchar_t szNumber[]  = L"Number";
static	wchar_t szAddress[] = L"Address%d";
static	wchar_t szTran[]    = L"Transaction";
static	wchar_t szReport[]  = L"Report";
static	wchar_t szSpecial[] = L"Special";
static	wchar_t szAdj[]     = L"Adjective";
static	wchar_t szMDept[]   = L"Major Department";
static	wchar_t sz16CM[]    = L"16 Characters";
static	wchar_t sz40CM[]    = L"40 Characters";
static	wchar_t szEod[]     = L"End of Day";
static	wchar_t szPtd[]     = L"Period to Day";
static	wchar_t szEP[]      = L"%d,%d";
static	wchar_t szState[]   = L"State";

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc

IMPLEMENT_DYNCREATE(CProgrptDoc, CDocument)

BEGIN_MESSAGE_MAP(CProgrptDoc, CDocument)
	//{{AFX_MSG_MAP(CProgrptDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc construction/destruction

CProgrptDoc::CProgrptDoc()
{
	m_pListView  = NULL;
	m_bDate      = FALSE;
	m_bTime      = FALSE;
	m_nUnique    = 1;
	m_lAllocSize = -1L;
	m_bMessage   = FALSE;
	m_pReport	 = NULL;
}

CProgrptDoc::~CProgrptDoc()
{
	RemoveAllReports();
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc serialization

void CProgrptDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc diagnostics

#ifdef _DEBUG
void CProgrptDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProgrptDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc overridables

BOOL CProgrptDoc::OnNewDocument()
{
	if (! CDocument::OnNewDocument())
		return FALSE;

	if (! ReadPepParameters())
		if (AfxMessageBox(IDS_ERRMSG_PEP, MB_YESNO) != IDYES)
			return FALSE;

	// create EOD and PTD data
	NewEodPtd(DOC_INDEX_EOD, ptheInfo->m_nEod);
	NewEodPtd(DOC_INDEX_PTD, ptheInfo->m_nPtd);

	UpdateStatusSize();					// update status bar

	return TRUE;
}

BOOL CProgrptDoc::OnOpenDocument(const wchar_t * pszPathName)
{
	if (! CDocument::OnOpenDocument(pszPathName))
		return FALSE;

	wchar_t szData[64];
	int nVersion = 0;
	if (::GetPrivateProfileString(szHeader, szVersion, L"", szData, sizeof(szData), pszPathName) > 0)
		swscanf(szData, szFmtVer, &nVersion);
		
#if	1										// HPUS V3.3 - Migration
	switch (nVersion)
	{
// DEL Saratoga
//	case 0x310:								// HPUS V3.1
//		if (AfxMessageBox(IDS_ERRMSG_OPEN310TO330, MB_YESNO) != IDYES)
//			return FALSE;
//		SetModifiedFlag();
//		break;

// ADD Saratoga
	case 0x330:															// HPUS V3.4 
	case 0x340:
	    if (AfxMessageBox(IDS_ERRMSG_OPEN100TO340, MB_YESNO) != IDYES)
	    	return FALSE;
	    SetModifiedFlag();
	    break;
// ADD Saratoga end
	case PROGRPT_VERSION:					// current version
		break;
	default:
		AfxMessageBox(IDS_ERRMSG_VERSION);
		return FALSE;
	}
#endif										// HPUS V3.3 - Migration

	if (! ReadPepParameters())
		if (AfxMessageBox(IDS_ERRMSG_PEP, MB_YESNO) != IDYES)
			return FALSE;

	// read 16 and 40 characters mnemonics
	ReadMnemonics(MNEMO_INDEX_16CM, MNEMO_MAX_16CM, sz16CM, pszPathName, FALSE);
	ReadMnemonics(MNEMO_INDEX_40CM, MNEMO_MAX_40CM, sz40CM, pszPathName, FALSE);

	// initialize migration information
	int	nPos = 1;
    memset(m_anFile, '\0', sizeof(m_anFile));

	// read all reports
	for (int nCount = 1; nCount <= REPORT_MAX_SHEET; nCount++)
	{
		CReport*	pReport = new CReport(this);
		int			nResult = pReport->OpenDocument(nCount, pszPathName);
		if (nResult == 0)				// error or end of file
		{
			delete pReport;
			break;
		}
		else if (nResult == -1)			// old version file
		{
			delete pReport;
			continue;
		}
		AddReport(pReport, FALSE);
		m_anFile[nCount - 1] = nPos++;
	}

	// read EOD and PTD data
	ReadEodPtd(DOC_INDEX_EOD, szEod, pszPathName);
	ReadEodPtd(DOC_INDEX_PTD, szPtd, pszPathName);

	UpdateStatusSize();					// update status bar

	return TRUE;
}

BOOL CProgrptDoc::OnSaveDocument(const wchar_t * pszPathName)
{
	if (! CDocument::OnSaveDocument(pszPathName))
		return FALSE;

	wchar_t szData[64];
	
	// write document version
	wsprintf(szData, szFmtVer, PROGRPT_VERSION);
	WritePrivateProfileString(szHeader, szVersion, szData, pszPathName);

	// write 16 and 40 characters mnemonics	
	WriteMnemonics(MNEMO_INDEX_16CM, MNEMO_MAX_16CM, sz16CM, pszPathName);
	WriteMnemonics(MNEMO_INDEX_40CM, MNEMO_MAX_40CM, sz40CM, pszPathName);

	// write all report
	int nIndex = 1;
	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);

		if (! pReport->SaveDocument(nIndex++, pszPathName))
			break;
	}

	// write EOD and PTD data
	WriteEodPtd(DOC_INDEX_EOD, szEod, pszPathName);
	WriteEodPtd(DOC_INDEX_PTD, szPtd, pszPathName);

	// set modified flag of PEP
	CWnd* pWnd;
	if ((pWnd = CWnd::FindWindow(L"PEP:MAIN", NULL)) != NULL)
		pWnd->SendMessage(WM_USER + 101, 0x200);	// caution !!

	return TRUE;
}

BOOL CProgrptDoc::CanCloseFrame(CFrameWnd* pFrameArg)
{
	CView* pView = pFrameArg->GetActiveView();

	if (FindActiveReport((CReportView*)pView) != NULL)
		return TRUE;

	BOOL bClosing = SaveModified();

	if (bClosing)
	{
		// destroy all report frames
		POSITION pos = GetHeadPositionReport();
		while (pos != NULL)
		{
			CScrollView* pView = GetNextReport(pos)->GetView();
			if (pView != NULL && pView->GetSafeHwnd() != NULL)
				pView->GetParentFrame()->DestroyWindow();
		}
	}
	
	return bClosing;
}

void CProgrptDoc::DeleteContents()
{
	DeleteEodPtd(DOC_INDEX_EOD);
	DeleteEodPtd(DOC_INDEX_PTD);

	RemoveAllReports();
}

void CProgrptDoc::SetPathName(const wchar_t* pszPathName, BOOL bAddtoMRU)
{
	CString strTitle = GetTitle();

	CDocument::SetPathName(pszPathName, bAddtoMRU);

	// modified title of report window
	if (! strTitle.IsEmpty() && strTitle != GetTitle())
		UpdateAllViews(NULL);			// update title of report window
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc implementations

BOOL CProgrptDoc::ReadPepParameters()
{
	// get path of .EXE
	wchar_t szPathName[_MAX_PATH+1];
	GetModuleFileName(AfxGetInstanceHandle(), szPathName, _MAX_PATH);
	int nLen = wcslen(szPathName);
	ASSERT(nLen > 12 && szPathName[nLen - 12] == '\\');	// must end in \PROGRPT.EXE
	nLen -= 12;
	szPathName[nLen] = L'\0';        					// no suffix
	static wchar_t szPep[] = L"\\PARAMPEP.INI";
	lstrcat(szPathName, szPep);

	int nData = (int)::GetPrivateProfileInt(szParam, szBlock, -1, szPathName);
	if (nData != -1)
		m_lAllocSize = (long)nData * 1024L;				// from block size to bytes size
	else
		return FALSE;

	nData = (int)::GetPrivateProfileInt(szParam, szDate, -1, szPathName);
	if (nData != -1)
		m_bDate = (nData != 0) ? (TRUE) : (FALSE);
	else
		return FALSE;

	nData = (int)::GetPrivateProfileInt(szParam, szTime, -1, szPathName);
	if (nData != -1)
		m_bTime = (nData != 0) ? (TRUE) : (FALSE);
	else
		return FALSE;

	ReadMnemonics(MNEMO_INDEX_TRAN,    -1, szTran,    szPathName);
	ReadMnemonics(MNEMO_INDEX_REPORT,  -1, szReport,  szPathName);
	ReadMnemonics(MNEMO_INDEX_SPECIAL, -1, szSpecial, szPathName);
	ReadMnemonics(MNEMO_INDEX_ADJ,     -1, szAdj,     szPathName);
	ReadMnemonics(MNEMO_INDEX_MDEPT,   -1, szMDept,   szPathName);

	return TRUE;
}

void CProgrptDoc::ReadMnemonics(int nIndex, int nNumber,
	const wchar_t * pszSection, const wchar_t *pszFile, BOOL bContinue /*=TRUE*/)
{
	wchar_t szSection[64], szEntry[32], szData[64];

	// ===== caution !! caution !! caution =====
	if (nIndex == MNEMO_INDEX_ADJ || nIndex == MNEMO_INDEX_MDEPT)
		m_strMnemo[nIndex].AddTail(L"Auto");
	// ===== caution !! caution !! caution =====
	
	wcscpy(szSection, pszSection);
	wcscat(szSection, szMnemo);

	if (nNumber == -1)
		nNumber = (int)::GetPrivateProfileInt(szSection, szNumber, 0, pszFile);
	
	for (int nCount = 1; nCount <= nNumber; nCount++)
	{
		wsprintf(szEntry, szAddress, nCount);
		int nBytes = ::GetPrivateProfileString(szSection, szEntry, L"", szData, sizeof(szData), pszFile);
		// convert double wide characters
		for (int nOffset = 0; szData[nOffset] != L'\0'; nOffset++)
			if (szData[nOffset] == DOC_WIDE_CHAR)
				szData[nOffset] = DOC_WIDE_SHOW;
		m_strMnemo[nIndex].AddTail(szData);
	}
}

void CProgrptDoc::WriteMnemonics(int nIndex, int nNumber,
	const wchar_t* pszSection, const wchar_t *pszFile)
{
	wchar_t szSection[64], szEntry[32];

	wcscpy(szSection, pszSection);
	wcscat(szSection, szMnemo);

	POSITION pos = m_strMnemo[nIndex].GetHeadPosition();
	for (int nCount = 1; nCount <= nNumber; nCount++)
	{
		wsprintf(szEntry, szAddress, nCount);
		wchar_t szMnemo[64];
		if (pos != NULL)
		{
			wcscpy (szMnemo, m_strMnemo[nIndex].GetNext(pos));
			// convert double wide characters
			for (int nOffset = 0; szMnemo[nOffset] != L'\0'; nOffset++)
				if (szMnemo[nOffset] == DOC_WIDE_SHOW)
					szMnemo[nOffset] = DOC_WIDE_CHAR;
		}
		else
			szMnemo[0] = L'\0';
		::WritePrivateProfileString(szSection, szEntry, szMnemo, pszFile);
	}
}

void CProgrptDoc::NewEodPtd(int nIndex, int* pnInfo)
{
	m_nEPState[nIndex] = 0;

	for (int nCount = 0; nCount < INFO_NUM_FILE; nCount++)
	{
		if (*pnInfo == _FREE)
			break;

		EOD_PTD* pData = new EOD_PTD;
		pData->m_nFile   = *pnInfo++;
		pData->m_nUnique = DOC_EP_NOT;
		m_EPList[nIndex].AddTail(pData);
	}
}

void CProgrptDoc::DeleteEodPtd(int nIndex)
{
	POSITION pos = m_EPList[nIndex].GetHeadPosition();
	while (pos != NULL)
	{
		EOD_PTD* pData = (EOD_PTD*)m_EPList[nIndex].GetNext(pos);
		delete pData;
	}
	m_EPList[nIndex].RemoveAll();
}

void CProgrptDoc::ReadEodPtd(int nIndex, const wchar_t * pszSection,
	const wchar_t  *pszFile)
{
	wchar_t szEntry[32], szData[32];
	BOOL fOperator = FALSE;

	m_nEPState[nIndex] = (int)GetPrivateProfileInt(pszSection, szState, 0, pszFile);

	for (int nCount = 1; nCount <= INFO_NUM_FILE; nCount++)
	{
		wsprintf(szEntry, szAddress, nCount);
		if (::GetPrivateProfileString(pszSection, szEntry, L"",
									  szData, sizeof(szData), pszFile) <= 0)
			break;

		int nFile, nType;
		swscanf(szData, szEP, &nFile, &nType);
#if	1										// HPUS V3.3 - Migration
		if (nFile == MIGRATE_ID_SERVER)
			continue;
		if (nFile == MIGRATE_ID_OPERATOR)
			fOperator = TRUE;
#endif										// HPUS V3.3 - Migration
		if (nType > 0)
		{
#if	1										// HPUS V3.3 - Migration
			nType = m_anFile[nType - 1];
#endif										// HPUS V3.3 - Migration
			CReport* pReport = GetReport(nType);
			ASSERT(pReport != NULL);
			nType = pReport->GetUnique();
		}

		EOD_PTD* pData = new EOD_PTD;
		pData->m_nFile   = nFile;
		pData->m_nUnique = nType;
		m_EPList[nIndex].AddTail(pData);
	}

#if	1										// HPUS V3.3 - Migration
	// If the PTD does not contain the operator file,
	// the progrpt.exe adds the operator file to the PTD.
	if ((nIndex == DOC_INDEX_PTD) && (! fOperator))
	{
		EOD_PTD* pData = new EOD_PTD;
		pData->m_nFile   = MIGRATE_ID_OPERATOR;
		pData->m_nUnique = -1;				// Do not execute
		m_EPList[nIndex].AddTail(pData);
	}
#endif										// HPUS V3.3 - Migration
}

void CProgrptDoc::WriteEodPtd(int nIndex, const wchar_t * pszSection,
	const wchar_t  *pszFile)
{
	wchar_t szEntry[32], szData[32];

	_itow(m_nEPState[nIndex], szData, 10);
	WritePrivateProfileString(pszSection, szState, szData, pszFile);

	int nCount = 1;

	POSITION pos = m_EPList[nIndex].GetHeadPosition();
	while (pos != NULL)
	{
		EOD_PTD* pData = (EOD_PTD*)m_EPList[nIndex].GetNext(pos);
		ASSERT(pData != NULL);

		int nReport = pData->m_nUnique;
		if (nReport > 0)
			nReport = GetReportNo(nReport);
		wsprintf(szData, szEP, pData->m_nFile, nReport);

		wsprintf(szEntry, szAddress, nCount++);
		::WritePrivateProfileString(pszSection, szEntry, szData, pszFile);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptDoc operations

void CProgrptDoc::UpdateStatusMsg(UINT nID)
{
	AfxGetApp()->m_pMainWnd->PostMessage(ID_STATUS_MSG, nID);
}

void CProgrptDoc::UpdateStatusSize()
{
	AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, ID_INDICATOR_SIZE, 0L);
}

int CProgrptDoc::CheckSize(long lAdd, BOOL bWarning /*=TRUE*/)
{
	if (m_lAllocSize > 0L && m_lAllocSize <= GetUsedSize() + lAdd)
	{
		if (m_bMessage)					// already displayed
			return PROGRPT_WARNING;
		else if (bWarning)
		{
			CSizeDlg dlg;
			int nState = dlg.DoModal();
			m_bMessage = dlg.m_bDisp;
			if (nState == IDYES)
				return PROGRPT_WARNING;
		}
		return PROGRPT_ERROR;
	}
	return PROGRPT_OK;
}

long CProgrptDoc::GetUsedSize()
{
	long lUsed = DOC_SIZE_FIXED;		// fixed parameters

	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		lUsed += pReport->GetUsedSize();
	}
	
	return lUsed;
}

void CProgrptDoc::SetNewReport(CReport* pReport)
{
	if (m_pReport == NULL)
		m_pReport = pReport;
	else
		ASSERT(FALSE);
}

CReport* CProgrptDoc::GetNewReport()
{
	CReport* pReport = m_pReport;

	if (pReport != NULL)
		m_pReport = NULL;
	else
		ASSERT(FALSE);

	return pReport;
}

BOOL CProgrptDoc::IsNewReport(int nFile, int nDevice)
{
	if (nDevice != INFO_DEVICE_LCD)		// not LCD
		return TRUE;

	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		
		// same file and device ?
		if (nFile == pReport->GetFileIndex() &&
			nDevice == pReport->GetDevice())
			return FALSE;
	}

	return TRUE;
}

void CProgrptDoc::AddReport(CReport* pReport, BOOL bModified /*=TRUE*/)
{
	ASSERT(pReport != NULL);
	
	if (pReport->GetDevice() != INFO_DEVICE_LCD)
	{
		int nCount = GetCountReport(FALSE);
		if (nCount > 0)
		{
			POSITION pos = m_ReportList.FindIndex(nCount - 1);
			m_ReportList.InsertAfter(pos, pReport);
		}
		else
			m_ReportList.AddHead(pReport);
	}
	else
		m_ReportList.AddTail(pReport);

	UpdateStatusSize();					// update status bar

	if (bModified)
		SetModifiedFlag();
}

BOOL CProgrptDoc::RemoveReport(CReport* pReport, BOOL bModified /*=TRUE*/)
{
	ASSERT(pReport != NULL);
	
	POSITION pos = m_ReportList.Find(pReport);
	if (pos == NULL)
		return FALSE;					// object not found

	m_ReportList.RemoveAt(pos);
	delete pReport;

	UpdateStatusSize();					// update status bar

	if (bModified)
		SetModifiedFlag();

	return TRUE;
}

void CProgrptDoc::RemoveAllReports()
{
	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		delete pReport;
	}
	m_ReportList.RemoveAll();

	UpdateStatusSize();					// update status bar
}

int CProgrptDoc::GetCountReport(BOOL bLcd)
{
	if (bLcd)							// include LCD
		return m_ReportList.GetCount();

	int nCount = 0;

	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		if (pReport->GetDevice() != INFO_DEVICE_LCD)
			nCount++;
		else
			return nCount;
	}

	return nCount;
}

CReport* CProgrptDoc::GetReport(int nNumber)
{
	int	nCount = 0;
	
	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		if (++nCount == nNumber)
			return pReport;
	}
	
	return NULL;
}

int CProgrptDoc::GetReportNo(int nUnique)
{
	int nCount = 1;

	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		if (pReport->GetUnique() == nUnique)
			return nCount;
		else
			nCount++;
	}

	return -1;	
}

CReport* CProgrptDoc::FindActiveReport(CReportView* pView)
{
	POSITION pos = GetHeadPositionReport();
	while (pos != NULL)
	{
		CReport* pReport = GetNextReport(pos);
		if (pReport->GetView() == pView)
			return pReport;
	}
	
	return NULL;
}
