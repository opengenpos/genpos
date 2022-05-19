
#include	"stdafx.h"	
#include	"PluTtlM.h"


class	CnPluTotal;

class	CnPluTotalAPI
{
private:
	static int				s_nObjCnt;			// object counter
protected:
	int						m_nMyObjNo;
	PLUTOTAL_DB_HANDLE		m_hdDb;				// databases handle
	PLUTOTAL_FILE_HANDLE	m_hdDbFile;			// file handle

	CnPluTotal				m_pPluTotal;

public:
//static const	int	c_nDBNULL;				
static const	int	c_nOBJMAX;				

protected:
	static int	__GetObjNum(){return s_nObjCnt;};
	PLUTOTAL_DB_HANDLE		__DBHandle(){return (PLUTOTAL_DB_HANDLE)m_nMyObjNo;};
	PLUTOTAL_FILE_HANDLE	__DBFileHandle(){return (PLUTOTAL_FILE_HANDLE)(m_nMyObjNo + 100);};

	BOOL	CheckDBHandle(const PLUTOTAL_DB_HANDLE hdDb) const { return (BOOL)(hdDb == m_hdDb);};
	BOOL	CheckDBFileHandle(const PLUTOTAL_FILE_HANDLE hdFile) const { return (BOOL)(hdFile == m_hdDbFile);};

public:
	CnPluTotalAPI(){
		m_hdDb = PLUTOTAL_HANDLE_NULL;
		//m_pfDbFile = (CFile *)NULL;
		s_nObjCnt++;
		m_nMyObjNo = s_nObjCnt;
	};
	virtual ~CnPluTotalAPI(){
		s_nObjCnt--;
	};

	ULONG	Open(PLUTOTAL_DB_HANDLE * phdDB, SHORT fPTD, SHORT fTmp);
	ULONG	Close(const PLUTOTAL_DB_HANDLE hdDB);
	ULONG	GetRecordCnt(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,ULONG *ulRecCnt);

	ULONG	CreateTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const ULONG ulRecNum);
	ULONG	DropTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId);
	ULONG	MoveTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nFromId,const SHORT nToId);
	ULONG	ExportTableToFile (const PLUTOTAL_DB_HANDLE hdDB, const SHORT nFromId, const TCHAR *pFileName);
	ULONG	CheckTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblId);

	ULONG	Insert(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const PLUTOTAL_REC &RecData);
	ULONG	Delete(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const TCHAR pPluNo[],const BYTE byAdjIdx);
	ULONG	Update(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const PLUTOTAL_REC &RecData);
	ULONG	Find(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC *RecData);

	ULONG	dbg_ExecSQL(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const CHAR * szSqlCode);

	ULONG	SelectRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG SearchCond);
	ULONG	FirstRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
	ULONG	LastRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
	ULONG	NextRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
	ULONG	PrevRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
	ULONG	GetRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,PLUTOTAL_REC *pRecData);
	ULONG	GetNumRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblId,ULONG *pulRecNum);
	ULONG	ReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblId);

	ULONG	GetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec);
	ULONG	SetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec);

	ULONG	OpenDBFile(PLUTOTAL_FILE_HANDLE * phdDBFile,const SHORT nTblID,const ULONG ulOption);
	ULONG	RestoreDB(const PLUTOTAL_FILE_HANDLE phdDBFile, const SHORT nTblID);
	ULONG	CloseDBFile(const PLUTOTAL_FILE_HANDLE hdDBFile);
	ULONG	ReadDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG ulOffset, const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes);
	ULONG	WriteDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile, const ULONG ulOffset, const ULONG ulBytes,const VOID *byBuf);
	// ### ADD (04112000)
	ULONG	DeleteDbFile(const SHORT nTblID);
	ULONG	MoveDbFile(const SHORT nFromTblID, const SHORT nToTblID);
	ULONG	CopyDbFile(const SHORT nFromTblID, const SHORT nToTblID);

	// ### ADD (053100)
	ULONG	StartBackup(void);
	ULONG	StartRestore(void);
	ULONG	GetBackupStatus(void);

	// DEBUG & TESTS (041500)
	ULONG	__DbgMakeTestData(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const LONG lFrom,const LONG lTo,const LONG lStep = 1);

};


