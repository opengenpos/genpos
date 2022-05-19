// dragdropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "dragdropDlg.h"

#include "Crypto.h"
#include "MySourceData.h"

#include <WinInet.h>  // from SDK

#define LICENSEKEYFILENAME  _T("LICENKEY")
#define LICENSEKEYPATHNAME  _T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\LICENKEY")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------    A FEW NOTES

// Concerning testing Drag and Drop to drop a license file onto the Terminal unlock
// dialog of DeviceConfig please note that there may be a conflict between the
// DeviceConfig utility running as Administrator and the Windows File Explorer
// directory window running as Administrator.
//
// See these stackoverflow postings:
//  https://stackoverflow.com/questions/8239271/uac-elevation-does-not-allow-drag-and-drop
//
//  https://superuser.com/questions/59051/drag-and-drop-file-into-application-under-run-as-administrator
//   The problem is tied to how security permissions work. The ability to drag and drop from a
//   normal applications to an elevated one would break the security model behind UAC. However,
//   I'm unclear as to why a UAC prompt isn't shown, thus allowing for a temporary elevated operation
//   (much like any linux user experiences every day). This is definitely something that Microsoft
//   needs to work on. What troubles me is that this problem is already old. Vista behaved the same.
//   
//   There's 2 things you can do (one ugly, one annoying)
//     - Ugly: Disable UAC. But you lose all the extra security it offers.
//     - Annoying: Use another File manager and run it too as an Administrator
//   
//   Why another file manager? Because you can't actually elevate Windows Explorer. Despite seeing
//   the option in the context menu of the Windows Explorer icon and a UAC prompt being displayed,
//   the fact is Windows Explorer will not be elevated.

//
// To generate a log that shows the results of encrypt and decrypt, you
// will need to define the following macro.

//#define DEBUG_LOG_ENCRYPT


// To allow for the generation of a license key file by entry of a
// serial number other than the terminal on which DeviceConfig is running
// then uncomment and define the following macro.

//#define ALLOW_LICENSE_CREATE

//----------------------------------------------------------------------


// CdragdropDlg dialog

// A license file will have a license pay load in the file.
// <licensepayload>
//    <serialnumber>54-1234567890</serialnumber>
//    <resellerid>1234567</resellerid>
//    <keycode>abcdefghigh</keycode>
// </licensepayload>



CdragdropDlg::CdragdropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CdragdropDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_SerialNumberString[0] = 0;
	m_ResellerIdString[0] = 0;
	m_KeyCodeString[0] = 0;

	m_LicSerialNumberString[0] = 0;
	m_LicResellerIdString[0] = 0;
	m_LicKeyCodeString[0] = 0;
}

void CdragdropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CdragdropDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_APP_DROP_STRING, OnDropString)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(WM_LBUTTONDOWN, OnBeginDrag)
	ON_WM_LBUTTONDOWN()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CdragdropDlg::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_SET_RESELLER_ID, &CdragdropDlg::OnBnClickedButtonSetResellerId)
	ON_BN_CLICKED(IDC_BUTTON_SET_SERIAL_NO, &CdragdropDlg::OnBnClickedButtonSetSerialNo)
	ON_BN_CLICKED(IDC_BUTTON9, &CdragdropDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON8, &CdragdropDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, &CdragdropDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CdragdropDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CdragdropDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CdragdropDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CdragdropDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CdragdropDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CdragdropDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON0, &CdragdropDlg::OnBnClickedButton0)
	ON_BN_CLICKED(IDC_BUTTONDASH, &CdragdropDlg::OnBnClickedButtonDash)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CdragdropDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_FETCH, &CdragdropDlg::OnBnClickedButtonFetch)
	ON_STN_CLICKED(IDC_STATIC_RESELLER_ID, &CdragdropDlg::OnStnClickedStaticResellerId)
END_MESSAGE_MAP()


// CdragdropDlg message handlers

BOOL CdragdropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	DragAcceptFiles (1);

	// OLE drag and drop and copy/paste object initialization
	pDropTarget = new CMyDropTarget(this);
	pDropTarget->pMyDropDialog = this;

	CWnd *pItem;
	pItem = GetDlgItem (IDC_STATIC_SERIALNUMBER1);
	if (pItem) {
		pItem->SetWindowText(m_tcsSerialNo);
	}

	pItem = GetDlgItem (IDC_STATIC_PORTAL_URL);
	if (pItem) {
		pItem->SetWindowText(m_tcsUrl);
	}

#if !defined(ALLOW_LICENSE_CREATE)
	// to allow for use to generate a license key file then disable the following lines
	// of source by defining ALLOW_LICENSE_CREATE.
	pItem = GetDlgItem (IDC_BUTTON_GENERATE);
	pItem->EnableWindow (FALSE);
	pItem = GetDlgItem (IDC_BUTTON_SET_SERIAL_NO);
	pItem->EnableWindow (FALSE);
