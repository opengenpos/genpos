// DEntryAdjPls.cpp : implementation file
//NEW

#include "stdafx.h"
#include "NewLayout.h"
#include "DEntryAdjPls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDEntry dialog


CDEntry::CDEntry(CWnd* pParent /*=NULL*/)
	: CDialog(CDEntry::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CDEntry)
	m_csExtMsg = _T("");
	m_csExtMnemonic = _T("");
	m_csRange = _T("");
	m_csExtFsc = _T("0");
	m_uiExtFsc = 0;
	//}}AFX_DATA_INIT
}


void CDEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDEntry)
	DDX_Control(pDX, IDC_ENTRY_SPIN, m_sbcEntry);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonic);
	DDX_Text(pDX, 65535, m_csRange);
	DDX_Text(pDX, IDD_P03_EXTFSC, m_uiExtFsc);
	DDX_Text(pDX, IDD_P03_EXTFSC, m_csExtFsc);
	DDX_Control(pDX, IDD_P03_EXTFSC, m_cExtFsc);
	//}}AFX_DATA_MAP
	//DDX_Text(pDX, IDD_P03_EXTFSC, myActionUnion.data.extFSC);

	//if (pDX->m_bSaveAndValidate) {
	//	myActionUnion.data.extFSC = m_uiExtFsc;
	//}
	//else {
	//	//m_csExtFsc.Format (_T("%u"), myActionUnion.data.extFSC);
	//	//m_uiExtFsc = myActionUnion.data.extFSC;
	//}
}


BEGIN_MESSAGE_MAP(CDEntry, CDialog)
	//{{AFX_MSG_MAP(CDEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEntry message handlers

		
BOOL CDEntry::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sbcEntry.SetRange(myRangeMin,myRangeMax);		//set range of spinner
	m_sbcEntry.SetPos(myRangeMin);

	//M-KSo m_uiExtFsc = myRangeMin;
	m_uiExtFsc = myActionUnion.data.extFSC == 0 ? myRangeMin : myActionUnion.data.extFSC;
	m_csExtFsc.Format (_T("%d"), m_uiExtFsc);
	m_cExtFsc.SetWindowText(m_csExtFsc);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDEntry::OnOK() 
{
	// TODO: Add extra validation here
	int iErrFlag = FALSE;	//iErrFlag is False if data is in range, True otherwise
	CString csErr;
	CString csErr_Format;

	UpdateData(TRUE);		//retrieve data from dialog

	//validate whether is data is within correct range
	if(m_uiExtFsc > myRangeMax){
		m_uiExtFsc = myRangeMax;
		iErrFlag = TRUE;
	}
	if(m_uiExtFsc < myRangeMin){
		m_uiExtFsc = myRangeMin;
		iErrFlag = TRUE;
	}
	
	//if in range, close dialog
	if(!iErrFlag)
	{
		myActionUnion.data.extFSC = m_uiExtFsc;
		CDialog::OnOK();
	}

	//if not in range, display error message, and update window with min or max value 
	else{
		csErr_Format.LoadString(IDS_RANGE_ERR);
		csErr.Format (csErr_Format,myRangeMin,myRangeMax);
	
		AfxMessageBox(csErr,MB_OK,0);
		UpdateData(FALSE);
	}

}
