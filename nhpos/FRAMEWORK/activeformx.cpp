// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "activeformx.h"

/////////////////////////////////////////////////////////////////////////////
// CActiveFormX

IMPLEMENT_DYNCREATE(CActiveFormX, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CActiveFormX properties

/////////////////////////////////////////////////////////////////////////////
// CActiveFormX operations

void CActiveFormX::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long CActiveFormX::GetTrialStatus()
{
	long result;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void CActiveFormX::SetPath(LPCTSTR PathName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 PathName);
}

void CActiveFormX::SetOrderButton(long Param1)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Param1);
}

CString CActiveFormX::GetProductID()
{
	CString result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CActiveFormX::SetProductID(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CActiveFormX::GetUserName_()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CActiveFormX::GetRegNumber()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL CActiveFormX::GetMakeLicense()
{
	BOOL result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CActiveFormX::SetMakeLicense(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

CString CActiveFormX::GetVBOVersion()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL CActiveFormX::GetHidden()
{
	BOOL result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CActiveFormX::SetHidden(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

void CActiveFormX::SetUserName(LPCTSTR UserName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 UserName);
}

void CActiveFormX::SetUnlockCode(LPCTSTR code)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 code);
}

CString CActiveFormX::RegisterProduct()
{
	CString result;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CActiveFormX::DaysRemaining()
{
	CString result;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CActiveFormX::NumUses()
{
	CString result;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CActiveFormX::GetCompanyName()
{
	CString result;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CActiveFormX::AntiCode(BOOL Value)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Value);
}

void CActiveFormX::Events(BOOL Events)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Events);
}

void CActiveFormX::HideScreen(BOOL Screen)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Screen);
}

void CActiveFormX::SetTimer(long MS)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MS);
}

double CActiveFormX::DaysInstalled()
{
	double result;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_R8, (void*)&result, NULL);
	return result;
}

void CActiveFormX::ExpireLicense(long Expire)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Expire);
}

void CActiveFormX::RevertLicense(long Revert)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Revert);
}

void CActiveFormX::UseBios(long Allow)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Allow);
}

void CActiveFormX::UseNetWork(long Allow)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Allow);
}

void CActiveFormX::AntiDump()
{
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CActiveFormX::EnhSoftIce()
{
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}