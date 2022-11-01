// SECControl.cpp : implementation file
//

#include "stdafx.h"
#include "PEPControl.h"
#include "SECControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SECControl dialog


SECControl::SECControl(CWnd* pParent /*=NULL*/)
	: CDialog(SECControl::IDD, pParent)
{
	//{{AFX_DATA_INIT(SECControl)
	m_status = _T("");
	m_serialNum = _T("");
	m_daysLeft = _T("");
	m_unlockCode = _T("");
	//}}AFX_DATA_INIT
}


void SECControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SECControl)
	DDX_Control(pDX, IDC_REPAIR, m_repair);
	DDX_Control(pDX, IDC_UNLOCK, m_unlockButton);
	DDX_Control(pDX, IDC_SOFTLOCX6CTRL1, m_softlocx);
	DDX_Text(pDX, IDC_STATUS, m_status);
	DDX_Text(pDX, IDC_SERIAL_NUM, m_serialNum);
	DDX_Text(pDX, IDC_DAYSLEFT, m_daysLeft);
	DDX_Text(pDX, IDC_UNLOCK_CODE, m_unlockCode);
	DDV_MaxChars(pDX, m_unlockCode, 56);
	DDX_Control(pDX, IDC_LOCKIT1, m_vbolock);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SECControl, CDialog)
	//{{AFX_MSG_MAP(SECControl)
	ON_BN_CLICKED(IDC_UNLOCK, OnUnlock)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_REPAIR, OnRepair)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SECControl message handlers

