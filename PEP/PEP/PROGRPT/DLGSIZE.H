// dlgsize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSizeDlg dialog

class CSizeDlg : public CDialog
{
// Construction
public:
	CSizeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSizeDlg)
	enum { IDD = IDD_SIZE_FORM };
	CStatic	m_Icon;
	BOOL	m_bDisp;
	CString	m_strMessage;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CSizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
