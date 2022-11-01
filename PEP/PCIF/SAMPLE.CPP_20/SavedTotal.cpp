// SavedTotal.cpp : implementation file
//

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

#include <R20_PIF.H>

#include "PCSample.h"
#include "PCSampleDoc.h"
#include "MainFrm.h"
#include "SavedTotal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSavedTotal

CSavedTotal::CSavedTotal(class CPCSampleDoc* const pDocument):
        m_pDocument( pDocument )
{
	m_ulMyPLUCounter = 0;
	memset(&m_ttlHourly,0,sizeof(m_ttlHourly));
	memset(&m_ttlService,0,sizeof(m_ttlService));
	memset(&m_ttlCashier,0,sizeof(m_ttlCashier));
	memset(&m_ttlRegFinTerminal,0,sizeof(m_ttlRegFinTerminal));
	memset(&m_ttlRegFinTotal,0,sizeof(m_ttlRegFinTotal));
	memset(&m_ttlDept,0,sizeof(m_ttlDept));
	memset(&m_ttlCpn,0,sizeof(m_ttlCpn));
	memset(&m_ttlPLU,0,sizeof(m_ttlPLU));
}

CSavedTotal::~CSavedTotal()
{
}

// This function reads various sections from a SavedTotals file stored in the
// SavedTotals folder of a GenPOS terminal when an End of Day is done.
// The function in GenPOS which does the writing of the file is TtlWriteSavFile()
// and the reading of the file here needs to correspond with the writing of the
// SavedTotals file there in GenPOS.
//
// See also handler for ID_READ_SAV_TOTAL message, void CMainFrame::OnReadSavTotal();
BOOL CSavedTotal::Read(const UCHAR uchClassToRead)
{
	BOOL    bRet = TRUE;
	TTLPLU  myPlu;

   switch(uchClassToRead) {

	case CLASS_TTLHOURLY:
		m_mySaveTotalFile.Seek(m_myIndex.ulHourlyTotalOffset,0);
		m_mySaveTotalFile.Read(&m_ttlHourly,sizeof(m_ttlHourly));
		break;

	case CLASS_TTLEODRESET:         /* Reset EOD Total File */
		m_mySaveTotalFile.Seek(m_myIndex.ulFinancialTotalOffset,0);
		m_mySaveTotalFile.Read(&m_ttlRegFinTotal,sizeof(m_ttlRegFinTotal));
        break;
   
    case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
		m_mySaveTotalFile.Seek(m_myIndex.ulCouponTotalOffset, 0);
		m_mySaveTotalFile.Read(&m_ttlCpn, sizeof(m_ttlCpn));
        break;
    
    case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
		for(int i = 0; i < STD_NUM_OF_CASHIERS; i++)
		{
			if(m_myIndex.ulCashierTotalOffset[i])
			{
				m_mySaveTotalFile.Seek(m_myIndex.ulCashierTotalOffset[i],0);
				m_mySaveTotalFile.Read(&m_ttlCashier[i], sizeof(m_ttlCashier[i]));
			}
		}
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Reset Service Time File */
		m_mySaveTotalFile.Seek(m_myIndex.ulServiceTotalOffset,0);
		m_mySaveTotalFile.Read(&m_ttlService,sizeof(m_ttlService));
        break;


    case CLASS_TTLREGFIN:           /* Reset Individual Financial File */
		for(int i = 0; i < STD_NUM_OF_TERMINALS; i++)
		{
			m_mySaveTotalFile.Seek(m_myIndex.ulIndTerminalOffset[i],0);
			m_mySaveTotalFile.Read(&m_ttlRegFinTerminal[i], sizeof(m_ttlRegFinTerminal[i]));
		}

        break;

	case CLASS_TTLDEPT:
		for(int i = 0; i < FLEX_DEPT_MAX; i++)
		{
			DEPTTBL tmpTbl = { 0 };
			m_mySaveTotalFile.Seek((m_myIndex.ulDeptTotalOffset + (sizeof(DEPTTBL) * i)), 0);
			m_mySaveTotalFile.Read(&tmpTbl, sizeof(DEPTTBL));
			if(tmpTbl.usDEPT)
			{
				m_ttlDept[i] = tmpTbl;
			}
		}
		break;

	case CLASS_TTLPLU:
		if (m_myIndex.ulPLUTotalCounter > 0 && m_ulMyPLUCounter < m_myIndex.ulPLUTotalCounter) {
			int iIndex = 0;

			m_ttlPLU.m_First = (m_ulMyPLUCounter == 0);
			memset (m_ttlPLU.m_ttlPluEx.Plu, 0, sizeof(m_ttlPLU.m_ttlPluEx.Plu));

			while( (m_ulMyPLUCounter < m_myIndex.ulPLUTotalCounter) &&
					(iIndex < sizeof(m_ttlPLU.m_ttlPluEx.Plu)/sizeof(m_ttlPLU.m_ttlPluEx.Plu[0])))
			{
				m_mySaveTotalFile.Seek(m_myIndex.ulPluTotalBeginOffset + (m_ulMyPLUCounter * sizeof(TTLPLU)), 0);
				m_mySaveTotalFile.Read(&myPlu, sizeof(myPlu));
				memcpy(m_ttlPLU.m_ttlPluEx.Plu[iIndex].auchPLUNumber, myPlu.auchPLUNumber, sizeof(myPlu.auchPLUNumber));
				m_ttlPLU.m_ttlPluEx.Plu[iIndex].uchAdjectNo = myPlu.uchAdjectNo;
				m_ttlPLU.m_ttlPluEx.Plu[iIndex].PLUTotal = myPlu.PLUTotal;
				m_ulMyPLUCounter++;
				iIndex++;
			}
		}
		else {
			bRet = FALSE;   // indicate nothing to report on this class.
		}
		break;

    /* ===== New Functions (Release 3.1) END ===== */
    default:
		bRet = FALSE;   // indicate nothing to report on this class.
        break;
    }

	return bRet;
}

