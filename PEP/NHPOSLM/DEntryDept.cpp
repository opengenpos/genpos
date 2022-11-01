// DEntryDept.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DEntryDept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDEntryDept dialog


CDEntryDept::CDEntryDept(CWnd* pParent /*=NULL*/)
	: CDialog(CDEntryDept::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDEntryDept)
	m_csExtMsg = _T("");
	m_csExtMnemonic = _T("");
	m_csExtFsc = _T("");
	//}}AFX_DATA_INIT
}


void CDEntryDept::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDEntryDept)
	DDX_Control(pDX, IDD_P03_DEPTSPIN, m_sbcExtFsc);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonic);
	DDV_MaxChars(pDX, m_csExtFsc, 4);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		DDX_Text(pDX, IDD_P03_DEPTIN, m_csExtFsc);        // get extended value from control

		// clear the data area used for the department number then copy the
		// department number from the dialog control into our data area.
		memset(myActionUnion.data.PLU, 0, sizeof(myActionUnion.data.PLU));
		LPTSTR pExtFsc = m_csExtFsc.LockBuffer ();
		ASSERT(pExtFsc);
		for (int i = 0; *pExtFsc && i < sizeof(myActionUnion.data.PLU); i++, pExtFsc++) {
			myActionUnion.data.PLU[i] = (UCHAR) *pExtFsc;
		}
		m_csExtFsc.ReleaseBuffer ();
	}
	else {
		m_csExtFsc.Format (_T("%S"), myActionUnion.data.PLU);
		//set the department number edit control with previous department number text if it exists
		DDX_Text(pDX, IDD_P03_DEPTIN, m_csExtFsc);      // set the control with the extended value.
	}
}


BEGIN_MESSAGE_MAP(CDEntryDept, CDialog)
	//{{AFX_MSG_MAP(CDEntryDept)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEntryDept message handlers



BOOL CDEntryDept::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sbcExtFsc.SetRange(myRangeMin,myRangeMax);
	UpdateData();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDEntryGroupTable dialog


CDEntryGroupTable::CDEntryGroupTable(CWnd* pParent /*=NULL*/)
	: CDialog(CDEntryGroupTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDEntryGroupTable)
	m_csExtMsg = _T("");
	m_csExtMnemonic = _T("");
	m_csExtFsc = _T("");
	m_myTableRangeMin = 0;
	m_myTableRangeMax = 2;
	m_myGroupRangeMin = 1;
	m_myGroupRangeMax = 9999;
	m_usOptionFlags = 0;
	//}}AFX_DATA_INIT
}


void CDEntryGroupTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	BOOL  bPriority = FALSE;

	if (!pDX->m_bSaveAndValidate) {
		// data exchange to window controls to display to user
		m_csExtFsc.Format (_T("%d"), myActionUnion.data.group_table.usTableNumber);
		m_csExtFsc2.Format (_T("%d"), myActionUnion.data.group_table.usGroupNumber);
		m_usOptionFlags = myActionUnion.data.group_table.usOptionFlags;
		bPriority = ((m_usOptionFlags & BUTTONACTIONTYPEGROUP_PRIORITY) != 0) ? TRUE : FALSE;
	}

	//{{AFX_DATA_MAP(CDEntryDept)
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonic);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_DEPTIN, m_csExtFsc);
	DDX_Control(pDX, IDD_P03_DEPTSPIN, m_sbcExtFsc);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG2, m_csExtMsg2);
	DDX_Text(pDX, IDD_P03_DEPTIN2, m_csExtFsc2);
	DDX_Control(pDX, IDD_P03_DEPTSPIN2, m_sbcExtFsc2);
	DDV_MaxChars(pDX, m_csExtFsc, 1);
	DDV_MaxChars(pDX, m_csExtFsc2, 4);
	DDX_Check(pDX, IDC_CHECK1, bPriority);
	//}}AFX_DATA_MAP


	if (pDX->m_bSaveAndValidate) {
		// data exchange from window controls to setup our data
		myActionUnion.data.group_table.usTableNumber = _ttoi (m_csExtFsc);
		myActionUnion.data.group_table.usGroupNumber = _ttoi (m_csExtFsc2);
		m_usOptionFlags = 0;

		if (bPriority)
			m_usOptionFlags |= BUTTONACTIONTYPEGROUP_PRIORITY;

		myActionUnion.data.group_table.usOptionFlags = m_usOptionFlags;
	}
}


BEGIN_MESSAGE_MAP(CDEntryGroupTable, CDialog)
	//{{AFX_MSG_MAP(CDEntryDept)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_CHECK20, &CDEntryGroupTable::OnStnClickedCheck20)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEntryDept message handlers



BOOL CDEntryGroupTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sbcExtFsc.SetRange(m_myTableRangeMin,m_myTableRangeMax);
	m_sbcExtFsc2.SetRange(m_myGroupRangeMin,m_myGroupRangeMax);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CDEntryGroupTable::OnStnClickedCheck20()
{
	// TODO: Add your control notification handler code here
}
