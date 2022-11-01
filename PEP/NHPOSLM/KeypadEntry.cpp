// KeypadEntry.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "KeypadEntry.h"
#include "P003.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeypadEntry dialog


CKeypadEntry::CKeypadEntry(CWnd* pParent /*=NULL*/)
	: CDialog(CKeypadEntry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeypadEntry)
	m_csExtMsg = _T("");
	m_csExtMnemonic = _T("");
	m_iLabel = -1;
	//}}AFX_DATA_INIT
}


void CKeypadEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeypadEntry)
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonic);
	DDX_CBIndex(pDX, IDD_P03_LABEL, (int &)this->myActionUnion.data.extFSC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeypadEntry, CDialog)
	//{{AFX_MSG_MAP(CKeypadEntry)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeypadEntry message handlers
