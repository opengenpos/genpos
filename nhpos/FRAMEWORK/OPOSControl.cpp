// OPOSControl.cpp : implementation file
//

#include "stdafx.h"
#include "framework.h"
#include "OPOSControl.h"
#include "Opos.h"
#include "OposCash.h"
#include "Oposptr.h"
#include "oposLock.h"
#include "scf.h"
#include "bl.h"
#include "BlFWif.h"
#include "DeviceIOPrinter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The following defines will enable additional logs when
// events are received from the OPOS control.
// #define DEBUG_PINPAD_LOG 1
// #define DEBUG_PRINTER_LOG 1
// #define DEBUG_MSR_LOG 1

/////////////////////////////////////////////////////////////////////////////
// COPOSControl dialog


COPOSControl::COPOSControl(CWnd* pParent /*=NULL*/)
	: CDialog(COPOSControl::IDD, pParent)
{
	m_lPrinter1Status = PTR_SUE_IDLE;
	m_ulPrinter1StatusMap = 0;
	m_ulPrinter1ResultMap = 0;
	m_lCashDrawer1Status = CASH_SUE_DRAWERCLOSED;	
	m_lCashDrawer2Status = 	CASH_SUE_DRAWERCLOSED;

	m_bSemdBlMessage = TRUE;
	m_sPinPadState = PINPADSTATE_CLEAR;

	// Removed in application payment support functionality with OpenGenPOS
	// as part of removing legacy code that may be used to work around PCI-DSS
	// account holder information security requriements.
	//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers


	m_ulLineDisplayFlags = 0;

	//{{AFX_DATA_INIT(COPOSControl)
	//}}AFX_DATA_INIT
}

COPOSControl::~COPOSControl ()
{
}

void COPOSControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COPOSControl)
	DDX_Control(pDX, IDC_CASHDRAWER1, m_cashDrawer);
	DDX_Control(pDX, IDC_LINEDISPLAY1, m_lineDisplay);
	DDX_Control(pDX, IDC_POSPRINTER1, m_OPOSPrinter);
	DDX_Control(pDX, IDC_COINDISPENSER1, m_OPOSCoin);
	DDX_Control(pDX, IDC_CASHDRAWER2, m_cashDrawer2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COPOSControl, CDialog)
	//{{AFX_MSG_MAP(COPOSControl)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COPOSControl message handlers

