// SystemParameter.cpp: implementation of the SystemParameter class.
//
//
//  WARNING FOR WINDOWS 7    **
//     DeviceConfig must be started using Run as Administrator in order to access
//     the Windows Registry keys in the tree of HKEY_LOCAL_MACHINE.
//     If DeviceConfig is run as normal user, any changes will be put into the VirtualStore
//     section of HKEY_CURRENT_USER and changes will not be made to HKEY_LOCAL_MACHINE.
//
//     We have run into a problem with provisioning changes that did not seem to take only
//     to find that we had a VirtualStore section in HKEY_CURRENT_USER which was not modified
//     by DeviceConfig since it was being Run as Administrator resulting in GenPOS using the
//     old provisioning in VirtualStore of HKEY_CURRENT_USER instead.
//  
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SCF.h"
#include "SystemParameter.h"
#include "Win32CE.h"
#include <ecr.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CONVERT_DATA aConvertData[] =
{
    {SCF_DATANAME_BAUD,      _T("300"),    SCF_CAPS_BAUD_300},          // 
    {SCF_DATANAME_BAUD,      _T("600"),    SCF_CAPS_BAUD_600},          // 
    {SCF_DATANAME_BAUD,      _T("1200"),   SCF_CAPS_BAUD_1200},         // 
    {SCF_DATANAME_BAUD,      _T("2400"),   SCF_CAPS_BAUD_2400},         // 
    {SCF_DATANAME_BAUD,      _T("4800"),   SCF_CAPS_BAUD_4800},         // 
    {SCF_DATANAME_BAUD,      _T("9600"),   SCF_CAPS_BAUD_9600},         // 
    {SCF_DATANAME_BAUD,      _T("19200"),  SCF_CAPS_BAUD_19200},        // 
    {SCF_DATANAME_BAUD,      _T("38400"),  SCF_CAPS_BAUD_38400},        // 
    {SCF_DATANAME_BAUD,      _T("57600"),  SCF_CAPS_BAUD_57600},        // 
    {SCF_DATANAME_BAUD,      _T("115200"), SCF_CAPS_BAUD_115200},       // 
    {SCF_DATANAME_CHARBIT,   _T("8"),      SCF_CAPS_CHAR_8},            // 
    {SCF_DATANAME_CHARBIT,   _T("7"),      SCF_CAPS_CHAR_7},            // 
    {SCF_DATANAME_PARITY,    _T("NONE"),   SCF_CAPS_PARITY_NONE},       // 
    {SCF_DATANAME_PARITY,    _T("EVEN"),   SCF_CAPS_PARITY_EVEN},       // 
    {SCF_DATANAME_PARITY,    _T("ODD"),    SCF_CAPS_PARITY_ODD},        // 
    {SCF_DATANAME_PARITY,    _T("MARK"),   SCF_CAPS_PARITY_MARK},       // 
    {SCF_DATANAME_PARITY,    _T("SPACE"),  SCF_CAPS_PARITY_SPACE},      // 
    {SCF_DATANAME_STOPBIT,   _T("1"),      SCF_CAPS_STOP_1},            // 
    {SCF_DATANAME_STOPBIT,   _T("2"),      SCF_CAPS_STOP_2},            // 
	{SCF_DATANAME_HANDSHAKE, _T("NONE"),   SCF_CAPS_HANDSHAKE_NONE},    // 
	{SCF_DATANAME_HANDSHAKE, _T("RTS/CTS"),SCF_CAPS_HANDSHAKE_RTSCTS},  // 
	{SCF_DATANAME_HANDSHAKE, _T("CTS"),    SCF_CAPS_HANDSHAKE_CTS},     // 
	{SCF_DATANAME_HANDSHAKE, _T("RTS"),    SCF_CAPS_HANDSHAKE_RTS},     // 
    {SCF_DATANAME_HANDSHAKE, _T("XON/XOFF"), SCF_CAPS_HANDSHAKE_XONOFF},  //
	{SCF_DATANAME_HANDSHAKE, _T("DTR/DSR"), SCF_CAPS_HANDHSAKE_DTRDSR},

    {SCF_DATANAME_MSR,       _T("ISO"),    SCF_CAPS_MSR_ISO},           // 
    {SCF_DATANAME_MSR,       _T("JIS"),    SCF_CAPS_MSR_JIS},           // 

    {SCF_DATANAME_DRAWER,    _T("DRAWER-A"), SCF_CAPS_DRAWER_A},        // 
    {SCF_DATANAME_DRAWER,    _T("DRAWER-B"), SCF_CAPS_DRAWER_B},        // 

    {NULL, NULL, 0}     // Table End.
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemParameter::CSystemParameter()
{

}

CSystemParameter::~CSystemParameter()
{

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       Set();
//
// Inputs:      
//
// Outputs:     DWORD       SCF_SUCCESS - Successful operation.
//                          other       - Error
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////

DWORD CSystemParameter::Set(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
    CString         strSubKey;
    strSubKey.Format(SYSTEM_PARAMETER_KEY _T("\\%s"), lpszDeviceName);

    CmRegistry      regKey;

    if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey, KEY_ALL_ACCESS) )
    {
        return SCF_NO_DEVICE;
    }

    // write parameter
    DWORD dwType = (dwDataType == SCF_DATA_STRING) ? REG_SZ : REG_DWORD;
    regKey.Write(lpszDataName, dwType, (const BYTE *)lpData, dwNumberOfBytesToWrite);
    *lpNumberOfBytesWritten = dwNumberOfBytesToWrite;
    regKey.CloseKey();

    return SCF_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       SetOPOSSettings();
//
// Inputs:      
//
// Outputs:     DWORD       SCF_SUCCESS - Successful operation.
//                          other       - Error
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
//Used to set the paramters for an OPOS device in the OPOS_PARAMETER_KEY location in the registry
DWORD CSystemParameter::SetOPOSSettings(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, DWORD dwDataType, LPVOID lpData, DWORD dwNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
    // make sub key name for my object
	//lpszDeviceType is the type of device like COIN
	//lpszDeviceName is the name of the device stored under the COIN type like Transact
	//lpszDataName is the paramter or option for the device
	//dwDataType is the data type for the parameter value
	//lpData is the value for the parameter of the device

	CString			s = _T("\\");
	CString			strKey = s + lpszDeviceName;
	
	CString         strSubKey;
	strSubKey.Format(OPOS_PARAMETER_KEY _T("\\%ls"), lpszDeviceType);
	strSubKey = strSubKey + strKey;

    CmRegistry      regKey;

    if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey, KEY_ALL_ACCESS) )
    {
        return SCF_NO_DEVICE;
    }

    // write parameter
    DWORD dwType = (dwDataType == SCF_DATA_STRING) ? REG_SZ : REG_DWORD;
	if(dwType == REG_DWORD)
	{
		DWORD dwData = _ttoi((TCHAR *)lpData);
		regKey.Write(lpszDataName, dwType, (const BYTE *)&dwData, sizeof(dwData));
	}else
	{
		regKey.Write(lpszDataName, dwType, (const BYTE *)lpData, dwNumberOfBytesToWrite);
	}

    *lpNumberOfBytesWritten = dwNumberOfBytesToWrite;

    regKey.CloseKey();

    return SCF_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       Get();
