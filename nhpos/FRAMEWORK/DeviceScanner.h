/*************************************************************************
 *
 * DeviceScanner.h
 *
 * $Workfile:: DeviceScanner.h                                           $
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
 * $History:: DeviceScanner.h                                            $
 *
 ************************************************************************/

#if !defined(AFX_DEVICESCANNER_H__1095A1F7_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_)
#define AFX_DEVICESCANNER_H__1095A1F7_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

class CDeviceScanner : public CDeviceSS
{
public:
	CDeviceScanner(CDeviceEngine* pEngine);
	virtual ~CDeviceScanner();

// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Close();
	BOOL	Enable(BOOL bState);
	BOOL	NotOnFile(BOOL bState);
	BOOL	ReadWeight(PDEVICEIO_SCALE_DATA pScale);

// Internal functions
private:
	static UINT	DoDeviceThread(LPVOID pParam);
	UINT		DeviceThread();				// thread body

// Internal variables.
protected:
	HANDLE			m_hDone;				// synch. event
	CWinThread*		m_pThread;				// ptr. to device thread object
	BOOL			m_bDoMyJob;				//
};

#endif // !defined(AFX_DEVICESCANNER_H__1095A1F7_ACDE_11D3_88F9_005004B8DA0C__INCLUDED_)
