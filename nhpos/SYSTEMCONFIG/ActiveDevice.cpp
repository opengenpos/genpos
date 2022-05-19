// ActiveDevice.cpp: implementation of the ActiveDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SCF.h"
#include "ActiveDevice.h"
#include "Win32CE.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef struct _DeviceInfo
{
	CString	sName;
	CString	sDll;
	DWORD	dwPort;
} DeviceInfo, *PDeviceInfo;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActiveDevice::CActiveDevice()
{

}

CActiveDevice::~CActiveDevice()
{

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		Set();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

DWORD CActiveDevice::Set(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDllFileName)
{
	// make sub key name for my object
	CString			strSubKey;
	strSubKey.Format(ACTIVE_DEVICE_KEY _T("\\%ls"), lpszDeviceType);

	CmRegistry      regKey;
	if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey, KEY_ALL_ACCESS) )
	{
        return SCF_NO_DEVICE;
	}

	// write device info 
	if (lpszDeviceName && lpszDllFileName && _tcslen(lpszDeviceName) > 0)
		regKey.WriteString(lpszDeviceName, lpszDllFileName);

	regKey.CloseKey();
	return SCF_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		Reset();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

DWORD CActiveDevice::Reset(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDllFileName)
{
	// make sub key name for my object
	CString			strSubKey;
	strSubKey.Format(ACTIVE_DEVICE_KEY _T("\\%ls"), lpszDeviceType);

	CmRegistry      regKey;
	if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey, KEY_ALL_ACCESS) )
	{
        return SCF_NO_DEVICE;
	}

	// write device info 
	regKey.DeleteValue(lpszDeviceName);
	regKey.CloseKey();
	return SCF_SUCCESS;
}

// reset all values of an Active Device

DWORD CActiveDevice::ResetAll(LPCTSTR lpszDeviceType)
{
	// make sub key name for my object
	CString			strSubKey;
	strSubKey.Format(ACTIVE_DEVICE_KEY _T("\\%ls"), lpszDeviceType);

	CmRegistry      regKey;
	if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey, KEY_ALL_ACCESS) )
	{
        return SCF_NO_DEVICE;
	}

	// write device info 
	for (DWORD dwIndex = 0; ; ) 
	{
		TCHAR	szName[SCF_MAX_DATA] = {0};
		TCHAR	szDll[SCF_MAX_DATA] = {0};
		DWORD	dwName = sizeof(szName);
		DWORD	dwDll  = sizeof(szDll);

		if (! regKey.EnumValueString(dwIndex, szName, &dwName, szDll, &dwDll))
		{
			break;
		}
		regKey.DeleteValue(szName);
	}

	regKey.CloseKey();
	return SCF_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		GetOPOSSettings();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
