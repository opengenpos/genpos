// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IZebraPrintCtl wrapper class

#include "StdAfx.h"

class IZebraPrintCtl : public COleDispatchDriver
{
public:
	IZebraPrintCtl() {}		// Calls COleDispatchDriver default constructor
	IZebraPrintCtl(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IZebraPrintCtl(const IZebraPrintCtl& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Version();
	void SerialConnection(LPCTSTR szPort, long lBaud);
	void TcpConnection(LPCTSTR szHostname, long iPort);
	void IrDAConnection();
	long Print(LPCTSTR szData);
	long Open();
	void Close();
	long GetLastError();
	void ShowLastError();
	void ShowPrompts(long bEnablePrompts);
	void EnableStatusCheck(long bEnableCheck);
	long Send(LPCTSTR data);
	long GetPrinterStatus();
	void SetStatusQueryTimeout(long lTimeout);
	void EnablePrintVerify(long bVerify);
	void SerialCloseWait(long lWaitTime);
	CString Recv(long lTimeout);
	void SetFlowCtl(long lFlowCtlMask);
};

#define  ZEBRA_BUSY_MASK             0x0001      /* printer status, busy bit      */
#define  ZEBRA_PAPEROUT_MASK         0x0002      /* printer status, paper-out bit */
#define  ZEBRA_HEADUP_MASK           0x0004      /* printer status, head-up bit   */
#define  ZEBRA_LOWBATTERY_MASK       0x0008      /* printer status, low-battery bit */
#define  ZEBRA_NORESPONSE_MASK       0xFFFF0000  /* printer status, no response from printer */
