/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Total.h
//
//  PURPOSE:    Base Total class definitions and declarations.
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
#include "time.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Global.h"

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

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CTotal::CTotal()
//
//  PURPOSE :   Constructor of total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTotal::CTotal() :
    m_sLastError( SUCCESS )
{
}

//===========================================================================
//
//  FUNCTION :  CTotal::~CTotal()
//
//  PURPOSE :   Destructor of total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTotal::~CTotal()
{
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalRead()
//
//  PURPOSE :   Read the specified total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTotal::TotalRead(
    LPVOID lpvTotal             // points to buffer to receive total data
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalRead()") );

    // --- read specified total via PC Interface function ---

    m_sLastError = ::SerTtlTotalRead( lpvTotal );

    CString str;
    str.Format( _T("\t::SerTtlTotalRead() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalRead() = %d"), ( m_sLastError == TTL_SUCCESS ));

    return ( m_sLastError == TTL_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalReset()
//
//  PURPOSE :   Reset the specified total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTotal::TotalReset(
    LPCVOID lpvTotal,               // points to total data to reset
    const USHORT usEODResetType )   // which total to be resetted at EOD
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalReset()") );

    // --- reset specified total via PC Interface function ---

    m_sLastError = ::SerTtlTotalReset( const_cast< LPVOID >( lpvTotal ), usEODResetType );

    CString str;
    str.Format( _T("\t::SerTtlTotalReset() - Called (%d)"), m_sLastError );
    str += CString( "...Reset Type:%04x" );
    pcsample::TraceFunction( str, usEODResetType );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalReset() = %d"), ( m_sLastError == TTL_SUCCESS ));

    return ( m_sLastError == TTL_SUCCESS || m_sLastError == TTL_NOT_ISSUED_MDC );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalIsReportIssued()
//
//  PURPOSE :   Determine whether the reset report is already issued or not.
//
//  RETURN :    TRUE    - Specified total is issued or it is not in file.
//              FALSE   - Specified total is not issued or error occurs.
//
//===========================================================================

BOOL CTotal::TotalIsReportIssued(
    LPCVOID lpvTotal            // points to total data to determine
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalIsReportIssued()") );

    // --- check specified total is issued via PC Interface function ---

    m_sLastError = ::SerTtlIssuedCheck( const_cast< LPVOID >( lpvTotal ),
                                        pcttl::RESET_ENABLED );

    CString str;
    str.Format( _T("\t::SerTtlIssuedCheck() - Called (%d)"), m_sLastError );
    str += CString( "...Check Type:%04x" );
    pcsample::TraceFunction( str, pcttl::RESET_ENABLED );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalIsResetIssued() = %d"),
                             (( m_sLastError == TTL_ISSUED    )||
                              ( m_sLastError == TTL_NOTINFILE )) );

    return ( m_sLastError == TTL_ISSUED || m_sLastError == TTL_NOTINFILE || m_sLastError == TTL_NOT_ISSUED_MDC );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalReportAsIssued()
//
//  PURPOSE :   Set the reset report of the specified total is issued.
//
//  RETURN :    TRUE    - Set specified total as issued.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTotal::TotalReportAsIssued( LPCVOID lpvTotal )
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalReportAsIssued()") );

    // --- set reset report of this total as already issued ---

    m_sLastError = ::SerTtlIssuedReset( const_cast< LPVOID >( lpvTotal ),
                                        pcttl::RESET_ENABLED );

    CString str;
    str.Format( _T("\t::SerTtlIssuedReset() - Called (%d)"), m_sLastError );
    str += CString( "...Issue Type:%04x" );
    pcsample::TraceFunction( str, pcttl::RESET_ENABLED );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalReportAsIssued() = %d"),
                             ( m_sLastError == TTL_SUCCESS ));

    return ( m_sLastError == TTL_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalReadPluEx()
//
//  PURPOSE :   Read the plural totals in one function call.
//
//  RETURN :    TRUE    - Plural totals are read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTotal::TotalReadPluEx(
    LPVOID lpvTotal             // points to buffer to receive total data
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalReadPlural()") );

    // --- read the plural totals via PC Interface function ---

    m_sLastError = ::SerTtlTotalReadPluEx( lpvTotal );

    CString str;
    str.Format( _T("\t::SerTtlTotalReadPluEx() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalReadPluEx() = %d"),
                             ( m_sLastError == TTL_SUCCESS ));

    return ( m_sLastError == TTL_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalReadPlural()
//
//  PURPOSE :   Read the plural totals in one function call.
//
//  RETURN :    TRUE    - Plural totals are read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTotal::TotalReadPlural(
    LPVOID lpvTotal             // points to buffer to receive total data
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CTotal::TotalReadPlural()") );

    // --- read the plural totals via PC Interface function ---

    m_sLastError = ::SerTtlTotalReadPlu( lpvTotal );

    CString str;
    str.Format( _T("\t::SerTtlTotalReadPlu() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTotal::TotalReadPlural() = %d"),
                             ( m_sLastError == TTL_SUCCESS ));

    return ( m_sLastError == TTL_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTotal::TotalGetCurrentDateTime()
//
//  PURPOSE :   Get current system date & time.
//
//  RETURN :    TRUE    - Current system date & time is retrieved.
//              FALSE   - Function is failed.
//
//===========================================================================

VOID CTotal::TotalGetCurrentDateTime(
    DATE& thisDate              // buffer to receive current date and time
    )
{
    // --- get current system date & time ---

    time_t  timeCurrent;
    struct tm*  ptmTime;

    time( &timeCurrent );
    ptmTime = localtime( &timeCurrent );

    // --- copy system time to user defined structure ---

    thisDate.usMonth = static_cast< USHORT >( ptmTime->tm_mon + 1 );
    thisDate.usMDay  = static_cast< USHORT >( ptmTime->tm_mday    );
    thisDate.usHour  = static_cast< USHORT >( ptmTime->tm_hour    );
    thisDate.usMin   = static_cast< USHORT >( ptmTime->tm_min     );
#if defined(DCURRENCY_LONGLONG)
	thisDate.usYear  = static_cast< USHORT >(ptmTime->tm_year);    // with 64 bit totals we are adding year to the date stamp
#endif
}

//////////////////// END OF FILE ( Total.cpp ) //////////////////////////////
