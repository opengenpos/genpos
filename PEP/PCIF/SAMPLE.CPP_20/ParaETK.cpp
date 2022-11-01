/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaETK.cpp
//
//  PURPOSE:    ETK Parameter class definitions and declarations.
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
//  FUNCTION :  CParaETK::CParaETK()
//
//  PURPOSE :   Constructor of ETK Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaETK::CParaETK() :
    m_sLastError( FALSE )
{
	memset( &m_jobETK, 0, sizeof( m_jobETK    ));
    memset( m_auchETKName, 0, (ETK_MAX_NAME_SIZE + 1) * sizeof(TCHAR));
    /*::ZeroMemory( &m_jobETK,     sizeof( m_jobETK    ));
    ::ZeroMemory( m_auchETKName, sizeof( m_auchETKName ));*/
}

//===========================================================================
//
//  FUNCTION :  CParaETK::~CParaETK()
//
//  PURPOSE :   Destructor of ETK Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaETK::~CParaETK()
{
}

//===========================================================================
//
//  FUNCTION :  CParaETK::Read()
//
//  PURPOSE :   Read the ETK parameter by Employee number.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaETK::Read(
    const ULONG ulETKNo         // Employee# to read parameter
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CParaETK::Read()") );

    // --- get ETK parameter by the specified employee number ---

    m_sLastError = ::SerEtkIndJobRead( ulETKNo, &m_jobETK, m_auchETKName );

    CString str;
    str.Format( _T("\t::SerEtkIndJobRead() - Called (%d)"), m_sLastError );
    str += CString( "...Employee#%ld" );
    pcsample::TraceFunction( str, ulETKNo );

    pcsample::TraceFunction( _T("END   >> CParaETK::Read() = %d"),
                             ( m_sLastError == ETK_SUCCESS ));

    return ( m_sLastError == ETK_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CParaETK::getName()
//
//  PURPOSE :   Get Employee name of specified Employee number.
//
//  RETURN :    LPTSTR  - Points to retrieved Employee name.
//
//===========================================================================

LPTSTR CParaETK::getName(
    LPTSTR lpszETKName          // Points to buffer to retrieve Employee name
    ) const
{
    ASSERT( ! IsBadWritePtr( lpszETKName, ETK_MAX_NAME_SIZE + 1 ));

    // --- copy cashier name to user buffer ---

    memset( lpszETKName, 0, (ETK_MAX_NAME_SIZE + 1) * sizeof(TCHAR) );
    _tcsncpy( lpszETKName, m_auchETKName, ETK_MAX_NAME_SIZE );
    /*::ZeroMemory( lpszETKName, ETK_MAX_NAME_SIZE + 1 );
    ::CopyMemory( lpszETKName, m_auchETKName, ETK_MAX_NAME_SIZE * sizeof(TCHAR));*/

    // --- return to department name buffer address ---

    return ( lpszETKName );
}

////////////////////// END OF FILE ( ParaETK.cpp ) //////////////////////////
