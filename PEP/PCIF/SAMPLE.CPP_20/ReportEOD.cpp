/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ReportEOD.cpp
//
//  PURPOSE:    Provides End Of Day Report class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxtempl.h>

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"

#include "PCSample.h"
#include "PCSampleDoc.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

class CSqlitedbRegFin : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE regfin (Stamp CHAR(32), Name TEXT, Count INTEGER, TotalAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, Name));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		const char * const pFormat = "INSERT INTO regfin VALUES('%s', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, pName, 0, (long long)d13digit);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		const char * const pFormat = "INSERT INTO regfin VALUES('%s', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, pName, total.sCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		const char * const pFormat = "INSERT INTO regfin VALUES('%s', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.lCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		const char * const pFormat = "INSERT INTO regfin VALUES('%s', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, pName, 0, (long long)lValue);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		const char * const pFormat = "INSERT INTO regfin VALUES('%s', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, pName, sValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		return;
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		return;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		return;
	}
	virtual void	ReportElement( LPCTSTR, const void * )
	{
		return;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]}
		};
		CSqlitedbRegFin *pObj = static_cast<CSqlitedbRegFin *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S"), argv[1], argv[2], argv[3]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("Label,Count,Total"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM regfin WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbRegFin::myCallback, this);
	}
};

class CSqlitedbIndFin : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE indfin (Stamp CHAR(32), TermId TEXT, Name TEXT, Count INTEGER, TotalAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, TermId, Name));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		const char * const pFormat = "INSERT INTO indfin VALUES('%s', '%2.2ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)d13digit);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		const char * const pFormat = "INSERT INTO indfin VALUES('%s', '%2.2ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.sCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		const char * const pFormat = "INSERT INTO indfin VALUES('%s', '%2.2ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.lCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		const char * const pFormat = "INSERT INTO indfin VALUES('%s', '%2.2ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)lValue);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		const char * const pFormat = "INSERT INTO indfin VALUES('%s', '%2.2ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, sValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		return;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		return;
	}
	virtual void	ReportElement( LPCTSTR, const void * )
	{
		return;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]}
		};
		CSqlitedbIndFin *pObj = static_cast<CSqlitedbIndFin *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S,%S"), argv[1], argv[2], argv[3], argv[4]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("Term Id,Label,Count,Total"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM indfin WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbIndFin::myCallback, this);
	}
};


class CSqlitedbDept : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE dept (Stamp CHAR(32), DeptId INTEGER, Name TEXT, Count INTEGER, TotalAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, DeptId, Name));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)d13digit);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.sCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.lCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
   void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)lValue);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, sValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		return;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		return;
	}
	virtual void	ReportElement( LPCTSTR, const void * pVoid)
	{
		const CSqlitedb::DeptTotalDb * myDeptTbl = (CSqlitedb::DeptTotalDb *)pVoid;

		const char * const pFormat = "INSERT INTO dept VALUES('%s', '%4.4ld', '%S', %d, %lld);";
		char  stmtBuf[256];

		char  strType[8] = { ' ', 0 };
		switch ( myDeptTbl->uchItemType )
        {
			case pcpara::TYPE_MINUS:
				strcpy (strType, "MINUS");
				break;

			case pcpara::TYPE_HASH:
				strcpy (strType, "HASH");
				break;

			default:
				strType[0] = ' ';
				break;
        }

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, myDeptTbl->usDeptNo, myDeptTbl->auchMnemo, myDeptTbl->DeptTotal.lCounter, (long long)myDeptTbl->DeptTotal.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]}
		};
		CSqlitedbDept *pObj = static_cast<CSqlitedbDept *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S,%S"), argv[1], argv[2], argv[3], argv[4]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("Dept Id,Label,Count,Total"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM dept WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbDept::myCallback, this);
	}
};

class CSqlitedbCashierTotals : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE cashier (Stamp CHAR(32), CashId TEXT, Name TEXT, Count INTEGER, TotalAmt INTEGER, OnHandAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, CashId, Name));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)d13digit, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.sCounter, (long long)total.lAmount, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.lCounter, (long long)total.lAmount, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)lValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, sValue, (long long)0, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, usValue, (long long)0, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		const char * const pFormat = "INSERT INTO cashier VALUES('%s', '%8.8lu', '%S', %d, %lld, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, tendht.Total.sCounter, (long long)tendht.Total.lAmount, (long long)tendht.lHandTotal);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
	virtual void	ReportElement( LPCTSTR, const void * )
	{
		return;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]},
			{argv[5], azColName[5]}
		};
		CSqlitedbCashierTotals *pObj = static_cast<CSqlitedbCashierTotals *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S,%S,%S"), argv[1], argv[2], argv[3], argv[4], argv[5]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("Cashier Id,Label,Count,Total,OnHand"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM cashier WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbCashierTotals::myCallback, this);
	}
};

class CSqlitedbCouponTotals : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE coupon (Stamp CHAR(32), CouponId TEXT, Name TEXT, Count INTEGER, TotalAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, CouponId, Name));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)d13digit);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.sCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, total.lCounter, (long long)total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, 0, (long long)lValue);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, sValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, usValue, (long long)0);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, m_ulId, pName, tendht.Total.sCounter, (long long)tendht.Total.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}
	virtual void	ReportElement( LPCTSTR, const void *pVoid)
	{
		const CSqlitedb::CouponTotalDb * ttlCoupon = (CSqlitedb::CouponTotalDb *)pVoid;

		const char * const pFormat = "INSERT INTO coupon VALUES('%s', '%8.8lu', '%S', %d, %lld);";
		char  stmtBuf[256];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, ttlCoupon->usCpnNumber, ttlCoupon->auchMnemo, ttlCoupon->CpnTotal.lCounter, (long long)ttlCoupon->CpnTotal.lAmount);
		m_stRetCode = ExecSqlStmt (stmtBuf);
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]},
			{argv[5], azColName[5]}
		};
		CSqlitedbCouponTotals *pObj = static_cast<CSqlitedbCouponTotals *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S,%S"), argv[1], argv[2], argv[3], argv[4]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("coupon Id,Label,Count,Total"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM coupon WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbCouponTotals::myCallback, this);
	}
};

