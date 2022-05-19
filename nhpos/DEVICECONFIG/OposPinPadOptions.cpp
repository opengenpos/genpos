// OposPinPadOptions.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "OposPinPadOptions.h"
#include "ScfInterface.h"
#include "ScfParameters.h"


// COposPinPadOptions dialog

IMPLEMENT_DYNAMIC(COposPinPadOptions, CEquipmentDialog)

COposPinPadOptions::COposPinPadOptions(CWnd* pParent /*=NULL*/)
	: CEquipmentDialog(COposPinPadOptions::IDD, pParent)
{

}

COposPinPadOptions::~COposPinPadOptions()
{
}

void COposPinPadOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COposPinPadOptions, CDialog)
	ON_BN_CLICKED(IDOK, &COposPinPadOptions::OnBnClickedOk)
END_MESSAGE_MAP()


// COposPinPadOptions message handlers

BOOL COposPinPadOptions::UpdateDialogBox (int iDlgItemId, LPCTSTR csDeviceName, LPCTSTR csScfFieldName, TCHAR *csScfFieldDefault, int iLimit, int Status)
{
	TCHAR	portname[SCF_USER_BUFFER];
	DWORD dwRet, dwCount, dwDataType;
	CEdit	*pclEditBox;

	pclEditBox = (CEdit *)GetDlgItem (iDlgItemId);

	portname[0] = 0;
	if(Status == USE_REGISTRY)
	{
		dwRet = ScfGetParameter (csDeviceName, csScfFieldName, &dwDataType, portname, sizeof(portname), &dwCount);
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		if (dwRet == SCF_SUCCESS) {
			pclEditBox->SetWindowText (portname);
		}
		else {
			if (csScfFieldDefault)
				pclEditBox->SetWindowText (csScfFieldDefault);
			else
				pclEditBox->SetWindowText (_T(""));
		}
	}
	else
	{
		if (csScfFieldDefault)
			pclEditBox->SetWindowText (csScfFieldDefault);
		else
			pclEditBox->SetWindowText (_T(""));
	}

	if (iLimit > 0)
		pclEditBox->LimitText (iLimit);		

	return TRUE;
}

BOOL COposPinPadOptions::UpdateRegistry (CString csFieldValue, LPCTSTR csDeviceName, LPCTSTR csScfFieldName, TCHAR *csScfFieldDefault)
{
	TCHAR	portname[SCF_USER_BUFFER];
	DWORD   dwRet, dwCount;

	csFieldValue.TrimLeft ();
	csFieldValue.TrimRight ();

	portname[0] = 0;
	if (csFieldValue.GetLength()) {
		_tcscpy (portname, csFieldValue);
	} else {
		if (csScfFieldDefault)
			_tcscpy (portname, csScfFieldDefault);
	}

	dwRet = ScfSetParameter(csDeviceName, csScfFieldName, SCF_DATA_STRING, portname, _tcslen(portname) * sizeof(TCHAR), &dwCount);

	return TRUE;
}


BOOL  COposPinPadOptions::readOptions ()
{
	TCHAR	portname[SCF_USER_BUFFER];
	DWORD   dwRet, dwCount, dwDataType;

	portname[0] = 0;
	dwRet = ScfGetParameter (csDeviceName, SCF_DATANAME_KEYMANAGETYPE, &dwDataType, portname, sizeof(portname), &dwCount);
	if (dwRet == 0) {
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		EPTsetting.KeyManagement = portname;
	}
	else {
		EPTsetting.KeyManagement = _T("0");
	}

	portname[0] = 0;
	dwRet = ScfGetParameter (csDeviceName, SCF_DATANAME_IDLEPROMPT, &dwDataType, portname, sizeof(portname), &dwCount);
	if (dwRet == 0) {
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		EPTsetting.PadIdlePrompt = portname;
	}
	else {
		EPTsetting.PadIdlePrompt = _T("ELLO WELCOME");
	}

	portname[0] = 0;
	dwRet = ScfGetParameter (csDeviceName, SCF_DATANAME_WORKINGKEY, &dwDataType, portname, sizeof(portname), &dwCount);
	if (dwRet == 0) {
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		EPTsetting.WorkingKey = portname;
	}
	else {
		EPTsetting.WorkingKey = SCF_DATANAME_WORKINGKEY;
	}

	portname[0] = 0;
	dwRet = ScfGetParameter (csDeviceName, SCF_DATANAME_MASTERKEYINDEX, &dwDataType, portname, sizeof(portname), &dwCount);
	if (dwRet == 0) {
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		EPTsetting.MasterKeyIndex = portname;
	}
	else {
		EPTsetting.MasterKeyIndex = _T("0");
	}

	portname[0] = 0;
	dwRet = ScfGetParameter (csDeviceName, SCF_DATANAME_SHOULDROTATE, &dwDataType, portname, sizeof(portname), &dwCount);
	if (dwRet == 0) {
		portname[SCF_USER_BUFFER - 1] = 0;  // ensure end of string terminator is in the string.
		EPTsetting.ShouldRotate = portname;
	}
	else {
		EPTsetting.ShouldRotate = _T("0");
	}
	return TRUE;
}

