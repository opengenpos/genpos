/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PCSampleDoc.cpp
//
//  PURPOSE:    Provides PCSample Main Document class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"


#include "PCSample.h"
#include "PCSampleDoc.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSqlitedb::CSqlitedb(CONST TCHAR *tcsDbFileName /*  = 0 */) : m_hDb(0), m_stRetCode(0), m_ulId(0), m_ulRecordCnt(0), m_callback(0)
{
	if (tcsDbFileName) {
		char  aszFileName[512] = {0};
		for (int i = 0; i < 510 && (aszFileName[i] = tcsDbFileName[i]); i++);
		m_stRetCode = sqlite3_open(aszFileName, &m_hDb);
	}
}

CSqlitedb::~CSqlitedb ()
{
	if (m_hDb) {
		sqlite3_close(m_hDb);
	}
	m_hDb = 0;
	m_ulId = 0;
	m_callback = 0;
}

int CSqlitedb::OpenFile(CONST TCHAR *tcsDbFileName)
{
	if (m_hDb) {
		sqlite3_close(m_hDb);
		m_hDb = 0;
		m_callback = 0;
	}

	if (tcsDbFileName) {
		char  aszFileName[512] = {0};
		for (int i = 0; i < 510 && (aszFileName[i] = tcsDbFileName[i]); i++);
		m_stRetCode = sqlite3_open(aszFileName, &m_hDb);
	}

	return m_stRetCode;
}

int CSqlitedb::ExecSqlStmt(CONST char *aszSqlStmt, CSqlitedbCallback *pCallBack /* = 0 */, void *pArg /* = 0 */)
{
	if (m_hDb) {
		if (!pCallBack) pCallBack = m_callback;
		m_stRetCode = sqlite3_exec (m_hDb, aszSqlStmt, pCallBack, pArg, 0);
	}

	return m_stRetCode;
}


CSqlitedbTime::CSqlitedbTime(SYSTEMTIME *pSysTime /* = 0 */)
{
	if (pSysTime) SystemTimeToSqliteTime (pSysTime);
}


int CSqlitedbTime::SystemTimeToSqliteTime (const SYSTEMTIME *pSysTime)
{
	// YYYY-MM-DD HH:MM:SS.SSS  SQLite time string format #4

	sprintf (m_currKey, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d",
		pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay,
		pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds);

	return 0;
}

int CSqlitedbTime::SqliteTimeToSystemTime (SYSTEMTIME *pSysTime)
{
	SYSTEMTIME x = {0};
	WORD  wVal = 0;
	int   nDx = 0;

	for (nDx = wVal = 0; nDx < 4; nDx++) wVal = wVal * 10 + ((m_currKey +  0)[nDx] & 0x0f);  // YYYY
	x.wYear = wVal;
	for (nDx = wVal = 0; nDx < 2; nDx++) wVal = wVal * 10 + ((m_currKey +  5)[nDx] & 0x0f);  // MM
	x.wMonth = wVal;
	for (nDx = wVal = 0; nDx < 2; nDx++) wVal = wVal * 10 + ((m_currKey +  8)[nDx] & 0x0f);  // DD
	x.wDay = wVal;
	for (nDx = wVal = 0; nDx < 2; nDx++) wVal = wVal * 10 + ((m_currKey + 11)[nDx] & 0x0f);  // HH
	x.wHour = wVal;
	for (nDx = wVal = 0; nDx < 2; nDx++) wVal = wVal * 10 + ((m_currKey + 14)[nDx] & 0x0f);  // MM
	x.wMinute = wVal;
	for (nDx = wVal = 0; nDx < 2; nDx++) wVal = wVal * 10 + ((m_currKey + 17)[nDx] & 0x0f);  // SS
	x.wSecond = wVal;
	for (nDx = wVal = 0; nDx < 3; nDx++) wVal = wVal * 10 + ((m_currKey + 20)[nDx] & 0x0f);  // .SSS
	x.wMilliseconds = wVal;

	*pSysTime = x;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//          D Y N A M I C    O B J E C T    C R E A T I O N
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPCSampleDoc, CDocument)

extern USHORT readOffset = 0;
/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPCSampleDoc, CDocument)
	//{{AFX_MSG_MAP(CPCSampleDoc)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//          S T A T I C    V A R I A B L E s
//
/////////////////////////////////////////////////////////////////////////////