// This function writes various sections into a SavedTotals file when an End of Day is done.
//
// See also handler for ID_READ_SAV_TOTAL message, void CMainFrame::OnReadSavTotal();
BOOL CSavedTotal::Write(const UCHAR uchClassToRead)
{
	BOOL    bRet = TRUE;
	TTLPLU  myPlu;

	switch (uchClassToRead) {

	case CLASS_TTLHOURLY:
		m_mySaveTotalFile.Seek(m_myIndex.ulHourlyTotalOffset, 0);
		m_mySaveTotalFile.Write(&m_ttlHourly, sizeof(m_ttlHourly));
		break;

	case CLASS_TTLEODRESET:         /* Reset EOD Total File */
		m_mySaveTotalFile.Seek(m_myIndex.ulFinancialTotalOffset, 0);
		m_mySaveTotalFile.Write(&m_ttlRegFinTotal, sizeof(m_ttlRegFinTotal));
		break;

	case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
		m_mySaveTotalFile.Seek(m_myIndex.ulCouponTotalOffset, 0);
		m_mySaveTotalFile.Write(&m_ttlCpn, sizeof(m_ttlCpn));
		break;

	case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
		for (int i = 0; i < STD_NUM_OF_CASHIERS; i++)
		{
			if (m_myIndex.ulCashierTotalOffset[i])
			{
				m_mySaveTotalFile.Seek(m_myIndex.ulCashierTotalOffset[i], 0);
				m_mySaveTotalFile.Write(&m_ttlCashier[i], sizeof(m_ttlCashier[i]));
			}
		}
		break;

		/* ===== New Functions (Release 3.1) BEGIN ===== */
	case CLASS_TTLSERVICE:          /* Reset Service Time File */
		m_mySaveTotalFile.Seek(m_myIndex.ulServiceTotalOffset, 0);
		m_mySaveTotalFile.Write(&m_ttlService, sizeof(m_ttlService));
		break;


	case CLASS_TTLREGFIN:           /* Reset Individual Financial File */
		for (int i = 0; i < STD_NUM_OF_TERMINALS; i++)
		{
			m_mySaveTotalFile.Seek(m_myIndex.ulIndTerminalOffset[i], 0);
			m_mySaveTotalFile.Write(&m_ttlRegFinTerminal[i], sizeof(m_ttlRegFinTerminal[i]));
		}

		break;

	case CLASS_TTLDEPT:
		for (int i = 0; i < FLEX_DEPT_MAX; i++)
		{
			DEPTTBL tmpTbl = { 0 };
			m_mySaveTotalFile.Seek((m_myIndex.ulDeptTotalOffset + (sizeof(DEPTTBL) * i)), 0);
			m_mySaveTotalFile.Write(&tmpTbl, sizeof(DEPTTBL));
			if (tmpTbl.usDEPT)
			{
				m_ttlDept[i] = tmpTbl;
			}
		}
		break;

	case CLASS_TTLPLU:
		if (m_myIndex.ulPLUTotalCounter > 0 && m_ulMyPLUCounter < m_myIndex.ulPLUTotalCounter) {
			int iIndex = 0;

			m_ttlPLU.m_First = (m_ulMyPLUCounter == 0);
			memset(m_ttlPLU.m_ttlPluEx.Plu, 0, sizeof(m_ttlPLU.m_ttlPluEx.Plu));

			while ((m_ulMyPLUCounter < m_myIndex.ulPLUTotalCounter) &&
				(iIndex < sizeof(m_ttlPLU.m_ttlPluEx.Plu) / sizeof(m_ttlPLU.m_ttlPluEx.Plu[0])))
			{
				memcpy(myPlu.auchPLUNumber, m_ttlPLU.m_ttlPluEx.Plu[iIndex].auchPLUNumber, sizeof(myPlu.auchPLUNumber));
				myPlu.uchAdjectNo = m_ttlPLU.m_ttlPluEx.Plu[iIndex].uchAdjectNo;
				myPlu.PLUTotal = m_ttlPLU.m_ttlPluEx.Plu[iIndex].PLUTotal;
				m_mySaveTotalFile.Seek(m_myIndex.ulPluTotalBeginOffset + (m_ulMyPLUCounter * sizeof(TTLPLU)), 0);
				m_mySaveTotalFile.Write(&myPlu, sizeof(myPlu));
				m_ulMyPLUCounter++;
				iIndex++;
			}
		}
		else {
			bRet = FALSE;   // indicate nothing to report on this class.
		}
		break;

		/* ===== New Functions (Release 3.1) END ===== */
	default:
		bRet = FALSE;   // indicate nothing to report on this class.
		break;
	}

	return bRet;
}

