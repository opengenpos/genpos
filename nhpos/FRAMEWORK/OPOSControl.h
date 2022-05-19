//{{AFX_INCLUDES()
#include "oposcashdrawer.h"
#include "linedisplay.h"
#include "oposposprinter.h"
#include "oposcoindispenser.h"
#include "oposlinedisplay.h"
//}}AFX_INCLUDES
#if !defined(AFX_OPOSCONTROL_H__27FD0995_12E4_40D8_A5EA_0190684BAA0D__INCLUDED_)
#define AFX_OPOSCONTROL_H__27FD0995_12E4_40D8_A5EA_0190684BAA0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OPOSControl.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// COPOSControl dialog

#define LINEDISPLAY_FLAG_SCROLLING  0x00000001

#define PINPADSTATE_CLEAR     0
#define PINPADSTATE_MSR       1
#define PINPADSTATE_PINENTRY  2

class COPOSControl : public CDialog
{
// Construction
public:
	COPOSControl(CWnd* pParent = NULL);   // standard constructor
	~COPOSControl();                      // standard destructor

	static const ULONG  PRINTERSTATUSMAP_POWER        = 0x00000001;
	static const ULONG  PRINTERSTATUSMAP_CHEKPRINTER  = 0x00000002;
	static const ULONG  PRINTERSTATUSMAP_COVEROPEN    = 0x00000004;
	static const ULONG  PRINTERSTATUSMAP_PAPEROUT     = 0x00000008;

	LONG  m_lPrinter1Status;
	ULONG m_ulPrinter1StatusMap;   // map of printer status events in COPOSControl::OnStatusUpdateEventPosprinter1()
	ULONG m_ulPrinter1ResultMap;   // map of printer status events in CDeviceEngine::DevicePrinterWrite()

	LONG  m_lCashDrawer1Status;
	LONG  m_lCashDrawer2Status;

	ULONG m_ulLineDisplayFlags;              // indicates the type of line display and its capabilities

	BOOL     m_bSemdBlMessage;
	SHORT    m_sPinPadState;

	UCHAR    m_uchPaymentType;


// Dialog Data
	//{{AFX_DATA(COPOSControl)
	enum { IDD = IDD_BASE_DIALOG };
	COPOSCashDrawer	m_cashDrawer;
	COPOSCashDrawer	m_cashDrawer2;
	COPOSLineDisplay	m_lineDisplay;
	COPOSPOSPrinter	m_OPOSPrinter;
	COPOSCoinDispenser	m_OPOSCoin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COPOSControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COPOSControl)
	afx_msg void OnErrorEventPosprinter1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse);
	afx_msg void OnStatusUpdateEventPosprinter1(long Data);
	afx_msg void OnDataEventMsr1(long Status);
	afx_msg void OnErrorEventMsr1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse);
	afx_msg void OnDataEventMsr2(long Status);
	afx_msg void OnErrorEventMsr2(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse);
	afx_msg void OnStatusUpdateEventKeylock1(long Data);
	afx_msg void OnDirectIOEventKeylock1(long EventNumber, long FAR* pData, BSTR FAR* pString);
	afx_msg void OnStatusUpdateEventCashDrawer1(long Data);
	afx_msg void OnStatusUpdateEventCashDrawer2(long Data);
	afx_msg void OnDataEventSigcap1(long Status);
	afx_msg void OnErrorEventSigcap1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse);
	afx_msg void OnDataEventPinpad1(long Status);
	afx_msg void OnErrorEventPinpad1(long ResultCode, long ResultCodeExtended, long ErrorLocus, long FAR* pErrorResponse);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPOSCONTROL_H__27FD0995_12E4_40D8_A5EA_0190684BAA0D__INCLUDED_)
