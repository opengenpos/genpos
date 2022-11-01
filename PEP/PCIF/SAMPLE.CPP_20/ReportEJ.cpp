/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ReportEJ.cpp
//
//  PURPOSE:    Provides Electronic Journal report class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Global.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////
extern USHORT readOffset;
//===========================================================================
//
//  FUNCTION :  CReportEJ::CReportEJ()
//
//  PURPOSE :   Constructor of Electronic Journal report class.
//
//  RETURN :    No return value.
//
//===========================================================================

CReportEJ::CReportEJ() :
	m_usTerminalNo(0),
	m_usLineNo(0),
    m_ulTtlReadSize( 0 ),
    m_ulReadOffset( 0 ),
    m_usBytesRead( 0 ),
    m_usNoOfReadBlocks( 0 ),
    m_usReadEJTrans( 0 ),
    m_sLastError( SUCCESS ),
	m_ulTtlHeader( 0 ),
	m_ulTtlEJLines( 0 )
{
	memset( m_auchEJBuffer, 0, sizeof(m_auchEJBuffer));
	memset (&m_ejtHeader, 0, sizeof(m_ejtHeader));
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::~CReportEJ()
//
//  PURPOSE :   Destructor of Electronic Journal report class.
//
//  RETURN :    No return value.
//
//===========================================================================

CReportEJ::~CReportEJ()
{
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::Initialize()
//
//  PURPOSE :   Initialize all of member variables.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEJ::Initialize(LPCTSTR mySubTitle)
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::Initialize()") );

    m_ulTtlReadSize     = 0;
    m_ulReadOffset      = 0;
    m_usBytesRead       = 0;
    m_usNoOfReadBlocks  = 0;
    m_usReadEJTrans     = 0;
	m_usTerminalNo      = 0;
	m_usLineNo          = 1;
    m_sLastError        = SUCCESS;

	if (mySubTitle)
		m_ReportSubTitle   = mySubTitle;
	else
		m_ReportSubTitle    = _T("");

    memset( m_auchEJBuffer, 0, sizeof( m_auchEJBuffer) );
    memset( &m_ejtHeader, 0, sizeof( m_ejtHeader    ));

    pcsample::TraceFunction( _T("END   >> CReportEJ::Initialize() = VOID") );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::GetBlockCount()
//
//  PURPOSE :   Get number of blocks in electronic journal.
//
//  RETURN :    TRUE    - Number of blocks is retrieved.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEJ::GetBlockCount(
    USHORT& usNoOfBlocks        // variable to recevive no. of blocks
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::GetBlockCount()") );

    SHORT   sResult;

    // --- get number of blocks in 2170 via PC Interface function ---
    sResult = ::SerEJNoOfTrans();

    CString str;
    str.Format( _T("\t::SerEJNoOfTrans() - Called (%d)"), sResult );
    pcsample::TraceFunction( str );

    if ( 0 <= sResult )
    {
        // --- save no. of blocks data to user specified variable ---
        usNoOfBlocks = sResult;
        m_sLastError = EJ_PERFORM;
    }
    else
    {
		usNoOfBlocks = 0;
        m_sLastError = sResult;
    }

    pcsample::TraceFunction( _T("END   >> CReportEJ::GetBlockCount() = %d, usNoOfBlocks %d"), ( m_sLastError == SUCCESS ), usNoOfBlocks);

    return ( m_sLastError == EJ_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::Read()
//
//  PURPOSE :   Read a block data in electronic journal.
//
//  RETURN :    TRUE    - Electronic Journal block data is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEJ::Read(
    BOOL& fIsEndOfFile          // indicates end of file reached or not
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::Read()") );

    // --- read a block data sequencially in 2170 with terminal lock ---
    m_sLastError = ::SerEJReadFile( m_ulReadOffset, m_auchEJBuffer, sizeof( m_auchEJBuffer ), &m_usBytesRead );

    CString str;
    str.Format( _T("\t::SerEJReadFile() - Called (%d)"), m_sLastError );
    str += CString( "...Read Offset#%ld, Actual Read#%d" );
    pcsample::TraceFunction( str, m_ulReadOffset, m_usBytesRead );

    switch ( m_sLastError )
    {
    case EJ_PERFORM:
    case EJ_EOF:
        // --- update read offset for next read function call ---
		readOffset = 0;
        // --- determine whether end of file is reached or not ---
        fIsEndOfFile = ( m_sLastError == EJ_EOF ) ? TRUE : FALSE;
		if (m_usBytesRead > 0)
			m_sLastError = EJ_PERFORM;
		else
			m_sLastError = EJ_EOF;

        break;

    default:
        break;
    }

    pcsample::TraceFunction( _T("END   >> CReportEJ::Read() = %d"), ( m_sLastError == EJ_PERFORM ));

    return ( m_sLastError == EJ_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::Reset()
//
//  PURPOSE :   Reset (clear) the electronic journal in 2170.
//
//  RETURN :    TRUE    - Electronic Journal is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEJ::Reset()
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::Reset()") );

    // --- reset the electronic journal with terminal lock ---
    m_sLastError = ::SerEJClear();

    CString str;
    str.Format( _T("\t::SerEJClear() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CReportEJ::Reset() = %d"), ( m_sLastError == EJ_PERFORM ));

    return ( m_sLastError == EJ_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::ReadAndResetOverride()
//
//  PURPOSE :   Read and reset override type electronic journal.
//
//  RETURN :    TRUE    - Block data is read and resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEJ::ReadAndResetOverride(
    BOOL& fIsEndOfFile          // indicates end of file is reached or not
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::ReadAndResetOverride()") );
	CString str;
    // --- read and reset electronic journal without lock terminal ---
    SHORT   sNoOfTrans;

	ULONG  ulReadOffset = m_ulReadOffset;

	// Read and reset the Electronic Journal a block at a time.
	// We might get a repeat of an EJ block which is detected by receiving
	// a message containing a block with an offset that is non-zero, indicating there is
	// more data to read and the offset is one we have already processed.
	// In the case of duplicated blocks we just discard the duplicates.
	do {
		sNoOfTrans = ::SerEJReadResetTransOver( &ulReadOffset, m_auchEJBuffer, sizeof( m_auchEJBuffer ), &m_usBytesRead );
	} while (sNoOfTrans >= 0 && ulReadOffset != 0 && ulReadOffset <= m_ulReadOffset);
	m_ulReadOffset = ulReadOffset;

    str.Format( _T("\t::SerEJReadResetTransOver() - Called (%d)"), m_sLastError );
    str += CString( "...Read Offset#%ld, Actual Read#%d" );
    pcsample::TraceFunction( str, m_ulReadOffset, m_usBytesRead );

    if (( 0 <= sNoOfTrans ) || ( sNoOfTrans == EJ_END ))
    {
        // --- update total no. of read size ---
        m_ulTtlReadSize += m_usBytesRead;
        // --- determine whether end of file is reached or not ---
        fIsEndOfFile = ( sNoOfTrans == EJ_END ) ? TRUE : FALSE;
        m_sLastError = EJ_PERFORM;
    }
    else
    {
        // --- the function is failed, and store its error code ---
        m_sLastError = sNoOfTrans;
		if (sNoOfTrans == EJ_PROHIBIT) {
			m_ulReadOffset = 0;
			m_sLastError = EJ_PERFORM;
		}
    }

    pcsample::TraceFunction( _T("END   >> CReportEJ::ReadAndResetOverride() = %d"), ( m_sLastError == EJ_PERFORM ));

    return ( m_sLastError == EJ_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::ReadSetHeader()
//
//  PURPOSE :   Read and set header information
//
//  RETURN :    TRUE    - Block data is read and resetted.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CReportEJ::ReadSetHeader( 
	BOOL& fIsEndOfFile )
{

    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::ReadSetHeader()") );

    m_sLastError = ::SerEJReadFile( m_ulReadOffset, m_auchEJBuffer, sizeof( m_auchEJBuffer ), &m_usBytesRead );

    CString str;
    str.Format( _T("\t::SerEJReadFile() - Called (%d)"), m_sLastError );
    str += CString( "...Read Offset#%ld, Actual Read#%d" );
    pcsample::TraceFunction( str, m_ulReadOffset, m_usBytesRead );

	if( m_usBytesRead && ( (m_sLastError == EJ_PERFORM) || (m_sLastError == EJ_EOF)))
	{
		USHORT prevConsecutive = m_ejtHeader.usConsecutive;
		// --- set ej header
		memcpy( &m_ejtHeader, m_auchEJBuffer, sizeof( EJT_HEADER ));
		fIsEndOfFile = (( m_sLastError == EJ_EOF ) && (m_usBytesRead <= m_ejtHeader.usCVLI)) ? TRUE : FALSE;

        // --- update read offset for next read function call ---
		m_usReadEJTrans = sizeof( EJT_HEADER ); //Read into current EJ Transaction
		m_ulReadOffset += sizeof(EJT_HEADER ); //Read into EJ File on Terminal
		readOffset = sizeof( EJT_HEADER ); //Read offset into data recieved
		m_ulTtlReadSize += sizeof(EJT_HEADER );

		m_ulTtlHeader++;
		if (m_ejtHeader.usConsecutive != prevConsecutive) m_usLineNo =  1;
		m_sLastError = EJ_PERFORM;
	}
	else
		m_sLastError = EJ_EOF;

    pcsample::TraceFunction( _T("END   >> CReportEJ::ReadSetHeader() = %d"), ( m_sLastError == EJ_PERFORM ));

    return (m_sLastError == EJ_PERFORM);
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::ReadAndResetNonOverride()
//
//  PURPOSE :   Read and reset non-override type electronic journal.
//
//  RETURN :    TRUE    - Block data is read and resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEJ::ReadAndResetNonOverride(
    BOOL& fIsEndOfFile          // indicate end of file is reached or not
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEJ::ReadAndReset()") );
    CString str;

    // --- read and reset electronic journal without lock terminal ---
    if ( 0 < m_ulTtlReadSize )
    {
        m_sLastError = ::SerEJReadResetTrans( &m_ulReadOffset, m_auchEJBuffer, sizeof( m_auchEJBuffer ), &m_usBytesRead, &m_usNoOfReadBlocks );
        str = CString( "\t::SerEJReadResetTrans() - Called (%d)" );
    }
    else
    {
        m_sLastError = ::SerEJReadResetTransStart( &m_ulReadOffset, m_auchEJBuffer, sizeof( m_auchEJBuffer ), &m_usBytesRead, &m_usNoOfReadBlocks );
        str = CString( "\t::SerEJReadResetTransStart() - Called (%d)" );
    }

    str += CString( "...Read Offset#%ld, Actual Read#%d, Read Block#%d" );
    pcsample::TraceFunction( str, m_sLastError, m_ulReadOffset, m_usBytesRead, m_usNoOfReadBlocks );

    switch ( m_sLastError )
    {
    case EJ_CONTINUE:
    case EJ_END:
        // --- update total no. of read size ---
        m_ulTtlReadSize += m_usBytesRead;

        // --- determine whether end of file is reached or not ---
        fIsEndOfFile = ( m_sLastError == EJ_END ) ? TRUE : FALSE;
        m_sLastError = EJ_PERFORM;
        break;

    default:
        break;
    }

    if (( m_sLastError == EJ_PERFORM ) && ( fIsEndOfFile ))
    {
        // --- reset the last read block before exit this function ---
        m_sLastError = ::SerEJReadResetTransEnd( m_usNoOfReadBlocks );

        str =  CString( "\t::SerEJReadResetTransEnd() - Called (%d)" );
        str += CString( "...Read Block#%d" );
        pcsample::TraceFunction( str, m_sLastError, m_usNoOfReadBlocks );
    }

    pcsample::TraceFunction( _T("END   >> CReportEJ::ReadAndReset() = %d"), ( m_sLastError == EJ_PERFORM ));

    return ( m_sLastError == EJ_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::getEJLine()
//
//  PURPOSE :   Get a line of read EJ block.
//
//  RETURN :    lpszEJLine  - Pointer to retrieved string data.
//
//===========================================================================

LPSTR CReportEJ::getEJLine( 
	LPSTR lpszEJLine, BOOL& fSuccess )
{
	const UCHAR* puchSource;

	//if we have a full line of ej data return that
	//if not get more
	if( (m_usBytesRead - readOffset) >= (EJ_COLUMN * sizeof(TCHAR)))
	{}
	else
	{
		BOOL fIsEndOfFile;

		fSuccess = Read(fIsEndOfFile);
		if(!fSuccess)
		{
			return (lpszEJLine);
		}
	}

	puchSource = &m_auchEJBuffer[0]; 
    puchSource += readOffset;

	// --- copy source EJ line data to destination buffer ---
	UCHAR uchI;
    for ( uchI = 0; uchI < EJ_REPORT_WIDE; uchI++ )
    {
        // --- convert character, if double wide char is found ---

        if ( puchSource[ uchI ] == 0x12 )
        {
            lpszEJLine[ uchI ] = _T('~');
        }
        else
        {
            lpszEJLine[ uchI ] = puchSource[ uchI ];
        }
    }

    // --- make up a null-terminated string ---
    lpszEJLine[ uchI ] = 0x00;

	//Increment read offset into buffer and transaction
	//Increment number of EJ Lines
	m_usReadEJTrans += EJ_COLUMN * sizeof(TCHAR);
	m_ulReadOffset += EJ_COLUMN * sizeof(TCHAR);
	m_ulTtlReadSize += EJ_COLUMN * sizeof(TCHAR);
	readOffset += (EJ_COLUMN * sizeof(TCHAR));

	m_ulTtlEJLines ++;
	fSuccess = TRUE;

	return (lpszEJLine);
}

//===========================================================================
//
//  FUNCTION :  CReportEJ::getEJLine()
//
//  PURPOSE :   Get a line of read EJ block.
//
//  RETURN :    lpszEJLine  - Pointer to retrieved string data.
//
//===========================================================================

LPSTR CReportEJ::getEJLine(
    const USHORT usLineNo,      // line# to read from EJ block
    LPSTR  lpszEJLine,         // points to buffer to store EJ line changed from LPTSTR
    BOOL&   fIsEJTHeader        // indicates this line is EJT header
    )
{
    ASSERT( usLineNo < ( (pcttl::MAX_EJ_BUFFER * sizeof(TCHAR)) / (EJ_COLUMN * sizeof(TCHAR))));
    ASSERT( ! IsBadWritePtr( lpszEJLine, EJ_COLUMN + 1 ));

    const UCHAR* puchSource;    // --- points to specified line number data ---

    puchSource = &m_auchEJBuffer[0]; //ESMITH 
    puchSource += readOffset;

    // --- determine the whole of EJ transaction has been read ---
    if (( m_ejtHeader.usCVLI == 0 ) || ( m_ejtHeader.usCVLI <= m_usReadEJTrans ))
    {
        // --- copy new EJ transaction header to member variable ---
        ::CopyMemory( &m_ejtHeader, puchSource, sizeof( EJT_HEADER ));

        // --- set no. of read bytes in this EJ transaction ---
        m_usReadEJTrans = sizeof( EJT_HEADER );
		readOffset += sizeof( EJT_HEADER );

        // --- this line is EJ transaction header ---
        fIsEJTHeader = TRUE;
		m_ulTtlHeader++;
    }
    else
    {
        // --- copy source EJ line data to destination buffer ---
		UCHAR uchI;
        for ( uchI = 0; uchI < EJ_REPORT_WIDE; uchI++ )
        {
            // --- convert character, if double wide char is found ---
            if ( puchSource[ uchI ] == 0x12 )
            {
                lpszEJLine[ uchI ] = _T('~');
            }
            else
            {
                lpszEJLine[ uchI ] = puchSource[ uchI ];
            }
        }

        // --- make up a null-terminated string ---
        lpszEJLine[ uchI ] = 0x00;

        // --- increment no. of read bytes in m_auchEJBuffer ---
        m_usReadEJTrans += EJ_COLUMN * sizeof(TCHAR);
		readOffset += (EJ_COLUMN * sizeof(TCHAR));

        // --- this line is normal EJ line ---
        fIsEJTHeader = FALSE;
		m_ulTtlEJLines ++;
    }

    // --- return address of destination buffer ---
    return ( lpszEJLine );
}

////////////////////// END OF FILE ( ReportEJ.cpp ) /////////////////////////