//				Get the list of active devices associated with a particular device type.
//				There may be more than one device.  If there is, then we will cycle
//				through all of the devices and create string of the devices by creating
//				appending the null terminated device names and dll file names.  We return
//				the count of the number of devices in lpDeviceCount.
//				In order to obtain all of the devices you need to begin with the first string
//				and find its end of string, then the next device will be following the end
//				of string character.  The loop would look something like:
//					LPTSTR xlist[20], pDevName;
//					LPTSTR lpDevNames = (LPTSTR) malloc(256);
//					LPTSTR lpDllNames = (LPTSTR) malloc(256);
//					DWORD  dwDevCount, i;
//					get (_T("DEVICE"), &dwDevCount, lpDevNames, lpDllNames);
//					pDevName = lpDevNames;
//					for (i = 0; i < dwDevCount; i++) {
//						xlist [i] = pDevName;
//						pDevName += _tcslen(pDevName) + 1;
//					}
/////////////////////////////////////////////////////////////////////////////
//Function is used to get all of the paramters or settings for an OPOS device.
DWORD CActiveDevice::GetOPOSSettings(LPCTSTR lpszDeviceType,LPCTSTR lpszDevice,LPDWORD lpDeviceCount, LPTSTR lpszDeviceName, LPTSTR lpszDllFileName,LPDWORD lpType)
{
    // make sub key name for my object
    CString     strSubKey;
	strSubKey.Format(OPOS_PARAMETER_KEY _T("\\%ls\\%s"), lpszDeviceType, lpszDevice);
    
    lpszDeviceName[0] = _T('\0');
    lpszDllFileName[0]  = _T('\0');
    *lpDeviceCount = 0;

    DWORD	    dwRet = SCF_NO_DEVICE;	
    CmRegistry      regKey;
    if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
	{
        return dwRet;
	}
    
	// query all device names
	CPtrList	lstData;
	for (DWORD dwIndex = 0; ; dwIndex++) 
	{
		TCHAR	szName[SCF_MAX_DATA] = {0};
		TCHAR	szDll[SCF_MAX_DATA] = {0};
		DWORD	dwName = sizeof(szName);
		DWORD	dwDll  = sizeof(szDll);

		if (! regKey.EnumValueString(dwIndex, szName, &dwName, szDll, &dwDll))
		{
			break;
		}

		// Insure that these strings are actually terminated by an end of string character.
		// Under circumstances in which a string value has been created without a value,
		// szDll will not be zero terminated as discovered during testing under bizarre
		// circumstances.  What happens is dwDll returns a value of 1 but there is nothing
		// valid in the szDll string that is returned.  dwDll is the number of bytes and
		// not characters so under UNICODE, dwDll should be at least 2 for one character.
		szDll[SCF_MAX_DATA - 1] = 0;
		szName[SCF_MAX_DATA - 1] = 0;
		if (dwDll < 2)
			szDll[0] = 0;

		DWORD		dwPort = 0;
		CString		sKey2;
		sKey2.Format (_T("%s\\%s"), strSubKey, szName);
		CmRegistry	reg;
		if (reg.OpenKey(HKEY_LOCAL_MACHINE, sKey2))
		{
			CString	sPort;

			if (reg.ReadString(_T("Type"), sPort))
			{
				dwPort = (DWORD)_ttol(sPort);
			}
			reg.CloseKey();
		}

		DWORD   dwCount, dwDataType;
		TCHAR   atchDeviceName[SCF_USER_BUFFER] = {0};
		LPDWORD lpDataType = &dwDataType; 
		LPVOID lpData=atchDeviceName;
		DWORD dwNumberOfBytesToRead = sizeof(atchDeviceName); 
		LPDWORD lpNumberOfBytesRead = &dwCount;

		BOOL            bRet;
		
		CmRegistry	reg2;
		if(reg2.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
		{
			// read parameter 
			*lpNumberOfBytesRead = dwNumberOfBytesToRead;
			bRet = reg2.Read(szName, lpType, (LPBYTE)lpData, lpNumberOfBytesRead);

		 	reg2.CloseKey();
			if (bRet)
			{
				if(*lpDataType==REG_SZ)
				{
					*lpType=SCF_DATA_STRING;
				}
				else if(*lpDataType==REG_DWORD)
				{
					*lpType=SCF_DATA_BINARY;
				}
			}
			else 
			{
				*lpDataType =NULL;
			}
		}
		Insert(szName, szDll, dwPort, lstData);
	}

	regKey.CloseKey();

	// write device info. to user buffer
	LPTSTR		lpszName  = lpszDeviceName;
	LPTSTR		lpszDll   = lpszDllFileName;
	int			nNameByte = SCF_USER_BUFFER;
	int			nDllByte  = SCF_USER_BUFFER;
	POSITION	pos       = lstData.GetHeadPosition();

	dwRet = SCF_NO_DATA;
	*lpDeviceCount = 0;
	while (pos != NULL)
	{
		PDeviceInfo	pInfo = (PDeviceInfo)lstData.GetNext(pos);

		if (nNameByte - (pInfo->sName.GetLength()+2)*sizeof(TCHAR) < 0 ||
			nDllByte  - (pInfo->sDll.GetLength() +2)*sizeof(TCHAR) < 0)
		{
			break;
		}

		_tcscpy(lpszName, pInfo->sName);
		lpszName += (pInfo->sName.GetLength() + 1);

		_tcscpy(lpszDll, pInfo->sDll);
		lpszDll += (pInfo->sDll.GetLength() + 1);

		(*lpDeviceCount)++;
		dwRet = SCF_SUCCESS;
	}

	while (lstData.GetCount())
	{
		delete (PDeviceInfo)lstData.RemoveHead();
	}
    
	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		Get();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
//				Get the list of active devices associated with a particular device type.
//				There may be more than one device.  If there is, then we will cycle
//				through all of the devices and create string of the devices by creating
//				appending the null terminated device names and dll file names.  We return
//				the count of the number of devices in lpDeviceCount.
//				In order to obtain all of the devices you need to begin with the first string
//				and find its end of string, then the next device will be following the end
//				of string character.  The loop would look something like:
//					LPTSTR xlist[20], pDevName;
//					LPTSTR lpDevNames = (LPTSTR) malloc(256);
//					LPTSTR lpDllNames = (LPTSTR) malloc(256);
//					DWORD  dwDevCount, i;
//					get (_T("DEVICE"), &dwDevCount, lpDevNames, lpDllNames);
//					pDevName = lpDevNames;
//					for (i = 0; i < dwDevCount; i++) {
//						xlist [i] = pDevName;
//						pDevName += _tcslen(pDevName) + 1;
//					}
/////////////////////////////////////////////////////////////////////////////

#if	1	// Add - V1.0.0.5, sort by COM port number
DWORD CActiveDevice::Get(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, LPTSTR lpszDeviceName, LPTSTR lpszDllFileName)
{
	// make sub key name for my object
	CString		strSubKey;
	strSubKey.Format(ACTIVE_DEVICE_KEY _T("\\%ls"), lpszDeviceType);

	lpszDeviceName[0] = _T('\0');
	lpszDllFileName[0]  = _T('\0');
	*lpDeviceCount = 0;

	DWORD		 dwRet = SCF_NO_DEVICE;
	CmRegistry   regKey;
	if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
	{
        return dwRet;
	}

	// query all device names
	CPtrList	lstData;
	for (DWORD dwIndex = 0; ; dwIndex++) 
	{
		TCHAR	szName[SCF_MAX_DATA] = {0};
		TCHAR	szDll[SCF_MAX_DATA] = {0};
		DWORD	dwName = sizeof(szName);
		DWORD	dwDll  = sizeof(szDll);

		if (! regKey.EnumValueString(dwIndex, szName, &dwName, szDll, &dwDll))
		{
			break;
		}

		// Insure that these strings are actually terminated by an end of string character.
		// Under circumstances in which a string value has been created without a value,
		// szDll will not be zero terminated as discovered during testing under bizarre
		// circumstances.  What happens is dwDll returns a value of 1 but there is nothing
		// valid in the szDll string that is returned.  dwDll is the number of bytes and
		// not characters so under UNICODE, dwDll should be at least 2 for one character.
		szDll[SCF_MAX_DATA - 1] = 0;
		szName[SCF_MAX_DATA - 1] = 0;
		if (dwDll < 2)
			szDll[0] = 0;

		DWORD       dwPort = 0;
		CString     sKey;
	    sKey.Format(SYSTEM_PARAMETER_KEY _T("\\%ls"), szName);
		CmRegistry	reg;
		if (reg.OpenKey(HKEY_LOCAL_MACHINE, sKey))
		{
			CString	 sPort;

			if (reg.ReadString(_T("Port"), sPort))
			{
				dwPort = (DWORD)_ttol(sPort);
			}

			reg.CloseKey();
		}

		Insert(szName, szDll, dwPort, lstData);
	}

	regKey.CloseKey();

	// write device info. to user buffer
	LPTSTR		lpszName  = lpszDeviceName;
	LPTSTR		lpszDll   = lpszDllFileName;
	int			nNameByte = SCF_USER_BUFFER;
	int			nDllByte  = SCF_USER_BUFFER;
	POSITION	pos       = lstData.GetHeadPosition();

	dwRet = SCF_NO_DATA;
	*lpDeviceCount = 0;
	while (pos != NULL)
	{
		PDeviceInfo	pInfo = (PDeviceInfo)lstData.GetNext(pos);

		if (nNameByte - (pInfo->sName.GetLength()+2)*sizeof(TCHAR) < 0 ||
			nDllByte  - (pInfo->sDll.GetLength() +2)*sizeof(TCHAR) < 0)
		{
			break;
		}

		_tcscpy(lpszName, pInfo->sName);
		lpszName += (pInfo->sName.GetLength() + 1);

		_tcscpy(lpszDll, pInfo->sDll);
		lpszDll += (pInfo->sDll.GetLength() + 1);

		(*lpDeviceCount)++;
		dwRet = SCF_SUCCESS;
	}

	while (lstData.GetCount())
	{
		delete (PDeviceInfo)lstData.RemoveHead();
	}

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	void		Insert();
//
// Inputs:		
//
// Outputs:		void		nothing
//
/////////////////////////////////////////////////////////////////////////////

void CActiveDevice::Insert(LPCTSTR lpszName, LPCTSTR lpszDll, DWORD dwPort,
	CPtrList& lstData)
{
	PDeviceInfo	pNew, pData;
	
	pNew         = new DeviceInfo;
	pNew->sName  = lpszName;
	pNew->sDll   = lpszDll;
	pNew->dwPort = dwPort;

	POSITION	base;
	POSITION	pos  = lstData.GetHeadPosition();

	while (pos != NULL)
	{
		base  = pos;
		pData = (PDeviceInfo)lstData.GetNext(pos);

		if (pData->dwPort == pNew->dwPort)
		{
			if (pData->sName.Compare(pNew->sName) > 0)
			{
				lstData.InsertBefore(base, pNew);
				return;						// exit ...
			}
		}
		else if (pData->dwPort > pNew->dwPort && pNew->dwPort)
		{
			lstData.InsertBefore(base, pNew);
			return;							// exit ...
		}
	}

	lstData.AddTail(pNew);
}
#else
DWORD CActiveDevice::Get(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, LPTSTR lpszDeviceName, LPTSTR lpszDllFileName)
{
	// make sub key name for my object

	CString			strSubKey;

	strSubKey.Format(ACTIVE_DEVICE_KEY _T("\\%ls"), lpszDeviceType);

	// open my registry,    HK

	CmRegistry      regKey;

	if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
	{
        return SCF_NO_DEVICE;
        // exit ...
	}

	// write device info 

	DWORD			dwIndex;
	LPTSTR			lpszName;
	DWORD			dwNameByte;
	LPTSTR			lpszDllName;
	DWORD			dwDllNameByte;
	DWORD			dwRet;

	dwRet       = SCF_NO_DEVICE;
	lpszName    = lpszDeviceName;
	lpszDllName = lpszDllFileName;

	for (dwIndex = 0; ; dwIndex++) 
	{
		dwNameByte    = SCF_USER_BUFFER - (DWORD)(lpszName - lpszDeviceName);
		dwDllNameByte = SCF_USER_BUFFER - (DWORD)(lpszDllName - lpszDllFileName);

		if(! regKey.EnumValueString(dwIndex, lpszName, &dwNameByte, lpszDllName, &dwDllNameByte))
		{
			break;
		}

		dwRet = SCF_SUCCESS;

		lpszDllName = lpszDllName + dwDllNameByte/sizeof(TCHAR);
												/* dwDllNameByte : actual memory byte */

		lpszName = lpszName + dwNameByte;		/* dwNameByte : character count for Unicode */ 
		lpszName++;						/* + '\0' */
	}

	*lpDeviceCount = dwIndex;

	// close my registry,    HK

	regKey.CloseKey();

	return dwRet;
}
#endif