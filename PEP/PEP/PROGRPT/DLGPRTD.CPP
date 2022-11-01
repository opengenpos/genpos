/////////////////////////////////////////////////////////////////////////////
// dlgprtd.cpp : implementation file
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
#include "dlgprtd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDetailDlg dialog


CPrintDetailDlg::CPrintDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintDetailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintDetailDlg)
	//}}AFX_DATA_INIT

	m_nFile   = -1;
	m_pItem   = NULL;

	Init();
}

CPrintDetailDlg::CPrintDetailDlg(int nFile, CPrintItem* pItem, CWnd* pParent /*=NULL*/)
	: CDialog(CPrintDetailDlg::IDD, pParent)
{
	m_nFile   = nFile;
	m_pItem   = pItem;

	Init();
}

void CPrintDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDetailDlg)
	DDX_Control(pDX, IDC_PRINTD_COND2, m_Cond2);
	DDX_Control(pDX, IDC_PRINTD_COND1, m_Cond1);
	DDX_Control(pDX, IDC_PRINTD_MI2, m_Minor2);
	DDX_Control(pDX, IDC_PRINTD_MI1, m_Minor1);
	DDX_Control(pDX, IDC_PRINTD_MA2, m_Major2);
	DDX_Control(pDX, IDC_PRINTD_MA1, m_Major1);
	DDX_Control(pDX, IDC_PRINTD_IT2, m_Item2);
	DDX_Control(pDX, IDC_PRINTD_IT1, m_Item1);
	DDX_Control(pDX, IDC_PRINTD_GR2, m_Group2);
	DDX_Control(pDX, IDC_PRINTD_GR1, m_Group1);
	DDX_Control(pDX, IDC_PRINTD_MINOR2, m_CbMinor2);
	DDX_Control(pDX, IDC_PRINTD_MINOR1, m_CbMinor1);
	DDX_Control(pDX, IDC_PRINTD_MAJOR2, m_CbMajor2);
	DDX_Control(pDX, IDC_PRINTD_MAJOR1, m_CbMajor1);
	DDX_Control(pDX, IDC_PRINTD_ITEM2, m_CbItem2);
	DDX_Control(pDX, IDC_PRINTD_GROUP2, m_CbGroup2);
	DDX_Control(pDX, IDC_PRINTD_GROUP1, m_CbGroup1);
	DDX_Control(pDX, IDC_PRINTD_ITEM1, m_CbItem1);
	DDX_Control(pDX, IDC_PRINTD_COND, m_CbCond);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintDetailDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintDetailDlg)
	ON_CBN_SELCHANGE(IDC_PRINTD_COND, OnSelchangePrintdCond)
	ON_CBN_SELCHANGE(IDC_PRINTD_GROUP1, OnSelchangePrintdGroup1)
	ON_CBN_SELCHANGE(IDC_PRINTD_GROUP2, OnSelchangePrintdGroup2)
	ON_CBN_SELCHANGE(IDC_PRINTD_MAJOR1, OnSelchangePrintdMajor1)
	ON_CBN_SELCHANGE(IDC_PRINTD_MAJOR2, OnSelchangePrintdMajor2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFieldDlg implementation

void CPrintDetailDlg::Init()
{
	m_bCond1  = -2;						// illegal data
	m_nGroup1 = -1;
	m_nMajor1 = -1;

	m_bCond2  = -2;						// illegal data
	m_nGroup2 = -1;
	m_nMajor2 = -1;
}

void CPrintDetailDlg::SetInitData()
{
	// insert condition list
	int nIndex = 0;
	POSITION pos = ptheInfo->CondGetHeadPosition();
	while (pos != NULL)
	{
		INFO_COND* pCond = ptheInfo->CondGetNext(pos);
		if (pCond != NULL)
		{
			m_CbCond.InsertString(-1, pCond->m_strName);
			if (pCond->m_nNumber == m_pItem->m_nCondition)
				nIndex = m_CbCond.GetCount() - 1;	// zero-based
		}
		else
			ASSERT(pCond != NULL);
	}
	m_CbCond.SetCurSel(nIndex);

	// insert group list
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;
	pos = pConfig->GetHeadGroup();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = pConfig->GetNextGroup(pos, CFG_INDEX_PRINT);
		if (pOpeGroup == NULL)
			break;
		m_CbGroup1.InsertString(-1, pOpeGroup->m_strName);
		m_CbGroup2.InsertString(-1, pOpeGroup->m_strName);
	}

	m_bCond1 = UpdateState(1);
	if (m_pItem->Element[1].m_nGroup >= 0)
	{
		m_CbGroup1.SetCurSel(m_pItem->Element[1].m_nGroup);
		RefreshMajor(TRUE, 1);
		RefreshMinor(TRUE, 1);
	}

	m_bCond2 = UpdateState(2);
	if (m_pItem->Element[2].m_nGroup >= 0)
	{
		m_CbGroup2.SetCurSel(m_pItem->Element[2].m_nGroup);
		RefreshMajor(TRUE, 2);
		RefreshMinor(TRUE, 2);
	}
}