class CSqlitedbPluTotals : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE plutotals (Stamp CHAR(32), Number TEXT, Type TEXT, Adj INTEGER, DeptNo INTEGER, Count INTEGER, TotalAmt INTEGER, CONSTRAINT total_pk PRIMARY KEY (Stamp, Number, Type, Adj));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		return;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		return;
	}
	virtual void	ReportElement( LPCTSTR, const void * pVoid)
	{
		const CSqlitedb::PluTotalDb * ttlPlu = (CSqlitedb::PluTotalDb *)pVoid;

		const char * const pFormat = "INSERT INTO plutotals VALUES('%s', '%S', '%s', %d, %d, %ld, %lld);";
		char  stmtBuf[256];

		TCHAR uchPluNo[STD_PLU_NUMBER_LEN + 1] = {0};

		CParaPlu::convertPLU(uchPluNo, &ttlPlu->auchPLUNumber[0]);

        char  strType[8];
		switch ( ttlPlu->uchItemType )
        {
			case PLU_OPEN_TYP:
				strcpy (strType, "OPEN");
				break;

			case PLU_NON_ADJ_TYP:
				strcpy (strType, "NON");
				break;

			default:
				sprintf(strType,  " %2d", ttlPlu->uchAdjectNo);
				break;
        }

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, uchPluNo, strType, ttlPlu->uchAdjectNo, ttlPlu->usDeptNo, ttlPlu->PLUTotal.lCounter, (long long)ttlPlu->PLUTotal.lAmount );
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]},
			{argv[5], azColName[5]},
			{argv[6], azColName[6]}
		};
		CSqlitedbPluTotals *pObj = static_cast<CSqlitedbPluTotals *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,%S,%S,%S,%S"), argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("PLU Id,Type,Adj,Dept No.,Count,Total"));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM plutotals WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbPluTotals::myCallback, this);
	}
};

class CSqlitedbElecJournal : public CSqlitedb
{
public:
	void    CreateDb( )
	{
		m_stRetCode = ExecSqlStmt ("CREATE TABLE elecjournal (Stamp CHAR(32), ConsecNo TEXT, LineNo INTEGER, EjEntry TEXT, CONSTRAINT total_pk PRIMARY KEY (Stamp, ConsecNo, LineNo));");
	}
	void    ReportElement( LPCTSTR pName, const D13DIGITS & d13digit)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const TOTAL & total)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const LTOTAL & total)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const LONG & lValue)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const SHORT & sValue)
	{
		return;
	}
    void    ReportElement( LPCTSTR pName, const USHORT & usValue)
	{
		return;
	}
	void	ReportElement( LPCTSTR pName, const TENDHT & tendht)
	{
		return;
	}
	virtual void	ReportElement( LPCTSTR, const void * pVoid)
	{
		const CSqlitedb::ElecJournalDb * ttlEj = (CSqlitedb::ElecJournalDb *)pVoid;

		const char * const pFormat = "INSERT INTO elecjournal VALUES('%s', '%4.4d', %d, '%S');";
		char  stmtBuf[2048];

		sprintf (stmtBuf, pFormat, m_CurrKey.m_currKey, ttlEj->usConsecNo, ttlEj->usLineNo, ttlEj->pTextBuff );
		m_stRetCode = ExecSqlStmt (stmtBuf);
		m_ulRecordCnt++;
	}

	CList <CString, CString &> myListx;

	static int myCallback (void *pObject, int argc, char **argv, char **azColName)
	{
		struct {
			char *argv;
			char *azColName;
		} list[] = {
			{argv[0], azColName[0]},
			{argv[1], azColName[1]},
			{argv[2], azColName[2]},
			{argv[3], azColName[3]},
			{argv[4], azColName[4]}
		};
		CSqlitedbElecJournal *pObj = static_cast<CSqlitedbElecJournal *>(pObject);

		CString  tcsbuff;
		tcsbuff.Format(_T("%S,%S,\"%S\""), argv[1], argv[2], argv[3]);

		pObj->myListx.AddTail (tcsbuff);
		return 0;
	}

	void  ListSelect ()
	{
		myListx.RemoveAll ();

		CString  tcsbuff(_T("\"ConsecNo\",\"LineNo\",\"Text\""));
		myListx.AddTail (tcsbuff);

		char  stmtBuf[256];
		sprintf (stmtBuf, "SELECT * FROM elecjournal WHERE Stamp == '%s' ORDER BY rowid;", m_CurrKey.m_currKey);
		m_stRetCode = ExecSqlStmt (stmtBuf, CSqlitedbElecJournal::myCallback, this);
	}
};


//===========================================================================
//
//  FUNCTION :  CReportEOD::CReportEOD()
//
//  PURPOSE :   Constructor of End Of Day report class.
//
//  RETURN :    No return value.
//
//===========================================================================

