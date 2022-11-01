/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlHourly.h
//
//  PURPOSE:    Hourly Activity Total class definitions and declarations.
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

#if !defined(AFX_TTLHOURLY_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLHOURLY_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlHourly
//---------------------------------------------------------------------------

class CTtlHourly : public CTotal
{
public:
    CTtlHourly();
    ~CTtlHourly();

    BOOL    Read( const UCHAR  uchClassToRead );
    BOOL    Reset( const BOOL fIsEODReset = TRUE, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    const DATE& getFromDate() const;
    const DATE& getToDate() const;
    LONG        getItemProductivity( const USHORT usHourlyBlock ) const;
    SHORT       getNoOfPerson( const USHORT usHourlyBlock ) const;
    DCURRENCY   getHourlyTotal( const USHORT usHourlyBlock ) const;

private:
    BOOL    Read( const UCHAR uchClassToRead,
                  const USHORT usRecordNo );
    BOOL    Read( const UCHAR uchClassToRead,
                  const USHORT usRecordNo,
                  const UCHAR  uchSubRecordNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );
    BOOL    Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK( const USHORT usRecordNO );

public:
    TTLHOURLY   m_ttlHourly;
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
//  FUNCTION :  CTtlHourly::Read()
//===========================================================================

inline BOOL CTtlHourly::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo ));
}

inline BOOL CTtlHourly::Read(
    const UCHAR uchClassToRead,
    const ULONG ulRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, ulRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlHourly::Read()
//===========================================================================

inline BOOL CTtlHourly::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR  uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlHourly::Reset()
//===========================================================================

inline BOOL CTtlHourly::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlHourly::IsResetOK()
//===========================================================================

inline BOOL CTtlHourly::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlHourly::ResetAsOK()
//===========================================================================

inline BOOL CTtlHourly::ResetAsOK(
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
//  FUNCTION :  CTtlHourly::getFromDate()
//===========================================================================

inline const DATE& CTtlHourly::getFromDate() const
{
    return ( m_ttlHourly.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlHourly::getToDate()
//===========================================================================

inline const DATE& CTtlHourly::getToDate() const
{
    return ( m_ttlHourly.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlHourly::getHourlyTotal()
//===========================================================================

inline DCURRENCY CTtlHourly::getHourlyTotal(
    const USHORT usHourlyBlock
    ) const
{
    ASSERT( usHourlyBlock < pcttl::MAX_HOURLY_BLOCK );

    return ( m_ttlHourly.Total[ usHourlyBlock ].lHourlyTotal );
}

//===========================================================================
//  FUNCTION :  CTtlHourly::getItemProductivity()
//===========================================================================

inline LONG CTtlHourly::getItemProductivity(
    const USHORT usHourlyBlock
    ) const
{
    ASSERT( usHourlyBlock < pcttl::MAX_HOURLY_BLOCK );

    return ( m_ttlHourly.Total[ usHourlyBlock ].lItemproductivityCount );
}

//===========================================================================
//  FUNCTION :  CTtlHourly::getNoOfPerson()
//===========================================================================

inline SHORT CTtlHourly::getNoOfPerson(
    const USHORT usHourlyBlock
    ) const
{
    ASSERT( usHourlyBlock < pcttl::MAX_HOURLY_BLOCK );

    return ( m_ttlHourly.Total[ usHourlyBlock ].sDayNoOfPerson );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLHOURLY_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TtlHourly.h ) ////////////////////////////
