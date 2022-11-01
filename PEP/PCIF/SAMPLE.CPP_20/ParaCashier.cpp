/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaCashier.cpp
//
//  PURPOSE:    Cashier Parameter class definitions and declarations.
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

//===========================================================================
//
//  FUNCTION :  CParaCashier::CParaCashier()
//
//  PURPOSE :   Constructor of Cashier Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCashier::CParaCashier() :
    m_sLastError( FALSE )
{
	memset( &m_paraCashier, 0, sizeof( m_paraCashier ));
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::~CParaCashier()
//
//  PURPOSE :   Destructor of Cashier Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCashier::~CParaCashier()
{
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::Read()
//
//  PURPOSE :   Read the Cashier parameter by cashier#
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCashier::Read(const ULONG ulCashierNo)
{
    ASSERT(( 0 < ulCashierNo ) && ( ulCashierNo <= pcttl::MAX_CASHIER_NO ));

    pcsample::TraceFunction( _T("BEGIN >> CParaCashier::Read()") );

    // --- read Department parameter by specified major department# ---
    m_paraCashier.ulCashierNo = ulCashierNo;
    m_sLastError = ::SerCasIndRead( &m_paraCashier );

    CString str;
    str.Format( _T("\t::SerCasIndRead() - Called (%d)"), m_sLastError );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    pcsample::TraceFunction( _T("END   >> CParaCashier::Read() = %d"), ( m_sLastError == CAS_PERFORM ));
    return ( m_sLastError == CAS_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::GetAllCashierNo()
//
//  PURPOSE :   Read all of existing cashier number in 2170.
//
//  RETURN :    TRUE    - All of cashier number is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCashier::GetAllCashierNo(
    ULONG * pausCashierNo,      // points to buffer to retrieve cashier#
    const USHORT usBufferSize,  // size of pausCashierNo buffer in bytes
    USHORT& usNoOfPerson        // no. of person read in pauscashierNo
    )
{
    ASSERT(( sizeof( USHORT ) * CAS_NUMBER_OF_MAX_CASHIER ) <= usBufferSize );
    ASSERT( ! IsBadWritePtr( pausCashierNo, usBufferSize ));

    pcsample::TraceFunction( _T("BEGIN >> CParaCashier::GetAllCashierNo()") );

    // --- get all of existing cashier number via PC Interface function ---
    SHORT   sActualRead = ::SerCasAllIdRead( usBufferSize, pausCashierNo );

    CString str;
    str.Format( _T("\t::SerCasAllIdRead() - Called (%d)"), sActualRead );
    pcsample::TraceFunction( str );

    if ( 0 <= sActualRead )
    {
        // --- get no. of read cashier number and save it ---
        usNoOfPerson = sActualRead;
        m_sLastError = CAS_PERFORM;
    }
    else
    {
        // --- function is failed ---
        m_sLastError = sActualRead;
    }

    pcsample::TraceFunction( _T("END   >> CParaCashier::GetAllCashierNo() = %d"), ( m_sLastError == CAS_PERFORM ));
    return ( m_sLastError == CAS_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::ClearSecretCode()
//
//  PURPOSE :   Clear secret code of the specified cashier.
//
//  RETURN :    TRUE    - Sercret code is cleared.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CParaCashier::ClearSecretCode(const ULONG ulCashierNo)
{
    pcsample::TraceFunction( _T("BEGIN >> CParaCashier::ClearSecretCode()") );

    // --- clear sercret code of specified casheir ---
    m_sLastError = ::SerCasSecretClr( ulCashierNo );

    CString str;
    str.Format( _T("\t::SerCasSecretClr() - Called (%d)"), m_sLastError );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    pcsample::TraceFunction( _T("END   >> CParaCashier::ClearSecretCode() = %d"), ( m_sLastError == CAS_PERFORM ));

    return ( m_sLastError == CAS_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::IsSignIn()
//
//  PURPOSE :   Determine whether the specified cashier is sign in terminal.
//
//  RETURN :    TRUE    - Sercret code is cleared.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CParaCashier::IsSignIn(const ULONG ulCashierNo)
{
    pcsample::TraceFunction( _T("BEGIN >> CParaCashier::IsSignIn()") );

	ULONG  ausCashierOnTerminal[ MAX_TERMINAL_NO ] = {0};
    SHORT   sResult;
    BOOL    fIsSignIn;

    // --- get all of terminal# which is signed in by cashier ---
    sResult = ::SerCasChkSignIn( &ausCashierOnTerminal[ 0 ] );

    CString str;
    str.Format( _T("\t::SerCasChkSignIn() - Called (%d)"), sResult );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    switch ( sResult )
    {
    case CAS_PERFORM:
        // --- there is no terminal which is signed in by cashier ---
        fIsSignIn    = FALSE;
        m_sLastError = CAS_PERFORM;
        break;

    case CAS_DURING_SIGNIN:
        // --- look up target cashier from siged in terminal# ---
        fIsSignIn = FALSE;
        for (USHORT usI = 0; usI < MAX_TERMINAL_NO; usI++)
        {
            if ( ausCashierOnTerminal[ usI ] == ulCashierNo )
            {
                // --- yes, the cashier is sign in this terminal ---
                fIsSignIn = TRUE;
				break;
            }
        }
        m_sLastError = CAS_PERFORM;
        break;

    default:
        // --- function is failed ---
        fIsSignIn    = FALSE;
        m_sLastError = sResult;
        break;
    }

    pcsample::TraceFunction( _T("END   >> CParaCashier::IsSignIn() = %d"), fIsSignIn );
    return ( fIsSignIn );
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::SignOutAllTerminal()
//
//  PURPOSE :   Sign out the cashier from current signed in terminal.
//
//  RETURN :    TRUE    - Cashier is signed out.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCashier::SignOutAllTerminal(const ULONG ulCashierNo)
{
    pcsample::TraceFunction( _T("BEGIN >> CParaCashier::SignOutAllTerminal()") );

    // --- sign out the specified cashier from signed in terminal ---
    m_sLastError = ::SerCasClose( ulCashierNo );

    CString str;
    str.Format( _T("\t::SerCasClose() - Called (%d)"), m_sLastError );
    str += CString( "...Cashier#%d" );
    pcsample::TraceFunction( str, ulCashierNo );

    pcsample::TraceFunction( _T("END   >> CParaCashier::SignOutAllTerminal() = %d"),
                             (( m_sLastError == CAS_PERFORM   ) ||
                              ( m_sLastError == CAS_NO_SIGNIN )) );

    return (( m_sLastError == CAS_PERFORM ) || ( m_sLastError == CAS_NO_SIGNIN ));
}

//===========================================================================
//
//  FUNCTION :  CParaCashier::getName()
//
//  PURPOSE :   Get cashier name of specified cashier number.
//
//  RETURN :    LPTSTR  - Points to retrieved cashier name.
//
//===========================================================================
LPTSTR CParaCashier::getName(LPTSTR lpszCashierName) const
{
    ASSERT( ! IsBadWritePtr( lpszCashierName, CAS_NUMBER_OF_CASHIER_NAME + 1 ));

    // --- copy cashier name to user buffer ---
    memset( lpszCashierName, 0, (CAS_NUMBER_OF_CASHIER_NAME + 1) * sizeof(TCHAR) );
    _tcsncpy( lpszCashierName, m_paraCashier.auchCashierName, CAS_NUMBER_OF_CASHIER_NAME );

    // --- return to department name buffer address ---
    return ( lpszCashierName );
}

////////////////////// END OF FILE ( ParaCashier.cpp ) //////////////////////
