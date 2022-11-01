// TerminalLock.cpp : implementation of the CTerminalLock class
//

/////////////////////////////////////////////////////////////////////////////
// Include Files

#include "stdafx.h"
#include "Total.h"

/////////////////////////////////////////////////////////////////////////////
// Debug Macros

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerminalLock construction

CTerminalLock::CTerminalLock() :
    m_fLocked( FALSE )
{
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalLock destruction

CTerminalLock::~CTerminalLock()
{
    Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalLock::Lock()

BOOL CTerminalLock::Lock()
{
    BOOL    fSuccess;

    if ( IsLocked())
    {
        fSuccess = TRUE;
    }
    else
    {
        fSuccess = ( ::IspLockKeyBoard() == ISP_SUCCESS ) ? TRUE : FALSE;
    }

    m_fLocked = fSuccess;

    return ( fSuccess );
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalLock::Unlock()

BOOL CTerminalLock::Unlock()
{
    BOOL    fSuccess;

    if ( IsLocked())
    {
        fSuccess = ( ::IspUnLockKeyBoard() == ISP_SUCCESS ) ? TRUE : FALSE;
    }
    else
    {
        fSuccess = TRUE;
    }

    m_fLocked = ( fSuccess ) ? FALSE : TRUE;

    return ( fSuccess );
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalLock::IsLocked()

inline BOOL CTerminalLock::IsLocked()
{
    return ( m_fLocked );
}

/////////////////////////////////////////////////////////////////////////////
// TerminalLock.cpp : END OF FILE