BEGIN_EVENTSINK_MAP(SECControl, CDialog)
    //{{AFX_EVENTSINK_MAP(SECControl)
	ON_EVENT(SECControl, IDC_SOFTLOCX6CTRL1, 1 /* DesktopStatus */, OnDesktopStatusSoftlocx6ctrl1, VTS_PBSTR VTS_I4 VTS_PBSTR VTS_PBSTR)
	ON_EVENT(SECControl, IDC_SOFTLOCX6CTRL1, 2 /* Registerstatus */, OnRegisterstatusSoftlocx6ctrl1, VTS_PBSTR VTS_I4)
	ON_EVENT(SECControl, IDC_LOCKIT1, 7 /* OnRegistered */, VBOLockOnRegistered, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 11 /* OnExpired */, VBOLockOnExpired, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 14 /* OnBackDate */, VBOLockOnBackDate, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 16 /* OnVerified */, VBOLockOnVerified, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 12 /* OnMissing */, VBOLockOnMissing, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 8 /* OnMaxtimes */, VBOLockOnMaxtimes, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 13 /* OnRunning */, OnRunning, VTS_NONE)
	ON_EVENT(SECControl, IDC_LOCKIT1, 9 /* OnIllegal */, OnIllegal, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void SECControl::OnDesktopStatusSoftlocx6ctrl1(BSTR FAR* Status, long Level, BSTR FAR* Serial, BSTR FAR* ExpireVal) 
{
	//if the application has been licensed then no need for the unlock display
	//end the dialog with a good status
	if(_tcscmp(*Status, _T("Licensed")) == 0)
	{ 
#ifdef SOFTLOCX_EXPIRE_ALWAYS
			::RegDeleteKey(HKEY_CLASSES_ROOT, m_softlocx.GetLicenseName());
#endif
		EndDialog(2);
	}


	CWnd* pStatusWnd = GetDlgItem(IDC_STATUS);
	CWnd* pSerialWnd = GetDlgItem(IDC_SERIAL_NUM);
	CWnd* pDaysLeftWnd = GetDlgItem(IDC_DAYSLEFT);

	pStatusWnd->SetWindowText(*Status);
	m_status = *Status;
	pSerialWnd->SetWindowText(*Serial);
	m_serialNum = *Serial;
	//if the licsense is expired even if days are left
	//could be due to tampering
	//then set the days left text to 0
	//otherwise use the correct value
	
	if(_tcscmp(*Status, _T("Expired")) == 0)
	{
		pDaysLeftWnd->SetWindowText(_T("0"));
		m_daysLeft = _T("0");
	}
	else
	{
		pDaysLeftWnd->SetWindowText(*ExpireVal);
		m_daysLeft = *ExpireVal;
	}

}

void SECControl::OnRegisterstatusSoftlocx6ctrl1(BSTR FAR* Status, long Level) 
{
	CWnd* pStatusWnd = GetDlgItem(IDC_STATUS);

	pStatusWnd->SetWindowText(*Status);
	m_status = *Status;

	if(_tcscmp(*Status, _T("Licensed")) == 0)
	{
		m_unlockButton.EnableWindow(FALSE);		
	}

}

void SECControl::OnUnlock() 
{
	TCHAR	szConvert[4];
	TCHAR   szUnlock[10];
	//int		nConvert;
	
	memset(szUnlock, 0x00, sizeof(szUnlock));
	memset(szConvert, 0x00, sizeof(szConvert));

	CWnd* pUnlockWnd = GetDlgItem(IDC_UNLOCK_CODE);
	pUnlockWnd->GetWindowText(m_unlockCode);

	//OLD CONVERSION CODE (ONLY WORKED WITH SOFTLOCX) - TLDJR
	//szConvert[0] = '0';
	/*
	for(int i = 0, j = 1, k = 0; i < m_unlockCode.GetLength(); i++)
	{
		//parse charater out of the number
		//in case the person did a paste of the unlock number
		//only numbers can be entered manually, but a paste will
		//put charaters in the edit box
		if(m_unlockCode.GetAt(i) < 0x30 || m_unlockCode.GetAt(i) > 0x39)
		{
			continue;
		}
		else
		{
			szConvert[j++] = m_unlockCode.GetAt(i);
			if(j == 4)
			{
				j = 1;
				nConvert = wcstoul(szConvert, NULL, 8);
				szUnlock[k++] = (TCHAR)nConvert;
			}
		}
	}
	*/
	/*
	// THIS CODE BELOW IS NO LONGER NEEDED, WE ASSUME THE USER 
	// HAS A KEYBOARD AND THERFORE DOESN'T NEED THE UNLOCK CODE TO 
	// BE ENCODED --TLDJR 02-29-2008
	// This is slightly modified version of the old conversion code
	// that works with both vbo lock and softlocx unlock codes - TLDJR
	for(int i = 0, j = 0, k = 0; i < m_unlockCode.GetLength(); i++)
	{
		//parse charater out of the number
		//in case the person did a paste of the unlock number
		//only numbers can be entered manually, but a paste will
		//put charaters in the edit box
		if(m_unlockCode.GetAt(i) < 0x30 || m_unlockCode.GetAt(i) > 0x39)
		{
			continue;
		}
		else
		{
			szConvert[j++] = m_unlockCode.GetAt(i);
			if(j == 3)
			{
				j = 0;
				nConvert = wcstoul(szConvert, NULL, 8);
				szUnlock[k++] = (TCHAR)nConvert;
			}
		}
	}
	*/
	//szUnlock = m_unlockCode.GetBuffer(m_unlockCode.GetLength());
/*	if(!(m_softlocx.Register(szUnlock)) && _tcscmp(m_daysLeft, _T("0")) == 0)
	{
		m_repair.ShowWindow(SW_SHOW);
	}
	*/
	if(!(m_softlocx.Register(m_unlockCode)) && _tcscmp(m_daysLeft, _T("0")) == 0)
	{
		m_repair.ShowWindow(SW_SHOW);
	}
}

BOOL SECControl::OnInitDialog() 
{
	
	CDialog::OnInitDialog();
#ifdef VBOLOCK // If We're using VBOLock

	//CString path = "C:\\FlashDisk\\NCR\\Saratoga\\"; <--THIS DIRECTORY IS FOR NHPOS' LICENSE.DAT...NOT PEP
    
	// Set the path so that VBOLock will know where to find the 
	// LICENSE.DAT file.
;
	CString path = __argv[0];
	// trim "pep.exe" from the filename so we're left with
	// just the path.
	path.Replace(_T("pep.exe"),_T("")); 
	

	m_vbolock.SetPath(path);

	// Expire the vboolock license for testing purposes
	//m_vbolock.ExpireLicense(1);

	m_vbolock.SetHidden(FALSE);

	int x = m_vbolock.GetTrialStatus();

#else //We're using softlocx.
	m_softlocx.License();
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SECControl::OnContinue() 
{
	int nRet = 0;

	if(_tcscmp(m_daysLeft, _T("0")) != 0 && _tcscmp(m_status, _T("Expired")))
	{
		nRet = 1;
	}
	else if(_tcscmp(m_status, _T("Licensed")) == 0)
	{
		nRet = 2;
	}

	EndDialog(nRet);
}

void SECControl::OnRepair() 
{
	::RegDeleteKey(HKEY_CLASSES_ROOT, m_softlocx.GetLicenseName());
	m_softlocx.License();
}

void SECControl::VBOLockOnRegistered() 
{
	//AfxMessageBox((CString)"Product has been successfully registered");
	EndDialog(2);
}

void SECControl::VBOLockOnExpired() 
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox
	//FINISH THIS TOMORROW.....
	//m_vbolock.GetTrialStatus();
	EndDialog(0);
}

void SECControl::VBOLockOnBackDate() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox((CString)"It appears that your date has been changed to a date in the past (Backdated).\r\n This has cause your license to expire. You must register  your software again!");
	int tmp = m_vbolock.GetTrialStatus();
}

void SECControl::VBOLockOnVerified() 
{
	// TODO: Add your control notification handler code here
	// The unlock code has been verified so proceed with normal
	// program execution
	EndDialog(2);
	
}

void SECControl::VBOLockOnMissing() 
{
	// TODO: Add your control notification handler code here
	CString path = __argv[0];
	// trim "pep.exe" from the filename so we're left with
	// just the path.
	path.Replace(_T("pep.exe"),_T("")); 
	AfxMessageBox((CString)"Your LICENSE.DAT file is missing. You must place this file in the \r\n : " 
	              + path + (CString)" directory!");
	EndDialog(0);				 
}

void SECControl::VBOLockOnMaxtimes() 
{
	// TODO: Add your control notification handler code here
	// AfxMessageBox((CString)"You've reached the maximum number of trial uses");
	EndDialog(0);
	
}

void SECControl::OnRunning() 
{
	// TODO: Add your control notification handler code here
	// This isn't needed since they can just read tbe number
	// of days left in the VBOLock Trial Dialog
	//AfxMessageBox((CString)"You have " + m_vbolock.DaysRemaining() + " days left before you must register this software.");
	EndDialog(1);
}

void SECControl::OnIllegal() 
{
	// TODO: Add your control notification handler code here

}

