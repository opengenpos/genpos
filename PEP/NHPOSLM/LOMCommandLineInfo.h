// LOMCommandLineInfo.h: interface for the CLOMCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOMCOMMANDLINEINFO_H__53876975_EF1F_499B_B96D_85311F369FE1__INCLUDED_)
#define AFX_LOMCOMMANDLINEINFO_H__53876975_EF1F_499B_B96D_85311F369FE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CLOMCommandLineInfo : public CCommandLineInfo  
{
public:
	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );

	WORD lang;
	WORD sublang;

private:

};

#endif // !defined(AFX_LOMCOMMANDLINEINFO_H__53876975_EF1F_499B_B96D_85311F369FE1__INCLUDED_)
