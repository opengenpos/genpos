/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaDept.cpp
//
//  PURPOSE:    Department Parameter class definitions and declarations.
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
//  FUNCTION :  CParaDept::CParaDept()
//
//  PURPOSE :   Constructor of Department Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaDept::CParaDept() :
    m_sLastError( FALSE )
{
	memset( &m_paraDept, 0, sizeof( m_paraDept ));
    //::ZeroMemory( &m_paraDept, sizeof( m_paraDept ));
}

//===========================================================================
//
//  FUNCTION :  CParaDept::~CParaDept()
//
//  PURPOSE :   Destructor of Department Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaDept::~CParaDept()
{
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::Initialize()
//
//  PURPOSE :   Initialize member variables in Department parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CParaDept::Initialize()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaDept::Initialize()") );

    m_sLastError = FALSE;
	memset( &m_paraDept, 0, sizeof( m_paraDept ));
    //::ZeroMemory( &m_paraDept, sizeof( m_paraDept ));

    pcsample::TraceFunction( _T("END   >> CParaDept::Initialize() = VOID") );
}

//===========================================================================
//
//  FUNCTION :  CParaDept::Read()
//
//  PURPOSE :   Read the Department parameter by major department#
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaDept::Read(
    const UCHAR uchMajorDeptNo  // major department# to read
    )
{
    ASSERT( uchMajorDeptNo <= MAX_MDEPT_NO );

    pcsample::TraceFunction( _T("BEGIN >> CParaDept::Read()") );

    // --- read Department parameter by specified major department# ---

    m_paraDept.uchMajorDeptNo = uchMajorDeptNo;

    m_sLastError = ::SerOpMajorDeptRead( &m_paraDept, 0 );

    CString str;
    str.Format( _T("\t::SerOpMajorDeptRead() - Called (%d)"), m_sLastError );
    str += CString( "...MajorDept#%d" );
    pcsample::TraceFunction( str, uchMajorDeptNo );

    pcsample::TraceFunction( _T("END   >> CParaDept::Read() = %d"),
                             ( m_sLastError == OP_PERFORM ));

    return ( m_sLastError == OP_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaDept::getName()
//
//  PURPOSE :   Get Department name in read Department parameter.
//
//  RETURN :    LPTSTR  - Address of buffer to Department name
//
//===========================================================================

LPTSTR CParaDept::getName(
    LPTSTR lpszDeptName         // points to buffer to retrieve dept. name
    ) const
{
    ASSERT( ! IsBadWritePtr( lpszDeptName, OP_DEPT_NAME_SIZE + 1 ));

    // --- copy department name to user buffer ---

    memset( lpszDeptName, 0, (OP_DEPT_NAME_SIZE + 1) * sizeof(TCHAR) );
    _tcsncpy( lpszDeptName,
                  m_paraDept.ParaDept.auchMnemonic,
                  OP_DEPT_NAME_SIZE );
    /*::ZeroMemory( lpszDeptName, OP_DEPT_NAME_SIZE + 1 );
    ::CopyMemory( lpszDeptName,
                  m_paraDept.ParaDept.auchMnemonic,
                  OP_DEPT_NAME_SIZE );*/

    // --- return to department name buffer address ---

    return ( lpszDeptName );
}

////////////////////// END OF FILE ( ParaDept.cpp ) /////////////////////////
