/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Terminal.h
//
//  PURPOSE:    Terminal class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.01 : 1999-07-12 : Corrected the definitions of Server Lock 
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TERMINAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//            C O N S T A N T    V A L U E s
//
/////////////////////////////////////////////////////////////////////////////

namespace pcterm
{
const ULONG  LOCK_ALL_OPERATOR      = 0;
const USHORT LOCK_ALL_TERMINAL      = 0;

const USHORT MAX_DEVICENAME_LEN     = 32;

const USHORT MAX_SYSTEMID_LEN       = 64;
const USHORT MAX_REMOTE_NO          = 2;
const USHORT MAX_ROM_NO             = 2;
const USHORT MAX_RAM_NO             = 5;
const UCHAR  MAX_DRAWER_NO          = 2;

const UCHAR  MASK_DATABITS          = 0x03;
const UCHAR  MASK_PARITYBITS        = 0x18;
const UCHAR  MASK_STOPBITS          = 0x04;
const UCHAR  STOP_BITS_1            = 0x00;
const UCHAR  STOP_BITS_2            = 0x04;
const UCHAR  DATA_BITS_8            = 0x03;
const UCHAR  DATA_BITS_7            = 0x02;
const UCHAR  PARITY_BITS_ODD        = 0x08;
const UCHAR  PARITY_BITS_EVEN       = 0x18;
const UCHAR  PARITY_BITS_NONE       = 0x00;

const UCHAR  DEVICE_NONE            = 0;
const UCHAR  DEVICE_THERMAL_PRINTER = 11;
const UCHAR  DEVICE_SLIP_PRINTER    = 2;
const UCHAR  DEVICE_RJ_PRINTER      = 99;
const UCHAR  DEVICE_ERROR           = 0xFF;

const UCHAR  DISP_NONE              = 0x00;
const UCHAR  DISP_2X20              = 0x80;
const UCHAR  DISP_10N10D            = 0x90;
const UCHAR  DISP_LCD               = 0xA0;
const UCHAR  DISP_4X20              = 0xB0; /* 2172 */

const UCHAR  KEYBOARD_NONE          = 0x00;
const UCHAR	 KEYBOARD_TOUCH         = 0x02;	 // TOUCHSCREEN
const UCHAR  KEYBOARD_CONVENTION    = 0x10;  // NCR 7448 terminal with Conventional keyboard
const UCHAR  KEYBOARD_MICRO         = 0x12;  // NCR 7448 terminal with Micromotion keyboard
const UCHAR  KEYBOARD_WEDGE_68      = /*0x14*/0x22;  // 68 key NCR 5932 Wedge keyboard
const UCHAR  KEYBOARD_PCSTD_102     = 0x20;  // standard PC 102 key keyboard

const UCHAR  WAITER_NONE            = 0x00;
const UCHAR  WAITER_PEN             = 0x01;
const UCHAR  WAITER_LOCK            = 0x02;
/* const UCHAR  WAITER_LOCK            = 0x01; */
/* const UCHAR  WAITER_PEN             = 0x02; */

const UCHAR  IMSR_NONE              = 0x00;
const UCHAR  IMSR_TRACK12           = 0x03;
const UCHAR  IMSR_TRACK2            = 0x02;
const UCHAR  IMSR_TRACK2J           = 0x0A;

const UCHAR  DRAWER_NONE            = 0x00;
const UCHAR  DRAWER_PROVIDED        = 0xFF;

const UCHAR  LOAD_DEVICE_COM        = 1;
const UCHAR  LOAD_DEVICE_MODEM      = 2;

const UCHAR  RS232_PROVIDED         = 0xFF;
const UCHAR  RS232_NONE             = 0x00;

const UCHAR  IHC_PROVIDED           = 0xFF;
const UCHAR  IHC_NCR_DLC            = 0xFE;
const UCHAR  IHC_NONE               = 0x00;
}

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CTerminal
//---------------------------------------------------------------------------

class CTerminal
{
public:
    CTerminal();
    ~CTerminal();


	BOOL	LogOn( const USHORT IPAddy1,const USHORT IPAddy2,const USHORT IPAddy3,
														const USHORT IPAddy4,
														const CString& strPassword,
														const USHORT usTerminalNo);
    
