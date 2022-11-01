// ModeEntry.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "ModeEntry.h"
#include "P003.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModeEntry dialog


CModeEntry::CModeEntry(CWnd* pParent /*=NULL*/)
	: CDialog(CModeEntry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModeEntry)
	m_csExtMsg = _T("");
	m_csExtMnemonic = _T("");
	m_iLabel = 1;
	//}}AFX_DATA_INIT
}


void CModeEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModeEntry)
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonic);
	DDX_CBIndex(pDX, IDD_P03_LABEL, (int &)this->myActionUnion.data.extFSC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModeEntry, CDialog)
	//{{AFX_MSG_MAP(CModeEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModeEntry message handlers

