/*************************************************************************
 *
 * Device.h
 *
 * $Workfile:: Device.h                                                  $
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
 * $History:: Device.h                                                   $
 *
 ************************************************************************/

#if !defined(AFX_DEVICE_H__814C3280_A973_11D3_88F3_005004B8DA0C__INCLUDED_)
#define AFX_DEVICE_H__814C3280_A973_11D3_88F3_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Scf.h"							// for System Configuration


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////

// device information

const int	NUMBER_OF_DEVICE	= 1;


/////////////////////////////////////////////////////////////////////////////

class CPacket;

class CDevice  
{
public:
	CDevice();
	virtual ~CDevice();

	// public functions
public:
	HANDLE	Open(LPCTSTR lpszDeviceName);
	DWORD	Close(HANDLE hDevice);
	DWORD	Write(HANDLE hDevice,
				LPVOID lpBuffer,
				DWORD dwNumberOfBytesToWrite,
				LPDWORD lpNumberOfBytesWritten);
	DWORD	Read(HANDLE hDevice,
				LPVOID lpBuffer,
				DWORD dwNumberOfBytesToRead,
				LPDWORD lpNumberOfBytesRead);
	DWORD	IoControl(HANDLE hDevice,
				DWORD dwIoControlCode,
				LPVOID lpInBuffer,
				DWORD dwInBufferSize,
				LPVOID lpOutBuffer,
				DWORD dwOutBufferSize,
				LPDWORD lpBytesReturned);
	DWORD	GetDeviceName(LPDWORD lpDeviceCount,
				LPTSTR lpszDeviceName);
	DWORD	GetDeviceCapability(LPCTSTR lpszDeviceName,
				DWORD dwFunc,
				DWORD dwType,
				LPVOID lpCapsData);
	DWORD	GetParameter(LPCTSTR lpszDeviceName,
				LPCTSTR lpszDataName, 
				LPDWORD lpDataType, 
				LPVOID lpData,
				DWORD dwNumberOfBytesToRead, 
				LPDWORD lpNumberOfBytesRead);
	DWORD	SetParameter(LPCTSTR lpszDeviceName,
				LPCTSTR lpszDataName, 
				DWORD dwDataType,
				LPVOID lpData,
				DWORD dwNumberOfBytesToWrite,
				LPDWORD lpNumberOfBytesWritten);

	enum {	EVENT_SCALE_DATA   = 0x10000,	// scale event
			EVENT_TICK_TIMER   = 0x20000,	// tick timer event
			EVENT_USER_PACKET  = 0x40000 };	// user request event

// Internal functions
protected:
	BOOL	OnInitialize(VOID);
	VOID	OnFinalize(VOID);
	DWORD	OnWaitForEvent(BOOL fPacket);
	HANDLE	OnOpen(LPCTSTR lpszDeviceName);
	DWORD	OnClose(HANDLE hDevice);
	DWORD	OnWrite(HANDLE hDevice,
				LPVOID lpBuffer,
				DWORD dwNumberOfBytesToWrite,
				LPDWORD lpNumberOfBytesWritten);
	DWORD	OnRead(HANDLE hDevice,
				LPVOID lpBuffer,
				DWORD dwNumberOfBytesToRead,
				LPDWORD lpNumberOfBytesRead);
	DWORD	OnIoControl(HANDLE hDevice,
				DWORD dwIoControlCode,
				LPVOID lpInBuffer,
				DWORD dwInBufferSize,
				LPVOID lpOutBuffer,
				DWORD dwOutBufferSize,
				LPDWORD lpBytesReturned);
	DWORD	OnIoControlReadWeight(HANDLE hDevice,
				DWORD dwIoControlCode,
				LPVOID lpInBuffer,
				DWORD dwInBufferSize,
				LPVOID lpOutBuffer,
				DWORD dwOutBufferSize,
				LPDWORD lpBytesReturned);
	DWORD	OnGetDeviceName(LPDWORD lpDeviceCount,
				LPTSTR lpszDeviceName);
	DWORD	OnGetDeviceCapability(LPCTSTR lpszDeviceName,
				DWORD dwFunc,
				DWORD dwType,
				LPVOID lpCapsData);
	DWORD	OnGetParameter(LPCTSTR lpszDeviceName,
				LPCTSTR lpszDataName, 
				LPDWORD lpDataType, 
				LPVOID lpData,
				DWORD dwNumberOfBytesToRead, 
				LPDWORD lpNumberOfBytesRead);
	DWORD	OnSetParameter(LPCTSTR lpszDeviceName,
				LPCTSTR lpszDataName, 
				DWORD dwDataType,
				LPVOID lpData,
				DWORD dwNumberOfBytesToWrite,
				LPDWORD lpNumberOfBytesWritten);
	DWORD	OnScaleEvent();
	int		GetDeviceIndex(LPCTSTR lpszDeviceName);
	BOOL	ReadParameter(int nIndex);
	USHORT	GetPort(int nIndex);
	ULONG	GetBaud(int nIndex);
	UCHAR	GetSerialFormat(int nIndex);

private:
	static UINT	DoDeviceThread(LPVOID pParam);
	UINT		DeviceThread();				// thread body

// Internal variables.
protected:
	BOOL				m_bInitialized;		// initialized or not
	BOOL				m_bOpened;			//
	HANDLE				m_hDone;			// synch. event
	CWinThread*			m_pThread;			// ptr. to device thread object
	HANDLE				m_hScale;			// scale data event
	CPacket*			m_pScale;			// ptr. to scale packet
	CDevice*			m_pUser;
	CString				m_sName[NUMBER_OF_DEVICE];
	DEVICE_CAPS			m_Caps[NUMBER_OF_DEVICE];		// capability of summary
	DEVICE_CAPS_SERIAL	m_CapsSerial[NUMBER_OF_DEVICE];	// capability of serial
	DEVICE_CAPS_SERIAL	m_Serial[NUMBER_OF_DEVICE];		// paramerer of seral
	CString				m_sPort[NUMBER_OF_DEVICE];		// parameter of port
};

#endif // !defined(AFX_DEVICE_H__814C3280_A973_11D3_88F3_005004B8DA0C__INCLUDED_)
