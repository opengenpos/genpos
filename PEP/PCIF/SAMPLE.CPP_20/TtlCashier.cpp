/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlCashier.cpp
//
//  PURPOSE:    Provides Cashier total class.
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
//  FUNCTION :  CTtlCashier::CTtlCashier()
//
//  PURPOSE :   Constructor of Cashier total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlCashier::CTtlCashier()
{
	memset( &m_ttlCashier, 0, sizeof( m_ttlCashier ));
    //::ZeroMemory( &m_ttlCashier, sizeof( m_ttlCashier ));
}

//===========================================================================
//
//  FUNCTION :  CTtlCashier::~CTtlCashier()
//
//  PURPOSE :   Destructor of Cashier total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlCashier::~CTtlCashier()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlCashier::Read()
//
//  PURPOSE :   Read Cashier total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CTtlCashier::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const USHORT usCashierNo    // cashier# to read cashier total
    )
{
	ULONG  ulCashierNo = usCashierNo;

	return Read (uchClassToRead, ulCashierNo);
}


BOOL CTtlCashier::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const ULONG ulCashierNo    // cashier# to read cashier total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));
    ASSERT(( 0 < ulCashierNo ) && ( ulCashierNo <= pcttl::MAX_CASHIER_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlCashier::Read()") );
    str += CString( "...Minor#%d, Cashier#%d" );
    pcsample::TraceFunction( str, uchClassToRead, ulCashierNo );

    BOOL    fSuccess;

    // --- read specified total via base class function ---
    
    m_ttlCashier.uchMajorClass   = CLASS_TTLCASHIER;
    m_ttlCashier.uchMinorClass   = uchClassToRead;
    m_ttlCashier.ulCashierNumber = ulCashierNo;

    fSuccess = CTotal::TotalRead( &m_ttlCashier );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            CTotal::TotalGetCurrentDateTime( m_ttlCashier.ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlCashier::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlCashier::Reset()
//
//  PURPOSE :   Reset Cashier total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlCashier::Reset(
    const USHORT  usCashierNo,   // cashier# to reset cashier total
    const BOOL fIsEODReset,      // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
	ULONG  ulCashierNo = usCashierNo;

	return Reset (ulCashierNo, fIsEODReset);
}

BOOL CTtlCashier::Reset(
    const ULONG ulCashierNo,   // cashier# to reset cashier total
    const BOOL fIsEODReset,    // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    ASSERT(( 0 < ulCashierNo ) && ( ulCashierNo <= pcttl::MAX_CASHIER_NO ));
    ASSERT( fIsEODReset );

    CString str;
    str.Format( _T("BEGIN >> CTtlCashier::Reset()") );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    // --- reset specified total via base class function ---
    m_ttlCashier.uchMajorClass   = CLASS_TTLCASHIER;
	if (uchMajorClass == CLASS_TTLEODRESET) {
		m_ttlCashier.uchMinorClass   = CLASS_TTLCURDAY;
	} else {
		m_ttlCashier.uchMinorClass   = CLASS_TTLCURPTD;
	}
    m_ttlCashier.ulCashierNumber = ulCashierNo;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlCashier, pcttl::CASHIER_RESET );

    pcsample::TraceFunction( _T("END   >> CTtlCashier::Reset() = %d"), fSuccess );

    return ( fSuccess );

    UNREFERENCED_PARAMETER( fIsEODReset );
}

//===========================================================================
//
//  FUNCTION :  CTtlCashier::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlCashier::IsResetOK(
    const USHORT  usCashierNo    // cashier# to determine its status
    )
{
	ULONG  ulCashierNo = usCashierNo;

	return IsResetOK (ulCashierNo);
}

BOOL CTtlCashier::IsResetOK(
    const ULONG ulCashierNo    // cashier# to determine its status
    )
{
    ASSERT( ulCashierNo <= pcttl::MAX_CASHIER_NO );

    CString str;
    str.Format( _T("BEGIN >> CTtlCashier::IsResetOK()") );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    // --- determine specified total is able to reset via base class ---

    m_ttlCashier.uchMajorClass   = CLASS_TTLCASHIER;
    m_ttlCashier.uchMinorClass   = CLASS_TTLSAVDAY;
    m_ttlCashier.ulCashierNumber = ulCashierNo;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlCashier );

    pcsample::TraceFunction( _T("END   >> CTtlCashier::IsResetOK() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlCashier::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CTtlCashier::ResetAsOK(
    const USHORT usCashierNo    // cashier# to set as able to reset
    )
{
	ULONG  ulCashierNo = usCashierNo;

	return ResetAsOK (ulCashierNo);
}


BOOL CTtlCashier::ResetAsOK(
    const ULONG ulCashierNo    // cashier# to set as able to reset
    )
{
    ASSERT(( 0 < ulCashierNo ) && ( ulCashierNo <= pcttl::MAX_CASHIER_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlCashier::ResetAsOK()") );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    // --- set specified total as able to reset via base class ---

    m_ttlCashier.uchMajorClass   = CLASS_TTLCASHIER;
    m_ttlCashier.uchMinorClass   = CLASS_TTLSAVDAY;
    m_ttlCashier.ulCashierNumber = ulCashierNo;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlCashier );

    pcsample::TraceFunction( _T("END   >> CTtlCashier::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//////////////////// END OF FILE ( TtlCashier.cpp ) /////////////////////////
