/////////////////////////////////////////////////////////////////////////////
// dlgimp.cpp : implementation file
//
// Copyright (C) 1996-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Oct-12-98           O.Nakada     HPUS V3.3
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "progrdoc.h"
#include "dlgimp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportDlg dialog


CImportDlg::CImportDlg(const CString& strPathName, CWnd* pParent /*=NULL*/)
	: CDialog(CImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_strPathName = strPathName;
	m_nReport     = -1;
}

void CImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportDlg)
	DDX_Control(pDX, IDC_IMPORT_IMPORT, m_btnImport);
	DDX_Control(pDX, IDC_IMPORT_LIST, m_LbList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportDlg, CDialog)
	//{{AFX_MSG_MAP(CImportDlg)
	ON_LBN_SELCHANGE(IDC_IMPORT_LIST, OnSelchangeImportList)
	ON_BN_CLICKED(IDC_IMPORT_IMPORT, OnImportImport)
	ON_LBN_DBLCLK(IDC_IMPORT_LIST, OnDblclkImportList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListView implementations

CString CImportDlg::GetReportInfo(int nNumber, int* pnFile)
{
	// caution !!
	static wchar_t szReport[] = L"Report%d";
	static wchar_t szFile[]	  = L"File";
	static wchar_t szDevice[] = L"Device";
	static wchar_t szTitle[]  = L"Title";

	wchar_t szSection[32], szData[128];
	CString strInfo;

	wsprintf(szSection, szReport, nNumber);

	if (::GetPrivateProfileString(szSection, szFile, L"", szData, sizeof(szData), m_strPathName) > 0)
	{
		INFO_FILE* pFile = ptheInfo->GetFile(_wtoi(szData));
		strInfo  = pFile->m_strName;
		strInfo += L"\t";
		*pnFile  = pFile->m_nNumber;
	}
	else
	{
		strInfo.Empty();
		return strInfo;
	}

	if (::GetPrivateProfileString(szSection, szDevice, L"", szData, sizeof(szData), m_strPathName) > 0)
	{
		INFO_DEVICE* pDevice = ptheInfo->GetDevice(_wtoi(szData));
		strInfo += pDevice->m_strName;
		strInfo += L"\t";
	}
	else
	{
		strInfo.Empty();
		return strInfo;
	}

	if (::GetPrivateProfileString(szSection, szTitle, L"", szData, sizeof(szData), m_strPathName) > 0)
		strInfo += szData;

	return strInfo;
}

/////////////////////////////////////////////////////////////////////////////
// CImportDlg message handlers

BOOL CImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// set tab stops in list box.
    int nTabs[2] = {70, 140};
	m_LbList.SetTabStops(2, nTabs);
    
    memset(m_anFile, '\0', sizeof(m_anFile));
	BOOL	fWarning = FALSE;
	
	for (int nCount = 1; nCount <= REPORT_MAX_SHEET; nCount++)
	{
		int		nFile;
		CString strInfo = GetReportInfo(nCount, &nFile);
		if (strInfo.IsEmpty())
			break;
#if	1									// HPUS V3.3 - Migration
		if (nFile == MIGRATE_ID_SERVER)
		{
			fWarning = TRUE;
			continue;
		}
#endif									// HPUS V3.3 - Migration
		m_LbList.InsertString(-1, strInfo);
		m_anFile[m_LbList.GetCount() - 1] = nCount;
	}
	
	m_btnImport.EnableWindow(FALSE);	// disable import button

#if	1									// HPUS V3.3 - Migration
	if (fWarning)
		AfxMessageBox(IDS_ERRMSG_IMP310TO330, MB_OK);
#endif									// HPUS V3.3 - Migration
			
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImportDlg::OnOK()
{
}

void CImportDlg::OnImportImport()
{
	if ((m_nReport = m_LbList.GetCurSel()) != LB_ERR)
	{
		m_nReport = m_anFile[m_nReport];
		EndDialog(IDOK);
	}
	else
		ASSERT(FALSE);
}

void CImportDlg::OnSelchangeImportList()
{
	if (m_LbList.GetCurSel() != LB_ERR)
		m_btnImport.EnableWindow(TRUE);
	else
		m_btnImport.EnableWindow(FALSE);
}

void CImportDlg::OnDblclkImportList()
{
	if ((m_nReport = m_LbList.GetCurSel()) != LB_ERR)
	{
		m_nReport = m_anFile[m_nReport];
		EndDialog(IDOK);
	}
	else
		ASSERT(FALSE);
}
