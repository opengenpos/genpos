// SystemParameter.h: interface for the SystemParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMPARAMETER_H__EFA5A075_A7DA_11D3_BB93_005004B8DABD__INCLUDED_)
#define AFX_SYSTEMPARAMETER_H__EFA5A075_A7DA_11D3_BB93_005004B8DABD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSystemParameter  
{
public:
	DWORD Get(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	DWORD GetOPOSType(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	DWORD Set(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);
	DWORD SetOPOSSettings(LPCTSTR lpszDeviceType,LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);
	DWORD GetOPOS(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, PDEVICE_CAPS_OPOS lpDeviceOpos);
	DWORD GetDLL_Device(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, PDEVICE_CAPS_OPOS lpDeviceOpos);
	DWORD GetRsmValue(LPCTSTR lpszKeyword, LPDWORD lpDeviceCount, LPTSTR lpszValue);
	DWORD GetStandard(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, TCHAR **lpDeviceOpos);
	DWORD GetStandardByInterface (LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceInterface, LPDWORD lpDeviceCount, TCHAR **lpDeviceOpos);
	BOOL ConvertString2Bit(LPCTSTR lpszDataName, LPDWORD lpdwDest, LPCTSTR lpszSrc);
	BOOL ConvertBit2String(LPCTSTR lpszDataName, LPTSTR lpszDest, DWORD dwSrc);
	CSystemParameter();
	virtual ~CSystemParameter();

};

#endif // !defined(AFX_SYSTEMPARAMETER_H__EFA5A075_A7DA_11D3_BB93_005004B8DABD__INCLUDED_)
