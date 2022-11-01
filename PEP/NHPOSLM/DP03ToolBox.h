#if !defined(AFX_DP03TOOLBOX_H__6ED0A0C7_FA9F_4E62_88E2_F8DF542AF8B9__INCLUDED_)
#define AFX_DP03TOOLBOX_H__6ED0A0C7_FA9F_4E62_88E2_F8DF542AF8B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DP03ToolBox.h : header file
//

#include "P003.H"

/////////////////////////////////////////////////////////////////////////////
// CDP03ToolBox dialog

class CDP03ToolBox : public CDialog
{
// Construction
public:
	CDP03ToolBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDP03ToolBox)
	enum { IDD = IDD_P03_BOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDP03ToolBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDP03ToolBox)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DP03TOOLBOX_H__6ED0A0C7_FA9F_4E62_88E2_F8DF542AF8B9__INCLUDED_)
