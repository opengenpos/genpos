/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : SERIAL PORT CONTROL FUNCTIONS
:   Category       : PLATFORM MODULE, NCR 2170 US Hospitality Application
:   Program Name   : pifsio.c
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
:  Abstract:
:  SHORT  PIFENTRY PifOpenCom(USHORT usPortId, CONST PROTOCOL *pProtocol);
:  SHORT  PIFENTRY PifReadCom(USHORT usPort, VOID *pBuffer, USHORT usBytes);
:  SHORT  PIFENTRY PifWriteCom(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes);
:  SHORT  PIFENTRY PifControlCom(USHORT usPort, USHORT usFunc, ...);
:  VOID   PIFENTRY PifCloseCom(USHORT usPort);
:
:  ---------------------------------------------------------------------
: Update Histories
:    Date  : Ver.Rev. :   Name      : Description
:
:** GenPOS **
: Nov-30-16: 02.02.02 : R.Chambers  : Correct COM port path for COM10 and greater
: Nov-30-16: 02.02.02 : R.Chambers  : PifAbort() turned into PifLog(), return PIF_ERROR_COM_ACCESS_DENIED
: Sep-06-17: 02.02.02 : R.Chambers  : PifOpenCom() needed LeaveCriticalSection(&g_SioCriticalSection) before return.
-------------------------------------------------------------------------*/

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"

#define SIO32_STS_FRAMING           0x0800
#define SIO32_STS_PARITY            0x0400
#define SIO32_STS_OVERRUN           0x0200
#define SIO32_STS_RLSD              0x0080
#define SIO32_STS_DSR               0x0020
#define SIO32_STS_CTS               0x0010

extern SHORT    sPifPrintHandle;    /* for PifAbort.  set in PifOpenCom() under some circumstances */

CRITICAL_SECTION    g_SioCriticalSection;

PIFHANDLETABLE  aPifSioTable[PIF_MAX_SIO] = {0};

static COMMTIMEOUTS aComTimerSave[PIF_MAX_SIO] = {0};

static SHORT PifSubComGetStatus(USHORT usPort, HANDLE hHandle);
static SHORT PifSubComSetTime(USHORT usPort, HANDLE hHandle, USHORT usTime);
static SHORT PifSubComSendTime(USHORT usPort, HANDLE hHandle, USHORT usTime);
static SHORT PifSubComLoopTest(USHORT usPort, HANDLE hHandle);
static SHORT PifSubGetErrorCode(HANDLE hHandle);
static USHORT PifSioCheckPowerDown(USHORT usPort, PIFHANDLETABLE *pPifHandleTable);
static SHORT PifSubComPurgeReceive(USHORT usPort, HANDLE hHandle);
static SHORT PifSubComPurgeSend(USHORT usPort, HANDLE hHandle);
static SHORT PifSubComResetCompulsory(USHORT usPort);


