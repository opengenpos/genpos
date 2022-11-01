/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TallyCPM.cpp
//
//  PURPOSE:    Provides Tally of CPM/EPT class.
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
#include "tallycpm.h"

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
//  FUNCTION :  CTallyCPM::CTallyCPM()
//
//  PURPOSE :   Constructor of Tally of CPM/EPT class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTallyCPM::CTallyCPM() :
    m_sLastError( CPM_RET_SUCCESS )
{
	memset( &m_tallyCPM, 0, sizeof( m_tallyCPM ));
    //::ZeroMemory( &m_tallyCPM, sizeof( m_tallyCPM ));
}

//===========================================================================
//
//  FUNCTION :  CTallyCPM::~CTallyCPM()
//
//  PURPOSE :   Destructor of Tally of CPM/EPT class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTallyCPM::~CTallyCPM()
{
}

//===========================================================================
//
//  FUNCTION :  CTallyCPM::Read()
//
//  PURPOSE :   Read tally of CPM/EPT in 2170.
//
//  RETURN :    TRUE    - Specified tally is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTallyCPM::Read()
{
    // --- read tally of CPM/EPT in 2170 ---

    m_sLastError = ::SerCpmReadResetTally( CLI_READ_TLY_BOTH, &m_tallyCPM );

    return ( m_sLastError == CPM_RET_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTallyCPM::Reset()
//
//  PURPOSE :   Reset tally of CPM/EPT in 2170.
//
//  RETURN :    TRUE    - Specified tally is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTallyCPM::ReadAndReset()
{
    // --- reset tally of CPM/EPT in 2170 ---

    m_sLastError = ::SerCpmReadResetTally( CLI_RESET_TLY_BOTH, &m_tallyCPM );

    return ( m_sLastError == CPM_RET_SUCCESS );
}

//////////////////// END OF FILE ( TallyCPM.cpp ) ///////////////////////////
