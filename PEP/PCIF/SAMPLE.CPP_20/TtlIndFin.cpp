/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlIndFin.cpp
//
//  PURPOSE:    Provides individual register finalcial total class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
/////////////////////////////////////////////////////////////////////////////

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
//  FUNCTION :  CTtlIndFin::CTtlIndFin()
//
//  PURPOSE :   Constructor of individual register financial class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlIndFin::CTtlIndFin()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlIndFin::~CTtlIndFin()
//
//  PURPOSE :   Destructor of individual register financial class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlIndFin::~CTtlIndFin()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlIndFin::Read()
//
//  PURPOSE :   Read individual register financial total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlIndFin::Read(
    const UCHAR uchClassToRead, // indicates current or saved daily/PTD total
    const USHORT usTerminalNo   // terminal# to read individual total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlIndFin::Read()") );
    str += CString( "...Minor#%d, Terminal#%d" );
    pcsample::TraceFunction( str, uchClassToRead, usTerminalNo );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlRegFin.uchMajorClass    = CLASS_TTLINDFIN;
    m_ttlRegFin.uchMinorClass    = uchClassToRead;
    m_ttlRegFin.usTerminalNumber = usTerminalNo;

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

    pcsample::TraceFunction( _T("END   >> CTtlIndFin::Read() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlIndFin::Reset()
//
//  PURPOSE :   Reset register financial total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlIndFin::Reset(
    const USHORT usTerminalNo,  // terminal# to reset individual total
    const BOOL fIsEODReset,     // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlIndFin::Reset()") );
    str += CString( "...Terminal#%d, At EOD:%d" );
    pcsample::TraceFunction( str, usTerminalNo, fIsEODReset );

    // --- reset specified total via base class function ---

    m_ttlRegFin.uchMajorClass = CLASS_TTLINDFIN;
	if (uchMajorClass == CLASS_TTLEODRESET) {
		m_ttlRegFin.uchMinorClass = CLASS_TTLCURDAY;
	} else {
		m_ttlRegFin.uchMinorClass = CLASS_TTLCURPTD;
	}
    m_ttlRegFin.usTerminalNumber = usTerminalNo;

    USHORT  usResetType;
    usResetType = ( fIsEODReset ) ? pcttl::EOD_INDFIN : pcttl::INDFIN_RESET;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlRegFin, usResetType );

    pcsample::TraceFunction( _T("END   >> CTtlIndFin::Reset() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlIndFin::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - This total is able to reset.
//              FALSE   - This total is not able to reset.
//
//===========================================================================

BOOL CTtlIndFin::IsResetOK(
    const USHORT usTerminalNo   // terminal# to determine its status
    )
{
    ASSERT( usTerminalNo <= MAX_TERMINAL_NO );

    CString str;
    str.Format( _T("BEGIN >> CTtlIndFin::IsResetOK()") );
    str += CString( "...Terminal#%d" );
    pcsample::TraceFunction( str, usTerminalNo );

    // --- determine specified total is able to reset via base class ---

    m_ttlRegFin.uchMajorClass    = CLASS_TTLINDFIN;
    m_ttlRegFin.uchMinorClass    = CLASS_TTLSAVDAY;
    m_ttlRegFin.usTerminalNumber = usTerminalNo;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlRegFin );

    pcsample::TraceFunction( _T("END   >> CTtlIndFin::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlIndFin::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlIndFin::ResetAsOK(
    const USHORT usTerminalNo   // terminal# to set as able to reset
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlIndFin::ResetAsOK()") );
    str += CString( "...Terminal#%d" );
    pcsample::TraceFunction( str, usTerminalNo );

    // --- set specified total as able to reset via base class ---

    m_ttlRegFin.uchMajorClass    = CLASS_TTLINDFIN;
    m_ttlRegFin.uchMinorClass    = CLASS_TTLSAVDAY;
    m_ttlRegFin.usTerminalNumber = usTerminalNo;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlRegFin );

    pcsample::TraceFunction( _T("END   >> CTtlIndFin::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

///////////////// END OF FILE ( TtlIndFin.cpp ) /////////////////////////////