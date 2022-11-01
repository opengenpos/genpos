// RPageEmployeeChange.cpp : implementation file
//

#include "stdafx.h"
#include "PCSample.h"
#include "RPageBase.h"
#include "RPageEmployeeChange.h"


// RPageEmployeeChange dialog

IMPLEMENT_DYNAMIC(RPageEmployeeChange, RPageBase)

RPageEmployeeChange::RPageEmployeeChange()
	: RPageBase(RPageEmployeeChange::IDD)
	, m_actionType(_T("Employee-Data"))
	, m_employeeId(_T("1"))
	, m_mnemonic(_T(""))
	, m_jobCode(_T(""))
{

}

RPageEmployeeChange::~RPageEmployeeChange()
{
}

void RPageEmployeeChange::DoDataExchange(CDataExchange* pDX)
{
	RPageBase::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CE_ACTIONTYPE, m_actionType);
	DDX_Text(pDX, IDC_CE_EMPLOYEEID, m_employeeId);
	DDX_Control(pDX, IDC_CE_MNEMONIC, m_cMnemonic);
	DDX_Text(pDX, IDC_CE_MNEMONIC, m_mnemonic);
	DDX_Control(pDX, IDC_CE_JOBCODE, m_cJobCode);
	DDX_Text(pDX, IDC_CE_JOBCODE, m_jobCode);
	DDX_Control(pDX, IDC_CE_EMPLOYEEID, m_cEmployeeId);
}


BEGIN_MESSAGE_MAP(RPageEmployeeChange, RPageBase)
	ON_CBN_SELCHANGE(IDC_CE_ACTIONTYPE, &RPageEmployeeChange::OnCbnSelchangeCeActiontype)
	ON_BN_CLICKED(IDC_CE_SEND, &RPageEmployeeChange::OnBnClickedCeSend)
END_MESSAGE_MAP()


// RPageEmployeeChange message handlers

void RPageEmployeeChange::OnCbnSelchangeCeActiontype()
{
	UpdateData(true);
	if (this->m_actionType.Compare(_T("Employee_Data")) == 0)
	{
		this->m_cJobCode.EnableWindow(false);
		this->m_cEmployeeId.EnableWindow(false);
		this->m_cMnemonic.EnableWindow(false);
	}
	else
	{
		this->m_cJobCode.EnableWindow(true);
		this->m_cEmployeeId.EnableWindow(true);
		this->m_cMnemonic.EnableWindow(true);
	}
}

void RPageEmployeeChange::OnBnClickedCeSend()
{
	UpdateData(true);
	CString commandString;
	commandString.Format(_T("<EMPLOYEECHANGE>\r<tcsActionType>%s</tcsActionType>\r"), m_actionType);
	if (this->m_actionType.Compare(_T("Employee_Data")) != 0)
	{
		commandString.AppendFormat(_T("<ulEmployeeId>%s</ulEmployeeId>\r"), this->m_employeeId);
		if (this->m_mnemonic.Trim().IsEmpty() == false)
			commandString.AppendFormat(_T("<tcsMnemonic>%s</tcsMnemonic>\r"), this->m_mnemonic);
		if (this->m_jobCode.Trim().IsEmpty() == false)
			commandString.AppendFormat(_T("<usJobCode>%s</usJobCode>\r"), this->m_jobCode);
	}
	commandString.Append(_T("</EMPLOYEECHANGE>"));
	this->SendCommand(commandString);
}
