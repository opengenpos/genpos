/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlDept.h
//
//  PURPOSE:    Department Total class definitions and declarations.
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

#if !defined(AFX_TTLDEPT_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLDEPT_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlDept
//---------------------------------------------------------------------------

class CTtlDept : public CTotal
{
public:
    CTtlDept();
    ~CTtlDept();

    BOOL    Read( const UCHAR  uchClassToRead,
                  const USHORT usDeptNo );
    BOOL    Reset( const BOOL fIsEODReset = TRUE, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    const DATE& getFromDate() const;
    const DATE& getToDate() const;
    DCURRENCY   getAmount() const;
    LONG        getCounter() const;
	USHORT      getMajorDept() const;
	USHORT      getDept() const;

private:
    BOOL    Read( const UCHAR uchClassToRead );
    BOOL    Read( const UCHAR uchClassToRead, const USHORT usRecordNo, const UCHAR  uchSubRecordNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );
    BOOL    Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK( const USHORT usRecordNo );

public:
    TTLDEPT m_ttlDept;
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
//  FUNCTION :  CTtlDept::Read()
//===========================================================================

inline BOOL CTtlDept::Read(
    const UCHAR uchClassToRead
    )
{
    return ( CTotal::Read( uchClassToRead ));
}

//===========================================================================
//  FUNCTION :  CTtlDept::Read()
//===========================================================================

inline BOOL CTtlDept::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlDept::Reset()
//===========================================================================

inline BOOL CTtlDept::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlDept::IsResetOK()
//===========================================================================

inline BOOL CTtlDept::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlDept::ResetAsOK()
//===========================================================================

inline BOOL CTtlDept::ResetAsOK(
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
//  FUNCTION :  CTtlDept::getFromDate()
//===========================================================================

inline const DATE& CTtlDept::getFromDate() const
{
    return ( m_ttlDept.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlDept::getToDate()
//===========================================================================

inline const DATE& CTtlDept::getToDate() const
{
    return ( m_ttlDept.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlDept::getAmount()
//===========================================================================

inline DCURRENCY CTtlDept::getAmount() const
{
    return ( m_ttlDept.DEPTTotal.lAmount );
}

//===========================================================================
//  FUNCTION :  CTtlDept::getCounter()
//===========================================================================

inline LONG CTtlDept::getCounter() const
{
    return ( m_ttlDept.DEPTTotal.lCounter );
}

inline USHORT CTtlDept::getMajorDept() const
{
	return (m_ttlDept.uchMajorDEPTNo);
}

inline USHORT CTtlDept::getDept() const
{
	return (m_ttlDept.usDEPTNumber);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLDEPT_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

///////////////////// END OF FILE ( TtlDept.h ) /////////////////////////////
