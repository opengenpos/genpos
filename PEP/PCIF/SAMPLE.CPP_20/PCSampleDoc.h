/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PCSampleDoc.h
//
//  PURPOSE:    Main Document class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCSAMPLEDOC_H__70072F0A_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_PCSAMPLEDOC_H__70072F0A_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <TCHAR.H>

#include "sqlite3.h"

#include "SavedTotal.h"

typedef int CSqlitedbCallback (void *NotUsed, int argc, char **argv, char **azColName);
class CSqlitedbTime
{
public:
	CSqlitedbTime(SYSTEMTIME *pSysTime = 0);
	int SystemTimeToSqliteTime (const SYSTEMTIME *pSysTime);
	int SqliteTimeToSystemTime (SYSTEMTIME *pSysTime);

	CSqlitedbTime & operator = (LPCTSTR csVal) { for (int i = 0; i < 32 && (m_currKey[i] = csVal[i]); i++) ; m_currKey[31] = 0; return *this; }

	char m_currKey[32];      // YYYY-MM-DD HH:MM:SS.SSS[+-]HH:MM  time string format #4
};

class CSqlitedb
{
protected:
	sqlite3  *m_hDb;
	int      m_stRetCode;
	ULONG    m_ulId;
	ULONG    m_ulRecordCnt;
	CSqlitedbCallback   *m_callback;

public:
	CSqlitedbTime        m_CurrKey;
	char                *m_FormatInsert;

	enum DataValuesType {D13Digits, TotalStruct, LongVal, ShortVal, UShortVal, TendhtStruct };

	struct PluTotalDb {
		LTOTAL      PLUTotal;           /* PLU Total */
		WCHAR       auchPLUNumber[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
		USHORT      usDeptNo;
		UCHAR       uchAdjectNo;        /* Adjective Number */
		UCHAR       uchItemType;        // PLU_OPEN_TYP, PLU_NON_ADJ_TYP, or adjective number
	};

	struct DeptTotalDb {
		LTOTAL      DeptTotal;           /* PLU Total */
		WCHAR       auchMnemo[STD_PLU_NUMBER_LEN];  /* PLU Number,  Saratoga */
		USHORT      usDeptNo;
		USHORT      usMajorDeptNo;
		UCHAR       uchItemType;        // PLU_OPEN_TYP, PLU_NON_ADJ_TYP, or adjective number
	};

	struct CouponTotalDb {
		LTOTAL      CpnTotal;                         /* Coupon Total */
		WCHAR       auchMnemo[STD_CPNNAME_LEN + 1];   /* Coupon name,  Saratoga */
		USHORT      usCpnNumber;                      /* Coupon Number */
	};

	struct ElecJournalDb {
		USHORT      usConsecNo;
		USHORT      usLineNo;
		TCHAR       *pTextBuff;
	};

	struct DataValues {
		DataValuesType   iType;
		union {
			D13DIGITS  d13Digits;
			TOTAL      sTotal;
			LONG       lLong;
			SHORT      sShort;
			USHORT     uUShort;
			TENDHT     sTendht;
		} u;
	};

