// DGroupPermission.cpp : implementation file
//

#include "stdafx.h"
#include "DGroupPermission.h"


// DGroupPermission dialog

IMPLEMENT_DYNAMIC(DGroupPermission, CDialog)

DGroupPermission::DGroupPermission(ULONG groupPermissionMask, CWnd* pParent /*=NULL*/)
	: CDialog(DGroupPermission::IDD, pParent),
	groupPermissionMask(groupPermissionMask)
{
	for(int i = 0; i < 32; i++)
		CButtonGroupAllowed[i] = FALSE;

	InterpretMask();
}

DGroupPermission::~DGroupPermission()
{
}

void DGroupPermission::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, CButtonGroupAllowed[0]);
	DDX_Check(pDX, IDC_CHECK2, CButtonGroupAllowed[1]);
	DDX_Check(pDX, IDC_CHECK3, CButtonGroupAllowed[2]);
	DDX_Check(pDX, IDC_CHECK4, CButtonGroupAllowed[3]);
	DDX_Check(pDX, IDC_CHECK5, CButtonGroupAllowed[4]);
	DDX_Check(pDX, IDC_CHECK6, CButtonGroupAllowed[5]);
	DDX_Check(pDX, IDC_CHECK7, CButtonGroupAllowed[6]);
	DDX_Check(pDX, IDC_CHECK8, CButtonGroupAllowed[7]);
	DDX_Check(pDX, IDC_CHECK9, CButtonGroupAllowed[8]);
	DDX_Check(pDX, IDC_CHECK10, CButtonGroupAllowed[9]);
	DDX_Check(pDX, IDC_CHECK11, CButtonGroupAllowed[10]);
	DDX_Check(pDX, IDC_CHECK12, CButtonGroupAllowed[11]);
	DDX_Check(pDX, IDC_CHECK13, CButtonGroupAllowed[12]);
	DDX_Check(pDX, IDC_CHECK14, CButtonGroupAllowed[13]);
	DDX_Check(pDX, IDC_CHECK15, CButtonGroupAllowed[14]);
	DDX_Check(pDX, IDC_CHECK16, CButtonGroupAllowed[15]);
	DDX_Check(pDX, IDC_CHECK17, CButtonGroupAllowed[16]);
	DDX_Check(pDX, IDC_CHECK18, CButtonGroupAllowed[17]);
	DDX_Check(pDX, IDC_CHECK19, CButtonGroupAllowed[18]);
	DDX_Check(pDX, IDC_CHECK20, CButtonGroupAllowed[19]);
	DDX_Check(pDX, IDC_CHECK21, CButtonGroupAllowed[20]);
	DDX_Check(pDX, IDC_CHECK22, CButtonGroupAllowed[21]);
	DDX_Check(pDX, IDC_CHECK23, CButtonGroupAllowed[22]);
	DDX_Check(pDX, IDC_CHECK24, CButtonGroupAllowed[23]);
	DDX_Check(pDX, IDC_CHECK25, CButtonGroupAllowed[24]);
	DDX_Check(pDX, IDC_CHECK26, CButtonGroupAllowed[25]);
	DDX_Check(pDX, IDC_CHECK27, CButtonGroupAllowed[26]);
	DDX_Check(pDX, IDC_CHECK28, CButtonGroupAllowed[27]);
	DDX_Check(pDX, IDC_CHECK29, CButtonGroupAllowed[28]);
	DDX_Check(pDX, IDC_CHECK30, CButtonGroupAllowed[29]);
	DDX_Check(pDX, IDC_CHECK31, CButtonGroupAllowed[30]);
	DDX_Check(pDX, IDC_CHECK32, CButtonGroupAllowed[31]);
}


BEGIN_MESSAGE_MAP(DGroupPermission, CDialog)
	ON_BN_CLICKED(IDC_BTN_CHECK_ALL, &DGroupPermission::OnBnClickedCheckAll)
	ON_BN_CLICKED(IDC_BTN_CHECK_NONE, &DGroupPermission::OnBnClickedCheckNone)
	ON_BN_CLICKED(IDC_BTN_CHECK_INVERT, &DGroupPermission::OnBnClickedCheckInvert)
	ON_BN_CLICKED(IDOK, &DGroupPermission::OnBnClickedOk)
END_MESSAGE_MAP()


// DGroupPermission message handlers

void DGroupPermission::OnBnClickedCheckAll(){
	for(int i = 0; i < 32; i++)
		CButtonGroupAllowed[i] = TRUE;
	UpdateData(FALSE);
}

void DGroupPermission::OnBnClickedCheckNone(){
	for(int i = 0; i < 32; i++)
		CButtonGroupAllowed[i] = FALSE;
	UpdateData(FALSE);
}

void DGroupPermission::OnBnClickedCheckInvert(){
	UpdateData();
	for(int i = 0; i < 32; i++)
		CButtonGroupAllowed[i] = CButtonGroupAllowed[i] ? FALSE : TRUE;
	UpdateData(FALSE);
}

void DGroupPermission::OnBnClickedOk()
{
	UpdateData();
	ConvertToMask();
	OnOK();
}

ULONG DGroupPermission::GetGroupPermissionMask(){
	return groupPermissionMask;
}
