/*************************************************************************
 *
 * UIELDEV.h
 *
 * $Workfile:: UIELDEV.h                                                 $
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
 * $History:: UIELDEV.h                                                  $
 *
 ************************************************************************/

#pragma pack(push, 8)


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

BOOL UieLoadDevice(LPCTSTR lpszName, PLOAD_FUNCTION pLoadFunction, HINSTANCE* phInstance);
VOID UieFreeDevice(HINSTANCE hInstance);


#pragma pack(pop)

/* ================================== */
/* ========== End of LOADDEV.H ========== */
/* ================================== */
