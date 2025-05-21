// SecurityCommandLineParser.cpp: implementation of the CSecurityCommandLineParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "framework.h"
#include "FrameworkIo.h"
#include "SecurityCommandLineParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecurityCommandLineParser::CSecurityCommandLineParser()
{
	m_enRegisterState = ParseState::StateStart;
	memset (m_szUnlockCode, 0, sizeof(m_szUnlockCode));
	memset (m_szUserName, 0, sizeof(m_szUserName));
}

CSecurityCommandLineParser::~CSecurityCommandLineParser()
{

}

void CSecurityCommandLineParser::ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast)
{
	CString tmp;
	tmp = pszParam;

	if (bFlag && tmp == _T("register"))
	{
		m_enRegisterState = ParseState::StateRegister;
	}

	if (bFlag && tmp == _T("retrieve"))
	{
		m_enRegisterState = ParseState::StateRetrieve;
	}

	if (bFlag && tmp == _T("unlockcode") && m_enRegisterState == ParseState::StateRegister)
	{
		m_enRegisterState = ParseState::StateUnlockCode;
	}

	if (bFlag && tmp == _T("username") && m_enRegisterState == ParseState::StateRegister)
	{
		m_enRegisterState = ParseState::StateUserName;
	}

	if (bFlag && tmp == _T("shutdown"))
	{
		m_enRegisterState = ParseState::StateShutdown;
	}

	if (bFlag && tmp == _T("serveronly"))
	{
		m_enRegisterState = ParseState::StateServerOnly;
	}

	if (bFlag)
		return;

	if (m_enRegisterState == ParseState::StateUnlockCode) {
		_tcsncpy (m_szUnlockCode, pszParam, sizeof(m_szUnlockCode)/sizeof(m_szUnlockCode[0]));
		m_enRegisterState = ParseState::StateRegister;
		return;
	}

	if (m_enRegisterState == ParseState::StateUserName) {
		_tcsncpy (m_szUserName, pszParam, sizeof(m_szUserName)/sizeof(m_szUserName[0]));
		m_enRegisterState = ParseState::StateRegister;
		return;
	}

}