	BOOL    LogOn(	const USHORT usTerminalNo,
                   const CString& strPassword );
    BOOL    LogOn( const USHORT usTerminalNo );
	BOOL	HostLogon( const CString& strHostName, const CString& strPassword, const USHORT usTerminalNo);	//PDINU
    BOOL    LogOff();

    BOOL    LockKeyboard();
    BOOL    UnlockKeyboard();
    BOOL    ChangePassword( const CString& strNewPassword );
    BOOL    ReadDateTime( CLIDATETIME& thisTime );
    BOOL    WriteDateTime( const CLIDATETIME& newTime );
    BOOL    ReadSystemParameter();
    BOOL    PrintOnJournal( const UCHAR uchActionCode );
    BOOL    PrintOnJournal( const CString& strMnemonic );
    BOOL    GetNoOfTerminals( UCHAR& uchNoOfTerminals );
    BOOL    InsertEjNote( const CString& strMnemonic );

    BOOL    IsLoggedOn() const;
    BOOL    IsKeyboardLocked() const;
    USHORT  GetLoggedOnTerminalNo() const;
    SHORT   GetLastError() const;

    UCHAR   getTerminalNo() const;
    WCHAR   getPeripheral( const USHORT usPortNo ) const;
    LPTSTR  getDeviceName( const USHORT usPortNo, LPTSTR lpszSystemID ) const;
    USHORT  getBaudRate( const USHORT usPortNo ) const;
    UCHAR   getDataBits( const USHORT usPortNo ) const;
    UCHAR   getParityBits( const USHORT usPortNo ) const;
    UCHAR   getStopBits( const USHORT usPortNo ) const;
    USHORT  getOption( const USHORT usOptionNo ) const;
    UCHAR   getRemotePeripheral( const USHORT usRemoteNo,
                                 const USHORT usPortNo ) const;
    UCHAR   getLoadDevice() const;
    UCHAR   getLoadPortNo() const;
    USHORT  getLoadBaudRate() const;
    LPTSTR  getSystemID( LPTSTR lpszSystemID ) const;
    LPTSTR  getApplicationID( LPTSTR lpszApplicationID ) const;
    USHORT  getROMSize( const USHORT usROMNo ) const;
    USHORT  getRAMSize( const USHORT usRAMNo ) const;
    UCHAR   getRS232Board() const;
    UCHAR   getIHCBoard() const;
    WCHAR   getPrinter() const;
    UCHAR   getSlipPrinter() const;
    UCHAR   getOperatorDisplay() const;
    UCHAR   getCustomerDisplay() const;
    UCHAR   getKeyboard() const;
    UCHAR   getOperatorLock() const;
    UCHAR   getMSR() const;
    UCHAR   getDrawer( const USHORT usDrawerNo ) const;
	ULONG   getFreeBytes() const;		// ADD Saratoga
	ULONG   getTotalBytes() const;		// ADD Saratoga
	ULONG   getTotalFreeBytes() const;	// ADD Saratoga

private:
    USHORT      m_usLoggedOnTerminalNo;
    TCHAR       m_auchPassword[ MAX_PCIF_SIZE + 1 ]; //ESMITH
	TCHAR		m_auchHostName[ MAX_PCIF_HOSTNAME_SIZE + 1 ];	//PDINU
	TCHAR		m_auchDbFileName[ MAX_PATH + 1 ];
    BOOL        m_fKeyboardLocked;
    CLISYSPARA  m_paraSystem;
    SHORT       m_sLastError;
};

//---------------------------------------------------------------------------
//  CSerialInfo
//---------------------------------------------------------------------------

class CSerialInfo
{
public:
    CSerialInfo();
    ~CSerialInfo();

    UCHAR   m_uchPortNo;
    USHORT  m_usBaudRate;
    UCHAR   m_uchDataBits;
    UCHAR   m_uchParityBits;
    UCHAR   m_uchStopBits;
};

//---------------------------------------------------------------------------
//  CPcIfPort
//---------------------------------------------------------------------------

class CPcIfPort
{
public:
    CPcIfPort();
    ~CPcIfPort();

