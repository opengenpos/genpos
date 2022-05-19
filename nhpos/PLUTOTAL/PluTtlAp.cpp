
#include    "stdAfx.h"

//  files included through other file includes
//#include  "PluTtlD.h"
//#include  "CnAdoCe.h"
//#include  "PluTtlDB.h"
//#include  "PluTtlM.h"

#include    "PluTtlAp.h"

int     CnPluTotalAPI::s_nObjCnt;               // object counter
const   int CnPluTotalAPI::c_nOBJMAX = 1;       // 

/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Open(PLUTOTAL_DB_HANDLE * phdDB)
*
*    Input:     
*
*   Output:     phdDB               - database handle
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS    - Successed the Opening PLUTotalDatabase
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/
//#define DELETE_INDEX    1
ULONG   CnPluTotalAPI::Open(PLUTOTAL_DB_HANDLE * phdDB, SHORT fPTD, SHORT fTmp){
	ULONG ulSts;

    if(s_nObjCnt <= c_nOBJMAX &&
        m_hdDb == PLUTOTAL_HANDLE_NULL &&
        m_hdDbFile == PLUTOTAL_HANDLE_NULL){

        // Create ADOCE PluTotal Object 
       ulSts =  m_pPluTotal.OpenDatabase(TRUE, fPTD, fTmp);	// 08/30/01
	   if(ulSts == PLUTOTAL_DB_ERROR_MSDEDOWN)
	   {
		 
		   return ulSts;
	   }
#if defined(DELETE_INDEX)
        m_pPluTotal.DropIndex(PLUTOTAL_ID_DAILY_CUR);
        m_pPluTotal.DropIndex(PLUTOTAL_ID_PTD_CUR);
        m_pPluTotal.CreateIndex(PLUTOTAL_ID_DAILY_CUR);
        m_pPluTotal.CreateIndex(PLUTOTAL_ID_PTD_CUR);
#endif
        m_hdDb = (PLUTOTAL_DB_HANDLE)m_nMyObjNo;
        *phdDB = m_hdDb;
        return  (ULONG)PLUTOTAL_SUCCESS;
    }
    return  (ULONG)PLUTOTAL_E_FAILURE;
}

