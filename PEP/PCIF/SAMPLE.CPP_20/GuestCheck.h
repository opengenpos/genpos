/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  GuestCheck.h
//
//  PURPOSE:    Guest Check class definitions and declarations.
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

#if !defined(AFX_GUESTCHECK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_GUESTCHECK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CGuestCheck
//---------------------------------------------------------------------------

class CGuestCheck
{
public:
    CGuestCheck();
    ~CGuestCheck();

    BOOL    Read( const USHORT usGCNo );
    BOOL    Reset( const USHORT usGCNo, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    GetAllGCNo( USHORT* pausGCNo, const USHORT usBufferSize, USHORT& usNoOfGC );
    BOOL    GetAllGCNoByOperator( const ULONG ulOperatorNo, USHORT* pausGCNo, const USHORT usBufferSize, USHORT& usNoOfGC );
    BOOL    ReadAndLock( const USHORT usGCNo );
    BOOL    Lock( const USHORT usGCNo );
    BOOL    Unlock();
    BOOL    CloseOpenedGC( const USHORT usGCNo );
    BOOL    ResetDeliveryQueue();
    BOOL    IsEmpty();

    SHORT   GetLastError() const;

    USHORT  getGuestCheckNo() const;
    UCHAR   getCheckDigit() const;
    ULONG   getCashierNo() const;
    UCHAR   getCheckOpenHour() const;
    UCHAR   getCheckOpenMinute() const;
    USHORT  getConsNo() const;
    USHORT  getTableNo() const;
    USHORT  getNoOfPerson() const;
    ULONG   getPrevCashierNo() const;
    LONG    getTransfferedTotal() const;
    LONG    getCurrentTotal() const;

private:
    SHORT   m_sLastError;
    BOOL    m_fLocked;
    USHORT  m_usLockedGCNo;
    TRANGCFQUAL     m_tranGCF;
};

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CGuestCheck::GetLastError()
//===========================================================================

inline SHORT CGuestCheck::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getGuestCheckNo()
//===========================================================================

inline USHORT CGuestCheck::getGuestCheckNo() const
{
    return ( m_tranGCF.usGuestNo );
}
inline USHORT CGuestCheck::getConsNo() const
{
    return ( m_tranGCF.usConsNo );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getCheckDigit()
//===========================================================================

inline UCHAR CGuestCheck::getCheckDigit() const
{
    return ( m_tranGCF.uchCdv );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getCashierNo()
//===========================================================================

inline ULONG CGuestCheck::getCashierNo() const
{
    return ( m_tranGCF.ulCashierID );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getCheckOpenHour()
//===========================================================================

inline UCHAR CGuestCheck::getCheckOpenHour() const
{
    return ( m_tranGCF.auchCheckOpen[ 0 ] );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getCheckOpenMinute()
//===========================================================================

inline UCHAR CGuestCheck::getCheckOpenMinute() const
{
    return ( m_tranGCF.auchCheckOpen[ 1 ] );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getTableNo()
//===========================================================================

inline USHORT CGuestCheck::getTableNo() const
{
    return ( m_tranGCF.usTableNo );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getNoOfPerson()
//===========================================================================

inline USHORT CGuestCheck::getNoOfPerson() const
{
    return ( m_tranGCF.usTableNo );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getPrevCashierNo()
//===========================================================================

inline ULONG CGuestCheck::getPrevCashierNo() const
{
    return ( m_tranGCF.ulPrevCashier );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getTransfferedTotal()
//===========================================================================

inline LONG CGuestCheck::getTransfferedTotal() const
{
    return ( m_tranGCF.lTranBalance );
}

//===========================================================================
//  FUNCTION :  CGuestCheck::getCurrentTotal()
//===========================================================================

inline LONG CGuestCheck::getCurrentTotal() const
{
    return ( m_tranGCF.lCurBalance );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUESTCHECK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( GuesetCheck.h ) //////////////////////////