/* --- Serial I/O --- */
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifOpenCom(USHORT usPortId,          **
**                                  CONST PROTOCOL FAR *pProtocol) **
**              usPortId:    port number to open                    **
**              pProtocol:   protocol data                          **
**                                                                  **
**  return:     handle of serial i/o                                 **
**                                                                  **
**  Description:Opening serial i/o                                  **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifOpenCom(USHORT usPortId, CONST PROTOCOL *pProtocol)
{
    SHORT   sPort;
	TCHAR   wszPortName[16] = {0};
    HANDLE  hHandle;
    DWORD dwError;
    DCB dcb = {0};
    COMMTIMEOUTS comTimer = {0};
    DWORD   dwCommMasks;
    DWORD   dwBaudRate;                 // baud rate
    DWORD   nCharBits, nMultiple;
    BYTE    bByteSize;                  // number of bits/byte, 4-8
    BYTE    bParity;                    // 0-4 = no, odd, even, mark, space
    BYTE    bStopBits;                  // 0,1,2 = 1, 1.5, 2
    BOOL    fResult;
    USHORT  usFlag = 0;
#if 1
    TCHAR  wszDisplay[128];
#endif
	USHORT  i;

    /* get device option from port id */
    usFlag = usPortId;
    usPortId &= 0x00FF;

    EnterCriticalSection(&g_SioCriticalSection);

    /* search free ID */
    sPort = PifSubGetNewId(aPifSioTable, PIF_MAX_SIO);
    if (sPort == PIF_ERROR_HANDLE_OVERFLOW) {
		char    aszErrorBuffer[256];

		NHPOS_ASSERT_TEXT (0, "PifOpenCom (): Handle Overflow in aPifSioTable");
        LeaveCriticalSection(&g_SioCriticalSection);
#if 1
        wsprintf(wszDisplay, TEXT("Pif Handle Buffer Overflow, COM%d"), usPortId);
        OutputDebugString(wszDisplay);
        MessageBox(NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
		// add new line to end of string for the snapshot file output.
		sprintf (aszErrorBuffer, "    Comm Port requested %d", usPortId);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		strcat (aszErrorBuffer, "\n");
		PifSnapShotCommTable(_T("SNAP0016"), __FILE__, __LINE__, aszErrorBuffer);
        PifLog(FAULT_AT_PIFOPENCOM, FAULT_SHORT_RESOURCE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFOPENCOM), usPortId);
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_SHORT_RESOURCE);
        return PIF_ERROR_COM_ERRORS;
    }

	// see Microsoft document HOWTO: Specify Serial Ports Larger than COM9.
	// https://support.microsoft.com/en-us/kb/115831
	// CreateFile() can be used to get a handle to a serial port. The "Win32 Programmer's Reference" entry for "CreateFile()"
	// mentions that the share mode must be 0, the create parameter must be OPEN_EXISTING, and the template must be NULL. 
	//
	// CreateFile() is successful when you use "COM1" through "COM9" for the name of the file;
	// however, the value INVALID_HANDLE_VALUE is returned if you use "COM10" or greater. 
	//
	// If the name of the port is \\.\COM10, the correct way to specify the serial port in a call to
	// CreateFile() is "\\\\.\\COM10".
	//
	// NOTES: This syntax also works for ports COM1 through COM9. Certain boards will let you choose
	//        the port names yourself. This syntax works for those names as well.
    wsprintf(wszPortName, TEXT("\\\\.\\COM%d"), usPortId);

	/* Open the serial port. */
	/* avoid to failuer of CreateFile */
	for (i = 0; i < 10; i++) {
	    hHandle = CreateFile (wszPortName, /* Pointer to the name of the port, PifOpenCom() */
    	                  GENERIC_READ | GENERIC_WRITE,  /* Access (read-write) mode */
            	          0,            /* Share mode */
                	      NULL,         /* Pointer to the security attribute */
                    	  OPEN_EXISTING,/* How to open the serial port */
	                      0,            /* Port attributes */
    	                  NULL);        /* Handle to port with attribute */
        	                            /* to copy */

	    /* If it fails to open the port, return FALSE. */
   		if ( hHandle == INVALID_HANDLE_VALUE )   {    /* Could not open the port. */
       		dwError = GetLastError ();
			if (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_INVALID_NAME || dwError == ERROR_ACCESS_DENIED) {
				LeaveCriticalSection(&g_SioCriticalSection);
				// the COM port does not exist. probably a Virtual Serial Communications Port
				// from a USB device which was either unplugged or turned off.
				// or the COM port or Virtual Serial Communications port is in use by some other application.
				return PIF_ERROR_COM_ACCESS_DENIED;
			}
			PifLog (MODULE_PIF_OPENCOM, LOG_ERROR_PIFSIO_CODE_01);
	        PifLog (MODULE_ERROR_NO(MODULE_PIF_OPENCOM), (USHORT)dwError);
			PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFOPENCOM), usPortId);
			PifSleep(500);
		} else {
			break;
		}
    }
   	if ( hHandle == INVALID_HANDLE_VALUE )   {    /* Could not open the port. */
#if 1        
        wsprintf(wszDisplay, TEXT("CreateFile, COM%d, Last Error =%d\n"), usPortId, dwError);
        OutputDebugString(wszDisplay);
       // MessageBox(NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
        LeaveCriticalSection(&g_SioCriticalSection);
#if SR517
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_SYSTEM);
#endif
        return PIF_ERROR_COM_ERRORS;
    }

    /* clear the error and purge the receive buffer */
    dwError = (DWORD)(~0);                  // set all error code bits on
    ClearCommError(hHandle, &dwError, NULL);
    PurgeComm( hHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

    /* set device option */
    switch (SysConfig.auchComPort[usPortId]) {
    case DEVICE_PC:
        usFlag |= PC_PORT_FLAG;
        break;

    case DEVICE_KITCHEN_PRINTER:
        usFlag |= KITCHEN_PORT_FLAG;
        break;

    default:
        break;
    }
    if (pProtocol->fPip == PIF_COM_PROTOCOL_XON) {
        usFlag |= KITCHEN_PORT_FLAG;
    }

    /* make up comm. parameters */

    dwBaudRate = pProtocol->ulComBaud;
    if (dwBaudRate == 0) {
        CloseHandle(hHandle);
		PifLog(MODULE_PIF_OPENCOM, LOG_ERROR_PIFSIO_CODE_02);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_OPENCOM), (USHORT)dwError);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_OPENCOM), usPortId);
        LeaveCriticalSection(&g_SioCriticalSection);
#if 1
        wsprintf(wszDisplay, TEXT("Baud Rate == 0, COM%d"), usPortId);
        OutputDebugString(wszDisplay);
        MessageBox(NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
		PifSnapShotCommTable(_T("SNAP0016"), __FILE__, __LINE__, NULL);
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_INVALID_ARGUMENTS);
        return PIF_ERROR_COM_ERRORS;
    }

    if ((pProtocol->uchComByteFormat & (COM_BYTE_7_BITS_DATA|COM_BYTE_8_BITS_DATA)) == COM_BYTE_7_BITS_DATA) {
        bByteSize = 7;
    } else {
        bByteSize = 8;
    }
    if ((pProtocol->uchComByteFormat & COM_BYTE_2_STOP_BITS) == COM_BYTE_2_STOP_BITS) {
        bStopBits = TWOSTOPBITS;
    } else {
        bStopBits = ONESTOPBIT;
    }
    if ((pProtocol->uchComByteFormat & (COM_BYTE_ODD_PARITY|COM_BYTE_EVEN_PARITY)) == COM_BYTE_EVEN_PARITY) {
        bParity = EVENPARITY;
    } else
    if ((pProtocol->uchComByteFormat & (COM_BYTE_ODD_PARITY|COM_BYTE_EVEN_PARITY)) == COM_BYTE_ODD_PARITY) {
        bParity = ODDPARITY;
    } else {
        bParity = NOPARITY;
    }

    /* Get the default port setting information. */
    GetCommState (hHandle, &dcb);


    /* set up no flow control as default */

    dcb.BaudRate = dwBaudRate;              // Current baud 
    dcb.fBinary = TRUE;               // Binary mode; no EOF check 
    dcb.fParity = (bParity != NOPARITY);               // Enable parity checking 
    dcb.ByteSize = bByteSize;                 // Number of bits/byte, 4-8
    dcb.Parity = bParity;            // 0-4=no,odd,even,mark,space 
    dcb.StopBits = bStopBits;        // 0,1,2 = 1, 1.5, 2 
    dcb.fDsrSensitivity = FALSE;      // DSR sensitivity 