//
// Inputs:      
//
// Outputs:     DWORD       SCF_SUCCESS - Successful operation.
//                          other       - Error
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////

DWORD CSystemParameter::Get(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
    CString         strSubKey;
    strSubKey.Format(SYSTEM_PARAMETER_KEY _T("\\%s"), lpszDeviceName);

	CmRegistry      regKey;

    if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
    {
        return SCF_NO_DEVICE;
    }

    // read parameter 
    *lpNumberOfBytesRead = dwNumberOfBytesToRead;
    BOOL  bRet = regKey.Read(lpszDataName, lpDataType, (LPBYTE)lpData, lpNumberOfBytesRead);
    regKey.CloseKey();

    if (bRet)
    {
        *lpDataType = (*lpDataType == REG_SZ) ? SCF_DATA_STRING : SCF_DATA_BINARY;
        return SCF_SUCCESS;
    }
    else 
    {
        return SCF_ERROR;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       GetOPOSType();
//
// Inputs:      
//
// Outputs:     DWORD       SCF_SUCCESS - Successful operation.
//                          other       - Error
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
DWORD CSystemParameter::GetOPOSType(LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceName, LPCTSTR lpszDataName, LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	// make sub key name for my object
	//lpszDeviceType is the type of device like COIN
	//lpszDeviceName is the name of the device stored under the COIN type like Transact
	//lpszDataName is the paramter or option for the device
	//dwDataType is the data type for the parameter value
	//lpData is the value for the parameter of the device

	CString			s = _T("\\");
	CString			strKey = s + lpszDeviceName;
	
	CString         strSubKey;
	strSubKey.Format (OPOS_PARAMETER_KEY _T("\\%ls"), lpszDeviceType);
	strSubKey = strSubKey + strKey;

   // strSubKey.Format(OPOS_DEVICE_PARAMETER_KEY _T("\\%s"), lpszDeviceName);
    CmRegistry      regKey;
    if(! regKey.OpenKey(HKEY_LOCAL_MACHINE, strSubKey) )
    {
        return SCF_NO_DEVICE;
    }

    // read parameter 
    *lpNumberOfBytesRead = dwNumberOfBytesToRead;
    BOOL  bRet = regKey.Read(lpszDataName, lpDataType, (LPBYTE)lpData, lpNumberOfBytesRead);
    regKey.CloseKey();

    if (bRet)
    {
        *lpDataType = (*lpDataType == REG_SZ) ? SCF_DATA_STRING : SCF_DATA_BINARY;
        return SCF_SUCCESS;
    }
    else 
    {
        return SCF_ERROR;
    }
}

//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
DWORD CSystemParameter::GetOPOS(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, PDEVICE_CAPS_OPOS lpDeviceOpos)
{
	DWORD		dwRet = 0;
	CString		strSubKey;
	USHORT		usCounter = 0;
	PDEVICE_CAPS_OPOS lpDeviceOposSave = lpDeviceOpos;

	strSubKey.Format(OPOS_PARAMETER_KEY _T("\\%ls"), lpszDeviceType);

	HKEY	hKeyActiveDevice;

	(*lpDeviceCount) = 0;

	//Open the Key Containing list of OPOS Controls for a particular device
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSubKey, 0, KEY_READ, &hKeyActiveDevice) == ERROR_SUCCESS)
	{
			DWORD	dwIndex=0;
			TCHAR	chSubKey[256] = {0};		// buffer for subkey name
			DWORD   dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
			int temp = 0;
			int nCount = 0;

			//Read Sub Key Values Of Controls that are available for this device
			while( ::RegEnumKeyEx(hKeyActiveDevice, dwIndex, chSubKey, &dwSubKeyLen, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
			{
				nCount++;
				dwIndex++;
				dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
			}

			// We now have an idea of how many specific device controllers are associated with
			// this device.  Now lets iterate through, get them, and then return them.
			dwIndex = 0;
			dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
			while( (usCounter < STD_MAX_NUM_DEVICE) &&
				(::RegEnumKeyEx(hKeyActiveDevice, dwIndex, chSubKey, &dwSubKeyLen, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS))
			{
					HKEY	hKeySubKey;
					TCHAR	lpSub[256] = {0};

					_tcscpy(lpSub, strSubKey);
					_tcscat(lpSub, _T("\\"));
					_tcscat(lpSub, chSubKey);

					_tcscpy(lpDeviceOpos->aszNameOPOS, chSubKey);
					//Open Sub Keys of OPOS Devices
					if(  ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSub, 0, KEY_READ, &hKeySubKey) == ERROR_SUCCESS)
					{
						DWORD	dwSubKeyIndex = 0;
						TCHAR	chValueName[256] = {0};
						DWORD	dwValueNameLen = sizeof(chValueName) / sizeof(TCHAR);
						DWORD	dwType;
						TCHAR	chValueAttrib[256] = {0};
						DWORD	dwValueAttribLen = sizeof(chValueAttrib);
						while( temp = ::RegEnumValue(hKeySubKey, dwSubKeyIndex, chValueName, &dwValueNameLen, 0, &dwType, (UCHAR *)chValueAttrib, &dwValueAttribLen)	!= ERROR_NO_MORE_ITEMS)
						{
							_tcsncpy(lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevAttrib, chValueName, 60);
							lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevAttrib[60] = 0;
							_tcsncpy(lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevData, chValueAttrib, 60);
							lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevData[60] = 0;
							dwSubKeyIndex++;
							dwValueNameLen = sizeof(chValueName) / sizeof(TCHAR);
							dwValueAttribLen = sizeof(chValueAttrib);
							memset (chValueName, 0, sizeof(chValueName));
							memset (chValueAttrib, 0, sizeof(chValueAttrib));
							if (dwSubKeyIndex + 1 >= sizeof(lpDeviceOpos->azDevices)/sizeof(lpDeviceOpos->azDevices[0])) break;
						}
						RegCloseKey(hKeySubKey);
						lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevAttrib[0] = _T('\0');
						lpDeviceOpos->azDevices[dwSubKeyIndex].aszDevData[0] = _T('\0');
						lpDeviceOpos++;
					}
					dwIndex++;
					(*lpDeviceCount)++;
					dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey

					usCounter++;
			}
		}
	::RegCloseKey(hKeyActiveDevice);
	return dwRet;
}

//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
DWORD CSystemParameter::GetDLL_Device (LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, PDEVICE_CAPS_OPOS lpDeviceOpos)
{
	CString		strSubKey;
	strSubKey.Format(SYSTEM_PARAMETER_KEY _T("\\%ls"), lpszDeviceType);

	(*lpDeviceCount) = 0;

	//Open the Key the specified DLL control and obtain the settings information from it.
	PDEVICE_CAPS_OPOS lpDeviceOposSave = lpDeviceOpos;
	DWORD   dwRet = 0;
	HKEY	hKeyActiveDevice;
	if( ( dwRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSubKey, 0, KEY_READ, &hKeyActiveDevice)) == ERROR_SUCCESS)
	{
		int     temp = 0;
		DWORD	dwSubKeyIndex=0;
		TCHAR	chValueName[256] = {0};
		DWORD	dwValueNameLen = sizeof(chValueName) / sizeof(TCHAR);
		DWORD	dwType;
		TCHAR	chValueAttrib[256] = {0};
		DWORD	dwValueAttribLen = sizeof(chValueAttrib);

		// iterate over the key values storing them into the provided array.
		while( temp = ::RegEnumValue(hKeyActiveDevice, dwSubKeyIndex, chValueName, &dwValueNameLen, 0, &dwType, (UCHAR *)chValueAttrib, &dwValueAttribLen)	!= ERROR_NO_MORE_ITEMS)
		{
			_tcsncpy(lpDeviceOpos->azDevices[(*lpDeviceCount)].aszDevAttrib, chValueName, 60);
			lpDeviceOpos->azDevices[(*lpDeviceCount)].aszDevAttrib[60] = 0;
			_tcsncpy(lpDeviceOpos->azDevices[(*lpDeviceCount)].aszDevData, chValueAttrib, 60);
			lpDeviceOpos->azDevices[(*lpDeviceCount)].aszDevData[60] = 0;
			dwValueNameLen=sizeof(chValueName) / sizeof(TCHAR);
			dwValueAttribLen=sizeof(chValueAttrib);
			(*lpDeviceCount)++;
			dwSubKeyIndex++;
			memset (chValueName, 0, sizeof(chValueName));
			memset (chValueAttrib, 0, sizeof(chValueAttrib));
			if ((*lpDeviceCount) >= sizeof(lpDeviceOpos->azDevices)/sizeof(lpDeviceOpos->azDevices[0])) break;
		}
		::RegCloseKey(hKeyActiveDevice);
		dwRet = ERROR_SUCCESS;
	}
	return dwRet;
}

DWORD CSystemParameter::GetRsmValue(LPCTSTR lpszKeyword, LPDWORD lpDeviceCount, LPTSTR lpszValue)
{
	DWORD	  dwRet = 0;
	HKEY	  hKeySubKey;

	*lpDeviceCount = lpszValue[0] = 0;

	//Open Sub Keys of SystemParameters Registry key looking for devices of this particular type
	if( (dwRet = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE, RSM_PARAMETER_KEY, 0, KEY_READ, &hKeySubKey)) == ERROR_SUCCESS)
	{
		DWORD	dwType;
		TCHAR	atchBuffer[256] = {0};
		DWORD	dwBytes = sizeof(atchBuffer) / sizeof(atchBuffer[0]);

		// attempt to read the registry
		LONG lRet = ::RegQueryValueEx (hKeySubKey, lpszKeyword, 0, &dwType, (LPBYTE)atchBuffer, &dwBytes);
		if (lRet == 0) {
			CString   strReadDevType;

			strReadDevType = atchBuffer;
			strReadDevType.TrimLeft (); strReadDevType.TrimRight ();

			_tcscpy(lpszValue, strReadDevType);
			*lpDeviceCount = 1;
		}
		RegCloseKey(hKeySubKey);				
	}

	return dwRet;
}

