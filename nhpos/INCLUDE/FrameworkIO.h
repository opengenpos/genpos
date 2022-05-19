/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
************************************************************************
 *
 * Framework.h
 *
 * $Workfile:: Framework.h                                               $
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
 * $History:: Framework.h                                                $
 *
 ************************************************************************/

#if !defined(_INC_FRAMEWORK)
#define _INC_FRAMEWORK

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Error Code Value
/////////////////////////////////////////////////////////////////////////////
*/


/*
/////////////////////////////////////////////////////////////////////////////
//  Call Back Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

/* Common Device Control, device status change */

#define	FRAMEWORK_IOCTL_CONTROL				1000
#define FRAMEWORK_IOCTL_VERSION_INFO		1001 //JHALL 7-6-05

typedef struct _FRAMEWORK_IOCTL_STATE
{
	DWORD	dwDevice;						/* device number */
	BOOL	bState;							/* device status */
	TCHAR   Version[1024];
} FRAMEWORK_IOCTL_STATE, *PFRAMEWORK_IOCTL_STATE;

/* device number */

#define	FRAMEWORK_KEYLOCK			1
#define	FRAMEWORK_DRAWER			2
#define	FRAMEWORK_MSR				3
#define	FRAMEWORK_SCANNER			4
#define	FRAMEWORK_SCALE				5
#define	FRAMEWORK_SERVERLOCK		6
#define FRAMEWORK_PRINTER			7
#define FRAMEWORK_CDISPLAY			8
#define FRAMEWORK_COIN				9


/* Drawer Device Control, open a drawer */

#define	FRAMEWORK_IOCTL_DRAWER_OPEN			1100

typedef struct _FRAMEWORK_IO_DRAWER_OPEN
{
	DWORD	dwNumber;						/* drawer number, 1 or 2 */
} FRAMEWORK_IO_DRAWER_OPEN, *PFRAMEWORK_IO_DRAWER_OPEN;


/* Drawer Device Control, get drawer status */

#define	FRAMEWORK_IOCTL_DRAWER_STATUS		1101

typedef struct _FRAMEWORK_IO_DRAWER_STATUS
{
	DWORD	dwDrawerA;						/* drawer A status */
	DWORD	dwDrawerB;						/* drawer B status */
} FRAMEWORK_IO_DRAWER_STATUS, *PFRAMEWORK_IO_DRAWER_STATUS;

#define	FRAMEWORK_IO_DRAWER_OPENED			0x0001	/* drawer is opened       */
#define	FRAMEWORK_IO_DRAWER_BEING_OPENED	0x0002	/* drawer is being opened */
#define	FRAMEWORK_IO_DRAWER_CLOSED			0x0004	/* drawer is closed       */


#define FRAMEWORK_IOCTL_DRAWER_CLOSE		1102 // Command to close the drawers.


/* Scanner Device Control, not on file conrol */

#define	FRAMEWORK_IOCTL_SCANNER_NOF			1200

typedef struct _FRAMEWORK_IO_SCANNER_NOF
{
	BOOL	bState;							/* device status */
} FRAMEWORK_IO_SCANNER_NOF, *PFRAMEWORK_IO_SCANNER_NOF;


/* Scale Device Control, read weight */

#define	FRAMEWORK_IOCTL_SCALE_READWEIGHT	1300

typedef struct _FRAMEWORK_IO_SCALE_DATA
{
	SHORT	sStatus;						/* status                */
	UCHAR	uchUnit;						/* unit                  */
	UCHAR	uchDec;							/* precision             */
	ULONG	ulData;							/* data                  */
} FRAMEWORK_IO_SCALE_DATA, *PFRAMEWORK_IO_SCALE_DATA;

// Line Display
#define FRAMEWORK_IO_LINEDISPLAY_WRITE		1400
#define FRAMEWORK_IO_LINEDISPLAY_GETSO		1401



// This struct needs to be synchronized with struct DEVICEIO_DISPLAY_DATA
// in file DeviceIODisplay.h so anything added here probably needs to be added there.
typedef struct _FRAMEWORK_IO_DISPLAY_DATA
{
	LONG	nRow;							/* start of row        */
	LONG	nColumn;						/* start of column     */
	UCHAR	uchAttribute;					/* character attribute */
	LONG	nChars;							/* number of character */
	TCHAR*	lpuchBuffer;					/* buffer address      */
	TCHAR	tchDeviceDescription[256];		/* Description of the Device*/
} FRAMEWORK_IO_DISPLAY_DATA, *PFRAMEWORK_IO_DISPLAY_DATA;

