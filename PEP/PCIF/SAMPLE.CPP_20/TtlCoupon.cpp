/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlCoupon.cpp
//
//  PURPOSE:    Provides Coupon total class.
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
//  FUNCTION :  CTtlCoupon::CTtlCoupon()
//
//  PURPOSE :   Constructor of Coupon total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlCoupon::CTtlCoupon()
{
	memset( &m_ttlCoupon, 0,  sizeof( m_ttlCoupon ));
    //::ZeroMemory( &m_ttlCoupon,  sizeof( m_ttlCoupon ));
}

//===========================================================================
//
//  FUNCTION :  CTtlCoupon::~CTtlCoupon()
//
//  PURPOSE :   Destructor of Coupon total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlCoupon::~CTtlCoupon()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlCoupon::Read()
//
//  PURPOSE :   Read Coupon total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CTtlCoupon::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const ULONG  ulCouponNo     // coupon# to read coupon total
    )
{
	USHORT  usCouponNo = (USHORT)ulCouponNo;

	return Read (uchClassToRead, usCouponNo);
}

BOOL CTtlCoupon::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const USHORT usCouponNo     // coupon# to read coupon total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));
    ASSERT(( 0 < usCouponNo ) && ( usCouponNo <= pcttl::MAX_COUPON_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlCoupon::Read()") );
    str += CString( "...Minor#%d, Coupon#%d" );
    pcsample::TraceFunction( str, uchClassToRead, usCouponNo );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlCoupon.uchMajorClass = CLASS_TTLCPN;
    m_ttlCoupon.uchMinorClass = uchClassToRead;
    m_ttlCoupon.usCpnNumber   = usCouponNo;

    fSuccess = CTotal::TotalRead( &m_ttlCoupon );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            CTotal::TotalGetCurrentDateTime( m_ttlCoupon.ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlCoupon::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlCoupon::Reset()
//
//  PURPOSE :   Reset Coupon total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlCoupon::Reset(
    const BOOL fIsEODReset,  // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    CString str;
    str.Format( _T("BEGIN >> CTtlCoupon::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    USHORT  usResetType;

    // --- reset specified total via base class function ---

    if ( fIsEODReset )
    {
        m_ttlCoupon.uchMajorClass = CLASS_TTLEODRESET;
		if (uchMajorClass == CLASS_TTLEODRESET) {
			m_ttlCoupon.uchMinorClass = CLASS_TTLCURDAY;
		} else {
			m_ttlCoupon.uchMinorClass = CLASS_TTLCURPTD;
		}
        usResetType = pcttl::EOD_COUPON;
    }
    else
    {
        m_ttlCoupon.uchMajorClass = CLASS_TTLCPN;
		if (uchMajorClass == CLASS_TTLEODRESET) {
			m_ttlCoupon.uchMinorClass = CLASS_TTLCURDAY;
		} else {
			m_ttlCoupon.uchMinorClass = CLASS_TTLCURPTD;
		}
        usResetType = pcttl::NOT_EOD;
    }

    BOOL    fSuccess = CTotal::TotalReset( &m_ttlCoupon, usResetType );

    pcsample::TraceFunction( _T("END   >> CTtlCoupon::Reset() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlCoupon::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlCoupon::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlCoupon::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlCoupon.uchMajorClass = CLASS_TTLCPN;
    m_ttlCoupon.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlCoupon );

    pcsample::TraceFunction( _T("END   >> CTtlCoupon::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlCoupon::ResetAsOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlCoupon::ResetAsOK()
{
    // --- determine specified total is able to reset via base class ---

    m_ttlCoupon.uchMajorClass = CLASS_TTLCPN;
    m_ttlCoupon.uchMinorClass = CLASS_TTLSAVDAY;

    return ( CTotal::TotalReportAsIssued( &m_ttlCoupon ));
}

////////////////// END OF FILE ( TtlCoupon.cpp ) ////////////////////////////
