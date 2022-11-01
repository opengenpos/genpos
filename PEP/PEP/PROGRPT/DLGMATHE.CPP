/////////////////////////////////////////////////////////////////////////////
// dlgmathe.cpp : implementation file
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
#include "dlgmathe.h"

#include <errno.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMathePropDlg dialog


CMathePropDlg::CMathePropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMathePropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMathePropDlg)
	m_strGroup = "";
	m_strItem = "";
	m_strMajor = "";
	m_strMinor = "";
	//}}AFX_DATA_INIT

	m_nFile = -1;
	m_pItem = NULL;

	Init();
}

CMathePropDlg::CMathePropDlg(int nFile, CMatheItem* pItem, CWnd* pParent /*=NULL*/)
	: CDialog(CMathePropDlg::IDD, pParent)
{
	m_nFile = nFile;
	m_pItem = pItem;

	Init();
}

void CMathePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMathePropDlg)
	DDX_Control(pDX, IDC_MATHE_OPE2, m_Ope2);
	DDX_Control(pDX, IDC_MATHE_MI2, m_Minor2);
	DDX_Control(pDX, IDC_MATHE_MI1, m_Minor1);
	DDX_Control(pDX, IDC_MATHE_MA2, m_Major2);
	DDX_Control(pDX, IDC_MATHE_MA1, m_Major1);
	DDX_Control(pDX, IDC_MATHE_IT2, m_Item2);
	DDX_Control(pDX, IDC_MATHE_IT1, m_Item1);
	DDX_Control(pDX, IDC_MATHE_GR2, m_Group2);
	DDX_Control(pDX, IDC_MATHE_GR1, m_Group1);
	DDX_Control(pDX, IDC_MATHE_VALUE, m_editValue);
	DDX_Control(pDX, IDC_MATHE_USE, m_btnUse);
	DDX_Control(pDX, IDC_MATHE_MINOR2, m_CbMinor2);
	DDX_Control(pDX, IDC_MATHE_MINOR1, m_CbMinor1);
	DDX_Control(pDX, IDC_MATHE_MAJOR2, m_CbMajor2);
	DDX_Control(pDX, IDC_MATHE_MAJOR1, m_CbMajor1);
	DDX_Control(pDX, IDC_MATHE_ITEM2, m_CbItem2);
	DDX_Control(pDX, IDC_MATHE_ITEM1, m_CbItem1);
	DDX_Control(pDX, IDC_MATHE_GROUP2, m_CbGroup2);
	DDX_Control(pDX, IDC_MATHE_GROUP1, m_CbGroup1);
	DDX_Control(pDX, IDC_MATHE_COND, m_CbCond);
	DDX_Text(pDX, IDC_MATHE_GROUP, m_strGroup);
	DDX_Text(pDX, IDC_MATHE_ITEM, m_strItem);
	DDX_Text(pDX, IDC_MATHE_MAJOR, m_strMajor);
	DDX_Text(pDX, IDC_MATHE_MINOR, m_strMinor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMathePropDlg, CDialog)
	//{{AFX_MSG_MAP(CMathePropDlg)
	ON_CBN_SELCHANGE(IDC_MATHE_COND, OnSelchangeMatheCond)
	ON_BN_CLICKED(IDC_MATHE_USE, OnMatheUse)
	ON_CBN_SELCHANGE(IDC_MATHE_GROUP1, OnSelchangeMatheGroup1)
	ON_CBN_SELCHANGE(IDC_MATHE_GROUP2, OnSelchangeMatheGroup2)
	ON_CBN_SELCHANGE(IDC_MATHE_MAJOR1, OnSelchangeMatheMajor1)
	ON_CBN_SELCHANGE(IDC_MATHE_MAJOR2, OnSelchangeMatheMajor2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMathePropDlg implementations

void CMathePropDlg::Init()
{
	m_bCond1  = -2;						// illegal data
	m_nGroup1 = -1;
	m_nMajor1 = -1;

	m_bCond2  = -2;						// illegal data
	m_nGroup2 = -1;
	m_nMajor2 = -1;
}

void CMathePropDlg::SetInitData()
{
	CInfoMathe* pMathe = CInfoMathe::GetField(m_nFile,
											  m_pItem->Element[0].m_nGroup,
											  m_pItem->Element[0].m_nField[0]);

	// insert condition list
	int nIndex = -1;
	for (int nCount = 0; nCount < MATHE_NUM_COND; nCount++)
	{
		if (pMathe->Condition[nCount].m_nNumber != _FREE)
			m_CbCond.InsertString(-1, pMathe->Condition[nCount].m_strName);
		if (pMathe->Condition[nCount].m_nNumber == m_pItem->m_nCondition)
			nIndex = nCount;
    }
    if (nIndex != -1)
		m_CbCond.SetCurSel(nIndex);
    else
    	m_CbCond.ShowDropDown();

	m_editValue.LimitText(11);			// max. 11 digits (singed long)

	// insert group list
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;
	POSITION pos = pConfig->GetHeadGroup();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = pConfig->GetNextGroup(pos, CFG_INDEX_PRINT);
		if (pOpeGroup == NULL)
			break;
		m_CbGroup1.InsertString(-1, pOpeGroup->m_strName);
		m_CbGroup2.InsertString(-1, pOpeGroup->m_strName);
	}

	if (m_pItem->m_nFormat[0] == 0)
	{
		m_bCond1 = UpdateState(1);
		if (m_pItem->Element[1].m_nGroup >= 0)
		{
			m_CbGroup1.SetCurSel(m_pItem->Element[1].m_nGroup);
			RefreshMajor(TRUE, 1);
			RefreshMinor(TRUE, 1);
		}
	}
	else if (m_pItem->m_nFormat[0] == 1)
	{
		m_btnUse.SetCheck(1);
		m_bCond1 = UpdateState(1);

		wchar_t szBuffer[16];
		_ltow(m_pItem->m_lData, szBuffer, 10);
		m_editValue.SetWindowText(szBuffer);
	}

	m_bCond2 = UpdateState(2);
	if (m_pItem->Element[2].m_nGroup >= 0)
	{
		m_CbGroup2.SetCurSel(m_pItem->Element[2].m_nGroup);
		RefreshMajor(TRUE, 2);
		RefreshMinor(TRUE, 2);
	}
}