// Printer
#define FRAMEWORK_IOCTL_PRINTER_WRITE			1500
#define FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS	1501
#define FRAMEWORK_IOCTL_PRINTER_WAIT			1502
#define FRAMEWORK_IOCTL_PRINTER_BEG_INS			1503
#define FRAMEWORK_IOCTL_PRINTER_END_INS			1504
#define FRAMEWORK_IOCTL_PRINTER_BEG_REM			1505
#define FRAMEWORK_IOCTL_PRINTER_END_REM			1506
#define FRAMEWORK_IOCTL_PRINTER_FONT			1507 //ESMITH EJ FONT  USHORT  PmgFont( USHORT usPrtType, USHORT usFont)
#define FRAMEWORK_IOCTL_PRINTER_STATUS			1508 //JHALL 6-10-04
#define FRAMEWORK_IOCTL_PRINTER_TYPE			1509 //JHALL 5-01-06
#define FRAMEWORK_IOCTL_PRINTER_TRANS_START		1510
#define FRAMEWORK_IOCTL_PRINTER_TRANS_END		1511
#define FRAMEWORK_IOCTL_PRINTER_GETSETCAP		1512 // LONG  PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue)
#define FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE	1513 // VOID  PmgPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags)
#define FRAMEWORK_IOCTL_PRINTER_INIT_LOADLOGO	1514 // VOID  PmgPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags)

// Windows Printer

#define FRAMEWORK_IOCTL_WINPRINT_WRITE			1550
#define FRAMEWORK_IOCTL_WINPRINT_START			1551
#define FRAMEWORK_IOCTL_WINPRINT_END			1552

// Following defines used with ulAlignment member of FRAMEWORK_IO_PRINTER to indicate
// text manipulation to be used with bar code printing.  Nibles are numbered from least significant as 1.
//   mask FRAMEWORK_IO_PRINTER_TEXT_MASK_LCR  gets the text alignment left, right, center
//   mask FRAMEWORK_IO_PRINTER_TEXT_MASK_NAB  gets the text appears above, below bar code or not shown
#define FRAMEWORK_IO_PRINTER_TEXT_MASK_LCR      0x000F    // ulAlignment nibble 1, mask off following values
#define FRAMEWORK_IO_PRINTER_TEXT_LEFT          0x0000    // ulAlignment nibble 1 , indicates PTR_BC_LEFT
#define FRAMEWORK_IO_PRINTER_TEXT_CENTER        0x0001    // ulAlignment nibble 1, indicates PTR_BC_CENTER
#define FRAMEWORK_IO_PRINTER_TEXT_RIGHT         0x0002    // ulAlignment nibble 1, indicates PTR_BC_RIGHT
#define FRAMEWORK_IO_PRINTER_TEXT_MASK_NAB      0x00F0    // ulAlignment nibble 2, mask off following values
#define FRAMEWORK_IO_PRINTER_TEXT_BELOW         0x0000    // ulAlignment nibble 2, indicates PTR_BC_TEXT_BELOW
#define FRAMEWORK_IO_PRINTER_TEXT_ABOVE         0x0010    // ulAlignment nibble 2, indicates PTR_BC_TEXT_ABOVE
#define FRAMEWORK_IO_PRINTER_TEXT_NONE          0x0020    // ulAlignment nibble 2, indicates PTR_BC_TEXT_NONE

typedef struct _FRAMEWORK_IO_PRINTER
{
	LONG	lPrintStation;
	ULONG	ulLength;
	TCHAR*	ptchBuffer;
	LONG	lColumns;
	BOOL	bSlipStationExist;
	LONG	lTimeout;
	USHORT	usFontType;		//ESMITH FONT
	USHORT	fbStatus;		//JHALL Status of Printer
	ULONG   ulSymbology;     // used with FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE, indicates bar code type
	ULONG   ulAlignment;    // used with FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE, indicates align left, center, right
	ULONG   m_ulPrinter1StatusMap;  // used with DevicePrinterStatus() to contain m_pOPOSControl->m_ulPrinter1StatusMap
	DWORD   dwErrorCode;            // error code, if any, reported by CDeviceEngine::DevicePrinterWrite()
	LONG    resultCode;
	LONG    extResultCode;
} FRAMEWORK_IO_PRINTER, *PFRAMEWORK_IO_PRINTER;

