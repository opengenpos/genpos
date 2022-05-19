// SystemConfig.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SCF.h"
#include "SystemConfig.h"
#include "ActiveDevice.h"
#include "SystemParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSystemConfigApp

BEGIN_MESSAGE_MAP(CSystemConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CSystemConfigApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemConfigApp construction

CSystemConfigApp::CSystemConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSystemConfigApp object

CSystemConfigApp theApp;

// My Implementation
CActiveDevice		s_ActiveDevice;			// Active Device Infomation Interface
CSystemParameter	s_SystemParameter;		// System Parameter Interface

BOOL CSystemConfigApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::InitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// API Entry

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfSetActiveDevice();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfSetActiveDevice(
    LPCTSTR lpszDeviceType, 
    LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDllFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_ActiveDevice.Set(lpszDeviceType, lpszDeviceName, lpszDllFileName));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfResetActiveDevice();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfResetActiveDevice(
    LPCTSTR lpszDeviceType, 
    LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDllFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_ActiveDevice.Reset(lpszDeviceType, lpszDeviceName, lpszDllFileName));
}

SYSTEMCONFIG_API
DWORD ScfResetActiveDeviceAll(
    LPCTSTR lpszDeviceType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_ActiveDevice.ResetAll(lpszDeviceType));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfGetActiveDevice();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfGetActiveDevice(
    LPCTSTR lpszDeviceType, 
    LPDWORD lpDeviceCount,
    LPTSTR lpszDeviceName, 
    LPTSTR lpszDllFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_ActiveDevice.Get(lpszDeviceType, lpDeviceCount, lpszDeviceName, lpszDllFileName));
}

SYSTEMCONFIG_API
DWORD ScfGetOPOSDevice(
    LPCTSTR lpszDeviceType, 
    LPDWORD lpDeviceCount,
    PDEVICE_CAPS_OPOS lpDeviceOpos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(s_SystemParameter.GetOPOS(lpszDeviceType, lpDeviceCount, lpDeviceOpos));
}

SYSTEMCONFIG_API
DWORD ScfGetDLLDevice(
    LPCTSTR lpszDeviceType, 
    LPDWORD lpDeviceCount,
    PDEVICE_CAPS_OPOS lpDeviceOpos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(s_SystemParameter.GetDLL_Device(lpszDeviceType, lpDeviceCount, lpDeviceOpos));
}

SYSTEMCONFIG_API
DWORD ScfGetRsmValue(
    LPCTSTR lpszKeyword, 
    LPDWORD lpDeviceCount,
    LPTSTR  lpszValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(s_SystemParameter.GetRsmValue(lpszKeyword, lpDeviceCount, lpszValue));
}

SYSTEMCONFIG_API
DWORD ScfGetStandardDevice(
    LPCTSTR lpszDeviceType, 
    LPDWORD lpDeviceCount,
    TCHAR **lpDeviceOpos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(s_SystemParameter.GetStandard(lpszDeviceType, lpDeviceCount, lpDeviceOpos));
}

