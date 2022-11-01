/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Terminal.cpp
//
//  PURPOSE:    Terminal class definitions and declarations.
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
#include "R20_PC2172.h"

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
//  FUNCTION :  CTerminal::CTerminal()
//
//  PURPOSE :   Constructor of terminal class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTerminal::CTerminal() :
    m_usLoggedOnTerminalNo( 0 ),
    m_fKeyboardLocked( FALSE ),
    m_sLastError( ISP_SUCCESS )
{
    memset( m_auchPassword, 0, sizeof(m_auchPassword));
    memset( &m_paraSystem, 0, sizeof( m_paraSystem   ));
}

//===========================================================================
//
//  FUNCTION :  CTerminal::~CTerminal()
//
//  PURPOSE :   Destructor of terminal class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTerminal::~CTerminal()
{
    // --- log off terminal before discard this class ---

    LogOff();
}

//===========================================================================
//
//  FUNCTION :  CTerminal::LogOn()
//
//  PURPOSE :   Log on to 2170 terminal by terminal number and password.
//
//  RETURN :    TRUE    - Terminal is logged on.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::LogOn(
    const USHORT usTerminalNo,// terminal# to logged on
    const CString& strPassword  // terminal logged on password
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));
    ASSERT( strPassword.GetLength() <= MAX_PCIF_SIZE );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::LogOn()") );

    BOOL    fSuccess;

    // --- is the target not the same as current logged on terminal ? ---

    if ( m_usLoggedOnTerminalNo != usTerminalNo )
    {
        // --- at first, attempt to log off from current terminal  ---

        if ( LogOff())
        {
            UCHAR   auchIPAddress[ 4 ];
            TCHAR   auchPassword[ MAX_PCIF_SIZE + 1 ]; //UCHAR to TCHAR ESMITH

            // --- get IP address from terminal number ---
			
            auchIPAddress[ 0 ] = (UCHAR)127;
            auchIPAddress[ 1 ] = (UCHAR)0;
            auchIPAddress[ 2 ] = (UCHAR)0;
            auchIPAddress[ 3 ] = (UCHAR)1;

            // --- get PC Interface password ---
			memset(auchPassword, 0, sizeof(auchPassword));
			_tcsncpy(auchPassword,( LPCTSTR )strPassword, MAX_PCIF_SIZE);

            // --- attempt to log on to the target terminal ---

            m_sLastError = ::IspLogOn( auchIPAddress, auchPassword );

            CString str;
            str.Format( _T("\t::IspLogOn() - Called (%d)"), m_sLastError );
            str += CString( "...IP#%d.%d.%d.%d, Password:%s" );
            pcsample::TraceFunction( str,
                                     auchIPAddress[ 0 ],
                                     auchIPAddress[ 1 ],
                                     auchIPAddress[ 2 ],
                                     auchIPAddress[ 3 ],
                                     strPassword );

            fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;
        }
        else
        {
            // --- function is failed ---

            fSuccess = FALSE;
        }
    }
    else
    {
        // --- we already log on this terminal, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current logged on terminal# and password ---

        m_usLoggedOnTerminalNo = usTerminalNo;
		memset(m_auchPassword,0,sizeof(m_auchPassword));
		_tcsncpy(m_auchPassword,strPassword,MAX_PCIF_SIZE);
    }
    else
    {
        // --- an error occurs, clear logged on terminal# and password ---

        m_usLoggedOnTerminalNo = 0;
		memset(m_auchPassword,0,sizeof(m_auchPassword));
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::LogOn() = %d"), fSuccess );

    return ( fSuccess );
}



