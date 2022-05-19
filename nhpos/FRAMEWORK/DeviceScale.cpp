/*************************************************************************
 *
 * DeviceScale.cpp
 *
 * $Workfile:: DeviceScale.cpp                                           $
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
 * $History:: DeviceScale.cpp                                            $
 *
 * Dec-05-16: 02.02.02 : R.Chambers  : ReadWeight() now returns status of UIE_SCALE_NOT_PROVIDE if no opened.
 ************************************************************************/

#include "stdafx.h"
#include "Framework.h"
#include "DeviceIOScale.h"
#include "DeviceEngine.h"
#include "Device.h"
#include "DeviceScale.h"

#include "uie.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceScale::CDeviceScale();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceScale::CDeviceScale(CDeviceEngine* pEngine)
	: CDeviceSS(pEngine)
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Construction
//
// Prototype:	VOID	CDeviceScale::~CDeviceScale();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceScale::~CDeviceScale()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Open a device
//
// Prototype:	BOOL	CDeviceScale::Open(lpszDeviceName, lpszDllName);
//
// Inputs:		LPCTSTR	lpszDeviceName;	-
//				LPCTSTR	lpszDllName;	-
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScale::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
	BOOL	bResult;

	bResult = CDeviceSS::Open(lpszDeviceName, lpszDllName);

	// examine status

	if (! bResult)
	{
		return FALSE;						// exit ...
	}

	// exit ...

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Close the device
//
// Prototype:	BOOL	CDeviceScale::Close();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScale::Close()
{
	BOOL	bResult;

	bResult = CDeviceSS::Close();

	// exit ...

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceScale::Enable();
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScale::Enable(BOOL bState)
{
	// not open ?

	if (! m_bOpened)
	{
		return FALSE;						// exit ...
	}

	// exit ...

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL	CDeviceScale::ReadWeight(pScale);
//
// Inputs:		nothing
//
// Outputs:		BOOL	bResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceScale::ReadWeight(PDEVICEIO_SCALE_DATA pScale)
{
	DWORD				dwResult;
	DWORD				dwReturned;
	DEVICEIO_SCALE_DATA	Response = {0};

	// not open ?
	if (! m_bOpened)
	{
		pScale->sStatus = UIE_SCALE_NOT_PROVIDE;    // scale not provided or provisioned.
		return TRUE;						// exit ...
	}

	dwResult = m_pfnIoControl(
				m_hDevice,
				DEVICEIO_CTL_SCALE_READWEIGHT,
				NULL,
				0,
				&Response,
				sizeof(Response),
				&dwReturned);

	// examine status
	*pScale = Response;
	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	return TRUE;
}


//============================================================
//============================================================

CDevicePrinter::CDevicePrinter(CDeviceEngine* pEngine)
	: CDeviceSS(pEngine)
{
}


CDevicePrinter::~CDevicePrinter()
{
	Close();
}


BOOL CDevicePrinter::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
	BOOL	bResult = CDeviceSS::Open(lpszDeviceName, lpszDllName);

	// examine status
	if (!bResult)
	{
		return FALSE;						// exit ...
	}

	return TRUE;
}


BOOL CDevicePrinter::Close()
{
	BOOL	bResult = CDeviceSS::Close();

	return bResult;
}


BOOL CDevicePrinter::Enable(BOOL bState)
{
	if (!m_bOpened)
	{
		return FALSE;						// exit ...
	}
	return FALSE;
}


BOOL CDevicePrinter::Read(wchar_t *pBuffer, DWORD dwCount, DWORD *dwRead)
{
	DWORD				dwResult = DEVICEIO_E_ILLEGAL;
	DWORD				dwReturned = 0;
	DEVICEIO_SCALE_DATA	Response = { 0 };

	if (!m_bOpened)
	{
		//		pScale->sStatus = UIE_SCALE_NOT_PROVIDE;    // scale not provided or provisioned.
		return TRUE;						// exit ...
	}

	dwResult = m_pfnRead(m_hDevice, pBuffer, dwCount, dwRead);

	// examine status
	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	return TRUE;
}

BOOL	CDevicePrinter::Write(LPVOID pBuffer, DWORD dwLength, DWORD *dwWritten)
{
	DWORD				dwResult = DEVICEIO_E_ILLEGAL;
	DWORD				dwReturned = 0;

	if (!m_bOpened)
	{
		//		pScale->sStatus = UIE_SCALE_NOT_PROVIDE;    // scale not provided or provisioned.
		return TRUE;						// exit ...
	}

	dwResult = m_pfnWrite(m_hDevice, pBuffer, dwLength, dwWritten);

	// examine status
	if (dwResult != DEVICEIO_SUCCESS)
	{
		return FALSE;						// exit ...
	}

	return TRUE;

}