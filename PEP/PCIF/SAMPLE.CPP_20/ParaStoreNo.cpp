/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaStoreNo.cpp
//
//  PURPOSE:    Store/Terminal# Parameter class definitions and declarations.
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
//  FUNCTION :  CParaStoreNo::CParaStoreNo()
//
//  PURPOSE :   Constructor of Store/Termial# Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaStoreNo::CParaStoreNo()
{
	memset( m_ausStoreRegNo, 0, sizeof( m_ausStoreRegNo ));
    //::ZeroMemory( m_ausStoreRegNo, sizeof( m_ausStoreRegNo ));
}

//===========================================================================
//
//  FUNCTION :  CParaStoreNo::~CParaStoreNo()
//
//  PURPOSE :   Destructor of Store/Termial# Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaStoreNo::~CParaStoreNo()
{
}

//===========================================================================
//
//  FUNCTION :  CParaStoreNo::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaStoreNo::Read()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaStoreNo::Read()") );

    BOOL    fSuccess;
    fSuccess = CParameter::ReadAllPara( CLASS_PARASTOREGNO,
                                        m_ausStoreRegNo,
                                        sizeof( m_ausStoreRegNo ));

    pcsample::TraceFunction( _T("END   >> CParaStoreNo::Read() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CParaStoreNo::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaStoreNo::Write()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaStoreNo::Write()") );

    BOOL    fSuccess;
    fSuccess = CParameter::WriteAllPara( CLASS_PARASTOREGNO,
                                         m_ausStoreRegNo,
                                         sizeof( m_ausStoreRegNo ));

    pcsample::TraceFunction( _T("END   >> CParaStoreNo::Write() = %d"), fSuccess );

    return ( fSuccess );
}

/////////////////// END OF FILE ( ParaStoreNo.cpp ) /////////////////////////
