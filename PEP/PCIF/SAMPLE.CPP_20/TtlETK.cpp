/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlETK.cpp
//
//  PURPOSE:    Provides ETK total class.
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
//  FUNCTION :  CTtlETK::CTtlETK()
//
//  PURPOSE :   Constructor of ETK total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlETK::CTtlETK() :
    m_sLastError( SUCCESS ),
    m_ulEmployeeNo( 0 ),
    m_usFieldNo( 0 )
{
	memset( &m_fieldETK, 0, sizeof( m_fieldETK ));
    memset( &m_timeETK,0,  sizeof( m_timeETK  ));
    /*::ZeroMemory( &m_fieldETK, sizeof( m_fieldETK ));
    ::ZeroMemory( &m_timeETK,  sizeof( m_timeETK  ));*/
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::~CTtlETK()
//
//  PURPOSE :   Destructor of ETK total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlETK::~CTtlETK()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::Read()
//
//  PURPOSE :   Read ETK total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::Read(
    const UCHAR uchClassToRead, // indicates current or saved daily total
    const ULONG ulETKNo,        // employee# to read total
    USHORT&     usFieldNo,      // field# to read total
    BOOL&       fIsEndOfField   // indicates all ETK in the field is read
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ));
    ASSERT(( 0 < ulETKNo ) && ( ulETKNo <= pcttl::MAX_ETK_NO ));
    ASSERT( usFieldNo < ETK_MAX_FIELD );

    CString str;

    // --- read ETK total by specified employee and field# ---

    if ( uchClassToRead == CLASS_TTLCURDAY )
    {
        m_sLastError = ::SerEtkCurIndRead( ulETKNo,
                                           &usFieldNo,
                                           &m_fieldETK,
                                           &m_timeETK );

        str.Format( _T("\t::SerEtkCurIndRead() - Called (%d)"), m_sLastError );
    }
    else
    {
        m_sLastError = ::SerEtkSavIndRead( ulETKNo,
                                           &usFieldNo,
                                           &m_fieldETK,
                                           &m_timeETK );

        str.Format( _T("\t::SerEtkSavIndRead() - Called (%d)"), m_sLastError );
    }
    str += CString( "...Employee#%ld, Filed#%d" );
    pcsample::TraceFunction( str, ulETKNo, usFieldNo );

    switch ( m_sLastError )
    {
    case ETK_SUCCESS:
    case ETK_CONTINUE:

        if ( uchClassToRead == CLASS_TTLCURDAY )
        {
            // --- set current time, if read report is specified ---

            GetCurrentDateTime( m_timeETK );
        }

        // --- set end of field flag as TRUE, if all record is read ---

        fIsEndOfField  = ( m_sLastError == ETK_SUCCESS ) ? TRUE : FALSE;

        // --- save employee# and field# of the last read ---

        m_ulEmployeeNo = ulETKNo;
        m_usFieldNo    = static_cast< USHORT >( usFieldNo - 1 );

        m_sLastError   = ETK_SUCCESS;
        break;

    default:
        break;
    }

    pcsample::TraceFunction( _T("END   >> CTtlETK::Read() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::Reset()
//
//  PURPOSE :   Reset all of ETK totals in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::Reset()
{
    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::Reset()") );

    // --- reset all of totals via PC Interface function ---

    m_sLastError = ::SerEtkAllReset();

    CString str;
    str.Format( _T("\t::SerEtkAllReset() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTtlETK()::Reset() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Total is able to reset.
//              FALSE   - Total is not able to reset or function is failed.
//
//===========================================================================

BOOL CTtlETK::IsResetOK()
{
    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::IsResetOK()") );

    // --- determine specified total is able to reset or not ---

    m_sLastError = ::SerEtkIssuedCheck();

    CString str;
    str.Format( _T("\t::SerEtkIssuedCheck() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTtlETK()::IsResetOK() = %d"),
                             ( m_sLastError == ETK_ISSUED ));

    return ( m_sLastError == ETK_ISSUED );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::ResetAsOK()
{
    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::ResetAsOK()") );

    // --- set specified total as able to reset via base class ---

    m_sLastError = ::SerEtkIssuedSet();

    CString str;
    str.Format( _T("\t::SerEtkIssuedSet() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTtlETK()::ResetAsOK() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::Lock()
//
//  PURPOSE :   Lock all of ETK in 2170 terminal.
//
//  RETURN :    TRUE    - All of ETK record is locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::Lock()
{
    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::Lock()") );

    // --- lock all of ETK records in 2170 via PC Interface function ---

    m_sLastError = ::SerEtkAllLock();

    CString str;
    str.Format( _T("\t::SerEtkAllLock() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTtlETK()::Lock() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::Unlock()
//
//  PURPOSE :   Unlock all of ETK in 2170 terminal.
//
//  RETURN :    TRUE    - All of ETK record is locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::Unlock()
{
    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::Lock()") );

    // --- Unlock all of ETK records in 2170 via PC Interface function ---

    ::SerEtkAllUnLock();
    m_sLastError = ETK_SUCCESS;

    CString str;
    str.Format( _T("\t::SerEtkAllUnlock() - Called (VOID)") );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTtlETK()::Lock() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::GetAllETKNo()
//
//  PURPOSE :   Get all of existing ETK record number in 2170.
//
//  RETURN :    TRUE    - All of ETK record number is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlETK::GetAllETKNo(
    const UCHAR uchClassToRead, // indicates current or saved daily total
    ULONG*      paulETKNo,      // points to buffer to receive ETK number
    const USHORT usBufferSize,  // size of paulETKNo buffer in bytes
    USHORT& usNoOfETK           // no. of ETK read in paulETKNo buffer
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ));
    ASSERT( ! IsBadWritePtr( paulETKNo, usBufferSize ));

    pcsample::TraceFunction( _T("BEGIN >> CTtlETK::GetAllETKNo()") );

    // --- read all of existing ETK total in 2170 ---

    SHORT   sResult;

    CString str;

    if ( uchClassToRead == CLASS_TTLCURDAY )
    {
        sResult = ::SerEtkCurAllIdRead( usBufferSize, paulETKNo );
        str.Format( _T("\t::SerEtkCurAllIdRead() - Called (%d)"), sResult );
    }
    else
    {
        sResult = ::SerEtkSavAllIdRead( usBufferSize, paulETKNo );
        str.Format( _T("\t::SerEtkSavAllIdRead() - Called (%d)"), sResult );
    }
    pcsample::TraceFunction( str );

    if ( 0 <= sResult )
    {
        // --- set no. of ETK numbers to user specified area ---

        usNoOfETK    = sResult;
        m_sLastError = ETK_SUCCESS;
    }
    else
    {
        // --- function is failed ---

        m_sLastError = sResult;
    }

    pcsample::TraceFunction( _T("END   >> CTtlETK()::GetAllETKNo() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTtlETK::GetCurrentDateTime()
//
//  PURPOSE :   Get current system time to user specified structure.
//
//  RETURN :    TRUE    - Current system time is retrieved.
//              FALSE   - Function is failed.
//
//===========================================================================

VOID CTtlETK::GetCurrentDateTime(
    ETK_TIME& thisDate          // points to structure to store date/time
    )
{
    // --- get current system date & time ---

    time_t  timeCurrent;
    struct tm*  ptmTime;

    time( &timeCurrent );
    ptmTime = localtime( &timeCurrent );

    // --- copy system time to user defined structure ---

    thisDate.usToMonth  = static_cast< USHORT >( ptmTime->tm_mon + 1 );
    thisDate.usToDay    = static_cast< USHORT >( ptmTime->tm_mday    );
    thisDate.usToTime   = static_cast< USHORT >( ptmTime->tm_hour    );
    thisDate.usToMinute = static_cast< USHORT >( ptmTime->tm_min     );
}

/////////////////// END OF FILE ( TtlETK.cpp ) //////////////////////////////
