/////////////////////////////////////////////////////////////////////////////
// dlgprt.cpp : implementation file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "item.h"
#include "dlgprt.h"
#include "dlgprtd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintPropDlg dialog


CPrintPropDlg::CPrintPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintPropDlg)
	m_nAlign = -1;
	m_strMajor = "";
	m_strItem = "";
	m_bWide = FALSE;
	m_nWidth = 0;
	m_nMinor = 0;
	m_strCond = "";
	m_strGroup = "";
	//}}AFX_DATA_INIT

	m_nFile     = -1;
	m_nMaxWidth = -1;
	m_pItem     = NULL;
}

CPrintPropDlg::CPrintPropDlg(int nFile, int nWidth, CPrintItem* pItem,
	CWnd* pParent /*=NULL*/) : CDialog(CPrintPropDlg::IDD, pParent)
{
	m_nFile     = nFile;
	m_nMaxWidth = nWidth;
	m_pItem     = pItem;
}

void CPrintPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintPropDlg)
	DDX_Control(pDX, IDC_PRINT_FORMAT, m_CbFormat);
	DDX_Radio(pDX, IDC_PRINT_LEFT, m_nAlign);
	DDX_Text(pDX, IDC_PRINT_MAJOR, m_strMajor);
	DDX_Text(pDX, IDC_PRINT_ITEM, m_strItem);
	DDX_Check(pDX, IDC_PRINT_WIDE, m_bWide);
	DDX_Text(pDX, IDC_PRINT_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_PRINT_MINOR, m_nMinor);
	DDX_Text(pDX, IDC_PRINT_COND, m_strCond);
	DDX_Text(pDX, IDC_PRINT_GROUP, m_strGroup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintPropDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintPropDlg)
	ON_BN_CLICKED(IDC_PRINT_DETAIL, OnPrintDetail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPrintPropDlg implementations

const wchar_t * CPrintPropDlg::GetCondition()
{
	POSITION pos = ptheInfo->CondGetHeadPosition();
	while (pos != NULL)
	{
		INFO_COND* pCond = (INFO_COND*)ptheInfo->CondGetNext(pos);
		ASSERT(pCond != NULL);
		if (pCond != NULL && pCond->m_nNumber == m_pItem->m_nCondition)
			return pCond->m_strName;
	}

	return NULL;
}

BOOL CPrintPropDlg::CheckWidth()
{
	int nMin;
	int nMax = m_nMaxWidth - m_pItem->m_pt.x + 1;
	int nCheck = ((CButton*)(GetDlgItem(IDC_PRINT_WIDE)))->GetCheck();
	if (nCheck == 0)					// single
		nMin = 1;
	else								// double wide
		nMin = 2;
	BOOL bState;
	int nWidth = (int)GetDlgItemInt(IDC_PRINT_WIDTH, &bState);
	if (bState && nMin <= nWidth && nWidth <= nMax)
		return TRUE;

	wchar_t szMin[16], szMax[16];
	CString strMsg;
	AfxFormatString2(strMsg, IDS_ERRMSG_RANGE, _itow(nMin, szMin, 10),
											   _itow(nMax, szMax, 10));
	AfxMessageBox(strMsg);
	GetDlgItem(IDC_PRINT_WIDTH)->SetFocus();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrintPropDlg message handlers

BOOL CPrintPropDlg::OnInitDialog()
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	CInfoPrint* pPrint = CInfoPrint::GetField(m_nFile,
											  m_pItem->Element[0].m_nGroup,
											  m_pItem->Element[0].m_nField[0]);
	int nItem  = m_pItem->Element[0].m_nItem;

	m_strGroup = pConfig->GetGroupName(CFG_INDEX_PRINT,
								       m_pItem->Element[0].m_nGroup);
	m_strMajor = pPrint->m_strName;
	m_nMinor   = pPrint->m_nMinor + m_pItem->Element[0].m_nField[1];
	m_strItem  = ptheInfo->GetItem(pPrint->Item[nItem].m_nNumber);
	m_nWidth   = m_pItem->m_nWidth;
	m_bWide    = (m_pItem->m_nFormat[1] & PITEM_FORMAT_WIDE) ? (TRUE) : (FALSE);
	m_nAlign   = (m_pItem->m_nFormat[1] & PITEM_FORMAT_LEFT) ? (0) : (1);
	m_strCond  = GetCondition();

	CDialog::OnInitDialog();

	int nIndex = 0;
	POSITION pos = ptheInfo->m_Format.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FORMAT* pFormat = (INFO_FORMAT*)ptheInfo->m_Format.GetNext(pos);
		if (pPrint->Item[nItem].m_nFormat >= 2 && pFormat->m_nNumber <= 1)
			continue;
		m_CbFormat.InsertString(-1, pFormat->m_strName);
		if (pFormat->m_nNumber == m_pItem->m_nFormat[0])
			nIndex = m_CbFormat.GetCount() - 1;	// zero-based
	}
	m_CbFormat.SetCurSel(nIndex);

	if (! (pPrint->m_nFlags & PRINT_F_WIDTH))
		GetDlgItem(IDC_PRINT_WIDTH)->EnableWindow(FALSE);
	if (! (pPrint->m_nFlags & PRINT_F_RIGHT))
		GetDlgItem(IDC_PRINT_RIGHT)->EnableWindow(FALSE);
	if (! (pPrint->m_nFlags & PRINT_F_LEFT))
		GetDlgItem(IDC_PRINT_LEFT)->EnableWindow(FALSE);
	if (! (pPrint->m_nFlags & PRINT_F_WIDE))
		GetDlgItem(IDC_PRINT_WIDE)->EnableWindow(FALSE);
	if (! (pPrint->m_nFlags & PRINT_F_FORMAT))
		GetDlgItem(IDC_PRINT_FORMAT)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrintPropDlg::OnOK()
{
	if (! CheckWidth())
		return;

	CDialog::OnOK();

	CInfoPrint* pPrint = CInfoPrint::GetField(m_nFile,
											  m_pItem->Element[0].m_nGroup,
											  m_pItem->Element[0].m_nField[0]);

	int nBias = (pPrint->Item[m_pItem->Element[0].m_nItem].m_nFormat < 2) ?
				(0) : (2);

	POSITION pos = ptheInfo->m_Format.FindIndex(nBias + m_CbFormat.GetCurSel());
	if (pos != NULL)
	{
		INFO_FORMAT* pFormat = (INFO_FORMAT*)ptheInfo->m_Format.GetAt(pos);
		ASSERT(pFormat != NULL);
		m_pItem->m_nFormat[0] = pFormat->m_nNumber;
	}

	m_pItem->m_nWidth      = m_nWidth;
	m_pItem->m_nFormat[1]  = (m_bWide)       ? (PITEM_FORMAT_WIDE) : (0);
	m_pItem->m_nFormat[1] |= (m_nAlign == 0) ? (PITEM_FORMAT_LEFT) :
											   (PITEM_FORMAT_RIGHT);
}

void CPrintPropDlg::OnPrintDetail()
{
	CPrintDetailDlg dlg(m_nFile, m_pItem);
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_PRINT_COND, GetCondition());
}
