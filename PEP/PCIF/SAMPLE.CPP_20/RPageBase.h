#pragma once
#include "afxdlgs.h"

class RPageBase :
	public CPropertyPage
{
private:
	void _logOutgoingMsgs(CString &cmdMsg);

protected:
	virtual BOOL OnInitDialog();

public:
	RPageBase();
	RPageBase(UINT nIDTemplate);
	~RPageBase(void);

	static int m_fLogOutgoingMsgs;
	void SendCommand(CString &commandString);
	void DisplayText(LPCTSTR pText, bool fIndent=false);
};

