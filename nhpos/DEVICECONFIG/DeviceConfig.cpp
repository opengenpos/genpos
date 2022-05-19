// DeviceConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "DeviceConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

//-----------------------------------------------------------
// Constructors and basic functions for the CEquipmentDialog class that is used to
// present various types of provisioning dialogs in DeviceConfig

CEquipmentDialog::CEquipmentDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd): CDialog(lpszTemplateName, pParentWnd) {}
CEquipmentDialog::CEquipmentDialog(UINT nIDTemplate, CWnd* pParentWnd): CDialog(nIDTemplate, pParentWnd) {}
void CEquipmentDialog::writeChanges (void) {}
BOOL CEquipmentDialog::readOptions (void) {return TRUE;}

// -----------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigApp

BEGIN_MESSAGE_MAP(CDeviceConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CDeviceConfigApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigApp construction

CDeviceConfigApp::CDeviceConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDeviceConfigApp object

CDeviceConfigApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigApp initialization
# pragma comment(lib, "wbemuuid.lib")

class WmiClass {
public:
	WmiClass() : m_pEnum(0), m_pclsObj(0), m_hrLastResult(0), m_uReturn(1) {}
	WmiClass(IWbemServices *pSvc) : m_pEnum(0), m_pclsObj(0), m_hrLastResult(0), m_uReturn(1) {m_pSvc = pSvc;}
	~WmiClass() { if (m_pclsObj) {m_pclsObj->Release(); m_pclsObj = 0;} if (m_pEnum) { m_pEnum->Release(); m_pEnum = 0;} }
	HRESULT ExecQuery (wchar_t *pQuery, IEnumWbemClassObject **pEnum = 0);
	int  StartEnum (HRESULT &hr);
	HRESULT GetProp (wchar_t *pProp, VARIANT &vtProp);
	bool IsValidProp () { return m_uReturn != 0 && !FAILED(m_hrLastResult); }
	HRESULT  GetMethod (wchar_t *pMethod);
	HRESULT  ExecLastMethod (void);

public:
	HRESULT  m_hrLastResult;
	ULONG    m_uReturn;

private:
	wchar_t  m_LastMethodName[256];
	wchar_t  m_LastObjectPath[512];
	IEnumWbemClassObject *m_pEnum;
	IWbemClassObject *m_pclsObj;
	IWbemServices *m_pSvc;

	IWbemClassObject *m_pMethodParmsIn;
	IWbemClassObject *m_pMethodParmsOut;

};

HRESULT WmiClass::ExecQuery (wchar_t *pQuery, IEnumWbemClassObject **pEnum /* = 0 */)
{
	m_hrLastResult = m_pSvc->ExecQuery(
		bstr_t("WQL"), 
		bstr_t(pQuery),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL,
		&m_pEnum);

	if (FAILED(m_hrLastResult)) {
		m_pEnum = 0;
	}
	if (pEnum) {
		*pEnum = m_pEnum;
	}
	return m_hrLastResult;
}

int  WmiClass::StartEnum (HRESULT &hr)
{
	hr = m_hrLastResult = m_pEnum->Next(WBEM_INFINITE, 1, &m_pclsObj, &m_uReturn);
	if (m_uReturn != 0 && !FAILED(m_hrLastResult)) {
	}

	return 0;
}

HRESULT  WmiClass::GetProp (wchar_t *pProp, VARIANT &vtProp)
{
	m_hrLastResult = m_pclsObj->Get(pProp, 0, &vtProp, 0, 0);
	return m_hrLastResult;
}

HRESULT  WmiClass::GetMethod (wchar_t *pMethod)
{
	m_hrLastResult = m_pclsObj->GetMethod(pMethod, 0, &m_pMethodParmsIn, &m_pMethodParmsOut);
	if (!FAILED(m_hrLastResult)) {
		wcsncpy (m_LastMethodName, pMethod, 250);
	}
	return m_hrLastResult;
}

HRESULT  WmiClass::ExecLastMethod (void)
{
	m_hrLastResult = m_pSvc->ExecMethod(
		bstr_t(m_LastObjectPath), 
		bstr_t(m_LastMethodName),
		0, 
		NULL,
		NULL,
		NULL,
		NULL);
	return m_hrLastResult;
}

static long long llConvertAddress (wchar_t *pString)
{
	long long  llAddress = 1;

	for (int i = 0; pString[i]; i++) {
		if (pString[i] >= L'0' && pString[i] <= L'9') {
			llAddress <<= 4;
			llAddress += pString[i] - L'0';
		} else if (pString[i] >= L'A' && pString[i] <= L'F') {
			llAddress <<= 4;
			llAddress += pString[i] - L'A' + 10;
		} else if (pString[i] >= L'a' && pString[i] <= L'f') {
			llAddress <<= 4;
			llAddress += pString[i] - L'a' + 10;
		}
	}

	return llAddress;
}

static SHORT CDeviceConfigCheckSerialNumber (TCHAR *tcsSerialNo)
{
    HRESULT hres;
    IWbemLocator *pLoc = NULL;

	*tcsSerialNo = 0;    // initialze return string to empty string

	hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
 
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        pLoc->Release();     
        return 2;                // Program has failed.
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
        return 3;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
//        bstr_t("SELECT * FROM Win32_SystemEnclosure"),
//        bstr_t("SELECT * FROM Win32_BaseBoard"),
//        bstr_t("SELECT * FROM Win32_BIOS"),
    ULONG uReturn = 0;
   
	SHORT  sRetStatus = -100;
	{
		WmiClass   myClass (pSvc);
		hres = myClass.ExecQuery (L"SELECT * FROM Win32_BIOS");
		if (FAILED(hres)) {
			pSvc->Release();
			pLoc->Release();
			return 4;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		for (myClass.StartEnum (hres); myClass.IsValidProp(); myClass.StartEnum(hres)) {
			VARIANT vtProp;

			// Get the value of the Name property
			hres = myClass.GetProp(L"SerialNumber", vtProp);
			if (!FAILED(hres)) {
				switch (vtProp.vt) {
					case VT_BSTR:
						_tcscpy (tcsSerialNo, vtProp.bstrVal);
						sRetStatus = 0;
						break;
				}
			}
			VariantClear(&vtProp);
		}
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	// read the MAC address of the terminal in case we need
	// to derive a pseudo-serial number.
	long long llMacAddress = 0;
	{
		WmiClass   myClass (pSvc);
		hres = myClass.ExecQuery (L"SELECT * FROM Win32_NetworkAdapter");
		if (FAILED(hres)) {
			pSvc->Release();
			pLoc->Release();
			return 4;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		SHORT  sRetStatusTwo = -100;
		for (myClass.StartEnum (hres); myClass.IsValidProp(); myClass.StartEnum(hres)) {
			VARIANT vtProp;

			// Get the value of the Name property
			hres = myClass.GetProp(L"PNPDeviceID", vtProp);
			if (!FAILED(hres)) {
				wchar_t *pPciVen = L"PCI\\VEN";
				switch (vtProp.vt) {
					case VT_BSTR:
						if (wcsncmp (pPciVen, vtProp.bstrVal, sizeof(pPciVen)) == 0) {
							hres = myClass.GetProp(L"MACAddress", vtProp);
							if (!FAILED(hres)) {
								switch (vtProp.vt) {
									case VT_BSTR:
										llMacAddress = llConvertAddress (vtProp.bstrVal);
										sRetStatusTwo = 0;
										break;
								}
							}
						}
						break;
 				}
			}
			VariantClear(&vtProp);
		}
	}


    // Cleanup
    // ========
    pSvc->Release();
    pLoc->Release();

	// if the search for a serial number in the BIOS did not provide
	// something then lets see if the NCR Retail Systems Manager has
	// put a serial number into the Windows Registry.
	if (sRetStatus != 0) {
		ULONG  ulCount = 0;
		TCHAR  lpszValue[256] = {0};

		ScfGetRsmValue (L"SerialNumber", &ulCount, lpszValue);
		if (ulCount > 0) {
			_tcscpy (tcsSerialNo, lpszValue);
			sRetStatus = 0;
		}
	}
	return sRetStatus;
}

BOOL CDeviceConfigApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	//SR 490 & 491, We do not want device config to be able to run while NHPOS is running
	//If it IS running while NHPOS is already running, it will be in a "read only" mode
	//where the user can read the information in device config, but cannot make any new changes
	HANDLE hMutex;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, _T("DeviceConfigApp"));

	//If the mutex does not exist, we will create it, because this is the first time that
	//the application has been created.  
	if (!hMutex)
	{
	  // Mutex doesn’t exist. This is
	  // the first instance so create
	  // the mutex.
	  hMutex = 
		CreateMutex(0, 0, _T("DeviceConfigApp"));
	}else
	{
		exit(1);
	}

	HRESULT xOleResult = ::OleInitialize(NULL);
	AfxEnableControlContainer();

	SHORT  sSerialCheck = CDeviceConfigCheckSerialNumber(m_BiosSerialNumber);

	CDeviceConfigDlg dlg;
	_tcscpy (dlg.m_BiosSerialNumber, m_BiosSerialNumber);
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	::OleUninitialize();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
