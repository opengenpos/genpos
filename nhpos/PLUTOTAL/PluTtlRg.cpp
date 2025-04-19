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
	TCHAR	szPath[255] = { 0 };
	TCHAR	szDir[100] = { 0 };
	DWORD	dwSize1 = sizeof(szPath) / sizeof(szPath[0]);
	DWORD	dwSize2 = sizeof(szDir) / sizeof(szDir[0]);
	BOOL	bRet = FALSE;
	LONG    lResult;

	strPath->Empty();
	bRet = FALSE;
	lResult = Open(HKEY_LOCAL_MACHINE,m_strKeyDbPath,KEY_READ);
	if (lResult == ERROR_SUCCESS) {
		if((QueryStringValue(szPathKey, szPath, &dwSize1) == ERROR_SUCCESS) &&		//VS2k5 compat
			(QueryStringValue(szDirKey, szDir, &dwSize2) == ERROR_SUCCESS)){
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