#if 0
    /* CE Emulation Driver Default */
    dcb.fOutxCtsFlow = FALSE;         // No CTS output flow control 
    dcb.fOutxDsrFlow = FALSE;         // DSR output flow control 
    dcb.fDtrControl = DTR_CONTROL_ENABLE; 
                                        // DTR flow control type 
    dcb.fDsrSensitivity = FALSE;      // DSR sensitivity 
    dcb.fTXContinueOnXoff = FALSE;     // XOFF continues Tx 
    dcb.fOutX = FALSE;                // No XON/XOFF out flow control 
    dcb.fInX = FALSE;                 // No XON/XOFF in flow control 
    dcb.fErrorChar = FALSE;           // Disable error replacement 
    dcb.fNull = FALSE;                // Disable null stripping 
    dcb.fRtsControl = RTS_CONTROL_ENABLE; 
                                        // RTS flow control 
    dcb.fAbortOnError = FALSE;        // Do not abort reads/writes on 
                                        // error
#endif
    //if ((usFlag & KITCHEN_PORT_FLAG)) {
//  if (pProtocol->fPip == PIF_COM_PROTOCOL_XON) { //
	if( SysConfig.auchComHandShakePro[usPortId] & COM_BYTE_HANDSHAKE_XONOFF)
	{

        dcb.fOutxCtsFlow = FALSE;         // No CTS output flow control 
        dcb.fOutxDsrFlow = TRUE;          // No DSR output flow control for 7161
        dcb.fTXContinueOnXoff = TRUE;                       /* XOFF continues Tx */
        dcb.fOutX             = TRUE;                       /* XON/XOFF out flow control */
        dcb.fInX              = TRUE;                       /* XON/XOFF in flow control */
		dcb.XonChar			  = 0x11;	//The default value for this property is the ASCII/ANSI value 17
		dcb.XoffChar		  = 0x13;	//The default value for this property is the ASCII/ANSI value 19

    } else
    /* set up RTS/CTS flow control by option in device configulation*/
    if (SysConfig.auchComHandShakePro[usPortId] & COM_BYTE_HANDSHAKE_RTSCTS) {

        dcb.fOutxCtsFlow = TRUE;         // CTS output flow control 
        dcb.fOutxDsrFlow = FALSE;         // No DSR output flow control 
        dcb.fTXContinueOnXoff = FALSE;     // XOFF continues Tx heee
        dcb.fOutX = FALSE;                // No XON/XOFF out flow control 
        dcb.fInX = FALSE;                 // No XON/XOFF in flow control 
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; 
        dcb.fDsrSensitivity = FALSE;      // DSR sensitivity 
    }else
	//Set up RTS flow control by option in device configulation
	if(SysConfig.auchComHandShakePro[usPortId] & COM_BYTE_HANDSHAKE_RTS) {

        dcb.fOutxCtsFlow = FALSE;         // no CTS output flow control 
        dcb.fOutxDsrFlow = FALSE;         // No DSR output flow control 
        dcb.fTXContinueOnXoff = FALSE;     // XOFF continues Tx 
        dcb.fOutX = FALSE;                // No XON/XOFF out flow control 
        dcb.fInX = FALSE;                 // No XON/XOFF in flow control 
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; 
        dcb.fDsrSensitivity = FALSE;      // DSR sensitivity
	}
	else
	//Set up CTS flow control by option in device configulation
	if(SysConfig.auchComHandShakePro[usPortId] & COM_BYTE_HANDSHAKE_CTS) {

        dcb.fOutxCtsFlow = TRUE;         // CTS output flow control 
        dcb.fOutxDsrFlow = FALSE;         // No DSR output flow control 
        dcb.fTXContinueOnXoff = FALSE;     // XOFF continues Tx 
        dcb.fOutX = FALSE;                // No XON/XOFF out flow control 
        dcb.fInX = FALSE;                 // No XON/XOFF in flow control 
        dcb.fRtsControl = FALSE; 
        dcb.fDsrSensitivity = FALSE;      // DSR sensitivity
	}else
	//Set up DSR/DTR flow control by option in device configulation
	if(SysConfig.auchComHandShakePro[usPortId] & COM_BYTE_HANDSHAKE_DTRDSR){


        dcb.fOutxCtsFlow = FALSE;         // CTS output flow control 
        dcb.fOutxDsrFlow = TRUE;         // DSR output flow control 
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; //
        dcb.fTXContinueOnXoff = FALSE;     // XOFF continues Tx 
        dcb.fOutX = FALSE;                // No XON/XOFF out flow control 
        dcb.fInX = FALSE;                 // No XON/XOFF in flow control 
        dcb.fRtsControl = FALSE; 
        dcb.fDsrSensitivity = FALSE;      // DSR sensitivity
	}


    /* Configure the port according to the specifications of the DCB */
    /* structure. */
    fResult = SetCommState (hHandle, &dcb);
    if (!fResult) {
        /* Could not create the read thread. */
        dwError = GetLastError ();
		PifLog (MODULE_PIF_OPENCOM, LOG_ERROR_PIFSIO_CODE_03);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_OPENCOM), (USHORT)dwError);
        PifLog (MODULE_DATA_VALUE(MODULE_PIF_OPENCOM), usPortId);
        CloseHandle(hHandle);
        LeaveCriticalSection(&g_SioCriticalSection);