//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
DWORD CSystemParameter::GetStandard(LPCTSTR lpszDeviceType, LPDWORD lpDeviceCount, TCHAR **lpDeviceOpos)
{
	// make sub key name for my object
	DWORD		dwRet = 0;
	CString		strSubKey;
	TCHAR       **lpDeviceOposSave = lpDeviceOpos;
	CString     strDevType;

	strSubKey = SYSTEM_PARAMETER_KEY;
	strDevType = lpszDeviceType;
	strDevType.TrimLeft (); strDevType.TrimRight ();

	*lpDeviceCount = 0;

	HKEY	hKeyActiveDevice;

	// Open the Key Containing the list of Standard Devices stored beneath SystemParameters
	// There will be a set of keys one per different kind of device such as NCR 5197 Printer.
	if( ( dwRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYSTEM_PARAMETER_KEY, 0, KEY_READ, &hKeyActiveDevice)) == ERROR_SUCCESS)
	{
		DWORD	dwIndex = 0;
		TCHAR	chSubKey[256] = {0};		// buffer for subkey name
		DWORD   dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		int temp = 0;
		int nCount = 0;

		dwIndex = 0;
		dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		while( ::RegEnumKeyEx(hKeyActiveDevice, dwIndex, chSubKey, &dwSubKeyLen, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
		{
			HKEY	 hKeySubKey;
			TCHAR	 lpSub[256] = {0};
			CString  strReadDevType;

			_tcscpy(lpSub, strSubKey);
			_tcscat(lpSub, _T("\\"));
			_tcscat(lpSub, chSubKey);

			//Open Sub Keys of SystemParameters Registry key looking for devices of this particular type
			if(  ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSub, 0, KEY_READ, &hKeySubKey) == ERROR_SUCCESS)
			{
				DWORD	dwSubKeyIndex = 0;
				DWORD	dwType;
				TCHAR	atchBuffer[256] = {0};
				DWORD	dwBytes = sizeof(atchBuffer) / sizeof(TCHAR);

				// attempt to read the registry
				LONG lRet = ::RegQueryValueEx(hKeySubKey, SCF_DATANAME_TYPENAME, 0, &dwType, (LPBYTE)atchBuffer, &dwBytes);

				strReadDevType = atchBuffer;
				strReadDevType.TrimLeft (); strReadDevType.TrimRight ();

				if (lRet == 0) {
					if (strDevType == strReadDevType) {
						_tcscpy(*lpDeviceOpos, chSubKey);
						lpDeviceOpos++;
						(*lpDeviceCount)++;
					}
				}
				RegCloseKey(hKeySubKey);				
			}
			dwIndex++;
			dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		}
		::RegCloseKey(hKeyActiveDevice);
		dwRet = ERROR_SUCCESS;
	}
	return dwRet;
}


