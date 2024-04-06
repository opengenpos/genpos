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
//CnAdoXPRec* CnPluTotalDb::s_pRec[RECORDSET_NUM];    // record sets
CnAdoXPRec* CnPluTotalDb::__pRecS;	// recordset (ADOCE)
CnAdoXPRec* CnPluTotalDb::__pRecO;	// recordset (ADOCE)
const int   CnPluTotalDb::s_IdxNum         = 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnPluTotalDb::CnPluTotalDb(const LPCTSTR szDbFileName){
#ifdef XP_PORT
#if defined(USE_PLU_TOTALS_SQLSERVER)
	// We are using shared memory protocol for our connection string.  Change made for
	// Amtrak because the tablet with its wireless cell card was causing GenPOS to be unable
	// to connect to the MSDE or SQL Server instance under some circumstances.
	ConnectionStringTemp = L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=lpc:(local);DATABASE=%s;UID=; Password=;";
	ConnectionStringNoDB = L"Provider=MSDASQL;DRIVER={SQL Server};SERVER=lpc:(local);DATABASE=;UID=; Password=;";
#elif defined(USE_PLU_TOTALS_SQLITE)
	ConnectionStringTemp = L"DRIVER=SQLite3 ODBC Driver;Database=\\FlashDisk\\NCR\\Saratoga\\Database\\%s.db;LongNames=0;Timeout=1000;NoTXN=0;SyncPragma=NORMAL;StepAPI=0;";
	ConnectionStringNoDB = L"DRIVER=SQLite3 ODBC Driver;LongNames=0;Timeout=1000;NoTXN=0;SyncPragma=NORMAL;StepAPI=0;";
#elif defined(USE_PLU_TOTALS_LOCALDB)
    ConnectionStringTemp = L"Server=(localdb)\\MSSQLLocalDB;Integrated Security=true;DataTypeCompatibility=80;AttachDbFileName=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.mdf;";
    ConnectionStringNoDB = L"Server=(localdb)\\MSSQLLocalDB;Integrated Security=true;DataTypeCompatibility=80;AttachDbFileName=C:\\FlashDisk\\NCR\\Saratoga\\Database\\TOTALPLU.mdf";
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
#endif
    CreateObject(szDbFileName);
}


CnPluTotalDb::~CnPluTotalDb(){
    DestroyObject();
}


void    CnPluTotalDb::CreateObject(LPCTSTR szDbFileName){

    m_strDbFileName = szDbFileName;
#ifdef XP_PORT
    ConnectionString.Format(ConnectionStringTemp,m_strDbFileName);
#endif
#ifdef  _WIN32_WCE_EMULATION
    m_vAC_DbFileName.SetString(_T(""),VT_BSTR);
#else
    m_vAC_DbFileName = m_strDbFileName;
#endif

    if(__pRecS == NULL)
        __pRecS = new CnAdoXPRec;
    if(__pRecO == NULL)
        __pRecO = new CnAdoXPRec;

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
	ULONG ulStatus;
#ifndef _WIN32_WCE_EMULATION
    CString     szSqlCode;
#ifdef XP_PORT
	CoInitialize(NULL);
#endif
    szSqlCode.Format(_T("CREATE DATABASE %s"),m_strDbFileName);
#ifdef XP_PORT
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),ConnectionStringNoDB,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),CnVariant(_T("")),adOpenKeyset,adLockOptimistic,adCmdText);
#endif

//    __pRecO->Close();
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
#endif
	__pRecO->Close();

	CoUninitialize();
	return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::CreateDbBackUpFile(){
#ifndef _WIN32_WCE_EMULATION
    CString     szSqlCode;

	CoInitialize(NULL);

	szSqlCode.Format(PLUTOTAL_DB_BACKUP_BACKUP, (LPCTSTR)m_strDbFileName, (LPCTSTR)m_strDbFileName);

	m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),ConnectionStringNoDB,adOpenKeyset,adLockOptimistic,adCmdText);

//    __pRecO->Close();
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
#endif
	__pRecO->Close();

	CoUninitialize();
	return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::DropDbFile(){
#ifndef _WIN32_WCE_EMULATION
    CString     szSqlCode;
    szSqlCode.Format(_T("DROP DATABASE %s"),m_strDbFileName);
#ifdef XP_PORT
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),ConnectionStringNoDB,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode),CnVariant(_T("")),adOpenKeyset,adLockOptimistic,adCmdText);
#endif
    __pRecO->Close();
    if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::DropDbFile() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
    }
