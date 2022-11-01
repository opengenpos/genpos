// SendOpMessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pcsample.h"
#include "SendOpMessageDialog.h"

#include "R20_PC2172.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendOpMessageDialog dialog


CSendOpMessageDialog::CSendOpMessageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSendOpMessageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendOpMessageDialog)
	m_csMessageToSend = _T("");
	m_bNoBroadcast = FALSE;
	m_bDeleteMessages = FALSE;
	m_bForceSignOut = FALSE;
	m_bAutoSignOut = FALSE;
	m_bBlockOperatorSignIn = FALSE;
	m_bUnBlockOperatorSignIn = FALSE;
	m_bForceShutDown = FALSE;
	//}}AFX_DATA_INIT
}


void CSendOpMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendOpMessageDialog)
	DDX_Text(pDX, IDC_EDIT_MSG, m_csMessageToSend);
	DDX_Check(pDX, IDC_CHECK1, m_bNoBroadcast);
	DDX_Check(pDX, IDC_CHECK2, m_bDeleteMessages);
	DDX_Check(pDX, IDC_CHECK3, m_bForceSignOut);
	DDX_Check(pDX, IDC_CHECK4, m_bAutoSignOut);
	DDX_Check(pDX, IDC_CHECK5, m_bBlockOperatorSignIn);
	DDX_Check(pDX, IDC_CHECK6, m_bUnBlockOperatorSignIn);
	DDX_Check(pDX, IDC_CHECK7, m_bForceShutDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendOpMessageDialog, CDialog)
	//{{AFX_MSG_MAP(CSendOpMessageDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSendOpMessageDialog message handlers


void CSendOpMessageDialog::OnOK() 
{
	// TODO: Add extra validation here

	CDialog::OnOK();

	UCHAR  ucFlags[4];

	memset (ucFlags, 0, sizeof(ucFlags));

	if (m_bNoBroadcast)												// if Broadcast protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_NO_BROADCAST, ucFlags);	// set the don't Broadcast flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_NO_BROADCAST, ucFlags);	// unset/clear the don't Broadcast flag

	if (m_bDeleteMessages)												// if the Delete Messages protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_DELETE_MSGS, ucFlags);	// set the Delete Messages flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_DELETE_MSGS, ucFlags);	// unset/clear the Delete Messages flag

	if (m_bForceSignOut)												// if the Force Operator Sign Out protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_FORCE_SIGNOUT, ucFlags);	// set the Force Sign Out flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_FORCE_SIGNOUT, ucFlags);	// unset/clear the Force Sign Out flag

	if (m_bAutoSignOut)												// if the Force Operator Sign Out protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_AUTO_SIGNOUT, ucFlags);	// set the Force Sign Out flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_AUTO_SIGNOUT, ucFlags);	// unset/clear the Force Sign Out flag

	if (m_bBlockOperatorSignIn)												// if the Force Operator Sign Out protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_SIGNIN_BLOCK, ucFlags);	// set the Force Sign Out flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_SIGNIN_BLOCK, ucFlags);	// unset/clear the Force Sign Out flag

	if (m_bUnBlockOperatorSignIn)												// if the Force Operator Sign Out protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_SIGNIN_UNBLOCK, ucFlags);	// set the Force Sign Out flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_SIGNIN_UNBLOCK, ucFlags);	// unset/clear the Force Sign Out flag

	if (m_bForceShutDown)												// if the Force Operator Sign Out protocol was checked off
		SEROPDISPLAY_OPTIONS_SET(SEROPDISPLAY_FORCE_SHUTDOWN, ucFlags);	// set the Force Application Shutdown flag
	else
		SEROPDISPLAY_OPTIONS_UNSET(SEROPDISPLAY_FORCE_SHUTDOWN, ucFlags);	// unset/clear the Force Application Shutdown flag

	memset (myTextBuffer, 0, sizeof(myTextBuffer));

	for (int iLoop = 0; iLoop < 512 && iLoop < m_csMessageToSend.GetLength (); iLoop++) {
		myTextBuffer[iLoop] = m_csMessageToSend[iLoop];
	}

	SerOpDisplayOperatorMessage (ucFlags, myTextBuffer, m_csMessageToSend.GetLength ());

}

/////////////////////////////////////////////////////////////////////////////
// CSendOpMessageDialog message handlers