	CSqlitedb(CONST TCHAR *tcsDbFileName = 0);
	virtual ~CSqlitedb ();
	int OpenFile(CONST TCHAR *tcsDbFileName);
	int ExecSqlStmt(CONST char *aszSqlStmt, CSqlitedbCallback *pCallBack = 0, void *pArg = 0);
	sqlite3 *GetHandle(void) { return m_hDb; }
	CSqlitedbCallback   *SetCallBack (CSqlitedbCallback   *p) { CSqlitedbCallback   *x = m_callback; m_callback = p; return x; }
	int SystemTimeToSqliteTime (SYSTEMTIME *pSysTime, CSqlitedbTime pSqlTime);
	int SqliteTimeToSystemTime (CSqlitedbTime pSqlTime, SYSTEMTIME *pSysTime);
    virtual void    CreateDb( ) {}
	virtual void   	SetId (ULONG ulId) { m_ulId = ulId; }
	virtual void    ReportElement( LPCTSTR, const D13DIGITS& ) {}
    virtual void    ReportElement( LPCTSTR, const TOTAL& ) {}
    virtual void    ReportElement( LPCTSTR, const LTOTAL & ) {}
    virtual void    ReportElement( LPCTSTR, const LONG& ) {}
    virtual void    ReportElement( LPCTSTR, const SHORT& ) {}
    virtual void    ReportElement( LPCTSTR, const USHORT& ) {}
	virtual void	ReportElement( LPCTSTR, const TENDHT& ) {}
	virtual void	ReportElement(LPCTSTR, const TENDHT&, const ULONG ulConversionRate) {}
	virtual void	ReportElement( LPCTSTR, const void * ) {}
};

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CPCSampleDoc
//---------------------------------------------------------------------------

class CPCSampleDoc : public CDocument
{
protected: // create from serialization only
	CPCSampleDoc();
	DECLARE_DYNCREATE(CPCSampleDoc)

// Attributes
public:
	struct ListReportList {
		CString   m_SectionTitle;
		INT_PTR   m_ListOffset;
		bool      m_bTwoColumn;

		ListReportList() : m_ListOffset(0), m_bTwoColumn(false) {}
	};

// Operations
public:
    int     GetNoOfReportStrings() const;
    DWORD   GetMaxColumnOfStrings() const;
	INT_PTR SetReportSectionHeader(const CString & strReport, bool twoColumn = false);
	BOOL    MoveToReportSectionHeader(const CString & strReport);
	BOOL    MoveToReportSectionHeader(INT_PTR  ipListOffset);
	POSITION  GetReportSectionHeader(POSITION  pos, ListReportList &sectionHeader);
	BOOL    SetReportString( const CString& );
    BOOL    SetReportString( LPCTSTR lpszSource );
	BOOL    SetSpreadSheetString (const CString& strReport);

	bool    SetDecimalFromMdc(CParaMdcMem   &mdcMem);
	CString MakeCurrencyString(D13DIGITS d13Value);
	CString MakeCurrencyString(LONG lValue);
	CString MakeCurrencyString(ULONG ulValue);

    BOOL    GetReportString( CString&, const int nIndex );
    void    ReportFeedLines( const DWORD dwNoOfFeeds );
    VOID    ClearAllReport();

    void    ReportLogOn( const USHORT usTerminalNo );
    void    ReportLogOff( const USHORT usTerminalNo );
    void    ReportLockKbd( const BOOL fLocked );
    void    ReportChangePassword();
    void    ReportSetDateTime( const CLIDATETIME& r_thisDate, const CLIDATETIME& r_newDate );
    void    ReportSetStoreNo( const USHORT usStoreNo, const USHORT usRegNo, const USHORT usNewStoreNo, const USHORT usNewRegNo );
    void    ReportError(  const UCHAR, const int, const short, const ULONG ulSubItem = 0 );
    void    ReportHeader( const CTerminal& Terminal, const DATE& thisDate );
    void    ReportHeader( const CTtlRegFin& ttlRegFin );
    void    ReportHeader( const CTtlIndFin& ttlIndFin );
    void    ReportTotal( const CTtlRegFin& ttlRegFin, CParaMdcMem  & mdcMem);
    void    ReportHeader( const CTtlServTime& ttlServTime );
    void    ReportTotal( const CTtlServTime& ttlServTime );
    void    ReportConfig( const CTerminal& Terminal );
    void    ReportHeader( const CTallyCPM& tallyCPM, const DATE& ToDate );
    void    ReportTotal( const CTallyCPM& tallyCPM );
    void    ReportHeader( const CReportEJ& reportEJ, const DATE& ToDate, const USHORT usTerminalNo );
    void    ReportEJLineData(CReportEJ& reportEJ);
	void    ReportEJ( CReportEJ& reportEJ );
    void    ReportTrailer( const CReportEJ& reportEJ );
    void    ReportHeader( const CTtlCashier& ttlCashier );
    void    ReportTotal( const CTtlCashier& ttlCashier, CParaMdcMem  &mdcMem);
    void    ReportParameter( const CParaCashier& paraCashier );
    void    ReportHeader( const CTtlETK& ttlETK );
    void    ReportTotal( const CTtlETK& ttlETK, const CParaETK& paraETK );
    void    ReportTotal( const CTtlETK& ttlETK );
    void    ReportHeader( const CGuestCheck& ttlGC, const ULONG ulCashierNo = 0 );
    void    ReportTotal( const CGuestCheck& ttlGC );
    void    ReportCloseGC( const USHORT );
    void    ReportHeader( const CTtlHourly& ttlHourly );
    void    ReportTotal( const CTtlHourly& ttlHourly );
    void    ReportHeader( const CTtlDept& ttlDept );
    void    ReportTotal( const CTtlDept& ttlDept, const CParaDept& paraDept );
    void    ReportHeader( const CTtlPlu& ttlPlu );
    void    ReportTotal( const CTtlPlu& ttlPlu, const CParaPlu& paraPlu, const USHORT usNoOfPlu = 1 );
    void    ReportHeader( const CTtlCoupon& ttlCoupon );
    void    ReportTotal( const CTtlCoupon& ttlCoupon, const CParaCoupon& paraCoupon );
	void    ReportItemCaption( const CReportEJ& reportEJ );

