/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlServTime.cpp
//
//  PURPOSE:    Provides Service Time total class.
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
//  FUNCTION :  CTtlServTime::CTtlServTime()
//
//  PURPOSE :   Constructor of Service Time total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlServTime::CTtlServTime()
{
    memset( &m_ttlServTime, 0, sizeof( m_ttlServTime ));
    //::ZeroMemory( &m_ttlServTime, sizeof( m_ttlServTime ));
}

//===========================================================================
//
//  FUNCTION :  CTtlServTime::~CTtlServTime()
//
//  PURPOSE :   Destructor of Service Time total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlServTime::~CTtlServTime()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlServTime::Read()
//
//  PURPOSE :   Read Service Time total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlServTime::Read(
    const UCHAR  uchClassToRead // indicates current or saved daily/PTD total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));

    CString str;
    str.Format( _T("BEGIN >> CTtlServTime::Read()") );
    str += CString( "...Minor#%d" );
    pcsample::TraceFunction( str, uchClassToRead );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlServTime.uchMajorClass = CLASS_TTLSERVICE;
    m_ttlServTime.uchMinorClass = uchClassToRead;

    fSuccess = CTotal::TotalRead( &m_ttlServTime );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            CTotal::TotalGetCurrentDateTime( m_ttlServTime.ToDate );
            break;

        default:
            break;
        }
    }
    else
    {
        if ( GetLastError() == TTL_NOTCLASS )
        {
            memset( m_ttlServTime.ServiceTime, 0, 
                          sizeof( m_ttlServTime.ServiceTime ));
            /*::ZeroMemory( m_ttlServTime.ServiceTime,
                          sizeof( m_ttlServTime.ServiceTime ));*/
            fSuccess = TRUE;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlServTime::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlServTime::Reset()
//
//  PURPOSE :   Reset Service Time total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlServTime::Reset(
    const BOOL fIsEODReset,  // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    CString str;
    str.Format( _T("BEGIN >> CTtlServTime::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    USHORT  usResetType;

    // --- reset specified total via base class function ---

    if ( fIsEODReset )
    {
        m_ttlServTime.uchMajorClass = CLASS_TTLEODRESET;
        m_ttlServTime.uchMinorClass = CLASS_TTLCURDAY;
        usResetType = pcttl::EOD_SERVTIME;
    }
    else
    {
        m_ttlServTime.uchMajorClass = CLASS_TTLSERVICE;
        m_ttlServTime.uchMinorClass = CLASS_TTLCURDAY;
        usResetType = pcttl::NOT_EOD;
    }

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlServTime, usResetType );

    if (( ! fSuccess ) && ( GetLastError() == TTL_NOTCLASS ))
    {
        fSuccess = TRUE;
    }

    pcsample::TraceFunction( _T("END   >> CTtlServTime::Reset() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlServTime::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlServTime::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlServTime::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlServTime.uchMajorClass = CLASS_TTLSERVICE;
    m_ttlServTime.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlServTime );

    if (( ! fSuccess ) && ( GetLastError() == TTL_NOTCLASS ))
    {
        fSuccess = TRUE;
    }

    pcsample::TraceFunction( _T("END   >> CTtlServTime::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlServTime::ResetAsOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlServTime::ResetAsOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlServTime::ResetAsOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlServTime.uchMajorClass = CLASS_TTLSERVICE;
    m_ttlServTime.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlServTime );

    if (( ! fSuccess ) && ( GetLastError() == TTL_NOTCLASS ))
    {
        fSuccess = TRUE;
    }

    pcsample::TraceFunction( _T("END   >> CTtlServTime::ResetAsOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

////////////////// END OF FILE ( TtlServTime.cpp ) //////////////////////////
