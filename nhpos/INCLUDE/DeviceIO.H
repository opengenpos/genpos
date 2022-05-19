/*************************************************************************
 *
 * DeviceIO.h
 *
 * $Workfile:: DeviceIO.h                                                $
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
 * $History:: DeviceIO.h                                                 $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO)
#define _INC_DEVICEIO

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Error Code Value
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_SUCCESS			   0
#define	DEVICEIO_E_ILLEGAL			1001         /* means same as UIE_DEVICE_NOT_PROVIDE */
#define	DEVICEIO_E_FAILURE			1002         /* means same as UIE_DEVICE_ERROR */
#define	DEVICEIO_E_TIMEOUT			1003         /* means same as UIE_DEVICE_TIMEOUT */
#define	DEVICEIO_E_PENDING			1004
#define	DEVICEIO_E_NOHARDWARE		1005         /* means same as UIE_DEVICE_NOT_PROVIDE */
#define	DEVICEIO_E_COMERROR			1006         /* means same as UIE_DEVICE_ERROR_COM */


/*
/////////////////////////////////////////////////////////////////////////////
//  Bias Value of Each Device
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_PRINTER			1000
#define	DEVICEIO_SCANNER			1100
#define	DEVICEIO_SCALE				1200
#define	DEVICEIO_MSR				1300
#define	DEVICEIO_KEYLOCK			1400
#define	DEVICEIO_SERVERLOCK			1500
#define	DEVICEIO_DISPLAY			1600
#define	DEVICEIO_COIN				1700
#define	DEVICEIO_DRAWER				1800
#define	DEVICEIO_CHARGE_POSTING		1900
#define	DEVICEIO_EPT				2000
#define	DEVICEIO_BEVERRAGE			2100
#define	DEVICEIO_KITCHEN_CRT		2200
#define	DEVICEIO_KITCHEN_PRINTER	2300
#define	DEVICEIO_PC					2400


/*
/////////////////////////////////////////////////////////////////////////////
//  Compile Switch
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef DEVICEIO_EXPORTS
#define DEVICEIO_API __declspec(dllexport)
#else
#define DEVICEIO_API __declspec(dllimport)
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////
*/

DEVICEIO_API
HANDLE WINAPI Open(
    LPCTSTR lpszDeviceName
    );

DEVICEIO_API
DWORD WINAPI Close(HANDLE hDevice);

DEVICEIO_API
DWORD WINAPI Write(
	HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten
    );

DEVICEIO_API
DWORD WINAPI Read(
	HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD dwNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead
    );

DEVICEIO_API
DWORD WINAPI IoControl(
	HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD dwInBufferSize,
    LPVOID lpOutBuffer,
    DWORD dwOutBufferSize,
    LPDWORD lpBytesReturned
    );

DEVICEIO_API
DWORD WINAPI GetDeviceName(
    LPDWORD lpDeviceCount,
    LPTSTR lpszDeviceName
    );

DEVICEIO_API
DWORD WINAPI GetDeviceCapability(
    LPCTSTR lpszDeviceName,
    DWORD dwFunc,
    DWORD dwType,
    LPVOID lpCapsData
    );

DEVICEIO_API
DWORD WINAPI GetParameter(
    LPCTSTR lpszDeviceName,
    LPCTSTR lpszDataName, 
    LPDWORD lpDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToRead, 
    LPDWORD lpNumberOfBytesRead
    );

DEVICEIO_API
DWORD WINAPI SetParameter(
    LPCTSTR lpszDeviceName,
    LPCTSTR lpszDataName, 
    DWORD dwDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten
    );


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO) */