CReportEOD::CReportEOD(
    class CMainFrame*   const pMainFrame,   // points to main frame window
    class CPCSampleDoc* const pDoc          // points to document class
    ) :
        m_pMainFrame( pMainFrame ),
        m_pDoc( pDoc ),
        m_sLastErrorLocus( 0 ),
        m_sLastError( SUCCESS )
{
    ASSERT( pMainFrame != NULL );
    ASSERT( pDoc != NULL );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::CReportEOD()
//
//  PURPOSE :   Destructor of End Of Day report class.
//
//  RETURN :    No return value.
//
//===========================================================================

CReportEOD::~CReportEOD()
{
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IsResetOK()
//
//  PURPOSE :   Determine whether totals are able to reset or not.
//
//  RETURN :    TRUE    - All of totals are able to reset.
//              FALSE   - Some total is not able to reset or error occurs.
//
//===========================================================================

BOOL CReportEOD::IsResetOK()
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEOD::IsResetOK()") );

    // --- get no. of terminals belongs to master 2170 terminal ---
    m_pMainFrame->GetNoOfTerminals( m_uchNoOfTerminals );

    BOOL    fSuccess = TRUE;
    // --- determine all of totals are able to reset ---
    for (INT  nI = CReportEOD::TTL_BEGIN; nI < CReportEOD::TTL_END; nI++ )
    {
        // --- get pointer of total class ---
		CTotal* pTotal = GetTotalObject( nI );
		BOOL    fSuccess_nI;
        switch ( nI )
        {
			case CReportEOD::TTL_INDFIN:
			case CReportEOD::TTL_CASHIER:
				// --- determine all of records are able to reset ---
				fSuccess_nI = pTotal->IsResetOK( pcttl::RESET_ALL_TOTAL );
				break;

			default:
				// --- determine the total is able to reset ---
				fSuccess_nI = ( pTotal != NULL ) ? pTotal->IsResetOK() : TRUE;
				break;
        }

		m_sTotalIsResetLastError [nI] = 0;
        if ( ! fSuccess_nI && pTotal != NULL)
        {
            // --- some total is not able to reset ---
            m_sLastError      = pTotal->GetLastError();
            m_sLastErrorLocus = static_cast< SHORT >( nI );
			m_sTotalIsResetLastError [nI] = m_sLastError;
			pcsample::TraceFunction ( _T("     Not Ok %d IsResetOK() %d"), nI, m_sLastError);

#if 0
			// following code will call ::SerTtlIssuedReset() on the totals object to clear the
			// reset indicator for a -4 error indicating previous End of Day did not complete on the
			// indicated totals object.
			// this is only provided as a demonstration of technique and may also be used in
			// a designer build if needed.
			switch ( nI )
			{
				case CReportEOD::TTL_INDFIN:
				case CReportEOD::TTL_CASHIER:
					// --- determine all of records are able to reset ---
					fSuccess_nI = pTotal->ResetAsOK( pcttl::RESET_ALL_TOTAL );
					break;

				default:
					// --- determine the total is able to reset ---
					fSuccess_nI = ( pTotal != NULL ) ? pTotal->ResetAsOK() : TRUE;
					break;
			}
#endif
        }

		fSuccess = (fSuccess && fSuccess_nI);
    }

    pcsample::TraceFunction( _T("END   >> CReportEOD::IsResetOK() = %d"), fSuccess );
    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::Reset()
//
//  PURPOSE :   Reset all of totals by End Of Day report.
//
//  RETURN :    TRUE    - All of totals are resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::Reset(const UCHAR uchMajorClass)
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEOD::Reset()\tBegin EOD") );

    BOOL    fSuccess;
    INT     nI;
	CParaMdcMem   mdcMem;
	CParaFlexMem  flexMem;

    // --- reset all of totals until error occuers ---
    fSuccess = TRUE;

    BOOL    fResetAtEOD = TRUE;

	flexMem.Read();
	mdcMem.Read();

	m_pDoc->SetDecimalFromMdc(mdcMem);

    nI = CReportEOD::TTL_BEGIN;
    while ( nI < CReportEOD::TTL_END )
    {
		CTotal*    pTotal = GetTotalObject(nI);

        switch ( nI )
        {
		case CReportEOD::TTL_REGFIN:
			// --- reset all records indiviaually ---
			if (mdcMem.CheckMdcBit(MDC_EOD1_ADR, CParaMdcMem::MdcBitD)) {
				TRACE(L"Not Execute Register Financial Report");
				fSuccess = 1;
			}
			else {
				fSuccess = ResetAllRecords(m_ttlRegFin, uchMajorClass);
			}
			break;

		case CReportEOD::TTL_INDFIN:
            // --- reset all records indiviaually ---
			if (mdcMem.CheckMdcBit(MDC_EOD3_ADR, CParaMdcMem::MdcBitD)) {
				TRACE(L"Not Execute Individual Financial Report");
				fSuccess = 1;
			}
			else {
				fSuccess = ResetAllRecords( m_ttlIndFin, uchMajorClass );
			}
            break;

		case CReportEOD::TTL_DEPT:
			// --- reset the whole of selected total ---
			if (mdcMem.CheckMdcBit(MDC_EOD1_ADR, CParaMdcMem::MdcBitC) || flexMem.getMemorySize(FLEX_DEPT_ADR) < 1) {
				TRACE(L"Not Execute Departmental Report");
				fSuccess = 1;
			}
			else {
				fSuccess = (pTotal != NULL) ? pTotal->Reset(fResetAtEOD) : TRUE;
			}
			break;

		case CReportEOD::TTL_PLU:
			// --- reset the whole of selected total ---
			if (mdcMem.CheckMdcBit(MDC_EOD1_ADR, CParaMdcMem::MdcBitB) || 
				(flexMem.getMemorySize(FLEX_DEPT_ADR) < 1 || flexMem.getMemorySize(FLEX_PLU_ADR) < 1)) {
				TRACE(L"Not Execute PLU Report");
				fSuccess = 1;
			}
			else {
				fSuccess = (pTotal != NULL) ? pTotal->Reset(fResetAtEOD) : TRUE;
			}
			break;

		case CReportEOD::TTL_HOURLY:
			// --- reset the whole of selected total ---
			if (mdcMem.CheckMdcBit(MDC_EOD1_ADR, CParaMdcMem::MdcBitA)) {
				TRACE(L"Not Execute Hourly Report");
				fSuccess = 1;
			}
			else {
				fSuccess = (pTotal != NULL) ? pTotal->Reset(fResetAtEOD) : TRUE;
			}
			break;

		case CReportEOD::TTL_SERVTIME:
			// --- reset the whole of selected total ---
			if (mdcMem.CheckMdcBit(MDC_EOD3_ADR, CParaMdcMem::MdcBitC) || flexMem.getMemorySize(FLEX_GC_ADR) < 1) {
				TRACE(L"Not Execute Service Time Report");
				fSuccess = 1;
			}
			else {
				fSuccess = (pTotal != NULL) ? pTotal->Reset(fResetAtEOD) : TRUE;
			}
			break;

		case CReportEOD::TTL_CASHIER:
            // --- reset all records indiviaually ---
			if (mdcMem.CheckMdcBit(MDC_EOD2_ADR, CParaMdcMem::MdcBitD)) {
				TRACE(L"Not Execute Individual Financial Report");
				fSuccess = 1;
			}
			else {
				fSuccess = ResetAllRecords(m_ttlCashier, uchMajorClass);
			}
            break;

		case CReportEOD::TTL_COUPON:
			// --- reset the whole of selected total ---
			if (mdcMem.CheckMdcBit(MDC_EOD2_ADR, CParaMdcMem::MdcBitA) || flexMem.getMemorySize(FLEX_CPN_ADR) < 1) {
				TRACE(L"Not Execute Coupon Report");
				fSuccess = 1;
			}
			else {
				fSuccess = (pTotal != NULL) ? pTotal->Reset(fResetAtEOD) : TRUE;
			}
			break;

        case CReportEOD::TTL_GUEST:
#if 0
			// do not reset any existing guest checks. this should be a separate action
			// and not part of the End of Day. The End of Day functionality of the GenPOS
			// terminal application does not do this.
			//    Richard Chambers, Jun-14-2019

            // --- reset all records indiviaually ---
            fSuccess = ResetAllRecords( m_GuestCheck, uchMajorClass );
#endif
            break;

        default:
            // --- reset the whole of selected total ---
            fSuccess = ( pTotal != NULL ) ? pTotal->Reset( fResetAtEOD ) : TRUE;
            break;
        }

        if ( ! fSuccess )
        {
            // --- function is failed and report its error code ---
            m_sLastError = ( pTotal != NULL ) ? pTotal->GetLastError() : m_sLastError;
            m_sLastErrorLocus = static_cast< SHORT >( nI );
            m_pDoc->ReportError( GetLastErrorLocus(), pcsample::ERR_RESET, GetLastError());
        }

        // --- increment counter to next total item ---
        nI++;
    }

	//This function sends a request to the terminal to update 
	//its totals from the transactions store in the total update 
	//file for delay balance.  This needs to be called after ALL totals
	//have been reset. JHHJ 1-5-06
	SerTtlTumUpdateDelayedBalance();

	// close terminal's saved totals file
	SerTtlCloseSavFile();
    
	pcsample::TraceFunction( _T("END   >> CReportEOD::Reset() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::Read()
//
//  PURPOSE :   Read all of resetted totals by End Of Day report.
//
//  RETURN :    TRUE    - All of totals are read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::Read()
{
    pcsample::TraceFunction( _T("BEGIN >> CReportEOD::Read()") );

	SYSTEMTIME  sysTime = {0};
	GetSystemTime (&sysTime);

	if (m_pDoc->m_SaveResetDataToDatabase) {
		CSqlitedb  myDbFile;
		char  aszRecord[256];

		myDbFile.OpenFile (_T("pcsample.db"));

		myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
		int rc = myDbFile.ExecSqlStmt ("CREATE TABLE actionList (Stamp CHAR(32), Name TEXT, CONSTRAINT item_pk PRIMARY KEY (Stamp, Name));");

		char  *format = "INSERT INTO actionList VALUES('%s', '%s');";
		sprintf (aszRecord, format, myDbFile.m_CurrKey.m_currKey, "EndOfDay");
		rc = myDbFile.ExecSqlStmt (aszRecord);
	}

	m_pDoc->m_listCurrentStr.RemoveAll();
	m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsReportPrint;

	m_pDoc->psqlDbFile = 0;

	{
		SYSTEMTIME  sysTimeLocal = { 0 };
		GetLocalTime(&sysTimeLocal);

		m_pDoc->pSavedTotalFile = new CSavedTotal (m_pDoc);
		CString stfName;

		stfName.Format(L"pcsample_endofday_%4.4d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.sav", sysTimeLocal.wYear, sysTimeLocal.wMonth, sysTimeLocal.wDay, sysTimeLocal.wHour, sysTimeLocal.wMinute, sysTimeLocal.wSecond);
		m_pDoc->pSavedTotalFile->Create(stfName);

	}

	m_pDoc->m_mdcMem.Read();
	m_pDoc->m_discountRate.Read();
	m_pDoc->m_currencyRate.Read();
	m_pDoc->m_totalKeyData.Read();
	m_pDoc->m_transactionMnemonics.Read();

	// --- read all of resetted totals until error occurs ---
    INT     nI = CReportEOD::TTL_BEGIN;

    while ( nI < CReportEOD::TTL_END )
    {
        switch ( nI )
        {
        case CReportEOD::TTL_REGFIN:
			{
				CSqlitedbRegFin  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlRegFin );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_INDFIN:
			{
				CSqlitedbIndFin  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlIndFin );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_DEPT:
			{
				CSqlitedbDept  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlDept );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_PLU:
			{
				CSqlitedbPluTotals  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlPlu );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_HOURLY:
            IssueReport( m_ttlHourly );
            break;

        case CReportEOD::TTL_CASHIER:
			{
				CSqlitedbCashierTotals  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlCashier );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_COUPON:
			{
				CSqlitedbCouponTotals  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_ttlCoupon );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_SERVTIME:
            IssueReport( m_ttlServTime );
            break;

        case CReportEOD::TTL_EJ:
			{
				CSqlitedbElecJournal  myDbFile;

				if (m_pDoc->m_SaveResetDataToDatabase) {
					myDbFile.OpenFile (_T("pcsample.db"));
					m_pDoc->psqlDbFile = &myDbFile;

					myDbFile.m_CurrKey.SystemTimeToSqliteTime (&sysTime);
					m_pDoc->ulReportOptionsMap |= CPCSampleDoc::ulReportOptionsExportDbFile;
					m_pDoc->psqlDbFile->CreateDb();
				}

				IssueReport( m_reportEJ );

				m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsExportDbFile;
				m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
			}
            break;

        case CReportEOD::TTL_BEGIN:
        case CReportEOD::TTL_GUEST:
        case CReportEOD::TTL_END:
        default:

            break;
        }

        // --- increment counter to next total item ---
        nI++;
    }

	delete m_pDoc->pSavedTotalFile; m_pDoc->pSavedTotalFile  = 0;

	m_pDoc->ulReportOptionsMap &= ~CPCSampleDoc::ulReportOptionsReportPrint;

	pcsample::TraceFunction( _T("END   >> CReportEOD::Reset() = %d"), TRUE );

    return ( TRUE );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::ResetAllRecords()
//
//  PURPOSE :   Resets all of records in total individually for 
//				an individual financial report
//
//  RETURN :    TRUE    - All of totals are resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::ResetAllRecords(
	CTtlRegFin& ttlRegFin,       // individual financial total to reset
	const UCHAR uchMajorClass
)
{
	BOOL    fSuccess = TRUE;
	BOOL    fResetAtEOD = TRUE;

	// --- reset current specified individual financial ---
	fSuccess = ttlRegFin.Reset(fResetAtEOD, uchMajorClass);
	if (!fSuccess)
	{
		// --- function is failed, and report its error ---
		m_pDoc->ReportError(CLASS_RPTREGFIN, pcsample::ERR_RESET, ttlRegFin.GetLastError());
	}

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::ResetAllRecords()
//
//  PURPOSE :   Resets all of records in total individually for 
//				an individual financial report
//
//  RETURN :    TRUE    - All of totals are resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::ResetAllRecords(
    CTtlIndFin& ttlIndFin,       // individual financial total to reset
	const UCHAR uchMajorClass
    )
{
    BOOL    fSuccess = TRUE;
    BOOL    fResetAtEOD = TRUE;

    // --- reset all of total records in master 2170 terminal ---
    for ( UCHAR uchI = 1; uchI <= m_uchNoOfTerminals; uchI++ )
    {
        // --- reset current specified individual financial ---
        fSuccess = ttlIndFin.Reset( uchI, fResetAtEOD, uchMajorClass );
        if ( ! fSuccess )
        {
            // --- function is failed, and report its error ---
            m_pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_RESET, ttlIndFin.GetLastError(), uchI );
        }
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::ResetAllRecords()
//
//  PURPOSE :   Resets all of records in total individually for a cashier
//
//  RETURN :    TRUE    - All of totals are resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::ResetAllRecords(
    CTtlCashier& ttlCashier,     // cashier total to reset
	const UCHAR uchMajorClass
    )
{
    BOOL            fSuccess;
    CParaCashier    paraCashier;
    ULONG           ausCashierNo[ CAS_NUMBER_OF_MAX_CASHIER ];
    USHORT          usNoOfPerson;

    // --- get all of existing cashier no. in 2170 ---
    fSuccess = paraCashier.GetAllCashierNo( ausCashierNo, sizeof( ausCashierNo ), usNoOfPerson );

    if ( fSuccess )
    {
        // --- reset all of existing total in 2170 ---
        for ( USHORT usI = 0; usI < usNoOfPerson; usI++ )
        {
            if ( ! ttlCashier.Reset( ausCashierNo[ usI ] ))
            {
                // --- function is failed. and report its error ---
                m_pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_RESET, ttlCashier.GetLastError(), ausCashierNo[ usI ] );
            }
        }
    }
    else
    {
        // --- function is failed ---
        m_sLastError      = pcsample::ERR_ALLIDREAD;
        m_sLastErrorLocus = CReportEOD::TTL_CASHIER;
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::ResetAllRecords()
//
//  PURPOSE :   Reset all of records in total individually for guest checks 
//
//  RETURN :    TRUE    - All of totals are resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CReportEOD::ResetAllRecords(
    CGuestCheck&    ttlGC,       // guest check to reset
	const UCHAR uchMajorClass
    )
{
    BOOL    fSuccess;
	USHORT  ausGCNo[GCF_MAX_GCF_NUMBER] = { 0 };
    USHORT  usNoOfGC;

    // --- get all of existing guest check no. in 2170 ---
    fSuccess = ttlGC.GetAllGCNo( ausGCNo, sizeof( ausGCNo ), usNoOfGC );

    if ( fSuccess )
    {
        // --- print header of guest check report ---
        m_pDoc->ReportHeader( ttlGC );

        // --- read and report all of existing guest check ---
        for ( USHORT usI = 0; usI < usNoOfGC; usI++ )
        {
            if ( ttlGC.Read( ausGCNo[ usI ] ))
            {
                // --- report retrieved guest check ---
                m_pDoc->ReportTotal( ttlGC );

                // --- reset retrieved guest check ---
                if ( ! ttlGC.Reset( ausGCNo[ usI ] ))
                {
                    // --- function is failed, and report its error ---
                    m_pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_RESET, ttlGC.GetLastError(), ausGCNo[ usI ] );
                }
            }
            else
            {
                // --- if total does not exist, it will be ignored ---
                if ( ttlGC.GetLastError() != GCF_NOT_IN )
                {
                    // --- other error occurs, report its error ---
                    m_pDoc->ReportError( CLASS_TTLGUEST, pcsample::ERR_TTLREAD, ttlGC.GetLastError(), ausGCNo[ usI ] );
                }
            }
        }
    }
    else
    {
        // --- if total does not exist, it will be ignored ---
        if ( ttlGC.GetLastError() == GCF_NOT_IN )
        {
            fSuccess = TRUE;
        }
        else
        {
            // --- other error occurs, report its error ---
            m_sLastError      = pcsample::ERR_ALLIDREAD;
            m_sLastErrorLocus = CReportEOD::TTL_GUEST;
        }
    }

    if ( fSuccess )
    {
        // --- reset delivery queue of drive through ---
        ttlGC.ResetDeliveryQueue();
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total register financial
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlRegFin& ttlRegFin       // register financial total to report
    )
{

	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_Financial.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
		// --- read all of resetted total ---
		m_pDoc->nSheetCol = m_pDoc->nSheetRow = 1;
	}

	// --- read resetted total ---
    if ( ttlRegFin.Read( CLASS_TTLSAVDAY ))
    {
		m_pDoc->pSavedTotalFile && m_pDoc->pSavedTotalFile->Write(ttlRegFin);
		// --- print header and body of resetted total ---
        m_pDoc->ReportHeader( ttlRegFin );
        m_pDoc->ReportTotal( ttlRegFin, m_pDoc->m_mdcMem );

        // --- set this total as enable to reset ---
        if ( ! ttlRegFin.ResetAsOK())
        {
            // --- function is failed, and report its error ---
            m_pDoc->ReportError( CLASS_TTLREGFIN, pcsample::ERR_TTLISSRST, ttlRegFin.GetLastError());
        }
    }
    else
    {
        // --- function is failed, and report its error ---
        m_pDoc->ReportError( CLASS_TTLREGFIN, pcsample::ERR_TTLREAD, ttlRegFin.GetLastError());
    }

	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();

}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlIndFin& ttlIndFin       // individual financial to report
    )
{
	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_FinancialInd.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
	}

	// --- read all of resetted total ---
	m_pDoc->nSheetCol = 1;
    for (UCHAR uchTerminalNo = 1; uchTerminalNo <= m_uchNoOfTerminals; uchTerminalNo++)
    {
        if ( ttlIndFin.Read( CLASS_TTLSAVDAY, uchTerminalNo ))
        {
			m_pDoc->pSavedTotalFile && m_pDoc->pSavedTotalFile->Write(ttlIndFin);
			if (m_pDoc->psqlDbFile && (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportDbFile) != 0) {
				m_pDoc->psqlDbFile->SetId (uchTerminalNo);
			}
			m_pDoc->nSheetRow = 1;
            // --- print header and body of resetted total ---
            m_pDoc->ReportHeader( ttlIndFin );
            m_pDoc->ReportTotal( ttlIndFin, m_pDoc->m_mdcMem);
			m_pDoc->nSheetCol += 4;

            // --- set this total as enable to reset ---
            if ( ! ttlIndFin.ResetAsOK( uchTerminalNo ))
            {
                m_pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_TTLISSRST, ttlIndFin.GetLastError(), uchTerminalNo );
            }
        }
        else
        {
            m_pDoc->ReportError( CLASS_TTLINDFIN, pcsample::ERR_TTLREAD, ttlIndFin.GetLastError());
        }
    }

	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlDept&   ttlDept         // department total to report
    )
{
    BOOL        fHeaderPrinted;
	int         iIndex;
    UCHAR       uchMajorDept;
    USHORT       usDeptNo;

	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_MajorDept.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
		// --- read all of resetted total ---
		m_pDoc->nSheetCol = m_pDoc->nSheetRow = 1;
	}

	// --- read all of total by major deparment number ---
    fHeaderPrinted = FALSE;
	m_pDoc->llReportTotalAmount = m_pDoc->lReportTotalCount = 0;
	iIndex = 0;
	for ( uchMajorDept = 0; uchMajorDept <= MAX_MDEPT_NO; uchMajorDept++ )
    {
		CParaDept   paraDept;

        // --- initialize before read function call ---
        paraDept.Initialize();

        // --- read parameter belongs to specified major deparment no. ---
        while ( paraDept.Read( uchMajorDept ))
        {
            // --- read total by retrieved dapertment no. ---
            usDeptNo = paraDept.getDeptNo();

            if ( ttlDept.Read( CLASS_TTLSAVDAY, usDeptNo ))
            {
				DEPTTBL  xDept = { 0 };
				TCHAR    szDeptName[OP_DEPT_NAME_SIZE + 1] = { 0 }; //CHAR to TCHAR ESMITH

				xDept.DEPTTotal = ttlDept.m_ttlDept.DEPTTotal;
				xDept.uchStatus = ttlDept.m_ttlDept.uchResetStatus;
				xDept.uchMDEPT = ttlDept.m_ttlDept.uchMajorDEPTNo;
				xDept.usDEPT = ttlDept.m_ttlDept.usDEPTNumber;
				_tcsncpy(xDept.auchMnemo, paraDept.getName(szDeptName), OP_DEPT_NAME_SIZE);

				m_pDoc->pSavedTotalFile && m_pDoc->pSavedTotalFile->Write(xDept, iIndex);
				iIndex++;

				if ( ! fHeaderPrinted )
                {
                    // --- print header of department report ---
                    m_pDoc->ReportHeader( ttlDept );
                    fHeaderPrinted = TRUE;
					m_pDoc->nSheetRow += 1;
                }

                // --- print body of department report ---
                m_pDoc->ReportTotal( ttlDept, paraDept );
				m_pDoc->nSheetRow += 1;
            }
        }
    }

	CString str;
	str.Format(_T("  TOTALS  \t\t\t\t\t\t\t\t\t\t %d\t%16s"), m_pDoc->lReportTotalCount, m_pDoc->MakeCurrencyString(m_pDoc->llReportTotalAmount));
	m_pDoc->SetReportString(str);