void CPrintDetailDlg::RefreshMajor(BOOL bInit, int nLevel)
{
	int nGroup;
	CComboBox* pCbMajor;

	if (nLevel == 1)
	{
		nGroup = m_CbGroup1.GetCurSel();
		if (nGroup != m_nGroup1)
			m_nGroup1 = nGroup;
		else
			return;

		pCbMajor = &m_CbMajor1;
		pCbMajor->ResetContent();
		m_CbMinor1.ResetContent();
		m_CbItem1.ResetContent();
	}
	else
	{
		nGroup = m_CbGroup2.GetCurSel();
		if (nGroup != m_nGroup2)
			m_nGroup2 = nGroup;
		else
			return;

		pCbMajor = &m_CbMajor2;
		pCbMajor->ResetContent();
		m_CbMinor2.ResetContent();
		m_CbItem2.ResetContent();

	}
	if (nGroup == CB_ERR)
		return;

	// get operation configuration
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(CFG_INDEX_PRINT, nGroup);
	if (pOpeGroup == NULL)
		return;

	// insert major list
	int nCount = 0;
	int nIndex = 0;
	POSITION pos = pConfig->FieldFindIndex(pOpeGroup->m_nIndex);
	while (pos != NULL && nCount < pOpeGroup->m_nCount)
	{
		CInfoPrint* pPrint = (CInfoPrint*)pConfig->FieldGetNext(pos);
		if (pPrint == NULL)
			break;
		if (pPrint->Item[0].m_nNumber > 0)	// condition object
		{
			pCbMajor->InsertString(-1, pPrint->m_strName);
			m_nIndex[nLevel - 1][nIndex++] = nCount;
		}
		if (bInit && nCount == m_pItem->Element[nLevel].m_nField[0])
			pCbMajor->SetCurSel(pCbMajor->GetCount() - 1);
		nCount++;
	}

	if (nIndex < PDETAIL_NUM_INDEX)
		m_nIndex[nLevel - 1][nIndex] = -1;	// end of index
	else
		ASSERT(FALSE);
}

