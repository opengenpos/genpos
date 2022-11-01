/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlDept.cpp
//
//  PURPOSE:    Provides Department total class.
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
//  FUNCTION :  CTtlDept::CTtlDept()
//
//  PURPOSE :   Constructor of Department total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlDept::CTtlDept()
{
	memset( &m_ttlDept, 0, sizeof( m_ttlDept ));
    //::ZeroMemory( &m_ttlDept,  sizeof( m_ttlDept ));
}

//===========================================================================
//
//  FUNCTION :  CTtlDept::~CTtlDept()
//
//  PURPOSE :   Destructor of Department total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlDept::~CTtlDept()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlDept::Read()
//
//  PURPOSE :   Read Department total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CTtlDept::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const ULONG  ulDeptNo       // department# to read department total
    )
{
	USHORT  usDeptNo = (USHORT)ulDeptNo;

	return Read (uchClassToRead, usDeptNo);
}

BOOL CTtlDept::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const USHORT usDeptNo       // department# to read department total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));
    ASSERT(( usDeptNo >= 0 ) && ( usDeptNo <= OP_MAX_DEPT_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlDept::Read()") );
    str += CString( "...Minor#%d, Department#%d" );
    pcsample::TraceFunction( str, uchClassToRead, usDeptNo );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlDept.uchMajorClass  = CLASS_TTLDEPT;
    m_ttlDept.uchMinorClass  = uchClassToRead;
//    m_ttlDept.uchDEPTNumber  = static_cast< UCHAR >( usDeptNo );
    m_ttlDept.usDEPTNumber  =  usDeptNo;

    fSuccess = CTotal::TotalRead( &m_ttlDept );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            CTotal::TotalGetCurrentDateTime( m_ttlDept.ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlDept::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlDept::Reset()
//
//  PURPOSE :   Reset Department total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlDept::Reset(
    const BOOL fIsEODReset,      // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    ASSERT( fIsEODReset );

    CString str;
    str.Format( _T("BEGIN >> CTtlDept::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    // --- reset specified total via base class function ---

    m_ttlDept.uchMajorClass = CLASS_TTLEODRESET;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlDept, pcttl::EOD_DEPT );

    pcsample::TraceFunction( _T("END   >> CTtlDept::Reset() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlDept::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Total is able to reset.
//              FALSE   - Total is not able to reset or function is failed.
//
//===========================================================================

BOOL CTtlDept::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlDept::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlDept.uchMajorClass  = CLASS_TTLDEPT;
    m_ttlDept.uchMinorClass  = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlDept );

    pcsample::TraceFunction( _T("END   >> CTtlDept::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlDept::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlDept::ResetAsOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlDept::ResetAsOK()") );
    pcsample::TraceFunction( str );

    // --- set specified total as able to reset via base class ---

    m_ttlDept.uchMajorClass = CLASS_TTLDEPT;
    m_ttlDept.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlDept );

    pcsample::TraceFunction( _T("END   >> CTtlDept::ResetAsOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

/////////////////// END OF FILE ( TtlDept.cpp ) /////////////////////////////