//MSR device on a multiple option peripheral such as Zebra RW 220 printer with MSR option
#define FRAMEWORK_IOCTL_MULTI_MSR_READ          1580   // Read from an MSR that is attached to another device such as Zebra RW 220
#define FRAMEWORK_IOCTL_MULTI_MSR_EXIST         1581   // Check if exists MSR that is attached to another device such as Zebra RW 220
#define FRAMEWORK_IOCTL_COM_MSR_READ            1582   // Read from an MSR that is attached via a COM port such as on the DT310 tablet
#define FRAMEWORK_IOCTL_SIGNATURECAPTURE_CAPTURE  1583   // Do a signature capture via OPOS for Ingenico i6550 or other device
#define FRAMEWORK_IOCTL_SIGNATURECAPTURE_READ     1584   // Read the last signature capture from Ingenico i6550 or other device
#define FRAMEWORK_IOCTL_SIGNATURECAPTURE_BITMAP   1585   // Generate a bitmap from a signature capture from Ingenico i6550 or other device
#define FRAMEWORK_IOCTL_SIGNATURECAPTURE_CUSTOMERDISPLAY    1586   // Set display to clear form via OPOS for Ingenico i6550 or other device
#define FRAMEWORK_IOCTL_LIC_DETAILS                         1587   // Perform license key check

typedef struct _FRAMEWORK_IO_MSR_DATA
{
	USHORT  usOptions;                      // options for the I/O request
    UCHAR   uchLength1;
    TCHAR   auchTrack1[PIF_LEN_TRACK1];
    UCHAR   uchLength2;
    TCHAR   auchTrack2[PIF_LEN_TRACK2];
    UCHAR   uchLengthJ;
    TCHAR   auchTrackJ[PIF_LEN_TRACKJ];
} FRAMEWORK_IO_MSR_DATA, *PFRAMEWORK_IO_MSR_DATA;

typedef struct _FRAMEWORK_IO_SIGCAP_DATA
{
	USHORT   usOptions;                      // options for the I/O request; FRAMEWORK_IO_SIGCAP_DATA struct
	LONG     m_SigCapMaxX;                   // max X for signature; FRAMEWORK_IO_SIGCAP_DATA struct
	LONG     m_SigCapMaxY;                   // max X for signature; FRAMEWORK_IO_SIGCAP_DATA struct
	TCHAR   *m_SigCapPointArray;             // pointer to buffer holding signature; FRAMEWORK_IO_SIGCAP_DATA struct
	USHORT   m_ArrayLength;                  // length of the array currently in m_SigCapPointArray; FRAMEWORK_IO_SIGCAP_DATA struct
	USHORT   m_ArrayMaxSize;                 // max buffer size containing signature; FRAMEWORK_IO_SIGCAP_DATA struct
    USHORT   usConsNo;                       // consecutive No to identify the transaction from the EJ
    USHORT   usGuestNo;                      // guest check No to identify the transaction from the Guest Check File
    ULONG    ulCashierID;                    // cashier ID to identify the transaction
    ULONG    ulStoreregNo;                   // store/reg to identify the transaction. See ItemCountCons() - encoded as store number * 1000L + register number
	USHORT   usMonth;                        // Month the signature was captured
	USHORT   usDay;                          // Day the signature was captured
	USHORT   usYear;                         // Year the signature was captured
	USHORT   usHour;                         // Hour the signature was captured
	USHORT   usMinute;                       // Minute the signature was captured
	USHORT   usSecond;                       // Second the signature was captured
	USHORT   usFscMajor;                     // the FSC associated with this signature, allows more than one signature per transaction
	USHORT   usFscMinor;                     // the extended FSC associated with this signature
	TCHAR    tchTranInvoiceNum[20];	         // Unique Transacton Invoice Number used in EPT messages to identify the transaction
	USHORT   usReturnType;                   // indicates the Preselect Transaction Return return type
	USHORT   usReasonCode;                   // indicates the Preselect Transaction Return reason code
	UCHAR    uchUniqueIdentifier[24];        // Unique identifer for returns as digits, binary coded decimal
	UCHAR    uchUniqueIdentifierReturn[24];  // Unique identifer of original transaction for returns as digits, binary coded decimal
} FRAMEWORK_IO_SIGCAP_DATA, *PFRAMEWORK_IO_SIGCAP_DATA;

