// PluTtlM.h: interface for the CnPluTotal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUTTLM_H__3D31ECC5_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)
#define AFX_PLUTTLM_H__3D31ECC5_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"PluTtlDb.h"
//#include	"PluTtlRg.h"
#include	"PluTtlBk.h"

//#include	"atlbase.h"

#define		DBFILE_NUM	(5)
//#define		DBFILE_NUM	(4)
#define		STSTBL_NUM	DBFILE_NUM			/* number of status */// ### ADD
#define		TBL_NUM		(10)

// *** Database define table ***
class	CnPluTotalDb;

typedef struct{
	SHORT	nId;								// reserved
	CString	strPath;							// db file path	// ### ADD Saratoga (061600)
	CString	strName;							// db file name
	__CnTblFormat	lstTbl[TBL_NUM];			// table list in db file
	CnPluTotalDb	* pDb;						// database class(ADOCE)
}__CnDbFormat;

// *** Status table chash ***
typedef struct{
	SHORT			nStsTblIdx;
	PLUTOTAL_STATUS	Status;
	SHORT			nStsUpdate;	// 08/30/01
}__StatusBuf;


typedef	CnPluTotalDb *	PDBObject;

class CnPluTotal
{
private:
	static	int		s_nObjCnt;
	__CnDbFormat	s_lstDb[DBFILE_NUM+1] ;
protected:
	SHORT			m_nCurrentDbFile;				// (table id)
	CFile			m_fDbFile;						// database file object
	CFileException	m_feErrorInfo;					// file exception object

	CnPluTtlReg		m_Regs;
//	CString			m_strDbPath;

	CnPluTotalBackUp m_BackUp;						// TBC object

	// status buffer
	__StatusBuf		m_StatusBuf[STSTBL_NUM];

protected:
	virtual BOOL	MakeParmsPluTotals(COleSafeArray& saFields);
	virtual BOOL	MakeParmsPluStatus(COleSafeArray& saFields);
	virtual	BOOL	MakeParams(const SHORT nTblID,COleSafeArray &vFields);

	BOOL			Var2PluTtlRec(const CnVariant vRecData,PLUTOTAL_REC * pRecData);
	BOOL			Var2PluTtlRec(const CnVariant vRecData,PLUTOTAL_STATUS * pStsData);
													// variant -> PLUTOTAL_REC/PLUTOTAL_STATUS
	BOOL			Var2PluTtlDate(const CnVariant &vDate,PLUTOTAL_DATE * pDate);
	BOOL			PluTtlDate2Var(const PLUTOTAL_DATE &pdDate,CnVariant  &vDate);

	ULONG			LoadStatus(const SHORT nStsTblID,PLUTOTAL_STATUS * pStatusRec);
	ULONG			SaveStatus(const SHORT nStsTblID,const PLUTOTAL_STATUS &StatusRec);
	BOOL			DelStatusBuf(const SHORT nStsTblID);

public:
					CnPluTotal();
	virtual 		~CnPluTotal();

	virtual	ULONG	OpenDatabase(const BOOL bCreateTbl = TRUE, SHORT fPTD = TRUE, SHORT fTmp = TRUE);
	virtual	ULONG	CloseDatabase();

	virtual	ULONG	GetRecordCnt(const SHORT nTblID,LONG * plRecNum);

	virtual ULONG	CreateTable(const SHORT nTblID,const ULONG ulRecNum);
	virtual ULONG	DropTable(const SHORT nTblID);
	virtual	ULONG	MoveTableN(const SHORT nFromTblID,const SHORT nToTblID);
	virtual	ULONG	ExportTableToFile (const SHORT nFromTblID, const TCHAR *pFileName);
	virtual	ULONG	ClearTable(const SHORT nTblID);
	virtual	ULONG	CheckTable(const SHORT nTblId);

	virtual ULONG	CreateIndex(const SHORT nTblID);
	virtual ULONG	DropIndex(const SHORT nTblID);

	// ### NewFunctions
	virtual	ULONG	InsertN(const SHORT nTblID,const PLUTOTAL_REC &RecData);
	virtual	ULONG	DeleteN(const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx);
	virtual	ULONG	Find(const SHORT nTblID,const TCHAR pPluNo[],const SHORT sAdjIdx,const BOOL bClose,long * plRecNum);
	virtual	ULONG	Find(const SHORT nTblID,const ULONG ulFindVal);
	virtual	ULONG	FindN(const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC * pRecData);
	virtual	ULONG	UpdateN(const SHORT nTblID,const PLUTOTAL_REC &RecData);

	virtual	ULONG	dbg_ExecSQL(const SHORT nTblID,LPCTSTR szSqlCode);

	virtual	ULONG	SelectRec(const SHORT nTblID,const ULONG SearchCond);
	virtual	ULONG	FirstRec(const SHORT nTblID);
	virtual	ULONG	LastRec(const SHORT nTblID);
	virtual	ULONG	NextRec(const SHORT nTblID);
	virtual	ULONG	PrevRec(const SHORT nTblID);
	virtual	ULONG	GetRec(const SHORT nTblID,PLUTOTAL_REC * pRecData);
	virtual	ULONG	GetNumRec(const SHORT nTblID,ULONG * pulRecNum);
	virtual	ULONG	ReleaseRec(const SHORT nTblID);

