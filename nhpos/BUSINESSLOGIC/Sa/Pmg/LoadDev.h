/*
*===========================================================================
*
*   Copyright (c) 1999-, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       : 
* Category    : Print Manager, 2172 System Application
* Program Name: LoadDevice.h
* --------------------------------------------------------------------------
* Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                LoadDevice()  - 
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Dec-04-99  01.00.00  T.Koyama	    Initial
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>

/*
*===========================================================================
*   Local Definitions
*===========================================================================
*/
// misc. data structures

typedef struct _PROCADR {				// array of procedure address
	INT			iIndex;						// index to control
	LPCTSTR		lpName;						// ptr. to a proc. name
} PROCADR, *PPROCADR;

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
typedef HANDLE	(WINAPI *PLD_OPEN)(LPCTSTR);
typedef DWORD	(WINAPI *PLD_CLOSE)(HANDLE);
typedef DWORD	(WINAPI *PLD_WRITE)(HANDLE, LPVOID, DWORD, LPDWORD);
typedef DWORD	(WINAPI *PLD_READ)(HANDLE, LPVOID, DWORD, LPDWORD);
typedef DWORD	(WINAPI *PLD_IO_CONTROL)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD);
#ifdef _WIN32_WCE
typedef long	(WINAPI *PLD_GET_DEVICE_NAME)(LPDWORD, LPTSTR);
typedef long	(WINAPI *PLD_GET_DEVICE_CAPS)(LPCTSTR, DWORD, DWORD, LPVOID);
typedef long	(WINAPI *PLD_GET_PARAMETER)(LPCTSTR, LPCTSTR, LPDWORD, LPVOID, DWORD, LPDWORD);
typedef long	(WINAPI *PLD_SET_PARAMETER)(LPCTSTR, LPCTSTR, DWORD, LPVOID, DWORD, LPDWORD);
#else
typedef long	(       *PLD_GET_DEVICE_NAME)(LPDWORD, LPTSTR);
typedef long	(       *PLD_GET_DEVICE_CAPS)(LPCTSTR, DWORD, DWORD, LPVOID);
typedef long	(       *PLD_GET_PARAMETER)(LPCTSTR, LPCTSTR, LPDWORD, LPVOID, DWORD, LPDWORD);
typedef long	(       *PLD_SET_PARAMETER)(LPCTSTR, LPCTSTR, DWORD, LPVOID, DWORD, LPDWORD);
#endif

typedef struct _LOAD_FUNCTION { 
	PLD_OPEN			Open;
	PLD_CLOSE			Close;
	PLD_WRITE			Write;
	PLD_READ			Read;
	PLD_IO_CONTROL		IoControl;
	PLD_GET_DEVICE_NAME GetDeviceName;
	PLD_GET_DEVICE_CAPS	GetDeviceCapability;
	PLD_GET_PARAMETER	GetParameter;
	PLD_SET_PARAMETER	SetParameter;
} LOAD_FUNCTION, *PLOAD_FUNCTION;

BOOL LoadDevice(LPCTSTR	lpszName, PLOAD_FUNCTION pLoadFunction);
VOID FreeDevice(VOID);

/* ================================== */
/* ========== End of LOADDEV.H ========== */
/* ================================== */