LPCWSTR CPCSampleDoc::m_lpszLogFileName = _T("PCSample.txt");

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::CPCSampleDoc()
//
//  PURPOSE :   Constructor of PCSample Document class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleDoc::CPCSampleDoc() :
    m_listReportStr( 400 ),
    m_dwMaxColumnInReportStr(0),
	lReportTotalCount(0),
	llReportTotalAmount(0),
	nSheetRow(0),
	nSheetCol(0),
	psqlDbFile(0),
	pSavedTotalFile(0),
	ulReportOptionsMap(0)
{
	memset(&m_pageSetup, 0, sizeof(m_pageSetup));
	m_pageSetup.lStructSize = sizeof(m_pageSetup);
	m_usDecPlace = 2;              // default is US Currency number of places
	m_usDecSymbol = '.';           // default is US Currency decimal symbol

#ifdef _DEBUG
    // --- create text file to log messages ---

    UINT    unFlags;
    unFlags = CFile::modeReadWrite | CFile::modeCreate | CFile::typeText;

    m_file.Open( m_lpszLogFileName,  unFlags );
#endif
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::~CPCSampleDoc()
//
//  PURPOSE :   Destructor of PCSample Document class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleDoc::~CPCSampleDoc()
{
#ifdef _DEBUG
    // --- close log file before this object is disacarded ---

    m_file.Close();
#endif
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::Serialize()
//
//  PURPOSE :   Write the formatted string to log file.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::Serialize(
     CArchive& ar               // archive object to write string
     )
{

    // --- determine this function call is write to file ---

    if ( ar.IsStoring() )
    {

#ifdef _DEBUG
        int   dw = GetNoOfReportStrings();

        if ( dw != 0 )
        {
            CString str;
            GetReportString( str, dw - 1 );

            ar.WriteString( str );
            ar.WriteString( _T("\n") );
        }

#endif

    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::AssertValid()
//
//  PURPOSE :   Perform Validity check of this object.
//
//  RETURN :    No return value.
//
//===========================================================================

#ifdef _DEBUG
void CPCSampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::Dump()
//
//  PURPOSE :   Dump the contents of this object.,
//
//  RETURN :    No return value.
//
//===========================================================================

#ifdef _DEBUG
void CPCSampleDoc::Dump( CDumpContext& dc ) const
{
    // --- Dump the contect ob this object ---
	CDocument::Dump(dc);
}
#endif

void CPCSampleDoc::OnPageSetup()
{
	CPageSetupDialog  pageDlg;

	// set our default margins.
	pageDlg.m_psd.rtMargin.left = m_pageSetup.rtMargin.left;
	pageDlg.m_psd.rtMargin.top = m_pageSetup.rtMargin.top;
	pageDlg.m_psd.rtMargin.right = m_pageSetup.rtMargin.right;
	pageDlg.m_psd.rtMargin.bottom = m_pageSetup.rtMargin.bottom;

	if (pageDlg.DoModal() == IDOK) {
		m_pageSetup = pageDlg.m_psd;
	}
}

CString CPCSampleDoc::MakeCurrencyString(D13DIGITS d13Value)
{
	CString  x;

	if (m_usDecPlace == 0) {
		x.Format(L"%lld", d13Value);
	}
	else {
		D13DIGITS d13Amt = 0;
		LONG      lDec = 0;
		wchar_t   sym = L'.';
		wchar_t   sign = L' ';

		if (m_usDecSymbol == L',') sym = L',';
		if (d13Value < 0) {
			sign = L'-';
			d13Value *= (-1);
		}

		switch (m_usDecPlace) {
		case 3:
			d13Amt = d13Value / 1000;
			lDec = d13Value % 1000;
			break;
		case 2:
		default:
			d13Amt = d13Value / 100;
			lDec = d13Value % 100;
			break;
		}
		x.Format(L"%c%lld%c%2.2ld", sign, d13Amt, sym, abs(lDec));
	}

	return x;
}

CString CPCSampleDoc::MakeCurrencyString(LONG lValue)
{
	CString  x;

	if (m_usDecPlace == 0) {
		x.Format(L"%ld", lValue);
	}
	else {
		LONG      lAmt = 0;
		LONG      lDec = 0;
		wchar_t   sym = L'.';
		wchar_t   sign = L' ';

		if (m_usDecSymbol == L',') sym = L',';
		if (lValue < 0) {
			sign = L'-';
			lValue *= (-1);
		}

		switch (m_usDecPlace) {
		case 3:
			lAmt = lValue / 1000;
			lDec = lValue % 1000;
			break;
		case 2:
		default:
			lAmt = lValue / 100;
			lDec = lValue % 100;
			break;
		}
		x.Format(L"%c%ld%c%2.2ld", sign, lAmt, sym, abs(lDec));
	}

	return x;
}

CString CPCSampleDoc::MakeCurrencyString(ULONG ulValue)
{
	CString  x;

	x.Format(L"ld", ulValue);

	return x;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::SetReportString()
//
//  PURPOSE :   Add the specified string to string list.
//
//  RETURN :    TRUE    - Specified string is added
//              FALSE   - Function is Failed
//
//===========================================================================

BOOL CPCSampleDoc::SetReportString(
    LPCWSTR lpszSource          // source string to add
    )
{
    // --- add to string list for report ---
    m_listReportStr.AddTail( lpszSource );

	if (ulReportOptionsMap & CPCSampleDoc::ulReportOptionsReportPrint) {
		m_listCurrentStr.AddTail (lpszSource);
	}

    // --- save no. of columns, if the string is the logest---
    DWORD   dwNoOfColumns;

    dwNoOfColumns = _tcslen( lpszSource );

    if ( m_dwMaxColumnInReportStr < dwNoOfColumns )
    {
        m_dwMaxColumnInReportStr = dwNoOfColumns;
    }

    // --- update all viewer with new document ---
    UpdateAllViews( NULL, 1 );

	// while we are updating views we need to be able to handle windows messages
	// without this code the application will appear frozen for the duration of
	// some action such as reading the Electronic Journal.
	MSG  mssg;
	// prime the message structure
	PeekMessage (&mssg, NULL, 0, 0, PM_NOREMOVE);
	// run till completed
	while (mssg.message != WM_QUIT) {
		// is there a message to process?
		if (PeekMessage (&mssg, NULL, 0, 0, PM_REMOVE) ) {
			// dispatch the message
			TranslateMessage (&mssg);
			DispatchMessage (&mssg);
		} else {
			break;
		}
	}

    // --- trace string to debug window ---

//    TRACE( lpszSource );  removed TRACE since lpszSource may have percent signs in it leading to format errors.
    TRACE( CString( "\r\n" ));

    // --- write added string to log file ---
    CArchive ar(&m_file, CArchive::store );

    Serialize( ar );

    return ( TRUE );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::SetReportString()
//
//  PURPOSE :   Add the specified string to string list.
//
//  RETURN :    TRUE    - Specified string is added
//              FALSE   - Function is Failed
//
//===========================================================================

INT_PTR CPCSampleDoc::SetReportSectionHeader(const CString & strReport, bool bTwoColumn)
{
	ListReportList sectionHeader;

	sectionHeader.m_ListOffset = m_listReportStr.GetCount();
	sectionHeader.m_SectionTitle = strReport;
	sectionHeader.m_bTwoColumn = bTwoColumn;

	m_listReportList.AddTail(sectionHeader);
	return sectionHeader.m_ListOffset;
}

BOOL CPCSampleDoc::MoveToReportSectionHeader(const CString & strReport)
{

	POSITION  pos = m_listReportList.GetHeadPosition();
	while (pos != NULL) {
		ListReportList sectionHeader = m_listReportList.GetNext(pos);
		if (strReport == sectionHeader.m_SectionTitle) {
			POSITION pos = GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = GetNextView(pos);
				if (pView)
					pView->PostMessage(WM_APP + 10, sectionHeader.m_ListOffset, 1);
			}
		}
	}
	return TRUE;
}

BOOL CPCSampleDoc::MoveToReportSectionHeader (INT_PTR  ipListOffset)
{

	POSITION  pos = m_listReportList.GetHeadPosition();
	while (pos != NULL) {
		ListReportList sectionHeader = m_listReportList.GetNext(pos);
		if (ipListOffset == sectionHeader.m_ListOffset) {
			POSITION pos = GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = GetNextView(pos);
				if (pView)
					pView->PostMessage(WM_APP + 10, sectionHeader.m_ListOffset, 1);
			}
		}
	}
	return TRUE;
}

POSITION  CPCSampleDoc::GetReportSectionHeader(POSITION  pos, ListReportList &sectionHeader)
{
	if (pos == NULL) {
		pos = m_listReportList.GetHeadPosition();

	}

	if (pos)
		sectionHeader = m_listReportList.GetNext(pos);

	return pos;
}

BOOL CPCSampleDoc::SetReportString( const CString& strReport )
{
    return ( SetReportString( strReport.operator LPCTSTR () ));
}

BOOL  CPCSampleDoc::SetSpreadSheetString (const CString& strReport)
{
	if (!FAILED(hSheetFile.m_hFile)) {
		char largebuffer[512] = {0};

		sprintf (largebuffer, "%S", strReport);
		hSheetFile.Write (largebuffer, strlen(largebuffer));
	}

	return TRUE;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::GetReportString()
//
//  PURPOSE :   Get the string in report string list by ndlek
//
//  RETURN :    TRUE    - Specified string is added
//              FALSE   - Function is Failed
//
//===========================================================================

BOOL CPCSampleDoc::GetReportString(
    CString&    str,            // variable to retrieve string
    const int   nIndex          // zero based index of report list
    )
{
    BOOL        fSuccess;
    POSITION    pos;

    // --- get position of specified index in report list ---
    pos = m_listReportStr.FindIndex( nIndex );
    if ( pos != NULL )
    {
        // --- get target string in report list by position ---
        str = m_listReportStr.GetAt( pos );
        fSuccess = TRUE;
    }
    else
    {
        // --- Function is Failed ---
        fSuccess = FALSE;
    }
    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportFeedLines()
//
//  PURPOSE :   Add feed line to report list.
//
//  RETURN :    TRUE    - Feed line is added.
//              FALSE   - Function is Failed
//
//  DESCRIPTION:  number of lines to feed
//===========================================================================

void CPCSampleDoc::ReportFeedLines(const DWORD dwNoOfLines)
{
    CString str( "" );

    // --- add no. of feed lines to report list ---
    for ( DWORD dwI = 0; dwI < dwNoOfLines; dwI++ )
    {
        SetReportString( str );
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportError()
//
//  PURPOSE :   Report error with specified error code.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportError(
    const UCHAR uchClass,       // error class
    const int   nErrorLocus,    // error locus
    const short sResult,        // error code
    const ULONG ulSubItem )     // error occured item number
{
    LPCTSTR lpszClass;

    // --- get error class string ---

    switch ( uchClass )
    {
    case CLASS_TTLREGFIN:
        lpszClass = _T("REGFIN");
        break;

    case CLASS_TTLHOURLY:
        lpszClass = _T("HOURLY");
        break;

    case CLASS_TTLDEPT:
        lpszClass = _T("DEPT");
        break;

    case CLASS_TTLPLU:
        lpszClass = _T("PLU");
        break;

    case CLASS_TTLCASHIER:
        lpszClass = _T("OPERATOR");
        break;

    case CLASS_TTLGUEST:
        lpszClass = _T("GCF");
        break;

    case CLASS_TTLCPN:
        lpszClass = _T("COUPON");
        break;

    case CLASS_RPTEJ:
        lpszClass = _T("EJ");
        break;

    case CLASS_RPTEMPLOYEE:
        lpszClass = _T("ETK");
        break;

    case CLASS_RPTCPMTALLY:
        lpszClass = _T("CPM");
        break;

    case CLASS_TTLSERVICE:
        lpszClass = _T("SERVICE");
        break;

    case CLASS_TTLINDFIN:
        lpszClass = _T("IND-FIN");
        break;

    case pcsample::CLASS_PARAMETER:
        lpszClass = _T("PARAMETER");
        break;

    case CLASS_TTLWAITER:
    default:
        lpszClass = _T("MISC");
        break;
    }

    LPCTSTR lpszLocus[] =
    {
        _T("SUCCESS"),
        _T("ALL LOCK"),
        _T("ISSUED CHECK"),
        _T("TOTAL RESET"),
        _T("ALL ID READ"),
        _T("TOTAL READ"),
        _T("IND LOCK"),
        _T("IND UNLOCK"),
        _T("IND READ"),
        _T("CHK EXIST"),
        _T("MISC OPR."),
        _T("RESET ISSUED"),
        _T("LOCK TERMINAL"),
        _T("UNLOCK TERM."),
        _T("CANCEL FUNC."),
        _T("ALL READ"),
        _T("ALL WRITE"),
        _T("PORT OPEN"),
        _T("PORT CLOSE"),
        _T("COULD NOT LOG ON"),
        _T("LOG OFF"),
        _T("CHG PASSWORD"),
		_T("NEVER CLOCKED IN"),
		_T("NO ENTRY EXISTS"),
		_T("MAX ITEMS"),
		_T("ALL UNLOCK"),
		_T("HEADER READ"),
		_T("LINE DATA READ")
    };

    ASSERT( nErrorLocus < pcsample::ERR_LINEREAD );

    CString str;
    if ( 0 < ulSubItem )
    {
        // --- report error message with specified item# ---
        str.Format( _T("  *** ERROR:%s(REC#=%lu)  %s%5d ***"), lpszClass, ulSubItem, lpszLocus[ nErrorLocus ], sResult );
    }
    else
    {
        // --- report error message ---
        str.Format( _T("  *** ERROR:%s  %s%5d ***"), lpszClass, lpszLocus[ nErrorLocus ], sResult );
    }

    // --- set error message to report list ---
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  register financial total to report
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const CTtlRegFin& ttlRegFin)
{
    // --- report register financial report header ---
	INT_PTR  x = SetReportSectionHeader(L"Financial Report");
    SetReportString( CString( "  " ));
    ReportElement( _T("REGISTER FINANCIAL"), ttlRegFin.getFromDate(), ttlRegFin.getToDate());
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  individual financial total to report
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const CTtlIndFin& ttlIndFin)
{
    // --- report individual financial report header ---
    CString str;

	str.Format(_T(" %s = %2d"), _T("Individual Financial Report "), ttlIndFin.getTerminalNo());
	INT_PTR  x = SetReportSectionHeader(str);

    SetReportString( CString( "  " ));
    ReportElement( _T("INDIVIDUAL FINANCIAL"), ttlIndFin.getFromDate(), ttlIndFin.getToDate());

    // --- report selected terminal# ---
    SetReportString( CString( " --------------" ));

    str.Format( _T(" %s = %2d"), _T("TERMINAL#"), ttlIndFin.getTerminalNo());
    SetReportString( str );

    SetReportString( CString( " --------------" ));
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  register financial total to report
//
//===========================================================================
void CPCSampleDoc::ReportTotal(const CTtlRegFin& ttlRegFin, CParaMdcMem  & mdcMem)
{
    CString str;
	str.Format (L"C;Y%d;X%d;K\"Terminal %d\"\r\n", nSheetRow, nSheetCol, ttlRegFin.getTerminalNo());
	SetSpreadSheetString (str);
	nSheetRow++;
	str.Format (L"C;Y%d;X%d;K\"Account\"\r\nC;Y%d;X%d;K\"Count\"\r\nC;Y%d;X%d;K\"Total\"\r\nC;Y%d;X%d;K\"On Hand\"\r\n",
		nSheetRow, nSheetCol, nSheetRow, nSheetCol + 1, nSheetRow, nSheetCol + 2, nSheetRow, nSheetCol + 3);
	SetSpreadSheetString (str);
	nSheetRow++;

	D13DIGITS  lSubTotal;
	D13DIGITS  lSalesTotal;

	// --- report each element of total ---
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN1_ADR, CParaMdcMem::MdcBitD)) {          /* Check MDC#101-BIT0  */
		ReportElement(_T("Current Gross"), ttlRegFin.getCurrentGrossTotal());
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN1_ADR, CParaMdcMem::MdcBitC)) {          /* Check MDC#101-BIT1  */
		ReportElement( _T("Net Total"),             ttlRegFin.getNetTotal());
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN2_ADR, CParaMdcMem::MdcBitB)) {         /* Check MDC#102-BIT2  */
		ReportElement(_T(""));
	}

    ReportElement( _T("Daily Gross"),           ttlRegFin.getDailyGrossTotal());
	lSubTotal = ttlRegFin.getDailyGrossTotal();

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN1_ADR, CParaMdcMem::MdcBitB)) {          /* Check MDC#101-BIT2  */
		ReportElement(_T("Training Total"), ttlRegFin.getTrainingTotal());
	}
	lSubTotal += ttlRegFin.getTrainingTotal();

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN1_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Void Total"), ttlRegFin.getPlusVoid());
	}
	lSubTotal += ttlRegFin.getPlusVoid().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN2_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Preselect Void"), ttlRegFin.getPreselectVoid());
	}
	lSubTotal += ttlRegFin.getPreselectVoid().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN46_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Transaction Return"), ttlRegFin.getTransactionReturn());
	}
	lSubTotal += ttlRegFin.getTransactionReturn().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN46_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Transaction Exchange"), ttlRegFin.getTransactionExchng());
	}
	lSubTotal += ttlRegFin.getTransactionExchng().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN46_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Transaction Refund"), ttlRegFin.getTransactionRefund());
	}
	lSubTotal += ttlRegFin.getTransactionRefund().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN2_ADR, CParaMdcMem::MdcBitC)) {         /* Check MDC#102-BIT1  */
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN2_ADR, CParaMdcMem::MdcBitB)) {         /* Check MDC#102-BIT2  */
		ReportElement(_T(""));
	}

	lSalesTotal = 0;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN2_ADR, CParaMdcMem::MdcBitA)) {         /* Check MDC#102-BIT3  */
		ReportElement(_T("Consolidated Coupon"), ttlRegFin.getConsolidatedCoupon());
	}
	lSubTotal += ttlRegFin.getConsolidatedCoupon().lAmount;

	struct {
		SHORT                      sMdcAddrTtl;    // indicates if discount key is a Discount or a Surcharge
		CParaMdcMem::MdcBitFlags   sBitTtl;
		SHORT                      sMdcAddrRpt;    // indicates if discount is to appear on report or not
		CParaMdcMem::MdcBitFlags   sBitRpt;
		wchar_t *  sTrnMnem;       // transaction mnemonic identifier for report mnemonic
	} ItemDiscountTotalsList[] = {
		{ MDC_MODID13_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN3_ADR, CParaMdcMem::MdcBitD, _T("Item Discount #1") },
		{ MDC_MODID23_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN3_ADR, CParaMdcMem::MdcBitC, _T("Item Discount #2 (MODIFY)") },
		{ MDC_MODID33_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN45_ADR, CParaMdcMem::MdcBitD, _T("Item Discount #3") },
		{ MDC_MODID43_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN45_ADR, CParaMdcMem::MdcBitC, _T("Item Discount #4") },
		{ MDC_MODID53_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN45_ADR, CParaMdcMem::MdcBitB, _T("Item Discount #5") },
		{ MDC_MODID63_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN45_ADR, CParaMdcMem::MdcBitA, _T("Item Discount #6") }
	},
		RegDiscountTotalsList[] = {
			{ MDC_RDISC13_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN3_ADR, CParaMdcMem::MdcBitB, _T("Trans Discount #1") },
			{ MDC_RDISC23_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN3_ADR, CParaMdcMem::MdcBitA, _T("Trans Discount #2") },
			{ MDC_RDISC33_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN29_ADR, CParaMdcMem::MdcBitC, _T("Trans Discount #3") },
			{ MDC_RDISC43_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN29_ADR, CParaMdcMem::MdcBitB, _T("Trans Discount #4") },
			{ MDC_RDISC53_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN29_ADR, CParaMdcMem::MdcBitA, _T("Trans Discount #5") },
			{ MDC_RDISC63_ADR, CParaMdcMem::MdcBitD, MDC_RPTFIN30_ADR, CParaMdcMem::MdcBitD, _T("Trans Discount #6") }
	},
		TenderTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN6_ADR, CParaMdcMem::MdcBitD, _T("Tender #1 (CASH)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN6_ADR, CParaMdcMem::MdcBitC, _T("Tender #2 (CHECK)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN6_ADR, CParaMdcMem::MdcBitB, _T("Tender #3 (CHARGE)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN6_ADR, CParaMdcMem::MdcBitA, _T("Tender #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN7_ADR, CParaMdcMem::MdcBitD, _T("Tender #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN7_ADR, CParaMdcMem::MdcBitC, _T("Tender #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN7_ADR, CParaMdcMem::MdcBitB, _T("Tender #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN7_ADR, CParaMdcMem::MdcBitA, _T("Tender #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN25_ADR, CParaMdcMem::MdcBitB, _T("Tender #9") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN25_ADR, CParaMdcMem::MdcBitA, _T("Tender #10") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN26_ADR, CParaMdcMem::MdcBitD, _T("Tender #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN39_ADR, CParaMdcMem::MdcBitB, _T("Tender #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN39_ADR, CParaMdcMem::MdcBitA, _T("Tender #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN40_ADR, CParaMdcMem::MdcBitD, _T("Tender #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN40_ADR, CParaMdcMem::MdcBitC, _T("Tender #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN40_ADR, CParaMdcMem::MdcBitB, _T("Tender #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN40_ADR, CParaMdcMem::MdcBitA, _T("Tender #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN41_ADR, CParaMdcMem::MdcBitD, _T("Tender #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN41_ADR, CParaMdcMem::MdcBitC, _T("Tender #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN41_ADR, CParaMdcMem::MdcBitB, _T("Tender #20") }
	},
		ForeignTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitB, _T("Foreign Total #1") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitA, _T("Foreign Total #2") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN32_ADR, CParaMdcMem::MdcBitB, _T("Foreign Total #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN32_ADR, CParaMdcMem::MdcBitA, _T("Foreign Total #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN33_ADR, CParaMdcMem::MdcBitD, _T("Foreign Total #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN33_ADR, CParaMdcMem::MdcBitC, _T("Foreign Total #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN33_ADR, CParaMdcMem::MdcBitB, _T("Foreign Total #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN33_ADR, CParaMdcMem::MdcBitA, _T("Foreign Total #8") }
	},
		TotalTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN9_ADR, CParaMdcMem::MdcBitB, _T("Total #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN9_ADR, CParaMdcMem::MdcBitA, _T("Total #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN10_ADR, CParaMdcMem::MdcBitD,  _T("Total #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN10_ADR, CParaMdcMem::MdcBitC, _T("Total #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN10_ADR, CParaMdcMem::MdcBitB, _T("Total #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN10_ADR, CParaMdcMem::MdcBitA, _T("Total #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN31_ADR, CParaMdcMem::MdcBitC, _T("Total #10") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN31_ADR, CParaMdcMem::MdcBitB, _T("Total #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN31_ADR, CParaMdcMem::MdcBitA, _T("Total #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN37_ADR, CParaMdcMem::MdcBitB, _T("Total #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN37_ADR, CParaMdcMem::MdcBitA, _T("Total #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN38_ADR, CParaMdcMem::MdcBitD, _T("Total #15") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN38_ADR, CParaMdcMem::MdcBitC, _T("Total #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN38_ADR, CParaMdcMem::MdcBitB, _T("Total #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN38_ADR, CParaMdcMem::MdcBitA, _T("Total #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN39_ADR, CParaMdcMem::MdcBitD, _T("Total #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN39_ADR, CParaMdcMem::MdcBitC, _T("Total #20") }
	},
		BonusTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN19_ADR, CParaMdcMem::MdcBitB,  _T("Total Bonus #1") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN19_ADR, CParaMdcMem::MdcBitA,  _T("Total Bonus #2") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN20_ADR, CParaMdcMem::MdcBitD,  _T("Total Bonus #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN20_ADR, CParaMdcMem::MdcBitC,  _T("Total Bonus #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN20_ADR, CParaMdcMem::MdcBitB,  _T("Total Bonus #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN20_ADR, CParaMdcMem::MdcBitA,  _T("Total Bonus #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN21_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN21_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #9") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #10") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #15") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #20") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #21") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #22") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #23") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #24") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #25") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #26") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #27") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #28") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #29") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN42_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #30") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #31") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #32") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #33") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #34") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #35") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #36") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #37") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #38") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #39") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #40") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #41") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #42") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #43") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #44") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #45") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #46") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #47") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #48") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #49") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #50") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #51") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #52") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #53") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #54") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #55") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #56") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #57") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #58") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #59") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #60") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #61") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #62") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #63") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #64") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #65") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #66") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #67") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #68") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #69") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN43_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #70") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #71") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #72") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #73") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #74") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #75") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #76") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #77") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #78") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #79") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #80") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #81") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #82") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #83") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #84") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #85") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #86") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #87") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #88") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #89") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #90") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #91") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #92") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #93") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #94") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #95") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #96") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #97") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #98") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #99") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTFIN44_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #100") },
	};

	for (int i = 0; i < sizeof(ItemDiscountTotalsList) / sizeof(ItemDiscountTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrTtl, ItemDiscountTotalsList[i].sBitTtl)) {
			// is an Item Discount rather than an Item Surcharge so print in the Item Discount section.
			lSubTotal += ttlRegFin.getItemDiscount(i + 1).lAmount;
			if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrRpt, ItemDiscountTotalsList[i].sBitRpt)) {
				ReportElement(ItemDiscountTotalsList[i].sTrnMnem, ttlRegFin.getItemDiscount(i + 1));
				lSalesTotal += ttlRegFin.getItemDiscount(i + 1).lAmount;
			}
		}
	}

	for (int i = 0; i < sizeof(RegDiscountTotalsList) / sizeof(RegDiscountTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrTtl, RegDiscountTotalsList[i].sBitTtl)) {
			// is a Transaction Discount rather than a Transaction Surcharge so print in the Transaction Discount section.
			lSubTotal += ttlRegFin.getRegularDiscount(i + 1).lAmount;
			if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrRpt, RegDiscountTotalsList[i].sBitRpt)) {
				ReportElement(RegDiscountTotalsList[i].sTrnMnem, ttlRegFin.getRegularDiscount(i + 1));
				lSalesTotal += ttlRegFin.getRegularDiscount(i + 1).lAmount;
			}
		}
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN29_ADR, CParaMdcMem::MdcBitD)) {          /* Check MDC#103-BIT3  */
		ReportElement(_T("Combination Coupon"), ttlRegFin.getCombinationCoupon());
	}
	lSubTotal += ttlRegFin.getCombinationCoupon().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN35_ADR, CParaMdcMem::MdcBitD)) {          /* Check MDC#103-BIT3  */
		ReportElement(_T("UPC Coupon"), ttlRegFin.getUPCCoupon(1));          // ### ADD Saratoga (052900)
	}
	lSubTotal += ttlRegFin.getUPCCoupon(1).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitD)) {         /* Check MDC#104-BIT0  */
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitC)) {         /* Check MDC#104-BIT1  */
		ReportElement(_T(""));
	}
		
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitB)) {         /* Check MDC#104-BIT2  */
		ReportElement(_T("Paid Out"), ttlRegFin.getPaidOut());
	}
	lSubTotal += ttlRegFin.getPaidOut().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitA)) {         /* Check MDC#104-BIT3  */
		ReportElement(_T("Tips Paid Out"), ttlRegFin.getTipsPaidOut());
	}
	lSubTotal += ttlRegFin.getTipsPaidOut().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN5_ADR, CParaMdcMem::MdcBitD)) {          /* Check MDC#105-BIT0  */
		ReportElement(_T("Received on Account"), ttlRegFin.getReceivedOnAccount());
	}
	lSubTotal += ttlRegFin.getReceivedOnAccount().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN5_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Charge Tips #1"), ttlRegFin.getChargeTips(1));
	}
	lSubTotal += ttlRegFin.getChargeTips(1).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN30_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Charge Tips #2"), ttlRegFin.getChargeTips(2));
	}
	lSubTotal += ttlRegFin.getChargeTips(2).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN30_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Charge Tips #3"), ttlRegFin.getChargeTips(3));
	}
	lSubTotal += ttlRegFin.getChargeTips(3).lAmount;

	if (!(mdcMem.CheckMdcBit(MDC_DEPT1_ADR, CParaMdcMem::MdcBitC))) {      /* Check MDC#27-BIT2   */
		lSubTotal += ttlRegFin.getHashDepartment().lAmount;
		if (!mdcMem.CheckMdcBit(MDC_RPTFIN22_ADR, CParaMdcMem::MdcBitC)) {        /* Check MDC#122-BIT1  */
			ReportElement(_T("Hash Department"), ttlRegFin.getHashDepartment());
		}
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN34_ADR, CParaMdcMem::MdcBitD)) {         /* Check MDC#357-BIT0  */
		ReportElement(_T("Food Stamp Credit"), ttlRegFin.getFoodStampCredit());       // ### ADD Saratoga (052900)
	}
	lSubTotal += ttlRegFin.getFoodStampCredit().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitD)) {         /* Check MDC#104-BIT0  */
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTFIN4_ADR, CParaMdcMem::MdcBitC)) {         /* Check MDC#104-BIT1  */
		ReportElement(_T(""));
	}

	// TENDHT
	lSubTotal = 0L;
	for (int i = 0; i < sizeof(TenderTotalsList) / sizeof(TenderTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(TenderTotalsList[i].sMdcAddrRpt, TenderTotalsList[i].sBitRpt)) {
			ReportElement(TenderTotalsList[i].sTrnMnem, ttlRegFin.getNormalizedTender(i + 1).OnlineTotal.lAmount);
		}
		// we want to add up all tenders including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of tenders.
		lSubTotal += ttlRegFin.getNormalizedTender(i + 1).OnlineTotal.lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T(""));
	}

	// Foreign Currency should report amount using the local currency which involves doing the
	// calculation to convert the foreign currency amount into local currency amount using the
	// currency conversion formula and rate.
	for (int i = 0; i < sizeof(ForeignTotalsList) / sizeof(ForeignTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(ForeignTotalsList[i].sMdcAddrRpt, ForeignTotalsList[i].sBitRpt)) {
			ULONG  ulConversion = m_currencyRate.getRate(CNV_NO1_ADR + i);
			ReportElement(ForeignTotalsList[i].sTrnMnem, ttlRegFin.getForeginTotal(i + 1), ulConversion);
		}
		// need to have the calculations to convert foreign currency into local currency.
		//		lSubTotal += ttlRegFin.getForeginTotal(i + 1).Total.lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN8_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T(""));
	}

	lSubTotal = 0L;
	TOTAL  lSvcTotal[3] = { 0 };

	for (int i = 0; i < sizeof(TotalTotalsList) / sizeof(TotalTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(TotalTotalsList[i].sMdcAddrRpt, TotalTotalsList[i].sBitRpt)) {
			ReportElement(TotalTotalsList[i].sTrnMnem, ttlRegFin.getServiceTotal(i + 1));
		}
		// we want to add up all totals including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of totals.
		lSubTotal += ttlRegFin.getServiceTotal(i + 1).lAmount;
		this->m_totalKeyData.updateServiceTotal(i + 1, ttlRegFin.getServiceTotal(i + 1), lSvcTotal);
	}
	//	ReportElement(_T("SUBTOTAL"), lSubTotal);
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T(""));
	}

	// First generate a subtotal of Serviced Checks and then generate a separate subtotal of Add Checks
	// and then subtract Add Check total from Serviced Check total to arrive at outstanding check total.
	lSubTotal = 0L;
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(L"Service Total #1", lSvcTotal[0]);
		lSubTotal += lSvcTotal[0].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(L"Service Total #2", lSvcTotal[1]);
		lSubTotal += lSvcTotal[1].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(L"Service Total #3", lSvcTotal[2]);
		lSubTotal += lSvcTotal[2].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN12_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTFIN12_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T(""));
	}

	{
		DCURRENCY  lTtlAddCheck = 0;
		!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("Add Check Total #1"), ttlRegFin.getAddCheckTotal(1));
		lTtlAddCheck += ttlRegFin.getAddCheckTotal(1).lAmount;
		!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("Add Check Total #2"), ttlRegFin.getAddCheckTotal(2));
		lTtlAddCheck += ttlRegFin.getAddCheckTotal(2).lAmount;
		!mdcMem.CheckMdcBit(MDC_RPTFIN11_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("Add Check Total #3"), ttlRegFin.getAddCheckTotal(3));
		lTtlAddCheck += ttlRegFin.getAddCheckTotal(3).lAmount;
		!mdcMem.CheckMdcBit(MDC_RPTFIN13_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T("  SUBTOTAL"), lTtlAddCheck);
		!mdcMem.CheckMdcBit(MDC_RPTFIN13_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("Outstanding Total"), lSubTotal - lTtlAddCheck);
		!mdcMem.CheckMdcBit(MDC_RPTFIN13_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T(""));
	}

	lSubTotal = 0L;
	!mdcMem.CheckMdcBit(MDC_RPTFIN14_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Charge Tips #1"),  ttlRegFin.getChargeTips( 1 ));
	lSubTotal += ttlRegFin.getChargeTips(1).lAmount;
	!mdcMem.CheckMdcBit(MDC_RPTFIN30_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("Charge Tips #2"),  ttlRegFin.getChargeTips( 2 ));
	lSubTotal += ttlRegFin.getChargeTips(2).lAmount;
	!mdcMem.CheckMdcBit(MDC_RPTFIN31_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Charge Tips #3"),  ttlRegFin.getChargeTips( 3 ));
	lSubTotal += ttlRegFin.getChargeTips(3).lAmount;
	!mdcMem.CheckMdcBit(MDC_RPTFIN14_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T("Tips Paid Out"), ttlRegFin.getTipsPaidOut());
	lSubTotal += ttlRegFin.getTipsPaidOut().lAmount;
	!mdcMem.CheckMdcBit(MDC_RPTFIN14_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("  SUBTOTAL"), lSubTotal);
	!mdcMem.CheckMdcBit(MDC_RPTFIN14_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTFIN15_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Declared Tips"), ttlRegFin.getDeclaredTips());
	!mdcMem.CheckMdcBit(MDC_RPTFIN15_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTFIN15_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("Tax #1 Gross"),      ttlRegFin.getTaxGross(  1 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN15_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("  Tax #1 Collect"),    ttlRegFin.getTaxAmount( 1 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN16_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("  Tax #1 Exempt"),     ttlRegFin.getTaxExempt( 1 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN34_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("  Tax #1 FST Exempt"), ttlRegFin.getFSTaxExempt( 1 ));       // ### ADD Saratoga (052900)
	!mdcMem.CheckMdcBit(MDC_RPTFIN16_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("Tax #2 Gross"),          ttlRegFin.getTaxGross(  2 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN16_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("  Tax #2 Collect"),        ttlRegFin.getTaxAmount( 2 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN16_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("  Tax #2 Exempt"),         ttlRegFin.getTaxExempt( 2 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN34_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("  Tax #2 FST Exempt"),      ttlRegFin.getFSTaxExempt( 2 ));       // ### ADD Saratoga (052900)
	!mdcMem.CheckMdcBit(MDC_RPTFIN17_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Tax #3 Gross"),          ttlRegFin.getTaxGross(  3 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN17_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("  Tax #3 Collect"),        ttlRegFin.getTaxAmount( 3 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN17_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("  Tax #3 Exempt"),         ttlRegFin.getTaxExempt( 3 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN34_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("  Tax #3 FST Exempt"),      ttlRegFin.getFSTaxExempt( 3 ));       // ### ADD Saratoga (052900)
	!mdcMem.CheckMdcBit(MDC_RPTFIN17_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("Tax #4 Gross"),          ttlRegFin.getTaxGross(  4 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN18_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("  Tax #4 Collect"),        ttlRegFin.getTaxAmount( 4 ));
	!mdcMem.CheckMdcBit(MDC_RPTFIN18_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("  Tax #4 Exempt"),         ttlRegFin.getTaxExempt( 4 ));
    //ReportElement( _T("TAX #4 FSTEXEMPT",      ttlRegFin.getFSTaxExempt( 4 ));       // ### ADD Saratoga (052900)
	!mdcMem.CheckMdcBit(MDC_RPTFIN18_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("Non-Taxable"),           ttlRegFin.getNonTaxable() );
	!mdcMem.CheckMdcBit(MDC_RPTFIN19_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Consolidation Tax"),     ttlRegFin.getConsolidationTax());
	!mdcMem.CheckMdcBit(MDC_RPTFIN19_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));

	lSubTotal = 0L;
	for (int i = 0; i < sizeof(BonusTotalsList) / sizeof(BonusTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(BonusTotalsList[i].sMdcAddrRpt, BonusTotalsList[i].sBitRpt)) {
			if (m_transactionMnemonics.m_bDataRead) {
				UINT  uiAddress = 0;
				if (i < 8) {
					uiAddress = 68 + i;
				}
				else {
					uiAddress = 203 + (i - 8);
				}
				ReportElement(m_transactionMnemonics.getMnemonic(uiAddress), ttlRegFin.getBonus(i + 1));
			}
			else {
				ReportElement(BonusTotalsList[i].sTrnMnem, ttlRegFin.getBonus(i + 1));
			}
		}
		// we want to add up all bonus totals including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of bonus totals.
		lSubTotal += ttlRegFin.getBonus(i + 1).lAmount;
	}
	!mdcMem.CheckMdcBit(MDC_RPTFIN21_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T(""));
	!mdcMem.CheckMdcBit(MDC_RPTFIN21_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T("  SUBTOTAL"), lSubTotal);
	!mdcMem.CheckMdcBit(MDC_RPTFIN21_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T(""));

	for (int i = 0; i < sizeof(ItemDiscountTotalsList) / sizeof(ItemDiscountTotalsList[0]); i++) {
		if (mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrTtl, ItemDiscountTotalsList[i].sBitTtl)) {
			// is an Item Surcharge rather than an Item Discount so print in the Item Surcharge section.
			if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrRpt, ItemDiscountTotalsList[i].sBitRpt)) {
				ReportElement(ItemDiscountTotalsList[i].sTrnMnem, ttlRegFin.getItemDiscount(i + 1));
			}
		}
	}

	for (int i = 0; i < sizeof(RegDiscountTotalsList) / sizeof(RegDiscountTotalsList[0]); i++) {
		if (mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrTtl, RegDiscountTotalsList[i].sBitTtl)) {
			// is a Transaction Surcharge rather than a Transaction Discount so print in the Transaction Surcharge section.
			lSalesTotal += ttlRegFin.getRegularDiscount(i + 1).lAmount;
			if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrRpt, RegDiscountTotalsList[i].sBitRpt)) {
				ReportElement(RegDiscountTotalsList[i].sTrnMnem, ttlRegFin.getRegularDiscount(i + 1));
				lSubTotal += ttlRegFin.getRegularDiscount(i + 1).lAmount;
			}
		}
	}
	!mdcMem.CheckMdcBit(MDC_RPTFIN22_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T(""));


	!mdcMem.CheckMdcBit(MDC_RPTFIN22_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("Transaction Cancel"),    ttlRegFin.getTransactionCancel());
	!mdcMem.CheckMdcBit(MDC_RPTFIN23_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T("Misc. Void"), ttlRegFin.getMiscVoid());
	!mdcMem.CheckMdcBit(MDC_RPTFIN32_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T("Loan"), ttlRegFin.getLoan());
	!mdcMem.CheckMdcBit(MDC_RPTFIN32_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T("Pickup"), ttlRegFin.getPickup());
	!mdcMem.CheckMdcBit(MDC_RPTFIN23_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTFIN23_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("Audaction"),             ttlRegFin.getAudaction());
	!mdcMem.CheckMdcBit(MDC_RPTFIN24_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("No Sale"),               ttlRegFin.getNoSaleCount());
	!mdcMem.CheckMdcBit(MDC_RPTFIN24_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("Item Productivity"),     DCOUNT(ttlRegFin.getItemProductivity()));
	!mdcMem.CheckMdcBit(MDC_RPTFIN24_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("No. of Person"),         ttlRegFin.getNoOfPerson());
	!mdcMem.CheckMdcBit(MDC_RPTFIN24_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("No. of Customer"),       ttlRegFin.getNoOfCustomer());
	!mdcMem.CheckMdcBit(MDC_RPTFIN25_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("No. of Opened Checks"),          ttlRegFin.getCheckOpened());
	!mdcMem.CheckMdcBit(MDC_RPTFIN25_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("No. of Closed Checks"),          ttlRegFin.getCheckClosed());
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  cashier total to report
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const CTtlCashier& ttlCashier)
{
    // --- report cashier total header ---
    CString str;

	str.Format(_T(" %s = %8.8u"), _T("Cashier Report "), ttlCashier.getCashierNo());
	INT_PTR  x = SetReportSectionHeader(str);

	SetReportString( CString( "  " ));
    ReportElement( _T("OPERATOR"), ttlCashier.getFromDate(), ttlCashier.getToDate());

    // --- report selected cashier# ---
    SetReportString( CString( " --------------" ));

    str.Format( _T(" %s = %8.8u"), _T("OPERATOR#"), ttlCashier.getCashierNo());
    SetReportString( str );

    SetReportString( CString( " --------------" ));
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  cashier total to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTtlCashier& ttlCashier, CParaMdcMem  & mdcMem)
{
    // --- report each element of total ---
    CString str;
	str.Format (L"C;Y%d;X%d;K\"Cashier %d\"\r\n",
		nSheetRow, nSheetCol, ttlCashier.getCashierNo());
	SetSpreadSheetString (str);
	nSheetRow++;
	str.Format (L"C;Y%d;X%d;K\"Account\"\r\nC;Y%d;X%d;K\"Count\"\r\nC;Y%d;X%d;K\"Total\"\r\nC;Y%d;X%d;K\"On Hand\"\r\n",
		nSheetRow, nSheetCol, nSheetRow, nSheetCol + 1, nSheetRow, nSheetCol + 2, nSheetRow, nSheetCol + 3);
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->SetId (ttlCashier.getCashierNo());
	}

	D13DIGITS  lSalesTotal = 0;

	ReportElement(_T("Cashier Daily Gross"), ttlCashier.getDailyGrossTotal());
	lSalesTotal = ttlCashier.getDailyGrossTotal();

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS1_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Void Total"), ttlCashier.getPlusVoid());
	}
	lSalesTotal += ttlCashier.getPlusVoid().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS1_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement( _T("Transaction Voids"),      ttlCashier.getPreselectVoid());
	}
	lSalesTotal += ttlCashier.getPreselectVoid().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS50_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement( _T("Transaction Returns"),    ttlCashier.getTransactionReturn());
	}
	lSalesTotal += ttlCashier.getTransactionReturn().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS50_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement( _T("Transaction Exchanges"),  ttlCashier.getTransactionExchng());
	}
	lSalesTotal += ttlCashier.getTransactionReturn().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS50_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Transaction Refunds"), ttlCashier.getTransactionRefund());
	}
	lSalesTotal += ttlCashier.getTransactionRefund().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS1_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement( _T("  SUBTOTAL"), lSalesTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS2_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T(""));
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS2_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement( _T("Consolidated Coupon"),   ttlCashier.getConsolidatedCoupon());
	}
	lSalesTotal += ttlCashier.getConsolidatedCoupon().lAmount;

	struct {
		SHORT                      sMdcAddrTtl;    // indicates if discount key is a Discount or a Surcharge
		CParaMdcMem::MdcBitFlags   sBitTtl;
		SHORT                      sMdcAddrRpt;    // indicates if discount is to appear on report or not
		CParaMdcMem::MdcBitFlags   sBitRpt;
		wchar_t *  sTrnMnem;       // transaction mnemonic identifier for report mnemonic
	} ItemDiscountTotalsList[] = {
		{MDC_MODID13_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS2_ADR, CParaMdcMem::MdcBitB, _T("Item Discount #1")},
		{MDC_MODID23_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS2_ADR, CParaMdcMem::MdcBitA, _T("Item Discount #2 (MODIFY)") },
		{MDC_MODID33_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS49_ADR, CParaMdcMem::MdcBitD, _T("Item Discount #3")},
		{MDC_MODID43_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS49_ADR, CParaMdcMem::MdcBitC, _T("Item Discount #4")},
		{MDC_MODID53_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS49_ADR, CParaMdcMem::MdcBitB, _T("Item Discount #5")},
		{MDC_MODID63_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS49_ADR, CParaMdcMem::MdcBitA, _T("Item Discount #6")}
	},
		RegDiscountTotalsList[] = {
		{ MDC_RDISC13_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS3_ADR, CParaMdcMem::MdcBitD, _T("Trans Discount #1") },
		{ MDC_RDISC23_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS3_ADR, CParaMdcMem::MdcBitC, _T("Trans Discount #2") },
		{ MDC_RDISC33_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS22_ADR, CParaMdcMem::MdcBitD, _T("Trans Discount #3") },
		{ MDC_RDISC43_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS22_ADR, CParaMdcMem::MdcBitC, _T("Trans Discount #4") },
		{ MDC_RDISC53_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS22_ADR, CParaMdcMem::MdcBitB, _T("Trans Discount #5") },
		{ MDC_RDISC63_ADR, CParaMdcMem::MdcBitD, MDC_RPTCAS22_ADR, CParaMdcMem::MdcBitA, _T("Trans Discount #6") }
	},
		TenderTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS5_ADR, CParaMdcMem::MdcBitB, _T("Tender #1 (CASH)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS5_ADR, CParaMdcMem::MdcBitA, _T("Tender #2 (CHECK)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS6_ADR, CParaMdcMem::MdcBitD, _T("Tender #3 (CHARGE)") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS6_ADR, CParaMdcMem::MdcBitC, _T("Tender #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS6_ADR, CParaMdcMem::MdcBitB, _T("Tender #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS6_ADR, CParaMdcMem::MdcBitA, _T("Tender #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS7_ADR, CParaMdcMem::MdcBitD, _T("Tender #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS7_ADR, CParaMdcMem::MdcBitC, _T("Tender #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS21_ADR, CParaMdcMem::MdcBitD, _T("Tender #9") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS21_ADR, CParaMdcMem::MdcBitC, _T("Tender #10") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS21_ADR, CParaMdcMem::MdcBitB, _T("Tender #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS38_ADR, CParaMdcMem::MdcBitB, _T("Tender #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS38_ADR, CParaMdcMem::MdcBitA, _T("Tender #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS39_ADR, CParaMdcMem::MdcBitD, _T("Tender #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS39_ADR, CParaMdcMem::MdcBitC, _T("Tender #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS39_ADR, CParaMdcMem::MdcBitB, _T("Tender #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS39_ADR, CParaMdcMem::MdcBitA, _T("Tender #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS40_ADR, CParaMdcMem::MdcBitD, _T("Tender #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS40_ADR, CParaMdcMem::MdcBitC, _T("Tender #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS40_ADR, CParaMdcMem::MdcBitB, _T("Tender #20") }
	},
		ForeignTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS8_ADR, CParaMdcMem::MdcBitD, _T("Foreign Total #1") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS8_ADR, CParaMdcMem::MdcBitC, _T("Foreign Total #2") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS27_ADR, CParaMdcMem::MdcBitB, _T("Foreign Total #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS27_ADR, CParaMdcMem::MdcBitA, _T("Foreign Total #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS28_ADR, CParaMdcMem::MdcBitD, _T("Foreign Total #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS28_ADR, CParaMdcMem::MdcBitC, _T("Foreign Total #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS28_ADR, CParaMdcMem::MdcBitB, _T("Foreign Total #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS28_ADR, CParaMdcMem::MdcBitA, _T("Foreign Total #8") }
	},
		TotalTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS9_ADR, CParaMdcMem::MdcBitD, _T("Total #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS9_ADR, CParaMdcMem::MdcBitC, _T("Total #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS9_ADR, CParaMdcMem::MdcBitB,  _T("Total #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS9_ADR, CParaMdcMem::MdcBitA, _T("Total #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS10_ADR, CParaMdcMem::MdcBitD, _T("Total #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS10_ADR, CParaMdcMem::MdcBitC, _T("Total #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS31_ADR, CParaMdcMem::MdcBitA, _T("Total #10") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS32_ADR, CParaMdcMem::MdcBitB, _T("Total #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS32_ADR, CParaMdcMem::MdcBitA, _T("Total #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS36_ADR, CParaMdcMem::MdcBitB, _T("Total #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS36_ADR, CParaMdcMem::MdcBitA, _T("Total #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS37_ADR, CParaMdcMem::MdcBitD, _T("Total #15") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS37_ADR, CParaMdcMem::MdcBitC, _T("Total #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS37_ADR, CParaMdcMem::MdcBitB, _T("Total #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS37_ADR, CParaMdcMem::MdcBitA, _T("Total #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS38_ADR, CParaMdcMem::MdcBitD, _T("Total #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS38_ADR, CParaMdcMem::MdcBitC, _T("Total #20") }
	},
		BonusTotalsList[] = {
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS15_ADR, CParaMdcMem::MdcBitD,  _T("Total Bonus #1") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS15_ADR, CParaMdcMem::MdcBitC,  _T("Total Bonus #2") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS15_ADR, CParaMdcMem::MdcBitB,  _T("Total Bonus #3") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS15_ADR, CParaMdcMem::MdcBitA,  _T("Total Bonus #4") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS16_ADR, CParaMdcMem::MdcBitD,  _T("Total Bonus #5") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS16_ADR, CParaMdcMem::MdcBitC,  _T("Total Bonus #6") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS16_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #7") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS16_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #8") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #9") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #10") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #11") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #12") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #13") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #14") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #15") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #16") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #17") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #18") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #19") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #20") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #21") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #22") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #23") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #24") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #25") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #26") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #27") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #28") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #29") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS46_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #30") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #31") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #32") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #33") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #34") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #35") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #36") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #37") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #38") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #39") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #40") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #41") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #42") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #43") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #44") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #45") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #46") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #47") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #48") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #49") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #50") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #51") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #52") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #53") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #54") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #55") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #56") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #57") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #58") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #59") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #60") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #61") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #62") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #63") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #64") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #65") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #66") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #67") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #68") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #69") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS47_ADR, CParaMdcMem::MdcBitA, _T("Total Bonus #70") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #71") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #72") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #73") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #74") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #75") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #76") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #77") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #78") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #79") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitD, _T("Total Bonus #80") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #81") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #82") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #83") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #84") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #85") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #86") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #87") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #88") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #89") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitC, _T("Total Bonus #90") },

			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #91") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #92") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #93") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #94") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #95") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #96") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #97") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #98") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #99") },
			{ 0, CParaMdcMem::MdcBitD, MDC_RPTCAS48_ADR, CParaMdcMem::MdcBitB, _T("Total Bonus #100") },
	};

	for (int i = 0; i < sizeof(ItemDiscountTotalsList) / sizeof(ItemDiscountTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrTtl, ItemDiscountTotalsList[i].sBitTtl)) {
			// is an Item Discount rather than an Item Surcharge so print in the Item Discount section.
			lSalesTotal += ttlCashier.getItemDiscount(i+1).lAmount;
			if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrRpt, ItemDiscountTotalsList[i].sBitRpt)) {
				ReportElement(ItemDiscountTotalsList[i].sTrnMnem, ttlCashier.getItemDiscount(i+1));
			}
		}
	}

	for (int i = 0; i < sizeof(RegDiscountTotalsList) / sizeof(RegDiscountTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrTtl, RegDiscountTotalsList[i].sBitTtl)) {
			// is a Transaction Discount rather than a Transaction Surcharge so print in the Transaction Discount section.
			lSalesTotal += ttlCashier.getRegularDiscount(i + 1).lAmount;
			if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrRpt, RegDiscountTotalsList[i].sBitRpt)) {
				ReportElement(RegDiscountTotalsList[i].sTrnMnem, ttlCashier.getRegularDiscount(i + 1));
			}
		}
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS21_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Combination Coupon"), ttlCashier.getCombinationCoupon());
	}
	lSalesTotal += ttlCashier.getCombinationCoupon().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS32_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("UPC Coupon"), ttlCashier.getUPCCoupon(1));          // ### ADD Saratoga (052900)