	virtual	ULONG	GetStsInfo(const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec);
	virtual	ULONG	SetStsInfo(const SHORT nTblID,const PLUTOTAL_STATUS StatusRec, BOOL bSave);

	virtual	ULONG	OpenDBFile(const SHORT nTblID,const ULONG ulOption);
	virtual ULONG   RestoreDB(const SHORT nTblID);    //SR 725 JHHJ AC42 Plu Database Synchronization
	virtual	ULONG	CloseDBFile();
	virtual	ULONG	ReadDbFile(const ULONG ulOffset, const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes);
	virtual	ULONG	WriteDbFile(const ULONG ulOffset, const ULONG ulBytes,const VOID *pBuf);
	// ### ADD (041100)
	ULONG			DeleteDbFile(const SHORT nTblID);
	ULONG			MoveDbFile(const SHORT nFromTblID, const SHORT nToTblID);
	ULONG			CopyDbFile(const SHORT nFromTblID, const SHORT nToTblID);
	// ### ADD (053100)
	ULONG			StartBackup(void);
	ULONG			StartRestore(void);
	ULONG			GetBackupStatus(void);

	// log
	void			PutLog(LPCTSTR szLabel,const SHORT nTblID,const PLUTOTAL_REC &RecData,const ULONG ulSts);
	void			PutLog(LPCTSTR szLabel,const SHORT nTblID,const char aucPluNumber,const ULONG ulSts);
	void			PutLog(LPCTSTR szLabel,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec,const ULONG ulSts);
	void			PutLog(LPCTSTR szLabel, const ULONG ulBytes, const ULONG ulSts, CFileException* pEx = NULL);
	void			PutLog(LPCTSTR szLabel, const ULONGLONG ullBytes, const ULONG ulSts, CFileException* pEx = NULL);

	// *** DEBUG & TESTS (041500)
	ULONG			__DbgMakeTestData(const SHORT nTblID,const LONG lFrom,const LONG lTo,const LONG lStep = 1);

	HRESULT				m_pDB_hr;						// status (ADOCE)
	HRESULT				m_pStsDB_hr;					// status (ADOCE)

protected:
	virtual void	CreateObject();
	virtual void	DestroyObject();
	BOOL			GetTblFormat();
	int				SearchTbl(const SHORT sTblID,int *nTblIdx);
	SHORT			GetStatusTblID(const SHORT sTblID)const;
	void            SetPluTotalStatusVal (ULONG ulResetSts,DATE_TIME *dtFrom,DATE_TIME *dtTo, DCURRENCY lAllTotal, DCURRENCY lAmount, PLUTOTAL_STATUS *ppsRec);
private:
	LPCTSTR			_TableName(const SHORT nTblID) ;
	CString			_DbPathName(const SHORT nTblID);
	CString			_DbFileName(const SHORT nTblID); //SR 725 JHHJ AC42 Plu Database Synchronization
//	### DEL LPCTSTR			_FileName(const SHORT nTblID) ;
	CnPluTotalDb*	_DataBaseObject(const SHORT nTblID) ;
	__CnTblFormat&	_TableInfo(const SHORT nTblID) ;
	int				_StsBufIdx(const SHORT sStsTblID = -1)const;	// ### ADD (032400)
	// *** DEBUG & TESTS (041500)
	void			__GenPluNo(const LONG lNo,LPTSTR pPluNo);
};

#endif // !defined(AFX_PLUTTLM_H__3D31ECC5_B1CB_11D3_BE4A_00A0C961E76F__INCLUDED_)





#ifdef	__GOMI
	/* ### Old
	virtual	BOOL	MakeSearchParams(CnVariant &vFields,CnVariant &vValues);
	virtual	BOOL	MakeParams(const PLUTOTAL_REC &RecData,COleSafeArray &vFields,COleSafeArray &vData);
	virtual	BOOL	MakeParams(const PLUTOTAL_STATUS &RecData,COleSafeArray &vFields,COleSafeArray &vData);

	virtual	BOOL	MakeParams(const TCHAR pPluNo[],const BYTE byAdjIdx,COleSafeArray &vFields,COleSafeArray &vData);
	virtual	ULONG	MoveTable(const SHORT nFromTblID,const SHORT nToTblID);
	virtual	ULONG	Insert(const SHORT nTblID,const PLUTOTAL_REC &RecData);
	virtual	ULONG	Delete(const SHORT nTblID,const UCHAR pPluNo[],const BYTE byAdjIdx);
	virtual	ULONG	Update(const SHORT nTblID,const PLUTOTAL_REC &RecData);
	virtual	ULONG	Find(const SHORT nTblID,const UCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC * pRecData);
	*/

#endif