#if 1
        wsprintf (wszDisplay, TEXT("SetCommState, COM%d, Last Error =%d"), usPortId, dwError);
        OutputDebugString (wszDisplay);
        MessageBox (NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_SYSTEM);
        return PIF_ERROR_COM_ERRORS;
    }

    /* set up default time out value */

    /* compute no. of bits / data */
    nCharBits  = 1 + bByteSize;
    nCharBits += (bParity   == NOPARITY)   ? 0 : 1;
    nCharBits += (bStopBits == ONESTOPBIT) ? 1 : 2;
    nMultiple = (2*CBR_9600 / dwBaudRate);

    fResult = GetCommTimeouts(hHandle, &comTimer);
    //Windows CE default values are listed next to the variables
	//These were verified on Windows CE
	//These default values were different on Windows XP
	//so set the values to the CE defaults
    comTimer.ReadIntervalTimeout         = 250;         /* 250 default of CE Emulation driver */
    comTimer.ReadTotalTimeoutMultiplier  = 10;          /* 10 default of CE Emulation driver */
    comTimer.ReadTotalTimeoutConstant    = 2000;        /* read within 2000 msec (100 default of CE Emulation driver) */
    comTimer.WriteTotalTimeoutMultiplier = 0;           /* 0 default of CE Emulation driver */
    comTimer.WriteTotalTimeoutConstant   = 1000;        /* allow 1000 msec to write (0 default of CE Emulation driver) */

    fResult = SetCommTimeouts(hHandle, &comTimer);
    if (! fResult)
    {
        dwError = GetLastError();
		PifLog(MODULE_PIF_OPENCOM, LOG_ERROR_PIFSIO_CODE_04);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_OPENCOM), (USHORT)dwError);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_OPENCOM), usPortId);
        CloseHandle(hHandle);
        LeaveCriticalSection(&g_SioCriticalSection);
#if 1
        wsprintf(wszDisplay, TEXT("SetCommTimeouts, COM%d, Last Error =%d"), usPortId, dwError);
        OutputDebugString(wszDisplay);
        MessageBox(NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_SYSTEM);
        return PIF_ERROR_COM_ERRORS;
    }

    aComTimerSave[sPort] = comTimer;    /* for PifComControl */

    /* Direct the port to perform extended functions SETDTR and SETRTS */
    /* SETDTR: Sends the DTR (data-terminal-ready) signal. */
    /* SETRTS: Sends the RTS (request-to-send) signal. */
    EscapeCommFunction (hHandle, SETDTR);
    EscapeCommFunction (hHandle, SETRTS);
    
    /* make up comm. event mask */
    dwCommMasks = EV_CTS | EV_DSR | EV_ERR | EV_RLSD | EV_RXCHAR;

    /* set up comm. event mask */
    fResult = SetCommMask(hHandle, dwCommMasks);
    if (! fResult)
    {
        dwError = GetLastError();
		PifLog(MODULE_PIF_OPENCOM, LOG_ERROR_PIFSIO_CODE_05);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_OPENCOM), (USHORT)dwError);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_OPENCOM), usPortId);
        CloseHandle(hHandle);
        LeaveCriticalSection(&g_SioCriticalSection);
#if 1
        wsprintf(wszDisplay, TEXT("SetCommMask, COM%d, Last Error =%d"), usPortId, dwError);
        OutputDebugString(wszDisplay);
        MessageBox(NULL, wszDisplay, TEXT("PIFABORT"), MB_OK);
#endif
        PifAbort(FAULT_AT_PIFOPENCOM, FAULT_SYSTEM);
        return PIF_ERROR_COM_ERRORS;
    }

    /* clear the error and purge the receive buffer */

//  dwErrors = (DWORD)(~0);                 // set all error code bits on
//  fResult  = ClearCommError(hComm, &dwErrors, NULL);
//  fResult  = PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);

    /* set Pif Handle Table */
    aPifSioTable[sPort].sPifHandle = sPort;
    aPifSioTable[sPort].hWin32Handle = hHandle;
    aPifSioTable[sPort].fCompulsoryReset = TRUE;
	wcsncpy(aPifSioTable[sPort].wszDeviceName, wszPortName, PIF_DEVICE_LEN);
	aPifSioTable[sPort].fsMode = 0;
	aPifSioTable[sPort].ulFilePointer = 0;

    /* for PifAbort */
    if ((SysConfig.auchComPort[usPortId & 0x00FF] == DEVICE_RJ_PRINTER) ||
        (SysConfig.auchComPort[usPortId & 0x00FF] == DEVICE_THERMAL_PRINTER)) {
        sPifPrintHandle = sPort;
    }

    LeaveCriticalSection(&g_SioCriticalSection);
    
    return sPort;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifReadCom(USHORT usPort           **
**                                          VOID FAR *pBuffer       **
**                                          USHORT usBytes)          **
**              usFile:         com handle                         **
**              pBuffer:        reading buffer                      **
**              usBytes:        sizeof pBuffer                      **
**                                                                  **
**  return:     number of bytes to be read                          **
**                                                                  **
**  Description:reading from serial i/o port                        **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifReadCom(USHORT usPort, VOID FAR *pBuffer, USHORT usBytes)
{
    DWORD   dwBytesRead;
    HANDLE  hHandle;
    BOOL    fResult;
    DWORD   dwError;
//    COMSTAT stat;
//    DWORD   dwErrors;

    EnterCriticalSection(&g_SioCriticalSection);
    
    if (PifSubSearchId(usPort, aPifSioTable, PIF_MAX_SIO) != usPort) {

        if (fsPifDuringPowerDown == TRUE) {
			/* power down process */
            LeaveCriticalSection(&g_SioCriticalSection);
            return PIF_ERROR_COM_POWER_FAILURE;
        }
        LeaveCriticalSection(&g_SioCriticalSection);
    
        PifLog (FAULT_AT_PIFREADCOM, FAULT_INVALID_HANDLE);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_READCOM), (USHORT)abs(PIF_ERROR_COM_ACCESS_DENIED));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_READCOM), usPort);
		return PIF_ERROR_COM_ACCESS_DENIED;
    }

    hHandle = aPifSioTable[usPort].hWin32Handle;
    LeaveCriticalSection(&g_SioCriticalSection);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

