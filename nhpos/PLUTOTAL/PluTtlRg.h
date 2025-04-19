// PluTtlRg.h: interface for the CnPluTtlReg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUTTLRG_H__A1CF6F53_EA59_11D3_BE92_00A0C961E76F__INCLUDED_)
#define AFX_PLUTTLRG_H__A1CF6F53_EA59_11D3_BE92_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<atlbase.h>
#include	"pif.h"

class CnPluTtlReg : public CRegKey  
{
private:
	CString		m_strKeyDbPath;
	// ### ADD Saratoga (060100)

	BOOL	GetReg(LPCTSTR szPathKey,LPCTSTR szDirKey,CString * strPath);
	void	CreateKeys();
	void	AddPath(LPTSTR szPath,LPCTSTR szDir);

public:
	CString			m_strTotalPath;			// TempPath (TempDisk....)
	CString			m_strBackupPath;		// Path     (FlashDusk...)

					CnPluTtlReg();
	virtual			~CnPluTtlReg();

	BOOL	GetTotalPath(LPTSTR szPath,const BOOL bRefresh = FALSE);
	BOOL	GetBackupPath(LPTSTR szPath,const BOOL bRefresh = FALSE);


};

#endif // !defined(AFX_PLUTTLRG_H__A1CF6F53_EA59_11D3_BE92_00A0C961E76F__INCLUDED_)