//		ReportElement(_T("UPC Coupon (Double)"), ttlCashier.getUPCCoupon(2));          // GenPOS Double Coupon Total
//		ReportElement(_T("UPC Coupon (Triple)"), ttlCashier.getUPCCoupon(3));          // GenPOS Triple Coupon Total
	}
	lSalesTotal += ttlCashier.getUPCCoupon(1).lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS3_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("  SUBTOTAL"), lSalesTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS3_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T(""));
	}
	
	D13DIGITS  lSubTotal = 0;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS4_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Paid Out"), ttlCashier.getPaidOut());
	}
	lSubTotal += ttlCashier.getPaidOut().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS4_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Tips Paid Out"), ttlCashier.getTipsPaidOut());
	}
	lSubTotal += ttlCashier.getTipsPaidOut().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS4_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Receive on Account"), ttlCashier.getReceivedOnAccount());
	}
	lSubTotal += ttlCashier.getReceivedOnAccount().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS4_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Charge Tips #1"), ttlCashier.getChargeTips(1));
	}
	lSubTotal += ttlCashier.getChargeTips(1).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS24_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Charge Tips #2"), ttlCashier.getChargeTips(2));
	}
	lSubTotal += ttlCashier.getChargeTips(2).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS24_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Charge Tips #3"), ttlCashier.getChargeTips(3));
	}
	lSubTotal += ttlCashier.getChargeTips(3).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS27_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Loan"), ttlCashier.getLoan());            // ADD Saratoga
	}
	lSubTotal += ttlCashier.getLoan().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS27_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T("Pickup"), ttlCashier.getPickup());          // ADD Saratoga
	}
	lSubTotal += ttlCashier.getPickup().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS17_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Hash Department"), ttlCashier.getHashDepartment());
	}
	if (!mdcMem.CheckMdcBit(MDC_DEPT1_ADR, CParaMdcMem::MdcBitC)) {
		lSubTotal += ttlCashier.getHashDepartment().lAmount;
	}

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS32_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Food Stamp Credit"), ttlCashier.getFoodStampCredit());       // ### ADD Saratoga (052900)
	}
	lSubTotal += ttlCashier.getFoodStampCredit().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS23_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Check Trans To"), ttlCashier.getCheckTransTo());       // ### ADD Saratoga (052900)
	}
	lSubTotal += ttlCashier.getCheckTransTo().lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS23_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Check Trans From"), ttlCashier.getCheckTransFrom());       // ### ADD Saratoga (052900)
	}
	lSubTotal += ttlCashier.getCheckTransFrom().lAmount;

	if (!mdcMem.CheckMdcBit(MDC_RPTCAS5_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS5_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(_T(""));
	}
	
	
	// TENDHT
	lSubTotal = 0L;
	for (int i = 0; i < sizeof(TenderTotalsList) / sizeof(TenderTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(TenderTotalsList[i].sMdcAddrRpt, TenderTotalsList[i].sBitRpt)) {
			ReportElement(TenderTotalsList[i].sTrnMnem, ttlCashier.getNormalizedTender(i + 1));
		}
		// we want to add up all tenders including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of tenders.
		lSubTotal += ttlCashier.getNormalizedTender(i + 1).Total.lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS7_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS7_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T(""));
	}

	// Foreign Currency should report amount using the local currency which involves doing the
	// calculation to convert the foreign currency amount into local currency amount using the
	// currency conversion formula and rate.
	for (int i = 0; i < sizeof(ForeignTotalsList) / sizeof(ForeignTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(ForeignTotalsList[i].sMdcAddrRpt, ForeignTotalsList[i].sBitRpt)) {
			ULONG  ulConversion = m_currencyRate.getRate(CNV_NO1_ADR + i);
			ReportElement(ForeignTotalsList[i].sTrnMnem, ttlCashier.getForeginTotal(i + 1).Total, ulConversion);
		}
		// need to have the calculations to convert foreign currency into local currency.
//		lSubTotal += ttlCashier.getForeginTotal(i + 1).Total.lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS8_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS8_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T(""));
	}

	lSubTotal = 0L;
	TOTAL  lSvcTotal[3] = { 0 };

	for (int i = 0; i < sizeof(TotalTotalsList) / sizeof(TotalTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(TotalTotalsList[i].sMdcAddrRpt, TotalTotalsList[i].sBitRpt)) {
			ReportElement(TotalTotalsList[i].sTrnMnem, ttlCashier.getServiceTotal(i + 1));
		}
		// we want to add up all totals including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of totals.
		lSubTotal += ttlCashier.getServiceTotal(i + 1).lAmount;
		this->m_totalKeyData.updateServiceTotal(i + 1, ttlCashier.getServiceTotal(i + 1), lSvcTotal);
	}