//===========================================================================
//	Changed
//  FUNCTION :  CTerminal::LogOn()  added three variables for IP connection in a 
//		dialog box
//
//  PURPOSE :   Log on to 2170 terminal by ip address and password.
//
//  RETURN :    TRUE    - Terminal is logged on.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::LogOn( const USHORT IPAddy1,const USHORT IPAddy2,const USHORT IPAddy3,
    const USHORT IPAddy4,// terminal# to logged on
    const CString& strPassword,  // terminal logged on password
	const USHORT usTerminalNo
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));
    ASSERT( strPassword.GetLength() <= MAX_PCIF_SIZE );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::LogOn()") );

    BOOL    fSuccess;

    // --- is the target not the same as current logged on terminal ? ---

    if ( m_usLoggedOnTerminalNo != usTerminalNo )
    {
        // --- at first, attempt to log off from current terminal  ---

        if ( LogOff())
        {
            UCHAR   auchIPAddress[ 4 ];
            TCHAR   auchPassword[ MAX_PCIF_SIZE +1]; //UCHAR to TCHAR ESMITH

            // --- get IP address from dialog box---

            auchIPAddress[ 0 ] = static_cast< UCHAR >( IPAddy1 );
            auchIPAddress[ 1 ] = static_cast< UCHAR >( IPAddy2 );
            auchIPAddress[ 2 ] = static_cast< UCHAR >( IPAddy3 );
            auchIPAddress[ 3 ] = static_cast< UCHAR >( IPAddy4 );

            // --- get PC Interface password ---

			memset( auchPassword, 0, sizeof( auchPassword ));
			_tcsncpy(auchPassword,strPassword, MAX_PCIF_SIZE);

            // --- attempt to log on to the target terminal ---

            m_sLastError = ::IspLogOn( auchIPAddress, auchPassword );

			CString str;
            str.Format( _T("\t::IspLogOn() - Called (%d)"), m_sLastError );
            str += CString( "...IP#%d.%d.%d.%d, Password:%s" );
            pcsample::TraceFunction( str,
                                     auchIPAddress[ 0 ],
                                     auchIPAddress[ 1 ],
                                     auchIPAddress[ 2 ],
                                     auchIPAddress[ 3 ],
                                     strPassword );

           fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;

		       // --- format string by user defined format list ---


		  		
        }
        else
        {
            // --- function is failed ---

            fSuccess = FALSE;
        }
    }
    else
    {
        // --- we already log on this terminal, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current logged on terminal# and password ---

        m_usLoggedOnTerminalNo = usTerminalNo;

		memset( m_auchPassword, 0, (MAX_PCIF_SIZE + 1) * sizeof(TCHAR));
		_tcsncpy(m_auchPassword, strPassword, MAX_PCIF_SIZE);
    }
    else
    {
        // --- an error occurs, clear logged on terminal# and password ---

        m_usLoggedOnTerminalNo = 0;
		memset(m_auchPassword,0, (MAX_PCIF_SIZE + 1) * sizeof(TCHAR));
        ::ZeroMemory( m_auchPassword, sizeof( m_auchPassword ));
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::LogOn() = %d"), fSuccess );

    return ( fSuccess );
}
//PDINU
//===========================================================================
//  FUNCTION :  CTerminal::HostLogOn()  added three variables for IP connection in a 
//		dialog box
//
//  PURPOSE :   Log on to 2170 terminal by Host Name an d password.
//
//  RETURN :    TRUE    - Terminal is logged on.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::HostLogon( const CString& strHostName, const CString& strPassword,  // terminal logged on password
	const USHORT usTerminalNo
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));
    ASSERT( strPassword.GetLength() <= MAX_PCIF_SIZE );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::HostLogOn()") );

    BOOL    fSuccess;

    // --- is the target not the same as current logged on terminal ? ---

    if ( m_usLoggedOnTerminalNo != usTerminalNo )
    {
        // --- at first, attempt to log off from current terminal  ---

        if ( LogOff())
        {
            TCHAR   auchPassword[ MAX_PCIF_SIZE +1]; //UCHAR to TCHAR ESMITH
			TCHAR	auchHostName[ MAX_PCIF_HOSTNAME_SIZE +1];	//UCHAR to TCHAR PDINU

            // --- get PC Interface password and host name ---

			memset( auchPassword, 0, sizeof( auchPassword ));
			memset( auchHostName, 0, sizeof(auchHostName));
			_tcsncpy(auchPassword,strPassword, MAX_PCIF_SIZE);
			_tcsncpy(auchHostName, strHostName, MAX_PCIF_HOSTNAME_SIZE);

            // --- attempt to log on to the target terminal ---

            m_sLastError = ::IspHostLogOn( auchHostName, auchPassword );

			CString str;
            str.Format( _T("\t::IspLogOn() - Called (%d)"), m_sLastError );
            str += CString( "...Host Name:%d, Password:%s" );

           fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;

		       // --- format string by user defined format list ---
		  		
        }
        else
        {
            // --- function is failed ---

            fSuccess = FALSE;
        }
    }
    else
    {
        // --- we already log on this terminal, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current logged on terminal# and password ---

        m_usLoggedOnTerminalNo = usTerminalNo;

		memset( m_auchPassword, 0, (MAX_PCIF_SIZE + 1) * sizeof(TCHAR));
		_tcsncpy(m_auchPassword,strPassword, MAX_PCIF_SIZE);
    }
    else
    {
        // --- an error occurs, clear logged on terminal# and password ---

        m_usLoggedOnTerminalNo = 0;
		memset(m_auchPassword,0, (MAX_PCIF_SIZE + 1) * sizeof(TCHAR));
		memset(m_auchHostName, 0, (MAX_PCIF_HOSTNAME_SIZE + 1) * sizeof(TCHAR));
        ::ZeroMemory( m_auchPassword, sizeof( m_auchPassword ));
		::ZeroMemory( m_auchHostName, sizeof( m_auchHostName));
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::HostLogOn() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::LogOn()
//
//  PURPOSE :   Log on to 2170 terminal by terminal number.
//
//  RETURN :    TRUE    - Terminal is logged on.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::LogOn(
    const USHORT usTerminalNo   // terminal# to log on 
    )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));

    // --- get password which is used by last logged on ---

    CString strPassword( m_auchPassword );

    // --- log on to specified terminal ---

    return ( LogOn( usTerminalNo, strPassword ));
}