typedef struct _FRAMEWORK_IO_SIGCAPBLOB {
	FRAMEWORK_IO_SIGCAP_DATA  Header;
	TCHAR  BlobData[1800];
}  FRAMEWORK_IO_SIGCAP_BLOB, *PFRAMEWORK_IO_SIGCAP_BLOB;

typedef struct _FRAMEWORK_IO_PINPAD_DATA
{
	USHORT   usOptions;                      // options for the I/O request; FRAMEWORK_IO_PINPAD_DATA struct
	LONG     m_Amount;                       // purchase amount; FRAMEWORK_IO_PINPAD_DATA struct
    UCHAR    uchLength1;
    TCHAR    auchTrack1[PIF_LEN_TRACK1];
    UCHAR    uchLength2;
    TCHAR    auchTrack2[PIF_LEN_TRACK2];
} FRAMEWORK_IO_PINPAD_DATA, *PFRAMEWORK_IO_PINPAD_DATA;

//PinPad device such as Everest
// Uses FRAMEWORK_IO_MSR_DATA as does MSR device read
#define FRAMEWORK_IOCTL_PINPAD_START_READ    1589   // Start Read from an OPOS PinPad that is attached to this terminal
#define FRAMEWORK_IOCTL_PINPAD_READ          1590   // Read from a PinPad that is attached to this terminal
#define FRAMEWORK_IOCTL_PINPAD_EXIST         1591   // Check if exists PinPad that is attached to this terminal

#define FRAMEWORK_IOCTL_COM_VIRTUAL_KEYBOARD 1592   // Check if exists PinPad that is attached to this terminal

#define FRAMEWORK_IO_VIRTUALKEY_READ_ENABLE     1  // Enable the processing of Virtual Key events for MSR and Electronic Payment Terminal
#define FRAMEWORK_IO_VIRTUALKEY_READ_DISABLE    2  // Disable the processing of Virtual Key events for MSR and Electronic Payment Terminal

typedef struct _FRAMEWORK_IO_VIRTUALKEY_DATA
{
	USHORT  usOptions;                      // options for the I/O request see defines above.
} FRAMEWORK_IO_VIRTUALKEY_DATA, *PFRAMEWORK_IO_VIRTUALKEY_DATA;


//Coin Dispenser
#define FRAMEWORK_IOCTL_COIN_DISPENSE			1600

typedef struct _FRAMEWORK_IO_COIN_DATA
{
	LONG	lDispenseAmount;				// amount to dispense
	SHORT	sReturnCode;					// Error Code
} FRAMEWORK_IO_COIN_DATA, *PFRAMEWORK_IO_COIN_DATA;


//ActiveX Control
#define FRAMEWORK_AXCTL_LIC_DETAILS				2000
#define FRAMEWORK_AXCTL_LIC_REG					2001
#define FRAMEWORK_AXCTL_LIC_SERIAL				2002
#define FRAMEWORK_AXCTL_LIC_EXPIRE				2003
#define FRAMEWORK_AXCTL_LIC_PUTSTATUS			2004

//VBOLock -- TLDJR
#define FRAMEWORK_VBO_LIC_DETAILS				2000
#define FRAMEWORK_VBO_LIC_REG					2001
#define FRAMEWORK_VBO_LIC_SERIAL				2002
#define FRAMEWORK_VBO_LIC_EXPIRE				2003
#define FRAMEWORK_VBO_LIC_PUTSTATUS				2004

typedef struct _AXDATA
{
	TCHAR*	ptchUnlock;	// 
	ULONG*	pulong;		// (no longer used)
	TCHAR*	ptchStatus;	// Status string "Registered, Unregistered, Demo....etc" (NOT USED)
	LONG	lDaysLeft;  // Number of days left on Evaluation
	int   	licStatus;  // Integer representation of Status
} AXDATA, *PAXDATA;


/*
/////////////////////////////////////////////////////////////////////////////
//  Compile Switch
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////
*/

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


#endif /* !defined(_INC_FRAMEWORK) */