//	str.Format(_T("*** END OF DEPARTMENT DETAILS ***"));
//	m_pDoc->SetReportString(str);

	// --- set this total as enable to reset ---
    if ( ! ttlDept.ResetAsOK())
    {
        // --- function is failed, and report its error ---
        m_pDoc->ReportError( CLASS_TTLDEPT, pcsample::ERR_TTLISSRST, ttlDept.GetLastError());
    }

	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlPlu&    ttlPlu          // PLU total to report
    )
{
    CParaFlexMem    paraFlex;

    // --- get flexible memory size of 2170 ---
    if ( paraFlex.Read())
    {
        BOOL    fHeaderPrinted;
        BOOL    fSuccess;
        USHORT  usMaxDeptNo;
        USHORT  usDeptNo;

        // --- read and report total by department number ---
        fHeaderPrinted = FALSE;

		if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
			CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_PLU.slk"));
			m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

			CString str (L"ID;PPCSample\r\n");
			m_pDoc->SetSpreadSheetString (str);
			// --- read all of resetted total ---
			m_pDoc->nSheetCol = 1;
			m_pDoc->nSheetRow = 1;
			str.Format(_T("C;Y%d;X%d;K\"PLU No.\"\r\nC;Y%d;X%d;K\"PLU Type\"\r\nC;Y%d;X%d;K\"PLU Name\"\r\nC;Y%d;X%d;K\"PLU Dept.\"\r\nC;Y%d;X%d;K\"PLU Count\"\r\nC;Y%d;X%d;K\"PLU Amount\"\r\n"),
				m_pDoc->nSheetRow, m_pDoc->nSheetCol,
				m_pDoc->nSheetRow, m_pDoc->nSheetCol + 1,
				m_pDoc->nSheetRow, m_pDoc->nSheetCol + 2,
				m_pDoc->nSheetRow, m_pDoc->nSheetCol + 3,
				m_pDoc->nSheetRow, m_pDoc->nSheetCol + 4,
				m_pDoc->nSheetRow, m_pDoc->nSheetCol + 5);
			m_pDoc->SetSpreadSheetString (str);
			m_pDoc->nSheetRow++;
		}

		fSuccess    = TRUE;
        usDeptNo   = 1;
        usMaxDeptNo = (USHORT)paraFlex.getMemorySize( FLEX_DEPT_ADR );
        while ( fSuccess && ( usDeptNo <= usMaxDeptNo ))
        {
            fSuccess = pcsample::ReadAllRecords( CLASS_TTLSAVDAY, ttlPlu, usDeptNo, fHeaderPrinted, m_pDoc );
            usDeptNo++;
        }

        if ( fSuccess )
        {
            // --- set this total as enable to reset ---
            if ( ! ttlPlu.ResetAsOK())
            {
                // --- function is failed and report its error ---
                m_pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLISSRST, ttlPlu.GetLastError());
            }
        }
		char *pHeader = "E\r\n";
		if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

		if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
    }
    else
    {
        // --- function is failed and report its error ---
        m_pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraFlex.GetLastError());
    }

}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total for hourly activity
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlHourly& ttlHourly       // hourly activity total to report
    )
{
    // --- read resetted total ---
    if ( ttlHourly.Read( CLASS_TTLSAVDAY ))
    {
		if (m_pDoc->pSavedTotalFile) {
			TTLCSHOURLY ttlHourlyTemp = { 0 };

			ttlHourlyTemp.uchResetStatus = ttlHourly.m_ttlHourly.uchResetStatus;
			ttlHourlyTemp.FromDate = ttlHourly.m_ttlHourly.FromDate;
			ttlHourlyTemp.ToDate = ttlHourly.m_ttlHourly.ToDate;
			for (int i = 0; i < 48; i++) {
				ttlHourlyTemp.Total[i] = ttlHourly.m_ttlHourly.Total[i];
			}
			m_pDoc->pSavedTotalFile->Write(ttlHourlyTemp);
		}
		// --- print header and body of hourly activiy report ---
        m_pDoc->ReportHeader( ttlHourly );
        m_pDoc->ReportTotal( ttlHourly );

        // --- set this total as enable to reset ---
        if ( ! ttlHourly.ResetAsOK())
        {
            // --- function is failed and report its error ---
            m_pDoc->ReportError( CLASS_TTLHOURLY, pcsample::ERR_TTLISSRST, ttlHourly.GetLastError());
        }
    }
    else
    {
        // --- function is failed and report its error ---
        m_pDoc->ReportError( CLASS_TTLHOURLY, pcsample::ERR_TTLREAD, ttlHourly.GetLastError());
    }
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total for total cashier.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlCashier&    ttlCashier  // cashier total to report
    )
{
    CParaCashier    paraCashier;
    ULONG           ausCashierNo[ CAS_NUMBER_OF_MAX_CASHIER ];
    USHORT          usNoOfPerson;

	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_Cashier.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
	}

    // --- get all of existing cashier number in 2170 ---
    if ( paraCashier.GetAllCashierNo( ausCashierNo, sizeof( ausCashierNo ), usNoOfPerson ))
    {
		// --- read total by retrieved cashier number ---
		m_pDoc->nSheetCol = 1;
        for ( USHORT usI = 0; usI < usNoOfPerson; usI++ )
        {
			m_pDoc->nSheetRow = 1;
            if ( ttlCashier.Read( CLASS_TTLSAVDAY, ausCashierNo[ usI ] ))
            {
				m_pDoc->pSavedTotalFile && m_pDoc->pSavedTotalFile->Write(ttlCashier, usI);
				// --- print header and body of cashier report ---
                m_pDoc->ReportHeader( ttlCashier );
                m_pDoc->ReportTotal( ttlCashier, m_pDoc->m_mdcMem);

				m_pDoc->nSheetCol += 4;
                // --- set this total as enable to reset ---
                if ( ! ttlCashier.ResetAsOK( ausCashierNo[ usI ] ))
                {
                    // --- function is failed and report its error ---
                    m_pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_RESET, ttlCashier.GetLastError(), ausCashierNo[ usI ] );
                }
            }
        }
    }
    else
    {
        // --- function is failed and report its error ---
        m_pDoc->ReportError( CLASS_TTLCASHIER, pcsample::ERR_ALLIDREAD, paraCashier.GetLastError());
    }
	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total for total coupons
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlCoupon& ttlCoupon       // coupon total to report
    )
{
	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_Coupons.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
		// --- read all of resetted total ---
		m_pDoc->nSheetCol = m_pDoc->nSheetRow = 1;
	}
    // --- read and report resetted total ---
    if ( pcsample::ReadAllRecords( CLASS_TTLSAVDAY, ttlCoupon, m_pDoc ))
    {
        // --- set this total as enable to reset ---
        if ( ! ttlCoupon.ResetAsOK())
        {
            // --- function is failed and report its error ---
            m_pDoc->ReportError( CLASS_TTLCPN, pcsample::ERR_RESET, ttlCoupon.GetLastError());
        }
    }

	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total servie times,
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CTtlServTime& ttlServTime   // service time total to report
    )
{
	if (m_pDoc->ulReportOptionsMap & CPCSampleDoc::ulReportOptionsExportSylk) {
		CString fileName (m_pDoc->csSheetNamePrefix + _T("Totals_ServiceTime.slk"));
		m_pDoc->hSheetFile.Open (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		CString str (L"ID;PPCSample\r\n");
		m_pDoc->SetSpreadSheetString (str);
		// --- read all of resetted total ---
		m_pDoc->nSheetCol = m_pDoc->nSheetRow = 1;
	}
    // --- read resetted total ---
    if ( ttlServTime.Read( CLASS_TTLSAVDAY ))
    {
        // --- print header and body of service time total ---
        m_pDoc->ReportHeader( ttlServTime );
		m_pDoc->nSheetRow += 1;
        m_pDoc->ReportTotal( ttlServTime );
		m_pDoc->nSheetRow += 1;

        // --- set this total as enable to reset ---
        if ( ! ttlServTime.ResetAsOK())
        {
            // --- function is failed and report its error ---
            m_pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_TTLISSRST, ttlServTime.GetLastError());
        }
    }
    else
    {
        // --- function is failed and report its error ---
        m_pDoc->ReportError( CLASS_TTLSERVICE, pcsample::ERR_TTLREAD, ttlServTime.GetLastError());
    }

	char *pHeader = "E\r\n";
	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Write (pHeader, strlen(pHeader));

	if (!FAILED(m_pDoc->hSheetFile.m_hFile)) m_pDoc->hSheetFile.Close ();
}

