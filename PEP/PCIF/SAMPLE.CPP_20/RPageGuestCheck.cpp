// RPageGuestCheck.cpp : implementation file
//

#include "stdafx.h"
#include "PCSample.h"
#include "RPageBase.h"
#include "RPageGuestCheck.h"


// RPageGuestCheck dialog

IMPLEMENT_DYNAMIC(RPageGuestCheck, RPageBase)

RPageGuestCheck::RPageGuestCheck()
	: RPageBase(RPageGuestCheck::IDD)
	, m_guestCheck(_T("0"))
{

}

RPageGuestCheck::~RPageGuestCheck()
{
}

void RPageGuestCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CE_CHECKNUM, m_guestCheck);
}


BEGIN_MESSAGE_MAP(RPageGuestCheck, RPageBase)
	ON_BN_CLICKED(IDC_CE_SEND, &RPageGuestCheck::OnBnClickedCeSend)
END_MESSAGE_MAP()


// RPageGuestCheck message handlers

void RPageGuestCheck::OnBnClickedCeSend()
{
	UpdateData(TRUE);
	CString commandString;
	commandString.Format(_T("<GUESTCHECK_RTRV>\r<gcnum>%s</gcnum>\r</GUESTCHECK_RTRV>"),
		this->m_guestCheck);
	this->SendCommand(commandString);
}
