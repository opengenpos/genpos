// ActiveDevice.h: interface for the ActiveDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVEDEVICE_H__EFA5A074_A7DA_11D3_BB93_005004B8DABD__INCLUDED_)
#define AFX_ACTIVEDEVICE_H__EFA5A074_A7DA_11D3_BB93_005004B8DABD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActiveDevice  
{
public:
	CActiveDevice();
	virtual ~CActiveDevice();

	DWORD Reset(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDllFileName);
	DWORD ResetAll(LPCTSTR lpszDeviceType);
	DWORD Get(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, LPTSTR lpszDeviceName, LPTSTR lpszDllFileName);
	DWORD GetOPOSSettings(LPCTSTR lpszDeviceType,LPCTSTR lpszDevice,LPDWORD lpDeviceCount, LPTSTR lpszDeviceName, LPTSTR lpszDllFileName,LPDWORD lpType);
	DWORD Set(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDllFileName);
	void  Insert(LPCTSTR lpszName, LPCTSTR lpszDll, DWORD dwPort, CPtrList& m_lstData);

};

#endif // !defined(AFX_ACTIVEDEVICE_H__EFA5A074_A7DA_11D3_BB93_005004B8DABD__INCLUDED_)
