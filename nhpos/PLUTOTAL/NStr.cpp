// NStr.cpp: implementation of the CnString class.
//
//////////////////////////////////////////////////////////////////////

#if defined(POSSIBLE_DEAD_CODE) || defined(_CNPLUTOTAL_TRACE)

// This source for a CnString class is used for converting a PLU Number
// into a string for the purposes of logs. See areas that are
// ifdefed out with the defined constant of _CNPLUTOTAL_TRACE.


#include "stdafx.h"
#ifndef _PLUTOTAL_NOP
// if turning off PluTotal functionality, don't include PluTotal.h file
#include "PluTotal.h"
#endif
#include "NStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnString::CnString()
{
	Clear();
}

CnString::~CnString()
{

}

CnString::CnString(const char * pStr){
	Clear();

	int len = strlen(pStr);
	if(len <= 0 || CNSTRING_STR_LEN_MAX < len){
		return;
	}

	memcpy(m_aStr,pStr,len);
	m_aStr[len] = '\0';

	TCHAR	tBuf[CNSTRING_STR_LEN_MAX + 1];
	mbstowcs(tBuf,pStr,len);
	tBuf[len] = (TCHAR)0;
	Format(TEXT("%s"),tBuf);
}

CnString::CnString(const char * pStr,const int nLen){
	Clear();

	if(nLen <= 0 || CNSTRING_STR_LEN_MAX < nLen){
		return;
	}

	memcpy(m_aStr,pStr,nLen);
	m_aStr[nLen] = '\0';

	TCHAR	tBuf[CNSTRING_STR_LEN_MAX + 1];
	mbstowcs(tBuf,pStr,nLen);
	tBuf[nLen] = (TCHAR)0;
	Format(TEXT("%s"),tBuf);
}

CnString::CnString(const long nVal,LPCTSTR szFormat){

	Clear();
	m_vValues = nVal;

	if(szFormat == NULL)
		Format(TEXT("%ld"),nVal);
	else
		Format(szFormat,nVal);
}

void	CnString::Clear(){
	Empty();
	m_vValues.Clear();
	m_aStr[0] = '\0';
}

#endif
