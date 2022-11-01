/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaCoupon.cpp
//
//  PURPOSE:    Coupon Parameter class definitions and declarations.
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

#include "Parameter.h"
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
//  FUNCTION :  CParaCoupon::CParaCoupon()
//
//  PURPOSE :   Constructor of Coupon Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCoupon::CParaCoupon() :
    m_sLastError( FALSE )
{
	memset( &m_paraCoupon, 0, sizeof( m_paraCoupon ));
    //::ZeroMemory( &m_paraCoupon, sizeof( m_paraCoupon ));
}

//===========================================================================
//
//  FUNCTION :  CParaCoupon::~CParaCoupon()
//
//  PURPOSE :   Destructor of Coupon Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCoupon::~CParaCoupon()
{
}

//===========================================================================
//
//  FUNCTION :  CParaDept::Read()
//
//  PURPOSE :   Read the Coupon parameter by coupon#.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCoupon::Read(
    const USHORT usCouponNo     // coupon# to read
    )
{
    ASSERT(( 0 < usCouponNo ) && ( usCouponNo <= pcttl::MAX_COUPON_NO ));

    pcsample::TraceFunction( _T("BEGIN >> CParaCoupon::Read()") );

    // --- read Coupon parameter by specified coupon# ---

    m_paraCoupon.uchCpnNo = usCouponNo;

    m_sLastError = ::CliOpCpnIndRead( &m_paraCoupon, 0 );

    CString str;
    str.Format( _T("\t::CliOpCpnIndRead() - Called (%d)"), m_sLastError );
    str += CString( "...Coupon#%d" );
    pcsample::TraceFunction( str, usCouponNo);

    pcsample::TraceFunction( _T("END   >> CParaCoupon::Read() = %d"), ( m_sLastError == OP_PERFORM ));

    return ( m_sLastError == OP_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaCoupon::getName()
//
//  PURPOSE :   Get Coupon name in read Coupon parameter.
//
//  RETURN :    LPTSTR  - Address of buffer to Department name
//
//===========================================================================

LPCTSTR CParaCoupon::getName(
    LPTSTR  lpszCouponName      // points to buffer to retrieve coupon name
    ) const
{
    // --- copy coupon name to user buffer ---
	if (lpszCouponName) {
		memset( lpszCouponName, 0,  (OP_CPN_NAME_SIZE + 1) * sizeof(TCHAR) );
		_tcsncpy(lpszCouponName, m_paraCoupon.ParaCpn.aszCouponMnem, OP_CPN_NAME_SIZE );
	}
	else {
		return m_paraCoupon.ParaCpn.aszCouponMnem;
	}

    // --- return to coupon name buffer address ---
    return ( lpszCouponName );
}

//////////////////// END OF FILE ( ParaCoupon.cpp ) /////////////////////////