#endif

#if 0

	// This dialog has some additional controls that might be useful however
	// we currently do not want to be able to use these.  So we are going to
	// hide them for now.
	pItem = GetDlgItem (IDC_STATIC_RESELLER_ID_LABEL);
	if (pItem) {
		pItem->ShowWindow (SW_HIDE); ;
	}
	pItem = GetDlgItem (IDC_STATIC_RESELLER_ID);
	if (pItem) {
		pItem->SetWindowText(m_ResellerIdString);
		pItem->ShowWindow (SW_HIDE); ;
	}

	pItem = GetDlgItem (IDC_EDIT_NEW_RESELLER_ID);
	if (pItem) {
		pItem->ShowWindow (SW_HIDE); ;
	}

	pItem = GetDlgItem (IDC_STATIC_RESELLER_ID);
	if (pItem) {
		pItem->ShowWindow (SW_HIDE); ;
	}

	pItem = GetDlgItem (IDC_BUTTON_SET_RESELLER_ID);
	if (pItem) {
		pItem->ShowWindow (SW_HIDE); ;
	}

	pItem = GetDlgItem (IDC_STATIC_RESELLER_LABEL);
	if (pItem) {
		pItem->ShowWindow (SW_HIDE); ;
	}
#endif


#if 1
	{
		int i;
		for (i = 0; i < m_tcsSerialNo.GetLength (); i++) {
			m_aszSerialNo[i] = m_tcsSerialNo[i];
		}
		m_aszSerialNo[i] = 0;
	}

	char  myXmlBuffer[4096];
	memset (myXmlBuffer, 0, sizeof(myXmlBuffer));

	// Open the license file that may be in the Database folder and
	// then use MSXML XML parser to pull the information from the XML text.
	int  ibFileOpen = 0;
	TRY
	{
		CFile cfXmlFile (LICENSEKEYPATHNAME, CFile::modeRead);
		cfXmlFile.Read (myXmlBuffer, sizeof(myXmlBuffer)/sizeof(myXmlBuffer[0]));
		cfXmlFile.Close();
		ibFileOpen = 1;
	}
	CATCH (CFileException, pExp)
	{
	}
	END_CATCH

	if (ibFileOpen) {
		CMsxmlWrapper  m_Msxml;

		ibFileOpen = -1;
		m_Msxml.LoadText (myXmlBuffer);

		if (m_Msxml.SelectNode (L"//body")) {
			m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
			m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
			m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
			ibFileOpen = 1;
		} else if (m_Msxml.SelectNode (L"//licensepayload")) {
			m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
			m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
			m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
			ibFileOpen = 1;
		}

		if (ibFileOpen > 0) {
			// If we were successful in pulling some information from the XML then
			// lets update the UI with the information from the license file dropped.
			pItem = GetDlgItem (IDC_STATIC_SERIALNUMBER2);

			if (pItem) {
				pItem->SetWindowText(m_LicSerialNumberString);

				pItem = GetDlgItem (IDC_STATIC_RESELLER_ID2);
				if (pItem) {
					pItem->SetWindowText(m_LicResellerIdString);
				}

				pItem = GetDlgItem (IDC_STATIC_RESELLER_ID);
				if (pItem) {
					pItem->SetWindowText(m_LicResellerIdString);
				}
			}

			ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_LicKeyCodeString);

			CHAR    xValidateString [128];
			int      iLoop, i;

			memset (xValidateString, 0, sizeof(xValidateString));
			for (iLoop = 0, i = 0; iLoop < 64 && m_LicSerialNumberString[i]; iLoop++, i++) {
				xValidateString[iLoop] = m_LicSerialNumberString[i];
			}

			pItem = GetDlgItem (IDC_STATIC_VALIDATE_DROP);
			if (strncmp (xValidateString, m_ValidatedString, strlen(xValidateString)) == 0 &&
				strncmp (m_aszSerialNo, m_ValidatedString, strlen(m_aszSerialNo)) == 0) {
				pItem->SetWindowText (L"Valid license key");
			} else {
				_tcscpy (m_LicKeyCodeString, _T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
				pItem->SetWindowText (L"FAILED Validate license key");
			}
		}
	}

	if (ibFileOpen < 1) {
		pItem = GetDlgItem (IDC_STATIC_VALIDATE_DROP);
		pItem->SetWindowText (L"FAILED Validate license key");
	}


#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CdragdropDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CdragdropDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CdragdropDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

afx_msg BOOL CdragdropDlg::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	return TRUE;
}

