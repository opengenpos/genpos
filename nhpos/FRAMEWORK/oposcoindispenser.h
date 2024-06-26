#if !defined(AFX_OPOSCOINDISPENSER_H__0ECA6A04_50DF_440F_812C_AC338DD96D41__INCLUDED_)
#define AFX_OPOSCOINDISPENSER_H__0ECA6A04_50DF_440F_812C_AC338DD96D41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// COPOSCoinDispenser wrapper class

class COPOSCoinDispenser : public CWnd
{
protected:
	DECLARE_DYNCREATE(COPOSCoinDispenser)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xccb90062, 0xb81e, 0x11d2, { 0xab, 0x74, 0x0, 0x40, 0x5, 0x4c, 0x37, 0x19 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	long GetOpenResult();
	CString GetCheckHealthText();
	BOOL GetClaimed();
	BOOL GetDeviceEnabled();
	void SetDeviceEnabled(BOOL bNewValue);
	BOOL GetFreezeEvents();
	void SetFreezeEvents(BOOL bNewValue);
	long GetResultCode();
	long GetResultCodeExtended();
	long GetState();
	CString GetControlObjectDescription();
	long GetControlObjectVersion();
	CString GetServiceObjectDescription();
	long GetServiceObjectVersion();
	CString GetDeviceDescription();
	CString GetDeviceName();
	long CheckHealth(long Level);
	long ClaimDevice(long Timeout);
	long Close();
	long DirectIO(long Command, long* pData, BSTR* pString);
	long Open(LPCTSTR DeviceName);
	long ReleaseDevice();
	BOOL GetCapEmptySensor();
	BOOL GetCapJamSensor();
	BOOL GetCapNearEmptySensor();
	long GetDispenserStatus();
	long DispenseChange(long Amount);
	long GetBinaryConversion();
	void SetBinaryConversion(long nNewValue);
	long GetCapPowerReporting();
	long GetPowerNotify();
	void SetPowerNotify(long nNewValue);
	long GetPowerState();
	BOOL GetCapStatisticsReporting();
	BOOL GetCapUpdateStatistics();
	long ResetStatistics(LPCTSTR StatisticsBuffer);
	long RetrieveStatistics(BSTR* pStatisticsBuffer);
	long UpdateStatistics(LPCTSTR StatisticsBuffer);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPOSCOINDISPENSER_H__0ECA6A04_50DF_440F_812C_AC338DD96D41__INCLUDED_)
