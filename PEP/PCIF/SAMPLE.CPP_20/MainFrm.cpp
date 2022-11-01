/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  MainFrm.cpp
//
//  PURPOSE:    Provides PCSample Main Frame Window class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"


#include "Total.h"
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"

#include "PCSample.h"
#include "PCSampleDoc.h"
#include "MainFrm.h"

#include "LogonDlg.h"
#include "ModemDlg.h"
#include "PasswordDlg.h"
#include "DateTimeDlg.h"
#include "StoreNoDlg.h"
#include "TerminalNoDlg.h"
#include "LanDlg.h"
#include "SavedTotal.h"
#include "SendOpMessageDialog.h"

#include "R20_RMT.h"
#include "R20_pif.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//          D Y N A M I C    O B J E C T    C R E A T I O N
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CONNECT_LOGON, OnConnectLogon)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOGON, OnUpdateConnectLogon)
	ON_COMMAND(ID_CONNECT_LOGOFF, OnConnectLogoff)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOGOFF, OnUpdateConnectLogoff)
	ON_COMMAND(ID_CONNECT_LOCK_KBD, OnConnectLockKbd)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCK_KBD, OnUpdateConnectLockKbd)
	ON_COMMAND(ID_CONNECT_UNLOCK_KBD, OnConnectUnlockKbd)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_UNLOCK_KBD, OnUpdateConnectUnlockKbd)
	ON_COMMAND(ID_CONNECT_MODEM_ON, OnConnectModemOn)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_MODEM_ON, OnUpdateConnectModemOn)
	ON_COMMAND(ID_CONNECT_MODEM_OFF, OnConnectModemOff)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_MODEM_OFF, OnUpdateConnectModemOff)
	ON_COMMAND(ID_CONECT_LAN_OFF, OnConectLanOff)
	ON_UPDATE_COMMAND_UI(ID_CONECT_LAN_OFF, OnUpdateConectLanOff)
	ON_COMMAND(ID_CONNECT_LAN_ON, OnConnectLanOn)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LAN_ON, OnUpdateConnectLanOn)
	ON_COMMAND(ID_CONNECT_CHG_PASSWORD, OnConnectChgPassword)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_CHG_PASSWORD, OnUpdateMenuElement)
	ON_COMMAND(ID_PARA_CONFIG, OnParaConfig)
	ON_UPDATE_COMMAND_UI(ID_PARA_CONFIG, OnUpdateMenuElement)
	ON_COMMAND(ID_PARA_SET_DATE, OnParaSetDate)
	ON_UPDATE_COMMAND_UI(ID_PARA_SET_DATE, OnUpdateMenuElement)
	ON_COMMAND(ID_PARA_SET_REGNO, OnParaSetRegno)
	ON_UPDATE_COMMAND_UI(ID_PARA_SET_REGNO, OnUpdateMenuElement)
	ON_COMMAND(ID_READ_CPM, OnReadCpmTally)
	ON_UPDATE_COMMAND_UI(ID_READ_CPM, OnUpdateMenuElement)
	ON_COMMAND(ID_READ_EJ, OnReadEJ)
	ON_UPDATE_COMMAND_UI(ID_READ_EJ, OnUpdateMenuElement)
	ON_COMMAND(ID_RESET_EJ, OnResetEJ)
	ON_UPDATE_COMMAND_UI(ID_RESET_EJ, OnUpdateMenuElement)
	ON_COMMAND(ID_RESET_EJ_OVERRIDE, OnResetEJOverride)
	ON_UPDATE_COMMAND_UI(ID_RESET_EJ_OVERRIDE, OnUpdateMenuElement)
	ON_COMMAND(ID_RESET_EJ_NONOVERRIDE, OnResetEJNonOverride)
	ON_UPDATE_COMMAND_UI(ID_RESET_EJ_NONOVERRIDE, OnUpdateMenuElement)
	ON_COMMAND(ID_RESET_CPM, OnResetCpmTally)
	ON_UPDATE_COMMAND_UI(ID_RESET_CPM, OnUpdateMenuElement)
	ON_COMMAND(ID_RESET_EOD, OnResetEOD)
	ON_UPDATE_COMMAND_UI(ID_RESET_EOD, OnUpdateMenuElement)
	ON_COMMAND(ID_RCVDEBGLOG, OnRcvDebgLog)
	ON_UPDATE_COMMAND_UI(ID_RCVDEBGLOG, OnUpdateMenuElement)
	ON_COMMAND(ID_RCVASSRTLOG, OnRcvAssrtLog)
	ON_UPDATE_COMMAND_UI(ID_RCVASSRTLOG, OnUpdateMenuElement)
	ON_COMMAND(ID_DELASSRTLOG, OnDelAssrtLog)
	ON_UPDATE_COMMAND_UI(ID_DELASSRTLOG, OnUpdateMenuElement)
	ON_COMMAND(ID_RCVLAYOUT, OnRcvLayout)
	ON_UPDATE_COMMAND_UI(ID_RCVLAYOUT, OnUpdateMenuElement)
	ON_COMMAND(ID_RCVLGOFILE, OnRcvLgoFile)
	ON_UPDATE_COMMAND_UI(ID_RCVLGOFILE, OnUpdateMenuElement)
	ON_COMMAND(ID_RCVLOGFILE, OnRcvLogFile)
	ON_UPDATE_COMMAND_UI(ID_RCVLOGFILE, OnUpdateMenuElement)
	ON_COMMAND(ID_EOD_DB, OnEodDb)
	ON_UPDATE_COMMAND_UI(ID_EOD_DB, OnUpdateMenuElement)
	ON_COMMAND(IDD_RECIEVE_SAV_TTL, OnRecieveSavTtl)
	ON_UPDATE_COMMAND_UI(IDD_RECIEVE_SAV_TTL, OnUpdateMenuElement)
	ON_COMMAND(ID_READ_SAV_TOTAL, OnReadSavTotal)