//===========================================================================
//
//  FUNCTION :  CReportEOD::IssueReport()
//
//  PURPOSE :   Issue report of resetted total of Electronic Journal.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CReportEOD::IssueReport(
    CReportEJ&  reportEJ        // Electronic Journal object to report
    )
{
    // --- get current logged on terminal number ---
    USHORT  usCurrentTerminal;
    usCurrentTerminal = m_pMainFrame->GetLoggedOnTerminalNo();
	
	// --- read and report electronic journal by terminal no. ---
    UCHAR   uchTerminalNo;
    uchTerminalNo = 1;

	//see if is lan connected
	if( m_pMainFrame->IsLanConnected())
		{
			reportEJ.m_usTerminalNo = uchTerminalNo;
			reportEJ.m_usLineNo = 1;
			if ( m_pMainFrame->ReadLockedEJ( reportEJ ))
            {
                if ( ! reportEJ.Reset())
                {
                    // --- function is failed and report its error ---
                    m_pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_RESET, reportEJ.GetLastError());
                }
            }
		}
    else
	{
		while ( uchTerminalNo <= m_uchNoOfTerminals )
		{
			reportEJ.m_usTerminalNo = uchTerminalNo;
			reportEJ.m_usLineNo = 1;
			if ( m_pMainFrame->LogOnAndLockTerminal( uchTerminalNo ))
			{
				// --- read electronic journal in logged on terminal ---
				if ( m_pMainFrame->ReadLockedEJ( reportEJ ))
				{
					//--- reset electronic journal ---
					if ( ! reportEJ.Reset())
					{
					// --- function is failed and report its error ---
					m_pDoc->ReportError( CLASS_RPTEJ, pcsample::ERR_RESET, reportEJ.GetLastError());
					}
				}
            }
       }

		// --- increment to next terminal for next read function call ---
		uchTerminalNo++;

		 //--- restore to log on terminal ---
		m_pMainFrame->LogOn( usCurrentTerminal );
	}
	
	reportEJ.m_usTerminalNo = 0;
}