//    fResult = ClearCommError(hHandle, &dwErrors, &stat);
    fResult = ReadFile(hHandle, pBuffer, (DWORD)usBytes, &dwBytesRead, NULL);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

    if (fResult) {
        if (!dwBytesRead) return PIF_ERROR_COM_TIMEOUT;
        return (SHORT)dwBytesRead;
    } else {
		SHORT  sErrorCode = 0;     // error code from PifSubGetErrorCode(). must call after GetLastError().
        dwError = GetLastError();
		PifLog (MODULE_PIF_READCOM, LOG_ERROR_PIFSIO_CODE_06);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_READCOM), (USHORT)dwError);
        sErrorCode = PifSubGetErrorCode(hHandle);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_READCOM), (USHORT)abs(sErrorCode));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_READCOM), usPort);
        return (sErrorCode);
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifWriteCom(USHORT usPort           **
**                                          VOID FAR *pBuffer       **
**                                          USHORT usBytes)          **
**              usFile:         com handle                         **
**              pBuffer:        reading buffer                      **
**              usBytes:        sizeof pBuffer                      **
**                                                                  **
**  return:     number of bytes to be written                       **
**                                                                  **
**  Description:writing to serial i/o port                          **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifWriteCom(USHORT usPort, CONST VOID FAR *pBuffer,
                            USHORT usBytes)
{
    HANDLE  hHandle;
    DWORD dwBytesWritten;
    BOOL    fResult;
    DWORD   dwError;

    EnterCriticalSection(&g_SioCriticalSection);

    if (PifSubSearchId(usPort, aPifSioTable, PIF_MAX_SIO) != usPort) {

        /* power down process */
        if (fsPifDuringPowerDown == TRUE) {
            LeaveCriticalSection(&g_SioCriticalSection);
            return PIF_ERROR_COM_POWER_FAILURE;
        }

        LeaveCriticalSection(&g_SioCriticalSection);
        PifLog (MODULE_PIF_WRITECOM, FAULT_INVALID_HANDLE);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_WRITECOM), (USHORT)abs(PIF_ERROR_COM_ACCESS_DENIED));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_WRITECOM), usPort);
		return PIF_ERROR_COM_ACCESS_DENIED;
    }
    hHandle = aPifSioTable[usPort].hWin32Handle;

    LeaveCriticalSection(&g_SioCriticalSection);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

    fResult = WriteFile(hHandle, pBuffer, (DWORD)usBytes, &dwBytesWritten, NULL);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

    if (fResult) {
        if ((usBytes != dwBytesWritten) && (dwBytesWritten == 0)) {
            PurgeComm(hHandle, PURGE_TXCLEAR);
            return PIF_ERROR_COM_TIMEOUT;
        }
        return (SHORT)dwBytesWritten;
    } else {
		SHORT  sErrorCode = 0;     // error code from PifSubGetErrorCode(). must call after GetLastError().

        dwError = GetLastError();
		PifLog (MODULE_PIF_WRITECOM, LOG_ERROR_PIFSIO_CODE_07);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_WRITECOM), (USHORT)dwError);
        sErrorCode = PifSubGetErrorCode(hHandle);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_WRITECOM), (USHORT)abs(sErrorCode));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_WRITECOM), usPort);
        return (sErrorCode);
    }
}

/*fhfh
***********************************************************************
**                                                                   **
**  Synopsis:   SHORT PIFENTRY PifControlCom(USHORT usPort           **
**                                          USHORT usFunc, ...)      **
**              usFile:         com handle                           **
**              usFunc:         function code of PifControlCom       **
**                       zero or more arguments may follow usFunc    **
**                                                                   **
**  return:     PIF_OK                                               **
**                                                                   **
**  Description:misc. control of serial i/o                          **     
**                                                                   **
**********************************************************************
fhfh*/
SHORT  PifControlComInternal(USHORT usPort, USHORT usFunc, ULONG  *pulArg)
{
    USHORT usTime;
    HANDLE  hHandle;

    EnterCriticalSection(&g_SioCriticalSection);

    if (PifSubSearchId(usPort, aPifSioTable, PIF_MAX_SIO) != usPort) {
        /* power down process */
        if (fsPifDuringPowerDown == TRUE) {
            LeaveCriticalSection(&g_SioCriticalSection);
            return PIF_ERROR_COM_POWER_FAILURE;
        }

        LeaveCriticalSection(&g_SioCriticalSection);
        PifLog (FAULT_AT_PIFCONTROLCOM, FAULT_INVALID_HANDLE);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)abs(PIF_ERROR_COM_ACCESS_DENIED));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usFunc);
        return PIF_ERROR_COM_ACCESS_DENIED;;
    }
    hHandle = aPifSioTable[usPort].hWin32Handle;

    LeaveCriticalSection(&g_SioCriticalSection);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

    pulArg++;

	/*
	 *  WARNING: the arguments to PIF_COM_SET_TIME and PIF_COM_SEND_TIME are values and
	 *           not addresses of values in this modified version of PifControlCom().
	 * 
	 *           This function is used by PifControlNetComIoEx() and this difference is to
	 *           reconcile the argument list difference between PifControlCom() and PifNetControl()
	 *           for setting the send time out and the receive time out.
	*/
    if (usFunc == PIF_COM_GET_STATUS) {
        return (PifSubComGetStatus(usPort, hHandle));
    } else if (usFunc == PIF_COM_SET_TIME) {
        usTime = *(USHORT *)pulArg;
       	return (PifSubComSetTime(usPort, hHandle, usTime));
    } else if (usFunc == PIF_COM_SEND_TIME) {
        usTime = *(USHORT *)pulArg;
        return (PifSubComSendTime(usPort, hHandle, usTime));
    } else if (usFunc == PIF_COM_LOOP_TEST) {
        return (PifSubComLoopTest(usPort, hHandle));
    } else if (usFunc == PIF_COM_RECIVE_PURGE) {
        return (PifSubComPurgeReceive(usPort, hHandle));
    } else if (usFunc == PIF_COM_SEND_PURGE) {
        return (PifSubComPurgeSend(usPort, hHandle));
    } else if (usFunc == PIF_COM_RESET_COMPULSORY) {
        return (PifSubComResetCompulsory(usPort));
    } else {
        return (PIF_ERROR_COM_ERRORS);
    }
}

