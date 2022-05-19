// PluTtlRg.cpp: implementation of the CnPluTtlReg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluTotal.h"
#include "atlbase.h"
#include "PluTtlRg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnPluTtlReg::CnPluTtlReg()
{
	CreateKeys();
	m_strTotalPath.Empty();	
	m_strBackupPath.Empty();
}

CnPluTtlReg::~CnPluTtlReg()
{

}

BOOL CnPluTtlReg::GetTotalPath(LPTSTR szPath,const BOOL bRefresh)
{
	BOOL bRet = TRUE;
	if(m_strTotalPath.IsEmpty() || bRefresh){
		bRet = GetReg(TEMPPATH,DATABASE,&m_strTotalPath);
	}
	if(szPath != NULL)
		wsprintf(szPath,TEXT("%s"),(LPCTSTR)m_strTotalPath);

	return	bRet;
}

BOOL CnPluTtlReg::GetBackupPath(LPTSTR szPath,const BOOL bRefresh)
{
	BOOL bRet = TRUE;
	if(m_strTotalPath.IsEmpty() || bRefresh){
		bRet = GetReg(PATH,DATABASE,&m_strBackupPath);
	}
	if(szPath != NULL)
		wsprintf(szPath,TEXT("%s"),(LPCTSTR)m_strBackupPath);

	return	bRet;
}

BOOL CnPluTtlReg::GetTotalDbPath(LPTSTR szPath)
{
	TCHAR	szDbPath[255];
	TCHAR	szTotalDir[100];
	DWORD	dwSize1 = 255,dwSize2 = 100;
	BOOL	bRet = FALSE;
	LONG    lResult;

	lResult = Open(HKEY_LOCAL_MACHINE, m_strKeyDbPath, KEY_READ);
	if (lResult == ERROR_SUCCESS) {
		if((QueryStringValue(PATH, szDbPath, &dwSize1) == ERROR_SUCCESS) &&		//VS2k5 compat
			(QueryStringValue(TOTALDATA, szTotalDir, &dwSize2) == ERROR_SUCCESS)){
//		if((QueryValue(szDbPath,PATH,&dwSize1) == ERROR_SUCCESS) &&
//			(QueryValue(szTotalDir,TOTALDATA,&dwSize2) == ERROR_SUCCESS)){
#ifdef	_PLUTOTAL_TEST_1	// plu total tests
	/*#pragma message("### WARNING  PLU TOTAL TEST version!!!")*/
			CString	strTmp(szTotalDir);
			if(strTmp.Right(1) != TEXT("\\")){
				strTmp += TEXT("\\");
			}
			wsprintf(szPath,TEXT("%s"),(LPCTSTR)strTmp);
#else	// release
			AddPath(szDbPath,szTotalDir);
			wsprintf(szPath,TEXT("%s"),szDbPath);
#endif
			bRet = TRUE;

		}
		Close();
	}

	return	bRet;
}

void	CnPluTtlReg::AddPath(LPTSTR szPath,LPCTSTR szDir){
	CString	strPath(szPath);
	CString	strDir;

	if(szDir[0] == '\\')
		strDir = &szDir[1];
	else
		strDir = szDir;

	if(strPath.Right(1) != TEXT("\\"))
		strPath += TEXT("\\");
	strPath += strDir;

	if(strPath.Right(1) != TEXT("\\"))
		wsprintf(szPath,TEXT("%s\\"),(LPCTSTR)strPath);
	else
		wsprintf(szPath,TEXT("%s"),(LPCTSTR)strPath);
}


BOOL	CnPluTtlReg::GetReg(LPCTSTR szPathKey,LPCTSTR szDirKey,CString * strPath)
{
	TCHAR	szPath[255];
	TCHAR	szDir[100];
	DWORD	dwSize1 = sizeof(szPath),dwSize2 = sizeof(szDir);
	BOOL	bRet = FALSE;
	LONG    lResult;

	strPath->Empty();
	bRet = FALSE;
	lResult = Open(HKEY_LOCAL_MACHINE,m_strKeyDbPath,KEY_READ);
	if (lResult == ERROR_SUCCESS) {
		if((QueryStringValue(szPathKey, szPath, &dwSize1) == ERROR_SUCCESS) &&		//VS2k5 compat
			(QueryStringValue(szDirKey, szDir, &dwSize2) == ERROR_SUCCESS)){
//		if((QueryValue(szPath,szPathKey,&dwSize1) == ERROR_SUCCESS) &&
//			(QueryValue(szDir,szDirKey,&dwSize2) == ERROR_SUCCESS)){
			*strPath = szPath;
			if(strPath->Right(1) != TEXT("\\"))
				*strPath += TEXT("\\");
			*strPath += szDir;
			if(strPath->Right(1) != TEXT("\\"))
				*strPath += TEXT("\\");
			bRet = TRUE;
		}

		Close();
	}

	return	bRet;
}

void CnPluTtlReg::CreateKeys()
{
	// making of KEY
	m_strKeyDbPath.Format(_T("\\%s\\%s"), PIFROOTKEY,FILEKEY ) ;
}
