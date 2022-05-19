// NDbgTrc.cpp: implementation of the CnDbgTrace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NDbgTrc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnDbgTrace::CnDbgTrace(LPCTSTR szLogFileName)
{
	if(szLogFileName == NULL)
		m_strLogName = TEXT("\\DbgLog.txt"); // root
	else
		m_strLogName = szLogFileName;
	m_bOpenFlg =  FALSE;

	m_nCnt = 0;
}

CnDbgTrace::~CnDbgTrace()
{
	CloseLog();
}

BOOL CnDbgTrace::OpenLog(const int nOpenMode)
{
	BOOL	bSts;
	if(m_bOpenFlg)
		return	FALSE;
	switch(nOpenMode){
	case	0:	// New,OverWrite
		bSts = m_fLog.Open((LPCTSTR)m_strLogName,CFile::modeCreate | CFile::modeWrite);
		break;
	case	1:	// Add
	default:
		bSts = m_fLog.Open((LPCTSTR)m_strLogName,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
		m_fLog.SeekToEnd();
		break;
	}
	if(bSts){
		m_bOpenFlg = TRUE;
		return	TRUE;
	}
	return	FALSE;
}

void CnDbgTrace::CloseLog()
{
	if(m_bOpenFlg){
		m_fLog.Close();
		m_bOpenFlg = FALSE;
	}
}


void CnDbgTrace::PutLog(LPCTSTR szLabel1,LPCTSTR szLabel2,LPCTSTR szLabel3)
{
#if 0   // not write log file
	TCHAR	szTimeBuf[128];
	CString	strBuf;

	m_nCnt++;

	// current time
	GetTimeString(szTimeBuf);
	strBuf.Format(TEXT("%s (%06ld)[%s]%s%s\n"),szLabel1,m_nCnt,szTimeBuf,szLabel2,szLabel3);
	PutLogFile(strBuf);
#endif
}

void CnDbgTrace::PutTitle(LPCTSTR szTitle)
{
#if 0   // not write log file
	CString	strBuf(szTitle);
	PutLogFile(strBuf);
#endif
}

void	CnDbgTrace::PutLogFile(CString &strBuf){
#if 0   // not write log file
#if defined(_CNPLUTOTAL_DB_TRACE) || defined(_CNPLUTOTAL_TRACE) \
	|| defined(_CNPLUTOTAL_DBFILE_TRACE)

	char	sBuf[1024];
	if(!m_bOpenFlg)
		OpenLog(1);
	int	nLen = strBuf.GetLength();
	wcstombs(sBuf,(LPCTSTR)strBuf,nLen);
	sBuf[nLen] = '\0';

	m_fLog.Write((const void *)sBuf,nLen);
	m_fLog.Flush();
	CloseLog();

#ifdef	_DEBUG
	::OutputDebugString((LPCTSTR)strBuf);
#endif

#endif
#endif
}

void CnDbgTrace::StartTime(LPCTSTR szLabel)
{
	// ### MAKING
}

void CnDbgTrace::StopTime()
{
	// ### MAKING
}

void CnDbgTrace::GetTimeString(LPTSTR szCurTime)
{
	::GetSystemTime(&m_stTime);
	wsprintf(szCurTime,TEXT("%02u:%02u:%02u.%03u"),m_stTime.wHour,m_stTime.wMinute,m_stTime.wSecond,m_stTime.wMilliseconds);
}