int GetFile (HINTERNET hOpen, TCHAR * szURL, BYTE  szTemp[4096])   
{
    DWORD dwSize;
    TCHAR szHead[15];
    HINTERNET  hConnect;

    szHead[0] = '\0';
	szTemp[0] = 0;

    if ( !(hConnect = InternetOpenUrl( hOpen, szURL, szHead, 15, INTERNET_FLAG_DONT_CACHE, 0)))
    {
		DWORD  dwlasterror = GetLastError();
		if (dwlasterror == ERROR_INTERNET_NAME_NOT_RESOLVED) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_NAME_NOT_RESOLVED - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_TIMEOUT) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_TIMEOUT - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_SERVER_UNREACHABLE) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_SERVER_UNREACHABLE - check LAN connectivity."));
		} else if (dwlasterror == ERROR_INTERNET_OPERATION_CANCELLED) {
			AfxMessageBox (_T("Error: ERROR_INTERNET_OPERATION_CANCELLED - check LAN connectivity."));
		} else {
			CString msg;
			msg.Format (_T("Error: GetLastError() returned %d."), dwlasterror);
			AfxMessageBox (msg);
		}
        return -2;
    }

	if (InternetReadFile (hConnect, szTemp, 4096,  &dwSize) )
	{
		if (dwSize) {
			return dwSize;
		}
		return -3;
	}
	return -4;
}