void CMathePropDlg::RefreshMajor(BOOL bInit, int nLevel)
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

		m_nMajor1 = -1;
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

		m_nMajor2 = -1;
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

	if (nIndex < MATHE_NUM_INDEX)
		m_nIndex[nLevel - 1][nIndex] = -1;	// end of index
	else
		ASSERT(FALSE);
}

void CMathePropDlg::RefreshMinor(BOOL bInit, int nLevel)
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

BOOL CMathePropDlg::UpdateState(int nLevel)
{
	CComboBox* pCb;
	BOOL bState = GetCondLevel(nLevel);

	if (nLevel == 1)
	{
		if (m_CbCond.GetCurSel() != CB_ERR)
			m_editValue.EnableWindow(! bState);
		else
		{
			bState = FALSE;
			m_editValue.EnableWindow(bState);
		}
	}

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

int CMathePropDlg::GetCondLevel(int nLevel)
{
	if (nLevel == 1)
		return (m_btnUse.GetCheck() == 0) ? (TRUE) : (FALSE);
	else
		return (m_CbCond.GetCurSel() > 0) ? (TRUE) : (FALSE);
}

CInfoPrint* CMathePropDlg::GetInfo(int nLevel, int nGroup, int nMajor)
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

int CMathePropDlg::GetValue(LONG* plData)
{
	long lMin, lMax;
	BOOL bState;

	int nData = (int)GetDlgItemInt(IDC_MATHE_VALUE, &bState);
	if (bState)											// integer value ?
	{
		*plData = (LONG)nData;
		return 2;										// integer
	}
	else												// long value ?
	{
		wchar_t *pEnd;
		CString strValue;
		m_editValue.GetWindowText(strValue);
		//errno = EZERO;
		*plData = wcstol(strValue, &pEnd, 10);

		if (*plData != 0L && *pEnd == '\0' && errno != ERANGE)
		{
			if (m_pItem->Element[0].m_nField[0] == 0)
				return 1;								// long integer
			else										// overflow
			{
				lMin = 0xFFFF8001L;
				lMax = 0x00007FFFL;
			}
		}
		else											// overflow
		{
				lMin = 0x80000001L;
				lMax = 0x7FFFFFFFL;
		}
		
	}

	wchar_t szMin[16], szMax[16];
	CString strMsg;
	AfxFormatString2(strMsg, IDS_ERRMSG_RANGE, _ltow(lMin, szMin, 10),
											   _ltow(lMax, szMax, 10));
	AfxMessageBox(strMsg);
	GetDlgItem(IDC_MATHE_VALUE)->SetFocus();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMathePropDlg message handlers

BOOL CMathePropDlg::OnInitDialog()
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	CInfoMathe* pMathe = CInfoMathe::GetField(m_nFile,
											  m_pItem->Element[0].m_nGroup,
											  m_pItem->Element[0].m_nField[0]);
	int nMinor = pMathe->m_nMinor + m_pItem->Element[0].m_nField[1];
	wchar_t szBuffer[16];

	m_strGroup = pConfig->GetGroupName(CFG_INDEX_MATHE, m_pItem->Element[0].m_nGroup);
	m_strMajor = pMathe->m_strName;
	m_strMinor = _itow(nMinor, szBuffer, 10);
	m_strItem  = pMathe->m_strName;

	CDialog::OnInitDialog();

	SetInitData();
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMathePropDlg::OnOK()
{
	BOOL bCond1, bCond2;
	int nMinor1, nMinor2, nItem1, nItem2, nType;
	LONG lValue;

	bCond1  = GetCondLevel(1);
	bCond2  = GetCondLevel(2);
	nMinor1 = m_CbMinor1.GetCurSel();
	nMinor2 = m_CbMinor2.GetCurSel();
	nItem1  = m_CbItem1.GetCurSel();
	nItem2  = m_CbItem2.GetCurSel();
		
	if (! bCond1 && (nType = GetValue(&lValue)) == 0)
		return;

	if ((bCond1 && (m_nGroup1 < 0 || m_nMajor1 < 0 || nMinor1 == CB_ERR ||
													  nItem1  == CB_ERR)) ||
		(bCond2 && (m_nGroup2 < 0 || m_nMajor2 < 0 || nMinor2 == CB_ERR ||
													  nItem2  == CB_ERR)))
	{
		AfxMessageBox(IDS_ERRMSG_COND);
		return;
	}

	CInfoMathe* pMathe = CInfoMathe::GetField(m_nFile,
											  m_pItem->Element[0].m_nGroup,
											  m_pItem->Element[0].m_nField[0]);

	m_pItem->m_nCondition = pMathe->Condition[m_CbCond.GetCurSel()].m_nNumber;

	if (bCond1)
	{
		CInfoPrint* pPrint = GetInfo(1, m_nGroup1, m_nMajor1);
		if (pPrint == NULL)
		{
			ASSERT(FALSE);
			return;
		}
		
		m_pItem->m_nFormat[0]           = 0;
		m_pItem->Element[1].m_nGroup    = m_nGroup1;
		m_pItem->Element[1].m_nField[0] = m_nIndex[0][m_nMajor1];
		m_pItem->Element[1].m_nField[1] = nMinor1;
		m_pItem->Element[1].m_nItem     = nItem1;
	}
	else
	{
		m_pItem->m_nFormat[0]           = 1;
		m_pItem->m_lData                = lValue;
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

	EndDialog(IDOK);
}

void CMathePropDlg::OnSelchangeMatheCond()
{
	BOOL bCond;

	if ((bCond = UpdateState(1)) != m_bCond1)
	{
		m_bCond1  = bCond;
		m_nGroup1 = -1;
		m_nMajor1 = -1;
		m_CbMajor1.ResetContent();
		m_CbMinor1.ResetContent();
		m_CbItem1.ResetContent();
	}

	if ((bCond = UpdateState(2)) != m_bCond2)
	{
		m_bCond2  = bCond;
		m_nGroup2 = -1;
		m_nMajor2 = -1;
		m_CbMajor2.ResetContent();
		m_CbMinor2.ResetContent();
		m_CbItem2.ResetContent();
	}
}

void CMathePropDlg::OnMatheUse()
{
	BOOL bCond;

	if ((bCond = UpdateState(1)) != m_bCond1)
	{
		m_bCond1  = bCond;
		m_nMajor1 = -1;
		m_editValue.SetWindowText(L"");
		m_CbMajor1.ResetContent();
		m_CbMinor1.ResetContent();
		m_CbItem1.ResetContent();
	}
}

void CMathePropDlg::OnSelchangeMatheGroup1()
{
	RefreshMajor(FALSE, 1);
}

void CMathePropDlg::OnSelchangeMatheGroup2()
{
	RefreshMajor(FALSE, 2);
}

void CMathePropDlg::OnSelchangeMatheMajor1()
{
	RefreshMinor(FALSE, 1);
}

void CMathePropDlg::OnSelchangeMatheMajor2()
{
	RefreshMinor(FALSE, 2);
}
