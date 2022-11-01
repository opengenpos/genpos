/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlRegFin.cpp
//
//  PURPOSE:    Provides register finalcial total class.
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
//  FUNCTION :  CTtlRegFin::CTtlRegFin()
//
//  PURPOSE :   Constructor of register financial class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlRegFin::CTtlRegFin()
{
    memset( &m_ttlRegFin, 0, sizeof( m_ttlRegFin ));
    //::ZeroMemory( &m_ttlRegFin, sizeof( m_ttlRegFin ));
}

//===========================================================================
//
//  FUNCTION :  CTtlRegFin::~CTtlRegFin()
//
//  PURPOSE :   Destructor of register financial class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlRegFin::~CTtlRegFin()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlRegFin::Read()
//
//  PURPOSE :   Read register financial total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlRegFin::Read(
    const UCHAR uchClassToRead  // indicates current or saved daily/PTD total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));

    CString str;
    str.Format( _T("BEGIN >> CTtlRegFin::Read()") );
    str += CString( "...Minor#%d" );
    pcsample::TraceFunction( str, uchClassToRead );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlRegFin.uchMajorClass = CLASS_TTLREGFIN;
    m_ttlRegFin.uchMinorClass = uchClassToRead;

    fSuccess = CTotal::TotalRead( &m_ttlRegFin );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            CTotal::TotalGetCurrentDateTime( m_ttlRegFin.ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlRegFin::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlRegFin::Reset()
//
//  PURPOSE :   Reset register financial total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlRegFin::Reset(
    const BOOL fIsEODReset,      // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    ASSERT( fIsEODReset );

    CString str;
    str.Format( _T("BEGIN >> CTtlRegFin::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    // --- reset specified total via base class function ---

    m_ttlRegFin.uchMajorClass = CLASS_TTLEODRESET;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlRegFin, pcttl::EOD_REGFIN );

    pcsample::TraceFunction( _T("END   >> CTtlDept::Reset() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlRegFin::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - This total is able to reset.
//              FALSE   - This total is not able to reset.
//
//===========================================================================

BOOL CTtlRegFin::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlRegFin::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlRegFin.uchMajorClass = CLASS_TTLREGFIN;
    m_ttlRegFin.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlRegFin );

    pcsample::TraceFunction( _T("END   >> CTtlRegFin::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlRegFin::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlRegFin::ResetAsOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlRegFin::ResetAsOK()") );
    pcsample::TraceFunction( str );

    // --- set specified total as able to reset via base class ---

    m_ttlRegFin.uchMajorClass = CLASS_TTLREGFIN;
    m_ttlRegFin.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlRegFin );

    pcsample::TraceFunction( _T("END   >> CTtlRegFin::ResetAsOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

///////////////// END OF FILE ( TtlRegFin.cpp ) /////////////////////////////
