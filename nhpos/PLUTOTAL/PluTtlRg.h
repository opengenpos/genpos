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
public:
	CString			m_strTotalPath;			// TempPath (TempDisk....)
	CString			m_strBackupPath;		// Path     (FlashDusk...)
protected:
	CString		m_strKeyDbPath;
	// ### ADD Saratoga (060100)
public:
					CnPluTtlReg();
	virtual			~CnPluTtlReg();

	virtual BOOL	GetTotalPath(LPTSTR szPath,const BOOL bRefresh = FALSE);
	virtual BOOL	GetBackupPath(LPTSTR szPath,const BOOL bRefresh = FALSE);

	// ### OLD Saratoga (060100)
	virtual BOOL	GetTotalDbPath(LPTSTR szPath);

			void	AddPath(LPTSTR szPath,LPCTSTR szDir);

protected:
	virtual BOOL	GetReg(LPCTSTR szPathKey,LPCTSTR szDirKey,CString * strPath);
	virtual	void	CreateKeys();
};

#endif // !defined(AFX_PLUTTLRG_H__A1CF6F53_EA59_11D3_BE92_00A0C961E76F__INCLUDED_)
