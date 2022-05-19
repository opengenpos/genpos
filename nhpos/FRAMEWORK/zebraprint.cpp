// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "zebraprint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IZebraPrintCtl properties

/////////////////////////////////////////////////////////////////////////////
// IZebraPrintCtl operations

void IZebraPrintCtl::Version()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IZebraPrintCtl::SerialConnection(LPCTSTR szPort, long lBaud)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szPort, lBaud);
}

void IZebraPrintCtl::TcpConnection(LPCTSTR szHostname, long iPort)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szHostname, iPort);
}

void IZebraPrintCtl::IrDAConnection()
{
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long IZebraPrintCtl::Print(LPCTSTR szData)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		szData);
	return result;
}

long IZebraPrintCtl::Open()
{
	long result;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void IZebraPrintCtl::Close()
{
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long IZebraPrintCtl::GetLastError()
{
	long result;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void IZebraPrintCtl::ShowLastError()
{
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IZebraPrintCtl::ShowPrompts(long bEnablePrompts)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bEnablePrompts);
}

void IZebraPrintCtl::EnableStatusCheck(long bEnableCheck)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bEnableCheck);
}

long IZebraPrintCtl::Send(LPCTSTR data)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		data);
	return result;
}

long IZebraPrintCtl::GetPrinterStatus()
{
	long result;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void IZebraPrintCtl::SetStatusQueryTimeout(long lTimeout)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 lTimeout);
}

void IZebraPrintCtl::EnablePrintVerify(long bVerify)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bVerify);
}

void IZebraPrintCtl::SerialCloseWait(long lWaitTime)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 lWaitTime);
}

CString IZebraPrintCtl::Recv(long lTimeout)
{
	CString result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		lTimeout);
	return result;
}

void IZebraPrintCtl::SetFlowCtl(long lFlowCtlMask)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 lFlowCtlMask);
}