//===========================================================================
//
//  FUNCTION :  CTerminal::LogOff()
//
//  PURPOSE :   Log off from the current logged on 2170 terminal
//
//  RETURN :    TRUE    - Terminal is logged off.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::LogOff()
{
    pcsample::TraceFunction( _T("BEGIN >> CTerminal::LogOff()") );

    BOOL    fSuccess;

    // --- is this PC logged on to 2170 terminal now ? ---

    if ( IsLoggedOn())
    {
        // --- unlock keyboard before log off, if keyboard is locked ---

        if ( UnlockKeyboard())
        {
            // --- log off from the current logged on terminal ---

            m_sLastError = ::IspLogOff();

            CString str;
            str.Format( _T("\t::IspLogOff() - Called (%d)"), m_sLastError );
            pcsample::TraceFunction( str );

            fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;
        }
        else
        {
            // --- function is failed ---

            fSuccess = FALSE;
        }
    }
    else
    {
        // --- this PC is not logged on to terminal, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- clear current logged on terminal number ---

        m_usLoggedOnTerminalNo = 0;
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::LogOff() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::LockKeyboard()
//
//  PURPOSE :   Lock the keyboard on 2170 terminal.
//
//  RETURN :    TRUE    - Keyboard is locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::LockKeyboard()
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::LockKeyboard()") );

    BOOL    fSuccess;

    // --- is the keyboard already locked ? ---

    if ( IsKeyboardLocked())
    {
        // --- yes, the keyboard is already locked, do nothing ---

        fSuccess = TRUE;
    }
    else
    {
        // --- lock the keyboard on 2170 ---

        m_sLastError = ::IspLockKeyBoard();

        CString str;
        str.Format( _T("\t::IspLockKeyBoard() - Called (%d)"), m_sLastError );
        pcsample::TraceFunction( str );

        fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;
    }

    // --- save current keyboard lock state to member variable ---

    m_fKeyboardLocked = fSuccess;

    pcsample::TraceFunction( _T("END   >> CTerminal::LockKeyboard() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::UnlockKeyboard()
//
//  PURPOSE :   Unlock the keyboard on 2170 terminal.
//
//  RETURN :    TRUE    - Keyboard is unlocked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::UnlockKeyboard()
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::UnlockKeyboard()") );

    BOOL    fSuccess;

    // --- is the keyboard locked ? ---

    if ( IsKeyboardLocked())
    {
        // --- unlock the locked keyboard ---

        m_sLastError = ::IspUnLockKeyBoard();

        CString str;
        str.Format( _T("\t::IspUnLockKeyBoard() - Called (%d)"), m_sLastError );
        pcsample::TraceFunction( str );

        fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;
    }
    else
    {
        // --- no, the keyboard is not locked, do nothing ---

        fSuccess = TRUE;
    }

    // --- save current keyboard lock state to member variable ---

    m_fKeyboardLocked = ( fSuccess ) ? FALSE : TRUE;

    pcsample::TraceFunction( _T("END   >> CTerminal::UnlockKeyboard() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::ChangePassword()
//
//  PURPOSE :   Change PC Interface password.
//
//  RETURN :    TRUE    - Password is changed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::ChangePassword(
    const CString& strNewPassword   // new password to change
    )
{
    ASSERT( IsLoggedOn());
    ASSERT( strNewPassword.GetLength() <= MAX_PCIF_SIZE );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::ChangePassword()") );

    TCHAR   auchPassword[ MAX_PCIF_SIZE ]; //UCHAR to TCHAR ESMITH

    // --- get new password from user specified string ---

	memset( auchPassword, 0, MAX_PCIF_SIZE * sizeof(TCHAR));
    _tcsncpy(auchPassword,strNewPassword,MAX_PCIF_SIZE);
    // --- attempt to change PC Interface password ---

    m_sLastError = ::IspChangePassWord( auchPassword );

    CString str;
    str.Format( _T("\t::IspChangePassWord() - Called (%d)"), m_sLastError );
    str += CString( "...Password:%s" );
    pcsample::TraceFunction( str, strNewPassword );

    if (  m_sLastError == ISP_SUCCESS )
    {
        // --- save changed password to member variable ---

		memset( m_auchPassword, 0, ( MAX_PCIF_SIZE + 1 ) * sizeof(TCHAR));
        _tcsncpy(m_auchPassword,strNewPassword,MAX_PCIF_SIZE);
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::ChangePassword() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::ReadDateTime()
//
//  PURPOSE :   Get the current system date and time in 2170.
//
//  RETURN :    TRUE    - System time is retrieved.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::ReadDateTime(
    CLIDATETIME& thisTime       // buffer to retrieve system date/time
    )
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::ReadDateTime()") );

    // --- get system date and time in 2170 terminal ---

    m_sLastError = ::IspDateTimeRead( &thisTime );

    CString str;
    str.Format( _T("\t::IspDateTimeRead() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTerminal::ReadDateTime() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::WriteDateTime()
//
//  PURPOSE :   Set the system date and time in 2170.
//
//  RETURN :    TRUE    - System time is updated.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::WriteDateTime(
    const CLIDATETIME& newTime  // new date and time data to write
    )
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::WriteDateTime()") );

    // --- set system date and time in 2170 termial ---

    m_sLastError = ::IspDateTimeWrite( const_cast<CLIDATETIME*>( &newTime ));

    CString str;
    str.Format( _T("\t::IspDateTimeWrite() - Called (%d)"), m_sLastError );
    str += CString( "...%d/%02d/%02d(%d) %02d:%02d:%02d" );
    pcsample::TraceFunction( str,
                             newTime.usYear,
                             newTime.usMonth,
                             newTime.usMDay,
                             newTime.usWDay,
                             newTime.usHour,
                             newTime.usMin,
                             newTime.usSec );

    pcsample::TraceFunction( _T("END   >> CTerminal::WriteDateTime() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::PrintOnJournal()
//
//  PURPOSE :   Print the reset report log on the journal by action code.
//
//  RETURN :    TRUE    - Reset report log is printed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::PrintOnJournal(
    const UCHAR uchActionCode   // action code to print on journal
    )
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::PrintOnJournal()") );

    // --- print the specified action code to the journal printer ---

    m_sLastError = ::IspRecvResetLog( uchActionCode );

    CString str;
    str.Format( _T("\t::IspRecvResetLog() - Called (%d)"), m_sLastError );
    str += CString( "...Action Code#%d" );
    pcsample::TraceFunction( str, uchActionCode );

    pcsample::TraceFunction( _T("END   >> CTerminal::PrintOnJounal() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}


//===========================================================================
//
//  FUNCTION :  CTerminal::PrintOnJournal()
//
//  PURPOSE :   Print the reset report log on the journal by string.
//
//  RETURN :    TRUE    - Reset report log is printed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::PrintOnJournal(
    const CString& strMnemonic  // string to print on journal
    )
{
    ASSERT( IsLoggedOn());
    ASSERT( strMnemonic.GetLength() <= CLI_ISPLOG_LEN );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::PrintOnJournal()") );

    TCHAR   auchLog[ CLI_ISPLOG_LEN ]; //UCHAR to TCHAR ESMITH

    // --- get reset log mnemonic by user specified string ---

    memset( auchLog, 0, CLI_ISPLOG_LEN * sizeof(TCHAR));
    _tcsncpy( auchLog, ( LPCTSTR )strMnemonic, CLI_ISPLOG_LEN);
    // --- print the specified mnemonic to the journal printer ---
    m_sLastError = ::IspRecvResetLog2( auchLog );

    CString str;
    str.Format( _T("\t::IspRecvResetLog2() - Called (%d)"), m_sLastError );
    str += CString( "...Mnemonic:%s" );
    pcsample::TraceFunction( str, strMnemonic );

    pcsample::TraceFunction( _T("END   >> CTerminal::PrintOnJounal() = %d"), ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

BOOL CTerminal::InsertEjNote(
    const CString& strMnemonic  // string to print on journal
    )
{
    ASSERT( IsLoggedOn());
    ASSERT( strMnemonic.GetLength() <= CLI_ISPLOG_LEN );

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::InsertEjNote()") );

    TCHAR   auchLog[ CLI_ISPLOG_LEN ]; //UCHAR to TCHAR ESMITH

    // --- get reset log mnemonic by user specified string ---
    memset( auchLog, 0, CLI_ISPLOG_LEN * sizeof(TCHAR));
    _tcsncpy( auchLog, ( LPCTSTR )strMnemonic, CLI_ISPLOG_LEN);
    // --- print the specified mnemonic to the journal printer ---

    m_sLastError = ::IspRecvResetLog3( auchLog );

    CString str;
    str.Format( _T("\t::IspRecvResetLog3() - Called (%d)"), m_sLastError );
    str += CString( "...Mnemonic: %s" );
    pcsample::TraceFunction( str, strMnemonic );

    pcsample::TraceFunction( _T("END   >> CTerminal::InsertEjNote() = %d"), ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::ReadSystemParameter()
//
//  PURPOSE :   Read the system configuration parameter in 2170 terminal
//
//  RETURN :    TRUE    - Reset report log is printed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::ReadSystemParameter()
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::ReadSystemParameter()") );

    // --- get system configuration parameter in 2170 ---

    m_sLastError = ::IspSysConfig( &m_paraSystem );

    CString str;
    str.Format( _T("\t::IspSysConfig() - Called (%d)"), m_sLastError );
    pcsample::TraceFunction( str );

    pcsample::TraceFunction( _T("END   >> CTerminal::ReadSystemParameter() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CTerminal::GetNoOfTerminals()
//
//  PURPOSE :   Get the number of terminals in the cluster.
//
//  RETURN :    TRUE    - no. of terminal is retrieved.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTerminal::GetNoOfTerminals(
    UCHAR& uchNoOfTerminals     // variable to recieve no. of terminals
    )
{
    ASSERT( IsLoggedOn());

    pcsample::TraceFunction( _T("BEGIN >> CTerminal::GetNoOfTerminals()") );

    // --- get no. of terminal without master terminal ---

    m_sLastError = ::IspNoOfSatellite( &uchNoOfTerminals );

    CString str;
    str.Format( _T("\t::IspNoOfSattelite() - Called (%d)"), m_sLastError );
    str += CString( "...No of Sattelite#%d" );
    pcsample::TraceFunction( str, uchNoOfTerminals );

    if ( m_sLastError == ISP_SUCCESS )
    {
        if ( uchNoOfTerminals < MAX_TERMINAL_NO )
        {
            // --- adjust no. of terminal because of master terminal ---

            uchNoOfTerminals++;
        }
        else
        {
            uchNoOfTerminals = MAX_TERMINAL_NO;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTerminal::GetNoOfTerminals() = %d"),
                             ( m_sLastError == ISP_SUCCESS ));

    return ( m_sLastError == ISP_SUCCESS );
}

//////////////////// END OF FILE ( Terminal.cpp) ////////////////////////////