int DownloadURLImage (TCHAR * szURL, BYTE  szTemp[4096])
{
	int result = -1;
    HINTERNET hInternet;

    hInternet= InternetOpen (_T("DeviceConfig"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);

	if (hInternet) {
		result = GetFile (hInternet, szURL, szTemp) ; 
		InternetCloseHandle(hInternet);
	}
    return result ;
}

BOOL CdragdropDlg::ReadAndProcessLicenseFile (TCHAR  tcsFileName[512])
{
	BYTE  myXmlBuffer[4096];

	memset (myXmlBuffer, 0, sizeof(myXmlBuffer));

	// Open the license file that was dropped and then use MSXML XML parser
	// to pull the information from the XML text.
	TRY
	{
		CFile cfXmlFile (tcsFileName, CFile::modeRead);
		cfXmlFile.Read (myXmlBuffer, sizeof(myXmlBuffer)/sizeof(myXmlBuffer[0]));
		cfXmlFile.Close();
	}
	CATCH (CFileException, pExp)
	{
	}
	END_CATCH

	ParseAndUpdateDialog (myXmlBuffer);

	return FALSE;
}

int CdragdropDlg::ParseAndUpdateDialog (BYTE  *szTemp)
{
	int  iRetStatus;

	// If we were successful in pulling some information from the XML then
	// lets update the UI with the information from the license file dropped.
	if ((iRetStatus = ParseXmlBuffer (szTemp)) >= 0) {
		CWnd *pItem = GetDlgItem (IDC_STATIC_VALIDATE_GEN);
		if (pItem) {
			pItem->SetWindowText(m_LicErrorString);
		}

		pItem = GetDlgItem (IDC_BUTTON_GENERATE);
		if (pItem) {
			if (iRetStatus < 100) {
				pItem->EnableWindow (TRUE);
			} else {
				pItem->EnableWindow (FALSE);
			}
		}

		pItem = GetDlgItem (IDC_STATIC_SERIALNUMBER2);
		if (pItem) {
			pItem->SetWindowText(m_LicSerialNumberString);
		}

		pItem = GetDlgItem (IDC_STATIC_VALIDATE_GEN);
		if (pItem) {
			pItem->SetWindowText(m_LicErrorString);
		}

		pItem = GetDlgItem (IDC_STATIC_RESELLER_ID2);
		if (pItem) {
			pItem->SetWindowText(m_LicResellerIdString);
		}

		CHAR    xValidateString [128];
		int      iLoop, i;

		memset (xValidateString, 0, sizeof(xValidateString));
		for (iLoop = 0, i = 0; iLoop < 64 && m_LicSerialNumberString[i]; iLoop++, i++) {
			xValidateString[iLoop] = m_LicSerialNumberString[i];
		}
//		for (i = 0; iLoop < 64 && m_LicResellerIdString[i]; iLoop++, i++) {
//			xValidateString[iLoop] = m_LicResellerIdString[i];
//		}

		ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_LicKeyCodeString);
		pItem = GetDlgItem (IDC_STATIC_VALIDATE_DROP);
		if (strncmp (xValidateString, m_ValidatedString, strlen(xValidateString)) == 0 &&
			strncmp (m_aszSerialNo, m_ValidatedString, strlen(m_aszSerialNo)) == 0) {
			pItem->SetWindowText (L"Valid license key");
			return TRUE;
		} else {
			_tcscpy (m_LicKeyCodeString, _T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
			pItem->SetWindowText (L"FAILED Validate license key");
		}
	}

	return iRetStatus;
}

int CdragdropDlg::ParseXmlBuffer (BYTE  *szTemp)
{
	CMsxmlWrapper  m_Msxml;
	m_Msxml.LoadText ((char *)szTemp);

	m_LicErrorString[0] = 0;

	if (m_Msxml.SelectNode (L"//body")) {
		int   iRetStatus = 1;
		BOOL  bError = FALSE;
		m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
		bError = m_Msxml.SelectNodeGettext(L"//licensepayload/error", m_LicErrorString, 120);
		if (bError) iRetStatus += 100;
		return iRetStatus;
	} else if (m_Msxml.SelectNode (L"//licensepayload")) {
		int   iRetStatus = 2;
		BOOL  bError = FALSE;
		m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
		bError = m_Msxml.SelectNodeGettext(L"//licensepayload/error", m_LicErrorString, 120);
		if (bError) iRetStatus += 100;
		return iRetStatus;
	}

	return -1;
}

afx_msg LRESULT  CdragdropDlg::OnDropString(WPARAM wParam, LPARAM lParam)
{
	char *pString = (char *)wParam;
	int    i;
	TCHAR  szURL[512];
	BYTE  szTemp[4096];

	memset (szTemp, 0, sizeof(szTemp));

	i = 0;
	do {
		szURL[i] = pString[i];
		i++;
	} while (pString[i] && i < 510);
	szURL[i] = 0;    // ensure zero terminated.

	CWnd *pItem = 0;

	if (DownloadURLImage (szURL, szTemp)) {
		int  iRetStatus;

		// If we were successful in pulling some information from the XML then
		// lets update the UI with the information from the license file dropped.
		if ((iRetStatus = ParseXmlBuffer (szTemp)) >= 0) {
			pItem = GetDlgItem (IDC_STATIC_VALIDATE_GEN);
			if (pItem) {
				pItem->SetWindowText(m_LicErrorString);
			}

			pItem = GetDlgItem (IDC_BUTTON_GENERATE);
			if (pItem) {
				if (iRetStatus < 100) {
					pItem->EnableWindow (TRUE);
				} else {
					pItem->EnableWindow (FALSE);
				}
			}

			pItem = GetDlgItem (IDC_STATIC_SERIALNUMBER2);
			if (pItem) {
				pItem->SetWindowText(m_LicSerialNumberString);
			}

			pItem = GetDlgItem (IDC_STATIC_RESELLER_ID2);
			if (pItem) {
				pItem->SetWindowText(m_LicResellerIdString);
			}

			CHAR    xValidateString [128];
			int      iLoop, i;

			memset (xValidateString, 0, sizeof(xValidateString));
			for (iLoop = 0, i = 0; iLoop < 64 && m_LicSerialNumberString[i]; iLoop++, i++) {
				xValidateString[iLoop] = m_LicSerialNumberString[i];
			}
	//		for (i = 0; iLoop < 64 && m_LicResellerIdString[i]; iLoop++, i++) {
	//			xValidateString[iLoop] = m_LicResellerIdString[i];
	//		}

			ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_LicKeyCodeString);
			CWnd *pItem = GetDlgItem (IDC_STATIC_VALIDATE_DROP);
			if (strncmp (xValidateString, m_ValidatedString, strlen(xValidateString)) == 0 &&
				strncmp (m_aszSerialNo, m_ValidatedString, strlen(m_aszSerialNo)) == 0) {
				pItem->SetWindowText (L"Valid license key");
				return TRUE;
			}
		}
	}

	_tcscpy (m_LicKeyCodeString, _T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
	pItem = GetDlgItem (IDC_STATIC_VALIDATE_DROP);
	pItem->SetWindowText (L"FAILED Validate license key");

	return 1;
}

afx_msg void CdragdropDlg::OnDropFiles(HDROP hDropInfo)
{
	TCHAR  tcsFileName[512];
	int    iLen=255;
	int    iLoop = 0;

			// Send the file drop notification to the dialog to be processed.
			// We use SendMessage() so that we process and then unlock it.
			UINT   uInt, uInt2 = 1;

			// discover number of files being dropped.
			uInt = DragQueryFile (hDropInfo, 0xffffffff, tcsFileName, iLen);
			// now loop through the files processing each one.
			for (iLoop = 0; iLoop < uInt && uInt2 > 0; iLoop++) {
				uInt2 = DragQueryFile (hDropInfo, iLoop, tcsFileName, iLen);
				DWORD dwFileAttrib = GetFileAttributes (tcsFileName);
				if (dwFileAttrib & FILE_ATTRIBUTE_DIRECTORY) {
					WIN32_FIND_DATA search_data;
					uInt2++;
					TCHAR *ptcsFileNameEnd = tcsFileName + _tcslen(tcsFileName);

					_tcscat (ptcsFileNameEnd, _T("\\*.*"));
					memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
					HANDLE handle = FindFirstFile(tcsFileName, &search_data);

					while(handle != INVALID_HANDLE_VALUE)
					{
						if ((search_data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) == 0) {
							*(ptcsFileNameEnd + 1) = 0;
							_tcscat (ptcsFileNameEnd, search_data.cFileName);
							if (ReadAndProcessLicenseFile (tcsFileName)) {
								uInt2 = 0;
								break;
							}
						}
						if(FindNextFile(handle, &search_data) == FALSE)
							break;
					}
				} else if ((dwFileAttrib & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) == 0) {
					if (ReadAndProcessLicenseFile (tcsFileName))
						break;
				}
			}

	return;
}

void CdragdropDlg::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
}

