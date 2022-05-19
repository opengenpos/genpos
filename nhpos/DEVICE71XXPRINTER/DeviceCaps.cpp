// DeviceCaps.cpp: implementation of the CDeviceCaps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceIO.h"
#include "PrinterCaps.h"
#include "DeviceCaps.h"
#include "PrintIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Construction
//
// Prototype:   CDeviceCaps::CDeviceCaps();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceCaps::CDeviceCaps()
{
#if defined (DEVICE_7158)
    // initialize

    m_bInitialized = FALSE;

    m_sName[0].LoadString(IDS_DEVICENAME1);

    m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
    m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
    m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
    m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_1200 |
                                  SCF_CAPS_BAUD_2400 |
                                  SCF_CAPS_BAUD_4800 |
                                  SCF_CAPS_BAUD_9600 |
                                  SCF_CAPS_BAUD_19200 |
								SCF_CAPS_BAUD_38400 |
								SCF_CAPS_BAUD_57600 |
								SCF_CAPS_BAUD_115200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8;
    m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
                                  SCF_CAPS_PARITY_EVEN |
                                  SCF_CAPS_PARITY_ODD;
    m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1 |
	                              SCF_CAPS_STOP_2;
    m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

    m_CapsPrinter[0].dwSize     = sizeof(DEVICE_CAPS_PRINTER);
    m_CapsPrinter[0].dwStation  = PRINTER_CAPS_RECIEPT |
                                  PRINTER_CAPS_SLIP;
    m_CapsPrinter[0].dwColumnR  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnJ  = 0;
    m_CapsPrinter[0].dwColumnS  = COLMUN_SLIP;
    m_CapsPrinter[0].dwColumnV  = 0;
    m_CapsPrinter[0].dwLineV    = 0;
    m_CapsPrinter[0].dwCut      = PRINTER_CAPS_FULL_CAT |
                                  PRINTER_CAPS_PARTIAL;
    m_CapsPrinter[0].dwStamp    = 0;
    m_CapsPrinter[0].dwStampSize = 0;
    m_CapsPrinter[0].dwFeedR  = FEED_COUNT_TO_PAPER_CUT;

#endif
#if defined (DEVICE_7194)
    // initialize

    m_bInitialized = FALSE;

    m_sName[0].LoadString(IDS_DEVICENAME1);

    m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
    m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
    m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
    m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_1200 |
                                  SCF_CAPS_BAUD_2400 |
                                  SCF_CAPS_BAUD_4800 |
                                  SCF_CAPS_BAUD_9600 |
								SCF_CAPS_BAUD_19200 |
								SCF_CAPS_BAUD_38400 |
								SCF_CAPS_BAUD_57600 |
								SCF_CAPS_BAUD_115200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8;
    m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
                                  SCF_CAPS_PARITY_EVEN |
                                  SCF_CAPS_PARITY_ODD;
    m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1 |
	                              SCF_CAPS_STOP_2;
    m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

    m_CapsPrinter[0].dwSize     = sizeof(DEVICE_CAPS_PRINTER);
    m_CapsPrinter[0].dwStation  = PRINTER_CAPS_RECIEPT;
    m_CapsPrinter[0].dwColumnR  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnJ  = 0;
    m_CapsPrinter[0].dwColumnS  = 0;
    m_CapsPrinter[0].dwColumnV  = 0;
    m_CapsPrinter[0].dwLineV    = 0;
    m_CapsPrinter[0].dwCut      = PRINTER_CAPS_FULL_CAT |
                                  PRINTER_CAPS_PARTIAL;
    m_CapsPrinter[0].dwStamp    = 0;
    m_CapsPrinter[0].dwStampSize = 0;
    m_CapsPrinter[0].dwFeedR  = FEED_COUNT_TO_PAPER_CUT;

#endif
#if defined (DEVICE_7161)
    // initialize

    m_bInitialized = FALSE;

    m_sName[0].LoadString(IDS_DEVICENAME1);

    m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
    m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
    m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
    m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_1200 |
                                  SCF_CAPS_BAUD_2400 |
                                  SCF_CAPS_BAUD_4800 |
                                  SCF_CAPS_BAUD_9600 |
								SCF_CAPS_BAUD_19200 |
								SCF_CAPS_BAUD_38400 |
								SCF_CAPS_BAUD_57600 |
								SCF_CAPS_BAUD_115200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8;
    m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
                                  SCF_CAPS_PARITY_EVEN |
                                  SCF_CAPS_PARITY_ODD;
    m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1 |
	                              SCF_CAPS_STOP_2;
    m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_XONOFF;

    m_CapsPrinter[0].dwSize     = sizeof(DEVICE_CAPS_PRINTER);
    m_CapsPrinter[0].dwStation  = PRINTER_CAPS_RECIEPT;
    m_CapsPrinter[0].dwColumnR  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnJ  = 0;
    m_CapsPrinter[0].dwColumnS  = 0;
    m_CapsPrinter[0].dwColumnV  = 0;
    m_CapsPrinter[0].dwLineV    = 0;
    m_CapsPrinter[0].dwCut      = PRINTER_CAPS_PARTIAL;
    m_CapsPrinter[0].dwStamp    = 0;
    m_CapsPrinter[0].dwStampSize = 0;
    m_CapsPrinter[0].dwFeedR  = FEED_COUNT_TO_PAPER_CUT;

#endif
#if defined (DEVICE_7140)
    // initialize

    m_bInitialized = FALSE;

    m_sName[0].LoadString(IDS_DEVICENAME1);

    m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
    m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
    m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
    m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_1200 |
                                  SCF_CAPS_BAUD_2400 |
                                  SCF_CAPS_BAUD_4800 |
                                  SCF_CAPS_BAUD_9600 |
								SCF_CAPS_BAUD_19200 |
								SCF_CAPS_BAUD_38400 |
								SCF_CAPS_BAUD_57600 |
								SCF_CAPS_BAUD_115200;
	m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8 |
                                  SCF_CAPS_CHAR_7;
    m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE |
                                  SCF_CAPS_PARITY_EVEN |
                                  SCF_CAPS_PARITY_ODD |
                                  SCF_CAPS_PARITY_MARK |
                                  SCF_CAPS_PARITY_SPACE;
    m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1;
    m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

    m_CapsPrinter[0].dwSize     = sizeof(DEVICE_CAPS_PRINTER);
    m_CapsPrinter[0].dwStation  = PRINTER_CAPS_RECIEPT |
                                  PRINTER_CAPS_JOURNAL |
                                  PRINTER_CAPS_VALIDATION;
    m_CapsPrinter[0].dwColumnR  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnJ  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnS  = 0;
    m_CapsPrinter[0].dwColumnV  = COLMUN_VALIDATION;
    m_CapsPrinter[0].dwLineV    = LINE_COUNT_VALIDATION;
    m_CapsPrinter[0].dwCut      = PRINTER_CAPS_FULL_CAT |
                                  PRINTER_CAPS_PARTIAL;
    m_CapsPrinter[0].dwStamp    = PRINTER_CAPS_ELEC_STAMP;
    m_CapsPrinter[0].dwStampSize = 0;
    m_CapsPrinter[0].dwFeedR  = FEED_COUNT_TO_PAPER_CUT;

#endif
#if defined (DEVICE_7196)
    // initialize

    m_bInitialized = FALSE;

    m_sName[0].LoadString(IDS_DEVICENAME1);

    m_Caps[0].dwSize       = sizeof(DEVICE_CAPS);
    m_Caps[0].dwDeviceType = SCF_TYPE_PRINTER;
    m_Caps[0].dwPortType   = SCF_PORT_SERIAL;

    m_CapsSerial[0].dwSize =      sizeof(DEVICE_CAPS_SERIAL);
    m_CapsSerial[0].dwBaud =      SCF_CAPS_BAUD_38400;
    m_CapsSerial[0].dwByteSize =  SCF_CAPS_CHAR_8;
    m_CapsSerial[0].dwParity =    SCF_CAPS_PARITY_NONE;
    m_CapsSerial[0].dwStopBit   = SCF_CAPS_STOP_1;
    m_CapsSerial[0].dwHandShake = SCF_CAPS_HANDSHAKE_NONE;

    m_CapsPrinter[0].dwSize     = sizeof(DEVICE_CAPS_PRINTER);
    m_CapsPrinter[0].dwStation  = PRINTER_CAPS_RECIEPT;
    m_CapsPrinter[0].dwColumnR  = COLMUN_RECEIPT_B;
    m_CapsPrinter[0].dwColumnJ  = 0;
    m_CapsPrinter[0].dwColumnS  = 0;
    m_CapsPrinter[0].dwColumnV  = 0;
    m_CapsPrinter[0].dwLineV    = 0;
    m_CapsPrinter[0].dwCut      = PRINTER_CAPS_PARTIAL;
    m_CapsPrinter[0].dwStamp    = 0;
    m_CapsPrinter[0].dwStampSize = 0;
    m_CapsPrinter[0].dwFeedR  = FEED_COUNT_TO_PAPER_CUT;

#endif

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Destruction
//
// Prototype:   CDeviceCaps::~CDeviceCaps();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceCaps::~CDeviceCaps()
{

}

//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CDeviceCaps::GetDeviceName(lpDeviceCount, lpszDeviceName);
//
// Inputs:      LPVOID      lpDeviceCount;  -
//              DWORD       lpszDeviceName;
//
// Outputs:     DWORD       dwResult;   -
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDeviceCaps::GetDeviceName(LPDWORD lpDeviceCount, LPTSTR lpszDeviceName)
{
    CString     sName;

    // check parameters

    if (lpDeviceCount == NULL || lpszDeviceName == NULL)
    {
        return DEVICEIO_E_ILLEGAL;          // exit ...
    }

    // make up data

    *lpDeviceCount = NUMBER_OF_DEVICE;

    sName.LoadString(IDS_DEVICENAME1);
    _tcscpy(lpszDeviceName + 0, sName);

    // exit ...

    return DEVICEIO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CDeviceCaps::GetDeviceCapability(lpszDeviceName,
//                              dwFunc, dwType, lpCapsData);
//
// Inputs:      LPCTSTR     lpszDeviceName; -
//              DWORD       dwFunc;         -
//              DWORD       dwType;         -
//              LPVOID      lpCapsData;     -
//
// Outputs:     DWORD       dwResult;       -
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDeviceCaps::GetDeviceCapability(LPCTSTR lpszDeviceName, 
                                         DWORD dwFunc, DWORD dwType, LPVOID lpCapsData)
{
    DWORD                   dwResult = DEVICEIO_E_ILLEGAL;
    int                     nIndex;
    PDEVICE_CAPS            pCaps;
    PDEVICE_CAPS_SERIAL     pSerial;
    PDEVICE_CAPS_PRINTER    pPrinter;

    // check parameters

    if (lpszDeviceName == NULL || lpCapsData == NULL)
    {
        return DEVICEIO_E_ILLEGAL;          // exit ...
    }

    // search device

    for (nIndex = 0; nIndex < NUMBER_OF_DEVICE; nIndex++)
    {
        if (m_sName[nIndex].CompareNoCase(lpszDeviceName) == 0)
        {
            // which function ?

            switch (dwFunc)
            {
            case SCF_CAPS_SUMMARY:
                pCaps = (PDEVICE_CAPS)lpCapsData;
                *pCaps   = m_Caps[nIndex];
                dwResult = DEVICEIO_SUCCESS;
                pCaps->dwSize = m_Caps[nIndex].dwSize;
                break;
            case SCF_CAPS_HWPORT:
                pSerial = (PDEVICE_CAPS_SERIAL)lpCapsData;
                *pSerial = m_CapsSerial[nIndex];
                dwResult = DEVICEIO_SUCCESS;
                pSerial->dwSize = m_CapsSerial[nIndex].dwSize;
                break;
            case SCF_CAPS_DEVICE:
                pPrinter = (PDEVICE_CAPS_PRINTER)lpCapsData;
                *pPrinter = m_CapsPrinter[nIndex];
                dwResult = DEVICEIO_SUCCESS;
                pPrinter->dwSize = m_CapsPrinter[nIndex].dwSize;
                break;
            default:
                break;
            }

            // exit loop ...

            break;
        }
    }

    // exit ...

    return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CDevice::GetParameter(lpszDeviceName, lpszDataName,
//                              lpDataType, lpData, dwNumberOfBytesToRead,
//                              lpNumberOfBytesRead);
//
// Inputs:      LPCTSTR     lpszDeviceName;         -
//              LPCTSTR     lpszDataName;           -
//              LPDWORD     lpDataType;             -
//              LPVOID      lpData;                 -
//              DWORD       dwNumberOfBytesToRead;  -
//              LPDWORD     lpNumberOfBytesRead;    -
//
// Outputs:     DWORD       dwResult;               -
//
/////////////////////////////////////////////////////////////////////////////


DWORD CDeviceCaps::GetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, 
                                LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead, 
                                LPDWORD lpNumberOfBytesRead)
{
    DWORD   dwResult;
    TCHAR   atchBuffer[SCF_MAX_DATA];
    BOOL    bConverted;

    bConverted = FALSE;

    dwResult = ScfGetParameter(
        lpszDeviceName, 
        lpszDataName, 
        lpDataType, 
        (LPVOID)atchBuffer, 
        sizeof(atchBuffer), 
        lpNumberOfBytesRead);
    
    if (dwResult != SCF_SUCCESS)
    {
        return(dwResult);
    }

    if (*lpDataType == SCF_DATA_STRING)
    {
        bConverted = ScfConvertString2Bit(lpszDataName, (LPDWORD)lpData, atchBuffer);
    }

    if (bConverted)
    {
        *lpDataType = SCF_DATA_BINARY;
        *lpNumberOfBytesRead = sizeof(DWORD);
    } 
    else 
    {
        memcpy(lpData, (LPVOID)atchBuffer, *lpNumberOfBytesRead);
    }

    return (dwResult);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CDevice::SetParameter(lpszDeviceName, lpszDataName,
//                              lpData, dwNumberOfBytesToWrite, lpNumberOfBytesWritten);
//
// Inputs:      LPCTSTR     lpszDeviceName;         -
//              LPCTSTR     lpszDataName;           -
//              LPVOID      lpData;                 -
//              DWORD       dwNumberOfBytesToWrite; -
//              LPDWORD     lpNumberOfBytesWritten; -
//
// Outputs:     DWORD       dwResult;               -
//
/////////////////////////////////////////////////////////////////////////////

DWORD CDeviceCaps::SetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, 
                                DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite, 
                                LPDWORD lpNumberOfBytesWritten)
{
    DWORD   dwResult;
    TCHAR   atchBuffer[SCF_MAX_DATA];
    BOOL    bConverted;

    dwResult   = DEVICEIO_E_ILLEGAL;
    bConverted = FALSE;

    if (dwDataType == SCF_DATA_BINARY)
    {
        bConverted = ScfConvertBit2String(lpszDataName, atchBuffer, *((LPDWORD)lpData));
    }

    if (bConverted)
    {
        dwResult = ScfSetParameter(
            lpszDeviceName, 
            lpszDataName, 
            SCF_DATA_STRING, 
            (LPVOID)atchBuffer, 
            (_tcslen(atchBuffer) + sizeof(UCHAR)) * sizeof(TCHAR), 
            lpNumberOfBytesWritten);
    } 
    else 
    {
        dwResult = ScfSetParameter(
            lpszDeviceName, 
            lpszDataName, 
            dwDataType, 
            lpData, 
            dwNumberOfBytesToWrite, 
            lpNumberOfBytesWritten);
    }

    *lpNumberOfBytesWritten = dwNumberOfBytesToWrite;
    return (dwResult);
}

