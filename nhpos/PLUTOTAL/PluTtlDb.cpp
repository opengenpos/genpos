// PluTtlDb.cpp: implementation of the CnPluTotalDb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluTtlDb.h"

#include "NDbgTrc.h"    // TRACER object


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int         CnPluTotalDb::s_nObjCnt;                // object counter

// following define is used to enable ASSRTLOG file logs.
#define CNPLUTOTALDB_LOG  0

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

const int   CnPluTotalDb::s_IdxNum         = 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnPluTotalDb::CnPluTotalDb(const LPCTSTR szDbFileName){
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
        sprintf(xBuff, "CnPluTotalDb::CheckTable() '%S'::'%S' OpenRec() FAILED: m_hr = 0x%8.8x", m_strDbFileName, lpcTblName, m_hr);
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



// New Functions
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


VOID    CnPluTotalDb::CloseRec(void){
#if defined(SQLITE_TEST) && SQLITE_TEST
    m_hr = 0;    // pretned that the command worked.
#else
    if(__pRecO->IsOpened())
        __pRecO->Close();
#endif
}


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

// *** DEBUG & TEST ************************************



// *** record set series
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