SHORT  PIFENTRY PifControlCom(USHORT usPort, USHORT usFunc, ...)
{
    ULONG  *pulArg = (ULONG*)(&usFunc);

    USHORT usTime;
    HANDLE  hHandle;

    EnterCriticalSection(&g_SioCriticalSection);

    if (PifSubSearchId(usPort, aPifSioTable, PIF_MAX_SIO) != usPort) {
        /* power down process */
        if (fsPifDuringPowerDown == TRUE) {
            LeaveCriticalSection(&g_SioCriticalSection);
            return PIF_ERROR_COM_POWER_FAILURE;
        }

        LeaveCriticalSection(&g_SioCriticalSection);
        PifLog (FAULT_AT_PIFCONTROLCOM, FAULT_INVALID_HANDLE);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)abs(PIF_ERROR_COM_ACCESS_DENIED));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usFunc);
        return PIF_ERROR_COM_ACCESS_DENIED;;
    }
    hHandle = aPifSioTable[usPort].hWin32Handle;

    LeaveCriticalSection(&g_SioCriticalSection);

    if (PifSioCheckPowerDown(usPort, aPifSioTable) == TRUE) {
        return PIF_ERROR_COM_POWER_FAILURE;
    }

    pulArg++;

    if (usFunc == PIF_COM_GET_STATUS) {
        return (PifSubComGetStatus(usPort, hHandle));
    } else if (usFunc == PIF_COM_SET_TIME) {
        usTime = **(USHORT **)pulArg;
       	return (PifSubComSetTime(usPort, hHandle, usTime));
    } else if (usFunc == PIF_COM_SEND_TIME) {
        usTime = **(USHORT **)pulArg;
        return (PifSubComSendTime(usPort, hHandle, usTime));
    } else if (usFunc == PIF_COM_LOOP_TEST) {
        return (PifSubComLoopTest(usPort, hHandle));
    } else if (usFunc == PIF_COM_RECIVE_PURGE) {
        return (PifSubComPurgeReceive(usPort, hHandle));
    } else if (usFunc == PIF_COM_SEND_PURGE) {
        return (PifSubComPurgeSend(usPort, hHandle));
    } else if (usFunc == PIF_COM_RESET_COMPULSORY) {
        return (PifSubComResetCompulsory(usPort));
    } else {
        return (PIF_ERROR_COM_ERRORS);
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifCloseCom(USHORT usPort)           **
**              usPort:         com handle                         **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:closing serial i/o                                  **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifCloseCom(USHORT usPort)
{
    HANDLE  hHandle;
    BOOL    fReturn;
    DWORD   dwError;

    EnterCriticalSection(&g_SioCriticalSection);

    if (PifSubSearchId(usPort, aPifSioTable, PIF_MAX_SIO) != usPort) {
        LeaveCriticalSection(&g_SioCriticalSection);
        return;
    }
    hHandle = aPifSioTable[usPort].hWin32Handle;

    if (hHandle != INVALID_HANDLE_VALUE) {

        fReturn = CloseHandle(hHandle);
        if (fReturn == 0) {
            dwError = GetLastError();
			PifLog(MODULE_PIF_CLOSECOM, LOG_ERROR_PIFSIO_CODE_08);
	        PifLog(MODULE_ERROR_NO(MODULE_PIF_CLOSECOM), (USHORT)dwError);
			PifLog (MODULE_DATA_VALUE(MODULE_PIF_CLOSECOM), usPort);
        }
    }
    aPifSioTable[usPort].sPifHandle = -1;
    aPifSioTable[usPort].hWin32Handle = INVALID_HANDLE_VALUE;

    LeaveCriticalSection(&g_SioCriticalSection);
    return;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComGetStatus(USHORT usPort, HANDLE hHandle) **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:     modem status                                        **
**                                                                  **
**  Description:read modem status of serial i/o                     **     
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComGetStatus(USHORT usPort, HANDLE hHandle)
{
    BOOL    bResult;
    COMSTAT stat;
    DWORD   dwError, dwErrors, dwModem;
    SHORT   sStatus = 0;

    bResult = ClearCommError(hHandle, &dwErrors, &stat);
    if (! bResult)
    {
        dwError = GetLastError();
        PifLog(MODULE_PIF_CONTROLCOM, PIF_COM_GET_STATUS);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_09);
        PifLog(MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        return PIF_ERROR_COM_ERRORS;                    // exit ...
    }

    // make up comm. status

    if (dwErrors & CE_FRAME)
        sStatus |= SIO32_STS_FRAMING;
    if (dwErrors & CE_OVERRUN || dwErrors & CE_RXOVER)
        sStatus |= SIO32_STS_OVERRUN;
    if (dwErrors & CE_RXPARITY)
        sStatus |= SIO32_STS_PARITY;

    bResult = GetCommModemStatus(hHandle, &dwModem);
    if (! bResult)
    {
        dwError = GetLastError();
        PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_GET_STATUS);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_10);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
        PifLog(MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        /* PifAbort(FAULT_AT_PIFCONTROLCOM, FAULT_SYSTEM); */
        return PIF_ERROR_COM_ERRORS;                    // exit ...
    }

    if (dwModem & MS_CTS_ON)
        sStatus |= SIO32_STS_CTS;
    if (dwModem & MS_DSR_ON)
        sStatus |= SIO32_STS_DSR;
    if (dwModem & MS_RLSD_ON)
        sStatus |= SIO32_STS_RLSD;
        
    return sStatus;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComSetTime(HANDLE hHandle,              **
**                                  USHORT usTime)                  **
**              hHandle:         handle value of serial i/o         **
**              usTime:          time out value of receiving        **
**                                                                  **
**  return:     PIF_OK                                              **
**                                                                  **
**  Description:set receiving time out value of serial i/o          **     
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComSetTime(USHORT usPort, HANDLE hHandle, USHORT usTime)
{
    BOOL            fResult;
    DWORD           dwError;
	COMMTIMEOUTS    comTimer = {0};

    /* check parameters */
    fResult = GetCommTimeouts(hHandle, &comTimer);
    if (! fResult)
    {
        dwError = GetLastError();
        PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_SET_TIME);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_11);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        /* PifAbort(FAULT_AT_PIFCONTROLCOM, FAULT_SYSTEM); */
        return PIF_ERROR_COM_ERRORS;
    }

    comTimer.ReadTotalTimeoutConstant = (DWORD)usTime;
    if (usTime == 0) {
        /* never time out */
        comTimer.ReadIntervalTimeout = 0;
        comTimer.ReadTotalTimeoutMultiplier = 0;
    } else {
        if (comTimer.ReadTotalTimeoutConstant == 0) {
            /* recover default time out value */
            comTimer.ReadIntervalTimeout = aComTimerSave[usPort].ReadIntervalTimeout;
            comTimer.ReadTotalTimeoutMultiplier = aComTimerSave[usPort].ReadTotalTimeoutMultiplier;
        }
    }
    /* set comm timeouts */
    fResult = SetCommTimeouts(hHandle, &comTimer);
    if (!fResult) {
        dwError = GetLastError();
        PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_SET_TIME);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_12);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        /* PifAbort(FAULT_AT_PIFCONTROLCOM, FAULT_SYSTEM); */
        return PIF_ERROR_COM_ERRORS;
    }
    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComSendTime(HANDLE hHandle,              **
**                                  USHORT usTime)                  **
**              hHandle:         handle value of serial i/o         **
**              usTime:          time out value of sending      **
**                                                                  **
**  return:     PIF_OK                                              **
**                                                                  **
**  Description:set sending time out value of serial i/o          **     
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComSendTime(USHORT usPort, HANDLE hHandle, USHORT usTime)
{
    BOOL            fResult;
    DWORD           dwError;
	COMMTIMEOUTS    comTimer = {0};

    /* check parameters */
    fResult = GetCommTimeouts(hHandle, &comTimer);
    if (! fResult)
    {
        dwError= GetLastError();
        PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_SEND_TIME);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_13);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        /* PifAbort(FAULT_AT_PIFCONTROLCOM, FAULT_SYSTEM); */
        return PIF_ERROR_COM_ERRORS;
    }

    comTimer.WriteTotalTimeoutConstant = (DWORD)usTime;
    if (usTime == 0) {
        /* never time out */
        comTimer.WriteTotalTimeoutMultiplier = 0;
    } else {
        if (comTimer.WriteTotalTimeoutConstant == 0) {
            /* recover default time out value */
            comTimer.WriteTotalTimeoutMultiplier = aComTimerSave[usPort].WriteTotalTimeoutMultiplier;
        }
    }

    /* set comm timeouts */
    fResult = SetCommTimeouts(hHandle, &comTimer);
    if (!fResult) {
        dwError = GetLastError();
        PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_SEND_TIME);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_14);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
        /* PifAbort(FAULT_AT_PIFCONTROLCOM, FAULT_SYSTEM); */
        return PIF_ERROR_COM_ERRORS;
    }
    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComLoopTest(HANDLE hHandle)           **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:     modem status                                        **