//	ReportElement(_T("SUBTOTAL"), lSubTotal);
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS10_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T(""));
	}

	lSubTotal = 0L;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS10_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(L"Service Total #1", lSvcTotal[0]);
		lSubTotal += lSvcTotal[0].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS11_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(L"Service Total #2", lSvcTotal[1]);
		lSubTotal += lSvcTotal[1].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS11_ADR, CParaMdcMem::MdcBitC)) {
		ReportElement(L"Service Total #3", lSvcTotal[2]);
		lSubTotal += lSvcTotal[2].lAmount;
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS11_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("  SUBTOTAL"), lSubTotal);
	}
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS11_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T(""));
	}


//	ReportElement( _T("VAT SERVICE TOTAL"),     ttlCashier.getVATServiceTotal());
	{
		DCURRENCY lSubTotal2 = 0L;

		!mdcMem.CheckMdcBit(MDC_RPTCAS12_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T("Add Check Total #1"), ttlCashier.getAddCheckTotal(1));
		lSubTotal2 += ttlCashier.getAddCheckTotal(1).lAmount;
		!mdcMem.CheckMdcBit(MDC_RPTCAS12_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T("Add Check Total #2"), ttlCashier.getAddCheckTotal(2));
		lSubTotal2 += ttlCashier.getAddCheckTotal(2).lAmount;
		!mdcMem.CheckMdcBit(MDC_RPTCAS12_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T("Add Check Total #3"), ttlCashier.getAddCheckTotal(3));
		lSubTotal2 += ttlCashier.getAddCheckTotal(3).lAmount;

		!mdcMem.CheckMdcBit(MDC_RPTCAS12_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T("  SUBTOTAL"), lSubTotal - lSubTotal2);
		!mdcMem.CheckMdcBit(MDC_RPTCAS13_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));
	}
	
	lSubTotal = 0L;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS13_ADR, CParaMdcMem::MdcBitB)) {
		ReportElement(_T("Charge Tips #1"), ttlCashier.getChargeTips(1));
	}
	lSubTotal += ttlCashier.getChargeTips(1).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS25_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Charge Tips #2"), ttlCashier.getChargeTips(2));
	}
	lSubTotal += ttlCashier.getChargeTips(2).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS26_ADR, CParaMdcMem::MdcBitD)) {
		ReportElement(_T("Charge Tips #3"), ttlCashier.getChargeTips(3));
	}
	lSubTotal += ttlCashier.getChargeTips(3).lAmount;
	if (!mdcMem.CheckMdcBit(MDC_RPTCAS13_ADR, CParaMdcMem::MdcBitA)) {
		ReportElement(_T("Tips Paid"), ttlCashier.getTipsPaidOut());
	}
	lSubTotal += ttlCashier.getTipsPaidOut().lAmount;
	!mdcMem.CheckMdcBit(MDC_RPTCAS14_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T("  SUBTOTAL"), lSubTotal);
	!mdcMem.CheckMdcBit(MDC_RPTCAS24_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTCAS25_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Declared Tips"),         ttlCashier.getDeclaredTips());
	!mdcMem.CheckMdcBit(MDC_RPTCAS14_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTCAS14_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("Consolidated Tax"),     ttlCashier.getConsolidationTax());
	!mdcMem.CheckMdcBit(MDC_RPTCAS14_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T(""));

	lSubTotal = 0L;
	for (int i = 0; i < sizeof(BonusTotalsList) / sizeof(BonusTotalsList[0]); i++) {
		if (!mdcMem.CheckMdcBit(BonusTotalsList[i].sMdcAddrRpt, BonusTotalsList[i].sBitRpt)) {
			if (m_transactionMnemonics.m_bDataRead) {
				UINT  uiAddress = 0;
				if (i < 8) {
					uiAddress = 68 + i;
				}
				else {
					uiAddress = 203 + (i - 8);
				}
				ReportElement(m_transactionMnemonics.getMnemonic(uiAddress), ttlCashier.getBonus(i + 1));
			}
			else {
				ReportElement(BonusTotalsList[i].sTrnMnem, ttlCashier.getBonus(i + 1));
			}
		}
		// we want to add up all bonus totals including those not printed which will highlight
		// a total is missing from the report if the subtotal does not match the list of bonus totals.
		lSubTotal += ttlCashier.getBonus(i + 1).lAmount;
	}
	!mdcMem.CheckMdcBit(MDC_RPTCAS17_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T(""));
	!mdcMem.CheckMdcBit(MDC_RPTCAS17_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T("  SUBTOTAL"), lSubTotal);
	!mdcMem.CheckMdcBit(MDC_RPTCAS17_ADR, CParaMdcMem::MdcBitB) && ReportElement(_T(""));

	for (int i = 0; i < sizeof(ItemDiscountTotalsList) / sizeof(ItemDiscountTotalsList[0]); i++) {
		if (mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrTtl, ItemDiscountTotalsList[i].sBitTtl)) {
			// is an Item Surcharge rather than an Item Discount so print in the Item Surcharge section.
			if (!mdcMem.CheckMdcBit(ItemDiscountTotalsList[i].sMdcAddrRpt, ItemDiscountTotalsList[i].sBitRpt)) {
				ReportElement(ItemDiscountTotalsList[i].sTrnMnem, ttlCashier.getItemDiscount(i + 1));
			}
		}
	}

	for (int i = 0; i < sizeof(RegDiscountTotalsList) / sizeof(RegDiscountTotalsList[0]); i++) {
		if (mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrTtl, RegDiscountTotalsList[i].sBitTtl)) {
			// is a Transaction Surcharge rather than a Transaction Discount so print in the Transaction Surcharge section.
			lSalesTotal += ttlCashier.getRegularDiscount(i + 1).lAmount;
			if (!mdcMem.CheckMdcBit(RegDiscountTotalsList[i].sMdcAddrRpt, RegDiscountTotalsList[i].sBitRpt)) {
				ReportElement(RegDiscountTotalsList[i].sTrnMnem, ttlCashier.getRegularDiscount(i + 1));
				lSubTotal += ttlCashier.getRegularDiscount(i + 1).lAmount;
			}
		}
	}
	!mdcMem.CheckMdcBit(MDC_RPTCAS18_ADR, CParaMdcMem::MdcBitD) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTCAS18_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("Transaction Cancel"),    ttlCashier.getTransactionCancel());
	!mdcMem.CheckMdcBit(MDC_RPTCAS18_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("Misc. Void"),            ttlCashier.getMiscVoid());
	!mdcMem.CheckMdcBit(MDC_RPTCAS18_ADR, CParaMdcMem::MdcBitA) && ReportElement(_T(""));
	
	!mdcMem.CheckMdcBit(MDC_RPTCAS19_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Audaction"),             ttlCashier.getAudaction());
	!mdcMem.CheckMdcBit(MDC_RPTCAS19_ADR, CParaMdcMem::MdcBitC) && ReportElement(_T(""));

	!mdcMem.CheckMdcBit(MDC_RPTCAS19_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("No Sale"),               ttlCashier.getNoSaleCount());
	!mdcMem.CheckMdcBit(MDC_RPTCAS19_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("Item Productivity"),     DCOUNT(ttlCashier.getItemProductivity()));
	!mdcMem.CheckMdcBit(MDC_RPTCAS20_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("No. of Persons"),        ttlCashier.getNoOfPerson());
	!mdcMem.CheckMdcBit(MDC_RPTCAS20_ADR, CParaMdcMem::MdcBitC) && ReportElement( _T("No. of Customers"),      ttlCashier.getNoOfCustomer());
	!mdcMem.CheckMdcBit(MDC_RPTCAS20_ADR, CParaMdcMem::MdcBitB) && ReportElement( _T("No. of Open Checks"),    ttlCashier.getCheckOpened());
	!mdcMem.CheckMdcBit(MDC_RPTCAS20_ADR, CParaMdcMem::MdcBitA) && ReportElement( _T("No. of Closed Checks"),  ttlCashier.getCheckClosed());
	!mdcMem.CheckMdcBit(MDC_RPTCAS23_ADR, CParaMdcMem::MdcBitD) && ReportElement( _T("Post Receipt Count"),    ttlCashier.getPostReceipt());

	ReportElement(_T(""));
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportParameter()
//
//  PURPOSE :   Report additional parameter of total report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportParameter( const CParaCashier& paraCashier )
{
    TCHAR    szCashierName[ CAS_NUMBER_OF_CASHIER_NAME + 1 ]; //CHAR TO TCHAR ESMITH

    paraCashier.getName( szCashierName );
    // --- report each element of parameter --
    CString str;
    str.Format( _T(" %s : %8.8u\t%s %2u\t%s %s"), _T("OPERATOR#"),paraCashier.getCashierNo(),
                _T("SECRET# :"),paraCashier.getSercretNo(),_T("NAME :"),szCashierName );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  hourly activity total to report
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CTtlHourly& ttlHourly)
{
    // --- report hourly activity total header ---
	INT_PTR  x = SetReportSectionHeader(L"Hourly Activity");
	
	SetReportString( CString( "  " ));
    ReportElement( _T("HOURLY ACTIVTY"), ttlHourly.getFromDate(), ttlHourly.getToDate());

    CString str;
	str.Format( _T(" %s\t%10s\t%s\t%10s"), _T("BLOCK#   Hr:Mn Hr:Mn"), _T("ITEM CO."), _T("#OF PERSON"), _T("TOTAL") );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  hourly activity total to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTtlHourly& ttlHourly)
{
    CString str;
	CParaHourlyTime  hourlyTime;

	SetReportString(L" ");
	SetReportString(L"  Hourly Totals");

	str.Format(_T("\t%s    %s  %s\t%s\t%s\t\t%s"), L"Block", L"From", L" To ", L"Count", L"Persons", L"Total");
	SetReportString(str);

	hourlyTime.Read();

	LONG       lCount = 0, lPersons = 0;
	DCURRENCY  dTotal = 0;

	for ( USHORT usBlock = 0; usBlock < pcttl::MAX_HOURLY_BLOCK; usBlock++ )
	{
		if (( ttlHourly.getItemProductivity( usBlock ) != 0 ) ||
            ( ttlHourly.getNoOfPerson( usBlock )       != 0 ) ||
            ( ttlHourly.getHourlyTotal( usBlock )      != 0 ))
		{
			CParaHourlyTime::HourlyTime  myTime1, myTime2;

			hourlyTime.GetBlockTime(usBlock + 1, myTime1);
			hourlyTime.GetBlockTime(usBlock + 2, myTime2);

			// --- report element, if each element is greater than 0 ---
            str.Format( _T("\t%2d       %2.2d:%2.2d %2.2d:%2.2d\t%10ld\t%5d\t\t%16s"), usBlock + 1, myTime1.uchHour, myTime1.uchMin, myTime2.uchHour, myTime2.uchMin, ttlHourly.getItemProductivity( usBlock ),
                        ttlHourly.getNoOfPerson( usBlock ), MakeCurrencyString(ttlHourly.getHourlyTotal( usBlock )));
            SetReportString( str );
			lCount += ttlHourly.getItemProductivity(usBlock);
			lPersons += ttlHourly.getNoOfPerson(usBlock);
			dTotal += ttlHourly.getHourlyTotal(usBlock);
		}
	}
	str.Format(_T("\t%2s               \t%10ld\t%5d\t\t%16s"), L"TotaLs", lCount, lPersons, MakeCurrencyString(dTotal));
	SetReportString(str);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CTtlServTime& ttlServTime)
{
    // --- report service time total header ---
	INT_PTR  x = SetReportSectionHeader(L"Service Time");
	
	SetReportString( CString( "  " ));
    ReportElement( _T("SERVICE TIME"), ttlServTime.getFromDate(), ttlServTime.getToDate());
    CString str;
	str.Format( _T(" %s\t%s\t%s\t%s"), _T("BLOCK#"), _T("BORDER"), _T("#OF TRANS"), _T("TTL TIME") );
    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\n"),
		nSheetRow, nSheetCol, _T("BLOCK#"),
		nSheetRow, nSheetCol + 1, _T("BORDER"),
		nSheetRow, nSheetCol + 2, _T("#OF TRANS"),
		nSheetRow, nSheetCol + 3, _T("TTL TIME"));
	SetSpreadSheetString (str);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  service time total to report
//
//===========================================================================
void CPCSampleDoc::ReportTotal(const CTtlServTime& ttlServTime)
{
    USHORT usBorder;
    CString str;
	CParaHourlyTime  hourlyTime;

	hourlyTime.Read();

    for ( USHORT usBlock = 0; usBlock < pcttl::MAX_HOURLY_BLOCK; usBlock++ )
    {
        // --- report element, if each element is greater than 0 ---
        if (( ttlServTime.getServiceCount( usBlock, 0 ) == 0 ) &&
            ( ttlServTime.getServiceTime ( usBlock, 0 ) == 0 ) &&
            ( ttlServTime.getServiceCount( usBlock, 1 ) == 0 ) &&
            ( ttlServTime.getServiceTime ( usBlock, 1 ) == 0 ) &&
            ( ttlServTime.getServiceCount( usBlock, 2 ) == 0 ) &&
            ( ttlServTime.getServiceTime ( usBlock, 2 ) == 0 ))
        {
           continue;
        }

		CParaHourlyTime::HourlyTime  myTime1, myTime2;

		hourlyTime.GetBlockTime(usBlock + 1, myTime1);
		hourlyTime.GetBlockTime(usBlock + 2, myTime2);

		usBorder = 0;
        str.Format( _T(" %d\t\t%d\t\t%d\t\t%s"), usBlock  + 1, usBorder + 1,
                    ttlServTime.getServiceCount( usBlock, usBorder ), MakeCurrencyString(ttlServTime.getServiceTime(  usBlock, usBorder )));
        SetReportString( str );

		str.Format(_T("C;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
			nSheetRow, nSheetCol, usBlock  + 1,
			nSheetRow, nSheetCol + 1, usBorder + 1,
			nSheetRow, nSheetCol + 2, ttlServTime.getServiceCount( usBlock, usBorder ),
			nSheetRow, nSheetCol + 3, MakeCurrencyString(ttlServTime.getServiceTime(  usBlock, usBorder )));
		SetSpreadSheetString (str);
		nSheetCol += 4;
        for( usBorder = 1; usBorder < pcttl::MAX_SERVICE_BLOCK; usBorder++ )
        {
            str.Format( _T("  \t\t%d\t\t%d\t\t%s"), usBorder + 1, ttlServTime.getServiceCount( usBlock, usBorder ),
				MakeCurrencyString(ttlServTime.getServiceTime(  usBlock, usBorder )));
            SetReportString( str );

			str.Format(_T("C;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
				nSheetRow, nSheetCol, usBorder + 1,
				nSheetRow, nSheetCol + 1, ttlServTime.getServiceCount( usBlock, usBorder ),
				nSheetRow, nSheetCol + 2, MakeCurrencyString(ttlServTime.getServiceTime(  usBlock, usBorder )));
			SetSpreadSheetString (str);
			nSheetCol += 3;
        }
		nSheetCol = 1;
		nSheetRow++;
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  CPM/EPT tally to report
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const CTallyCPM& tallyCPM, const DATE& ToDate)
{
    // --- report CPM/EPT tally report header ---
    SetReportString( CString( "  " ));
    ReportElement( _T("CPM/EPT TALLY"), ToDate );

    UNREFERENCED_PARAMETER( tallyCPM );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  CPM/EPT tally to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTallyCPM& tallyCPM)
{
    // --- report each element of tally --
    ReportElement( _T("PMS SEND SUCCESS"),  tallyCPM.getCpmSendOK());
    ReportElement( _T("PMS SEND ERROR"),    tallyCPM.getCpmSendError());
    ReportElement( _T("PMS RESPONSE"),      tallyCPM.getCpmResponse());
    ReportElement( _T("PMS UNSOLI-DATA"),   tallyCPM.getCpmUnsolicitedData());
    ReportElement( _T("PMS DROPPED RESP"),  tallyCPM.getCpmDroppedResponse());
    ReportElement( _T("PMS TIMEOUT"),       tallyCPM.getCpmTimeOut());
    ReportElement( _T("PMS IHC ERROR"),     tallyCPM.getCpmIHCError());
    ReportElement( _T("EPT SEND SUCCESS"),  tallyCPM.getEptSendOK());
    ReportElement( _T("EPT SEND ERROR"),    tallyCPM.getEptSendError());
    ReportElement( _T("EPT RECV SUCCESS"),  tallyCPM.getEptResponseOK());
    ReportElement( _T("EPT RECV ERROR"),    tallyCPM.getEptResponseError());
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  ETK total to report
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CTtlETK& ttlETK)
{
    // --- report ETK report header ---
    SetReportString( CString( "  " ));
    SetReportString( CString( "*** ETK REPORT ***" ));

    UNREFERENCED_PARAMETER( ttlETK );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  ETK total to report, ETK parameter to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTtlETK& ttlETK, const CParaETK& paraETK)
{
    CString str;
    TCHAR    szEmployeeName[ ETK_MAX_NAME_SIZE + 1 ]; //CHAR to TCHAR

    // --- report employee number and name ---
	memset(szEmployeeName, 0, sizeof(szEmployeeName));
    paraETK.getName( szEmployeeName );

    SetReportString( _T("\t-------------------------------------------------------") );

    str.Format( _T("\tEMPLOYEE# = %8.8lu : EMPLOYEE NAME = %s"), ttlETK.getEmployeeNo(), szEmployeeName );
    SetReportString( str );

    SetReportString( CString( "\t-------------------------------------------------------" ) );

    // --- report each element of ETK total ---
	str.Format(_T("\t\tFrom Date Time    To Date Time"));
	SetReportString( str );

	str.Format( _T("\t\t %2d/%2d  %2d:%02d  ->  %2d/%2d %2d:%02d"),
                ttlETK.getFromMonth(), ttlETK.getFromDay(), ttlETK.getFromHour(), ttlETK.getFromMinute(),
                ttlETK.getToMonth(), ttlETK.getToDay(), ttlETK.getToHour(), ttlETK.getToMinute());
    SetReportString( str );

    ReportTotal( ttlETK );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  ETK total to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTtlETK& ttlETK)
{
    USHORT  usInHour;
    USHORT  usInMinute;

    // --- get employee time in / out value ---
    usInHour   = ttlETK.getTimeInHour();
    usInMinute = ttlETK.getTimeInMinute();

    if (( usInHour == ETK_TIME_NOT_IN ) || ( usInMinute == ETK_TIME_NOT_IN ))
    {
        usInHour   = 0;
        usInMinute = 0;
    }

    USHORT  usOutHour;
    USHORT  usOutMinute;

    usOutHour   = ttlETK.getTimeOutHour();
    usOutMinute = ttlETK.getTimeOutMinute();

    if (( usOutHour == ETK_TIME_NOT_IN ) || ( usOutMinute == ETK_TIME_NOT_IN ))
    {
        usOutHour   = 0;
        usOutMinute = 0;
    }

    // --- report each element of ETK report ---
    CString str;

    str.Format( _T("\t\t\t\t\t\t\t\t\t DATE   TIME-IN\t TIME-OUT"));
    SetReportString( str );

    str.Format( _T("\t\t%s = %2d\t%s = %2d\t%2d/%2d\t%2d:%02d -> %2d:%02d"),
                _T("FIELD#"), ttlETK.getFieldNo() + 1,
                _T("JOB CODE"), ttlETK.getJobCode(),
                ttlETK.getTimeInMonth(), ttlETK.getTimeInDay(),
                usInHour, usInMinute,
                usOutHour, usOutMinute );

    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  guest check total to report, cashier# of guest check opened
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CGuestCheck& ttlGC, const ULONG ulCashierNo)
{
    ASSERT( ulCashierNo <= pcttl::MAX_CASHIER_NO );

    // --- report guest check report header ---
    SetReportString( CString( "  " ));
    SetReportString( CString( "*** GUEST CHECK REPORT ***" ));

    CString str;
    if ( 0 < ulCashierNo )
    {
        // --- report cashier# of guest check opened ---
        SetReportString( CString( " ----------------" ));

        str.Format( _T(" %s = %8.8u"), _T("OPERATOR#"), ulCashierNo );
        SetReportString( str );

        SetReportString( CString( " ----------------" ));
    }

    // --- report item caption of guest check ---
    str.Format( _T("\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s"),
                _T(" G.C.#"),
                _T("CONS#"),
                _T("  OPE #  "),
                _T("OPENED"),
                _T("TABLE#"),
                _T("PERSON"),
                _T(" OLD-OP"),
                _T("T-FROM"),
                _T("TOTAL") );
    SetReportString( str );

    UNREFERENCED_PARAMETER( ttlGC );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  guest check total to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CGuestCheck& ttlGC)
{
    CString str;

    // --- report each element of guest check total ---
    str.Format( _T("\t%4u%02u\t%4u\t%8.8u\t%2u:%02u\t%u\t%8u\t%8.8u\t%4ld\t%4ld"),
                ttlGC.getGuestCheckNo(),
                ttlGC.getCheckDigit(),
                ttlGC.getConsNo(),
                ttlGC.getCashierNo(),
                ttlGC.getCheckOpenHour(),
                ttlGC.getCheckOpenMinute(),
                ttlGC.getTableNo(),
                ttlGC.getNoOfPerson(),
                ttlGC.getPrevCashierNo(),
                ttlGC.getTransfferedTotal(),
                ttlGC.getCurrentTotal());

    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportCloseGC()
//
//  PURPOSE :   Report message of close guest check.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  guest check# to close
//
//===========================================================================

void CPCSampleDoc::ReportCloseGC(const USHORT usGCNumber)
{
    CString str;

    // --- report message of close guest check ---
    str.Format( _T("*** %s : %s %u ***"), _T("CLOSE GUEST CHECK"), _T("GC# ="), usGCNumber );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  EJ record to report, report date and time, terminal# to report EJ 
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CReportEJ& reportEJ, const DATE& ToDate, const USHORT usTerminalNo)
{
    // --- report EJ header ---
	INT_PTR  x = SetReportSectionHeader(L"Electronic Journal Report", true);
	
	SetReportString( CString( "  " ));
    SetReportString( CString( "=========================" ));
    SetReportString( CString( "  " ));
	CString myTitle (reportEJ.m_ReportSubTitle + _T(" ELECTRONIC JOURNAL REPORT"));
    ReportElement(myTitle, ToDate );

    // --- report selected terminal# ---
    SetReportString( CString( " --------------" ));

    CString str;
    str.Format( _T(" %s = %2d"), _T("TERMINAL#"), usTerminalNo );
    SetReportString( str );

    SetReportString( CString( " --------------" ));

    UNREFERENCED_PARAMETER( reportEJ );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportEJLineData()
//
//  PURPOSE :   Report body of EJ report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportEJLineData(CReportEJ& reportEJ)
{
	TCHAR   szEJLine[ EJ_REPORT_WIDE + 5] = {0};
	BOOL    fSuccess;

	szEJLine[0] = szEJLine[1] = szEJLine[2] = szEJLine[3] = ' ';  // added to indent the EJ line 4 spaces
	while(reportEJ.getMoreEJLines())
	{
		reportEJ.getEJLine( (CHAR *)(szEJLine + 4), fSuccess);

		if(fSuccess)
		{
			SetReportString( szEJLine );
			if (psqlDbFile) {
				CSqlitedb::ElecJournalDb  myElecJournal = {0};

				myElecJournal.usConsecNo = reportEJ.getCurrentConsecutiveNumber();
				myElecJournal.usLineNo = reportEJ.m_usLineNo;
				myElecJournal.pTextBuff = szEJLine + 4;
				psqlDbFile->ReportElement (NULL, (void *)&myElecJournal);
			}
			reportEJ.m_usLineNo++;
		}
		else
		{
			ReportError( CLASS_RPTEJ, pcsample::ERR_LINEREAD, reportEJ.GetLastError());
		}
	}
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportEJ()
//
//  PURPOSE :   Report body of EJ report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  EJ record to report
//
//===========================================================================
void CPCSampleDoc::ReportEJ(CReportEJ& reportEJ)
{
	TCHAR   szEJLine[ EJ_REPORT_WIDE + 1 ] = {0};	
    BOOL    fIsEJTHeader;
	ULONG   usNoOfTotalReadLines;

    // --- report EJ records by 1 line ---
	usNoOfTotalReadLines = reportEJ.getTotalBytesRead();

    for( USHORT usI = 0; readOffset < usNoOfTotalReadLines; usI++ )
    {			
		reportEJ.getEJLine( usI, (CHAR *)szEJLine, fIsEJTHeader );

		if ( fIsEJTHeader )
		{
			ReportItemCaption( reportEJ );
		}
		else
		{
			SetReportString( szEJLine );
			if (psqlDbFile) {
				CSqlitedb::ElecJournalDb  myElecJournal = {0};

				myElecJournal.usConsecNo = reportEJ.getCurrentConsecutiveNumber();
				myElecJournal.usLineNo = reportEJ.m_usLineNo;
				myElecJournal.pTextBuff = szEJLine;
				psqlDbFile->ReportElement (NULL, (void *)&myElecJournal);
			}
			reportEJ.m_usLineNo++;
		}
    }
	if( readOffset != 0)
	{
		readOffset = 0;
	}
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportItemCaption()
//
//  PURPOSE :   Report item caption of EJ report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  EJ record to report
//
//===========================================================================

void CPCSampleDoc::ReportItemCaption(const CReportEJ& reportEJ)
{
    USHORT  usBlockSize;

    // --- get current read block size ---
    usBlockSize = reportEJ.getCurrentBlockSize();

    // --- report each element of EJ record ---
    SetReportString( CString( "    " ) );
    SetReportString( CString( "  ----------------------------------" ) );

    CString str;
    str.Format( _T("  ConsNo %4.4d Length %4d Lines %3d"), reportEJ.getCurrentConsecutiveNumber(),
                usBlockSize,
                ((usBlockSize - sizeof(EJT_HEADER)) / (EJ_COLUMN * sizeof(TCHAR)))
                /*usBlockSize / EJ_COLUMN*/ );

    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTrailer()
//
//  PURPOSE :   Report item trailer of EJ report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  EJ record to report
//
//===========================================================================

void CPCSampleDoc::ReportTrailer(const CReportEJ& reportEJ)
{
    ULONG   ulFileSize;

    // --- get total size of EJ file ---
    ulFileSize = reportEJ.getTotalReadSize();

    // --- report element of EJ file ---
	/*
		The total number of lines in the EJ file can not be 
		determined by the file size because the EJ Header is
		twenty four bytes where a line of EJ data is 48 bytes.
		The EJ header will also specify how many lines of EJ
		Data are are associated with that specific header 
		(i.e. a header file can have 1 to many lines of EJ
		data associated with it). ESMITH
	*/
    CString str;
    str.Format( _T(" %s %10ld : %s %10ld"), _T("EJ FILE SIZE ="), ulFileSize, _T("TTL LINES (INC. HEADER) ="), reportEJ.getTotalLines() );

    SetReportString( str );
    SetReportString( CString( "==================================" ) );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  department total to report
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const CTtlDept& ttlDept)
{
    // --- report header of department total ---
    CString str;

	str.Format(_T("Department Report %2.2u  %4.4u"), ttlDept.getMajorDept(), ttlDept.getDept());
	INT_PTR  x = SetReportSectionHeader(str);
	
	SetReportString( CString( "  " ));
    ReportElement( _T("DEPARTMENT"), ttlDept.getFromDate(), ttlDept.getToDate());

    // --- report item caption of department total ---
    str.Format( _T(" %s\t%-12s\t%s\t%5s\t%10s\t%10s"), _T("DEPT#"), _T("NAME"), _T("MAJOR#"), _T("TYPE"),  _T("COUNTER"), _T("TOTAL") );
    SetReportString( str );
}
//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportExHeader()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  
//
//===========================================================================
//ESMITH SR275
void CPCSampleDoc::ReportExHeader(const CTtlPlu& ttlPlu)
{
	ReportElement(_T("RECENT"), ttlPlu.getExFromDate(), ttlPlu.getExToDate());

	CString str;
	str.Format( _T(" %-14s\t%-2s\t%10s\t%10s"), _T("     PLU#"), _T("ADJ#"), _T("COUNTER#"), _T("TOTAL") );
	SetReportString(str);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotalPluEx()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//===========================================================================
//ESMITH SR275
void CPCSampleDoc::ReportTotalPluEx(const CTtlPlu& ttlPlu, const CParaPlu& paraPlu)
{
	CString str;

	for( USHORT usI = 0; usI < CLI_PLU_EX_MAX_NO; usI++)
	{
		if( ttlPlu.getAmountEx(usI) != 0 )
		{
			TCHAR uchPluNo[STD_PLU_NUMBER_LEN + 1] = {0};

			paraPlu.convertPLU(uchPluNo, ttlPlu.getPluNoEx(usI));

			str.Format( _T(" %14s\t%2d\t%10ld\t\t%16s"), uchPluNo, ttlPlu.getAdjectNoEx(usI),
						ttlPlu.getCounterEx(usI), MakeCurrencyString(ttlPlu.getAmountEx(usI)) );

			SetReportString(str);
			lReportTotalCount += ttlPlu.getCounterEx(usI);
			llReportTotalAmount += ttlPlu.getAmountEx(usI);

			if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
//				psqlDbFile->ReportElement (lpsz, d13digit);  ReportTotalPluEx
			}
		}
	}
}
//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  department total to report, department parameter to report
//
//===========================================================================

void CPCSampleDoc::ReportTotal(const CTtlDept& ttlDept, const CParaDept& paraDept)
{
	TCHAR    szDeptName[OP_DEPT_NAME_SIZE + 1] = { 0 }; //CHAR to TCHAR ESMITH

    // --- get department name and type ---
    paraDept.getName( szDeptName );

    CString strType;

    switch ( paraDept.getDeptType())
    {
    case pcpara::TYPE_MINUS:
        strType = "MINUS";
        break;

    case pcpara::TYPE_HASH:
        strType = "HASH";
        break;

    default:
        strType = " ";
        break;
    }

    CString str;



	// --- report each element of department total ---

//    str.Format( _T(" %2d\t\t%-12s\t%2d\t\t%5s\t%10ld\t%10ld"),
    str.Format( _T("  %4d\t%-12s\t\t%2d\t%5s\t%10ld\t%16s"), paraDept.getDeptNo(), szDeptName,
                paraDept.getMajorDeptNo(), strType, ttlDept.getCounter(), MakeCurrencyString(ttlDept.getAmount()));
    SetReportString( str );

	if (nSheetCol < 1) nSheetCol = 1;
	str.Format(_T("C;Y%d;X%d;K%d\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, paraDept.getDeptNo(),
		nSheetRow, nSheetCol + 1, szDeptName,
		nSheetRow, nSheetCol + 2, paraDept.getMajorDeptNo(),
		nSheetRow, nSheetCol + 3, strType,
		nSheetRow, nSheetCol + 4, ttlDept.getCounter(),
		nSheetRow, nSheetCol + 5, MakeCurrencyString(ttlDept.getAmount()));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile) {
		CSqlitedb::DeptTotalDb  myDept = {0};

		myDept.usMajorDeptNo = paraDept.getMajorDeptNo();
		myDept.usDeptNo = paraDept.getDeptNo();
		myDept.uchItemType = paraDept.getDeptType();
		myDept.DeptTotal.lCounter = ttlDept.getCounter();
		myDept.DeptTotal.lAmount = ttlDept.getAmount();
		psqlDbFile->ReportElement (NULL, (void *)&myDept);
	}

	lReportTotalCount += ttlDept.getCounter();
	llReportTotalAmount += ttlDept.getAmount();
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  PLU total to report
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CTtlPlu& ttlPlu)
{
    // --- report PLU total report header ---
	INT_PTR  x = SetReportSectionHeader(L"PLU Report");
	
	SetReportString( CString( "  " ));
    ReportElement( _T("PLU"), ttlPlu.getFromDate(), ttlPlu.getToDate());

    CString str;
    // --- report item caption of PLU total report ---
    str.Format( _T(" %s\t\t\t%s\t%-20s\t%s\t%10s\t%10s"), _T("PLU#"), _T("ADJ#"), _T("NAME"), _T("DEPT#"), _T("COUNTER"), _T("TOTAL") );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  PLU total to report, PLU parameter to report, number of PLU records in parameter
//
//===========================================================================

void CPCSampleDoc::ReportTotal( const CTtlPlu& ttlPlu, const CParaPlu& paraPlu, const USHORT usNoOfPlu)
{
    ASSERT(( 0 < usNoOfPlu ) && ( usNoOfPlu <= CLI_PLU_MAX_NO ));

    CString str;

    // --- report each element of PLU by 1 record ---
    for ( USHORT usI = 0; usI < usNoOfPlu; usI++ )
    {
        TCHAR    szPluName[ OP_PLU_NAME_SIZE + 1 ]; //CHAR to TCHAR ESMITH
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
		CHAR	szName[ OP_PLU_NAME_SIZE + 1 ];	// ADD Saratoga
#endif
        // --- get PLU name and type ---

        paraPlu.getName( szPluName, usI );
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
		// --- edit PLU name, Saratoga ---
		USHORT usPos = 0;
		int nCnt = 0;
		memset(szName, 0x00, sizeof(szName));

		for (nCnt = 0; nCnt < OP_PLU_NAME_SIZE; nCnt++)
		{
			if ((szPluName[ nCnt ] == 0x00) || (szPluName[ nCnt ] == 0x20))
			{
				usPos++;
			} else {
				break;
			}
		}
//		nCnt = OP_PLU_NAME_SIZE - strlen(&szPluName[ usPos ]);
		strcpy(szName, &szPluName[ usPos ]);
#endif
        CString strType;

        switch ( paraPlu.getPluType( usI ))
        {
        case PLU_OPEN_TYP:
            strType = "OPEN";
            break;

        case PLU_NON_ADJ_TYP:
            strType = "NON ";
            break;

        default:
            strType.Format( _T(" %2d "), paraPlu.getAdjectiveNo( usI ));
            break;
        }

		// get Plu Number, ###ADD Saratoga
		TCHAR	uchPluNo[OP_PLU_LEN + 1];

		memset(uchPluNo, 0x00, sizeof(uchPluNo));
		paraPlu.getPluNo((LPTSTR)uchPluNo, usI);

		// edit Plu number, ###ADD Saratoga
		TCHAR	uchPluTemp[OP_PLU_LEN + 1];
		memset(uchPluTemp, 0x00, sizeof(uchPluTemp));

		paraPlu.convertPLU(uchPluTemp, uchPluNo);

        // -- report element of each PLU total ---

//        str.Format( _T(" %4d\t%s\t%-12s\t%3d\t\t%10ld\t%10ld"),
//                    paraPlu.getPluNo( usI )
        str.Format( _T(" %14s\t %s\t%-20s\t %4d\t%10ld\t%16s"), uchPluTemp, strType, szPluName,
                    paraPlu.getDeptNo( usI ), ttlPlu.getCounter( usI ), MakeCurrencyString(ttlPlu.getAmount( usI )));
        SetReportString( str );

		if (nSheetCol < 1) nSheetCol = 1;
		//                uchPluTemp            strType               szPluName            DeptNo         Counter            Amount
		str.Format(_T("C;Y%d;X%d;K\"%14.14s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
			nSheetRow, nSheetCol, uchPluTemp,
			nSheetRow, nSheetCol + 1, strType,
			nSheetRow, nSheetCol + 2, szPluName,
			nSheetRow, nSheetCol + 3, paraPlu.getDeptNo(usI),
			nSheetRow, nSheetCol + 4, ttlPlu.getCounter(usI),
			nSheetRow, nSheetCol + 5, MakeCurrencyString(ttlPlu.getAmount(usI )));
		SetSpreadSheetString (str);
		nSheetRow++;

		lReportTotalCount += ttlPlu.getCounter(usI);
		llReportTotalAmount += ttlPlu.getAmount(usI);

		if (psqlDbFile) {
			CSqlitedb::PluTotalDb  myPlu = {0};

			paraPlu.getPluNo((LPTSTR)myPlu.auchPLUNumber, usI);
			myPlu.usDeptNo = paraPlu.getDeptNo(usI);
			myPlu.uchAdjectNo = paraPlu.getAdjectiveNo(usI);
			myPlu.uchItemType = paraPlu.getPluType(usI);
			myPlu.PLUTotal.lCounter = ttlPlu.getCounter(usI);
			myPlu.PLUTotal.lAmount = ttlPlu.getAmount(usI);
			psqlDbFile->ReportElement (NULL, (void *)&myPlu);
		}
    }
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  coupon total to report
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CTtlCoupon& ttlCoupon)
{
    // --- report coupon total report header ---
	INT_PTR  x = SetReportSectionHeader(L"Coupon Report");
	
	SetReportString( CString( "  " ));
    ReportElement( _T("COUPON"), ttlCoupon.getFromDate(), ttlCoupon.getToDate());

    CString str;
    // --- report item caption of coupon total report ---
    str.Format( _T(" %s\t%-12s\t%7s\t%10s"), _T("COUPON#"), _T("NAME"), _T("COUNTER"), _T("TOTAL") );
    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K\"%s\"\r\n"),
		nSheetRow, nSheetCol, _T("COUPON#"),
		nSheetRow, nSheetCol + 1, _T("NAME"),
		nSheetRow, nSheetCol + 2, _T("COUNTER"),
		nSheetRow, nSheetCol + 3, _T("TOTAL"));
	SetSpreadSheetString (str);
	nSheetRow++;
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  coupon total to report, coupon parameter to report
//
//===========================================================================
void CPCSampleDoc::ReportTotal(const CTtlCoupon& ttlCoupon, const CParaCoupon& paraCoupon )
{
    TCHAR    szCouponName[ OP_CPN_NAME_SIZE + 1 ]; //CHAR to TCHAR ESMITH

    // --- get coupon name ---
	memset(&szCouponName, 0x00, sizeof(szCouponName));
    paraCoupon.getName( szCouponName );

    CString str;
    // --- report each elements of coupon total ---
    str.Format( _T(" \t%2d\t\t%-12s\t%7d\t%16s"), paraCoupon.getCouponNo(), szCouponName, ttlCoupon.getCounter(), MakeCurrencyString(ttlCoupon.getAmount()));
    SetReportString( str );
	str.Format(_T("C;Y%d;X%d;K%d\r\nC;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, paraCoupon.getCouponNo(),
		nSheetRow, nSheetCol + 1, szCouponName,
		nSheetRow, nSheetCol + 2, ttlCoupon.getCounter(),
		nSheetRow, nSheetCol + 3, MakeCurrencyString(ttlCoupon.getAmount()));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile) {
		CSqlitedb::CouponTotalDb  myCoupon = {0};

		myCoupon.usCpnNumber = paraCoupon.getCouponNo();
		for (int ij = 0; ij < STD_CPNNAME_LEN; ij++) myCoupon.auchMnemo[ij] = szCouponName[ij];
		myCoupon.CpnTotal.lCounter = ttlCoupon.getCounter();
		myCoupon.CpnTotal.lAmount = ttlCoupon.getAmount();
		psqlDbFile->ReportElement (NULL, (void *)&myCoupon);
	}
}


//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of Saved Totals report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportHeader(const DEPTTBL myDeptTbl)
{
	// --- report department total report header ---
	INT_PTR  x = SetReportSectionHeader(L"  Department Report");

    // --- report coupon total report header ---
    CString str;

    // --- report item caption of coupon total report ---
	SetReportString(_T("   "));         // empty line
	str.Format( _T("  Department Totals"));
    SetReportString( str );
	str.Format(_T(" %s\t%-12s\t%7s\t%10s"), _T("  Dept"), _T("   Name"), _T("Counter"), _T("Total"));
	SetReportString(str);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportTotal(const DEPTTBL myDeptTbl)
{
	CString str;
    // --- report each elements of coupon total ---
    str.Format( _T(" \t%2d\t\t%-12s\t%7d\t%10s"), myDeptTbl.usDEPT, myDeptTbl.auchMnemo, myDeptTbl.DEPTTotal.lCounter, MakeCurrencyString(myDeptTbl.DEPTTotal.lAmount));
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of Saved Totals report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportHeader(const CPNTBL myCPNTbl)
{
	// --- report coupon total report header ---
	INT_PTR  x = SetReportSectionHeader(L"  Coupon Report");

	// --- report coupon total report header ---
    CString str;
    // --- report item caption of coupon total report ---
	SetReportString(_T("   "));         // empty line
	str.Format( _T("  Coupon Totals"));
    SetReportString( str );
	str.Format(_T(" %s\t%-20s\t%7s\t%10s"), _T("Coupon"), _T("    Name"), _T("Counter"), _T("Total"));
	SetReportString(str);
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportTotal()
//
//  PURPOSE :   Report body of total report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportTotal(const CPNTBL myCPNTbl)
{
    CString str;
    // --- report each elements of coupon total ---
    str.Format( _T("   %2d \t%-20s\t%7d\t%10s"), myCPNTbl.usCpnNo, myCPNTbl.auchMnemo, myCPNTbl.CPNTotal.sCounter, MakeCurrencyString(myCPNTbl.CPNTotal.lAmount));
    SetReportString( str );
}


//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportSetDateTime()
//
//  PURPOSE :   Report message of new date/time is set.
//
//  RETURN :    No return value.
//
//  DESCRIPTION:  current date/time, new date/time
//
//===========================================================================

void CPCSampleDoc::ReportSetDateTime(const CLIDATETIME& thisDate, const CLIDATETIME& newDate)
{
    CString str;
    LPCTSTR lpszWDay[] = {
        _T("Sun"),
        _T("Mon"),
        _T("Tue"),
        _T("Wed"),
        _T("Thu"),
        _T("Fri"),
        _T("Sat")
    };

    // --- report message of new date/time is set ---

    str.Format( _T("*** %s : %4d %2d/%2d (%s) %2d:%02d -> %4d %2d/%2d (%s) %2d:%02d ***"),
                _T("SET DATE/TIME"),
                thisDate.usYear, thisDate.usMonth, thisDate.usMDay, lpszWDay[ thisDate.usWDay ],
                thisDate.usHour, thisDate.usMin,
                newDate.usYear, newDate.usMonth, newDate.usMDay, lpszWDay[ newDate.usWDay ],
                newDate.usHour, newDate.usMin );

    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportSetStoreNo()
//
//  PURPOSE :   Report message of new store/terminal# is set.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportSetStoreNo(
    const USHORT usStoreNo,     // current store#
    const USHORT usRegNo,       // current register#
    const USHORT usNewStoreNo,  // new store#
    const USHORT usNewRegNo     // new register#
    )
{
    CString str;
    // --- report message of new store/terminal# is set ---
    str.Format( _T("*** %s : %s(%04u) %s(%03u) -> %s(%04u) %s(%03u) ***"), _T("SET STORE/TERMINAL#"),
                _T("STORE#"), usStoreNo,
                _T("TERM#"), usRegNo,
                _T("STORE#"), usNewStoreNo,
                _T("TERM#"), usNewRegNo );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportChangePassword()
//
//  PURPOSE :   Report message of password is changed.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportChangePassword()
{
    CString str;
    // --- report message of password is changed ---
    str.Format( _T("*** %s : %s ***"), _T("CHANGE PASSWORD"), _T("SUCCESS") );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportLogOn()
//
//  PURPOSE :   Report message of logged on terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportLogOn(
    const USHORT usTerminalNo   // terminal# to logged on
    )
{
    CString str;
    // --- report message of logged on terminal ---
    str.Format( _T("*** %s (%s) : TERMINAL# = %d ***"), _T("LOG ON "), _T("SUCCESS"), usTerminalNo );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportLogOff()
//
//  PURPOSE :   Report message of logged off terminal.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportLogOff( const USHORT usTerminalNo )
{
    CString str;
    // --- report message of logged off terminal ---
    str.Format( _T("*** %s (%s) : TERMINAL# = %d ***"), _T("LOG OFF"), _T("SUCCESS"), usTerminalNo );
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportLockKbd()
//
//  PURPOSE :   Report message of keyboard is locked.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportLockKbd( const BOOL fLocked )
{
    CString str;

    if ( fLocked )
    {
        str.Format( _T("*** %s %s ***"), _T("KEYBOARD :"), _T("LOCKED") );
    }
    else
    {
        str.Format( _T("*** %s %s ***"), _T("KEYBOARD :"), _T("UNLOCKED") );
    }
    SetReportString( str );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportHeader()
//
//  PURPOSE :   Report header of terminal report.
//
//  RETURN :    No return value.
//
//===========================================================================
void CPCSampleDoc::ReportHeader(
    const CTerminal& Terminal,  // terminal object to report
    const DATE& thisDate        // report date and time
    )
{
    // --- report terminal configuration report header ---
    SetReportString( CString( "  " ));
    ReportElement( _T("SYSTEM CONFIGURATION"), thisDate );

    // --- report selected terminal# ---
    SetReportString( CString( " --------------" ));

    CString str;
    str.Format( _T(" %s = %2d"), _T("TERMINAL#"), Terminal.getTerminalNo());
    SetReportString( str );

    SetReportString( CString( " --------------" ));
}

//===========================================================================
//
//  FUNCTION :  CPCSampleDoc::ReportConfig()
//
//  PURPOSE :   Report body of terminal report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleDoc::ReportConfig(
    const CTerminal& Terminal   // terminal object to report
    )
{
    using namespace pcterm;

    CString str;
    USHORT  usI;

    // --- report each peripheral configuration ---

    SetReportString( CString( "\tPERIPHERALs" ));

    for ( usI = 1; usI < CLI_LEN_PORT; usI++ )
    {

		TCHAR    szDeviceName[ MAX_DEVICENAME_LEN + 1 ]; //CHAR to TCHAR ESMITH
		Terminal.getDeviceName(usI, szDeviceName);

		if( !(_tcscmp(szDeviceName,_T("")))) {
			_tcscpy(szDeviceName, _T("Not Used"));
		}

        str.Format( _T("\t\t%s%1d : %-s,%5d,"), _T("PORT"), usI, szDeviceName, Terminal.getBaudRate( usI ));
        switch ( Terminal.getDataBits( usI ))
        {
        case DATA_BITS_8:
            str += " 8,";
            break;
        case DATA_BITS_7:
            str += " 7,";
            break;
        default:
            str += " ?,";
            break;
        }

        switch ( Terminal.getParityBits( usI ))
        {
        case PARITY_BITS_EVEN:
            str += " EVEN,";
            break;
        case PARITY_BITS_ODD:
            str += " ODD ,";
            break;
        default:
            str += " NONE,";
            break;
        }

        switch ( Terminal.getStopBits( usI ))
        {
        case STOP_BITS_2:
            str += " 2";
            break;
        default:
            str += " 1";
            break;
        }

        SetReportString( str );
    }
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
	// DEL Saratoga
    // --- report each options ---

    SetReportString( CString( _T("\tOPTIONs" ));

    for ( usI = 0; usI < CLI_LEN_OPTION; usI++ )
    {
        str.Format( _T("\t\t%s%1d : %04d"),
                    _T("OP"),
                    usI + 1,
                    Terminal.getOption( usI ));
        SetReportString( str );
    }

    // --- report each protocol converter configurations ---
    for ( usI = 0; usI < MAX_REMOTE_NO; usI++ )
    {
        str.Format( _T("\t%s%1d"),
                    _T("PROTOCOL CONVERTER#"),
                    usI + 1 );
        SetReportString( str );

        for ( USHORT usPortNo = 0; usPortNo < CLI_LEN_RMT_PORT; usPortNo++ )
        {
            str.Format( _T("\t\t%s%1d : %02d"),
                        _T("PORT"),
                        usPortNo + 1,
                        Terminal.getRemotePeripheral( usI, usPortNo ));
            SetReportString( str );
        }
    }

    // --- report load software parameters ----

    SetReportString( CString( "\tLOAD PARAMETERS" ));

    str.Format( _T("\t\t%s : "), _T("LOAD DEVICE") );
    switch ( Terminal.getLoadDevice())
    {
    case LOAD_DEVICE_COM:
        str += "RS232C";
        break;
    case LOAD_DEVICE_MODEM:
        str += "MODEM";
        break;
    default:
        str += "?";
        break;
    }
    SetReportString( str );
    str.Format( _T("\t\t%s%1d : %5u"),
                _T("PORT"),
                Terminal.getLoadPortNo(),
                Terminal.getLoadBaudRate());
    SetReportString( str );
#endif

    // --- report system and application information ---

    SetReportString( CString( "\tCONFIGURATION" ));

    TCHAR    szSystemID[ MAX_SYSTEMID_LEN + 1 ]; //CHAR to TCHAR ESMITH
    str.Format( _T("\t\t%s :\t%s"), _T("SYSTEM ID"), Terminal.getSystemID( szSystemID ));
    SetReportString( str );

    TCHAR    szApplID[ CLI_LEN_SOFTWARE_ID + 1 ]; //CHAR to TCHAR ESMITH
    str.Format( _T("\t\t%s :\t%s"), _T("APPL. ID"), Terminal.getApplicationID( szApplID ));
    SetReportString( str );

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
	// DEL Saratoga
    // --- report ROM/RAM information ---

    for ( usI = 0; usI < MAX_ROM_NO; usI++ )
    {
        str.Format( _T("\t\t%s%1d : %4u%s"),
                    _T("ROM"),
                    usI + 1,
                    Terminal.getROMSize( usI ),
                    "KB" );
        SetReportString( str );
    }

    for ( usI = 0; usI < MAX_RAM_NO; usI++ )
    {
        str.Format( _T("\t\t%s%1d : %4u%s"),
                    _T("RAM"),
                    usI + 1,
                    Terminal.getRAMSize( usI ),
                    _T("KB") );
        SetReportString( str );
    }
#endif

    // --- report RS232 and IHC configurations ---

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    str.Format( _T("\t\t%s :\t"), _T("RS232 BOARD") );
    switch ( Terminal.getRS232Board())
    {
    case RS232_PROVIDED:
        str += "PRESENT";
        break;
    case RS232_NONE:
        str += "NONE";
        break;
    default:
        str += "?";
        break;
    }
    SetReportString( str );

    str.Format( _T("\t\t%s :\t\t"), _T("IHC BOARD") );
    switch ( Terminal.getIHCBoard())
    {
    case IHC_PROVIDED:
        str += "2172 IHC";
        break;
    case IHC_NCR_DLC:
        str += "NCR DLC";
        break;
    case IHC_NONE:
        str += "NONE";
        break;
    default:
        str += "?";
        break;
    }
    SetReportString( str );

    // --- report POS device configurations ---

    str.Format( _T("\t\t%s :\t"), _T("R/J PRINTER") );
    switch ( Terminal.getPrinter())
    {
    case DEVICE_RJ_PRINTER:
    case DEVICE_THERMAL_PRINTER:
        str += "PRESENT";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

    str.Format( _T("\t\t%s :\t"), _T("SLIP PRINTER") );
    switch ( Terminal.getSlipPrinter())
    {
    case DEVICE_SLIP_PRINTER:
        str += "PRESENT";
        break;
    case DEVICE_ERROR:
        str += "ERROR";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );
#endif

    str.Format( _T("\t\t%s :\t"), _T("OPER. DISPLAY") );
    switch ( Terminal.getOperatorDisplay())
    {
    case DISP_2X20:
        str += "2x20";
        break;
    case DISP_10N10D:
        str += "10N10D";
        break;
    case DISP_LCD:
        str += "LCD";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

    str.Format( _T("\t\t%s :\t"), _T("CUST. DISPLAY") );
    switch ( Terminal.getCustomerDisplay())
    {
    case DISP_2X20:
        str += "2x20";
        break;
    case DISP_10N10D:
        str += "10N10D";
        break;
    case DISP_LCD:
        str += "LCD";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

    str.Format( _T("\t\t%s :\t\t"), _T("KEYBOARD") );
    switch ( Terminal.getKeyboard())
    {
    case KEYBOARD_CONVENTION:
        str += "CONVENTIONAL";
        break;
    case KEYBOARD_MICRO:
        str += "MICROMOTION";
        break;
    case KEYBOARD_WEDGE_68:
        str += "5932 68 KEY";
        break;
    case KEYBOARD_PCSTD_102:
        str += "PC 102 KEY";
        break;
    case KEYBOARD_TOUCH:
        str += "TOUCHSCREEN";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    str.Format( _T("\t\t%s :\t"), _T("OPERATOR LOCK") );
    switch ( Terminal.getOperatorLock())
    {
    case WAITER_PEN:
        str += "PEN BOARD";
        break;
    case WAITER_LOCK:
        str += "LOCK BOARD";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

    str.Format( _T("\t\t%s :\t\t"), _T("INTEG MSR") );
    switch ( Terminal.getMSR())
    {
    case IMSR_TRACK12:
        str += "TRACK 1&&2";
        break;
    case IMSR_TRACK2:
        str += "TRACK 2";
        break;
    case IMSR_TRACK2J:
        str += "TRACK 2&3";
        break;
    default:
        str += "NONE";
        break;
    }
    SetReportString( str );

    for ( usI = 0; usI < MAX_DRAWER_NO; usI++ )
    {
        str.Format( _T("\t\t%s%1d :\t\t"),
                    _T("DRAWER"),
                    usI + 1 );
        switch ( Terminal.getDrawer( usI ))
        {
        case DRAWER_PROVIDED:
            str += "PRESENT";
            break;
        default:
            str += "NONE";
            break;
        }
        SetReportString( str );
    }
#endif

	SetReportString( CString( "*** END OF CONFIGURATION ***" ));
}

// A-KSo - Single line only
void CPCSampleDoc::DisplayText(LPCTSTR lpszFormat, va_list argList)
{
	CString strText;
	strText.Format(lpszFormat, argList);
	this->SetReportString(strText);
}


////////////////// END OF FILE ( PCSampleDoc.cpp ) //////////////////////////
