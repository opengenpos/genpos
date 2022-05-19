/*************************************************************************
 *
 * DeviceDrawer.h
 *
 * $Workfile:: DeviceDrawer.h                                            $
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
 * $History:: DeviceDrawer.h                                             $
 *
 ************************************************************************/

#if !defined(AFX_DEVICEDRAWER_H__89BA2E6A_AD76_11D3_88FB_005004B8DA0C__INCLUDED_)
#define AFX_DEVICEDRAWER_H__89BA2E6A_AD76_11D3_88FB_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

class CDeviceDrawer : public CDevice
{
public:
	CDeviceDrawer(CDeviceEngine* pEngine);
	virtual ~CDeviceDrawer();

// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Close();
	BOOL	Enable(BOOL bState);
	BOOL	OpenDrawer(DWORD dwNumber);
	BOOL	GetStatus(PDEVICEIO_DRAWER_STATUS pStatus);

// Internal functions
private:
	static UINT	DoDeviceThread(LPVOID pParam);
	UINT		DeviceThread();				// thread body

// Internal variables.
protected:
	HANDLE					m_hDone;		// synch. event
	CWinThread*				m_pThread;		// ptr. to device thread object
	BOOL					m_bDoMyJob;		//
	DEVICEIO_DRAWER_STATUS	m_dsCurrent;
	DEVICEIO_DRAWER_STATUS	m_dsPrevious;
};

#endif // !defined(AFX_DEVICEDRAWER_H__89BA2E6A_AD76_11D3_88FB_005004B8DA0C__INCLUDED_)