BOOL CSavedTotal::Write(CTtlRegFin& ttlRegFin)
{
	BOOL  bRet = TRUE;
	int   i = ttlRegFin.m_ttlRegFin.usTerminalNumber;

	m_mySaveTotalFile.Seek(m_myIndex.ulFinancialTotalOffset, 0);
	m_mySaveTotalFile.Write(&ttlRegFin.m_ttlRegFin.uchResetStatus, sizeof(TTLCSREGFIN));

	return bRet;
}

BOOL CSavedTotal::Write(CTtlIndFin& ttlIndFin)
{
	BOOL  bRet = TRUE;
	int   i = ttlIndFin.m_ttlRegFin.usTerminalNumber;

	m_mySaveTotalFile.Seek(m_myIndex.ulIndTerminalOffset[i], 0);
	m_mySaveTotalFile.Write(&ttlIndFin.m_ttlRegFin.uchResetStatus, sizeof(TTLCSREGFIN));

	return bRet;
}

BOOL CSavedTotal::Write(CTtlCashier & ttlCashier, int iIndex)
{
	BOOL  bRet = TRUE;

	m_mySaveTotalFile.Seek(m_myIndex.ulCashierTotalOffset[iIndex], 0);
	m_mySaveTotalFile.Write(&ttlCashier.m_ttlCashier, sizeof(TTLCASHIER));

	return bRet;
}

BOOL CSavedTotal::Write(DEPTTBL & ttlDept, int iIndex)
{
	BOOL  bRet = TRUE;

	m_mySaveTotalFile.Seek((m_myIndex.ulDeptTotalOffset + (iIndex * sizeof(DEPTTBL))), 0);
	m_mySaveTotalFile.Write(&ttlDept, sizeof(DEPTTBL));

	return bRet;
}

BOOL CSavedTotal::Write(TTLPLU & ttlPlu)
{
	BOOL  bRet = TRUE;

	m_mySaveTotalFile.Seek(m_myIndex.ulPluTotalBeginOffset + (m_myIndex.ulPLUTotalCounter * sizeof(TTLPLU)), 0);
	m_mySaveTotalFile.Write(&ttlPlu, sizeof(TTLPLU));
	m_myIndex.ulPLUTotalCounter++;

	return bRet;
}