long long ParseCsvString (LPCTSTR pStr, LPTSTR pcRet, int iSize, long long *plList, LPCTSTR *pcList)
{
	long long val = 0, sign = 1;
	LPCTSTR  pStart = pcRet;

	if (!pcRet) iSize = 2;   // patch up iSize value to work if not using return string.

	while (*pStr && iSize > 1) {
		switch ((pcRet ? (iSize--, *pcRet++ = *pStr) : (*pStr))) {
			case ' ':
			case '\t':
				pStr++;
				break;
			case ',':
				val *= sign;                   // set the sign if we had a leading minus sign.
				if (plList) *plList = val;
				if (pcList) *pcList = pStart;
				if (pcRet) *(pcRet - 1) = 0;
				return ParseCsvString (pStr + 1, pcRet, iSize, (plList ? plList + 1 : plList), (pcList ? pcList + 1 : pcList));
				break;
			case '"':
				sign = 1;     // reset the sign in case it was previously set.
				pStr++;       // we have already processed this character so go to next.
				if (pcRet && iSize > 2) {
					while (*pStr && *pStr != '"' && iSize > 2) { *pcRet++ = *pStr++; iSize--; };
					if (*pStr == '"') { *pcRet++ = *pStr++; iSize--; }
				} else {
					while (*pStr && *pStr != '"') pStr++; 
					if (*pStr == '"') pStr++;
				}
				// fall through
				break;
			case '-':
				if (val == 0) sign *= -1;
				pStr++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				val = val * 10 + (*pStr - '0');
				pStr++;
				break;
			default:
				sign = 1;     // reset the sign in case it was previously set.
				pStr++;
				break;
		}
	}

	val *= sign;                   // set the sign if we had a leading minus sign.
	if (pcRet) *pcRet = 0;
	if (plList) *plList = val;
	if (pcList) *pcList = pStart;
	return val;
}