void COposPinPadOptions::writeChanges () 
{
	UpdateRegistry (EPTsetting.KeyManagement, csDeviceName, SCF_DATANAME_KEYMANAGETYPE, _T("0"));
	UpdateRegistry (EPTsetting.PadIdlePrompt, csDeviceName, SCF_DATANAME_IDLEPROMPT, _T("HELLO WELCOME"));
	UpdateRegistry (EPTsetting.WorkingKey, csDeviceName, SCF_DATANAME_WORKINGKEY, SCF_DATANAME_WORKINGKEY);
	UpdateRegistry (EPTsetting.MasterKeyIndex, csDeviceName, SCF_DATANAME_MASTERKEYINDEX, _T("0"));
	UpdateRegistry (EPTsetting.ShouldRotate, csDeviceName, SCF_DATANAME_SHOULDROTATE, _T("0"));
}


BOOL COposPinPadOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString csTempStr,csTempStr2, csTempNameServer;
	TCHAR atchSetting [128];
	CString csDeviceName;

	//--------------------------------------------------
	if(EPTsetting.KeyManagement.IsEmpty ()){
		UpdateDialogBox (IDC_KEYMANAGETYPE_ID, csDeviceName, SCF_DATANAME_KEYMANAGETYPE, _T("0"), 1, USE_REGISTRY);
	}
	else{
		_tcscpy(atchSetting, EPTsetting.KeyManagement );
		UpdateDialogBox (IDC_KEYMANAGETYPE_ID, csDeviceName, SCF_DATANAME_KEYMANAGETYPE, atchSetting, 1, USE_DATA);
	}

	if(EPTsetting.PadIdlePrompt.IsEmpty ()){
		UpdateDialogBox (IDC_IDLEPROMPT_ID, csDeviceName, SCF_DATANAME_IDLEPROMPT, _T("HELLO WELCOME"), 16, USE_REGISTRY);
	}
	else{
		_tcscpy(atchSetting, EPTsetting.PadIdlePrompt );
		UpdateDialogBox (IDC_IDLEPROMPT_ID, csDeviceName, SCF_DATANAME_IDLEPROMPT, atchSetting, 16, USE_DATA);
	}

	if(EPTsetting.WorkingKey.IsEmpty ()){
		UpdateDialogBox (IDC_WORKINGKEY_ID, csDeviceName, SCF_DATANAME_WORKINGKEY, SCF_DATANAME_WORKINGKEY, 16, USE_REGISTRY);
	}
	else{
		_tcscpy(atchSetting, EPTsetting.WorkingKey );
		UpdateDialogBox (IDC_WORKINGKEY_ID, csDeviceName, SCF_DATANAME_WORKINGKEY, atchSetting, 16, USE_DATA);
	}
	if(EPTsetting.MasterKeyIndex.IsEmpty ()){
		UpdateDialogBox (IDC_MASTERKEYINDEX_ID, csDeviceName, SCF_DATANAME_MASTERKEYINDEX, _T("0"), 1, USE_REGISTRY);
	}
	else{
		_tcscpy(atchSetting, EPTsetting.MasterKeyIndex );
		UpdateDialogBox (IDC_MASTERKEYINDEX_ID, csDeviceName, SCF_DATANAME_MASTERKEYINDEX, atchSetting, 1, USE_DATA);
	}
	if(EPTsetting.ShouldRotate.IsEmpty ()){
		UpdateDialogBox (IDC_SHOULDROTATE_ID, csDeviceName, SCF_DATANAME_SHOULDROTATE, _T("0"), 1, USE_REGISTRY);
	}
	else{
		_tcscpy(atchSetting, EPTsetting.ShouldRotate );
		UpdateDialogBox (IDC_SHOULDROTATE_ID, csDeviceName, SCF_DATANAME_SHOULDROTATE, atchSetting, 1, USE_DATA);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


short COposPinPadOptions::UpdateDialogSettings () 
{
	// TODO: Add your control notification handler code here
	CString csTempStr;
	CString csTempStr2;
	TCHAR   atchDeviceName[SCF_USER_BUFFER];
	DWORD   dwCount, dwDataType;
	int     iLen;
	BOOL	bEqualSet = FALSE;
	CEdit   *pclEditBox;
	CString csDeviceName;

	//--------------------------------------------------
	//getting Key Management Type entered in edit box
	//  Verifone 2000 KeyManagementType: 0, 1,2 ONLY
	//		MASTER_SESSION = 0
	//		MASTER_ONLY = 1
	//		DUKPT = 2
	pclEditBox = (CEdit *)GetDlgItem (IDC_KEYMANAGETYPE_ID);
	csTempStr2.Empty ();
	pclEditBox->GetWindowText (csTempStr2);
	csTempStr2.TrimLeft ();
	csTempStr2.TrimRight ();
	csKeyManagementType = csTempStr2;	
	int i = _ttoi(csKeyManagementType);
	if(i > EPT_DIALOG_KEY_MANAGMENT_MAX)
	{
		MessageBox(_T("Management Type Too High"),_T("Key Management Error"),MB_OK);
		ScfGetParameter (csDeviceName, SCF_DATANAME_KEYMANAGETYPE, &dwDataType, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
		pclEditBox->SetWindowText(atchDeviceName);
		return -1;
	}
	//writing Key Management Type to registy if ok is clicked
	if (csKeyManagementType.GetLength()) {
		_tcscpy (atchDeviceName, csKeyManagementType);
	} else {
		_tcscpy (atchDeviceName, _T("0"));
	}
	EPTsetting.KeyManagement = atchDeviceName;
							
	//--------------------------------------------------
	//getting Master Key Index entered in edit box.  Range is 0-9 for Verifone 2000
	pclEditBox = (CEdit *)GetDlgItem (IDC_MASTERKEYINDEX_ID);
	csTempStr2.Empty ();
	pclEditBox->GetWindowText (csTempStr2);
	csTempStr2.TrimLeft ();
	csTempStr2.TrimRight ();
	csMasterKeyIndex = csTempStr2;	
	//writing Master Key Index to registy if ok is clicked
	if (csMasterKeyIndex.GetLength()) {
		_tcscpy (atchDeviceName, csMasterKeyIndex);
	} else {
		_tcscpy (atchDeviceName, _T("0"));
	}
	EPTsetting.MasterKeyIndex = atchDeviceName;
							
	//--------------------------------------------------
	//getting Working Key entered in edit box
	pclEditBox = (CEdit *)GetDlgItem (IDC_WORKINGKEY_ID);
	csTempStr2.Empty ();
	pclEditBox->GetWindowText (csTempStr2);
	csTempStr2.MakeUpper ();
	csTempStr2.TrimLeft ();
	csTempStr2.TrimRight ();
	csWorkingKey = csTempStr2;
	//writing Working Key to registy if ok is clicked
	if (csWorkingKey.GetLength()) {
		_tcscpy (atchDeviceName, csWorkingKey);
	} else {
		_tcscpy (atchDeviceName, SCF_DATANAME_WORKINGKEY);
	}
	EPTsetting.WorkingKey = atchDeviceName;

	//--------------------------------------------------
	//getting Idle Prompt entered in edit box
	pclEditBox = (CEdit *)GetDlgItem (IDC_IDLEPROMPT_ID);
	csTempStr2.Empty ();
	pclEditBox->GetWindowText (csTempStr2);
	csIdlePrompt = csTempStr2;
	csIdlePrompt.TrimLeft ();
	csIdlePrompt.TrimRight ();
	csIdlePrompt.MakeUpper ();
	// replace known bad characters with spaces.  Testing indicates this should be
	// an alphanumeric string only or may cause NHPOS crash.  Especially true
	// since Everest uses XML.
	for (iLen = 0; iLen < csIdlePrompt.GetLength (); iLen++) {
		if (!_istalnum(csIdlePrompt.GetAt (iLen))) {
			csIdlePrompt.SetAt (iLen, _T(' '));
		}
	}
	//writing Idle Prompt to registy if ok is clicked
	if (csIdlePrompt.GetLength()) {
		_tcscpy (atchDeviceName, csIdlePrompt);
	} else {
		_tcscpy (atchDeviceName, _T("WELCOME"));
	}
	EPTsetting.PadIdlePrompt = atchDeviceName;
								
	//--------------------------------------------------
	//getting Should Rotate entered in edit box
	pclEditBox = (CEdit *)GetDlgItem (IDC_SHOULDROTATE_ID);
	csTempStr2.Empty ();
	pclEditBox->GetWindowText (csTempStr2);
	csShouldRotate = csTempStr2;	
	csShouldRotate.TrimLeft ();
	csShouldRotate.TrimRight ();
	//writing Should Rotate to registy if ok is clicked
	if (csShouldRotate.GetLength()) {
		_tcscpy (atchDeviceName, csShouldRotate);
	} else {
		_tcscpy (atchDeviceName, _T("0"));
	}
	EPTsetting.ShouldRotate = atchDeviceName;

	return 1;
}


void COposPinPadOptions::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateDialogSettings ();
	OnOK();
}
