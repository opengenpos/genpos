//{{AFX_INCLUDES()
#include "softlocx6.h"
#include "activeformx.h"
//}}AFX_INCLUDES
#if !defined(AFX_SECCONTROL_H__5894FDBD_1FAA_496E_87A4_C704EC00E0EA__INCLUDED_)
#define AFX_SECCONTROL_H__5894FDBD_1FAA_496E_87A4_C704EC00E0EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SECControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SECControl dialog

class SECControl : public CDialog
{
// Construction
public:
	SECControl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SECControl)
	enum { IDD = IDD_SECURITY_DIALOG };
	CButton	m_repair;
	CButton	m_unlockButton;
	CSoftlocx6	m_softlocx;
	CString	m_status;
	CString	m_serialNum;
	CString	m_daysLeft;
	CString	m_unlockCode;
	VBOControl	m_vbolock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SECControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SECControl)
	afx_msg void OnDesktopStatusSoftlocx6ctrl1(BSTR FAR* Status, long Level, BSTR FAR* Serial, BSTR FAR* ExpireVal);
	afx_msg void OnRegisterstatusSoftlocx6ctrl1(BSTR FAR* Status, long Level);
	afx_msg void OnUnlock();
	virtual BOOL OnInitDialog();
	afx_msg void OnContinue();
	afx_msg void OnRepair();
	afx_msg void VBOLockOnRegistered();
	afx_msg void VBOLockOnExpired();
	afx_msg void VBOLockOnBackDate();
	afx_msg void VBOLockOnVerified();
	afx_msg void VBOLockOnMissing();
	afx_msg void VBOLockOnMaxtimes();
	afx_msg void OnRunning();
	afx_msg void OnIllegal();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECCONTROL_H__5894FDBD_1FAA_496E_87A4_C704EC00E0EA__INCLUDED_)

