//{{AFX_INCLUDES()
#include "oposposprinter.h"
//}}AFX_INCLUDES
#if !defined(AFX_OPOSDLG_H__06B53518_6239_4584_9FF7_FF63E9819EB0__INCLUDED_)
#define AFX_OPOSDLG_H__06B53518_6239_4584_9FF7_FF63E9819EB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OPOSDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OPOSDlg dialog

class OPOSDlg : public CDialog
{
// Construction
public:
	OPOSDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OPOSDlg)
	enum { IDD = IDD_DIALOG_OPOS_TEST };
	COPOSPOSPrinter	m_OPOSPrinter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OPOSDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OPOSDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPOSDLG_H__06B53518_6239_4584_9FF7_FF63E9819EB0__INCLUDED_)