#if 1
BOOL CdragdropDlg::ValidateKeyCode (char *sKeyHash, wchar_t *sKeyCode)
{
	int      i, j;
	UCHAR    xBuffer[128];

	CCrypto  myCrypto;

	memset (xBuffer, 0, sizeof(xBuffer));
	for (i = 0, j = 0; sKeyCode[i]; j++) {
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] = (sKeyCode[i] - '0') & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] = (sKeyCode[i] - 'A' + 10) & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		}
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] |= (sKeyCode[i] - '0') & 0x0f;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] |= (sKeyCode[i] - 'A' + 10) & 0x0f;
			i++;
		}
	}

	char yBuffer[120];

	myCrypto.RflDecryptSerialNoString((UCHAR *)yBuffer, xBuffer);
	for (i = 0; yBuffer[i]; i++) {
		m_ValidatedString[i] = yBuffer[i];
	}
	m_ValidatedString[i] = 0;

	return TRUE;
}


BOOL CdragdropDlg::GenerateKeyCode (char *sKeyCode)
{
	// transform array used to transform nibble values to hex digits.
	char TransformArray[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	int      iLoop, i, j;
	UCHAR    xBuffer[128];
	UCHAR    xEncryptSerial[48];

	CCrypto  myCrypto;

	// Create a string composed of the terminal serial number and the reseller id
	// so that we can then do the encryt in order to generate a unique key code
	// based on this terminal's serial number and this reseller id.
	memset (m_GeneratedString, 0, sizeof(m_GeneratedString));
	memset (xBuffer, 0, sizeof(xBuffer));
	for (iLoop = 0, i = 0; iLoop < 64 && m_tcsSerialNo[i]; iLoop++, i++) {
		m_GeneratedString[iLoop] = xBuffer[iLoop] = m_tcsSerialNo[i];
	}

	xBuffer[iLoop] = 0;
	myCrypto.RflEncryptSerialNoString(xBuffer, xEncryptSerial);

	// now we generate the actual key code which is composed of the hex digits of the
	// encrypted serial number, reseller id string.  we do this transformation so that
	// we can deal with the key code as a test string.
	for (i = 0, j = 0; j < 48; j++) {
		m_GeneratedKeyCode[i++] = TransformArray[((xEncryptSerial[j] >> 4) & 0x0f)];
		m_GeneratedKeyCode[i++] = TransformArray[(xEncryptSerial[j] & 0x0f)];
	}
	m_GeneratedKeyCode[i] = 0;  // end of string terminator
	for (i = 0; i < 48 * 2; i++) {
		m_KeyCodeString[i] = m_GeneratedKeyCode[i];
	}
	m_KeyCodeString[i] = 0;

#if defined(DEBUG_LOG_ENCRYPT)
	// next we will generate a log file so that we can compare the encrypted string
	// and can also log a decrypt of the string to check that we get out what we put in.
	char xBuff[256];
	sprintf (xBuff, "-%S- -%S- -%s-\r\n", m_tcsSerialNo, m_ResellerIdString, m_GeneratedKeyCode);
	CFile cfXmlFile (L"licenselist.txt", CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate);
	cfXmlFile.SeekToEnd ();
	cfXmlFile.Write (xBuff, strlen(xBuff));

	memset (xBuffer, 0, sizeof(xBuffer));
	for (i = 0, j = 0; m_GeneratedKeyCode[i]; j++) {
		if (m_GeneratedKeyCode[i] >= '0' && m_GeneratedKeyCode[i] <= '9') {
			xBuffer[j] = (m_GeneratedKeyCode[i] - '0') & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		} else if (m_GeneratedKeyCode[i] >= 'A' && m_GeneratedKeyCode[i] <= 'F') {
			xBuffer[j] = (m_GeneratedKeyCode[i] - 'A' + 10) & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		}
		if (m_GeneratedKeyCode[i] >= '0' && m_GeneratedKeyCode[i] <= '9') {
			xBuffer[j] |= (m_GeneratedKeyCode[i] - '0') & 0x0f;
			i++;
		} else if (m_GeneratedKeyCode[i] >= 'A' && m_GeneratedKeyCode[i] <= 'F') {
			xBuffer[j] |= (m_GeneratedKeyCode[i] - 'A' + 10) & 0x0f;
			i++;
		}
	}

	char yBuffer[64];

	myCrypto.RflDecryptSerialNoString((UCHAR *)yBuffer, xBuffer);

	sprintf (xBuff, "-%s-\r\n", yBuffer);
	cfXmlFile.Write (xBuff, strlen(xBuff));
	cfXmlFile.Close();
#endif

	return TRUE;
}
#else
BOOL CdragdropDlg::ValidateKeyCode (char *sKeyHash, wchar_t *sKeyCode)
{
	int      i, j;
	UCHAR    xBuffer[128];

	// init the cryptographic hash with the provided hash key code.
	// if we reuse this hash each time then we will get the same crypto results.
	CCrypto  myCrypto;
	myCrypto.RflConstructEncryptByteStringClass(sKeyHash);

	memset (xBuffer, 0, sizeof(xBuffer));
	for (i = 0, j = 0; sKeyCode[i]; j++) {
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] = (sKeyCode[i] - '0') & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] = (sKeyCode[i] - 'A' + 10) & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		}
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] |= (sKeyCode[i] - '0') & 0x0f;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] |= (sKeyCode[i] - 'A' + 10) & 0x0f;
			i++;
		}
	}
	myCrypto.RflDecryptByteString (xBuffer, j, 128);

	for (i = 0; i < j; i++) {
		m_ValidatedString[i] = xBuffer[i];
	}
	return TRUE;
}