BOOL CSavedTotal::Write(const CPNTBL  & ttlCoupon, USHORT usCouponNo)
{
	BOOL  bRet = TRUE;
	int   iIndex = ttlCoupon.usCpnNo - 1;

	m_mySaveTotalFile.Seek(m_myIndex.ulCouponTotalOffset + (iIndex * sizeof(CPNTBL)), 0);
	m_mySaveTotalFile.Write(&ttlCoupon, sizeof(CPNTBL));

	return bRet;
}

BOOL CSavedTotal::Write(const TTLCSHOURLY &ttlHourly)
{
	BOOL  bRet = TRUE;

	m_mySaveTotalFile.Seek(m_myIndex.ulHourlyTotalOffset, 0);
	m_mySaveTotalFile.Write(&ttlHourly, sizeof(ttlHourly));

	return bRet;
}

BOOL CSavedTotal::WriteIndex(void)
{
	BOOL  bRet = TRUE;

	m_mySaveTotalFile.Seek(0, 0);
	m_mySaveTotalFile.Write(&m_myIndex, sizeof(m_myIndex));

	return bRet;
}

BOOL CSavedTotal::Open(LPCTSTR cstFileName)
{
	CFileException E;
	LPVOID lpMsgBuf;

	if(!m_mySaveTotalFile.Open(cstFileName, CFile::modeRead, &E)){
		
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// look at (LPTSTR)lpMsgBuf to see error message

		return FALSE;
	}

	m_mySaveTotalFileName = cstFileName;
	m_mySaveTotalFile.Seek(0, 0);
	m_mySaveTotalFile.Read(&m_myIndex, sizeof(m_myIndex));

	return TRUE;
}

BOOL CSavedTotal::Create(LPCTSTR cstFileName)
{
	CFileException E;
	LPVOID lpMsgBuf;
	DATE_TIME	dt = { 0 };

	if (!m_mySaveTotalFile.Open(cstFileName, CFile::modeReadWrite | CFile::modeCreate, &E)) {

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);
		// look at (LPTSTR)lpMsgBuf to see error message

		return FALSE;
	}

	PifGetDateTime(&dt);
	m_myIndex.dtCreationDate.usMin = dt.usMin;
	m_myIndex.dtCreationDate.usHour = dt.usHour;
	m_myIndex.dtCreationDate.usMDay = dt.usMDay;
	m_myIndex.dtCreationDate.usMonth = dt.usMonth;
	m_myIndex.usYear = dt.usYear;

	// Starting with an offset after the Index information in the SavedTotals file
	// populate the header index information so that the various fields in the SavedTotals
	// file can be located.  We do this in the order they are in the SAVTTLFILEINDEX struct.
	m_myIndex.ulFileSize = sizeof(SAVTTLFILEINDEX);
	for (int i = 0; i < STD_NUM_OF_TERMINALS; i++)
	{
		m_myIndex.ulIndTerminalOffset[i] = m_myIndex.ulFileSize;
		m_myIndex.ulFileSize += sizeof(TTLCSREGFIN);
	}

	m_myIndex.ulFinancialTotalOffset = m_myIndex.ulFileSize;
	m_myIndex.ulFileSize += sizeof(TTLCSREGFIN);

	m_myIndex.ulDeptTotalOffset = m_myIndex.ulFileSize;
	// plus one to hold the RPT_EOF record indicating end of list. See function RptEODOnlyReset().
	m_myIndex.ulFileSize += (sizeof(DEPTTBL) * (FLEX_DEPT_MAX + 1));

	m_myIndex.ulHourlyTotalOffset = m_myIndex.ulFileSize;
	m_myIndex.ulFileSize += sizeof(TTLCSHOURLY);

	m_myIndex.ulServiceTotalOffset = m_myIndex.ulFileSize;
	m_myIndex.ulFileSize += sizeof(TTLCSSERTIME);
	for (int i = 0; i < FLEX_CAS_MAX; i++)
	{
		m_myIndex.ulCashierTotalOffset[i] = m_myIndex.ulFileSize;
		m_myIndex.ulFileSize += sizeof(TTLCASHIER);
	}
	m_myIndex.ulCouponTotalOffset = m_myIndex.ulFileSize;
	m_myIndex.ulFileSize += (sizeof(CPNTBL) * FLEX_CPN_MAX);

	m_myIndex.ulPluTotalBeginOffset = m_myIndex.ulFileSize;
	m_myIndex.ulPLUTotalCounter = 0;

	m_mySaveTotalFileName = cstFileName;
	m_mySaveTotalFile.Seek(0, 0);
	m_mySaveTotalFile.Write(&m_myIndex, sizeof(m_myIndex));

	return TRUE;
}

