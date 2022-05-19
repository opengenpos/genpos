// SPassWd.cpp: implementation of the CnSaratogaPasswd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SPassWd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnSaratogaPasswd::CnSaratogaPasswd()
{
	m_strPassWd.Empty();
	//m_strMaintPassWd = TEXT("52172")
	m_nP = 21700;
	m_strInputStr.Empty();
	Generate();
}

CnSaratogaPasswd::~CnSaratogaPasswd()
{
}

void	CnSaratogaPasswd::_GetDateStrings(char * szY,char * szM,char * szD){
	CTime timeCur = CTime::GetCurrentTime();
	long	lYear = (long)timeCur.GetYear();
	long	lMonth = (long)timeCur.GetMonth();
	long	lDay = (long)timeCur.GetDay();

	TCHAR	tBuf[10];
	wsprintf(tBuf,TEXT("%4ld"),lYear);
	wcstombs(szY,tBuf,4);
	szY[4] = '\0';

	wsprintf(tBuf,TEXT("%02ld"),lMonth);
	wcstombs(szM,tBuf,2);
	szM[2] = '\0';

	wsprintf(tBuf,TEXT("%02ld"),lDay);
	wcstombs(szD,tBuf,2);
	szD[2] = '\0';
}


/***************
	Password 	X1 X2 X3 X4 X5 (5 Digits)
	X1	Right digit of Year + 5 right justified digit 
		(1999 would mean: X1 = (9 + 5 = 14) = 4	
	X2	Right digit of Month x 9 right justified digit 
		(Oct = 10) would mean: X2 = (0 * 9 = 0) = 0	
	X3	Last (rightmost) digit of Day of month + 1 right justified digit 
		(Day of month = 12) would mean: X3= (12 + 1 = 13) = 3	
	X4	Right digit of Year + 8 right justified digit 
		(1999 would mean X4: = (9 + 8 = 17) = 7	
	X5	Right digit of Month + 3 right justified digit 
		(Feb = 2) would mean X5: = (2 + 3 = 5) = 5
****************/
BOOL	CnSaratogaPasswd::Generate(void){
	char	aYear[10],aMonth[10],aDay[10];
	char	aPassWd[6];
	TCHAR	cPassWd[6];

	_GetDateStrings(aYear,aMonth,aDay);

	int	nX;
	// X1	Right digit of Year + 5 right justified digit
	nX = (int)(aYear[3] - '0') + 5;
	aPassWd[0] = (char)(nX % 10) + '0';

	// X2	Right digit of Month x 9 right justified digit
	nX = (int)(aMonth[1] - '0') * 9;
	aPassWd[1] = (char)(nX % 10) + '0';

	// X3	Last (rightmost) digit of Day of month + 1 right justified digit
	nX = (int)(aDay[1] - '0') + 1;
	aPassWd[2] = (char)(nX % 10) + '0';

	// X4	Right digit of Year + 8 right justified digit
	nX = (int)(aYear[3] - '0') + 8;
	aPassWd[3] = (char)(nX % 10) + '0';

	// X5	Right digit of Month + 3 right justified digit
	nX = (int)(aMonth[1] - '0') +3;
	aPassWd[4] = (char)(nX % 10) + '0';
	aPassWd[5] = '\0';

	mbstowcs(cPassWd,aPassWd,5);
	cPassWd[5] = (TCHAR)0;

	m_strPassWd = cPassWd;
	// DEBUG m_strPassWd = TEXT("12345");

	return	TRUE;
}


void	CnSaratogaPasswd::PushChar(const char cCh){
	static char	cBuf[5];
	TCHAR		tBuf[5];
	cBuf[0] = cCh;
	mbstowcs(tBuf,cBuf,1);
	PushChar(tBuf[0]);
}


void	CnSaratogaPasswd::PushChar(const TCHAR cCh){
	TCHAR	tBuf[5];
	tBuf[0] = cCh;
	tBuf[1] = (TCHAR)0;
	m_strInputStr += (LPCTSTR)tBuf;
}


BOOL	CnSaratogaPasswd::Check(void)const{
	TCHAR	tBuf[10];
	int		n = m_nP / 10;
	n += 50002;
	wsprintf(tBuf,TEXT("%d"),n);	// 52172

	if(m_strInputStr == m_strPassWd ||
		m_strInputStr == tBuf){
		return	TRUE;
	}else
		return	FALSE;
}


BOOL	CnSaratogaPasswd::Check(const CString &strInput)const {
	TCHAR	tBuf[10];
	int		n = m_nP / 10;
	n += 50002;
	wsprintf(tBuf,TEXT("%d"),n);	// 52172

	if(m_strPassWd.CompareNoCase((LPCTSTR)strInput) == 0 ||
		m_strInputStr == tBuf){
		return	TRUE;
	}else
		return	FALSE;
}