void CPrintDetailDlg::RefreshMinor(BOOL bInit, int nLevel)
{
	int nGroup, nCount;
	int nMajor;
	CComboBox* pCbMinor;
	CComboBox* pCbItem;

	if (nLevel == 1)
	{
		nMajor = m_CbMajor1.GetCurSel();
		if (nMajor != m_nMajor1)
			m_nMajor1 = nMajor;
		else
			return;

		pCbMinor = &m_CbMinor1;
		pCbMinor->ResetContent();
		pCbItem = &m_CbItem1;
		pCbItem->ResetContent();

		nGroup = m_CbGroup1.GetCurSel();
	}
	else
	{
		nMajor = m_CbMajor2.GetCurSel();
		if (nMajor != m_nMajor2)
			m_nMajor2 = nMajor;
		else
			return;

		pCbMinor = &m_CbMinor2;
		pCbMinor->ResetContent();
		pCbItem = &m_CbItem2;
		pCbItem->ResetContent();

		nGroup = m_CbGroup2.GetCurSel();
	}
	if (nGroup == CB_ERR || nMajor == CB_ERR)
		return;

	// get major field information
	CInfoPrint* pPrint = GetInfo(nLevel, nGroup, nMajor);

	// insert minor list
	wchar_t szBuf[16];
	for (nCount = 0; nCount < pPrint->m_nMinorCount; nCount++)
	{
		pCbMinor->InsertString(-1, _itow(pPrint->m_nMinor + nCount, szBuf, 10));
		if (bInit && nCount == m_pItem->Element[nLevel].m_nField[1])
			pCbMinor->SetCurSel(nCount);
	}

	// insert item list
	for (nCount = 0; nCount < ITEM_NUM_ELEMENT; nCount++)
	{
		int nItem = pPrint->Item[nCount].m_nNumber;
		if (nItem == _FREE)
			break;
		pCbItem->InsertString(-1, ptheInfo->GetItem(nItem));
		if (bInit && nCount == m_pItem->Element[nLevel].m_nItem)
			pCbItem->SetCurSel(pCbItem->GetCount() - 1);
	}
}

BOOL CPrintDetailDlg::UpdateState(int nLevel)
{
	CComboBox* pCb;

	BOOL bState = GetCondLevel(nLevel);

	((nLevel == 1) ? (m_Cond1)  : (m_Cond2) ).EnableWindow(bState);
	((nLevel == 1) ? (m_Group1) : (m_Group2)).EnableWindow(bState);
	((nLevel == 1) ? (m_Major1) : (m_Major2)).EnableWindow(bState);
	((nLevel == 1) ? (m_Minor1) : (m_Minor2)).EnableWindow(bState);
	((nLevel == 1) ? (m_Item1)  : (m_Item2) ).EnableWindow(bState);

	pCb = (nLevel == 1) ? (&m_CbGroup1) : (&m_CbGroup2);
	pCb->EnableWindow(bState);
	if (! bState)
		pCb->SetCurSel(-1);

	pCb = (nLevel == 1) ? (&m_CbMajor1) : (&m_CbMajor2);
	pCb->EnableWindow(bState);
	if (! bState)
		pCb->ResetContent();

	pCb = (nLevel == 1) ? (&m_CbMinor1) : (&m_CbMinor2);
	pCb->EnableWindow(bState);
	if (! bState)
		pCb->ResetContent();

	pCb = (nLevel == 1) ? (&m_CbItem1) : (&m_CbItem2);
	pCb->EnableWindow(bState);
	if (! bState)
		pCb->ResetContent();

	return bState;
}

BOOL CPrintDetailDlg::GetCondLevel(int nLevel)
{
	INFO_COND* pCond = GetCond();
	if (pCond == NULL)
		return FALSE;

	if (nLevel == 1)
		return (0 < pCond->m_nNumber) ? (TRUE) : (FALSE);
	else
		return (0 < pCond->m_nNumber && pCond->m_nNumber < 12) ? (TRUE) : (FALSE);
}

INFO_COND* CPrintDetailDlg::GetCond()
{
	int nSel = m_CbCond.GetCurSel();
	if (nSel == CB_ERR)
		return NULL;
	POSITION pos = ptheInfo->CondFindIndex(nSel);
	if (pos == NULL)
		return NULL;

	return ptheInfo->CondGetAt(pos);
}

CInfoPrint* CPrintDetailDlg::GetInfo(int nLevel, int nGroup, int nMajor)
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return NULL;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(CFG_INDEX_PRINT, nGroup);
	if (pOpeGroup == NULL)
		return NULL;
	POSITION pos = pConfig->
		FieldFindIndex(pOpeGroup->m_nIndex + m_nIndex[nLevel - 1][nMajor]);
	if (pos == NULL)
		return NULL;
		
	return (CInfoPrint*)pConfig->FieldGetNext(pos);
}