BEGIN_EVENTSINK_MAP(COPOSControl, CDialog)
    //{{AFX_EVENTSINK_MAP(COPOSControl)
	ON_EVENT(COPOSControl, IDC_POSPRINTER1, 3 /* ErrorEvent */, OnErrorEventPosprinter1, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	ON_EVENT(COPOSControl, IDC_POSPRINTER1, 5 /* StatusUpdateEvent */, OnStatusUpdateEventPosprinter1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_MSR1, 1 /* DataEvent */, OnDataEventMsr1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_MSR1, 3 /* ErrorEvent */, OnErrorEventMsr1, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	ON_EVENT(COPOSControl, IDC_MSR2, 1 /* DataEvent */, OnDataEventMsr2, VTS_I4)
	ON_EVENT(COPOSControl, IDC_MSR2, 3 /* ErrorEvent */, OnErrorEventMsr2, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	ON_EVENT(COPOSControl, IDC_KEYLOCK1, 5 /* StatusUpdateEvent */, OnStatusUpdateEventKeylock1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_CASHDRAWER1, 5 /* StatusUpdateEvent */, OnStatusUpdateEventCashDrawer1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_CASHDRAWER2, 5 /* StatusUpdateEvent */, OnStatusUpdateEventCashDrawer2, VTS_I4)
	ON_EVENT(COPOSControl, IDC_SIGCAP1, 1 /* DataEvent */, OnDataEventSigcap1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_SIGCAP1, 3 /* ErrorEvent */, OnErrorEventSigcap1, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	ON_EVENT(COPOSControl, IDC_PINPAD1, 1 /* DataEvent */, OnDataEventPinpad1, VTS_I4)
	ON_EVENT(COPOSControl, IDC_PINPAD1, 3 /* ErrorEvent */, OnErrorEventPinpad1, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


/** ----------------------------------------------------------------------
	Data Event handlers for the MSR.

	We can have two different MSRs provisioned using the OPOS interface.
	A scenario where both may be needed is if using the VivoTech EPT
	while also wanting to allow the Terminal MSR to be used for a card swipe.

	So we must have two different MSR OPOS controls, one for each device.
	However since both of these are handled the same, we have a single
	function, COPOSControl::ProcessMsrEvent(), which we use to do the
	actual processing of the event.

	COPOSControl Keywords:  SCF_TYPE_MSR, SCF_OPOS_NAME_MSR, SCF_TYPENAME_MSR, COPOSMSR
**/
// Remove MSR event handlers as part of eliminating legacy MSR code for OpenGenPOS.
// This is to eliminate the possibility of not meeting PCI-DSS account holder information
// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
// Datacap Out Of Scope control for electronic payment.
//
//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

void COPOSControl::OnDataEventMsr1(long Status) 
{
}

void COPOSControl::OnDataEventMsr2(long Status) 
{
}


// Process an error event if there is an error with the MSR device.
// Basically, we want to log the fact there was an error and then
// clear the device to make it ready for reuse.
void COPOSControl::OnErrorEventMsr1(long ResultCode, long ResultCodeExtended,
	long ErrorLocus, long * pErrorResponse)
{
}

void COPOSControl::OnErrorEventMsr2(long ResultCode, long ResultCodeExtended,
	long ErrorLocus, long * pErrorResponse)
{
}

void COPOSControl::OnErrorEventPosprinter1(long ResultCode, long ResultCodeExtended,
	long ErrorLocus, long * pErrorResponse)
{
	DWORD dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;

	m_ulPrinter1ResultMap = PRINTERSTATUSMAP_CHEKPRINTER;

	if (ResultCode == OPOS_E_EXTENDED)
	{
		if (ResultCodeExtended == OPOS_EPTR_SLP_EMPTY)
		{
			dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPINS_ADR;
		}
		else if (ResultCodeExtended == OPOS_EPTR_REC_EMPTY)
		{
			dwErrorCode = DEVICEIO_PRINTER_MSG_PAPEROUT_ADR;
			m_ulPrinter1ResultMap |= PRINTERSTATUSMAP_COVEROPEN;
		}
		else if (ResultCodeExtended == OPOS_EPTR_COVER_OPEN)
		{
			dwErrorCode = DEVICEIO_PRINTER_MSG_COVEROPEN_ADR;
			m_ulPrinter1ResultMap |= PRINTERSTATUSMAP_COVEROPEN;
		}
	}

#if defined(DEBUG_PRINTER_LOG)
	// following code used for printer testing to check that the printer is sending Error Events
	{
		char xBuff[128];
		sprintf(xBuff, "## OnErrorEventPosprinter1() called code %d, extended %d, m_ulPrinter1ResultMap 0x%x", ResultCode, ResultCodeExtended, m_ulPrinter1ResultMap);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}
#endif

}

void COPOSControl::OnStatusUpdateEventPosprinter1(long Data) 
{
	static DWORD  dwTickCount = 0;
	LONG  unusedExampleDataValue = OPOS_SUE_POWER_ONLINE;  // allows value constants to be found.

	m_lPrinter1Status = Data;

	switch (Data) {
		case OPOS_SUE_POWER_ONLINE:
			m_ulPrinter1StatusMap &= ~PRINTERSTATUSMAP_POWER;
			break;

		case OPOS_SUE_POWER_OFF_OFFLINE:
		case OPOS_SUE_POWER_OFFLINE:
			m_ulPrinter1StatusMap |= PRINTERSTATUSMAP_POWER;
			break;

		case PTR_SUE_COVER_OPEN:
			m_ulPrinter1StatusMap |= PRINTERSTATUSMAP_COVEROPEN;
			break;

		case PTR_SUE_COVER_OK:
			m_ulPrinter1StatusMap &= ~PRINTERSTATUSMAP_COVEROPEN;
			m_ulPrinter1StatusMap &= ~PRINTERSTATUSMAP_PAPEROUT;
			break;

		case PTR_SUE_REC_EMPTY:
			m_ulPrinter1StatusMap |= PRINTERSTATUSMAP_PAPEROUT;
			dwTickCount = GetTickCount();
			break;

		case PTR_SUE_REC_PAPEROK:
			if (GetTickCount() - dwTickCount > 30)
				m_ulPrinter1StatusMap &= ~PRINTERSTATUSMAP_PAPEROUT;
			break;

		default:
			break;
	}

#if defined(DEBUG_PRINTER_LOG)
	// following code used for printer testing to check that the printer is sending Status Update Events
	{
		char xBuff[128];
		sprintf (xBuff, "## OnStatusUpdateEventPosprinter1() called Data %d, 0x%8.8x", Data, m_ulPrinter1StatusMap);
		NHPOS_ASSERT_TEXT(0, xBuff); 
	}
#endif
}


void COPOSControl::OnStatusUpdateEventKeylock1(long Data) 
{
    BL_NOTIFYDATA   BlNotify;
    CHARDATA        CharData;

	BlNotify.u.Data.ulDeviceId = BL_DEVICE_DATA_KEY;
	BlNotify.u.Data.ulLength   = sizeof(CharData);
	BlNotify.u.Data.pvAddr     = &CharData;

	CharData.uchFlags = 0;
	CharData.uchASCII = 0;

	//We do this so that if a keyboard terminal loses its focus, all we have to 
	//do is flip the key lock and we will regain focus. JHHJ
	AfxGetApp()->m_pMainWnd->SetForegroundWindow();

	switch(Data){
	case LOCK_KP_LOCK:
		CharData.uchScan = 0x03;
		break;
	case LOCK_KP_NORM:
		CharData.uchScan = 0x04;
		break;
	case LOCK_KP_SUPR:
		CharData.uchScan = 0x05;
		break;
	default:
		CharData.uchScan = 0x06;
		break;
	}

	BlNotify.ulType = BL_INPUT_DATA;
    BlNotifyData(&BlNotify, NULL);
}

void COPOSControl::OnStatusUpdateEventCashDrawer1(long Data) 
{
	m_lCashDrawer1Status = Data;
}

void COPOSControl::OnStatusUpdateEventCashDrawer2(long Data) 
{
	m_lCashDrawer2Status = Data;	
}


// Remove Signature Capture event handlers as part of eliminating legacy MSR code for OpenGenPOS.
// This is to eliminate the possibility of not meeting PCI-DSS account holder information
// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
// Datacap Out Of Scope control for electronic payment.
//
//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers
void COPOSControl::OnDataEventSigcap1(long Status)
{
}

void COPOSControl::OnErrorEventSigcap1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse) 
{
}

void COPOSControl::OnDataEventPinpad1(long Status) 
{
}

void COPOSControl::OnErrorEventPinpad1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse) 
{
}
