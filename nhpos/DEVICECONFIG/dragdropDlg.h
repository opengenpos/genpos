// dragdropDlg.h : header file
//

#pragma once

#include "MsxmlWrapper.h"
#include "MyDropTarget.h"

// CdragdropDlg dialog
class CdragdropDlg : public CDialog
{
// Construction
public:
	CdragdropDlg(CWnd* pParent = NULL);	// standard constructor

	BOOL ValidateKeyCode (char *sKeyHash, wchar_t *sKeyCode);
	BOOL GenerateKeyCode (char *sKeyCode);

	BOOL ReadAndProcessLicenseFile (TCHAR  tcsFileName[512]);

	void HandlePinPadEntry (const TCHAR  tcsChar);


// Dialog Data
	enum { IDD = IDD_DRAGDROP_DIALOG };

	CString  m_tcsUrl;
	CString  m_tcsSerialNo;

	// the following data areas are used during the process of
	// creating a license key value, processing a dropped license key file,
	// or other uses.
	wchar_t m_SerialNumberString[64];
	wchar_t m_ResellerIdString[64];
	wchar_t m_KeyCodeString[128];

	wchar_t m_LicSerialNumberString[64];
	wchar_t m_LicResellerIdString[64];
	wchar_t m_LicKeyCodeString[128];
	wchar_t m_LicErrorString[128];

	char    m_GeneratedString[128];   // contains the string to be encrypted by GenerateKeyCode().
	char    m_GeneratedKeyCode[256];  // contains the encrypt as a hex string created by GenerateKeyCode().
	char    m_ValidatedString[128];  // contains the unecrypted string from ValidateKeyCode()
	char    m_aszSerialNo[64];       // contains terminal serial number (m_tcsSerialNo) as char string
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CMyDropTarget    *pDropTarget;
	int ParseXmlBuffer (BYTE  *szTemp);
	int ParseAndUpdateDialog (BYTE  *szTemp);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT wParam, CPoint myPoint);
	afx_msg LRESULT  OnDropString(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedButtonSetResellerId();
	afx_msg void OnBnClickedButtonSetSerialNo();
	afx_msg void OnBnClickedButtonDash();
	afx_msg void OnBnClickedButton0();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonFetch();
	afx_msg void OnStnClickedStaticResellerId();
};