/////////////////////////////////////////////////////////////////////////////
// CPrintDetailDlg message handlers

BOOL CPrintDetailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetInitData();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrintDetailDlg::OnOK()
{
	BOOL bCond1, bCond2;
	int nMinor1, nMinor2, nItem1, nItem2;

	bCond1  = GetCondLevel(1);
	bCond2  = GetCondLevel(2);
	nMinor1 = m_CbMinor1.GetCurSel();
	nMinor2 = m_CbMinor2.GetCurSel();
	nItem1  = m_CbItem1.GetCurSel();
	nItem2  = m_CbItem2.GetCurSel();
		
	if ((bCond1 && (m_nGroup1 < 0 || m_nMajor1 < 0 || nMinor1 == CB_ERR ||
													  nItem1  == CB_ERR)) ||
		(bCond2 && (m_nGroup2 < 0 || m_nMajor2 < 0 || nMinor2 == CB_ERR ||
													  nItem2  == CB_ERR)))
	{
		AfxMessageBox(IDS_ERRMSG_COND);
		return;
	}

	INFO_COND* pCond = GetCond();
	if (pCond == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	m_pItem->m_nCondition = pCond->m_nNumber;

	if (bCond1)
	{
		CInfoPrint* pPrint = GetInfo(1, m_nGroup1, m_nMajor1);
		if (pPrint == NULL)
		{
			ASSERT(FALSE);
			return;
		}
		
		m_pItem->Element[1].m_nGroup    = m_nGroup1;
		m_pItem->Element[1].m_nField[0] = m_nIndex[0][m_nMajor1];
		m_pItem->Element[1].m_nField[1] = nMinor1;
		m_pItem->Element[1].m_nItem     = nItem1;
	}
	else
	{
		m_pItem->Element[1].m_nGroup    = -1;
		m_pItem->Element[1].m_nField[0] = -1;
		m_pItem->Element[1].m_nField[1] = -1;
		m_pItem->Element[1].m_nItem     = -1;
	}

	if (bCond2)
	{
		CInfoPrint* pPrint = GetInfo(2, m_nGroup2, m_nMajor2);
		if (pPrint == NULL)
		{
			ASSERT(FALSE);
			return;
		}
		
		m_pItem->Element[2].m_nGroup    = m_nGroup2;
		m_pItem->Element[2].m_nField[0] = m_nIndex[1][m_nMajor2];
		m_pItem->Element[2].m_nField[1] = nMinor2;
		m_pItem->Element[2].m_nItem     = nItem2;
	}
	else
	{
		m_pItem->Element[2].m_nGroup    = -1;
		m_pItem->Element[2].m_nField[0] = -1;
		m_pItem->Element[2].m_nField[1] = -1;
		m_pItem->Element[2].m_nItem     = -1;
	}

	EndDialog(IDOK);
}

void CPrintDetailDlg::OnSelchangePrintdCond()
{
	BOOL bCond;

	if ((bCond = UpdateState(1)) != m_bCond1)
	{
		m_bCond1 = bCond;
		m_CbMajor1.ResetContent();
		m_CbMinor1.ResetContent();
		m_CbItem1.ResetContent();
	}

	if ((bCond = UpdateState(2)) != m_bCond2)
	{
		m_bCond2 = bCond;
		m_CbMajor2.ResetContent();
		m_CbMinor2.ResetContent();
		m_CbItem2.ResetContent();
	}
}

void CPrintDetailDlg::OnSelchangePrintdGroup1()
{
	RefreshMajor(FALSE, 1);
}

void CPrintDetailDlg::OnSelchangePrintdGroup2()
{
	RefreshMajor(FALSE, 2);
}

void CPrintDetailDlg::OnSelchangePrintdMajor1()
{
	RefreshMinor(FALSE, 1);
}

void CPrintDetailDlg::OnSelchangePrintdMajor2()
{
	RefreshMinor(FALSE, 2);
}