**                                                                  **
**  Description:execute loop back test of serial i/o                **     
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComLoopTest(USHORT usPort, HANDLE hHandle)
{
    DWORD   dwBytesWritten, dwBytesRead;
    UCHAR   auchLoopWrite[128];
    UCHAR   auchLoopRead[128];
    BOOL    fResult;
    DWORD   dwError;

    memset(auchLoopWrite, 'A', sizeof(auchLoopWrite));

    /* clear all errors beforehand */
    PurgeComm(hHandle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXABORT);

    fResult = WriteFile(hHandle, auchLoopWrite, sizeof(auchLoopWrite), &dwBytesWritten, NULL);
    if (!fResult) {
        /* sucessed to write */
        fResult = ReadFile(hHandle, auchLoopRead, sizeof(auchLoopRead), &dwBytesRead, NULL);
        if (fResult) {
            if (!dwBytesRead) return PIF_ERROR_COM_TIMEOUT;
            if (memcmp(auchLoopRead, auchLoopWrite, sizeof(auchLoopRead)) == 0) {
                return PIF_OK;
            } else {
                return PIF_ERROR_COM_ERRORS;
            }
        } else {
			SHORT  sErrorCode = 0;     // error code from PifSubGetErrorCode(). must call after GetLastError().

            dwError = GetLastError();
        	PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_LOOP_TEST);
			PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_15);
        	PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
			sErrorCode = PifSubGetErrorCode(hHandle);
			PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)abs(sErrorCode));
			PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
			return (sErrorCode);
        }
    } else {
		SHORT  sErrorCode = 0;     // error code from PifSubGetErrorCode(). must call after GetLastError().

        dwError = GetLastError();
       	PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_LOOP_TEST);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_16);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		sErrorCode = PifSubGetErrorCode(hHandle);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)abs(sErrorCode));
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
		return (sErrorCode);
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComPurgeReceive(HANDLE hHandle)         **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:     PIF_OK                                              **
**                                                                  **
**  Description:purge receive buffer                                ** 
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComPurgeReceive(USHORT usPort, HANDLE hHandle)
{
    BOOL     fResult;
	DWORD    dwError = 0;

    fResult  = ClearCommError(hHandle, &dwError, NULL);
    fResult  = PurgeComm(hHandle, PURGE_RXABORT | PURGE_RXCLEAR);
    if (!fResult) {
		dwError = GetLastError();
       	PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), PIF_COM_RECIVE_PURGE);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_17);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
    }

    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComPurgeSend(HANDLE hHandle)         **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:     PIF_OK                                              **