#endif
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::CreateTable(const __CnTblFormat &TblFormat){

    CString     strSqlCode;         // SQL statement
    strSqlCode.Format(_T("CREATE TABLE %s (%s)"),TblFormat.strName,TblFormat.strFldFormat);
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
            strIdxName.Format(_T("%s_%02d"),TblFormat.strIdxName,i);
            m_hr = CnPluTotalDb::CreateIndex(TblFormat.strName,strIdxName,TblFormat.strIdxFld[i]);
            if(FAILED(m_hr)){
                CnPluTotalDb::DropTable(TblFormat.strName);
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::DropTable(LPCTSTR strTblName){
    CString     strSqlCode;

    strSqlCode.Format(_T("DROP TABLE %s "),strTblName);
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
    __pRecO->Close();   // one shot!!!

	if(FAILED(m_hr)) {
		char  xBuff[256];
		sprintf (xBuff, "CnPluTotalDb::DropTable() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
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

    return PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::CreateIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName,LPCTSTR lpszFldNames){
    CString strSqlCode;
    strSqlCode.Format(_T("CREATE INDEX %s ON %s ( %s ) "),lpszIdxName,lpszTblName,lpszFldNames);
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdText);
#endif
    __pRecO->Close();   // one shot!!!
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::CreateIndex() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}

    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::DropIndex(LPCTSTR lpszTblName,LPCTSTR lpszIdxName){
    CString strSqlCode;
    strSqlCode.Format(_T("DROP INDEX %s.%s"),lpszTblName,lpszIdxName);
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(COleVariant(strSqlCode),m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdText);
#endif
    __pRecO->Close();   // one shot!!!
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::DropIndex() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}

    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::CreateIndexEx(const __CnTblFormat &TblFormat)
{
    ULONG   ulSts;

    if(!TblFormat.strIdxName.IsEmpty()){
        CString strIdxName;
        for(int i=0; i < CnPluTotalDb::s_IdxNum; i++){
            if(TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format(_T("%s_%02d"),TblFormat.strIdxName,i);
            ulSts = CnPluTotalDb::CreateIndex(TblFormat.strName,strIdxName,TblFormat.strIdxFld[i]);
            if(ulSts != PLUTOTAL_SUCCESS)
            {
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
    return PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::DropIndexEx(const __CnTblFormat &TblFormat)
{
    ULONG   ulSts;
    if ( !TblFormat.strIdxName.IsEmpty())
    {
        CString strIdxName;
        for ( int i = 0; i < CnPluTotalDb::s_IdxNum; i++ )
        {
            if ( TblFormat.strIdxFld[i].IsEmpty())
                break;
            strIdxName.Format( _T("%s_%02d"), TblFormat.strIdxName, i );
            ulSts = CnPluTotalDb::DropIndex( TblFormat.strName, strIdxName );
            if(ulSts != PLUTOTAL_SUCCESS)
            {
                return  PLUTOTAL_E_FAILURE;
            }
        }
    }
    return  PLUTOTAL_SUCCESS;
}

// ### ADD (041500)
ULONG   CnPluTotalDb::CheckTable(LPCTSTR lpcTblName){
    ULONG   ulSts = PLUTOTAL_SUCCESS;
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(COleVariant(lpcTblName), strConnect,adOpenForwardOnly,adLockReadOnly,adCmdTable);
#else //WINCE
    m_hr = __pRecO->OpenRec(COleVariant(lpcTblName),m_vAC_DbFileName,adOpenForwardOnly,adLockReadOnly,adCmdTable);
#endif

    if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::CheckTable() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        ulSts = PLUTOTAL_NOT_FOUND;
    }
    __pRecO->Close();
    return  ulSts;
}


ULONG   CnPluTotalDb::get_RecordCnt(LPCTSTR szTblName,LONG * plRecCnt){
    CnVariant   varTableName(szTblName);
#ifdef XP_PORT
    // open recoredset
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(varTableName, strConnect, adOpenKeyset,adLockOptimistic,adCmdTable);
#else //WINCE
    m_hr = __pRecO->OpenRec(varTableName, m_vAC_DbFileName, adOpenKeyset,adLockOptimistic,adCmdTable);
#endif
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
    return  PLUTOTAL_E_FAILURE;
}


ULONG   CnPluTotalDb::ExecSQL(LPCTSTR szSqlCode){
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), strConnect, adOpenForwardOnly,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), m_vAC_DbFileName, adOpenForwardOnly,adLockOptimistic,adCmdText);
#endif	
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

    return PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::RestoreDB(LPCTSTR szSQlCode){

	CString szSQL;
	USHORT usError = 0;
	USHORT usErrorCount = 0;

	CString activeConn;
	activeConn.Format(ConnectionStringTemp,L"master");
	_bstr_t strConnect = activeConn;

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

    return PLUTOTAL_SUCCESS;
}



// New Functions
ULONG   CnPluTotalDb::OpenRec(LPCTSTR lpcTableName){
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(lpcTableName), strConnect,adOpenKeyset,adLockOptimistic,adCmdTable);
#else //WINCE
    m_hr = __pRecO->OpenRec(CnVariant(lpcTableName), m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdTable);
#endif
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
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::OpenRec(LPCTSTR szSqlCode,const BOOL bClose,LONG * plRecCnt){
    *plRecCnt = 0;

    ULONG   ulStatus, ulSts = PLUTOTAL_SUCCESS;
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), strConnect, adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(CnVariant(szSqlCode), m_vAC_DbFileName, adOpenForwardOnly,adLockOptimistic,adCmdText);
#endif
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
    return  ulSts;
}


VOID    CnPluTotalDb::CloseRec(void){
    if(__pRecO->IsOpened())
        __pRecO->Close();
}


ULONG   CnPluTotalDb::GetRec(VARIANT vGetFields,LPVARIANT lpvValues){
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;
    COleVariant vStart;

	vStart.Clear();	/* 09/03/01 */

    m_hr = __pRecO->GetRows(1,vStart,vGetFields,lpvValues);
	vStart.Clear();	/* 09/03/01 */
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::PutRec(VARIANT vPutFields,VARIANT vValues){
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecO->Update(vPutFields,vValues);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::AddRec(VARIANT vPutFields,VARIANT vValues){
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;

    m_hr = __pRecO->AddNew(COleVariant(vPutFields),COleVariant(vValues));
    if(FAILED(m_hr)){
        return  PLUTOTAL_E_FAILURE;
    }

    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::DelRec(const LONG lRecNum){
    if(!__pRecO->IsOpened())
        return  PLUTOTAL_E_FAILURE;
    if(lRecNum < 1)
        return  PLUTOTAL_E_ILLEAGAL;

    m_hr = __pRecO->Delete(lRecNum);
    if(FAILED(m_hr)){
        return  PLUTOTAL_E_FAILURE;
    }
    return  PLUTOTAL_SUCCESS;
}

// *** DEBUG & TEST ************************************



// *** record set series
ULONG   CnPluTotalDb::OpenRecoredset(const LPCTSTR szSqlCode,CursorTypeEnum CursorType,LockTypeEnum LockType,long lOption){
    if(__pRecS->IsOpened() == TRUE)
        return  PLUTOTAL_E_OPENED;

    CnVariant   vSqlCode(szSqlCode);
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecS->OpenRec(vSqlCode, strConnect,CursorType,LockType,lOption);
#else //WINCE
    m_hr = __pRecS->OpenRec(vSqlCode,m_vAC_DbFileName,CursorType,LockType,lOption);
#endif
	if(FAILED(m_hr)) {
		char  xBuff[128];
		sprintf (xBuff, "CnPluTotalDb::OpenRecoredset() FAILED: HRESULT m_hr = 0x%8.8x", m_hr);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        return  PLUTOTAL_E_FAILURE;
	}

    __pRecS->MoveFirst();

    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::CloseRecoredset(void){
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    __pRecS->Close();
    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::MoveFirst(void){
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MoveFirst();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::MoveLast(void){
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MoveLast();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::MoveNext(void){
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MoveNext();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::MovePrev(void){
    if(__pRecS->IsOpened() == FALSE)
        return  PLUTOTAL_E_FAILURE;
    m_hr = __pRecS->MovePrevious();
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::GetRow(const CnVariant vFldNames,CnVariant * pvValues){
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
}


ULONG   CnPluTotalDb::get_RecordCnt(LONG * plRecCnt){
    if(__pRecS->IsOpened() == FALSE){
        *plRecCnt = 0L;
        return  PLUTOTAL_E_FAILURE;
    }
    m_hr = __pRecS->get_RecordCount(plRecCnt);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    else
        return  PLUTOTAL_SUCCESS;
}


// *** OLD FUNCTNIONS **********************************
#ifdef  ___GOMI
#ifdef  __DEL_061600

const ULONG CnPluTotalDb::s_NoClose        = 0x01;
const ULONG CnPluTotalDb::s_NoGet          = 0x02;

BOOL    CnPluTotalDb::MakeSQLWhere(const CnVariant &vFields,const CnVariant &vValues,CString &strWhere){
    COleSafeArray   saFields;
    COleSafeArray   saValues;

    if(vFields.vt & VT_ARRAY){
        saFields = vFields;
        saValues = vValues;
    }
    else{   // not Array
        saFields.CreateOneDim(VT_VARIANT,1,(VOID *)&vFields);
        saValues.CreateOneDim(VT_VARIANT,1,(VOID *)&vValues);
    }

    strWhere.Empty();   // clear

    LONG    lLBnd,lUBnd;    // get Bounds
    saFields.GetLBound(1,&lLBnd);
    saFields.GetUBound(1,&lUBnd);

    BOOL    bSts = TRUE;
    CnVariant   vFld,vVal;
    CString     strWrk,strWrk2;
    LONG    lIdx[1];
    for(int cnt=0;cnt <= (lUBnd - lLBnd);cnt++){
        lIdx[0] = cnt;

        saFields.GetElement(lIdx,(LPVOID)&vFld);
        saValues.GetElement(lIdx,(LPVOID)&vVal);

        strWrk.Empty();
        switch(vVal.vt){
            case VT_BSTR:
                strWrk.Format(_T(" %s = '%s' and"), vFld.bstrVal,vVal.bstrVal);
                break;
            case VT_UI2:
            case VT_I2:
                strWrk.Format(_T(" %s = %d and"), V_BSTR(&vFld),V_UI1(&vVal));
                break;
            case VT_UI4 :
            case VT_I4 :
                strWrk.Format(_T(" %s = %ld and"), vFld.bstrVal,vVal.lVal);
                break;
            default:
                bSts = FALSE;
        }
        strWrk2 += strWrk;
    }
    // delete "and"
    int len = strWrk2.GetLength();
    if(len > 3 && bSts == TRUE)
        strWhere = strWrk2.Left(len - 3);

    return bSts;
}


extern   CnDbgTrace         dbgtrc;
ULONG   CnPluTotalDb::Insert(const LPCTSTR szTblName,const CnVariant &varFldOrds,const CnVariant &varFldData){

    // open recoredset
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(CnVariant(szTblName), strConnect,adOpenKeyset,adLockOptimistic,adCmdTable);
#else
    m_hr = __pRecO->OpenRec(CnVariant(szTblName), m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdTable);
#endif
    if(FAILED(m_hr)){
        __pRecO->Close();
        return  PLUTOTAL_E_FAILURE;
    }
dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Insert --- AddNew"),TEXT(" (1)"));

    // Insert
    m_hr = __pRecO->AddNew(varFldOrds,varFldData);
    if(FAILED(m_hr)){
        __pRecO->Close();
        return  PLUTOTAL_E_FAILURE;
    }

    // close recoredset
    __pRecO->Close();   // one shot!!!
    return PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::Delete(const LPCTSTR szTblName,const CnVariant &vFields,const CnVariant &vValues){
    CString     strSqlCode;
    CString     strWhere;

    MakeSQLWhere(vFields,vValues,strWhere);
    strSqlCode.Format(_T("SELECT * FROM %s WHERE %s "), szTblName, (LPCTSTR)strWhere);
    CnVariant   vSqlCode(strSqlCode);
#ifdef XP_PORT
    _bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(vSqlCode, strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(vSqlCode, m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdText);
#endif
	if(FAILED(m_hr))
        goto DEL_ERR;

    long    lRecNum;
    __pRecO->get_RecordCount(&lRecNum);
    if(lRecNum != 1){
        __pRecO->Close();
        return  PLUTOTAL_E_FAILURE;
    }

    m_hr = __pRecO->Delete(1L);
    if(FAILED(m_hr))
        goto DEL_ERR;

    __pRecO->Close();
    return  PLUTOTAL_SUCCESS;
DEL_ERR:
    __pRecO->Close();
    return  PLUTOTAL_E_FAILURE;
}


ULONG   CnPluTotalDb::Update(const LPCTSTR lpszTblName,const CnVariant &varFldOrds,const CnVariant &varFldData){
    CnVariant   varTableName(lpszTblName);


    // open recoredset
    if(!(__pRecO->IsOpened()) && (lpszTblName != NULL)){
dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Update --- OpenRec"),TEXT(" (2)"));
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(varTableName, strConnect,adOpenKeyset,adLockOptimistic,adCmdTable);
#else
    m_hr = __pRecO->OpenRec(varTableName, m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdTable);
#endif
        if(FAILED(m_hr))
            goto UPDATE_ERR;
    }

    // update
dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Update --- Update"),TEXT(" (2)"));
    m_hr = __pRecO->Update(varFldOrds,varFldData);
    if(FAILED(m_hr))
        goto UPDATE_ERR;
dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Update --- CloseRec"),TEXT(" (2)"));

    // close recoredset
    __pRecO->Close();   // one shot!!!
    return PLUTOTAL_SUCCESS;

UPDATE_ERR:
    __pRecO->Close();
    return  PLUTOTAL_E_FAILURE;
}


// search & update
ULONG   CnPluTotalDb::Update(const LPCTSTR lpszTableName,const CnVariant &vFldNames,const CnVariant &vFldValues,const CnVariant &vUpdateFldNames,CnVariant &vUpdateData){
    ULONG   ulSts;

    // search the record
    CnVariant   vDummy;
    ULONG ulOption = CnPluTotalDb::s_NoGet;
    ulOption |= CnPluTotalDb::s_NoClose;
    ulSts = CnPluTotalDb::Find(lpszTableName,vFldNames,vFldValues,vDummy,ulOption);
    if(ulSts == PLUTOTAL_SUCCESS){
        // update!
        ulSts = CnPluTotalDb::Update(NULL,vUpdateFldNames,vUpdateData);
    }
    else{
        __pRecO->Close();
    }
    return  ulSts;
}


ULONG   CnPluTotalDb::Find(const LPCTSTR szTableName,const CnVariant &vFldNames,const CnVariant &vFldValues,CnVariant &vRecData,const ULONG ulOption){
    CString     strSqlCode;
    CString     strWhere;

    if(vFldNames.vt != VT_EMPTY && vFldValues.vt != VT_EMPTY){
        MakeSQLWhere(vFldNames,vFldValues,strWhere);
        strSqlCode.Format(_T("SELECT * FROM %s WHERE %s "), szTableName, (LPCTSTR)strWhere);
    }
    else
        strSqlCode.Format(_T("SELECT * FROM %s "), szTableName);
    CnVariant   vSqlCode(strSqlCode);


    if(ulOption & CnPluTotalDb::s_NoClose){
        // update or delete
	dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Find(1)- OpenRec(NO CLOSE)"),(LPCTSTR)strSqlCode);
#ifdef XP_PORT
	_bstr_t strConnect = ConnectionString;
    m_hr = __pRecO->OpenRec(vSqlCode, strConnect,adOpenKeyset,adLockOptimistic,adCmdText);
#else //WINCE
    m_hr = __pRecO->OpenRec(vSqlCode, m_vAC_DbFileName,adOpenKeyset,adLockOptimistic,adCmdText);
#endif
    }
    else{
        // read only
dbgtrc.PutLog(TEXT("###"),TEXT("CnPluTotalDb::Find(1) OpenRec(REOPN)"),(LPCTSTR)strSqlCode);
#ifdef XP_PORT
        m_hr = __pRecO->OpenRec(vSqlCode, CnVariant(activeConn),adOpenKeyset,adLockReadOnly,adCmdText);
#else
        m_hr = __pRecO->OpenRec(vSqlCode, m_vAC_DbFileName,adOpenKeyset,adLockReadOnly,adCmdText);
#endif
    }
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;

    LONG    lRecCnt;
    m_hr = __pRecO->get_RecordCount(&lRecCnt);
    if(FAILED(m_hr))
        return  PLUTOTAL_E_FAILURE;
    if(lRecCnt == 0L){
        __pRecO->Close();
        return PLUTOTAL_NOT_FOUND;
    }
    CnVariant   vFldNms(vRecData);

    if(!(ulOption & CnPluTotalDb::s_NoGet)){
        m_hr = __pRecO->GetRow(vFldNms,&vRecData);
        if(FAILED(m_hr))
            return  PLUTOTAL_E_FAILURE;
    }

    // close record set
    if(!(ulOption & CnPluTotalDb::s_NoClose))
        __pRecO->Close();

    return PLUTOTAL_SUCCESS;
}
#endif  // __DEL_061600

#ifdef  ___DEL_20000407
ULONG   CnPluTotalDb::OpenDbFile(const ULONG ulOpenMode){
    UINT    uMode = (UINT)ulOpenMode;


    uMode |= s_TypeBin;
    uMode |= s_ShareExclusive;
    if(m_fDbFile.Open((LPCTSTR)m_strDbFileName,uMode,&m_fError)){
        PutLog(TEXT("$$$ CnPluTotalDb::OpenDbFile"),(ULONG)0,(ULONG)PLUTOTAL_SUCCESS);
        return  PLUTOTAL_SUCCESS;
    }else{
        PutLog(TEXT("$$$ CnPluTotalDb::OpenDbFile"),(ULONG)0,(ULONG)PLUTOTAL_E_FAILURE);
        return  PLUTOTAL_E_FAILURE;
    }
}


ULONG   CnPluTotalDb::CloseDbFile(){
    m_fDbFile.Close();
    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::MoveDbFile(LPCTSTR lpszNewName){
    // drop dbfile
    CFile::Remove(lpszNewName);

    TRY{
        CFile::Rename((LPCTSTR)m_strDbFileName , lpszNewName );
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

    // regist new name
    m_strDbFileName.Empty();
    m_strDbFileName = lpszNewName;

    return  PLUTOTAL_SUCCESS;
}


ULONG   CnPluTotalDb::ReadDbFile(const ULONG ulOffset, const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes){
#if 1
	DWORD dwLength

    TRY{
        dwLength = m_fDbFile.getlength(ulOffset,CFile::begin);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH
    if (dwLength > ulOffset) {
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),*pulReadBytes,(ULONG)PLUTOTAL_EOF);
        return  PLUTOTAL_EOF;
	}
    TRY{
        dwLength = m_fDbFile.seek(ulOffset,CFile::begin);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH
#endif
    
    TRY{
        *pulReadBytes = m_fDbFile.Read(pBuf,(UINT)ulBytes);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

    // EOF?
    if(ulBytes != *pulReadBytes){
        PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),*pulReadBytes,(ULONG)PLUTOTAL_EOF);
        return  PLUTOTAL_EOF;
    }
    PutLog(TEXT("$$$ CnPluTotalDb::ReadDbFile"),*pulReadBytes,(ULONG)PLUTOTAL_SUCCESS);
    return  PLUTOTAL_SUCCESS;
}

ULONG   CnPluTotalDb::WriteDbFile(const ULONG ulOffset, const ULONG ulBytes,const VOID *pBuf){
#if 1
	DWORD dwLength

    TRY{
        dwLength = m_fDbFile.seek(ulOffset,CFile::begin);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,199,NULL);
        ::OutputDebugString(msg);
    }END_CATCH
#endif

    TRY{
        m_fDbFile.Write(pBuf,(UINT)ulBytes);
    }
    CATCH( CFileException, e ){
        TCHAR   msg[200];
        e->GetErrorMessage(msg,200,NULL);
        ::OutputDebugString(msg);
    }END_CATCH

    PutLog(TEXT("$$$ CnPluTotalDb::WriteDbFile"),ulBytes,(ULONG)PLUTOTAL_SUCCESS);
    return  PLUTOTAL_SUCCESS;
}

void    CnPluTotalDb::PutLog(LPCTSTR szLabel,const ULONG ulBytes,const ULONG ulSts){
#ifdef  _CNPLUTOTAL_DB_TRACE
    TCHAR   tBuf[512],fname[256];
    wsprintf(tBuf,TEXT("{ %ld bytes } (%ld)"),ulBytes,ulSts);
    wsprintf(fname,TEXT(" [%s] "),(LPCTSTR)m_strDbFileName);
    dbgtrc.PutLog(szLabel,fname,tBuf);
#endif
}

#endif  // ___DEL_20000407

#endif  // ___GOMI

