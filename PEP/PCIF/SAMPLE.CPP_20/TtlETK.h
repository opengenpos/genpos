/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlETK.h
//
//  PURPOSE:    ETK Total class definitions and declarations.
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

#if !defined(AFX_TTLETK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLETK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlETK
//---------------------------------------------------------------------------

class CTtlETK
{
public:
    CTtlETK();
    ~CTtlETK();

    BOOL    Read( const UCHAR uchClassToRead,
                  const ULONG ulETKNo,
                  USHORT& usFieldNo,
                  BOOL& fIsEndOfField );
    BOOL    Reset();
    BOOL    GetAllETKNo( const UCHAR uchClassToRead,
                         ULONG* aulETKNo,
                         const USHORT usBufferSize,
                         USHORT& usNoOfETK );
    BOOL    Lock();
    BOOL    Unlock();
    BOOL    IsResetOK();
    BOOL    ResetAsOK();

    SHORT   GetLastError() const;

    ULONG   getEmployeeNo() const;
    USHORT  getFieldNo() const;
    UCHAR   getJobCode() const;
    USHORT  getTimeInYear() const;
    USHORT  getTimeInMonth() const;
    USHORT  getTimeInDay() const;
    USHORT  getTimeInHour() const;
    USHORT  getTimeInMinute() const;
    USHORT  getTimeOutHour() const;
    USHORT  getTimeOutMinute() const;
    USHORT  getFromMonth() const;
    USHORT  getFromDay() const;
    USHORT  getFromHour() const;
    USHORT  getFromMinute() const;
    USHORT  getToMonth() const;
    USHORT  getToDay() const;
    USHORT  getToHour() const;
    USHORT  getToMinute() const;

private:

    VOID    GetCurrentDateTime( ETK_TIME& thisDate );

private:
    SHORT   m_sLastError;
    ULONG   m_ulEmployeeNo;
    USHORT  m_usFieldNo;
    ETK_FIELD   m_fieldETK;
    ETK_TIME    m_timeETK;
};

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlETK::GetLastError()
//===========================================================================

inline SHORT CTtlETK::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getEmployeeNo()
//===========================================================================

inline ULONG CTtlETK::getEmployeeNo() const
{
    return ( m_ulEmployeeNo );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getFieldNo()
//===========================================================================

inline USHORT CTtlETK::getFieldNo() const
{
    return ( m_usFieldNo );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getJobCode()
//===========================================================================

inline UCHAR CTtlETK::getJobCode() const
{
    return ( m_fieldETK.uchJobCode );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeInYear()
//===========================================================================

inline USHORT CTtlETK::getTimeInYear() const
{
    return ( m_fieldETK.usYear );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeInMonth()
//===========================================================================

inline USHORT CTtlETK::getTimeInMonth() const
{
    return ( m_fieldETK.usMonth );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeInDay()
//===========================================================================

inline USHORT CTtlETK::getTimeInDay() const
{
    return ( m_fieldETK.usDay );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeInHour()
//===========================================================================

inline USHORT CTtlETK::getTimeInHour() const
{
    return ( m_fieldETK.usTimeinTime );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeInMinute()
//===========================================================================

inline USHORT CTtlETK::getTimeInMinute() const
{
    return ( m_fieldETK.usTimeinMinute );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeoutHour()
//===========================================================================

inline USHORT CTtlETK::getTimeOutHour() const
{
    return ( m_fieldETK.usTimeOutTime );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getTimeoutMinute()
//===========================================================================

inline USHORT CTtlETK::getTimeOutMinute() const
{
    return ( m_fieldETK.usTimeOutMinute );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getFromMonth()
//===========================================================================

inline USHORT CTtlETK::getFromMonth() const
{
    return ( m_timeETK.usFromMonth );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getFromDay()
//===========================================================================

inline USHORT CTtlETK::getFromDay() const
{
    return ( m_timeETK.usFromDay );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getFromHour()
//===========================================================================

inline USHORT CTtlETK::getFromHour() const
{
    return ( m_timeETK.usFromTime );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getFromMinute()
//===========================================================================

inline USHORT CTtlETK::getFromMinute() const
{
    return ( m_timeETK.usFromMinute );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getToMonth()
//===========================================================================

inline USHORT CTtlETK::getToMonth() const
{
    return ( m_timeETK.usToMonth );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getToDay()
//===========================================================================

inline USHORT CTtlETK::getToDay() const
{
    return ( m_timeETK.usToDay );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getToHour()
//===========================================================================

inline USHORT CTtlETK::getToHour() const
{
    return ( m_timeETK.usToTime );
}

//===========================================================================
//  FUNCTION :  CTtlETK::getToMinute()
//===========================================================================

inline USHORT CTtlETK::getToMinute() const
{
    return ( m_timeETK.usToMinute );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLETK_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TtkETK.h ) ///////////////////////////////
