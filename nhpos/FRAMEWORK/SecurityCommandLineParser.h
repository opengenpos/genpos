// SecurityCommandLineParser.h: interface for the CSecurityCommandLineParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECURITYCOMMANDLINEPARSER_H__E05D44E2_9C93_41AC_B88E_5E5CEC8229D0__INCLUDED_)
#define AFX_SECURITYCOMMANDLINEPARSER_H__E05D44E2_9C93_41AC_B88E_5E5CEC8229D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSecurityCommandLineParser : public CCommandLineInfo  
{
public:
	CSecurityCommandLineParser();
	virtual ~CSecurityCommandLineParser();
	virtual void ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast);

	typedef  enum { StateStart = 0, StateRegister = 1, StateUnlockCode = 2, StateUserName = 3, StateRetrieve = 4, StateShutdown = 5, StateServerOnly = 6 } ParseState;

	ParseState     m_enRegisterState;
	TCHAR          m_szUnlockCode[128];
	TCHAR          m_szUserName[128];
};

#endif // !defined(AFX_SECURITYCOMMANDLINEPARSER_H__E05D44E2_9C93_41AC_B88E_5E5CEC8229D0__INCLUDED_)
