// NDbgTrc.h: interface for the CnDbgTrace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NDBGTRC_H__E4361434_FAF2_11D3_BEAD_00A0C961E76F__INCLUDED_)
#define AFX_NDBGTRC_H__E4361434_FAF2_11D3_BEAD_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CnDbgTrace : public CObject  
{
private:
	static	int	s_nObjectCounter;
protected:
	SYSTEMTIME	m_stTime;
	CTime		m_tmTimer;
	BOOL		m_bOpenFlg;
	CFile		m_fLog;
	int			m_nCnt;
public:
	CString	m_strLogName;
public:
	virtual void GetTimeString(LPTSTR szCurTime);
	virtual void StopTime();
	virtual void StartTime(LPCTSTR szLabel);
	CnDbgTrace(LPCTSTR szLogFileName = NULL);
	virtual ~CnDbgTrace();
	BOOL OpenLog(const int nOpenMode = 0);
	void CloseLog();
	virtual	void	PutLogFile(CString &strBuf);
	virtual void	PutTitle(LPCTSTR szTitle);
	virtual void	PutLog(LPCTSTR szLabel1,LPCTSTR szLabel2,LPCTSTR szLabel3);
};

#endif // !defined(AFX_NDBGTRC_H__E4361434_FAF2_11D3_BEAD_00A0C961E76F__INCLUDED_)
