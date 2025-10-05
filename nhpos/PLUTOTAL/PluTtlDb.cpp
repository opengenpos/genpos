// PluTtlDb.cpp: implementation of the CnPluTotalDb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluTtlDb.h"

#include "NDbgTrc.h"    // TRACER object

#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int         CnPluTotalDb::s_nObjCnt;                // object counter

// following define is used to enable ASSRTLOG file logs.
#define CNPLUTOTALDB_LOG  0

#if !defined(ENABLE_EMBED_SQLITE_DB)
#if defined(SQLITE_TEST) && SQLITE_TEST
// replace the real objects with fake objects to eliminate
// compiler errors when stubbing out database interface.
// used for testing with Linux using WINE.
int * CnPluTotalDb::__pRecS;	// recordset (ADOCE)
int * CnPluTotalDb::__pRecO;	// recordset (ADOCE)
#else
CnAdoXPRec* CnPluTotalDb::__pRecS;	// recordset (ADOCE)
CnAdoXPRec* CnPluTotalDb::__pRecO;	// recordset (ADOCE)
#endif
#endif

const int   CnPluTotalDb::s_IdxNum         = 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if !defined(ENABLE_EMBED_SQLITE_DB)
CnPluTotalDb::CnPluTotalDb(const LPCTSTR szDbFileName) {
#else
CnPluTotalDb::CnPluTotalDb(const LPCTSTR szDbFileName) {
#endif
//
// See also the Microsoft article Driver history for Microsoft SQL Server
//    https://learn.microsoft.com/en-us/sql/connect/connect-history?view=sql-server-ver15
//
#if defined(USE_PLU_TOTALS_SQLSERVER)
	// We are using shared memory protocol for our connection string.  Change made for
	// Amtrak because the tablet with its wireless cell card was causing GenPOS to be unable
	// to connect to the MSDE or SQL Server instance under some circumstances.
	ConnectionStringTemp = L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=lpc:(local);DATABASE=%s;UID=; Password=;";
	ConnectionStringNoDB = L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=lpc:(local);DATABASE=;UID=; Password=;";
#elif defined(USE_PLU_TOTALS_SQLITE)
    // we've added ConnectionStringDbPath because SQLite uses a file for a database
    // and does not support the CREATE DATABASE SQL command. to create a database
    // for SQLite we use the CFILE class to check with an open in read mode if the
    // file exists and if that fails then we create the file. See CnPluTotalDb::CreateDbFile().
    ConnectionStringDbPath = L"\\FlashDisk\\NCR\\Saratoga\\Database\\%s.db";
	ConnectionStringTemp = L"DRIVER=SQLite3 ODBC Driver;Database=\\FlashDisk\\NCR\\Saratoga\\Database\\%s.db;LongNames=0;Timeout=1000;NoTXN=0;SyncPragma=NORMAL;StepAPI=0;";
	ConnectionStringNoDB = L"DRIVER=SQLite3 ODBC Driver;LongNames=0;Timeout=1000;NoTXN=0;SyncPragma=NORMAL;StepAPI=0;";
#elif defined(USE_PLU_TOTALS_LOCALDB)
    ConnectionStringTemp = L"Provider=MSOLEDBSQL; Server=(localdb)\\MSSQLLocalDB;Integrated Security=true;DataTypeCompatibility=80;AttachDbFileName=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.mdf;";
    ConnectionStringNoDB = L"Provider=MSOLEDBSQL; Server=(localdb)\\MSSQLLocalDB;Integrated Security=true;DataTypeCompatibility=80;AttachDbFileName=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.mdf";
#elif defined(USE_PLU_TOTALS_JET)
	// See https://stackoverflow.com/questions/6174724/problem-with-oledbconnection-excel-and-connection-pooling
	// OLE DB Services = -4;Persist Security Info=True;User Id=admin;Password=;
	ConnectionStringTemp = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=\\FlashDisk\\NCR\\Saratoga\\Database\\%s.mdb;OLE DB Services = -4;Persist Security Info=True;";
	ConnectionStringNoDB = L"Provider=Microsoft.Jet.OLEDB.4.0;OLE DB Services = -4;Persist Security Info=True;";
#elif defined(USE_PLU_TOTALS_SQLSERVER_EMBEDDED)
//	ConnectionStringTemp = L"Driver={SQL Server Native Client 10.0};Server=(local);Database=%s;Trusted_Connection=yes;";
//	ConnectionStringNoDB = L"Driver={SQL Server Native Client 10.0};Server=(local);Database=;Trusted_Connection=yes;";
	ConnectionStringTemp = L"Provider=Microsoft.SQLSERVER.MOBILE.OLEDB.3.0;Data Source=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.sdf;";
	ConnectionStringNoDB = L"Provider=Microsoft.SQLSERVER.MOBILE.OLEDB.3.0;Data Source=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.sdf;";
#endif
    CreateObject(szDbFileName);
}


CnPluTotalDb::~CnPluTotalDb(){
    DestroyObject();
}


#if !defined(ENABLE_EMBED_SQLITE_DB)
void    CnPluTotalDb::CreateObject(LPCTSTR szDbFileName){

    m_strDbFileName = szDbFileName;
    ConnectionString.Format(ConnectionStringTemp,m_strDbFileName);

#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    if (__pRecS == NULL)
        __pRecS = new int;
    if (__pRecO == NULL)
        __pRecO = new int;
#else
    if(__pRecS == NULL)
        __pRecS = new CnAdoXPRec;
    if(__pRecO == NULL)
        __pRecO = new CnAdoXPRec;
#endif
    CnPluTotalDb::s_nObjCnt++;
}
#else
void    CnPluTotalDb::CreateObject(LPCTSTR szDbFileName) {

    m_strDbFileName = szDbFileName;
    ConnectionString.Format(ConnectionStringDbPath, m_strDbFileName);

    char sqlBuf[256] = { 0 };
    snprintf(sqlBuf, sizeof(sqlBuf), "%S", (LPCWSTR)ConnectionString);

    int rc = sqlite3_open(sqlBuf, &cnRec0.pSqliteDb);
    if (rc != SQLITE_OK) {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::CreateObject() %S rc = %d", (LPCWSTR)m_strDbFileName, rc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }

    rc = sqlite3_open(sqlBuf, &cnRecS.pSqliteDb);
    if (rc != SQLITE_OK) {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::CreateObject() %S rc = %d", (LPCWSTR)m_strDbFileName, rc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }

    cnRecS.prepTotalDbKeyFields.CreateOneDim(VT_VARIANT, 6);
    cnRecS.prepTotalDbKeyValues.CreateOneDim(VT_VARIANT, 6);

    cnRec0.prepTotalDbKeyFields.CreateOneDim(VT_VARIANT, 6);
    cnRec0.prepTotalDbKeyValues.CreateOneDim(VT_VARIANT, 6);

    CnPluTotalDb::s_nObjCnt++;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
void    CnPluTotalDb::DestroyObject(){
    CnPluTotalDb::s_nObjCnt--;
    if(CnPluTotalDb::s_nObjCnt == 0){
        if(__pRecS != NULL){
            delete  __pRecS;
            __pRecS = NULL;
        }
        if(__pRecO != NULL){
            delete  __pRecO;
            __pRecO = NULL;
        }
    }
}
#else
void    CnPluTotalDb::DestroyObject() {
    CnPluTotalDb::s_nObjCnt--;

    sqlite3_close(cnRecS.pSqliteDb);
    cnRecS.pSqliteDb = NULL;

    sqlite3_close(cnRec0.pSqliteDb);
    cnRec0.pSqliteDb = NULL;
}
#endif

ULONG   CnPluTotalDb::CreateDbFile(){

#if defined(CNPLUTOTALDB_LOG) && CNPLUTOTALDB_LOG
    {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::CreateDbFile() %S", (LPCWSTR)m_strDbFileName);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }
#endif

#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else

#if defined(USE_PLU_TOTALS_SQLITE)
    // for SQLite which does not support the CREATE DATABASE SQL directive
    // we have to create a file with the name of the database to allow
    // SQLite to then create the various tables in it.

    CString dbPath;
    dbPath.Format(ConnectionStringDbPath, m_strDbFileName);

    CFile f;

    bool bSts = f.Open(dbPath, CFile::modeRead);
    if (bSts) {
        // if we are able to open the file in read mode then the file already
        // exists. it may have PLU data in it so just close it and we are done.
        f.Close();
    }
    else {
        // SQLite does not support the SQL statement CREATE DATABASE.
        // A database is a separate file so we need to create an empty file
        // and then let SQLite initialize it properly.
        bSts = f.Open(dbPath, CFile::modeCreate | CFile::modeWrite);
        if (bSts) f.Close();
    }

#else
	ULONG ulStatus = 0;
    CString     szSqlCode;
	m_hr = CoInitialize(NULL);

    szSqlCode.Format(_T("CREATE DATABASE %s"),(LPCWSTR)m_strDbFileName);
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),ConnectionStringNoDB,adOpenKeyset,adLockOptimistic,adCmdText);

    if(FAILED(m_hr)) {
		// following log removed as it can generate lots of times under some circumstances while
		// providing no value whatsoever. Keep it here to indicate why it is not being used.
//		char  xBuff[128];
//		sprintf (xBuff, "CnPluTotalDb::CreateDbFile() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
//		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		ulStatus = HRESULT_CODE(m_hr);
		__pRecO->Close();

		CoUninitialize();

		switch(ulStatus)
		{
			case PLUTOTAL_DB_ERROR_MSDEDOWN:
				return PLUTOTAL_DB_ERROR_MSDEDOWN;
			default:
				return  PLUTOTAL_E_FAILURE;
		}
    }
	__pRecO->Close();

	CoUninitialize();
#endif
#endif
	return  PLUTOTAL_SUCCESS;
}

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::CreateDbBackUpFile(){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    CString     szSqlCode;

    m_hr = CoInitialize(NULL);

	szSqlCode.Format(PLUTOTAL_DB_BACKUP_BACKUP, (LPCTSTR)m_strDbFileName, (LPCTSTR)m_strDbFileName);

    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),ConnectionStringNoDB,adOpenKeyset,adLockOptimistic,adCmdText);

    if(FAILED(m_hr)) {
		char  xBuff[256];
		sprintf (xBuff, "CnPluTotalDb::CreateDbBackUpFile() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		__pRecO->Close();

		sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
		NHPOS_NONASSERT_TEXT(xBuff);
		int iLen = wcslen(__pRecO->m_errorMessage);
		sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
		NHPOS_NONASSERT_TEXT(xBuff);
		if (iLen > 110) {
			iLen -= 110;
			sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
		CoUninitialize();
		return  PLUTOTAL_E_FAILURE;
    }
	__pRecO->Close();

	CoUninitialize();
#endif
	return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::CreateDbBackUpFile() {

    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::CreateTable(const __CnTblFormat &TblFormat){
#if defined(CNPLUTOTALDB_LOG) && CNPLUTOTALDB_LOG
    {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::CreateTable() %S -> %S", (LPCWSTR)m_strDbFileName, (LPCWSTR)TblFormat.strName);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }
#endif
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    CString     strSqlCode;         // SQL statement
    strSqlCode.Format(_T("CREATE TABLE %s (%s)"), (LPCWSTR)TblFormat.strName, (LPCWSTR)TblFormat.strFldFormat);
	_bstr_t strConnect = ConnectionString;

    m_hr = __pRecO->OpenRec(CnVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
    __pRecO->Close();   // one shot!!!

	if(FAILED(m_hr)) {
		char  xBuff[256];
		sprintf (xBuff, "CnPluTotalDb::CreateTable() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
		NHPOS_NONASSERT_TEXT(xBuff);
		int iLen = wcslen(__pRecO->m_errorMessage);
		sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
		NHPOS_NONASSERT_TEXT(xBuff);
		if (iLen > 110) {
			iLen -= 110;
			sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
		return  PLUTOTAL_E_FAILURE;
	}

    // create index(s)
    if(!TblFormat.strIdxName.IsEmpty()){
        CString strIdxName;
        for(int i=0; i < CnPluTotalDb::s_IdxNum; i++){
            if(TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format(_T("%s_%02d"), (LPCTSTR)TblFormat.strIdxName,i);
            m_hr = CnPluTotalDb::CreateIndex(TblFormat.strName,strIdxName,TblFormat.strIdxFld[i]);
            if(FAILED(m_hr)){
                CnPluTotalDb::DropTable(TblFormat.strName);
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
#endif
    return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::CreateTable(const __CnTblFormat& TblFormat)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

#if defined(CNPLUTOTALDB_LOG) && CNPLUTOTALDB_LOG
    {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::CreateTable() %S -> %S", (LPCWSTR)m_strDbFileName, (LPCWSTR)TblFormat.strName);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }
#endif

    char sqlBuf[512];
    snprintf(sqlBuf, sizeof(sqlBuf), "CREATE TABLE IF NOT EXISTS %S (%S)", (LPCWSTR)TblFormat.strName, (LPCWSTR)TblFormat.strFldFormat);

    char* errMsg = nullptr;
    int rc = sqlite3_exec(cnRec0.pSqliteDb, sqlBuf, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return PLUTOTAL_E_FAILURE;
    }

    // create index(s)
    if (!TblFormat.strIdxName.IsEmpty()) {
        CString strIdxName;
        for (int i = 0; i < CnPluTotalDb::s_IdxNum; i++) {
            if (TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format(_T("%s_%02d"), (LPCTSTR)TblFormat.strIdxName, i);
            m_hr = CnPluTotalDb::CreateIndex(TblFormat.strName, strIdxName, TblFormat.strIdxFld[i]);
            if (FAILED(m_hr)) {
                CnPluTotalDb::DropTable(TblFormat.strName);
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }

    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::DropTable(LPCTSTR strTblName){
#if defined(CNPLUTOTALDB_LOG) && CNPLUTOTALDB_LOG
    {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::DropTable() %S -> %S", (LPCWSTR)m_strDbFileName, strTblName);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }
#endif
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    if (CheckTable(strTblName) == PLUTOTAL_SUCCESS) {
        CString     strSqlCode;

        strSqlCode.Format(_T("DROP TABLE %s "),strTblName);
	    _bstr_t strConnect = ConnectionString;
        m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
        __pRecO->Close();   // one shot!!!

        // m_hr failing with a value of 0x80040e21 probably means the table
        // does not exist so it can't be dropped.
	    if(FAILED(m_hr)) {
		    char  xBuff[256];
		    sprintf (xBuff, "CnPluTotalDb::DropTable() %S::%S FAILED: m_hr = 0x%8.8x", (LPCWSTR)m_strDbFileName, strTblName, m_hr);
		    NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		    return  PLUTOTAL_E_FAILURE;
	    }
    }
#endif

    return PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::DropTable(LPCTSTR strTblName) {
#if defined(CNPLUTOTALDB_LOG) && CNPLUTOTALDB_LOG
    {
        char xBuff[128];
        sprintf(xBuff, "==NOTE: CnPluTotalDb::DropTable() %S -> %S", (LPCWSTR)m_strDbFileName, strTblName);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
    }
#endif
    if (CheckTable(strTblName) == PLUTOTAL_SUCCESS) {
        char sqlBuf[256];
        snprintf(sqlBuf, sizeof(sqlBuf), "DROP TABLE %S ", strTblName);

        char* errMsg = nullptr;
        int rc = sqlite3_exec(cnRec0.pSqliteDb, sqlBuf, nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            char  xBuff[256];
            sprintf(xBuff, "CnPluTotalDb::DropTable() %S::%S FAILED: rc = 0x%8.8x", (LPCWSTR)m_strDbFileName, strTblName, rc);
            NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
            if (errMsg) sqlite3_free(errMsg);
            return PLUTOTAL_E_FAILURE;
        }
    }

    return PLUTOTAL_SUCCESS;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::CreateIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName,LPCTSTR lpszFldNames){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    CString strSqlCode;
	_bstr_t strConnect = ConnectionString;

    strSqlCode.Format(_T("CREATE INDEX %s ON %s ( %s ) "),lpszIdxName,lpszTblName,lpszFldNames);
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
    __pRecO->Close();   // one shot!!!
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::CreateIndex() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}
#endif

    return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::CreateIndex(LPCTSTR lpszTblName, LPCTSTR lpszIdxName, LPCTSTR lpszFldNames) {
    char sqlBuf[256];
    snprintf(sqlBuf, sizeof(sqlBuf), "CREATE INDEX %S ON %S ( %S ) ", lpszIdxName, lpszTblName, lpszFldNames);

    char* errMsg = nullptr;
    int rc = sqlite3_exec(cnRec0.pSqliteDb, sqlBuf, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        char  xBuff[128];
        sprintf(xBuff, "CnPluTotalDb::CreateIndex() FAILED: HRESULT rc = 0x%8.8x", rc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::DropIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    CString strSqlCode;
	_bstr_t strConnect = ConnectionString;

    strSqlCode.Format(_T("DROP INDEX %s.%s"),lpszTblName,lpszIdxName);
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
    __pRecO->Close();   // one shot!!!
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::DropIndex() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}
#endif

    return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::DropIndex(LPCTSTR lpszTblName, LPCTSTR lpszIdxName) {
    char sqlBuf[256];
    snprintf(sqlBuf, sizeof(sqlBuf), "DROP INDEX %S.%S", lpszTblName, lpszIdxName);

    char* errMsg = nullptr;
    int rc = sqlite3_exec(cnRec0.pSqliteDb, sqlBuf, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        char  xBuff[128];
        sprintf(xBuff, "CnPluTotalDb::DropIndex() FAILED: HRESULT rc = 0x%8.8x", rc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
}
#endif


ULONG   CnPluTotalDb::CreateIndexEx(const __CnTblFormat &TblFormat)
{
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    ULONG   ulSts;

    if(!TblFormat.strIdxName.IsEmpty()){
        CString strIdxName;
        for(int i=0; i < CnPluTotalDb::s_IdxNum; i++){
            if(TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format(_T("%s_%02d"), (LPCTSTR)TblFormat.strIdxName,i);
            ulSts = CnPluTotalDb::CreateIndex(TblFormat.strName,strIdxName,TblFormat.strIdxFld[i]);
            if(ulSts != PLUTOTAL_SUCCESS)
            {
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
#endif

    return PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::DropIndexEx(const __CnTblFormat &TblFormat)
{
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    ULONG   ulSts;
    if ( !TblFormat.strIdxName.IsEmpty())
    {
        CString strIdxName;
        for ( int i = 0; i < CnPluTotalDb::s_IdxNum; i++ )
        {
            if ( TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format( _T("%s_%02d"), (LPCTSTR)TblFormat.strIdxName, i );
            ulSts = CnPluTotalDb::DropIndex( TblFormat.strName, strIdxName );
            if(ulSts != PLUTOTAL_SUCCESS)
            {
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
#endif

    return  PLUTOTAL_SUCCESS;
}

// ### ADD (041500)
#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::CheckTable(LPCTSTR lpcTblName){
    ULONG   ulSts = PLUTOTAL_SUCCESS;
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
	_bstr_t strConnect = ConnectionString;

#if defined(USE_PLU_TOTALS_SQLITE)
    //
    // what this code does is to look up in the SQLite database management
    // data to ask if a specific table is in the list of tables and to provide
    // us the name for the table. if this query fails then the requested table
    // does not exist. if the query succeeds then fetch the SELECT result and
    // then clean up.

    CString   strSqlCode;         // SQL statement

    strSqlCode.Format(_T("SELECT name FROM sqlite_master WHERE type='table' AND name='%s'"), lpcTblName);

    m_hr = __pRecO->OpenRec(CnVariant(strSqlCode), strConnect, adOpenKeyset, adLockOptimistic, adCmdText);
    if (FAILED(m_hr)) {
        char  xBuff[128];
        sprintf(xBuff, "CnPluTotalDb::CheckTable() '%S'::'%S' OpenRec() FAILED: m_hr = 0x%8.8x", (LPCTSTR)m_strDbFileName, lpcTblName, m_hr);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        ulSts = PLUTOTAL_NOT_FOUND;
    }
    else {
        // retrieve the result rows for out SELECT. There should only
        // be one that contains the name of the table we searched for.

        COleVariant vStart;
        vStart.Clear();	/* 09/03/01 */

        COleSafeArray   saFields;
        long    lIdx[1] = { 0 };    // bound

        saFields.CreateOneDim(VT_VARIANT, 1, NULL, 0);
        lIdx[0] = 0;
        saFields.PutElement(lIdx, COleVariant(_T("name"), VT_BSTR));

        COleVariant vValues;
        m_hr = __pRecO->GetRows(1, vStart, saFields, vValues);
        vStart.Clear();	/* 09/03/01 */

        // m_hr will probably be equal to a value of 0x800a0bcd or adErrNoCurrentRecord
        // if the SELECT doesn't return any rows because the table does not exist.
        // 0x800a0bcd indicates "Either BOF or EOF is True, or the current record
        // has been deleted. Requested operation requires a current record."
        if (FAILED(m_hr)) {
            if (m_hr != 0x800a0bcd) {
                char  xBuff[128];
                sprintf(xBuff, "CnPluTotalDb::CheckTable() GetRows() FAILED: m_hr = 0x%8.8x", m_hr);
                NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
            }
            ulSts = PLUTOTAL_NOT_FOUND;
        }
    }

    __pRecO->Close();
#else
    m_hr = __pRecO->OpenRec(COleVariant(lpcTblName), strConnect,adOpenForwardOnly,adLockReadOnly,adCmdTable);

    if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::CheckTable() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        ulSts = PLUTOTAL_NOT_FOUND;
    }
    __pRecO->Close();
#endif
#endif

    return  ulSts;
}
#else
ULONG   CnPluTotalDb::CheckTable(LPCTSTR lpcTblName) {
    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::CheckTable() cnRec0 called.");

    ULONG   ulSts = PLUTOTAL_SUCCESS;

    //
    // what this code does is to look up in the SQLite database management
    // data to ask if a specific table is in the list of tables and to provide
    // us the name for the table. if this query fails then the requested table
    // does not exist. if the query succeeds then fetch the SELECT result and
    // then clean up.

    char   sqlBuf[256] = { 0 };         // SQL statement
    snprintf(sqlBuf, sizeof(sqlBuf), "SELECT name FROM sqlite_master WHERE type='table' AND name='%S'", lpcTblName);

    cnRec0.prepStmt = nullptr;
    int rc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (rc != SQLITE_OK) {
        char  xBuff[128];
        if (cnRec0.prepStmt) sqlite3_finalize(cnRec0.prepStmt);
        sprintf(xBuff, "CnPluTotalDb::CheckTable() '%S' OpenRec() FAILED: rc = %d", lpcTblName, rc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return PLUTOTAL_E_FAILURE;
    }

    rc = sqlite3_step(cnRec0.prepStmt);
    if (rc == SQLITE_ROW) {
        // table is the database file so we are good
        sqlite3_finalize(cnRec0.prepStmt);
        ulSts = PLUTOTAL_SUCCESS;
    }
    else {
        // either an error of some kind or the table is not in the database file.
        char  xBuff[128];
        sqlite3_finalize(cnRec0.prepStmt);
        sprintf(xBuff, "CnPluTotalDb::CheckTable() '%S' OpenRec() FAILED: m_hr = 0x%8.8x", lpcTblName, m_hr);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        ulSts = PLUTOTAL_NOT_FOUND;
    }

    return  ulSts;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::get_RecordCnt(LPCTSTR szTblName,LONG * plRecCnt){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    * plRecCnt = 1;   // always indicate 1
#else
    CnVariant   varTableName(szTblName);
    // open recoredset
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(varTableName, strConnect, adOpenKeyset,adLockOptimistic,adCmdTable);
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::get_RecordCnt() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        goto GETRECCNT_ERR;
	}

    // Insert
    m_hr = __pRecO->get_RecordCount(plRecCnt);
    if(FAILED(m_hr))
        goto GETRECCNT_ERR;

    // close recoredset
    __pRecO->Close();   // one shot!!!
    return PLUTOTAL_SUCCESS;

GETRECCNT_ERR:
    __pRecO->Close();
#endif
    return  PLUTOTAL_E_FAILURE;
}
#else
ULONG   CnPluTotalDb::get_RecordCnt(LPCTSTR szTblName, LONG* plRecCnt) {

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::get_RecordCnt() cnRec0 called.");

    ULONG   ulSts = PLUTOTAL_SUCCESS;

    if (!plRecCnt) return PLUTOTAL_E_ILLEAGAL;

    *plRecCnt = 0;

    // Prepare SQL statement to count records
    char sqlBuf[128];
    snprintf(sqlBuf, sizeof(sqlBuf), "SELECT COUNT(*) FROM %S;", szTblName);

    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_OK);
        return PLUTOTAL_E_FAILURE;
    }

    cnRec0.prepRc = sqlite3_step(cnRec0.prepStmt);
    if (cnRec0.prepRc == SQLITE_ROW) {
        *plRecCnt = (ULONG)sqlite3_column_int64(cnRec0.prepStmt, 0);
        ulSts = PLUTOTAL_SUCCESS;
    }
    else {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_ROW);
        return PLUTOTAL_E_FAILURE;
    }

    if (cnRec0.prepStmt) sqlite3_finalize(cnRec0.prepStmt);
	return ulSts;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::ExecSQL(LPCTSTR szSqlCode){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);
    __pRecO->Close();   // one shot!!!

	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::ExecSQL() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		{
			char xBuff[256];
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			int iLen = wcslen(__pRecO->m_errorMessage);
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (iLen > 110) {
				iLen -= 110;
				sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
		}
		return  PLUTOTAL_E_FAILURE;
	}
#endif

    return PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::ExecSQL(LPCTSTR szSqlCode) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::ExecSQL() cnRec0");

    return PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::RestoreDB(LPCTSTR szSQlCode){

	CString szSQL;
	USHORT usError = 0;
	USHORT usErrorCount = 0;

	CString activeConn;
	activeConn.Format(ConnectionStringTemp,L"master");
	_bstr_t strConnect = activeConn;

#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    //We set the database to be in single user mode while we do this database reset
	//so that no one else can access the database information while we reset the DB
	//SR 725 JHHJ
SINGLE_USER_DB:
	szSQL.Format(L"ALTER DATABASE %s SET SINGLE_USER WITH ROLLBACK IMMEDIATE", szSQlCode);
    m_hr = __pRecO->OpenRec(CnVariant(szSQL), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);

	//Close the recordset object so we can open a new one 
	//for the next SQL command
	__pRecO->Close();

	if (FAILED(m_hr))
	{
		char  xBuff[128];
		sprintf(xBuff, "CnPluTotalDb::RestoreDB() SET SINGLE_USER FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);
		usError = 1;
        goto MULTI_USER_DB;
	}

	//We will drop the database because the information in it is out of date
	//so that we can restore it in the next call.
	szSQL.Format(L"DROP DATABASE %s", szSQlCode);
	m_hr = __pRecO->OpenRec(CnVariant(szSQL), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);
	__pRecO->Close();

	if (FAILED(m_hr))
	{
		char  xBuff[128];
		sprintf(xBuff, "CnPluTotalDb::RestoreDB() DROP DATABASE FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);

		{
			char xBuff[256];
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			int iLen = wcslen(__pRecO->m_errorMessage);
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (iLen > 110) {
				iLen -= 110;
				sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
		}
	}


	//Restore the database from the file that the master/backupmaster has just sent over.
	szSQL.Format(PLUTOTAL_DB_BACKUP_RESTORE, szSQlCode, szSQlCode);
	m_hr = __pRecO->OpenRec(CnVariant(szSQL), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);
	__pRecO->Close();

	if (FAILED(m_hr))
	{
		char  xBuff[128];
		sprintf(xBuff, "CnPluTotalDb::RestoreDB() RESTORE DATABASE FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);

		{
			char xBuff[256];
			sprintf(xBuff, "%S", (LPCTSTR) __pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			int iLen = wcslen(__pRecO->m_errorMessage);
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (iLen > 110) {
				iLen -= 110;
				sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
		}
	}

MULTI_USER_DB:
	//Return the database back to multi-user mode. 
	szSQL.Format(L"ALTER DATABASE %s SET MULTI_USER", szSQlCode);
    m_hr = __pRecO->OpenRec(CnVariant(szSQL), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);

	__pRecO->Close();   // one shot!!!

	if (FAILED(m_hr))
	{
		char  xBuff[128];
		sprintf(xBuff, "CnPluTotalDb::RestoreDB() SET MULTI_USER FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);
		usErrorCount++;
		if (usErrorCount < 5) goto MULTI_USER_DB;
		{
			char xBuff[256];
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			int iLen = wcslen(__pRecO->m_errorMessage);
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (iLen > 110) {
				iLen -= 110;
				sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
		}
		return PLUTOTAL_E_FAILURE;
	} else if (usError)
	{
		usError = 0;
		goto SINGLE_USER_DB;
	}
#endif

    return PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::RestoreDB(LPCTSTR szSQlCode) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::RestoreDB() cnRec0");


    return PLUTOTAL_SUCCESS;
}
#endif

//================================================================

static ULONG GetValuesFromRec (sqlite3_stmt* prepStmt, const VARIANT & vGetFields, COleVariant & lpvValues)
{
    COleSafeArray   saRec = vGetFields;
    COleSafeArray   saValues;
    COleVariant     vWrk;
    long    lIdx[1] = { 0 };    // bound

    int column_count = sqlite3_column_count(prepStmt);

    saValues.CreateOneDim(VT_VARIANT, column_count, NULL, 0);
    for (int i = 0; i < column_count; i++) {
        const wchar_t* pText = nullptr;
        const wchar_t* p0 = (const wchar_t*)sqlite3_column_name16(prepStmt, i);
        int tp0 = sqlite3_column_type(prepStmt, i);
        long long   i64Value = 0;

        switch (tp0) {
        case SQLITE_INTEGER:
            i64Value = sqlite3_column_int64(prepStmt, i);
            vWrk.Clear(); vWrk = i64Value;
            saValues.PutElement(lIdx, &vWrk);
            lIdx[0]++;
            break;
        case SQLITE_TEXT:
            pText = (const wchar_t*)sqlite3_column_text16(prepStmt, i);
            vWrk.Clear(); vWrk.SetString(pText, VT_BSTR);
            saValues.PutElement(lIdx, &vWrk);
            lIdx[0]++;
            break;
        case SQLITE_NULL:
            vWrk.Clear(); vWrk = 0L;
            saValues.PutElement(lIdx, &vWrk);
            lIdx[0]++;
            break;
        default:
            break;
        }
    }
    lpvValues = CComVariant(saValues);
    return PLUTOTAL_SUCCESS;
}

// New Functions
#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::OpenRec(LPCTSTR lpcTableName){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(lpcTableName), strConnect,adOpenKeyset,adLockOptimistic,adCmdTable);
    if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::OpenRec() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        __pRecO->Close();
		{
			char xBuff[256];
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			int iLen = wcslen(__pRecO->m_errorMessage);
			sprintf(xBuff, "%S", (LPCTSTR)__pRecO->m_errorMessage);
			NHPOS_NONASSERT_TEXT(xBuff);
			if (iLen > 110) {
				iLen -= 110;
				sprintf(xBuff, "%S", ((LPCTSTR)__pRecO->m_errorMessage) + iLen);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
		}
		return  PLUTOTAL_E_FAILURE;
    }
#endif

    return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::OpenRec(LPCTSTR lpcTableName) {

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::OpenRec() table name called.");

    wcscpy(cnRec0.prepTableName, lpcTableName);
    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::OpenRec(LPCTSTR lpcTableName, COleSafeArray &saFields, COleSafeArray &saValues) {

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::OpenRec() table name, saFields, saValues called.");

    wcscpy(cnRec0.prepTableName, lpcTableName);
    cnRec0.prepTotalDbKeyFields = saFields;
    cnRec0.prepTotalDbKeyValues = saValues;
    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::OpenRec(LPCTSTR szSqlCode,const BOOL bClose,LONG * plRecCnt){
    *plRecCnt = 0;

    ULONG   ulStatus, ulSts = PLUTOTAL_SUCCESS;
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    *plRecCnt = 1;
#else
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), strConnect, adOpenKeyset,adLockOptimistic,adCmdText);
    if(FAILED(m_hr))
	{
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::OpenRec() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		ulStatus = HRESULT_CODE(m_hr);
			
		switch(ulStatus)
		{
			case PLUTOTAL_DB_ERROR_MSDEDOWN:
				return PLUTOTAL_DB_ERROR_MSDEDOWN;
			default:
				return  PLUTOTAL_E_FAILURE;
		}
	}
    m_hr = __pRecO->get_RecordCount(plRecCnt);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    if(*plRecCnt == 0L){
        ulSts = PLUTOTAL_NOT_FOUND;
    }
#endif
    return  ulSts;
}
#else
ULONG   CnPluTotalDb::OpenRec(LPCTSTR szSqlCode, const BOOL bClose, LONG* plRecCnt) {
    *plRecCnt = 0;

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::OpenRec() szSqlCode cnRec0 called.");

    char   sqlBuf[256] = { 0 };         // SQL statement
    snprintf(sqlBuf, sizeof(sqlBuf), "%S", szSqlCode);

    cnRec0.prepStmt = nullptr;
    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        char  xBuff[128];
        if (cnRec0.prepStmt) sqlite3_finalize(cnRec0.prepStmt);
        cnRec0.prepStmt = nullptr;
        sprintf(xBuff, "CnPluTotalDb::CheckTable() '%S' OpenRec() FAILED: rc = %d", (LPCTSTR)m_strDbFileName, cnRec0.prepRc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return PLUTOTAL_E_FAILURE;
    }

    cnRec0.iRecordCount = 0;
    do {
        cnRec0.prepRc = sqlite3_step(cnRec0.prepStmt);
        if (cnRec0.prepRc == SQLITE_ROW) cnRec0.iRecordCount += 1;
    } while (cnRec0.prepRc == SQLITE_ROW);

    *plRecCnt = cnRec0.iRecordCount;

    if (cnRec0.prepStmt) sqlite3_finalize(cnRec0.prepStmt);
    cnRec0.prepStmt = nullptr;

    ULONG   ulSts = PLUTOTAL_SUCCESS;

    if (*plRecCnt == 0L) {
        ulSts = PLUTOTAL_NOT_FOUND;
    }

    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_OK);
        return PLUTOTAL_E_FAILURE;
    }

    return  ulSts;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
VOID    CnPluTotalDb::CloseRec(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    if(__pRecO->IsOpened())
        __pRecO->Close();
#endif
}
#else
VOID    CnPluTotalDb::CloseRec(void) {
    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::CloseRec() cnRec0 called.");
    cnRec0.iRecordCount = 0;
    if (cnRec0.prepStmt) sqlite3_finalize(cnRec0.prepStmt);
    cnRec0.prepStmt = nullptr;
    cnRec0.prepRc = 0;
    cnRec0.prepTotalDbKeyFields.Clear();
    cnRec0.prepTotalDbKeyValues.Clear();
    cnRec0.prepTableName[0] = 0;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::GetRec(VARIANT vGetFields,LPVARIANT lpvValues){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;
    COleVariant vStart;

	vStart.Clear();	/* 09/03/01 */

    m_hr = __pRecO->GetRows(1,vStart,vGetFields,lpvValues);
	vStart.Clear();	/* 09/03/01 */
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::GetRec(VARIANT vGetFields, COleVariant & lpvValues) {
    ULONG  ulSts = PLUTOTAL_E_FAILURE;

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::GetRec() cnRec0 called.");

    if (cnRec0.pSqliteDb && cnRec0.prepStmt) {
        COleVariant vStart;

        vStart.Clear();	/* 09/03/01 */

        int rc = sqlite3_step(cnRec0.prepStmt);
        if (rc == SQLITE_ROW) {
            GetValuesFromRec(cnRec0.prepStmt, vGetFields, lpvValues);
            ulSts = PLUTOTAL_SUCCESS;
        }
    }

    return  ulSts;
}
#endif

// Add to the output strings wsFields and wsValues
// the field names and the values from the variants.
//
// We allow additional fields and values such as for
// a row key in order to use this function with both
// PutRec() and AddRec().
// PutRec() is not provided the key in the vPutFields variant
// where as AddRec() is.

static int FigureArrayCount(COleSafeArray& osaFields)
{
    if (osaFields.vt == VT_EMPTY) return -1;

    long lUbound = 0, lLbound = 0;

    osaFields.GetUBound(1, &lUbound);
    osaFields.GetLBound(1, &lLbound);
    
    return lUbound - lLbound + 1;
}


static int BuildInsertStmt (const wchar_t* wsTableName, VARIANT vPutFields, VARIANT vPutValues, std::wstring  &wsStmt)
{
    // building fields and values list part of an INSERT statement.
    // example: "INSERT INTO users (name, age) VALUES ('John Doe', 30);";
    COleSafeArray   saFields = vPutFields;
    COleSafeArray   saValues = vPutValues;
    COleVariant     vWrk;
    std::wstring    wsFields, wsValues;
    long    lIdx[1] = { 0 };    // bound

    int column_count = FigureArrayCount(saFields);

    NHPOS_ASSERT(column_count > 0);

    wsStmt = L"INSERT INTO ";
    wsStmt += wsTableName;
    wsStmt += L" ";
    wsFields.clear();
    wsValues.clear();

    for (int i = 0; i < column_count; i++) {
        long long   i64Value = 0;
        COleDateTime    dtDate;
        wchar_t       wsValueTemp[32] = { 0 };

        vWrk.Clear();
        saFields.GetElement(lIdx, &vWrk);
        wsFields += vWrk.bstrVal;
        wsFields += L", ";

        vWrk.Clear();
        saValues.GetElement(lIdx, &vWrk);
        lIdx[0]++;
        switch (vWrk.vt) {
        case VT_I1:
            swprintf(wsValueTemp, 30, L"%d", vWrk.bVal);
            wsValues += wsValueTemp;
            wsValues += L", ";
            break;
        case VT_I2:
            swprintf(wsValueTemp, 30, L"%d", vWrk.iVal);
            wsValues += wsValueTemp;
            wsValues += L", ";
            break;
        case VT_UI4:
            swprintf(wsValueTemp, 30, L"%u", vWrk.ulVal);
            wsValues += wsValueTemp;
            wsValues += L", ";
            break;
        case VT_I4:
            swprintf(wsValueTemp, 30, L"%d", vWrk.lVal);
            wsValues += wsValueTemp;
            wsValues += L", ";
            break;
        case VT_I8:
            swprintf(wsValueTemp, 30, L"%lld", vWrk.llVal);
            wsValues += wsValueTemp;
            wsValues += L", ";
            break;
        case VT_BSTR:
            wsValues += L"\'";
            wsValues += vWrk.bstrVal;
            wsValues += L"\'";
            wsValues += L", ";
            break;
        case VT_DATE:
            wsValues += L"\'";
            dtDate = vWrk;
            wsValues += dtDate.Format(_T("%Y-%m-%d %H:%M:%S"));
            wsValues += L"\'";
            wsValues += L", ";
            break;
        default:
            break;
        }
    }

    wsFields.erase(wsFields.length() - 2);
    wsValues.erase(wsValues.length() - 2);
    wsStmt += L"( " + wsFields + L") VALUES ( " + wsValues + L" );";

    return 0;
}

static int BuildUpdateStmt(const wchar_t *wsTableName, VARIANT vPutFields, std::wstring& wsStmt)
{
    // building fields and values list part of an UPDATE statement.
    // example: "UPDATE users SET name = ?, age = ? WHERE id = ?;";
    COleSafeArray   saFields = vPutFields;
    COleVariant     vWrk;
    std::wstring    wsFields;
    long    lIdx[1] = { 0 };    // bound

    int column_count = FigureArrayCount(saFields);

    NHPOS_ASSERT(column_count > 0);

    wsStmt = L"UPDATE ";
    wsStmt += wsTableName;
    wsStmt += L" SET ";

    for (int i = 0; i < column_count; i++) {
        long long   i64Value = 0;
        wchar_t       wsValueTemp[32] = { 0 };

        vWrk.Clear();
        saFields.GetElement(lIdx, &vWrk);
        wsFields += vWrk.bstrVal;
        wsFields += L" = ?, ";
        lIdx[0]++;
    }

    wsFields.erase(wsFields.length() - 2);
    wsStmt += wsFields ;

    return 0;
}

static int bindSafeArray(sqlite3_stmt* prepStmt, int iArgIndex, COleSafeArray& saValues)
{
    COleDateTime   dtDate;
    COleVariant    vWrk;
    long           lIdx[1] = { 0 };    // bound

    int column_count = FigureArrayCount(saValues);

    NHPOS_ASSERT(column_count > 0);

    for (int i = iArgIndex + 1; i <= iArgIndex + column_count; i++) {
        long long   i64Value = 0;
        int         iLen;
        int         rc = 0;
        char        sValueTemp[64] = { 0 };

        vWrk.Clear();
        saValues.GetElement(lIdx, &vWrk);
        lIdx[0]++;
        switch (vWrk.vt) {
        case VT_I1:
            rc = sqlite3_bind_int(prepStmt, i, vWrk.iVal);
            break;
        case VT_I2:
            rc = sqlite3_bind_int(prepStmt, i, vWrk.iVal);
            break;
        case VT_UI4:
            rc = sqlite3_bind_int(prepStmt, i, vWrk.iVal);
            break;
        case VT_I4:
            rc = sqlite3_bind_int(prepStmt, i, vWrk.iVal);
            break;
        case VT_I8:
            rc = sqlite3_bind_int64(prepStmt, i, vWrk.llVal);
            break;
        case VT_BSTR:
            {
                 _bstr_t b = vWrk.bstrVal;
                unsigned int bl = b.length();
                char* bc = _com_util::ConvertBSTRToString(b);
                rc = sqlite3_bind_text(prepStmt, i, bc, -1, SQLITE_TRANSIENT);
                delete[] bc;
           }
            break;
        case VT_DATE:
            // convert from the internal date format to the string style date format
            // we use in our SQLite database.
            dtDate = vWrk;
            iLen = snprintf(sValueTemp, sizeof(sValueTemp), "%S", (LPCWSTR)dtDate.Format(_T("%Y-%m-%d %H:%M:%S")));
            rc = sqlite3_bind_text(prepStmt, i, sValueTemp, -1, SQLITE_TRANSIENT);
            break;
        default:
            break;
        }
    }

    return iArgIndex + column_count;
}

int bindPrepStmt(sqlite3_stmt* prepStmt, VARIANT &vPutValues, COleSafeArray &prepDbKeyValues)
{
    // Bind parameters from RecData
    COleSafeArray   saValues = vPutValues;

    int iArgIndex = 0;

    iArgIndex = bindSafeArray(prepStmt, iArgIndex, saValues);   // values for the fields

    iArgIndex = bindSafeArray(prepStmt, iArgIndex, prepDbKeyValues);   // values for the key

    return iArgIndex;
}

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::PutRec(VARIANT vPutFields,VARIANT vValues){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecO->Update(vPutFields,vValues);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::PutRec(VARIANT vPutFields, VARIANT vPutValues) {
    ULONG ulSts = PLUTOTAL_E_FAILURE;

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::PutRec() cnRec0 called.");
    if (cnRec0.prepTableName[0] == 0)
        return  PLUTOTAL_E_FAILURE;

    std::wstring  wsStmt;

    BuildUpdateStmt(cnRec0.prepTableName, vPutFields, wsStmt);

    wsStmt += L" WHERE ";
    int column_count = FigureArrayCount(cnRec0.prepTotalDbKeyFields);
    long    lIdx[1] = { 0 };    // bound
    for (int i = 0; i < column_count; i++) {
        long long   i64Value = 0;
        COleVariant     vWrk;

        vWrk.Clear();
        cnRec0.prepTotalDbKeyFields.GetElement(lIdx, &vWrk);
        wsStmt += vWrk.bstrVal;
        wsStmt += L" = ? AND ";
        lIdx[0]++;
    }
    wsStmt.erase(wsStmt.length() - 4);
    wsStmt += L";";

    // Prepare SQL statement for all fields
    char sqlBuf[512] = { 0 };
    int iLen = snprintf(sqlBuf, sizeof(sqlBuf), "%S", wsStmt.c_str());

    cnRec0.prepStmt = nullptr;
    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_OK);
        return PLUTOTAL_E_FAILURE;
    }

    // Bind parameters from RecData
    bindPrepStmt(cnRec0.prepStmt, vPutValues, cnRec0.prepTotalDbKeyValues);

    cnRec0.prepRc = sqlite3_step(cnRec0.prepStmt);
    if (cnRec0.prepRc != SQLITE_DONE) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_DONE);
        return PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::AddRec(VARIANT vPutFields,VARIANT vValues){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecO->AddNew(COleVariant(vPutFields),COleVariant(vValues));
    if(FAILED(m_hr)){
        return  PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::AddRec(VARIANT vPutFields, VARIANT vPutValues) {
    ULONG ulSts = PLUTOTAL_E_FAILURE;

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::AddRec() cnRec0 called.");

    if (cnRec0.prepTableName[0] == 0)
        return  PLUTOTAL_E_FAILURE;

    std::wstring  wsStmt;

    BuildInsertStmt(cnRec0.prepTableName, vPutFields, vPutValues, wsStmt);

    // Prepare SQL statement for all fields
    char sqlBuf[512] = { 0 };
    snprintf(sqlBuf, sizeof(sqlBuf), "%S", wsStmt.c_str());

    cnRec0.prepStmt = nullptr;
    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_OK);
        return PLUTOTAL_E_FAILURE;
    }

    cnRec0.prepRc = sqlite3_step(cnRec0.prepStmt);
    if (cnRec0.prepRc != SQLITE_DONE) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_DONE);
        return PLUTOTAL_E_FAILURE;
    }
    
    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::DelRec(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecO->Delete();
    if(FAILED(m_hr)){
        return  PLUTOTAL_E_FAILURE;
    }
    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::DelRec(void) {
    ULONG ulSts = PLUTOTAL_E_FAILURE;

    NHPOS_NONASSERT_TEXT("##CnPluTotalDb::DelRec() cnRec0 called.");
    if (cnRec0.prepTableName[0] == 0)
        return  PLUTOTAL_E_FAILURE;

    std::wstring  wsStmt;

    wsStmt = L"DELETE FROM ";
    wsStmt += cnRec0.prepTableName;
    wsStmt += L" WHERE ";

    int column_count = FigureArrayCount(cnRec0.prepTotalDbKeyFields);
    std::wstring    wsKey;
    long    lIdx[1] = { 0 };    // bound
    for (int i = 0; i < column_count; i++) {
        long long   i64Value = 0;
        COleVariant     vWrk;

        vWrk.Clear();
        cnRec0.prepTotalDbKeyFields.GetElement(lIdx, &vWrk);
        wsStmt += vWrk.bstrVal;
        wsKey += L" = ?, ";
        lIdx[0]++;
    }
    wsKey.erase(wsKey.length() - 2);
    wsKey += L";";

    // Prepare SQL statement for all fields
    char sqlBuf[512] = { 0 };
    int iLen = snprintf(sqlBuf, sizeof(sqlBuf), "%S", wsStmt.c_str());

    cnRec0.prepStmt = nullptr;
    cnRec0.prepRc = sqlite3_prepare_v2(cnRec0.pSqliteDb, sqlBuf, -1, &cnRec0.prepStmt, nullptr);
    if (cnRec0.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_OK);
        return PLUTOTAL_E_FAILURE;
    }

    // Bind parameters from RecData
    int iArgIndex = bindSafeArray(cnRec0.prepStmt, 0, cnRec0.prepTotalDbKeyValues);   // values for the key

    cnRec0.prepRc = sqlite3_step(cnRec0.prepStmt);
    if (cnRec0.prepRc != SQLITE_DONE) {
        NHPOS_ASSERT(cnRec0.prepRc == SQLITE_DONE);
        return PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
}
#endif

// *** DEBUG & TEST ************************************



// *** record set series
#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::OpenRecoredset(const LPCTSTR szSqlCode,CursorTypeEnum CursorType,LockTypeEnum LockType,long lOption){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(__pRecS->IsOpened() == TRUE)
        return  PLUTOTAL_E_OPENED;

    CnVariant   vSqlCode(szSqlCode);
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecS->OpenRec(vSqlCode, strConnect,CursorType,LockType,lOption);
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::OpenRecoredset() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}

    __pRecS->MoveFirst();

    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::OpenRecoredset(const LPCTSTR szSqlCode, CursorTypeEnum CursorType, LockTypeEnum LockType, long lOption) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::OpenRecordset() cnRecs");
    ULONG   ulSts = PLUTOTAL_SUCCESS;

    char   sqlBuf[256] = { 0 };         // SQL statement
    snprintf(sqlBuf, sizeof(sqlBuf), "%S", szSqlCode);
    NHPOS_NONASSERT_NOTE("==NOTE", sqlBuf);

    cnRecS.prepStmt = nullptr;
    cnRecS.prepRc = sqlite3_prepare_v2(cnRecS.pSqliteDb, sqlBuf, -1, &cnRecS.prepStmt, nullptr);
    if (cnRecS.prepRc != SQLITE_OK) {
        char  xBuff[128];
        if (cnRecS.prepStmt) sqlite3_finalize(cnRecS.prepStmt);
        cnRecS.prepStmt = nullptr;
        sprintf(xBuff, "CnPluTotalDb::OpenRecoredset() '%S' sqlite3_prepare_v2() FAILED: rc = %d", (LPCTSTR)m_strDbFileName, cnRecS.prepRc);
        NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return PLUTOTAL_E_FAILURE;
    }

    cnRecS.iRecordCount = 0;
    do {
        cnRecS.prepRc = sqlite3_step(cnRecS.prepStmt);
        if (cnRecS.prepRc == SQLITE_ROW) cnRecS.iRecordCount += 1;
    } while (cnRecS.prepRc == SQLITE_ROW);

    if (cnRecS.prepStmt) sqlite3_finalize(cnRecS.prepStmt);
    cnRecS.prepStmt = nullptr;

    cnRecS.prepRc = sqlite3_prepare_v2(cnRecS.pSqliteDb, sqlBuf, -1, &cnRecS.prepStmt, nullptr);
    if (cnRecS.prepRc != SQLITE_OK) {
        NHPOS_ASSERT(cnRecS.prepRc == SQLITE_OK);
    }

    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::CloseRecoredset(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    __pRecS->Close();
#endif
    return  PLUTOTAL_SUCCESS;
}
#else
ULONG   CnPluTotalDb::CloseRecoredset(void) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::CloseRecordset() cnRecs");
    if (cnRecS.prepStmt) sqlite3_finalize(cnRecS.prepStmt);
    cnRecS.prepStmt = nullptr;
    return  PLUTOTAL_SUCCESS;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::MoveFirst(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MoveFirst();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::MoveFirst(void) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::MoveFirst() cnRecs");
    return  PLUTOTAL_SUCCESS;
}
#endif

#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::MoveNext(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MoveNext();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::MoveNext(void) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::MoveNext() cnRecs");
    cnRecS.prepRc = sqlite3_step(cnRecS.prepStmt);
    switch (cnRecS.prepRc) {
    case SQLITE_ROW:
        break;
    default:
        NHPOS_ASSERT(cnRecS.prepRc == SQLITE_ROW);
        return PLUTOTAL_E_FAILURE;
        break;
    }

    return  PLUTOTAL_SUCCESS;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::GetRow(const CnVariant vFldNames,CnVariant * pvValues){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecS->GetRow(vFldNames,pvValues);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    VARIANT_BOOL    vbEof;
    m_hr = __pRecS->get_EOF(&vbEof);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    if(vbEof)
        return  PLUTOTAL_EOF;
    else
        return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::GetRow(const CnVariant vFldNames, CnVariant* pvValues) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::GetRow() cnRecs");
    if (cnRecS.prepStmt) {
        cnRecS.prepRc = sqlite3_step(cnRecS.prepStmt);
        switch (cnRecS.prepRc) {
        case SQLITE_ROW:
            GetValuesFromRec(cnRecS.prepStmt, vFldNames, *pvValues);
            return  PLUTOTAL_SUCCESS;
            break;

        case SQLITE_DONE:
            break;

        default:
            NHPOS_ASSERT(cnRecS.prepRc == SQLITE_ROW);
            break;
        }
    }

    return  PLUTOTAL_EOF;
}
#endif


#if !defined(ENABLE_EMBED_SQLITE_DB)
ULONG   CnPluTotalDb::get_RecordCnt(LONG * plRecCnt){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
    *plRecCnt = 1;
    return  PLUTOTAL_SQLITE_TEST;
#else
    if(__pRecS->IsOpened() == FALSE){
        *plRecCnt = 0L;
        return  PLUTOTAL_E_FAILURE;
    }
    m_hr = __pRecS->get_RecordCount(plRecCnt);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    else
        return  PLUTOTAL_SUCCESS;
#endif
}
#else
ULONG   CnPluTotalDb::get_RecordCnt(LONG* plRecCnt) {
    NHPOS_NONASSERT_TEXT("## CnPluTotalDb::get_RecordCnt() cnRecs");
    *plRecCnt = cnRecS.iRecordCount;
    return  PLUTOTAL_SUCCESS;
}
#endif
