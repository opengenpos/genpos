// RPageConnect.cpp : implementation file
//

#include "stdafx.h"
#include "PCSample.h"
#include "RPageBase.h"
#include "RPageConnect.h"
#include "ConnEngine.h"

extern CPCSampleApp theApp;

#define WM_APP_CONNENGINE_MSG_RCVD    (WM_APP+1)
#define WM_APP_CONNENGINE_CLOSE_MSG   (WM_APP_CONNENGINE_MSG_RCVD+1)

IMPLEMENT_DYNAMIC(RPageConnect, RPageBase)

RPageConnect::RPageConnect() : RPageBase(RPageConnect::IDD)
, m_fLogOutgoingMsgs(0)
, m_fLogIncomingMsgs(0)
, m_incomingMsgs(_T(""))
{
	m_ipPort = 8282;
	m_commandText = _T(""); // _T("<EMPLOYEECHANGE>\r\n<tcsActionType>Employee-Data</tcsActionType>\r\n</EMPLOYEECHANGE>");
}

RPageConnect::~RPageConnect() {}

void RPageConnect::DoDataExchange(CDataExchange* pDX)
{
	RPageBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CE_CONNECT, m_btnConnect);
	DDX_CBString(pDX, IDC_CE_IPADDRESS, m_ipAddress);
	DDX_Text(pDX, IDC_CE_IPPORT, m_ipPort);
	DDV_MinMaxInt(pDX, m_ipPort, 0, 65535);
	DDX_Text(pDX, IDC_CE_COMMAND, m_commandText);
	DDX_Control(pDX, IDC_CE_SEND, m_btnSend);
	DDX_Control(pDX, IDC_CE_COMMAND, m_editCommand);
	DDX_Control(pDX, IDC_CE_IPADDRESS, m_cIpAddress);
	DDX_Check(pDX, IDC_CE_LOGMSGOUT, m_fLogOutgoingMsgs);
	DDX_Check(pDX, IDC_CE_LOGMSGIN, m_fLogIncomingMsgs);
	DDX_Text(pDX, IDC_CE_NOTIFICATION, m_incomingMsgs);
	DDX_Control(pDX, IDC_CE_NOTIFICATION, m_cIncomingMsgs);
	DDX_Control(pDX, IDC_DISCONNECT, m_btnDisconnect);
}

BEGIN_MESSAGE_MAP(RPageConnect, RPageBase)
	ON_BN_CLICKED(IDC_CE_CONNECT, &RPageConnect::OnBtnClickedConnect)
	ON_MESSAGE(WM_APP_CONNENGINE_MSG_RCVD, OnReceiveMessage)
	ON_MESSAGE(WM_APP_CONNENGINE_CLOSE_MSG, OnCloseMessage)
	ON_BN_CLICKED(IDC_CE_SEND, &RPageConnect::OnBnClickedCesend)
	ON_BN_CLICKED(IDC_CE_CLEAR, &RPageConnect::OnBnClickedCeclear)
	ON_BN_CLICKED(IDC_CE_LOGMSGOUT, &RPageConnect::OnBnClickedCeLogMsgOut)
	ON_BN_CLICKED(IDC_CE_CLEARINCOMING, &RPageConnect::OnBnClickedCeClearincoming)
	ON_BN_CLICKED(IDC_DISCONNECT, &RPageConnect::OnBnClickedDisconnect)
END_MESSAGE_MAP()

BOOL RPageConnect::OnInitDialog()
{
	RPageBase::OnInitDialog();
	_initData();
	return TRUE;
}

void RPageConnect::OnBtnClickedConnect()
{
	this->UpdateData(TRUE);

	if (m_ipAddress.Trim().IsEmpty() == true)
	{
		::MessageBeep(0);
		return;
	}

	int idx = m_cIpAddress.FindStringExact(0, m_ipAddress);
	if (idx == -1)
		this->_saveIpAddress(m_ipAddress);

	TCHAR *tcsAddress = LPTSTR(m_ipAddress.GetBuffer());
	int rc = fnConnEngineConnectToExt (tcsAddress, m_ipPort, this->m_hWnd, WM_APP_CONNENGINE_MSG_RCVD, WM_APP_CONNENGINE_CLOSE_MSG);
	CString resText;
	resText.Format(_T("::fnConnEngineConnectTo returned %d"), rc);
	this->DisplayText(resText);
	// returned rc is always 0
	this->m_btnConnect.EnableWindow(FALSE);
	this->m_btnSend.EnableWindow(TRUE);
	this->m_btnDisconnect.EnableWindow(TRUE);
	m_ipAddress.ReleaseBuffer();
}

LRESULT RPageConnect::OnReceiveMessage(WPARAM wParam, LPARAM lParam)
{
	_logIncomingMsgs(wParam);
	//this->DisplayText(_T("\rConnectEngine: Message Received --------------"));
	//this->DisplayText((LPCTSTR)wParam, true);

	this->_displayIncomingMessage(_T("\rConnectEngine: Message Received --------------"));
	this->_displayIncomingMessage((LPCTSTR)wParam, true);

	return 0;
}