	void	ReportExHeader( const CTtlPlu& ttlPlu );	/* SR275 */
	void	ReportTotalPluEx( const CTtlPlu& ttlPlu, const CParaPlu& paraPlu ); /* SR275 */

	void	ReportHeader ( const DEPTTBL myDeptTbl);
	void	ReportTotal	 ( const DEPTTBL myDeptTbl);

	void	ReportHeader(const CPNTBL myCPNTbl);
	void	ReportTotal(const CPNTBL myCPNTbl);

	// A-KSo
	void DisplayText(LPCTSTR lpszFormat, va_list argList);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCSampleDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPCSampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// special variables for reports
	long         lReportTotalCount;
	long long    llReportTotalAmount;

	CFile        hSheetFile;           // .sylk output file constructor for writing to .sylk file
	ULONG        nSheetRow;            // current row of .sylk output file used in creation of row/column address
	ULONG        nSheetCol;            // current column of .sylk output file used in creation of row/column address
	CString      csSheetNamePrefix;    // prefix for the .sylk output file typically with date stamp
	CSqlitedb    *psqlDbFile;
	CSavedTotal  *pSavedTotalFile;     // pointer to the Saved Total File used for saving retrieved totals during End of Day.
	ULONG        ulReportOptionsMap;
	BOOL         m_SaveResetDataToDatabase;
	CString      m_strDbFileName;

	USHORT  m_usDecPlace;              // number of decimal places for currency values, 0, 2, or 3 places
	USHORT  m_usDecSymbol;             // symbol used for the decimal place, period (.) or comma (,)

	CParaMdcMem         m_mdcMem;
	CParaDiscountRate   m_discountRate;
	CParaCurrencyRate   m_currencyRate;
	CParaTotalType      m_totalKeyData;
	CParaMnemonicTransaction  m_transactionMnemonics;

	static const ULONG  ulReportOptionsExportSylk   = 0x00000001;
	static const ULONG  ulReportOptionsExportDbFile = 0x00000002;
	static const ULONG  ulReportOptionsReportPrint  = 0x00000004;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPCSampleDoc)
	afx_msg void OnPageSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct DCOUNT {
		DCOUNT(LONG l) { lValue = l; }
		LONG  lValue;
	};

	CStringList m_listReportStr;
	CStringList m_listCurrentStr;     // Current or last report output.

	PAGESETUPDLG  m_pageSetup;
	PRINTDLG      m_printSetup;