// This function displays the data from various sections from a SavedTotals file
// stored in the SavedTotals folder of a GenPOS terminal when an End of Day is done.
//
// See also handler for ID_READ_SAV_TOTAL message, void CMainFrame::OnReadSavTotal();
BOOL CSavedTotal::Report(const UCHAR uchClassToRead)
{
	CTtlIndFin		myIndFin;
	CTtlCoupon		myCoupon;
	CTtlDept		myDept;
	CTtlServTime	myServTime;
	CTtlRegFin		myRegFin;
	CTtlPlu			myTtlPlu;
	CParaPlu    paraPlu;
	static BOOL fFirst = TRUE;
	LONG			lTotalAllTotal = 0, lCounterAllCounter = 0;   //total of ALL CPNs or DEPTS JHHJ
	CParaMdcMem   mdcMem;

	switch (uchClassToRead) {

	case CLASS_TTLHOURLY:
	{
		CTtlHourly  myHourly;
		myHourly.m_ttlHourly.uchResetStatus = m_ttlHourly.uchResetStatus;
		myHourly.m_ttlHourly.FromDate = m_ttlHourly.FromDate;
		myHourly.m_ttlHourly.ToDate = m_ttlHourly.ToDate;
		memcpy(&myHourly.m_ttlHourly.Total, &m_ttlHourly.Total, sizeof(m_ttlHourly.Total));
		m_pDocument->ReportTotal(myHourly);
	}
	break;

	case CLASS_TTLEODRESET:         /* Reset EOD Total File */
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_Financial.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		mdcMem.Read();
		m_pDocument->m_discountRate.Read();
		m_pDocument->m_currencyRate.Read();
		m_pDocument->m_totalKeyData.Read();
		m_pDocument->m_transactionMnemonics.Read();

		memcpy(&myRegFin.m_ttlRegFin.uchResetStatus, &m_ttlRegFinTotal.uchResetStatus, sizeof(m_ttlRegFinTotal));
		m_pDocument->ReportHeader(myRegFin);
		m_pDocument->ReportTotal(myRegFin, mdcMem);
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			char *pHeader = "E\r\n";
			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
		}
		break;

	case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_Coupons.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		// following call to write the Coupon Report section does
		// not use any of the values provided. The CPNTBL argument
		// is used to select the correct ReportHeader() method for
		// the header of the Coupon Report section.
		m_pDocument->ReportHeader(m_ttlCpn[0]);

		for (int i = 0; i < FLEX_CPN_MAX; i++)
		{
			if (m_ttlCpn[i].CPNTotal.lAmount)
			{
				m_pDocument->ReportTotal(m_ttlCpn[i]);
				lTotalAllTotal += m_ttlCpn[i].CPNTotal.lAmount;
				lCounterAllCounter += m_ttlCpn[i].CPNTotal.sCounter;
			}
		}

		{
			CPNTBL	EndCPNTable = { 0 };

			wcscpy(EndCPNTable.auchMnemo, L"TOTAL");
			EndCPNTable.CPNTotal.lAmount = lTotalAllTotal;
			EndCPNTable.CPNTotal.sCounter = lCounterAllCounter;
			m_pDocument->ReportTotal(EndCPNTable);

		}

		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			char *pHeader = "E\r\n";
			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
		}
		break;

	case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_Cashier.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		mdcMem.Read();
		m_pDocument->m_discountRate.Read();
		m_pDocument->m_currencyRate.Read();
		m_pDocument->m_totalKeyData.Read();
		m_pDocument->m_transactionMnemonics.Read();

		for (int i = 0; i < STD_NUM_OF_CASHIERS; i++)
		{
			CTtlCashier  myCashier;

			if ((!m_ttlCashier[i].ulCashierNumber) || (!m_ttlCashier[i].lDGGtotal))
			{
				continue;
			}
			myCashier.m_ttlCashier = m_ttlCashier[i];
			m_pDocument->ReportHeader(myCashier);
			m_pDocument->ReportTotal(myCashier, mdcMem);
		}
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			char *pHeader = "E\r\n";
			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
		}
		break;

		/* ===== New Functions (Release 3.1) BEGIN ===== */
	case CLASS_TTLSERVICE:          /* Reset Service Time File */
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_ServiceTime.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		memcpy(&myServTime.m_ttlServTime.uchResetStatus, &m_ttlService.uchResetStatus, sizeof(m_ttlService));
		m_pDocument->ReportHeader(myServTime);
		m_pDocument->ReportTotal(myServTime);

		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			char *pHeader = "E\r\n";
			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
		}
		break;

	case CLASS_TTLREGFIN:           /* Reset Individual Financial File */
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_Financial.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		mdcMem.Read();
		m_pDocument->m_discountRate.Read();
		m_pDocument->m_currencyRate.Read();
		m_pDocument->m_totalKeyData.Read();
		m_pDocument->m_transactionMnemonics.Read();

		for (USHORT usI = 0; usI < STD_NUM_OF_TERMINALS; usI++) {
			if (m_ttlRegFinTerminal[usI].FromDate.usMonth || m_ttlRegFinTerminal[usI].ToDate.usMonth) {
				// struct TTLCSREGFIN does not have the beginning header info of a TTLREGFIN struct.
				// they both are the same from the uchResetStatus member which is the first member of a
				// TTLCSREGFIN struct.
				memcpy(&myIndFin.m_ttlRegFin.uchResetStatus, &m_ttlRegFinTerminal[usI], sizeof(TTLCSREGFIN));
				myIndFin.m_ttlRegFin.usTerminalNumber = usI;      // fix up  the terminal  number for this report section.
				m_pDocument->ReportHeader(myIndFin);
				m_pDocument->ReportTotal(myIndFin, mdcMem);

				if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
					char *pHeader = "E\r\n";
					if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

					if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
				}
			}
		}
		break;

	case CLASS_TTLDEPT:
		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName(m_pDocument->csSheetNamePrefix + _T("Totals_MajorDept.slk"));
			m_pDocument->hSheetFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str(L"ID;PPCSample\r\n");
			m_pDocument->SetSpreadSheetString(str);
			// --- read all of resetted total ---
			m_pDocument->nSheetCol = m_pDocument->nSheetRow = 1;
		}

		// following call to write the Department Report section does
		// not use any of the values provided. The DEPTTBL argument
		// is used to select the correct ReportHeader() method for
		// the header of the Department Report section.
		m_pDocument->ReportHeader(m_ttlDept[0]);

		for(int i = 0; i < FLEX_DEPT_MAX; i++)
		{
			if(m_ttlDept[i].DEPTTotal.lAmount)
			{
				m_pDocument->ReportTotal(m_ttlDept[i]);
				lTotalAllTotal += m_ttlDept[i].DEPTTotal.lAmount;
				lCounterAllCounter += m_ttlDept[i].DEPTTotal.lCounter;
			}
		}

		{
			DEPTTBL EndTable = { 0 };

			wcscpy(EndTable.auchMnemo, L"TOTAL");
			EndTable.DEPTTotal.lAmount = lTotalAllTotal;
			EndTable.DEPTTotal.lCounter = lCounterAllCounter;
			m_pDocument->ReportTotal(EndTable);
		}

		if (m_pDocument->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			char *pHeader = "E\r\n";
			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Write(pHeader, strlen(pHeader));

			if (!FAILED(m_pDocument->hSheetFile.m_hFile)) m_pDocument->hSheetFile.Close();
		}
		break;

	case CLASS_TTLPLU:
		if(m_ttlPLU.m_First) {
			m_pDocument->ReportExHeader(m_ttlPLU);
			m_ttlPLU.m_First = FALSE;
		}
		m_pDocument->ReportTotalPluEx(m_ttlPLU, paraPlu);
		if (m_ulMyPLUCounter >= m_myIndex.ulPLUTotalCounter) {
			CString str;

			str.Format(_T(" %14s\t%2d\t%10ld\t\t%16s"), L"TOTAL", 0,
				m_pDocument->lReportTotalCount, m_pDocument->MakeCurrencyString(m_pDocument->llReportTotalAmount));

			m_pDocument->SetReportString(str);
		}
		break;

    /* ===== New Functions (Release 3.1) END ===== */
        
    default:
        break;
    }

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSavedTotal message handlers