//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
DWORD CSystemParameter::GetStandardByInterface (LPCTSTR lpszDeviceType, LPCTSTR lpszDeviceInterface, LPDWORD lpDeviceCount, TCHAR **lpDeviceOpos)
{
	// make sub key name for my object
	DWORD		dwRet = 0;
	CString		strSubKey;
	TCHAR       **lpDeviceOposSave = lpDeviceOpos;
	CString     strDevType, strDevType2;

	strSubKey = SYSTEM_PARAMETER_KEY;
	strDevType = lpszDeviceType;
	strDevType.TrimLeft (); strDevType.TrimRight ();

	strDevType2 = lpszDeviceInterface;
	strDevType2.TrimLeft (); strDevType2.TrimRight ();

	*lpDeviceCount = 0;

	HKEY	hKeyActiveDevice;

	// Open the Key Containing the list of Standard Devices stored beneath SystemParameters
	// There will be a set of keys one per different kind of device such as NCR 5197 Printer.
	if( ( dwRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYSTEM_PARAMETER_KEY, 0, KEY_READ, &hKeyActiveDevice)) == ERROR_SUCCESS)
	{
		DWORD	dwIndex = 0;
		TCHAR	chSubKey[256] = {0};		// buffer for subkey name
		DWORD   dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		int temp = 0;
		int nCount = 0;

		dwIndex = 0;
		dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		while( ::RegEnumKeyEx(hKeyActiveDevice, dwIndex, chSubKey, &dwSubKeyLen, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
		{
			HKEY	 hKeySubKey;
			TCHAR	 lpSub[512] = {0};
			CString  strReadDevType, strReadDevType2;

			_tcscpy(lpSub, strSubKey);
			_tcscat(lpSub, _T("\\"));
			_tcscat(lpSub, chSubKey);

			//Open Sub Keys of SystemParameters Registry key looking for devices of this particular type
			if(  ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSub, 0, KEY_READ, &hKeySubKey) == ERROR_SUCCESS)
			{
				DWORD	dwSubKeyIndex=0;
				DWORD	dwType;
				DWORD	dwType2;
				TCHAR	atchBuffer[256] = {0};
				DWORD	dwBytes=sizeof(atchBuffer) / sizeof(TCHAR);
				TCHAR	atchBuffer2[256] = {0};
				DWORD	dwBytes2=sizeof(atchBuffer2) / sizeof(TCHAR);

				// attempt to read the registry
				LONG lRet = ::RegQueryValueEx(hKeySubKey, SCF_DATANAME_TYPENAME, 0, &dwType, (LPBYTE)atchBuffer, &dwBytes);
				LONG lRet2 = ::RegQueryValueEx(hKeySubKey, SCF_DATANAME_PRINTPORT, 0, &dwType2, (LPBYTE)atchBuffer2, &dwBytes2);

				strReadDevType = atchBuffer;
				strReadDevType.TrimLeft (); strReadDevType.TrimRight ();

				strReadDevType2 = atchBuffer2;
				strReadDevType2.TrimLeft (); strReadDevType2.TrimRight ();

				if (lRet == 0) {
					if (strDevType == strReadDevType && strDevType2 == strReadDevType2) {
						_tcscpy(*lpDeviceOpos, chSubKey);
						lpDeviceOpos++;
						(*lpDeviceCount)++;
					}
				}
				RegCloseKey(hKeySubKey);				
			}
			dwIndex++;
			dwSubKeyLen = sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		}
		::RegCloseKey(hKeyActiveDevice);
		dwRet = ERROR_SUCCESS;
	}
	return dwRet;
}



