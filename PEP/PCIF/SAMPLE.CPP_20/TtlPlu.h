/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlPlu.h
//
//  PURPOSE:    PLU Total class definitions and declarations.
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

#if !defined(AFX_TTLPLU_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLPLU_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlPlu
//---------------------------------------------------------------------------

class CTtlPlu : public CTotal
{
public:
    CTtlPlu();
    ~CTtlPlu();

    BOOL    Read( const UCHAR   uchClassToRead,
                  const USHORT  usPluNo,
                  const UCHAR   uchAdjectiveNo );



    BOOL    Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    BOOL    ReadPlural( const UCHAR  uchClassToRead,
                        const TCHAR  uchPluNo[][OP_PLU_LEN],
                        const UCHAR  uchAdjectiveNo[] );

	BOOL	ReadEx( const UCHAR uchClassToRead );

    const DATE& getExFromDate() const;
    const DATE& getExToDate() const;
	DCURRENCY	getTtlAmtEx() const;
	LONG		getTtlCntEx() const;
    DCURRENCY   getAmountEx( const USHORT usIndex = 0 ) const;
    LONG		getCounterEx( const USHORT usIndex = 0 ) const;
	UCHAR		getAdjectNoEx( const USHORT usIndex = 0 ) const;
	LPTSTR		getPluNoEx( const USHORT usIndex = 0 ) const;

    const DATE& getFromDate( const USHORT usIndex = 0 ) const;
    const DATE& getToDate( const USHORT usIndex = 0 ) const;
    DCURRENCY   getAmount( const USHORT usIndex = 0 ) const;
    LONG	    getCounter( const USHORT usIndex = 0 ) const;

	BOOL        m_First;

private:
    BOOL    Read( const UCHAR uchClassToRead );
    BOOL    Read( const UCHAR uchClassToRead,
                  const USHORT usRecordNo );
    BOOL    Read( const UCHAR   uchClassToRead,
                  const TCHAR*  uchPluNo,
                  const UCHAR   uchAdjectiveNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );
    BOOL    Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK( const USHORT usRecordNo );

public:
    TTLPLU  m_ttlPlu[ CLI_PLU_MAX_NO ];
	TTLPLUEX m_ttlPluEx;
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
//  FUNCTION :  CTtlPlu::Read()
//===========================================================================

inline BOOL CTtlPlu::Read(
    const UCHAR uchClassToRead
    )
{
    return ( CTotal::Read( uchClassToRead ));
}

//===========================================================================
//  FUNCTION :  CTtlPlu::Read()
//===========================================================================

inline BOOL CTtlPlu::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo ));
}

inline BOOL CTtlPlu::Read(
    const UCHAR uchClassToRead,
    const ULONG ulRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, ulRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlPlu::Reset()
//===========================================================================

inline BOOL CTtlPlu::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlPlu::IsResetOK()
//===========================================================================

inline BOOL CTtlPlu::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlPlu::ResetAsOK()
//===========================================================================

inline BOOL CTtlPlu::ResetAsOK(
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
//  FUNCTION :  CTtlPlu::getExFromDate()
//===========================================================================

inline const DATE& CTtlPlu::getExFromDate() const
{
    return ( m_ttlPluEx.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getExToDate()
//===========================================================================

inline const DATE& CTtlPlu::getExToDate() const
{
    return ( m_ttlPluEx.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getAmountEx()
//===========================================================================

inline DCURRENCY CTtlPlu::getAmountEx(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_EX_MAX_NO );

    return ( m_ttlPluEx.Plu[ usIndex ].PLUTotal.lAmount );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getTtlAmtEx()
//===========================================================================

inline DCURRENCY CTtlPlu::getTtlAmtEx() const
{
    return ( m_ttlPluEx.PLUAllTotal.lAmount );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getTtlAmtEx()
//===========================================================================

inline LONG CTtlPlu::getTtlCntEx() const
{
    return ( m_ttlPluEx.PLUAllTotal.lCounter );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getCounterEx()
//===========================================================================

inline LONG CTtlPlu::getCounterEx(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_EX_MAX_NO );

    return ( m_ttlPluEx.Plu[ usIndex ].PLUTotal.lCounter );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getAdjectNoEx()
//===========================================================================

inline UCHAR CTtlPlu::getAdjectNoEx(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_EX_MAX_NO );

    return ( m_ttlPluEx.Plu[ usIndex ].uchAdjectNo );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getPluNoEx()
//===========================================================================

inline LPTSTR CTtlPlu::getPluNoEx(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_EX_MAX_NO );

    return ( (LPTSTR) m_ttlPluEx.Plu[ usIndex ].auchPLUNumber );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getFromDate()
//===========================================================================

inline const DATE& CTtlPlu::getFromDate(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

    return ( m_ttlPlu[ usIndex ].FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getToDate()
//===========================================================================

inline const DATE& CTtlPlu::getToDate(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

    return ( m_ttlPlu[ usIndex ].ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getAmount()
//===========================================================================

inline DCURRENCY CTtlPlu::getAmount(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

    return ( m_ttlPlu[ usIndex ].PLUTotal.lAmount );
}

//===========================================================================
//  FUNCTION :  CTtlPlu::getCounter()
//===========================================================================

inline LONG CTtlPlu::getCounter(
    const USHORT usIndex
    ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

    return ( m_ttlPlu[ usIndex ].PLUTotal.lCounter );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLPLU_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////////// END OF FILE ( TtlPlu.h ) /////////////////////////////
