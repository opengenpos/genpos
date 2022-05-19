/*************************************************************************
 *
 * DeviceSS.h
 *
 * $Workfile:: DeviceSS.h                                                $
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
 * $History:: DeviceSS.h                                                 $
 *
 ************************************************************************/

#if !defined(AFX_DEVICESS_H__77BBE1F4_AF07_11D3_88FE_005004B8DA0C__INCLUDED_)
#define AFX_DEVICESS_H__77BBE1F4_AF07_11D3_88FE_005004B8DA0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

class CDeviceSS : public CDevice
{
public:
	CDeviceSS(CDeviceEngine* pEngine);
	virtual ~CDeviceSS();

// Public functions
	BOOL	Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName);
	BOOL	Close();
	BOOL	Enable(BOOL bState);
	BOOL	NotOnFile(BOOL bState);
	BOOL	ReadWeight(PDEVICEIO_SCALE_DATA pScale);

// Internal functions

// Internal variables.
protected:
};

#endif // !defined(AFX_DEVICESS_H__77BBE1F4_AF07_11D3_88FE_005004B8DA0C__INCLUDED_)
