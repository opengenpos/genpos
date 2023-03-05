/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlServTime.h
//
//  PURPOSE:    Service Time Total class definitions and declarations.
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

#if !defined(AFX_TTLSERVTIME_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLSERVTIME_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlServTime
//---------------------------------------------------------------------------

class CTtlServTime : public CTotal
{
public:
    CTtlServTime();
    ~CTtlServTime();

    BOOL    Read( const UCHAR  uchClassToRead );
    BOOL    Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    const DATE& getFromDate() const;
    const DATE& getToDate() const;

    DSCOUNTER   getServiceCount( const USHORT usHourlyBlock,
                                 const USHORT usServiceBlock ) const;
    DCURRENCY   getServiceTime ( const USHORT usHourlyBlock,
                                 const USHORT usServiceBlock ) const;

private:
    BOOL    Read( const UCHAR uchClassToRead,
                  const USHORT usRecordNo );
    BOOL    Read( const UCHAR uchClassToRead,
                  const USHORT usRecordNo,
                  const UCHAR  uchSubRecordNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );
    BOOL    Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK( const USHORT usRecordNo );

public:
    TTLSERTIME  m_ttlServTime;
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
//  FUNCTION :  CTtlServTime::Read()
//===========================================================================

inline BOOL CTtlServTime::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo ));
}

inline BOOL CTtlServTime::Read(
    const UCHAR uchClassToRead,
    const ULONG ulRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, ulRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlServTime::Read()
//===========================================================================

inline BOOL CTtlServTime::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR  uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlServTime::Reset()
//===========================================================================

inline BOOL CTtlServTime::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlServTime::IsResetOK()
//===========================================================================

inline BOOL CTtlServTime::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlServTime::ResetAsOK()
//===========================================================================

inline BOOL CTtlServTime::ResetAsOK(
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
//  FUNCTION :  CTtlServTime::getFromDate()
//===========================================================================

inline const DATE& CTtlServTime::getFromDate() const
{
    return ( m_ttlServTime.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlServTime::getToDate()
//===========================================================================

inline const DATE& CTtlServTime::getToDate() const
{
    return ( m_ttlServTime.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlServTime::getServiceCount()
//===========================================================================

inline DSCOUNTER CTtlServTime::getServiceCount(
    const USHORT usHourlyBlock,
    const USHORT usServiceBlock
    ) const
{
    ASSERT( usHourlyBlock < pcttl::MAX_HOURLY_BLOCK );
    ASSERT( usServiceBlock < pcttl::MAX_SERVICE_BLOCK );

    return ( m_ttlServTime.ServiceTime[ usHourlyBlock ][ usServiceBlock ].sCounter );
}

//===========================================================================
//  FUNCTION :  CTtlServTime::getServiceTime()
//===========================================================================

inline DCURRENCY CTtlServTime::getServiceTime(
    const USHORT usHourlyBlock,
    const USHORT usServiceBlock
    ) const
{
    ASSERT( usHourlyBlock < pcttl::MAX_HOURLY_BLOCK );
    ASSERT( usServiceBlock < pcttl::MAX_SERVICE_BLOCK );

    return ( m_ttlServTime.ServiceTime[ usHourlyBlock ][ usServiceBlock ].lAmount );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLSERVTIME_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TtlServTime.h ) //////////////////////////
