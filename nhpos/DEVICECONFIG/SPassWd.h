// SPassWd.h: interface for the CnSaratogaPasswd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPASSWD_H__E8324E59_1BCB_11D4_BEED_00A0C961E76F__INCLUDED_)
#define AFX_SPASSWD_H__E8324E59_1BCB_11D4_BEED_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CnSaratogaPasswd
{
private:
	CString	m_strPassWd;
	int		m_nP;
public:
	CString	m_strInputStr;
public:
	CnSaratogaPasswd();
	virtual ~CnSaratogaPasswd();

	virtual BOOL	Generate(void);
	virtual	void	ClearInputBuff(void){ m_strInputStr.Empty(); };
	virtual void	PushChar(const char cCh);
	virtual void	PushChar(const TCHAR cCh);
	virtual	BOOL	CheckInputStringSize(void)const{
						if(m_strInputStr.GetLength() == m_strPassWd.GetLength())
							return	TRUE;
						return	FALSE;
					};
	virtual BOOL	Check(void) const;
	virtual BOOL	Check(const CString &strInput)const;
protected:
	void	_GetDateStrings(char * szY,char * szM,char * szD);
};

#endif // !defined(AFX_SPASSWD_H__E8324E59_1BCB_11D4_BEED_00A0C961E76F__INCLUDED_)
