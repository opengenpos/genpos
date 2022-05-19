/*************************************************************************
 *
 * DeviceSS.cpp
 *
 * $Workfile:: DeviceSS.cpp                                              $
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
 * $History:: DeviceSS.cpp                                               $
 *
 ************************************************************************/

#include "stdafx.h"
#include "Framework.h"
#include "DeviceIOScale.h"
#include "Device.h"
#include "DeviceSS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceSS::CDeviceSS();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceSS::CDeviceSS(CDeviceEngine* pEngine)
	: CDevice(pEngine)
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceSS::~CDeviceSS();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceSS::~CDeviceSS()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Open a device
//
// Prototype:	BOOL	CDeviceSS::Open(lpszDeviceName, lpszDllName);
//
// Inputs:		LPCTSTR	lpszDeviceName;	-
//				LPCTSTR	lpszDllName;	-
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceSS::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
	return CDevice::Open(lpszDeviceName, lpszDllName);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Close the device
//
// Prototype:	BOOL	CDeviceSS::Close();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceSS::Close()
{
	return CDevice::Close();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceSS::Enable();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceSS::Enable(BOOL bState)
{
	// exit ...

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceSS::NotOnFile();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceSS::NotOnFile(BOOL bState)
{
	// exit ...

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceSS::ReadWeight(pScale);
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceSS::ReadWeight(PDEVICEIO_SCALE_DATA pScale)
{
	// exit ...

	return FALSE;
}