/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CSystemParameter::ConvertBit2String(lpszDataName, 
//                              LPTSTR lpszDest, DWORD dwSrc);
//
// Inputs:      LPCTSTR     lpszDataName;   - Data Name
//              LPCTSTR     lpszDest;       - Pointer to Dest String Buffer
//              DWORD       dwSrc;          - 
//
// Outputs:     BOOL        = TRUE          - Converted
//                          = FALSE         - Not Converted
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CSystemParameter::ConvertBit2String(LPCTSTR lpszDataName, LPTSTR lpszDest, DWORD dwSrc)
{
    PCONVERT_DATA   pConvData;

    pConvData = aConvertData;
    while(pConvData->lpszDataName)
    {
        if (!_tcscmp(pConvData->lpszDataName, lpszDataName) && (pConvData->dwBit == dwSrc))
        {
            _tcscpy(lpszDest, pConvData->lpszString);
            return TRUE;                        // Converted
        }
        pConvData++;
    }

    return FALSE;           // Not Converted...
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   DWORD       CSystemParameter::ConvertString2Bit(LPCTSTR lpszDataName, 
//                              LPDWORD lpdwDest, LPCTSTR lpszSrc);
//
// Inputs:      LPCTSTR     lpszDataName;   - Data Name
//              LPDWORD     lpdwDest;       - Pointer to Dest Buffer
//              LPCTSTR     lpszSrc;        - Pointer to Surce String Buffer
//
// Outputs:     BOOL        = TRUE          - Converted
//                          = FALSE         - Not Converted
//
//
//  Also see  WARNING FOR WINDOWS 7   at top of this file.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CSystemParameter::ConvertString2Bit(LPCTSTR lpszDataName, LPDWORD lpdwDest, LPCTSTR lpszSrc)
{
    PCONVERT_DATA   pConvData;

    pConvData = aConvertData;
    while(pConvData->lpszDataName)
    {
        if (!_tcsicmp(pConvData->lpszDataName, lpszDataName) && 
            !_tcsicmp(pConvData->lpszString, lpszSrc))
        {
            *lpdwDest = pConvData->dwBit;
            return TRUE;                        // Converted
        }
        pConvData++;
    }

    return FALSE;           // Not Converted...
}
