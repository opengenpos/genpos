// PluTtlDb.h: interface for the CnPluTotalDb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)
#define AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef XP_PORT  //building for Win2000/XP
#include "CnAdoXP.h"
#else  //building for CE
#include "CnAdoCe.h"
#endif

#include "PluTtlD.h"

// *** Index define table ***
typedef struct{
	CString	strName;
	CString	strFormat;
}__CnIdxFormat;

// *** Table define table ***
typedef struct {
	SHORT	nId;			// reserved
	CString	strName;		// table mame
	CString	strFldFormat;	// -> CREATE TABLE(SQL)
	CString	strIdxName;		// index name
	CString	strIdxFld[3];	// -> CREATE INDEX(SQL)
	CString	strOptions;		// option string(reserved)
}__CnTblFormat;

const	int	c_nTblNumMax = 10;

//#define	RECORDSET_NUM	(2)				// max recoredset num
//#define	__pRecS		s_pRec[0]					// cursor
//#define	__pRecO		s_pRec[1]					// one shot

class	CnAdoXPRec;

class CnPluTotalDb
{
private:
	static	int			s_nObjCnt;					// object counter
//	static	CnAdoXPRec* s_pRec[RECORDSET_NUM];	// recordset (ADOCE)
	static	CnAdoXPRec* __pRecS;	// recordset (ADOCE)
	static	CnAdoXPRec* __pRecO;	// recordset (ADOCE)
protected:
	CnVariant			m_vAC_DbFileName;			// ACTIVE CONNECTION (ADOCE)
public:
	CString				m_strDbFileName;			// database name (???.cdb)
#ifdef XP_PORT
private:
	CString				ConnectionStringTemp; 		//= L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=(local);DATABASE=%s;UID=; Password=;";
	_bstr_t				ConnectionStringNoDB; 		//= L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=(local);DATABASE=;UID=; Password=;";
public:
	CString				ConnectionString;			//the database connection string for the XP port
#endif
	HRESULT				m_hr;						// status (ADOCE)
	static const int	s_IdxNum;

private:
	CnPluTotalDb(){};

protected:
	void	CreateObject(const LPCTSTR szDbFileName);	// init this object
	void	DestroyObject();						// destroy
													// Where statement
public:
	CnPluTotalDb(const LPCTSTR szDBFileName);		// standard constractor
	virtual ~CnPluTotalDb();						// destractor

	HRESULT			GetLastError() const{ return m_hr; };
													// returning ADO error code

	BOOL			MakeSQLWhere(const CnVariant &vFields,const CnVariant &vValues,CString &strWhere);

	// create
	virtual	ULONG	CreateDbFile();					// create database file
	virtual	ULONG	CreateDbBackUpFile();			// create database file
	virtual	ULONG	DropDbFile();					// delete database

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
	virtual	ULONG	DelRec(const LONG lRecNum = 1);

	// cursor type
	virtual	ULONG	OpenRecoredset(const LPCTSTR szSqlCode,CursorTypeEnum CursorType = adOpenKeyset,
					LockTypeEnum LockType = adLockOptimistic,long lOption = adCmdText);
													// open cursor
	virtual	ULONG	CloseRecoredset(void);			// close cursor
	virtual	ULONG	MoveFirst(void);				// go to first record
	virtual	ULONG	MoveLast(void);					//       last
	virtual	ULONG	MoveNext(void);					//       next
	virtual	ULONG	MovePrev(void);					//       previous

	virtual	ULONG	GetRow(const CnVariant vFldNames,CnVariant * vValues);
	virtual	ULONG	get_RecordCnt(LONG * plRecCnt);
													// Recordset->getRecoedCount
	virtual	ULONG	ExecSQL(LPCTSTR szSqlCode);
	virtual ULONG   RestoreDB(LPCTSTR szSQLCode);

#ifdef	___GOMI
	// *** Find mode
	static const ULONG			s_NoClose;
	static const ULONG			s_NoGet;

// ### DEL
//	CFile				m_fDbFile;					// database file object
//	CFileException		m_fError;					// file exception object
	// *** open modes
/* ### DEL 061600
	static	const UINT			s_ModeCreate;
	static	const UINT			s_ModeRead;
	static	const UINT			s_ModeWrite;
	static	const UINT			s_ShareExclusive;
	static	const UINT			s_TypeBin;
*/
/*### 061600
	// OldFunctions
	virtual	ULONG	Insert(const LPCTSTR lpszTableName,const CnVariant &varFldOrds,const CnVariant &varFldData);
	virtual	ULONG	Delete(const LPCTSTR lpszTableName,const CnVariant &varFldOrds,const CnVariant &varFldData);
	virtual	ULONG	Update(const LPCTSTR lpszTableName,const CnVariant &varFldOrds,const CnVariant &varFldData);
	virtual	ULONG	Update(const LPCTSTR lpszTableName,const CnVariant &vFldNames,const CnVariant &vFldValues,const CnVariant &vUpdateFldNames,CnVariant &vUpdateData);
	virtual	ULONG	Find(const LPCTSTR lpszTableName,const CnVariant &vFldNames,const CnVariant &vFldValues,CnVariant &vRecData,const ULONG nOption = 0);
*/
#ifdef	___DEL_20000407
	// back up methods (file)
	virtual	ULONG	OpenDbFile(const ULONG ulOpenMode);
	virtual	ULONG	CloseDbFile();
    virtual	ULONG	MoveDbFile(LPCTSTR lpszToName);
	virtual	ULONG	ReadDbFile(const ULONG ulBytes,VOID *byBuf,ULONG *pulReadBytes);
	virtual	ULONG	WriteDbFile(const ULONG ulBytes,const VOID *byBuf);
	// logfile
	void			PutLog(LPCTSTR szLabel,const ULONG ulBytes,const ULONG ulSts);
#endif	// ___DEL_20000407
#endif	// ___GOMI

};

#endif // !defined(AFX_PLUTTLDB_H__3D31ECC6_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)
