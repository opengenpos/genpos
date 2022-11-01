/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ReportEJ.h
//
//  PURPOSE:    Elecronic Journal class definitions and declarations.
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

#if !defined(AFX_REPORTEJ_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_REPORTEJ_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
	#error include 'Total.h' before including this file
#endif

//---------------------------------------------------------------------------
//  CReportEJ
//---------------------------------------------------------------------------

class CReportEJ
{
public:
    CReportEJ();
    ~CReportEJ();

	VOID    Initialize(LPCTSTR mySubTitle = 0);
    BOOL    GetBlockCount( USHORT& usNoOfBlocks );
    BOOL    Read( BOOL& fIsEndOfFile );
    BOOL    Reset();
    BOOL    ReadAndResetNonOverride( BOOL& fIsEndOfFile );
    BOOL    ReadAndResetOverride( BOOL& fIsEndOfFile );
	BOOL	ReadSetHeader( BOOL& fIsEndOfFile );

    SHORT   GetLastError() const;

    ULONG   getTotalReadSize() const;
    USHORT  getCurrentBlockSize() const;
	USHORT  getCurrentConsecutiveNumber () const;
	ULONG   getTotalBytesRead() const;
    USHORT  getLinesOfEJ() const;
    LPSTR  getEJLine( const USHORT usLineNo,
                       LPSTR lpszEJLine,
                       BOOL&  fIsEJTHeader );
	LPSTR	getEJLine( LPSTR lpszEJLine, BOOL& fSuccess  );
	ULONG	getTotalLines() const;
	BOOL	getMoreEJLines() const;

	USHORT   m_usTerminalNo;
	USHORT   m_usLineNo;
	CString  m_ReportSubTitle;

private:
    UCHAR   m_auchEJBuffer[ pcttl::MAX_EJ_BUFFER ];
    ULONG   m_ulTtlReadSize;
    ULONG   m_ulReadOffset;
    ULONG   m_usBytesRead;
    USHORT  m_usNoOfReadBlocks;
    EJT_HEADER  m_ejtHeader;
    USHORT  m_usReadEJTrans;

    SHORT   m_sLastError;

	ULONG	m_ulTtlHeader;
	ULONG	m_ulTtlEJLines;
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
//  FUNCTION :  CReportEJ::getMoreEJLines()
//===========================================================================

inline BOOL CReportEJ::getMoreEJLines() const
{
    return ( ((m_ejtHeader.usCVLI - m_usReadEJTrans) > 0) ? TRUE : FALSE );
}

//===========================================================================
//  FUNCTION :  CReportEJ::GetLastError()
//===========================================================================

inline SHORT CReportEJ::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CReportEJ::getTotalReadSize()
//===========================================================================

inline ULONG CReportEJ::getTotalReadSize() const
{
    return ( m_ulTtlReadSize );
}

//===========================================================================
//  FUNCTION :  CReportEJ::getLinesOfEJ()
//===========================================================================

inline USHORT CReportEJ::getLinesOfEJ() const
{
    return ( static_cast< USHORT >( m_usBytesRead / EJ_COLUMN ));
}

//===========================================================================
//  FUNCTION :  CReportEJ::getLinesOfEJ()
//===========================================================================

inline USHORT CReportEJ::getCurrentBlockSize() const
{
    return ( m_ejtHeader.usCVLI );
}

inline USHORT CReportEJ::getCurrentConsecutiveNumber () const
{
    return ( m_ejtHeader.usConsecutive );
}

//===========================================================================
//  FUNCTION :  CReportEJ::getLinesOfEJ()
//===========================================================================

inline ULONG CReportEJ::getTotalBytesRead() const
{
    return ( m_usBytesRead );
}

//===========================================================================
//  FUNCTION :  CReportEJ::getTotalLines()
//===========================================================================

inline ULONG CReportEJ::getTotalLines() const
{
    return ( (m_ulTtlHeader + m_ulTtlEJLines) );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTEJ_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( ReportEJ.h ) ///////////////////////////
