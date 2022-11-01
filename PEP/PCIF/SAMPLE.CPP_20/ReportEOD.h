/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ReportEOD.h
//
//  PURPOSE:    End Of Day report class definitions and declarations.
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

#if !defined(AFX_REPORTEOD_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_REPORTEOD_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
	#error include 'Total.h' before including this file
#endif


//---------------------------------------------------------------------------
//  CReportEOD
//---------------------------------------------------------------------------

class CReportEOD
{
public:
    CReportEOD( class CMainFrame* const pMainFrame,
                class CPCSampleDoc* const pDoc );
    ~CReportEOD();

    BOOL    Read();
    BOOL    Reset(const UCHAR uchMajorClass = CLASS_TTLEODRESET);
    BOOL    IsResetOK();

    UCHAR   GetLastErrorLocus(SHORT sLastErrorLocus = -1) const;
    SHORT   GetLastError() const;

	void	ExportDbToFiles(CString &csActivity);

private:
    CTotal* GetTotalObject( const INT nTotalID );
	BOOL    ResetAllRecords(CTtlRegFin& ttlRegFin, const UCHAR uchMajorClass = CLASS_TTLEODRESET);
	BOOL    ResetAllRecords(CTtlIndFin& ttlIndFin, const UCHAR uchMajorClass = CLASS_TTLEODRESET);
	BOOL    ResetAllRecords( CTtlCashier& ttlCashier, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    ResetAllRecords( CGuestCheck& ttlGC, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    VOID    IssueReport( CTtlRegFin& ttlRegFin );
    VOID    IssueReport( CTtlIndFin& ttlIndFin );
    VOID    IssueReport( CTtlDept& ttlDept );
    VOID    IssueReport( CTtlPlu& ttlPlu );
    VOID    IssueReport( CTtlHourly& ttlHourly );
    VOID    IssueReport( CTtlCashier& ttlCashier );
    VOID    IssueReport( CTtlCoupon& ttlCoupon );
    VOID    IssueReport( CTtlServTime& ttlServTime );
    VOID    IssueReport( CReportEJ& reportEJ );
	SHORT	IssueDelayBalance();

private:
    CMainFrame*     m_pMainFrame;
    CPCSampleDoc*   m_pDoc;

    CTtlRegFin      m_ttlRegFin;
    CTtlIndFin      m_ttlIndFin;
    CTtlDept        m_ttlDept;
    CTtlPlu         m_ttlPlu;
    CTtlHourly      m_ttlHourly;
    CTtlServTime    m_ttlServTime;
    CTtlCashier     m_ttlCashier;
    CTtlCoupon      m_ttlCoupon;
    CGuestCheck     m_GuestCheck;
    CReportEJ       m_reportEJ;

    SHORT   m_sLastErrorLocus;
    SHORT   m_sLastError;
    UCHAR   m_uchNoOfTerminals;

public:
    enum enumTotal
    {
        TTL_BEGIN = 0,
        TTL_GUEST,
        TTL_REGFIN,
        TTL_INDFIN,
        TTL_DEPT,
        TTL_PLU,
        TTL_HOURLY,
        TTL_CASHIER,
        TTL_COUPON,
        TTL_SERVTIME,
        TTL_EJ,
        TTL_END
    };

	SHORT m_sTotalIsResetLastError[TTL_END];

};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CReportEOD::GetLastError()
//===========================================================================

inline SHORT CReportEOD::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CReportEOD::GetLastErrorLocus()
//===========================================================================

inline UCHAR CReportEOD::GetLastErrorLocus(SHORT sLastErrorLocus /* = -1 */) const
{
    UCHAR   uchErrorClass;

	if (sLastErrorLocus < 0) {
		sLastErrorLocus = m_sLastErrorLocus;
	}
    switch ( sLastErrorLocus )
    {
		case CReportEOD::TTL_BEGIN:
		case CReportEOD::TTL_END:
			uchErrorClass = 0;
			break;

		case CReportEOD::TTL_REGFIN:
			uchErrorClass = CLASS_TTLREGFIN;
			break;

		case CReportEOD::TTL_INDFIN:
			uchErrorClass = CLASS_TTLINDFIN;
			break;

		case CReportEOD::TTL_DEPT:
			uchErrorClass = CLASS_TTLDEPT;
			break;

		case CReportEOD::TTL_PLU:
			uchErrorClass = CLASS_TTLPLU;
			break;

		case CReportEOD::TTL_HOURLY:
			uchErrorClass = CLASS_TTLHOURLY;
			break;

		case CReportEOD::TTL_CASHIER:
			uchErrorClass = CLASS_TTLCASHIER;
			break;

		case CReportEOD::TTL_COUPON:
			uchErrorClass = CLASS_TTLCPN;
			break;

		case CReportEOD::TTL_SERVTIME:
			uchErrorClass = CLASS_TTLSERVICE;
			break;

		case CReportEOD::TTL_GUEST:
			uchErrorClass = CLASS_TTLGUEST;
			break;

		case CReportEOD::TTL_EJ:
			uchErrorClass = CLASS_RPTEJ;
			break;

		default:
			ASSERT( FALSE );
			uchErrorClass = 0;
			break;
    }

    return ( uchErrorClass );
}

//---------------------------------------------------------------------------
//
//  PRIVATE FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CReportEOD::GetTotalObject()
//===========================================================================

inline CTotal* CReportEOD::GetTotalObject( const INT nTotalID )
{
    CTotal* pTotal = NULL;

    switch ( nTotalID )
    {
    case CReportEOD::TTL_REGFIN:
        pTotal = &m_ttlRegFin;
        break;

    case CReportEOD::TTL_INDFIN:
        pTotal = &m_ttlIndFin;
        break;

    case CReportEOD::TTL_DEPT:
        pTotal = &m_ttlDept;
        break;

    case CReportEOD::TTL_PLU:
        pTotal = &m_ttlPlu;
        break;

    case CReportEOD::TTL_HOURLY:
        pTotal = &m_ttlHourly;
        break;

    case CReportEOD::TTL_CASHIER:
        pTotal = &m_ttlCashier;
        break;

    case CReportEOD::TTL_COUPON:
        pTotal = &m_ttlCoupon;
        break;

    case CReportEOD::TTL_SERVTIME:
        pTotal = &m_ttlServTime;
        break;

    case CReportEOD::TTL_BEGIN:
    case CReportEOD::TTL_GUEST:
    case CReportEOD::TTL_EJ:
    case CReportEOD::TTL_END:
    default:
        pTotal = NULL;
        break;
    }

    return ( pTotal );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTEOD_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

/////////////////// END OF FILE ( ReportEOD.h ) ///////////////////////////