	CList <ListReportList>  m_listReportList;

private:
    bool    ReportElement( LPCTSTR, const D13DIGITS& );
    bool    ReportElement( LPCTSTR, const TOTAL& );
#if !defined(DCURRENCY_LONGLONG)
	// if DCURRENCY is defined as a long long then it is the same as a D13DIGIT and
	// we will get compiler errors should this method be defined.
	bool    ReportElement( LPCTSTR, const DCURRENCY& );
#endif
	bool    ReportElement(LPCTSTR lpsz, const DCOUNT & lValue);
	bool    ReportElement( LPCTSTR, const SHORT& );
	bool    ReportElement( LPCTSTR, const USHORT& );
	bool    ReportElement( LPCTSTR, const DATE&, const DATE& );
	bool    ReportElement( LPCTSTR, const DATE& );
	bool    ReportElement(LPCTSTR);
// ###ADD Saratoga
	bool	ReportElement( LPCTSTR, const TENDHT& );
	bool	ReportElement(LPCTSTR, const TOTAL&, const ULONG ulConversionRate);

    DWORD   m_dwMaxColumnInReportStr;

    static  LPCWSTR m_lpszLogFileName;

    CStdioFile   m_file;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  REPORT ELEMENT FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::SetDecimalFromMdc(CParaMdcMem   &mdcMem)
{
	m_usDecPlace = 2;
	if (mdcMem.CheckMdcBit(MDC_SYSTEM3_ADR, CParaMdcMem::MdcBitC)) {
		m_usDecPlace = 0;
	}
	else if (mdcMem.CheckMdcBit(MDC_SYSTEM3_ADR, CParaMdcMem::MdcBitD)) {
		m_usDecPlace = 3;
	}
	m_usDecSymbol = '.';
	if (mdcMem.CheckMdcBit(MDC_SYSTEM3_ADR, CParaMdcMem::MdcBitC)) {
		m_usDecSymbol = ',';
	}

	return true;
}

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpsz, const D13DIGITS& d13digit)
{
    CString str;

	str.Format( _T("   %-20.20s%10s%16s"), lpsz, L"  ", MakeCurrencyString(d13digit));    // ReportElement() report format

    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, MakeCurrencyString(d13digit));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, d13digit);
	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement(), ###ADD Saratoga
//===========================================================================

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpsz, const TENDHT& tendht)
{
    CString str;

	str.Format( _T("   %-20.20s%10d%16s%16s"), lpsz, tendht.Total.sCounter, MakeCurrencyString(tendht.Total.lAmount), MakeCurrencyString(tendht.lHandTotal) );    // ReportElement() report format
    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, tendht.Total.sCounter,
		nSheetRow, nSheetCol + 2, MakeCurrencyString(tendht.Total.lAmount),
		nSheetRow, nSheetCol + 3, MakeCurrencyString(tendht.lHandTotal));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, tendht);
	}

	return true;
}

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpsz, const TOTAL& totalTend, const ULONG ulConversionRate)
{
	DCURRENCY   lConvAmount = totalTend.lAmount;

	CString str;
	str.Format(_T("   %-20.20s%10d%16s%16s"), lpsz, totalTend.sCounter, MakeCurrencyString(lConvAmount), MakeCurrencyString(totalTend.lAmount));    // ReportElement() report format
	SetReportString(str);

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, totalTend.sCounter,
		nSheetRow, nSheetCol + 2, MakeCurrencyString(lConvAmount),
		nSheetRow, nSheetCol + 3, MakeCurrencyString(totalTend.lAmount));
	SetSpreadSheetString(str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement(lpsz, totalTend);
	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::ReportElement( LPCTSTR lpsz, const TOTAL& total )
{
    CString str;

	str.Format( _T("   %-20.20s%10d%16s"), lpsz, total.sCounter, MakeCurrencyString(total.lAmount) );    // ReportElement() report format
    SetReportString( str );

	if (nSheetCol < 1) nSheetCol = 1;
	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, total.sCounter,
		nSheetRow, nSheetCol + 2, MakeCurrencyString(total.lAmount));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, total);
	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

