/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PcIfPort.cpp
//
//  PURPOSE:    PC Interface class definitions and declarations.
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
//  FUNCTION :  CPcIfPort::CPcIfPort()
//
//  PURPOSE :   Constructor of PC Interface class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPcIfPort::CPcIfPort() :
    m_fOpened( FALSE ),
    m_fModemConnected( FALSE ),
	m_fLanConnected(FALSE),		// ###Add Saratoga
    m_sLastError( ISP_SUCCESS )
{
}

//===========================================================================
//
//  FUNCTION :  CPcIfPort::~CPcIfPort()
//
//  PURPOSE :   Destructor of PC Interface class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPcIfPort::~CPcIfPort()
{
    // --- close communication port before this object is discarded ---

    Close();
	CloseEx(PCIF_FUNC_CLOSE_LAN);	// ###ADD Saratoga
    DisconnectModem();
}

//===========================================================================
//
//  FUNCTION :  CPcIfPort::Open()
//
//  PURPOSE :   Open serial port with the specified configuration.
//
//  RETURN :    TRUE    - Serial port is opened.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::Open(
    const CSerialInfo& infoSerial   // configuration of serial communication
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CPcIfPort::Open()") );

    BOOL    fSuccess;

    // --- at first, close the current opened port ---

    if ( Close())
    {
        COMCONTROL  ComCtrl;

        // --- get configuration of serial communication ---

        ComCtrl.uchPort     = infoSerial.m_uchPortNo;
        ComCtrl.usBaudRate  = infoSerial.m_usBaudRate;
        ComCtrl.uchByteSize = infoSerial.m_uchDataBits;
        ComCtrl.uchParity   = infoSerial.m_uchParityBits;
        ComCtrl.uchStopBits = infoSerial.m_uchStopBits;

        // --- open serial port with user specified configuration ---

        m_sLastError = ::PcifOpen( &ComCtrl );

        fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;

        CString str;
        str.Format( _T("\t::PcifOpen() - Called (%d)"), m_sLastError );
        str += CString( "...COM%d, %d, %d, %d, %d" );
        pcsample::TraceFunction( str,
                                 ComCtrl.uchPort,
                                 ComCtrl.usBaudRate,
                                 ComCtrl.uchByteSize,
                                 ComCtrl.uchParity,
                                 ComCtrl.uchStopBits );
    }
    else
    {
        // --- function is failed ---

        fSuccess = FALSE;
    }

    // --- save current port open state to member variable ---

    m_fOpened = fSuccess;

    pcsample::TraceFunction( _T("END   >> CPcIfPort::Open() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPcIfPort::OpenEx()
//
//  PURPOSE :   Open serial port with the specified configuration.
//
//  RETURN :    TRUE    - Serial port is opened.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::OpenEx(
    const USHORT usFunc
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CPcIfPort::OpenEx()") );

    BOOL    fSuccess;

    // --- at first, close the current opened port ---

    if ( CloseEx(usFunc))
    {
        // --- open serial port with user specified configuration ---

        m_sLastError = ::PcifOpenEx( usFunc, NULL );

        fSuccess = ( m_sLastError == PCIF_SUCCESS ) ? TRUE : FALSE;

	    pcsample::TraceFunction( _T("SUCCESS   >> CPcIfPort::Open() = %d"),
		                         fSuccess );
		m_fLanConnected = fSuccess;
    }
    else
    {
        // --- function is failed ---

        fSuccess = FALSE;

    }

    // --- save current port open state to member variable ---

    m_fOpened = fSuccess;

    pcsample::TraceFunction( _T("END   >> CPcIfPort::OpenEx() = %d"),
                             fSuccess );

    return ( fSuccess );
}


//===========================================================================
//
//  FUNCTION :  CPcIfPort::CloseEx()
//
//  PURPOSE :   Close opened Ethernet.
//
//  RETURN :    TRUE    - Serial port is closed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::CloseEx(const USHORT usFunc)
{
	pcsample::TraceFunction(_T("BEGIN >> CPcIfPort::CloseEx()"));

	BOOL	fSuccess;

	// --- is opened now ?

	if(IsOpened())
	{
        // --- yes, port is opened and close it now ---

        m_sLastError = ::PcifCloseEx(usFunc, NULL);

        fSuccess = ( m_sLastError == PCIF_SUCCESS ) ? TRUE : FALSE;

		m_fLanConnected = FALSE;
        pcsample::TraceFunction( _T("\t::PcifCloseEx() - Called (%d)"),
                                 m_sLastError );
	}
    else
    {
        // --- no, port is not opened, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current port open state to member variable ---

        m_fOpened = FALSE;
    }

    pcsample::TraceFunction( _T("END   >> CPcIfPort::CloseEx() = %d"),
                             fSuccess );

    return ( fSuccess );
}
//===========================================================================
//
//  FUNCTION :  CPcIfPort::Close()
//
//  PURPOSE :   Close opened serial port.
//
//  RETURN :    TRUE    - Serial port is closed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::Close()
{
    pcsample::TraceFunction( _T("BEGIN >> CPcIfPort::Close()") );

    BOOL    fSuccess;

    // --- is the serial port opened now ? ---

	if ( IsLanConnected())
	{
		fSuccess = TRUE;
	}

    if ( IsOpened())
    {
        // --- yes, port is opened and close it now ---

        m_sLastError = ::PcifClose();

        fSuccess = ( m_sLastError == ISP_SUCCESS ) ? TRUE : FALSE;

        pcsample::TraceFunction( _T("\t::PcifClose() - Called (%d)"),
                                 m_sLastError );
    }
    else
    {
        // --- no, port is not opened, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current port open state to member variable ---

        m_fOpened = FALSE;
    }

    pcsample::TraceFunction( _T("END   >> CPcIfPort::Close() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPcIfPort::ConnectModem()
//
//  PURPOSE :   Connect modem with the specified AT command.
//
//  RETURN :    TRUE    - Modem is opened.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::ConnectModem(
    const CString& strATCommand,    // AT command to send modem
    const CString& strPhoneNumber,  // Phone number to send medem
    const int nPhoneType            // Type of phone (pulse or tone)
    )
{
    pcsample::TraceFunction( _T("BEGIN >> CPcIfPort::ConnectModem()") );

    BOOL    fSuccess;

    // --- at first, close the current opened ---

    if ( DisconnectModem())
    {
        UCHAR   auchCommand[ PARA_COMMAND_LEN ];
		memset(auchCommand, 0, PARA_COMMAND_LEN );
		int i;	//i will be used later
        for (i = 0; i < strATCommand.GetLength(); i++) {
			auchCommand [i] = (UCHAR)strATCommand [i];
		}
		strncpy((CHAR*)&auchCommand[strATCommand.GetLength()],"\r\n", sizeof(UCHAR) * 2);

        /*::ZeroMemory( auchCommand, sizeof( auchCommand ));
        ::CopyMemory( auchCommand,
                      ( LPCTSTR )strATCommand,
                      strATCommand.GetLength());
        ::CopyMemory( &auchCommand[ strATCommand.GetLength() ],
                      "\r\n",
                      sizeof( UCHAR ) * 2 );*/

        // --- open serial port with user specified configuration ---

        m_sLastError = ::PcifConnect( auchCommand );

        if (( m_sLastError == PCIF_RESULT_OK ) ||
            ( m_sLastError == PCIF_RESULT_CONNECT ))
        {
			memset(auchCommand, 0,  PARA_COMMAND_LEN);
            //::ZeroMemory( auchCommand, sizeof( auchCommand ));

            if ( nPhoneType == 1 )
            {
				strncpy((CHAR*)auchCommand,"DT", sizeof(UCHAR) * 2);
                /*::CopyMemory( auchCommand,
                              "DT",
                              sizeof( UCHAR ) * 2 );*/
            }
            else
            {
                strncpy((CHAR*)auchCommand,"DP", sizeof(UCHAR) * 2);
               /* ::CopyMemory( auchCommand,
                              "DP",
                              sizeof( UCHAR ) * 2 );*/
            }

			for (i = 0; i < strPhoneNumber.GetLength(); i++) {
				auchCommand [i + 2] = (UCHAR)strPhoneNumber [i];
			}
			strncpy((CHAR*)&auchCommand[2 + strPhoneNumber.GetLength()],"\r\n", sizeof(UCHAR) * 2);
            /*::CopyMemory( &auchCommand[ 2 ],
                          ( LPCTSTR )strPhoneNumber,
                          strPhoneNumber.GetLength());
            ::CopyMemory( &auchCommand[ 2 + strPhoneNumber.GetLength() ],
                          "\r\n",
                          sizeof( UCHAR ) * 2 );*/

            m_sLastError = ::PcifConnect( auchCommand );

            fSuccess = ( 10 <= m_sLastError) ? TRUE : FALSE;
        }
        else
        {
            fSuccess = FALSE;
        }

        CString str;
        str.Format( _T("\t::PcifConnect() - Called (%d)"), m_sLastError );
        str += CString( "...COMMAND:%s" );
        pcsample::TraceFunction( str, ( LPCTSTR )strATCommand );
    }
    else
    {
        // --- function is failed ---

        fSuccess = FALSE;
    }

    // --- save current modem open state to member variable ---

    m_fModemConnected = fSuccess;

    pcsample::TraceFunction( _T("END   >> CPcIfPort::ConnectModem() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPcIfPort::DisconnectModem()
//
//  PURPOSE :   Close opened modem.
//
//  RETURN :    TRUE    - Modem is closed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPcIfPort::DisconnectModem()
{
    pcsample::TraceFunction( _T("BEGIN >> CPcIfPort::DisconnectModem()") );

    BOOL    fSuccess;

    // --- is the serial port opened now ? ---

    if ( IsModemConnected())
    {
        // --- yes, port is opened and close it now ---

        m_sLastError = ::PcifDisconnect();

        fSuccess = (( m_sLastError == PCIF_RESULT_OK ) ||
                    ( m_sLastError == 3 )) ? TRUE : FALSE;

        pcsample::TraceFunction( _T("\t::PcifDisconnect() - Called (%d)"),
                                 m_sLastError );
    }
    else
    {
        // --- no, modem is not opened, do nothing ---

        fSuccess = TRUE;
    }

    if ( fSuccess )
    {
        // --- save current modem open state to member variable ---

        m_fModemConnected = FALSE;
    }

    pcsample::TraceFunction( _T("END   >> CPcIfPort::DisconnectModem() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//////////////////// END OF FILE ( PcIfPort.cpp ) ///////////////////////////