/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Close(PLUTOTAL_DB_HANDLE hdDB)
*
*    Input:     hdDB                    - database handle
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_FAILURE
** Description  
*
*============================================================================
*/
ULONG   CnPluTotalAPI::Close(const PLUTOTAL_DB_HANDLE hdDB){
//  if(hdDB == m_hdDb){
    if(CheckDBHandle(hdDB) == TRUE){

        // Delete PluTotal Object
        m_pPluTotal.CloseDatabase();

        m_hdDb = PLUTOTAL_HANDLE_NULL;
        return  (ULONG)PLUTOTAL_SUCCESS;
    }
    return  (ULONG)PLUTOTAL_E_FAILURE;
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::GetRecordCnt(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,ULONG *pulRecCnt)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     pulRecCnt               - record number
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::GetRecordCnt(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG *pulRecCnt){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::GetRecordCnt(nTblID,(LONG *)pulRecCnt);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::CreateTable(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const ULONG ulRecNum)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               ulRecNum                - Max record number
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::CreateTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const ULONG ulRecNum){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CreateTable(nId,ulRecNum);
}


/*
*============================================================================
**Synopsis:     
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::DropTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.DropTable(nId);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::MoveTable(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nFromId,const SHORT nToId)
*
*    Input:     hdDB                    - database handle
*               nFromID                 - move from 
*               nToID                   - move to
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::MoveTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nFromId,const SHORT nToId){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::MoveTableN(nFromId,nToId);
}

ULONG   CnPluTotalAPI::ExportTableToFile (const PLUTOTAL_DB_HANDLE hdDB, const SHORT nFromId, const TCHAR *pFileName){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::ExportTableToFile(nFromId, pFileName);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::CheckTable(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS        - exists
*               PLUTOTAL_NOT_FOUND      - not exists
*               PLUTOTAL_E_ILLEAGAL     - 
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::CheckTable(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::CheckTable(nTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Insert(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nIblIDd,const PLUTOTAL_REC &recData)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               recData                 - record data
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::Insert(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const PLUTOTAL_REC &recData){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

//  return  m_pPluTotal.CnPluTotal::Insert(nId,recData);
    return  m_pPluTotal.CnPluTotal::InsertN(nId,recData);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Delete(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const UCHAR pPluNo[],const BYTE byAdjIdx)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               pPluNo                  - PLU number (13bytes)
*               byAdjIdx                - adjective index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::Delete(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const TCHAR pPluNo[],const BYTE byAdjIdx){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::DeleteN(nId,pPluNo,byAdjIdx);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Update(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const PLUTOTAL_REC &recData)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               recData                 - record data
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::Update(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const PLUTOTAL_REC &recData){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

//  return  m_pPluTotal.CnPluTotal::Update(nId,recData);
    return  m_pPluTotal.CnPluTotal::UpdateN(nId,recData);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::Find(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const UCHAR pPluNo[],const BYTE byAdjIdx,
*                       PLUTOTAL_REC * pRecData)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               pPluNo                  - PLU number (13bytes)
*               byAdjIdx                - adjective index
*
*   Output:     pRecData                - record data
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::Find(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC * pRecData){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

//  return  m_pPluTotal.CnPluTotal::Find(nId,pPluNo,byAdjIdx,pRecData);
    return  m_pPluTotal.CnPluTotal::FindN(nId,pPluNo,byAdjIdx,pRecData);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::dbg_ExecSQL(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const CHAR * szSqlCode)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               szSqlCode               - SQL statement
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  (for debugging)
*
*============================================================================
*/

ULONG   CnPluTotalAPI::dbg_ExecSQL(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,const CHAR * szSqlCode){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    // ASCII -> Unicode
    int     nLen = strlen(szSqlCode);
    LPTSTR  lpstrSql = new TCHAR [nLen+1];
    mbstowcs(lpstrSql,szSqlCode,nLen);
    lpstrSql[nLen] = (TCHAR)0;

    return  m_pPluTotal.CnPluTotal::dbg_ExecSQL(nId,(LPCTSTR)lpstrSql);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::SelectRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,PLUTOTAL_SERCH_COND SearchCond)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*               SearchCond              - (see PluTtlD.H)
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::SelectRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,ULONG SearchCond){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle

    return  m_pPluTotal.CnPluTotal::SelectRec(nId,SearchCond);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::FirstRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::FirstRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::FirstRec(nTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::LastRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::LastRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::LastRec(nTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::NextRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::NextRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::NextRec(nTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::PrevRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::PrevRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::PrevRec(nTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::GetRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nId,PLUTOTAL_REC *pRecData)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     pRecData                - record data
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::GetRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,PLUTOTAL_REC *pRecData){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::GetRec(nId,pRecData);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::GetNumRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,ULONG *pulRecNum)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     pulRecNum               - record number (in the recordset)
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::GetNumRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nId,ULONG *pulRecNum){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::GetNumRec(nId,pulRecNum);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::ReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblId)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::ReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblId){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::ReleaseRec(nTblId);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::GetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::GetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::GetStsInfo(nTblID,pStatusRec);
}

/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::RestoreDB(const PLUTOTAL_FILE_HANDLE phdDBFile,
																	 const SHORT nTblID)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/
ULONG   CnPluTotalAPI::RestoreDB(const PLUTOTAL_FILE_HANDLE phdDBFile, const SHORT nTblID)
{
	return m_pPluTotal.CnPluTotal::RestoreDB(nTblID);
}

/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::SetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,
*                       const SHORT nTblID,const PLUTOTAL_STATUS StatusRec)
*
*    Input:     hdDB                    - database handle
*               nTblID                  - table index
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::SetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    // not save status table data to db at each time, 08/18/01
	// now we will save the status table data each time, 10/26/2014  Richard Chambers
    return  m_pPluTotal.CnPluTotal::SetStsInfo(nTblID,StatusRec, TRUE);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::OpenDBFile(PLUTOTAL_FILE_HANDLE * phdDBFile,
*                       const SHORT nTblID,const ULONG ulOption)
*
*    Input:     nTblID                  - table index
*               ulOption                - 
*
*   Output:     phdDBFile               - database file handle
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::OpenDBFile(PLUTOTAL_FILE_HANDLE * phdDBFile,const SHORT nTblID,const ULONG ulOption){
    // check handles (DB & file)
    if(s_nObjCnt <= c_nOBJMAX &&
        m_hdDbFile == PLUTOTAL_HANDLE_NULL){

        // open file
        ULONG   ulSts = m_pPluTotal.CnPluTotal::OpenDBFile(nTblID,ulOption);
        if(ulSts == PLUTOTAL_SUCCESS){
            *phdDBFile = __DBFileHandle();
            m_hdDbFile = *phdDBFile;
            return  PLUTOTAL_SUCCESS;
        }
        return  ulSts;
    }
    return  (ULONG)PLUTOTAL_E_OPENED;
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::CloseDBFile(const PLUTOTAL_FILE_HANDLE hdDBFile)
*
*    Input:     hdDBFile                - file handle
*
*   Output:     
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::CloseDBFile(const PLUTOTAL_FILE_HANDLE hdDBFile){
    if(CheckDBFileHandle(hdDBFile)){
        // close file
        ULONG   ulSts = m_pPluTotal.CnPluTotal::CloseDBFile();
        if(ulSts == PLUTOTAL_SUCCESS){
            m_hdDbFile = PLUTOTAL_HANDLE_NULL;
            return  (ULONG)PLUTOTAL_SUCCESS;
        }
        return  ulSts;
    }
    return  (ULONG)PLUTOTAL_E_ILLEAGAL;
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::ReadDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile,
*                       const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes)
*
*    Input:     hdDBFile                - file handle
*               ulBYtes                 - 
*
*   Output:     pBuf                    - 
*               pulReadBytes            - 
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::ReadDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG ulOffset, const ULONG ulBytes,VOID *pBuf,ULONG *pulReadBytes){
    if(CheckDBFileHandle(hdDBFile)){
        // read
        return  m_pPluTotal.CnPluTotal::ReadDbFile(ulOffset, ulBytes,pBuf,pulReadBytes);
    }
	NHPOS_ASSERT_TEXT(!"CheckDBFileHandle Fail", "**ERROR** CnPluTotalAPI::ReadDbFile(): CheckDBFileHandle() failed.");
    return  (ULONG)PLUTOTAL_E_ILLEAGAL;
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::WriteDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile,
*                       const ULONG ulBytes,const VOID *pBuf)
*
*    Input:     hdDBFile                - file handle
*               ulBytes                 - 
*               pBuf                    - buffer
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  
*
*============================================================================
*/

