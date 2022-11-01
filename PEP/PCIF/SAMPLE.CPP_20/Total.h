/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Total.h
//
//  PURPOSE:    Total class definitions and declarations.
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

#if !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//            C O N S T A N T    V A L U E s
//
/////////////////////////////////////////////////////////////////////////////

namespace pcttl
{
const USHORT LOCK_ALL_GC        = 0;
const USHORT RESET_ALL_TOTAL    = 0;
const UCHAR  RESET_ENABLED      = TTL_NOTRESET;

const USHORT NOT_EOD            = 0x0000;
const USHORT EOD_REGFIN         = static_cast< const USHORT >( ~0x0001 );
const USHORT EOD_DEPT           = static_cast< const USHORT >( ~0x0002 );
const USHORT EOD_PLU            = static_cast< const USHORT >( ~0x0004 );
const USHORT EOD_HOURLY         = static_cast< const USHORT >( ~0x0008 );
const USHORT EOD_COUPON         = static_cast< const USHORT >( ~0x0080 );
const USHORT EOD_INDFIN         = static_cast< const USHORT >( ~0x0100 );
const USHORT EOD_SERVTIME       = static_cast< const USHORT >( ~0x0200 );

const USHORT CASHIER_RESET      = 0x0000;
const USHORT INDFIN_RESET       = 0x0100;

const USHORT MAX_COUPON_NO      = 255;
const ULONG  MAX_CASHIER_NO     = 99999999;    // Max cashier number is now 8 digits for Amtrak
const USHORT MAX_GC_NO          = 9999;
const ULONG  MAX_ETK_NO         = 999999999;

const USHORT MAX_MISC_TENDER    = 17;
//const USHORT MAX_FOREIGN_TOTAL  = 2;	// ###DEL Saratoga
const USHORT MAX_FOREIGN_TOTAL  = 8;
const USHORT MAX_SERVICE_TOTAL  = 17;
const USHORT MAX_TAX_TYPE       = 4;
const USHORT MAX_BONUS_TYPE     = STD_NO_BONUS_TTL;
const USHORT MAX_HOURLY_BLOCK   = 48;
const USHORT MAX_SERVICE_BLOCK  = 3;
const USHORT MAX_COUPON         = 3;
const USHORT MAX_EJ_BUFFER      = ( CLI_MAX_EJDATA );
}

/////////////////////////////////////////////////////////////////////////////
//
//              B A S E    C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CTotal
//---------------------------------------------------------------------------

class CTotal
{
public:
    CTotal();
    virtual ~CTotal();

    virtual BOOL Read( const UCHAR  uchClassToRead, const USHORT usRecordNo, const UCHAR  uchSubRecordNo ) = 0;
    virtual BOOL Read( const UCHAR  uchClassToRead, const USHORT usRecordNo ) = 0;
    virtual BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo ) = 0;
    virtual BOOL Read( const UCHAR  uchClassToRead ) = 0;
    virtual BOOL Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass =CLASS_TTLEODRESET ) = 0;
    virtual BOOL Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass =CLASS_TTLEODRESET ) = 0;
    virtual BOOL IsResetOK( const USHORT usRecordNo ) = 0;
    virtual BOOL IsResetOK() = 0;
    virtual BOOL ResetAsOK( const USHORT usRecordNo ) = 0;
    virtual BOOL ResetAsOK() = 0;

    virtual SHORT GetLastError() const;

protected:
    BOOL    TotalRead( LPVOID lpvTotal );
	BOOL	TotalReadPluEx( LPVOID lpvTotal );
    BOOL    TotalReset( LPCVOID lpvTotal,
                        const USHORT usEODResetType );
    BOOL    TotalIsReportIssued( LPCVOID lpvTotal );
    BOOL    TotalReportAsIssued( LPCVOID lpvTotal );
    BOOL    TotalReadPlural( LPVOID lpvTotal );
    VOID    TotalGetCurrentDateTime( DATE& thisDate );

private:
    SHORT   m_sLastError;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTotal::GetLastError()
//===========================================================================

inline SHORT CTotal::GetLastError() const
{
    return ( m_sLastError );
}

//---------------------------------------------------------------------------
//
//  RESERVED FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTotal::Read()
//===========================================================================

inline BOOL CTotal::Read(
    const UCHAR uchClassToRead
    )
{
    UNREFERENCED_PARAMETER( uchClassToRead );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::Read()
//===========================================================================

inline BOOL CTotal::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo
    )
{
    UNREFERENCED_PARAMETER( uchClassToRead );
    UNREFERENCED_PARAMETER( usRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}

inline BOOL CTotal::Read(
    const UCHAR uchClassToRead,
    const ULONG ulRecordNo
    )
{
    UNREFERENCED_PARAMETER( uchClassToRead );
    UNREFERENCED_PARAMETER( ulRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::Read()
//===========================================================================

inline BOOL CTotal::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR  uchSubRecordNo
    )
{
    UNREFERENCED_PARAMETER( uchClassToRead );
    UNREFERENCED_PARAMETER( usRecordNo );
    UNREFERENCED_PARAMETER( uchSubRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}
//===========================================================================
//  FUNCTION :  CTotal::Reset()
//===========================================================================

inline BOOL CTotal::Reset(
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    UNREFERENCED_PARAMETER( fIsEODReset );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::Reset()
//===========================================================================

inline BOOL CTotal::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    UNREFERENCED_PARAMETER( fIsEODReset );
    UNREFERENCED_PARAMETER( usRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::IsResetOK()
//===========================================================================

inline BOOL CTotal::IsResetOK()
{
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::IsResetOK()
//===========================================================================

inline BOOL CTotal::IsResetOK(
    const USHORT usRecordNo
    )
{
    UNREFERENCED_PARAMETER( usRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::ResetAsOK()
//===========================================================================

inline BOOL CTotal::ResetAsOK()
{
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CTotal::ResetAsOK()
//===========================================================================

inline BOOL CTotal::ResetAsOK(
    const USHORT usRecordNo
    )
{
    UNREFERENCED_PARAMETER( usRecordNo );
    ASSERT( FALSE );
    return ( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
//
//            D E R I V E D    C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#include "TtlRegFin.h"
#include "TtlIndFin.h"
#include "TtlPlu.h"
#include "TtlDept.h"
#include "TtlHourly.h"
#include "TtlServTime.h"
#include "TtlCoupon.h"
#include "TtlCashier.h"

/////////////////////////////////////////////////////////////////////////////
//
//            O T H E R    C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#include "GuestCheck.h"
#include "TallyCPM.h"
#include "TtlETK.h"
#include "ReportEJ.h"
#include "ReportEOD.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////////// END OF FILE ( Total.h ) //////////////////////////////