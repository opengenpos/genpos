#include "stdafx.h"
#include "RPageBase.h"
#include "PCSample.h"
#include "ConnEngine.h"

extern CPCSampleApp theApp;

int RPageBase::m_fLogOutgoingMsgs = 0;
RPageBase::RPageBase() {}
RPageBase::RPageBase(UINT nIDTemplate) : CPropertyPage(nIDTemplate) {}
RPageBase::~RPageBase(void) {}

void RPageBase::DisplayText(LPCTSTR pText, bool fIndent)
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

		theApp.DisplayText(tmpString);
		pSplit = wcstok(NULL, _T("\r"));
	}
	return;
}

void RPageBase::SendCommand(CString &commandString)
{
	// CR-LF = OD 0A
	int len = commandString.GetLength();
	if (len > 0)
	{
		_logOutgoingMsgs(commandString);

		::fnConnEngineSendMessage(0, commandString, len);
		// Remove line-feeds
		commandString.Trim();
		commandString.Remove('\n');
		this->DisplayText(_T("\rConnectEngine: Sending Message  --------------"));
		this->DisplayText((LPCTSTR)commandString, true);
	}
	else
		::MessageBeep(0);
	return;
}

void RPageBase::_logOutgoingMsgs(CString &cmdMsg)
{
	if (m_fLogOutgoingMsgs != 0)
	{
		HANDLE  hFile;
		hFile = CreateFile (_T("ConnectEngine_Send.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			ULONG     ulWriteSize;
			ULONG     ulActualBytes;
			LPCTSTR   pBuffer;
			CTime     myTime = CTime::GetCurrentTime ();

			CString   myDateString = myTime.Format (_T("\n SENT %A, %B %d, %Y at %X\n"));

			SetFilePointer (hFile, 0, NULL, FILE_END);

			pBuffer = myDateString;
			ulWriteSize = myDateString.GetLength() * sizeof(TCHAR);
			WriteFile (hFile, pBuffer, ulWriteSize, &ulActualBytes, NULL);

			pBuffer = cmdMsg;
			ulWriteSize = cmdMsg.GetLength() * sizeof(TCHAR);
			WriteFile (hFile, pBuffer, ulWriteSize, &ulActualBytes, NULL);
			CloseHandle (hFile);
		}
	}
	return;
}

BOOL RPageBase::OnInitDialog()
{
	return CPropertyPage::OnInitDialog();
}
