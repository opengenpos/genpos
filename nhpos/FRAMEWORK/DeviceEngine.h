/*************************************************************************
 *
 * DeviceEngine.h
 *
 * $Workfile:: DeviceEngine.h                                            $
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
 * $History:: DeviceEngine.h                                             $
 *
 ************************************************************************/

#if !defined(AFX_DEVICEENGINE_H__A385C815_9469_11D3_88D3_005004B8DA0C__INCLUDED_)
#define AFX_DEVICEENGINE_H__A385C815_9469_11D3_88D3_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ecr.h"
#include "pif.h"
#include "DeviceIOPrinter.h"
#include "FrameworkIo.h"
#include "DeviceIOKeylock.h"
#include "DeviceIOCashDrawer.h"
#include "DeviceIOMSR.h"
#include "DeviceIOScanner.h"
#include "DeviceIOScale.h"
#include "DeviceIOServerLock.h"
#include "Device.h"
#include "DeviceSS.h"
#include "DeviceScanner.h"
#include "DeviceScale.h"
#include "bl.h"
#include <atlbase.h>
#include "OPOSControl.h"
#include "zebraprint.h"

#include "ServerProc.h"


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

#define LOGO_STAT_PERCENTAGE 0x01

#define NO_SPECIFIED_LOGO  0
#define STANDARD_LOGO      1
#define SPECIAL_LOGO       2

class CDeviceMultiFunction {
public:
    CDeviceMultiFunction();
    virtual ~CDeviceMultiFunction();

public:
	COleDispatchDriver  *m_pControl;
	BOOL                m_bControlDispatch;    // // m_pControl->CreateDispatch() has been called (TRUE) nor not (FALSE)
};

class CDeviceWinPrinter : public CDC
{
	public:
		typedef enum {NCR_7197 = 1, NCR_7167} TargetPrinterType;
		typedef enum {PrintClosed = 1, PrintOpen, PrintUnavailable} PrinterObjectStatus;
		typedef enum {PrintCode_PaperCut = 1, PrintCode_PaperFeed, PrintCode_SingleHighWide,
						PrintCode_DoubleHighWide, PrintCode_DoubleHigh, PrintCode_DoubleWide} PrinterCodes;


		CDeviceWinPrinter(TargetPrinterType  tpType = NCR_7197);
		virtual ~CDeviceWinPrinter();

		int StartPage();
		int EndPage();
		int PrintLine (LPTSTR ptchBuffer, ULONG  ulLength);
		CFont                m_fontText;
		CFont                *m_oldFont;
		PrinterObjectStatus  m_tpObjectStatus;

	protected:
		TargetPrinterType    tpPrinterType;
		PrinterCodes         tpPrinterCode;
		int                  iXColumn;
		int                  iYRow;
		bool                 bFilterEscSeq;      // indicates if we are to filter escape sequences used with thermal printers or not.

		static USHORT        usReceiptPrintCounter;

//		CFont                m_cfont;
//		LOGFONT              m_logfont;

	private:

};

class CDeviceMultipleDeviceMap
{
public:
	// following enum is used to index into the m_usDeviceMapArray[] array which contains a bit map for the
	// different types of devices that are managed by a CDeviceMultipleDeviceMap object.
	typedef enum {NO_DEVICE=0, PRINTER_DEVICE=1, MSR_DEVICE=2} MultipleDeviceMapBits;

	CDeviceMultipleDeviceMap (MultipleDeviceMapBits bit = NO_DEVICE);
	~CDeviceMultipleDeviceMap();

	void SetDevice (MultipleDeviceMapBits bit) { if (bit == NO_DEVICE) m_usDeviceMap = 0; else m_usDeviceMap |= m_usDeviceMapArray[bit]; }
	void UnsetDevice (MultipleDeviceMapBits  bit) { m_usDeviceMap &= ~m_usDeviceMapArray[bit]; }
	BOOL TestDevice (MultipleDeviceMapBits bit) {return (m_usDeviceMap & m_usDeviceMapArray[bit]);}


protected:
	unsigned short   m_usDeviceMap;
	static  unsigned short  m_usDeviceMapArray[3];
};

//----------------------------------------------------------------------------------------

class CDeviceEngine  
{
public:
    CDeviceEngine();
    virtual ~CDeviceEngine();

// public functions
public:
    BOOL        Open();
    BOOL        Close();
    BOOL        BlInterface(DWORD dwCommand, VOID* pvData, DWORD dwLength);
    BOOL        PreTranslateMessage(MSG* pMsg);
    BOOL        AddData(DWORD dwType, VOID* pvData, DWORD nLength);
    BOOL        RemoveData(DWORD* pdwType, VOID* pvData, DWORD* pnLength);
    VOID        RemoveAllData();
	SHORT       HandleIOCTL (WPARAM wParam, LPARAM lParam);

	SHORT       SetdwErrorCode (VOID* pvData);

