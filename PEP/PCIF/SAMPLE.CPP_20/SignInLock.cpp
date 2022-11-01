/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  SignInLock.cpp
//
//  PURPOSE:    Sign In Lock class definitions and declarations.
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
#include "Terminal.h"
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
//  FUNCTION :  CSignInLock::CSignInLock()
//
//  PURPOSE :   Constructor of Sign In Lock class.
//
//  RETURN :    No return value.
//
//===========================================================================

CSignInLock::CSignInLock() :
    m_usCurrentLockedTerminal( static_cast< USHORT >( -1 ))
{
}

//===========================================================================
//
//  FUNCTION :  CSignInLock::~CSignInLock()
//
//  PURPOSE :   Destructor of Sign In Lock class.
//
//  RETURN :    No return value.
//
//===========================================================================

CSignInLock::~CSignInLock()
{
    // --- unlock locked terminal before this object is discarded ---

    Unlock();
}

//===========================================================================
//
//  FUNCTION :  COperatorLock::Lock()
//
//  PURPOSE :   Lock the specified terminal.
//
//  RETURN :    TRUE    - Terminal is locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CSignInLock::Lock(
    const ULONG ulTerminalNo   // terminal# to lock
    )
{
	USHORT  usTerminalNo = (USHORT)ulTerminalNo;

	return Lock (usTerminalNo);
}


BOOL CSignInLock::Lock(
    const USHORT usTerminalNo   // terminal# to lock
    )
{
    ASSERT(( usTerminalNo == pcterm::LOCK_ALL_TERMINAL ) ||
           (( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO )));

    pcsample::TraceFunction( _T("BEGIN >> CSignInLock::Lock()") );

    // --- is this object already locked some terminal ? ---

    if ( CLockObject::IsLocked())
    {
        // --- yes, this object is already locked ---

        if ( usTerminalNo == m_usCurrentLockedTerminal )
        {
            // --- attempt to lock the same terminal, do nothing ---

            m_sLastError = CAS_PERFORM;
        }
        else
        {
            // --- unlock current locked terminal before other locks ---

            m_sLastError = CAS_OTHER_LOCK;
        }
    }
    else
    {
        // --- no, this object is not locked ---

        if ( usTerminalNo == pcterm::LOCK_ALL_TERMINAL )
        {
            // --- attempt to lock all of terminals ---

            m_sLastError = ::SerCasAllTermLock();

            CString str;
            str.Format(_T( "\t::SerCasAllTermLock() - Called (%d)"), m_sLastError );
            pcsample::TraceFunction( str );
        }
        else
        {
            // --- attempt to lock terminal by specified terminal# ---

            m_sLastError = ::SerCasIndTermLock( usTerminalNo );

            CString str;
            str.Format( _T("\t::SerCasIndTermLock() - Called (%d)"), m_sLastError );
            str += CString( "...Terminal#%d" );
            pcsample::TraceFunction( str, usTerminalNo );
        }
    }

    if ( m_sLastError == CAS_PERFORM )
    {
        // --- save current lock state to member variable ---

        m_fLocked = TRUE;
        m_usCurrentLockedTerminal = usTerminalNo;
    }

    pcsample::TraceFunction( _T("END   >> CSignInLock::Lock() = %d"),
                             ( m_sLastError == CAS_PERFORM ));

    return ( m_sLastError == CAS_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CSignInLock::Unlock()
//
//  PURPOSE :   Unlock the current locked terminal.
//
//  RETURN :    TRUE    - Terminal is unlocked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CSignInLock::Unlock()
{
    pcsample::TraceFunction( _T("BEGIN >> CSignInLock::Unlock()") );

    // --- is this object locked some terminal ? ---

    if ( CLockObject::IsLocked())
    {
        // --- yes, this object locked some terminal ---

        if ( m_usCurrentLockedTerminal == pcterm::LOCK_ALL_TERMINAL )
        {
            // --- attempt to unlock all of terminals ---

            ::SerCasAllTermUnLock();

            CString str;
            str.Format( _T("\t::SerCasAllTermUnLock() - Called (VOID)") );
            pcsample::TraceFunction( str );

            m_sLastError = CAS_PERFORM;
        }
        else
        {
            // --- attempt to unlock the current locked terminal ---

            m_sLastError = ::SerCasIndTermUnLock( m_usCurrentLockedTerminal );

            CString str;
            str.Format( _T("\t::SerCasIndTermUnLock() - Called (%d)"),
                        m_sLastError );
            str += CString( "...Terminal#%d" );
            pcsample::TraceFunction( str, m_usCurrentLockedTerminal );
        }
    }
    else
    {
        // --- this object is not locked now, do nothing ---

        m_sLastError = CAS_PERFORM;
    }

    if ( m_sLastError == CAS_PERFORM )
    {
        // --- save current lock state to member variable ---

        m_fLocked = FALSE;
        m_usCurrentLockedTerminal = static_cast< USHORT >( -1 );
    }

    pcsample::TraceFunction( _T("END   >> CSignInLock::Unlock() = %d"),
                             ( m_sLastError == CAS_PERFORM ));

    return ( m_sLastError == CAS_PERFORM );
}

////////////////////// END OF FILE ( SignInLock.h ) /////////////////////////
