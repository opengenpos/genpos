// DeviceCaps.h: interface for the CDeviceCaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICECAPS_H__FEB99D33_AA57_11D3_BB95_005004B8DABD__INCLUDED_)
#define AFX_DEVICECAPS_H__FEB99D33_AA57_11D3_BB95_005004B8DABD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"		// main symbols

#include "SCF.h"

/////////////////////////////////////////////////////////////////////////////
// Constants Definition
/////////////////////////////////////////////////////////////////////////////

// device information

#if defined (DEVICE_BEVERAGE)       || \
	defined (DEVICE_CHARGE_POSTING) || \
	defined (DEVICE_EPT)            || \
	defined (DEVICE_EPT_EX)         || \
	defined (DEVICE_KITCHEN_CRT)    || \
	defined (DEVICE_PCIF)
const int	NUMBER_OF_DEVICE	= 1;
#endif

#if defined (DEVICE_KITCHEN_PRINTER)
const int	NUMBER_OF_DEVICE	= 6;
#endif


/////////////////////////////////////////////////////////////////////////////

class CDeviceCaps  
{
public:
	CDeviceCaps();
	virtual ~CDeviceCaps();
	DWORD GetDeviceName(LPDWORD lpDeviceCount, 
						LPTSTR lpszDeviceName);
	DWORD GetDeviceCapability(LPCTSTR lpszDeviceName, 
							  DWORD dwFunc, 
							  DWORD dwType, 
							  LPVOID lpCapsData);
	DWORD GetParameter(LPCTSTR lpszDeviceName, 
					   LPCTSTR lpszDataName, 
					   LPDWORD lpDataType, 
					   LPVOID lpData, 
					   DWORD dwNumberOfBytesToRead, 
					   LPDWORD lpNumberOfBytesRead);
	DWORD SetParameter(LPCTSTR lpszDeviceName, 
					   LPCTSTR lpszDataName,
					   DWORD dwDataType, 
					   LPVOID lpData, 
					   DWORD dwNumberOfBytesToWrite, 
					   LPDWORD lpNumberOfBytesWritten);
	int GetDeviceIndex(LPCTSTR lpszDeviceName);

// Internal variables.
protected:
	BOOL				m_bInitialized;		// initialized or not
//	BOOL				m_bOpened;			//
//	HANDLE				m_hDone;			// synch. event
	CString				m_sName[NUMBER_OF_DEVICE];
	DEVICE_CAPS			m_Caps[NUMBER_OF_DEVICE];
	DEVICE_CAPS_SERIAL	m_CapsSerial[NUMBER_OF_DEVICE];
};

#endif // !defined(AFX_DEVICECAPS_H__FEB99D33_AA57_11D3_BB95_005004B8DABD__INCLUDED_)
