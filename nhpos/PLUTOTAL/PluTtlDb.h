// PluTtlDb.h: interface for the CnPluTotalDb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)
#define AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//building for Win2000/XP
#include "CnAdoXP.h"

#include "PluTtlD.h"

// *** Index define table ***
struct __CnIdxFormat {
	CString	strName;
	CString	strFormat;
};

// *** Table define table ***
struct __CnTblFormat {
	SHORT	nId;			// reserved
	CString	strName;		// table mame
	CString	strFldFormat;	// -> CREATE TABLE(SQL)
	CString	strIdxName;		// index name
	CString	strIdxFld[3];	// -> CREATE INDEX(SQL)
	CString	strOptions;		// option string(reserved)
};

const	int	c_nTblNumMax = 10;


class	CnAdoXPRec;

class CnPluTotalDb
{
private:
	static	int			s_nObjCnt;					// object counter
//	static	CnAdoXPRec* s_pRec[RECORDSET_NUM];	// recordset (ADOCE)
#if defined(SQLITE_TEST) && SQLITE_TEST
// replace the real objects with fake objects to eliminate
// compiler errors when stubbing out database interface.
	static int * __pRecS;	// recordset (ADOCE)
	static int * __pRecO;	// recordset (ADOCE)
#else
	static	CnAdoXPRec* __pRecS;	// recordset (ADOCE)
	static	CnAdoXPRec* __pRecO;	// recordset (ADOCE)
#endif
	CString             ConnectionStringDbPath;     // used with SQLite only to create database file using CFile since SQLite doesn't support CREATE DATABASE 
	CString				ConnectionStringTemp; 		//= L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=(local);DATABASE=%s;UID=; Password=;";
	_bstr_t				ConnectionStringNoDB; 		//= L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=(local);DATABASE=;UID=; Password=;";

	CnPluTotalDb();                                 // prevent default constructor

	void	CreateObject(const LPCTSTR szDbFileName);	// init this object
	void	DestroyObject();						// destroy

public:
	CString				m_strDbFileName;			// database name (???.cdb)
	CString				ConnectionString;			//the database connection string for the XP port
	HRESULT				m_hr;						// status (ADOCE)
	static const int	s_IdxNum;

	CnPluTotalDb(const LPCTSTR szDBFileName);		// standard constractor
	virtual ~CnPluTotalDb();						// destractor

	HRESULT			GetLastError() const{ return m_hr; };
													// returning ADO error code


	// create
	virtual	ULONG	CreateDbFile();					// create database file
	virtual	ULONG	CreateDbBackUpFile();			// create database file

	virtual	ULONG	CreateTable(const __CnTblFormat &TblFormat);
													// create table in database
	virtual ULONG	DropTable(LPCTSTR szTblName);
													// delete table
	// index
	virtual	ULONG	CreateIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName,LPCTSTR lpszFldNames);
	virtual	ULONG	DropIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName);

	virtual	ULONG	CreateIndexEx(const __CnTblFormat &TblFormat);
    virtual ULONG   DropIndexEx(const __CnTblFormat &TblFormat);

	// one shot type
	virtual	ULONG	CheckTable(LPCTSTR lpcTblName);
	virtual	ULONG	get_RecordCnt(LPCTSTR szTblName,LONG * plRecCnt);

	// ### NewFunctions
	virtual	ULONG	OpenRec(LPCTSTR szTableName);
	virtual	ULONG	OpenRec(LPCTSTR szSqlCode,const BOOL bClose,LONG *plRecCnt);
	virtual	VOID	CloseRec();
	virtual	ULONG	GetRec(VARIANT vGetFields,LPVARIANT lpvValues);
	virtual	ULONG	PutRec(VARIANT vPutFields,VARIANT vValues);
	virtual	ULONG	AddRec(VARIANT vPutFields,VARIANT vValues);
	virtual	ULONG	DelRec(void);

	// cursor type
	virtual	ULONG	OpenRecoredset(const LPCTSTR szSqlCode,CursorTypeEnum CursorType = adOpenKeyset,
					LockTypeEnum LockType = adLockOptimistic,long lOption = adCmdText);
													// open cursor
	virtual	ULONG	CloseRecoredset(void);			// close cursor
	virtual	ULONG	MoveFirst(void);				// go to first record
	virtual	ULONG	MoveNext(void);					//       next

	virtual	ULONG	GetRow(const CnVariant vFldNames,CnVariant * vValues);
	virtual	ULONG	get_RecordCnt(LONG * plRecCnt);
													// Recordset->getRecoedCount
	virtual	ULONG	ExecSQL(LPCTSTR szSqlCode);
	virtual ULONG   RestoreDB(LPCTSTR szSQLCode);

};

#endif // !defined(AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)