#if !defined(DCURRENCY_LONGLONG)
// if DCURRENCY is defined as a long long then it is the same as a D13DIGIT and
// we will get compiler errors should this method be defined.
inline bool CPCSampleDoc::ReportElement( LPCTSTR lpsz, const DCURRENCY& lValue )
{
    CString str;
    str.Format( _T("   %-20.20s%10s%16s"), lpsz, L"    ", MakeCurrencyString(lValue) );    // ReportElement() report format
    SetReportString( str );

	if (nSheetCol < 1) nSheetCol = 1;
	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%s\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, 0,
		nSheetRow, nSheetCol + 2, MakeCurrencyString(lValue));
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, lValue);
	}

	return true;
}
#endif

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpsz, const DCOUNT & lValue)
{
	CString str;
	str.Format(_T("   %-20.20s%10ld"), lpsz, lValue.lValue);    // ReportElement() report format
	SetReportString(str);

	if (nSheetCol < 1) nSheetCol = 1;
	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\nC;Y%d;X%d;K%d\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, 0,
		nSheetRow, nSheetCol + 2, lValue.lValue);
	SetSpreadSheetString(str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement(lpsz, lValue.lValue);
	}

	return true;
}

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpsz)
{
	CString str;
	str.Format(_T("   %-20.20s"), lpsz);
	SetReportString(str);

	if (nSheetCol < 1) nSheetCol = 1;
	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\n"),	nSheetRow, nSheetCol, lpsz);
	SetSpreadSheetString(str);
	nSheetRow++;

//	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
//		psqlDbFile->ReportElement(lpsz, lValue);
//	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::ReportElement( LPCTSTR lpsz, const SHORT& sValue )
{
    CString str;
    str.Format( _T("   %-20.20s%10d"), lpsz, sValue );    // ReportElement() report format
    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, sValue);
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, sValue);
	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::ReportElement( LPCTSTR lpsz, const USHORT& usValue )
{
    CString str;
    str.Format( _T("   %-20.20s%10u"), lpsz, usValue );
    SetReportString( str );

	str.Format(_T("C;Y%d;X%d;K\"%s\"\r\nC;Y%d;X%d;K%d\r\n"),
		nSheetRow, nSheetCol, lpsz,
		nSheetRow, nSheetCol + 1, usValue);
	SetSpreadSheetString (str);
	nSheetRow++;

	if (psqlDbFile && (ulReportOptionsMap & ulReportOptionsExportDbFile) != 0) {
		psqlDbFile->ReportElement (lpsz, usValue);
	}

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpszCaption, const DATE& FromDate, const DATE& ToDate )
{
    CString strHeader;

    strHeader.Format( _T("*** %s REPORT *** %2d/%2d %2d:%02d -> %2d/%2d %2d:%02d"),
                      lpszCaption,
                      FromDate.usMonth,
                      FromDate.usMDay,
                      FromDate.usHour,
                      FromDate.usMin,
                      ToDate.usMonth,
                      ToDate.usMDay,
                      ToDate.usHour,
                      ToDate.usMin );

    SetReportString( strHeader );

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ReportElement()
//===========================================================================

inline bool CPCSampleDoc::ReportElement(LPCTSTR lpszCaption, const DATE& ToDate )
{
    CString strHeader;

    strHeader.Format( _T("*** %s REPORT *** %2d/%2d %2d:%02d"),
                      lpszCaption,
                      ToDate.usMonth,
                      ToDate.usMDay,
                      ToDate.usHour,
                      ToDate.usMin );

    SetReportString( strHeader );

	return true;
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::ClearAllReport()
//===========================================================================

inline void CPCSampleDoc::ClearAllReport()
{
	DeleteContents();
    UpdateAllViews( NULL, 1 );
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::DeleteContents()
//===========================================================================

inline void CPCSampleDoc::DeleteContents() 
{
	m_listReportStr.RemoveAll();
	m_listCurrentStr.RemoveAll();
	m_listReportList.RemoveAll();
    m_dwMaxColumnInReportStr = 0;
}

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CPCSampleDoc::GetNoOfReportStrings()
//===========================================================================

inline int CPCSampleDoc::GetNoOfReportStrings() const
{
    return  ( static_cast<int> (m_listReportStr.GetCount()));
}

//===========================================================================
//  FUNCTION :  CPCSampleDoc::GetMaxColumnOfStrings()
//===========================================================================

inline DWORD CPCSampleDoc::GetMaxColumnOfStrings() const
{
    return ( m_dwMaxColumnInReportStr );
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSAMPLEDOC_H__70072F0A_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////// END OF FILE ( PCSampleDoc.h ) ////////////////////////////
