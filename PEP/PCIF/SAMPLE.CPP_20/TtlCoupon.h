/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlCoupon.h
//
//  PURPOSE:    Coupon Total class definitions and declarations.
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

#if !defined(AFX_TTLCOUPON_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLCOUPON_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
	#error include 'Total.h' before including this file
#endif

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CTtlCoupon
//---------------------------------------------------------------------------

class CTtlCoupon : public CTotal
{
public:
    CTtlCoupon();
    ~CTtlCoupon();

    BOOL    Read( const UCHAR  uchClassToRead,
                  const USHORT usCouponNo );
    BOOL    Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    const DATE& getFromDate() const;
    const DATE& getToDate() const;
    DCURRENCY   getAmount() const;
    DSCOUNTER   getCounter() const;

private:
    BOOL    Read( const UCHAR uchClassToRead );
    BOOL    Read( const UCHAR uchClassToRead, const USHORT usRecordNo, const UCHAR  uchSubRecordNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );
    BOOL    Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK( const USHORT usRecordNo );

public:
    TTLCPN  m_ttlCoupon;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  RESERVED FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlCoupon::Read()
//===========================================================================

inline BOOL CTtlCoupon::Read(
    const UCHAR uchClassToRead
    )
{
    return ( CTotal::Read( uchClassToRead ));
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::Read()
//===========================================================================

inline BOOL CTtlCoupon::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::Reset()
//===========================================================================

inline BOOL CTtlCoupon::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::IsResetOK()
//===========================================================================

inline BOOL CTtlCoupon::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::ResetAsOK()
//===========================================================================

inline BOOL CTtlCoupon::ResetAsOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::ResetAsOK( usRecordNo ));
}

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlCoupon::getFromDate()
//===========================================================================

inline const DATE& CTtlCoupon::getFromDate() const
{
    return ( m_ttlCoupon.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::getToDate()
//===========================================================================

inline const DATE& CTtlCoupon::getToDate() const
{
    return ( m_ttlCoupon.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::getAmount()
//===========================================================================

inline DCURRENCY CTtlCoupon::getAmount() const
{
    return ( m_ttlCoupon.CpnTotal.lAmount );
}

//===========================================================================
//  FUNCTION :  CTtlCoupon::getCounter()
//===========================================================================

inline DSCOUNTER CTtlCoupon::getCounter() const
{
    return ( m_ttlCoupon.CpnTotal.sCounter );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLCOUPON_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TtlCoupon.h ) ////////////////////////////