ULONG   CnPluTotalAPI::WriteDbFile(const PLUTOTAL_FILE_HANDLE hdDBFile,const ULONG ulOffset, const ULONG ulBytes,const VOID *pBuf){
    if(CheckDBFileHandle(hdDBFile)){
        // write
        return  m_pPluTotal.CnPluTotal::WriteDbFile(ulOffset, ulBytes,pBuf);
    }
	NHPOS_ASSERT_TEXT(!"CheckDBFileHandle Fail", "**ERROR** CnPluTotalAPI::WriteDbFile(): CheckDBFileHandle() failed.");
    return  (ULONG)PLUTOTAL_E_ILLEAGAL;
}


ULONG   CnPluTotalAPI::DeleteDbFile(const SHORT nTblID){
    return  m_pPluTotal.CnPluTotal::DeleteDbFile(nTblID);
}

ULONG   CnPluTotalAPI::MoveDbFile(const SHORT nFromTblID, const SHORT nToTblID){
    return  m_pPluTotal.CnPluTotal::MoveDbFile(nFromTblID, nToTblID);
}

ULONG   CnPluTotalAPI::CopyDbFile(const SHORT nFromTblID, const SHORT nToTblID){
    return  m_pPluTotal.CnPluTotal::CopyDbFile(nFromTblID, nToTblID);
}


/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalAPI::StartBackup(void);
*               ULONG   CnPluTotalAPI::StartRestore(void);
*               ULONG   CnPluTotalAPI::GetBackupStatus(void);
*
*    Input:     Nothing
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      PLUTOTAL_SUCCESS
*               PLUTOTAL_E_ILLEAGAL
*               PLUTOTAL_E_FAILURE
*
** Description  Backup / Restore  Database files.
*
*============================================================================
*/
// ### ADD Saratoga (053100) 
ULONG   CnPluTotalAPI::StartBackup(void){
	
	//Memory Leak 7.8.05
	if(m_hdDbFile != PLUTOTAL_HANDLE_NULL)
        return  PLUTOTAL_E_OPENED;
    else
        return  m_pPluTotal.CnPluTotal::StartBackup();
}

// ### ADD Saratoga (060700) 
ULONG   CnPluTotalAPI::StartRestore(void){
    if(m_hdDb != PLUTOTAL_HANDLE_NULL ||
            m_hdDbFile != PLUTOTAL_HANDLE_NULL)
        return  PLUTOTAL_E_OPENED;
    else
        return  m_pPluTotal.CnPluTotal::StartRestore();
}

ULONG   CnPluTotalAPI::GetBackupStatus(void){
    return  m_pPluTotal.CnPluTotal::GetBackupStatus();
}


// *** DEBUG & TESTS (041500) ************************************************
ULONG   CnPluTotalAPI::__DbgMakeTestData(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const LONG lFrom,const LONG lTo,const LONG lStep){
    if(CheckDBHandle(hdDB) == FALSE)
        return  PLUTOTAL_E_ILLEAGAL;    // Illeagal DB handle
    return  m_pPluTotal.CnPluTotal::__DbgMakeTestData(nTblID,lFrom,lTo,lStep);
}

/* =========================================================================== */

