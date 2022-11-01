/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlIndFin.h
//
//  PURPOSE:    Individual Register Financial Total class definitions
//              and declarations.
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

#if !defined(AFX_TTLINDFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLINDFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlIndFin
//---------------------------------------------------------------------------

class CTtlIndFin : public CTtlRegFin
{
public:
    CTtlIndFin();
    ~CTtlIndFin();

    BOOL Read( const UCHAR  uchClassToRead, const USHORT usRecordNo );
    BOOL Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL IsResetOK( const USHORT usTerminalNo );
    BOOL ResetAsOK( const USHORT usTerminalNo );

private:
    BOOL Read( const UCHAR  uchClassToRead );
    BOOL Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL IsResetOK();
    BOOL ResetAsOK();
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
//  FUNCTION :  CTtlIndFin::Read()
//===========================================================================

inline BOOL CTtlIndFin::Read(
    const UCHAR uchClassToRead
    )
{
    return ( CTotal::Read( uchClassToRead ));
}

//===========================================================================
//  FUNCTION :  CTtlIndFin::Reset()
//===========================================================================

inline BOOL CTtlIndFin::Reset(
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlIndFin::IsResetOK()
//===========================================================================

inline BOOL CTtlIndFin::IsResetOK()
{
    return ( CTotal::IsResetOK());
}

//===========================================================================
//  FUNCTION :  CTtlIndFin::ResetAsOK()
//===========================================================================

inline BOOL CTtlIndFin::ResetAsOK()
{
    return ( CTotal::ResetAsOK());
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLINDFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

/////////////////// END OF FILE ( TtlRegFin.h ) /////////////////////////////