	int LoadLicenseKeyFile (void);
	int BlFwIfLicStatus (VOID);

#ifndef DEVENG_REPLACEMENT
	CWinApp    *m_myAppForMessages;
#else
	CWinThread *m_myAppForMessages;
#endif
// Internal functions
protected:
    BOOL        KeyboardDataMessage(MSG* pMsg);
    USHORT      ConvertKey(MSG *pMsg, VIRTUALKEY_BUFFER  *pVirtualKeyData);
    USHORT      ConvertAlgorismForKeyInput(MSG *pMsg, VIRTUALKEY_BUFFER  *pVirtualKeyData);
    VOID        MakeKeylockModeEvent();
    BOOL        DeviceDataMessage(MSG* pMsg);
    BOOL        ConvertKeylockData(CHARDATA* pBl, PDEVICEIO_KEYLOCK_POSITION pDevice, DWORD dwLength);
    BOOL        ConvertDrawerData(USHORT* pBl, PDEVICEIO_DRAWER_STATUS pDevice, DWORD dwLength);
    BOOL        ConvertMSRData(MSR_BUFFER* pBl, PDEVICEIO_MSR_DATA pDevice, DWORD dwLength);
    BOOL        ConvertScannerData(UCHAR* pBl, UCHAR* pDevice, DWORD dwLength);
    BOOL        ConvertServerLockData(SHORT* pBl, PDEVICEIO_SERVERLOCK_NUMBER pDevice, DWORD dwLength);
	BOOL		PrinterLoadLogoIndexInformation();
	LONG		PrinterChangePrintSide(SHORT sType);
	LONG		PrinterPrintLogo(ULONG ulTransNo, BOOL blFreq);
	SHORT		myRandNumber (USHORT low, USHORT high);
	VOID		PrinterCheckStatus (PFRAMEWORK_IO_PRINTER pData, LONG resultCode);
	LONG	    LoadImageInformation(USHORT printingLogoType);  // return bitmap height or 0 if error

	BOOL        DevicePrinterStatus (VOID* pvData, DWORD dwLength);
	BOOL        DevicePrinterWrite (VOID* pvData, DWORD dwLength);
	BOOL        DevicePrinterColumnStatus (VOID * pvData, DWORD dwLength);

	BOOL		ValidateKeyCode (char *sKeyHash, wchar_t *sKeyCode);

	// for System Configuration

	typedef struct _SCINFO
	{
		TCHAR*  pszName;
		DWORD   dwType;
	} SCINFO, *PSCINFO;

	int			Device_SCF_TYPE_CDISPLAY_Setup(PSCINFO pInfo, TCHAR * pchDLL);
	int			Device_SCF_TYPE_PRINTER_Setup(PSCINFO pInfo, TCHAR * pchDevice, TCHAR * pchDLL);

private:

protected:

	// OPOS was added with Rel 2.0.0 with the move away from the NCR 7448, which
	// was being manufacture discontinued, to Windows XP based NCR touchscreen terminals.
	// The NCR 7448, Windows CE OS, did not use OPOS with printers and other peripherals
	// but instead used a set of device drivers that were part of the
	// Platform Independent Functions (PIF) library. However these device drivers
	// were designed for Serial COM port connected devices only and these days
	// we have Bluetooth and Ethernet connected devices in addition to
	// RS-232 Serial COM port devices.

	BOOL                m_bOpened;          // 
	BOOL                m_bStartDoc;
	BOOL                m_bFirstKeylock;    //
    short               m_nKeyNumber;       //
    // CDeviceKeylock*     m_pDeviceKeylock;   // keylock device        Removed 4/12/2021 - replaced by OPOS, m_OposKeyLock
    // CDeviceDrawer*      m_pDeviceDrawer;    // cash drawer device    Removed 4/12/2021 - replaced by OPOS, m_cashDrawer
    CDeviceScanner*     m_pDeviceScanner1;  // scanner device
    CDeviceScanner*     m_pDeviceScanner2;  // scanner device
    CDeviceScale*       m_pDeviceScale;     // scale device
	CDevicePrinter*     m_pDevicePrinter;   // direct to COM thermal printer, not using OPOS. lPrintType == PMG_COM_PRINTER
	CRITICAL_SECTION    m_csData;           // synch. semaphore
    CPtrList            m_lstData;          // device data queue
	COPOSControl*		m_pOPOSControl;
	CServerProc *       m_pHttpServerProc;  // http server object

	IZebraPrintCtl*		m_pZebraControl;            // pointer to a portable Zebra printer such as RW 220. lPrintType == PMG_ZEBRA_PRINTER
	BOOL                m_bZebraControlDispatch;    // m_pZebraControl->CreateDispatch() has been called (TRUE) nor not (FALSE)
	CDeviceMultipleDeviceMap   m_mZebraControlDevices;           
	CString             csZebraBuffer;		// buffer/spool for the zebra printer(s) (RW 220/420)

	FLOGOFILE			m_flfLogoIndex;
	ULONG				m_ulPrintCount;

	// following data areas used by license key file processing.
	CString     m_tcsSerialNo;
	wchar_t     m_LicSerialNumberString[64];
	wchar_t     m_LicResellerIdString[64];
	wchar_t     m_LicKeyCodeString[128];
	char        m_ValidatedString[128];  // contains the unecrypted string from ValidateKeyCode()
	int   		m_licStatus;  // Integer representation of Status see also AXDATA

	int                 m_nConvertKeyStateMachine;
	int                 m_nConvertKeyStateMachineIndex;
	BOOL                m_bVirtualKeyProcessingEnabled;
	USHORT              m_usConvertKeyBuffer [512];

};

#endif // !defined(AFX_DEVICEENGINE_H__A385C815_9469_11D3_88D3_005004B8DA0C__INCLUDED_)
