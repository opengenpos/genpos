/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  OperatorLock.cpp
//
//  PURPOSE:    Operator Lock class definitions and declarations.
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
//  FUNCTION :  COperatorLock::COperatorLock()
//
//  PURPOSE :   Constructor of Operator Lock class.
//
//  RETURN :    No return value.
//
//===========================================================================

COperatorLock::COperatorLock() :
    m_ulCurrentLockedOperator( static_cast< ULONG >( -1 ))
{
}

//===========================================================================
//
//  FUNCTION :  COperatorLock::~COperatorLock()
//
//  PURPOSE :   Destructor of Operator Lock class.
//
//  RETURN :    No return value.
//
//===========================================================================

COperatorLock::~COperatorLock()
{
    // --- unlock operator before this object is discarded ---

    Unlock();
}

//===========================================================================
//
//  FUNCTION :  COperatorLock::Lock()
//
//  PURPOSE :   Lock the specified operator.
//
//  RETURN :    TRUE    - Operator is locked.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL COperatorLock::Lock(
    const USHORT usOperatorNo   // operator# to lock
    )
{
	ULONG  ulOperatorNo = usOperatorNo;

	return Lock (ulOperatorNo);
}

BOOL COperatorLock::Lock(
    const ULONG ulOperatorNo   // operator# to lock
    )
{
    ASSERT(( ulOperatorNo == pcterm::LOCK_ALL_OPERATOR ) ||
           (( 0 < ulOperatorNo ) && ( ulOperatorNo <= pcttl::MAX_CASHIER_NO )));

    pcsample::TraceFunction( _T("BEGIN >> COperatorLock::Lock()") );

    // --- is this object already locked ? ---
    if ( CLockObject::IsLocked())
    {
        // --- yes, this object is already locked ---
        if ( ulOperatorNo == m_ulCurrentLockedOperator )
        {
            // --- attempt to lock the same operator, do nothing ---
            m_sLastError = CAS_PERFORM;
        }
        else
        {
            // --- unlock current locked operator before other locks ---
            m_sLastError = CAS_OTHER_LOCK;
        }
    }
    else
    {
        // --- no, this object is not locked ---
        if ( ulOperatorNo == pcterm::LOCK_ALL_OPERATOR )
        {
            // --- attempt to lock all of operators ---
            m_sLastError = ::SerCasAllLock();

            CString str;
            str.Format( _T("\t::SerCasAllLock() - Called (%d)"), m_sLastError );
            pcsample::TraceFunction( str );

            if ( m_sLastError == CAS_FILE_NOT_FOUND )
            {
                // --- no operator in terminal, do nothing ---
                m_sLastError = CAS_PERFORM;
            }
        }
        else
        {
            // --- attempt to lock operator by specified operator# ---
            m_sLastError = ::SerCasIndLock( ulOperatorNo );

            CString str;
            str.Format( _T("\t::SerCasIndLock() - Called (%d)"), m_sLastError );
            str += CString( "...Cashier#%d" );
            pcsample::TraceFunction( str, ulOperatorNo );
        }
    }

    if ( m_sLastError == CAS_PERFORM )
    {
        // --- save current operator lock state to member variable ---
        m_fLocked = TRUE;
        m_ulCurrentLockedOperator = ulOperatorNo;
    }

    pcsample::TraceFunction( _T("END   >> COperatorLock::Lock() = %d"), ( m_sLastError == CAS_PERFORM ));

    return ( m_sLastError == CAS_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  COperatorLock::Unlock()
//
//  PURPOSE :   Unlock the current locked operator.
//
//  RETURN :    TRUE    - Operator is unlocked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL COperatorLock::Unlock()
{
    pcsample::TraceFunction( _T("BEGIN >> COperatorLock::Unlock()") );

    // --- has this object locked someone ? ---
    if ( CLockObject::IsLocked())
    {
        // --- yes, this object locked some operator ---
        if ( m_ulCurrentLockedOperator == pcterm::LOCK_ALL_OPERATOR )
        {
            // --- attempt to unlock all of operators ---
            ::SerCasAllUnLock();

            CString str;
            str.Format( _T("\t::SerCasAllUnLock() - Called (VOID)") );
            pcsample::TraceFunction( str );

            m_sLastError = CAS_PERFORM;
        }
        else
        {
            // --- attempt to unlock the current locked operator ---
            m_sLastError = ::SerCasIndUnLock( m_ulCurrentLockedOperator );

            CString str;
            str.Format( _T("\t::SerCasIndUnLock() - Called (%d)"), m_sLastError );
            str += CString( "...Cashier#%d" );
            pcsample::TraceFunction( str, m_ulCurrentLockedOperator );
        }
    }
    else
    {
        // --- this object is not locked now, do nothing ---
        m_sLastError = CAS_PERFORM;
    }

    if ( m_sLastError == CAS_PERFORM )
    {
        // --- save current operator lock state to member variable ---
        m_fLocked = FALSE;
        m_ulCurrentLockedOperator = static_cast< ULONG >( -1 );
    }

    pcsample::TraceFunction( _T("END   >> COperatorLock::Unlock() = %d"), ( m_sLastError == CAS_PERFORM ));

    return ( m_sLastError == CAS_PERFORM );
}

////////////////////// END OF FILE ( OperatorLock.h ) ///////////////////////