BOOL CdragdropDlg::GenerateKeyCode (char *sKeyCode)
{
	// transform array used to transform nibble values to hex digits.
	char TransformArray[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	int      iLoop, i, j;
	UCHAR    xBuffer[128];

	// init the cryptographic hash with the provided hash key code.
	// if we reuse this hash each time then we will get the same crypto results.
	CCrypto  myCrypto;
	myCrypto.RflConstructEncryptByteStringClass(sKeyCode);

	// Create a string composed of the terminal serial number and the reseller id
	// so that we can then do the encryt in order to generate a unique key code
	// based on this terminal's serial number and this reseller id.
	memset (m_GeneratedString, 0, sizeof(m_GeneratedString));
	memset (xBuffer, 0, sizeof(xBuffer));
	for (iLoop = 0, i = 0; iLoop < 64 && m_tcsSerialNo[i]; iLoop++, i++) {
		m_GeneratedString[iLoop] = xBuffer[iLoop] = m_tcsSerialNo[i];
	}
	for (i = 0; iLoop < 64 && m_ResellerIdString[i]; iLoop++, i++) {
		m_GeneratedString[iLoop] = xBuffer[iLoop] = m_ResellerIdString[i];
	}

	// encrypt the string composed of the serial number and the reseller id.
	// this function indicates the actual number of bytes in the encrypted string
	// by putting the count in the last byte of the array.
	myCrypto.RflEncryptByteString (xBuffer, iLoop, 128);
	iLoop = xBuffer[127];

	// now we generate the actual key code which is composed of the hex digits of the
	// encrypted serial number, reseller id string.  we do this transformation so that
	// we can deal with the key code as a test string.
	for (i = 0, j = 0; j < iLoop; j++) {
		m_GeneratedKeyCode[i++] = TransformArray[((xBuffer[j] >> 4) & 0x0f)];
		m_GeneratedKeyCode[i++] = TransformArray[(xBuffer[j] & 0x0f)];
	}
	m_GeneratedKeyCode[i] = 0;  // end of string terminator

	for (i = 0; i < 120; i++) {
		m_KeyCodeString[i] = m_GeneratedKeyCode[i];
	}

#if defined(DEBUG_LOG_ENCRYPT)
	// next we will generate a log file so that we can compare the encrypted string
	// and can also log a decrypt of the string to check that we get out what we put in.
	char xBuff[256];
	sprintf (xBuff, "-%S- -%S- -%s-\r\n", m_tcsSerialNo, m_ResellerIdString, m_GeneratedKeyCode);
	CFile cfXmlFile (L"licenselist.txt", CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate);
	cfXmlFile.SeekToEnd ();
	cfXmlFile.Write (xBuff, strlen(xBuff));

	memset (xBuffer, 0, sizeof(xBuffer));
	for (i = 0, j = 0; m_GeneratedKeyCode[i]; j++) {
		if (m_GeneratedKeyCode[i] >= '0' && m_GeneratedKeyCode[i] <= '9') {
			xBuffer[j] = (m_GeneratedKeyCode[i] - '0') & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		} else if (m_GeneratedKeyCode[i] >= 'A' && m_GeneratedKeyCode[i] <= 'F') {
			xBuffer[j] = (m_GeneratedKeyCode[i] - 'A' + 10) & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		}
		if (m_GeneratedKeyCode[i] >= '0' && m_GeneratedKeyCode[i] <= '9') {
			xBuffer[j] |= (m_GeneratedKeyCode[i] - '0') & 0x0f;
			i++;
		} else if (m_GeneratedKeyCode[i] >= 'A' && m_GeneratedKeyCode[i] <= 'F') {
			xBuffer[j] |= (m_GeneratedKeyCode[i] - 'A' + 10) & 0x0f;
			i++;
		}
	}
	myCrypto.RflDecryptByteString (xBuffer, j, 128);

	sprintf (xBuff, "-%s-\r\n", xBuffer);
	cfXmlFile.Write (xBuff, strlen(xBuff));
	cfXmlFile.Close();
#endif

	return TRUE;
}
#endif

// This function is triggered by the user pressing the "Set License" button.
//
// The purpose of this function is to take the data used to generate a license key,
// generate a license key, and then create the license key file.
void CdragdropDlg::OnBnClickedButtonGenerate()
{
	int iLoop;

	memset (m_GeneratedString, 0, sizeof(m_GeneratedString));
	for (iLoop = 0; iLoop < 64 && m_tcsSerialNo[iLoop]; iLoop++) {
		m_GeneratedString[iLoop] = m_tcsSerialNo[iLoop];
	}
	CWnd *pItem = GetDlgItem (IDC_STATIC_VALIDATE_GEN);
#if 0
//	for (i = 0; iLoop < 64 && m_ResellerIdString[i]; iLoop++, i++) {
//		m_GeneratedString[iLoop] = m_ResellerIdString[i];
//	}

	int  iLength = 0;
	char xmlBuffer[4096];

	GenerateKeyCode (CRYPTO_KEYCODE_HASH_VALUE);

	// Generate the XML text used for the license file.
	iLength = sprintf (xmlBuffer, "<licensepayload>\r\n");
	iLength += sprintf (xmlBuffer+iLength, "\t<serialnumber>%S</serialnumber>\r\n", m_tcsSerialNo);
	iLength += sprintf (xmlBuffer+iLength, "\t<resellerid>%S</resellerid>\r\n", m_ResellerIdString);
	iLength += sprintf (xmlBuffer+iLength, "\t<keycode>%S</keycode>\r\n", m_KeyCodeString);
	iLength += sprintf (xmlBuffer+iLength, "</licensepayload>");

	TRY
	{
		CFile cfXmlFile (_T("licensekeyGen.txt"), CFile::modeCreate | CFile::modeReadWrite);
		cfXmlFile.Write (xmlBuffer, iLength);
		cfXmlFile.Close();
	}
	CATCH (CFileException, pExp)
	{
	}
	END_CATCH

	ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_KeyCodeString);
	if (strncmp (m_GeneratedString, m_ValidatedString, strlen(m_GeneratedString)) == 0) {
		pItem->SetWindowText (L"Valid license key");
	} else {
		_tcscpy (m_LicKeyCodeString, _T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
		pItem->SetWindowText (L"FAILED Validate license key");
	}
#else

	ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_LicKeyCodeString);

	if (strncmp (m_GeneratedString, m_ValidatedString, strlen(m_GeneratedString)) == 0) {
		pItem->SetWindowText (L"Valid license key");
	} else {
		_tcscpy (m_LicSerialNumberString, m_tcsSerialNo);
		_tcscpy (m_LicKeyCodeString, _T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
		pItem->SetWindowText (L"FAILED Validate license key");
	}

	int  iLength = 0;
	char xmlBuffer[4096];

	// Generate the XML text used for the license file.
	iLength = sprintf (xmlBuffer, "<licensepayload>\r\n");
	iLength += sprintf (xmlBuffer+iLength, "\t<serialnumber>%S</serialnumber>\r\n", m_LicSerialNumberString);
	iLength += sprintf (xmlBuffer+iLength, "\t<resellerid>%S</resellerid>\r\n", m_LicResellerIdString);
	iLength += sprintf (xmlBuffer+iLength, "\t<keycode>%S</keycode>\r\n", m_LicKeyCodeString);
	iLength += sprintf (xmlBuffer+iLength, "</licensepayload>");

	TRY
	{
		CFile cfXmlFile (LICENSEKEYPATHNAME, CFile::modeCreate | CFile::modeReadWrite);
		cfXmlFile.Write (xmlBuffer, iLength);
		cfXmlFile.Close();
	}
	CATCH (CFileException, pExp)
	{
	}
	END_CATCH
#endif
}

// This function is triggered by the user pressing the "Set Reseller Id" button.
// What this function does is to take the reseller id entered into the edit box
// and saves it in our dialog data in order to generate a license key file.
//
// We perform the following steps:
//   - take the data from the edit box
//   - save it in our dialog data
//   - clear the edit box
//   - copy the entered reseller id into the reseller display text area
void CdragdropDlg::OnBnClickedButtonSetResellerId()
{
	CWnd *pItem = GetDlgItem (IDC_EDIT_NEW_RESELLER_ID);
	if (pItem) {
		pItem->GetWindowText (m_ResellerIdString, sizeof(m_ResellerIdString)/sizeof(m_ResellerIdString[0]));
	}
	pItem->SetWindowText (L"");

	pItem = GetDlgItem (IDC_STATIC_RESELLER_ID);
	if (pItem) {
		pItem->SetWindowText (m_ResellerIdString);
	}
}
void CdragdropDlg::OnBnClickedButtonSetSerialNo()
{
	CWnd *pItem = GetDlgItem (IDC_EDIT_NEW_RESELLER_ID);
	if (pItem) {
		pItem->GetWindowText (m_SerialNumberString, sizeof(m_SerialNumberString)/sizeof(m_SerialNumberString[0]));
	}
	pItem->SetWindowText (L"");

	pItem = GetDlgItem (IDC_STATIC_SERIALNUMBER1);
	if (pItem) {
		pItem->SetWindowText (m_SerialNumberString);
		m_tcsSerialNo = m_SerialNumberString;
		for (int i = 0; i < sizeof(m_aszSerialNo)/sizeof(m_aszSerialNo[0]); i++) {
			m_aszSerialNo[i] = m_SerialNumberString[i];
		}
	}
}


void CdragdropDlg::OnLButtonDown(UINT wParam, CPoint myPoint)
{
	CWnd *pCwndFromPoint = ChildWindowFromPoint(myPoint);

	if (pCwndFromPoint) {
		CMySourceData  MySource;
		CWnd *pCwndSerial, *pCwndFile;
	
		pCwndSerial = GetDlgItem (IDC_STATIC_SERIALNUMBER1);
		pCwndFile = GetDlgItem (IDC_STATIC_NAME);
		if (pCwndSerial == pCwndFromPoint) {
			TCHAR  tcsText[65];
			pCwndSerial->GetWindowTextW(tcsText, 64);
			MySource.StartDragText(tcsText);
		} else if (pCwndFile == pCwndFromPoint) {
			TCHAR  tcsText[1024];
			pCwndFile->GetWindowTextW(tcsText, 64);
			GetCurrentDirectory (750, tcsText);
			_tcscat (tcsText, _T("\\"));
			_tcscat (tcsText, LICENSEKEYFILENAME);
			MySource.StartDragFile(tcsText);
		}
	}
}

void CdragdropDlg::HandlePinPadEntry (const TCHAR  tcsChar)
{
	CEdit   *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_NEW_RESELLER_ID);
	if (pclEditBox) {
		CString  myText;
		TCHAR mytcsCharStr[2] = {0};

		mytcsCharStr[0] = tcsChar;
		pclEditBox->GetWindowText (myText);
		myText.Append (mytcsCharStr);
		pclEditBox->SetWindowText (myText);
	}
}

void CdragdropDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('9'));
}
void CdragdropDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('8'));
}
void CdragdropDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('7'));
}
void CdragdropDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('6'));
}
void CdragdropDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('5'));
}
void CdragdropDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('4'));
}
void CdragdropDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('3'));
}
void CdragdropDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('2'));
}
void CdragdropDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('1'));
}
void CdragdropDlg::OnBnClickedButton0()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('0'));
}
void CdragdropDlg::OnBnClickedButtonDash()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('-'));
}

void CdragdropDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	CEdit   *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_NEW_RESELLER_ID);
	if (pclEditBox) {
		pclEditBox->SetWindowText(_T(""));
	}
}

void CdragdropDlg::OnBnClickedButtonFetch()
{
	// TODO: Add your control notification handler code here

	BYTE  buffer[4096];
	CWnd   *pclSerial = GetDlgItem (IDC_STATIC_SERIALNUMBER1);
	CWnd   *pclReseller = GetDlgItem (IDC_STATIC_RESELLER_ID);
	if (pclReseller && pclSerial) {
		int  iRetStatus;
		CString  csResellerId;
		pclReseller->GetWindowText (csResellerId);
		CString  csSerialNumber;
		pclSerial->GetWindowText (csSerialNumber);

		CString szUrl;
		CString szUrlFmt = m_tcsUrl + _T("index.php?resellerid=%s&serialnumber=%s");
		szUrl.Format (szUrlFmt, csResellerId, csSerialNumber);
		if ((iRetStatus = DownloadURLImage (szUrl.GetBuffer (), buffer)) < 0) {
			CString myErrorMsg;

			myErrorMsg.Format (_T("Error downloading license key file %s %s error %d"), csResellerId, csSerialNumber, iRetStatus);
			AfxMessageBox (myErrorMsg);
		} else {
			buffer[iRetStatus] = 0;
			if (ParseAndUpdateDialog (buffer) >= 0) {
			}
		}
	}

}

void CdragdropDlg::OnStnClickedStaticResellerId()
{
	// TODO: Add your control notification handler code here
}
