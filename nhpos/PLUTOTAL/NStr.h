// NStr.h: interface for the CnString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NSTR_H__29247213_09D4_11D4_BEC0_00A0C961E76F__INCLUDED_)
#define AFX_NSTR_H__29247213_09D4_11D4_BEC0_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(POSSIBLE_DEAD_CODE) || defined(_CNPLUTOTAL_TRACE)

// This source for a CnString class is used for converting a PLU Number
// into a string for the purposes of logs. See areas that are
// ifdefed out with the defined constant of _CNPLUTOTAL_TRACE.

#define	CNSTRING_STR_LEN_MAX	255

class CnString : public CString  
{
protected:
	COleVariant		m_vValues;
	char			m_aStr[CNSTRING_STR_LEN_MAX + 10];
public:
	CnString();
	virtual ~CnString();

	CnString(const char * pStr);
	CnString(const char * pStr,const int nLen = 0);
	CnString(const long nVal,LPCTSTR szFromat = NULL);

	void	Clear(void);
//	void	GetValue(VARIANT * lpVar);
//	void	GetValue(char * pStr);
};

#endif

#endif // !defined(AFX_NSTR_H__29247213_09D4_11D4_BEC0_00A0C961E76F__INCLUDED_)
