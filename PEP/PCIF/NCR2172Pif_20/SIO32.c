/////////////////////////////////////////////////////////////////////////////
// SIO32.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Jun-15-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "sio32.h"
#include "debug.h"
#include "AW_Interfaces.h"




/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////

typedef struct _INFOCOM {
	BOOL	fOpened;
	HANDLE	hHandle;					// comm. handle
	HANDLE	hRead;						//
	HANDLE	hWrite;
	DWORD	dwCommMasks;
	WCHAR	szFileName[MAX_PATH];		// port name
	UINT	nPort;						// port number
	DWORD	dwBaudRate;					// baud rate
    BYTE	bByteSize;					// number of bits/byte, 4-8
    BYTE	bParity;					// 0-4 = no, odd, even, mark, space
    BYTE	bStopBits;					// 0,1,2 = 1, 1.5, 2
} INFOCOM;

INFOCOM		InfoCom;


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioInit32();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioInit32()
{
	// initialzie

	memset(&InfoCom, 0, sizeof(InfoCom));
	//ZeroMemory(&InfoCom, sizeof(InfoCom));

	// exit ...

	return SIO32_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioOpen32(uchPort, ulBaudRate, uchByteSize,
//								uchParity, uchStopBits);
//
// Inputs:		UCHAR		uchPort;		-
//				ULONG		ulBaudRate;		-
//				UCHAR		uchByteSize;	-
//				UCHAR		uchParity;		-
//				UCHAR		uchStopBits;	-
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioOpen32(UCHAR uchPort, ULONG ulBaudRate, UCHAR uchByteSize,
	UCHAR uchParity, UCHAR uchStopBits)
{
	BOOL	fResult;
	HANDLE	hComm;
	DCB		dcb;
	DWORD	dwErrors;

	DebugPrintf(L"+SioOpen32: Port=%hd, Baud=%d,Size=%hd,P=%hd,Stop=%hd\r\n",
		uchPort, ulBaudRate, uchByteSize, uchParity, uchStopBits);

	// already opened ?

	if (InfoCom.fOpened)
	{
		DebugPrintf(L"-SioOpen32: Port=%hd, SIO32_E_ALREADYOPENED\r\n", uchPort);
		return SIO32_E_ALREADYOPENED;		// exit ...
	}

	// make up comm. parameters

	InfoCom.nPort      = (UINT)uchPort;
	InfoCom.dwBaudRate = (DWORD)ulBaudRate;
	InfoCom.bByteSize  = (BYTE)uchByteSize;
	InfoCom.bParity    = (BYTE)uchParity;
	InfoCom.bStopBits  = (BYTE)uchStopBits;

	// make up comm. port name

	swprintf_s (InfoCom.szFileName, MAX_PATH, WIDE("\\\\.\\COM%u"), InfoCom.nPort);
//	sprintf(InfoCom.szFileName, "COM%u", InfoCom.nPort);

	hComm = CreateFilePep(InfoCom.szFileName,
					   GENERIC_READ | GENERIC_WRITE,
					   0,
					   NULL,				// no security attrs
					   OPEN_EXISTING,		// comm devices must use OPEN_EXISTING
					   0,					// no overlapped I/O
					   NULL);				// hTemplate must be NULL for comm devices

	if (hComm == INVALID_HANDLE_VALUE)
	{
		dwErrors = GetLastError();
		DebugPrintf(L"-SioOpen32: Port=%hd, GetLastError=%d\r\n", uchPort, dwErrors);
		return SIO32_E_CREATEFILE;			// exit ...
	}

	// clear the error and purge the receive buffer

	dwErrors = (DWORD)(~0);					// set all error code bits on
	fResult  = ClearCommError(hComm, &dwErrors, NULL);
	fResult  = PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);

	// make up new comm. state, CTR/RTS flow control

	memset(&dcb, 0, sizeof(dcb));
	//ZeroMemory(&dcb, sizeof(dcb));

	dcb.DCBlength         = sizeof(dcb);				// sizeof(DCB)
	dcb.BaudRate          = InfoCom.dwBaudRate;			// current baud rate
	dcb.fBinary           = TRUE;						// binary mode, no EOF check
	dcb.fParity           = (InfoCom.bParity != NOPARITY);	// enable parity checking
	dcb.fOutxCtsFlow      = TRUE;						// CTS output flow control
	dcb.fOutxDsrFlow      = FALSE;						// DSR output flow control
	dcb.fDtrControl       = DTR_CONTROL_ENABLE;			// DTR flow control type
	dcb.fDsrSensitivity   = FALSE;						// DSR sensitivity
	dcb.fTXContinueOnXoff = FALSE;						// XOFF continues Tx
	dcb.fOutX			  = FALSE;						// XON/XOFF out flow control
	dcb.fInX              = FALSE;						// XON/XOFF in flow control
	dcb.fErrorChar		  = FALSE;						// enable error replacement
	dcb.fNull			  = FALSE;						// enable null stripping
	dcb.fRtsControl		  = RTS_CONTROL_HANDSHAKE;		// RTS flow control
	dcb.fAbortOnError     = TRUE;						// abort reads/writes on error
	dcb.wReserved         = 0;							// must be set to zero
	dcb.XonLim			  = 0;							// transmit XON threshold
	dcb.XoffLim           = 0;							// transmit XOFF threshold
	dcb.ByteSize		  = InfoCom.bByteSize;			// number of bits/byte, 4-8
	dcb.Parity			  = InfoCom.bParity;			// 0-4=no,odd,even,mark,space
	dcb.StopBits		  = InfoCom.bStopBits;			// 0,1,2 = 1, 1.5, 2
	dcb.XonChar			  = 0;							// Tx and Rx XON character
	dcb.XoffChar          = 0;							// Tx and Rx XOFF character
	dcb.ErrorChar         = 0;							// error replacement character
	dcb.EofChar			  = 0;							// end of input character
	dcb.EvtChar			  = 0;							// received event character

	// set up the state

	fResult = SetCommState(hComm, &dcb);

	if (fResult)
	{
		DWORD			nCharBits, nMultiple;
		COMMTIMEOUTS	comTimer;

		// compute no. of bits / data

		nCharBits  = 1 + InfoCom.bByteSize;
		nCharBits += (InfoCom.bParity   == NOPARITY)   ? 0 : 1;
		nCharBits += (InfoCom.bStopBits == ONESTOPBIT) ? 1 : 2;
		nMultiple  = (nCharBits * 1000 / InfoCom.dwBaudRate) + 1;

		// prepare comm. timer value

		comTimer.ReadIntervalTimeout         = 0;
		comTimer.ReadTotalTimeoutMultiplier  = nMultiple;
		comTimer.ReadTotalTimeoutConstant    = 500;			// read within 500 msec
		comTimer.WriteTotalTimeoutMultiplier = nMultiple;
		comTimer.WriteTotalTimeoutConstant   = 1000;		// allow 1000 msec to write

		// set comm timeouts

		fResult = SetCommTimeouts(hComm, &comTimer);
	}

    if (! fResult)
    {
		dwErrors = GetLastError();
        CloseHandle(hComm);
		DebugPrintf(L"-SioOpen32: Port=%hd, GetLastError=%d\r\n", uchPort, dwErrors);
		return FALSE;						// exit ...
    }

	// make up comm. event mask

	InfoCom.dwCommMasks = EV_CTS | EV_DSR | EV_ERR | EV_RLSD | EV_RXCHAR;

	// set up comm. event mask

	fResult = SetCommMask(hComm, InfoCom.dwCommMasks);

	// clear the error and purge the receive buffer

//	dwErrors = (DWORD)(~0);					// set all error code bits on
//	fResult  = ClearCommError(hComm, &dwErrors, NULL);
//	fResult  = PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);

	// handle

	InfoCom.hHandle = hComm;

	// mark as opened

	InfoCom.fOpened = TRUE;

	// exit ...

	DebugPrintf(L"-SioOpen32: Port=%hd, SIO32_OK\r\n", uchPort);
	return SIO32_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioClose32();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioClose32()
{
	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioClose32: ALREADYCLOSED\r\n");
		return FALSE;						// exit ...
	}

	// close the comm. handle

	CloseHandle(InfoCom.hHandle);

	// mask as closed

	InfoCom.hHandle = NULL;
	InfoCom.fOpened = FALSE;

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioRead32();
//
// Inputs:		VOID*		pvReadBuf;	-
//				SHORT		sReadLen;	-
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioRead32(VOID *pvReadBuf, SHORT sReadLen)
{
	BOOL		fResult;
	DWORD		dwBytesRead, dwLastError;

	DebugPrintf(L"+SioRead32: ReadLen=%hd\r\n", sReadLen);

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"-SioRead32: ReadLen=%hd, SIO32_E_SYSERROR\r\n", sReadLen);
		return SIO32_E_SYSERROR;			// exit ...
	}

	if (sReadLen == 0)
	{
		DebugPrintf(L"-SioRead32: ReadLen=%hd\r\n", sReadLen);
		return 0;							// exit ...
	}
	else if (sReadLen < 0)
	{
		DebugPrintf(L"-SioRead32: ReadLen=%hd, SIO32_E_SYSERROR\r\n", sReadLen);
		return SIO32_E_SYSERROR;			// exit ...
	}

	// perform

	fResult = ReadFile(InfoCom.hHandle,
					   pvReadBuf,
					   (DWORD)sReadLen,
					   &dwBytesRead,
					   NULL);

	// inspect execution status

	dwLastError = fResult ? ERROR_SUCCESS : GetLastError();

	if (dwLastError != ERROR_SUCCESS)
	{
		return SioGetErrorCode32();			// exit ...
	}
	else if (dwBytesRead == 0)
	{
		DebugPrintf(L"-SioRead32: ReadLen=%hd, SIO32_E_TIMEOUT\r\n", sReadLen);
		return SIO32_E_TIMEOUT;				// exit ...
	}

	// exit ...

	DebugPrintf(L"-SioRead32: ReadLen=%hd, BytesRead=%d\r\n", sReadLen, dwBytesRead);
	return (SHORT)dwBytesRead;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioWrite32();
//
// Inputs:		VOID*		pvReadBuf;	-
//				SHORT		sReadLen;	-
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioWrite32(VOID *pvWriteBuf, SHORT sWriteLen)
{
	BOOL		fResult;
	DWORD		dwBytesWritten, dwLastError;

	DebugPrintf(L"+SioWrite32: WriteLen=%hd\r\n", sWriteLen);

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"-SioWrite32: WriteLen=%hd, SIO32_E_SYSERROR\r\n", sWriteLen);
		return SIO32_E_SYSERROR;			// exit ...
	}

	// check parameters

	if (sWriteLen == 0)
	{
		DebugPrintf(L"-SioWrite32: WriteLen=%hd\r\n", sWriteLen);
		return 0;							// exit ...
	}

	if (pvWriteBuf == NULL || sWriteLen < 0)
	{
		DebugPrintf(L"-SioWrite32: WriteLen=%hd, SIO32_E_SYSERROR\r\n", sWriteLen);
		return SIO32_E_SYSERROR;			// exit ...
	}

    // clear all errors beforehand

   	PurgeComm(InfoCom.hHandle, PURGE_RXCLEAR | PURGE_TXCLEAR
                                             | PURGE_RXABORT
                                             | PURGE_RXABORT);

	// perform

	fResult = WriteFile(InfoCom.hHandle,
						pvWriteBuf,
						(DWORD)sWriteLen,
						&dwBytesWritten,
						NULL);

	// inspect execution status

	dwLastError = fResult ? ERROR_SUCCESS : GetLastError();

	if (dwLastError != ERROR_SUCCESS)
	{
		return SioGetErrorCode32();
	}
	else if ((DWORD)sWriteLen != dwBytesWritten)
	{
		DebugPrintf(L"-SioWrite32: WriteLen=%hd, SIO32_E_WRITE_ERROR, BytesWritten=%d\r\n",
			sWriteLen, dwBytesWritten);
		return SIO32_E_WRITE_ERROR;
	}

	// exit ...

	DebugPrintf(L"-SioWrite32: WriteLen=%hd, BytesWritten=%d\r\n", sWriteLen, dwBytesWritten);
	return (SHORT)dwBytesWritten;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioCntlGetStatus32();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sStatus;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioCntlGetStatus32()
{
	BOOL	bResult;
	COMSTAT	stat;
	DWORD	dwErrors, dwModem;
	USHORT	usStatus = 0;

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioCntlGetStatus32: ALREADYCLOSED\r\n");
		return usStatus;					// exit ...
	}

	bResult = ClearCommError(InfoCom.hHandle, &dwErrors, &stat);

	if (! bResult)
	{
		DebugPrintf(L"*SioCntlGetStatus32: ClearCommError(GetLastError)=%d\r\n",
			GetLastError());
		return usStatus;					// exit ...
	}

	// make up comm. status

	if (dwErrors & CE_FRAME)
		usStatus |= SIO32_STS_FRAMING;
	if (dwErrors & CE_OVERRUN || dwErrors & CE_RXOVER)
		usStatus |= SIO32_STS_OVERRUN;
	if (dwErrors & CE_RXPARITY)
		usStatus |= SIO32_STS_PARITY;

	bResult = GetCommModemStatus(InfoCom.hHandle, &dwModem);

	if (! bResult)
	{
		DebugPrintf(L"*SioCntlGetStatus32: GetCommModemStatus(GetLastError)=%d\r\n",
			GetLastError());
		return usStatus;					// exit ...
	}

	if (dwModem & MS_CTS_ON)
		usStatus |= SIO32_STS_CTS;
	if (dwModem & MS_DSR_ON)
		usStatus |= SIO32_STS_DSR;
	if (dwModem & MS_RLSD_ON)
		usStatus |= SIO32_STS_RLSD;
		
	// exit ...

	DebugPrintf(L"*SioCntlGetStatus32: Status=0x%04hX\r\n", usStatus);
	return usStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		SioCntlSetTimer32(ulTime);
