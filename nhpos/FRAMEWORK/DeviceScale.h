/*************************************************************************
 *
 * DeviceScale.h
 *
 * $Workfile:: DeviceScale.h                                             $
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
 * $History:: DeviceScale.h                                              $
 *
 ************************************************************************/

#if !defined(AFX_DEVICESCALE_H__89BA2E67_AD76_11D3_88FB_005004B8DA0C__INCLUDED_)
#define AFX_DEVICESCALE_H__89BA2E67_AD76_11D3_88FB_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

class CDeviceScale : public CDeviceSS
{
public:
	CDeviceScale(CDeviceEngine* pEngine);
	virtual ~CDeviceScale();

// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Close();
	BOOL	Enable(BOOL bState);
	BOOL	ReadWeight(PDEVICEIO_SCALE_DATA pScale);

// Internal functions
protected:

// Internal variables.
protected:
};

class CDevicePrinter : public CDeviceSS
{
public:
	CDevicePrinter(CDeviceEngine* pEngine);
	virtual ~CDevicePrinter();

	// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Close();
	BOOL	Enable(BOOL bState);
	BOOL	Read(wchar_t *pBuffer, DWORD dwCount, DWORD *dwRead);
	BOOL	Write(LPVOID pBuffer, DWORD dwLength, DWORD *dwWritten);

	// Internal functions
protected:

	// Internal variables.
protected:
};
#endif // !defined(AFX_DEVICESCALE_H__89BA2E67_AD76_11D3_88FB_005004B8DA0C__INCLUDED_)
