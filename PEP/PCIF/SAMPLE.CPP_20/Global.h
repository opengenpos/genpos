/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Global.h
//
//  PURPOSE:    Global function and constant definitions and declarations.
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

#if !defined(AFX_GLOBAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_GLOBAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//            C O N S T A N T    V A L U E s
//
/////////////////////////////////////////////////////////////////////////////

namespace pcsample
{
    const UCHAR     CLASS_PARAMETER = 200;

    const USHORT    BY_INDFIN_RESET = 0x0100;

    const int ERR_SUCCESS           = 0;
    const int ERR_ALLLOCK           = 1;
    const int ERR_ISSUED            = 2;
    const int ERR_RESET             = 3;
    const int ERR_ALLIDREAD         = 4;
    const int ERR_TTLREAD           = 5;
    const int ERR_INDLOCK           = 6;
    const int ERR_INDUNLOCK         = 7;
    const int ERR_INDREAD           = 8;
    const int ERR_CHKEXIST          = 9;
    const int ERR_ISP               = 10;
    const int ERR_TTLISSRST         = 11;
    const int ERR_LOCK_TERMINAL     = 12;
    const int ERR_UNLOCK_TERMINAL   = 13;
    const int ERR_CANCEL            = 14;
    const int ERR_ALLREAD           = 15;
    const int ERR_ALLWRITE          = 16;
    const int ERR_PORTOPEN          = 17;
    const int ERR_PORTCLOSE         = 18;
    const int ERR_LOGON             = 19;
    const int ERR_LOGOFF            = 20;
    const int ERR_CHGPASSWORD       = 21;
	const int ERR_NVRCLKDIN			= 22;
	const int ERR_DNE				= 23;
    const int ERR_MAXITEMS          = 24;
	const int ERR_ALLUNLOCK         = 25;
	const int ERR_HEADREAD			= 26;
	const int ERR_LINEREAD			= 27;
}

/////////////////////////////////////////////////////////////////////////////
//
//            F U N C T I O N S
//
/////////////////////////////////////////////////////////////////////////////

namespace pcsample
{
    VOID AFX_CDECL  TraceFunction( LPCTSTR lpszFormat,
                                   ... );
 
    BOOL    ReadAllRecords( const UCHAR     uchClassToRead,
                            class CTtlPlu&  ttlPlu,
                            const USHORT    usDeptNo,
                                  BOOL&     fHeaderPrinted,
                            class CPCSampleDoc* const pDoc );

    BOOL    ReadAllRecords( const UCHAR     uchClassToRead,
                            class CTtlPlu&  ttlPlu,
                                  BOOL&     fHeaderPrinted,
                            class CPCSampleDoc* const pDoc );

    BOOL    ReadAllRecords( const UCHAR         uchClassToRead,
                            class CTtlCoupon&   ttlCoupon,
                            class CPCSampleDoc* const pDoc );

	//ESMITH SR275
	BOOL	ReadPluEx( class CTtlPlu&	TtlPlu,
					   const UCHAR		uchClassToRead,
                       class CPCSampleDoc* const pDoc );
}

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N S
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CMyString
//---------------------------------------------------------------------------

class CMyString : public CString
{
public:
    void FormatV( LPCTSTR lpszFormat, va_list argList );
};

inline void CMyString::FormatV( LPCTSTR lpszFormat, va_list argList )
{
	CString::FormatV( lpszFormat, argList );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////////// END OF FILE ( Global.h ) /////////////////////////////