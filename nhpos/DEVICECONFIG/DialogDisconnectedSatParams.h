#if !defined(AFX_DIALOGDISCONNECTEDSATPARAMS_H__64D838AA_F065_46B3_95B5_CA19B83E9E4F__INCLUDED_)
#define AFX_DIALOGDISCONNECTEDSATPARAMS_H__64D838AA_F065_46B3_95B5_CA19B83E9E4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDisconnectedSatParams.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDisconnectedSatParams dialog

class CDialogDisconnectedSatParams : public CDialog
{
// Construction
public:
	CDialogDisconnectedSatParams(CWnd* pParent = NULL);   // standard constructor
	void SetParameters (int IsSatelliteOverride, int IsDisconnected, int IsJoined, TCHAR *LastMaster, TCHAR *SatOverride);
	void GetParameters (int &IsSatelliteOverride, int &IsDisconnected, int &IsJoined, TCHAR *LastMaster, TCHAR *SatOverride);
	void SetStaticTermLabelText (void);

// Dialog Data
	//{{AFX_DATA(CDialogDisconnectedSatParams)
	enum { IDD = IDD_DIALOG_DISCONNECTED_SAT_PARAMS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDisconnectedSatParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int      m_IsDisconnectedSatellite;
	int      m_IsJoinedDisconnectedSatellite;
	int      m_IsSatelliteOverride;
	CString  m_LastJoinMaster;
	CString  m_SatelliteOverride;

	// Generated message map functions
	//{{AFX_MSG(CDialogDisconnectedSatParams)
	virtual void OnOK();
	afx_msg void OnNhposStandardSatellite();
	afx_msg void OnNhposDisconnectedSatellite();
	afx_msg void OnNhposDisconnectedSatJoined();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioDisconnectedSatellite2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDISCONNECTEDSATPARAMS_H__64D838AA_F065_46B3_95B5_CA19B83E9E4F__INCLUDED_)