    BOOL    Open( const CSerialInfo& infoSerial );
    BOOL    Close();
	BOOL    OpenEx(const USHORT usFunc);	// ###ADD Saratoga
	BOOL	CloseEx(const USHORT usFunc);	// ###ADD Saratoga
	BOOL    CloseEx();
    BOOL    ConnectModem( const CString& strATCommand,
                          const CString& strPhoneNumber,
                          const int nPhoneType );
    BOOL    DisconnectModem();
    BOOL    IsOpened() const;
    BOOL    IsModemConnected() const;
    SHORT   GetLastError() const;

	BOOL    IsLanConnected() const;

private:
    BOOL    m_fOpened;
    BOOL    m_fModemConnected;
    SHORT   m_sLastError;
	BOOL    m_fLanConnected;
};

//---------------------------------------------------------------------------
//  CLockObject
//---------------------------------------------------------------------------

class CLockObject
{
public:
    CLockObject();
    virtual ~CLockObject();

    virtual BOOL    Lock( const ULONG  ulObjectNo ) = 0;
    virtual BOOL    Lock( const USHORT usObjectNo ) = 0;
    virtual BOOL    Unlock() = 0;
    virtual BOOL    IsLocked() const;

    virtual SHORT   GetLastError() const;

protected:
    BOOL    m_fLocked;
    SHORT   m_sLastError;
};

//---------------------------------------------------------------------------
//  COperatorLock
//---------------------------------------------------------------------------

class COperatorLock : public CLockObject
{
public:
    COperatorLock();
    ~COperatorLock();

    BOOL    Lock( const ULONG ulOperatorNo );
    BOOL    Unlock();

    ULONG   GetLockedOperator() const;

private:
    ULONG   m_ulCurrentLockedOperator;
    BOOL    Lock( const USHORT usRecordNo );
};

//---------------------------------------------------------------------------
//  CSignInLock
//---------------------------------------------------------------------------

class CSignInLock : public CLockObject
{
public:
    CSignInLock();
    ~CSignInLock();

    BOOL    Lock( const USHORT usTerminalNo );
    BOOL    Unlock();

