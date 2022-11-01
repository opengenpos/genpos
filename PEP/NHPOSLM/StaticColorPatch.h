#if !defined(AFX_STATICCOLORPATCH_H__FC5803CB_D266_49A0_B98E_6EDBB2C40C3E__INCLUDED_)
#define AFX_STATICCOLORPATCH_H__FC5803CB_D266_49A0_B98E_6EDBB2C40C3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticColorPatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticColorPatch window

class CStaticColorPatch : public CStatic
{
// Construction
public:
	CStaticColorPatch(COLORREF xColor = 0);

// Attributes
public:
	COLORREF  m_FillColor;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticColorPatch)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticColorPatch();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticColorPatch)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICCOLORPATCH_H__FC5803CB_D266_49A0_B98E_6EDBB2C40C3E__INCLUDED_)
