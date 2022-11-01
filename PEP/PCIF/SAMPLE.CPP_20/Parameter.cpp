/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Parameter.cpp
//
//  PURPOSE:    Base Parameter class definitions and declarations.
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

#include "Parameter.h"
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

//===========================================================================
//
//  FUNCTION :  CParameter::CParameter()
//
//  PURPOSE :   Constructor of Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParameter::CParameter() :
    m_sLastError( SUCCESS ),
	m_bDataRead (false),
	m_usClassRead(0)
{
}

//===========================================================================
//
//  FUNCTION :  CParameter::~CParameter()
//
//  PURPOSE :   Destructor of Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParameter::~CParameter()
{
}

//===========================================================================
//
//  FUNCTION :  CParameter::ReadAllParameter()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParameter::ReadAllPara(
    const UCHAR uchClass,       // class to read parameter
    LPVOID lpvBuffer,           // buffer to recieve parameter
    const USHORT usBytesRead    // size of lpvBuffer in bytes
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CParameter::ReadAllPara()") );

    USHORT  usReadOffset;
    USHORT  usActualRead;

    // --- read specified parameter via PC Interface function ---

    usReadOffset = 0;

    m_sLastError = ::CliParaAllRead( uchClass, static_cast< UCHAR* >( lpvBuffer ), usBytesRead, usReadOffset, &usActualRead );
	if (m_sLastError == SUCCESS && usActualRead == 0) {
		m_sLastError = -1;
	}

    CString str;
    str.Format( _T("\t::CliParaAllRead() - Called (%d)"), m_sLastError );
    str += CString( "...Class#%d" );
    pcsample::TraceFunction( str, uchClass );

    pcsample::TraceFunction( _T("END   >> CParameter::ReadAllPara() = %d"), ( m_sLastError == SUCCESS ));

	m_bDataRead = (m_sLastError == SUCCESS);
	m_usClassRead = uchClass;

    return ( m_sLastError == SUCCESS );
}

BOOL CParameter::ReadAllParaByBlocks(
	const UCHAR uchClass,       // class to read parameter
	LPVOID lpvBuffer,           // buffer to recieve parameter
	const USHORT usBytesRead    // size of lpvBuffer in bytes
)
{
	pcsample::TraceFunction(_T("BEGIN >> CParameter::ReadAllParaByBlocks()"));

	USHORT  usReadOffset;
	USHORT  usActualRead;
	const   USHORT usBufRead = 1024;

	// --- read specified parameter via PC Interface function ---
	usReadOffset = 0;

	do {
		m_sLastError = ::CliParaAllRead(uchClass, static_cast< UCHAR* >(lpvBuffer) + usReadOffset, usBufRead, usReadOffset, &usActualRead);
		if (m_sLastError == SUCCESS && usReadOffset == 0 && usActualRead == 0) {
			m_sLastError = -1;
			break;
		}
		/* ----- seek start addresss ----- */
		usReadOffset += usBufRead;
	} while (usActualRead == usBufRead && m_sLastError == SUCCESS);

	CString str;
	str.Format(_T("\t::ReadAllParaByBlocks() - Called (%d)"), m_sLastError);
	str += CString("...Class#%d");
	pcsample::TraceFunction(str, uchClass);

	pcsample::TraceFunction(_T("END   >> CParameter::ReadAllParaByBlocks() = %d"), (m_sLastError == SUCCESS));

	m_bDataRead = (m_sLastError == SUCCESS);
	m_usClassRead = uchClass;

	return (m_sLastError == SUCCESS);
}

//===========================================================================
//
//  FUNCTION :  CParameter::WriteAllParameter()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is written.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParameter::WriteAllPara(
    const UCHAR uchClass,
    LPCVOID lpvBuffer,
    const USHORT usBytesWrite
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CParameter::WriteAllPara()") );

    USHORT  usWriteOffset;
    USHORT  usActualWrite;

    // --- write specified parameter via PC Interface function ---

    usWriteOffset = 0;

    LPVOID  lpv;
    lpv = const_cast< LPVOID >( lpvBuffer );

    m_sLastError = ::CliParaAllWrite( uchClass, static_cast< UCHAR* >( lpv ), usBytesWrite, usWriteOffset, &usActualWrite );

    CString str;
    str.Format( _T("\t::CliParaAllWrite() - Called (%d)"), m_sLastError );
    str += CString( "...Class#%d" );
    pcsample::TraceFunction( str, uchClass );

    pcsample::TraceFunction( _T("END   >> CParameter::WriteAllPara() = %d"), ( m_sLastError == SUCCESS ));

	return ( m_sLastError == SUCCESS );
}

BOOL CParameter::WriteAllParaByBlocks(
	const UCHAR uchClass,       // class to read parameter
	LPVOID lpvBuffer,           // buffer to recieve parameter
	const USHORT usBytesRead    // size of lpvBuffer in bytes
)
{
	pcsample::TraceFunction(_T("BEGIN >> CParameter::WriteAllParaByBlocks()"));

	USHORT  usWriteOffset;
	USHORT  usActualWrite;
	const   USHORT usBufWrite = 1024;

	// --- write specified parameter via PC Interface function ---
	usWriteOffset = 0;

	do {
		m_sLastError = ::CliParaAllWrite(uchClass, static_cast< UCHAR* >(lpvBuffer) + usWriteOffset, usBufWrite, usWriteOffset, &usActualWrite);
		/* ----- seek start addresss ----- */
		usWriteOffset += usBufWrite;
	} while (usActualWrite == usBufWrite);

	CString str;
	str.Format(_T("\t::WriteAllParaByBlocks() - Called (%d)"), m_sLastError);
	str += CString("...Class#%d");
	pcsample::TraceFunction(str, uchClass);

	pcsample::TraceFunction(_T("END   >> CParameter::WriteAllParaByBlocks() = %d"), (m_sLastError == SUCCESS));

	return (m_sLastError == SUCCESS);
}


//////////////////// END OF FILE ( Parameter.cpp ) //////////////////////////
