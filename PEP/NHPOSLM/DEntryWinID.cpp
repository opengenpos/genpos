// DEntryWinID.cpp : implementation file
//
// This file cloned and modified from DEntryPLU.cpp
// Defines the popup dialogue for entering a Window ID
// Used in button creation in the layout manager
// Specifically created to satisfy the needs of two new FSC codes:
// FSC_WINDOW_DISPLAY and FSC_WINDOW_DISMISS

#include "stdafx.h"
#include "NewLayout.h"
#include "DEntryWinID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDEntryWinID dialog


CDEntryWinID::CDEntryWinID(CWnd* pParent /*=NULL*/)
	: CDialog(CDEntryWinID::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDEntryWinID)
	m_csExtFsc = _T("");
	m_csLabelType = _T("");
	m_iExtFSC = 1;
	//}}AFX_DATA_INIT
}


void CDEntryWinID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDEntryWinID)
	DDV_MaxChars(pDX, m_csExtFsc, MaxSizeOfWinID);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonics);
	DDX_Text(pDX, IDD_P03_EXTFSC, m_iExtFSC);
	//}}AFX_DATA_MAP
	

	if (pDX->m_bSaveAndValidate) {
		DDX_Text(pDX, IDD_P03_EXTFSC, m_csExtFsc);        // get extended value from control

		// clear the data area used for the window id number then copy the
		// window id number from the dialog control into our data area.
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
		//set the window id edit control with previous window id text if it exists
		DDX_Text(pDX, IDD_P03_EXTFSC, m_csExtFsc);      // set the control with the extended value.
	}
}


BEGIN_MESSAGE_MAP(CDEntryWinID, CDialog)
	//{{AFX_MSG_MAP(CDEntryWinID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEntryWinID message handlers

BOOL CDEntryWinID::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDEntryWinID::OnOK() 
{	// TODO  any kind of necessary validation for the IDs
	// before accepting them into the system
	// see the validation code in the analogous function of DEntryPLU.cpp for hints
	CDialog::OnOK();
}
