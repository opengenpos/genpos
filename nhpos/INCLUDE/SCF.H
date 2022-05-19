/*************************************************************************
 *
 * SCF.h
 *
 * $Workfile:: SCF.h                                                     $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: SCF.h                                                      $
 *
 ************************************************************************/

#if !defined(_INC_SCF)
#define _INC_SCF

#ifdef __cplusplus
extern "C"{
#endif

/*
/////////////////////////////////////////////////////////////////////////////
//  Define Symbolic Constants
/////////////////////////////////////////////////////////////////////////////
*/
#ifdef SYSTEMCONFIG_EXPORTS
#define SYSTEMCONFIG_API __declspec(dllexport)
#else
#define SYSTEMCONFIG_API __declspec(dllimport)
#endif

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Registry Key
//
//  changes in use of Windows Registry for Windows 7 to properly work
//  with the new security policies for Windows 7 Registry and the 
//  use of HKEY_LOCAL_MACHINE and HKEY_CURRENT_USER.
//
//  WARNING FOR WINDOWS 7    **
//     DeviceConfig must be started using Run as Administrator in order to access
//     the Windows Registry keys in the tree of HKEY_LOCAL_MACHINE.
//     If DeviceConfig is run as normal user, any changes will be put into the VirtualStore
//     section of HKEY_CURRENT_USER and changes will not be made to HKEY_LOCAL_MACHINE.
//
//     We have run into a problem with provisioning changes that did not seem to take only
//     to find that we had a VirtualStore section in HKEY_CURRENT_USER which was not modified
//     by DeviceConfig since it was being Run as Administrator resulting in GenPOS using the
//     old provisioning in VirtualStore of HKEY_CURRENT_USER instead.
//  
/////////////////////////////////////////////////////////////////////////////
*/
#define SYSTEM_CONFIG_KEY               _T("SOFTWARE\\NCR\\Platform Software\\SystemConfig")
#define ACTIVE_DEVICE_KEY               _T("SOFTWARE\\NCR\\Platform Software\\SystemConfig\\ActiveDevice")
#define SYSTEM_PARAMETER_KEY            _T("SOFTWARE\\NCR\\Platform Software\\SystemConfig\\SystemParameter")
#define RSM_PARAMETER_KEY               _T("SOFTWARE\\NCR\\Retail Platform Software for Windows")
#define OPOS_PARAMETER_KEY              _T("SOFTWARE\\OLEforRetail\\ServiceOPOS")
#define OPOS_DEVICE_PARAMETER_KEY		_T("SOFTWARE\\OLEforRetail\\ServiceOPOS\\CoinDispenser")

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Device Name
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_TYPENAME_PRINTER            _T("PRINTER")
#define SCF_TYPENAME_SCANNER            _T("SCANNER")
#define SCF_TYPENAME_SCANNER1           _T("SCANNER1")
#define SCF_TYPENAME_SCALE              _T("SCALE")
#define SCF_TYPENAME_MSR                _T("MSR")
#define SCF_TYPENAME_MSR1               _T("MSR1")
#define SCF_TYPENAME_MSR2               _T("MSR2")
#define SCF_TYPENAME_KEYLOCK            _T("KEYLOCK")
#define SCF_TYPENAME_SERVERLOCK         _T("SERVERLOCK")
#define SCF_TYPENAME_ODISPLAY           _T("ODISPLAY")
#define SCF_TYPENAME_CDISPLAY           _T("CDISPLAY")
#define SCF_TYPENAME_COIN               _T("COIN")
#define SCF_TYPENAME_DRAWER1            _T("DRAWER1")
#define SCF_TYPENAME_DRAWER2			_T("DRAWER2")
#define SCF_TYPENAME_CHARGE_POSTING     _T("CHARGE_POSTING")
#define SCF_TYPENAME_CHARGE_POSTINGLAN  _T("CHARGE_POSTING_LAN")
#define SCF_TYPENAME_EPT                _T("EPT")
#define SCF_TYPENAME_EPT_LAN            _T("EPT_LAN")
#define SCF_TYPENAME_EPT_EX             _T("EPT_EX")
#define SCF_TYPENAME_EPT_EX_LAN         _T("EPT_EX_LAN")
#define SCF_TYPENAME_EPT_EX_DLL         _T("EPT_EX_DLL")
#define SCF_TYPENAME_EPT_EX_DLL1        _T("EPT_EX_DLL1")
#define SCF_TYPENAME_EPT_EX_DLL2        _T("EPT_EX_DLL2")
#define SCF_TYPENAME_EPT_EX_DLL3        _T("EPT_EX_DLL3")
#define SCF_TYPENAME_BEVERAGE           _T("BEVERAGE")
#define SCF_TYPENAME_KITCHEN_CRT        _T("KITCHEN_CRT")
#define SCF_TYPENAME_KITCHEN_PRINTER    _T("KITCHEN_PRINTER")
#define SCF_TYPENAME_PC                 _T("PC")
#define SCF_TYPENAME_KEYBOARD           _T("KEYBOARD")
#define SCF_TYPENAME_FLASHSCREEN        _T("FLASHSCREEN")
#define SCF_TYPENAME_HTTPSERVER         _T("HTTPSERVER")
#define SCF_TYPENAME_CONNENGINEMFC      _T("CONNENGINEMFC")
#define SCF_TYPENAME_DISCONNECTED_SAT   _T("DISCONNECTED_SAT")
#define SCF_TYPENAME_OVERRIDENAME_SAT   _T("OVERRIDENAME_SAT")    // allow DeviceConfig to provide a Satellite name to override PC name setting.
#define SCF_TYPENAME_SIGCAP1            _T("SIGCAP1")
#define SCF_TYPENAME_PINPAD1            _T("PINPAD1")

//  Definition of OPOS Service Control Names
//		These are OPOS standard keys for different types of devices
//		Under each of these keys are a series of keys for the specific devices of each type.
//  The text strings must be the sane as the Windows Registry keys that are created by the
//  NCR Retail System application which are in the registry path of
//     HKEY_LOCAL_MACHINE->SOFTWARE->NCR->OLEForRetail->ServiceOPOS
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//

#define SCF_OPOS_NAME_DRAWER			_T("CashDrawer")
#define SCF_OPOS_NAME_SCANNER           _T("Scanner")
#define SCF_OPOS_NAME_SCALE             _T("Scale")
#define SCF_OPOS_NAME_MSR               _T("MSR")
#define SCF_OPOS_NAME_CDISPLAY          _T("LineDisplay")
#define SCF_OPOS_NAME_COIN				_T("CoinDispenser")
#define SCF_OPOS_NAME_PRINTER			_T("POSPrinter")
#define SCF_OPOS_NAME_KEYLOCK			_T("KeyLock")
#define SCF_OPOS_NAME_WIN_PRINTER		_T("Windows Driver Printer")
#define SCF_OPOS_NAME_SIGCAP			_T("SignatureCapture")
#define SCF_OPOS_NAME_PINPAD			_T("PINPad")

#define SCF_COM_NAME_PRINTER            _T("NCR7194.dll")          // DIRECT NCR 7194 Printer
#define SCF_ZEBRA_NAME_PRINTER          _T("Zebra Printer")        // Zebra RW 220 Printer
#define SCF_COM_NAME_MSR                _T("NCRDirectMSR.dll")      // DIRECT Mag Strip Reader.

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Device Name
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_CAPS_SUMMARY            1
#define SCF_CAPS_HWPORT             2
#define SCF_CAPS_DEVICE             3

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Device Type
//
// See method CDeviceEngine::Open() in file DeviceEngine.cpp which contains
// the source code for initialing for these various devices. Some devices are
// OPOS devices and some are not.
//
// OPOS was added with Rel 2.0.0 with the move away from the NCR 7448, which
// was being manufacture discontinued, to Windows XP based NCR touchscreen terminals.
// The NCR 7448 did not use OPOS with printers and other peripherals but instead
// used a set of device drivers that were part of the Platform Independent Functions (PIF)
// library. However these device drivers were designed for Serial COM port
// connected devices only and these days we have Bluetooth and Ethernet
// connected devices in addition to RS-232 Serial COM port devices.
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_TYPE_PRINTER            0x00000001			// See also DEVICE_THERMAL_PRINTER, DEVICE_KITCHEN_PRINTER, DEVICE_SLIP_PRINTER
#define SCF_TYPE_SCANNER            0x00000002			// See also DEVICE_SCANNER and DEVICE_SCANNER_SCALE
#define SCF_TYPE_SCALE              0x00000004			// See also DEVICE_SCALE
#define SCF_TYPE_MSR                0x00000008
#define SCF_TYPE_KEYLOCK            0x00000010
#define SCF_TYPE_SERVERLOCK         0x00000020
#define SCF_TYPE_ODISPLAY           0x00000040			// See also DEVICE_ODISPLAY
#define SCF_TYPE_CDISPLAY           0x00000080			// See also DEVICE_CDISPLAY
#define SCF_TYPE_COIN               0x00000100			// See also DEVICE_COIN_DISPENSER
#define SCF_TYPE_DRAWER1            0x00000200
#define SCF_TYPE_CHARGE_POSTING     0x00000400
#define SCF_TYPE_EPT                0x00000800
#define SCF_TYPE_BEVERRAGE          0x00001000			// See also DEVICE_BEVERAGE
#define SCF_TYPE_KITCHEN_CRT        0x00002000
#define SCF_TYPE_KITCHEN_PRINTER    0x00004000
#define SCF_TYPE_PC                 0x00008000
#define SCF_TYPE_EPT_EX             0x00010000
#define SCF_TYPE_DRAWER2            0x00020000
#define SCF_TYPE_MSR1               0x00040000
#define SCF_TYPE_MSR2               0x00080000
#define SCF_TYPE_SIGCAP1            0x00100000
#define SCF_TYPE_PINPAD1            0x00200000

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Device Type
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_PORT_SERIAL             0x00000001
#define SCF_PORT_MSR                0x00000002
#define SCF_PORT_KEYLOCK            0x00000004
#define SCF_PORT_SERVER_LOCK        0x00000008
#define SCF_PORT_DRAWER             0x00000010

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Device Capability
//    These definitions are for device characteristics and are the
//    registery keys used to access the values for these attributes.
//    The first few are standard defines for accessing COMM port
//    attributes such as baud rate, etc.
//    The other attributes are used as part of Electronic Payment
//    interfaces such as DataCap and Mercury Payments to define
//    the characteristics for the PIN pad devices such as the
//    Verifone 2000.
//
//    For examples of usage see the DeviceConfig utility source code
//    and the source code for DataCap and Mercury Payments DLLs.
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_DATANAME_BAUD           _T("Baud")
#define SCF_DATANAME_CHARBIT        _T("CharBit")
#define SCF_DATANAME_PARITY         _T("Parity")
#define SCF_DATANAME_STOPBIT        _T("StopBit")
#define SCF_DATANAME_HANDSHAKE      _T("HandShake")

#define SCF_DATANAME_PORT			_T("Port")
#define SCF_DEFAULTDATA_PORT        _T("0000")
#define SCF_DATANAME_DLLNAME		_T("DLLName")
#define SCF_DATANAME_TYPENAME		_T("TypeName")
#define SCF_DATANAME_USES			_T("Uses")

// Standard Credit/Debit Server
#define SCF_DATANAME_IPADDR			_T("IPAddr")
#define SCF_DATANAME_IPADDR2		_T("IPAddr2")
//GiftCard Server
#define SCF_DATANAME_IPADDR3		_T("IPAddr3")
#define SCF_DATANAME_IPADDR4		_T("IPAddr4")
//Employee Payroll Deduction Server
#define SCF_DATANAME_IPADDR5		_T("IPAddr5")
#define SCF_DATANAME_IPADDR6		_T("IPAddr6")
//Bad Check Server
#define SCF_DATANAME_IPADDR7		_T("IPAddr7")
#define SCF_DATANAME_IPADDR8		_T("IPAddr8")

#define SCF_DATANAME_IPPORT			_T("IPPort")

// type of electronic payment interface
#define SCF_DATANAME_EPT_INTERFACE	_T("eptinterface")
#define SCF_DATANAME_EPT_DSICLIENT  _T("dsiclient")
#define SCF_DATANAME_EPT_DSIPDC     _T("dsipdc")

#define SCF_DATANAME_MERCHANTID		_T("Merchant ID")
#define SCF_DEFAULTDATA_MERCHANTID  _T("000000000000000000000000")
#define SCF_DATANAME_KEYMANAGETYPE	_T("KeyManagementType")
#define SCF_DATANAME_MASTERKEYINDEX	_T("MasterKeyIndex")
#define SCF_DATANAME_IDLEPROMPT		_T("IdlePrompt")
#define SCF_DATANAME_WORKINGKEY		_T("WorkingKey")
#define SCF_DEFAULTDATA_WORKINGKEY  _T("0000000000000000")
#define SCF_DATANAME_SHOULDROTATE	_T("ShouldRotateIfMasterOnly")
#define SCF_DATANAME_PINPAD			_T("PINPad")
#define SCF_DATANAME_TIMEOUT		_T("TimeOut")
#define SCF_DATANAME_OPTION			_T("Option")
#define SCF_DATANAME_PDC			_T("PDC")   //PDC for use with verifone 1000
#define SCF_DATANAME_CHECKREADTYPE	_T("CheckReadType")
#define SCF_DATANAME_CHECKREADTO	_T("CheckReadTimeout")
#define SCF_DATANAME_MAGSTRIPETYPE	_T("MagStripeType")
#define SCF_DATANAME_MAGSTRIPETO	_T("MagStripeTimemout")
#define SCF_DATANAME_UNICODE        _T("Unicode")
#define SCF_DATANAME_STOREFORWARD	_T("StoreAndForward")          // 0 - enabled but off, 1 - enabled and on, 2 - disabled
#define SCF_DATANAME_SECURENAME		_T("SecureName")               // dsiPDCX Secure Device name
#define SCF_DATANAME_DSIPADTYPE		_T("DsiPadType")               // dsiPDCX Pad Type name
#define SCF_DATANAME_SECURENAME_NONE	    _T("--NONE--")             // indicates that there is no dsiPDCX Secure Device specified
#define SCF_DATANAME_SECURENAME_EMV		    _T("SecureEmvName")        // dsiEMVX EMV Secure Device Processor Name
#define SCF_DATANAME_SECURENAME_EMV_NONE	_T("--NONE--")         // indicates that there is no dsiEMVX Secure Device specified
#define SCF_DATANAME_TERMINALID_EMV		    _T("TerminalIdEmv")    // dsiEMVX EMV TerminalID Name used for some processors and installations
#define SCF_DATANAME_TERMINALID_EMV_NONE	_T("")                 // indicates that there is no dsiEMVX TerminalID specified

																	// see PIF_STORE_AND_FORWARD_(ON/OFF/DISABLED)
#define SCF_DATANAME_STOREFORWARDFILE	_T("StoreAndForwardFile")	// file name if StoreAndForward enabled
#define SCF_DEFAULTDATA_STOREFORWARDFILE  _T("EEPTSTO")
#define SCF_DATANAME_PRINTPORT     _T("ActivePort")	  // DIRECT, OPOS, NOTE, DLL, ETHERNET - TLDJR

#define SCF_DATANAME_ISENABLED      _T("IsEnabled")


#define SCF_DATANAME_WEDGE_ISWEDGE _T("IsWedgeDevice")     // Boolean indicating Wedge device or not, 0 not wedge or 1 is wedge
#define SCF_DATANAME_WEDGE_PREFIX  _T("PrefixCharacter")   // Prefix for wedge character stream, indicates which wedge device this is
#define SCF_DATANAME_WEDGE_SUFFIX  _T("SuffixCharacter")   // Suffix for wedge character stream, terminates input stream

#define SCF_PINPADNAME_UNAVAILABLE	_T("MSR Unavailable")
#define SCF_PINPADNAME_1000			_T("Verifone 1000")
#define SCF_PINPADNAME_2000			_T("Verifone 2000")
#define SCF_PINPADNAME_EVEREST		_T("Verifone Everest")
#define SCF_PINPADNAME_TERM_MSR		_T("Terminal MSR")
#define SCF_PINPADNAME_VIVOTECH		_T("VivoTech MSR")
#define SCF_PINPADNAME_OPOS			_T("OPOS MSR/PinPad")
#define SCF_PINPADNAME_DATACAPSECURE	_T("DataCap Secure")

#define SCF_INTERFACE_OPOS          _T("OPOS")
#define SCF_INTERFACE_DIRECT        _T("DIRECT")
#define SCF_INTERFACE_NOTE          _T("NOTE")
#define SCF_INTERFACE_ETHERNET      _T("ETHERNET")
#define SCF_INTERFACE_DLL           _T("DLL")
#define SCF_INTERFACE_SPECIAL       _T("SPECIAL")
#define SCF_INTERFACE_WEDGE         _T("WEDGE")

#define SCF_PIRACY_MAIN             _T("TakeWhatYouCanGiveNothingBack")
#define SCF_PIRACY_REGISTRATION     _T("RegistrationNumber")
#define SCF_PIRACY_UNLOCKCODE       _T("UnlockCode")
#define SCF_PIRACY_USERNAME         _T("UserName")

//mag reader types for PDC
#define SCF_MAGREADER_NONE			_T("NONE")
#define SCF_MAGREADER_DATACAP		_T("Datacap")

//Check reader types for PDC
#define SCF_CHECKREADER_NONE		_T("NONE")
#define SCF_CHECKREADER_MAGTEK		_T("MagTek")

/* Capability of Serial I/F */
#define SCF_CAPS_BAUD_300           0x00000001
#define SCF_CAPS_BAUD_600           0x00000002
#define SCF_CAPS_BAUD_1200          0x00000004
#define SCF_CAPS_BAUD_2400          0x00000008
#define SCF_CAPS_BAUD_4800          0x00000010
#define SCF_CAPS_BAUD_9600          0x00000020
#define SCF_CAPS_BAUD_19200         0x00000040
#define SCF_CAPS_BAUD_38400         0x00000080
#define SCF_CAPS_BAUD_57600         0x00000100
#define SCF_CAPS_BAUD_115200        0x00000200

#define SCF_CAPS_CHAR_8             0x00000001
#define SCF_CAPS_CHAR_7             0x00000002

#define SCF_CAPS_PARITY_NONE        0x00000001
#define SCF_CAPS_PARITY_EVEN        0x00000002
#define SCF_CAPS_PARITY_ODD         0x00000004
#define SCF_CAPS_PARITY_MARK        0x00000008
#define SCF_CAPS_PARITY_SPACE       0x00000010

#define SCF_CAPS_STOP_1             0x00000001
#define SCF_CAPS_STOP_2             0x00000002

#define SCF_CAPS_HANDSHAKE_NONE     0x00000001
#define SCF_CAPS_HANDSHAKE_RTSCTS   0x00000002
#define SCF_CAPS_HANDSHAKE_CTS      0x00000004
#define SCF_CAPS_HANDSHAKE_RTS      0x00000008
#define SCF_CAPS_HANDSHAKE_XONOFF   0x00000010
#define SCF_CAPS_HANDHSAKE_DTRDSR   0x00000020 //added for Device config options JHHJ 9-13-05


/* Capability of MSR I/F */
#define SCF_DATANAME_MSR            _T("MsrType")

#define SCF_CAPS_MSR_ISO            0x00000001
#define SCF_CAPS_MSR_JIS            0x00000002

/* Capability of Drawer I/F */
#define SCF_DATANAME_DRAWER         _T("Drawer")

#define SCF_CAPS_DRAWER_A           0x00000001
#define SCF_CAPS_DRAWER_B           0x00000002

/* Capability of Ethernet based CPM or EPT manager hosting
 * The following text strings must agree with what is in DeviceConfig.txt
 * and in the Windows Registry for the key names.
**/
#define SCF_DEVICENAME_LAN_CPM      				_T("Ethernet CPM")
#define SCF_DEVICENAME_LAN_EPT      				_T("Ethernet EPT")
#define SCF_DEVICENAME_LAN_EPTEX    				_T("Ethernet EPT EX")
#define SCF_DEVICENAME_EPTEXDLL_DATACAP   			_T("DataCap EEPT")
#define SCF_DEVICENAME_EPTEXDLL_MERCURY   			_T("Mercury Payments EPT")
#define SCF_DEVICENAME_EPTEXDLL_SECURE   			_T("DataCap Secure")
#define SCF_DEVICENAME_EPTEXDLL_SECURETOKENIZED   	_T("DataCap Secure Tokenized")
#define SCF_DEVICENAME_EPTEXDLL_FREEDOMPAY 			_T("FreedomPay EPT")

#define SCF_DEVICENAME_OVERRIDE_HOSTNAME			_T("HostName")    // used with SCF_TYPENAME_OVERRIDENAME_SAT to override Satellite PC name

// Constants used with SCF_TYPENAME_DISCONNECTED_SAT
/*
In order to keep up with the Join/Unjoin status I am planning to use the registry
as a persistant store and add a new string value, "JoinStatus" if the terminal is
a Disconnected Satellite which will have the values of:
 - "1"  -> the terminal has been Joined to an existing cluster
 - "0"  -> the terminal is disconnected and acting as a standalone
 - " "  -> the terminal is not a Disconnected Satellite

String value, "LastJoinMaster" will contain the name of the Master Terminal to
which the Disconnected Satellite was last joined or is currently joined.  It will
be set to the name of the Master Terminal at the time a Join command is performed.

String value, "LastJoinBackupMaster" which will have a
non-blank value if the value of "JoinStatus" is "Joined" and the cluster was
provisioned with a Backup Master Terminal.  The string will be the name of the
Backup Master Terminal to which the Disconnected Satellite is joined.  It will be
set to the name of the Backup Master Terminal at the time a Join command is
performed and will be set to a single space when an Unjoin is performed or if
the cluster to which a Disconnected Satellite is joined is not provisioned with
a Backup Master Terminal.

String value, "LastJoinBackupMaster" will contain the name of the Backup Master Terminal to
which the Disconnected Satellite was last joined or is currently joined.  It will
be set to the name of the Backup Master Terminal at the time a Join command is performed
if the cluster to which a Disconnected Satellite is joined is provisioned with
a Backup Master Terminal.

"LastJoinDate" will have as its value a date-time stamp indicating the last time a Join
command was done on the terminal.

"LastUnjoinDate" will have as its value a date-time stamp indicating the last time an
Unjoin command was done on the terminal.
**/
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
#define SCF_DEVICENAME_JOIN_ISDISCONNECTED      _T("IsDisconnected")
#define SCF_DEVICENAME_JOIN_JOINSTATUS          _T("JoinStatus")
#define SCF_DEVICENAME_JOIN_LASTJOINMASTER      _T("LastJoinMaster")
#define SCF_DEVICENAME_JOIN_LASTJOINDATE        _T("LastJoinDate")
#define SCF_DEVICENAME_JOIN_LASTUNJOINDATE      _T("LastUnjoinDate")
#define SCF_DEVICENAME_JOIN_LASTTERMINALNO      _T("LastTerminalNumber")  // used for debugging only!!!!

// Device types describing whether device is a LAN device, OPOS, or what ever
#define SCF_DEVICETYPE_OPOS			_T("OPOS")		// Device is an OPOS device specifying a profile in OLERetail registery sub-key
#define SCF_DEVICETYPE_LAN			_T("LAN")		// Device is a LAN device with parameters in SystemParameter
#define SCF_DEVICETYPE_COMM			_T("COMM")		// Device is a COMM port device and uses DLL with parameters in SystemParameter
#define SCF_DEVICETYPE_NONE			_T("NONE")		// Device uses no COMM port, LAN, OPOS, and has no parameters
#define SCF_DLLNAME_LAN             _T("Dummy DLL")

#define SCF_DLLNAME_JOINSTATUS_JOINED     _T("1")
#define SCF_DLLNAME_JOINSTATUS_UNJOINED   _T("0")
#define SCF_DLLNAME_DISCONNECTED_YES      _T("1")
#define SCF_DLLNAME_DISCONNECTED_NO       _T("0")


typedef struct _CONVERT_DATA                // Convert Bit Data with String Data
{
    TCHAR* lpszDataName;                    // Data Name
    TCHAR* lpszString;                      // Stirng Data
    ULONG   dwBit;                          // Bit Data
} CONVERT_DATA, *PCONVERT_DATA;

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition DATA Type
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_DATA_BINARY             1
#define SCF_DATA_STRING             2

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Return Value
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_SUCCESS                 0           /* Successful operation. */
#define SCF_NO_DEVICE               -1          /* not found */
#define SCF_NO_DATA                 -3          // Device found but no data.  Key found in the registry but no values
#define SCF_ERROR                   -2          /* other error. */

/*
/////////////////////////////////////////////////////////////////////////////
//  Support Configration Setup
/////////////////////////////////////////////////////////////////////////////
*/
#define SCF_USER_BUFFER 512                 /* size of user buffer */
#define SCF_MAX_NAME    32                  /* 31byte + \0 */
#define SCF_MAX_DATA    512                 /* 31byte + \0 */

typedef struct _DEVICE_CAPS {
    ULONG   dwSize;                         /* size of structure */
    ULONG   dwDeviceType;                   /* Type of supporting device */
    ULONG   dwPortType;                     /* Type of supporting port */
} DEVICE_CAPS, *PDEVICE_CAPS;

typedef struct {
    ULONG   dwSize;                         /* size of structure */
    ULONG   dwBaud;                         /* Set bit of supporting baud rate */
    ULONG   dwByteSize;                     /* Set bit of supporting byte size */
    ULONG   dwParity;                       /* Set bit of supporting prity type */
    ULONG   dwStopBit;                      /* Set bit of supporting #of stop bit */
    ULONG   dwHandShake;                    /* Set bit of supporting hand shake type */
} DEVICE_CAPS_SERIAL, *PDEVICE_CAPS_SERIAL;

typedef struct {
    ULONG   dwSize;                         /* size of structure */
    ULONG   dwType;                         /* Set bit of supporting MSR type */
} DEVICE_CAPS_MSR, *PDEVICE_CAPS_MSR;

typedef struct _DEVICE_CAPS_DRAWER {
    ULONG   dwSize;                         /* size of structure */
    ULONG   dwNumber;                       /* Set bit of supporting drawer no. */
} DEVICE_CAPS_DRAWER, *PDEVICE_CAPS_DRAWER;

// The _DEVICE_CAPS_OPS struct is used to obtain one entry for a particular SCF_OPOS_NAME type.
// This struct contains the data for a particular device.  For each of the SCF_OPOS_NAME types
// there may be multiple devices hence multiple of these structs.
typedef struct _DEVICE_CAPS_OPOS {
	TCHAR  aszNameOPOS [32];		// same as one of the SCF_OPOS_NAME types above
	struct {
		TCHAR  aszDevAttrib [64];	// contains list the device attributes
		TCHAR  aszDevData [64];		// contains the data string for the attributes
	} azDevices [56];
} DEVICE_CAPS_OPOS, *PDEVICE_CAPS_OPOS;

/*
/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////
*/
SYSTEMCONFIG_API
ULONG ScfSetActiveDevice(
    const TCHAR* lpszDeviceType,
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDllFileName
    );

SYSTEMCONFIG_API
ULONG ScfResetActiveDevice(
    const TCHAR* lpszDeviceType,
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDllFileName
    );

SYSTEMCONFIG_API
ULONG ScfResetActiveDeviceAll(
    const TCHAR* lpszDeviceType);

SYSTEMCONFIG_API
ULONG ScfGetActiveDevice(
    const TCHAR* lpszDeviceType,
    ULONG* lpDeviceCount,
    TCHAR* lpszDeviceName,
    TCHAR* lpszDllFileName
    );

SYSTEMCONFIG_API
ULONG ScfGetOPOSDevice(
    const TCHAR* lpszDeviceType,
    ULONG* lpDeviceCount,
    PDEVICE_CAPS_OPOS lpDeviceOpos
    );

SYSTEMCONFIG_API
ULONG ScfGetDLLDevice(
    const TCHAR* lpszDeviceType,
    ULONG* lpDeviceCount,
    PDEVICE_CAPS_OPOS lpDeviceOpos
    );

SYSTEMCONFIG_API
DWORD ScfGetRsmValue(
    const TCHAR*  lpszKeyword, 
    ULONG*  lpDeviceCount,
    TCHAR*  lpszValue
	);

SYSTEMCONFIG_API
ULONG ScfGetStandardDevice(
    const TCHAR* lpszDeviceType,
    ULONG* lpDeviceCount,
    TCHAR   **lpDeviceOpos
    );

SYSTEMCONFIG_API
ULONG ScfGetStandardDeviceByInterface(
    const TCHAR* lpszDeviceType,
    const TCHAR* lpszDeviceInterface, 
    ULONG* lpDeviceCount,
    TCHAR   **lpDeviceOpos
    );

SYSTEMCONFIG_API
ULONG ScfGetParameter(
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDataName,
    ULONG* lpDataType,
    VOID* lpData,
    ULONG dwNumberOfBytesToRead,
    ULONG* lpNumberOfBytesRead
    );

SYSTEMCONFIG_API
ULONG ScfGetOPOSType(
	const TCHAR* lpszDeviceType,
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDataName,
    ULONG* lpDataType,
    VOID* lpData,
    ULONG dwNumberOfBytesToRead,
    ULONG* lpNumberOfBytesRead
    );

SYSTEMCONFIG_API
ULONG ScfGetOPOSParameters(
    const TCHAR* lpszDeviceType,
	 const TCHAR* lpszDevice,
    ULONG* lpDeviceCount,
    TCHAR* lpszDeviceName,
    TCHAR* lpszDllFileName,
	ULONG* lpType
    );

SYSTEMCONFIG_API
ULONG ScfSetParameter(
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDataName,
    ULONG dwDataType,
    VOID* lpData,
    ULONG dwNumberOfBytesToWrite,
    ULONG* lpNumberOfBytesWritten
    );

SYSTEMCONFIG_API
ULONG ScfSetOPOSParameter(
	const TCHAR* lpszDeviceType,
    const TCHAR* lpszDeviceName,
    const TCHAR* lpszDataName,
    ULONG dwDataType,
    VOID* lpData,
    ULONG dwNumberOfBytesToWrite,
    ULONG* lpNumberOfBytesWritten
    );

SYSTEMCONFIG_API
BOOL ScfConvertBit2String(
    const TCHAR* lpszDataName,
    TCHAR* lpszDest,
    ULONG dwSrc
    );

SYSTEMCONFIG_API
BOOL ScfConvertString2Bit(
    const TCHAR* lpszDataName,
    ULONG* lpdwDest,
    const TCHAR* lpszSrc
    );

SYSTEMCONFIG_API
ULONG ScfExportRegistryKeyData (const TCHAR *lpszSubKeyPath, const TCHAR *lpszOutputFileName);

SYSTEMCONFIG_API
ULONG ScfImportRegistryKeyData (const TCHAR *lpszOutputFileName);

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif // !defined(_INC_DEVICEIO)