//	ON_UPDATE_COMMAND_UI(ID_READ_SAV_TOTAL, OnUpdateMenuElement)    // allow use of reading SavedTotal file at any time.
	ON_COMMAND(ID_SEND_OP_MESSAGE, OnSendOpMessage)
	ON_UPDATE_COMMAND_UI(ID_SEND_OP_MESSAGE, OnUpdateMenuElement)
	ON_UPDATE_COMMAND_UI(ID_PARA_UPDATE_PLU, OnUpdateMenuElement)
	ON_COMMAND(ID_INSERT_EJ_NOTE, OnInsertEjNote)
	ON_UPDATE_COMMAND_UI(ID_INSERT_EJ_NOTE, OnUpdateMenuElement)
	ON_UPDATE_COMMAND_UI(ID_PROV_CASHIER, OnUpdateMenuElement)
	ON_UPDATE_COMMAND_UI(ID_BCAST_SUP_AC_PARMS, OnUpdateMenuElement)
	ON_UPDATE_COMMAND_UI(ID_BCAST_ALL_P_AC_PARMS, OnUpdateMenuElement)
	ON_UPDATE_COMMAND_UI(ID_BCAST_PLU_OEP_PARMS, OnUpdateMenuElement)
	ON_COMMAND(ID_SELECTFROMACTIVITYLIST, OnSelectFromActivityList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//          S T A T I C    V A R I A B L E s
//
/////////////////////////////////////////////////////////////////////////////

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CMainFrame::CMainFrame()
//
//  PURPOSE :   Constructor of main frame class.
//
//  RETURN :    No return value.
//
//===========================================================================

CMainFrame::CMainFrame()
{
    m_strModemCommand = "";
    m_strPhoneNumber = "";
    m_nPhoneType = 0;
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::~CMainFrame()
//
//  PURPOSE :   Destructor of main frame class.
//
//  RETURN :    No return value.
//
//===========================================================================

CMainFrame::~CMainFrame()
{
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnCreate()
//
//  PURPOSE :   Create main frame window.
//
//  RETURN :    0   - Continue to create window.
//              -1  - Destroy to this window.
//
//===========================================================================

int CMainFrame::OnCreate(
    LPCREATESTRUCT  lpCreateStruct  // points to creation information
    )
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::PreCreateWindow()
//
//  PURPOSE :   Called by frame work before the creation of window.
//
//  RETURN :    TRUE    - Window creation is successful.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CMainFrame::PreCreateWindow(
    CREATESTRUCT& cs            // initialization parameters
    )
{
	return CFrameWnd::PreCreateWindow(cs);
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::AssertValid()
//
//  PURPOSE :   Perform Validity check of this object.
//
//  RETURN :    No return value.
//
//===========================================================================

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
#endif

//===========================================================================
//
//  FUNCTION :  CMainFrame::Dump()
//
//  PURPOSE :   Dump the contents of this object.,
//
//  RETURN :    No return value.
//
//===========================================================================

#ifdef _DEBUG
void CMainFrame::Dump( CDumpContext& dc ) const
{
    // --- Dump the contect ob this object ---
	CFrameWnd::Dump(dc);
}
#endif

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLogon()
//
//  PURPOSE :   Log On to the specified 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectLogon()
{
    ASSERT( ! IsLoggedOn());

    // --- show terminal# selection dialog on screen ---
    CLogonDlg   dlg( 1, m_Serial );

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc = GetMyDocument();

        // --- get serial port configuration ---
        m_Serial = dlg.m_infoSerial;

        // --- open specified serial port ---
        if ( m_PcIfPort.Open( m_Serial ))
        {
            // --- attempt to log on the selected terminal ---
            if ( m_Terminal.LogOn( dlg.m_bTerminalNo, dlg.m_strPassword ))
            {
                // --- report log on is successful ---
                pDoc->ReportLogOn( dlg.m_bTerminalNo );
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());

                // --- close opened serial port ---
                m_PcIfPort.Close();
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_PORTOPEN, m_PcIfPort.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLogoff()
//
//  PURPOSE :   Log Off to the specified 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectLogoff() 
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc = GetMyDocument();

    // --- get current logged on terminal# ---
    // --- attempt to log off from 2170 terminal ---
    USHORT  usTerminalNo = GetLoggedOnTerminalNo();
    if ( m_Terminal.LogOff())
    {
        // --- report log off is successful ---
        pDoc->ReportLogOff( usTerminalNo );

		//closes lan port, if it is connected
		if( ! m_PcIfPort.CloseEx(PCIF_FUNC_CLOSE_LAN))
		{	
			pDoc->ReportError( 0, pcsample::ERR_PORTCLOSE, m_PcIfPort.GetLastError());
		}
			
		// --- close opened serial port, before exit this function ---   
		if ( ! m_PcIfPort.Close())
		{
			// --- function is failed, and report its error ---
			pDoc->ReportError( 0,
            pcsample::ERR_PORTCLOSE,
            m_PcIfPort.GetLastError());
		}
	}
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_LOGOFF, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLanOn()
//
//  PURPOSE :   Log On to the specified 2172 terminal by LAN.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectLanOn() 
{
	// TODO: Add your command handler code here
	ASSERT(! IsLoggedOn());

	CLanDlg	dlg( 1, m_Serial );

	if(dlg.DoModal() == IDOK)
	{
		
		CPCSampleDoc* pDoc = GetMyDocument();

		pDoc->m_SaveResetDataToDatabase = dlg.m_SaveResetDataToDatabase;
		pDoc->m_strDbFileName = dlg.m_strDbFileName;

		// --- open specified ---
		if(m_PcIfPort.OpenEx(PCIF_FUNC_OPEN_LAN) == FALSE)
		{
			// --- function is failed, and report its error
			pDoc->ReportError(0, pcsample::ERR_PORTOPEN, m_PcIfPort.GetLastError());
			m_PcIfPort.CloseEx(PCIF_FUNC_OPEN_LAN);
		}
		else
		{
            if (dlg.m_HostConnectType == CLanDlg::HostConnectHost )  //Connect with Host is selected
			{
				if (m_Terminal.HostLogon(dlg.m_strHostName, dlg.m_strPassword, dlg.m_bTerminalNum))
				{
					// --- report log on is successful ---
					pDoc->ReportLogOn( dlg.m_bTerminalNum );
				}
				else 
				{
					// --- function is failed, and report its error ---
					pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());

					// --- close opened serial port ---
					m_PcIfPort.CloseEx(PCIF_FUNC_CLOSE_LAN);
				}
			}
			else
			{
				// --- attempt to log on the selected terminal ---
				//pulls the IP from the dialog box, and sends it to the LogOn function to log onto a LAN
				if ( m_Terminal.LogOn( dlg.m_bIPAddress1, dlg.m_bIPAddress2, dlg.m_bIPAddress3,  dlg.m_bTerminalNo, dlg.m_strPassword, dlg.m_bTerminalNum ))
				{
					// --- report log on is successful ---
					pDoc->ReportLogOn( dlg.m_bTerminalNum );
				}
				else
				{
					// --- function is failed, and report its error ---
					pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());

					// --- close opened serial port ---
					m_PcIfPort.CloseEx(PCIF_FUNC_CLOSE_LAN);
				}
			}
		}
	}
}


//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLanOff()
//
//  PURPOSE :   Log Off of the specified 2172 terminal by .
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConectLanOff() 
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc = GetMyDocument();

    // --- attempt to log off from 2170 terminal ---
	USHORT usTerminalNo = GetLoggedOnTerminalNo();
    if ( m_Terminal.LogOff())
    {
        // --- close opened serial port, before exit this function ---
        if ( ! m_PcIfPort.CloseEx(PCIF_FUNC_CLOSE_LAN))
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_LOGOFF, m_PcIfPort.GetLastError());
        }
		else
		{
			pDoc->ReportLogOff(usTerminalNo);
		}
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_LOGOFF, m_Terminal.GetLastError());
    }
	
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectModemOn()
//
//  PURPOSE :   Log On to the specified 2170 terminal by Modem.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectModemOn() 
{
    ASSERT( ! IsLoggedOn());

    // --- show terminal# selection dialog on screen ---
    CLogonDlg   dlg( 1, m_Serial );

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc;
        pDoc = GetMyDocument();

        // --- get serial port configuration ---
        m_Serial = dlg.m_infoSerial;

        // --- open specified serial port ---
        if ( m_PcIfPort.Open( m_Serial ))
        {
            // --- show terminal# selection dialog on screen ---
            CModemDlg   Modemdlg( m_strModemCommand, m_strPhoneNumber, m_nPhoneType );

            if ( Modemdlg.DoModal() == IDOK )
            {
                wait.Restore();

                // --- get serial port configuration ---
                m_strModemCommand = Modemdlg.m_strCommand;
                m_strPhoneNumber  = Modemdlg.m_strPhone;
                m_nPhoneType      = Modemdlg.m_nPulse;

                // --- open specified serial port ---
                if ( m_PcIfPort.ConnectModem( m_strModemCommand, m_strPhoneNumber, m_nPhoneType ))
                {
                    // --- attempt to log on the selected terminal ---
                    if ( m_Terminal.LogOn( dlg.m_bTerminalNo, dlg.m_strPassword ))
                    {
                        // --- report log on is successful ---
                        pDoc->ReportLogOn( dlg.m_bTerminalNo );
                    }
                    else
                    {
                        // --- function is failed, and report its error ---
                        pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());

                        // --- close opened serial port ---
                        m_PcIfPort.DisconnectModem();
                        m_PcIfPort.Close();
                    }
                }
                else
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());
                    m_PcIfPort.Close();
                }
            }
            else
            {
                m_PcIfPort.Close();
            }
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_PORTOPEN, m_PcIfPort.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectModemoff()
//
//  PURPOSE :   Log Off to the specified 2170 terminal by Modem.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectModemOff() 
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- get current logged on terminal# ---
    USHORT  usTerminalNo;
    usTerminalNo = GetLoggedOnTerminalNo();

    // --- attempt to log off from 2170 terminal ---

    if ( m_Terminal.LogOff())
    {
        // --- report log off is successful ---
        pDoc->ReportLogOff( usTerminalNo );

        // --- close opened serial port, before exit this function ---
        if ( ! m_PcIfPort.DisconnectModem())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_PORTCLOSE, m_PcIfPort.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_LOGOFF, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLockKbd()
//
//  PURPOSE :   Lock keyboard on the logged on terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectLockKbd() 
{
    ASSERT( IsLoggedOn() && ( ! IsKeyboardLocked() ));

    CWaitCursor wait;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- attempt to lock keyboard ---
    if ( m_Terminal.LockKeyboard())
    {
        // --- report keyboard is locked ---
        pDoc->ReportLockKbd( TRUE );
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_LOCK_TERMINAL, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectLockKbd()
//
//  PURPOSE :   Lock keyboard on the logged on terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectUnlockKbd() 
{
    ASSERT( IsLoggedOn() && IsKeyboardLocked());

    CWaitCursor wait;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- attempt to unlock keyboard ---
    if ( m_Terminal.UnlockKeyboard())
    {
        // --- report keyboard is unlocked ---
        pDoc->ReportLockKbd( FALSE );
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_UNLOCK_TERMINAL, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnConnectChgPassword()
//
//  PURPOSE :   Change PC Interface password.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnConnectChgPassword() 
{
    ASSERT( IsLoggedOn());

    // --- show new password entry dialog on screen ---
    CPasswordDlg    dlg;

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc;
        pDoc = GetMyDocument();

        // --- attempt to change PC Interface password ---
        if ( m_Terminal.ChangePassword( dlg.m_strPassword ))
        {
            // --- report password is changed ---
            pDoc->ReportChangePassword();
        }
        else
        {
            // --- Function is failed, and report its error code ---
            pDoc->ReportError( 0, pcsample::ERR_CHGPASSWORD, m_Terminal.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnParaConfig()
//
//  PURPOSE :   Retrieve system parameter of 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnParaConfig()
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- attempt to read system parameter ---
    if ( m_Terminal.ReadSystemParameter())
    {
        // --- get current system date/time ---
        DATE    thisDate;
        GetCurrentDateTime( thisDate );

        // --- report header and body of system parameter report ---
        pDoc->ReportHeader( m_Terminal, thisDate );
        pDoc->ReportConfig( m_Terminal );
    }
    else
    {
        // --- function is failed, report its error ---
        pDoc->ReportError( 0, pcsample::ERR_ISP, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnParaSetDate()
//
//  PURPOSE :   Set new date and time to 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnParaSetDate()
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    CLIDATETIME thisDate;

    // --- show current date and time entry dialog on screen ---
    if ( m_Terminal.ReadDateTime( thisDate ))
    {
        CDateTimeDlg    dlg( thisDate );

        if ( dlg.DoModal() == IDOK )
        {
            wait.Restore();

            // --- get inputed date and time ---
            CLIDATETIME newDate;

            newDate.usYear  = dlg.m_sYear;
            newDate.usMonth = static_cast< USHORT >( dlg.m_nMonth + 1 );
            newDate.usMDay  = dlg.m_sDay;
            newDate.usWDay  = static_cast< USHORT >( dlg.m_nWeekDay );
            newDate.usHour  = dlg.m_sHour;
            newDate.usMin   = dlg.m_sMinute;
            newDate.usSec   = 0;

            // --- write new date and time to 2170 terminal ---
            if ( m_Terminal.WriteDateTime( newDate ))
            {
                // --- report new date/time is written ---
                pDoc->ReportSetDateTime( thisDate, newDate );
            }
            else
            {
                // --- function is failed, and report its error
                pDoc->ReportError( 0, pcsample::ERR_ISP, m_Terminal.GetLastError());
            }
        }
    }
    else
    {
        // --- function is failed, and report its error
        pDoc->ReportError( 0, pcsample::ERR_ISP, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnParaSetRegNo()
//
//  PURPOSE :   Set new store/register# to 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnParaSetRegno()
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc = GetMyDocument();

    CParaStoreNo    paraStoreNo;

    // --- read current store and register# in 2170 ---
    if ( paraStoreNo.Read())
    {
        USHORT  usStoreNo = paraStoreNo.getStoreNo();
        USHORT  usRegNo = paraStoreNo.getRegNo();

        // --- show store/register# entry dialog on screen ---
        CStoreNoDlg dlg( usStoreNo, usRegNo );

		if ( dlg.DoModal() == IDOK && dlg.m_sChangeConfirmed )
        {
            wait.Restore();

            paraStoreNo.setStoreNo( dlg.m_sStoreNo    );
            paraStoreNo.setRegNo(   dlg.m_sTerminalNo );

            // --- lock keyborad of 2170 before write parameter ---
            if ( m_Terminal.LockKeyboard())
            {
                // --- write new stor/register# in 2170 ---
                if ( paraStoreNo.Write())
                {
                    // --- report new store/register# is written ---
                    pDoc->ReportSetStoreNo( usStoreNo, usRegNo, dlg.m_sStoreNo, dlg.m_sTerminalNo );
                }
                else
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLWRITE, paraStoreNo.GetLastError());
                }

                // --- unlock locked keyboard before exit function ---
                if ( ! m_Terminal.UnlockKeyboard())
                {
                    // --- function is failed, and report its error ---
                    pDoc->ReportError( 0, pcsample::ERR_UNLOCK_TERMINAL, m_Terminal.GetLastError());
                }
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( 0, pcsample::ERR_LOCK_TERMINAL, m_Terminal.GetLastError());
            }
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraStoreNo.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnReadCpmTally()
//
//  PURPOSE :   Read CPM/EPT tally in 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnReadCpmTally()
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc = GetMyDocument();

    CTallyCPM   tallyCPM;

    // --- read CPM/EPT tally in 2170 terminal ---
    if ( tallyCPM.Read())
    {
        DATE    thisDate;
        GetCurrentDateTime( thisDate );

        // --- report header and body of CPM/EPT tally report ---
        pDoc->ReportHeader( tallyCPM, thisDate );
        pDoc->ReportTotal ( tallyCPM );

        // --- print reset report log on journal paper ---
        PrintOnJournal( AC_TALLY_CPM_EPT );
        PrintOnJournal( CString( "CPM/EPT TALLY READ" ));
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_RPTCPMTALLY, pcsample::ERR_TTLREAD, tallyCPM.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnResetCpmTally()
//
//  PURPOSE :   Reset CPM/EPT tally in 2170 terminal.
//
//  RETURN :    No return value.
//
//===========================================================================
void CMainFrame::OnResetCpmTally() 
{
    ASSERT( IsLoggedOn());

    CWaitCursor wait;

    CPCSampleDoc* pDoc = GetMyDocument();

    // --- lock keyboard on 2170 before reset tally ---
    if ( m_Terminal.LockKeyboard())
    {
        CTallyCPM   tallyCPM;

        // --- attempt to read and reset CPM/EPT tally ---
        if ( tallyCPM.ReadAndReset())
        {
            DATE    thisDate;
            GetCurrentDateTime( thisDate );

            // --- report header and body of CPM/EPT tally ---
            pDoc->ReportHeader( tallyCPM, thisDate );
            pDoc->ReportTotal ( tallyCPM );

            // --- print reset report log on journal paper ---
            PrintOnJournal( AC_TALLY_CPM_EPT );
            PrintOnJournal( CString( "CPM/EPT TALLY RESET" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTCPMTALLY, pcsample::ERR_TTLREAD, tallyCPM.GetLastError());
        }

        // --- unlock locked keyboard before exit function ---
        if ( ! m_Terminal.UnlockKeyboard())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_UNLOCK_TERMINAL, m_Terminal.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( 0, pcsample::ERR_LOCK_TERMINAL, m_Terminal.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnReadEJ()
//
//  PURPOSE :   Read Electronic Journal in keyboard locked 2170.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnReadEJ()
{
    ASSERT( IsLoggedOn());

    // --- show terminal# entry dialog on screen ---
    CTerminalNoDlg  dlg;
	if(m_PcIfPort.IsLanConnected())
	{
		dlg.m_bTerminalNo = (UCHAR) theApp.GetProfileInt(_T(""),_T("IP4"), 0);
	}
	
    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc = GetMyDocument();

        // --- store current logged on terminal# ---
        USHORT  usThisTerminal = GetLoggedOnTerminalNo();

        CReportEJ   reportEJ;

        // --- log on terminal and lock its keyboard ---

        //if ( LogOnAndLockTerminal( dlg.m_bTerminalNo ))
		// Locking the terminal will fail if an operator is logged in, and
		// this will cause the assert in Terminal.cpp function UnlockKeyboard()
		// to fail. As far as I know there's no need to lock the terminal, so we
		// will simply log on as in the OnResetEJ(Non)Override functions. JV 1/08
        if ( m_Terminal.LogOn( dlg.m_bTerminalNo ))
        {
            // --- read and report EJ in locked terminal ---
            ReadLockedEJ( reportEJ );

            // --- print reset report log on journal paper ---
            PrintOnJournal( AC_EJ_READ_RPT );
            PrintOnJournal( CString( "E/J READ" ));
        }

        // --- is the previous terminal the same as current ? ---
        if ( dlg.m_bTerminalNo == usThisTerminal )
        {
            // --- unlock locked keyboard before exit function ---
            if ( ! m_Terminal.UnlockKeyboard())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_UNLOCK_TERMINAL, m_Terminal.GetLastError());
            }
        }
        else
        {
            // --- restore log on to previous logged on terminal ---
            if ( ! m_Terminal.LogOn( usThisTerminal ))
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
            }
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnResetEJ()
//
//  PURPOSE :   Reset Electronic Journal in keyboard locked 2170.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnResetEJ() 
{
    ASSERT( IsLoggedOn());

    // --- show terminal# entry dialog on screen ---
    CTerminalNoDlg  dlg;

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc = GetMyDocument();

        // --- get current logged on terminal# ---
        USHORT  usThisTerminal = GetLoggedOnTerminalNo();

        CReportEJ   reportEJ;

        // --- log on and lock terminal, and then read EJ ---
        if (( LogOnAndLockTerminal( dlg.m_bTerminalNo )) && ( ReadLockedEJ( reportEJ )))
        {
            // --- reset EJ in locked terminal ---
            if ( ! reportEJ.Reset())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_RESET, reportEJ.GetLastError());
            }

            // --- print reset report log on journal paper ---
            PrintOnJournal( AC_EJ_RESET_RPT );
            PrintOnJournal( CString( "E/J RESET" ));
        }

        // --- is the previous terminal the same as current ? ---
        if ( dlg.m_bTerminalNo == usThisTerminal )
        {
            // --- unlock locked keyboard before exit function ---
            if ( ! m_Terminal.UnlockKeyboard())
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_UNLOCK_TERMINAL, m_Terminal.GetLastError());
            }
        }
        else
        {
            // --- restore log on to previous logged on terminal ---
            if ( ! m_Terminal.LogOn( usThisTerminal ))
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
            }
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnResetEJOverride()
//
//  PURPOSE :   Reset Override Electronic Journal in unlocked 2170.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnResetEJOverride() 
{
    ASSERT( IsLoggedOn());

    // --- show terminal# entry dialog on screen ---
    CTerminalNoDlg  dlg;

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc;
        pDoc = GetMyDocument();

        // --- get current logged on terminal# ---
        USHORT  usThisTerminal;
        usThisTerminal = GetLoggedOnTerminalNo();

        CReportEJ   reportEJ;

        // --- attept to log on the selected terminal ---
        if ( m_Terminal.LogOn( dlg.m_bTerminalNo ))
        {
            // --- read and reset override EJ ----
			reportEJ.m_usTerminalNo = usThisTerminal;
			reportEJ.m_usLineNo = 1;
            ReadAndResetEJ( reportEJ, TRUE );

            // --- print reset report log on journal printer ---
            PrintOnJournal( AC_EJ_RESET_RPT );
            PrintOnJournal( CString( "E/J RESET" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
        }

        // --- restore log on to previous logged on terminal ---
        if ( ! m_Terminal.LogOn( usThisTerminal ))
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnResetEJNonOverride()
//
//  PURPOSE :   Reset Non-Override Electronic Journal in unlocked 2170.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnResetEJNonOverride() 
{
    ASSERT( IsLoggedOn());

    // --- show terminal# entry dialog on screen ---
    CTerminalNoDlg  dlg;

    if ( dlg.DoModal() == IDOK )
    {
        CWaitCursor wait;

        CPCSampleDoc* pDoc;
        pDoc = GetMyDocument();

        // --- get current logged on terminal# ---
        USHORT  usThisTerminal;
        usThisTerminal = GetLoggedOnTerminalNo();

        CReportEJ   reportEJ;

        // --- attept to log on the selected terminal ---
        if ( m_Terminal.LogOn( dlg.m_bTerminalNo ))
        {
            // --- read and reset non-override EJ ----
			reportEJ.m_usTerminalNo = usThisTerminal;
			reportEJ.m_usLineNo = 1;
            ReadAndResetEJ( reportEJ, FALSE );

            // --- print reset report log on journal printer ---
            PrintOnJournal( AC_EJ_RESET_RPT );
            PrintOnJournal( CString( "E/J RESET" ));
        }
        else
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
        }

        // --- restore log on to previous logged on terminal ---
        if ( ! m_Terminal.LogOn( usThisTerminal ))
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_LOGON, m_Terminal.GetLastError());
        }
    }
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnResetEOD()
//
//  PURPOSE :   Perform End Of Day Reset Report.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnResetEOD() 
{
    CWaitCursor wait;

    CPCSampleDoc*   pDoc;
    pDoc = GetMyDocument();

    // --- lock all of operators to log on ---
    COperatorLock   Operator;

    if ( Operator.Lock( pcterm::LOCK_ALL_OPERATOR ))
    {
        CReportEOD  reportEOD( this, pDoc );

        // --- determine all of total is able to reset ---
        if ( reportEOD.IsResetOK())
        {
            // --- reset all of total and then report it ---
            reportEOD.Reset();

			SYSTEMTIME sysTime;
			GetSystemTime(&sysTime);
			pDoc->csSheetNamePrefix.Format (_T("EOD_%4.4d%2.2d%2.2d_%2.2d%2.2d%2.2d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay,
					sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

            reportEOD.Read();

			pDoc->csSheetNamePrefix.Empty();

            // --- print reset report log on journal paper ---
            PrintOnJournal( AC_EOD_RPT );
            PrintOnJournal( CString( "EOD RESET" ));
        }
        else
        {
			// report which total objects had problems and the status code of the reset check.
			// we are now going through all total objects to determine their status rather than
			// just failing on the first one with an error.
			for (INT nI = CReportEOD::TTL_BEGIN; nI < CReportEOD::TTL_END; nI++ ) {
				if (reportEOD.m_sTotalIsResetLastError[nI]) {
					pDoc->ReportError( reportEOD.GetLastErrorLocus(nI), pcsample::ERR_ISSUED,  reportEOD.m_sTotalIsResetLastError[nI]);
				}
			}

            // --- function is failed, and report its error ---
//            pDoc->ReportError( reportEOD.GetLastErrorLocus(), pcsample::ERR_ISSUED,  reportEOD.GetLastError());
        }

        // --- unlock all of operators before exit function ---
        if ( ! Operator.Unlock())
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLUNLOCK, Operator.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLLOCK, Operator.GetLastError());
    }
}

// ID_INSERT_EJ_NOTE, OnInsertEjNote
void CMainFrame::OnInsertEjNote() 
{
	InsertEjNote( CString( "INSERTED NOTE USING #3" ));
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::LogOnAndLockTerminal()
//
//  PURPOSE :   Log on terminal and the lock keyboard.
//
//  RETURN :    TRUE    - Log on and lock keyboard is successful.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CMainFrame::LogOnAndLockTerminal( const USHORT usTerminalNo )
{
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= MAX_TERMINAL_NO ));

    BOOL    fSuccess;

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- attempt to log on specified terminal ---
    if ( m_Terminal.LogOn( usTerminalNo ))
    {
        // --- lock keyboard on logged on terminal ---
        fSuccess = m_Terminal.LockKeyboard();
        if ( ! fSuccess )
        {
            // --- function is failed, and report its error ---
            pDoc->ReportError( 0, pcsample::ERR_LOCK_TERMINAL, m_Terminal.GetLastError());

            // --- log off terminal before exit function ---
            m_Terminal.LogOff();
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        fSuccess = FALSE;
        pDoc->ReportError( 0, pcsample::ERR_LOGON, m_Terminal.GetLastError());
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::ReadLockedEJ()
//
//  PURPOSE :   Read and report EJ in locked terminal.
//
//  RETURN :    TRUE    - EJ record is read.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CMainFrame::ReadLockedEJ( CReportEJ& reportEJ )
{
    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();
    BOOL    fSuccess;
    BOOL    fEndOfFile;

    fSuccess   = TRUE;
    fEndOfFile = FALSE;

    // --- initialize EJ object before EJ function call ---
    reportEJ.Initialize(_T(" ReadLockedEJ -"));

    // --- report EJ read report header ---
    DATE    thisDate;
    GetCurrentDateTime( thisDate );

    pDoc->ReportHeader( reportEJ, thisDate, GetLoggedOnTerminalNo());

    // --- read and report all EJ records until error occurs ---
	while( fSuccess && (!fEndOfFile))
	{
		fSuccess = reportEJ.ReadSetHeader( fEndOfFile );
		if( fSuccess )
		{
			pDoc->ReportItemCaption(reportEJ);
			pDoc->ReportEJLineData(reportEJ);
		}
	}

    pDoc->ReportTrailer( reportEJ );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::ReadAndResetEJ()
//
//  PURPOSE :   Read and reset EJ in unlocked terminal.
//
//  RETURN :    TRUE    - EJ record is read and reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CMainFrame::ReadAndResetEJ( CReportEJ & reportEJ, const BOOL  fOverride )
{
    ASSERT( IsLoggedOn());

    CPCSampleDoc* pDoc;
    pDoc = GetMyDocument();

    // --- initialize EJ object before EJ function call ---
    reportEJ.Initialize(_T(" ReadAndResetEJ -"));

    // --- report EJ reset report header ---
    DATE    thisDate;
    GetCurrentDateTime( thisDate );

    pDoc->ReportHeader( reportEJ, thisDate, GetLoggedOnTerminalNo());

    // --- read and reset all EJ records until error occurs ---
    BOOL    fSuccess;
    BOOL    fEndOfFile;

    fSuccess   = TRUE;
    fEndOfFile = FALSE;

    if ( fOverride )
    {
        while ( fSuccess && ( ! fEndOfFile ))
        {
            // --- call override type read function ---
            fSuccess = reportEJ.ReadAndResetOverride( fEndOfFile );
            if ( fSuccess )
            {
                // --- report read EJ record ---
                pDoc->ReportEJ( reportEJ );
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_TTLREAD, reportEJ.GetLastError());
            }
        }
    }
    else
    {
        while ( fSuccess && ( ! fEndOfFile ))
        {
            // --- call non-override type read function ---
            fSuccess = reportEJ.ReadAndResetNonOverride( fEndOfFile );
            if ( fSuccess )
            {
                // --- report read EJ record ---
                pDoc->ReportEJ( reportEJ );
            }
            else
            {
                // --- function is failed, and report its error ---
                pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_TTLREAD, reportEJ.GetLastError());
            }
        }
    }

    if ( fSuccess && fEndOfFile )
    {
        // --- report trailer of EJ report ---
        pDoc->ReportTrailer( reportEJ );
    }

    return ( fSuccess );
}

//----------   Log file retrieve functions start here

SHORT  CMainFrame::RetrieveRemoteFile (CFile &myFile, const SHORT sRemoteFile)
{
	SHORT  sRmtReadStatus = -1;

	if (sRemoteFile >= 0) {
		CHAR pBuffer[CMainFrame::iTransferBufferSize];

		for(;;) 
		{
			sRmtReadStatus = RmtReadFile(sRemoteFile, pBuffer, CMainFrame::iTransferBufferSize);
			if (sRmtReadStatus > 0) 
			{
				try
				{
					myFile.Write(pBuffer, sRmtReadStatus);
				}
				catch (CFileException *Error)
				{
					Error->Delete();
					break;
				}
				// check end of file
				if(sRmtReadStatus != CMainFrame::iTransferBufferSize) {
					break;
				}
			} else if (sRmtReadStatus == 0) { /* end of file */
				break;
			} else {
				break;
			}
		}
	}

	return sRmtReadStatus;
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnRcvDebgLog()
//
//  PURPOSE :   Receive all Debug Logs from terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnRcvDebgLog() 
{
	CPCSampleDoc* pDoc = GetMyDocument();
	CString  outMessage;

	pDoc->SetReportString(_T("*** RECEIVE DEBUG LOG(S) [BEGIN] ***"));

	WCHAR    aszFileName[RMT_NUM_DBG_FILE][RMT_FILENAME_SIZE];
	memset(aszFileName, 0x00, sizeof(aszFileName));

	RmtGetFileNames(aszFileName[0], _T("f"), _T(" "));

	IspLockKeyBoard();

	// loop through each debug file
	for(int dbgCount=0; dbgCount<RMT_NUM_DBG_FILE; dbgCount++) {
		if(aszFileName[dbgCount][0] != NULL) {
			WCHAR    sLocalFile[RMT_FILENAME_SIZE];
			WCHAR    sRmtFile[RMT_FILENAME_SIZE];		// handle of remote file

			// copy over name of debug file on remote terminal
			memcpy(sLocalFile, aszFileName[dbgCount], sizeof(aszFileName[dbgCount]));
			memcpy(sRmtFile, aszFileName[dbgCount], sizeof(aszFileName[dbgCount]));

			CFileException E;
			CFile    myFile;
			myFile.Open(sLocalFile, (CFile::modeCreate | CFile::modeWrite), &E);

			// open remote and terminal file
			SHORT sRemoteFile = RmtOpenFile(aszFileName[dbgCount],"or");
			if (sRemoteFile >= 0) {
				// transmission of file from remote terminal
				RetrieveRemoteFile (myFile, sRemoteFile);
			} 

			try 
			{
				outMessage.Format(_T("\tReceived: %s"), sRmtFile);
				pDoc->SetReportString(outMessage);
				myFile.Close();
			}
			catch (CFileException *e)
			{
				e->Delete();
				break;
			}
			RmtCloseFile(sRemoteFile);
		}
	}
	IspUnLockKeyBoard();

	pDoc->SetReportString(_T("*** RECEIVE DEBUG LOG(S) [COMPLETE] ***"));
	return;
}


//===========================================================================
//
//  FUNCTION :  CMainFrame::OnRcvAssrtLog()
//
//  PURPOSE :   Receive Assert Log from terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnRcvAssrtLog() 
{
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;

	pDoc->SetReportString(_T("*** RECEIVE ASSERT LOG [BEGIN] ***"));

	// copy over name of debug file on remote terminal
	LPCTSTR logFile = _T("ASSRTLOG");
	CFileException E;
	CFile  myFile;
	myFile.Open(logFile,(CFile::modeCreate | CFile::modeWrite),&E);

	// open remote and terminal file
	SHORT sRemoteFile = RmtOpenFile(logFile,"or");
	if (sRemoteFile >= 0) {
		// transmission of file from remote terminal
		RetrieveRemoteFile (myFile, sRemoteFile);
	} 

	try 
	{
		outMessage.Format(_T("\tReceived: %s"), logFile);
		pDoc->SetReportString(outMessage);
		myFile.Close();
	}
	catch (CFileException *e)
	{
		e->Delete();
	}

	RmtCloseFile(sRemoteFile);

	pDoc->SetReportString(_T("*** RECEIVE ASSERT LOG [COMPLETE] ***"));
	return;
}

void CMainFrame::OnDelAssrtLog() 
{
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;

	pDoc->SetReportString(_T("*** DELETE ASSERT LOG [BEGIN] ***"));

	// copy over name of debug file on remote terminal
	LPCTSTR logFile = _T("ASSRTLOG");

	// open remote and terminal file
	SHORT sRemoteFile = RmtDeleteFile(logFile);

	return;
}
//===========================================================================
//
//  FUNCTION :  CMainFrame::OnRcvLayout()
//
//  PURPOSE :   Receive all Layout files from terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnRcvLayout() 
{
	// TODO: Add your command handler code here
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;

	#define NUM_LAY_FILES		27

	LPCTSTR layoutFile[NUM_LAY_FILES] = {					{_T("DEFAULT")},
															{_T("PARAMLA")},
															{_T("PARAMLB")},
															{_T("PARAMLC")},
															{_T("PARAMLD")},
															{_T("PARAMLE")},
															{_T("PARAMLF")},
															{_T("PARAMLG")},
															{_T("PARAMLH")},
															{_T("PARAMLI")},
															{_T("PARAMLJ")},
															{_T("PARAMLK")},
															{_T("PARAMLL")},
															{_T("PARAMLM")},
															{_T("PARAMLN")},
															{_T("PARAMLO")},
															{_T("PARAMLP")},
															{_T("PARAMLQ")},
															{_T("PARAMLR")},
															{_T("PARAMLS")},
															{_T("PARAMLT")},
															{_T("PARAMLU")},
															{_T("PARAMLV")},
															{_T("PARAMLW")},
															{_T("PARAMLX")},
															{_T("PARAMLY")},
															{_T("PARAMLZ")}
		};

	CString statusString = _T("Receiving ");

	CDialog progressScreen;
	CProgressCtrl *myProgress;

	progressScreen.Create(IDD_RCV_PROGRESS);
	myProgress = (CProgressCtrl *) progressScreen.GetDlgItem(IDC_PROGRESS1);
	myProgress->SetRange(1, NUM_LAY_FILES);
	myProgress->SetStep(1);
	myProgress->SetPos(1);
	progressScreen.ShowWindow(SW_SHOW);

	pDoc->SetReportString(_T("*** RECEIVE LAYOUT FILE(S) [BEGIN] ***"));
	for(int i=0; i < NUM_LAY_FILES; i++)
	{
		statusString += ".";
		m_wndStatusBar.SetPaneText(0, statusString);

		Sleep(100);
		myProgress->SetPos(i);

		// open remote and local file
		SHORT sRemoteFile = RmtOpenFile(layoutFile[i],"or");
		if( sRemoteFile == 0 )
		{
			CFileException E;
			CFile  myFile;
			myFile.Open(layoutFile[i],(CFile::modeCreate | CFile::modeWrite),&E);

			// transmission of file from remote terminal
			RetrieveRemoteFile (myFile, sRemoteFile);

			try 
			{
				outMessage.Format(_T("\tReceived: %s"), layoutFile[i]);
				pDoc->SetReportString(outMessage);
				myFile.Close();
			}
			catch (CFileException *e)
			{
				e->Delete();
			}

			RmtCloseFile(sRemoteFile);
		}
	}
	pDoc->SetReportString(_T("*** RECEIVE LAYOUT FILE(S) [COMPLETE] ***"));

	statusString += " DONE";
	m_wndStatusBar.SetPaneText(0, statusString);

	progressScreen.ShowWindow(SW_HIDE);
	progressScreen.DestroyWindow();

	return;	
	
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnRcvLgoLog()
//
//  PURPOSE :   Receive Printer Logo file(s) from terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnRcvLgoFile() 
{
	// TODO: Add your command handler code here
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;
	CFileException E;

	pDoc->SetReportString(_T("*** RECEIVE LOGO FILE(S) [BEGIN] ***"));

	LPCTSTR logFile[] = { {_T("PARAMLO1")} , {_T("PARAMLO2")} };
	for(int i = 0; i < sizeof(logFile)/sizeof(logFile[0]); i++)
	{
		// open remote and local file
		SHORT sRemoteFile = RmtOpenFile(logFile[i],"or");
		if( sRemoteFile == 0 )
		{
			CFile  myFile;
			myFile.Open(logFile[i],(CFile::modeCreate | CFile::modeWrite),&E);

			// transmission of file from remote terminal
			RetrieveRemoteFile (myFile, sRemoteFile);

			try 
			{
				outMessage.Format(_T("\tReceived: %s"), logFile[i]);
				pDoc->SetReportString(outMessage);
				myFile.Close();
			}
			catch (CFileException *e)
			{
				e->Delete();
			}

			RmtCloseFile(sRemoteFile);
		}
	}
	pDoc->SetReportString(_T("*** RECEIVE LOGO FILE(S) [COMPLETE] ***"));
	return;	
}

//===========================================================================
//
//  FUNCTION :  CMainFrame::OnRcvLogLog()
//
//  PURPOSE :   Receive Pif Log from terminal.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMainFrame::OnRcvLogFile() 
{
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;

	pDoc->SetReportString(_T("*** RECEIVE PIF LOG [BEGIN] ***"));

	// copy over name of debug file on remote terminal
	LPCTSTR logFile = _T("PIFLOG");
	CFileException E;
	CFile  myFile;
	myFile.Open(logFile,(CFile::modeCreate | CFile::modeWrite),&E);

	// open remote and terminal file
	SHORT sRemoteFile = RmtOpenFile(logFile,"or");
	if (sRemoteFile >= 0) {
		// transmission of file from remote terminal
		RetrieveRemoteFile (myFile, sRemoteFile);
	} 

	try 
	{
		outMessage.Format(_T("\tReceived: %s"), logFile);
		pDoc->SetReportString(outMessage);
		myFile.Close();
	}
	catch (CFileException *e)
	{
		e->Delete();
	}

	RmtCloseFile(sRemoteFile);

	pDoc->SetReportString(_T("*** RECEIVE PIF LOG [COMPLETE] ***"));
	return;
}

void CMainFrame::OnEodDb() 
{
	USHORT usCurrentTerminal;
	usCurrentTerminal = GetLoggedOnTerminalNo();

	SerCasDelayedBalance((UCHAR)usCurrentTerminal);
}


void CMainFrame::OnRecieveSavTtl() 
{
	CPCSampleDoc* pDoc = GetMyDocument();
	CString outMessage;

	CFileDialog openFile(FALSE,NULL,L"SaveHere", OFN_DONTADDTORECENT);
		
	INT_PTR iRet = openFile.DoModal();

	pDoc->SetReportString(_T("*** RECEIVE SAV FILES [BEGIN] ***"));
	
	if (iRet == IDOK) {
		// This loop will check terminal for Saved Totals, and get
		// RMT_NUM_SAV_FILE_NM file names at a time. For each name,
		// we'll create or write to a local file of the same name,
		// open the remote file, then copy the contents of the
		// remote file to the local file. When RmtGetSavFileNames
		// returns a value != RMT_NUM_SAV_FILE_NM we'll know
		// we've received all files.
		int i = RMT_NUM_SAV_FILE_NM;
		do{
			WCHAR aszFileName[RMT_NUM_SAV_FILE_NM][RMT_FILENAME_SIZE];

			i = RmtGetSavFileNames(aszFileName[0]);
			for(int j=0; j < i; j++){
				WCHAR sLocalFile[RMT_FILENAME_SIZE];
				WCHAR sRmtFile[RMT_FILENAME_SIZE];

				// we've got an array of file names, now receive those files
				memcpy(sLocalFile, aszFileName[j], sizeof(aszFileName[j]));
				memcpy(sRmtFile, aszFileName[j], sizeof(aszFileName[j]));

				// open local file
				CFileException E;
				CFile  myFile;
				myFile.Open(sLocalFile,(CFile::modeCreate | CFile::modeWrite),&E);

				// open remote file
				SHORT sRemoteFile = RmtOpenFile(aszFileName[j],"ors");
				if (sRemoteFile >= 0) {
					// transmission of file from remote terminal
					RetrieveRemoteFile (myFile, sRemoteFile);
				} 

				try 
				{
					outMessage.Format(_T("\tReceived: %s"), aszFileName[j]);
					pDoc->SetReportString(outMessage);

					myFile.Close();
				}
				catch (CFileException *e)
				{
					e->Delete();
				}

				RmtCloseFile(sRemoteFile);
			}
		} while (i == RMT_NUM_SAV_FILE_NM);
	}
	pDoc->SetReportString(_T("*** RECEIVE SAV FILES [COMPLETE] ***"));
}

// Handle the ID_READ_SAV_TOTAL menu message by opening a SavedTotal file,
// reading it, and then displaying the results.
void CMainFrame::OnReadSavTotal() 
{
	CPCSampleDoc*   pDoc = GetMyDocument();

	CString myFileDirectory, myFileName;
	BOOL bRet;
	CFileDialog openFile(TRUE);
	CSavedTotal mySavedTotal(pDoc);
		
	openFile.DoModal();
	myFileName = openFile.GetFileName();
	myFileDirectory = openFile.GetPathName();

	bRet = mySavedTotal.Open(myFileDirectory);
	if(bRet == TRUE)
	{
		pDoc->csSheetNamePrefix = mySavedTotal.m_mySaveTotalFileName;
		for( UCHAR i = CLASS_TTLREGFIN; i <= CLASS_TTLCPN; i++)
		{
			bRet = mySavedTotal.Read(i);
			if(bRet == TRUE)
			{
				mySavedTotal.Report(i);
			}
		}
		bRet = mySavedTotal.Read(CLASS_TTLSERVICE);
		if(bRet == TRUE)
		{
			mySavedTotal.Report(CLASS_TTLSERVICE);
		}
		pDoc->csSheetNamePrefix.Empty();
	}
}


void CMainFrame::OnSendOpMessage() 
{
	CSendOpMessageDialog  mySendOpMessage;

	mySendOpMessage.DoModal ();
}

void CMainFrame::OnUpdateMenuElement(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsLoggedOn() );	
}

#include <afxtempl.h>

static CList<CString, CString &> myListx;

static int myCallback (void *NotUsed, int argc, char **argv, char **azColName)
{
	struct {
		char *argv;
		char *azColName;
	} list[4] = {
		{argv[0], azColName[0]},
		{argv[1], azColName[1]},
		{argv[2], azColName[2]},
		{argv[3], azColName[3]},
	};

	CString  tcsbuff(argv[0]);
	myListx.AddTail (tcsbuff);

	return 0;
}

void CMainFrame::OnSelectFromActivityList() 
{
	CPCSampleDoc *pDoc = this->GetMyDocument();

	CSelectActivityList  mySelectActivityList;

	CSqlitedb  myDbFile;

	myDbFile.OpenFile (_T("pcsample.db"));

	myListx.RemoveAll();

	int rc = myDbFile.ExecSqlStmt ("SELECT * FROM actionList ORDER BY rowid;", myCallback);

	mySelectActivityList.m_list = &myListx;

	if (mySelectActivityList.DoModal () == IDOK) {
		TRACE1("ok selected %s\n", mySelectActivityList.m_csSelection);
        CReportEOD  reportEOD( this, pDoc );
		reportEOD.ExportDbToFiles (mySelectActivityList.m_csSelection);
	}
}