SYSTEMCONFIG_API
DWORD ScfGetStandardDeviceByInterface(
    LPCTSTR lpszDeviceType, 
    LPCTSTR lpszDeviceInterface, 
    LPDWORD lpDeviceCount,
    TCHAR **lpDeviceOpos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(s_SystemParameter.GetStandardByInterface(lpszDeviceType, lpszDeviceInterface, lpDeviceCount, lpDeviceOpos));
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfGetParameter();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfGetParameter(
    LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDataName, 
    LPDWORD lpDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToRead, 
    LPDWORD lpNumberOfBytesRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.Get(lpszDeviceName, lpszDataName, lpDataType, lpData, dwNumberOfBytesToRead, lpNumberOfBytesRead));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfGetOPOSType();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////
SYSTEMCONFIG_API
DWORD ScfGetOPOSType(
	LPCTSTR lpszDeviceType,					
    LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDataName, 
    LPDWORD lpDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToRead, 
    LPDWORD lpNumberOfBytesRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.GetOPOSType(lpszDeviceType, lpszDeviceName, lpszDataName, lpDataType, lpData, dwNumberOfBytesToRead, lpNumberOfBytesRead));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfGetOPOSParameters();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfGetOPOSParameters(
    LPCTSTR lpszDeviceType,
	LPCTSTR lpszDevice, 
    LPDWORD lpDeviceCount,
    LPTSTR lpszDeviceName, 
    LPTSTR lpszDllFileName,
	LPDWORD lpType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_ActiveDevice.GetOPOSSettings(lpszDeviceType,lpszDevice,lpDeviceCount, lpszDeviceName, lpszDllFileName,lpType));
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfSetParameter();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfSetParameter(
    LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDataName, 
    DWORD dwDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.Set(lpszDeviceName, lpszDataName, dwDataType, lpData, dwNumberOfBytesToWrite, lpNumberOfBytesWritten));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfSetOPOSParameter();
//
// Inputs:		
//
// Outputs:		DWORD		SCF_SUCCESS	- Successful operation.
//                          other       - Error
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
DWORD ScfSetOPOSParameter(
    LPCTSTR lpszDeviceType,
	LPCTSTR lpszDeviceName, 
    LPCTSTR lpszDataName, 
    DWORD dwDataType, 
    LPVOID lpData,
    DWORD dwNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.SetOPOSSettings(lpszDeviceType, lpszDeviceName, lpszDataName, dwDataType, lpData, dwNumberOfBytesToWrite, lpNumberOfBytesWritten));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfConvertBit2String(lpszDataName, 
//								LPTSTR lpszDest, DWORD dwSrc);
//
// Inputs:		LPCTSTR		lpszDataName;	- Data Name
//				LPCTSTR		lpszDest;		- Pointer to Dest String Buffer
//				DWORD		dwSrc;			- 
//
// Outputs:		BOOL		= TRUE			- Converted
//							= FALSE			- Not Converted
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
BOOL ScfConvertBit2String(LPCTSTR lpszDataName, LPTSTR lpszDest, DWORD dwSrc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.ConvertBit2String(lpszDataName, lpszDest, dwSrc));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfConvertString2Bit(LPCTSTR lpszDataName, 
//								LPDWORD lpdwDest, LPCTSTR lpszSrc);
//
// Inputs:		LPCTSTR		lpszDataName;	- Data Name
//				LPDWORD		lpdwDest;		- Pointer to Dest Buffer
//				LPCTSTR		lpszSrc;		- Pointer to Surce String Buffer
//
// Outputs:		BOOL		= TRUE			- Converted
//							= FALSE			- Not Converted
//
/////////////////////////////////////////////////////////////////////////////

SYSTEMCONFIG_API
BOOL ScfConvertString2Bit(LPCTSTR lpszDataName, LPDWORD lpdwDest, LPCTSTR lpszSrc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return(s_SystemParameter.ConvertString2Bit(lpszDataName, lpdwDest, lpszSrc));
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfConvertString2Bit(LPCTSTR lpszDataName, 
//								LPDWORD lpdwDest, LPCTSTR lpszSrc);
//
// Inputs:		LPCTSTR		lpszDataName;	- Data Name
//				LPDWORD		lpdwDest;		- Pointer to Dest Buffer
//				LPCTSTR		lpszSrc;		- Pointer to Surce String Buffer
//
// Outputs:		BOOL		= TRUE			- Converted
//							= FALSE			- Not Converted
//
/////////////////////////////////////////////////////////////////////////////
SYSTEMCONFIG_API
ULONG ScfExportRegistryKeyData (LPCTSTR lpszSubKeyPath, LPCTSTR  lpszOutputFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CStdioFile myExportFile;
	SHORT      sFileOpen = 0;

	try {
		myExportFile.Open (lpszOutputFileName, CFile::modeWrite | CFile::typeText | CFile::modeCreate);
		sFileOpen = 1;
	}
	catch (CFileException *myE) {
		if (myE->m_cause != CFileException::endOfFile ) {
		}
		myE->Delete();
	}

	if (sFileOpen == 0)
		return -1;

	// make sub key name for my object
	DWORD		dwRet = 0;
	USHORT		usCounter = 0;
	CString     csPrintLine;

	HKEY	hKeyActiveDevice;

	csPrintLine.Format (_T("#-------------------------------------------------\n"));
	myExportFile.WriteString (csPrintLine);

	csPrintLine.Format (_T("#  NeighborhoodPOS  Rel 2.1.0  Registry Export File\n"));
	myExportFile.WriteString (csPrintLine);

	CTime timeCur = CTime::GetCurrentTime();
	long	lYear = (long)timeCur.GetYear();
	long	lMonth = (long)timeCur.GetMonth();
	long	lDay = (long)timeCur.GetDay();

	csPrintLine.Format (_T("#  Export Date: %d/%d/%d\n"), timeCur.GetMonth(), timeCur.GetDay(), timeCur.GetYear());
	myExportFile.WriteString (csPrintLine);

	csPrintLine.Format (_T("#\n"));
	myExportFile.WriteString (csPrintLine);


	// Open the Key which contains the subkeys we are wanting to export along with their values
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszSubKeyPath, 0, KEY_READ, &hKeyActiveDevice) == ERROR_SUCCESS)
	{

		csPrintLine = _T(">");
		csPrintLine += lpszSubKeyPath;
		csPrintLine += _T("\n");
		myExportFile.WriteString (csPrintLine);

		DWORD	dwIndex=0;
		TCHAR	chSubKey[256];		// buffer for subkey name
		DWORD   dwSubKeyLen=sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		int nCount = 0;

		// We now have an idea of how many specific device controllers are associated with
		// this device.  Now lets iterate through, get them, and then return them.
		dwIndex = 0;
		dwSubKeyLen=sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey
		while(::RegEnumKeyEx(hKeyActiveDevice, dwIndex, chSubKey, &dwSubKeyLen, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
		{
			HKEY	hKeySubKey;
			TCHAR	lpSub[256];
			LONG    lRetStatus;

			csPrintLine = _T(">>");
			csPrintLine += chSubKey;
			csPrintLine += _T("\n");
			myExportFile.WriteString (csPrintLine);

			_tcscpy(lpSub, lpszSubKeyPath);
			_tcscat(lpSub, _T("\\"));
			_tcscat(lpSub, chSubKey);

			// Open each of the Sub Keys of the main key and then iterate through the list of values that are
			// associated with the sub Key.
			if(  ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSub, 0, KEY_READ, &hKeySubKey) == ERROR_SUCCESS)
			{
				DWORD	dwSubKeyIndex=0;
				TCHAR	chValueName[256];
				DWORD	dwValueNameLen=sizeof(chValueName);
				DWORD	dwType;
				TCHAR	chValueAttrib[256];
				DWORD	dwValueAttribLen=sizeof(chValueAttrib);

				while( (lRetStatus = ::RegEnumValue(hKeySubKey, dwSubKeyIndex, chValueName, &dwValueNameLen, 0, &dwType, (UCHAR *)chValueAttrib, &dwValueAttribLen)) == ERROR_SUCCESS)
				{
					csPrintLine = _T(">>>");
					csPrintLine += chValueName;
					csPrintLine += _T("=");
					csPrintLine += chValueAttrib;
					csPrintLine += _T("\n");
					myExportFile.WriteString (csPrintLine);
					dwValueNameLen=sizeof(chValueName);
					dwValueAttribLen=sizeof(chValueAttrib);
					memset (chValueName, 0, sizeof(chValueName));
					memset (chValueAttrib, 0, sizeof(chValueAttrib));
					dwSubKeyIndex++;
				}
				RegCloseKey(hKeySubKey);
			}
			dwIndex++;
			dwSubKeyLen=sizeof(chSubKey) / sizeof(TCHAR);		// size of subkey

			usCounter++;
		}
	}

	::RegCloseKey(hKeyActiveDevice);
	myExportFile.Close ();

	return (ULONG)dwRet;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	DWORD		ScfConvertString2Bit(LPCTSTR lpszDataName, 
//								LPDWORD lpdwDest, LPCTSTR lpszSrc);
//
// Inputs:		LPCTSTR		lpszDataName;	- Data Name
//				LPDWORD		lpdwDest;		- Pointer to Dest Buffer
//				LPCTSTR		lpszSrc;		- Pointer to Surce String Buffer
//
// Outputs:		BOOL		= TRUE			- Converted
//							= FALSE			- Not Converted
//
/////////////////////////////////////////////////////////////////////////////
SYSTEMCONFIG_API
ULONG ScfImportRegistryKeyData (LPCTSTR  lpszOutputFileName)
{
	CStdioFile myExportFile;
	SHORT      sFileOpen = 0;

	try {
		myExportFile.Open (lpszOutputFileName, CFile::modeRead | CFile::typeText);
		sFileOpen = 1;
	}
	catch (CFileException *myE) {
		if (myE->m_cause != CFileException::endOfFile ) {
		}
		myE->Delete();
	}

	if (sFileOpen == 0)
		return -1;

	int     iRegOpenStatus = ERROR_BADKEY;  // RegOpenKeyEx() returns ERROR_SUCCESS if open successful
	HKEY	hKeyActiveDevice;
    DWORD   dwDisposition = 0;

	// make sub key name for my object
	DWORD		dwRet = 0;
	USHORT		usCounter = 0;
	CString     csPrintLine, csKeyString;
	CString     csDeviceKeyName, csKeyPath, csKeyName, csValueName, csValueValue;

	// Open the Key which contains the subkeys we are wanting to export along with their values
	while (myExportFile.ReadString (csPrintLine))
	{
		if (csPrintLine.Left (1) == _T("#") || csPrintLine.GetLength() < 4) {
			// This is a comment line or an empty line so just ignore it
			continue;
		}
		else if (csPrintLine.Left (3) == _T(">>>")) {
			// This is a key value string that is composed of a name=value pair
			csKeyString = csPrintLine.Mid (3);
			csValueName = csKeyString.SpanExcluding(_T("="));
			int iIndex = csKeyString.Find (_T("=")) + 1;
			csValueValue = csKeyString.Mid(iIndex);
		}
		else if (csPrintLine.Left (2) == _T(">>")) {
			// This is a new key path that we need to open in order to process
			// keys.
			if (iRegOpenStatus == ERROR_SUCCESS) {
				::RegCloseKey(hKeyActiveDevice);
				iRegOpenStatus = ERROR_BADKEY;
			}
			// This is a key name
			csKeyName = csPrintLine.Mid (2);
			csValueName.Empty();
			csValueValue.Empty();
			csKeyPath = csDeviceKeyName + _T("\\") + csKeyName;
			iRegOpenStatus = ::RegCreateKeyEx(HKEY_LOCAL_MACHINE, csKeyPath, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyActiveDevice, &dwDisposition);
		}
		else if (csPrintLine.Left (1) == _T(">")) {
			csKeyName.Empty();
			csValueName.Empty();
			csValueValue.Empty();
			csDeviceKeyName = csPrintLine.Mid (1);
		}
		else {
			break;
		}

		if (iRegOpenStatus == ERROR_SUCCESS && ! csKeyName.IsEmpty ()) {
			if (! (csValueValue.IsEmpty () && csValueName.IsEmpty())) {
				UCHAR  aszValue [512];

				memcpy (aszValue, csValueValue, (csValueValue.GetLength() + 1) * sizeof(TCHAR));
				dwRet = ::RegSetValueEx (hKeyActiveDevice, csValueName, 0, REG_SZ, aszValue, (csValueValue.GetLength() + 1) * sizeof(TCHAR));
			}
			else {
			}
		}
	}

	if (iRegOpenStatus == ERROR_SUCCESS) {
		::RegCloseKey(hKeyActiveDevice);
		iRegOpenStatus = ERROR_BADKEY;
	}

	myExportFile.Close ();

	return (ULONG)dwRet;
}

//---------------------------------------------------------------------------