LRESULT RPageConnect::OnCloseMessage(WPARAM wParam, LPARAM lParam)
{
	_logIncomingMsgs(wParam);
	//this->DisplayText(_T("\rConnectEngine: Message Received --------------"));
	//this->DisplayText((LPCTSTR)wParam, true);

	this->_displayIncomingMessage(_T("\rConnectEngine: CLOSE SOCKET Received --------------"));

	return 0;
}

void RPageConnect::_displayIncomingMessage(LPCTSTR pText, bool fIndent)
{
	LPCTSTR pSplit;
	CString tmpString;

	pSplit = wcstok ((TCHAR*)pText, _T("\r"));
	while (pSplit != NULL)
	{
		tmpString = pSplit;
		tmpString.Trim();
		if (fIndent == true)
			tmpString.Insert(0, _T("\t"));

		tmpString.Replace(_T("%"), _T("*"));
		theApp.DisplayText(tmpString);
		tmpString.Append(_T("\r\n"));
		this->m_cIncomingMsgs.ReplaceSel(tmpString);
		pSplit = wcstok(NULL, _T("\r"));
	}
	UpdateData(TRUE);
	return;
}

void RPageConnect::_logIncomingMsgs(WPARAM wParam)
{
	UpdateData(TRUE);
	if (this->m_fLogIncomingMsgs != 0)
	{
		HANDLE  hFile;
		hFile = CreateFile (_T("ConnectEngine_Receive.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			ULONG     ulWriteSize;
			ULONG     ulActualBytes;
			LPCTSTR   pBuffer;

			CTime     myTime = CTime::GetCurrentTime ();

			CString   myDateString = myTime.Format (_T("\n RECEIVED %A, %B %d, %Y at %X\n"));

			SetFilePointer (hFile, 0, NULL, FILE_END);

			pBuffer = myDateString;
			ulWriteSize = myDateString.GetLength() * sizeof(TCHAR);
			WriteFile (hFile, pBuffer, ulWriteSize, &ulActualBytes, NULL);

			pBuffer = (TCHAR *)wParam;
			ulWriteSize = _tcslen((TCHAR *)wParam) * sizeof(TCHAR);
			WriteFile (hFile, pBuffer, ulWriteSize, &ulActualBytes, NULL);
			CloseHandle (hFile);
		}
	}
	return;
}

void RPageConnect::OnBnClickedCesend()
{
	this->UpdateData(TRUE);

	CString resText;
	ULONG  ulStatus = fnConnEngineRetrieveStatus (0);
	resText.Format(_T("::fnConnEngineRetrieveStatus returned 0x%8.8x"), ulStatus);
	this->DisplayText(resText);

	this->SendCommand(this->m_commandText);
}

void RPageConnect::OnBnClickedCeclear()
{
	this->UpdateData(TRUE);
	this->m_commandText = "";
	this->UpdateData(FALSE);
	this->m_editCommand.SetFocus();
}

void RPageConnect::_initData()
{
	CString strKey = "SOFTWARE\\NCR\\GenPOS\\PCSample\\ConnectEngine";
	CRegKey cRegKey;
	if (cRegKey.Open(HKEY_CURRENT_USER, strKey, KEY_READ)==ERROR_SUCCESS) 
	{
		const int MAX_VALUE_NAME = 100;
		DWORD	idx = 0;
		LONG	rc;
		TCHAR	achValue[MAX_VALUE_NAME]; 
		DWORD	cchValue = MAX_VALUE_NAME; 
        do 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0'; 
			rc = ::RegEnumValue(cRegKey.m_hKey, idx, achValue, &cchValue, NULL, NULL, NULL, NULL);
            if (rc == ERROR_SUCCESS ) 
				m_cIpAddress.AddString(achValue);
			idx++;
		} while (rc == ERROR_SUCCESS);

	}
}

void RPageConnect::_saveIpAddress(CString &newIpAddress)
{
	this->m_cIpAddress.AddString(newIpAddress);
	CString strKey = "SOFTWARE\\NCR\\GenPOS\\PCSample\\ConnectEngine";
	CRegKey cRegKey;
	LONG res = cRegKey.Open(HKEY_CURRENT_USER, strKey);
	if (res != ERROR_SUCCESS) 
		res = cRegKey.Create(HKEY_CURRENT_USER, strKey);

	if (res == ERROR_SUCCESS)
	{
		cRegKey.SetDWORDValue(newIpAddress, m_ipPort);
	}
}

void RPageConnect::OnBnClickedCeLogMsgOut()
{
	UpdateData(TRUE);
	RPageBase::m_fLogOutgoingMsgs = this->m_fLogOutgoingMsgs;
}

void RPageConnect::OnBnClickedCeClearincoming()
{
	UpdateData(TRUE);
	this->m_incomingMsgs = "";
	this->UpdateData(FALSE);
	this->m_editCommand.SetFocus();
}

void RPageConnect::OnBnClickedDisconnect()
{
	// TODO: Add your control notification handler code here
	int rc = fnConnEngineDisconnect ();
	CString resText;
	resText.Format(_T("::fnConnEngineDisconnect returned %d"), rc);
	this->DisplayText(resText);
	// returned rc is always 0
	this->m_btnConnect.EnableWindow(TRUE);
	this->m_btnSend.EnableWindow(FALSE);
	this->m_btnDisconnect.EnableWindow(FALSE);
	m_ipAddress.ReleaseBuffer();
}