//
// Inputs:		ULONG		ulTime;		-
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL SioCntlSetTimer32(USHORT usFunc, ULONG ulTime)
{
	BOOL			fResult;
	COMMTIMEOUTS	comTimer;

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioCntlSetTimer32: ALREADYCLOSED\r\n");
		return FALSE;						// exit ...
	}

	// check parameters

	fResult = GetCommTimeouts(InfoCom.hHandle, &comTimer);

	if (! fResult)
	{
		DebugPrintf(L"*SioCntlSetTimer32: GetCommTimeouts(GetLastError)=%d\r\n",
			GetLastError());
		return FALSE;						// exit ...
	}

	switch (usFunc)
	{
	case SIO32_CNTL_TXTIMER:
		comTimer.WriteTotalTimeoutConstant = (DWORD)ulTime;
		break;
	case SIO32_CNTL_RXTIMER:
		comTimer.ReadTotalTimeoutConstant = (DWORD)ulTime;
		break;
	default:								// unknown ...
		return FALSE;						// exit ...
	}

	// set comm timeouts

	fResult = SetCommTimeouts(InfoCom.hHandle, &comTimer);

	// exit ...

	DebugPrintf(L"*SioCntlSetTimer32: SetCommTimeouts(GetLastError)=%d\r\n",
		GetLastError());
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		SioCntlClearBuffer32(usFlags);
//
// Inputs:		USHORT		usFlags;	-
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL SioCntlClearBuffer32(USHORT usFlags)
{
	BOOL	fResult;
	DWORD	dwFlags = 0;

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioCntlClearBuffer32: ALREADYCLOSED\r\n");
		return FALSE;						// exit ...
	}

	// make up parameters

	if (usFlags & SIO32_CNTL_TXCLEAR)
		dwFlags = PURGE_TXCLEAR;
	if (usFlags & SIO32_CNTL_RXCLEAR)
		dwFlags = PURGE_RXCLEAR;

	fResult = PurgeComm(InfoCom.hHandle, dwFlags);

	// exit ...

	DebugPrintf(L"*SioCntlClearBuffer32: PurgeComm(GetLastError)=%d\r\n", GetLastError());
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		SioCntlDtr32(fState);
//
// Inputs:		BOOL		fState;		-
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL SioCntlDtr32(BOOL fState)
{
	BOOL	fResult;
	DCB		dcb;

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioCntlDtr32: ALREADYCLOSED\r\n");
		return FALSE;						// exit ...
	}

	fResult = GetCommState(InfoCom.hHandle, &dcb);

	if (fResult)
	{
		dcb.fDtrControl	= (fState) ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
		fResult			= SetCommState(InfoCom.hHandle, &dcb);
	}

	// exit ...

	DebugPrintf(L"*SioCntlDtr32: SetCommState(GetLastError)=%d\r\n", GetLastError());
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		SioGetErrorCode32();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sStatus;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT SioGetErrorCode32()
{
	BOOL	bResult;
	COMSTAT	stat;
	DWORD	dwErrors;

	// still closed ?

	if (! InfoCom.fOpened)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_E_SYSERROR\r\n");
		return SIO32_E_SYSERROR;			// exit ...
	}

	bResult = ClearCommError(InfoCom.hHandle, &dwErrors, &stat);

	if (! bResult)
	{
		DebugPrintf(L"*SioGetErrorCode32: ClearCommError(GetLastError)=%d\r\n",
			GetLastError());
		return SIO32_E_SYSERROR;			// exit ...
	}

	// CTS low and DSR low ?

	if (stat.fCtsHold && stat.fDsrHold)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_CTS_DSR\r\n");
		return SIO32_ERROR_CTS_DSR;
	}

	// CTS low ?

	else if (stat.fCtsHold)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_CTS\r\n");
		return SIO32_ERROR_CTS;
	}

	// DSR low ?

	else if (stat.fDsrHold)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_DSR\r\n");
		return SIO32_ERROR_DSR;
	}

	// framing error ?

	else if (dwErrors & CE_FRAME)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_FRAMING\r\n");
		return SIO32_ERROR_FRAMING;
	}

	// framing error ?

	else if (dwErrors & CE_MODE)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_NO_PORT\r\n");
		return SIO32_ERROR_NO_PORT;
	}

	// overrun error ?

	else if (dwErrors & CE_OVERRUN)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_OVERRUN\r\n");
		return SIO32_ERROR_OVERRUN;
	}

	// parity error ?

	else if (dwErrors & CE_RXPARITY)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_PARITY\r\n");
		return SIO32_ERROR_PARITY;
	}

	// buffer overflow ?

	else if (dwErrors & CE_RXOVER || dwErrors & CE_TXFULL)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_ERROR_OVERRUN\r\n");
		return SIO32_ERROR_OVERRUN;
	}

	// other error ?

	else if (dwErrors != 0)
	{
		DebugPrintf(L"*SioGetErrorCode32: SIO32_E_SYSERROR\r\n");
		return SIO32_E_SYSERROR;
	}

	// exit ...

	DebugPrintf(L"*SioGetErrorCode32: SIO32_OK\r\n");
	return SIO32_OK;
}


///////////////////////////////	 End of File  ///////////////////////////////
