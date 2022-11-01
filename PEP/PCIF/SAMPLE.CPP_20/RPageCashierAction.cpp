// RPageCashierAction.cpp : implementation file
//

#include "stdafx.h"
#include "PCSample.h"
#include "RPageBase.h"
#include "RPageCashierAction.h"
//#include "afxdialogex.h"


// RPageCashierAction dialog

IMPLEMENT_DYNAMIC(RPageCashierAction, RPageBase)

RPageCashierAction::RPageCashierAction()
	: RPageBase(RPageCashierAction::IDD)
	, m_actionType(_T("loan"))
	, m_employeeId(_T("1"))
	, m_amount(1000)
	, m_totalId(1)
{
}

RPageCashierAction::~RPageCashierAction()
{
}

void RPageCashierAction::DoDataExchange(CDataExchange* pDX)
{
	RPageBase::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CE_ACTIONTYPE, m_actionType);
	DDV_MaxChars(pDX, m_actionType, 30);
	DDX_Text(pDX, IDC_CE_EMPLOYEEID, m_employeeId);
	DDV_MaxChars(pDX, m_employeeId, 30);
	DDX_Text(pDX, IDC_CE_AMOUNT, m_amount);
	DDV_MinMaxLong(pDX, m_amount, 0, 999999);
	DDX_Text(pDX, IDC_CE_TOTALID, m_totalId);
	DDV_MinMaxUInt(pDX, m_totalId, 0, 999999);
	DDX_Control(pDX, IDC_CE_SEND, m_btnSend);
	DDX_Control(pDX, IDC_CE_ACTIONTYPE, m_cActionType);
	DDX_Control(pDX, IDC_CE_AMOUNT, m_cAmount);
	DDX_Control(pDX, IDC_CE_TOTALID, m_cTotalId);
}


BEGIN_MESSAGE_MAP(RPageCashierAction, RPageBase)
	ON_BN_CLICKED(IDC_CE_SEND, &RPageCashierAction::OnBnClickedBtnsendcommand)
	ON_CBN_SELCHANGE(IDC_CE_ACTIONTYPE, &RPageCashierAction::OnCbnSelchangeCecaActiontype)
END_MESSAGE_MAP()


// RPageCashierAction message handlers

void RPageCashierAction::OnBnClickedBtnsendcommand()
{
	UpdateData();
	CString commandString;
	commandString.Format(_T("<CASHIERACTION>\r<tcsActionType>%s</tcsActionType>\r<ulEmployeeId>%s</ulEmployeeId>\r"), 
		m_actionType, m_employeeId);

	if (this->m_actionType.Compare(_T("sign-in"))  != 0 &&
		this->m_actionType.Compare(_T("sign-out")) != 0)
	{
		commandString.AppendFormat(_T("<lAmount>%d</lAmount>\r<usTotalID>%d</usTotalID>\r"), 
			m_amount, m_totalId); 
	}
	commandString.Append(_T("</CASHIERACTION>"));
	this->SendCommand(commandString);
}

void RPageCashierAction::OnCbnSelchangeCecaActiontype()
{
	UpdateData();
	if (this->m_actionType.Compare(_T("sign-in")) == 0 ||
		this->m_actionType.Compare(_T("sign-out")) == 0)
	{
		this->m_cAmount.EnableWindow(false);
		this->m_cTotalId.EnableWindow(false);
	}
	else
	{
		this->m_cAmount.EnableWindow(true);
		this->m_cTotalId.EnableWindow(true);
	}

}