void CReportEOD::ExportDbToFiles(CString &csActivity)
{
	int rc = 0;

	CString fileNameRoot (csActivity);
	fileNameRoot.Replace(_T(":"), _T("-"));
	fileNameRoot.Replace(_T("."), _T("_"));


	{
		CSqlitedbRegFin  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_Financial.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbIndFin  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_FinancialInd.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbCashierTotals  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_Cashier.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbDept  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_dept.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbCouponTotals  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_coupon.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbPluTotals  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_plu.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}

	{
		CSqlitedbElecJournal  myDbFile;

		myDbFile.OpenFile (_T("pcsample.db"));
		m_pDoc->psqlDbFile = &myDbFile;

		myDbFile.m_CurrKey = csActivity;
		myDbFile.ListSelect();

		CString fileName (fileNameRoot);
		fileName += _T("_Totals_electjournal.csv");

		CFile myOutFile (fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);

		POSITION pos = myDbFile.myListx.GetHeadPosition();

		while (pos != NULL) 
		{
			CString &xstrng = myDbFile.myListx.GetNext (pos);
			char  xBuff[256];
			sprintf (xBuff, "%S\r\n", (LPCTSTR) xstrng);
			myOutFile.Write (xBuff, strlen (xBuff));
		}

		m_pDoc->psqlDbFile = 0;  // we are done with the database. let the CSqlitedb object clean up after itself.
	}
}

/////////////////// END OF FILE ( ReportEOD.cpp ) ///////////////////////////