**                                                                  **
**  Description:purge send buffer                                   ** 
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComPurgeSend(USHORT usPort, HANDLE hHandle)
{
    BOOL      fResult;
	DWORD     dwError = 0;

    fResult  = ClearCommError(hHandle, &dwError, NULL);
    fResult  = PurgeComm(hHandle, PURGE_TXABORT | PURGE_TXCLEAR);
    if (!fResult) {
		dwError = GetLastError();
       	PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_SEND_PURGE);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_18);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwError);
		PifLog (MODULE_DATA_VALUE(MODULE_PIF_CONTROLCOM), usPort);
    }

    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubComResetCompulsory(USHORT usPort)           **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:     PIF_OK                                              **
**                                                                  **
**  Description:                                                    ** 
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubComResetCompulsory(USHORT usPort)
{

    aPifSioTable[usPort].fCompulsoryReset = FALSE;

    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifSubGetErrorCode(HANDLE hHandle)            **
**              hHandle:         handle value of serial i/o         **
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:get error code of RS232C                            **     
**                                                                  **
**********************************************************************
fhfh*/
static SHORT PifSubGetErrorCode(HANDLE hHandle)
{
    BOOL    bResult;
    DWORD   dwErrors = 0;
	COMSTAT stat = {0};

    bResult = ClearCommError(hHandle, &dwErrors, &stat);
    if (! bResult)
    {
        dwErrors = GetLastError();
       	PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_GET_STATUS);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_19);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwErrors);
        return PIF_ERROR_COM_ERRORS;            // exit ...
    }

    // CTS low and DSR low ?
    if (stat.fCtsHold && stat.fDsrHold)
    {
        return PIF_ERROR_COM_NO_DSR_AND_CTS;
    }
    // CTS low ?
    else if (stat.fCtsHold)
    {
        return PIF_ERROR_COM_NO_CTS;
    }
    // DSR low ?
    else if (stat.fDsrHold)
    {
        return PIF_ERROR_COM_NO_DSR;
    }
    // framing error ?
    else if (dwErrors & CE_FRAME)
    {
        return PIF_ERROR_COM_FRAMING;
    }
    // framing error ?
    else if (dwErrors & CE_MODE)
    {
        return PIF_ERROR_COM_NOT_PROVIDED;
    }
    // overrun error ?
    else if (dwErrors & CE_OVERRUN)
    {
        return PIF_ERROR_COM_OVERRUN;
    }
    // parity error ?
    else if (dwErrors & CE_RXPARITY)
    {
        return PIF_ERROR_COM_PARITY;
    }
    // buffer overflow ?
    else if (dwErrors & CE_RXOVER || dwErrors & CE_TXFULL)
    {
        return PIF_ERROR_COM_OVERRUN;
    }
    // other error ?
    else if (dwErrors != 0)
    {
       	PifLog (MODULE_PIF_CONTROLCOM, PIF_COM_GET_STATUS);
		PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), LOG_ERROR_PIFSIO_CODE_20);
        PifLog (MODULE_ERROR_NO(MODULE_PIF_CONTROLCOM), (USHORT)dwErrors);
        return PIF_ERROR_COM_ERRORS;
    }

    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PifCheckPowerDown(USHORT usHandle)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
static USHORT PifSioCheckPowerDown(USHORT usPort, PIFHANDLETABLE *pPifHandleTable)
{
    USHORT usReturn = FALSE;

    EnterCriticalSection(&g_SioCriticalSection);

    if (pPifHandleTable[usPort].fCompulsoryReset == TRUE) {
        if (pPifHandleTable[usPort].hWin32Handle == INVALID_HANDLE_VALUE) {
            usReturn = TRUE;
        } else {
            usReturn = FALSE;
        }
        /* power down process */
        if (fsPifDuringPowerDown == TRUE) {
            usReturn = TRUE;
        }
    }
    LeaveCriticalSection(&g_SioCriticalSection);
    return usReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifCloseSioHanle(VOID)                            **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:close each device handle at finalize or power down recovery **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifCloseSioHandle(USHORT usPowerDown)
{
    int i;

    /* close serial handle */
    EnterCriticalSection(&g_SioCriticalSection);

    for (i = 0; i < PIF_MAX_SIO; i++) {
        if (aPifSioTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {
            if (usPowerDown == TRUE) {
                /* close handle by power down recovery */
                if (aPifSioTable[i].fCompulsoryReset == TRUE) {
                    CloseHandle(aPifSioTable[i].hWin32Handle);
                    aPifSioTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
                }
            } else {
                /* unconditional closing */
                CloseHandle(aPifSioTable[i].hWin32Handle);
                aPifSioTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
            }
        }
    }

    LeaveCriticalSection(&g_SioCriticalSection);
}
    
/* end of pifsio.c */