    USHORT  GetLockedTerminal() const;

private:
    USHORT  m_usCurrentLockedTerminal;
    BOOL    Lock( const ULONG ulRecordNo );
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  CSerialInfo class
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CSerialInfo::CSerialInfo()
//===========================================================================

inline CSerialInfo::CSerialInfo() :
    m_uchPortNo( 1 ),
    m_usBaudRate( 19200 ),
    m_uchDataBits( 8 ),
    m_uchParityBits( 0 ),
    m_uchStopBits( 1 )
{
}

//===========================================================================
//  FUNCTION :  CSerialInfo::~CSerialInfo()
//===========================================================================

inline CSerialInfo::~CSerialInfo()
{
}

//---------------------------------------------------------------------------
//
//  CTerminal class
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTerminal::IsLoggedOn() 
//===========================================================================

inline BOOL CTerminal::IsLoggedOn() const
{
    return (( m_usLoggedOnTerminalNo != 0 ) ? TRUE : FALSE );
}

//===========================================================================
//  FUNCTION :  CTerminal::IsKeyboardLocked() 
//===========================================================================

inline BOOL CTerminal::IsKeyboardLocked() const
{
    return ( m_fKeyboardLocked );
}

//===========================================================================
//  FUNCTION :  CTerminal::GetLoggedOnTerminalNo() 
//===========================================================================

inline USHORT CTerminal::GetLoggedOnTerminalNo() const
{
    return ( m_usLoggedOnTerminalNo );
}

//===========================================================================
//  FUNCTION :  CTerminal::GetLastError() 
//===========================================================================

inline SHORT CTerminal::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CTerminal::getTerminalNo() 
//===========================================================================

inline UCHAR CTerminal::getTerminalNo() const
{
    return ( m_paraSystem.uchTerminal );
}


//===========================================================================
//  FUNCTION :  CTerminal::getPeripheral()
//===========================================================================

inline WCHAR CTerminal::getPeripheral(
    const USHORT usPortNo
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    return ( m_paraSystem.auchComPort[ usPortNo ] );
}

//===========================================================================
//  FUNCTION :  CTerminal::getDeviceName()
//===========================================================================

inline LPTSTR CTerminal::getDeviceName(
    const USHORT usPortNo,
    LPTSTR lpszDeviceName
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    ::ZeroMemory( lpszDeviceName, pcterm::MAX_DEVICENAME_LEN + 1 );
    ::CopyMemory( lpszDeviceName,
                  &m_paraSystem.auchComDeviceName[usPortNo][0],
                  pcterm::MAX_DEVICENAME_LEN );

    return ( lpszDeviceName );
}


//===========================================================================
//  FUNCTION :  CTerminal::getBaudRate()
//===========================================================================

inline USHORT CTerminal::getBaudRate(
    const USHORT usPortNo
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    return ( m_paraSystem.ausComBaud[ usPortNo ] );
}

//===========================================================================
//  FUNCTION :  CTerminal::getDataBits()
//===========================================================================

inline UCHAR CTerminal::getDataBits(
    const USHORT usPortNo
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    UCHAR   uchDataBits = m_paraSystem.auchComByteFormat[ usPortNo ];

    uchDataBits &= pcterm::MASK_DATABITS;

    return (  uchDataBits );
}

//===========================================================================
//  FUNCTION :  CTerminal::getParityBits()
//===========================================================================

inline UCHAR CTerminal::getParityBits(
    const USHORT usPortNo
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    UCHAR   uchDataBits = m_paraSystem.auchComByteFormat[ usPortNo ];

    uchDataBits &= pcterm::MASK_PARITYBITS;

    return (  uchDataBits );
}

//===========================================================================
//  FUNCTION :  CTerminal::getStopBits()
//===========================================================================

inline UCHAR CTerminal::getStopBits(
    const USHORT usPortNo
    ) const
{
    ASSERT( usPortNo < CLI_LEN_PORT );

    UCHAR   uchDataBits = m_paraSystem.auchComByteFormat[ usPortNo ];

    uchDataBits &= pcterm::MASK_STOPBITS;

    return (  uchDataBits );
}

//===========================================================================
//  FUNCTION :  CTerminal::getOption()
//===========================================================================

inline USHORT CTerminal::getOption(
    const USHORT usOptionNo
    ) const
{
    ASSERT( usOptionNo < CLI_LEN_OPTION );

    return ( m_paraSystem.ausOption[ usOptionNo ] );
}

//===========================================================================
//  FUNCTION :  CTerminal::getRemotePeripheral()
//===========================================================================

inline UCHAR CTerminal::getRemotePeripheral(
    const USHORT usRemoteNo,
    const USHORT usPortNo
    ) const
{
    ASSERT( usRemoteNo < pcterm::MAX_REMOTE_NO );
    ASSERT( usPortNo   < CLI_LEN_RMT_PORT );

    return ( m_paraSystem.auchRmtPort[ usRemoteNo ][ usPortNo ] );
}

//===========================================================================
//  FUNCTION :  CTerminal::getLoadDevice()
//===========================================================================

inline UCHAR CTerminal::getLoadDevice() const
{
    return ( m_paraSystem.uchLoadDevice );
}

//===========================================================================
//  FUNCTION :  CTerminal::getLoadPortNo()
//===========================================================================

inline UCHAR CTerminal::getLoadPortNo() const
{
    return ( m_paraSystem.uchLoadCom );
}

//===========================================================================
//  FUNCTION :  CTerminal::getLoadBuadRate()
//===========================================================================

inline USHORT CTerminal::getLoadBaudRate() const
{
    return ( m_paraSystem.usLoadBaud );
}

//===========================================================================
//  FUNCTION :  CTerminal::getSystemID()
//===========================================================================

inline LPTSTR CTerminal::getSystemID(
    LPTSTR lpszSystemID
    ) const
{
    ASSERT( ! IsBadWritePtr( lpszSystemID, pcterm::MAX_SYSTEMID_LEN + 1 ));

    ::ZeroMemory( lpszSystemID, pcterm::MAX_SYSTEMID_LEN + 1 );
    ::CopyMemory( lpszSystemID,
                  m_paraSystem.SystemId,
                  pcterm::MAX_SYSTEMID_LEN );

    return ( lpszSystemID );
}

//===========================================================================
//  FUNCTION :  CTerminal::getApplicationID()
//===========================================================================

inline LPTSTR CTerminal::getApplicationID(
    LPTSTR lpszApplicationID
    ) const
{
    ASSERT( ! IsBadWritePtr( lpszApplicationID, CLI_LEN_SOFTWARE_ID + 1 ));

    ::ZeroMemory( lpszApplicationID, CLI_LEN_SOFTWARE_ID + 1 );
    ::CopyMemory( lpszApplicationID,
                  m_paraSystem.ApplicationId,
                  CLI_LEN_SOFTWARE_ID );

    return ( lpszApplicationID );
}

//===========================================================================
//  FUNCTION :  CTerminal::getROMSize()
//===========================================================================

inline USHORT CTerminal::getROMSize(
    const USHORT usROMNo
    ) const
{
    ASSERT( usROMNo < pcterm::MAX_ROM_NO );

    USHORT  usSizeInKB;

    switch ( usROMNo )
    {
    case 0:
        usSizeInKB = m_paraSystem.uchROM1Size;
        break;
    default:
        usSizeInKB = m_paraSystem.uchROM2Size;
        break;
    }

    usSizeInKB *= 64;

    return ( usSizeInKB );
}

//===========================================================================
//  FUNCTION :  CTerminal::getRAMSize()
//===========================================================================

inline USHORT CTerminal::getRAMSize(
    const USHORT usRAMNo
    ) const
{
    ASSERT( usRAMNo < pcterm::MAX_RAM_NO );

    USHORT  usSizeInKB;

    switch ( usRAMNo )
    {
    case 0:
        usSizeInKB = m_paraSystem.uchSRAM1Size;
        break;
    case 1:
        usSizeInKB = m_paraSystem.uchSRAM2Size;
        break;
    case 2:
        usSizeInKB = m_paraSystem.uchSRAM3Size;
        break;
    case 3:
        usSizeInKB = m_paraSystem.uchSRAM4Size;
        break;
    default:
        usSizeInKB = m_paraSystem.uchSRAM5Size;
        break;
    }

    usSizeInKB *= 64;

    return ( usSizeInKB );
}

//===========================================================================
//  FUNCTION :  CTerminal::getRS232Board()
//===========================================================================

inline UCHAR CTerminal::getRS232Board() const
{
    return ( m_paraSystem.uchCom );
}

//===========================================================================
//  FUNCTION :  CTerminal::getIHCBoard()
//===========================================================================

inline UCHAR CTerminal::getIHCBoard() const
{
    return ( m_paraSystem.uchNet );
}

//===========================================================================
//  FUNCTION :  CTerminal::getPrinter()
//===========================================================================

inline WCHAR CTerminal::getPrinter() const
{
    return ( m_paraSystem.auchComPort[ 0 ] );
}

//===========================================================================
//  FUNCTION :  CTerminal::getSlipPrinter()
//===========================================================================

inline UCHAR CTerminal::getSlipPrinter() const
{
    BOOL    fFound;
    UCHAR   uchDeviceID;
    UCHAR   uchI;

    fFound = FALSE;
    uchI   = 0;

    while (( ! fFound ) && ( uchI < CLI_LEN_PORT ))
    {
        if ( m_paraSystem.auchComPort[ uchI ] == pcterm::DEVICE_SLIP_PRINTER )
        {
            fFound = TRUE;
        }
        else
        {
            uchI++;
        }
    }

    if ( fFound )
    {
        if ( m_paraSystem.usErrorStatusS == 0 )
        {
            uchDeviceID = pcterm::DEVICE_SLIP_PRINTER;
        }
        else
        {
            uchDeviceID = pcterm::DEVICE_ERROR;
        }
    }
    else
    {
        uchDeviceID = pcterm::DEVICE_NONE;
    }
    return ( uchDeviceID );
}

//===========================================================================
//  FUNCTION :  CTerminal::getOperatorDisplay()
//===========================================================================

inline UCHAR CTerminal::getOperatorDisplay() const
{
    return ( m_paraSystem.uchOperType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getCustomerDisplay()
//===========================================================================

inline UCHAR CTerminal::getCustomerDisplay() const
{
    return ( m_paraSystem.uchCustType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getKeyboard()
//===========================================================================

inline UCHAR CTerminal::getKeyboard() const
{
    return ( m_paraSystem.uchKeyType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getOperatorLock()
//===========================================================================

inline UCHAR CTerminal::getOperatorLock() const
{
    return ( m_paraSystem.uchWaiterType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getMSR()
//===========================================================================

inline UCHAR CTerminal::getMSR() const
{
    return ( m_paraSystem.uchIntegMSRType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getDrawer()
//===========================================================================

inline UCHAR CTerminal::getDrawer(
    const USHORT usDrawerNo
    ) const
{
    ASSERT( usDrawerNo < pcterm::MAX_DRAWER_NO );

    UCHAR   uchDrawerType;

    switch ( usDrawerNo )
    {
    case 0:
        uchDrawerType = m_paraSystem.uchDrawer1;
        break;

    default:
        uchDrawerType = m_paraSystem.uchDrawer2;
        break;
    }
    return ( uchDrawerType );
}

//===========================================================================
//  FUNCTION :  CTerminal::getFreeBytes(), ADD Saratoga
//===========================================================================

inline ULONG CTerminal::getFreeBytes() const
{
    return ( m_paraSystem.ulFreeBytesAvailableToCaller );
}

//===========================================================================
//  FUNCTION :  CTerminal::getTotalBytes(), ADD Saratoga
//===========================================================================

inline ULONG CTerminal::getTotalBytes() const
{
    return ( m_paraSystem.ulTotalNumberOfBytes );
}

//===========================================================================
//  FUNCTION :  CTerminal::getTotalFreeBytes(), ADD Saratoga
//===========================================================================

inline ULONG CTerminal::getTotalFreeBytes() const
{
    return ( m_paraSystem.ulTotalNumberOfFreeBytes );
}

/////////////////////////////////////////////////////////////////////////////
//
//  CPcIfPort class
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  FUNCTION :  CPcIfPort::IsOpened()
//===========================================================================

inline BOOL CPcIfPort::IsOpened() const
{
    return ( m_fOpened );
}

//===========================================================================
//  FUNCTION :  CPcIfPort::IsModemConnected()
//===========================================================================

inline BOOL CPcIfPort::IsModemConnected() const
{
    return ( m_fModemConnected );
}

//===========================================================================
//  FUNCTION :  CPcIfPort::IsLanConnected()	// ###ADD Saratoga
//===========================================================================

inline BOOL CPcIfPort::IsLanConnected() const
{
    return ( m_fLanConnected );
}

//===========================================================================
//  FUNCTION :  CPcIfPort::GetLastError()
//===========================================================================

inline SHORT CPcIfPort::GetLastError() const
{
    return ( m_sLastError );
}

/////////////////////////////////////////////////////////////////////////////
//
//  CLockObject class
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  FUNCTION :  CLockObject::CLockObject()
//===========================================================================

inline CLockObject::CLockObject() :
    m_fLocked( FALSE ),
    m_sLastError( SUCCESS )
{
}

//===========================================================================
//  FUNCTION :  CLockObject::~CLockObject()
//===========================================================================

inline CLockObject::~CLockObject()
{
}

//===========================================================================
//  FUNCTION :  CLockObject::Lock()
//===========================================================================

inline BOOL CLockObject::Lock( const ULONG ulObjectNo )
{
    UNREFERENCED_PARAMETER( ulObjectNo );
    ASSERT( FALSE );
    return ( FALSE );
}
inline BOOL CLockObject::Lock( const USHORT usObjectNo )
{
    UNREFERENCED_PARAMETER( usObjectNo );
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CLockObject::Unlock()
//===========================================================================

inline BOOL CLockObject::Unlock()
{
    ASSERT( FALSE );
    return ( FALSE );
}

//===========================================================================
//  FUNCTION :  CLockObject::IsLocked()
//===========================================================================

inline BOOL CLockObject::IsLocked() const
{
    return ( m_fLocked );
}

//===========================================================================
//  FUNCTION :  CLockObject::GetLastError()
//===========================================================================

inline SHORT CLockObject::GetLastError() const
{
    return ( m_sLastError );
}

/////////////////////////////////////////////////////////////////////////////
//
//  COperatorLock class
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  FUNCTION :  COperatorLock::GetLockedOperator()
//===========================================================================

inline ULONG  COperatorLock::GetLockedOperator() const
{
    return ( m_ulCurrentLockedOperator );
}

/////////////////////////////////////////////////////////////////////////////
//
//  CSignInLock class
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  FUNCTION :  CSignInLock::GetLockedOperator()
//===========================================================================

inline USHORT CSignInLock::GetLockedTerminal() const
{
    return ( m_usCurrentLockedTerminal );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////////// END OF FILE ( Terminal.h ) ///////////////////////////
